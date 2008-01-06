/*
 * Linux OS Independent Layer
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#define LINUX_OSL

#include <typedefs.h>
#include <bcmendian.h>
#include <linuxver.h>
#include <bcmdefs.h>
#include <osl.h>
#include "linux_osl.h"
#include "bcmutils.h"
#include <linux/delay.h>
#ifdef mips
#include <asm/paccess.h>
#endif /* mips */
#include <pcicfg.h>

#define PCI_CFG_RETRY 		10

#define OS_HANDLE_MAGIC		0x1234abcd	/* Magic # to recognise osh */
#define BCM_MEM_FILENAME_LEN 	24	/* Mem. filename length */

typedef struct bcm_mem_link
{
  struct bcm_mem_link *prev;
  struct bcm_mem_link *next;
  uint size;
  int line;
  char file[BCM_MEM_FILENAME_LEN];
} bcm_mem_link_t;

#if 0
struct osl_info
{
  osl_pubinfo_t pub;
  uint magic;
  void *pdev;
  uint malloced;
  uint failed;
  uint bustype;
  bcm_mem_link_t *dbgmem_list;
#ifdef BCMDBG_PKT		/* pkt logging for debugging */
  pktlist_info_t pktlist;
#endif				/* BCMDBG_PKT */
};
#endif

static int16 linuxbcmerrormap[] = { 0,	/* 0 */
  -EINVAL,			/* BCME_ERROR */
  -EINVAL,			/* BCME_BADARG */
  -EINVAL,			/* BCME_BADOPTION */
  -EINVAL,			/* BCME_NOTUP */
  -EINVAL,			/* BCME_NOTDOWN */
  -EINVAL,			/* BCME_NOTAP */
  -EINVAL,			/* BCME_NOTSTA */
  -EINVAL,			/* BCME_BADKEYIDX */
  -EINVAL,			/* BCME_RADIOOFF */
  -EINVAL,			/* BCME_NOTBANDLOCKED */
  -EINVAL,			/* BCME_NOCLK */
  -EINVAL,			/* BCME_BADRATESET */
  -EINVAL,			/* BCME_BADBAND */
  -E2BIG,			/* BCME_BUFTOOSHORT */
  -E2BIG,			/* BCME_BUFTOOLONG */
  -EBUSY,			/* BCME_BUSY */
  -EINVAL,			/* BCME_NOTASSOCIATED */
  -EINVAL,			/* BCME_BADSSIDLEN */
  -EINVAL,			/* BCME_OUTOFRANGECHAN */
  -EINVAL,			/* BCME_BADCHAN */
  -EFAULT,			/* BCME_BADADDR */
  -ENOMEM,			/* BCME_NORESOURCE */
  -EOPNOTSUPP,			/* BCME_UNSUPPORTED */
  -EMSGSIZE,			/* BCME_BADLENGTH */
  -EINVAL,			/* BCME_NOTREADY */
  -EPERM,			/* BCME_NOTPERMITTED */
  -ENOMEM,			/* BCME_NOMEM */
  -EINVAL,			/* BCME_ASSOCIATED */
  -ERANGE,			/* BCME_RANGE */
  -EINVAL,			/* BCME_NOTFOUND */
  -EINVAL,			/* BCME_WME_NOT_ENABLED */
  -EINVAL,			/* BCME_TSPEC_NOTFOUND */
  -EINVAL,			/* BCME_ACM_NOTSUPPORTED */
  -EINVAL,			/* BCME_NOT_WME_ASSOCIATION */
  -EIO,				/* BCME_SDIO_ERROR */
  -ENODEV,			/* BCME_DONGLE_DOWN */
  -EINVAL			/* BCME_VERSION */
/* When an new error code is added to bcmutils.h, add os 
 * spcecific error translation here as well
 */
/* check if BCME_LAST changed since the last time this function was updated */
#if BCME_LAST != -37
#error "You need to add a OS error translation in the linuxbcmerrormap \
	for new error code defined in bcmuitls.h"
#endif /* BCME_LAST != -37 */
};

/* translate bcmerrors into linux errors */
int
osl_error (int bcmerror)
{
  if (bcmerror > 0)
    bcmerror = 0;
  else if (bcmerror < BCME_LAST)
    bcmerror = BCME_ERROR;

  /* Array bounds covered by ASSERT in osl_attach */
  return linuxbcmerrormap[-bcmerror];
}

osl_t *
osl_attach (void *pdev, uint bustype, bool pkttag)
{
  osl_t *osh;

  osh = kmalloc (sizeof (osl_t), GFP_ATOMIC);
  ASSERT (osh);

  bzero (osh, sizeof (osl_t));

  /* Check that error map has the right number of entries in it */
  ASSERT (ABS (BCME_LAST) == (ARRAYSIZE (linuxbcmerrormap) - 1));

  osh->magic = OS_HANDLE_MAGIC;
  osh->malloced = 0;
  osh->failed = 0;
  osh->dbgmem_list = NULL;
  osh->pdev = pdev;
  osh->pub.pkttag = pkttag;
  osh->bustype = bustype;

  switch (bustype)
    {
    case PCI_BUS:
    case SB_BUS:
    case PCMCIA_BUS:
      osh->pub.mmbus = TRUE;
      break;
    case JTAG_BUS:
    case SDIO_BUS:
      break;
    default:
      ASSERT (FALSE);
      break;
    }

#ifdef BCMDBG
  if (pkttag)
    {
      struct sk_buff *skb;
      ASSERT (OSL_PKTTAG_SZ <= sizeof (skb->cb));
    }
#endif
  return osh;
}

void
osl_detach (osl_t * osh)
{
  if (osh == NULL)
    return;

  ASSERT (osh->magic == OS_HANDLE_MAGIC);
  kfree (osh);
}

/* Return a new packet. zero out pkttag */
void *
osl_pktget (osl_t * osh, uint len)
{
  struct sk_buff *skb;

  if ((skb = dev_alloc_skb (len)))
    {
      skb_put (skb, len);
      skb->priority = 0;

#ifdef BCMDBG_PKT
      pktlist_add (&(osh->pktlist), (void *) skb);
#endif /* BCMDBG_PKT */

      osh->pub.pktalloced++;
    }

  return ((void *) skb);
}

/* Free the driver packet. Free the tag if present */
void
osl_pktfree (osl_t * osh, void *p, bool send)
{
  struct sk_buff *skb, *nskb;

  skb = (struct sk_buff *) p;

  if (send && osh->pub.tx_fn)
    osh->pub.tx_fn (osh->pub.tx_ctx, p, 0);

  /* perversion: we use skb->next to chain multi-skb packets */
  while (skb)
    {
      nskb = skb->next;
      skb->next = NULL;

#ifdef BCMDBG_PKT
      pktlist_remove (&(osh->pktlist), (void *) skb);
#endif /* BCMDBG_PKT */

      if (skb->destructor)
	{
	  /* cannot kfree_skb() on hard IRQ (net/core/skbuff.c) if destructor exists
	   */
	  dev_kfree_skb_any (skb);
	}
      else
	{
	  /* can free immediately (even in_irq()) if destructor does not exist */
	  dev_kfree_skb (skb);
	}

      osh->pub.pktalloced--;

      skb = nskb;
    }
}

uint32
osl_pci_read_config (osl_t * osh, uint offset, uint size)
{
  uint val;
  uint retry = PCI_CFG_RETRY;

  ASSERT ((osh && (osh->magic == OS_HANDLE_MAGIC)));

  /* only 4byte access supported */
  ASSERT (size == 4);

  do
    {
      pci_read_config_dword (osh->pdev, offset, &val);
      if (val != 0xffffffff)
	break;
    }
  while (retry--);

#ifdef BCMDBG
  if (retry < PCI_CFG_RETRY)
    printk ("PCI CONFIG READ access to %d required %d retries\n", offset,
	    (PCI_CFG_RETRY - retry));
#endif /* BCMDBG */

  return (val);
}

void
osl_pci_write_config (osl_t * osh, uint offset, uint size, uint val)
{
  uint retry = PCI_CFG_RETRY;

  ASSERT ((osh && (osh->magic == OS_HANDLE_MAGIC)));

  /* only 4byte access supported */
  ASSERT (size == 4);

  do
    {
      pci_write_config_dword (osh->pdev, offset, val);
      if (offset != PCI_BAR0_WIN)
	break;
      if (osl_pci_read_config (osh, offset, size) == val)
	break;
    }
  while (retry--);

#ifdef BCMDBG
  if (retry < PCI_CFG_RETRY)
    printk ("PCI CONFIG WRITE access to %d required %d retries\n", offset,
	    (PCI_CFG_RETRY - retry));
#endif /* BCMDBG */
}

/* return bus # for the pci device pointed by osh->pdev */
uint
osl_pci_bus (osl_t * osh)
{
  ASSERT (osh && (osh->magic == OS_HANDLE_MAGIC) && osh->pdev);

  return ((struct pci_dev *) osh->pdev)->bus->number;
}

/* return slot # for the pci device pointed by osh->pdev */
uint
osl_pci_slot (osl_t * osh)
{
  ASSERT (osh && (osh->magic == OS_HANDLE_MAGIC) && osh->pdev);

  return PCI_SLOT (((struct pci_dev *) osh->pdev)->devfn);
}

static void
osl_pcmcia_attr (osl_t * osh, uint offset, char *buf, int size, bool write)
{
}

void
osl_pcmcia_read_attr (osl_t * osh, uint offset, void *buf, int size)
{
  osl_pcmcia_attr (osh, offset, (char *) buf, size, FALSE);
}

void
osl_pcmcia_write_attr (osl_t * osh, uint offset, void *buf, int size)
{
  osl_pcmcia_attr (osh, offset, (char *) buf, size, TRUE);
}


#ifdef BCMDBG_MEM

void *
osl_debug_malloc (osl_t * osh, uint size, int line, char *file)
{
  bcm_mem_link_t *p;
  char *basename;

  ASSERT (size);

  if ((p =
       (bcm_mem_link_t *) osl_malloc (osh,
				      sizeof (bcm_mem_link_t) + size)) ==
      NULL)
    return (NULL);

  p->size = size;
  p->line = line;

  basename = strrchr (file, '/');
  /* skip the '/' */
  if (basename)
    basename++;

  if (!basename)
    basename = file;

  strncpy (p->file, basename, BCM_MEM_FILENAME_LEN);
  p->file[BCM_MEM_FILENAME_LEN - 1] = '\0';

  /* link this block */
  p->prev = NULL;
  p->next = osh->dbgmem_list;
  if (p->next)
    p->next->prev = p;
  osh->dbgmem_list = p;

  return p + 1;
}

void
osl_debug_mfree (osl_t * osh, void *addr, uint size, int line, char *file)
{
  bcm_mem_link_t *p =
    (bcm_mem_link_t *) ((int8 *) addr - sizeof (bcm_mem_link_t));

  ASSERT ((osh && (osh->magic == OS_HANDLE_MAGIC)));

  if (p->size == 0)
    {
      printk
	("osl_debug_mfree: double free on addr %p size %d at line %d file %s\n",
	 addr, size, line, file);
      ASSERT (p->size);
      return;
    }

  if (p->size != size)
    {
      printk
	("osl_debug_mfree: dealloc size %d does not match alloc size %d on addr %p"
	 " at line %d file %s\n", size, p->size, addr, line, file);
      ASSERT (p->size == size);
      return;
    }

  /* unlink this block */
  if (p->prev)
    p->prev->next = p->next;
  if (p->next)
    p->next->prev = p->prev;
  if (osh->dbgmem_list == p)
    osh->dbgmem_list = p->next;
  p->next = p->prev = NULL;

  osl_mfree (osh, p, size + sizeof (bcm_mem_link_t));
}

int
osl_debug_memdump (osl_t * osh, struct bcmstrbuf *b)
{
  bcm_mem_link_t *p;

  ASSERT ((osh && (osh->magic == OS_HANDLE_MAGIC)));

  bcm_bprintf (b, "   Address\tSize\tFile:line\n");
  for (p = osh->dbgmem_list; p; p = p->next)
    bcm_bprintf (b, "0x%08x\t%5d\t%s:%d\n",
		 (uintptr) p + sizeof (bcm_mem_link_t), p->size, p->file,
		 p->line);

  return 0;
}

#endif /* BCMDBG_MEM */

void *
osl_malloc (osl_t * osh, uint size)
{
  void *addr;

  /* only ASSERT if osh is defined */
  if (osh)
    ASSERT (osh->magic == OS_HANDLE_MAGIC);

  if ((addr = kmalloc (size, GFP_ATOMIC)) == NULL)
    {
      if (osh)
	osh->failed++;
      return (NULL);
    }
  if (osh)
    osh->malloced += size;

  return (addr);
}

void
osl_mfree (osl_t * osh, void *addr, uint size)
{
  if (osh)
    {
      ASSERT (osh->magic == OS_HANDLE_MAGIC);
      osh->malloced -= size;
    }
  kfree (addr);
}

uint
osl_malloced (osl_t * osh)
{
  ASSERT ((osh && (osh->magic == OS_HANDLE_MAGIC)));
  return (osh->malloced);
}

uint
osl_malloc_failed (osl_t * osh)
{
  ASSERT ((osh && (osh->magic == OS_HANDLE_MAGIC)));
  return (osh->failed);
}

void *
osl_dma_alloc_consistent (osl_t * osh, uint size, ulong * pap)
{
  ASSERT ((osh && (osh->magic == OS_HANDLE_MAGIC)));

  return (pci_alloc_consistent (osh->pdev, size, (dma_addr_t *) pap));
}

void
osl_dma_free_consistent (osl_t * osh, void *va, uint size, ulong pa)
{
  ASSERT ((osh && (osh->magic == OS_HANDLE_MAGIC)));

  pci_free_consistent (osh->pdev, size, va, (dma_addr_t) pa);
}

uint
osl_dma_map (osl_t * osh, void *va, uint size, int direction)
{
  int dir;

  ASSERT ((osh && (osh->magic == OS_HANDLE_MAGIC)));
  dir = (direction == DMA_TX) ? PCI_DMA_TODEVICE : PCI_DMA_FROMDEVICE;
  return (pci_map_single (osh->pdev, va, size, dir));
}

void
osl_dma_unmap (osl_t * osh, uint pa, uint size, int direction)
{
  int dir;

  ASSERT ((osh && (osh->magic == OS_HANDLE_MAGIC)));
  dir = (direction == DMA_TX) ? PCI_DMA_TODEVICE : PCI_DMA_FROMDEVICE;
  pci_unmap_single (osh->pdev, (uint32) pa, size, dir);
}

#if defined(BINOSL) || defined(BCMDBG_ASSERT)
void
osl_assert (char *exp, char *file, int line)
{
  char tempbuf[255];

  sprintf (tempbuf, "assertion \"%s\" failed: file \"%s\", line %d\n", exp,
	   file, line);
  panic (tempbuf);
}
#endif /* BCMDBG_ASSERT || BINOSL */

void
osl_delay (uint usec)
{
  uint d;

  while (usec > 0)
    {
      d = MIN (usec, 1000);
      udelay (d);
      usec -= d;
    }
}

/* Clone a packet.
 * The pkttag contents are NOT cloned.
 */
void *
osl_pktdup (osl_t * osh, void *skb)
{
  void *p;

  if ((p = skb_clone ((struct sk_buff *) skb, GFP_ATOMIC)) == NULL)
    return NULL;

  /* skb_clone copies skb->cb.. we don't want that */
  if (osh->pub.pkttag)
    bzero ((void *) ((struct sk_buff *) p)->cb, OSL_PKTTAG_SZ);

  /* Increment the packet counter */
  osh->pub.pktalloced++;
#ifdef BCMDBG_PKT
  pktlist_add (&(osh->pktlist), (void *) p);
#endif /* BCMDBG_PKT */
  return (p);
}

uint
osl_pktalloced (osl_t * osh)
{
  return (osh->pub.pktalloced);
}

#ifdef BCMDBG_PKT
char *
osl_pktlist_dump (osl_t * osh, char *buf)
{
  pktlist_dump (&(osh->pktlist), buf);
  return buf;
}

void
osl_pktlist_add (osl_t * osh, void *p)
{
  pktlist_add (&(osh->pktlist), p);
}

void
osl_pktlist_remove (osl_t * osh, void *p)
{
  pktlist_remove (&(osh->pktlist), p);
}
#endif /* BCMDBG_PKT */

/*
 * BINOSL selects the slightly slower function-call-based binary compatible osl.
 */
#ifdef BINOSL

int
osl_printf (const char *format, ...)
{
  va_list args;
  char buf[1024];
  int len;

  /* sprintf into a local buffer because there *is* no "vprintk()".. */
  va_start (args, format);
  len = vsnprintf (buf, 1024, format, args);
  va_end (args);

  if (len > sizeof (buf))
    {
      printk ("osl_printf: buffer overrun\n");
      return (0);
    }

  return (printk (buf));
}

int
osl_sprintf (char *buf, const char *format, ...)
{
  va_list args;
  int rc;

  va_start (args, format);
  rc = vsprintf (buf, format, args);
  va_end (args);
  return (rc);
}

int
osl_strcmp (const char *s1, const char *s2)
{
  return (strcmp (s1, s2));
}

int
osl_strncmp (const char *s1, const char *s2, uint n)
{
  return (strncmp (s1, s2, n));
}

int
osl_strlen (const char *s)
{
  return (strlen (s));
}

char *
osl_strcpy (char *d, const char *s)
{
  return (strcpy (d, s));
}

char *
osl_strncpy (char *d, const char *s, uint n)
{
  return (strncpy (d, s, n));
}

void
bcopy (const void *src, void *dst, int len)
{
  memcpy (dst, src, len);
}

int
bcmp (const void *b1, const void *b2, int len)
{
  return (memcmp (b1, b2, len));
}

void
bzero (void *b, int len)
{
  memset (b, '\0', len);
}

uint32
osl_readl (volatile uint32 * r)
{
  return (readl (r));
}

uint16
osl_readw (volatile uint16 * r)
{
  return (readw (r));
}

uint8
osl_readb (volatile uint8 * r)
{
  return (readb (r));
}

void
osl_writel (uint32 v, volatile uint32 * r)
{
  writel (v, r);
}

void
osl_writew (uint16 v, volatile uint16 * r)
{
  writew (v, r);
}

void
osl_writeb (uint8 v, volatile uint8 * r)
{
  writeb (v, r);
}

void *
osl_uncached (void *va)
{
#ifdef mips
  return ((void *) KSEG1ADDR (va));
#else
  return ((void *) va);
#endif /* mips */
}

uint
osl_getcycles (void)
{
  uint cycles;

#if defined(mips)
  cycles = read_c0_count () * 2;
#elif defined(__i386__)
  rdtscl (cycles);
#else
  cycles = 0;
#endif /* defined(mips) */
  return cycles;
}

void *
osl_reg_map (uint32 pa, uint size)
{
  return (ioremap_nocache ((unsigned long) pa, (unsigned long) size));
}

void
osl_reg_unmap (void *va)
{
  iounmap (va);
}

int
osl_busprobe (uint32 * val, uint32 addr)
{
#ifdef mips
  return get_dbe (*val, (uint32 *) addr);
#else
  *val = readl ((uint32 *) (uintptr) addr);
  return 0;
#endif /* mips */
}

bool
osl_pktshared (void *skb)
{
  return (((struct sk_buff *) skb)->cloned);
}

uchar *
osl_pktdata (osl_t * osh, void *skb)
{
  return (((struct sk_buff *) skb)->data);
}

uint
osl_pktlen (osl_t * osh, void *skb)
{
  return (((struct sk_buff *) skb)->len);
}

uint
osl_pktheadroom (osl_t * osh, void *skb)
{
  return (uint) skb_headroom ((struct sk_buff *) skb);
}

uint
osl_pkttailroom (osl_t * osh, void *skb)
{
  return (uint) skb_tailroom ((struct sk_buff *) skb);
}

void *
osl_pktnext (osl_t * osh, void *skb)
{
  return (((struct sk_buff *) skb)->next);
}

void
osl_pktsetnext (void *skb, void *x)
{
  ((struct sk_buff *) skb)->next = (struct sk_buff *) x;
}

void
osl_pktsetlen (osl_t * osh, void *skb, uint len)
{
  __skb_trim ((struct sk_buff *) skb, len);
}

uchar *
osl_pktpush (osl_t * osh, void *skb, int bytes)
{
  return (skb_push ((struct sk_buff *) skb, bytes));
}

uchar *
osl_pktpull (osl_t * osh, void *skb, int bytes)
{
  return (skb_pull ((struct sk_buff *) skb, bytes));
}

void *
osl_pkttag (void *skb)
{
  return ((void *) (((struct sk_buff *) skb)->cb));
}

void *
osl_pktlink (void *skb)
{
  return (((struct sk_buff *) skb)->prev);
}

void
osl_pktsetlink (void *skb, void *x)
{
  ((struct sk_buff *) skb)->prev = (struct sk_buff *) x;
}

uint
osl_pktprio (void *skb)
{
  return (((struct sk_buff *) skb)->priority);
}

void
osl_pktsetprio (void *skb, uint x)
{
  ((struct sk_buff *) skb)->priority = x;
}

/* Convert a driver packet to native(OS) packet
 * In the process, packettag is zeroed out before sending up
 * IP code depends on skb->cb to be setup correctly with various options
 * In our case, that means it should be 0
 */
struct sk_buff *
osl_pkt_tonative (osl_t * osh, void *pkt)
{
  struct sk_buff *nskb;

  if (osh->pub.pkttag)
    bzero ((void *) ((struct sk_buff *) pkt)->cb, OSL_PKTTAG_SZ);

  /* Decrement the packet counter */
  for (nskb = (struct sk_buff *) pkt; nskb; nskb = nskb->next)
    {
#ifdef BCMDBG_PKT
      pktlist_remove (&(osh->pktlist), (void *) nskb);
#endif /* BCMDBG_PKT */
      osh->pub.pktalloced--;
    }

  return (struct sk_buff *) pkt;
}

/* Convert a native(OS) packet to driver packet.
 * In the process, native packet is destroyed, there is no copying
 * Also, a packettag is zeroed out
 */
void *
osl_pkt_frmnative (osl_t * osh, struct sk_buff *skb)
{
  struct sk_buff *nskb;

  if (osh->pub.pkttag)
    bzero ((void *) skb->cb, OSL_PKTTAG_SZ);

  /* Increment the packet counter */
  for (nskb = skb; nskb; nskb = nskb->next)
    {
#ifdef BCMDBG_PKT
      pktlist_add (&(osh->pktlist), (void *) nskb);
#endif /* BCMDBG_PKT */
      osh->pub.pktalloced++;
    }

  return (void *) skb;
}

#endif /* BINOSL */
