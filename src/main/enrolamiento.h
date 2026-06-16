#ifndef ENROLAMIENTO_H
#define ENROLAMIENTO_H

#include <Arduino.h>
#include <Adafruit_Fingerprint.h>

extern Adafruit_Fingerprint finger;

bool as608Init();
bool as608Presente();
bool esperarDedo();
bool esperarSinDedo();
bool capturarHuella(uint8_t* idOut);
void enrollarDedo();
bool borrarTemplate(uint8_t id);
bool huellaYaExiste(uint8_t* idExistente);
bool limpiarHuellas();

#endif
