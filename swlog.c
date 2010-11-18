
#include <stdarg.h>
#include <stdio.h>
#include "swlog.h"

static FILE *logfile = NULL;

static void openlogfile(void)
{
	if (!logfile)
		logfile = fopen("swlog", "w");
}

static void sw_log(const char *pre, const char *fmt, va_list args)
{
	openlogfile();

	if (logfile) {
		fprintf(logfile, "%s", pre);
		vfprintf(logfile, fmt, args);
		fprintf(logfile, "\n");
		fflush(logfile);
	}
}

void sw_logmsg(const char *fmt, ...)
{
#if SWLOG == 1
	va_list args;
	va_start(args, fmt);

	sw_log("msg: ", fmt, args);

	va_end(args);
#endif
}

void sw_logerr(const char *fmt, ...)
{
#if SWLOG == 1
	va_list args;
	va_start(args, fmt);

	sw_log("err: ", fmt, args);

	va_end(args);
#endif
}

void sw_logwrn(const char *fmt, ...)
{
#if SWLOG == 1
	va_list args;
	va_start(args, fmt);

	sw_log("wrn: ", fmt, args);

	va_end(args);
#endif
}

void sw_lograw(const char *fmt, ...)
{
#if SWLOG == 1
	va_list args;
	va_start(args, fmt);

	openlogfile();

	if (logfile) {
		vfprintf(logfile, fmt, args);
		fflush(logfile);
	}

	va_end(args);
#endif
}

