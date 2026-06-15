---
name: ds3231-rtc
description: Use when writing code for the DS3231 real-time clock (I2C). Covers initialization, timestamp reading, time setting, and temperature reading.
---

# DS3231 — RTC (Reloj de Tiempo Real)

## Communication

- **Interface:** I2C (shared bus with SSD1306 OLED)
- **Address:** `0x68`
- **Library:** `RTClib` by Adafruit >= 2.1.4
- **CRITICAL:** Use Adafruit's `RTClib`, NOT JChristensen's version (different API).

## Pin connections (Mega 2560)

| Mega pin | DS3231 pin |
|----------|------------|
| 20 (SDA) | SDA        |
| 21 (SCL) | SCL        |
| 5V       | VCC        |
| GND      | GND        |

## Initialization

```cpp
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void initRTC() {
  if (!rtc.begin()) {
    Serial.println(F("RTC no detectado"));
    while (1) delay(1);
  }
  if (rtc.lostPower()) {
    // Set to compile time if battery was lost
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}
```

## Key operations

### Get timestamp
```cpp
DateTime now = rtc.now();
char timestamp[20];  // Buffer for "YYYY-MM-DD,HH:MM:SS"
snprintf_P(timestamp, sizeof(timestamp),
  PSTR("%04d-%02d-%02d,%02d:%02d:%02d"),
  now.year(), now.month(), now.day(),
  now.hour(), now.minute(), now.second());
```

### Get date and time separately
```cpp
DateTime now = rtc.now();
char dateStr[11];   // "YYYY-MM-DD"
char timeStr[9];    // "HH:MM:SS"
snprintf_P(dateStr, sizeof(dateStr), PSTR("%04d-%02d-%02d"), now.year(), now.month(), now.day());
snprintf_P(timeStr, sizeof(timeStr), PSTR("%02d:%02d:%02d"), now.hour(), now.minute(), now.second());
```

### Force set time
```cpp
rtc.adjust(DateTime(2026, 6, 5, 8, 30, 0));  // YYYY, M, D, HH, MM, SS
```

### Read temperature (DS3231 internal sensor)
```cpp
float temp = rtc.getTemperature();  // Returns Celsius
```

## Important notes

- **Battery-backed:** Maintains time through power loss (CR2032 coin cell).
- **CSV format requires:** `ID,Fecha,Hora` — use `dateStr` and `timeStr` as shown above.
- **No `String` class** — use `char[]` and `snprintf_P` with `PSTR()`.
- I2C addresses differ: DS3231=0x68, SSD1306=0x3C — no conflict on shared bus.

## Constants reference

See `src/utils/constantes.h`: `RTC_ADDRESS`.
