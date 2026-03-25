#ifndef TSL2591_H
#define TSL2591_H

#include <Arduino.h>

#define TSL2591_ADDR 0x29

void tsl2591Write8(uint8_t reg, uint8_t value);
uint8_t tsl2591Read8(uint8_t reg);
void tsl2591ReadChannels(uint16_t &full, uint16_t &ir);
bool tsl2591Init();
float tsl2591ComputeLux(uint16_t full, uint16_t ir);

#endif // TSL2591_H
