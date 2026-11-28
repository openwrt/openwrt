#ifndef _CRYPTO_SIFLOWER_COMMON_H
#define _CRYPTO_SIFLOWER_COMMON_H

#define pr_fmt(fmt)	"siflower-crypto: " fmt
#include <linux/unaligned.h>
#include <crypto/aes.h>
#include <crypto/gcm.h>
#include <crypto/scatterwalk.h>
#include <crypto/internal/aead.h>
#include <crypto/internal/hash.h>
#include <crypto/internal/skcipher.h>
#include <crypto/md5.h>
#include <crypto/sha1.h>
#include <crypto/sha2.h>
#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/types.h>

#define DMA_CH_NUM	4
#define DMA_RX_ALIGN	8
#define DMA_RING_SIZE	4096
#define SHA1_DMA_SIZE	ALIGN(SHA1_DIGEST_SIZE, DMA_RX_ALIGN)
#define SHA1_DMA_OFFSET	(SHA1_DMA_SIZE - SHA1_DIGEST_SIZE)

enum {
	DMA_CH_SHA,
	DMA_CH_AES,
	DMA_CH_RSA,
	DMA_CH_MD5,
};

struct sf_ce_desc {
	__le32 des0;	// buf1 addr
	__le32 des1;	// buf2 addr
	__le32 des2;
	__le32 des3;
};

struct sf_ce_chan {
	dma_addr_t dma_tx_phy;
	dma_addr_t dma_rx_phy;
	struct sf_ce_desc *dma_tx;
	struct sf_ce_desc *dma_rx;
	struct crypto_async_request **areqs;
	unsigned int cur_tx;
	unsigned int cur_rx;
	unsigned int dirty_tx;
	unsigned int dirty_rx;
	struct tasklet_struct bh;
#ifdef CONFIG_CRYPTO_SIFLOWER_HASH
	spinlock_t ring_lock;
#endif
	unsigned int ch_num;
	int tx_irq;
	int rx_irq;
} ____cacheline_aligned_in_smp;

struct sf_ce_dev {
	void __iomem *base;
	struct device *dev;
	struct clk *csr_clk;
	struct clk *app_clk;
	struct reset_control *csr_rstc;
	struct reset_control *app_rstc;
	struct sf_ce_chan chan[DMA_CH_NUM];
	dma_addr_t zero_pad_phys;
};

struct sf_ce_aes_reqctx {
	unsigned int ssg_len;
	unsigned int dsg_len;
	u8 iv[AES_BLOCK_SIZE];
	dma_addr_t iv_phys;
	void *tmp_buf;
	dma_addr_t tmp_buf_phys;
};

struct sf_ce_aes_ctx {
	struct sf_ce_dev *priv;
	struct crypto_sync_skcipher *fback;
	dma_addr_t key_phys;
	u8 key[AES_MAX_KEY_SIZE];
	u32 keylen;
};

struct sf_ce_aes_gcm_reqctx {
	u8 iv[AES_BLOCK_SIZE];
	__be64 alen_dma;
	__be64 plen_dma;
	__be64 taglen_dma;
	u8 tag[AES_BLOCK_SIZE];
	dma_addr_t iv_extra_tag_phys;
	void *tmp_buf;
	dma_addr_t tmp_buf_phys;
	unsigned int ssg_len;
	unsigned int dsg_len;
};

struct sf_ce_aes_gcm_ctx {
	/* key and k0 together must be physically contiguous.
	 * used as a single dma map
	 */
	u8 key[AES_MAX_KEY_SIZE];
	u8 k0[AES_BLOCK_SIZE];
	struct sf_ce_dev *priv;
	dma_addr_t key_0_phys;
	struct crypto_aead *fback;
	u32 keylen;
};

struct sf_ce_aes_ccm_reqctx {
	u8 iv[AES_BLOCK_SIZE];
	__be64 taglen_dma;
	u8 tag[AES_BLOCK_SIZE];
	u8 b0[AES_BLOCK_SIZE];
	__be16 alen_dma;
	dma_addr_t iv_extra_tag_phys;
	void *tmp_buf;
	dma_addr_t tmp_buf_phys;
	unsigned int ssg_len;
	unsigned int dsg_len;
};

struct sf_ce_aes_ccm_ctx {
	u8 key[AES_MAX_KEY_SIZE];
	struct sf_ce_dev *priv;
	dma_addr_t key_0_phys;
	struct crypto_aead *fback;
	u32 keylen;
};

struct sf_ce_ahash_reqctx {
	/* buffer for intermediate hash state, must be aligned to cacheline
	 * size on non-coherent RX DMA to prevent data loss */
	u8 hash[SHA256_DIGEST_SIZE] __aligned(ARCH_DMA_MINALIGN);
	/* buffer for partial updates and padding */
	u8 block[SHA256_BLOCK_SIZE * 2 + sizeof(u64)] __aligned(ARCH_DMA_MINALIGN);
	/* DMA address of @hash */
	dma_addr_t hash_phys;
	/* DMA address of @block */
	dma_addr_t block_phys;
	/* number of valid bytes in @block */
	u32 block_len;
	/* number of req->src segments */
	int nents;
	/* total plaintext length */
	u64 length;
	/* final state */
	bool final;
};

struct sf_ce_ahash512_reqctx {
	/* buffer for intermediate hash state, must be aligned to cacheline
	 * size on non-coherent RX DMA to prevent data loss */
	u8 hash[SHA512_DIGEST_SIZE] __aligned(ARCH_DMA_MINALIGN);
	/* buffer for partial updates and padding */
	u8 block[SHA512_BLOCK_SIZE * 2 + sizeof(__uint128_t)] __aligned(ARCH_DMA_MINALIGN);
	/* DMA address of @hash */
	dma_addr_t hash_phys;
	/* DMA address of @block */
	dma_addr_t block_phys;
	/* number of valid bytes in @block */
	u32 block_len;
	/* number of req->src segments */
	int nents;
	/* total plaintext length */
	__uint128_t length;
	/* final state */
	bool final;
};

struct sf_ce_ahash_ctx {
	struct sf_ce_dev *priv;
};

struct sf_ce_md5_state {
	u64 count;
	u8 hash[MD5_DIGEST_SIZE];
	u8 block[MD5_HMAC_BLOCK_SIZE];
};

struct sf_ce_sha1_state {
	u64 count;
	u8 hash[SHA1_DIGEST_SIZE];
	u8 block[SHA1_BLOCK_SIZE];
};

struct sf_ce_sha256_state {
	u64 count;
	u8 hash[SHA256_DIGEST_SIZE];
	u8 block[SHA256_BLOCK_SIZE];
};

struct sf_ce_sha512_state {
	__uint128_t count;
	u8 hash[SHA512_DIGEST_SIZE];
	u8 block[SHA512_BLOCK_SIZE];
};

#define CE_DMA_MODE			0x00003000
#define CE_INTM			GENMASK(13, 12)
#define CE_SWR			BIT(0)
#define CE_DMA_SYSBUS_MODE		0x00003004
#define CE_WR_OSR_LMT		GENMASK(29, 24)
#define CE_WR_OSR_LMT_SHIFT		24
#define CE_RD_OSR_LMT		GENMASK(21, 16)
#define CE_RD_OSR_LMT_SHIFT		16
#define CE_EN_LPI			BIT(15)
#define CE_LPI_XIT_PKT		BIT(14)
#define CE_AAL			BIT(12)
#define CE_EAME			BIT(11)
#define CE_BLEN			GENMASK(7, 1)
#define CE_BLEN256			BIT(7)
#define CE_BLEN128			BIT(6)
#define CE_BLEN64			BIT(5)
#define CE_BLEN32			BIT(4)
#define CE_BLEN16			BIT(3)
#define CE_BLEN8			BIT(2)
#define CE_BLEN4			BIT(1)
#define CE_UNDEF			BIT(0)
#define CE_DMA_INT_STATUS		0x00003008
#define CE_MTLIS			BIT(16)
#define CE_TX_EDMA_CTRL		0x00003040
#define CE_TEDM			GENMASK(31, 30)
#define CE_TDPS			GENMASK(29, 0)
#define CE_RX_EDMA_CTRL		0x00003044
#define CE_REDM			GENMASK(31, 30)
#define CE_RDPS			GENMASK(29, 0)
#define CE_DMA_TBS_CTRL0		0x00003054
#define CE_DMA_TBS_CTRL1		0x00003058
#define CE_DMA_TBS_CTRL2		0x0000305c
#define CE_DMA_TBS_CTRL3		0x00003060
#define CE_FTOS			GENMASK(31, 8)
#define CE_FTOV			BIT(0)
#define CE_DEF_FTOS			(CE_FTOS | CE_FTOV)
#define CE_DMA_SAFETY_INT_STATUS	0x00003064
#define CE_MCSIS			BIT(31)
#define CE_MSUIS			BIT(29)
#define CE_MSCIS			BIT(28)
#define CE_DEUIS			BIT(1)
#define CE_DECIS			BIT(0)
#define CE_DMA_ECC_INT_ENABLE	0x00003068
#define CE_DCEIE			BIT(1)
#define CE_TCEIE			BIT(0)
#define CE_DMA_ECC_INT_STATUS	0x0000306c
#define CE_DMA_CH_CONTROL(x)		(0x00003100 + 0x80 * (x))
#define CE_SPH			BIT(24)
#define CE_PBLx8			BIT(16)
#define CE_DMA_CH_TX_CONTROL(x)	(0x00003104 + 0x80 * (x))
#define CE_EDSE			BIT(28)
#define CE_TxPBL			GENMASK(21, 16)
#define CE_TxPBL_SHIFT		16
#define CE_TSE			BIT(12)
#define CE_OSP			BIT(4)
#define CE_TXST			BIT(0)
#define CE_DMA_CH_RX_CONTROL(x)	(0x00003108 + 0x80 * (x))
#define CE_RPF			BIT(31)
#define CE_RxPBL			GENMASK(21, 16)
#define CE_RxPBL_SHIFT		16
#define CE_RBSZ			GENMASK(14, 1)
#define CE_RBSZ_SHIFT		1
#define CE_RXST			BIT(0)
#define CE_DMA_CH_TxDESC_LADDR(x)	(0x00003114 + 0x80 * (x))
#define CE_DMA_CH_RxDESC_LADDR(x)	(0x0000311c + 0x80 * (x))
#define CE_DMA_CH_TxDESC_TAIL_LPTR(x)	(0x00003124 + 0x80 * (x))
#define CE_DMA_CH_RxDESC_TAIL_LPTR(x)	(0x0000312c + 0x80 * (x))
#define CE_DMA_CH_TxDESC_RING_LEN(x)		(0x00003130 + 0x80 * (x))
#define CE_DMA_CH_RxDESC_RING_LEN(x)		(0x00003134 + 0x80 * (x))
#define CE_DMA_CH_INT_EN(x)		(0x00003138 + 0x80 * (x))
#define CE_NIE			BIT(15)
#define CE_AIE			BIT(14)
#define CE_RBUE			BIT(7)
#define CE_RIE			BIT(6)
#define CE_TBUE			BIT(2)
#define CE_TIE			BIT(0)
#define CE_DMA_INT_DEFAULT_EN	(CE_RIE)
#define CE_DMA_INT_DEFAULT_RX	(CE_RBUE | CE_RIE)
#define CE_DMA_INT_DEFAULT_TX	(CE_TBUE | CE_TIE)
#define CE_DMA_CH_Rx_WATCHDOG(x)	(0x0000313c + 0x80 * (x))
#define CE_RWT			GENMASK(7, 0)
#define CE_DMA_CH_CUR_TxDESC_LADDR(x)	(0x00003144 + 0x80 * (x))
#define CE_DMA_CH_CUR_RxDESC_LADDR(x)	(0x0000314c + 0x80 * (x))
#define CE_DMA_CH_CUR_RxBUFF_LADDR(x)	(0x0000315c + 0x80 * (x))
#define CE_DMA_CH_STATUS(x)		(0x00003160 + 0x80 * (x))
#define CE_NIS			BIT(15)
#define CE_AIS			BIT(14)
#define CE_FBE			BIT(12)
#define CE_RPS			BIT(8)
#define CE_RBU			BIT(7)
#define CE_RI			BIT(6)
#define CE_TBU			BIT(2)
#define CE_TPS			BIT(1)
#define CE_TI			BIT(0)
#define CE_REGSIZE			((0x0000317c + (0x80 * 15)) / 4)

#define CE_DMA_STATUS_MSK_COMMON	(CE_NIS | CE_AIS | CE_FBE)
#define CE_DMA_STATUS_MSK_RX		(CE_RBU | CE_RI | \
					 CE_DMA_STATUS_MSK_COMMON)
#define CE_DMA_STATUS_MSK_TX		(CE_TBU | CE_TPS | CE_TI | \
					 CE_DMA_STATUS_MSK_COMMON)

#define CE_TDES2_IOC			BIT(31)
#define CE_TDES2_SC			BIT(30)
#define CE_TDES2_ED			BIT(29)
enum {
	CE_ENDIAN_LITTLE,
	CE_ENDIAN_BIG,
};

#define CE_TDES2_B2T			GENMASK(28, 26)
#define CE_TDES2_B2L			GENMASK(25, 13)
#define CE_TDES2_B1L			GENMASK(12, 0)
#define CE_TDES3_OWN			BIT(31)
#define CE_TDES3_FD			BIT(30)
#define CE_TDES3_LD			BIT(29)
#define CE_TDES3_B1T			GENMASK(27, 25)

enum crypt_buffer_type {
	/* Payload (cipher text or plain text) for all algorithms */
	CE_BUF_PAYLOAD = 6,
	/* MD5, SHA initial */
	CE_BUF_HASH_INIT = 0,
	/* RSA */
	CE_BUF_RSA_E = 1,
	CE_BUF_RSA_N = 2,
	/* AES */
	CE_BUF_AES_KEY = 4,
	CE_BUF_AES_IV = 5,
	/* Additional params for AEAD */
	CE_BUF_AES_AEAD_EXTRA = 0,
	CE_BUF_AES_AEAD_TAG = 1,
	CE_BUF_AES_AEAD_K0 = 3,
	CE_BUF_AES_AEAD_HEADER = 7,
};

#define CE_TDES3_CM			BIT(24)
enum {
	CM_ENCRYPT,
	CM_DECRYPT,
};

#define CE_TDES3_CT			GENMASK(23, 20)
enum crypt_algo {
	CE_MD5,
	CE_SHA1,
	CE_SHA256,
	CE_SHA512,
	CE_RSA1024,
	CE_RSA2048,
	CE_RSA4096,
	CE_AES_CTR,
	CE_AES_CCM,
	CE_AES_GCM,
};
#define CE_TDES3_PL			GENMASK(19, 0)

#define CE_RDES2_B2L			GENMASK(31, 16)
#define CE_RDES2_B1L			GENMASK(15, 0)
#define CE_RDES3_OWN			BIT(31)
#define CE_RDES3_IOC			BIT(30)
#define CE_RDES3_TL			GENMASK(7, 0)

#define CE_RDES2_WB_TL			GENMASK(31, 16)
#define CE_RDES3_WB_CM			BIT(28)
#define CE_RDES3_WB_CT			GENMASK(27, 24)
#define CE_RDES3_WB_AEAD_VERIFY		BIT(23)
#define CE_RDES3_WB_FD			BIT(21)
#define CE_RDES3_WB_LD			BIT(20)
#define CE_RDES3_WB_LEN			GENMASK(19, 0)

#define CE_US_ENDIAN_CFG		0x1092c
#define CE_US_ENDIAN_CFG_0		BIT(0)

extern struct sf_ce_dev *g_dev;

static __always_inline u32
reg_read(struct sf_ce_dev *priv, unsigned long addr)
{
	return readl_relaxed(priv->base + addr);
}

static __always_inline void
reg_write(struct sf_ce_dev *priv, unsigned long addr, u32 val)
{
	writel_relaxed(val, priv->base + addr);
}

static __always_inline void
reg_write_flush(struct sf_ce_dev *priv, unsigned long addr, u32 val)
{
	writel(val, priv->base + addr);
}

static __always_inline void
reg_rmw(struct sf_ce_dev *priv, unsigned long addr, u32 clear, u32 set)
{
	u32 val;

	val = readl_relaxed(priv->base + addr);
	val &= ~clear;
	val |= set;
	writel_relaxed(val, priv->base + addr);
}

static __always_inline void
reg_set(struct sf_ce_dev *priv, unsigned long addr, u32 set)
{
	return reg_rmw(priv, addr, 0, set);
}

static __always_inline void
reg_clear(struct sf_ce_dev *priv, unsigned long addr, u32 clear)
{
	return reg_rmw(priv, addr, clear, 0);
}

#endif
