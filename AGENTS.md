# AGENTS.md — Sistema Biométrico de Asistencia

## Project state

- **MCU:** Arduino Mega 2560 (ATmega2560, AVR) — C++ with Arduino SDK
- **Phase:** 2 (complete) — integrated firmware in `src/main/`, hardware tests in `tests/*/`
- **Build:** `arduino-cli compile --fqbn arduino:avr:mega src/main/`
- **Upload:** `arduino-cli compile --fqbn arduino:avr:mega --upload --port /dev/ttyUSB0 src/main/`
- **Permission:** `sudo chmod 666 /dev/ttyUSB0` before upload
- **No CI/CD, no automated tests** — manual `.ino` sketches flashed to hardware
- **No `String`/`malloc`/`new`** — SRAM 8KB, flash ~35KB of 253KB

## Pin assignments (current)

| Función | Pin | Detalle |
|---------|-----|---------|
| Buzzer | 12 | Active high |
| Pulsador | 3 | `INPUT_PULLUP`, `LOW` = presionado |
| AS608 UART | 18/19 | TX1/RX1, divisor 1KΩ+2.2KΩ en RX |
| microSD SPI | 50-53 | MISO directo, MOSI/SCK/CS con divisor |
| OLED I2C | 20/21 | Dirección 0x3C |
| RTC I2C | 20/21 | Dirección 0x68 |

## Interconnection quirks

| Quirk | Handling |
|-------|----------|
| Serial loopback (Mega clone) | `flush()` + 300ms cooldown after `setup()` in all serial tests |
| microSD needs voltage dividers (1KΩ+2.2KΩ) on MOSI/SCK/CS | Signal-only module has no level shifters |
| AS608 3.3V only (8‑pin capacitive) | Voltage divider on Mega TX1→AS608 RX; no TCH (UART poll only) |
| OLED SSD1306 v4.3 | Use `SSD1306_SWITCHCAPVCC` |
| DS3231 battery | `rtc.lostPower()` → auto-sync from `__DATE__`/`__TIME__` |
| SD.begin() is the only reliable presence check | Takes ~200-500ms, needed for sdPresente() |
| finger.verifyPassword() takes ~1s when disconnected | Library timeout |

## Modes (cycled by button)

| # | Modo | Descripción |
|---|------|-------------|
| 0 | ASISTENCIA | Default, fingerSearch → RTC → CSV |
| 1 | ENROLAR | Dual scan, dup check, store template |
| 2 | CORREGIR | Delete template + re-enroll |
| 3 | FORMATEAR | Double-hold 3s each → `finger.emptyDatabase()` + reset CSVs |

## Error priority display

`OLED perdida → microSD perdida → RTC perdido → AS608 perdido`

- Re-verifica cada 3s con hardware real (no cache)
- En recuperación: mensaje "Recuperado" en OLED

## Constants & CSV format

- **Active constants:** `src/main/constantes.h`
- **Canonical source:** `src/utils/constantes.h` (manual mirror)
- **Stale copy at root** `./constantes.h` — never edit it
- **CSV:** `ASIST.CSV` header `ID,Fecha,Hora`; `ESTUDIANTES.CSV` header `ID,Nombre,Apellido`
- No spaces, no BOM, ASCII

## Library requirements (install via Arduino Library Manager)

| Library | Min version |
|---------|-------------|
| Adafruit Fingerprint Sensor Library | 2.1.0 |
| RTClib (Adafruit) | 2.1.4 |
| Adafruit SSD1306 | 2.5.7 |
| Adafruit GFX | 1.11.9 |
| SdFat | 2.3.0 (for soft SPI if needed) |
| SD, Wire, SPI | built-in |

## Key reference files

| File | Purpose |
|------|---------|
| `docs/architecture.md` | State machine diagram, cascading detection flow |
| `docs/registro-evolucion.md` | Problem/solution log for all phases |
| `docs/plan-aplicacion.md` | Application flows (enrollment, attendance, correction) |
| `.opencode/skills/` | Per-module skill files (load via skill tool) |
| `.hermes/rules.md` | Legacy Hermes agent rules (preserve) |

## Code conventions

- `camelCase` for vars/functions, `UPPER_SNAKE_CASE` for constants
- Comments in Spanish
- One `.cpp` + `.h` per module, header guards
- `delay()` is used throughout (blocking) — acceptable for single-task project
