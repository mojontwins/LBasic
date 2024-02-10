#include <stdio.h>
#include <stdlib.h>

#define ICE_FS_IMPL
#include "ice_fs.h"

char *get_file_spec (void) {
	int done = 0;
	int new_path = 1;
	ice_is_dir cur_dir = NULL;
	char *cur_dir_name;

	char *spec;

	while (!done) {

		if (new_path) {
			ice_fs_free_dir_content (cur_dir);
			free(cur_dir_name);
			
			cur_dir_name = ice_fs_cwd();
			cur_dir = ice_fs_dir_content (".");

			// Show dir contents
			printf ("%s\n", cur_dir_name);

			// Show contents

			// First, dirs
			for (int i = 0; i < cur_dir.items_count; i ++) {
				ice_fs_object *item = cur_dir.items [i];

				if (item.type == ICE_FS_OBJECT_TYPE_DIR) {
					printf ("[%20s]\t", item.name);
				}
			}
			printf("\n");

			// Then files
			for (int i = 0; i < cur_dir.items_count; i ++) {
				ice_fs_object *item = cur_dir.items [i];

				if (item.type == ICE_FS_OBJECT_TYPE_FILE) {
					printf ("%20s\t", item.name);
				}
			}
		}

	}

	// shutdown
	ice_fs_free_dir_content (cur_dir);
	free(cur_dir_name);

	return spec;
}

void main(void) {
	char *spec;
	printf ("Result: %s", spec);
	free (spec);
}