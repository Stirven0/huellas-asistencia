#include "asistencia.h"
#include "constantes.h"
#include "notificador.h"
#include "pantalla.h"
#include "enrolamiento.h"
#include "almacenamiento.h"
#include "rtc_helper.h"

void tomarAsistencia() {
  pantallaMsg("ASISTENCIA", "Coloca el dedo", "");

  if (!esperarDedo()) return;

  uint8_t id;
  if (!capturarHuella(&id)) {
    pantallaMsg("ASISTENCIA", "No reconocido", "");
    notificarError();
    return;
  }
  char nombre[NOMBRE_MAX];

  if (!buscarNombre(id, nombre)) {
    snprintf(nombre, sizeof(nombre), "ID %d", id);
  }

  char fecha[FECHA_MAX], hora[HORA_MAX];
  obtenerFechaHora(fecha, hora);

  if (esDuplicado(id, fecha)) {
    pantallaMsg(nombre, "Ya registraste", "hoy");
    notificarError();
    return;
  }

  if (registrarAsistencia(id, fecha, hora)) {
    pantallaMsg(nombre, "Asistencia", "registrada");
    notificarOk();
  } else {
    pantallaMsg("ERROR", "No guardar", "ASIST.CSV");
    notificarError();
  }
}

void corregirDedo() {
  pantallaMsg("CORREGIR", "Coloca dedo", "a reemplazar");

  if (!esperarDedo()) return;

  uint8_t id;
  if (!capturarHuella(&id)) {
    pantallaMsg("CORREGIR", "No reconocido", "");
    notificarError();
    return;
  }
  char nombre[NOMBRE_MAX];
  char msg[MSG_MAX];
  char linea2[MSG_MAX];

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
        notificarError();
        return;
      }

      pantallaMsg("CORREGIR", "Template borrado", "Nuevo dedo...");
      notificarOk();
      delay(1000);

      enrollarDedoEnId(id, nombre);

      pantallaMsg("CORREGIR", "Completado", "");
      delay(1000);
      return;
    }
    delay(20);
  }

  pantallaMsg("CORREGIR", "Sin confirmacion", "");
}
