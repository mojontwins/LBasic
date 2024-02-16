#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "..\dos-like\source\dos.h"

void main(void) {
	unsigned char small [4];
	setvideomode( videomode_640x350 );
	int font = installuserfont( "ega14.fnt" );
	settextstyle( font, 0, 0, 0 );

	setcolor (14);
	outtextxy (0, 0, "And now, some EGA text!");

	setcolor (7);
	for (int i = 0; i < 256; i ++) {
		small [0] = (unsigned char) i;
		small [1] = 0;

		outtextxy(8 * (i & 15), 28 + 14 * (i / 16), small);
	}

	while (!shuttingdown ()) {}
}
