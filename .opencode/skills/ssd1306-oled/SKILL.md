---
name: ssd1306-oled
description: Use when writing code for the SSD1306 OLED display (128x64, I2C). Covers initialization, text rendering, and visual feedback patterns.
---

# SSD1306 — Pantalla OLED

## Communication

- **Interface:** I2C (shared bus with DS3231 RTC)
- **Address:** `0x3C`
- **Resolution:** 128x64 pixels
- **Libraries:** `Adafruit_SSD1306` >= 2.5.7, `Adafruit_GFX` >= 1.11.9

## Pin connections (Mega 2560)

| Mega pin | OLED pin |
|----------|----------|
| 20 (SDA) | SDA      |
| 21 (SCL) | SCL      |
| 5V       | VCC      |
| GND      | GND      |

## Initialization

```cpp
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  // No reset pin

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED no detectado"));
    while (1) delay(1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();
}
```

## Common patterns

### Show a message
```cpp
void showMessage(const char* line1, const char* line2) {
  display.clearDisplay();
  display.setCursor(0, 16);
  display.println(F(line1));
  display.println(F(line2));
  display.display();
}
```

### Show success with icon
```cpp
void showSuccess() {
  display.clearDisplay();
  display.fillCircle(64, 20, 10, SSD1306_WHITE);     // Checkmark circle
  display.setCursor(40, 40);
  display.println(F("OK"));
  display.display();
}
```

### Show error
```cpp
void showError(const char* msg) {
  display.clearDisplay();
  display.setCursor(20, 20);
  display.println(F("ERROR"));
  display.setCursor(10, 40);
  display.println(msg);
  display.display();
}
```

### Show waiting indicator
```cpp
void showWaiting() {
  display.clearDisplay();
  display.setCursor(10, 20);
  display.println(F("Coloque su dedo"));
  display.display();
}
```

## Memory note

- Each `display.print()` call uses SRAM for the display buffer (1KB).
- Always use `F()` macro for string literals to keep them in Flash.
- The Adafruit_GFX library enables drawing primitives: circles, rectangles, lines, text, bitmaps.

## Constants reference

See `src/utils/constantes.h`: `OLED_ADDRESS`, `OLED_WIDTH`, `OLED_HEIGHT`.
