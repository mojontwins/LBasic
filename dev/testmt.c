// Test 

#include <stdlib.h>
#include "../dos-like/source/dos.h"

int buf_x, buf_y;
int buf_c1, buf_c2;

void buf_setxy (int x, int y) {
	buf_x = x; buf_y = y;
}

void buf_setx (int x) {
	buf_x = x;
}

void buf_sety (int y) {
	buf_y = y;
}

void buf_color (int c1, int c2) {
	if (c1 != -1) buf_c1 = c1;
	if (c2 != -1) buf_c2 = c2;
}

void buf_scroll_up (int from, int to) {
	unsigned char *buf = screenbuffer ();
	int scr_w = screenwidth ();
	int scr_w_bytes = scr_w * 2;
	int idx;
	int x, y;

	// Move everything up
	for (y = from; y <= to; y ++) {
		idx = y * scr_w_bytes;
		for (x = 0; x < scr_w_bytes; x ++) {
			buf [idx - scr_w_bytes] = buf [idx ++];
		}
	}

	// Clear bottom
	idx = to * scr_w_bytes;
	for (x = 0; x < scr_w_bytes; x ++) {
		buf [idx - scr_w_bytes] = buf [idx ++];
	}
}

void buf_print (char *s) {
	unsigned char *buf = screenbuffer ();
	char c;
	int scr_w = screenwidth ();
	int scr_h = screenheight ();
	int idx;
	int attrib = (buf_c1 & 0xf) | ((buf_c2 & 0xf) << 4);

	while (c = *s ++) {
		idx = (buf_x + buf_y * scr_w) * 2;
		buf [idx] = c;
		buf [idx + 1] = attrib;

		buf_x ++; if (buf_x == scr_w) {
			buf_x = 0;
			buf_y ++;

			// Leave 1 empty line at the bottom for a status bar
			if (buf_y == scr_h - 1) {
				buf_scroll_up (2, scr_h - 2);
				buf_y --;
			}
		}
	}
}

void put_string_xy (int x, int y, int c1, int c2, char *s) {
	unsigned char *buf = screenbuffer ();
	char c;
	int scr_w = screenwidth ();
	int idx = (x + y * scr_w) * 2;
	int attrib = (c1 & 0xf) | ((c2 & 0xf) << 4);

	while (c = *s ++) {
		buf [idx ++] = c;
		buf [idx ++] = attrib;
	}
}

int main (void) {
	setvideomode (videomode_80x25_9x16);
	buf_setxy (0, 0);

	//                          "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
	put_string_xy (0, 0, 14, 1, " LBASIC - The Black Smurff                                                      ");
	put_string_xy (0, 24, 14, 1, " Bloque: 001 | Oportunidades: 7                                                 ");

	while (!shuttingdown ()) {
		buf_color (rand () & 15, rand () & 15);
		buf_print ("HOLA PICHA DE PLATINO");

        waitvbl();
	}
}
