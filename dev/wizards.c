#include "../tui_dev/lstui.h"
#include "lstokens.h"

#define MAX_CONTROLS 20
int control_handles [MAX_CONTROLS];

typedef struct WIZARD_FIELD {
	char caption [40];
	int rx, ry;
	int width;
	int text_width;
} WIZARD_FIELD;

WIZARD_FIELD fields_bg [] = {
	{ "#bg config", 0, 0, 40, 5 },
	{ "X", 0, 0, 2, 2 },
	{ "Y", 10, 0, 2, 2 },
	{ NULL, 0, 0 }
};

WIZARD_FIELD fields_menu [] = {
	{ "#menu config", 0, 0, 40, 9 },
	{ "X", 0, 0, 2, 2 },
	{ "Y", 8, 0, 2, 2 },
	{ "Width", 16, 0, 2, 2 },
	{ "Color 1", 0, 2, 2, 2 },
	{ "Color 2", 10, 2, 2, 2 },
	{ "?fixed", 0, 4, 0, 0 },
	{ "?noframe", 20, 4, 0, 0 },
	{ NULL, 0, 0 } 
};

WIZARD_FIELD fields_talk [] = {
	{ "#talk config", 0, 0, 40, 11 },
	{ "X", 0, 0, 2, 2 },
	{ "Y", 8, 0, 2, 2 },
	{ "Width", 16, 0, 2, 2 },
	{ "Color 1", 0, 2, 2, 2 },
	{ "Color 2", 10, 2, 2, 2 },
	{ "Overlay GIF", 0, 4, 36, 200 },
	{ "Ov. X", 0, 6, 2, 2 },
	{ "Ov. Y", 8, 6, 2, 2 },
	{ "Mask", 16, 6, 2, 2 },
	{ NULL, 0, 0 }
};

void create_form (WIZARD_FIELD fields) {
	int i = 0;
	int ox = oy = 0;
	while (fields [i].caption != NULL) {
		int handle = -1;

		if (fields [i].caption [0] == '#') {
			// Initial box & setup
			ox = 40 - fields [i].width / 2;
			oy = 12 - fields [i].text_width / 2;

			control_handles [i ++] = lstui_add (
				lstui_box (ox, oy, fields [i].width, fields [i].text_width, 15, 1, LSTUI_BOX_DOUBLE)
			);

			control_handles [i ++] = lstui_add (
				lstui_caption (ox + 1, oy + 1, fields [i].width - 2, 14, 0, LSTUI_ALIGN_CENTER, fields [i].caption + 1);
			);

		} else if (fields [i].caption [0] == '?') {
			// Add a checkbox
			handle = lstui_add (
				lstui_
			);

		} else {
			// Add a text box
		}

	}
}

char *wizards_bg (char *line) {
	return line;
}

char *wizards_menu (char *line) {
	return line;
}

char *wizards_talk (char *line) {
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

char *wizards_parse (*line) {
	parse_to_tokens (line_buffer);
	char *cmd = get_token (0);
	utils_tolower (cmd);
	char *arg = get_token (1);
	utils_tolower (arg);

	// Find which wizard should we fire up
	if (strcmp (arg, "config") == 0) {
		if (strcmp (cmd, "bg") == 0) {
			return wizards_bg (line);
		
		} else if (strcmp (cmd, "menu") == 0) {
			return wizards_menu (line);

		} else if (strcmp (cmd, "talk") == 0) {
			return wizards_talk (line);

		} else if (strcmp (cmd, "tb") == 0) {
			return wizards_tb (line);

		} else if (strcmp (cmd, "resp") == 0) {
			return wizards_resp (line);

		} else if (strcmp (cmd, "interface") == 0) {
			return wizards_interface (line);

		}
	} else if (strcmp (cmd, "infobar") == 0) {
		return wizards_infobar (line);

	} else if (strcmp (cmd, "indicator") == 0) {
		return wizards_indicator (line);

	} else if (strcmp (cmd, "zones") == 0 && strcmp (arg, "def") == 0) {
		return wizards_zones (line);

	}

	return line;
}
