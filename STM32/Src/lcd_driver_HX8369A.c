#include "settings.h"
#if defined(LCD_HX8369A)

// Header files
#include "fonts.h"
#include "functions.h"
#include "lcd_driver.h"
#include "lcd_driver_HX8369A.h"
#include "main.h"

#define _swap_int16_t(a, b) \
	do {                      \
		int16_t t = a;          \
		a = b;                  \
		b = t;                  \
	} while (0)

//***** Functions prototypes *****//
static inline void LCDDriver_SetCursorPosition(uint16_t x, uint16_t y);
inline void LCDDriver_SetCursorAreaPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

// Write command to LCD
inline static void LCDDriver_SendCommand(uint16_t com) { *(__IO uint16_t *)((uint32_t)(LCD_FSMC_COMM_ADDR)) = com; }

// Write data to LCD
inline static void LCDDriver_SendData(uint16_t data) { *(__IO uint16_t *)((uint32_t)(LCD_FSMC_DATA_ADDR)) = data; }

// Write data to LCD
inline void LCDDriver_SendData16(uint16_t data) { *(__IO uint16_t *)((uint32_t)(LCD_FSMC_DATA_ADDR)) = data; }

// Write pair command-data
inline static void LCDDriver_writeReg(uint16_t reg, uint16_t val) {
	LCDDriver_SendCommand(reg);
	LCDDriver_SendData(val);
}

// Read command from LCD
inline uint16_t LCDDriver_ReadStatus(void) { return *(__IO uint16_t *)((uint32_t)(LCD_FSMC_COMM_ADDR)); }
// Read data from LCD
inline uint16_t LCDDriver_ReadData(void) { return *(__IO uint16_t *)((uint32_t)(LCD_FSMC_DATA_ADDR)); }

// Read Register
inline uint16_t LCDDriver_readReg(uint16_t reg) {
	LCDDriver_SendCommand(reg);
	return LCDDriver_ReadData();
}

// Initialise function
void LCDDriver_Init(void) {
	HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_SET);

	HAL_Delay(20);
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(100);

	LCDDriver_SendCommand(0x2D);
	for (int i = 0; i <= 63; i++) {
		LCDDriver_SendData(i * 8);
	}

	for (int i = 0; i <= 63; i++) {
		LCDDriver_SendData(i * 4);
	}

	for (int i = 0; i <= 63; i++) {
		LCDDriver_SendData(i * 8);
	}

	LCDDriver_SendCommand(0xB9); // Set_EXTC
	LCDDriver_SendData(0xFF);
	LCDDriver_SendData(0x83);
	LCDDriver_SendData(0x69);

	LCDDriver_SendCommand(0xB1); // Set Power
	LCDDriver_SendData(0x85);
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0x34);
	LCDDriver_SendData(0x0A);
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0x0F);
	LCDDriver_SendData(0x0F);
	LCDDriver_SendData(0x2A);
	LCDDriver_SendData(0x32);
	LCDDriver_SendData(0x3F);
	LCDDriver_SendData(0x3F);
	LCDDriver_SendData(0x01); // update VBIAS
	LCDDriver_SendData(0x23);
	LCDDriver_SendData(0x01);
	LCDDriver_SendData(0xE6);
	LCDDriver_SendData(0xE6);
	LCDDriver_SendData(0xE6);
	LCDDriver_SendData(0xE6);
	LCDDriver_SendData(0xE6);

	LCDDriver_SendCommand(0xB2); // SET Display 480x800
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0x20);
	LCDDriver_SendData(0x0A);
	LCDDriver_SendData(0x0A);
	LCDDriver_SendData(0x70);
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0xFF);
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0x03);
	LCDDriver_SendData(0x03);
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0x01);

	LCDDriver_SendCommand(0xB4); // SET Display 480x800
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0x18);
	LCDDriver_SendData(0x80);
	LCDDriver_SendData(0x10);
	LCDDriver_SendData(0x01);
	LCDDriver_SendCommand(0xB6); // SET VCOM
	LCDDriver_SendData(0x2C);
	LCDDriver_SendData(0x2C);

	LCDDriver_SendCommand(0xD5); // SET GIP
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0x05);
	LCDDriver_SendData(0x03);
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0x01);
	LCDDriver_SendData(0x09);
	LCDDriver_SendData(0x10);
	LCDDriver_SendData(0x80);
	LCDDriver_SendData(0x37);
	LCDDriver_SendData(0x37);
	LCDDriver_SendData(0x20);
	LCDDriver_SendData(0x31);
	LCDDriver_SendData(0x46);
	LCDDriver_SendData(0x8A);
	LCDDriver_SendData(0x57);
	LCDDriver_SendData(0x9B);
	LCDDriver_SendData(0x20);
	LCDDriver_SendData(0x31);
	LCDDriver_SendData(0x46);
	LCDDriver_SendData(0x8A);
	LCDDriver_SendData(0x57);
	LCDDriver_SendData(0x9B);
	LCDDriver_SendData(0x07);
	LCDDriver_SendData(0x0F);
	LCDDriver_SendData(0x02);
	LCDDriver_SendData(0x00);
	LCDDriver_SendCommand(0xE0); // SET GAMMA
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0x08);
	LCDDriver_SendData(0x0D);
	LCDDriver_SendData(0x2D);
	LCDDriver_SendData(0x34);
	LCDDriver_SendData(0x3F);
	LCDDriver_SendData(0x19);
	LCDDriver_SendData(0x38);
	LCDDriver_SendData(0x09);
	LCDDriver_SendData(0x0E);
	LCDDriver_SendData(0x0E);
	LCDDriver_SendData(0x12);
	LCDDriver_SendData(0x14);
	LCDDriver_SendData(0x12);
	LCDDriver_SendData(0x14);
	LCDDriver_SendData(0x13);
	LCDDriver_SendData(0x19);
	LCDDriver_SendData(0x00);
	LCDDriver_SendData(0x08);

	LCDDriver_SendData(0x0D);
	LCDDriver_SendData(0x2D);
	LCDDriver_SendData(0x34);
	LCDDriver_SendData(0x3F);
	LCDDriver_SendData(0x19);
	LCDDriver_SendData(0x38);
	LCDDriver_SendData(0x09);
	LCDDriver_SendData(0x0E);
	LCDDriver_SendData(0x0E);
	LCDDriver_SendData(0x12);
	LCDDriver_SendData(0x14);
	LCDDriver_SendData(0x12);
	LCDDriver_SendData(0x14);
	LCDDriver_SendData(0x13);
	LCDDriver_SendData(0x19);
	LCDDriver_SendCommand(0xC1); // set DGC
	LCDDriver_SendData(0x01);    // enable DGC function
	LCDDriver_SendData(0x02);    // SET R-GAMMA
	LCDDriver_SendData(0x08);
	LCDDriver_SendData(0x12);
	LCDDriver_SendData(0x1A);
	LCDDriver_SendData(0x22);
	LCDDriver_SendData(0x2A);
	LCDDriver_SendData(0x31);
	LCDDriver_SendData(0x36);
	LCDDriver_SendData(0x3F);
	LCDDriver_SendData(0x48);
	LCDDriver_SendData(0x51);
	LCDDriver_SendData(0x58);
	LCDDriver_SendData(0x60);
	LCDDriver_SendData(0x68);
	LCDDriver_SendData(0x70);
	LCDDriver_SendData(0x78);
	LCDDriver_SendData(0x80);
	LCDDriver_SendData(0x88);
	LCDDriver_SendData(0x90);
	LCDDriver_SendData(0x98);
	LCDDriver_SendData(0xA0);
	LCDDriver_SendData(0xA7);
	LCDDriver_SendData(0xAF);
	LCDDriver_SendData(0xB6);
	LCDDriver_SendData(0xBE);
	LCDDriver_SendData(0xC7);
	LCDDriver_SendData(0xCE);
	LCDDriver_SendData(0xD6);
	LCDDriver_SendData(0xDE);
	LCDDriver_SendData(0xE6);
	LCDDriver_SendData(0xEF);
	LCDDriver_SendData(0xF5);
	LCDDriver_SendData(0xFB);
	LCDDriver_SendData(0xFC);
	LCDDriver_SendData(0xFE);
	LCDDriver_SendData(0x8C);
	LCDDriver_SendData(0xA4);
	LCDDriver_SendData(0x19);
	LCDDriver_SendData(0xEC);
	LCDDriver_SendData(0x1B);
	LCDDriver_SendData(0x4C);

	LCDDriver_SendData(0x40);
	LCDDriver_SendData(0x02); // SET G-Gamma
	LCDDriver_SendData(0x08);
	LCDDriver_SendData(0x12);
	LCDDriver_SendData(0x1A);
	LCDDriver_SendData(0x22);
	LCDDriver_SendData(0x2A);
	LCDDriver_SendData(0x31);
	LCDDriver_SendData(0x36);
	LCDDriver_SendData(0x3F);
	LCDDriver_SendData(0x48);
	LCDDriver_SendData(0x51);
	LCDDriver_SendData(0x58);
	LCDDriver_SendData(0x60);
	LCDDriver_SendData(0x68);
	LCDDriver_SendData(0x70);
	LCDDriver_SendData(0x78);
	LCDDriver_SendData(0x80);
	LCDDriver_SendData(0x88);
	LCDDriver_SendData(0x90);
	LCDDriver_SendData(0x98);
	LCDDriver_SendData(0xA0);
	LCDDriver_SendData(0xA7);
	LCDDriver_SendData(0xAF);
	LCDDriver_SendData(0xB6);
	LCDDriver_SendData(0xBE);
	LCDDriver_SendData(0xC7);
	LCDDriver_SendData(0xCE);
	LCDDriver_SendData(0xD6);
	LCDDriver_SendData(0xDE);
	LCDDriver_SendData(0xE6);
	LCDDriver_SendData(0xEF);
	LCDDriver_SendData(0xF5);
	LCDDriver_SendData(0xFB);
	LCDDriver_SendData(0xFC);
	LCDDriver_SendData(0xFE);
	LCDDriver_SendData(0x8C);
	LCDDriver_SendData(0xA4);
	LCDDriver_SendData(0x19);
	LCDDriver_SendData(0xEC);
	LCDDriver_SendData(0x1B);
	LCDDriver_SendData(0x4C);
	LCDDriver_SendData(0x40);
	LCDDriver_SendData(0x02); // SET B-Gamma
	LCDDriver_SendData(0x08);
	LCDDriver_SendData(0x12);
	LCDDriver_SendData(0x1A);
	LCDDriver_SendData(0x22);
	LCDDriver_SendData(0x2A);
	LCDDriver_SendData(0x31);
	LCDDriver_SendData(0x36);
	LCDDriver_SendData(0x3F);
	LCDDriver_SendData(0x48);
	LCDDriver_SendData(0x51);
	LCDDriver_SendData(0x58);
	LCDDriver_SendData(0x60);
	LCDDriver_SendData(0x68);
	LCDDriver_SendData(0x70);
	LCDDriver_SendData(0x78);

	LCDDriver_SendData(0x80);
	LCDDriver_SendData(0x88);
	LCDDriver_SendData(0x90);
	LCDDriver_SendData(0x98);
	LCDDriver_SendData(0xA0);
	LCDDriver_SendData(0xA7);
	LCDDriver_SendData(0xAF);
	LCDDriver_SendData(0xB6);
	LCDDriver_SendData(0xBE);
	LCDDriver_SendData(0xC7);
	LCDDriver_SendData(0xCE);
	LCDDriver_SendData(0xD6);
	LCDDriver_SendData(0xDE);
	LCDDriver_SendData(0xE6);
	LCDDriver_SendData(0xEF);
	LCDDriver_SendData(0xF5);
	LCDDriver_SendData(0xFB);
	LCDDriver_SendData(0xFC);
	LCDDriver_SendData(0xFE);
	LCDDriver_SendData(0x8C);
	LCDDriver_SendData(0xA4);
	LCDDriver_SendData(0x19);
	LCDDriver_SendData(0xEC);
	LCDDriver_SendData(0x1B);
	LCDDriver_SendData(0x4C);
	LCDDriver_SendData(0x40);
	LCDDriver_SendCommand(0x3A); // Set COLMOD
	LCDDriver_SendData(0x55);
	LCDDriver_SendCommand(0x11); // Sleep Out
	HAL_Delay(120);
	LCDDriver_SendCommand(0x29); // Display On

	HAL_Delay(10);
	LCDDriver_SendCommand(0x3A); // pixel format setting
	LCDDriver_SendData(0x55);
	LCDDriver_SendCommand(0x36); // set address mode
	LCDDriver_SendData(0x60);

	LCDDriver_SendCommand(0x11);
	HAL_Delay(120);

	LCDDriver_SendCommand(0x29); // Display on
}

// Set screen rotation
void LCDDriver_setRotation(uint8_t rotate) {
}

// Set cursor position
inline void LCDDriver_SetCursorAreaPosition(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend) {
	LCDDriver_SendCommand(0x2a);
	LCDDriver_SendData(Xstart >> 8);
	LCDDriver_SendData(Xstart & 0xff);
	LCDDriver_SendData(Xend >> 8);
	LCDDriver_SendData(Xend & 0xff);

	LCDDriver_SendCommand(0x2b);
	LCDDriver_SendData(Ystart >> 8);
	LCDDriver_SendData(Ystart & 0xff);
	LCDDriver_SendData(Yend >> 8);
	LCDDriver_SendData(Yend & 0xff);

	LCDDriver_SendCommand(0x2c);
}

static inline void LCDDriver_ReadCursorPosition(uint16_t x, uint16_t y) {
	LCDDriver_SendCommand(0x4f);
	LCDDriver_SendData(y);
	LCDDriver_SendCommand(0x4e);
	LCDDriver_SendData(x);

	LCDDriver_SendCommand(0x22);
}

// Write data to a single pixel
void LCDDriver_drawPixel(uint16_t x, uint16_t y, uint16_t color) {
	LCDDriver_SetCursorAreaPosition(x, y, x + 1, y + 1);
	LCDDriver_SendData(color);
}

// Fill the entire screen with a background color
void LCDDriver_Fill(uint16_t color) { LCDDriver_Fill_RectXY(0, 0, LCD_WIDTH, LCD_HEIGHT, color); }

// Rectangle drawing functions
void LCDDriver_Fill_RectXY(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	if (x1 > (LCD_WIDTH - 1)) // Set fill area
		x1 = LCD_WIDTH - 1;
	if (y1 > (LCD_HEIGHT - 1))
		y1 = LCD_HEIGHT - 1;

	LCDDriver_SetCursorAreaPosition(x0, y0, x1, y1);
	uint32_t n = ((x1 + 1) - x0) * ((y1 + 1) - y0);
	if (n < 100) {
		while (n--)
			LCDDriver_SendData(color);
		return;
	}

#if 0
    // DMA2D Set color in 32bit format
    WRITE_REG(hdma2d.Instance->OCOLR, (color << 16) | color);
    // DMA2D Set width and 32bit align
    uint32_t w = x1 - x0 + 1;
    if (w & 0x1)
        MODIFY_REG(hdma2d.Instance->NLR, (DMA2D_NLR_NL | DMA2D_NLR_PL), ((y1 - y0 + 1) | ((w + 1) << DMA2D_NLR_PL_Pos)));
    else
        MODIFY_REG(hdma2d.Instance->NLR, (DMA2D_NLR_NL | DMA2D_NLR_PL), ((y1 - y0 + 1) | (w << DMA2D_NLR_PL_Pos)));
    // DMA2D Start
    hdma2d.Instance->CR |= DMA2D_CR_START;
    // DMA2D Polling
    while ((hdma2d.Instance->ISR & DMA2D_FLAG_TC) == 0U)
        CPULOAD_GoToSleepMode();
    // DMA2D clean flags
    hdma2d.Instance->IFCR = DMA2D_FLAG_TC | DMA2D_FLAG_CTC;
#else
	while (n--)
		LCDDriver_SendData(color);
#endif
}

void LCDDriver_Fill_RectWH(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) { LCDDriver_Fill_RectXY(x, y, x + w, y + h, color); }

// Line drawing functions
void LCDDriver_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		uswap(x0, y0) uswap(x1, y1)
	}

	if (x0 > x1) {
		uswap(x0, x1) uswap(y0, y1)
	}

	int16_t dx, dy;
	dx = (int16_t)(x1 - x0);
	dy = (int16_t)abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			LCDDriver_drawPixel(y0, x0, color);
		} else {
			LCDDriver_drawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void LCDDriver_drawFastHLine(uint16_t x, uint16_t y, int16_t w, uint16_t color) {
	int16_t x2 = x + w;
	if (x2 < 0)
		x2 = 0;
	if (x2 > (LCD_WIDTH - 1))
		x2 = LCD_WIDTH - 1;

	if (x2 < x)
		LCDDriver_Fill_RectXY((uint16_t)x2, y, x, y, color);
	else
		LCDDriver_Fill_RectXY(x, y, (uint16_t)x2, y, color);
}

void LCDDriver_drawFastVLine(uint16_t x, uint16_t y, int16_t h, uint16_t color) {
	int16_t y2 = y + h - 1;
	if (y2 < 0)
		y2 = 0;
	if (y2 > (LCD_HEIGHT - 1))
		y2 = LCD_HEIGHT - 1;

	if (y2 < y)
		LCDDriver_Fill_RectXY(x, (uint16_t)y2, x, y, color);
	else
		LCDDriver_Fill_RectXY(x, y, x, (uint16_t)y2, color);
}

void LCDDriver_drawRectXY(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	LCDDriver_drawFastHLine(x0, y0, (int16_t)(x1 - x0), color);
	LCDDriver_drawFastHLine(x0, y1, (int16_t)(x1 - x0), color);
	LCDDriver_drawFastVLine(x0, y0, (int16_t)(y1 - y0), color);
	LCDDriver_drawFastVLine(x1, y0, (int16_t)(y1 - y0), color);
}

void LCDDriver_Fill_Triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
		_swap_int16_t(y0, y1);
		_swap_int16_t(x0, x1);
	}
	if (y1 > y2) {
		_swap_int16_t(y2, y1);
		_swap_int16_t(x2, x1);
	}
	if (y0 > y1) {
		_swap_int16_t(y0, y1);
		_swap_int16_t(x0, x1);
	}

	if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if (x1 < a)
			a = x1;
		else if (x1 > b)
			b = x1;
		if (x2 < a)
			a = x2;
		else if (x2 > b)
			b = x2;
		LCDDriver_drawFastHLine(a, y0, b - a + 1, color);
		return;
	}

	int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0, dx12 = x2 - x1, dy12 = y2 - y1;
	int32_t sa = 0, sb = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if (y1 == y2)
		last = y1; // Include y1 scanline
	else
		last = y1 - 1; // Skip it

	for (y = y0; y <= last; y++) {
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
*/
		if (a > b)
			_swap_int16_t(a, b);
		LCDDriver_drawFastHLine(a, y, b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = (int32_t)dx12 * (y - y1);
	sb = (int32_t)dx02 * (y - y0);
	for (; y <= y2; y++) {
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
*/
		if (a > b)
			_swap_int16_t(a, b);
		LCDDriver_drawFastHLine(a, y, b - a + 1, color);
	}
}

void LCDDriver_drawRoundedRectWH(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color, uint16_t radius, bool filled) {
	if (filled)
		LCDDriver_Fill_RectXY(x0, y0, x0 + w, y0 + h, color);
	else
		LCDDriver_drawRectXY(x0, y0, x0 + w, y0 + h, color);
}

void LCDDriver_fadeScreen(float32_t brightness) {
#if 0
	uint16_t color = 0;
	LCDDriver_ReadCursorPosition(0, 0);
	for (uint16_t y = 0; y < LCD_HEIGHT; y++) {
		for (uint16_t x = 0; x < LCD_WIDTH; x++) {
			// get current pixel
			color = LCDDriver_ReadData();
			// transform
			// uint8_t r = (uint8_t)((float32_t)((color >> 11) & 0x1F) * brightness);
			// uint8_t g = (uint8_t)((float32_t)((color >> 5) & 0x3F) * brightness);
			// uint8_t b = (uint8_t)((float32_t)((color >> 0) & 0x1F) * brightness);
			uint8_t r = ((color >> 11) & 0x1F) >> 2;
			uint8_t g = ((color >> 5) & 0x3F) >> 2;
			uint8_t b = ((color >> 0) & 0x1F) >> 2;
			color = (uint16_t)(r << 11) | (uint16_t)(g << 5) | (uint16_t)b;
			// write pixel back
			LCDDriver_SendData(color);
		}
	}
#endif
}

void LCDDriver_setBrightness(uint8_t percent) {
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, percent * 14 / 10);
}

#endif
