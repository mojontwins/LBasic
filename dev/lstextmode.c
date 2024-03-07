// Simple text mode routines 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../dos-like/source/dos.h"
#include "../dos-like/source/libs/pixelfont.h"

#include "lstextmode.h"
#include "keys.h"

#include "ega14.h"
#include "atari.h"

int buf_x, buf_y;
int buf_c1, buf_c2;
int buf_col1, buf_col2;
int viewport_y2 = 23;
int viewport_y1 = 1;
int buf_mode = LS_MODE_TEXT;
int buf_mouse_x = 0;
int buf_mouse_y = 0;

int buf_char_height = 8;
int font_ega14;
int font_atari8x8;

int buf_char_delay = 4;
int buf_do_delay;

char *buf_copy;
int buf_size;

int buf_scrwidth = 80;
int buf_scrheight = 25;

int pixyoffset = 0;

void lstextmode_init (void) {
	font_ega14 = installuserfont_array (ega14_fnt);
	font_atari8x8 = installuserfont_array (atari_fnt);

	buf_setmode (LS_MODE_TEXT);
}

void buf_set_buf_char_delay (int d) {
	buf_char_delay = d;
}

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

int buf_get_scrwidth (void) {
	return buf_scrwidth;
}

int buf_get_scrheight (void) {
	return buf_scrheight;
}

void buf_char_by_char_delay (void) {
	if (buf_do_delay) {
		keys_read ();
		int keys_this_frame = keys_get_this_frame ();
		if (keys_this_frame & (MT_KEY_ENTER | MT_KEY_ESC | MT_KEY_LBUTTON)) {
			buf_do_delay = 0;
		} else {
			for (int i = 0; i < buf_char_delay && !buf_heartbeat (); i ++);
		}
	}
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

int buf_to_char_coords_x (int x) {
	return x >> 3;
}

int buf_to_char_coords_y (int y) {
	return y / buf_char_height;
}

void buf_setviewport (int y1, int y2) {
	viewport_y1 = y1;
	viewport_y2 = y2;
	buf_y = viewport_y1;
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
	buf_x = buf_col1;
}

void buf_setxy (int x, int y) {
	buf_x = x; buf_y = y;
}

void buf_resetxy (void) {
	buf_x = buf_col1; buf_y = viewport_y1;
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
	if (buf_mode == LS_MODE_TEXT) {
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
	} else {
		int x = x1 * 8;
		int y = y1 * buf_char_height;
		int w = (x2 - x1 + 1) * 8;
		int h = (y2 - y1 + 1) * buf_char_height;

		if (screenheight () > 240) {
			y += buf_char_height - 8;
			h -= 2 * (buf_char_height - 8);
		}

		setcolor (buf_c2);
		bar (x, y, w, h);

		setcolor (buf_c1);
		rectangle (x, y, w, h);
	}
}

// TODO - Do it properly, use buf_col1, buf_col2. 
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

		memcpy (temp_buf, buf + (from * buf_char_height + pixyoffset) * scr_w, byte_size);

		// Put at 8 * from - 8
		memcpy (buf + ((from - 1) * buf_char_height + pixyoffset) * scr_w, temp_buf, byte_size);

		// Clear 8 * to to 8 * to + 7
		setcolor (buf_c2);
		bar (0, to * buf_char_height + pixyoffset, scr_w, buf_char_height);

		free (temp_buf);
	}
}

void buf_scroll_up_window (int x1, int y1, int x2, int y2) {
	if (buf_mode == LS_MODE_TEXT) {
		// Text mode

		unsigned char *buf = screenbuffer ();
		int scr_w = screenwidth ();
		int scr_w_bytes = scr_w * 2;
		int w = x2 - x1 + 1;
		int w_bytes = w * 2;
		char *ptr;
		int attrib = buf_get_attrib ();

		// Move everything up
		for (int y = y1; y < y2; y ++) {
			ptr = buf + y * scr_w_bytes + x1 * 2;
			memcpy (ptr, ptr + scr_w_bytes, w_bytes);
		}

		// Clear bottom
		ptr = buf + y2 * scr_w_bytes + x1 * 2;
		for (int x = 0; x < w; x ++) {
			*ptr ++ = 32; *ptr ++ = attrib;
		}
	} else {
		// Graphics mode
		int x1_p = 8 * x1;
		int x2_p = 8 * x2 + 7;
		int y1_p = buf_char_height * y1;
		int y2_p = buf_char_height * y2 + buf_char_height - 1;
		int width = 1 + x2_p - x1_p;
		int height = 1 + y2_p - y1_p - buf_char_height;

		int byte_size = width * height;
		char *temp_buf = malloc (byte_size);

		// I've checked, blit works up to down, so I can be dirty and do this:
		blit (
			x1_p, y1_p + pixyoffset, 
			screenbuffer (), 
			screenwidth (), screenheight (), 
			x1_p, y1_p + buf_char_height + pixyoffset, 
			width, height
		);

		// Clear bottom
		setcolor (buf_c2);
		bar (x1_p, y2 * buf_char_height + pixyoffset, width, buf_char_height);

		free (temp_buf);
	}
}

void buf_scroll_up_if_needed (void) {
	while (buf_y > viewport_y2) {
		//buf_scroll_up (viewport_y1 + 1, viewport_y2);
		buf_scroll_up_window (buf_col1, viewport_y1, buf_col2, viewport_y2);
		buf_y --;
	}
}

void buf_char_abs (int x, int y, int c1, int c2, char c) {
	if (buf_mode == LS_MODE_TEXT) {
		unsigned char *buf = screenbuffer ();
		
		int scr_w = screenwidth ();
		int idx = (x + y * scr_w) * 2;
		
		buf [idx] = c;
		buf [idx + 1] = c1 | (c2 << 4);

	} else {
		int x1 = 8 * x;
		int y1 = buf_char_height * y;

		unsigned char minibuf [2];
		minibuf [0] = c; minibuf [1] = 0;

		setcolor (c2);
		if (c2 < 256) bar (x1, y1, 8, buf_char_height);
		setcolor (c1);
		outtextxy (x1, y1, minibuf);
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

		buf_x ++; if (buf_x == buf_col2 + 1) {
			buf_x = buf_col1;
			buf_y ++;
		}
	} else {
		int x1 = 8 * buf_x;
		int y1 = buf_char_height * buf_y;

		unsigned char minibuf [2];
		minibuf [0] = c; minibuf [1] = 0;

		setcolor (buf_c2);
		if (buf_c2 < 256) bar (x1, y1, 8, buf_char_height);
		setcolor (buf_c1);
		outtextxy (x1, y1, minibuf);		
		buf_x ++; if (buf_x == buf_col2 + 1) {
			buf_x = buf_col1;
			buf_y ++;
		}
	}
}

void _buf_print (char *s, int scroll, int no_break, int clip_to_scroll, int char_by_char) {
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

			if (char_by_char) buf_char_by_char_delay ();

			buf_x ++; if (buf_x == buf_col2 + 1) {
				if (no_break) break;
				
				buf_x = buf_col1;
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
			if (buf_c2 < 256) bar (x1, y1 + pixyoffset, 8, buf_char_height);
			substr [0] = c;
			setcolor (buf_c1);
			outtextxy (x1, y1 + pixyoffset, substr);

			if (char_by_char) buf_char_by_char_delay ();

			x1 += 8; buf_x ++; if (buf_x == scr_w) {
				if (no_break) break;
				
				buf_x = x1 = 0;
				buf_y ++;
				y1 += buf_char_height;
			}
		}
	}
}

void _buf_wordwrap (char *s, int char_by_char) {
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

			_buf_print (word, 1, 0, 0, char_by_char);

			if (buf_x <= buf_col2) {
				_buf_print (" ", 1, 0, 0, char_by_char);
			}

			wp = word;
		} else {
			*wp ++ = c;
		}

		if (c == 0) break;
	}

	buf_y ++;
	buf_x = buf_col1;
	// buf_scroll_up_if_needed ();
}

void buf_wordwrap (char *s) {
	_buf_wordwrap (s, 0);
}

void buf_print_abs (char *s) {
	_buf_print (s, 0, 0, 0, 0);
}

void buf_print_abs_clip_to_scroll (char *s) {
	_buf_print (s, 0, 0, 1, 0);
}

void buf_print (char *s) {
	_buf_print (s, 1, 0, 0, 0);
}

void buf_print_trim (char *s) {
	_buf_print (s, 0, 1, 0, 0);
}

void buf_print_ln (char *s) {
	buf_print (s);
	if (buf_x != 0 || strlen (s) == 0) { 
		buf_y ++;
		buf_x = buf_col1;
		// buf_scroll_up_if_needed ();
	}
}

void buf_wordwrap_char_by_char (char *s) {
	buf_do_delay = 1;
	keys_read ();
	_buf_wordwrap (s, 1);
}

void buf_print_char_by_char (char *s) {
	buf_do_delay = 1;
	keys_read ();
	_buf_print (s, 1, 0, 0, 1);
}

void buf_tb (int x1, int y1, int w, int h, int c1, int c2, int tc1, int tc2, int f, char *title, char *text, int char_by_char) {
	if (buf_mode == LS_MODE_TEXT) {
		// TODO
	} else {
		int x1p = x1 * 8;
		int y1p = y1 * buf_char_height;
		int wp = w * 8;
		int hp = h * buf_char_height;
		int x2p = x1p + wp - 1;
		int y2p = y1p + hp - 1;
		int dotitle = title && strlen (title);

		int h2 = h;
		if (dotitle) {
			setcolor (tc2);
			bar (x1p, y1p, wp, buf_char_height);
			setcolor (tc1);
			outtextxy (x1p + 8, y1p, title);
			h2 --;
		} else {
			pixyoffset = -8;
		}


		int y1p2 = y1 * buf_char_height;

		for (int hi = 1; hi <= h2; hi ++) {
			setcolor (c2);
			bar (x1p, y1p2, wp, hi * buf_char_height);
			setcolor (f);
			rectangle (x1p - 1, y1p - 1, wp + 2, (hi + dotitle) * buf_char_height + 1);
			rectangle (x1p - 2, y1p - 2, wp + 2, (hi + dotitle) * buf_char_height + 3);
			waitvbl ();
		}

		if (dotitle == 0) {
			y1 ++;
			h --;
		}
		
		int backup_x = buf_x;
		int backup_y = buf_y;
		int backup_c1 = buf_c1;
		int backup_c2 = buf_c2;
		int backup_viewport_y1 = viewport_y1;
		int backup_viewport_y2 = viewport_y2;
		int backup_col1 = buf_col1;
		int backup_col2 = buf_col2;

		buf_c1 = c1; buf_c2 = c2;
		buf_x = x1 + 1; buf_y = y1;
		buf_col1 = x1 + 1; buf_col2 = x1 + w - 2;
		viewport_y1 = y1; viewport_y2 = y1 + h - 1;

		buf_do_delay = 1;
		keys_read ();
		_buf_wordwrap (text, char_by_char);

		buf_x = backup_x; buf_y = backup_y;
		buf_c1 = backup_c1; buf_c2 = backup_c2;
		viewport_y1 = backup_viewport_y1; viewport_y2 = backup_viewport_y2;
		buf_col1 = backup_col1; buf_col2 = backup_col2;
	}

	pixyoffset = 0;

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
		char *buf = screenbuffer ();
		char *ptr;
		int attrib = buf_get_attrib ();
		int scr_w = screenwidth ();
		int scr_w_bytes = 2 * scr_w;
		int w = buf_col2 - buf_col1 + 1;

		for (int y = viewport_y1; y <= viewport_y2; y ++) {
			ptr = buf + y * scr_w_bytes + buf_col1 * 2;
			for (int x = 0; x < w; x ++) {
				*ptr ++ = 32; * ptr ++ = attrib;
			}
		}

	} else {
		setcolor (buf_c2);
		bar (
			buf_col1 * 8, 
			buf_char_height * viewport_y1, 
			(1 + buf_col2 - buf_col1) * 8, 
			buf_char_height * (viewport_y2 - viewport_y1 + 1)
		);
	}

	buf_x = buf_col1; buf_y = viewport_y1;
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

void buf_font_atari (void) {
	settextstyle (font_atari8x8, 0, 0, 0);
}

void buf_font_cga (void) {
	settextstyle (1, 0, 0, 0);
}

void buf_setmode(int mode) {
	buf_mode = mode;
	buf_char_height = 8;
	
	switch (buf_mode) {
		case LS_MODE_GFX:
			setvideomode (videomode_320x200);
			settextstyle (1, 0, 0, 0);
			break;
		case LS_MODE_GFX_SQ:
			setvideomode (videomode_320x240);
			settextstyle (1, 0, 0, 0);
			break;
		case LS_MODE_GFX_HIRES:
			setvideomode (videomode_640x350);
			settextstyle (font_ega14, 0, 0, 0);
			buf_char_height = 14;
			break;
		case LS_MODE_GFX_MED:
			setvideomode (videomode_640x200);
			settextstyle (1, 0, 0, 0);
			break;
		case LS_MODE_GFX_DBL:
			setvideomode (videomode_640x400);
			settextstyle (2, 0, 0, 0);
			buf_char_height = 16;
			break;
		default:
			setvideomode (videomode_80x25_9x16);
			settextstyle (3, 0, 0, 0);
			break;
	}

	if (buf_mode == LS_MODE_TEXT) {
		buf_setviewport (1, screenheight () - 2);
		buf_setmargins (0, screenwidth () - 1);
		buf_clscroll ();
		buf_size = screenwidth () * screenheight () * 2;
		buf_scrwidth = screenwidth ();
		buf_scrheight = screenheight ();
	} else {
		buf_setviewport (1, screenheight () / buf_char_height);
		buf_setmargins (0, screenwidth () / 8 - 1);
		buf_cls ();
		buf_size = screenwidth () * screenheight ();
		buf_scrwidth = screenwidth () / 8;
		buf_scrheight = screenheight () / buf_char_height;
	}

	free (buf_copy);
	buf_copy = malloc (buf_size);
}

void buf_sve (void) {
	memcpy (buf_copy, screenbuffer (), buf_size);
}

void buf_rec (void) {
	memcpy (screenbuffer (), buf_copy, buf_size);
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

void buf_gif_at (char *gif, int x, int y, int do_setpal, int mask) {
	// Wont load a gif bigger than the screen so...
	char *gif_buffer;
	char *pal_buffer = malloc (768);
	int w, h, p;

	gif_buffer = loadgif (gif, &w, &h, &p, pal_buffer);

	if (mask == -1) {
		blit (x, y, gif_buffer, w, h, 0, 0, w, h);
	} else {
		maskblit (x, y, gif_buffer, w, h, 0, 0, w, h, mask);
	}

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

// Assumes compatible video mode!
void buf_bulma_pix (char *pix, int dbl, int do_setpal, int fancy) {
	unsigned char *buf = screenbuffer ();
	int width = screenwidth ();
	int height = screenheight ();

	char pix_buffer [7 + 64000 + 768];
	char render_buffer [width * height];

	if (dbl && width != 640) dbl = 0;

	FILE *pf = fopen (pix, "rb");
	if (pf) {
		int bytes = fread (pix_buffer, 1, sizeof (pix_buffer), pf);

		if (do_setpal) {
			char *pal_ptr = pix_buffer + 7 + 64000;
			int r, g, b;
			for (int i = 0; i < 256; i ++) {
				r = (int) *pal_ptr ++;
				g = (int) *pal_ptr ++;
				b = (int) *pal_ptr ++;
				setpal (i, r, g, b);
			}
		}

		if (dbl) {
			unsigned char *ptr = pix_buffer + 7;
			int x = 0; int ofs = 0;
			for (int i = 0; i < 64000; i ++) {
				unsigned char c = *ptr ++;
				render_buffer [ofs + x] = c;
				render_buffer [ofs + x + 1] = c;
				render_buffer [ofs + width + x] = c;
				render_buffer [ofs + width + x + 1] = c;
				x += 2;
				if (x == width) {
					x = 0; ofs += width + width;
				}
			}
		} else {
			// Just copy raw
			memcpy (render_buffer, pix_buffer + 7, 64000);
		}

		fclose (pf);

		// Blit
		if (fancy) {
			int sqs = width == 640 ? 16 : 8;
			int wx = width / sqs; 				// wx = 40
			int hy = height / sqs; 				// hy = 25
			int maxy = hy / 2;

			for (int i =maxy; i >= 0; i --) {
				for (int x = i; x < wx - i; x ++) {
					blit (x * sqs, i * sqs, render_buffer, width, height, 
						x * sqs, i * sqs, sqs, sqs);

					blit (x * sqs, height - (i + 1) * sqs, render_buffer, width, height, 
						x * sqs, height - (i + 1) * sqs, sqs, sqs);
				}

				for (int y = i; y < hy - i; y ++) {
					blit (i * sqs, y * sqs, render_buffer, width, height,
						i * sqs, y * sqs, sqs, sqs);

					blit (width - (i + 1) * sqs, y * sqs, render_buffer, width, height,
						width - (i + 1) * sqs, y * sqs, sqs, sqs);
				}

				waitvbl ();
			}

		} else {
			memcpy (buf, render_buffer, sizeof (render_buffer));
		}
	}

}

void buf_textmode_pic (char *pic) {
	if (buf_mode == LS_MODE_TEXT) {
		char *buf = screenbuffer ();
		FILE *pf;
		if (pf = fopen (pic, "rb")) {
			fread (buf, sizeof (char), 80*25*2, pf);
			fclose (pf);
		}
	}
}

void buf_bulma_lin (char *pic) {
	// Read 11 byte chunks, build by hand (safer), draw using dos-like.
	FILE *file;

	if (file = fopen (pic, "rb")) {
		while (!feof (file)) {
			char cmd = (char) fgetc (file);
			int v1 = fgetc (file) + 256 * fgetc (file);
			int v2 = fgetc (file) + 256 * fgetc (file);
			int v3 = fgetc (file) + 256 * fgetc (file);
			int v4 = fgetc (file) + 256 * fgetc (file);
			int v5 = fgetc (file) + 256 * fgetc (file);

			switch (cmd) {
				case 'L':
					setcolor (v5);
					line (v1, v2, v3, v4);
					break;
				case 'P':
					setcolor (v3);
					floodfill (v1, v2);
					break;
				case 'C':
					setcolor (v4);
					circle (v1, v2, v3);
					break;
			}
		}
		fclose (file);
	}
}

void buf_load_font_16 (char *font) {
	FILE *file = fopen (font, "rb");
	if (file) {
		PIXELFONT_U8 pixels [32 * 32];
		pixelfont_builder_t* builder = pixelfont_builder_create (
			16, 
			12, 
			16,
			NULL
		);

		for (int i = 0; i < 256; i ++) {
			for (int y = 0; y < 16; y ++) {
				char c = fgetc (file);

				for (int x = 0; x < 8; x ++) {
					if (c & (1 << (7 - x))) {
						pixels [x + 8 * y] = 1;
					} else {
						pixels [x + 8 * y] = 0;
					}
				}
			}

			pixelfont_builder_glyph (builder, i, 8, pixels, 8, 0, 8);
		}

		pixelfont_t* pixelfont = pixelfont_builder_font (builder);
		int font = installuserfont_array ((char const *) pixelfont);
		settextstyle (font, 0, 0, 0);

		fclose (file);
	}
}

void lstextmode_shutdown (void) {
	free (buf_copy);
}
