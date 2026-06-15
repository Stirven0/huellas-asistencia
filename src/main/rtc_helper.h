#ifndef RTC_HELPER_H
#define RTC_HELPER_H

#include <RTClib.h>

bool rtcInit();
bool rtcPresente();
void obtenerFechaHora(char* fechaOut, char* horaOut);

#endif
