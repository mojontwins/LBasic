//#define DEBUG

#include <stdio.h>

#include "lbasi.h"
#include "backend.h"

void backend_print (char *string) {
	printf ("%s\n", string);
}

void backend_center (char *string) {
	#ifdef DEBUG
		printf ("[%s]\n", string);
	#else
		backend_print (string);
	#endif
}

void backend_color (int c1, int c2) {
	#ifdef DEBUG
		printf ("COLOR %d %d\n", c1, c2);
	#endif
}

void backend_pause (void) {
	#ifdef DEBUG
		printf ("PAUSE\n");
	#endif
}

void backend_beep (void) {
	#ifdef DEBUG
		printf ("DEBUG\n");
	#endif
}

void backend_cls (void) {
	#ifdef DEBUG
		printf ("CLS\n");
	#endif	
}

void backend_draw (char *string) {
	#ifdef DEBUG
		printf ("DRAW\n");
	#endif
	// TODO: Interpret embedded color codes
	backend_print (string);
}

unsigned char backend_read_option (int num_choices) {
	int input; 
	char c; 

	while (1) {
		printf (">");
		if (scanf (" %d", &input) != 1) {
			// Discard buffer
			while ((c = getchar ()) != '\n' && c != EOF);
			if (c == EOF) break;
		}

		if (input >= 1 && input <= num_choices) {
			return input;
		}
	}
}

unsigned char backend_choice (int num_choices, int correct, char **choices) {
	int input;
	char c;
	int input_numeric;

	while (1) {
		#ifdef DEBUG
			printf ("[%d:%d]", num_choices, correct);
		#endif

		input = backend_read_option (num_choices);

		#ifdef DEBUG
			printf ("[%d]", input);
		#endif

		backend_print (choices [input + 2]);
		return input == correct;
	}
}

void backend_try_again (int attempts) {
	printf (get_str_try_again (), attempts);
	printf ("\n");
}
