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

/**
 * @brief Lee todos los valores del sensor VEML7700 y los almacena internamente
 */
void lee_VEML7700();

/**
 * @brief Obtiene el último valor de lux leído
 * @return Último valor de iluminancia en lux
 */
float get_ultimo_lux();

/**
 * @brief Obtiene el último valor de white leído
 * @return Último valor de luz blanca
 */
float get_ultimo_white();

/**
 * @brief Obtiene el último valor de ALS leído
 * @return Último valor raw del ALS
 */
uint16_t get_ultimo_als();

/**
 * @brief Imprime por serial los últimos valores leídos del VEML7700
 */
void logs_valores_VML7700();

#endif // LUX_VML7700_H
