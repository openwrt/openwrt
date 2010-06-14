/*
 * Misc useful os-independent macros and functions.
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 */

#ifndef	_bcmutils_h_
#define	_bcmutils_h_
#include "linux_osl.h"

/* ctype replacement */
#define _BCM_U	0x01	/* upper */
#define _BCM_L	0x02	/* lower */
#define _BCM_D	0x04	/* digit */
#define _BCM_C	0x08	/* cntrl */
#define _BCM_P	0x10	/* punct */
#define _BCM_S	0x20	/* white space (space/lf/tab) */
#define _BCM_X	0x40	/* hex digit */
#define _BCM_SP	0x80	/* hard space (0x20) */

extern const unsigned char bcm_ctype[];
#define bcm_ismask(x)	(bcm_ctype[(int)(unsigned char)(x)])

#define bcm_isalnum(c)	((bcm_ismask(c)&(_BCM_U|_BCM_L|_BCM_D)) != 0)
#define bcm_isalpha(c)	((bcm_ismask(c)&(_BCM_U|_BCM_L)) != 0)
#define bcm_iscntrl(c)	((bcm_ismask(c)&(_BCM_C)) != 0)
#define bcm_isdigit(c)	((bcm_ismask(c)&(_BCM_D)) != 0)
#define bcm_isgraph(c)	((bcm_ismask(c)&(_BCM_P|_BCM_U|_BCM_L|_BCM_D)) != 0)
#define bcm_islower(c)	((bcm_ismask(c)&(_BCM_L)) != 0)
#define bcm_isprint(c)	((bcm_ismask(c)&(_BCM_P|_BCM_U|_BCM_L|_BCM_D|_BCM_SP)) != 0)
#define bcm_ispunct(c)	((bcm_ismask(c)&(_BCM_P)) != 0)
#define bcm_isspace(c)	((bcm_ismask(c)&(_BCM_S)) != 0)
#define bcm_isupper(c)	((bcm_ismask(c)&(_BCM_U)) != 0)
#define bcm_isxdigit(c)	((bcm_ismask(c)&(_BCM_D|_BCM_X)) != 0)
#define bcm_tolower(c)	(bcm_isupper((c)) ? ((c) + 'a' - 'A') : (c))
#define bcm_toupper(c)	(bcm_islower((c)) ? ((c) + 'A' - 'a') : (c))

/* Buffer structure for collecting string-formatted data 
* using bcm_bprintf() API.
* Use bcm_binit() to initialize before use
*/

struct bcmstrbuf {
	char *buf;	/* pointer to current position in origbuf */
	unsigned int size;	/* current (residual) size in bytes */
	char *origbuf;	/* unmodified pointer to orignal buffer */
	unsigned int origsize;	/* unmodified orignal buffer size in bytes */
};

/* ** driver-only section ** */
#include <osl.h>

#define GPIO_PIN_NOTDEFINED 	0x20	/* Pin not defined */


/* osl multi-precedence packet queue */
#ifndef PKTQ_LEN_DEFAULT
#define PKTQ_LEN_DEFAULT        128	/* Max 128 packets */
#endif
#ifndef PKTQ_MAX_PREC
#define PKTQ_MAX_PREC           16	/* Maximum precedence levels */
#endif

typedef struct pktq_prec {
	void *head;     /* first packet to dequeue */
	void *tail;     /* last packet to dequeue */
	uint16 len;     /* number of queued packets */
	uint16 max;     /* maximum number of queued packets */
} pktq_prec_t;


/* multi-priority pkt queue */
struct pktq {
	uint16 num_prec;        /* number of precedences in use */
	uint16 hi_prec;         /* rapid dequeue hint (>= highest non-empty prec) */
	uint16 max;             /* total max packets */
	uint16 len;             /* total number of packets */
	/* q array must be last since # of elements can be either PKTQ_MAX_PREC or 1 */
	struct pktq_prec q[PKTQ_MAX_PREC];
};

/* simple, non-priority pkt queue */
struct spktq {
	uint16 num_prec;        /* number of precedences in use (always 1) */
	uint16 hi_prec;         /* rapid dequeue hint (>= highest non-empty prec) */
	uint16 max;             /* total max packets */
	uint16 len;             /* total number of packets */
	/* q array must be last since # of elements can be either PKTQ_MAX_PREC or 1 */
	struct pktq_prec q[1];
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
#define pktenq(pq, p)		pktq_penq(((struct pktq *)pq), 0, (p))
#define pktenq_head(pq, p)	pktq_penq_head(((struct pktq *)pq), 0, (p))
#define pktdeq(pq)		pktq_pdeq(((struct pktq *)pq), 0)
#define pktdeq_tail(pq)		pktq_pdeq_tail(((struct pktq *)pq), 0)
#define pktqinit(pq, len) pktq_init(((struct pktq *)pq), 1, len)

extern void pktq_init(struct pktq *pq, int num_prec, int max_len);
/* prec_out may be NULL if caller is not interested in return value */
extern void *pktq_deq(struct pktq *pq, int *prec_out);
extern void *pktq_deq_tail(struct pktq *pq, int *prec_out);
extern void *pktq_peek(struct pktq *pq, int *prec_out);
extern void *pktq_peek_tail(struct pktq *pq, int *prec_out);
extern void pktq_flush(osl_t *osh, struct pktq *pq, bool dir); /* Empty the entire queue */
extern int pktq_setmax(struct pktq *pq, int max_len);

/* externs */
/* packet */
extern uint pktcopy(osl_t *osh, void *p, uint offset, int len, uchar *buf);
extern uint pkttotlen(osl_t *osh, void *p);
extern void *pktlast(osl_t *osh, void *p);

/* Get priority from a packet and pass it back in scb (or equiv) */
extern uint pktsetprio(void *pkt, bool update_vtag);
#define	PKTPRIO_VDSCP	0x100		/* DSCP prio found after VLAN tag */
#define	PKTPRIO_VLAN	0x200		/* VLAN prio found */
#define	PKTPRIO_UPD	0x400		/* DSCP used to update VLAN prio */
#define	PKTPRIO_DSCP	0x800		/* DSCP prio found */

/* string */
extern int BCMROMFN(bcm_atoi)(char *s);
extern ulong BCMROMFN(bcm_strtoul)(char *cp, char **endp, uint base);
extern char *BCMROMFN(bcmstrstr)(char *haystack, char *needle);
extern char *BCMROMFN(bcmstrcat)(char *dest, const char *src);
extern char *BCMROMFN(bcmstrncat)(char *dest, const char *src, uint size);
extern ulong wchar2ascii(char *abuf, ushort *wbuf, ushort wbuflen, ulong abuflen);
/* ethernet address */
extern char *bcm_ether_ntoa(struct ether_addr *ea, char *buf);
extern int BCMROMFN(bcm_ether_atoe)(char *p, struct ether_addr *ea);

/* ip address */
struct ipv4_addr;
extern char *bcm_ip_ntoa(struct ipv4_addr *ia, char *buf);

/* delay */
extern void bcm_mdelay(uint ms);
/* variable access */
extern char *getvar(char *vars, const char *name);
extern int getintvar(char *vars, const char *name);
extern uint getgpiopin(char *vars, char *pin_name, uint def_pin);
#ifdef BCMDBG
extern void prpkt(const char *msg, osl_t *osh, void *p0);
#endif /* BCMDBG */
#ifdef BCMPERFSTATS
extern void bcm_perf_enable(void);
extern void bcmstats(char *fmt);
extern void bcmlog(char *fmt, uint a1, uint a2);
extern void bcmdumplog(char *buf, int size);
extern int bcmdumplogent(char *buf, uint idx);
#else
#define bcm_perf_enable()
#define bcmstats(fmt)
#define	bcmlog(fmt, a1, a2)
#define	bcmdumplog(buf, size)	*buf = '\0'
#define	bcmdumplogent(buf, idx)	-1
#endif /* BCMPERFSTATS */
extern char *bcm_nvram_vars(uint *length);
extern int bcm_nvram_cache(void *sbh);

/* Support for sharing code across in-driver iovar implementations.
 * The intent is that a driver use this structure to map iovar names
 * to its (private) iovar identifiers, and the lookup function to
 * find the entry.  Macros are provided to map ids and get/set actions
 * into a single number space for a switch statement.
 */

/* iovar structure */
typedef struct bcm_iovar {
	const char *name;	/* name for lookup and display */
	uint16 varid;		/* id for switch */
	uint16 flags;		/* driver-specific flag bits */
	uint16 type;		/* base type of argument */
	uint16 minlen;		/* min length for buffer vars */
} bcm_iovar_t;

/* varid definitions are per-driver, may use these get/set bits */

/* IOVar action bits for id mapping */
#define IOV_GET 0 /* Get an iovar */
#define IOV_SET 1 /* Set an iovar */

/* Varid to actionid mapping */
#define IOV_GVAL(id)		((id)*2)
#define IOV_SVAL(id)		(((id)*2)+IOV_SET)
#define IOV_ISSET(actionid)	((actionid & IOV_SET) == IOV_SET)

/* flags are per-driver based on driver attributes */

extern const bcm_iovar_t *bcm_iovar_lookup(const bcm_iovar_t *table, const char *name);
extern int bcm_iovar_lencheck(const bcm_iovar_t *table, void *arg, int len, bool set);

/* Base type definitions */
#define IOVT_VOID	0	/* no value (implictly set only) */
#define IOVT_BOOL	1	/* any value ok (zero/nonzero) */
#define IOVT_INT8	2	/* integer values are range-checked */
#define IOVT_UINT8	3	/* unsigned int 8 bits */
#define IOVT_INT16	4	/* int 16 bits */
#define IOVT_UINT16	5	/* unsigned int 16 bits */
#define IOVT_INT32	6	/* int 32 bits */
#define IOVT_UINT32	7	/* unsigned int 32 bits */
#define IOVT_BUFFER	8	/* buffer is size-checked as per minlen */
#define BCM_IOVT_VALID(type) (((unsigned int)(type)) <= IOVT_BUFFER)

/* Initializer for IOV type strings */
#define BCM_IOV_TYPE_INIT { \
	"void", \
	"bool", \
	"int8", \
	"uint8", \
	"int16", \
	"uint16", \
	"int32", \
	"uint32", \
	"buffer", \
	"" }

#define BCM_IOVT_IS_INT(type) (\
	(type == IOVT_BOOL) || \
	(type == IOVT_INT8) || \
	(type == IOVT_UINT8) || \
	(type == IOVT_INT16) || \
	(type == IOVT_UINT16) || \
	(type == IOVT_INT32) || \
	(type == IOVT_UINT32))

/* ** driver/apps-shared section ** */

#define BCME_STRLEN 		64	/* Max string length for BCM errors */
#define VALID_BCMERROR(e)  ((e <= 0) && (e >= BCME_LAST))


/*
 * error codes could be added but the defined ones shouldn't be changed/deleted
 * these error codes are exposed to the user code
 * when ever a new error code is added to this list
 * please update errorstring table with the related error string and
 * update osl files with os specific errorcode map
*/

#define BCME_OK				0	/* Success */
#define BCME_ERROR			-1	/* Error generic */
#define BCME_BADARG			-2	/* Bad Argument */
#define BCME_BADOPTION			-3	/* Bad option */
#define BCME_NOTUP			-4	/* Not up */
#define BCME_NOTDOWN			-5	/* Not down */
#define BCME_NOTAP			-6	/* Not AP */
#define BCME_NOTSTA			-7	/* Not STA  */
#define BCME_BADKEYIDX			-8	/* BAD Key Index */
#define BCME_RADIOOFF 			-9	/* Radio Off */
#define BCME_NOTBANDLOCKED		-10	/* Not  band locked */
#define BCME_NOCLK			-11	/* No Clock */
#define BCME_BADRATESET			-12	/* BAD Rate valueset */
#define BCME_BADBAND			-13	/* BAD Band */
#define BCME_BUFTOOSHORT		-14	/* Buffer too short */
#define BCME_BUFTOOLONG			-15	/* Buffer too long */
#define BCME_BUSY			-16	/* Busy */
#define BCME_NOTASSOCIATED		-17	/* Not Associated */
#define BCME_BADSSIDLEN			-18	/* Bad SSID len */
#define BCME_OUTOFRANGECHAN		-19	/* Out of Range Channel */
#define BCME_BADCHAN			-20	/* Bad Channel */
#define BCME_BADADDR			-21	/* Bad Address */
#define BCME_NORESOURCE			-22	/* Not Enough Resources */
#define BCME_UNSUPPORTED		-23	/* Unsupported */
#define BCME_BADLEN			-24	/* Bad length */
#define BCME_NOTREADY			-25	/* Not Ready */
#define BCME_EPERM			-26	/* Not Permitted */
#define BCME_NOMEM			-27	/* No Memory */
#define BCME_ASSOCIATED			-28	/* Associated */
#define BCME_RANGE			-29	/* Not In Range */
#define BCME_NOTFOUND			-30	/* Not Found */
#define BCME_WME_NOT_ENABLED		-31	/* WME Not Enabled */
#define BCME_TSPEC_NOTFOUND		-32	/* TSPEC Not Found */
#define BCME_ACM_NOTSUPPORTED		-33	/* ACM Not Supported */
#define BCME_NOT_WME_ASSOCIATION	-34	/* Not WME Association */
#define BCME_SDIO_ERROR			-35	/* SDIO Bus Error */
#define BCME_DONGLE_DOWN		-36	/* Dongle Not Accessible */
#define BCME_VERSION			-37 /* Incorrect version */
#define BCME_LAST			BCME_VERSION

/* These are collection of BCME Error strings */
#define BCMERRSTRINGTABLE {		\
	"OK",				\
	"Undefined error",		\
	"Bad Argument",			\
	"Bad Option",			\
	"Not up",			\
	"Not down",			\
	"Not AP",			\
	"Not STA",			\
	"Bad Key Index",		\
	"Radio Off",			\
	"Not band locked",		\
	"No clock",			\
	"Bad Rate valueset",		\
	"Bad Band",			\
	"Buffer too short",		\
	"Buffer too long",		\
	"Busy",				\
	"Not Associated",		\
	"Bad SSID len",			\
	"Out of Range Channel",		\
	"Bad Channel",			\
	"Bad Address",			\
	"Not Enough Resources",		\
	"Unsupported",			\
	"Bad length",			\
	"Not Ready",			\
	"Not Permitted",		\
	"No Memory",			\
	"Associated",			\
	"Not In Range",			\
	"Not Found",			\
	"WME Not Enabled",		\
	"TSPEC Not Found",		\
	"ACM Not Supported",		\
	"Not WME Association",		\
	"SDIO Bus Error",		\
	"Dongle Not Accessible",	\
	"Incorrect version"	\
}

#ifndef ABS
#define	ABS(a)			(((a) < 0)?-(a):(a))
#endif /* ABS */

#ifndef MIN
#define	MIN(a, b)		(((a) < (b))?(a):(b))
#endif /* MIN */

#ifndef MAX
#define	MAX(a, b)		(((a) > (b))?(a):(b))
#endif /* MAX */

#define CEIL(x, y)		(((x) + ((y)-1)) / (y))
#define	ROUNDUP(x, y)		((((x)+((y)-1))/(y))*(y))
#define	ISALIGNED(a, x)		(((a) & ((x)-1)) == 0)
#define	ISPOWEROF2(x)		((((x)-1)&(x)) == 0)
#define VALID_MASK(mask)	!((mask) & ((mask) + 1))
#ifndef OFFSETOF
#define	OFFSETOF(type, member)	((uint)(uintptr)&((type *)0)->member)
#endif /* OFFSETOF */
#ifndef ARRAYSIZE
#define ARRAYSIZE(a)		(sizeof(a)/sizeof(a[0]))
#endif

/* bit map related macros */
#ifndef setbit
#ifndef NBBY		    /* the BSD family defines NBBY */
#define	NBBY	8	/* 8 bits per byte */
#endif /* #ifndef NBBY */
#define	setbit(a, i)	(((uint8 *)a)[(i)/NBBY] |= 1<<((i)%NBBY))
#define	clrbit(a, i)	(((uint8 *)a)[(i)/NBBY] &= ~(1<<((i)%NBBY)))
#define	isset(a, i)	(((const uint8 *)a)[(i)/NBBY] & (1<<((i)%NBBY)))
#define	isclr(a, i)	((((const uint8 *)a)[(i)/NBBY] & (1<<((i)%NBBY))) == 0)
#endif /* setbit */

#define	NBITS(type)	(sizeof(type) * 8)
#define NBITVAL(nbits)	(1 << (nbits))
#define MAXBITVAL(nbits)	((1 << (nbits)) - 1)
#define	NBITMASK(nbits)	MAXBITVAL(nbits)
#define MAXNBVAL(nbyte)	MAXBITVAL((nbyte) * 8)

/* basic mux operation - can be optimized on several architectures */
#define MUX(pred, true, false) ((pred) ? (true) : (false))

/* modulo inc/dec - assumes x E [0, bound - 1] */
#define MODDEC(x, bound) MUX((x) == 0, (bound) - 1, (x) - 1)
#define MODINC(x, bound) MUX((x) == (bound) - 1, 0, (x) + 1)

/* modulo inc/dec, bound = 2^k */
#define MODDEC_POW2(x, bound) (((x) - 1) & ((bound) - 1))
#define MODINC_POW2(x, bound) (((x) + 1) & ((bound) - 1))

/* modulo add/sub - assumes x, y E [0, bound - 1] */
#define MODADD(x, y, bound) \
    MUX((x) + (y) >= (bound), (x) + (y) - (bound), (x) + (y))
#define MODSUB(x, y, bound) \
    MUX(((int)(x)) - ((int)(y)) < 0, (x) - (y) + (bound), (x) - (y))

/* module add/sub, bound = 2^k */
#define MODADD_POW2(x, y, bound) (((x) + (y)) & ((bound) - 1))
#define MODSUB_POW2(x, y, bound) (((x) - (y)) & ((bound) - 1))

/* crc defines */
#define CRC8_INIT_VALUE  0xff		/* Initial CRC8 checksum value */
#define CRC8_GOOD_VALUE  0x9f		/* Good final CRC8 checksum value */
#define CRC16_INIT_VALUE 0xffff		/* Initial CRC16 checksum value */
#define CRC16_GOOD_VALUE 0xf0b8		/* Good final CRC16 checksum value */
#define CRC32_INIT_VALUE 0xffffffff	/* Initial CRC32 checksum value */
#define CRC32_GOOD_VALUE 0xdebb20e3	/* Good final CRC32 checksum value */

/* bcm_format_flags() bit description structure */
typedef struct bcm_bit_desc {
	uint32	bit;
	const char* name;
} bcm_bit_desc_t;

/* tag_ID/length/value_buffer tuple */
typedef struct bcm_tlv {
	uint8	id;
	uint8	len;
	uint8	data[1];
} bcm_tlv_t;

/* Check that bcm_tlv_t fits into the given buflen */
#define bcm_valid_tlv(elt, buflen) ((buflen) >= 2 && (int)(buflen) >= (int)(2 + (elt)->len))

/* buffer length for ethernet address from bcm_ether_ntoa() */
#define ETHER_ADDR_STR_LEN	18	/* 18-bytes of Ethernet address buffer length */

/* unaligned load and store macros */
#ifdef IL_BIGENDIAN
static INLINE uint32
load32_ua(uint8 *a)
{
	return ((a[0] << 24) | (a[1] << 16) | (a[2] << 8) | a[3]);
}

static INLINE void
store32_ua(uint8 *a, uint32 v)
{
	a[0] = (v >> 24) & 0xff;
	a[1] = (v >> 16) & 0xff;
	a[2] = (v >> 8) & 0xff;
	a[3] = v & 0xff;
}

static INLINE uint16
load16_ua(uint8 *a)
{
	return ((a[0] << 8) | a[1]);
}

static INLINE void
store16_ua(uint8 *a, uint16 v)
{
	a[0] = (v >> 8) & 0xff;
	a[1] = v & 0xff;
}

#else /* IL_BIGENDIAN */

static INLINE uint32
load32_ua(uint8 *a)
{
	return ((a[3] << 24) | (a[2] << 16) | (a[1] << 8) | a[0]);
}

static INLINE void
store32_ua(uint8 *a, uint32 v)
{
	a[3] = (v >> 24) & 0xff;
	a[2] = (v >> 16) & 0xff;
	a[1] = (v >> 8) & 0xff;
	a[0] = v & 0xff;
}

static INLINE uint16
load16_ua(uint8 *a)
{
	return ((a[1] << 8) | a[0]);
}

static INLINE void
store16_ua(uint8 *a, uint16 v)
{
	a[1] = (v >> 8) & 0xff;
	a[0] = v & 0xff;
}

#endif /* IL_BIGENDIAN */

/* externs */
/* crc */
extern uint8 BCMROMFN(hndcrc8)(uint8 *p, uint nbytes, uint8 crc);
extern uint16 BCMROMFN(hndcrc16)(uint8 *p, uint nbytes, uint16 crc);
extern uint32 BCMROMFN(hndcrc32)(uint8 *p, uint nbytes, uint32 crc);
/* format/print */
#ifdef BCMDBG
extern int bcm_format_flags(const bcm_bit_desc_t *bd, uint32 flags, char* buf, int len);
extern int bcm_format_hex(char *str, const void *bytes, int len);
extern void deadbeef(void *p, uint len);
extern void prhex(const char *msg, uchar *buf, uint len);
#endif /* BCMDBG */
extern char *bcm_brev_str(uint16 brev, char *buf);
extern void printfbig(char *buf);

/* IE parsing */
extern bcm_tlv_t *BCMROMFN(bcm_next_tlv)(bcm_tlv_t *elt, int *buflen);
extern bcm_tlv_t *BCMROMFN(bcm_parse_tlvs)(void *buf, int buflen, uint key);
extern bcm_tlv_t *BCMROMFN(bcm_parse_ordered_tlvs)(void *buf, int buflen, uint key);

/* bcmerror */
extern const char *bcmerrorstr(int bcmerror);

/* multi-bool data type: set of bools, mbool is true if any is set */
typedef uint32 mbool;
#define mboolset(mb, bit)		((mb) |= (bit))		/* set one bool */
#define mboolclr(mb, bit)		((mb) &= ~(bit))	/* clear one bool */
#define mboolisset(mb, bit)		(((mb) & (bit)) != 0)	/* TRUE if one bool is set */
#define	mboolmaskset(mb, mask, val)	((mb) = (((mb) & ~(mask)) | (val)))

/* power conversion */
extern uint16 BCMROMFN(bcm_qdbm_to_mw)(uint8 qdbm);
extern uint8 BCMROMFN(bcm_mw_to_qdbm)(uint16 mw);

/* generic datastruct to help dump routines */
struct fielddesc {
	const char *nameandfmt;
	uint32 	offset;
	uint32 	len;
};

extern void bcm_binit(struct bcmstrbuf *b, char *buf, uint size);
extern int bcm_bprintf(struct bcmstrbuf *b, const char *fmt, ...);

typedef  uint32 (*readreg_rtn)(void *arg0, void *arg1, uint32 offset);
extern uint bcmdumpfields(readreg_rtn func_ptr, void *arg0, void *arg1, struct fielddesc *str,
                          char *buf, uint32 bufsize);

extern uint bcm_mkiovar(char *name, char *data, uint datalen, char *buf, uint len);
extern uint BCMROMFN(bcm_bitcount)(uint8 *bitmap, uint bytelength);

#ifdef BCMDBG_PKT      /* pkt logging for debugging */
#define PKTLIST_SIZE 1000
typedef struct {
	void *list[PKTLIST_SIZE]; /* List of pointers to packets */
	uint count; /* Total count of the packets */
} pktlist_info_t;

extern void pktlist_add(pktlist_info_t *pktlist, void *p);
extern void pktlist_remove(pktlist_info_t *pktlist, void *p);
extern char* pktlist_dump(pktlist_info_t *pktlist, char *buf);
#endif  /* BCMDBG_PKT */

#endif	/* _bcmutils_h_ */
