// Simple text mode routines 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../dos-like/source/dos.h"
#include "lstextmode.h"

#include "ega14.h"

int buf_x, buf_y;
int buf_c1, buf_c2;
int buf_col1, buf_col2;
int viewport_y1 = 1;
int viewport_y2 = 23;
int buf_mode = LS_MODE_TEXT;
int buf_mouse_x = 0;
int buf_mouse_y = 0;

int buf_char_height = 8;
int font_ega14;

void debuff_keys (void) {
	int any;
	do {
		any = 0;

		unsigned char const* chars = (unsigned char*) readchars ();
		enum keycode_t* keys = readkeys ();

		char c;
		while (c = *chars ++) {
			any = 1;
		}		

		unsigned long long key;
		while (key = (unsigned long long) *keys ++) {
			any = 1;
		}
	} while (any);
}

int buf_get_mouse_x (void) {
	buf_mouse_x = mousex ();
	if (buf_mouse_x < 0) buf_mouse_x = 0;
	if (buf_mouse_x >= screenwidth ()) buf_mouse_x = screenwidth () - 1;
	return buf_mouse_x;
}

int buf_get_mouse_y (void) {
	buf_mouse_y = mousey ();
	if (buf_mouse_y < 0) buf_mouse_y = 0;
	if (buf_mouse_y >= screenheight ()) buf_mouse_y = screenheight () - 1;
	return buf_mouse_y;
}

int buf_get_mouse_b (int button) {
	if (button & 1) return keystate (KEY_LBUTTON);
	if (button & 2) return keystate (KEY_RBUTTON);
	return 0;
}

void buf_setviewport (int y1, int y2) {
	viewport_y1 = y1;
	viewport_y2 = y2;
}

int buf_getviewport_y1 (void) {
	return viewport_y1;
}

int buf_getviewport_y2 (void) {
	return viewport_y2;
}

void buf_setmargins (int col1, int col2) {
	buf_col1 = col1;
	buf_col2 = col2;
}

void buf_setxy (int x, int y) {
	buf_x = x; buf_y = y;
}

void buf_resetxy (void) {
	buf_x = 0; buf_y = viewport_y1;
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

void buf_box (int x1, int y1, int x2, int y2) {
	unsigned char *buf = screenbuffer ();
	int scr_w = screenwidth ();
	int scr_w_bytes = scr_w * 2;
	int attrib = buf_get_attrib ();

	for (int y = y1; y <= y2; y ++) {
		for (int x = x1; x <= x2; x ++) {
			char c;
			if (x > x1 && x < x2) {
				if (y == y1 || y == y2) {
					c = 0xC4;
				} else {
					c = 32;
				}
			} else if (x == x1 || x == x2) {
				if (y != y1 && y != y2) {
					c = 0xB3;
				} else {
					if (x == x1) {
						c = y == y1 ? 0xDA : 0xC0;
					} else {
						c = y == y1 ? 0xBF : 0xD9;
					}
				}
			}

			int idx = y * scr_w_bytes + x * 2;
			buf [idx] = c;
			buf [idx + 1] = attrib;
		}
	}
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
		int scr_w = screenwidth ();
		int byte_size = (to - from + 1) * buf_char_height * scr_w;
		char *temp_buf = malloc (byte_size);
		unsigned char *buf = screenbuffer ();

		memcpy (temp_buf, buf + from * buf_char_height * scr_w, byte_size);

		// Put at 8 * from - 8
		memcpy (buf + (from - 1) * buf_char_height * scr_w, temp_buf, byte_size);

		// Clear 8 * to to 8 * to + 7
		setcolor (buf_c2);
		bar (0, to * buf_char_height, scr_w, buf_char_height);

		free (temp_buf);
	}
}

void buf_scroll_up_if_needed (void) {
	while (buf_y > viewport_y2) {
		buf_scroll_up (viewport_y1 + 1, viewport_y2);
		buf_y --;
	}
}

void buf_char (char c) {
	buf_scroll_up_if_needed ();
	if (buf_mode == LS_MODE_TEXT) {
		unsigned char *buf = screenbuffer ();
		
		int scr_w = screenwidth ();
		int idx = (buf_x + buf_y * scr_w) * 2;
		
		buf [idx] = c;
		buf [idx + 1] = buf_get_attrib ();

		buf_x ++; if (buf_x == scr_w) {
			buf_x = 0;
			buf_y ++;
		}
	} else {
		// TODO
	}
}

void _buf_print (char *s, int scroll, int no_break, int clip_to_scroll) {
	if (buf_mode == LS_MODE_TEXT) {
		unsigned char *buf = screenbuffer ();
		char c;
		int scr_w = screenwidth ();

		int idx;
		int attrib = buf_get_attrib ();

		while (c = *s ++) {
			if (clip_to_scroll && (buf_y < viewport_y1 || buf_y > viewport_y2)) break;

			if (scroll) {
				buf_scroll_up_if_needed ();
			} else {
				if (buf_y >= screenheight ()) break;
			}

			idx = (buf_x + buf_y * scr_w) * 2;
			buf [idx] = c;
			buf [idx + 1] = attrib;

			buf_x ++; if (buf_x == scr_w) {
				if (no_break) break;
				
				buf_x = 0;
				buf_y ++;
			}
		}
	} else {
		int x1 = 8 * buf_x;
		int y1 = buf_char_height * buf_y;
		
		char c;
		char *substr = " ";
		int scr_w = screenwidth () / 8;

		while (c = * s ++) {
			if (clip_to_scroll && (buf_y < viewport_y1 || buf_y > viewport_y2)) break;

			if (scroll) {
				buf_scroll_up_if_needed ();
				y1 = buf_char_height * buf_y;
			} else {
				if (buf_y >= screenheight ()) break;
			}

			setcolor (buf_c2);
			if (buf_c2 < 256) bar (x1, y1, 8, buf_char_height);
			substr [0] = c;
			setcolor (buf_c1);
			outtextxy (x1, y1, substr);

			x1 += 8; buf_x ++; if (buf_x == scr_w) {
				if (no_break) break;
				
				buf_x = x1 = 0;
				buf_y ++;
				y1 += buf_char_height;
			}
		}
	}
}

void buf_wordwrap (char *s) {
	// col1, col2 are INCLUSIVE i.e. 0 79 for full width

	char word [256]; char *wp = word;
	char c;

	while (1) {
		c = *s ++;

		// Obtain a whole word
		if (c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == 0) {
			*wp = 0;		// End string

			if (buf_x + strlen (word) - 1 > buf_col2) {
				buf_y ++;
				buf_x = buf_col1;
			}

			_buf_print (word, 1, 0, 0);

			if (buf_x <= buf_col2) {
				_buf_print (" ", 1, 0, 0);
			}

			wp = word;
		} else {
			*wp ++ = c;
		}

		if (c == 0) break;
	}

	buf_y ++;
	buf_x = 0;
	buf_scroll_up_if_needed ();
}

void buf_print_abs (char *s) {
	_buf_print (s, 0, 0, 0);
}

void buf_print_abs_clip_to_scroll (char *s) {
	_buf_print (s, 0, 0, 1);
}

void buf_print (char *s) {
	_buf_print (s, 1, 0, 0);
}

void buf_print_trim (char *s) {
	_buf_print (s, 0, 1, 0);
}

void buf_print_ln (char *s) {
	buf_print (s);
	if (buf_x != 0) { 
		buf_y ++;
		buf_x = 0;
		// buf_scroll_up_if_needed ();
	}
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
		bar (0, buf_char_height * viewport_y1, screenwidth (), buf_char_height * (viewport_y2 - viewport_y1 + 1));
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

void buf_setmode(int mode) {
	buf_mode = mode;
	buf_char_height = 8;
	switch (buf_mode) {
		case LS_MODE_GFX:
			setvideomode (videomode_320x200);
			break;
		case LS_MODE_GFX_SQ:
			setvideomode (videomode_320x240);
			break;
		case LS_MODE_GFX_HIRES:
			setvideomode (videomode_640x350);
			font_ega14 = installuserfont_array (ega14_fnt);
			settextstyle (font_ega14, 0, 0, 0);
			buf_char_height = 14;
			break;
		case LS_MODE_GFX_MED:
			setvideomode (videomode_640x200);
			break;
		default:
			setvideomode (videomode_80x25_9x16);
			break;
	}

	if (buf_mode == LS_MODE_TEXT) {
		buf_setviewport (1, screenheight () - 2);
		buf_setmargins (0, screenwidth () - 1);
		buf_clscroll ();
	} else {
		buf_setviewport (1, screenheight () / buf_char_height);
		buf_setmargins (0, screenwidth () / 8 - 1);
		buf_cls ();
	}

}

int buf_getmode (void) {
	return buf_mode;
}

int buf_heartbeat (void) {
	waitvbl ();
	return shuttingdown ();
}

void buf_pause (void) {
	int c;

	while (!buf_heartbeat ()) {
		c = *readchars ();
		if (c > 0) break;
	}

	enum keycode_t* keys;
	do {
		keys = readkeys();
	} while (*keys);
}

void buf_gif_at (char *gif, int x, int y, int do_setpal) {
	// Wont load a gif bigger than the screen so...
	char *gif_buffer;
	char *pal_buffer = malloc (768);
	int w, h, p;

	gif_buffer = loadgif (gif, &w, &h, &p, pal_buffer);

	blit (x, y, gif_buffer, w, h, 0, 0, w, h);

	if (do_setpal) {
		char *pal_ptr = pal_buffer;
		int i;
		int r, g, b;
		for (i = 0; i < p; i ++) {
			r = (int) *pal_ptr; pal_ptr ++;
			g = (int) *pal_ptr; pal_ptr ++;
			b = (int) *pal_ptr; pal_ptr ++;
			setpal (i, r, g, b);
		}
	}

	free (gif_buffer);
	free (pal_buffer);
}

void buf_textmode_pic (char *pic) {
	if (buf_mode == LS_MODE_TEXT) {
		char *buf = screenbuffer ();
		FILE *pf = fopen (pic, "rb");
		fread (buf, sizeof (char), 80*25*2, pf);
		fclose (pf);
	}
}

void lstextmode_init (void) {
	buf_setmode (LS_MODE_TEXT);
}
