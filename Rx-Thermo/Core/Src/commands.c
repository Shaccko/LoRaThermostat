/*
 * commands.c
 *
 *  Created on: Oct 2, 2025
 *      Author: Ghanim Jamil
 */


#include <temp.h>
#include <commands.h>
#include <lcd_temp.h>
#include <string.h>


void Parse_Command(char* command, TempCtx* temp_ctx) {
	LCD_Backlight(ENABLE);
	if (strcmp(command, "TEMP_UP") == 0) {
		Execute_TempUp(temp_ctx);
	}
    else if (strcmp(command, "TEMP_DOWN") == 0) {
    	Execute_TempDown(temp_ctx);
    }
    else if (strcmp(command, "HEAT") == 0) {
    	Execute_HeatMode(temp_ctx);
    }
    else if (strcmp(command, "AC") == 0) {
    	Execute_ACMode(temp_ctx);
    }
    else if (strcmp(command, "OFF") == 0) {
    	Execute_OFFMode(temp_ctx);
    }
	temp_ctx->alarm_update = HAL_GetTick();
	temp_ctx->temp_update = HAL_GetTick();
}

uint8_t ValidateCommand(uint8_t* packet, char* buf) {
	/* [0] = len
	 * [..] = data
	 * [len + 1] = checksum
	 */
	/* Packet length at most 11 bytes */
	if (*packet > 16) return 0;
	size_t msg_len = *packet;
	uint8_t checksum_chk = 0;
	size_t i;

	memcpy(buf, &packet[1], msg_len);
	buf[msg_len] = '\0';

	for (i = 0; i < msg_len; i++)
		checksum_chk ^= packet[i + 1];

	if (checksum_chk != packet[msg_len + 1]) return 0;

	return 1;
}

void Execute_TempUp(TempCtx* temp_ctx) {
	if (temp_ctx->air_mode == AC || temp_ctx->air_mode == HEAT) {
		temp_ctx->temp_setpt++;
		LCD_UpdateSetpt(temp_ctx->temp_setpt);
	}
}

void Execute_TempDown(TempCtx* temp_ctx) {
	if (temp_ctx->air_mode == AC || temp_ctx->air_mode == HEAT) {
		temp_ctx->temp_setpt--;
		LCD_UpdateSetpt(temp_ctx->temp_setpt);
	}
}

void Execute_HeatMode(TempCtx* temp_ctx) {
	temp_ctx->air_mode = HEAT;
	LCD_UpdateAirMode(temp_ctx->air_mode);
}

void Execute_ACMode(TempCtx* temp_ctx) {
	temp_ctx->air_mode = AC;
	LCD_UpdateAirMode(temp_ctx->air_mode);
}

void Execute_OFFMode(TempCtx* temp_ctx) {
	temp_ctx->air_mode = OFF;
	LCD_UpdateAirMode(temp_ctx->air_mode);
}
