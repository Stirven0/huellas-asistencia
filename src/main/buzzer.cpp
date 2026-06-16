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
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}
