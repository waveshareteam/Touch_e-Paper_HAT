/*****************************************************************************
* | File      	:   Readme_CN.txt
* | Author      :   Waveshare team
* | Function    :   Help with use
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2021-06-05
* | Info        :   在这里提供一个中文版本的使用文档，以便你的快速使用
******************************************************************************/
这个文件是帮助您使用本例程。
在这里简略的描述本工程的使用：

1.基本信息：
本例程使用 xxxinch e-Paper HAT 模块 在 Raspbe Pi 4B 进行了验证，你可以在工程的examples\中查看对应的测试例程;

2.管脚连接：
管脚连接你可以在\lib\epdconfig.py中查看，这里也再重述一次：
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

3.安装库：
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

4.基本使用：
由于本工程是一个综合工程，对于使用而言，你可能需要阅读以下内容：
你可以在main.c中看到已经进行了注释的函数，
请注意你购买的是哪一款的墨水屏。
栗子1：
    如果你购买的 2.9inch Touch e-Paper HAT，那么你应该执行命令：
        sudo python TP2in9_test.py
    或
        sudo python3 TP2in9_test.py

