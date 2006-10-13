/*
 * Misc useful OS-independent routines.
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id: bcmutils.c,v 1.1.1.12 2006/02/27 03:43:16 honor Exp $
 */

#include <typedefs.h>
#include <bcmdefs.h>
#include <stdarg.h>
#include <osl.h>
#include "linux_osl.h"
#include "pktq.h"
#include <bcmutils.h>
#include <sbutils.h>
#include <bcmnvram.h>
#include <bcmendian.h>
#include <bcmdevs.h>
#include "bcmip.h"

#define ETHER_TYPE_8021Q       0x8100
#define ETHER_TYPE_IP          0x0800
#define VLAN_PRI_SHIFT             13
#define VLAN_PRI_MASK               7


struct  ether_header {
	uint8   ether_dhost[6];
	uint8   ether_shost[6];
	uint16  ether_type;
} __attribute__((packed));


struct ethervlan_header {
	uint8   ether_dhost[6];
	uint8   ether_shost[6];
	uint16  vlan_type;		/* 0x8100 */
	uint16  vlan_tag;		/* priority, cfi and vid */
	uint16  ether_type;
};

/* copy a pkt buffer chain into a buffer */
uint
pktcopy(osl_t *osh, void *p, uint offset, int len, uchar *buf)
{
	uint n, ret = 0;

	if (len < 0)
		len = 4096;	/* "infinite" */

	/* skip 'offset' bytes */
	for (; p && offset; p = PKTNEXT(osh, p)) {
		if (offset < (uint)PKTLEN(osh, p))
			break;
		offset -= PKTLEN(osh, p);
	}

	if (!p)
		return 0;

	/* copy the data */
	for (; p && len; p = PKTNEXT(osh, p)) {
		n = MIN((uint)PKTLEN(osh, p) - offset, (uint)len);
		bcopy(PKTDATA(osh, p) + offset, buf, n);
		buf += n;
		len -= n;
		ret += n;
		offset = 0;
	}

	return ret;
}

/* return total length of buffer chain */
uint
pkttotlen(osl_t *osh, void *p)
{
	uint total;

	total = 0;
	for (; p; p = PKTNEXT(osh, p))
		total += PKTLEN(osh, p);
	return (total);
}

/* return the last buffer of chained pkt */
void *
pktlast(osl_t *osh, void *p)
{
	for (; PKTNEXT(osh, p); p = PKTNEXT(osh, p))
		;

	return (p);
}


/*
 * osl multiple-precedence packet queue
 * hi_prec is always >= the number of the highest non-empty queue
 */
void *
pktq_penq(struct pktq *pq, int prec, void *p)
{
	struct pktq_prec *q;

	ASSERT(prec >= 0 && prec < pq->num_prec);
	ASSERT(PKTLINK(p) == NULL);         /* queueing chains not allowed */

	ASSERT(!pktq_full(pq));
	ASSERT(!pktq_pfull(pq, prec));

	q = &pq->q[prec];

	if (q->head)
		PKTSETLINK(q->tail, p);
	else
		q->head = p;

	q->tail = p;
	q->len++;

	pq->len++;

	if (pq->hi_prec < prec)
		pq->hi_prec = (uint8)prec;

	return p;
}

void *
pktq_penq_head(struct pktq *pq, int prec, void *p)
{
	struct pktq_prec *q;

	ASSERT(prec >= 0 && prec < pq->num_prec);
	ASSERT(PKTLINK(p) == NULL);         /* queueing chains not allowed */

	ASSERT(!pktq_full(pq));
	ASSERT(!pktq_pfull(pq, prec));

	q = &pq->q[prec];

	if (q->head == NULL)
		q->tail = p;

	PKTSETLINK(p, q->head);
	q->head = p;
	q->len++;

	pq->len++;

	if (pq->hi_prec < prec)
		pq->hi_prec = (uint8)prec;

	return p;
}

void *
pktq_pdeq(struct pktq *pq, int prec)
{
	struct pktq_prec *q;
	void *p;

	ASSERT(prec >= 0 && prec < pq->num_prec);

	q = &pq->q[prec];

	if ((p = q->head) == NULL)
		return NULL;

	if ((q->head = PKTLINK(p)) == NULL)
		q->tail = NULL;

	q->len--;

	pq->len--;

	PKTSETLINK(p, NULL);

	return p;
}

void *
pktq_pdeq_tail(struct pktq *pq, int prec)
{
	struct pktq_prec *q;
	void *p, *prev;

	ASSERT(prec >= 0 && prec < pq->num_prec);

	q = &pq->q[prec];

	if ((p = q->head) == NULL)
		return NULL;

	for (prev = NULL; p != q->tail; p = PKTLINK(p))
		prev = p;

	if (prev)
		PKTSETLINK(prev, NULL);
	else
		q->head = NULL;

	q->tail = prev;
	q->len--;

	pq->len--;

	return p;
}

void
pktq_pflush(osl_t *osh, struct pktq *pq, int prec, bool dir)
{
	struct pktq_prec *q;
	void *p;

	q = &pq->q[prec];
	p = q->head;
	while (p) {
		q->head = PKTLINK(p);
		PKTSETLINK(p, NULL);
		PKTFREE(osh, p, dir);
		q->len--;
		pq->len--;
		p = q->head;
	}
	ASSERT(q->len == 0);
	q->tail = NULL;
}

bool
pktq_pdel(struct pktq *pq, void *pktbuf, int prec)
{
	struct pktq_prec *q;
	void *p;

	ASSERT(prec >= 0 && prec < pq->num_prec);

	if (!pktbuf)
		return FALSE;

	q = &pq->q[prec];

	if (q->head == pktbuf) {
		if ((q->head = PKTLINK(pktbuf)) == NULL)
			q->tail = NULL;
	} else {
		for (p = q->head; p && PKTLINK(p) != pktbuf; p = PKTLINK(p))
			;
		if (p == NULL)
			return FALSE;

		PKTSETLINK(p, PKTLINK(pktbuf));
		if (q->tail == pktbuf)
			q->tail = p;
	}

	q->len--;
	pq->len--;
	PKTSETLINK(pktbuf, NULL);
	return TRUE;
}

void
pktq_init(struct pktq *pq, int num_prec, int max_len)
{
	int prec;

	ASSERT(num_prec > 0 && num_prec <= PKTQ_MAX_PREC);

	bzero(pq, sizeof(*pq));

	pq->num_prec = (uint16)num_prec;

	pq->max = (uint16)max_len;

	for (prec = 0; prec < num_prec; prec++)
		pq->q[prec].max = pq->max;
}

void *
pktq_deq(struct pktq *pq, int *prec_out)
{
	struct pktq_prec *q;
	void *p;
	int prec;

	if (pq->len == 0)
		return NULL;

	while ((prec = pq->hi_prec) > 0 && pq->q[prec].head == NULL)
		pq->hi_prec--;

	q = &pq->q[prec];

	if ((p = q->head) == NULL)
		return NULL;

	if ((q->head = PKTLINK(p)) == NULL)
		q->tail = NULL;

	q->len--;

	pq->len--;

	if (prec_out)
		*prec_out = prec;

	PKTSETLINK(p, NULL);

	return p;
}

void *
pktq_deq_tail(struct pktq *pq, int *prec_out)
{
	struct pktq_prec *q;
	void *p, *prev;
	int prec;

	if (pq->len == 0)
		return NULL;

	for (prec = 0; prec < pq->hi_prec; prec++)
		if (pq->q[prec].head)
			break;

	q = &pq->q[prec];

	if ((p = q->head) == NULL)
		return NULL;

	for (prev = NULL; p != q->tail; p = PKTLINK(p))
		prev = p;

	if (prev)
		PKTSETLINK(prev, NULL);
	else
		q->head = NULL;

	q->tail = prev;
	q->len--;

	pq->len--;

	if (prec_out)
		*prec_out = prec;

	PKTSETLINK(p, NULL);

	return p;
}

void *
pktq_peek(struct pktq *pq, int *prec_out)
{
	int prec;

	if (pq->len == 0)
		return NULL;

	while ((prec = pq->hi_prec) > 0 && pq->q[prec].head == NULL)
		pq->hi_prec--;

	if (prec_out)
		*prec_out = prec;

	return (pq->q[prec].head);
}

void *
pktq_peek_tail(struct pktq *pq, int *prec_out)
{
	int prec;

	if (pq->len == 0)
		return NULL;

	for (prec = 0; prec < pq->hi_prec; prec++)
		if (pq->q[prec].head)
			break;

	if (prec_out)
		*prec_out = prec;

	return (pq->q[prec].tail);
}

void
pktq_flush(osl_t *osh, struct pktq *pq, bool dir)
{
	int prec;
	for (prec = 0; prec < pq->num_prec; prec++)
		pktq_pflush(osh, pq, prec, dir);
	ASSERT(pq->len == 0);
}

/* Return sum of lengths of a specific set of precedences */
int
pktq_mlen(struct pktq *pq, uint prec_bmp)
{
	int prec, len;

	len = 0;

	for (prec = 0; prec <= pq->hi_prec; prec++)
		if (prec_bmp & (1 << prec))
			len += pq->q[prec].len;

	return len;
}

/* Priority dequeue from a specific set of precedences */
void *
pktq_mdeq(struct pktq *pq, uint prec_bmp, int *prec_out)
{
	struct pktq_prec *q;
	void *p;
	int prec;

	if (pq->len == 0)
		return NULL;

	while ((prec = pq->hi_prec) > 0 && pq->q[prec].head == NULL)
		pq->hi_prec--;

	while ((prec_bmp & (1 << prec)) == 0 || pq->q[prec].head == NULL)
		if (prec-- == 0)
			return NULL;

	q = &pq->q[prec];

	if ((p = q->head) == NULL)
		return NULL;

	if ((q->head = PKTLINK(p)) == NULL)
		q->tail = NULL;

	q->len--;

	if (prec_out)
		*prec_out = prec;

	pq->len--;

	PKTSETLINK(p, NULL);

	return p;
}

char*
bcmstrcat(char *dest, const char *src)
{
	strcpy(&dest[strlen(dest)], src);
	return (dest);
}

char*
bcm_ether_ntoa(struct ether_addr *ea, char *buf)
{
	sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",
		ea->octet[0]&0xff, ea->octet[1]&0xff, ea->octet[2]&0xff,
		ea->octet[3]&0xff, ea->octet[4]&0xff, ea->octet[5]&0xff);
	return (buf);
}

/* parse a xx:xx:xx:xx:xx:xx format ethernet address */
int
bcm_ether_atoe(char *p, struct ether_addr *ea)
{
	int i = 0;

	for (;;) {
		ea->octet[i++] = (char) bcm_strtoul(p, &p, 16);
		if (!*p++ || i == 6)
			break;
	}

	return (i == 6);
}

/* Takes an Ethernet frame and sets out-of-bound PKTPRIO
 * Also updates the inplace vlan tag if requested
 */
void
pktsetprio(void *pkt, bool update_vtag)
{
	struct ether_header *eh;
	struct ethervlan_header *evh;
	uint8 *pktdata;
	int priority = 0;

	pktdata = (uint8 *) PKTDATA(NULL, pkt);
	ASSERT(ISALIGNED((uintptr)pktdata, sizeof(uint16)));

	eh = (struct ether_header *) pktdata;

	if (ntoh16(eh->ether_type) == ETHER_TYPE_8021Q) {
		uint16 vlan_tag;
		int vlan_prio, dscp_prio = 0;

		evh = (struct ethervlan_header *)eh;

		vlan_tag = ntoh16(evh->vlan_tag);
		vlan_prio = (int) (vlan_tag >> VLAN_PRI_SHIFT) & VLAN_PRI_MASK;

		if (ntoh16(evh->ether_type) == ETHER_TYPE_IP) {
			uint8 *ip_body = pktdata + sizeof(struct ethervlan_header);
			uint8 tos_tc = IP_TOS(ip_body);
			dscp_prio = (int)(tos_tc >> IPV4_TOS_PREC_SHIFT);
		}

		/* DSCP priority gets precedence over 802.1P (vlan tag) */
		priority = (dscp_prio != 0) ? dscp_prio : vlan_prio;

		/* 
		 * If the DSCP priority is not the same as the VLAN priority,
		 * then overwrite the priority field in the vlan tag, with the
		 * DSCP priority value. This is required for Linux APs because
		 * the VLAN driver on Linux, overwrites the skb->priority field
		 * with the priority value in the vlan tag
		 */
		if (update_vtag && (priority != vlan_prio)) {
			vlan_tag &= ~(VLAN_PRI_MASK << VLAN_PRI_SHIFT);
			vlan_tag |= (uint16)priority << VLAN_PRI_SHIFT;
			evh->vlan_tag = hton16(vlan_tag);
		}
	} else if (ntoh16(eh->ether_type) == ETHER_TYPE_IP) {
		uint8 *ip_body = pktdata + sizeof(struct ether_header);
		uint8 tos_tc = IP_TOS(ip_body);
		priority = (int)(tos_tc >> IPV4_TOS_PREC_SHIFT);
	}

	ASSERT(priority >= 0 && priority <= MAXPRIO);
	PKTSETPRIO(pkt, priority);
}

static char bcm_undeferrstr[BCME_STRLEN];

static const char *bcmerrorstrtable[] = BCMERRSTRINGTABLE;

/* Convert the Error codes into related Error strings  */
const char *
bcmerrorstr(int bcmerror)
{
	int abs_bcmerror;

	abs_bcmerror = ABS(bcmerror);

	/* check if someone added a bcmerror code but forgot to add errorstring */
	ASSERT(ABS(BCME_LAST) == (ARRAYSIZE(bcmerrorstrtable) - 1));
	if ((bcmerror > 0) || (abs_bcmerror > ABS(BCME_LAST))) {
		sprintf(bcm_undeferrstr, "undefined Error %d", bcmerror);
		return bcm_undeferrstr;
	}

	ASSERT((strlen((char*)bcmerrorstrtable[abs_bcmerror])) < BCME_STRLEN);

	return bcmerrorstrtable[abs_bcmerror];
}


int
bcm_iovar_lencheck(const bcm_iovar_t *vi, void *arg, int len, bool set)
{
	int bcmerror = 0;

	/* length check on io buf */
	switch (vi->type) {
	case IOVT_BOOL:
	case IOVT_INT8:
	case IOVT_INT16:
	case IOVT_INT32:
	case IOVT_UINT8:
	case IOVT_UINT16:
	case IOVT_UINT32:
		/* all integers are int32 sized args at the ioctl interface */
		if (len < (int)sizeof(int)) {
			bcmerror = BCME_BUFTOOSHORT;
		}
		break;

	case IOVT_BUFFER:
		/* buffer must meet minimum length requirement */
		if (len < vi->minlen) {
			bcmerror = BCME_BUFTOOSHORT;
		}
		break;

	case IOVT_VOID:
		if (!set) {
			/* Cannot return nil... */
			bcmerror = BCME_UNSUPPORTED;
		} else if (len) {
			/* Set is an action w/o parameters */
			bcmerror = BCME_BUFTOOLONG;
		}
		break;

	default:
		/* unknown type for length check in iovar info */
		ASSERT(0);
		bcmerror = BCME_UNSUPPORTED;
	}

	return bcmerror;
}

#define CRC_INNER_LOOP(n, c, x) \
		    (c) = ((c) >> 8) ^ crc##n##_table[((c) ^ (x)) & 0xff]

static uint32 crc32_table[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
    0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

uint32
hndcrc32(
    uint8 *pdata,  /* pointer to array of data to process */
    uint   nbytes, /* number of input data bytes to process */
    uint32 crc     /* either CRC32_INIT_VALUE or previous return value */
)
{
	uint8 *pend;
#ifdef __mips__
	uint8 tmp[4];
	ulong *tptr = (ulong *)tmp;

	/* in case the beginning of the buffer isn't aligned */
	pend = (uint8 *)((uint)(pdata + 3) & 0xfffffffc);
	nbytes -= (pend - pdata);
	while (pdata < pend)
		CRC_INNER_LOOP(32, crc, *pdata++);

	/* handle bulk of data as 32-bit words */
	pend = pdata + (nbytes & 0xfffffffc);
	while (pdata < pend) {
		*tptr = *(ulong *)pdata;
		pdata += sizeof(ulong *);
		CRC_INNER_LOOP(32, crc, tmp[0]);
		CRC_INNER_LOOP(32, crc, tmp[1]);
		CRC_INNER_LOOP(32, crc, tmp[2]);
		CRC_INNER_LOOP(32, crc, tmp[3]);
	}

	/* 1-3 bytes at end of buffer */
	pend = pdata + (nbytes & 0x03);
	while (pdata < pend)
		CRC_INNER_LOOP(32, crc, *pdata++);
#else
	pend = pdata + nbytes;
	while (pdata < pend)
		CRC_INNER_LOOP(32, crc, *pdata++);
#endif /* __mips__ */

	return crc;
}


/*
 * Advance from the current 1-byte tag/1-byte length/variable-length value
 * triple, to the next, returning a pointer to the next.
 * If the current or next TLV is invalid (does not fit in given buffer length),
 * NULL is returned.
 * *buflen is not modified if the TLV elt parameter is invalid, or is decremented
 * by the TLV paramter's length if it is valid.
 */
bcm_tlv_t *
bcm_next_tlv(bcm_tlv_t *elt, int *buflen)
{
	int len;

	/* validate current elt */
	if (!bcm_valid_tlv(elt, *buflen))
		return NULL;

	/* advance to next elt */
	len = elt->len;
	elt = (bcm_tlv_t*)(elt->data + len);
	*buflen -= (2 + len);

	/* validate next elt */
	if (!bcm_valid_tlv(elt, *buflen))
		return NULL;

	return elt;
}

/*
 * Traverse a string of 1-byte tag/1-byte length/variable-length value
 * triples, returning a pointer to the substring whose first element
 * matches tag
 */
bcm_tlv_t *
bcm_parse_tlvs(void *buf, int buflen, uint key)
{
	bcm_tlv_t *elt;
	int totlen;

	elt = (bcm_tlv_t*)buf;
	totlen = buflen;

	/* find tagged parameter */
	while (totlen >= 2) {
		int len = elt->len;

		/* validate remaining totlen */
		if ((elt->id == key) && (totlen >= (len + 2)))
			return (elt);

		elt = (bcm_tlv_t*)((uint8*)elt + (len + 2));
		totlen -= (len + 2);
	}

	return NULL;
}

/*
 * Traverse a string of 1-byte tag/1-byte length/variable-length value
 * triples, returning a pointer to the substring whose first element
 * matches tag.  Stop parsing when we see an element whose ID is greater
 * than the target key.
 */
bcm_tlv_t *
bcm_parse_ordered_tlvs(void *buf, int buflen, uint key)
{
	bcm_tlv_t *elt;
	int totlen;

	elt = (bcm_tlv_t*)buf;
	totlen = buflen;

	/* find tagged parameter */
	while (totlen >= 2) {
		uint id = elt->id;
		int len = elt->len;

		/* Punt if we start seeing IDs > than target key */
		if (id > key)
			return (NULL);

		/* validate remaining totlen */
		if ((id == key) && (totlen >= (len + 2)))
			return (elt);

		elt = (bcm_tlv_t*)((uint8*)elt + (len + 2));
		totlen -= (len + 2);
	}
	return NULL;
}


/* Initialization of bcmstrbuf structure */
void
bcm_binit(struct bcmstrbuf *b, char *buf, uint size)
{
	b->origsize = b->size = size;
	b->origbuf = b->buf = buf;
}

/* Buffer sprintf wrapper to guard against buffer overflow */
int
bcm_bprintf(struct bcmstrbuf *b, const char *fmt, ...)
{
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = vsnprintf(b->buf, b->size, fmt, ap);

	/* Non Ansi C99 compliant returns -1,
	 * Ansi compliant return r >= b->size,
	 * bcmstdlib returns 0, handle all
	 */
	if ((r == -1) || (r >= (int)b->size) || (r == 0))
	{
		b->size = 0;
	}
	else
	{
		b->size -= r;
		b->buf += r;
	}

	va_end(ap);

	return r;
}
