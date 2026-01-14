#include <Wire.h>
#include "tsl.h"

void tslWrite8(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(TSL_ADDR);
    Wire.write(0x80 | reg); // bit comando
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t tslRead8(uint8_t reg) {
    Wire.beginTransmission(TSL_ADDR);
    Wire.write(0x80 | reg);
    Wire.endTransmission();
    Wire.requestFrom(TSL_ADDR, (uint8_t)1);
    if (Wire.available()) return Wire.read();
    return 0;
}

uint16_t readChannel1Raw() {
    uint16_t low  = (uint16_t)tslRead8(0x0E);
    uint16_t high = (uint16_t)tslRead8(0x0F);
    return (high << 8) | low;
}

void readChannels(uint16_t &ch0, uint16_t &ch1) {
    uint16_t ch0_low  = tslRead8(0x0C);
    uint16_t ch0_high = tslRead8(0x0D);
    uint16_t ch1_low  = tslRead8(0x0E);
    uint16_t ch1_high = tslRead8(0x0F);

    ch0 = (ch0_high << 8) | ch0_low; // broadband
    ch1 = (ch1_high << 8) | ch1_low; // IR
}

bool tslInit() {
    Wire.begin();           // en ESP32: SDA=21, SCL=22 por defecto
    uint8_t id = tslRead8(0x0A);

    // Encender sensor
    tslWrite8(0x00, 0x03);  // CONTROL: power ON
    // Timings: 402ms, gain x16 (alta sensibilidad)
    tslWrite8(0x01, 0x12);  // TIMING
    delay(500);

    Serial.print("TSL2561 ID: 0x");
    Serial.println(id, HEX);
    return true;
}