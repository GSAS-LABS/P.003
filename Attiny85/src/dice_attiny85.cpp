#include <Arduino.h>

#include "dice.h"
#include "buttons.h"
#include "display.h"
#include "power.h"

void setup() {
    displayBegin();
    buttonsBegin();
    powerBegin();
    diceBegin();
}

void loop() {
    diceUpdate();
}
