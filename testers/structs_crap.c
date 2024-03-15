#include <stdio.h>
#include <stdlib.h>

typedef struct PELO {
	int pelito;
	char *textor;
	char (*function) (char);
} PELO;

PELO *popoth = NULL;
int elements = 0;

void add (PELO pelo) {
	int index = elements;
	elements ++;
	popoth = realloc (popoth, elements * sizeof (PELO));
	popoth [index] = pelo;
}

char char1 (char c) {
	return c;
}

char char2 (char c) {
	return c + 1;
}

struct PELO getPelo1 (void) {
	PELO pelo;
	pelo.pelito = 1;
	pelo.textor = "Mariantonia";
	pelo.function = char1;
	return pelo;
}

struct PELO getPelo2 (void) {
	PELO pelo;
	pelo.pelito = 2;
	pelo.textor = "Josemario";
	pelo.function = char2;
	return pelo;
}

void main(void) {
	add (getPelo1());
	add (getPelo2());

	printf ("%d %s %d\n", popoth [0].pelito, popoth [0].textor, popoth [0].function (1));
	printf ("%d %s %d\n", popoth [1].pelito, popoth [1].textor, popoth [1].function (1));
}