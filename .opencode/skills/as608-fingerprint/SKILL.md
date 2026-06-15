---
name: as608-fingerprint
description: Use when writing code for the AS608 optical fingerprint sensor. Communicates via UART (Serial1 on Mega). Covers enrollment, search, deletion, and template management.
---

# AS608 — Sensor de Huellas Dactilares

## Communication

- **Interface:** UART via `Serial1` (pins TX1=18, RX1=19 on Mega 2560)
- **DO NOT use `Serial`** — that is reserved for USB debug output at 115200 baud.
- **Library:** `Adafruit Fingerprint Sensor Library` >= 2.1.0

## Pin connections

| Mega pin | AS608 pin |
|----------|-----------|
| 18 (TX1) | RX        |
| 19 (RX1) | TX        |
| 5V       | VCC       |
| GND      | GND       |

## Initialization

```cpp
#include <Adafruit_Fingerprint.h>

// SoftwareSerial is NOT needed — use hardware Serial1 on Mega
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);

void setup() {
  Serial.begin(115200);  // USB debug
  finger.begin(57600);   // AS608 default baud

  if (finger.verifyPassword()) {
    Serial.println(F("Sensor listo"));
  } else {
    Serial.println(F("Sensor no detectado"));
    while (1) delay(1);
  }
}
```

## Key operations

### Enroll a fingerprint (dual scan)
```cpp
uint8_t enrollFinger(uint8_t id) {
  int p = -1;
  // First scan
  while (p != FINGERPRINT_OK) p = finger.getImage();
  p = finger.image2Tz(1);
  // Second scan
  while (p != FINGERPRINT_OK) p = finger.getImage();
  p = finger.image2Tz(2);
  // Create model
  p = finger.createModel();
  if (p != FINGERPRINT_OK) return p;
  // Store template
  p = finger.storeModel(id);
  return p;
}
```

### Search for a fingerprint
```cpp
uint8_t searchFinger() {
  finger.getImage();
  finger.image2Tz();
  uint8_t id = finger.fingerSearch();
  return id;  // 0 = no match, 1-127 = matched ID
}
```

### Delete a template
```cpp
finger.deleteModel(id);    // Delete specific ID
finger.emptyDatabase();    // Clear ALL templates
```

### Get template count
```cpp
finger.getTemplateCount();
uint8_t count = finger.templateCount;
```

## Constants

| Constant | Value | Meaning |
|----------|-------|---------|
| `FINGERPRINT_OK` | 0x00 | Success |
| `FINGERPRINT_NOFINGER` | 0x02 | No finger on sensor |
| `FINGERPRINT_NOTFOUND` | 0x09 | No match in database |
| `MAX_FINGERPRINTS` | 127 | Library limit (defined in `constantes.h`) |

## Constraints

- **No fingerprint images stored** — only numeric ID (1-127).
- **Timeout** for finger detection: 5000ms (`FINGERPRINT_TIMEOUT` in constantes.h).
- **Do not use `delay()`** in sensor loops — use `millis()` for non-blocking waits.
- Sensor stores templates in its internal flash memory (not on Arduino).

## Wiring reference

See `src/utils/constantes.h` for pin constants: `FP_RX_PIN`, `FP_TX_PIN`.
