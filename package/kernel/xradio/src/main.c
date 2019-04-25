/*
 * Main code of XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#include <linux/firmware.h>
#include <net/cfg80211.h>
#include <linux/of_net.h>
#include <linux/mmc/sdio_func.h>

#include "xradio.h"
#include "fwio.h"
#include "hwio.h"
#include "bh.h"
#include "sta.h"
#include "ap.h"
#include "keys.h"
#include "scan.h"
#include "pm.h"
#include "sdio.h"

/* TODO: use rates and channels from the device */
#define RATETAB_ENT(_rate, _rateid, _flags)		\
	{						\
		.bitrate  = (_rate),    \
		.hw_value = (_rateid),  \
		.flags    = (_flags),   \
	}

static struct ieee80211_rate xradio_rates[] = {
	RATETAB_ENT(10,  0,   0),
	RATETAB_ENT(20,  1,   0),
	RATETAB_ENT(55,  2,   0),
	RATETAB_ENT(110, 3,   0),
	RATETAB_ENT(60,  6,  0),
	RATETAB_ENT(90,  7,  0),
	RATETAB_ENT(120, 8,  0),
	RATETAB_ENT(180, 9,  0),
	RATETAB_ENT(240, 10, 0),
	RATETAB_ENT(360, 11, 0),
	RATETAB_ENT(480, 12, 0),
	RATETAB_ENT(540, 13, 0),
};

static struct ieee80211_rate xradio_mcs_rates[] = {
	RATETAB_ENT(65,  14, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(130, 15, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(195, 16, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(260, 17, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(390, 18, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(520, 19, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(585, 20, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(650, 21, IEEE80211_TX_RC_MCS),
};

#define xradio_g_rates      (xradio_rates + 0)
#define xradio_a_rates      (xradio_rates + 4)
#define xradio_n_rates      (xradio_mcs_rates)

#define xradio_g_rates_size (ARRAY_SIZE(xradio_rates))
#define xradio_a_rates_size (ARRAY_SIZE(xradio_rates) - 4)
#define xradio_n_rates_size (ARRAY_SIZE(xradio_mcs_rates))

#define CHAN2G(_channel, _freq, _flags) {   \
	.band             = NL80211_BAND_2GHZ,  \
	.center_freq      = (_freq),              \
	.hw_value         = (_channel),           \
	.flags            = (_flags),             \
	.max_antenna_gain = 0,                    \
	.max_power        = 30,                   \
}

#define CHAN5G(_channel, _flags) {   \
	.band             = NL80211_BAND_5GHZ,     \
	.center_freq      = 5000 + (5 * (_channel)), \
	.hw_value         = (_channel),              \
	.flags            = (_flags),                \
	.max_antenna_gain = 0,                       \
	.max_power        = 30,                      \
}

static struct ieee80211_channel xradio_2ghz_chantable[] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
	CHAN2G(12, 2467, 0),
	CHAN2G(13, 2472, 0),
	CHAN2G(14, 2484, 0),
};

static struct ieee80211_supported_band xradio_band_2ghz = {
	.channels = xradio_2ghz_chantable,
	.n_channels = ARRAY_SIZE(xradio_2ghz_chantable),
	.bitrates = xradio_g_rates,
	.n_bitrates = xradio_g_rates_size,
	.ht_cap = {
		.cap = IEEE80211_HT_CAP_GRN_FLD |
		       (1 << IEEE80211_HT_CAP_RX_STBC_SHIFT),
		.ht_supported  = 1,
		.ampdu_factor  = IEEE80211_HT_MAX_AMPDU_32K,
		.ampdu_density = IEEE80211_HT_MPDU_DENSITY_NONE,
		.mcs = {
			.rx_mask[0] = 0xFF,
			.rx_highest = __cpu_to_le16(0x41),
			.tx_params  = IEEE80211_HT_MCS_TX_DEFINED,
		},
	},
};

static const unsigned long xradio_ttl[] = {
	1 * HZ,	/* VO */
	2 * HZ,	/* VI */
	5 * HZ, /* BE */
	10 * HZ	/* BK */
};

static const struct ieee80211_ops xradio_ops = {
	.start             = xradio_start,
	.stop              = xradio_stop,
	.add_interface     = xradio_add_interface,
	.remove_interface  = xradio_remove_interface,
	.change_interface  = xradio_change_interface,
	.tx                = xradio_tx,
	.hw_scan           = xradio_hw_scan,
#ifdef ROAM_OFFLOAD
	.sched_scan_start  = xradio_hw_sched_scan_start,
	.sched_scan_stop   = xradio_hw_sched_scan_stop,
#endif /*ROAM_OFFLOAD*/
	.set_tim           = xradio_set_tim,
	.sta_notify        = xradio_sta_notify,
	.sta_add           = xradio_sta_add,
	.sta_remove        = xradio_sta_remove,
	.set_key           = xradio_set_key,
	.set_rts_threshold = xradio_set_rts_threshold,
	.config            = xradio_config,
	.bss_info_changed  = xradio_bss_info_changed,
	.prepare_multicast = xradio_prepare_multicast,
	.configure_filter  = xradio_configure_filter,
	.conf_tx           = xradio_conf_tx,
	.get_stats         = xradio_get_stats,
	.ampdu_action      = xradio_ampdu_action,
	.flush             = xradio_flush,
#ifdef CONFIG_PM
	.suspend           = xradio_wow_suspend,
	.resume            = xradio_wow_resume,
#endif /* CONFIG_PM */
	/* Intentionally not offloaded:					*/
	/*.channel_switch	 = xradio_channel_switch,		*/
	.remain_on_channel = xradio_remain_on_channel,
	.cancel_remain_on_channel = xradio_cancel_remain_on_channel,
};


/*************************************** functions ***************************************/

static void xradio_set_ifce_comb(struct xradio_common *hw_priv,
				 struct ieee80211_hw *hw)
{
	hw_priv->if_limits1[0].max = 1;

	hw_priv->if_limits1[0].types = BIT(NL80211_IFTYPE_STATION);
	hw_priv->if_limits1[1].max = 1;
	hw_priv->if_limits1[1].types = BIT(NL80211_IFTYPE_AP);

	hw_priv->if_limits2[0].max = 2;
	hw_priv->if_limits2[0].types = BIT(NL80211_IFTYPE_STATION);

	hw_priv->if_limits3[0].max = 1;

	hw_priv->if_limits3[0].types = BIT(NL80211_IFTYPE_STATION);
	hw_priv->if_limits3[1].max = 1;
	hw_priv->if_limits3[1].types = BIT(NL80211_IFTYPE_P2P_CLIENT) |
				      BIT(NL80211_IFTYPE_P2P_GO);

	/* TODO:COMBO: mac80211 doesn't yet support more than 1
	 * different channel */
	hw_priv->if_combs[0].num_different_channels = 1;
	hw_priv->if_combs[0].max_interfaces = 2;
	hw_priv->if_combs[0].limits = hw_priv->if_limits1;
	hw_priv->if_combs[0].n_limits = 2;

	hw_priv->if_combs[1].num_different_channels = 1;

	hw_priv->if_combs[1].max_interfaces = 2;
	hw_priv->if_combs[1].limits = hw_priv->if_limits2;
	hw_priv->if_combs[1].n_limits = 1;

	hw_priv->if_combs[2].num_different_channels = 1;
	hw_priv->if_combs[2].max_interfaces = 2;
	hw_priv->if_combs[2].limits = hw_priv->if_limits3;
	hw_priv->if_combs[2].n_limits = 2;

	hw->wiphy->iface_combinations = &hw_priv->if_combs[0];
	hw->wiphy->n_iface_combinations = 3;
}

struct ieee80211_hw *xradio_init_common(size_t hw_priv_data_len)
{
	int i;
	struct ieee80211_hw *hw;
	struct xradio_common *hw_priv;
	struct ieee80211_supported_band *sband;
	int band;

	/* Alloc ieee_802.11 hw and xradio_common struct. */
	hw = ieee80211_alloc_hw(hw_priv_data_len, &xradio_ops);
	if (!hw)
		return NULL;
	hw_priv = hw->priv;
	memset(hw_priv, 0, sizeof(*hw_priv));

	/* Initialize members of hw_priv. */
	hw_priv->hw = hw;
	hw_priv->if_id_slot = 0;
	hw_priv->roc_if_id = -1;
	atomic_set(&hw_priv->num_vifs, 0);
	/* initial rates and channels TODO: fetch from FW */
	hw_priv->rates = xradio_rates;    
	hw_priv->mcs_rates = xradio_n_rates;
#ifdef ROAM_OFFLOAD
	hw_priv->auto_scanning = 0;
	hw_priv->frame_rcvd = 0;
	hw_priv->num_scanchannels = 0;
	hw_priv->num_2g_channels = 0;
	hw_priv->num_5g_channels = 0;
#endif /*ROAM_OFFLOAD*/
#ifdef AP_AGGREGATE_FW_FIX
	/* Enable block ACK for 4 TID (BE,VI,VI,VO). */
	hw_priv->ba_tid_mask = 0xB1;  /*due to HW limitations*/
#else
	/* Enable block ACK for every TID but voice. */
	hw_priv->ba_tid_mask = 0x3F;
#endif
	hw_priv->noise = -94;
	/* hw_priv->beacon_req_id = cpu_to_le32(0); */

	/* Initialize members of ieee80211_hw, it works in UMAC. */
	hw->sta_data_size = sizeof(struct xradio_sta_priv);
	hw->vif_data_size = sizeof(struct xradio_vif);

	ieee80211_hw_set(hw, SIGNAL_DBM);
	ieee80211_hw_set(hw, SUPPORTS_PS);
	ieee80211_hw_set(hw, SUPPORTS_DYNAMIC_PS);
	ieee80211_hw_set(hw, REPORTS_TX_ACK_STATUS);
	ieee80211_hw_set(hw, CONNECTION_MONITOR);

/*	hw->flags = IEEE80211_HW_SIGNAL_DBM            |
	            IEEE80211_HW_SUPPORTS_PS           |
	            IEEE80211_HW_SUPPORTS_DYNAMIC_PS   |
	            IEEE80211_HW_REPORTS_TX_ACK_STATUS |
	            IEEE80211_HW_CONNECTION_MONITOR;*/
	            //IEEE80211_HW_SUPPORTS_CQM_RSSI     |
	            /* Aggregation is fully controlled by firmware.
	             * Do not need any support from the mac80211 stack */
	            /* IEEE80211_HW_AMPDU_AGGREGATION  | */
#if defined(CONFIG_XRADIO_USE_EXTENSIONS)
	            //IEEE80211_HW_SUPPORTS_P2P_PS          |
	            //IEEE80211_HW_SUPPORTS_CQM_BEACON_MISS |
	          //  IEEE80211_HW_SUPPORTS_CQM_TX_FAIL     |
#endif /* CONFIG_XRADIO_USE_EXTENSIONS */
	            //IEEE80211_HW_BEACON_FILTER;

	hw->wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION)    |
	                             BIT(NL80211_IFTYPE_ADHOC)      |
	                             BIT(NL80211_IFTYPE_AP)         |
	                             BIT(NL80211_IFTYPE_MESH_POINT) |
	                             BIT(NL80211_IFTYPE_P2P_CLIENT) |
	                             BIT(NL80211_IFTYPE_P2P_GO);

	/* Support only for limited wowlan functionalities */
	/* TODO by Icenowy: RESTORE THIS */
/*	hw->wiphy->wowlan.flags = WIPHY_WOWLAN_ANY | WIPHY_WOWLAN_DISCONNECT;
	hw->wiphy->wowlan.n_patterns = 0;*/

#if defined(CONFIG_XRADIO_USE_EXTENSIONS)
	hw->wiphy->flags |= WIPHY_FLAG_AP_UAPSD;
#endif /* CONFIG_XRADIO_USE_EXTENSIONS */
	/* fix the problem that driver can not set pro-resp template frame to fw */
	hw->wiphy->flags |= WIPHY_FLAG_AP_PROBE_RESP_OFFLOAD;

#if defined(CONFIG_XRADIO_DISABLE_BEACON_HINTS)
	hw->wiphy->flags |= WIPHY_FLAG_DISABLE_BEACON_HINTS;
#endif
	hw->wiphy->n_addresses = XRWL_MAX_VIFS;
	hw->wiphy->addresses   = hw_priv->addresses;
	hw->wiphy->max_remain_on_channel_duration = 500;
	hw->extra_tx_headroom = WSM_TX_EXTRA_HEADROOM +
	                        8  /* TKIP IV */      +
	                        12 /* TKIP ICV and MIC */;
	hw->wiphy->bands[NL80211_BAND_2GHZ] = &xradio_band_2ghz;
	hw->queues         = AC_QUEUE_NUM;
	hw->max_rates      = MAX_RATES_STAGE;
	hw->max_rate_tries = MAX_RATES_RETRY;
	/* Channel params have to be cleared before registering wiphy again */
	for (band = 0; band < NUM_NL80211_BANDS; band++) {
		sband = hw->wiphy->bands[band];
		if (!sband)
			continue;
		for (i = 0; i < sband->n_channels; i++) {
			sband->channels[i].flags = 0;
			sband->channels[i].max_antenna_gain = 0;
			sband->channels[i].max_power = 30;
		}
	}
	/* hw_priv->channel init value is the local->oper_channel init value;when transplanting,take care */
	for (band = 0; band < NUM_NL80211_BANDS; band++) {
		sband = hw->wiphy->bands[band];
		if (!sband)
			continue;
		if(!hw_priv->channel){
			hw_priv->channel = &sband->channels[2];
		}	
	}
	hw->wiphy->max_scan_ssids = WSM_SCAN_MAX_NUM_OF_SSIDS;
	hw->wiphy->max_scan_ie_len = IEEE80211_MAX_DATA_LEN;
	SET_IEEE80211_PERM_ADDR(hw, hw_priv->addresses[0].addr);

	/* Initialize locks. */
	spin_lock_init(&hw_priv->vif_list_lock);
	mutex_init(&hw_priv->wsm_cmd_mux);
	mutex_init(&hw_priv->conf_mutex);
	mutex_init(&hw_priv->wsm_oper_lock);
	atomic_set(&hw_priv->tx_lock, 0);
	sema_init(&hw_priv->tx_lock_sem, 1);

	hw_priv->workqueue = create_singlethread_workqueue(XRADIO_WORKQUEUE);
	sema_init(&hw_priv->scan.lock, 1);
	sema_init(&hw_priv->scan.status_lock,1);
	INIT_WORK(&hw_priv->scan.work, xradio_scan_work);
#ifdef ROAM_OFFLOAD
	INIT_WORK(&hw_priv->scan.swork, xradio_sched_scan_work);
#endif /*ROAM_OFFLOAD*/
	INIT_DELAYED_WORK(&hw_priv->scan.probe_work, xradio_probe_work);
	INIT_DELAYED_WORK(&hw_priv->scan.timeout, xradio_scan_timeout);
	INIT_DELAYED_WORK(&hw_priv->rem_chan_timeout, xradio_rem_chan_timeout);
	INIT_WORK(&hw_priv->tx_policy_upload_work, tx_policy_upload_work);
	atomic_set(&hw_priv->upload_count, 0);
	memset(&hw_priv->connet_time, 0, sizeof(hw_priv->connet_time));

	spin_lock_init(&hw_priv->event_queue_lock);
	INIT_LIST_HEAD(&hw_priv->event_queue);
	INIT_WORK(&hw_priv->event_handler, xradio_event_handler);
	INIT_WORK(&hw_priv->ba_work, xradio_ba_work);
	spin_lock_init(&hw_priv->ba_lock);
	timer_setup(&hw_priv->ba_timer, xradio_ba_timer, 0);

	if (unlikely(xradio_queue_stats_init(&hw_priv->tx_queue_stats,
			WLAN_LINK_ID_MAX,xradio_skb_dtor, hw_priv))) {
		ieee80211_free_hw(hw);
		return NULL;
	}
	for (i = 0; i < AC_QUEUE_NUM; ++i) {
		if (unlikely(xradio_queue_init(&hw_priv->tx_queue[i],
				&hw_priv->tx_queue_stats, i, XRWL_MAX_QUEUE_SZ, xradio_ttl[i]))) {
			for (; i > 0; i--)
				xradio_queue_deinit(&hw_priv->tx_queue[i - 1]);
			xradio_queue_stats_deinit(&hw_priv->tx_queue_stats);
			ieee80211_free_hw(hw);
			return NULL;
		}
	}

	init_waitqueue_head(&hw_priv->channel_switch_done);
	init_waitqueue_head(&hw_priv->wsm_cmd_wq);
	init_waitqueue_head(&hw_priv->wsm_startup_done);
	init_waitqueue_head(&hw_priv->offchannel_wq);
	hw_priv->wsm_caps.firmwareReady = 0;
	hw_priv->driver_ready = 0;
	hw_priv->offchannel_done = 0;
	wsm_buf_init(&hw_priv->wsm_cmd_buf);
	spin_lock_init(&hw_priv->wsm_cmd.lock);
	tx_policy_init(hw_priv);
	xradio_init_resv_skb(hw_priv);
	/* add for setting short_frame_max_tx_count(mean wdev->retry_short) to drv,init the max_rate_tries */
	spin_lock_bh(&hw_priv->tx_policy_cache.lock);
	hw_priv->long_frame_max_tx_count = hw->conf.long_frame_max_tx_count;
	hw_priv->short_frame_max_tx_count =
			(hw->conf.short_frame_max_tx_count< 0x0F) ?
			hw->conf.short_frame_max_tx_count : 0x0F;
	hw_priv->hw->max_rate_tries = hw->conf.short_frame_max_tx_count;
	spin_unlock_bh(&hw_priv->tx_policy_cache.lock);

	for (i = 0; i < XRWL_MAX_VIFS; i++)
		hw_priv->hw_bufs_used_vif[i] = 0;

#ifdef MCAST_FWDING
	for (i = 0; i < WSM_MAX_BUF; i++)
		wsm_init_release_buffer_request(hw_priv, i);
	hw_priv->buf_released = 0;
#endif
	hw_priv->vif0_throttle = XRWL_HOST_VIF0_11BG_THROTTLE;
	hw_priv->vif1_throttle = XRWL_HOST_VIF1_11BG_THROTTLE;

	hw_priv->query_packetID = 0;
	atomic_set(&hw_priv->query_cnt, 0);
	INIT_WORK(&hw_priv->query_work, wsm_query_work);

#ifdef CONFIG_XRADIO_SUSPEND_POWER_OFF
	atomic_set(&hw_priv->suspend_state, XRADIO_RESUME);
#endif

	xradio_set_ifce_comb(hw_priv, hw_priv->hw);

	return hw;
}

void xradio_free_common(struct ieee80211_hw *dev)
{
	int i;
	struct xradio_common *hw_priv = dev->priv;

	cancel_work_sync(&hw_priv->query_work);
	del_timer_sync(&hw_priv->ba_timer);
	mutex_destroy(&hw_priv->wsm_oper_lock);
	mutex_destroy(&hw_priv->conf_mutex);
	mutex_destroy(&hw_priv->wsm_cmd_mux);
	wsm_buf_deinit(&hw_priv->wsm_cmd_buf);
	flush_workqueue(hw_priv->workqueue);
	destroy_workqueue(hw_priv->workqueue);
	hw_priv->workqueue = NULL;

	xradio_deinit_resv_skb(hw_priv);
	if (hw_priv->skb_cache) {
		dev_kfree_skb(hw_priv->skb_cache);
		hw_priv->skb_cache = NULL;
	}

	for (i = 0; i < 4; ++i)
		xradio_queue_deinit(&hw_priv->tx_queue[i]);
	xradio_queue_stats_deinit(&hw_priv->tx_queue_stats);

	for (i = 0; i < XRWL_MAX_VIFS; i++) {
		kfree(hw_priv->vif_list[i]);
		hw_priv->vif_list[i] = NULL;
	}

//fixed memory leakage by yangfh
#ifdef MCAST_FWDING
	wsm_deinit_release_buffer(hw_priv);
#endif
	/* unsigned int i; */
	ieee80211_free_hw(dev);
}

int xradio_register_common(struct ieee80211_hw *dev)
{
	int err = 0;
	struct xradio_common *hw_priv = dev->priv;

	SET_IEEE80211_DEV(dev, hw_priv->pdev);
	err = ieee80211_register_hw(dev);
	if (err) {
		dev_dbg(hw_priv->pdev, "Cannot register device (%d).\n", err);
		return err;
	}
	dev_dbg(hw_priv->pdev, "is registered as '%s'\n",
	           wiphy_name(dev->wiphy));

	hw_priv->driver_ready = 1;
	wake_up(&hw_priv->wsm_startup_done);
	return 0;
}

void xradio_unregister_common(struct ieee80211_hw *dev)
{
	struct xradio_common *hw_priv = dev->priv;

	if (wiphy_dev(dev->wiphy)) {
	ieee80211_unregister_hw(dev);
		SET_IEEE80211_DEV(dev, NULL);
	}
	hw_priv->driver_ready = 0;
}

int xradio_core_init(struct sdio_func* func)
{
	int err = -ENOMEM;
	u16 ctrl_reg;
	int if_id;
	struct ieee80211_hw *dev;
	struct xradio_common *hw_priv;
	unsigned char randomaddr[ETH_ALEN];
	const unsigned char *addr = NULL;

	//init xradio_common
	dev = xradio_init_common(sizeof(struct xradio_common));
	if (!dev) {
		dev_dbg(&func->dev, "xradio_init_common failed\n");
		return err;
	}
	hw_priv = dev->priv;
	hw_priv->pdev = &func->dev;
	hw_priv->sdio_func = func;
	sdio_set_drvdata(func, hw_priv);

	// fill in mac addresses
	if (hw_priv->pdev->of_node) {
		addr = of_get_mac_address(hw_priv->pdev->of_node);
	}
	if (!addr) {
		dev_warn(hw_priv->pdev, "no mac address provided, using random\n");
		eth_random_addr(randomaddr);
		addr = randomaddr;
	}
	memcpy(hw_priv->addresses[0].addr, addr, ETH_ALEN);
	memcpy(hw_priv->addresses[1].addr, addr, ETH_ALEN);
	hw_priv->addresses[1].addr[5] += 0x01;

	/*init pm and wakelock. */
#ifdef CONFIG_PM
	err = xradio_pm_init(&hw_priv->pm_state, hw_priv);
	if (err) {
		dev_dbg(hw_priv->pdev, "xradio_pm_init failed(%d).\n", err);
		goto err2;
	}
#endif
	/* Register bh thread*/
	err = xradio_register_bh(hw_priv);
	if (err) {
		dev_dbg(hw_priv->pdev, "xradio_register_bh failed(%d).\n", err);
		goto err3;
	}

	/* Load firmware and register Interrupt Handler */
	err = xradio_load_firmware(hw_priv);
	if (err) {
		dev_dbg(hw_priv->pdev, "xradio_load_firmware failed(%d).\n", err);
		goto err4;
	}

	/* Set sdio blocksize. */
	sdio_lock(hw_priv);
	WARN_ON(sdio_set_blk_size(hw_priv,
			SDIO_BLOCK_SIZE));
	sdio_unlock(hw_priv);

	if (wait_event_interruptible_timeout(hw_priv->wsm_startup_done,
				hw_priv->wsm_caps.firmwareReady, 3*HZ) <= 0) {

		/* TODO: Needs to find how to reset device */
		/*       in QUEUE mode properly.           */
		dev_dbg(hw_priv->pdev, "Firmware Startup Timeout!\n");
		err = -ETIMEDOUT;
		goto err5;
	}
	dev_dbg(hw_priv->pdev, "Firmware Startup Done.\n");

	/* Keep device wake up. */
	WARN_ON(xradio_reg_write_16(hw_priv, HIF_CONTROL_REG_ID, HIF_CTRL_WUP_BIT));
	if (xradio_reg_read_16(hw_priv,HIF_CONTROL_REG_ID, &ctrl_reg))
		WARN_ON(xradio_reg_read_16(hw_priv,HIF_CONTROL_REG_ID, &ctrl_reg));
	WARN_ON(!(ctrl_reg & HIF_CTRL_RDY_BIT));

	/* Set device mode parameter. */
	for (if_id = 0; if_id < xrwl_get_nr_hw_ifaces(hw_priv); if_id++) {
		/* Set low-power mode. */
		WARN_ON(wsm_set_operational_mode(hw_priv, &defaultoperationalmode, if_id));
		/* Enable multi-TX confirmation */
		WARN_ON(wsm_use_multi_tx_conf(hw_priv, true, if_id));
	}

	/* Register wireless net device. */
	err = xradio_register_common(dev);
	if (err) {
		dev_dbg(hw_priv->pdev, "xradio_register_common failed(%d)!\n", err);
		goto err5;
	}

	return err;

err5:
	xradio_dev_deinit(hw_priv);
err4:
	xradio_unregister_bh(hw_priv);
err3:
	xradio_pm_deinit(&hw_priv->pm_state);
err2:
/* err1: 	MRK: unused label*/
	xradio_free_common(dev);
	return err;
}

void xradio_core_deinit(struct sdio_func* func)
{
	struct xradio_common* hw_priv = sdio_get_drvdata(func);
	if (hw_priv) {
		xradio_unregister_common(hw_priv->hw);
		xradio_dev_deinit(hw_priv);
		xradio_unregister_bh(hw_priv);
		xradio_pm_deinit(&hw_priv->pm_state);
		xradio_free_common(hw_priv->hw);
	}
	return;
}
