#include "dice.h"

#include <Arduino.h>

#include "display.h"
#include "buttons.h"


// ============================================================
// Dice state
// ============================================================

DiceType selectedDice = DiceType::D6;
DiceType editingDice = DiceType::D6;

// ============================================================
// Roll configuration
// ============================================================

constexpr unsigned long ROLL_ANIMATION_MS = 3000;
constexpr unsigned long ROLL_STEP_MS = 80;

constexpr uint8_t ROLL_SEQUENCE[] = {
    2, // C
    3, // D
    4, // E
    6, // G
    1, // B
    0, // A
    5, // F
    6  // G
};

// ============================================================
// Roll state
// ============================================================

bool rolling = false;
bool randomSeedInitialized = false;

unsigned long rollStartTime = 0;
unsigned long rollLastStepTime = 0;

uint8_t rollSequenceIndex = 0;

// ============================================================
// Dice selection
// ============================================================

bool selectingDice = false;

// ============================================================
// Selection animation
// ============================================================

bool diceSelectionAnimation = false;

uint8_t diceSelectionBlinkCount = 0;

bool diceSelectionDisplayOn = false;

unsigned long diceSelectionAnimationTime = 0;

constexpr unsigned long DICE_SELECTION_BLINK_MS = 150;
constexpr uint8_t DICE_SELECTION_BLINKS = 3;

// ============================================================
// Dice display
// ============================================================

void displayDiceType(DiceType type) {
    switch (type) {
        case DiceType::D4:
            displayNumber(4, true);
            break;
        case DiceType::D6:
            displayNumber(6, true);
            break;
        case DiceType::D8:
            displayNumber(8, true);
            break;
        case DiceType::D10:
            displayNumber(0, true);
            break;
    }
}

// ============================================================
// Dice selection
// ============================================================

void displayNextDiceType() {
    switch (editingDice) {
        case DiceType::D4:
            editingDice = DiceType::D6;
            break;
        case DiceType::D6:
            editingDice = DiceType::D8;
            break;
        case DiceType::D8:
            editingDice = DiceType::D10;
            break;
        case DiceType::D10:
            editingDice = DiceType::D4;
            break;
    }
    displayClear();
    displayDiceType(editingDice);
}

// ============================================================
// Dice maximum value
// ============================================================

uint8_t diceMaxValue(DiceType type) {
    switch (type) {
        case DiceType::D4:
            return 4;
        case DiceType::D6:
            return 6;
        case DiceType::D8:
            return 8;
        case DiceType::D10:
            return 10;
    }
    return 6;
}

// ============================================================
// Random
// ============================================================

void initializeRandom() {
    const unsigned long seed = micros() ^ millis();
    randomSeed(seed);
    randomSeedInitialized = true;
}

// ============================================================
// Roll
// ============================================================

void startRoll() {
    if (!randomSeedInitialized) {
        initializeRandom();
    }

    displayClear();

    rolling = true;

    rollStartTime = millis();
    rollLastStepTime = millis();

    rollSequenceIndex = 0;
}

void handleRoll() {
    if (!rolling) {
        return;
    }

    const unsigned long now = millis();

    // End of animation
    if (now - rollStartTime >= ROLL_ANIMATION_MS) {
        const uint8_t result = random(1, diceMaxValue(selectedDice) + 1);

        displayNumber(result);
        rolling = false;
        return;
    }

    // Animation step
    if (now - rollLastStepTime >= ROLL_STEP_MS) {
        rollLastStepTime = now;

        displaySegment(ROLL_SEQUENCE[rollSequenceIndex]);

        rollSequenceIndex++;

        if (rollSequenceIndex >= sizeof(ROLL_SEQUENCE)) {
            rollSequenceIndex = 0;
        }
    }
}

// ============================================================
// Selection animation
// ============================================================

void startDiceSelectionAnimation() {
    diceSelectionAnimation = true;
    diceSelectionBlinkCount = 0;
    diceSelectionDisplayOn = true;
    diceSelectionAnimationTime = millis();

    displayDiceType(selectedDice);
}

void handleDiceSelectionAnimation() {
    if (!diceSelectionAnimation) {
        return;
    }

    const unsigned long now = millis();

    if (now - diceSelectionAnimationTime < DICE_SELECTION_BLINK_MS) {
        return;
    }

    diceSelectionAnimationTime = now;

    if (diceSelectionDisplayOn) {
        displayClear();
        diceSelectionDisplayOn = false;
        diceSelectionBlinkCount++;

        if (diceSelectionBlinkCount >= DICE_SELECTION_BLINKS) {
            diceSelectionAnimation = false;
        }
    } else {
        displayDiceType(selectedDice);
        diceSelectionDisplayOn = true;
    }
}

// ============================================================
// Button events
// ============================================================

void handleButtonEvent(ButtonEvent event) {
    switch (event) {
        case ButtonEvent::TYPE_PRESSED:
            if (!rolling && !selectingDice) {
                selectingDice = true;
                editingDice = selectedDice;
                 displayDiceType(editingDice);
            }
            break;
        case ButtonEvent::TYPE_SHORT_PRESS:
            if (selectingDice && !rolling) {
                displayNextDiceType();
            }
            break;
        case ButtonEvent::TYPE_LONG_PRESS:
            if (selectingDice && !rolling) {
                selectedDice = editingDice;
                selectingDice = false;
                startDiceSelectionAnimation();
            }
            break;
        case ButtonEvent::ROLL_PRESSED:
            if (!rolling && !selectingDice) {
                startRoll();
            }
            break;
        case ButtonEvent::NONE:
            break;
    }
}

// ============================================================
// Initialization
// ============================================================

void diceBegin() {
    selectedDice = DiceType::D6;
    editingDice = selectedDice;

    displayDiceType(selectedDice);
}

// ============================================================
// Main update
// ============================================================

void diceUpdate() {
    const ButtonEvent event = buttonsUpdate();
    handleButtonEvent(event);
    handleRoll();
    handleDiceSelectionAnimation();
}

// ============================================================
// Public getter
// ============================================================

DiceType getSelectedDice() {
    return selectedDice;
}
