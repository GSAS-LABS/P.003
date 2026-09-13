#include "power.h"

#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

#include "buttons.h"

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
    // Disable the ADC since it is not used by the project
    ADCSRA &= ~_BV(ADEN);

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
    // Prepare button states before entering sleep
    buttonsPrepareForSleep();

    // Disable all unused peripherals to minimize power consumption
    power_all_disable();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    noInterrupts();

    // Clear any pending pin change interrupt
    GIFR |= _BV(PCIF);

    // Enable sleep mode
    sleep_enable();

    // Re-enable interrupts so they can wake the MCU
    interrupts();

    // Enter Power-down mode
    sleep_cpu();

    // Execution resumes here after the MCU wakes up
    sleep_disable();

    // Restore peripherals for normal operation
    power_all_enable();
}
