#include "../dos-like/source/dos.h"

void main (void) {

	int w = 720, h = 400;
	setvidemode ((h - 1) | ((w - 1) << 10));

	settextstyle (3, 0, 0, 0);
	outtextxy (100, 100, "Este Texto es 9x16 en modo GFX");
}