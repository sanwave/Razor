
#include "Stringdev.h"

Stringdev::Stringdev() {
}

/*****************************************************************************/
/*           字符串分割                                                      */
/*****************************************************************************/
char* Stringdev::spiltHalf(char **fromto, char spilter) {
	char *from = *fromto;
	int i = 0;
	for(; *(from+i)!=0x00; ++i) {
		if(*(from+i) == spilter) {
			*(from+i) = '\0';
			*fromto = from + i + 1;
			return from;
		}
	}
	*fromto = from + i;
	return from;
}