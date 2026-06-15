#include "enrolamiento.h"
#include "constantes.h"
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

bool huellaYaExiste(uint8_t* idExistente) {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return false;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return false;
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    *idExistente = finger.fingerID;
    return true;
  }
  return false;
}

void enrollarDedo() {
  if (!as608Ok) {
    pantallaMsg("ERROR", "AS608 no", "disponible");
    beepError();
    return;
  }

  uint8_t idExistente;
  if (huellaYaExiste(&idExistente)) {
    char nombre[NOMBRE_MAX];
    char msg[22];
    if (buscarNombre(idExistente, nombre))
      snprintf(msg, sizeof(msg), "Ya es ID %d: %s", idExistente, nombre);
    else
      snprintf(msg, sizeof(msg), "Ya es ID %d", idExistente);
    pantallaMsg("DUP DOBLADO", msg, "");
    beepError();
    delay(2000);
    return;
  }

  pantallaMsg("ENROLAR", "Dedo 1 de 2", "");
  if (!esperarDedo()) return;

  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) { beepError(); return; }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) { beepError(); return; }

  pantallaMsg("ENROLAR", "Retira dedo", "");
  if (!esperarSinDedo()) return;

  pantallaMsg("ENROLAR", "Dedo 2 de 2", "");
  if (!esperarDedo()) return;

  p = finger.getImage();
  if (p != FINGERPRINT_OK) { beepError(); return; }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) { beepError(); return; }

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    pantallaMsg("ENROLAR", "No coinciden", "");
    beepError();
    return;
  }

  if (huellaYaExiste(&idExistente)) {
    pantallaMsg("DUP ENROL", "Huella ya existe", "");
    beepError();
    return;
  }

  finger.getTemplateCount();
  uint8_t id = finger.templateCount + 1;

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    char msg[16];
    snprintf(msg, sizeof(msg), "ID %d OK", id);
    pantallaMsg("ENROLAR", msg, "");
    beepExito();
  } else {
    pantallaMsg("ENROLAR", "Error guardar", "");
    beepError();
  }
}

bool borrarTemplate(uint8_t id) {
  if (!as608Ok) return false;
  return (finger.deleteModel(id) == FINGERPRINT_OK);
}
