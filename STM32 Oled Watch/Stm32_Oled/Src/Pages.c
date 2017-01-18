#include "Pages.h"

uint8_t Enable_Alarm_Flag = 0;
uint8_t Disable_Alarm_Flag = 0;

ButtonState_St 						ButtonState     = {0};
PageState_St   						PageState		 		= {0};
struct Page_IndVal_St			page[3]					= {0};


/*
  * @brief Routes corresponding pages 	
  * @retval None
  */
void Page_Process(void)
{
	if(ButtonState.PressedState == DoubleTapped)
	{
		ButtonState.PressedState = NotPressed; 
		PageState.MainPage_Number++;
		PageState.Page_Changed = TRUE;
		if(PageState.MainPage_Number > 2)
		{
			PageState.MainPage_Number = 0;
		}
	}

	/** page transitions **/
	switch(PageState.MainPage_Number)
	{
		case 0:
		{
			PageScreen_1();
			break;
		}
		case 1:
		{
			PageScreen_2();
			break;
		}
		case 2:
		{
			PageScreen_3();
			break;
		}
		default:
			PageScreen_1();
	}
}

/*
  * @brief Showing 1st page
  * @note Shows the instant hour&calender data
	*				And warns on screen when the alarm goes off
  * @retval None
  */
void PageScreen_1(void)
{
	uint8_t aShowTime[20] = {0};
	char tempbuffer[20] = {0};
	if(PageState.Page_Changed == TRUE){
		PageState.Page_Changed = FALSE;
		ssd1306_Fill(Black);
		
		/**	when alarm is set, shows the alarm time	**/
		if(Enable_Alarm_Flag)
		{
			sprintf((char*)tempbuffer,"%02d:%02d:%02d",sAlarm.AlarmTime.Hours,sAlarm.AlarmTime.Minutes,sAlarm.AlarmTime.Seconds);		
			ssd1306_SetCursor(35,40);
			ssd1306_WriteString((char*)tempbuffer,Font_7x10,White);
			ssd1306_UpdateScreen();	
		}
	}
	/**	show clock&calender data on screen	**/
	RTC_TimeShow(aShowTime);
	RTC_DateShow(aShowTime);
	Proceed_Commands();
}

/*
  * @brief Showing 2nd page
  * @note When user wants to set or disable the alarm,
	*				that needs to switch the 2nd page of clock by double tap
  * @note This function sets the alarm parameters with data from the user 
  * @retval None
  */
void PageScreen_2(void)
{
	/**	in case of ShortPressed event , increase instant parameters value	**/
	if(ButtonState.PressedState == ShortPressed)
	{
		ButtonState.PressedState = NotPressed;
		page[2].ParameterValue[(page[2].ParameterIndex)]++;
	}
	/**	in case of LongPressed event , skip next parameters **/
	if(ButtonState.PressedState == LongPressed)
	{
		ButtonState.PressedState = NotPressed;
		page[2].ParameterIndex++;	
	}	
	
	/*Check the limits of parameters*/
	CheckLimit(page , 2);
	/*Indicates which parameter are you changing*/
	Indicate_Parameter(&(page[2].ParameterIndex));
	ssd1306_UpdateScreen();	
	
	/**	Objects of Page Structure ara transferred to Alarm Struct	**/
	sAlarm.AlarmTime.Hours  	  = page[2].ParameterValue[0];
	sAlarm.AlarmTime.Minutes  	= page[2].ParameterValue[1];
	sAlarm.AlarmTime.Seconds		= 0;	
	
	Enable_Alarm_Flag  					= page[2].ParameterValue[2];
	Disable_Alarm_Flag 					= page[2].ParameterValue[3];
	
	/**	shows the string of alarm **/
	ShowAlarm();
	Proceed_Commands();
	
	/**	Cancel the alarm and turn back	**/
		if(Disable_Alarm_Flag)
		{
			page[2].ParameterValue[3] = 0;
			page[2].ParameterIndex = 0;
			PageState.MainPage_Number = 0;
			PageState.Page_Changed = TRUE;		
		}
		/**	Set the alarm up and turn back	**/
		if(Enable_Alarm_Flag)
		{
			Set_Alarm();
			ssd1306_Fill(Black);
			ssd1306_UpdateScreen();
			page[2].ParameterValue[2] = 0;
			page[2].ParameterIndex = 0;
			PageState.MainPage_Number = 0;
			PageState.Page_Changed = TRUE;		
		}			
}


/**	Useless Now **/
void PageScreen_3(void)
{
	if(PageState.Page_Changed == TRUE){
		PageState.Page_Changed = FALSE;
		ssd1306_Fill(Black);
		ssd1306_UpdateScreen();
	}

	ssd1306_SetCursor(10,20);
	ssd1306_WriteString("HELLO WLRD",Font_11x18,White);
	ssd1306_UpdateScreen();
	Proceed_Commands();
}

/*
  * @brief Shows the string of alarm 
  * @retval None
  */
void ShowAlarm(void)
{
	uint8_t ShowAlarm_String[20] = {0};
	
	if(PageState.Page_Changed == TRUE){
		PageState.Page_Changed = FALSE;
		ssd1306_Fill(Black);
	}
	ssd1306_SetCursor(30,02);
	ssd1306_WriteString("Set Alarm",Font_7x10,White);
	
	sprintf((char*)ShowAlarm_String,"%02d:%02d",sAlarm.AlarmTime.Hours,sAlarm.AlarmTime.Minutes);		
	ssd1306_SetCursor(35,20);
	ssd1306_WriteString((char*)ShowAlarm_String,Font_11x18,White);	
	
	ssd1306_SetCursor(20,48);
	ssd1306_WriteString("Set  Disable",Font_7x10,White);
	ssd1306_UpdateScreen();
}


/*
  * @brief Limit Checker
  * @note The maximum value of the corresponding page's parameters 
						is checked and corrected by this function
	*				
  * @retval None
  */
void CheckLimit(struct Page_IndVal_St Page[] , uint16_t Page_Number)
{
	switch( Page_Number )
	{
		/**	Limit values of 2nd Page	**/
		case 2:
		{
			if(page[2].ParameterIndex > 3) 						page[2].ParameterIndex = 0;
			if(page[2].ParameterValue[0] > 23)				page[2].ParameterValue[0] = 0;
			if(page[2].ParameterValue[1] > 59)				page[2].ParameterValue[1] = 0;				
			break;
		}
		/**	3rd page **/
		case 3:
		{
			// will be added
			break;
		}		
	}
}

/*
  * @brief Indicate_Parameter
  * @note The maximum position of the corresponding page's parameters 
						is shown
  * @retval None
  */
void Indicate_Parameter(uint32_t* IndexOfParameters)
{
	switch(*IndexOfParameters)
	{
		case 0:
			SSD1306_DrawLine(55,60,102,60,Black);
			SSD1306_DrawLine(35,40,54,40,White);
			break;
		case 1:
			SSD1306_DrawLine(35,40,54,40,Black);
			SSD1306_DrawLine(68,40,87,40,White);
			break;
		case 2:
			SSD1306_DrawLine(68,40,87,40,Black);
			SSD1306_DrawLine(20,60,40,60,White);

			break;
		case 3:
			SSD1306_DrawLine(20,60,40,60,Black);
			SSD1306_DrawLine(55,60,102,60,White);
			break;		
	}
}


/*
  * @brief Timer Interrupt Routine
  * @note  The button state is determined by counting pushing and
	*				 releasing timings
	*	
	*	@note  3 states can be occurred 
						- DoubleTap
						- Short Press
						- Long Press
  * @retval None
  */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint32_t usTick;
	static uint32_t HighCounter;
	static uint32_t LowCounter;
	static uint32_t DT_Start;
	
    // Button pressed
    if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
    {
        usTick++;
				HighCounter++;
				DT_Start = 1;
				if(HighCounter > 50 && LowCounter > 50){
					ButtonState.PressedState = DoubleTapped;
					HighCounter = 0;
					LowCounter = 0;
				}
    }
    else
    {
        // Not pressed
				if (DT_Start == 1)
					LowCounter++;
				if(LowCounter > 100)
				{
					HighCounter = 0;
					DT_Start = 0;
					LowCounter = 0;
				}
        if( usTick > 150 )
        {
            ButtonState.PressedState = usTick > 500 ? LongPressed : ShortPressed;
            HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
        }
        usTick = 0;
    }
}
