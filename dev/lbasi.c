// LBasic Interpreter 
// Hybrid LBasic 6.03-8.5 + extensions

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef DOSLIKE
#include "../dos-like/source/dos.h"
#endif

#include "lstokens.h"
#include "backend.h"
#include "sys_utils.h"

#define LINE_BUFFER_SIZE 2048
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

int get_clr_statusbar1 (void) {
	return clr_statusbar_1;
}

int get_clr_statusbar2 (void) {
	return clr_statusbar_2;
}

char *get_str_try_again (void) {
	return str_try_again;
}

int lbasi_run_file (FILE *file) {
	int res = 0;
	int choice_res;
	int run = 1;

	char line_buffer [LINE_BUFFER_SIZE];
	char temp_buffer [256];
	char *command_token;

	while (run && fgets (line_buffer, LINE_BUFFER_SIZE, file) != NULL) {
		// Tokenize line_buffer
		parse_to_tokens (line_buffer);
		if (get_num_tokens () == 0) continue;

		// Execute
		command_token = get_token (0);
		utils_tolower (command_token);

		if (strcmp(command_token, "print") == 0) {
			backend_print_ln (get_token (1));

		} else if (strcmp(command_token, "write") == 0) {
			backend_print (get_token (1));

		} else if(strcmp (command_token, "center") == 0) {
			backend_center (get_token (1));

		} else if(strcmp (command_token, "color") == 0) {
			backend_color (atoi (get_token (1)), atoi (get_token (2)));

		} else if(strcmp (command_token, "pause") == 0) {
			backend_pause ();

		} else if(strcmp (command_token, "beep") == 0) {
			backend_beep ();

		} else if(strcmp (command_token, "cls") == 0) {
			backend_cls ();

		} else if(strcmp (command_token, "draw") == 0) {
			backend_draw (get_token (1));

		} else if(strcmp (command_token, "choice") == 0) {
			while ((choice_res = backend_choice (
				atoi (get_token (1)),
				atoi (get_token (2)),
				get_tokens ()
			)) == 0) {
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

		} else if(strcmp (command_token, "attempts") == 0) {
			attempts = atoi (get_token (1));

		} else if(strcmp (command_token, "let") == 0) {
			int flag = flags_parse_value (get_token (1));
			int value = flags_parse_value (get_token (2));

			flags_set (flag, value);
		} else if (strcmp (command_token, "input") == 0) {
			int flag = flags_parse_value (get_token (1));			
			itoa (backend_read_option (0), temp_buffer, 10);
			int value = flags_parse_value (temp_buffer);

			flags_set (flag, value);

		} else if (strcmp (command_token, "tell") == 0) {
			int flag = flags_parse_value (get_token (1));
			itoa (flag, temp_buffer, 10);
			backend_print (temp_buffer);

		}

		// Update screen, etc
		if (backend_heartbeat ()) {
			run = 0;
		}
	}

	return res;
}

void lbasi_read_labels (FILE *file) {
	char line_buffer [LINE_BUFFER_SIZE];
	
	while (fgets (line_buffer, LINE_BUFFER_SIZE, file) != NULL) {
		if (line_buffer [0] == ':') {
			if (labels_add (ftell (file), line_buffer) == 0) {
				backend_print_ln ("Warning! label too long:");
				backend_print_ln (line_buffer);
			}
		}
	}

	fseek (file, 0, SEEK_SET);
}

void lbasi_goto (FILE *file, char *label) {
	int label_index = labels_find (label);
	int label_filepos = labels_get_filepos (label_index);

	fseek (file, label_filepos, SEEK_SET);
}

int lbasi_run (char *spec, int autoboot) {
	int res = autoboot ? 2 : 1;
	int cur_block = 0;
	int playing = 1;
	int backend_on = 0;

	char file_extension [5];
	char *filename;

	FILE *file;

	while (playing) {
		// Make filename
		
		sprintf (file_extension, ".%03d", cur_block); 			// Generate a .XXX extension
		filename = strdup (spec); 								// Duplicate spec, pointed by fileaname
		filename = realloc (filename, strlen (spec) + 4);		// Add space for the extension
		strcat (filename, file_extension); 						// Concatenate spec + extension

		sprintf (str_status_top, "LBASIC: %s, BLOQUE: %d", spec, cur_block);

		if ((file = fopen (filename, "r")) != NULL) {
			if (!backend_on) {
				backend_init ();
				backend_on = 1;
			}

			// First find & store all labels

			lbasi_read_labels (file);

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
					backend_print (str_game_over);
					backend_print (str_continue);
					backend_print (str_restart);
					backend_print (str_exit);

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

void main (char argc, char *argv []) {
	int res;
	int autoboot;
	char *spec;

	lstokens_init ();
	init_strings ();

	if (argc == 1) {
		spec = strdup ("boot");
		autoboot = 1;
	} else {
		spec = strdup (argv [1]);
		autoboot = 0;
	}

	attempts = DEFAULT_ATTEMPTS;

	res = lbasi_run (spec, autoboot);

	if (res < 0) {
		// Exit direct

	} else switch (res) {
		case 0:
			// Exit
			break;

		case 1:
			printf ("LBAS Interpreter v0.1.20240201\n");
			printf ("Spec %s does not exist\n", spec);
			break;

		case 2: 
			// Show usage
			printf ("LBAS Interpreter v0.1.20240201\n");
			printf (" $ lbasi <spec> - run <spec>.000, <spec>.001, etc\n");
			printf (" $ lbasi        - use <spec> = boot\n");
			break;

		case 4:
	

			break;

		case 5:
			// Reset
			break;
	}

	free (spec);
	lstokens_free ();
}
