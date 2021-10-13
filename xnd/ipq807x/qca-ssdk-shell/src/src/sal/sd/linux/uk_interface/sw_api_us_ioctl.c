/*
 * Copyright (c) 2014, 2017-2019, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/ioctl.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#include <fcntl.h>
#include "sw.h"
#include "sw_api.h"
#include "sw_api_us.h"

#define MISC_CHR_DEV       10
static int glb_socket_fd = 0;

sw_error_t
sw_uk_if(unsigned long arg_val[SW_MAX_API_PARAM])
{
    ioctl(glb_socket_fd, SIOCDEVPRIVATE, arg_val);
    return SW_OK;
}

#ifndef SHELL_DEV
#define SHELL_DEV "/dev/switch_ssdk"
#endif
#define MISC_DEV "/proc/misc"

static int sw_device_minor_get(a_uint32_t *device_minor)
{
	char buf[200] = {0};
	FILE *fp;
	char *p;

	fp = fopen(MISC_DEV, "r");
	if (!fp) {
		printf("failed to open %s\n", MISC_DEV);
		return -1;
	}
	fseek(fp, 0, SEEK_SET);
	while (fgets(buf, 200, fp) != NULL) {
		p = strstr(buf, "switch_ssdk");
		if (p) {
			sscanf(buf,"%d",device_minor);
			fclose(fp);
			return 0;
		}
	}

	fclose(fp);
	return -1;
}

static void sw_device_check(void)
{
	struct stat buf;
	a_uint32_t file_minor;
	a_uint32_t device_minor;
	int rv;

	memset(&buf, 0, sizeof(buf));

	if (stat( SHELL_DEV, &buf) < 0) {
		printf("failed to stat!\n");
		return;
	}
	if (S_ISCHR(buf.st_mode)) {
		file_minor = minor(buf.st_rdev);
		rv =  sw_device_minor_get(&device_minor);
		if (!rv) {
			if (device_minor !=  file_minor)
				printf("device:%x file:%x mismatch!\n",
					device_minor, file_minor);
			else
				printf("device:%x file:%x match!\n",
					device_minor, file_minor);
		}
	}

}


sw_error_t
sw_uk_init(a_uint32_t nl_prot)
{
    if (!glb_socket_fd)
    {
        /* even mknod fail we not quit, perhaps the device node exist already */
#if defined UK_MINOR_DEV
        mknod(SHELL_DEV, S_IFCHR, makedev(MISC_CHR_DEV, UK_MINOR_DEV));
#else
        mknod(SHELL_DEV, S_IFCHR, makedev(MISC_CHR_DEV, nl_prot));
#endif
        if ((glb_socket_fd = open(SHELL_DEV, O_RDWR)) < 0)
        {
            sw_device_check();
            return SW_INIT_ERROR;
        }
    }

    return SW_OK;
}

sw_error_t
sw_uk_cleanup(void)
{
    close(glb_socket_fd);
    glb_socket_fd = 0;
#if 0
    remove("/dev/switch_ssdk");
#endif
    return SW_OK;
}

