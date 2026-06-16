#ifndef MODOS_H
#define MODOS_H

#include <stdint.h>

extern const char* NOMBRES_MODO[4];

void ejecutarModo(uint8_t modo);
void cambiarModo(uint8_t* modoActual);

#endif
