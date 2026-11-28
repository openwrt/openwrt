#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <getopt.h>
#include <libubus.h>
#include <sys/time.h>
#include <sys/syscall.h>

#include "sf_genl_msg.h"
#include "private.h"

int should_exit = 0;
static sem_t sem_exit;
static struct ubus_context *ctx;

struct ubus_context *ubus_ctx_get(void)
{
        return ctx;
}

uint64_t milli_time_get(void)
{
        struct timespec t = { 0, 0 };
        syscall(SYS_clock_gettime, CLOCK_MONOTONIC, &t);
        return ((uint64_t)t.tv_sec) * 1000 + ((uint64_t)t.tv_nsec) / 1000000;
}

int is_gonna_exit(void)
{
        return should_exit;
}

void go_exit(void)
{
        should_exit = 1;
        uloop_end();
        sem_post(&sem_exit);
}

static void signal_handler(int sig_no)
{
        switch (sig_no) {
        case SIGTERM:
        case SIGKILL:
        case SIGINT:
                go_exit();
                break;
        }
}

int main(int argc, char *argv[])
{
        int err;

        sem_init(&sem_exit, 0, 0);

        ctx = ubus_connect(NULL);
        if (!ctx) {
                pr_err("%s(): ubus_connect() failed\n", __func__);
                return -EIO;
        }

        // ubus init
        uloop_init();
        signal(SIGPIPE, SIG_IGN);
        ubus_add_uloop(ctx);

        if ((err = sf_user_genl_init())) {
                pr_err("sf_user_genl_init() failed\n");
                goto out_ubus;
        }

     	nat_init();
	l2_mac_init();
	vlan_init();
	common_init();
        router_init();
        tmu_init();
        mcast_init();
        acl_init();

        signal(SIGKILL, signal_handler);
        signal(SIGTERM, signal_handler);
        signal(SIGINT,  signal_handler);

        uloop_run();

        sem_wait(&sem_exit);

	nat_exit();
	l2_mac_exit();
	vlan_exit();
	common_exit();
        router_exit();
        tmu_exit();
        mcast_exit();
        acl_exit();

        sf_user_genl_exit();
out_ubus:
        ubus_free(ctx);
        uloop_done();

        return err;
}
