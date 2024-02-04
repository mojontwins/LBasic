// Simple text mode routines 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../dos-like/source/dos.h"
#include "lstextmode.h"

int buf_x, buf_y;
int buf_c1, buf_c2;
int viewport_y1 = 1;
int viewport_y2 = 23;
int buf_mode = LS_MODE_TEXT;

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
	if (buf_mode == LS_MODE_TEXT) {
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
	} else {
		// Get from 8 * from to 8 * to + 7 
		// Put at 8 * from - 8
		// Clear 8 * to to 8 * to + 7
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
	if (buf_mode == LS_MODE_TEXT) {
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
	} else {
		int x1 = 8 * buf_x;
		int y1 = 8 * buf_y;
		int w = 8 * strlen(s);
		int h = 8;
		char c;
		char *substr = " ";
		int scr_w = screenwidth ();

		while (c = * s ++) {
			setcolor (buf_c2);
			if (buf_c2 < 256) bar (x1, y1, 8, 8);
			substr [0] = c;
			setcolor (buf_c1);
			outtextxy (x1, y1, substr);

			x1 += 8; buf_x ++; if (buf_x == scr_w) {
				buf_x = x1 = 0;
				buf_y ++;
				y1 += 8;
			}
		}
	}
}

void buf_print (char *s) {
	if (buf_mode == LS_MODE_TEXT) {
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
	} else {
		int x1 = 8 * buf_x;
		int y1 = 8 * buf_y;
		int w = 8 * strlen(s);
		int h = 8;
		char c;
		char *substr = " ";
		int scr_w = screenwidth ();

		while (c = * s ++) {
			buf_scroll_up_if_needed ();
			setcolor (buf_c2);
			if (buf_c2 < 256) bar (x1, y1, 8, 8);
			substr [0] = c;
			setcolor (buf_c1);
			outtextxy (x1, y1, substr);

			x1 += 8; buf_x ++; if (buf_x == scr_w) {
				buf_x = x1 = 0;
				buf_y ++;
				y1 += 8;
			}
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
	if (buf_mode == LS_MODE_TEXT) {
		int i;
		int attrib = buf_get_attrib ();
		int scr_chars = screenwidth () * screenheight ();
		char *buf = screenbuffer ();

		for(i = 0; i < scr_chars; i ++) {
			*buf ++ = 32;
			*buf ++ = attrib;
		}
	} else {
		setcolor (buf_c2);
		bar (0, 0, screenwidth (), screenheight ());
	}

	buf_x = buf_y = 0;
}

void buf_clscroll (void) {
	if (buf_mode == LS_MODE_TEXT) {
		int i;
		int attrib = buf_get_attrib ();
		int scr_chars = screenwidth () * (viewport_y2 - viewport_y1 + 1);
		char *buf = screenbuffer () + viewport_y1 * 2 * screenwidth ();

		for(i = 0; i < scr_chars; i ++) {
			*buf ++ = 32;
			*buf ++ = attrib;
		}
	} else {
		setcolor (buf_c2);
		bar (0, 8 * viewport_y1, screenwidth (), 8 * (viewport_y2 - viewport_y1 + 1));
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
	buf_setmode (LS_MODE_TEXT);
}

void buf_setmode(unsigned char mode) {
	buf_mode = mode;
	switch (buf_mode) {
		case LS_MODE_GFX:
			setvideomode (videomode_320x200);
			break;
		case LS_MODE_GFX_SQ:
			setvideomode (videomode_320x240);
			break;
		case LS_MODE_GFX_HIRES:
			setvideomode (videomode_640x480);
			break;
		default:
			setvideomode (videomode_80x25_9x16);
			break;
	}
}

int heartbeat (void) {
	waitvbl ();
	return shuttingdown ();
}

void buf_pause (void) {
	while (!heartbeat ()) {
		int c = *readchars ();
		if (c > 0) break;
	}
}

