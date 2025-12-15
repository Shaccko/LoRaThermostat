/*
 * temp.c
 *
 *  Created on: Oct 2, 2025
 *      Author: Ghanim Jamil
 */


#include <stdio.h>
#include <string.h>
#include <temp.h>
#include <lcd_temp.h>
#include <Si7021_driver.h>
#include <LoRa.h>
#include <rtc.h>


void TempCtx_Init(TempCtx* temp_ctx) {
	temp_ctx->air_mode = OFF;
	temp_ctx->temperature = HVAC_ReadTemp();
	temp_ctx->temp_setpt = 20;
	temp_ctx->alarm_update = 0;
	temp_ctx->temp_update = 0;

	HVAC_Report((int) temp_ctx->temperature,
			temp_ctx->temp_setpt);
}

float HVAC_ReadTemp(void) {
	static float temperature = 0;
	r_single_Si7021(&temperature, Temperature);

	return temperature;
}

void HVAC_Report(uint8_t temp, uint8_t temp_setpt) {
	uint8_t buf[8];
	snprintf((char*) buf, 8, "%d", (temp << 8) | temp_setpt);
	LoRa_transmit(&myLoRa, buf, 8, 2000);
}

void HVAC_TempChange(TempCtx* temp_ctx) {
	switch (temp_ctx->air_mode) {
		case AC:
			HVAC_Heat(FREE);
			if (temp_ctx->temperature > temp_ctx->temp_setpt + DEADZONE) {
				if (HVAC_GetACPinState() == FREE) {
					HVAC_AC(BUSY);
				}
				LCD_UpdateTemp(temp_ctx->temperature);
				HVAC_Report(temp_ctx->temperature, temp_ctx->temp_setpt);
			}
			else {
				HVAC_AC(FREE);
			}
			break;

		case HEAT:
			HVAC_AC(FREE);
			if (temp_ctx->temperature < temp_ctx->temp_setpt - DEADZONE) {
				if (HVAC_GetHeatPinState() == FREE) {
					HVAC_Heat(BUSY);
				}
				LCD_UpdateTemp(temp_ctx->temperature);
				HVAC_Report(temp_ctx->temperature, temp_ctx->temp_setpt);
			}
			else {
				HVAC_Heat(FREE);
			}
			break;
		case OFF:
			HVAC_Heat(FREE);
			HVAC_AC(FREE);
			break;
	}
}

uint8_t IncrementTempSetpt(TempCtx* temp_ctx) {
	return (temp_ctx->temp_setpt < 35) ? temp_ctx->temp_setpt++ : 0;
}

uint8_t DecrementTempSetpt(TempCtx* temp_ctx) {
	return (temp_ctx->temp_setpt > 5) ? temp_ctx->temp_setpt-- : 0;
}

void EnterStopMode(TempCtx* temp_ctx) {
	/* Init Alarm */
	RTC_AlarmTypeDef sAlarm = {0};
    RTC_TimeTypeDef now;
    HAL_RTC_GetTime(&hrtc, &now, RTC_FORMAT_BIN);
    sAlarm.AlarmTime.Hours = now.Hours;
    sAlarm.AlarmTime.Minutes = now.Minutes;
    uint8_t secs = now.Seconds + ALARM_DURATION;

    if (secs >= 60) {
  	    secs -= 60;
	    sAlarm.AlarmTime.Minutes = (now.Minutes + 1) % 60;
    } else {
	    sAlarm.AlarmTime.Minutes = now.Minutes;
    }

    sAlarm.AlarmTime.Seconds = secs;
    sAlarm.Alarm = RTC_ALARM_A;
    temp_ctx->rtc_alarm_fired = 1;

    LCD_Backlight(DISABLE); /* Save power lol */
    HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN); /* Fire alarm */

    /* Enter Stop Mode */
    HAL_SuspendTick();
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    /* Exit commands below: */
    /* Report temp on wakeup */
    HVAC_Report(temp_ctx->temperature, temp_ctx->temp_setpt);
}
