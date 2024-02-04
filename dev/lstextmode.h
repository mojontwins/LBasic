#define LS_MODE_TEXT 		0
#define LS_MODE_GFX  		1
#define LS_MODE_GFX_SQ		2
#define LS_MODE_GFX_HIRES	3

void buf_setviewport (int y1, int y2);
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
void buf_scroll_up (int from, int to);
void buf_printabs (char *s);
void buf_print (char *s);
void buf_print_ln (char *s);
void buf_cls (void);
void buf_clscroll (void);
void buf_put_string_xy (int x, int y, int c1, int c2, char *s);
void lstextmode_init (void);
void buf_setmode (unsigned char mode);
void buf_pause (void);
