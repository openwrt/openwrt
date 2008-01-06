#ifndef __osl_h
#define __osl_h

#include <linux/delay.h>
#include <typedefs.h>
#include <linuxver.h>
#include <pcicfg.h>

#define ASSERT(n)

#ifndef ABS
#define	ABS(a)			(((a) < 0)?-(a):(a))
#endif /* ABS */

#ifndef MIN
#define	MIN(a, b)		(((a) < (b))?(a):(b))
#endif /* MIN */

#ifndef MAX
#define	MAX(a, b)		(((a) > (b))?(a):(b))
#endif /* MAX */

#define CEIL(x, y)		(((x) + ((y)-1)) / (y))
#define	ROUNDUP(x, y)		((((x)+((y)-1))/(y))*(y))
#define	ISALIGNED(a, x)		(((a) & ((x)-1)) == 0)
#define	ISPOWEROF2(x)		((((x)-1)&(x)) == 0)
#define VALID_MASK(mask)	!((mask) & ((mask) + 1))
#ifndef OFFSETOF
#define	OFFSETOF(type, member)	((uint)(uintptr)&((type *)0)->member)
#endif /* OFFSETOF */
#ifndef ARRAYSIZE
#define ARRAYSIZE(a)		(sizeof(a)/sizeof(a[0]))
#endif

/*
 * Spin at most 'us' microseconds while 'exp' is true.
 * Caller should explicitly test 'exp' when this completes
 * and take appropriate error action if 'exp' is still true.
 */
#define SPINWAIT(exp, us) { \
	uint countdown = (us) + 9; \
	while ((exp) && (countdown >= 10)) {\
		OSL_DELAY(10); \
		countdown -= 10; \
	} \
}


typedef void (*pktfree_cb_fn_t)(void *ctx, void *pkt, unsigned int status);
/* Pkttag flag should be part of public information */
typedef struct {
	bool pkttag;
	uint pktalloced; 	/* Number of allocated packet buffers */
	bool mmbus;		/* Bus supports memory-mapped register accesses */
	pktfree_cb_fn_t tx_fn;  /* Callback function for PKTFREE */
	void *tx_ctx;		/* Context to the callback function */
} osl_pubinfo_t;

struct osl_info {
  osl_pubinfo_t pub;
  uint magic;
  void *pdev;
  uint malloced;
  uint failed;
  uint bustype;
  void *dbgmem_list;
};

typedef struct osl_info osl_t;

#define PCI_CFG_RETRY                10

/* map/unmap direction */
#define DMA_TX  1   /* TX direction for DMA */
#define DMA_RX  2   /* RX direction for DMA */

#define	AND_REG(osh, r, v)		W_REG(osh, (r), R_REG(osh, r) & (v))
#define	OR_REG(osh, r, v)		W_REG(osh, (r), R_REG(osh, r) | (v))
#define SET_REG(osh, r, mask, val)	W_REG((osh), (r), ((R_REG((osh), r) & ~(mask)) | (val)))

/* bcopy, bcmp, and bzero */
#define	bcopy(src, dst, len)	memcpy((dst), (src), (len))
#define	bcmp(b1, b2, len)	memcmp((b1), (b2), (len))
#define	bzero(b, len)		memset((b), '\0', (len))

/* uncached virtual address */
#ifdef mips
#define OSL_UNCACHED(va)	KSEG1ADDR((va))
#include <asm/addrspace.h>
#else
#define OSL_UNCACHED(va)	(va)
#endif /* mips */


#ifndef IL_BIGENDIAN
#define R_REG(osh, r) (\
	sizeof(*(r)) == sizeof(uint8) ? readb((volatile uint8*)(r)) : \
	sizeof(*(r)) == sizeof(uint16) ? readw((volatile uint16*)(r)) : \
	readl((volatile uint32*)(r)) \
)
#define W_REG(osh, r, v) do { \
	switch (sizeof(*(r))) { \
	case sizeof(uint8):	writeb((uint8)(v), (volatile uint8*)(r)); break; \
	case sizeof(uint16):	writew((uint16)(v), (volatile uint16*)(r)); break; \
	case sizeof(uint32):	writel((uint32)(v), (volatile uint32*)(r)); break; \
	} \
} while (0)
#else	/* IL_BIGENDIAN */
#define R_REG(osh, r) ({ \
	__typeof(*(r)) __osl_v; \
	switch (sizeof(*(r))) { \
	case sizeof(uint8):	__osl_v = readb((volatile uint8*)((uint32)r^3)); break; \
	case sizeof(uint16):	__osl_v = readw((volatile uint16*)((uint32)r^2)); break; \
	case sizeof(uint32):	__osl_v = readl((volatile uint32*)(r)); break; \
	} \
	__osl_v; \
})
#define W_REG(osh, r, v) do { \
	switch (sizeof(*(r))) { \
	case sizeof(uint8):	writeb((uint8)(v), (volatile uint8*)((uint32)r^3)); break; \
	case sizeof(uint16):	writew((uint16)(v), (volatile uint16*)((uint32)r^2)); break; \
	case sizeof(uint32):	writel((uint32)(v), (volatile uint32*)(r)); break; \
	} \
} while (0)
#endif /* IL_BIGENDIAN */

/* dereference an address that may cause a bus exception */
#define	BUSPROBE(val, addr)	get_dbe((val), (addr))
#include <asm/paccess.h>

/* map/unmap physical to virtual I/O */
#define	REG_MAP(pa, size)	ioremap_nocache((unsigned long)(pa), (unsigned long)(size))
#define	REG_UNMAP(va)		iounmap((void *)(va))

/* shared (dma-able) memory access macros */
#define	R_SM(r)			*(r)
#define	W_SM(r, v)		(*(r) = (v))
#define	BZERO_SM(r, len)	memset((r), '\0', (len))

#define	MALLOC(osh, size)	kmalloc((size), GFP_ATOMIC)
#define	MFREE(osh, addr, size)	kfree((addr))
#define MALLOCED(osh)	(0)	

#define	OSL_DELAY	_osl_delay
static inline void _osl_delay(uint usec)
{
	uint d;

	while (usec > 0) {
		d = MIN(usec, 1000);
		udelay(d);
		usec -= d;
	}
}

static inline void
bcm_mdelay(uint ms)
{
	uint i;

	for (i = 0; i < ms; i++) {
		OSL_DELAY(1000);
	}
}


#define	OSL_PCMCIA_READ_ATTR(osh, offset, buf, size)
#define	OSL_PCMCIA_WRITE_ATTR(osh, offset, buf, size)

#define	OSL_PCI_READ_CONFIG(osh, offset, size) \
	_osl_pci_read_config((osh), (offset), (size))

static inline uint32
_osl_pci_read_config(osl_t *osh, uint offset, uint size)
{
	uint val;
	uint retry = PCI_CFG_RETRY;	 

	do {
		pci_read_config_dword(osh->pdev, offset, &val);
		if (val != 0xffffffff)
			break;
	} while (retry--);

	return (val);
}

#define	OSL_PCI_WRITE_CONFIG(osh, offset, size, val) \
	_osl_pci_write_config((osh), (offset), (size), (val))
static inline void
_osl_pci_write_config(osl_t *osh, uint offset, uint size, uint val)
{
	uint retry = PCI_CFG_RETRY;	 

	do {
		pci_write_config_dword(osh->pdev, offset, val);
		if (offset != PCI_BAR0_WIN)
			break;
		if (_osl_pci_read_config(osh, offset, size) == val)
			break;
	} while (retry--);
}


/* return bus # for the pci device pointed by osh->pdev */
#define OSL_PCI_BUS(osh)	_osl_pci_bus(osh)
static inline uint
_osl_pci_bus(osl_t *osh)
{
	return ((struct pci_dev *)osh->pdev)->bus->number;
}

/* return slot # for the pci device pointed by osh->pdev */
#define OSL_PCI_SLOT(osh)	_osl_pci_slot(osh)
static inline uint
_osl_pci_slot(osl_t *osh)
{
	return PCI_SLOT(((struct pci_dev *)osh->pdev)->devfn);
}

#endif
