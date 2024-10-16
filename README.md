# OctoWS2811Controller

This project allows the control of an [OctoWS2811-enabled]([url](https://github.com/PaulStoffregen/OctoWS2811)) 
Teensy 4.1+ board from commands sent over a serial connection. The OctoWS2811 is an 8-channel LED controller that
makes it easy to connect to WS2811 LED arrays or strips.

The control syntax is very simple and is explained below. However, you can also use a pre-made implementation and
not worry about the syntax at all. One such implementaton is available in the form of a TouchDesigner component 
which can be found in the `OctoWS2811Controller.tox` file.

## Serial Commands

Here are the commands that can be sent over serial to the board.

#### Update LEDs

The command starts with `>` and ends with `\n`. In between are the RGB values for all the LEDs:

```
>12,23,34,0,0,255,\n
```

#### Update configuration

The command starts with `%` and ends with `\n`. In between are 3 comma-separated values for colour order,
speed and LEDs per channel:

```
%GBR,800,512\n
```
The command above would set the colour order to GBR, the speed to 800kHz and the number of LEDs per channel to
512 (those are the default values).

Available colour orders are: 

  - RGB
  - RBG
  - GRB
  - GBR
  - BRG
  - BGR
  - RGBW
  - RBGW
  - GRBW
  - GBRW
  - BRGW
  - BGRW
  - WRGB
  - WRBG
  - WGRB
  - WGBR
  - WBRG
  - WBGR
  - RWGB
  - RWBG
  - GWRB
  - GWBR
  - BWRG
  - BWGR
  - RGWB
  - RBWG
  - GRWB
  - GBWR
  - BRWG
  - BGWR

Available speeds are:

  - 400
  - 800
  - WS2813_800

The number of LEDs per channel must be an integer between 1 and 512.
