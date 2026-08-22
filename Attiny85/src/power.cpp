#include "power.h"

#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "buttons.h"

// ============================================================
// ATtiny85 GPIO
// ============================================================

constexpr uint8_t PIN_BUTTON_TYPE = PB3;
constexpr uint8_t PIN_BUTTON_ROLL = PB4;

// ============================================================
// Wake-up interrupt
// ============================================================

ISR(PCINT0_vect) {
    // Interrupt only wakes the MCU.
}

// ============================================================
// Initialization
// ============================================================

void powerBegin() {
    // Enable Pin Change Interrupt
    GIMSK |= _BV(PCIE);

    // PB3 and PB4 can wake the MCU
    PCMSK |= _BV(PCINT3);
    PCMSK |= _BV(PCINT4);
}

// ============================================================
// Sleep
// ============================================================

void powerSleep() {
    buttonsPrepareForSleep();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    noInterrupts();

    // Clear any pending Pin Change Interrupt
    GIFR |= _BV(PCIF);

    sleep_enable();

    interrupts();

    sleep_cpu();

    sleep_disable();
}
