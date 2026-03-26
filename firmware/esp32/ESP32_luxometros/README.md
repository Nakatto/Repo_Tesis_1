# Luxómetro ESP32 con VEML7700

Proyecto de firmware para ESP32 que mide la iluminancia (lux) usando el sensor VEML7700.

## Hardware Requerido

- **ESP32** (cualquier variante)
- **Sensor VEML7700** (módulo I2C)
- Cables de conexión

## Conexiones I2C

| VEML7700 | ESP32 |
|----------|-------|
| VCC      | 3.3V  |
| GND      | GND   |
| SDA      | GPIO21|
| SCL      | GPIO22|

**Nota:** Si usas otros pines, modifica las definiciones `SDA_PIN` y `SCL_PIN` en el código.

## Características del Sensor VEML7700

- Rango de medición: 0 a ~120,000 lux
- Interfaz: I2C
- Dirección I2C: 0x10 (fija)
- Bajo consumo de energía
- Respuesta espectral similar al ojo humano

## Configuración del Proyecto

### Opción 1: PlatformIO (Recomendado)

1. Instala [PlatformIO](https://platformio.org/) en VS Code
2. Abre esta carpeta como proyecto PlatformIO
3. Compila y sube el código:
   ```bash
   pio run --target upload
   ```
4. Abre el monitor serial:
   ```bash
   pio device monitor
   ```

### Opción 2: Arduino IDE

1. Copia el contenido de `src/main.cpp` a un nuevo sketch
2. Instala las librerías necesarias:
   - Adafruit VEML7700 Library
   - Adafruit BusIO
3. Selecciona tu placa ESP32
4. Compila y sube

## Pipeline MQTT -> Telegraf -> InfluxDB -> Grafana

El firmware publica métricas numéricas en topics con este formato:

```
lab/lux/<device>/<sensor>/<metric>
```

Ejemplos:

```
lab/lux/esp32_01/veml7700/lux
lab/lux/esp32_01/tsl2561/ch0
lab/lux/esp32_01/tsl2591/ir
lab/lux/esp32_01/wifi/rssi
lab/lux/esp32_01/system/status
```

Notas:

- `system/status`: `1` cuando el dispositivo está conectado y `0` por Last Will si se desconecta inesperadamente.
- `wifi/rssi`: intensidad de señal WiFi en dBm.

### Telegraf

Usa `telegraf.mqtt.conf` para consumir `lab/lux/esp32_01/#`.
El bloque `processors.regex` crea las tags:

- `device`
- `sensor`
- `metric`

Con esto, en Influx tendrás la medición `luxometro` con el campo `value` y tags que Grafana puede filtrar fácilmente.

### Consultas Flux para Grafana (InfluxDB 2.x)

Serie de lux del VEML7700:

```flux
from(bucket: "TU_BUCKET")
   |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
   |> filter(fn: (r) => r._measurement == "luxometro")
   |> filter(fn: (r) => r._field == "value")
   |> filter(fn: (r) => r.device == "esp32_01")
   |> filter(fn: (r) => r.sensor == "veml7700")
   |> filter(fn: (r) => r.metric == "lux")
   |> aggregateWindow(every: v.windowPeriod, fn: mean, createEmpty: false)
   |> yield(name: "mean")
```

Estado del dispositivo (`1` online, `0` offline):

```flux
from(bucket: "TU_BUCKET")
   |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
   |> filter(fn: (r) => r._measurement == "luxometro")
   |> filter(fn: (r) => r._field == "value")
   |> filter(fn: (r) => r.device == "esp32_01")
   |> filter(fn: (r) => r.sensor == "system")
   |> filter(fn: (r) => r.metric == "status")
   |> last()
```

## Monitor Serial

El proyecto muestra las lecturas cada 2 segundos en el monitor serial (115200 baudios):

```
=================================
Iluminancia: 342.50 lux
Luz blanca: 450.23
ALS (raw): 1234
Nivel: Medio (ej: oficina con luz artificial)
=================================
```

## Configuración del Sensor

Puedes ajustar la ganancia y el tiempo de integración en `setup()`:

**Ganancia:**
- `VEML7700_GAIN_1_8` - Para muy alta luminosidad (sol directo)
- `VEML7700_GAIN_1_4` - Para alta luminosidad
- `VEML7700_GAIN_1` - Uso general (por defecto)
- `VEML7700_GAIN_2` - Para baja luminosidad

**Tiempo de Integración:**
- `VEML7700_IT_100MS` - Rápido (por defecto)
- `VEML7700_IT_200MS`
- `VEML7700_IT_400MS`
- `VEML7700_IT_800MS` - Mayor precisión en baja luz

## Referencias de Iluminancia

| Condición | Lux aprox. |
|-----------|------------|
| Luz solar directa | 32,000 - 100,000 |
| Día nublado | 1,000 - 10,000 |
| Oficina bien iluminada | 320 - 500 |
| Sala de estar | 50 - 150 |
| Luz de vela | 10 - 15 |
| Luna llena | 0.1 - 1 |

## Solución de Problemas

**Error: No se pudo encontrar el sensor**
- Verifica las conexiones I2C
- Asegúrate de que el sensor esté alimentado con 3.3V
- Verifica que los pines SDA y SCL estén correctos
- Prueba con un escáner I2C para detectar la dirección 0x10

**Lecturas inestables**
- Añade capacitores de desacoplo (0.1µF) cerca del sensor
- Usa cables más cortos
- Ajusta el tiempo de integración

## Autor

Proyecto de tesis - ESP32 Luxómetro VEML7700

## Licencia

MIT
