# AGENTS.md — Sistema Biométrico de Asistencia

## Project state

- **MCU:** Arduino Mega 2560 (ATmega2560, AVR) — C++ with Arduino SDK
- **Phase:** 2 (complete) — integrated firmware in `src/main/`, hardware tests in `tests/*/`
- **Build:** `arduino-cli compile --fqbn arduino:avr:mega src/main/`
- **Upload:** `arduino-cli compile --fqbn arduino:avr:mega --upload --port /dev/ttyUSB0 src/main/`
- **Permission:** `sudo chmod 666 /dev/ttyUSB0` before upload
- **No CI/CD, no automated tests** — manual `.ino` sketches flashed to hardware
- **Constraints:** no `String`/`malloc`/`new`; use `char[]` + `snprintf`; SRAM 8KB, flash ~35KB of 253KB

## Pin assignments

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
| AS608 TCH pin deactivated | Too sensitive to electrical noise; UART `finger.getImage()` polling instead |
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

- **Active constants (source of truth):** `src/main/constantes.h`
- **Stale file:** `src/utils/constantes.h` — missing modes 2-3 and `ALUMNOS.CSV`
- **CSV:** `ASIST.CSV` header `ID,Fecha,Hora`; `ALUMNOS.CSV` header `ID,Nombre,Apellido`
- No spaces, no BOM, ASCII

## Active codebase layout

All firmware lives under `src/main/`. One `.cpp` + `.h` per module (header guards), `delay()` used throughout.
Peripheral health uses a function-pointer struct array (`verificador.h`), modes dispatch via handler arrays (`modos.h`).

| File | Purpose |
|------|---------|
| `main.ino` | Setup, loop, button debounce, format mode, peripheral detection cascade |
| `almacenamiento.cpp/h` | microSD init, CSV read/write, duplicate check, `formatearCSVs()` |
| `asistencia.cpp/h` | Attendance and correction flow |
| `enrolamiento.cpp/h` | AS608 init, `capturarHuella()`, enrollment, template delete, `limpiarHuellas()` |
| `pantalla.cpp/h` | OLED display only |
| `rtc_helper.cpp/h` | DS3231 init, `obtenerFechaHora()` via `snprintf` |
| `notificador.cpp/h` | LED13 + buzzer unified notifications (`notificarOk`, `notificarError`, `notificarAlerta`) |
| `verificador.cpp/h` | `Periferico` struct + `verificarPeriferico()`, replaces 4× duplicate health checks |
| `modos.cpp/h` | Mode handler array, `NOMBRES_MODO[]`, `ejecutarModo()`, `cambiarModo()` |
| `constantes.h` | All pin defines, CSV filenames, mode constants, buffer sizes (`FECHA_MAX`, `HORA_MAX`, `MSG_MAX`) |

## Library requirements (install via Arduino Library Manager)

| Library | Version | Notes |
|---------|---------|-------|
| Adafruit Fingerprint Sensor Library | ≥ 2.1.0 | AS608 |
| RTClib (Adafruit) | ≥ 2.1.4 | DS3231 — not JChristensen version |
| Adafruit SSD1306 | ≥ 2.5.7 | Installs GFX as dependency |
| Adafruit GFX | ≥ 1.11.9 | OLED base |
| SD, Wire, SPI | built-in | No external SD library needed |

## Conventions

- `camelCase` for vars/functions, `UPPER_SNAKE_CASE` for constants
- Comments in Spanish
- `snprintf` for string formatting (no `String` class)
- `Serial.begin(115200)` for debug; `Serial1` for AS608

## Key reference files

| File | Purpose |
|------|---------|
| `docs/architecture.md` | State machine diagram, cascading detection flow |
| `docs/plan-aplicacion.md` | Application flows (enrollment, attendance, correction) |
| `docs/dependencies.md` | Full dependency versions and install notes |
| `.opencode/skills/` | Per-module skill files (load via skill tool) |
| `.hermes/rules.md` | Legacy agent rules (some pin info stale — buttons/pins defined in `constantes.h`) |
