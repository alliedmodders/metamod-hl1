// vi: set ts=4 sw=4 :
// vim: set tw=75 :

// log_mega.cpp - logging routines

/*
 * Copyright (c) 2001-2003 Will Day <willday@hpgx.net>
 *
 *    This file is part of Metamod.
 *
 *    Metamod is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    Metamod is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Metamod; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */

#include <stdio.h>		// vsnprintf, etc
#include <stdarg.h>		// va_start, etc

#include <extdll.h>				// always
#include <enginecallback.h>		// ALERT, etc

#include "log_meta.h"			// me
#include "osdep.h"				// win32 vsnprintf, etc

cvar_t meta_debug = {"meta_debug", "0", FCVAR_EXTDLL, 0, NULL};

// Print to console.
void META_CONS(char *fmt, ...) {
	va_list ap;
	char buf[MAX_LOGMSG_LEN];
	unsigned int len;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	len=strlen(buf);
	if(len < sizeof(buf)-2)		// -1 null, -1 for newline
		strcat(buf, "\n");
	else
		buf[len-1] = '\n';

	SERVER_PRINT(buf);
}

// Log developer-level messages (obsoleted).
void META_DEV(char *fmt, ...) {
	va_list ap;
	char buf[MAX_LOGMSG_LEN];
	int dev;

	dev=(int) CVAR_GET_FLOAT("developer");
	if(dev==0) return;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	ALERT(at_logged, "[META] dev: %s\n", buf);
}

// Log errors.
void META_ERROR(char *fmt, ...) {
	va_list ap;
	char buf[MAX_LOGMSG_LEN];

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	ALERT(at_logged, "[META] ERROR: %s\n", buf);
}

// Normal log messages.
void META_LOG(char *fmt, ...) {
	va_list ap;
	char buf[MAX_LOGMSG_LEN];

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	ALERT(at_logged, "[META] %s\n", buf);
}

// Print to client.
void META_CLIENT(edict_t *pEntity, char *fmt, ...) {
	va_list ap;
	char buf[MAX_CLIENTMSG_LEN];
	unsigned int len;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	len=strlen(buf);
	if(len < sizeof(buf)-2)		// -1 null, -1 for newline
		strcat(buf, "\n");
	else
		buf[len-1] = '\n';

	CLIENT_PRINTF(pEntity, print_console, buf);
}

