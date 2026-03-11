#include <Arduino.h>
#include <Wire.h>
#include "lux_VML7700.h"
#include "tsl2561.h"
#include "lux_BH1750.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

// Credenciales WiFi
#define WIFI_SSID "Stark-C6"
#define WIFI_PASSWORD "WinterIsComing-C6"

// Credenciales MQTT
const char* MQTT_HOST = "192.168.1.168";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "esp32_01_lux";
const char* MQTT_TOPIC_BASE = "lab/lux/esp32_01";

// Pines I2C
#define SDA_PIN 21
#define SCL_PIN 22
#define VEML7700_ADDR 0x10
#define BH1750_ADDR 0x23

void scanI2C(int sdaPin, int sclPin) {
  Wire.begin(sdaPin, sclPin);
  Serial.println("\nI2C scan...");

  int found = 0;
  bool foundVeml7700 = false;
  bool foundTsl2561 = false;
  bool foundBH1750 = false;
  
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();
    if (err == 0) {
      const char* label = "";
      if (addr == VEML7700_ADDR) {
        label = " (VEML7700)";
        foundVeml7700 = true;
      } else if (addr == TSL_ADDR) {
        label = " (TSL2561)";
        foundTsl2561 = true;
      } else if (addr == BH1750_ADDR) {
        label = " (BH1750)";
        foundBH1750 = true;
      }

      Serial.printf("Found: 0x%02X%s\n", addr, label);
      found++;
    }
  }

  Serial.printf("Done. Devices: %d\n\n", found);
  if (!foundVeml7700) {
    Serial.printf("Aviso: no se detectó VEML7700 en 0x%02X\n", VEML7700_ADDR);
  }
  if (!foundTsl2561) {
    Serial.printf("Aviso: no se detectó TSL2561 en 0x%02X\n", TSL_ADDR);
  }
  if (!foundBH1750) {
    Serial.printf("Aviso: no se detectó BH1750 en 0x%02X\n", BH1750_ADDR);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Luxómetro ESP32 (VEML7700 + TSL2561 + BH1750) ===");

  scanI2C(SDA_PIN, SCL_PIN);

  if (!wifiInit(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("Advertencia: No se pudo conectar a WiFi");
    Serial.println("Continuando sin conexión WiFi...\n");
  }
  Serial.println();

  if (!mqttInit(MQTT_HOST, MQTT_PORT, MQTT_CLIENT_ID)) {
    Serial.println("Advertencia: No se pudo conectar a MQTT");
    Serial.println("Se publicarán datos por serial...\n");
  }
  Serial.println();

  if (!luxInit(SDA_PIN, SCL_PIN)) {
    Serial.println("Error: No se pudo encontrar el sensor VEML7700");
    Serial.println("Verifica las conexiones I2C (SDA, SCL)");
    while (1) {
      delay(1000);
    }
  }
  
  Serial.println("Sensor VEML7700 inicializado correctamente");
  luxPrintConfig();
  
  if (!tslInit()) {
    Serial.println("Advertencia: No se pudo inicializar el sensor TSL2561");
    Serial.println("Continuando sin TSL2561...");
  }
  
  if (!luxBH1750Init(SDA_PIN, SCL_PIN)) {
    Serial.println("Advertencia: No se pudo inicializar el sensor BH1750");
    Serial.println("Continuando sin BH1750...");
  }
  
  Serial.println("\nIniciando mediciones...\n");
}

void publica_VEML7700() {
  if (!mqttIsConnected()) return;
  
  char topic[96];

  snprintf(topic, sizeof(topic), "%s/veml7700/lux", MQTT_TOPIC_BASE);
  if (mqttPublishFloat(topic, get_ultimo_lux(), 2, false)) {
    Serial.printf("MQTT -> %s = %.2f\n", topic, get_ultimo_lux());
  }

  snprintf(topic, sizeof(topic), "%s/veml7700/white", MQTT_TOPIC_BASE);
  if (mqttPublishFloat(topic, get_ultimo_white(), 2, false)) {
    Serial.printf("MQTT -> %s = %.2f\n", topic, get_ultimo_white());
  }

  snprintf(topic, sizeof(topic), "%s/veml7700/als", MQTT_TOPIC_BASE);
  if (mqttPublishInt(topic, get_ultimo_als(), false)) {
    Serial.printf("MQTT -> %s = %u\n", topic, get_ultimo_als());
  }
}

void publica_TSL2561() {
  if (!mqttIsConnected()) return;
  
  char topic[96];

  snprintf(topic, sizeof(topic), "%s/tsl2561/lux", MQTT_TOPIC_BASE);
  if (mqttPublishFloat(topic, get_ultimo_tsl_lux(), 2, false)) {
    Serial.printf("MQTT -> %s = %.2f\n", topic, get_ultimo_tsl_lux());
  }

  snprintf(topic, sizeof(topic), "%s/tsl2561/ch0", MQTT_TOPIC_BASE);
  if (mqttPublishInt(topic, get_ultimo_ch0(), false)) {
    Serial.printf("MQTT -> %s = %u\n", topic, get_ultimo_ch0());
  }

  snprintf(topic, sizeof(topic), "%s/tsl2561/ch1", MQTT_TOPIC_BASE);
  if (mqttPublishInt(topic, get_ultimo_ch1(), false)) {
    Serial.printf("MQTT -> %s = %u\n", topic, get_ultimo_ch1());
  }
}

void publica_BH1750() {
  if (!mqttIsConnected()) return;
  
  char topic[96];

  snprintf(topic, sizeof(topic), "%s/bh1750/lux", MQTT_TOPIC_BASE);
  if (mqttPublishFloat(topic, get_ultimo_bh1750_lux(), 2, false)) {
    Serial.printf("MQTT -> %s = %.2f\n", topic, get_ultimo_bh1750_lux());
  }
}

void loop() {
  mqttKeepAlive();

  lee_VEML7700();
  lee_TSL2561();
  lee_BH1750();

  Serial.println("=================================");
  logs_valores_VML7700();
  logs_valores_TSL2561();
  logs_valores_BH1750();
  Serial.println("=================================\n");

  publica_VEML7700();
  publica_TSL2561();
  publica_BH1750();
  
  delay(2000);
}
