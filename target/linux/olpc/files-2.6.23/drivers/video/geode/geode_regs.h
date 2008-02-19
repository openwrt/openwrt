/* This header file defines the registers and suspend/resume
   structures for the Geode GX and LX.   The lxfb driver defines
   _GEODELX_ before including this file, which will unlock the
   extra registers that are only valid for LX.
*/

#ifndef _GEODE_REGS_H_
#define _GEODE_REGS_H_

/* MSRs */

#define GX_VP_MSR_PAD_SELECT    0xC0002011
#define LX_VP_MSR_PAD_SELECT    0x48000011

#define GEODE_MSR_GLCP_DOTPLL   0x4c000015

#define GLCP_DOTPLL_RESET    (1 << 0)
#define GLCP_DOTPLL_BYPASS   (1 << 15)
#define GLCP_DOTPLL_HALFPIX  (1 << 24)
#define GLCP_DOTPLL_LOCK     (1 << 25)

/* Registers */
#define VP_FP_START          0x400


#ifdef _GEODELX_

#define GP_REG_SIZE  0x7C
#define DC_REG_SIZE  0xF0
#define VP_REG_SIZE  0x158
#define FP_REG_SIZE  0x70

#else

#define GP_REG_SIZE 0x50
#define DC_REG_SIZE 0x90
#define VP_REG_SIZE 0x138
#define FP_REG_SIZE 0x70

#endif

#define DC_PAL_SIZE 0x105

struct geoderegs {

	struct {
		u64 padsel;
		u64 dotpll;

#ifdef _GEODELX_
		u64 dfglcfg;
		u64 dcspare;
#else
		u64 rstpll;
#endif
	} msr;

	union {
		unsigned char b[GP_REG_SIZE];
		struct {
			u32 dst_offset;         /* 0x00 */
			u32 src_offset;         /* 0x04 */
			u32 stride;             /* 0x08 */
			u32 wid_height;         /* 0x0C */
			u32 src_color_fg;       /* 0x10 */
			u32 src_color_bg;       /* 0x14 */
			u32 pat_color_0;        /* 0x18 */
			u32 pat_color_1;        /* 0x1C */
			u32 pat_color_2;        /* 0x20 */
			u32 pat_color_3;        /* 0x24 */
			u32 pat_color_4;        /* 0x28 */
			u32 pat_color_5;        /* 0x2C */
			u32 pat_data_0;         /* 0x30 */
			u32 pat_data_1;         /* 0x34 */
			u32 raster_mode;        /* 0x38 */
			u32 vector_mode;        /* 0x3C */
			u32 blt_mode;           /* 0x40 */
			u32 blit_status;        /* 0x4C */
			u32 hst_src;            /* 0x48 */
			u32 base_offset;        /* 0x4C */

#ifdef _GEODELX_
			u32 cmd_top;            /* 0x50 */
			u32 cmd_bot;            /* 0x54 */
			u32 cmd_read;           /* 0x58 */
			u32 cmd_write;          /* 0x5C */
			u32 ch3_offset;         /* 0x60 */
			u32 ch3_mode_str;       /* 0x64 */
			u32 ch3_width;          /* 0x68 */
			u32 ch3_hsrc;           /* 0x6C */
			u32 lut_index;          /* 0x70 */
			u32 lut_data;           /* 0x74 */
			u32 int_cntrl;          /* 0x78 */
#endif
		} r;
	} gp;

	union {
		unsigned char b[DC_REG_SIZE];

		struct {
			u32 unlock;             /* 0x00 */
			u32 gcfg;               /* 0x04 */
			u32 dcfg;               /* 0x08 */
			u32 arb;                /* 0x0C */
			u32 fb_st_offset;       /* 0x10 */
			u32 cb_st_offset;       /* 0x14 */
			u32 curs_st_offset;     /* 0x18 */
			u32 icon_st_offset;     /* 0x1C */
			u32 vid_y_st_offset;    /* 0x20 */
			u32 vid_u_st_offset;    /* 0x24 */
			u32 vid_v_st_offset;    /* 0x28 */
			u32 dctop;              /* 0x2c */
			u32 line_size;          /* 0x30 */
			u32 gfx_pitch;          /* 0x34 */
			u32 vid_yuv_pitch;      /* 0x38 */
			u32 rsvd2;              /* 0x3C */
			u32 h_active_timing;    /* 0x40 */
			u32 h_blank_timing;     /* 0x44 */
			u32 h_sync_timing;      /* 0x48 */
			u32 rsvd3;              /* 0x4C */
			u32 v_active_timing;    /* 0x50 */
			u32 v_blank_timing;     /* 0x54 */
			u32 v_sync_timing;      /* 0x58 */
			u32 fbactive;           /* 0x5C */
			u32 dc_cursor_x;        /* 0x60 */
			u32 dc_cursor_y;        /* 0x64 */
			u32 dc_icon_x;          /* 0x68 */
			u32 dc_line_cnt;        /* 0x6C */
			u32 rsvd5;              /* 0x70 - palette address */
			u32 rsvd6;              /* 0x74 - palette data */
			u32 dfifo_diag;         /* 0x78 */
			u32 cfifo_diag;         /* 0x7C */
			u32 dc_vid_ds_delta;    /* 0x80 */
			u32 gliu0_mem_offset;   /* 0x84 */
			u32 dv_ctl;             /* 0x88 - added by LX */
			u32 dv_acc;             /* 0x8C */

#ifdef _GEODELX_
			u32 gfx_scale;
			u32 irq_filt_ctl;
			u32 filt_coeff1;
			u32 filt_coeff2;
			u32 vbi_event_ctl;
			u32 vbi_odd_ctl;
			u32 vbi_hor;
			u32 vbi_ln_odd;
			u32 vbi_ln_event;
			u32 vbi_pitch;
			u32 clr_key;
			u32 clr_key_mask;
			u32 clr_key_x;
			u32 clr_key_y;
			u32 irq;
			u32 rsvd8;
			u32 genlk_ctrl;
			u32 vid_even_y_st_offset;    /* 0xD8 */
			u32 vid_even_u_st_offset;    /* 0xDC */
			u32 vid_even_v_st_offset;    /* 0xE0 */
			u32 v_active_even_timing;    /* 0xE4 */
			u32 v_blank_even_timing;     /* 0xE8 */
			u32 v_sync_even_timing;      /* 0xEC */
#endif
		} r;
	} dc;

	union {
		unsigned char b[VP_REG_SIZE];

		struct {
			u64 vcfg;               /* 0x00 */
			u64 dcfg;               /* 0x08 */
			u64 vx;                 /* 0x10 */
			u64 vy;                 /* 0x18 */
			u64 vs;                 /* 0x20 */
			u64 vck;                /* 0x28 */
			u64 vcm;                /* 0x30 */
			u64 rsvd1;              /* 0x38 - Gamma address*/
			u64 rsvd2;              /* 0x40 - Gamma data*/
			u64 rsvd3;              /* 0x48 */
			u64 misc;               /* 0x50 */
			u64 ccs;                /* 0x58 */
			u64 rsvd4[3];           /* 0x60-0x70 */
			u64 vdc;                /* 0x78 */
			u64 vco;                /* 0x80 */
			u64 crc;                /* 0x88 */
			u64 crc32;              /* 0x90 */
			u64 vde;                /* 0x98 */
			u64 cck;                /* 0xA0 */
			u64 ccm;                /* 0xA8 */
			u64 cc1;                /* 0xB0 */
			u64 cc2;                /* 0xB8 */
			u64 a1x;                /* 0xC0 */
			u64 a1y;                /* 0xC8 */
			u64 a1c;                /* 0xD0 */
			u64 a1t;                /* 0xD8 */
			u64 a2x;                /* 0xE0 */
			u64 a2y;                /* 0xE8 */
			u64 a2c;                /* 0xF0 */
			u64 a2t;                /* 0xF8 */
			u64 a3x;                /* 0x100 */
			u64 a3y;                /* 0x108 */
			u64 a3c;                /* 0x110 */
			u64 a3t;                /* 0x118 */
			u64 vrr;                /* 0x120 */
			u64 awt;                /* 0x128 */
			u64 vtm;                /* 0x130 */
#ifdef _GEODELX_
			u64 vye;                /* 0x138 */
			u64 a1ye;               /* 0x140 */
			u32 a2ye;               /* 0x148 */
			u32 a3ye;	        /* 0x150 */
#endif
		} r;
	} vp;

	union {
		unsigned char b[FP_REG_SIZE];

		struct {
			u64 pt1;                /* 0x400 */
			u64 pt2;                /* 0x408 */
			u64 pm;                 /* 0x410 */
			u64 dfc;                /* 0x418 */
			u64 blfsr;              /* 0x420 */
			u64 rlfsr;              /* 0x428 */
			u64 fmi;                /* 0x430 */
			u64 fmd;                /* 0x438 */
			u64 rsvd;               /* 0x440 */
			u64 dca;                /* 0x448 */
			u64 dmd;                /* 0x450 */
			u64 crc;                /* 0x458 */
			u64 fbb;                /* 0x460 */
			u64 crc32;              /* 0x468 */
		} r;
	} fp;

	u32 pal[DC_PAL_SIZE];
	u32 gamma[256];
};

#endif
