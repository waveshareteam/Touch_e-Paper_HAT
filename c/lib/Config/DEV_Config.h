#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_
/***********************************************************************************************************************
			------------------------------------------------------------------------
			|\\\																///|
			|\\\					Hardware interface							///|
			------------------------------------------------------------------------
***********************************************************************************************************************/
#ifdef USE_BCM2835_LIB
    #include <bcm2835.h>
#elif USE_WIRINGPI_LIB
	#include <wiringPi.h>
    #include <wiringPiSPI.h>
	#include <wiringPiI2C.h>
    #include "dev_hardware_i2c.h"  
#elif USE_LGPIO_LIB
    #include <lgpio.h>
    #define LFLAGS 0
    #define NUM_MAXBUF  4
#elif USE_GPIOD_LIB
    #include "RPI_gpiod.h"
    #include "dev_hardware_i2c.h"   
	#include "dev_hardware_SPI.h"
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "Debug.h"

// #define IIC_Address			0x14
// #define IIC_Address_Read	0x29
// #define IIC_Address_Write	0x28

/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

//TP Define
#define TRST		22
#define INT			27

#define TRST_0		DEV_Digital_Write(TRST, 0)
#define TRST_1		DEV_Digital_Write(TRST, 1)

#define INT_0		DEV_Digital_Write(INT, 0)
#define INT_1		DEV_Digital_Write(INT, 1)

/**
 * GPIOI config
**/
extern int EPD_RST_PIN;
extern int EPD_DC_PIN;
extern int EPD_CS_PIN;
extern int EPD_BUSY_PIN;

/*------------------------------------------------------------------------------------------------------*/

UBYTE DEV_ModuleInit(void);
void  DEV_ModuleExit(void);

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode);
void DEV_Delay_ms(UDOUBLE xms);

void DEV_Digital_Write(UWORD Pin, UBYTE Value);
UBYTE DEV_Digital_Read(UWORD Pin);

void DEV_SPI_WriteByte(UBYTE Value);
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len);

UBYTE I2C_Write_Byte(UWORD Reg, char *Data, UBYTE len);
UBYTE I2C_Read_Byte(UWORD Reg, char *Data, UBYTE len);
#endif
