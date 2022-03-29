/*
 * linux/drivers/video/fb_sys_read.c - Generic file operations where
 * framebuffer is in system RAM
 *
 * Copyright (C) 2007 Antonino Daplas <adaplas@pol.net>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 *
 */
#include <linux/fb.h>
#include <linux/module.h>
#include <linux/uaccess.h>

static ssize_t
fbusb_read(struct fb_info *info, char __user *buf, size_t count,
		    loff_t *ppos)
{
	unsigned long p = *ppos;
	void *src;
	int err = 0;
	unsigned long total_size;

	if (info->state != FBINFO_STATE_RUNNING)
		return -EPERM;

	total_size = info->screen_size;

	if (total_size == 0)
		total_size = info->fix.smem_len;

	if (p >= total_size)
		return 0;

	if (count >= total_size)
		count = total_size;

	if (count + p > total_size)
		count = total_size - p;

	src = (void __force *)(info->screen_base + p);

	if (info->fbops->fb_sync)
		info->fbops->fb_sync(info);

	if (copy_to_user(buf, src, count))
		err = -EFAULT;

	if  (!err)
		*ppos += count;

	return (err) ? err : count;
}

static ssize_t
fbusb_write(struct fb_info *info, const char __user *buf,
		     size_t count, loff_t *ppos)
{
	unsigned long p = *ppos;
	void *dst;
	int err = 0;
	unsigned long total_size;

	if (info->state != FBINFO_STATE_RUNNING)
		return -EPERM;

	total_size = info->screen_size;

	if (total_size == 0)
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

	dst = (void __force *) (info->screen_base + p);

	if (info->fbops->fb_sync)
		info->fbops->fb_sync(info);

	if (copy_from_user(dst, buf, count))
		err = -EFAULT;

	if  (!err)
		*ppos += count;

	return (err) ? err : count;
}

/*
 *  Generic Bit Block Transfer for frame buffers located in system RAM with
 *  packed pixels of any depth.
 *
 *  Based almost entirely from cfbcopyarea.c (which is based almost entirely
 *  on Geert Uytterhoeven's copyarea routine)
 *
 *      Copyright (C)  2007 Antonino Daplas <adaplas@pol.net>
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file COPYING in the main directory of this archive for
 *  more details.
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fb.h>
#include <asm/types.h>
#include <asm/io.h>
#include "core/fb_draw.h"

    /*
     *  Generic bitwise copy algorithm
     */

static void
bitcpy(struct fb_info *p, unsigned long *dst, unsigned dst_idx,
	const unsigned long *src, unsigned src_idx, int bits, unsigned n)
{
	unsigned long first, last;
	int const shift = dst_idx-src_idx;
	int left, right;

	first = FB_SHIFT_HIGH(p, ~0UL, dst_idx);
	last = ~(FB_SHIFT_HIGH(p, ~0UL, (dst_idx+n) % bits));

	if (!shift) {
		/* Same alignment for source and dest */
		if (dst_idx+n <= bits) {
			/* Single word */
			if (last)
				first &= last;
			*dst = comp(*src, *dst, first);
		} else {
			/* Multiple destination words */
			/* Leading bits */
			if (first != ~0UL) {
				*dst = comp(*src, *dst, first);
				dst++;
				src++;
				n -= bits - dst_idx;
			}

			/* Main chunk */
			n /= bits;
			while (n >= 8) {
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				n -= 8;
			}
			while (n--)
				*dst++ = *src++;

			/* Trailing bits */
			if (last)
				*dst = comp(*src, *dst, last);
		}
	} else {
		unsigned long d0, d1;
		int m;

		/* Different alignment for source and dest */
		right = shift & (bits - 1);
		left = -shift & (bits - 1);

		if (dst_idx+n <= bits) {
			/* Single destination word */
			if (last)
				first &= last;
			if (shift > 0) {
				/* Single source word */
				*dst = comp(*src << left, *dst, first);
			} else if (src_idx+n <= bits) {
				/* Single source word */
				*dst = comp(*src >> right, *dst, first);
			} else {
				/* 2 source words */
				d0 = *src++;
				d1 = *src;
				*dst = comp(d0 >> right | d1 << left, *dst,
					    first);
			}
		} else {
			/* Multiple destination words */
			/** We must always remember the last value read,
			    because in case SRC and DST overlap bitwise (e.g.
			    when moving just one pixel in 1bpp), we always
			    collect one full long for DST and that might
			    overlap with the current long from SRC. We store
			    this value in 'd0'. */
			d0 = *src++;
			/* Leading bits */
			if (shift > 0) {
				/* Single source word */
				*dst = comp(d0 << left, *dst, first);
				dst++;
				n -= bits - dst_idx;
			} else {
				/* 2 source words */
				d1 = *src++;
				*dst = comp(d0 >> right | d1 << left, *dst,
					    first);
				d0 = d1;
				dst++;
				n -= bits - dst_idx;
			}

			/* Main chunk */
			m = n % bits;
			n /= bits;
			while (n >= 4) {
				d1 = *src++;
				*dst++ = d0 >> right | d1 << left;
				d0 = d1;
				d1 = *src++;
				*dst++ = d0 >> right | d1 << left;
				d0 = d1;
				d1 = *src++;
				*dst++ = d0 >> right | d1 << left;
				d0 = d1;
				d1 = *src++;
				*dst++ = d0 >> right | d1 << left;
				d0 = d1;
				n -= 4;
			}
			while (n--) {
				d1 = *src++;
				*dst++ = d0 >> right | d1 << left;
				d0 = d1;
			}

			/* Trailing bits */
			if (m) {
				if (m <= bits - right) {
					/* Single source word */
					d0 >>= right;
				} else {
					/* 2 source words */
					d1 = *src;
					d0 = d0 >> right | d1 << left;
				}
				*dst = comp(d0, *dst, last);
			}
		}
	}
}

    /*
     *  Generic bitwise copy algorithm, operating backward
     */

static void
bitcpy_rev(struct fb_info *p, unsigned long *dst, unsigned dst_idx,
	   const unsigned long *src, unsigned src_idx, unsigned bits,
	   unsigned n)
{
	unsigned long first, last;
	int shift;

	dst += (dst_idx + n - 1) / bits;
	src += (src_idx + n - 1) / bits;
	dst_idx = (dst_idx + n - 1) % bits;
	src_idx = (src_idx + n - 1) % bits;

	shift = dst_idx-src_idx;

	first = ~FB_SHIFT_HIGH(p, ~0UL, (dst_idx + 1) % bits);
	last = FB_SHIFT_HIGH(p, ~0UL, (bits + dst_idx + 1 - n) % bits);

	if (!shift) {
		/* Same alignment for source and dest */
		if ((unsigned long)dst_idx+1 >= n) {
			/* Single word */
			if (first)
				last &= first;
			*dst = comp(*src, *dst, last);
		} else {
			/* Multiple destination words */

			/* Leading bits */
			if (first) {
				*dst = comp(*src, *dst, first);
				dst--;
				src--;
				n -= dst_idx+1;
			}

			/* Main chunk */
			n /= bits;
			while (n >= 8) {
				*dst-- = *src--;
				*dst-- = *src--;
				*dst-- = *src--;
				*dst-- = *src--;
				*dst-- = *src--;
				*dst-- = *src--;
				*dst-- = *src--;
				*dst-- = *src--;
				n -= 8;
			}
			while (n--)
				*dst-- = *src--;
			/* Trailing bits */
			if (last != -1UL)
				*dst = comp(*src, *dst, last);
		}
	} else {
		/* Different alignment for source and dest */

		int const left = shift & (bits-1);
		int const right = -shift & (bits-1);

		if ((unsigned long)dst_idx+1 >= n) {
			/* Single destination word */
			if (first)
				last &= first;
			if (shift < 0) {
				/* Single source word */
				*dst = comp(*src >> right, *dst, last);
			} else if (1+(unsigned long)src_idx >= n) {
				/* Single source word */
				*dst = comp(*src << left, *dst, last);
			} else {
				/* 2 source words */
				*dst = comp(*src << left | *(src-1) >> right,
					    *dst, last);
			}
		} else {
			/* Multiple destination words */
			/** We must always remember the last value read,
			    because in case SRC and DST overlap bitwise (e.g.
			    when moving just one pixel in 1bpp), we always
			    collect one full long for DST and that might
			    overlap with the current long from SRC. We store
			    this value in 'd0'. */
			unsigned long d0, d1;
			int m;

			d0 = *src--;
			/* Leading bits */
			if (shift < 0) {
				/* Single source word */
				d1 = d0;
				d0 >>= right;
			} else {
				/* 2 source words */
				d1 = *src--;
				d0 = d0 << left | d1 >> right;
			}
			if (!first)
				*dst = d0;
			else
				*dst = comp(d0, *dst, first);
			d0 = d1;
			dst--;
			n -= dst_idx+1;

			/* Main chunk */
			m = n % bits;
			n /= bits;
			while (n >= 4) {
				d1 = *src--;
				*dst-- = d0 << left | d1 >> right;
				d0 = d1;
				d1 = *src--;
				*dst-- = d0 << left | d1 >> right;
				d0 = d1;
				d1 = *src--;
				*dst-- = d0 << left | d1 >> right;
				d0 = d1;
				d1 = *src--;
				*dst-- = d0 << left | d1 >> right;
				d0 = d1;
				n -= 4;
			}
			while (n--) {
				d1 = *src--;
				*dst-- = d0 << left | d1 >> right;
				d0 = d1;
			}

			/* Trailing bits */
			if (m) {
				if (m <= bits - left) {
					/* Single source word */
					d0 <<= left;
				} else {
					/* 2 source words */
					d1 = *src;
					d0 = d0 << left | d1 >> right;
				}
				*dst = comp(d0, *dst, last);
			}
		}
	}
}

static void
fbusb_copyarea(struct fb_info *p, const struct fb_copyarea *area)
{
	u32 dx = area->dx, dy = area->dy, sx = area->sx, sy = area->sy;
	u32 height = area->height, width = area->width;
	unsigned long const bits_per_line = p->fix.line_length*8u;
	unsigned long *base = NULL;
	int bits = BITS_PER_LONG, bytes = bits >> 3;
	unsigned dst_idx = 0, src_idx = 0, rev_copy = 0;

	if (p->state != FBINFO_STATE_RUNNING)
		return;

	/* if the beginning of the target area might overlap with the end of
	the source area, be have to copy the area reverse. */
	if ((dy == sy && dx > sx) || (dy > sy)) {
		dy += height;
		sy += height;
		rev_copy = 1;
	}

	/* split the base of the framebuffer into a long-aligned address and
	   the index of the first bit */
	base = (unsigned long *)((unsigned long)p->screen_base & ~(bytes-1));
	dst_idx = src_idx = 8*((unsigned long)p->screen_base & (bytes-1));
	/* add offset of source and target area */
	dst_idx += dy*bits_per_line + dx*p->var.bits_per_pixel;
	src_idx += sy*bits_per_line + sx*p->var.bits_per_pixel;

	if (p->fbops->fb_sync)
		p->fbops->fb_sync(p);

	if (rev_copy) {
		while (height--) {
			dst_idx -= bits_per_line;
			src_idx -= bits_per_line;
			bitcpy_rev(p, base + (dst_idx / bits), dst_idx % bits,
				base + (src_idx / bits), src_idx % bits, bits,
				width*p->var.bits_per_pixel);
		}
	} else {
		while (height--) {
			bitcpy(p, base + (dst_idx / bits), dst_idx % bits,
				base + (src_idx / bits), src_idx % bits, bits,
				width*p->var.bits_per_pixel);
			dst_idx += bits_per_line;
			src_idx += bits_per_line;
		}
	}
}

/*
 *  Generic fillrect for frame buffers in system RAM with packed pixels of
 *  any depth.
 *
 *  Based almost entirely from cfbfillrect.c (which is based almost entirely
 *  on Geert Uytterhoeven's fillrect routine)
 *
 *      Copyright (C)  2007 Antonino Daplas <adaplas@pol.net>
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file COPYING in the main directory of this archive for
 *  more details.
 */
#include <linux/module.h>
#include <linux/string.h>
#include <linux/fb.h>
#include <asm/types.h>
#include "core/fb_draw.h"

    /*
     *  Aligned pattern fill using 32/64-bit memory accesses
     */

static void
bitfill_aligned(struct fb_info *p, unsigned long *dst, int dst_idx,
		unsigned long pat, unsigned n, int bits)
{
	unsigned long first, last;

	if (!n)
		return;

	first = FB_SHIFT_HIGH(p, ~0UL, dst_idx);
	last = ~(FB_SHIFT_HIGH(p, ~0UL, (dst_idx+n) % bits));

	if (dst_idx+n <= bits) {
		/* Single word */
		if (last)
			first &= last;
		*dst = comp(pat, *dst, first);
	} else {
		/* Multiple destination words */

		/* Leading bits */
		if (first!= ~0UL) {
			*dst = comp(pat, *dst, first);
			dst++;
			n -= bits - dst_idx;
		}

		/* Main chunk */
		n /= bits;
		while (n >= 8) {
			*dst++ = pat;
			*dst++ = pat;
			*dst++ = pat;
			*dst++ = pat;
			*dst++ = pat;
			*dst++ = pat;
			*dst++ = pat;
			*dst++ = pat;
			n -= 8;
		}
		while (n--)
			*dst++ = pat;
		/* Trailing bits */
		if (last)
			*dst = comp(pat, *dst, last);
	}
}


    /*
     *  Unaligned generic pattern fill using 32/64-bit memory accesses
     *  The pattern must have been expanded to a full 32/64-bit value
     *  Left/right are the appropriate shifts to convert to the pattern to be
     *  used for the next 32/64-bit word
     */

static void
bitfill_unaligned(struct fb_info *p, unsigned long *dst, int dst_idx,
		  unsigned long pat, int left, int right, unsigned n, int bits)
{
	unsigned long first, last;

	if (!n)
		return;

	first = FB_SHIFT_HIGH(p, ~0UL, dst_idx);
	last = ~(FB_SHIFT_HIGH(p, ~0UL, (dst_idx+n) % bits));

	if (dst_idx+n <= bits) {
		/* Single word */
		if (last)
			first &= last;
		*dst = comp(pat, *dst, first);
	} else {
		/* Multiple destination words */
		/* Leading bits */
		if (first) {
			*dst = comp(pat, *dst, first);
			dst++;
			pat = pat << left | pat >> right;
			n -= bits - dst_idx;
		}

		/* Main chunk */
		n /= bits;
		while (n >= 4) {
			*dst++ = pat;
			pat = pat << left | pat >> right;
			*dst++ = pat;
			pat = pat << left | pat >> right;
			*dst++ = pat;
			pat = pat << left | pat >> right;
			*dst++ = pat;
			pat = pat << left | pat >> right;
			n -= 4;
		}
		while (n--) {
			*dst++ = pat;
			pat = pat << left | pat >> right;
		}

		/* Trailing bits */
		if (last)
			*dst = comp(pat, *dst, last);
	}
}

    /*
     *  Aligned pattern invert using 32/64-bit memory accesses
     */
static void
bitfill_aligned_rev(struct fb_info *p, unsigned long *dst, int dst_idx,
		    unsigned long pat, unsigned n, int bits)
{
	unsigned long val = pat;
	unsigned long first, last;

	if (!n)
		return;

	first = FB_SHIFT_HIGH(p, ~0UL, dst_idx);
	last = ~(FB_SHIFT_HIGH(p, ~0UL, (dst_idx+n) % bits));

	if (dst_idx+n <= bits) {
		/* Single word */
		if (last)
			first &= last;
		*dst = comp(*dst ^ val, *dst, first);
	} else {
		/* Multiple destination words */
		/* Leading bits */
		if (first!=0UL) {
			*dst = comp(*dst ^ val, *dst, first);
			dst++;
			n -= bits - dst_idx;
		}

		/* Main chunk */
		n /= bits;
		while (n >= 8) {
			*dst++ ^= val;
			*dst++ ^= val;
			*dst++ ^= val;
			*dst++ ^= val;
			*dst++ ^= val;
			*dst++ ^= val;
			*dst++ ^= val;
			*dst++ ^= val;
			n -= 8;
		}
		while (n--)
			*dst++ ^= val;
		/* Trailing bits */
		if (last)
			*dst = comp(*dst ^ val, *dst, last);
	}
}


    /*
     *  Unaligned generic pattern invert using 32/64-bit memory accesses
     *  The pattern must have been expanded to a full 32/64-bit value
     *  Left/right are the appropriate shifts to convert to the pattern to be
     *  used for the next 32/64-bit word
     */

static void
bitfill_unaligned_rev(struct fb_info *p, unsigned long *dst, int dst_idx,
		      unsigned long pat, int left, int right, unsigned n,
		      int bits)
{
	unsigned long first, last;

	if (!n)
		return;

	first = FB_SHIFT_HIGH(p, ~0UL, dst_idx);
	last = ~(FB_SHIFT_HIGH(p, ~0UL, (dst_idx+n) % bits));

	if (dst_idx+n <= bits) {
		/* Single word */
		if (last)
			first &= last;
		*dst = comp(*dst ^ pat, *dst, first);
	} else {
		/* Multiple destination words */

		/* Leading bits */
		if (first != 0UL) {
			*dst = comp(*dst ^ pat, *dst, first);
			dst++;
			pat = pat << left | pat >> right;
			n -= bits - dst_idx;
		}

		/* Main chunk */
		n /= bits;
		while (n >= 4) {
			*dst++ ^= pat;
			pat = pat << left | pat >> right;
			*dst++ ^= pat;
			pat = pat << left | pat >> right;
			*dst++ ^= pat;
			pat = pat << left | pat >> right;
			*dst++ ^= pat;
			pat = pat << left | pat >> right;
			n -= 4;
		}
		while (n--) {
			*dst ^= pat;
			pat = pat << left | pat >> right;
		}

		/* Trailing bits */
		if (last)
			*dst = comp(*dst ^ pat, *dst, last);
	}
}

static void
fbusb_fillrect(struct fb_info *p, const struct fb_fillrect *rect)
{
	unsigned long pat, pat2, fg;
	unsigned long width = rect->width, height = rect->height;
	int bits = BITS_PER_LONG, bytes = bits >> 3;
	u32 bpp = p->var.bits_per_pixel;
	unsigned long *dst;
	int dst_idx, left;

	if (p->state != FBINFO_STATE_RUNNING)
		return;

	if (p->fix.visual == FB_VISUAL_TRUECOLOR ||
	    p->fix.visual == FB_VISUAL_DIRECTCOLOR )
		fg = ((u32 *) (p->pseudo_palette))[rect->color];
	else
		fg = rect->color;

	pat = pixel_to_pat( bpp, fg);

	dst = (unsigned long *)((unsigned long)p->screen_base & ~(bytes-1));
	dst_idx = ((unsigned long)p->screen_base & (bytes - 1))*8;
	dst_idx += rect->dy*p->fix.line_length*8+rect->dx*bpp;
	/* FIXME For now we support 1-32 bpp only */
	left = bits % bpp;
	if (p->fbops->fb_sync)
		p->fbops->fb_sync(p);
	if (!left) {
		void (*fill_op32)(struct fb_info *p, unsigned long *dst,
				  int dst_idx, unsigned long pat, unsigned n,
				  int bits) = NULL;

		switch (rect->rop) {
		case ROP_XOR:
			fill_op32 = bitfill_aligned_rev;
			break;
		case ROP_COPY:
			fill_op32 = bitfill_aligned;
			break;
		default:
			printk( KERN_ERR "cfb_fillrect(): unknown rop, "
				"defaulting to ROP_COPY\n");
			fill_op32 = bitfill_aligned;
			break;
		}
		while (height--) {
			dst += dst_idx >> (ffs(bits) - 1);
			dst_idx &= (bits - 1);
			fill_op32(p, dst, dst_idx, pat, width*bpp, bits);
			dst_idx += p->fix.line_length*8;
		}
	} else {
		int right, r;
		void (*fill_op)(struct fb_info *p, unsigned long *dst,
				int dst_idx, unsigned long pat, int left,
				int right, unsigned n, int bits) = NULL;
#ifdef __LITTLE_ENDIAN
		right = left;
		left = bpp - right;
#else
		right = bpp - left;
#endif
		switch (rect->rop) {
		case ROP_XOR:
			fill_op = bitfill_unaligned_rev;
			break;
		case ROP_COPY:
			fill_op = bitfill_unaligned;
			break;
		default:
			printk(KERN_ERR "sys_fillrect(): unknown rop, "
				"defaulting to ROP_COPY\n");
			fill_op = bitfill_unaligned;
			break;
		}
		while (height--) {
			dst += dst_idx / bits;
			dst_idx &= (bits - 1);
			r = dst_idx % bpp;
			/* rotate pattern to the correct start position */
			pat2 = le_long_to_cpu(rolx(cpu_to_le_long(pat), r, bpp));
			fill_op(p, dst, dst_idx, pat2, left, right,
				width*bpp, bits);
			dst_idx += p->fix.line_length*8;
		}
	}
}

/*
 *  Generic 1-bit or 8-bit source to 1-32 bit destination expansion
 *  for frame buffer located in system RAM with packed pixels of any depth.
 *
 *  Based almost entirely on cfbimgblt.c
 *
 *      Copyright (C)  April 2007 Antonino Daplas <adaplas@pol.net>
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file COPYING in the main directory of this archive for
 *  more details.
 */
#include <linux/module.h>
#include <linux/string.h>
#include <linux/fb.h>
#include <asm/types.h>

static const u32 cfb_tab8_be[] = {
    0x00000000,0x000000ff,0x0000ff00,0x0000ffff,
    0x00ff0000,0x00ff00ff,0x00ffff00,0x00ffffff,
    0xff000000,0xff0000ff,0xff00ff00,0xff00ffff,
    0xffff0000,0xffff00ff,0xffffff00,0xffffffff
};

static const u32 cfb_tab8_le[] = {
    0x00000000,0xff000000,0x00ff0000,0xffff0000,
    0x0000ff00,0xff00ff00,0x00ffff00,0xffffff00,
    0x000000ff,0xff0000ff,0x00ff00ff,0xffff00ff,
    0x0000ffff,0xff00ffff,0x00ffffff,0xffffffff
};

static const u32 cfb_tab16_be[] = {
    0x00000000, 0x0000ffff, 0xffff0000, 0xffffffff
};

static const u32 cfb_tab16_le[] = {
    0x00000000, 0xffff0000, 0x0000ffff, 0xffffffff
};

static const u32 cfb_tab32[] = {
	0x00000000, 0xffffffff
};

static void color_imageblit(const struct fb_image *image, struct fb_info *p,
			    void *dst1, u32 start_index, u32 pitch_index)
{
	/* Draw the penguin */
	u32 *dst, *dst2;
	u32 color = 0, val, shift;
	int i, n, bpp = p->var.bits_per_pixel;
	u32 null_bits = 32 - bpp;
	u32 *palette = (u32 *) p->pseudo_palette;
	const u8 *src = image->data;

	dst2 = dst1;
	for (i = image->height; i--; ) {
		n = image->width;
		dst = dst1;
		shift = 0;
		val = 0;

		if (start_index) {
			u32 start_mask = ~(FB_SHIFT_HIGH(p, ~(u32)0,
							 start_index));
			val = *dst & start_mask;
			shift = start_index;
		}
		while (n--) {
			if (p->fix.visual == FB_VISUAL_TRUECOLOR ||
			    p->fix.visual == FB_VISUAL_DIRECTCOLOR )
				color = palette[*src];
			else
				color = *src;
			color <<= FB_LEFT_POS(p, bpp);
			val |= FB_SHIFT_HIGH(p, color, shift);
			if (shift >= null_bits) {
				*dst++ = val;

				val = (shift == null_bits) ? 0 :
					FB_SHIFT_LOW(p, color, 32 - shift);
			}
			shift += bpp;
			shift &= (32 - 1);
			src++;
		}
		if (shift) {
			u32 end_mask = FB_SHIFT_HIGH(p, ~(u32)0, shift);

			*dst &= end_mask;
			*dst |= val;
		}
		dst1 += p->fix.line_length;
		if (pitch_index) {
			dst2 += p->fix.line_length;
			dst1 = (u8 *)((long)dst2 & ~(sizeof(u32) - 1));

			start_index += pitch_index;
			start_index &= 32 - 1;
		}
	}
}

static void slow_imageblit(const struct fb_image *image, struct fb_info *p,
				  void *dst1, u32 fgcolor, u32 bgcolor,
				  u32 start_index, u32 pitch_index)
{
	u32 shift, color = 0, bpp = p->var.bits_per_pixel;
	u32 *dst, *dst2;
	u32 val, pitch = p->fix.line_length;
	u32 null_bits = 32 - bpp;
	u32 spitch = (image->width+7)/8;
	const u8 *src = image->data, *s;
	u32 i, j, l;

	dst2 = dst1;
	fgcolor <<= FB_LEFT_POS(p, bpp);
	bgcolor <<= FB_LEFT_POS(p, bpp);

	for (i = image->height; i--; ) {
		shift = val = 0;
		l = 8;
		j = image->width;
		dst = dst1;
		s = src;

		/* write leading bits */
		if (start_index) {
			u32 start_mask = ~(FB_SHIFT_HIGH(p, ~(u32)0,
							 start_index));
			val = *dst & start_mask;
			shift = start_index;
		}

		while (j--) {
			l--;
			color = (*s & (1 << l)) ? fgcolor : bgcolor;
			val |= FB_SHIFT_HIGH(p, color, shift);

			/* Did the bitshift spill bits to the next long? */
			if (shift >= null_bits) {
				*dst++ = val;
				val = (shift == null_bits) ? 0 :
					FB_SHIFT_LOW(p, color, 32 - shift);
			}
			shift += bpp;
			shift &= (32 - 1);
			if (!l) { l = 8; s++; }
		}

		/* write trailing bits */
		if (shift) {
			u32 end_mask = FB_SHIFT_HIGH(p, ~(u32)0, shift);

			*dst &= end_mask;
			*dst |= val;
		}

		dst1 += pitch;
		src += spitch;
		if (pitch_index) {
			dst2 += pitch;
			dst1 = (u8 *)((long)dst2 & ~(sizeof(u32) - 1));
			start_index += pitch_index;
			start_index &= 32 - 1;
		}

	}
}

/*
 * fast_imageblit - optimized monochrome color expansion
 *
 * Only if:  bits_per_pixel == 8, 16, or 32
 *           image->width is divisible by pixel/dword (ppw);
 *           fix->line_legth is divisible by 4;
 *           beginning and end of a scanline is dword aligned
 */
static void fast_imageblit(const struct fb_image *image, struct fb_info *p,
				  void *dst1, u32 fgcolor, u32 bgcolor)
{
	u32 fgx = fgcolor, bgx = bgcolor, bpp = p->var.bits_per_pixel;
	u32 ppw = 32/bpp, spitch = (image->width + 7)/8;
	u32 bit_mask, end_mask, eorx, shift;
	const char *s = image->data, *src;
	u32 *dst;
	const u32 *tab = NULL;
	int i, j, k;

	switch (bpp) {
	case 8:
		tab = fb_be_math(p) ? cfb_tab8_be : cfb_tab8_le;
		break;
	case 16:
		tab = fb_be_math(p) ? cfb_tab16_be : cfb_tab16_le;
		break;
	case 32:
	default:
		tab = cfb_tab32;
		break;
	}

	for (i = ppw-1; i--; ) {
		fgx <<= bpp;
		bgx <<= bpp;
		fgx |= fgcolor;
		bgx |= bgcolor;
	}

	bit_mask = (1 << ppw) - 1;
	eorx = fgx ^ bgx;
	k = image->width/ppw;

	for (i = image->height; i--; ) {
		dst = dst1;
		shift = 8;
		src = s;

		for (j = k; j--; ) {
			shift -= ppw;
			end_mask = tab[(*src >> shift) & bit_mask];
			*dst++ = (end_mask & eorx) ^ bgx;
			if (!shift) {
				shift = 8;
				src++;
			}
		}
		dst1 += p->fix.line_length;
		s += spitch;
	}
}

static void
fbusb_imageblit(struct fb_info *p, const struct fb_image *image)
{
	u32 fgcolor, bgcolor, start_index, bitstart, pitch_index = 0;
	u32 bpl = sizeof(u32), bpp = p->var.bits_per_pixel;
	u32 width = image->width;
	u32 dx = image->dx, dy = image->dy;
	void *dst1;

	if (p->state != FBINFO_STATE_RUNNING)
		return;

	bitstart = (dy * p->fix.line_length * 8) + (dx * bpp);
	start_index = bitstart & (32 - 1);
	pitch_index = (p->fix.line_length & (bpl - 1)) * 8;

	bitstart /= 8;
	bitstart &= ~(bpl - 1);
	dst1 = (void __force *)p->screen_base + bitstart;

	if (p->fbops->fb_sync)
		p->fbops->fb_sync(p);

	if (image->depth == 1) {
		if (p->fix.visual == FB_VISUAL_TRUECOLOR ||
		    p->fix.visual == FB_VISUAL_DIRECTCOLOR) {
			fgcolor = ((u32*)(p->pseudo_palette))[image->fg_color];
			bgcolor = ((u32*)(p->pseudo_palette))[image->bg_color];
		} else {
			fgcolor = image->fg_color;
			bgcolor = image->bg_color;
		}

		if (32 % bpp == 0 && !start_index && !pitch_index &&
		    ((width & (32/bpp-1)) == 0) &&
		    bpp >= 8 && bpp <= 32)
			fast_imageblit(image, p, dst1, fgcolor, bgcolor);
		else
			slow_imageblit(image, p, dst1, fgcolor, bgcolor,
					start_index, pitch_index);
	} else
		color_imageblit(image, p, dst1, start_index, pitch_index);
}
