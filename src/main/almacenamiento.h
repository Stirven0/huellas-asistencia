#ifndef ALMACENAMIENTO_H
#define ALMACENAMIENTO_H

#include <Arduino.h>
#include <SD.h>

bool initSD();
bool sdPresente();
bool buscarNombre(uint8_t id, char* nombreOut);
bool registrarAsistencia(uint8_t id, const char* fecha, const char* hora);
bool esDuplicado(uint8_t id, const char* fecha);

#endif
