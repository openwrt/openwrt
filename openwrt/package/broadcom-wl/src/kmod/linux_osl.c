/*
 * Linux OS Independent Layer
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: linux_osl.c,v 1.1.1.14 2006/04/08 06:13:39 honor Exp $
 */

#define LINUX_OSL

#include <typedefs.h>
#include <bcmendian.h>
#include <linux/module.h>
#include <linuxver.h>
#include <bcmdefs.h>
#include <osl.h>
#include "linux_osl.h"
#include <bcmutils.h>
#include <linux/delay.h>
#ifdef mips
#include <asm/paccess.h>
#endif /* mips */
#include <pcicfg.h>

#define PCI_CFG_RETRY 		10	

#define OS_HANDLE_MAGIC		0x1234abcd	/* Magic # to recognise osh */
#define BCM_MEM_FILENAME_LEN 	24		/* Mem. filename length */

typedef struct bcm_mem_link {
	struct bcm_mem_link *prev;
	struct bcm_mem_link *next;
	uint	size;
	int	line;
	char	file[BCM_MEM_FILENAME_LEN];
} bcm_mem_link_t;

static int16 linuxbcmerrormap[] =  \
{	0, 			/* 0 */
	-EINVAL,		/* BCME_ERROR */
	-EINVAL,		/* BCME_BADARG */
	-EINVAL,		/* BCME_BADOPTION */
	-EINVAL,		/* BCME_NOTUP */
	-EINVAL,		/* BCME_NOTDOWN */
	-EINVAL,		/* BCME_NOTAP */
	-EINVAL,		/* BCME_NOTSTA */
	-EINVAL,		/* BCME_BADKEYIDX */
	-EINVAL,		/* BCME_RADIOOFF */
	-EINVAL,		/* BCME_NOTBANDLOCKED */
	-EINVAL, 		/* BCME_NOCLK */
	-EINVAL, 		/* BCME_BADRATESET */
	-EINVAL, 		/* BCME_BADBAND */
	-E2BIG,			/* BCME_BUFTOOSHORT */
	-E2BIG,			/* BCME_BUFTOOLONG */
	-EBUSY, 		/* BCME_BUSY */
	-EINVAL, 		/* BCME_NOTASSOCIATED */
	-EINVAL, 		/* BCME_BADSSIDLEN */
	-EINVAL, 		/* BCME_OUTOFRANGECHAN */
	-EINVAL, 		/* BCME_BADCHAN */
	-EFAULT, 		/* BCME_BADADDR */
	-ENOMEM, 		/* BCME_NORESOURCE */
	-EOPNOTSUPP,		/* BCME_UNSUPPORTED */
	-EMSGSIZE,		/* BCME_BADLENGTH */
	-EINVAL,		/* BCME_NOTREADY */
	-EPERM,			/* BCME_NOTPERMITTED */
	-ENOMEM, 		/* BCME_NOMEM */
	-EINVAL, 		/* BCME_ASSOCIATED */
	-ERANGE, 		/* BCME_RANGE */
	-EINVAL, 		/* BCME_NOTFOUND */
	-EINVAL, 		/* BCME_WME_NOT_ENABLED */
	-EINVAL, 		/* BCME_TSPEC_NOTFOUND */
	-EINVAL, 		/* BCME_ACM_NOTSUPPORTED */
	-EINVAL,		/* BCME_NOT_WME_ASSOCIATION */
	-EIO,			/* BCME_SDIO_ERROR */
	-ENODEV			/* BCME_DONGLE_DOWN */
};

/* translate bcmerrors into linux errors */
int
osl_error(int bcmerror)
{
	int abs_bcmerror;
	int array_size = ARRAYSIZE(linuxbcmerrormap);

	abs_bcmerror = ABS(bcmerror);

	if (bcmerror > 0)
		abs_bcmerror = 0;

	else if (abs_bcmerror >= array_size)
		abs_bcmerror = BCME_ERROR;

	return linuxbcmerrormap[abs_bcmerror];
}

osl_t *
osl_attach(void *pdev, bool pkttag)
{
	osl_t *osh;

	osh = kmalloc(sizeof(osl_t), GFP_ATOMIC);
	ASSERT(osh);

	bzero(osh, sizeof(osl_t));

	/*
	 * check the cases where
	 * 1.Error code Added to bcmerror table, but forgot to add it to the OS
	 * dependent error code
	 * 2. Error code is added to the bcmerror table, but forgot to add the
	 * corresponding errorstring(dummy call to bcmerrorstr)
	 */
	bcmerrorstr(0);
	ASSERT(ABS(BCME_LAST) == (ARRAYSIZE(linuxbcmerrormap) - 1));

	osh->magic = OS_HANDLE_MAGIC;
	osh->malloced = 0;
	osh->failed = 0;
	osh->dbgmem_list = NULL;
	osh->pdev = pdev;
	osh->pub.pkttag = pkttag;

	return osh;
}

void
osl_detach(osl_t *osh)
{
	if (osh == NULL)
		return;

	ASSERT(osh->magic == OS_HANDLE_MAGIC);
	kfree(osh);
}

/* Return a new packet. zero out pkttag */
void*
osl_pktget(osl_t *osh, uint len, bool send)
{
	struct sk_buff *skb;

	if ((skb = dev_alloc_skb(len))) {
		skb_put(skb, len);
		skb->priority = 0;

#ifdef BCMDBG_PKT
	pktlist_add(&(osh->pktlist), (void *) skb);
#endif  /* BCMDBG_PKT */

		osh->pub.pktalloced++;
	}

	return ((void*) skb);
}

/* Free the driver packet. Free the tag if present */
void
osl_pktfree(osl_t *osh, void *p)
{
	struct sk_buff *skb, *nskb;

	skb = (struct sk_buff*) p;

	/* perversion: we use skb->next to chain multi-skb packets */
	while (skb) {
		nskb = skb->next;
		skb->next = NULL;

#ifdef BCMDBG_PKT
		pktlist_remove(&(osh->pktlist), (void *) skb);
#endif  /* BCMDBG_PKT */

		if (skb->destructor) {
			/* cannot kfree_skb() on hard IRQ (net/core/skbuff.c) if destructor exists
			 */
			dev_kfree_skb_any(skb);
		} else {
			/* can free immediately (even in_irq()) if destructor does not exist */
			dev_kfree_skb(skb);
		}

		osh->pub.pktalloced--;

		skb = nskb;
	}
}

void*
osl_malloc(osl_t *osh, uint size)
{
	void *addr;

	/* only ASSERT if osh is defined */
	if (osh)
		ASSERT(osh->magic == OS_HANDLE_MAGIC);

	if ((addr = kmalloc(size, GFP_ATOMIC)) == NULL) {
		if (osh)
			osh->failed++;
		return (NULL);
	}
	if (osh)
		osh->malloced += size;

	return (addr);
}

void
osl_mfree(osl_t *osh, void *addr, uint size)
{
	if (osh) {
		ASSERT(osh->magic == OS_HANDLE_MAGIC);
		osh->malloced -= size;
	}
	kfree(addr);
}

uint
osl_malloced(osl_t *osh)
{
	ASSERT((osh && (osh->magic == OS_HANDLE_MAGIC)));
	return (osh->malloced);
}

uint osl_malloc_failed(osl_t *osh)
{
	ASSERT((osh && (osh->magic == OS_HANDLE_MAGIC)));
	return (osh->failed);
}

#undef osl_delay
void
osl_delay(uint usec)
{
	OSL_DELAY(usec);
}

/* Clone a packet.
 * The pkttag contents are NOT cloned.
 */
void *
osl_pktdup(osl_t *osh, void *skb)
{
	void * p;

	if ((p = skb_clone((struct sk_buff*)skb, GFP_ATOMIC)) == NULL)
		return NULL;

	/* skb_clone copies skb->cb.. we don't want that */
	if (osh->pub.pkttag)
		bzero((void*)((struct sk_buff *)p)->cb, OSL_PKTTAG_SZ);

	/* Increment the packet counter */
	osh->pub.pktalloced++;
	return (p);
}

uint
osl_pktalloced(osl_t *osh)
{
	return (osh->pub.pktalloced);
}

