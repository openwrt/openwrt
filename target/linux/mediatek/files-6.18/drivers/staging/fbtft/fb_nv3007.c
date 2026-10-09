// SPDX-License-Identifier: GPL-2.0+
/*
 * NewVision NV3007 FBTFT driver for the EdgePi E87N SPI TFT panel.
 * Register initialization sequence follows the same set of vendor
 * registers documented in the MIT-licensed LVGL nv3007 driver
 * (lvgl/lvgl, src/drivers/display/nv3007/lv_nv3007.c), with timing/
 * gamma values tuned for this panel.
 */
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <video/mipi_display.h>

#include "fbtft.h"

#define DRVNAME "fb_nv3007"
#define WIDTH 428
#define HEIGHT 142
#define TXBUFLEN (16 * PAGE_SIZE)

#define DEFAULT_GAMMA \
	"00 02 04 07 05 02 21 23 3A 08 13 13 29 31 0F\n" \
	"00 02 04 07 05 02 21 23 3A 08 13 13 29 31 0F"

static int init_display(struct fbtft_par *par)
{
	par->fbtftops.reset(par);
	write_reg(par, 0x11);
	mdelay(5);
	write_reg(par, 0x28);
	write_reg(par, 0xff, 0xa5);
	write_reg(par, 0x9a, 0x08);
	write_reg(par, 0x9b, 0x08);
	write_reg(par, 0x9c, 0xb0);
	write_reg(par, 0x9d, 0x17);
	write_reg(par, 0x9e, 0xc2);
	write_reg(par, 0x8f, 0x22, 0x04);
	write_reg(par, 0x84, 0x90);
	write_reg(par, 0x83, 0x7b);
	write_reg(par, 0x85, 0x4f);
	write_reg(par, 0x50, 0x00);
	write_reg(par, 0x52, 0xd6);
	write_reg(par, 0x53, 0x04);
	write_reg(par, 0x54, 0x04);
	write_reg(par, 0x55, 0x1b);
	write_reg(par, 0x56, 0x1b);
	write_reg(par, 0xa0, 0x2a, 0x24, 0x00);
	write_reg(par, 0xa1, 0x84);
	write_reg(par, 0xa2, 0x85);
	write_reg(par, 0xa8, 0x34);
	write_reg(par, 0xa9, 0x80);
	write_reg(par, 0xaa, 0x73);
	write_reg(par, 0xab, 0x03, 0x61);
	write_reg(par, 0xac, 0x03, 0x65);
	write_reg(par, 0xad, 0x03, 0x60);
	write_reg(par, 0xae, 0x03, 0x64);
	write_reg(par, 0xb9, 0x82);
	write_reg(par, 0xba, 0x83);
	write_reg(par, 0xbb, 0x80);
	write_reg(par, 0xbc, 0x81);
	write_reg(par, 0xbd, 0x02);
	write_reg(par, 0xbe, 0x01);
	write_reg(par, 0xbf, 0x04);
	write_reg(par, 0xc0, 0x03);
	write_reg(par, 0xc4, 0x33);
	write_reg(par, 0xc5, 0x80);
	write_reg(par, 0xc6, 0x73);
	write_reg(par, 0xc7, 0x00);
	write_reg(par, 0xc8, 0x33, 0x33);
	write_reg(par, 0xc9, 0x5b);
	write_reg(par, 0xca, 0x5a);
	write_reg(par, 0xcb, 0x5d);
	write_reg(par, 0xcc, 0x5c);
	write_reg(par, 0xcd, 0x33, 0x33);
	write_reg(par, 0xce, 0x5f);
	write_reg(par, 0xcf, 0x5e);
	write_reg(par, 0xd0, 0x61);
	write_reg(par, 0xd1, 0x60);
	write_reg(par, 0xb0, 0x3a, 0x3a, 0x00, 0x00);
	write_reg(par, 0xb6, 0x32);
	write_reg(par, 0xb7, 0x80);
	write_reg(par, 0xb8, 0x73);
	write_reg(par, 0xe0, 0x00);
	write_reg(par, 0xe1, 0x03, 0x0f);
	write_reg(par, 0xe2, 0x04);
	write_reg(par, 0xe3, 0x01);
	write_reg(par, 0xe4, 0x0e);
	write_reg(par, 0xe5, 0x01);
	write_reg(par, 0xe6, 0x19);
	write_reg(par, 0xe7, 0x10);
	write_reg(par, 0xe8, 0x10);
	write_reg(par, 0xe9, 0x21);
	write_reg(par, 0xea, 0x12);
	write_reg(par, 0xeb, 0xd0);
	write_reg(par, 0xec, 0x04);
	write_reg(par, 0xed, 0x07);
	write_reg(par, 0xee, 0x07);
	write_reg(par, 0xef, 0x09);
	write_reg(par, 0xf0, 0xd0);
	write_reg(par, 0xf1, 0x0e);
	write_reg(par, 0xf9, 0x56);
	write_reg(par, 0xf2, 0x26, 0x1b, 0x0b, 0x20);
	write_reg(par, 0xec, 0x04);
	write_reg(par, 0x35, 0x00);
	write_reg(par, 0x44, 0x00, 0x10);
	write_reg(par, 0x46, 0x10);
	write_reg(par, 0xff, 0x00);
	write_reg(par, 0x3a, 0x05);
	write_reg(par, 0x11);
	mdelay(200);
	write_reg(par, 0x29);
	mdelay(150);
	write_reg(par, 0x2a, 0x00, 0x0c, 0x00, 0x99);
	write_reg(par, 0x2b, 0x00, 0x00, 0x01, 0xab);
	mdelay(20);

	return 0;
}

static void set_addr_win(struct fbtft_par *par, int xs, int ys, int xe, int ye)
{
	switch (par->info->var.rotate) {
	case 0:
		xs += 14;
		xe += 14;
		break;
	case 90:
		ys += 14;
		ye += 14;
		break;
	case 180:
		xs += 12;
		xe += 12;
		break;
	case 270:
		ys += 12;
		ye += 12;
		break;
	default:
		break;
	}

	write_reg(par, 0x2a, (xs >> 8) & 0xff, xs & 0xff,
		  (xe >> 8) & 0xff, xe & 0xff);
	write_reg(par, 0x2b, (ys >> 8) & 0xff, ys & 0xff,
		  (ye >> 8) & 0xff, ye & 0xff);
	write_reg(par, 0x2c);
}

static int set_var(struct fbtft_par *par)
{
	u8 madctl = par->bgr ? 0x08 : 0x00;

	switch (par->info->var.rotate) {
	case 0:
		madctl |= 0xc0;
		break;
	case 90:
		madctl |= 0x60;
		break;
	case 180:
		break;
	case 270:
		madctl |= 0xa0;
		break;
	default:
		return 0;
	}

	write_reg(par, 0x36, madctl);
	return 0;
}

static int set_gamma(struct fbtft_par *par, u32 *curves)
{
	int i;
	int j;

	write_reg(par, 0xff, 0xa5);
	for (i = 0; i < par->gamma.num_curves; i++)
		for (j = 0; j < par->gamma.num_values; j++)
			write_reg(par, 0x60 + 0x10 * i + j,
				  curves[i * par->gamma.num_values + j]);
	write_reg(par, 0xff, 0x00);
	return 0;
}

static int blank(struct fbtft_par *par, bool on)
{
	write_reg(par, on ? 0x28 : 0x29);
	return 0;
}

static struct fbtft_display display = {
	.regwidth = 8,
	.width = WIDTH,
	.height = HEIGHT,
	.txbuflen = TXBUFLEN,
	.gamma_num = 2,
	.gamma_len = 15,
	.gamma = DEFAULT_GAMMA,
	.fbtftops = {
		.init_display = init_display,
		.set_addr_win = set_addr_win,
		.blank = blank,
		.set_var = set_var,
		.set_gamma = set_gamma,
	},
};

FBTFT_REGISTER_SPI_DRIVER(DRVNAME, "newvision", "nv3007", &display);

MODULE_ALIAS("spi:nv3007");
MODULE_ALIAS("platform:nv3007");
MODULE_ALIAS("spi:fb_nv3007");
MODULE_ALIAS("platform:fb_nv3007");
MODULE_AUTHOR("Navas Abubacker <navas@outlook.com>");
MODULE_DESCRIPTION("FB driver for the NewVision NV3007 LCD controller");
MODULE_LICENSE("GPL");
