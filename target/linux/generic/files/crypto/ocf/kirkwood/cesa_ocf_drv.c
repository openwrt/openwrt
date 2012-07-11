/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.


********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
*******************************************************************************/

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38) && !defined(AUTOCONF_INCLUDED)
#include <linux/config.h>
#endif
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
#include <linux/platform_device.h>
#include <asm/scatterlist.h>
#include <linux/spinlock.h>
#include "ctrlEnv/sys/mvSysCesa.h"
#include "cesa/mvCesa.h" /* moved here before cryptodev.h due to include dependencies */
#include <cryptodev.h>
#include <uio.h>
#include <plat/mv_cesa.h>
#include <linux/mbus.h>
#include "mvDebug.h"

#include "cesa/mvMD5.h"
#include "cesa/mvSHA1.h"

#include "cesa/mvCesaRegs.h"
#include "cesa/AES/mvAes.h"
#include "cesa/mvLru.h"

#undef  RT_DEBUG
#ifdef RT_DEBUG
static int debug = 1;
module_param(debug, int, 1);
MODULE_PARM_DESC(debug, "Enable debug");
#undef dprintk
#define dprintk(a...)	if (debug) { printk(a); } else
#else
static int debug = 0;
#undef dprintk
#define dprintk(a...)
#endif


/* TDMA Regs */
#define WINDOW_BASE(i) 0xA00 + (i << 3)
#define WINDOW_CTRL(i) 0xA04 + (i << 3)

/* interrupt handling */
#undef CESA_OCF_POLLING
#undef CESA_OCF_TASKLET

#if defined(CESA_OCF_POLLING) && defined(CESA_OCF_TASKLET)
#error "don't use both tasklet and polling mode"
#endif

extern int cesaReqResources;
/* support for spliting action into 2 actions */
#define CESA_OCF_SPLIT

/* general defines */
#define CESA_OCF_MAX_SES 128
#define CESA_Q_SIZE	 64

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)
#define FRAG_PAGE(f)    (f).p
#else
#define FRAG_PAGE(f)    (f)
#endif

/* data structures */
struct cesa_ocf_data {
        int                                      cipher_alg;
        int                                      auth_alg;
	int					 encrypt_tn_auth;
#define  auth_tn_decrypt  encrypt_tn_auth
	int					 ivlen;
	int 					 digestlen;
	short					 sid_encrypt;
	short					 sid_decrypt;
	/* fragment workaround sessions */
	short					 frag_wa_encrypt;
	short					 frag_wa_decrypt;
	short					 frag_wa_auth;
};

/* CESA device data */
struct cesa_dev {
	void __iomem *sram;
	void __iomem *reg;
        struct mv_cesa_platform_data *plat_data;
	int irq;
};

#define DIGEST_BUF_SIZE	32
struct cesa_ocf_process {
	MV_CESA_COMMAND 			cesa_cmd;
	MV_CESA_MBUF 				cesa_mbuf;	
	MV_BUF_INFO  				cesa_bufs[MV_CESA_MAX_MBUF_FRAGS];
	char					digest[DIGEST_BUF_SIZE];
	int					digest_len;
	struct cryptop 				*crp;
	int 					need_cb;
};

/* global variables */
static int32_t			cesa_ocf_id 		= -1;
static struct cesa_ocf_data 	*cesa_ocf_sessions[CESA_OCF_MAX_SES];
static spinlock_t 		cesa_lock;
static struct cesa_dev cesa_device;

/* static APIs */
static int 		cesa_ocf_process	(device_t, struct cryptop *, int);
static int 		cesa_ocf_newsession	(device_t, u_int32_t *, struct cryptoini *);
static int 		cesa_ocf_freesession	(device_t, u_int64_t);
static void 		cesa_callback		(unsigned long);
static irqreturn_t	cesa_interrupt_handler	(int, void *);
#ifdef CESA_OCF_POLLING
static void cesa_interrupt_polling(void);
#endif
#ifdef CESA_OCF_TASKLET
static struct tasklet_struct cesa_ocf_tasklet;
#endif

static struct timeval          tt_start;
static struct timeval          tt_end;

/*
 * dummy device structure
 */

static struct {
	softc_device_decl	sc_dev;
} mv_cesa_dev;

static device_method_t mv_cesa_methods = {
	/* crypto device methods */
	DEVMETHOD(cryptodev_newsession,	cesa_ocf_newsession),
	DEVMETHOD(cryptodev_freesession,cesa_ocf_freesession),
	DEVMETHOD(cryptodev_process,	cesa_ocf_process),
	DEVMETHOD(cryptodev_kprocess,	NULL),
};



/* Add debug Trace */
#undef CESA_OCF_TRACE_DEBUG
#ifdef CESA_OCF_TRACE_DEBUG

#define MV_CESA_USE_TIMER_ID    0

typedef struct
{
    int             type;       /* 0 - isrEmpty, 1 - cesaReadyGet, 2 - cesaAction */
    MV_U32          timeStamp;
    MV_U32          cause;
    MV_U32          realCause;
    MV_U32          dmaCause;
    int             resources;
    MV_CESA_REQ*    pReqReady;
    MV_CESA_REQ*    pReqEmpty;
    MV_CESA_REQ*    pReqProcess;
} MV_CESA_TEST_TRACE;

#define MV_CESA_TEST_TRACE_SIZE      50

static int cesaTestTraceIdx = 0;
static MV_CESA_TEST_TRACE    cesaTestTrace[MV_CESA_TEST_TRACE_SIZE];

static void cesaTestTraceAdd(int type)
{
    cesaTestTrace[cesaTestTraceIdx].type = type;
    cesaTestTrace[cesaTestTraceIdx].realCause = MV_REG_READ(MV_CESA_ISR_CAUSE_REG);
    //cesaTestTrace[cesaTestTraceIdx].idmaCause = MV_REG_READ(IDMA_CAUSE_REG);
    cesaTestTrace[cesaTestTraceIdx].resources = cesaReqResources;
    cesaTestTrace[cesaTestTraceIdx].pReqReady = pCesaReqReady;
    cesaTestTrace[cesaTestTraceIdx].pReqEmpty = pCesaReqEmpty;
    cesaTestTrace[cesaTestTraceIdx].pReqProcess = pCesaReqProcess;
    cesaTestTrace[cesaTestTraceIdx].timeStamp = mvCntmrRead(MV_CESA_USE_TIMER_ID);
    cesaTestTraceIdx++;
    if(cesaTestTraceIdx == MV_CESA_TEST_TRACE_SIZE)
        cesaTestTraceIdx = 0;
}

#else /* CESA_OCF_TRACE_DEBUG */

#define cesaTestTraceAdd(x)

#endif /* CESA_OCF_TRACE_DEBUG */

unsigned int
get_usec(unsigned int start)
{
	if(start) {
		do_gettimeofday (&tt_start);
		return 0;
	}
	else {
        	do_gettimeofday (&tt_end);
        	tt_end.tv_sec -= tt_start.tv_sec;
        	tt_end.tv_usec -= tt_start.tv_usec;
        	if (tt_end.tv_usec < 0) {
                	tt_end.tv_usec += 1000 * 1000;
                	tt_end.tv_sec -= 1;
        	}
	}
	printk("time taken is  %d\n", (unsigned int)(tt_end.tv_usec + tt_end.tv_sec * 1000000));
	return (tt_end.tv_usec + tt_end.tv_sec * 1000000);
}

#ifdef RT_DEBUG
/* 
 * check that the crp action match the current session
 */
static int 
ocf_check_action(struct cryptop *crp, struct cesa_ocf_data *cesa_ocf_cur_ses) {
	int count = 0;
	int encrypt = 0, decrypt = 0, auth = 0;
	struct cryptodesc *crd;

        /* Go through crypto descriptors, processing as we go */
        for (crd = crp->crp_desc; crd; crd = crd->crd_next, count++) {
		if(count > 2) {
			printk("%s,%d: session mode is not supported.\n", __FILE__, __LINE__);
			return 1;
		}
		
		/* Encryption /Decryption */
		if(crd->crd_alg == cesa_ocf_cur_ses->cipher_alg) {
			/* check that the action is compatible with session */
			if(encrypt || decrypt) {
				printk("%s,%d: session mode is not supported.\n", __FILE__, __LINE__);
				return 1;
			}

			if(crd->crd_flags & CRD_F_ENCRYPT) { /* encrypt */
				if( (count == 2) && (cesa_ocf_cur_ses->encrypt_tn_auth) ) {
					printk("%s,%d: sequence isn't supported by this session.\n", __FILE__, __LINE__);
					return 1;
				}
				encrypt++;
			}
			else { 					/* decrypt */
				if( (count == 2) && !(cesa_ocf_cur_ses->auth_tn_decrypt) ) {
					printk("%s,%d: sequence isn't supported by this session.\n", __FILE__, __LINE__);
					return 1;
				}
				decrypt++;
			}

		}
		/* Authentication */
		else if(crd->crd_alg == cesa_ocf_cur_ses->auth_alg) {
			/* check that the action is compatible with session */
			if(auth) {
				printk("%s,%d: session mode is not supported.\n", __FILE__, __LINE__);
				return 1;
			}
			if( (count == 2) && (decrypt) && (cesa_ocf_cur_ses->auth_tn_decrypt)) {
				printk("%s,%d: sequence isn't supported by this session.\n", __FILE__, __LINE__);
				return 1;
			}
			if( (count == 2) && (encrypt) && !(cesa_ocf_cur_ses->encrypt_tn_auth)) {
				printk("%s,%d: sequence isn't supported by this session.\n", __FILE__, __LINE__);
				return 1;
			}
			auth++;
		} 
		else {
			printk("%s,%d: Alg isn't supported by this session.\n", __FILE__, __LINE__);
			return 1;
		}
	}
	return 0;

}
#endif

/*
 * Process a request.
 */
static int 
cesa_ocf_process(device_t dev, struct cryptop *crp, int hint)
{
	struct cesa_ocf_process *cesa_ocf_cmd = NULL;
	struct cesa_ocf_process *cesa_ocf_cmd_wa = NULL;
	MV_CESA_COMMAND	*cesa_cmd;
	struct cryptodesc *crd;
	struct cesa_ocf_data *cesa_ocf_cur_ses;
	int sid = 0, temp_len = 0, i;
	int encrypt = 0, decrypt = 0, auth = 0;
	int  status;
	struct sk_buff *skb = NULL;
	struct uio *uiop = NULL;
	unsigned char *ivp;
	MV_BUF_INFO *p_buf_info;	
	MV_CESA_MBUF *p_mbuf_info;
	unsigned long flags;

        dprintk("%s()\n", __FUNCTION__);

	if( cesaReqResources <= 1 ) {
                dprintk("%s,%d: ERESTART\n", __FILE__, __LINE__);
                return ERESTART;
	}

#ifdef RT_DEBUG
        /* Sanity check */
        if (crp == NULL) {
                printk("%s,%d: EINVAL\n", __FILE__, __LINE__);
                return EINVAL;
        }

        if (crp->crp_desc == NULL || crp->crp_buf == NULL ) {
                printk("%s,%d: EINVAL\n", __FILE__, __LINE__);
                crp->crp_etype = EINVAL;
                return EINVAL;
        }

        sid = crp->crp_sid & 0xffffffff;
        if ((sid >= CESA_OCF_MAX_SES) || (cesa_ocf_sessions[sid] == NULL)) {
                crp->crp_etype = ENOENT;
                printk("%s,%d: ENOENT session %d \n", __FILE__, __LINE__, sid);
                return EINVAL;
        }
#endif

	sid = crp->crp_sid & 0xffffffff;
	crp->crp_etype = 0;
	cesa_ocf_cur_ses = cesa_ocf_sessions[sid];

#ifdef RT_DEBUG
	if(ocf_check_action(crp, cesa_ocf_cur_ses)){
		goto p_error;
	}
#endif

	/* malloc a new  cesa process */	
	cesa_ocf_cmd = kmalloc(sizeof(struct cesa_ocf_process), GFP_ATOMIC);
	
        if (cesa_ocf_cmd == NULL) {
            	printk("%s,%d: ENOBUFS \n", __FILE__, __LINE__);
            	goto p_error;
      	}
	memset(cesa_ocf_cmd, 0, sizeof(struct cesa_ocf_process));

	/* init cesa_process */
	cesa_ocf_cmd->crp = crp;
	/* always call callback */
	cesa_ocf_cmd->need_cb = 1;

	/* init cesa_cmd for usage of the HALs */
	cesa_cmd = &cesa_ocf_cmd->cesa_cmd;
	cesa_cmd->pReqPrv = (void *)cesa_ocf_cmd;
	cesa_cmd->sessionId = cesa_ocf_cur_ses->sid_encrypt; /* defualt use encrypt */

	/* prepare src buffer 	*/
	/* we send the entire buffer to the HAL, even if only part of it should be encrypt/auth.  */
	/* if not using seesions for both encrypt and auth, then it will be wiser to to copy only */
	/* from skip to crd_len. 								  */
	p_buf_info = cesa_ocf_cmd->cesa_bufs;	
	p_mbuf_info = &cesa_ocf_cmd->cesa_mbuf;

	p_buf_info += 2; /* save 2 first buffers for IV and digest - 
			    we won't append them to the end since, they 
			    might be places in an unaligned addresses. */
	
	p_mbuf_info->pFrags = p_buf_info;
	temp_len = 0;

	/* handle SKB */
	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		
		dprintk("%s,%d: handle SKB.\n", __FILE__, __LINE__);
		skb = (struct sk_buff *) crp->crp_buf;

                if (skb_shinfo(skb)->nr_frags >= (MV_CESA_MAX_MBUF_FRAGS - 1)) {
                        printk("%s,%d: %d nr_frags > MV_CESA_MAX_MBUF_FRAGS", __FILE__, __LINE__, skb_shinfo(skb)->nr_frags);
                        goto p_error;
                }

		p_mbuf_info->mbufSize = skb->len;
		temp_len = skb->len;
        	/* first skb fragment */
        	p_buf_info->bufSize = skb_headlen(skb);
        	p_buf_info->bufVirtPtr = skb->data;
		p_buf_info++;

        	/* now handle all other skb fragments */
        	for ( i = 0; i < skb_shinfo(skb)->nr_frags; i++ ) {
            		skb_frag_t *frag = &skb_shinfo(skb)->frags[i];
            		p_buf_info->bufSize = frag->size;
            		p_buf_info->bufVirtPtr = page_address(FRAG_PAGE(frag->page)) + frag->page_offset;
            		p_buf_info++;
        	}
        	p_mbuf_info->numFrags = skb_shinfo(skb)->nr_frags + 1;
	}
	/* handle UIO */
	else if(crp->crp_flags & CRYPTO_F_IOV) {
	
		dprintk("%s,%d: handle UIO.\n", __FILE__, __LINE__);
		uiop = (struct uio *) crp->crp_buf;

                if (uiop->uio_iovcnt > (MV_CESA_MAX_MBUF_FRAGS - 1)) {
                        printk("%s,%d: %d uio_iovcnt > MV_CESA_MAX_MBUF_FRAGS \n", __FILE__, __LINE__, uiop->uio_iovcnt);
                        goto p_error;
                }

		p_mbuf_info->mbufSize = crp->crp_ilen;
		p_mbuf_info->numFrags = uiop->uio_iovcnt;
		for(i = 0; i < uiop->uio_iovcnt; i++) {
			p_buf_info->bufVirtPtr = uiop->uio_iov[i].iov_base;
			p_buf_info->bufSize = uiop->uio_iov[i].iov_len;
			temp_len += p_buf_info->bufSize;
			dprintk("%s,%d: buf %x-> addr %x, size %x \n"
				, __FILE__, __LINE__, i, (unsigned int)p_buf_info->bufVirtPtr, p_buf_info->bufSize);
			p_buf_info++;			
		}

	}
	/* handle CONTIG */
	else {
		dprintk("%s,%d: handle CONTIG.\n", __FILE__, __LINE__); 
		p_mbuf_info->numFrags = 1;
		p_mbuf_info->mbufSize = crp->crp_ilen;
		p_buf_info->bufVirtPtr = crp->crp_buf;
		p_buf_info->bufSize = crp->crp_ilen;
		temp_len = crp->crp_ilen;
		p_buf_info++;
	}
	
	/* Support up to 64K why? cause! */
	if(crp->crp_ilen > 64*1024) {
		printk("%s,%d: buf too big %x \n", __FILE__, __LINE__, crp->crp_ilen);
		goto p_error;
	}

	if( temp_len != crp->crp_ilen ) {
		printk("%s,%d: warning size don't match.(%x %x) \n", __FILE__, __LINE__, temp_len, crp->crp_ilen);
	}	

	cesa_cmd->pSrc = p_mbuf_info;
	cesa_cmd->pDst = p_mbuf_info;
	
	/* restore p_buf_info to point to first available buf */
	p_buf_info = cesa_ocf_cmd->cesa_bufs;	
	p_buf_info += 1; 


        /* Go through crypto descriptors, processing as we go */
        for (crd = crp->crp_desc; crd; crd = crd->crd_next) {
		
		/* Encryption /Decryption */
		if(crd->crd_alg == cesa_ocf_cur_ses->cipher_alg) {

			dprintk("%s,%d: cipher", __FILE__, __LINE__);

			cesa_cmd->cryptoOffset = crd->crd_skip;
    	              	cesa_cmd->cryptoLength = crd->crd_len;

			if(crd->crd_flags & CRD_F_ENCRYPT) { /* encrypt */
				dprintk(" encrypt \n");
				encrypt++;

				/* handle IV */
				if (crd->crd_flags & CRD_F_IV_EXPLICIT) {  /* IV from USER */
					dprintk("%s,%d: IV from USER (offset %x) \n", __FILE__, __LINE__, crd->crd_inject);
					cesa_cmd->ivFromUser = 1;
					ivp = crd->crd_iv;

                                	/*
                                 	 * do we have to copy the IV back to the buffer ?
                                 	 */
                                	if ((crd->crd_flags & CRD_F_IV_PRESENT) == 0) {
						dprintk("%s,%d: copy the IV back to the buffer\n", __FILE__, __LINE__);
						cesa_cmd->ivOffset = crd->crd_inject;
						crypto_copyback(crp->crp_flags, crp->crp_buf, crd->crd_inject, cesa_ocf_cur_ses->ivlen, ivp);
                                	}
					else {
						dprintk("%s,%d: don't copy the IV back to the buffer \n", __FILE__, __LINE__);
						p_mbuf_info->numFrags++;
						p_mbuf_info->mbufSize += cesa_ocf_cur_ses->ivlen; 
						p_mbuf_info->pFrags = p_buf_info;

						p_buf_info->bufVirtPtr = ivp;
						p_buf_info->bufSize = cesa_ocf_cur_ses->ivlen; 
						p_buf_info--;

						/* offsets */
						cesa_cmd->ivOffset = 0;
						cesa_cmd->cryptoOffset += cesa_ocf_cur_ses->ivlen;
						if(auth) {
							cesa_cmd->macOffset += cesa_ocf_cur_ses->ivlen;
							cesa_cmd->digestOffset += cesa_ocf_cur_ses->ivlen; 
						}	
					}
                                }
				else {					/* random IV */
					dprintk("%s,%d: random IV \n", __FILE__, __LINE__);
					cesa_cmd->ivFromUser = 0;

                                	/*
                                 	 * do we have to copy the IV back to the buffer ?
                                 	 */
					/* in this mode the HAL will always copy the IV */
					/* given by the session to the ivOffset  	*/
					if ((crd->crd_flags & CRD_F_IV_PRESENT) == 0) {
						cesa_cmd->ivOffset = crd->crd_inject;
					} 
					else {
						/* if IV isn't copy, then how will the user know which IV did we use??? */
						printk("%s,%d: EINVAL\n", __FILE__, __LINE__);
						goto p_error; 
					}
				}
			}
			else { 					/* decrypt */
				dprintk(" decrypt \n");
				decrypt++;
				cesa_cmd->sessionId = cesa_ocf_cur_ses->sid_decrypt;

				/* handle IV */
				if (crd->crd_flags & CRD_F_IV_EXPLICIT) {
					dprintk("%s,%d: IV from USER \n", __FILE__, __LINE__);
					/* append the IV buf to the mbuf */
					cesa_cmd->ivFromUser = 1;	
					p_mbuf_info->numFrags++;
					p_mbuf_info->mbufSize += cesa_ocf_cur_ses->ivlen; 
					p_mbuf_info->pFrags = p_buf_info;

					p_buf_info->bufVirtPtr = crd->crd_iv;
					p_buf_info->bufSize = cesa_ocf_cur_ses->ivlen; 
					p_buf_info--;

					/* offsets */
					cesa_cmd->ivOffset = 0;
					cesa_cmd->cryptoOffset += cesa_ocf_cur_ses->ivlen;
					if(auth) {
						cesa_cmd->macOffset += cesa_ocf_cur_ses->ivlen;
						cesa_cmd->digestOffset += cesa_ocf_cur_ses->ivlen; 
					}
                                }
				else {
					dprintk("%s,%d: IV inside the buffer \n", __FILE__, __LINE__);
					cesa_cmd->ivFromUser = 0;
					cesa_cmd->ivOffset = crd->crd_inject;
				}
			}

		}
		/* Authentication */
		else if(crd->crd_alg == cesa_ocf_cur_ses->auth_alg) {
			dprintk("%s,%d:  Authentication \n", __FILE__, __LINE__);
			auth++;
			cesa_cmd->macOffset = crd->crd_skip;
			cesa_cmd->macLength = crd->crd_len;

			/* digest + mac */
			cesa_cmd->digestOffset = crd->crd_inject;
		} 
		else {
			printk("%s,%d: Alg isn't supported by this session.\n", __FILE__, __LINE__);
			goto p_error;
		}
	}

	dprintk("\n");
	dprintk("%s,%d: Sending Action: \n", __FILE__, __LINE__);
	dprintk("%s,%d: IV from user: %d. IV offset %x \n",  __FILE__, __LINE__, cesa_cmd->ivFromUser, cesa_cmd->ivOffset);
	dprintk("%s,%d: crypt offset %x len %x \n", __FILE__, __LINE__, cesa_cmd->cryptoOffset, cesa_cmd->cryptoLength);
	dprintk("%s,%d: Auth offset %x len %x \n", __FILE__, __LINE__, cesa_cmd->macOffset, cesa_cmd->macLength);
	dprintk("%s,%d: set digest in offset %x . \n", __FILE__, __LINE__, cesa_cmd->digestOffset);
	if(debug) {
		mvCesaDebugMbuf("SRC BUFFER", cesa_cmd->pSrc, 0, cesa_cmd->pSrc->mbufSize);
	}


	/* send action to HAL */
	spin_lock_irqsave(&cesa_lock, flags);
	status = mvCesaAction(cesa_cmd);
	spin_unlock_irqrestore(&cesa_lock, flags);

	/* action not allowed */
	if(status == MV_NOT_ALLOWED) {
#ifdef CESA_OCF_SPLIT
		/* if both encrypt and auth try to split */
		if(auth && (encrypt || decrypt)) {
			MV_CESA_COMMAND	*cesa_cmd_wa;

			/* malloc a new cesa process and init it */	
			cesa_ocf_cmd_wa = kmalloc(sizeof(struct cesa_ocf_process), GFP_ATOMIC);
	
        		if (cesa_ocf_cmd_wa == NULL) {
            			printk("%s,%d: ENOBUFS \n", __FILE__, __LINE__);
            			goto p_error;
      			}
			memcpy(cesa_ocf_cmd_wa, cesa_ocf_cmd, sizeof(struct cesa_ocf_process));
			cesa_cmd_wa = &cesa_ocf_cmd_wa->cesa_cmd;
			cesa_cmd_wa->pReqPrv = (void *)cesa_ocf_cmd_wa;
			cesa_ocf_cmd_wa->need_cb = 0;

			/* break requests to two operation, first operation completion won't call callback */
			if((decrypt) && (cesa_ocf_cur_ses->auth_tn_decrypt)) {
				cesa_cmd_wa->sessionId = cesa_ocf_cur_ses->frag_wa_auth;
				cesa_cmd->sessionId = cesa_ocf_cur_ses->frag_wa_decrypt;
			}
			else if((decrypt) && !(cesa_ocf_cur_ses->auth_tn_decrypt)) {
				cesa_cmd_wa->sessionId = cesa_ocf_cur_ses->frag_wa_decrypt;
				cesa_cmd->sessionId = cesa_ocf_cur_ses->frag_wa_auth;
			}
			else if((encrypt) && (cesa_ocf_cur_ses->encrypt_tn_auth)) {
				cesa_cmd_wa->sessionId = cesa_ocf_cur_ses->frag_wa_encrypt;
				cesa_cmd->sessionId = cesa_ocf_cur_ses->frag_wa_auth;
			}
			else if((encrypt) && !(cesa_ocf_cur_ses->encrypt_tn_auth)){
				cesa_cmd_wa->sessionId = cesa_ocf_cur_ses->frag_wa_auth;
				cesa_cmd->sessionId = cesa_ocf_cur_ses->frag_wa_encrypt;
			}
			else {
				printk("%s,%d: Unsupporterd fragment wa mode \n", __FILE__, __LINE__);
            			goto p_error;
			}

			/* send the 2 actions to the HAL */
			spin_lock_irqsave(&cesa_lock, flags);
			status = mvCesaAction(cesa_cmd_wa);
			spin_unlock_irqrestore(&cesa_lock, flags);

			if((status != MV_NO_MORE) && (status != MV_OK)) {
				printk("%s,%d: cesa action failed, status = 0x%x\n", __FILE__, __LINE__, status);
				goto p_error;
			}
			spin_lock_irqsave(&cesa_lock, flags);
			status = mvCesaAction(cesa_cmd);
			spin_unlock_irqrestore(&cesa_lock, flags);

		}
		/* action not allowed and can't split */
		else 
#endif
		{
			goto p_error;
		}
	}

	/* Hal Q is full, send again. This should never happen */
	if(status == MV_NO_RESOURCE) {
		printk("%s,%d: cesa no more resources \n", __FILE__, __LINE__);
		if(cesa_ocf_cmd)
			kfree(cesa_ocf_cmd);
		if(cesa_ocf_cmd_wa)
			kfree(cesa_ocf_cmd_wa);
		return ERESTART;
	} 
	else if((status != MV_NO_MORE) && (status != MV_OK)) {
                printk("%s,%d: cesa action failed, status = 0x%x\n", __FILE__, __LINE__, status);
		goto p_error;
        }


#ifdef CESA_OCF_POLLING
	cesa_interrupt_polling();
#endif
	cesaTestTraceAdd(5);

	return 0;
p_error:
	crp->crp_etype = EINVAL;
	if(cesa_ocf_cmd)
		kfree(cesa_ocf_cmd);
	if(cesa_ocf_cmd_wa)
		kfree(cesa_ocf_cmd_wa);
       	return EINVAL;
}

/*
 * cesa callback. 
 */
static void
cesa_callback(unsigned long dummy)
{
	struct cesa_ocf_process *cesa_ocf_cmd = NULL;
	struct cryptop 		*crp = NULL;
	MV_CESA_RESULT  	result[MV_CESA_MAX_CHAN];
	int 			res_idx = 0,i;
	MV_STATUS               status;

	dprintk("%s()\n", __FUNCTION__);

#ifdef CESA_OCF_TASKLET
	disable_irq(cesa_device.irq);
#endif
    while(MV_TRUE) {
	
		 /* Get Ready requests */
		spin_lock(&cesa_lock);
		status = mvCesaReadyGet(&result[res_idx]);
		spin_unlock(&cesa_lock);

	        cesaTestTraceAdd(2);	

		    if(status != MV_OK) {
#ifdef CESA_OCF_POLLING
		        if(status == MV_BUSY) { /* Fragment */
			        cesa_interrupt_polling();
			        return;
		        }
#endif
	    	    break;
    	    }
	        res_idx++;
		    break;
	    }
	
	for(i = 0; i < res_idx; i++) {

		if(!result[i].pReqPrv) {
			printk("%s,%d: warning private is NULL\n", __FILE__, __LINE__);
			break;
		}

		cesa_ocf_cmd = result[i].pReqPrv;
		crp = cesa_ocf_cmd->crp; 

		// ignore HMAC error.
		//if(result->retCode)
		//	crp->crp_etype = EIO;	
	
#if  defined(CESA_OCF_POLLING) 
		if(!cesa_ocf_cmd->need_cb){
			cesa_interrupt_polling();
		}	
#endif
		if(cesa_ocf_cmd->need_cb) {
			if(debug) {
				mvCesaDebugMbuf("DST BUFFER", cesa_ocf_cmd->cesa_cmd.pDst, 0, cesa_ocf_cmd->cesa_cmd.pDst->mbufSize);
			}
			crypto_done(crp);
		}
		kfree(cesa_ocf_cmd);
    	}
#ifdef CESA_OCF_TASKLET
	enable_irq(cesa_device.irq);
#endif

	cesaTestTraceAdd(3);

	return;
}

#ifdef CESA_OCF_POLLING
static void
cesa_interrupt_polling(void)
{
        u32                  	cause;

	dprintk("%s()\n", __FUNCTION__);

  	/* Read cause register */
	do {
		cause = MV_REG_READ(MV_CESA_ISR_CAUSE_REG);
		cause &= MV_CESA_CAUSE_ACC_DMA_ALL_MASK;

	} while (cause == 0);
		
	/* clear interrupts */
    	MV_REG_WRITE(MV_CESA_ISR_CAUSE_REG, 0);

	cesa_callback(0);

	return;
}

#endif

/*
 * cesa Interrupt polling routine.
 */
static irqreturn_t
cesa_interrupt_handler(int irq, void *arg)
{
        u32                  	cause;

	dprintk("%s()\n", __FUNCTION__);

	cesaTestTraceAdd(0);

  	/* Read cause register */
	cause = MV_REG_READ(MV_CESA_ISR_CAUSE_REG);

    	if( (cause & MV_CESA_CAUSE_ACC_DMA_ALL_MASK) == 0)
    	{
        /* Empty interrupt */
		dprintk("%s,%d: cesaTestReadyIsr: cause=0x%x\n", __FILE__, __LINE__, cause);
        	return IRQ_HANDLED;
    	}
	
	/* clear interrupts */
    	MV_REG_WRITE(MV_CESA_ISR_CAUSE_REG, 0);

	cesaTestTraceAdd(1);
#ifdef CESA_OCF_TASKLET	
	tasklet_hi_schedule(&cesa_ocf_tasklet);
#else
	cesa_callback(0);
#endif
	return IRQ_HANDLED;
}

/*
 * Open a session.
 */
static int 
/*cesa_ocf_newsession(void *arg, u_int32_t *sid, struct cryptoini *cri)*/
cesa_ocf_newsession(device_t dev, u_int32_t *sid, struct cryptoini *cri)
{
	u32 status = 0, i;
	u32 count = 0, auth = 0, encrypt =0;
	struct cesa_ocf_data *cesa_ocf_cur_ses;
	MV_CESA_OPEN_SESSION cesa_session;
	MV_CESA_OPEN_SESSION *cesa_ses = &cesa_session;


        dprintk("%s()\n", __FUNCTION__);
        if (sid == NULL || cri == NULL) {
                printk("%s,%d: EINVAL\n", __FILE__, __LINE__);
                return EINVAL;
        }

	/* leave first empty like in other implementations */
        for (i = 1; i < CESA_OCF_MAX_SES; i++) {
       		if (cesa_ocf_sessions[i] == NULL)
               		break;
	}

	if(i >= CESA_OCF_MAX_SES) {
		printk("%s,%d: no more sessions \n", __FILE__, __LINE__);
                return EINVAL;
	}

        cesa_ocf_sessions[i] = (struct cesa_ocf_data *) kmalloc(sizeof(struct cesa_ocf_data), GFP_ATOMIC);
        if (cesa_ocf_sessions[i] == NULL) {
                cesa_ocf_freesession(NULL, i);
                printk("%s,%d: ENOBUFS \n", __FILE__, __LINE__);
                return ENOBUFS;
        }
	dprintk("%s,%d: new session %d \n", __FILE__, __LINE__, i);
	
        *sid = i;
        cesa_ocf_cur_ses = cesa_ocf_sessions[i];
        memset(cesa_ocf_cur_ses, 0, sizeof(struct cesa_ocf_data));
	cesa_ocf_cur_ses->sid_encrypt = -1;
	cesa_ocf_cur_ses->sid_decrypt = -1;
	cesa_ocf_cur_ses->frag_wa_encrypt = -1;
	cesa_ocf_cur_ses->frag_wa_decrypt = -1;
	cesa_ocf_cur_ses->frag_wa_auth = -1;

	/* init the session */	
	memset(cesa_ses, 0, sizeof(MV_CESA_OPEN_SESSION));
	count = 1;
        while (cri) {	
		if(count > 2) {
        		printk("%s,%d: don't support more then 2 operations\n", __FILE__, __LINE__);
        		goto error;
		}
                switch (cri->cri_alg) {
		case CRYPTO_AES_CBC:
			dprintk("%s,%d: (%d) AES CBC \n", __FILE__, __LINE__, count);
			cesa_ocf_cur_ses->cipher_alg = cri->cri_alg;
			cesa_ocf_cur_ses->ivlen = MV_CESA_AES_BLOCK_SIZE;
			cesa_ses->cryptoAlgorithm = MV_CESA_CRYPTO_AES;
			cesa_ses->cryptoMode = MV_CESA_CRYPTO_CBC;
			if(cri->cri_klen/8 > MV_CESA_MAX_CRYPTO_KEY_LENGTH) {
        			printk("%s,%d: CRYPTO key too long.\n", __FILE__, __LINE__);
        			goto error;
			}
			memcpy(cesa_ses->cryptoKey, cri->cri_key, cri->cri_klen/8);
			dprintk("%s,%d: key length %d \n", __FILE__, __LINE__, cri->cri_klen/8);
			cesa_ses->cryptoKeyLength = cri->cri_klen/8;
			encrypt += count;
			break;
                case CRYPTO_3DES_CBC:
			dprintk("%s,%d: (%d) 3DES CBC \n", __FILE__, __LINE__, count);
			cesa_ocf_cur_ses->cipher_alg = cri->cri_alg;
			cesa_ocf_cur_ses->ivlen = MV_CESA_3DES_BLOCK_SIZE;
			cesa_ses->cryptoAlgorithm = MV_CESA_CRYPTO_3DES;
			cesa_ses->cryptoMode = MV_CESA_CRYPTO_CBC;
			if(cri->cri_klen/8 > MV_CESA_MAX_CRYPTO_KEY_LENGTH) {
        			printk("%s,%d: CRYPTO key too long.\n", __FILE__, __LINE__);
        			goto error;
			}
			memcpy(cesa_ses->cryptoKey, cri->cri_key, cri->cri_klen/8);
			cesa_ses->cryptoKeyLength = cri->cri_klen/8;
			encrypt += count;
			break;
                case CRYPTO_DES_CBC:
			dprintk("%s,%d: (%d) DES CBC \n", __FILE__, __LINE__, count);
			cesa_ocf_cur_ses->cipher_alg = cri->cri_alg;
			cesa_ocf_cur_ses->ivlen = MV_CESA_DES_BLOCK_SIZE;
			cesa_ses->cryptoAlgorithm = MV_CESA_CRYPTO_DES;
			cesa_ses->cryptoMode = MV_CESA_CRYPTO_CBC;
			if(cri->cri_klen/8 > MV_CESA_MAX_CRYPTO_KEY_LENGTH) {
        			printk("%s,%d: CRYPTO key too long.\n", __FILE__, __LINE__);
        			goto error;
			}
			memcpy(cesa_ses->cryptoKey, cri->cri_key, cri->cri_klen/8);
			cesa_ses->cryptoKeyLength = cri->cri_klen/8;
			encrypt += count;
			break;
                case CRYPTO_MD5:
                case CRYPTO_MD5_HMAC:
			dprintk("%s,%d: (%d) %sMD5 CBC \n", __FILE__, __LINE__, count, (cri->cri_alg != CRYPTO_MD5)? "H-":" ");
                        cesa_ocf_cur_ses->auth_alg = cri->cri_alg;
			cesa_ocf_cur_ses->digestlen = (cri->cri_alg == CRYPTO_MD5)? MV_CESA_MD5_DIGEST_SIZE : 12;
			cesa_ses->macMode = (cri->cri_alg == CRYPTO_MD5)? MV_CESA_MAC_MD5 : MV_CESA_MAC_HMAC_MD5;
			if(cri->cri_klen/8 > MV_CESA_MAX_CRYPTO_KEY_LENGTH) {
        			printk("%s,%d: MAC key too long. \n", __FILE__, __LINE__);
        			goto error;
			}
			cesa_ses->macKeyLength = cri->cri_klen/8;
			memcpy(cesa_ses->macKey, cri->cri_key, cri->cri_klen/8);
			cesa_ses->digestSize = cesa_ocf_cur_ses->digestlen; 
			auth += count;
			break;
                case CRYPTO_SHA1:
                case CRYPTO_SHA1_HMAC:
			dprintk("%s,%d: (%d) %sSHA1 CBC \n", __FILE__, __LINE__, count, (cri->cri_alg != CRYPTO_SHA1)? "H-":" ");
                        cesa_ocf_cur_ses->auth_alg = cri->cri_alg;
			cesa_ocf_cur_ses->digestlen = (cri->cri_alg == CRYPTO_SHA1)? MV_CESA_SHA1_DIGEST_SIZE : 12; 
			cesa_ses->macMode = (cri->cri_alg == CRYPTO_SHA1)? MV_CESA_MAC_SHA1 : MV_CESA_MAC_HMAC_SHA1;
			if(cri->cri_klen/8 > MV_CESA_MAX_CRYPTO_KEY_LENGTH) {
        			printk("%s,%d: MAC key too long. \n", __FILE__, __LINE__);
        			goto error;
			}
			cesa_ses->macKeyLength = cri->cri_klen/8;
			memcpy(cesa_ses->macKey, cri->cri_key, cri->cri_klen/8);
			cesa_ses->digestSize = cesa_ocf_cur_ses->digestlen;
			auth += count;
			break;
                default:
                        printk("%s,%d: unknown algo 0x%x\n", __FILE__, __LINE__, cri->cri_alg);
                        goto error;
                }
                cri = cri->cri_next;
		count++;
        }

	if((encrypt > 2) || (auth > 2)) {
		printk("%s,%d: session mode is not supported.\n", __FILE__, __LINE__);
                goto error;
	}
	/* create new sessions in HAL */
	if(encrypt) {
		cesa_ses->operation = MV_CESA_CRYPTO_ONLY;
		/* encrypt session */
		if(auth == 1) {
			cesa_ses->operation = MV_CESA_MAC_THEN_CRYPTO;
		}
		else if(auth == 2) {
			cesa_ses->operation = MV_CESA_CRYPTO_THEN_MAC;
			cesa_ocf_cur_ses->encrypt_tn_auth = 1;
		}
		else {
			cesa_ses->operation = MV_CESA_CRYPTO_ONLY;
		}
		cesa_ses->direction = MV_CESA_DIR_ENCODE;
		status = mvCesaSessionOpen(cesa_ses, &cesa_ocf_cur_ses->sid_encrypt);
    		if(status != MV_OK) {
        		printk("%s,%d: Can't open new session - status = 0x%x\n", __FILE__, __LINE__, status);
        		goto error;
    		}	
		/* decrypt session */
		if( cesa_ses->operation == MV_CESA_MAC_THEN_CRYPTO ) {
			cesa_ses->operation = MV_CESA_CRYPTO_THEN_MAC;
		}
		else if( cesa_ses->operation == MV_CESA_CRYPTO_THEN_MAC ) {
			cesa_ses->operation = MV_CESA_MAC_THEN_CRYPTO;
		}
		cesa_ses->direction = MV_CESA_DIR_DECODE;
		status = mvCesaSessionOpen(cesa_ses, &cesa_ocf_cur_ses->sid_decrypt);
		if(status != MV_OK) {
        		printk("%s,%d: Can't open new session - status = 0x%x\n", __FILE__, __LINE__, status);
        		goto error;
    		}

		/* preapre one action sessions for case we will need to split an action */
#ifdef CESA_OCF_SPLIT
		if(( cesa_ses->operation == MV_CESA_MAC_THEN_CRYPTO ) || 
			( cesa_ses->operation == MV_CESA_CRYPTO_THEN_MAC )) {
			/* open one session for encode and one for decode */
			cesa_ses->operation = MV_CESA_CRYPTO_ONLY;
			cesa_ses->direction = MV_CESA_DIR_ENCODE;
			status = mvCesaSessionOpen(cesa_ses, &cesa_ocf_cur_ses->frag_wa_encrypt);
    			if(status != MV_OK) {
        			printk("%s,%d: Can't open new session - status = 0x%x\n", __FILE__, __LINE__, status);
        			goto error;
    			}

			cesa_ses->direction = MV_CESA_DIR_DECODE;
			status = mvCesaSessionOpen(cesa_ses, &cesa_ocf_cur_ses->frag_wa_decrypt);
    			if(status != MV_OK) {
        			printk("%s,%d: Can't open new session - status = 0x%x\n", __FILE__, __LINE__, status);
        			goto error;
    			}
			/* open one session for auth */	
			cesa_ses->operation = MV_CESA_MAC_ONLY;
			cesa_ses->direction = MV_CESA_DIR_ENCODE;
			status = mvCesaSessionOpen(cesa_ses, &cesa_ocf_cur_ses->frag_wa_auth);
			if(status != MV_OK) {
        			printk("%s,%d: Can't open new session - status = 0x%x\n", __FILE__, __LINE__, status);
				goto error;
    			}
		}
#endif
	}
	else { /* only auth */
		cesa_ses->operation = MV_CESA_MAC_ONLY;
		cesa_ses->direction = MV_CESA_DIR_ENCODE;
        	status = mvCesaSessionOpen(cesa_ses, &cesa_ocf_cur_ses->sid_encrypt);
		if(status != MV_OK) {
        		printk("%s,%d: Can't open new session - status = 0x%x\n", __FILE__, __LINE__, status);
			goto error;
    		}
	}
	
        return 0;
error:
     	cesa_ocf_freesession(NULL, *sid);
      	return EINVAL;	

}


/*
 * Free a session.
 */
static int
cesa_ocf_freesession(device_t dev, u_int64_t tid)
{
        struct cesa_ocf_data *cesa_ocf_cur_ses;
        u_int32_t sid = CRYPTO_SESID2LID(tid);
	//unsigned long flags;

        dprintk("%s() %d \n", __FUNCTION__, sid);
        if ( (sid >= CESA_OCF_MAX_SES) || (cesa_ocf_sessions[sid] == NULL) ) {
                printk("%s,%d: EINVAL can't free session %d \n", __FILE__, __LINE__, sid);
                return(EINVAL);
        }

        /* Silently accept and return */
        if (sid == 0)
                return(0);

	/* release session from HAL */
	cesa_ocf_cur_ses = cesa_ocf_sessions[sid];
     	if (cesa_ocf_cur_ses->sid_encrypt != -1) {
		mvCesaSessionClose(cesa_ocf_cur_ses->sid_encrypt);
	}
	if (cesa_ocf_cur_ses->sid_decrypt != -1) {
		mvCesaSessionClose(cesa_ocf_cur_ses->sid_decrypt);
	}
     	if (cesa_ocf_cur_ses->frag_wa_encrypt != -1) {
		mvCesaSessionClose(cesa_ocf_cur_ses->frag_wa_encrypt);
	}
	if (cesa_ocf_cur_ses->frag_wa_decrypt != -1) {
		mvCesaSessionClose(cesa_ocf_cur_ses->frag_wa_decrypt);
	}
	if (cesa_ocf_cur_ses->frag_wa_auth != -1) {
		mvCesaSessionClose(cesa_ocf_cur_ses->frag_wa_auth);
	}

      	kfree(cesa_ocf_cur_ses);
	cesa_ocf_sessions[sid] = NULL;

        return 0;
}


/* TDMA Window setup */

static void __init
setup_tdma_mbus_windows(struct cesa_dev *dev)
{
    int i;
    
    for (i = 0; i < 4; i++) {
        writel(0, dev->reg + WINDOW_BASE(i));
        writel(0, dev->reg + WINDOW_CTRL(i));
    }
    
    for (i = 0; i < dev->plat_data->dram->num_cs; i++) {
        struct mbus_dram_window *cs = dev->plat_data->dram->cs + i;
        writel(
            ((cs->size - 1) & 0xffff0000) |
            (cs->mbus_attr << 8) |
            (dev->plat_data->dram->mbus_dram_target_id << 4) | 1,
            dev->reg + WINDOW_CTRL(i)
        );
        writel(cs->base, dev->reg + WINDOW_BASE(i));
    }
}
                                        
/*
 * our driver startup and shutdown routines
 */
static int
mv_cesa_ocf_init(struct platform_device *pdev)
{
#if defined(CONFIG_MV78200) || defined(CONFIG_MV632X)
	if (MV_FALSE == mvSocUnitIsMappedToThisCpu(CESA))
	{
		dprintk("CESA is not mapped to this CPU\n");
		return -ENODEV;
	}		
#endif

	dprintk("%s\n", __FUNCTION__);
	memset(&mv_cesa_dev, 0, sizeof(mv_cesa_dev));
	softc_device_init(&mv_cesa_dev, "MV CESA", 0, mv_cesa_methods);
	cesa_ocf_id = crypto_get_driverid(softc_get_device(&mv_cesa_dev),CRYPTOCAP_F_HARDWARE);

	if (cesa_ocf_id < 0)
		panic("MV CESA crypto device cannot initialize!");

	dprintk("%s,%d: cesa ocf device id is %d \n", __FILE__, __LINE__, cesa_ocf_id);

	/* CESA unit is auto power on off */
#if 0
	if (MV_FALSE == mvCtrlPwrClckGet(CESA_UNIT_ID,0))
	{
		printk("\nWarning CESA %d is Powered Off\n",0);
		return EINVAL;
	}
#endif

	memset(&cesa_device, 0, sizeof(struct cesa_dev));
	/* Get the IRQ, and crypto memory regions */
	{
		struct resource *res;
		res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "sram");
		
		if (!res)
			return -ENXIO;
		
		cesa_device.sram = ioremap(res->start, res->end - res->start + 1);
		res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "regs");
		
		if (!res) {
		        iounmap(cesa_device.sram);
			return -ENXIO;
                }
                cesa_device.reg = ioremap(res->start, res->end - res->start + 1);
		cesa_device.irq = platform_get_irq(pdev, 0);
		cesa_device.plat_data = pdev->dev.platform_data;
	        setup_tdma_mbus_windows(&cesa_device);	
		
	}
	
	
	if( MV_OK != mvCesaInit(CESA_OCF_MAX_SES*5, CESA_Q_SIZE, cesa_device.reg,
				NULL) ) {
            	printk("%s,%d: mvCesaInit Failed. \n", __FILE__, __LINE__);
		return EINVAL;
	}

	/* clear and unmask Int */
	MV_REG_WRITE( MV_CESA_ISR_CAUSE_REG, 0);
#ifndef CESA_OCF_POLLING
    MV_REG_WRITE( MV_CESA_ISR_MASK_REG, MV_CESA_CAUSE_ACC_DMA_MASK);
#endif
#ifdef CESA_OCF_TASKLET
	tasklet_init(&cesa_ocf_tasklet, cesa_callback, (unsigned int) 0);
#endif
	/* register interrupt */
	if( request_irq( cesa_device.irq, cesa_interrupt_handler,
                             (IRQF_DISABLED) , "cesa", &cesa_ocf_id) < 0) {
            	printk("%s,%d: cannot assign irq %x\n", __FILE__, __LINE__, cesa_device.reg);
		return EINVAL;
        }


	memset(cesa_ocf_sessions, 0, sizeof(struct cesa_ocf_data *) * CESA_OCF_MAX_SES);

#define	REGISTER(alg) \
	crypto_register(cesa_ocf_id, alg, 0,0)
	REGISTER(CRYPTO_AES_CBC);
	REGISTER(CRYPTO_DES_CBC);
	REGISTER(CRYPTO_3DES_CBC);
	REGISTER(CRYPTO_MD5);
	REGISTER(CRYPTO_MD5_HMAC);
	REGISTER(CRYPTO_SHA1);
	REGISTER(CRYPTO_SHA1_HMAC);
#undef REGISTER

	return 0;
}

static void
mv_cesa_ocf_exit(struct platform_device *pdev)
{
	dprintk("%s()\n", __FUNCTION__);

	crypto_unregister_all(cesa_ocf_id);
	cesa_ocf_id = -1;
	iounmap(cesa_device.reg);
	iounmap(cesa_device.sram);
	free_irq(cesa_device.irq, NULL);
	
	/* mask and clear Int */
	MV_REG_WRITE( MV_CESA_ISR_MASK_REG, 0);
	MV_REG_WRITE( MV_CESA_ISR_CAUSE_REG, 0);
    	

	if( MV_OK != mvCesaFinish() ) {
            	printk("%s,%d: mvCesaFinish Failed. \n", __FILE__, __LINE__);
		return;
	}
}


void cesa_ocf_debug(void)
{

#ifdef CESA_OCF_TRACE_DEBUG
    {
        int i, j;
        j = cesaTestTraceIdx;
        mvOsPrintf("No  Type   rCause   iCause   Proc   Isr   Res     Time     pReady    pProc    pEmpty\n");
        for(i=0; i<MV_CESA_TEST_TRACE_SIZE; i++)
        {
            mvOsPrintf("%02d.  %d   0x%04x   0x%04x   0x%02x   0x%02x   %02d   0x%06x  %p  %p  %p\n",
                j, cesaTestTrace[j].type, cesaTestTrace[j].realCause,
                cesaTestTrace[j].idmaCause, 
                cesaTestTrace[j].resources, cesaTestTrace[j].timeStamp,
                cesaTestTrace[j].pReqReady, cesaTestTrace[j].pReqProcess, cesaTestTrace[j].pReqEmpty);
            j++;
            if(j == MV_CESA_TEST_TRACE_SIZE)
                j = 0;
        }
    }
#endif

}

static struct platform_driver marvell_cesa = {
	.probe		= mv_cesa_ocf_init,
	.remove		= mv_cesa_ocf_exit,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "mv_crypto",
	},
};

MODULE_ALIAS("platform:mv_crypto");

static int __init mv_cesa_init(void)
{
	return platform_driver_register(&marvell_cesa);
}

module_init(mv_cesa_init);

static void __exit mv_cesa_exit(void)
{
	platform_driver_unregister(&marvell_cesa);
}

module_exit(mv_cesa_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ronen Shitrit");
MODULE_DESCRIPTION("OCF module for Orion CESA crypto");
