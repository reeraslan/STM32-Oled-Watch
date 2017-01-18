
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef Calender
#define Calender

#include "stm32l4xx_hal.h"
#include "ssd1306.h"
#include "main.h"
#include "string.h" 
#include "Uart_Process.h"
#include "Pages.h"
/*-----------------	DEFINE ---------------*/
#define RTC_BKP_DATE_TIME_UPDTATED ((uint32_t)0xFB0A)			//datanin daha once yazilip yazilmadigini kontrol etmek icin random bi key value

#define FALSE 0
#define TRUE  1



/*-----------------	EXTERN ---------------*/
extern RTC_HandleTypeDef hrtc;
extern RTC_DateTypeDef sdatestructureget;
extern RTC_TimeTypeDef stimestructureget;
extern uint32_t MyFlag;
extern RTC_TimeTypeDef sTime;
extern struct Page_IndVal_St			page[3];

/*-----------------	DECLARE ---------------*/


void RTC_TimeShow(uint8_t* showtime);
void RTC_DateShow(uint8_t* showtime);
void StartRTC_Routine(void);
void Give_RTC_Start_Values(void);
void Set_Alarm(void);
void Init_RTC(void);
#endif



