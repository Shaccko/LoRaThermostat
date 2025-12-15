/*
 * lcd_temp.h
 *
 *  Created on: Oct 2, 2025
 *      Author: Ghanim Jamil
 */

#ifndef INC_LCD_TEMP_H_
#define INC_LCD_TEMP_H_

#include "temp.h"
#include "stm32f1xx_hal.h"

#define ENABLE 1
#define DISABLE 0

void LCD_UpdateTemp(float temperature);
void LCD_UpdateSetpt(uint8_t temp_setpt);
void LCD_UpdateAirMode(size_t air_mode);
void LCD_HVACInit(TempCtx* temp_ctx);

__STATIC_INLINE void LCD_Backlight(GPIO_PinState pin) {
	HAL_GPIO_WritePin(GPIOA, LCD_BACKLIGHT_Pin, pin);
}

#endif /* INC_LCD_TEMP_H_ */
