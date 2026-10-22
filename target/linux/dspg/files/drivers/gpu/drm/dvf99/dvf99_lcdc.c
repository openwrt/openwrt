// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 DSPG Technologies GmbH
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/completion.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>

#include <drm/clients/drm_client_setup.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_bridge.h>
#include <drm/drm_drv.h>
#include <drm/drm_fb_dma_helper.h>
#include <drm/drm_fbdev_dma.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_framebuffer.h>
#include <drm/drm_gem_atomic_helper.h>
#include <drm/drm_gem_dma_helper.h>
#include <drm/drm_gem_framebuffer_helper.h>
#include <drm/drm_of.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_simple_kms_helper.h>
#include <drm/drm_vblank.h>

#define LCDC_INTSR		0x0004
#define LCDC_INTER		0x0008
#define LCDC_LCDCCR		0x0010
#define LCDC_INTMR		0x0020
#define LCDC_DISPCR		0x0024
#define LCDC_OSDMCR		0x0028
#define LCDC_INDTR		0x0040
#define LCDC_INDXSR		0x0044
#define LCDC_INDYSR		0x0048
#define LCDC_DISPXSPOSR		0x0050
#define LCDC_DISPXEPOSR		0x0054
#define LCDC_DISPYSPOS1R	0x0060
#define LCDC_DISPYEPOS1R	0x0064
#define LCDC_DISPYSPOS2R	0x0068
#define LCDC_DISPYEPOS2R	0x006c
#define LCDC_DISPIDXR		0x0090
#define LCDC_MSBAHBA0R		0x00a0
#define LCDC_LSBAHBA0R		0x00a4
#define LCDC_OFFAR0		0x00a8
#define LCDC_OFFAR1		0x00b8
#define LCDC_BACKCPR		0x0140
#define LCDC_RBCR		0x0150
#define LCDC_GBCR		0x0154
#define LCDC_BBCR		0x0158
#define LCDC_GC0R		0x0160
#define LCDC_GCER		0x01a4
#define LCDC_YCLPCR		0x0200
#define LCDC_CCLPCR		0x0204
#define LCDC_CLPER		0x0208
#define LCDC_DISPIR		0x0240
#define LCDC_PANCSR		0x0250
#define LCDC_VSTR		0x0270
#define LCDC_VFTR		0x0274
#define LCDC_VATR		0x0278
#define LCDC_VETR		0x027c
#define LCDC_HSTR		0x0280
#define LCDC_HFTR		0x0284
#define LCDC_HADSTR		0x0288
#define LCDC_HAPWR		0x028c
#define LCDC_HETR		0x0290
#define LCDC_CTLTR0		0x02a8
#define LCDC_CMDFSR		0x02c0
#define LCDC_CMDAR		0x02c4
#define LCDC_TXDR		0x02c8
#define LCDC_GP0A_H_HI		0x0310
#define LCDC_GP0A_V_ST		0x0314
#define LCDC_GP0A_H_LO		0x0318
#define LCDC_GP0B_H_HI		0x0330
#define LCDC_GP0B_V_ST		0x0334
#define LCDC_GP0B_H_LO		0x0338
#define LCDC_GP0B_V_END		0x033c
#define LCDC_GP0BCNTR		0x0340
#define LCDC_GP1B_H_HI		0x0370
#define LCDC_GP1B_V_ST		0x0374
#define LCDC_GP1B_H_LO		0x0378
#define LCDC_GP1B_V_END		0x037c
#define LCDC_GP1BCNTR		0x0380
#define LCDC_GP2_H_HI		0x0390
#define LCDC_GP2_V_ST		0x0394
#define LCDC_GP2_H_LO		0x0398
#define LCDC_GP2_V_END		0x039c
#define LCDC_GP2CNTR		0x03a8
#define LCDC_GP3_H_HI		0x03b0
#define LCDC_GP3_V_ST		0x03b4
#define LCDC_GP3_H_LO		0x03b8
#define LCDC_GP3_V_END		0x03bc
#define LCDC_GP3CNTR		0x03c8
#define LCDC_GPSELR		0x04a0
#define LCDC_PARUP		0x04d0
#define LCDC_CDISPUPR		0x04d4
#define LCDC_ISTAR1		0x04f4

#define LCDC_INT_FRAME_START	BIT(0)
#define LCDC_INT_FRAME_DONE	BIT(1)
#define LCDC_INT_UNDERRUN	BIT(2)

#define LCDC_LCDCCR_EN		BIT(0)
#define LCDC_DISPCR_EN		BIT(0)
#define LCDC_INTMR_RGB565	0
#define LCDC_INTMR_RGB888	1
#define LCDC_DISPIR_TFT24	1
#define LCDC_DISPIR_CPU8080	4
#define LCDC_LCDCCR_FIFO_RST	BIT(1)
#define LCDC_ISTAR1_TXCMD_FULL	BIT(1)
#define LCDC_ISTAR1_STATE	GENMASK(6, 4)
#define LCDC_ISTAR1_STUCK	6
#define LCDC_CMDAR_RS_DATA	BIT(4)
#define LCDC_CMDAR_WRITE	BIT(5)
#define LCDC_CMDAR_LAST		BIT(6)
#define LCDC_CMD_FIFO_SIZE	11
#define LCDC_CMD_TIMEOUT_US	1000
#define LCDC_FRAME_TIMEOUT_MS	100
#define LCDC_GAMMA_STEPS	17
#define MIPI_CASET		0x2a
#define MIPI_RASET		0x2b
#define MIPI_RAMWR		0x2c
#define LCDC_COLOR_UNITY	0x80

#define LCDC_PANCSR_PCLK_INV	BIT(0)
#define LCDC_PANCSR_VSYNC_LOW	BIT(1)
#define LCDC_PANCSR_HSYNC_LOW	BIT(2)
#define LCDC_PANCSR_BGR		BIT(4)

struct dvf99_lcdc {
	struct drm_device drm;
	struct drm_simple_display_pipe pipe;
	struct drm_connector *connector;
	void __iomem *base;
	struct clk *clk;
	struct completion frame_done;
	bool frame_pending;
	bool cpu_mode;
	bool bgr;
};

struct dvf99_lcdc_reg {
	u16 reg;
	u16 val;
};

static struct dvf99_lcdc *drm_to_lcdc(struct drm_device *drm)
{
	return container_of(drm, struct dvf99_lcdc, drm);
}

static void dvf99_lcdc_write(struct dvf99_lcdc *lcdc, unsigned int reg,
			     u32 val)
{
	writel(val, lcdc->base + reg);
}

static void dvf99_lcdc_set_addr(struct dvf99_lcdc *lcdc,
				struct drm_plane_state *pstate)
{
	dma_addr_t addr = drm_fb_dma_get_gem_addr(pstate->fb, pstate, 0);

	dvf99_lcdc_write(lcdc, LCDC_MSBAHBA0R, upper_16_bits(addr));
	dvf99_lcdc_write(lcdc, LCDC_LSBAHBA0R, lower_16_bits(addr));
	dvf99_lcdc_write(lcdc, LCDC_PARUP, 1);
}

static irqreturn_t dvf99_lcdc_irq(int irq, void *data)
{
	struct dvf99_lcdc *lcdc = data;
	u32 stat = readl(lcdc->base + LCDC_INTSR);

	if (!stat)
		return IRQ_NONE;

	dvf99_lcdc_write(lcdc, LCDC_INTSR, stat);

	if (stat & LCDC_INT_FRAME_DONE)
		complete(&lcdc->frame_done);

	if (stat & LCDC_INT_FRAME_START)
		drm_crtc_handle_vblank(&lcdc->pipe.crtc);

	if (stat & LCDC_INT_UNDERRUN)
		drm_dbg_kms(&lcdc->drm, "FIFO underrun\n");

	return IRQ_HANDLED;
}

static void dvf99_lcdc_cpu_reinit(struct dvf99_lcdc *lcdc,
				  const struct drm_display_mode *mode)
{
	const struct dvf99_lcdc_reg seq[] = {
		{ LCDC_INTER, LCDC_INT_FRAME_START | LCDC_INT_UNDERRUN },
		{ LCDC_DISPIR, LCDC_DISPIR_CPU8080 },
		{ LCDC_INTMR, LCDC_INTMR_RGB565 },
		{ LCDC_OSDMCR, 0 },
		{ LCDC_PANCSR, 0 },
		{ LCDC_DISPCR, LCDC_DISPCR_EN },
		{ LCDC_VSTR, 0x0002 },
		{ LCDC_VFTR, 0x0004 },
		{ LCDC_VATR, mode->vdisplay * 3 / 2 - 1 },
		{ LCDC_VETR, 0x0005 },
		{ LCDC_HSTR, 0x0020 },
		{ LCDC_HFTR, 0x0020 },
		{ LCDC_HADSTR, mode->hdisplay - 1 },
		{ LCDC_HAPWR, mode->hdisplay - 1 },
		{ LCDC_HETR, 0x0010 },
		{ LCDC_CMDFSR, 0x000a },
		{ LCDC_CTLTR0, 0x0800 },
		{ LCDC_GP0A_H_HI, 0x000e },
		{ LCDC_GP0A_V_ST, 0x000e },
		{ LCDC_GP0A_H_LO, 0x0006 },
		{ LCDC_GP0B_H_HI, 0x0000 },
		{ LCDC_GP0B_V_ST, 0x0000 },
		{ LCDC_GP0B_H_LO, 0x000b },
		{ LCDC_GP0B_V_END, 0x000b },
		{ LCDC_GP0BCNTR, 0xa800 },
		{ LCDC_GP1B_H_HI, 0x0002 },
		{ LCDC_GP1B_V_ST, 0x0003 },
		{ LCDC_GP1B_H_LO, 0x0007 },
		{ LCDC_GP1B_V_END, 0x0007 },
		{ LCDC_GP1BCNTR, 0x20a0 },
		{ LCDC_GP2_H_HI, 0x0002 },
		{ LCDC_GP2_V_ST, 0x0003 },
		{ LCDC_GP2_H_LO, 0x0008 },
		{ LCDC_GP2_V_END, 0x0007 },
		{ LCDC_GP2CNTR, 0xa800 },
		{ LCDC_GP3_H_HI, 0x0001 },
		{ LCDC_GP3_V_ST, 0x0002 },
		{ LCDC_GP3_H_LO, 0x0009 },
		{ LCDC_GP3_V_END, 0x0008 },
		{ LCDC_GP3CNTR, 0xa8a0 },
		{ LCDC_INDTR, mode->hdisplay * 3 / 4 - 1 },
		{ LCDC_INDXSR, mode->hdisplay - 1 },
		{ LCDC_INDYSR, mode->vdisplay - 1 },
		{ LCDC_BACKCPR, 0 },
		{ LCDC_RBCR, LCDC_COLOR_UNITY },
		{ LCDC_GBCR, LCDC_COLOR_UNITY },
		{ LCDC_BBCR, LCDC_COLOR_UNITY },
		{ LCDC_DISPXSPOSR, 0 },
		{ LCDC_DISPXEPOSR, mode->hdisplay - 1 },
		{ LCDC_DISPYSPOS1R, 0 },
		{ LCDC_DISPYEPOS1R, mode->vdisplay * 3 / 2 - 1 },
		{ LCDC_DISPYSPOS2R, 0 },
		{ LCDC_DISPYEPOS2R, mode->vdisplay - 1 },
		{ LCDC_GCER, 0 },
		{ LCDC_YCLPCR, 0xff00 },
		{ LCDC_CCLPCR, 0xff00 },
		{ LCDC_CLPER, 0 },
		{ LCDC_OFFAR0, mode->hdisplay * 3 },
		{ LCDC_OFFAR1, mode->hdisplay * 3 },
		{ LCDC_PARUP, 1 },
		{ LCDC_LCDCCR, LCDC_LCDCCR_EN },
	};
	unsigned int i;

	for (i = 0; i < LCDC_GAMMA_STEPS - 1; i++)
		dvf99_lcdc_write(lcdc, LCDC_GC0R + i * 4,
				 (i * 0x10 + 0x08) << 8 | i * 0x10);
	dvf99_lcdc_write(lcdc, LCDC_GC0R + i * 4, 0x00ff);

	for (i = 0; i < ARRAY_SIZE(seq); i++)
		dvf99_lcdc_write(lcdc, seq[i].reg, seq[i].val);
}

static int dvf99_lcdc_cpu_cmd(struct dvf99_lcdc *lcdc, u8 cmd,
			      const u8 *data, unsigned int len)
{
	u32 stat, ctl = LCDC_CMDAR_WRITE;
	unsigned int i;
	int ret;

	for (i = 0; i <= len; i++) {
		u32 next = i ? LCDC_CMDAR_WRITE | LCDC_CMDAR_RS_DATA :
			       LCDC_CMDAR_WRITE;

		if (i == len)
			next |= LCDC_CMDAR_LAST;

		if (!i || next != ctl) {
			ctl = next;
			dvf99_lcdc_write(lcdc, LCDC_CMDAR, ctl);
		}

		ret = readl_poll_timeout(lcdc->base + LCDC_ISTAR1, stat,
					 !(stat & LCDC_ISTAR1_TXCMD_FULL), 1,
					 LCDC_CMD_TIMEOUT_US);
		if (ret)
			return ret;

		dvf99_lcdc_write(lcdc, LCDC_TXDR, i ? data[i - 1] : cmd);
	}

	return 0;
}

static void dvf99_lcdc_cpu_push(struct dvf99_lcdc *lcdc,
				struct drm_plane_state *pstate,
				const struct drm_display_mode *mode)
{
	u8 caset[] = { 0, 0, (mode->hdisplay - 1) >> 8,
		       (mode->hdisplay - 1) & 0xff };
	u8 raset[] = { 0, 0, (mode->vdisplay - 1) >> 8,
		       (mode->vdisplay - 1) & 0xff };
	u32 state;

	if (lcdc->frame_pending &&
	    !wait_for_completion_timeout(&lcdc->frame_done,
					 msecs_to_jiffies(LCDC_FRAME_TIMEOUT_MS)))
		drm_warn_once(&lcdc->drm, "no frame done interrupt\n");

	reinit_completion(&lcdc->frame_done);

	dvf99_lcdc_set_addr(lcdc, pstate);

	state = FIELD_GET(LCDC_ISTAR1_STATE,
			  readl(lcdc->base + LCDC_ISTAR1));
	if (state >= LCDC_ISTAR1_STUCK) {
		dvf99_lcdc_write(lcdc, LCDC_LCDCCR, LCDC_LCDCCR_FIFO_RST);
		dvf99_lcdc_cpu_reinit(lcdc, mode);
		dvf99_lcdc_set_addr(lcdc, pstate);
	}

	dvf99_lcdc_write(lcdc, LCDC_CMDFSR, LCDC_CMD_FIFO_SIZE);
	if (dvf99_lcdc_cpu_cmd(lcdc, MIPI_CASET, caset, sizeof(caset)) ||
	    dvf99_lcdc_cpu_cmd(lcdc, MIPI_RASET, raset, sizeof(raset)) ||
	    dvf99_lcdc_cpu_cmd(lcdc, MIPI_RAMWR, NULL, 0)) {
		drm_warn_once(&lcdc->drm, "panel command FIFO timeout\n");
		return;
	}

	dvf99_lcdc_write(lcdc, LCDC_PARUP, 1);
	dvf99_lcdc_write(lcdc, LCDC_CDISPUPR, 1);
	lcdc->frame_pending = true;
}

static enum drm_mode_status
dvf99_lcdc_mode_valid(struct drm_simple_display_pipe *pipe,
		      const struct drm_display_mode *mode)
{
	if (mode->hdisplay % 4)
		return MODE_H_ILLEGAL;

	if (mode->hsync_start == mode->hdisplay ||
	    mode->hsync_end == mode->hsync_start ||
	    mode->htotal == mode->hsync_end ||
	    mode->vsync_start == mode->vdisplay ||
	    mode->vsync_end == mode->vsync_start ||
	    mode->vtotal == mode->vsync_end)
		return MODE_BAD;

	return MODE_OK;
}

static int dvf99_lcdc_check(struct drm_simple_display_pipe *pipe,
			    struct drm_plane_state *pstate,
			    struct drm_crtc_state *cstate)
{
	struct drm_framebuffer *old_fb = pipe->plane.state->fb;
	struct drm_framebuffer *fb = pstate->fb;

	struct dvf99_lcdc *lcdc = drm_to_lcdc(pipe->crtc.dev);

	if (!fb)
		return 0;

	if (fb->pitches[0] != cstate->mode.hdisplay * fb->format->cpp[0])
		return -EINVAL;

	if (lcdc->cpu_mode && fb->format->format != DRM_FORMAT_RGB888)
		return -EINVAL;

	if (old_fb && old_fb->format != fb->format)
		cstate->mode_changed = true;

	return 0;
}

static u32 dvf99_lcdc_pancsr(struct dvf99_lcdc *lcdc,
			     const struct drm_display_mode *mode)
{
	u32 bus_flags = lcdc->connector->display_info.bus_flags;
	u32 pancsr = 0;

	if (bus_flags & DRM_BUS_FLAG_PIXDATA_DRIVE_NEGEDGE)
		pancsr |= LCDC_PANCSR_PCLK_INV;
	if (mode->flags & DRM_MODE_FLAG_NVSYNC)
		pancsr |= LCDC_PANCSR_VSYNC_LOW;
	if (mode->flags & DRM_MODE_FLAG_NHSYNC)
		pancsr |= LCDC_PANCSR_HSYNC_LOW;
	if (lcdc->bgr)
		pancsr |= LCDC_PANCSR_BGR;

	return pancsr;
}

static void dvf99_lcdc_enable(struct drm_simple_display_pipe *pipe,
			      struct drm_crtc_state *cstate,
			      struct drm_plane_state *pstate)
{
	struct dvf99_lcdc *lcdc = drm_to_lcdc(pipe->crtc.dev);
	const struct drm_display_mode *mode = &cstate->adjusted_mode;
	struct drm_framebuffer *fb = pstate->fb;
	unsigned int bpp = fb->format->cpp[0] * 8;
	int ret;

	if (lcdc->cpu_mode) {
		dvf99_lcdc_write(lcdc, LCDC_INTSR, readl(lcdc->base + LCDC_INTSR));
		dvf99_lcdc_write(lcdc, LCDC_INTER, LCDC_INT_FRAME_DONE);
		lcdc->frame_pending = false;
		dvf99_lcdc_cpu_push(lcdc, pstate, mode);
		return;
	}

	ret = clk_set_rate(lcdc->clk, mode->clock * 1000);
	if (ret)
		drm_warn(&lcdc->drm, "pixel clock %d kHz not set: %d\n",
			 mode->clock, ret);

	dvf99_lcdc_write(lcdc, LCDC_LCDCCR, 0);
	dvf99_lcdc_write(lcdc, LCDC_DISPCR, 0);

	dvf99_lcdc_write(lcdc, LCDC_GCER, 0);
	dvf99_lcdc_write(lcdc, LCDC_CLPER, 0);
	dvf99_lcdc_write(lcdc, LCDC_OSDMCR, 0);
	dvf99_lcdc_write(lcdc, LCDC_DISPIDXR, 0);
	dvf99_lcdc_write(lcdc, LCDC_BACKCPR, 0);
	dvf99_lcdc_write(lcdc, LCDC_RBCR, LCDC_COLOR_UNITY);
	dvf99_lcdc_write(lcdc, LCDC_GBCR, LCDC_COLOR_UNITY);
	dvf99_lcdc_write(lcdc, LCDC_BBCR, LCDC_COLOR_UNITY);
	dvf99_lcdc_write(lcdc, LCDC_DISPYSPOS2R, 0);
	dvf99_lcdc_write(lcdc, LCDC_DISPYEPOS2R, 0);
	dvf99_lcdc_write(lcdc, LCDC_GPSELR, 0);

	dvf99_lcdc_write(lcdc, LCDC_INTMR, bpp == 16 ? LCDC_INTMR_RGB565 :
					    LCDC_INTMR_RGB888);
	dvf99_lcdc_write(lcdc, LCDC_DISPIR, LCDC_DISPIR_TFT24);
	dvf99_lcdc_write(lcdc, LCDC_PANCSR, dvf99_lcdc_pancsr(lcdc, mode));

	dvf99_lcdc_write(lcdc, LCDC_VSTR, mode->vsync_end - mode->vsync_start - 1);
	dvf99_lcdc_write(lcdc, LCDC_VFTR, mode->vtotal - mode->vsync_end - 1);
	dvf99_lcdc_write(lcdc, LCDC_VATR, mode->vdisplay - 1);
	dvf99_lcdc_write(lcdc, LCDC_VETR, mode->vsync_start - mode->vdisplay - 1);
	dvf99_lcdc_write(lcdc, LCDC_HSTR, mode->hsync_end - mode->hsync_start - 1);
	dvf99_lcdc_write(lcdc, LCDC_HFTR, mode->htotal - mode->hsync_end - 1);
	dvf99_lcdc_write(lcdc, LCDC_HADSTR, mode->hdisplay - 1);
	dvf99_lcdc_write(lcdc, LCDC_HAPWR, mode->hdisplay - 1);
	dvf99_lcdc_write(lcdc, LCDC_HETR, mode->hsync_start - mode->hdisplay - 1);

	dvf99_lcdc_write(lcdc, LCDC_INDTR, mode->hdisplay * bpp / 32 - 1);
	dvf99_lcdc_write(lcdc, LCDC_INDXSR, mode->hdisplay - 1);
	dvf99_lcdc_write(lcdc, LCDC_INDYSR, mode->vdisplay - 1);
	dvf99_lcdc_write(lcdc, LCDC_DISPXSPOSR, 0);
	dvf99_lcdc_write(lcdc, LCDC_DISPXEPOSR, mode->hdisplay - 1);
	dvf99_lcdc_write(lcdc, LCDC_DISPYSPOS1R, 0);
	dvf99_lcdc_write(lcdc, LCDC_DISPYEPOS1R, mode->vdisplay - 1);
	dvf99_lcdc_write(lcdc, LCDC_OFFAR0, fb->pitches[0]);

	dvf99_lcdc_set_addr(lcdc, pstate);

	dvf99_lcdc_write(lcdc, LCDC_DISPCR, LCDC_DISPCR_EN);
	dvf99_lcdc_write(lcdc, LCDC_LCDCCR, LCDC_LCDCCR_EN);

	drm_crtc_vblank_on(&pipe->crtc);
}

static void dvf99_lcdc_disable(struct drm_simple_display_pipe *pipe)
{
	struct dvf99_lcdc *lcdc = drm_to_lcdc(pipe->crtc.dev);

	if (lcdc->cpu_mode) {
		if (lcdc->frame_pending)
			wait_for_completion_timeout(&lcdc->frame_done,
						    msecs_to_jiffies(LCDC_FRAME_TIMEOUT_MS));
		lcdc->frame_pending = false;
		dvf99_lcdc_write(lcdc, LCDC_INTER, 0);
		return;
	}

	drm_crtc_vblank_off(&pipe->crtc);

	dvf99_lcdc_write(lcdc, LCDC_LCDCCR, 0);
	dvf99_lcdc_write(lcdc, LCDC_DISPCR, 0);
}

static void dvf99_lcdc_update(struct drm_simple_display_pipe *pipe,
			      struct drm_plane_state *old_pstate)
{
	struct dvf99_lcdc *lcdc = drm_to_lcdc(pipe->crtc.dev);
	struct drm_plane_state *pstate = pipe->plane.state;
	struct drm_crtc *crtc = &pipe->crtc;
	struct drm_pending_vblank_event *event = crtc->state->event;

	if (pstate->fb && crtc->state->active) {
		if (lcdc->cpu_mode)
			dvf99_lcdc_cpu_push(lcdc, pstate, &crtc->state->adjusted_mode);
		else
			dvf99_lcdc_set_addr(lcdc, pstate);
	}

	if (!event)
		return;

	crtc->state->event = NULL;

	spin_lock_irq(&crtc->dev->event_lock);
	if (crtc->state->active && !lcdc->cpu_mode &&
	    drm_crtc_vblank_get(crtc) == 0)
		drm_crtc_arm_vblank_event(crtc, event);
	else
		drm_crtc_send_vblank_event(crtc, event);
	spin_unlock_irq(&crtc->dev->event_lock);
}

static int dvf99_lcdc_enable_vblank(struct drm_simple_display_pipe *pipe)
{
	struct dvf99_lcdc *lcdc = drm_to_lcdc(pipe->crtc.dev);

	dvf99_lcdc_write(lcdc, LCDC_INTSR, readl(lcdc->base + LCDC_INTSR));
	dvf99_lcdc_write(lcdc, LCDC_INTER,
			 LCDC_INT_FRAME_START | LCDC_INT_UNDERRUN);

	return 0;
}

static void dvf99_lcdc_disable_vblank(struct drm_simple_display_pipe *pipe)
{
	struct dvf99_lcdc *lcdc = drm_to_lcdc(pipe->crtc.dev);

	dvf99_lcdc_write(lcdc, LCDC_INTER, 0);
}

static const struct drm_simple_display_pipe_funcs dvf99_lcdc_pipe_funcs = {
	.mode_valid	= dvf99_lcdc_mode_valid,
	.check		= dvf99_lcdc_check,
	.enable		= dvf99_lcdc_enable,
	.disable	= dvf99_lcdc_disable,
	.update		= dvf99_lcdc_update,
	.enable_vblank	= dvf99_lcdc_enable_vblank,
	.disable_vblank	= dvf99_lcdc_disable_vblank,
};

static const u32 dvf99_lcdc_formats[] = {
	DRM_FORMAT_RGB565,
	DRM_FORMAT_RGB888,
};

static const struct drm_mode_config_funcs dvf99_lcdc_mode_config_funcs = {
	.fb_create	= drm_gem_fb_create_with_dirty,
	.atomic_check	= drm_atomic_helper_check,
	.atomic_commit	= drm_atomic_helper_commit,
};

DEFINE_DRM_GEM_DMA_FOPS(dvf99_lcdc_fops);

static const struct drm_driver dvf99_lcdc_driver = {
	.driver_features	= DRIVER_MODESET | DRIVER_GEM | DRIVER_ATOMIC,
	.fops			= &dvf99_lcdc_fops,
	.name			= "dvf99-lcdc",
	.desc			= "DSPG DVF99 LCD controller",
	.major			= 1,
	.minor			= 0,
	DRM_GEM_DMA_DRIVER_OPS,
	DRM_FBDEV_DMA_DRIVER_OPS,
};

static int dvf99_lcdc_modeset_init(struct dvf99_lcdc *lcdc)
{
	struct drm_device *drm = &lcdc->drm;
	struct drm_bridge *bridge;
	int ret;

	ret = drmm_mode_config_init(drm);
	if (ret)
		return ret;

	drm->mode_config.funcs = &dvf99_lcdc_mode_config_funcs;
	drm->mode_config.min_width = 4;
	drm->mode_config.max_width = 1024;
	drm->mode_config.min_height = 1;
	drm->mode_config.max_height = 1024;

	bridge = devm_drm_of_get_bridge(drm->dev, drm->dev->of_node, 0, 0);
	if (IS_ERR(bridge))
		return dev_err_probe(drm->dev, PTR_ERR(bridge),
				     "no panel or bridge\n");

	ret = drm_simple_display_pipe_init(drm, &lcdc->pipe,
					   &dvf99_lcdc_pipe_funcs,
					   dvf99_lcdc_formats,
					   ARRAY_SIZE(dvf99_lcdc_formats),
					   NULL, NULL);
	if (ret)
		return ret;

	ret = drm_simple_display_pipe_attach_bridge(&lcdc->pipe, bridge);
	if (ret)
		return ret;

	if (!drm_bridge_is_panel(bridge))
		return dev_err_probe(drm->dev, -ENODEV, "bridge is not a panel\n");

	lcdc->connector = drm_panel_bridge_connector(bridge);

	ret = drm_vblank_init(drm, 1);
	if (ret)
		return ret;

	drm_mode_config_reset(drm);

	return 0;
}

static int dvf99_lcdc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct dvf99_lcdc *lcdc;
	int irq, ret;

	lcdc = devm_drm_dev_alloc(dev, &dvf99_lcdc_driver, struct dvf99_lcdc,
				  drm);
	if (IS_ERR(lcdc))
		return PTR_ERR(lcdc);

	platform_set_drvdata(pdev, &lcdc->drm);

	lcdc->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(lcdc->base))
		return PTR_ERR(lcdc->base);

	lcdc->clk = devm_clk_get_enabled(dev, NULL);
	if (IS_ERR(lcdc->clk))
		return dev_err_probe(dev, PTR_ERR(lcdc->clk), "no clock\n");

	lcdc->bgr = of_property_read_bool(dev->of_node, "dspg,bgr-order");
	lcdc->cpu_mode = readl(lcdc->base + LCDC_DISPIR) == LCDC_DISPIR_CPU8080;
	init_completion(&lcdc->frame_done);
	if (lcdc->cpu_mode)
		dev_info(dev, "bootloader left the panel in 8080 CPU mode\n");

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	dvf99_lcdc_write(lcdc, LCDC_INTER, 0);
	dvf99_lcdc_write(lcdc, LCDC_INTSR, readl(lcdc->base + LCDC_INTSR));

	ret = devm_request_irq(dev, irq, dvf99_lcdc_irq, 0, dev_name(dev),
			       lcdc);
	if (ret)
		return ret;

	ret = dvf99_lcdc_modeset_init(lcdc);
	if (ret)
		return ret;

	ret = drm_dev_register(&lcdc->drm, 0);
	if (ret)
		return ret;

	drm_client_setup_with_fourcc(&lcdc->drm, lcdc->cpu_mode ?
				     DRM_FORMAT_RGB888 : DRM_FORMAT_RGB565);

	return 0;
}

static void dvf99_lcdc_remove(struct platform_device *pdev)
{
	struct drm_device *drm = platform_get_drvdata(pdev);

	drm_dev_unregister(drm);
	drm_atomic_helper_shutdown(drm);
}

static void dvf99_lcdc_shutdown(struct platform_device *pdev)
{
	drm_atomic_helper_shutdown(platform_get_drvdata(pdev));
}

static const struct of_device_id dvf99_lcdc_of_match[] = {
	{ .compatible = "dspg,dvf99-lcdc" },
	{ }
};
MODULE_DEVICE_TABLE(of, dvf99_lcdc_of_match);

static struct platform_driver dvf99_lcdc_platform_driver = {
	.probe = dvf99_lcdc_probe,
	.remove = dvf99_lcdc_remove,
	.shutdown = dvf99_lcdc_shutdown,
	.driver = {
		.name = "dvf99-lcdc",
		.of_match_table = dvf99_lcdc_of_match,
	},
};
module_platform_driver(dvf99_lcdc_platform_driver);

MODULE_DESCRIPTION("DSPG DVF99 LCD controller DRM driver");
MODULE_LICENSE("GPL");
