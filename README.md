# Embedded systems module semester project

## Task

Create a pressure measurement device that can measure 90-110 kPa range, can measure changes upto 10 Hz, and display it on a screen every 5 seconds and send it to PC every second.

## Hardware used

- Bluepill board
- LCD1602 with I2C
- BMP180

## Build requirements

- Arm GNU toolchain
- Newlib
- Make

  ### Alternatives

  You can also use other build configurations by changing Toolchain in STM32CubeMx using included .ioc file.

## Building project
Once you have all required dependencies install all you need is to run this in root directory:
```
make
```
