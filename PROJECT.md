# PROJECT.md — Contexto del Agente

> Este archivo es el punto de entrada para Hermes Agent.
> Inyéctalo al iniciar: `@PROJECT.md`

## Identidad del proyecto

**Nombre:** Sistema Biométrico de Huellas Digitales para Registro de Asistencia Académica  
**Tipo:** Sistema embebido IoT offline  
**Estado actual:** ✅ Fase 4 — Completa (todas las fases implementadas)  
**Lenguaje:** C++ con Arduino SDK  
**MCU:** Arduino Mega 2560 (ATmega2560, AVR)

## Hardware disponible (físico, en mano)

- Arduino Mega 2560
- Sensor de huellas AS608 (UART)
- RTC DS3231 (I2C)
- Módulo microSD + driver
- Pantalla OLED SSD1306 (I2C)
- Buzzer activo
- Pulsador (selector de modo)

## Pines y comunicación

| Componente  | Interfaz | Pines Mega               |
|-------------|----------|--------------------------|
| AS608       | UART     | Serial1 (TX1=18, RX1=19) |
| DS3231      | I2C      | SDA=20, SCL=21           |
| SSD1306     | I2C      | SDA=20, SCL=21           |
| microSD     | SPI      | MOSI=51, MISO=50, SCK=52, CS=53 |
| Buzzer      | Digital  | Pin 12 (constantes.h)    |
| Pulsador    | Digital  | Pin 3 (INPUT_PULLUP)     |

## Restricciones críticas

- ❌ Sin conexión a internet en producción
- ❌ No almacenar imágenes de huellas — solo minutiae/ID
- ❌ No usar librerías que requieran heap dinámico excesivo (RAM limitada: 8KB SRAM)
- ✅ Todo debe funcionar tras corte de energía (RTC con batería)
- ✅ CSV como único formato de salida

## Librerías autorizadas

```
Adafruit-Fingerprint-Sensor-Library  # AS608
RTClib (Adafruit)                    # DS3231
SD (built-in Arduino)                # microSD
Adafruit_SSD1306                     # OLED
Adafruit_GFX                         # OLED base
Wire (built-in)                      # I2C
SPI (built-in)                       # SPI
```

## Estructura de archivos relevantes

Todo el firmware está en `src/main/` (flat, sin subdirectorios):

```
src/main/
  constantes.h       # Pines, CSV, modos, buffers
  buzzer.cpp/h       # Beep patterns (extraído de pantalla)
  pantalla.cpp/h     # OLED display (sin buzzer)
  rtc_helper.cpp/h   # DS3231 RTC
  almacenamiento.cpp/h # microSD, CSV, formatearCSVs()
  enrolamiento.cpp/h   # AS608, capturarHuella(), limpiarHuellas()
  asistencia.cpp/h     # Toma de asistencia, corrección
  verificador.cpp/h    # Periferico{} array de salud
  modos.cpp/h          # HANDLERS_MODO[] array, ejecutarModo()
  main.ino             # setup(), loop(), formatearSistema()
```

## Formato CSV de salida

```
ID,Fecha,Hora
1,2026-06-05,08:30:00
2,2026-06-05,08:31:12
```

Archivo: `ASIST.CSV` en raíz de microSD.

## Contexto adicional

- Ver `docs/dependencies.md` para versiones exactas de librerías
- Ver `docs/architecture.md` para diagramas de flujo
- Ver `docs/phases.md` para estado de cada fase
- Ver `.hermes/rules.md` para reglas de trabajo del agente
