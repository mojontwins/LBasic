#include <stdlib.h>
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
	buf_gif_at ("TBS/tbs1.gif", 0, 0, 1);
	int i; for (i = 0; i < 12; i ++) {
		buf_color (rand () & 15, rand () & 15);
		buf_print_ln("LOREM IPSUM Susmuertum totus enbragum enfilum bailando la lambadum cogidos de la manum...");
		buf_pause ();
	}
	buf_pause ();
}