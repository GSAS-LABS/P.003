#include "buttons.h"

// ============================================================
// ATtiny85 GPIO
// ============================================================

constexpr uint8_t PIN_BUTTON_TYPE = PB3;
constexpr uint8_t PIN_BUTTON_ROLL = PB4;

// ============================================================
// Button configuration
// ============================================================

constexpr unsigned long DEBOUNCE_MS = 30;
constexpr unsigned long LONG_PRESS_MS = 1500;

// ============================================================
// TYPE button state
// ============================================================

bool buttonTypeLastReading = HIGH;
bool buttonTypeStableState = HIGH;

unsigned long buttonTypeLastChangeTime = 0;
unsigned long buttonTypePressedTime = 0;

bool buttonTypeLongPressHandled = false;

// ============================================================
// ROLL button state
// ============================================================

bool buttonRollLastReading = HIGH;
bool buttonRollStableState = HIGH;

unsigned long buttonRollLastChangeTime = 0;
unsigned long buttonRollPressedTime = 0;

bool buttonRollLongPressHandled = false;
// ============================================================
// Initialization
// ============================================================

void buttonsBegin() {
    pinMode(PIN_BUTTON_TYPE, INPUT_PULLUP);
    pinMode(PIN_BUTTON_ROLL, INPUT_PULLUP);
}

void buttonsPrepareForSleep() {
    buttonTypeLastReading = digitalRead(PIN_BUTTON_TYPE);
    buttonTypeStableState = buttonTypeLastReading;
    buttonTypeLastChangeTime = millis();
    buttonTypePressedTime = 0;
    buttonTypeLongPressHandled = false;

    buttonRollLastReading = digitalRead(PIN_BUTTON_ROLL);
    buttonRollStableState = buttonRollLastReading;
    buttonRollLastChangeTime = millis();
    buttonRollPressedTime = 0;
    buttonRollLongPressHandled = false;
}

// ============================================================
// TYPE button
// ============================================================

ButtonEvent handleTypeButton() {
    const bool reading = digitalRead(PIN_BUTTON_TYPE);

    // Detect reading change
    if (reading != buttonTypeLastReading) {
        buttonTypeLastChangeTime = millis();
        buttonTypeLastReading = reading;
    }

    // Wait for debounce
    if ((millis() - buttonTypeLastChangeTime) < DEBOUNCE_MS) {
        return ButtonEvent::NONE;
    }

    // Stable state changed
    if (reading != buttonTypeStableState) {
        buttonTypeStableState = reading;

        // Button pressed
        if (buttonTypeStableState == LOW) {
            buttonTypePressedTime = millis();
            buttonTypeLongPressHandled = false;

            return ButtonEvent::NONE;
        }

        // Button released
        const unsigned long pressDuration = millis() - buttonTypePressedTime;

        if (pressDuration < LONG_PRESS_MS &&
            !buttonTypeLongPressHandled) {
            return ButtonEvent::TYPE_SHORT_PRESS;
        }
    }

    // Long press
    if (buttonTypeStableState == LOW &&
        !buttonTypeLongPressHandled) {

        const unsigned long pressDuration = millis() - buttonTypePressedTime;

        if (pressDuration >= LONG_PRESS_MS) {
            buttonTypeLongPressHandled = true;

            return ButtonEvent::TYPE_LONG_PRESS;
        }
    }

    return ButtonEvent::NONE;
}

// ============================================================
// ROLL button
// ============================================================

ButtonEvent handleRollButton() {
    const bool reading = digitalRead(PIN_BUTTON_ROLL);

    // Detect reading change
    if (reading != buttonRollLastReading) {
        buttonRollLastChangeTime = millis();
        buttonRollLastReading = reading;
    }

    // Wait for debounce
    if ((millis() - buttonRollLastChangeTime) < DEBOUNCE_MS) {
        return ButtonEvent::NONE;
    }

    // Stable state changed
    if (reading != buttonRollStableState) {
        buttonRollStableState = reading;

        // Button pressed
        if (buttonRollStableState == LOW) {
            buttonRollPressedTime = millis();
            buttonRollLongPressHandled = false;

            return ButtonEvent::NONE;
        }

        // Button released
        const unsigned long pressDuration = millis() - buttonRollPressedTime;

        if (pressDuration < LONG_PRESS_MS &&
            !buttonRollLongPressHandled) {
            return ButtonEvent::ROLL_SHORT_PRESS;
        }
    }

    // Long press
    if (buttonRollStableState == LOW &&
        !buttonRollLongPressHandled) {

        const unsigned long pressDuration = millis() - buttonRollPressedTime;

        if (pressDuration >= LONG_PRESS_MS) {
            buttonRollLongPressHandled = true;

            return ButtonEvent::ROLL_LONG_PRESS;
        }
    }

    return ButtonEvent::NONE;
}

// ============================================================
// Update
// ============================================================

ButtonEvent buttonsUpdate() {
    ButtonEvent event;

    // TYPE button
    event = handleTypeButton();

    if (event != ButtonEvent::NONE) {
        return event;
    }

    // ROLL button
    event = handleRollButton();

    if (event != ButtonEvent::NONE) {
        return event;
    }

    return ButtonEvent::NONE;
}
