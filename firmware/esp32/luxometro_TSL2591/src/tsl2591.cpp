#include <Wire.h>
#include "tsl2591.h"

// TSL2591 register map (normal command mode)
static const uint8_t TSL2591_CMD = 0xA0;
static const uint8_t REG_ENABLE = 0x00;
static const uint8_t REG_CONTROL = 0x01;
static const uint8_t REG_DEVICE_ID = 0x12;
static const uint8_t REG_CHAN0_LOW = 0x14;
static const uint8_t REG_CHAN0_HIGH = 0x15;
static const uint8_t REG_CHAN1_LOW = 0x16;
static const uint8_t REG_CHAN1_HIGH = 0x17;

void tslWrite8(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(TSL2591_ADDR);
    Wire.write(TSL2591_CMD | reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t tslRead8(uint8_t reg) {
    Wire.beginTransmission(TSL2591_ADDR);
    Wire.write(TSL2591_CMD | reg);
    Wire.endTransmission();
    Wire.requestFrom(TSL2591_ADDR, (uint8_t)1);
    if (Wire.available()) return Wire.read();
    return 0;
}

void readChannels(uint16_t &full, uint16_t &ir) {
    uint16_t fullLow = tslRead8(REG_CHAN0_LOW);
    uint16_t fullHigh = tslRead8(REG_CHAN0_HIGH);
    uint16_t irLow = tslRead8(REG_CHAN1_LOW);
    uint16_t irHigh = tslRead8(REG_CHAN1_HIGH);

    full = (fullHigh << 8) | fullLow;
    ir = (irHigh << 8) | irLow;
}

bool tslInit() {
    Wire.begin();
    uint8_t id = tslRead8(REG_DEVICE_ID);

    // ENABLE = PON | AEN
    tslWrite8(REG_ENABLE, 0x03);
    // CONTROL = AGAIN_MED (x25) + ATIME_100MS
    tslWrite8(REG_CONTROL, 0x10);
    delay(500);

    Serial.print("TSL2591 ID: 0x");
    Serial.println(id, HEX);
    return id == 0x50;
}

float tslComputeLux(uint16_t full, uint16_t ir) {
    if (full == 0xFFFF || ir == 0xFFFF || full == 0) {
        return 0.0f;
    }

    float atimeMs = 100.0f;
    float again = 25.0f;
    float cpl = (atimeMs * again) / 408.0f;

    float lux1 = ((float)full - (1.64f * (float)ir)) / cpl;
    float lux2 = ((0.59f * (float)full) - (0.86f * (float)ir)) / cpl;
    float lux = lux1 > lux2 ? lux1 : lux2;

    if (lux < 0.0f) {
        lux = 0.0f;
    }
    return lux;
}