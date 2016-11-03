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

#ifndef __OPTIONS_H
#define __OPTIONS_H

#include "storage_allocator.h"

struct saOptions {
	int allocations;
	enum sa_mode mode;
	int size;
};

enum optionsRtrn {
	optionsRtrnOk=0,
	optionsRtrnUsage,
	optionsRtrnVersion,
	optionsRtrnImpossible
};

extern enum optionsRtrn optionsParse(int,char **);

#endif