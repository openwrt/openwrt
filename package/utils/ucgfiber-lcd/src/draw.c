// SPDX-License-Identifier: GPL-2.0-only
/*
 * ucgfiber-lcd: draws a frame into a buffer of pixels. Nothing here knows
 * where the rates come from or where the pixels go.
 *
 * Layout, on black:
 * - The OpenWrt logo right of the rings, above the axis of the graph, as a
 *   faint, still background; everything else is drawn over it.
 * - A graph of the rates of the last 10 seconds over the full width, two
 *   pixel columns per sample, newest at the right: upload above the middle
 *   axis, download below it, each a line with a faint fill towards the axis.
 * - Two rings around a common centre at the left: the outer one for
 *   download, the inner one for upload. The rings are open at the bottom;
 *   the arc of a ring starts left of the gap and runs clockwise, its length
 *   follows the current rate. The two rates are written inside the rings,
 *   upload above download, each number in the colour of its ring; a triangle
 *   for each stands in the gap, in the same order.
 * - Graph and arcs use the square root of the rate. The top of the scale is
 *   the link speed of the WAN device, or the rate given with -m. A tenth of
 *   the top fills about a third, a hundredth a tenth; a mark on the rings
 *   shows each of the two.
 * - At the bottom, one mark per port shows the link speed by colour. Like a
 *   port LED, the mark of a port that has traffic blinks.
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "draw.h"
#include "font.h"
#include "logo.h"

#define ARRAY_SIZE(a)	(sizeof(a) / sizeof((a)[0]))

#define RGB(r, g, b) ((uint16_t)((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | ((b) >> 3)))

#define COL_BG		RGB(0, 0, 0)		/* black: the colours stand out best */
#define COL_AXIS	RGB(255, 255, 255)	/* axis of the graph, drawn faintly */
#define COL_DOWN	RGB(91, 140, 255)	/* blue */
#define COL_UP		RGB(255, 154, 46)	/* orange */

/* port marks */
#define COL_LINK_FAST	RGB(150, 255, 70)	/* 2.5 Gbit/s and more, or speed unknown */
#define COL_LINK_MID	RGB(255, 170, 0)	/* 1 Gbit/s */
#define COL_LINK_SLOW	RGB(255, 60, 60)	/* below 1 Gbit/s */
#define COL_LINK_DOWN	RGB(40, 44, 60)	/* no link: dim */

#define COL_LOGO_R	0	/* the cyan of the logo */
#define COL_LOGO_G	181
#define COL_LOGO_B	226

/* opacities in sixteenths, see blend() */
#define A_SOLID		16
#define A_FILL		4	/* graph: area between the line and the axis */
#define A_AXIS		5	/* graph: axis */
#define A_CORNER	8	/* graph: pixels that soften a step of the line */
#define A_DISC		14	/* black disc under the rings */
#define A_TRACK		6	/* rings: the part beyond the arc */
#define A_TICK		8	/* rings: black scale marks */
#define A_PATCH		11	/* black patch under the port marks */

#define LOGO_PCT	40		/* opacity of the logo in percent */
#define LOGO_FULL	(255 * 100)	/* coverage times percent of a solid pixel */

static const uint16_t dir_col[2] = { [DOWN] = COL_DOWN, [UP] = COL_UP };

/*
 * The layout. For a panel of another size, change this block, the table of
 * the ports and the size in draw.h; nothing else depends on the size.
 */
#define AXIS_Y		(PANEL_H / 2)	/* row of the graph axis */
#define GRAPH_H		(AXIS_Y - 5)	/* rows from the axis to the top of the scale */

/* rings: centre on a pixel corner; radii and half width in 1/8 pixel */
#define RING_CX		(PANEL_H / 2)	/* as far from the left border as from the top */
#define RING_CY		(PANEL_H / 2)
#define RING_OUT8	208	/* middle of the outer ring: 26 pixels */
#define RING_IN8	168	/* middle of the inner ring: 21 pixels */
#define RING_HW8	16	/* half of the ring width of 4 pixels */
#define DISC_R		30	/* dimmed disc under the rings */
#define TEXT_UP_Y	(RING_CY - 11)	/* top row of the upload value */
#define TEXT_DOWN_Y	(RING_CY + 1)
#define UNIT_X		(RING_CX + 11)	/* left column of the unit letter */
#define TRI_X		(RING_CX - 4)	/* left column of the triangles */
#define TRI_UP_Y	(RING_CY + 17)	/* top row of the upload triangle, in the gap */
#define TRI_DOWN_Y	(RING_CY + 23)

/*
 * The rings are open at the bottom: with the bottom as 0 degrees, clockwise,
 * the scale starts at RING_GAP_HALF_DEG and ends at 360 - RING_GAP_HALF_DEG.
 * RING_START is that start as a fraction of the full circle.
 */
#define RING_GAP_HALF_DEG 20
#define RING_START	(RING_GAP_HALF_DEG / 360.0)

#define LOGO_X		(RING_CX + DISC_R + 4)	/* right of the rings */
#define LOGO_Y		(AXIS_Y - 2 - LOGO_H)	/* above the axis of the graph */

/* port marks: a strip at the bottom, centred */
#define LINK_SIZE	4	/* diameter of a mark */
#define LINK_PITCH	7	/* from one mark to the next */
#define LINK_GROUP_GAP	6	/* added between two groups */
#define LINK_PAD_X	3	/* dimmed patch: room left and right of the marks */
#define LINK_PAD_Y	2	/* and above and below them */
#define LINK_MARGIN	5	/* from the marks to the bottom border */

/*
 * The ports in the order of their marks, with the group each belongs to.
 * The marks of a group stand close together.
 */
const struct port ports[] = {
	{ "sfp1", 0 }, { "sfp2", 0 },
	{ "wan", 1 },
	{ "lan1", 2 }, { "lan2", 2 }, { "lan3", 2 }, { "lan4", 2 },
};

_Static_assert(ARRAY_SIZE(ports) == NPORTS, "NPORTS in draw.h");

/*
 * Scale marks on the rings: at one hundredth and one tenth of the top rate.
 * Their places do not depend on the top rate.
 */
static const double mark_share[] = { 0.01, 0.1 };

#define NMARKS		((int)ARRAY_SIZE(mark_share))

/* the drawing functions clip: pixels outside the screen are left out */

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

static void put_px(struct screen *s, int x, int y, uint16_t c)
{
	if (x >= 0 && x < (int)s->w && y >= 0 && y < (int)s->h)
		s->px[y * s->pitch + x] = c;
}

/* the glyph of a character in a font table, NULL if the font has none */
static const struct glyph *find_glyph(const struct glyph *tab, unsigned int count, char c)
{
	unsigned int n;

	for (n = 0; n < count; n++)
		if (tab[n].c == c)
			return &tab[n];
	return NULL;
}

/* draw a glyph, return the advance to the next one (width and a 1 pixel gap) */
static int put_glyph(struct screen *s, const struct glyph *tab, unsigned int count,
		     int x, int y, char c, uint16_t col)
{
	const struct glyph *g = find_glyph(tab, count, c);
	unsigned int r, b;

	if (!g)
		return 0;
	for (r = 0; r < GLYPH_ROWS && g->row[r]; r++)
		for (b = 0; g->row[r][b]; b++)
			if (g->row[r][b] == '#')
				put_px(s, x + b, y + r, col);
	return (int)strlen(g->row[0]) + 1;
}

/* width of a text in pixels, with the gaps between its glyphs */
static int text_width(const struct glyph *tab, unsigned int count, const char *t)
{
	const struct glyph *g;
	int w = 0;

	for (; *t; t++) {
		g = find_glyph(tab, count, *t);
		if (g)
			w += (int)strlen(g->row[0]) + 1;
	}
	return w ? w - 1 : 0;
}

/* rate in at most four characters: 0, 999, 1.5K, 12M, 940M, 1.5G */
static void fmt_compact(double bps, char *val, size_t vn)
{
	static const char names[] = { 0, 'K', 'M', 'G' };
	int u = 0;
	double v = bps;

	while (v >= 999.5 && u < 3) {
		v /= 1000;
		u++;
	}
	if (u == 0)
		snprintf(val, vn, "%.0f", v);
	else if (v < 9.95)
		snprintf(val, vn, "%.1f%c", v, names[u]);
	else
		snprintf(val, vn, "%.0f%c", v, names[u]);
}

/*
 * Position 0..1 of a rate on the scale that ends at top_bps. The scale is
 * the square root of the share of the top rate: a tenth of the top fills
 * about a third, a hundredth a tenth. This shows small rates and still
 * leaves most of the room to the large ones.
 */
double scale_frac(double bps, double top_bps)
{
	double f;

	if (bps <= 0)
		return 0;
	f = sqrt(bps / top_bps);
	return f > 1 ? 1 : f;
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

/* like put_px(), with the colour mixed a/16 over what is there */
static void mix_px(struct screen *s, int x, int y, uint16_t c, int a)
{
	if (x >= 0 && x < (int)s->w && y >= 0 && y < (int)s->h)
		s->px[y * s->pitch + x] = blend(s->px[y * s->pitch + x], c, a);
}

/* the logo as a faint, still background; everything else is drawn over it */
static void draw_logo(struct screen *s)
{
	int x, y;

	for (y = 0; y < LOGO_H; y++) {
		for (x = 0; x < LOGO_W; x++) {
			int a = logo_cov[y][x] * LOGO_PCT;	/* of LOGO_FULL */

			if (a)
				put_px(s, LOGO_X + x, LOGO_Y + y,
				       RGB(COL_LOGO_R * a / LOGO_FULL, COL_LOGO_G * a / LOGO_FULL,
					   COL_LOGO_B * a / LOGO_FULL));
		}
	}
}

/*
 * A new sample: keep its rates and add its columns to the graph. The height
 * of a sample is the mean of its place on the scale and of those of the two
 * samples before it, in whole rows; the columns between two samples go
 * evenly from one height to the other. Only earlier samples enter a height,
 * and it is stored: a column that is drawn does not change any more, also
 * not when the top of the scale changes.
 */
void view_push(struct view *v, double down, double up)
{
	const double in[2] = { [DOWN] = down, [UP] = up };
	int dir, k, r;

	if (v->nrate == RATES) {
		for (dir = 0; dir < 2; dir++)
			memmove(&v->rate[dir][0], &v->rate[dir][1], (RATES - 1) * sizeof(double));
		v->nrate--;
	}
	if (v->ncol + FRAMES_PER_SAMPLE > GRAPH_COLS) {
		for (dir = 0; dir < 2; dir++)
			memmove(&v->col[dir][0], &v->col[dir][FRAMES_PER_SAMPLE],
				v->ncol - FRAMES_PER_SAMPLE);
		v->ncol -= FRAMES_PER_SAMPLE;
	}
	for (dir = 0; dir < 2; dir++) {
		int n = v->nrate, rows, prev;
		double f[3];

		v->rate[dir][n] = in[dir];
		/* this sample and the two before it; fewer at the start */
		for (k = 0; k < 3; k++)
			f[k] = scale_frac(v->rate[dir][n >= k ? n - k : 0], v->top_bps);
		rows = (int)floor((f[0] + 2 * f[1] + f[2]) / 4 * GRAPH_H + 0.5);
		prev = v->ncol ? v->col[dir][v->ncol - 1] : rows;
		for (r = 1; r <= FRAMES_PER_SAMPLE; r++)
			v->col[dir][v->ncol + r - 1] =
				(prev * (FRAMES_PER_SAMPLE - r) + rows * r + FRAMES_PER_SAMPLE / 2) /
				FRAMES_PER_SAMPLE;
	}
	v->nrate++;
	v->ncol += FRAMES_PER_SAMPLE;
}

/*
 * The graph: upload above the axis, download below it, the newest column at
 * the right border. It shows the heights in v->col as they are and moves
 * one column to the left in each frame: of the columns of the newest
 * sample, one more comes in per frame. The line is solid, one pixel wide
 * and joined to the previous column, the area between the line and the
 * axis is filled with a quarter of the colour. Where the line steps by one
 * or two rows, half-strength pixels beside the step make it look less like
 * a staircase.
 */
static void draw_graph(struct screen *s, const struct view *v)
{
	/*
	 * The column in v->col that stands at the right border. The one after
	 * it is held back: it may put a pixel that softens a step into the
	 * last visible column, and that pixel must be there from the start.
	 */
	int last = v->ncol - FRAMES_PER_SAMPLE + v->phase - 1;
	int dir, y, x;

	for (dir = 0; dir < 2; dir++) {
		int sign = dir == UP ? -1 : 1, prev = 0, have_prev = 0;

		/*
		 * Start one column left of the screen and end one right of it:
		 * the pixels outside are not drawn, but the outermost visible
		 * columns are joined to their neighbours like every other one.
		 */
		for (x = -1; x <= (int)s->w; x++) {
			int i = last - ((int)s->w - 1 - x), h, lo, hi;

			if (i < 0 || i >= v->ncol)
				continue;
			h = v->col[dir][i];
			lo = hi = h;

			/* join to the previous column without doubling the line */
			if (have_prev && h > prev + 1)
				lo = prev + 1;
			else if (have_prev && h < prev - 1)
				hi = prev - 1;

			for (y = 1; y <= h; y++)
				mix_px(s, x, AXIS_Y + sign * y, dir_col[dir], A_FILL);
			for (y = lo; y <= hi; y++)
				mix_px(s, x, AXIS_Y + sign * (y + 1), dir_col[dir], A_SOLID);

			/*
			 * Soften a step of one or two rows: a pixel beside the end
			 * of the higher run, and for a step of one row another one
			 * beside the end of the lower run.
			 */
			if (have_prev && h > prev && h <= prev + 2) {
				mix_px(s, x - 1, AXIS_Y + sign * (prev + 2), dir_col[dir], A_CORNER);
				if (h == prev + 1)
					mix_px(s, x, AXIS_Y + sign * (prev + 1), dir_col[dir], A_CORNER);
			} else if (have_prev && h < prev && h >= prev - 2) {
				mix_px(s, x, AXIS_Y + sign * (prev + 1), dir_col[dir], A_CORNER);
				if (h == prev - 1)
					mix_px(s, x - 1, AXIS_Y + sign * (h + 1), dir_col[dir], A_CORNER);
			}
			prev = h;
			have_prev = 1;
		}
	}
	for (x = 0; x < (int)s->w; x++)
		mix_px(s, x, AXIS_Y, COL_AXIS, A_AXIS);
}

/*
 * Geometry of the rings, worked out once: for each pixel around the centre
 * how many of its 4x4 samples lie in the dimmed disc, on a scale mark and
 * in each ring, and for the samples in a ring their place along its scale
 * (0 at the start of the scale, 1 at its end, clockwise). The rings do not
 * touch, so a sample lies in one ring at most and the two rings share
 * pos[]: the samples of the outer ring fill it from the front, those of the
 * inner ring from the back.
 */
#define GEO_N		(2 * (DISC_R + 1))
#define GEO_SAMPLES	16

static struct geo_px {
	uint8_t disc, tick, track[2];
	float pos[GEO_SAMPLES];
} geo[GEO_N][GEO_N];

/* where pos[] holds sample k of ring i */
static int geo_slot(int i, int k)
{
	return i ? GEO_SAMPLES - 1 - k : k;
}

/* one sample of a pixel, at (dx, dy) from the centre in 1/8 pixel */
static void geo_sample(struct geo_px *g, int dx, int dy, const double *ux, const double *uy)
{
	static const int radius[2] = { [DOWN] = RING_OUT8, [UP] = RING_IN8 };
	int band_in = RING_IN8 - RING_HW8, band_out = RING_OUT8 + RING_HW8;
	int d2 = dx * dx + dy * dy, i, m;

	if (d2 <= DISC_R * 8 * DISC_R * 8)
		g->disc++;

	/* scale marks: 1 pixel wide, across both rings */
	for (m = 0; m < NMARKS && d2 >= band_in * band_in && d2 <= band_out * band_out; m++) {
		if (dx * ux[m] + dy * uy[m] > 0 && fabs(dx * uy[m] - dy * ux[m]) <= 4) {
			g->tick++;
			break;
		}
	}

	for (i = 0; i < 2; i++) {
		int r_in = radius[i] - RING_HW8, r_out = radius[i] + RING_HW8;
		double pos;

		if (d2 < r_in * r_in || d2 > r_out * r_out)
			continue;
		pos = (atan2(dy, dx) - M_PI / 2) / (2 * M_PI);
		if (pos < 0)
			pos += 1;
		/* leave the gap out, then 0..1 along the scale */
		if (pos < RING_START || pos > 1 - RING_START)
			continue;
		pos = (pos - RING_START) / (1 - 2 * RING_START);
		g->pos[geo_slot(i, g->track[i]++)] = (float)pos;
	}
}

/* fill geo[]; needed once before the first draw_frame() */
void draw_init(void)
{
	double ux[NMARKS], uy[NMARKS];
	int gx, gy, sx, sy, m;

	/* direction of each scale mark, seen from the centre */
	for (m = 0; m < NMARKS; m++) {
		double a = RING_START + (1 - 2 * RING_START) * scale_frac(mark_share[m], 1);

		ux[m] = cos(M_PI / 2 + 2 * M_PI * a);
		uy[m] = sin(M_PI / 2 + 2 * M_PI * a);
	}
	for (gy = 0; gy < GEO_N; gy++)
		for (gx = 0; gx < GEO_N; gx++)
			for (sy = 0; sy < 4; sy++)
				for (sx = 0; sx < 4; sx++)
					geo_sample(&geo[gy][gx],
						   (gx - GEO_N / 2) * 8 + 2 * sx + 1,
						   (gy - GEO_N / 2) * 8 + 2 * sy + 1, ux, uy);
}

/*
 * The rings, smoothed with 4x4 samples per pixel. A dimmed disc under them
 * keeps them readable on the graph. Each ring is a faint circle that is
 * open at the bottom; the part from the start of the scale, clockwise, up
 * to the shown fraction is solid and ends in a straight cut.
 * Half-transparent black lines mark a hundredth and a tenth of the top rate.
 */
static void draw_rings(struct screen *s, const struct view *v)
{
	static const int text_y[2] = { [DOWN] = TEXT_DOWN_Y, [UP] = TEXT_UP_Y };
	static const int tri_y[2] = { [DOWN] = TRI_DOWN_Y, [UP] = TRI_UP_Y };
	static const char tri[2] = { [DOWN] = 'v', [UP] = '^' };
	int x, y, i, k;

	for (y = 0; y < GEO_N; y++) {
		for (x = 0; x < GEO_N; x++) {
			const struct geo_px *g = &geo[y][x];
			int px = RING_CX - GEO_N / 2 + x, py = RING_CY - GEO_N / 2 + y;

			if (!g->disc)
				continue;
			mix_px(s, px, py, COL_BG, g->disc * A_DISC / 16);
			for (i = 0; i < 2; i++) {
				int arc = 0;

				if (!g->track[i])
					continue;
				for (k = 0; k < g->track[i]; k++)
					if (g->pos[geo_slot(i, k)] < v->arc[i])
						arc++;
				mix_px(s, px, py, dir_col[i], (g->track[i] * A_TRACK + 8) / 16);
				mix_px(s, px, py, dir_col[i], arc);
			}
			mix_px(s, px, py, COL_BG, g->tick * A_TICK / 16);
		}
	}

	for (i = 0; i < 2; i++) {
		char val[8];

		if (v->valid)
			fmt_compact(v->num[i], val, sizeof(val));
		else
			snprintf(val, sizeof(val), "-");
		/*
		 * The unit letter stands at a fixed place; the number ends
		 * 2 pixels in front of it. So nothing moves when the number of
		 * digits changes. Both have the colour of their ring; the
		 * unit is smaller, so a G is not read as a 6.
		 */
		k = (int)strlen(val);
		if (k && val[k - 1] >= 'A' && val[k - 1] <= 'Z') {
			/* small letter, on the base line of the digits */
			put_glyph(s, font, ARRAY_SIZE(font), UNIT_X, text_y[i] + 3, val[k - 1],
				  dir_col[i]);
			val[k - 1] = 0;
		}
		x = UNIT_X - 2 - text_width(big_font, ARRAY_SIZE(big_font), val);
		for (k = 0; val[k]; k++)
			x += put_glyph(s, big_font, ARRAY_SIZE(big_font), x, text_y[i], val[k],
				       dir_col[i]);

		/*
		 * The triangles stand in the gap of the rings, in the order
		 * of the values: upload above download.
		 */
		put_glyph(s, font, ARRAY_SIZE(font), TRI_X, tri_y[i], tri[i], dir_col[i]);
	}
}

/* colour of a mark for a link state */
static uint16_t link_color(int mbit)
{
	if (mbit <= LINK_DOWN)
		return COL_LINK_DOWN;
	if (mbit == LINK_UP || mbit >= 2500)
		return COL_LINK_FAST;
	return mbit >= 1000 ? COL_LINK_MID : COL_LINK_SLOW;
}

/* colour of a mark: off in every other blink phase while the port has traffic */
uint16_t mark_color(int mbit, int active, int phase)
{
	if (mbit > LINK_DOWN && active && phase)
		return COL_BG;
	return link_color(mbit);
}

/* left column of the mark of port i in the strip of the marks */
static int mark_x(int i)
{
	return i * LINK_PITCH + ports[i].group * LINK_GROUP_GAP;
}

/* the marks as round dots on a dimmed patch without its four corner pixels */
static void draw_links(struct screen *s, const uint16_t *col)
{
	int w = mark_x(NPORTS - 1) + LINK_SIZE + 2 * LINK_PAD_X, h = LINK_SIZE + 2 * LINK_PAD_Y;
	int x0 = ((int)s->w - w) / 2, y0 = (int)s->h - LINK_MARGIN - LINK_SIZE - LINK_PAD_Y;
	int i, x, y;

	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++)
			if (!((x == 0 || x == w - 1) && (y == 0 || y == h - 1)))
				mix_px(s, x0 + x, y0 + y, COL_BG, A_PATCH);
	for (i = 0; i < NPORTS; i++) {
		for (y = 0; y < LINK_SIZE; y++) {
			for (x = 0; x < LINK_SIZE; x++) {
				int dx = 2 * x + 1 - LINK_SIZE, dy = 2 * y + 1 - LINK_SIZE;

				if (dx * dx + dy * dy <= LINK_SIZE * LINK_SIZE)
					put_px(s, x0 + LINK_PAD_X + mark_x(i) + x,
					       y0 + LINK_PAD_Y + y, col[i]);
			}
		}
	}
}

/* one frame: each part is drawn over the ones before it */
void draw_frame(struct screen *s, const struct view *v, const uint16_t *marks)
{
	fill_rect(s, 0, 0, s->w, s->h, COL_BG);
	draw_logo(s);
	draw_graph(s, v);
	draw_rings(s, v);
	draw_links(s, marks);
}
