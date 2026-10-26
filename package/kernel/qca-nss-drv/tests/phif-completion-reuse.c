#define _GNU_SOURCE
#include <unistd.h>
/*
 * Does a timed-out phys_if request corrupt the next one?
 *
 * Models nss_phys_if_msg_sync() and nss_phys_if_callback() from
 * qca-nss-drv/nss_phys_if.c exactly: one semaphore, one completion, one shared
 * response int, a 3s timeout, and a firmware that answers on its own schedule.
 * struct completion is modelled by its actual semantics - complete() increments
 * a counter, wait_for_completion_timeout() consumes one if present and
 * otherwise waits.
 *
 * Build: cc -O1 -pthread phif_test.c -o phif_test
 */
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define TIMEOUT_MS 300		/* NSS_PHYS_IF_TX_TIMEOUT, scaled 10x down */
#define NSS_TX_SUCCESS 0
#define NSS_TX_FAILURE 1

static bool fixed;		/* apply patch 0109 */

/* --- struct completion ---------------------------------------------------- */
struct completion {
	pthread_mutex_t lock;
	pthread_cond_t cond;
	unsigned done;
};

static void init_completion(struct completion *x)
{
	pthread_mutex_init(&x->lock, NULL);
	pthread_cond_init(&x->cond, NULL);
	x->done = 0;
}

static void reinit_completion(struct completion *x)
{
	pthread_mutex_lock(&x->lock);
	x->done = 0;
	pthread_mutex_unlock(&x->lock);
}

static void complete(struct completion *x)
{
	pthread_mutex_lock(&x->lock);
	x->done++;
	pthread_cond_signal(&x->cond);
	pthread_mutex_unlock(&x->lock);
}

static unsigned long now_ms(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000UL + tv.tv_usec / 1000;
}

/* returns remaining jiffies (nonzero) or 0 on timeout, like the kernel's */
static int wait_for_completion_timeout(struct completion *x, int ms)
{
	struct timespec ts;
	struct timeval tv;
	int rc = 0;

	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + (tv.tv_usec / 1000 + ms) / 1000;
	ts.tv_nsec = ((tv.tv_usec / 1000 + ms) % 1000) * 1000000L;

	pthread_mutex_lock(&x->lock);
	while (x->done == 0 && rc == 0)
		rc = pthread_cond_timedwait(&x->cond, &x->lock, &ts);
	if (x->done) {
		x->done--;
		pthread_mutex_unlock(&x->lock);
		return 1;
	}
	pthread_mutex_unlock(&x->lock);
	return 0;
}

/* --- the driver ----------------------------------------------------------- */
static struct nss_phys_if_pvt {
	pthread_mutex_t sem;
	struct completion complete;
	int response;
	unsigned long serial;	/* patch 0109 */
} phif;

/* a message in flight towards the firmware */
struct msg {
	unsigned long app_data;	/* serial, echoed back by the firmware */
	int ack;		/* what the firmware will answer */
	int delay_ms;		/* how long the firmware takes */
};

static void nss_phys_if_callback(unsigned long app_data, int ack)
{
	if (fixed && app_data != phif.serial)
		return;		/* stale: no longer the awaited request */

	phif.response = ack ? NSS_TX_SUCCESS : NSS_TX_FAILURE;
	complete(&phif.complete);
}

static void *firmware(void *arg)
{
	struct msg *m = arg;

	usleep(m->delay_ms * 1000);
	nss_phys_if_callback(m->app_data, m->ack);
	return NULL;
}

/* nss_phys_if_msg_sync() */
static int msg_sync(struct msg *m, unsigned long *elapsed)
{
	pthread_t fw;
	unsigned long t0;
	int status, ret;

	pthread_mutex_lock(&phif.sem);

	if (fixed) {
		phif.serial++;
		m->app_data = phif.serial;
		reinit_completion(&phif.complete);
	}

	t0 = now_ms();
	pthread_create(&fw, NULL, firmware, m);	/* request is queued and stays queued */
	pthread_detach(fw);

	ret = wait_for_completion_timeout(&phif.complete, TIMEOUT_MS);
	if (!ret)
		phif.response = NSS_TX_FAILURE;

	status = phif.response;
	*elapsed = now_ms() - t0;
	pthread_mutex_unlock(&phif.sem);
	return status;
}

static int run(bool with_fix)
{
	struct msg slow = { .ack = 1, .delay_ms = TIMEOUT_MS * 2 };  /* will time out */
	struct msg quick = { .ack = 0, .delay_ms = TIMEOUT_MS / 3 }; /* firmware NAKs it */
	unsigned long e1, e2;
	int r1, r2;

	fixed = with_fix;
	memset(&phif, 0, sizeof(phif));
	pthread_mutex_init(&phif.sem, NULL);
	init_completion(&phif.complete);

	/* 1: firmware is late; the sender gives up but the request stays queued */
	r1 = msg_sync(&slow, &e1);

	/* the late ACK lands here, with nobody waiting */
	usleep(TIMEOUT_MS * 1500);

	/* 2: a fresh request the firmware will REJECT */
	r2 = msg_sync(&quick, &e2);

	printf("  request 1 (times out):  status=%s  waited=%lums\n",
	       r1 == NSS_TX_SUCCESS ? "SUCCESS" : "FAILURE", e1);
	printf("  request 2 (firmware NAKs it): status=%s  waited=%lums\n",
	       r2 == NSS_TX_SUCCESS ? "SUCCESS" : "FAILURE", e2);
	return r2;
}

int main(void)
{
	int unpatched, patched;

	printf("unpatched (completion reused, no serial):\n");
	unpatched = run(false);
	printf("patched (0109):\n");
	patched = run(true);

	printf("\n");
	/*
	 * The firmware rejected request 2 in both runs. Unpatched, the sender
	 * is told it succeeded - it consumed the leftover token from request 1
	 * and read request 1's ACK - and it returns without waiting for its own
	 * answer at all. That is a control message reporting a success it never
	 * had, which for link_state means the host believes a port is up that
	 * the firmware never brought up.
	 */
	assert(unpatched == NSS_TX_SUCCESS &&
	       "unpatched: request 2 should wrongly report SUCCESS");
	assert(patched == NSS_TX_FAILURE &&
	       "patched: request 2 must report the firmware's actual rejection");
	printf("PASS: unpatched reports a success the firmware never gave;"
	       " patched reports the rejection.\n");
	return 0;
}
