#include <stdlib.h>
#include "lstokens.h"

char *tokens [MAX_TOKENS];
int cur_token;

void lstokens_init (void) {
	int i;

	for(i = 0; i < MAX_TOKENS; i ++) {
		tokens [i] = malloc (TOKEN_MAX_LENGTH * sizeof (char));
	}
}

void lstokens_free (void) {
	int i;

	for(i = 0; i < MAX_TOKENS; i ++) {
		free (tokens [i]);
	}
}

void parse_to_tokens (char *string) {
	char *pt = string;
	char cur_char;
	int i;

	cur_token = 0;
	int cur_index = 0;
	int in_quotes = 0;

	// Clear first
	for (i = 0; i < MAX_TOKENS; i ++) {
		tokens [i][0] = 0;
	}

	do {
		cur_char = *pt ++;

		if (cur_char == 0 || cur_char == '\r' || cur_char == '\n' || (cur_char == ' ' && in_quotes == 0)) {
			if (cur_index > 0) {
				tokens [cur_token ++][cur_index] = 0;
				cur_index = 0;
			}
		} else if (cur_char == 34) {
			in_quotes = !in_quotes;

			if (in_quotes == 0 && cur_index > 0) {
				tokens [cur_token ++][cur_index] = 0;
				cur_index = 0;
			}
		} else {
			tokens [cur_token][cur_index ++] = cur_char;
			if (cur_index == TOKEN_MAX_LENGTH) {
				tokens [cur_token ++][cur_index - 1] = 0;
				cur_index = 0;
			}
		}

		if (cur_token == MAX_TOKENS) break;
	} while (cur_char != 0);
}

int get_num_tokens (void) {
	return cur_token;
}

char *get_token (int i) {
	return tokens [i];
}

char **get_tokens_from (int i) {
	return tokens + i * sizeof (char *);
}

char **get_tokens (void) {
	return tokens;
}
