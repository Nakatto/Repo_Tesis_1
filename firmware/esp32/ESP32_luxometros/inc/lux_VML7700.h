#ifndef LUX_VML7700_H
#define LUX_VML7700_H

#include <Arduino.h>
#include "Adafruit_VEML7700.h"

/**
 * @brief Inicializa el sensor VEML7700
 * @param sdaPin Pin SDA para comunicación I2C
 * @param sclPin Pin SCL para comunicación I2C
 * @return true si la inicialización fue exitosa, false en caso contrario
 */
bool luxInit(int sdaPin, int sclPin);

/**
 * @brief Lee la iluminancia en lux
 * @return Valor de iluminancia en lux
 */
float luxRead();

/**
 * @brief Lee el valor de luz blanca
 * @return Valor de luz blanca
 */
float luxReadWhite();

/**
 * @brief Lee el valor raw del sensor ALS (Ambient Light Sensor)
 * @return Valor raw del ALS
 */
uint16_t luxReadALS();

/**
 * @brief Obtiene la clasificación del nivel de iluminación
 * @param lux Valor de iluminancia en lux
 * @return Cadena con la descripción del nivel de iluminación
 */
const char* luxGetLevel(float lux);

/**
 * @brief Imprime todas las mediciones del sensor por Serial
 */
void luxPrintMeasurements();

/**
 * @brief Imprime la configuración actual del sensor
 */
void luxPrintConfig();

#endif // LUX_VML7700_H
