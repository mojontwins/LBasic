//#define DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lbasi.h"
#include "backend.h"
#include "lstextmode.h"
#include "sys_utils.h"

#include "keys.h"
#include "sys_utils.h"

#include "../dos-like/source/dos.h"

#define INPUT_LENGTH 10

char *full_line = "                                                                                ";
int show_status = 1;
int forced_break = 0;

int backend_menu_x = 0;
int backend_menu_y = 0;
int backend_menu_c1 = 15;
int backend_menu_c2 = 1;
int backend_menu_w = 16;
int backend_menu_selected = 0;

void backend_init (void) {
	lstextmode_init ();
	buf_color (7, 0);
	buf_clscroll ();
}

int backend_get_break (void) {
	return forced_break;
}

char *compute_full_path (char *pathspec, char *filename) {
	char *full_path = strdup (pathspec);
	strcat (full_path, filename);
	return full_path;
}

void backend_set_show_status (int i) {
	show_status = i;
}

void backend_statusbar (int clr_statusbar1, int clr_statusbar2, char *string_top, char *string_bottom, int attempts) {
	if (buf_getmode () == LS_MODE_TEXT && show_status) {
		int x, y;
		int c1, c2;
		char attempts_str [3];

		x = buf_getx (); y = buf_gety ();
		c1 = buf_getc1 (); c2 = buf_getc2 ();

		buf_setxy (0, 0);
		buf_color (clr_statusbar1, clr_statusbar2);
		buf_print_abs (full_line);
		
		buf_setxy (0, 0);
		buf_print_abs (string_top);

		buf_setxy (0, 24);
		buf_print_abs (full_line);
		
		buf_setxy (0, 24);
		buf_print_abs (string_bottom);
		sprintf (attempts_str, "%1d", attempts);
		buf_print_abs (attempts_str);

		buf_setxy (x, y);
		buf_color (c1, c2);
	}	
}

void backend_set_viewport (int l1, int l2) {
	buf_setviewport (l1, l2);
}

void backend_print_ln (char *string) {
	buf_print_ln (string);
}

void backend_print (char *string) {
	buf_print (string);
}

void backend_setxy (int x, int y) {
	buf_setxy (x, y);
}

void backend_center (char *string) {
	buf_setx (buf_getscrw () / 2 - strlen (string) / 2);
	buf_print_ln (string);
}

void backend_color (int c1, int c2) {
	buf_color (c1, c2);
}

void backend_pause (void) {
	while (!backend_heartbeat ()) {
		int c = *readchars ();
		if (c == 0) break;
	}

	while (!backend_heartbeat ()) {
		int c = *readchars ();
		if (c > 0) break;
	}
}

void backend_beep (void) {

}

void backend_cls (void) {
	buf_clscroll ();
	buf_resetxy ();
}

void backend_draw (char *string) {
	int c1 = buf_getc1 ();
	int c2 = buf_getc2 ();

	int oc1 = c1;
	int oc2 = c2;

	char c; while (c = *string ++) {
		switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				c1 = c - '0';
				buf_color (c1, c2);
				break;

			case '=':
				c2 = 0;
				buf_color (c1, c2);
				break;

			case '!':
			case 34:
			case '$':
			case '%':
			case '&':
				c2 = c - 32;
				buf_color (c1, c2);
				break;

			case 0xFA:
				c2 = 3;
				buf_color (c1, c2);
				break;

			case '/':
				c2 = 7;
				buf_color (c1, c2); 
				break;

			default:
				buf_char (c);
				break;
		}
	}

	buf_color (oc1, oc2);
	buf_print_ln ("");
}

unsigned char backend_choice (int num_choices, int correct, char **choices) {
	int input;

	while (!backend_heartbeat ()) {
		input = backend_read_option (num_choices);
		if (input <= num_choices) backend_print_ln (choices [input + 2]);
		return input == correct;
	}
}

void backend_try_again (char *string_try_again, int attempts) {
	char str [256];
	sprintf (str, string_try_again, attempts);
	buf_print_ln (str);
}

unsigned char backend_read_option (int num_choices) {
	int res = 0;
	int len = 0;
	int x = buf_getx ();

	char input [INPUT_LENGTH + 1];
	memset (input, 0, INPUT_LENGTH + 1);

	debuff_keys ();

	while (1) {
		int c = *readchars ();
		while (!backend_heartbeat () && c != '\r' && len + 1 < INPUT_LENGTH) {
			buf_setx (x);
			buf_print (">");
			buf_print (input);
			buf_print ("_ ");

			if (c >= '0' && c <= '9') {
				input [len ++] = c;
			} else {
				switch( c >> 8 ) {
					case 0x08: 
						// Backspace
						if( len > 0 ) {
							--len; 
							input [len] = 0;
						}
						break;
				}
			}

			c = *readchars ();
			if (c == '\0') {
				if (*readkeys () == KEY_BACK) {
					c = 0x08 << 8;
				}
			}
		}

		int input_i = atoi (input);

		if (input_i == 99) {
			forced_break = 1;
			return 99;
		}

		if (num_choices == 0 || (input_i >= 1 && input_i <= num_choices)) {
			buf_setx (x);
			buf_print (">");
			buf_print (input);
			buf_print_ln (" ");

			return input_i;
		} else {
			buf_setx (x);
			buf_print (">");
			for (int i = 0; i < strlen (input) + 1; i ++) {
				buf_print (" ");
			}
		}
	}

	
}

int backend_heartbeat (void) {
	waitvbl ();

	return shuttingdown ();
}

void backend_gif_at (char *pathspec, char *gif, int x, int y, int load_pal) {
	char *fullpath = compute_full_path (pathspec, gif);

	if (buf_getmode () != LS_MODE_TEXT) {
		buf_gif_at (fullpath, x, y, load_pal);
	}

	free (fullpath);
}

void backend_bulma_lin (char *pathspec, char *lin) {
	char *fullpath = compute_full_path (pathspec, lin);

	if (buf_getmode () != LS_MODE_TEXT) {
		buf_bulma_lin (fullpath);
	}

	free (fullpath);
}

void backend_wait_frames (int frames) {
	while (!backend_heartbeat () && frames --) {
		int c = *readchars ();
		if (c > 0) break;
	}
}

void backend_set_margins (int col1, int col2) {
	buf_setmargins (col1, col2);
}

void backend_wordwrap (char *s, int char_by_char) {
	if (char_by_char) {
		buf_wordwrap_char_by_char (s);
	} else {
		buf_wordwrap (s);
	}
}

void backend_set_mode (char *mode) {
	if (strcmp (mode, "text") == 0) {
		buf_setmode (LS_MODE_TEXT);
	} else if (strcmp (mode, "gfx") == 0) {
		buf_setmode (LS_MODE_GFX);
	} else if (strcmp (mode, "gfx_sq") == 0) {
		buf_setmode (LS_MODE_GFX_SQ);
	} else if (strcmp (mode, "gfx_med") == 0) {
		buf_setmode (LS_MODE_GFX_MED);
	} else if (strcmp (mode, "gfx_hi") == 0) {
		buf_setmode (LS_MODE_GFX_HIRES);
	}
}

void backend_ansibin (char *pathspec, char *filename) {
	char *fullpath = compute_full_path (pathspec, filename);

	buf_textmode_pic (fullpath);

	free (fullpath);
}

void backend_shutdown (void) {
	
}

void backend_menu_config (int x, int y, int w, int c1, int c2) {
	backend_menu_x = x;
	backend_menu_y = y;
	backend_menu_w = w;
	backend_menu_c1 = c1;
	backend_menu_c2 = c2;
}

void backend_menu_set_selected (int selected) {
	backend_menu_selected = selected;
}

int backend_menu_run (void) {
	int done = 0;
	int backend_menu_options = menu_get_options ();
	if (backend_menu_selected >= backend_menu_options) backend_menu_selected = backend_menu_options - 1;

	// Paint box
	buf_color (backend_menu_c1, backend_menu_c2);
	buf_box (backend_menu_x, backend_menu_y, backend_menu_x + backend_menu_w - 1, backend_menu_y + 1 + backend_menu_options);

	keys_read ();
	while (!done && !buf_heartbeat ()) {
		// Paint options (w/selected)
		for (int i = 0; i < backend_menu_options; i ++) {
			buf_setxy (backend_menu_x + 1, backend_menu_y + 1 + i);
			if (i == backend_menu_selected) {
				buf_color (backend_menu_c2, backend_menu_c1);
			} else {
				buf_color (backend_menu_c1, backend_menu_c2);
			}
			
			unsigned char *option = menu_get_option (i);
			int l = strlen (option);
			for (int j = 0; j < backend_menu_w - 2; j ++) {
				buf_char (j < l ? option [j] : ' ');
			}
		}

		// Read keys
		keys_read ();
		int pad_this_frame = keys_get_this_frame ();

		if (pad_this_frame & MT_KEY_UP) {
			if (backend_menu_selected > 0) backend_menu_selected --;
		}

		if (pad_this_frame & MT_KEY_DOWN) {
			if (backend_menu_selected < backend_menu_options - 1) backend_menu_selected ++;
		}

		if (pad_this_frame & MT_KEY_ENTER) {
			done = 1;
		}

		if (pad_this_frame & MT_KEY_ESC) {
			done = 1;
			backend_menu_selected = -1;
		}

	}

	debuff_keys ();

	return backend_menu_selected;
}
