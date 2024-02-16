#include <string.h>

#include "tui.h"
#include "keys.h"
#include "lstextmode.h"
#include "conversion.h"

#include "../dos-like/source/dos.h"

// A pointer to this array will be returned but it should be
// memcopied / strcpy'ed to elsewhere!
unsigned char text_area [512];
char str_divider_line [] = "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4";

char *tui_textbox (int y, char *caption, char *org_text, int max_lines, int *action) {
	int cursor;
	int max_chars = 80 * max_lines - 1;
	int done = 0;
	unsigned char c;
	char *res;

	if (org_text) {
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
