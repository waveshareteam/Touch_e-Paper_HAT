import logging
from . import epdconfig as config

class ICNT_Development:
    def __init__(self):
        self.Touch = 0
        self.TouchGestureid = 0
        self.TouchCount = 0
        
        self.TouchEvenid = [0, 1, 2, 3, 4]
        self.X = [0, 1, 2, 3, 4]
        self.Y = [0, 1, 2, 3, 4]
        self.P = [0, 1, 2, 3, 4]
    
class INCT86:
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
    
    def ICNT_Reset(self):
        config.digital_write(self.TRST, 1)
        config.delay_ms(100)
        config.digital_write(self.TRST, 0)
        config.delay_ms(100)
        config.digital_write(self.TRST, 1)
        config.delay_ms(100)

    def ICNT_Write(self, Reg, Data):
        config.i2c_writebyte(Reg, Data)

    def ICNT_Read(self, Reg, len):
        return config.i2c_readbyte(Reg, len)
        
    def ICNT_ReadVersion(self):
        buf = self.ICNT_Read(0x000a, 4)
        print(buf)

    def ICNT_Init(self):
        self.ICNT_Reset()
        self.ICNT_ReadVersion()

    def ICNT_Scan(self, ICNT_Dev, ICNT_Old):
        buf = []
        mask = 0x00
        
        if(ICNT_Dev.Touch == 1):
            # ICNT_Dev.Touch = 0
            buf = self.ICNT_Read(0x1001, 1)
            
            if(buf[0] == 0x00):
                self.ICNT_Write(0x1001, mask)
                config.delay_ms(1)
                # print("buffers status is 0")
                return
            else:
                ICNT_Dev.TouchCount = buf[0]
                
                if(ICNT_Dev.TouchCount > 5 or ICNT_Dev.TouchCount < 1):
                    self.ICNT_Write(0x1001, mask)
                    ICNT_Dev.TouchCount = 0
                    # print("TouchCount number is wrong")
                    return
                    
                buf = self.ICNT_Read(0x1002, ICNT_Dev.TouchCount*7)
                self.ICNT_Write(0x1001, mask)
                
                ICNT_Old.X[0] = ICNT_Dev.X[0];
                ICNT_Old.Y[0] = ICNT_Dev.Y[0];
                ICNT_Old.P[0] = ICNT_Dev.P[0];
                
                for i in range(0, ICNT_Dev.TouchCount, 1):
                    ICNT_Dev.TouchEvenid[i] = buf[6 + 7*i] 
                    ICNT_Dev.X[i] = 295 - ((buf[2 + 7*i] << 8) + buf[1 + 7*i])
                    ICNT_Dev.Y[i] = 127 - ((buf[4 + 7*i] << 8) + buf[3 + 7*i])
                    ICNT_Dev.P[i] = buf[5 + 7*i]

                print(ICNT_Dev.X[0], ICNT_Dev.Y[0], ICNT_Dev.P[0])
                return
        return
                