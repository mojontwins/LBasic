#include <stdio.h>

char *array [] = {
	"print",
	"choice",
	"draw",
	"color",
	"pause",
};

void main (void) {
	printf ("%d \n", sizeof (array));
	for (int i = 0; i < sizeof (array); i ++) {
		printf ("%s\n", array [i]);
	}	
}
