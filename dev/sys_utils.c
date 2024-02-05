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

/*
 * FLAGS
 */

int s_flags [MAX_FLAGS];

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

	cur_label ++;

	return 1;
}

int labels_find (char *label) {
	int res = -1;

	for(int i = 0; i < cur_label; i ++) {
		if (strcmp (labels [i].text, label) == 0) return i;
	}

	return res;
}

int labels_get_filepos (int index) {
	if (index >= 0 && index < cur_label) return labels [index].file_pos;
	return 0;
}
