/**
 * huellas-asistencia
 * Fase 2 — Sistema Biometrico de Asistencia
 *
 * Deteccion en cascada:
 *   LED13+buzzer(2x) → OLED → RTC → AS608 → SD
 * Monitoreo continuo: verifica perifericos periodicamente
 *
 * Prioridad de errores en pantalla:
 *   1. OLED (LED+buzzer, no hay pantalla)
 *   2. microSD
 *   3. RTC
 *   4. AS608
 *
 * Modos: ASISTENCIA (defecto) | ENROLAR | CORREGIR
 * Pulsador Pin 7 cambia modo.
 */

#include "constantes.h"
#include "pantalla.h"
#include "rtc_helper.h"
#include "almacenamiento.h"
#include "enrolamiento.h"
#include "asistencia.h"

uint8_t modoActual = MODO_ASISTENCIA;
bool botonPendiente = false;
unsigned long ultimoBoton = 0;
unsigned long ultimaVerificacion = 0;

bool oledAlerta = false;
bool rtcAlerta = false;
bool as608Alerta = false;
bool sdAlerta = false;

#define VERIFICAR_CADA_MS 3000

void detectarPerifericos() {
  while (true) {
    if (pantallaInit()) break;
    alertaDoble();
    delay(2000);
  }

  while (true) {
    if (rtcInit()) break;
    pantallaMsg("RTC DS3231", "No detectado", "Revisar conexion");
    beepError();
    delay(2000);
  }

  while (true) {
    if (as608Init()) break;
    if (pantallaPresente())
      pantallaMsg("AS608", "No detectado", "Verificar cable");
    else
      alertaDoble();
    delay(2000);
  }

  while (true) {
    if (initSD()) break;
    pantallaMsg("microSD", "No detectada", "Insertar tarjeta");
    beepError();
    delay(2000);
  }
}

void cambiarModo() {
  modoActual = (modoActual + 1) % 4;

  const char* nombreModo = "";
  if (modoActual == MODO_ASISTENCIA) nombreModo = "ASISTENCIA";
  else if (modoActual == MODO_ENROLAMIENTO) nombreModo = "ENROLAR";
  else if (modoActual == MODO_CORRECCION) nombreModo = "CORREGIR";
  else nombreModo = "FORMATEAR";

  pantallaMsg("MODO:", nombreModo, "Coloca el dedo");
  beepExito();
  delay(1500);
  while (digitalRead(BUTTON_PIN) == LOW) delay(10);
}

void revisarBoton() {
  if (digitalRead(BUTTON_PIN) == LOW && millis() - ultimoBoton > 300) {
    ultimoBoton = millis();
    botonPendiente = true;
  }
}

// Solo actualizan flags y recuperacion, sin mostrar errores
void verificarOLED() {
  if (!pantallaPresente()) {
    oledAlerta = true;
  } else if (oledAlerta) {
    pantallaReinit();
    oledAlerta = false;
    pantallaMsg("OLED", "Recuperada", "");
    beepExito();
    delay(1000);
  }
}

void verificarSD() {
  if (!sdPresente()) {
    sdAlerta = true;
  } else if (sdAlerta) {
    initSD();
    sdAlerta = false;
    if (!oledAlerta) {
      pantallaMsg("microSD", "Recuperada", "");
      beepExito();
      delay(1000);
    }
  }
}

void verificarRTC() {
  if (!rtcPresente()) {
    rtcAlerta = true;
  } else if (rtcAlerta) {
    rtcInit();
    rtcAlerta = false;
    if (!oledAlerta) {
      pantallaMsg("RTC", "Recuperado", "");
      beepExito();
      delay(1000);
    }
  }
}

void verificarAS608() {
  if (!as608Presente()) {
    as608Alerta = true;
  } else if (as608Alerta) {
    as608Init();
    as608Alerta = false;
    if (!oledAlerta) {
      pantallaMsg("AS608", "Recuperado", "");
      beepExito();
      delay(1000);
    }
  }
}

void formatearSistema() {
  unsigned long inicio;
  bool paso1 = false, paso2 = false;

  pantallaMsg("FORMATEAR", "Manten 3s para", "confirmar");

  while (!paso1) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      inicio = millis();
      while (digitalRead(BUTTON_PIN) == LOW) {
        if (millis() - inicio >= 3000) {
          paso1 = true;
          break;
        }
        delay(50);
      }
      if (!paso1) { modoActual = MODO_ASISTENCIA; return; }
    }
    delay(50);
  }

  pantallaMsg("FORMATEAR?", "Manten 3s para", "ejecutar");
  while (digitalRead(BUTTON_PIN) == LOW) delay(10);

  while (!paso2) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      inicio = millis();
      while (digitalRead(BUTTON_PIN) == LOW) {
        if (millis() - inicio >= 3000) {
          paso2 = true;
          break;
        }
        delay(50);
      }
      if (!paso2) { modoActual = MODO_ASISTENCIA; return; }
    }
    delay(50);
  }

  pantallaMsg("FORMATEANDO...", "", "No apagar");
  delay(500);

  finger.emptyDatabase();

  if (SD.exists(ESTUDIANTES_CSV)) SD.remove(ESTUDIANTES_CSV);
  if (SD.exists(ASIST_CSV)) SD.remove(ASIST_CSV);

  File f = SD.open(ESTUDIANTES_CSV, FILE_WRITE);
  if (f) { f.println("ID,Nombre,Apellido"); f.close(); }
  f = SD.open(ASIST_CSV, FILE_WRITE);
  if (f) { f.println(CSV_HEADER); f.close(); }

  pantallaMsg("SISTEMA", "FORMATEADO", "Volviendo...");
  beepExito();
  delay(2000);

  modoActual = MODO_ASISTENCIA;
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  detectarPerifericos();

  pantallaMsg("SISTEMA LISTO", "Pulsador:", "cambia modo");
  beepExito();
  delay(1500);

  while (Serial.available()) Serial.read();
  ultimaVerificacion = millis();
}

void loop() {
  if (millis() - ultimaVerificacion > VERIFICAR_CADA_MS) {
    ultimaVerificacion = millis();
    verificarOLED();
    verificarSD();
    verificarRTC();
    verificarAS608();
  }

  // PRIORIDAD 1: OLED perdida → solo LED+buzzer, nada mas
  if (oledAlerta) {
    alertaDoble();
    delay(300);
    return;
  }

  // PRIORIDAD 2: microSD perdida
  if (sdAlerta) {
    pantallaMsg("microSD", "Perdida", "No registrar");
    beepError();
    delay(300);
    return;
  }

  // PRIORIDAD 3: RTC perdido
  if (rtcAlerta) {
    pantallaMsg("RTC", "Perdido", "Revisar conexion");
    beepError();
    delay(300);
    return;
  }

  // PRIORIDAD 4: AS608 perdido
  if (as608Alerta) {
    pantallaMsg("AS608", "Perdido", "Revisar cable");
    beepError();
    delay(300);
    return;
  }

  // TODO OK → modo normal
  revisarBoton();

  if (botonPendiente) {
    botonPendiente = false;
    cambiarModo();
    return;
  }

  if (modoActual == MODO_ASISTENCIA) {
    tomarAsistencia();
  } else if (modoActual == MODO_ENROLAMIENTO) {
    enrollarDedo();
  } else if (modoActual == MODO_CORRECCION) {
    corregirDedo();
  } else {
    formatearSistema();
  }

  for (int i = 0; i < 10; i++) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      botonPendiente = true;
      return;
    }
    delay(100);
  }
}
