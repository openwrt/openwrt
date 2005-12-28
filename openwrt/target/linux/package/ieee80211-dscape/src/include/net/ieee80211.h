/*
 * Low-level hardware driver -- IEEE 802.11 driver (80211.o) interface
 * Copyright 2002-2005, Devicescape Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef IEEE80211_H
#define IEEE80211_H

#include "ieee80211_shared.h"

/* Note! Only ieee80211_tx_status_irqsafe() and ieee80211_rx_irqsave() can be
 * called in hardware interrupt context. The low-level driver must not call any
 * other functions in hardware interrupt context. If there is a need for such
 * call, the low-level driver should first ACK the interrupt and perform the
 * IEEE 802.11 code call after this, e.g., from a scheduled tasklet (in
 * software interrupt context).
 */

/*
 * Frame format used when passing frame between low-level hardware drivers
 * and IEEE 802.11 driver the same as used in the wireless media, i.e.,
 * buffers start with IEEE 802.11 header and include the same octets that
 * are sent over air.
 *
 * If hardware uses IEEE 802.3 headers (and perform 802.3 <-> 802.11
 * conversion in firmware), upper layer 802.11 code needs to be changed to
 * support this.
 *
 * If the receive frame format is not the same as the real frame sent
 * on the wireless media (e.g., due to padding etc.), upper layer 802.11 code
 * could be updated to provide support for such format assuming this would
 * optimize the performance, e.g., by removing need to re-allocation and
 * copying of the data.
 */

/* Interface version (used for compatibility verification) */
#define IEEE80211_VERSION 2


/* Channel information structure. Low-level driver is expected to fill in chan,
 * freq, and val fields. Other fields will be filled in by 80211.o based on
 * hostapd information and low-level driver does not need to use them. The
 * limits for each channel will be provided in 'struct ieee80211_conf' when
 * configuring the low-level driver with hw->config callback. */
struct ieee80211_channel {
	short chan; /* channel number (IEEE 802.11) */
	short freq; /* frequency in MHz */
	int val; /* hw specific value for the channel */
	int flag; /* flag for hostapd use (IEEE80211_CHAN_*) */
        unsigned char power_level;
        unsigned char antenna_max;
};

struct ieee80211_rate {
	int rate; /* rate in 100 kbps */
	int val; /* hw specific value for the rate */
	int flags; /* IEEE80211_RATE_ flags */
	int val2; /* hw specific value for the rate when using short preamble
		   * (only when IEEE80211_RATE_PREAMBLE2 flag is set, i.e., for
		   * 2, 5.5, and 11 Mbps) */
	signed char min_rssi_ack;
	unsigned char min_rssi_ack_delta;

	/* following fields are set by 80211.o and need not be filled by the
	 * low-level driver */
	int rate_inv; /* inverse of the rate (LCM(all rates) / rate) for
		       * optimizing channel utilization estimates */
};

struct ieee80211_hw_modes {
	int mode;
	int num_channels;
	struct ieee80211_channel *channels;
	int num_rates;
        struct ieee80211_rate *rates;
	int xr_end; /* only used with Atheros XR */
};

struct ieee80211_tx_queue_params {
	int aifs; /* 0 .. 255; -1 = use default */
	int cw_min; /* 2^n-1: 1, 3, 7, .. , 1023; 0 = use default */
	int cw_max; /* 2^n-1: 1, 3, 7, .. , 1023; 0 = use default */
	int burst_time; /* maximum burst time in 0.1 ms (i.e., 10 = 1 ms);
			 * 0 = disabled */
};

#define NUM_TX_DATA_QUEUES 6

struct ieee80211_tx_queue_stats_data {
	unsigned int len; /* num packets in queue */
	unsigned int limit; /* queue len (soft) limit */
	unsigned int count; /* total num frames sent */
};

struct ieee80211_tx_queue_stats {
	struct ieee80211_tx_queue_stats_data data[NUM_TX_DATA_QUEUES];
};

#ifndef IEEE80211_TX_QUEUE_NUMS
#define IEEE80211_TX_QUEUE_NUMS
/* TODO: these need to be synchronized with hostapd_ioctl.h; make a shared
 * header file that can be included into low-level drivers, 80211.o, and
 * hostapd */
enum {
	IEEE80211_TX_QUEUE_DATA0 = 0,
	IEEE80211_TX_QUEUE_DATA1 = 1,
	IEEE80211_TX_QUEUE_DATA2 = 2,
	IEEE80211_TX_QUEUE_DATA3 = 3,
	IEEE80211_TX_QUEUE_DATA4 = 4,
	IEEE80211_TX_QUEUE_SVP = 5,
	IEEE80211_TX_QUEUE_AFTER_BEACON = 6,
	IEEE80211_TX_QUEUE_BEACON = 7
};
#endif /* IEEE80211_TX_QUEUE_NUMS */


struct ieee80211_low_level_stats {
	unsigned int dot11ACKFailureCount;
	unsigned int dot11RTSFailureCount;
        unsigned int dot11FCSErrorCount;
	unsigned int dot11RTSSuccessCount;
};

/* Transmit control fields. This data structure is passed to low-level driver
 * with each TX frame. The low-level driver is responsible of configuring
 * hardware to use given values (depending on what is supported). */
#define HW_KEY_IDX_INVALID -1

struct ieee80211_tx_control {
	enum { PKT_NORMAL = 0, PKT_PROBE_RESP } pkt_type;
	int tx_rate; /* Transmit rate, given as the hw specific value for the
		      * rate (from struct ieee80211_rate) */
	int rts_cts_rate; /* Transmit rate for RTS/CTS frame, given as the hw
			   * specific value for the rate (from
			   * struct ieee80211_rate) */
	/* 1 = only first attempt, 2 = one retry, .. */
	unsigned int retry_limit:8;
	/* duration field for RTS/CTS frame */
	unsigned int rts_cts_duration:16;
	/* TODO: change these bit flags to use one unsigned int variable and
	 * defines with BIT(n). These are copied to TX status structure and
	 * this will make the code faster and smaller. */
	unsigned int req_tx_status:1; /* request TX status callback for this
				       * frame */
	unsigned int do_not_encrypt:1; /* send this frame without encryption;
			       * e.g., for EAPOL frames */
	unsigned int use_rts_cts:1; /* Use RTS-CTS before sending frame. */
	unsigned int use_cts_protect:1; /* Use CTS protection for the frame
					 * (e.g., for combined 802.11g /
					 * 802.11b networks) */
        unsigned int no_ack:1; /* Tell the low level not to wait for an ack */
	unsigned int rate_ctrl_probe:1;
	unsigned int clear_dst_mask:1;
	unsigned int requeue:1;
	 /* following three flags are only used with Atheros Super A/G */
	unsigned int compress:1;
	unsigned int turbo_prime_notify:1; /* notify HostaAPd after frame
					    * transmission */
	unsigned int fast_frame:1;

	unsigned int atheros_xr:1; /* only used with Atheros XR */

        unsigned int power_level:8; /* per-packet transmit power level, in dBm
				     */
	unsigned int antenna_sel:4; /* 0 = default/diversity,
				     * 1 = Ant0, 2 = Ant1 */
	int key_idx:8; /* -1 = do not encrypt, >= 0 keyidx from hw->set_key()
			*/
	int icv_len:8; /* Length of the ICV/MIC field in octets */
	int iv_len:8; /* Length of the IV field in octets */
	unsigned int queue:4; /* hardware queue to use for this frame;
		      * 0 = highest, hw->queues-1 = lowest */
	unsigned int sw_retry_attempt:4; /* no. of times hw has tried to
		      * transmit frame (not incl. hw retries) */

#if 0
	int rateidx; /* internal 80211.o rateidx, to be copied to tx_status */
	int alt_retry_rate; /* retry rate for the last retries, given as the
			     * hw specific value for the rate (from
			     * struct ieee80211_rate). To be used to limit
			     * packet dropping when probing higher rates, if hw
			     * supports multiple retry rates. -1 = not used */
#endif
};

#define IEEE80211_CB_MAGIC 0xAAB80211

struct ieee80211_tx_packet_data {
        unsigned int magic;
        struct ieee80211_tx_control control;
        unsigned long jiffies;
        struct ieee80211_sub_if_data *sdata;
};

#define RX_FLAG_MMIC_ERROR       0x1
#define RX_FLAG_DECRYPTED        0x2
#define RX_FLAG_XR_DOUBLE_CHIRP  0x4

/* Receive status. The low-level driver should provide this information
 * (the subset supported by hardware) to the 802.11 code with each received
 * frame.
 * Current implementation copies this into skb->cb, so it must be less than
 * 48 bytes. */
struct ieee80211_rx_status {
#if 0
        u64 hosttime;
	u64 mactime;
#endif
        int freq; /* receive frequency in Mhz */
        int channel;
        int phymode;
        int ssi;
        int antenna;
        int rate;
        int flag;
};

/* Transmit status. The low-level driver should provide this information
 * (the subset supported by hardware) to the 802.11 code for each transmit
 * frame. */
struct ieee80211_tx_status {
	/* flags copied from struct ieee80211_tx_control) */
	unsigned int req_tx_status:1; /* whether TX status was explicitly
				       * requested */
	unsigned int rate_ctrl_probe:1; /* whether this was a probe packet from
					 * rate control */
	unsigned int tx_filtered:1;

	/* following three fields are only used with Atheros Super A/G */
	unsigned int turbo_prime_notify:1; /* notify HostAPd - CTS for Turbo
					    * Prime is sent */
        int queue_length;      /* information about TX queue */
        int queue_number;

	int ack; /* whether the TX frame was ACKed */
	int ack_signal; /* measured signal strength of the ACK frame */
	int excessive_retries;
	int retry_count;
	int rateidx; /* internal 80211.o rateidx, to be copied to tx_status */
};


struct ieee80211_conf {
	int channel;			/* IEEE 802.11 channel number */
	int freq;			/* MHz */
	int channel_val;		/* hw specific value for the channel */

	int mode;			/* IW_MODE_ */

	int phymode;			/* MODE_IEEE80211A, .. */
        unsigned int regulatory_domain;
        int adm_status;

        int beacon_int;

	/* Bitfields, grouped together */

	int sw_encrypt:1;
	int sw_decrypt:1;
	int short_slot_time:1;		/* use IEEE 802.11g Short Slot Time */
        int ssid_hidden:1;		/* do not broadcast the ssid */

        /* these fields are used by low level drivers for hardware
         * that generate beacons independently */
        u8 *ssid;
	size_t ssid_len;
	u8 *generic_elem;
	size_t generic_elem_len;

        u8 power_level;			/* transmit power limit for current
					 * regulatory domain; in dBm */
        u8 antenna_max;			/* maximum antenna gain */
	short tx_power_reduction; /* in 0.1 dBm */

	int antenna_sel;		/* default antenna conf:
					 *	0 = default/diversity,
			  		 *	1 = Ant0,
					 *	2 = Ant1 */

	int calib_int;			/* hw/radio calibration interval in
					 * seconds */
        int antenna_def;
        int antenna_mode;

	u8 bssid_mask[ETH_ALEN];	/* ff:ff:ff:ff:ff:ff = 1 BSSID */
	int bss_count;

	int atheros_super_ag_compression;
	int atheros_super_ag_fast_frame;
	int atheros_super_ag_burst;
	int atheros_super_ag_wme_ele;
	int atheros_super_ag_turbo_g;
	int atheros_super_ag_turbo_prime;

	int atheros_xr;

	u8 client_bssid[ETH_ALEN];

	/* Following five fields are used for IEEE 802.11H */
	unsigned int radar_detect;
	unsigned int spect_mgmt;
	unsigned int quiet_duration; /* duration of quiet period */
	unsigned int quiet_offset; /* how far into the beacon is the quiet
				    * period */
	unsigned int quiet_period;
};


typedef enum { ALG_NONE, ALG_WEP, ALG_TKIP, ALG_CCMP, ALG_NULL }
ieee80211_key_alg;


struct ieee80211_key_conf {

	int hw_key_idx;			/* filled + used by low-level driver */
	ieee80211_key_alg alg;
	int keylen;

        int force_sw_encrypt:1;		/* to be cleared by low-level driver */
	int keyidx:8;			/* WEP key index */
	int default_tx_key:1;		/* This key is the new default TX key
			       		 * (used only for broadcast keys). */
	int default_wep_only:1; /* static WEP is the only configured security
				 * policy; this allows some low-level drivers
				 * to determine when hwaccel can be used */
        u8 key[0];
};

#define IEEE80211_SCAN_START 1
#define IEEE80211_SCAN_END   2

struct ieee80211_scan_conf {
        int scan_channel;     /* IEEE 802.11 channel number to do passive scan
			       * on */
        int scan_freq;        /* new freq in MHz to switch to for passive scan
			       */
	int scan_channel_val; /* hw specific value for the channel */
        int scan_phymode;     /* MODE_IEEE80211A, .. */
        unsigned char scan_power_level;
        unsigned char scan_antenna_max;


        int running_channel; /* IEEE 802.11 channel number we operate on
			      * normally */
        int running_freq;    /* freq in MHz we're operating on normally */
	int running_channel_val; /* hw specific value for the channel */
        int running_phymode;
        unsigned char running_power_level;
        unsigned char running_antenna_max;

        int scan_time;       /* time a scan will take in us */
        int tries;

        struct sk_buff *skb; /* skb to transmit before changing channels, maybe
			      * NULL for none */
        struct ieee80211_tx_control *tx_control;

};

#ifndef IW_MODE_ADHOC
#define IW_MODE_ADHOC 1
#endif

#ifndef IW_MODE_INFRA
#define IW_MODE_INFRA 2
#endif

#ifndef IW_MODE_MASTER
#define IW_MODE_MASTER 3
#endif

#ifndef IW_MODE_MONITOR
#define IW_MODE_MONITOR 6
#endif

#define IEEE80211_SEQ_COUNTER_RX	0
#define IEEE80211_SEQ_COUNTER_TX	1

typedef enum {
	SET_KEY, DISABLE_KEY, REMOVE_ALL_KEYS,
	ENABLE_COMPRESSION, DISABLE_COMPRESSION
} set_key_cmd;

/* Configuration block used by the low-level driver to tell 802.11 code about
 * supported hardware features and to pass function pointers for callback
 * functions. */
struct ieee80211_hw {
	int version; /* IEEE80211_VERSION */

	/* Driver name */
	char *name;

	/* TODO: frame_type 802.11/802.3, sw_encryption requirements */

	/* Some wireless LAN chipsets generate beacons in the hardware/firmware
	 * and others rely on host generated beacons. This option is used to
	 * configure upper layer IEEE 802.11 module to generate beacons. The
	 * low-level driver can use ieee80211_beacon_get() to fetch next
	 * beacon frame. */
	int host_gen_beacon:1;


	/* Some devices handle decryption internally and do not
	 * indicate whether the frame was encrypted (unencrypted frames
	 * will be dropped by the hardware, unless specifically allowed
	 * through) */
        int device_hides_wep:1;

	/* Whether RX frames passed to ieee80211_rx() include FCS in the end
	 */
	int rx_includes_fcs:1;

	/* Some wireless LAN chipsets buffer broadcast/multicast frames for
	 * power saving stations in the hardware/firmware and others rely on
	 * the host system for such buffering. This option is used to
	 * configure upper layer IEEE 802.11 to buffer broadcast/multicast
	 * frames when there are power saving stations so that low-level driver
	 * can fetch them with ieee80211_get_buffered_bc(). */
	int host_broadcast_ps_buffering:1;

	int wep_include_iv:1;
	int data_nullfunc_ack:1; /* will data nullfunc frames get proper
                                  * TX status callback */

	/* Force sw version of encryption for TKIP packets if WMM is enabled.
	 */
	int no_tkip_wmm_hwaccel:1;

        /* 1 if the payload needs to be padded at even boundaries after the
	 * header */
        unsigned int extra_hdr_room:1;

	/* Some devices handle Michael MIC internally and do not include MIC in
	 * the received packets given to 80211.o. device_strips_mic must be set
	 * for such devices. ISWEP bit is still expected to be set in the IEEE
	 * 802.11 header with this option unlike with device_hides_wep option.
	 */
	unsigned int device_strips_mic:1;

	/* 1 = low-level driver supports skb fraglist (NETIF_F_FRAGLIST), i.e.,
	 * more than one skb per frame */
	unsigned int fraglist;

        /* This is the time in us to change channels
         */
        int channel_change_time;

	int num_modes;
	struct ieee80211_hw_modes *modes;

	/* Handler that 802.11 module calls for each transmitted frame.
	 * skb contains the buffer starting from the IEEE 802.11 header.
	 * The low-level driver should send the frame out based on
	 * configuration in the TX control data. */
	int (*tx)(struct net_device *dev, struct sk_buff *skb,
		  struct ieee80211_tx_control *control);

	/* Handler for performing hardware reset. */
	int (*reset)(struct net_device *dev);

	/* Handler that is called when any netdevice attached to the hardware
	 * device is set UP for the first time. This can be used, e.g., to
	 * enable interrupts and beacon sending. */
	int (*open)(struct net_device *dev);

	/* Handler that is called when the last netdevice attached to the
	 * hardware device is set DOWN. This can be used, e.g., to disable
	 * interrupts and beacon sending. */
	int (*stop)(struct net_device *dev);

	/* Handler for configuration requests. IEEE 802.11 code calls this
	 * function to change hardware configuration, e.g., channel. */
	int (*config)(struct net_device *dev, struct ieee80211_conf *conf);

	/* Set TIM bit handler. If the hardware/firmware takes care of beacon
	 * generation, IEEE 802.11 code uses this function to tell the
	 * low-level to set (or clear if set==0) TIM bit for the given aid. If
	 * host system is used to generate beacons, this handler is not used
	 * and low-level driver should set it to NULL. */
	int (*set_tim)(struct net_device *dev, int aid, int set);

	/* Set encryption key. IEEE 802.11 module calls this function to set
	 * encryption keys. addr is ff:ff:ff:ff:ff:ff for default keys and
	 * station hwaddr for individual keys. aid of the station is given
	 * to help low-level driver in selecting which key->hw_key_idx to use
	 * for this key. TX control data will use the hw_key_idx selected by
	 * the low-level driver. */
	int (*set_key)(struct net_device *dev, set_key_cmd cmd, u8 *addr,
                       struct ieee80211_key_conf *key, int aid);

	/* Set TX key index for default/broadcast keys. This is needed in cases
	 * where wlan card is doing full WEP/TKIP encapsulation (wep_include_iv
	 * is not set), in other cases, this function pointer can be set to
	 * NULL since 80211.o takes care of selecting the key index for each
	 * TX frame. */
	int (*set_key_idx)(struct net_device *dev, int idx);

	/* Enable/disable IEEE 802.1X. This item requests wlan card to pass
	 * unencrypted EAPOL-Key frames even when encryption is configured.
	 * If the wlan card does not require such a configuration, this
	 * function pointer can be set to NULL. 80211.o */
	int (*set_ieee8021x)(struct net_device *dev, int use_ieee8021x);

	/* Set port authorization state (IEEE 802.1X PAE) to be authorized
	 * (authorized=1) or unauthorized (authorized=0). This function can be
	 * used if the wlan hardware or low-level driver implements PAE.
	 * 80211.o module will anyway filter frames based on authorization
	 * state, so this function pointer can be NULL if low-level driver does
	 * not require event notification about port state changes. */
	int (*set_port_auth)(struct net_device *dev, u8 *addr, int authorized);

        /* Ask the hardware to do a passive scan on a new channel. The hardware
         * will do what ever is required to nicely leave the current channel
         * including transmit any CTS packets, etc. */
        int (*passive_scan)(struct net_device *dev, int state,
                            struct ieee80211_scan_conf *conf);

        /* return low-level statistics */
	int (*get_stats)(struct net_device *dev,
			 struct ieee80211_low_level_stats *stats);

        /* Enable/disable test modes; mode = IEEE80211_TEST_* */
	int (*test_mode)(struct net_device *dev, int mode);

	/* Configuration of test parameters */
	int (*test_param)(struct net_device *dev, int param, int value);

	/* Change MAC address. addr is pointer to struct sockaddr. */
	int (*set_mac_address)(struct net_device *dev, void *addr);

        /* For devices that generate their own beacons and probe response
         * or association responses this updates the state of privacy_invoked
         * returns 0 for success or an error number */

        int (*set_privacy_invoked)(struct net_device *dev,
                                   int privacy_invoked);

	/* For devices that have internal sequence counters, allow 802.11
	 * code to access the current value of a counter */
	int (*get_sequence_counter)(struct net_device *dev,
	                            u8* addr, u8 keyidx, u8 txrx,
				    u32* iv32, u16* iv16);

	/* Configuration of RTS threshold (if device needs it) */
	int (*set_rts_threshold)(struct net_device *dev, u32 value);

	/* Configuration of fragmentation threshold (if device needs it) */
	int (*set_frag_threshold)(struct net_device *dev, u32 value);

	/* Configuration of retry limits (if device needs it) */
	int (*set_retry_limit)(struct net_device *dev, u32 short_retry,
			       u32 long_retr);

	/* Number of STAs in STA table notification (NULL = disabled) */
	void (*sta_table_notification)(struct net_device *dev, int num_sta);

	/* Configure TX queue parameters (EDCF (aifs, cw_min, cw_max),
	 * bursting) for a hardware TX queue.
	 * queue = IEEE80211_TX_QUEUE_*. */
	int (*conf_tx)(struct net_device *dev, int queue,
		       const struct ieee80211_tx_queue_params *params);

	/* Get statistics of the current TX queue status. This is used to get
	 * number of currently queued packets (queue length), maximum queue
	 * size (limit), and total number of packets sent using each TX queue
	 * (count). This information is used for WMM to find out which TX
	 * queues have room for more packets and by hostapd to provide
	 * statistics about the current queueing state to external programs. */
	int (*get_tx_stats)(struct net_device *dev,
			    struct ieee80211_tx_queue_stats *stats);

	/* Number of available hardware TX queues for data packets.
	 * WMM requires at least four queues. */
	int queues;

	/* Get the current TSF timer value from firmware/hardware. Currently,
	 * this is only used for IBSS mode debugging and, as such, is not a
	 * required function. */
	u64 (*get_tsf)(struct net_device *dev);

	/* Reset the TSF timer and allow firmware/hardware to synchronize with
	 * other STAs in the IBSS. This is only used in IBSS mode. This
	 * function is optional if the firmware/hardware takes full care of
	 * TSF synchronization. */
	void (*reset_tsf)(struct net_device *dev);

	/* Setup beacon data for IBSS beacons. Unlike access point (Master),
	 * IBSS uses a fixed beacon frame which is configured using this
	 * function. This handler is required only for IBSS mode. */
	int (*beacon_update)(struct net_device *dev, struct sk_buff *skb,
			     struct ieee80211_tx_control *control);

	/* Determine whether the last IBSS beacon was sent by us. This is
	 * needed only for IBSS mode and the result of this function is used to
	 * determine whether to reply to Probe Requests. */
	int (*tx_last_beacon)(struct net_device *dev);

	/* Optional handler for XR-in-use notification. */
	int (*atheros_xr_in_use)(struct net_device *dev, int in_use);
};

/* Allocate a new hardware device. This must be called once for each
 * hardware device. The returned pointer must be used to refer to this
 * device when calling other functions. 802.11 code allocates a private data
 * area for the low-level driver. The size of this area is given as
 * priv_data_len. ieee80211_dev_hw_data() is used to get a pointer to the
 * private data area.
 *
 * Note: in this version of the interface the returned pointer is struct
 * net_device *. This may change in the future and low-level driver should
 * not refer the device data directly to remain compatible with the future
 * versions of the interface. */
struct net_device *ieee80211_alloc_hw(size_t priv_data_len,
				      void (*setup)(struct net_device *));

/* Register hardware device to the IEEE 802.11 code and kernel. Low-level
 * drivers must call this function before using any other IEEE 802.11
 * function. */
int ieee80211_register_hw(struct net_device *dev, struct ieee80211_hw *hw);

/* This function is allowed to update hardware configuration (e.g., list of
 * supported operation modes and rates). */
int ieee80211_update_hw(struct net_device *dev, struct ieee80211_hw *hw);

/* Unregister a hardware device. This function instructs 802.11 code to free
 * allocated resources and unregister netdevices from the kernel. */
void ieee80211_unregister_hw(struct net_device *dev);

/* Free allocated net_device including private data of a driver. */
void ieee80211_free_hw(struct net_device *dev);

/* Receive frame callback function. The low-level driver uses this function to
 * send received frames to the IEEE 802.11 code. Receive buffer (skb) must
 * start with IEEE 802.11 header. */
void ieee80211_rx(struct net_device *dev, struct sk_buff *skb,
		  struct ieee80211_rx_status *status);
void ieee80211_rx_irqsafe(struct net_device *dev, struct sk_buff *skb,
			  struct ieee80211_rx_status *status);

/* Transmit status callback function. The low-level driver must call this
 * function to report transmit status for all the TX frames that had
 * req_tx_status set in the transmit control fields. In addition, this should
 * be called at least for all unicast frames to provide information for TX rate
 * control algorithm. In order to maintain all statistics, this function is
 * recommended to be called after each frame, including multicast/broadcast, is
 * sent. */
void ieee80211_tx_status(struct net_device *dev, struct sk_buff *skb,
			 struct ieee80211_tx_status *status);
void ieee80211_tx_status_irqsafe(struct net_device *dev, struct sk_buff *skb,
				 struct ieee80211_tx_status *status);

/* Beacon generation function. If the beacon frames are generated by the host
 * system (i.e., not in hardware/firmware), the low-level driver uses this
 * function to receive the next beacon frame from the 802.11 code. The
 * low-level is responsible for calling this function before beacon data is
 * needed (e.g., based on hardware interrupt). Returned skb is used only once
 * and low-level driver is responsible of freeing it. */
struct sk_buff * ieee80211_beacon_get(struct net_device *dev, int bss_idx,
				      struct ieee80211_tx_control *control);

/* Function for accessing buffered broadcast and multicast frames. If
 * hardware/firmware does not implement buffering of broadcast/multicast
 * frames when power saving is used, 802.11 code buffers them in the host
 * memory. The low-level driver uses this function to fetch next buffered
 * frame. In most cases, this is used when generating beacon frame. This
 * function returns a pointer to the next buffered skb or NULL if no more
 * buffered frames are available.
 *
 * Note: buffered frames are returned only after DTIM beacon frame was
 * generated with ieee80211_beacon_get() and the low-level driver must thus
 * call ieee80211_beacon_get() first. ieee80211_get_buffered_bc() returns
 * NULL if the previous generated beacon was not DTIM, so the low-level driver
 * does not need to check for DTIM beacons separately and should be able to
 * use common code for all beacons. */
struct sk_buff *
ieee80211_get_buffered_bc(struct net_device *dev, int bss_idx,
			  struct ieee80211_tx_control *control);

/* Low level drivers that have their own MLME and MAC indicate
 * the aid for an associating station with this call */
int ieee80211_set_aid_for_sta(struct net_device *dev, u8 *peer_address,
			      u16 aid);


/* Given an sk_buff with a raw 802.11 header at the data pointer this function
 * returns the 802.11 header length in bytes (not including encryption
 * headers). If the data in the sk_buff is too short to contain a valid 802.11
 * header the function returns 0.
 */
int ieee80211_get_hdrlen_from_skb(struct sk_buff *skb);

/* Like ieee80211_get_hdrlen_from_skb() but takes a FC in CPU order. */
int ieee80211_get_hdrlen(u16 fc);

/* Function for net interface operation. IEEE 802.11 may use multiple kernel
 * netdevices for each hardware device. The low-level driver does not "see"
 * these interfaces, so it should use this function to perform netif
 * operations on all interface. */
typedef enum {
	NETIF_ATTACH, NETIF_DETACH, NETIF_START, NETIF_STOP, NETIF_WAKE,
	NETIF_IS_STOPPED, NETIF_UPDATE_TX_START
} Netif_Oper;
int ieee80211_netif_oper(struct net_device *dev, Netif_Oper op);


/*
 * Function to get hardware configuration information
 * by the low level driver should it need it.
 */
struct ieee80211_conf *
ieee80211_get_hw_conf(struct net_device *dev);


/* Return a pointer to the low-level private data area for the given device. */
void * ieee80211_dev_hw_data(struct net_device *dev);
/* Return a pointer to network statistics data area for the given device. */
void * ieee80211_dev_stats(struct net_device *dev);

/* Function to indicate Radar Detection. The low level driver must call this
 * function to indicate the presence of radar in the current channel.
 * Additionally the radar type also could be sent */
int  ieee80211_radar_status(struct net_device *dev, int channel, int radar,
			    int radar_type);

/* Test modes */
enum {
	IEEE80211_TEST_DISABLE = 0 /* terminate testing */,
	IEEE80211_TEST_UNMASK_CHANNELS = 1 /* allow all channels to be used */,
	IEEE80211_TEST_CONTINUOUS_TX = 2,
};

/* Test parameters */
enum {
	/* TX power in hardware specific raw value */
	IEEE80211_TEST_PARAM_TX_POWER_RAW = 0,
	/* TX rate in hardware specific raw value */
	IEEE80211_TEST_PARAM_TX_RATE_RAW = 1,
	/* Continuous TX pattern (32-bit) */
	IEEE80211_TEST_PARAM_TX_PATTERN = 2,
	/* TX power in 0.1 dBm, 100 = 10 dBm */
	IEEE80211_TEST_PARAM_TX_POWER = 3,
	/* TX rate in 100 kbps, 540 = 54 Mbps */
	IEEE80211_TEST_PARAM_TX_RATE = 4,
	IEEE80211_TEST_PARAM_TX_ANT_SEL_RAW = 5,
};

/* ieee80211_tx_led called with state == 1 when the first frame is queued
 *   with state == 0 when the last frame is transmitted and tx queue is empty
 */
void ieee80211_tx_led(int state, struct net_device *dev);
/* ieee80211_rx_led is called each time frame is received, state is not used
 * (== 2)
 */
void ieee80211_rx_led(int state, struct net_device *dev);


/* IEEE 802.11 defines */

#define FCS_LEN 4

#define WLAN_FC_PVER		0x0003
#define WLAN_FC_TODS		0x0100
#define WLAN_FC_FROMDS		0x0200
#define WLAN_FC_MOREFRAG	0x0400
#define WLAN_FC_RETRY		0x0800
#define WLAN_FC_PWRMGT		0x1000
#define WLAN_FC_MOREDATA	0x2000
#define WLAN_FC_ISWEP		0x4000
#define WLAN_FC_ORDER		0x8000

#define WLAN_FC_GET_TYPE(fc)	(((fc) & 0x000c) >> 2)
#define WLAN_FC_GET_STYPE(fc)	(((fc) & 0x00f0) >> 4)

#define WLAN_GET_SEQ_FRAG(seq)	((seq) & 0x000f)
#define WLAN_GET_SEQ_SEQ(seq)	((seq) >> 4)

#define WLAN_FC_DATA_PRESENT(fc) (((fc) & 0x4c) == 0x08)

#define WLAN_FC_TYPE_MGMT	0
#define WLAN_FC_TYPE_CTRL	1
#define WLAN_FC_TYPE_DATA	2

/* management */
#define WLAN_FC_STYPE_ASSOC_REQ		0
#define WLAN_FC_STYPE_ASSOC_RESP	1
#define WLAN_FC_STYPE_REASSOC_REQ	2
#define WLAN_FC_STYPE_REASSOC_RESP	3
#define WLAN_FC_STYPE_PROBE_REQ		4
#define WLAN_FC_STYPE_PROBE_RESP	5
#define WLAN_FC_STYPE_BEACON		8
#define WLAN_FC_STYPE_ATIM		9
#define WLAN_FC_STYPE_DISASSOC		10
#define WLAN_FC_STYPE_AUTH		11
#define WLAN_FC_STYPE_DEAUTH		12
#define WLAN_FC_STYPE_ACTION		13

/* control */
#define WLAN_FC_STYPE_PSPOLL		10
#define WLAN_FC_STYPE_RTS		11
#define WLAN_FC_STYPE_CTS		12
#define WLAN_FC_STYPE_ACK		13
#define WLAN_FC_STYPE_CFEND		14
#define WLAN_FC_STYPE_CFENDACK		15

/* data */
#define WLAN_FC_STYPE_DATA		0
#define WLAN_FC_STYPE_DATA_CFACK	1
#define WLAN_FC_STYPE_DATA_CFPOLL	2
#define WLAN_FC_STYPE_DATA_CFACKPOLL	3
#define WLAN_FC_STYPE_NULLFUNC		4
#define WLAN_FC_STYPE_CFACK		5
#define WLAN_FC_STYPE_CFPOLL		6
#define WLAN_FC_STYPE_CFACKPOLL		7
#define WLAN_FC_STYPE_QOS_DATA		8
#define WLAN_FC_STYPE_QOS_DATA_CFACK	9
#define WLAN_FC_STYPE_QOS_DATA_CFPOLL	10
#define WLAN_FC_STYPE_QOS_DATA_CFACKPOLL 11
#define WLAN_FC_STYPE_QOS_NULLFUNC	12
#define WLAN_FC_STYPE_QOS_CFACK		13
#define WLAN_FC_STYPE_QOS_CFPOLL	14
#define WLAN_FC_STYPE_QOS_CFACKPOLL	15


#define IEEE80211_MAX_FRAG_THRESHOLD 2346
#define IEEE80211_MAX_RTS_THRESHOLD 2347

struct ieee80211_hdr {
	u16 frame_control;
	u16 duration_id;
	u8 addr1[6];
	u8 addr2[6];
	u8 addr3[6];
	u16 seq_ctrl;
	u8 addr4[6];
} __attribute__ ((packed));

/* return a pointer to the source address (SA) */
static inline u8 *ieee80211_get_SA(struct ieee80211_hdr *hdr)
{
	u8 *raw = (u8 *) hdr;
	u8 tofrom = (*(raw+1)) & 3; /* get the TODS and FROMDS bits */

	switch (tofrom) {
		case 2:
			return hdr->addr3;
		case 3:
			return hdr->addr4;
	}
	return hdr->addr2;
}

/* return a pointer to the destination address (DA) */
static inline u8 *ieee80211_get_DA(struct ieee80211_hdr *hdr)
{
	u8 *raw = (u8 *) hdr;
	u8 to_ds = (*(raw+1)) & 1; /* get the TODS bit */

	if (to_ds)
		return hdr->addr3;
	return hdr->addr1;
}

static inline int ieee80211_get_morefrag(struct ieee80211_hdr *hdr)
{
	return (le16_to_cpu(hdr->frame_control) & WLAN_FC_MOREFRAG) != 0;
}

#endif /* IEEE80211_H */
