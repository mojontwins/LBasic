#include <stdio.h>
#include "../dev/lstokens.h"

char tokenize_this[] = "Esto es una cadena para tokenizar a ver si guarda bien los índices";

void main (void) {
	lstokens_init ();
	printf ("%s\n", tokenize_this);

	parse_to_tokens (tokenize_this);
	int n_tokens = get_num_tokens ();

	for (int i = 0; i < n_tokens; i ++) {
		printf ("%d: %s\n", get_index(i), get_token (i));
	}

	lstokens_free ();
}