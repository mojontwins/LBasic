// Simple text mode routines 

#include <stdlib.h>
#include <string.h>
#include "../dos-like/source/dos.h"
#include "lstextmode.h"

int buf_x, buf_y;
int buf_c1, buf_c2;
int viewport_y1 = 1;
int viewport_y2 = 23;
int bud_mode = LS_MODE_TEXT

void buf_setviewport (int y1, int y2) {
	viewport_y1 = y1;
	viewport_y2 = y2;
}

void buf_setxy (int x, int y) {
	buf_x = x; buf_y = y;
}

void buf_setx (int x) {
	buf_x = x;
}

void buf_sety (int y) {
	buf_y = y;
}

int buf_getx (void) {
	return buf_x;
}

int buf_gety (void) {
	return buf_y;
}

int buf_getscrw (void) {
	return screenwidth ();
}

int buf_getscrh (void) {
	return screenheight ();
}

void buf_color (int c1, int c2) {
	if (c1 != -1) buf_c1 = c1;
	if (c2 != -1) buf_c2 = c2;
}

int buf_getc1 (void) {
	return buf_c1;
}

int buf_getc2 (void) {
	return buf_c2;
}

int buf_get_attrib (void) {
	return (buf_c1 & 0xf) | ((buf_c2 & 0xf) << 4);
}

void buf_scroll_up (int from, int to) {
	unsigned char *buf = screenbuffer ();
	int scr_w = screenwidth ();
	int scr_w_bytes = scr_w * 2;
	int idx;
	int x, y;
	int attrib = buf_get_attrib ();

	// Move everything up
	for (y = from; y <= to; y ++) {
		idx = y * scr_w_bytes;
		for (x = 0; x < scr_w_bytes; x ++) {
			buf [idx - scr_w_bytes] = buf [idx ++];
		}
	}

	// Clear bottom
	idx = to * scr_w_bytes;
	for (x = 0; x < scr_w; x ++) {
		buf [idx ++] = 32;
		buf [idx ++] = attrib;
	}
}

void buf_scroll_up_if_needed (void) {
	int scr_h = screenheight ();

	while (buf_y >= viewport_y2 + 1) {
		buf_scroll_up (viewport_y1 + 1, viewport_y2);
		buf_y --;
	}
}

void buf_printabs (char *s) {
	unsigned char *buf = screenbuffer ();
	char c;
	int scr_w = screenwidth ();

	int idx;
	int attrib = buf_get_attrib ();

	while (c = *s ++) {
		idx = (buf_x + buf_y * scr_w) * 2;
		buf [idx] = c;
		buf [idx + 1] = attrib;

		buf_x ++; if (buf_x == scr_w) {
			buf_x = 0;
			buf_y ++;
		}
	}
}

void buf_print (char *s) {
	unsigned char *buf = screenbuffer ();
	char c;
	int scr_w = screenwidth ();

	int idx;
	int attrib = buf_get_attrib ();

	while (c = *s ++) {
		buf_scroll_up_if_needed ();
		
		idx = (buf_x + buf_y * scr_w) * 2;
		buf [idx] = c;
		buf [idx + 1] = attrib;

		buf_x ++; if (buf_x == scr_w) {
			buf_x = 0;
			buf_y ++;
		}
	}
}

void buf_print_ln (char *s) {
	buf_print (s);
	buf_y ++;
	buf_x = 0;
	buf_scroll_up_if_needed ();
}

void buf_cls (void) {
	int i;
	int attrib = buf_get_attrib ();
	int scr_chars = screenwidth () * screenheight ();
	char *buf = screenbuffer ();

	for(i = 0; i < scr_chars; i ++) {
		*buf ++ = 32;
		*buf ++ = attrib;
	}

	buf_x = buf_y = 0;
}

void buf_clscroll (void) {
	int i;
	int attrib = buf_get_attrib ();
	int scr_chars = screenwidth () * (viewport_y2 - viewport_y1 + 1);
	char *buf = screenbuffer () + viewport_y1 * 2 * screenwidth ();

	for(i = 0; i < scr_chars; i ++) {
		*buf ++ = 32;
		*buf ++ = attrib;
	}

	buf_x = 0; buf_y = viewport_y1;
}

void buf_put_string_xy (int x, int y, int c1, int c2, char *s) {
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

void lstextmode_init (void) {
	setvideomode (videomode_80x25_9x16);
}

void buf_setmode(unsigned char mode) {
	buf_mode = mode;	
}
