/*===========================================================================

  thermal_actions.c

  DESCRIPTION
  Thermal mitigation action implementations, excluding modem actions which
  are implemented in modem_mitigation_* files.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  If present, action _init functions (e.g. cpufreq_init) should be called
  before the action functions. Each action function should keep its own
  state to avoid undesired repeating of mitigation actions at the same
  mitigation level.

  Copyright (c) 2010-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/


#include <unistd.h>
#include <sys/reboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <poll.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <dirent.h>
#include "thermal.h"
#include "thermal_server.h"

#ifndef IPQ_806x
#include <cutils/uevent.h>
#endif

#define SENSOR(x)   (((x >= 0) && (x < SENSOR_IDX_MAX)) ? \
			sensor_names[x] : "unknown")
#define NUM_US_IN_MS        (1000)
#define CPU_BUF_MAX         (50)
#define HOTPLUG_BUF_MAX     (1024)

enum {
	MITIGATION_ENABLE = 0,
	MITIGATION_DISABLE = 1
};

int nss_mitigation_level = MITIGATION_ENABLE;
int fab_mitigation_level = MITIGATION_ENABLE;
int powerctl_mitigation_restart = MITIGATION_ENABLE;

#ifndef IPQ_806x
/*===========================================================================
FUNCTION report_action

Action function to report the crossing of a thermal threshold.
The threshold information is sent across a local socket, in the string
format "sensorname\ntemperature\ncurrent_threshold_level\nis_trigger".

ARGUMENTS
	requester - requesting sensor enum
	temperature - temperature reached
	level - zero-count threshold level
	is_trigger - true if threshold triggered, false if cleared

RETURN VALUE
	0 on success, negative on failure.
===========================================================================*/
int report_action(int requester, int temperature, int level, int is_trigger)
{
	char tempBuf[REPORT_MSG_MAX];

	if (requester < 0 ||
	    requester >= SENSOR_IDX_MAX) {
		msg("%s: Invalid requester %d", __func__, requester);
		return -1;
	}

	info("ACTION: REPORT - "
	    "Sensor '%s' - temperature %d, threshold level %d, is_trigger %s",
	    SENSOR(requester), temperature, level + 1,
	    is_trigger ? "true" : "false");

	snprintf(tempBuf, REPORT_MSG_MAX, "%s\n%d\n%d\n%s",
		 SENSOR(requester), temperature, level + 1,
		 is_trigger ? "true" : "false");

	/* best effort report, no one may be listening on socket */
	write_to_local_socket(UI_LOCALSOCKET_NAME, tempBuf,
			      strlen(tempBuf));

	return 0;
}
#endif

/*===========================================================================
FUNCTION shutdown_action

Action function to shutdown the target on crossing of thermal threshold.

ARGUMENTS
	requester - requesting sensor enum
	temperature - temperature reached
	delay - delay before shutdown in milliseconds

RETURN VALUE
	0 on success, -1 on failure with errno set appropriately.
===========================================================================*/
int shutdown_action(int requester, int temperature, int delay)
{
	static int shutdown_requested = 0;
	int ret = 0;

	if (requester < 0 ||
	    requester >= SENSOR_IDX_MAX) {
		msg("%s: Invalid requester %d", __func__, requester);
		return -1;
	}

	if (delay < 0) {
		msg("Invalid shutdown delay value %d", delay);
		return -1;
	}

	if (!shutdown_requested) {
		info("ACTION: SHUTDOWN - "
		    "Sensor '%s' reached temperature %d - shutdown requested "
		    "with %d millisecond delay\n",
		    SENSOR(requester), temperature, delay);

		usleep(delay * NUM_US_IN_MS);

		/* commit buffers to disk and shutdown */
		sync();

		if ( -1 != system("reboot &") ){
			msg("reboot command successful\n");
		}
		else{
			msg("reboot - failed.\n");
		}
		shutdown_requested = 1;
	}
	exit_daemon = 1;
	return ret;
}

#define USERSPACE		"userspace"
#define GOVERNOR_STR_MAX	128
#define FREQ_STR_MAX		32

static char freq_max[MAX_CPUS][FREQ_STR_MAX];
static char freq_cpu_min[MAX_CPUS][FREQ_STR_MAX];
static int freq_req[MAX_CPUS][SENSOR_IDX_MAX];

/* variable indicating last cpu max frequency requested per cpu,
   0 value also indicates maximum frequency requested */
static int prev_cpu_max[MAX_CPUS];
static int online[MAX_CPUS];
static pthread_mutex_t cpufreq_set_mtx = PTHREAD_MUTEX_INITIALIZER;
#ifndef IPQ_806x
static pthread_t hotplug_thread;
#endif

/*===========================================================================
FUNCTION cpufreq_set

Helper function to set the frequency of the cpu.

ARGUMENTS
	cpu - cpu id
	frequency - frequency in Hz.

RETURN VALUE
	1 on success, else on failure.
===========================================================================*/
int cpufreq_set(int cpu, int frequency)
{
	char freq_buf[FREQ_STR_MAX] = {0};
	int  freq_min = 0;
	char node_buf[MAX_PATH] = {0};

	if (!online[cpu]) {
		dbgmsg("CPU[%d] offline. Not setting requested frequency %d\n",
				cpu, frequency);
		return 0;
	}

	/* Lower the minimum frequency if necessary */
	snprintf(node_buf, MAX_PATH, CPU_SYSFS(FREQ_MIN_NODE), cpu);
	read_line_from_file(node_buf, freq_buf, FREQ_STR_MAX - 1);
	freq_min = strtol(freq_buf, NULL, 10);

	snprintf(freq_buf, FREQ_STR_MAX - 1, "%d", frequency);

	if (freq_min > frequency) {
		write_to_file(node_buf, freq_buf, strlen(freq_buf));
	}

	snprintf(node_buf, MAX_PATH, CPU_SYSFS(FREQ_MAX_NODE), cpu);
	if (write_to_file(node_buf, freq_buf, strlen(freq_buf)) > 0) {
		dbgmsg("CPU[%d] frequency limited to %d\n", cpu, frequency);
		return 1;
	}

	msg("Failed to set desired CPU[%d] frequency limit to %d\n",
			cpu, frequency);
	return 0;
}

#ifndef IPQ_806x
/*===========================================================================
FUNCTION do_hotplug

Function to restore cpu max frequency on CPU hotplug.
This function will run in a separate thread.

ARGUMENTS
	data - data pointer, cpu-id.

RETURN VALUE
	void * - not used.
===========================================================================*/
void *do_hotplug(void *data)
{
	int err = 0;
	int recv_bytes = 0;
	struct pollfd fds;
	int cpu;
	char buf[HOTPLUG_BUF_MAX] = {0};
	char cpu_online[CPU_BUF_MAX] = {0};
	char cpu_offline[CPU_BUF_MAX] = {0};
	char fmaxinfo_buf[MAX_PATH] = {0};
	char freqmax_buf[MAX_PATH] = {0};

	/* Looking for online@/devices/system/cpu/cpu or
	 * offline@/devices/system/cpu/cpu */

	snprintf(cpu_online, CPU_BUF_MAX, "online@/devices/system/cpu/cpu");
	snprintf(cpu_offline, CPU_BUF_MAX, "offline@/devices/system/cpu/cpu");

	fds.events = POLLIN;
	fds.fd = uevent_open_socket(64*1024, true);
	if (fds.fd == -1) {
		msg("Error opening socket for hotplug uevent.\n");
		return NULL;
	}
	while (1) {

		err = poll(&fds, 1, -1);
		if (err == -1) {
			msg("Error in hotplug CPU poll.\n");
			break;
		}

		recv_bytes = uevent_kernel_multicast_recv(fds.fd, buf, sizeof(buf));
		if (recv_bytes == -1)
			continue;
		if (recv_bytes >= HOTPLUG_BUF_MAX)
			buf[HOTPLUG_BUF_MAX - 1] = '\0';
		else
			buf[recv_bytes] = '\0';

		if (NULL != strstr(buf, cpu_online)) {
			err = sscanf(buf, "online@/devices/system/cpu/cpu%d",
				     &cpu);
			if (err != 1 || cpu >= num_cpus) {
				msg("Unexpected hotplug notification "
				    "err:%d, cpu:%d, buf:\"%s\"", err, cpu, buf);
				continue;
			}
			pthread_mutex_lock(&cpufreq_set_mtx);
			online[cpu] = 1;
		} else if (NULL != strstr(buf, cpu_offline)) {
			err = sscanf(buf, "offline@/devices/system/cpu/cpu%d",
				     &cpu);
			if (err != 1 || cpu >= num_cpus) {
				msg("Unexpected offline hotplug notification "
				    "err:%d, cpu:%d, buf:\"%s\"", err, cpu, buf);
				continue;
			}
			pthread_mutex_lock(&cpufreq_set_mtx);
			online[cpu] = 0;
		} else
			continue;

		snprintf(fmaxinfo_buf, MAX_PATH, CPU_SYSFS(FMAX_INFO_NODE), cpu);
		snprintf(freqmax_buf, MAX_PATH, CPU_SYSFS(FREQ_MAX_NODE), cpu);

		if (online[cpu]) {
			char cur_max[FREQ_STR_MAX] = {0};
			int max_freq = strtol(freq_max[cpu], NULL, 10);

			/* The cpu was probably offline when thermald
			* started. Update freq_max for the cpu.
			*/
			if (!max_freq &&
				read_line_from_file(fmaxinfo_buf,
						    freq_max[cpu],
						    FREQ_STR_MAX) > 0) {
				max_freq = strtol(freq_max[cpu], NULL, 10);
				info("Maximum CPU[%d] frequency %d KHz\n",
						cpu, max_freq);
			}

			if (!prev_cpu_max[cpu])
				prev_cpu_max[cpu] = max_freq;

			if (read_line_from_file(freqmax_buf,
					cur_max, FREQ_STR_MAX) <= 0)
				msg("Unable to read max frequency from "
						"online cpu.\n");

			if (prev_cpu_max[cpu] != strtol(cur_max, NULL, 10)) {
				dbgmsg("CPU[%d] online. "
					"Setting max frequency to %d\n", cpu,
					prev_cpu_max[cpu]);
				cpufreq_set(cpu, prev_cpu_max[cpu]);
			}
		} else {
			dbgmsg("CPU[%d] offline\n", cpu);
		}
		pthread_mutex_unlock(&cpufreq_set_mtx);
	}
	return NULL;
}
#endif

static unsigned int get_num_cpus(void)
{
	DIR *tdir = NULL;
	int ncpus = 0;
	char cwd[MAX_PATH] = {0};
	int cpu = 0;
	struct dirent *cpu_dirent;

	if (!getcwd(cwd, sizeof(cwd)))
		return -1;

	/* Change dir to read the entries */
	if (chdir(CPU_SYSFS_DIR) != 0) {
		msg("Change directory failed: %s\n",
			CPU_SYSFS_DIR);
		return -1;
	}

	tdir = opendir(CPU_SYSFS_DIR);
	if (!tdir) {
		msg("Unable to open %s\n", CPU_SYSFS_DIR);
		return -1;
	}

	while ((cpu_dirent = readdir(tdir))) {
		if (!strstr(cpu_dirent->d_name, "cpu"))
			continue;
		if (!sscanf(cpu_dirent->d_name, "cpu%d", &cpu))
			continue;
		ncpus++;
	}

	closedir(tdir);
	if (chdir(cwd) != 0) { /* Restore current working dir */
		msg("Restore directory failed: %s\n", cwd);
		return -1;
	}

	dbgmsg("Number of CPU cores %d\n", ncpus);

	return ncpus;
}

/*===========================================================================
FUNCTION cpufreq_init

Initialization function for cpufreq.
Needs to be called before cpufreq_request() can be used.

ARGUMENTS
	None.

RETURN VALUE
	1 on success, else on failure.
===========================================================================*/
int cpufreq_init()
{
	int i;
	int cpu;
	char finfo_buf[MAX_PATH] = {0};

        num_cpus = get_num_cpus();
	if (num_cpus == -1) {
		msg("get_num_cpus() failed, default to MAX_CPUs %d", MAX_CPUS);
		num_cpus = MAX_CPUS;
	}
	if (num_cpus > MAX_CPUS) {
		msg("BUG: num_cpus > MAX_CPUS");
		return -1;
	}
	info("Number of cpus :%d\n", num_cpus);
	memset(online, 0, MAX_CPUS);

	for (cpu = 0; cpu < num_cpus; cpu++) {
		snprintf(finfo_buf, MAX_PATH, CPU_SYSFS(FMAX_INFO_NODE), cpu);
		/* Store max CPU frequency */
		if (read_line_from_file(finfo_buf,
					freq_max[cpu], FREQ_STR_MAX) > 0) {
			dbgmsg("Maximum CPU[%d] frequency %ld KHz\n", cpu,
					strtol(freq_max[cpu], NULL, 10));
			online[cpu] = 1;

		}
		else {
			info("Could not read max freq for CPU[%d]\n", cpu);
		}

		snprintf(finfo_buf, MAX_PATH, CPU_SYSFS(FMIN_INFO_NODE), cpu);
		/* Store min CPU frequency */
                if (read_line_from_file(finfo_buf,
                                        freq_cpu_min[cpu], FREQ_STR_MAX) > 0) {
                        dbgmsg("Minimum CPU[%d] frequency %ld KHz\n", cpu,
                                        strtol(freq_cpu_min[cpu], NULL, 10));
                        online[cpu] = 1;

                }
                else {
                        info("Could not read min freq for CPU[%d]\n", cpu);
                }

		for (i = 0; i < SENSOR_IDX_MAX; i++)
			freq_req[cpu][i] = -1;
	}

#ifndef IPQ_806x
	pthread_create(&hotplug_thread, NULL,
		       do_hotplug, NULL);
#endif


	return 1;
}

#ifdef IPQ_806x
/*===========================================================================
FUNCTION powerctl_restart

This is a utility function reset all the power controls and frequecy to the default state.

ARGUMENTS
        reset_max - 0 = Do not reset max freq
		    1 = Reset max freq

RETURN VALUE
        -1 ===> Failure.
	0  ===> Success.
===========================================================================*/

int powerctl_restart(int reset_max)
{
	int ret = 0;
	int cpu;
	int max_freq;
	char path[MAX_PATH] = {0};

	if (MITIGATION_ENABLE == powerctl_mitigation_restart){
		snprintf(path, MAX_PATH, "%s restart", POWER_CTL_PATH);
		if ( -1 != system(path) ){
			msg("powerctl command successful\n");
		}
		else{
			msg("powerctl - failed.\n");
		}
	}

	if (reset_max) {
		/* Setting Max freq to each CPU */
		for( cpu = 0; cpu < num_cpus; cpu++ ){
			max_freq = strtol(freq_max[cpu], NULL, 10);
			cpufreq_set(cpu , max_freq);
		}
	}
	return ret;
}

#endif

/*===========================================================================
FUNCTION mitigate_fabric_clks

This is a utility function to scale down or up the fabrics and NSS when CPU
 is at the minimum frequency.

ARGUMENTS
	mitigate - 1=scale down 0=defaults

RETURN VALUE
        void.
===========================================================================*/

#define MITIGATE_FAB_CLKS	1

#define APPS_FAB_NOMINAL	400000000
#define DFAB_NOMINAL		64000000
#define SFAB_NOMINAL		64000000
#define CFAB_NOMINAL		64000000
#define DFAB_IDLE	        32000000
#define SFAB_IDLE	        32000000
#define CFAB_IDLE            	32000000
#define NSS_FAB_IDLE		133000000
#define NSS_CLK_LOW		110000 /* KHz */

void mitigate_fabric_clks(int mitigate)
{
	char freq_buf[FREQ_STR_MAX] = {0};
	char freq_path[MAX_PATH] = {0};
	int afab_a_clk = APPS_FAB_NOMINAL;
	int dfab_a_clk = DFAB_NOMINAL;
	int sfpb_a_clk = SFAB_NOMINAL;
	int cfpb_a_clk = CFAB_NOMINAL;
	int nssfab0_a_clk = NSS_FAB_IDLE;
	int nssfab1_a_clk = NSS_FAB_IDLE;

	if (MITIGATION_DISABLE == fab_mitigation_level)
		return;

	if (1 == mitigate){
		dfab_a_clk = DFAB_IDLE;
		sfpb_a_clk = SFAB_IDLE;
		cfpb_a_clk = CFAB_IDLE;
		nssfreq_request( NSS_CLK_LOW );
	}
	else{
		nssfreq_request(-1);
	}

	snprintf(freq_buf, FREQ_STR_MAX, "%d", afab_a_clk);
	snprintf(freq_path, MAX_PATH, "%s", AFAB_CLK);
	if (write_to_file(freq_path, freq_buf, strlen(freq_buf)) > 0) {
		info("ACTION: CPU - ""Setting fabric to %s Hz at %s\n", freq_buf, freq_path);
	}
	else{
		msg("%s: Failed to set fabric freq to %s Hz at %s\n.", __func__, freq_buf, freq_path);
	}

	snprintf(freq_buf, FREQ_STR_MAX, "%d", dfab_a_clk);
        snprintf(freq_path, MAX_PATH, "%s", DFAB_CLK);
        if (write_to_file(freq_path, freq_buf, strlen(freq_buf)) > 0) {
                info("ACTION: CPU - ""Setting fabric to %s Hz at %s\n", freq_buf, freq_path);
        }
        else{
                msg("%s: Failed to set fabric freq to %s Hz at %s\n.", __func__, freq_buf, freq_path);
        }

	snprintf(freq_buf, FREQ_STR_MAX, "%d", sfpb_a_clk);
        snprintf(freq_path, MAX_PATH, "%s", SFPB_CLK);
        if (write_to_file(freq_path, freq_buf, strlen(freq_buf)) > 0) {
                info("ACTION: CPU - ""Setting fabric to %s Hz at %s\n", freq_buf, freq_path);
        }
        else{
                msg("%s: Failed to set freq to %s Hz at %s\n.", __func__, freq_buf, freq_path);
        }

	snprintf(freq_buf, FREQ_STR_MAX, "%d", cfpb_a_clk);
        snprintf(freq_path, MAX_PATH, "%s", CFPB_CLK);
        if (write_to_file(freq_path, freq_buf, strlen(freq_buf)) > 0) {
                info("ACTION: CPU - ""Setting fabric to %s Hz at %s\n", freq_buf, freq_path);
        }
        else{
                msg("%s: Failed to set fabric freq to %s Hz at %s\n.", __func__, freq_buf, freq_path);
        }

	snprintf(freq_buf, FREQ_STR_MAX, "%d", nssfab0_a_clk);
        snprintf(freq_path, MAX_PATH, "%s", NSSFAB0_CLK);
        if (write_to_file(freq_path, freq_buf, strlen(freq_buf)) > 0) {
                info("ACTION: CPU - ""Setting fabric to %s Hz at %s\n", freq_buf, freq_path);
        }
        else{
                msg("%s: Failed to set fabric freq to %s Hz at %s\n.", __func__, freq_buf, freq_path);
        }

	snprintf(freq_buf, FREQ_STR_MAX, "%d", nssfab1_a_clk);
        snprintf(freq_path, MAX_PATH, "%s", NSSFAB1_CLK);
        if (write_to_file(freq_path, freq_buf, strlen(freq_buf)) > 0) {
                info("ACTION: CPU - ""Setting fabric to %s Hz at %s\n", freq_buf, freq_path);
        }
        else{
                msg("%s: Failed to set fabric freq to %s Hz at %s\n.", __func__, freq_buf, freq_path);
        }
}
/*===========================================================================
FUNCTION cpufreq_request

Action function to request CPU frequency scaling.

ARGUMENTS
	cpu - cpu id
	requester - requesting sensor enum
	temperature - temperature reached
	frequency - cpu frequency to scale to, -1 for maximum cpu frequency

RETURN VALUE
	1 on success, else on failure.
===========================================================================*/
#define CPU_AT_MIN_FREQ		1
int cpu_at_min_freq = !CPU_AT_MIN_FREQ;

int cpufreq_request(int cpu, int requester, int temperature, int frequency)
{
	int i;
	int max_freq = strtol(freq_max[cpu], NULL, 10);
	int min_freq = strtol(freq_cpu_min[cpu], NULL, 10);

	if (requester < 0 ||
	    requester >= SENSOR_IDX_MAX) {
		msg("%s: Invalid requester %d", __func__, requester);
		return 0;
	}

	pthread_mutex_lock(&cpufreq_set_mtx);

	freq_req[cpu][requester] = frequency;

	if (!prev_cpu_max[cpu])
		prev_cpu_max[cpu] = max_freq;

	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		if ((freq_req[cpu][i] > 0) && (freq_req[cpu][i] < max_freq))
			max_freq = freq_req[cpu][i];
	}
	/* CPU was offline on thermald start, save frequency */
	if (max_freq == 0) {
		max_freq = (frequency == -1) ? 0 : frequency;
	}

	if (max_freq != prev_cpu_max[cpu]) {
		info("ACTION: CPU - "
		    "Setting CPU[%d] to %d\n", cpu, max_freq);
		cpufreq_set(cpu, max_freq);
#ifdef IPQ_806x
		if( (CPU_AT_MIN_FREQ == cpu_at_min_freq)
			&& (max_freq > prev_cpu_max[cpu])
			&& (cpu == (num_cpus - 1))){
			dbgmsg("%s: Resetting CPU freq setting to defaults\n", __func__);
			powerctl_restart(0);// Reset scaling governor freq to the default value
			cpu_at_min_freq = !CPU_AT_MIN_FREQ;
		}
		if (0 == cpu){/* Do it once for one CPU */
			if( max_freq == min_freq ){
				cpu_at_min_freq = CPU_AT_MIN_FREQ;
				mitigate_fabric_clks(MITIGATE_FAB_CLKS);
			}
			else{
				mitigate_fabric_clks(!MITIGATE_FAB_CLKS);
			}
		}
		prev_cpu_max[cpu] = max_freq;
#endif
	} else {
		dbgmsg("Using previous CPU[%d] frequency.\n", cpu);
	}

	pthread_mutex_unlock(&cpufreq_set_mtx);
	return 1;
}


#ifndef IPQ_806x

/* LCD Backlight brightness control action */

#define LCD_MAX_BRIGHTNESS "/sys/class/leds/lcd-backlight/max_brightness"
#define LCD_CURR_BRIGHTNESS "/sys/class/leds/lcd-backlight/brightness"
static int brightness_req[SENSOR_IDX_MAX];
static pthread_mutex_t lcd_mtx = PTHREAD_MUTEX_INITIALIZER;

/*===========================================================================
LOCAL FUNCTION restore_lcd_brightness

Helper function to restore LCD brightness.

ARGUMENTS
	value - 1-255 value of max LCD brightness, -1 for no throttling

RETURN VALUE
	None.
===========================================================================*/
static void restore_lcd_brightness(int restore_brightness,
				  int current_brightness)
{
	char buf[UINT_BUF_MAX] = {0};

	/* Don't restore if LCD is currently off */
	if (restore_brightness > 0 && current_brightness > 0) {
		snprintf(buf, UINT_BUF_MAX, "%d",
			 restore_brightness);
		if (write_to_file(LCD_CURR_BRIGHTNESS, buf,
				  strlen(buf)) > 0) {
			info("Setting curr LCD brightness to %d\n",
			    restore_brightness);
		} else {
			msg("Unable to set LCD brightness to %d\n",
			    restore_brightness);
		}
	}
}

/*===========================================================================
FUNCTION lcd_brightness_request

Action function to request LCD brightness throttling.

ARGUMENTS
	requester - requesting sensor enum
	temperature - temperature reached
	value - 1-255 value of max LCD brightness, -1 for no throttling

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int lcd_brightness_request(int requester, int temperature, int value)
{
	char buf[UINT_BUF_MAX] = {0};
	int i = 0;
	static int prev;
	static int in_mitigation = 0;
	static int restore_brightness = -1;
	int current_brightness = -1;
	char * end_ptr;

	if (requester < 0 ||
	    requester >= SENSOR_IDX_MAX) {
		msg("%s: Invalid requester %d", __func__, requester);
		return -1;
	}
	if (value > 255 ||
	    value < -1 ||
	    value == 0) {
		msg("Incorrect value %d value for LCD brightness\n", value);
		return -1;
	}

	if (value == -1) {
		value = 255;
	}

	pthread_mutex_lock(&lcd_mtx);

	/* get current brightness */
	if (read_line_from_file(LCD_CURR_BRIGHTNESS, buf, UINT_BUF_MAX) > 0) {
		current_brightness = strtol(buf, &end_ptr, 10);

		if (current_brightness == 0 &&
		    (end_ptr == buf || *end_ptr != '\n')) {
			dbgmsg("Failed to parse current brightness");
			current_brightness = -1;
		} else if (current_brightness < 0 ||
			   current_brightness > 255) {
			dbgmsg("Current brightness read invalid, value %d",
			       current_brightness);
			current_brightness = -1;
		} else if (in_mitigation == 0 && value < 255) {
			/* if entering mitigation, save current brightness
			   unless LCD is off */
			if (current_brightness == 0) {
				dbgmsg("LCD off at start of mitigation, "
				       "do not save restore value");
			} else {
				restore_brightness = current_brightness;
				dbgmsg("Brightness at start of mitigation is %d\n",
				       current_brightness);
			}
		}
	} else {
		msg("Error reading current brightness.\n");
	}

	brightness_req[requester] = value;

	/* Get the min of all the sensor requests */
	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		if ((brightness_req[i] > 0) && (brightness_req[i] < value))
			value = brightness_req[i];
	}

	if (value == 255) {
		in_mitigation = 0;
	} else {
		in_mitigation = 1;
	}

	if (prev != value) {
		snprintf(buf, UINT_BUF_MAX, "%d", value);
		if (write_to_file(LCD_MAX_BRIGHTNESS, buf, strlen(buf)) > 0) {
			info("ACTION: LCD - "
			    "Setting max LCD brightness to %d\n", value);
			prev = value;

			if (enable_restore_brightness)
				restore_lcd_brightness(restore_brightness,
						       current_brightness);
		} else {
			msg("Unable to set max LCD brightness to %d\n", value);
		}

		if (in_mitigation == 0) {
			restore_brightness = -1;
		}
	}

	pthread_mutex_unlock(&lcd_mtx);
	return 0;
}

static int battery_req[SENSOR_IDX_MAX];
static pthread_mutex_t battery_mtx = PTHREAD_MUTEX_INITIALIZER;
#define MAX_BATTERY_MITIGATION_LEVEL  (3)

#if SENSORS_8960
#define BATTERY_MITIGATION_SYSFS "/sys/module/pm8921_charger/parameters/thermal_mitigation"
#else
#define BATTERY_MITIGATION_SYSFS NULL
#endif

/*===========================================================================
FUNCTION battery_request

Action function to request battery charging current throttling action.

ARGUMENTS
	requester - requesting sensor enum
	temperature - temperature reached
	level - 0 - MAX_BATTERY_MITIGATION_LEVEL throttling level for
	        battery charging current

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int battery_request(int requester, int temperature, int level)
{
	int ret = -1;
	int i = 0;
	char buf[UINT_BUF_MAX] = {0};
	static int prev;

	if (requester < 0 ||
	    requester >= SENSOR_IDX_MAX) {
		msg("%s: Invalid requester %d", __func__, requester);
		return -1;
	}
	if (NULL == BATTERY_MITIGATION_SYSFS) {
		msg("%s: Unsupported action on current target", __func__);
		return -1;
	}

	if (level < 0)
		level = 0;

	if (level > MAX_BATTERY_MITIGATION_LEVEL)
		level = MAX_BATTERY_MITIGATION_LEVEL;

	pthread_mutex_lock(&battery_mtx);

	/* Aggregate battery charging level for all sensors */
	battery_req[requester] = level;
	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		if (battery_req[i] > level)
			level = battery_req[i];
	}

	if (level != prev) {
		snprintf(buf, UINT_BUF_MAX, "%d", level);
		if (write_to_file(BATTERY_MITIGATION_SYSFS, buf, strlen(buf)) > 0) {
			info("ACTION: BATTERY - "
			    "Setting battery charging mitigation to %d\n", level);
			prev = level;
			ret = 0;
		} else {
			msg("Unable to set battery charging mitigation to %d\n", level);
		}
	} else {
		dbgmsg("Battery charging mitigation already at level %d\n", level);
		ret = 0;
	}

	pthread_mutex_unlock(&battery_mtx);
	return ret;
}

#define MAX_GPUS              (4)       /* Supporting up to 4 core systems */
#define MAX_GPU_FREQ		400000000
#define GPU_FREQ_STR_MAX		32

static char gpu_freq_max[MAX_GPUS][GPU_FREQ_STR_MAX];
static int  gpu_freq_req[MAX_GPUS][SENSOR_IDX_MAX];

/* variable indicating last cpu max frequency requested per gpu,
   0 value also indicates maximum frequency requested */
static int prev_gpu_max[MAX_GPUS];
static pthread_mutex_t gpufreq_set_mtx = PTHREAD_MUTEX_INITIALIZER;

/*===========================================================================
FUNCTION gpufreq_set

Helper function to set the frequency of the GPU.

ARGUMENTS
	GPU - gpu id
	frequency - frequency in Hz.

RETURN VALUE
	0 on success, else on failure.
===========================================================================*/
int gpufreq_set(int gpu, int frequency)
{
	char freq_buf[GPU_FREQ_STR_MAX] = {0};
	char node_buf[MAX_PATH] = {0};

	snprintf(freq_buf, GPU_FREQ_STR_MAX - 1, "%d", frequency);

	/* Write out the max frequency */
	snprintf(node_buf, MAX_PATH, GPU_SYSFS(GPU_FREQ_MAX_NODE), gpu);

	if (write_to_file(node_buf, freq_buf, strlen(freq_buf)) > 0) {
		dbgmsg("GPU[%d] frequency limited to %d\n", gpu, frequency);
		return 0;
	}

	msg("Failed to set desired GPU[%d] frequency limit to %d\n",
			gpu, frequency);
	return 1;
}

static unsigned int get_num_gpus(void)
{
	DIR *tdir = NULL;
	int ngpus = 0;
	char cwd[MAX_PATH] = {0};
	int gpu = 0;
	struct dirent *gpu_dirent;

	if (!getcwd(cwd, sizeof(cwd)))
		return -1;

	/* Change dir to read the entries */
	if (chdir(GPU_SYSFS_DIR) != 0) {
		msg("Change directory failed: %s\n",
			GPU_SYSFS_DIR);
		return -1;
	}

	tdir = opendir(GPU_SYSFS_DIR);
	if (!tdir) {
		msg("Unable to open %s\n", GPU_SYSFS_DIR);
		return -1;
	}

	while ((gpu_dirent = readdir(tdir))) {
		if (!strstr(gpu_dirent->d_name, "kgsl-3d"))
			continue;
		if (!sscanf(gpu_dirent->d_name, "kgsl-3d%d", &gpu))
			continue;
		ngpus++;
	}

	closedir(tdir);
	if (chdir(cwd) != 0) { /* Restore current working dir */
		msg("Restore directory failed: %s\n", cwd);
		return -1;
	}

	dbgmsg("Number of GPU cores %d\n", ngpus);

	return ngpus;
}

/*===========================================================================
FUNCTION gpufreq_init

Initialization function for gpufreq.
Needs to be called before gpufreq_request() can be used.

ARGUMENTS
	None.

RETURN VALUE
	1 on success, else on failure.
===========================================================================*/
int gpufreq_init()
{
	int i;
	int gpu;
	char fmaxinfo_buf[MAX_PATH] = {0};

        num_gpus = get_num_gpus();
	if (num_gpus == -1) {
		msg("get_num_gpus() failed, default to MAX_GPUs %d", MAX_GPUS);
		num_gpus = MAX_GPUS;
	}
	if (num_gpus > MAX_GPUS) {
		msg("BUG: num_gpus > MAX_GPUS");
		return -1;
	}
	info("Number of gpus :%d\n", num_gpus);

	for (gpu = 0; gpu < num_gpus; gpu++) {

#if 0
		snprintf(fmaxinfo_buf, MAX_PATH, GPU_SYSFS(GPU_FREQ_MAX_NODE), gpu);
		/* Store max GPU frequency */
		if (read_line_from_file(fmaxinfo_buf,
					gpu_freq_max[gpu], GPU_FREQ_STR_MAX) > 0) {
			dbgmsg("Maximum GPU[%d] frequency %ld Hz\n", gpu,
					strtol(gpu_freq_max[gpu], NULL, 10));
		}
		else {
			info("GPUFreq not enabled for GPU[%d]\n", gpu);
		}
#endif
		snprintf(gpu_freq_max[gpu], GPU_FREQ_STR_MAX - 1, "%d", MAX_GPU_FREQ);
		for (i = 0; i < SENSOR_IDX_MAX; i++)
			gpu_freq_req[gpu][i] = -1;
	}

	return 1;
}

/*===========================================================================
FUNCTION gpufreq_request

Action function to request GPU frequency scaling.

ARGUMENTS
	gpu - gpu id
	requester - requesting sensor enum
	temperature - temperature reached
	frequency - gpu frequency to scale to, -1 for maximum gpu frequency

RETURN VALUE
	1 on success, else on failure.
===========================================================================*/
int gpufreq_request(int gpu, int requester, int temperature, int frequency)
{
	int i;
	int max_freq;

	if (gpu >= num_gpus) {
		msg("gpufreq_request() Invalid GPU ID %d", gpu);
		return 0;
	}
	if (requester < 0 ||
	    requester >= SENSOR_IDX_MAX) {
		msg("%s: Invalid requester %d", __func__, requester);
		return 0;
	}

	max_freq = strtol(gpu_freq_max[gpu], NULL, 10);

	pthread_mutex_lock(&gpufreq_set_mtx);

	gpu_freq_req[gpu][requester] = frequency;

	if (!prev_gpu_max[gpu])
		prev_gpu_max[gpu] = max_freq;

	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		if ((gpu_freq_req[gpu][i] > 0) && (gpu_freq_req[gpu][i] < max_freq))
			max_freq = gpu_freq_req[gpu][i];
	}

	if (max_freq != prev_gpu_max[gpu]) {
		info("ACTION: GPU - "
		    "Setting GPU[%d] to %d\n", gpu, max_freq);
		gpufreq_set(gpu, max_freq);
		prev_gpu_max[gpu] = max_freq;
	} else {
		dbgmsg("Using previous GPU[%d] frequency.\n", gpu);
	}

	pthread_mutex_unlock(&gpufreq_set_mtx);
	return 1;
}

static int wlan_req[SENSOR_IDX_MAX];
static pthread_mutex_t wlan_mtx = PTHREAD_MUTEX_INITIALIZER;
#define MAX_WLAN_MITIGATION_LEVEL  (4)

#if SENSORS_8960
#define WLAN_MITIGATION_SYSFS "/sys/devices/platform/wcnss_wlan.0/thermal_mitigation"
#else
#define WLAN_MITIGATION_SYSFS NULL
#endif

/*===========================================================================
FUNCTION wlan_request

Action function to request wlan throttling action.

ARGUMENTS
	requester - requesting sensor enum
	temperature - temperature reached
	level - 0 - MAX_WLAN_MITIGATION_LEVEL throttling level

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int wlan_request(int requester, int temperature, int level)
{
	int ret = -1;
	int i = 0;
	char buf[UINT_BUF_MAX] = {0};
	static int prev;

	if (NULL == WLAN_MITIGATION_SYSFS) {
		msg("%s: Unsupported action on current target", __func__);
		return -1;
	}
	if (requester < 0 ||
	    requester >= SENSOR_IDX_MAX) {
		msg("%s: Invalid requester %d", __func__, requester);
		return -1;
	}

	if (level < 0)
		level = 0;

	if (level > MAX_WLAN_MITIGATION_LEVEL)
		level = MAX_WLAN_MITIGATION_LEVEL;

	pthread_mutex_lock(&wlan_mtx);

	/* Aggregate wlan throttling level for all sensors */
	wlan_req[requester] = level;
	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		if (wlan_req[i] > level)
			level = wlan_req[i];
	}

	if (level != prev) {
		snprintf(buf, UINT_BUF_MAX, "%d", level);
		if (write_to_file(WLAN_MITIGATION_SYSFS, buf, strlen(buf)) > 0) {
			info("ACTION: WLAN - "
			    "Setting WLAN mitigation to %d\n", level);
			prev = level;
			ret = 0;
		} else {
			msg("Unable to set WLAN mitigation to %d\n", level);
		}
	} else {
		dbgmsg("WLAN mitigation already at level %d\n", level);
		ret = 0;
	}

	pthread_mutex_unlock(&wlan_mtx);
	return ret;
}

#define MAX_CAMERA_MITIGATION_LEVEL  (3)
#define MAX_CAMCORDER_MITIGATION_LEVEL  (3)
/*===========================================================================
FUNCTION camera_request

Action function to request camera throttling action.

ARGUMENTS
        requester - requesting sensor enum
        temperature - temperature reached
        level - 0 - MAX_CAMERA_MITIGATION_LEVEL throttling level

RETURN VALUE
        0 on success, -1 on failure.
===========================================================================*/
int camera_request(int requester, int temperature, int level)
{
	int ret = -1;
	int i = 0;
	char msgbuf[REPORT_MSG_MAX] = {0};
	static int camera_req[SENSOR_IDX_MAX];
	static pthread_mutex_t camera_mtx = PTHREAD_MUTEX_INITIALIZER;
	static int prev_camera = 0;

	if (requester < 0 ||
	    requester >= SENSOR_IDX_MAX) {
		msg("%s: Invalid requester %d", __func__, requester);
		return -1;
	}

	if (level < 0)
		level = 0;

	if (level > MAX_CAMERA_MITIGATION_LEVEL)
		level = MAX_CAMERA_MITIGATION_LEVEL;

	temperature = RCONV(temperature);

	pthread_mutex_lock(&camera_mtx);

	/* Aggregate camera mitigation level for all sensors */
	camera_req[requester] = level;
	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		if (camera_req[i] > level)
		level = camera_req[i];
	}

	if (level != prev_camera) {
		if (thermal_server_notify_clients("camera", level) < 0) {
			msg("Unable to set CAMERA mitigation to %d\n", level);
		} else {
			info("ACTION: CAMERA - "
			    "Setting CAMERA mitigation to %d\n", level);
			prev_camera = level;
			ret = 0;
		}
	} else {
		dbgmsg("CAMERA mitigation already at level %d\n", level);
		ret = 0;
	}

	pthread_mutex_unlock(&camera_mtx);
	return ret;
}

/*===========================================================================
FUNCTION camcorder_request

Action function to request camera throttling action.

ARGUMENTS
        requester - requesting sensor enum
        temperature - temperature reached
        level - 0 - MAX_CAMCORDER_MITIGATION_LEVEL throttling level

RETURN VALUE
        0 on success, -1 on failure.
===========================================================================*/
int camcorder_request(int requester, int temperature, int level)
{
	int ret = -1;
	int i = 0;
	char msgbuf[REPORT_MSG_MAX] = {0};
	static int camcorder_req[SENSOR_IDX_MAX];
	static pthread_mutex_t camcorder_mtx = PTHREAD_MUTEX_INITIALIZER;
	static int prev_camcorder = 0;

	if (requester < 0 ||
	    requester >= SENSOR_IDX_MAX) {
		msg("%s: Invalid requester %d", __func__, requester);
		return -1;
	}

	if (level < 0)
		level = 0;

	if (level > MAX_CAMCORDER_MITIGATION_LEVEL)
		level = MAX_CAMCORDER_MITIGATION_LEVEL;

	temperature = RCONV(temperature);

	pthread_mutex_lock(&camcorder_mtx);

	/* Aggregate camera mitigation level for all sensors */
	camcorder_req[requester] = level;
	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		if (camcorder_req[i] > level)
			level = camcorder_req[i];
	}

	if (level != prev_camcorder) {
		if (thermal_server_notify_clients("camcorder", level) < 0) {
			msg("Unable to set CAMCORDER mitigation to %d\n", level);
		} else {
			info("ACTION: CAMCORDER - "
			    "Setting CAMCORDER mitigation to %d\n", level);
			prev_camcorder = level;
			ret = 0;
		}
	} else {
		dbgmsg("CAMCORDER mitigation already at level %d\n", level);
		ret = 0;
	}

	pthread_mutex_unlock(&camcorder_mtx);
	return ret;
}
#endif

/*===========================================================================
FUNCTION powersave_request

Action function to enable/disable powersave mode.

ARGUMENTS
        enable - 0  => disable else enable

RETURN VALUE
        0 on success, -1 on failure.
===========================================================================*/

int powersave_request( int enable )
{
	int ret = -1;

	dbgmsg("%s: Enable = %d\n" , __func__ , enable );

	if( enable > 0 ){
		if ( -1 != system("pm-powersave true") ){
			msg("Enter powersave mode command successful\n");
			ret = 0;
		}
		else{
			msg("%s: pm-powersave true - failed.", __func__);
		}
	}
	else{
		if ( -1 != system("pm-powersave false") ){
			msg("Exit powersave mode command successful\n");
                        ret = 0;
                }
		else{
			msg("%s: pm-powersave false - failed.", __func__);
		}
	}

	return ret;
}

/*===========================================================================
FUNCTION nssfreq_request

Action function to request NSS frequency scaling.

ARGUMENTS
        frequency - nss frequency to scale to, -1 for maximum cpu frequency

RETURN VALUE
        0 on success, else -1 on failure.
===========================================================================*/
static pthread_mutex_t nssfreq_set_mtx = PTHREAD_MUTEX_INITIALIZER;

int nssfreq_request(int frequency)
{
        int ret = -1;
        char freq_buf[FREQ_STR_MAX] = {0};
        char buf[UINT_BUF_MAX] = {0};

	if (MITIGATION_DISABLE == nss_mitigation_level)
		return 0;

        pthread_mutex_lock(&nssfreq_set_mtx);

        if( frequency < 0 ){
		/* Resetting to default */
		snprintf(buf, UINT_BUF_MAX, "%d", 1);
        }
        else{
		snprintf(buf, UINT_BUF_MAX, "%d", 0);
        }

        if (write_to_file(NSS_FREQ_AUTO_SCALE, buf, strlen(buf)) > 0) {
		info("ACTION: NSS - ""Setting NSS Auto-scale to %s\n", buf);
		if( frequency > 0 ){
			frequency = frequency * 1000; /* Convert to Hz */
			snprintf(freq_buf, FREQ_STR_MAX, "%d", frequency);
			if (write_to_file(NSS_CURR_FREQ, freq_buf, strlen(freq_buf)) > 0) {
                                info("ACTION: NSS - ""Setting NSS max freq to %d Hz\n", frequency);
                                ret = 0;
                        }
                        else{
                                msg("%s: Failed to set NSS freq to %d Hz\n.", __func__, frequency);
                        }
                }
		else{
			ret = 0;
		}
	}
        else{
		msg("Setting NSS auto-scale to %s failed. \n", buf);
        }

        pthread_mutex_unlock(&nssfreq_set_mtx);
        return ret;
}

/*===========================================================================
FUNCTION set_mitigation_level

Sets the mitigation levels.

ARGUMENTS
	none

RETURN VALUE
	void.
===========================================================================*/

void set_mitigation_level()
{
	char buf[FREQ_STR_MAX];

	if (read_line_from_file( NSS_FREQ_AUTO_SCALE, buf, FREQ_STR_MAX) <= 0) {
		nss_mitigation_level = MITIGATION_DISABLE;
		dbgmsg("NSS is not mitigated. \n");
	}

	if (read_line_from_file( AFAB_CLK, buf, FREQ_STR_MAX) <= 0) {
		fab_mitigation_level = MITIGATION_DISABLE;
		dbgmsg("FABs are not mitigated. \n");
	}

	if (read_line_from_file( POWER_CTL_PATH, buf, FREQ_STR_MAX) <= 0) {
		powerctl_mitigation_restart = MITIGATION_DISABLE;
		dbgmsg("powerctl script is not present. \n");
	}
}
