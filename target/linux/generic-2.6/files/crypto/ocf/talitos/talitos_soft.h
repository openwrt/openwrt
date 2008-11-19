/*
 * Freescale SEC data structures for integration with ocf-linux
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
 */

/*
 * paired descriptor and associated crypto operation
 */
struct desc_cryptop_pair {
	struct talitos_desc	cf_desc;	/* descriptor ptr */
	struct cryptop		*cf_crp;	/* cryptop ptr */
};

/*
 * Holds data specific to a single talitos device.
 */
struct talitos_softc {
	softc_device_decl	sc_cdev;
	struct platform_device	*sc_dev;	/* device backpointer */
	ocf_iomem_t		sc_base_addr;
	int			sc_irq;
	int			sc_num;		/* if we have multiple chips */
	int32_t			sc_cid;		/* crypto tag */
	u64			sc_chiprev;	/* major/minor chip revision */
	int			sc_nsessions;
	struct talitos_session	*sc_sessions;
	int			sc_num_channels;/* number of crypto channels */
	int			sc_chfifo_len;	/* channel fetch fifo len */
	int			sc_exec_units;	/* execution units mask */
	int			sc_desc_types;	/* descriptor types mask */
	/*
	 * mutual exclusion for intra-channel resources, e.g. fetch fifos
	 * the last entry is a meta-channel lock used by the channel scheduler
	 */
	spinlock_t		*sc_chnfifolock;
	/* sc_chnlastalgo contains last algorithm for that channel */
	int			*sc_chnlastalg;
	/* sc_chnfifo holds pending descriptor--crypto operation pairs */
	struct desc_cryptop_pair	**sc_chnfifo;
};

struct talitos_session {
	u_int32_t	ses_used;
	u_int32_t	ses_klen;		/* key length in bits */
	u_int32_t	ses_key[8];		/* DES/3DES/AES key */
	u_int32_t	ses_hmac[5];		/* hmac inner state */
	u_int32_t	ses_hmac_len;		/* hmac length */
	u_int32_t	ses_iv[4];		/* DES/3DES/AES iv */
	u_int32_t	ses_mlen;		/* desired hash result len (12=ipsec or 16) */
};

#define	TALITOS_SESSION(sid)	((sid) & 0x0fffffff)
#define	TALITOS_SID(crd, sesn)	(((crd) << 28) | ((sesn) & 0x0fffffff))
