
#include"ssd1306.h"


// Databuffer voor het scherm
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Een scherm-object om lokaal in te werken
static SSD1306_t SSD1306;


//
//	Een byte sturen naar het commando register
//	Kan niet gebruikt worden buiten deze file
//
static void ssd1306_WriteCommand(uint8_t command)
{
	HAL_I2C_Mem_Write(&hi2c1,SSD1306_I2C_ADDR,0x00,1,&command,1,10);
}


//
//	Het scherm initialiseren voor gebruik
//
uint8_t ssd1306_Init(void)
{	
	// Even wachten zodat het scherm zeker opgestart is
	HAL_Delay(100);
	
	/* Init LCD */
	ssd1306_WriteCommand(0xAE); //display off
	ssd1306_WriteCommand(0x20); //Set Memory Addressing Mode   
	ssd1306_WriteCommand(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	ssd1306_WriteCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	ssd1306_WriteCommand(0xC8); //Set COM Output Scan Direction
	ssd1306_WriteCommand(0x00); //---set low column address
	ssd1306_WriteCommand(0x10); //---set high column address
	ssd1306_WriteCommand(0x40); //--set start line address
	ssd1306_WriteCommand(0x81); //--set contrast control register
	ssd1306_WriteCommand(0xFF);
	ssd1306_WriteCommand(0xA1); //--set segment re-map 0 to 127
	ssd1306_WriteCommand(0xA6); //--set normal display
	ssd1306_WriteCommand(0xA8); //--set multiplex ratio(1 to 64)
	ssd1306_WriteCommand(0x3F); //
	ssd1306_WriteCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	ssd1306_WriteCommand(0xD3); //-set display offset
	ssd1306_WriteCommand(0x00); //-not offset
	ssd1306_WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
	ssd1306_WriteCommand(0xF0); //--set divide ratio
	ssd1306_WriteCommand(0xD9); //--set pre-charge period
	ssd1306_WriteCommand(0x22); //
	ssd1306_WriteCommand(0xDA); //--set com pins hardware configuration
	ssd1306_WriteCommand(0x12);
	ssd1306_WriteCommand(0xDB); //--set vcomh
	ssd1306_WriteCommand(0x20); //0x20,0.77xVcc
	ssd1306_WriteCommand(0x8D); //--set DC-DC enable
	ssd1306_WriteCommand(0x14); //
	ssd1306_WriteCommand(0xAF); //--turn on SSD1306 panel
	
	/* Clearen scherm */
	ssd1306_Fill(Black);
	
	/* Update screen */
	ssd1306_UpdateScreen();
	
	/* Set default values */
	SSD1306.CurrentX = 0;
	SSD1306.CurrentY = 0;
	
	/* Initialized OK */
	SSD1306.Initialized = 1;
	
	/* Return OK */
	return 1;
}

//
//	We zetten de hele buffer op een bepaalde kleur
// 	color 	=> de kleur waarin alles moet
//
void ssd1306_Fill(SSD1306_COLOR color) 
{
	/* Set memory */
	uint32_t i;

	for(i = 0; i < sizeof(SSD1306_Buffer); i++)
	{
		SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
	}
}

//
//	Alle weizigingen in de buffer naar het scherm sturen
//
void ssd1306_UpdateScreen(void) 
{
	uint8_t i;
	
	for (i = 0; i < 8; i++) {
		ssd1306_WriteCommand(0xB0 + i);
		ssd1306_WriteCommand(0x00);
		ssd1306_WriteCommand(0x10);

		// We schrijven alles map per map weg
		HAL_I2C_Mem_Write(&hi2c1,SSD1306_I2C_ADDR,0x40,1,&SSD1306_Buffer[SSD1306_WIDTH * i],SSD1306_WIDTH,100);
	}
}

//
//	1 pixel op het scherm tekenen
//	X => X coordinaat
//	Y => Y coordinaat
//	color => kleur die pixel moet krijgen
//
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
{
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) 
	{
		// We gaan niet buiten het scherm schrijven
		return;
	}
	
	// Kijken of de pixel geinverteerd moet worden
	if (SSD1306.Inverted) 
	{
		color = (SSD1306_COLOR)!color;
	}
	
	// We zetten de juiste kleur voor de pixel
	if (color == White)
	{
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} 
	else 
	{
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

//
//	We willen 1 char naar het scherm sturen
//	ch 		=> char om weg te schrijven
//	Font 	=> Font waarmee we gaan schrijven
//	color 	=> Black or White
//
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color)
{
	uint32_t i, b, j;
	
	// Kijken of er nog plaats is op deze lijn
	if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font.FontWidth) ||
		SSD1306_HEIGHT <= (SSD1306.CurrentY + Font.FontHeight))
	{
		// Er is geen plaats meer
		return 0;
	}
	
	// We gaan door het font
	for (i = 0; i < Font.FontHeight; i++)
	{
		b = Font.data[(ch - 32) * Font.FontHeight + i];
		for (j = 0; j < Font.FontWidth; j++)
		{
			if ((b << j) & 0x8000) 
			{
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
			} 
			else 
			{
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
			}
		}
	}
	
	// De huidige positie is nu verplaatst
	SSD1306.CurrentX += Font.FontWidth;
	
	// We geven het geschreven char terug voor validatie
	return ch;
}

//
//	Functie voor het wegschrijven van een hele string
// 	str => string om op het scherm te schrijven
//	Font => Het font dat gebruikt moet worden
//	color => Black or White
//
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color)
{
	// We schrijven alle char tot een nulbyte
	while (*str) 
	{
		if (ssd1306_WriteChar(*str, Font, color) != *str)
		{
			// Het karakter is niet juist weggeschreven
			return *str;
		}
		
		// Volgende char
		str++;
	}
	
	// Alles gelukt, we sturen dus 0 terug
	return *str;
}

//
//	Zet de cursor op een coordinaat
//
void ssd1306_SetCursor(uint8_t x, uint8_t y) 
{
	/* Set write pointers */
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}





void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR c) 
{
	int16_t dx, dy, sx, sy, err, e2, i, tmp; 
	
	/* Check for overflow */
	if (x0 >= SSD1306_WIDTH) {
		x0 = SSD1306_WIDTH - 1;
	}
	if (x1 >= SSD1306_WIDTH) {
		x1 = SSD1306_WIDTH - 1;
	}
	if (y0 >= SSD1306_HEIGHT) {
		y0 = SSD1306_HEIGHT - 1;
	}
	if (y1 >= SSD1306_HEIGHT) {
		y1 = SSD1306_HEIGHT - 1;
	}
	
	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); 
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
	sx = (x0 < x1) ? 1 : -1; 
	sy = (y0 < y1) ? 1 : -1; 
	err = ((dx > dy) ? dx : -dy) / 2; 

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			ssd1306_DrawPixel(x0, i, c);
		}
		
		/* Return from function */
		return;
	}
	
	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			ssd1306_DrawPixel(i, y0, c);
		}
		
		/* Return from function */
		return;
	}
	
	while (1) {
		ssd1306_DrawPixel(x0, y0, c); 
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err; 
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		} 
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		} 
	}
}


  
void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR c) {
	/* Check input parameters */
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		/* Return error */
		return;
	}
	
	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}
	
	/* Draw 4 lines */
	SSD1306_DrawLine(x, y, x + w, y, c);         /* Top line */
	SSD1306_DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
	SSD1306_DrawLine(x, y, x, y + h, c);         /* Left line */
	SSD1306_DrawLine(x + w, y, x + w, y + h, c); /* Right line */
}







