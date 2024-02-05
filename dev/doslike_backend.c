//#define DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lbasi.h"
#include "backend.h"
#include "lstextmode.h"
#include "../dos-like/source/dos.h"

#define INPUT_LENGTH 10

char *full_line = "                                                                                ";
int show_status = 1;

void backend_init (void) {
	lstextmode_init ();
	buf_color (7, 0);
	buf_clscroll ();
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
		buf_printabs (full_line);
		
		buf_setxy (0, 0);
		buf_printabs (string_top);

		buf_setxy (0, 24);
		buf_printabs (full_line);
		
		buf_setxy (0, 24);
		buf_printabs (string_bottom);
		sprintf (attempts_str, "%1d", attempts);
		buf_printabs (attempts_str);

		buf_setxy (x, y);
		buf_color (c1, c2);
	}	
}

void backend_print_ln (char *string) {
	buf_print_ln (string);
}

void backend_print (char *string) {
	buf_print (string);
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
		if (c > 0) break;
	}
}

void backend_beep (void) {

}

void backend_cls (void) {
	buf_clscroll ();
}

void backend_draw (char *string) {
	// TODO: do this properly
	backend_print (string);
}

unsigned char backend_choice (int num_choices, int correct, char **choices) {
	int input;

	while (!backend_heartbeat ()) {
		input = backend_read_option (num_choices);
		backend_print (choices [input + 2]);
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
	int y = buf_gety ();
	char input [INPUT_LENGTH + 1];
	memset (input, 0, INPUT_LENGTH + 1);

	while (1) {
		int c = *readchars ();
		while (!backend_heartbeat () && c != '\r' && len + 1 < INPUT_LENGTH) {
			buf_setxy (x, y);
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

		if (num_choices == 0 || (input_i >= 1 && input_i <= num_choices)) {
			buf_setxy (x, y);
			buf_print (">");
			buf_print (input);
			buf_print_ln (" ");

			return input_i;
		} else {
			buf_setxy (x, y);
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

void backend_gif_at (char *gif, int x, int y, int load_pal) {
	if (buf_getmode () != LS_MODE_TEXT) {
		buf_gif_at (gif, x, y, load_pal);
	}
}

void backend_wait_frames (int frames) {
	while (!backend_heartbeat () && frames --);
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

void backend_shutdown (void) {
	
}
