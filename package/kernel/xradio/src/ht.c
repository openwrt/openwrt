#include <net/mac80211.h>

#include "xradio.h"
#include "sta.h"

#define AG_RATE_INDEX  6     //11a/g rate for important short frames in 5G.

#ifdef AP_HT_COMPAT_FIX
#define AP_COMPAT_THRESHOLD  2000
#define AP_COMPAT_MIN_CNT    200
u8 ap_compat_bssid[ETH_ALEN] = {0};
int xradio_apcompat_detect(struct xradio_vif *priv, u8 rx_rate)
{
	if (rx_rate < AG_RATE_INDEX) {
		priv->ht_compat_cnt++;
		txrx_printk(XRADIO_DBG_MSG,"%s:rate=%d.\n", __func__, rx_rate);
	} else {
		priv->ht_compat_det |= 1;
		priv->ht_compat_cnt = 0;
		txrx_printk(XRADIO_DBG_NIY,"%s:HT compat detect\n", __func__);
		return 0;
	}

	/* Enhance compatibility with some illegal APs.*/
	if (priv->ht_compat_cnt  > AP_COMPAT_THRESHOLD ||
		(priv->ht_compat_cnt > AP_COMPAT_MIN_CNT &&
		 priv->bssid[0] == 0xC8 &&
		 priv->bssid[1] == 0x3A &&
		 priv->bssid[2] == 0x35)) {
		struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);
		memcpy(ap_compat_bssid, priv->bssid, ETH_ALEN);
		wms_send_disassoc_to_self(hw_priv, priv);
		txrx_printk(XRADIO_DBG_WARN, "%s:SSID=%s, BSSID=" \
		            "%02x:%02x:%02x:%02x:%02x:%02x\n", __func__, priv->ssid,
		            ap_compat_bssid[0], ap_compat_bssid[1],
		            ap_compat_bssid[2], ap_compat_bssid[3],
		            ap_compat_bssid[4], ap_compat_bssid[5]);
		return 1;
	}
	return 0;
}

void xradio_remove_ht_ie(struct xradio_vif *priv, struct sk_buff *skb)
{
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)skb->data;
	u8 *ies        = NULL;
	size_t ies_len = 0;
	u8 *ht_ie      = NULL;

	if (!mgmt || memcmp(ap_compat_bssid, mgmt->bssid, ETH_ALEN))
		return;

	if (ieee80211_is_probe_resp(mgmt->frame_control))
		ies = mgmt->u.probe_resp.variable;
	else if (ieee80211_is_beacon(mgmt->frame_control))
		ies = mgmt->u.beacon.variable;
	else if (ieee80211_is_assoc_resp(mgmt->frame_control))
		ies = mgmt->u.assoc_resp.variable;
	else if (ieee80211_is_assoc_req(mgmt->frame_control))
		ies = mgmt->u.assoc_req.variable;
	else
		return;

	ies_len = skb->len - (ies - (u8 *)(skb->data));
	ht_ie   = (u8 *)xradio_get_ie(ies, ies_len, WLAN_EID_HT_CAPABILITY);
	if (ht_ie) {
		u8 ht_len   = *(ht_ie + 1) + 2;
		u8 move_len = (ies + ies_len) - (ht_ie + ht_len);
		memmove(ht_ie, (ht_ie + ht_len), move_len);
		skb_trim(skb, skb->len - ht_len);
		ies_len = skb->len - (ies - (u8 *)(skb->data));
		ht_ie = (u8 *)xradio_get_ie(ies, ies_len, WLAN_EID_HT_OPERATION);
		if (ht_ie) {
			ht_len   = *(ht_ie + 1) + 2;
			move_len = (ies + ies_len) - (ht_ie + ht_len);
			memmove(ht_ie, (ht_ie + ht_len), move_len);
			skb_trim(skb, skb->len - ht_len);
		}
	}
	txrx_printk(XRADIO_DBG_WARN, "%s: BSSID=%02x:%02x:%02x:%02x:%02x:%02x\n",
	            __func__,
	            mgmt->bssid[0], mgmt->bssid[1],
	            mgmt->bssid[2], mgmt->bssid[3],
	            mgmt->bssid[4], mgmt->bssid[5]);
}
#endif //AP_HT_COMPAT_FIX
