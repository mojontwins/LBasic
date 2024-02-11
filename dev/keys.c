#include "../dos-like/source/dos.h"

#include "keys.h"

int pad0;
int pad_this_frame;

void keys_read (void) {
	pad_this_frame = pad0;

	pad0 =
		keystate (KEY_RETURN) |
		(keystate (KEY_ESCAPE) << 1) |
		(keystate (KEY_UP) << 2) |
		(keystate (KEY_DOWN) << 3) |
		(keystate (KEY_LEFT) << 4) |
		(keystate (KEY_RIGHT) << 5) |
		(keystate (KEY_S) << 6) |
		(keystate (KEY_N) << 7)
	;

	pad_this_frame = (pad_this_frame ^ pad0) & pad0;
}

int keys_get (void) {
	return pad0;
}

int keys_get_this_frame (void) {
	return pad_this_frame;
}
