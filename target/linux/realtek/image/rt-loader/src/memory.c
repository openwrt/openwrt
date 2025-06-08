/*
 * rt-loader memory functions
 * (c) 2025 Markus Stockhausen
 *
 * This is a small function collection to get some rudimentary memory management working when
 * running bare metal. None of these functions is optimized but works well for current needs.
 */

#include "board.h"
#include "globals.h"
#include "memory.h"
#include "nanoprintf.h"

#define CACHE_OP(op, addr)			\
	__asm__ __volatile__(			\
	"	.set	push		\n"	\
	"	.set	noreorder	\n"	\
	"	.set	mips3\n\t	\n"	\
	"	cache	%0, %1		\n"	\
	"	.set	pop		\n"	\
	:					\
	: "i" (op), "R" (*(unsigned char *)(addr)))

void flush_cache(void *start_addr, unsigned long size)
{
	/*
	 * MIPS cores may have different cache lines. Most common are 16 and 32 bytes. Avoid
	 * detection routines or multiple implementations and take the lowest known value that
	 * will fit fine for cores with longer cache lines
	 */

	unsigned long lsize = 16;
	unsigned long addr = (unsigned long)start_addr & ~(lsize - 1);
	unsigned long aend = ((unsigned long)start_addr + size - 1) & ~(lsize - 1);

	while (1) {
		CACHE_OP(CACHE_HIT_INVALIDATE_I, addr);
		CACHE_OP(CACHE_HIT_WRITEBACK_INV_D, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}
}

void free(void *ptr)
{
	/* this is only one shot allocation */
}

int memcmp(const void *s1, const void *s2, size_t count)
{
	volatile char *p1 = (volatile char *)s1;
	volatile char *p2 = (volatile char *)s2;

	while (count--) {
		if (*p1 != *p2)
			return (int)(*p1) - (int)(*p2);

		p1++;
		p2++;
	}

	return 0;
}

void *memmove(void *dst, const void *src, size_t count)
{
	volatile char *d = (volatile char *)dst;
	volatile char *s = (volatile char *)src;

	if (d < s) {
		while (count--)
			*d++ = *s++;
	} else if (d > s) {
		d += count;
		s += count;
		while (count--)
			*--d = *--s;
	}

	return dst;
}

void *memcpy(void *dst, const void *src, size_t count)
{
	memmove(dst, src, count);
}

void *memset(void *dst, int c, size_t count)
{
	volatile char *d = (volatile char *)dst;

	while (count--)
		*d++ = c;

	return (void *)d;
}

void *malloc(size_t size)
{
	void *start;

	start = (void *)(((unsigned int)_heap_addr + MEMORY_ALIGNMENT - 1) & ~(MEMORY_ALIGNMENT - 1));
	if ((start + size) > _heap_addr_max) {
		printf("malloc(%d) failed. Only %dkB of %dkB heap left.\n",
		       size, (_heap_addr_max - start) >> 10, HEAP_SIZE >> 10);
		board_panic();
	}

	_heap_addr += size;

	return start;
}

size_t strlen(const char *s)
{
	const char *p = s;

	while (*p) ++p;

	return (size_t)(p - s);
}
