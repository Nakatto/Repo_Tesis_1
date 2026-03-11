#ifndef TSL_H   
#define TSL_H

#include <Arduino.h>

#define TSL_ADDR 0x39 // dirección por defecto (0x29, 0x39 o 0x49 según wiring)

void tslWrite8(uint8_t reg, uint8_t value);
uint8_t tslRead8(uint8_t reg);
uint16_t readChannel1Raw();
void readChannels(uint16_t &ch0, uint16_t &ch1);
bool tslInit();
float tsl_computeLux(uint16_t ch0, uint16_t ch1);

/**
 * @brief Lee todos los valores del sensor TSL2561 y los almacena internamente
 */
void lee_TSL2561();

/**
 * @brief Obtiene el último valor del canal 0 (broadband) leído
 * @return Último valor del canal 0
 */
uint16_t get_ultimo_ch0();

/**
 * @brief Obtiene el último valor del canal 1 (IR) leído
 * @return Último valor del canal 1
 */
uint16_t get_ultimo_ch1();

/**
 * @brief Obtiene el último valor de lux calculado
 * @return Último valor de iluminancia en lux
 */
float get_ultimo_tsl_lux();

/**
 * @brief Imprime por serial los últimos valores leídos del TSL2561
 */
void logs_valores_TSL2561();

#endif