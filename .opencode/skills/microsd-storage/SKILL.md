---
name: microsd-storage
description: Use when writing code for the microSD card module (SPI). Covers initialization, CSV writing, reading, and duplicate detection for attendance records.
---

# MicroSD — Almacenamiento en Tarjeta

## Communication

- **Interface:** SPI (dedicated bus — no other SPI devices)
- **Chip Select:** Pin 53 (defined as `SD_CS_PIN` in constantes.h)
- **Library:** `SD` (built-in Arduino library) >= 1.3.0

## Pin connections (Mega 2560)

| Mega pin | microSD pin |
|----------|-------------|
| 51 (MOSI) | MOSI       |
| 50 (MISO) | MISO       |
| 52 (SCK)  | SCK        |
| 53 (CS)   | CS         |
| 5V        | VCC        |
| GND       | GND        |

## Initialization

```cpp
#include <SPI.h>
#include <SD.h>

#define SD_CS_PIN 53

void initSD() {
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("microSD no detectada"));
    while (1) delay(1);
  }
}
```

## CSV format (strict)

File: `ASIST.CSV` on root of microSD (FAT16/FAT32).

```
ID,Fecha,Hora
1,2026-06-05,08:30:00
2,2026-06-05,08:31:12
```

- **Separator:** comma (`,`)
- **Date:** `YYYY-MM-DD`
- **Time:** `HH:MM:SS`
- **Encoding:** ASCII, no BOM, no spaces
- **Header:** only on first line

## Key operations

### Write a record (with duplicate check)
```cpp
#include "src/utils/constantes.h"

bool writeAttendanceRecord(uint8_t id, const char* date, const char* time) {
  File file = SD.open(CSV_FILENAME, FILE_READ);
  if (!file) return false;

  // Check for duplicate (same ID + same date)
  char buffer[40];
  while (file.available()) {
    int len = file.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
    buffer[len] = '\0';
    char idStr[4];
    snprintf_P(idStr, sizeof(idStr), PSTR("%d"), id);
    if (strstr(buffer, idStr) && strstr(buffer, date)) {
      file.close();
      return false;  // Duplicate
    }
  }
  file.close();

  // Append new record
  file = SD.open(CSV_FILENAME, FILE_WRITE);
  if (!file) return false;

  char csvLine[40];
  snprintf_P(csvLine, sizeof(csvLine), PSTR("%d,%s,%s"), id, date, time);
  file.println(csvLine);
  file.close();
  return true;
}
```

### Initialize CSV with header
```cpp
void initCSV() {
  if (!SD.exists(CSV_FILENAME)) {
    File file = SD.open(CSV_FILENAME, FILE_WRITE);
    if (file) {
      file.println(CSV_HEADER);
      file.close();
    }
  }
}
```

### Read all records (for post-processing)
```cpp
void readCSV() {
  File file = SD.open(CSV_FILENAME, FILE_READ);
  if (file) {
    while (file.available()) {
      Serial.write(file.read());
    }
    file.close();
  }
}
```

## Important notes

- **FAT16/FAT32 only** — format microSD as FAT32 before use.
- **No long filenames** — 8.3 format: `ASIST.CSV`.
- **SPI is microSD-only** — do not add other SPI devices without confirmation.
- **CSV write is append-only** — no in-place editing (read entire file, modify, rewrite if needed).
- **SD card must be inserted** before power-on or call `SD.begin()` after insertion.

## Constants reference

See `src/utils/constantes.h`: `SD_CS_PIN`, `CSV_FILENAME`, `CSV_HEADER`.
