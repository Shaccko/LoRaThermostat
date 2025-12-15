/*
 * lcd_temp.c
 *
 *  Created on: Oct 2, 2025
 *      Author: Ghanim Jamil
 */

#include <temp.h>
#include <stdio.h>
#include <string.h>
#include "lcd_temp.h"
#include "LiquidCrystal.h"



void LCD_UpdateTemp(float temperature) {
    char num_str[20];
    setCursor(16, 0);

    if (temperature < 10) {
        sprintf(num_str, " %d", (int) temperature);
    } else {
        sprintf(num_str, "%d", (int) temperature);
    }

    print(num_str);
    write(0xDF);  // Degree symbol
    print("C");
}

void LCD_UpdateSetpt(uint8_t temp_setpt) {
    char num_str[20];
    setCursor(6, 0);

    if (temp_setpt < 10) {
        sprintf(num_str, " %d", temp_setpt);
    } else {
        sprintf(num_str, "%d", temp_setpt);
    }

    print(num_str);
    write(0xDF);  // Degree symbol
    print("C");
}


void LCD_UpdateAirMode(size_t air_mode) {
    char heat_str[] = "Heat On";
    char ac_str[] = "Cool On";
    char off_str[] = "  Off  ";

    if (air_mode == HEAT) {
        setCursor((LCD_COLS / 2) - strlen(heat_str) / 2,3);
        print(heat_str);
    } else if (air_mode == AC) {
        setCursor((LCD_COLS / 2) - strlen(ac_str) / 2,3);
        print(ac_str);
    } else if (air_mode == OFF) {
        setCursor((LCD_COLS / 2) - strlen(off_str) / 2,3);
        print(off_str);
    }
}

void LCD_HVACInit(TempCtx* temp_ctx) {
    char setpt_str[] = "Setpt:";
    char temp_str[] = "T:";
    char off_str[] = "Off";

    LiquidCrystal(GPIOB, LCD_RS_Pin, 255, LCD_E_Pin,
		  LCD_D7_Pin, LCD_D6_Pin, LCD_D5_Pin, LCD_D4_Pin);
    HAL_Delay(5);

    // Setpt
    setCursor(0, 0);
    print(setpt_str);

    // Temp
    setCursor(14, 0);
    print(temp_str);

    // Air mode
    setCursor((LCD_COLS / 2) - strlen(off_str) / 2, 3);
    print(off_str);

    LCD_Backlight(ENABLE);
    HVAC_Report(temp_ctx->temperature, temp_ctx->temp_setpt); /* Initial report */

    LCD_UpdateTemp(temp_ctx->temperature);
    LCD_UpdateSetpt(temp_ctx->temp_setpt);
    LCD_UpdateAirMode(temp_ctx->air_mode);
}
