#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

enum class ButtonEvent : uint8_t {
    NONE = 0,

    TYPE_SHORT_PRESS,
    TYPE_LONG_PRESS,

    ROLL_SHORT_PRESS,
    ROLL_LONG_PRESS
};

void buttonsBegin();
void buttonsPrepareForSleep();

ButtonEvent buttonsUpdate();

#endif
