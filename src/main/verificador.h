#ifndef VERIFICADOR_H
#define VERIFICADOR_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  bool (*presente)();
  bool (*reinit)();
  bool* flag;
} Periferico;

bool verificarPeriferico(Periferico* p);

#endif
