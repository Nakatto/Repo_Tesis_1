#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_VEML7700.h"

// Crear instancia del sensor VEML7700
Adafruit_VEML7700 veml = Adafruit_VEML7700();

// Pines I2C (puedes ajustar según tu configuración)
#define SDA_PIN 21
#define SCL_PIN 22

void setup() {
  // Inicializar comunicación serial
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Luxómetro VEML7700 ===");

  // Inicializar I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Inicializar sensor VEML7700
  if (!veml.begin()) {
    Serial.println("Error: No se pudo encontrar el sensor VEML7700");
    Serial.println("Verifica las conexiones I2C (SDA, SCL)");
    while (1) {
      delay(1000);
    }
  }
  
  Serial.println("Sensor VEML7700 inicializado correctamente");
  
  // Configurar ganancia y tiempo de integración
  // VEML7700_GAIN_1    = 1x (para alta luminosidad)
  // VEML7700_GAIN_2    = 2x
  // VEML7700_GAIN_1_8  = 1/8 (para muy alta luminosidad)
  // VEML7700_GAIN_1_4  = 1/4
  veml.setGain(VEML7700_GAIN_1);
  
  // VEML7700_IT_100MS = 100ms
  // VEML7700_IT_200MS = 200ms
  // VEML7700_IT_400MS = 400ms
  // VEML7700_IT_800MS = 800ms
  veml.setIntegrationTime(VEML7700_IT_100MS);
  
  Serial.println("Configuración:");
  Serial.print("  Ganancia: ");
  switch(veml.getGain()) {
    case VEML7700_GAIN_1: Serial.println("1x"); break;
    case VEML7700_GAIN_2: Serial.println("2x"); break;
    case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
    case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
  }
  
  Serial.print("  Tiempo integración: ");
  switch(veml.getIntegrationTime()) {
    case VEML7700_IT_100MS: Serial.println("100ms"); break;
    case VEML7700_IT_200MS: Serial.println("200ms"); break;
    case VEML7700_IT_400MS: Serial.println("400ms"); break;
    case VEML7700_IT_800MS: Serial.println("800ms"); break;
  }
  
  Serial.println("\nIniciando mediciones...\n");
}

void loop() {
  // Leer lux (iluminancia en lux)
  float lux = veml.readLux();
  
  // Leer luz blanca (opcional)
  float white = veml.readWhite();
  
  // Leer ALS (Ambient Light Sensor - valor raw)
  uint16_t als = veml.readALS();
  
  // Mostrar resultados
  Serial.println("=================================");
  Serial.print("Iluminancia: ");
  Serial.print(lux);
  Serial.println(" lux");
  
  Serial.print("Luz blanca: ");
  Serial.println(white);
  
  Serial.print("ALS (raw): ");
  Serial.println(als);
  
  // Clasificación de nivel de iluminación
  Serial.print("Nivel: ");
  if (lux < 1) {
    Serial.println("Oscuridad");
  } else if (lux < 50) {
    Serial.println("Muy bajo (ej: noche con luna)");
  } else if (lux < 200) {
    Serial.println("Bajo (ej: luz de vela, atardecer)");
  } else if (lux < 400) {
    Serial.println("Medio (ej: oficina con luz artificial)");
  } else if (lux < 1000) {
    Serial.println("Buena iluminación interior");
  } else if (lux < 10000) {
    Serial.println("Muy brillante (ej: día nublado)");
  } else {
    Serial.println("Luz solar directa");
  }
  
  Serial.println("=================================\n");
  
  // Esperar 2 segundos antes de la siguiente lectura
  delay(2000);
}
