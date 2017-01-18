
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef Pages
#define Pages

#include "stm32l4xx_hal.h"
#include "main.h"
#include "Uart_Process.h"
#include "Calender.h"

#define FALSE 0

#define	PageParameter(x) 	Page[x].ParameterIndex

/*	Butonun durum yapisi	*/
typedef struct {
	uint8_t PressedState;
} ButtonState_St;

/*	Sayfanin durum structure yapisi	*/
typedef struct {
	uint8_t MainPage_Number;
	uint8_t SubPage_Number;
	uint8_t Page_Changed;
	uint8_t Main_Page_Route_Status;
	uint8_t Sub_Page_Route_Status;
} PageState_St;


/*	Sayfalarin Parametre Degeri & Parametre Index yapisi*/
struct Page_IndVal_St{
	uint32_t ParameterIndex;
	uint32_t ParameterValue[5];
};

struct SubPage_IndVal_St{
	uint32_t ParameterIndex;
	uint32_t ParameterValue[5];
};

/* Alarm Parametre tanimi	*/
typedef struct {
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Enable;
	uint8_t Disable;
} Alarm_St;

typedef enum {
	NotPressed    = 0x00,
	LongPressed   = 0x01,
	ShortPressed  = 0x02,
	DoubleTapped  = 0x03
} ButtonPressedStatus;

typedef enum {
	Disable   = 0,
	Enable    = 1
} status;


extern RTC_AlarmTypeDef sAlarm;
/** On tanimlar	**/
#define Start_From_Day_Index			2					//RX Buffer'in Day bilgisi iceren Indexi
#define Start_From_Month_Index		6					//RX Buffer'in Month bilgisi iceren Indexi
#define aaa page[3].ParameterValue[1] 

/**	Fonksiyon tanimlanmas1	**/
void Main_Page_Process(void);
void Check_Button_State(void);
void Indicate_Parameter_Page(struct Page_IndVal_St Page[] , uint16_t Page_Number);
void Indicate_Parameter_SubPage(struct SubPage_IndVal_St SubPage[] , uint16_t Page_Number);
void ShowAlarm(void);
void CheckLimitPages(struct Page_IndVal_St Page[] , uint16_t Page_Number);
void CheckLimitSubPages(struct SubPage_IndVal_St SubPage[] , uint16_t Page_Number);
void ShowSubPage(void);
void PageScreen_1(void);
void PageScreen_2(void);
void PageScreen_3(void);
void SubPage_1(void);
void SubPage_2(void);
void SubPage_3(void);
void Sub_Page_Process(void);
#endif
