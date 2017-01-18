
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef Uart_Process
#define Uart_Process

#include "stm32l4xx_hal.h"
#include "main.h"
#include "string.h" 
#include "calender.h"  
#include <stdlib.h>

//#include "ssd1306.h"  

#define FALSE 0

#define Start_From_Day_Index			2					//RX Buffer'in Day bilgisi iceren Indexi
#define Start_From_Month_Index		6					//RX Buffer'in Month bilgisi iceren Indexi

		
extern char Rx_Buffer[50];
extern char *Days[];
extern char *Month[];

extern RTC_TimeTypeDef sTime;
extern UART_HandleTypeDef huart3;

void Proceed_Commands(void);
void Update_Time(void);
void GrabDataFromBuffer(char *Destination_Buffer,char *Source_Buffer,uint16_t BufferIndex, uint16_t Lenght);
void testfunc(void);
#endif

