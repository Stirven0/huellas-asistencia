/**
 * test_integrado.ino
 * Fase 1 — Test integrado: AS608 + OLED + Pulsador + TCH + Buzzer
 *
 * Pulsador cambia entre modo ENROLAR y modo AUTH.
 * OLED muestra modo y estado.
 * Detección de dedo por UART (finger.getImage() cada 200ms).
 *
 * Hardware:
 *   AS608: Serial1 (18/19), V+=3.3V
 *   OLED: I2C (20/21)
 *   Pulsador: Pin 7 (INPUT_PULLUP)
 *   Buzzer: Pin 8
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Fingerprint.h>

// ============================================================
// PINES
// ============================================================
#define BUTTON_PIN      7
#define BUZZER_PIN      8
#define OLED_ADDRESS    0x3C
#define OLED_WIDTH      128
#define OLED_HEIGHT     64
#define DEBOUNCE_DELAY  50
#define BUZZER_SHORT    100
#define BUZZER_LONG     500

// ============================================================
// MODOS
// ============================================================
#define MODO_ENROLAR    0
#define MODO_AUTH       1

// ============================================================
// OBJETOS GLOBALES
// ============================================================
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);

uint8_t modoActual = MODO_ENROLAR;

// Debounce pulsador
unsigned long lastDebounceTime = 0;
bool lastButtonState = HIGH;
bool buttonState = HIGH;

// Buffer para log en OLED (última acción)
char logLine[20];

// Bandera: pulsador presionado mientras esperaba dedo
bool botonPendiente = false;

// ============================================================
// OLED
// ============================================================
void oledMostrar(const char* linea1, const char* linea2, const char* linea3) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  if (linea1) { display.print(F(">> ")); display.println(linea1); }
  if (linea2) display.println(linea2);
  if (linea3) { display.print(F("-> ")); display.println(linea3); }
  display.display();
}

void oledLog(const char* modo, const char* accion) {
  strncpy(logLine, accion, sizeof(logLine) - 1);
  logLine[sizeof(logLine) - 1] = '\0';
  oledMostrar(modo, "", logLine);
}

// ============================================================
// BUZZER
// ============================================================
void beep(unsigned long duracion) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duracion);
  digitalWrite(BUZZER_PIN, LOW);
}

// ============================================================
// DETECCIÓN POR UART
// ============================================================
bool esperarDedo() {
  while (finger.getImage() != FINGERPRINT_OK) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      botonPendiente = true;
      return false;
    }
  }
  return true;
}

bool esperarSinDedo() {
  while (finger.getImage() != FINGERPRINT_NOFINGER) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      botonPendiente = true;
      return false;
    }
  }
  delay(DEBOUNCE_DELAY);
  return true;
}

// ============================================================
// PULSADOR
// ============================================================
bool pulsadorPresionado() {
  bool lectura = digitalRead(BUTTON_PIN);

  if (lectura != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (lectura != buttonState) {
      buttonState = lectura;
      lastButtonState = lectura;
      return (buttonState == LOW);
    }
  }

  lastButtonState = lectura;
  return false;
}

// ============================================================
// ENROLAR
// ============================================================
void enrollarDedo() {
  oledMostrar("MODO: ENROLAR", "Coloca el dedo", "");

  if (!esperarDedo()) return;

  oledLog("ENROLAR", "Capturando 1...");
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    oledLog("ENROLAR", "Error captura 1");
    beep(BUZZER_LONG);
    return;
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    oledLog("ENROLAR", "Error procesar 1");
    beep(BUZZER_LONG);
    return;
  }

  oledLog("ENROLAR", "Retira el dedo");
  if (!esperarSinDedo()) return;

  oledLog("ENROLAR", "Mismo dedo otra vez");
  if (!esperarDedo()) return;

  oledLog("ENROLAR", "Capturando 2...");
  p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    oledLog("ENROLAR", "Error captura 2");
    beep(BUZZER_LONG);
    return;
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    oledLog("ENROLAR", "Error procesar 2");
    beep(BUZZER_LONG);
    return;
  }

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    oledLog("ENROLAR", "No coinciden");
    beep(BUZZER_LONG);
    return;
  }

  // Buscar próximo ID disponible
  finger.getTemplateCount();
  uint8_t id = finger.templateCount + 1;

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    char msg[14];
    snprintf(msg, sizeof(msg), "Enrolado ID %d", id);
    oledLog("ENROLAR", msg);
    beep(BUZZER_SHORT);
  } else {
    oledLog("ENROLAR", "Error al guardar");
    beep(BUZZER_LONG);
  }
}

// ============================================================
// AUTENTICAR
// ============================================================
void autenticarDedo() {
  oledMostrar("MODO: AUTH", "Coloca el dedo", "");

  if (!esperarDedo()) return;

  oledLog("AUTH", "Buscando...");
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    oledLog("AUTH", "Error captura");
    beep(BUZZER_LONG);
    return;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    oledLog("AUTH", "Error procesar");
    beep(BUZZER_LONG);
    return;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    char msg[14];
    snprintf(msg, sizeof(msg), "ID %d OK", finger.fingerID);
    oledLog("AUTH", msg);
    beep(BUZZER_SHORT);
  } else {
    oledLog("AUTH", "No reconocido");
    beep(BUZZER_LONG);
  }
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    while (1) delay(1);
  }

  finger.begin(57600);
  delay(100);

  if (!finger.verifyPassword()) {
    oledMostrar("ERROR", "AS608 no", "detectado");
    while (1) delay(1);
  }

  oledMostrar("SISTEMA LISTO", "Pulsador:", "cambia modo");
  beep(BUZZER_SHORT);
  delay(1500);
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  // Revisar pulsador o bandera para cambiar modo
  if (pulsadorPresionado() || botonPendiente) {
    botonPendiente = false;
    modoActual = (modoActual == MODO_ENROLAR) ? MODO_AUTH : MODO_ENROLAR;
    const char* nombreModo = (modoActual == MODO_ENROLAR) ? "ENROLAR" : "AUTH";
    oledMostrar("MODO CAMBIADO", nombreModo, "Esperando dedo...");
    delay(300);
    // Esperar a que suelten el botón
    while (digitalRead(BUTTON_PIN) == LOW) delay(10);
    return;
  }

  // Ejecutar según modo
  if (modoActual == MODO_ENROLAR) {
    enrollarDedo();
  } else {
    autenticarDedo();
  }

  // Pausa revisando pulsador
  for (int i = 0; i < 10; i++) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      botonPendiente = true;
      return;
    }
    delay(100);
  }
}
