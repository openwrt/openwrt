// SPDX-License-Identifier: GPL-2.0-only
/*
 * ucgfiber-lcd: show the WAN download and upload rate on the front panel
 * display of the Ubiquiti UCG-Fiber.
 *
 * The panel is a DRM device (panel-mipi-dbi). The program draws into a dumb
 * buffer with plain DRM ioctls, so it needs no library. Two rows, each with
 * an arrow, the rate of the last second and a bar that is filled by the rate
 * relative to the fastest known WAN link speed.
 *
 * Between the bars, one mark per port shows the link speed by colour. Like
 * a port LED, the mark of a port that has traffic blinks: the marks are
 * checked every TICK_MS and an active port's mark is off on every other
 * tick. Only the strip with the marks is sent to the panel between the
 * once-per-second full frames.
 *
 * With -o FILE the frame is written to FILE (raw RGB565, 160x80) instead of
 * the display; this is for testing on a host.
 */

#define _GNU_SOURCE
#include <dirent.h>
#include <drm/drm.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_mode.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define RGB(r, g, b) ((uint16_t)((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | ((b) >> 3)))

#define COL_BG		RGB(8, 16, 100)		/* deep blue, as on stock */
#define COL_TEXT	RGB(255, 255, 255)
#define COL_FRAME	RGB(255, 255, 255)
#define COL_DOWN	RGB(40, 80, 225)
#define COL_UP		RGB(0, 150, 235)

#define DEFAULT_MAX_MBIT	1000

#define COL_LINK_FAST	RGB(150, 255, 70)	/* 2.5 Gbit/s and more */
#define COL_LINK_MID	RGB(255, 170, 0)	/* 1 Gbit/s */
#define COL_LINK_SLOW	RGB(255, 60, 60)	/* below 1 Gbit/s */
#define COL_LINK_DOWN	RGB(28, 44, 130)	/* no link: dim */

#define TICK_MS		100	/* port marks */
#define TICKS_PER_FRAME	10	/* rates and bars: once per second */

#define BAR_H		24
#define BAR_GAP		11
#define NPORTS		7

struct screen {
	uint16_t *px;
	unsigned int w, h, pitch;	/* pitch in pixels */
};

struct drm_out {
	int fd;
	uint32_t fb_id;
	size_t size;
	struct screen scr;
};

static volatile sig_atomic_t stop;

/*
 * Font at native size, as on stock: digits and capitals 7 pixels wide and 10
 * high. A glyph is up to 12 rows of '#' and '.'; the width is the length of
 * the first row. Row 9 is the baseline, 'p' reaches down to row 11.
 */
struct glyph {
	char c;
	const char *row[12];
};

static const struct glyph font[] = {
	{ '0', { ".#####.", "##...##", "##...##", "##...##", "##...##",
		 "##...##", "##...##", "##...##", "##...##", ".#####." } },
	{ '1', { "...##..", "..###..", ".####..", "...##..", "...##..",
		 "...##..", "...##..", "...##..", "...##..", ".######" } },
	{ '2', { ".#####.", "##...##", ".....##", ".....##", "....##.",
		 "...##..", "..##...", ".##....", "##.....", "#######" } },
	{ '3', { ".#####.", "##...##", ".....##", ".....##", "..####.",
		 ".....##", ".....##", ".....##", "##...##", ".#####." } },
	{ '4', { "....##.", "...###.", "..####.", ".##.##.", "##..##.",
		 "#######", "....##.", "....##.", "....##.", "....##." } },
	{ '5', { "#######", "##.....", "##.....", "##.....", "######.",
		 ".....##", ".....##", ".....##", "##...##", ".#####." } },
	{ '6', { "..####.", ".##....", "##.....", "##.....", "######.",
		 "##...##", "##...##", "##...##", "##...##", ".#####." } },
	{ '7', { "#######", ".....##", ".....##", "....##.", "....##.",
		 "...##..", "...##..", "..##...", "..##...", "..##..." } },
	{ '8', { ".#####.", "##...##", "##...##", "##...##", ".#####.",
		 "##...##", "##...##", "##...##", "##...##", ".#####." } },
	{ '9', { ".#####.", "##...##", "##...##", "##...##", ".######",
		 ".....##", ".....##", ".....##", "....##.", ".####.." } },
	{ '.', { "..", "..", "..", "..", "..", "..", "..", "..", "##", "##" } },
	{ '-', { ".....", ".....", ".....", ".....", "#####", "#####" } },
	{ ' ', { "..." } },
	{ 'K', { "##...##", "##..##.", "##.##..", "####...", "###....",
		 "####...", "##.##..", "##..##.", "##...##", "##...##" } },
	{ 'M', { "##...##", "###.###", "#######", "#######", "##.#.##",
		 "##...##", "##...##", "##...##", "##...##", "##...##" } },
	{ 'G', { ".#####.", "##...##", "##.....", "##.....", "##.####",
		 "##...##", "##...##", "##...##", "##...##", ".#####." } },
	{ 'b', { "##.....", "##.....", "##.....", "##.###.", "###..##",
		 "##...##", "##...##", "##...##", "###..##", "##.###." } },
	{ 'p', { ".......", ".......", ".......", "##.###.", "###..##",
		 "##...##", "##...##", "##...##", "###..##", "##.###.",
		 "##.....", "##....." } },
	{ 's', { ".......", ".......", ".......", ".#####.", "##...##",
		 "##.....", ".#####.", ".....##", "##...##", ".#####." } },
	{ '^', { ".........", ".........", ".........", "....#....", "...###...",
		 "..#####..", ".#######.", "#########", ".........", "........." } },	/* up triangle */
	{ 'v', { ".........", ".........", ".........", "#########", ".#######.",
		 "..#####..", "...###...", "....#....", ".........", "........." } },	/* down triangle */
};

static void fill_rect(struct screen *s, int x, int y, int w, int h, uint16_t c)
{
	int i, j;

	for (j = y; j < y + h; j++) {
		if (j < 0 || j >= (int)s->h)
			continue;
		for (i = x; i < x + w; i++)
			if (i >= 0 && i < (int)s->w)
				s->px[j * s->pitch + i] = c;
	}
}

/* draw a glyph, return the advance to the next one (width and a 1 pixel gap) */
static int put_glyph(struct screen *s, int x, int y, char c, uint16_t col)
{
	unsigned int n, r, b;

	for (n = 0; n < sizeof(font) / sizeof(font[0]); n++) {
		if (font[n].c != c)
			continue;
		for (r = 0; r < 12 && font[n].row[r]; r++)
			for (b = 0; font[n].row[r][b]; b++)
				if (font[n].row[r][b] == '#')
					fill_rect(s, x + b, y + r, 1, 1, col);
		return (int)strlen(font[n].row[0]) + 1;
	}
	return 0;
}

static void put_text(struct screen *s, int x, int y, const char *t, uint16_t col)
{
	for (; *t; t++)
		x += put_glyph(s, x, y, *t, col);
}

static int text_width(const char *t)
{
	unsigned int n;
	int w = 0;

	for (; *t; t++)
		for (n = 0; n < sizeof(font) / sizeof(font[0]); n++)
			if (font[n].c == *t)
				w += (int)strlen(font[n].row[0]) + 1;
	return w ? w - 1 : 0;
}

/* rate as value and unit: 0 bps, 999 bps, 1.50 Kbps, 12.3 Mbps, 940 Mbps, 1.50 Gbps */
static void fmt_rate(double bps, char *val, size_t vn, char *unit, size_t un)
{
	static const char * const names[] = { "bps", "Kbps", "Mbps", "Gbps" };
	int u = 0;
	double v = bps;

	while (v >= 999.5 && u < 3) {
		v /= 1000;
		u++;
	}
	if (u == 0)
		snprintf(val, vn, "%.0f", v);
	else if (v < 9.995)
		snprintf(val, vn, "%.2f", v);
	else if (v < 99.95)
		snprintf(val, vn, "%.1f", v);
	else
		snprintf(val, vn, "%.0f", v);
	snprintf(unit, un, "%s", names[u]);
}

static int in_rrect(int x, int y, int rx, int ry, int rw, int rh, int r)
{
	int cx, cy, dx, dy;

	if (x < rx || x >= rx + rw || y < ry || y >= ry + rh)
		return 0;
	cx = x < rx + r ? rx + r : (x >= rx + rw - r ? rx + rw - 1 - r : x);
	cy = y < ry + r ? ry + r : (y >= ry + rh - r ? ry + rh - 1 - r : y);
	dx = x - cx;
	dy = y - cy;
	return dx * dx + dy * dy <= r * r;
}

/*
 * Coverage 0..16 of a rounded rectangle in the pixel (px, py), from 4x4
 * samples; the geometry is scaled by 8. Samples at or right of clip_x are
 * left out.
 */
static int rr_cov(int px, int py, int rx, int ry, int rw, int rh, int r, int clip_x)
{
	int sx, sy, n = 0;

	for (sy = 0; sy < 4; sy++) {
		for (sx = 0; sx < 4; sx++) {
			int X = px * 8 + 2 * sx + 1, Y = py * 8 + 2 * sy + 1;

			if (X < clip_x * 8 &&
			    in_rrect(X, Y, rx * 8, ry * 8, rw * 8, rh * 8, r * 8))
				n++;
		}
	}
	return n;
}

/* mix: a/16 of src over dst */
static uint16_t blend(uint16_t dst, uint16_t src, int a)
{
	int dr = ((dst >> 11) & 31) * 255 / 31, dg = ((dst >> 5) & 63) * 255 / 63;
	int db = (dst & 31) * 255 / 31;
	int sr = ((src >> 11) & 31) * 255 / 31, sg = ((src >> 5) & 63) * 255 / 63;
	int sb = (src & 31) * 255 / 31;

	if (a <= 0)
		return dst;
	if (a >= 16)
		return src;
	return RGB((dr * (16 - a) + sr * a) / 16, (dg * (16 - a) + sg * a) / 16,
		   (db * (16 - a) + sb * a) / 16);
}

/*
 * One bar, 90 percent of the screen width: white frame, fill in the colour
 * of the KPI up to the fraction. Triangle at the left; the unit is right
 * aligned at the end of the bar in a field as wide as "Mbps", the value is
 * right aligned in front of it, so nothing moves when the numbers change.
 */
static void draw_bar(struct screen *s, int row, char mark, const char *val,
		     const char *unit, double frac, uint16_t col)
{
	int bx = s->w / 20, bw = s->w - 2 * bx;
	int bh = BAR_H, r = 10, gap = BAR_GAP, by = (s->h - (2 * bh + gap)) / 2 + row * (bh + gap);
	int fx = bx + 3, fw = bw - 6, fill, x, y;
	int ty = by + (bh - 10) / 2, rx = bx + bw - 12;

	if (frac < 0)
		frac = 0;
	if (frac > 1)
		frac = 1;
	fill = (int)(fw * frac);
	if (frac > 0 && fill < 1)
		fill = 1;	/* any traffic shows */

	for (y = by; y < by + bh; y++) {
		for (x = bx; x < bx + bw; x++) {
			uint16_t *px = &s->px[y * s->pitch + x];
			int a;

			/* fill first, then the 1 pixel frame on top; both smoothed */
			a = rr_cov(x, y, bx + 3, by + 3, bw - 6, bh - 6, r - 3, fx + fill);
			*px = blend(*px, col, a);
			a = rr_cov(x, y, bx, by, bw, bh, r, 1 << 20) -
			    rr_cov(x, y, bx + 1, by + 1, bw - 2, bh - 2, r - 1, 1 << 20);
			*px = blend(*px, COL_FRAME, a);
		}
	}
	put_glyph(s, bx + 9, ty, mark, COL_TEXT);
	put_text(s, rx - text_width(unit), ty, unit, COL_TEXT);
	put_text(s, rx - text_width("Mbps") - 5 - text_width(val), ty, val, COL_TEXT);
}

/* network devices in the order the marks are drawn */
static const char * const ports[NPORTS] = {
	"sfp1", "sfp2", "wan", "lan1", "lan2", "lan3", "lan4",
};

static int link_size = 4;

static int read_u64(const char *path, uint64_t *v);

/* 0 down, 1 up with unknown speed, else the speed in Mbit/s */
static int read_link(const char *cls, const char *dev)
{
	char p[256];
	uint64_t v;

	snprintf(p, sizeof(p), "%s/%s/carrier", cls, dev);
	if (read_u64(p, &v) || v != 1)
		return 0;
	snprintf(p, sizeof(p), "%s/%s/speed", cls, dev);
	if (read_u64(p, &v) || v == 0 || v >= 1000000)
		return 1;
	return (int)v;
}

static uint16_t link_color(int mbit)
{
	if (mbit <= 0)
		return COL_LINK_DOWN;
	if (mbit == 1 || mbit >= 2500)
		return COL_LINK_FAST;
	return mbit >= 1000 ? COL_LINK_MID : COL_LINK_SLOW;
}

/* colour of a mark: off on every other tick while the port has traffic */
static uint16_t mark_color(int mbit, int active, int phase)
{
	if (mbit > 0 && active && phase)
		return COL_BG;
	return link_color(mbit);
}

static const int mark_off[NPORTS] = { 0, 12, 32, 52, 64, 76, 88 };

/* strip with the marks, centred in the gap between the two bars */
static void links_rect(const struct screen *s, int *x0, int *y0, int *w, int *h)
{
	int top = ((int)s->h - (2 * BAR_H + BAR_GAP)) / 2;

	*w = mark_off[NPORTS - 1] + link_size;
	*h = link_size;
	*x0 = ((int)s->w - *w) / 2;
	*y0 = top + BAR_H + (BAR_GAP - link_size) / 2;
}

static void draw_links(struct screen *s, const uint16_t *col)
{
	int sz = link_size, i, x, y, x0, y0, w, h;

	links_rect(s, &x0, &y0, &w, &h);
	fill_rect(s, x0, y0, w, h, COL_BG);
	for (i = 0; i < NPORTS; i++) {
		uint16_t c = col[i];

		for (y = 0; y < sz; y++) {
			for (x = 0; x < sz; x++) {
				int dx = 2 * x + 1 - sz, dy = 2 * y + 1 - sz;

				if (dx * dx + dy * dy <= sz * sz)
					fill_rect(s, x0 + mark_off[i] + x, y0 + y, 1, 1, c);
			}
		}
	}
}

static void draw_frame(struct screen *s, int valid, double down, double up, double max_bps,
		       const uint16_t *marks)
{
	char v[16], u[8];

	fill_rect(s, 0, 0, s->w, s->h, COL_BG);
	if (valid) {
		fmt_rate(down, v, sizeof(v), u, sizeof(u));
	} else {
		strcpy(v, "-");
		u[0] = 0;
	}
	draw_bar(s, 0, 'v', v, u, valid ? down / max_bps : 0, COL_DOWN);
	if (valid) {
		fmt_rate(up, v, sizeof(v), u, sizeof(u));
	} else {
		strcpy(v, "-");
		u[0] = 0;
	}
	draw_bar(s, 1, '^', v, u, valid ? up / max_bps : 0, COL_UP);
	draw_links(s, marks);
}

/* ---- WAN counters and link speed ---- */

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

static int read_counters(const char *cls, const char *dev, uint64_t *rx, uint64_t *tx)
{
	char p[256];

	snprintf(p, sizeof(p), "%s/%s/statistics/rx_bytes", cls, dev);
	if (read_u64(p, rx))
		return -1;
	snprintf(p, sizeof(p), "%s/%s/statistics/tx_bytes", cls, dev);
	return read_u64(p, tx);
}

static double rate_bps(uint64_t cur, uint64_t prev, double dt)
{
	return (cur >= prev && dt > 0) ? (double)(cur - prev) * 8.0 / dt : 0;
}

/* fastest link of the device and of its bridge ports, in bit/s; 0 if unknown */
static double link_speed_bps(const char *cls, const char *dev)
{
	char p[640], dir[256];
	struct dirent *e;
	uint64_t v, best = 0;
	DIR *d;

	snprintf(p, sizeof(p), "%s/%s/speed", cls, dev);
	if (!read_u64(p, &v) && v < 1000000)
		best = v;
	snprintf(dir, sizeof(dir), "%s/%s", cls, dev);
	d = opendir(dir);
	if (!d)
		return best * 1e6;
	while ((e = readdir(d))) {
		if (strncmp(e->d_name, "lower_", 6))
			continue;
		snprintf(p, sizeof(p), "%s/%s/speed", dir, e->d_name);
		if (!read_u64(p, &v) && v < 1000000 && v > best)
			best = v;
	}
	closedir(d);
	return best * 1e6;
}

/* ---- output: DRM dumb buffer ---- */

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
	void *map;
	int fd, i;

	fd = open(path, O_RDWR | O_CLOEXEC);
	if (fd < 0)
		return -1;

	if (ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &res) < 0)
		goto fail;
	if (!res.count_crtcs || !res.count_connectors)
		goto fail;
	crtcs = calloc(res.count_crtcs, sizeof(*crtcs));
	conns = calloc(res.count_connectors, sizeof(*conns));
	if (!crtcs || !conns)
		goto fail;
	res.count_fbs = 0;
	res.count_encoders = 0;
	res.fb_id_ptr = res.encoder_id_ptr = 0;
	res.crtc_id_ptr = (uintptr_t)crtcs;
	res.connector_id_ptr = (uintptr_t)conns;
	if (ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &res) < 0)
		goto fail;

	/* first connector that has a mode */
	for (i = 0; i < (int)res.count_connectors; i++) {
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
	if (con.encoder_id) {
		enc.encoder_id = con.encoder_id;
	} else if (con.count_encoders) {
		enc.encoder_id = encs[0];
	}
	if (enc.encoder_id && ioctl(fd, DRM_IOCTL_MODE_GETENCODER, &enc) == 0) {
		crtc_id = enc.crtc_id;
		for (i = 0; !crtc_id && i < (int)res.count_crtcs; i++)
			if (enc.possible_crtcs & (1u << i))
				crtc_id = crtcs[i];
	}
	if (!crtc_id)
		crtc_id = crtcs[0];

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
	o->size = cd.size;
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
	free(crtcs);
	free(conns);
	free(encs);
	free(modes);
	close(fd);
	return -1;
}

/* send the frame, or only the clip rectangle of it, to the panel */
static void drm_present(struct drm_out *o, const struct drm_clip_rect *clip)
{
	struct drm_mode_fb_dirty_cmd d = { .fb_id = o->fb_id };

	if (clip) {
		d.clips_ptr = (uintptr_t)clip;
		d.num_clips = 1;
	}

	if (ioctl(o->fd, DRM_IOCTL_MODE_DIRTYFB, &d) < 0 && errno != ENOSYS) {
		static int warned;

		if (!warned++)
			fprintf(stderr, "ucgfiber-lcd: DIRTYFB: %s\n", strerror(errno));
	}
}

static void on_signal(int sig)
{
	(void)sig;
	stop = 1;
}

static void usage(void)
{
	fprintf(stderr,
		"usage: ucgfiber-lcd [-d DEV] [-m MBIT] [-D DRMDEV] [-C CLASSDIR]\n"
		"                    [-o FILE] [-n TICKS] [-r DOWN,UP] [-S MBIT,...] [-a MASK] [-z SIZE]\n"
		"  -d DEV      WAN network device (default br-wan)\n"
		"  -m MBIT     rate for a full bar in Mbit/s (default: fastest WAN link, else %d)\n"
		"  -D DRMDEV   DRM device (default /dev/dri/card0)\n"
		"  -C CLASSDIR network class directory (default /sys/class/net)\n"
		"  -o FILE     write the frame (raw RGB565 160x80) to FILE, not the display\n"
		"  -n TICKS    stop after TICKS updates of %d ms (default: run until stopped)\n"
		"  -r DOWN,UP  show fixed rates in bit/s instead of reading counters\n"
		"  -S L,L,L,L,L,L,L  show fixed link speeds in Mbit/s (0 = down) for sfp1, sfp2,\n"
		"              wan, lan1 to lan4 instead of reading them\n"
		"  -a MASK     fixed port activity, bit 0 = sfp1 ... bit 6 = lan4 (for tests)\n"
		"  -z SIZE     size of the link marks in pixels (default 4)\n",
		DEFAULT_MAX_MBIT, TICK_MS);
}

int main(int argc, char **argv)
{
	const char *dev = "br-wan", *drm = "/dev/dri/card0", *cls = "/sys/class/net";
	const char *outfile = NULL;
	double fixed_down = -1, fixed_up = -1, max_bps = 0;
	unsigned long ticks = 0, n = 0;
	struct drm_out out = { .fd = -1 };
	struct screen file_scr = { 0 };
	struct screen *scr;
	struct timespec next, now, prev_t = { 0 };
	uint64_t prx = 0, ptx = 0;
	int have_prev = 0, c, warned = 0, fixed_on = 0, fixed_link[NPORTS] = { 0 };
	int fixed_act = -1, have_pkts[NPORTS] = { 0 }, link[NPORTS] = { 0 };
	uint64_t ppkts[NPORTS] = { 0 };
	uint16_t shown[NPORTS] = { 0 };
	double down = 0, up = 0, mx = 0;
	int valid = 0;

	while ((c = getopt(argc, argv, "d:m:D:C:o:n:r:S:a:z:h")) != -1) {
		switch (c) {
		case 'd': dev = optarg; break;
		case 'm': max_bps = atof(optarg) * 1e6; break;
		case 'D': drm = optarg; break;
		case 'C': cls = optarg; break;
		case 'o': outfile = optarg; break;
		case 'n': ticks = strtoul(optarg, NULL, 10); break;
		case 'a': fixed_act = (int)strtol(optarg, NULL, 0); break;
		case 'S':
			if (sscanf(optarg, "%d,%d,%d,%d,%d,%d,%d", &fixed_link[0], &fixed_link[1],
				   &fixed_link[2], &fixed_link[3], &fixed_link[4], &fixed_link[5],
				   &fixed_link[6]) != NPORTS) {
				usage();
				return 2;
			}
			fixed_on = 1;
			break;
		case 'z':
			link_size = atoi(optarg);
			if (link_size < 2 || link_size > 9)
				link_size = 4;
			break;
		case 'r':
			if (sscanf(optarg, "%lf,%lf", &fixed_down, &fixed_up) != 2) {
				usage();
				return 2;
			}
			break;
		default:
			usage();
			return c == 'h' ? 0 : 2;
		}
	}

	signal(SIGTERM, on_signal);
	signal(SIGINT, on_signal);

	if (outfile) {
		file_scr.w = 160;
		file_scr.h = 80;
		file_scr.pitch = 160;
		file_scr.px = calloc(160 * 80, sizeof(uint16_t));
		if (!file_scr.px)
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

	clock_gettime(CLOCK_MONOTONIC, &next);
	while (!stop) {
		int full = (n % TICKS_PER_FRAME) == 0, phase = n & 1, changed = 0;
		uint16_t marks[NPORTS];
		double dt, speed;
		uint64_t rx, tx;

		if (full) {
			clock_gettime(CLOCK_MONOTONIC, &now);
			valid = 0;
			if (fixed_down >= 0) {
				down = fixed_down;
				up = fixed_up;
				valid = 1;
			} else if (!read_counters(cls, dev, &rx, &tx)) {
				dt = (now.tv_sec - prev_t.tv_sec) + (now.tv_nsec - prev_t.tv_nsec) / 1e9;
				if (have_prev) {
					down = rate_bps(rx, prx, dt);
					up = rate_bps(tx, ptx, dt);
					valid = 1;
				}
				prx = rx;
				ptx = tx;
				prev_t = now;
				have_prev = 1;
			} else {
				have_prev = 0;
			}

			mx = max_bps;
			if (mx <= 0 && (speed = link_speed_bps(cls, dev)) > 0)
				mx = speed;
			if (mx <= 0)
				mx = DEFAULT_MAX_MBIT * 1e6;
		}

		for (c = 0; c < NPORTS; c++) {
			int active = 0;
			uint64_t pk;

			link[c] = fixed_on ? fixed_link[c] : read_link(cls, ports[c]);
			if (fixed_act >= 0) {
				active = (fixed_act >> c) & 1;
			} else if (!read_packets(cls, ports[c], &pk)) {
				active = have_pkts[c] && pk != ppkts[c];
				ppkts[c] = pk;
				have_pkts[c] = 1;
			} else {
				have_pkts[c] = 0;
			}
			marks[c] = mark_color(link[c], active, phase);
			if (marks[c] != shown[c])
				changed = 1;
		}

		if (full) {
			draw_frame(scr, valid, down, up, mx, marks);
			if (!outfile)
				drm_present(&out, NULL);
		} else if (changed) {
			struct drm_clip_rect clip;
			int x0, y0, w, h;

			draw_links(scr, marks);
			links_rect(scr, &x0, &y0, &w, &h);
			clip.x1 = x0;
			clip.y1 = y0;
			clip.x2 = x0 + w;
			clip.y2 = y0 + h;
			if (!outfile)
				drm_present(&out, &clip);
		}
		memcpy(shown, marks, sizeof(shown));

		if (outfile && (full || changed)) {
			FILE *f = fopen(outfile, "wb");

			if (!f || fwrite(scr->px, sizeof(uint16_t), 160 * 80, f) != 160 * 80)
				return 1;
			fclose(f);
		}

		if (ticks && ++n >= ticks)
			break;
		if (!ticks)
			n++;
		next.tv_nsec += TICK_MS * 1000000L;
		if (next.tv_nsec >= 1000000000L) {
			next.tv_nsec -= 1000000000L;
			next.tv_sec++;
		}
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
	}
	return 0;
}
