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

unsigned char get_character_input (unsigned char const* chars) {	
	char c;
	while (c = *chars ++) {
		// Translate to CP 437
		if (c < 0) return iso_ext_to_cp437 [128 + c];

		return c;
	}
}
