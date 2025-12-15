/*
 * commands.h
 *
 *  Created on: Oct 2, 2025
 *      Author: Ghanim Jamil
 */

#ifndef INC_COMMANDS_H_
#define INC_COMMANDS_H_

void Parse_Command(char* command, TempCtx* temp_ctx);
void Execute_TempUp(TempCtx* temp_ctx);
void Execute_TempDown(TempCtx* temp_ctx);
void Execute_HeatMode(TempCtx* temp_ctx);
void Execute_ACMode(TempCtx* temp_ctx);
void Execute_OFFMode(TempCtx* temp_ctx);
uint8_t ValidateCommand(uint8_t* packet, char* buf);

#endif
