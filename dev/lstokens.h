#define MAX_TOKENS 32
#define TOKEN_MAX_LENGTH 512

void lstokens_init (void);
void lstokens_free (void);
void parse_to_tokens (char *string);
int get_num_tokens (void);
char *get_token (int i);
char **get_tokens_from (int i);
char **get_tokens (void);
