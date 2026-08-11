#ifndef DICE_H
#define DICE_H

#include <Arduino.h>

enum class DiceType : uint8_t {
    D4,
    D6,
    D8,
    D10
};

void diceBegin();
void diceUpdate();

DiceType getSelectedDice();

#endif
