#define LS_MODE_TEXT 		0
#define LS_MODE_GFX  		1
#define LS_MODE_GFX_SQ		2
#define LS_MODE_GFX_HIRES	3
#define LS_MODE_GFX_MED		4
#define LS_MODE_GFX_DBL		5

void lstextmode_init (void);
void buf_set_buf_char_delay (int d);
void debuff_keys (void);
int buf_get_scrwidth (void);
int buf_get_scrheight (void);
void buf_setviewport (int y1, int y2);
int buf_getviewport_y1 (void);
int buf_getviewport_y2 (void);
void buf_setmargins (int col1, int col2);
void buf_setxy (int x, int y);
void buf_setx (int x);
void buf_sety (int y);
void buf_resetxy (void);
int buf_getx (void);
int buf_gety (void);
int buf_getscrw (void);
int buf_getscrh (void);
void buf_color (int c1, int c2);
int buf_getc1 (void);
int buf_getc2 (void);
int buf_get_attrib (void);
void buf_char (char c);
void buf_scroll_up (int from, int to);
void buf_print_abs (char *s);
void buf_print_abs_clip_to_scroll (char *s);
void buf_print_trim (char *s);
void buf_print (char *s);
void buf_print_char_by_char (char *s);
void buf_print_ln (char *s);
void buf_wordwrap (char *s);
void buf_wordwrap_char_by_char (char *s);
void buf_cls (void);
void buf_clscroll (void);
void buf_put_string_xy (int x, int y, int c1, int c2, char *s);
void buf_setmode (int mode);
int buf_getmode (void);
void buf_pause (void);
void buf_gif_at (char *gif, int x, int y, int do_setpal, int mask);
void buf_bulma_pix (char *pix, int dbl, int do_setpal, int fancy);
void buf_textmode_pic (char *pic);
void buf_box (int x1, int x2, int y1, int y2);
int buf_get_mouse_x (void);
int buf_get_mouse_y (void);
int buf_get_mouse_b (int button);
int buf_get_keystate (int keys);
int buf_heartbeat (void);
void buf_bulma_lin (char *pic);
void buf_sve (void);
void buf_rec (void);
void lstextmode_shutdown (void);
void buf_font_atari (void);
void buf_font_cga (void);
int buf_to_char_coords_x (int x);
int buf_to_char_coords_y (int y);
void buf_tb (int x1, int y1, int w, int h, int c1, int c2, int tc1, int tc2, int f, char *title, char *text, int char_by_char);
void buf_load_font_16 (char *font);
