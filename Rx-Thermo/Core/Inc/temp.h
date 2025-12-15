/*
 * temp.h
 *
 *  Created on: Oct 2, 2025
 *      Author: Ghanim Jamil
 */

#ifndef INC_TEMP_H_
#define INC_TEMP_H_

#include <stdlib.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "LoRa.h"

#define ALARM_FIRE_INTERVAL 15000 + 200 /* To account for internal LS instead, in MS */
#define ALARM_DURATION 50 + 3 /* In Seconds */
#define TEMP_UPDATE_INTERVAL 2000

#define HEAT_GPIO_PORT GPIOA
#define HEAT_GPIO_PIN GPIO_PIN_1
#define AC_GPIO_PORT GPIOA
#define AC_GPIO_PIN GPIO_PIN_2

#define HEAT 0
#define AC 1
#define OFF -1

#define FREE 0
#define BUSY 1

#define DEADZONE 1

extern LoRa myLoRa;

typedef struct {
	size_t air_mode;
	float temperature;
	uint8_t temp_setpt;
	uint32_t alarm_update;
	uint32_t temp_update;
	uint8_t rtc_alarm_fired;
} TempCtx;


void TempCtx_Init(TempCtx* temp_ctx);
float HVAC_ReadTemp(void);
void HVAC_Report(uint8_t temp, uint8_t temp_setpt);
void HVAC_TempChange(TempCtx* temp_ctx);
uint8_t IncrementTempSetpt(TempCtx* temp_ctx);
uint8_t DecrementTempSetpt(TempCtx* temp_ctx);
void EnterStopMode(TempCtx* temp_ctx);

__STATIC_INLINE void HVAC_Heat(size_t PIN_TYPE) {
	HAL_GPIO_WritePin(HEAT_GPIO_PORT, HEAT_GPIO_PIN, PIN_TYPE);
}

__STATIC_INLINE void HVAC_AC(size_t PIN_TYPE) {
	HAL_GPIO_WritePin(AC_GPIO_PORT, AC_GPIO_PIN, PIN_TYPE);
}

__STATIC_INLINE GPIO_PinState HVAC_GetACPinState(void) {
	return HAL_GPIO_ReadPin(AC_GPIO_PORT, AC_GPIO_PIN);
}

__STATIC_INLINE GPIO_PinState HVAC_GetHeatPinState(void) {
	return HAL_GPIO_ReadPin(HEAT_GPIO_PORT, HEAT_GPIO_PIN);
}

#endif /* INC_TEMP_H_ */
