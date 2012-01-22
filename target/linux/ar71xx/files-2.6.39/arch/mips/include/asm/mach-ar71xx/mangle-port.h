/*
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This file was derived from: inlude/asm-mips/mach-generic/mangle-port.h
 *      Copyright (C) 2003, 2004 Ralf Baechle
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __ASM_MACH_AR71XX_MANGLE_PORT_H
#define __ASM_MACH_AR71XX_MANGLE_PORT_H

#define __swizzle_addr_b(port)	((port) ^ 3)
#define __swizzle_addr_w(port)	((port) ^ 2)
#define __swizzle_addr_l(port)	(port)
#define __swizzle_addr_q(port)	(port)

#if defined(CONFIG_SWAP_IO_SPACE)

# define ioswabb(a, x)           (x)
# define __mem_ioswabb(a, x)     (x)
# define ioswabw(a, x)           le16_to_cpu(x)
# define __mem_ioswabw(a, x)     (x)
# define ioswabl(a, x)           le32_to_cpu(x)
# define __mem_ioswabl(a, x)     (x)
# define ioswabq(a, x)           le64_to_cpu(x)
# define __mem_ioswabq(a, x)     (x)

#else

# define ioswabb(a, x)           (x)
# define __mem_ioswabb(a, x)     (x)
# define ioswabw(a, x)           (x)
# define __mem_ioswabw(a, x)     cpu_to_le16(x)
# define ioswabl(a, x)           (x)
# define __mem_ioswabl(a, x)     cpu_to_le32(x)
# define ioswabq(a, x)           (x)
# define __mem_ioswabq(a, x)     cpu_to_le64(x)

#endif

#endif /* __ASM_MACH_AR71XX_MANGLE_PORT_H */
