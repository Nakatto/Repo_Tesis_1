#include "wifi_manager.h"
#include <WiFi.h>

bool wifiInit(const char* ssid, const char* password, unsigned long timeoutMs) {
  Serial.println("\n=== Iniciando conexión WiFi ===");
  Serial.print("SSID: ");
  Serial.println(ssid);
  
  // Configurar modo estación (cliente)
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("Conectando");
  unsigned long startTime = millis();
  
  // Esperar conexión con timeout
  while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < timeoutMs) {
    delay(500);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n¡Conectado exitosamente!");
    wifiPrintInfo();
    return true;
  } else {
    Serial.println("\n¡Error! No se pudo conectar al WiFi");
    Serial.print("Estado: ");
    Serial.println(WiFi.status());
    return false;
  }
}

bool wifiIsConnected() {
  return WiFi.status() == WL_CONNECTED;
}

String wifiGetIP() {
  return WiFi.localIP().toString();
}

int wifiGetRSSI() {
  return WiFi.RSSI();
}

void wifiPrintInfo() {
  Serial.println("Información de conexión:");
  Serial.print("  IP: ");
  Serial.println(wifiGetIP());
  Serial.print("  RSSI: ");
  Serial.print(wifiGetRSSI());
  Serial.println(" dBm");
  Serial.print("  MAC: ");
  Serial.println(WiFi.macAddress());
}

void wifiDisconnect() {
  WiFi.disconnect();
  Serial.println("WiFi desconectado");
}
