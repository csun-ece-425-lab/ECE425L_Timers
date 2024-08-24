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

#include "TM4C123GH6PM.h"
#include "SysTick_Delay.h"

extern const uint8_t number_pattern[16];

/**
 * @brief Initializes the Seven-Segment Display module on the EduBase board.
 *
 * This function initializes the pins connected to the shift register ICs that will communicate
 * with the Seven-Segment Display module on the EduBase board. It configures the
 * Synchronous Serial Interface 2 (SSI2) peripheral to operate in SPI mode.
 *
 * @param None
 *
 * @return None
 *
 * @note Assumes that a 16 MHz clock is used.
 */
void Seven_Segment_Display_Init(void);

/**
 * @brief Transmits data using the SSI2 module.
 *
 * This function writes the specified data to the Transmit Buffer of the SSI2 peripheral.
 * It asserts the slave select pin, writes the data to the SSI data register, waits for the data transmission to complete,
 * and then deasserts the slave select pin.
 *
 * @param data The 8-bit data to be written.
 *
 * @return None
 */
void SSI2_Write(uint8_t data);

/**
 * @brief Counts the number of digits in an integer value.
 *
 * This function counts the number of digits in the specified integer value.
 * It repeatedly divides the value by 10 until it becomes zero, incrementing a counter for each division.
 * The final counter value represents the number of digits in the original value.
 *
 * @param value The integer value for which the number of digits is to be counted.
 *
 * @return The number of digits in the specified integer value.
 */
int Count_Digits(int value);

/**
 * @brief Displays a value in decimal representation on the Seven-Segment Display module on the EduBase board.
 *
 * This function displays the specified number in decimal representation on the Seven-Segment Display module.
 * It calculates the number of digits in the value, extracts each digit, retrieves the corresponding pattern from
 * the number_pattern array, and writes the pattern to the seven-segment display sequentially.
 *
 * @param count_value The decimal number to be displayed on the Seven-Segment Display module.
 *
 * @return None
 */
void Seven_Segment_Display(int count_value);
