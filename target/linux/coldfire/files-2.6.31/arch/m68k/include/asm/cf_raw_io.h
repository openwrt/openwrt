/*
 * linux/include/asm-m68k/cf_raw_io.h
 *
 * Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 * 09/30/08 JKM: split Coldfire pieces into separate file
 */
#ifndef __CF_RAW_IO__
#define __CF_RAW_IO__

#ifdef __KERNEL__

#include <asm/types.h>

/* Values for nocacheflag and cmode */
#define IOMAP_FULL_CACHING		0
#define IOMAP_NOCACHE_SER		1
#define IOMAP_NOCACHE_NONSER		2
#define IOMAP_WRITETHROUGH		3

extern void iounmap(void __iomem *addr);

extern void __iomem *__ioremap(unsigned long physaddr, unsigned long size,
		       int cacheflag);
extern void __iounmap(void *addr, unsigned long size);


/* ++roman: The assignments to temp. vars avoid that gcc sometimes generates
 * two accesses to memory, which may be undesirable for some devices.
 */
#define in_8(addr) \
    ({ u8 __v = (*(__force volatile u8 *) (addr)); __v; })
#define in_be16(addr) \
    ({ u16 __v = (*(__force volatile u16 *) (addr)); __v; })
#define in_be32(addr) \
    ({ u32 __v = (*(__force volatile u32 *) (addr)); __v; })
#define in_le16(addr) \
    ({ u16 __v = le16_to_cpu(*(__force volatile __le16 *) (addr)); __v; })
#define in_le32(addr) \
    ({ u32 __v = le32_to_cpu(*(__force volatile __le32 *) (addr)); __v; })

#define out_8(addr,b) (void)((*(__force volatile u8 *) (addr)) = (b))
#define out_be16(addr,w) (void)((*(__force volatile u16 *) (addr)) = (w))
#define out_be32(addr,l) (void)((*(__force volatile u32 *) (addr)) = (l))
#define out_le16(addr,w) (void)((*(__force volatile __le16 *) (addr)) = cpu_to_le16(w))
#define out_le32(addr,l) (void)((*(__force volatile __le32 *) (addr)) = cpu_to_le32(l))


#ifdef CONFIG_PCI
/* pci */
unsigned char  pci_inb(long addr);
unsigned short pci_inw(long addr);
unsigned long  pci_inl(long addr);

void pci_outb(unsigned char  val, long addr);
void pci_outw(unsigned short val, long addr);
void pci_outl(unsigned long  val, long addr);

void pci_insb(volatile unsigned char *addr,
		unsigned char *buf, int len);
void pci_insw(volatile unsigned short *addr,
		unsigned short *buf, int len);
void pci_insl(volatile unsigned long *addr,
		unsigned long *buf, int len);

void pci_outsb(volatile unsigned char *addr,
		const unsigned char *buf, int len);
void pci_outsw(volatile unsigned short *addr,
		const unsigned short *buf, int len);
void pci_outsl(volatile unsigned long *addr,
		const unsigned long *buf, int len);

unsigned short pci_raw_inw(long addr);
unsigned long  pci_raw_inl(long addr);
void pci_raw_outw(unsigned short val, long addr);
void pci_raw_outl(unsigned long  val, long addr);

#define raw_inb(port) pci_inb((long)((volatile unsigned char *)(port)))
#define raw_inw(port) pci_raw_inw((long)((volatile unsigned short *)(port)))
#define raw_inl(port) pci_raw_inl((long)((volatile unsigned long *)(port)))

#define raw_outb(val, port) \
	pci_outb((val), (long)((volatile unsigned char *)(port)))
#define raw_outw(val, port) \
	pci_raw_outw((val), (long)((volatile unsigned short *)(port)))
#define raw_outl(val, port) \
	pci_raw_outl((val), (long)((volatile unsigned long *)(port)))

#define swap_inw(port) pci_inw((long)((volatile unsigned short *)(port)))
#define swap_outw(val, port) \
	pci_outw((val), (long)((volatile unsigned short *)(port)))

#else
/* non-pci */
#define raw_inb in_8
#define raw_inw in_be16
#define raw_inl in_be32

#define raw_outb(val,port) out_8((port),(val))
#define raw_outw(val,port) out_be16((port),(val))
#define raw_outl(val,port) out_be32((port),(val))

#define swap_inw(port) in_le16((port))
#define swap_outw(val,port) out_le16((port),(val))
#endif

static inline void raw_insb(volatile u8 __iomem *port, u8 *buf, unsigned int len)
{
	unsigned int i;

        for (i = 0; i < len; i++)
		*buf++ = in_8(port);
}

static inline void raw_outsb(volatile u8 __iomem *port, const u8 *buf,
			     unsigned int len)
{
	unsigned int i;

        for (i = 0; i < len; i++)
		out_8(port, *buf++);
}

static inline void raw_insw(volatile u16 *port, u16 *buf, unsigned int nr)
{
	unsigned int i;

	for (i = 0; i < nr; i++)
		*buf++ = raw_inw(port);
}

static inline void raw_outsw(volatile u16 *port, const u16 *buf,
	unsigned int nr)
{
	unsigned int i;

	for (i = 0; i < nr; i++, buf++)
		raw_outw(*buf, port);
}

static inline void raw_insl(volatile u32 *port, u32 *buf, unsigned int nr)
{
	unsigned int i;

	for (i = 0; i < nr; i++)
		*buf++ = raw_inl(port);
}

static inline void raw_outsl(volatile u32 *port, const u32 *buf,
	unsigned int nr)
{
	unsigned int i;

	for (i = 0; i < nr; i++, buf++)
		raw_outl(*buf, port);
}

static inline void raw_insw_swapw(volatile u16 *port, u16 *buf,
				  unsigned int nr)
{
#ifdef UNDEF
	unsigned int i;

	for (i = 0; i < nr; i++)
		*buf++ = in_le16(port);
#endif
}

static inline void raw_outsw_swapw(volatile u16 __iomem *port, const u16 *buf,
				   unsigned int nr)
{
#ifdef UNDEF
	unsigned int i;

	for (i = 0; i < nr; i++, buf++)
		out_le16(port, *buf);
#endif
}

#endif /* __KERNEL__ */

#endif /* __CF_RAW_IO__ */
