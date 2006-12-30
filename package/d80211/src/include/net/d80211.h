/*
 * Low-level hardware driver -- IEEE 802.11 driver (80211.o) interface
 * Copyright 2002-2005, Devicescape Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef D80211_H
#define D80211_H

#include <linux/kernel.h>
#include <linux/if_ether.h>
#include <linux/skbuff.h>
#include <linux/wireless.h>
#include <linux/device.h>
#include "d80211_shared.h"

/* Note! Only ieee80211_tx_status_irqsafe() and ieee80211_rx_irqsafe() can be
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
 * with each TX frame. The low-level driver is responsible for configuring
 * the hardware to use given values (depending on what is supported). */
#define HW_KEY_IDX_INVALID -1

struct ieee80211_tx_control {
	enum { PKT_NORMAL = 0, PKT_PROBE_RESP } pkt_type;
	int tx_rate; /* Transmit rate, given as the hw specific value for the
		      * rate (from struct ieee80211_rate) */
	int rts_cts_rate; /* Transmit rate for RTS/CTS frame, given as the hw
			   * specific value for the rate (from
			   * struct ieee80211_rate) */

#define IEEE80211_TXCTL_REQ_TX_STATUS	(1<<0)/* request TX status callback for
						* this frame */
#define IEEE80211_TXCTL_DO_NOT_ENCRYPT	(1<<1) /* send this frame without
						* encryption; e.g., for EAPOL
						* frames */
#define IEEE80211_TXCTL_USE_RTS_CTS	(1<<2) /* use RTS-CTS before sending
						* frame */
#define IEEE80211_TXCTL_USE_CTS_PROTECT	(1<<3) /* use CTS protection for the
						* frame (e.g., for combined
						* 802.11g / 802.11b networks) */
#define IEEE80211_TXCTL_NO_ACK		(1<<4) /* tell the low level not to
						* wait for an ack */
#define IEEE80211_TXCTL_RATE_CTRL_PROBE	(1<<5)
#define IEEE80211_TXCTL_CLEAR_DST_MASK	(1<<6)
#define IEEE80211_TXCTL_REQUEUE		(1<<7)
#define IEEE80211_TXCTL_FIRST_FRAGMENT	(1<<8) /* this is a first fragment of
						* the frame */
#define IEEE80211_TXCTL_TKIP_NEW_PHASE1_KEY (1<<9)
	u32 flags;			       /* tx control flags defined
						* above */
	u16 rts_cts_duration;	/* duration field for RTS/CTS frame */
	u8 retry_limit;		/* 1 = only first attempt, 2 = one retry, .. */
	u8 power_level;		/* per-packet transmit power level, in dBm */
	u8 antenna_sel; 	/* 0 = default/diversity, 1 = Ant0, 2 = Ant1 */
	s8 key_idx;		/* -1 = do not encrypt, >= 0 keyidx from
				 * hw->set_key() */
	u8 icv_len;		/* length of the ICV/MIC field in octets */
	u8 iv_len;		/* length of the IV field in octets */
	u8 tkip_key[16];	/* generated phase2/phase1 key for hw TKIP */
	u8 queue;		/* hardware queue to use for this frame;
				 * 0 = highest, hw->queues-1 = lowest */
	u8 sw_retry_attempt;	/* number of times hw has tried to
				 * transmit frame (not incl. hw retries) */

	int rateidx; /* internal 80211.o rateidx */
	int alt_retry_rate; /* retry rate for the last retries, given as the
			     * hw specific value for the rate (from
			     * struct ieee80211_rate). To be used to limit
			     * packet dropping when probing higher rates, if hw
			     * supports multiple retry rates. -1 = not used */
	int type;	/* internal */
	int ifindex;	/* internal */
};

#define RX_FLAG_MMIC_ERROR       0x1
#define RX_FLAG_DECRYPTED        0x2

/* Receive status. The low-level driver should provide this information
 * (the subset supported by hardware) to the 802.11 code with each received
 * frame. */
struct ieee80211_rx_status {
        u64 hosttime;
	u64 mactime;
        int freq; /* receive frequency in Mhz */
        int channel;
        int phymode;
        int ssi;
	int signal;
	int noise;
        int antenna;
        int rate;
        int flag;
};

/* Transmit status. The low-level driver should provide this information
 * (the subset supported by hardware) to the 802.11 code for each transmit
 * frame. */
struct ieee80211_tx_status {
	/* copied ieee80211_tx_control structure */
	struct ieee80211_tx_control control;

#define IEEE80211_TX_STATUS_TX_FILTERED	(1<<0)
#define IEEE80211_TX_STATUS_ACK		(1<<1) /* whether the TX frame was ACKed */
	u32 flags;		/* tx staus flags defined above */

	int ack_signal; /* measured signal strength of the ACK frame */
	int excessive_retries;
	int retry_count;

	int queue_length;      /* information about TX queue */
	int queue_number;
};


/**
 * struct ieee80211_conf - configuration of the device
 *
 * This struct indicates how the driver shall configure the hardware.
 *
 * @radio_enabled: when zero, driver is required to switch off the radio.
 */
struct ieee80211_conf {
	int channel;			/* IEEE 802.11 channel number */
	int freq;			/* MHz */
	int channel_val;		/* hw specific value for the channel */

	int phymode;			/* MODE_IEEE80211A, .. */
        unsigned int regulatory_domain;
	int radio_enabled;

        int beacon_int;

#define IEEE80211_CONF_SHORT_SLOT_TIME	(1<<0) /* use IEEE 802.11g Short Slot
						* Time */
#define IEEE80211_CONF_SSID_HIDDEN	(1<<1) /* do not broadcast the ssid */
	u32 flags;			/* configuration flags defined above */

        u8 power_level;			/* transmit power limit for current
					 * regulatory domain; in dBm */
        u8 antenna_max;			/* maximum antenna gain */
	short tx_power_reduction; /* in 0.1 dBm */

	int antenna_sel;		/* default antenna conf:
					 *	0 = default/diversity,
			  		 *	1 = Ant0,
					 *	2 = Ant1 */

        int antenna_def;
        int antenna_mode;

	/* Following five fields are used for IEEE 802.11H */
	unsigned int radar_detect;
	unsigned int spect_mgmt;
	unsigned int quiet_duration; /* duration of quiet period */
	unsigned int quiet_offset; /* how far into the beacon is the quiet
				    * period */
	unsigned int quiet_period;
	u8 radar_firpwr_threshold;
	u8 radar_rssi_threshold;
	u8 pulse_height_threshold;
	u8 pulse_rssi_threshold;
	u8 pulse_inband_threshold;
};

/**
 * enum ieee80211_if_types - types of 802.11 network interfaces
 *
 * @IEEE80211_IF_TYPE_AP: interface in AP mode.
 * @IEEE80211_IF_TYPE_MGMT: special interface for communication with hostap
 *	daemon. Drivers should never see this type.
 * @IEEE80211_IF_TYPE_STA: interface in STA (client) mode.
 * @IEEE80211_IF_TYPE_IBSS: interface in IBSS (ad-hoc) mode.
 * @IEEE80211_IF_TYPE_MNTR: interface in monitor (rfmon) mode.
 * @IEEE80211_IF_TYPE_WDS: interface in WDS mode.
 * @IEEE80211_IF_TYPE_VLAN: not used.
 */
enum ieee80211_if_types {
	IEEE80211_IF_TYPE_AP = 0x00000000,
	IEEE80211_IF_TYPE_MGMT = 0x00000001,
	IEEE80211_IF_TYPE_STA = 0x00000002,
	IEEE80211_IF_TYPE_IBSS = 0x00000003,
	IEEE80211_IF_TYPE_MNTR = 0x00000004,
	IEEE80211_IF_TYPE_WDS = 0x5A580211,
	IEEE80211_IF_TYPE_VLAN = 0x00080211,
};

/**
 * struct ieee80211_if_init_conf - initial configuration of an interface
 *
 * @if_id: internal interface ID. This number has no particular meaning to
 *	drivers and the only allowed usage is to pass it to
 *	ieee80211_beacon_get() and ieee80211_get_buffered_bc() functions.
 *	This field is not valid for monitor interfaces
 *	(interfaces of %IEEE80211_IF_TYPE_MNTR type).
 * @type: one of &enum ieee80211_if_types constants. Determines the type of
 *	added/removed interface.
 * @mac_addr: pointer to MAC address of the interface. This pointer is valid
 *	until the interface is removed (i.e. it cannot be used after
 *	remove_interface() callback was called for this interface).
 *
 * This structure is used in add_interface() and remove_interface()
 * callbacks of &struct ieee80211_hw.
 */
struct ieee80211_if_init_conf {
	int if_id;
	int type;
	void *mac_addr;
};

/**
 * struct ieee80211_if_conf - configuration of an interface
 *
 * @type: type of the interface. This is always the same as was specified in
 *	&struct ieee80211_if_init_conf. The type of an interface never changes
 *	during the life of the interface; this field is present only for
 *	convenience.
 * @bssid: BSSID of the network we are associated to/creating.
 * @ssid: used (together with @ssid_len) by drivers for hardware that
 *	generate beacons independently. The pointer is valid only during the
 *	config_interface() call, so copy the value somewhere if you need
 *	it.
 * @ssid_len: length of the @ssid field.
 * @generic_elem: used (together with @generic_elem_len) by drivers for
 *	hardware that generate beacons independently. The pointer is valid
 *	only during the config_interface() call, so copy the value somewhere
 *	if you need it.
 * @generic_elem_len: length of the generic element.
 * @beacon: beacon template. Valid only if @host_gen_beacon_template in
 *	&struct ieee80211_hw is set. The driver is responsible of freeing
 *	the sk_buff.
 *
 * This structure is passed to the config_interface() callback of
 * &struct ieee80211_hw.
 */
struct ieee80211_if_conf {
	int type;
	u8 *bssid;
	u8 *ssid;
	size_t ssid_len;
	u8 *generic_elem;
	size_t generic_elem_len;
	struct sk_buff *beacon;
};

typedef enum { ALG_NONE, ALG_WEP, ALG_TKIP, ALG_CCMP, ALG_NULL }
ieee80211_key_alg;


struct ieee80211_key_conf {

	int hw_key_idx;			/* filled + used by low-level driver */
	ieee80211_key_alg alg;
	int keylen;

#define IEEE80211_KEY_FORCE_SW_ENCRYPT (1<<0) /* to be cleared by low-level
						 driver */
#define IEEE80211_KEY_DEFAULT_TX_KEY   (1<<1) /* This key is the new default TX
						 key (used only for broadcast
						 keys). */
#define IEEE80211_KEY_DEFAULT_WEP_ONLY (1<<2) /* static WEP is the only
						 configured security policy;
						 this allows some low-level
						 drivers to determine when
						 hwaccel can be used */
	u32 flags; /* key configuration flags defined above */

	s8 keyidx;			/* WEP key index */
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

#define IEEE80211_SEQ_COUNTER_RX	0
#define IEEE80211_SEQ_COUNTER_TX	1

typedef enum {
	SET_KEY, DISABLE_KEY, REMOVE_ALL_KEYS,
} set_key_cmd;

/* This is driver-visible part of the per-hw state the stack keeps.
 * If you change something in here, call ieee80211_update_hw() to
 * notify the stack about the change. */
struct ieee80211_hw {
	/* these are assigned by d80211, don't write */
	int index;
	struct ieee80211_conf conf;

	/* Pointer to the private area that was
	 * allocated with this struct for you. */
	void *priv;

	/* The rest is information about your hardware */

	struct device *dev;

	/* permanent mac address */
	u8 perm_addr[ETH_ALEN];

	/* TODO: frame_type 802.11/802.3, sw_encryption requirements */

	/* Some wireless LAN chipsets generate beacons in the hardware/firmware
	 * and others rely on host generated beacons. This option is used to
	 * configure the upper layer IEEE 802.11 module to generate beacons.
	 * The low-level driver can use ieee80211_beacon_get() to fetch the
	 * next beacon frame. */
#define IEEE80211_HW_HOST_GEN_BEACON (1<<0)

	/* The device needs to be supplied with a beacon template only. */
#define IEEE80211_HW_HOST_GEN_BEACON_TEMPLATE (1<<1)

	/* Some devices handle decryption internally and do not
	 * indicate whether the frame was encrypted (unencrypted frames
	 * will be dropped by the hardware, unless specifically allowed
	 * through) */
#define IEEE80211_HW_DEVICE_HIDES_WEP (1<<2)

	/* Whether RX frames passed to ieee80211_rx() include FCS in the end */
#define IEEE80211_HW_RX_INCLUDES_FCS (1<<3)

	/* Some wireless LAN chipsets buffer broadcast/multicast frames for
	 * power saving stations in the hardware/firmware and others rely on
	 * the host system for such buffering. This option is used to
	 * configure the IEEE 802.11 upper layer to buffer broadcast/multicast
	 * frames when there are power saving stations so that low-level driver
	 * can fetch them with ieee80211_get_buffered_bc(). */
#define IEEE80211_HW_HOST_BROADCAST_PS_BUFFERING (1<<4)

#define IEEE80211_HW_WEP_INCLUDE_IV (1<<5)

	/* will data nullfunc frames get proper TX status callback */
#define IEEE80211_HW_DATA_NULLFUNC_ACK (1<<6)

	/* Force software encryption for TKIP packets if WMM is enabled. */
#define IEEE80211_HW_NO_TKIP_WMM_HWACCEL (1<<7)

	/* Some devices handle Michael MIC internally and do not include MIC in
	 * the received packets passed up. device_strips_mic must be set
	 * for such devices. The 'encryption' frame control bit is expected to
	 * be still set in the IEEE 802.11 header with this option unlike with
	 * the device_hides_wep configuration option.
	 */
#define IEEE80211_HW_DEVICE_STRIPS_MIC (1<<8)

	/* Device is capable of performing full monitor mode even during
	 * normal operation. */
#define IEEE80211_HW_MONITOR_DURING_OPER (1<<9)

	/* Set if the low-level driver supports skb fraglist (NETIF_F_FRAGLIST),
	 * i.e. more than one skb per frame */
#define IEEE80211_HW_FRAGLIST (1<<10)

	/* calculate Michael MIC for an MSDU when doing hwcrypto */
#define IEEE80211_HW_TKIP_INCLUDE_MMIC (1<<12)
	/* Do TKIP phase1 key mixing in stack to support cards only do
	 * phase2 key mixing when doing hwcrypto */
#define IEEE80211_HW_TKIP_REQ_PHASE1_KEY (1<<13)
	/* Do TKIP phase1 and phase2 key mixing in stack and send the generated
	 * per-packet RC4 key with each TX frame when doing hwcrypto */
#define IEEE80211_HW_TKIP_REQ_PHASE2_KEY (1<<14)

	u32 flags;			/* hardware flags defined above */

	/* Set to the size of a needed device specific skb headroom for TX skbs. */
	unsigned int extra_tx_headroom;

        /* This is the time in us to change channels
         */
        int channel_change_time;
	/* This is maximum value for rssi reported by this device */
	int maxssi;

	int num_modes;
	struct ieee80211_hw_modes *modes;

	/* Number of available hardware TX queues for data packets.
	 * WMM requires at least four queues. */
	int queues;
};

/* Configuration block used by the low-level driver to tell the 802.11 code
 * about supported hardware features and to pass function pointers to callback
 * functions. */
struct ieee80211_ops {
	/* Handler that 802.11 module calls for each transmitted frame.
	 * skb contains the buffer starting from the IEEE 802.11 header.
	 * The low-level driver should send the frame out based on
	 * configuration in the TX control data. */
	int (*tx)(struct ieee80211_hw *hw, struct sk_buff *skb,
		  struct ieee80211_tx_control *control);

	/* Handler for performing hardware reset. */
	int (*reset)(struct ieee80211_hw *hw);

	/* Handler that is called when any netdevice attached to the hardware
	 * device is set UP for the first time. This can be used, e.g., to
	 * enable interrupts and beacon sending. */
	int (*open)(struct ieee80211_hw *hw);

	/* Handler that is called when the last netdevice attached to the
	 * hardware device is set DOWN. This can be used, e.g., to disable
	 * interrupts and beacon sending. */
	int (*stop)(struct ieee80211_hw *hw);

	/* Handler for asking a driver if a new interface can be added (or,
	 * more exactly, set UP). If the handler returns zero, the interface
	 * is added. Driver should perform any initialization it needs prior
	 * to returning zero. By returning non-zero addition of the interface
	 * is inhibited. Unless monitor_during_oper is set, it is guaranteed
	 * that monitor interfaces and normal interfaces are mutually
	 * exclusive. The open() handler is called after add_interface()
	 * if this is the first device added. At least one of the open()
	 * open() and add_interface() callbacks has to be assigned. If
	 * add_interface() is NULL, one STA interface is permitted only. */
	int (*add_interface)(struct ieee80211_hw *hw,
			     struct ieee80211_if_init_conf *conf);

	/* Notify a driver that an interface is going down. The stop() handler
	 * is called prior to this if this is a last interface. */
	void (*remove_interface)(struct ieee80211_hw *hw,
				 struct ieee80211_if_init_conf *conf);

	/* Handler for configuration requests. IEEE 802.11 code calls this
	 * function to change hardware configuration, e.g., channel. */
	int (*config)(struct ieee80211_hw *hw, struct ieee80211_conf *conf);

	/* Handler for configuration requests related to interfaces (e.g.
	 * BSSID). */
	int (*config_interface)(struct ieee80211_hw *hw,
				int if_id, struct ieee80211_if_conf *conf);

	/* ieee80211 drivers do not have access to the &struct net_device
	 * that is (are) connected with their device. Hence (and because
	 * we need to combine the multicast lists and flags for multiple
	 * virtual interfaces), they cannot assign set_multicast_list.
	 * The parameters here replace dev->flags and dev->mc_count,
	 * dev->mc_list is replaced by calling ieee80211_get_mc_list_item. */
	void (*set_multicast_list)(struct ieee80211_hw *hw,
				   unsigned short flags, int mc_count);

	/* Set TIM bit handler. If the hardware/firmware takes care of beacon
	 * generation, IEEE 802.11 code uses this function to tell the
	 * low-level to set (or clear if set==0) TIM bit for the given aid. If
	 * host system is used to generate beacons, this handler is not used
	 * and low-level driver should set it to NULL. */
	int (*set_tim)(struct ieee80211_hw *hw, int aid, int set);

	/* Set encryption key. IEEE 802.11 module calls this function to set
	 * encryption keys. addr is ff:ff:ff:ff:ff:ff for default keys and
	 * station hwaddr for individual keys. aid of the station is given
	 * to help low-level driver in selecting which key->hw_key_idx to use
	 * for this key. TX control data will use the hw_key_idx selected by
	 * the low-level driver. */
	int (*set_key)(struct ieee80211_hw *hw, set_key_cmd cmd,
		       u8 *addr, struct ieee80211_key_conf *key, int aid);

	/* Set TX key index for default/broadcast keys. This is needed in cases
	 * where wlan card is doing full WEP/TKIP encapsulation (wep_include_iv
	 * is not set), in other cases, this function pointer can be set to
	 * NULL since the IEEE 802. 11 module takes care of selecting the key
	 * index for each TX frame. */
	int (*set_key_idx)(struct ieee80211_hw *hw, int idx);

	/* Enable/disable IEEE 802.1X. This item requests wlan card to pass
	 * unencrypted EAPOL-Key frames even when encryption is configured.
	 * If the wlan card does not require such a configuration, this
	 * function pointer can be set to NULL. */
	int (*set_ieee8021x)(struct ieee80211_hw *hw, int use_ieee8021x);

	/* Set port authorization state (IEEE 802.1X PAE) to be authorized
	 * (authorized=1) or unauthorized (authorized=0). This function can be
	 * used if the wlan hardware or low-level driver implements PAE.
	 * 80211.o module will anyway filter frames based on authorization
	 * state, so this function pointer can be NULL if low-level driver does
	 * not require event notification about port state changes. */
	int (*set_port_auth)(struct ieee80211_hw *hw, u8 *addr,
			     int authorized);

        /* Ask the hardware to do a passive scan on a new channel. The hardware
         * will do what ever is required to nicely leave the current channel
         * including transmit any CTS packets, etc. */
	int (*passive_scan)(struct ieee80211_hw *hw, int state,
                            struct ieee80211_scan_conf *conf);

	/* Ask the hardware to service the scan request, no need to start
	 * the scan state machine in stack. */
	int (*hw_scan)(struct ieee80211_hw *hw, u8 *ssid, size_t len);

        /* return low-level statistics */
	int (*get_stats)(struct ieee80211_hw *hw,
			 struct ieee80211_low_level_stats *stats);

        /* Enable/disable test modes; mode = IEEE80211_TEST_* */
	int (*test_mode)(struct ieee80211_hw *hw, int mode);

	/* Configuration of test parameters */
	int (*test_param)(struct ieee80211_hw *hw, int param, int value);

        /* For devices that generate their own beacons and probe response
         * or association responses this updates the state of privacy_invoked
         * returns 0 for success or an error number */
	int (*set_privacy_invoked)(struct ieee80211_hw *hw,
				   int privacy_invoked);

	/* For devices that have internal sequence counters, allow 802.11
	 * code to access the current value of a counter */
	int (*get_sequence_counter)(struct ieee80211_hw *hw,
	                            u8* addr, u8 keyidx, u8 txrx,
				    u32* iv32, u16* iv16);

	/* Configuration of RTS threshold (if device needs it) */
	int (*set_rts_threshold)(struct ieee80211_hw *hw, u32 value);

	/* Configuration of fragmentation threshold.
	 * Assign this if the device does fragmentation by itself,
	 * if this method is assigned then the stack will not do
	 * fragmentation. */
	int (*set_frag_threshold)(struct ieee80211_hw *hw, u32 value);

	/* Configuration of retry limits (if device needs it) */
	int (*set_retry_limit)(struct ieee80211_hw *hw,
			       u32 short_retry, u32 long_retr);

	/* Number of STAs in STA table notification (NULL = disabled) */
	void (*sta_table_notification)(struct ieee80211_hw *hw,
				       int num_sta);

	/* Configure TX queue parameters (EDCF (aifs, cw_min, cw_max),
	 * bursting) for a hardware TX queue.
	 * queue = IEEE80211_TX_QUEUE_*. */
	int (*conf_tx)(struct ieee80211_hw *hw, int queue,
		       const struct ieee80211_tx_queue_params *params);

	/* Get statistics of the current TX queue status. This is used to get
	 * number of currently queued packets (queue length), maximum queue
	 * size (limit), and total number of packets sent using each TX queue
	 * (count). This information is used for WMM to find out which TX
	 * queues have room for more packets and by hostapd to provide
	 * statistics about the current queueing state to external programs. */
	int (*get_tx_stats)(struct ieee80211_hw *hw,
			    struct ieee80211_tx_queue_stats *stats);

	/* Get the current TSF timer value from firmware/hardware. Currently,
	 * this is only used for IBSS mode debugging and, as such, is not a
	 * required function. */
	u64 (*get_tsf)(struct ieee80211_hw *hw);

	/* Reset the TSF timer and allow firmware/hardware to synchronize with
	 * other STAs in the IBSS. This is only used in IBSS mode. This
	 * function is optional if the firmware/hardware takes full care of
	 * TSF synchronization. */
	void (*reset_tsf)(struct ieee80211_hw *hw);

	/* Setup beacon data for IBSS beacons. Unlike access point (Master),
	 * IBSS uses a fixed beacon frame which is configured using this
	 * function. This handler is required only for IBSS mode. */
	int (*beacon_update)(struct ieee80211_hw *hw,
			     struct sk_buff *skb,
			     struct ieee80211_tx_control *control);

	/* Determine whether the last IBSS beacon was sent by us. This is
	 * needed only for IBSS mode and the result of this function is used to
	 * determine whether to reply to Probe Requests. */
	int (*tx_last_beacon)(struct ieee80211_hw *hw);
};

/* Allocate a new hardware device. This must be called once for each
 * hardware device. The returned pointer must be used to refer to this
 * device when calling other functions. 802.11 code allocates a private data
 * area for the low-level driver. The size of this area is given as
 * priv_data_len.
 */
struct ieee80211_hw *ieee80211_alloc_hw(size_t priv_data_len,
					struct ieee80211_ops *ops);

/* Register hardware device to the IEEE 802.11 code and kernel. Low-level
 * drivers must call this function before using any other IEEE 802.11
 * function. */
int ieee80211_register_hw(struct ieee80211_hw *hw);

/* driver can use this and ieee80211_get_rx_led_name to get the
 * name of the registered LEDs after ieee80211_register_hw
 * was called.
 * This is useful to set the default trigger on the LED class
 * device that your driver should export for each LED the device
 * has, that way the default behaviour will be as expected but
 * the user can still change it/turn off the LED etc.
 */
#ifdef CONFIG_D80211_LEDS
extern char *__ieee80211_get_tx_led_name(struct ieee80211_hw *hw);
extern char *__ieee80211_get_rx_led_name(struct ieee80211_hw *hw);
#endif
static inline char *ieee80211_get_tx_led_name(struct ieee80211_hw *hw)
{
#ifdef CONFIG_D80211_LEDS
	return __ieee80211_get_tx_led_name(hw);
#else
	return NULL;
#endif
}

static inline char *ieee80211_get_rx_led_name(struct ieee80211_hw *hw)
{
#ifdef CONFIG_D80211_LEDS
	return __ieee80211_get_rx_led_name(hw);
#else
	return NULL;
#endif
}

/* Call this function if you changed the hardware description after
 * ieee80211_register_hw */
int ieee80211_update_hw(struct ieee80211_hw *hw);

/* Unregister a hardware device. This function instructs 802.11 code to free
 * allocated resources and unregister netdevices from the kernel. */
void ieee80211_unregister_hw(struct ieee80211_hw *hw);

/* Free everything that was allocated including private data of a driver. */
void ieee80211_free_hw(struct ieee80211_hw *hw);

/* Receive frame callback function. The low-level driver uses this function to
 * send received frames to the IEEE 802.11 code. Receive buffer (skb) must
 * start with IEEE 802.11 header. */
void __ieee80211_rx(struct ieee80211_hw *hw, struct sk_buff *skb,
		    struct ieee80211_rx_status *status);
void ieee80211_rx_irqsafe(struct ieee80211_hw *hw,
			  struct sk_buff *skb,
			  struct ieee80211_rx_status *status);

/* Transmit status callback function. The low-level driver must call this
 * function to report transmit status for all the TX frames that had
 * req_tx_status set in the transmit control fields. In addition, this should
 * be called at least for all unicast frames to provide information for TX rate
 * control algorithm. In order to maintain all statistics, this function is
 * recommended to be called after each frame, including multicast/broadcast, is
 * sent. */
void ieee80211_tx_status(struct ieee80211_hw *hw,
			 struct sk_buff *skb,
			 struct ieee80211_tx_status *status);
void ieee80211_tx_status_irqsafe(struct ieee80211_hw *hw,
				 struct sk_buff *skb,
				 struct ieee80211_tx_status *status);

/**
 * ieee80211_beacon_get - beacon generation function
 * @hw: pointer obtained from ieee80211_alloc_hw().
 * @if_id: interface ID from &struct ieee80211_if_init_conf.
 * @control: will be filled with information needed to send this beacon.
 *
 * If the beacon frames are generated by the host system (i.e., not in
 * hardware/firmware), the low-level driver uses this function to receive
 * the next beacon frame from the 802.11 code. The low-level is responsible
 * for calling this function before beacon data is needed (e.g., based on
 * hardware interrupt). Returned skb is used only once and low-level driver
 * is responsible of freeing it.
 */
struct sk_buff *ieee80211_beacon_get(struct ieee80211_hw *hw,
				     int if_id,
				     struct ieee80211_tx_control *control);

/**
 * ieee80211_get_buffered_bc - accessing buffered broadcast and multicast frames
 * @hw: pointer as obtained from ieee80211_alloc_hw().
 * @if_id: interface ID from &struct ieee80211_if_init_conf.
 * @control: will be filled with information needed to send returned frame.
 *
 * Function for accessing buffered broadcast and multicast frames. If
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
 * use common code for all beacons.
 */
struct sk_buff *
ieee80211_get_buffered_bc(struct ieee80211_hw *hw, int if_id,
			  struct ieee80211_tx_control *control);

/* Low level drivers that have their own MLME and MAC indicate
 * the aid for an associating station with this call */
int ieee80211_set_aid_for_sta(struct ieee80211_hw *hw,
			      u8 *peer_address, u16 aid);


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
/* This function is deprecated. */
typedef enum {
	NETIF_ATTACH, NETIF_DETACH, NETIF_START, NETIF_STOP, NETIF_WAKE,
	NETIF_IS_STOPPED, NETIF_UPDATE_TX_START
} Netif_Oper;
int ieee80211_netif_oper(struct ieee80211_hw *hw, Netif_Oper op);

/**
 * ieee80211_wake_queue - wake specific queue
 * @hw: pointer as obtained from ieee80211_alloc_hw().
 * @queue: queue number (counted from zero).
 *
 * Drivers should use this function instead of netif_wake_queue.
 */
void ieee80211_wake_queue(struct ieee80211_hw *hw, int queue);

/**
 * ieee80211_stop_queue - stop specific queue
 * @hw: pointer as obtained from ieee80211_alloc_hw().
 * @queue: queue number (counted from zero).
 *
 * Drivers should use this function instead of netif_stop_queue.
 */
void ieee80211_stop_queue(struct ieee80211_hw *hw, int queue);

/**
 * ieee80211_start_queues - start all queues
 * @hw: pointer to as obtained from ieee80211_alloc_hw().
 *
 * Drivers should use this function instead of netif_start_queue.
 */
void ieee80211_start_queues(struct ieee80211_hw *hw);

/**
 * ieee80211_stop_queues - stop all queues
 * @hw: pointer as obtained from ieee80211_alloc_hw().
 *
 * Drivers should use this function instead of netif_stop_queue.
 */
void ieee80211_stop_queues(struct ieee80211_hw *hw);

/**
 * ieee80211_get_mc_list_item - iteration over items in multicast list
 * @hw: pointer as obtained from ieee80211_alloc_hw().
 * @prev: value returned by previous call to ieee80211_get_mc_list_item() or
 *	NULL to start a new iteration.
 * @ptr: pointer to buffer of void * type for internal usage of
 *	ieee80211_get_mc_list_item().
 *
 * Iterates over items in multicast list of given device. To get the first
 * item, pass NULL in @prev and in *@ptr. In subsequent calls, pass the
 * value returned by previous call in @prev. Don't alter *@ptr during
 * iteration. When there are no more items, NULL is returned.
 */
struct dev_mc_list *
ieee80211_get_mc_list_item(struct ieee80211_hw *hw,
			   struct dev_mc_list *prev,
			   void **ptr);

/* called by driver to notify scan status completed */
void ieee80211_scan_completed(struct ieee80211_hw *hw);

/* Function to indicate Radar Detection. The low level driver must call this
 * function to indicate the presence of radar in the current channel.
 * Additionally the radar type also could be sent */
int  ieee80211_radar_status(struct ieee80211_hw *hw, int channel,
			    int radar, int radar_type);

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

/* IEEE 802.11 defines */

#define FCS_LEN 4

#define IEEE80211_DATA_LEN              2304
/* Maximum size for the MA-UNITDATA primitive, 802.11 standard section
   6.2.1.1.2.

   The figure in section 7.1.2 suggests a body size of up to 2312
   bytes is allowed, which is a bit confusing, I suspect this
   represents the 2304 bytes of real data, plus a possible 8 bytes of
   WEP IV and ICV. (this interpretation suggested by Ramiro Barreiro) */

#define IEEE80211_FCTL_VERS		0x0003
#define IEEE80211_FCTL_FTYPE		0x000c
#define IEEE80211_FCTL_STYPE		0x00f0
#define IEEE80211_FCTL_TODS		0x0100
#define IEEE80211_FCTL_FROMDS		0x0200
#define IEEE80211_FCTL_MOREFRAGS	0x0400
#define IEEE80211_FCTL_RETRY		0x0800
#define IEEE80211_FCTL_PM		0x1000
#define IEEE80211_FCTL_MOREDATA		0x2000
#define IEEE80211_FCTL_PROTECTED	0x4000
#define IEEE80211_FCTL_ORDER		0x8000

#define IEEE80211_SCTL_FRAG		0x000F
#define IEEE80211_SCTL_SEQ		0xFFF0

#define IEEE80211_FTYPE_MGMT		0x0000
#define IEEE80211_FTYPE_CTL		0x0004
#define IEEE80211_FTYPE_DATA		0x0008

/* management */
#define IEEE80211_STYPE_ASSOC_REQ	0x0000
#define IEEE80211_STYPE_ASSOC_RESP	0x0010
#define IEEE80211_STYPE_REASSOC_REQ	0x0020
#define IEEE80211_STYPE_REASSOC_RESP	0x0030
#define IEEE80211_STYPE_PROBE_REQ	0x0040
#define IEEE80211_STYPE_PROBE_RESP	0x0050
#define IEEE80211_STYPE_BEACON		0x0080
#define IEEE80211_STYPE_ATIM		0x0090
#define IEEE80211_STYPE_DISASSOC	0x00A0
#define IEEE80211_STYPE_AUTH		0x00B0
#define IEEE80211_STYPE_DEAUTH		0x00C0
#define IEEE80211_STYPE_ACTION		0x00D0

/* control */
#define IEEE80211_STYPE_PSPOLL		0x00A0
#define IEEE80211_STYPE_RTS		0x00B0
#define IEEE80211_STYPE_CTS		0x00C0
#define IEEE80211_STYPE_ACK		0x00D0
#define IEEE80211_STYPE_CFEND		0x00E0
#define IEEE80211_STYPE_CFENDACK	0x00F0

/* data */
#define IEEE80211_STYPE_DATA			0x0000
#define IEEE80211_STYPE_DATA_CFACK		0x0010
#define IEEE80211_STYPE_DATA_CFPOLL		0x0020
#define IEEE80211_STYPE_DATA_CFACKPOLL		0x0030
#define IEEE80211_STYPE_NULLFUNC		0x0040
#define IEEE80211_STYPE_CFACK			0x0050
#define IEEE80211_STYPE_CFPOLL			0x0060
#define IEEE80211_STYPE_CFACKPOLL		0x0070
#define IEEE80211_STYPE_QOS_DATA		0x0080
#define IEEE80211_STYPE_QOS_DATA_CFACK		0x0090
#define IEEE80211_STYPE_QOS_DATA_CFPOLL		0x00A0
#define IEEE80211_STYPE_QOS_DATA_CFACKPOLL	0x00B0
#define IEEE80211_STYPE_QOS_NULLFUNC		0x00C0
#define IEEE80211_STYPE_QOS_CFACK		0x00D0
#define IEEE80211_STYPE_QOS_CFPOLL		0x00E0
#define IEEE80211_STYPE_QOS_CFACKPOLL		0x00F0


/* miscellaneous IEEE 802.11 constants */
#define IEEE80211_MAX_FRAG_THRESHOLD 2346
#define IEEE80211_MAX_RTS_THRESHOLD 2347
#define IEEE80211_MAX_AID 2007
#define IEEE80211_MAX_TIM_LEN 251

struct ieee80211_hdr {
	__le16 frame_control;
	__le16 duration_id;
	u8 addr1[6];
	u8 addr2[6];
	u8 addr3[6];
	__le16 seq_ctrl;
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
	return (le16_to_cpu(hdr->frame_control) &
		IEEE80211_FCTL_MOREFRAGS) != 0;
}

#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(x) ((u8*)(x))[0], ((u8*)(x))[1], ((u8*)(x))[2], \
		   ((u8*)(x))[3], ((u8*)(x))[4], ((u8*)(x))[5]

#endif /* D80211_H */
