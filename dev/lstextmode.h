#define LS_MODE_TEXT 		0
#define LS_MODE_GFX  		1
#define LS_MODE_GFX_SQ		2
#define LS_MODE_GFX_HIRES	3
#define LS_MODE_GFX_MED		4

void buf_setviewport (int y1, int y2);
void buf_setmargins (int col1, int col2);
void buf_setxy (int x, int y);
void buf_setx (int x);
void buf_sety (int y);
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
void buf_print (char *s);
void buf_print_ln (char *s);
void buf_wordwrap (char *s);
void buf_cls (void);
void buf_clscroll (void);
void buf_put_string_xy (int x, int y, int c1, int c2, char *s);
void lstextmode_init (void);
void buf_setmode (int mode);
int buf_getmode (void);
void buf_pause (void);
void buf_gif_at (char *gif, int x, int y, int do_setpal);
void buf_textmode_pic (char *pic);
