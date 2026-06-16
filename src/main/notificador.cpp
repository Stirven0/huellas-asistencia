#include <Arduino.h>
#include "notificador.h"
#include "constantes.h"

// Pitido corto + LED: operacion exitosa
void notificarOk() {
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(BUZZER_SHORT);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

// Pitido largo + LED: operacion fallida
void notificarError() {
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(BUZZER_LONG);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

// Doble pitido + LED: alerta de periferico perdido
void notificarAlerta() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(BUZZER_DOUBLE);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    delay(BUZZER_DOUBLE);
  }
}
