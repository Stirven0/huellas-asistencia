#include "verificador.h"

bool verificarPeriferico(Periferico* p) {
  if (!p->presente()) {
    *p->flag = true;
    return false;
  }
  if (*p->flag) {
    p->reinit();
    *p->flag = false;
    return true;
  }
  return false;
}
