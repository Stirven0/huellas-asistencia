#include "rtc_helper.h"
#include "constantes.h"
#include <Wire.h>

static RTC_DS3231 rtc;
static bool rtcOk = false;

bool rtcInit() {
  rtcOk = rtc.begin();
  if (!rtcOk) return false;

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  return true;
}

bool rtcPresente() {
  if (!rtcOk) return false;
  Wire.beginTransmission(RTC_ADDRESS);
  return (Wire.endTransmission() == 0);
}

void obtenerFechaHora(char* fechaOut, char* horaOut) {
  if (!rtcOk) {
    strcpy(fechaOut, "0000-00-00");
    strcpy(horaOut, "00:00:00");
    return;
  }
  DateTime now = rtc.now();
  snprintf(fechaOut, 12, "%04d-%02d-%02d", now.year(), now.month(), now.day());
  snprintf(horaOut, 10, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
}
