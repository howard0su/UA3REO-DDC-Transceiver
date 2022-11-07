#ifndef _LCDDRIVER_HX8369A1_H_
#include "settings.h"
#if defined(LCD_HX8369A)
#define _LCDDRIVER_HX8369A_H_

#define LCD_FSMC_COMM_ADDR 0x60000000
#define LCD_FSMC_DATA_ADDR 0x60080000

#define HAS_BRIGHTNESS_CONTROL true

// LCD dimensions defines
#define LCD_WIDTH 800
#define LCD_HEIGHT 480
#define LCD_TYPE_FSMC true

#endif
#endif
