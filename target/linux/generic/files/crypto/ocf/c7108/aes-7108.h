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

#ifndef __AES_7108_H__
#define __AES_7108_H__

/* Cypher 7108 AES Controller Hardware */
#define CCU_REG_BASE       0x1b500000	
#define CCU_AES_REG_BASE   (CCU_REG_BASE + 0x100)
#define C7108_AES_KEY0_LO        (0x0000) 
#define C7108_AES_KEY0_HI        (0x0004) 
#define C7108_AES_KEY1_LO        (0x0008) 
#define C7108_AES_KEY1_HI        (0x000c) 
#define C7108_AES_KEY2_LO        (0x0010) 
#define C7108_AES_KEY2_HI        (0x0014) 
#define C7108_AES_KEY3_LO        (0x0018) 
#define C7108_AES_KEY3_HI        (0x001c) 
#define C7108_AES_KEY4_LO        (0x0020) 
#define C7108_AES_KEY4_HI        (0x0024) 
#define C7108_AES_KEY5_LO        (0x0028) 
#define C7108_AES_KEY5_HI        (0x002c) 
#define C7108_AES_KEY6_LO        (0x0030) 
#define C7108_AES_KEY6_HI        (0x0034) 
#define C7108_AES_KEY7_LO        (0x0038) 
#define C7108_AES_KEY7_HI        (0x003c) 
#define C7108_AES_IV0_LO         (0x0040) 
#define C7108_AES_IV0_HI         (0x0044) 
#define C7108_AES_IV1_LO         (0x0048) 
#define C7108_AES_IV1_HI         (0x004c) 
#define C7108_AES_IV2_LO         (0x0050) 
#define C7108_AES_IV2_HI         (0x0054) 
#define C7108_AES_IV3_LO         (0x0058) 
#define C7108_AES_IV3_HI         (0x005c) 

#define C7108_AES_DMA_SRC0_LO    (0x0068) /* Bits 0:15 */
#define C7108_AES_DMA_SRC0_HI    (0x006c) /* Bits 27:16 */
#define C7108_AES_DMA_DST0_LO    (0x0070) /* Bits 0:15 */
#define C7108_AES_DMA_DST0_HI    (0x0074) /* Bits 27:16 */
#define C7108_AES_DMA_LEN        (0x0078)  /*Bytes:(Count+1)x16 */

/* AES/Copy engine control register */
#define C7108_AES_CTRL           (0x007c) /* AES control */
#define C7108_AES_CTRL_RS        (1<<0)     /* Which set of src/dst to use */

/* AES Cipher mode, controlled by setting Bits 2:0 */
#define C7108_AES_CTRL_MODE_CBC     0
#define C7108_AES_CTRL_MODE_CFB     (1<<0)
#define C7108_AES_CTRL_MODE_OFB     (1<<1)
#define C7108_AES_CTRL_MODE_CTR     ((1<<0)|(1<<1))
#define C7108_AES_CTRL_MODE_ECB     (1<<2)

/* AES Key length , Bits 5:4 */
#define C7108_AES_KEY_LEN_128         0       /* 00 */
#define C7108_AES_KEY_LEN_192         (1<<4)  /* 01 */
#define C7108_AES_KEY_LEN_256         (1<<5)  /* 10 */

/* AES Operation (crypt/decrypt), Bit 3 */
#define C7108_AES_DECRYPT             (1<<3)   /* Clear for encrypt */
#define C7108_AES_ENCRYPT              0       
#define C7108_AES_INTR                (1<<13) /* Set on done trans from 0->1*/
#define C7108_AES_GO                  (1<<14) /* Run */
#define C7108_AES_OP_DONE             (1<<15) /* Set when complete */


/* Expanded key registers */
#define C7108_AES_EKEY0_LO            (0x0080)
#define C7108_AES_EKEY0_HI            (0x0084)
#define C7108_AES_EKEY1_LO            (0x0088)
#define C7108_AES_EKEY1_HI            (0x008c)
#define C7108_AES_EKEY2_LO            (0x0090)
#define C7108_AES_EKEY2_HI            (0x0094)
#define C7108_AES_EKEY3_LO            (0x0098)
#define C7108_AES_EKEY3_HI            (0x009c)
#define C7108_AES_EKEY4_LO            (0x00a0)
#define C7108_AES_EKEY4_HI            (0x00a4)
#define C7108_AES_EKEY5_LO            (0x00a8)
#define C7108_AES_EKEY5_HI            (0x00ac)
#define C7108_AES_EKEY6_LO            (0x00b0)
#define C7108_AES_EKEY6_HI            (0x00b4)
#define C7108_AES_EKEY7_LO            (0x00b8)
#define C7108_AES_EKEY7_HI            (0x00bc)
#define C7108_AES_OK                  (0x00fc) /* Reset: "OK" */

#define offset_in_page(p) ((unsigned long)(p) & ~PAGE_MASK)

/* Software session entry */

#define HW_TYPE_CIPHER	0
#define SW_TYPE_HMAC	1
#define SW_TYPE_AUTH2	2
#define SW_TYPE_HASH	3
#define SW_TYPE_COMP	4

struct cipher_7108 {
	int			xfm_type;
	int			cri_alg;
	union {
		struct {
			char sw_key[HMAC_BLOCK_LEN];
			int  sw_klen;
			int  sw_authlen;
		} hmac;
	} u;
	struct cipher_7108	*next;
};



#endif /* __C7108_AES_7108_H__ */
