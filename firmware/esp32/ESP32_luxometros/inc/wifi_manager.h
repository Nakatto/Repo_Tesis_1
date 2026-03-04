#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

/**
 * @brief Inicializa y conecta a la red WiFi
 * @param ssid Nombre de la red WiFi
 * @param password Contraseña de la red WiFi
 * @param timeoutMs Tiempo máximo de espera en milisegundos (default: 10000)
 * @return true si se conectó exitosamente, false en caso contrario
 */
bool wifiInit(const char* ssid, const char* password, unsigned long timeoutMs = 10000);

/**
 * @brief Verifica si está conectado a WiFi
 * @return true si está conectado, false en caso contrario
 */
bool wifiIsConnected();

/**
 * @brief Obtiene la dirección IP asignada
 * @return String con la dirección IP
 */
String wifiGetIP();

/**
 * @brief Obtiene la intensidad de señal WiFi (RSSI)
 * @return Valor RSSI en dBm
 */
int wifiGetRSSI();

/**
 * @brief Imprime información de la conexión WiFi
 */
void wifiPrintInfo();

/**
 * @brief Desconecta de la red WiFi
 */
void wifiDisconnect();

#endif // WIFI_MANAGER_H
