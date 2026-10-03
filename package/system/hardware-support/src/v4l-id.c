/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2026 OpenWrt.org
 *
 * Video4Linux device identification.
 */

#include <fcntl.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/videodev2.h>

#include <libubox/blobmsg_json.h>

static const struct {
	const char *name;
	unsigned int mask;
} capabilities[] = {
	{ "video_capture", V4L2_CAP_VIDEO_CAPTURE },
	{ "video_output", V4L2_CAP_VIDEO_OUTPUT },
	{ "video_overlay", V4L2_CAP_VIDEO_OVERLAY },
	{ "vbi_capture", V4L2_CAP_VBI_CAPTURE },
	{ "vbi_output", V4L2_CAP_VBI_OUTPUT },
	{ "sliced_vbi_capture", V4L2_CAP_SLICED_VBI_CAPTURE },
	{ "sliced_vbi_output", V4L2_CAP_SLICED_VBI_OUTPUT },
	{ "rds_capture", V4L2_CAP_RDS_CAPTURE },
	{ "video_output_overlay", V4L2_CAP_VIDEO_OUTPUT_OVERLAY },
	{ "hw_freq_seek", V4L2_CAP_HW_FREQ_SEEK },
	{ "rds_output", V4L2_CAP_RDS_OUTPUT },
	{ "video_capture_mplane", V4L2_CAP_VIDEO_CAPTURE_MPLANE },
	{ "video_output_mplane", V4L2_CAP_VIDEO_OUTPUT_MPLANE },
	{ "video_m2m_mplane", V4L2_CAP_VIDEO_M2M_MPLANE },
	{ "video_m2m", V4L2_CAP_VIDEO_M2M },
	{ "tuner", V4L2_CAP_TUNER },
	{ "audio", V4L2_CAP_AUDIO },
	{ "radio", V4L2_CAP_RADIO },
	{ "modulator", V4L2_CAP_MODULATOR },
	{ "sdr_capture", V4L2_CAP_SDR_CAPTURE },
	{ "ext_pix_format", V4L2_CAP_EXT_PIX_FORMAT },
	{ "sdr_output", V4L2_CAP_SDR_OUTPUT },
	{ "meta_capture", V4L2_CAP_META_CAPTURE },
	{ "readwrite", V4L2_CAP_READWRITE },
	{ "edid", V4L2_CAP_EDID },
	{ "streaming", V4L2_CAP_STREAMING },
	{ "meta_output", V4L2_CAP_META_OUTPUT },
	{ "touch", V4L2_CAP_TOUCH },
	{ "io_mc", V4L2_CAP_IO_MC },
};

#define CAPABILITY_COUNT	(sizeof(capabilities) / sizeof(*capabilities))

static int usage(const char *argv0)
{
	fprintf(stderr,
		"Usage: %s [-c CAP[,CAP...] [-g GROUP] [-G GROUP]] DEVICE\n\n"
		"Describe a V4L2 device as a JSON object on stdout, with one\n"
		"boolean per capability.\n\n"
		"With -c, print nothing and put the device node into the -g\n"
		"GROUP if it has any of the listed capabilities, or into the\n"
		"-G GROUP if it has none of them.\n",
		argv0);

	return 2;
}

/* device_caps describes the node, capabilities the whole device */
static unsigned int device_caps(const struct v4l2_capability *cap)
{
	if (cap->capabilities & V4L2_CAP_DEVICE_CAPS)
		return cap->device_caps;

	return cap->capabilities;
}

static int report(const struct v4l2_capability *cap)
{
	static struct blob_buf b;
	unsigned int caps;
	char *json;
	size_t i;

	caps = device_caps(cap);

	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "driver", (const char *)cap->driver);
	blobmsg_add_string(&b, "card", (const char *)cap->card);
	blobmsg_add_string(&b, "bus_info", (const char *)cap->bus_info);
	blobmsg_add_u64(&b, "version", cap->version);
	blobmsg_add_u64(&b, "capabilities", cap->capabilities);
	blobmsg_add_u64(&b, "device_caps", caps);

	for (i = 0; i < CAPABILITY_COUNT; i++)
		blobmsg_add_u8(&b, capabilities[i].name,
			       !!(caps & capabilities[i].mask));

	json = blobmsg_format_json(b.head, true);
	if (!json) {
		blob_buf_free(&b);
		return 1;
	}

	printf("%s\n", json);
	free(json);
	blob_buf_free(&b);

	return 0;
}

static int capability_mask(char *names, unsigned int *mask)
{
	char *name, *state;
	bool known;
	size_t i;

	*mask = 0;

	for (name = strtok_r(names, ",", &state); name;
	     name = strtok_r(NULL, ",", &state)) {
		known = false;

		for (i = 0; i < CAPABILITY_COUNT; i++) {
			if (strcmp(capabilities[i].name, name))
				continue;

			*mask |= capabilities[i].mask;
			known = true;
			break;
		}

		if (!known) {
			fprintf(stderr, "v4l-id: unknown capability: %s\n",
				name);
			return 2;
		}
	}

	return 0;
}

static int classify(const struct v4l2_capability *cap, const char *device,
		    unsigned int mask, const char *present, const char *absent)
{
	const char *group;
	struct group *grp;

	group = (device_caps(cap) & mask) ? present : absent;
	if (!group)
		return 0;

	grp = getgrnam(group);
	if (!grp) {
		fprintf(stderr, "v4l-id: no such group: %s\n", group);
		return 5;
	}

	if (chown(device, (uid_t)-1, grp->gr_gid) < 0) {
		fprintf(stderr, "v4l-id: cannot set group on %s: %m\n", device);
		return 6;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	const char *device, *present, *absent;
	struct v4l2_capability cap;
	unsigned int mask;
	char *names;
	int fd, ch;

	present = NULL;
	absent = NULL;
	names = NULL;
	mask = 0;

	while ((ch = getopt(argc, argv, "c:g:G:")) != -1) {
		switch (ch) {
		case 'c':
			names = optarg;
			break;
		case 'g':
			present = optarg;
			break;
		case 'G':
			absent = optarg;
			break;
		default:
			return usage(argv[0]);
		}
	}

	if (optind != argc - 1 || !names != !(present || absent))
		return usage(argv[0]);

	if (names && capability_mask(names, &mask))
		return 2;

	device = argv[optind];

	fd = open(device, O_RDONLY | O_CLOEXEC);
	if (fd < 0)
		return 3;

	memset(&cap, 0, sizeof(cap));

	if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
		close(fd);
		return 4;
	}

	close(fd);

	if (names)
		return classify(&cap, device, mask, present, absent);

	return report(&cap);
}
