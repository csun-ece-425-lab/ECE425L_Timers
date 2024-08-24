/**
 * @file Buzzer.c
 *
 * @brief Source code for the Buzzer driver.
 *
 * This file contains the function definitions for the Buzzer driver.
 * It interfaces with the following:
 *	- DMT-1206 Magnetic Buzzer
 *
 * To verify the pinout of the user LED, refer to the Tiva C Series TM4C123G LaunchPad User's Guide
 * Link: https://www.ti.com/lit/pdf/spmu296
 *
 * @author Aaron Nanas
 */
 
#include "Buzzer.h"

// Constant definitions for the buzzer
const uint8_t BUZZER_OFF 		= 0x00;
const uint8_t BUZZER_ON			= 0x10;

// Constant definitions for musical notes
const double C4_NOTE = 261.6;
const double D4_NOTE = 293.7;
const double E4_NOTE = 329.6;
const double F4_NOTE = 349.2;
const double G4_NOTE = 392.0;
const double A4_NOTE = 440.0;
const double B4_NOTE = 493.0;

const double C5_NOTE = 523.2;

void Buzzer_Init(void)
{
	// Enable the clock to Port C
	SYSCTL->RCGCGPIO |= 0x04;
	
	// Set PC4 as an output GPIO pin
	GPIOC->DIR |= 0x10;
	
	// Configure PC4 to function as a GPIO pin
	GPIOC->AFSEL &= ~0x10;
	
	// Enable digital functionality for PC4
	GPIOC->DEN |= 0x10;
}
 
void Buzzer_Output(uint8_t buzzer_value)
{
	// Set the output of the buzzer
	GPIOC->DATA = (GPIOC->DATA & 0xEF) | buzzer_value;
}

void Play_Note(double note, unsigned int duration)
{
	// Calculate the period of the note in microseconds
	int period_us = (int)(((double)1/note) * ((double)1000000));
	
	// Calculate the half period of the note in microseconds
	int half_period_us = period_us / 2;
	
	// Generate a square wave for the specified duration
	for (unsigned int i = 0; i < duration; i++)
	{
		Buzzer_Output(BUZZER_ON);
		SysTick_Delay1us(half_period_us);
		Buzzer_Output(BUZZER_OFF);
		SysTick_Delay1us(half_period_us);
	}
}
