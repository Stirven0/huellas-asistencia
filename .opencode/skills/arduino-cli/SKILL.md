---
name: arduino-cli
description: Use when compiling or uploading Arduino sketches for this project via arduino-cli. The MCU is ATmega2560 (Arduino Mega 2560). Also use when setting up arduino-cli, installing cores, or managing libraries.
---

# Arduino CLI — Sistema Biométrico de Asistencia

## FQBN
`arduino:avr:mega` — Arduino Mega or Mega 2560 (ATmega2560).

## Project-specific commands

```bash
# Compile an .ino sketch
arduino-cli compile --fqbn arduino:avr:mega path/to/sketch.ino

# Upload to Arduino
arduino-cli upload --fqbn arduino:avr:mega --port /dev/ttyACM0 path/to/sketch.ino

# Compile + upload in one step
arduino-cli compile --fqbn arduino:avr:mega --upload --port /dev/ttyACM0 path/to/sketch.ino

# Compile and export binaries to sketch folder
arduino-cli compile --fqbn arduino:avr:mega -e path/to/sketch.ino

# Compile with custom build directory (avoids temp)
arduino-cli compile --fqbn arduino:avr:mega --build-path /tmp/arduino-build path/to/sketch.ino
```

## Monitor (Serial debug at 115200 baud)

```bash
arduino-cli monitor --port /dev/ttyACM0 --config baudrate=115200
```

## Board and port discovery

```bash
arduino-cli board list              # List connected boards with ports
arduino-cli board listall           # List all supported boards
```

## Core management

```bash
arduino-cli core update-index       # Update core index
arduino-cli core install arduino:avr # Install AVR core
arduino-cli core list               # List installed cores
```

## Library management

Required libraries (all installed via `arduino-cli lib install`):

```bash
arduino-cli lib update-index
arduino-cli lib install "Adafruit Fingerprint Sensor Library"
arduino-cli lib install "RTClib"
arduino-cli lib install "Adafruit SSD1306"
arduino-cli lib install "Adafruit GFX Library"
```

Check installed: `arduino-cli lib list`
Search: `arduino-cli lib search <query>`

## Testing workflow (Phase 1)

Each test sketch in `tests/` is a standalone `.ino`. Compile and upload individually:

```bash
arduino-cli compile --fqbn arduino:avr:mega tests/test_as608/test_as608.ino
arduino-cli compile --fqbn arduino:avr:mega tests/test_rtc/test_rtc.ino
arduino-cli compile --fqbn arduino:avr:mega tests/test_sd/test_sd.ino
arduino-cli compile --fqbn arduino:avr:mega tests/test_oled/test_oled.ino
```

For upload, add `--upload --port /dev/ttyACM0` (adjust port as needed).

## SRAM/Flash constraints

- SRAM: 8KB — avoid `String`, `malloc`, dynamic allocation. Use `char[]` and `F()`.
- Flash: 256KB — keep binary under 200KB.
- Check binary size after compile: look for "Sketch uses X bytes" in output.

## Troubleshooting

- `arduino-cli` not found: install via `curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh`
- Permission denied on `/dev/ttyACM0`: add user to `dialout` group: `sudo usermod -a -G dialout $USER`
- Library conflicts: ensure `RTClib` is by Adafruit (not JChristensen). Version >= 2.1.4.
- No port shows in `board list`: check USB cable, try `dmesg | grep tty`, ensure board has power.
