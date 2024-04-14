// LBasic Interpreter 
// Hybrid LBasic 6.03-8.5 + extensions

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef DOSLIKE
#include "../dos-like/source/dos.h"
#endif

#include "lstokens.h"
#include "interpreter.h"

#define DEBUGLOG_IMPLEMENTATION
#include "debuglog.h"

void main (char argc, char *argv []) {
	int res;
	int autoboot;
	char *spec;

	// This is extremely dangerous, as I'm overriding the [X] button
	// to close the window. I want to trap that event from then interpreter
	// so the "EXIT" module in the program is executed. App close will be
	// forced programaticly when we are ready!
	#ifdef DOSLIKE
		setexitbuttonenable (0);
	#endif

	lstokens_init ();
	log_init ("lbasi.log");
	
	if (argc == 1) {
		spec = strdup ("boot");
		autoboot = 1;
	} else {
		spec = strdup (argv [1]);
		autoboot = 0;
	}

	res = lbasi_run (spec, autoboot);

	if (res < 0) {
		// Exit direct

	} else switch (res) {
		case 0:
			// Exit
			break;

		#ifndef DOSLIKE
			case 1:
				// Will only run on console backend mode
				printf ("LBAS Interpreter v0.1.20240201\n");
				printf ("Spec %s does not exist\n", spec);
				break;

			case 2: 
				// Will only run on console backend mode
				// Show usage
				printf ("LBAS Interpreter v0.1.20240201\n");
				printf (" $ lbasi <spec> - run <spec>.000, <spec>.001, etc\n");
				printf (" $ lbasi        - use <spec> = boot\n");
				break;
		#endif

		case 4:
			break;

		case 5:
			// Reset
			break;
	}

	lstokens_free ();
	free (spec);
	log_close ();

	#ifdef DOSLIKE
		forceshutdown ();
	#endif
}
