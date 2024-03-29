#include <avr/io.h>
#include "inc/adc_util.h"
#include "inc/bit_util.h"

void adc_init(int s0,int s1)
{
	// AREF = Avcc .. Internal
	//s0 = 1 s1 = 1 Internal reference 1.1
	//s0 = 0 s1 = 0 turn off internal reference
	ADMUX = (s0<<REFS0);//if s0 is 1 set refs0 with 1
	ADMUX |= (s1<<REFS1);//if s1 is 1 set refs1 with 1 
	// ADC Enable and prescaler of 8
	// 1000000 / 8 = 125000
	ADCSRA = (1<<ADEN)| (1<<ADPS1)|(1<<ADPS0);
}

unsigned int adc_read(unsigned char ch)
{
	
	RESET_BIT_RNG(ADMUX, 0, 4);	//reset any previous selected channel
	ADMUX |= ch;	// select the corresponding channel 0~7
	ADCSRA |= (1<<ADSC);		// start single conversion, by writing '1' to ADSC (AD Start Conversion)
	// when the conversion is complete, ADSC returns to zero
	// till then, poll the ADC Status Register A
	while(ADCSRA & (1<<ADSC));
	return ADC;
}