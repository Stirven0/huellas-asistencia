#include "pantalla.h"
#include "constantes.h"

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
static bool pantallaOk = false;

bool pantallaInit() {
  Wire.begin();
  pantallaOk = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  return pantallaOk;
}

bool pantallaPresente() {
  Wire.beginTransmission(OLED_ADDRESS);
  return (Wire.endTransmission() == 0);
}

bool pantallaReinit() {
  pantallaOk = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  return pantallaOk;
}

void pantallaMsg(const char* l1, const char* l2, const char* l3) {
  if (!pantallaOk) return;
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  if (l1) { display.print(F(">> ")); display.println(l1); }
  if (l2) display.println(l2);
  if (l3) { display.print(F("-> ")); display.println(l3); }
  display.display();
}

void beepExito() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(BUZZER_SHORT);
  digitalWrite(BUZZER_PIN, LOW);
}

void beepError() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(BUZZER_LONG);
  digitalWrite(BUZZER_PIN, LOW);
}

void alertaDoble() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}
