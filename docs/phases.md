# Fases de Implementación

## Estado actual: FASE 2 — COMPLETA

---

## Fase 1 — Pruebas Individuales
**Estado: ✅ COMPLETA**

### Entregables
- `tests/test_as608.ino`
- `tests/test_rtc.ino`
- `tests/test_sd.ino`
- `tests/test_oled.ino`

---

## Fase 2 — Integración + Refactorización
**Estado: ✅ COMPLETA**

### Logros
- [x] Firmware integrado en `src/main/` (flat, un módulo por archivo)
- [x] Detección en cascada de periféricos (OLED → RTC → AS608 → SD)
- [x] Monitoreo continuo cada 3s con flags de alerta
- [x] 4 modos: ASISTENCIA, ENROLAR, CORREGIR, FORMATEAR
- [x] Doble confirmación de 3s para formateo
- [x] Sistema de modos basado en array de handlers (`modos.h/cpp`)
- [x] Verificador de periféricos genérico (`verificador.h/cpp`)
- [x] Buzzer extraído a módulo propio (`buzzer.h/cpp`)
- [x] SD: `formatearCSVs()` extraído, `sdPresente()` unificado
- [x] AS608: `capturarHuella()` + `limpiarHuellas()` extraídos
- [x] Constantes para tamaños de buffer (`FECHA_MAX`, `HORA_MAX`, `MSG_MAX`)
- [x] Compilación limpia con arduino-cli
- [x] Flash: ~35KB (14%), RAM: ~2.3KB (28%)
