#ifndef __PERF_MUSL_COMPAT_H
#define __PERF_MUSL_COMPAT_H

#ifndef __ASSEMBLER__

#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <syscall.h>
#include <sched.h>

#undef _IOWR
#undef _IOR
#undef _IOW
#undef _IOC
#undef _IO

/* Change XSI compliant version into GNU extension hackery */
static inline char *
gnu_strerror_r(int err, char *buf, size_t buflen)
{
	if (strerror_r(err, buf, buflen))
		return NULL;
	return buf;
}
#define strerror_r gnu_strerror_r

#define _SC_LEVEL1_DCACHE_LINESIZE -1

static inline long sysconf_wrap(int name)
{
	FILE *f;
	int val;

	switch (name) {
	case _SC_LEVEL1_DCACHE_LINESIZE:
		f = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");
		if (!f)
			return 0;

		if (fscanf(f, "%d", &val) != 1)
			return 0;

		fclose(f);
		return val;
	default:
		return sysconf(name);
	}
}

#define sysconf(_n) sysconf_wrap(_n)

static inline int compat_sched_getcpu(void)
{
#ifdef __NR_getcpu
	unsigned int val;

	if (syscall(__NR_getcpu, &val))
		return -1;

	return val;
#else
	return -1;
#endif
}

#define sched_getcpu compat_sched_getcpu

#endif
#endif
