#include "mqtt_manager.h"
#include <PubSubClient.h>
#include <WiFi.h>

// Cliente WiFi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Variables de estado
static bool mqtt_connected = false;
static char mqtt_client_id[64] = {0};  // Guardar clientId para reconexiones
static unsigned long last_reconnect_attempt_ms = 0;
static const unsigned long RECONNECT_INTERVAL_MS = 5000;

// Función de callback para mensajes recibidos
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido en topic: ");
  Serial.print(topic);
  Serial.print(" => ");
  
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

bool mqttInit(const char* host, uint16_t port, const char* clientId) {
#ifdef ENABLE_MQTT
  Serial.println("\n=== Iniciando conexión MQTT ===");
  Serial.print("Broker: ");
  Serial.print(host);
  Serial.print(":");
  Serial.println(port);
  Serial.print("Client ID: ");
  Serial.println(clientId);
  
  // Guardar clientId para reconexiones
  strncpy(mqtt_client_id, clientId, sizeof(mqtt_client_id) - 1);
  
  // Configurar cliente MQTT
  client.setServer(host, port);
  client.setCallback(mqttCallback);
  
  // Intentar conectar
  if (client.connect(clientId)) {
    mqtt_connected = true;
    last_reconnect_attempt_ms = 0;
    Serial.println("¡Conectado a MQTT exitosamente!");
    mqttPrintInfo();
    return true;
  } else {
    mqtt_connected = false;
    Serial.print("¡Error! Código de conexión MQTT: ");
    Serial.println(client.state());
    return false;
  }
#else
  Serial.println("MQTT deshabilitado (define ENABLE_MQTT no activo)");
  return false;
#endif
}

bool mqttIsConnected() {
  return client.connected();
}

void mqttKeepAlive() {
#ifdef ENABLE_MQTT
  // Reintentar conexión periódicamente cuando está desconectado.
  if (!client.connected()) {
    unsigned long now = millis();
    if (last_reconnect_attempt_ms == 0 || (now - last_reconnect_attempt_ms) >= RECONNECT_INTERVAL_MS) {
      last_reconnect_attempt_ms = now;
      Serial.println("Reconectando a MQTT...");
      if (client.connect(mqtt_client_id)) {
        mqtt_connected = true;
        Serial.println("Reconectado a MQTT");
      } else {
        Serial.print("Reintento MQTT fallido. Código: ");
        Serial.println(client.state());
      }
    }
  }
  
  // Mantener comunicación MQTT
  if (client.connected()) {
    client.loop();
  }
#endif
}

bool mqttPublish(const char* topic, const char* payload, bool retain) {
#ifdef ENABLE_MQTT
  if (!client.connected()) {
    return false;
  }
  
  return client.publish(topic, payload, retain);
#else
  // MQTT está deshabilitado
  return false;
#endif
}

bool mqttPublishFloat(const char* topic, float value, uint8_t decimals, bool retain) {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%.*f", decimals, value);
  return mqttPublish(topic, buffer, retain);
}

bool mqttPublishInt(const char* topic, int value, bool retain) {
  char buffer[16];
  snprintf(buffer, sizeof(buffer), "%d", value);
  return mqttPublish(topic, buffer, retain);
}

bool mqttPublishString(const char* topic, const char* payload, bool retain) {
  return mqttPublish(topic, payload, retain);
}

void mqttDisconnect() {
  client.disconnect();
  mqtt_connected = false;
  Serial.println("MQTT desconectado");
}

void mqttPrintInfo() {
  Serial.println("Información MQTT:");
  Serial.print("  Conectado: ");
  Serial.println(mqttIsConnected() ? "Sí" : "No");
}
