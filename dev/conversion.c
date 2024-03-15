#include "conversion.h"

// This table stores equivalents in cp 437 to all characters in ISO for 128->255
unsigned char iso_ext_to_cp437 [] = {
	// Not used x 16
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	// Not used x 16
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//	NBSP  ¡     ¢     £     ¤     ¥     ¦     §     ¨     ©     ª     «     ¬     SHY   ®     ¯
	0x20, 0xAD, 0x9B, 0x9C, 0xFE, 0x9D, 0xB3, 0x9E, 0xF8, 0xE9, 0xA6, 0xAE, 0xAA, 0x00, 0xE9, 0xC4,
//	°     ±     ²     ³     ´     µ     ¶     ·     ¸     ¹     º     »     ¼     ½     ¾     ¿
	0xF8, 0xF1, 0xFD, 0x20, 0x60, 0xE6, 0xB7, 0xFA, 0x2C, 0xD9, 0xA7, 0xAF, 0xAC, 0xAB, 0xED, 0xA8,
//	À     Á     Â     Ã     Ä     Å     Æ     Ç     È     É     Ê     Ë     Ì     Í     Î     Ï
	0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x92, 0x80, 0x90, 0x90, 0x90, 0x90, 0x8D, 0xA1, 0x8C, 0x8B,
//	Ð     Ñ     Ò     Ó     Ô     Õ     Ö     ×     Ø     Ù     Ú     Û     Ü     Ý     Þ     ß
	0xEE, 0xA5, 0x95, 0xA2, 0x93, 0x93, 0x94, 0xf9, 0xED, 0x9A, 0x9A, 0x9A, 0x9A, 0x98, 0xE0, 0xE1, 
//	à     á     â     ã     ä     å     æ     ç     è     é     ê     ë     ì     í     î     ï
	0x85, 0xA0, 0x83, 0x83, 0x84, 0x86, 0x91, 0x87, 0x8a, 0x82, 0x88, 0x89, 0x8d, 0xa1, 0x8c, 0x8b,
//	ð     ñ     ò     ó     ô     õ     ö     ÷     ø     ù     ú     û     ü     ý     þ     ÿ
	0xEB, 0xA4, 0x95, 0xA2, 0x93, 0x93, 0x94, 0xfa, 0xED, 0x97, 0xA3, 0x96, 0x81, 0x98, 0xc6, 0x98
};

unsigned char translate (char c) {
	if (c < 0) return iso_ext_to_cp437 [128 + c];
	return c;
}

unsigned char get_character_input (unsigned char const* chars) {	
	char c;
	while (c = *chars ++) {
		// Translate to CP 437
		if (c < 0) return iso_ext_to_cp437 [128 + c];

		return c;
	}
}

unsigned char key_to_draw_char (unsigned char c) {
	switch (c) {
		case 'q': return '\xDA';
		case 'w': return '\xC4';
		case 'e': return '\xBF';
		case 'r': return '\xC9';
		case 't': return '\xCD';
		case 'y': return '\xBB';
		case 'u': return '\xB0';
		case 'i': return '\xB1';
		case 'o': return '\xB2';
		case 'p': return '\xDB';
		case 'a': return '\xB3';
		case 's': return '\xBA';
		case 'd': return '\xC5';
		case 'f': return '\xCC';
		case 'g': return '\xB9';
		case 'h': return '\xCA';
		case 'j': return '\xCB';
		case 'k': return '\xCE';
		case 'l': return '\xDC';
		case '\xA4': return '\xDF';
		case 'z': return '\xC0';
		case 'x': return '\xD9';
		case 'c': return '\xC8';
		case 'v': return '\xBC';
		case 'V': return '\xC3';
		case 'b': return '\xB4';
		case 'n': return '\xC1';
		case 'm': return '\xC2';		
	}

	return c;
}

int key_to_color_1 (unsigned char c) {
	if (c >= '0' && c <= '7') return c - '0';
	return -1;
}

int key_to_color_2 (unsigned char c) {
	switch (c) {
		case '=': return 0;
		case '!': return 1;
		case '\"': return 2;
		case '\xFA': return 3;
		case '$': return 4;
		case '%': return 5;
		case '&': return 6;
		case '/': return 7;
	}
	return -1;
}
