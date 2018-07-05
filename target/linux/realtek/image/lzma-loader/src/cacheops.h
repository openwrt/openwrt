/*
 * Cache operations for the cache instruction.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#ifndef	__ASM_CACHEOPS_H
#define	__ASM_CACHEOPS_H

#define CP0_CCTL       $20
#define CCTL_DInval    0x00000001
#define CCTL_IInval    0x00000002
#define CCTL_DWB       0x00000100
#define CCTL_DWBInval  0x00000200

#ifndef __ASSEMBLY__

#define __write_32bit_c0_register(register, sel, value)			\
do {									\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"mtc0\t%z0, " #register "\n\t"			\
			: : "Jr" ((unsigned int)(value)));		\
	else								\
		__asm__ __volatile__(					\
			".set\tmips32\n\t"				\
			"mtc0\t%z0, " #register ", " #sel "\n\t"	\
			".set\tmips0"					\
			: : "Jr" ((unsigned int)(value)));		\
} while (0)

#define write_c0_cctl(val)	__write_32bit_c0_register($20, 0, val)

#endif	/* __ASSEMBLY__ */

#endif	/* __ASM_CACHEOPS_H */

