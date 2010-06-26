/*
 * Copyright (C) 2006 Micronas USA
 *
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Effort sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F30602-01-2-0537.
 *
 */

//#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/crypto.h>
#include <linux/mm.h>
#include <linux/skbuff.h>
#include <linux/random.h>
#include <asm/io.h>
#include <asm/delay.h>
//#include <asm/scatterlist.h>
#include <linux/scatterlist.h>
#include <linux/dma-mapping.h>
#include <linux/highmem.h>
#include <cryptodev.h>
#include <uio.h>
#include <aes-7108.h>

/* Runtime mode */
static int c7108_crypto_mode = C7108_AES_CTRL_MODE_CTR;
//static int c7108_crypto_mode = C7108_AES_CTRL_MODE_CBC;

static int32_t c7108_id = -1;
static struct cipher_7108 **c7108_sessions = NULL;
static u_int32_t c7108_sesnum = 0;
static unsigned long iobar;

/* Crypto entry points */
static	int c7108_process(void *, struct cryptop *, int);
static	int c7108_newsession(void *, u_int32_t *, struct cryptoini *);
static	int c7108_freesession(void *, u_int64_t);

/* Globals */
static int debug = 0;
static spinlock_t csr_mutex;

/* Generic controller-based lock */
#define AES_LOCK()\
          spin_lock(&csr_mutex)
#define AES_UNLOCK()\
          spin_unlock(&csr_mutex)

/* 7108 AES register access */
#define c7108_reg_wr8(a,d)   iowrite8(d, (void*)(iobar+(a)))
#define c7108_reg_wr16(a,d)  iowrite16(d, (void*)(iobar+(a)))
#define c7108_reg_wr32(a,d)  iowrite32(d, (void*)(iobar+(a)))
#define c7108_reg_rd8(a)     ioread8((void*)(iobar+(a)))
#define c7108_reg_rd16(a)    ioread16((void*)(iobar+(a)))
#define c7108_reg_rd32(a)    ioread32((void*)(iobar+(a)))

static int 
c7108_xlate_key(int klen, u8* k8ptr, u32* k32ptr)
{
        int i, nw=0;
	nw = ((klen >= 256) ? 8 : (klen >= 192) ? 6 : 4);
	for ( i = 0; i < nw; i++) { 
	    k32ptr[i] =    (k8ptr[i+3] << 24) | (k8ptr[i+2] << 16) | 
		           (k8ptr[i+1] << 8)  | k8ptr[i];
	    
	}
	return 0;
}

static int 
c7108_cache_key(int klen, u32* k32ptr, u8* k8ptr)
{
        int i, nb=0;
	u8* ptr = (u8*)k32ptr;
	nb = ((klen >= 256) ? 32 : (klen >= 192) ? 24 : 16);
	for ( i = 0; i < nb; i++)
	    k8ptr[i] = ptr[i];
	return 0;
}

static int
c7108_aes_setup_dma(u32 src, u32 dst, u32 len)
{
        if (len < 16) {
	    printk("len < 16\n");
	    return -10;
	}
	if (len % 16) {
	    printk("len not multiple of 16\n");
	    return -11;
	}	
	c7108_reg_wr16(C7108_AES_DMA_SRC0_LO, (u16) src);
	c7108_reg_wr16(C7108_AES_DMA_SRC0_HI, (u16)((src & 0xffff0000) >> 16));
	c7108_reg_wr16(C7108_AES_DMA_DST0_LO, (u16) dst);
	c7108_reg_wr16(C7108_AES_DMA_DST0_HI, (u16)((dst & 0xffff0000) >> 16));
	c7108_reg_wr16(C7108_AES_DMA_LEN, (u16) ((len / 16) - 1));

	return 0;
}

static int
c7108_aes_set_hw_iv(u8 iv[16])
{
        c7108_reg_wr16(C7108_AES_IV0_LO, (u16) ((iv[1] << 8) | iv[0]));
	c7108_reg_wr16(C7108_AES_IV0_HI, (u16) ((iv[3] << 8) | iv[2]));
	c7108_reg_wr16(C7108_AES_IV1_LO, (u16) ((iv[5] << 8) | iv[4]));
	c7108_reg_wr16(C7108_AES_IV1_HI, (u16) ((iv[7] << 8) | iv[6]));
	c7108_reg_wr16(C7108_AES_IV2_LO, (u16) ((iv[9] << 8) | iv[8]));
	c7108_reg_wr16(C7108_AES_IV2_HI, (u16) ((iv[11] << 8) | iv[10]));
	c7108_reg_wr16(C7108_AES_IV3_LO, (u16) ((iv[13] << 8) | iv[12]));
	c7108_reg_wr16(C7108_AES_IV3_HI, (u16) ((iv[15] << 8) | iv[14]));

    return 0;
}

static void
c7108_aes_read_dkey(u32 * dkey)
{
        dkey[0] = (c7108_reg_rd16(C7108_AES_EKEY0_HI) << 16) | 
	           c7108_reg_rd16(C7108_AES_EKEY0_LO);
	dkey[1] = (c7108_reg_rd16(C7108_AES_EKEY1_HI) << 16) | 
	           c7108_reg_rd16(C7108_AES_EKEY1_LO);
	dkey[2] = (c7108_reg_rd16(C7108_AES_EKEY2_HI) << 16) | 
	           c7108_reg_rd16(C7108_AES_EKEY2_LO);
	dkey[3] = (c7108_reg_rd16(C7108_AES_EKEY3_HI) << 16) | 
	           c7108_reg_rd16(C7108_AES_EKEY3_LO);
	dkey[4] = (c7108_reg_rd16(C7108_AES_EKEY4_HI) << 16) | 
                   c7108_reg_rd16(C7108_AES_EKEY4_LO);
	dkey[5] = (c7108_reg_rd16(C7108_AES_EKEY5_HI) << 16) | 
                   c7108_reg_rd16(C7108_AES_EKEY5_LO);
	dkey[6] = (c7108_reg_rd16(C7108_AES_EKEY6_HI) << 16) | 
                   c7108_reg_rd16(C7108_AES_EKEY6_LO);
	dkey[7] = (c7108_reg_rd16(C7108_AES_EKEY7_HI) << 16) | 
                   c7108_reg_rd16(C7108_AES_EKEY7_LO);
}

static int
c7108_aes_cipher(int op,
		 u32 dst,
		 u32 src,
		 u32 len,
		 int klen,
		 u16 mode,
		 u32 key[8],
		 u8 iv[16])
{
        int rv = 0, cnt=0;
	u16 ctrl = 0, stat = 0;

	AES_LOCK();

	/* Setup key length */
	if (klen == 128) {
	    ctrl |= C7108_AES_KEY_LEN_128;
	} else if (klen == 192) {
	    ctrl |= C7108_AES_KEY_LEN_192;
	} else if (klen == 256) {
	    ctrl |= C7108_AES_KEY_LEN_256;
	} else {
	    AES_UNLOCK();
	    return -3;
	}
	
	/* Check opcode */
	if (C7108_AES_ENCRYPT == op) {
	    ctrl |= C7108_AES_ENCRYPT;
	} else if (C7108_AES_DECRYPT == op) {
	    ctrl |= C7108_AES_DECRYPT;
	} else {
	    AES_UNLOCK();
	    return -4;
	}
	
	/* check mode */
	if ( (mode != C7108_AES_CTRL_MODE_CBC) &&
	     (mode != C7108_AES_CTRL_MODE_CFB) &&
	     (mode != C7108_AES_CTRL_MODE_OFB) &&
	     (mode != C7108_AES_CTRL_MODE_CTR) && 
	     (mode != C7108_AES_CTRL_MODE_ECB) ) { 
	    AES_UNLOCK();	    
	    return -5;
	}
	
	/* Now set mode */
	ctrl |= mode;
	
	/* For CFB, OFB, and CTR, neither backward key
	 * expansion nor key inversion is required.
	 */
	if ( (C7108_AES_DECRYPT == op) &&  
	     (C7108_AES_CTRL_MODE_CBC == mode ||
	      C7108_AES_CTRL_MODE_ECB == mode ) ){ 

	    /* Program Key */
	    c7108_reg_wr16(C7108_AES_KEY0_LO, (u16) key[4]);
	    c7108_reg_wr16(C7108_AES_KEY0_HI, (u16) (key[4] >> 16));
	    c7108_reg_wr16(C7108_AES_KEY1_LO, (u16) key[5]);
	    c7108_reg_wr16(C7108_AES_KEY1_HI, (u16) (key[5] >> 16));
	    c7108_reg_wr16(C7108_AES_KEY2_LO, (u16) key[6]);
	    c7108_reg_wr16(C7108_AES_KEY2_HI, (u16) (key[6] >> 16));
	    c7108_reg_wr16(C7108_AES_KEY3_LO, (u16) key[7]);
	    c7108_reg_wr16(C7108_AES_KEY3_HI, (u16) (key[7] >> 16));
	    c7108_reg_wr16(C7108_AES_KEY6_LO, (u16) key[2]);
	    c7108_reg_wr16(C7108_AES_KEY6_HI, (u16) (key[2] >> 16));
	    c7108_reg_wr16(C7108_AES_KEY7_LO, (u16) key[3]);
	    c7108_reg_wr16(C7108_AES_KEY7_HI, (u16) (key[3] >> 16));
	    
	    
	    if (192 == klen) { 
		c7108_reg_wr16(C7108_AES_KEY4_LO, (u16) key[7]);
		c7108_reg_wr16(C7108_AES_KEY4_HI, (u16) (key[7] >> 16));
		c7108_reg_wr16(C7108_AES_KEY5_LO, (u16) key[7]);
		c7108_reg_wr16(C7108_AES_KEY5_HI, (u16) (key[7] >> 16));
		
	    } else if (256 == klen) {
		/* 256 */
		c7108_reg_wr16(C7108_AES_KEY4_LO, (u16) key[0]);
		c7108_reg_wr16(C7108_AES_KEY4_HI, (u16) (key[0] >> 16));
		c7108_reg_wr16(C7108_AES_KEY5_LO, (u16) key[1]);
		c7108_reg_wr16(C7108_AES_KEY5_HI, (u16) (key[1] >> 16));
		
	    }
	    
	} else { 
	    /* Program Key */
	    c7108_reg_wr16(C7108_AES_KEY0_LO, (u16) key[0]);
	    c7108_reg_wr16(C7108_AES_KEY0_HI, (u16) (key[0] >> 16));
	    c7108_reg_wr16(C7108_AES_KEY1_LO, (u16) key[1]);
	    c7108_reg_wr16(C7108_AES_KEY1_HI, (u16) (key[1] >> 16));
	    c7108_reg_wr16(C7108_AES_KEY2_LO, (u16) key[2]);
	    c7108_reg_wr16(C7108_AES_KEY2_HI, (u16) (key[2] >> 16));
	    c7108_reg_wr16(C7108_AES_KEY3_LO, (u16) key[3]);
	    c7108_reg_wr16(C7108_AES_KEY3_HI, (u16) (key[3] >> 16));
	    c7108_reg_wr16(C7108_AES_KEY4_LO, (u16) key[4]);
	    c7108_reg_wr16(C7108_AES_KEY4_HI, (u16) (key[4] >> 16));
	    c7108_reg_wr16(C7108_AES_KEY5_LO, (u16) key[5]);
	    c7108_reg_wr16(C7108_AES_KEY5_HI, (u16) (key[5] >> 16));
	    c7108_reg_wr16(C7108_AES_KEY6_LO, (u16) key[6]);
	    c7108_reg_wr16(C7108_AES_KEY6_HI, (u16) (key[6] >> 16));
	    c7108_reg_wr16(C7108_AES_KEY7_LO, (u16) key[7]);
	    c7108_reg_wr16(C7108_AES_KEY7_HI, (u16) (key[7] >> 16));
	    
	}
	
	/* Set IV always */
	c7108_aes_set_hw_iv(iv);
	
	/* Program DMA addresses */
	if ((rv = c7108_aes_setup_dma(src, dst, len)) < 0) { 
	    AES_UNLOCK();
	    return rv;
	}

	
	/* Start AES cipher */
	c7108_reg_wr16(C7108_AES_CTRL, ctrl | C7108_AES_GO);
	
	//printk("Ctrl: 0x%x\n", ctrl | C7108_AES_GO);
	do {
	    /* TODO: interrupt mode */
	    //        printk("aes_stat=0x%x\n", stat);
	    //udelay(100);
	} while ((cnt++ < 1000000) && 
		 !((stat=c7108_reg_rd16(C7108_AES_CTRL))&C7108_AES_OP_DONE));


	if ((mode == C7108_AES_CTRL_MODE_ECB)||
	    (mode == C7108_AES_CTRL_MODE_CBC)) { 
	    /* Save out key when the lock is held ... */
	    c7108_aes_read_dkey(key);
	}
	
	AES_UNLOCK();
	return 0;
	
}

/*
 * Generate a new crypto device session.
 */
static int
c7108_newsession(void *arg, u_int32_t *sid, struct cryptoini *cri)
{
	struct cipher_7108 **swd;
	u_int32_t i;
	char *algo;
	int mode, xfm_type;

	dprintk("%s()\n", __FUNCTION__);
	if (sid == NULL || cri == NULL) {
		dprintk("%s,%d - EINVAL\n", __FILE__, __LINE__);
		return EINVAL;
	}

	if (c7108_sessions) {
		for (i = 1; i < c7108_sesnum; i++)
			if (c7108_sessions[i] == NULL)
				break;
	} else
		i = 1;		/* NB: to silence compiler warning */

	if (c7108_sessions == NULL || i == c7108_sesnum) {
	    if (c7108_sessions == NULL) {
		i = 1; /* We leave c7108_sessions[0] empty */
		c7108_sesnum = CRYPTO_SW_SESSIONS;
	    } else
		c7108_sesnum *= 2;
	    
	    swd = kmalloc(c7108_sesnum * sizeof(struct cipher_7108 *), 
			  GFP_ATOMIC);
	    if (swd == NULL) {
		/* Reset session number */
		if (c7108_sesnum == CRYPTO_SW_SESSIONS)
		    c7108_sesnum = 0;
		else
		    c7108_sesnum /= 2;
		dprintk("%s,%d: ENOBUFS\n", __FILE__, __LINE__);
		return ENOBUFS;
	    }
	    memset(swd, 0, c7108_sesnum * sizeof(struct cipher_7108 *));
	    
	    /* Copy existing sessions */
	    if (c7108_sessions) {
		memcpy(swd, c7108_sessions,
		       (c7108_sesnum / 2) * sizeof(struct cipher_7108 *));
		kfree(c7108_sessions);
	    }
	    
	    c7108_sessions = swd;

	}
	
	swd = &c7108_sessions[i];
	*sid = i;

	while (cri) {
		*swd = (struct cipher_7108 *) 
		    kmalloc(sizeof(struct cipher_7108), GFP_ATOMIC);
		if (*swd == NULL) {
		    c7108_freesession(NULL, i);
		    dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		    return ENOBUFS;
		}
		memset(*swd, 0, sizeof(struct cipher_7108));

		algo = NULL;
		mode = 0;
		xfm_type = HW_TYPE_CIPHER;

		switch (cri->cri_alg) {

		case CRYPTO_AES_CBC:
			algo = "aes";
			mode = CRYPTO_TFM_MODE_CBC;
			c7108_crypto_mode = C7108_AES_CTRL_MODE_CBC;
			break;
#if 0
		case CRYPTO_AES_CTR:
			algo = "aes_ctr";
			mode = CRYPTO_TFM_MODE_CBC;
			c7108_crypto_mode = C7108_AES_CTRL_MODE_CTR;
			break;
		case CRYPTO_AES_ECB:
			algo = "aes_ecb";
			mode = CRYPTO_TFM_MODE_CBC;
			c7108_crypto_mode = C7108_AES_CTRL_MODE_ECB;
			break;
		case CRYPTO_AES_OFB:
			algo = "aes_ofb";
			mode = CRYPTO_TFM_MODE_CBC;
			c7108_crypto_mode = C7108_AES_CTRL_MODE_OFB;
			break;
		case CRYPTO_AES_CFB:
			algo = "aes_cfb";
			mode = CRYPTO_TFM_MODE_CBC;
			c7108_crypto_mode = C7108_AES_CTRL_MODE_CFB;
			break;
#endif
		default:
		        printk("unsupported crypto algorithm: %d\n", 
			       cri->cri_alg);
			return -EINVAL;
			break;
		}


		if (!algo || !*algo) {
		    printk("cypher_7108_crypto: Unknown algo 0x%x\n", 
			   cri->cri_alg);
		    c7108_freesession(NULL, i);
		    return EINVAL;
		}
		
		if (xfm_type == HW_TYPE_CIPHER) {
		    if (debug) {
			dprintk("%s key:", __FUNCTION__);
			for (i = 0; i < (cri->cri_klen + 7) / 8; i++)
			    dprintk("%s0x%02x", (i % 8) ? " " : "\n    ",
				    cri->cri_key[i]);
			dprintk("\n");
		    }

		} else if (xfm_type == SW_TYPE_HMAC || 
			   xfm_type == SW_TYPE_HASH) {
		    printk("cypher_7108_crypto: HMAC unsupported!\n");
		    return -EINVAL;
		    c7108_freesession(NULL, i);
		} else {
		    printk("cypher_7108_crypto: "
			   "Unhandled xfm_type %d\n", xfm_type);
		    c7108_freesession(NULL, i);
		    return EINVAL;
		}
		
		(*swd)->cri_alg = cri->cri_alg;
		(*swd)->xfm_type = xfm_type;
		
		cri = cri->cri_next;
		swd = &((*swd)->next);
	}
	return 0;
}

/*
 * Free a session.
 */
static int
c7108_freesession(void *arg, u_int64_t tid)
{
	struct cipher_7108 *swd;
	u_int32_t sid = CRYPTO_SESID2LID(tid);

	dprintk("%s()\n", __FUNCTION__);
	if (sid > c7108_sesnum || c7108_sessions == NULL ||
			c7108_sessions[sid] == NULL) {
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		return(EINVAL);
	}

	/* Silently accept and return */
	if (sid == 0)
		return(0);

	while ((swd = c7108_sessions[sid]) != NULL) {
		c7108_sessions[sid] = swd->next;
		kfree(swd);
	}
	return 0;
}

/*
 * Process a hardware request.
 */
static int
c7108_process(void *arg, struct cryptop *crp, int hint)
{
	struct cryptodesc *crd;
	struct cipher_7108 *sw;
	u_int32_t lid;
	int type;
	u32 hwkey[8];

#define SCATTERLIST_MAX 16
	struct scatterlist sg[SCATTERLIST_MAX];
	int sg_num, sg_len, skip;
	struct sk_buff *skb = NULL;
	struct uio *uiop = NULL;

	dprintk("%s()\n", __FUNCTION__);
	/* Sanity check */
	if (crp == NULL) {
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		return EINVAL;
	}

	crp->crp_etype = 0;

	if (crp->crp_desc == NULL || crp->crp_buf == NULL) {
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		crp->crp_etype = EINVAL;
		goto done;
	}

	lid = crp->crp_sid & 0xffffffff;
	if (lid >= c7108_sesnum || lid == 0 || c7108_sessions == NULL ||
			c7108_sessions[lid] == NULL) {
		crp->crp_etype = ENOENT;
		dprintk("%s,%d: ENOENT\n", __FILE__, __LINE__);
		goto done;
	}

	/*
	 * do some error checking outside of the loop for SKB and IOV
	 * processing this leaves us with valid skb or uiop pointers
	 * for later
	 */
	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		skb = (struct sk_buff *) crp->crp_buf;
		if (skb_shinfo(skb)->nr_frags >= SCATTERLIST_MAX) {
			printk("%s,%d: %d nr_frags > SCATTERLIST_MAX", 
			       __FILE__, __LINE__,
			       skb_shinfo(skb)->nr_frags);
			goto done;
		}
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		uiop = (struct uio *) crp->crp_buf;
		if (uiop->uio_iovcnt > SCATTERLIST_MAX) {
			printk("%s,%d: %d uio_iovcnt > SCATTERLIST_MAX", 
			       __FILE__, __LINE__,
			       uiop->uio_iovcnt);
			goto done;
		}
	}

	/* Go through crypto descriptors, processing as we go */
	for (crd = crp->crp_desc; crd; crd = crd->crd_next) {
	    /*
	     * Find the crypto context.
	     *
	     * XXX Note that the logic here prevents us from having
	     * XXX the same algorithm multiple times in a session
	     * XXX (or rather, we can but it won't give us the right
	     * XXX results). To do that, we'd need some way of differentiating
	     * XXX between the various instances of an algorithm (so we can
	     * XXX locate the correct crypto context).
	     */
	    for (sw = c7108_sessions[lid]; 
		 sw && sw->cri_alg != crd->crd_alg;
		 sw = sw->next)
		;
	    
	    /* No such context ? */
	    if (sw == NULL) {
		crp->crp_etype = EINVAL;
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		goto done;
	    }
	    
	    skip = crd->crd_skip;
	    
	    /*
	     * setup the SG list skip from the start of the buffer
	     */
	    memset(sg, 0, sizeof(sg));
	    if (crp->crp_flags & CRYPTO_F_SKBUF) {
		int i, len;
		type = CRYPTO_BUF_SKBUF;
		
		sg_num = 0;
		sg_len = 0;

		if (skip < skb_headlen(skb)) {
		    //sg[sg_num].page   = virt_to_page(skb->data + skip);
			//sg[sg_num].offset = offset_in_page(skb->data + skip);
		    len = skb_headlen(skb) - skip;
		    if (len + sg_len > crd->crd_len)
			len = crd->crd_len - sg_len;
		    //sg[sg_num].length = len;
		    sg_set_page(&sg[sg_num], virt_to_page(skb->data + skip), len, offset_in_page(skb->data + skip));
			sg_len += sg[sg_num].length;
		    sg_num++;
		    skip = 0;
		} else
		    skip -= skb_headlen(skb);
		
		for (i = 0; sg_len < crd->crd_len &&
			 i < skb_shinfo(skb)->nr_frags &&
			 sg_num < SCATTERLIST_MAX; i++) {
		    if (skip < skb_shinfo(skb)->frags[i].size) {
			//sg[sg_num].page   = skb_shinfo(skb)->frags[i].page;
			//sg[sg_num].offset = skb_shinfo(skb)->frags[i].page_offset + skip;
			len = skb_shinfo(skb)->frags[i].size - skip;
			if (len + sg_len > crd->crd_len)
			    len = crd->crd_len - sg_len;
			//sg[sg_num].length = len;
			sg_set_page(&sg[sg_num], skb_shinfo(skb)->frags[i].page, len, skb_shinfo(skb)->frags[i].page_offset + skip);
			sg_len += sg[sg_num].length;
			sg_num++;
			skip = 0;
		    } else
			skip -= skb_shinfo(skb)->frags[i].size;
		}
	    } else if (crp->crp_flags & CRYPTO_F_IOV) {
		int len;
		type = CRYPTO_BUF_IOV;
		sg_len = 0;
		for (sg_num = 0; sg_len < crd->crd_len &&
			 sg_num < uiop->uio_iovcnt &&
			 sg_num < SCATTERLIST_MAX; sg_num++) {
		    if (skip < uiop->uio_iov[sg_num].iov_len) {
			//sg[sg_num].page   =			    virt_to_page(uiop->uio_iov[sg_num].iov_base+skip);
			//sg[sg_num].offset =			   offset_in_page(uiop->uio_iov[sg_num].iov_base+skip);
			len = uiop->uio_iov[sg_num].iov_len - skip;
			if (len + sg_len > crd->crd_len)
			    len = crd->crd_len - sg_len;
			//sg[sg_num].length = len;
			sg_set_page(&sg[sg_num], virt_to_page(uiop->uio_iov[sg_num].iov_base+skip), len, offset_in_page(uiop->uio_iov[sg_num].iov_base+skip));
			sg_len += sg[sg_num].length;
			skip = 0;
		    } else 
			skip -= uiop->uio_iov[sg_num].iov_len;
		}
	    } else {
		type = CRYPTO_BUF_CONTIG;
		//sg[0].page   = virt_to_page(crp->crp_buf + skip);
		//sg[0].offset = offset_in_page(crp->crp_buf + skip);
		sg_len = (crp->crp_ilen - skip);
		if (sg_len > crd->crd_len)
		    sg_len = crd->crd_len;
		//sg[0].length = sg_len;
		sg_set_page(&sg[0], virt_to_page(crp->crp_buf + skip), sg_len, offset_in_page(crp->crp_buf + skip));
		sg_num = 1;
	    }
	    
	    
	    switch (sw->xfm_type) {

	    case HW_TYPE_CIPHER: {

		unsigned char iv[64];
		unsigned char *ivp = iv;
		int i;
		int ivsize = 16;    /* fixed for AES */
		int blocksize = 16; /* fixed for AES */

		if (sg_len < blocksize) {
		    crp->crp_etype = EINVAL;
		    dprintk("%s,%d: EINVAL len %d < %d\n", 
			    __FILE__, __LINE__,
			    sg_len, 
			    blocksize);
		    goto done;
		}
		
		if (ivsize > sizeof(iv)) {
		    crp->crp_etype = EINVAL;
		    dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		    goto done;
		}
		
		if (crd->crd_flags & CRD_F_ENCRYPT) { /* encrypt */
		    
		    if (crd->crd_flags & CRD_F_IV_EXPLICIT) {
			ivp = crd->crd_iv;
		    } else {
			get_random_bytes(ivp, ivsize);
		    }
		    /*
		     * do we have to copy the IV back to the buffer ?
		     */
		    if ((crd->crd_flags & CRD_F_IV_PRESENT) == 0) {
			    crypto_copyback(crp->crp_buf,
					  crd->crd_inject,
					  ivsize,
					  (caddr_t)ivp);
		    }

		    c7108_xlate_key(crd->crd_klen,
				    (u8*)crd->crd_key, (u32*)hwkey);

		    /* Encrypt SG list */
		    for (i = 0; i < sg_num; i++) { 
			sg[i].dma_address = 
			    dma_map_single(NULL, 
					   kmap(sg_page(&sg[i])) + sg[i].offset, sg_len, DMA_BIDIRECTIONAL);
#if 0							   
			printk("sg[%d]:0x%08x, off 0x%08x "
			       "kmap 0x%08x phys 0x%08x\n", 
			       i, sg[i].page, sg[i].offset,
			       kmap(sg[i].page) + sg[i].offset,
			       sg[i].dma_address);
#endif
			c7108_aes_cipher(C7108_AES_ENCRYPT,
					 sg[i].dma_address,
					 sg[i].dma_address,
					 sg_len,
					 crd->crd_klen,
					 c7108_crypto_mode,
					 hwkey,
					 ivp);

			if ((c7108_crypto_mode == C7108_AES_CTRL_MODE_CBC)||
			    (c7108_crypto_mode == C7108_AES_CTRL_MODE_ECB)) { 
			    /* Read back expanded key and cache it in key
			     * context.
			     * NOTE: for ECB/CBC modes only (not CTR, CFB, OFB)
			     *       where you set the key once.
			     */
			    c7108_cache_key(crd->crd_klen, 
					    (u32*)hwkey, (u8*)crd->crd_key);
#if 0
			    printk("%s expanded key:", __FUNCTION__);
			    for (i = 0; i < (crd->crd_klen + 7) / 8; i++)
				printk("%s0x%02x", (i % 8) ? " " : "\n    ",
				       crd->crd_key[i]);
			    printk("\n");
#endif
			}
		    }
		}
		else { /*decrypt */

		    if (crd->crd_flags & CRD_F_IV_EXPLICIT) {
			ivp = crd->crd_iv;
		    } else {
			crypto_copydata(crp->crp_buf, crd->crd_inject,
				  ivsize, (caddr_t)ivp);
		    }

		    c7108_xlate_key(crd->crd_klen,
				    (u8*)crd->crd_key, (u32*)hwkey);

		    /* Decrypt SG list */
		    for (i = 0; i < sg_num; i++) { 
			sg[i].dma_address = 
			    dma_map_single(NULL, 
					   kmap(sg_page(&sg[i])) + sg[i].offset,
					   sg_len, DMA_BIDIRECTIONAL);

#if 0
			printk("sg[%d]:0x%08x, off 0x%08x "
			       "kmap 0x%08x phys 0x%08x\n", 
			       i, sg[i].page, sg[i].offset,
			       kmap(sg[i].page) + sg[i].offset,
			       sg[i].dma_address);
#endif
			c7108_aes_cipher(C7108_AES_DECRYPT,
					 sg[i].dma_address,
					 sg[i].dma_address,
					 sg_len,
					 crd->crd_klen,
					 c7108_crypto_mode,
					 hwkey,
					 ivp);
		    }
		}
	    } break;
	    case SW_TYPE_HMAC:
	    case SW_TYPE_HASH:
		crp->crp_etype = EINVAL;
		goto done;
		break;
		
	    case SW_TYPE_COMP:
		crp->crp_etype = EINVAL;
		goto done;
		break;
		
	    default:
		/* Unknown/unsupported algorithm */
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		crp->crp_etype = EINVAL;
		goto done;
	    }
	}
	
done:
	crypto_done(crp);
	return 0;
}

static struct {                                                                                                                 
	softc_device_decl sc_dev;                                                                                               
} a7108dev;

static device_method_t a7108_methods = {                                                                                          
/* crypto device methods */                                                                                             
	DEVMETHOD(cryptodev_newsession, c7108_newsession),                                                                  
	DEVMETHOD(cryptodev_freesession, c7108_freesession),                                                             
	DEVMETHOD(cryptodev_process, c7108_process),                                                                     
	DEVMETHOD(cryptodev_kprocess, NULL) 
};   

static int
cypher_7108_crypto_init(void)
{
	dprintk("%s(%p)\n", __FUNCTION__, cypher_7108_crypto_init);
	
	iobar = (unsigned long)ioremap(CCU_AES_REG_BASE, 0x4000);
	printk("7108: AES @ 0x%08x (0x%08x phys) %s mode\n", 
	       iobar, CCU_AES_REG_BASE, 
	       c7108_crypto_mode & C7108_AES_CTRL_MODE_CBC ? "CBC" :
	       c7108_crypto_mode & C7108_AES_CTRL_MODE_ECB ? "ECB" : 
	       c7108_crypto_mode & C7108_AES_CTRL_MODE_CTR ? "CTR" : 
	       c7108_crypto_mode & C7108_AES_CTRL_MODE_CFB ? "CFB" : 
	       c7108_crypto_mode & C7108_AES_CTRL_MODE_OFB ? "OFB" : "???");
	csr_mutex  = SPIN_LOCK_UNLOCKED;

	memset(&a7108dev, 0, sizeof(a7108dev));                                                                                     
	softc_device_init(&a7108dev, "aes7108", 0, a7108_methods);

       	c7108_id = crypto_get_driverid(softc_get_device(&a7108dev), CRYPTOCAP_F_HARDWARE);
	if (c7108_id < 0)
		panic("7108: crypto device cannot initialize!");

//	crypto_register(c7108_id, CRYPTO_AES_CBC, 0, 0, c7108_newsession, c7108_freesession, c7108_process, NULL);
	crypto_register(c7108_id, CRYPTO_AES_CBC, 0, 0);

	return(0);
}

static void
cypher_7108_crypto_exit(void)
{
	dprintk("%s()\n", __FUNCTION__);
	crypto_unregister_all(c7108_id);
	c7108_id = -1;
}

module_init(cypher_7108_crypto_init);
module_exit(cypher_7108_crypto_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Cypher 7108 Crypto (OCF module for kernel crypto)");
