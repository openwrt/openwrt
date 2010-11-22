/*
 * linux/include/asm-m68k/cf_io.h
 *
 * Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 * 9/30/08 JKM - Separated Coldfire pieces out from m68k.
 */

#ifndef __CF_IO__
#define __CF_IO__

#ifdef __KERNEL__

#include <linux/compiler.h>
#include <asm/raw_io.h>
#include <asm/virtconvert.h>

#include <asm-generic/iomap.h>

/*
 * These should be valid on any ioremap()ed region
 */
#define readb(addr)      in_8(addr)
#define writeb(val,addr) out_8((addr),(val))
#define readw(addr)      in_le16(addr)
#define writew(val,addr) out_le16((addr),(val))
#define readl(addr)      in_le32(addr)
#define writel(val,addr) out_le32((addr),(val))

#define readb_relaxed(addr) readb(addr)
#define readw_relaxed(addr) readw(addr)
#define readl_relaxed(addr) readl(addr)

#ifdef CONFIG_PCI

/*
 * IO space in Coldfire
 */
/*#define HAVE_ARCH_PIO_SIZE	1
#define PIO_OFFSET		0x00000000UL
#define PIO_RESERVED		0x100000000UL
#define PIO_MASK		(PIO_RESERVED - 1)
*/
#define inb_p   inb
#define inw_p   inw
#define inl_p   inl
#define outb_p  outb
#define outw_p  outw
#define outl_p  outl

#ifndef CONFIG_COLDFIRE
#define inb(port)      in_8(port)
#define outb(val,port) out_8((port),(val))
#define inw(port)      in_le16(port)
#define outw(val,port) out_le16((port),(val))
#define inl(port)      in_le32(port)
#define outl(val,port) out_le32((port),(val))
#define insb(port, buf, nr)	\
		raw_insb((u8 *)(port), (u8 *)(buf), (nr))
#define outsb(port, buf, nr)	\
		raw_outsb((u8 *)(port), (u8 *)(buf), (nr))
#define insw(port, buf, nr)	\
		raw_insw_swapw((u16 *)(port), (u16 *)(buf), (nr))
#define outsw(port, buf, nr)	\
		raw_outsw_swapw((u16 *)(port), (u16 *)(buf), (nr))
#define insl(port, buf, nr)	\
		raw_insw_swapw((u16 *)(port), (u16 *)(buf), (nr)<<1)
#define outsl(port, buf, nr)	\
		raw_outsw_swapw((u16 *)(port), (u16 *)(buf), (nr)<<1)
#else
#define inb(port)      pci_inb(port)
#define outb(val, port) pci_outb((val), (port))
#define inw(port)      pci_inw(port)
#define outw(val, port) pci_outw((val), (port))
#define insb(a, b, c)  \
	pci_insb((volatile unsigned char *)a, (unsigned char *)b, c)
#define insw(a, b, c)  \
	pci_insw((volatile unsigned short *)a, (const unsigned short *)b, c)
#define insl(a, b, c)  \
	pci_insl((volatile unsigned long *)a, (const unsigned long *)b, c)
#define outsb(a, b, c) \
	pci_outsb((volatile unsigned char *)a, (const unsigned char *)b, c)
#define outsw(a, b, c) \
	pci_outsw((volatile unsigned short *)a, (const unsigned short *)b, c)
#define outsl(a, b, c) \
	pci_outsl((volatile unsigned long *)a, (const unsigned long *)b, c)
#define inl(port)        pci_inl(port)
#define outl(val, port)   pci_outl((val), (port))
#endif

#else
/* no pci */

#define inb(port)      in_8(port)
#define outb(val, port) out_8((port), (val))
#define inw(port)      in_le16(port)
#define outw(val, port) out_le16((port), (val))
#define inl(port)      in_le32(port)
#define outl(val, port) out_le32((port), (val))
#define insb(port, buf, nr)	\
		raw_insb((u8 *)(port), (u8 *)(buf), (nr))
#define outsb(port, buf, nr)	\
		raw_outsb((u8 *)(port), (u8 *)(buf), (nr))
#define insw(port, buf, nr)	\
		raw_insw_swapw((u16 *)(port), (u16 *)(buf), (nr))
#define outsw(port, buf, nr)	\
		raw_outsw_swapw((u16 *)(port), (u16 *)(buf), (nr))
#define insl(port, buf, nr)	\
		raw_insw_swapw((u16 *)(port), (u16 *)(buf), (nr)<<1)
#define outsl(port, buf, nr)	\
		raw_outsw_swapw((u16 *)(port), (u16 *)(buf), (nr)<<1)

#endif /* CONFIG_PCI */

#define mmiowb()

static inline void __iomem *ioremap(unsigned long physaddr, unsigned long size)
{
	return __ioremap(physaddr, size, IOMAP_NOCACHE_SER);
}
static inline void __iomem *ioremap_nocache(unsigned long physaddr, unsigned long size)
{
	return __ioremap(physaddr, size, IOMAP_NOCACHE_SER);
}
static inline void __iomem *ioremap_writethrough(unsigned long physaddr,
					 unsigned long size)
{
	return __ioremap(physaddr, size, IOMAP_WRITETHROUGH);
}
static inline void __iomem *ioremap_fullcache(unsigned long physaddr,
				      unsigned long size)
{
	return __ioremap(physaddr, size, IOMAP_FULL_CACHING);
}

static inline void memset_io(volatile void __iomem *addr, unsigned char val, int count)
{
	__builtin_memset((void __force *) addr, val, count);
}
static inline void memcpy_fromio(void *dst, const volatile void __iomem *src, int count)
{
	__builtin_memcpy(dst, (void __force *) src, count);
}
static inline void memcpy_toio(volatile void __iomem *dst, const void *src, int count)
{
	__builtin_memcpy((void __force *) dst, src, count);
}

#define IO_SPACE_LIMIT 0xffffffff

#endif /* __KERNEL__ */

#define __ARCH_HAS_NO_PAGE_ZERO_MAPPED		1

/*
 * Convert a physical pointer to a virtual kernel pointer for /dev/mem
 * access
 */
#define xlate_dev_mem_ptr(p)	__va(p)

/*
 * Convert a virtual cached pointer to an uncached pointer
 */
#define xlate_dev_kmem_ptr(p)	p

#define __raw_readb(addr) \
    ({ unsigned char __v = (*(volatile unsigned char *) (addr)); __v; })
#define __raw_readw(addr) \
    ({ unsigned short __v = (*(volatile unsigned short *) (addr)); __v; })
#define __raw_readl(addr) \
    ({ unsigned long __v = (*(volatile unsigned long *) (addr)); __v; })
#define __raw_writeb(b,addr) (void)((*(volatile unsigned char *) (addr)) = (b))
#define __raw_writew(b,addr) (void)((*(volatile unsigned short *) (addr)) = (b))
#define __raw_writel(b,addr) (void)((*(volatile unsigned int *) (addr)) = (b))

#define memset_io(a, b, c) memset((void *)(a), (b), (c))
#define memcpy_fromio(a, b, c) memcpy((a), (void *)(b), (c))
#define memcpy_toio(a, b, c) memcpy((void *)(a), (b), (c))

#if !defined(readb)
#define readb(addr) \
    ({ unsigned char __v = (*(volatile unsigned char *) (addr)); __v; })
#define readw(addr) \
    ({ unsigned short __v = (*(volatile unsigned short *) (addr)); __v; })
#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })
#define writeb(b, addr) (void)((*(volatile unsigned char *) (addr)) = (b))
#define writew(b, addr) (void)((*(volatile unsigned short *) (addr)) = (b))
#define writel(b, addr) (void)((*(volatile unsigned int *) (addr)) = (b))
#endif /* readb */

#endif /* _IO_H */
