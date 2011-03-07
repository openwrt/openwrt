/*
 * Copyright (C) 2004-2006 Kay Sievers <kay@vrfy.org>
 * Copyright (C) 2006 Hannes Reinecke <hare@suse.de>
 *
 *	This program is free software; you can redistribute it and/or modify it
 *	under the terms of the GNU General Public License as published by the
 *	Free Software Foundation version 2 of the License.
 * 
 *	This program is distributed in the hope that it will be useful, but
 *	WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	General Public License for more details.
 * 
 *	You should have received a copy of the GNU General Public License along
 *	with this program; if not, write to the Free Software Foundation, Inc.,
 *	51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PATH_SIZE 512

static int verbose;
static int dry_run;

void log_message(int priority, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vsyslog(priority, format, args);
	va_end(args);
}

#undef err
#define err(format, arg...)                         \
    do {                                    \
        log_message(LOG_ERR ,"%s: " format ,__FUNCTION__ ,## arg);  \
    } while (0)

#undef info
#define info(format, arg...)                            \
    do {                                    \
        log_message(LOG_INFO ,"%s: " format ,__FUNCTION__ ,## arg); \
    } while (0)

#ifdef DEBUG
#undef dbg
#define dbg(format, arg...)                         \
    do {                                    \
        log_message(LOG_DEBUG ,"%s: " format ,__FUNCTION__ ,## arg);    \
    } while (0)
#else
#define dbg(...) do {} while(0)
#endif


static void trigger_uevent(const char *devpath)
{
	char filename[PATH_SIZE];
	int fd;

	strlcpy(filename, "/sys", sizeof(filename));
	strlcat(filename, devpath, sizeof(filename));
	strlcat(filename, "/uevent", sizeof(filename));

	if (verbose)
		printf("%s\n", devpath);

	if (dry_run)
		return;

	fd = open(filename, O_WRONLY);
	if (fd < 0) {
		dbg("error on opening %s: %s\n", filename, strerror(errno));
		return;
	}

	if (write(fd, "add", 3) < 0)
		info("error on triggering %s: %s\n", filename, strerror(errno));

	close(fd);
}

static int sysfs_resolve_link(char *devpath, size_t size)
{
	char link_path[PATH_SIZE];
	char link_target[PATH_SIZE];
	int len;
	int i;
	int back;

	strlcpy(link_path, "/sys", sizeof(link_path));
	strlcat(link_path, devpath, sizeof(link_path));
	len = readlink(link_path, link_target, sizeof(link_target));
	if (len <= 0)
		return -1;
	link_target[len] = '\0';
	dbg("path link '%s' points to '%s'", devpath, link_target);

	for (back = 0; strncmp(&link_target[back * 3], "../", 3) == 0; back++)
		;
	dbg("base '%s', tail '%s', back %i", devpath, &link_target[back * 3], back);
	for (i = 0; i <= back; i++) {
		char *pos = strrchr(devpath, '/');

		if (pos == NULL)
			return -1;
		pos[0] = '\0';
	}
	dbg("after moving back '%s'", devpath);
	strlcat(devpath, "/", size);
	strlcat(devpath, &link_target[back * 3], size);
	return 0;
}


static int device_list_insert(const char *path)
{
	char filename[PATH_SIZE];
	char devpath[PATH_SIZE];
	struct stat statbuf;

	dbg("add '%s'" , path);

	/* we only have a device, if we have an uevent file */
	strlcpy(filename, path, sizeof(filename));
	strlcat(filename, "/uevent", sizeof(filename));
	if (stat(filename, &statbuf) < 0)
		return -1;
	if (!(statbuf.st_mode & S_IWUSR))
		return -1;

	strlcpy(devpath, &path[4], sizeof(devpath));

	/* resolve possible link to real target */
	if (lstat(path, &statbuf) < 0)
		return -1;
	if (S_ISLNK(statbuf.st_mode))
		if (sysfs_resolve_link(devpath, sizeof(devpath)) != 0)
			return -1;

	trigger_uevent(devpath);
	return 0;
}


static void scan_subsystem(const char *subsys)
{
	char base[PATH_SIZE];
	DIR *dir;
	struct dirent *dent;

	strlcpy(base, "/sys/", sizeof(base));
	strlcat(base, subsys, sizeof(base));

	dir = opendir(base);
	if (dir != NULL) {
		for (dent = readdir(dir); dent != NULL; dent = readdir(dir)) {
			char dirname[PATH_SIZE];
			DIR *dir2;
			struct dirent *dent2;

			if (dent->d_name[0] == '.')
				continue;

			strlcpy(dirname, base, sizeof(dirname));
			strlcat(dirname, "/", sizeof(dirname));
			strlcat(dirname, dent->d_name, sizeof(dirname));
			strlcat(dirname, "/devices", sizeof(dirname));

			/* look for devices */
			dir2 = opendir(dirname);
			if (dir2 != NULL) {
				for (dent2 = readdir(dir2); dent2 != NULL; dent2 = readdir(dir2)) {
					char dirname2[PATH_SIZE];

					if (dent2->d_name[0] == '.')
						continue;

					strlcpy(dirname2, dirname, sizeof(dirname2));
					strlcat(dirname2, "/", sizeof(dirname2));
					strlcat(dirname2, dent2->d_name, sizeof(dirname2));
					device_list_insert(dirname2);
				}
				closedir(dir2);
			}
		}
		closedir(dir);
	}
}

static void scan_block(void)
{
	char base[PATH_SIZE];
	DIR *dir;
	struct dirent *dent;

	strlcpy(base, "/sys/block", sizeof(base));

	dir = opendir(base);
	if (dir != NULL) {
		for (dent = readdir(dir); dent != NULL; dent = readdir(dir)) {
			char dirname[PATH_SIZE];
			DIR *dir2;
			struct dirent *dent2;

			if (dent->d_name[0] == '.')
				continue;

			strlcpy(dirname, base, sizeof(dirname));
			strlcat(dirname, "/", sizeof(dirname));
			strlcat(dirname, dent->d_name, sizeof(dirname));
			if (device_list_insert(dirname) != 0)
				continue;

			/* look for partitions */
			dir2 = opendir(dirname);
			if (dir2 != NULL) {
				for (dent2 = readdir(dir2); dent2 != NULL; dent2 = readdir(dir2)) {
					char dirname2[PATH_SIZE];

					if (dent2->d_name[0] == '.')
						continue;

					if (!strcmp(dent2->d_name,"device"))
						continue;

					strlcpy(dirname2, dirname, sizeof(dirname2));
					strlcat(dirname2, "/", sizeof(dirname2));
					strlcat(dirname2, dent2->d_name, sizeof(dirname2));
					device_list_insert(dirname2);
				}
				closedir(dir2);
			}
		}
		closedir(dir);
	}
}

static void scan_class(void)
{
	char base[PATH_SIZE];
	DIR *dir;
	struct dirent *dent;

	strlcpy(base, "/sys/class", sizeof(base));

	dir = opendir(base);
	if (dir != NULL) {
		for (dent = readdir(dir); dent != NULL; dent = readdir(dir)) {
			char dirname[PATH_SIZE];
			DIR *dir2;
			struct dirent *dent2;

			if (dent->d_name[0] == '.')
				continue;

			strlcpy(dirname, base, sizeof(dirname));
			strlcat(dirname, "/", sizeof(dirname));
			strlcat(dirname, dent->d_name, sizeof(dirname));
			dir2 = opendir(dirname);
			if (dir2 != NULL) {
				for (dent2 = readdir(dir2); dent2 != NULL; dent2 = readdir(dir2)) {
					char dirname2[PATH_SIZE];

					if (dent2->d_name[0] == '.')
						continue;

					if (!strcmp(dent2->d_name, "device"))
						continue;

					strlcpy(dirname2, dirname, sizeof(dirname2));
					strlcat(dirname2, "/", sizeof(dirname2));
					strlcat(dirname2, dent2->d_name, sizeof(dirname2));
					device_list_insert(dirname2);
				}
				closedir(dir2);
			}
		}
		closedir(dir);
	}
}

int main(int argc, char *argv[], char *envp[])
{
	char base[PATH_SIZE];
	struct stat statbuf;
	int failed = 0;
	int option;

	openlog("udevtrigger", LOG_PID | LOG_CONS, LOG_DAEMON);

	while (1) {
		option = getopt(argc, argv, "vnh");
		if (option == -1)
			break;

		switch (option) {
		case 'v':
			verbose = 1;
			break;
		case 'n':
			dry_run = 1;
			break;
		case 'h':
			printf("Usage: udevtrigger OPTIONS\n"
			       "  -v                     print the list of devices while running\n"
			       "  -n                     do not actually trigger the events\n"
			       "  -h                     print this text\n"
			       "\n");
			goto exit;
		default:
			goto exit;
		}
	}


	/* if we have /sys/subsystem, forget all the old stuff */
	scan_subsystem("bus");
	scan_class();

	/* scan "block" if it isn't a "class" */
	strlcpy(base, "/sys/class/block", sizeof(base));
	if (stat(base, &statbuf) != 0)
		scan_block();

exit:

	closelog();
	return 0;
}
