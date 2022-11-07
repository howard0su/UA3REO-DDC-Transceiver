#ifndef TOUCHPAD_XPT2046_h
#define TOUCHPAD_XPT2046_h
#include "settings.h"
#if (defined(TOUCHPAD_XPT2046))
#define HAS_TOUCHPAD true

typedef struct {
	uint8_t Touch;
	uint8_t TouchpointFlag;
	uint8_t TouchCount;

} XPT2046_Dev;

extern XPT2046_Dev XPT2046;

extern void XPT2046_Scan(void);
extern void XPT2046_Init(void);
extern void XPT2046_ReadAux(float *battery, float *tempure);

#endif
#endif
