/* vi: set sw=4 ts=4: */
/*
 * timerfd_create() / timerfd_settime() / timerfd_gettime() for uClibc
 *
 * Copyright (C) 2009 Stephan Raue <stephan@openelec.tv>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
/*#include <sys/timerfd.h>*/
#include "timerfd.h"

/*
 * timerfd_create()
 */
#ifdef __NR_timerfd_create
int timerfd_create(int clockid, int flags)
{
	return syscall(__NR_timerfd_create, clockid, flags);
}
#endif

/*
 * timerfd_settime()
 */
#ifdef __NR_timerfd_settime
int timerfd_settime(int ufd, int flags, const struct itimerspec *umtr, struct itimerspec *otmr)
{
	return syscall(__NR_timerfd_settime, ufd, flags, umtr, otmr);
}
#endif

