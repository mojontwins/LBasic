// Lbasic 6 remake

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef DOSLIKE
#include "../dos-like/source/dos.h"
#endif

#include "lstextmode.h"
#include "interpreter.h"

int menu_x [3] = { 0, 11, 24 };

char menu_opt1 [] = "  Editar  ";
char menu_opt2 [] = "  Corregir  ";
char menu_opt3 [] = "  Ejecutar un programa  ";

char *menu_opt [] = { menu_opt1, menu_opt2, menu_opt3 };

char menu_help1 [] = " Para hacer un programa                  ";
char menu_help2 [] = " Para hacer correcciones en un programa  ";
char menu_help3 [] = " Para jugar a un programa que hayas hecho";

char *menu_help [] = { menu_help1, menu_help2, menu_help3 };

char **editor_lines;
int editor_n_lines = 0; 	// Total number of lines allocated
int editor_current_line;	// Line being edited
int editor_last_line; 		// Last line in program

#define LINE_BUFFER_SIZE 2048

void splash_screen_1 (void) {
	buf_color (7, 0);
	buf_cls ();
	
	buf_color (7, 1);
	buf_setxy (0, 24);
	buf_print_abs ("  Pulsa una tecla para comenzar \xB3 Introduce ' men\xA3 ' siempre que quieras usarlo ");
	buf_setxy (0, 0);
	buf_print_abs ("  Compilador Lokosoft LBasic versi\xA2n 6.03. (C)opyright 1994 by Loko Soft        ");
	buf_color (7, 0);

	buf_pause ();
}

int menu (void) {
	int menu_on = 1;
	int key_return = 1;
	int option = 0;
	int option_old = 0xff;

	buf_color (7, 1);
	buf_setxy (0, 24);
	buf_print_abs ("                                                                                ");

	buf_setxy (0, 0);
	buf_print_abs ("  Editar  \xB3  Corregir  \xB3  Ejecutar un programa  \xB3      (C) Versi\xA2n 6.03, 1994   ");

	while (menu_on) {
		if (option_old != option) {
			if (option_old != 0xff) {
				buf_color (7, 1);
				buf_setxy (menu_x [option_old], 0);
				buf_print_abs (menu_opt [option_old]);
			}

			buf_color (9, 7);
			buf_setxy (menu_x [option], 0);
			buf_print_abs (menu_opt [option]);

			buf_color (7, 1);
			buf_setxy (0, 24);
			buf_print_abs (menu_help [option]);

			option_old = option;
		}

		// Read cursors

		enum keycode_t *keys = readkeys();
		int old_key = 0;
		
		while (*keys) {
			unsigned long long key = (unsigned long long) *keys;

			if (key == KEY_LEFT) {
				option --; if (option < 0) option = 2;
				old_key = key;
			} 
			if (key == KEY_RIGHT) {
				option ++; if (option > 2) option = 0;
				old_key = key;
			}
			if (key == KEY_RETURN) {
				if (key_return == 0) menu_on = 0;
			} else {
				key_return = 0;
			}
			if (key == KEY_ESCAPE) {
				option = 4; menu_on = 0;
			}

			++keys;
		}

		// Wait for depress
		if (old_key) {
			int pressed = 1;
			while (pressed) {
				pressed = 0;
				keys = readkeys(); while (*keys) {
					unsigned long long key = (unsigned long long) *keys;
					if (key == old_key) {
						pressed = 1;
					}
				}
			}
		}

		waitvbl ();
	}
}

char *lines_add_new (void) {
	editor_n_lines ++;
	editor_lines = (char **) realloc (editor_lines, editor_n_lines * sizeof (char *));
	return editor_lines [editor_n_lines - 1];
}

void lines_free_all (void) {
	for (int i = 0; i < editor_n_lines; i ++) {
		free (editor_lines [i]);
	}

	free (editor_lines);
}

void lines_read_from_file (FILE *file) {
	char line_buffer [LINE_BUFFER_SIZE];
	int lines_read = 0;	

	while (fgets (line_buffer, LINE_BUFFER_SIZE, file) != NULL) {
		lines_read ++;
		if (lines_read > editor_n_lines) {
			char *line_pointer = lines_add_new ();
			line_pointer = strdup (line_buffer);
		}
	}

	editor_current_line = editor_n_lines;
}

int editor (void) {
	// File is already loaded.
	int line_change = 0;
	int new_line = 1;
	int line_length;
	int editing = 1;
	int cursor = 0;
	char *line_pointer = NULL;
	char c;
	int cur_line_rendered;

	int x = 0; 
	int y = 1;
	int i;
	int editor_next_line = editor_n_lines;

	int first_line_to_display = 0;

	while (editing) {

		if (new_line) {
			// Add new line.
			// - If editor_last_line + 1 == editor_n_lines allocate more lines 
			if (editor_last_line + 1 == editor_n_lines) lines_add_new ();

			// Advance to the next line
			editor_current_line ++;
			editor_last_line ++;

			// Now shuffle all pointers to make room for the new line
			for (int i = editor_last_line; i > editor_current_line; i --) {
				editor_lines [i] = editor_lines [i - 1];
			}

			// Create new, blank line
			char *blank_line = (char *) malloc (1 * sizeof (char));
			blank_line [0] = '\0';
			cursor = 0;

			// Add to lines
			editor_lines [editor_current_line] = blank_line;

			new_line = 0;
		} 

		if (line_change) {
			// Scroll display up is trivial
			if(editor_next_line < first_line_to_display) {
				first_line_to_display = editor_next_line;
			}

			// Scroll down is more tricky ... done elsehwere?

			editor_current_line = editor_next_line;


			line_change = 0;
		}

		line_pointer = editor_lines [editor_current_line];
		if (cursor > strlen (line_pointer)) cursor = strlen (line_pointer);

		// Read keys and write to line.
		unsigned char const* chars = (unsigned char*) readchars ();
		enum keycode_t* keys = readkeys ();

		// Characters
		while (c = *chars) {
			if (c >= ' ') {
				// Insert a new character (end / middle)
				line_length = strlen (line_pointer);
				
				// Increment buffer size, move right part right 1 char, insert

				line_pointer = realloc (line_pointer, (line_length + 2) * sizeof (char));
				editor_lines [editor_current_line] = line_pointer;

				for (int i = line_length; i >= cursor; i --) {
					line_pointer [i + 1] = line_pointer [i];
				}

				line_pointer [cursor] = c;
				cursor ++;			
			}

			chars ++;
		}

		// Special keys.
		while (*keys) {
			unsigned long long key = (unsigned long long) *keys;

			if (key == KEY_LEFT) {
				if (cursor > 0) cursor --;
			}
			if (key == KEY_RIGHT) {
				if (cursor < strlen (line_pointer)) cursor ++;
			}
			if (key == KEY_UP) {
				if (editor_current_line > 0) {
					editor_next_line = editor_current_line - 1;
					line_change = 1;
				}
			}
			if (key == KEY_DOWN) {
				if (editor_current_line < editor_last_line) {
					editor_next_line = editor_current_line + 1;
					line_change = 1;
				}
			}
			if (key == KEY_RETURN) {
				editor_next_line = editor_current_line + 1;
				new_line = 1;
			}

			++keys;
		}

		// Display
		y = 1;
		i = 0;
		cur_line_rendered = 0;

		buf_color (7, 0);
		buf_clscroll ();
		while (1) {
			int line_no = first_line_to_display + i;
			int bkg = line_no & 1 ? 8 : 0;

			if (line_no > editor_last_line) break;
			
			buf_setxy (0, y);
			buf_color (7, bkg);
			buf_print_abs (editor_lines [line_no]);

			// Draw cursor?
			if (line_no == editor_current_line) {
				int ycursor = y + cursor / 80;

				if (ycursor < 24) {
					cur_line_rendered = 1;

					c = line_pointer [cursor]; if (c == 0) c = ' ';
					buf_setxy (cursor % 80, ycursor);
					buf_color (0, 14);
					buf_char (c);
				}
			}

			y += 1 + strlen (editor_lines [line_no]) / 80;

			if (y > 23) {
				if (!cur_line_rendered) first_line_to_display ++;

				break;
			}

			i ++;
		}

		waitvbl ();
	}
}

void main (char argc, char *argv []) {
	buf_setviewport (1, 23);
	splash_screen_1 ();

	editor_current_line = -1;
	editor_last_line = -1;
	editor_n_lines = 0;

	while (menu () != 4) {
		// MEH
		editor ();
	}
}
