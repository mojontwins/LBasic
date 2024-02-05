// Backend prototypes. implement them as fit

void backend_init (void);
void backend_set_show_status (int i);
void backend_statusbar (int clr_statusbar1, int clr_statusbar2, char *string_top, char *string_bottom, int attempts);
void backend_print_ln (char *string);
void backend_print (char *string);
void backend_center (char *string);
void backend_color (int c1, int c2);
void backend_pause (void);
void backend_beep (void);
void backend_cls (void);
void backend_draw (char *string);
unsigned char backend_choice (int num_choices, int correct, char **choices);
void backend_try_again (char *string_try_again, int attempts);
unsigned char backend_read_option (int num_choices);
int backend_heartbeat (void);
void backend_gif_at (char *gif, int x, int y, int load_pal);
void backend_wait_frames (int frames);
void backend_set_mode (char *mode);
void backend_shutdown (void); 