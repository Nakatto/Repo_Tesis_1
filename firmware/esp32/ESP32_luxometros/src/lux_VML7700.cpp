#include "lux_VML7700.h"
#include <Wire.h>

// Instancia estática del sensor
static Adafruit_VEML7700 veml = Adafruit_VEML7700();

bool luxInit(int sdaPin, int sclPin) {
  // Inicializar I2C
  Wire.begin(sdaPin, sclPin);
  
  // Inicializar sensor VEML7700
  if (!veml.begin()) {
    return false;
  }
  
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
  
  return true;
}

float luxRead() {
  return veml.readLux();
}

float luxReadWhite() {
  return veml.readWhite();
}

uint16_t luxReadALS() {
  return veml.readALS();
}

const char* luxGetLevel(float lux) {
  if (lux < 1) {
    return "Oscuridad";
  } else if (lux < 50) {
    return "Muy bajo (ej: noche con luna)";
  } else if (lux < 200) {
    return "Bajo (ej: luz de vela, atardecer)";
  } else if (lux < 400) {
    return "Medio (ej: oficina con luz artificial)";
  } else if (lux < 1000) {
    return "Buena iluminación interior";
  } else if (lux < 10000) {
    return "Muy brillante (ej: día nublado)";
  } else {
    return "Luz solar directa";
  }
}

void luxPrintMeasurements() {
  // Leer todas las mediciones
  float lux = luxRead();
  float white = luxReadWhite();
  uint16_t als = luxReadALS();
  
  // Mostrar resultados
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
}

void luxPrintConfig() {
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
}
