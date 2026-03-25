#include <Arduino.h>
#include "tsl2591.h"

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("Iniciando TSL2591...");

    if (tslInit()) {
        Serial.println("TSL2591 inicializado correctamente.");
    } else {
        Serial.println("TSL2591 no detectado. Revisa cableado SDA/SCL/VCC/GND.");
    }
}



void loop() {
    uint16_t full, ir;
    readChannels(full, ir);
    float lux = tslComputeLux(full, ir);

    Serial.print("FULL: ");
    Serial.print(full);
    Serial.print("  IR: ");
    Serial.print(ir);
    Serial.print("  lux: ");
    Serial.println(lux, 2);

    delay(1000);
}
