//#define DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lbasi.h"
#include "backend.h"
#include "sys_utils.h"

int forced_break = 0;

void backend_init (void) {
	printf ("LBASI - Backend simple - basic I/O\n");
}

int backend_get_break (void) {
	return forced_break;
}

char *compute_full_path (char *pathspec, char *filename) {
	char *full_path = strdup (pathspec);
	strcat (full_path, filename);
	return full_path;
}

void backend_set_viewport (int l1, int l2) {
	// NO OP
}

void backend_set_show_status (int i) {
	// NO OP
}

void backend_statusbar (int clr_statusbar1, int clr_statusbar2, char *string_top, char *string_bottom, int attempts) {
	// NO OP
}

void backend_print_ln (char *string) {
	printf ("%s\n", string);
}

void backend_print (char *string) {
	printf (string);
}

void backend_print_ghost (int x, int y, int w, int c1, int c2, char *string) {
	// Almost NO OP
	backend_print (string);
}

void backend_setxy (int x, int y) {
	// NO OP
}

void backend_center (char *string) {
	#ifdef DEBUG
		printf ("[%s]\n", string);
	#else
		backend_print (string);
	#endif
}

void backend_color (int c1, int c2) {
	#ifdef DEBUG
		printf ("COLOR %d %d\n", c1, c2);
	#endif
}

void backend_pause (void) {
	#ifdef DEBUG
		printf ("PAUSE\n");
	#endif
}

void backend_beep (void) {
	#ifdef DEBUG
		printf ("DEBUG\n");
	#endif
}

void backend_cls (void) {
	#ifdef DEBUG
		printf ("CLS\n");
	#endif	
}

void backend_draw (char *string) {
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
			case '=':
			case '!':
			case 34:
			case '$':
			case '%':
			case '&':
			case 0xFA:
			case '/':
				break;

			default:
				printf ("%c", c);
				break;
		}
	}

	printf ("\n");
}

unsigned char backend_read_option (int num_choices) {
	int input; 
	char c; 

	while (1) {
		printf (">");
		if (scanf (" %d", &input) != 1) {
			// Discard buffer
			while ((c = getchar ()) != '\n' && c != EOF);
			if (c == EOF) break;
		}

		if (input == 99) {
			forced_break = 1;
			return 99;
		}

		if (num_choices == 0 || (input >= 1 && input <= num_choices)) {
			return input;
		}
	}
}

unsigned char backend_choice (int num_choices, int correct, char **choices) {
	int input;
	char c;
	int input_numeric;

	#ifdef DEBUG
		printf ("[%d:%d]", num_choices, correct);
	#endif

	input = backend_read_option (num_choices);

	#ifdef DEBUG
		printf ("[%d]", input);
	#endif

	if (input <= num_choices) backend_print (choices [input + 2]);
	return input == correct;

}

void backend_set_margins (int col1, int col2) {
	// NO OP
}

void backend_wordwrap (char *s, int meh) {
	// ALMOST NO OP
	backend_print_ln (s);
}

void backend_try_again (char *string_try_again, int attempts) {
	printf (string_try_again, attempts);
	printf ("\n");
}

int backend_heartbeat (void) {
	// NO ÔP
	return 0;
}

void backend_shutdown (void) {
	// NO OP
}

void backend_gif_at (char *pathspec, char *gif, int x, int y, int load_pal) {
	// NO OP
}

void backend_wait_frames (int frames) {
	// NO OP
}

void backend_ansibin (char *pathspec, char *filename) {
	char *fullpath = compute_full_path (pathspec, filename);

	char buffer [80*25*2];
	FILE *pf = fopen (fullpath, "rb");
	fread (buffer, sizeof (char), 80*25*2, pf);
	fclose (pf);

	char *pbuffer = buffer;
	for (int y = 0; y < 25; y ++) {
		for (int x = 0; x < 80; x ++) {
			printf ("%c", *pbuffer ++); 		// Print char
			pbuffer ++; 						// Skip color
		}
		printf ("\n");
	}

	free (fullpath);
}

void backend_set_mode (char *mode) {
	// NO OP
}

void backend_menu_config (int x, int y, int w, int c1, int c2) {
	// NO OP
}

void backend_menu_set_selected (int selected) {
	// NO OP
}

int backend_menu_run (void) {
	// Very simple implementation
	printf ("MENU OPTIONS: %d\n", menu_get_options ());
	for (int i = 0; i < menu_get_options (); i ++) {
		printf ("%d. %s\n", i + 1, menu_get_option_text (i));		
	}

	int res = backend_read_option (10);
	if (res == 99) return -1;
	return res - 1;
}

int backend_inventory_run (void) {
	// Very simple implementation
	printf ("INVENTORY ITEMS: %d\n", inventory_get_items ());
	for (int i = 0; i < inventory_get_items (); i ++) {
		printf ("%d. %s\n", i + 1, inventory_get_item (i));
	}

	int res = backend_read_option (10);
	if (res == 99) return -1;
	return res - 1;
}

int backend_exits_run (void) {
	// Very simple implementation
	printf ("EXITS: %d\n", exits_get_options ());
	for (int i = 0; i < exits_get_options (); i ++) {
		printf ("%d. %s\n", i + 1, exits_get_option_text (i));
	}

	int res = backend_read_option (10);
	if (res == 99) return -1;
	return res - 1;
}

void backend_talk_config (int x, int y, int w, int c1, int c2) {
	// NO OP
}

void backend_talk (char *s) {
	backend_print (s);
}

void backend_bulma_lin (char *pathspec, char *lin) {
	// NO OP
}

void backend_bulma_pix (char *pathspec, char *pix, int dbl, int load_pal) {
	// NO OP
}

void backend_fancy_font (char *f) {
	// NO OP
}
