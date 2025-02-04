//*****************************************************************************
//
// File Name			: 'LCD.c'
// Title				: 4 bit LCD routines
// Revision date		: 14-12-2009
// MCU					: Atmel ATMEGA164P
// LCD Driver			: Sitronix ST7066U
// Based on				: http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=65472&highlight
//						: http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=75516&highlight=
//
//*****************************************************************************

#include <avr/io.h>			// See HELP / avr-libc Reference Manual for detailed information
#include <util/delay.h>		// See HELP / avr-libc Reference Manual for detailed information
#include "LCD.h"

void LCD_enable()
{
    //*****************************************************************************
	//
	//	Toggle enable pin (first high then low)
	// 
	//*****************************************************************************
	_delay_us(1);
    LCD_CTRL_PORT |= (1<<LCD_en);	// Sets EN pin
	_delay_us(1);
	LCD_CTRL_PORT &= ~(1<<LCD_en);	// Clears EN pin
	_delay_us(1);
}
void LCD_command(unsigned char command)
{
    //*****************************************************************************
	//
	//	Convert a 8-bit command to 4-bit
	// 
	//*****************************************************************************
	
	LCD_CTRL_PORT &= ~(1<<LCD_rs); 									// Clear RS pin
    LCD_DATA_PORT = (command & 0xF0)|(LCD_DATA_PORT & 0x0F);		// For writing the high nibble, the low nibble of the argument is cleared
    LCD_enable();													// Toggle enable
	LCD_DATA_PORT = (command << 4 & 0xF0)|(LCD_DATA_PORT & 0x0F);	// For writing the low nibble, the high nibble of the argument is cleread, argument has to be shifted since datalines are on high port lines.
    LCD_enable();
    _delay_ms(1);
}

void LCD_putc(char ascii)
{
    //*****************************************************************************
	//
	//	Print chars on the LCD
	//  Doesn´t support following letters å,Å,Ä and Ö
	//
	//*****************************************************************************
	
	
	if(ascii=='ä') ascii=225; 									// Convert ä to 225
 	if(ascii=='ö') ascii=239; 									// Convert ö to 239
	
	LCD_CTRL_PORT |= (1<<LCD_rs);								// Set RS pin
    LCD_DATA_PORT = (ascii & 0xF0)|(LCD_DATA_PORT & 0x0F);		// For writing the high nibble, the low nibble of the argument is cleared
	LCD_enable();												// Toggle enable to write high nibble

	LCD_DATA_PORT = (ascii << 4 & 0xF0)|(LCD_DATA_PORT & 0x0F); // For writing the low nibble, the high nibble of the argument is cleread, argument has to be shifted since datalines are on high port lines.
    LCD_enable();												// Toggle enable write low nibble
	_delay_ms(1);

}

void LCD_puts(char *lcd_string)
{
    //*****************************************************************************
	//
	//	Prints strings on the LCD
	//
	//*****************************************************************************
	
	while (*lcd_string)
   	{  
	  LCD_putc(*lcd_string++);								// Character by character
	  waitForLCD();											// Wait until display is ready
   	}
}

void LCD_nibble(unsigned char nibble)
{
    //*****************************************************************************
	//
	//	Send high nibble (DB4 - DB7), low nibble is cleared (DB0 - DB3)
	//
	//*****************************************************************************
	
	LCD_CTRL_PORT &= ~(1<<LCD_rs);							// Clear RS pin
    LCD_DATA_PORT = (nibble & 0xF0)|(LCD_DATA_PORT & 0x0F);	// For writing the high nibble, the low nibble of the argument is cleared
    LCD_enable();
	_delay_ms(1);											// Toggle enable to write high nibble											
}

void LCD_init()
{
  	//*****************************************************************************
	//
	//	Intilializes the LCD
	//	4-bits interface
	//
	//****************************************************************************

   	LCD_CTRL_PORT &= ~(1<<LCD_rs);   		// Clear RS pin, defined in header file
   	LCD_CTRL_PORT &= ~(1<<LCD_rw);   		// Clear R/W pin, defined in header file
   	LCD_CTRL_PORT &= ~(1<<LCD_en);   		// Clear EN pin. defined in header file
	
	LCD_CTRL_DDR |= (1<<LCD_rs);			// Set RS pin (output)
   	LCD_CTRL_DDR |= (1<<LCD_rw);			// Set R/W pin (output)
   	LCD_CTRL_DDR |= (1<<LCD_en);			// Set EN pin (output)

	LCD_DATA_DDR |= 0xF0;					// Set PC4 - PC7 to output
    _delay_ms(100);      					// Delay after power up
	
	LCD_nibble(0x20);						// Select 4-bit mode
    _delay_ms(20);
	
	LCD_function_set();						// See header file for more detailed information
   	_delay_ms(10);
	   	
	LCD_function_set();						// See header file for more detailed information
   	waitForLCD();							// Wait until display is ready

	//LCD_display_off();					// See header file for more detailed information
	LCD_display_on();
	waitForLCD();							// Wait until display is ready
   	
	LCD_display_clear();					// See header file for more detailed information
	waitForLCD();							// Wait until display is ready
   	
	LCD_entry_mode_set();					// See header file for more detailed information
	waitForLCD();							// Wait until display is ready

	_delay_ms(50);
}

void LCD_goto(int p)
{
  	//*****************************************************************************
	//
	//	Move the cursor to given position (rows are defined in header file)
	// 
	//*****************************************************************************
	
	LCD_CTRL_PORT &= ~(1<<LCD_rs);   	// Clear RS pin
   	LCD_CTRL_PORT &= ~(1<<LCD_rw);  	// Clear R/W pin
	LCD_command(0x80+p);				// Move cursor to given position
	waitForLCD();						// Wait until display is ready
}

void LCD_puti(unsigned int v)
{
  
	//*****************************************************************************
	//
	//	Prints numbers on the LCD
	// 
	//*****************************************************************************
  
	  unsigned d;       
	  char tkn=' ';
  
	  d=v/10000%10;
	  if(d)
	  {
	    LCD_putc(d+'0');
	    tkn='0';
	  } 
	  else LCD_putc(tkn);
  
	  d=v/1000%10;
	  if(d)
	  {
	    LCD_putc(d+'0');
	    tkn='0';        
	  }
	  else LCD_putc(tkn);

	  d=v/100%10;
	  if(d)
	  {
	    LCD_putc(d+'0');
	    tkn='0';
	  }
	  else LCD_putc(tkn);

	  d=v/10%10;
	  if(d)
	  { 
	    LCD_putc(d+'0'); 
	    tkn='0';
	  }
	  else LCD_putc(tkn);
	  LCD_putc(v%10+'0');
}  


void waitForLCD() 
{
   //*****************************************************************************
	//
	//	Check the busy flag (DB7)
	// 
	//*****************************************************************************
   
   
   LCD_CTRL_PORT &= ~(1<<LCD_rs);   	// Clear RS pin, defined in header file
   LCD_CTRL_PORT |= (1<<LCD_rw);   		// Set R/W pin (enable read mode), defined in header file
   
   LCD_DATA_DDR &= ~0xF0;				// Set PC4 - PC7 to input

   char busy = 0;
   
   while (busy != 0)
   {
      	
		LCD_CTRL_PORT |= (1<<LCD_en);	// Set EN pin
		_delay_us(1);
      	
		busy = PINC & 0x80;				// Check if busy flag is set (DB7 (PC7))
      	
		LCD_CTRL_PORT &= ~(1<<LCD_en);	// Clear EN pin
	  	_delay_us(1);
      	
		LCD_CTRL_PORT |= (1<<LCD_en);	// Set EN pin
		_delay_us(1);
 
      	LCD_CTRL_PORT &= ~(1<<LCD_en);	// Clear EN pin
	  	_delay_us(1);
   	} 
	
	LCD_DATA_DDR |= 0xF0;				// Set PC4 - PC7 to output
	LCD_CTRL_PORT &= ~(1<<LCD_rw);		// Clear R/W pin (enable write mode)
} 
