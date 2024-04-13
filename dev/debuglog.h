#include <stdio.h>
#include <stdarg.h>

void log_init (char *filespec);
void log (const char *format, ...);
void log_close (void);

#ifdef DEBUGLOG_IMPLEMENTATION
#undef DEBUGLOG_IMPLEMENTATION

FILE *logfile;

void log_init (char *filespec) {
	logfile = fopen (filespec, "wb");
}

void log (const char *format, ...) {
	va_list args;
	va_start (args, format);
	vfprintf (logfile, format, args);
	va_end (args);
	fflush (logfile);
}

void log_close (void) {
	fclose (logfile);
}

#endif