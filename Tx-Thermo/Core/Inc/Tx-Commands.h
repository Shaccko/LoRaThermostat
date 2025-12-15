/*
 * Tx-Commands.h
 *
 *  Created on: Oct 1, 2025
 *      Author: Ghanim Jamil
 */

#ifndef INC_TX_COMMANDS_H_
#define INC_TX_COMMANDS_H_

#include <stdint.h>
#include <string.h>
#include <LiquidCrystal.h>
#include "stm32f1xx_hal.h"

#define UNAVAILABLE -1
#define FREE 0
#define BUSY 1

#define HEAT 0
#define AC 1
#define OFF -1

#define TEMP_PORT GPIOA

extern size_t air_mode;
extern uint8_t temperature;
extern uint8_t temp_setpt;
extern LoRa myLoRa;

uint8_t TransmitHEATCommand(void);
uint8_t TransmitACCommand(void);
uint8_t TransmitTEMPUPCommand(void);
uint8_t TransmitTEMPDOWNCommand(void);
uint8_t TransmitOFFCommand(void);

__STATIC_INLINE uint8_t Get_TempUpButtonState(void) {
	return (air_mode == HEAT || air_mode == AC)
			? HAL_GPIO_ReadPin(TEMP_PORT, Temp_Up_Pin) : BUSY;
}

__STATIC_INLINE uint8_t Get_TempDownButtonState(void) {
	return (air_mode == HEAT || air_mode == AC)
			? HAL_GPIO_ReadPin(TEMP_PORT, Temp_Down_Pin) : BUSY;
}

__STATIC_INLINE uint8_t Get_HeatButtonState(void) {
	return air_mode != HEAT
			? HAL_GPIO_ReadPin(TEMP_PORT, Heat_On_Pin) : BUSY;
}

__STATIC_INLINE uint8_t Get_ACButtonState(void) {
	return air_mode != AC
			? HAL_GPIO_ReadPin(TEMP_PORT, Cool_On_Pin) : BUSY;
}

__STATIC_INLINE uint8_t Get_OffButtonState(void) {
	return HAL_GPIO_ReadPin(TEMP_PORT, Off_State_Pin);
}

void LCD_UpdateTemp(void);
void LCD_UpdateAirMode(size_t air_mode);
void LCD_UpdateSetpt(void);
void LCD_StrInit(void);

void GeneratePacketMsg(const char* msg, uint8_t* buf, size_t len);

typedef struct {
	size_t state;
	uint8_t (*GetPinState)(void);
	uint8_t (*TransmitMessage)(void);
} Buttons;



#endif /* INC_TX_COMMANDS_H_ */
