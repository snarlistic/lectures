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

#ifndef __DEBUG_H
#define __DEBUG_H

/* These are the several debug levels that do exist. The higher the debug level
 * the more debug messages will be output. */
#define DEBUG_LEVELS \
	DEBUG_LEVEL(NONE) \
	DEBUG_LEVEL(EMERGENCY) \
	DEBUG_LEVEL(ALERT) \
	DEBUG_LEVEL(CRITICAL) \
	DEBUG_LEVEL(ERROR) \
	DEBUG_LEVEL(WARNING) \
	DEBUG_LEVEL(NOTICE) \
	DEBUG_LEVEL(INFORMATIONAL) \
	DEBUG_LEVEL(DEBUG)

#define DEBUG_LEVEL(name) debugLevel##name,
enum debugLevel {
	DEBUG_LEVELS
};
#undef DEBUG_LEVEL

extern enum debugLevel debugLevelGet(void);
extern enum debugLevel debugLevelSet(enum debugLevel);
/* Is this software built with debug support enabled? */
#ifdef DEBUG
/* Yes, do this external routine does exist. */
extern void debugPrintf(enum debugLevel,char *, ...);
#else
/* No, so voiding all calls to debugPrintf(). */
#define debugPrintf(...)
#endif

#endif
