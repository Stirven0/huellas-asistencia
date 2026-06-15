/**
 * test_rtc.ino
 * Fase 1 — Prueba individual del RTC DS3231 + OLED SSD1306
 *
 * Muestra fecha/hora/temp en OLED y Serial, actualiza cada 1 segundo.
 *
 * Hardware: DS3231 + SSD1306 en I2C (SDA=20, SCL=21)
 */

#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

RTC_DS3231 rtc;

#define OLED_WIDTH   128
#define OLED_HEIGHT   64
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

unsigned long ultimoComando = 0;
const unsigned long COOLDOWN = 500;

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("[ERROR] OLED no encontrado."));
    while (1) delay(1);
  }

  if (!rtc.begin()) {
    Serial.println(F("[ERROR] RTC no encontrado."));
    while (1) delay(1);
  }

  Serial.println(F("[OK] OLED + RTC detectados."));

  if (rtc.lostPower()) {
    Serial.println(F("[AVISO] RTC perdio energia. Ajustando..."));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  Serial.println(F("r=leer  s=sincronizar  t=temperatura"));

  delay(100);
  while (Serial.available()) Serial.read();
  ultimoComando = millis();
}

void loop() {
  if (Serial.available() && millis() - ultimoComando > COOLDOWN) {
    char cmd = Serial.read();
    while (Serial.available()) Serial.read();
    ultimoComando = millis();

    switch (cmd) {
      case 'r':
        leerHora();
        break;
      case 's':
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        Serial.println(F("[OK] Hora sincronizada."));
        leerHora();
        break;
      case 't':
        Serial.print(F("Temperatura: "));
        Serial.print(rtc.getTemperature());
        Serial.println(F(" C"));
        break;
    }
  }

  static unsigned long ultimaLectura = 0;
  if (millis() - ultimaLectura >= 1000) {
    leerHora();
    ultimaLectura = millis();
  }
}

void leerHora() {
  DateTime ahora = rtc.now();
  float temp = rtc.getTemperature();

  char fecha[11];
  snprintf(fecha, sizeof(fecha),
    "%02d/%02d/%04d",
    ahora.day(), ahora.month(), ahora.year()
  );

  char tiempo[9];
  snprintf(tiempo, sizeof(tiempo),
    "%02d:%02d:%02d",
    ahora.hour(), ahora.minute(), ahora.second()
  );

  Serial.print(F("[RTC] "));
  Serial.print(fecha);
  Serial.print(' ');
  Serial.print(tiempo);
  Serial.print(F("  Temp: "));
  Serial.print(temp);
  Serial.println(F(" C"));

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(fecha);

  display.setCursor(90, 0);
  display.print(temp, 1);
  display.print('C');

  display.setTextSize(2);
  display.setCursor(16, 20);
  display.print(tiempo);

  display.display();
}
