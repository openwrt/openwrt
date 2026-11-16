// SPDX-License-Identifier: GPL-2.0-only
/*
 * ucgfiber-lcd: show the WAN download and upload rate on the front panel
 * display of the Ubiquiti UCG-Fiber.
 *
 * The panel is a DRM device (panel-mipi-dbi). The program draws into a dumb
 * buffer with plain DRM ioctls, so it needs no DRM library.
 *
 * draw.c draws the frame, see there for the layout. The rates are sampled 8
 * times per second and the display is redrawn 16 times per second, two
 * frames per sample, from one clock: the graph moves one pixel per frame,
 * the arcs glide to the mean rate of the last half second, and the two
 * values show that mean and change twice per second.
 *
 * The options -C, -o, -n and -r exist for testing on a host: with -o FILE
 * the frames are written to FILE (raw RGB565), one after the other, instead
 * of to the display.
 */

#define _GNU_SOURCE
#include <dirent.h>
#include <drm/drm.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_mode.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include "draw.h"

#define NSEC_PER_SEC	1000000000L

#define DEFAULT_DEV	"br-wan"	/* WAN device if -d is not given */
#define DEFAULT_TOP_MBIT 10000	/* top of the scale if the link speed is unknown */
#define MAX_TOP_MBIT	100000	/* limit of -m */
#define MAX_FRAMES	1000000	/* limit of -n */

#define SAMPLE_MS	125	/* time between two rate samples */
#define FRAME_NS	(SAMPLE_MS * 1000000L / FRAMES_PER_SAMPLE)	/* and between two frames */
#define SPEED_SAMPLES	(1000 / SAMPLE_MS)	/* samples between two link speed reads */
#define ARC_GLIDE_MS	140.0	/* time constant of the arcs */

/* the open panel: the DRM device, its framebuffer and the mapped pixels */
struct drm_out {
	int fd;
	uint32_t fb_id;
	struct screen scr;
};

/* set by SIGTERM and SIGINT: the main loop ends after the current frame */
static volatile sig_atomic_t stop;

/* mean of the rates the view keeps, the newest RATES samples; there is at least one */
static double view_mean(const struct view *v, int dir)
{
	double sum = 0;
	int k;

	for (k = 0; k < v->nrate; k++)
		sum += v->rate[dir][k];
	return sum / v->nrate;
}

/* ---- input: counters, link state and link speed from sysfs ---- */

/* the unsigned number at the start of a file; -1 if it cannot be read */
static int read_u64(const char *path, uint64_t *v)
{
	char b[32];
	FILE *f = fopen(path, "r");
	int ok;

	if (!f)
		return -1;
	ok = fgets(b, sizeof(b), f) != NULL;
	fclose(f);
	if (!ok)
		return -1;
	errno = 0;
	*v = strtoull(b, NULL, 10);
	return errno ? -1 : 0;
}

/* link speed of a device in Mbit/s, 0 if it is not known */
static int read_speed(const char *cls, const char *dev)
{
	char p[768];
	uint64_t v;

	snprintf(p, sizeof(p), "%s/%s/speed", cls, dev);
	/*
	 * A device without link reports -1, or the read fails. strtoull()
	 * takes the sign and wraps -1 to the largest number, so the upper
	 * limit catches it.
	 */
	if (read_u64(p, &v) || v == 0 || v >= 1000000)
		return 0;
	return (int)v;
}

/* LINK_DOWN, LINK_UP or the speed in Mbit/s */
static int read_link(const char *cls, const char *dev)
{
	char p[256];
	uint64_t v;
	int mbit;

	snprintf(p, sizeof(p), "%s/%s/carrier", cls, dev);
	if (read_u64(p, &v) || v != 1)
		return LINK_DOWN;
	mbit = read_speed(cls, dev);
	return mbit ? mbit : LINK_UP;
}

/*
 * Link speed of the WAN device in Mbit/s, 0 if it is not known. If the
 * device has no speed of its own, it may be a bridge: its ports are the
 * lower_<name> entries of its directory; take the fastest that has a link.
 */
static int wan_speed(const char *cls, const char *dev)
{
	char p[512];
	struct dirent *e;
	int best = read_speed(cls, dev), mbit;
	DIR *d;

	if (best)
		return best;
	snprintf(p, sizeof(p), "%s/%s", cls, dev);
	d = opendir(p);
	if (!d)
		return 0;
	while ((e = readdir(d))) {
		if (strncmp(e->d_name, "lower_", 6))
			continue;
		snprintf(p, sizeof(p), "%s/%s", dev, e->d_name);
		mbit = read_speed(cls, p);
		if (mbit > best)
			best = mbit;
	}
	closedir(d);
	return best;
}

/* rx + tx packets of a port, for its activity */
static int read_packets(const char *cls, const char *dev, uint64_t *pkts)
{
	char p[256];
	uint64_t rx, tx;

	snprintf(p, sizeof(p), "%s/%s/statistics/rx_packets", cls, dev);
	if (read_u64(p, &rx))
		return -1;
	snprintf(p, sizeof(p), "%s/%s/statistics/tx_packets", cls, dev);
	if (read_u64(p, &tx))
		return -1;
	*pkts = rx + tx;
	return 0;
}

/* received and sent bytes of a device; -1 if one of them cannot be read */
static int read_counters(const char *cls, const char *dev, uint64_t *rx, uint64_t *tx)
{
	char p[256];

	snprintf(p, sizeof(p), "%s/%s/statistics/rx_bytes", cls, dev);
	if (read_u64(p, rx))
		return -1;
	snprintf(p, sizeof(p), "%s/%s/statistics/tx_bytes", cls, dev);
	return read_u64(p, tx);
}

/*
 * Rate in bit/s from two readings of a byte counter that are dt seconds
 * apart. A counter that went back, as after the device was created anew,
 * gives 0: the difference of the unsigned values would be a huge number.
 */
static double rate_bps(uint64_t cur, uint64_t prev, double dt)
{
	return (cur >= prev && dt > 0) ? (double)(cur - prev) * 8.0 / dt : 0;
}

/* milliseconds from a to b */
static double ms_between(const struct timespec *a, const struct timespec *b)
{
	return (b->tv_sec - a->tv_sec) * 1e3 + (b->tv_nsec - a->tv_nsec) / 1e6;
}

/* what the sampling reads and what it keeps from one sample to the next */
struct sampler {
	const char *cls;		/* network class directory */
	const char *dev;		/* WAN device */
	int top_mbit;			/* -m: fixed top of the scale, 0: link speed */
	int fixed;			/* -r: show fixed_rate, read no counters */
	double fixed_rate[2];
	unsigned int count;		/* samples taken */
	int have_prev;
	uint64_t prev[2];		/* byte counters of the previous sample */
	struct timespec prev_t;
	int have_pkts[NPORTS];
	uint64_t pkts[NPORTS];		/* packet counters of the previous sample */
	int link[NPORTS];		/* LINK_DOWN, LINK_UP or Mbit/s */
	int active[NPORTS];		/* packets since the previous sample */
};

/* a new sample: the rates, the top of the scale, links and activity of the ports */
static void sample(struct sampler *sp, struct view *v, const struct timespec *now)
{
	double rate[2] = { 0, 0 };
	uint64_t cnt[2], pk;
	int i, mbit;

	v->valid = 0;
	if (sp->fixed) {
		rate[DOWN] = sp->fixed_rate[DOWN];
		rate[UP] = sp->fixed_rate[UP];
		v->valid = 1;
	} else if (!read_counters(sp->cls, sp->dev, &cnt[DOWN], &cnt[UP])) {
		if (sp->have_prev) {
			double dt = ms_between(&sp->prev_t, now) / 1e3;

			rate[DOWN] = rate_bps(cnt[DOWN], sp->prev[DOWN], dt);
			rate[UP] = rate_bps(cnt[UP], sp->prev[UP], dt);
			v->valid = 1;
		}
		sp->prev[DOWN] = cnt[DOWN];
		sp->prev[UP] = cnt[UP];
		sp->prev_t = *now;
		sp->have_prev = 1;
	} else {
		/* no counters: the next reading is a first one again */
		sp->have_prev = 0;
	}

	/*
	 * The top of the scale, before the sample is put on that scale. A new
	 * top holds for the columns of the graph from now on; those already in
	 * the graph stay as they are.
	 */
	if (sp->count++ % SPEED_SAMPLES == 0) {
		mbit = sp->top_mbit ? sp->top_mbit : wan_speed(sp->cls, sp->dev);
		v->top_bps = (mbit ? mbit : DEFAULT_TOP_MBIT) * 1e6;
	}
	view_push(v, rate[DOWN], rate[UP]);

	for (i = 0; i < NPORTS; i++) {
		sp->link[i] = read_link(sp->cls, ports[i].name);
		if (!read_packets(sp->cls, ports[i].name, &pk)) {
			sp->active[i] = sp->have_pkts[i] && pk != sp->pkts[i];
			sp->pkts[i] = pk;
			sp->have_pkts[i] = 1;
		} else {
			sp->have_pkts[i] = 0;
			sp->active[i] = 0;
		}
	}
}

/* ---- output: DRM dumb buffer ---- */

/*
 * Open the DRM device and show a new framebuffer on it: find the first
 * connector that has a mode and a CRTC for it, create a dumb buffer of the
 * size of that mode with 16 bits per pixel, map it and set it as the
 * framebuffer of the CRTC. The pixels are then o->scr. Returns -1 if a step
 * fails, as it does while the driver of the panel is not loaded yet.
 */
static int drm_open_panel(const char *path, struct drm_out *o)
{
	struct drm_mode_card_res res = { 0 };
	struct drm_mode_get_connector con = { 0 };
	struct drm_mode_get_encoder enc = { 0 };
	struct drm_mode_create_dumb cd = { 0 };
	struct drm_mode_map_dumb md = { 0 };
	struct drm_mode_fb_cmd2 fb = { 0 };
	struct drm_mode_crtc crtc = { 0 };
	struct drm_mode_modeinfo *modes = NULL;
	uint32_t *crtcs = NULL, *conns = NULL, *encs = NULL, conn_id, crtc_id = 0;
	uint32_t ncrtcs, nconns;
	void *map = MAP_FAILED;
	int fd, i;

	fd = open(path, O_RDWR | O_CLOEXEC);
	if (fd < 0)
		return -1;

	/* the ids of the CRTCs and connectors: ask for the counts, then for the ids */
	if (ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &res) < 0)
		goto fail;
	ncrtcs = res.count_crtcs;
	nconns = res.count_connectors;
	if (!ncrtcs || !nconns)
		goto fail;
	crtcs = calloc(ncrtcs, sizeof(*crtcs));
	conns = calloc(nconns, sizeof(*conns));
	if (!crtcs || !conns)
		goto fail;
	res.count_fbs = 0;
	res.count_encoders = 0;
	res.fb_id_ptr = res.encoder_id_ptr = 0;
	res.crtc_id_ptr = (uintptr_t)crtcs;
	res.connector_id_ptr = (uintptr_t)conns;
	if (ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &res) < 0)
		goto fail;
	/* the second call may report other counts: use what both have */
	if (res.count_crtcs < ncrtcs)
		ncrtcs = res.count_crtcs;
	if (res.count_connectors < nconns)
		nconns = res.count_connectors;
	if (!ncrtcs)
		goto fail;

	/* first connector that has a mode */
	for (i = 0; i < (int)nconns; i++) {
		memset(&con, 0, sizeof(con));
		con.connector_id = conns[i];
		if (ioctl(fd, DRM_IOCTL_MODE_GETCONNECTOR, &con) < 0 || !con.count_modes)
			continue;
		modes = calloc(con.count_modes, sizeof(*modes));
		encs = calloc(con.count_encoders ? con.count_encoders : 1, sizeof(*encs));
		if (!modes || !encs)
			goto fail;
		con.count_props = 0;
		con.props_ptr = con.prop_values_ptr = 0;
		con.modes_ptr = (uintptr_t)modes;
		con.encoders_ptr = (uintptr_t)encs;
		if (ioctl(fd, DRM_IOCTL_MODE_GETCONNECTOR, &con) < 0 || !con.count_modes)
			goto fail;
		break;
	}
	if (!modes)
		goto fail;
	conn_id = conns[i];

	/* CRTC: the one the encoder uses now, else the first it can use */
	if (con.encoder_id)
		enc.encoder_id = con.encoder_id;
	else if (con.count_encoders)
		enc.encoder_id = encs[0];
	if (enc.encoder_id && ioctl(fd, DRM_IOCTL_MODE_GETENCODER, &enc) == 0) {
		crtc_id = enc.crtc_id;
		for (i = 0; !crtc_id && i < (int)ncrtcs; i++)
			if (enc.possible_crtcs & (1u << i))
				crtc_id = crtcs[i];
	}
	if (!crtc_id)
		crtc_id = crtcs[0];

	/* the buffer for the pixels, mapped into the program */
	cd.width = modes[0].hdisplay;
	cd.height = modes[0].vdisplay;
	cd.bpp = 16;
	if (ioctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &cd) < 0)
		goto fail;
	md.handle = cd.handle;
	if (ioctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &md) < 0)
		goto fail;
	map = mmap(NULL, cd.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, md.offset);
	if (map == MAP_FAILED)
		goto fail;

	/* make the buffer a framebuffer and show it */
	fb.width = cd.width;
	fb.height = cd.height;
	fb.pixel_format = DRM_FORMAT_RGB565;
	fb.handles[0] = cd.handle;
	fb.pitches[0] = cd.pitch;
	if (ioctl(fd, DRM_IOCTL_MODE_ADDFB2, &fb) < 0)
		goto fail;

	crtc.crtc_id = crtc_id;
	crtc.fb_id = fb.fb_id;
	crtc.set_connectors_ptr = (uintptr_t)&conn_id;
	crtc.count_connectors = 1;
	crtc.mode = modes[0];
	crtc.mode_valid = 1;
	if (ioctl(fd, DRM_IOCTL_MODE_SETCRTC, &crtc) < 0)
		goto fail;

	o->fd = fd;
	o->fb_id = fb.fb_id;
	o->scr.px = map;
	o->scr.w = cd.width;
	o->scr.h = cd.height;
	o->scr.pitch = cd.pitch / 2;
	free(crtcs);
	free(conns);
	free(encs);
	free(modes);
	return 0;
fail:
	if (map != MAP_FAILED)
		munmap(map, cd.size);
	free(crtcs);
	free(conns);
	free(encs);
	free(modes);
	close(fd);
	return -1;
}

/*
 * Send the frame to the panel: the driver transfers the buffer when it is
 * told that the buffer has changed. ENOSYS comes from a driver that needs
 * no such notice.
 */
static void drm_present(struct drm_out *o)
{
	struct drm_mode_fb_dirty_cmd d = { .fb_id = o->fb_id };

	if (ioctl(o->fd, DRM_IOCTL_MODE_DIRTYFB, &d) < 0 && errno != ENOSYS) {
		static int warned;

		if (!warned++)
			fprintf(stderr, "ucgfiber-lcd: DIRTYFB: %s\n", strerror(errno));
	}
}

/* -o: add the frame to the file */
static int write_frame(FILE *f, const struct screen *s)
{
	size_t n = fwrite(s->px, sizeof(uint16_t), PANEL_W * PANEL_H, f);

	return n == PANEL_W * PANEL_H && !fflush(f) ? 0 : -1;
}

/* SIGTERM, SIGINT */
static void on_signal(int sig)
{
	(void)sig;
	stop = 1;
}

/* the usage text: to stdout for -h, to stderr after a bad option */
static void show_usage(FILE *f)
{
	fprintf(f,
		"usage: ucgfiber-lcd [-d DEV] [-m MBIT] [-D DRMDEV] [-h]\n"
		"                    [-C CLASSDIR] [-o FILE] [-n FRAMES] [-r DOWN,UP]\n"
		"  -d DEV      WAN network device (default " DEFAULT_DEV ")\n"
		"  -m MBIT     rate for a full ring and the top of the graph in Mbit/s,\n"
		"              1 to %d (default: the link speed of DEV, or %d if\n"
		"              that is unknown); the scale is the square root of the rate\n"
		"  -D DRMDEV   DRM device (default /dev/dri/card0)\n"
		"  -h          show this text\n"
		"for testing on a host:\n"
		"  -C CLASSDIR network class directory (default /sys/class/net)\n"
		"  -o FILE     write the frames (raw RGB565, %dx%d each) to FILE, not to\n"
		"              the display\n"
		"  -n FRAMES   stop after FRAMES frames, 1 to %d (default: run until\n"
		"              stopped)\n"
		"  -r DOWN,UP  show fixed rates in bit/s, 0 or more, instead of reading\n"
		"              counters\n",
		MAX_TOP_MBIT, DEFAULT_TOP_MBIT, PANEL_W, PANEL_H, MAX_FRAMES);
}

/* value of a numeric option; a bad value prints the usage text and ends the program */
static long num_arg(const char *arg, long min, long max)
{
	char *end;
	long v;

	errno = 0;
	v = strtol(arg, &end, 10);
	if (errno || end == arg || *end || v < min || v > max) {
		show_usage(stderr);
		exit(2);
	}
	return v;
}

int main(int argc, char **argv)
{
	const char *drm = "/dev/dri/card0", *outfile = NULL;
	static struct view view;
	struct sampler sp = { .cls = "/sys/class/net", .dev = DEFAULT_DEV };
	struct drm_out out = { .fd = -1 };
	struct screen file_scr = { .w = PANEL_W, .h = PANEL_H, .pitch = PANEL_W };
	struct screen *scr;
	struct timespec next, now;
	unsigned long frames = 0, tick;
	FILE *outf = NULL;
	int opt, i, phase, warned = 0, arc_snap = 1;
	double glide;
	char tail;

	while ((opt = getopt(argc, argv, "d:m:D:C:o:n:r:h")) != -1) {
		switch (opt) {
		case 'd':
			sp.dev = optarg;
			break;
		case 'm':
			sp.top_mbit = num_arg(optarg, 1, MAX_TOP_MBIT);
			break;
		case 'D':
			drm = optarg;
			break;
		case 'C':
			sp.cls = optarg;
			break;
		case 'o':
			outfile = optarg;
			break;
		case 'n':
			frames = num_arg(optarg, 1, MAX_FRAMES);
			break;
		case 'r':
			/* two numbers and nothing after them; !(x >= 0) also catches NaN */
			if (sscanf(optarg, "%lf,%lf%c", &sp.fixed_rate[DOWN], &sp.fixed_rate[UP],
				   &tail) != 2 ||
			    !(sp.fixed_rate[DOWN] >= 0) || !(sp.fixed_rate[UP] >= 0)) {
				show_usage(stderr);
				return 2;
			}
			sp.fixed = 1;
			break;
		case 'h':
			show_usage(stdout);
			return 0;
		default:
			show_usage(stderr);
			return 2;
		}
	}

	signal(SIGTERM, on_signal);
	signal(SIGINT, on_signal);
	draw_init();

	/* the pixels: memory for -o, else the panel, which may appear only later */
	if (outfile) {
		file_scr.px = calloc(PANEL_W * PANEL_H, sizeof(uint16_t));
		outf = fopen(outfile, "wb");
		if (!file_scr.px || !outf)
			return 1;
		scr = &file_scr;
	} else {
		while (drm_open_panel(drm, &out) < 0) {
			if (stop)
				return 1;
			if (!warned++)
				fprintf(stderr, "ucgfiber-lcd: waiting for %s\n", drm);
			sleep(2);
		}
		scr = &out.scr;
	}

	/*
	 * The arcs do not jump directly to a new value: in each frame they go
	 * this share of the remaining way. Derived from the frame time, so the
	 * motion takes the same time whatever the frame rate is.
	 */
	glide = 1 - exp(-(FRAME_NS / 1e6) / ARC_GLIDE_MS);

	/*
	 * One clock for all: a frame every FRAME_NS, a sample every
	 * FRAMES_PER_SAMPLE frames. The graph moves one column per frame, so
	 * what it shows of a sample only ever moves and does not change.
	 */
	clock_gettime(CLOCK_MONOTONIC, &next);
	for (tick = 0; !stop; tick++) {
		uint16_t marks[NPORTS];

		if (tick % FRAMES_PER_SAMPLE == 0) {
			clock_gettime(CLOCK_MONOTONIC, &now);
			sample(&sp, &view, &now);
		}

		/* the values: the mean of the newest RATES samples, changed once in that time */
		if (tick % (FRAMES_PER_SAMPLE * RATES) == 0)
			for (i = 0; i < 2; i++)
				view.num[i] = view_mean(&view, i);

		/* the arcs glide to the same mean; the first measured rate is shown at once */
		for (i = 0; i < 2; i++) {
			double target = view.valid ?
				scale_frac(view_mean(&view, i), view.top_bps) : 0;

			if (arc_snap)
				view.arc[i] = target;
			else
				view.arc[i] += (target - view.arc[i]) * glide;
		}
		if (view.valid)
			arc_snap = 0;

		view.phase = tick % FRAMES_PER_SAMPLE;
		/* port marks: the blink phase changes with every sample */
		phase = (tick / FRAMES_PER_SAMPLE) & 1;
		for (i = 0; i < NPORTS; i++)
			marks[i] = mark_color(sp.link[i], sp.active[i], phase);

		draw_frame(scr, &view, marks);
		if (!outfile)
			drm_present(&out);
		else if (write_frame(outf, scr))
			return 1;

		if (frames && tick + 1 >= frames)
			break;
		/* sleep until the next frame is due: a fixed time grid, so no drift */
		next.tv_nsec += FRAME_NS;
		if (next.tv_nsec >= NSEC_PER_SEC) {
			next.tv_nsec -= NSEC_PER_SEC;
			next.tv_sec++;
		}
		/* after a stall, do not draw frames without a pause to catch up */
		clock_gettime(CLOCK_MONOTONIC, &now);
		if (ms_between(&now, &next) < 0)
			next = now;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
	}

	/* what -o took; the panel keeps showing the last frame */
	if (outf)
		fclose(outf);
	free(file_scr.px);
	return 0;
}
