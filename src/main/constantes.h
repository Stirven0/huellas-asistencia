#ifndef CONSTANTES_H
#define CONSTANTES_H

// PINES DE HARDWARE
#define FP_RX_PIN       19
#define FP_TX_PIN       18
// NOTA: FP_TOUCH_PIN (Pin 6) desactivado — demasiado sensible al ruido
// La deteccion de dedo se hace via UART polling con finger.getImage()
// #define FP_TOUCH_PIN    6
#define SD_CS_PIN       53
#define SD_MOSI_PIN     51
#define SD_MISO_PIN     50
#define SD_SCK_PIN      52
#define OLED_ADDRESS    0x3C
#define OLED_WIDTH      128
#define OLED_HEIGHT     64
#define RTC_ADDRESS     0x68
#define BUZZER_PIN      12
#define BUTTON_PIN      3

// CONFIGURACION
#define MAX_FINGERPRINTS    127
#define FINGERPRINT_TIMEOUT 5000
#define DEBOUNCE_DELAY      50
#define BUZZER_SHORT        100
#define BUZZER_LONG         500
#define BUZZER_DOUBLE       200
#define NOMBRE_MAX          30
#define LINEA_MAX           50
#define FECHA_MAX           12
#define HORA_MAX            10
#define MSG_MAX             22
#define BOTON_COOLDOWN_MS   300

// ARCHIVOS CSV
#define ESTUDIANTES_CSV  "ESTUDIANTES.CSV"
#define ASIST_CSV        "ASIST.CSV"
#define CSV_HEADER       "ID,Fecha,Hora"

// MODOS DE OPERACION
#define MODO_ASISTENCIA     0
#define MODO_ENROLAMIENTO   1
#define MODO_CORRECCION     2
#define MODO_FORMATEAR      3
#define MODOS_COUNT         4

#endif
