/*
 *  Lexra specific register definitions
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */
#ifndef _ASM_LXREGS_H
#define _ASM_LXREGS_H

#include <asm/mipsregs.h>

/*
 * Lexra specific CP0 register names
 */
#define LXCP0_ESTATUS		$0
#define LXCP0_ECAUSE		$1
#define LXCP0_INTVEC		$2

#define CP0_CCTL		$20
#define CCTL_DInval		0x00000001
#define CCTL_IInval		0x00000002
#define CCTL_ILock		0x000000c0
#define CCTL_IMEMFILL		0x00000010
#define CCTL_IMEMOFF		0x00000020
#define CCTL_IROM0ON		0x00000040
#define CCTL_IROM0OFF		0x00000080
#define CCTL_DWB		0x00000100
#define CCTL_DWBInval		0x00000200
#define CCTL_DMEMON		0x00000400
#define CCTL_DMEMOFF		0x00000800

/*
 * Lexra specific CP3 register names
 */
#define CP3_IMEMBASE		$0
#define CP3_IMEMTOP		$1
#define CP3_IMEM1BASE		$2
#define CP3_IMEM1TOP		$3
#define CP3_DMEMBASE		$4
#define CP3_DMEMTOP		$5
#define CP3_DMEM1BASE		$6
#define CP3_DMEM1TOP		$7


/*
 * Lexra specific status register bits
 */
#define EST0_IM			0x00ff0000
#define ESTATUSF_IP0		(_ULCAST_(1) << 16)
#define ESTATUSF_IP1		(_ULCAST_(1) << 17)
#define ESTATUSF_IP2		(_ULCAST_(1) << 18)
#define ESTATUSF_IP3		(_ULCAST_(1) << 19)
#define ESTATUSF_IP4		(_ULCAST_(1) << 20)
#define ESTATUSF_IP5		(_ULCAST_(1) << 21)
#define ESTATUSF_IP6		(_ULCAST_(1) << 22)
#define ESTATUSF_IP7		(_ULCAST_(1) << 23)

#define ECAUSEF_IP		(_ULCAST_(255) << 16)
#define ECAUSEF_IP0		(_ULCAST_(1)   << 16)
#define ECAUSEF_IP1		(_ULCAST_(1)   << 17)
#define ECAUSEF_IP2		(_ULCAST_(1)   << 18)
#define ECAUSEF_IP3		(_ULCAST_(1)   << 19)
#define ECAUSEF_IP4		(_ULCAST_(1)   << 20)
#define ECAUSEF_IP5		(_ULCAST_(1)   << 21)
#define ECAUSEF_IP6		(_ULCAST_(1)   << 22)
#define ECAUSEF_IP7		(_ULCAST_(1)   << 23)


#ifndef __ASSEMBLY__

/*
 * Macros to access the system control coprocessor
 */
#define __read_32bit_lxc0_register(source)			\
({ int __res;							\
	__asm__ __volatile__(					\
		"mflxc0\t%0, " #source "\n\t"			\
		: "=r" (__res));				\
	__res;							\
})

#define __write_32bit_lxc0_register(register, value)		\
do {								\
	__asm__ __volatile__(					\
		"mtlxc0\t%z0, " #register "\n\t"		\
		: : "Jr" ((unsigned int)(value)));		\
} while (0)

#define __read_32bit_c3_register(source)			\
({ int __res;							\
	__asm__ __volatile__(					\
		".set push\n"					\
		".set mips1\n"					\
		"mfc3\t%0, " #source "\n\t"			\
		".set pop\n"					\
		: "=r" (__res));				\
	__res;							\
})

#define __write_32bit_c3_register(register, value)		\
do {								\
	__asm__ __volatile__(					\
		".set push\n"					\
		".set mips1\n"					\
		"mtc3\t%z0, " #register "\n\t"			\
		".set pop\n"					\
		: : "Jr" ((unsigned int)(value)));		\
} while (0)


#define read_c0_cctl()		__read_32bit_c0_register($20, 0)
#define write_c0_cctl(val)	__write_32bit_c0_register($20, 0, val)

#define read_c0_cctl1()		__read_32bit_c0_register($20, 1)
#define write_c0_cctl1(val)	__write_32bit_c0_register($20, 1, val)

#define read_c3_imembase()	__read_32bit_c3_register($0)
#define write_c3_imembase(val)	__write_32bit_c3_register($0, val)

#define read_c3_imemtop()	__read_32bit_c3_register($1)
#define write_c3_imemtop(val)	__write_32bit_c3_register($1, val)

#define read_c3_imem1base()	__read_32bit_c3_register($2)
#define write_c3_imem1base(val)	__write_32bit_c3_register($2, val)

#define read_c3_imem1top()	__read_32bit_c3_register($3)
#define write_c3_imem1top(val)	__write_32bit_c3_register($3, val)

#define read_c3_dmembase()	__read_32bit_c3_register($4)
#define write_c3_dmembase(val)	__write_32bit_c3_register($4, val)

#define read_c3_dmemtop()	__read_32bit_c3_register($5)
#define write_c3_dmemtop(val)	__write_32bit_c3_register($5, val)

#define read_c3_dmem1base()	__read_32bit_c3_register($6)
#define write_c3_dmem1base(val)	__write_32bit_c3_register($6, val)

#define read_c3_dmem1top()	__read_32bit_c3_register($7)
#define write_c3_dmem1top(val)	__write_32bit_c3_register($7, val)


#define read_lxc0_estatus()	__read_32bit_lxc0_register($0)
#define read_lxc0_ecause()	__read_32bit_lxc0_register($1)
#define read_lxc0_intvec()	__read_32bit_lxc0_register($2)
#define write_lxc0_estatus(val)	__write_32bit_lxc0_register($0, val)
#define write_lxc0_ecause(val)	__write_32bit_lxc0_register($1, val)
#define write_lxc0_intvec(val)	__write_32bit_lxc0_register($2, val)


/*
 * Manipulate bits in a lxc0 register.
 */
#define __BUILD_SET_LXC0(name)					\
static inline unsigned int					\
set_lxc0_##name(unsigned int set)				\
{								\
	unsigned int res, new;					\
								\
	res = read_lxc0_##name();				\
	new |= set;						\
	write_lxc0_##name(new);					\
								\
	return res;						\
}								\
								\
static inline unsigned int					\
clear_lxc0_##name(unsigned int clear)				\
{								\
	unsigned int res, new;					\
								\
	res = read_lxc0_##name();				\
	new = res & ~clear;					\
	write_lxc0_##name(new);					\
								\
	return res;						\
}								\
								\
static inline unsigned int					\
change_lxc0_##name(unsigned int change, unsigned int val)	\
{								\
	unsigned int res, new;					\
								\
	res = read_lxc0_##name();				\
	new = res & ~change;					\
	new |= (val & change);					\
	write_lxc0_##name(new);					\
								\
	return res;						\
}

__BUILD_SET_LXC0(estatus)
__BUILD_SET_LXC0(ecause)
__BUILD_SET_LXC0(intvec)

#endif /* !__ASSEMBLY__ */

#endif /* _ASM_LXREGS_H */
