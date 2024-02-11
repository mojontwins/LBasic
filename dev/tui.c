#include <string.h>
#include "lstextmode.h"

void tui_general_box (char *message, char symbol) {
	buf_color (7, 1);
	buf_box (20, 8, 59, 15);
	buf_setxy (buf_getscrw () / 2 - strlen (message) / 2, 10);
	buf_print_abs (message);

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

	return res && buf_mouseb (1);
}

void tui_alert (char *message) {
	tui_general_box (message, '!');

	while (!tui_button_and_check (34, 12, 44, 14, "\xADOK!") && !buf_heartbeat ());
}

int tui_yes_or_no (char *message) {
	int res = 0;

	tui_general_box (message, '?');

}
