#include <stdio.h>
#include <stdlib.h>

#define _MSC_VER 1400
#include "tinydir.h"

char *get_file_spec (void) {
	tinydir_dir dir;

	int done = 0;
	int new_path = 1;
	char *cur_dir_name;
	char *spec;

	// TODO:: Error handle this

	while (!done) {
		if (new_path) {
			tinydir_open_sorted(&dir, ".");

			for (int i = 0; i < dir.n_files; i ++) {
				tinydir_file file;

				// TODO:: Error handle this
				tinydir_readfile_n (&dir, &file, i);

				if (file.is_dir) {
					printf("[%s/]\t", file.name);
				} else {
					printf("%s\t", file.name);
				}
			}

			new_path = 1;
			done = 1;
		}
	}

	// shutdown
	tinydir_close(&dir);
	free(cur_dir_name);

	return spec;
}

void main(void) {
	char *spec= get_file_spec ();;
	printf ("Result: %s", spec);
	free (spec);
}