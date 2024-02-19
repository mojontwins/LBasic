#include <string.h>
#include <stdio.h>
#include "sys_utils.h"

/*
 * GENERAL
 */

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

/*
 * FLAGS
 */

int s_flags [MAX_FLAGS];

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

	if (strlen (s)) {
		if (s [0] == '$') {
			return flags_get (flags_parse_value (s + 1));
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

/* 
 * LABELS
 */ 

LABEL labels [MAX_LABELS];
int cur_label = 0;

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

unsigned char menu_items [MAX_MENU_ITEMS][MENU_ITEM_MAX_LENGTH];
int menu_index = 0;

void menu_reset (void) {
	menu_index = 0;
	for (int i = 0; i < MAX_MENU_ITEMS; i ++) {
		menu_items [i][0] = 0;
	}
}

void menu_add_item (unsigned char *item) {
	strcpy (menu_items [menu_index ++], item);
}

void menu_reorganize (void) {
	// Clear gaps in the menu

	for (int i = menu_index - 2; i >= 0; i --) {
		if (menu_items [i][0] == 0) {
			for (int j = i; j < menu_index - 1; j ++) {
				strcpy (menu_items [j], menu_items [j + 1]);
				menu_index --;
			}
		}
	}
}

void menu_delete_item (unsigned char *item) {
	for (int i = 0; i < menu_index; i ++) {
		if (strcmp (item, menu_items [i]) == 0) menu_items [i][0] = 0;
	}
}

int menu_get_options (void) {
	return menu_index;
}

unsigned char *menu_get_option (int index) {
	return menu_items [index];
}
