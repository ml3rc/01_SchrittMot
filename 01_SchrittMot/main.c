/*
 * main.c
 * Version: 1.0
 * Created: 2/23/2026 8:17:20 AM
 * Last edit: See git
 * Author: maell -> See git
 * Project: StepMotor
 */

/********** Defines before Includes **********/
#define F_CPU 16000000UL // CPU frequency for delay (must be before includes)

/********** Includes **********/
#include <xc.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>

/********** Defines **********/

// Direction buttons
#define B_DIRECTION_CLOCKWISE      PA0
#define B_DIRECTION_ANTICLOCKWISE  PA1

// Speed buttons
#define B_SPEEDUP    PA7
#define B_SPEEDDOWN  PA6

// Output port
#define OUT PORTC // first 4 bits used

// Max, min and default speed (delay)
#define MIN_DELAY  3.3/LOOP_DELAY    // 3.33 ms
#define MAX_DELAY  500/LOOP_DELAY    // 500 ms
#define DEF_DELAY  50/LOOP_DELAY     // 20 steps per second default

// Speed change parameters
#define SPEED_CHANGE_PERCENTAGE 0.2   // percentage change for lower speeds
#define SPEED_CHANGE_MIN        0.2   // absolute change for higher speeds

// FSM loop delay
#define LOOP_DELAY 0.1 // 0.1 ms (must be small enough to allow 3.33 ms timing)

/********** Typedefs **********/

// FSM states
typedef enum {
    CLOCKWISE,
    ANTICLOCKWISE,
    NONE
} states_t;

/********** Constants **********/

const uint8_t STEPS[] = {
    0b00001001,
    0b00000001,
    0b00000011,
    0b00000010,
    0b00000110,
    0b00000100,
    0b00001100,
    0b00001000
};

const uint8_t NSTEPS = sizeof(STEPS) / sizeof(STEPS[0]); // number of steps

/********** Helper Functions **********/

/*
 * Reads a pin from a port register
 * pinReg: pointer to register
 * pin: pin number (0–7)
 * returns: true if pin is HIGH
 */
bool readPin(volatile uint8_t *pinReg, uint8_t pin) {
    return (bool)(((*pinReg) & (1 << pin)) ? 1 : 0);
}

/*
 * Initialize IO ports (PORTA and PORTC)
 */
void InitIO() {

    // PORTA inputs for buttons
    DDRA = 0x00;  // input
    PORTA = 0xFF; // enable pullups

    // PORTC outputs for motor driver
    DDRC = 0xFF;  // output
    PORTC = 0x00; // initialize LOW
}

/********** Main **********/

int main(void) {

    // Initialize hardware
    InitIO();

    // Current step
    uint8_t currStep = 0;

    // Previous button states
    bool prevBDirClockwise     = readPin(&PINA, B_DIRECTION_CLOCKWISE);
    bool prevBDirAnticlockwise = readPin(&PINA, B_DIRECTION_ANTICLOCKWISE);
    bool prevBSpeedUp          = readPin(&PINA, B_SPEEDUP);
    bool prevBSpeedDown        = readPin(&PINA, B_SPEEDDOWN);

    // FSM variables
    states_t currState = CLOCKWISE; // default direction
    uint16_t delayCounter = 0;
    float speedDelay = DEF_DELAY;

    // Temporary variable for button reads
    bool curr;

    while (1) {

        // FSM loop delay
        _delay_ms(LOOP_DELAY);

        switch (currState) {

            case CLOCKWISE:

                // Increase step
                if (delayCounter++ >= speedDelay) {
                    currStep = (currStep + 1) % NSTEPS;
                    delayCounter = 0;
                }

                // Direction change
                curr = readPin(&PINA, B_DIRECTION_CLOCKWISE);
                if (prevBDirClockwise != curr && curr) {
                    currState = ANTICLOCKWISE;
                }
                prevBDirClockwise = curr;

                break;

            case ANTICLOCKWISE:

                // Decrease step
                if (delayCounter++ >= speedDelay) {
                    currStep = (currStep + NSTEPS - 1) % NSTEPS;
                    delayCounter = 0;
                }

                // Direction change
                curr = readPin(&PINA, B_DIRECTION_ANTICLOCKWISE);
                if (prevBDirAnticlockwise != curr && curr) {
                    currState = CLOCKWISE;
                }
                prevBDirAnticlockwise = curr;

                break;

            case NONE:

                // Recovery state
                currState = CLOCKWISE;
                break;
        }

        /********** Code executed every loop **********/

        // Speed up
        curr = readPin(&PINA, B_SPEEDUP);
        if (prevBSpeedUp != curr && curr) {

            // Reduce delay to increase speed
            speedDelay *= (1 - SPEED_CHANGE_PERCENTAGE);
            speedDelay -= SPEED_CHANGE_MIN;

            // Limit speed
            if (speedDelay < MIN_DELAY) {
                speedDelay = MIN_DELAY;
            }
        }
        prevBSpeedUp = curr;

        // Slow down
        curr = readPin(&PINA, B_SPEEDDOWN);
        if (prevBSpeedDown != curr && curr) {

            // Increase delay
            speedDelay *= (1 + SPEED_CHANGE_PERCENTAGE);
            speedDelay += SPEED_CHANGE_MIN;

            // Limit speed
            if (speedDelay > MAX_DELAY) {
                speedDelay = MAX_DELAY;
            }
        }
        prevBSpeedDown = curr;

        // Output step pattern
        OUT = STEPS[currStep];
    }
}