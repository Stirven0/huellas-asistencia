# Dependencias del Proyecto

## Librerías Arduino (Arduino Library Manager)

| Librería                              | Versión  | Fuente              | Uso                        |
|---------------------------------------|----------|---------------------|----------------------------|
| Adafruit Fingerprint Sensor Library   | ≥ 2.1.0  | Arduino Library Manager | Comunicación con AS608  |
| RTClib                                | ≥ 2.1.4  | Arduino Library Manager (Adafruit) | DS3231 RTC |
| Adafruit SSD1306                      | ≥ 2.5.7  | Arduino Library Manager | OLED SSD1306              |
| Adafruit GFX Library                  | ≥ 1.11.9 | Arduino Library Manager | Base gráfica OLED         |
| SD                                    | built-in | Arduino IDE         | Módulo microSD             |
| Wire                                  | built-in | Arduino IDE         | I2C (RTC + OLED)          |
| SPI                                   | built-in | Arduino IDE         | SPI (microSD)             |

## Instalación rápida (Arduino IDE 2.x)

1. `Tools → Manage Libraries`
2. Buscar e instalar en orden:
   - `Adafruit Fingerprint Sensor Library`
   - `RTClib` (de Adafruit)
   - `Adafruit SSD1306` (instala GFX automáticamente)

## Herramienta de compilación

- **Arduino IDE 2.x** — recomendado
- **PlatformIO** compatible (ver `platformio.ini` si se agrega)
- Board: `Arduino Mega or Mega 2560`
- Processor: `ATmega2560`
- Programmer: `AVRISP mkII` o `USBasp`

## Post-procesamiento (PC)

| Herramienta | Versión | Uso                          |
|-------------|---------|------------------------------|
| Python      | ≥ 3.10  | Lectura CSV y generación reportes |
| pandas      | ≥ 2.0   | Procesamiento de datos       |
| openpyxl    | ≥ 3.1   | Export a Excel               |

## Notas

- No usar `Adafruit_Fingerprint` versión < 2.0 — API incompatible
- `RTClib` de Adafruit, no la de JChristensen (API diferente)
- Si aparece conflicto entre `Adafruit_BusIO` y `Wire`, actualizar `Adafruit_BusIO` a ≥ 1.14
