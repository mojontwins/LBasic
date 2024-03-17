#define LSTUI_CONTROL_IMPLEMENTATION
#include "lstui.h"

void main (void) {
	char buffer [80];
	char test [80];

	lstui_reset ();
	int bg = lstui_add (lstui_box (0, 0, 80, 24, 15, 1, LSTUI_BOX_SINGLE));
	int button1 = lstui_add (lstui_button (10, 10, 10, 3, "But Red"));
	int button2 = lstui_add (lstui_button (20, 20, 20, 3, "Buton Blu"));
	int button3 = lstui_add (lstui_button (50, 10, 10, 3, "Third"));
	int caption = lstui_add (lstui_caption (1, 1, 78, 14, 0, LSTUI_ALIGN_CENTER, "Esto va a salir centrado!"));
	int textbox = lstui_add (lstui_input (5, 5, 20, 60, "edit"));
	int textbox1 = lstui_add (lstui_input (5, 9, 10, 10, "otro"));
	int textbox2 = lstui_add (lstui_input (24, 9, 10, 10, ""));
	int caption2 = lstui_add (lstui_caption (5, 7, 60, 10, 1, LSTUI_ALIGN_LEFT, ""));
	int caption3 = lstui_add (lstui_caption (0, 24, 80, 15, 4, LSTUI_ALIGN_LEFT, buffer));
	int checkbox = lstui_add (lstui_checkbox (5, 15, "Hasme el clik"));

	lstui_setdata (caption2, lstui_getdata (textbox));
	char *pcaption3 = lstui_getdata (caption3);

	while (!shuttingdown ()) {
		itoa (lstui_getstate (textbox), pcaption3, 10);

		lstui_do ();

		if (lstui_getstate (button1) & LSTUI_STATE_CLICKED) {
			setpal (0, 63, 0, 0);
		}

		if (lstui_getstate (button2) & LSTUI_STATE_CLICKED) {
			setpal (0, 0, 0, 63);
		}

		if (lstui_getstate (button3) & LSTUI_STATE_CLICKED) {
			setpal (0, 0, 63, 0);
		}

		waitvbl ();
	}

	lstui_shutdown ();
} 
