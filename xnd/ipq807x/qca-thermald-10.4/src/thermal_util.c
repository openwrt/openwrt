/*===========================================================================

  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "thermal.h"

#define THERMAL_SYSFS "/sys/devices/virtual/thermal"
/* Sys proc ID */
#define SYSFS_CPU_TYPE "/proc/device-tree/cpu_type"
#define SYSFS_PLATFORMID  "/sys/devices/soc0/soc_id"
#define SYSFS_PLATFORMID_DEP  "/sys/devices/system/soc/soc0/id"

typedef struct
{
	enum therm_msm_id msm_id;
	int  soc_id;
} therm_msm_soc_type;

static therm_msm_soc_type msm_soc_table[] = {
	{THERM_MSM_8X60,   70},
	{THERM_MSM_8X60,   71},
	{THERM_MSM_8X60,   86},
	{THERM_MSM_8960,   87},
	{THERM_MSM_8960,   122},
	{THERM_MSM_8960,   123},
	{THERM_MSM_8960,   124},
	{THERM_MSM_8960AB, 138},
	{THERM_MSM_8960AB, 139},
	{THERM_MSM_8960AB, 140},
	{THERM_MSM_8960AB, 141},
	{THERM_MSM_8930,   116},
	{THERM_MSM_8930,   117},
	{THERM_MSM_8930,   118},
	{THERM_MSM_8930,   119},
	{THERM_MSM_8930,   120},
	{THERM_MSM_8930,   121},
	{THERM_MSM_8930AA, 142},
	{THERM_MSM_8930AA, 143},
	{THERM_MSM_8930AA, 144},
	{THERM_MSM_8930AA, 160},
	{THERM_MSM_8930AB, 154},
	{THERM_MSM_8930AB, 155},
	{THERM_MSM_8930AB, 156},
	{THERM_MSM_8930AB, 157},
	{THERM_MSM_8064,   109},
	{THERM_MSM_8064,   130},
	{THERM_MSM_8064AB, 153},
	{THERM_MSM_8X25,   90},
	{THERM_MSM_8X25,   91},
	{THERM_MSM_8X25,   92},
	{THERM_MSM_8X25,   97},
	{THERM_MSM_8974,   126},
	{THERM_IPQ_8062,   201},
	{THERM_IPQ_8064,   202},
	{THERM_IPQ_8066,   203},
	{THERM_IPQ_8068,   204},
	{THERM_IPQ_8065,   280},
	{THERM_IPQ_8069,   281},
	/* AC variants */
	{THERM_IPQ_8070,   375},
	{THERM_IPQ_8070A,  395},
	{THERM_IPQ_8071,   376},
	{THERM_IPQ_8071A,  396},
	/* HK variants */
	{THERM_IPQ_8072,   342},
	{THERM_IPQ_8072A,  389},
	{THERM_IPQ_8074,   323},
	{THERM_IPQ_8074A,  390},
	{THERM_IPQ_8076,   343},
	{THERM_IPQ_8076A,  391},
	{THERM_IPQ_8078,   344},
	{THERM_IPQ_8078A,  392},
	/* OAK variants */
	{THERM_IPQ_8172,   397},
	{THERM_IPQ_8173,   398},
	{THERM_IPQ_8174,   399},
	/* CP variants */
	{THERM_IPQ_6018,   402},
	{THERM_IPQ_6028,   403},
	{THERM_IPQ_6000,   421},
	{THERM_IPQ_6010,   422},
};

int read_id_from_binary_file(char *path, size_t size) {
	FILE *fp;
	int ret;

	fp = fopen(path, "r");
	if (fp == NULL)
		return -1;
	if(!fread(&ret, size, 1, fp)) {
		ret = ferror(fp);
	}
	fclose(fp);
	return ret;
}

/* returns platform id, or -errno if error */
static int get_platform_id(void)
{
	int ret = -1;
	char buf[UINT_BUF_MAX];
	if (!access(SYSFS_CPU_TYPE, F_OK)) {
		ret = read_id_from_binary_file(SYSFS_CPU_TYPE, sizeof(int));

		if (ret >= 0)
			return ret;
	} else if (!access(SYSFS_PLATFORMID, F_OK)) {
		ret = read_line_from_file(SYSFS_PLATFORMID, buf, UINT_BUF_MAX);
	} else {
		ret = read_line_from_file(SYSFS_PLATFORMID_DEP, buf, UINT_BUF_MAX);
	}
	if (ret < 0) {
		msg("Error getting platform_id %d", ret);
	} else {
		ret = atoi(buf);
	}

	return ret;
}

enum therm_msm_id therm_get_msm_id(void)
{
	enum therm_msm_id ret_val = THERM_MSM_UNKNOWN;
	int idx;
	int soc_id = get_platform_id();

	for (idx = 0 ; idx < ARRAY_SIZE(msm_soc_table); idx++) {
		if (soc_id == msm_soc_table[idx].soc_id) {
			ret_val = msm_soc_table[idx].msm_id;
			break;
		}
	}
	return ret_val;
}

/*===========================================================================
FUNCTION open_file

Utility function to open file.

ARGUMENTS
	path - pathname for file
	flags - file open flags

RETURN VALUE
	file descriptor on success,
	-1 on failure.
===========================================================================*/
static int open_file(char *path, int flags)
{
	int rv;

	if (!path) return -EINVAL;

	rv = open(path, flags);
	if (rv < 0)
		rv = -errno;

	return rv;
}

/*===========================================================================
FUNCTION write_to_fd

Utility function to write to provided file descriptor.

ARGUMENTS
	fd - file descriptor
	buf - destination buffer to write to
	count - number of bytes to write to fd

RETURN VALUE
	number of bytes written on success, -errno on failure.
===========================================================================*/
static int write_to_fd(int fd, char *buf, size_t count)
{
	ssize_t pos = 0;
	ssize_t rv = 0;

	do {
		rv = write(fd, buf + pos, count - pos);
		if (rv < 0)
			return -errno;
		pos += rv;
	} while ((ssize_t)count > pos);

	return count;
}

/*===========================================================================
FUNCTION read_line_from_file

Utility function to read characters from file and stores them in a string
until (count-1) characters are read or either a newline or EOF is reached.

ARGUMENTS
	path - file path
	buf - destination buffer to read from
	count - max number of bytes to read from file

RETURN VALUE
	number of bytes read on success, -errno on failure.
===========================================================================*/
int read_line_from_file(char *path, char *buf, size_t count)
{
	char * fgets_ret;
	FILE * fd;
	int rv;

	fd = fopen(path, "r");
	if (fd == NULL)
		return -1;

	fgets_ret = fgets(buf, count, fd);
	if (NULL != fgets_ret) {
		rv = strlen(buf);
	} else {
		rv = ferror(fd);
	}

	fclose(fd);

	return rv;
}

/*===========================================================================
FUNCTION write_to_file

Utility function to write to provided file path.

ARGUMENTS
	path - file path
	buf - destination buffer to write to
	count - number of bytes to write to file

RETURN VALUE
	number of bytes written on success, -errno on failure.
===========================================================================*/
int write_to_file(char *path, char *buf, size_t count)
{
	int fd, rv;

	fd = open_file(path, O_RDWR);
	if (fd < 0)
		return fd;

	rv = write_to_fd(fd, buf, count);
	close(fd);

	return rv;
}

/*===========================================================================
FUNCTION connect_local_file_socket

Utility function to open and connect to local UNIX filesystem socket.

ARGUMENTS
	socket_name - name of local UNIX filesystem socket to be connected

RETURN VALUE
	Connected socket_fd on success,
	-1 on failure.
===========================================================================*/
static int connect_local_file_socket(char *socket_name)
{
	int socket_fd = 0;
	struct sockaddr_un serv_addr;

	socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		dbgmsg("socket error - %s\n", strerror(errno));
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	snprintf(serv_addr.sun_path, UNIX_PATH_MAX, "%s", socket_name);
	serv_addr.sun_family = AF_LOCAL;

	if (connect(socket_fd, (struct sockaddr *) &serv_addr,
		    sizeof(sa_family_t) + strlen(socket_name) ) != 0) {
		dbgmsg("connect error on %s - %s\n",
		       socket_name, strerror(errno));
		close(socket_fd);
		return -1;
	}

	return socket_fd;
}


/*===========================================================================
FUNCTION connect_local_socket

Utility function to open and connect to abstract local UNIX socket.

ARGUMENTS
	socket_name - name of local UNIX socket to be connected in abstract
			socket namespace.

RETURN VALUE
	Connected socket_fd on success,
	-1 on failure.
===========================================================================*/
static int connect_local_socket(char *socket_name)
{
	int socket_fd = 0;
	struct sockaddr_un serv_addr;

	socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		dbgmsg("socket error - %s\n", strerror(errno));
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	snprintf(serv_addr.sun_path, UNIX_PATH_MAX, " %s", socket_name);
	serv_addr.sun_family = AF_LOCAL;
	/* abstract namespace socket starts with NULL char */
	serv_addr.sun_path[0] = '\0';

	/* effective sockaddr_un size == sun_family + null-char + name len */
	if (connect(socket_fd, (struct sockaddr *) &serv_addr,
		    sizeof(sa_family_t) + 1 + strlen(socket_name) ) != 0) {
		dbgmsg("connect error on %s - %s\n",
		       socket_name, strerror(errno));
		close(socket_fd);
		return -1;
	}

	return socket_fd;
}

/*===========================================================================
FUNCTION write_to_local_file_socket

Utility function to write to local filesystem UNIX socket.

ARGUMENTS
	socket_name - socket name to be written
	msg - message to be written on socket
	count - size of msg buffer to be written

RETURN VALUE
	Number of bytes written on success,
	-1 on failure.
===========================================================================*/
int write_to_local_file_socket(char *socket_name, char *msg, size_t count)
{
	int socket_fd = 0;
	int rv;

	if (minimum_mode) {
		return -1;
	}

	socket_fd = connect_local_file_socket(socket_name);
        if (socket_fd < 0) {
		return -1;
	}

	rv = write_to_fd(socket_fd, msg, count);
	close(socket_fd);

	return rv;
}

/*===========================================================================
FUNCTION write_to_local_socket

Utility function to write to abstract local UNIX socket.

ARGUMENTS
	socket_name - socket name to be written
	msg - message to be written on socket
	count - size of msg buffer to be written

RETURN VALUE
	Number of bytes written on success,
	-1 on failure.
===========================================================================*/
int write_to_local_socket(char *socket_name, char *msg, size_t count)
{
	int socket_fd = 0;
	int rv;

	if (minimum_mode) {
		return -1;
	}

	socket_fd = connect_local_socket(socket_name);
        if (socket_fd < 0) {
		return -1;
	}

	rv = write_to_fd(socket_fd, msg, count);
	close(socket_fd);

	return rv;
}

/*===========================================================================
FUNCTION get_tzn

Utility function to match a sensor name with thermal zone id.

ARGUMENTS
	sensor_name - name of sensor to match

RETURN VALUE
	Thermal zone id on success,
	-1 on failure.
===========================================================================*/
int get_tzn(char *sensor_name)
{
	DIR *tdir = NULL;
	struct dirent *tdirent = NULL;
	int found = -1;
	int tzn = 0;
	char name[MAX_PATH] = {0};
	char cwd[MAX_PATH] = {0};

	if (!getcwd(cwd, sizeof(cwd)))
		return found;

	/* Change dir to read the entries. Doesnt work otherwise */
	if (chdir(THERMAL_SYSFS) != 0) {
		msg("Change directory failed: %s\n", THERMAL_SYSFS);
		return found;
	}

	tdir = opendir(THERMAL_SYSFS);
	if (!tdir) {
		msg("Unable to open %s\n", THERMAL_SYSFS);
		return found;
	}

	while ((tdirent = readdir(tdir))) {
		char buf[50];
		struct dirent *tzdirent;
		DIR *tzdir = NULL;

		tzdir = opendir(tdirent->d_name);
		if (!tzdir)
			continue;
		while ((tzdirent = readdir(tzdir))) {
			if (strcmp(tzdirent->d_name, "type"))
				continue;
			snprintf(name, MAX_PATH, TZ_TYPE, tzn);
			dbgmsg("Opening %s\n", name);
			read_line_from_file(name, buf, sizeof(buf));
			buf[strlen(sensor_name)] = '\0';
			if (!strcmp(buf, sensor_name)) {
				found = 1;
				break;
			}
			tzn++;
		}
		closedir(tzdir);
		if (found == 1)
			break;
	}

	closedir(tdir);

	if (chdir(cwd) != 0) { /* Restore current working dir */
		msg("Restore directory failed: %s\n", cwd);
		return -1;
	}

	if (found == 1) {
		found = tzn;
		dbgmsg("Sensor %s found at tz: %d\n", sensor_name, tzn);
	}

	return found;
}
