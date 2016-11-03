/* Educational sharing, possibly highly illegal, but taking the risk anyway. :) */
#ifndef __STORAGE_ALLOCATOR_H
#define __STORAGE_ALLOCATOR_H

#include "debug.h"

#define PROGRAM_NAME "storage_allocator"
#define PROGRAM_VERSION "1.0.0"

extern struct saOptions _saOptions;

typedef long Align; /* for alignment to long boundary */

union header { /* block header: */
	struct {
		union header *ptr; /* next block if on free list */
		unsigned size; /* size of this block */
	} s;
	Align x; /* force alignment of blocks */
};

typedef union header Header;

#define SA_MODES \
	SA_MODE(FIRST_FIT) \
	SA_MODE(BEST_FIT)

#define SA_MODE(name) sa_mode##name,
enum sa_mode {
	SA_MODES
};
#undef SA_MODE

/* Show the current state of the storage allocator. */
extern void memorydump(enum debugLevel, char *);

/* General-purpose storage allocator. */
extern void *sa_malloc(unsigned);

/* Put block ap in free list. */
extern void sa_free(void *);

/* Select the storage allocator mode of operation. */
extern enum sa_mode sa_mode(enum sa_mode);

#endif
