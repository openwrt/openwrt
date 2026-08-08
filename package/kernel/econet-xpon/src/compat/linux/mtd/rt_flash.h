/*
 * compat shim for <linux/mtd/rt_flash.h>  (Ralink/MTK flash + base types)
 * The generated *_mac_reg_c_header*.h register headers include this only for
 * the FIELD / UINTxx integer types and PACKING attribute.  We forward to the
 * tc3162 shim which already defines FIELD, and provide the rest here.
 */
#ifndef _COMPAT_LINUX_MTD_RT_FLASH_H_
#define _COMPAT_LINUX_MTD_RT_FLASH_H_

#include <linux/types.h>

/* FIELD is the bitfield base type used by the *_mac_reg_c_header*.h register
 * headers (which include this file).  It must be a typedef, NOT a #define:
 * #define'd, it expanded the driver's own `typedef unsigned int FIELD;`
 * (inc/common/drv_types.h, phy/inc/phy_types.h) into
 * `typedef unsigned int unsigned int;` -> "duplicate 'unsigned'".  Redundant
 * identical typedefs are legal in C11, so this coexists with those headers. */
typedef unsigned int FIELD;
#ifndef PACKING
#define PACKING
#endif

#endif /* _COMPAT_LINUX_MTD_RT_FLASH_H_ */
