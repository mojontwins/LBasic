#ifndef SYSUTILS_H
#define SYSUTILS_H

/*
 * GENERAL
 */

void utils_tolower (char *s);
void utils_nospaces (char *s);
int utils_adjust_coords (int coord, int min, int max);

/*
 * FLAGS
 */

#define MAX_FLAGS 256
#define MAX_ALIAS_LENGTH 33

void flags_clear (void);
int flags_get (int index);
void flags_set (int index, int value);
int flags_parse_value (char *s);
int flags_reset_aliases (void);
int flags_find_alias (char *s);
int flags_find_or_create_alias (char *s);

/* 
 * LABELS
 */ 

#define MAX_LABELS 1024
#define LABEL_LEN 80

void labels_clear (void);
int labels_add (int file_pos, char *label);
int labels_find (char *label);
int labels_get_filepos (int index);

/*
 * MENUS
 */

#define MAX_MENU_ITEMS 16
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
#define MENU_ITEM_TYPE_EXITS 2

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

/*
 * EXITS
 */

#define MAX_EXITS 16
#define EXIT_TEXT_MAX_LENGTH 40
#define EXIT_LABEL_MAX_LENGTH 40

void exits_reset (void);
int exits_add_item (unsigned char *text, unsigned char *label);
void exits_reorganize (void);
void exits_delete_item (unsigned char *item);
int exits_get_options (void);
unsigned char *exits_get_option_text (int index);
unsigned char *exits_get_option_label (int index);

/*
 * ACTIONS
 */

#define MAX_ACTIONS 16
#define ACTIONS_MAX_LENGTH 40

#define ACTIONS_TYPE_NORMAL 0
#define ACTIONS_TYPE_ITEMS 1

unsigned char actions_text [MAX_ACTIONS][ACTIONS_MAX_LENGTH];
int actions_index;

void actions_reset (void);
int actions_add_item (unsigned char *text, int type);
void actions_reorganize (void);
void actions_delete_item (unsigned char *item);
int actions_get_actions (void);
unsigned char *actions_get_action (int index);
int actions_get_type (int index);

/*
 * ZONES
 */

#define MAX_ZONES 32
#define ZONE_TEXT_MAX_LENGTH 40

#define ZONE_TYPE_NORMAL 0
#define ZONE_TYPE_ACTIONS 3

void zones_reset (void);
int zones_get_x1 (int index);
int zones_get_y1 (int index);
int zones_get_x2 (int index);
int zones_get_y2 (int index);
int zones_add_item (unsigned char *text, int x1, int y1, int x2, int y2, int type);
void zones_reorganize (void);
void zones_delete_item (unsigned char *item);
int zones_get_zones (void);
char *zones_get_text (int index);
int zones_find (int x, int y);
int zones_get_type (int index);
int zones_get_token_type (unsigned char *text);
#endif
