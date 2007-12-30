#include <linux/fb.h>
#include <asm/io.h>
#include <asm/msr.h>

#include "geodefb.h"
#include "video_gx.h"

void gx_set_dotpll(struct fb_info *info, struct geoderegs *regs)
{
	int timeout = 1000;

	u64 rstpll, dotpll;
	
	rdmsrl(MSR_GLCP_SYS_RSTPLL, rstpll);
	rdmsrl(MSR_GLCP_DOTPLL, dotpll);

	dotpll &= 0x00000000ffffffffull;
	dotpll |= regs->msr.dotpll & 0xffffffff00000000ull;
	
	dotpll |= MSR_GLCP_DOTPLL_DOTRESET;
	dotpll &= ~MSR_GLCP_DOTPLL_BYPASS;
	
	wrmsrl(MSR_GLCP_DOTPLL, dotpll);

	rstpll |= (regs->msr.rstpll & 
		   ( MSR_GLCP_SYS_RSTPLL_DOTPREDIV2  |
		     MSR_GLCP_SYS_RSTPLL_DOTPREMULT2 |
		     MSR_GLCP_SYS_RSTPLL_DOTPOSTDIV3));
	
	wrmsrl(MSR_GLCP_SYS_RSTPLL, rstpll);
	dotpll &= ~(MSR_GLCP_DOTPLL_DOTRESET);
	wrmsrl(MSR_GLCP_DOTPLL, dotpll);

	do {
		rdmsrl(MSR_GLCP_DOTPLL, dotpll);
	} while (timeout-- && !(dotpll & MSR_GLCP_DOTPLL_LOCK));
}

/* FIXME: Make sure nothing is read to clear */

void gx_save_regs(struct fb_info *info, struct geoderegs *regs)
{
	struct geodefb_par *par = info->par;
	int i;

	/* Wait for the BLT engine to stop being busy */
	while(readl(par->gp_regs + 0x44) & 0x05);

	rdmsrl(GX_VP_MSR_PAD_SELECT, regs->msr.padsel);
	rdmsrl(MSR_GLCP_DOTPLL, regs->msr.dotpll);
	rdmsrl(MSR_GLCP_SYS_RSTPLL, regs->msr.rstpll);

	writel(0x4758, par->dc_regs + 0x00);

	memcpy(regs->gp.b, par->gp_regs, GP_REG_SIZE);
	memcpy(regs->dc.b, par->dc_regs, DC_REG_SIZE);
	memcpy(regs->vp.b, par->vid_regs, VP_REG_SIZE);
	memcpy(regs->fp.b, par->vid_regs + 0x400, FP_REG_SIZE);

	/* Save the palettes */
	writel(0, par->dc_regs + 0x70);

	for(i = 0; i < DC_PAL_SIZE; i++) 
		regs->pal[i] = readl(par->dc_regs + 0x74);
	
	writel(0, par->vid_regs + 0x38);

	for(i = 0; i < 0xFF; i++)
		regs->gamma[i] = readl(par->vid_regs + 0x40);       	
}

void gx_restore_regs(struct fb_info *info, struct geoderegs *regs)
{
	struct geodefb_par *par = info->par;
	u32 val, i;

	/* DOTPLL */
	gx_set_dotpll(info, regs);

	/* GP */

	writel(regs->gp.r.dst_offset, par->gp_regs + 0x00);
	writel(regs->gp.r.src_offset, par->gp_regs + 0x04);
	writel(regs->gp.r.stride, par->gp_regs + 0x08);
	writel(regs->gp.r.wid_height, par->gp_regs + 0x0C);
	writel(regs->gp.r.src_color_fg, par->gp_regs + 0x10);
	writel(regs->gp.r.src_color_bg, par->gp_regs + 0x14);
	writel(regs->gp.r.pat_color_0, par->gp_regs + 0x18);
	writel(regs->gp.r.pat_color_1, par->gp_regs + 0x1C);
	writel(regs->gp.r.pat_color_2, par->gp_regs + 0x20);
	writel(regs->gp.r.pat_color_3, par->gp_regs + 0x24);
	writel(regs->gp.r.pat_color_4, par->gp_regs + 0x28);
	writel(regs->gp.r.pat_color_5, par->gp_regs + 0x2C);
	writel(regs->gp.r.pat_data_0, par->gp_regs + 0x30);
	writel(regs->gp.r.pat_data_1, par->gp_regs + 0x34);

	/* Don't write the raster / vector / blt mode regs */
	/* status register is read only */

	writel(regs->gp.r.hst_src, par->gp_regs + 0x48);
	writel(regs->gp.r.base_offset, par->gp_regs + 0x4c);
	
	/* DC */
	
	/* Write the unlock value */
	writel(0x4758, par->dc_regs + 0x00);

	writel(0, par->dc_regs + 0x70);
	
	for(i = 0; i < DC_PAL_SIZE; i++)
		writel(regs->pal[i], par->dc_regs + 0x74);

	/* Write the gcfg register without the enables */
	writel(regs->dc.r.gcfg & ~0x0F, par->dc_regs + 0x04);
	
	/* Write the vcfg register without the enables */
	writel(regs->dc.r.dcfg & ~0x19, par->dc_regs + 0x08);
	
	/* Write the rest of the active registers */

	writel(regs->dc.r.fb_st_offset, par->dc_regs + 0x10);
	writel(regs->dc.r.cb_st_offset, par->dc_regs + 0x14);
	writel(regs->dc.r.curs_st_offset, par->dc_regs + 0x18);
	writel(regs->dc.r.icon_st_offset, par->dc_regs + 0x1C);
	writel(regs->dc.r.vid_y_st_offset, par->dc_regs + 0x20);
	writel(regs->dc.r.vid_u_st_offset, par->dc_regs + 0x24);
	writel(regs->dc.r.vid_v_st_offset, par->dc_regs + 0x28);
	writel(regs->dc.r.line_size, par->dc_regs + 0x30);
	writel(regs->dc.r.gfx_pitch, par->dc_regs + 0x34);
	writel(regs->dc.r.vid_yuv_pitch, par->dc_regs + 0x38);
	writel(regs->dc.r.h_active_timing, par->dc_regs + 0x40);
	writel(regs->dc.r.h_blank_timing, par->dc_regs + 0x44);
	writel(regs->dc.r.h_sync_timing, par->dc_regs + 0x48);
	writel(regs->dc.r.v_active_timing, par->dc_regs + 0x50);
	writel(regs->dc.r.v_blank_timing, par->dc_regs + 0x54);
	writel(regs->dc.r.v_sync_timing, par->dc_regs + 0x58);
	writel(regs->dc.r.dc_cursor_x, par->dc_regs + 0x60);
	writel(regs->dc.r.dc_cursor_y, par->dc_regs + 0x64);
	writel(regs->dc.r.dc_icon_x, par->dc_regs + 0x68);
	
	/* Don't write the line_cnt or diag registers */

	writel(regs->dc.r.dc_vid_ds_delta, par->dc_regs + 0x80);
	writel(regs->dc.r.gliu0_mem_offset, par->dc_regs + 0x84);
	writel(regs->dc.r.dv_acc, par->dc_regs + 0x8C);
	
	/* VP */

	/* MSR */
	wrmsrl(GX_VP_MSR_PAD_SELECT, regs->msr.padsel);

	writel(0, par->vid_regs + 0x38);
	
	for(i = 0; i < 0xFF; i++)
		writel((u32) regs->gamma[i], par->vid_regs + 0x40);   

	/* Don't enable video yet */
	writel((u32) regs->vp.r.vcfg & ~0x01, par->vid_regs + 0x00);

	/* Don't enable the CRT yet */
	writel((u32) regs->vp.r.dcfg & ~0x0F, par->vid_regs + 0x08);

	/* Write the rest of the VP registers */

	writel((u32) regs->vp.r.vx, par->vid_regs + 0x10);
	writel((u32) regs->vp.r.vy, par->vid_regs + 0x18);
	writel((u32) regs->vp.r.vs, par->vid_regs + 0x20);
	writel((u32) regs->vp.r.vck, par->vid_regs + 0x28);
	writel((u32) regs->vp.r.vcm, par->vid_regs + 0x30);
	writel((u32) regs->vp.r.misc, par->vid_regs + 0x50);
	writel((u32) regs->vp.r.ccs, par->vid_regs + 0x58);
	writel((u32) regs->vp.r.vdc, par->vid_regs + 0x78);
	writel((u32) regs->vp.r.vco, par->vid_regs + 0x80);
	writel((u32) regs->vp.r.crc, par->vid_regs + 0x88);
	writel((u32) regs->vp.r.vde, par->vid_regs + 0x98);
	writel((u32) regs->vp.r.cck, par->vid_regs + 0xA0);
	writel((u32) regs->vp.r.ccm, par->vid_regs + 0xA8);
	writel((u32) regs->vp.r.cc1, par->vid_regs + 0xB0);
	writel((u32) regs->vp.r.cc2, par->vid_regs + 0xB8);
	writel((u32) regs->vp.r.a1x, par->vid_regs + 0xC0);
	writel((u32) regs->vp.r.a1y, par->vid_regs + 0xC8);
	writel((u32) regs->vp.r.a1c, par->vid_regs + 0xD0);
	writel((u32) regs->vp.r.a1t, par->vid_regs + 0xD8);
	writel((u32) regs->vp.r.a2x, par->vid_regs + 0xE0);
	writel((u32) regs->vp.r.a2y, par->vid_regs + 0xE8);
	writel((u32) regs->vp.r.a2c, par->vid_regs + 0xF0);
	writel((u32) regs->vp.r.a2t, par->vid_regs + 0xF8);
	writel((u32) regs->vp.r.a3x, par->vid_regs + 0x100);
	writel((u32) regs->vp.r.a3y, par->vid_regs + 0x108);
	writel((u32) regs->vp.r.a3c, par->vid_regs + 0x110);
	writel((u32) regs->vp.r.a3t, par->vid_regs + 0x118);
	writel((u32) regs->vp.r.vrr, par->vid_regs + 0x120);
	
	
	/* FP registers */
	
	writel((u32) regs->fp.r.pt1, par->vid_regs + 0x400);
	writel((u32) regs->fp.r.pt2, par->vid_regs + 0x408);

	writel((u32) regs->fp.r.dfc, par->vid_regs + 0x418);
	writel(regs->fp.r.blfsr, par->vid_regs + 0x420);
	writel(regs->fp.r.rlfsr, par->vid_regs + 0x428);
	writel(regs->fp.r.fmi, par->vid_regs + 0x430);
	writel(regs->fp.r.fmd, par->vid_regs + 0x438);
	writel(regs->fp.r.dca, par->vid_regs + 0x448);
	writel(regs->fp.r.dmd, par->vid_regs + 0x450);
	writel(regs->fp.r.crc, par->vid_regs + 0x458);
	writel(regs->fp.r.fbb, par->vid_regs + 0x460);
	
	/* Final enables */

	val = readl(par->vid_regs + 0x410);

	/* Control the panel */
	if (regs->fp.r.pm & (1 << 24)) {

		if (!(val & 0x09))
			writel(regs->fp.r.pm, par->vid_regs + 0x410);
	}
	else {
		if (!(val & 0x05))
			writel(regs->fp.r.pm, par->vid_regs + 0x410);
	}
       	
	/* Turn everything on */

	writel(regs->dc.r.gcfg, par->dc_regs + 0x04);
	writel((u32) regs->vp.r.vcfg, par->vid_regs + 0x00);
	writel((u32) regs->vp.r.dcfg, par->vid_regs + 0x08);
	writel(regs->dc.r.dcfg, par->dc_regs + 0x08);
}


#ifdef DEBUG

void dump_regs(struct fb_info *info, int mode) {

	struct geodefb_par *par = info->par;
	u32 val;
	int i;

	if (mode == 0) {
		for(i = 0; i < GP_REG_SIZE; i += 4) {
			val = readl(par->gp_regs + i);
		}
	}

	if (mode == 1) {
		writel(0x4758, par->dc_regs + 0x00);

		for(i = 0; i < DC_REG_SIZE; i += 4) {
			val = readl(par->dc_regs + i);
			printk("DC%x: %x\n", i, val);
		}
	}

	if (mode == 2) {
		for(i = 0; i < VP_REG_SIZE; i += 8) {
			val = readl(par->vid_regs + i);
			printk("VP%x: %x\n", i, val);
		}
	}

	if (mode == 3) {
		for(i = 0; i < FP_REG_SIZE; i += 8) {
			val = readl(par->vid_regs + 0x400 + i);
			printk("FP%x: %x\n", i, val);
		}
	}
}

#endif
