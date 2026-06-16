# Reglas de Buenas Prácticas — opencode

## Principios SOLID (adaptados para AVR)

### S — Single Responsibility Principle
- Un archivo `.cpp` = una responsabilidad. Ej: `notificador` unifica notificaciones LED13 + buzzer, `pantalla` solo OLED.
- `notificador` es la EXCEPCIÓN deliberada: todas las notificaciones llevan LED13 + buzzer (el usuario necesita ambas señales).

### O — Open/Closed Principle
- Nuevos modos: agregar handler en `HANDLERS_MODO[]` y nombre en `NOMBRES_MODO[]`. No modificar `ejecutarModo()`.
- Nuevos periféricos: agregar entrada en `perifericos[]` con punteros a `presente()`/`reinit()`. No modificar `verificarPeriferico()`.

### L — Liskov Substitution
- No aplica (sin herencia polimórfica — AVR no tiene virtual functions).

### I — Interface Segregation
- Cada módulo expone solo las funciones que necesita el exterior. Ver `pantalla.h`: solo init/presente/reinit/msg.

### D — Dependency Inversion
- `verificador.h` depende de punteros a función, no de tipos concretos. Cualquier periférico que implemente `bool presente()` + `bool reinit()` puede ser verificado.

## Patrones de Diseño (AVR-safe)

| Patrón | Dónde | Cómo |
|--------|-------|------|
| **Strategy** | `modos.cpp` | Array de punteros a función `HANDLERS_MODO[]` — selecciona comportamiento en runtime |
| **Command** | `HANDLERS_MODO[]` | Cada entrada encapsula una operación como puntero a función |
| **Template Method** | `verificador.cpp` | `verificarPeriferico()` define el esqueleto (present? → flag/reinit), las subrutinas son los punteros |
| **Null Object** | `buscarNombre()` | Si no encuentra nombre, retorna false sin error; el caller muestra "ID {n}" |
| **Observer** | `main.ino` loop | Sondeo periódico del pulsador y periféricos (no hay interrupciones) |

## Convenciones de Código

### Nombres
- `camelCase` para funciones y variables
- `UPPER_SNAKE_CASE` para constantes (`#define`)
- Archivos: `snake_case.cpp/.h`
- Prefijo de módulo en funciones públicas: `pantallaInit()`, `as608Presente()`, `rtcInit()`, etc.
- Funciones booleanas: formato verbo → `sdPresente()`, `esDuplicado()`, `pantallaInit()`

### Strings
- NO usar `String` class. Usar `char[]` + `snprintf()`.
- Tamaños definidos en `constantes.h`: `NOMBRE_MAX`, `LINEA_MAX`, `MSG_MAX`, `FECHA_MAX`, `HORA_MAX`.
- `snprintf(dest, sizeof(dest), ...)` siempre.

### Errores de Periféricos
- En `detectarPerifericos()` (cascada de inicio) y `loop()` (monitoreo): `pantallaMsg()` + `notificarError()`. Si OLED falló: `notificarAlerta()`.
- No agregar `digitalWrite(LED_BUILTIN)` alrededor de notificaciones — `notificador` lo maneja internamente.

### Mensajes en OLED
- `pantallaMsg(header, línea2, línea3)`
- Header = modo en MAYÚSCULAS (`"ASISTENCIA"`, `"ENROLAR"`, etc.) o tipo de error (`"DUPLICADO"`, `"ERROR"`)
- Línea3 usa `""` cuando está vacía
- Todos en ASCII, sin acentos

### Constantes
- Fuente única: `src/main/constantes.h`
- `src/utils/constantes.h` está obsoleto — NO EDITAR
- Agregar constantes numéricas (no literales mágicas). Ej: `BUZZER_DOUBLE 200` en vez de `delay(200)`.
- `MODOS_COUNT` en vez de hardcodear `4`.

### Agregar un Modo Nuevo
1. Agregar `#define MODO_NUEVO X` y actualizar `MODOS_COUNT` en `constantes.h`
2. Agregar nombre en `NOMBRES_MODO[]` en `modos.cpp`
3. Agregar handler en `HANDLERS_MODO[]` en `modos.cpp`
4. Crear función handler en módulo nuevo o existente

### Agregar un Periférico Nuevo
1. Implementar `init()`, `presente()`, `reinit()` (si aplica) en nuevo `.cpp/.h`
2. En `setup()` de `main.ino`: agregar entrada en `perifericos[]`
3. Agregar flag `bool xxxAlerta` en `main.ino`
4. Agregar detección en `detectarPerifericos()` en cascada
5. Agregar monitoreo en `loop()` con prioridad (OLED primero)

### Estructura de Archivos
```
src/main/
  ├── main.ino          # Setup, loop, button, format mode, peripheral cascade + monitoring
  ├── constantes.h      # All #defines (pins, sizes, mode numbers, CSV names)
  ├── notificador.cpp/.h  # LED13 + buzzer unified (notificarOk, notificarError, notificarAlerta)
  ├── pantalla.cpp/.h   # OLED init + text display
  ├── rtc_helper.cpp/.h # DS3231 init + timestamp
  ├── almacenamiento.cpp/.h  # microSD init + CSV R/W
  ├── enrolamiento.cpp/.h    # AS608 init + fingerprint operations + enrollment
  ├── asistencia.cpp/.h      # Attendance + correction flows
  ├── verificador.cpp/.h     # Periferico struct + verificarPeriferico()
  └── modos.cpp/.h           # Mode handler array + dispatch
```

### Memoria
- `static` para flags internos de módulo (`sdOk`, `pantallaOk`, etc.)
- Sin `new`/`delete`, sin `malloc`/`free`
- SRAM ~2.3KB usados (27%), flash ~36KB (14%) — mantener bajo control

### Consistencia entre Mensajes
- `pantallaMsg(l1, l2, l3)` donde l1 es el contexto en MAYÚSCULAS
- NO usar headers inventados como `"DUP DOBLADO"` o `"DUP ENROL"` — usar `"DUPLICADO"` o el nombre del modo
- `notificarOk()` después de operación exitosa, `notificarError()` después de fallo, `notificarAlerta()` cuando OLED no disponible
- Notificador controla LED13 + buzzer siempre — no hacer `digitalWrite(LED_BUILTIN)` alrededor de estas llamadas
