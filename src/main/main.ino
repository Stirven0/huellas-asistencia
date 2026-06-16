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
 * Modos: ASISTENCIA (defecto) | ENROLAR | CORREGIR | FORMATEAR
 * Pulsador Pin 3 cambia modo.
 */

#include "constantes.h"
#include "buzzer.h"
#include "pantalla.h"
#include "rtc_helper.h"
#include "almacenamiento.h"
#include "enrolamiento.h"
#include "asistencia.h"
#include "verificador.h"
#include "modos.h"

uint8_t modoActual = MODO_ASISTENCIA;
bool botonPendiente = false;
unsigned long ultimoBoton = 0;
unsigned long ultimaVerificacion = 0;

bool oledAlerta = false;
bool rtcAlerta = false;
bool as608Alerta = false;
bool sdAlerta = false;

#define PERIF_COUNT 4
static Periferico perifericos[PERIF_COUNT];

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

void revisarBoton() {
  if (digitalRead(BUTTON_PIN) == LOW && millis() - ultimoBoton > BOTON_COOLDOWN_MS) {
    ultimoBoton = millis();
    botonPendiente = true;
  }
}

void mostrarRecuperacion(const char* nombre, const char* mensaje) {
  if (!oledAlerta) {
    pantallaMsg(nombre, mensaje, "");
    beepExito();
    delay(1000);
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

  limpiarHuellas();
  formatearCSVs();

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

  perifericos[0] = (Periferico){pantallaPresente, pantallaReinit, &oledAlerta};
  perifericos[1] = (Periferico){sdPresente,     initSD,          &sdAlerta};
  perifericos[2] = (Periferico){rtcPresente,    rtcInit,         &rtcAlerta};
  perifericos[3] = (Periferico){as608Presente,  as608Init,       &as608Alerta};

  ultimaVerificacion = millis();
}

void loop() {
  if (millis() - ultimaVerificacion > VERIFICAR_CADA_MS) {
    ultimaVerificacion = millis();

    static const char* nombres[PERIF_COUNT] = {"OLED", "microSD", "RTC", "AS608"};
    static const char* recuperados[PERIF_COUNT] = {"Recuperada", "Recuperada", "Recuperado", "Recuperado"};

    for (uint8_t i = 0; i < PERIF_COUNT; i++) {
      if (verificarPeriferico(&perifericos[i])) {
        if (i == 0) {
          pantallaMsg(nombres[i], recuperados[i], "");
          beepExito();
          delay(1000);
        } else {
          mostrarRecuperacion(nombres[i], recuperados[i]);
        }
      }
    }
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
    cambiarModo(&modoActual);
    return;
  }

  ejecutarModo(modoActual);

  for (int i = 0; i < 10; i++) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      botonPendiente = true;
      return;
    }
    delay(100);
  }
}
