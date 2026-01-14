#ifndef TSL_H   
#define TSL_H

#include <Arduino.h>

#define TSL_ADDR 0x39 // dirección por defecto (0x29, 0x39 o 0x49 según wiring)

void tslWrite8(uint8_t reg, uint8_t value);
uint8_t tslRead8(uint8_t reg);
uint16_t readChannel1Raw();
void readChannels(uint16_t &ch0, uint16_t &ch1);
bool tslInit();

#endif