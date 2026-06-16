#include "rtc_helper.h"
#include "constantes.h"
#include <Wire.h>

static RTC_DS3231 rtc;
static bool rtcOk = false;

// Inicializa el RTC DS3231. Si perdio bateria, ajusta desde hora de compilacion.
bool rtcInit() {
  rtcOk = rtc.begin();
  if (!rtcOk) return false;

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  return true;
}

// Verifica si el RTC responde en I2C
bool rtcPresente() {
  Wire.beginTransmission(RTC_ADDRESS);
  rtcOk = (Wire.endTransmission() == 0);
  return rtcOk;
}

// Lee la hora actual del RTC y la formatea como YYYY-MM-DD y HH:MM:SS
void obtenerFechaHora(char* fechaOut, char* horaOut) {
  if (!rtcOk) {
    strcpy(fechaOut, "0000-00-00");
    strcpy(horaOut, "00:00:00");
    return;
  }
  DateTime now = rtc.now();
  snprintf(fechaOut, FECHA_MAX, "%04d-%02d-%02d", now.year(), now.month(), now.day());
  snprintf(horaOut, HORA_MAX, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
}
