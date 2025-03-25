# OctoWS2811Controller

## Introduction

This project allows the control of an [OctoWS2811](https://github.com/PaulStoffregen/OctoWS2811)-enabled Teensy 
4.1+ board using commands sent over a serial connection. The OctoWS2811 is an 8-channel LED controller that makes
it easy to control WS2811 LED arrays or strips.

The control syntax is very simple and is explained below. However, you can also use a pre-made implementation and
not worry about the syntax at all. Current implementations:

  * TouchDesigner component: [OctoWS2811Controller.tox](https://github.com/djipco/OctoWS2811Controller/raw/refs/heads/main/OctoWS2811Controller.tox)

#### How many LEDs can be controlled?

Theoretically, this project allows you to control up to 768 addressable LEDs per channel, for a total of 6144 LEDs
(8 × 768). However, this depends on the speed of the serial connection. The Teensy uses a serial connection speed of 
either 12 or 480 Mbits/sec depending on the host it is connected to. So, depending on host speed, you may or may not
be able to reach the maximum of 6144 LEDs. 

## Installation

Use Arduino IDE to upload `OctoWS2811Controller.ino` to your Teensy 4.x device. Note that the code has only tested 
been with Teensy 4.1 (but I believe it should work with the 4.0).

Note that you will have to install the Teensy board in the Arduino IDE. To do so, follow these 
[instructions](https://www.pjrc.com/teensy/td_download.html). 

In TouchDesigner, simply drag and drop the `.tox` file to your project and enter the appropriate settings.

## OctoWS2811

Both OctoWS2811 RJ-45 ports are wired like this:

 1. Orange
 2. Blue
 3. Green
 4. Brown

## Serial Commands

Here are the commands that can be sent over serial to the board. You do not need to worry about the underlying 
syntax when using the TouchDesigner component (all of this is handled in the background).

#### Update LEDs

The command starts with `>` and ends with `\n`. In between are the RGB values for all the LEDs:

```
>12,23,34,0,0,255,\n
```
In the example above, we are updating 2 LEDs. The first one is set to R=12, G=23 and B=34. The second one is set
to R=0, G=0 and B=255. You should send as many RGB triplets as you have LEDs configured (max 768 per channel). If
you send less, the extra LEDs will simply remain as is.

#### Update configuration

The command starts with `%` and ends with `\n`. In between are 3 comma-separated values for colour order,
speed and LEDs per channel:

```
%GBR,800,512\n
```
The command above would set the colour order to GBR, the speed to 800kHz and the number of LEDs per channel to
512 (those are the default values).

Available colour orders are: 

  - **3 channels:** RGB, RBG, GRB, GBR, BRG, BGR

  - **4 channels:** RGBW, RBGW, GRBW, GBRW, BRGW, BGRW, WRGB, WRBG, WGRB, WGBR, WBRG, WBGR, RWGB, RWBG, GWRB, GWBR,
    BWRG, BWGR, RGWB, RBWG, GRWB, GBWR, BRWG, BGWR

Available speeds are:

  - 400
  - 800
  - WS2813_800

The number of LEDs per channel must be an integer between 1 and 768.

## Caveat

This is alpha software and might not be production ready. I'm having good success with it but your mileage may vary. 
Report issues if you find any. Cheers!
