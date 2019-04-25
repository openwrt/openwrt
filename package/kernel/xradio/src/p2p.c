#include "xradio.h"

#ifdef TES_P2P_0002_ROC_RESTART
///w, TES_P2P_0002 WorkAround:
///w, P2P GO Neg Process and P2P FIND may be collision.
///w, When P2P Device is waiting for GO NEG CFM in 30ms,
///w, P2P FIND may end with p2p listen, and then goes to p2p search.
///w, Then xradio scan will occupy phy on other channel in 3+ seconds.
///w, P2P Device will not be able to receive the GO NEG CFM.
///w, We extend the roc period to remaind phy to receive GO NEG CFM as WorkAround.

s32  TES_P2P_0002_roc_dur;
s32  TES_P2P_0002_roc_sec;
s32  TES_P2P_0002_roc_usec;
u32  TES_P2P_0002_packet_id;
u32  TES_P2P_0002_state =  TES_P2P_0002_STATE_IDLE;

void xradio_frame_monitor(struct xradio_common *hw_priv, struct sk_buff *skb, bool tx) {
	struct ieee80211_hdr *frame = (struct ieee80211_hdr *)skb->data;
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)skb->data;

	u8 *action        = (u8*)&mgmt->u.action.category;
	u8 *category_code = &(action[0]);
	u8 *action_code   = &(action[1]);
	u8 *oui			  = &(action[2]);
	u8 *subtype       = &(action[5]);
	u8 *oui_subtype   = &(action[6]);


	if(ieee80211_is_action(frame->frame_control)) {
		if( *category_code == WLAN_CATEGORY_PUBLIC) {
			if (*action_code == 0x09) {
				if((oui[0] == 0x50) && (oui[1] == 0x6F) &&
				   (oui[2] == 0x9A) && (*subtype == 0x09)) {
					if ( *oui_subtype == 0x01 ) {  ///w, GO Negotiation Response
						if((TES_P2P_0002_state == TES_P2P_0002_STATE_IDLE) &&
						   (tx == true)) { ///w, p2p atturbute:status,id=0
							u8 *go_neg_resp_res = &(action[17]);
							if (*go_neg_resp_res == 0x0) {
								TES_P2P_0002_state = TES_P2P_0002_STATE_SEND_RESP;
								txrx_printk(XRADIO_DBG_WARN, "[ROC_RESTART_STATE_SEND_RESP]\n");
							}
						}
					} else if ( *oui_subtype == 0x02 ) { ///w, GO Negotiation Confirmation
						if( tx == false ) {
							TES_P2P_0002_state = TES_P2P_0002_STATE_IDLE;
							txrx_printk(XRADIO_DBG_WARN, "[ROC_RESTART_STATE_IDLE]"
							            "[GO Negotiation Confirmation]\n");
						}
					} else if ( *oui_subtype == 0x08 ) { ///w, Provision Discovery Response
						if(tx == false) {
							TES_P2P_0002_state = TES_P2P_0002_STATE_IDLE;
							txrx_printk(XRADIO_DBG_WARN, "[ROC_RESTART_STATE_IDLE]"
							            "[Provision Discovery Response]\n");
						}
					}
				}
			}
		}
	}
}
#endif
