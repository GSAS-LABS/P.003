#include "display.h"

// ============================================================
// ATtiny85 GPIO
// ============================================================

constexpr uint8_t PIN_DATA  = PB0;
constexpr uint8_t PIN_CLOCK = PB1;
constexpr uint8_t PIN_LATCH = PB2;

// ============================================================
// 74HC595 + 7-segment display
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

void displayBegin() {
    pinMode(PIN_DATA, OUTPUT);
    pinMode(PIN_CLOCK, OUTPUT);
    pinMode(PIN_LATCH, OUTPUT);

    digitalWrite(PIN_DATA, LOW);
    digitalWrite(PIN_CLOCK, LOW);
    digitalWrite(PIN_LATCH, HIGH);

    displayClear();
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
// 7 - Segment Display
// ============================================================

void displayWrite(uint8_t segments) {
    shiftRegisterWrite(segments);
}

void displayClear() {
    displayWrite(DISPLAY_BLANK);
}

void displaySegment(uint8_t segment) {
    if (segment > 7) {
        return;
    }

    displayWrite(1 << segment);
}

void displayNumber(uint8_t number, bool decimalPoint) {
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
