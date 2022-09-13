/*
  u-blox ZED-F9P NEO-D9S Combo Board (SPX-20167) Test Procedure 
  By: Paul Clark
  SparkFun Electronics
  Date: September 2022
  
  Runs on SparkFun Thing Plus - ESP32 WROOM (USB-C) (WRL-20168)
                                  
  License: MIT
  Please see LICENSE.md for more details
  
*/


#define serialQUAD Serial // Use Serial for the Qwiic Universal Auto-Detect menus

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Use EEPROM for configuration storage

#include <EEPROM.h> // Include EEPROM.h to enable support for SFE_QUAD_Sensors__EEPROM. Do this before #include "SFE_QUAD_Sensors.h"

#include "SFE_QUAD_Sensors.h" // Click here to get the library:  http://librarymanager/All#SparkFun_Qwiic_Universal_Auto-Detect

SFE_QUAD_Sensors__EEPROM mySensors;

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Thing Plus C hardware specifics

int qwiicPower = 0; //Thing Plus C digital pin 0 is connected to the v-reg that controls the Qwiic power. It is also connected to the BOOT button.

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // The Thing Plus C STAT LED is connected to digital pin 13
#endif

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// ESP32 WiFi

#include <WiFi.h>
#include <HTTPClient.h>

WiFiClient myClient;

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const char assistNowServer[] = "https://online-live1.services.u-blox.com";
//const char assistNowServer[] = "https://online-live2.services.u-blox.com"; // Alternate server

const char getQuery[] = "GetOnlineData.ashx?";
const char tokenPrefix[] = "token=";
const char tokenSuffix[] = ";";
const char getGNSS[] = "gnss=gps,glo;"; // GNSS can be: gps,qzss,glo,bds,gal
const char getDataType[] = "datatype=eph,alm,aux;"; // Data type can be: eph,alm,aux,pos

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS; // ZED-F9P
SFE_UBLOX_GNSS myLBand; // NEO-D9S

//#define USE_MGA_ACKs // Uncomment this line to use the UBX_MGA_ACK_DATA0 acknowledgements

const uint32_t myLBandFreq = 1556290000; // Uncomment this line to use the US SPARTN 1.8 service
//const uint32_t myLBandFreq = 1545260000; // Uncomment this line to use the EU SPARTN 1.8 service

static volatile bool pmpDataSeen = false;

#define OK(ok) (ok ? F("  ->  OK") : F("  ->  ERROR!")) // Convert uint8_t into OK/ERROR

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Callback: printRXMPMP will be called when new PMP data arrives
// See u-blox_structs.h for the full definition of UBX_RXM_PMP_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setRXMPMPcallbackPtr
//        /               _____  This _must_ be UBX_RXM_PMP_data_t
//        |              /              _____ You can use any name you like for the struct
//        |              |             /
//        |              |             |
void printRXMPMP(UBX_RXM_PMP_data_t *pmpData)
{
  serialQUAD.println();
  
  serialQUAD.println(F("New PMP data received:"));

  serialQUAD.print(F("PMP message version: "));
  serialQUAD.println(pmpData->version);
  
  serialQUAD.print(F("numBytesUserData :   "));
  serialQUAD.println(pmpData->numBytesUserData);
  
  serialQUAD.print(F("serviceIdentifier:   "));
  serialQUAD.println(pmpData->serviceIdentifier);
  
  serialQUAD.print(F("uniqueWordBitErrors: "));
  serialQUAD.println(pmpData->uniqueWordBitErrors);
  
  serialQUAD.print(F("fecBits:             "));
  serialQUAD.println(pmpData->fecBits);
  
  serialQUAD.print(F("ebno:                "));
  serialQUAD.println(pmpData->ebno);

  serialQUAD.println();

  pmpDataSeen = true;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void setup()
{
  serialQUAD.begin(115200);
  delay(1000);

  // Enable power for the Qwiic bus
  // The input pull-up is enough to enable the regulator
  // We can then also use the BOOT button if we want to
  pinMode(qwiicPower, INPUT_PULLUP);

  // Flash LED_BUILTIN each time we test
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Detect, initialize and configure the sensors

  //mySensors.enableDebugging(serialQUAD); // Uncomment this line to enable debug messages on serialQUAD

  mySensors.setMenuPort(serialQUAD); // Use serialQUAD for the logging menu

  mySensors.theMenu.setMenuTimeout(60000); // Set the menu timeout to 60 seconds
  
  if (!mySensors.beginStorage()) // Begin EEPROM storage
    serialQUAD.println(F("beginStorage failed! You will not be able to read or write the sensor configuration..."));

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Create the menu

  // The Arduino IDE Serial Monitor does not support backspace so - by default - we need to clear text values when editing them.
  // The user enters the entire text value each time.
  // If the user is using a terminal emulator which supports backspace (e.g. Tera Term) then
  // setting supportsBackspace to true will allow the existing text to be edited.
  mySensors.theMenu.setMenuPort(serialQUAD, true); // Uncomment this line to use serialQUAD for the menu - on a terminal emulator which supports backspace

  while (mySensors.theMenu._menuPort->available()) // Clear the menu serial buffer
    mySensors.theMenu._menuPort->read();

  // Create the menu - using unique menu item names.
  // You can duplicate NONE or SUB_MENU_START items (e.g. "Wifi Menu") but all other items must be unique

  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("SparkFun ZED-F9P NEO-D9S Combo Board Test", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("=========================================", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Test a board", testTheBoard);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Connect to WiFi", wifiConnect);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Write the configuration to EEPROM", writeLoggerConfig);
  mySensors.theMenu.addMenuItem("Read the configuration from EEPROM", readLoggerConfig);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);

  // WiFi sub-menu: set the WiFi SSID and password
  mySensors.theMenu.addMenuItem("WiFi Menu", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START); // Start of the WiFi sub-menu
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("WiFi Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("=========", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("WiFi SSID", SFE_QUAD_MENU_VARIABLE_TYPE_TEXT);
  mySensors.theMenu.setMenuItemVariable("WiFi SSID", "sparkfun-guest"); // Set the default SSID - this will be updated by readLoggerConfig
  mySensors.theMenu.addMenuItem("WiFi password", SFE_QUAD_MENU_VARIABLE_TYPE_TEXT);
  mySensors.theMenu.setMenuItemVariable("WiFi password", "sparkfun6333"); // Set the default password - this will be updated by readLoggerConfig
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("Connect to WiFi", wifiConnect);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END); // End of the WiFi sub-menu
  
  // Thingstream PointPerfect AssistNow sub-menu: set the AssistNow Token
  mySensors.theMenu.addMenuItem("AssistNow Menu", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("AssistNow Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("============", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("AssistNow Token", SFE_QUAD_MENU_VARIABLE_TYPE_TEXT);
  mySensors.theMenu.setMenuItemVariable("AssistNow Token", "6AcdNZtqRhqwI1-6u7hX4g"); // Set the default AssistNow Token - this will be updated by readLoggerConfig
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END); // End of the sub-menu

  // Thingstream PointPerfect L-Band sub-menu: set the L-Band Key
  mySensors.theMenu.addMenuItem("PointPerfect L-Band Menu", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_START);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("PointPerfect L-Band Menu", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("============", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("L-Band Dynamic Key", SFE_QUAD_MENU_VARIABLE_TYPE_TEXT);
  mySensors.theMenu.setMenuItemVariable("L-Band Dynamic Key", "5002d290058c8c24ed00635144d53177"); // Set the default L-Band Token - this will be updated by readLoggerConfig
  mySensors.theMenu.addMenuItem("Valid From GPS Week", SFE_QUAD_MENU_VARIABLE_TYPE_UINT16_T);
  SFE_QUAD_Menu_Every_Type_t defaultValue;
  defaultValue.UINT16_T = 2224; // Valid from August 27th 2022. Expires: 23:59 (UTC) Sep 23, 2022
  mySensors.theMenu.setMenuItemVariable("Valid From GPS Week", &defaultValue); // Set the default valid from week no.
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_SUB_MENU_END); // End of the sub-menu

  
  mySensors.theMenu.addMenuItem("", SFE_QUAD_MENU_VARIABLE_TYPE_NONE);
  // End of the menu

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Read the configuration file, configure the sensors and update the menu variables. Do this _after_ the menu has been created.

  readLoggerConfig();

} // /setup()

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void loop()
{
  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Menu
  
//  if (mySensors.theMenu._menuPort->available()) // Has the user pressed a key?
//  {
    mySensors.theMenu.openMenu(); // If so, open the menu
//  }
} // /loop()


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Do the test
void testTheBoard(void)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    serialQUAD.println("You are not connected to WiFi! Please connect and then try again...");
    return;
  }

  char myAssistNowToken[33];
  mySensors.theMenu.getMenuItemVariable("AssistNow Token", myAssistNowToken, 33);

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Start I2C. Connect to the GNSS.

  Wire.begin(); //Start I2C

  if (myGNSS.begin() == false) //Connect to the Ublox module using Wire port
  {
    serialQUAD.println(F("u-blox ZED-F9P not detected. Please check wiring. Test Failed."));
    return;
  }
  
  serialQUAD.println(F("u-blox ZED-F9P connected"));

  myGNSS.setI2COutput(COM_TYPE_UBX); //Turn off NMEA noise
  myGNSS.setPortInput(COM_PORT_UART2, COM_TYPE_UBX | COM_TYPE_NMEA | COM_TYPE_SPARTN); //Be sure SPARTN input is enabled
  myGNSS.setDGNSSConfiguration(SFE_UBLOX_DGNSS_MODE_FIXED); // Set the differential mode - ambiguities are fixed whenever possible
  myGNSS.setVal8(UBLOX_CFG_SPARTN_USE_SOURCE, 1); // use LBAND PMP message

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Use HTTP GET to receive the AssistNow_Online data

  const int URL_BUFFER_SIZE  = 256;
  char theURL[URL_BUFFER_SIZE]; // This will contain the HTTP URL
  int payloadSize = 0; // This will be updated with the length of the data we get from the server
  String payload; // This will store the data we get from the server

  // Assemble the URL
  // Note the slash after the first %s (assistNowServer)
  snprintf(theURL, URL_BUFFER_SIZE, "%s/%s%s%s%s%s%s",
    assistNowServer,
    getQuery,
    tokenPrefix,
    myAssistNowToken,
    tokenSuffix,
    getGNSS,
    getDataType);

  HTTPClient http;

  http.begin(theURL);

  int httpCode = http.GET(); // HTTP GET

  // httpCode will be negative on error
  if(httpCode > 0)
  {
    // HTTP header has been sent and Server response header has been handled
    serialQUAD.printf("[HTTP] GET... code: %d\r\n", httpCode);
  
    // If the GET was successful, read the data
    if(httpCode == HTTP_CODE_OK) // Check for code 200
    {
      payloadSize = http.getSize();
      serialQUAD.printf("Server returned %d bytes\r\n", payloadSize);
      
      payload = http.getString(); // Get the payload
    }
  }
  else
  {
    serialQUAD.printf("[HTTP] GET... failed, error: %s\r\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();  
  
  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Push the AssistNow data to the module

  if (payloadSize > 0)
  {  
    // Uncomment the next line to enable the 'major' debug messages on Serial so you can see what AssistNow data is being sent
    //myGNSS.enableDebugging(Serial, true);

#ifndef USE_MGA_ACKs

    // ***** Don't use the UBX_MGA_ACK_DATA0 messages *****

    // Push all the AssistNow data. Don't use UBX_MGA_ACK_DATA0's. Use the default delay of 7ms between messages.
    myGNSS.pushAssistNowData(payload, (size_t)payloadSize);

#else

    // ***** Use the UBX_MGA_ACK_DATA0 messages *****

    // Tell the module to return UBX_MGA_ACK_DATA0 messages when we push the AssistNow data
    myGNSS.setAckAiding(1);

    // Speed things up by setting setI2CpollingWait to 1ms
    myGNSS.setI2CpollingWait(1);

    // Push all the AssistNow data.
    // We have called setAckAiding(1) to instruct the module to return MGA-ACK messages.
    // So, we could set the pushAssistNowData mgaAck parameter to SFE_UBLOX_MGA_ASSIST_ACK_YES.
    // But, just for giggles, let's use SFE_UBLOX_MGA_ASSIST_ACK_ENQUIRE just to confirm that the
    // MGA-ACK messages are actually enabled.
    // Wait for up to 100ms for each ACK to arrive! 100ms is a bit excessive... 7ms is nearer the mark.
    myGNSS.pushAssistNowData(payload, (size_t)payloadSize, SFE_UBLOX_MGA_ASSIST_ACK_ENQUIRE, 100);

    // Set setI2CpollingWait to 125ms to avoid pounding the I2C bus
    myGNSS.setI2CpollingWait(125);

#endif

  }

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Wait for up to 20 seconds for the fix type to be 3D

  serialQUAD.print(F("Waiting for a 3D fix"));
  
  unsigned long startTime = millis();
  byte fixType = 0;

  while ((fixType != 3) && (millis() < (startTime + 20000)))
  {
    delay(500);
    
    serialQUAD.print(F("."));

    fixType = myGNSS.getFixType();
  }

  serialQUAD.println();

  if (fixType != 3)
  {
    serialQUAD.println(F("Could not get a 3D fix. Test failed."));
    return;
  }

  serialQUAD.println(F("Got a 3D Fix!"));

  serialQUAD.println();

  serialQUAD.println(F("Please check that the TP LED is flashing. Please fail the board if not."));

  serialQUAD.println();

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Configure the SPARTN IP Dynamic Keys
  // "When the receiver boots, the host should send 'current' and 'next' keys in one message." - Use setDynamicSPARTNKeys for this.
  // "Every time the 'current' key is expired, 'next' takes its place."
  // "Therefore the host should then retrieve the new 'next' key and send only that." - Use setDynamicSPARTNKey for this.
  // The key can be provided in binary (uint8_t) format or in ASCII Hex (char) format, but in both cases keyLengthBytes _must_ represent the binary key length in bytes.

  char currentDynamicKey[33];
  mySensors.theMenu.getMenuItemVariable("L-Band Dynamic Key", currentDynamicKey, 33);

  SFE_QUAD_Menu_Every_Type_t currentKeyGPSWeek;
  mySensors.theMenu.getMenuItemVariable("Valid From GPS Week", &currentKeyGPSWeek); // Get the currentKeyGPSWeek

  const uint8_t currentKeyLengthBytes =   16; 
  const uint32_t currentKeyGPSToW =       518400;

  myGNSS.setDynamicSPARTNKey(currentKeyLengthBytes, currentKeyGPSWeek.UINT16_T, currentKeyGPSToW, currentDynamicKey);

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Begin and configure the NEO-D9S L-Band receiver

  //myLBand.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  while (myLBand.begin(Wire, 0x43) == false) //Connect to the u-blox NEO-D9S using Wire port. The D9S default I2C address is 0x43 (not 0x42)
  {
    serialQUAD.println(F("u-blox NEO-D9S not detected. Please check wiring. Test Failed."));
    return;
  }
  serialQUAD.println(F("u-blox NEO-D9S connected"));

  uint8_t ok = myLBand.setVal32(UBLOX_CFG_PMP_CENTER_FREQUENCY,   myLBandFreq); // Default 1539812500 Hz
  if (ok) ok = myLBand.setVal16(UBLOX_CFG_PMP_SEARCH_WINDOW,      2200);        // Default 2200 Hz
  if (ok) ok = myLBand.setVal8(UBLOX_CFG_PMP_USE_SERVICE_ID,      0);           // Default 1 
  if (ok) ok = myLBand.setVal16(UBLOX_CFG_PMP_SERVICE_ID,         21845);       // Default 50821
  if (ok) ok = myLBand.setVal16(UBLOX_CFG_PMP_DATA_RATE,          2400);        // Default 2400 bps
  if (ok) ok = myLBand.setVal8(UBLOX_CFG_PMP_USE_DESCRAMBLER,     1);           // Default 1
  if (ok) ok = myLBand.setVal16(UBLOX_CFG_PMP_DESCRAMBLER_INIT,   26969);       // Default 23560
  if (ok) ok = myLBand.setVal8(UBLOX_CFG_PMP_USE_PRESCRAMBLING,   0);           // Default 0
  if (ok) ok = myLBand.setVal64(UBLOX_CFG_PMP_UNIQUE_WORD,        16238547128276412563ull); 
  if (ok) ok = myLBand.setVal(UBLOX_CFG_MSGOUT_UBX_RXM_PMP_I2C,   1); // Ensure UBX-RXM-PMP is enabled on the I2C port 
  if (ok) ok = myLBand.setVal(UBLOX_CFG_MSGOUT_UBX_RXM_PMP_UART1, 1); // Output UBX-RXM-PMP on UART1
  if (ok) ok = myLBand.setVal(UBLOX_CFG_UART2OUTPROT_UBX, 1);         // Enable UBX output on UART2
  if (ok) ok = myLBand.setVal(UBLOX_CFG_MSGOUT_UBX_RXM_PMP_UART2, 1); // Output UBX-RXM-PMP on UART2
  if (ok) ok = myLBand.setVal32(UBLOX_CFG_UART1_BAUDRATE,         38400); // match baudrate with ZED default
  if (ok) ok = myLBand.setVal32(UBLOX_CFG_UART2_BAUDRATE,         38400); // match baudrate with ZED default

  serialQUAD.print(F("L-Band: configuration "));
  serialQUAD.println(OK(ok));

  myLBand.softwareResetGNSSOnly(); // Do a restart

  myLBand.setRXMPMPcallbackPtr(&printRXMPMP); // Call printRXMPMP when new PMP data arrives

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // Wait for up to 20 seconds PMP data to arrive

  serialQUAD.print(F("Waiting for PMP data"));
  
  startTime = millis();
  pmpDataSeen = false;

  while ((!pmpDataSeen) && (millis() < (startTime + 20000)))
  {
    serialQUAD.print(F("."));

    myLBand.checkUblox(); // Check for the arrival of new PMP data and process it.
    myLBand.checkCallbacks(); // Check if any LBand callbacks are waiting to be processed.

    if (!pmpDataSeen)
      delay(500);
  }

  serialQUAD.println();

  if (!pmpDataSeen)
  {
    serialQUAD.println(F("No PMP data received. Test failed."));
    return;
  }

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

  serialQUAD.println(F("If the L-Band Dynamic Key is valid, and you have a good L-Band signal, the NEO will be pushing corrections to the ZED."));
  serialQUAD.println();
  serialQUAD.println(F("If you see the RTK LED flash, you have a FLOAT solution. That's an excellent thing!"));
  serialQUAD.println();
  serialQUAD.println(F("If the RTK LED stays on continuously (on the x02 PCB) then you have a FIXED solution. That's a _really_ excellent thing!"));

  serialQUAD.println();
  serialQUAD.println();

  serialQUAD.println(F("Please measure the voltage of the back-up battery. Measure between the top of the battery and GND. It should be around 2.5V."));

  serialQUAD.println();
  serialQUAD.println();

  serialQUAD.println(F("If the battery voltage is OK - TEST PASSED!"));

  serialQUAD.println();

}
        
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Read the configuration, configure the sensors and update the menu variables
void readLoggerConfig(void)
{
  bool success = mySensors.readConfigurationFromStorage();
  success &= mySensors.applySensorAndMenuConfiguration();
  if (success)
    serialQUAD.println(F("Configuration read from EEPROM"));
  else
    serialQUAD.println(F("Unable to read configuration from EEPROM"));
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Write the logger configuration to EEPROM
void writeLoggerConfig(void)
{
  bool success = mySensors.getSensorAndMenuConfiguration();
  success &= mySensors.writeConfigurationToStorage(false); // Set append to false - overwrite the configuration
  if (success)
    serialQUAD.println(F("Configuration written to EEPROM"));
  else
    serialQUAD.println(F("Unable to write configuration to EEPROM"));
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Connect to WiFi
void wifiConnect(void)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    serialQUAD.println("WiFi is already connected! Disconnecting...");
    
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(5000);    
  }

  char ssid[32];
  mySensors.theMenu.getMenuItemVariable("WiFi SSID", ssid, 32);
  char password[32];
  mySensors.theMenu.getMenuItemVariable("WiFi password", password, 32);

  serialQUAD.printf("Connecting to %s ", ssid);

  WiFi.begin(ssid, password);

  const unsigned long wifiConnectTimeout = 20000; // Allow 10s for WiFi connection
  unsigned long startTime = millis();
  
  while ((WiFi.status() != WL_CONNECTED) && (millis() < (startTime + wifiConnectTimeout)))
  {
    delay(500);
    serialQUAD.print(".");
  }

  if (WiFi.status() == WL_CONNECTED)
    serialQUAD.println(" CONNECTED");
  else
    serialQUAD.println(" Connection failed! Please try again...");
}
