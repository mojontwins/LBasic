// Lbasic 6 remake

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../dos-like/source/dos.h"

#define LSTUI_CONTROL_IMPLEMENTATION
#include "../tui_dev/lstui.h"

#include "lstextmode.h"
#include "interpreter.h"
#include "conversion.h"
#include "tui.h"
#include "keys.h"
#include "conversion.h"
#include "file_browser.h"
#include "backend.h"
#include "sys_utils.h"
#include "lstokens.h"
#include "wizards.h"

int menu_x [] = { 0, 9, 18, 27, 38 };
int menu_f [] = { 7, 16, 25, 36, 45 };

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

#define SCROLL_DIRECTION_UP 1
#define SCROLL_DIRECTION_DOWN 2

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

int mouse_over_line;
int mouse_over_line_y;

char *keywords [] = {
	"cursor", "setxy", "print", "write", "center", "color", "pause", "beep",
	"cls", "draw", "choice", "viewport", "attempts", "statusbar", "margins", "interace",
	"ww", "wordwrap", "ansibin", "mode", "pic", "cut", "lin", "sleep", "menu", "items", "exits",  "resp",
	"wwc", "fancy_cls", "tb", "tbwc", "bg", "zones", "pix", "cbc", "extern", "music", "sound", "indicator", 
	NULL
};

char *flags_keywords [] = {
	 "let", "input", "tell", "inc", "dec", "add", "sub", 
	NULL
};

char *branching_keywords [] = {
	"go", "eq", "neq", "ge", "lt", "autogo", "ret",
	NULL
};

char *modifier_keywords [] = {
	"cbc", "on", "off", "text", "gfx", "gfx_sq", "gfx_med", "gfx_hi",
	"kbd", "reset", "limpia", "put", "pon", "remove", "quita", "config", "run", 
	"has", "tiene", "hasnt", "notiene", "def",
	NULL
};

void run_from (int from);

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

	buf_color (7, 1);
	buf_setxy (0, 24);
	buf_print_abs ("                                                                                ");	

	int old_key = 0;
	keys_read ();
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

		// Mouse

		int x = buf_get_mouse_x ();
		int y = mousey (); //buf_get_mouse_y ();
		if (y == 0) {
			for (int i = 0; i < 5; i ++) {
				if (x >= menu_x [i] && x <= menu_f [i]) {
					option = i; break;
				}
			}

			if (buf_get_mouse_b (1) && x <= menu_f [4]) {
				menu_on = 0;
			}
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
	FILE *pf = fopen (filename, "wb");
	unsigned char clean_buffer [LINE_BUFFER_SIZE];
	
	for(int i = from; i < editor_last_line; i ++) {

/*
		unsigned char *ptr = clean_buffer;
		for (int j = 0; j < strlen (editor_lines [j])) {
			unsigned char c = editor_lines [i][j]
		}
*/
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
	unsigned char clean_buffer [LINE_BUFFER_SIZE];
	int lines_read = 0;

	lines_free_all ();

	FILE *pf = fopen (filename, "r");
	while (fgets (line_buffer, LINE_BUFFER_SIZE, pf) != NULL) {
		lines_add_new ();

		unsigned char *ptr = clean_buffer;
		for (int i = 0; i < strlen (line_buffer); i ++) {
			unsigned char c = line_buffer [i];
			if (c >= 32) *ptr ++ = c;
		}
		*ptr = 0;

		editor_lines [lines_read] = strdup (clean_buffer);
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

int find_color (unsigned char *s) {
	// Cut early
	if (strlen (s) == 0) return 7;
	if (s[0] == '\t' || s[0] == 32 || s[0] == 34) return 7;

	// And then:

	if (s[0] == ':') return 10;

	if (s[0] == '$') return 15;

	for (int i = 0; keywords [i] != NULL; i ++) {
		if (strcmp (s, keywords [i]) == 0) return 12;
	}

	for (int i = 0; flags_keywords [i] != NULL; i ++) {
		if (strcmp (s, flags_keywords [i]) == 0) return 11;
	}

	for (int i = 0; branching_keywords [i] != NULL; i ++) {
		if (strcmp (s, branching_keywords [i]) == 0) return 14;
	}

	for (int i = 0; modifier_keywords [i] != NULL; i ++) {
		if (strcmp (s, modifier_keywords [i]) == 0) return 13;
	}

	return 7;
}

void syntax_hightlight (int bkg, unsigned char *s) {
	// Generates several buf_print_ab if nice stuff detected.
	// Rewriten in a much smarter way
	int is_comment = 0;

	parse_to_tokens_whitespace (s);

	int num_tokens = get_num_tokens ();	
	for (int i = 0; i < num_tokens; i ++) {
		unsigned char *token = get_token (i);

		// Detect comments early
		if (is_comment) {
			buf_color (3, bkg);
		} else {
			if (token [0] == '#') {
				is_comment = 1;
				buf_color (3, bkg);
			} else {
				buf_color (find_color (token), bkg);
			}
		}

		buf_print_abs_clip_to_scroll (token);
	}

	int x = buf_getx (); while (x < 80) {
		buf_char (' '); x ++;
	}
}

int display_editor_lines (int cursor, int scroll_until_no) {

	buf_color (7, 0);
	buf_clscroll ();

	if (editor_lines == NULL) return 0;

	int y = 1;
	int i = 0;
	int cur_line_rendered = 0;
	int lastline = 23;
	int res = 0;
	
	mouse_over_line = -1;

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

	if (editor_current_line < 0) return;
	char *line_pointer = editor_lines [editor_current_line];

	int mouse_y = buf_get_mouse_y ();

	while (1) {
		int line_no = first_line_to_display + i;
		int bkg = line_no & 1 ? 8 : 0;

		// Used for AV PAG / RE PAG: true if line is displayed
		if (line_no == scroll_until_no) res = 1;

		if (line_no > editor_last_line) break;
		
		buf_setxy (0, y);
		buf_color (7, bkg);

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

		// Detect mouse
		if (mouse_y < 24 && mouse_y > y) {
			mouse_over_line = line_no;
			mouse_over_line_y = y;
		}

		y += 1 + strlen (editor_lines [line_no]) / 80;

		if (y > lastline) break;

		i ++;
	}

	return res;
}

int wizard_config_command (void) {
	unsigned char *line_pointer;

	line_pointer = editor_lines [editor_current_line];

	// Parse line
	line_pointer = wizards_parse (line_pointer);

	// In case we realloc'ed this is mandatory
	editor_lines [editor_current_line] = line_pointer;

	return wizards_succeeded ();
}

void wizard_text_insert (int *cursor, int draw) {
	unsigned char *line_pointer;
	int action = 0;

	// From start of the line to cursor, turn on / off "between_quotes", 
	// If "between quotes" get from-to, copy to new string, pass it, 
	// Don't forget to free, or use an all-purpose container array.

	// asdasd "sadasdk*asasd" "asodpsod"
	//         A       C   B
	// editable_text = A-B,
	// line = 0-A, editable_text, B-fin
	
	int edit = 0;

	int last_opening_quotes;
	int crop_from;
	int crop_to;
	
	int quotes = 0;
	
	unsigned char edit_buffer [1024]; edit_buffer [0] = 0;
	unsigned char c;

	line_pointer = editor_lines [editor_current_line];

	for (int i = 0; i < strlen (line_pointer); i ++) {
		c = line_pointer [i];
		if (c == '\"') {
			if (quotes == 0) {
				last_opening_quotes = i;
				quotes = 1;
			} else {
				quotes = 0;
			}
		}

		if (i == *cursor) {
			if (quotes) {
				// Find closing quote, trim string, call,
				// etc
				int idx_buffer = 0;
				int idx_line = last_opening_quotes + 1;
				crop_from = idx_line;
				while (idx_line <= strlen (line_pointer)) {
					c = line_pointer [idx_line];
					if (c == '\"') break;
					edit_buffer [idx_buffer] = c;

					idx_line ++;
					idx_buffer ++;
				}
				edit_buffer [idx_buffer] = 0;
				crop_to = idx_line;

				edit = 1;
				break;
			} else {
				// Break, this is not an edition!
				break;
			}
		}
	}

	if (edit) {
		if (strlen (edit_buffer) >= 6*80) {
			tui_alert ("\xAD" "Imposible!", "El texto es muy largo");
			return;			
		}
	}

	// Orig / Insertion code:

	char *new_text;
	if (draw) {
		new_text = tui_drawbox (edit_buffer, &action);
	} else {
		new_text = tui_textbox (7, "Enter text to insert", edit_buffer, 6, &action);
	}

	if (new_text) {

		if (edit) {
			// Insert text in the right spot
			
			line_pointer [crop_from] = 0; 						// Terminate 1st part of the input string.
			unsigned char *trail = line_pointer + crop_to;		// Trail part of the input string.

			unsigned char *new_line = malloc (strlen (line_pointer) + strlen (new_text) + strlen (trail) + 2);
			strcpy (new_line, line_pointer);
			strcat (new_line, new_text);
			strcat (new_line, trail);

			free (line_pointer); 								// Free original line
			line_pointer = new_line; 							// Point to new line

		} else {
			line_pointer = realloc (line_pointer, (strlen (line_pointer) + strlen (new_text) + 4) * sizeof (char));
			
			strcat (line_pointer, "\"");
			strcat (line_pointer, new_text);
			strcat (line_pointer, "\"");
		}

		*cursor += strlen (new_text) + 2;
		editor_lines [editor_current_line] = line_pointer;
	}
}

void editor_top () {
	buf_setxy (0, 0);
	buf_color (7, 1);
	buf_print_abs (" Editar                                             (C) LBasic Legacy 1994-2024 ");

	buf_color (9, 7);
	buf_setxy (menu_x [0], 0);
	buf_print_abs (menu_opt [0]);
}

void editor_bottom () {
	buf_setxy (0, 24);
	buf_color (7, 1);

	buf_print_abs (" F3 CHOICE \xB3 F4 TEXT \xB3 F5 RUN \xB3 F6 RUN FROM \xB3 F7 DRAW \xB3                        ");
	
	debuff_keys ();	
}

char *insert_char (char *line_pointer, int cursor, unsigned char c) {
	// Insert a new character (end / middle)
	int line_length = strlen (line_pointer);
	
	// Increment buffer size, move right part right 1 char, insert

	line_pointer = realloc (line_pointer, (line_length + 2) * sizeof (char));
	editor_lines [editor_current_line] = line_pointer;

	for (int i = line_length; i >= cursor; i --) {
		line_pointer [i + 1] = line_pointer [i];
	}

	line_pointer [cursor] = c;
	return line_pointer;
}

int editor (void) {
	// File is already loaded.
	int new_line = 1;
	int line_change = 0;
	int delete_line = 0;
	
	int cursor = -1;
	char *line_pointer = NULL;
	unsigned char c;
	char info_buf [16];

	int editor_next_line = editor_n_lines;
	int scroll_until_no = -1;
	int scroll_direction = 0;

	editor_top ();
	editor_bottom ();

	int editing = 1;
	while (editing) {

		// Display	
		int displayed_no = display_editor_lines (cursor, scroll_until_no);



		if (scroll_direction) {
			if (displayed_no) {
				if (scroll_direction == SCROLL_DIRECTION_DOWN) {
					if (editor_current_line == editor_last_line) {
						scroll_direction = 0;
						scroll_until_no = -1;
					} else {
						editor_next_line = editor_current_line + 1;
						line_change = 1;
					}
				} else if(scroll_direction == SCROLL_DIRECTION_UP) {
					if (editor_current_line == 0) {
						scroll_direction = 0;
						scroll_until_no = -1;
					} else {
						editor_next_line = editor_current_line - 1;
						line_change = 1;
					}
				}
			} else {
				scroll_direction = 0;
				scroll_until_no = -1;
			}
		}

		if (new_line) {
			// Add new line.

			// Hack to insert line before
			int movedown = 0;
			if (cursor == 0) {
				editor_next_line --;
				editor_current_line --;
				movedown = 1;
			}

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

			if (movedown) editor_current_line ++;
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

		sprintf (info_buf, "%04d:%02d", editor_current_line, cursor);
		buf_setxy (73, 24);
		buf_color (4, 7);
		buf_print_abs (info_buf);

		line_pointer = editor_lines [editor_current_line];
		if (cursor > strlen (line_pointer)) cursor = strlen (line_pointer);

		// Read keys and write to line.
		unsigned char const* chars = (unsigned char*) readchars ();
		enum keycode_t* keys = readkeys ();
		//keys_read (); int keys_this_frame = keys_get_this_frame ();

		// Characters
		c = get_character_input (chars);

		if (c >= ' ') {
			insert_char (line_pointer, cursor, c);
			cursor ++;			
			needs_saving = 1;
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

				case '\t':
					if (!wizard_config_command ()) {
						// Insert 4 spaces
						
						for (int i = 0; i < 4; i ++) {
							line_pointer = insert_char (line_pointer, cursor, ' ');
							cursor ++;
						}
						
						needs_saving = 1;
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

			if (keystate (KEY_CONTROL)) {
				if (key == KEY_LEFT) {
					// Find previous space in line, put the cursor behind
					parse_to_tokens_plain (line_pointer);
					int n = get_num_tokens ();
					
					for (int i = n - 1; i >= 0; i ++) {
						if (cursor > get_index (i)) {
							cursor = get_index (i);
							break;
						}
					}
				}

				if (key == KEY_RIGHT) {
					// Find the next non space in line, put the cursor there
					parse_to_tokens_plain (line_pointer);
					int n = get_num_tokens ();

					for (int i = 0; i < n; i ++) {
						if (cursor < get_index (i)) {
							cursor = get_index (i);
							break;
						}
					}
				}

				if (key == KEY_UP) {
					// Move up within current line;
					cursor -= 80; if (cursor < 0) cursor = 0; // TODO: Jump to prev line?
				}

				if (key == KEY_DOWN) {
					// Move down within current line;
					cursor += 80; if (cursor > strlen(line_pointer)) cursor = strlen (line_pointer); // TODO: Jump to next line?
				}

				if (key == KEY_HOME) {
					editor_next_line = 0;
					line_change = 1;
				}

				if (key == KEY_END) {
					editor_next_line = editor_last_line;
					line_change = 1;
				}
			} else {
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

				if (key == KEY_HOME) {
					cursor = 0;
				}

				if (key == KEY_END) {
					cursor = strlen (line_pointer);
				}
			}

			if (key == KEY_RETURN) {
				editor_next_line = editor_current_line + 1;
				new_line = 1;
			}

			if (key == KEY_PRIOR) {
				// Up until current line disappears, or line 0, then set
				scroll_until_no = editor_current_line;
				scroll_direction = SCROLL_DIRECTION_UP;
			}

			if (key == KEY_NEXT) {
				// Down until current line disappears, or last line, then set
				scroll_until_no = editor_current_line;
				scroll_direction = SCROLL_DIRECTION_DOWN;
			}

			if (key == KEY_F4) {
				// Insert text from a text area
				wizard_text_insert (&cursor, 0);
			}

			if (key == KEY_F5) {
				run_from (0);
				editor_top ();
				editor_bottom ();
			}

			if (key == KEY_F6) {
				run_from (editor_current_line);
				editor_top ();
				editor_bottom ();
			}

			if (key == KEY_F7) {
				// Insert draw from a draw area
				wizard_text_insert (&cursor, 1);
			}

			++keys;
		}

		// Mouse
		if (buf_get_mouse_b (1) && mouse_over_line != -1) {
			editor_next_line = mouse_over_line;

			// Calculate where within line
			int y_within_line = buf_get_mouse_y () - mouse_over_line_y;
			cursor = y_within_line * 80 + buf_get_mouse_x ();
			if (cursor > strlen (line_pointer)) cursor = strlen (line_pointer);

			line_change = 1;
		}

		int w = buf_get_mouse_wheel ();
		if (w > 0) {
			if (editor_current_line > 0) {
				editor_next_line = editor_current_line - 1;
				line_change = 1;
			}
		} else if (w < 0) {
			if (editor_current_line < editor_last_line) {
				editor_next_line = editor_current_line + 1;
				line_change = 1;
			}
		}

		if(!scroll_direction) waitvbl ();
	}

	menu_show ();
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
	setpal (8, 8, 8, 8);
	buf_cls ();
	buf_setviewport (1, 23);
	
}

void run_from (int from) {
	save_program_do (from, "temp.tmp");
	lbasi_run_tmp ("temp.tmp", get_file_spec ());
	remove ("temp.tmp");
	
	backend_print ("[Programa terminado]");
	backend_pause ();
	debuff_keys ();

	reset_screen ();
	menu_show ();
}

void main (char argc, char *argv []) {
	lstextmode_init ();
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
		display_editor_lines (-1, -1);
	}

	clear_file_spec ();
	lstokens_free ();
	lstextmode_shutdown ();
}
