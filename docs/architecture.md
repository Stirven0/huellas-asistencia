# Arquitectura Técnica

## Mapa de estados

```
                    ╔══════════════════════╗
                    ║     INICIO / RESET   ║
                    ╚══════════════════════╝
                             │
                             ▼
                    ╔══════════════════════╗
                    ║  INICIALIZACIÓN      ║
                    ║  OLED, RTC, AS608,   ║
                    ║  microSD             ║
                    ╚══════════════════════╝
                             │
                             ▼
              ┌──────────────┴──────────────┐
              │         IDLE                │
              │  OLED: "Coloca el dedo"     │
              └──────────────┬──────────────┘
                             │
                    ┌───────┴───────┐
                    │   ¿Dedo puesto?│
                    └───────┬───────┘
                            │
                      ┌─────┴─────┐
                   NO │           │ SÍ
                 ┌────┘           └────────────┐
                 │                             │
              IDLE                  ┌──────────┴──────────┐
                                   │   fingerSearch()    │
                                   │   → ID              │
                                   └──────────┬──────────┘
                                              │
                                    ┌─────────┴─────────┐
                                    │   ID ≠ 0?         │
                                    └─────────┬─────────┘
                                              │
                              ┌───────────────┼───────────────┐
                              │               │               │
                         ┌────┴────┐    ┌─────┴─────┐   ┌────┴────┐
                         │ ASIST.  │    │ ENROLAR   │   │CORREGIR │
                         │(default)│    │ (botón x1) │   │(botón x2)│
                         └────┬────┘    └─────┬─────┘   └────┬────┘
                              │               │              │
                         ┌────┴────┐    ┌─────┴─────┐   ┌────┴────┐
                         │Buscar   │    │Verificar   │   │Mostrar  │
                         │nombre en│    │duplicado   │   │ID+nombre│
                         │ESTUD.CSV│    │(AS608)     │   │+botón   │
                         └────┬────┘    └─────┬─────┘   └────┬────┘
                              │               │              │
                         ┌────┴────┐    ┌─────┴─────┐   ┌────┴────┐
                         │Leer RTC │    │Enrolar    │   │¿Confirma?│
                         └────┬────┘    │2 tomas     │   └────┬────┘
                              │         └─────┬─────┘    NO  │  SÍ
                         ┌────┴────┐         ┌───────┐   ┌───┴───┐
                         │Verificar│         │Guardar│   │Borrar │
                         │duplicado│         │modelo │   │template│
                         │ASIST.CSV│         │AS608  │   │+re-enr│
                         └────┬────┘         └───┬───┘   └───────┘
                         ┌────┴────┐            │
                      SÍ │         │ NO      ┌──┴───┐
                    ┌────┘         └─────┐   │ OK!  │
                    │                    │   │BEEP  │
               ┌────┴────┐         ┌─────┴┐  │OLED ✓│
               │RECHAZAR │         │GUARD │  └──────┘
               │BEEP larg│         │CSV   │      │
               │"Ya reg."│         │BEEP  │      │
               └─────────┘         │corto │      │
                                   └──────┘      │
                                      │          │
                                      └──────────┘
                                         │
                                     (vuelve a IDLE)

      ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─

      INTERRUPCIÓN POR BOTÓN (en cualquier estado)

      [Pulsador] → digitalRead(BUTTON_PIN) == LOW
              │
              ▼
      ┌─────────────────┐
      │ Cambiar modo:   │
      │ 0→ Asistencia   │
      │ 1→ Enrolar      │
      │ 2→ Corregir     │
      └─────────────────┘
              │
              ▼
           IDLE
```

## Flujo por modo

### ASISTENCIA
```
IDLE → dedo → fingerSearch() → ID ≠ 0?
  └─ NO → "No reconocido", BEEP largo → IDLE
  └─ SÍ → buscarNombre(ID) en ESTUDIANTES.CSV
       └─ leer RTC (fecha + hora)
            └─ verificar duplicado en ASIST.CSV (ID + fecha)
                 ├─ existe → "Ya registrado hoy", BEEP largo → IDLE
                 └─ no existe → append "ID,Fecha,Hora" → "OK {Nombre}", BEEP corto → IDLE
```

### ENROLAR
```
IDLE → dedo → fingerSearch() → ID ≠ 0?
  └─ NO → es huella nueva → continuar
  └─ SÍ → "Esta huella ya es ID {N}" → IDLE (rechazar)

Capturar imagen 1 → "Retira dedo"
Capturar imagen 2 → finger.createModel()
  └─ falla → "No coinciden", BEEP largo → IDLE

fingerSearch() (verificar duplicado otra vez)
  └─ SÍ match → "Esta huella ya existe" → IDLE

Buscar primer ID sin enrolar en AS608:
  └─ finger.storeModel(ID_libre) → BEEP corto → "ID {N} enrolado" → IDLE
```

### CORREGIR
```
IDLE → dedo → fingerSearch() → mostrar ID + nombre
  └─ Admin presiona pulsador para confirmar
       └─ finger.deleteModel(ID) → BEEP
            └─ "Coloca dedo correcto"
                 └─ capturar 2 tomas → fingerSearch() (dup check)
                      └─ finger.storeModel(ID) → "Corregido" → IDLE
```

## Detección en cascada de periféricos

El sistema inicia verificando cada periférico en orden. Si uno falla, no avanza al siguiente:

```
INICIO
  │
  ▼
┌──────────────────────────────────────────────┐
│ 1. OLED SSD1306                              │
│    └─ ¿Existe en I2C (0x3C)?                 │
│         ├─ NO → LED13 ON + Buzzer 200ms ×2   │
│         │      └─ reintentar cada 2s          │
│         └─ SÍ → mostrar "Iniciando..."        │
└──────────────────────────────────────────────┘
  │
  ▼
┌──────────────────────────────────────────────┐
│ 2. RTC DS3231                                │
│    └─ ¿Existe en I2C (0x68)?                 │
│         ├─ NO → OLED: "RTC no detectado"     │
│         │      └─ reintentar cada 2s          │
│         └─ SÍ → sincronizar hora             │
└──────────────────────────────────────────────┘
  │
  ▼
┌──────────────────────────────────────────────┐
│ 3. AS608 (huellas)                           │
│    └─ finger.verifyPassword()?               │
│         ├─ NO → OLED: "AS608 no detectado"   │
│         │      └─ reintentar cada 2s          │
│         └─ SÍ → iniciar UART                 │
└──────────────────────────────────────────────┘
  │
  ▼
┌──────────────────────────────────────────────┐
│ 4. microSD                                   │
│    └─ SD.begin(CS=53)?                       │
│         ├─ NO → OLED: "Insertar tarjeta"     │
│         │      └─ reintentar cada 2s          │
│         └─ SÍ → abrir ESTUDIANTES.CSV        │
└──────────────────────────────────────────────┘
  │
  ▼
╔══════════════════════════════════════════════╗
║           SISTEMA LISTO                      ║
║  OLED: "SISTEMA LISTO - Pulsador:cambia modo"║
╚══════════════════════════════════════════════╝
```

### Monitoreo continuo (cada 3 segundos en loop)

Durante la operación normal, el sistema verifica periódicamente:

| Periférico | Método | Si falla |
|------------|--------|----------|
| **microSD** | `SD.open("/")` → root existente? | OLED: "microSD perdida - No registrar" |
| **RTC** | `Wire.beginTransmission(0x68)` | OLED: "RTC perdido - Revisar" |
| **AS608** | `finger.verifyPassword()` | OLED: "AS608 perdido - Revisar cable" |

Si un periférico se recupera (reinsertado / reconectado), el sistema lo detecta en la siguiente verificación y muestra "Recuperado" en OLED.

## Diagrama de flujo principal

```
[Usuario: coloca dedo]
    │
    ▼
[AS608 — Capturar minutiae]
    │
    ▼
[Arduino Mega 2560 — Procesamiento central]
    │                │
    ▼                ▼
[OLED SSD1306]  [RTC DS3231 + microSD]
[Buzzer]             │
(Feedback)           │ Transferencia física (fin de jornada)
                     ▼
               [Archivo CSV]
                     │
                     ▼
           [PC + Interfaz Python]
                     │
                     ▼
           [Reporte Final de Asistencia]
```

## Diagrama de módulos de software

```
main.ino
    ├── enrollment/
    │   ├── enrolamiento.h      # Interfaz pública
    │   └── enrolamiento.cpp    # enrollFinger(), deleteTemplate()
    ├── attendance/
    │   ├── asistencia.h
    │   └── asistencia.cpp      # checkAttendance(), isDuplicate()
    ├── storage/
    │   ├── almacenamiento.h
    │   └── almacenamiento.cpp  # initSD(), writeRecord(), readCSV()
    ├── display/
    │   ├── pantalla.h
    │   └── pantalla.cpp        # showMessage(), beepExito(), beepError()
    └── utils/
        ├── constantes.h        # Pines, timeouts, límites
        └── rtc_helper.h/.cpp   # getTimestamp(), syncRTC()
```

## Mapa de memoria estimado (ATmega2560)

| Sección         | Disponible | Uso estimado |
|-----------------|------------|--------------|
| Flash (programa)| 256 KB     | ~80-120 KB   |
| SRAM (runtime)  | 8 KB       | ~4-6 KB      |
| EEPROM          | 4 KB       | No usado     |

### Variables estáticas críticas

```cpp
// Nunca usar String de Arduino — usar char[]
char timestamp[20];        // "2026-06-05,08:30:00\0"
char csvLine[40];          // "ID,Fecha,Hora\0"
uint8_t fingerprintID;     // ID del AS608 (0-127)
```

## Direcciones I2C

| Dispositivo | Dirección | Notas                        |
|-------------|-----------|------------------------------|
| SSD1306     | 0x3C      | Dirección por defecto        |
| DS3231      | 0x68      | Fija en hardware             |

⚠️ Ambos en el mismo bus I2C (SDA=20, SCL=21 en Mega) — sin conflicto por direcciones distintas.

## Formato de datos

### CSV en microSD (`ASIST.CSV`)
```
ID,Fecha,Hora
1,2026-06-05,08:30:00
2,2026-06-05,08:31:12
1,2026-06-06,08:29:45
```

- Separador: coma (`,`)
- Fecha: `YYYY-MM-DD`
- Hora: `HH:MM:SS`
- Sin BOM, sin espacios, encoding ASCII
- Header solo en la primera línea

### Detección de duplicado

Regla: mismo `ID` no puede registrarse dos veces en la misma `Fecha`.  
Implementación: comparar ID + Fecha antes de escribir nuevo registro.
