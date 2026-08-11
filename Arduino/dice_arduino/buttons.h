#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

enum class ButtonEvent : uint8_t {
    NONE = 0,
    TYPE_PRESSED,
    TYPE_SHORT_PRESS,
    TYPE_LONG_PRESS,
    ROLL_PRESSED
};

void buttonsBegin();

ButtonEvent buttonsUpdate();

#endif
