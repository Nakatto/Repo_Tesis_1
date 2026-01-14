#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include "tsl.h"

// ----------------- WiFi -----------------
const char* ssid     = "Stark-C6";
const char* password = "WinterIsComing-C6";

// ----------------- TSL2561 -----------------



float computeLux(uint16_t ch0, uint16_t ch1) {
    if (ch0 == 0) return 0.0f;

    float ratio = (float)ch1 / (float)ch0;

    float lux = 0.0f;

    if (ratio <= 0.50f) {
        lux = 0.0304f * ch0 - 0.062f * ch0 * powf(ratio, 1.4f);
    } else if (ratio <= 0.61f) {
        lux = 0.0224f * ch0 - 0.031f * ch1;
    } else if (ratio <= 0.80f) {
        lux = 0.0128f * ch0 - 0.0153f * ch1;
    } else if (ratio <= 1.30f) {
        lux = 0.00146f * ch0 - 0.00112f * ch1;
    } else {
        lux = 0.0f;
    }

    if (lux < 0.0f) lux = 0.0f;
    return lux;
}



// ----------------- InfluxDB -----------------
// Usa la MISMA BD que el PC (metrics)
const char* influxUrl = "http://192.168.1.139:8086/write?db=metrics";

void postInflux(uint16_t ch0, uint16_t ch1, float lux) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi no conectado, no se envía a InfluxDB");
        return;
    }

    WiFiClient client;
    HTTPClient http;

    Serial.print("POST InfluxDB a: ");
    Serial.println(influxUrl);

    // measurement: lux
    // tags: host=esp32
    // fields: lux (calculado), ch0, ch1
    String line = "lux,host=esp32 "
                  "lux=" + String(lux, 2) +
                  ",ch0=" + String(ch0) +
                  ",ch1=" + String(ch1);

    Serial.print("Line protocol: ");
    Serial.println(line);

    http.begin(client, influxUrl);
    http.setTimeout(10000);
    http.addHeader("Content-Type", "text/plain; charset=utf-8");

    int httpCode = http.POST(line);

    if (httpCode > 0) {
        Serial.printf("InfluxDB -> HTTP code: %d\n", httpCode);
        if (httpCode < 200 || httpCode >= 300) {
            String resp = http.getString();
            Serial.print("Respuesta de Influx: ");
            Serial.println(resp);
        }
    } else {
        Serial.printf("POST InfluxDB falló, httpCode: %d\n", httpCode);
    }

    http.end();
}


void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("Conectando a Wi-Fi...");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Intentando conectar...");
    }

    Serial.println("Conectado a Wi-Fi");
    Serial.print("Dirección IP ESP32: ");
    Serial.println(WiFi.localIP());

    if (tslInit()) {
        Serial.println("TSL2561 inicializado.");
    } else {
        Serial.println("TSL2561: inicialización fallida.");
    }
}



void loop() {
    delay(1000);

    uint16_t ch0, ch1;
    readChannels(ch0, ch1);
    float lux = computeLux(ch0, ch1);

    Serial.print("ch0: ");
    Serial.print(ch0);
    Serial.print("  ch1: ");
    Serial.print(ch1);
    Serial.print("  lux: ");
    Serial.println(lux, 2);

    postInflux(ch0, ch1, lux);

    delay(2000);
}
