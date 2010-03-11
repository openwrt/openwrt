/*
 * linux/drivers/video/metronomefb.c -- FB driver for Metronome controller
 *
 * Copyright (C) 2008, Jaya Kumar
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file COPYING in the main directory of this archive for
 * more details.
 *
 * Layout is based on skeletonfb.c by James Simmons and Geert Uytterhoeven.
 *
 * This work was made possible by help and equipment support from E-Ink
 * Corporation. http://support.eink.com/community
 *
 * This driver is written to be used with the Metronome display controller.
 * It is intended to be architecture independent. A board specific driver
 * must be used to perform all the physical IO interactions. An example
 * is provided as am200epd.c
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/list.h>
#include <linux/firmware.h>
#include <linux/dma-mapping.h>
#include <linux/uaccess.h>
#include <linux/irq.h>
#include <linux/ctype.h>

#include <video/metronomefb.h>

#include <asm/unaligned.h>

#define WF_MODE_INIT	0 /* Initialization */
#define WF_MODE_MU	1 /* Monochrome update */
#define WF_MODE_GU	2 /* Grayscale update */
#define WF_MODE_GC	3 /* Grayscale clearing */

static int temp = 25;

/* frame differs from image. frame includes non-visible pixels */
struct epd_frame {
	int fw; /* frame width */
	int fh; /* frame height */
	u16 config[4];
	int wfm_size;
};

static const struct epd_frame epd_frame_table[] = {
	{
		.fw = 832,
		.fh = 622,
		.config = {
			15 /* sdlew */
			| 2 << 8 /* sdosz */
			| 0 << 11 /* sdor */
			| 0 << 12 /* sdces */
			| 0 << 15, /* sdcer */
			42 /* gdspl */
			| 1 << 8 /* gdr1 */
			| 1 << 9 /* sdshr */
			| 0 << 15, /* gdspp */
			18 /* gdspw */
			| 0 << 15, /* dispc */
			599 /* vdlc */
			| 0 << 11 /* dsi */
			| 0 << 12, /* dsic */
		},
		.wfm_size = 47001,
	},
	{
		.fw = 1088,
		.fh = 791,
		.config = {
			0x0104,
			0x031f,
			0x0088,
			0x02ff,
		},
		.wfm_size = 46770,
	},
	{
		.fw = 1200,
		.fh = 842,
		.config = {
			0x0101,
			0x030e,
			0x0012,
			0x0280,
		},
		.wfm_size = 46770,
	},
	{
		.fw = 800,
		.fh = 600,
		.config = {
			15 /* sdlew */
			| 2 << 8 /* sdosz */
			| 0 << 11 /* sdor */
			| 0 << 12 /* sdces */
			| 0 << 15, /* sdcer */
			42 /* gdspl */
			| 1 << 8 /* gdr1 */
			| 1 << 9 /* sdshr */
			| 0 << 15, /* gdspp */
			18 /* gdspw */
			| 0 << 15, /* dispc */
			599 /* vdlc */
			| 0 << 11 /* dsi */
			| 0 << 12, /* dsic */
		},
		.wfm_size = 46901,
	},
};

static const struct fb_fix_screeninfo metronomefb_fix __devinitconst = {
	.id =		"metronomefb",
	.type =		FB_TYPE_PACKED_PIXELS,
	.visual =	FB_VISUAL_STATIC_PSEUDOCOLOR,
	.xpanstep =	0,
	.ypanstep =	0,
	.ywrapstep =	0,
	.accel =	FB_ACCEL_NONE,
};

static const struct fb_var_screeninfo metronomefb_var __devinitconst = {
	.bits_per_pixel	= 8,
	.grayscale	= 1,
	.nonstd		= 1,
	.red =		{ 4, 3, 0 },
	.green =	{ 0, 0, 0 },
	.blue =		{ 0, 0, 0 },
	.transp =	{ 0, 0, 0 },
};

/* the waveform structure that is coming from userspace firmware */
struct waveform_hdr {
	u8 stuff[32];

	u8 wmta[3];
	u8 fvsn;

	u8 luts;
	u8 mc;
	u8 trc;
	u8 stuff3;

	u8 endb;
	u8 swtb;
	u8 stuff2a[2];

	u8 stuff2b[3];
	u8 wfm_cs;
} __attribute__ ((packed));

/* main metronomefb functions */
static u8 calc_cksum(int start, int end, u8 *mem)
{
	u8 tmp = 0;
	int i;

	for (i = start; i < end; i++)
		tmp += mem[i];

	return tmp;
}

static u16 calc_img_cksum(u16 *start, int length)
{
	u16 tmp = 0;

	while (length--)
		tmp += *start++;

	return tmp;
}

/* here we decode the incoming waveform file and populate metromem */
static int load_waveform(u8 *mem, size_t size, int m, int t,
				struct metronomefb_par *par)
{
	int tta;
	int wmta;
	int trn = 0;
	int i;
	unsigned char v;
	u8 cksum;
	int cksum_idx;
	int wfm_idx, owfm_idx;
	int mem_idx = 0;
	struct waveform_hdr *wfm_hdr;
	u8 *metromem = par->metromem_wfm;
	struct device *dev = &par->pdev->dev;
	u8 mc, trc;
	u16 *p;
	u16 img_cksum;

	dev_dbg(dev, "Loading waveforms, mode %d, temperature %d\n", m, t);

	wfm_hdr = (struct waveform_hdr *) mem;

	if (wfm_hdr->fvsn != 1) {
		dev_err(dev, "Error: bad fvsn %x\n", wfm_hdr->fvsn);
		return -EINVAL;
	}
	if (wfm_hdr->luts != 0) {
		dev_err(dev, "Error: bad luts %x\n", wfm_hdr->luts);
		return -EINVAL;
	}
	cksum = calc_cksum(32, 47, mem);
	if (cksum != wfm_hdr->wfm_cs) {
		dev_err(dev, "Error: bad cksum %x != %x\n", cksum,
					wfm_hdr->wfm_cs);
		return -EINVAL;
	}
	mc = wfm_hdr->mc + 1;
	trc = wfm_hdr->trc + 1;

	for (i = 0; i < 5; i++) {
		if (*(wfm_hdr->stuff2a + i) != 0) {
			dev_err(dev, "Error: unexpected value in padding\n");
			return -EINVAL;
		}
	}

	/* calculating trn. trn is something used to index into
	the waveform. presumably selecting the right one for the
	desired temperature. it works out the offset of the first
	v that exceeds the specified temperature */
	if ((sizeof(*wfm_hdr) + trc) > size)
		return -EINVAL;

	for (i = sizeof(*wfm_hdr); i <= sizeof(*wfm_hdr) + trc; i++) {
		if (mem[i] > t) {
			trn = i - sizeof(*wfm_hdr) - 1;
			break;
		}
	}

	/* check temperature range table checksum */
	cksum_idx = sizeof(*wfm_hdr) + trc + 1;
	if (cksum_idx > size)
		return -EINVAL;
	cksum = calc_cksum(sizeof(*wfm_hdr), cksum_idx, mem);
	if (cksum != mem[cksum_idx]) {
		dev_err(dev, "Error: bad temperature range table cksum"
				" %x != %x\n", cksum, mem[cksum_idx]);
		return -EINVAL;
	}

	/* check waveform mode table address checksum */
	wmta = get_unaligned_le32(wfm_hdr->wmta) & 0x00FFFFFF;
	cksum_idx = wmta + m*4 + 3;
	if (cksum_idx > size)
		return -EINVAL;
	cksum = calc_cksum(cksum_idx - 3, cksum_idx, mem);
	if (cksum != mem[cksum_idx]) {
		dev_err(dev, "Error: bad mode table address cksum"
				" %x != %x\n", cksum, mem[cksum_idx]);
		return -EINVAL;
	}

	/* check waveform temperature table address checksum */
	tta = get_unaligned_le32(mem + wmta + m * 4) & 0x00FFFFFF;
	cksum_idx = tta + trn*4 + 3;
	if (cksum_idx > size)
		return -EINVAL;
	cksum = calc_cksum(cksum_idx - 3, cksum_idx, mem);
	if (cksum != mem[cksum_idx]) {
		dev_err(dev, "Error: bad temperature table address cksum"
			" %x != %x\n", cksum, mem[cksum_idx]);
		return -EINVAL;
	}

	/* here we do the real work of putting the waveform into the
	metromem buffer. this does runlength decoding of the waveform */
	wfm_idx = get_unaligned_le32(mem + tta + trn * 4) & 0x00FFFFFF;
	owfm_idx = wfm_idx;
	if (wfm_idx > size)
		return -EINVAL;
	while (wfm_idx < size) {
		unsigned char rl;
		v = mem[wfm_idx++];
		if (v == wfm_hdr->swtb) {
			while (((v = mem[wfm_idx++]) != wfm_hdr->swtb) &&
				wfm_idx < size)
				metromem[mem_idx++] = v;

			continue;
		}

		if (v == wfm_hdr->endb)
			break;

		rl = mem[wfm_idx++];
		for (i = 0; i <= rl; i++)
			metromem[mem_idx++] = v;
	}

	cksum_idx = wfm_idx;
	if (cksum_idx > size)
		return -EINVAL;
	dev_dbg(dev, "mem_idx = %u\n", mem_idx);
	cksum = calc_cksum(owfm_idx, cksum_idx, mem);
	if (cksum != mem[cksum_idx]) {
		dev_err(dev, "Error: bad waveform data cksum"
				" %x != %x\n", cksum, mem[cksum_idx]);
		return -EINVAL;
	}
	par->frame_count = (mem_idx/64);

	p = (u16 *)par->metromem_wfm;
	img_cksum = calc_img_cksum(p, 16384 / 2);
	p[16384 / 2] = __cpu_to_le16(img_cksum);

	par->current_wf_mode = m;
	par->current_wf_temp = t;

	return 0;
}

static int check_err(struct metronomefb_par *par)
{
	int res;

	res = par->board->get_err(par);
	dev_dbg(&par->pdev->dev, "ERR = %d\n", res);
	return res;
}

static inline int wait_for_rdy(struct metronomefb_par *par)
{
	int res = 0;

	if (!par->board->get_rdy(par))
		res = par->board->met_wait_event_intr(par);

	return res;
}

static int metronome_display_cmd(struct metronomefb_par *par)
{
	int i;
	u16 cs;
	u16 opcode;
	int res;

	res = wait_for_rdy(par);
	if (res)
		return res;

	dev_dbg(&par->pdev->dev, "%s: ENTER\n", __func__);
	/* setup display command
	we can't immediately set the opcode since the controller
	will try parse the command before we've set it all up
	so we just set cs here and set the opcode at the end */

	if (par->metromem_cmd->opcode == 0xCC40)
		opcode = cs = 0xCC41;
	else
		opcode = cs = 0xCC40;

	/* set the args ( 2 bytes ) for display */
	i = 0;
	par->metromem_cmd->args[i] = 	0 << 3 /* border update */
					| (3 << 4)
//					| ((borderval++ % 4) & 0x0F) << 4
					| (par->frame_count - 1) << 8;
	cs += par->metromem_cmd->args[i++];

	/* the rest are 0 */
	memset((u8 *) (par->metromem_cmd->args + i), 0, (32-i)*2);

	par->metromem_cmd->csum = cs;
	par->metromem_cmd->opcode = opcode; /* display cmd */

	return 0;

}

static int __devinit metronome_powerup_cmd(struct metronomefb_par *par)
{
	int i;
	u16 cs;
	int res;

	dev_dbg(&par->pdev->dev, "%s: ENTER\n", __func__);
	/* setup power up command */
	par->metromem_cmd->opcode = 0x1234; /* pwr up pseudo cmd */
	cs = par->metromem_cmd->opcode;

	/* set pwr1,2,3 to 1024 */
	for (i = 0; i < 3; i++) {
//		par->metromem_cmd->args[i] = 1024;
		par->metromem_cmd->args[i] = 100;
		cs += par->metromem_cmd->args[i];
	}

	/* the rest are 0 */
	memset((u8 *) (par->metromem_cmd->args + i), 0, (32-i)*2);

	par->metromem_cmd->csum = cs;

	msleep(1);
	par->board->set_rst(par, 1);

	msleep(1);
	par->board->set_stdby(par, 1);

	res = par->board->met_wait_event(par);
	dev_dbg(&par->pdev->dev, "%s: EXIT: %d\n", __func__, res);
	return res;
}

static int __devinit metronome_config_cmd(struct metronomefb_par *par)
{
	/* setup config command
	we can't immediately set the opcode since the controller
	will try parse the command before we've set it all up */

	dev_dbg(&par->pdev->dev, "%s: ENTER\n", __func__);
	memcpy(par->metromem_cmd->args, par->epd_frame->config,
		sizeof(par->epd_frame->config));
	/* the rest are 0 */
	memset((u8 *) (par->metromem_cmd->args + 4), 0, (32-4)*2);

	par->metromem_cmd->csum = 0xCC10;
	par->metromem_cmd->csum += calc_img_cksum(par->metromem_cmd->args, 4);
	par->metromem_cmd->opcode = 0xCC10; /* config cmd */

	return par->board->met_wait_event(par);
}

static int __devinit metronome_init_cmd(struct metronomefb_par *par)
{
	int i;
	u16 cs;

	/* setup init command
	we can't immediately set the opcode since the controller
	will try parse the command before we've set it all up
	so we just set cs here and set the opcode at the end */

	dev_dbg(&par->pdev->dev, "%s: ENTER\n", __func__);
	cs = 0xCC20;

	/* set the args ( 2 bytes ) for init */
	i = 0;
	par->metromem_cmd->args[i] = 0x0007;
	cs += par->metromem_cmd->args[i++];

	/* the rest are 0 */
	memset((u8 *) (par->metromem_cmd->args + i), 0, (32-i)*2);

	par->metromem_cmd->csum = cs;
	par->metromem_cmd->opcode = 0xCC20; /* init cmd */

	return par->board->met_wait_event(par);
}

static int metronome_bootup(struct metronomefb_par *par)
{
	int res;

	res = metronome_powerup_cmd(par);
	if (res) {
		dev_err(&par->pdev->dev, "metronomefb: POWERUP cmd failed\n");
		goto finish;
	}

	check_err(par);
	res = metronome_config_cmd(par);
	if (res) {
		dev_err(&par->pdev->dev, "metronomefb: CONFIG cmd failed\n");
		goto finish;
	}
	check_err(par);

	res = metronome_init_cmd(par);
	if (res)
		dev_err(&par->pdev->dev, "metronomefb: INIT cmd failed\n");
	check_err(par);

finish:
	return res;
}

static int __devinit metronome_init_regs(struct metronomefb_par *par)
{
	int res;

	if (par->board->power_ctl)
		par->board->power_ctl(par, METRONOME_POWER_ON);

	res =  metronome_bootup(par);

	return res;
}

static uint16_t metronomefb_update_img_buffer_rotated(struct metronomefb_par *par)
{
	int x, y;
	int xstep, ystep;
	int i, j;
	uint16_t cksum = 0;
	uint8_t *buf = par->info->screen_base;
	uint32_t *img = (uint32_t *)(par->metromem_img);
	int fw = par->epd_frame->fw;
	int fh = par->epd_frame->fh;
	int fw_buf = fw / 4;
	uint32_t *fxbuckets = par->fxbuckets;
	uint32_t *fybuckets = par->fybuckets;
	uint32_t diff;
	uint32_t tmp;

	switch (par->info->var.rotate) {
	case FB_ROTATE_CW:
		xstep = -fh;
		ystep = fw * fh + 1;
		j = (fw - 1) * fh;
		break;
	case FB_ROTATE_UD:
		xstep = -1;
		ystep = 0;
		j = fw * fh - 1;
		break;
	case FB_ROTATE_CCW:
		xstep = fh;
		ystep = -fw * fh - 1;
		j = fh - 1;
		break;
	default:
		BUG();
		break;
	}

	memset(fxbuckets, 0, fw_buf * sizeof(*fxbuckets));
	memset(fybuckets, 0, fh * sizeof(*fybuckets));

	i = 0;
	for (y = 0; y < fh; y++) {
		for(x = 0; x < fw_buf; x++, i++) {
			if (j < 0 || j >= fw * fh) {
				printk("moo: %d %d %d %d %d\n", j, x, y, fw_buf, fh);
				return 0;
			}
			tmp = (buf[j] << 5);
			j += xstep;
			tmp |= (buf[j] << 13);
			j += xstep;
			tmp |= (buf[j] << 21);
			j += xstep;
			tmp |= (buf[j] << 29);
			j += xstep;
			tmp &= 0xe0e0e0e0;

			img[i] &= 0xf0f0f0f0;
			diff = img[i] ^ tmp;

			fxbuckets[x] |= diff;
			fybuckets[y] |= diff;

			img[i] = (img[i] >> 4) | tmp;
			cksum += img[i] & 0x0000ffff;
			cksum += (img[i] >> 16);

		}
		j += ystep;
	}

	return cksum;
}

static uint16_t metronomefb_update_img_buffer_normal(struct metronomefb_par *par)
{
	int x, y, i;
	uint16_t cksum = 0;
	uint32_t *buf = (uint32_t __force *)par->info->screen_base;
	uint32_t *img = (uint32_t *)(par->metromem_img);
	uint32_t diff;
	uint32_t tmp;
	int fw = par->epd_frame->fw;
	int fh = par->epd_frame->fh;
	int fw_buf = fw / sizeof(*buf);
	uint32_t *fxbuckets = par->fxbuckets;
	uint32_t *fybuckets = par->fybuckets;

	memset(fxbuckets, 0, fw_buf * sizeof(*fxbuckets));
	memset(fybuckets, 0, fh * sizeof(*fybuckets));

	i = 0;
	for (y = 0; y < fh; y++) {
		for(x = 0; x < fw_buf; x++, i++) {
			tmp = (buf[i] << 5) & 0xe0e0e0e0;
			img[i] &= 0xf0f0f0f0;
			diff = img[i] ^ tmp;

			fxbuckets[x] |= diff;
			fybuckets[y] |= diff;

			img[i] = (img[i] >> 4) | tmp;
			cksum += img[i] & 0x0000ffff;
			cksum += (img[i] >> 16);
		}
	}

	return cksum;
}

static unsigned int metronomefb_get_change_count(struct metronomefb_par *par)
{
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	int fw = par->epd_frame->fw / 4;
	int fh = par->epd_frame->fh;
	unsigned int change_count;
	uint32_t *fxbuckets = par->fxbuckets;
	uint32_t *fybuckets = par->fybuckets;

	for (min_x = 0; min_x < fw; ++min_x) {
		if(fxbuckets[min_x])
			break;
	}

	for (max_x = fw - 1; max_x >= 0; --max_x) {
		if(fxbuckets[max_x])
			break;
	}

	for (min_y = 0; min_y < fh; min_y++) {
		if(fybuckets[min_y])
			break;
	}

	for (max_y = fh - 1; max_y >= 0; --max_y) {
		if(fybuckets[max_y])
			break;
	}

	if ((min_x > max_x) || (min_y > max_y))
		change_count = 0;
	else
		change_count = (max_x - min_x + 1) * (max_y - min_y + 1) * 4;

	dev_dbg(&par->pdev->dev, "min_x = %d, max_x = %d, min_y = %d, max_y = %d\n",
			min_x, max_x, min_y, max_y);

	return change_count;
}

static void metronomefb_dpy_update(struct metronomefb_par *par, int clear_all)
{
	unsigned int fbsize = par->info->fix.smem_len;
	uint16_t cksum;
	int m;

	wait_for_rdy(par);

	if (par->info->var.rotate == 0)
		cksum = metronomefb_update_img_buffer_normal(par);
	else
		cksum = metronomefb_update_img_buffer_rotated(par);

	*par->metromem_img_csum = __cpu_to_le16(cksum);

	if (clear_all || par->is_first_update ||
		(par->partial_updates_count == par->partial_autorefresh_interval)) {
		m = WF_MODE_GC;
		par->partial_updates_count = 0;
	} else {
		int change_count = metronomefb_get_change_count(par);
		if (change_count < fbsize / 100 * par->manual_refresh_threshold)
			m = WF_MODE_GU;
		else
			m = WF_MODE_GC;

		dev_dbg(&par->pdev->dev, "change_count = %u, treshold = %u%% (%u pixels)\n",
				change_count, par->manual_refresh_threshold,
				fbsize / 100 * par->manual_refresh_threshold);
		++par->partial_updates_count;
	}

	if (m != par->current_wf_mode)
		load_waveform((u8 *) par->firmware->data, par->firmware->size,
				m, par->current_wf_temp, par);

again:
	metronome_display_cmd(par);
	wait_for_rdy(par);
	if (unlikely(check_err(par))) {
		par->board->set_stdby(par, 0);
		printk("Resetting Metronome\n");
		par->board->set_rst(par, 0);
		mdelay(1);
		if (par->board->power_ctl)
			par->board->power_ctl(par, METRONOME_POWER_OFF);

		mdelay(1);
		load_waveform((u8 *) par->firmware->data, par->firmware->size,
				WF_MODE_GC, par->current_wf_temp, par);

		if (par->board->power_ctl)
			par->board->power_ctl(par, METRONOME_POWER_ON);
		metronome_bootup(par);

		goto again;
	}

	par->is_first_update = 0;
}

/* this is called back from the deferred io workqueue */
static void metronomefb_dpy_deferred_io(struct fb_info *info,
				struct list_head *pagelist)
{
	struct metronomefb_par *par = info->par;

	/* We will update entire display because we need to change
	 * 'previous image' field in pixels which was changed at
	 * previous refresh
	 */
	mutex_lock(&par->lock);
	metronomefb_dpy_update(par, 0);
	mutex_unlock(&par->lock);
}

static void metronomefb_fillrect(struct fb_info *info,
				   const struct fb_fillrect *rect)
{
	struct metronomefb_par *par = info->par;

	mutex_lock(&par->lock);
	sys_fillrect(info, rect);
	metronomefb_dpy_update(par, 0);
	mutex_unlock(&par->lock);
}

static void metronomefb_copyarea(struct fb_info *info,
				   const struct fb_copyarea *area)
{
	struct metronomefb_par *par = info->par;

	mutex_lock(&par->lock);
	sys_copyarea(info, area);
	metronomefb_dpy_update(par, 0);
	mutex_unlock(&par->lock);
}

static void metronomefb_imageblit(struct fb_info *info,
				const struct fb_image *image)
{
	struct metronomefb_par *par = info->par;

	mutex_lock(&par->lock);
	sys_imageblit(info, image);
	metronomefb_dpy_update(par, 0);
	mutex_unlock(&par->lock);
}

/*
 * this is the slow path from userspace. they can seek and write to
 * the fb. it is based on fb_sys_write
 */
static ssize_t metronomefb_write(struct fb_info *info, const char __user *buf,
				size_t count, loff_t *ppos)
{
	struct metronomefb_par *par = info->par;
	unsigned long p = *ppos;
	void *dst;
	int err = 0;
	unsigned long total_size;

	if (info->state != FBINFO_STATE_RUNNING)
		return -EPERM;

	total_size = info->fix.smem_len;

	if (p > total_size)
		return -EFBIG;

	if (count > total_size) {
		err = -EFBIG;
		count = total_size;
	}

	if (count + p > total_size) {
		if (!err)
			err = -ENOSPC;

		count = total_size - p;
	}

	dst = (void __force *)(info->screen_base + p);

	mutex_lock(&par->lock);

	if (copy_from_user(dst, buf, count))
		err = -EFAULT;

	if  (!err)
		*ppos += count;

	metronomefb_dpy_update(par, 0);
	mutex_unlock(&par->lock);

	return (err) ? err : count;
}

static int metronome_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	struct metronomefb_par *par = info->par;

	if (par->epd_frame->fw == var->xres && par->epd_frame->fh == var->yres)
		return 0;

	return -EINVAL;
}

static int metronomefb_set_par(struct fb_info *info)
{
	struct metronomefb_par *par = info->par;

	switch (info->var.rotate) {
	case FB_ROTATE_CW:
	case FB_ROTATE_CCW:
		info->fix.line_length = info->var.yres;
		break;
	case FB_ROTATE_UD:
	default:
		info->fix.line_length = info->var.xres;
		break;
	}

	mutex_lock(&par->lock);
	metronomefb_dpy_update(info->par, 1);
	mutex_unlock(&par->lock);

	return 0;
}

static struct fb_ops metronomefb_ops = {
	.owner		= THIS_MODULE,
	.fb_write	= metronomefb_write,
	.fb_fillrect	= metronomefb_fillrect,
	.fb_copyarea	= metronomefb_copyarea,
	.fb_imageblit	= metronomefb_imageblit,
	.fb_check_var	= metronome_check_var,
	.fb_set_par	= metronomefb_set_par,
};

static struct fb_deferred_io metronomefb_defio = {
	.delay		= HZ / 4,
	.deferred_io	= metronomefb_dpy_deferred_io,
};

static ssize_t metronomefb_defio_delay_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct fb_info *info = dev_get_drvdata(dev);

	sprintf(buf, "%lu\n", info->fbdefio->delay * 1000 / HZ);
	return strlen(buf) + 1;
}

static ssize_t metronomefb_defio_delay_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct fb_info *info = dev_get_drvdata(dev);
	char *after;
	unsigned long state = simple_strtoul(buf, &after, 10);
	size_t count = after - buf;
	ssize_t ret = -EINVAL;

	if (*after && isspace(*after))
		count++;

	state = state * HZ / 1000;

	if (!state)
		state = 1;

	if (count == size) {
		ret = count;
		info->fbdefio->delay = state;
	}

	return ret;
}

static ssize_t metronomefb_manual_refresh_thr_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct fb_info *info = dev_get_drvdata(dev);
	struct metronomefb_par *par = info->par;

	return sprintf(buf, "%u\n", par->manual_refresh_threshold);
}

static ssize_t metronomefb_manual_refresh_thr_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct fb_info *info = dev_get_drvdata(dev);
	struct metronomefb_par *par = info->par;
	char *after;
	unsigned long val = simple_strtoul(buf, &after, 10);
	size_t count = after - buf;
	ssize_t ret = -EINVAL;

	if (*after && isspace(*after))
		count++;

	if (val > 100)
		return -EINVAL;


	if (count == size) {
		ret = count;
		par->manual_refresh_threshold = val;
	}

	return ret;
}

static ssize_t metronomefb_autorefresh_interval_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct fb_info *info = dev_get_drvdata(dev);
	struct metronomefb_par *par = info->par;

	return sprintf(buf, "%u\n", par->partial_autorefresh_interval);
}

static ssize_t metronomefb_autorefresh_interval_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct fb_info *info = dev_get_drvdata(dev);
	struct metronomefb_par *par = info->par;
	char *after;
	unsigned long val = simple_strtoul(buf, &after, 10);
	size_t count = after - buf;
	ssize_t ret = -EINVAL;

	if (*after && isspace(*after))
		count++;

	if (val > 100)
		return -EINVAL;


	if (count == size) {
		ret = count;
		par->partial_autorefresh_interval = val;
	}

	return ret;
}

static ssize_t metronomefb_temp_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct fb_info *info = dev_get_drvdata(dev);
	struct metronomefb_par *par = info->par;

	return sprintf(buf, "%u\n", par->current_wf_temp);
}

static ssize_t metronomefb_temp_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct fb_info *info = dev_get_drvdata(dev);
	struct metronomefb_par *par = info->par;
	char *after;
	unsigned long val = simple_strtoul(buf, &after, 10);
	size_t count = after - buf;
	ssize_t ret = -EINVAL;

	if (*after && isspace(*after))
		count++;

	if (val > 100)
		return -EINVAL;


	if (count == size) {
		ret = count;
		if (val != par->current_wf_temp)
			load_waveform((u8 *) par->firmware->data, par->firmware->size,
					par->current_wf_mode, val, par);
	}

	return ret;
}

DEVICE_ATTR(defio_delay, 0644,
		metronomefb_defio_delay_show, metronomefb_defio_delay_store);
DEVICE_ATTR(manual_refresh_threshold, 0644,
		metronomefb_manual_refresh_thr_show, metronomefb_manual_refresh_thr_store);
DEVICE_ATTR(temp, 0644,
		metronomefb_temp_show, metronomefb_temp_store);
DEVICE_ATTR(autorefresh_interval, 0644,
		metronomefb_autorefresh_interval_show, metronomefb_autorefresh_interval_store);


static int __devinit metronomefb_probe(struct platform_device *dev)
{
	struct fb_info *info;
	struct metronome_board *board;
	int retval = -ENOMEM;
	int videomemorysize;
	unsigned char *videomemory;
	struct metronomefb_par *par;
	const struct firmware *fw_entry;
	int i;
	int panel_type;
	int fw, fh;
	int epd_dt_index;

	/* pick up board specific routines */
	board = dev->dev.platform_data;
	if (!board)
		return -EINVAL;

	/* try to count device specific driver, if can't, platform recalls */
	if (!try_module_get(board->owner))
		return -ENODEV;

	info = framebuffer_alloc(sizeof(struct metronomefb_par), &dev->dev);
	if (!info)
		goto err;

	/* we have two blocks of memory.
	info->screen_base which is vm, and is the fb used by apps.
	par->metromem which is physically contiguous memory and
	contains the display controller commands, waveform,
	processed image data and padding. this is the data pulled
	by the device's LCD controller and pushed to Metronome.
	the metromem memory is allocated by the board driver and
	is provided to us */

	panel_type = board->get_panel_type();
	switch (panel_type) {
	case 5:
		epd_dt_index = 3;
		break;
	case 6:
		epd_dt_index = 0;
		break;
	case 8:
		epd_dt_index = 1;
		break;
	case 97:
		epd_dt_index = 2;
		break;
	default:
		dev_err(&dev->dev, "Unexpected panel type. Defaulting to 6\n");
		epd_dt_index = 0;
		break;
	}

	fw = epd_frame_table[epd_dt_index].fw;
	fh = epd_frame_table[epd_dt_index].fh;

	/* we need to add a spare page because our csum caching scheme walks
	 * to the end of the page */
	videomemorysize = PAGE_SIZE + (fw * fh);
	videomemory = vmalloc(videomemorysize);
	if (!videomemory)
		goto err_fb_rel;

	memset(videomemory, 0xff, videomemorysize);

	info->screen_base = (char __force __iomem *)videomemory;
	info->fbops = &metronomefb_ops;

	info->var = metronomefb_var;
	info->var.xres = fw;
	info->var.yres = fh;
	info->var.xres_virtual = fw;
	info->var.yres_virtual = fh;
	info->fix = metronomefb_fix;
	info->fix.line_length = fw;
	info->fix.smem_len = fw * fh; /* Real size of image area */
	par = info->par;
	par->info = info;
	par->board = board;
	par->epd_frame = &epd_frame_table[epd_dt_index];
	par->pdev = dev;

	par->fxbuckets = kmalloc((fw / 4 + 1) * sizeof(*par->fxbuckets), GFP_KERNEL);
	if (!par->fxbuckets)
		goto err_vfree;

	par->fybuckets = kmalloc(fh * sizeof(*par->fybuckets), GFP_KERNEL);
	if (!par->fybuckets)
		goto err_fxbuckets;

	init_waitqueue_head(&par->waitq);
	par->manual_refresh_threshold = 60;
	par->partial_autorefresh_interval = 256;
	par->partial_updates_count = 0;
	par->is_first_update = 1;
	mutex_init(&par->lock);

	/* this table caches per page csum values. */
	par->csum_table = vmalloc(videomemorysize/PAGE_SIZE);
	if (!par->csum_table)
		goto err_fybuckets;

	/* the physical framebuffer that we use is setup by
	 * the platform device driver. It will provide us
	 * with cmd, wfm and image memory in a contiguous area. */
	retval = board->setup_fb(par);
	if (retval) {
		dev_err(&dev->dev, "Failed to setup fb\n");
		goto err_csum_table;
	}

	/* after this point we should have a framebuffer */
	if ((!par->metromem_wfm) ||  (!par->metromem_img) ||
		(!par->metromem_dma)) {
		dev_err(&dev->dev, "fb access failure\n");
		retval = -EINVAL;
		goto err_csum_table;
	}

	info->fix.smem_start = par->metromem_dma;

	/* load the waveform in. assume mode 3, temp 31 for now
		a) request the waveform file from userspace
		b) process waveform and decode into metromem */
	retval = request_firmware(&fw_entry, "metronome.wbf", &dev->dev);
	if (retval < 0) {
		dev_err(&dev->dev, "Failed to get waveform\n");
		goto err_csum_table;
	}

	retval = load_waveform((u8 *) fw_entry->data, fw_entry->size, WF_MODE_GC, temp,
				par);
	if (retval < 0) {
		dev_err(&dev->dev, "Failed processing waveform\n");
		goto err_csum_table;
	}
	par->firmware = fw_entry;

	retval = board->setup_io(par);
	if (retval) {
		dev_err(&dev->dev, "metronomefb: setup_io() failed\n");
		goto err_csum_table;
	}

	if (board->setup_irq(info))
		goto err_csum_table;

	retval = metronome_init_regs(par);
	if (retval < 0)
		goto err_free_irq;

	info->flags = FBINFO_FLAG_DEFAULT;

	info->fbdefio = &metronomefb_defio;
	fb_deferred_io_init(info);

	retval = fb_alloc_cmap(&info->cmap, 8, 0);
	if (retval < 0) {
		dev_err(&dev->dev, "Failed to allocate colormap\n");
		goto err_free_irq;
	}

	/* set cmap */
	for (i = 0; i < 8; i++)
		info->cmap.red[i] = ((2 * i + 1)*(0xFFFF))/16;
	memcpy(info->cmap.green, info->cmap.red, sizeof(u16)*8);
	memcpy(info->cmap.blue, info->cmap.red, sizeof(u16)*8);

	retval = register_framebuffer(info);
	if (retval < 0)
		goto err_cmap;

	platform_set_drvdata(dev, info);

	retval = device_create_file(info->dev, &dev_attr_defio_delay);
	if (retval)
		goto err_devattr_defio_delay;

	retval = device_create_file(info->dev, &dev_attr_manual_refresh_threshold);
	if (retval)
		goto err_devattr_manual_refresh_thr;

	retval = device_create_file(info->dev, &dev_attr_temp);
	if (retval)
		goto err_devattr_temp;

	retval = device_create_file(info->dev, &dev_attr_autorefresh_interval);
	if (retval)
		goto err_devattr_autorefresh;

	dev_info(&dev->dev,
		"fb%d: Metronome frame buffer device, using %dK of video"
		" memory\n", info->node, videomemorysize >> 10);

	return 0;

	device_remove_file(info->dev, &dev_attr_autorefresh_interval);
err_devattr_autorefresh:
	device_remove_file(info->dev, &dev_attr_temp);
err_devattr_temp:
	device_remove_file(info->dev, &dev_attr_manual_refresh_threshold);
err_devattr_manual_refresh_thr:
	device_remove_file(info->dev, &dev_attr_defio_delay);
err_devattr_defio_delay:
	unregister_framebuffer(info);
err_cmap:
	fb_dealloc_cmap(&info->cmap);
err_free_irq:
	board->cleanup(par);
err_csum_table:
	vfree(par->csum_table);
err_fybuckets:
	kfree(par->fybuckets);
err_fxbuckets:
	kfree(par->fxbuckets);
err_vfree:
	vfree(videomemory);
err_fb_rel:
	framebuffer_release(info);
err:
	module_put(board->owner);
	return retval;
}

static int __devexit metronomefb_remove(struct platform_device *dev)
{
	struct fb_info *info = platform_get_drvdata(dev);

	if (info) {
		struct metronomefb_par *par = info->par;

		par->board->set_stdby(par, 0);
		mdelay(1);
		if (par->board->power_ctl)
			par->board->power_ctl(par, METRONOME_POWER_OFF);

		device_remove_file(info->dev, &dev_attr_autorefresh_interval);
		device_remove_file(info->dev, &dev_attr_temp);
		device_remove_file(info->dev, &dev_attr_manual_refresh_threshold);
		device_remove_file(info->dev, &dev_attr_defio_delay);
		unregister_framebuffer(info);
		fb_deferred_io_cleanup(info);
		fb_dealloc_cmap(&info->cmap);
		par->board->cleanup(par);
		vfree(par->csum_table);
		kfree(par->fybuckets);
		kfree(par->fxbuckets);
		vfree((void __force *)info->screen_base);
		module_put(par->board->owner);
		release_firmware(par->firmware);
		dev_dbg(&dev->dev, "calling release\n");
		framebuffer_release(info);
	}
	return 0;
}

#ifdef CONFIG_PM
static int metronomefb_suspend(struct platform_device *pdev, pm_message_t message)
{
	struct fb_info *info = platform_get_drvdata(pdev);
	struct metronomefb_par *par = info->par;

	par->board->set_stdby(par, 0);
	par->board->set_rst(par, 0);
	if (par->board->power_ctl)
		par->board->power_ctl(par, METRONOME_POWER_OFF);


	return 0;
}

static int metronomefb_resume(struct platform_device *pdev)
{
	struct fb_info *info = platform_get_drvdata(pdev);
	struct metronomefb_par *par = info->par;

	if (par->board->power_ctl)
		par->board->power_ctl(par, METRONOME_POWER_ON);

	mutex_lock(&par->lock);
	metronome_bootup(par);
	mutex_unlock(&par->lock);

	return 0;
}

#else
#define metronomefb_suspend NULL
#define metronomefb_resume NULL
#endif


static struct platform_driver metronomefb_driver = {
	.driver		= {
			.owner	= THIS_MODULE,
			.name	= "metronomefb",
			},
	.probe		= metronomefb_probe,
	.remove		= __devexit_p(metronomefb_remove),
	.suspend	= metronomefb_suspend,
	.resume		= metronomefb_resume,
};

static int __init metronomefb_init(void)
{
	return platform_driver_register(&metronomefb_driver);
}

static void __exit metronomefb_exit(void)
{
	platform_driver_unregister(&metronomefb_driver);
}

module_param(temp, int, 0);
MODULE_PARM_DESC(temp, "Set current temperature");

module_init(metronomefb_init);
module_exit(metronomefb_exit);

MODULE_DESCRIPTION("fbdev driver for Metronome controller");
MODULE_AUTHOR("Jaya Kumar");
MODULE_LICENSE("GPL");
