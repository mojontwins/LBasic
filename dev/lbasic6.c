// Lbasic 6 remake

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef DOSLIKE
#include "../dos-like/source/dos.h"
#endif

#include "lstextmode.h"
#include "interpreter.h"

int menu_x [3] = { 1, 12, 25 };

char menu_opt1 [] = "  Editar  ";
char menu_opt2 [] = "  Corregir  ";
char menu_opt3 [] = "  Ejecutar un programa  ";

char *menu_opt [] = { menu_opt1, menu_opt2, menu_opt3 };

char menu_help1 [] = "Para hacer un programa                  ";
char menu_help2 [] = "Para hacer correcciones en un programa  ";
char menu_help3 [] = "Para jugar a un programa que hayas hecho";

char *menu_help [] = { menu_help1, menu_help2, menu_help3 };

char **editor_lines;
int editor_n_lines = 0;
int editor_current_line;

#define LINE_BUFFER_SIZE 2048

void splash_screen_1 (void) {
	buf_color (7, 0);
	buf_cls ();
	
	buf_color (7, 1);
	buf_setxy (0, 24);
	buf_printabs ("  Pulsa una tecla para comenzar \xB3 Introduce ' men\xA3 ' siempre que quieras usarlo ");
	buf_setxy (0, 0);
	buf_printabs ("  Compilador Lokosoft LBasic versi\xA2n 6.03. (C)opyright 1994 by Loko Soft        ");
	buf_color (7, 0);

	buf_pause ();
}

int menu (void) {
	int menu_on = 1;
	int option = 0;
	int option_old = 0xff;

	buf_color (7, 1);
	buf_setxy (0, 0);
	buf_printabs ("  Editar  \xB3  Corregir  \xB3  Ejecutar un programa  \xB3      (C) Versi\xA2n 6.03, 1994   ");

	while (menu_on) {
		if (option_old != option) {
			if (option_old != 0xff) {
				buf_color (7, 1);
				buf_setxy (menu_x [option_old], 0);
				buf_printabs (menu_opt [option_old]);
			}

			buf_color (9, 7);
			buf_setxy (menu_x [option], 0);
			buf_printabs (menu_opt [option]);

			buf_color (7, 1);
			buf_setxy (0, 23);
			buf_printabs (menu_help [option]);

			option_old = option;
		}

		// Read cursors

		enum keycode_t* keys = readkeys();
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
				menu_on = 0;
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
	char *line_pointer;
	char c;

	int x = 0; 
	int y = 1;

	while (editing) {

		if (new_line) {

			// Increase size of buffer in 1 line
			lines_add_new ();

			// Now shuffle all pointers.
			for (int i = editor_n_lines - 2; i > editor_current_line; i ++) {
				editor_lines [i + 1] = editor_lines [i];
			}

			editor_current_line ++;

			line_pointer = (char *) malloc (1 * sizeof (char));
			line_pointer [0] = '\0';

			editor_lines [editor_current_line] = line_pointer;

			new_line = 0;
		} 

		if (line_change) {
			//
		}

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

				for (int i = line_length; i >= cursor; i --) {
					line_pointer [i + 1] = line_pointer [i];
				}

				line_pointer [cursor] = c;
				cursor ++;			
			}
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
			if (key == KEY_RETURN) {
				new_line = 1;
			}

			++keys;
		}

		// TO DO:: TAKE CARE OF POSSIBLE SCROLLING!
		buf_setxy (x, y);
		buf_color (7, 0);
		buf_printabs (line_pointer);

		c = line_pointer [cursor]; if(c == 0) c = ' ';
		buf_setxy (x + cursor % 80, y + cursor / 80);
		buf_color (0, 14);
		buf_char (c);

		waitvbl ();
	}
}

void main (char argc, char *argv []) {
	buf_setviewport (1, 23);
	splash_screen_1 ();

	while (menu () != 4) {
		// MEH
		editor ();
	}
}
