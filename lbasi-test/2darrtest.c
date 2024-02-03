#include <stdio.h>

int a1 [5][5] = {
	{1, 2, 3, 4, 5},
	{5, 4, 3, 2, 1},
	{1, 2, 0, 2, 1},
	{3, 3, 3, 3, 3},
	{2, 2, 2, 2, 6}
};

int main (void) {
	int ** a2;

	printf ("%d %d %d %d %d\n", a1 [0], a1 [1], a1[2], a1[3], a1[4]);

	a2 = a1;

	printf ("%d %d\n", a1, a2);
}
