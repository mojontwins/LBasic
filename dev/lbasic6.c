// Lbasic 6 remake

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../dos-like/source/dos.h"

#include "lstextmode.h"
#include "interpreter.h"
#include "conversion.h"
#include "tui.h"
#include "keys.h"
#include "conversion.h"
#include "file_browser.h"
#include "backend.h"

int menu_x [] = { 0, 9, 18, 27, 38 };

char menu_opt1 [] = " Editar ";
char menu_opt2 [] = " Cargar ";
char menu_opt3 [] = " Grabar ";
char menu_opt4 [] = " Ejecutar ";
char menu_opt5 [] = " Borrar ";

#define OPTION_EDIT 0
#define OPTION_LOAD 1
#define OPTION_SAVE 2
#define OPTION_RUN 3
#define OPTION_DEL 4

char *menu_opt [] = { menu_opt1, menu_opt2, menu_opt3, menu_opt4, menu_opt5 };

char menu_help1 [] = " Para hacer un programa                  ";
char menu_help2 [] = " Para cargar un programa del disco       ";
char menu_help3 [] = " Para guardar un programa en el disco    ";
char menu_help4 [] = " Para jugar al programa que est\xA0 cargado ";
char menu_help5 [] = " Para borrar el programa de la memoria   ";

char *menu_help [] = { menu_help1, menu_help2, menu_help3, menu_help4, menu_help5 };

char **editor_lines;
int editor_n_lines; 		// Total number of lines allocated
int editor_current_line;	// Line being edited
int editor_last_line; 		// Last line in program

int program_loaded; 		// Program in memory
int needs_saving;			// Altered loaded program
int last_option = 0;
int first_line_to_display;


#define LINE_BUFFER_SIZE 2048

void splash_screen_1 (void) {
	buf_color (7, 0);
	buf_cls ();
	
	buf_color (7, 1);
	buf_setxy (0, 24);
	buf_print_abs ("  Pulsa una tecla para comenzar \xB3 Introduce ' men\xA3 ' siempre que quieras usarlo ");
	buf_setxy (0, 0);
	buf_print_abs ("  Compilador Lokosoft LBasic versi\xA2n Legacy (C)opyright 1994-2024 by Loko Soft  ");
	buf_color (7, 0);

	tui_alert ("Compilador LBasic Legacy", "\xAD" "Bienvenid@s!");

	buf_color (7, 0);
	buf_clscroll ();
}

void menu_show (void) {
	buf_setxy (0, 0);
	buf_color (7, 1);
	buf_print_abs (" Editar \xB3 Cargar \xB3 Grabar \xB3 Ejecutar \xB3 Borrar       (C) LBasic Legacy 1994-2024 ");
}

int menu (void) {
	int menu_on = 1;
	int option = last_option;
	int option_old = 0xff;
	int key_enter = 1;

	while (!buf_heartbeat () && keystate (KEY_RETURN));

	buf_color (7, 1);
	buf_setxy (0, 24);
	buf_print_abs ("                                                                                ");	

	int old_key = 0;
	while (!buf_heartbeat () && menu_on) {
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

		readchars ();
		readkeys (); 						// dos-like Debuff?
		
		keys_read (); 						// My own handler
		int keys = keys_get_this_frame ();

		if (keys & MT_KEY_LEFT)  {
			option --; if (option < 0) option = 4;
		} 

		if (keys & MT_KEY_RIGHT) {
			option ++; if (option > 4) option = 0;
		}

		if (keys & MT_KEY_ESC) {
			option = 0xff; menu_on = 0;
		}

		if (keys & MT_KEY_ENTER) {
			menu_on = 0;
		}

		waitvbl ();
	}

	while (!buf_heartbeat () && keystate (old_key));

	last_option = option;
	return option;
}

void lines_add_new (void) {
	editor_n_lines ++;
	editor_lines = (char **) realloc (editor_lines, editor_n_lines * sizeof (char *));
}

void lines_free_all (void) {
	if (editor_lines) {
		for (int i = 0; i < editor_n_lines; i ++) {
			free (editor_lines [i]);
		}

		free (editor_lines);
	}
	editor_lines = NULL;

	editor_current_line = -1;
	editor_last_line = -1;
	editor_n_lines = 0;
	needs_saving = 0;

	first_line_to_display = 0;
}

void save_program_do (int from, char *filename) {
	FILE *pf = fopen (filename, "w");
	for(int i = from; i < editor_last_line; i ++) {
		fprintf (pf, "%s\r\n", editor_lines [i]);
	}
	fclose (pf);
}

void save_program (void) {
	if (select_file_spec ()) {
		save_program_do (0, get_file_spec ());
	}
}

void load_program_do (char *filename) {
	unsigned char line_buffer [LINE_BUFFER_SIZE];
	int lines_read = 0;

	lines_free_all ();

	FILE *pf = fopen (filename, "r");
	while (fgets (line_buffer, LINE_BUFFER_SIZE, pf) != NULL) {
		lines_add_new ();

		// This allocates a bit more mem than needed
		// But who cares?
		unsigned char *clean_line = malloc (strlen (line_buffer)); 
		unsigned char *ptr = clean_line;
		for (int i = 0; i < strlen (line_buffer); i ++) {
			unsigned char c = line_buffer [i];
			if (c >= ' ') *ptr ++ = c;
		}
		*ptr = 0;

		editor_lines [lines_read] = clean_line;
		editor_last_line = lines_read;
		lines_read ++;
	}

	fclose (pf);
	
	editor_current_line = editor_last_line;
	needs_saving = 0;
	first_line_to_display = 0;

	program_loaded = 1;
}

void load_program (void) {

	if (select_file_spec ()) {
		load_program_do (get_file_spec ());
	}
}

int find_color (char *s) {
	if (strlen (s) == 0) return;

	if (s[0] == ':') return 10;

	if (
		strcmp (s, "print") == 0 ||
		strcmp (s, "choice") == 0 ||
		strcmp (s, "color") == 0 ||
		strcmp (s, "centra") == 0 ||
		strcmp (s, "pause") == 0 ||
		strcmp (s, "beep") == 0
	) return 12;

	return 7;
}

void syntax_hightlight (int bkg, char *s) {
	// Generates several buf_print_ab if nice stuff detected.
	int n_tokens;
	int found_keyword = 0;

	int state = 0; 
	int l = strlen (s);
	
	int chars_to_copy = 0;
	char temp_buffer[l + 1];
	char c;

	int temp_index = 0;

	for (int i = 0; i < l; i ++) {
		c = s [i];

		if (state == 0) {
			if (c == ' ' || c == '\t') {
				temp_buffer [temp_index ++] = c;
			} else {
				temp_buffer [temp_index] = '\0';
				temp_index = 0;
				state = 1;

				buf_color (7, bkg);
				buf_print_abs_clip_to_scroll (temp_buffer);
			}
		}

		if (state == 1) {
			if (!(c == ' ' || c == '\t' || c == 34)) {
				temp_buffer [temp_index ++] = c;
			} else {
				temp_buffer [temp_index] = '\0';
				temp_index = 0;
				state = 2;

				buf_color (find_color (temp_buffer), bkg);
				buf_print_abs_clip_to_scroll (temp_buffer);
			}
		}

		if (state == 2) {
			temp_buffer [temp_index ++] = c;
		}
	}

	temp_buffer [temp_index] = '\0';
	buf_color (state == 1 ? find_color(temp_buffer) : 7, bkg);
	buf_print_abs_clip_to_scroll (temp_buffer);
}

void display_editor_lines (int cursor) {
	int y = 1;
	int i = 0;
	int cur_line_rendered = 0;
	int lastline = 23;
	
	// do a 1st pass to make sure current line is on display
	while (1) {
		int line_no = first_line_to_display + i;
		if (line_no > editor_last_line) break;
		if (line_no == editor_current_line) {
			int ycursor = y + cursor / 80;
			if (ycursor <= lastline) cur_line_rendered = 1;
		}

		y += 1 + strlen (editor_lines [line_no]) / 80;
		if (y > lastline) {
			if (cur_line_rendered) break;
			first_line_to_display ++;
			y = 1; i = 0;
		}

		i ++;
	}

	y = 1; i= 0; cur_line_rendered = 0;

	buf_color (7, 0);
	buf_clscroll ();

	if (editor_current_line < 0) return;
	char *line_pointer = editor_lines [editor_current_line];

	while (1) {
		int line_no = first_line_to_display + i;
		int bkg = line_no & 1 ? 8 : 0;

		if (line_no > editor_last_line) break;
		
		buf_setxy (0, y);
		buf_color (7, bkg);
		//buf_print_abs (editor_lines [line_no]);
		syntax_hightlight (bkg, editor_lines [line_no]);

		// Draw cursor?
		if (line_no == editor_current_line) {
			int ycursor = y + cursor / 80;

			if (ycursor <= lastline) {
				cur_line_rendered = 1;

				if (cursor >= 0) {
					char c = line_pointer [cursor]; if (c == 0) c = ' ';
					buf_setxy (cursor % 80, ycursor);
					buf_color (0, 14);
					buf_char (c);
				}
			}
		}

		y += 1 + strlen (editor_lines [line_no]) / 80;

		if (y > lastline) break;

		i ++;
	}
}

int editor (void) {
	// File is already loaded.

	int new_line = 1;
	int line_change = 0;
	int delete_line = 0;
	
	int line_length;
	int cursor = 0;
	char *line_pointer = NULL;
	unsigned char c;
	char info_buf [16];

	int editor_next_line = editor_n_lines;

	buf_setxy (0, 24);
	buf_color (7, 1);

	buf_print_abs (" F3 CHOICE \xB3 F4 TEXT \xB3 F5 RUN \xB3 F6 RUN FROM \xB3 F10 MENU \xB3                        ");
	
	debuff_keys ();

	int editing = 1;
	while (editing) {
		sprintf (info_buf, "%04d:%02d", editor_current_line, cursor);
		buf_setxy (73, 24);
		buf_color (4, 7);
		buf_print_abs (info_buf);

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
			needs_saving = 1;
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

		if (delete_line) {
			// Free current line
			free (editor_lines [editor_current_line]);

			// Shuffle all lines up
			for(int i = editor_current_line; i < editor_last_line; i ++) {
				editor_lines [i] = editor_lines [i + 1];
			}

			// Clear last line
			editor_lines [editor_last_line] = NULL;

			// decrease last line /
			editor_last_line --;
			editor_current_line --;
			cursor = strlen (editor_lines [editor_last_line]) + 1;

			delete_line = 0;
			needs_saving = 1;
		}

		line_pointer = editor_lines [editor_current_line];
		if (cursor > strlen (line_pointer)) cursor = strlen (line_pointer);

		// Read keys and write to line.
		unsigned char const* chars = (unsigned char*) readchars ();
		enum keycode_t* keys = readkeys ();
		//keys_read (); int keys_this_frame = keys_get_this_frame ();

		// Characters
		c = get_character_input (chars);

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
			needs_saving = 1;
			cursor ++;			
		} else {
			switch (c) {
				case 8:
					// Delete

					// If cursor is on the 1st character -> delete this line
					
					if (cursor == 0) {
						if (strlen (line_pointer) == 0) {
							delete_line = 1;
						}
					} else {
						// Delete character to the left of the cursor.
						
						cursor --;
						for (int i = cursor; i < strlen (line_pointer); i ++) {
							line_pointer [i] = line_pointer [i + 1];
							needs_saving = 1;
						}
					}

					break;

				case 27:
					// Exit
					editing = 0;
					while (keystate (KEY_ESCAPE));
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

			if (key == KEY_PRIOR) {
				// Up until current line disappears, or line 0, then set
			}

			if (key == KEY_NEXT) {
				// Down until current line disappears, or last line, then set
			}

			if (key == KEY_F4) {
				// Insert text from a text area
				int action = 0;

				// TODO: Parse line so if F4 is pressed in the middle of the string->edit string
				// Which may be tricky!
				// From start of the line to cursor, turn on / off "between_quotes", 
				// If "between quotes" get from-to, copy to new string, pass it, 
				// Don't forget to free, or use an all-purpose container array.

				char *new_text = tui_textbox (7, "Enter text to insert", 0, 6, &action);
				if (new_text) {
					line_pointer = editor_lines [editor_current_line];
					line_pointer = realloc (line_pointer, (strlen (line_pointer) + strlen (new_text) + 4) * sizeof (char));
					
					strcat (line_pointer, "\"");
					strcat (line_pointer, new_text);
					strcat (line_pointer, "\"");

					editor_lines [editor_current_line] = line_pointer;
				}
			}

			++keys;
		}

		// Display
		display_editor_lines (cursor);

		waitvbl ();
	}

}

void dialog_program_not_present (void) {
	return tui_alert ("\xAD" "Imposible!", "No hay programa en memoria");
}

int dialog_program_present_sure (void) {
	return tui_yes_or_no ("\xA8" "Seguro?", "Hay cambios sin grabar");
}

void reset_screen (void) {
	buf_setmode (LS_MODE_TEXT);
	setpal (6, 0xaa, 0xaa, 0);
	buf_cls ();
	buf_setviewport (1, 23);
	
}

void run_from (int from) {
	save_program_do (from, "temp.tmp");
	
	FILE *file = fopen ("temp.tmp", "r");
	lbasi_run_file (file);
	fclose (file);
	
	remove ("temp.tmp");
	
	backend_print ("[Programa terminado]");
	backend_pause ();
	debuff_keys ();

	reset_screen ();
	menu_show ();
}

void main (char argc, char *argv []) {
	cursoff ();
	
	reset_screen ();
	splash_screen_1 ();

	lines_free_all ();
	clear_file_spec ();

	menu_show ();
	int option;
	while ((option = menu ()) != 0xff) {

		switch (option) {
			case OPTION_EDIT:			
				editor ();
				break;

			case OPTION_LOAD:
				if (!needs_saving || dialog_program_present_sure ()) {
					load_program ();
				}
				break;

			case OPTION_SAVE:
				if (needs_saving || program_loaded) {
					save_program ();
				} else {
					dialog_program_not_present ();
				}
				break;

			case OPTION_RUN:
				if (needs_saving || program_loaded) {
					run_from (0);
				} else {
					dialog_program_not_present ();
				}
				break;

			case OPTION_DEL:
				if (!needs_saving || dialog_program_present_sure ()) {
					lines_free_all ();
					clear_file_spec ();
				}
				break;
		}

		debuff_keys ();
		display_editor_lines (-1);
	}

	clear_file_spec ();
}
