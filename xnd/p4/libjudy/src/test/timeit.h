#ifndef _TIMEIT_H
#define	_TIMEIT_H

// @(#) $Revision: 4.14 $ $Source: /judy/src/apps/benchmark/timeit.h $
//
// Timing and timers header file with example program.
//
// You should compile with one of these defined:
//
//   JU_HPUX_PA		# control register available (via asm()).
//   JU_HPUX_IPF	# TBD, see below.
//   JU_LINUX_IA32	# control register available (via get_cycles()).
//   JU_LINUX_IPF	# control register available (via get_cycles()).
//   JU_WIN_IA32	# uses clock().
//
// Otherwise default (low-res) timing code using gettimeofday() results.  This
// mode is only accurate to usecs, and fuzzy due to syscall overhead.
//
// Public macros; the *_HRTm() forms are much faster than the others:
//
// TIMER_vars(T) - declare variables to use for timers
// STARTTm(T)    - start the timer with variable T
// ENDTm(D,T)    - compute usec from last STARTTm(T), save result in double D
// START_HRTm(T) - high-res for short intervals (< 2^32[64] clock ticks) only
// END_HRTm(D,T) - high-res for short intervals (< 2^32[64] clock ticks) only
//
// Private macros:
//
// __START_HRTm(T) - read high-res control register, save in T
// __END_HRTm(T)   - read high-res control register, save in T
// __HRONLY(D,T)   - use high-res clock only
//
// Note:  The __*_HRTm and __HRONLY macros are only available on platforms with
// control registers for high-res clocks.  On hpux_pa this is a 32-bit register
// and gettimeofday() must be used to handle rollover; on linux_* this is a
// 64-bit register.

#ifndef JU_WIN_IA32
#include <sys/time.h>		// Win32 uses a whole different paradigm.
#include <unistd.h>		// for getopt(), which Win32 lacks.
#endif

#include <time.h>

// Public variables:

extern double USecPerClock;	// defined in timeit.c.

// __HRONLY is used for multiple platforms, but only in cases where there is a
// high-res clock and find_CPU_speed() is available from timeit.c:

#define	__HRONLY(D,T)							\
	{								\
	    if (USecPerClock == 0.0) USecPerClock = find_CPU_speed();   \
	    (D) = ((double) (__stop_##T - __start_##T)) * USecPerClock; \
	}


// TIMING ROUTINES:


// ********************* HPUX PA ****************************
//
// Define __START_HRTm and __END_HRTm only, and let later common code add
// STARTTm and ENDTm.
//
// TBD:  On hpux_pa or hpux_ipf 64-bit, is CR_IT a 64-bit register?  If so, it's
// unnecessary and wasteful to use gettimeofday() later to watch for rollover.
//
// TBD:  JU_HPUX_IPF does not recognize CR_IT ("Undeclared variable 'CR_IT'"),
// so for now treat that platform as having no high-res clock and do not
// included it in this section.

#if (JU_HPUX_PA)
#include <machine/reg.h>
#include <sys/pstat.h>

double find_CPU_speed(void);

// Note:  On hpux_*, at least older compilers, it is neither necessary nor even
// allowed to mark the __start_* and __stop_* variables as volatile; the
// compiler does not optimize out the code even without it:

#define	TIMER_vars(T)					 \
	register unsigned long  __start_##T, __stop_##T; \
		 struct timeval __TVBeg_##T, __TVEnd_##T

#define	__START_HRTm(T) _asm("MFCTL", CR_IT, __start_##T)
#define	__END_HRTm(T)   _asm("MFCTL", CR_IT, __stop_##T )

#endif // (JU_HPUX_PA)

// ********************* LINUX IA32 **************************
//
#ifdef JU_LINUX_IA32
#include <asm/msr.h>

double find_CPU_speed(void);

#define	TIMER_vars(T)					 \
	register unsigned long  __start_##T, __stop_##T; \
		 struct timeval __TVBeg_##T, __TVEnd_##T

#define	__START_HRTm(T) rdtscl(__start_##T)
#define	__END_HRTm(T)   rdtscl(__stop_##T)

#endif // JU_LINUX_IA32

// ********************* LINUX_IPF **************************
//
// Define __START_HRTm and __END_HRTm, and also STARTTm and ENDTm in terms of
// the former (no need for gettimeofday()).

#ifdef JU_LINUX_IPF

#include <asm/timex.h>

double find_CPU_speed(void);

// Using cycles_t rather than unsigned long [long] should be more portable;
// and, it appears necessary to mark __start_* and __end_* as volatile so the
// gcc compiler does not optimize out the register access:

#define	TIMER_vars(T)						  \
	register volatile cycles_t	 __start_##T, __stop_##T; \
			  struct timeval __TVBeg_##T, __TVEnd_##T

// This seems required for linux_ia32:


// Older code (see 4.13) used rdtscl(), but this is not portable and does not
// result in a 64-bit value, unlike get_cycles(), which apparently takes
// advantage of a 64-bit control register on both IA32 and IPF => always
// high-res timing with no rollover issues.  Note, cycles_t is unsigned, so the
// math works even in case of a rollover.

#define	__START_HRTm(T)  __start_##T = get_cycles()
#define	__END_HRTm(T)    __stop_##T  = get_cycles()

#define	STARTTm(T)  __START_HRTm(T)
#define	ENDTm(D,T)  { __END_HRTm(T); __HRONLY(D,T); }

#endif // JU_LINUX_IPF


// ********************* WIN IA32 *****************************
//
// WIN IA32 has no way to access the control register (?), so define STARTTm
// and ENDTm directly using clock():

#ifdef JU_WIN_IA32

clock_t TBeg, TEnd;

#define	TIMER_vars(T)  struct timeval __TVBeg_##T, __TVEnd_##T

#define	STARTTm(T)   __TVBeg_##T = clock()
#define	ENDTm(D,T) { __TVEnd_##T = clock(); \
		     (D) = ((double) (__TVEnd_##T - __TVBeg_##T)); }

#endif // JU_WIN_IA32


// ********************* OTHER *****************************
//
// Default to using the low-res, slow-access clock only.

#ifndef TIMER_vars

#define	TIMER_vars(T)  struct timeval __TVBeg_##T, __TVEnd_##T

#define	STARTTm(T)  gettimeofday(&__TVBeg_##T, NULL)

#define	ENDTm(D,T)  gettimeofday(&__TVEnd_##T, NULL);			  \
		    (D) = (((double) __TVEnd_##T.tv_sec * ((double) 1E6)) \
			  + (double) __TVEnd_##T.tv_usec)		  \
			- (((double) __TVBeg_##T.tv_sec * ((double) 1E6)) \
			  + (double) __TVBeg_##T.tv_usec)
#endif // ! TIMER_vars


// COMMON CODE FOR SYSTEMS WITH HIGH-RES CLOCKS (CONTROL REGISTERS):

#ifdef __START_HRTm

// Platforms that define __START_HRTm but not STARTTm (presently only hpux_pa)
// use gettimeofday() for the low-res clock and __START_HRTm/__END_HRTm for
// the high-res clock.  If the low-res clock did not "roll over", use the
// high-res clock; see __HRONLY.
//
// Note:  Rollover is defined conservatively as 1E5 usec (= 1/10 sec).  This
// would require a 40 GHz 32-bit system to be violated.

#ifndef STARTTm
#define	STARTTm(T)							\
	{								\
	    gettimeofday(&__TVBeg_##T, NULL); __START_HRTm(T);		\
	}

#define	ENDTm(D,T)							\
	{								\
	    __END_HRTm(T); gettimeofday(&__TVEnd_##T, NULL);		\
									\
	    (D) = (((double) __TVEnd_##T.tv_sec * ((double) 1E6))	\
		  + (double) __TVEnd_##T.tv_usec)			\
		- (((double) __TVBeg_##T.tv_sec * ((double) 1E6))	\
		  + (double) __TVBeg_##T.tv_usec);			\
									\
	    if ((D) < 1E5) __HRONLY(D,T);				\
	}
#endif // ! STARTTm

// Faster forms for heavy/frequent use in code loops where intervals are less
// than 2^32[64] clock ticks:

#define	START_HRTm(T)  __START_HRTm(T)
#define	END_HRTm(D,T)  { __END_HRTm(T); __HRONLY(D,T); }

#endif // __START_HRTm

#endif // #ifndef _TIMEIT_H
