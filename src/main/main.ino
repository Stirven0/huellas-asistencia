/**
 * huellas-asistencia
 * Fase 2 — Sistema Biometrico de Asistencia
 *
 * Deteccion en cascada:
 *   LED13+buzzer(2x) → OLED → RTC → AS608 → SD
 * Monitoreo continuo: verifica perifericos periodicamente
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
  modoActual = (modoActual + 1) % 3;

  const char* nombreModo = "";
  if (modoActual == MODO_ASISTENCIA) nombreModo = "ASISTENCIA";
  else if (modoActual == MODO_ENROLAMIENTO) nombreModo = "ENROLAR";
  else nombreModo = "CORREGIR";

  pantallaMsg("MODO:", nombreModo, "Coloca el dedo");
  beepExito();
  delay(500);
  while (digitalRead(BUTTON_PIN) == LOW) delay(10);
}

void revisarBoton() {
  if (digitalRead(BUTTON_PIN) == LOW && millis() - ultimoBoton > 300) {
    ultimoBoton = millis();
    botonPendiente = true;
  }
}

void verificarSD() {
  if (!sdPresente()) {
    if (!sdAlerta) {
      sdAlerta = true;
      pantallaMsg("ALERTA", "microSD perdida", "No registrar");
      beepError();
    }
  } else {
    if (sdAlerta) {
      sdAlerta = false;
      initSD();
      pantallaMsg("microSD", "Recuperada", "");
      beepExito();
      delay(1000);
      const char* nm = "ASISTENCIA";
      if (modoActual == MODO_ENROLAMIENTO) nm = "ENROLAR";
      else if (modoActual == MODO_CORRECCION) nm = "CORREGIR";
      pantallaMsg("MODO:", nm, "Coloca el dedo");
    }
  }
}

void verificarRTC() {
  if (!rtcPresente()) {
    pantallaMsg("RTC", "Perdido", "Revisar conexion");
    beepError();
    delay(500);
  }
}

void verificarAS608() {
  if (!as608Presente()) {
    pantallaMsg("AS608", "Perdido", "Revisar cable");
    beepError();
    delay(500);
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(LED_BUILTIN, OUTPUT);
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

    verificarSD();
    verificarRTC();
    verificarAS608();
  }

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
  } else {
    corregirDedo();
  }

  for (int i = 0; i < 10; i++) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      botonPendiente = true;
      return;
    }
    delay(100);
  }
}
