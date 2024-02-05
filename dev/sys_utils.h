/*
 * GENERAL
 */

void utils_tolower (char *s);

/*
 * FLAGS
 */

#define MAX_FLAGS 256

void flags_clear (void);
int flags_get (int index);
void flags_set (int index, int value);
int flags_parse_value (char *s);

/* 
 * LABELS
 */ 

#define MAX_LABELS 256
#define LABEL_LEN 64

typedef struct LABEL {
	int file_pos;
	char text [LABEL_LEN + 1];
} LABEL;

void labels_clear (void);
int labels_add (int file_pos, char *label);
int labels_find (char *label);
int labels_get_filepos (int index);
