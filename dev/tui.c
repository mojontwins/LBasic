#include <string.h>

#include "tui.h"
#include "keys.h"
#include "lstextmode.h"
#include "conversion.h"

#include "../dos-like/source/dos.h"

// A pointer to this array will be returned but it should be
// memcopied / strcpy'ed to elsewhere!
unsigned char text_area [2048];
char str_divider_line [] = "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4";
char blank [] = "                                                                                ";

int find_draw_cursor (int x, int y) {
	int xx = 0;
	int yy = 0;

	unsigned char c;

	for (int i = 0; i < strlen (text_area); i ++) {
		if (x == xx && y == yy) return i;

		c = text_area [i];
		if ((c >= '0' && c <= '7') || 
			c == '!' || c == '\"' || c == '\xFA' || c == '$' ||
			c == '%' || c == '&' || c == '/' || c == '='
		) {
			// Do not count
		} else {
			xx ++; if (xx == 80) { xx = 0; yy ++; }			
		}
	}
}

char *tui_drawbox (char *org_text, int *action) {
	int cursor; 
	int done = 0;
	char *res;
	unsigned char c;

	if (org_text && org_text [0] != 0) {
		strcpy (text_area, org_text);
		cursor = strlen (org_text);
	} else {
		text_area [0] = 0;
		cursor = 0;
	}

	buf_setxy (0, 1);
	buf_color (7, 1);
	buf_print_abs (str_divider_line);
	buf_setxy (1, 1);
	buf_print_abs ("\xB4 ");
	buf_print_abs ("Editor Draw");
	buf_print_abs (" \xC3");

	buf_setxy (0, 20);
	buf_print_abs (str_divider_line);
	buf_setxy (1, 20);
	buf_print_abs ("\xB4 ENTER: Insertar \xB3 ESC: Cancelar \xC3");	
	buf_setxy (0, 21);	
	buf_color (15, 1);
	buf_print_abs ("       \xDA q  \xC4 w  \xBF e   \xC9 r  \xCD t  \xBB y     \xB0 u  \xB1 i  \xB2 o  \xDB p      \xDC l  \xDF \xA4       ");
	buf_print_abs ("       \xB3 a             \xBA s        1-7 y 0 1er plano, SHIFT + 1-7 y 0 fondo      ");
	buf_print_abs ("       \xC0 z       \xD9 x   \xC8 c       \xBC v     \xC3V \xB4b \xC1n \xC2m \xC5d \xCCf \xB9g \xCAh \xCBj \xCEk             ");

	curson ();

	while (!done && !buf_heartbeat ()) {
		int cursor_x;
		int cursor_y;

		// Display & find x, y
		int x = 0;
		int y = 0;
		int c1 = 7; 
		int c2 = 0;
		int j;

		buf_color (7, 0);
		for (int i = 2; i < 20; i ++) {
			buf_setxy (0, i);
			buf_print_abs (blank);
		}

		// TODO:: Fix this
		for (int i = 0; i < strlen (text_area); i ++) {
			c = text_area [i];
			j = key_to_color_1 (c); if (j > 0) {
				c1 = j; buf_color (c1, -1);
			} else {
				j = key_to_color_2 (c); if (j > 0) {
					c2 = j; buf_color (-1, c2);
				} else {
					buf_setxy (x, y);
					buf_char (c);

					if (i == cursor) {
						cursor_x = x;
						cursor_y = y;
						gotoxy (cursor_x, 2 + cursor_y);
					}

					x ++; if (x == 80) {
						x = 0; y ++;
					}
				}
			}
		}

		// Get input
		unsigned char const* chars = (unsigned char*) readchars ();
		enum keycode_t* keys = readkeys ();

		// Characters
		c = get_character_input (chars);
		c = key_to_draw_char (c);

		if (c >= ' ') {
			if (cursor_y < 19 || cursor_x < 79) {
				text_area [cursor ++] = c;
				text_area [cursor] = 0;
			}
		} else {
			switch (c) {
				case 8:
					// Delete
					if (cursor > 0) {
						cursor --;
						text_area [cursor] = 0;
					}
					break;

				case 27:
					// Exit
					done = 1;
					res = NULL;
					*action = TUI_ACTION_ESC;
					break;
			}
		}

		// Special keys
		while (*keys) {
			unsigned long long key = (unsigned long long) *keys;

			if (key == KEY_UP) {
				if (cursor_y > 0) {
					cursor_y --;
					cursor = find_draw_cursor (cursor_x, cursor_y);
				}
			}

			if (key == KEY_DOWN) {
				if (cursor_y < 18) {
					cursor_y ++;
					cursor = find_draw_cursor (cursor_x, cursor_y);
				}
			}

			if (key == KEY_LEFT) {
				if (cursor > 0) cursor --;
			}

			if (key == KEY_RIGHT) {
				if (cursor < strlen (text_area) - 1) cursor ++;
			}

			if (key == KEY_RETURN) {
				res = text_area;
				done = 1;
				*action = TUI_ACTION_ENTER;
				break;
			}

			keys ++;
		}

		if (cursor < 0) {
			cursor = 0;
		} else if (cursor > strlen (text_area)) {
			cursor = strlen (text_area);
		}
	}

	debuff_keys ();
	return res;	
}

char *tui_textbox (int y, char *caption, char *org_text, int max_lines, int *action) {
	int cursor;
	int max_chars = 80 * max_lines - 1;
	int done = 0;
	unsigned char c;
	char *res;

	if (org_text && org_text [0] != 0) {
		strcpy (text_area, org_text);
		cursor = strlen (org_text);
	} else {
		text_area [0] = 0;
		cursor = 0;
	}

	buf_setxy (0, y);
	buf_color (7, 1);
	buf_print_abs (str_divider_line);
	buf_setxy (1, y);
	buf_print_abs ("\xB4 ");
	buf_print_abs (caption);
	buf_print_abs (" \xC3");

	buf_setxy (0, y + max_lines + 1);
	buf_print_abs (str_divider_line);
	buf_setxy (1, y);
	buf_print_abs ("\xB4 ENTER: Insertar \xB3 ESC: Cancelar \xC3");

	while (!done && !buf_heartbeat ()) {
		// Display
		
		// Fast clear
		buf_color (7, 0);

		int y1 = buf_getviewport_y1 ();
		int y2 = buf_getviewport_y2 ();
		buf_setviewport (y + 1, y + max_lines);
		buf_clscroll ();
		buf_setviewport (y1, y2);

		buf_setxy (0, y + 1);
		buf_print_abs (text_area);

		int ycursor = y + 1 + cursor / 80;
		int xcursor = cursor % 80;

		buf_setxy (xcursor, ycursor);
		buf_color (0, 14);
		c = text_area [cursor] ? text_area [cursor] : 32;
		buf_char (c);

		// Get input
		unsigned char const* chars = (unsigned char*) readchars ();
		enum keycode_t* keys = readkeys ();

		// Characters
		c = get_character_input (chars);

		if (c >= ' ') {
			if (cursor < max_chars) {
				text_area [cursor ++] = c;
				text_area [cursor] = 0;
			}
		} else {
			switch (c) {
				case 8:
					// Delete
					if (cursor > 0) {
						cursor --;
						text_area [cursor] = 0;
					}
					break;

				case 27:
					// Exit
					done = 1;
					res = NULL;
					*action = TUI_ACTION_ESC;
					break;
			}
		}

		// Special keys
		while (*keys) {
			unsigned long long key = (unsigned long long) *keys;

			if (key == KEY_UP) {
				cursor -= 80;
			}

			if (key == KEY_DOWN) {
				cursor += 80; 
			}

			if (key == KEY_LEFT) {
				cursor --;
			}

			if (key == KEY_RIGHT) {
				cursor ++;
			}

			if (key == KEY_RETURN) {
				res = text_area;
				done = 1;
				*action = TUI_ACTION_ENTER;
				break;
			}

			keys ++;
		}

		if (cursor < 0) {
			cursor = 0;
		} else if (cursor > strlen (text_area)) {
			cursor = strlen (text_area);
		}
	}

	debuff_keys ();
	return res;
}

void tui_general_box (char *message, char *alt, char symbol) {
	buf_color (7, 1);
	buf_box (20, 8, 59, 16);
	buf_setxy (buf_getscrw () / 2 - strlen (message) / 2, 10);
	buf_print_abs (message);

	if (alt) {
		buf_setxy (buf_getscrw () / 2 - strlen (alt) / 2, 11);
		buf_print_abs (alt);
	}

	buf_color (0, 6);
	buf_box (22, 9, 26, 11);
	buf_setxy (24, 10);
	buf_char (symbol);
}

int tui_button_and_check (int x1, int y1, int x2, int y2, char *caption) {
	// Check if mouse inside
	int mx = buf_get_mouse_x ();
	int my = buf_get_mouse_y ();

	int res = mx >= x1 && mx <= x2 && my >= y1 && my <= y2;

	if (res) {
		buf_color (0, 7);
	} else {
		buf_color (7, 0);
	}

	buf_box (x1, y1, x2, y2);
	buf_setxy (
		x1 + (1 + x2 - x1) / 2 - strlen (caption) / 2,
		y1 + (1 + y2 - y1) / 2
	);
	buf_print_abs (caption);

	return res && buf_get_mouse_b (1);
}

void tui_alert (char *message, char *alt) {
	tui_general_box (message, alt, '!');

	while (
		!tui_button_and_check (35, 13, 44, 15, "\xADOK!") && 
		!buf_heartbeat ()
	) {
		keys_read ();
		if (keys_get_this_frame () & (MT_KEY_ENTER | MT_KEY_ESC)) break;
	}
}

int tui_yes_or_no (char *message, char *alt) {
	int res = 0;

	tui_general_box (message, alt, '?');

	while (!buf_heartbeat ()) {
		int button_yes = tui_button_and_check (30, 13, 37, 15, "[S]\xA1");
		int button_no  = tui_button_and_check (42, 13, 49, 15, "[N]o");

		keys_read ();
		int keys = keys_get_this_frame (); 
		int key_yes = keys & MT_KEY_YES;
		int key_no = keys & MT_KEY_NO;

		if (button_yes || key_yes) {
			res = 1; break;
		}

		if (button_no || key_no) {
			break;
		}
	}

	return res;
}
