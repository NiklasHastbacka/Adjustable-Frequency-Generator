//*****************************************************************************
//
// File Name			: 'LCD.h'
// Title				: 4 bit LCD routines
// Revision date		: 14-12-2009
// MCU					: Atmel ATMEGA164P
// LCD Driver			: Sitronix ST7066U
//
//*****************************************************************************

#ifndef LCD_H
#define LCD_H

#include <stdio.h>			// See HELP / avr-libc Reference Manual for detailed information
#include <avr/io.h>			// See HELP / avr-libc Reference Manual for detailed information
#include <util/delay.h>		// See HELP / avr-libc Reference Manual for detailed information

/**** Define LCD data port ****/
#define LCD_DATA_PORT PORTC
#define LCD_DATA_DDR DDRC

/**** Define LCD control port ****/
#define LCD_CTRL_PORT PORTA
#define LCD_CTRL_DDR DDRA

/**** Define LCD control pins ****/
#define LCD_rs 5								
#define LCD_rw 6
#define LCD_en 7

/**** Define LCD rows 1-4 ****/
#define ROW1	0
#define ROW2	64
#define ROW3	20
#define ROW4	84

/**** Define LCD instructions ****/
#define LCD_origin() LCD_command(0x2)  			// Return cursor to orignal site and return display to its original status, if shifted
#define LCD_reset() LCD_command(0x01)			// Reset the display
#define LCD_function_set() LCD_command(0x28) 	// Select 2-line display mode
#define LCD_display_off() LCD_command(0x08) 	// Turn LCD off
#define LCD_display_on() LCD_command(0x0F) 		// Turn LCD on
#define LCD_display_clear() LCD_command(0x01)	// Clear LCD
#define LCD_entry_mode_set() LCD_command(0x06) 	// Set cursor/blink "move" direction to right and DDRAM adress is increased with 1

/**** LCD function declarations ****/
void LCD_init();		
void LCD_enable();	
void LCD_goto(int p);
void LCD_command(unsigned char command);
void LCD_puti(unsigned int v);
void LCD_nibble(unsigned char nibble);
void LCD_putc(char ascii);
void LCD_puts(char *lcd_string);		
void waitForLCD();

#endif 
