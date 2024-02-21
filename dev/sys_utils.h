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

void labels_clear (void);
int labels_add (int file_pos, char *label);
int labels_find (char *label);
int labels_get_filepos (int index);

/*
 * MENUS
 */

#define MAX_MENU_ITEMS 10
#define MENU_ITEM_MAX_LENGTH 40

void menu_reset (void);
void menu_add_item (unsigned char *item);
void menu_reorganize (void);
void menu_delete_item (unsigned char *item);
int menu_get_options (void);
unsigned char *menu_get_option (int index);

/*
 * INVENTORY
 */

#define MAX_INVENTORY_ITEMS 256
#define INVENTORY_ITEM_MAX_LENGTH 40

void inventory_reset (void);
void inventory_set_max_items (int max_items);
int inventory_add_item (unsigned char *item);
void inventory_delete_item (unsigned char *item);
int inventory_get_options (void);
unsigned char *inventory_get_option (int index);
int inventory_has_item (unsigned char *item);
int inventory_find_index (unsigned char *item);
