#include "../dos-like/source/dos.h"

void main (void) {
	cputs ("I'll ignore X --- you should press ENTER to exit");
	setexitbuttonenable (0);

	while (1) {
		waitvbl ();
		if (keystate (KEY_RETURN)) forceshutdown ();
	}
}
