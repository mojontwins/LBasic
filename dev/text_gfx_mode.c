#include "../dos-like/source/dos.h"
#include "lstextmode.h"

void main(void) {
	buf_setmode (LS_MODE_GFX);
	buf_color (4, 2);
	buf_cls ();
	buf_color (12, 10);
	buf_clscroll ();
	buf_setxy (10, 10);
	buf_color (14, 1);
	buf_printabs ("HOLA");
	buf_pause ();
}