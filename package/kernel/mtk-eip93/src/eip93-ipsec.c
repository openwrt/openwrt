// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021
 *
 * Richard van Schagen <vschagen@icloud.com>
 */

#include <linux/skbuff.h>
#include <linux/init.h>
#include <net/protocol.h>
#include <crypto/aead.h>
#include <crypto/authenc.h>
#include <linux/err.h>
#include <linux/module.h>
#include <net/ip.h>
#include <net/xfrm.h>
#include <net/esp.h>
#include <linux/scatterlist.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <net/udp.h>

#include <crypto/ctr.h>
#include <linux/netdevice.h>
#include <net/esp.h>
#include <net/xfrm.h>

#include "eip93-common.h"
#include "eip93-main.h"
#include "eip93-ipsec.h"
#include "eip93-regs.h"

static int mtk_xfrm_add_state(struct xfrm_state *x);
static void mtk_xfrm_del_state(struct xfrm_state *x);
static void mtk_xfrm_free_state(struct xfrm_state *x);
static bool mtk_ipsec_offload_ok(struct sk_buff *skb, struct xfrm_state *x);
static void mtk_advance_esn_state(struct xfrm_state *x);

static const struct xfrmdev_ops mtk_xfrmdev_ops = {
	.xdo_dev_state_add      = mtk_xfrm_add_state,
	.xdo_dev_state_delete   = mtk_xfrm_del_state,
	.xdo_dev_state_free     = mtk_xfrm_free_state,
	.xdo_dev_offload_ok     = mtk_ipsec_offload_ok,
	.xdo_dev_state_advance_esn = mtk_advance_esn_state,
};

int mtk_add_xfrmops(struct net_device *netdev)
{
	if (netdev->features & NETIF_F_HW_ESP)
		return NOTIFY_DONE;

	if(netdev->irq != 24) { // Workaround for gmac1
		if (netdev->dev.type == NULL)
			return NOTIFY_DONE;

		if (strcmp(netdev->dev.type->name, "dsa"))  {
			if (strcmp(netdev->dev.type->name, "bridge"))
				return NOTIFY_DONE;
		}
	}

	/* enable ESP HW offload */
	netdev->xfrmdev_ops = &mtk_xfrmdev_ops;
	netdev->features |= NETIF_F_HW_ESP;
	netdev->hw_enc_features |= NETIF_F_HW_ESP;
	/* enable ESP GSO */
	netdev->features |= NETIF_F_GSO_ESP;
	netdev->hw_enc_features |= NETIF_F_GSO_ESP;

	netdev_change_features(netdev);
	netdev_info(netdev, "ESP HW offload added.\n");
	return NOTIFY_DONE;
}

int mtk_del_xfrmops(struct net_device *netdev)
{
	if (netdev->features & NETIF_F_HW_ESP) {
		netdev->xfrmdev_ops = NULL;
		netdev->hw_enc_features &= ~NETIF_F_HW_ESP;
		netdev->features &= ~NETIF_F_HW_ESP;
		netdev_change_features(netdev);
		netdev_info(netdev, "ESP HW offload removed.\n");
	}

	return NOTIFY_DONE;
}

/*
 * mtk_validate_state
 * return 0 in case doesn't validate or "flags" which
 * can never be "0"
 */
u32 mtk_validate_state(struct xfrm_state *x)
{
 	struct net_device *netdev = x->xso.dev;
 	u32 flags = 0;

	if (x->id.proto != IPPROTO_ESP) {
 		netdev_info(netdev, "Only ESP XFRM state may be offloaded\n");
 		return 0;
 	}
	/* TODO: add ipv6 support */
	if (x->props.family != AF_INET) {
//		&& x->props.family != AF_INET6) {
		netdev_info(netdev, "Only IPv4 xfrm states may be offloaded\n");
		return 0;
	}
	if (x->aead) {
		netdev_info(netdev, "Cannot offload xfrm states with aead\n");
		return 0;
	}
 	if (x->props.aalgo == SADB_AALG_NONE) {
 		netdev_info(netdev, "Cannot offload without authentication\n");
 		return 0;
 	}
 	if (x->props.calgo != SADB_X_CALG_NONE) {
 		netdev_info(netdev, "Cannot offload compressed xfrm states\n");
 		return 0;
 	}
 	/* TODO: support ESN */
 	if (x->props.flags & XFRM_STATE_ESN) {
 		netdev_info(netdev, "Cannot offload ESN xfrm states\n");
 		return 0;
 	}
 	/* TODO: add transport mode */
 	if (x->props.mode != XFRM_MODE_TUNNEL
			&& x->props.mode != XFRM_MODE_TRANSPORT) {
 		netdev_info(netdev, "only offload Tunnel & Transport Mode\n");
 		return 0;
 	}
 	if (x->encap) {
 		netdev_info(netdev, "Encapsulated xfrm can not be offloaded\n");
 		return 0;
 	}
 	if (x->tfcpad) {
 		netdev_info(netdev, "No tfc padding supported\n");
 		return 0;
 	}

     	switch (x->props.ealgo) {
 	case SADB_EALG_DESCBC:
 		flags |= MTK_ALG_DES | MTK_MODE_CBC;
 		break;
 	case SADB_EALG_3DESCBC:
 		flags |= MTK_ALG_3DES | MTK_MODE_CBC;
 		break;
 	case SADB_X_EALG_AESCBC:
 		flags |= MTK_ALG_AES | MTK_MODE_CBC;
 		break;
 	case SADB_X_EALG_AESCTR: // CTR is ONLY in RFC3686 for ESP
 		flags |= MTK_ALG_AES | MTK_MODE_CTR | MTK_MODE_RFC3686;
 	case SADB_EALG_NULL:
 		break;
 	default:
 		netdev_info(netdev, "Cannot offload encryption: %s\n",
							x->ealg->alg_name);
 		return 0;
 	}

 	switch (x->props.aalgo) {
 	case SADB_AALG_SHA1HMAC:
 		flags |= MTK_HASH_HMAC | MTK_HASH_SHA1;
 		break;
 	case SADB_X_AALG_SHA2_256HMAC:
 		flags |= MTK_HASH_HMAC | MTK_HASH_SHA256;
 		break;
 	case SADB_AALG_MD5HMAC:
 		flags |= MTK_HASH_HMAC | MTK_HASH_MD5;
 		break;
 	default:
 		netdev_info(netdev, "Cannot offload authentication: %s\n",
							x->aalg->alg_name);
 		return 0;
	}
 /*
 	if (x->aead->alg_icv_len != 128) {
 		netdev_info(netdev, "Cannot offload xfrm states with AEAD ICV length other than 128bit\n");
 		return -EINVAL;
 	}
 */

 /*
 	TODO check key_len
 	// split for RFC3686 with nonce vs others !!
 	if ((x->aead->alg_key_len != 128 + 32) &&
 	    (x->aead->alg_key_len != 256 + 32)) {
 		netdev_info(netdev, "Cannot offload xfrm states with AEAD key length other than 128/256 bit\n");
 		return -EINVAL;
 	}
 */
	return flags;
}

static int mtk_create_sa(struct mtk_device *mtk, struct ipsec_sa_entry *ipsec,
			struct xfrm_state *x, u32 flags)
{
	struct saRecord_s *saRecord;
	char *alg_base;
	const u8 *enckey = x->ealg->alg_key;
	unsigned int enckeylen = (x->ealg->alg_key_len >>3);
	const u8 *authkey = x->aalg->alg_key;
	unsigned int authkeylen = (x->aalg->alg_key_len >>3);
	unsigned int trunc_len = (x->aalg->alg_trunc_len >>3);
	u32 nonce = 0;
	int err;

	if (IS_HASH_MD5(flags))
		alg_base = "md5";
	if (IS_HASH_SHA1(flags))
		alg_base = "sha1";
	if (IS_HASH_SHA256(flags))
		alg_base = "sha256";

	ipsec->shash = crypto_alloc_shash(alg_base, 0, CRYPTO_ALG_NEED_FALLBACK);

	if (IS_ERR(ipsec->shash)) {
	 	dev_err(mtk->dev, "base driver %s could not be loaded.\n",
			 alg_base);
	return PTR_ERR(ipsec->shash);
	}

	ipsec->sa = kzalloc(sizeof(struct saRecord_s), GFP_KERNEL);
	if (!ipsec->sa)
		return -ENOMEM;

	ipsec->sa_base = dma_map_single(mtk->dev, ipsec->sa,
				sizeof(struct saRecord_s), DMA_TO_DEVICE);

	saRecord = ipsec->sa;

	if (IS_RFC3686(flags)) {
		if (enckeylen < CTR_RFC3686_NONCE_SIZE)
			dev_err(mtk->dev, "rfc 3686 bad key\n");

		enckeylen -= CTR_RFC3686_NONCE_SIZE;
		memcpy(&nonce, enckey + enckeylen,
						CTR_RFC3686_NONCE_SIZE);
	}

	/* Encryption key */
	mtk_set_saRecord(saRecord, enckeylen, flags);

	memcpy(saRecord->saKey, enckey, enckeylen);
	saRecord->saNonce = nonce;

	/* authentication key */
	err = mtk_authenc_setkey(ipsec->shash,  saRecord, authkey, authkeylen);
	if (err)
		dev_err(mtk->dev, "Set Key failed: %d\n", err);

	saRecord->saCmd0.bits.opGroup = 1;
	saRecord->saCmd0.bits.opCode = 0;
	saRecord->saCmd1.bits.byteOffset = 0;
	saRecord->saCmd1.bits.hashCryptOffset = 0;
	saRecord->saCmd0.bits.digestLength = (trunc_len >> 2);
	saRecord->saCmd1.bits.hmac = 1;
	saRecord->saCmd0.bits.padType = 0; // IPSec padding
	saRecord->saCmd0.bits.extPad = 0;
	saRecord->saCmd0.bits.scPad = 1; // Allow Stream Cipher padding
	saRecord->saCmd1.bits.copyPad = 0;
	saRecord->saCmd0.bits.hdrProc = 1;
	saRecord->saCmd1.bits.seqNumCheck = 1;
	saRecord->saSpi = ntohl(x->id.spi);
	saRecord->saSeqNum[0] = 0;
	saRecord->saSeqNum[1] = 0;

	if (x->xso.flags & XFRM_OFFLOAD_INBOUND) {
		saRecord->saCmd0.bits.direction = 1;
		saRecord->saCmd1.bits.copyHeader = 1;
		saRecord->saCmd1.bits.copyDigest = 0;
		saRecord->saCmd0.bits.ivSource = 1;
		flags |= MTK_DECRYPT;
		dev_dbg(mtk->dev, "Inbound SA created. SPI: %08x\n",
							ntohl(x->id.spi));
	} else {
		saRecord->saCmd0.bits.direction = 0;
		saRecord->saCmd1.bits.copyHeader = 0;
		saRecord->saCmd1.bits.copyDigest = 1;
		saRecord->saCmd0.bits.ivSource = 3;
		flags |= MTK_ENCRYPT;
		dev_dbg(mtk->dev, "Outbound SA created. SPI: %08x\n",
							ntohl(x->id.spi));
	}

	ipsec->cdesc.peCrtlStat.bits.hostReady = 1;
	ipsec->cdesc.peCrtlStat.bits.prngMode = 0;
	ipsec->cdesc.peCrtlStat.bits.hashFinal = 1;
	ipsec->cdesc.peCrtlStat.bits.padCrtlStat = 2; // Pad align 4 as esp4.c
	ipsec->cdesc.peCrtlStat.bits.peReady = 0;
	ipsec->cdesc.saAddr = ipsec->sa_base;
	ipsec->cdesc.stateAddr = 0;
	ipsec->cdesc.arc4Addr = 0;
	ipsec->cdesc.userId = flags |
			MTK_DESC_IPSEC | MTK_DESC_LAST | MTK_DESC_FINISH;

	return 0;
}

 /*
 * mtk_xfrm_add_state
 */
static int mtk_xfrm_add_state(struct xfrm_state *x)
{
	struct crypto_rng *rng;
	struct rng_alg *alg;
	struct mtk_alg_template *tmpl;
	struct mtk_device *mtk;
	struct ipsec_sa_entry *ipsec;
	u32 flags = 0;
	int err;

	rng = crypto_alloc_rng("eip93-prng", 0, 0);
	if (IS_ERR(rng))
		return -EOPNOTSUPP;

	alg = crypto_rng_alg(rng);
	tmpl = container_of(alg, struct mtk_alg_template, alg.rng);
	mtk = tmpl->mtk;
	crypto_free_rng(rng);

	flags = mtk_validate_state(x);

	if (!flags)
		return -EOPNOTSUPP;

	ipsec = kmalloc(sizeof(struct ipsec_sa_entry), GFP_KERNEL);

	/* TODO: changed to ipsec pointer
	 * TODO: add key checks
	 */

	err = mtk_create_sa(mtk, ipsec, x, flags);
	if (err) {
		dev_err(mtk->dev, "error creating sa\n");
		return err;
	}
	ipsec->mtk = mtk;

	x->xso.offload_handle = (unsigned long)ipsec;
	try_module_get(THIS_MODULE);

	return 0;
}

static void mtk_xfrm_del_state(struct xfrm_state *x)
{
	// do nothing.

	return;
}

static void mtk_xfrm_free_state(struct xfrm_state *x)
{
	struct mtk_device *mtk;
	struct ipsec_sa_entry *ipsec;

	ipsec = (struct ipsec_sa_entry *)x->xso.offload_handle;
	mtk = ipsec->mtk;

	dma_unmap_single(mtk->dev, ipsec->sa_base, sizeof(struct saRecord_s),
								DMA_TO_DEVICE);
	kfree(ipsec->sa);
	kfree(ipsec);

	module_put(THIS_MODULE);

	return;
}

static void mtk_advance_esn_state(struct xfrm_state *x)
{
	return;
}

/**
 * mtk_ipsec_offload_ok - can this packet use the xfrm hw offload
 * @skb: current data packet
 * @xs: pointer to transformer state struct
 **/
static bool mtk_ipsec_offload_ok(struct sk_buff *skb, struct xfrm_state *x)
{
	if (x->props.family == AF_INET) {
		/* Offload with IPv4 options is not supported yet */
		if (ip_hdr(skb)->ihl != 5)
			return false;
	} else {
		/* Offload with IPv6 extension headers is not support yet */
		if (ipv6_ext_hdr(ipv6_hdr(skb)->nexthdr))
			return false;
	}

	return true;
}

void mtk_ipsec_rx_done(unsigned long data)
{
	struct mtk_device *mtk = (struct mtk_device *)data;
	struct mtk_ipsec_cb *mtk_ipsec_cb;
	struct sk_buff *skb;
	dma_addr_t dstAddr;
	u8 nexthdr;
	int err, len;
//	int handled = 0;
	struct ipsec_sa_entry *ipsec;
	struct xfrm_state *x;
	struct xfrm_offload *xo;
	__wsum csumdiff;

	while ((skb = skb_dequeue(&mtk->ring->rx_queue))) {
		mtk_ipsec_cb = (struct mtk_ipsec_cb *)MTK_IPSEC_CB(skb)->cb;
		nexthdr = mtk_ipsec_cb->nexthdr;
		err = mtk_ipsec_cb->err;
		len = mtk_ipsec_cb->len;
		dstAddr = mtk_ipsec_cb->dstAddr;
		MTK_IPSEC_CB(skb)->cb = mtk_ipsec_cb->org_cb;
		kfree(mtk_ipsec_cb);

		x = xfrm_input_state(skb);
		ipsec = (struct ipsec_sa_entry *)x->xso.offload_handle;

		xo = xfrm_offload(skb);
		xo->flags |= CRYPTO_DONE;
		xo->status = CRYPTO_SUCCESS;
		if (err ==  1)
			xo->status = CRYPTO_TUNNEL_ESP_AUTH_FAILED;

		dma_unmap_single(mtk->dev, dstAddr, skb->len, DMA_BIDIRECTIONAL);
		xo->proto = nexthdr;
		xo->flags |=  XFRM_ESP_NO_TRAILER;
		if (skb->ip_summed == CHECKSUM_COMPLETE) {
			csumdiff = skb_checksum(skb, len, skb->len - len, 0);
			skb->csum = csum_block_sub(skb->csum, csumdiff, len);
		}
		pskb_trim(skb, len);
		// for inbound continue XFRM (-2 is GRO)
		xfrm_input(skb, IPPROTO_ESP, x->id.spi, -2);
//		handled++;
	}
//	dev_info(ipsec->mtk->dev, "rx done: %d\n", handled);
}

void mtk_ipsec_tx_done(unsigned long data)
{
	struct mtk_device *mtk = (struct mtk_device *)data;
	struct mtk_ipsec_cb *mtk_ipsec_cb;
	struct sk_buff *skb;
	dma_addr_t dAddr;
	u8 nexthdr;
	int err, len;
//	int handled = 0;
	struct ipsec_sa_entry *ipsec;
	struct xfrm_state *x;
	struct xfrm_offload *xo;

	while ((skb = skb_dequeue(&mtk->ring->tx_queue))) {
		mtk_ipsec_cb = (struct mtk_ipsec_cb *)MTK_IPSEC_CB(skb)->cb;
		nexthdr = mtk_ipsec_cb->nexthdr;
		err = mtk_ipsec_cb->err;
		len = mtk_ipsec_cb->len;
		dAddr = mtk_ipsec_cb->dstAddr;
		MTK_IPSEC_CB(skb)->cb = mtk_ipsec_cb->org_cb;
		kfree(mtk_ipsec_cb);

		x = xfrm_input_state(skb);
		ipsec = (struct ipsec_sa_entry *)x->xso.offload_handle;

		xo = xfrm_offload(skb);
		xo->flags |= CRYPTO_DONE;
		xo->status = CRYPTO_SUCCESS;

		dma_unmap_single(mtk->dev, dAddr, len + 20, DMA_BIDIRECTIONAL);

		skb_put(skb, len - (skb->len - 20));
		ip_hdr(skb)->tot_len = htons(skb->len);
		ip_send_check(ip_hdr(skb));

/*
		if (xo && (xo->flags & XFRM_DEV_RESUME)) {
			if (err) {
				XFRM_INC_STATS(xs_net(x),
					LINUX_MIB_XFRMOUTSTATEPROTOERROR);
				kfree_skb(skb);
				return;
			}
*/
			skb_push(skb, skb->data - skb_mac_header(skb));
			secpath_reset(skb);
			xfrm_dev_resume(skb);
//			handled++;
//		}
	}
//	dev_info(ipsec->mtk->dev, "tx done: %d\n", handled);
}

int mtk_ipsec_offload(struct xfrm_state *x, struct sk_buff *skb)
{
	struct mtk_device *mtk;
	struct mtk_ipsec_cb *mtk_ipsec_cb;
	struct xfrm_offload *xo;
	struct eip93_descriptor_s cdesc;
	struct eip93_descriptor_s desc;
	struct ipsec_sa_entry *ipsec;
	struct crypto_aead *aead;
	dma_addr_t saddr;
	struct esp_info esp;
	int err, assoclen, alen, blksize, ivsize;
	int diff;

	ipsec = (struct ipsec_sa_entry *)x->xso.offload_handle;
	desc = ipsec->cdesc;
	mtk = ipsec->mtk;

	mtk_ipsec_cb = kmalloc(sizeof(struct mtk_ipsec_cb), GFP_KERNEL);
	mtk_ipsec_cb->org_cb = MTK_IPSEC_CB(skb)->cb;
	MTK_IPSEC_CB(skb)->cb = (u32)mtk_ipsec_cb;

	xo = xfrm_offload(skb);

	if (x->xso.flags & XFRM_OFFLOAD_INBOUND) {
		if (unlikely(atomic_read(&mtk->ring->free) <= MTK_RING_BUSY)) {
			dev_info(mtk->dev, "RCV packet drop\n");
			xfrm_input(skb, -ENOSPC, x->id.spi, -1);
			return -ENOSPC;
		}
		saddr = dma_map_single(mtk->dev, (void *)skb->data, skb->len,
							DMA_BIDIRECTIONAL);
		cdesc.peCrtlStat.word = desc.peCrtlStat.word;
		cdesc.srcAddr = saddr;
		cdesc.dstAddr = saddr;
		cdesc.peLength.bits.length = skb->len;
	} else {
		if (unlikely(atomic_read(&mtk->ring->free) <= MTK_RING_BUSY)) {
			dev_info(mtk->dev, "XMIT packet drop\n");
			return -ENOSPC;
		}

		aead = x->data;
		alen = crypto_aead_authsize(aead);

		esp.esph = ip_esp_hdr(skb);
		assoclen = sizeof(struct ip_esp_hdr);
		ivsize = crypto_aead_ivsize(aead);

		esp.tfclen = 0;
		/* XXX: Add support for tfc padding here. */

		blksize = ALIGN(crypto_aead_blocksize(aead), 4);
		esp.clen = ALIGN(skb->len + 2 + esp.tfclen, blksize);
		esp.plen = esp.clen - skb->len - esp.tfclen;
		esp.tailen = esp.tfclen + esp.plen + alen;

		diff = (u32)esp.esph - (u32)skb->data;
		if (diff != 20)
			printk("diff %d\n", diff);
		esp.clen = skb->len - assoclen - ivsize - 20;

		saddr = dma_map_single(mtk->dev, (void *)skb->data,
			skb->len + esp.tailen, DMA_BIDIRECTIONAL);
		cdesc.peCrtlStat.bits.hostReady = 1;
		cdesc.peCrtlStat.bits.prngMode = 0;
		cdesc.peCrtlStat.bits.padValue = xo->proto;
		cdesc.peCrtlStat.bits.padCrtlStat = 2; // Pad align 4 as esp4.c
		cdesc.peCrtlStat.bits.hashFinal = 1;
		cdesc.peCrtlStat.bits.peReady = 0;
		cdesc.srcAddr = (u32)esp.esph + assoclen + ivsize;
		cdesc.dstAddr = (u32)esp.esph;
		cdesc.peLength.bits.length = esp.clen;
	}

	cdesc.saAddr = desc.saAddr;
	cdesc.stateAddr = desc.stateAddr;
	cdesc.arc4Addr = (uintptr_t)skb;
	cdesc.userId = desc.userId;
	cdesc.peLength.bits.peReady = 0;
	cdesc.peLength.bits.byPass = 0;
	cdesc.peLength.bits.hostReady = 1;
again:
	err = mtk_put_descriptor(mtk, &cdesc);
	/* Should not happen 32 descriptors margin */
	if (err) {
		udelay(1000);
		goto again;
	}

	writel(1, mtk->base + EIP93_REG_PE_CD_COUNT);

	return -EINPROGRESS;
}

static struct sk_buff *mtk_esp4_gro_receive(struct list_head *head,
					struct sk_buff *skb)
{
	int offset = skb_gro_offset(skb);
	struct xfrm_offload *xo;
	struct xfrm_state *x;
	__be32 seq;
	__be32 spi;
	int err;

	if (!pskb_pull(skb, offset))
		return NULL;

	if ((err = xfrm_parse_spi(skb, IPPROTO_ESP, &spi, &seq)) != 0)
		goto out;

	xo = xfrm_offload(skb);
	if (!xo || !(xo->flags & CRYPTO_DONE)) {
		struct sec_path *sp = secpath_set(skb);

		if (!sp)
			goto out;

		if (sp->len == XFRM_MAX_DEPTH)
			goto out_reset;

		x = xfrm_state_lookup(dev_net(skb->dev), skb->mark,
				      (xfrm_address_t *)&ip_hdr(skb)->daddr,
				      spi, IPPROTO_ESP, AF_INET);
		if (!x)
			goto out_reset;

		skb->mark = xfrm_smark_get(skb->mark, x);

		sp->xvec[sp->len++] = x;
		sp->olen++;

		xo = xfrm_offload(skb);
		if (!xo)
			goto out_reset;
	}

	xo->flags |= XFRM_GRO;

	XFRM_TUNNEL_SKB_CB(skb)->tunnel.ip4 = NULL;
	XFRM_SPI_SKB_CB(skb)->family = AF_INET;
	XFRM_SPI_SKB_CB(skb)->daddroff = offsetof(struct iphdr, daddr);
	XFRM_SPI_SKB_CB(skb)->seq = seq;

	/* We don't need to handle errors from xfrm_input, it does all
	 * the error handling and frees the resources on error. */
	if (xo && x->xso.offload_handle)
		err = mtk_ipsec_offload(x, skb);
	else
		xfrm_input(skb, IPPROTO_ESP, spi, -2);

	return ERR_PTR(-EINPROGRESS);
out_reset:
	secpath_reset(skb);
out:
	skb_push(skb, offset);
	NAPI_GRO_CB(skb)->same_flow = 0;
	NAPI_GRO_CB(skb)->flush = 1;

	return NULL;
}


static struct sk_buff *xfrm4_tunnel_gso_segment(struct xfrm_state *x,
						struct sk_buff *skb,
						netdev_features_t features)
{
	__skb_push(skb, skb->mac_len);
	return skb_mac_gso_segment(skb, features);
}

static struct sk_buff *xfrm4_transport_gso_segment(struct xfrm_state *x,
						   struct sk_buff *skb,
						   netdev_features_t features)
{
	const struct net_offload *ops;
	struct sk_buff *segs = ERR_PTR(-EINVAL);
	struct xfrm_offload *xo = xfrm_offload(skb);

	skb->transport_header += x->props.header_len;
	ops = rcu_dereference(inet_offloads[xo->proto]);
	if (likely(ops && ops->callbacks.gso_segment))
		segs = ops->callbacks.gso_segment(skb, features);

	return segs;
}

static struct sk_buff *xfrm4_beet_gso_segment(struct xfrm_state *x,
					      struct sk_buff *skb,
					      netdev_features_t features)
{
	struct xfrm_offload *xo = xfrm_offload(skb);
	struct sk_buff *segs = ERR_PTR(-EINVAL);
	const struct net_offload *ops;
	u8 proto = xo->proto;

	skb->transport_header += x->props.header_len;

	if (x->sel.family != AF_INET6) {
		if (proto == IPPROTO_BEETPH) {
			struct ip_beet_phdr *ph =
				(struct ip_beet_phdr *)skb->data;

			skb->transport_header += ph->hdrlen * 8;
			proto = ph->nexthdr;
		} else {
			skb->transport_header -= IPV4_BEET_PHMAXLEN;
		}
	} else {
		__be16 frag;

		skb->transport_header +=
			ipv6_skip_exthdr(skb, 0, &proto, &frag);
		if (proto == IPPROTO_TCP)
			skb_shinfo(skb)->gso_type |= SKB_GSO_TCPV4;
	}

	__skb_pull(skb, skb_transport_offset(skb));
	ops = rcu_dereference(inet_offloads[proto]);
	if (likely(ops && ops->callbacks.gso_segment))
		segs = ops->callbacks.gso_segment(skb, features);

	return segs;
}

static struct sk_buff *xfrm4_outer_mode_gso_segment(struct xfrm_state *x,
						    struct sk_buff *skb,
						    netdev_features_t features)
{
	switch (x->outer_mode.encap) {
	case XFRM_MODE_TUNNEL:
		return xfrm4_tunnel_gso_segment(x, skb, features);
	case XFRM_MODE_TRANSPORT:
		return xfrm4_transport_gso_segment(x, skb, features);
	case XFRM_MODE_BEET:
		return xfrm4_beet_gso_segment(x, skb, features);
	}

	return ERR_PTR(-EOPNOTSUPP);
}

static struct sk_buff *mtk_esp4_gso_segment(struct sk_buff *skb,
				        netdev_features_t features)
{
	struct xfrm_state *x;
	struct ip_esp_hdr *esph;
	struct crypto_aead *aead;
	netdev_features_t esp_features = features;
	struct xfrm_offload *xo = xfrm_offload(skb);
	struct sec_path *sp;

	printk("esp-gso-segment\n");

	if (!xo)
		return ERR_PTR(-EINVAL);

	if (!(skb_shinfo(skb)->gso_type & SKB_GSO_ESP))
		return ERR_PTR(-EINVAL);

	sp = skb_sec_path(skb);
	x = sp->xvec[sp->len - 1];
	aead = x->data;
	esph = ip_esp_hdr(skb);

	if (esph->spi != x->id.spi)
		return ERR_PTR(-EINVAL);

	if (!pskb_may_pull(skb, sizeof(*esph) + crypto_aead_ivsize(aead)))
		return ERR_PTR(-EINVAL);

	__skb_pull(skb, sizeof(*esph) + crypto_aead_ivsize(aead));

	skb->encap_hdr_csum = 1;

	if ((!(skb->dev->gso_partial_features & NETIF_F_HW_ESP) &&
	     !(features & NETIF_F_HW_ESP)) || x->xso.dev != skb->dev)
		esp_features = features & ~(NETIF_F_SG | NETIF_F_CSUM_MASK |
					    NETIF_F_SCTP_CRC);
	else if (!(features & NETIF_F_HW_ESP_TX_CSUM) &&
		 !(skb->dev->gso_partial_features & NETIF_F_HW_ESP_TX_CSUM))
		esp_features = features & ~(NETIF_F_CSUM_MASK |
					    NETIF_F_SCTP_CRC);

	xo->flags |= XFRM_GSO_SEGMENT;

	return xfrm4_outer_mode_gso_segment(x, skb, esp_features);
}

static int mtk_esp_input_tail(struct xfrm_state *x, struct sk_buff *skb)
{
	struct crypto_aead *aead = x->data;
	struct xfrm_offload *xo = xfrm_offload(skb);

	if (!pskb_may_pull(skb, sizeof(struct ip_esp_hdr) + crypto_aead_ivsize(aead)))
		return -EINVAL;

	if (!(xo->flags & CRYPTO_DONE))
		skb->ip_summed = CHECKSUM_NONE;

	return esp_input_done2(skb, 0);
}

static int mtk_esp_xmit(struct xfrm_state *x, struct sk_buff *skb,
			netdev_features_t features)
{
	int err;
	int org_len;
	int alen;
	int blksize;
	struct ip_esp_hdr *esph;
	struct crypto_aead *aead;
	struct esp_info esp;
	struct xfrm_offload *xo = xfrm_offload(skb);
	bool hw_offload = true;
	__u32 seq;

	esp.inplace = true;

	if (!xo)
		return -EINVAL;

	if ((!(features & NETIF_F_HW_ESP) &&
	     !(skb->dev->gso_partial_features & NETIF_F_HW_ESP)) ||
	    x->xso.dev != skb->dev) {
		xo->flags |= CRYPTO_FALLBACK;
		hw_offload = false;
	}

	esp.proto = xo->proto;

	/* skb is pure payload to encrypt */

	aead = x->data;
	alen = crypto_aead_authsize(aead);

	esp.tfclen = 0;
	/* XXX: Add support for tfc padding here. */

	blksize = ALIGN(crypto_aead_blocksize(aead), 4);
	esp.clen = ALIGN(skb->len + 2 + esp.tfclen, blksize);
	esp.plen = esp.clen - skb->len - esp.tfclen;
	esp.tailen = esp.tfclen + esp.plen + alen;

	esp.esph = ip_esp_hdr(skb);

	if ((!hw_offload) || (esp.tailen > skb_tailroom(skb))) {
		org_len = skb->len;
		esp.nfrags = esp_output_head(x, skb, &esp);
		if (esp.nfrags < 0)
			return esp.nfrags;

		err = skb_linearize(skb);
		if (err)
			return err;

		if (hw_offload)
			skb_put(skb, org_len - skb->len);
	}

	seq = xo->seq.low;

	esph = esp.esph;
	esph->spi = x->id.spi;

	skb_push(skb, -skb_network_offset(skb));

	if (xo->flags & XFRM_GSO_SEGMENT) {
		esph->seq_no = htonl(seq);

		if (!skb_is_gso(skb)) {
			printk("!skb_is_gso\n");
			xo->seq.low++;
		} else {
			printk("Gso segs: %d\n", skb_shinfo(skb)->gso_segs);
			xo->seq.low += skb_shinfo(skb)->gso_segs;
		}
	}

	esp.seqno = cpu_to_be64(seq + ((u64)xo->seq.hi << 32));

	ip_hdr(skb)->tot_len = htons(skb->len);
	ip_send_check(ip_hdr(skb));

	if (hw_offload) {
		if (!skb_ext_add(skb, SKB_EXT_SEC_PATH))
			return -ENOMEM;

		xo = xfrm_offload(skb);
		if (!xo)
			return -EINVAL;

		xo->flags |= XFRM_XMIT;
		err = mtk_ipsec_offload(x, skb);

		return err;
	}

	err = esp_output_tail(x, skb, &esp);
	if (err)
		return err;

	secpath_reset(skb);

	return 0;
}

static void mtk_esp4_gso_encap(struct xfrm_state *x, struct sk_buff *skb)
{
	struct ip_esp_hdr *esph;
	struct iphdr *iph = ip_hdr(skb);
	struct xfrm_offload *xo = xfrm_offload(skb);
	int proto = iph->protocol;

	skb_push(skb, -skb_network_offset(skb));
	esph = ip_esp_hdr(skb);
	*skb_mac_header(skb) = IPPROTO_ESP;

	esph->spi = x->id.spi;
	esph->seq_no = htonl(XFRM_SKB_CB(skb)->seq.output.low);

	xo->proto = proto;
}

static int ipsec_dev_event(struct notifier_block *this, unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);

	switch (event) {
	case NETDEV_REGISTER:
		return mtk_add_xfrmops(dev);

	case NETDEV_FEAT_CHANGE:
		return mtk_add_xfrmops(dev);

	case NETDEV_DOWN:
	case NETDEV_UNREGISTER:
		return mtk_del_xfrmops(dev);
	}
	return NOTIFY_DONE;
}

static struct notifier_block ipsec_dev_notifier = {
	.notifier_call	= ipsec_dev_event,
};

static const struct net_offload mtk_esp4_offload = {
	.callbacks = {
		.gro_receive = mtk_esp4_gro_receive,
		.gso_segment = mtk_esp4_gso_segment,
	},
};

static struct xfrm_type_offload mtk_esp_offload = {
	.description	= "EIP93 ESP OFFLOAD",
	.owner		= THIS_MODULE,
	.proto	     	= IPPROTO_ESP,
	.input_tail	= mtk_esp_input_tail,
	.xmit		= mtk_esp_xmit,
	.encap		= mtk_esp4_gso_encap,
};

//static int __init mtk_offload_register(void)
int mtk_offload_register(void)
{
	xfrm_register_type_offload(&mtk_esp_offload, AF_INET);

	inet_add_offload(&mtk_esp4_offload, IPPROTO_ESP);

	return register_netdevice_notifier(&ipsec_dev_notifier);
}

//static
void mtk_offload_deregister(void)
{
	xfrm_unregister_type_offload(&mtk_esp_offload, AF_INET);

	inet_del_offload(&mtk_esp4_offload, IPPROTO_ESP);
}

//module_init(mtk_offload_register);
//module_exit(mtk_offload_deregister);
//MODULE_AUTHOR("Richard van Schagen <vschagen@icloud.com>");
//MODULE_ALIAS("platform:" KBUILD_MODNAME);
//MODULE_DESCRIPTION("Mediatek EIP-93 ESP Offload");
//MODULE_LICENSE("GPL v2");
