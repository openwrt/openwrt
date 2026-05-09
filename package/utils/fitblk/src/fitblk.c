// SPDX-License-Identifier: GPL-2.0-only
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <linux/dm-ioctl.h>
#include <linux/fitblk.h>

#define DM_CONTROL_PATH "/dev/mapper/control"

static int dm_remove(const char *holder)
{
	char path[PATH_MAX];
	char name[DM_NAME_LEN];
	struct dm_ioctl dmi;
	int fd, ret, n;

	/* Read DM device name from sysfs */
	snprintf(path, sizeof(path), "/sys/block/%s/dm/name", holder);
	fd = open(path, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "fitblk: failed to open %s: %s\n",
			path, strerror(errno));
		return errno;
	}

	n = read(fd, name, sizeof(name) - 1);
	close(fd);

	if (n <= 0)
		return errno ? errno : EIO;

	/* Strip trailing newline */
	if (name[n - 1] == '\n')
		n--;
	name[n] = '\0';

	/* Open DM control device */
	fd = open(DM_CONTROL_PATH, O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "fitblk: failed to open %s: %s\n",
			DM_CONTROL_PATH, strerror(errno));
		return errno;
	}

	/* Prepare DM ioctl structure */
	memset(&dmi, 0, sizeof(dmi));
	dmi.version[0] = DM_VERSION_MAJOR;
	dmi.version[1] = DM_VERSION_MINOR;
	dmi.version[2] = DM_VERSION_PATCHLEVEL;
	dmi.data_size = sizeof(dmi);
	strncpy(dmi.name, name, sizeof(dmi.name) - 1);

	/* Remove the DM device */
	ret = ioctl(fd, DM_DEV_REMOVE, &dmi);
	close(fd);

	if (ret == -1) {
		fprintf(stderr, "fitblk: failed to remove dm device '%s': %s\n",
			name, strerror(errno));
		return errno;
	}

	fprintf(stderr, "fitblk: removed dm device '%s' (%s)\n", name, holder);
	return 0;
}

static int release_holders(const char *devname)
{
	char path[PATH_MAX];
	DIR *dir;
	struct dirent *entry;
	int ret;

	snprintf(path, sizeof(path), "/sys/block/%s/holders", devname);
	dir = opendir(path);
	if (!dir) {
		/* No holders directory is fine */
		if (errno == ENOENT)
			return 0;
		fprintf(stderr, "fitblk: failed to open %s: %s\n",
			path, strerror(errno));
		return errno;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_name[0] == '.')
			continue;

		/* Recursively release holders of this holder first */
		ret = release_holders(entry->d_name);
		if (ret) {
			closedir(dir);
			return ret;
		}

		if (!strncmp(entry->d_name, "dm-", 3)) {
			ret = dm_remove(entry->d_name);
			if (ret) {
				closedir(dir);
				return ret;
			}
		} else {
			fprintf(stderr,
				"fitblk: unsupported holder '%s', skipping\n",
				entry->d_name);
		}
	}

	closedir(dir);
	return 0;
}

static int fitblk_release(char *device)
{
	const char *devname;
	int fd, ret;

	/* Extract block device name from path (e.g. /dev/fit0 -> fit0) */
	devname = strrchr(device, '/');
	if (devname)
		devname++;
	else
		devname = device;

	/* Release any holders (e.g. dm-verity) before releasing the device */
	ret = release_holders(devname);
	if (ret) {
		fprintf(stderr,
			"fitblk: error releasing holders of %s: %s\n",
			device, strerror(ret));
		return ret;
	}

	fd = open(device, O_RDONLY);
	if (fd == -1)
		return errno;

	ret = ioctl(fd, FITBLK_RELEASE, NULL);
	close(fd);

	if (ret == -1)
		return errno;

	return 0;
}

int main(int argc, char *argp[])
{
	int ret;

	if (argc != 2) {
		fprintf(stderr, "Release uImage.FIT sub-image block device\n");
		fprintf(stderr, "Syntax: %s /dev/fitXXX\n", argp[0]);
		return -EINVAL;
	}

	ret = fitblk_release(argp[1]);
	if (ret)
		fprintf(stderr, "fitblk: error releasing %s: %s\n", argp[1],
			strerror(ret));
	else
		fprintf(stderr, "fitblk: %s released\n", argp[1]);

	return ret;
}
