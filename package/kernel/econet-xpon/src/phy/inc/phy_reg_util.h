#ifndef _DRV_REG_UTIL_H_
#define _DRV_REG_UTIL_H_

#include <asm/io.h>
#include "i2c.h"
#include <asm/tc3162/tc3162.h>

/* PORT: A60928 accesses PHY regs over I2C via phy_I2C_*_translet (defined in phy.c).
 * Declare them so IO_GPHYREG/IO_SPHYREG users compile under GCC 14 (implicit function
 * declaration is now a hard error). */
unsigned int phy_I2C_read_translet(unsigned int sub_addr);
void phy_I2C_write_translet(unsigned int sub_addr, unsigned int val);

#ifdef TCSUPPORT_CPU_EN7521
#ifdef TCSUPPORT_AUTOBENCH
#define I2C_U2_CLK_DIV	(0xc7)
#else
#define I2C_U2_CLK_DIV	(0x60)
#endif
#else
#define I2C_U2_CLK_DIV	(0xc7)
#endif
#ifdef CONFIG_USE_MT7520_ASIC
#define IO_GPHYREG(reg)						regReadPhy32(reg)//ioread32((void __iomem *)(reg)) /* GetReg((uint)reg) */
#define IO_SPHYREG(reg, val)				iowrite32(val, (void __iomem *)(reg)) /* SetReg((uint)reg, val) */
#elif defined(CONFIG_USE_A60901) || defined(CONFIG_USE_A60928)
	#ifdef CONFIG_USE_A60901
	#define	U1_DEV_ADDR  	(0x60)
	#elif CONFIG_USE_A60928
	/* PORT: sif_i2c_scan proved the ONLY device on this EN7528's SIF bus is at
	 * 0x70 (0x50 = generic A60928 default returns 0xff/no-ACK here). The integrated
	 * GPON PHY answers at 0x70 (raw read there = real data). */
	#define	U1_DEV_ADDR	 	(0x70)
	#endif
	/* PORT FIX (2026-07-02): on THIS EN7528, the integrated digital GPON PHY is
	 * MEMORY-MAPPED at 0xbfaf0000 + sub_addr, NOT an external A60928 on the SIF/I2C
	 * bus. Proven: stock phy.ko phy_ready_status/phy_los_status read 0xbfaf0130 /
	 * 0xbfaf05e0 via raw `lw` (disasm), and on-device the MMIO file holds the live
	 * state (GPON-mode bit, valid ANACAL/TDCSTA cals) while the SIF READ path at
	 * dev 0x70 returns garbage/zeros. The I2C-translet approach mis-routed every
	 * digital-PHY read (mode/sync/los/cal all read 0 -> "analog cal railed", mode=EPON,
	 * is_sync flicker). Route digital-PHY CSR access to MMIO to match the stock.
	 * (The en7571 OPTICAL transceiver at SIF dev 0x70 is a separate device and is
	 * accessed by the en7571_* helpers via SIF_X_Read/Write directly, not IO_GPHYREG.) */
	/* READ via MMIO (the SIF/I2C read path returns garbage on this integrated PHY).
	 * WRITE keeps the SIF/I2C-translet path: its inherent ~ms/reg pacing sequences the
	 * PLL/reset writes -- back-to-back MMIO writes violate that timing and
	 * crash/watchdog-reboot the SoC during bring-up.
	 * ★ CORRECTION (2026-07-04, session 8 — the earlier "coherent, same physical register"
	 * claim here was WRONG): IO_SPHYREG -> phy_I2C_write_translet -> SIF_X_Write to I2C
	 * DEVICE 0x70 (phy.c:173). That is a DIFFERENT register file from the MMIO PHY at
	 * 0xbfaf0000 that IO_GPHYREG reads. Proven live: writing ANATXREG1[19:16] via IO_SPHYREG
	 * left the MMIO readback (0x00900006) UNCHANGED every time. So read (MMIO) and write
	 * (I2C 0x70) are NOT coherent. The real GPON datapath is driven by raw IO_SREG/IO_GREG
	 * MMIO pokes (gpon_init.c, en7571_* helpers) which DO land -- that is why CDR still locks
	 * despite phy_dev_init's IO_SPHYREG writes going to 0x70. To change any MMIO PHY analog
	 * field (e.g. TX drive swing ANATXREG1[19:16]=0xbfaf01a0) you MUST use IO_SREG, not
	 * IO_SPHYREG. See gpon_do_lock() in phy_init.c. */
	#define PHY_MMIO_BASE						(0xbfaf0000u)
	#define IO_GPHYREG(sub_addr)				ioread32((void __iomem *)(PHY_MMIO_BASE + (unsigned int)(sub_addr)))
	/* ★★★ FIX (2026-07-04, session 8 part 2): restore IO_SPHYREG to a plain MMIO WRITE —
	 * exactly what stock SetReg did. The I2C-dev-0x70 redirect (phy_I2C_write_translet) was
	 * the real bug: it silently DROPPED every digital-PHY write — analog init AND the whole
	 * TX serializer/FEC/BIST config that forms the upstream burst. PROVEN by readback ('q'
	 * MMIO-BIST diag: misrouted=1, LOOPBACK_SEL stayed 0 after an IO_SPHYREG write). Stock
	 * drove these as MMIO with no special pacing; the "back-to-back MMIO → watchdog" note
	 * above was a misattribution (all 'q'/'Z' MMIO writes ran fine). If bring-up watchdogs,
	 * re-add pacing (udelay) here, NOT the I2C path. Read (IO_GPHYREG) and write now coherent. */
	#define IO_SPHYREG(sub_addr, val)			iowrite32((val), (void __iomem *)(PHY_MMIO_BASE + (unsigned int)(sub_addr)))
#else
#define IO_GPHYREG(reg)						ioReadPhyReg32((uint)(reg))
#define IO_SPHYREG(reg, val)				ioWritePhyReg32((uint)(reg), val)
#endif /* CONFIG_USE_MT7520_ASIC */

/* PORT: with BOTH MT7520_ASIC+A60928 defined (vendor build), the MT7520 branch wins for
 * IO_GPHYREG (MMIO) but U1_DEV_ADDR (used by phy_I2C_*_translet for the SIF/I2C transceiver)
 * is left undefined. Provide it. */
#ifndef U1_DEV_ADDR
#define U1_DEV_ADDR	(0x70)
#endif

#define INREG32(ptr)						(ptr)
#define REG_FLD(bits, shift)				(0xFFFFFFFF>>(32-bits)), shift
#define REG_FLD_GET(FLD, reg)				GetRegMask((uint)(reg), FLD)
#define REG_FLD_SET(FLD, reg, val)			SetRegMask((uint)(reg), FLD, val)

#define IO_GREG(reg)						ioread32((void __iomem *)(reg)) /* GetReg((uint)reg) */
#define IO_SREG(reg, val)					iowrite32(val, (void __iomem *)(reg)) /* SetReg((uint)reg, val) */
#define IO_SBITS(reg, bits)					SetBits((uint)(reg), bits)
#define IO_CBITS(reg, bits)					ClearBits((uint)(reg), bits)
#define IO_GMASK(reg, mask, shift)			GetRegMask((uint)(reg), mask, shift)
#define IO_SMASK(reg, mask, shift, val)		SetRegMask((uint)(reg), mask, shift, val)

/************************************************************************
 Description:	get register value
 Input:			arg1: device register
 Output:		return the value of register
************************************************************************/
static uint __inline__ GetReg(uint reg)
{
	return ioread32((void __iomem *)reg) ;
}

/************************************************************************
 Description:	set register
 Input:			arg1: device register
 				arg2: setting value
  Output:		no return
************************************************************************/
static void __inline__ SetReg(uint reg, uint value)
{
	iowrite32(value, (void __iomem *)reg) ;
}


/************************************************************************
 Description:	get mask value of register
 Input:			arg1: device register
 				arg2: mask bits to get
 				arg3: offset of the mask
 Output:		return the mask value of register
************************************************************************/
static uint __inline__ GetRegMask(uint reg, uint mask, uint shift)
{
	return (((ioread32((void __iomem *)reg))>>shift) & mask) ;
}

/************************************************************************
 Description:	set mask value of register
 Input:			arg1: device register
 				arg2: mask bits to get
 				arg3: offset of the mask
 				arg4: setting value
 Output:		no return
************************************************************************/
static void __inline__ SetRegMask(uint reg, uint mask, uint shift, uint value)
{
    uint data = ioread32((void __iomem *)reg) ;
    
    data = (data & ~(mask<<shift)) | ((value&mask)<<shift) ;
    iowrite32(data, (void __iomem *)reg) ;
}

/************************************************************************
 Description:	set multiple bits to 1
 Input:			arg1: device register
 				arg2: multiple bits
 Output:		no return
************************************************************************/
static void __inline__ SetBits(uint reg, uint bits)
{
    uint data = ioread32((void __iomem *)reg) ;
    
    data |= bits ;
	iowrite32(data, (void __iomem *)reg) ;
}

/************************************************************************
 Description:	set multiple bits to 0
 Input:			arg1: device register
 				arg2: multiple bits
 Output:		no return
************************************************************************/
static void __inline__ ClearBits(uint reg, uint bits)
{
    uint data =ioread32((void __iomem *)reg) ;
    
    data &= ~bits ;
	iowrite32(data, (void __iomem *)reg) ;
}


#endif /* _DRV_REG_UTIL_H_ */

