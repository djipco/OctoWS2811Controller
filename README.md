# OctoWS2811Controller
 
## Serial Commands

Here are the commands that can be sent over serial to the board.

#### Update LEDs

The command starts with `>` and ends with `\n`. In between are the RGB values for all the LEDs:

```
>12,23,34,0,0,255,\n
```

#### Update configuration

The command starts with `%` and ends with `\n`. In between 3 comma-separated values for colour order,
speed and LEDs per channel:

```
%GBR,800,512\n
```

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

  - 800
  - 400
  - WS2813_800

The number of LEDs per channel must be an interger between 1 and 512.
