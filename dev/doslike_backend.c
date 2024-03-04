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

int backend_menu_x = 0;
int backend_menu_y = 0;
int backend_menu_c1 = 15;
int backend_menu_c2 = 1;
int backend_menu_w = 16;
int backend_menu_selected = 0;
int backend_menu_fixed = 0;

int backend_talk_x = 0;
int backend_talk_y = 0;
int backend_talk_c1 = 15;
int backend_talk_c2 = 1;
int backend_talk_w = 16;
int backend_talk_selected = 0;

int backend_info_bar_y = -1; 	// -1 means OFF
int backend_info_bar_c1 = 0;
int backend_info_bar_c2 = 0;

int zones_last_x = 0;
int zones_last_y = 0;

int tb_x = 1;
int tb_y = 1;
int tb_w = 78;
int tb_h = 10;
int tb_c1 = 15;
int tb_c2 = 7;
int tb_tc1 = 0;
int tb_tc2 = 7;
int tb_f = 8; 

int resp_y = 25;
int resp_c1 = 15;
int resp_c2 = 0;

struct music_t *mus = NULL;
struct sound_t *sound = NULL;

int cur_sound_channel = 0;

void backend_init (void) {
	lstextmode_init ();
	buf_color (7, 0);
	buf_clscroll ();
	setsoundbank (DEFAULT_SOUNDBANK_SB16);
}

int backend_get_break (void) {
	return forced_break;
}

char *compute_full_path (char *pathspec, char *filename) {
	char *full_path = strdup (pathspec);
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

void backend_pause (void) {
	while (!backend_heartbeat ()) {
		int c = *readchars ();
		if (c == 0) break;
	}

	keys_read ();
	while (!backend_heartbeat ()) {
		int c = *readchars ();
		if (c > 0) break;

		keys_read ();
		int keys_this_frame = keys_get_this_frame ();
		if (keys_this_frame & MT_KEY_LBUTTON) break;
	}
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

void backend_menu_config (int x, int y, int w, int c1, int c2, int fixed) {
	backend_menu_x = x;
	backend_menu_y = y;
	backend_menu_w = w;
	backend_menu_c1 = c1;
	backend_menu_c2 = c2;
	backend_menu_fixed = fixed;
}

void backend_talk_config (int x, int y, int w, int c1, int c2) {
	backend_talk_x = x;
	backend_talk_y = y;
	backend_talk_w = w;
	backend_talk_c1 = c1;
	backend_talk_c2 = c2;
}

void backend_resp_config (int y, int c1, int c2) {
	resp_y = y;
	resp_c1 = c1;
	resp_c2 = c2;
}

void backend_talk (char *who) {
	backend_print_ghost (backend_talk_x, backend_talk_y, backend_talk_w, backend_talk_c1, backend_talk_c2, who);
}

void backend_menu_set_selected (int selected) {
	backend_menu_selected = selected;
}

void backend_menu_display (int x, int y, unsigned char* (*get_option) (int), int backend_menu_options) {
	for (int i = 0; i < backend_menu_options; i ++) {
		buf_setxy (x + 1, y + 1 + i);
		if (i == backend_menu_selected) {
			buf_color (backend_menu_c2, backend_menu_c1);
		} else {
			buf_color (backend_menu_c1, backend_menu_c2);
		}
		
		unsigned char *option = get_option (i);
		int l = strlen (option);
		for (int j = 0; j < backend_menu_w - 2; j ++) {
			buf_char (j < l ? option [j] : ' ');
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
	int y = buf_to_char_coords_y (buf_get_mouse_y ()) - y0 - 1;

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
	buf_color (backend_menu_c1, backend_menu_c2);
	buf_box (backend_menu_x, backend_menu_y, backend_menu_x + backend_menu_w - 1, backend_menu_y + 1 + backend_menu_options);
	backend_menu_display (backend_menu_x, backend_menu_y, menu_get_option_text, backend_menu_options);
}

int backend_menu_run (void) {
	int prev_x = buf_getx ();
	int prev_y = buf_gety (); 
	int prev_c1 = buf_getc1 ();
	int prev_c2 = buf_getc2 ();
	
	if (!backend_menu_fixed) buf_sve ();

	int done = 0;
	int backend_menu_options = menu_get_options ();
	if (backend_menu_selected >= backend_menu_options) backend_menu_selected = backend_menu_options - 1;

	// Paint box
	buf_color (backend_menu_c1, backend_menu_c2);
	buf_box (backend_menu_x, backend_menu_y, backend_menu_x + backend_menu_w - 1, backend_menu_y + 1 + backend_menu_options);

	keys_read ();
	while (!done && !buf_heartbeat ()) {
		// Paint options (w/selected)
		backend_menu_display (backend_menu_x, backend_menu_y, menu_get_option_text, backend_menu_options);

		// Read keys
		if (backend_menu_keys (
			&backend_menu_selected, &done, backend_menu_options,
			backend_menu_x, backend_menu_y, backend_menu_w
		) == 999999) return 999999;
	}

	debuff_keys ();

	if (!backend_menu_fixed) buf_rec ();
	
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
	x = utils_adjust_coords (x, 0, buf_get_scrwidth () - backend_menu_w);
	y = utils_adjust_coords (y, 0, buf_get_scrheight () - backend_inventory_items - 2);

	// Paint box
	buf_color (backend_menu_c1, backend_menu_c2);
	buf_box (x, y, x + backend_menu_w - 1, y + 1 + backend_inventory_items);

	keys_read ();
	while (!done && !buf_heartbeat ()) {
		// Paint options (w/selected)
		backend_menu_display (x, y, inventory_get_item, backend_inventory_items);

		// Read keys
		if (backend_menu_keys (&backend_menu_selected, &done, backend_inventory_items,
			x, y, backend_menu_w
		) == 999999) return 999999;
	}

	debuff_keys ();

	buf_rec ();
	buf_setxy (prev_x, prev_y);
	buf_color (prev_c1, prev_c2);

	return backend_menu_selected;
}

int backend_inventory_run (void) {
	return _backend_inventory_run (backend_menu_x, backend_menu_y);
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
	buf_color (backend_menu_c1, backend_menu_c2);
	buf_box (backend_menu_x, backend_menu_y, backend_menu_x + backend_menu_w - 1, backend_menu_y + 1 + backend_exits_items);

	keys_read ();
	while (!done && !buf_heartbeat ()) {
		// Paint options (w/selected)
		backend_menu_display (backend_menu_x, backend_menu_y, exits_get_option_text, backend_exits_items);

		// Read keys
		if (backend_menu_keys (&backend_menu_selected, &done, backend_exits_items,
			backend_menu_x, backend_menu_y, backend_menu_w
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
	x = utils_adjust_coords (x, 0, buf_get_scrwidth () - backend_menu_w);
	y = utils_adjust_coords (y, 0, buf_get_scrheight () - backend_actions_items - 2);

	// Paint box
	buf_color (backend_menu_c1, backend_menu_c2);
	buf_box (x, y, x + backend_menu_w - 1, y + 1 + backend_actions_items);

	keys_read ();
	while (!done && !buf_heartbeat ()) {
		// Paint options (w/selected)
		backend_menu_display (x, y, actions_get_action, backend_actions_items);

		// Read keys
		if (backend_menu_keys (&backend_menu_selected, &done, backend_actions_items,
			x, y, backend_menu_w
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
	int y = resp_y; if (y > h - 1 - backend_resp_items - 2) y = h - 1 - backend_resp_items - 2;

	int prev_x = buf_getx ();
	int prev_y = buf_gety (); 
	int prev_c1 = buf_getc1 ();
	int prev_c2 = buf_getc2 ();
	buf_sve ();

	int done = 0;
	int selected = 0;

	// Paint box
	buf_color (resp_c1, resp_c2);
	buf_box (1, y, 1 + w - 1, y + 1 + backend_resp_items);

	keys_read ();
	while (!done && !buf_heartbeat ()) {
		// Paint options (w/selected)
		for (int i = 0; i < backend_resp_items; i ++) {
			buf_setxy (1 + 1, y + 1 + i);
			if (i == selected) {
				buf_color (resp_c2, resp_c1);
			} else {
				buf_color (resp_c1, resp_c2);
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

	buf_setxy (0, backend_info_bar_y);
	buf_color (backend_info_bar_c1, backend_info_bar_c2);
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
	return buf_to_char_coords_x (zones_last_x);
}

int backend_zones_last_y (void) {
	return buf_to_char_coords_y (zones_last_y);
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
		if (backend_info_bar_y >= 0) {
			backend_show_info_bar (zone);
		}

		// Click
		keys_read ();
		int pad_this_frame = keys_get_this_frame ();
		
		if (pad_this_frame & MT_KEY_LBUTTON) {
			zones_last_x = mouse_x;
			zones_last_y = mouse_y;
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
	backend_info_bar_y = y;
	backend_info_bar_c1 = c1;
	backend_info_bar_c2 = c2;
}

int backend_menu_get_w (void) {
	return backend_menu_w;
}

void backend_tb_config (int x, int y, int w, int h, int c1, int c2, int tc1, int tc2, int f) {
	tb_x = x; tb_y = y; tb_w = w; tb_h = h;
	tb_c1 = c1; tb_c2 = c2; tb_tc1 = tc1; tb_tc2 = tc2;
	tb_f = f;
}

void backend_tb (char *text, char *title, int wt, int cbc) {
	if (wt) buf_sve ();

	buf_tb (
		tb_x, tb_y, tb_w, tb_h,
		tb_c1, tb_c2, tb_tc1, tb_tc2, tb_f,
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

void backend_shutdown (void) {
	lstextmode_shutdown ();
}
