/**
 * test_sd.ino
 * Test microSD interactivo con OLED
 */

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SD_CS_PIN     53
#define OLED_ADDRESS  0x3C
#define OLED_WIDTH    128
#define OLED_HEIGHT   64
#define TEST_FILE     "TEST.CSV"

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
unsigned long ultimoCmd = 0;

void oledMsg(const char* l1, const char* l2, const char* l3) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  if (l1) { display.print(F(">> ")); display.println(l1); }
  if (l2) display.println(l2);
  if (l3) { display.print(F("-> ")); display.println(l3); }
  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    while (1) delay(1);
  }

  oledMsg("microSD", "Iniciando...", "");

  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);

  if (!SD.begin(SD_CS_PIN)) {
    oledMsg("ERROR", "microSD no", "detectada");
    Serial.println(F("[ERROR] SD no detectada"));
    while (1) delay(1);
  }

  oledMsg("microSD: OK", "Comandos:", "w r d i");
  Serial.println(F("microSD OK. w=escribir r=leer d=borrar i=info"));

  delay(100);
  while (Serial.available()) Serial.read();
  ultimoCmd = millis();
}

void loop() {
  if (Serial.available() && millis() - ultimoCmd > 300) {
    char c = Serial.read();
    while (Serial.available()) Serial.read();
    ultimoCmd = millis();

    switch (c) {
      case 'w': escribir(); break;
      case 'r': leer(); break;
      case 'd': borrar(); break;
      case 'i': info(); break;
    }
    oledMsg("microSD: OK", "Comandos:", "w r d i");
  }
}

void escribir() {
  File f = SD.open(TEST_FILE, FILE_WRITE);
  if (!f) { oledMsg("ERROR", "No abrir archivo", ""); return; }
  if (f.size() == 0) f.println(F("ID,Fecha,Hora"));
  f.println(F("1,2026-06-09,22:30:00"));
  f.close();
  oledMsg("OK", "Escrito en", "TEST.CSV");
  Serial.println(F("[OK] Escrito"));
}

void leer() {
  File f = SD.open(TEST_FILE);
  if (!f) { oledMsg("INFO", "TEST.CSV", "no existe"); return; }
  Serial.println(F("--- TEST.CSV ---"));
  while (f.available()) Serial.write(f.read());
  Serial.println(F("--- FIN ---"));
  f.close();
  oledMsg("OK", "Leido", "TEST.CSV");
}

void borrar() {
  if (SD.remove(TEST_FILE)) {
    oledMsg("OK", "Borrado", "");
    Serial.println(F("[OK] Borrado"));
  } else {
    oledMsg("ERROR", "No borrar", "");
  }
}

void info() {
  File r = SD.open("/");
  int n = 0;
  while (r.openNextFile()) { n++; r.close(); }
  r.close();
  char buf[16];
  snprintf(buf, sizeof(buf), "%d archivos", n);
  oledMsg("Info", buf, "");
  Serial.print(F("[INFO] ")); Serial.print(n); Serial.println(F(" archivos"));
}
