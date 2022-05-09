DIR_Config   = ./lib/Config
DIR_Driver	 = ./lib/Driver
DIR_GUI		 = ./lib/GUI
DIR_FONTS    = ./lib/Fonts
DIR_EPD		 = ./lib/EPD
DIR_Examples = ./examples
DIR_BIN      = ./bin

OBJ_C = $(wildcard ${DIR_Driver}/*.c ${DIR_GUI}/*.c ${DIR_EPD}/*.c ${DIR_Config}/*.c ${DIR_Examples}/*.c ${DIR_FONTS}/*.c )
OBJ_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = main

USELIB = USE_BCM2835_LIB
# USELIB = USE_WIRINGPI_LIB
# USELIB = USE_DEV_LIB

DEBUG = -D $(USELIB)

ifeq ($(USELIB), USE_BCM2835_LIB)
    LIB = -lbcm2835 -lm 
else ifeq ($(USELIB), USE_WIRINGPI_LIB)
    LIB = -lwiringPi -lm 
else ifeq ($(USELIB), USE_DEV_LIB)
    LIB = -lm 
endif
LIB += -lpthread

CC = gcc
MSG = -g -O0 -Wall
CFLAGS += $(MSG) $(DEBUG)

${TARGET}:${OBJ_O}
	$(CC) $(CFLAGS) $(OBJ_O) -o $@ $(LIB)
    
$(shell mkdir -p $(DIR_BIN))

${DIR_BIN}/%.o:$(DIR_Examples)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ -I $(DIR_Config) -I $(DIR_Driver)  -I $(DIR_EPD) -I $(DIR_GUI)
    
${DIR_BIN}/%.o:$(DIR_Driver)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ -I $(DIR_Config) $(DEBUG)

${DIR_BIN}/%.o:$(DIR_EPD)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ -I $(DIR_Config) $(DEBUG)
    
${DIR_BIN}/%.o:$(DIR_FONTS)/%.c 
	$(CC) $(CFLAGS) -c  $< -o $@ $(DEBUG)
	
${DIR_BIN}/%.o:$(DIR_GUI)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ -I $(DIR_Config) $(DEBUG)
	
${DIR_BIN}/%.o:$(DIR_Config)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(LIB)

	
clean :
	rm $(DIR_BIN)/*.* 
	rm $(TARGET) 
