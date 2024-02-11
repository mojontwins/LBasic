#include <string.h>
#include "lstextmode.h"

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
		!buf_get_keystate (BUF_KEY_ENTER | BUF_KEY_ESC) &&
		!buf_heartbeat ()
	);

	while (!buf_heartbeat () && buf_get_keystate (BUF_KEY_ENTER | BUF_KEY_ESC));
}

int tui_yes_or_no (char *message, char *alt) {
	int res = 0;

	tui_general_box (message, alt, '?');

	while (!buf_heartbeat ()) {
		int button_yes = tui_button_and_check (30, 13, 37, 15, "[S]\xA1");
		int button_no  = tui_button_and_check (42, 13, 49, 15, "[N]o");

		if (button_yes) {
			res = 1; break;
		}

		if (button_no) {
			break;
		}

/*
		if (buf_get_keystate (BUF_KEY_YES) || button_yes) {
			res = 1; break;
		}

		if (buf_get_keystate (BUF_KEY_NO) || button_no) {
			res = 0; break;
		}
		*/
	}

	return res;
}
