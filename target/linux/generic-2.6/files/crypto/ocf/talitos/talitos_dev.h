/*
 * Freescale SEC (talitos) device dependent data structures
 *
 * Copyright (c) 2006 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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
 */

/* device ID register values */
#define TALITOS_ID_SEC_2_0	0x40
#define TALITOS_ID_SEC_2_1	0x40 /* cross ref with IP block revision reg */

/*
 * following num_channels, channel-fifo-depth, exec-unit-mask, and 
 * descriptor-types-mask are for forward-compatibility with openfirmware
 * flat device trees
 */

/*
 *  num_channels : the number of channels available in each SEC version.
 */

/* n.b. this driver requires these values be a power of 2 */
#define TALITOS_NCHANNELS_SEC_1_0	4
#define TALITOS_NCHANNELS_SEC_1_2	1
#define TALITOS_NCHANNELS_SEC_2_0	4
#define TALITOS_NCHANNELS_SEC_2_01	4
#define TALITOS_NCHANNELS_SEC_2_1	4
#define TALITOS_NCHANNELS_SEC_2_4	4

/*
 *  channel-fifo-depth : The number of descriptor
 *  pointers a channel fetch fifo can hold.
 */
#define TALITOS_CHFIFOLEN_SEC_1_0	1
#define TALITOS_CHFIFOLEN_SEC_1_2	1
#define TALITOS_CHFIFOLEN_SEC_2_0	24
#define TALITOS_CHFIFOLEN_SEC_2_01	24
#define TALITOS_CHFIFOLEN_SEC_2_1	24
#define TALITOS_CHFIFOLEN_SEC_2_4	24

/* 
 *  exec-unit-mask : The bitmask representing what Execution Units (EUs)
 *  are available. EU information should be encoded following the SEC's 
 *  EU_SEL0 bitfield documentation, i.e. as follows:
 * 
 *    bit 31 = set if SEC permits no-EU selection (should be always set)
 *    bit 30 = set if SEC has the ARC4 EU (AFEU)
 *    bit 29 = set if SEC has the des/3des EU (DEU)
 *    bit 28 = set if SEC has the message digest EU (MDEU)
 *    bit 27 = set if SEC has the random number generator EU (RNG)
 *    bit 26 = set if SEC has the public key EU (PKEU)
 *    bit 25 = set if SEC has the aes EU (AESU)
 *    bit 24 = set if SEC has the Kasumi EU (KEU)
 * 
 */
#define TALITOS_HAS_EU_NONE		(1<<0)
#define TALITOS_HAS_EU_AFEU		(1<<1)
#define TALITOS_HAS_EU_DEU		(1<<2)
#define TALITOS_HAS_EU_MDEU		(1<<3)
#define TALITOS_HAS_EU_RNG		(1<<4)
#define TALITOS_HAS_EU_PKEU		(1<<5)
#define TALITOS_HAS_EU_AESU		(1<<6)
#define TALITOS_HAS_EU_KEU		(1<<7)

/* the corresponding masks for each SEC version */
#define TALITOS_HAS_EUS_SEC_1_0		0x7f
#define TALITOS_HAS_EUS_SEC_1_2		0x4d
#define TALITOS_HAS_EUS_SEC_2_0		0x7f
#define TALITOS_HAS_EUS_SEC_2_01	0x7f
#define TALITOS_HAS_EUS_SEC_2_1		0xff
#define TALITOS_HAS_EUS_SEC_2_4		0x7f

/*
 *  descriptor-types-mask : The bitmask representing what descriptors
 *  are available. Descriptor type information should be encoded 
 *  following the SEC's Descriptor Header Dword DESC_TYPE field 
 *  documentation, i.e. as follows:
 *
 *    bit 0  = set if SEC supports the aesu_ctr_nonsnoop desc. type
 *    bit 1  = set if SEC supports the ipsec_esp descriptor type
 *    bit 2  = set if SEC supports the common_nonsnoop desc. type
 *    bit 3  = set if SEC supports the 802.11i AES ccmp desc. type
 *    bit 4  = set if SEC supports the hmac_snoop_no_afeu desc. type
 *    bit 5  = set if SEC supports the srtp descriptor type
 *    bit 6  = set if SEC supports the non_hmac_snoop_no_afeu desc.type
 *    bit 7  = set if SEC supports the pkeu_assemble descriptor type
 *    bit 8  = set if SEC supports the aesu_key_expand_output desc.type
 *    bit 9  = set if SEC supports the pkeu_ptmul descriptor type
 *    bit 10 = set if SEC supports the common_nonsnoop_afeu desc. type
 *    bit 11 = set if SEC supports the pkeu_ptadd_dbl descriptor type
 *
 *  ..and so on and so forth.
 */
#define TALITOS_HAS_DT_AESU_CTR_NONSNOOP	(1<<0)
#define TALITOS_HAS_DT_IPSEC_ESP		(1<<1)
#define TALITOS_HAS_DT_COMMON_NONSNOOP		(1<<2)

/* the corresponding masks for each SEC version */
#define TALITOS_HAS_DESCTYPES_SEC_2_0	0x01010ebf
#define TALITOS_HAS_DESCTYPES_SEC_2_1	0x012b0ebf

/* 
 * a TALITOS_xxx_HI address points to the low data bits (32-63) of the register
 */

/* global register offset addresses */
#define TALITOS_ID		0x1020
#define TALITOS_ID_HI		0x1024
#define TALITOS_MCR		0x1030		/* master control register */
#define TALITOS_MCR_HI		0x1038		/* master control register */
#define TALITOS_MCR_SWR		0x1
#define TALITOS_IMR		0x1008		/* interrupt mask register */
#define TALITOS_IMR_ALL		0x00010fff	/* enable all interrupts mask */
#define TALITOS_IMR_ERRONLY	0x00010aaa	/* enable error interrupts */
#define TALITOS_IMR_HI		0x100C		/* interrupt mask register */
#define TALITOS_IMR_HI_ALL	0x00323333	/* enable all interrupts mask */
#define TALITOS_IMR_HI_ERRONLY	0x00222222	/* enable error interrupts */
#define TALITOS_ISR		0x1010		/* interrupt status register */
#define TALITOS_ISR_ERROR	0x00010faa	/* errors mask */
#define TALITOS_ISR_DONE	0x00000055	/* channel(s) done mask */
#define TALITOS_ISR_HI		0x1014		/* interrupt status register */
#define TALITOS_ICR		0x1018		/* interrupt clear register */
#define TALITOS_ICR_HI		0x101C		/* interrupt clear register */

/* channel register address stride */
#define TALITOS_CH_OFFSET	0x100

/* channel register offset addresses and bits */
#define TALITOS_CH_CCCR		0x1108	/* Crypto-Channel Config Register */
#define TALITOS_CH_CCCR_RESET	0x1	/* Channel Reset bit */
#define TALITOS_CH_CCCR_HI	0x110c	/* Crypto-Channel Config Register */
#define TALITOS_CH_CCCR_HI_CDWE	0x10	/* Channel done writeback enable bit */
#define TALITOS_CH_CCCR_HI_NT	0x4	/* Notification type bit */
#define TALITOS_CH_CCCR_HI_CDIE	0x2	/* Channel Done Interrupt Enable bit */
#define TALITOS_CH_CCPSR	0x1110	/* Crypto-Channel Pointer Status Reg */
#define TALITOS_CH_CCPSR_HI	0x1114	/* Crypto-Channel Pointer Status Reg */
#define TALITOS_CH_FF		0x1148	/* Fetch FIFO */
#define TALITOS_CH_FF_HI	0x114c	/* Fetch FIFO's FETCH_ADRS */
#define TALITOS_CH_CDPR		0x1140	/* Crypto-Channel Pointer Status Reg */
#define TALITOS_CH_CDPR_HI	0x1144	/* Crypto-Channel Pointer Status Reg */
#define TALITOS_CH_DESCBUF	0x1180	/* (thru 11bf) Crypto-Channel 
					 * Descriptor Buffer (debug) */

/* execution unit register offset addresses and bits */
#define TALITOS_DEUSR		0x2028	/* DEU status register */
#define TALITOS_DEUSR_HI	0x202c	/* DEU status register */
#define TALITOS_DEUISR		0x2030	/* DEU interrupt status register */
#define TALITOS_DEUISR_HI	0x2034	/* DEU interrupt status register */
#define TALITOS_DEUICR		0x2038	/* DEU interrupt control register */
#define TALITOS_DEUICR_HI	0x203c	/* DEU interrupt control register */
#define TALITOS_AESUISR		0x4030	/* AESU interrupt status register */
#define TALITOS_AESUISR_HI	0x4034	/* AESU interrupt status register */
#define TALITOS_AESUICR		0x4038	/* AESU interrupt control register */
#define TALITOS_AESUICR_HI	0x403c	/* AESU interrupt control register */
#define TALITOS_MDEUISR		0x6030	/* MDEU interrupt status register */
#define TALITOS_MDEUISR_HI	0x6034	/* MDEU interrupt status register */
#define TALITOS_RNGSR		0xa028	/* RNG status register */
#define TALITOS_RNGSR_HI	0xa02c	/* RNG status register */
#define TALITOS_RNGSR_HI_RD	0x1	/* RNG Reset done */
#define TALITOS_RNGSR_HI_OFL	0xff0000/* number of dwords in RNG output FIFO*/
#define TALITOS_RNGDSR		0xa010	/* RNG data size register */
#define TALITOS_RNGDSR_HI	0xa014	/* RNG data size register */
#define TALITOS_RNG_FIFO	0xa800	/* RNG FIFO - pool of random numbers */
#define TALITOS_RNGISR		0xa030	/* RNG Interrupt status register */
#define TALITOS_RNGISR_HI	0xa034	/* RNG Interrupt status register */
#define TALITOS_RNGRCR		0xa018	/* RNG Reset control register */
#define TALITOS_RNGRCR_HI	0xa01c	/* RNG Reset control register */
#define TALITOS_RNGRCR_HI_SR	0x1	/* RNG RNGRCR:Software Reset */

/* descriptor pointer entry */
struct talitos_desc_ptr {
	u16	len;		/* length */
	u8	extent;		/* jump (to s/g link table) and extent */
	u8	res;		/* reserved */
	u32	ptr;		/* pointer */
};

/* descriptor */
struct talitos_desc {
	u32	hdr;				/* header */
	u32	res;				/* reserved */
	struct talitos_desc_ptr		ptr[7];	/* ptr/len pair array */
};

/* talitos descriptor header (hdr) bits */

/* primary execution unit select */
#define	TALITOS_SEL0_AFEU	0x10000000
#define	TALITOS_SEL0_DEU	0x20000000
#define	TALITOS_SEL0_MDEU	0x30000000
#define	TALITOS_SEL0_RNG	0x40000000
#define	TALITOS_SEL0_PKEU	0x50000000
#define	TALITOS_SEL0_AESU	0x60000000

/* primary execution unit mode (MODE0) and derivatives */
#define	TALITOS_MODE0_AESU_CBC		0x00200000
#define	TALITOS_MODE0_AESU_ENC		0x00100000
#define	TALITOS_MODE0_DEU_CBC		0x00400000
#define	TALITOS_MODE0_DEU_3DES		0x00200000
#define	TALITOS_MODE0_DEU_ENC		0x00100000
#define	TALITOS_MODE0_MDEU_INIT		0x01000000	/* init starting regs */
#define	TALITOS_MODE0_MDEU_HMAC		0x00800000
#define	TALITOS_MODE0_MDEU_PAD		0x00400000	/* PD */
#define	TALITOS_MODE0_MDEU_MD5		0x00200000
#define	TALITOS_MODE0_MDEU_SHA256	0x00100000
#define	TALITOS_MODE0_MDEU_SHA1		0x00000000	/* SHA-160 */
#define	TALITOS_MODE0_MDEU_MD5_HMAC	\
		(TALITOS_MODE0_MDEU_MD5 | TALITOS_MODE0_MDEU_HMAC)
#define	TALITOS_MODE0_MDEU_SHA256_HMAC	\
		(TALITOS_MODE0_MDEU_SHA256 | TALITOS_MODE0_MDEU_HMAC)
#define	TALITOS_MODE0_MDEU_SHA1_HMAC	\
		(TALITOS_MODE0_MDEU_SHA1 | TALITOS_MODE0_MDEU_HMAC)

/* secondary execution unit select (SEL1) */
/* it's MDEU or nothing */
#define	TALITOS_SEL1_MDEU	0x00030000

/* secondary execution unit mode (MODE1) and derivatives */
#define	TALITOS_MODE1_MDEU_INIT		0x00001000	/* init starting regs */
#define	TALITOS_MODE1_MDEU_HMAC		0x00000800
#define	TALITOS_MODE1_MDEU_PAD		0x00000400	/* PD */
#define	TALITOS_MODE1_MDEU_MD5		0x00000200
#define	TALITOS_MODE1_MDEU_SHA256	0x00000100
#define	TALITOS_MODE1_MDEU_SHA1		0x00000000	/* SHA-160 */
#define	TALITOS_MODE1_MDEU_MD5_HMAC	\
	(TALITOS_MODE1_MDEU_MD5 | TALITOS_MODE1_MDEU_HMAC)
#define	TALITOS_MODE1_MDEU_SHA256_HMAC	\
	(TALITOS_MODE1_MDEU_SHA256 | TALITOS_MODE1_MDEU_HMAC)
#define	TALITOS_MODE1_MDEU_SHA1_HMAC	\
	(TALITOS_MODE1_MDEU_SHA1 | TALITOS_MODE1_MDEU_HMAC)

/* direction of overall data flow (DIR) */
#define	TALITOS_DIR_OUTBOUND	0x00000000
#define	TALITOS_DIR_INBOUND	0x00000002

/* done notification (DN) */
#define	TALITOS_DONE_NOTIFY	0x00000001

/* descriptor types */
/* odd numbers here are valid on SEC2 and greater only (e.g. ipsec_esp) */
#define TD_TYPE_AESU_CTR_NONSNOOP	(0 << 3)
#define TD_TYPE_IPSEC_ESP		(1 << 3)
#define TD_TYPE_COMMON_NONSNOOP_NO_AFEU	(2 << 3)
#define TD_TYPE_HMAC_SNOOP_NO_AFEU	(4 << 3)

#define TALITOS_HDR_DONE_BITS	0xff000000

#define	DPRINTF(a...)	do { \
						if (debug) { \
							printk("%s: ", sc ? \
								device_get_nameunit(sc->sc_cdev) : "talitos"); \
							printk(a); \
						} \
					} while (0)
