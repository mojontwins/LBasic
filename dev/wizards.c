#include "../tui_dev/lstui.h"
#include "lstokens.h"

typedef struct WIZARD_FIELD {
	char caption [40];
	int width;
	int text_width;
} WIZARD_FIELD;

WIZARD_FIELD fields_bg [] = {
	{ "X", 2, 2 },
	{ "Y", 2, 2 },
	{ NULL, 0, 0 }
};

WIZARD_FIELD fields_menu [] = {
	{ "X", 2, 2 },
	{ "Y", 2, 2 },
	{ "Width", 2, 2 },
	{ "--", 0, 0 },
	{ "Color 1", 2, 2 },
	{ "Color 2", 2, 2 },
	{ "--", 0, 0 },
	{ "Fixed?", 0, 0 },
	{ "Noframe?", 0, 0 },
	{ NULL, 0, 0 } 
};

WIZARD_FIELD fields_talk [] = {
	{ "X", 2, 2 },
	{ "Y", 2, 2 },
	{ "Width", 2, 2 },
	{ "--", 0, 0 },
	{ "Color 1", 2, 2 },
	{ "Color 2", 2, 2 },
	{ "--", 0, 0 },
	{ "Overlay GIF", 40, 200 },
	{ "--", 0, 0 },
	{ "Ov. X", 2, 2 },
	{ "Ov. Y", 2, 2 },
	{ "Mask", 2, 2 },
	{ NULL, 0, 0 }
};

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
