# Plan de Aplicación — Sistema Biométrico de Asistencia

## Archivos

### En microSD

| Archivo | Formato | Propósito |
|---------|---------|-----------|
| `ALUMNOS.CSV` | `ID,Nombre,Apellido` | Catálogo de estudiantes (30 por curso). Creado en PC, copiado a la microSD (o generado por initSD). |
| `ASIST.CSV` | `ID,Fecha,Hora` | Registro de asistencia. Generado por el Arduino. Un registro por estudiante por día. |

Archivos de ejemplo en `examples/`:
- `examples/ALUMNOS.CSV` — mismos 30 estudiantes usados en pruebas reales
- `examples/ASIST.CSV` — 4 semanas de registros (Jun 2026, lunes a viernes, 20 días), 361 registros generados con patrón semi-aleatorio
- `examples/reporte.py` — script Python para generar tabla de asistencia por estudiante. Uso: `python3 examples/reporte.py` (requiere pandas):

  | Asistencia | IDs | Días presentes |
  |------------|-----|---------------|
  | Perfecta | 01, 02, 03, 05, 06 | 20/20 |
  | Alta (1-3 ausencias) | 04, 07, 08, 10, 12, 14, 18 | 15-17/20 |
  | Media (4-8 ausencias) | 09, 11, 15, 20, 22, 24, 25 | 10-14/20 |
  | Baja (9-16 ausencias) | 13, 16, 17, 19, 21, 23, 26, 27, 28, 29, 30 | 4-9/20 |

### En código (`src/main/`)

| Módulo | Archivos | Propósito |
|--------|----------|-----------|
| `almacenamiento` | `.h` + `.cpp` | initSD(), CSV read/write, duplicate check, formatearCSVs() |
| `almacenamiento` | `.h` + `.cpp` | initSD() (crea CSVs si faltan), CSV read/write, duplicate check, buscarSinHuella(), hayEstudiantes(), formatearCSVs() |
| `asistencia` | `.h` + `.cpp` | Toma de asistencia + corrección |
| `enrolamiento` | `.h` + `.cpp` | AS608 init, esperarDedo/SinDedo, capturarHuella(), enrollarDedo(), enrollarDedoEnId(), borrarTemplate(), limpiarHuellas() |
| `pantalla` | `.h` + `.cpp` | OLED display init/presente/reinit, pantallaMsg() |
| `notificador` | `.h` + `.cpp` | notificarOk(), notificarError(), notificarAlerta() — LED13 + buzzer |
| `rtc_helper` | `.h` + `.cpp` | RTC DS3231 init, obtenerFechaHora() |
| `verificador` | `.h` + `.cpp` | Periferico{} struct + verificarPeriferico() (reemplaza 4× checks duplicados) |
| `modos` | `.h` + `.cpp` | HANDLERS_MODO[] array + NOMBRES_MODO[], ejecutarModo(), cambiarModo() |
| `constantes.h` | — | Pines, CSV, modos, buffers |

## Pin TCH del AS608 — DESACTIVADO

El pin **TCH** (Touch) del AS608 se desactivó porque se disparaba sin dedo presente (ruido eléctrico de los conversores DC-DC). La detección de dedo se hace vía UART polling con `finger.getImage()`.

### Función `esperarDedo()` — polling por UART

```
esperarDedo():
  └── while finger.getImage() != FINGERPRINT_OK:
  └── timeout 5s si no hay dedo
  └── botón también cancela
```

El pin TCH del AS608 (Pin 6) no se usa en la implementación actual; la detección se hace vía UART polling con `finger.getImage()`.

## Modos de operación (seleccionados con pulsador Pin 3)

| N° | Modo | Descripción |
|----|------|-------------|
| 0 | Asistencia (default) | Estudiantes marcan entrada. |
| 1 | Enrolamiento | Enrolar huellas nuevas (vinculado a ALUMNOS.CSV). |
| 2 | Corrección | Borrar y re-enrolar una huella específica. |
| 3 | Formatear | Doble confirmación 3s → borrar todo (huellas + CSVs). |

---

## Flujo: Asistencia

```
INICIO
  └── OLED: "Coloca dedo..."

ESTUDIANTE coloca dedo
  └── esperarDedo()  // getImage() polling, 5s timeout
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
