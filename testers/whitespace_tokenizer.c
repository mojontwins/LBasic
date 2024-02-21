#include <stdio.h>
#include "../dev/lstokens.h"

unsigned char *whitespace_string = "    Cadena  con whitespace    al azar   \"Y un texto entrecomillado\"  vale?\r\n";

void main (void) {
	lstokens_init ();

	printf ("%s\n", whitespace_string);
	parse_to_tokens_whitespace (whitespace_string);

	for (int i = 0; i < get_num_tokens (); i ++) {
		printf (get_token (i));
	}
	printf ("\n");

	for (int i = 0; i < get_num_tokens (); i ++) {
		printf ("[%s]\n", get_token (i));
	}
}
