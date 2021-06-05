#include "Test.h"
#include "EPD_2in13_V2.h"
#include "GT1151.h"

extern GT1151_Dev Dev_Now, Dev_Old;
extern int IIC_Address;
pthread_t t1;
UBYTE flag_t = 1;	

char *PhotoPath_S[7] = {"./pic/2in13/Photo_1_0.bmp",
						"./pic/2in13/Photo_1_1.bmp", "./pic/2in13/Photo_1_2.bmp", "./pic/2in13/Photo_1_3.bmp", "./pic/2in13/Photo_1_4.bmp",
						"./pic/2in13/Photo_1_5.bmp", "./pic/2in13/Photo_1_6.bmp",
						};
char *PhotoPath_L[7] = {"./pic/2in13/Photo_2_0.bmp",
						"./pic/2in13/Photo_2_1.bmp", "./pic/2in13/Photo_2_2.bmp", "./pic/2in13/Photo_2_3.bmp", "./pic/2in13/Photo_2_4.bmp",
						"./pic/2in13/Photo_2_5.bmp", "./pic/2in13/Photo_2_6.bmp",
						};
char *PagePath[4] = {"./pic/2in13/Menu.bmp", "./pic/2in13/White_board.bmp", "./pic/2in13/Photo_1.bmp", "./pic/2in13/Photo_2.bmp"};


void Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:exit\r\n");
	EPD_2IN13_V2_Sleep();
	DEV_Delay_ms(2000);
	flag_t = 0;
	pthread_join(t1, NULL);
    DEV_ModuleExit();
    exit(0);
}

void *pthread_irq(void *arg)
{
	while(flag_t) {
		if(DEV_Digital_Read(INT) == 0) {
			Dev_Now.Touch = 1;
		}
		else {
			Dev_Now.Touch = 0;
		}
		DEV_Delay_ms(0.01);
	}
	printf("thread:exit\r\n");
	pthread_exit(NULL);
}

void Show_Photo_Small(UBYTE small)
{
	for(UBYTE t=1; t<5; t++) {
		// printf("t= %d , small= %d \r\n", t, small);
		if(small*2+t > 6)
			GUI_ReadBmp(PhotoPath_S[0], (t-1)/2*45+2, (t%2)*124+2);
		else
			GUI_ReadBmp(PhotoPath_S[small*2+t], (t-1)/2*45+2, (t%2)*124+2);
	}
}

void Show_Photo_Large(UBYTE large)
{
	if(large > 6)
		GUI_ReadBmp(PhotoPath_L[0], 2, 2);
	else
		GUI_ReadBmp(PhotoPath_L[large], 2, 2);
}

int TestCode_2in13(void)
{
	IIC_Address = 0x14;
	
	UDOUBLE i=0, j=0, k=0;
	UBYTE Page=0, Photo_L=0, Photo_S=0;
	UBYTE ReFlag=0, SelfFlag=0;
	signal(SIGINT, Handler);
	DEV_ModuleInit();
	
	pthread_create(&t1, NULL, pthread_irq, NULL);
	
    EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
	
    EPD_2IN13_V2_Clear();
	
	GT_Init();
	DEV_Delay_ms(100);
    //Create a new image cache
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_2IN13_V2_WIDTH % 8 == 0)? (EPD_2IN13_V2_WIDTH / 8 ): (EPD_2IN13_V2_WIDTH / 8 + 1)) * EPD_2IN13_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN13_V2_WIDTH, EPD_2IN13_V2_HEIGHT, 0, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_SetMirroring(MIRROR_HORIZONTAL);
    Paint_Clear(WHITE);
	GUI_ReadBmp("./pic/2in13/Menu.bmp", 0, 0);
	EPD_2IN13_V2_DisplayPartBaseImage(BlackImage);
	EPD_2IN13_V2_Init(EPD_2IN13_V2_PART);

	while(1) {
		// k++;
		if(i > 12 || ReFlag == 1) {
			if(Page == 1 && SelfFlag != 1)
				EPD_2IN13_V2_DisplayPart(BlackImage);
			else 
				EPD_2IN13_V2_DisplayPart_Wait(BlackImage);
			i = 0;
			k = 0;
			j++;
			ReFlag = 0;
			printf("*** Draw Refresh ***\r\n");
		}else if(k++>30000000 && i>0 && Page == 1) {
			EPD_2IN13_V2_DisplayPart(BlackImage);
			i = 0;
			k = 0;
			j++;
			printf("*** Overtime Refresh ***\r\n");
		}else if(j > 100 || SelfFlag) {
			SelfFlag = 0;
			j = 0;
			EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
			EPD_2IN13_V2_DisplayPartBaseImage(BlackImage);
			EPD_2IN13_V2_Init(EPD_2IN13_V2_PART);
			printf("--- Self Refresh ---\r\n");
		}
		
		if(GT_Scan()==1 || (Dev_Now.X[0] == Dev_Old.X[0] && Dev_Now.Y[0] == Dev_Old.Y[0])) { // No new touch
			// printf("%d %d \r\n", j, SelfFlag);
			// printf("No new touch \r\n");
			continue;
		}

		if(Dev_Now.TouchpointFlag) {
			i++;
			Dev_Now.TouchpointFlag = 0;

			if(Page == 0  && ReFlag == 0) {	//main menu
				if(Dev_Now.X[0] > 29 && Dev_Now.X[0] < 92 && Dev_Now.Y[0] > 56 && Dev_Now.Y[0] < 95) {
					printf("Photo ...\r\n");
					Page = 2;
					GUI_ReadBmp(PagePath[Page], 0, 0);
					Show_Photo_Small(Photo_S);
					ReFlag = 1;
				}
				else if(Dev_Now.X[0] > 29 && Dev_Now.X[0] < 92 && Dev_Now.Y[0] > 153 && Dev_Now.Y[0] < 193) {
					printf("Draw ...\r\n");
					Page = 1;
					GUI_ReadBmp(PagePath[Page], 0, 0);
					ReFlag = 1;
				}
			}

			if(Page == 1 && ReFlag == 0) {	//white board
				Paint_DrawPoint(Dev_Now.X[0], Dev_Now.Y[0], BLACK, Dev_Now.S[0]/8+1, DOT_STYLE_DFT);
				
				if(Dev_Now.X[0] > 96 && Dev_Now.X[0] < 118 && Dev_Now.Y[0] > 6 && Dev_Now.Y[0] < 30) {
					printf("Home ...\r\n");
					Page = 1;
					GUI_ReadBmp(PagePath[Page], 0, 0);
					ReFlag = 1;
				}
				else if(Dev_Now.X[0] > 96 && Dev_Now.X[0] < 118 && Dev_Now.Y[0] > 113 && Dev_Now.Y[0] < 136) {
					printf("Clear ...\r\n");
					Page = 0;
					GUI_ReadBmp(PagePath[Page], 0, 0);
					ReFlag = 1;
				}
				else if(Dev_Now.X[0] > 96 && Dev_Now.X[0] < 118 && Dev_Now.Y[0] > 220 && Dev_Now.Y[0] < 242) {
					printf("Refresh ...\r\n");
					SelfFlag = 1;
					ReFlag = 1;
				}
			}

			if(Page == 2  && ReFlag == 0) { //photo menu
				if(Dev_Now.X[0] > 97 && Dev_Now.X[0] < 119 && Dev_Now.Y[0] > 113 && Dev_Now.Y[0] < 136) {
					printf("Home ...\r\n");
					Page = 0;
					GUI_ReadBmp(PagePath[Page], 0, 0);
					ReFlag = 1;
				}
				else if(Dev_Now.X[0] > 97 && Dev_Now.X[0] < 119 && Dev_Now.Y[0] > 57 && Dev_Now.Y[0] < 78) {
					printf("Next page ...\r\n");
					Photo_S++;
					if(Photo_S > 2)	// 6 photos is a maximum of three pages
						Photo_S=0;
					ReFlag = 2;
				}
				else if(Dev_Now.X[0] > 97 && Dev_Now.X[0] < 119 && Dev_Now.Y[0] > 169 && Dev_Now.Y[0] < 190) {
					printf("Last page ...\r\n");
					if(Photo_S == 0)
						printf("Top page ...\r\n");
					else {
						Photo_S--;
						ReFlag = 2;
					}
				}
				else if(Dev_Now.X[0] > 97 && Dev_Now.X[0] < 119 && Dev_Now.Y[0] > 220 && Dev_Now.Y[0] < 242) {
					printf("Refresh ...\r\n");
					SelfFlag = 1;
					ReFlag = 1;
				}
				else if(Dev_Now.X[0] > 2 && Dev_Now.X[0] < 90 && Dev_Now.Y[0] > 2 && Dev_Now.Y[0] < 248 && ReFlag == 0) {
					printf("Select photo ...\r\n");
					Page = 3;
					GUI_ReadBmp(PagePath[Page], 0, 0);		
					Photo_L = Dev_Now.X[0]/46*2 + 2-Dev_Now.Y[0]/124 + Photo_S*2;
					Show_Photo_Large(Photo_L);
					ReFlag = 1;
				}
				if(ReFlag == 2) { // Refresh small photo
					ReFlag = 1;
					GUI_ReadBmp(PagePath[Page], 0, 0);
					Show_Photo_Small(Photo_S);	// show small photo
				}
			}
			
			if(Page == 3  && ReFlag == 0) {	//view the photo
				if(Dev_Now.X[0] > 96 && Dev_Now.X[0] < 117 && Dev_Now.Y[0] > 4 && Dev_Now.Y[0] < 25) {
					printf("Photo menu ...\r\n");
					Page = 2;
					GUI_ReadBmp(PagePath[Page], 0, 0);
					Show_Photo_Small(Photo_S);
					ReFlag = 1;
				}
				else if(Dev_Now.X[0] > 96 && Dev_Now.X[0] < 117 && Dev_Now.Y[0] > 57 && Dev_Now.Y[0] < 78) {
					printf("Next photo ...\r\n");
					Photo_L++;
					if(Photo_L > 6)
						Photo_L=1;
					ReFlag = 2;
				}
				else if(Dev_Now.X[0] > 96 && Dev_Now.X[0] < 117 && Dev_Now.Y[0] > 113 && Dev_Now.Y[0] < 136) {
					printf("Home ...\r\n");
					Page = 0;
					GUI_ReadBmp(PagePath[Page], 0, 0);
					ReFlag = 1;
				}
				else if(Dev_Now.X[0] > 96 && Dev_Now.X[0] < 117 && Dev_Now.Y[0] > 169 && Dev_Now.Y[0] < 190) {
					printf("Last page ...\r\n");
					if(Photo_L == 1)
						printf("Top photo ...\r\n");
					else {
						Photo_L--;
						ReFlag = 2;
					}
				}
				else if(Dev_Now.X[0] > 96 && Dev_Now.X[0] < 117 && Dev_Now.Y[0] > 220 && Dev_Now.Y[0] < 242) {
					printf("Refresh photo ...\r\n");
					SelfFlag = 1;
					ReFlag = 1;
				}
				if(ReFlag == 2) {	// Refresh large photo
					ReFlag = 1;
					Show_Photo_Large(Photo_L);
				}
			}

		}
	}
	return 0;
}
