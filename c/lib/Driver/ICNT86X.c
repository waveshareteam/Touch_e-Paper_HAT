#include "ICNT86X.h"

ICNT86_Dev ICNT86_Dev_Now, ICNT86_Dev_Old;

void ICNT_Reset(void)
{
	TRST_1;
	DEV_Delay_ms(100);
	TRST_0;
	DEV_Delay_ms(100);
	TRST_1;
	DEV_Delay_ms(100);
}

void ICNT_Write(UWORD Reg, char *Data, UBYTE len)
{
	I2C_Write_Byte(Reg, Data, len);
}

void ICNT_Read(UWORD Reg, char *Data, UBYTE len)
{
	I2C_Read_Byte(Reg, Data, len);
}

void ICNT_ReadVersion(void)
{
	char buf[4];
	ICNT_Read(0x000a, buf, 4);
	printf("IC Version is %x %x \r\n", buf[0], buf[1]);
	printf("FW Version is %x %x \r\n", buf[2], buf[3]);
}

void ICNT_Init(void)
{
	ICNT_Reset();
	ICNT_ReadVersion();
}

UBYTE ICNT_Scan(void)
{
	char buf[100];
	char mask[1] = {0x00};
	if (ICNT86_Dev_Now.Touch == 1) {
		// ICNT86_Dev_Now.Touch = 0;
		ICNT_Read(0x1001, buf, 1);
		if (buf[0] == 0x00) {		//No new touch
			ICNT_Write(0x1001, mask, 1);
			DEV_Delay_ms(1);
			// printf("buffers status is 0 \r\n");
			return 1;
		}
		else {
			ICNT86_Dev_Now.TouchCount = buf[0];
			if (ICNT86_Dev_Now.TouchCount > 5 || ICNT86_Dev_Now.TouchCount < 1) {
				ICNT_Write(0x1001, mask, 1);
				ICNT86_Dev_Now.TouchCount = 0;
				// printf("TouchCount number is wrong \r\n");
				return 1;
			}
			ICNT_Read(0x1002, buf, ICNT86_Dev_Now.TouchCount*7);
			ICNT_Write(0x1001, mask, 1);
			
			ICNT86_Dev_Old.X[0] = ICNT86_Dev_Now.X[0];
			ICNT86_Dev_Old.Y[0] = ICNT86_Dev_Now.Y[0];
			ICNT86_Dev_Old.P[0] = ICNT86_Dev_Now.P[0];
			
			for(UBYTE i=0; i<ICNT86_Dev_Now.TouchCount; i++) {
				ICNT86_Dev_Now.X[i] = ((UWORD)buf[2+7*i] << 8) + buf[1+7*i];
				ICNT86_Dev_Now.Y[i] = ((UWORD)buf[4+7*i] << 8) + buf[3+7*i];
				ICNT86_Dev_Now.P[i] = buf[5+7*i];
				ICNT86_Dev_Now.TouchEvenid[i] = buf[6 + 7*i];
			}
			
			for(UBYTE i=0; i<ICNT86_Dev_Now.TouchCount; i++)
				printf("Point %d: X is %d, Y is %d, Pressure is %d \r\n", i+1, ICNT86_Dev_Now.X[i], ICNT86_Dev_Now.Y[i], ICNT86_Dev_Now.P[i]);
			return 0;
		}
	}
	return 1;
}