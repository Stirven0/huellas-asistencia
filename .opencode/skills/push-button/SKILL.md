---
name: push-button
description: Use when writing code for the mode-selector push button on Pin 7. Covers INPUT_PULLUP usage, debouncing, edge detection, and mode switching.
---

# Pulsador — Selector de Modo

## Hardware

- **Pin:** Digital Pin 7 (`BUTTON_PIN` in constantes.h)
- **Configuration:** `INPUT_PULLUP` — no external resistor needed
- **Logic:** **Inverted** — `LOW` = pressed, `HIGH` = released

## Pin connection

| Mega pin | Button pin |
|----------|------------|
| 7        | Signal     |
| GND      | GND        |

No VCC connection needed — `INPUT_PULLUP` connects internal pull-up to 5V.

## Initialization

```cpp
#include "src/utils/constantes.h"

void setupButton() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}
```

## Debouncing

```cpp
unsigned long lastDebounceTime = 0;
bool lastButtonState = HIGH;     // Previous reading
bool buttonState = HIGH;         // Debounced state
bool modeChanged = false;

void checkButton() {
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != buttonState) {
      buttonState = reading;
      // LOW = pressed (INPUT_PULLUP logic)
      if (buttonState == LOW) {
        modeChanged = true;
      }
    }
  }

  lastButtonState = reading;
}
```

## Mode switching

```cpp
uint8_t currentMode = MODO_ASISTENCIA;  // Default: attendance

void switchMode() {
  if (modeChanged) {
    modeChanged = false;
    currentMode = (currentMode == MODO_ASISTENCIA) ? MODO_ENROLAMIENTO : MODO_ASISTENCIA;
  }
}
```

## Important notes

- **Logic is inverted:** `LOW` = pressed (because `INPUT_PULLUP` connects to 5V internally, and the button shorts to GND).
- **Always use `INPUT_PULLUP`** — do not add external pull-up/down resistors.
- **Debounce delay:** 50ms (`DEBOUNCE_DELAY` in constantes.h).
- **Do not use `delay()`** for debouncing — use `millis()` as shown above.

## Constants reference

See `src/utils/constantes.h`: `BUTTON_PIN`, `DEBOUNCE_DELAY`, `MODO_ASISTENCIA`, `MODO_ENROLAMIENTO`.
