#include "enrolamiento.h"
#include "constantes.h"
#include "notificador.h"
#include "pantalla.h"
#include "almacenamiento.h"

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);
static bool as608Ok = false;

bool as608Init() {
  finger.begin(57600);
  delay(100);
  as608Ok = finger.verifyPassword();
  return as608Ok;
}

bool as608Presente() {
  if (!as608Ok) {
    finger.begin(57600);
    delay(100);
  }
  while (Serial1.available()) Serial1.read();
  as608Ok = finger.verifyPassword();
  return as608Ok;
}

bool esperarDedo() {
  unsigned long inicio = millis();
  while (finger.getImage() != FINGERPRINT_OK) {
    if (millis() - inicio > FINGERPRINT_TIMEOUT) return false;
    if (digitalRead(BUTTON_PIN) == LOW) return false;
    if (!as608Presente()) return false;
    delay(50);
  }
  return true;
}

bool esperarSinDedo() {
  while (finger.getImage() != FINGERPRINT_NOFINGER) {
    if (digitalRead(BUTTON_PIN) == LOW) return false;
    delay(50);
  }
  delay(DEBOUNCE_DELAY);
  return true;
}

bool capturarHuella(uint8_t* idOut) {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return false;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return false;
  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) return false;
  *idOut = finger.fingerID;
  return true;
}

bool huellaYaExiste(uint8_t* idExistente) {
  return capturarHuella(idExistente);
}

void enrollarDedoEnId(uint8_t id, const char* nombre) {
  uint8_t idExistente;
  if (huellaYaExiste(&idExistente)) {
    char nombreExistente[NOMBRE_MAX];
    char linea3[MSG_MAX];
    if (buscarNombre(idExistente, nombreExistente))
      snprintf(linea3, sizeof(linea3), "como %s", nombreExistente);
    else
      snprintf(linea3, sizeof(linea3), "como ID %d", idExistente);
    pantallaMsg(nombre, "Huella ya existe", linea3);
    notificarError();
    return;
  }

  // mostrar nombre del estudiante solo si ya hay dedo puesto
  pantallaMsg(nombre, "Dedo 1 de 2", "");
  if (!esperarDedo()) return;

  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) { notificarError(); return; }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) { notificarError(); return; }

  pantallaMsg(nombre, "Retira dedo", "");
  if (!esperarSinDedo()) return;

  pantallaMsg(nombre, "Dedo 2 de 2", "");
  if (!esperarDedo()) return;

  p = finger.getImage();
  if (p != FINGERPRINT_OK) { notificarError(); return; }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) { notificarError(); return; }

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    pantallaMsg(nombre, "No coinciden", "");
    notificarError();
    return;
  }

  if (huellaYaExiste(&idExistente)) {
    pantallaMsg("ENROLAR", "Ya existe en BD", "");
    notificarError();
    return;
  }

  if (finger.loadModel(id) == FINGERPRINT_OK) {
    pantallaMsg("ENROLAR", "ID ocupado", "Reintentar");
    notificarError();
    return;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    char msg[MSG_MAX];
    snprintf(msg, sizeof(msg), "ID %d OK", id);
    pantallaMsg(nombre, "Enrolado OK", msg);
    notificarOk();
  } else {
    pantallaMsg(nombre, "Error guardar", "");
    notificarError();
  }
}

void enrollarDedo() {
  if (!as608Ok) {
    pantallaMsg("ERROR", "AS608 no", "disponible");
    notificarError();
    return;
  }

  uint8_t id;
  char nombre[NOMBRE_MAX];
  if (!buscarSinHuella(&id, nombre)) {
    pantallaMsg("ENROLAR", "Todos los", "estudiantes OK");
    notificarError();
    return;
  }

  char msg[MSG_MAX];
  snprintf(msg, sizeof(msg), "ID %d", id);
  pantallaMsg("Enrolar a:", nombre, msg);
  notificarOk();
  delay(2000);

  enrollarDedoEnId(id, nombre);
}

bool borrarTemplate(uint8_t id) {
  if (!as608Ok) return false;
  return (finger.deleteModel(id) == FINGERPRINT_OK);
}

bool limpiarHuellas() {
  if (!as608Ok) return false;
  return (finger.emptyDatabase() == FINGERPRINT_OK);
}
