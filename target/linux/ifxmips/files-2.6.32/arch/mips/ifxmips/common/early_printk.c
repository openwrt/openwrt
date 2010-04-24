#include <linux/init.h>
#include <linux/cpu.h>

#include <ifxmips.h>

#ifdef CONFIG_IFXMIPS_PROM_ASC0
#define IFXMIPS_ASC_DIFF	0
#else
#define IFXMIPS_ASC_DIFF	IFXMIPS_ASC_BASE_DIFF
#endif

static char buf[1024];

static inline u32
asc_r32(unsigned long r)
{
	return ifxmips_r32((u32 *)(IFXMIPS_ASC_BASE_ADDR + IFXMIPS_ASC_DIFF + r));
}

static inline void
asc_w32(u32 v, unsigned long r)
{
	ifxmips_w32(v, (u32 *)(IFXMIPS_ASC_BASE_ADDR + IFXMIPS_ASC_DIFF + r));
}

void
prom_putchar(char c)
{
	unsigned long flags;

	local_irq_save(flags);
	while ((asc_r32(IFXMIPS_ASC_FSTAT) & ASCFSTAT_TXFFLMASK) >> ASCFSTAT_TXFFLOFF);

	if (c == '\n')
		asc_w32('\r', IFXMIPS_ASC_TBUF);
	asc_w32(c, IFXMIPS_ASC_TBUF);
	local_irq_restore(flags);
}

void
early_printf(const char *fmt, ...)
{
	va_list args;
	int l;
	char *p, *buf_end;

	va_start(args, fmt);
	l = vsprintf(buf, fmt, args);
	va_end(args);
	buf_end = buf + l;

	for (p = buf; p < buf_end; p++)
		prom_putchar(*p);
}


