# .hermes/skills/project-context.md
# Skill: Cargar contexto del proyecto biométrico

## Descripción
Carga automáticamente el contexto completo del proyecto al inicio de cada sesión.

## Uso
Al iniciar Hermes en este directorio, ejecutar:
```
@PROJECT.md @docs/architecture.md @docs/phases.md @.hermes/rules.md
```

## Prompt de inicio sugerido

```
Estamos trabajando en un sistema biométrico de huellas digitales para registro de asistencia académica en La Guajira, Colombia. Hardware: Arduino Mega 2560, sensor AS608, RTC DS3231, microSD, OLED SSD1306, buzzer y pulsador. Lenguaje: C++ con Arduino SDK. El sistema es 100% offline. Actualmente estamos en Fase 1 (pruebas individuales de sensores). Aplica las reglas en .hermes/rules.md para todo el código que generes.
```

## Comandos frecuentes en este proyecto

| Acción                        | Comando Hermes                              |
|-------------------------------|---------------------------------------------|
| Cargar contexto completo      | `@PROJECT.md @.hermes/rules.md`             |
| Ver estado de fases           | `@docs/phases.md`                           |
| Ver arquitectura              | `@docs/architecture.md`                     |
| Ver dependencias              | `@docs/dependencies.md`                     |
| Trabajar en módulo específico | `@PROJECT.md @src/enrollment/enrolamiento.h`|
