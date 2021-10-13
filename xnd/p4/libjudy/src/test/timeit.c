// @(#) $Revision: 4.12 $ $Source: /judy/judy/src/apps/benchmark/timeit.c,v $
//
// Timer functions.
//
// YOU MUST COMPILE THIS WITH ONE OF THESE DEFINED:
//
//   JU_HPUX_PA
//   JU_HPUX_IPF
//   JU_LINUX_IA32
//   JU_LINUX_IPF
//   JU_WIN_IA32
//
// If NONE of these are defined, this whole code section is ifdef'd out to
// avoid compile/link errors due to usage here of objects not defined in
// timeit.h.  See end of file.
//
// TBD:  Improve on this; possibly it's OK now not to ifdef this out, and get
// default, low-res timing behavior; see timeit.h.
//
// Compile with -D_TIMEIT_TEST to include a main program for testing; see
// main() below.

#if (JU_HPUX_PA || JU_LINUX_IA32 || JU_LINUX_IPF)
#define _TIMEIT_HIGHRES
#endif

#ifdef _TIMEIT_HIGHRES

#include <sys/time.h>		// Win32 uses a whole different paradigm.
#include <unistd.h>		// for getopt(), which Win32 lacks.
#include <time.h>
#include "timeit.h"

double USecPerClock;	// usec per control register count.


// ****************************************************************************
// F I N D   C P U   S P E E D
//
// Return microseconds per control/timer register count.  Examples:
//
// 0.002 for 500 MHz processor
// 0.001 for 1 GHz processor

double find_CPU_speed(void)
{
	double DeltaUSec;	// Timing result in uSec.
	TIMER_vars(tm);		// creates __TVBeg_tm, ...
				// (used for consistency with __START_HRTm)

	gettimeofday(&__TVBeg_tm, NULL);	// get low-res time.
	__START_HRTm(tm);			// get __start_tm (high-res).
	sleep(1);				// time passes; 1 sec suffices.
	__END_HRTm(tm);				// get __stop_tm (high-res).
	gettimeofday(&__TVEnd_tm, NULL);	// get low-res time.

// gettimeofday() returns usec; compute elapsed time:

	DeltaUSec = (((double) __TVEnd_tm.tv_sec * ((double) 1E6))
		    + (double) __TVEnd_tm.tv_usec)
		  - (((double) __TVBeg_tm.tv_sec * ((double) 1E6))
		    + (double) __TVBeg_tm.tv_usec);

// Control register returns ticks, and the ratio can now be computed:

	return (DeltaUSec / ((double) (__stop_tm - __start_tm)));

} // find_CPU_speed()

#else // _TIMEIT_HIGHRES
void dummy() {}	 // avoid "empty source file" warnings when no _TIMEIT_TEST.
#endif


// ****************************************************************************
//
// Ifdef the test main() separately, including #includes, for platforms that do
// not define find_CPU_speed() above.

#ifdef _TIMEIT_TEST

#include <sys/time.h>		// Win32 uses a whole different paradigm.
#include <unistd.h>		// for getopt(), which Win32 lacks.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
//#include <values.h>             // for MAXDOUBLE
#define MAXDOUBLE       (10e99)
#include "timeit.h"


// ****************************************************************************
// M A I N
//
// Example code for timeit:
//
// To compile and test this program on HP-UX, run the next lines as commands.
//
//   cc -Wl,-a,archive -DJU_HPUX_PA -D__HPUX__ -D_TIMEIT_TEST -o timeit timeit.c
//   timeit			# run test program.
//   rm -f timeit		# clean up after test.

int main(int argc, char **argv)
{
	int     i = 0;		// loop index.
	long    i_max = 10;	// number of loops.
	int     preload = 1;	// loops to throw away (preload cache).
	double  ztime;		// timer overhead.
	double  usec[4];	// for early timing tests.
	double  DeltaUSec;	// timing result in usec.
	double  prevtime;	// from previous loop.
	double  mintime;	// minimum event time.
	struct timeval tmjunk;	// for throw-away syscall.
	TIMER_vars(tm1);	// overall timer variables.
	TIMER_vars(tm2);	// misc + loop timer variables.


// INITIALIZE:

	STARTTm(tm1);		// whole program timer.

	i_max += preload;

// The first arg is the number of iterations (default is i_max):

	if (argc > 1)
	{
	    i = atoi(argv[1]) + preload;
	    if (i > 0) i_max = (long)i;
	}

// Calculate timer overhead (ztime):

#ifdef _TIMEIT_HIGHRES
	(void) puts("Possible slight delay here due to find_CPU_speed()...");
#else
	(void) puts("No high-res clock or find_CPU_speed() for this platform.");
#endif

	ztime = 0.0;

	for (i = 0; i < 100; ++i)	// average many runs.
	{
	    STARTTm(tm2);
	    ENDTm(DeltaUSec, tm2);
	    ztime += DeltaUSec;
	}
	ztime = ztime / ((double) i);


// SIMPLE TESTS OF TIMER OVERHEAD:
//
// Make two passes at both the high-res (if any) and slower timers.

	(void) puts("\nTiming timers themselves: start, end, end");

#define	PRINTPASS(Desc,Pass)						\
	(void) printf("%-8s pass %d:  %f - %f = %f usec\n", Desc, Pass,	\
		      usec[((Pass) * 2) - 1],  usec[((Pass) * 2) - 2],	\
		      usec[((Pass) * 2) - 1] - usec[((Pass) * 2) - 2])

#ifdef _TIMEIT_HIGHRES
	START_HRTm(tm2);
	END_HRTm(usec[0], tm2);	// throw away in case of sleep(1) here.

	START_HRTm(tm2);
	END_HRTm(usec[0], tm2);
	END_HRTm(usec[1], tm2);

	START_HRTm(tm2);
	END_HRTm(usec[2], tm2);
	END_HRTm(usec[3], tm2);

	PRINTPASS("High-res", 1);
	PRINTPASS("High-res", 2);
#endif

	STARTTm(tm2);
	ENDTm(usec[0], tm2);	// throw away in case of sleep(1) here.

	STARTTm(tm2);
	ENDTm(usec[0], tm2);
	ENDTm(usec[1], tm2);

	STARTTm(tm2);
	ENDTm(usec[2], tm2);
	ENDTm(usec[3], tm2);

	PRINTPASS("Non-HR", 1);
	PRINTPASS("Non-HR", 2);


// PRINT INITIAL INFO:

#ifdef _TIMEIT_HIGHRES

// Print the CPU speed:
//
// Note:  USecPerClock is a global set by the first instance of STARTTm.  You
// can also get this number by calling find_CPU_speed().

	(void) printf("\nClock step = %.3f nsec => %.1f MHz.\n",
		      USecPerClock * 1000.0, 1.0 / USecPerClock);
#endif

// Print timer overhead even though it's been subtracted from the reported
// results.

	(void) printf("Timer overhead subtracted from the times below = %f "
		      "usec.\n", ztime);


// DO A FAST TIMER CHECK:

	(void) puts("\nCheck timer precision by repeating the same action:");
	(void) puts("Times in each group should be close together.");
	(void) puts("\nTiming something very fast: \"++i\":");

	mintime = MAXDOUBLE;

	for (i = 1; i <= i_max; ++i)
	{
	    prevtime = DeltaUSec;
	    STARTTm(tm2);		// start the timer.
	    ++i;			// statement to time.
	    ENDTm(DeltaUSec, tm2);	// stop the timer.
	    DeltaUSec -= ztime;		// remove timer overhead.

// Throw away the first loop iteration to warm up the cache:

	    if (--i > preload)
	    {
		if (mintime == MAXDOUBLE) mintime = DeltaUSec;

		(void) printf("%3d. %8.3f nanosec,\tmintime diff %8.1f %%\n",
			      i - preload, DeltaUSec * 1000.0,
			      ((DeltaUSec - mintime) * 100) / mintime);

		if (DeltaUSec < mintime) mintime = DeltaUSec;
	    }
	}


// TIME A FUNCTION CALL:

	(void) puts("\nTiming a function: \"gettimeofday()\":");
	mintime = MAXDOUBLE;

	for (i = 1; i <= i_max; ++i)
	{
	    prevtime = DeltaUSec;
	    STARTTm(tm2);			// start the timer.
	    gettimeofday(&tmjunk, NULL);	// burn some cycles.
	    ENDTm(DeltaUSec, tm2);		// stop the timer.
	    DeltaUSec -= ztime;			// remove timer overhead.

// Throw away the first loop iteration to warm up the cache:

	    if (i > preload)
	    {
		if (mintime == MAXDOUBLE) mintime = DeltaUSec;

		(void) printf("%3d. %8.3f usec,\tmintime diff %8.1f %%\n",
			      i - preload, DeltaUSec,
			      ((DeltaUSec - mintime) * 100) / mintime);

		if (DeltaUSec < mintime) mintime = DeltaUSec;
	    }
	}


// TIME SOMETHING SLOW:

	(void) puts("\nTiming something slow: \"sleep(1)\":");
	mintime = MAXDOUBLE;

	for (i = 1; i <= i_max; ++i)
	{
	    prevtime = DeltaUSec;
	    STARTTm(tm2);		// start the timer.
	    sleep(1);
	    ENDTm(DeltaUSec, tm2);	// stop the timer.
	    DeltaUSec -= ztime;		// remove timer overhead.

// Throw away the first loop iteration to warm up the cache:

	    if (i > preload)
	    {
		if (mintime == MAXDOUBLE) mintime = DeltaUSec;

		(void) printf("%3d. %8.3f sec,\tmintime diff %8.1f %%\n",
			      i - preload, DeltaUSec/1E6,
			      ((DeltaUSec - mintime) * 100) / mintime);

		if (DeltaUSec < mintime) mintime = DeltaUSec;
	    }
	}


// FINISH UP:
//
// Print program execution time:

	ENDTm(DeltaUSec, tm1);
	(void) printf("\nProgram execution time: %.3f sec\n", DeltaUSec / 1E6);
	return(0);

} // main()

#endif // #ifdef _TIMEIT_TEST
