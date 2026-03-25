#ifndef TSL_H   
#define TSL_H

#include <Arduino.h>

#define TSL2591_ADDR 0x29

void tslWrite8(uint8_t reg, uint8_t value);
uint8_t tslRead8(uint8_t reg);
void readChannels(uint16_t &full, uint16_t &ir);
bool tslInit();
float tslComputeLux(uint16_t full, uint16_t ir);

#endif