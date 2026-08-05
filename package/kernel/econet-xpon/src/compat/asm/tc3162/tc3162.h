/*
 * compat shim for <asm/tc3162/tc3162.h>  (TrendChip / EcoNet SoC header)
 *
 * The vendor driver was built against the tclinux_phoenix SDK which ships this
 * header providing register-access macros + chip-detect macros.  Mainline
 * OpenWrt has no such header, so we provide the minimal surface the GPON
 * driver references.  Register CONSTANTS stay in the driver's own headers
 * (hardware facts); here we only provide the access *primitives*.
 *
 * NOTE (compile milestone): MMIO is done against KSEG1 (0xbf......) physical
 * addresses exactly as the vendor code passes them.  On MIPS that is the
 * uncached window, so ioread32/iowrite32 on the raw address is correct; we do
 * NOT ioremap here.  TODO EN7528: route through ioremap'd bases
 */
#ifndef _COMPAT_ASM_TC3162_TC3162_H_
#define _COMPAT_ASM_TC3162_TC3162_H_

#include <linux/types.h>
#include <linux/io.h>

/*
 * NOTE: FIELD (the bitfield base type used by the *_reg headers) is a real
 * `typedef unsigned int FIELD;` in the driver's own inc/common/drv_types.h and
 * phy/inc/phy_types.h.  It must NOT be #define'd here -- doing so expanded the
 * driver's typedef into `typedef unsigned int unsigned int;` (duplicate
 * 'unsigned' / two-data-types errors).
 */

#ifndef VPint
#define VPint		*(volatile unsigned int *)
#define VPshort		*(volatile unsigned short *)
#define VPchar		*(volatile unsigned char *)
#endif

/* ---- raw register access ---- */
#ifndef regRead32
#define regRead32(addr)		ioread32((void __iomem *)(unsigned long)(addr))
#endif
#ifndef regWrite32
#define regWrite32(addr, val)	iowrite32((val), (void __iomem *)(unsigned long)(addr))
#endif
#ifndef sysRegRead
#define sysRegRead(addr)	regRead32(addr)
#endif
#ifndef sysRegWrite
#define sysRegWrite(addr, val)	regWrite32(addr, val)
#endif

/*
 * NOTE: GetRegMask/SetRegMask/SetBits/ClearBits are defined by the driver's
 * own drv_types.h and phy/inc/phy_reg_util.h, so they must NOT be (re)defined
 * here -- doing so caused "redefinition" errors.
 */

/* ---- internal-PHY register access (CONFIG_USE_MT7520_ASIC path) ---- */
static inline unsigned int regReadPhy32(unsigned int reg)
{
	return regRead32(reg);
}
static inline void regWritePhy32(unsigned int reg, unsigned int val)
{
	regWrite32(reg, val);
}
/*
 * NOTE: ioReadPhyReg32()/ioWritePhyReg32() are the REAL optical-PHY I2C/MDIO
 * accessors defined in phy/src/i2c.c (declared in phy/inc/i2c.h) with
 * signatures uint(ushort) / int(ushort,uint).  They must NOT be shimmed here
 * -- doing so conflicted with the driver's own definitions.
 */

/* ---- chip-detect macros (xpondrv chip table) ----
 * EN7521 build: the MT7520/MT7525 ASIC variants are not this SoC, so the
 * detect predicates are compile-time false.  TODO EN7528
 */
#ifndef isMT7520
#define isMT7520	(0)
#define isMT7520S	(0)
#define isMT7520G	(0)
#define isMT7520E2E3	(0)
#define isMT7525	(0)
#define isMT7525G	(0)
#endif
#ifndef isEN751221
#define isEN751221	(1)		/* EN7521 family */
#endif

/* FPGA-vs-silicon predicate: this is production silicon, not an FPGA. */
#ifndef isFPGA
#define isFPGA		(0)
#endif

/* Other EN752x/EN7521 SKU detect predicates referenced by the xpondrv chip
 * table + a couple of EPON debug paths.  This is the EN7521/EN7528 build, so
 * the *other* SKUs are compile-time false.  TODO EN7528: wire real chip-id. */
#ifndef isEN7521S
#define isEN7521S	(0)
#define isEN7521F	(0)
#define isEN7526F	(0)
#define isEN7526D	(0)
#define isEN7526G	(0)
#define isEN7526c	(0)
#endif

/*
 * NOTE: the MT7520/MT7525 chip-id enum constants are defined by the driver's
 * own inc/common/drv_global.h, so they are NOT defined here (would clash).
 */

#endif /* _COMPAT_ASM_TC3162_TC3162_H_ */
