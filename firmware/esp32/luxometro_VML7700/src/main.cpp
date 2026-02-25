#include <Arduino.h>
#include "lux_VML7700.h"
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

void setup() {
  // Inicializar comunicación serial
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Luxómetro VEML7700 ===");

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
  
  Serial.println("\nIniciando mediciones...\n");
}

void loop() {
  // Mantener viva la conexión MQTT
  mqttKeepAlive();

  // Leer mediciones del sensor
  float lux = luxRead();
  float white = luxReadWhite();
  uint16_t als = luxReadALS();

  // Imprimir por serial
  Serial.println("=================================");
  Serial.print("Iluminancia: ");
  Serial.print(lux);
  Serial.println(" lux");
  
  Serial.print("Luz blanca: ");
  Serial.println(white);
  
  Serial.print("ALS (raw): ");
  Serial.println(als);
  
  Serial.print("Nivel: ");
  Serial.println(luxGetLevel(lux));
  Serial.println("=================================\n");

  // Publicar en MQTT si está conectado
  if (mqttIsConnected()) {
    // Publicar lux
    char topicLux[64];
    snprintf(topicLux, sizeof(topicLux), "%s/lux", MQTT_TOPIC);
    mqttPublishFloat(topicLux, lux, 2, false);

    // Publicar luz blanca
    char topicWhite[64];
    snprintf(topicWhite, sizeof(topicWhite), "%s/white", MQTT_TOPIC);
    mqttPublishFloat(topicWhite, white, 2, false);

    // Publicar ALS raw
    char topicALS[64];
    snprintf(topicALS, sizeof(topicALS), "%s/als", MQTT_TOPIC);
    mqttPublishInt(topicALS, als, false);

    Serial.println("Datos publicados en MQTT");
  }
  
  // Esperar 2 segundos antes de la siguiente lectura
  delay(2000);
}
