#include "utils.h"

int strToInt(char* str) {

	int negative = 0;	/* sign */
	int acum = 0;	/* Accumulator */

	/* Get rid of white spaces */
	while (*str == ' ')
		str++;
	
	/* Infer sign */
	if (*str == '-') {
		negative = 1;
		str++;
	
	} else if (*str == '+') {
		str++;
	}


	/* While *str is a digit */
    while((*str >= '0')&&(*str <= '9')) {
		
		/* Shift number to the left in the decimal system and add the new digit */
		acum = acum * 10 + *str - '0';

		/* Increment str */
    	str++;
	}

    return negative ? -acum : acum;
}