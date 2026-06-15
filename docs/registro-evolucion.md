# Registro de Evolución del Proyecto

> Sistema Biométrico de Huellas Digitales — Registro de Asistencia Académica
> Arduino Mega 2560 · C++ (Arduino SDK) · 100% offline

---

## 2026-06-09 — Sesión de pruebas Fase 1

### Test 1: OLED SSD1306 ✅

**Problema:** Pantalla encendía como "TV sin señal" (estática/ruido visual).
**Causa:** Se usó `SSD1306_EXTERNALVCC` en lugar de `SSD1306_SWITCHCAPVCC`.
**Solución:** Cambiar a `SSD1306_SWITCHCAPVCC` — el módulo GM009605 v4.3 trae regulador interno que necesita el charge pump.

**Problema:** Loopback Serial — el Mega recibía su propia transmisión Serial como eco.
**Causa:** Clon de Mega 2560 con chip USB-Serial que tiene TX→RX loopback.
**Solución:** Agregar cooldown de 300-500ms entre comandos, ignorando caracteres durante ese período. Usar `millis()` en vez de `delay()`.

**Problema:** Comandos Serial no actualizaban la pantalla.
**Causa:** Los caracteres del menú impreso por setup() se metían en el buffer RX y activaban funciones al azar.
**Solución:** `delay(100) + while(Serial.available()) Serial.read()` después del menú para limpiar el buffer.

**Resultado final:** OLED muestra "Hola" permanentemente. Test interactivo funcional con comandos `e, r, w, h, j, c`.

---

### Test 2: RTC DS3231 ✅

**Problema:** RTC mostraba hora incorrecta o no persistía.
**Causa:** Batería CR2032 agotada o sin batería. `rtc.lostPower()` == true.
**Solución:** Auto-sincronizar a hora de compilación con `rtc.adjust(DateTime(F(__DATE__), F(__TIME__)))`.

**Problema:** Al usar OLED + RTC juntos, el display se actualizaba mal.
**Causa:** Mismo bus I2C (SDA=20, SCL=21), direcciones distintas (0x3C y 0x68) — sin conflicto real, pero la inicialización del OLED debe ir primero.
**Solución:** Inicializar OLED antes que RTC en setup().

**Mejora:** temperatura del DS3231 mostrada en OLED (esquina superior derecha).
**Mejora:** actualización cada 1 segundo en vez de 5.
**Mejora:** formato fecha `DD/MM/YYYY`, hora grande `HH:MM:SS`.

**Resultado final:** OLED muestra fecha, hora (tamaño grande centrado) y temperatura. Se actualiza cada 1s.

---

### Test 3: AS608 — Sensor de Huellas ✅

**Problema:** Módulo de 8 pines (D-, D+, VA, TCH, GND, RX, TX, V+) — voltaje dudoso.
**Causa:** Versión capacitiva con USB. Algunas fuentes dicen 3.3V, otras 3.3-5V.
**Solución:** Alimentar a **3.3V** (seguro). Divisor de voltaje 1KΩ + 2.2KΩ en RX (Mega TX1 5V → AS608 RX 3.3V).

**Problema:** TX del Mega a 5V, RX del AS608 a 3.3V — podía dañar el sensor.
**Solución:** Divisor de voltaje: Mega 18 (TX1) → [1KΩ] ─┬─ AS608 RX ─ [2.2KΩ] ─ GND.

**Problema:** Enrolamiento exitoso pero búsqueda no encontraba la huella.
**Causa:** `fingerSearch()` requiere que el dedo esté en el sensor al momento exacto de la llamada.
**Solución:** Agregar loop de espera con `finger.getImage()` cada 200ms hasta que retorne `FINGERPRINT_OK`.

**Problema:** Pin TCH (Touch) se disparaba sin dedo presente.
**Causa:** Ruido eléctrico de los DC-DC converters (batería LiPo 7.4V → 3.3V y 5V). El TCH es una salida, no una entrada, y el ruido se acopla.
**Soluciones intentadas:**
- Filtro RC (resistor + capacitor a GND) — no funcionó
- Pull-down 10KΩ — no funcionó
- Alimentar desde 3.3V del Mega (sin DC-DC) — no funcionó

**Solución definitiva:** Abandonar TCH. Usar detección UART con `finger.getImage()` cada 50-200ms. Inmune al ruido eléctrico.

**Problema:** Pulsador no cambiaba de modo durante enrolamiento.
**Causa:** `esperarDedo()` bloqueaba el loop con `delay()`, el pulsador nunca se revisaba.
**Solución:** Revisar el pin del pulsador DIRECTAMENTE (`digitalRead`) dentro de `esperarDedo()` cada 50ms. Usar bandera `botonPendiente` para que el loop principal procese el cambio de modo.

**Pinout final AS608:**
| AS608 | Mega |
|---|---|
| V+ | 3.3V |
| GND | GND |
| TX | 19 (RX1) — directo |
| RX | 18 (TX1) — divisor 1KΩ+2.2KΩ |
| D+, D- | No conectar |
| VA, TCH | No conectar |

**Resultado final:** Test integrado con OLED + pulsador + buzzer. Modos ENROLAR y AUTH seleccionables con el pulsador. Detección de dedo por UART.

---

### Test 4: microSD ✅

**Problema:** Tarjeta no detectada — `SD.begin(CS=53)` retornaba falso.
**Síntomas:**
- `card.init()` fallaba
- Board se reiniciaba al intentar comunicación SPI
- En Wokwi el mismo código funcionaba perfectamente

**Causa:** El módulo microSD de 6 pines (marca "3V3") **no tiene regulador ni level shifters**. Las señales SPI del Mega (5V) iban directo a la tarjeta (3.3V), lo que impedía la comunicación y podía dañarla.

**Soluciones intentadas (fallaron):**
- Cambiar VCC entre 3.3V y 5V
- Probar múltiples pines CS (53, 4, 10, 49)
- Invertir MOSI/MISO
- Agregar pull-ups en CS
- SPI lento (250kHz)
- Regulador externo 3.3V con batería
- Aislar módulo conectando pines de a uno

**Solución definitiva:** Divisores de voltaje 1KΩ + 2.2KΩ en las 3 líneas de salida del Mega hacia el módulo:

| Línea | Mega | Divisor | Módulo |
|-------|------|---------|--------|
| MOSI | 51 | [1KΩ]─┬─[2.2KΩ]─GND | MOSI |
| SCK | 52 | [1KΩ]─┬─[2.2KΩ]─GND | CLK |
| CS | 53 | [1KΩ]─┬─[2.2KΩ]─GND | CS |

MISO (pin 50) va **directo** sin divisor (la tarjeta ya manda 3.3V).

**Resultado final:** microSD detectada, escritura/lectura correcta de TEST.CSV con formato `ID,Fecha,Hora`. Comandos vía Serial: w=escribir, r=leer, d=borrar, i=info.

---

## 2026-06-14 — Sesión Fase 2

### Código de aplicación integrada ✅

**Archivos creados:**

| Archivo | Propósito |
|---------|-----------|
| `src/main/main.ino` | Loop principal con máquina de estados |
| `src/main/constantes.h` | Pines, modos, límites |
| `src/main/rtc_helper.h/.cpp` | Inicialización y timestamps |
| `src/main/pantalla.h/.cpp` | OLED + buzzer + LED13 |
| `src/main/almacenamiento.h/.cpp` | manejo de CSVs en microSD |
| `src/main/enrolamiento.h/.cpp` | enrolamiento con verificación de duplicados |
| `src/main/asistencia.h/.cpp` | asistencia y corrección |

**Arquitectura de detección en cascada:**

```
LED13+buzzer → OLED → RTC → AS608 → microSD → SISTEMA LISTO
```

Cada periférico se verifica en orden; si falta, no avanza y reintenta cada 2s. Durante operación, se monitorean cada 3s. Si un periférico se desconecta, se alerta en OLED; si se reconecta, se recupera automáticamente.

**Métricas:** Flash 13% (33KB), SRAM 25% (2KB) — dentro de límites.

---

## Glosario de soluciones recurrentes

### Loopback Serial (afecta TODOS los tests)
- **Síntoma:** El Mega recibe y procesa sus propios Serial.print como comandos.
- **Solución estándar en cada sketch:**
  1. Después de `Serial.begin()`, esperar 100ms + limpiar buffer: `while(Serial.available()) Serial.read()`
  2. En `loop()`, solo procesar comandos si `millis() - ultimoCmd > COOLDOWN` (300ms)
  3. Después de leer un comando, limpiar buffer: `while(Serial.available()) Serial.read()`

### Ruido en pines digitales (DC-DC)
- **Síntoma:** Pines leen HIGH sin señal presente.
- **Solución general:** Filtros en software > filtros en hardware. Si es posible, cambiar a protocolo serie (UART/I2C) que es inherentemente más robusto.

### OLED no enciende al reconectar alimentación
- **Síntoma:** Al desconectar y reconectar USB, la OLED no enciende aunque el Mega funcione.
- **Causa:** El regulador 3.3V externo tarda en estabilizarse; cuando el Mega arranca e inicializa I2C, el 3.3V aún no está listo y el bus se traba.
- **Soluciones:**
  - Agregar capacitor de 100µF en salida del regulador 3.3V
  - O simplemente cargar el código de nuevo (reinicia setup() y re-inicializa la OLED)
  - O presionar Reset después de reconectar alimentación

### Señales 5V → módulos 3.3V sin level shifter
- **Síntoma:** Módulos 3.3V (microSD, AS608) no responden o funcionan erráticamente.
- **Solución:** Divisores de voltaje 1KΩ + 2.2KΩ en cada línea de salida del Mega (5V → ~3.4V). Entradas del Mega (MISO, RX) van directas sin divisor.
