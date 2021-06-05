#include "DEV_Config.h"

#ifndef __ICNT86X_H
#define __ICNT86X_H

#define ICNT_MAX_TOUCH 5

typedef struct{
	UBYTE Touch;
	UBYTE TouchCount;
	UBYTE TouchGestureid;
	
	UBYTE TouchEvenid[ICNT_MAX_TOUCH];
	UWORD X[ICNT_MAX_TOUCH];
	UWORD Y[ICNT_MAX_TOUCH];
	UBYTE P[ICNT_MAX_TOUCH];
}ICNT86_Dev;

void ICNT_Reset(void);
void ICNT_Write(UWORD Reg, char *Data, UBYTE len);
void ICNT_Read(UWORD Reg, char *Data, UBYTE len);
void ICNT_ReadVersion(void);
UBYTE ICNT_Scan(void);
void ICNT_Init(void);

#endif
