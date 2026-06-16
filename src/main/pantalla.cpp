#include "pantalla.h"
#include "constantes.h"

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
static bool pantallaOk = false;

// Inicializa la pantalla OLED SSD1306 sobre I2C
bool pantallaInit() {
  Wire.begin();
  pantallaOk = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  return pantallaOk;
}

// Verifica si la OLED responde en el bus I2C
bool pantallaPresente() {
  Wire.beginTransmission(OLED_ADDRESS);
  return (Wire.endTransmission() == 0);
}

// Re-inicializa la OLED (usado tras recuperacion de periferico)
bool pantallaReinit() {
  pantallaOk = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  return pantallaOk;
}

// Muestra 3 lineas de texto en la OLED.
// Formato: ">> l1", l2 centrado, "-> l3"
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


