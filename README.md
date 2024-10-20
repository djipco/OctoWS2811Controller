# OctoWS2811Controller

This project allows the control of an [OctoWS2811](https://github.com/PaulStoffregen/OctoWS2811)-enabled
Teensy 4.1+ board from commands sent over a serial connection. The OctoWS2811 is an 8-channel LED controller that
makes it easy to control WS2811 LED arrays or strips.

The control syntax is very simple and is explained below. However, you can also use a pre-made implementation and
not worry about the syntax at all. Current implementations:

  * TouchDesigner component: [OctoWS2811Controller.tox](https://github.com/djipco/OctoWS2811Controller/raw/refs/heads/main/OctoWS2811Controller.tox)

## Installation

Use Arduino IDE to upload OctoWS2811Controller.ino to your Teensy 4.x device. Note that the code has only tested 
been with Teensy 4.1 (but I believe it should work with the 4.0).

## Serial Commands

Here are the commands that can be sent over serial to the board.

#### Update LEDs

The command starts with `>` and ends with `\n`. In between are the RGB values for all the LEDs:

```
>12,23,34,0,0,255,\n
```
In the example above, we are updating 2 LEDs. The first one is set to R=12, G=23 and B=34. The second one is set
to R=0, G=0 and B=255. You should send as many RGB triplets as you have LEDs configured (max 512 per channel). If
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

  - RGB
  - RBG
  - GRB
  - GBR
  - BRG
  - BGR

Available speeds are:

  - 400
  - 800
  - WS2813_800

The number of LEDs per channel must be an integer between 1 and 512.

## Caveat

This is alpha software and might not be production ready. I'm having good success with it but your mileage may vary. 
Report issues if you find any. Cheers!
