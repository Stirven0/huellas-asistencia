#include <Arduino.h>
#include "modos.h"
#include "constantes.h"
#include "buzzer.h"
#include "pantalla.h"
#include "asistencia.h"
#include "enrolamiento.h"
#include "almacenamiento.h"

void formatearSistema(); // defined in main.ino, linked by Arduino build

const char* NOMBRES_MODO[4] = {"ASISTENCIA", "ENROLAR", "CORREGIR", "FORMATEAR"};

typedef void (*ModoHandler)();

static const ModoHandler HANDLERS_MODO[4] = {
  tomarAsistencia,
  enrollarDedo,
  corregirDedo,
  formatearSistema
};

void ejecutarModo(uint8_t modo) {
  if (modo < 4) {
    HANDLERS_MODO[modo]();
  }
}

void cambiarModo(uint8_t* modoActual) {
  *modoActual = (*modoActual + 1) % 4;
  pantallaMsg("MODO:", NOMBRES_MODO[*modoActual], "Coloca el dedo");
  beepExito();
  delay(1500);
  while (digitalRead(BUTTON_PIN) == LOW) delay(10);
}
