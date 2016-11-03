/* storage_allocator - a general-purpose storage allocator
 * Copyright (C) 2016-2016 Freddy Spierenburg
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <syslog.h>
#include <time.h>

#include "debug.h"

static struct debug {
	enum debugLevel level;
} _debug={
	.level=debugLevelNONE
};

enum debugLevel debugLevelGet(void) {
	/* Return the current debug level. */
	return(_debug.level);
}

/* Is this software built with debug support enabled? */
#ifdef DEBUG
/* Yes, this is a nice useful routine. */
enum debugLevel debugLevelSet(enum debugLevel level) {
	enum debugLevel save;
	/* Save the current debug level. */
	save=_debug.level;
	/* Set the new debug level. */
	_debug.level=level;
	/* Return the saved debug level, which is the debug level
	 * we have just overwritten. */
	return(save);
}
#else
	/* No, this routine does not do much. */
enum debugLevel debugLevelSet(enum debugLevel level) {
	/* Is any debug level required? */
	if(level!=debugLevelNONE) {
		/* Yes, unfortunately this software is built without debug support. Write
		 * a nice error message on stderr. */
		fprintf(
			stderr,
			"This software is unfortunately compiled without debug support, sorry!\n"
			"Try to rebuild the software with DEBUG enabled in the Makefile.\n"
		);
	}
	/* Return the debug level, more to keep the compiler happy than it is
	 * of any use. But I might be wrong and usage pays of later... :) */
	return(_debug.level);
}
#endif

/* Is this software built with debug support enabled? */
#ifdef DEBUG
/* Yes, so we compile the software with this routine below active. If not,
 * the header file will define a bogus debugPrintf which will cause all
 * calls to this routine to be void. */
void debugPrintf(enum debugLevel level,char *fmt, ...) {
	if(level<=_debug.level) {
		va_list args;
		struct timeval timeCurrent;
		struct tm *time;
		char timestamp[32];
		char buffer[256];
		/* Do we have a format and wants the user a concatenation of the last message? */
		if((fmt!=NULL)&&(fmt[0]=='\b')) {
			/* Yes, create an empty timestamp and... */
			timestamp[0]='\0';
			/* ...remove the concatenation mark from the format. */
			fmt++;
		} else {
			/* Nope, so get the current time of day. */
			gettimeofday(&timeCurrent,NULL);
			/* Convert it to a nice printable type... */
			time=localtime(&timeCurrent.tv_sec);
			/* ...and than fill our timestamp with it. */
			snprintf(
				timestamp,sizeof(timestamp),
				"%04d-%02d-%02d %02d:%02d:%02d.%06ld: ",
				time->tm_year+1900,time->tm_mon+1,time->tm_mday,
				time->tm_hour,time->tm_min,time->tm_sec,timeCurrent.tv_usec
			);
		}
		/* Initialize variable argument list. */
		va_start(args,fmt);
		/* Put the arguments inside the temporarily buffer. */
		vsnprintf(buffer,sizeof(buffer),fmt,args);
		/* Destroy the argument list again. */
		va_end(args);
		/* Write the message to stderr. */
		fprintf(stderr,"%s%s",timestamp,buffer);
		/* Also write it to the syslog. */
		/* syslog(LOG_DEBUG,"%s%s",timestamp,buffer); */
	}
}
#endif
