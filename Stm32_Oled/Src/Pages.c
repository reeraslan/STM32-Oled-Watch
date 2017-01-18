#include "Pages.h"

uint8_t Enable_Alarm_Flag = 0;
uint8_t Disable_Alarm_Flag = 0;

ButtonState_St 						ButtonState     = {0};
PageState_St   						PageState		 		= {0};
struct Page_IndVal_St			page[4]					= {0};
struct SubPage_IndVal_St	Subpage[4]			= {0}; 

/*
  * @brief Routes corresponding pages 	
  * @retval None
  */
void Main_Page_Process(void)
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

	/** page transitions  **/
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

void Sub_Page_Process(void)
{

	/** page transitions  **/
	switch(PageState.SubPage_Number)
	{
		case 1:
		{
			SubPage_1();
			break;
		}
		case 2:
		{
			SubPage_2();
			break;
		}
		case 3:
		{
			SubPage_3();
			break;
		}
		default:
			break;
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
	CheckLimitPages(page , 2);
	/*Indicates which parameter are you changing*/
	Indicate_Parameter_Page(page,2);
	ssd1306_UpdateScreen();	
	
	/**	Objects of Page Structure ara transferred to Alarm Struct	**/
	sAlarm.AlarmTime.Hours  	  	= page[2].ParameterValue[0];
	sAlarm.AlarmTime.Minutes  		= page[2].ParameterValue[1];
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
	if(ButtonState.PressedState == ShortPressed)
	{
		ButtonState.PressedState = NotPressed;
		page[3].ParameterValue[(page[3].ParameterIndex)]++;
		PageState.Page_Changed = TRUE;
	}
	/**	in case of LongPressed event , skip next parameters **/
	if(ButtonState.PressedState == LongPressed)
	{
		ButtonState.PressedState = NotPressed;
		page[3].ParameterIndex++;	
	}	
	CheckLimitPages(page,3);
	Indicate_Parameter_Page(page,3);
	ssd1306_SetCursor(20,20);
	ssd1306_WriteString("Clock Settings",Font_7x10,White);
	ssd1306_SetCursor(20,32);
	ssd1306_WriteString("Date Settings",Font_7x10,White);	
	ssd1306_UpdateScreen();
	Proceed_Commands();	
	
	//**	Set SubPage	**//
	if(page[3].ParameterValue[0])	
	{
		page[3].ParameterValue[0] = 0;
		PageState.Sub_Page_Route_Status = ENABLE;
		PageState.Main_Page_Route_Status = DISABLE;
		PageState.Page_Changed = TRUE;
		PageState.SubPage_Number = 1;
	}
	else if (page[3].ParameterValue[1])
	{
		page[3].ParameterValue[1] = 0;
		PageState.Sub_Page_Route_Status = ENABLE;
		PageState.Main_Page_Route_Status = DISABLE;
		PageState.Page_Changed = TRUE;
		PageState.SubPage_Number = 2;
	}		

}
/**	Time Settings Page	**/
void SubPage_1(void)
{
	char TempBuf[19] = {0};
	static uint8_t Enable_Time_Change = 0;
	static uint8_t Disable_Time_Change = 0;
	
	if(PageState.Page_Changed == TRUE){
			ssd1306_SetCursor(20,40);
			ssd1306_WriteString("Change Time",Font_7x10,White);		
			PageState.Page_Changed = FALSE;
			ssd1306_Fill(Black);
			ssd1306_UpdateScreen();
	}
	
	ssd1306_SetCursor(25,0);
	ssd1306_WriteString("Change Time",Font_7x10,White);		
	
	/**	in case of ShortPressed event , increase instant parameters value	**/
	
	if(ButtonState.PressedState == ShortPressed)
	{
		ButtonState.PressedState = NotPressed;
		Subpage[1].ParameterValue[(Subpage[1].ParameterIndex)]++;
	}
	/**	in case of LongPressed event , skip next parameters **/
	if(ButtonState.PressedState == LongPressed)
	{
		ButtonState.PressedState = NotPressed;
		Subpage[1].ParameterIndex++;	
	}	
	Indicate_Parameter_SubPage(Subpage,1);
	sprintf((char*)TempBuf,"%02d:%02d:%02d",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
	ssd1306_SetCursor(20,20);
	ssd1306_WriteString((char*)TempBuf,Font_11x18,White);			
	ssd1306_SetCursor(20,48);
	ssd1306_WriteString("Set  Disable",Font_7x10,White);
	
	ssd1306_UpdateScreen();	
	
	/**	Objects of Page Structure ara transferred to Alarm Struct	**/
	stimestructureget.Hours 	  	= Subpage[1].ParameterValue[0];
	stimestructureget.Minutes  		= Subpage[1].ParameterValue[1];
	stimestructureget.Seconds	   	= Subpage[1].ParameterValue[2];
	
	Enable_Time_Change  					= Subpage[1].ParameterValue[3];
	Disable_Time_Change 					= Subpage[1].ParameterValue[4];
	
	/**	shows the string of alarm **/
	Proceed_Commands();
	
	/**	Cancel the alarm and turn back	**/
		if(Enable_Time_Change)
		{
			HAL_RTC_SetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
			PageState.Sub_Page_Route_Status = DISABLE;
			PageState.Main_Page_Route_Status = ENABLE;            
			PageState.MainPage_Number = 0;
			PageState.SubPage_Number = 0;
			PageState.Page_Changed = TRUE;    
			Subpage[2].ParameterValue[3] = 0;
			Subpage[2].ParameterIndex = 0;
          
		}
		/**	Set the alarm up and turn back	**/
		if(Disable_Time_Change)
		{
			PageState.Sub_Page_Route_Status = DISABLE;
			PageState.Main_Page_Route_Status = ENABLE;            
			PageState.MainPage_Number = 0;
			PageState.SubPage_Number = 0;
			PageState.Page_Changed = TRUE;    
			Subpage[2].ParameterValue[4] = 0;
			Subpage[2].ParameterIndex = 0;

		}	
}


void SubPage_2(void)
{
	if(PageState.Page_Changed == TRUE){
		PageState.Page_Changed = FALSE;
		ssd1306_Fill(Black);
		ssd1306_UpdateScreen();
	}
		ssd1306_SetCursor(30,30);
		ssd1306_WriteString("SUB2",Font_7x10,White);
		ssd1306_UpdateScreen();	
		if(ButtonState.PressedState == ShortPressed)
		{
			PageState.Sub_Page_Route_Status = DISABLE;
			PageState.Main_Page_Route_Status = ENABLE;			
			PageState.MainPage_Number = 0;
			PageState.SubPage_Number = 0;
			PageState.Page_Changed = TRUE;	
		}	
}
void SubPage_3(void)
{}
	

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
void CheckLimitPages(struct Page_IndVal_St Page[] , uint16_t Page_Number)
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
			if(page[3].ParameterIndex > 1) 					page[3].ParameterIndex = 0;				
			break;
		}		
	}
}



void Indicate_Parameter_SubPage(struct SubPage_IndVal_St SubPage[] , uint16_t Page_Number)
{
	switch( Page_Number )
	{
		case 1:
		{			
			if(SubPage[1].ParameterIndex == 0)			{SSD1306_DrawLine(55,60,102,60,Black);			  SSD1306_DrawLine(20,38,39,38,White);}
			else if(SubPage[1].ParameterIndex == 1)	{SSD1306_DrawLine(20,38,39,38,Black);					SSD1306_DrawLine(54,38,73,38,White);}
			else if(SubPage[1].ParameterIndex == 2)	{SSD1306_DrawLine(54,38,73,38,Black);					SSD1306_DrawLine(87,38,105,38,White);}
			else if(SubPage[1].ParameterIndex == 3)	{SSD1306_DrawLine(87,38,105,38,Black);				SSD1306_DrawLine(20,60,40,60,White);}
			else if(SubPage[1].ParameterIndex == 4)	{SSD1306_DrawLine(20,60,40,60,Black);					SSD1306_DrawLine(55,60,102,60,White);}
			break;
		}
	}
}


/*
  * @brief Indicate_Parameter_Page
  * @note The maximum position of the corresponding page's parameters 
						is shown
  * @retval None
  */
void Indicate_Parameter_Page(struct Page_IndVal_St Page[] , uint16_t Page_Number)
{
	switch( Page_Number )
	{
		case 2:
		{			
			if(page[2].ParameterIndex == 0)				{SSD1306_DrawLine(55,60,102,60,Black);			SSD1306_DrawLine(35,40,54,40,White);}
			else if(page[2].ParameterIndex == 1)	{SSD1306_DrawLine(35,40,54,40,Black);				SSD1306_DrawLine(68,40,87,40,White);}
			else if(page[2].ParameterIndex == 2)	{SSD1306_DrawLine(68,40,87,40,Black);				SSD1306_DrawLine(20,60,40,60,White);}
			else if(page[2].ParameterIndex == 3)	{SSD1306_DrawLine(20,60,40,60,Black);				SSD1306_DrawLine(55,60,102,60,White);}
			break;
		}
		case 3:
		{
			if(page[3].ParameterIndex == 0)			{SSD1306_DrawLine(12,35,18,35,Black);				SSD1306_DrawLine(12,25,18,25,White);}
			else if(page[3].ParameterIndex == 1)	{SSD1306_DrawLine(12,25,18,25,Black);			SSD1306_DrawLine(12,35,18,35,White);}
			break;
		}
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
            ButtonState.PressedState = usTick > 400 ? LongPressed : ShortPressed;
            HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
        }
        usTick = 0;
    }
}
