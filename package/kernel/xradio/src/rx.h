#ifndef XRADIO_RX_H
#define XRADIO_RX_H

void xradio_rx_cb(struct xradio_vif *priv,
		  struct wsm_rx *arg,
		  struct sk_buff **skb_p);

#endif
