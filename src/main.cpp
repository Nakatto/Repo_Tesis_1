#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h> // nueva inclusión para I2C
#include <HTTPClient.h> // nueva inclusión para HTTP
#include <WiFiClient.h> // para cliente TCP

// Configuración de la red Wi-Fi
const char* ssid = "Stark-C6";       // Reemplaza con el nombre de tu red Wi-Fi
const char* password = "WinterIsComing-C6"; // Reemplaza con la contraseña de tu red Wi-Fi

// Añadir configuración del TSL2561
#define TSL_ADDR 0x39 // dirección por defecto (0x29, 0x39 o 0x49 según wiring)

// URL de la Raspberry (ajusta a tu IP/puerto/ruta) -> usar el endpoint que funciona con curl
const char* serverUrl = "http://192.168.1.129:5000/api/lux"; // <-- actualizado

void tslWrite8(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(TSL_ADDR);
    Wire.write(0x80 | reg); // bit comando
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t tslRead8(uint8_t reg) {
    Wire.beginTransmission(TSL_ADDR);
    Wire.write(0x80 | reg);
    Wire.endTransmission();
    Wire.requestFrom(TSL_ADDR, (uint8_t)1);
    if (Wire.available()) return Wire.read();
    return 0;
}

// Nueva: leer canal 1 (raw, 16 bits)
uint16_t readChannel1Raw() {
    uint16_t low = (uint16_t)tslRead8(0x0E);
    uint16_t high = (uint16_t)tslRead8(0x0F);
    return (high << 8) | low;
}

// Nueva: enviar JSON {"lux":<valor>} a la Raspberry con HTTP POST (mejorado)
void postLux(float lux) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi no conectado, no se envía");
        return;
    }

    WiFiClient client;
    HTTPClient http;
    Serial.print("POST a: ");
    Serial.println(serverUrl);
    Serial.print("JSON: ");
    String json = String("{\"lux\":") + String(lux, 2) + String("}");
    Serial.println(json);

    // Usar overload con WiFiClient y aumentar timeout
    http.begin(client, serverUrl);
    http.setTimeout(10000); // 10s

    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(json);

    if (httpCode > 0) {
        Serial.printf("POST -> code: %d\n", httpCode);
        String resp = http.getString();
        Serial.println(resp);
    } else {
        // httpCode <= 0 -> error de conexión/tiempo de espera
        Serial.printf("POST falló, httpCode: %d\n", httpCode);
        // información adicional útil para depurar
        if (!client.connected()) {
            Serial.println("Cliente TCP no conectado al servidor (posible IP/puerto incorrecto o servidor no escuchando).");
        } else {
            Serial.println("Cliente TCP aparentemente conectado; revisar servidor.");
        }
    }
    http.end();
}

bool tslInit() {
    Wire.begin();
    // Leer ID solo como información (no obligatorio)
    uint8_t id = tslRead8(0x0A);
    // Encender sensor
    tslWrite8(0x00, 0x03); // CONTROL: power ON
    // Configurar timing: 402ms y gain x16 (0x02 + 0x10 = 0x12) -> alta sensibilidad
    tslWrite8(0x01, 0x12); // TIMING
    delay(500);
    Serial.print("TSL2561 ID: 0x");
    Serial.println(id, HEX);
    return true;
}

void printLuxRaw() {
    uint16_t ch0 = (uint16_t)tslRead8(0x0C) | ((uint16_t)tslRead8(0x0D) << 8);
    uint16_t ch1 = (uint16_t)tslRead8(0x0E) | ((uint16_t)tslRead8(0x0F) << 8);
    Serial.print("TSL2561 ch0: ");
    Serial.print(ch0);
    Serial.print("  ch1: ");
    Serial.println(ch1);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Conectando a Wi-Fi...");

    // Conexión a la red Wi-Fi
    WiFi.begin(ssid, password);

    // Esperar hasta que se conecte
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Intentando conectar...");
    }

    Serial.println("Conectado a Wi-Fi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());

    // Después de conectar Wi‑Fi:
    if (tslInit()) {
        Serial.println("TSL2561 inicializado.");
    } else {
        Serial.println("TSL2561: inicialización fallida.");
    }
}

void loop() {
    // Aquí puedes añadir tu lógica principal
    delay(1000);
    Serial.println("El dispositivo está funcionando correctamente.");

    // Leer canal 1 y enviar como "lux"
    uint16_t ch1 = readChannel1Raw();
    float lux = (float)ch1; // aquí tomamos el valor bruto del canal 1 como 'lux'
    Serial.print("Canal 1 (raw) -> lux: ");
    Serial.println(lux, 2);

    postLux(lux);

    delay(2000);
}