#include <stdlib.h>
#include "lstokens.h"

char *tokens [MAX_TOKENS];
int cur_token;

int is_initialized = 0;

void lstokens_init (void) {
	if (is_initialized == 0) {
		for(int i = 0; i < MAX_TOKENS; i ++) {
			tokens [i] = malloc (TOKEN_MAX_LENGTH * sizeof (char));
		}

		is_initialized = 1;
	}
}

void lstokens_free (void) {
	if (is_initialized) {
		for(int i = 0; i < MAX_TOKENS; i ++) {
			free (tokens [i]);
		}
	}
}

void parse_to_tokens (char *string) {
	if (is_initialized == 0) {
		lstokens_init ();
	}

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

		if (cur_char == 0 || cur_char == '\r' || cur_char == '\n' || ((cur_char == ' ' || cur_char == '\t') && in_quotes == 0)) {
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

void parse_to_tokens_whitespace (char *string) {
	if (is_initialized == 0) {
		lstokens_init ();
	}

	char *pt = string;
	char cur_char;
	int i;

	cur_token = 0;
	int cur_index = 0;
	int in_quotes = 0;

	int whitespace = 0;

	// Clear first
	for (i = 0; i < MAX_TOKENS; i ++) {
		tokens [i][0] = 0;
	}

	do {
		cur_char = *pt;

		if (whitespace) {
			// We are collecting whitespace
			if (cur_char == '\r' || cur_char == '\n' || cur_char == 0) {
				// ignore
				++ pt;
			} else if (cur_char != ' ' && cur_char != '\t') {
				tokens [cur_token ++][cur_index] = 0;
				cur_index = 0;				
				whitespace = 0;
			} else {
				tokens [cur_token][cur_index ++] = cur_char;
				if (cur_index == TOKEN_MAX_LENGTH) {
					tokens [cur_token ++][cur_index - 1] = 0;
					cur_index = 0;
				}

				++ pt;
			}
		} else {
			// We are collecting words / quoted text
			if ((in_quotes == 0 && (cur_char == 0 || cur_char == '\r' || cur_char == '\n' || ((cur_char == ' ' || cur_char == '\t') && in_quotes == 0))) ||
				(in_quotes == 1 && cur_char == 0)) {
				if (cur_index > 0) {
					tokens [cur_token ++][cur_index] = 0;
					cur_index = 0;
				}
				whitespace = 1;
			} else {
				if (cur_char == 34) in_quotes = !in_quotes;

				tokens [cur_token][cur_index ++] = cur_char;
				if (cur_index == TOKEN_MAX_LENGTH) {
					tokens [cur_token ++][cur_index - 1] = 0;
					cur_index = 0;
				}

				++ pt;
			}
		}

		if (cur_token == MAX_TOKENS) break;
	} while (cur_char != 0);
}

int get_num_tokens (void) {
	return cur_token;
}

char *get_token (int i) {
	if (is_initialized == 0) {
		lstokens_init ();
	}

	return tokens [i];
}

char **get_tokens_from (int i) {
	if (is_initialized == 0) {
		lstokens_init ();
	}

	return tokens + i * sizeof (char *);
}

char **get_tokens (void) {
	if (is_initialized == 0) {
		lstokens_init ();
	}

	return tokens;
}
