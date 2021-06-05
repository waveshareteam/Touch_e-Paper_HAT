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
This file is to help you use this Demo.
A brief description of the use of this project is here:

1. Basic information:
This routine was verified on Raspbe Pi 4B using the XXXinch e-paper HAT module.
You can view the corresponding test routines in the \lib\Examples\ of the project.

2. Pin connection:
Pin connections can be viewed in \lib\epdconfig.py and will be repeated here:
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

3.Installation library
python2
    sudo apt-get update
    sudo apt-get install python-pip
    sudo apt-get install python-pil
    sudo apt-get install python-numpy
    sudo pip install RPi.GPIO
    sudo pip install spidev

python3
    sudo apt-get update
    sudo apt-get install python3-pip
    sudo apt-get install python3-pil
    sudo apt-get install python3-numpy
    sudo pip3 install RPi.GPIO
    sudo pip3 install spidev

4. Basic use:
Since this project is a comprehensive project, you may need to read the following for use:
You can view the test program in the examples\ directory.
Please note which ink screen you purchased.
Chestnut 1:
    If you purchased 2.9inch Touch e-Paper HAT, then you should execute the command:
        sudo python TP2in9_test.py
    or
        sudo python3 TP2in9_test.py

