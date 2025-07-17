/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *    *** IMPORTANT ***
 * This file is not only included from C-code but also from devicetree source
 * files. As such this file MUST only contain comments and defines.
 *
 * Device Tree constants identical to those in include/linux/mtd/partitions.h
 */

#ifndef _DT_BINDINGS_MTD_PARTITIONS_CONSTANTS_H
#define _DT_BINDINGS_MTD_PARTITIONS_CONSTANTS_H

#define MTDPART_OFS_SPECIAL	(-1)
#define MTDPART_OFS_RETAIN	(-3)
#define MTDPART_OFS_NXTBLK	(-2)
#define MTDPART_OFS_APPEND	(-1)
#define MTDPART_SIZ_FULL	(0)

#endif
