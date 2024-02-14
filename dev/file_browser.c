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
		ptr [1] = 0;
	}

	return bytes;
} 

void *get_file_spec (char *file_spec) {
	tinydir_dir dir;

	int done = 0;
	int new_path = 1;
	int first_line_to_display;

	char cwd [256];
	char **file_list;
	int file_count = 0;
	int must_close = 0;	// This is a bit lame, think of a better way
	int selected_file = 0;

	char debug[256];

	char c;
	get_cwd (cwd, sizeof(cwd));

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
				buf_print_abs (file.name);
				buf_char (']');
			} else {
				if (selected_file == idx) {
					buf_color (0, 7);
				} else {
					buf_color (7, 0);
				}
				buf_print_abs (file.name);				
			}

			idx ++;
			y ++;
		}

		// Get input
		unsigned char const* chars = (unsigned char*) readchars ();
		enum keycode_t* keys = readkeys ();

		// Characters
		c = get_character_input (chars);

		if (c >= ' ') {

		} else {
			switch (c) {
				case 8:
					// Delete
					break;

				case 27:
					// Exit
					break;
			}
		}

		// Special keys
		while (*keys) {
			unsigned long long key = (unsigned long long) *keys;

			if (key == KEY_UP) {
				if (selected_file > 0) selected_file --;
			}

			if (key == KEY_DOWN) {
				if (selected_file < dir.n_files - 1) selected_file ++;
			}

			if (key == KEY_RETURN) {
				tinydir_file file;
				tinydir_readfile_n (&dir, &file, selected_file);

				if (file.is_dir) {
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
							ptr [1] = 0;
						}
					} else {
						if (strlen (file.name) + strlen (cwd) < 256) {
							/*
							#ifdef _WIN32
								strcat (cwd, "\\");
							#else
								strcat (cwd, "/");
							#endif
							*/
							strcat (cwd, file.name);
						}
					}
					
					new_path = 1;
				}
			}

			keys ++;
		}

	}

	buf_pause ();
	if (must_close) tinydir_close (&dir);
	file_spec = strdup ("test/test.000"); // DUMMY
}
