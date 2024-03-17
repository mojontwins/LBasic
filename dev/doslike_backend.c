//#define DEBUG

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lbasi.h"
#include "backend.h"
#include "lstextmode.h"
#include "sys_utils.h"

#include "keys.h"
#include "sys_utils.h"

#include "../dos-like/source/dos.h"

#define INPUT_LENGTH 10

char *full_line = "                                                                                ";
int show_status = 1;
int forced_break = 0;

typedef struct BACKEND_MENU {
	int x;
	int y;
	int c1;
	int c2;
	int w;
	int fixed;
	int noframe;
} BACKEND_MENU;

BACKEND_MENU backend_menu;
int backend_menu_selected = 0;

typedef struct BACKEND_TALK {
	int x;
	int y;
	int c1;
	int c2;
	int w;
	int ox;
	int oy;
	int om;
	int ow;
	int oh;
	char *ovl_pixels;
	int ovl_on;
} BACKEND_TALK;

BACKEND_TALK backend_talk;

typedef struct BACKEND_INFO_BAR {
	int y;
	int c1;
	int c2;
} BACKEND_INFO_BAR;

BACKEND_INFO_BAR backend_info_bar;

typedef struct BACKEND_ZONES {
	int last_x;
	int last_y;
} BACKEND_ZONES;

BACKEND_ZONES backend_zones;

typedef struct BACKEND_TB {
	int x;
	int y;
	int w;
	int h;
	int c1;
	int c2;
	int tc1;
	int tc2;
	int f;
} BACKEND_TB;

BACKEND_TB backend_tb_cfg;

typedef struct BACKEND_RESP {
	int y;
	int c1;
	int c2;
} BACKEND_RESP;

BACKEND_RESP backend_resp;

typedef struct BACKEND_INDICATOR {
	int on;
	int x;
	int y;
	int c1;
	int c2;
	unsigned char c;
} BACKEND_INDICATOR;

BACKEND_INDICATOR backend_indicator;

typedef struct BACKEND_BG {
	int x;
	int y;
} BACKEND_BG;

BACKEND_BG backend_bg_cfg;

typedef struct BACKEND_INTERFACE {
	int on;
	char *interface_pixels;
	int x;
	int y;
	int m;
	int w;
	int h;
} BACKEND_INTERFACE;

BACKEND_INTERFACE backend_interface_cfg;

int cur_sound_channel = 0;
struct music_t *mus = NULL;
struct sound_t *sound = NULL;

int menu_items_auto = 0;
char menu_items [MENU_ITEM_MAX_LENGTH];

void backend_init (void) {
	lstextmode_init ();
	buf_color (7, 0);
	buf_clscroll ();
	setsoundbank (DEFAULT_SOUNDBANK_SB16);

	backend_menu.x = 0;
	backend_menu.y = 0;
	backend_menu.c1 = 15;
	backend_menu.c2 = 1;
	backend_menu.w = 16;
	backend_menu.fixed = 0;
	backend_menu.noframe = 0;

	backend_talk.x = 0;
	backend_talk.y = 0;
	backend_talk.c1 = 15;
	backend_talk.c2 = 1;
	backend_talk.w = 16;
	backend_talk.ox = 0;
	backend_talk.oy = 0;
	backend_talk.om = 255;
	backend_talk.ovl_pixels = NULL;
	backend_talk.ovl_on = 0;

	backend_info_bar.y = -1; 	// -1 means OFF
	backend_info_bar.c1 = 0;
	backend_info_bar.c2 = 0;

	backend_zones.last_x = 0;
	backend_zones.last_y = 0;

	backend_tb_cfg.x = 1;
	backend_tb_cfg.y = 1;
	backend_tb_cfg.w = 78;
	backend_tb_cfg.h = 10;
	backend_tb_cfg.c1 = 15;
	backend_tb_cfg.c2 = 7;
	backend_tb_cfg.tc1 = 0;
	backend_tb_cfg.tc2 = 7;
	backend_tb_cfg.f = 8; 

	backend_resp.y = 25;
	backend_resp.c1 = 15;
	backend_resp.c2 = 0;

	backend_indicator.on = 0;
	backend_indicator.x = 0;
	backend_indicator.y = 0;
	backend_indicator.c1 = 7;
	backend_indicator.c2 = 0;
	backend_indicator.c = 16;

	backend_bg_cfg.x = 0;
	backend_bg_cfg.y = 0;

	backend_interface_cfg.interface_pixels = NULL;
	backend_interface_cfg.on = 0;
	backend_interface_cfg.x = 0;
	backend_interface_cfg.y = 0;
	backend_interface_cfg.m = 0;
	backend_interface_cfg.w = 0;
	backend_interface_cfg.h = 0;
}

int backend_get_break (void) {
	return forced_break;
}

char *compute_full_path (char *pathspec, char *filename) {
	char *full_path = malloc (strlen (pathspec) + strlen (filename) + 1);
	strcpy (full_path, pathspec);
	strcat (full_path, filename);
	return full_path;
}

void backend_set_show_status (int i) {
	show_status = i;
}

void backend_statusbar (int clr_statusbar1, int clr_statusbar2, char *string_top, char *string_bottom, int attempts) {
	if (buf_getmode () == LS_MODE_TEXT && show_status) {
		int x, y;
		int c1, c2;
		char attempts_str [3];

		x = buf_getx (); y = buf_gety ();
		c1 = buf_getc1 (); c2 = buf_getc2 ();

		buf_setxy (0, 0);
		buf_color (clr_statusbar1, clr_statusbar2);
		buf_print_abs (full_line);
		
		buf_setxy (0, 0);
		buf_print_abs (string_top);

		buf_setxy (0, 24);
		buf_print_abs (full_line);
		
		buf_setxy (0, 24);
		buf_print_abs (string_bottom);
		sprintf (attempts_str, "%1d", attempts);
		buf_print_abs (attempts_str);

		buf_setxy (x, y);
		buf_color (c1, c2);
	}	
}

void backend_set_viewport (int l1, int l2) {
	buf_setviewport (l1, l2);
}

void backend_print_ln (char *string) {
	buf_print_ln (string);
}

void backend_print (char *string) {
	buf_print (string);
}

void backend_print_ghost (int x, int y, int w, int c1, int c2, char *string) {
	int org_x = buf_getx ();
	int org_y = buf_gety ();
	int org_c1 = buf_getc1 ();
	int org_c2 = buf_getc2 ();

	buf_setxy (x, y);
	buf_color (c1, c2);

	int l = strlen (string);
	for (int i = 0; i < w; i ++) {
		buf_char (i < l ? string [i] : 32);
	}

	buf_setxy (org_x, org_y);
	buf_color (org_c1, org_c2);
}

void backend_setxy (int x, int y) {
	buf_setxy (x, y);
}

void backend_center (char *string) {
	buf_setx (buf_getscrw () / 2 - strlen (string) / 2);
	buf_print_ln (string);
}

void backend_color (int c1, int c2) {
	buf_color (c1, c2);
}

void backend_indicator_off (void) {
	backend_indicator.on = 0;
}

void backend_indicator_config (int x, int y, int c1, int c2, unsigned char c) {
	backend_indicator.x = x;
	backend_indicator.y = y;
	backend_indicator.c1 = c1;
	backend_indicator.c2 = c2;
	backend_indicator.c = c;

	backend_indicator.on = 1;
}

void backend_pause (void) {
	while (!backend_heartbeat ()) {
		int c = *readchars ();
		if (c == 0) break;
	}

	int ind = 0; int parp = 1;
	keys_read ();
	while (!backend_heartbeat ()) {
		int c = *readchars ();
		if (c > 0) break;

		keys_read ();
		int keys_this_frame = keys_get_this_frame ();
		if (keys_this_frame & MT_KEY_LBUTTON) break;

		if (backend_indicator.on) {
			ind ++; if (ind == 30) {
				ind = 0; parp ^= 1;
			}

			buf_char_abs (backend_indicator.x, backend_indicator.y, backend_indicator.c1, backend_indicator.c2, parp ? backend_indicator.c : 0);
		}
	}

	if (backend_indicator.on) buf_char_abs (backend_indicator.x, backend_indicator.y, backend_indicator.c1, backend_indicator.c2, 0);
}

void backend_beep (void) {

}

void backend_cls (void) {
	buf_clscroll ();
	buf_resetxy ();
}

void backend_draw (char *string) {
	int c1 = buf_getc1 ();
	int c2 = buf_getc2 ();

	int oc1 = c1;
	int oc2 = c2;

	char c; while (c = *string ++) {
		switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				c1 = c - '0';
				buf_color (c1, c2);
				break;

			case '=':
				c2 = 0;
				buf_color (c1, c2);
				break;

			case '!':
			case 34:
			case '$':
			case '%':
			case '&':
				c2 = c - 32;
				buf_color (c1, c2);
				break;

			case 0xFA:
				c2 = 3;
				buf_color (c1, c2);
				break;

			case '/':
				c2 = 7;
				buf_color (c1, c2); 
				break;

			default:
				buf_char (c);
				break;
		}
	}

	buf_color (oc1, oc2);
	buf_print_ln ("");
}

unsigned char backend_choice (int num_choices, int correct, char **choices) {
	int input;

	while (!backend_heartbeat ()) {
		input = backend_read_option (num_choices);
		if (input <= num_choices) backend_print_ln (choices [input + 2]);
		return input == correct;
	}
}

void backend_try_again (char *string_try_again, int attempts) {
	char str [256];
	sprintf (str, string_try_again, attempts);
	buf_print_ln (str);
}

unsigned char backend_read_option (int num_choices) {
	int res = 0;
	int len = 0;
	int x = buf_getx ();

	char input [INPUT_LENGTH + 1];
	memset (input, 0, INPUT_LENGTH + 1);

	debuff_keys ();

	while (1) {
		int c = *readchars ();
		while (!backend_heartbeat () && c != '\r' && len + 1 < INPUT_LENGTH) {
			buf_setx (x);
			buf_print (">");
			buf_print (input);
			buf_print ("_ ");

			if (c >= '0' && c <= '9') {
				input [len ++] = c;
			} else {
				switch( c >> 8 ) {
					case 0x08: 
						// Backspace
						if( len > 0 ) {
							--len; 
							input [len] = 0;
						}
						break;
				}
			}

			c = *readchars ();
			if (c == '\0') {
				if (*readkeys () == KEY_BACK) {
					c = 0x08 << 8;
				}
			}
		}

		int input_i = atoi (input);

		if (input_i == 99) {
			forced_break = 1;
			return 99;
		}

		if (num_choices == 0 || (input_i >= 1 && input_i <= num_choices)) {
			buf_setx (x);
			buf_print (">");
			buf_print (input);
			buf_print_ln (" ");

			return input_i;
		} else {
			buf_setx (x);
			buf_print (">");
			for (int i = 0; i < strlen (input) + 1; i ++) {
				buf_print (" ");
			}
		}
	}
	
}

int backend_heartbeat (void) {
	waitvbl ();

	return shuttingdown ();
}

int backend_shuttingdown (void) {
	return shuttingdown ();
}

void backend_gif_at (char *pathspec, char *gif, int x, int y, int load_pal, int mask) {
	char *fullpath = compute_full_path (pathspec, gif);

	if (buf_getmode () != LS_MODE_TEXT) {
		buf_gif_at (fullpath, x, y, load_pal, mask);
	}

	free (fullpath);
}

void backend_bulma_lin (char *pathspec, char *lin) {
	char *fullpath = compute_full_path (pathspec, lin);

	if (buf_getmode () != LS_MODE_TEXT) {
		buf_bulma_lin (fullpath);
	}

	free (fullpath);
}

void backend_bulma_pix (char *pathspec, char *pix, int dbl, int load_pal, int fancy) {
	char *fullpath = compute_full_path (pathspec, pix);

	if (buf_getmode () != LS_MODE_TEXT) {
		buf_bulma_pix (fullpath, dbl, load_pal, fancy);
	}

	free (fullpath);
}

void backend_fnt (char *pathspec, char *fnt) {
	char *fullpath = compute_full_path (pathspec, fnt);

	if (buf_getmode () != LS_MODE_TEXT) {
		// TODO: Call the right version depending on video mode!
		buf_load_font_16 (fullpath);
	}

	free (fullpath);	
}

void backend_wait_frames (int frames) {
	while (!backend_heartbeat () && frames --) {
		int c = *readchars ();
		if (c > 0) break;
	}
}

void backend_set_margins (int col1, int col2) {
	buf_setmargins (col1, col2);
}

void backend_wordwrap (char *s, int char_by_char) {
	if (char_by_char) {
		buf_wordwrap_char_by_char (s);
	} else {
		buf_wordwrap (s);
	}
}

void backend_set_mode (char *mode) {
	if (strcmp (mode, "text") == 0) {
		buf_setmode (LS_MODE_TEXT);
	} else if (strcmp (mode, "gfx") == 0) {
		buf_setmode (LS_MODE_GFX);
	} else if (strcmp (mode, "gfx_sq") == 0) {
		buf_setmode (LS_MODE_GFX_SQ);
	} else if (strcmp (mode, "gfx_med") == 0) {
		buf_setmode (LS_MODE_GFX_MED);
	} else if (strcmp (mode, "gfx_hi") == 0) {
		buf_setmode (LS_MODE_GFX_HIRES);
	} else if (strcmp (mode, "gfx_dbl") == 0) {
		buf_setmode (LS_MODE_GFX_DBL);
	}
}

void backend_ansibin (char *pathspec, char *filename) {
	char *fullpath = compute_full_path (pathspec, filename);

	buf_textmode_pic (fullpath);

	free (fullpath);
}

void backend_menu_config (int x, int y, int w, int c1, int c2, int fixed, int noframe) {
	backend_menu.x = x;
	backend_menu.y = y;
	backend_menu.w = w;
	backend_menu.c1 = c1;
	backend_menu.c2 = c2;
	backend_menu.fixed = fixed;
	backend_menu.noframe = noframe;
}

void backend_talk_config (int x, int y, int w, int c1, int c2, char *ovl, int ox, int oy, int om) {
	char pal_dummy [768];
	int p_dummy;

	backend_talk.x = x;
	backend_talk.y = y;
	backend_talk.w = w;
	backend_talk.c1 = c1;
	backend_talk.c2 = c2;

	if (ovl) {
		// Just in case
		if (backend_talk.ovl_pixels) {
			free (backend_talk.ovl_pixels);
		}

		backend_talk.ovl_pixels = loadgif (
			ovl, 
			&backend_talk.ow,
			&backend_talk.oh,
			&p_dummy,
			pal_dummy
		);

		backend_talk.ox = ox;
		backend_talk.oy = oy;
		backend_talk.om = om;
		backend_talk.ovl_on = 1;
	} else {
		backend_talk.ovl_on = 0;
	}
}

void backend_resp_config (int y, int c1, int c2) {
	backend_resp.y = y;
	backend_resp.c1 = c1;
	backend_resp.c2 = c2;
}

void backend_talk_do (char *who) {
	if (backend_talk.ovl_on && backend_talk.ovl_pixels) {
		maskblit (
			backend_talk.ox, backend_talk.oy,
			backend_talk.ovl_pixels,
			backend_talk.ow, backend_talk.oh,
			0, 0,
			backend_talk.ow, backend_talk.oh,
			backend_talk.om
		);
	}

	backend_print_ghost (backend_talk.x, backend_talk.y, backend_talk.w, backend_talk.c1, backend_talk.c2, who);
}

void backend_menu_set_selected (int selected) {
	backend_menu_selected = selected;
}

void backend_menu_display (int x, int y, unsigned char* (*get_option) (int), int backend_menu_options) {
	int c1, c2, w;

	for (int i = 0; i < backend_menu_options; i ++) {
		
		if (i == backend_menu_selected) {
			c1 = backend_menu.c2; c2 = backend_menu.c1;
		} else {
			c1 = backend_menu.c1; c2 = backend_menu.c2;
		}
		
		unsigned char *option = get_option (i);
		int l = strlen (option);
		w = backend_menu.noframe ? backend_menu.w : backend_menu.w - 2;
		for (int j = 0; j < w; j ++) {
			buf_char_abs (
				x + (backend_menu.noframe == 0) + j, 
				y + (backend_menu.noframe == 0) + i, 
				c1, c2, 
				j < l ? option [j] : ' '
			);
		}
	}
}

int backend_menu_keys (
	int *backend_menu_selected, int *done, int backend_menu_options, 
	int x0, int y0, int w
) {
	int res = 0;

	keys_read ();
	int pad_this_frame = keys_get_this_frame ();

	if (pad_this_frame & MT_KEY_UP) {
		if (*backend_menu_selected > 0) *backend_menu_selected = *backend_menu_selected - 1;
	}

	if (pad_this_frame & MT_KEY_DOWN) {
		if (*backend_menu_selected < backend_menu_options - 1) *backend_menu_selected  = *backend_menu_selected + 1;
	}

	if ((pad_this_frame & MT_KEY_ENTER) || (pad_this_frame & MT_KEY_LBUTTON)) {
		*done = 1;
	}

	if (pad_this_frame & MT_KEY_ESC) {
		*done = 1;
		*backend_menu_selected = -1;
		res = 999999;
	}

	int x = buf_to_char_coords_x (buf_get_mouse_x ()) - x0;
	int y = buf_to_char_coords_y (buf_get_mouse_y ()) - y0 - (backend_menu.noframe == 0);

	if (x >= 0 && x <= w) {
		if (y >= 0 && y <= backend_menu_options) {
			*backend_menu_selected = y;
		}
	}

	return res;
}

void backend_menu_show (void) {
	int prev_x = buf_getx ();
	int prev_y = buf_gety (); 
	int prev_c1 = buf_getc1 ();
	int prev_c2 = buf_getc2 ();
	int backend_menu_options = menu_get_options ();
	if (backend_menu_selected >= backend_menu_options) backend_menu_selected = backend_menu_options - 1;
	if (!backend_menu.noframe) {
		buf_color (backend_menu.c1, backend_menu.c2);
		buf_box (backend_menu.x, backend_menu.y, backend_menu.x + backend_menu.w - 1, backend_menu.y + 1 + backend_menu_options);
	}
	backend_menu_display (backend_menu.x, backend_menu.y, menu_get_option_text, backend_menu_options);
	buf_setxy (prev_x, prev_y);
	buf_color (prev_c1, prev_c2);	
}

void backend_menu_items_config (char *item) {
	if (item == NULL || strlen (item) == 0) {
		menu_items_auto = 0;
	} else {
		strcpy (menu_items, item);
		menu_items_auto = 1;
	}
}

int backend_menu_run (void) {
	// Auto add "items"
	if (menu_items_auto) {
		if (inventory_get_options () && !menu_has_item (menu_items)) {
			menu_add_item (menu_items, MENU_ITEM_TYPE_ITEMS, NULL);
		}
	}

	int prev_x = buf_getx ();
	int prev_y = buf_gety (); 
	int prev_c1 = buf_getc1 ();
	int prev_c2 = buf_getc2 ();
	
	if (!backend_menu.fixed) buf_sve ();

	int done = 0;
	int backend_menu_options = menu_get_options ();
	if (backend_menu_selected >= backend_menu_options) backend_menu_selected = backend_menu_options - 1;

	// Paint box
	if (!backend_menu.noframe) {
		buf_color (backend_menu.c1, backend_menu.c2);
		buf_box (backend_menu.x, backend_menu.y, backend_menu.x + backend_menu.w - 1, backend_menu.y + 1 + backend_menu_options);
	}

	keys_read ();
	while (!done && !buf_heartbeat ()) {
		// Paint options (w/selected)
		backend_menu_display (backend_menu.x, backend_menu.y, menu_get_option_text, backend_menu_options);

		// Read keys
		if (backend_menu_keys (
			&backend_menu_selected, &done, backend_menu_options,
			backend_menu.x, backend_menu.y, backend_menu.w
		) == 999999) return 999999;
	}

	debuff_keys ();

	if (!backend_menu.fixed) buf_rec ();
	
	buf_setxy (prev_x, prev_y);
	buf_color (prev_c1, prev_c2);

	return backend_menu_selected;
}

int _backend_inventory_run (int x, int y) {
	int prev_x = buf_getx ();
	int prev_y = buf_gety (); 
	int prev_c1 = buf_getc1 ();
	int prev_c2 = buf_getc2 ();
	buf_sve ();

	int done = 0;
	int backend_inventory_items = inventory_get_items ();
	if (backend_menu_selected >= backend_inventory_items) backend_menu_selected = backend_inventory_items - 1;

	// Fix coords
	x = utils_adjust_coords (x, 0, buf_get_scrwidth () - backend_menu.w);
	y = utils_adjust_coords (y, 0, buf_get_scrheight () - backend_inventory_items - 2);

	// Paint box
	if (!backend_menu.noframe) {
		buf_color (backend_menu.c1, backend_menu.c2);
		buf_box (x, y, x + backend_menu.w - 1, y + 1 + backend_inventory_items);
	}

	keys_read ();
	while (!done && !buf_heartbeat ()) {
		// Paint options (w/selected)
		backend_menu_display (x, y, inventory_get_item, backend_inventory_items);

		// Read keys
		if (backend_menu_keys (&backend_menu_selected, &done, backend_inventory_items,
			x, y, backend_menu.w
		) == 999999) return 999999;
	}

	debuff_keys ();

	buf_rec ();
	buf_setxy (prev_x, prev_y);
	buf_color (prev_c1, prev_c2);

	return backend_menu_selected;
}

int backend_inventory_run (void) {
	return _backend_inventory_run (backend_menu.x, backend_menu.y);
}

int backend_inventory_run_xy (int x, int y) {
	return _backend_inventory_run (x, y);
}

int backend_exits_run (void) {
	int prev_x = buf_getx ();
	int prev_y = buf_gety (); 
	int prev_c1 = buf_getc1 ();
	int prev_c2 = buf_getc2 ();
	buf_sve ();

	int done = 0;
	int backend_exits_items = exits_get_options ();
	if (backend_menu_selected >= backend_exits_items) backend_menu_selected = backend_exits_items - 1;

	// Paint box
	if (!backend_menu.noframe) {
		buf_color (backend_menu.c1, backend_menu.c2);
		buf_box (backend_menu.x, backend_menu.y, backend_menu.x + backend_menu.w - 1, backend_menu.y + 1 + backend_exits_items);
	}

	keys_read ();
	while (!done && !buf_heartbeat ()) {
		// Paint options (w/selected)
		backend_menu_display (backend_menu.x, backend_menu.y, exits_get_option_text, backend_exits_items);

		// Read keys
		if (backend_menu_keys (&backend_menu_selected, &done, backend_exits_items,
			backend_menu.x, backend_menu.y, backend_menu.w
		) == 999999) return 999999;
	}

	debuff_keys ();

	buf_rec ();
	buf_setxy (prev_x, prev_y);
	buf_color (prev_c1, prev_c2);

	return backend_menu_selected;
}

int backend_actions_run (int x, int y) {
	int prev_x = buf_getx ();
	int prev_y = buf_gety (); 
	int prev_c1 = buf_getc1 ();
	int prev_c2 = buf_getc2 ();
	buf_sve ();

	int done = 0;
	int backend_actions_items = actions_get_actions ();
	if (backend_menu_selected >= backend_actions_items) backend_menu_selected = backend_actions_items - 1;

	// Fix coords
	x = utils_adjust_coords (x, 0, buf_get_scrwidth () - backend_menu.w);
	y = utils_adjust_coords (y, 0, buf_get_scrheight () - backend_actions_items - 2);

	// Paint box
	if (!backend_menu.noframe) {
		buf_color (backend_menu.c1, backend_menu.c2);
		buf_box (x, y, x + backend_menu.w - 1, y + 1 + backend_actions_items);
	}

	keys_read ();
	while (!done && !buf_heartbeat ()) {
		// Paint options (w/selected)
		backend_menu_display (x, y, actions_get_action, backend_actions_items);

		// Read keys
		if (backend_menu_keys (&backend_menu_selected, &done, backend_actions_items,
			x, y, backend_menu.w
		) == 999999) return 999999;
	}

	debuff_keys ();

	buf_rec ();
	buf_setxy (prev_x, prev_y);
	buf_color (prev_c1, prev_c2);

	return backend_menu_selected;
}

int backend_resp_run (void) {
	// Adjust real y so it fits
	int h = buf_get_scrheight ();
	int w = buf_get_scrwidth () - 2;
	int backend_resp_items = resp_get_resps ();
	int y = backend_resp.y; if (y > h - 1 - backend_resp_items - 2) y = h - 1 - backend_resp_items - 2;

	int prev_x = buf_getx ();
	int prev_y = buf_gety (); 
	int prev_c1 = buf_getc1 ();
	int prev_c2 = buf_getc2 ();
	buf_sve ();

	int done = 0;
	int selected = 0;

	// Paint box
	buf_color (backend_resp.c1, backend_resp.c2);
	buf_box (1, y, 1 + w - 1, y + 1 + backend_resp_items);

	keys_read ();
	while (!done && !buf_heartbeat ()) {
		// Paint options (w/selected)
		for (int i = 0; i < backend_resp_items; i ++) {
			buf_setxy (1 + 1, y + 1 + i);
			if (i == selected) {
				buf_color (backend_resp.c2, backend_resp.c1);
			} else {
				buf_color (backend_resp.c1, backend_resp.c2);
			}
			
			unsigned char *option = resp_get_text (i);
			int l = strlen (option);
			for (int j = 0; j < w - 2; j ++) {
				buf_char (j < l ? option [j] : ' ');
			}
		}

		// Read keys
		if (backend_menu_keys (&selected, &done, backend_resp_items,
			1, y, w
		) == 999999) return 999999;
	}

	debuff_keys ();

	buf_rec ();
	buf_setxy (prev_x, prev_y);
	buf_color (prev_c1, prev_c2);

	return selected;		
}

int backend_show_info_bar (int zone) {
	unsigned char zone_text [ZONE_TEXT_MAX_LENGTH + 1];

	if (zone >= 0) {
		strcpy (zone_text, zones_get_text (zone));
	} else {
		zone_text [0] = 0;
	}
// 
	buf_setxy (0, backend_info_bar.y);
	buf_color (backend_info_bar.c1, backend_info_bar.c2);
	buf_char (' ');

	int w = buf_get_scrwidth (); 
	for (int i = 0; i < w - 1; i ++) {
		buf_char (
			i > 0 && 
			i <= strlen (zone_text) && 
			i < w - 1 
			? 
				zone_text [i - 1] 
			: 
				32
		);			
	}
}

int backend_zones_last_x (void) {
	return buf_to_char_coords_x (backend_zones.last_x);
}

int backend_zones_last_y (void) {
	return buf_to_char_coords_y (backend_zones.last_y);
}

int backend_zones_run (void) {
	// Read mouse, show info, register zone click
	int done = 0;
	int zone;

	do {
		keys_read ();
	} while (keys_get ());

	while (!done && !buf_heartbeat ()) {
		// Get mouse pos
		int mouse_x = buf_get_mouse_x ();
		int mouse_y = buf_get_mouse_y ();
		
		// Get zone
		zone = zones_find (mouse_x, mouse_y);

		// Show
		if (backend_info_bar.y >= 0) {
			backend_show_info_bar (zone);
		}

		// Click
		keys_read ();
		int pad_this_frame = keys_get_this_frame ();
		
		if (pad_this_frame & MT_KEY_LBUTTON) {
			backend_zones.last_x = mouse_x;
			backend_zones.last_y = mouse_y;
			done = 1;
		}

		if (pad_this_frame & MT_KEY_ESC) {
			zone = 999999;
			done = 1;
		}
	}
	
	return zone;
}

void backend_fancy_font (char *f) {
	utils_tolower (f);
	if (strcmp (f, "cga") == 0) {
		buf_font_cga ();
	} else if (strcmp (f, "atari") == 0) {
		buf_font_atari ();
	}
}

void backend_set_info_bar (int y, int c1, int c2) {
	backend_info_bar.y = y;
	backend_info_bar.c1 = c1;
	backend_info_bar.c2 = c2;
}

int backend_menu_get_w (void) {
	return backend_menu.w;
}

void backend_tb_config (int x, int y, int w, int h, int c1, int c2, int tc1, int tc2, int f) {
	backend_tb_cfg.x = x; backend_tb_cfg.y = y; backend_tb_cfg.w = w; backend_tb_cfg.h = h;
	backend_tb_cfg.c1 = c1; backend_tb_cfg.c2 = c2; backend_tb_cfg.tc1 = tc1; backend_tb_cfg.tc2 = tc2;
	backend_tb_cfg.f = f;
}

void backend_tb (char *text, char *title, int wt, int cbc) {
	if (wt) buf_sve ();

	buf_tb (
		backend_tb_cfg.x, backend_tb_cfg.y, backend_tb_cfg.w, backend_tb_cfg.h,
		backend_tb_cfg.c1, backend_tb_cfg.c2, backend_tb_cfg.tc1, backend_tb_cfg.tc2, backend_tb_cfg.f,
		title, text, cbc
	);

	if (wt) {
		backend_pause ();
		buf_rec ();
	}
}

void backend_buf_char_delay (int d) {
	buf_set_buf_char_delay (d);
}

void backend_shpal (void) {
	int x = 0;
	int y = 0;
	for (int i = 0; i < 256; i ++) {
		setcolor (i);
		bar (x, y, 8, 8);
		x += 8; if (x == 128) {
			x = 0; y += 8;
		}
	}
}

void interface_show (void) {
	if (backend_interface_cfg.on && backend_interface_cfg.interface_pixels) {
		maskblit (
			backend_interface_cfg.x,
			backend_interface_cfg.y,
			backend_interface_cfg.interface_pixels,
			backend_interface_cfg.w,
			backend_interface_cfg.h,
			0, 0,
			backend_interface_cfg.w,
			backend_interface_cfg.h,
			backend_interface_cfg.m
		);
	}
}

void backend_fancy_cls_box (int x1, int y1, int x2, int y2) {
	int w = x2 - x1 + 1;
	int h = y2 - y1 + 1;

	int sqs = w == 640 ? 16 : 8; 	// 16

	int maxy = 1 + (h / sqs) / 2; 		// (400 / 16) / 2 = 12
	int wx = w / sqs; 				// wx = 40
	int hy = h / sqs; 				// hy = 25

	setcolor (0);
	for (int i = 0; i < maxy; i ++) {
		for (int x = i; x < wx - i; x ++) {
			bar (x * sqs, i * sqs, sqs, sqs);
			bar (x * sqs, h - (i + 1) * sqs, sqs, sqs);
		}

		for (int y = i; y < hy - i; y ++) {
			bar (i * sqs, y * sqs, sqs, sqs);
			bar (w - (i + 1) * sqs, y * sqs, sqs, sqs);
		}

		interface_show ();
		waitvbl ();
	}		
}

void backend_fancy_cls (void) {
	backend_fancy_cls_box (0, 0, buf_getscrw () - 1, buf_getscrh () - 1);
}

void backend_midi_load (char *pathspec, char *mid, int loop) {
	char *fullpath = compute_full_path (pathspec, mid);

	if (mus != NULL) free (mus);
	mus = loadmid (fullpath);
	playmusic (mus, loop, 192);

	free (fullpath);
}

void backend_mod_load (char *pathspec, char *mod, int loop) {
	char *fullpath = compute_full_path (pathspec, mod);

	if (mus != NULL) free (mus);
	mus = loadmod (fullpath);
	playmusic (mus, loop, 192);

	free (fullpath);
}

void backend_music_stop (void) {
	if (musicplaying ()) stopmusic ();
}

void backend_wav_load (char *pathspec, char *wav, int loop, int channel) {
	char *fullpath = compute_full_path (pathspec, wav);

	if (sound != NULL) free (sound);
	sound = loadwav (fullpath);
	if (channel == -1) {
		channel = cur_sound_channel;
		cur_sound_channel = (cur_sound_channel + 1) % SOUND_CHANNELS;	
	}
	playsound (channel, sound, loop, 255);

	free (fullpath);
}

void backend_sound_stop (int channel) {
	stopsound (channel);
}

void backend_sve () {
	buf_sve ();
}

void backend_rec () {
	buf_rec ();
}

void backend_bg_config (int x, int y) {
	backend_bg_cfg.x = x;
	backend_bg_cfg.y = y;
}

void backend_bg_do (char *pathspec, char *bg) {
	if (buf_getmode () != LS_MODE_TEXT) {
		char *fullpath = compute_full_path (pathspec, bg);
		buf_gif_at (fullpath, backend_bg_cfg.x, backend_bg_cfg.y, 1, -1);
		free (fullpath);

		interface_show ();
	}
}

void backend_interface_off (void) {
	backend_interface_cfg.on = 0;
}

void backend_interface_config (char *interface, int x, int y, int m) {
	char pal_dummy [768]; // discard
	int p_dummy;

	// Just in case
	if (backend_interface_cfg.interface_pixels) {
		free (backend_interface_cfg.interface_pixels);
	}

	// Read pixels
	backend_interface_cfg.interface_pixels = loadgif (
		interface,
		&backend_interface_cfg.w,
		&backend_interface_cfg.h,
		&p_dummy,
		pal_dummy
	);

	backend_interface_cfg.on = 1;
	backend_interface_cfg.x = x;
	backend_interface_cfg.y = y;
	backend_interface_cfg.m = m;
}

void backend_shutdown (void) {
	free (backend_interface_cfg.interface_pixels);
	free (backend_talk.ovl_pixels);
	lstextmode_shutdown ();
}
