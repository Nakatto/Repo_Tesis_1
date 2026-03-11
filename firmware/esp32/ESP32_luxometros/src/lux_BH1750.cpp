#include "lux_BH1750.h"
#include <Wire.h>

// Dirección I2C del BH1750
#define BH1750_ADDR 0x23  // Dirección con pin ADDR a GND (alternativa: 0x5C si ADDR a VCC)

// Comandos del BH1750
#define BH1750_POWER_ON 0x01
#define BH1750_RESET 0x07
#define BH1750_CONT_HIGH_RES 0x10  // Resolve: 1 lux, Tiempo: 120-180ms

// Variable interna para almacenar lectura
static float ultimo_bh1750_lux = 0.0f;

bool luxBH1750Init(int sdaPin, int sclPin) {
  // Inicializar I2C si aún no está inicializado
  // (Nota: si ya se inicializó en VEML7700, esto es seguro)
  Wire.begin(sdaPin, sclPin);
  
  // Verificar presencia del sensor
  Wire.beginTransmission(BH1750_ADDR);
  if (Wire.endTransmission() != 0) {
    return false;
  }
  
  // Enviar comando de reset
  Wire.beginTransmission(BH1750_ADDR);
  Wire.write(BH1750_RESET);
  Wire.endTransmission();
  delay(10);
  
  // Enviar comando de encendido
  Wire.beginTransmission(BH1750_ADDR);
  Wire.write(BH1750_POWER_ON);
  Wire.endTransmission();
  delay(10);
  
  // Enviar modo de lectura: resolución alta continua
  Wire.beginTransmission(BH1750_ADDR);
  Wire.write(BH1750_CONT_HIGH_RES);
  Wire.endTransmission();
  delay(180);  // Esperar tiempo de integración
  
  return true;
}

void lee_BH1750() {
  // Solicitar 2 bytes del sensor
  Wire.requestFrom(BH1750_ADDR, (uint8_t)2);
  
  if (Wire.available() >= 2) {
    uint16_t raw = (Wire.read() << 8) | Wire.read();
    // Convertir a lux: valor bruto / 1.2
    ultimo_bh1750_lux = raw / 1.2f;
  } else {
    ultimo_bh1750_lux = 0.0f;
  }
}

float get_ultimo_bh1750_lux() {
  return ultimo_bh1750_lux;
}

void logs_valores_BH1750() {
  Serial.println("\nBH1750:");
  Serial.print("Iluminancia: ");
  Serial.print(ultimo_bh1750_lux, 2);
  Serial.println(" lux");
}
