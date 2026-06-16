#include <Arduino.h>
#include "modos.h"
#include "constantes.h"
#include "buzzer.h"
#include "pantalla.h"
#include "asistencia.h"
#include "enrolamiento.h"
#include "almacenamiento.h"

void formatearSistema(); // defined in main.ino, linked by Arduino build

const char* NOMBRES_MODO[MODOS_COUNT] = {"ASISTENCIA", "ENROLAR", "CORREGIR", "FORMATEAR"};

typedef void (*ModoHandler)();

static const ModoHandler HANDLERS_MODO[MODOS_COUNT] = {
  tomarAsistencia,
  enrollarDedo,
  corregirDedo,
  formatearSistema
};

void ejecutarModo(uint8_t modo) {
  if (modo < MODOS_COUNT) {
    HANDLERS_MODO[modo]();
  }
}

void cambiarModo(uint8_t* modoActual) {
  *modoActual = (*modoActual + 1) % MODOS_COUNT;
  pantallaMsg("MODO:", NOMBRES_MODO[*modoActual], "Coloca el dedo");
  beepExito();
  delay(1500);
  while (digitalRead(BUTTON_PIN) == LOW) delay(10);
}
