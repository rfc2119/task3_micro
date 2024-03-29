#include <avr/io.h>
#include <stdlib.h>			/*div_t, div*/
#include <avr/interrupt.h> /*sei, INTX_vect*/

#include "inc/bit_util.h"
#include "inc/adc_util.h"

#define MODE_TEMPERATURE_SENSOR PORTC0
#define MODE_POT PORTC1

#define BUZZER PORTD0
#define F_CPU 1000000UL

/*global vars*/
volatile char mode;

int main(void)
{
	
	/*configurations*/
	SET_BIT_IO_RNG(B, PORTB0, PORTB7, OUT);	//output to BCD drivers
	SET_BIT_IO_RNG(C, PORTC0, PORTC1, IN);	//ADC{0,1}
	
	SET_BIT_IO(D, INT0, IN);		//INT0
	SET_BIT(PORTD, PORTD2);			//activate pull-up res for INT0
	SET_BIT_IO(D, BUZZER, OUT);		//activate buzzer

	SET_BIT(EICRA, ISC01);  // set INT0 to trigger on falling edge change
	RESET_BIT(EICRA, ISC00);
	
	EIMSK |= (1 << INT0); // Turns on INT0 
	sei(); // turn on interrupts

	/*initializations */
	float analog, digital;
	div_t result;
	mode = MODE_TEMPERATURE_SENSOR;
	adc_init(1,0);		//5V for all modes
	
	for (;;)
	{
		if (mode == MODE_POT){
			/*
			read the potentiometer
			analog = reading * v_ref / ((1 << 11) - 1)
			display integer part
			display fraction part
			*/
			ADC = 0;
			digital = adc_read(MODE_POT);
			analog = (digital * 5) / ((1 << 10) -1 );	//TODO: 1<<10 - 1 is ? (hint: adc bits)
			result = div(analog*10, 10); 
			
			// TODO: improve output operation to BCD drivers
			PORTB = (result.rem & 0x0F) << PORTB0;	//display fraction part
			PORTB |= (result.quot & 0x0F) << PORTB4;//display digit part
			
			//
			RESET_BIT(PORTD, BUZZER);

		}
		else /*MODE_TEMPERATURE_SENSOR*/
		{
			
			/*
			read the lm sensor
			analog = reading * v_ref / ((1 << 11) - 1)
			analog *= 100 (hint: 10mV/C)
			if (analog > 60) BUZZ!
			*/
			digital = adc_read(MODE_TEMPERATURE_SENSOR);
			analog = (digital * 5) / ((1 << 10) - 1);
			analog *= 100;							//TODO: clarify the magic 100 (hint: 10mv/C)
			(analog > 60) ?	SET_BIT(PORTD, BUZZER) : RESET_BIT(PORTD, BUZZER);
			
			result = div((int)analog, 10);
			PORTB = (result.rem & 0x0F) << PORTB0;	//display units part
			PORTB |= (result.quot & 0x0F) << PORTB4;//display tens part
			
		}
	}
	return -1;
}

ISR (INT0_vect) 
{
	/*switches modes*/
	TOGGLE_BIT(mode, 0);
}