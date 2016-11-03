/* Educational sharing, possibly highly illegal, but taking the risk anyway. :) */
#include <unistd.h>

#include "debug.h"
#include "options.h"
#include "storage_allocator.h"

static Header base; /* empty list to get started */
static Header *freep = NULL; /* start of free list */

struct saOptions _saOptions;

/* morecore: ask system for more memory */
static Header *morecore(unsigned nu) {
	char *cp;
	Header *up;
	unsigned nalloc;

	nalloc = sysconf(_SC_PAGESIZE) / sizeof(Header); /* minimum #units to request */
	debugPrintf(debugLevelINFORMATIONAL, "morecore(%d => ", nu);
	if (nu < nalloc) {
		nu = nalloc;
	}
	debugPrintf(debugLevelINFORMATIONAL, "\b%d at ", nu);
	cp = sbrk(nu * sizeof(Header));
	if (cp == (char *) -1) { /* no space at all */
		debugPrintf(debugLevelCRITICAL, "\bFAILED,NO SPACE AT ALL!\n", cp);
		return(NULL);
	}
	debugPrintf(debugLevelINFORMATIONAL, "\b%p)\n", cp);
	up = (Header *)cp;
	up->s.size = nu;
	sa_free((void *)(up + 1));
	return(freep);
}

void memorydump(enum debugLevel level, char *s) {
	Header *p;

	if(debugLevelGet() >= level) {
		int allocations;

		if(s == NULL) {
			s = "";
		}
		debugPrintf(level, "Memory(%s, freep=%p):\n",s, freep);
		p = &base;
		allocations = 0;
		do {
			debugPrintf(level, "\t%p -> %d\n", p->s.ptr, p->s.ptr->s.size);
			allocations++;
		} while ((p = p->s.ptr) && (p != &base));
		debugPrintf(level, "allocations = %d\n", allocations);
	}
}

/* sa_mallocFIRST_FIT: general-purpose storage allocator, "first fit" mode of operation */
void *sa_mallocFIRST_FIT(unsigned nbytes) {
	Header *p, *prevp;
	unsigned nunits;

	nunits = ((nbytes + sizeof(Header) - 1) / sizeof(Header)) + 1;
	debugPrintf(debugLevelDEBUG, "(first fit) nunits(%d) = ((nbytes(%d) + header(%d) - 1) / header(%d)) + 1\n", nunits, nbytes, sizeof(Header), sizeof(Header));
	if ((prevp = freep) == NULL) { /* no free list yet */
		base.s.ptr = freep = prevp = &base;
		base.s.size = 0;
	}
	memorydump(debugLevelDEBUG, "before");
	for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
		debugPrintf(debugLevelDEBUG, "prevp = %p, p = %p\n", prevp, p);
		if (p->s.size >= nunits) { /* big enough */
			if (p->s.size == nunits) { /* exactly */
				prevp->s.ptr = p->s.ptr;
			} else { /* allocate tail end */
				p->s.size -= nunits;
				p += p->s.size;
				p->s.size = nunits;
			}
			freep = prevp;
			debugPrintf(debugLevelINFORMATIONAL, "%p=sa_malloc(%d)\n", (p + 1), nbytes);
			memorydump(debugLevelDEBUG, "after");
			return((void *)(p + 1));
		}
		if (p == freep) { /* wrapped around free list */
			if ((p = morecore(nunits)) == NULL) {
				debugPrintf(debugLevelINFORMATIONAL, "NULL=sa_malloc(%d)\n", nbytes);
				return(NULL); /* none left */
			}
		}
	}
}

/* sa_mallocBEST_FIT: general-purpose storage allocator, "best fit" mode of operation */
void *sa_mallocBEST_FIT(unsigned nbytes) {
	Header *p, *prevp;
	unsigned nunits;
	struct {
		long nunits;
		Header *prevp, *p;
	} fit;

	nunits = ((nbytes + sizeof(Header) - 1) / sizeof(Header)) + 1;
	debugPrintf(debugLevelDEBUG, "(best fit) nunits(%d) = ((nbytes(%d) + header(%d) - 1) / header(%d)) + 1\n", nunits, nbytes, sizeof(Header), sizeof(Header));
	if ((prevp = freep) == NULL) { /* no free list yet */
		base.s.ptr = freep = prevp = &base;
		base.s.size = 0;
	}
	memorydump(debugLevelDEBUG, "before");
	for (
		fit.prevp = prevp = &base, fit.p = p = prevp->s.ptr, fit.nunits = (long)p->s.size - nunits;
		(fit.nunits != 0) && (p != &base);
		prevp = p, p = p->s.ptr
	) {
		long diff;
		diff = (long)p->s.size - nunits;
		debugPrintf(debugLevelDEBUG, "%ld = %d - %d\n", diff, p->s.size, nunits);
		if (diff >= 0) {
			if ((fit.nunits < 0) || (fit.nunits > diff)) {
				fit.nunits = diff;
				fit.prevp = prevp;
				fit.p = p;
			}
		}
	}
	if (fit.nunits < 0) {
		if ((p = morecore(nunits)) == NULL) {
			debugPrintf(debugLevelINFORMATIONAL, "NULL=sa_malloc(%d)\n", nbytes);
			return(NULL); /* none left */
		} else {
			return(sa_mallocBEST_FIT(nbytes));
		}
	}
	if (fit.p->s.size == nunits) { /* exactly */
		fit.prevp->s.ptr = fit.p->s.ptr;
	} else { /* allocate tail end */
		fit.p->s.size -= nunits;
		fit.p += fit.p->s.size;
		fit.p->s.size = nunits;
	}
	debugPrintf(debugLevelINFORMATIONAL, "%p=sa_malloc(%d)\n", (fit.p + 1), nbytes);
	memorydump(debugLevelDEBUG, "after");
	return((void *)(fit.p + 1));
}

/* sa_malloc: general-purpose storage allocator */
void *sa_malloc(unsigned nbytes) {
	void *p;
	switch(_saOptions.mode) {
#define SA_MODE(name) \
		case sa_mode##name: { \
			p=sa_malloc##name(nbytes); \
			break; \
		}
		SA_MODES
#undef SA_MODE
		default: {
			p=NULL;
			break;
		}
	}
	return(p);
}

/* sa_free: put block ap in free list */
void sa_free(void *ap) {
	Header *bp, *p;

	memorydump(debugLevelDEBUG, "before");
	debugPrintf(debugLevelINFORMATIONAL, "free(%p)\n", ap);
	bp = (Header *)ap - 1; /* point to block header */
	debugPrintf(debugLevelDEBUG, "bp (new block) = %p\n", bp);
	debugPrintf(debugLevelDEBUG, "p (freep) = %p\n", freep);
	for (p = freep; !((bp > p) && (bp < p->s.ptr)); p = p->s.ptr) {
		debugPrintf(debugLevelDEBUG, "! (bp(%p) > p(%p)) && (bp(%p) < p->s.ptr(%p))\n", bp, p, bp, p->s.ptr);
		if ((p >= p->s.ptr) && ((bp > p) || (bp < p->s.ptr))) {
			debugPrintf(debugLevelDEBUG, "(p(%p) >= p->s.ptr(%p)) && ((bp(%p) > p(%p)) || (bp(%p) < p->s.ptr(%p)))\n", p, p->s.ptr, bp, p, bp, p->s.ptr);
			break; /* freed block at start or end of arena */
		}
	}
	debugPrintf(debugLevelDEBUG, "(bp(%p) + bp->s.size(%d)) == p->s.ptr(%p): ", bp, bp->s.size, p->s.ptr);
	if ((bp + bp->s.size) == p->s.ptr) { /* join to upper nbr */
		debugPrintf(debugLevelDEBUG, "\bYES - join to upper nbr\n");
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
	} else {
		debugPrintf(debugLevelDEBUG, "\bNO\n");
		bp->s.ptr = p->s.ptr;
	}
	debugPrintf(debugLevelDEBUG, "(p(%p) + p->s.size(%d)) == bp(%p): ", p, p->s.size, bp);
	if ((p + p->s.size) == bp) { /* join to lower nbr */
		debugPrintf(debugLevelDEBUG, "\bYES - join to lower nbr\n");
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	} else {
		debugPrintf(debugLevelDEBUG, "\bNO\n");
		p->s.ptr = bp;
	}
	freep = p;
	memorydump(debugLevelDEBUG, "after");
}

/* sa_mode: set the mode of operation. */
enum sa_mode sa_mode(enum sa_mode mode) {
	enum sa_mode save;
	/* Save the current mode of operation. */
	save = _saOptions.mode;
	/* Set the new mode of operation. */
	_saOptions.mode = mode;
	/* Return the saved mode of operation, which is the mode of operation
	   we have just overwritten. */
	return(save);
}
