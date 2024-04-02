#define MAX_TOKENS 32
#define TOKEN_MAX_LENGTH 1024

void lstokens_init (void);
void lstokens_free (void);
void parse_to_tokens (char *string);
void parse_to_tokens_plain (char *string);
void parse_to_tokens_whitespace (char *string);
int get_num_tokens (void);
char *get_token (int i);
int get_index (int i);
char **get_tokens_from (int i);
char **get_tokens (void);
int token_exists (char *token);
