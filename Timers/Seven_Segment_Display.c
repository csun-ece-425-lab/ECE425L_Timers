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

// Values used to represent hexadecimal numbers on the Seven-Segment Display module
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
	// Enable the clock to the SSI2 module by setting the
	// R2 bit (Bit 2) in the RCGCSSI register
	SYSCTL->RCGCSSI |= 0x04;
	
	// Enable the clock to Port B by setting the
	// R1 bit (Bit 1) in the RCGCGPIO register
	SYSCTL->RCGCGPIO |= 0x02;

	// Enable the clock to Port C (Bit 2) by setting the
	// R2 bit (Bit 2) in the RCGCGPIO register
	SYSCTL->RCGCGPIO |= 0x04;

	// Configure the PB4 (SSI2 SCLK) and the PB7 (SSI2 MOSI) pins to use the alternate function
	// by setting Bit 4 and Bit 7 in the AFSEL register
	GPIOB->AFSEL |= 0x90;
	
	// Clear the PMC4 and PMC7 fields in the PCTL register before configuration
	GPIOB->PCTL &= ~0xF00F0000;
	
	// Configure the PB7 and the PB4 pins to operate as SSI pins by writing 0x2 to the
	// corresponding PMCn fields in the PCTL register.
	// The 0x2 value is derived from Table 15-1 in the TM4C123G Microcontroller Datasheet
	GPIOB->PCTL |= 0x20020000;

	// Enable the digital functionality for the PB4 and PB7 pins
	// by setting Bit 4 and Bit 7 in the DEN register
	GPIOB->DEN |= 0x90;
	
	// Configure the direction of the PC7 pin to output. This pin will be used
	// as the slave select pin for SSI2 and it will be active low
	GPIOC->DIR |= 0x80;
	
	// Configure the PC7 pin to function as a GPIO pin by
	// clearing Bit 7 in the AFSEL register
	GPIOC->AFSEL &= ~0x80;

	// Enable the digital functionality for PC7 pin
	// by setting Bit 7 in the DEN register
	GPIOC->DEN |= 0x80;
	
	// Initialize the output of the PC7 pin to high by setting Bit 7 in the DATA register
	GPIOC->DATA |= 0x80;
	
	// Disable the SSI2 module before configuration by clearing
	// the SSE bit (Bit 1) in the CR1 register
	SSI2->CR1 &= ~0x02;
	
	// Disable the SSI loopback mode by clearing the LBM bit (Bit 0) in the CR1 register
	SSI2->CR1 &= ~0x01;
	
	// Configure the SSI2 module as a master device by clearing
	// the MS bit (Bit 2) in the CR1 register
	SSI2->CR1 &= ~0x04;
	
	// Specify the SSI2 clock source to use the PIOSC by
	// writing a value of 0x05 to the CS field (Bits 3 to 0) in the CC register
	SSI2->CC = 0x05;
	
	// Set the clock frequency of SCLK by writing the prescale divisor value
	// to the CPSDVSR field (Bits 7 to 0) in the CPSR register.
	// For example, a value of 50 can be written to CPSR and the SCR field (Bits 15 to 8)
	// in the CR0 register can be cleared to 0 to configure the SCLK frequency to 1 MHz
	// SSInClk = PIOSC Frequency / (CPSDVSR * (1 + SCR))
	// SSInClk = 16 MHz / (16 * (1 + 0)) = 1 MHz
	SSI2->CPSR = 16;
	SSI2->CR0 &= ~0xFF00;
	
	// Configure the SSI2 module to capture data on the first clock edge transition
	// by clearing the SPH bit (Bit 7) in the CR0 register
	SSI2->CR0 &= ~0x0080;
	
	// Configure the SCLK of the SSI2 module to be idle low when inactive
	// by clearing the SPO bit (Bit 6) in the CR0 register
	SSI2->CR0 &= ~0x0040;
	
	// Configure the SSI2 module to use the Freescale SPI Frame Format
	// by clearing the FRF field (Bits 5 to 4) in the CR0 register
	SSI2->CR0 &= ~0x0030;
	
	// Configure the SSI0 module to have a data length of 8 bits
	// by writing a value of 0x7 to the DSS field (Bits 3 to 0) in the CR0 register
	SSI2->CR0 |= 0x0007;

	// Enable the SSI2 module after configuration by setting the SSE bit (Bit 1) in the CR1 register
	SSI2->CR1 |= 0x02;

	// Enable the SSI2 module after configuration by setting
	// the SSE bit (Bit 1) in the CR1 register
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
