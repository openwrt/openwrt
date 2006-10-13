/*
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * Low resolution timer interface linux specific implementation.
 *
 * $Id$
 */

/*
* debug facilities
*/
#define TIMER_DEBUG	0
#if TIMER_DEBUG
#define TIMERDBG(fmt, args...) printf("%s: " fmt "\n" , __FUNCTION__ , ## args)
#else
#define TIMERDBG(fmt, args...)
#endif


/*
 * POSIX timer support for Linux. Taken from linux_timer.c in upnp
 */

#define __USE_GNU


#include <stdlib.h>	    // for malloc, free, etc.
#include <string.h>	    // for memset, strncasecmp, etc.
#include <assert.h>	    // for assert, of course.
#include <signal.h>	    // for sigemptyset, etc.
#include <stdio.h>	    // for printf, etc.
#include <sys/time.h>
#include <time.h>

/* define TIMER_PROFILE to enable code which guages how accurate the timer functions are.
   For each expiring timer the code will print the expected time interval and the actual time interval.
#define TIMER_PROFILE
*/
#undef TIMER_PROFILE

/*
timer_cancel( ) - cancel a timer
timer_connect( ) - connect a user routine to the timer signal
timer_create( ) - allocate a timer using the specified clock for a timing base (POSIX)
timer_delete( ) - remove a previously created timer (POSIX)
timer_gettime( ) - get the remaining time before expiration and the reload value (POSIX)
timer_getoverrun( ) - return the timer expiration overrun (POSIX)
timer_settime( ) - set the time until the next expiration and arm timer (POSIX)
nanosleep( ) - suspend the current task until the time interval elapses (POSIX)
*/

#define MS_PER_SEC 1000
#define US_PER_SEC 1000000
#define US_PER_MS  1000
#define UCLOCKS_PER_SEC 1000000

typedef void (*event_callback_t)(timer_t, int);

#ifndef TIMESPEC_TO_TIMEVAL
# define TIMESPEC_TO_TIMEVAL(tv, ts) {                                   \
        (tv)->tv_sec = (ts)->tv_sec;                                    \
        (tv)->tv_usec = (ts)->tv_nsec / 1000;                           \
}
#endif

#ifndef TIMEVAL_TO_TIMESPEC
# define TIMEVAL_TO_TIMESPEC(tv, ts) {                                   \
        (ts)->tv_sec = (tv)->tv_sec;                                    \
        (ts)->tv_nsec = (tv)->tv_usec * 1000;                           \
}
#endif

#define ROUNDUP(x,y) ((((x)+(y)-1)/(y))*(y))

#define timerroundup(t,g) \
    do { \
	if (!timerisset(t)) (t)->tv_usec=1; \
	if ((t)->tv_sec == 0) (t)->tv_usec=ROUNDUP((t)->tv_usec, g); \
    } while (0)

typedef long uclock_t;

#define TFLAG_NONE	0
#define TFLAG_CANCELLED	(1<<0)
#define TFLAG_DELETED	(1<<1)

struct event {
    struct timeval it_interval;
    struct timeval it_value;
    event_callback_t func;
    int arg;
    unsigned short flags;
    struct event *next;
#ifdef TIMER_PROFILE
    uint expected_ms;
    uclock_t start;
#endif
};

void timer_cancel(timer_t timerid);

static void alarm_handler(int i);
static void check_event_queue();
static void print_event_queue();
static void check_timer();
#if THIS_FINDS_USE
static int count_queue(struct event *);
#endif
static int timer_change_settime(timer_t timer_id, const struct itimerspec *timer_spec);
void block_timer();
void unblock_timer();

static struct event *event_queue = NULL;
static struct event *event_freelist;
static uint g_granularity;
static int g_maxevents = 0;

uclock_t uclock()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * US_PER_SEC) + tv.tv_usec);
}


void init_event_queue(int n)
{
    int i;
    struct itimerval tv;
    
    g_maxevents = n;
    event_freelist = (struct event *) malloc(n * sizeof(struct event));
    memset(event_freelist, 0, n * sizeof(struct event));

    for (i = 0; i < (n-1); i++) 
	event_freelist[i].next = &event_freelist[i+1];

    event_freelist[i].next = NULL;

    tv.it_interval.tv_sec = 0;
    tv.it_interval.tv_usec = 1;
    tv.it_value.tv_sec = 0;
    tv.it_value.tv_usec = 0;
    setitimer (ITIMER_REAL, &tv, 0);
    setitimer (ITIMER_REAL, 0, &tv);
    g_granularity = tv.it_interval.tv_usec;

    signal(SIGALRM, alarm_handler);
}


int clock_gettime(
    clockid_t         clock_id, /* clock ID (always CLOCK_REALTIME) */
    struct timespec * tp        /* where to store current time */
)
{
    struct timeval tv;
    int n;


    n = gettimeofday(&tv, NULL);
    TIMEVAL_TO_TIMESPEC(&tv, tp);
    
    return n;
}


int timer_create(
    clockid_t         clock_id, /* clock ID (always CLOCK_REALTIME) */
    struct sigevent * evp,      /* user event handler */
    timer_t *         pTimer    /* ptr to return value */
)
{
    struct event *event;

    if (clock_id != CLOCK_REALTIME) {
	TIMERDBG("timer_create can only support clock id CLOCK_REALTIME");
	exit(1);
    }

    if (evp != NULL) {
	if (evp->sigev_notify != SIGEV_SIGNAL || evp->sigev_signo != SIGALRM) {
	    TIMERDBG("timer_create can only support signalled alarms using SIGALRM");
	    exit(1);
	}
    }

    event = event_freelist;
    if (event == NULL) {
	print_event_queue();
    }
    assert(event != NULL);

    event->flags = TFLAG_NONE;
    
    event_freelist = event->next;
    event->next = NULL;

    check_event_queue();

    *pTimer = (timer_t) event;

    return 0;
}

int timer_delete(
    timer_t timerid /* timer ID */
)
{
    struct event *event = (struct event *) timerid;
    
    if (event->flags & TFLAG_DELETED) {
	TIMERDBG("Cannot delete a deleted event");
	return 1;
    }

    timer_cancel(timerid);
    
    event->flags |= TFLAG_DELETED;

    event->next = event_freelist;
    event_freelist = event;

    return 0;
}

int timer_connect
(
    timer_t     timerid, /* timer ID */
    void (*routine)(timer_t, int), /* user routine */
    int         arg      /* user argument */
)
{
    struct event *event = (struct event *) timerid;

    assert(routine != NULL);
    event->func = routine;
    event->arg = arg;
    
    return 0;
}    

/* 
 * Please Call this function only from the call back functions of the alarm_handler.
 * This is just a hack 
*/
int timer_change_settime
(
    timer_t                   timerid, /* timer ID */
    const struct itimerspec * value   /* time to be set */
)
{
    struct event *event = (struct event *) timerid;

    TIMESPEC_TO_TIMEVAL(&event->it_interval, &value->it_interval);
    TIMESPEC_TO_TIMEVAL(&event->it_value, &value->it_value);

    return 1; 	
}

int timer_settime
(
    timer_t                   timerid, /* timer ID */
    int                       flags,   /* absolute or relative */
    const struct itimerspec * value,   /* time to be set */
    struct itimerspec *       ovalue   /* previous time set (NULL=no result) */
)
{
    struct itimerval itimer;
    struct event *event = (struct event *) timerid;
    struct event **ppevent;

    TIMESPEC_TO_TIMEVAL(&event->it_interval, &value->it_interval);
    TIMESPEC_TO_TIMEVAL(&event->it_value, &value->it_value);

    /* if .it_value is zero, the timer is disarmed */
    if (!timerisset(&event->it_value)) {
	timer_cancel(timerid);
	return 0;
    }

    block_timer();

#ifdef TIMER_PROFILE
    event->expected_ms = (event->it_value.tv_sec * MS_PER_SEC) + (event->it_value.tv_usec / US_PER_MS);
    event->start = uclock();
#endif
    if (event->next) {
	TIMERDBG("calling timer_settime with a timer that is already on the queue.");
    }


    /* We always want to make sure that the event at the head of the
       queue has a timeout greater than the itimer granularity.
       Otherwise we end up with the situation that the time remaining
       on an itimer is greater than the time at the head of the queue
       in the first place. */
    timerroundup(&event->it_value, g_granularity);

    timerclear(&itimer.it_value);
    getitimer(ITIMER_REAL, &itimer);
    if (timerisset(&itimer.it_value)) {
	// reset the top timer to have an interval equal to the remaining interval 
	// when the timer was cancelled.
	if (event_queue) {
	    if (timercmp(&(itimer.it_value), &(event_queue->it_value), >)) {
		// it is an error if the amount of time remaining is more than the amount of time 
		// requested by the top event.
		//
		TIMERDBG("timer_settime: TIMER ERROR!");

	    } else {
		// some portion of the top event has already expired.
		// Reset the interval of the top event to remaining
		// time left in that interval.
		//
		event_queue->it_value = itimer.it_value;

		// if we were the earliest timer before now, we are still the earliest timer now.
		// we do not need to reorder the list.
	    }
	}
    }

    // Now, march down the list, decrementing the new timer by the
    // current it_value of each event on the queue.
    ppevent = &event_queue;
    while (*ppevent) {
	if ( timercmp(&(event->it_value), &((*ppevent)->it_value), <) ) {
	    // if the proposed event will trigger sooner than the next event
	    // in the queue, we will insert the new event just before the next one.
	    //
	    // we also need to adjust the delta value to the next event.
	    timersub(&((*ppevent)->it_value), &(event->it_value), &((*ppevent)->it_value));
	    break;
	}
	// subtract the interval of the next event from the proposed interval.
	timersub(&(event->it_value), &((*ppevent)->it_value), &(event->it_value));

	ppevent = &((*ppevent)->next);
    }
    
    // we have found our proper place in the queue, 
    // link our new event into the pending event queue.
    event->next = *ppevent;
    *ppevent = event;

    check_event_queue();

    // if our new event ended up at the front of the queue, reissue the timer.
    if (event == event_queue) {
	timerroundup(&event_queue->it_value, g_granularity);
	timerclear(&itimer.it_interval);
	itimer.it_value = event_queue->it_value;
	
	// we want to be sure to never turn off the timer completely, 
	// so if the next interval is zero, set it to some small value.
	if (!timerisset(&(itimer.it_value)))
	    itimer.it_value = (struct timeval) { 0, 1 };
	
	assert(!timerisset(&itimer.it_interval));
	assert(itimer.it_value.tv_sec > 0 || itimer.it_value.tv_usec >= g_granularity);
	assert(event_queue->it_value.tv_sec > 0 || event_queue->it_value.tv_usec >= g_granularity);
	setitimer(ITIMER_REAL, &itimer, NULL);
	check_timer();
    }

    event->flags &= ~TFLAG_CANCELLED;
    
    unblock_timer();

    return 0;
}

static void check_timer()
{
    struct itimerval itimer;
    
    getitimer(ITIMER_REAL, &itimer);
    if (timerisset(&itimer.it_interval)) {
	TIMERDBG("ERROR timer interval is set.");
    }
    if (timercmp(&(itimer.it_value), &(event_queue->it_value), >)) {
	TIMERDBG("ERROR timer expires later than top event.");
    }
}


static void check_event_queue()
{
    struct timeval sum;
    struct event *event;
    int i = 0;

#ifdef notdef
    int nfree = 0;
    struct event *p;
    for (p = event_freelist; p; p = p->next)
	nfree++;
    printf("%d free events\n", nfree);
#endif
    
    timerclear(&sum);
    for (event = event_queue; event; event = event->next) {
	if (i > g_maxevents) {
	    TIMERDBG("timer queue looks like it loops back on itself!");
	    print_event_queue();
	    exit(1);
	}
	i++;
    }
}

#if THIS_FINDS_USE
/* The original upnp version has this unused function, so I left it in
   to maintain the resemblance. */
static int count_queue(struct event *event_queue)
{
    struct event *event;
    int i = 0;
    for (event = event_queue; event; event = event->next) 
	i++;
    return i;
}
#endif

static void print_event_queue()
{
    struct event *event;
    int i = 0;

    for (event = event_queue; event; event = event->next) {
	printf("#%d (0x%x)->0x%x: \t%d sec %d usec\t%p\n", 
	       i++, (unsigned int) event, (unsigned int) event->next, (int) event->it_value.tv_sec, (int) event->it_value.tv_usec, event->func);
	if (i > g_maxevents) {
	    printf("...(giving up)\n");
	    break;
	}
    }
}

// The top element of the event queue must have expired.
// Remove that element, run its function, and reset the timer.
// if there is no interval, recycle the event structure.
static void alarm_handler(int i)
{
    struct event *event, **ppevent;
    struct itimerval itimer;
    struct timeval small_interval = { 0, g_granularity/2 };
#ifdef TIMER_PROFILE
    uint junk;
    uclock_t end;
    uint actual;
#endif

    block_timer();

    // Loop through the event queue and remove the first event plus any 
    // subsequent events that will expire very soon thereafter (within 'small_interval'}.
    //
    do {
	// remove the top event.
	event = event_queue;
	event_queue = event_queue->next;
	event->next = NULL;

#ifdef TIMER_PROFILE
	end = uclock();
	actual = ((end-event->start)/((uclock_t)UCLOCKS_PER_SEC/1000));
	if (actual < 0)
	    junk = end;
	TIMERDBG("expected %d ms  actual %d ms", event->expected_ms, ((end-event->start)/((uclock_t)UCLOCKS_PER_SEC/1000)));
#endif
	
	    // call the event callback function
	    (*(event->func))((timer_t) event, (int)event->arg);

	/* If the event has been cancelled, do NOT put it back on the queue. */
	if ( !(event->flags & TFLAG_CANCELLED) ) {

	    // if the event is a recurring event, reset the timer and
	    // find its correct place in the sorted list of events.
	    //
	    if (timerisset(&event->it_interval)) {
		// event is recurring...
		//
		event->it_value = event->it_interval;
#ifdef TIMER_PROFILE
		event->expected_ms = (event->it_value.tv_sec * MS_PER_SEC) + (event->it_value.tv_usec / US_PER_MS);
		event->start = uclock();
#endif
		timerroundup(&event->it_value, g_granularity);

		// Now, march down the list, decrementing the new timer by the
		// current delta of each event on the queue.
		ppevent = &event_queue;
		while (*ppevent) {
		    if ( timercmp(&(event->it_value), &((*ppevent)->it_value), <) ) {
			// if the proposed event will trigger sooner than the next event
			// in the queue, we will insert the new event just before the next one.
			//
			// we also need to adjust the delta value to the next event.
			timersub(&((*ppevent)->it_value), &(event->it_value), &((*ppevent)->it_value));
			break;
		    }
		    timersub(&(event->it_value), &((*ppevent)->it_value), &(event->it_value));
		    ppevent = &((*ppevent)->next);
		}

		// we have found our proper place in the queue, 
		// link our new event into the pending event queue.
		event->next = *ppevent;
		*ppevent = event;
	    } else {
		// there is no interval, so recycle the event structure.
		//timer_delete((timer_t) event);
	    }
	}

	check_event_queue();
	
    } while (event_queue && timercmp(&event_queue->it_value, &small_interval, <));
    
    // re-issue the timer...
    if (event_queue) {
	timerroundup(&event_queue->it_value, g_granularity);

	timerclear(&itimer.it_interval);
	itimer.it_value = event_queue->it_value;
	// we want to be sure to never turn off the timer completely, 
	// so if the next interval is zero, set it to some small value.
	if (!timerisset(&(itimer.it_value)))
	    itimer.it_value = (struct timeval) { 0, 1 };

	setitimer(ITIMER_REAL, &itimer, NULL);
	check_timer();
    } else {
	TIMERDBG("There are no events in the queue - timer not reset.");
    }

    unblock_timer();
}

static int block_count = 0;

void block_timer()
{
    sigset_t set;

    if (block_count++ == 0) {
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	sigprocmask(SIG_BLOCK, &set, NULL);
    }
}

void unblock_timer()
{
    sigset_t set;

    if (--block_count == 0) {
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &set, NULL);
    }
}

void timer_cancel_all()
{
    struct itimerval timeroff = { { 0, 0 }, { 0, 0} };
    struct event *event;
    struct event **ppevent;

    setitimer(ITIMER_REAL, &timeroff, NULL);

    ppevent = &event_queue;
    while (*ppevent) {
	event = *ppevent;
	*ppevent = event->next;
	event->next = NULL;
    }
}



void timer_cancel(timer_t timerid)
{
    struct itimerval itimer;
    struct itimerval timeroff = { { 0, 0 }, { 0, 0} };
    struct event *event = (struct event *) timerid;
    struct event **ppevent;

    if (event->flags & TFLAG_CANCELLED) {
	TIMERDBG("Cannot cancel a cancelled event");
    	return;
    }

    block_timer();
    
    ppevent = &event_queue;
    while (*ppevent) {
	if ( *ppevent == event ) {

	    /* RACE CONDITION - if the alarm goes off while we are in
	       this loop, and if the timer we want to cancel is the
	       next to expire, the alarm will end up firing
	       after this routine is complete, causing it to go off early. */

	    /* If the cancelled timer is the next to expire, 
	       we need to do something special to clean up correctly. */
	    if (event == event_queue && event->next != NULL) {
		timerclear(&itimer.it_value);
		getitimer(ITIMER_REAL, &itimer);
		
		/* subtract the time that has already passed while waiting for this timer... */
		timersub(&(event->it_value), &(itimer.it_value), &(event->it_value));

		/* and add any remainder to the next timer in the list */
		timeradd(&(event->next->it_value), &(event->it_value), &(event->next->it_value));
	    }

	    *ppevent = event->next;
	    event->next = NULL;

	    if (event_queue) {
		timerroundup(&event_queue->it_value, g_granularity);
		timerclear(&itimer.it_interval);
		itimer.it_value = event_queue->it_value;
		
		/* We want to be sure to never turn off the timer
		   completely if there are more events on the queue,
		   so if the next interval is zero, set it to some
		   small value.  */

		if (!timerisset(&(itimer.it_value)))
		    itimer.it_value = (struct timeval) { 0, 1 };
		
		assert(itimer.it_value.tv_sec > 0 || itimer.it_value.tv_usec >= g_granularity);
		assert(event_queue->it_value.tv_sec > 0 || event_queue->it_value.tv_usec >= g_granularity);
		setitimer(ITIMER_REAL, &itimer, NULL);
		check_timer();
	    } else {
		setitimer(ITIMER_REAL, &timeroff, NULL);
	    }
	    break;
	}
	ppevent = &((*ppevent)->next);
    }

    event->flags |= TFLAG_CANCELLED;

    unblock_timer();
}

/*
* timer related headers
*/
#include "bcmtimer.h"

/*
* locally used global variables and constants
*/

/*
* Initialize internal resources used in the timer module. It must be called
* before any other timer function calls. The param 'timer_entries' is used
* to pre-allocate fixed number of timer entries.
*/
int bcm_timer_module_init(int timer_entries, bcm_timer_module_id *module_id)
{
	init_event_queue(timer_entries);
	*module_id = (bcm_timer_module_id)event_freelist;
	return 0;
}

/*
* Cleanup internal resources used by this timer module. It deletes all
* pending timer entries from the backend timer system as well.
*/
int bcm_timer_module_cleanup(bcm_timer_module_id module_id)
{
	module_id = 0;
	return 0;
}

/* Enable/Disable timer module */
int bcm_timer_module_enable(bcm_timer_module_id module_id, int enable)
{
	if (enable)
		unblock_timer();
	else
		block_timer();
	return 0;
}

int bcm_timer_create(bcm_timer_module_id module_id, bcm_timer_id *timer_id)
{
	module_id = 0;
	return timer_create(CLOCK_REALTIME, NULL, (timer_t *)timer_id);
}

int bcm_timer_delete(bcm_timer_id timer_id)
{
	return timer_delete((timer_t)timer_id);
}

int bcm_timer_gettime(bcm_timer_id timer_id, struct itimerspec *timer_spec)
{
	return -1;
}

int bcm_timer_settime(bcm_timer_id timer_id, const struct itimerspec *timer_spec)
{
	return timer_settime((timer_t)timer_id, 0, timer_spec, NULL);
}

int bcm_timer_connect(bcm_timer_id timer_id, bcm_timer_cb func, int data)
{
	return timer_connect((timer_t)timer_id, (void *)func, data);
}

int bcm_timer_cancel(bcm_timer_id timer_id)
{
	timer_cancel((timer_t)timer_id);
	return 0;
}
int bcm_timer_change_expirytime(bcm_timer_id timer_id, const struct itimerspec *timer_spec)
{
	timer_change_settime((timer_t)timer_id, timer_spec);
	return 1;
}
