#!/usr/bin/python
# -*- coding:utf-8 -*-
import sys
import os
picdir = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'pic/2in9')
fontdir = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'pic')
libdir = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'lib')
if os.path.exists(libdir):
    sys.path.append(libdir)
    
from TP_lib import icnt86
from TP_lib import epd2in9_V2
from TP_lib import weather_2in9_V2

import time 
import logging
from PIL import Image, ImageDraw, ImageFont
import traceback
import threading

logging.basicConfig(level=logging.DEBUG)
flag_t = 1

def pthread_irq() :
    print("pthread irq running")
    while flag_t == 1 :
        if(tp.digital_read(tp.INT) == 0) :
            ICNT_Dev.Touch = 1
        else :
            ICNT_Dev.Touch = 0
        time.sleep(0.01)
    print("thread irq: exit")
    
def Show_Photo_Small(image, small):
    for t in range(1, 7):
        if(small*3+t > 9):
            newimage = Image.open(os.path.join(picdir, PhotoPath_S[0]))
            image.paste(newimage, ((t-1)%3*98+2, (t-1)//3*48+2))
        else:
            newimage = Image.open(os.path.join(picdir, PhotoPath_S[small*3+t]))
            image.paste(newimage, ((t-1)%3*98+2, (t-1)//3*48+2))

def Show_Photo_Large(image, large):
    if(large > 9):
        newimage = Image.open(os.path.join(picdir, PhotoPath_L[0]))
        image.paste(newimage, (2, 2))
    else:
        newimage = Image.open(os.path.join(picdir, PhotoPath_L[large]))
        image.paste(newimage, (2, 2))

def Read_BMP(File, x, y):
    newimage = Image.open(os.path.join(picdir, File))
    image.paste(newimage, (x, y))

def Draw_Time(image, x, y, font1, font2):
    Time = time.strftime("%H : %M", time.localtime())
    Date = time.strftime("%Y - %m - %d", time.localtime())
    imagefill=255
    if image.mode!="1":
        imagefill = (255, 255, 255, 255)
    image.text((x, y), Time, font = font1, fill = imagefill)
    image.text((x-9, y+35), Date, font = font2, fill = imagefill)


try:
    logging.info("epd2in9_V2 Touch Demo")
    
    epd = epd2in9_V2.EPD_2IN9_V2()

    

    tp = icnt86.INCT86()
    
    ICNT_Dev = icnt86.ICNT_Development()
    ICNT_Old = icnt86.ICNT_Development()
    
    '''
        Because the touch display requires a relatively fast refresh speed, the default 
        needs to use partial refresh, and four gray levels cannot be used in this mode. 
        Here, only four gray level picture refresh demonstration is used
    '''
    # epd.Init_4Gray()
    # Himage = Image.open(os.path.join(picdir, '2in9_Scale.bmp'))
    # epd.display_4Gray(epd.getbuffer_4Gray(Himage))
    # time.sleep(2)

    logging.info("init and Clear")
    epd.init()
    tp.ICNT_Init()
    epd.Clear(0xFF)

    t1 = threading.Thread(target = pthread_irq)
    t1.setDaemon(True)
    t1.start()
    
    # Drawing on the image
    font15 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 15)
    font24 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 24)
    
    image = Image.open(os.path.join(picdir, 'Menu.bmp'))
    DrawImage = ImageDraw.Draw(image)
    
    Draw_Time(DrawImage, 209, 40, font24, font15)
    
    epd.display_Base(epd.getbuffer(image))
    
    i = j = k = ReFlag = SelfFlag = Page = Photo_L = Photo_S = 0
    PhotoPath_S = [ "Photo_1_0.bmp",
                    "Photo_1_1.bmp", "Photo_1_2.bmp", "Photo_1_3.bmp", "Photo_1_4.bmp",
                    "Photo_1_5.bmp", "Photo_1_6.bmp", "Photo_1_7.bmp", "Photo_1_8.bmp",
                    "Photo_1_9.bmp",
                    ]
    PhotoPath_L = [ "Photo_2_0.bmp",
                    "Photo_2_1.bmp", "Photo_2_2.bmp", "Photo_2_3.bmp", "Photo_2_4.bmp",
                    "Photo_2_5.bmp", "Photo_2_6.bmp", "Photo_2_7.bmp", "Photo_2_8.bmp",
                    "Photo_2_9.bmp",
                    ]
    PagePath = ["Menu.bmp", "screen_output.png", "Photo_1.bmp", "Photo_2.bmp"]
    
    while(1):
        if(i > 20 or ReFlag == 1):
            if(Page == 0):
                DrawImage.rectangle((209, 40, 290, 120), fill = 0)
                Draw_Time(DrawImage, 209, 40, font24, font15)
                # print("*** Time Refresh ***\r\n")
        
            if(Page == 1):
                weather_2in9_V2.get_weather_png()
                Read_BMP(PagePath[Page], 0, 0)
        
            epd.display_Partial_Wait(epd.getbuffer(image))
            print("*** Touch Refresh ***\r\n")
            i = 0
            k = 0
            j += 1
            ReFlag = 0
        elif(k>50000 and i>0 and Page == 1):
            epd.display_Partial_Wait(epd.getbuffer(image))
            i = 0
            k = 0
            j += 1
            print("*** Overtime Refresh ***\r\n")
        elif(j > 50 or SelfFlag):
            SelfFlag = 0
            j = 0
            epd.init()
            epd.display_Base(epd.getbuffer(image))
            print("--- Self Refresh ---\r\n")
        else:
            k += 1

        if(Page==0 and k>5000000):
            ReFlag = 1

        tp.ICNT_Scan(ICNT_Dev, ICNT_Old)
        if(ICNT_Old.X[0] == ICNT_Dev.X[0] and ICNT_Old.Y[0] == ICNT_Dev.Y[0]):
            continue
        
        if(ICNT_Dev.TouchCount):
            ICNT_Dev.TouchCount = 0
            i += 1
            if(Page == 0  and ReFlag == 0):     #main menu
                if(ICNT_Dev.X[0] > 119 and ICNT_Dev.X[0] < 152 and ICNT_Dev.Y[0] > 31 and ICNT_Dev.Y[0] < 96):
                    print("Photo ...\r\n")
                    Page = 2
                    Read_BMP(PagePath[Page], 0, 0)
                    Show_Photo_Small(image, Photo_S)
                    ReFlag = 1
                elif(ICNT_Dev.X[0] > 39 and ICNT_Dev.X[0] < 80 and ICNT_Dev.Y[0] > 31 and ICNT_Dev.Y[0] < 96): 
                    print("Weather ...\r\n")
                    Page = 1
                    Read_BMP(PagePath[Page], 0, 0)
                    ReFlag = 1
                
            
            if(Page == 1 and ReFlag == 0):   #weather
                if(ICNT_Dev.X[0] > 136 and ICNT_Dev.X[0] < 159 and ICNT_Dev.Y[0] > 101 and ICNT_Dev.Y[0] < 124): 
                    print("Home ...\r\n")
                    Page = 0
                    Read_BMP(PagePath[Page], 0, 0)
                    ReFlag = 1
                elif(ICNT_Dev.X[0] > 5 and ICNT_Dev.X[0] < 27 and ICNT_Dev.Y[0] > 101 and ICNT_Dev.Y[0] < 124): 
                    print("Refresh ...\r\n")
                    SelfFlag = 1
                    ReFlag = 1
                
            
            if(Page == 2  and ReFlag == 0):  #photo menu
                if(ICNT_Dev.X[0] > 135 and ICNT_Dev.X[0] < 160 and ICNT_Dev.Y[0] > 101 and ICNT_Dev.Y[0] < 124): 
                    print("Home ...\r\n")
                    Page = 0
                    Read_BMP(PagePath[Page], 0, 0)
                    ReFlag = 1
                elif(ICNT_Dev.X[0] > 203 and ICNT_Dev.X[0] < 224 and ICNT_Dev.Y[0] > 101 and ICNT_Dev.Y[0] < 124): 
                    print("Next page ...\r\n")
                    Photo_S += 1
                    if(Photo_S > 2): # 9 photos is a maximum of three pages
                        Photo_S=0
                    ReFlag = 2
                elif(ICNT_Dev.X[0] > 71 and ICNT_Dev.X[0] < 92 and ICNT_Dev.Y[0] > 101 and ICNT_Dev.Y[0] < 124): 
                    print("Last page ...\r\n")
                    if(Photo_S == 0):
                        print("Top page ...\r\n")
                    else:
                        Photo_S -= 1
                        ReFlag = 2
                elif(ICNT_Dev.X[0] > 5 and ICNT_Dev.X[0] < 27 and ICNT_Dev.Y[0] > 101 and ICNT_Dev.Y[0] < 124): 
                    print("Refresh ...\r\n")
                    SelfFlag = 1
                    ReFlag = 1
                elif(ICNT_Dev.X[0] > 2 and ICNT_Dev.X[0] < 293 and ICNT_Dev.Y[0] > 2 and ICNT_Dev.Y[0] < 96 and ReFlag == 0):
                    print("Select photo ...\r\n")
                    Page = 3
                    Read_BMP(PagePath[Page], 0, 0)
                    Photo_L = ICNT_Dev.X[0]//96 + ICNT_Dev.Y[0]//48*3 + Photo_S*3 + 1
                    Show_Photo_Large(image, Photo_L)
                    ReFlag = 1
                if(ReFlag == 2):  # Refresh small photo
                    ReFlag = 1
                    Read_BMP(PagePath[Page], 0, 0)
                    Show_Photo_Small(image, Photo_S)   # show small photo
                
            
            if(Page == 3  and ReFlag == 0):     #view the photo
                if(ICNT_Dev.X[0] > 268 and ICNT_Dev.X[0] < 289 and ICNT_Dev.Y[0] > 101 and ICNT_Dev.Y[0] < 124): 
                    print("Photo menu ...\r\n")
                    Page = 2
                    Read_BMP(PagePath[Page], 0, 0)
                    Show_Photo_Small(image, Photo_S)
                    ReFlag = 1
                elif(ICNT_Dev.X[0] > 203 and ICNT_Dev.X[0] < 224 and ICNT_Dev.Y[0] > 101 and ICNT_Dev.Y[0] < 124): 
                    print("Next photo ...\r\n")
                    Photo_L += 1
                    if(Photo_L > 9):
                        Photo_L = 1
                    ReFlag = 2
                elif(ICNT_Dev.X[0] > 135 and ICNT_Dev.X[0] < 160 and ICNT_Dev.Y[0] > 101 and ICNT_Dev.Y[0] < 124): 
                    print("Home ...\r\n")
                    Page = 0
                    Read_BMP(PagePath[Page], 0, 0)
                    ReFlag = 1
                elif(ICNT_Dev.X[0] > 71 and ICNT_Dev.X[0] < 92 and ICNT_Dev.Y[0] > 101 and ICNT_Dev.Y[0] < 124): 
                    print("Last page ...\r\n")
                    if(Photo_L == 1):
                        print("Top photo ...\r\n")
                    else:
                        Photo_L -= 1
                        ReFlag = 2
                elif(ICNT_Dev.X[0] > 5 and ICNT_Dev.X[0] < 27 and ICNT_Dev.Y[0] > 101 and ICNT_Dev.Y[0] < 124): 
                    print("Refresh photo ...\r\n")
                    SelfFlag = 1
                    ReFlag = 1
                if(ReFlag == 2):    # Refresh large photo
                    ReFlag = 1
                    Show_Photo_Large(image, Photo_L)
                
                
except IOError as e:
    logging.info(e)
    
except KeyboardInterrupt:    
    logging.info("ctrl + c:")
    flag_t = 0
    epd.sleep()
    time.sleep(2)
    t1.join()
    epd.Dev_exit()
    exit()
