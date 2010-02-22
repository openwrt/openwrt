/*
 * arch/ubicom32/include/asm/io.h
 *   I/O memory accessor functions for Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#ifndef _ASM_UBICOM32_IO_H
#define _ASM_UBICOM32_IO_H

#ifdef __KERNEL__
#include <linux/string.h>
#include <linux/compiler.h>

static inline unsigned short _swapw(volatile unsigned short v)
{
    return ((v << 8) | (v >> 8));
}

static inline unsigned int _swapl(volatile unsigned long v)
{
    return ((v << 24) | ((v & 0xff00) << 8) | ((v & 0xff0000) >> 8) | (v >> 24));
}

#ifndef CONFIG_PCI
#define readb(addr) \
    ({ unsigned char __v = (*(volatile unsigned char *) (addr)); __v; })
#define readw(addr) \
    ({ unsigned short __v = (*(volatile unsigned short *) (addr)); __v; })
#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define writeb(b,addr) (void)((*(volatile unsigned char *) (addr)) = (b))
#define writew(b,addr) (void)((*(volatile unsigned short *) (addr)) = (b))
#define writel(b,addr) (void)((*(volatile unsigned int *) (addr)) = (b))
#else /*CONFIG_PCI */

#define PCI_CPU_REG_BASE (0x00000000UL)   /* taking lower 2GB space */
#define PCI_DEV_REG_BASE (0x80000000UL)

#if PCI_CPU_REG_BASE > PCI_DEV_REG_BASE
#define IS_PCI_ADDRESS(x) (((unsigned int)(x)&(PCI_CPU_REG_BASE)) == 0)
#else
#define IS_PCI_ADDRESS(x) ((unsigned int)(x)&(PCI_DEV_REG_BASE))
#endif

extern unsigned int ubi32_pci_read_u32(const volatile void __iomem *addr);
extern unsigned short ubi32_pci_read_u16(const volatile void __iomem *addr);
extern unsigned char ubi32_pci_read_u8(const volatile void __iomem *addr);
extern  void ubi32_pci_write_u32(unsigned int val, const volatile void __iomem *addr);
extern  void ubi32_pci_write_u16(unsigned short val, const volatile void __iomem *addr);
extern  void ubi32_pci_write_u8(unsigned char val, const volatile void __iomem *addr);

static  inline unsigned char readb(const volatile void __iomem *addr)
{
	if (IS_PCI_ADDRESS(addr))
		return ubi32_pci_read_u8(addr);
	else
		return (unsigned char)(*(volatile unsigned char *)addr);
}
static inline unsigned short readw(const volatile void __iomem *addr)
{
	if (IS_PCI_ADDRESS(addr))
		return ubi32_pci_read_u16(addr);
	else
		return (unsigned short)(*(volatile unsigned short *)addr);
}

static  inline unsigned int  readl(const volatile void __iomem *addr)
{
	if (IS_PCI_ADDRESS(addr))
		return ubi32_pci_read_u32(addr);
	else
		return (unsigned int)(*(volatile unsigned int *)addr);
}

static inline void writel(unsigned int val, volatile void __iomem *addr)
{
	if (IS_PCI_ADDRESS(addr))
                ubi32_pci_write_u32(val, addr);
        else
		*(volatile unsigned int *)addr = val;
}

static inline void writew(unsigned short val, volatile void __iomem *addr)
{
	if (IS_PCI_ADDRESS(addr))
                ubi32_pci_write_u16(val, addr);
        else
		*(volatile unsigned short *)addr = val;
}

static inline void writeb(unsigned char val, volatile void __iomem *addr)
{
	if (IS_PCI_ADDRESS(addr))
                ubi32_pci_write_u8(val, addr);
        else
		*(volatile unsigned char *)addr = val;
}
#endif

#define readb_relaxed(addr) readb(addr)
#define readw_relaxed(addr) readw(addr)
#define readl_relaxed(addr) readl(addr)


#define __raw_readb readb
#define __raw_readw readw
#define __raw_readl readl
#define __raw_writeb writeb
#define __raw_writew writew
#define __raw_writel writel

static inline void io_outsb(unsigned int addr, const void *buf, int len)
{
	volatile unsigned char *ap = (volatile unsigned char *) addr;
	unsigned char *bp = (unsigned char *) buf;
	while (len--)
		*ap = *bp++;
}

static inline void io_outsw(unsigned int addr, const void *buf, int len)
{
	volatile unsigned short *ap = (volatile unsigned short *) addr;
	unsigned short *bp = (unsigned short *) buf;
	while (len--)
		*ap = _swapw(*bp++);
}

static inline void io_outsl(unsigned int addr, const void *buf, int len)
{
	volatile unsigned int *ap = (volatile unsigned int *) addr;
	unsigned int *bp = (unsigned int *) buf;
	while (len--)
		*ap = _swapl(*bp++);
}

static inline void io_insb(unsigned int addr, void *buf, int len)
{
	volatile unsigned char *ap = (volatile unsigned char *) addr;
	unsigned char *bp = (unsigned char *) buf;
	while (len--)
		*bp++ = *ap;
}

static inline void io_insw(unsigned int addr, void *buf, int len)
{
	volatile unsigned short *ap = (volatile unsigned short *) addr;
	unsigned short *bp = (unsigned short *) buf;
	while (len--)
		*bp++ = _swapw(*ap);
}

static inline void io_insl(unsigned int addr, void *buf, int len)
{
	volatile unsigned int *ap = (volatile unsigned int *) addr;
	unsigned int *bp = (unsigned int *) buf;
	while (len--)
		*bp++ = _swapl(*ap);
}

#define mmiowb()

/*
 *	make the short names macros so specific devices
 *	can override them as required
 */
#ifndef CONFIG_PCI
#define memset_io(a,b,c)	memset((void *)(a),(b),(c))
#define memcpy_fromio(a,b,c)	memcpy((a),(void *)(b),(c))
#define memcpy_toio(a,b,c)	memcpy((void *)(a),(b),(c))
#else
extern void memcpy_fromio(void *to, const volatile void __iomem *from, unsigned len);
extern void memcpy_toio(volatile void __iomem *to, const void *from, unsigned len);
extern void memset_io(volatile void __iomem *addr, int val, size_t count);
#endif

#define inb(addr)    readb(addr)
#define inw(addr)    readw(addr)
#define inl(addr)    readl(addr)
#define outb(x,addr) ((void) writeb(x,addr))
#define outw(x,addr) ((void) writew(x,addr))
#define outl(x,addr) ((void) writel(x,addr))

#define inb_p(addr)    inb(addr)
#define inw_p(addr)    inw(addr)
#define inl_p(addr)    inl(addr)
#define outb_p(x,addr) outb(x,addr)
#define outw_p(x,addr) outw(x,addr)
#define outl_p(x,addr) outl(x,addr)

#define outsb(a,b,l) io_outsb(a,b,l)
#define outsw(a,b,l) io_outsw(a,b,l)
#define outsl(a,b,l) io_outsl(a,b,l)

#define insb(a,b,l) io_insb(a,b,l)
#define insw(a,b,l) io_insw(a,b,l)
#define insl(a,b,l) io_insl(a,b,l)

#ifndef CONFIG_PCI
#define ioread8_rep(a,d,c)	insb(a,d,c)
#define ioread16_rep(a,d,c)	insw(a,d,c)
#define ioread32_rep(a,d,c)	insl(a,d,c)
#define iowrite8_rep(a,s,c)	outsb(a,s,c)
#define iowrite16_rep(a,s,c)	outsw(a,s,c)
#define iowrite32_rep(a,s,c)	outsl(a,s,c)
#else
extern void  ioread8_rep(void __iomem *port, void *buf, unsigned long count);
extern void  ioread16_rep(void __iomem *port, void *buf, unsigned long count);
extern void  ioread32_rep(void __iomem *port, void *buf, unsigned long count);
extern void  iowrite8_rep(void __iomem *port, const void *buf, unsigned long count);
extern void  iowrite16_rep(void __iomem *port, const void *buf, unsigned long count);
extern void  iowrite32_rep(void __iomem *port, const void *buf, unsigned long count);
#endif


#ifndef CONFIG_PCI
#define ioread8(X)			readb(X)
#define ioread16(X)			readw(X)
#define ioread32(X)			readl(X)
#define iowrite8(val,X)			writeb(val,X)
#define iowrite16(val,X)		writew(val,X)
#define iowrite32(val,X)		writel(val,X)
#else /*CONFIG_PCI */
extern  unsigned char  ioread8(void __iomem *addr);
extern  unsigned short ioread16(void __iomem *addr);
extern  unsigned int  ioread32(void __iomem *addr);
extern  void iowrite8(unsigned char val, void __iomem *addr);
extern  void iowrite16(unsigned short val, void __iomem *addr);
extern  void iowrite32(unsigned int val, void __iomem *addr);
#endif /* CONFIG_PCI */

#define IO_SPACE_LIMIT 0xffff

/* Values for nocacheflag and cmode */
#define IOMAP_FULL_CACHING		0
#define IOMAP_NOCACHE_SER		1
#define IOMAP_NOCACHE_NONSER		2
#define IOMAP_WRITETHROUGH		3

extern void *__ioremap(unsigned long physaddr, unsigned long size, int cacheflag);
extern void __iounmap(void *addr, unsigned long size);

static inline void *ioremap(unsigned long physaddr, unsigned long size)
{
	return __ioremap(physaddr, size, IOMAP_NOCACHE_SER);
}
static inline void *ioremap_nocache(unsigned long physaddr, unsigned long size)
{
	return __ioremap(physaddr, size, IOMAP_NOCACHE_SER);
}
static inline void *ioremap_writethrough(unsigned long physaddr, unsigned long size)
{
	return __ioremap(physaddr, size, IOMAP_WRITETHROUGH);
}
static inline void *ioremap_fullcache(unsigned long physaddr, unsigned long size)
{
	return __ioremap(physaddr, size, IOMAP_FULL_CACHING);
}

extern void iounmap(void *addr);

#define ioport_map(port, nr)            ((void __iomem*)(port))
#define ioport_unmap(addr)


/* Pages to physical address... */
#define page_to_phys(page)      ((page - mem_map) << PAGE_SHIFT)
#define page_to_bus(page)       ((page - mem_map) << PAGE_SHIFT)

/*
 * Macros used for converting between virtual and physical mappings.
 */
#define phys_to_virt(vaddr)	((void *) (vaddr))
#define virt_to_phys(vaddr)	((unsigned long) (vaddr))

#define virt_to_bus virt_to_phys
#define bus_to_virt phys_to_virt

/*
 * Convert a physical pointer to a virtual kernel pointer for /dev/mem
 * access
 */
#define xlate_dev_mem_ptr(p)	__va(p)

/*
 * Convert a virtual cached pointer to an uncached pointer
 */
#define xlate_dev_kmem_ptr(p)	p

#endif /* __KERNEL__ */

#endif /* _ASM_UBICOM32_IO_H */
