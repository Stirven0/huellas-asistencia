# Arquitectura Técnica

## Diagrama de flujo principal

```
[Usuario]
    │
    ▼
[AS608 — Extracción de minutiae]
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
    │   └── pantalla.cpp        # showMessage(), showSuccess(), showError()
    └── utils/
        ├── constantes.h        # Pines, timeouts, límites
        └── rtc_helper.cpp      # getTimestamp(), syncRTC()
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
