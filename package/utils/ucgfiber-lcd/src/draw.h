/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * ucgfiber-lcd: what the drawing code in draw.c and the rest of the program
 * share.
 */

#ifndef UCGFIBER_LCD_DRAW_H
#define UCGFIBER_LCD_DRAW_H

#include <stdint.h>

/* size of the panel; draw.c holds the layout for it */
#define PANEL_W		160
#define PANEL_H		80

/*
 * A frame is drawn FRAMES_PER_SAMPLE times per sample, and the graph moves
 * one column to the left in each frame. So a sample is that many columns
 * wide, and the graph shows PANEL_W / FRAMES_PER_SAMPLE samples.
 */
#define FRAMES_PER_SAMPLE 2

#define GRAPH_COLS	(PANEL_W + 2 * FRAMES_PER_SAMPLE)	/* columns kept, see struct view */
#define RATES		4	/* newest rates kept */

enum { DOWN, UP };

/* the pixels to draw into, RGB565 */
struct screen {
	uint16_t *px;
	unsigned int w, h, pitch;	/* pitch in pixels */
};

/* what is shown: the graph and the current rates; [DOWN] and [UP] */
struct view {
	/*
	 * The graph: the height of the line in rows for each column, oldest
	 * first. A height is worked out once, when its sample arrives, and
	 * never again. Kept are the columns on the screen, one on each side
	 * of them, which the outermost visible columns are joined to, and
	 * those of the newest sample that have not come in yet.
	 */
	uint8_t col[2][GRAPH_COLS];
	int ncol;		/* columns in col */
	int phase;		/* frames since the newest sample */
	double rate[2][RATES];	/* newest rates in bit/s, oldest first */
	int nrate;		/* rates in rate */
	double arc[2];		/* shown length of the arcs, 0..1 */
	double num[2];		/* shown values in bit/s */
	int valid;		/* the rates are measured */
	double top_bps;		/* top of the scale */
};

/* a port that has a mark: its network device and the group of its mark */
struct port {
	const char *name;
	int group;
};

#define NPORTS		7

extern const struct port ports[];

/* state of a link: LINK_DOWN, LINK_UP or the speed in Mbit/s */
#define LINK_DOWN	0
#define LINK_UP		1	/* up, speed unknown */

double scale_frac(double bps, double top_bps);
uint16_t mark_color(int mbit, int active, int phase);

/* draw_init() once, then view_push() for each sample and draw_frame() for each frame */
void draw_init(void);
void view_push(struct view *v, double down, double up);
void draw_frame(struct screen *s, const struct view *v, const uint16_t *marks);

#endif
