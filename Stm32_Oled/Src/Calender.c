#include "calender.h"  

char RTC_Day[7];
char *Days[] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
char *Month[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

RTC_AlarmTypeDef sAlarm;

void StartRTC_Routine(void)
{
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_RTC_ENABLE();
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
	
	/* RTC Backup registera daha once birsey yazilip yazilmadigini kontrol eder.
		 Eger yazilmis ise,RTC saat ve tarih bilgilerinin guncel oldugu kabul edilir ve baslangic icin default degerler yuklenmez */
	if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR0) != RTC_BKP_DATE_TIME_UPDTATED)				
	{
		/*RTC Init et*/
		Init_RTC();
		/*RTC date ve time bilgilerine baslangic icin bir default bilgi ver*/
		HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,RTC_BKP_DATE_TIME_UPDTATED);					//yazildiysa RTC nin baslangic saat tarih ayarlarini yap
	}
	else
	{
		/* Check if the Power On Reset flag is set */
		if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
		{
			MyFlag = 0xBA;
		}
		/* Check if the Soft Reset flag is set */
		if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)
		{
			MyFlag = 0xFB;
		}		
	}
}



void RTC_TimeShow(uint8_t* showtime)
{  
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
	
  /* Display time Format : hh:mm:ss */
  sprintf((char*)showtime,"%02d:%02d:%02d",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
		
	ssd1306_SetCursor(20,20);
	ssd1306_WriteString((char*)showtime,Font_11x18,White);
	ssd1306_UpdateScreen();
} 



void RTC_DateShow(uint8_t* showtime)
{  
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
	
	/* sdatestructureget.WeekDay 01 ile baslayip 07 ile bitiyor. Bu yuzden gercek gun datasi sdatestructureget.WeekDay -1 olarak gosterilir*/
	strcpy(RTC_Day,*(Days +(sdatestructureget.WeekDay)));		
	ssd1306_SetCursor(0,0);
	ssd1306_WriteString(RTC_Day,Font_7x10,White);
	
	sprintf((char*)showtime,"%02d/%02d/%d",sdatestructureget.Date,sdatestructureget.Month,sdatestructureget.Year);		
	ssd1306_SetCursor(65,0);
	ssd1306_WriteString((char*)showtime,Font_7x10,White);		

}

/*	Initial Values of RTC	**/
void Give_RTC_Start_Values(void)
{
    /**	Initialize RTC and set the Time and Date  **/
  stimestructureget.Hours = 0;
  stimestructureget.Minutes = 5;
  stimestructureget.Seconds = 0;
  stimestructureget.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  stimestructureget.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  sdatestructureget.WeekDay = RTC_WEEKDAY_MONDAY;
  sdatestructureget.Month = RTC_MONTH_JANUARY;
  sdatestructureget.Date = 16;
  sdatestructureget.Year = 17;

  if (HAL_RTC_SetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
}



void Init_RTC(void)
{
    /**Initialize RTC Only **/
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
}

void Set_Alarm(void)
{
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
	
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Turn LED2 on: Alarm generation */
		ssd1306_SetCursor(35 	,50);
		ssd1306_WriteString("Alarm On ",Font_7x10,White);			
}


