# Plan de Aplicación — Sistema Biométrico de Asistencia

## Archivos

### En microSD

| Archivo | Formato | Propósito |
|---------|---------|-----------|
| `ESTUDIANTES.CSV` | `ID,Nombre,Apellido` | Catálogo de estudiantes (30 por curso). Creado en PC, copiado a la microSD. |
| `ASIST.CSV` | `ID,Fecha,Hora` | Registro de asistencia. Generado por el Arduino. Un registro por estudiante por día. |

### En código (`src/`)

| Módulo | Archivos | Propósito |
|--------|----------|-----------|
| `estudiantes/` | `.h` + `.cpp` | Leer `ESTUDIANTES.CSV`, buscar nombre por ID |
| `enrollment/` | `.h` + `.cpp` | Enrolamiento con verificación de duplicados |
| `attendance/` | `.h` + `.cpp` | Toma de asistencia con detección de duplicados (`ASIST.CSV`) |
| `storage/` | `.h` + `.cpp` | Inicializar microSD, leer/escribir CSVs |
| `display/` | `.h` + `.cpp` | OLED + buzzer (retroalimentación visual/sonora) |
| `utils/` | `constantes.h`, `rtc_helper.cpp` | Constantes, RTC |

## Conexiones adicionales por TCH

| Pin | Señal | Conecta a |
|-----|-------|-----------|
| Mega 6 | FP_TOUCH_PIN | AS608 TCH |
| 3.3V | - | AS608 VA |

## Pin TCH del AS608

El pin **TCH** (Touch) del AS608 es una salida digital que va a **HIGH** cuando detecta un dedo sobre el sensor. Se conecta a `FP_TOUCH_PIN` (pin 6 del Mega). VA se alimenta a 3.3V.

### Función `esperarDedo()` — usada en todos los flujos

```
esperarDedo():
  └── while digitalRead(FP_TOUCH_PIN) == LOW:
  └── (TCH pasa a HIGH → dedo presente)
  └── delay(DEBOUNCE_DELAY)  // estabilizar
  └── retornar
```

Ventaja: no necesita `finger.getImage()` para saber si hay dedo, reduce tráfico Serial1 y da respuesta inmediata.

## Modos de operación (seleccionados con pulsador Pin 7)

| Pulsaciones | Modo | Descripción |
|---|---|---|
| 1 | Asistencia | Modo por defecto. Estudiantes marcan entrada. |
| 2 | Enrolamiento | Enrolar huellas nuevas (con verificación de duplicados). |
| 3 | Corrección | Borrar y re-enrolar una huella específica. |

---

## Flujo: Asistencia

```
INICIO
  └── OLED: "Coloca dedo..."

ESTUDIANTE coloca dedo
  └── esperarDedo()  // TCH bloquea hasta dedo presente
  └── AS608 captura + fingerSearch()
      ├── Sin match → BEEP largo, OLED: "No reconocido", vuelve al inicio
      └── Match → ID = finger.fingerID

  └── buscarNombre(ID) en ESTUDIANTES.CSV

  └── OLED: "Bienvenido, {Nombre}"

  └── leer RTC → fecha + hora

  └── verificar duplicado en ASIST.CSV
      ├── Mismo ID + misma fecha ya existe
      │   └── BEEP largo, OLED: "Ya registrado hoy", vuelve
      └── No es duplicado
          └── ASIST.CSV append: ID,Fecha,Hora
          └── BEEP corto, OLED: "Asistencia OK", vuelve
```

---

## Flujo: Enrolamiento (con verificación de duplicados)

```
Admin selecciona MODO ENROLAMIENTO (pulsador ×2)

  └── Arduino lee ESTUDIANTES.CSV completa
  └── Arduino lee finger.getTemplateCount() del AS608

  └── Recorre CSV, encuentra primer ID sin huella en AS608
      └── OLED: "Enrolar ID X: {Nombre}"
      └── OLED: "Pulsador = confirmar"

  └── Admin verifica que ES ese estudiante → presiona pulsador

      └── OLED: "Coloca el dedo, {Nombre}"
      └── esperarDedo()  // TCH bloquea hasta dedo presente
      └── AS608 captura imagen 1
      └── esperarDedo()  // espera a que retiren
      └── esperarDedo()  // espera dedo para imagen 2
      └── AS608 captura imagen 2
      └── AS608 crea modelo (finger.createModel())

      └── Antes de guardar: fingerSearch() con el modelo creado
          ├── Match encontrado → esa huella ya existe en ID Y
          │   └── BEEP largo
          │   └── OLED: "ERROR: esta huella ya es ID Y ({NombreY})"
          │   └── No se guarda. Vuelve a pedir confirmación.
          └── Sin match → huella nueva
              └── finger.storeModel(X) → BEEP corto
              └── OLED: "ID X: {Nombre} enrolado ✓"
              └── Pasa automáticamente al siguiente ID pendiente

  └── Cuando todos los IDs están enrolados:
      └── OLED: "Curso completo (30/30)"
      └── Vuelve a MODO ASISTENCIA
```

---

## Flujo: Corrección (borrar y re-enrolar)

```
Admin selecciona MODO CORRECCIÓN (pulsador ×3)

  └── OLED: "Coloca dedo del estudiante a corregir"

  └── esperarDedo()  // TCH
  └── AS608 busca → devuelve ID y nombre desde ESTUDIANTES.CSV

  └── OLED: "ID X: {Nombre}"
      └── OLED: "Pulsador = borrar y re-enrolar"

  └── Admin presiona pulsador
      └── finger.deleteModel(X) → BEEP
      └── OLED: "ID X borrado. Coloca dedo correcto."

      └── (Estudiante correcto coloca dedo 2 tomas)
      └── fingerSearch() → verificar no duplicado
      └── finger.storeModel(X) → BEEP corto
      └── OLED: "ID X: {Nombre correcto} enrolado ✓"
```

---

## Reglas clave

- **Sin duplicados en AS608**: fingerSearch() antes de storeModel() en enrolamiento
- **Sin duplicados en ASIST.CSV**: mismo ID + misma fecha = rechazado
- **IDs 1-127**: límite del AS608, máximo 30 estudiantes por curso
- **100% offline**: todo funciona sin internet, el CSV se procesa en PC después
