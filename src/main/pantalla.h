#ifndef PANTALLA_H
#define PANTALLA_H

#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

bool pantallaInit();
bool pantallaPresente();
void pantallaMsg(const char* l1, const char* l2, const char* l3);
void beepExito();
void beepError();
void alertaDoble();

#endif
