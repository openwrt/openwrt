/* Copyright (C) 2008 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef	_SYS_TIMERFD_H
#define	_SYS_TIMERFD_H	1

#include <time.h>


/* Bits to be set in the FLAGS parameter of `timerfd_create'.  */
enum
  {
    TFD_CLOEXEC = 02000000,
#define TFD_CLOEXEC TFD_CLOEXEC
    TFD_NONBLOCK = 04000
#define TFD_NONBLOCK TFD_NONBLOCK
  };


/* Bits to be set in the FLAGS parameter of `timerfd_settime'.  */
enum
  {
    TFD_TIMER_ABSTIME = 1 << 0
#define TFD_TIMER_ABSTIME TFD_TIMER_ABSTIME
  };


__BEGIN_DECLS

/* Return file descriptor for new interval timer source.  */
extern int timerfd_create (clockid_t __clock_id, int __flags) __THROW;

/* Set next expiration time of interval timer source UFD to UTMR.  If
   FLAGS has the TFD_TIMER_ABSTIME flag set the timeout value is
   absolute.  Optionally return the old expiration time in OTMR.  */
extern int timerfd_settime (int __ufd, int __flags,
			    __const struct itimerspec *__utmr,
			    struct itimerspec *__otmr) __THROW;

/* Return the next expiration time of UFD.  */
extern int timerfd_gettime (int __ufd, struct itimerspec *__otmr) __THROW;

__END_DECLS

#endif /* sys/timerfd.h */

