# Plan de Aplicación — Sistema Biométrico de Asistencia

## Archivos

### En microSD

| Archivo | Formato | Propósito |
|---------|---------|-----------|
| `ESTUDIANTES.CSV` | `ID,Nombre,Apellido` | Catálogo de estudiantes (30 por curso). Creado en PC, copiado a la microSD. |
| `ASIST.CSV` | `ID,Fecha,Hora` | Registro de asistencia. Generado por el Arduino. Un registro por estudiante por día. |

### En código (`src/main/`)

| Módulo | Archivos | Propósito |
|--------|----------|-----------|
| `almacenamiento` | `.h` + `.cpp` | initSD(), CSV read/write, duplicate check, formatearCSVs() |
| `asistencia` | `.h` + `.cpp` | Toma de asistencia + corrección |
| `enrolamiento` | `.h` + `.cpp` | AS608 init, capturarHuella(), enroll, delete, limpiarHuellas() |
| `pantalla` | `.h` + `.cpp` | OLED display (buzzer extraído a módulo propio) |
| `buzzer` | `.h` + `.cpp` | beepExito(), beepError(), alertaDoble() |
| `rtc_helper` | `.h` + `.cpp` | RTC DS3231 init, obtenerFechaHora() |
| `verificador` | `.h` + `.cpp` | Periferico{} array + verificarPeriferico() (reemplaza 4× checks) |
| `modos` | `.h` + `.cpp` | HANDLERS_MODO[] array + NOMBRES_MODO[], ejecutarModo() |
| `constantes.h` | — | Pines, CSV, modos, buffers |

## Conexiones adicionales por TCH

| Pin | Señal | Conecta a |
|-----|-------|-----------|
| Mega 6 | FP_TOUCH_PIN | AS608 TCH |
| 3.3V | - | AS608 VA |

## Pin TCH del AS608

El pin **TCH** (Touch) del AS608 es una salida digital que va a **HIGH** cuando detecta un dedo sobre el sensor. Se conecta a `FP_TOUCH_PIN` (pin 6 del Mega). VA se alimenta a 3.3V.

### Función `esperarDedo()` — polling por UART

```
esperarDedo():
  └── while finger.getImage() != FINGERPRINT_OK:
  └── timeout 5s si no hay dedo
  └── botón también cancela
```

El pin TCH del AS608 (Pin 6) no se usa en la implementación actual; la detección se hace vía UART polling con `finger.getImage()`.

## Modos de operación (seleccionados con pulsador Pin 3)

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

  └── Verifica duplicado: fingerSearch()
      ├── Huella ya existe → OLED: "Ya es ID {N}", rechaza
      └── Huella nueva → continúa

  └── OLED: "Dedo 1 de 2"
  └── esperarDedo()  // getImage() polling, 5s timeout
  └── AS608 captura imagen 1 (image2Tz(1))

  └── OLED: "Retira dedo"
  └── esperarSinDedo()

  └── OLED: "Dedo 2 de 2"
  └── esperarDedo()
  └── AS608 captura imagen 2 (image2Tz(2))

  └── finger.createModel()
      ├── Falla → "No coinciden", BEEP largo
      └── OK → finger.getTemplateCount() → ID = count + 1

  └── finger.storeModel(ID)
      ├── OK → "ID {N} OK", BEEP corto
      └── Error → "Error guardar", BEEP largo
```

---

## Flujo: Corrección (borrar y re-enrolar)

```
Admin selecciona MODO CORRECCIÓN (pulsador ×2)

  └── OLED: "Coloca dedo del estudiante a corregir"

  └── esperarDedo()  // getImage() polling
  └── capturarHuella() → ID + nombre desde ESTUDIANTES.CSV

  └── OLED: "ID {N}: {Nombre}" + "Boton=reemplazar"
  └── 4s para presionar botón
      └── Sin confirmación → "Sin confirmacion"
      └── Botón presionado:
          └── finger.deleteModel(ID) → BEEP
          └── enrollarDedo() (flujo completo de 2 tomas)
          └── "Completado", BEEP corto
```

---

---

## Flujo: Formatear (borrar todo)

```
Admin selecciona MODO FORMATEAR (pulsador ×3)

  └── OLED: "Manten 3s para confirmar"
  └── Mantiene botón 3s → paso 1
      └── Suelta antes → vuelve a ASISTENCIA

  └── OLED: "Manten 3s para ejecutar"
  └── Mantiene botón 3s → paso 2
      └── Suelta antes → vuelve a ASISTENCIA

  └── OLED: "FORMATEANDO... No apagar"
  └── finger.emptyDatabase()
  └── Elimina ASIST.CSV y ESTUDIANTES.CSV
  └── Recrea CSVs con headers
  └── "SISTEMA FORMATEADO", BEEP
  └── Vuelve a MODO ASISTENCIA
```

> Doble confirmación de 3s para evitar formateo accidental.

---

## Reglas clave

- **Sin duplicados en AS608**: fingerSearch() antes de storeModel() en enrolamiento
- **Sin duplicados en ASIST.CSV**: mismo ID + misma fecha = rechazado
- **IDs 1-127**: límite del AS608, máximo 30 estudiantes por curso
- **100% offline**: todo funciona sin internet, el CSV se procesa en PC después
