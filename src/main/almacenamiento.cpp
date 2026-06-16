#include "almacenamiento.h"
#include "constantes.h"

static bool sdOk = false;

bool initSD() {
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  sdOk = SD.begin(SD_CS_PIN);
  return sdOk;
}

bool sdPresente() {
  return initSD();
}

bool buscarNombre(uint8_t id, char* nombreOut) {
  if (!sdOk) return false;
  File f = SD.open(ESTUDIANTES_CSV);
  if (!f) return false;

  char buf[LINEA_MAX];
  bool encontrado = false;
  f.readBytesUntil('\n', buf, sizeof(buf));

  while (f.available()) {
    int len = f.readBytesUntil('\n', buf, sizeof(buf));
    buf[len] = '\0';

    uint8_t idLeido = atoi(buf);
    if (idLeido == id) {
      char* coma = strchr(buf, ',');
      if (coma) {
        coma++;
        char* coma2 = strchr(coma, ',');
        if (coma2) *coma2 = '\0';
        strncpy(nombreOut, coma, NOMBRE_MAX - 1);
        nombreOut[NOMBRE_MAX - 1] = '\0';
        encontrado = true;
      }
      break;
    }
  }
  f.close();
  return encontrado;
}

bool registrarAsistencia(uint8_t id, const char* fecha, const char* hora) {
  if (!sdOk) return false;
  File f = SD.open(ASIST_CSV, FILE_WRITE);
  if (!f) return false;

  if (f.size() == 0) f.println(CSV_HEADER);

  char linea[LINEA_MAX];
  snprintf(linea, sizeof(linea), "%d,%s,%s", id, fecha, hora);
  f.println(linea);
  f.close();
  return true;
}

bool esDuplicado(uint8_t id, const char* fecha) {
  if (!sdOk) return false;
  File f = SD.open(ASIST_CSV);
  if (!f) return false;

  char buf[LINEA_MAX];
  f.readBytesUntil('\n', buf, sizeof(buf));

  while (f.available()) {
    int len = f.readBytesUntil('\n', buf, sizeof(buf));
    buf[len] = '\0';

    uint8_t idLeido = atoi(buf);
    if (idLeido != id) continue;

    if (strstr(buf, fecha) != NULL) {
      f.close();
      return true;
    }
  }
  f.close();
  return false;
}

bool formatearCSVs() {
  if (!sdOk) return false;
  if (SD.exists(ESTUDIANTES_CSV)) SD.remove(ESTUDIANTES_CSV);
  if (SD.exists(ASIST_CSV)) SD.remove(ASIST_CSV);
  File f = SD.open(ESTUDIANTES_CSV, FILE_WRITE);
  if (f) { f.println("ID,Nombre,Apellido"); f.close(); }
  f = SD.open(ASIST_CSV, FILE_WRITE);
  if (f) { f.println(CSV_HEADER); f.close(); }
  return true;
}
