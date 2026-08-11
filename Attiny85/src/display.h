#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

void displayBegin();
void displayClear();
void displayNumber(uint8_t number, bool decimalPoint = false);
void displaySegment(uint8_t segment);

#endif
