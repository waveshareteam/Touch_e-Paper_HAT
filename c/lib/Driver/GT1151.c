#include "GT1151.h"

GT1151_Dev Dev_Now, Dev_Old;
UBYTE GT_Gesture_Mode = 0;

void GT_Reset(void)
{
	TRST_1;
	DEV_Delay_ms(100);
	TRST_0;
	DEV_Delay_ms(100);
	TRST_1;
	DEV_Delay_ms(100);
}

void GT_Write(UWORD Reg, char *Data, UBYTE len)
{
	I2C_Write_Byte(Reg, Data, len);
}

void GT_Read(UWORD Reg, char *Data, UBYTE len)
{
	I2C_Read_Byte(Reg, Data, len);
}

void GT_ReadVersion(void)
{
	char buf[4];
	GT_Read(0x8140, buf, 4);
	printf("Product ID is %c %c %c %c \r\n", buf[0], buf[1], buf[2] ,buf[3]);
}

void GT_Init(void)
{
	GT_Reset();
	GT_ReadVersion();
}

void GT_Gesture(void)
{
	char buf[3] ={0x08, 0x00, 0xf8};
	GT_Write(0x8040, &buf[0], 1);
	GT_Write(0x8041, &buf[1], 1);
	GT_Write(0x8042, &buf[2], 1);
	GT_Gesture_Mode = 1;
	printf("into gesture mode \r\n");
	DEV_Delay_ms(1);
}

void GT_Gesture_Scan(void)
{
	char buf;
	GT_Read(0x814c, &buf, 1);
	if(buf == 0xcc)
	{
		printf("gesture mode exiting \r\n");
		GT_Gesture_Mode = 0;
		GT_Reset();
		Dev_Old.X[0] = Dev_Now.X[0];
		Dev_Old.Y[0] = Dev_Now.Y[0];
		Dev_Old.S[0] = Dev_Now.S[0];
	}
	else
	{
		buf = 0x00;
		GT_Write(0x814c, &buf, 1);
	}
		
}

UBYTE GT_Scan(void)
{
	char buf[100];
	char mask[1] = {0x00};
	
	if (Dev_Now.Touch == 1) {
		// Dev_Now.Touch = 0;
		if(GT_Gesture_Mode == 1)
		{
			GT_Gesture_Scan();
			return 1;
		}
		else 
		{
			GT_Read(0x814E, buf, 1);
			if ((buf[0]&0x80) == 0x00) {		//No new touch
				GT_Write(0x814E, mask, 1);
				DEV_Delay_ms(1);
				// printf("buffers status is 0 \r\n");
				return 1;
			}
			else {
				Dev_Now.TouchpointFlag = buf[0]&0x80;
				Dev_Now.TouchCount = buf[0]&0x0f;
				if (Dev_Now.TouchCount > 5 || Dev_Now.TouchCount < 1) {
					GT_Write(0x814E, mask, 1);
					// printf("TouchCount number is wrong \r\n");
					return 1;
				}
				GT_Read(0x814F, &buf[1], Dev_Now.TouchCount*8);
				GT_Write(0x814E, mask, 1);
				
				Dev_Old.X[0] = Dev_Now.X[0];
				Dev_Old.Y[0] = Dev_Now.Y[0];
				Dev_Old.S[0] = Dev_Now.S[0];
				
				for(UBYTE i=0; i<Dev_Now.TouchCount; i++) {
					Dev_Now.Touchkeytrackid[i] = buf[1 + 8*i];
					Dev_Now.X[i] = ((UWORD)buf[3+8*i] << 8) + buf[2+8*i];
					Dev_Now.Y[i] = ((UWORD)buf[5+8*i] << 8) + buf[4+8*i];
					Dev_Now.S[i] = ((UWORD)buf[7+8*i] << 8) + buf[6+8*i];
				}
				
				for(UBYTE i=0; i<Dev_Now.TouchCount; i++)
					printf("Point %d: X is %d, Y is %d, Size is %d \r\n", i+1, Dev_Now.X[i], Dev_Now.Y[i], Dev_Now.S[i]);
				return 0;
			}
		}
	}
	return 1;
}