#include "asistencia.h"
#include "constantes.h"
#include "pantalla.h"
#include "enrolamiento.h"
#include "almacenamiento.h"
#include "rtc_helper.h"

void tomarAsistencia() {
  pantallaMsg("ASISTENCIA", "Coloca el dedo", "");

  if (!esperarDedo()) return;

  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) { beepError(); return; }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) { beepError(); return; }

  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) {
    pantallaMsg("ASISTENCIA", "No reconocido", "");
    beepError();
    return;
  }

  uint8_t id = finger.fingerID;
  char nombre[NOMBRE_MAX];

  if (!buscarNombre(id, nombre)) {
    snprintf(nombre, sizeof(nombre), "ID %d", id);
  }

  char fecha[12], hora[10];
  obtenerFechaHora(fecha, hora);

  if (esDuplicado(id, fecha)) {
    pantallaMsg(nombre, "Ya registrado", "hoy");
    beepError();
    return;
  }

  if (registrarAsistencia(id, fecha, hora)) {
    pantallaMsg(nombre, "Asistencia OK", hora);
    beepExito();
  } else {
    pantallaMsg("ERROR", "No guardar", "ASIST.CSV");
    beepError();
  }
}

void corregirDedo() {
  pantallaMsg("CORREGIR", "Coloca dedo", "a reemplazar");

  if (!esperarDedo()) return;

  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) { beepError(); return; }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) { beepError(); return; }

  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) {
    pantallaMsg("CORREGIR", "No reconocido", "");
    beepError();
    return;
  }

  uint8_t id = finger.fingerID;
  char nombre[NOMBRE_MAX];
  char msg[22];
  char linea2[22];

  if (buscarNombre(id, nombre)) {
    snprintf(msg, sizeof(msg), "ID %d: %s", id, nombre);
  } else {
    snprintf(msg, sizeof(msg), "ID %d", id);
  }
  snprintf(linea2, sizeof(linea2), "Boton=reemplazar");
  pantallaMsg("CORREGIR", msg, linea2);

  unsigned long espera = millis();
  while (millis() - espera < 4000) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      delay(DEBOUNCE_DELAY);

      if (!borrarTemplate(id)) {
        pantallaMsg("CORREGIR", "Error borrar", "");
        beepError();
        return;
      }

      pantallaMsg("CORREGIR", "Template borrado", "Nuevo dedo...");
      beepExito();
      delay(1000);

      enrollarDedo();

      pantallaMsg("CORREGIR", "Completado", "");
      delay(1000);
      return;
    }
    delay(20);
  }

  pantallaMsg("CORREGIR", "Sin confirmacion", "");
}
