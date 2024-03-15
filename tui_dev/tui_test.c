#define LSTUI_CONTROL_IMPLEMENTATION
#include "lstui.h"

void main (void) {
	lstui_reset ();
	int button1 = lstui_add (lstui_button (10, 10, 10, 3, "But Red"));
	int button2 = lstui_add (lstui_button (20, 20, 20, 3, "Buton Blu"));
	int button3 = lstui_add (lstui_button (50, 10, 10, 3, "Third"));

	while (!shuttingdown ()) {
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
