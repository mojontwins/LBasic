#include "../tui_dev/lstui.h"
#include "lstokens.h"
#include "sys_utils.h"

#define MAX_CONTROLS 20
int control_handles [MAX_CONTROLS];
int control_indexes [MAX_CONTROLS];

#define CONTROL_TYPE_DISPLAY 0
#define CONTROL_TYPE_BYVAL 1
#define CONTROL_TYPE_BYNAME 2
#define CONTROL_TYPE_STRING 3

typedef struct WIZARD_FIELD {
	char caption [40];
	int rx, ry;
	int width;
	int text_width;
	int type;
} WIZARD_FIELD;

WIZARD_FIELD fields_bg [] = {
	{ "bg config", 0, 0, 40, 5, CONTROL_TYPE_DISPLAY },
	{ "X", 0, 0, 2, 2, CONTROL_TYPE_BYVAL },
	{ "Y", 18, 0, 2, 2, CONTROL_TYPE_BYVAL },
	{ "-", 0, 0, 0, 0, 0 }
};

WIZARD_FIELD fields_menu [] = {
	{ "menu config", 0, 0, 40, 9, CONTROL_TYPE_DISPLAY },
	{ "X", 0, 0, 2, 2, CONTROL_TYPE_BYVAL },
	{ "Y", 12, 0, 2, 2, CONTROL_TYPE_BYVAL },
	{ "Width", 24, 0, 2, 2, CONTROL_TYPE_BYVAL },
	{ "Color 1", 0, 2, 2, 2, CONTROL_TYPE_BYVAL },
	{ "Color 2", 18, 2, 2, 2, CONTROL_TYPE_BYVAL },
	{ "fixed", 0, 4, 0, 0, CONTROL_TYPE_BYNAME },
	{ "noframe", 20, 4, 0, 0, CONTROL_TYPE_BYNAME },
	{ "-", 0, 0, 0, 0, 0 } 
};

WIZARD_FIELD fields_talk [] = {
	{ "talk config", 0, 0, 40, 11, CONTROL_TYPE_DISPLAY },
	{ "X", 0, 0, 2, 2, CONTROL_TYPE_BYVAL },
	{ "Y", 12, 0, 2, 2, CONTROL_TYPE_BYVAL },
	{ "Width", 24, 0, 2, 2, CONTROL_TYPE_BYVAL },
	{ "Color 1", 0, 2, 2, 2, CONTROL_TYPE_BYVAL },
	{ "Color 2", 18, 2, 2, 2, CONTROL_TYPE_BYVAL },
	{ "Overlay GIF", 0, 4, 36, 200, CONTROL_TYPE_STRING },
	{ "Ov. X", 0, 6, 2, 2, CONTROL_TYPE_BYVAL },
	{ "Ov. Y", 12, 6, 2, 2, CONTROL_TYPE_BYVAL },
	{ "Mask", 24, 6, 2, 2, CONTROL_TYPE_BYVAL },
	{ "-", 0, 0, 0, 0, 0 }
};

int success;

char *create_form (WIZARD_FIELD fields [], int token_from, char *line) {
	int i = 0;
	int handle_i = 0;
	int token_n = token_from;
	int ox = 0;
	int oy = 0;
	int ow = 0;
	int oh;

	// TODO : PREFILL CONTROLS WITH EXISTING DATA!
	while (fields [i].caption [0] != '-') {
		if (fields [i].type == CONTROL_TYPE_DISPLAY) {
			// Initial box & setup
			ow = fields [i].width;
			oh = fields [i].text_width + 4;
			ox = 40 - ow / 2;
			oy = 12 - oh / 2;

			lstui_add (
				lstui_box (ox, oy, ow, oh, 15, 1, LSTUI_BOX_DOUBLE)
			);

			lstui_add (
				lstui_caption (ox + 1, oy + 1, ow - 2, 14, 0, LSTUI_ALIGN_CENTER, fields [i].caption)
			);

			ox += 2; oy += 3;

		} else if (fields [i].type == CONTROL_TYPE_BYNAME) {
			// Add a checkbox
			control_handles [handle_i] = lstui_add (
				lstui_checkbox (ox + fields [i].rx, oy + fields [i].ry, fields [i].caption)
			);
			control_indexes [handle_i] = i;

			if (strlen (get_token (token_n))) {
				lstui_setstate (control_handles [handle_i], 1);
			}

			handle_i ++; token_n ++;

		} else {
			// Add a text box
			lstui_add (
				lstui_caption (ox + fields [i].rx, oy + fields [i].ry, strlen (fields [i].caption), 15, 1, LSTUI_ALIGN_LEFT, fields [i].caption)
			);

			control_handles [handle_i] = lstui_add (
				lstui_input (ox + fields [i].rx + 1 + strlen (fields [i].caption), oy + fields [i].ry, fields [i].width, fields [i].text_width, get_token (token_n))
			);
			control_indexes [handle_i] = i;
			if (token_n < get_num_tokens ()) {
				lstui_copydata (control_handles [handle_i], get_token (token_n));
			}

			handle_i ++; token_n ++;
		}

		i ++;
	}

	int button_ok = lstui_add (
		lstui_button (ox + 2, oy + oh - 8, 10, 3, "OK")
	);

	int button_cancel = lstui_add (
		lstui_button (ox + ow - 16, oy + oh - 8, 10, 3, "Cancelar")
	);

	int done = 0;
	int rehash_line = 0;

	while (!shuttingdown () && !done) {
		lstui_do ();
		if (lstui_get_signal () & LSTUI_SIGNAL_ESC) { done = 1; }
		if (lstui_getstate (button_cancel) & LSTUI_STATE_CLICKED) { done = 1; }
		if (lstui_getstate (button_ok) & LSTUI_STATE_CLICKED) { done = 1; rehash_line = 1; }

		waitvbl ();
	}

	if (rehash_line) {
		// First trim to header:
		// THIS bit is not working!
		int l = get_index (token_from);
		char *temp_line = strndup (line, l);
		free (line);
		line = temp_line;

		// Make new line
		for (int i = 0; i < handle_i; i ++) {
			int type = fields [control_indexes [i]].type;
			int handle = control_handles [i];
			unsigned char *data = lstui_getdata (handle);

			switch (type) {
				case CONTROL_TYPE_BYVAL:
					// Increase allocated: 1 byte (SPACE) + value length + 1 (null)
					line = realloc (line, strlen (line) + 1 + strlen (data) + 1);
					strcat (line, " ");
					strcat (line, data);
					break;

				case CONTROL_TYPE_BYNAME:
					if (lstui_getstate (handle)) {
						// Increase allocated: 1 byte (SPACE) + name length + 1 (null)
						line = realloc (line, strlen (line) + 1 + strlen (fields [control_indexes [i]].caption) - 1 + 1);
						strcat (line, " ");
						strcat (line, fields [control_indexes [i]].caption + 1);
					}
					break;

				case CONTROL_TYPE_STRING:
					// Increase allocated: 1 byte (SPACE) + value length + 2 (quotes) + 1 (null)
					line = realloc (line, strlen (line) + 1 + strlen (data) + 2 + 1);
					strcat (line, " \"");
					strcat (line, data);
					strcat (line, "\"");
					break;
			}
		}
	} 

	return line;
}

char *wizards_bg (char *line) {
	line = create_form (fields_bg, 2, line);
	return line;
}

char *wizards_menu (char *line) {
	line = create_form (fields_menu, 2, line);
	return line;
}

char *wizards_talk (char *line) {
	line = create_form (fields_talk, 2, line);
	return line;
}

char *wizards_tb (char *line) {
	return line;
}

char *wizards_resp (char *line) {
	return line;
}

char *wizards_interface (char *line) {
	return line;
}

char *wizards_infobar (char *line) {
	return line;
}

char *wizards_indicator (char *line) {
	return line;
}

char *wizards_zones (char *line) {
	return line;
}

char *wizards_parse (char *line) {
	success = 0;
	lstui_reset ();

	parse_to_tokens (line);
	char *cmd = get_token (0);
	utils_tolower (cmd);
	char *arg = get_token (1);
	utils_tolower (arg);

	// Find which wizard should we fire up
	if (strcmp (arg, "config") == 0) {
		if (strcmp (cmd, "bg") == 0) {
			line = wizards_bg (line);
			success = 1;
		
		} else if (strcmp (cmd, "menu") == 0) {
			line = wizards_menu (line);
			success = 1;

		} else if (strcmp (cmd, "talk") == 0) {
			line = wizards_talk (line);
			success = 1;

		} else if (strcmp (cmd, "tb") == 0) {
			line = wizards_tb (line);
			success = 1;

		} else if (strcmp (cmd, "resp") == 0) {
			line = wizards_resp (line);
			success = 1;

		} else if (strcmp (cmd, "interface") == 0) {
			line = wizards_interface (line);
			success = 1;

		}
	} else if (strcmp (cmd, "infobar") == 0) {
		line = wizards_infobar (line);
		success = 1;

	} else if (strcmp (cmd, "indicator") == 0) {
		line = wizards_indicator (line);
		success = 1;

	} else if (strcmp (cmd, "zones") == 0 && strcmp (arg, "def") == 0) {
		line = wizards_zones (line);
		success = 1;

	}

	lstui_shutdown ();

	return line;
}

int wizards_succeeded (void) {
	return success;
}
