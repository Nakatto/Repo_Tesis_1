#include <Arduino.h>
#include <Wire.h>
#include "lux_VML7700.h"
#include "tsl2561.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

// Credenciales WiFi
#define WIFI_SSID "Stark-C6"
#define WIFI_PASSWORD "WinterIsComing-C6"

// Credenciales MQTT
const char* MQTT_HOST = "test.mosquitto.org";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "esp32_01_lux";      // Que sea único
const char* MQTT_TOPIC     = "lab/lux/esp32_01";  // Topic de publicación

// Pines I2C (puedes ajustar según tu configuración)
#define SDA_PIN 21
#define SCL_PIN 22
#define VEML7700_ADDR 0x10

void scanI2C(int sdaPin, int sclPin) {
  Wire.begin(sdaPin, sclPin);
  Serial.println("\nI2C scan...");

  int found = 0;
  bool foundVeml7700 = false;
  bool foundTsl2561 = false;
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
  Serial.println();
}

void setup() {
  // Inicializar comunicación serial
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Luxómetro VEML7700 ===");

  // Escanear dispositivos I2C al arranque
  scanI2C(SDA_PIN, SCL_PIN);

  // Conectar a WiFi
  if (!wifiInit(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("Advertencia: No se pudo conectar a WiFi");
    Serial.println("Continuando sin conexión WiFi...\n");
  }
  Serial.println();

  // Conectar a MQTT
  if (!mqttInit(MQTT_HOST, MQTT_PORT, MQTT_CLIENT_ID)) {
    Serial.println("Advertencia: No se pudo conectar a MQTT");
    Serial.println("Se publicarán datos por serial...\n");
  }
  Serial.println();

  // Inicializar sensor VEML7700
  if (!luxInit(SDA_PIN, SCL_PIN)) {
    Serial.println("Error: No se pudo encontrar el sensor VEML7700");
    Serial.println("Verifica las conexiones I2C (SDA, SCL)");
    while (1) {
      delay(1000);
    }
  }
  
  Serial.println("Sensor VEML7700 inicializado correctamente");
  
  // Mostrar configuración
  luxPrintConfig();
  
  // Inicializar sensor TSL2561
  if (!tslInit()) {
    Serial.println("Advertencia: No se pudo inicializar el sensor TSL2561");
    Serial.println("Continuando solo con VEML7700...");
  }
  
  Serial.println("\nIniciando mediciones...\n");
}

void loop() {
  // Mantener viva la conexión MQTT
  mqttKeepAlive();

  // Leer mediciones del sensor VEML7700
  float lux = luxRead();
  float white = luxReadWhite();
  uint16_t als = luxReadALS();

  // Leer mediciones del sensor TSL2561
  uint16_t tsl_ch0, tsl_ch1;
  readChannels(tsl_ch0, tsl_ch1);
  float tsl_lux = tsl_computeLux(tsl_ch0, tsl_ch1);

  // Imprimir por serial - VEML7700
  Serial.println("=================================");
  Serial.println("VEML7700:");
  Serial.print("Iluminancia: ");
  Serial.print(lux);
  Serial.println(" lux");
  
  Serial.print("Luz blanca: ");
  Serial.println(white);
  
  Serial.print("ALS (raw): ");
  Serial.println(als);
  
  Serial.print("Nivel: ");
  Serial.println(luxGetLevel(lux));
  
  // Imprimir por serial - TSL2561
  Serial.println("\nTSL2561:");
  Serial.print("Iluminancia: ");
  Serial.print(tsl_lux);
  Serial.println(" lux");
  Serial.print("CH0 (Broadband): ");
  Serial.println(tsl_ch0);
  Serial.print("CH1 (IR): ");
  Serial.println(tsl_ch1);
  Serial.println("=================================\n");

  // Publicar en MQTT si está conectado
  // if (mqttIsConnected()) {
  //   // Publicar lux
  //   char topicLux[64];
  //   snprintf(topicLux, sizeof(topicLux), "%s/lux", MQTT_TOPIC);
  //   mqttPublishFloat(topicLux, lux, 2, false);

  //   // Publicar luz blanca
  //   char topicWhite[64];
  //   snprintf(topicWhite, sizeof(topicWhite), "%s/white", MQTT_TOPIC);
  //   mqttPublishFloat(topicWhite, white, 2, false);

  //   // Publicar ALS raw
  //   char topicALS[64];
  //   snprintf(topicALS, sizeof(topicALS), "%s/als", MQTT_TOPIC);
  //   mqttPublishInt(topicALS, als, false);

  //   Serial.println("Datos publicados en MQTT");
  // }

  if (mqttIsConnected()) {
    // Publicar datos VEML7700
    char payload1[160];
    snprintf(payload1, sizeof(payload1),
             "{\"sensor\":\"VEML7700\",\"id\":0,\"lux\":%.2f,\"white\":%.2f,\"als\":%u}",
             lux, white, als);

    mqttPublishString(MQTT_TOPIC, payload1, false);

    Serial.print("MQTT -> ");
    Serial.println(payload1);
    
    // Publicar datos TSL2561
    char payload2[160];
    snprintf(payload2, sizeof(payload2),
             "{\"sensor\":\"TSL2561\",\"id\":1,\"lux\":%.2f,\"ch0\":%u,\"ch1\":%u}",
             tsl_lux, tsl_ch0, tsl_ch1);

    mqttPublishString(MQTT_TOPIC, payload2, false);

    Serial.print("MQTT -> ");
    Serial.println(payload2);
  }
  
  // Esperar 2 segundos antes de la siguiente lectura
  delay(2000);
}
