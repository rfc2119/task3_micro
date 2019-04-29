#ifndef _ADC_UTIL_H_
#define _ADC_UTIL_H_

/*intializes the adc unit; check your manual for values
 of s0 and s1*/
void adc_init(int s0,int s1);

/*reads and returns from adc channel ch*/
unsigned int adc_read(unsigned char ch);

#endif