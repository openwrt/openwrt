/*
 * Based on code found at https://dev.openwrt.org/ticket/4876 .
 * Extended by Jo-Philipp Wich <jo@mein.io> for use in OpenWrt.
 *
 * You may use this program under the terms of the GPLv2 license.
 */

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

/* This application returns zero on success and any
 * of the error codes below on error: */
#define ERROR_TIMEOUT 1
#define ERROR_IN_GETADDRINFO 2
#define ERROR_IN_ADDRESS 3
#define ERROR_IN_TIMER 4

static void abort_query(int sig)
{
	exit(ERROR_TIMEOUT);
}

static void show_usage(void)
{
	printf("Usage:\n");
	printf("	resolveip -h\n");
	printf("	resolveip [-t timeout] hostname\n");
	printf("	resolveip -4 [-t timeout] hostname\n");
	printf("	resolveip -6 [-t timeout] hostname\n");
	exit(255);
}

int main(int argc, char **argv)
{
	int timeout = 3;
	int opt, gret;
	char ipaddr[INET6_ADDRSTRLEN];
	void *addr;
	struct addrinfo *res, *rp;
	struct sigaction sa = {	.sa_handler = &abort_query };
	timer_t timerid;
	struct itimerspec its;
	struct timespec ts;
	struct sigevent sev;
	struct addrinfo hints = {
		.ai_family   = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = IPPROTO_TCP,
		.ai_flags    = 0
	};

	while ((opt = getopt(argc, argv, "46t:h")) > -1)
	{
		switch ((char)opt)
		{
			case '4':
				hints.ai_family = AF_INET;
				break;

			case '6':
				hints.ai_family = AF_INET6;
				break;

			case 't':
				timeout = atoi(optarg);
				if (timeout <= 0)
					show_usage();
				break;

			case 'h':
				show_usage();
				break;
		}
	}

	if (!argv[optind])
		show_usage();

	sigaction(SIGALRM, &sa, NULL);

	/* simulate alarm using the monotonic clock */
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGALRM;
	sev.sigev_value.sival_ptr = &timerid;
	if (timer_create(CLOCK_MONOTONIC, &sev, &timerid) == -1)
		exit(ERROR_IN_TIMER);

	/* start the timer */
	its.it_value.tv_sec = timeout;
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = 0;
	if (timer_settime(timerid, 0, &its, NULL) == -1)
		exit(ERROR_IN_TIMER);

	/* retry getaddrinfo on temporary failures */
	do
	{
		gret = getaddrinfo(argv[optind], NULL, &hints, &res);
		if (gret == EAI_AGAIN) {
			ts.tv_nsec = 0;
			ts.tv_sec = 1;
			nanosleep(&ts, NULL);
		}
	} while (gret == EAI_AGAIN);

	if (gret)
		exit(ERROR_IN_GETADDRINFO);

	/* disarm timer */
	its.it_value.tv_sec = 0;
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = 0;
	timer_settime(timerid, 0, &its, NULL);

	for (rp = res; rp != NULL; rp = rp->ai_next)
	{
		addr = (rp->ai_family == AF_INET)
			? (void *)&((struct sockaddr_in *)rp->ai_addr)->sin_addr
			: (void *)&((struct sockaddr_in6 *)rp->ai_addr)->sin6_addr
		;

		if (!inet_ntop(rp->ai_family, addr, ipaddr, INET6_ADDRSTRLEN - 1))
			exit(ERROR_IN_ADDRESS);

		printf("%s\n", ipaddr);
	}

	freeaddrinfo(res);
	exit(0);
}
