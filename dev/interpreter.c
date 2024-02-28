// Lbasic Interpreter

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lstokens.h"
#include "backend.h"
#include "sys_utils.h"
#include "interpreter.h"

#define LINE_BUFFER_SIZE 2048
#define LEGACY_BUFFER_SIZE 16
#define DEFAULT_ATTEMPTS 5

int attempts;
char str_try_again [80];
char str_game_over [80];
char str_continue [80];
char str_restart [80];
char str_exit [80];
char str_status_top [80];
char str_status_bottom [80];

int clr_statusbar_1;
int clr_statusbar_2;

char main_path_spec [256];

/*

	lbasi spec

	Will run spec.000, spec.001... etc. Will stop on missing index so be careful.

	lbasi

	Will assume spec = "boot", and run that if it exists.

*/

void init_strings (void) {
	// TODO: Read a config file

	strcpy (str_try_again, "Prueba de nuevo. Intentos: %d");
	strcpy (str_game_over, "GAME OVER");
	strcpy (str_continue, "1. Reiniciar este cap\xA1tulo");
	strcpy (str_restart, "2. Empezar desde el principio");
	strcpy (str_exit, "3. Salir");
	strcpy (str_status_bottom, " Reintentos: ");

	clr_statusbar_1 = 14;
	clr_statusbar_2 = 1;
}

void update_path_spec (char *fullpath) {
	char *last_slash = strrchr (fullpath, '/');

	if (last_slash == NULL) {
		last_slash = strrchr (fullpath, '\\');
	}
	
	if (last_slash == NULL) {
		main_path_spec [0] = 0;
	} else {
		int parentLen = strlen(fullpath) - strlen(last_slash + 1);
		strncpy(main_path_spec, fullpath, parentLen);
	}
}

int get_clr_statusbar1 (void) {
	return clr_statusbar_1;
}

int get_clr_statusbar2 (void) {
	return clr_statusbar_2;
}

char *get_str_try_again (void) {
	return str_try_again;
}

void lbasi_goto (FILE *file, char *label) {
	int label_index = labels_find (label);

	if (label_index >= 0) {
		int label_filepos = labels_get_filepos (label_index);

		fseek (file, label_filepos, SEEK_SET);
	}
}

void lbasi_read_labels (FILE *file) {
	char line_buffer [LINE_BUFFER_SIZE];
	
	labels_clear ();

	while (fgets (line_buffer, LINE_BUFFER_SIZE, file) != NULL) {

		if (line_buffer [0] == ':') {
			parse_to_tokens (line_buffer);			
			if (labels_add (ftell (file), get_token (0)) == 0) {
				backend_print_ln ("Warning! label too long:");
				backend_print_ln (line_buffer);
			}
		}
	}

	fseek (file, 0, SEEK_SET);
}

int lbasi_run_file (FILE *file) {
	int res = 0;
	int choice_res;
	int run = 1;

	char line_buffer [LINE_BUFFER_SIZE];
	char temp_buffer [256];
	char string_convert_buffer [256];
	char legacy_buffer [LEGACY_BUFFER_SIZE];
	char draw_buffer [2048];
	char *command_token;

	lbasi_read_labels (file);

	while (run && fgets (line_buffer, LINE_BUFFER_SIZE, file) != NULL) {
		// Tokenize line_buffer
		parse_to_tokens (line_buffer);
		if (get_num_tokens () == 0) continue;

		// Execute
		command_token = get_token (0);
		utils_tolower (command_token);

		if (strcmp (command_token, "cursor") == 0 || strcmp (command_token, "setxy") == 0) {
			backend_setxy (atoi (get_token (1)), atoi (get_token (2)));

		} else if (strcmp (command_token, "print") == 0) {
			backend_print_ln (get_token (1));

		} else if (strcmp(command_token, "write") == 0) {
			backend_print (get_token (1));

		} else if (strcmp (command_token, "center") == 0) {
			backend_center (get_token (1));

		} else if (strcmp (command_token, "color") == 0) {
			backend_color (atoi (get_token (1)), atoi (get_token (2)));

		} else if (strcmp (command_token, "pause") == 0) {
			backend_pause ();

		} else if (strcmp (command_token, "beep") == 0) {
			backend_beep ();

		} else if (strcmp (command_token, "cls") == 0) {
			backend_cls ();

		} else if (strcmp (command_token, "draw") == 0) {
			backend_draw (get_token (1));

		} else if (strcmp (command_token, "choice") == 0) {
			while ((choice_res = backend_choice (
				flags_parse_value (get_token (1)), 		// Number of choices. Beware using flags here!
				flags_parse_value (get_token (2)), 		// Correct choice. 
				get_tokens ()
			)) == 0 && !backend_get_break ()) {
				attempts --;
				backend_statusbar (clr_statusbar_1, clr_statusbar_2, str_status_top, str_status_bottom, attempts);

				if (attempts >= 0) {
					backend_try_again (str_try_again, attempts);
				} else {
					res = 4;
					run = 0;
					break;
				}
			}

			if (choice_res < 0) {
				res = choice_res;
				run = 0;
			}

		} else if (strcmp (command_token, "viewport") == 0) {
			backend_set_viewport (
				flags_parse_value (get_token (1)),
				flags_parse_value (get_token (2))
			);

		} else if (strcmp (command_token, "attempts") == 0) {
			attempts = atoi (get_token (1));

		} else if (strcmp (command_token, "statusbar") == 0) {
			char *onoff = get_token (1);
			utils_tolower (onoff);
			backend_set_show_status (strcmp (onoff, "off") == 0);

		} else if (strcmp (command_token, "margins") == 0) {
			backend_set_margins (
				flags_parse_value (get_token (1)),
				flags_parse_value (get_token (2))
			);
			
		} else if (strcmp (command_token, "ww") == 0 || strcmp (command_token, "wordwrap") == 0) {
			backend_wordwrap (get_token (1), (strcmp (get_token (2), "cbc") == 0));

		} else if (strcmp (command_token, "ansibin") == 0) {
			backend_ansibin (main_path_spec, get_token (1));

		}

		// *** FLAGS ***

		else if (strcmp (command_token, "let") == 0) {
			int flag = flags_parse_value (get_token (1));
			int value = flags_parse_value (get_token (2));

			flags_set (flag, value);
		} else if (strcmp (command_token, "input") == 0) {
			int flag = flags_parse_value (get_token (1));			
			itoa (backend_read_option (0), string_convert_buffer, 10);
			int value = flags_parse_value (string_convert_buffer);

			flags_set (flag, value);

		} else if (strcmp (command_token, "tell") == 0) {
			int flag = flags_parse_value (get_token (1));
			itoa (flag, string_convert_buffer, 10);
			backend_print (string_convert_buffer);

		} else if (strcmp (command_token, "inc") == 0) {
			int flag = flags_parse_value (get_token (1));
			flags_set (flag, flags_get (flag) + 1);

		} else if (strcmp (command_token, "dec") == 0) {
			int flag = flags_parse_value (get_token (1));
			flags_set (flag, flags_get (flag) - 1);

		} else if (strcmp (command_token, "add") == 0) {
			int flag = flags_parse_value (get_token (1));
			int value = flags_parse_value (get_token (1));
			flags_set (flag, flags_get (flag) + value);

		} else if (strcmp (command_token, "sub") == 0) {
			int flag = flags_parse_value (get_token (1));
			int value = flags_parse_value (get_token (1));
			flags_set (flag, flags_get (flag) - value);

		}

		// *** BRANCHING ***

		else if (strcmp (command_token, "go") == 0) {
			lbasi_goto (file, get_token (1));

		} else if (strcmp (command_token, "eq") == 0) {
			int val1 = flags_parse_value (get_token (1));
			int val2 = flags_parse_value (get_token (2));

			if (val1 == val2) lbasi_goto (file, get_token (3));
		} else if (strcmp (command_token, "neq") == 0) {
			int val1 = flags_parse_value (get_token (1));
			int val2 = flags_parse_value (get_token (2));

			if (val1 != val2) lbasi_goto (file, get_token (3));
		} else if (strcmp (command_token, "ge") == 0) {
			int val1 = flags_parse_value (get_token (1));
			int val2 = flags_parse_value (get_token (2));

			if (val1 >= val2) lbasi_goto (file, get_token (3));
		} else if (strcmp (command_token, "lt") == 0) {
			int val1 = flags_parse_value (get_token (1));
			int val2 = flags_parse_value (get_token (2));

			if (val1 < val2) lbasi_goto (file, get_token (3));

		} else if (strcmp (command_token, "autogo") == 0) {
			int flag = flags_get (flags_parse_value (get_token (1)));
			itoa (flag, string_convert_buffer, 10);

			strcpy (temp_buffer, get_token (2));
			strcat (temp_buffer, "_");
			strcat (temp_buffer, string_convert_buffer);

			lbasi_goto (file, temp_buffer);
		}

		// *** GFX MODE ***

		else if (strcmp (command_token, "mode") == 0) {
			// mode TEXT, GFX, GFX_SQ, GFX_MED, GFX_HI
			char *mode = get_token (1);

			utils_tolower (mode);
			backend_set_mode (mode);
			backend_statusbar (clr_statusbar_1, clr_statusbar_2, str_status_top, str_status_bottom, attempts);

		}

		// *** PRESENTS.EXE ***

		else if (strcmp (command_token, "pic") == 0) {
			// pic "pic.gif" seconds|kbd
			backend_gif_at (main_path_spec, get_token (1), 0, 0, 1);

			char *duration = get_token (2);
			utils_tolower (duration);

			if (strcmp (duration, "kbd") == 0) {
				backend_pause ();
			} else {
				// Wait 60 * seconds frames (can be float)
				backend_wait_frames (60 * atof (duration));
			}

		} else if (strcmp (command_token, "cut") == 0) {
			// cut "cut.gif" x y
			backend_gif_at (main_path_spec, get_token (1), flags_parse_value (get_token(2)), flags_parse_value (get_token(3)), 1);

		} else if (strcmp (command_token, "sleep") == 0) {
			// Wait 60 * seconds frames (can be float)
			backend_wait_frames (60 * atof (get_token (1)));

		}

		// *** BULMA ***

		else if (strcmp (command_token, "lin") == 0) {
			backend_bulma_lin (main_path_spec, get_token (1));
		
		} else if (strcmp (command_token, "font") == 0) {
			backend_fancy_font (get_token (1));

		} else if (strcmp (command_token, "pix") == 0) {
			backend_bulma_pix (main_path_spec, get_token (1), strcmp (get_token (2), "dbl") == 0, 1);

		}

		// *** ACTIONS ***

		else if (strcmp (command_token, "actions") == 0) {
			char *actions_command = get_token (1);
			utils_tolower (actions_command);

			if (strcmp (actions_command, "reset") == 0 || strcmp (actions_command, "limpia") == 0) {
				actions_reset ();

			} else if (strcmp (actions_command, "put") == 0 || strcmp (actions_command, "pon") == 0) {
				if (
					menu_get_options () < MAX_ACTIONS && 
					strlen (get_token (2)) < ACTIONS_MAX_LENGTH
				) actions_add_item (get_token (2), menu_get_token_type (get_token (3)));

			} else if (strcmp (actions_command, "remove") == 0 || strcmp (actions_command, "quita") == 0) {
				actions_delete_item (get_token (2));

			}
		}

		// *** ZONES ***

		else if (strcmp (command_token, "zones") == 0) {
			char *zones_command = get_token (1);
			utils_tolower (zones_command);

			if (strcmp (zones_command, "reset") == 0 || strcmp (zones_command, "limpia") == 0) {
				zones_reset ();

			} else if (strcmp (zones_command, "def") == 0) {
				// "texto" x1 y1 x2 y2 [actions]
				zones_add_item (
					get_token (2),
					flags_parse_value (get_token (3)),
					flags_parse_value (get_token (4)),
					flags_parse_value (get_token (5)),
					flags_parse_value (get_token (6)),
					zones_get_token_type (get_token (7))
				);

			} else if (strcmp (zones_command, "run") == 0) {

				int zone = backend_zones_run ();
				if (zone >= 0) {

					int type = zones_get_type (zone);

					strcpy (temp_buffer, get_token (2)); 				// :label
					strcat (temp_buffer, "_");                          // _
					strcat (temp_buffer, zones_get_text (zone));        // ZONE

					if (type == ZONE_TYPE_ACTIONS) {

						// Display actions menu
						int x = backend_zones_last_x ();
						int y = backend_zones_last_y ();

						x -= backend_menu_get_w () / 2;
						y -= actions_get_actions () / 2;

						int action = backend_actions_run (x, y);
						if (action >= 0) {
							strcat (temp_buffer, "_");
							strcat (temp_buffer, actions_get_action (action));

							// Get action type
							int action_type = actions_get_type (action);

							// Show items?
							if (action_type == ACTIONS_TYPE_ITEMS) {
								int item = backend_inventory_run_xy (x, y);

								if (item >= 0) {
									strcat (temp_buffer, "_");
									strcat (temp_buffer, inventory_get_item (item));
								}
							}
						}

					}

					lbasi_goto (file, temp_buffer);
				}
			}

		} else if (strcmp (command_token, "infobar") == 0) {
			char *infobar_command = get_token (1);
			utils_tolower (infobar_command);

			if (strcmp (infobar_command, "off") == 0) {
				backend_set_info_bar (-1, 0, 0);

			} else {
				backend_set_info_bar (
					flags_parse_value (get_token (1)),
					flags_parse_value (get_token (2)),
					flags_parse_value (get_token (3))
				);

			}

		}

		// *** MENU ***

		else if (strcmp (command_token, "menu") == 0) {
			char *menu_command = get_token (1);
			utils_tolower (menu_command);

			if (strcmp (menu_command, "reset") == 0 || strcmp (menu_command, "limpia") == 0) {
				menu_reset ();
				backend_menu_set_selected (0);

			} else if (strcmp (menu_command, "put") == 0 || strcmp (menu_command, "pon") == 0) {
				if (
					menu_get_options () < MAX_MENU_ITEMS && 
					strlen (get_token (2)) < MENU_ITEM_MAX_LENGTH
				) menu_add_item (get_token (2), menu_get_token_type (get_token (3)));

			} else if (strcmp (menu_command, "remove") == 0 || strcmp (menu_command, "quita") == 0) {
				menu_delete_item (get_token (2));
				menu_reorganize ();

			} else if (strcmp (menu_command, "config") == 0) {
				backend_menu_config (
					flags_parse_value (get_token (2)), 
					flags_parse_value (get_token (3)), 
					flags_parse_value (get_token (4)), 
					flags_parse_value (get_token (5)),
					flags_parse_value (get_token (6))
				);

			} else if (strcmp (menu_command, "run") == 0) {
				int selected = backend_menu_run ();				

				if (selected >= 0) {
					int type = menu_get_option_type (selected);
					int submenu;

					strcpy (temp_buffer, get_token (2));
					strcat (temp_buffer, "_");

					switch (type) {
						case MENU_ITEM_TYPE_ITEMS:
							strcat (temp_buffer, "item_");

							submenu = backend_inventory_run ();
							if (submenu >= 0) {
								strcat (temp_buffer, inventory_get_item (submenu));
							}

							break;

						case MENU_ITEM_TYPE_EXITS:

							submenu = backend_exits_run ();
							if (submenu >= 0) {
								strcpy (temp_buffer, exits_get_option_label (submenu));
							}

							break;

						default:
							strcat (temp_buffer, menu_get_option_text (selected));

							break;
					}

					lbasi_goto (file, temp_buffer);
				}
			}
		}

		// *** INVENTARIO **

		else if (strcmp (command_token, "items") == 0) {
			char *items_command = get_token (1);
			utils_tolower (items_command);

			if (strcmp (items_command, "reset") == 0 || strcmp (items_command, "limpia") == 0) {
				inventory_reset ();
			
			} else if (strcmp (items_command, "size") == 0 || strcmp (items_command, "tam") == 0) {
				inventory_set_max_items (flags_parse_value (get_token (2)));

			} else if (strcmp (items_command, "put") == 0 || strcmp (items_command, "pon") == 0) {
				// TODO : ERROR HANDLE THIS
				inventory_add_item (get_token (2));

			} else if (strcmp (items_command, "remove") == 0 || strcmp (items_command, "quita") == 0) {
				inventory_delete_item (get_token (2));

			} else if (strcmp (items_command, "has") == 0 || strcmp (items_command, "tiene") == 0) {
				if (inventory_has_item (get_token (2))) {
					lbasi_goto (file, get_token (3));
				}

			} else if (strcmp (items_command, "hasnt") == 0 || strcmp (items_command, "notiene") == 0) {
				if (!inventory_has_item (get_token (2))) {
					lbasi_goto (file, get_token (3));
				}

			}
		}

		// *** EXITS ***

		else if (strcmp (command_token, "exits") == 0) {
			char *items_command = get_token (1);
			utils_tolower (items_command);

			if (strcmp (items_command, "reset") == 0 || strcmp (items_command, "limpia") == 0) {
				exits_reset ();
			
			} else if (strcmp (items_command, "put") == 0 || strcmp (items_command, "pon") == 0) {
				// TODO : ERROR HANDLE THIS
				exits_add_item (get_token (2), get_token (3));

			} else if (strcmp (items_command, "remove") == 0 || strcmp (items_command, "quita") == 0) {
				exits_delete_item (get_token (2));

			}
		}

		// *** TALK ***

		else if (strcmp (command_token, "talk") == 0) {
			strcpy (string_convert_buffer, get_token (1));
			char *talk_command = string_convert_buffer;
			utils_tolower (talk_command);

			if (strcmp (talk_command, "config") == 0) {
				backend_talk_config (
					flags_parse_value (get_token (2)), 
					flags_parse_value (get_token (3)), 
					flags_parse_value (get_token (4)), 
					flags_parse_value (get_token (5)),
					flags_parse_value (get_token (6))
				);
			} else {
				backend_talk (get_token (1));
			}
		}

		// *** LEGACY ***

		else if (strcmp (command_token, "<fin>") == 0) {
			backend_pause ();

		} else {
			// read command
			char command_char = line_buffer [0];
			char *argument = line_buffer + 1;
			char *text;
			argument [strlen (argument) - 1] = '\0';

			int n_opt, c_opt;

			//printf ("%d %c [%s]\n", command_char, command_char, argument);

			switch (command_char) {
				case '=': backend_cls (); break;
	
				case '#': backend_beep (); break;

				case '\xA8': backend_pause (); break;

				case 'c': 
					backend_color (
						10 * argument [0] + argument [1], 
						10 * argument [2] + argument [3]
					);
					break;

				case '*':
					// Legacy choice

					n_opt = (int)(argument [1] - '0');
					c_opt = (int)(argument [3] - '0');
					char c;
					char *reader = argument + 4;

					// Read & parse prints into get_token (3) onwards
					for (int i = 0; i < n_opt; i ++) {
						text = get_token (i + 3);
						
						while ((c = *reader ++) != ')') {
							*text ++ = c;
						}
						
						*text = '\0';
					}

					while ((choice_res = backend_choice (
						n_opt, 		// Number of choices. Beware using flags here!
						c_opt, 		// Correct choice. 
						get_tokens ()
					)) == 0 && !backend_get_break ()) {
						attempts --;
						backend_statusbar (clr_statusbar_1, clr_statusbar_2, str_status_top, str_status_bottom, attempts);

						if (attempts >= 0) {
							backend_try_again (str_try_again, attempts);
						} else {
							res = 4;
							run = 0;
							break;
						}
					}

					if (choice_res < 0) {
						res = choice_res;
						run = 0;
					}
					
					break;
				case 'p': backend_print_ln (argument); break;
	
				case '\x87': backend_center (argument); break;
	
				case '@':
					// Legacy cursor
					// TODO
					break;

				case 'd':
					text = draw_buffer;
					while (fgets (legacy_buffer, LEGACY_BUFFER_SIZE, file) != NULL) {
						if (strcmp (legacy_buffer, "<END>") == 0) break;
						*text ++ = legacy_buffer [0];
					}
					*text = '\0';

					backend_draw (draw_buffer);
					break;

				case '-':
					// Legacy play					
					break;
			}
			
		}

		// Update screen, etc
		if (backend_heartbeat ()) {
			run = 0;
		}

		if (backend_get_break ()) {
			run = 0;
		}
	}

	lstokens_free ();

	return res;
}

int lbasi_run_tmp (char *tmp, char *spec) {
	int res;

	if (spec != NULL) {
		update_path_spec (spec);
		sprintf (str_status_top, "LBASIC: %s", spec);
	} else {
		strcpy (str_status_top, "LBASIC: ?");
	}

	init_strings ();

	flags_clear ();
	menu_reset ();
	inventory_reset ();
	exits_reset ();
	actions_reset ();
	zones_reset ();
	
	attempts = DEFAULT_ATTEMPTS;

	FILE *file;
	if ((file = fopen (tmp, "r")) != NULL) {
		backend_init ();
		backend_color (7, 0);
		backend_cls ();

		backend_set_viewport (1, 23);
		backend_set_show_status (1);
		backend_statusbar (clr_statusbar_1, clr_statusbar_2, str_status_top, str_status_bottom, attempts);

		res = lbasi_run_file (file);
		
		fclose (file);
	}

	return res;
}

int lbasi_run (char *spec, int autoboot) {
	int res = autoboot ? 2 : 1;
	int cur_block = 0;
	int playing = 1;
	int backend_on = 0;

	char file_extension [5];
	char *filename;

	FILE *file;

	init_strings ();
	
	flags_clear ();
	menu_reset ();
	attempts = DEFAULT_ATTEMPTS;

	while (playing) {
		// Make filename
		
		sprintf (file_extension, ".%03d", cur_block); 			// Generate a .XXX extension
		filename = strdup (spec); 								// Duplicate spec, pointed by fileaname
		filename = realloc (filename, strlen (spec) + 4);		// Add space for the extension
		strcat (filename, file_extension); 						// Concatenate spec + extension

		sprintf (str_status_top, "LBASIC: %s, BLOQUE: %d", spec, cur_block);

		if ((file = fopen (filename, "r")) != NULL) {			
			update_path_spec (filename);

			if (!backend_on) {
				backend_init ();
				backend_on = 1;
			}

			// Now let's parse.

			backend_statusbar (clr_statusbar_1, clr_statusbar_2, str_status_top, str_status_bottom, attempts);

			res = lbasi_run_file (file);
			fclose (file);

			if (res < 0) {
				playing = 0;
			} else switch (res) {
				case 0:
					// Next block
					cur_block ++;
					break;

				case 4:
					// Out of attempts
					backend_print_ln (str_game_over);
					backend_print_ln (str_continue);
					backend_print_ln (str_restart);
					backend_print_ln (str_exit);

					attempts = DEFAULT_ATTEMPTS;

					switch (backend_read_option (3)) {
						case 1:
							// Do nothing, will reload the same block
							break;
						case 2:
							cur_block = 0;
							break;
						case 3:
							playing = 0;
							break;
					}

					break;
			}

		} else {
			playing = 0;
		}

		free (filename);
	}

	backend_shutdown ();

	return res;
}