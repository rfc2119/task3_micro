#ifndef _BIT_UTIL_H_
#define _BIT_UTIL_H_

#define _PASTE2(x, y)			x ## y
#define SET_BIT(reg, n)			(reg |= (1 << n))
#define RESET_BIT(reg, n)		(reg &= ~(1 << n))
#define TOGGLE_BIT(reg, n)		(reg ^= (1 << n))
#define READ_BIT(reg, n)		(reg & (1 << n))

#define IN 1
#define OUT 0
#define SET_BIT_IO(reg_letter, n, io)\
	io == IN ? RESET_BIT(_PASTE2(DDR, reg_letter), n): SET_BIT(_PASTE2(DDR, reg_letter), n)

//from bit 0 till bit 7
#define SET_BIT_RNG(reg, from, to)\
	(reg |= ((1 << (to+1)) - 1) ^ ((1 << from) - 1))
#define RESET_BIT_RNG(reg, from, to)\
	(reg &= ~( ((1 << (to+1)) - 1) ^ ((1 << from) - 1)) )

#define SET_BIT_IO_RNG(reg_letter, from, to, io)\
	io == IN ? RESET_BIT_RNG(_PASTE2(DDR, reg_letter), from, to) : SET_BIT_RNG(_PASTE2(DDR, reg_letter), from, to)
#endif