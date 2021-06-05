import logging
from . import epdconfig as config

class GT_Development:
    def __init__(self):
        self.Touch = 0
        self.TouchpointFlag = 0
        self.TouchCount = 0
        self.Touchkeytrackid = [0, 1, 2, 3, 4]
        self.X = [0, 1, 2, 3, 4]
        self.Y = [0, 1, 2, 3, 4]
        self.S = [0, 1, 2, 3, 4]
    
class GT1151:
    def __init__(self):
        # e-Paper
        self.ERST = config.EPD_RST_PIN  
        self.DC = config.EPD_DC_PIN
        self.CS = config.EPD_CS_PIN
        self.BUSY = config.EPD_BUSY_PIN
        # TP
        self.TRST = config.TRST
        self.INT = config.INT

    def digital_read(self, pin):
        return config.digital_read(pin)
    
    def GT_Reset(self):
        config.digital_write(self.TRST, 1)
        config.delay_ms(100)
        config.digital_write(self.TRST, 0)
        config.delay_ms(100)
        config.digital_write(self.TRST, 1)
        config.delay_ms(100)

    def GT_Write(self, Reg, Data):
        config.i2c_writebyte(Reg, Data)

    def GT_Read(self, Reg, len):
        return config.i2c_readbyte(Reg, len)
         
    def GT_ReadVersion(self):
        buf = self.GT_Read(0x8140, 4)
        print(buf)

    def GT_Init(self):
        self.GT_Reset()
        self.GT_ReadVersion()

    def GT_Scan(self, GT_Dev, GT_Old):
        buf = []
        mask = 0x00
        
        if(GT_Dev.Touch == 1):
            GT_Dev.Touch = 0
            buf = self.GT_Read(0x814E, 1)
            
            if(buf[0]&0x80 == 0x00):
                self.GT_Write(0x814E, mask)
                config.delay_ms(10)
                
            else:
                GT_Dev.TouchpointFlag = buf[0]&0x80
                GT_Dev.TouchCount = buf[0]&0x0f
                
                if(GT_Dev.TouchCount > 5 or GT_Dev.TouchCount < 1):
                    self.GT_Write(0x814E, mask)
                    return
                    
                buf = self.GT_Read(0x814F, GT_Dev.TouchCount*8)
                self.GT_Write(0x814E, mask)
                
                GT_Old.X[0] = GT_Dev.X[0];
                GT_Old.Y[0] = GT_Dev.Y[0];
                GT_Old.S[0] = GT_Dev.S[0];
                
                for i in range(0, GT_Dev.TouchCount, 1):
                    GT_Dev.Touchkeytrackid[i] = buf[0 + 8*i] 
                    GT_Dev.X[i] = (buf[2 + 8*i] << 8) + buf[1 + 8*i]
                    GT_Dev.Y[i] = (buf[4 + 8*i] << 8) + buf[3 + 8*i]
                    GT_Dev.S[i] = (buf[6 + 8*i] << 8) + buf[5 + 8*i]

                print(GT_Dev.X[0], GT_Dev.Y[0], GT_Dev.S[0])
                