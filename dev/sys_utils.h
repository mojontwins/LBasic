#ifndef SYSUTILS_H
#define SYSUTILS_H

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
void menu_add_item (unsigned char *item, int type);
void menu_reorganize (void);
void menu_delete_item (unsigned char *item);
int menu_get_options (void);
unsigned char *menu_get_option_text (int index);
int menu_get_option_type (int index);

/*
 * INVENTORY
 */

#define MAX_INVENTORY_ITEMS 256
#define INVENTORY_ITEM_MAX_LENGTH 40

#define MENU_ITEM_TYPE_NORMAL 0
#define MENU_ITEM_TYPE_ITEMS 1

void inventory_reset (void);
void inventory_set_max_items (int max_items);
int menu_get_token_type (unsigned char *text);
int inventory_add_item (unsigned char *item);
void inventory_delete_item (unsigned char *item);
int inventory_get_options (void);
unsigned char *inventory_get_option (int index);
int inventory_has_item (unsigned char *item);
int inventory_find_index (unsigned char *item);
int inventory_get_items (void);
unsigned char *inventory_get_item (int index);

#endif
