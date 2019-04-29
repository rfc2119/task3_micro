#include <avr/io.h>
#include <stdlib.h>			/*div_t, div*/
#include <avr/interrupt.h> /*sei, INTX_vect*/

#include "inc/bit_util.h"
#include "inc/adc_util.h"

#define MODE_TEMPERATURE_SENSOR 0
#define MODE_POT 1

#define BUZZER PORTD0
#define F_CPU 1000000UL

/*global vars*/
unsigned char mode;
void (*resetptr)( void ) = 0x0000;

int main(void)
{
	/*configurations*/
	SET_BIT_IO_RNG(B, PORTB0, PORTB7, OUT);	//output to BCD drivers
	SET_BIT_IO_RNG(C, PORTC0, PORTC1, IN);	//ADC{0,1}
	
	SET_BIT_IO(D, INT0, IN);		//INT0
	SET_BIT(PORTD, PORTD2);			//activate pull-up res for INT0
	SET_BIT_IO(D, BUZZER, OUT);		//activate buzzer
	
	EICRA |= (1 << ISC01);  // set INT0 to trigger on falling edge change
	EIMSK |= (1 << INT0); // Turns on INT0 
	sei(); // turn on interrupts

	/*initializations */
	float analog, digital;
	div_t result;
	mode = MODE_TEMPERATURE_SENSOR;
	//mode = MODE_POT;
	adc_init(1,0);		//5V for all modes
	for (;;)
	{
		if (READ_BIT(mode, 0) == MODE_POT){

			/*
			set vref = 5
			read the potentiometer
			analog = reading * v_ref / ((1 << 11) - 1)
			display integer part
			display fraction part
			*/
			
			digital = adc_read(MODE_POT);
			analog = digital * 5 / ((1 << 10) -1 );	//TODO: 1<<10 - 1 is ? (hint: adc bits)
			result = div(analog*10, 10); 
			
			// TODO: improve output operation to BCD drivers
			PORTB = ((int)analog & 0x0F) << PORTB0;//display digit part
			PORTB |= (result.rem & 0x0F) << PORTB4;	//display fraction part
			
		}
		else if (READ_BIT(mode, 0) == MODE_TEMPERATURE_SENSOR)
		{
			/*
			set vref = 1.1 (hmmm...)
			read the lm sensor
			analog = reading * v_ref / ((1 << 11) - 1)
			analog *= 100 (hint: 10mV/C)
			if (analog > 60) BUZZ! display 0xffC
			*/
			digital = adc_read(MODE_TEMPERATURE_SENSOR);
			analog = (digital * 5) / ((1 << 10) - 1);
			analog *= 100;//TODO: clarify the magic 100 (hint: 10mv/C)
			
			//buzzer!
			(analog > 60 ) ? SET_BIT(PORTD, BUZZER) :RESET_BIT(PORTD, BUZZER); 
			
			//SET_BIT_RNG(PORTB, PORTB0, PORTB7); //0xff, for debug
			result = div((int)analog, 10);
			PORTB = (result.quot & 0x0F) << PORTB0;//display tens part
			PORTB |= (result.rem & 0x0F) << PORTB4;	//display units part
			
		}
	}
	return -1;
}

ISR (INT0_vect) 
{
	/*switches modes and reset display*/
	TOGGLE_BIT(mode, 0);
	if(mode == MODE_TEMPERATURE_SENSOR){resetptr();};
}