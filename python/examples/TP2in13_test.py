#!/usr/bin/python
# -*- coding:utf-8 -*-
import sys
import os
picdir = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'pic/2in13')
fontdir = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'pic')
libdir = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'lib')
if os.path.exists(libdir):
    sys.path.append(libdir)
    
from TP_lib import gt1151
from TP_lib import epd2in13_V2
import time
import logging
from PIL import Image,ImageDraw,ImageFont
import traceback
import threading

logging.basicConfig(level=logging.DEBUG)
flag_t = 1

def pthread_irq() :
    print("pthread running")
    while flag_t == 1 :
        if(gt.digital_read(gt.INT) == 0) :
            GT_Dev.Touch = 1
        else :
            GT_Dev.Touch = 0
    print("thread:exit")

def Show_Photo_Small(image, small):
    for t in range(1, 5):
        if(small*2+t > 6):
            newimage = Image.open(os.path.join(picdir, PhotoPath_S[0]))
            image.paste(newimage, ((t-1)//2*45+2, (t%2)*124+2))
        else:
            newimage = Image.open(os.path.join(picdir, PhotoPath_S[small*2+t]))
            image.paste(newimage, ((t-1)//2*45+2, (t%2)*124+2))

def Show_Photo_Large(image, large):
    if(large > 6):
        newimage = Image.open(os.path.join(picdir, PhotoPath_L[0]))
        image.paste(newimage, (2, 2))
    else:
        newimage = Image.open(os.path.join(picdir, PhotoPath_L[large]))
        image.paste(newimage, (2, 2))

def Read_BMP(File, x, y):
    newimage = Image.open(os.path.join(picdir, File))
    image.paste(newimage, (x, y))

try:
    logging.info("epd2in13_V2 Touch Demo")
    
    epd = epd2in13_V2.EPD_2IN13_V2()
    gt = gt1151.GT1151()
    GT_Dev = gt1151.GT_Development()
    GT_Old = gt1151.GT_Development()
    
    logging.info("init and Clear")
    epd.init(epd.FULL_UPDATE)
    gt.GT_Init()
    epd.Clear(0xFF)

    t = threading.Thread(target = pthread_irq)
    t.setDaemon(True)
    t.start()

    # Drawing on the image
    font15 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 15)
    font24 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 24)
    
    image = Image.open(os.path.join(picdir, 'Menu.bmp'))
    epd.displayPartBaseImage(epd.getbuffer(image))
    DrawImage = ImageDraw.Draw(image)
    epd.init(epd.PART_UPDATE)
    
    i = j = k = ReFlag = SelfFlag = Page = Photo_L = Photo_S = 0
    PhotoPath_S = [ "Photo_1_0.bmp",
                    "Photo_1_1.bmp", "Photo_1_2.bmp", "Photo_1_3.bmp", "Photo_1_4.bmp",
                    "Photo_1_5.bmp", "Photo_1_6.bmp",
                    ]
    PhotoPath_L = [ "Photo_2_0.bmp",
                    "Photo_2_1.bmp", "Photo_2_2.bmp", "Photo_2_3.bmp", "Photo_2_4.bmp",
                    "Photo_2_5.bmp", "Photo_2_6.bmp",
                    ]
    PagePath = ["Menu.bmp", "White_board.bmp", "Photo_1.bmp", "Photo_2.bmp"]
    
    while(1):
        if(i > 12 or ReFlag == 1):
            if(Page == 1 and SelfFlag == 0):
                epd.displayPartial(epd.getbuffer(image))
            else:
                epd.displayPartial_Wait(epd.getbuffer(image))
            i = 0
            k = 0
            j += 1
            ReFlag = 0
            print("*** Draw Refresh ***\r\n")
        elif(k>50000 and i>0 and Page == 1):
            epd.displayPartial(epd.getbuffer(image))
            i = 0
            k = 0
            j += 1
            print("*** Overtime Refresh ***\r\n")
        elif(j > 50 or SelfFlag):
            SelfFlag = 0
            j = 0
            epd.init(epd.FULL_UPDATE)
            epd.displayPartBaseImage(epd.getbuffer(image))
            epd.init(epd.PART_UPDATE)
            print("--- Self Refresh ---\r\n")
        else:
            k += 1
        # Read the touch input
        gt.GT_Scan(GT_Dev, GT_Old)
        if(GT_Old.X[0] == GT_Dev.X[0] and GT_Old.Y[0] == GT_Dev.Y[0] and GT_Old.S[0] == GT_Dev.S[0]):
            continue
        
        if(GT_Dev.TouchpointFlag):
            i += 1
            GT_Dev.TouchpointFlag = 0

            if(Page == 0  and ReFlag == 0):     #main menu
                if(GT_Dev.X[0] > 29 and GT_Dev.X[0] < 92 and GT_Dev.Y[0] > 56 and GT_Dev.Y[0] < 95):
                    print("Photo ...\r\n")
                    Page = 2
                    Read_BMP(PagePath[Page], 0, 0)
                    Show_Photo_Small(image, Photo_S)
                    ReFlag = 1
                elif(GT_Dev.X[0] > 29 and GT_Dev.X[0] < 92 and GT_Dev.Y[0] > 153 and GT_Dev.Y[0] < 193): 
                    print("Draw ...\r\n")
                    Page = 1
                    Read_BMP(PagePath[Page], 0, 0)
                    ReFlag = 1
                
            
            if(Page == 1 and ReFlag == 0):   #white board
                DrawImage.rectangle([(GT_Dev.X[0], GT_Dev.Y[0]), (GT_Dev.X[0] + GT_Dev.S[0]/8 + 1, GT_Dev.Y[0] + GT_Dev.S[0]/8 + 1)], fill=0)
                if(GT_Dev.X[0] > 96 and GT_Dev.X[0] < 118 and GT_Dev.Y[0] > 6 and GT_Dev.Y[0] < 30): 
                    print("Home ...\r\n")
                    Page = 1
                    Read_BMP(PagePath[Page], 0, 0)
                    ReFlag = 1
                elif(GT_Dev.X[0] > 96 and GT_Dev.X[0] < 118 and GT_Dev.Y[0] > 113 and GT_Dev.Y[0] < 136): 
                    print("Clear ...\r\n")
                    Page = 0
                    Read_BMP(PagePath[Page], 0, 0)
                    ReFlag = 1
                elif(GT_Dev.X[0] > 96 and GT_Dev.X[0] < 118 and GT_Dev.Y[0] > 220 and GT_Dev.Y[0] < 242): 
                    print("Refresh ...\r\n")
                    SelfFlag = 1
                    ReFlag = 1
                
            
            if(Page == 2  and ReFlag == 0):  #photo menu
                if(GT_Dev.X[0] > 97 and GT_Dev.X[0] < 119 and GT_Dev.Y[0] > 113 and GT_Dev.Y[0] < 136): 
                    print("Home ...\r\n")
                    Page = 0
                    Read_BMP(PagePath[Page], 0, 0)
                    ReFlag = 1
                elif(GT_Dev.X[0] > 97 and GT_Dev.X[0] < 119 and GT_Dev.Y[0] > 57 and GT_Dev.Y[0] < 78): 
                    print("Next page ...\r\n")
                    Photo_S += 1
                    if(Photo_S > 2): # 6 photos is a maximum of three pages
                        Photo_S=0
                    ReFlag = 2
                elif(GT_Dev.X[0] > 97 and GT_Dev.X[0] < 119 and GT_Dev.Y[0] > 169 and GT_Dev.Y[0] < 190): 
                    print("Last page ...\r\n")
                    if(Photo_S == 0):
                        print("Top page ...\r\n")
                    else:
                        Photo_S -= 1
                        ReFlag = 2
                elif(GT_Dev.X[0] > 97 and GT_Dev.X[0] < 119 and GT_Dev.Y[0] > 220 and GT_Dev.Y[0] < 242): 
                    print("Refresh ...\r\n")
                    SelfFlag = 1
                    ReFlag = 1
                elif(GT_Dev.X[0] > 2 and GT_Dev.X[0] < 90 and GT_Dev.Y[0] > 2 and GT_Dev.Y[0] < 248 and ReFlag == 0):
                    print("Select photo ...\r\n")
                    Page = 3
                    Read_BMP(PagePath[Page], 0, 0)
                    Photo_L = int(GT_Dev.X[0]/46*2 + 2-GT_Dev.Y[0]/124 + Photo_S*2)
                    Show_Photo_Large(image, Photo_L)
                    ReFlag = 1
                if(ReFlag == 2):  # Refresh small photo
                    ReFlag = 1
                    Read_BMP(PagePath[Page], 0, 0)
                    Show_Photo_Small(image, Photo_S)   # show small photo
                
            
            if(Page == 3  and ReFlag == 0):     #view the photo
                if(GT_Dev.X[0] > 96 and GT_Dev.X[0] < 117 and GT_Dev.Y[0] > 4 and GT_Dev.Y[0] < 25): 
                    print("Photo menu ...\r\n")
                    Page = 2
                    Read_BMP(PagePath[Page], 0, 0)
                    Show_Photo_Small(image, Photo_S)
                    ReFlag = 1
                elif(GT_Dev.X[0] > 96 and GT_Dev.X[0] < 117 and GT_Dev.Y[0] > 57 and GT_Dev.Y[0] < 78): 
                    print("Next photo ...\r\n")
                    Photo_L += 1
                    if(Photo_L > 6):
                        Photo_L = 1
                    ReFlag = 2
                elif(GT_Dev.X[0] > 96 and GT_Dev.X[0] < 117 and GT_Dev.Y[0] > 113 and GT_Dev.Y[0] < 136): 
                    print("Home ...\r\n")
                    Page = 0
                    Read_BMP(PagePath[Page], 0, 0)
                    ReFlag = 1
                elif(GT_Dev.X[0] > 96 and GT_Dev.X[0] < 117 and GT_Dev.Y[0] > 169 and GT_Dev.Y[0] < 190): 
                    print("Last page ...\r\n")
                    if(Photo_L == 1):
                        print("Top photo ...\r\n")
                    else: 
                        Photo_L -= 1
                        ReFlag = 2
                elif(GT_Dev.X[0] > 96 and GT_Dev.X[0] < 117 and GT_Dev.Y[0] > 220 and GT_Dev.Y[0] < 242): 
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
    t.join()
    epd.Dev_exit()
    exit()
