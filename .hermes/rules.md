# .hermes/rules.md — Reglas del Agente

Estas reglas aplican en todas las sesiones de trabajo en este proyecto.

## Lenguaje y entorno

- Todo el código es **C++ con Arduino SDK** para **ATmega2560**
- No sugerir MicroPython, CircuitPython ni ningún otro runtime — el MCU es AVR
- Usar `Serial1` para AS608, nunca `Serial` (reservado para debug)
- Asumir **8KB SRAM** disponible — evitar malloc/new, preferir buffers estáticos
- Flash: 256KB — mantener binario bajo 200KB compilado

## Estilo de código

- Un archivo `.cpp` + `.h` por módulo (ver estructura en PROJECT.md)
- Nombres de variables y funciones en **camelCase**, constantes en **UPPER_SNAKE_CASE**
- Comentarios en **español**
- Siempre incluir guard headers (`#ifndef / #define / #endif`)
- No usar `String` de Arduino — usar `char[]` para evitar fragmentación de heap

## Restricciones de hardware

- I2C compartido entre DS3231 y SSD1306 — verificar direcciones antes de escribir código
  - SSD1306: 0x3C
  - DS3231: 0x68
- SPI exclusivo para microSD — no agregar otros dispositivos SPI sin confirmar
- Pulsador en Pin 7 con INPUT_PULLUP — lógica invertida (LOW = presionado)

## Flujo de trabajo con Hermes

- Al iniciar sesión siempre inyectar: `@PROJECT.md`
- Al trabajar en un módulo específico, inyectar también su `.h`
- Antes de generar código, confirmar en qué **Fase** estamos (ver docs/phases.md)
- No generar código de fases superiores a la actual sin confirmación explícita

## Salidas esperadas

- Código listo para copiar en Arduino IDE 2.x
- Sin dependencias externas no listadas en `docs/dependencies.md`
- CSV con formato estricto: `ID,Fecha,Hora` (sin espacios, sin BOM)

## Lo que NO hacer

- No sugerir módulos WiFi/Bluetooth — diseño 100% offline
- No almacenar imágenes de huellas, solo ID numérico del AS608
- No usar `delay()` en loops críticos de lectura — usar millis()
- No modificar archivos en `lib/` sin confirmación
