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

void alertarPerdidaOLED() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}

void alertarPerdida(const char* nombre, const char* solucion) {
  digitalWrite(LED_BUILTIN, HIGH);
  beepError();
  digitalWrite(LED_BUILTIN, LOW);
  pantallaMsg(nombre, "Perdido", solucion);
}

void detectarPerifericos() {
  while (true) {
    if (pantallaInit()) break;
    alertarPerdidaOLED();
    delay(2000);
  }

  while (true) {
    if (rtcInit()) break;
    alertarPerdida("RTC", "Revisar conexion I2C");
    delay(2000);
  }

  while (true) {
    if (as608Init()) break;
    if (pantallaPresente())
      alertarPerdida("AS608", "Verificar cable UART");
    else
      alertarPerdidaOLED();
    delay(2000);
  }

  while (true) {
    if (initSD()) break;
    alertarPerdida("microSD", "Insertar tarjeta SPI");
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
    digitalWrite(LED_BUILTIN, HIGH);
    beepExito();
    digitalWrite(LED_BUILTIN, LOW);
    pantallaMsg(nombre, mensaje, "");
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

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, LOW);

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

  if (oledAlerta) {
    alertarPerdidaOLED();
    delay(300);
    return;
  }

  if (sdAlerta) {
    alertarPerdida("microSD", "Insertar tarjeta");
    delay(300);
    return;
  }

  if (rtcAlerta) {
    alertarPerdida("RTC", "Revisar conexion I2C");
    delay(300);
    return;
  }

  if (as608Alerta) {
    alertarPerdida("AS608", "Verificar cable UART");
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
