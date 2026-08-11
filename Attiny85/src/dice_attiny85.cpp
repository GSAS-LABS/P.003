#include <Arduino.h>

#include "dice.h"
#include "buttons.h"
#include "display.h"

void setup() {
    displayBegin();
    buttonsBegin();
    diceBegin();
}

void loop() {
    diceUpdate();
}
