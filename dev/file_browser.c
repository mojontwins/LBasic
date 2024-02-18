#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../dos-like/source/dos.h"

#include "file_browser.h"
#include "lstextmode.h"
#include "tui.h"
#include "conversion.h"

#ifdef _WIN32
#define _MSC_VER 1400
#endif
#include "tinydir.h"

char *file_spec = NULL;
void clear_file_spec (void) {
	free (file_spec);
	file_spec = NULL;
}

char *get_file_spec (void) {
	return file_spec;
}

int get_cwd (char *pBuf, size_t len) {
	// Code from https://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from
	int bytes;
	char *ptr;

	#ifdef _WIN32
		bytes = GetModuleFileName (NULL, pBuf, len);
		bytes = bytes ? bytes : -1;
		ptr = strrchr (pBuf, '\\');
	#else
		bytes = MIN (readlink ("/proc/self/exe", pBuf, len), len - 1);
		if (bytes >= 0)
			pBuf [bytes] = '\0';
		ptr = strrchr (pBuf, '/');
	#endif
	
	// Now trim the last part (i.e. keep only directory)
	if (ptr) {
		ptr [0] = 0;
	}

	return bytes;
} 

int select_file_spec (void) {
	tinydir_dir dir;

	int res = 1;
	int done = 0;
	int cursor = 0;
	int new_path = 1;
	int first_line_to_display;

	char cwd [512];
	char **file_list;
	int file_count = 0;
	int must_close = 0;	// This is a bit lame, think of a better way
	int selected_file = 0;
	unsigned char user_input [512];

	char debug[256];

	unsigned char c;
	user_input [0] = 0;
	int editing = 0;

	if (file_spec == NULL) {
		// Get cwd from exe location
		get_cwd (cwd, sizeof(cwd));
	} else {
		// Get cwd & user_input from file_spec!
		unsigned char *ptr;
		strcpy (cwd, file_spec);
		#ifdef _WIN32
			ptr = strrchr (cwd, '\\');
		#else
			ptr = strrchr (cwd, '/');
		#endif
		strcpy (user_input, ptr + 1);
		ptr [0] = 0;
		cursor = strlen (user_input);

		// This is done so ENTER just works right away
		editing = 1;
	}

	curson ();

	while (!done && !buf_heartbeat()) {
		if (new_path) {
			if (must_close) tinydir_close (&dir);
			
			tinydir_open_sorted(&dir, cwd);

			new_path = 0;
			
			must_close = 1;
			first_line_to_display = 0;
			selected_file = 0;
		}

		// Display

		buf_color (7, 0);
		buf_clscroll ();

		buf_setxy (0, 1);
		buf_color (15, 0);
		buf_print_ln (cwd);

		// Lines 2 to 22
		if (selected_file < first_line_to_display) {
			first_line_to_display = selected_file;
		}

		while (selected_file > first_line_to_display + 19) {
			first_line_to_display = selected_file - 19;
		}

		int y = 2;
		int idx = first_line_to_display;

		while (y < 22) {
			if (idx >= dir.n_files) break;

			tinydir_file file;
			tinydir_readfile_n (&dir, &file, idx);

			buf_setxy (0, y);
			if (file.is_dir) {
				if (selected_file == idx) {
					buf_color (0, 13);
				} else {
					buf_color (13, 0);
				}
				buf_char ('[');
				buf_print_trim (file.name);
				buf_char (']');
			} else {
				if (selected_file == idx) {
					buf_color (0, 7);
				} else {
					buf_color (7, 0);
				}
				buf_print_trim (file.name);				
			}

			idx ++;
			y ++;
		}
		// Input line
		buf_setxy (0, 22);
		buf_color (7, 1);
		buf_print_abs ("\xC4\xB4 Introduce nombre \xC3\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4");

		gotoxy (cursor, 23);
		buf_setxy (0, 23);
		buf_color (15, 0);
		buf_print_abs (user_input);

		// Get input
		unsigned char const* chars = (unsigned char*) readchars ();
		enum keycode_t* keys = readkeys ();

		// Characters
		c = get_character_input (chars);

		if (c >= ' ') {
			if (cursor < 79) {
				user_input [cursor ++] = c;
				user_input [cursor] = 0;

				editing = 1;
			}
		} else {
			switch (c) {
				case 8:
					// Delete
					if (cursor > 0) {
						cursor --;
						user_input [cursor] = 0;

						editing = 1;
					}
					break;

				case 27:
					// Exit
					done = 1;
					res = 0;
					break;
			}
		}

		// Special keys
		while (*keys) {
			unsigned long long key = (unsigned long long) *keys;

			if (key == KEY_UP) {
				if (selected_file > 0) selected_file --;
				tinydir_file file;
				tinydir_readfile_n (&dir, &file, selected_file);
				if (!file.is_dir) {
					strcpy (user_input, file.name);
					cursor = strlen (user_input);
				}
				editing = 0;
			}

			if (key == KEY_DOWN) {
				if (selected_file < dir.n_files - 1) selected_file ++;
				tinydir_file file;
				tinydir_readfile_n (&dir, &file, selected_file);
				if (!file.is_dir) {
					strcpy (user_input, file.name);
					cursor = strlen (user_input);
				}
				editing = 0;
			}

			if (key == KEY_RETURN) {
				tinydir_file file;
				tinydir_readfile_n (&dir, &file, selected_file);

				if (file.is_dir && !editing) {
					// Change dir
					if (strcmp (file.name, ".") == 0) {
						// NO OP
					} else if (strcmp (file.name, "..") == 0) {
						char *ptr;
						#ifdef _WIN32
							ptr = strrchr (cwd, '\\');
						#else
							ptr = strrchr (cwd, '/');
						#endif
						if (ptr) {
							ptr [0] = 0;
						}
					} else {
						if (strlen (file.name) + strlen (cwd) < 256) {
							#ifdef _WIN32
								strcat (cwd, "\\");
							#else
								strcat (cwd, "/");
							#endif
							strcat (cwd, file.name);
						}
					}
					
					new_path = 1;
				} else {
					free (file_spec);
					file_spec = malloc (strlen (cwd) + 1 + strlen (user_input) + 1);
					strcpy (file_spec, cwd);
					#ifdef _WIN32
						strcat (file_spec, "\\");
					#else
						strcat (file_spec, "/");
					#endif
					strcat (file_spec, user_input);
					done = 1;
				}
			}

			keys ++;
		}

	}

	cursoff ();
	if (must_close) tinydir_close (&dir);

	return res;
}
