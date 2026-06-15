# PROJECT.md — Contexto del Agente

> Este archivo es el punto de entrada para Hermes Agent.
> Inyéctalo al iniciar: `@PROJECT.md`

## Identidad del proyecto

**Nombre:** Sistema Biométrico de Huellas Digitales para Registro de Asistencia Académica  
**Tipo:** Sistema embebido IoT offline  
**Estado actual:** Fase 1 — Completa (todos los módulos probados: OLED, RTC, AS608, microSD, buzzer, pulsador)  
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
| Buzzer      | Digital  | Pin 8                    |
| Pulsador    | Digital  | Pin 7 (INPUT_PULLUP)     |

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

```
src/enrollment/   → enrolamiento.cpp / enrolamiento.h
src/attendance/   → asistencia.cpp / asistencia.h
src/storage/      → almacenamiento.cpp / almacenamiento.h
src/display/      → pantalla.cpp / pantalla.h
src/utils/        → rtc_helper.cpp, constantes.h
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
