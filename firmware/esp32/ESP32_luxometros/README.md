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
