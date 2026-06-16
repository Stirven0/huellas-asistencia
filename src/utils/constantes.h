// OBSOLETO — este archivo no se incluye en la compilación.
// La fuente de verdad es src/main/constantes.h
// Conservado solo como referencia histórica.
#ifndef CONSTANTES_H
#define CONSTANTES_H

// ============================================================
// PINES DE HARDWARE
// ============================================================

// AS608 — Sensor de huellas (UART via Serial1)
#define FP_RX_PIN       19   // RX1 del Mega
#define FP_TX_PIN       18   // TX1 del Mega
// DESACTIVADO: demasiado sensible al ruido. Se usa UART polling con finger.getImage()
// #define FP_TOUCH_PIN    6    // TCH — HIGH = dedo detectado

// microSD — SPI (sin regulador — alimentar a 3.3V solamente)
#define SD_CS_PIN       53   // Chip Select
#define SD_MOSI_PIN     51   // MOSI
#define SD_MISO_PIN     50   // MISO
#define SD_SCK_PIN      52   // SCK / CLK

// OLED SSD1306 — I2C (compartido con DS3231)
#define OLED_ADDRESS    0x3C
#define OLED_WIDTH      128
#define OLED_HEIGHT     64

// DS3231 — I2C
#define RTC_ADDRESS     0x68  // Fija en hardware

// Buzzer
#define BUZZER_PIN      12

// Pulsador (INPUT_PULLUP → LOW = presionado)
#define BUTTON_PIN      3

// ============================================================
// CONFIGURACIÓN DEL SISTEMA
// ============================================================

#define MAX_FINGERPRINTS    127   // Límite del AS608
#define FINGERPRINT_TIMEOUT 5000  // ms para lectura de huella
#define DEBOUNCE_DELAY      50    // ms para pulsador
#define BUZZER_SHORT        100   // ms tono corto (éxito)
#define BUZZER_LONG         500   // ms tono largo (error)

// ============================================================
// ARCHIVO CSV
// ============================================================

#define CSV_FILENAME    "ASIST.CSV"
#define CSV_HEADER      "ID,Fecha,Hora"

// ============================================================
// MODOS DE OPERACIÓN
// ============================================================

#define MODO_ASISTENCIA     0
#define MODO_ENROLAMIENTO   1

#endif // CONSTANTES_H
