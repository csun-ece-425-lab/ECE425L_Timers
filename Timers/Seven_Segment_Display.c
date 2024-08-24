/**
 * @file Seven_Segment_Display.c
 *
 * @brief Source code for the Seven_Segment_Display driver.
 *
 * This file contains the function definitions for the Seven_Segment_Display driver.
 * It interfaces with the Seven-Segment Display module on the EduBase board.
 *
 * @note Assumes that a 16 MHz clock is used.
 *
 * @author Aaron Nanas
 */
 
#include "Seven_Segment_Display.h"

// Values used to represent numbers on the Seven-Segment Display module
const uint8_t number_pattern[16] =
{
	0xC0, // 0
	0xF9, // 1
	0xA4, // 2
	0xB0, // 3
	0x99, // 4
	0x92, // 5
	0x82, // 6
	0xF8, // 7
	0x80, // 8
	0x98, // 9
	0x88, // A
	0x83, // B
	0xC6, // C
	0xA1, // D
	0x86, // E
	0x8E  // F
};

void Seven_Segment_Display_Init(void)
{
	// Enable the clock to Port B (Bit 1)
	SYSCTL->RCGCGPIO |= 0x02;

	// Enable the clock to Port C (Bit 2)
	SYSCTL->RCGCGPIO |= 0x04;

	// Enable the clock to SSI2 (Bit 2)
	SYSCTL->RCGCSSI |= 0x04;

	// Configure PB4 (SSI2 CLK) and PB7 (SSI2 TX Data) to use alternate function
	GPIOB->AFSEL |= 0x90;

	// Clear functions for PB4 (SSI2 CLK) and PB7 (SSI2 TX Data)
	GPIOB->PCTL &= ~0xF00F0000;

	// Enable SSI2 function for PB4 (SSI2 CLK) and PB7 (SSI2 TX Data)
	GPIOB->PCTL |= 0x20020000;

	// Enable digital functionality for PB4 and PB7
	GPIOB->DEN |= 0x90;

	// Set PC7 as an output GPIO pin for SSI2 Slave Select (SSI2 SS)
	// Note: Slave Select pin is active low
	GPIOC->DIR |= 0x80;

	// Configure PC7 (SSI2 SS) to function as a GPIO pin
	GPIOC->AFSEL &= ~0x80;

	// Enable digital functionality for PC7 (SSI2 SS)
	GPIOC->DEN |= 0x80;

	// Initialize the output of PC7 (SSI2 SS) to high
	GPIOC->DATA |= 0x80;

	// Disable SSI2 during configuration
	SSI2->CR1 = 0;
	
	// Use system clock as the clock source
	SSI2->CC = 5;

	// Assumes that a 16 MHz clock is used
	// Set the prescale value to 16
	// New clock frequency = (16 MHz / 16) = 1 MHz
	SSI2->CPSR = 16;

	// Select 8-bit data format (DSS = 0x07)
	// Use Freescale SPI Frame Format (FRF = 0)
	// Select steady state low value on SCLK pin (Clock Polarity, SPO = 0)
	// Capture data on the first clock edge transition (Clock Phase, SPH = 0)
	// Set Serial Clock Rate to to 0 (SCR = 0)
	SSI2->CR0 = 0x0007;

	// Set the Synchronous Serial Enable bit (SSE)
	// Enable SSI in master mode (MS = 0)
	SSI2->CR1 |= 0x02;
}

void SSI2_Write(uint8_t data)
{
	// Assert the slave select pin by clearing Bit 7
	// of the DATA register for Port C
	GPIOC->DATA &= ~0x80;

	// Write the data to the SSI Data Register (SSIDR)
	SSI2->DR = data;

	// Wait until data transmission is done by checking
	// the BSY bit of the SSI Status Register (SSISR)
	while (SSI2->SR & 0x10);

	// Deassert the slave select pin by setting Bit 7
	// of the DATA register for Port C
	GPIOC->DATA |= 0x80;
}

int Count_Digits(int value)
{
	// Initialize the digit counter
	int num_digits = 0;

	// Iterate until the value becomes zero
	while (value != 0)
	{
		// Divide the value by 10
		value = value / 10;
		
		// Increment the digit counter
		num_digits++;
	}
	
	// Return the number of digits
	return num_digits;
}

void Seven_Segment_Display(int count_value)
{
	// If the count value is zero, then display "0" on the seven-segment display
	if (count_value == 0)
	{
		// Send the command to write "0" on the seven-segment display
		SSI2_Write(number_pattern[0]);
		
		// Send the command to write "0" as the first digit on the seven-segment display
		SSI2_Write(1);
		
		// Exit the function
		return;
	}

	// Count the number of digits in count_value
	int num_digits = Count_Digits(count_value);
	
	// Initialize a local variable to store the extracted digit
	int digit = 0;

	// Iterate through each digit in count_value
	for (int i = 1; i < (num_digits * 2); i = i * 2)
	{
		// Get the least significant digit
		digit = count_value % 10;
		
		// Remove the least significant digit from count_value by dividing by 10
		count_value = count_value / 10;
		
		// Send the command to write the extracted digit's pattern on the seven-segment display
		SSI2_Write(number_pattern[digit]);
		
		// Send the command to write each digit in the correct place on the seven-segment display
		SSI2_Write(i);
		
		// Add a short delay in order to show all digits on the seven-segment display
		SysTick_Delay1ms(1);
	}
}
