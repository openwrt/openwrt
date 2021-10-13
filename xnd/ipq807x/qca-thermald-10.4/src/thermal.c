/*===========================================================================

  Copyright (c) 2010-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/


#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

#include "thermal.h"

#ifndef IPQ_806x
#include "thermal_server.h"
#endif


thermal_setting_t thermal_settings;
int num_cpus = 0;
int num_gpus = 0;
int exit_daemon = 0;
int debug_output = 0;
int enable_restore_brightness = 1;
int minimum_mode = 0;
int new_corelimit = 0;
int dump_bcl_ibat_imax = 0;
char *dump_bcl_ibat_imax_file = NULL;

static char *config_file = NULL;

void set_mitigation_level();

#ifdef IPQ_806x
void terminate_signal (int sig)
{

	/* Do all uninitialization here */

	/* Enabling NSS Auto scaling */
	nssfreq_request( -1 );

	/* Resetting the powerctl */
	powerctl_restart(1);

	/* Proceed with the termination */
	exit(sig);
}
#endif

void print_help(void)
{
	printf("\nTemperature sensor daemon\n");
	printf("Optional arguments:\n");
	printf("  --config-file/-c <file>        config file\n");
	printf("  --debug/-d                     debug output\n");
#ifndef IPQ_806x
	printf("  --norestorebrightness/-r       disable restore brightness functionality\n");
	printf("  --overridecorelimit/-o <degC>  override core limit temperature (test only)\n");
	printf("  --dump-bcl/-i                  BCL ibat/imax file\n");
#endif
	printf("  --help/-h                      this help screen\n");
}

int parse_commandline(int argc, char *const argv[])
{
	int c;

	struct option long_options[] = {
		{"config-file",         1, 0, 'c'},
		{"debug",               0, 0, 'd'},
#ifndef IPQ_806x
		{"norestorebrightness", 0, 0, 'r'},
		{"overridecorelimit",   1, 0, 'o'},
		{"dump-bcl",            2, 0, 'i'},
#endif
		{"help",                0, 0, 'h'},
		{0, 0, 0, 0}
	};

	while ((c = getopt_long(argc, argv, "c:i::dro:h", long_options, NULL)) != EOF) {
		switch (c) {
			case 'c':
				info("Using config file '%s'\n", optarg);
				config_file = optarg;
				break;
			case 'd':
                                info("Debug output enabled\n");
                                debug_output = 1;
                                break;

#ifndef IPQ_806x
			case 'i':
				info("dump BCL ibat/imax to a file\n");
				dump_bcl_ibat_imax = 1;
				dump_bcl_ibat_imax_file = optarg;
				break;
			case 'r':
				info("Restore brightness feature disabled\n");
				enable_restore_brightness = 0;
				break;
			case 'o':
				new_corelimit = atoi(optarg);
				/* sanity check */
				if (new_corelimit == 0) {
					printf("Invalid argument %s for core limit, value should be an "
					    "integral value in degC\n", optarg);
					return 0;
				}
				else if (new_corelimit < CORELIMIT_MIN ||
					 new_corelimit > CORELIMIT_MAX) {
					printf("Core limit %d out of sanity check range, "
					    "should be between %d and %ddegC\n",
					    new_corelimit, CORELIMIT_MIN, CORELIMIT_MAX);
					return 0;
				}

				info("Core limit override to %d\n", new_corelimit);
				break;
#endif
			case 'h':
			default:
				return 0;
		}
	}

	/* Too many/unrecognized argument(s) */
	if (optind < argc) {
		msg("Too many arguments\n");
		return 0;
	}

	return 1;
}

int main(int argc, char **argv)
{
#ifdef IPQ_806x
	if (signal (SIGTERM, terminate_signal) == SIG_IGN)
		signal (SIGTERM, SIG_IGN);
#endif

	info("Thermal daemon started\n");

	setpriority(PRIO_PROCESS, getpid(), -20);

	if (!parse_commandline(argc, argv)) {
		print_help();
		return 0;
	}

	init_settings(&thermal_settings);

	if (!load_config(&thermal_settings, config_file)) {
		return 0;
	}

#ifdef ANDROID
	{
		char buf[PROPERTY_VALUE_MAX];
		/* Early stage of encrypted data partition;
		 * run without modem mitigation and framework socket
		 */
		if ((property_get("vold.decrypt", buf, "0") > 0)
		    && (buf[0] == '1')) {
			minimum_mode = 1;
			info("Running in minimum mode\n");
		}
	}
#endif
	set_mitigation_level();
	cpufreq_init();

#ifndef IPQ_806x
	gpufreq_init();

	if (!minimum_mode) {
		modem_communication_init();
		modem_ts_qmi_init();
	}

	/* Initialize and setup a socket server for external
	   userspace apps interested in thermal mitigation. */
	thermal_server_init();

#endif
	/* Disable kernel thermal module and take over */
	write_to_file("/sys/module/msm_thermal/parameters/enabled", "N" , strlen("N") + 1);
	thermal_monitor(&thermal_settings);

#ifndef IPQ_806x
	if (!minimum_mode) {
		modem_communication_release();
		modem_qmi_ts_comm_release();
	}

	thermal_server_release();
#endif
	terminate_signal(0);
	info("Thermal daemon exited\n");
	return 0;
}
