/**
 * test_oled.ino
 * Fase 1 — Prueba individual del display OLED SSD1306
 *
 * Hardware: SSD1306 en I2C (SDA=20, SCL=21)
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_WIDTH   128
#define OLED_HEIGHT   64
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    while (1) delay(1);
  }

  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 20);
  display.print(F("Hola"));
  display.display();

  Serial.println(F("[OK] OLED muestra 'Hola' permanentemente."));
}

void loop() {

}
