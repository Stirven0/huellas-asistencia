#include "verificador.h"

// Verifica un periferico usando su funcion presente().
// Si falla, marca flag. Si se recupera, llama reinit() y desmarca flag.
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
