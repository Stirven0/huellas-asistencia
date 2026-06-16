# Sistema Biométrico de Huellas Digitales — Registro de Asistencia Académica

> Solución IoT Offline para el Contexto Educativo de La Guajira  
> Hardware: Arduino Mega 2560 · Lenguaje: C++ (Arduino SDK)

## Problema que resuelve

Los métodos manuales de registro de asistencia en instituciones educativas de La Guajira
presentan tres fallas críticas:

1. **Vulnerabilidad** — suplantación de identidad estudiantil, registros falsos.
2. **Pérdida de datos** — documentos físicos sin respaldo digital confiable.
3. **Aislamiento digital** — conectividad nula o inestable en zonas rurales impide soluciones cloud.

## Solución

Sistema embebido 100% offline con autenticación biométrica, almacenamiento local en microSD
y post-procesamiento en PC mediante interfaz Python + CSV.

---

## Hardware

| Componente       | Función                                              |
|------------------|------------------------------------------------------|
| Arduino Mega 2560| Núcleo de procesamiento (MCU ATmega2560)             |
| Sensor AS608     | Captura biométrica vía UART, almacena plantillas     |
| RTC DS3231       | Reloj de tiempo real de alta precisión (I2C)         |
| Módulo microSD   | Persistencia local: ID, Fecha, Hora (CSV)            |
| Pantalla OLED SSD1306 | Retroalimentación visual (I2C)                  |
| Buzzer           | Retroalimentación sonora                             |
| Pulsador         | Selector de modo (Enrolamiento / Asistencia)         |

## Flujo de datos

```
[Usuario] → [AS608: minutiae] → [Mega: procesamiento]
                                        ↓               ↓
                              [OLED + Buzzer]    [RTC DS3231 + microSD]
                                                         ↓
                                              [Transferencia física]
                                                         ↓
                                        [CSV] → [Python PC] → [Reporte]
```

---

## Modos de operación

### Modo Enrolamiento (Docentes/Administradores)
1. Activación por pulsador (menú controlado)
2. Escaneo dual de huella en AS608
3. Sensor procesa y guarda plantilla en memoria interna
4. Asignación de ID único al estudiante

### Modo Asistencia (Estudiantes)
1. Estudiante coloca dedo en sensor
2. Arduino consulta AS608 buscando coincidencia de ID
3. Match → consulta RTC DS3231 para timestamp exacto
4. Escribe registro `ID,Fecha,Hora` en CSV de microSD + feedback OLED/Buzzer

---

## Stack técnico

- **Lenguaje:** C++ con Arduino SDK
- **IDE compatible:** Arduino IDE 2.x / PlatformIO (VSCode)
- **Post-procesamiento:** Python 3 (lectura CSV, generación de reportes)
- **Librerías principales:** ver [`docs/dependencies.md`](docs/dependencies.md)

---

## Estructura del proyecto

```
huellas-asistencia/
├── src/
│   ├── enrollment/     # Lógica de enrolamiento de huellas
│   ├── attendance/     # Lógica de marcación de asistencia
│   ├── storage/        # Escritura/lectura microSD (CSV)
│   ├── display/        # OLED SSD1306 + Buzzer
│   └── utils/          # RTC, helpers, constantes
├── lib/                # Librerías externas (vendor)
├── docs/               # Documentación técnica
├── tests/              # Pruebas por fase
└── .hermes/            # Contexto del agente Hermes
```

---

## Fases de implementación

| Fase | Semana | Objetivo                                      | Estado |
|------|--------|-----------------------------------------------|--------|
| 1    | 1      | Pruebas individuales de sensores (AS608 vía serial) | ✅ Completa |
| 2    | 2      | Integración mínima: huella + OLED + energía   | ✅ Completa |
| 3    | 3      | Persistencia offline: RTC + microSD + CSV     | ✅ Completa |
| 4    | 4      | Validación final + interfaz Python en PC      | ✅ Completa |

---

## Resultados validados (prototipo)

- ✅ 5+ usuarios registrados, 3 marcaciones por usuario
- ✅ Rechazo de falsos positivos, sin duplicados en < 1 minuto
- ✅ 100% funcionalidad offline (sin internet ni PC conectada)
- ✅ Persistencia CSV tras ciclos de apagado/encendido
