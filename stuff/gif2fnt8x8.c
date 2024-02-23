#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../dos-like/source/dos.h"

#include "../dos-like/source/libs/pixelfont.h"


int main(void) {
	int chr_width = 8;
	int chr_height = 8;
	int chr_baseline = 6;

	int w, h, p;
	char palette [256*3];

	char *font_pixel_data = loadgif ("8x8_font_atari.gif", &w, &h, &p, palette);

	PIXELFONT_U8 pixels [32 * 32];

	pixelfont_builder_t* builder = pixelfont_builder_create (
		chr_height, 
		chr_baseline, 
		chr_height,
		NULL
	);

	int x0 = 0;
	int y0 = 0;
	for (int c = 0; c < 256; c ++) {
		// Paint glyph
		for (int y = 0; y < 8; y ++) {
			for (int x = 0; x < 8; x ++) {
				pixels [x + chr_width * y] = font_pixel_data [(y0 + y) * 128 + x0 + x];
			}
		}
		x0 = x0 + 8; if (x0 == 128) {
			x0 = 0; y0 = y0 + 8;
		}

		pixelfont_builder_glyph (builder, c, chr_width, pixels, chr_width, 0, chr_width);
	}

	pixelfont_t* pixelfont = pixelfont_builder_font (builder);
	pixelfont_t* output = (pixelfont_t*) malloc (pixelfont->size_in_bytes);
	memcpy (output, pixelfont, pixelfont->size_in_bytes);

	FILE *pf = fopen ("atari.fnt", "wb");
	fwrite (output, sizeof (unsigned char), pixelfont->size_in_bytes, pf);
	fclose (pf);
	pixelfont_builder_destroy (builder);
}

