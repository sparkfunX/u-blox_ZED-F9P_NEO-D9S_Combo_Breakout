# Hardware Overview

The u-blox ZED-F9P NEO-D9S Combo Breakout is a sophisticated yet compact GNSS board. The following is a summary of its features and functions:

* **`ZED-F9P`** - The ZED-F9P is a top-of-the-line module for high accuracy GNSS and GPS location solutions
    * Concurrent reception of GPS, GLONASS, Galileo and BeiDou
    * Multi-band RTK with fast convergence times and reliable performance
    * Centimeter-level accuracy in a small and energy-efficient module
* **`NEO-D9S`** - The NEO-D9S is a satellite data receiver for L-band correction broadcast
    * The NEO-D9S can be configured for use with a variety of L-band correction services including u-blox's PointPerfect service
    * It decodes the satellite transmission and outputs a correction stream directly to the ZED-F9P via UART2
* **`UART2 TX and RX`** - The ZED-F9P and NEO-D9S UART2 TX and RX are interconnected (crossed-over) to allow the NEO to feed corrections directly to the ZED
    * The UART2 pins can be isolated if required by opening the **TX2** and **RX2** jumpers
* **`Antenna Power Divider`** - The breakout has an on-board power divider (antenna splitter) to divide the satellite signals between the ZED and the NEO
    * The connector is SMA female (standard polarity)
    * 3.3V power is provided for an active antenna
* **`Dual USB-C connections`** - One provides a direct connection to the ZED-F9P; the seconds provides a direct connection to the NEO-D9S
    * The board can be powered via either USB-C connector, or both
    * There is no on-board USB hub. Both ports can be connected to the same computer, or different computers if desired
* **`LED Indicators`** - The breakout has four LED indicators:
    * Power
    * RTK: Off = No Carrier Solution; Flashing = Floating Solution; On = Fixed Solution
    * GEO: Geofence indication
    * TP: Timing Pulse (Pulse Per Second)
    * Each LED can be disabled by opening the corresponding split pad jumper on the bottom of the breakout
* **`Dual Qwiic Connectors`** - The breakout has two Qwiic (I<sup>2</sup>C) connectors allowing the breakout to be daisy-chained
* **`Back-up Battery`** - A small rechargeable coin cell maintains the ZED-F9P battery-backed RAM while the power is disconnected
* **`Full set of breakout pads`** - The breakout pads provide full access to the ZED-F9P and NEO-D9S pins 
    * I<sup>2</sup>C
    * UART1
    * UART2
    * External Interrupt
    * Reset
    * Safeboot
    * ZED-F9P: TX_RDY, TP, GEO, RTK
    * Power: 5V and 3.3V

## Board Dimensions

The board dimensions are illustrated in the drawing below. The listed measurements are in inches. The four mounting holes are compatible with 4-40 UNC or M3.0 screws.

<center>
[![Board Dimensions](./board_files/Dimensions.png){ width="200" }](./board_files/Dimensions.png)<br>
*Board dimensions. (Click to enlarge)*
</center>

## Left Edge Breakout Pads

With the SMA connector oriented to the 'top', the breakout pads down the left side of the board are:

<table style="font-size:12.25px; width:100%">
    <tr>
        <th style="text-align:center; vertical-align:middle; min-width:1rem;" width="20.0%">
            Pad
        </th>
        <th style="text-align:center; vertical-align:middle; min-width:4rem;" width="80.0%">
            Function
        </th>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>ANT PWR</b></td>
        <td style="vertical-align:middle" align="center">External antenna power<br>Cut the <b>ANT_VCC<b> trace<br>and install L1 to enable</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>EXT INT NEO</b></td>
        <td style="vertical-align:middle" align="center">Connected to the<br>NEO-D9S EXT_INT pin</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>RX1</b></td>
        <td style="vertical-align:middle" align="center">NEO-D9S UART1 RX Pin</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>TX1</b></td>
        <td style="vertical-align:middle" align="center">NEO-D9S UART1 TX Pin</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>RX2</b></td>
        <td style="vertical-align:middle" align="center">NEO-D9S UART2 RX Pin<br>Connected to ZED-F9P TX2<br>(Open the **TX2** jumper to isolate)</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>TX2</b></td>
        <td style="vertical-align:middle" align="center">NEO-D9S UART2 TX Pin<br>Connected to ZED-F9P RX2<br>(Open the **RX2** jumper to isolate)</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>GND</b></td>
        <td style="vertical-align:middle" align="center">Power Ground (0V)</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>3V3</b></td>
        <td style="vertical-align:middle" align="center">3.3V Supply rail<br>Output of regulator U2<br>(Can be used as a power input)</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>SDA</b></td>
        <td style="vertical-align:middle" align="center">I<sup>2</sup>C Data<br>Common to both ZED and NEO</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>SCL</b></td>
        <td style="vertical-align:middle" align="center">I<sup>2</sup>C Clock<br>Common to both ZED and NEO</td>
    </tr>
</table>

*For more details, users can reference the [schematic](./board_files/Schematic.pdf).*

## Right Edge Breakout Pads

With the SMA connector oriented to the 'top', the breakout pads down the right side of the board are:

<table style="font-size:12.25px; width:100%">
    <tr>
        <th style="text-align:center; vertical-align:middle; min-width:1rem;" width="20.0%">
            Pad
        </th>
        <th style="text-align:center; vertical-align:middle; min-width:4rem;" width="80.0%">
            Function
        </th>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>RESET</b></td>
        <td style="vertical-align:middle" align="center">Reset<br>Active low<br>Common to both ZED and NEO</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>EXT INT</b></td>
        <td style="vertical-align:middle" align="center">Connected to the<br>ZED-F9P EXT_INT pin</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>TX1</b></td>
        <td style="vertical-align:middle" align="center">ZED-F9P UART1 TX Pin</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>RX1</b></td>
        <td style="vertical-align:middle" align="center">ZED-F9P UART1 RX Pin</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>TX2</b></td>
        <td style="vertical-align:middle" align="center">ZED-F9P UART2 TX Pin<br>Connected to NEO-D9S RX2<br>(Open the **TX2** jumper to isolate)</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>RX2</b></td>
        <td style="vertical-align:middle" align="center">ZED-F9P UART2 RX Pin<br>Connected to NEO-D9S TX2<br>(Open the **RX2** jumper to isolate)</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>SCL</b></td>
        <td style="vertical-align:middle" align="center">I<sup>2</sup>C Clock<br>Common to both ZED and NEO</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>SDA</b></td>
        <td style="vertical-align:middle" align="center">I<sup>2</sup>C Data<br>Common to both ZED and NEO</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>3V3</b></td>
        <td style="vertical-align:middle" align="center">3.3V Supply rail<br>Output of regulator U2<br>(Can be used as a power input)</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>GND</b></td>
        <td style="vertical-align:middle" align="center">Power Ground (0V)</td>
    </tr>
</table>

*For more details, users can reference the [schematic](./board_files/Schematic.pdf).*

## Right Edge Breakout Pads

With the SMA connector oriented to the 'top', the breakout pads along the bottom edge of the board are:

<table style="font-size:12.25px; width:100%">
    <tr>
        <th style="text-align:center; vertical-align:middle; min-width:1rem;" width="20.0%">
            Pad
        </th>
        <th style="text-align:center; vertical-align:middle; min-width:4rem;" width="80.0%">
            Function
        </th>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>TX RDY</b></td>
        <td style="vertical-align:middle" align="center">Indicates the ZED-F9P has data<br>waiting in its I<sup>2</sup>C TX buffer</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>TP</b></td>
        <td style="vertical-align:middle" align="center">ZED-F9P Timing Pulse<br>(Pulse Per Second)</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>GEO</b></td>
        <td style="vertical-align:middle" align="center">ZED-F9P Geofence indicator</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>RTK</b></td>
        <td style="vertical-align:middle" align="center">ZED-F9P RTK indication<br>High = No Carrier Solution<br>Square Wave = Floating Solution<br>Low = Fixed Solution</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>5V</b></td>
        <td style="vertical-align:middle" align="center">5V Power Rail<br>Can be used as a power input<br>or output (from USB)<br>(Diode protected)</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>3V3</b></td>
        <td style="vertical-align:middle" align="center">3.3V Supply rail<br>Output of regulator U2<br>(Can be used as a power input)</td>
    </tr>
    <tr>
        <td style="vertical-align:middle" align="center"><b>GND</b></td>
        <td style="vertical-align:middle" align="center">Power Ground (0V)</td>
    </tr>
</table>

*For more details, users can reference the [schematic](./board_files/Schematic.pdf).*

## Jumpers

!!! note
    <p>Never modified a jumper before? Check out our <a href="https://learn.sparkfun.com/tutorials/664">Jumper Pads and PCB Traces tutorial</a> for a quick introduction!</p>
    <p align="center">
        <a href="https://learn.sparkfun.com/tutorials/664">How to Work with Jumper Pads and PCB Traces<br>
        <img src="https://cdn.sparkfun.com/c/264-148/assets/learn_tutorials/6/6/4/PCB_TraceCutLumenati.jpg"></a>
    </p>

There are multiple jumpers on the back of the board that can be used to modify how the board functions.

<center>
[![Jumpers](./img/hookup_guide/jumpers.png){ width="200" }](./img/hookup_guide/jumpers.png)<br>
*The jumpers on the back of the function board. (Click to enlarge)*
</center>

* **TX2** - Open this jumper to isolate NEO-D9S RX2 from ZED-F9P TX2
* **RX2** - Open this jumper to isolate NEO-D9S TX2 from ZED-F9P RX2
    * By default, the NEO-D9S PMP (SPARTN) correction data stream is output directly from NEO-D9S TX2 to the ZED-F9P RX2
    * Opening this jumper will break that connection
    * Correction data can be read from the NEO-D9S and pushed to the ZED-F9P via I<sup>2</sup>C instead - if desired
* **I<sup>2</sup>C** - Close this double jumper to connect additional pull-up resistors to I<sup>2</sup>C **SCL** and **SDA**. Not recommended
* **SHLD** - The two **SHLD** jumpers can be opened to isolate the USB-C connector shields from **GND**. Not recommended, but may help prevent EMI
* **PWR** - Open this jumper to disable the Power LED
* **RTK** - Open this jumper to disable the RTK LED
* **GEO** - Open this jumper to disable the GEO LED
* **TP** - Open this jumper to disable the TP (PPS) LED

## ANT PWR

By default, 3.3V active antenna power is provided on the SMA connector via the NEO-D9S **VCC_RF** output pin.
Power is delivered via a 10 Ohm resistor, to limit the current in case the antenna cable becomes shorted.

If your antenna requires a higher voltage (or current):

* Isolate **VCC_RF** by cutting the trace marked by the arrow symbols on the top of the PCB

<center>
[![Jumpers](./img/hookup_guide/VCC_RF.png){ width="200" }](./img/hookup_guide/VCC_RF.png)<br>
*The VCC_RF trace. (Click to enlarge)*
</center>

* Install inductor L1 - using a 0603 inductor with impedance >500 Ohms at 1.5 GHz

<center>
[![Jumpers](./img/hookup_guide/L1.png){ width="200" }](./img/hookup_guide/L1.png)<br>
*The position of L1. (Click to enlarge)*
</center>

* Inject a **clean** DC supply voltage between the **ANT PWR** breakout pad and **GND**

## SAFEBOOT

It _may_ be necessary to place the ZED-F9P or NEO-D9S into a "safe boot" state before applying a firmware upgrade.

If this becomes necessary:

* Disconnect power to the breakout board
* For the ZED-F9P: connect a jumper wire between the **SAFEBOOT ZED** test point and **GND**
* Reconnect the power
* Disconnect the jumper
* Upgrade the firmware using u-center

Repeat for the NEO-D9S using the **SAFEBOOT NEO** test point.

<center>
[![Jumpers](./img/hookup_guide/safeboot.png){ width="200" }](./img/hookup_guide/safeboot.png)<br>
*The two SAFEBOOT test points. (Click to enlarge)*
</center>

## LED Indicators

The breakout has four LED indicators:

* Power
* RTK: Off = No Carrier Solution; Flashing = Floating Solution; On = Fixed Solution
* GEO: Geofence indication
* TP: Timing Pulse (Pulse Per Second)

Each LED can be disabled by opening the corresponding split pad jumper on the bottom of the breakout

## Back-up Battery

A small rechargeable coin cell maintains the ZED-F9P battery-backed RAM while the power is disconnected. It maintains the ZED's GNSS Almanac and Real Time Clock.

The battery is recharged slowly whenever 3.3V power is present.

If the battery becomes discharged, connect 3.3V power for 48 hours to completely recharge the battery.

## Power Supply and I/O voltage

Both the ZED-F9P and NEO-D9S are 3.3V devices. They operate from 3.3V and all of their I/O pins are 3.3V.

!!! attention
    Do not drive any of the ZED-F9P or NEO-D9S I/O pins using 5V signals. You may damage either or both modules if you do. Use a suitable level-shifter if you want to interface to a 5V developemnt board.

The board has a 3.3V regulator on-board to regulate 5V USB-C power (if present) down to 3.3V.

The options for providing power to the board are as follows:

* 5V power via one or both USB-C ports
    * Diode protection is provided
* 5V power via the **5V** breakout pin
    * Diode protection prevents the USB-C ports from being reverse-fed
* 3.3V power via:
    * Either of the Qwiic connectors
    * Any of the **3V3** breakout pins

## Current Draw

Worst-case current draw of the ZED-F9P and NEO-D9S combined is 260mA at 3.3V.

Typical current draw of the ZED-F9P and NEO-D9S combined is 100mA.

(These figures exclude any current drawn by an active antenna.)
