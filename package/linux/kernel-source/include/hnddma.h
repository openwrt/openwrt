/*
 * Generic Broadcom Home Networking Division (HND) DMA engine definitions.
 * This supports the following chips: BCM42xx, 44xx, 47xx .
 *
 * Copyright 2004, Broadcom Corporation      
 * All Rights Reserved.      
 *       
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY      
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM      
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS      
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.      
 * $Id$
 */

#ifndef	_hnddma_h_
#define	_hnddma_h_

/*
 * Each DMA processor consists of a transmit channel and a receive channel.
 */
typedef volatile struct {
	/* transmit channel */
	uint32	xmtcontrol;			/* enable, et al */
	uint32	xmtaddr;			/* descriptor ring base address (4K aligned) */
	uint32	xmtptr;				/* last descriptor posted to chip */
	uint32	xmtstatus;			/* current active descriptor, et al */

	/* receive channel */
	uint32	rcvcontrol;			/* enable, et al */
	uint32	rcvaddr;			/* descriptor ring base address (4K aligned) */
	uint32	rcvptr;				/* last descriptor posted to chip */
	uint32	rcvstatus;			/* current active descriptor, et al */
} dmaregs_t;

typedef volatile struct {
	/* diag access */
	uint32	fifoaddr;			/* diag address */
	uint32	fifodatalow;			/* low 32bits of data */
	uint32	fifodatahigh;			/* high 32bits of data */
	uint32	pad;				/* reserved */
} dmafifo_t;

/* transmit channel control */
#define	XC_XE		((uint32)1 << 0)	/* transmit enable */
#define	XC_SE		((uint32)1 << 1)	/* transmit suspend request */
#define	XC_LE		((uint32)1 << 2)	/* loopback enable */
#define	XC_FL		((uint32)1 << 4)	/* flush request */

/* transmit descriptor table pointer */
#define	XP_LD_MASK	0xfff			/* last valid descriptor */

/* transmit channel status */
#define	XS_CD_MASK	0x0fff			/* current descriptor pointer */
#define	XS_XS_MASK	0xf000			/* transmit state */
#define	XS_XS_SHIFT	12
#define	XS_XS_DISABLED	0x0000			/* disabled */
#define	XS_XS_ACTIVE	0x1000			/* active */
#define	XS_XS_IDLE	0x2000			/* idle wait */
#define	XS_XS_STOPPED	0x3000			/* stopped */
#define	XS_XS_SUSP	0x4000			/* suspend pending */
#define	XS_XE_MASK	0xf0000			/* transmit errors */
#define	XS_XE_SHIFT	16
#define	XS_XE_NOERR	0x00000			/* no error */
#define	XS_XE_DPE	0x10000			/* descriptor protocol error */
#define	XS_XE_DFU	0x20000			/* data fifo underrun */
#define	XS_XE_BEBR	0x30000			/* bus error on buffer read */
#define	XS_XE_BEDA	0x40000			/* bus error on descriptor access */
#define	XS_AD_MASK	0xfff00000		/* active descriptor */
#define	XS_AD_SHIFT	20

/* receive channel control */
#define	RC_RE		((uint32)1 << 0)	/* receive enable */
#define	RC_RO_MASK	0xfe			/* receive frame offset */
#define	RC_RO_SHIFT	1
#define	RC_FM		((uint32)1 << 8)	/* direct fifo receive (pio) mode */

/* receive descriptor table pointer */
#define	RP_LD_MASK	0xfff			/* last valid descriptor */

/* receive channel status */
#define	RS_CD_MASK	0x0fff			/* current descriptor pointer */
#define	RS_RS_MASK	0xf000			/* receive state */
#define	RS_RS_SHIFT	12
#define	RS_RS_DISABLED	0x0000			/* disabled */
#define	RS_RS_ACTIVE	0x1000			/* active */
#define	RS_RS_IDLE	0x2000			/* idle wait */
#define	RS_RS_STOPPED	0x3000			/* reserved */
#define	RS_RE_MASK	0xf0000			/* receive errors */
#define	RS_RE_SHIFT	16
#define	RS_RE_NOERR	0x00000			/* no error */
#define	RS_RE_DPE	0x10000			/* descriptor protocol error */
#define	RS_RE_DFO	0x20000			/* data fifo overflow */
#define	RS_RE_BEBW	0x30000			/* bus error on buffer write */
#define	RS_RE_BEDA	0x40000			/* bus error on descriptor access */
#define	RS_AD_MASK	0xfff00000		/* active descriptor */
#define	RS_AD_SHIFT	20

/* fifoaddr */
#define	FA_OFF_MASK	0xffff			/* offset */
#define	FA_SEL_MASK	0xf0000			/* select */
#define	FA_SEL_SHIFT	16
#define	FA_SEL_XDD	0x00000			/* transmit dma data */
#define	FA_SEL_XDP	0x10000			/* transmit dma pointers */
#define	FA_SEL_RDD	0x40000			/* receive dma data */
#define	FA_SEL_RDP	0x50000			/* receive dma pointers */
#define	FA_SEL_XFD	0x80000			/* transmit fifo data */
#define	FA_SEL_XFP	0x90000			/* transmit fifo pointers */
#define	FA_SEL_RFD	0xc0000			/* receive fifo data */
#define	FA_SEL_RFP	0xd0000			/* receive fifo pointers */

/*
 * DMA Descriptor
 * Descriptors are only read by the hardware, never written back.
 */
typedef volatile struct {
	uint32	ctrl;		/* misc control bits & bufcount */
	uint32	addr;		/* data buffer address */
} dmadd_t;

/*
 * Each descriptor ring must be 4096byte aligned
 * and fit within a single 4096byte page.
 */
#define	DMAMAXRINGSZ	4096
#define	DMARINGALIGN	4096

/* control flags */
#define	CTRL_BC_MASK	0x1fff			/* buffer byte count */
#define	CTRL_EOT	((uint32)1 << 28)	/* end of descriptor table */
#define	CTRL_IOC	((uint32)1 << 29)	/* interrupt on completion */
#define	CTRL_EOF	((uint32)1 << 30)	/* end of frame */
#define	CTRL_SOF	((uint32)1 << 31)	/* start of frame */

/* control flags in the range [27:20] are core-specific and not defined here */
#define	CTRL_CORE_MASK	0x0ff00000

/* export structure */
typedef volatile struct {
	/* rx error counters */
	uint		rxgiants;	/* rx giant frames */
	uint		rxnobuf;	/* rx out of dma descriptors */
	/* tx error counters */
	uint		txnobuf;	/* tx out of dma descriptors */
} hnddma_t;

#ifndef di_t
#define	di_t	void
#endif

/* externs */
extern void *dma_attach(void *drv, void *dev, char *name, dmaregs_t *dmaregs,
	uint ntxd, uint nrxd, uint rxbufsize, uint nrxpost, uint rxoffset,
	uint ddoffset, uint dataoffset, uint *msg_level);
extern void dma_detach(di_t *di);
extern void dma_txreset(di_t *di);
extern void dma_rxreset(di_t *di);
extern void dma_txinit(di_t *di);
extern bool dma_txenabled(di_t *di);
extern void dma_rxinit(di_t *di);
extern void dma_rxenable(di_t *di);
extern bool dma_rxenabled(di_t *di);
extern void dma_txsuspend(di_t *di);
extern void dma_txresume(di_t *di);
extern bool dma_txsuspended(di_t *di);
extern bool dma_txstopped(di_t *di);
extern bool dma_rxstopped(di_t *di);
extern int dma_txfast(di_t *di, void *p, uint32 coreflags);
extern int dma_tx(di_t *di, void *p, uint32 coreflags);
extern void dma_fifoloopbackenable(di_t *di);
extern void *dma_rx(di_t *di);
extern void dma_rxfill(di_t *di);
extern void dma_txreclaim(di_t *di, bool forceall);
extern void dma_rxreclaim(di_t *di);
extern char *dma_dump(di_t *di, char *buf);
extern char *dma_dumptx(di_t *di, char *buf);
extern char *dma_dumprx(di_t *di, char *buf);
extern uint dma_getvar(di_t *di, char *name);
extern void *dma_getnexttxp(di_t *di, bool forceall);
extern void *dma_peeknexttxp(di_t *di);
extern void *dma_getnextrxp(di_t *di, bool forceall);
extern void dma_txblock(di_t *di);
extern void dma_txunblock(di_t *di);
extern uint dma_txactive(di_t *di);
extern void dma_txrotate(di_t *di);

#endif	/* _hnddma_h_ */
