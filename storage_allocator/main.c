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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "options.h"
#include "storage_allocator.h"

static int _running, _report;

void handle_signal(int signal) {
	switch(signal) {
		case SIGINT: {
			_running = 0;
			break;
		}
		case SIGUSR1: {
			_report = 1;
			break;
		}
	}
}

int main(int argc, char **argv) {
	enum optionsRtrn returnValue;

	returnValue = optionsParse(argc, argv);
	if (returnValue == optionsRtrnOk) {
		struct sigaction sa;
		void **memory;
		int index, usage;

		sa.sa_handler = &handle_signal;
		sa.sa_flags = SA_RESTART;
		sigfillset(&sa.sa_mask);
		if(sigaction(SIGINT, &sa, NULL) == -1) {
			perror("Error: cannot handle SIGINT");
		}
		if(sigaction(SIGUSR1, &sa, NULL) == -1) {
			perror("Error: cannot handle SIGUSR1");
		}

		debugPrintf(debugLevelALERT, "Allocate memory for %d allocations", _saOptions.allocations);
		debugPrintf(debugLevelALERT, "\b, each %d bytes max.\n", _saOptions.size);
		memory = calloc(_saOptions.allocations, sizeof(*memory));
		usage = 0;
		_report = 0;
		for (_running = 1; _running;) {
			if (_report) {
				debugPrintf(debugLevelALERT, "Request to report received: currently %d memory slots in use.\n", usage);
				memorydump(debugLevelALERT, "");
				_report = 0;
			}
			index = rand() % _saOptions.allocations;
			if (memory[index] == NULL) {
				debugPrintf(debugLevelNOTICE, "ALLOCATE: %p(%d)\n", &(memory[index]), index);
				memory[index] = sa_malloc(rand() % _saOptions.size);
				if (memory[index] != NULL) {
					usage++;
				}
			}
			index = rand() % _saOptions.allocations;
			if (memory[index] != NULL) {
				debugPrintf(debugLevelNOTICE, "FREE: %p(%d)\n", &(memory[index]), index);
				sa_free(memory[index]);
				memory[index] = NULL;
				usage--;
			}
		}
		debugPrintf(debugLevelALERT, "Request to stop received: currently %d memory slots in use.\n", usage);
		for (index = _saOptions.allocations - 1; index >= 0; index--) {
			if (memory[index] != NULL) {
				debugPrintf(debugLevelNOTICE, "FREE: %p(%d)\n", &(memory[index]), index);
				sa_free(memory[index]);
			}
		}
		free(memory);
	}
	return(returnValue);
}
