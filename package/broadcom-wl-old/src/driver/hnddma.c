/*
 * Generic Broadcom Home Networking Division (HND) DMA module.
 * This supports the following chips: BCM42xx, 44xx, 47xx .
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include "linux_osl.h"
#include <bcmendian.h>
#include <sbconfig.h>
#include "bcmutils.h"
#include <bcmdevs.h>
#include <sbutils.h>

#include "sbhnddma.h"
#include "hnddma.h"

/* debug/trace */
#ifdef BCMDBG
#define	DMA_ERROR(args) if (!(*di->msg_level & 1)); else printf args
#define	DMA_TRACE(args) if (!(*di->msg_level & 2)); else printf args
#else
#define	DMA_ERROR(args)
#define	DMA_TRACE(args)
#endif

/* default dma message level (if input msg_level pointer is null in dma_attach()) */
static uint dma_msg_level = 0;

#define	MAXNAMEL	8	/* 8 char names */

#define	DI_INFO(dmah)	(dma_info_t *)dmah
typedef struct osl_dmainfo osldma_t;

/* dma engine software state */
typedef struct dma_info
{
  struct hnddma_pub hnddma;	/* exported structure, don't use hnddma_t,
				 * which could be const
				 */
  uint *msg_level;		/* message level pointer */
  char name[MAXNAMEL];		/* callers name for diag msgs */

  void *osh;			/* os handle */
  sb_t *sbh;			/* sb handle */

  bool dma64;			/* dma64 enabled */
  bool addrext;			/* this dma engine supports DmaExtendedAddrChanges */

  dma32regs_t *d32txregs;	/* 32 bits dma tx engine registers */
  dma32regs_t *d32rxregs;	/* 32 bits dma rx engine registers */
  dma64regs_t *d64txregs;	/* 64 bits dma tx engine registers */
  dma64regs_t *d64rxregs;	/* 64 bits dma rx engine registers */

  uint32 dma64align;		/* either 8k or 4k depends on number of dd */
  dma32dd_t *txd32;		/* pointer to dma32 tx descriptor ring */
  dma64dd_t *txd64;		/* pointer to dma64 tx descriptor ring */
  uint ntxd;			/* # tx descriptors tunable */
  uint txin;			/* index of next descriptor to reclaim */
  uint txout;			/* index of next descriptor to post */
  void **txp;			/* pointer to parallel array of pointers to packets */
  osldma_t *tx_dmah;		/* DMA TX descriptor ring handle */
  osldma_t **txp_dmah;		/* DMA TX packet data handle */
  ulong txdpa;			/* physical address of descriptor ring */
  uint txdalign;		/* #bytes added to alloc'd mem to align txd */
  uint txdalloc;		/* #bytes allocated for the ring */

  dma32dd_t *rxd32;		/* pointer to dma32 rx descriptor ring */
  dma64dd_t *rxd64;		/* pointer to dma64 rx descriptor ring */
  uint nrxd;			/* # rx descriptors tunable */
  uint rxin;			/* index of next descriptor to reclaim */
  uint rxout;			/* index of next descriptor to post */
  void **rxp;			/* pointer to parallel array of pointers to packets */
  osldma_t *rx_dmah;		/* DMA RX descriptor ring handle */
  osldma_t **rxp_dmah;		/* DMA RX packet data handle */
  ulong rxdpa;			/* physical address of descriptor ring */
  uint rxdalign;		/* #bytes added to alloc'd mem to align rxd */
  uint rxdalloc;		/* #bytes allocated for the ring */

  /* tunables */
  uint rxbufsize;		/* rx buffer size in bytes,
				   not including the extra headroom
				 */
  uint nrxpost;			/* # rx buffers to keep posted */
  uint rxoffset;		/* rxcontrol offset */
  uint ddoffsetlow;		/* add to get dma address of descriptor ring, low 32 bits */
  uint ddoffsethigh;		/*   high 32 bits */
  uint dataoffsetlow;		/* add to get dma address of data buffer, low 32 bits */
  uint dataoffsethigh;		/*   high 32 bits */
} dma_info_t;

#ifdef BCMDMA64
#define	DMA64_ENAB(di)	((di)->dma64)
#define DMA64_CAP	TRUE
#else
#define	DMA64_ENAB(di)	(0)
#define DMA64_CAP	FALSE
#endif

/* descriptor bumping macros */
#define	XXD(x, n)	((x) & ((n) - 1))	/* faster than %, but n must be power of 2 */
#define	TXD(x)		XXD((x), di->ntxd)
#define	RXD(x)		XXD((x), di->nrxd)
#define	NEXTTXD(i)	TXD(i + 1)
#define	PREVTXD(i)	TXD(i - 1)
#define	NEXTRXD(i)	RXD(i + 1)
#define	NTXDACTIVE(h, t)	TXD(t - h)
#define	NRXDACTIVE(h, t)	RXD(t - h)

/* macros to convert between byte offsets and indexes */
#define	B2I(bytes, type)	((bytes) / sizeof(type))
#define	I2B(index, type)	((index) * sizeof(type))

#define	PCI32ADDR_HIGH		0xc0000000	/* address[31:30] */
#define	PCI32ADDR_HIGH_SHIFT	30	/* address[31:30] */


/* common prototypes */
static bool _dma_isaddrext (dma_info_t * di);
static bool _dma_alloc (dma_info_t * di, uint direction);
static void _dma_detach (dma_info_t * di);
static void _dma_ddtable_init (dma_info_t * di, uint direction, ulong pa);
static void _dma_rxinit (dma_info_t * di);
static void *_dma_rx (dma_info_t * di);
static void _dma_rxfill (dma_info_t * di);
static void _dma_rxreclaim (dma_info_t * di);
static void _dma_rxenable (dma_info_t * di);
static void *_dma_getnextrxp (dma_info_t * di, bool forceall);

static void _dma_txblock (dma_info_t * di);
static void _dma_txunblock (dma_info_t * di);
static uint _dma_txactive (dma_info_t * di);

static void *_dma_peeknexttxp (dma_info_t * di);
static uintptr _dma_getvar (dma_info_t * di, const char *name);
static void _dma_counterreset (dma_info_t * di);
static void _dma_fifoloopbackenable (dma_info_t * di);

/* ** 32 bit DMA prototypes */
static bool dma32_alloc (dma_info_t * di, uint direction);
static bool dma32_txreset (dma_info_t * di);
static bool dma32_rxreset (dma_info_t * di);
static bool dma32_txsuspendedidle (dma_info_t * di);
static int dma32_txfast (dma_info_t * di, void *p0, bool commit);
static void *dma32_getnexttxp (dma_info_t * di, bool forceall);
static void *dma32_getnextrxp (dma_info_t * di, bool forceall);
static void dma32_txrotate (dma_info_t * di);
static bool dma32_rxidle (dma_info_t * di);
static void dma32_txinit (dma_info_t * di);
static bool dma32_txenabled (dma_info_t * di);
static void dma32_txsuspend (dma_info_t * di);
static void dma32_txresume (dma_info_t * di);
static bool dma32_txsuspended (dma_info_t * di);
static void dma32_txreclaim (dma_info_t * di, bool forceall);
static bool dma32_txstopped (dma_info_t * di);
static bool dma32_rxstopped (dma_info_t * di);
static bool dma32_rxenabled (dma_info_t * di);
static bool _dma32_addrext (osl_t * osh, dma32regs_t * dma32regs);

/* ** 64 bit DMA prototypes and stubs */
#ifdef BCMDMA64
static bool dma64_alloc (dma_info_t * di, uint direction);
static bool dma64_txreset (dma_info_t * di);
static bool dma64_rxreset (dma_info_t * di);
static bool dma64_txsuspendedidle (dma_info_t * di);
static int dma64_txfast (dma_info_t * di, void *p0, bool commit);
static void *dma64_getnexttxp (dma_info_t * di, bool forceall);
static void *dma64_getnextrxp (dma_info_t * di, bool forceall);
static void dma64_txrotate (dma_info_t * di);

static bool dma64_rxidle (dma_info_t * di);
static void dma64_txinit (dma_info_t * di);
static bool dma64_txenabled (dma_info_t * di);
static void dma64_txsuspend (dma_info_t * di);
static void dma64_txresume (dma_info_t * di);
static bool dma64_txsuspended (dma_info_t * di);
static void dma64_txreclaim (dma_info_t * di, bool forceall);
static bool dma64_txstopped (dma_info_t * di);
static bool dma64_rxstopped (dma_info_t * di);
static bool dma64_rxenabled (dma_info_t * di);
static bool _dma64_addrext (osl_t * osh, dma64regs_t * dma64regs);

#else
static bool
dma64_alloc (dma_info_t * di, uint direction)
{
  return FALSE;
}
static bool
dma64_txreset (dma_info_t * di)
{
  return FALSE;
}
static bool
dma64_rxreset (dma_info_t * di)
{
  return FALSE;
}
static bool
dma64_txsuspendedidle (dma_info_t * di)
{
  return FALSE;
}
static int
dma64_txfast (dma_info_t * di, void *p0, bool commit)
{
  return 0;
}
static void *
dma64_getnexttxp (dma_info_t * di, bool forceall)
{
  return NULL;
}
static void *
dma64_getnextrxp (dma_info_t * di, bool forceall)
{
  return NULL;
}
static void
dma64_txrotate (dma_info_t * di)
{
  return;
}

static bool
dma64_rxidle (dma_info_t * di)
{
  return FALSE;
}
static void
dma64_txinit (dma_info_t * di)
{
  return;
}
static bool
dma64_txenabled (dma_info_t * di)
{
  return FALSE;
}
static void
dma64_txsuspend (dma_info_t * di)
{
  return;
}
static void
dma64_txresume (dma_info_t * di)
{
  return;
}
static bool
dma64_txsuspended (dma_info_t * di)
{
  return FALSE;
}
static void
dma64_txreclaim (dma_info_t * di, bool forceall)
{
  return;
}
static bool
dma64_txstopped (dma_info_t * di)
{
  return FALSE;
}
static bool
dma64_rxstopped (dma_info_t * di)
{
  return FALSE;
}
static bool
dma64_rxenabled (dma_info_t * di)
{
  return FALSE;
}
static bool
_dma64_addrext (osl_t * osh, dma64regs_t * dma64regs)
{
  return FALSE;
}

#endif /* BCMDMA64 */

#ifdef BCMDBG
static void dma32_dumpring (dma_info_t * di, struct bcmstrbuf *b,
			    dma32dd_t * ring, uint start, uint end,
			    uint max_num);
static void dma32_dump (dma_info_t * di, struct bcmstrbuf *b, bool dumpring);
static void dma32_dumptx (dma_info_t * di, struct bcmstrbuf *b,
			  bool dumpring);
static void dma32_dumprx (dma_info_t * di, struct bcmstrbuf *b,
			  bool dumpring);

static void dma64_dumpring (dma_info_t * di, struct bcmstrbuf *b,
			    dma64dd_t * ring, uint start, uint end,
			    uint max_num);
static void dma64_dump (dma_info_t * di, struct bcmstrbuf *b, bool dumpring);
static void dma64_dumptx (dma_info_t * di, struct bcmstrbuf *b,
			  bool dumpring);
static void dma64_dumprx (dma_info_t * di, struct bcmstrbuf *b,
			  bool dumpring);
#endif


static di_fcn_t dma64proc = {
  (di_detach_t) _dma_detach,
  (di_txinit_t) dma64_txinit,
  (di_txreset_t) dma64_txreset,
  (di_txenabled_t) dma64_txenabled,
  (di_txsuspend_t) dma64_txsuspend,
  (di_txresume_t) dma64_txresume,
  (di_txsuspended_t) dma64_txsuspended,
  (di_txsuspendedidle_t) dma64_txsuspendedidle,
  (di_txfast_t) dma64_txfast,
  (di_txstopped_t) dma64_txstopped,
  (di_txreclaim_t) dma64_txreclaim,
  (di_getnexttxp_t) dma64_getnexttxp,
  (di_peeknexttxp_t) _dma_peeknexttxp,
  (di_txblock_t) _dma_txblock,
  (di_txunblock_t) _dma_txunblock,
  (di_txactive_t) _dma_txactive,
  (di_txrotate_t) dma64_txrotate,

  (di_rxinit_t) _dma_rxinit,
  (di_rxreset_t) dma64_rxreset,
  (di_rxidle_t) dma64_rxidle,
  (di_rxstopped_t) dma64_rxstopped,
  (di_rxenable_t) _dma_rxenable,
  (di_rxenabled_t) dma64_rxenabled,
  (di_rx_t) _dma_rx,
  (di_rxfill_t) _dma_rxfill,
  (di_rxreclaim_t) _dma_rxreclaim,
  (di_getnextrxp_t) _dma_getnextrxp,

  (di_fifoloopbackenable_t) _dma_fifoloopbackenable,
  (di_getvar_t) _dma_getvar,
  (di_counterreset_t) _dma_counterreset,

#ifdef BCMDBG
  (di_dump_t) dma64_dump,
  (di_dumptx_t) dma64_dumptx,
  (di_dumprx_t) dma64_dumprx,
#else
  NULL,
  NULL,
  NULL,
#endif
  34
};

static di_fcn_t dma32proc = {
  (di_detach_t) _dma_detach,
  (di_txinit_t) dma32_txinit,
  (di_txreset_t) dma32_txreset,
  (di_txenabled_t) dma32_txenabled,
  (di_txsuspend_t) dma32_txsuspend,
  (di_txresume_t) dma32_txresume,
  (di_txsuspended_t) dma32_txsuspended,
  (di_txsuspendedidle_t) dma32_txsuspendedidle,
  (di_txfast_t) dma32_txfast,
  (di_txstopped_t) dma32_txstopped,
  (di_txreclaim_t) dma32_txreclaim,
  (di_getnexttxp_t) dma32_getnexttxp,
  (di_peeknexttxp_t) _dma_peeknexttxp,
  (di_txblock_t) _dma_txblock,
  (di_txunblock_t) _dma_txunblock,
  (di_txactive_t) _dma_txactive,
  (di_txrotate_t) dma32_txrotate,

  (di_rxinit_t) _dma_rxinit,
  (di_rxreset_t) dma32_rxreset,
  (di_rxidle_t) dma32_rxidle,
  (di_rxstopped_t) dma32_rxstopped,
  (di_rxenable_t) _dma_rxenable,
  (di_rxenabled_t) dma32_rxenabled,
  (di_rx_t) _dma_rx,
  (di_rxfill_t) _dma_rxfill,
  (di_rxreclaim_t) _dma_rxreclaim,
  (di_getnextrxp_t) _dma_getnextrxp,

  (di_fifoloopbackenable_t) _dma_fifoloopbackenable,
  (di_getvar_t) _dma_getvar,
  (di_counterreset_t) _dma_counterreset,

#ifdef BCMDBG
  (di_dump_t) dma32_dump,
  (di_dumptx_t) dma32_dumptx,
  (di_dumprx_t) dma32_dumprx,
#else
  NULL,
  NULL,
  NULL,
#endif
  34
};

hnddma_t *
dma_attach (osl_t * osh, char *name, sb_t * sbh, void *dmaregstx,
	    void *dmaregsrx, uint ntxd, uint nrxd, uint rxbufsize,
	    uint nrxpost, uint rxoffset, uint * msg_level)
{
  dma_info_t *di;
  uint size;

  /* allocate private info structure */
  if ((di = MALLOC (osh, sizeof (dma_info_t))) == NULL)
    {
#ifdef BCMDBG
      printf ("dma_attach: out of memory, malloced %d bytes\n",
	      MALLOCED (osh));
#endif
      return (NULL);
    }
  bzero ((char *) di, sizeof (dma_info_t));

  di->msg_level = msg_level ? msg_level : &dma_msg_level;

  /* old chips w/o sb is no longer supported */
  ASSERT (sbh != NULL);

  di->dma64 = ((sb_coreflagshi (sbh, 0, 0) & SBTMH_DMA64) == SBTMH_DMA64);

#ifndef BCMDMA64
  if (di->dma64)
    {
      DMA_ERROR (("dma_attach: driver doesn't have the capability to support "
		  "64 bits DMA\n"));
      goto fail;
    }
#endif

  /* check arguments */
  ASSERT (ISPOWEROF2 (ntxd));
  ASSERT (ISPOWEROF2 (nrxd));
  if (nrxd == 0)
    ASSERT (dmaregsrx == NULL);
  if (ntxd == 0)
    ASSERT (dmaregstx == NULL);


  /* init dma reg pointer */
  if (di->dma64)
    {
      ASSERT (ntxd <= D64MAXDD);
      ASSERT (nrxd <= D64MAXDD);
      di->d64txregs = (dma64regs_t *) dmaregstx;
      di->d64rxregs = (dma64regs_t *) dmaregsrx;

      di->dma64align = D64RINGALIGN;
      if ((ntxd < D64MAXDD / 2) && (nrxd < D64MAXDD / 2))
	{
	  /* for smaller dd table, HW relax the alignment requirement */
	  di->dma64align = D64RINGALIGN / 2;
	}
    }
  else
    {
      ASSERT (ntxd <= D32MAXDD);
      ASSERT (nrxd <= D32MAXDD);
      di->d32txregs = (dma32regs_t *) dmaregstx;
      di->d32rxregs = (dma32regs_t *) dmaregsrx;
    }

  DMA_TRACE (("%s: dma_attach: %s osh %p ntxd %d nrxd %d rxbufsize %d nrxpost %d " "rxoffset %d dmaregstx %p dmaregsrx %p\n", name, (di->dma64 ? "DMA64" : "DMA32"), osh, ntxd, nrxd, rxbufsize, nrxpost, rxoffset, dmaregstx, dmaregsrx));

  /* make a private copy of our callers name */
  strncpy (di->name, name, MAXNAMEL);
  di->name[MAXNAMEL - 1] = '\0';

  di->osh = osh;
  di->sbh = sbh;

  /* save tunables */
  di->ntxd = ntxd;
  di->nrxd = nrxd;

  /* the actual dma size doesn't include the extra headroom */
  if (rxbufsize > BCMEXTRAHDROOM)
    di->rxbufsize = rxbufsize - BCMEXTRAHDROOM;
  else
    di->rxbufsize = rxbufsize;

  di->nrxpost = nrxpost;
  di->rxoffset = rxoffset;

  /*
   * figure out the DMA physical address offset for dd and data
   *   for old chips w/o sb, use zero
   *   for new chips w sb,
   *     PCI/PCIE: they map silicon backplace address to zero based memory, need offset
   *     Other bus: use zero
   *     SB_BUS BIGENDIAN kludge: use sdram swapped region for data buffer, not descriptor
   */
  di->ddoffsetlow = 0;
  di->dataoffsetlow = 0;
  /* for pci bus, add offset */
  if (sbh->bustype == PCI_BUS)
    {
      if ((sbh->buscoretype == SB_PCIE) && di->dma64)
	{
	  /* pcie with DMA64 */
	  di->ddoffsetlow = 0;
	  di->ddoffsethigh = SB_PCIE_DMA_H32;
	}
      else
	{
	  /* pci(DMA32/DMA64) or pcie with DMA32 */
	  di->ddoffsetlow = SB_PCI_DMA;
	  di->ddoffsethigh = 0;
	}
      di->dataoffsetlow = di->ddoffsetlow;
      di->dataoffsethigh = di->ddoffsethigh;
    }

#if defined(__mips__) && defined(IL_BIGENDIAN)
  di->dataoffsetlow = di->dataoffsetlow + SB_SDRAM_SWAPPED;
#endif

  di->addrext = _dma_isaddrext (di);

  /* allocate tx packet pointer vector */
  if (ntxd)
    {
      size = ntxd * sizeof (void *);
      if ((di->txp = MALLOC (osh, size)) == NULL)
	{
	  DMA_ERROR (("%s: dma_attach: out of tx memory, malloced %d bytes\n",
		      di->name, MALLOCED (osh)));
	  goto fail;
	}
      bzero ((char *) di->txp, size);
    }

  /* allocate rx packet pointer vector */
  if (nrxd)
    {
      size = nrxd * sizeof (void *);
      if ((di->rxp = MALLOC (osh, size)) == NULL)
	{
	  DMA_ERROR (("%s: dma_attach: out of rx memory, malloced %d bytes\n",
		      di->name, MALLOCED (osh)));
	  goto fail;
	}
      bzero ((char *) di->rxp, size);
    }

  /* allocate transmit descriptor ring, only need ntxd descriptors but it must be aligned */
  if (ntxd)
    {
      if (!_dma_alloc (di, DMA_TX))
	goto fail;
    }

  /* allocate receive descriptor ring, only need nrxd descriptors but it must be aligned */
  if (nrxd)
    {
      if (!_dma_alloc (di, DMA_RX))
	goto fail;
    }

  if ((di->ddoffsetlow == SB_PCI_DMA) && (di->txdpa > SB_PCI_DMA_SZ)
      && !di->addrext)
    {
      DMA_ERROR (("%s: dma_attach: txdpa 0x%lx: addrext not supported\n",
		  di->name, di->txdpa));
      goto fail;
    }
  if ((di->ddoffsetlow == SB_PCI_DMA) && (di->rxdpa > SB_PCI_DMA_SZ)
      && !di->addrext)
    {
      DMA_ERROR (("%s: dma_attach: rxdpa 0x%lx: addrext not supported\n",
		  di->name, di->rxdpa));
      goto fail;
    }

  DMA_TRACE (("ddoffsetlow 0x%x ddoffsethigh 0x%x dataoffsetlow 0x%x dataoffsethigh " "0x%x addrext %d\n", di->ddoffsetlow, di->ddoffsethigh, di->dataoffsetlow, di->dataoffsethigh, di->addrext));

  /* allocate tx packet pointer vector and DMA mapping vectors */
  if (ntxd)
    {

      size = ntxd * sizeof (osldma_t **);
      if ((di->txp_dmah = (osldma_t **) MALLOC (osh, size)) == NULL)
	goto fail;
      bzero ((char *) di->txp_dmah, size);
    }
  else
    di->txp_dmah = NULL;

  /* allocate rx packet pointer vector and DMA mapping vectors */
  if (nrxd)
    {

      size = nrxd * sizeof (osldma_t **);
      if ((di->rxp_dmah = (osldma_t **) MALLOC (osh, size)) == NULL)
	goto fail;
      bzero ((char *) di->rxp_dmah, size);

    }
  else
    di->rxp_dmah = NULL;

  /* initialize opsvec of function pointers */
  di->hnddma.di_fn = DMA64_ENAB (di) ? dma64proc : dma32proc;

  return ((hnddma_t *) di);

fail:
  _dma_detach (di);
  return (NULL);
}

/* init the tx or rx descriptor */
static INLINE void
dma32_dd_upd (dma_info_t * di, dma32dd_t * ddring, ulong pa, uint outidx,
	      uint32 * flags, uint32 bufcount)
{
  /* dma32 uses 32 bits control to fit both flags and bufcounter */
  *flags = *flags | (bufcount & CTRL_BC_MASK);

  if ((di->dataoffsetlow != SB_PCI_DMA) || !(pa & PCI32ADDR_HIGH))
    {
      W_SM (&ddring[outidx].addr, BUS_SWAP32 (pa + di->dataoffsetlow));
      W_SM (&ddring[outidx].ctrl, BUS_SWAP32 (*flags));
    }
  else
    {
      /* address extension */
      uint32 ae;
      ASSERT (di->addrext);
      ae = (pa & PCI32ADDR_HIGH) >> PCI32ADDR_HIGH_SHIFT;
      pa &= ~PCI32ADDR_HIGH;

      *flags |= (ae << CTRL_AE_SHIFT);
      W_SM (&ddring[outidx].addr, BUS_SWAP32 (pa + di->dataoffsetlow));
      W_SM (&ddring[outidx].ctrl, BUS_SWAP32 (*flags));
    }
}

static INLINE void
dma64_dd_upd (dma_info_t * di, dma64dd_t * ddring, ulong pa, uint outidx,
	      uint32 * flags, uint32 bufcount)
{
  uint32 ctrl2 = bufcount & D64_CTRL2_BC_MASK;

  /* PCI bus with big(>1G) physical address, use address extension */
  if ((di->dataoffsetlow != SB_PCI_DMA) || !(pa & PCI32ADDR_HIGH))
    {
      W_SM (&ddring[outidx].addrlow, BUS_SWAP32 (pa + di->dataoffsetlow));
      W_SM (&ddring[outidx].addrhigh, BUS_SWAP32 (0 + di->dataoffsethigh));
      W_SM (&ddring[outidx].ctrl1, BUS_SWAP32 (*flags));
      W_SM (&ddring[outidx].ctrl2, BUS_SWAP32 (ctrl2));
    }
  else
    {
      /* address extension */
      uint32 ae;
      ASSERT (di->addrext);

      ae = (pa & PCI32ADDR_HIGH) >> PCI32ADDR_HIGH_SHIFT;
      pa &= ~PCI32ADDR_HIGH;

      ctrl2 |= (ae << D64_CTRL2_AE_SHIFT) & D64_CTRL2_AE;
      W_SM (&ddring[outidx].addrlow, BUS_SWAP32 (pa + di->dataoffsetlow));
      W_SM (&ddring[outidx].addrhigh, BUS_SWAP32 (0 + di->dataoffsethigh));
      W_SM (&ddring[outidx].ctrl1, BUS_SWAP32 (*flags));
      W_SM (&ddring[outidx].ctrl2, BUS_SWAP32 (ctrl2));
    }
}

static bool
_dma32_addrext (osl_t * osh, dma32regs_t * dma32regs)
{
  uint32 w;

  OR_REG (osh, &dma32regs->control, XC_AE);
  w = R_REG (osh, &dma32regs->control);
  AND_REG (osh, &dma32regs->control, ~XC_AE);
  return ((w & XC_AE) == XC_AE);
}

static bool
_dma_alloc (dma_info_t * di, uint direction)
{
  if (DMA64_ENAB (di))
    {
      return dma64_alloc (di, direction);
    }
  else
    {
      return dma32_alloc (di, direction);
    }
}

/* !! may be called with core in reset */
static void
_dma_detach (dma_info_t * di)
{
  if (di == NULL)
    return;

  DMA_TRACE (("%s: dma_detach\n", di->name));

  /* shouldn't be here if descriptors are unreclaimed */
  ASSERT (di->txin == di->txout);
  ASSERT (di->rxin == di->rxout);

  /* free dma descriptor rings */
  if (DMA64_ENAB (di))
    {
      if (di->txd64)
	DMA_FREE_CONSISTENT (di->osh,
			     ((int8 *) (uintptr) di->txd64 - di->txdalign),
			     di->txdalloc, (di->txdpa - di->txdalign),
			     &di->tx_dmah);
      if (di->rxd64)
	DMA_FREE_CONSISTENT (di->osh,
			     ((int8 *) (uintptr) di->rxd64 - di->rxdalign),
			     di->rxdalloc, (di->rxdpa - di->rxdalign),
			     &di->rx_dmah);
    }
  else
    {
      if (di->txd32)
	DMA_FREE_CONSISTENT (di->osh,
			     ((int8 *) (uintptr) di->txd32 - di->txdalign),
			     di->txdalloc, (di->txdpa - di->txdalign),
			     &di->tx_dmah);
      if (di->rxd32)
	DMA_FREE_CONSISTENT (di->osh,
			     ((int8 *) (uintptr) di->rxd32 - di->rxdalign),
			     di->rxdalloc, (di->rxdpa - di->rxdalign),
			     &di->rx_dmah);
    }

  /* free packet pointer vectors */
  if (di->txp)
    MFREE (di->osh, (void *) di->txp, (di->ntxd * sizeof (void *)));
  if (di->rxp)
    MFREE (di->osh, (void *) di->rxp, (di->nrxd * sizeof (void *)));

  /* free tx packet DMA handles */
  if (di->txp_dmah)
    MFREE (di->osh, (void *) di->txp_dmah, di->ntxd * sizeof (osldma_t **));

  /* free rx packet DMA handles */
  if (di->rxp_dmah)
    MFREE (di->osh, (void *) di->rxp_dmah, di->nrxd * sizeof (osldma_t **));

  /* free our private info structure */
  MFREE (di->osh, (void *) di, sizeof (dma_info_t));

}

/* return TRUE if this dma engine supports DmaExtendedAddrChanges, otherwise FALSE */
static bool
_dma_isaddrext (dma_info_t * di)
{
  if (DMA64_ENAB (di))
    {
      /* DMA64 supports full 32 bits or 64 bits. AE is always valid */

      /* not all tx or rx channel are available */
      if (di->d64txregs != NULL)
	{
	  if (!_dma64_addrext (di->osh, di->d64txregs))
	    {
	      DMA_ERROR (("%s: _dma_isaddrext: DMA64 tx doesn't have AE set\n", di->name));
	      ASSERT (0);
	    }
	  return TRUE;
	}
      else if (di->d64rxregs != NULL)
	{
	  if (!_dma64_addrext (di->osh, di->d64rxregs))
	    {
	      DMA_ERROR (("%s: _dma_isaddrext: DMA64 rx doesn't have AE set\n", di->name));
	      ASSERT (0);
	    }
	  return TRUE;
	}
      return FALSE;
    }
  else if (di->d32txregs)
    return (_dma32_addrext (di->osh, di->d32txregs));
  else if (di->d32rxregs)
    return (_dma32_addrext (di->osh, di->d32rxregs));
  return FALSE;
}

/* initialize descriptor table base address */
static void
_dma_ddtable_init (dma_info_t * di, uint direction, ulong pa)
{
  if (DMA64_ENAB (di))
    {

      if ((di->ddoffsetlow != SB_PCI_DMA) || !(pa & PCI32ADDR_HIGH))
	{
	  if (direction == DMA_TX)
	    {
	      W_REG (di->osh, &di->d64txregs->addrlow,
		     (pa + di->ddoffsetlow));
	      W_REG (di->osh, &di->d64txregs->addrhigh, di->ddoffsethigh);
	    }
	  else
	    {
	      W_REG (di->osh, &di->d64rxregs->addrlow,
		     (pa + di->ddoffsetlow));
	      W_REG (di->osh, &di->d64rxregs->addrhigh, di->ddoffsethigh);
	    }
	}
      else
	{
	  /* DMA64 32bits address extension */
	  uint32 ae;
	  ASSERT (di->addrext);

	  /* shift the high bit(s) from pa to ae */
	  ae = (pa & PCI32ADDR_HIGH) >> PCI32ADDR_HIGH_SHIFT;
	  pa &= ~PCI32ADDR_HIGH;

	  if (direction == DMA_TX)
	    {
	      W_REG (di->osh, &di->d64txregs->addrlow,
		     (pa + di->ddoffsetlow));
	      W_REG (di->osh, &di->d64txregs->addrhigh, di->ddoffsethigh);
	      SET_REG (di->osh, &di->d64txregs->control, D64_XC_AE,
		       (ae << D64_XC_AE_SHIFT));
	    }
	  else
	    {
	      W_REG (di->osh, &di->d64rxregs->addrlow,
		     (pa + di->ddoffsetlow));
	      W_REG (di->osh, &di->d64rxregs->addrhigh, di->ddoffsethigh);
	      SET_REG (di->osh, &di->d64rxregs->control, D64_RC_AE,
		       (ae << D64_RC_AE_SHIFT));
	    }
	}

    }
  else
    {
      if ((di->ddoffsetlow != SB_PCI_DMA) || !(pa & PCI32ADDR_HIGH))
	{
	  if (direction == DMA_TX)
	    W_REG (di->osh, &di->d32txregs->addr, (pa + di->ddoffsetlow));
	  else
	    W_REG (di->osh, &di->d32rxregs->addr, (pa + di->ddoffsetlow));
	}
      else
	{
	  /* dma32 address extension */
	  uint32 ae;
	  ASSERT (di->addrext);

	  /* shift the high bit(s) from pa to ae */
	  ae = (pa & PCI32ADDR_HIGH) >> PCI32ADDR_HIGH_SHIFT;
	  pa &= ~PCI32ADDR_HIGH;

	  if (direction == DMA_TX)
	    {
	      W_REG (di->osh, &di->d32txregs->addr, (pa + di->ddoffsetlow));
	      SET_REG (di->osh, &di->d32txregs->control, XC_AE,
		       ae << XC_AE_SHIFT);
	    }
	  else
	    {
	      W_REG (di->osh, &di->d32rxregs->addr, (pa + di->ddoffsetlow));
	      SET_REG (di->osh, &di->d32rxregs->control, RC_AE,
		       ae << RC_AE_SHIFT);
	    }
	}
    }
}

static void
_dma_fifoloopbackenable (dma_info_t * di)
{
  DMA_TRACE (("%s: dma_fifoloopbackenable\n", di->name));
  if (DMA64_ENAB (di))
    OR_REG (di->osh, &di->d64txregs->control, D64_XC_LE);
  else
    OR_REG (di->osh, &di->d32txregs->control, XC_LE);
}

static void
_dma_rxinit (dma_info_t * di)
{
  DMA_TRACE (("%s: dma_rxinit\n", di->name));

  if (di->nrxd == 0)
    return;

  di->rxin = di->rxout = 0;

  /* clear rx descriptor ring */
  if (DMA64_ENAB (di))
    BZERO_SM ((void *) (uintptr) di->rxd64, (di->nrxd * sizeof (dma64dd_t)));
  else
    BZERO_SM ((void *) (uintptr) di->rxd32, (di->nrxd * sizeof (dma32dd_t)));

  _dma_rxenable (di);
  _dma_ddtable_init (di, DMA_RX, di->rxdpa);
}

static void
_dma_rxenable (dma_info_t * di)
{
  DMA_TRACE (("%s: dma_rxenable\n", di->name));

  if (DMA64_ENAB (di))
    W_REG (di->osh, &di->d64rxregs->control,
	   ((di->rxoffset << D64_RC_RO_SHIFT) | D64_RC_RE));
  else
    W_REG (di->osh, &di->d32rxregs->control,
	   ((di->rxoffset << RC_RO_SHIFT) | RC_RE));
}

/* !! rx entry routine, returns a pointer to the next frame received,
 * or NULL if there are no more
 */
static void *
_dma_rx (dma_info_t * di)
{
  void *p;
  uint len;
  int skiplen = 0;

  while ((p = _dma_getnextrxp (di, FALSE)))
    {
      /* skip giant packets which span multiple rx descriptors */
      if (skiplen > 0)
	{
	  skiplen -= di->rxbufsize;
	  if (skiplen < 0)
	    skiplen = 0;
	  PKTFREE (di->osh, p, FALSE);
	  continue;
	}

      len = ltoh16 (*(uint16 *) (PKTDATA (di->osh, p)));
      DMA_TRACE (("%s: dma_rx len %d\n", di->name, len));

      /* bad frame length check */
      if (len > (di->rxbufsize - di->rxoffset))
	{
	  DMA_ERROR (("%s: dma_rx: bad frame length (%d)\n", di->name, len));
	  if (len > 0)
	    skiplen = len - (di->rxbufsize - di->rxoffset);
	  PKTFREE (di->osh, p, FALSE);
	  di->hnddma.rxgiants++;
	  continue;
	}

      /* set actual length */
      PKTSETLEN (di->osh, p, (di->rxoffset + len));

      break;
    }

  return (p);
}

/* post receive buffers */
static void
_dma_rxfill (dma_info_t * di)
{
  void *p;
  uint rxin, rxout;
  uint32 flags = 0;
  uint n;
  uint i;
  uint32 pa;
  uint extra_offset = 0;

  /*
   * Determine how many receive buffers we're lacking
   * from the full complement, allocate, initialize,
   * and post them, then update the chip rx lastdscr.
   */

  rxin = di->rxin;
  rxout = di->rxout;

  n = di->nrxpost - NRXDACTIVE (rxin, rxout);

  DMA_TRACE (("%s: dma_rxfill: post %d\n", di->name, n));

  if (di->rxbufsize > BCMEXTRAHDROOM)
    extra_offset = BCMEXTRAHDROOM;

  for (i = 0; i < n; i++)
    {
      /* the di->rxbufsize doesn't include the extra headroom, we need to add it to the
         size to be allocated
       */
      if ((p = PKTGET (di->osh, di->rxbufsize + extra_offset, FALSE)) == NULL)
	{
	  DMA_ERROR (("%s: dma_rxfill: out of rxbufs\n", di->name));
	  di->hnddma.rxnobuf++;
	  break;
	}
      /* reserve an extra headroom, if applicable */
      if (extra_offset)
	PKTPULL (di->osh, p, extra_offset);

      /* Do a cached write instead of uncached write since DMA_MAP
       * will flush the cache.
       */
      *(uint32 *) (PKTDATA (di->osh, p)) = 0;

      pa = (uint32) DMA_MAP (di->osh, PKTDATA (di->osh, p),
			     di->rxbufsize, DMA_RX, p, &di->rxp_dmah[rxout]);

      ASSERT (ISALIGNED (pa, 4));

      /* save the free packet pointer */
      ASSERT (di->rxp[rxout] == NULL);
      di->rxp[rxout] = p;

      /* reset flags for each descriptor */
      flags = 0;
      if (DMA64_ENAB (di))
	{
	  if (rxout == (di->nrxd - 1))
	    flags = D64_CTRL1_EOT;

	  dma64_dd_upd (di, di->rxd64, pa, rxout, &flags, di->rxbufsize);
	}
      else
	{
	  if (rxout == (di->nrxd - 1))
	    flags = CTRL_EOT;

	  dma32_dd_upd (di, di->rxd32, pa, rxout, &flags, di->rxbufsize);
	}
      rxout = NEXTRXD (rxout);
    }

  di->rxout = rxout;

  /* update the chip lastdscr pointer */
  if (DMA64_ENAB (di))
    {
      W_REG (di->osh, &di->d64rxregs->ptr, I2B (rxout, dma64dd_t));
    }
  else
    {
      W_REG (di->osh, &di->d32rxregs->ptr, I2B (rxout, dma32dd_t));
    }
}

/* like getnexttxp but no reclaim */
static void *
_dma_peeknexttxp (dma_info_t * di)
{
  uint end, i;

  if (di->ntxd == 0)
    return (NULL);

  if (DMA64_ENAB (di))
    {
      end =
	B2I (R_REG (di->osh, &di->d64txregs->status0) & D64_XS0_CD_MASK,
	     dma64dd_t);
    }
  else
    {
      end =
	B2I (R_REG (di->osh, &di->d32txregs->status) & XS_CD_MASK, dma32dd_t);
    }

  for (i = di->txin; i != end; i = NEXTTXD (i))
    if (di->txp[i])
      return (di->txp[i]);

  return (NULL);
}

static void
_dma_rxreclaim (dma_info_t * di)
{
  void *p;

  /* "unused local" warning suppression for OSLs that
   * define PKTFREE() without using the di->osh arg
   */
  di = di;

  DMA_TRACE (("%s: dma_rxreclaim\n", di->name));

  while ((p = _dma_getnextrxp (di, TRUE)))
    PKTFREE (di->osh, p, FALSE);
}

static void *
_dma_getnextrxp (dma_info_t * di, bool forceall)
{
  if (di->nrxd == 0)
    return (NULL);

  if (DMA64_ENAB (di))
    {
      return dma64_getnextrxp (di, forceall);
    }
  else
    {
      return dma32_getnextrxp (di, forceall);
    }
}

static void
_dma_txblock (dma_info_t * di)
{
  di->hnddma.txavail = 0;
}

static void
_dma_txunblock (dma_info_t * di)
{
  di->hnddma.txavail = di->ntxd - NTXDACTIVE (di->txin, di->txout) - 1;
}

static uint
_dma_txactive (dma_info_t * di)
{
  return (NTXDACTIVE (di->txin, di->txout));
}

static void
_dma_counterreset (dma_info_t * di)
{
  /* reset all software counter */
  di->hnddma.rxgiants = 0;
  di->hnddma.rxnobuf = 0;
  di->hnddma.txnobuf = 0;
}

/* get the address of the var in order to change later */
static uintptr
_dma_getvar (dma_info_t * di, const char *name)
{
  if (!strcmp (name, "&txavail"))
    return ((uintptr) & (di->hnddma.txavail));
  else
    {
      ASSERT (0);
    }
  return (0);
}

void
dma_txpioloopback (osl_t * osh, dma32regs_t * regs)
{
  OR_REG (osh, &regs->control, XC_LE);
}

#ifdef BCMDBG
static void
dma32_dumpring (dma_info_t * di, struct bcmstrbuf *b, dma32dd_t * ring,
		uint start, uint end, uint max_num)
{
  uint i;

  for (i = start; i != end; i = XXD ((i + 1), max_num))
    {
      /* in the format of high->low 8 bytes */
      bcm_bprintf (b, "ring index %d: 0x%x %x\n", i, ring[i].addr,
		   ring[i].ctrl);
    }
}

static void
dma32_dumptx (dma_info_t * di, struct bcmstrbuf *b, bool dumpring)
{
  if (di->ntxd == 0)
    return;

  bcm_bprintf (b, "DMA32: txd32 %p txdpa 0x%lx txp %p txin %d txout %d "
	       "txavail %d\n", di->txd32, di->txdpa, di->txp, di->txin,
	       di->txout, di->hnddma.txavail);

  bcm_bprintf (b, "xmtcontrol 0x%x xmtaddr 0x%x xmtptr 0x%x xmtstatus 0x%x\n",
	       R_REG (di->osh, &di->d32txregs->control),
	       R_REG (di->osh, &di->d32txregs->addr),
	       R_REG (di->osh, &di->d32txregs->ptr),
	       R_REG (di->osh, &di->d32txregs->status));

  if (dumpring && di->txd32)
    dma32_dumpring (di, b, di->txd32, di->txin, di->txout, di->ntxd);
}

static void
dma32_dumprx (dma_info_t * di, struct bcmstrbuf *b, bool dumpring)
{
  if (di->nrxd == 0)
    return;

  bcm_bprintf (b, "DMA32: rxd32 %p rxdpa 0x%lx rxp %p rxin %d rxout %d\n",
	       di->rxd32, di->rxdpa, di->rxp, di->rxin, di->rxout);

  bcm_bprintf (b, "rcvcontrol 0x%x rcvaddr 0x%x rcvptr 0x%x rcvstatus 0x%x\n",
	       R_REG (di->osh, &di->d32rxregs->control),
	       R_REG (di->osh, &di->d32rxregs->addr),
	       R_REG (di->osh, &di->d32rxregs->ptr),
	       R_REG (di->osh, &di->d32rxregs->status));
  if (di->rxd32 && dumpring)
    dma32_dumpring (di, b, di->rxd32, di->rxin, di->rxout, di->nrxd);
}

static void
dma32_dump (dma_info_t * di, struct bcmstrbuf *b, bool dumpring)
{
  dma32_dumptx (di, b, dumpring);
  dma32_dumprx (di, b, dumpring);
}

static void
dma64_dumpring (dma_info_t * di, struct bcmstrbuf *b, dma64dd_t * ring,
		uint start, uint end, uint max_num)
{
  uint i;

  for (i = start; i != end; i = XXD ((i + 1), max_num))
    {
      /* in the format of high->low 16 bytes */
      bcm_bprintf (b, "ring index %d: 0x%x %x %x %x\n",
		   i, ring[i].addrhigh, ring[i].addrlow, ring[i].ctrl2,
		   ring[i].ctrl1);
    }
}

static void
dma64_dumptx (dma_info_t * di, struct bcmstrbuf *b, bool dumpring)
{
  if (di->ntxd == 0)
    return;

  bcm_bprintf (b, "DMA64: txd64 %p txdpa 0x%lx txp %p txin %d txout %d "
	       "txavail %d\n", di->txd64, di->txdpa, di->txp, di->txin,
	       di->txout, di->hnddma.txavail);

  bcm_bprintf (b, "xmtcontrol 0x%x xmtaddrlow 0x%x xmtaddrhigh 0x%x "
	       "xmtptr 0x%x xmtstatus0 0x%x xmtstatus1 0x%x\n",
	       R_REG (di->osh, &di->d64txregs->control),
	       R_REG (di->osh, &di->d64txregs->addrlow),
	       R_REG (di->osh, &di->d64txregs->addrhigh),
	       R_REG (di->osh, &di->d64txregs->ptr),
	       R_REG (di->osh, &di->d64txregs->status0),
	       R_REG (di->osh, &di->d64txregs->status1));

  if (dumpring && di->txd64)
    {
      dma64_dumpring (di, b, di->txd64, di->txin, di->txout, di->ntxd);
    }
}

static void
dma64_dumprx (dma_info_t * di, struct bcmstrbuf *b, bool dumpring)
{
  if (di->nrxd == 0)
    return;

  bcm_bprintf (b, "DMA64: rxd64 %p rxdpa 0x%lx rxp %p rxin %d rxout %d\n",
	       di->rxd64, di->rxdpa, di->rxp, di->rxin, di->rxout);

  bcm_bprintf (b, "rcvcontrol 0x%x rcvaddrlow 0x%x rcvaddrhigh 0x%x rcvptr "
	       "0x%x rcvstatus0 0x%x rcvstatus1 0x%x\n",
	       R_REG (di->osh, &di->d64rxregs->control),
	       R_REG (di->osh, &di->d64rxregs->addrlow),
	       R_REG (di->osh, &di->d64rxregs->addrhigh),
	       R_REG (di->osh, &di->d64rxregs->ptr),
	       R_REG (di->osh, &di->d64rxregs->status0),
	       R_REG (di->osh, &di->d64rxregs->status1));
  if (di->rxd64 && dumpring)
    {
      dma64_dumpring (di, b, di->rxd64, di->rxin, di->rxout, di->nrxd);
    }
}

static void
dma64_dump (dma_info_t * di, struct bcmstrbuf *b, bool dumpring)
{
  dma64_dumptx (di, b, dumpring);
  dma64_dumprx (di, b, dumpring);
}

#endif /* BCMDBG */


/* 32 bits DMA functions */
static void
dma32_txinit (dma_info_t * di)
{
  DMA_TRACE (("%s: dma_txinit\n", di->name));

  if (di->ntxd == 0)
    return;

  di->txin = di->txout = 0;
  di->hnddma.txavail = di->ntxd - 1;

  /* clear tx descriptor ring */
  BZERO_SM ((void *) (uintptr) di->txd32, (di->ntxd * sizeof (dma32dd_t)));
  W_REG (di->osh, &di->d32txregs->control, XC_XE);
  _dma_ddtable_init (di, DMA_TX, di->txdpa);
}

static bool
dma32_txenabled (dma_info_t * di)
{
  uint32 xc;

  /* If the chip is dead, it is not enabled :-) */
  xc = R_REG (di->osh, &di->d32txregs->control);
  return ((xc != 0xffffffff) && (xc & XC_XE));
}

static void
dma32_txsuspend (dma_info_t * di)
{
  DMA_TRACE (("%s: dma_txsuspend\n", di->name));

  if (di->ntxd == 0)
    return;

  OR_REG (di->osh, &di->d32txregs->control, XC_SE);
}

static void
dma32_txresume (dma_info_t * di)
{
  DMA_TRACE (("%s: dma_txresume\n", di->name));

  if (di->ntxd == 0)
    return;

  AND_REG (di->osh, &di->d32txregs->control, ~XC_SE);
}

static bool
dma32_txsuspended (dma_info_t * di)
{
  return (di->ntxd == 0)
    || ((R_REG (di->osh, &di->d32txregs->control) & XC_SE) == XC_SE);
}

static void
dma32_txreclaim (dma_info_t * di, bool forceall)
{
  void *p;

  DMA_TRACE (("%s: dma_txreclaim %s\n", di->name, forceall ? "all" : ""));

  while ((p = dma32_getnexttxp (di, forceall)))
    PKTFREE (di->osh, p, TRUE);
}

static bool
dma32_txstopped (dma_info_t * di)
{
  return ((R_REG (di->osh, &di->d32txregs->status) & XS_XS_MASK) ==
	  XS_XS_STOPPED);
}

static bool
dma32_rxstopped (dma_info_t * di)
{
  return ((R_REG (di->osh, &di->d32rxregs->status) & RS_RS_MASK) ==
	  RS_RS_STOPPED);
}

static bool
dma32_alloc (dma_info_t * di, uint direction)
{
  uint size;
  uint ddlen;
  void *va;

  ddlen = sizeof (dma32dd_t);

  size = (direction == DMA_TX) ? (di->ntxd * ddlen) : (di->nrxd * ddlen);

  if (!ISALIGNED (DMA_CONSISTENT_ALIGN, D32RINGALIGN))
    size += D32RINGALIGN;


  if (direction == DMA_TX)
    {
      if ((va =
	   DMA_ALLOC_CONSISTENT (di->osh, size, &di->txdpa,
				 &di->tx_dmah)) == NULL)
	{
	  DMA_ERROR (("%s: dma_attach: DMA_ALLOC_CONSISTENT(ntxd) failed\n",
		      di->name));
	  return FALSE;
	}

      di->txd32 = (dma32dd_t *) ROUNDUP ((uintptr) va, D32RINGALIGN);
      di->txdalign = (uint) ((int8 *) (uintptr) di->txd32 - (int8 *) va);
      di->txdpa += di->txdalign;
      di->txdalloc = size;
      ASSERT (ISALIGNED ((uintptr) di->txd32, D32RINGALIGN));
    }
  else
    {
      if ((va =
	   DMA_ALLOC_CONSISTENT (di->osh, size, &di->rxdpa,
				 &di->rx_dmah)) == NULL)
	{
	  DMA_ERROR (("%s: dma_attach: DMA_ALLOC_CONSISTENT(nrxd) failed\n",
		      di->name));
	  return FALSE;
	}
      di->rxd32 = (dma32dd_t *) ROUNDUP ((uintptr) va, D32RINGALIGN);
      di->rxdalign = (uint) ((int8 *) (uintptr) di->rxd32 - (int8 *) va);
      di->rxdpa += di->rxdalign;
      di->rxdalloc = size;
      ASSERT (ISALIGNED ((uintptr) di->rxd32, D32RINGALIGN));
    }

  return TRUE;
}

static bool
dma32_txreset (dma_info_t * di)
{
  uint32 status;

  if (di->ntxd == 0)
    return TRUE;

  /* suspend tx DMA first */
  W_REG (di->osh, &di->d32txregs->control, XC_SE);
  SPINWAIT (((status = (R_REG (di->osh, &di->d32txregs->status) & XS_XS_MASK))
	     != XS_XS_DISABLED) &&
	    (status != XS_XS_IDLE) && (status != XS_XS_STOPPED), (10000));

  W_REG (di->osh, &di->d32txregs->control, 0);
  SPINWAIT (((status = (R_REG (di->osh,
			       &di->d32txregs->status) & XS_XS_MASK)) !=
	     XS_XS_DISABLED), 10000);

  /* wait for the last transaction to complete */
  OSL_DELAY (300);

  return (status == XS_XS_DISABLED);
}

static bool
dma32_rxidle (dma_info_t * di)
{
  DMA_TRACE (("%s: dma_rxidle\n", di->name));

  if (di->nrxd == 0)
    return TRUE;

  return ((R_REG (di->osh, &di->d32rxregs->status) & RS_CD_MASK) ==
	  R_REG (di->osh, &di->d32rxregs->ptr));
}

static bool
dma32_rxreset (dma_info_t * di)
{
  uint32 status;

  if (di->nrxd == 0)
    return TRUE;

  W_REG (di->osh, &di->d32rxregs->control, 0);
  SPINWAIT (((status = (R_REG (di->osh,
			       &di->d32rxregs->status) & RS_RS_MASK)) !=
	     RS_RS_DISABLED), 10000);

  return (status == RS_RS_DISABLED);
}

static bool
dma32_rxenabled (dma_info_t * di)
{
  uint32 rc;

  rc = R_REG (di->osh, &di->d32rxregs->control);
  return ((rc != 0xffffffff) && (rc & RC_RE));
}

static bool
dma32_txsuspendedidle (dma_info_t * di)
{
  if (di->ntxd == 0)
    return TRUE;

  if (!(R_REG (di->osh, &di->d32txregs->control) & XC_SE))
    return 0;

  if ((R_REG (di->osh, &di->d32txregs->status) & XS_XS_MASK) != XS_XS_IDLE)
    return 0;

  OSL_DELAY (2);
  return ((R_REG (di->osh, &di->d32txregs->status) & XS_XS_MASK) ==
	  XS_XS_IDLE);
}

/* !! tx entry routine
 * supports full 32bit dma engine buffer addressing so
 * dma buffers can cross 4 Kbyte page boundaries.
 */
static int
dma32_txfast (dma_info_t * di, void *p0, bool commit)
{
  void *p, *next;
  uchar *data;
  uint len;
  uint txout;
  uint32 flags = 0;
  uint32 pa;

  DMA_TRACE (("%s: dma_txfast\n", di->name));

  txout = di->txout;

  /*
   * Walk the chain of packet buffers
   * allocating and initializing transmit descriptor entries.
   */
  for (p = p0; p; p = next)
    {
      data = PKTDATA (di->osh, p);
      len = PKTLEN (di->osh, p);
      next = PKTNEXT (di->osh, p);

      /* return nonzero if out of tx descriptors */
      if (NEXTTXD (txout) == di->txin)
	goto outoftxd;

      if (len == 0)
	continue;

      /* get physical address of buffer start */
      pa =
	(uint32) DMA_MAP (di->osh, data, len, DMA_TX, p,
			  &di->txp_dmah[txout]);

      flags = 0;
      if (p == p0)
	flags |= CTRL_SOF;
      if (next == NULL)
	flags |= (CTRL_IOC | CTRL_EOF);
      if (txout == (di->ntxd - 1))
	flags |= CTRL_EOT;

      dma32_dd_upd (di, di->txd32, pa, txout, &flags, len);
      ASSERT (di->txp[txout] == NULL);

      txout = NEXTTXD (txout);
    }

  /* if last txd eof not set, fix it */
  if (!(flags & CTRL_EOF))
    W_SM (&di->txd32[PREVTXD (txout)].ctrl,
	  BUS_SWAP32 (flags | CTRL_IOC | CTRL_EOF));

  /* save the packet */
  di->txp[PREVTXD (txout)] = p0;

  /* bump the tx descriptor index */
  di->txout = txout;

  /* kick the chip */
  if (commit)
    W_REG (di->osh, &di->d32txregs->ptr, I2B (txout, dma32dd_t));

  /* tx flow control */
  di->hnddma.txavail = di->ntxd - NTXDACTIVE (di->txin, di->txout) - 1;

  return (0);

outoftxd:
  DMA_ERROR (("%s: dma_txfast: out of txds\n", di->name));
  PKTFREE (di->osh, p0, TRUE);
  di->hnddma.txavail = 0;
  di->hnddma.txnobuf++;
  return (-1);
}

/*
 * Reclaim next completed txd (txds if using chained buffers) and
 * return associated packet.
 * If 'force' is true, reclaim txd(s) and return associated packet
 * regardless of the value of the hardware "curr" pointer.
 */
static void *
dma32_getnexttxp (dma_info_t * di, bool forceall)
{
  uint start, end, i;
  void *txp;

  DMA_TRACE (("%s: dma_getnexttxp %s\n", di->name, forceall ? "all" : ""));

  if (di->ntxd == 0)
    return (NULL);

  txp = NULL;

  start = di->txin;
  if (forceall)
    end = di->txout;
  else
    end =
      B2I (R_REG (di->osh, &di->d32txregs->status) & XS_CD_MASK, dma32dd_t);

  if ((start == 0) && (end > di->txout))
    goto bogus;

  for (i = start; i != end && !txp; i = NEXTTXD (i))
    {
      DMA_UNMAP (di->osh,
		 (BUS_SWAP32 (R_SM (&di->txd32[i].addr)) - di->dataoffsetlow),
		 (BUS_SWAP32 (R_SM (&di->txd32[i].ctrl)) & CTRL_BC_MASK),
		 DMA_TX, di->txp[i], &di->txp_dmah[i]);

      W_SM (&di->txd32[i].addr, 0xdeadbeef);
      txp = di->txp[i];
      di->txp[i] = NULL;
    }

  di->txin = i;

  /* tx flow control */
  di->hnddma.txavail = di->ntxd - NTXDACTIVE (di->txin, di->txout) - 1;

  return (txp);

bogus:
/*
	DMA_ERROR(("dma_getnexttxp: bogus curr: start %d end %d txout %d force %d\n",
		start, end, di->txout, forceall));
*/
  return (NULL);
}

static void *
dma32_getnextrxp (dma_info_t * di, bool forceall)
{
  uint i;
  void *rxp;

  /* if forcing, dma engine must be disabled */
  ASSERT (!forceall || !dma32_rxenabled (di));

  i = di->rxin;

  /* return if no packets posted */
  if (i == di->rxout)
    return (NULL);

  /* ignore curr if forceall */
  if (!forceall
      && (i ==
	  B2I (R_REG (di->osh, &di->d32rxregs->status) & RS_CD_MASK,
	       dma32dd_t)))
    return (NULL);

  /* get the packet pointer that corresponds to the rx descriptor */
  rxp = di->rxp[i];
  ASSERT (rxp);
  di->rxp[i] = NULL;

  /* clear this packet from the descriptor ring */
  DMA_UNMAP (di->osh,
	     (BUS_SWAP32 (R_SM (&di->rxd32[i].addr)) - di->dataoffsetlow),
	     di->rxbufsize, DMA_RX, rxp, &di->rxp_dmah[i]);

  W_SM (&di->rxd32[i].addr, 0xdeadbeef);

  di->rxin = NEXTRXD (i);

  return (rxp);
}

/*
 * Rotate all active tx dma ring entries "forward" by (ActiveDescriptor - txin).
 */
static void
dma32_txrotate (dma_info_t * di)
{
  uint ad;
  uint nactive;
  uint rot;
  uint old, new;
  uint32 w;
  uint first, last;

  ASSERT (dma32_txsuspendedidle (di));

  nactive = _dma_txactive (di);
  ad =
    B2I (((R_REG (di->osh, &di->d32txregs->status) & XS_AD_MASK) >>
	  XS_AD_SHIFT), dma32dd_t);
  rot = TXD (ad - di->txin);

  ASSERT (rot < di->ntxd);

  /* full-ring case is a lot harder - don't worry about this */
  if (rot >= (di->ntxd - nactive))
    {
      DMA_ERROR (("%s: dma_txrotate: ring full - punt\n", di->name));
      return;
    }

  first = di->txin;
  last = PREVTXD (di->txout);

  /* move entries starting at last and moving backwards to first */
  for (old = last; old != PREVTXD (first); old = PREVTXD (old))
    {
      new = TXD (old + rot);

      /*
       * Move the tx dma descriptor.
       * EOT is set only in the last entry in the ring.
       */
      w = BUS_SWAP32 (R_SM (&di->txd32[old].ctrl)) & ~CTRL_EOT;
      if (new == (di->ntxd - 1))
	w |= CTRL_EOT;
      W_SM (&di->txd32[new].ctrl, BUS_SWAP32 (w));
      W_SM (&di->txd32[new].addr, R_SM (&di->txd32[old].addr));

      /* zap the old tx dma descriptor address field */
      W_SM (&di->txd32[old].addr, BUS_SWAP32 (0xdeadbeef));

      /* move the corresponding txp[] entry */
      ASSERT (di->txp[new] == NULL);
      di->txp[new] = di->txp[old];
      di->txp[old] = NULL;
    }

  /* update txin and txout */
  di->txin = ad;
  di->txout = TXD (di->txout + rot);
  di->hnddma.txavail = di->ntxd - NTXDACTIVE (di->txin, di->txout) - 1;

  /* kick the chip */
  W_REG (di->osh, &di->d32txregs->ptr, I2B (di->txout, dma32dd_t));
}

/* 64 bits DMA functions */

#ifdef BCMDMA64
static void
dma64_txinit (dma_info_t * di)
{
  DMA_TRACE (("%s: dma_txinit\n", di->name));

  if (di->ntxd == 0)
    return;

  di->txin = di->txout = 0;
  di->hnddma.txavail = di->ntxd - 1;

  /* clear tx descriptor ring */
  BZERO_SM ((void *) (uintptr) di->txd64, (di->ntxd * sizeof (dma64dd_t)));
  W_REG (di->osh, &di->d64txregs->control, D64_XC_XE);
  _dma_ddtable_init (di, DMA_TX, di->txdpa);
}

static bool
dma64_txenabled (dma_info_t * di)
{
  uint32 xc;

  /* If the chip is dead, it is not enabled :-) */
  xc = R_REG (di->osh, &di->d64txregs->control);
  return ((xc != 0xffffffff) && (xc & D64_XC_XE));
}

static void
dma64_txsuspend (dma_info_t * di)
{
  DMA_TRACE (("%s: dma_txsuspend\n", di->name));

  if (di->ntxd == 0)
    return;

  OR_REG (di->osh, &di->d64txregs->control, D64_XC_SE);
}

static void
dma64_txresume (dma_info_t * di)
{
  DMA_TRACE (("%s: dma_txresume\n", di->name));

  if (di->ntxd == 0)
    return;

  AND_REG (di->osh, &di->d64txregs->control, ~D64_XC_SE);
}

static bool
dma64_txsuspended (dma_info_t * di)
{
  return (di->ntxd == 0)
    || ((R_REG (di->osh, &di->d64txregs->control) & D64_XC_SE) == D64_XC_SE);
}

static void
dma64_txreclaim (dma_info_t * di, bool forceall)
{
  void *p;

  DMA_TRACE (("%s: dma_txreclaim %s\n", di->name, forceall ? "all" : ""));

  while ((p = dma64_getnexttxp (di, forceall)))
    PKTFREE (di->osh, p, TRUE);
}

static bool
dma64_txstopped (dma_info_t * di)
{
  return ((R_REG (di->osh, &di->d64txregs->status0) & D64_XS0_XS_MASK) ==
	  D64_XS0_XS_STOPPED);
}

static bool
dma64_rxstopped (dma_info_t * di)
{
  return ((R_REG (di->osh, &di->d64rxregs->status0) & D64_RS0_RS_MASK) ==
	  D64_RS0_RS_STOPPED);
}

static bool
dma64_alloc (dma_info_t * di, uint direction)
{
  uint size;
  uint ddlen;
  uint32 alignbytes;
  void *va;

  ddlen = sizeof (dma64dd_t);

  size = (direction == DMA_TX) ? (di->ntxd * ddlen) : (di->nrxd * ddlen);

  alignbytes = di->dma64align;

  if (!ISALIGNED (DMA_CONSISTENT_ALIGN, alignbytes))
    size += alignbytes;

  if (direction == DMA_TX)
    {
      if ((va =
	   DMA_ALLOC_CONSISTENT (di->osh, size, &di->txdpa,
				 &di->tx_dmah)) == NULL)
	{
	  DMA_ERROR (("%s: dma_attach: DMA_ALLOC_CONSISTENT(ntxd) failed\n",
		      di->name));
	  return FALSE;
	}

      di->txd64 = (dma64dd_t *) ROUNDUP ((uintptr) va, alignbytes);
      di->txdalign = (uint) ((int8 *) (uintptr) di->txd64 - (int8 *) va);
      di->txdpa += di->txdalign;
      di->txdalloc = size;
      ASSERT (ISALIGNED ((uintptr) di->txd64, alignbytes));
    }
  else
    {
      if ((va =
	   DMA_ALLOC_CONSISTENT (di->osh, size, &di->rxdpa,
				 &di->rx_dmah)) == NULL)
	{
	  DMA_ERROR (("%s: dma_attach: DMA_ALLOC_CONSISTENT(nrxd) failed\n",
		      di->name));
	  return FALSE;
	}
      di->rxd64 = (dma64dd_t *) ROUNDUP ((uintptr) va, alignbytes);
      di->rxdalign = (uint) ((int8 *) (uintptr) di->rxd64 - (int8 *) va);
      di->rxdpa += di->rxdalign;
      di->rxdalloc = size;
      ASSERT (ISALIGNED ((uintptr) di->rxd64, alignbytes));
    }

  return TRUE;
}

static bool
dma64_txreset (dma_info_t * di)
{
  uint32 status;

  if (di->ntxd == 0)
    return TRUE;

  /* suspend tx DMA first */
  W_REG (di->osh, &di->d64txregs->control, D64_XC_SE);
  SPINWAIT (((status =
	      (R_REG (di->osh, &di->d64txregs->status0) & D64_XS0_XS_MASK)) !=
	     D64_XS0_XS_DISABLED) && (status != D64_XS0_XS_IDLE)
	    && (status != D64_XS0_XS_STOPPED), 10000);

  W_REG (di->osh, &di->d64txregs->control, 0);
  SPINWAIT (((status =
	      (R_REG (di->osh, &di->d64txregs->status0) & D64_XS0_XS_MASK)) !=
	     D64_XS0_XS_DISABLED), 10000);

  /* wait for the last transaction to complete */
  OSL_DELAY (300);

  return (status == D64_XS0_XS_DISABLED);
}

static bool
dma64_rxidle (dma_info_t * di)
{
  DMA_TRACE (("%s: dma_rxidle\n", di->name));

  if (di->nrxd == 0)
    return TRUE;

  return ((R_REG (di->osh, &di->d64rxregs->status0) & D64_RS0_CD_MASK) ==
	  R_REG (di->osh, &di->d64rxregs->ptr));
}

static bool
dma64_rxreset (dma_info_t * di)
{
  uint32 status;

  if (di->nrxd == 0)
    return TRUE;

  W_REG (di->osh, &di->d64rxregs->control, 0);
  SPINWAIT (((status =
	      (R_REG (di->osh, &di->d64rxregs->status0) & D64_RS0_RS_MASK)) !=
	     D64_RS0_RS_DISABLED), 10000);

  return (status == D64_RS0_RS_DISABLED);
}

static bool
dma64_rxenabled (dma_info_t * di)
{
  uint32 rc;

  rc = R_REG (di->osh, &di->d64rxregs->control);
  return ((rc != 0xffffffff) && (rc & D64_RC_RE));
}

static bool
dma64_txsuspendedidle (dma_info_t * di)
{

  if (di->ntxd == 0)
    return TRUE;

  if (!(R_REG (di->osh, &di->d64txregs->control) & D64_XC_SE))
    return 0;

  if ((R_REG (di->osh, &di->d64txregs->status0) & D64_XS0_XS_MASK) ==
      D64_XS0_XS_IDLE)
    return 1;

  return 0;
}


/* !! tx entry routine */
static int
dma64_txfast (dma_info_t * di, void *p0, bool commit)
{
  void *p, *next;
  uchar *data;
  uint len;
  uint txout;
  uint32 flags = 0;
  uint32 pa;

  DMA_TRACE (("%s: dma_txfast\n", di->name));

  txout = di->txout;

  /*
   * Walk the chain of packet buffers
   * allocating and initializing transmit descriptor entries.
   */
  for (p = p0; p; p = next)
    {
      data = PKTDATA (di->osh, p);
      len = PKTLEN (di->osh, p);
      next = PKTNEXT (di->osh, p);

      /* return nonzero if out of tx descriptors */
      if (NEXTTXD (txout) == di->txin)
	goto outoftxd;

      if (len == 0)
	continue;

      /* get physical address of buffer start */
      pa =
	(uint32) DMA_MAP (di->osh, data, len, DMA_TX, p,
			  &di->txp_dmah[txout]);

      flags = 0;
      if (p == p0)
	flags |= D64_CTRL1_SOF;
      if (next == NULL)
	flags |= (D64_CTRL1_IOC | D64_CTRL1_EOF);
      if (txout == (di->ntxd - 1))
	flags |= D64_CTRL1_EOT;

      dma64_dd_upd (di, di->txd64, pa, txout, &flags, len);
      ASSERT (di->txp[txout] == NULL);

      txout = NEXTTXD (txout);
    }

  /* if last txd eof not set, fix it */
  if (!(flags & D64_CTRL1_EOF))
    W_SM (&di->txd64[PREVTXD (txout)].ctrl1,
	  BUS_SWAP32 (flags | D64_CTRL1_IOC | D64_CTRL1_EOF));

  /* save the packet */
  di->txp[PREVTXD (txout)] = p0;

  /* bump the tx descriptor index */
  di->txout = txout;

  /* kick the chip */
  if (commit)
    W_REG (di->osh, &di->d64txregs->ptr, I2B (txout, dma64dd_t));

  /* tx flow control */
  di->hnddma.txavail = di->ntxd - NTXDACTIVE (di->txin, di->txout) - 1;

  return (0);

outoftxd:
  DMA_ERROR (("%s: dma_txfast: out of txds\n", di->name));
  PKTFREE (di->osh, p0, TRUE);
  di->hnddma.txavail = 0;
  di->hnddma.txnobuf++;
  return (-1);
}

/*
 * Reclaim next completed txd (txds if using chained buffers) and
 * return associated packet.
 * If 'force' is true, reclaim txd(s) and return associated packet
 * regardless of the value of the hardware "curr" pointer.
 */
static void *
dma64_getnexttxp (dma_info_t * di, bool forceall)
{
  uint start, end, i;
  void *txp;

  DMA_TRACE (("%s: dma_getnexttxp %s\n", di->name, forceall ? "all" : ""));

  if (di->ntxd == 0)
    return (NULL);

  txp = NULL;

  start = di->txin;
  if (forceall)
    end = di->txout;
  else
    end =
      B2I (R_REG (di->osh, &di->d64txregs->status0) & D64_XS0_CD_MASK,
	   dma64dd_t);

  if ((start == 0) && (end > di->txout))
    goto bogus;

  for (i = start; i != end && !txp; i = NEXTTXD (i))
    {
      DMA_UNMAP (di->osh,
		 (BUS_SWAP32 (R_SM (&di->txd64[i].addrlow)) -
		  di->dataoffsetlow),
		 (BUS_SWAP32 (R_SM (&di->txd64[i].ctrl2)) &
		  D64_CTRL2_BC_MASK), DMA_TX, di->txp[i], &di->txp_dmah[i]);

      W_SM (&di->txd64[i].addrlow, 0xdeadbeef);
      W_SM (&di->txd64[i].addrhigh, 0xdeadbeef);

      txp = di->txp[i];
      di->txp[i] = NULL;
    }

  di->txin = i;

  /* tx flow control */
  di->hnddma.txavail = di->ntxd - NTXDACTIVE (di->txin, di->txout) - 1;

  return (txp);

bogus:
/*
	DMA_ERROR(("dma_getnexttxp: bogus curr: start %d end %d txout %d force %d\n",
		start, end, di->txout, forceall));
*/
  return (NULL);
}

static void *
dma64_getnextrxp (dma_info_t * di, bool forceall)
{
  uint i;
  void *rxp;

  /* if forcing, dma engine must be disabled */
  ASSERT (!forceall || !dma64_rxenabled (di));

  i = di->rxin;

  /* return if no packets posted */
  if (i == di->rxout)
    return (NULL);

  /* ignore curr if forceall */
  if (!forceall &&
      (i ==
       B2I (R_REG (di->osh, &di->d64rxregs->status0) & D64_RS0_CD_MASK,
	    dma64dd_t)))
    return (NULL);

  /* get the packet pointer that corresponds to the rx descriptor */
  rxp = di->rxp[i];
  ASSERT (rxp);
  di->rxp[i] = NULL;

  /* clear this packet from the descriptor ring */
  DMA_UNMAP (di->osh,
	     (BUS_SWAP32 (R_SM (&di->rxd64[i].addrlow)) - di->dataoffsetlow),
	     di->rxbufsize, DMA_RX, rxp, &di->rxp_dmah[i]);

  W_SM (&di->rxd64[i].addrlow, 0xdeadbeef);
  W_SM (&di->rxd64[i].addrhigh, 0xdeadbeef);

  di->rxin = NEXTRXD (i);

  return (rxp);
}

static bool
_dma64_addrext (osl_t * osh, dma64regs_t * dma64regs)
{
  uint32 w;
  OR_REG (osh, &dma64regs->control, D64_XC_AE);
  w = R_REG (osh, &dma64regs->control);
  AND_REG (osh, &dma64regs->control, ~D64_XC_AE);
  return ((w & D64_XC_AE) == D64_XC_AE);
}

/*
 * Rotate all active tx dma ring entries "forward" by (ActiveDescriptor - txin).
 */
static void
dma64_txrotate (dma_info_t * di)
{
  uint ad;
  uint nactive;
  uint rot;
  uint old, new;
  uint32 w;
  uint first, last;

  ASSERT (dma64_txsuspendedidle (di));

  nactive = _dma_txactive (di);
  ad =
    B2I ((R_REG (di->osh, &di->d64txregs->status1) & D64_XS1_AD_MASK),
	 dma64dd_t);
  rot = TXD (ad - di->txin);

  ASSERT (rot < di->ntxd);

  /* full-ring case is a lot harder - don't worry about this */
  if (rot >= (di->ntxd - nactive))
    {
      DMA_ERROR (("%s: dma_txrotate: ring full - punt\n", di->name));
      return;
    }

  first = di->txin;
  last = PREVTXD (di->txout);

  /* move entries starting at last and moving backwards to first */
  for (old = last; old != PREVTXD (first); old = PREVTXD (old))
    {
      new = TXD (old + rot);

      /*
       * Move the tx dma descriptor.
       * EOT is set only in the last entry in the ring.
       */
      w = BUS_SWAP32 (R_SM (&di->txd64[old].ctrl1)) & ~D64_CTRL1_EOT;
      if (new == (di->ntxd - 1))
	w |= D64_CTRL1_EOT;
      W_SM (&di->txd64[new].ctrl1, BUS_SWAP32 (w));

      w = BUS_SWAP32 (R_SM (&di->txd64[old].ctrl2));
      W_SM (&di->txd64[new].ctrl2, BUS_SWAP32 (w));

      W_SM (&di->txd64[new].addrlow, R_SM (&di->txd64[old].addrlow));
      W_SM (&di->txd64[new].addrhigh, R_SM (&di->txd64[old].addrhigh));

      /* zap the old tx dma descriptor address field */
      W_SM (&di->txd64[old].addrlow, BUS_SWAP32 (0xdeadbeef));
      W_SM (&di->txd64[old].addrhigh, BUS_SWAP32 (0xdeadbeef));

      /* move the corresponding txp[] entry */
      ASSERT (di->txp[new] == NULL);
      di->txp[new] = di->txp[old];
      di->txp[old] = NULL;
    }

  /* update txin and txout */
  di->txin = ad;
  di->txout = TXD (di->txout + rot);
  di->hnddma.txavail = di->ntxd - NTXDACTIVE (di->txin, di->txout) - 1;

  /* kick the chip */
  W_REG (di->osh, &di->d64txregs->ptr, I2B (di->txout, dma64dd_t));
}

#endif /* BCMDMA64 */

uint
dma_addrwidth (sb_t * sbh, void *dmaregs)
{
  dma32regs_t *dma32regs;
  osl_t *osh;

  osh = sb_osh (sbh);

  if (DMA64_CAP)
    {
      /* DMA engine is 64-bit capable */
      if (((sb_coreflagshi (sbh, 0, 0) & SBTMH_DMA64) == SBTMH_DMA64))
	{
	  /* backplane are 64 bits capable */
	  if (sb_backplane64 (sbh))
	    /* If bus is System Backplane or PCIE then we can access 64-bits */
	    if ((BUSTYPE (sbh->bustype) == SB_BUS) ||
		((BUSTYPE (sbh->bustype) == PCI_BUS) &&
		 sbh->buscoretype == SB_PCIE))
	      return (DMADDRWIDTH_64);

	  /* DMA64 is always 32 bits capable, AE is always TRUE */
#ifdef BCMDMA64
	  ASSERT (_dma64_addrext (osh, (dma64regs_t *) dmaregs));
#endif
	  return (DMADDRWIDTH_32);
	}
    }

  /* Start checking for 32-bit / 30-bit addressing */
  dma32regs = (dma32regs_t *) dmaregs;

  /* For System Backplane, PCIE bus or addrext feature, 32-bits ok */
  if ((BUSTYPE (sbh->bustype) == SB_BUS) ||
      ((BUSTYPE (sbh->bustype) == PCI_BUS) && sbh->buscoretype == SB_PCIE) ||
      (_dma32_addrext (osh, dma32regs)))
    return (DMADDRWIDTH_32);

  /* Fallthru */
  return (DMADDRWIDTH_30);
}
