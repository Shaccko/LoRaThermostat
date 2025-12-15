/*
 * Tx-Commands.c
 *
 *  Created on: Oct 1, 2025
 *      Author: Ghanim Jamil
 */
#include <stdint.h>
#include <LoRa.h>
#include <stdio.h>
#include <LiquidCrystal.h>
#include "Tx-Commands.h"



uint8_t TransmitHEATCommand(void) {
	const char* msg = "HEAT";
	uint8_t packet[16];
	size_t len = strlen(msg);
	size_t plen = len; /* Checksum + len */
	GeneratePacketMsg(msg, packet, len);

	int8_t success = LoRa_transmit(&myLoRa, packet, plen + 2, 2000);
	LCD_UpdateAirMode(HEAT);

	return success;
}

uint8_t TransmitACCommand(void) {
	const char* msg = "AC";
	uint8_t packet[16];
	size_t len = strlen(msg);
	size_t plen = len; /* Checksum + len */
	GeneratePacketMsg(msg, packet, len);

	int8_t success = LoRa_transmit(&myLoRa, packet, plen + 2, 2000);
	LCD_UpdateAirMode(AC);

	return success;
}

uint8_t TransmitTEMPUPCommand(void) {
	if (temp_setpt >= 35) return 0;
	const char* msg = "TEMP_UP";
	uint8_t packet[16];
	size_t len = strlen(msg);
	size_t plen = len; /* Checksum + len */
	GeneratePacketMsg(msg, packet, len);
	temp_setpt++;

	int8_t success = LoRa_transmit(&myLoRa, packet, plen + 2, 2000);
	LCD_UpdateSetpt();

	return success;
}

uint8_t TransmitTEMPDOWNCommand(void) {
	if (temp_setpt <= 5) return 0;
	const char* msg = "TEMP_DOWN";
	uint8_t packet[16];
	size_t len = strlen(msg);
	size_t plen = len; /* Checksum + len */
	GeneratePacketMsg(msg, packet, len);
	temp_setpt--;

	int8_t success = LoRa_transmit(&myLoRa, packet, plen + 2, 2000);
	LCD_UpdateSetpt();

	return success;
}

uint8_t TransmitOFFCommand(void) {
	const char* msg = "OFF";
	uint8_t packet[16];
	size_t len = strlen(msg);
	size_t plen = len; /* Checksum + len */
	GeneratePacketMsg(msg, packet, len);

	uint8_t success = LoRa_transmit(&myLoRa, packet, plen + 2, 2000);
	LCD_UpdateAirMode(OFF);

	return success;
}

void GeneratePacketMsg(const char* msg, uint8_t* buf, size_t len) {
	/* [0] = len
	 * [..] = data
	 * [len + 1] = checksum
	 */
	size_t i;
	uint8_t checksum = 0;

	buf[0] = (uint8_t)len;

	memcpy(&buf[1], msg, len);

	for(i = 0; i < len; i++)
		checksum ^= msg[i];
	buf[len + 1] = checksum;
}


/* Communicating with our LCD */
void LCD_StrInit(void) {
    char setpt_str[] = "Setpt:";
    char temp_str[] = "T:";
    char off_str[] = "Off";

    // Setpt
    setCursor(0, 0);
    print(setpt_str);

    // Temp
    setCursor(14, 0);
    print(temp_str);

    // Air mode
    setCursor((LCD_COLS / 2) - strlen(off_str) / 2, 3);
    print(off_str);

    LCD_UpdateTemp();
    LCD_UpdateSetpt();
    LCD_UpdateAirMode(air_mode);
}

void LCD_UpdateTemp() {
    char num_str[10];
    setCursor(16, 0);

    if (temperature < 10) {
        sprintf(num_str, " %d", temperature);
    } else {
        sprintf(num_str, "%d", temperature);
    }

    print(num_str);
    write(0xDF);  // Degree symbol
    print("C");
}

void LCD_UpdateSetpt() {
    char num_str[10];
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

void LCD_UpdateAirMode(size_t new_air_mode) {
    char heat_str[] = "Heat On";
    char ac_str[] = "Cool On";
    char off_str[] = "  Off  ";

    if (new_air_mode == HEAT) {
        setCursor((LCD_COLS / 2) - strlen(heat_str) / 2,3);
        air_mode = HEAT;
        print(heat_str);
    } else if (new_air_mode == AC) {
        setCursor((LCD_COLS / 2) - strlen(ac_str) / 2,3);
        air_mode = AC;
        print(ac_str);
    } else if (new_air_mode == OFF) {
        setCursor((LCD_COLS / 2) - strlen(off_str) / 2,3);
        air_mode = OFF;
        print(off_str);
    }
}
