#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sys_utils.h"

/*
 * GENERAL
 */

int menu_added;
int menu_removed;
int menu_last;

void utils_tolower (char *s) {
	char *p;
	for (p = s; *p; ++p) {
		*p = *p > 0x40 && *p < 0x5b ? *p | 0x60 : *p;
	}
}

void utils_nospaces (char *s) {
	char *p;
	for (p = s; *p; ++p) {
		*p = *p == 32 ? '_' : *p;
	}
}

int utils_adjust_coords (int coord, int min, int max) {
	if (coord < min) return min;
	if (coord > max) return max;
	return coord;
}

char *strndup (char *str, size_t chars) {
    char *buffer;
    int n;

    buffer = (char *) malloc (chars + 1);
    if (buffer) {
        for (n = 0; n < chars && str[n] != 0; n++) buffer [n] = str [n];
        buffer [n] = 0;
    }

    return buffer;
}

void rtrim (char *str) {
	int n = strlen (str);
	while (n >= 0 && str [n] <= ' ') n --;
	str [n + 1] = 0; 
}

int save_game (char *pathspec, int save_number) {
	// Won't save too high
	if (save_number > 255) return 0;

	char name_buffer[512];
	sprintf (name_buffer, "%sSAVEGAME.S%2X", pathspec, save_number);

 	FILE *pf = fopen (name_buffer, "wb");
 	if (!pf) return 0;
 	
 	save_all_flags (pf);
 	save_all_inventory (pf);
 	fclose (pf);

 	return 1;
}

int load_game (char *pathspec, int save_number) {
	// Won't save too high
	if (save_number > 255) return 0;

	char name_buffer[512];
	sprintf (name_buffer, "%sSAVEGAME.S%2X", pathspec, save_number);

 	FILE *pf = fopen (name_buffer, "rb");
 	if (!pf) return 0;

 	load_all_flags (pf);
 	load_all_inventory (pf);
 	fclose (pf);
 	
 	return 1;
}

/*
 * FLAGS
 */

int s_flags [MAX_FLAGS];
char aliases [MAX_FLAGS][MAX_ALIAS_LENGTH];

int aliases_index = 0;

void flags_clear (void) {
	memset (s_flags, 0, MAX_FLAGS * sizeof (int));
}

int flags_get (int index) {
	return s_flags [index];
}

void flags_set (int index, int value) {
	s_flags [index] = value;
}

int flags_parse_value (char *s) {
	// Parse an encoded value with possible flags
	// 1, $3, $$2, etc are allowed.
	// *3 returns 6 (3x2)

	if (strlen (s)) {
		if (s [0] == '@') {
			if (strcmp (s, "@LAST_MENU_OPTION") == 0) {
				return menu_last;
			} else if (strcmp (s, "@MENU_OPTIONS_ADDED") == 0) {
				return menu_added;
			} else if (strcmp (s, "@MENU_OPTIONS_REMOVED") == 0) {
				return menu_removed;
			}
		} else if (s [0] == '*') {
			return flags_parse_value (s + 1) * 2;
		} else if (s [0] == '$') {
			return flags_get (flags_parse_value (s + 1));
		} else if (s [0] == '%') {
			int i = flags_find_alias (s + 1);
			if (i >= 0) return flags_get (i);
			return 0;
		} else {
			// Compose number
			char c;
			int value = 0;

			while (c = *s ++) {
				if (c >= '0' && c <= '9') {
					value = value * 10 + c - '0';
				}
			}

			return value;
		}
	}

	return 0;
}

int flags_parse_lvalue (char *s) {
	// Means one indirection less. 
	// $4 -> 4, $$4 -> flags [4]
	// %cat -> 4 , $%cat -> flags [4]

	if (strlen (s)) {
		if (s [0] == '$') {
			return flags_parse_value (s + 1);
		} else if (s [0] == '%') {
			int i = flags_find_alias (s + 1);
			if (i >= 0) return i;
		} 
	}

	return -1;
}

int flags_reset_aliases (void) {
	aliases_index = 0;
}

int flags_find_alias (char *s) {
	for (int i = 0; i < aliases_index; i ++) {
		if (strcmp (s, aliases [i]) == 0) return i;
	}

	return -1;
}

int flags_find_or_create_alias (char *s) {
	int i = flags_find_alias (s);
	if (i >= 0) return i;

	// Create
	if (aliases_index < MAX_FLAGS - 1) {
		strcpy (aliases [aliases_index ++], s);
	}

	return aliases_index - 1;
}

void save_all_flags (FILE *file) {
	fwrite (aliases, sizeof(char), sizeof(aliases), file);
	fwrite (flags, sizeof(int), MAX_FLAGS, file);
}

void load_all_flags (FILE *file) {
	fread (aliases, sizeof(char), sizeof(aliases), file);
	fread (flags, sizeof(int), MAX_FLAGS, file);
}

/* 
 * LABELS
 */ 

typedef struct LABEL {
	int file_pos;
	char text [LABEL_LEN + 1];
} LABEL;

LABEL labels [MAX_LABELS];
int cur_label = 0;

unsigned char *ret_label = NULL;

void labels_set_ret (unsigned char *ret) {
	ret_label = ret;
}

void labels_clear (void) {
	cur_label = 0;
}

char *labels_copy_clean (char *dst, char *src) {
	char *dstp = dst;
	char *srcp = src;
	char c;

	while ((c = *srcp ++) && c != '\n' && c != '\r') {
		*dstp ++ = c;
	}

	return dst;
}

int labels_add (int file_pos, char *label) {
	if (cur_label == MAX_LABELS || strlen (label) > LABEL_LEN) return 0;

	labels [cur_label].file_pos = file_pos;
	strcpy (labels [cur_label].text, label);
	utils_tolower (labels [cur_label].text);

	cur_label ++;

	return 1;
}

int labels_find (char *label) {
	if (strcmp ("ret", label) == 0) {
		label = ret_label;
	}
	
	utils_tolower (label);
	utils_nospaces (label);

	for(int i = 0; i < cur_label; i ++) {
		if (strcmp (labels [i].text, label) == 0) return i;
	}

	return -1;
}

int labels_get_filepos (int index) {
	if (index >= 0 && index < cur_label) return labels [index].file_pos;
	return 0;
}

/*
 * MENUS
 */

typedef struct MENU_ITEM {
	unsigned char text [MENU_ITEM_MAX_LENGTH];
	int type;
	char label [LABEL_LEN];
} MENU_ITEM;

MENU_ITEM menu_items [MAX_MENU_ITEMS];
int menu_index = 0;

void menu_reset (void) {
	menu_index = 0;
	for (int i = 0; i < MAX_MENU_ITEMS; i ++) {
		menu_items [i].text [0] = 0;
	}
	menu_added = 0;
	menu_removed = 0;
	menu_last = -1;
}

int menu_get_token_type (unsigned char *text) {
	if (text [0] == ':') return MENU_ITEM_TYPE_LABEL_DIRECT;
	if (strcmp (text, "items") == 0) return MENU_ITEM_TYPE_ITEMS;
	if (strcmp (text, "exits") == 0) return MENU_ITEM_TYPE_EXITS;

	return MENU_ITEM_TYPE_NORMAL;
}

void menu_add_item (unsigned char *item, int type, unsigned char *label) {
	if (strlen (item) < MENU_ITEM_MAX_LENGTH) {
		menu_items [menu_index].type = type;
		strcpy (menu_items [menu_index].text, item);
		if (type == MENU_ITEM_TYPE_LABEL_DIRECT) {
			strcpy (menu_items [menu_index].label, label);
		} else {
			menu_items [menu_index].label [0] = 0;
		}
		menu_index ++;
		menu_added ++;
	}
}

void menu_reorganize (void) {
	// Clear gaps in the menu

	for (int i = 0; i < menu_index; i ++) {
		if (menu_items [i].text [0] == 0) {
			// There's a gap! shift everything up!
			for (int j = i + 1; j <= menu_index; j ++) {
				strcpy (menu_items [j - 1].text, menu_items [j].text);
				menu_items [j - 1].type = menu_items [i].type;
			}

			// One menu item less!
			menu_index --;
		}
	}
}

void menu_delete_item (unsigned char *item) {
	for (int i = 0; i < menu_index; i ++) {
		if (strcmp (item, menu_items [i].text) == 0) {
			menu_items [i].text [0] = 0;
			menu_removed ++;
		}
	}
}

int menu_get_options (void) {
	return menu_index;
}

unsigned char *menu_get_option_text (int index) {
	return menu_items [index].text;
}

int menu_get_option_type (int index) {
	return menu_items [index].type;
}

unsigned char *menu_get_label (int index) {
	return menu_items [index].label;
}

void menu_set_last_selected (int selected) {
	menu_last = selected;
}

int menu_has_item (unsigned char *item) {
	for (int i = 0; i < menu_index; i ++) {
		if (strcmp (item, menu_items [i].text) == 0) return 1;
	}

	return 0;
}

// To make backups

MENU_ITEM *backup_menu_items = NULL;
int backup_menu_index = 0;

void menu_create_backup (void) {
	if (backup_menu_items) free (backup_menu_items);
	backup_menu_items = malloc (sizeof (MENU_ITEM) * menu_index);
	memcpy (backup_menu_items, menu_items, sizeof (MENU_ITEM) * menu_index);
	backup_menu_index = menu_index;
}

void menu_restore_backup (void) {
	memcpy (menu_items, backup_menu_items, sizeof (MENU_ITEM) * backup_menu_index);
	menu_index = backup_menu_index;
}

void menu_destroy_backup (void) {
	if (backup_menu_items) free (backup_menu_items);
}

/*
 * INVENTORY
 */

unsigned char inventory_items [MAX_INVENTORY_ITEMS][INVENTORY_ITEM_MAX_LENGTH];
int inventory_index = 0;
int inventory_max_items = MAX_INVENTORY_ITEMS;

void inventory_reset (void) {
	inventory_index = 0;
	for (int i = 0; i < MAX_INVENTORY_ITEMS; i ++) {
		inventory_items [i][0] = 0;
	}
}

void inventory_set_max_items (int max_items) {
	inventory_max_items = max_items;
}

int inventory_add_item (unsigned char *item) {
	if (strlen (item) < INVENTORY_ITEM_MAX_LENGTH) {
		if (inventory_index < inventory_max_items - 1) {
			strcpy (inventory_items [inventory_index ++], item);
			return 1;
		}
	}

	return 0;
}

void inventory_reorganize (void) {
	// Clear gaps in the inventory

	for (int i = 0; i < inventory_index; i ++) {
		if (inventory_items [i][0] == 0) {
			// There's a gap! shift everything up!
			for (int j = i + 1; j <= inventory_index; j ++) {
				strcpy (inventory_items [j - 1], inventory_items [j]);
			}

			// One menu item less!
			inventory_index --;
		}
	}
}

void inventory_delete_item (unsigned char *item) {
	for (int i = 0; i < inventory_index; i ++) {
		if (strcmp (item, inventory_items [i]) == 0) inventory_items [i][0] = 0;
	}

	inventory_reorganize ();
}

int inventory_get_options (void) {
	return inventory_index;
}

unsigned char *inventory_get_option (int index) {
	return inventory_items [index];
}

int inventory_has_item (unsigned char *item) {
	for (int i = 0; i < inventory_index; i ++) {
		if (strcmp (item, inventory_items [i]) == 0) return 1;
	}

	return 0;
}

int inventory_find_index (unsigned char *item) {
	for (int i = 0; i < inventory_index; i ++) {
		if (strcmp (item, inventory_items [i]) == 0) return i;
	}

	return -1;
}

int inventory_get_items (void) {
	return inventory_index;
}

unsigned char *inventory_get_item (int index) {
	return inventory_items [index];
}

void save_all_inventory (FILE *file) {
	fwrite (&inventory_index, sizeof (int), 1, file);
	fwrite (&inventory_max_items, sizeof (int), 1, file);
	fwrite (inventory_items, sizeof(char), MAX_INVENTORY_ITEMS * INVENTORY_ITEM_MAX_LENGTH, file);
}

void load_all_inventory (FILE *file) {
	fread (&inventory_index, sizeof (int), 1, file);
	fread (&inventory_max_items, sizeof (int), 1, file);
	fread (inventory_items, sizeof(char), MAX_INVENTORY_ITEMS * INVENTORY_ITEM_MAX_LENGTH, file);
}

/*
 * EXITS
 */

typedef struct EXIT {
	unsigned char text [EXIT_TEXT_MAX_LENGTH];
	unsigned char label [EXIT_LABEL_MAX_LENGTH];
} EXIT;

EXIT exits_items [MAX_EXITS];
int exits_index;

void exits_reset (void) {
	exits_index = 0;
	for (int i = 0; i < MAX_EXITS; i ++) {
		exits_items [i].text [0] = 0;
	}
}

int exits_add_item (unsigned char *text, unsigned char *label) {
	if (exits_index < MAX_EXITS && strlen (text) < EXIT_TEXT_MAX_LENGTH && strlen (label) < EXIT_LABEL_MAX_LENGTH) {
		strcpy (exits_items [exits_index].text, text);
		strcpy (exits_items [exits_index].label, label);
		exits_index ++;
		return 1;
	}

	return 0;
}

void exits_reorganize (void) {
	// Clear gaps in the exits

	for (int i = 0; i < exits_index; i ++) {
		if (exits_items [i].text [0] == 0) {
			// There's a gap! shift everything up!
			for (int j = i + 1; j <= exits_index; j ++) {
				strcpy (exits_items [j - 1].text, exits_items [j].text);
				strcpy (exits_items [j - 1].label, exits_items [j].label);
			}

			// One menu item less!
			exits_index --;
		}
	}
}

void exits_delete_item (unsigned char *item) {
	for (int i = 0; i < exits_index; i ++) {
		if (strcmp (item, exits_items [i].text) == 0) {
			exits_items [i].text [0] = 0;
		}
	}

	exits_reorganize ();
}

int exits_get_options (void) {
	return exits_index;
}

unsigned char *exits_get_option_text (int index) {
	return exits_items [index].text;
}

unsigned char *exits_get_option_label (int index) {
	return exits_items [index].label;
}

/*
 * ACTIONS
 */

typedef struct ACTION {
	unsigned char text [ACTIONS_MAX_LENGTH];
	int type;
} ACTION;

ACTION actions [MAX_ACTIONS];
int actions_index;

void actions_reset (void) {
	actions_index = 0;
	for (int i = 0; i < MAX_ACTIONS; i ++) {
		actions [i].text [0] = 0;
	}
}

int actions_add_item (unsigned char *text, int type) {
	if (actions_index < MAX_ACTIONS && strlen (text) < ACTIONS_MAX_LENGTH) {
		strcpy (actions [actions_index].text, text);
		actions [actions_index].type = type;
		actions_index ++;
		return 1;
	}

	return 0;
}

void actions_reorganize (void) {
	// Clear gaps in the actions

	for (int i = 0; i < actions_index; i ++) {
		if (actions [i].text [0] == 0) {
			// There's a gap! shift everything up!
			for (int j = i + 1; j <= actions_index; j ++) {
				strcpy (actions [j - 1].text, actions [j].text);
				actions [j - 1].type = actions [j].type;
			}

			// One menu item less!
			actions_index --;
		}
	}
}

void actions_delete_item (unsigned char *item) {
	for (int i = 0; i < actions_index; i ++) {
		if (strcmp (item, actions [i].text) == 0) actions [i].text [0] = 0;
	}

	actions_reorganize ();
}

int actions_get_actions (void) {
	return actions_index;
}

unsigned char *actions_get_action (int index) {
	return actions [index].text;
}

int actions_get_type (int index) {
	return actions [index].type;
}

/*
 * ZONES
 */

typedef struct ZONE {
	char text [ZONE_TEXT_MAX_LENGTH];
	int x1;
	int y1;
	int x2;
	int y2;
	int type;
	char label [LABEL_LEN];
} ZONE;

ZONE zones [MAX_ZONES];
int zones_index;

int zones_get_x1 (int index) {
	return zones [index].x1;
}

int zones_get_y1 (int index) {
	return zones [index].y1;
}

int zones_get_x2 (int index) {
	return zones [index].x2;
}

int zones_get_y2 (int index) {
	return zones [index].y2;
}

void zones_reset (void) {
	zones_index = 0;
	for (int i = 0; i < MAX_ZONES; i ++) {
		zones [i].text [0] = 0;
	}
}

int zones_add_item (unsigned char *text, int x1, int y1, int x2, int y2, int type, unsigned char *label) {
	if (zones_index < MAX_ZONES && strlen (text) < ZONE_TEXT_MAX_LENGTH) {
		strcpy (zones [zones_index].text, text);
		zones [zones_index].x1 = x1;
		zones [zones_index].y1 = y1;
		zones [zones_index].x2 = x2;
		zones [zones_index].y2 = y2;
		zones [zones_index].type = type;
		if (type == ZONE_TYPE_LABEL_DIRECT) {
			strcpy (zones [zones_index].label, label);
		}
		zones_index ++;
		return 1;
	}

	return 0;
}

void zones_reorganize (void) {
	// Clear gaps in the zones

	for (int i = 0; i < zones_index; i ++) {
		if (zones [i].text [0] == 0) {
			// There's a gap! shift everything up!
			for (int j = i + 1; j <= zones_index; j ++) {
				strcpy (zones [j - 1].text, zones [j].text);
				zones [j - 1].x1 = zones [j].x1;
				zones [j - 1].y1 = zones [j].y1;
				zones [j - 1].x2 = zones [j].x2;
				zones [j - 1].y2 = zones [j].y2;
				zones [j - 1].type = zones [j].type;
			}

			// One menu item less!
			zones_index --;
		}
	}
}

void zones_delete_item (unsigned char *item) {
	for (int i = 0; i < zones_index; i ++) {
		if (strcmp (item, zones [i].text) == 0) zones [i].text [0] = 0;
	}

	zones_reorganize ();
}

int zones_get_zones (void) {
	return zones_index;
}

char *zones_get_text (int index) {
	return zones [index].text;
}

char *zones_get_label (int index) {
	if (zones [index].type != ZONE_TYPE_LABEL_DIRECT) return NULL;
	return zones [index].label;
}

int zones_find (int x, int y) {
	for (int i = 0; i < zones_index; i ++) {
		if (
			x >= zones [i].x1 && x <= zones [i].x2 &&
			y >= zones [i].y1 && y <= zones [i].y2
		) {
			return i;
		}
	}

	return -1;
}

int zones_get_type (int index) {
	return zones [index].type;
}

int zones_get_token_type (unsigned char *text) {
	if (text [0] == ':') return ZONE_TYPE_LABEL_DIRECT;
	if (strcmp (text, "actions") == 0) return ZONE_TYPE_ACTIONS;

	return ZONE_TYPE_NORMAL;
}

// To make backups

ZONE *backup_zones = NULL;
int backup_zones_index = 0;

void zones_create_backup (void) {
	if (backup_zones) free (backup_zones);
	backup_zones = malloc (sizeof (ZONE) * zones_index);
	memcpy (backup_zones, menu_items, sizeof (ZONE) * zones_index);
	backup_zones_index = zones_index;
}

void zones_restore_backup (void) {
	memcpy (menu_items, backup_zones, sizeof (ZONE) * backup_zones_index);
	zones_index = backup_zones_index;
}

void zones_destroy_backup (void) {
	if (backup_zones) free (backup_zones);
}

/*
 * RESP
 */

char resp [MAX_RESP][RESP_MAX_LENGTH];
int resp_index;

void resp_reset (void) {
	resp_index = 0;
}

void resp_add (char *s) {
	if (resp_index < MAX_RESP - 1 && strlen (s) < RESP_MAX_LENGTH) {
		strcpy (resp [resp_index ++], s);
	}
}

int resp_get_resps (void) {
	return resp_index;
}

char *resp_get_text (int index) {
	return resp [index];
}
