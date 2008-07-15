/* $Id: $ */

/*
 * Copyright (c) 2008 Daniel Mueller (daniel@danm.de)
 * Copyright (c) 2007 David McCullough (david_mccullough@securecomputing.com)
 * Copyright (c) 2000 Jason L. Wright (jason@thought.net)
 * Copyright (c) 2000 Theo de Raadt (deraadt@openbsd.org)
 * Copyright (c) 2001 Patrik Lindergren (patrik@ipunplugged.com)
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Effort sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F30602-01-2-0537.
 *
 */
#undef UBSEC_DEBUG
#undef UBSEC_VERBOSE_DEBUG

#ifdef UBSEC_VERBOSE_DEBUG
#define UBSEC_DEBUG
#endif

/*
 * uBsec BCM5365 hardware crypto accelerator
 */

#include <linux/kernel.h>
#include <linux/byteorder/swab.h>
#include <linux/byteorder/generic.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/random.h>
#include <linux/skbuff.h>
#include <linux/stat.h>
#include <asm/io.h>

#include <linux/ssb/ssb.h>

/*
 * BSD queue
 */
#include "bsdqueue.h"

/* 
 * OCF
 */
#include "cryptodev.h"
#include "uio.h"

#define HMAC_HACK 1

#ifdef HMAC_HACK
#include "hmachack.h"
#include "md5.h"
#include "md5.c"
#include "sha1.h"
#include "sha1.c"
#endif

#include "ubsecreg.h"
#include "ubsecvar.h"

#define DRV_MODULE_NAME     "ubsec_ssb"
#define PFX DRV_MODULE_NAME ": "
#define DRV_MODULE_VERSION  "0.01"
#define DRV_MODULE_RELDATE  "Jan 1, 2008"

#if 1
#define DPRINTF(a...) \
    if (debug) \
    { \
        printk(DRV_MODULE_NAME ": " a); \
    }
#else
#define DPRINTF(a...)
#endif

/*
 * Prototypes 
 */
static irqreturn_t ubsec_ssb_isr(int, void *, struct pt_regs *);
static int __devinit ubsec_ssb_probe(struct ssb_device *sdev,
    const struct ssb_device_id *ent);
static void __devexit ubsec_ssb_remove(struct ssb_device *sdev);
int ubsec_attach(struct ssb_device *sdev, const struct ssb_device_id *ent, 
    struct device *self);
static void ubsec_setup_mackey(struct ubsec_session *ses, int algo, 
    caddr_t key, int klen);
static int dma_map_skb(struct ubsec_softc *sc, 
    struct ubsec_dma_alloc* q_map, struct sk_buff *skb, int *mlen);
static int dma_map_uio(struct ubsec_softc *sc, 
    struct ubsec_dma_alloc *q_map, struct uio *uio, int *mlen);
static void dma_unmap(struct ubsec_softc *sc, 
    struct ubsec_dma_alloc *q_map, int mlen);
static int ubsec_dmamap_aligned(struct ubsec_softc *sc, 
    const struct ubsec_dma_alloc *q_map, int mlen);

#ifdef UBSEC_DEBUG
static int proc_read(char *buf, char **start, off_t offset,
    int size, int *peof, void *data);
#endif

void ubsec_reset_board(struct ubsec_softc *);
void ubsec_init_board(struct ubsec_softc *);
void ubsec_cleanchip(struct ubsec_softc *);
void ubsec_totalreset(struct ubsec_softc *);
int  ubsec_free_q(struct ubsec_softc*, struct ubsec_q *);

static int ubsec_newsession(device_t, u_int32_t *, struct cryptoini *);
static int ubsec_freesession(device_t, u_int64_t);
static int ubsec_process(device_t, struct cryptop *, int);

void    ubsec_callback(struct ubsec_softc *, struct ubsec_q *);
void    ubsec_feed(struct ubsec_softc *);
void    ubsec_mcopy(struct sk_buff *, struct sk_buff *, int, int);
void    ubsec_dma_free(struct ubsec_softc *, struct ubsec_dma_alloc *);
int     ubsec_dma_malloc(struct ubsec_softc *, struct ubsec_dma_alloc *,
        size_t, int);

/* DEBUG crap... */
void ubsec_dump_pb(struct ubsec_pktbuf *);
void ubsec_dump_mcr(struct ubsec_mcr *);

#define READ_REG(sc,r) \
    ssb_read32((sc)->sdev, (r));
#define WRITE_REG(sc,r,val) \
    ssb_write32((sc)->sdev, (r), (val));
#define READ_REG_SDEV(sdev,r) \
    ssb_read32((sdev), (r));
#define WRITE_REG_SDEV(sdev,r,val) \
    ssb_write32((sdev), (r), (val));

#define SWAP32(x) (x) = htole32(ntohl((x)))
#define HTOLE32(x) (x) = htole32(x)

#ifdef __LITTLE_ENDIAN
#define letoh16(x) (x)
#define letoh32(x) (x)
#endif

static int debug;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Enable debug output");

#define UBSEC_SSB_MAX_CHIPS 1
static struct ubsec_softc *ubsec_chip_idx[UBSEC_SSB_MAX_CHIPS];
static struct ubsec_stats ubsecstats;

#ifdef UBSEC_DEBUG
static struct proc_dir_entry *procdebug;
#endif

static struct ssb_device_id ubsec_ssb_tbl[] = {
    /* Broadcom BCM5365P IPSec Core */
    SSB_DEVICE(SSB_VENDOR_BROADCOM, SSB_DEV_IPSEC, SSB_ANY_REV),
    SSB_DEVTABLE_END
};

static struct ssb_driver ubsec_ssb_driver = {
    .name       = DRV_MODULE_NAME,
    .id_table   = ubsec_ssb_tbl,
    .probe      = ubsec_ssb_probe,
    .remove     = __devexit_p(ubsec_ssb_remove),
     /*
    .suspend    = ubsec_ssb_suspend,
    .resume     = ubsec_ssb_resume
    */
};

static device_method_t ubsec_ssb_methods = {
    /* crypto device methods */
    DEVMETHOD(cryptodev_newsession, ubsec_newsession),
    DEVMETHOD(cryptodev_freesession,ubsec_freesession),
    DEVMETHOD(cryptodev_process,    ubsec_process),
};

#ifdef UBSEC_DEBUG
static int 
proc_read(char *buf, char **start, off_t offset,
    int size, int *peof, void *data)
{
    int i = 0, byteswritten = 0, ret;
    unsigned int stat, ctrl;
#ifdef UBSEC_VERBOSE_DEBUG
    struct ubsec_q *q;
    struct ubsec_dma *dmap;
#endif
   
    while ((i < UBSEC_SSB_MAX_CHIPS) && (ubsec_chip_idx[i] != NULL))
    {
        struct ubsec_softc *sc = ubsec_chip_idx[i];
        
        stat = READ_REG(sc, BS_STAT);
        ctrl = READ_REG(sc, BS_CTRL);
        ret = snprintf((buf + byteswritten), 
            (size - byteswritten) , 
            "DEV %d, DMASTAT %08x, DMACTRL %08x\n", i, stat, ctrl);

        byteswritten += ret;

#ifdef UBSEC_VERBOSE_DEBUG
        printf("DEV %d, DMASTAT %08x, DMACTRL %08x\n", i, stat, ctrl);

        /* Dump all queues MCRs */
        if (!BSD_SIMPLEQ_EMPTY(&sc->sc_qchip)) {
            BSD_SIMPLEQ_FOREACH(q, &sc->sc_qchip, q_next)
            {
                dmap = q->q_dma;
                ubsec_dump_mcr(&dmap->d_dma->d_mcr);
            }
        }
#endif

        i++;
    }

    *peof = 1;

    return byteswritten;
}
#endif

/*
 * map in a given sk_buff
 */
static int
dma_map_skb(struct ubsec_softc *sc, struct ubsec_dma_alloc* q_map, struct sk_buff *skb, int *mlen)
{
    int i = 0;
    dma_addr_t tmp;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    /*
     * We support only a limited number of fragments.
     */
    if (unlikely((skb_shinfo(skb)->nr_frags + 1) >= UBS_MAX_SCATTER))
    {
        printk(KERN_ERR "Only %d scatter fragments are supported.\n", UBS_MAX_SCATTER);
        return (-ENOMEM);
    }

#ifdef UBSEC_VERBOSE_DEBUG
    DPRINTF("%s - map %d 0x%x %d\n", __FUNCTION__, 0, (unsigned int)skb->data, skb_headlen(skb));
#endif

    /* first data package */
    tmp = dma_map_single(sc->sc_dv,
                         skb->data,
                         skb_headlen(skb),
                         DMA_BIDIRECTIONAL);
    
    q_map[i].dma_paddr = tmp;
    q_map[i].dma_vaddr = skb->data;
    q_map[i].dma_size = skb_headlen(skb);

    if (unlikely(tmp == 0))
    {
        printk(KERN_ERR "Could not map memory region for dma.\n");
        return (-EINVAL);
    }

#ifdef UBSEC_VERBOSE_DEBUG
    DPRINTF("%s - map %d done physical addr 0x%x\n", __FUNCTION__, 0, (unsigned int)tmp);
#endif


    /* all other data packages */    
    for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {

#ifdef UBSEC_VERBOSE_DEBUG
        DPRINTF("%s - map %d 0x%x %d\n", __FUNCTION__, i + 1, 
            (unsigned int)page_address(skb_shinfo(skb)->frags[i].page) +
            skb_shinfo(skb)->frags[i].page_offset, skb_shinfo(skb)->frags[i].size);
#endif

        tmp = dma_map_single(sc->sc_dv,
                             page_address(skb_shinfo(skb)->frags[i].page) +
                                 skb_shinfo(skb)->frags[i].page_offset, 
                             skb_shinfo(skb)->frags[i].size,
                             DMA_BIDIRECTIONAL);

        q_map[i + 1].dma_paddr = tmp;
        q_map[i + 1].dma_vaddr = (void*)(page_address(skb_shinfo(skb)->frags[i].page) +
                                  skb_shinfo(skb)->frags[i].page_offset);
        q_map[i + 1].dma_size = skb_shinfo(skb)->frags[i].size;

        if (unlikely(tmp == 0))
        {
            printk(KERN_ERR "Could not map memory region for dma.\n");
            return (-EINVAL);
        }

#ifdef UBSEC_VERBOSE_DEBUG
        DPRINTF("%s - map %d done physical addr 0x%x\n", __FUNCTION__, i + 1, (unsigned int)tmp);
#endif

    }
    *mlen = i + 1;

    return(0);
}

/*
 * map in a given uio buffer
 */

static int
dma_map_uio(struct ubsec_softc *sc, struct ubsec_dma_alloc *q_map, struct uio *uio, int *mlen)
{
    struct iovec *iov = uio->uio_iov;
    int n;
    dma_addr_t tmp;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    /*
     * We support only a limited number of fragments.
     */
    if (unlikely(uio->uio_iovcnt >= UBS_MAX_SCATTER))
    {
        printk(KERN_ERR "Only %d scatter fragments are supported.\n", UBS_MAX_SCATTER);
        return (-ENOMEM);
    }

    for (n = 0; n < uio->uio_iovcnt; n++) {
#ifdef UBSEC_VERBOSE_DEBUG
        DPRINTF("%s - map %d 0x%x %d\n", __FUNCTION__, n, (unsigned int)iov->iov_base, iov->iov_len);
#endif
        tmp = dma_map_single(sc->sc_dv,
                             iov->iov_base,
                             iov->iov_len,
                             DMA_BIDIRECTIONAL);

        q_map[n].dma_paddr = tmp;
        q_map[n].dma_vaddr = iov->iov_base;
        q_map[n].dma_size = iov->iov_len;

        if (unlikely(tmp == 0))
                       {
            printk(KERN_ERR "Could not map memory region for dma.\n");
            return (-EINVAL);
        }

#ifdef UBSEC_VERBOSE_DEBUG
        DPRINTF("%s - map %d done physical addr 0x%x\n", __FUNCTION__, n, (unsigned int)tmp);
#endif

        iov++;
    }
    *mlen = n;

    return(0);
}

static void
dma_unmap(struct ubsec_softc *sc, struct ubsec_dma_alloc *q_map, int mlen)
{
    int i;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    for(i = 0; i < mlen; i++)
    {
#ifdef UBSEC_VERBOSE_DEBUG
        DPRINTF("%s - unmap %d 0x%x %d\n", __FUNCTION__, i, (unsigned int)q_map[i].dma_paddr, q_map[i].dma_size);
#endif
        dma_unmap_single(sc->sc_dv,
                         q_map[i].dma_paddr,
                         q_map[i].dma_size,
                         DMA_BIDIRECTIONAL);
    }
    return;
}

/*
 * Is the operand suitable aligned for direct DMA.  Each
 * segment must be aligned on a 32-bit boundary and all
 * but the last segment must be a multiple of 4 bytes.
 */
static int
ubsec_dmamap_aligned(struct ubsec_softc *sc, const struct ubsec_dma_alloc *q_map, int mlen)
{
    int i;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    for (i = 0; i < mlen; i++) {
        if (q_map[i].dma_paddr & 3)
            return (0);
        if (i != (mlen - 1) && (q_map[i].dma_size & 3))
            return (0);
    }
    return (1);
}


#define N(a)    (sizeof(a) / sizeof (a[0]))
static void
ubsec_setup_mackey(struct ubsec_session *ses, int algo, caddr_t key, int klen)
{
#ifdef HMAC_HACK
    MD5_CTX md5ctx;
    SHA1_CTX sha1ctx;
    int i;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    for (i = 0; i < klen; i++)
        key[i] ^= HMAC_IPAD_VAL;

    if (algo == CRYPTO_MD5_HMAC) {
        MD5Init(&md5ctx);
        MD5Update(&md5ctx, key, klen);
        MD5Update(&md5ctx, hmac_ipad_buffer, MD5_HMAC_BLOCK_LEN - klen);
        bcopy(md5ctx.md5_st8, ses->ses_hminner, sizeof(md5ctx.md5_st8));
    } else {
        SHA1Init(&sha1ctx);
        SHA1Update(&sha1ctx, key, klen);
        SHA1Update(&sha1ctx, hmac_ipad_buffer,
            SHA1_HMAC_BLOCK_LEN - klen);
        bcopy(sha1ctx.h.b32, ses->ses_hminner, sizeof(sha1ctx.h.b32));
    }

    for (i = 0; i < klen; i++)
        key[i] ^= (HMAC_IPAD_VAL ^ HMAC_OPAD_VAL);

    if (algo == CRYPTO_MD5_HMAC) {
        MD5Init(&md5ctx);
        MD5Update(&md5ctx, key, klen);
        MD5Update(&md5ctx, hmac_opad_buffer, MD5_HMAC_BLOCK_LEN - klen);
        bcopy(md5ctx.md5_st8, ses->ses_hmouter, sizeof(md5ctx.md5_st8));
    } else {
        SHA1Init(&sha1ctx);
        SHA1Update(&sha1ctx, key, klen);
        SHA1Update(&sha1ctx, hmac_opad_buffer,
            SHA1_HMAC_BLOCK_LEN - klen);
        bcopy(sha1ctx.h.b32, ses->ses_hmouter, sizeof(sha1ctx.h.b32));
    }

    for (i = 0; i < klen; i++)
        key[i] ^= HMAC_OPAD_VAL;

#else /* HMAC_HACK */
    DPRINTF("md5/sha not implemented\n");
#endif /* HMAC_HACK */
}
#undef N

static int 
__devinit ubsec_ssb_probe(struct ssb_device *sdev, 
    const struct ssb_device_id *ent) 
{
    int err;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    err = ssb_bus_powerup(sdev->bus, 0);
    if (err) {
        dev_err(sdev->dev, "Failed to powerup the bus\n");
        goto err_powerup;
    }

    err = request_irq(sdev->irq, (irq_handler_t)ubsec_ssb_isr, 
        IRQF_DISABLED | IRQF_SHARED, DRV_MODULE_NAME, sdev);
    if (err) {
        dev_err(sdev->dev, "Could not request irq\n");
        goto err_out_powerdown;
    }

    err = ssb_dma_set_mask(sdev, DMA_32BIT_MASK);
    if (err) {
        dev_err(sdev->dev,
        "Required 32BIT DMA mask unsupported by the system.\n");
        goto err_out_powerdown;
    }

    printk(KERN_INFO "Sentry5(tm) ROBOGateway(tm) IPSec Core at IRQ %u\n",
        sdev->irq);

    DPRINTF("Vendor: %x, core id: %x, revision: %x\n",
        sdev->id.vendor, sdev->id.coreid, sdev->id.revision);

    ssb_device_enable(sdev, 0);

    if (ubsec_attach(sdev, ent, sdev->dev) != 0)
        goto err_disable_interrupt;

#ifdef UBSEC_DEBUG
    procdebug = create_proc_entry(DRV_MODULE_NAME, S_IRUSR, NULL);
    if (procdebug)
    {
        procdebug->read_proc = proc_read;
        procdebug->data = NULL;
    } else 
        DPRINTF("Unable to create proc file.\n");
#endif

    return 0;

err_disable_interrupt:
    free_irq(sdev->irq, sdev);

err_out_powerdown:
    ssb_bus_may_powerdown(sdev->bus);

err_powerup:
    ssb_device_disable(sdev, 0);
    return err;    
}

static void __devexit ubsec_ssb_remove(struct ssb_device *sdev) {

    struct ubsec_softc *sc;
    unsigned int ctrlflgs;
    struct ubsec_dma *dmap;
    u_int32_t i;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    ctrlflgs = READ_REG_SDEV(sdev, BS_CTRL);
    /* disable all IPSec Core interrupts globally */
    ctrlflgs ^= (BS_CTRL_MCR1INT | BS_CTRL_MCR2INT |
        BS_CTRL_DMAERR);
    WRITE_REG_SDEV(sdev, BS_CTRL, ctrlflgs);

    free_irq(sdev->irq, sdev);

    sc = (struct ubsec_softc *)ssb_get_drvdata(sdev);

    /* unregister all crypto algorithms */
    crypto_unregister_all(sc->sc_cid);

    /* Free queue / dma memory */
    for (i = 0; i < UBS_MAX_NQUEUE; i++) {
        struct ubsec_q *q;

        q = sc->sc_queuea[i];
        if (q != NULL)
        {
            dmap = q->q_dma;
            if (dmap != NULL)
            {
                ubsec_dma_free(sc, &dmap->d_alloc);
                q->q_dma = NULL;
            }
            kfree(q);
        }
        sc->sc_queuea[i] = NULL;
    }

    ssb_bus_may_powerdown(sdev->bus);
    ssb_device_disable(sdev, 0);
    ssb_set_drvdata(sdev, NULL);

#ifdef UBSEC_DEBUG
    if (procdebug)
        remove_proc_entry(DRV_MODULE_NAME, NULL);
#endif

}


int
ubsec_attach(struct ssb_device *sdev, const struct ssb_device_id *ent, 
    struct device *self)
{
    struct ubsec_softc *sc = NULL;
    struct ubsec_dma *dmap;
    u_int32_t i;
    static int num_chips = 0;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    sc = (struct ubsec_softc *) kmalloc(sizeof(*sc), GFP_KERNEL);
    if (!sc)
        return(-ENOMEM);
    memset(sc, 0, sizeof(*sc));

    sc->sc_dv = sdev->dev;
    sc->sdev = sdev;

    spin_lock_init(&sc->sc_ringmtx);

    softc_device_init(sc, "ubsec_ssb", num_chips, ubsec_ssb_methods);

    /* Maybe someday there are boards with more than one chip available */
    if (num_chips < UBSEC_SSB_MAX_CHIPS) {
        ubsec_chip_idx[device_get_unit(sc->sc_dev)] = sc;
        num_chips++;
    }

    ssb_set_drvdata(sdev, sc);

    BSD_SIMPLEQ_INIT(&sc->sc_queue);
    BSD_SIMPLEQ_INIT(&sc->sc_qchip);
    BSD_SIMPLEQ_INIT(&sc->sc_queue2);
    BSD_SIMPLEQ_INIT(&sc->sc_qchip2);
    BSD_SIMPLEQ_INIT(&sc->sc_q2free);

    sc->sc_statmask = BS_STAT_MCR1_DONE | BS_STAT_DMAERR;

    sc->sc_cid = crypto_get_driverid(softc_get_device(sc), CRYPTOCAP_F_HARDWARE);
    if (sc->sc_cid < 0) {
        device_printf(sc->sc_dev, "could not get crypto driver id\n");
        return -1;
    }

    BSD_SIMPLEQ_INIT(&sc->sc_freequeue);
    dmap = sc->sc_dmaa;
    for (i = 0; i < UBS_MAX_NQUEUE; i++, dmap++) {
        struct ubsec_q *q;

        q = (struct ubsec_q *)kmalloc(sizeof(struct ubsec_q), GFP_KERNEL);
        if (q == NULL) {
            printf(": can't allocate queue buffers\n");
            break;
        }

        if (ubsec_dma_malloc(sc, &dmap->d_alloc, sizeof(struct ubsec_dmachunk),0)) {
            printf(": can't allocate dma buffers\n");
            kfree(q);
            break;
        }
        dmap->d_dma = (struct ubsec_dmachunk *)dmap->d_alloc.dma_vaddr;

        q->q_dma = dmap;
        sc->sc_queuea[i] = q;

        BSD_SIMPLEQ_INSERT_TAIL(&sc->sc_freequeue, q, q_next);
    }

    /*
     * Reset Broadcom chip
     */
    ubsec_reset_board(sc);

    /*
     * Init Broadcom chip
     */
    ubsec_init_board(sc);

    /* supported crypto algorithms */
    crypto_register(sc->sc_cid, CRYPTO_3DES_CBC, 0, 0);
    crypto_register(sc->sc_cid, CRYPTO_DES_CBC, 0, 0);

    if (sc->sc_flags & UBS_FLAGS_AES) {
        crypto_register(sc->sc_cid, CRYPTO_AES_CBC, 0, 0);
        printf(KERN_INFO DRV_MODULE_NAME ": DES 3DES AES128 AES192 AES256 MD5_HMAC SHA1_HMAC\n");
    }
    else
        printf(KERN_INFO DRV_MODULE_NAME ": DES 3DES MD5_HMAC SHA1_HMAC\n");

    crypto_register(sc->sc_cid, CRYPTO_MD5_HMAC, 0, 0);
    crypto_register(sc->sc_cid, CRYPTO_SHA1_HMAC, 0, 0);

    return 0;
}

/*
 * UBSEC Interrupt routine
 */
static irqreturn_t 
ubsec_ssb_isr(int irq, void *arg, struct pt_regs *regs) 
{
    struct ubsec_softc *sc = NULL;
    volatile u_int32_t stat;
    struct ubsec_q *q;
    struct ubsec_dma *dmap;
    int npkts = 0, i;

#ifdef UBSEC_VERBOSE_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    sc = (struct ubsec_softc *)ssb_get_drvdata(arg);

    stat = READ_REG(sc, BS_STAT);

    stat &= sc->sc_statmask;
    if (stat == 0)
        return IRQ_NONE;

    WRITE_REG(sc, BS_STAT, stat);       /* IACK */

    /*
     * Check to see if we have any packets waiting for us
     */
    if ((stat & BS_STAT_MCR1_DONE)) {
        while (!BSD_SIMPLEQ_EMPTY(&sc->sc_qchip)) {
            q = BSD_SIMPLEQ_FIRST(&sc->sc_qchip);
            dmap = q->q_dma;

            if ((dmap->d_dma->d_mcr.mcr_flags & htole16(UBS_MCR_DONE)) == 0)
            {
                DPRINTF("error while processing MCR. Flags = %x\n", dmap->d_dma->d_mcr.mcr_flags);
                break;
            }

            BSD_SIMPLEQ_REMOVE_HEAD(&sc->sc_qchip, q_next);

            npkts = q->q_nstacked_mcrs;
            /*
             * search for further sc_qchip ubsec_q's that share
             * the same MCR, and complete them too, they must be
             * at the top.
             */
            for (i = 0; i < npkts; i++) {
                if(q->q_stacked_mcr[i])
                    ubsec_callback(sc, q->q_stacked_mcr[i]);
                else
                    break;
            }
            ubsec_callback(sc, q);
        }

        /*
         * Don't send any more packet to chip if there has been
         * a DMAERR.
         */
        if (likely(!(stat & BS_STAT_DMAERR)))
            ubsec_feed(sc);
        else
            DPRINTF("DMA error occurred. Stop feeding crypto chip.\n");
    }

    /*
     * Check to see if we got any DMA Error
     */
    if (stat & BS_STAT_DMAERR) {
        volatile u_int32_t a = READ_REG(sc, BS_ERR);

        printf(KERN_ERR "%s: dmaerr %s@%08x\n", DRV_MODULE_NAME,
            (a & BS_ERR_READ) ? "read" : "write", a & BS_ERR_ADDR);

        ubsecstats.hst_dmaerr++;
        ubsec_totalreset(sc);
        ubsec_feed(sc);
    }

    return IRQ_HANDLED;
}

/*
 * ubsec_feed() - aggregate and post requests to chip
 *        It is assumed that the caller set splnet()
 */
void
ubsec_feed(struct ubsec_softc *sc)
{
#ifdef UBSEC_VERBOSE_DEBUG
    static int max;
#endif 
    struct ubsec_q *q, *q2;
    int npkts, i;
    void *v;
    u_int32_t stat;

    npkts = sc->sc_nqueue;
    if (npkts > UBS_MAX_AGGR)
        npkts = UBS_MAX_AGGR;
    if (npkts < 2)
        goto feed1;

    stat = READ_REG(sc, BS_STAT);

    if (stat & (BS_STAT_MCR1_FULL | BS_STAT_DMAERR)) {
        if(stat & BS_STAT_DMAERR) {
            ubsec_totalreset(sc);
            ubsecstats.hst_dmaerr++;
        }
        return;
    }

#ifdef UBSEC_VERBOSE_DEBUG
    DPRINTF("merging %d records\n", npkts);

    /* XXX temporary aggregation statistics reporting code */
    if (max < npkts) {
        max = npkts;
        DPRINTF("%s: new max aggregate %d\n", DRV_MODULE_NAME, max);
    }
#endif /* UBSEC_VERBOSE_DEBUG */

    q = BSD_SIMPLEQ_FIRST(&sc->sc_queue);
    BSD_SIMPLEQ_REMOVE_HEAD(&sc->sc_queue, q_next);
    --sc->sc_nqueue;

#if 0
    /* 
     * XXX 
     * We use dma_map_single() - no sync required!
     */

    bus_dmamap_sync(sc->sc_dmat, q->q_src_map,
        0, q->q_src_map->dm_mapsize, BUS_DMASYNC_PREWRITE);
    if (q->q_dst_map != NULL)
        bus_dmamap_sync(sc->sc_dmat, q->q_dst_map,
            0, q->q_dst_map->dm_mapsize, BUS_DMASYNC_PREREAD);
#endif

    q->q_nstacked_mcrs = npkts - 1;     /* Number of packets stacked */

    for (i = 0; i < q->q_nstacked_mcrs; i++) {
        q2 = BSD_SIMPLEQ_FIRST(&sc->sc_queue);

#if 0
        bus_dmamap_sync(sc->sc_dmat, q2->q_src_map,
            0, q2->q_src_map->dm_mapsize, BUS_DMASYNC_PREWRITE);
        if (q2->q_dst_map != NULL)
            bus_dmamap_sync(sc->sc_dmat, q2->q_dst_map,
                0, q2->q_dst_map->dm_mapsize, BUS_DMASYNC_PREREAD);
#endif
        BSD_SIMPLEQ_REMOVE_HEAD(&sc->sc_queue, q_next);
        --sc->sc_nqueue;

        v = ((char *)&q2->q_dma->d_dma->d_mcr) + sizeof(struct ubsec_mcr) -
            sizeof(struct ubsec_mcr_add);
        bcopy(v, &q->q_dma->d_dma->d_mcradd[i], sizeof(struct ubsec_mcr_add));
        q->q_stacked_mcr[i] = q2;
    }
    q->q_dma->d_dma->d_mcr.mcr_pkts = htole16(npkts);
    BSD_SIMPLEQ_INSERT_TAIL(&sc->sc_qchip, q, q_next);
#if 0
    bus_dmamap_sync(sc->sc_dmat, q->q_dma->d_alloc.dma_map,
        0, q->q_dma->d_alloc.dma_map->dm_mapsize,
        BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
#endif
    WRITE_REG(sc, BS_MCR1, q->q_dma->d_alloc.dma_paddr +
        offsetof(struct ubsec_dmachunk, d_mcr));
#ifdef UBSEC_VERBOSE_DEBUG
    DPRINTF("feed (1): q->chip %p %08x %08x\n", q,
        (u_int32_t)q->q_dma->d_alloc.dma_paddr,
        (u_int32_t)(q->q_dma->d_alloc.dma_paddr +
        offsetof(struct ubsec_dmachunk, d_mcr)));
#endif /* UBSEC_DEBUG */
    return;

feed1:
    while (!BSD_SIMPLEQ_EMPTY(&sc->sc_queue)) {
        stat = READ_REG(sc, BS_STAT);

        if (stat & (BS_STAT_MCR1_FULL | BS_STAT_DMAERR)) {
            if(stat & BS_STAT_DMAERR) {
                ubsec_totalreset(sc);
                ubsecstats.hst_dmaerr++;
            }
            break;
        }

        q = BSD_SIMPLEQ_FIRST(&sc->sc_queue);

#if 0
        bus_dmamap_sync(sc->sc_dmat, q->q_src_map,
            0, q->q_src_map->dm_mapsize, BUS_DMASYNC_PREWRITE);
        if (q->q_dst_map != NULL)
            bus_dmamap_sync(sc->sc_dmat, q->q_dst_map,
                0, q->q_dst_map->dm_mapsize, BUS_DMASYNC_PREREAD);
        bus_dmamap_sync(sc->sc_dmat, q->q_dma->d_alloc.dma_map,
            0, q->q_dma->d_alloc.dma_map->dm_mapsize,
            BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
#endif

        WRITE_REG(sc, BS_MCR1, q->q_dma->d_alloc.dma_paddr +
            offsetof(struct ubsec_dmachunk, d_mcr));
#ifdef UBSEC_VERBOSE_DEBUG
        DPRINTF("feed (2): q->chip %p %08x %08x\n", q, 
            (u_int32_t)q->q_dma->d_alloc.dma_paddr,
            (u_int32_t)(q->q_dma->d_alloc.dma_paddr +
            offsetof(struct ubsec_dmachunk, d_mcr)));
#endif /* UBSEC_DEBUG */
        BSD_SIMPLEQ_REMOVE_HEAD(&sc->sc_queue, q_next);
        --sc->sc_nqueue;
        BSD_SIMPLEQ_INSERT_TAIL(&sc->sc_qchip, q, q_next);
    }
}

/*
 * Allocate a new 'session' and return an encoded session id.  'sidp'
 * contains our registration id, and should contain an encoded session
 * id on successful allocation.
 */
static int
ubsec_newsession(device_t dev, u_int32_t *sidp, struct cryptoini *cri)
{
    struct cryptoini *c, *encini = NULL, *macini = NULL;
    struct ubsec_softc *sc = NULL;
    struct ubsec_session *ses = NULL;
    int sesn, i;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    if (sidp == NULL || cri == NULL)
        return (EINVAL);

    sc = device_get_softc(dev);

    if (sc == NULL)
        return (EINVAL);

    for (c = cri; c != NULL; c = c->cri_next) {
        if (c->cri_alg == CRYPTO_MD5_HMAC ||
            c->cri_alg == CRYPTO_SHA1_HMAC) {
            if (macini)
                return (EINVAL);
            macini = c;
        } else if (c->cri_alg == CRYPTO_DES_CBC ||
            c->cri_alg == CRYPTO_3DES_CBC ||
            c->cri_alg == CRYPTO_AES_CBC) {
            if (encini)
                return (EINVAL);
            encini = c;
        } else
            return (EINVAL);
    }
    if (encini == NULL && macini == NULL)
        return (EINVAL);

    if (sc->sc_sessions == NULL) {
        ses = sc->sc_sessions = (struct ubsec_session *)kmalloc(
            sizeof(struct ubsec_session), SLAB_ATOMIC);
        if (ses == NULL)
            return (ENOMEM);
        memset(ses, 0, sizeof(struct ubsec_session));
        sesn = 0;
        sc->sc_nsessions = 1;
    } else {
        for (sesn = 0; sesn < sc->sc_nsessions; sesn++) {
            if (sc->sc_sessions[sesn].ses_used == 0) {
                ses = &sc->sc_sessions[sesn];
                break;
            }
        }

        if (ses == NULL) {
            sesn = sc->sc_nsessions;
            ses = (struct ubsec_session *)kmalloc((sesn + 1) *
                sizeof(struct ubsec_session), SLAB_ATOMIC);
            if (ses == NULL)
                return (ENOMEM);
            memset(ses, 0, (sesn + 1) * sizeof(struct ubsec_session));
            bcopy(sc->sc_sessions, ses, sesn *
                sizeof(struct ubsec_session));
            bzero(sc->sc_sessions, sesn *
                sizeof(struct ubsec_session));
            kfree(sc->sc_sessions);
            sc->sc_sessions = ses;
            ses = &sc->sc_sessions[sesn];
            sc->sc_nsessions++;
        }
    }

    bzero(ses, sizeof(struct ubsec_session));
    ses->ses_used = 1;
    if (encini) {
        /* get an IV */
        /* XXX may read fewer than requested */
        read_random(ses->ses_iv, sizeof(ses->ses_iv));

        /* Go ahead and compute key in ubsec's byte order */
        if (encini->cri_alg == CRYPTO_DES_CBC) {
            /* DES uses the same key three times:
             * 1st encrypt -> 2nd decrypt -> 3nd encrypt */
            bcopy(encini->cri_key, &ses->ses_key[0], 8);
            bcopy(encini->cri_key, &ses->ses_key[2], 8);
            bcopy(encini->cri_key, &ses->ses_key[4], 8);
            ses->ses_keysize = 192; /* Fake! Actually its only 64bits .. 
                                       oh no it is even less: 54bits. */
        } else if(encini->cri_alg == CRYPTO_3DES_CBC) {
            bcopy(encini->cri_key, ses->ses_key, 24);
            ses->ses_keysize = 192;
        } else if(encini->cri_alg == CRYPTO_AES_CBC) {
            ses->ses_keysize = encini->cri_klen;

            if (ses->ses_keysize != 128 &&
                ses->ses_keysize != 192 &&
                ses->ses_keysize != 256)
            {
                DPRINTF("unsupported AES key size: %d\n", ses->ses_keysize);
                return (EINVAL);
            }
            bcopy(encini->cri_key, ses->ses_key, (ses->ses_keysize / 8));
        }

        /* Hardware requires the keys in little endian byte order */
        for (i=0; i < (ses->ses_keysize / 32); i++)
            SWAP32(ses->ses_key[i]);
    }

    if (macini) {
        ses->ses_mlen = macini->cri_mlen;

        if (ses->ses_mlen == 0 ||
            ses->ses_mlen > SHA1_HASH_LEN) {

            if (macini->cri_alg == CRYPTO_MD5_HMAC ||
                macini->cri_alg == CRYPTO_SHA1_HMAC)
            {
                ses->ses_mlen = DEFAULT_HMAC_LEN;
            } else
            {
                /*
                 * Reserved for future usage. MD5/SHA1 calculations have
                 * different hash sizes.
                 */
                printk(KERN_ERR DRV_MODULE_NAME ": unsupported hash operation with mac/hash len: %d\n", ses->ses_mlen);
                return (EINVAL);
            }
            
        }

        if (macini->cri_key != NULL) {
            ubsec_setup_mackey(ses, macini->cri_alg, macini->cri_key,
                macini->cri_klen / 8);
        }
    }

    *sidp = UBSEC_SID(device_get_unit(sc->sc_dev), sesn);
    return (0);
}

/*
 * Deallocate a session.
 */
static int
ubsec_freesession(device_t dev, u_int64_t tid)
{
    struct ubsec_softc *sc = device_get_softc(dev);
    int session;
    u_int32_t sid = ((u_int32_t)tid) & 0xffffffff;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    if (sc == NULL)
        return (EINVAL);

    session = UBSEC_SESSION(sid);
    if (session < sc->sc_nsessions) {
        bzero(&sc->sc_sessions[session], sizeof(sc->sc_sessions[session]));
        return (0);
    } else
        return (EINVAL);
}

static int
ubsec_process(device_t dev, struct cryptop *crp, int hint)
{
    struct ubsec_q *q = NULL;
    int err = 0, i, j, nicealign;
    struct ubsec_softc *sc = device_get_softc(dev);
    struct cryptodesc *crd1, *crd2, *maccrd, *enccrd;
    int encoffset = 0, macoffset = 0, cpskip, cpoffset;
    int sskip, dskip, stheend, dtheend, ivsize = 8;
    int16_t coffset;
    struct ubsec_session *ses;
    struct ubsec_generic_ctx ctx;
    struct ubsec_dma *dmap = NULL;
    unsigned long flags;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    if (unlikely(crp == NULL || crp->crp_callback == NULL)) {
        ubsecstats.hst_invalid++;
        return (EINVAL);
    }

    if (unlikely(sc == NULL))
        return (EINVAL);

#ifdef UBSEC_VERBOSE_DEBUG
    DPRINTF("spin_lock_irqsave\n");
#endif
    spin_lock_irqsave(&sc->sc_ringmtx, flags);
    //spin_lock_irq(&sc->sc_ringmtx);

    if (BSD_SIMPLEQ_EMPTY(&sc->sc_freequeue)) {
        ubsecstats.hst_queuefull++;
#ifdef UBSEC_VERBOSE_DEBUG
        DPRINTF("spin_unlock_irqrestore\n");
#endif
        spin_unlock_irqrestore(&sc->sc_ringmtx, flags);
        //spin_unlock_irq(&sc->sc_ringmtx);
        err = ENOMEM;
        goto errout2;
    }

    q = BSD_SIMPLEQ_FIRST(&sc->sc_freequeue);
    BSD_SIMPLEQ_REMOVE_HEAD(&sc->sc_freequeue, q_next);
#ifdef UBSEC_VERBOSE_DEBUG
    DPRINTF("spin_unlock_irqrestore\n");
#endif
    spin_unlock_irqrestore(&sc->sc_ringmtx, flags);
    //spin_unlock_irq(&sc->sc_ringmtx);

    dmap = q->q_dma; /* Save dma pointer */
    bzero(q, sizeof(struct ubsec_q));
    bzero(&ctx, sizeof(ctx));

    q->q_sesn = UBSEC_SESSION(crp->crp_sid);
    q->q_dma = dmap;
    ses = &sc->sc_sessions[q->q_sesn];

    if (crp->crp_flags & CRYPTO_F_SKBUF) {
        q->q_src_m = (struct sk_buff *)crp->crp_buf;
        q->q_dst_m = (struct sk_buff *)crp->crp_buf;
    } else if (crp->crp_flags & CRYPTO_F_IOV) {
        q->q_src_io = (struct uio *)crp->crp_buf;
        q->q_dst_io = (struct uio *)crp->crp_buf;
    } else {
        err = EINVAL;
        goto errout;    /* XXX we don't handle contiguous blocks! */
    }

    bzero(&dmap->d_dma->d_mcr, sizeof(struct ubsec_mcr));

    dmap->d_dma->d_mcr.mcr_pkts = htole16(1);
    dmap->d_dma->d_mcr.mcr_flags = 0;
    q->q_crp = crp;

    crd1 = crp->crp_desc;
    if (crd1 == NULL) {
        err = EINVAL;
        goto errout;
    }
    crd2 = crd1->crd_next;

    if (crd2 == NULL) {
        if (crd1->crd_alg == CRYPTO_MD5_HMAC ||
            crd1->crd_alg == CRYPTO_SHA1_HMAC) {
            maccrd = crd1;
            enccrd = NULL;
        } else if (crd1->crd_alg == CRYPTO_DES_CBC ||
            crd1->crd_alg == CRYPTO_3DES_CBC || 
            crd1->crd_alg == CRYPTO_AES_CBC) {
            maccrd = NULL;
            enccrd = crd1;
        } else {
            err = EINVAL;
            goto errout;
        }
    } else {
        if ((crd1->crd_alg == CRYPTO_MD5_HMAC ||
            crd1->crd_alg == CRYPTO_SHA1_HMAC) &&
            (crd2->crd_alg == CRYPTO_DES_CBC ||
            crd2->crd_alg == CRYPTO_3DES_CBC ||
            crd2->crd_alg == CRYPTO_AES_CBC) &&
            ((crd2->crd_flags & CRD_F_ENCRYPT) == 0)) {
            maccrd = crd1;
            enccrd = crd2;
        } else if ((crd1->crd_alg == CRYPTO_DES_CBC ||
            crd1->crd_alg == CRYPTO_3DES_CBC ||
            crd1->crd_alg == CRYPTO_AES_CBC) &&
            (crd2->crd_alg == CRYPTO_MD5_HMAC ||
            crd2->crd_alg == CRYPTO_SHA1_HMAC) &&
            (crd1->crd_flags & CRD_F_ENCRYPT)) {
            enccrd = crd1;
            maccrd = crd2;
        } else {
            /*
             * We cannot order the ubsec as requested
             */
            printk(KERN_ERR DRV_MODULE_NAME ": got wrong algorithm/signature order.\n");
            err = EINVAL;
            goto errout;
        }
    }

    /* Encryption/Decryption requested */
    if (enccrd) {
        encoffset = enccrd->crd_skip;

        if (enccrd->crd_alg == CRYPTO_DES_CBC ||
            enccrd->crd_alg == CRYPTO_3DES_CBC)
        {
            ctx.pc_flags |= htole16(UBS_PKTCTX_ENC_3DES);
            ctx.pc_type = htole16(UBS_PKTCTX_TYPE_IPSEC_DES);
            ivsize = 8;     /* [3]DES uses 64bit IVs */
        } else {
            ctx.pc_flags |= htole16(UBS_PKTCTX_ENC_AES);
            ctx.pc_type = htole16(UBS_PKTCTX_TYPE_IPSEC_AES);
            ivsize = 16;    /* AES uses 128bit IVs / [3]DES 64bit IVs */

            switch(ses->ses_keysize)
            {
                case 128:
                    ctx.pc_flags |= htole16(UBS_PKTCTX_AES128);
                    break;
                case 192:
                    ctx.pc_flags |= htole16(UBS_PKTCTX_AES192);
                    break;
                case 256:
                    ctx.pc_flags |= htole16(UBS_PKTCTX_AES256);
                    break;
                default:
                    DPRINTF("invalid AES key size: %d\n", ses->ses_keysize);
                    err = EINVAL;
                    goto errout;
            }
        }

        if (enccrd->crd_flags & CRD_F_ENCRYPT) {
            /* Direction: Outbound */

            q->q_flags |= UBSEC_QFLAGS_COPYOUTIV;

            if (enccrd->crd_flags & CRD_F_IV_EXPLICIT) {
                bcopy(enccrd->crd_iv, ctx.pc_iv, ivsize);
            } else {
                for(i=0; i < (ivsize / 4); i++)
                    ctx.pc_iv[i] = ses->ses_iv[i];
            }

            /* If there is no IV in the buffer -> copy it here */
            if ((enccrd->crd_flags & CRD_F_IV_PRESENT) == 0) {
                if (crp->crp_flags & CRYPTO_F_SKBUF)
                    /*
                    m_copyback(q->q_src_m,
                        enccrd->crd_inject,
                        8, ctx.pc_iv);
                    */
                    crypto_copyback(crp->crp_flags, (caddr_t)q->q_src_m,
                        enccrd->crd_inject, ivsize, (caddr_t)ctx.pc_iv);
                else if (crp->crp_flags & CRYPTO_F_IOV)
                    /*
                    cuio_copyback(q->q_src_io,
                        enccrd->crd_inject,
                        8, ctx.pc_iv);
                    */
                    crypto_copyback(crp->crp_flags, (caddr_t)q->q_src_io,
                        enccrd->crd_inject, ivsize, (caddr_t)ctx.pc_iv);
            }
        } else {
            /* Direction: Inbound */

            ctx.pc_flags |= htole16(UBS_PKTCTX_INBOUND);

            if (enccrd->crd_flags & CRD_F_IV_EXPLICIT)
                bcopy(enccrd->crd_iv, ctx.pc_iv, ivsize);
            else if (crp->crp_flags & CRYPTO_F_SKBUF)
                /*
                m_copydata(q->q_src_m, enccrd->crd_inject,
                    8, (caddr_t)ctx.pc_iv);
                */
                crypto_copydata(crp->crp_flags, (caddr_t)q->q_src_m,
                    enccrd->crd_inject, ivsize,
                    (caddr_t)ctx.pc_iv);
            else if (crp->crp_flags & CRYPTO_F_IOV)
                /*
                cuio_copydata(q->q_src_io,
                    enccrd->crd_inject, 8,
                    (caddr_t)ctx.pc_iv);
                */
                crypto_copydata(crp->crp_flags, (caddr_t)q->q_src_io,
                    enccrd->crd_inject, ivsize,
                    (caddr_t)ctx.pc_iv);

        }

        /* Even though key & IV sizes differ from cipher to cipher
         * copy / swap the full array lengths. Let the compiler unroll
         * the loop to increase the cpu pipeline performance... */
        for(i=0; i < 8; i++)
            ctx.pc_key[i] = ses->ses_key[i];
        for(i=0; i < 4; i++)
            SWAP32(ctx.pc_iv[i]);
    }

    /* Authentication requested */
    if (maccrd) {
        macoffset = maccrd->crd_skip;

        if (maccrd->crd_alg == CRYPTO_MD5_HMAC)
            ctx.pc_flags |= htole16(UBS_PKTCTX_AUTH_MD5);
        else
            ctx.pc_flags |= htole16(UBS_PKTCTX_AUTH_SHA1);

        for (i = 0; i < 5; i++) {
            ctx.pc_hminner[i] = ses->ses_hminner[i];
            ctx.pc_hmouter[i] = ses->ses_hmouter[i];

            HTOLE32(ctx.pc_hminner[i]);
            HTOLE32(ctx.pc_hmouter[i]);
        }
    }

    if (enccrd && maccrd) {
        /*
         * ubsec cannot handle packets where the end of encryption
         * and authentication are not the same, or where the
         * encrypted part begins before the authenticated part.
         */
        if (((encoffset + enccrd->crd_len) !=
            (macoffset + maccrd->crd_len)) ||
            (enccrd->crd_skip < maccrd->crd_skip)) {
            err = EINVAL;
            goto errout;
        }
        sskip = maccrd->crd_skip;
        cpskip = dskip = enccrd->crd_skip;
        stheend = maccrd->crd_len;
        dtheend = enccrd->crd_len;
        coffset = enccrd->crd_skip - maccrd->crd_skip;
        cpoffset = cpskip + dtheend;
#ifdef UBSEC_DEBUG
        DPRINTF("mac: skip %d, len %d, inject %d\n",
            maccrd->crd_skip, maccrd->crd_len, maccrd->crd_inject);
        DPRINTF("enc: skip %d, len %d, inject %d\n",
            enccrd->crd_skip, enccrd->crd_len, enccrd->crd_inject);
        DPRINTF("src: skip %d, len %d\n", sskip, stheend);
        DPRINTF("dst: skip %d, len %d\n", dskip, dtheend);
        DPRINTF("ubs: coffset %d, pktlen %d, cpskip %d, cpoffset %d\n",
            coffset, stheend, cpskip, cpoffset);
#endif
    } else {
        cpskip = dskip = sskip = macoffset + encoffset;
        dtheend = stheend = (enccrd)?enccrd->crd_len:maccrd->crd_len;
        cpoffset = cpskip + dtheend;
        coffset = 0;
    }
    ctx.pc_offset = htole16(coffset >> 2);

#if 0
    if (bus_dmamap_create(sc->sc_dmat, 0xfff0, UBS_MAX_SCATTER,
        0xfff0, 0, BUS_DMA_NOWAIT, &q->q_src_map) != 0) {
        err = ENOMEM;
        goto errout;
    }
#endif

    if (crp->crp_flags & CRYPTO_F_SKBUF) {
#if 0
        if (bus_dmamap_load_mbuf(sc->sc_dmat, q->q_src_map,
            q->q_src_m, BUS_DMA_NOWAIT) != 0) {
            bus_dmamap_destroy(sc->sc_dmat, q->q_src_map);
            q->q_src_map = NULL;
            err = ENOMEM;
            goto errout;
        }
#endif
        err = dma_map_skb(sc, q->q_src_map, q->q_src_m, &q->q_src_len);
        if (unlikely(err != 0))
            goto errout;

    } else if (crp->crp_flags & CRYPTO_F_IOV) {
#if 0
        if (bus_dmamap_load_uio(sc->sc_dmat, q->q_src_map,
            q->q_src_io, BUS_DMA_NOWAIT) != 0) {
            bus_dmamap_destroy(sc->sc_dmat, q->q_src_map);
            q->q_src_map = NULL;
            err = ENOMEM;
            goto errout;
        }
#endif
        err = dma_map_uio(sc, q->q_src_map, q->q_src_io, &q->q_src_len);
        if (unlikely(err != 0))
           goto errout;
    }

    /* 
     * Check alignment 
     */
    nicealign = ubsec_dmamap_aligned(sc, q->q_src_map, q->q_src_len);

    dmap->d_dma->d_mcr.mcr_pktlen = htole16(stheend);

#ifdef UBSEC_DEBUG
    DPRINTF("src skip: %d\n", sskip);
#endif
    for (i = j = 0; i < q->q_src_len; i++) {
        struct ubsec_pktbuf *pb;
        size_t packl = q->q_src_map[i].dma_size;
        dma_addr_t packp = q->q_src_map[i].dma_paddr;

        if (sskip >= packl) {
            sskip -= packl;
            continue;
        }

        packl -= sskip;
        packp += sskip;
        sskip = 0;

        /* maximum fragment size is 0xfffc */
        if (packl > 0xfffc) {
            DPRINTF("Error: fragment size is bigger than 0xfffc.\n");
            err = EIO;
            goto errout;
        }

        if (j == 0)
            pb = &dmap->d_dma->d_mcr.mcr_ipktbuf;
        else
            pb = &dmap->d_dma->d_sbuf[j - 1];

        pb->pb_addr = htole32(packp);

        if (stheend) {
            if (packl > stheend) {
                pb->pb_len = htole32(stheend);
                stheend = 0;
            } else {
                pb->pb_len = htole32(packl);
                stheend -= packl;
            }
        } else
            pb->pb_len = htole32(packl);

        if ((i + 1) == q->q_src_len)
            pb->pb_next = 0;
        else
            pb->pb_next = htole32(dmap->d_alloc.dma_paddr +
                offsetof(struct ubsec_dmachunk, d_sbuf[j]));
        j++;
    }

    if (enccrd == NULL && maccrd != NULL) {
        /* Authentication only */
        dmap->d_dma->d_mcr.mcr_opktbuf.pb_addr = 0;
        dmap->d_dma->d_mcr.mcr_opktbuf.pb_len = 0;
        dmap->d_dma->d_mcr.mcr_opktbuf.pb_next =
            htole32(dmap->d_alloc.dma_paddr +
            offsetof(struct ubsec_dmachunk, d_macbuf[0]));
#ifdef UBSEC_DEBUG
        DPRINTF("opkt: %x %x %x\n",
            dmap->d_dma->d_mcr.mcr_opktbuf.pb_addr,
            dmap->d_dma->d_mcr.mcr_opktbuf.pb_len,
            dmap->d_dma->d_mcr.mcr_opktbuf.pb_next);
#endif
    } else {
        if (crp->crp_flags & CRYPTO_F_IOV) {
            if (!nicealign) {
                err = EINVAL;
                goto errout;
            }
#if 0
            if (bus_dmamap_create(sc->sc_dmat, 0xfff0,
                UBS_MAX_SCATTER, 0xfff0, 0, BUS_DMA_NOWAIT,
                &q->q_dst_map) != 0) {
                err = ENOMEM;
                goto errout;
            }
            if (bus_dmamap_load_uio(sc->sc_dmat, q->q_dst_map,
                q->q_dst_io, BUS_DMA_NOWAIT) != 0) {
                bus_dmamap_destroy(sc->sc_dmat, q->q_dst_map);
                q->q_dst_map = NULL;
                goto errout;
            }
#endif

            /* HW shall copy the result into the source memory */
            for(i = 0; i < q->q_src_len; i++)
                q->q_dst_map[i] = q->q_src_map[i];

            q->q_dst_len = q->q_src_len;
            q->q_has_dst = 0;

        } else if (crp->crp_flags & CRYPTO_F_SKBUF) {
            if (nicealign) {

                /* HW shall copy the result into the source memory */
                q->q_dst_m = q->q_src_m;
                for(i = 0; i < q->q_src_len; i++)
                    q->q_dst_map[i] = q->q_src_map[i];

                q->q_dst_len = q->q_src_len;
                q->q_has_dst = 0;

            } else {
#ifdef NOTYET
                int totlen, len;
                struct sk_buff *m, *top, **mp;

                totlen = q->q_src_map->dm_mapsize;
                if (q->q_src_m->m_flags & M_PKTHDR) {
                    len = MHLEN;
                    MGETHDR(m, M_DONTWAIT, MT_DATA);
                } else {
                    len = MLEN;
                    MGET(m, M_DONTWAIT, MT_DATA);
                }
                if (m == NULL) {
                    err = ENOMEM;
                    goto errout;
                }
                if (len == MHLEN)
                    M_DUP_PKTHDR(m, q->q_src_m);
                if (totlen >= MINCLSIZE) {
                    MCLGET(m, M_DONTWAIT);
                    if (m->m_flags & M_EXT)
                        len = MCLBYTES;
                }
                m->m_len = len;
                top = NULL;
                mp = &top;

                while (totlen > 0) {
                    if (top) {
                        MGET(m, M_DONTWAIT, MT_DATA);
                        if (m == NULL) {
                            m_freem(top);
                            err = ENOMEM;
                            goto errout;
                        }
                        len = MLEN;
                    }
                    if (top && totlen >= MINCLSIZE) {
                        MCLGET(m, M_DONTWAIT);
                        if (m->m_flags & M_EXT)
                            len = MCLBYTES;
                    }
                    m->m_len = len = min(totlen, len);
                    totlen -= len;
                    *mp = m;
                    mp = &m->m_next;
                }
                q->q_dst_m = top;
                ubsec_mcopy(q->q_src_m, q->q_dst_m,
                    cpskip, cpoffset);
                if (bus_dmamap_create(sc->sc_dmat, 0xfff0,
                    UBS_MAX_SCATTER, 0xfff0, 0, BUS_DMA_NOWAIT,
                    &q->q_dst_map) != 0) {
                    err = ENOMEM;
                    goto errout;
                }
                if (bus_dmamap_load_mbuf(sc->sc_dmat,
                    q->q_dst_map, q->q_dst_m,
                    BUS_DMA_NOWAIT) != 0) {
                    bus_dmamap_destroy(sc->sc_dmat,
                    q->q_dst_map);
                    q->q_dst_map = NULL;
                    err = ENOMEM;
                    goto errout;
                }
#else
                device_printf(sc->sc_dev,
                    "%s,%d: CRYPTO_F_SKBUF unaligned not implemented\n",
                    __FILE__, __LINE__);
                err = EINVAL;
                goto errout;
#endif
            }
        } else {
            err = EINVAL;
            goto errout;
        }

#ifdef UBSEC_DEBUG
        DPRINTF("dst skip: %d\n", dskip);
#endif
        for (i = j = 0; i < q->q_dst_len; i++) {
            struct ubsec_pktbuf *pb;
            size_t packl = q->q_dst_map[i].dma_size;
            dma_addr_t packp = q->q_dst_map[i].dma_paddr;

            if (dskip >= packl) {
                dskip -= packl;
                continue;
            }

            packl -= dskip;
            packp += dskip;
            dskip = 0;

            if (packl > 0xfffc) {
                DPRINTF("Error: fragment size is bigger than 0xfffc.\n");
                err = EIO;
                goto errout;
            }

            if (j == 0)
                pb = &dmap->d_dma->d_mcr.mcr_opktbuf;
            else
                pb = &dmap->d_dma->d_dbuf[j - 1];

            pb->pb_addr = htole32(packp);

            if (dtheend) {
                if (packl > dtheend) {
                    pb->pb_len = htole32(dtheend);
                    dtheend = 0;
                } else {
                    pb->pb_len = htole32(packl);
                    dtheend -= packl;
                }
            } else
                pb->pb_len = htole32(packl);

            if ((i + 1) == q->q_dst_len) {
                if (maccrd)
                    /* Authentication:
                     * The last fragment of the output buffer 
                     * contains the HMAC. */
                    pb->pb_next = htole32(dmap->d_alloc.dma_paddr +
                        offsetof(struct ubsec_dmachunk, d_macbuf[0]));
                else
                    pb->pb_next = 0;
            } else
                pb->pb_next = htole32(dmap->d_alloc.dma_paddr +
                    offsetof(struct ubsec_dmachunk, d_dbuf[j]));
            j++;
        }
    }

    dmap->d_dma->d_mcr.mcr_cmdctxp = htole32(dmap->d_alloc.dma_paddr +
        offsetof(struct ubsec_dmachunk, d_ctx));

    if (sc->sc_flags & UBS_FLAGS_LONGCTX) {
        /* new Broadcom cards with dynamic long command context structure */

        if (enccrd != NULL &&
            enccrd->crd_alg == CRYPTO_AES_CBC)
        {
            struct ubsec_pktctx_aes128 *ctxaes128;    
            struct ubsec_pktctx_aes192 *ctxaes192;    
            struct ubsec_pktctx_aes256 *ctxaes256;    

            switch(ses->ses_keysize)
            {
                /* AES 128bit */
                case 128:
                ctxaes128 = (struct ubsec_pktctx_aes128 *)
                    (dmap->d_alloc.dma_vaddr + 
                    offsetof(struct ubsec_dmachunk, d_ctx));

                ctxaes128->pc_len = htole16(sizeof(struct ubsec_pktctx_aes128));
                ctxaes128->pc_type = ctx.pc_type;
                ctxaes128->pc_flags = ctx.pc_flags;
                ctxaes128->pc_offset = ctx.pc_offset;
                for (i = 0; i < 4; i++)
                    ctxaes128->pc_aeskey[i] = ctx.pc_key[i];
                for (i = 0; i < 5; i++)
                    ctxaes128->pc_hminner[i] = ctx.pc_hminner[i];
                for (i = 0; i < 5; i++)
                    ctxaes128->pc_hmouter[i] = ctx.pc_hmouter[i];
                for (i = 0; i < 4; i++)
                    ctxaes128->pc_iv[i] = ctx.pc_iv[i];
                break;

                /* AES 192bit */
                case 192:
                ctxaes192 = (struct ubsec_pktctx_aes192 *)
                    (dmap->d_alloc.dma_vaddr + 
                    offsetof(struct ubsec_dmachunk, d_ctx));

                ctxaes192->pc_len = htole16(sizeof(struct ubsec_pktctx_aes192));
                ctxaes192->pc_type = ctx.pc_type;
                ctxaes192->pc_flags = ctx.pc_flags;
                ctxaes192->pc_offset = ctx.pc_offset;
                for (i = 0; i < 6; i++)
                    ctxaes192->pc_aeskey[i] = ctx.pc_key[i];
                for (i = 0; i < 5; i++)
                    ctxaes192->pc_hminner[i] = ctx.pc_hminner[i];
                for (i = 0; i < 5; i++)
                    ctxaes192->pc_hmouter[i] = ctx.pc_hmouter[i];
                for (i = 0; i < 4; i++)
                    ctxaes192->pc_iv[i] = ctx.pc_iv[i];
                break;

                /* AES 256bit */
                case 256:
                ctxaes256 = (struct ubsec_pktctx_aes256 *)
                    (dmap->d_alloc.dma_vaddr + 
                    offsetof(struct ubsec_dmachunk, d_ctx));

                ctxaes256->pc_len = htole16(sizeof(struct ubsec_pktctx_aes256));
                ctxaes256->pc_type = ctx.pc_type;
                ctxaes256->pc_flags = ctx.pc_flags;
                ctxaes256->pc_offset = ctx.pc_offset;
                for (i = 0; i < 8; i++)
                    ctxaes256->pc_aeskey[i] = ctx.pc_key[i];
                for (i = 0; i < 5; i++)
                    ctxaes256->pc_hminner[i] = ctx.pc_hminner[i];
                for (i = 0; i < 5; i++)
                    ctxaes256->pc_hmouter[i] = ctx.pc_hmouter[i];
                for (i = 0; i < 4; i++)
                    ctxaes256->pc_iv[i] = ctx.pc_iv[i];
                break;

            }
        } else {
            /* 
             * [3]DES / MD5_HMAC / SHA1_HMAC
             *
             * MD5_HMAC / SHA1_HMAC can use the IPSEC 3DES operation without
             * encryption.
             */
            struct ubsec_pktctx_des *ctxdes;

            ctxdes = (struct ubsec_pktctx_des *)(dmap->d_alloc.dma_vaddr +
                offsetof(struct ubsec_dmachunk, d_ctx));
            
            ctxdes->pc_len = htole16(sizeof(struct ubsec_pktctx_des));
            ctxdes->pc_type = ctx.pc_type;
            ctxdes->pc_flags = ctx.pc_flags;
            ctxdes->pc_offset = ctx.pc_offset;
            for (i = 0; i < 6; i++)
                ctxdes->pc_deskey[i] = ctx.pc_key[i];
            for (i = 0; i < 5; i++)
                ctxdes->pc_hminner[i] = ctx.pc_hminner[i];
            for (i = 0; i < 5; i++)
                ctxdes->pc_hmouter[i] = ctx.pc_hmouter[i];   
            ctxdes->pc_iv[0] = ctx.pc_iv[0];
            ctxdes->pc_iv[1] = ctx.pc_iv[1];
        }
    } else
    {
        /* old Broadcom card with fixed small command context structure */

        /*
         * [3]DES / MD5_HMAC / SHA1_HMAC
         */
        struct ubsec_pktctx *ctxs;

        ctxs = (struct ubsec_pktctx *)(dmap->d_alloc.dma_vaddr +
                    offsetof(struct ubsec_dmachunk, d_ctx));
 
        /* transform generic context into small context */
        for (i = 0; i < 6; i++)
            ctxs->pc_deskey[i] = ctx.pc_key[i];
        for (i = 0; i < 5; i++)
            ctxs->pc_hminner[i] = ctx.pc_hminner[i];
        for (i = 0; i < 5; i++)
            ctxs->pc_hmouter[i] = ctx.pc_hmouter[i];
        ctxs->pc_iv[0] = ctx.pc_iv[0];
        ctxs->pc_iv[1] = ctx.pc_iv[1];
        ctxs->pc_flags = ctx.pc_flags;
        ctxs->pc_offset = ctx.pc_offset;
    }

#ifdef UBSEC_VERBOSE_DEBUG
    DPRINTF("spin_lock_irqsave\n");
#endif
    spin_lock_irqsave(&sc->sc_ringmtx, flags);
    //spin_lock_irq(&sc->sc_ringmtx);

    BSD_SIMPLEQ_INSERT_TAIL(&sc->sc_queue, q, q_next);
    sc->sc_nqueue++;
    ubsecstats.hst_ipackets++;
    ubsecstats.hst_ibytes += stheend;
    ubsec_feed(sc);

#ifdef UBSEC_VERBOSE_DEBUG
    DPRINTF("spin_unlock_irqrestore\n");
#endif
    spin_unlock_irqrestore(&sc->sc_ringmtx, flags);
    //spin_unlock_irq(&sc->sc_ringmtx);
    
    return (0);

errout:
    if (q != NULL) {
#ifdef NOTYET
        if ((q->q_dst_m != NULL) && (q->q_src_m != q->q_dst_m))
            m_freem(q->q_dst_m);
#endif

        if ((q->q_has_dst == 1) && q->q_dst_len > 0) {
#if 0
            bus_dmamap_unload(sc->sc_dmat, q->q_dst_map);
            bus_dmamap_destroy(sc->sc_dmat, q->q_dst_map);
#endif
            dma_unmap(sc, q->q_dst_map, q->q_dst_len);
        }
        if (q->q_src_len > 0) {
#if 0
            bus_dmamap_unload(sc->sc_dmat, q->q_src_map);
            bus_dmamap_destroy(sc->sc_dmat, q->q_src_map);
#endif
            dma_unmap(sc, q->q_src_map, q->q_src_len);
        }

#ifdef UBSEC_VERBOSE_DEBUG
        DPRINTF("spin_lock_irqsave\n");
#endif
        spin_lock_irqsave(&sc->sc_ringmtx, flags);
        //spin_lock_irq(&sc->sc_ringmtx);

        BSD_SIMPLEQ_INSERT_TAIL(&sc->sc_freequeue, q, q_next);

#ifdef UBSEC_VERBOSE_DEBUG
       DPRINTF("spin_unlock_irqrestore\n");
#endif
        spin_unlock_irqrestore(&sc->sc_ringmtx, flags);
        //spin_unlock_irq(&sc->sc_ringmtx);

    }
    if (err == EINVAL)
        ubsecstats.hst_invalid++;
    else
        ubsecstats.hst_nomem++;
errout2:
    crp->crp_etype = err;
    crypto_done(crp);

#ifdef UBSEC_DEBUG
    DPRINTF("%s() err = %x\n", __FUNCTION__, err);
#endif

    return (0);
}

void
ubsec_callback(struct ubsec_softc *sc, struct ubsec_q *q)
{
    struct cryptop *crp = (struct cryptop *)q->q_crp;
    struct cryptodesc *crd;
    struct ubsec_dma *dmap = q->q_dma;
    int ivsize = 8;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    ubsecstats.hst_opackets++;
    ubsecstats.hst_obytes += dmap->d_alloc.dma_size;

#if 0
    bus_dmamap_sync(sc->sc_dmat, dmap->d_alloc.dma_map, 0,
        dmap->d_alloc.dma_map->dm_mapsize,
        BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);
    if (q->q_dst_map != NULL && q->q_dst_map != q->q_src_map) {
        bus_dmamap_sync(sc->sc_dmat, q->q_dst_map,
            0, q->q_dst_map->dm_mapsize, BUS_DMASYNC_POSTREAD);
        bus_dmamap_unload(sc->sc_dmat, q->q_dst_map);
        bus_dmamap_destroy(sc->sc_dmat, q->q_dst_map);
    }
    bus_dmamap_sync(sc->sc_dmat, q->q_src_map,
        0, q->q_src_map->dm_mapsize, BUS_DMASYNC_POSTWRITE);
    bus_dmamap_unload(sc->sc_dmat, q->q_src_map);
    bus_dmamap_destroy(sc->sc_dmat, q->q_src_map);
#endif

    if ((q->q_has_dst == 1) && q->q_dst_len > 0)
        dma_unmap(sc, q->q_dst_map, q->q_dst_len);

    dma_unmap(sc, q->q_src_map, q->q_src_len);

#ifdef NOTYET
    if ((crp->crp_flags & CRYPTO_F_SKBUF) && (q->q_src_m != q->q_dst_m)) {
        m_freem(q->q_src_m);
        crp->crp_buf = (caddr_t)q->q_dst_m;
    }
#endif

    /* copy out IV for future use */
    if (q->q_flags & UBSEC_QFLAGS_COPYOUTIV) {
        for (crd = crp->crp_desc; crd; crd = crd->crd_next) {
            if (crd->crd_alg != CRYPTO_DES_CBC &&
                crd->crd_alg != CRYPTO_3DES_CBC &&
                crd->crd_alg != CRYPTO_AES_CBC)
                continue;

            if (crd->crd_alg == CRYPTO_AES_CBC)
                ivsize = 16;
            else
                ivsize = 8;

            if (crp->crp_flags & CRYPTO_F_SKBUF)
#if 0
                m_copydata((struct sk_buff *)crp->crp_buf,
                    crd->crd_skip + crd->crd_len - 8, 8,
                    (caddr_t)sc->sc_sessions[q->q_sesn].ses_iv);
#endif
                crypto_copydata(crp->crp_flags, (caddr_t)crp->crp_buf,
                    crd->crd_skip + crd->crd_len - ivsize, ivsize,
                    (caddr_t)sc->sc_sessions[q->q_sesn].ses_iv);

            else if (crp->crp_flags & CRYPTO_F_IOV) {
#if 0
                cuio_copydata((struct uio *)crp->crp_buf,
                    crd->crd_skip + crd->crd_len - 8, 8,
                    (caddr_t)sc->sc_sessions[q->q_sesn].ses_iv);
#endif
                crypto_copydata(crp->crp_flags, (caddr_t)crp->crp_buf,
                    crd->crd_skip + crd->crd_len - ivsize, ivsize,
                    (caddr_t)sc->sc_sessions[q->q_sesn].ses_iv);
                    
            }
            break;
        }
    }

    for (crd = crp->crp_desc; crd; crd = crd->crd_next) {
        if (crd->crd_alg != CRYPTO_MD5_HMAC &&
            crd->crd_alg != CRYPTO_SHA1_HMAC)
            continue;
#if 0
        if (crp->crp_flags & CRYPTO_F_SKBUF)
            m_copyback((struct sk_buff *)crp->crp_buf,
                crd->crd_inject, 12,
                dmap->d_dma->d_macbuf);
#endif
#if 0
            /* BUG? it does not honor the mac len.. */
            crypto_copyback(crp->crp_flags, crp->crp_buf,
                crd->crd_inject, 12,
                (caddr_t)dmap->d_dma->d_macbuf);
#endif
            crypto_copyback(crp->crp_flags, crp->crp_buf,
                crd->crd_inject, 
                sc->sc_sessions[q->q_sesn].ses_mlen,
                (caddr_t)dmap->d_dma->d_macbuf);
#if 0
        else if (crp->crp_flags & CRYPTO_F_IOV && crp->crp_mac)
            bcopy((caddr_t)dmap->d_dma->d_macbuf,
                crp->crp_mac, 12);
#endif
        break;
    }
    BSD_SIMPLEQ_INSERT_TAIL(&sc->sc_freequeue, q, q_next);
    crypto_done(crp);
}

void
ubsec_mcopy(struct sk_buff *srcm, struct sk_buff *dstm, int hoffset, int toffset)
{
    int i, j, dlen, slen;
    caddr_t dptr, sptr;

    j = 0;
    sptr = srcm->data;
    slen = srcm->len;
    dptr = dstm->data;
    dlen = dstm->len;

    while (1) {
        for (i = 0; i < min(slen, dlen); i++) {
            if (j < hoffset || j >= toffset)
                *dptr++ = *sptr++;
            slen--;
            dlen--;
            j++;
        }
        if (slen == 0) {
            srcm = srcm->next;
            if (srcm == NULL)
                return;
            sptr = srcm->data;
            slen = srcm->len;
        }
        if (dlen == 0) {
            dstm = dstm->next;
            if (dstm == NULL)
                return;
            dptr = dstm->data;
            dlen = dstm->len;
        }
    }
}

int
ubsec_dma_malloc(struct ubsec_softc *sc, struct ubsec_dma_alloc *dma, 
    size_t size, int mapflags)
{
    dma->dma_vaddr = dma_alloc_coherent(sc->sc_dv, 
        size, &dma->dma_paddr, GFP_KERNEL);

    if (likely(dma->dma_vaddr))
    {
        dma->dma_size = size;
        return (0);
    }

    DPRINTF("could not allocate %d bytes of coherent memory.\n", size);

    return (1);
}

void
ubsec_dma_free(struct ubsec_softc *sc, struct ubsec_dma_alloc *dma)
{
    dma_free_coherent(sc->sc_dv, dma->dma_size, dma->dma_vaddr, 
        dma->dma_paddr);
}

/*
 * Resets the board.  Values in the regesters are left as is
 * from the reset (i.e. initial values are assigned elsewhere).
 */
void
ubsec_reset_board(struct ubsec_softc *sc)
{
    volatile u_int32_t ctrl;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif
    DPRINTF("Send reset signal to chip.\n");

    ctrl = READ_REG(sc, BS_CTRL);
    ctrl |= BS_CTRL_RESET;
    WRITE_REG(sc, BS_CTRL, ctrl);

    /*
     * Wait aprox. 30 PCI clocks = 900 ns = 0.9 us
     */
    DELAY(10);
}

/*
 * Init Broadcom registers
 */
void
ubsec_init_board(struct ubsec_softc *sc)
{
    u_int32_t ctrl;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif
    DPRINTF("Initialize chip.\n");

    ctrl = READ_REG(sc, BS_CTRL);
    ctrl &= ~(BS_CTRL_BE32 | BS_CTRL_BE64);
    ctrl |= BS_CTRL_LITTLE_ENDIAN | BS_CTRL_MCR1INT | BS_CTRL_DMAERR;

    WRITE_REG(sc, BS_CTRL, ctrl);

    /* Set chip capabilities (BCM5365P) */
    sc->sc_flags |= UBS_FLAGS_LONGCTX | UBS_FLAGS_AES;
}

/*
 * Clean up after a chip crash.
 * It is assumed that the caller has spin_lock_irq(sc_ringmtx).
 */
void
ubsec_cleanchip(struct ubsec_softc *sc)
{
    struct ubsec_q *q;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif
    DPRINTF("Clean up queues after chip crash.\n");

    while (!BSD_SIMPLEQ_EMPTY(&sc->sc_qchip)) {
        q = BSD_SIMPLEQ_FIRST(&sc->sc_qchip);
        BSD_SIMPLEQ_REMOVE_HEAD(&sc->sc_qchip, q_next);
        ubsec_free_q(sc, q);
    }
}

/*
 * free a ubsec_q
 * It is assumed that the caller has spin_lock_irq(sc_ringmtx).
 */
int
ubsec_free_q(struct ubsec_softc *sc, struct ubsec_q *q)
{
    struct ubsec_q *q2;
    struct cryptop *crp;
    int npkts;
    int i;

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif

    npkts = q->q_nstacked_mcrs;

    for (i = 0; i < npkts; i++) {
        if(q->q_stacked_mcr[i]) {
            q2 = q->q_stacked_mcr[i];

            if ((q2->q_dst_m != NULL) && (q2->q_src_m != q2->q_dst_m)) 
#ifdef NOTYET
                m_freem(q2->q_dst_m);
#else
                printk(KERN_ERR "%s,%d: SKB not supported\n", __FILE__, __LINE__);
#endif

            crp = (struct cryptop *)q2->q_crp;
            
            BSD_SIMPLEQ_INSERT_TAIL(&sc->sc_freequeue, q2, q_next);
            
            crp->crp_etype = EFAULT;
            crypto_done(crp);
        } else {
            break;
        }
    }

    /*
     * Free header MCR
     */
    if ((q->q_dst_m != NULL) && (q->q_src_m != q->q_dst_m))
#ifdef NOTYET
        m_freem(q->q_dst_m);
#else
        printk(KERN_ERR "%s,%d: SKB not supported\n", __FILE__, __LINE__);
#endif

    crp = (struct cryptop *)q->q_crp;
    
    BSD_SIMPLEQ_INSERT_TAIL(&sc->sc_freequeue, q, q_next);
    
    crp->crp_etype = EFAULT;
    crypto_done(crp);
    return(0);
}

/*
 * Routine to reset the chip and clean up.
 * It is assumed that the caller has spin_lock_irq(sc_ringmtx).
 */
void
ubsec_totalreset(struct ubsec_softc *sc)
{

#ifdef UBSEC_DEBUG
    DPRINTF("%s()\n", __FUNCTION__);
#endif
    DPRINTF("initiate total chip reset.. \n");
    ubsec_reset_board(sc);
    ubsec_init_board(sc);
    ubsec_cleanchip(sc);
}

void
ubsec_dump_pb(struct ubsec_pktbuf *pb)
{
    printf("addr 0x%x (0x%x) next 0x%x\n",
        pb->pb_addr, pb->pb_len, pb->pb_next);
}

void
ubsec_dump_mcr(struct ubsec_mcr *mcr)
{
    struct ubsec_mcr_add *ma;
    int i;

    printf("MCR:\n");
    printf(" pkts: %u, flags 0x%x\n",
        letoh16(mcr->mcr_pkts), letoh16(mcr->mcr_flags));
    ma = (struct ubsec_mcr_add *)&mcr->mcr_cmdctxp;
    for (i = 0; i < letoh16(mcr->mcr_pkts); i++) {
        printf(" %d: ctx 0x%x len 0x%x rsvd 0x%x\n", i,
            letoh32(ma->mcr_cmdctxp), letoh16(ma->mcr_pktlen),
            letoh16(ma->mcr_reserved));
        printf(" %d: ipkt ", i);
        ubsec_dump_pb(&ma->mcr_ipktbuf);
        printf(" %d: opkt ", i);
        ubsec_dump_pb(&ma->mcr_opktbuf);
        ma++;
    }
    printf("END MCR\n");
}

static int __init mod_init(void) {
        return ssb_driver_register(&ubsec_ssb_driver);
}

static void __exit mod_exit(void) {
        ssb_driver_unregister(&ubsec_ssb_driver);
}

module_init(mod_init);
module_exit(mod_exit);

// Meta information
MODULE_AUTHOR("Daniel Mueller <daniel@danm.de>");
MODULE_LICENSE("BSD");
MODULE_DESCRIPTION("OCF driver for BCM5365P IPSec Core");
MODULE_VERSION(DRV_MODULE_VERSION);

