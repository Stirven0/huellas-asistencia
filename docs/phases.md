# Fases de Implementación

## Estado actual: FASE 1

---

## Fase 1 — Pruebas Individuales (Semana 1)
**Estado: 🔄 EN CURSO**

### Objetivos
- [ ] Validación unitaria del sensor AS608 vía Serial Monitor
- [ ] Prueba de enrolamiento de 1 huella
- [ ] Verificación de búsqueda/match de huella enrollada
- [ ] Prueba de lectura de RTC DS3231 (fecha y hora correctas)
- [ ] Prueba de escritura/lectura en microSD (archivo de prueba)
- [ ] Prueba de pantalla OLED (texto y iconos básicos)
- [ ] Prueba de buzzer (tonos de éxito y error)
- [ ] Prueba de pulsador (detección de flanco)

### Entregables
- `tests/test_as608.ino` — sketch de prueba aislada del sensor
- `tests/test_rtc.ino` — sketch de prueba del RTC
- `tests/test_sd.ino` — sketch de prueba de microSD
- `tests/test_oled.ino` — sketch de prueba de pantalla

---

## Fase 2 — Integración Mínima (Semana 2)
**Estado: ⏳ PENDIENTE**

### Objetivos
- [ ] Conexión de hardware base completo
- [ ] Lectura estable de huellas con feedback OLED
- [ ] Gestión básica de energía
- [ ] Selector de modo por pulsador funcional

---

## Fase 3 — Persistencia y Offline (Semana 3)
**Estado: ⏳ PENDIENTE**

### Objetivos
- [ ] Integración RTC DS3231 con timestamp en CSV
- [ ] Módulo microSD escribiendo ASIST.CSV correctamente
- [ ] **Prueba crítica:** corte de energía — verificar que hora y datos persisten
- [ ] Generación de archivo CSV con formato correcto

---

## Fase 4 — Validación Final e Interfaz (Semana 4)
**Estado: ⏳ PENDIENTE**

### Objetivos
- [ ] Construcción de maqueta física completa
- [ ] Pruebas de estrés negativo:
  - [ ] Dedo incorrecto → rechazo correcto
  - [ ] Doble marcación → bloqueo de duplicado
  - [ ] Dedo no enrolado → mensaje de error
- [ ] Interfaz Python en PC para lectura de CSV
- [ ] Generación de reporte final de asistencia

---

## Registro de sesiones

| Fecha      | Fase | Logros                          | Pendiente            |
|------------|------|---------------------------------|----------------------|
| 2026-06-05 | 1    | Inicio del proyecto, estructura | Pruebas de hardware  |
