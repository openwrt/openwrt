/******************************************************************************

                            Copyright (c) 2007
                         Infineon Technologies AG
               St. Martin Strasse 53; 81669 Munich, Germany

  Any use of this Software is subject to the conclusion of a respective
  License Agreement. Without such a License Agreement no rights to the
  Software are granted.

******************************************************************************/

#ifndef __BOOT_REG_H
#define __BOOT_REG_H

#define LTQ_BOOT_CPU_OFFSET		0x20

#define LTQ_BOOT_RVEC(cpu)		(volatile u32*)(LTQ_L2_SPRAM_BASE + \
	(cpu * LTQ_BOOT_CPU_OFFSET) + 0x00)
#define LTQ_BOOT_NVEC(cpu)		(volatile u32*)(LTQ_L2_SPRAM_BASE + \
	(cpu * LTQ_BOOT_CPU_OFFSET) + 0x04)
#define LTQ_BOOT_EVEC(cpu)		(volatile u32*)(LTQ_L2_SPRAM_BASE + \
	(cpu * LTQ_BOOT_CPU_OFFSET) + 0x08)
#define LTQ_BOOT_CP0_STATUS(cpu)	(volatile u32*)(LTQ_L2_SPRAM_BASE + \
	(cpu * LTQ_BOOT_CPU_OFFSET) + 0x0C)
#define LTQ_BOOT_CP0_EPC(cpu)		(volatile u32*)(LTQ_L2_SPRAM_BASE + \
	(cpu * LTQ_BOOT_CPU_OFFSET) + 0x10)
#define LTQ_BOOT_CP0_EEPC(cpu)		(volatile u32*)(LTQ_L2_SPRAM_BASE + \
	(cpu * LTQ_BOOT_CPU_OFFSET) + 0x14)
#define LTQ_BOOT_SIZE(cpu)		(volatile u32*)(LTQ_L2_SPRAM_BASE + \
	(cpu * LTQ_BOOT_CPU_OFFSET) + 0x18) /* only for CP1 */
#define LTQ_BOOT_RCU_SR(cpu)		(volatile u32*)(LTQ_L2_SPRAM_BASE + \
	(cpu * LTQ_BOOT_CPU_OFFSET) + 0x18) /* only for CP0 */
#define LTQ_BOOT_CFG_STAT(cpu)		(volatile u32*)(LTQ_L2_SPRAM_BASE + \
	(cpu * LTQ_BOOT_CPU_OFFSET) + 0x1C)

#endif
