/*
 * Copyright (c) 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /usr/local/cvs/linux/tools/build/e100boot/libpcap-0.4/gencode.c,v 1.1 1999/08/26 10:05:22 johana Exp $ (LBL)";
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#if __STDC__
struct mbuf;
struct rtentry;
#endif

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <stdlib.h>
#include <memory.h>
#include <setjmp.h>
#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include "pcap-int.h"

#include "ethertype.h"
#include "gencode.h"
#include "ppp.h"
#include <pcap-namedb.h>

#include "gnuc.h"
#ifdef HAVE_OS_PROTO_H
#include "os-proto.h"
#endif

#define JMP(c) ((c)|BPF_JMP|BPF_K)

/* Locals */
static jmp_buf top_ctx;
static pcap_t *bpf_pcap;

/* XXX */
#ifdef PCAP_FDDIPAD
int	pcap_fddipad = PCAP_FDDIPAD;
#else
int	pcap_fddipad;
#endif

/* VARARGS */
__dead void
#if __STDC__
bpf_error(const char *fmt, ...)
#else
bpf_error(fmt, va_alist)
	const char *fmt;
	va_dcl
#endif
{
	va_list ap;

#if __STDC__
	va_start(ap, fmt);
#else
	va_start(ap);
#endif
	if (bpf_pcap != NULL)
		(void)vsprintf(pcap_geterr(bpf_pcap), fmt, ap);
	va_end(ap);
	longjmp(top_ctx, 1);
	/* NOTREACHED */
}

static void init_linktype(int);

static int alloc_reg(void);
static void free_reg(int);

static struct block *root;

/*
 * We divy out chunks of memory rather than call malloc each time so
 * we don't have to worry about leaking memory.  It's probably
 * not a big deal if all this memory was wasted but it this ever
 * goes into a library that would probably not be a good idea.
 */
#define NCHUNKS 16
#define CHUNK0SIZE 1024
struct chunk {
	u_int n_left;
	void *m;
};

static struct chunk chunks[NCHUNKS];
static int cur_chunk;

static void *newchunk(u_int);
static void freechunks(void);
static inline struct block *new_block(int);
static inline struct slist *new_stmt(int);
static struct block *gen_retblk(int);
static inline void syntax(void);

static void backpatch(struct block *, struct block *);
static void merge(struct block *, struct block *);
static struct block *gen_cmp(u_int, u_int, bpf_int32);
static struct block *gen_mcmp(u_int, u_int, bpf_int32, bpf_u_int32);
static struct block *gen_bcmp(u_int, u_int, const u_char *);
static struct block *gen_uncond(int);
static inline struct block *gen_true(void);
static inline struct block *gen_false(void);
static struct block *gen_linktype(int);
static struct block *gen_hostop(bpf_u_int32, bpf_u_int32, int, int, u_int, u_int);
static struct block *gen_ehostop(const u_char *, int);
static struct block *gen_fhostop(const u_char *, int);
static struct block *gen_dnhostop(bpf_u_int32, int, u_int);
static struct block *gen_host(bpf_u_int32, bpf_u_int32, int, int);
static struct block *gen_gateway(const u_char *, bpf_u_int32 **, int, int);
static struct block *gen_ipfrag(void);
static struct block *gen_portatom(int, bpf_int32);
struct block *gen_portop(int, int, int);
static struct block *gen_port(int, int, int);
static int lookup_proto(const char *, int);
static struct block *gen_proto(int, int, int);
static struct slist *xfer_to_x(struct arth *);
static struct slist *xfer_to_a(struct arth *);
static struct block *gen_len(int, int);

static void *
newchunk(n)
	u_int n;
{
	struct chunk *cp;
	int k, size;

	/* XXX Round up to nearest long. */
	n = (n + sizeof(long) - 1) & ~(sizeof(long) - 1);

	cp = &chunks[cur_chunk];
	if (n > cp->n_left) {
		++cp, k = ++cur_chunk;
		if (k >= NCHUNKS)
			bpf_error("out of memory");
		size = CHUNK0SIZE << k;
		cp->m = (void *)malloc(size);
		memset((char *)cp->m, 0, size);
		cp->n_left = size;
		if (n > size)
			bpf_error("out of memory");
	}
	cp->n_left -= n;
	return (void *)((char *)cp->m + cp->n_left);
}

static void
freechunks()
{
	int i;

	cur_chunk = 0;
	for (i = 0; i < NCHUNKS; ++i)
		if (chunks[i].m != NULL) {
			free(chunks[i].m);
			chunks[i].m = NULL;
		}
}

/*
 * A strdup whose allocations are freed after code generation is over.
 */
char *
sdup(s)
	register const char *s;
{
	int n = strlen(s) + 1;
	char *cp = newchunk(n);

	strcpy(cp, s);
	return (cp);
}

static inline struct block *
new_block(code)
	int code;
{
	struct block *p;

	p = (struct block *)newchunk(sizeof(*p));
	p->s.code = code;
	p->head = p;

	return p;
}

static inline struct slist *
new_stmt(code)
	int code;
{
	struct slist *p;

	p = (struct slist *)newchunk(sizeof(*p));
	p->s.code = code;

	return p;
}

static struct block *
gen_retblk(v)
	int v;
{
	struct block *b = new_block(BPF_RET|BPF_K);

	b->s.k = v;
	return b;
}

static inline void
syntax()
{
	bpf_error("syntax error in filter expression");
}

static bpf_u_int32 netmask;
static int snaplen;

int
pcap_compile(pcap_t *p, struct bpf_program *program,
	     char *buf, int optimize, bpf_u_int32 mask)
{
	extern int n_errors;
	int len;

	n_errors = 0;
	root = NULL;
	bpf_pcap = p;
	if (setjmp(top_ctx)) {
		freechunks();
		return (-1);
	}

	netmask = mask;
	snaplen = pcap_snapshot(p);

	lex_init(buf ? buf : "");
	init_linktype(pcap_datalink(p));
	(void)pcap_parse();

	if (n_errors)
		syntax();

	if (root == NULL)
		root = gen_retblk(snaplen);

	if (optimize) {
		bpf_optimize(&root);
		if (root == NULL ||
		    (root->s.code == (BPF_RET|BPF_K) && root->s.k == 0))
			bpf_error("expression rejects all packets");
	}
	program->bf_insns = icode_to_fcode(root, &len);
	program->bf_len = len;

	freechunks();
	return (0);
}

/*
 * Backpatch the blocks in 'list' to 'target'.  The 'sense' field indicates
 * which of the jt and jf fields has been resolved and which is a pointer
 * back to another unresolved block (or nil).  At least one of the fields
 * in each block is already resolved.
 */
static void
backpatch(list, target)
	struct block *list, *target;
{
	struct block *next;

	while (list) {
		if (!list->sense) {
			next = JT(list);
			JT(list) = target;
		} else {
			next = JF(list);
			JF(list) = target;
		}
		list = next;
	}
}

/*
 * Merge the lists in b0 and b1, using the 'sense' field to indicate
 * which of jt and jf is the link.
 */
static void
merge(b0, b1)
	struct block *b0, *b1;
{
	register struct block **p = &b0;

	/* Find end of list. */
	while (*p)
		p = !((*p)->sense) ? &JT(*p) : &JF(*p);

	/* Concatenate the lists. */
	*p = b1;
}

void
finish_parse(p)
	struct block *p;
{
	backpatch(p, gen_retblk(snaplen));
	p->sense = !p->sense;
	backpatch(p, gen_retblk(0));
	root = p->head;
}

void
gen_and(b0, b1)
	struct block *b0, *b1;
{
	backpatch(b0, b1->head);
	b0->sense = !b0->sense;
	b1->sense = !b1->sense;
	merge(b1, b0);
	b1->sense = !b1->sense;
	b1->head = b0->head;
}

void
gen_or(b0, b1)
	struct block *b0, *b1;
{
	b0->sense = !b0->sense;
	backpatch(b0, b1->head);
	b0->sense = !b0->sense;
	merge(b1, b0);
	b1->head = b0->head;
}

void
gen_not(b)
	struct block *b;
{
	b->sense = !b->sense;
}

static struct block *
gen_cmp(offset, size, v)
	u_int offset, size;
	bpf_int32 v;
{
	struct slist *s;
	struct block *b;

	s = new_stmt(BPF_LD|BPF_ABS|size);
	s->s.k = offset;

	b = new_block(JMP(BPF_JEQ));
	b->stmts = s;
	b->s.k = v;

	return b;
}

static struct block *
gen_mcmp(offset, size, v, mask)
	u_int offset, size;
	bpf_int32 v;
	bpf_u_int32 mask;
{
	struct block *b = gen_cmp(offset, size, v);
	struct slist *s;

	if (mask != 0xffffffff) {
		s = new_stmt(BPF_ALU|BPF_AND|BPF_K);
		s->s.k = mask;
		b->stmts->next = s;
	}
	return b;
}

static struct block *
gen_bcmp(offset, size, v)
	register u_int offset, size;
	register const u_char *v;
{
	register struct block *b, *tmp;

	b = NULL;
	while (size >= 4) {
		register const u_char *p = &v[size - 4];
		bpf_int32 w = ((bpf_int32)p[0] << 24) |
		    ((bpf_int32)p[1] << 16) | ((bpf_int32)p[2] << 8) | p[3];

		tmp = gen_cmp(offset + size - 4, BPF_W, w);
		if (b != NULL)
			gen_and(b, tmp);
		b = tmp;
		size -= 4;
	}
	while (size >= 2) {
		register const u_char *p = &v[size - 2];
		bpf_int32 w = ((bpf_int32)p[0] << 8) | p[1];

		tmp = gen_cmp(offset + size - 2, BPF_H, w);
		if (b != NULL)
			gen_and(b, tmp);
		b = tmp;
		size -= 2;
	}
	if (size > 0) {
		tmp = gen_cmp(offset, BPF_B, (bpf_int32)v[0]);
		if (b != NULL)
			gen_and(b, tmp);
		b = tmp;
	}
	return b;
}

/*
 * Various code constructs need to know the layout of the data link
 * layer.  These variables give the necessary offsets.  off_linktype
 * is set to -1 for no encapsulation, in which case, IP is assumed.
 */
static u_int off_linktype;
static u_int off_nl;
static int linktype;

static void
init_linktype(type)
	int type;
{
	linktype = type;

	switch (type) {

	case DLT_EN10MB:
		off_linktype = 12;
		off_nl = 14;
		return;

	case DLT_SLIP:
		/*
		 * SLIP doesn't have a link level type.  The 16 byte
		 * header is hacked into our SLIP driver.
		 */
		off_linktype = -1;
		off_nl = 16;
		return;

	case DLT_SLIP_BSDOS:
		/* XXX this may be the same as the DLT_PPP_BSDOS case */
		off_linktype = -1;
		/* XXX end */
		off_nl = 24;
		return;

	case DLT_NULL:
		off_linktype = 0;
		off_nl = 4;
		return;

	case DLT_PPP:
		off_linktype = 2;
		off_nl = 4;
		return;

	case DLT_PPP_BSDOS:
		off_linktype = 5;
		off_nl = 24;
		return;

	case DLT_FDDI:
		/*
		 * FDDI doesn't really have a link-level type field.
		 * We assume that SSAP = SNAP is being used and pick
		 * out the encapsulated Ethernet type.
		 */
		off_linktype = 19;
#ifdef PCAP_FDDIPAD
		off_linktype += pcap_fddipad;
#endif
		off_nl = 21;
#ifdef PCAP_FDDIPAD
		off_nl += pcap_fddipad;
#endif
		return;

	case DLT_IEEE802:
		off_linktype = 20;
		off_nl = 22;
		return;

	case DLT_ATM_RFC1483:
		/*
		 * assume routed, non-ISO PDUs
		 * (i.e., LLC = 0xAA-AA-03, OUT = 0x00-00-00)
		 */
		off_linktype = 6;
		off_nl = 8;
		return;

	case DLT_RAW:
		off_linktype = -1;
		off_nl = 0;
		return;
	}
	bpf_error("unknown data link type 0x%x", linktype);
	/* NOTREACHED */
}

static struct block *
gen_uncond(rsense)
	int rsense;
{
	struct block *b;
	struct slist *s;

	s = new_stmt(BPF_LD|BPF_IMM);
	s->s.k = !rsense;
	b = new_block(JMP(BPF_JEQ));
	b->stmts = s;

	return b;
}

static inline struct block *
gen_true()
{
	return gen_uncond(1);
}

static inline struct block *
gen_false()
{
	return gen_uncond(0);
}

static struct block *
gen_linktype(proto)
	register int proto;
{
	struct block *b0, *b1;

	/* If we're not using encapsulation and checking for IP, we're done */
	if (off_linktype == -1 && proto == ETHERTYPE_IP)
		return gen_true();

	switch (linktype) {

	case DLT_SLIP:
		return gen_false();

	case DLT_PPP:
		if (proto == ETHERTYPE_IP)
			proto = PPP_IP;			/* XXX was 0x21 */
		break;

	case DLT_PPP_BSDOS:
		switch (proto) {

		case ETHERTYPE_IP:
			b0 = gen_cmp(off_linktype, BPF_H, PPP_IP);
			b1 = gen_cmp(off_linktype, BPF_H, PPP_VJC);
			gen_or(b0, b1);
			b0 = gen_cmp(off_linktype, BPF_H, PPP_VJNC);
			gen_or(b1, b0);
			return b0;

		case ETHERTYPE_DN:
			proto = PPP_DECNET;
			break;

		case ETHERTYPE_ATALK:
			proto = PPP_APPLE;
			break;

		case ETHERTYPE_NS:
			proto = PPP_NS;
			break;
		}
		break;

	case DLT_NULL:
		/* XXX */
		if (proto == ETHERTYPE_IP)
			return (gen_cmp(0, BPF_W, (bpf_int32)htonl(AF_INET)));
		else
			return gen_false();
	}
	return gen_cmp(off_linktype, BPF_H, (bpf_int32)proto);
}

static struct block *
gen_hostop(addr, mask, dir, proto, src_off, dst_off)
	bpf_u_int32 addr;
	bpf_u_int32 mask;
	int dir, proto;
	u_int src_off, dst_off;
{
	struct block *b0, *b1;
	u_int offset;

	switch (dir) {

	case Q_SRC:
		offset = src_off;
		break;

	case Q_DST:
		offset = dst_off;
		break;

	case Q_AND:
		b0 = gen_hostop(addr, mask, Q_SRC, proto, src_off, dst_off);
		b1 = gen_hostop(addr, mask, Q_DST, proto, src_off, dst_off);
		gen_and(b0, b1);
		return b1;

	case Q_OR:
	case Q_DEFAULT:
		b0 = gen_hostop(addr, mask, Q_SRC, proto, src_off, dst_off);
		b1 = gen_hostop(addr, mask, Q_DST, proto, src_off, dst_off);
		gen_or(b0, b1);
		return b1;

	default:
		abort();
	}
	b0 = gen_linktype(proto);
	b1 = gen_mcmp(offset, BPF_W, (bpf_int32)addr, mask);
	gen_and(b0, b1);
	return b1;
}

static struct block *
gen_ehostop(eaddr, dir)
	register const u_char *eaddr;
	register int dir;
{
	register struct block *b0, *b1;

	switch (dir) {
	case Q_SRC:
		return gen_bcmp(6, 6, eaddr);

	case Q_DST:
		return gen_bcmp(0, 6, eaddr);

	case Q_AND:
		b0 = gen_ehostop(eaddr, Q_SRC);
		b1 = gen_ehostop(eaddr, Q_DST);
		gen_and(b0, b1);
		return b1;

	case Q_DEFAULT:
	case Q_OR:
		b0 = gen_ehostop(eaddr, Q_SRC);
		b1 = gen_ehostop(eaddr, Q_DST);
		gen_or(b0, b1);
		return b1;
	}
	abort();
	/* NOTREACHED */
}

/*
 * Like gen_ehostop, but for DLT_FDDI
 */
static struct block *
gen_fhostop(eaddr, dir)
	register const u_char *eaddr;
	register int dir;
{
	struct block *b0, *b1;

	switch (dir) {
	case Q_SRC:
#ifdef PCAP_FDDIPAD
		return gen_bcmp(6 + 1 + pcap_fddipad, 6, eaddr);
#else
		return gen_bcmp(6 + 1, 6, eaddr);
#endif

	case Q_DST:
#ifdef PCAP_FDDIPAD
		return gen_bcmp(0 + 1 + pcap_fddipad, 6, eaddr);
#else
		return gen_bcmp(0 + 1, 6, eaddr);
#endif

	case Q_AND:
		b0 = gen_fhostop(eaddr, Q_SRC);
		b1 = gen_fhostop(eaddr, Q_DST);
		gen_and(b0, b1);
		return b1;

	case Q_DEFAULT:
	case Q_OR:
		b0 = gen_fhostop(eaddr, Q_SRC);
		b1 = gen_fhostop(eaddr, Q_DST);
		gen_or(b0, b1);
		return b1;
	}
	abort();
	/* NOTREACHED */
}

/*
 * This is quite tricky because there may be pad bytes in front of the
 * DECNET header, and then there are two possible data packet formats that
 * carry both src and dst addresses, plus 5 packet types in a format that
 * carries only the src node, plus 2 types that use a different format and
 * also carry just the src node.
 *
 * Yuck.
 *
 * Instead of doing those all right, we just look for data packets with
 * 0 or 1 bytes of padding.  If you want to look at other packets, that
 * will require a lot more hacking.
 *
 * To add support for filtering on DECNET "areas" (network numbers)
 * one would want to add a "mask" argument to this routine.  That would
 * make the filter even more inefficient, although one could be clever
 * and not generate masking instructions if the mask is 0xFFFF.
 */
static struct block *
gen_dnhostop(addr, dir, base_off)
	bpf_u_int32 addr;
	int dir;
	u_int base_off;
{
	struct block *b0, *b1, *b2, *tmp;
	u_int offset_lh;	/* offset if long header is received */
	u_int offset_sh;	/* offset if short header is received */

	switch (dir) {

	case Q_DST:
		offset_sh = 1;	/* follows flags */
		offset_lh = 7;	/* flgs,darea,dsubarea,HIORD */
		break;

	case Q_SRC:
		offset_sh = 3;	/* follows flags, dstnode */
		offset_lh = 15;	/* flgs,darea,dsubarea,did,sarea,ssub,HIORD */
		break;

	case Q_AND:
		/* Inefficient because we do our Calvinball dance twice */
		b0 = gen_dnhostop(addr, Q_SRC, base_off);
		b1 = gen_dnhostop(addr, Q_DST, base_off);
		gen_and(b0, b1);
		return b1;

	case Q_OR:
	case Q_DEFAULT:
		/* Inefficient because we do our Calvinball dance twice */
		b0 = gen_dnhostop(addr, Q_SRC, base_off);
		b1 = gen_dnhostop(addr, Q_DST, base_off);
		gen_or(b0, b1);
		return b1;

	default:
		abort();
	}
	b0 = gen_linktype(ETHERTYPE_DN);
	/* Check for pad = 1, long header case */
	tmp = gen_mcmp(base_off + 2, BPF_H,
	    (bpf_int32)ntohs(0x0681), (bpf_int32)ntohs(0x07FF));
	b1 = gen_cmp(base_off + 2 + 1 + offset_lh,
	    BPF_H, (bpf_int32)ntohs(addr));
	gen_and(tmp, b1);
	/* Check for pad = 0, long header case */
	tmp = gen_mcmp(base_off + 2, BPF_B, (bpf_int32)0x06, (bpf_int32)0x7);
	b2 = gen_cmp(base_off + 2 + offset_lh, BPF_H, (bpf_int32)ntohs(addr));
	gen_and(tmp, b2);
	gen_or(b2, b1);
	/* Check for pad = 1, short header case */
	tmp = gen_mcmp(base_off + 2, BPF_H,
	    (bpf_int32)ntohs(0x0281), (bpf_int32)ntohs(0x07FF));
	b2 = gen_cmp(base_off + 2 + 1 + offset_sh,
	    BPF_H, (bpf_int32)ntohs(addr));
	gen_and(tmp, b2);
	gen_or(b2, b1);
	/* Check for pad = 0, short header case */
	tmp = gen_mcmp(base_off + 2, BPF_B, (bpf_int32)0x02, (bpf_int32)0x7);
	b2 = gen_cmp(base_off + 2 + offset_sh, BPF_H, (bpf_int32)ntohs(addr));
	gen_and(tmp, b2);
	gen_or(b2, b1);

	/* Combine with test for linktype */
	gen_and(b0, b1);
	return b1;
}

static struct block *
gen_host(addr, mask, proto, dir)
	bpf_u_int32 addr;
	bpf_u_int32 mask;
	int proto;
	int dir;
{
	struct block *b0, *b1;

	switch (proto) {

	case Q_DEFAULT:
		b0 = gen_host(addr, mask, Q_IP, dir);
		b1 = gen_host(addr, mask, Q_ARP, dir);
		gen_or(b0, b1);
		b0 = gen_host(addr, mask, Q_RARP, dir);
		gen_or(b1, b0);
		return b0;

	case Q_IP:
		return gen_hostop(addr, mask, dir, ETHERTYPE_IP,
				  off_nl + 12, off_nl + 16);

	case Q_RARP:
		return gen_hostop(addr, mask, dir, ETHERTYPE_REVARP,
				  off_nl + 14, off_nl + 24);

	case Q_ARP:
		return gen_hostop(addr, mask, dir, ETHERTYPE_ARP,
				  off_nl + 14, off_nl + 24);

	case Q_TCP:
		bpf_error("'tcp' modifier applied to host");

	case Q_UDP:
		bpf_error("'udp' modifier applied to host");

	case Q_ICMP:
		bpf_error("'icmp' modifier applied to host");

	case Q_IGMP:
		bpf_error("'igmp' modifier applied to host");

	case Q_IGRP:
		bpf_error("'igrp' modifier applied to host");

	case Q_ATALK:
		bpf_error("ATALK host filtering not implemented");

	case Q_DECNET:
		return gen_dnhostop(addr, dir, off_nl);

	case Q_SCA:
		bpf_error("SCA host filtering not implemented");

	case Q_LAT:
		bpf_error("LAT host filtering not implemented");

	case Q_MOPDL:
		bpf_error("MOPDL host filtering not implemented");

	case Q_MOPRC:
		bpf_error("MOPRC host filtering not implemented");

	default:
		abort();
	}
	/* NOTREACHED */
}

static struct block *
gen_gateway(eaddr, alist, proto, dir)
	const u_char *eaddr;
	bpf_u_int32 **alist;
	int proto;
	int dir;
{
	struct block *b0, *b1, *tmp;

	if (dir != 0)
		bpf_error("direction applied to 'gateway'");

	switch (proto) {
	case Q_DEFAULT:
	case Q_IP:
	case Q_ARP:
	case Q_RARP:
		if (linktype == DLT_EN10MB)
			b0 = gen_ehostop(eaddr, Q_OR);
		else if (linktype == DLT_FDDI)
			b0 = gen_fhostop(eaddr, Q_OR);
		else
			bpf_error(
			    "'gateway' supported only on ethernet or FDDI");

		b1 = gen_host(**alist++, 0xffffffff, proto, Q_OR);
		while (*alist) {
			tmp = gen_host(**alist++, 0xffffffff, proto, Q_OR);
			gen_or(b1, tmp);
			b1 = tmp;
		}
		gen_not(b1);
		gen_and(b0, b1);
		return b1;
	}
	bpf_error("illegal modifier of 'gateway'");
	/* NOTREACHED */
}

struct block *
gen_proto_abbrev(proto)
	int proto;
{
	struct block *b0, *b1;

	switch (proto) {

	case Q_TCP:
		b0 = gen_linktype(ETHERTYPE_IP);
		b1 = gen_cmp(off_nl + 9, BPF_B, (bpf_int32)IPPROTO_TCP);
		gen_and(b0, b1);
		break;

	case Q_UDP:
		b0 =  gen_linktype(ETHERTYPE_IP);
		b1 = gen_cmp(off_nl + 9, BPF_B, (bpf_int32)IPPROTO_UDP);
		gen_and(b0, b1);
		break;

	case Q_ICMP:
		b0 =  gen_linktype(ETHERTYPE_IP);
		b1 = gen_cmp(off_nl + 9, BPF_B, (bpf_int32)IPPROTO_ICMP);
		gen_and(b0, b1);
		break;

	case Q_IGMP:
		b0 =  gen_linktype(ETHERTYPE_IP);
		b1 = gen_cmp(off_nl + 9, BPF_B, (bpf_int32)2);
		gen_and(b0, b1);
		break;

#ifndef	IPPROTO_IGRP
#define	IPPROTO_IGRP	9
#endif
	case Q_IGRP:
		b0 =  gen_linktype(ETHERTYPE_IP);
		b1 = gen_cmp(off_nl + 9, BPF_B, (long)IPPROTO_IGRP);
		gen_and(b0, b1);
		break;

	case Q_IP:
		b1 =  gen_linktype(ETHERTYPE_IP);
		break;

	case Q_ARP:
		b1 =  gen_linktype(ETHERTYPE_ARP);
		break;

	case Q_RARP:
		b1 =  gen_linktype(ETHERTYPE_REVARP);
		break;

	case Q_LINK:
		bpf_error("link layer applied in wrong context");

	case Q_ATALK:
		b1 =  gen_linktype(ETHERTYPE_ATALK);
		break;

	case Q_DECNET:
		b1 =  gen_linktype(ETHERTYPE_DN);
		break;

	case Q_SCA:
		b1 =  gen_linktype(ETHERTYPE_SCA);
		break;

	case Q_LAT:
		b1 =  gen_linktype(ETHERTYPE_LAT);
		break;

	case Q_MOPDL:
		b1 =  gen_linktype(ETHERTYPE_MOPDL);
		break;

	case Q_MOPRC:
		b1 =  gen_linktype(ETHERTYPE_MOPRC);
		break;

	default:
		abort();
	}
	return b1;
}

static struct block *
gen_ipfrag()
{
	struct slist *s;
	struct block *b;

	/* not ip frag */
	s = new_stmt(BPF_LD|BPF_H|BPF_ABS);
	s->s.k = off_nl + 6;
	b = new_block(JMP(BPF_JSET));
	b->s.k = 0x1fff;
	b->stmts = s;
	gen_not(b);

	return b;
}

static struct block *
gen_portatom(off, v)
	int off;
	bpf_int32 v;
{
	struct slist *s;
	struct block *b;

	s = new_stmt(BPF_LDX|BPF_MSH|BPF_B);
	s->s.k = off_nl;

	s->next = new_stmt(BPF_LD|BPF_IND|BPF_H);
	s->next->s.k = off_nl + off;

	b = new_block(JMP(BPF_JEQ));
	b->stmts = s;
	b->s.k = v;

	return b;
}

struct block *
gen_portop(port, proto, dir)
	int port, proto, dir;
{
	struct block *b0, *b1, *tmp;

	/* ip proto 'proto' */
	tmp = gen_cmp(off_nl + 9, BPF_B, (bpf_int32)proto);
	b0 = gen_ipfrag();
	gen_and(tmp, b0);

	switch (dir) {
	case Q_SRC:
		b1 = gen_portatom(0, (bpf_int32)port);
		break;

	case Q_DST:
		b1 = gen_portatom(2, (bpf_int32)port);
		break;

	case Q_OR:
	case Q_DEFAULT:
		tmp = gen_portatom(0, (bpf_int32)port);
		b1 = gen_portatom(2, (bpf_int32)port);
		gen_or(tmp, b1);
		break;

	case Q_AND:
		tmp = gen_portatom(0, (bpf_int32)port);
		b1 = gen_portatom(2, (bpf_int32)port);
		gen_and(tmp, b1);
		break;

	default:
		abort();
	}
	gen_and(b0, b1);

	return b1;
}

static struct block *
gen_port(port, ip_proto, dir)
	int port;
	int ip_proto;
	int dir;
{
	struct block *b0, *b1, *tmp;

	/* ether proto ip */
	b0 =  gen_linktype(ETHERTYPE_IP);

	switch (ip_proto) {
	case IPPROTO_UDP:
	case IPPROTO_TCP:
		b1 = gen_portop(port, ip_proto, dir);
		break;

	case PROTO_UNDEF:
		tmp = gen_portop(port, IPPROTO_TCP, dir);
		b1 = gen_portop(port, IPPROTO_UDP, dir);
		gen_or(tmp, b1);
		break;

	default:
		abort();
	}
	gen_and(b0, b1);
	return b1;
}

static int
lookup_proto(name, proto)
	register const char *name;
	register int proto;
{
	register int v;

	switch (proto) {

	case Q_DEFAULT:
	case Q_IP:
		v = pcap_nametoproto(name);
		if (v == PROTO_UNDEF)
			bpf_error("unknown ip proto '%s'", name);
		break;

	case Q_LINK:
		/* XXX should look up h/w protocol type based on linktype */
		v = pcap_nametoeproto(name);
		if (v == PROTO_UNDEF)
			bpf_error("unknown ether proto '%s'", name);
		break;

	default:
		v = PROTO_UNDEF;
		break;
	}
	return v;
}

static struct block *
gen_proto(v, proto, dir)
	int v;
	int proto;
	int dir;
{
	struct block *b0, *b1;

	if (dir != Q_DEFAULT)
		bpf_error("direction applied to 'proto'");

	switch (proto) {
	case Q_DEFAULT:
	case Q_IP:
		b0 = gen_linktype(ETHERTYPE_IP);
		b1 = gen_cmp(off_nl + 9, BPF_B, (bpf_int32)v);
		gen_and(b0, b1);
		return b1;

	case Q_ARP:
		bpf_error("arp does not encapsulate another protocol");
		/* NOTREACHED */

	case Q_RARP:
		bpf_error("rarp does not encapsulate another protocol");
		/* NOTREACHED */

	case Q_ATALK:
		bpf_error("atalk encapsulation is not specifiable");
		/* NOTREACHED */

	case Q_DECNET:
		bpf_error("decnet encapsulation is not specifiable");
		/* NOTREACHED */

	case Q_SCA:
		bpf_error("sca does not encapsulate another protocol");
		/* NOTREACHED */

	case Q_LAT:
		bpf_error("lat does not encapsulate another protocol");
		/* NOTREACHED */

	case Q_MOPRC:
		bpf_error("moprc does not encapsulate another protocol");
		/* NOTREACHED */

	case Q_MOPDL:
		bpf_error("mopdl does not encapsulate another protocol");
		/* NOTREACHED */

	case Q_LINK:
		return gen_linktype(v);

	case Q_UDP:
		bpf_error("'udp proto' is bogus");
		/* NOTREACHED */

	case Q_TCP:
		bpf_error("'tcp proto' is bogus");
		/* NOTREACHED */

	case Q_ICMP:
		bpf_error("'icmp proto' is bogus");
		/* NOTREACHED */

	case Q_IGMP:
		bpf_error("'igmp proto' is bogus");
		/* NOTREACHED */

	case Q_IGRP:
		bpf_error("'igrp proto' is bogus");
		/* NOTREACHED */

	default:
		abort();
		/* NOTREACHED */
	}
	/* NOTREACHED */
}

struct block *
gen_scode(name, q)
	register const char *name;
	struct qual q;
{
	int proto = q.proto;
	int dir = q.dir;
	int tproto;
	u_char *eaddr;
	bpf_u_int32 mask, addr, **alist;
	struct block *b, *tmp;
	int port, real_proto;

	switch (q.addr) {

	case Q_NET:
		addr = pcap_nametonetaddr(name);
		if (addr == 0)
			bpf_error("unknown network '%s'", name);
		/* Left justify network addr and calculate its network mask */
		mask = 0xffffffff;
		while (addr && (addr & 0xff000000) == 0) {
			addr <<= 8;
			mask <<= 8;
		}
		return gen_host(addr, mask, proto, dir);

	case Q_DEFAULT:
	case Q_HOST:
		if (proto == Q_LINK) {
			switch (linktype) {

			case DLT_EN10MB:
				eaddr = pcap_ether_hostton(name);
				if (eaddr == NULL)
					bpf_error(
					    "unknown ether host '%s'", name);
				return gen_ehostop(eaddr, dir);

			case DLT_FDDI:
				eaddr = pcap_ether_hostton(name);
				if (eaddr == NULL)
					bpf_error(
					    "unknown FDDI host '%s'", name);
				return gen_fhostop(eaddr, dir);

			default:
				bpf_error(
			"only ethernet/FDDI supports link-level host name");
				break;
			}
		} else if (proto == Q_DECNET) {
			unsigned short dn_addr = __pcap_nametodnaddr(name);
			/*
			 * I don't think DECNET hosts can be multihomed, so
			 * there is no need to build up a list of addresses
			 */
			return (gen_host(dn_addr, 0, proto, dir));
		} else {
			alist = pcap_nametoaddr(name);
			if (alist == NULL || *alist == NULL)
				bpf_error("unknown host '%s'", name);
			tproto = proto;
			if (off_linktype == -1 && tproto == Q_DEFAULT)
				tproto = Q_IP;
			b = gen_host(**alist++, 0xffffffff, tproto, dir);
			while (*alist) {
				tmp = gen_host(**alist++, 0xffffffff,
					       tproto, dir);
				gen_or(b, tmp);
				b = tmp;
			}
			return b;
		}

	case Q_PORT:
		if (proto != Q_DEFAULT && proto != Q_UDP && proto != Q_TCP)
			bpf_error("illegal qualifier of 'port'");
		if (pcap_nametoport(name, &port, &real_proto) == 0)
			bpf_error("unknown port '%s'", name);
		if (proto == Q_UDP) {
			if (real_proto == IPPROTO_TCP)
				bpf_error("port '%s' is tcp", name);
			else
				/* override PROTO_UNDEF */
				real_proto = IPPROTO_UDP;
		}
		if (proto == Q_TCP) {
			if (real_proto == IPPROTO_UDP)
				bpf_error("port '%s' is udp", name);
			else
				/* override PROTO_UNDEF */
				real_proto = IPPROTO_TCP;
		}
		return gen_port(port, real_proto, dir);

	case Q_GATEWAY:
		eaddr = pcap_ether_hostton(name);
		if (eaddr == NULL)
			bpf_error("unknown ether host: %s", name);

		alist = pcap_nametoaddr(name);
		if (alist == NULL || *alist == NULL)
			bpf_error("unknown host '%s'", name);
		return gen_gateway(eaddr, alist, proto, dir);

	case Q_PROTO:
		real_proto = lookup_proto(name, proto);
		if (real_proto >= 0)
			return gen_proto(real_proto, proto, dir);
		else
			bpf_error("unknown protocol: %s", name);

	case Q_UNDEF:
		syntax();
		/* NOTREACHED */
	}
	abort();
	/* NOTREACHED */
}

struct block *
gen_mcode(s1, s2, masklen, q)
	register const char *s1, *s2;
	register int masklen;
	struct qual q;
{
	register int nlen, mlen;
	bpf_u_int32 n, m;

	nlen = __pcap_atoin(s1, &n);
	/* Promote short ipaddr */
	n <<= 32 - nlen;

	if (s2 != NULL) {
		mlen = __pcap_atoin(s2, &m);
		/* Promote short ipaddr */
		m <<= 32 - mlen;
		if ((n & ~m) != 0)
			bpf_error("non-network bits set in \"%s mask %s\"",
			    s1, s2);
	} else {
		/* Convert mask len to mask */
		if (masklen > 32)
			bpf_error("mask length must be <= 32");
		m = 0xffffffff << (32 - masklen);
		if ((n & ~m) != 0)
			bpf_error("non-network bits set in \"%s/%d\"",
			    s1, masklen);
	}

	switch (q.addr) {

	case Q_NET:
		return gen_host(n, m, q.proto, q.dir);

	default:
		bpf_error("Mask syntax for networks only");
		/* NOTREACHED */
	}
}

struct block *
gen_ncode(s, v, q)
	register const char *s;
	bpf_u_int32 v;
	struct qual q;
{
	bpf_u_int32 mask;
	int proto = q.proto;
	int dir = q.dir;
	register int vlen;

	if (s == NULL)
		vlen = 32;
	else if (q.proto == Q_DECNET)
		vlen = __pcap_atodn(s, &v);
	else
		vlen = __pcap_atoin(s, &v);

	switch (q.addr) {

	case Q_DEFAULT:
	case Q_HOST:
	case Q_NET:
		if (proto == Q_DECNET)
			return gen_host(v, 0, proto, dir);
		else if (proto == Q_LINK) {
			bpf_error("illegal link layer address");
		} else {
			mask = 0xffffffff;
			if (s == NULL && q.addr == Q_NET) {
				/* Promote short net number */
				while (v && (v & 0xff000000) == 0) {
					v <<= 8;
					mask <<= 8;
				}
			} else {
				/* Promote short ipaddr */
				v <<= 32 - vlen;
				mask <<= 32 - vlen;
			}
			return gen_host(v, mask, proto, dir);
		}

	case Q_PORT:
		if (proto == Q_UDP)
			proto = IPPROTO_UDP;
		else if (proto == Q_TCP)
			proto = IPPROTO_TCP;
		else if (proto == Q_DEFAULT)
			proto = PROTO_UNDEF;
		else
			bpf_error("illegal qualifier of 'port'");

		return gen_port((int)v, proto, dir);

	case Q_GATEWAY:
		bpf_error("'gateway' requires a name");
		/* NOTREACHED */

	case Q_PROTO:
		return gen_proto((int)v, proto, dir);

	case Q_UNDEF:
		syntax();
		/* NOTREACHED */

	default:
		abort();
		/* NOTREACHED */
	}
	/* NOTREACHED */
}

struct block *
gen_ecode(eaddr, q)
	register const u_char *eaddr;
	struct qual q;
{
	if ((q.addr == Q_HOST || q.addr == Q_DEFAULT) && q.proto == Q_LINK) {
		if (linktype == DLT_EN10MB)
			return gen_ehostop(eaddr, (int)q.dir);
		if (linktype == DLT_FDDI)
			return gen_fhostop(eaddr, (int)q.dir);
	}
	bpf_error("ethernet address used in non-ether expression");
	/* NOTREACHED */
}

void
sappend(s0, s1)
	struct slist *s0, *s1;
{
	/*
	 * This is definitely not the best way to do this, but the
	 * lists will rarely get long.
	 */
	while (s0->next)
		s0 = s0->next;
	s0->next = s1;
}

static struct slist *
xfer_to_x(a)
	struct arth *a;
{
	struct slist *s;

	s = new_stmt(BPF_LDX|BPF_MEM);
	s->s.k = a->regno;
	return s;
}

static struct slist *
xfer_to_a(a)
	struct arth *a;
{
	struct slist *s;

	s = new_stmt(BPF_LD|BPF_MEM);
	s->s.k = a->regno;
	return s;
}

struct arth *
gen_load(proto, index, size)
	int proto;
	struct arth *index;
	int size;
{
	struct slist *s, *tmp;
	struct block *b;
	int regno = alloc_reg();

	free_reg(index->regno);
	switch (size) {

	default:
		bpf_error("data size must be 1, 2, or 4");

	case 1:
		size = BPF_B;
		break;

	case 2:
		size = BPF_H;
		break;

	case 4:
		size = BPF_W;
		break;
	}
	switch (proto) {
	default:
		bpf_error("unsupported index operation");

	case Q_LINK:
		s = xfer_to_x(index);
		tmp = new_stmt(BPF_LD|BPF_IND|size);
		sappend(s, tmp);
		sappend(index->s, s);
		break;

	case Q_IP:
	case Q_ARP:
	case Q_RARP:
	case Q_ATALK:
	case Q_DECNET:
	case Q_SCA:
	case Q_LAT:
	case Q_MOPRC:
	case Q_MOPDL:
		/* XXX Note that we assume a fixed link link header here. */
		s = xfer_to_x(index);
		tmp = new_stmt(BPF_LD|BPF_IND|size);
		tmp->s.k = off_nl;
		sappend(s, tmp);
		sappend(index->s, s);

		b = gen_proto_abbrev(proto);
		if (index->b)
			gen_and(index->b, b);
		index->b = b;
		break;

	case Q_TCP:
	case Q_UDP:
	case Q_ICMP:
	case Q_IGMP:
	case Q_IGRP:
		s = new_stmt(BPF_LDX|BPF_MSH|BPF_B);
		s->s.k = off_nl;
		sappend(s, xfer_to_a(index));
		sappend(s, new_stmt(BPF_ALU|BPF_ADD|BPF_X));
		sappend(s, new_stmt(BPF_MISC|BPF_TAX));
		sappend(s, tmp = new_stmt(BPF_LD|BPF_IND|size));
		tmp->s.k = off_nl;
		sappend(index->s, s);

		gen_and(gen_proto_abbrev(proto), b = gen_ipfrag());
		if (index->b)
			gen_and(index->b, b);
		index->b = b;
		break;
	}
	index->regno = regno;
	s = new_stmt(BPF_ST);
	s->s.k = regno;
	sappend(index->s, s);

	return index;
}

struct block *
gen_relation(code, a0, a1, reversed)
	int code;
	struct arth *a0, *a1;
	int reversed;
{
	struct slist *s0, *s1, *s2;
	struct block *b, *tmp;

	s0 = xfer_to_x(a1);
	s1 = xfer_to_a(a0);
	s2 = new_stmt(BPF_ALU|BPF_SUB|BPF_X);
	b = new_block(JMP(code));
	if (code == BPF_JGT || code == BPF_JGE) {
		reversed = !reversed;
		b->s.k = 0x80000000;
	}
	if (reversed)
		gen_not(b);

	sappend(s1, s2);
	sappend(s0, s1);
	sappend(a1->s, s0);
	sappend(a0->s, a1->s);

	b->stmts = a0->s;

	free_reg(a0->regno);
	free_reg(a1->regno);

	/* 'and' together protocol checks */
	if (a0->b) {
		if (a1->b) {
			gen_and(a0->b, tmp = a1->b);
		}
		else
			tmp = a0->b;
	} else
		tmp = a1->b;

	if (tmp)
		gen_and(tmp, b);

	return b;
}

struct arth *
gen_loadlen()
{
	int regno = alloc_reg();
	struct arth *a = (struct arth *)newchunk(sizeof(*a));
	struct slist *s;

	s = new_stmt(BPF_LD|BPF_LEN);
	s->next = new_stmt(BPF_ST);
	s->next->s.k = regno;
	a->s = s;
	a->regno = regno;

	return a;
}

struct arth *
gen_loadi(val)
	int val;
{
	struct arth *a;
	struct slist *s;
	int reg;

	a = (struct arth *)newchunk(sizeof(*a));

	reg = alloc_reg();

	s = new_stmt(BPF_LD|BPF_IMM);
	s->s.k = val;
	s->next = new_stmt(BPF_ST);
	s->next->s.k = reg;
	a->s = s;
	a->regno = reg;

	return a;
}

struct arth *
gen_neg(a)
	struct arth *a;
{
	struct slist *s;

	s = xfer_to_a(a);
	sappend(a->s, s);
	s = new_stmt(BPF_ALU|BPF_NEG);
	s->s.k = 0;
	sappend(a->s, s);
	s = new_stmt(BPF_ST);
	s->s.k = a->regno;
	sappend(a->s, s);

	return a;
}

struct arth *
gen_arth(code, a0, a1)
	int code;
	struct arth *a0, *a1;
{
	struct slist *s0, *s1, *s2;

	s0 = xfer_to_x(a1);
	s1 = xfer_to_a(a0);
	s2 = new_stmt(BPF_ALU|BPF_X|code);

	sappend(s1, s2);
	sappend(s0, s1);
	sappend(a1->s, s0);
	sappend(a0->s, a1->s);

	free_reg(a1->regno);

	s0 = new_stmt(BPF_ST);
	a0->regno = s0->s.k = alloc_reg();
	sappend(a0->s, s0);

	return a0;
}

/*
 * Here we handle simple allocation of the scratch registers.
 * If too many registers are alloc'd, the allocator punts.
 */
static int regused[BPF_MEMWORDS];
static int curreg;

/*
 * Return the next free register.
 */
static int
alloc_reg()
{
	int n = BPF_MEMWORDS;

	while (--n >= 0) {
		if (regused[curreg])
			curreg = (curreg + 1) % BPF_MEMWORDS;
		else {
			regused[curreg] = 1;
			return curreg;
		}
	}
	bpf_error("too many registers needed to evaluate expression");
	/* NOTREACHED */
}

/*
 * Return a register to the table so it can
 * be used later.
 */
static void
free_reg(n)
	int n;
{
	regused[n] = 0;
}

static struct block *
gen_len(jmp, n)
	int jmp, n;
{
	struct slist *s;
	struct block *b;

	s = new_stmt(BPF_LD|BPF_LEN);
	b = new_block(JMP(jmp));
	b->stmts = s;
	b->s.k = n;

	return b;
}

struct block *
gen_greater(n)
	int n;
{
	return gen_len(BPF_JGE, n);
}

struct block *
gen_less(n)
	int n;
{
	struct block *b;

	b = gen_len(BPF_JGT, n);
	gen_not(b);

	return b;
}

struct block *
gen_byteop(op, idx, val)
	int op, idx, val;
{
	struct block *b;
	struct slist *s;

	switch (op) {
	default:
		abort();

	case '=':
		return gen_cmp((u_int)idx, BPF_B, (bpf_int32)val);

	case '<':
		b = gen_cmp((u_int)idx, BPF_B, (bpf_int32)val);
		b->s.code = JMP(BPF_JGE);
		gen_not(b);
		return b;

	case '>':
		b = gen_cmp((u_int)idx, BPF_B, (bpf_int32)val);
		b->s.code = JMP(BPF_JGT);
		return b;

	case '|':
		s = new_stmt(BPF_ALU|BPF_OR|BPF_K);
		break;

	case '&':
		s = new_stmt(BPF_ALU|BPF_AND|BPF_K);
		break;
	}
	s->s.k = val;
	b = new_block(JMP(BPF_JEQ));
	b->stmts = s;
	gen_not(b);

	return b;
}

struct block *
gen_broadcast(proto)
	int proto;
{
	bpf_u_int32 hostmask;
	struct block *b0, *b1, *b2;
	static u_char ebroadcast[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	switch (proto) {

	case Q_DEFAULT:
	case Q_LINK:
		if (linktype == DLT_EN10MB)
			return gen_ehostop(ebroadcast, Q_DST);
		if (linktype == DLT_FDDI)
			return gen_fhostop(ebroadcast, Q_DST);
		bpf_error("not a broadcast link");
		break;

	case Q_IP:
		b0 = gen_linktype(ETHERTYPE_IP);
		hostmask = ~netmask;
		b1 = gen_mcmp(off_nl + 16, BPF_W, (bpf_int32)0, hostmask);
		b2 = gen_mcmp(off_nl + 16, BPF_W,
			      (bpf_int32)(~0 & hostmask), hostmask);
		gen_or(b1, b2);
		gen_and(b0, b2);
		return b2;
	}
	bpf_error("only ether/ip broadcast filters supported");
}

struct block *
gen_multicast(proto)
	int proto;
{
	register struct block *b0, *b1;
	register struct slist *s;

	switch (proto) {

	case Q_DEFAULT:
	case Q_LINK:
		if (linktype == DLT_EN10MB) {
			/* ether[0] & 1 != 0 */
			s = new_stmt(BPF_LD|BPF_B|BPF_ABS);
			s->s.k = 0;
			b0 = new_block(JMP(BPF_JSET));
			b0->s.k = 1;
			b0->stmts = s;
			return b0;
		}

		if (linktype == DLT_FDDI) {
			/* XXX TEST THIS: MIGHT NOT PORT PROPERLY XXX */
			/* fddi[1] & 1 != 0 */
			s = new_stmt(BPF_LD|BPF_B|BPF_ABS);
			s->s.k = 1;
			b0 = new_block(JMP(BPF_JSET));
			b0->s.k = 1;
			b0->stmts = s;
			return b0;
		}
		/* Link not known to support multicasts */
		break;

	case Q_IP:
		b0 = gen_linktype(ETHERTYPE_IP);
		b1 = gen_cmp(off_nl + 16, BPF_B, (bpf_int32)224);
		b1->s.code = JMP(BPF_JGE);
		gen_and(b0, b1);
		return b1;
	}
	bpf_error("only IP multicast filters supported on ethernet/FDDI");
}

/*
 * generate command for inbound/outbound.  It's here so we can
 * make it link-type specific.  'dir' = 0 implies "inbound",
 * = 1 implies "outbound".
 */
struct block *
gen_inbound(dir)
	int dir;
{
	register struct block *b0;

	b0 = gen_relation(BPF_JEQ,
			  gen_load(Q_LINK, gen_loadi(0), 1),
			  gen_loadi(0),
			  dir);
	return (b0);
}
