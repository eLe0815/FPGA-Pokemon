# FPGA-Pokemon
This project was developed by Ethan Le and Colin Sianturi and consists of building a system on chip (SoC) on the Nexys A7 100T FPGA board. The SoC itself uses the MicroBlaze Micro Controller System (MCS) and consists of a MMIO subsystem as well as a video subsystem. 

## System Diagram
[Diagram](https://github.com/eLe0815/FPGA-Pokemon/blob/main/Images/soc.PNG)

## MMIO Subsystem

The MMIO subsystem consists of a controller to select a specific slot and can accommodate up to 64 instantiated cores. After being “wrapped” with an interface circuit, custom digital logic can be plugged into the FPGA platform. For this project, we used a number said these cores to implement things such as UART, SPI, PS2 keyboard, GPIO, timers, PWM, and ADCs to connect outside components. Specifically for this project, the SPI core was used to detect tapping on the FPGA board, the PS2 core was used to connect keyboard inputs so that the user could select the desired actions, and the UART core was used mainly for debugging the system.

## Video Subsystem

This video subsystem handles various video processing tasks by chaining multiple cores, each responsible for a specific function. The frame counter coordinates the pixel processing, while the sync core ensures proper synchronization of the video signal. Each slot-specific core processes the video data in the daisy chain, contributing to the final output displayed on the screen.

## Results

[Image](https://github.com/eLe0815/FPGA-Pokemon/blob/main/Images/titlescreen.jpg)

[Image](https://github.com/eLe0815/FPGA-Pokemon/blob/main/Images/pokemongame.jpg)







