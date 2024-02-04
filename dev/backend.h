// Backend prototypes. implement them as fit

void backend_init (void);
void backend_print (char *string);
void backend_center (char *string);
void backend_color (int c1, int c2);
void backend_pause (void);
void backend_beep (void);
void backend_cls (void);
void backend_draw (char *string);
unsigned char backend_choice (int num_choices, int correct, char **choices);
void backend_try_again (int attempts);
unsigned char backend_read_option (int num_choices);
void backend_shutdown (void); 
