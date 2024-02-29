// Backend prototypes. implement them as fit

unsigned char backend_choice (int num_choices, int correct, char **choices);
unsigned char backend_read_option (int num_choices);
void backend_ansibin (char *pathspec, char *filename);
void backend_beep (void);
void backend_center (char *string);
void backend_cls (void);
void backend_color (int c1, int c2);
void backend_draw (char *string);
int backend_get_break (void);
void backend_gif_at (char *pathspec, char *gif, int x, int y, int load_pal);
void backend_bulma_lin (char *pathspec, char *lin);
void backend_bulma_pix (char *pathspec, char *lin, int dbl, int load_pal);
void backend_fnt (char *pathspec, char *fnt);
int backend_heartbeat (void);
void backend_init (void);
void backend_menu_config (int x, int y, int w, int c1, int c2);
int backend_menu_get_w (void);
int backend_menu_run (void);
int backend_inventory_run (void);
int backend_exits_run (void);
int backend_actions_run (int x, int y);
int backend_inventory_run_xy (int x, int y);
int backend_zones_run (void);
void backend_menu_set_selected (int selected);
void backend_pause (void);
void backend_setxy (int x, int y);
void backend_print (char *string);
void backend_print_ln (char *string);
void backend_set_margins (int col1, int col2);
void backend_set_mode (char *mode);
void backend_set_show_status (int i);
void backend_set_viewport (int l1, int l2);
void backend_shutdown (void); 
void backend_statusbar (int clr_statusbar1, int clr_statusbar2, char *string_top, char *string_bottom, int attempts);
void backend_try_again (char *string_try_again, int attempts);
void backend_wait_frames (int frames);
void backend_wordwrap (char *s, int char_by_char);
void backend_print_ghost (int x, int y, int w, int c1, int c2, char *string);
void backend_talk_config (int x, int y, int w, int c1, int c2);
void backend_talk (char *s);
void backend_fancy_font (char *f);
int backend_zones_last_x (void);
int backend_zones_last_y (void);
void backend_set_info_bar (int y, int c1, int c2);
void backend_tb_config (int x, int y, int w, int h, int c1, int c2, int tc1, int tc2, int f);
void backend_tb (char *text, char *title, int wt, int cbc);
void backend_buf_char_delay (int d);
void backend_shpal (void);
void backend_fancy_cls (void);
