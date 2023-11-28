/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V2.0
* | Date        :   2020-06-17
* | Info        :   Basic version
*
******************************************************************************/
#include "DEV_Config.h"
#include <unistd.h>
#include <fcntl.h>

/**
 * GPIO
**/
int EPD_RST_PIN;
int EPD_DC_PIN;
int EPD_CS_PIN;
int EPD_BUSY_PIN;
int IIC_Address;


uint32_t fd;
/*****************************************
                GPIO
*****************************************/
void DEV_Digital_Write(UWORD Pin, UBYTE Value)
{
#ifdef USE_BCM2835_LIB
    bcm2835_gpio_write(Pin, Value);
#elif USE_WIRINGPI_LIB
	digitalWrite(Pin, Value);
#elif USE_DEV_LIB
    GPIOD_Write(Pin, Value);
#endif
}

UBYTE DEV_Digital_Read(UWORD Pin)
{
    UBYTE Read_value = 0;
#ifdef USE_BCM2835_LIB
    Read_value = bcm2835_gpio_lev(Pin);
#elif USE_WIRINGPI_LIB
	Read_value = digitalRead(Pin);
#elif USE_DEV_LIB
    Read_value = GPIOD_Read(Pin);
#endif
    return Read_value;
}

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode)
{
#ifdef USE_BCM2835_LIB  
    if(Mode == 0 || Mode == BCM2835_GPIO_FSEL_INPT){
        bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_INPT);
    }else {
        bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_OUTP);
    }
#elif USE_WIRINGPI_LIB
	if(Mode == 0 || Mode == INPUT) {
		pinMode(Pin, INPUT);
		pullUpDnControl(Pin, PUD_UP);
	} else {
		pinMode(Pin, OUTPUT);
		// Debug (" %d OUT \r\n",Pin);
	}
#elif USE_DEV_LIB
    if(Mode == 0 || Mode == GPIOD_IN){
        GPIOD_Direction(Pin, GPIOD_IN);
        // printf("IN Pin = %d\r\n",Pin);
    }else{
        GPIOD_Direction(Pin, GPIOD_OUT);
        // printf("OUT Pin = %d\r\n",Pin);
    }
#endif   
}

/**
 * delay x ms
**/
void DEV_Delay_ms(UDOUBLE xms)
{
#ifdef USE_BCM2835_LIB
    bcm2835_delay(xms);
#elif USE_WIRINGPI_LIB
	delay(xms);
#elif USE_DEV_LIB
    UDOUBLE i;
    for(i=0; i < xms; i++){
        usleep(1000);
    }
#endif
}

static void DEV_GPIO_Init(void)
{
	EPD_RST_PIN     = 17;
	EPD_DC_PIN      = 25;
	EPD_CS_PIN      = 8;
	EPD_BUSY_PIN    = 24;
	
	DEV_GPIO_Mode(EPD_RST_PIN, 1);
	DEV_GPIO_Mode(EPD_DC_PIN, 1);
	DEV_GPIO_Mode(EPD_CS_PIN, 1);
	DEV_GPIO_Mode(EPD_BUSY_PIN, 0);
	
	DEV_Digital_Write(EPD_CS_PIN, 1);
	
    DEV_GPIO_Mode(TRST, 1);
    DEV_GPIO_Mode(INT, 0);
}

/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE DEV_ModuleInit(void)
{
 #ifdef USE_BCM2835_LIB
    if(!bcm2835_init()) {
        printf("bcm2835 init failed  !!! \r\n");
        return 1;
    } else {
        printf("bcm2835 init success !!! \r\n");
    }
	
	DEV_GPIO_Init();
	
	bcm2835_i2c_begin();	
	bcm2835_i2c_setSlaveAddress(IIC_Address);
	// bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_2500);
	
	bcm2835_spi_begin();                                         //Start spi interface, set spi pin for the reuse function
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);     //High first transmission
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                  //spi mode 0
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32);  //Frequency
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                     //set CE0
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);     //enable cs0
#elif USE_WIRINGPI_LIB
	// if(wiringPiSetup() < 0) {//use wiringpi Pin number table
	if(wiringPiSetupGpio() < 0) { //use BCM2835 Pin number table
		printf("set wiringPi lib failed	!!! \r\n");
		return 1;
	} else {
		printf("set wiringPi lib success !!! \r\n");
	}
	// GPIO Config
	DEV_GPIO_Init();
	
	// wiringPiSPISetup(0,10000000);
	wiringPiSPISetupMode(0, 10000000, 0);
	fd = wiringPiI2CSetup(IIC_Address);
	
	DEV_HARDWARE_I2C_begin("/dev/i2c-1");
	DEV_HARDWARE_I2C_setSlaveAddress(IIC_Address);
#elif USE_DEV_LIB
    GPIOD_Export();
	DEV_GPIO_Init();

	DEV_HARDWARE_I2C_begin("/dev/i2c-1");
	DEV_HARDWARE_I2C_setSlaveAddress(IIC_Address);
	
	DEV_HARDWARE_SPI_begin("/dev/spidev0.0");
    DEV_HARDWARE_SPI_setSpeed(10000000);
#endif
    return 0;
}

/**
 * SPI
**/
void DEV_SPI_WriteByte(uint8_t Value)
{
#ifdef USE_BCM2835_LIB
	bcm2835_spi_transfer(Value);
#elif USE_WIRINGPI_LIB
	wiringPiSPIDataRW(0, &Value, 1);
#elif USE_DEV_LIB
	DEV_HARDWARE_SPI_TransferByte(Value);
#endif
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len)
{
#ifdef USE_BCM2835_LIB
	char rData[Len];
	bcm2835_spi_transfernb(pData, rData, Len);
#elif USE_WIRINGPI_LIB
	wiringPiSPIDataRW(0, pData, Len);
#elif USE_DEV_LIB
	DEV_HARDWARE_SPI_Transfer(pData, Len);
#endif
}

UBYTE I2C_Write_Byte(UWORD Reg, char *Data, UBYTE len)
{
    char wbuf[50]={(Reg>>8)&0xff, Reg&0xff};
	for(UBYTE i=0; i<len; i++) {
		wbuf[i+2] = Data[i];
	}
#ifdef USE_BCM2835_LIB
    if(bcm2835_i2c_write(wbuf, len+2) != BCM2835_I2C_REASON_OK) {
		printf("WRITE ERROR \r\n");
		return -1;
	}
#elif USE_WIRINGPI_LIB
	if(len > 1)
		printf("wiringPi I2C WARING \r\n");
	wiringPiI2CWriteReg16(fd, wbuf[0], wbuf[1] | (wbuf[2]<<8));
#elif USE_DEV_LIB
    DEV_HARDWARE_I2C_write(wbuf, len+2);
#endif

	return 0;
}

#ifdef USE_WIRINGPI_LIB
static I2C_Write_WiringPi(UWORD Reg)
{
	wiringPiI2CWriteReg8(fd, (Reg>>8) & 0xff, Reg & 0xff);
}
#endif

UBYTE I2C_Read_Byte(UWORD Reg, char *Data, UBYTE len)
{
	char *rbuf = Data;
	
#ifdef USE_BCM2835_LIB
	I2C_Write_Byte(Reg, 0, 0);
    if(bcm2835_i2c_read(rbuf, len) != BCM2835_I2C_REASON_OK) {
		printf("READ ERROR \r\n");
		return -1;
	}
#elif USE_WIRINGPI_LIB
	I2C_Write_WiringPi(Reg);
	for(UBYTE i=0; i<len; i++) {
		rbuf[i] = wiringPiI2CRead(fd);
	}   
#elif USE_DEV_LIB
	I2C_Write_Byte(Reg, 0, 0);
    DEV_HARDWARE_I2C_read(rbuf, len);
#endif

	return 0;
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_ModuleExit(void)
{
	TRST_0;
	DEV_Digital_Write(EPD_CS_PIN, 0);
	DEV_Digital_Write(EPD_DC_PIN, 0);
	DEV_Digital_Write(EPD_RST_PIN, 0);
#ifdef USE_BCM2835_LIB
	bcm2835_i2c_end();
	bcm2835_spi_end();
    bcm2835_close();
#elif USE_WIRINGPI_LIB

#elif USE_DEV_LIB
    DEV_HARDWARE_I2C_end();
	DEV_HARDWARE_SPI_end();
    GPIOD_Unexport(EPD_CS_PIN);
    GPIOD_Unexport(EPD_DC_PIN);
    GPIOD_Unexport(EPD_RST_PIN);
    GPIOD_Unexport(EPD_BUSY_PIN);
    GPIOD_Unexport_GPIO();
#endif
}

