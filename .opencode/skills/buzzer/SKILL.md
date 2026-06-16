---
name: buzzer
description: Use when writing code for the active buzzer on Pin 12. Covers success/error tones, beep patterns, and non-blocking timing.
---

# Buzzer — Retroalimentación Sonora (integrada en `notificador`)

## Hardware

- **Type:** Active buzzer (generates tone when powered — no PWM needed)
- **Pin:** Digital Pin 12 (`BUZZER_PIN` in `constantes.h`)
- **LED13** (`LED_BUILTIN`) se activa simultáneamente con el buzzer
- **Logic:** `HIGH` = sound ON, `LOW` = sound OFF

## Pin connection

| Mega pin | Buzzer pin |
|----------|------------|
| 12       | Signal     |
| 5V       | VCC        |
| GND      | GND        |

## Notifications (use `notificador.h`)

Todas las notificaciones controlan LED13 + buzzer como una unidad.

### Éxito
```cpp
#include "notificador.h"

notificarOk();   // 100ms beep + blink
```

### Error
```cpp
notificarError();  // 500ms beep + blink
```

### Alerta (OLED no disponible — doble pulso)
```cpp
notificarAlerta();  // 2× 200ms pulse + blink
```

## Constants reference

See `src/main/constantes.h`: `BUZZER_PIN`, `BUZZER_SHORT` (100ms), `BUZZER_LONG` (500ms), `BUZZER_DOUBLE` (200ms).
