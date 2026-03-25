#include <Arduino.h>
#include <Wire.h>
#include "lux_VML7700.h"
#include "tsl2561.h"
#include "tsl2591.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

// Credenciales WiFi
#define WIFI_SSID "Stark-C6"
#define WIFI_PASSWORD "WinterIsComing-C6"

// Credenciales MQTT
//const char* MQTT_HOST = "test.mosquitto.org";
const char* MQTT_HOST = "192.168.1.168";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "esp32_01_lux";      // Que sea único
const char* MQTT_TOPIC_BASE = "lab/lux/esp32_01"; // Base de topics para Influx/Telegraf

// Pines I2C (puedes ajustar según tu configuración)
#define SDA_PIN 21
#define SCL_PIN 22
#define VEML7700_ADDR 0x10

// Variables globales para rastrear sensores disponibles
bool veml7700_available = false;
bool tsl2561_available = false;
bool tsl2591_available = false;

void scanI2C(int sdaPin, int sclPin) {
  Wire.begin(sdaPin, sclPin);
  Serial.println("\nI2C scan...");

  int found = 0;
  bool foundVeml7700 = false;
  bool foundTsl2561 = false;
  bool foundTsl2591 = false;
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
      } else if (addr == TSL2591_ADDR) {
        label = " (TSL2591)";
        foundTsl2591 = true;
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
  if (!foundTsl2591) {
    Serial.printf("Aviso: no se detectó TSL2591 en 0x%02X\n", TSL2591_ADDR);
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

  // Intentar inicializar sensor VEML7700
  if (luxInit(SDA_PIN, SCL_PIN)) {
    veml7700_available = true;
    Serial.println("Sensor VEML7700 inicializado correctamente");
    luxPrintConfig();
  } else {
    Serial.println("Aviso: No se pudo inicializar el sensor VEML7700");
  }
  
  Serial.println();
  
  // Intentar inicializar sensor TSL2561
  if (tslInit()) {
    tsl2561_available = true;
    Serial.println("Sensor TSL2561 inicializado correctamente");
  } else {
    Serial.println("Aviso: No se pudo inicializar el sensor TSL2561");
  }
  
  Serial.println();
  
  // Intentar inicializar sensor TSL2591
  if (tsl2591Init()) {
    tsl2591_available = true;
    Serial.println("Sensor TSL2591 inicializado correctamente");
  } else {
    Serial.println("Aviso: No se pudo inicializar el sensor TSL2591");
  }
  
  Serial.println();
  
  // Verificar que al menos un sensor esté disponible
  if (!veml7700_available && !tsl2561_available && !tsl2591_available) {
    Serial.println("ERROR CRITICO: No hay sensores de luz disponibles");
    Serial.println("Verifica las conexiones I2C (SDA, SCL) y direcciones de los dispositivos");
    while (1) {
      delay(1000);
    }
  }
  
  Serial.println("Iniciando mediciones...\n");
}

void loop() {
  // Mantener viva la conexión MQTT
  mqttKeepAlive();

  Serial.println("=================================");

  // Variables para VEML7700
  float lux = 0;
  float white = 0;
  uint16_t als = 0;

  // Leer mediciones del sensor VEML7700 si está disponible
  if (veml7700_available) {
    lux = luxRead();
    white = luxReadWhite();
    als = luxReadALS();

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
  }

  // Variables para TSL2561
  uint16_t tsl_ch0 = 0, tsl_ch1 = 0;
  float tsl_lux = 0;

  // Leer mediciones del sensor TSL2561 si está disponible
  if (tsl2561_available) {
    if (veml7700_available) {
      Serial.println();
    }
    readChannels(tsl_ch0, tsl_ch1);
    tsl_lux = tsl_computeLux(tsl_ch0, tsl_ch1);

    Serial.println("TSL2561:");
    Serial.print("Iluminancia: ");
    Serial.print(tsl_lux);
    Serial.println(" lux");
    Serial.print("CH0 (Broadband): ");
    Serial.println(tsl_ch0);
    Serial.print("CH1 (IR): ");
    Serial.println(tsl_ch1);
  }

  // Variables para TSL2591
  uint16_t tsl2591_full = 0, tsl2591_ir = 0;
  float tsl2591_lux = 0;

  // Leer mediciones del sensor TSL2591 si está disponible
  if (tsl2591_available) {
    if (veml7700_available || tsl2561_available) {
      Serial.println();
    }
    tsl2591ReadChannels(tsl2591_full, tsl2591_ir);
    tsl2591_lux = tsl2591ComputeLux(tsl2591_full, tsl2591_ir);

    Serial.println("TSL2591:");
    Serial.print("Iluminancia: ");
    Serial.print(tsl2591_lux);
    Serial.println(" lux");
    Serial.print("CH0 (Full spectrum): ");
    Serial.println(tsl2591_full);
    Serial.print("CH1 (IR): ");
    Serial.println(tsl2591_ir);
  }

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
    char topic[96];

    // Publicar VEML7700 en topics separados si está disponible
    if (veml7700_available) {
      snprintf(topic, sizeof(topic), "%s/veml7700/lux", MQTT_TOPIC_BASE);
      if (mqttPublishFloat(topic, lux, 2, false)) {
        Serial.printf("MQTT -> %s = %.2f\n", topic, lux);
      }

      snprintf(topic, sizeof(topic), "%s/veml7700/white", MQTT_TOPIC_BASE);
      if (mqttPublishFloat(topic, white, 2, false)) {
        Serial.printf("MQTT -> %s = %.2f\n", topic, white);
      }

      snprintf(topic, sizeof(topic), "%s/veml7700/als", MQTT_TOPIC_BASE);
      if (mqttPublishInt(topic, als, false)) {
        Serial.printf("MQTT -> %s = %u\n", topic, als);
      }
    }

    // Publicar TSL2561 en topics separados si está disponible
    if (tsl2561_available) {
      snprintf(topic, sizeof(topic), "%s/tsl2561/lux", MQTT_TOPIC_BASE);
      if (mqttPublishFloat(topic, tsl_lux, 2, false)) {
        Serial.printf("MQTT -> %s = %.2f\n", topic, tsl_lux);
      }

      snprintf(topic, sizeof(topic), "%s/tsl2561/ch0", MQTT_TOPIC_BASE);
      if (mqttPublishInt(topic, tsl_ch0, false)) {
        Serial.printf("MQTT -> %s = %u\n", topic, tsl_ch0);
      }

      snprintf(topic, sizeof(topic), "%s/tsl2561/ch1", MQTT_TOPIC_BASE);
      if (mqttPublishInt(topic, tsl_ch1, false)) {
        Serial.printf("MQTT -> %s = %u\n", topic, tsl_ch1);
      }
    }

    // Publicar TSL2591 en topics separados si está disponible
    if (tsl2591_available) {
      snprintf(topic, sizeof(topic), "%s/tsl2591/lux", MQTT_TOPIC_BASE);
      if (mqttPublishFloat(topic, tsl2591_lux, 2, false)) {
        Serial.printf("MQTT -> %s = %.2f\n", topic, tsl2591_lux);
      }

      snprintf(topic, sizeof(topic), "%s/tsl2591/full", MQTT_TOPIC_BASE);
      if (mqttPublishInt(topic, tsl2591_full, false)) {
        Serial.printf("MQTT -> %s = %u\n", topic, tsl2591_full);
      }

      snprintf(topic, sizeof(topic), "%s/tsl2591/ir", MQTT_TOPIC_BASE);
      if (mqttPublishInt(topic, tsl2591_ir, false)) {
        Serial.printf("MQTT -> %s = %u\n", topic, tsl2591_ir);
      }
    }
  }
  
  // Esperar 2 segundos antes de la siguiente lectura
  delay(2000);
}
