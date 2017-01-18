#include "Uart_Process.h"

char Rx_data[2], Rx_Buffer[50];
unsigned char UartBufferComplete_Flag;


/*	1Byte data geldiginde,UART HAL library nin cagirdigi fonksiyon...
	/r gelene kadar Rx_Buffer doldurulur.	*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t i;
	static char Rx_Index;
	
	if(huart->Instance == USART3)
	{
		if(Rx_Index == 0)
		{
			/*clear Rx_Buffer before receiving new data	*/
			for (i=0;i<100;i++)
				Rx_Buffer[i]=0;																
		}
		if(Rx_data[0] != 13 && Rx_Index <50)							
		{
			Rx_Buffer[Rx_Index++] = Rx_data[0];
		}
		else
		{
			Rx_Index = 0;
			UartBufferComplete_Flag = 1;
			
			/*Data alimi tamamlandiginda Buffera gelen datayi anlamlandir */
			Proceed_Commands();
		}
		HAL_UART_Receive_IT(&huart3,(uint8_t*)Rx_data,1);
		
	}
}

/*	Gelen komuta gore islem yapar*/
void Proceed_Commands(void)
{
		if(UartBufferComplete_Flag == 1)
		{
			UartBufferComplete_Flag = 0;
			
			/*	Handshake	*/
			if(Rx_Buffer[0] == 'C')
			{	
				printf("X");
			}
			/*Change Request geldi*/
			else if(Rx_Buffer[0] == 'c' && Rx_Buffer[1] == 'h')
			{	
				printf("STM: Thnx PC Bro,I Got It\n");									
				/*Change Ok bildirimi gonder*/
				printf("chOk");	
				Update_Time();
				ssd1306_UpdateScreen();
			}

		}
}

/*	Rx_Buffer dan Datalar parselleniyor	*/
void Update_Time(void)
{
	static char TimeBuffer[30];
	int count = 0;
	/*************	DATE	*************/
	sdatestructureget.Date 		= (Rx_Buffer[10] - 48) * 10 + (Rx_Buffer[11] - 48);
	sdatestructureget.Year 		= (Rx_Buffer[24] - 48) * 10 + (Rx_Buffer[25] - 48);
	
	/*Rx_Bufferdan TimeBuffera 2. indexten 3byte kopyala... Bu PC den gelen stringde Gun bilgisi iceriyor */
	GrabDataFromBuffer(TimeBuffer,Rx_Buffer,Start_From_Day_Index,3);
	for(count = 0; count<7; count++)
	{
		/*Days stringdeki datalar ile bufferdaki datalari karsilastir. Dogru cikan count datasi gun bilgisini icerir ve struct guncellenir*/
		if(strcmp((Days[count]), TimeBuffer) == 0)
		{
			sdatestructureget.WeekDay = count;			
			break;
		}
	}	
	
	GrabDataFromBuffer(TimeBuffer,Rx_Buffer,Start_From_Month_Index,3);
	for(count = 0; count<12; count++)
	{
		if(strcmp((Month[count]), TimeBuffer) == 0)
		{
			sdatestructureget.Month = count + 1;
			break;
		}
	}		
	
	/*************	TIME	*************/
	stimestructureget.Hours 	= (Rx_Buffer[13] - 48) * 10 + (Rx_Buffer[14] - 48);
	stimestructureget.Minutes = (Rx_Buffer[16] - 48) * 10 + (Rx_Buffer[17] - 48);
	stimestructureget.Seconds = (Rx_Buffer[19] - 48) * 10 + (Rx_Buffer[20] - 48);
	
	HAL_RTC_SetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
}

/*Source_Buffer dan Destination_Buffera BufferIndex den baslanarak Lenght kadar data kopyalanir */
void GrabDataFromBuffer(char *Destination_Buffer,char *Source_Buffer,uint16_t BufferIndex, uint16_t Lenght)
{
	memcpy(Destination_Buffer, (Source_Buffer + BufferIndex), Lenght);
}



