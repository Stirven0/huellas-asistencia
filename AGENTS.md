# AGENTS.md — Sistema Biométrico de Asistencia

## Project state

- **MCU:** Arduino Mega 2560 (ATmega2560, AVR) — C++ with Arduino SDK
- **Phase:** 1 — test sketches exist in `tests/` subdirectories, but no `main.ino`, no module `.cpp/.h` files. `src/utils/constantes.h` is the only source file. No git commits yet.
- **No build config** (no platformio.ini, Makefile). Compile via Arduino IDE 2.x: board "Arduino Mega or Mega 2560", upload, interact at 115200 baud Serial Monitor.
- **No CI/CD or automated tests** — tests are manual `.ino` sketches flashed to hardware.

## Critical constraints

- **SRAM: 8KB** — no `String`, no `malloc`/`new`. Use `char[]` and `F()`. Keep binary under 200KB.
- **Serial1** (pins 18/19) = AS608 fingerprint sensor. `Serial` (USB) = debug-only.
- **Button Pin 7** is `INPUT_PULLUP` — `LOW` = pressed.
- **I2C shared** between SSD1306 (0x3C) and DS3231 (0x68). SPI is microSD-only.
- **CSV strict:** `ASIST.CSV` with header `ID,Fecha,Hora` (no spaces, no BOM).
- **100% offline** — no WiFi/Bluetooth.

## Code conventions

- **Canonical constants:** `src/utils/constantes.h`. A stale duplicate exists at root `./constantes.h` — never edit it.
- `camelCase` for vars/functions, `UPPER_SNAKE_CASE` for constants. Comments in Spanish.
- One `.cpp` + `.h` per module. Header guards required. No `delay()` in sensor loops — use `millis()`.
- Do not generate Phase 2+ code (enrollment, attendance, etc.) without explicit confirmation.
- External libs are NOT vendored in `lib/` (it's empty). Install via Arduino Library Manager per `docs/dependencies.md`.

## Library gotchas

| Risk | Fix |
|------|-----|
| Wrong RTClib (JChristensen vs Adafruit) | Use **Adafruit RTClib** ≥ 2.1.4 |
| Adafruit_Fingerprint < 2.0 | Use **≥ 2.1.0** (API changed) |
| Adafruit_BusIO vs Wire conflict | Update BusIO to **≥ 1.14** |

## Key reference files

| File | Purpose |
|------|---------|
| `src/utils/constantes.h` | Pin definitions, system config, CSV format |
| `docs/dependencies.md` | Exact library versions |
| `docs/phases.md` | Implementation phase status |
| `docs/plan-aplicacion.md` | Application logic flows (enrollment, attendance, correction) |
| `.opencode/skills/` | Per-module skill files (load via skill tool) |
| `.hermes/rules.md` | Legacy Hermes agent rules (preserve) |
