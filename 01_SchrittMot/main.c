/*
 * main.c
 * Version: 1.0
 * Created: 2/23/2026 8:17:20 AM
 * Last edit: See git
 * Author: maell -> See git
 * Project: StepMotor
 */ 

/********** Defines before Includes **********/
#define F_CPU 16000000UL //cpu freq for delay -> has to be beforr includes because its used for delays

/********** Includes **********/
#include <xc.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>

/********** Defines **********/
//Direction Buttons
#define B_DIRECTION_CLOCKWISE PA0
#define B_DIRECTION_ANTICLOCKWISE PA1

//Speed Buttons
#define B_SPEEDUP PA7
#define B_SPEEDDOWN PA6

//Output ports
#define OUT PORTC // first 4 bits

//Max Min and Default Speed(delay)
#define MIN_DELAY  3.3/LOOP_DELAY // 3.33 ms
#define MAX_DELAY  500/LOOP_DELAY  // 50 ms
#define DEF_DELAY 50/LOOP_DELAY	//20 steps per second as default

//Speed change button values
#define SPEED_CHANGE_PERCENTAGE 0.2 //percentage for low speeds
#define SPEED_CHANGE_MIN 0.2 //absolute value for high speeds

//FSM loop delay
#define LOOP_DELAY 0.1 //0.1ms Note: has to be tiny enougth to allow for the 3.33ms (3.4ms) delays

/********** typedefs **********/
//FSM States
typedef enum{
	CLOCKWISE, ANTICLOKWISE, NONE
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

const uint8_t NSTEPS = sizeof(STEPS) / sizeof(STEPS[0]); //len


/********** Helper Functions **********/

/***
 * This function will take in the pointer to a Port and the pin number from 0-7 and it will return the bool of that pin
 */
bool readPin(volatile uint8_t *pinReg, uint8_t pin) {
	//read reg and mask bit
	return (bool)(((*pinReg) & (1 << pin)) ? 1 : 0);
}

/***
 * This function will init the io's (PortA and PortC)
 */
void InitIO(){
	// PORTA inputs for B_DIRECTION pins
	DDRA = 0x00; //input
	PORTA = 0xFF; //pullup

	// PORTC outputs for LEDs
	DDRC = 0xFF; //output
	PORTC = 0x00; // init LOW
}

/********** Main **********/
int main(void){
	//Init
	InitIO();
	
	//Init vars
	//current step
	uint8_t currStep = 0;
	
	//button states
	bool prevBDirClockwise = readPin(&PINA, B_DIRECTION_CLOCKWISE);
	bool prevBDirAnticlockwise = readPin(&PINA, B_DIRECTION_ANTICLOCKWISE);
	bool prevBSpeedUp = readPin(&PINA, B_SPEEDUP);
	bool prevBSpeedDown = readPin(&PINA, B_SPEEDDOWN);
	
	//fsm
	states_t currState = CLOCKWISE; //default clockwise
	uint16_t delayCounter = 0;
	float speedDelay = DEF_DELAY;
	
	//general bool for button reads
	bool curr;
	
    while(1){
		//fsm loop delay
		_delay_ms(LOOP_DELAY);
		
		switch(currState){
			case CLOCKWISE:
				//increase step
				if(delayCounter++ >= speedDelay){
					currStep = (currStep + 1) % NSTEPS;
					delayCounter = 0;
				}
				
			
				//state switch on direction change
				curr = readPin(&PINA, B_DIRECTION_CLOCKWISE);
				if(prevBDirClockwise != curr && curr){
					currState = ANTICLOKWISE;
				}
				prevBDirClockwise = curr;
				
				break;
			case ANTICLOKWISE:
				//decrease step
				if(delayCounter++ >= speedDelay){
					currStep = (currStep + NSTEPS - 1) % NSTEPS;
					delayCounter = 0;
				}
				
				
				//state switch on direction change
				curr = readPin(&PINA, B_DIRECTION_ANTICLOCKWISE);
				if(prevBDirAnticlockwise != curr && curr){
					currState = CLOCKWISE;
				}
				prevBDirAnticlockwise = curr;
				
				break;
			case NONE:
				//repair state
				currState = CLOCKWISE;
				break;
		}
		// General code that need to run every time
		// speed up
		curr = readPin(&PINA, B_SPEEDUP);
		if(prevBSpeedUp != curr && curr){
			// subtract delay to speed up
			speedDelay *= (1-SPEED_CHANGE_PERCENTAGE);
			speedDelay -= SPEED_CHANGE_MIN;
			//limit speed
			if(speedDelay < MIN_DELAY){
				speedDelay = MIN_DELAY;
			}
		}
		prevBSpeedUp = curr;
		
		// speed down
		curr = readPin(&PINA, B_SPEEDDOWN);
		if(prevBSpeedDown != curr && curr){
			//add delay to slow 
			speedDelay *= (1+SPEED_CHANGE_PERCENTAGE);
			speedDelay += SPEED_CHANGE_MIN;
			//limit speed
			if(speedDelay > MAX_DELAY){
				speedDelay = MAX_DELAY;
			}
		}
		prevBSpeedDown = curr;

		// Update lower 4 bits of OUT
		OUT = STEPS[currStep];
    }
}