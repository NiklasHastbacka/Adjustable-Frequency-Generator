#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "LCD.h"

void PORT_init();
void LCD_init();
void INT_init();
void FREQ_count(); // Function to calculate display freq

// Assigned variables
unsigned int count = 59286; // Initial value for Timer1 counter
int presc[] = {1,8,64,256,1024}; // Array of prescaler values
int i=1, n=12, steg=10000; // Variables for encoder step

float freq; // Store calculated frequency
long freq_helt; // Integer part of frequency
int freq_deci; // Decimal part of freq

int main (void)
{
	// Initialize ports, LCD, interrupts and freq calculations
	PORT_init();
	LCD_init();
	INT_init();
	FREQ_count();

	while(1);
	return(0);
}


ISR(TIMER1_OVF_vect)
{
	PORTA ^= 0x80;	// Toggle PA7, this generates a square wave signal
	TCNT1 = count;	// Reload the Timer1 counter register with 'count' Value(59286)
}

// Interrupt service routine for INT0
// INT0 is an external interrupt on AVR microcontrollers
ISR(INT0_vect)
{
	cli(); // Disable global interrupts to safely modify variables
	
	// Check if pin 3 on port B is high or low to determine the direction of rotation
	if (bit_is_set(PINB, 3))
	{
		count += steg; // Increase (higher freq)
	}
	else if (bit_is_clear(PINB, 3))
	{
		count -= steg; // Decrease (lower freq)
	}
	
	_delay_ms(100);

	FREQ_count(); // Recalculate and update freq
}

// Interrupt service routine for INT1 
ISR(INT1_vect)
{
	cli();

	n++; // Increment the mode index for prescaler adjustments
	
	if (n == 17) n = 12; // Wrap around if 'n' exceeds the limit (Mode cycling)
	
	LCD_goto(ROW4+1);


	if (n == 12) steg = 10000;
	else if (n == 13) steg = 1000;
	else if (n == 14) steg = 100;
	else if (n == 15) steg = 10;
	else steg = 1;

	sei(); // Re-enable global interrupts
}

// Interrupt service routine for PCINT1 (handles additional encoder buttons)
ISR(PCINT1_vect)
{
	// Check the state of the encoder buttons and modify 'i' for prescaler selection
	if (bit_is_clear(PINB, 2))
	i++;
	else if (bit_is_clear(PINB, 1))
	i = 2;
	else if (bit_is_clear(PINB, 0))
	i--;
	
	// Ensure 'i' stays within the bounds of the prescaler array
	if (i < 0) i = 0;
	else if (i > 4) i = 4;

	_delay_ms(100);

	FREQ_count();
}

// Initialize the microcontrollers ports
void PORT_init(void)
{
	DDRA = 0x80; // Set PA7 as output (for signal generation)
	DDRB = 0x00; // Set port B pins as inputs (for encoder and buttons)
}

// Initialize the interrupt system
void INT_init(void)
{
	// Configure interrupts for INT0 and INT1 to trigger on a falling edge (when signal goes from high to low)
	EICRA = 0x0A;  // Falling edge trigger for INT0 and INT1
	
	// Enable interrupts for INT0 and INT1
	EIMSK = 0x03;  // Enable INT0 (bit 0) and INT1 (bit 1)
	
	// Set Timer1 prescaler based on 'i'. This controls how fast Timer1 counts.
	TCCR1B = (i + 1);  // Prescaler based on value of 'i'
	
	// Initialize Timer1 counter with 'count' value
	TCNT1 = count;  // Set initial counter value
	
	// Enable Timer1 overflow interrupt (TOIE1), which triggers when Timer1 overflows
	TIMSK1 = 0x01;  // Enable Timer1 overflow interrupt
	
	// Enable pin change interrupts on Port B (for buttons)
	PCICR = 0x02;  // Enable pin change interrupts on Port B (PCINT1)

	// Enable interrupts for PB0, PB1, PB2 (buttons connected to these pins)
	PCMSK1 = 0x07;  // Enable interrupts for PB0, PB1, PB2
	
	sei(); // Enable global interrupts

}

// Function to calculate and display the frequency on the LCD
void FREQ_count()
{
	cli(); // Disable global interrupts to safely calculate freq
	
	// Calculate the frequency using the formula
	freq = (float) 20000000 / 2 / presc[i] / (1 + 65535 - count);
	
	freq_helt = freq; // Get the integer part of the frequency
	freq_deci = (freq * 100.0) - (freq_helt * 100); // Get the decimal part (2 digits)

	// Line 1

	LCD_goto(ROW1+17);
	LCD_puts(" Hz");
	
	LCD_goto(ROW1+12);
	LCD_puti(freq_deci);

	LCD_goto(ROW1+14);
	LCD_putc(',');

	LCD_goto(ROW1+9);
	LCD_puti(freq_helt);

	if (freq_deci < 10)
	{
		LCD_goto(ROW1+15);
		LCD_putc('0');
	}

	LCD_goto(ROW1);
	LCD_puts("Freq.:");

	// Line 2

	LCD_goto(ROW2+12);
	LCD_puti(presc[i]);

	LCD_goto(ROW2);
	LCD_puts("Prescaler:");

	// Line 3

	LCD_goto(ROW3+12);
	LCD_puti(count);

	LCD_goto(ROW3);
	LCD_puts("Count:");

	// Line 4

	LCD_goto(ROW4+1);
	
	// Reinitialize interrupt settings after each frequency update
	INT_init();
}