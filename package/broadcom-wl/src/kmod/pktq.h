/*
 * Misc useful os-independent macros and functions.
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id: bcmutils.h,v 1.1.1.16 2006/04/08 06:13:39 honor Exp $
 */

#ifndef	_pktq_h_
#define	_pktq_h_
#include <osl.h>

/* osl multi-precedence packet queue */

#define PKTQ_LEN_DEFAULT        128	/* Max 128 packets */
#define PKTQ_MAX_PREC           16	/* Maximum precedence levels */

struct pktq {
	struct pktq_prec {
		void *head;     /* first packet to dequeue */
		void *tail;     /* last packet to dequeue */
		uint16 len;     /* number of queued packets */
		uint16 max;     /* maximum number of queued packets */
	} q[PKTQ_MAX_PREC];
	uint16 num_prec;        /* number of precedences in use */
	uint16 hi_prec;         /* rapid dequeue hint (>= highest non-empty prec) */
	uint16 max;             /* total max packets */
	uint16 len;             /* total number of packets */
};

#define PKTQ_PREC_ITER(pq, prec)        for (prec = (pq)->num_prec - 1; prec >= 0; prec--)

/* forward definition of ether_addr structure used by some function prototypes */

struct ether_addr;

/* operations on a specific precedence in packet queue */

#define pktq_psetmax(pq, prec, _max)    ((pq)->q[prec].max = (_max))
#define pktq_plen(pq, prec)             ((pq)->q[prec].len)
#define pktq_pavail(pq, prec)           ((pq)->q[prec].max - (pq)->q[prec].len)
#define pktq_pfull(pq, prec)            ((pq)->q[prec].len >= (pq)->q[prec].max)
#define pktq_pempty(pq, prec)           ((pq)->q[prec].len == 0)

#define pktq_ppeek(pq, prec)            ((pq)->q[prec].head)
#define pktq_ppeek_tail(pq, prec)       ((pq)->q[prec].tail)

extern void *pktq_penq(struct pktq *pq, int prec, void *p);
extern void *pktq_penq_head(struct pktq *pq, int prec, void *p);
extern void *pktq_pdeq(struct pktq *pq, int prec);
extern void *pktq_pdeq_tail(struct pktq *pq, int prec);
/* Empty the queue at particular precedence level */
extern void pktq_pflush(osl_t *osh, struct pktq *pq, int prec, bool dir);
/* Remove a specified packet from its queue */
extern bool pktq_pdel(struct pktq *pq, void *p, int prec);

/* operations on a set of precedences in packet queue */

extern int pktq_mlen(struct pktq *pq, uint prec_bmp);
extern void *pktq_mdeq(struct pktq *pq, uint prec_bmp, int *prec_out);

/* operations on packet queue as a whole */

#define pktq_len(pq)                    ((int)(pq)->len)
#define pktq_max(pq)                    ((int)(pq)->max)
#define pktq_avail(pq)                  ((int)((pq)->max - (pq)->len))
#define pktq_full(pq)                   ((pq)->len >= (pq)->max)
#define pktq_empty(pq)                  ((pq)->len == 0)

/* operations for single precedence queues */
#define pktenq(pq, p)		pktq_penq((pq), 0, (p))
#define pktenq_head(pq, p)	pktq_penq_head((pq), 0, (p))
#define pktdeq(pq)		pktq_pdeq((pq), 0)
#define pktdeq_tail(pq)		pktq_pdeq_tail((pq), 0)

extern void pktq_init(struct pktq *pq, int num_prec, int max_len);
/* prec_out may be NULL if caller is not interested in return value */
extern void *pktq_deq(struct pktq *pq, int *prec_out);
extern void *pktq_deq_tail(struct pktq *pq, int *prec_out);
extern void *pktq_peek(struct pktq *pq, int *prec_out);
extern void *pktq_peek_tail(struct pktq *pq, int *prec_out);
extern void pktq_flush(osl_t *osh, struct pktq *pq, bool dir); /* Empty the entire queue */

/* externs */
/* packet */
extern uint pktcopy(osl_t *osh, void *p, uint offset, int len, uchar *buf);
extern uint pkttotlen(osl_t *osh, void *p);
extern void *pktlast(osl_t *osh, void *p);

extern void pktsetprio(void *pkt, bool update_vtag);

#endif	/* _pktq_h_ */
