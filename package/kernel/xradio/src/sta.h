/*
 * sta interfaces for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef STA_H_INCLUDED
#define STA_H_INCLUDED


#ifdef XRADIO_USE_LONG_KEEP_ALIVE_PERIOD
#define XRADIO_KEEP_ALIVE_PERIOD         (28)
#else
/*For Samsung, it is defined as 4*/
#define XRADIO_KEEP_ALIVE_PERIOD         (4)
#endif

#ifdef XRADIO_USE_LONG_DTIM_PERIOD
#define XRADIO_BSS_LOSS_THOLD_DEF  30
#define XRADIO_LINK_LOSS_THOLD_DEF 50
#else
#define XRADIO_BSS_LOSS_THOLD_DEF  20
#define XRADIO_LINK_LOSS_THOLD_DEF 40
#endif

/* ******************************************************************** */
/* mac80211 API								*/

int xradio_start(struct ieee80211_hw *dev);
void xradio_stop(struct ieee80211_hw *dev);
int xradio_add_interface(struct ieee80211_hw *dev, struct ieee80211_vif *vif);
void xradio_remove_interface(struct ieee80211_hw *dev, struct ieee80211_vif *vif);
int xradio_change_interface(struct ieee80211_hw *dev,
                            struct ieee80211_vif *vif,
                            enum nl80211_iftype new_type,
                            bool p2p);
int xradio_config(struct ieee80211_hw *dev, u32 changed);
int xradio_change_interface(struct ieee80211_hw *dev,
                            struct ieee80211_vif *vif,
                            enum nl80211_iftype new_type,
                            bool p2p);
void xradio_configure_filter(struct ieee80211_hw *dev,
                             unsigned int changed_flags,
                             unsigned int *total_flags,
                             u64 multicast);
int xradio_conf_tx(struct ieee80211_hw *dev, struct ieee80211_vif *vif,
                   u16 queue, const struct ieee80211_tx_queue_params *params);
int xradio_get_stats(struct ieee80211_hw *dev,
                     struct ieee80211_low_level_stats *stats);
/* Not more a part of interface?
int xradio_get_tx_stats(struct ieee80211_hw *dev,
			struct ieee80211_tx_queue_stats *stats);
*/
int xradio_set_rts_threshold(struct ieee80211_hw *hw, u32 value);

void xradio_flush(struct ieee80211_hw *hw, struct ieee80211_vif *vif, u32 queues, bool drop);


int xradio_remain_on_channel(struct ieee80211_hw *hw,
			     struct ieee80211_vif *vif,
                             struct ieee80211_channel *chan,
                             int duration, enum ieee80211_roc_type type);
int xradio_cancel_remain_on_channel(struct ieee80211_hw *hw);
int xradio_set_arpreply(struct ieee80211_hw *hw, struct ieee80211_vif *vif);
u64 xradio_prepare_multicast(struct ieee80211_hw *hw,
                             struct netdev_hw_addr_list *mc_list);
int xradio_set_pm(struct xradio_vif *priv, const struct wsm_set_pm *arg);
void xradio_set_data_filter(struct ieee80211_hw *hw,
                            struct ieee80211_vif *vif,
                            void *data,
                            int len);

/* ******************************************************************** */
/* WSM callbacks							*/

/* void xradio_set_pm_complete_cb(struct xradio_common *hw_priv,
	struct wsm_set_pm_complete *arg); */
void xradio_channel_switch_cb(struct xradio_common *hw_priv);

/* ******************************************************************** */
/* WSM events								*/

void xradio_free_event_queue(struct xradio_common *hw_priv);
void xradio_event_handler(struct work_struct *work);
void xradio_bss_loss_work(struct work_struct *work);
void xradio_connection_loss_work(struct work_struct *work);
void xradio_keep_alive_work(struct work_struct *work);
void xradio_tx_failure_work(struct work_struct *work);

/* ******************************************************************** */
/* Internal API								*/

int xradio_setup_mac(struct xradio_common *hw_priv);
void xradio_join_work(struct work_struct *work);
void xradio_join_timeout(struct work_struct *work);
void xradio_unjoin_work(struct work_struct *work);
void xradio_offchannel_work(struct work_struct *work);
void xradio_wep_key_work(struct work_struct *work);
void xradio_update_filtering(struct xradio_vif *priv);
void xradio_update_filtering_work(struct work_struct *work);
int __xradio_flush(struct xradio_common *hw_priv, bool drop, int if_id);
void xradio_set_beacon_wakeup_period_work(struct work_struct *work);
int xradio_enable_listening(struct xradio_vif *priv, struct ieee80211_channel *chan);
int xradio_disable_listening(struct xradio_vif *priv);
int xradio_set_uapsd_param(struct xradio_vif *priv, const struct wsm_edca_params *arg);
void xradio_ba_work(struct work_struct *work);
void xradio_ba_timer(struct timer_list *t);
const u8 *xradio_get_ie(u8 *start, size_t len, u8 ie);
int xradio_vif_setup(struct xradio_vif *priv);
int xradio_setup_mac_pvif(struct xradio_vif *priv);
void xradio_iterate_vifs(void *data, u8 *mac, struct ieee80211_vif *vif);
void xradio_rem_chan_timeout(struct work_struct *work);
int xradio_set_macaddrfilter(struct xradio_common *hw_priv, struct xradio_vif *priv, u8 *data);
#ifdef ROAM_OFFLOAD
int xradio_testmode_event(struct wiphy *wiphy, const u32 msg_id,
                          const void *data, int len, gfp_t gfp);
#endif /*ROAM_OFFLOAD*/
#endif
