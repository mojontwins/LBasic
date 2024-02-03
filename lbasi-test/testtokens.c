// Simple string tokenizer test
// Using fixed size buffers only

#include <stdio.h>

#include "lstokens.h"

int main (void) {
	int i;
	int num_tokens;
	char *test_string = "El amigo de tu pare es más tonto que abundio y dijo \"qué paha\".";

	char **local_tokens;

	lstokens_init ();
	parse_to_tokens(test_string);

	num_tokens = get_num_tokens ();

	for(i = 0; i < num_tokens; i ++) {
		printf ("[%s]\n", get_token (i));
	}

	local_tokens = get_tokens_from(3);
	printf ("[[%s]]\n", local_tokens [4]);

	local_tokens = get_tokens();
	printf ("[[%s]]\n", local_tokens [2]);

	printf ("FIN\n");
	lstokens_free ();
}
