/*===========================================================================

  Copyright (c) 2010-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#ifndef __THERMAL_H__
#define __THERMAL_H__

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include "sensors-hw.h"

#ifdef ANDROID
#  include "cutils/properties.h"
#  ifdef USE_ANDROID_LOG
#    define LOG_TAG         "ThermalDaemon"
#    include "cutils/log.h"
#  endif
#endif

#ifndef IPQ_806x
#include "common_log.h" /* define after cutils/log.h */
#endif

#ifdef USE_GLIB
#include <glib/gprintf.h>
#define strlcat g_strlcat
#define strlcpy g_strlcpy
#endif /* USE_GLIB */

#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif

#ifndef CONFIG_FILE_DEFAULT
#define CONFIG_FILE_DEFAULT  "/system/etc/thermald.conf"
#endif

#define MAX_CPUS              (4)       /* Supporting up to 4 core systems */

/* Abstract local socket name that report actions are sent on */
#define UI_LOCALSOCKET_NAME  "THERMALD_UI"

#define SAMPLING_MS_DEFAULT  (5000)
#ifdef SENSORS_8960
#define SAMPLING_MS_MINIMUM  (100)
#else
#define SAMPLING_MS_MINIMUM  (250)
#endif
#define THRESHOLDS_MAX       (8)
#define ACTIONS_MAX          (8)
#define MAX_PATH             (256)
#define REPORT_MSG_MAX       (60)
#define UINT_BUF_MAX         (12)

/* core limit temperature sanity check values in degC */
#define CORELIMIT_MIN        (50)
#define CORELIMIT_MAX        (150)

/* Common thermal sysfs defines */
#define TZ_NODE "/sys/class/thermal/thermal_zone%d"
#define TZ_MODE "/sys/devices/virtual/thermal/thermal_zone%d/mode"
#define TZ_TEMP "/sys/devices/virtual/thermal/thermal_zone%d/temp"
#define TZ_TYPE "/sys/devices/virtual/thermal/thermal_zone%d/type"

/* CPU Frequency Scaling Action */
#define GOVERNOR_NODE    "/cpufreq/scaling_governor"
#define FMAX_INFO_NODE   "/cpufreq/cpuinfo_max_freq"
#define FMIN_INFO_NODE   "/cpufreq/cpuinfo_min_freq"
#define FREQ_MAX_NODE    "/cpufreq/scaling_max_freq"
#define FREQ_MIN_NODE    "/cpufreq/scaling_min_freq"
#define FREQ_USR_NODE    "/cpufreq/scaling_setspeed"
#define FREQ_RPT_NODE    "/cpufreq/scaling_cur_freq"
#define HOTPLUG_NODE     "/online"
#define CPU_SYSFS_DIR    "/sys/devices/system/cpu"
#define CPU_SYSFS(NODE)  (CPU_SYSFS_DIR "/cpu%d" NODE)

/*NSS Frequency Scaling Actions */
#define NSS_FREQ_MAX_NODE 	"/proc/sys/dev/nss/clock/freq_table"
#define NSS_FREQ_AUTO_SCALE 	"/proc/sys/dev/nss/clock/auto_scale"
#define NSS_CURR_FREQ		"/proc/sys/dev/nss/clock/current_freq"

/* Fabric Frequency Scaling Actions */
#define AFAB_CLK "/sys/kernel/debug/clk/afab_a_clk/rate"
#define DFAB_CLK "/sys/kernel/debug/clk/dfab_a_clk/rate"
#define SFPB_CLK "/sys/kernel/debug/clk/sfpb_a_clk/rate"
#define CFPB_CLK "/sys/kernel/debug/clk/cfpb_a_clk/rate"
#define NSSFAB0_CLK "/sys/kernel/debug/clk/nssfab0_a_clk/rate"
#define NSSFAB1_CLK "/sys/kernel/debug/clk/nssfab1_a_clk/rate"
#define EBI1_CLK "/sys/kernel/debug/clk/ebi1_a_clk/rate"

/* Power Control Script */
#define POWER_CTL_PATH "/etc/init.d/powerctl"

/* GPU Frequency Scaling Action */
#define GPU_FREQ_MAX_NODE "/max_gpuclk"
#define GPU_FREQ_RPT_NODE "/gpuclk"
#define GPU_SYSFS_DIR     "/sys/class/kgsl"
#define GPU_SYSFS(NODE)   (GPU_SYSFS_DIR "/kgsl-3d%d" NODE)

enum therm_msm_id {
	THERM_MSM_UNKNOWN = 0,
	THERM_MSM_8X60,
	THERM_MSM_8960,
	THERM_MSM_8960AB,
	THERM_MSM_8930,
	THERM_MSM_8930AA,
	THERM_MSM_8930AB,
	THERM_MSM_8064,
	THERM_MSM_8064AB,
	THERM_MSM_8X25,
	THERM_MSM_8974,
	THERM_IPQ_8062,
	THERM_IPQ_8064,
	THERM_IPQ_8066,
	THERM_IPQ_8068,
	THERM_IPQ_8065,
	THERM_IPQ_8069,
	THERM_IPQ_8070,
	THERM_IPQ_8070A,
	THERM_IPQ_8071,
	THERM_IPQ_8071A,
	THERM_IPQ_8072,
	THERM_IPQ_8072A,
	THERM_IPQ_8074,
	THERM_IPQ_8074A,
	THERM_IPQ_8076,
	THERM_IPQ_8076A,
	THERM_IPQ_8078,
	THERM_IPQ_8078A,
	THERM_IPQ_8172,
	THERM_IPQ_8173,
	THERM_IPQ_8174,
	THERM_IPQ_6018,
	THERM_IPQ_6028,
	THERM_IPQ_6000,
	THERM_IPQ_6010
};

enum therm_msm_id therm_get_msm_id(void);

/* Utility macros */
#define ARRAY_SIZE(x) (int)(sizeof(x)/sizeof(x[0]))

/* Convert from Celcius to hardware unit (2^-10 *C); and back */
#define CONV(x)               (int)((double)x / pow(2.0, -10))
#define RCONV(x)              ((double)x * pow(2.0, -10))


enum {
	NONE,
	CPU_FREQ,
	SHUTDOWN,
#ifdef IPQ_806x
	POWERSAVE,
	NSS_FREQ,
#else
	REPORT,
	LCD,
	MODEM,
	FUSION,
	BATTERY,
	GPU_FREQ,
	WLAN,
	CAMERA,
	CAMCORDER,
#endif
	ACTION_IDX_MAX
} action_type;

enum {
	THRESHOLD_CLEAR = -1,
	THRESHOLD_NOCHANGE = 0,
	THRESHOLD_CROSS = 1
} threshold_trigger;


typedef struct _action_t {
	int action;
	int info;
} action_t;

typedef struct _threshold_t {
	int      lvl_trig;
	int      lvl_clr;
	action_t actions[ACTIONS_MAX];
	int      num_actions;
} threshold_t;

struct _sensor_t;

typedef struct _sensor_setting_t {
	char        *desc;
	int         id;
	unsigned int chan_idx;
	int         disabled;
	pthread_t   monitor;
	int         sampling_period_us;
	int         num_thresholds;
	threshold_t t[THRESHOLDS_MAX];
	int         last_lvl;
	struct _sensor_t   *sensor;
	unsigned int action_mask;
	int         hotplug_lvl_trig;
	int         hotplug_lvl_clr;

	/* internal counters used during config parsing */
	int       _n_thresholds;
	int       _n_to_clear;
	int       _n_actions;
	int       _n_action_info;
} sensor_setting_t;

/* Common sensor struct */
typedef struct _sensor_t {
	/* sensor name */
	char *name;
	/* setup function needs to be called before get_temperature */
	int (*setup)(sensor_setting_t *settings, struct _sensor_t* sensor);
	/* shutdown function for resource cleanup */
	void (*shutdown)(sensor_setting_t *settings);
	/* get_temperature function to query sensor reading */
	int (*get_temperature)(sensor_setting_t *setting);
	/* wait on threshold interrupt */
	void (*interrupt_wait)(sensor_setting_t *setting);
	/* update sensor thresholds */
	void (*update_thresholds)(sensor_setting_t *setting,
				  int threshold_raised,
				  int threshold_level);
	/* enable/disable sensor */
	void (*enable_sensor)(int tzn, int enabled);
	/* associated sensor settings */
	sensor_setting_t *setting;
	/* sysfs thermal zone number */
	int tzn;
	/* misc data */
	void *data;
	/* enable interrupt */
	int interrupt_enable;
} sensor_t;

typedef struct _thermal_setting_t {
	int              sample_period_ms;
	int              soc_id;
	int              disable_unused_tsens;
	sensor_setting_t sensors[SENSOR_IDX_MAX];
} thermal_setting_t;

typedef struct _def_sensor_setting_t {
	int sensor_count;
	sensor_setting_t *sensors;
}def_sensor_setting_t;

typedef struct {
	char *sensor_name;
	int cpu;
	int sensor_idx;
} hotplug_map_t;

#ifdef USE_ANDROID_LOG
#define msg(format, ...)   LOGE(format, ## __VA_ARGS__)
#define info(format, ...)   LOGI(format, ## __VA_ARGS__)
#else
#define msg(format, ...)   printf(format, ## __VA_ARGS__)
#define info(format, ...)   printf(format, ## __VA_ARGS__)
#endif

#define dbgmsg(format, ...)				\
	do {						\
		if (debug_output)			\
			info(format, ## __VA_ARGS__);	\
	} while (0)

extern int exit_daemon;
extern int debug_output;
extern int enable_restore_brightness;
extern int num_cpus;
extern int num_gpus;
extern int minimum_mode;
extern int new_corelimit;
extern int dump_bcl_ibat_imax;
extern char *dump_bcl_ibat_imax_file;

void init_settings(thermal_setting_t *settings);
int  load_config(thermal_setting_t *settings, const char *pFName);

int read_line_from_file(char *path, char *buf, size_t count);
int write_to_file(char *path, char *buf, size_t count);
int write_to_local_file_socket(char * socket_name, char *msg, size_t count);
int write_to_local_socket(char * socket_name, char *msg, size_t count);
int get_tzn(char *sensor_name);
int send_to_clients(char *buf);

void thermal_monitor(thermal_setting_t *settings);

int cpufreq_init(void);
void cpufreq_restore(void);
#ifndef IPQ_806x
int gpufreq_init(void);
int thermald_client_socket_init(void);
void thermald_client_socket_release(void);
#endif

int shutdown_action(int requester, int temperature, int delay);
int cpufreq_request(int cpu, int requester, int temperature, int frequency);
#ifdef IPQ_806x
int powersave_request( int enable );
int nssfreq_request( int frequency );
int powerctl_restart(int reset_max);
#else
int report_action(int requester, int temperature, int level, int is_trigger);
int lcd_brightness_request(int requester, int temperature, int value);
int battery_request(int requester, int temperature, int level);
int gpufreq_request(int gpu, int requester, int temperature, int level);
int wlan_request(int requester, int temperature, int level);
int camera_request(int requester, int temperature, int level);
int camcorder_request(int requester, int temperature, int level);
#endif


#ifndef IPQ_806x

#ifdef ENABLE_MODEM_MITIGATION
int modem_communication_init(void);
int modem_communication_release(void);
int modem_request(int requester, int temperature, int level);
int fusion_modem_request(int requester, int temperature, int level);
#else
static inline int modem_communication_init(void)
{
	return -1;
}
static inline int modem_communication_release(void)
{
	return -1;
}
static inline int modem_request(int requester, int temperature, int level)
{
	return -1;
}
static inline int fusion_modem_request(int requester, int temperature, int level)
{
	return -1;
}
#endif

#ifdef ENABLE_MODEM_TS
int modem_ts_qmi_init(void);
int modem_qmi_ts_comm_release(void);
int modem_ts_temp_request(const char *sensor_id, int send_current_temp_report,
			   int high_valid, int high_thresh, int low_valid,
			   int low_thresh);
#else
static int modem_ts_qmi_init(void)
{
	return -(EPERM);
}
static int modem_qmi_ts_comm_release(void)
{
	return -(EPERM);
}

static int modem_ts_temp_request(const char *sensor_id,
				  int send_current_temp_report,
				  int high_valid, int high_thresh,
				  int low_valid,
				  int low_thresh)
{
	return -(EPERM);
}
#endif

#endif

int sensors_setup(thermal_setting_t *setting);
void sensors_shutdown(void);
int sensor_get_temperature(sensor_setting_t *sensor);
int sensor_threshold_trigger(int value, sensor_setting_t *sensor, int level);
int sensor_threshold_clear(int value, sensor_setting_t *sensor, int level);
void sensor_wait(sensor_setting_t *sensor);
void sensor_update_thresholds(sensor_setting_t *sensor, int threshold_type, int level);
#ifdef IPQ_806x
void sensor_enable( sensor_setting_t *setting, int tzn ,int enabled );
#endif


#endif  /* __THERMAL_H__ */
