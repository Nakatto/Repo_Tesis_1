#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>

// Define para habilitar/deshabilitar MQTT completamente
// Descomenta esta línea para habilitar conexión y publicación en MQTT
// #define ENABLE_MQTT

/**
 * @brief Inicializa la conexión MQTT
 * @param host Dirección del broker MQTT
 * @param port Puerto del broker MQTT
 * @param clientId ID único del cliente MQTT
 * @return true si se conectó exitosamente, false en caso contrario
 */
bool mqttInit(const char* host, uint16_t port, const char* clientId);

/**
 * @brief Define topic de estado (online/offline) para el dispositivo
 * @param topic Topic a usar para estado, por ejemplo: lab/lux/esp32_01/status
 */
void mqttSetStatusTopic(const char* topic);

/**
 * @brief Verifica si está conectado a MQTT
 * @return true si está conectado, false en caso contrario
 */
bool mqttIsConnected();

/**
 * @brief Mantiene viva la conexión MQTT (debe llamarse en el loop)
 */
void mqttKeepAlive();

/**
 * @brief Publica un mensaje en un topic MQTT
 * @param topic Topic donde publicar
 * @param payload Mensaje a publicar
 * @param retain Si se quiere retener el mensaje en el broker
 * @return true si se publicó exitosamente, false en caso contrario
 */
bool mqttPublish(const char* topic, const char* payload, bool retain = false);

/**
 * @brief Publica un valor float en un topic MQTT
 * @param topic Topic donde publicar
 * @param value Valor a publicar
 * @param decimals Número de decimales a mostrar
 * @param retain Si se quiere retener el mensaje en el broker
 * @return true si se publicó exitosamente, false en caso contrario
 */
bool mqttPublishFloat(const char* topic, float value, uint8_t decimals = 2, bool retain = false);

/**
 * @brief Publica un entero en un topic MQTT
 * @param topic Topic donde publicar
 * @param value Valor a publicar
 * @param retain Si se quiere retener el mensaje en el broker
 * @return true si se publicó exitosamente, false en caso contrario
 */
bool mqttPublishInt(const char* topic, int value, bool retain = false);

/**
 * @brief Publica un string en un topic MQTT
 * @param topic Topic donde publicar
 * @param payload String a publicar
 * @param retain Si se quiere retener el mensaje en el broker
 * @return true si se publicó exitosamente, false en caso contrario
 */
bool mqttPublishString(const char* topic, const char* payload, bool retain = false);

/**
 * @brief Desconecta de MQTT
 */
void mqttDisconnect();

/**
 * @brief Imprime información de la conexión MQTT
 */
void mqttPrintInfo();

#endif // MQTT_MANAGER_H
