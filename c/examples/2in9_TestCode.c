#include "Test.h"
#include "EPD_2in9_V2.h"
#include "ICNT86X.h"
#include "time.h"

extern ICNT86_Dev ICNT86_Dev_Now, ICNT86_Dev_Old;
extern int IIC_Address;
static pthread_t t1, t2;
UBYTE flag_2in9=1, dis_lock=1, dis_flag=1;	
UBYTE *BlackImage, *BlackImage_ASYNC;

char *PhotoPath_S_2in9[10] = {"./pic/2in9/Photo_1_0.bmp",
						"./pic/2in9/Photo_1_1.bmp", "./pic/2in9/Photo_1_2.bmp", "./pic/2in9/Photo_1_3.bmp", "./pic/2in9/Photo_1_4.bmp",
						"./pic/2in9/Photo_1_5.bmp", "./pic/2in9/Photo_1_6.bmp", "./pic/2in9/Photo_1_7.bmp", "./pic/2in9/Photo_1_8.bmp",
						"./pic/2in9/Photo_1_9.bmp",
						};
char *PhotoPath_L_2in9[10] = {"./pic/2in9/Photo_2_0.bmp",
						"./pic/2in9/Photo_2_1.bmp", "./pic/2in9/Photo_2_2.bmp", "./pic/2in9/Photo_2_3.bmp", "./pic/2in9/Photo_2_4.bmp",
						"./pic/2in9/Photo_2_5.bmp", "./pic/2in9/Photo_2_6.bmp", "./pic/2in9/Photo_2_7.bmp", "./pic/2in9/Photo_2_8.bmp",
						"./pic/2in9/Photo_2_9.bmp",
						};
char *PagePath_2in9[4] = {"./pic/2in9/Menu.bmp", "./pic/2in9/White_board.bmp", "./pic/2in9/Photo_1.bmp", "./pic/2in9/Photo_2.bmp"};


void Handler_2in9(int signo)
{
    //System Exit
    printf("\r\nHandler_2in9:exit\r\n");
	EPD_2IN9_V2_Sleep();
	DEV_Delay_ms(1000);
	flag_2in9 = 0;
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
    DEV_ModuleExit();
    exit(0);
}

void *pthread_irq_2in9(void *arg)
{
	while(flag_2in9) {
		if(DEV_Digital_Read(INT) == 0) {
			ICNT86_Dev_Now.Touch = 1;
			// printf("!");
		}
		else {
			ICNT86_Dev_Now.Touch = 0;
		}
		DEV_Delay_ms(0.01);
	}
	printf("thread1:exit\r\n");
	pthread_exit(NULL);
}

void *pthread_dis_2in9(void *arg)
{
	while(flag_2in9) {
		if(dis_flag) {
			dis_lock = 1;
			EPD_2IN9_V2_Display_Partial(BlackImage_ASYNC);
			dis_flag = 0;
			dis_lock = 0;
			printf("ASYNC display over, unlock \r\n");
		}
		else {
			dis_lock = 0;
		}
		// DEV_Delay_ms(0.01);
	}
	
	printf("thread2:exit\r\n");
	pthread_exit(NULL);
}

void Show_Photo_Small_2in9(UBYTE small)
{
	for(UBYTE t=1; t<7; t++) {
		// printf("t= %d , small= %d \r\n", t, small);
		if(small*3+t > 9) // Max image is 9
			GUI_ReadBmp(PhotoPath_S_2in9[0], (t-1)%3*98+2, (t-1)/3*48+2);
		else {
			// printf("x is %d, y is %d \r\n", (t-1)%3*98, (t-1)/3*48);
			GUI_ReadBmp(PhotoPath_S_2in9[small*3+t], (t-1)%3*98+2, (t-1)/3*48+2);
		}
	}
}

void Show_Photo_Large_2in9(UBYTE large)
{
	if(large > 9) // Max image is 9
		GUI_ReadBmp(PhotoPath_L_2in9[0], 2, 2);
	else
		GUI_ReadBmp(PhotoPath_L_2in9[large], 2, 2);
}

void Get_Current_Time(PAINT_TIME *pTime)
{
    time_t t;
    struct tm *nowtime;
    
    time(&t);
	nowtime = localtime(&t);
	
	pTime->Year = nowtime->tm_year + 1900;
	pTime->Month = nowtime->tm_mon + 1;
	pTime->Day = nowtime->tm_mday;
	pTime->Hour = nowtime->tm_hour;
	pTime->Min = nowtime->tm_min;
}


int Test4gray_2in9(void)
{
    EPD_2IN9_V2_Gray4_Init();
    UWORD Imagesize = ((EPD_2IN9_V2_WIDTH % 4 == 0)? (EPD_2IN9_V2_WIDTH / 4 ): (EPD_2IN9_V2_WIDTH / 4 + 1)) * EPD_2IN9_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 0, WHITE);
    Paint_SetScale(4);
    Paint_Clear(WHITE);
    GUI_ReadBmp_4Gray("./pic/2in9/2in9_Scale.bmp", 0, 0);
    EPD_2IN9_V2_4GrayDisplay(BlackImage);
    DEV_Delay_ms(3000);
    free(BlackImage);
}


int TestCode_2in9(void)
{
	IIC_Address = 0x48;
	
	UDOUBLE i=0, j=0, k=0;
	UBYTE Page=0, Photo_L=0, Photo_S=0;
	UBYTE ReFlag=0, SelfFlag=0;
	signal(SIGINT, Handler_2in9);
	DEV_ModuleInit();
	
	pthread_create(&t1, NULL, pthread_irq_2in9, NULL);
	
    /*
        Because the touch display requires a relatively fast refresh speed, the default 
        needs to use partial refresh, and four gray levels cannot be used in this mode. 
        Here, only four gray level picture refresh demonstration is used
    */
    // Test4gray_2in9();

    EPD_2IN9_V2_Init();
	
    EPD_2IN9_V2_Clear();
	
	ICNT_Init();
	DEV_Delay_ms(100);
    //Create a new image cache
    UWORD Imagesize = ((EPD_2IN9_V2_WIDTH % 8 == 0)? (EPD_2IN9_V2_WIDTH / 8 ): (EPD_2IN9_V2_WIDTH / 8 + 1)) * EPD_2IN9_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    if((BlackImage_ASYNC = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
	GUI_ReadBmp("./pic/2in9/Menu.bmp", 0, 0);

    PAINT_TIME sPaint_time, sPaint_time_f;
	Get_Current_Time(&sPaint_time);
	Get_Current_Time(&sPaint_time_f);
	Paint_DrawTime(210, 45, &sPaint_time, &Font24, WHITE, WHITE);
	Paint_DrawDate(212, 80, &sPaint_time, &Font12, WHITE, WHITE);

	EPD_2IN9_V2_Display_Base(BlackImage);
	memcpy(BlackImage_ASYNC, BlackImage, Imagesize);
	pthread_create(&t2, NULL, pthread_dis_2in9, NULL);
	while(1) {
		// k++;
		if(i > 30 || ReFlag == 1) {
			if(Page == 0) {
				Get_Current_Time(&sPaint_time);
				Paint_ClearWindows(210, 40, 290, 100, BLACK);
				Paint_DrawTime(212, 45, &sPaint_time, &Font24, WHITE, WHITE);
				Paint_DrawDate(212, 80, &sPaint_time, &Font12, WHITE, WHITE);
			}
			
			if(Page == 1 && SelfFlag != 1 && dis_lock != 1) {
				memcpy(BlackImage_ASYNC, BlackImage, Imagesize);
				dis_flag = 1;
				
				i = 0;
				k = 0;
				j++;
				ReFlag = 0;
				printf("*** Draw Refresh ***\r\n");
			}
			else if(!dis_lock){
				EPD_2IN9_V2_Display_Partial_Wait(BlackImage);
				
				i = 0;
				k = 0;
				j++;
				ReFlag = 0;
				printf("*** Touch Refresh ***\r\n");
			}
		}else if(k++>50000000 && i>0 && Page == 1) {
			EPD_2IN9_V2_Display_Partial(BlackImage);
			i = 0;
			k = 0;
			j++;
			printf("*** Overtime Refresh ***\r\n");
		}else if(j > 100 || SelfFlag) {
			SelfFlag = 0;
			j = 0;
			EPD_2IN9_V2_Init();
			EPD_2IN9_V2_Display_Base(BlackImage);
			printf("--- Self Refresh ---\r\n");
		}
		
		if(Page == 0  && ReFlag == 0) {	//main menu
			Get_Current_Time(&sPaint_time_f);
			if(sPaint_time_f.Min != sPaint_time.Min) {
				ReFlag = 1;
			}
		}
		
		
		if(ICNT_Scan()==1 || (ICNT86_Dev_Now.X[0] == ICNT86_Dev_Old.X[0] && ICNT86_Dev_Now.Y[0] == ICNT86_Dev_Old.Y[0])) { // No new touch
			// printf("%d %d \r\n", j, SelfFlag);
			// printf("No new touch \r\n");
			continue;
		}

		if(ICNT86_Dev_Now.TouchCount) {
			i++;
			if(Page == 0  && ReFlag == 0) {	//main menu
				if(ICNT86_Dev_Now.X[0] > 119 && ICNT86_Dev_Now.X[0] < 152 && ICNT86_Dev_Now.Y[0] > 31 && ICNT86_Dev_Now.Y[0] < 96) {
					printf("Photo ...\r\n");
					Page = 2;
					GUI_ReadBmp(PagePath_2in9[Page], 0, 0);
					Show_Photo_Small_2in9(Photo_S);
					ReFlag = 1;
				}
				else if(ICNT86_Dev_Now.X[0] > 39 && ICNT86_Dev_Now.X[0] < 80 && ICNT86_Dev_Now.Y[0] > 31 && ICNT86_Dev_Now.Y[0] < 96) {
					printf("Draw ...\r\n");
					Page = 1;
					GUI_ReadBmp(PagePath_2in9[Page], 0, 0);
					ReFlag = 1;
				}
			}

			if(Page == 1 && ReFlag == 0) {	//white board
				// Paint_DrawPoint(ICNT86_Dev_Now.X[0], ICNT86_Dev_Now.Y[0], BLACK, ICNT86_Dev_Now.P[0]/8+1, DOT_STYLE_DFT);
				Paint_DrawPoint(ICNT86_Dev_Now.X[0], ICNT86_Dev_Now.Y[0], BLACK, 3, DOT_STYLE_DFT);
				
				if(ICNT86_Dev_Now.X[0] > 136 && ICNT86_Dev_Now.X[0] < 159 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Home ...\r\n");
					Page = 0;
					GUI_ReadBmp(PagePath_2in9[Page], 0, 0);
					ReFlag = 1;
				}
				else if(ICNT86_Dev_Now.X[0] > 266 && ICNT86_Dev_Now.X[0] < 289 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Clear ...\r\n");
					Page = 1;
					GUI_ReadBmp(PagePath_2in9[Page], 0, 0);
					ReFlag = 1;
				}
				else if(ICNT86_Dev_Now.X[0] > 5 && ICNT86_Dev_Now.X[0] < 27 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Refresh ...\r\n");
					SelfFlag = 1;
					ReFlag = 1;
				}
			}

			if(Page == 2 && ReFlag == 0) { //photo menu
				if(ICNT86_Dev_Now.X[0] > 135 && ICNT86_Dev_Now.X[0] < 160 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Home ...\r\n");
					Page = 0;
					GUI_ReadBmp(PagePath_2in9[Page], 0, 0);
					ReFlag = 1;
				}
				else if(ICNT86_Dev_Now.X[0] > 203 && ICNT86_Dev_Now.X[0] < 224 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Next page ...\r\n");
					Photo_S++;
					if(Photo_S > 2)	// 9 photos is a maximum of three pages
						Photo_S=0;
					ReFlag = 2;
				}
				else if(ICNT86_Dev_Now.X[0] > 71 && ICNT86_Dev_Now.X[0] < 92 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Last page ...\r\n");
					if(Photo_S == 0)
						printf("Top page ...\r\n");
					else {
						Photo_S--;
						ReFlag = 2;
					}
				}
				else if(ICNT86_Dev_Now.X[0] > 5 && ICNT86_Dev_Now.X[0] < 27 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Refresh ...\r\n");
					SelfFlag = 1;
					ReFlag = 1;
				}
				else if(ICNT86_Dev_Now.X[0] > 2 && ICNT86_Dev_Now.X[0] < 293 && ICNT86_Dev_Now.Y[0] > 2 && ICNT86_Dev_Now.Y[0] < 96 && ReFlag == 0) {
					printf("Select photo ...\r\n");
					Page = 3;
					GUI_ReadBmp(PagePath_2in9[Page], 0, 0);		
					Photo_L = ICNT86_Dev_Now.X[0]/96 + ICNT86_Dev_Now.Y[0]/48*3 + Photo_S*3 + 1;
					Show_Photo_Large_2in9(Photo_L);
					ReFlag = 1;
				}
				if(ReFlag == 2) { // Refresh small photo
					ReFlag = 1;
					GUI_ReadBmp(PagePath_2in9[Page], 0, 0);
					Show_Photo_Small_2in9(Photo_S);	// show small photo
				}
			}
			
			if(Page == 3 && ReFlag == 0) {	//view the photo
				if(ICNT86_Dev_Now.X[0] > 268 && ICNT86_Dev_Now.X[0] < 289 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Photo menu ...\r\n");
					Page = 2;
					GUI_ReadBmp(PagePath_2in9[Page], 0, 0);
					Show_Photo_Small_2in9(Photo_S);
					ReFlag = 1;
				}
				else if(ICNT86_Dev_Now.X[0] > 203 && ICNT86_Dev_Now.X[0] < 224 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Next photo ...\r\n");
					Photo_L++;
					if(Photo_L > 9)
						Photo_L=1;
					ReFlag = 2;
				}
				else if(ICNT86_Dev_Now.X[0] > 135 && ICNT86_Dev_Now.X[0] < 160 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Home ...\r\n");
					Page = 0;
					GUI_ReadBmp(PagePath_2in9[Page], 0, 0);
					ReFlag = 1;
				}
				else if(ICNT86_Dev_Now.X[0] > 71 && ICNT86_Dev_Now.X[0] < 92 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Last page ...\r\n");
					if(Photo_L == 1)
						printf("Top photo ...\r\n");
					else {
						Photo_L--;
						ReFlag = 2;
					}
				}
				else if(ICNT86_Dev_Now.X[0] > 5 && ICNT86_Dev_Now.X[0] < 27 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Refresh photo ...\r\n");
					SelfFlag = 1;
					ReFlag = 1;
				}
				if(ReFlag == 2) {	// Refresh large photo
					ReFlag = 1;
					Show_Photo_Large_2in9(Photo_L);
				}
			}

		}
	}
	return 0;
}
