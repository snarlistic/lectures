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
#include <stdlib.h>
#include <getopt.h>

#include "options.h"
#include "storage_allocator.h"
#include "debug.h"

static void usage(void) {
	printf(
		"Usage: %s [--version] [--help] [--debug=level]\n"
		"         [--allocations=allocations] [--size=size] [--mode=mode]\n",
		PROGRAM_NAME
	);
}

static void optionsDefault(void) {
	/* Amount of memory allocation slots. */
	_saOptions.allocations = 1024;

	/* Show messages at debug level ALERT and higher. */
	debugLevelSet(debugLevelALERT);

	/* By default we run at a first fit mode of operation. */
	sa_mode(sa_modeFIRST_FIT);

	/* Maximum size of a memory allocation. */ 
	_saOptions.size = 1024;
}

enum optionsRtrn optionsParse(int argc,char **argv) {
	int finished;
	optionsDefault();
	for(finished = 0; !finished;) {
		int option, option_index;
		static struct option long_options[] = {
			{"allocations", required_argument, NULL, 'a'},
			{"debug", required_argument, NULL, 'd'},
			{"mode", required_argument, NULL, 'm'},
			{"size", required_argument, NULL, 's'},
			{"help", no_argument, NULL, 'h'},
			{"version", no_argument, NULL, 'v'},
			{NULL, no_argument, NULL, 0}
		};
		option = getopt_long(argc, argv, "a:d:hm:s:v", long_options, &option_index);
		switch(option) {
			case -1: {
				finished = !finished;
				break;
			}
			case 'a': {
				int allocations;
				allocations = atoi(optarg);
				if (allocations > 0) {
					_saOptions.allocations = allocations;
				} else {
					fprintf(stderr, "%s: Discarding allocations. It should be bigger than zero.\n", PROGRAM_NAME);
				}
				break;
			}
			case 'd': {
				int level;
				level = atoi(optarg);
				if ((level >= (int)debugLevelNONE) && (level <= (int)debugLevelDEBUG)) {
					debugLevelSet((enum debugLevel)level);
				} else {
					fprintf(stderr, "%s: Discarding debug level. It should be in between %d and %d.\n",PROGRAM_NAME, debugLevelNONE, debugLevelDEBUG);
				}
				break;
			}
			case 'h': {
				usage();
				return(optionsRtrnUsage);
			}
			case 'm': {
				int mode;
				mode = atoi(optarg);
				if (mode >= 0) {
					_saOptions.mode = (enum sa_mode)mode;
				} else {
					fprintf(stderr, "%s: Discarding mode. It should be a positive number.\n", PROGRAM_NAME);
				}
				break;
			}
			case 'v': {
				printf(
					"%s version %s\n"
					"Copyright (C) 2016-2016 Freddy Spierenburg <freddy@snarl.nl>\n"
					"This is free software.  You may redistribute copies of it under the terms of\n"
					"the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.\n"
					"There is NO WARRANTY, to the extent permitted by law.\n",
					PROGRAM_NAME, PROGRAM_VERSION
				);
				return(optionsRtrnVersion);
			}
			case 's': {
				int size;
				size = atoi(optarg);
				if (size > 0) {
					_saOptions.size = size;
				} else {
					fprintf(stderr, "%s: Discarding size. It should be bigger than zero.\n", PROGRAM_NAME);
				}
				break;
			}
		}
	}

	return(optionsRtrnOk);
}
