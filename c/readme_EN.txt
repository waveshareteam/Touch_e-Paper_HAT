/*****************************************************************************
* | File      	:   Readme_CN.txt
* | Author      :   Waveshare team
* | Function    :   Help with use
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2021-06-05
* | Info        :   Here is an English version of the documentation for your quick use.
******************************************************************************/
This file is to help you use this routine.
A brief description of the use of this project is here:

1. Basic information:
This routine was verified on Raspbe Pi 4B using the XXXinch e-paper HAT module.
You can view the corresponding test routines in the \lib\Examples\ of the project.

2. Pin connection:
Pin connection You can view it in DEV_Config.h in the \lib\Config\ directory, and repeat it here:
EPD    =>    Jetson Nano/RPI(BCM)
VCC    ->    3.3
GND    ->    GND
DIN    ->    10(SPI0_MOSI)
CLK    ->    11(SPI0_SCK)
CS     ->    8(SPI0_CS0)
DC     ->    25
ERST   ->    17
BUSY   ->    24
INT    ->    27
TRST   ->    22
SDA    ->    SDA1
SCL    ->    SCL1

3. Basic use:
Since this project is a comprehensive project, you may need to read the following for use:
You can see the functions that have been annotated of main.c.
Please note which ink screen you purchased.
Chestnut 1:
     If you purchased 2.9inch Touch e-Paper HAT, then you should remove the comment for the corresponding 6 lines of code, ie:
        // TestCode_2in9();
     changed to:
        TestCode_2in9();

Then you need to execute: 
make
compile the program, will generate the executable file: main
Run: 
sudo ./main
If you modify the program, you need to execute: 
make clear
then:
make

4. Directory structure (selection):
If you use our products frequently, we will be very familiar with our program directory structure. We have a copy of the specific function.
The API manual for the function, you can download it on our WIKI or request it as an after-sales customer service. Here is a brief introduction:
Config\: This directory is a hardware interface layer file. You can see many definitions in DEV_Config.c(.h), including:
   type of data;
    GPIO;
    Read and write GPIO;
    Delay: Note: This delay function does not use an oscilloscope to measure specific values.
    Module Init and exit processing:
        void DEV_Module_Init(void);
        void DEV_Module_Exit(void);
        Note: 1. Here is the processing of some GPIOs before and after using the ink screen.
              2. For the PCB with Rev2.1, the entire module will enter low power consumption after DEV_Module_Exit(). After testing, the power consumption is basically 0;
             
\lib\GUI\: This directory is some basic image processing functions, in GUI_Paint.c(.h):
    Common image processing: creating graphics, flipping graphics, mirroring graphics, setting pixels, clearing screens, etc.
    Common drawing processing: drawing points, lines, boxes, circles, Chinese characters, English characters, numbers, etc.;
    Common time display: Provide a common display time function;
    Commonly used display pictures: provide a function to display bitmaps;
    
\lib\Fonts\: for some commonly used fonts:
    Ascii:
        Font8: 5*8
        Font12: 7*12
        Font16: 11*16
        Font20: 14*20
        Font24: 17*24
    Chinese:
        font12CN: 16*21
        font24CN: 32*41
        
\lib\E-paper\: This screen is the ink screen driver function;
Examples\: This is the test program for the ink screen. You can see the specific usage method in it.