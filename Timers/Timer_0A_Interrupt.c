/**
 * @file Timer_0A_Interrupt.c
 *
 * @brief Source code for the Timer_0A_Interrupt driver.
 *
 * This file contains the function definitions for the Timer_0A_Interrupt driver.
 * It uses the Timer 0A module to generate periodic interrupts.
 *
 * @note Timer 0A has been configured to generate periodic interrupts every 1 ms
 * for the Timers lab.
 *
 * @note This driver assumes that the system clock's frequency is 50 MHz.
 * 
 * @note Refer to Table 2-9 (Interrupts) on pages 104 - 106 from the TM4C123G Microcontroller Datasheet
 * to view the Vector Number, Interrupt Request (IRQ) Number, and the Vector Address
 * for each peripheral.
 *
 * @author Aaron Nanas
 */

#include "Timer_0A_Interrupt.h"

// Declare pointer to the user-defined task
void (*Timer_0A_Task)(void);

void Timer_0A_Interrupt_Init(void(*task)(void))
{
	// Store the user-defined task function for use during interrupt handling
	Timer_0A_Task = task;
	
	// Set the R0 bit (Bit 0) in the RCGCTIMER register
	// to enable the clock for Timer 0A
	SYSCTL->RCGCTIMER |=  0x01;
	
	// Clear the TAEN bit (Bit 0) of the GPTMCTL register
	// to disable Timer 0A
	TIMER0->CTL &= ~0x01;
	
	// Set the bits of the GPTMCFG field (Bits 2 to 0) in the GPTMCFG register
	// 0x4 = Select the 16-bit timer configuration
	TIMER0->CFG |= 0x04;
	
	// Set the bits of the TAMR field (Bits 1 to 0) in the GPTMTAMR register
	// 0x2 = Periodic Timer Mode
	TIMER0->TAMR |= 0x02;
	
	// Clear the bits of the TAPSR field (Bits 7 to 0) in the
	// GPTMTAPR register before setting the prescale value
	TIMER0->TAPR &= ~0x000000FF;
	
	// Set the prescale value to 50 by setting the bits of the
	// TAPSR field (Bits 7 to 0) in the GPTMTAPR register
	// New timer clock frequency = (50 MHz / 50) = 1 MHz
	TIMER0->TAPR |= 50;
	
	// Set the timer interval load value by writing to the
	// TAILR field (Bits 31 to 0) in the GPTMTAILR register
	// (1 us * 1000) = 1 ms
	// Timer 0A Resolution: 1 ms
	TIMER0->TAILR = (1000 - 1);
	
	// Set the TATOCINT bit (Bit 0) to 1 in the GPTMICR register
	// The TATOCINT bit will be automatically cleared when it is set to 1
	TIMER0->ICR |= 0x01;
	
	// Enable the Timer 0A interrupt by setting the TATOIM bit (Bit 0)
	// in the GPTMIMR register
	TIMER0->IMR |= 0x01;
	
	// Set the priority level to 1 for the Timer 0A interrupt
	// In the Interrupt 16-19 Priority (PRI4) register,
	// the INTD field (Bits 31 to 29) corresponds to Interrupt Request (IRQ) 19
	// Timer 0A has an IRQ of 19
	NVIC->IPR[4] = (NVIC->IPR[4] & 0x00FFFFFF) | (1 << 29);
	
	// Enable IRQ 19 for Timer 0A by setting Bit 19 in the ISER[0] register
	NVIC->ISER[0] |= (1 << 19);
	
	// Set the TAEN bit (Bit 0) in the GPTMCTL register to enable Timer 0A
	TIMER0->CTL |= 0x01;
}

void TIMER0A_Handler(void)
{
	// Read the Timer 0A time-out interrupt flag
	if (TIMER0->MIS & 0x01)
	{
		// Execute the user-defined function
		(*Timer_0A_Task)();
		
		// Acknowledge the Timer 0A interrupt and clear it
		TIMER0->ICR |= 0x01;
	}
}
