#include <Arduino.h>
#include "buzzer.h"
#include "constantes.h"

void beepExito() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(BUZZER_SHORT);
  digitalWrite(BUZZER_PIN, LOW);
}

void beepError() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(BUZZER_LONG);
  digitalWrite(BUZZER_PIN, LOW);
}

void alertaDoble() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(BUZZER_DOUBLE);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    delay(BUZZER_DOUBLE);
  }
}
