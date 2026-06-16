#include "almacenamiento.h"
#include "constantes.h"
#include "enrolamiento.h"

static bool sdOk = false;

// Inicializa la microSD y crea CSVs si no existen
bool initSD() {
  sdOk = SD.begin(SD_CS_PIN);
  if (!sdOk) return false;

  if (!SD.exists(ESTUDIANTES_CSV)) {
    File f = SD.open(ESTUDIANTES_CSV, FILE_WRITE);
    if (f) { f.println(ESTUDIANTES_HEADER); f.close(); }
  }
  if (!SD.exists(ASIST_CSV)) {
    File f = SD.open(ASIST_CSV, FILE_WRITE);
    if (f) { f.println(CSV_HEADER); f.close(); }
  }
  return true;
}

// Re-verifica presencia de la microSD (re-inicializa)
bool sdPresente() {
  return initSD();
}

// Busca el nombre de un estudiante por su ID en ALUMNOS.CSV
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

// Agrega un registro de asistencia (ID,Fecha,Hora) a ASIST.CSV
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

// Verifica si ya existe un registro del mismo ID en la misma fecha
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

// Verifica si ALUMNOS.CSV tiene al menos un estudiante (excluye header)
bool hayEstudiantes() {
  if (!sdOk) return false;
  File f = SD.open(ESTUDIANTES_CSV);
  if (!f) return false;

  char buf[LINEA_MAX];
  f.readBytesUntil('\n', buf, sizeof(buf));

  bool encontrado = false;
  while (f.available()) {
    int len = f.readBytesUntil('\n', buf, sizeof(buf));
    if (len > 0) { encontrado = true; break; }
  }
  f.close();
  return encontrado;
}

// Elimina y recrea ambos CSVs con sus headers
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

// Busca el primer estudiante en ALUMNOS.CSV sin huella registrada en el AS608.
// Itera los IDs del CSV y llama finger.loadModel() para verificar si existe template.
bool buscarSinHuella(uint8_t* idOut, char* nombreOut) {
  if (!sdOk) return false;
  File f = SD.open(ESTUDIANTES_CSV);
  if (!f) return false;

  char buf[LINEA_MAX];
  f.readBytesUntil('\n', buf, sizeof(buf));

  while (f.available()) {
    int len = f.readBytesUntil('\n', buf, sizeof(buf));
    buf[len] = '\0';
    if (len == 0) continue;

    uint8_t id = atoi(buf);
    if (finger.loadModel(id) != FINGERPRINT_OK) {
      char* coma = strchr(buf, ',');
      if (coma) {
        coma++;
        char* coma2 = strchr(coma, ',');
        if (coma2) *coma2 = '\0';
        strncpy(nombreOut, coma, NOMBRE_MAX - 1);
        nombreOut[NOMBRE_MAX - 1] = '\0';
      }
      *idOut = id;
      f.close();
      return true;
    }
  }
  f.close();
  return false;
}
