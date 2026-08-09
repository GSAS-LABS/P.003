#include <Arduino.h>
#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

// ============================================================
// ATtiny85 GPIO
// ============================================================

constexpr uint8_t PIN_DATA  = PB0;
constexpr uint8_t PIN_CLOCK = PB1;
constexpr uint8_t PIN_LATCH = PB2;

constexpr uint8_t PIN_BUTTON_TYPE = PB3;
constexpr uint8_t PIN_BUTTON_ROLL = PB4;

// ============================================================
// 74HC595
// ============================================================
//        A          0 = A
//       ---         1 = B
//    F |   | B      2 = C
//       -G-         3 = D
//    E |   | C      4 = E
//       ---         5 = F
//        D          6 = G
//                   7 = DP

constexpr uint8_t DISPLAY_0 = 0b00111111;
constexpr uint8_t DISPLAY_1 = 0b00000110;
constexpr uint8_t DISPLAY_2 = 0b01011011;
constexpr uint8_t DISPLAY_3 = 0b01001111;
constexpr uint8_t DISPLAY_4 = 0b01100110;
constexpr uint8_t DISPLAY_5 = 0b01101101;
constexpr uint8_t DISPLAY_6 = 0b01111101;
constexpr uint8_t DISPLAY_7 = 0b00000111;
constexpr uint8_t DISPLAY_8 = 0b01111111;
constexpr uint8_t DISPLAY_9 = 0b01101111;

constexpr uint8_t DISPLAY_BLANK = 0b00000000;
constexpr uint8_t SEGMENT_DP = 7;

// ============================================================
// EEPROM
// ============================================================

constexpr int EEPROM_DICE_TYPE_ADDRESS = 0;
constexpr uint8_t EEPROM_MAGIC = 0xD6;

// ============================================================
// Dice type
// ============================================================

enum class DiceType : uint8_t {
    D4,
    D6,
    D8,
    D10
};

DiceType selectedDice = DiceType::D6;
DiceType editingDice = selectedDice;

// ============================================================
// Button config
// ============================================================

constexpr unsigned long DEBOUNCE_MS = 30;
constexpr unsigned long LONG_PRESS_MS = 1500;

// ============================================================
// Roll config
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
// Selection button state
// ============================================================
bool buttonTypeLastReading = HIGH;
bool buttonTypeStableState = HIGH;
bool buttonTypeLongPressHandled = false;


unsigned long buttonTypeLastChangeTime = 0;
unsigned long buttonTypePressedTime = 0;

bool selectingDice = false;
bool diceSelectionAnimation = false;
uint8_t diceSelectionBlinkCount = 0;
bool diceSelectionDisplayOn = false;
unsigned long diceSelectionAnimationTime = 0;

constexpr unsigned long DICE_SELECTION_BLINK_MS = 150;
constexpr uint8_t DICE_SELECTION_BLINKS = 3;
// ============================================================
// Roll button state
// ============================================================

bool buttonRollLastReading = HIGH;
bool buttonRollStableState = HIGH;

unsigned long buttonRollLastChangeTime = 0;

// ============================================================
// Roll state
// ============================================================

bool rolling = false;
bool randomSeedInitialized = false;

unsigned long rollStartTime = 0;
unsigned long rollLastStepTime = 0;

uint8_t rollSequenceIndex = 0;

// ============================================================
// Functions
// ============================================================

void setup() {
    pinMode(PIN_DATA, OUTPUT);
    pinMode(PIN_CLOCK, OUTPUT);
    pinMode(PIN_LATCH, OUTPUT);

    pinMode(PIN_BUTTON_TYPE, INPUT_PULLUP);
    pinMode(PIN_BUTTON_ROLL, INPUT_PULLUP);

    digitalWrite(PIN_DATA, LOW);
    digitalWrite(PIN_CLOCK, LOW);
    digitalWrite(PIN_LATCH, HIGH);

    displayClear();
}

void loop() {
    handleTypeButton();
    handleRollButton();
    handleRoll();
    handleDiceSelectionAnimation();
}

// ============================================================
// EEPROM
// ============================================================
void saveDiceType(DiceType type) {
    EEPROM.update(
        EEPROM_DICE_TYPE_ADDRESS,
        EEPROM_MAGIC
    );

    EEPROM.update(
        EEPROM_DICE_TYPE_ADDRESS + 1,
        static_cast<uint8_t>(type)
    );
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

bool isValidDiceType(uint8_t value) {
    return value <= static_cast<uint8_t>(DiceType::D10);
}

// ============================================================
// 74HC595
// ============================================================

void shiftRegisterWrite(uint8_t value) {
    digitalWrite(PIN_LATCH, LOW);

    for (int8_t bit = 7; bit >= 0; bit--) {
        digitalWrite(PIN_CLOCK, LOW);
        digitalWrite(PIN_DATA, (value & (1 << bit)) ? HIGH : LOW);
        digitalWrite(PIN_CLOCK, HIGH);
    }

    digitalWrite(PIN_CLOCK, LOW);
    digitalWrite(PIN_LATCH, HIGH);
}


// ============================================================
// Display
// ============================================================

void displayWrite(uint8_t segments) {
    shiftRegisterWrite(segments);
}

void displayClear() {
    displayWrite(DISPLAY_BLANK);
}

void displaySegment(uint8_t segment) {
    displayWrite(1 << segment);
}

void displayNumber(uint8_t number, bool decimalPoint = false) {
    uint8_t value;

    static constexpr uint8_t segments[] = {
        DISPLAY_0,
        DISPLAY_1,
        DISPLAY_2,
        DISPLAY_3,
        DISPLAY_4,
        DISPLAY_5,
        DISPLAY_6,
        DISPLAY_7,
        DISPLAY_8,
        DISPLAY_9
    };

    if (number > 9) {
        value = DISPLAY_0;
    } else {
        value = segments[number];
    }

    if (decimalPoint) {
        value |= (1 << SEGMENT_DP);
    }

    displayWrite(value);
}

// ============================================================
// Dice Type
// ============================================================

void displayDiceType(DiceType type){
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

    displayDiceType(editingDice);
}

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
// Roll button
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

void initializeRandom() {
    const unsigned long seed = micros() ^ millis();
    randomSeed(seed);
    randomSeedInitialized = true;
}

void handleRollButton() {
    const bool reading = digitalRead(PIN_BUTTON_ROLL);

    if (reading != buttonRollLastReading) {
        buttonRollLastChangeTime = millis();
        buttonRollLastReading = reading;
    }

    if ((millis() - buttonRollLastChangeTime) < DEBOUNCE_MS) {
        return;
    }

    if (reading != buttonRollStableState) {
        buttonRollStableState = reading;
        if (buttonRollStableState == LOW && !rolling) {
            startRoll();
        }
    }
}

void handleRoll() {
    if (!rolling) {
        return;
    }

    const unsigned long now = millis();

    //End of the animation
    if (now - rollStartTime >= ROLL_ANIMATION_MS) {
        const uint8_t result = random(1, diceMaxValue(selectedDice) + 1);
        displayNumber(result);
        rolling = false;
        return;
    }

    // Next step of the animation
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
// Selection button
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

void handleTypeButton() {
    const bool reading = digitalRead(PIN_BUTTON_TYPE);

    if (reading != buttonTypeLastReading) {
        buttonTypeLastChangeTime = millis();
        buttonTypeLastReading = reading;
    }

    if ((millis() - buttonTypeLastChangeTime) < DEBOUNCE_MS) {
        return;
    }

    if (reading != buttonTypeStableState) {
        buttonTypeStableState = reading;

        if (buttonTypeStableState == LOW) {
            buttonTypePressedTime = millis();
            buttonTypeLongPressHandled = false;

            if (!selectingDice) {
                selectingDice = true;
                editingDice = selectedDice;
            }

            displayDiceType(editingDice);
        } else {
            const unsigned long pressDuration = millis() - buttonTypePressedTime;

            if (pressDuration < LONG_PRESS_MS && !buttonTypeLongPressHandled) {
                displayNextDiceType();
            }
        }
    }

    if (buttonTypeStableState == LOW && !buttonTypeLongPressHandled) {
        const unsigned long pressDuration = millis() - buttonTypePressedTime;

        if (pressDuration >= LONG_PRESS_MS) {
            buttonTypeLongPressHandled = true;
            selectedDice = editingDice;
            saveDiceType(selectedDice);

            selectingDice = false;

            startDiceSelectionAnimation();
        }
    }
}
