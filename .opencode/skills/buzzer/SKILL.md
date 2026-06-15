---
name: buzzer
description: Use when writing code for the active buzzer on Pin 8. Covers success/error tones, beep patterns, and non-blocking timing.
---

# Buzzer — Retroalimentación Sonora

## Hardware

- **Type:** Active buzzer (generates tone when powered — no PWM needed)
- **Pin:** Digital Pin 8 (`BUZZER_PIN` in constantes.h)
- **Logic:** `HIGH` = sound ON, `LOW` = sound OFF

## Pin connection

| Mega pin | Buzzer pin |
|----------|------------|
| 8        | Signal     |
| 5V       | VCC        |
| GND      | GND        |

## Common patterns

### Success beep (short)
```cpp
#include "src/utils/constantes.h"

void beepSuccess() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(BUZZER_SHORT);   // 100ms
  digitalWrite(BUZZER_PIN, LOW);
}
```

### Error beep (long)
```cpp
void beepError() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(BUZZER_LONG);    // 500ms
  digitalWrite(BUZZER_PIN, LOW);
}
```

### Non-blocking beep (using millis)
```cpp
bool buzzerActive = false;
unsigned long buzzerEndTime = 0;

void buzzerNonBlocking(bool on, unsigned long duration) {
  if (on && !buzzerActive) {
    digitalWrite(BUZZER_PIN, HIGH);
    buzzerActive = true;
    buzzerEndTime = millis() + duration;
  }
  if (buzzerActive && millis() >= buzzerEndTime) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerActive = false;
  }
}
```

## Important notes

- **Active buzzer:** does not require PWM or `tone()`. Simple `digitalWrite` is sufficient.
- **No `delay()` in sensor loops** — use the `millis()` pattern above if the buzzer runs concurrently with sensor reads.
- `BUZZER_SHORT` (100ms) = success indication.
- `BUZZER_LONG` (500ms) = error indication.

## Constants reference

See `src/utils/constantes.h`: `BUZZER_PIN`, `BUZZER_SHORT`, `BUZZER_LONG`.
