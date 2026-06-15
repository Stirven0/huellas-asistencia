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
unsigned long ultimoCheck = 0;
bool sdOk = false;
bool sdAntes = false;

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

bool checkSD() {
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  return SD.begin(SD_CS_PIN);
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    while (1) delay(1);
  }

  sdOk = checkSD();

  if (sdOk) {
    oledMsg("microSD: OK", "Comandos:", "w r d i");
    Serial.println(F("microSD OK. w r d i"));
  } else {
    oledMsg("microSD: NO", "Insertar tarjeta", "");
    Serial.println(F("microSD no detectada"));
  }

  sdAntes = sdOk;
  ultimoCheck = millis();

  delay(100);
  while (Serial.available()) Serial.read();
  ultimoCmd = millis();
}

void loop() {
  if (millis() - ultimoCheck > 1000) {
    ultimoCheck = millis();
    sdOk = checkSD();

    if (sdOk && !sdAntes) {
      oledMsg("microSD", "Insertada!", "w r d i");
      Serial.println(F("SD INSERTADA"));
    } else if (!sdOk && sdAntes) {
      oledMsg("ALERTA", "microSD", "RETIRADA!");
      Serial.println(F("SD RETIRADA!"));
    }
    sdAntes = sdOk;
  }

  if (Serial.available() && millis() - ultimoCmd > 300) {
    char c = Serial.read();
    while (Serial.available()) Serial.read();
    ultimoCmd = millis();

    if (!sdOk && c != 'i') {
      Serial.println(F("SD no presente"));
      return;
    }

    switch (c) {
      case 'w': {
        File f = SD.open(TEST_FILE, FILE_WRITE);
        if (f) {
          if (f.size() == 0) f.println("ID,Fecha,Hora");
          f.println("1,2026-06-09,22:30:00");
          f.close();
          oledMsg("OK", "Escrito", "TEST.CSV");
          Serial.println(F("[OK] Escrito"));
        } else {
          Serial.println(F("[ERR] No abrir"));
        }
        break;
      }
      case 'r': {
        File f = SD.open(TEST_FILE);
        if (f) {
          Serial.println(F("--- TEST.CSV ---"));
          while (f.available()) Serial.write(f.read());
          Serial.println(F("--- FIN ---"));
          f.close();
          oledMsg("OK", "Leido", "TEST.CSV");
        } else {
          oledMsg("INFO", "TEST.CSV", "no existe");
        }
        break;
      }
      case 'd': {
        if (SD.remove(TEST_FILE)) {
          oledMsg("OK", "Borrado", "");
          Serial.println(F("[OK] Borrado"));
        } else {
          oledMsg("ERROR", "No borrar", "");
        }
        break;
      }
      case 'i': {
        char buf[22];
        if (!sdOk) {
          oledMsg("microSD", "NO PRESENTE", "");
          break;
        }
        File r = SD.open("/");
        int n = 0;
        while (r.openNextFile()) { n++; r.close(); }
        r.close();
        snprintf(buf, sizeof(buf), "%d archivos", n);
        oledMsg("Info", buf, "");
        Serial.print(F("[INFO] ")); Serial.print(n); Serial.println(F(" archivos"));
        break;
      }
    }
    if (sdOk) oledMsg("microSD: OK", "Comandos:", "w r d i");
  }
}
