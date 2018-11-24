# Tinker-Slider-v1.0

I have never seen a commercial slider. As a YouTuber, I wanted a Camera slider for taking b-rolls for my videos. So as a weekend project I started this project, and it took several weekends to complete. 

The following parts were used to build the single axis slider. 

1.M3 and M4 Machine Screw Box
2.Tripod Mount 1/4 Screw Adapter x 2
3.Tripod Mount Screw
4.Linear Motion Bearing x 4
5.Heat shrink tubes
6.Endstop modules x 2 
7.GT2 6mm, 2M timing belt
8.ESP32 NodeMcu Dev board
9.A4988 Stepper motor
10.Perf board
11.Male and Female Headers
12.SD1306 128x64 OLED display module
13.Belt tensioner spring
14.Rotary encoder
15.Linear, smooth rods 8mm, 500mm x 2
16.Wire Pipe 480mm
17.Linear Rail supports x 4


At the heart of the project sits the ESP32 microcontroller. Initially, I tried with ESP8266, but there were not enough pins, and then I tried with Arduino Pro Mini, but then the memory was minimal to use the LCD. 

The ESP32 runs Arduino Core. 

I used a 2 phase NEMA17 Hybrid motor that has a holding torque of 4000. The motor needs 1.7A per phase. It has a resolution of 1.8 degrees per step.  So to complete a single revolution, we need to step the motor 200 times. However, we need the resolution as low as 1 degree per step to be able to capture the super smooth moving time-lapse videos. 

The ESP32 Controls the stepper motor through the A4988 Stepper motor driver chip. This driver chip supports up to 1/16th  Microstepping mode which enables us to achieve rotational accuracy up to 0.0625degrees per step.

Awesome!

The SSD1306 0.96" OLED display module enables us to see an configure the display even under bright sunlight.

The Rotary Encoder can be used to precisely control the position of the motor and also as a selection tool for the menu. Several Modes like manual and automatic time-lapse, real-time control, tried and tested presets to shoot flowers, people, gadgets are supported.

Libraries required:
Accelstepper - https://github.com/adafruit/AccelStepper
Adafruit GFX - https://github.com/adafruit/Adafruit-GFX-Library
Adafruit SSD1306 - https://github.com/adafruit/Adafruit_SSD1306

References:
https://github.com/Wogified/Arduino-Motorized-Camera-Slider

To - Do :
WiFi Control by App / Custom Remote 
3 Axis Conversion 
CNC Milled Design 
