#include "lcd.h"
#include "touchpad.h"

#if (defined(TOUCHPAD_XPT2046))

#define _BV(bit) (1U << (bit))

#define XPT2046_CFG_START _BV(7)

#define XPT2046_CFG_MUX(v) ((v & 0b111) << (4))

#define XPT2046_CFG_8BIT _BV(3)
#define XPT2046_CFG_12BIT (0)

#define XPT2046_CFG_SER _BV(2)
#define XPT2046_CFG_DFR (0)

#define XPT2046_CFG_PWR(v) ((v & 0b11))

#define XPT2046_MUX_Y 0b101
#define XPT2046_MUX_X 0b001

#define XPT2046_MUX_Z1 0b011
#define XPT2046_MUX_Z2 0b100

#define XPT2046_MUX_VBAT 0b010
#define XPT2046_MUX_TEMP 0b000

#define RESAMPLE_COUNT 16

XPT2046_Dev XPT2046;

void XPT2046_Scan(void) {
	uint32_t x = 0;
	uint32_t y = 0;
	uint32_t z1 = 0;
	uint32_t z2 = 0;

	if (XPT2046.Touch == 1) {
		if (HRDW_SPI_Periph_busy) {
			println(__func__, "SPI Busy");
			return;
		}

		HRDW_SPI_Periph_busy = true;
		XPT2046.Touch = 0;

		uint8_t inbuf[12] = {
		    (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_Y) | XPT2046_CFG_PWR(3)),  0x00, 0x00,
		    (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_X) | XPT2046_CFG_PWR(3)),  0x00, 0x00,
		    (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_Z1) | XPT2046_CFG_PWR(3)), 0x00, 0x00,
		    (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_Z2) | XPT2046_CFG_PWR(3)), 0x00, 0x00};
		uint8_t outbuf[12] = {0};
		for (int i = 0; i < RESAMPLE_COUNT; i++) {
			// SPI requirer 32bit aliment
			SPI_Transmit(&hspi2, inbuf, outbuf, sizeof(inbuf), TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, true, SPI_TOUCH_PRESCALER, false);

			y += (outbuf[1] << 8 | outbuf[2]) >> 3;
			x += (outbuf[4] << 8 | outbuf[5]) >> 3;
			z1 += (outbuf[7] << 8 | outbuf[8]) >> 3;
			z2 += (outbuf[10] << 8 | outbuf[11]) >> 3;
		}

		uint8_t buf[4] = {(XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_Y)), 0x00, 0x00, 0x00};
		SPI_Transmit(&hspi2, buf, NULL, sizeof(buf), TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, false, SPI_TOUCH_PRESCALER, false);
		HRDW_SPI_Periph_busy = false;

		x /= RESAMPLE_COUNT;
		y /= RESAMPLE_COUNT;
		z1 /= RESAMPLE_COUNT;
		z2 /= RESAMPLE_COUNT;

		TOUCHPAD_processTouch(x, y, 0, 0);
	}
}

void XPT2046_Init() {

	if (HRDW_SPI_Periph_busy) {
		println(__func__, "SPI Busy");
		return;
	}

	HRDW_SPI_Periph_busy = true;
	// enable IRQ
	uint8_t buf[4] = {(XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_Y)), 0x00, 0x00, 0x00};
	SPI_Transmit(&hspi2, buf, buf, sizeof(buf), TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, false, SPI_TOUCH_PRESCALER, false);
	HRDW_SPI_Periph_busy = false;

    println("Touchpad XPT2046 initialized");
}

void XPT2046_ReadAux(float *battery, float *temperature) {
	uint32_t vbat = 0;
	uint32_t temp = 0;

	if (HRDW_SPI_Periph_busy) {
		println(__func__, "SPI Busy");
		return;
	}

    uint8_t inbuf[6] = {(XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_VBAT)), 0x00, 0x00,
                        (XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_TEMP)), 0x00, 0x00};
	uint8_t outbuf[6] = {0};

	HRDW_SPI_Periph_busy = true;
	for (int i = 0; i < RESAMPLE_COUNT; i++) {
		// SPI requirer 32bit aliment
		SPI_Transmit(&hspi2, inbuf, outbuf, sizeof(outbuf), TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, true, SPI_TOUCH_PRESCALER, false);

		vbat += (outbuf[1] << 8 | outbuf[2]) >> 3;
		temp += (outbuf[4] << 8 | outbuf[5]) >> 3;
	}

    uint8_t buf[4] = {(XPT2046_CFG_START | XPT2046_CFG_12BIT | XPT2046_CFG_DFR | XPT2046_CFG_MUX(XPT2046_MUX_Y)), 0x00, 0x00, 0x00};
    SPI_Transmit(&hspi2, buf, NULL, sizeof(buf), TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, false, SPI_TOUCH_PRESCALER, false);

	HRDW_SPI_Periph_busy = false;

	vbat /= RESAMPLE_COUNT;
	temp /= RESAMPLE_COUNT;

    if (battery)
	    *battery = vbat;
    
    if (temperature)
	    *temperature = temp;
}

#endif