/*==============================================================================*/
/* rbmipsnand.c                                                                 */
/* This module is derived from the 2.4 driver shipped by Microtik for their     */
/* Routerboard 1xx and 5xx series boards.  It provides support for the built in */
/* NAND flash on the Routerboard 1xx series boards for Linux 2.6.19+.           */
/* Licence: Original Microtik code seems not to have a licence.                 */
/*          Rewritten code all GPL V2.                                          */
/* Copyright(C) 2007 david.goodenough@linkchoose.co.uk (for rewriten code)      */
/*==============================================================================*/

#include <linux/init.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/bootinfo.h>
#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/adm5120_defs.h>

#define SMEM1(x) (*((volatile unsigned char *) (KSEG1ADDR(ADM5120_SRAM1_BASE) + x)))

#define NAND_RW_REG	0x0	//data register
#define NAND_SET_CEn	0x1	//CE# low
#define NAND_CLR_CEn	0x2	//CE# high
#define NAND_CLR_CLE	0x3	//CLE low
#define NAND_SET_CLE	0x4	//CLE high
#define NAND_CLR_ALE	0x5	//ALE low
#define NAND_SET_ALE	0x6	//ALE high
#define NAND_SET_SPn	0x7	//SP# low (use spare area)
#define NAND_CLR_SPn	0x8	//SP# high (do not use spare area)
#define NAND_SET_WPn	0x9	//WP# low
#define NAND_CLR_WPn	0xA	//WP# high
#define NAND_STS_REG	0xB	//Status register

#define MEM32(x) *((volatile unsigned *) (x))
static void __iomem *p_nand;

static int rb100_dev_ready(struct mtd_info *mtd) {
    return SMEM1(NAND_STS_REG) & 0x80;
}

static void rbmips_hwcontrol100(struct mtd_info *mtd, int cmd, unsigned int ctrl) {
    struct nand_chip *chip = mtd->priv;
    if (ctrl & NAND_CTRL_CHANGE) {
        SMEM1((( ctrl & NAND_CLE) ? NAND_SET_CLE : NAND_CLR_CLE)) = 0x01;
        SMEM1((( ctrl & NAND_ALE) ? NAND_SET_ALE : NAND_CLR_ALE)) = 0x01;
        SMEM1((( ctrl & NAND_NCE) ? NAND_SET_CEn : NAND_CLR_CEn)) = 0x01;
        }
    if( cmd != NAND_CMD_NONE)
        writeb( cmd, chip->IO_ADDR_W);
}

static struct mtd_partition partition_info[] = {
    {
        name: "RouterBoard NAND Boot",
        offset: 0,
	size: 4 * 1024 * 1024
    },
    {
        name: "rootfs",
	offset: MTDPART_OFS_NXTBLK,
	size: MTDPART_SIZ_FULL
    }
};

static struct mtd_info rmtd;
static struct nand_chip rnand;
/*========================================================================*/
/* We need to use the OLD Yaffs-1 OOB layout, otherwise the RB bootloader */
/* will not be able to find the kernel that we load.  So set the oobinfo  */
/* when creating the partitions.                                          */ 
/*========================================================================*/
static struct nand_ecclayout rb_ecclayout = {
        .eccbytes = 6,
        .eccpos = { 8, 9, 10, 13, 14, 15 },
	.oobavail = 9,
        .oobfree = { { 0, 4 }, { 6, 2 }, { 11, 2 }, { 4, 1} }
};
static unsigned init_ok = 0;

unsigned get_rbnand_block_size(void) {
	return init_ok ? rmtd.writesize : 0;
}

EXPORT_SYMBOL(get_rbnand_block_size);

int __init rbmips_init(void) {

	if (!adm5120_nand_boot)
		return -ENODEV;

	memset(&rmtd, 0, sizeof(rmtd));
	memset(&rnand, 0, sizeof(rnand));
	printk(KERN_INFO "RB1xx nand\n");
	MEM32(0xB2000064) = 0x100;
	MEM32(0xB2000008) = 0x1;
	SMEM1(NAND_SET_SPn) = 0x01;
	SMEM1(NAND_CLR_WPn) = 0x01;
	rnand.IO_ADDR_R = (unsigned char *)KSEG1ADDR(ADM5120_SRAM1_BASE);
	rnand.IO_ADDR_W = rnand.IO_ADDR_R;
	rnand.cmd_ctrl = rbmips_hwcontrol100;
	rnand.dev_ready = rb100_dev_ready;
	p_nand = (void __iomem *)ioremap(( unsigned long)ADM5120_SRAM1_BASE, 0x1000);
	if (!p_nand) {
		printk(KERN_WARNING "RB1xx nand Unable ioremap buffer\n");
		return -ENXIO;
	}
	rnand.ecc.mode = NAND_ECC_SOFT;
	rnand.ecc.layout = &rb_ecclayout;
	rnand.chip_delay = 25;
	rnand.options |= NAND_NO_AUTOINCR;
	rmtd.priv = &rnand;
	if (nand_scan(&rmtd, 1) && nand_scan(&rmtd, 1)
	    && nand_scan(&rmtd, 1)  && nand_scan(&rmtd, 1)) {
		printk(KERN_INFO "RB1xxx nand device not found\n");
		iounmap ((void *)p_nand);
		return -ENXIO;
	}
	add_mtd_partitions(&rmtd, partition_info, 2);
	init_ok = 1;
	return 0;
}

module_init(rbmips_init);

