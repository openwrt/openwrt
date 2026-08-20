/* SPDX-License-Identifier: GPL-2.0 */
/*
 * econet-xpon -> econet-eth runtime seam.
 * Minimal extern declarations for the GPL-exported PON datapath symbols in
 * econet-eth.ko (see package/kernel/econet-eth patch 100-xpon-bench-spike).
 * econet-xpon DEPENDS +kmod-econet-eth; these resolve at module load.
 *
 * Opaque/forward types only (no layout) so this header stays decoupled from
 * econet-eth's internal headers. The TX path that needs `union desc_msg`
 * layout will include econet-eth's qdma_desc.h when it is wired.
 */
#ifndef _ECONET_XPON_ETH_SEAM_H
#define _ECONET_XPON_ETH_SEAM_H

struct en75_qdma;
struct sk_buff;

/* Real econet-eth TX descriptor layout (union desc_msg + set_etx_* accessors +
 * ETX_FPORT_GDM2) so the TX path can build a valid message for en75_qdma_xmit().
 * Copied verbatim from econet-eth's qdma_desc.h (self-contained). */
#include "eth_qdma_desc.h"

/* Must match econet_eth.h exactly (same values; separate TU => no ODR issue). */
enum en75_irq_purpose_type {
	IPS_INVAL = 0,
	IPS_DONE,
	IPS_LOW_DSCP,
	IPS_NO_DSCP,
	IPS_OVERFLOW,
	IPS_ERR_COHERENT,
	IPS_GPON_INT,
	IPS_EPON_INT,
	IPS_XPON_INT,
};

extern struct en75_qdma *en75_xpon_pon_qdma(void);
extern int en75_qdma_use(struct en75_qdma *qdma);
extern int en75_qdma_unuse(struct en75_qdma *qdma);
extern int en75_qdma_xmit(struct en75_qdma *qdma, struct sk_buff *skb,
			  union desc_msg *msg, int qid);
extern int en75_qdma_register_pon(struct en75_qdma *qdma,
				  void (*cb)(void *, enum en75_irq_purpose_type),
				  void *arg);
extern int en75_qdma_register_pon_rx(struct en75_qdma *qdma,
				     void (*cb)(void *, struct sk_buff *, u8, u16),
				     void *arg);
extern int en75_qdma_set_loopback(struct en75_qdma *qdma, bool on);

#endif /* _ECONET_XPON_ETH_SEAM_H */
