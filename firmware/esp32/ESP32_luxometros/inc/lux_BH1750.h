#ifndef LUX_BH1750_H
#define LUX_BH1750_H

#include <Arduino.h>

/**
 * @brief Inicializa el sensor BH1750
 * @param sdaPin Pin SDA para comunicación I2C
 * @param sclPin Pin SCL para comunicación I2C
 * @return true si la inicialización fue exitosa, false en caso contrario
 */
bool luxBH1750Init(int sdaPin, int sclPin);

/**
 * @brief Lee el valor de iluminancia del BH1750 y lo almacena internamente
 */
void lee_BH1750();

/**
 * @brief Obtiene el último valor de lux leído del BH1750
 * @return Último valor de iluminancia en lux
 */
float get_ultimo_bh1750_lux();

/**
 * @brief Imprime por serial los últimos valores leídos del BH1750
 */
void logs_valores_BH1750();

#endif // LUX_BH1750_H
