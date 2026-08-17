#include "dice.h"

#include <Arduino.h>
#include <EEPROM.h>

#include "display.h"
#include "buttons.h"

// ============================================================
// EEPROM
// ============================================================

constexpr int EEPROM_DICE_TYPE_ADDRESS = 0;
constexpr uint8_t EEPROM_MAGIC = 0xD6;

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
bool diceSelectionDisplayOn = false;

uint8_t diceSelectionBlinkCount = 0;

unsigned long diceSelectionAnimationTime = 0;

constexpr unsigned long DICE_SELECTION_BLINK_MS = 150;
constexpr uint8_t DICE_SELECTION_BLINKS = 3;

// ============================================================
// Edit mode animation
// ============================================================

bool diceEditAnimation = false;
bool diceEditDisplayOn = true;

unsigned long diceEditAnimationTime = 0;

constexpr unsigned long DICE_EDIT_BLINK_MS = 500;

// ============================================================
// EEPROM
// ============================================================

bool isValidDiceType(uint8_t value) {
    return value <= static_cast<uint8_t>(DiceType::D10);
}

void saveDiceType(DiceType type) {
    EEPROM.update(EEPROM_DICE_TYPE_ADDRESS, EEPROM_MAGIC);

    EEPROM.update(EEPROM_DICE_TYPE_ADDRESS + 1, static_cast<uint8_t>(type));
}

void loadDiceType() {
    const uint8_t magic = EEPROM.read(EEPROM_DICE_TYPE_ADDRESS);
    const uint8_t value = EEPROM.read(EEPROM_DICE_TYPE_ADDRESS + 1);

    if (magic == EEPROM_MAGIC && isValidDiceType(value)) {
        selectedDice = static_cast<DiceType>(value);
    } else {
        selectedDice = DiceType::D6;
        saveDiceType(selectedDice);
    }

    editingDice = selectedDice;
}

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
    diceEditDisplayOn = true;
    diceEditAnimationTime = millis();
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

    rolling = true;

    displayClear();

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
        const uint8_t result = random(
            1,
            diceMaxValue(selectedDice) + 1
        );

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
// Dice Animation
// ============================================================

void startDiceSelectionAnimation() {
    diceSelectionAnimation = true;
    diceSelectionBlinkCount = 0;
    diceSelectionDisplayOn = true;
    diceSelectionAnimationTime = millis();

    displayDiceType(selectedDice);
}

void startDiceEditAnimation() {
    diceEditAnimation = true;
    diceEditDisplayOn = true;
    diceEditAnimationTime = millis();

    displayDiceType(editingDice);
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

void handleDiceEditAnimation() {
    if (!diceEditAnimation) {
        return;
    }

    const unsigned long now = millis();

    if (now - diceEditAnimationTime < DICE_EDIT_BLINK_MS) {
        return;
    }

    diceEditAnimationTime = now;

    if (diceEditDisplayOn) {
        displayClear();
        diceEditDisplayOn = false;
    } else {
        displayDiceType(editingDice);
        diceEditDisplayOn = true;
    }
}

// ============================================================
// Button events
// ============================================================

void handleButtonEvent(ButtonEvent event) {
    switch (event) {
    case ButtonEvent::TYPE_SHORT_PRESS:
        if (!rolling) {
            if (!selectingDice) {
                selectingDice = true;
                editingDice = selectedDice;
                startDiceEditAnimation();
            } else {
                displayNextDiceType();
            }
        }
        break;

    case ButtonEvent::TYPE_LONG_PRESS:
        if (selectingDice && !rolling) {
            selectedDice = editingDice;
            saveDiceType(selectedDice);

            selectingDice = false;
            diceEditAnimation = false;

            startDiceSelectionAnimation();
        }
        break;

    case ButtonEvent::ROLL_SHORT_PRESS:
        if (!rolling && !selectingDice) {
            startRoll();
        }
        break;

    case ButtonEvent::ROLL_LONG_PRESS:
        if (!rolling && !selectingDice) {
            displayClear();
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
    loadDiceType();
    displayDiceType(selectedDice);
}

// ============================================================
// Main update
// ============================================================

void diceUpdate() {
    const ButtonEvent event = buttonsUpdate();
    handleButtonEvent(event);
    handleRoll();
    handleDiceEditAnimation();
    handleDiceSelectionAnimation();
}

// ============================================================
// Public getter
// ============================================================

DiceType getSelectedDice() {
    return selectedDice;
}
