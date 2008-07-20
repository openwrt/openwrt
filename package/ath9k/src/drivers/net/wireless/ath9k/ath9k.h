/*
 * Copyright (c) 2008 Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef ATH9K_H
#define ATH9K_H

#include <linux/io.h>

#define ATHEROS_VENDOR_ID	0x168c

#define AR5416_DEVID_PCI	0x0023
#define AR5416_DEVID_PCIE	0x0024
#define AR9160_DEVID_PCI	0x0027
#define AR9280_DEVID_PCI	0x0029
#define AR9280_DEVID_PCIE	0x002a

#define AR5416_AR9100_DEVID	0x000b

#define	AR_SUBVENDOR_ID_NOG	0x0e11
#define AR_SUBVENDOR_ID_NEW_A	0x7065

#define	HAL_TXERR_XRETRY        0x01
#define	HAL_TXERR_FILT	        0x02
#define	HAL_TXERR_FIFO		0x04
#define HAL_TXERR_XTXOP		0x08
#define HAL_TXERR_TIMER_EXPIRED 0x10

#define HAL_TX_BA               0x01
#define HAL_TX_PWRMGMT          0x02
#define HAL_TX_DESC_CFG_ERR     0x04
#define HAL_TX_DATA_UNDERRUN    0x08
#define HAL_TX_DELIM_UNDERRUN   0x10
#define HAL_TX_SW_ABORTED       0x40
#define HAL_TX_SW_FILTERED      0x80

#define NBBY    8
#ifndef howmany
#define howmany(x, y)   (((x)+((y)-1))/(y))
#endif

struct ath_tx_status {
	u_int32_t ts_tstamp;
	u_int16_t ts_seqnum;
	u_int8_t ts_status;
	u_int8_t ts_ratecode;
	u_int8_t ts_rateindex;
	int8_t ts_rssi;
	u_int8_t ts_shortretry;
	u_int8_t ts_longretry;
	u_int8_t ts_virtcol;
	u_int8_t ts_antenna;
	u_int8_t ts_flags;
	int8_t ts_rssi_ctl0;
	int8_t ts_rssi_ctl1;
	int8_t ts_rssi_ctl2;
	int8_t ts_rssi_ext0;
	int8_t ts_rssi_ext1;
	int8_t ts_rssi_ext2;
	u_int8_t pad[3];
	u_int32_t ba_low;
	u_int32_t ba_high;
	u_int32_t evm0;
	u_int32_t evm1;
	u_int32_t evm2;
};

struct ath_rx_status {
	u_int32_t rs_tstamp;
	u_int16_t rs_datalen;
	u_int8_t rs_status;
	u_int8_t rs_phyerr;
	int8_t rs_rssi;
	u_int8_t rs_keyix;
	u_int8_t rs_rate;
	u_int8_t rs_antenna;
	u_int8_t rs_more;
	int8_t rs_rssi_ctl0;
	int8_t rs_rssi_ctl1;
	int8_t rs_rssi_ctl2;
	int8_t rs_rssi_ext0;
	int8_t rs_rssi_ext1;
	int8_t rs_rssi_ext2;
	u_int8_t rs_isaggr;
	u_int8_t rs_moreaggr;
	u_int8_t rs_num_delims;
	u_int8_t rs_flags;
	u_int32_t evm0;
	u_int32_t evm1;
	u_int32_t evm2;
};

#define	HAL_RXERR_CRC		0x01
#define	HAL_RXERR_PHY		0x02
#define	HAL_RXERR_FIFO		0x04
#define	HAL_RXERR_DECRYPT	0x08
#define	HAL_RXERR_MIC		0x10

#define HAL_RX_MORE             0x01
#define HAL_RX_MORE_AGGR        0x02
#define HAL_RX_GI               0x04
#define HAL_RX_2040             0x08
#define HAL_RX_DELIM_CRC_PRE    0x10
#define HAL_RX_DELIM_CRC_POST   0x20
#define HAL_RX_DECRYPT_BUSY     0x40

enum hal_bool {
	AH_FALSE = 0,
	AH_TRUE = 1,
};

#define	HAL_RXKEYIX_INVALID	((u_int8_t)-1)
#define	HAL_TXKEYIX_INVALID	((u_int)-1)

struct ath_desc {
	u_int32_t ds_link;
	u_int32_t ds_data;
	u_int32_t ds_ctl0;
	u_int32_t ds_ctl1;
	u_int32_t ds_hw[20];
	union {
		struct ath_tx_status tx;
		struct ath_rx_status rx;
		void *stats;
	} ds_us;
	void *ds_vdata;
} __packed;

#define	ds_txstat	ds_us.tx
#define	ds_rxstat	ds_us.rx
#define ds_stat		ds_us.stats

#define	HAL_TXDESC_CLRDMASK	0x0001
#define	HAL_TXDESC_NOACK	0x0002
#define	HAL_TXDESC_RTSENA	0x0004
#define	HAL_TXDESC_CTSENA	0x0008
#define	HAL_TXDESC_INTREQ	0x0010
#define	HAL_TXDESC_VEOL		0x0020
#define HAL_TXDESC_EXT_ONLY     0x0040
#define HAL_TXDESC_EXT_AND_CTL  0x0080
#define HAL_TXDESC_VMF          0x0100
#define HAL_TXDESC_FRAG_IS_ON   0x0200

#define	HAL_RXDESC_INTREQ	0x0020

enum hal_capability_type {
	HAL_CAP_CIPHER = 0,
	HAL_CAP_TKIP_MIC,
	HAL_CAP_TKIP_SPLIT,
	HAL_CAP_PHYCOUNTERS,
	HAL_CAP_DIVERSITY,
	HAL_CAP_PSPOLL,
	HAL_CAP_TXPOW,
	HAL_CAP_PHYDIAG,
	HAL_CAP_MCAST_KEYSRCH,
	HAL_CAP_TSF_ADJUST,
	HAL_CAP_WME_TKIPMIC,
	HAL_CAP_RFSILENT,
	HAL_CAP_ANT_CFG_2GHZ,
	HAL_CAP_ANT_CFG_5GHZ
};

struct hal_capabilities {
	u_int halChanSpreadSupport:1,
		halChapTuningSupport:1,
		halMicAesCcmSupport:1,
		halMicCkipSupport:1,
		halMicTkipSupport:1,
		halCipherAesCcmSupport:1,
		halCipherCkipSupport:1,
		halCipherTkipSupport:1,
		halVEOLSupport:1,
		halBssIdMaskSupport:1,
		halMcastKeySrchSupport:1,
		halTsfAddSupport:1,
		halChanHalfRate:1,
		halChanQuarterRate:1,
		halHTSupport:1,
		halGTTSupport:1,
		halFastCCSupport:1,
		halRfSilentSupport:1,
		halWowSupport:1,
		halCSTSupport:1,
		halEnhancedPmSupport:1,
		halAutoSleepSupport:1,
		hal4kbSplitTransSupport:1,
		halWowMatchPatternExact:1;
	u_int32_t halWirelessModes;
	u_int16_t halTotalQueues;
	u_int16_t halKeyCacheSize;
	u_int16_t halLow5GhzChan, halHigh5GhzChan;
	u_int16_t halLow2GhzChan, halHigh2GhzChan;
	u_int16_t halNumMRRetries;
	u_int16_t halRtsAggrLimit;
	u_int8_t halTxChainMask;
	u_int8_t halRxChainMask;
	u_int16_t halTxTrigLevelMax;
	u_int16_t halRegCap;
	u_int8_t halNumGpioPins;
	u_int8_t halNumAntCfg2GHz;
	u_int8_t halNumAntCfg5GHz;
};

struct hal_ops_config {
	int ath_hal_dma_beacon_response_time;
	int ath_hal_sw_beacon_response_time;
	int ath_hal_additional_swba_backoff;
	int ath_hal_6mb_ack;
	int ath_hal_cwmIgnoreExtCCA;
	u_int8_t ath_hal_pciePowerSaveEnable;
	u_int8_t ath_hal_pcieL1SKPEnable;
	u_int8_t ath_hal_pcieClockReq;
	u_int32_t ath_hal_pcieWaen;
	int ath_hal_pciePowerReset;
	u_int8_t ath_hal_pcieRestore;
	u_int8_t ath_hal_analogShiftReg;
	u_int8_t ath_hal_htEnable;
	u_int32_t ath_hal_ofdmTrigLow;
	u_int32_t ath_hal_ofdmTrigHigh;
	u_int32_t ath_hal_cckTrigHigh;
	u_int32_t ath_hal_cckTrigLow;
	u_int32_t ath_hal_enableANI;
	u_int8_t ath_hal_noiseImmunityLvl;
	u_int32_t ath_hal_ofdmWeakSigDet;
	u_int32_t ath_hal_cckWeakSigThr;
	u_int8_t ath_hal_spurImmunityLvl;
	u_int8_t ath_hal_firStepLvl;
	int8_t ath_hal_rssiThrHigh;
	int8_t ath_hal_rssiThrLow;
	u_int16_t ath_hal_diversityControl;
	u_int16_t ath_hal_antennaSwitchSwap;
	int ath_hal_serializeRegMode;
	int ath_hal_intrMitigation;
	int ath_hal_debug;
#define SPUR_DISABLE        	0
#define SPUR_ENABLE_IOCTL   	1
#define SPUR_ENABLE_EEPROM  	2
#define AR_EEPROM_MODAL_SPURS   5
#define AR_SPUR_5413_1      	1640
#define AR_SPUR_5413_2      	1200
#define AR_NO_SPUR      	0x8000
#define AR_BASE_FREQ_2GHZ   	2300
#define AR_BASE_FREQ_5GHZ   	4900
#define AR_SPUR_FEEQ_BOUND_HT40 19
#define AR_SPUR_FEEQ_BOUND_HT20 10
	int ath_hal_spurMode;
	u_int16_t ath_hal_spurChans[AR_EEPROM_MODAL_SPURS][2];
};

enum hal_tx_queue {
	HAL_TX_QUEUE_INACTIVE = 0,
	HAL_TX_QUEUE_DATA,
	HAL_TX_QUEUE_BEACON,
	HAL_TX_QUEUE_CAB,
	HAL_TX_QUEUE_UAPSD,
	HAL_TX_QUEUE_PSPOLL
};

#define	HAL_NUM_TX_QUEUES 10

enum hal_tx_queue_subtype {
	HAL_WME_AC_BK = 0,
	HAL_WME_AC_BE,
	HAL_WME_AC_VI,
	HAL_WME_AC_VO,
	HAL_WME_UPSD
};

enum hal_tx_queue_flags {
	TXQ_FLAG_TXOKINT_ENABLE = 0x0001,
	TXQ_FLAG_TXERRINT_ENABLE = 0x0001,
	TXQ_FLAG_TXDESCINT_ENABLE = 0x0002,
	TXQ_FLAG_TXEOLINT_ENABLE = 0x0004,
	TXQ_FLAG_TXURNINT_ENABLE = 0x0008,
	TXQ_FLAG_BACKOFF_DISABLE = 0x0010,
	TXQ_FLAG_COMPRESSION_ENABLE = 0x0020,
	TXQ_FLAG_RDYTIME_EXP_POLICY_ENABLE = 0x0040,
	TXQ_FLAG_FRAG_BURST_BACKOFF_ENABLE = 0x0080,
};

struct hal_txq_info {
	u_int32_t tqi_ver;
	enum hal_tx_queue_subtype tqi_subtype;
	enum hal_tx_queue_flags tqi_qflags;
	u_int32_t tqi_priority;
	u_int32_t tqi_aifs;
	u_int32_t tqi_cwmin;
	u_int32_t tqi_cwmax;
	u_int16_t tqi_shretry;
	u_int16_t tqi_lgretry;
	u_int32_t tqi_cbrPeriod;
	u_int32_t tqi_cbrOverflowLimit;
	u_int32_t tqi_burstTime;
	u_int32_t tqi_readyTime;
	u_int32_t tqi_compBuf;
};

#define HAL_TQI_NONVAL 0xffff

#define	HAL_TXQ_USEDEFAULT ((u_int32_t) -1)

#define HAL_COMP_BUF_MAX_SIZE    9216
#define HAL_COMP_BUF_ALIGN_SIZE  512
#define HAL_DECOMP_MASK_SIZE     128

#define HAL_READY_TIME_LO_BOUND  50
#define HAL_READY_TIME_HI_BOUND  96

enum hal_pkt_type {
	HAL_PKT_TYPE_NORMAL = 0,
	HAL_PKT_TYPE_ATIM,
	HAL_PKT_TYPE_PSPOLL,
	HAL_PKT_TYPE_BEACON,
	HAL_PKT_TYPE_PROBE_RESP,
	HAL_PKT_TYPE_CHIRP,
	HAL_PKT_TYPE_GRP_POLL,
};

struct hal_tx_queue_info {
	u_int32_t tqi_ver;
	enum hal_tx_queue tqi_type;
	enum hal_tx_queue_subtype tqi_subtype;
	enum hal_tx_queue_flags tqi_qflags;
	u_int32_t tqi_priority;
	u_int32_t tqi_aifs;
	u_int32_t tqi_cwmin;
	u_int32_t tqi_cwmax;
	u_int16_t tqi_shretry;
	u_int16_t tqi_lgretry;
	u_int32_t tqi_cbrPeriod;
	u_int32_t tqi_cbrOverflowLimit;
	u_int32_t tqi_burstTime;
	u_int32_t tqi_readyTime;
	u_int32_t tqi_physCompBuf;
	u_int32_t tqi_intFlags;
};

enum hal_rx_filter {
	HAL_RX_FILTER_UCAST = 0x00000001,
	HAL_RX_FILTER_MCAST = 0x00000002,
	HAL_RX_FILTER_BCAST = 0x00000004,
	HAL_RX_FILTER_CONTROL = 0x00000008,
	HAL_RX_FILTER_BEACON = 0x00000010,
	HAL_RX_FILTER_PROM = 0x00000020,
	HAL_RX_FILTER_PROBEREQ = 0x00000080,
	HAL_RX_FILTER_PSPOLL = 0x00004000,
	HAL_RX_FILTER_PHYERR = 0x00000100,
	HAL_RX_FILTER_PHYRADAR = 0x00002000,
};

enum hal_int {
	HAL_INT_RX = 0x00000001,
	HAL_INT_RXDESC = 0x00000002,
	HAL_INT_RXNOFRM = 0x00000008,
	HAL_INT_RXEOL = 0x00000010,
	HAL_INT_RXORN = 0x00000020,
	HAL_INT_TX = 0x00000040,
	HAL_INT_TXDESC = 0x00000080,
	HAL_INT_TIM_TIMER = 0x00000100,
	HAL_INT_TXURN = 0x00000800,
	HAL_INT_MIB = 0x00001000,
	HAL_INT_RXPHY = 0x00004000,
	HAL_INT_RXKCM = 0x00008000,
	HAL_INT_SWBA = 0x00010000,
	HAL_INT_BMISS = 0x00040000,
	HAL_INT_BNR = 0x00100000,
	HAL_INT_TIM = 0x00200000,
	HAL_INT_DTIM = 0x00400000,
	HAL_INT_DTIMSYNC = 0x00800000,
	HAL_INT_GPIO = 0x01000000,
	HAL_INT_CABEND = 0x02000000,
	HAL_INT_CST = 0x10000000,
	HAL_INT_GTT = 0x20000000,
	HAL_INT_FATAL = 0x40000000,
	HAL_INT_GLOBAL = 0x80000000,
	HAL_INT_BMISC = HAL_INT_TIM
	    | HAL_INT_DTIM | HAL_INT_DTIMSYNC | HAL_INT_CABEND,
	HAL_INT_COMMON = HAL_INT_RXNOFRM
	    | HAL_INT_RXDESC
	    | HAL_INT_RXEOL
	    | HAL_INT_RXORN
	    | HAL_INT_TXURN
	    | HAL_INT_TXDESC
	    | HAL_INT_MIB
	    | HAL_INT_RXPHY
	    | HAL_INT_RXKCM | HAL_INT_SWBA | HAL_INT_BMISS | HAL_INT_GPIO,
	HAL_INT_NOCARD = 0xffffffff
};

struct hal_rate_table {
	int rateCount;
	u_int8_t rateCodeToIndex[256];
	struct {
		u_int8_t valid;
		u_int8_t phy;
		u_int32_t rateKbps;
		u_int8_t rateCode;
		u_int8_t shortPreamble;
		u_int8_t dot11Rate;
		u_int8_t controlRate;
		u_int16_t lpAckDuration;
		u_int16_t spAckDuration;
	} info[32];
};

#define HAL_RATESERIES_RTS_CTS  0x0001
#define HAL_RATESERIES_2040     0x0002
#define HAL_RATESERIES_HALFGI   0x0004

struct hal_11n_rate_series {
	u_int Tries;
	u_int Rate;
	u_int PktDuration;
	u_int ChSel;
	u_int RateFlags;
};

struct hal_channel {
	u_int16_t channel;
	u_int32_t channelFlags;
	u_int8_t privFlags;
	int8_t maxRegTxPower;
	int8_t maxTxPower;
	int8_t minTxPower;
};

#define CHANNEL_CW_INT    0x00002
#define CHANNEL_CCK       0x00020
#define CHANNEL_OFDM      0x00040
#define CHANNEL_2GHZ      0x00080
#define CHANNEL_5GHZ      0x00100
#define CHANNEL_PASSIVE   0x00200
#define CHANNEL_DYN       0x00400
#define CHANNEL_HALF      0x04000
#define CHANNEL_QUARTER   0x08000
#define CHANNEL_HT20      0x10000
#define CHANNEL_HT40PLUS  0x20000
#define CHANNEL_HT40MINUS 0x40000

#define CHANNEL_INTERFERENCE    0x01
#define CHANNEL_DFS             0x02
#define CHANNEL_4MS_LIMIT       0x04
#define CHANNEL_DFS_CLEAR       0x08
#define CHANNEL_DISALLOW_ADHOC  0x10
#define CHANNEL_PER_11D_ADHOC   0x20

#define CHANNEL_A           (CHANNEL_5GHZ|CHANNEL_OFDM)
#define CHANNEL_B           (CHANNEL_2GHZ|CHANNEL_CCK)
#define CHANNEL_PUREG       (CHANNEL_2GHZ|CHANNEL_OFDM)
#define CHANNEL_G           (CHANNEL_2GHZ|CHANNEL_OFDM)
#define CHANNEL_G_HT20      (CHANNEL_2GHZ|CHANNEL_HT20)
#define CHANNEL_A_HT20      (CHANNEL_5GHZ|CHANNEL_HT20)
#define CHANNEL_G_HT40PLUS  (CHANNEL_2GHZ|CHANNEL_HT40PLUS)
#define CHANNEL_G_HT40MINUS (CHANNEL_2GHZ|CHANNEL_HT40MINUS)
#define CHANNEL_A_HT40PLUS  (CHANNEL_5GHZ|CHANNEL_HT40PLUS)
#define CHANNEL_A_HT40MINUS (CHANNEL_5GHZ|CHANNEL_HT40MINUS)
#define CHANNEL_ALL				\
	(CHANNEL_OFDM|				\
	 CHANNEL_CCK|				\
	 CHANNEL_2GHZ |				\
	 CHANNEL_5GHZ |				\
	 CHANNEL_HT20 |				\
	 CHANNEL_HT40PLUS |			\
	 CHANNEL_HT40MINUS)

struct hal_channel_internal {
	u_int16_t channel;
	u_int32_t channelFlags;
	u_int8_t privFlags;
	int8_t maxRegTxPower;
	int8_t maxTxPower;
	int8_t minTxPower;
	enum hal_bool bssSendHere;
	u_int8_t gainI;
	enum hal_bool iqCalValid;
	int32_t CalValid;
	enum hal_bool oneTimeCalsDone;
	int8_t iCoff;
	int8_t qCoff;
	int16_t rawNoiseFloor;
	int16_t noiseFloorAdjust;
	int8_t antennaMax;
	u_int32_t regDmnFlags;
	u_int32_t conformanceTestLimit;
	u_int64_t ah_tsf_last;
	u_int64_t ah_channel_time;
	u_int16_t mainSpur;
	u_int64_t dfsTsf;
#ifdef ATH_NF_PER_CHAN
	struct hal_nfcal_hist nfCalHist[NUM_NF_READINGS];
#endif
};

#define HAL_SPUR_VAL_MASK       0x3FFF
#define HAL_SPUR_CHAN_WIDTH     87
#define HAL_BIN_WIDTH_BASE_100HZ    3125
#define HAL_MAX_BINS_ALLOWED        28

#define IS_CHAN_A(_c) ((((_c)->channelFlags & CHANNEL_A) == CHANNEL_A) || \
       (((_c)->channelFlags & CHANNEL_A_HT20) == CHANNEL_A_HT20) || \
       (((_c)->channelFlags & CHANNEL_A_HT40PLUS) == CHANNEL_A_HT40PLUS) || \
       (((_c)->channelFlags & CHANNEL_A_HT40MINUS) == CHANNEL_A_HT40MINUS))
#define IS_CHAN_B(_c) (((_c)->channelFlags & CHANNEL_B) == CHANNEL_B)
#define IS_CHAN_G(_c) ((((_c)->channelFlags & (CHANNEL_G)) == CHANNEL_G) || \
       (((_c)->channelFlags & CHANNEL_G_HT20) == CHANNEL_G_HT20) || \
       (((_c)->channelFlags & CHANNEL_G_HT40PLUS) == CHANNEL_G_HT40PLUS) || \
       (((_c)->channelFlags & CHANNEL_G_HT40MINUS) == CHANNEL_G_HT40MINUS))
#define IS_CHAN_PUREG(_c) \
	(((_c)->channelFlags & CHANNEL_PUREG) == CHANNEL_PUREG)
#define IS_CHAN_CCK(_c) (((_c)->channelFlags & CHANNEL_CCK) != 0)
#define IS_CHAN_OFDM(_c) (((_c)->channelFlags & CHANNEL_OFDM) != 0)
#define IS_CHAN_5GHZ(_c) (((_c)->channelFlags & CHANNEL_5GHZ) != 0)
#define IS_CHAN_2GHZ(_c) (((_c)->channelFlags & CHANNEL_2GHZ) != 0)
#define IS_CHAN_PASSIVE(_c) (((_c)->channelFlags & CHANNEL_PASSIVE) != 0)
#define IS_CHAN_HALF_RATE(_c) (((_c)->channelFlags & CHANNEL_HALF) != 0)
#define IS_CHAN_QUARTER_RATE(_c) (((_c)->channelFlags & CHANNEL_QUARTER) != 0)
#define IS_CHAN_HT20(_c) (((_c)->channelFlags & CHANNEL_HT20) != 0)
#define IS_CHAN_HT40(_c) ((((_c)->channelFlags & CHANNEL_HT40PLUS) != 0) \
			  || (((_c)->channelFlags & CHANNEL_HT40MINUS) != 0))
#define IS_CHAN_HT(_c) (IS_CHAN_HT20((_c)) || IS_CHAN_HT40((_c)))
#define IS_CHAN_IN_PUBLIC_SAFETY_BAND(_c) ((_c) > 4940 && (_c) < 4990)
#define IS_CHAN_A_5MHZ_SPACED(_c) \
	((((_c)->channelFlags & CHANNEL_5GHZ) != 0) && \
	(((_c)->channel % 20) != 0) &&	\
	(((_c)->channel % 10) != 0))

struct hal_keyval {
	u_int8_t kv_type;
	u_int8_t kv_pad;
	u_int16_t kv_len;
	u_int8_t kv_val[16];
	u_int8_t kv_mic[8];
	u_int8_t kv_txmic[8];
};

enum hal_key_type {
	HAL_KEY_TYPE_CLEAR,
	HAL_KEY_TYPE_WEP,
	HAL_KEY_TYPE_AES,
	HAL_KEY_TYPE_TKIP,
};

enum hal_cipher {
	HAL_CIPHER_WEP = 0,
	HAL_CIPHER_AES_OCB = 1,
	HAL_CIPHER_AES_CCM = 2,
	HAL_CIPHER_CKIP = 3,
	HAL_CIPHER_TKIP = 4,
	HAL_CIPHER_CLR = 5,
	HAL_CIPHER_MIC = 127
};

#define AR_EEPROM_EEPCAP_COMPRESS_DIS   0x0001
#define AR_EEPROM_EEPCAP_AES_DIS        0x0002
#define AR_EEPROM_EEPCAP_FASTFRAME_DIS  0x0004
#define AR_EEPROM_EEPCAP_BURST_DIS      0x0008
#define AR_EEPROM_EEPCAP_MAXQCU         0x01F0
#define AR_EEPROM_EEPCAP_MAXQCU_S       4
#define AR_EEPROM_EEPCAP_HEAVY_CLIP_EN  0x0200
#define AR_EEPROM_EEPCAP_KC_ENTRIES     0xF000
#define AR_EEPROM_EEPCAP_KC_ENTRIES_S   12

#define AR_EEPROM_EEREGCAP_EN_FCC_MIDBAND   0x0040
#define AR_EEPROM_EEREGCAP_EN_KK_U1_EVEN    0x0080
#define AR_EEPROM_EEREGCAP_EN_KK_U2         0x0100
#define AR_EEPROM_EEREGCAP_EN_KK_MIDBAND    0x0200
#define AR_EEPROM_EEREGCAP_EN_KK_U1_ODD     0x0400
#define AR_EEPROM_EEREGCAP_EN_KK_NEW_11A    0x0800

#define AR_EEPROM_EEREGCAP_EN_KK_U1_ODD_PRE4_0  0x4000
#define AR_EEPROM_EEREGCAP_EN_KK_NEW_11A_PRE4_0 0x8000

#define SD_NO_CTL               0xE0
#define NO_CTL                  0xff
#define CTL_MODE_M              7
#define CTL_11A                 0
#define CTL_11B                 1
#define CTL_11G                 2
#define CTL_2GHT20              5
#define CTL_5GHT20              6
#define CTL_2GHT40              7
#define CTL_5GHT40              8

#define AR_EEPROM_MAC(i)        (0x1d+(i))
#define EEP_SCALE       100
#define EEP_DELTA       10

#define AR_EEPROM_RFSILENT_GPIO_SEL     0x001c
#define AR_EEPROM_RFSILENT_GPIO_SEL_S   2
#define AR_EEPROM_RFSILENT_POLARITY     0x0002
#define AR_EEPROM_RFSILENT_POLARITY_S   1

enum {
	CTRY_DEBUG = 0x1ff,
	CTRY_DEFAULT = 0
};

enum reg_ext_bitmap {
	REG_EXT_JAPAN_MIDBAND = 1,
	REG_EXT_FCC_DFS_HT40 = 2,
	REG_EXT_JAPAN_NONDFS_HT40 = 3,
	REG_EXT_JAPAN_DFS_HT40 = 4
};

struct hal_country_entry {
	u_int16_t countryCode;
	u_int16_t regDmnEnum;
	u_int16_t regDmn5G;
	u_int16_t regDmn2G;
	u_int8_t isMultidomain;
	u_int8_t iso[3];
};

#define HAL_DBG_RESET           0x00000001
#define HAL_DBG_PHY_IO          0x00000002
#define HAL_DBG_REG_IO          0x00000004
#define HAL_DBG_RF_PARAM        0x00000008
#define HAL_DBG_QUEUE           0x00000010
#define HAL_DBG_EEPROM_DUMP     0x00000020
#define HAL_DBG_EEPROM          0x00000040
#define HAL_DBG_NF_CAL          0x00000080
#define HAL_DBG_CALIBRATE       0x00000100
#define HAL_DBG_CHANNEL         0x00000200
#define HAL_DBG_INTERRUPT       0x00000400
#define HAL_DBG_DFS             0x00000800
#define HAL_DBG_DMA             0x00001000
#define HAL_DBG_REGULATORY      0x00002000
#define HAL_DBG_TX              0x00004000
#define HAL_DBG_TXDESC          0x00008000
#define HAL_DBG_RX              0x00010000
#define HAL_DBG_RXDESC          0x00020000
#define HAL_DBG_ANI             0x00040000
#define HAL_DBG_BEACON          0x00080000
#define HAL_DBG_KEYCACHE        0x00100000
#define HAL_DBG_POWER_MGMT      0x00200000
#define HAL_DBG_MALLOC		0x00400000
#define HAL_DBG_POWER_OVERRIDE	0x01000000
#define HAL_DBG_SPUR_MITIGATE	0x02000000
#define HAL_DBG_UNMASKABLE      0xFFFFFFFF

#define REG_WRITE(_ah, _reg, _val) iowrite32(_val, _ah->ah_sh + _reg)
#define REG_READ(_ah, _reg) ioread32(_ah->ah_sh + _reg)

#define SM(_v, _f)  (((_v) << _f##_S) & _f)
#define MS(_v, _f)  (((_v) & _f) >> _f##_S)
#define OS_REG_RMW(_a, _r, _set, _clr)    \
	REG_WRITE(_a, _r, (REG_READ(_a, _r) & ~(_clr)) | (_set))
#define OS_REG_RMW_FIELD(_a, _r, _f, _v) \
	REG_WRITE(_a, _r, \
	(REG_READ(_a, _r) & ~_f) | (((_v) << _f##_S) & _f))
#define OS_REG_SET_BIT(_a, _r, _f) \
	REG_WRITE(_a, _r, REG_READ(_a, _r) | _f)
#define OS_REG_CLR_BIT(_a, _r, _f) \
	REG_WRITE(_a, _r, REG_READ(_a, _r) & ~_f)
#define OS_REG_ath9k_regd_is_bit_set(_a, _r, _f) \
	((REG_READ(_a, _r) & _f) != 0)

#define HAL_COMP_BUF_MAX_SIZE   9216
#define HAL_COMP_BUF_ALIGN_SIZE 512

#define HAL_TXQ_USE_LOCKOUT_BKOFF_DIS   0x00000001

#define INIT_AIFS       2
#define INIT_CWMIN      15
#define INIT_CWMIN_11B  31
#define INIT_CWMAX      1023
#define INIT_SH_RETRY   10
#define INIT_LG_RETRY   10
#define INIT_SSH_RETRY  32
#define INIT_SLG_RETRY  32

#define WLAN_CTRL_FRAME_SIZE (2+2+6+4)

#define ATH_AMPDU_LIMIT_MAX      (64 * 1024 - 1)
#define ATH_AMPDU_LIMIT_DEFAULT  ATH_AMPDU_LIMIT_MAX

#define IEEE80211_WEP_IVLEN      3
#define IEEE80211_WEP_KIDLEN     1
#define IEEE80211_WEP_CRCLEN     4
#define IEEE80211_MAX_MPDU_LEN  (3840 + FCS_LEN +		\
				 (IEEE80211_WEP_IVLEN +		\
				  IEEE80211_WEP_KIDLEN +	\
				  IEEE80211_WEP_CRCLEN))
#define IEEE80211_MAX_LEN       (2300 + FCS_LEN +		\
				 (IEEE80211_WEP_IVLEN +		\
				  IEEE80211_WEP_KIDLEN +	\
				  IEEE80211_WEP_CRCLEN))

#define MAX_REG_ADD_COUNT   129
#define MAX_RATE_POWER 63

#define LE_READ_2(p)							\
	((u_int16_t)							\
	 ((((const u_int8_t *)(p))[0]) | \
		(((const u_int8_t *)(p))[1] << 8)))

#define LE_READ_4(p)							\
	((u_int32_t)							\
	 ((((const u_int8_t *)(p))[0]) | \
		(((const u_int8_t *)(p))[1] << 8) | \
		(((const u_int8_t *)(p))[2] << 16) | \
			(((const u_int8_t *)(p))[3] << 24)))

enum hal_status {
	HAL_OK = 0,
	HAL_ENXIO,
	HAL_ENOMEM,
	HAL_EIO,
	HAL_EEMAGIC,
	HAL_EEVERSION,
	HAL_EELOCKED,
	HAL_EEBADSUM,
	HAL_EEREAD,
	HAL_EEBADMAC,
	HAL_EESIZE,
	HAL_EEWRITE,
	HAL_EINVAL,
	HAL_ENOTSUPP,
	HAL_ESELFTEST,
	HAL_EINPROGRESS
};

enum hal_power_mode {
	HAL_PM_AWAKE = 0,
	HAL_PM_FULL_SLEEP,
	HAL_PM_NETWORK_SLEEP,
	HAL_PM_UNDEFINED
};

enum hal_rfgain {
	HAL_RFGAIN_INACTIVE = 0,
	HAL_RFGAIN_READ_REQUESTED,
	HAL_RFGAIN_NEED_CHANGE
};

#define HAL_ANTENNA_MIN_MODE  0
#define HAL_ANTENNA_FIXED_A   1
#define HAL_ANTENNA_FIXED_B   2
#define HAL_ANTENNA_MAX_MODE  3

struct hal_mib_stats {
	u_int32_t ackrcv_bad;
	u_int32_t rts_bad;
	u_int32_t rts_good;
	u_int32_t fcs_bad;
	u_int32_t beacons;
};

enum hal_ant_setting {
	HAL_ANT_VARIABLE = 0,
	HAL_ANT_FIXED_A,
	HAL_ANT_FIXED_B
};

enum hal_opmode {
	HAL_M_STA = 1,
	HAL_M_IBSS = 0,
	HAL_M_HOSTAP = 6,
	HAL_M_MONITOR = 8
};

enum {
	HAL_SLOT_TIME_6 = 6,
	HAL_SLOT_TIME_9 = 9,
	HAL_SLOT_TIME_20 = 20,
};


enum hal_ht_macmode {
	HAL_HT_MACMODE_20 = 0,
	HAL_HT_MACMODE_2040 = 1,
};

enum hal_ht_extprotspacing {
	HAL_HT_EXTPROTSPACING_20 = 0,
	HAL_HT_EXTPROTSPACING_25 = 1,
};

struct hal_ht_cwm {
	enum hal_ht_macmode ht_macmode;
	enum hal_ht_extprotspacing ht_extprotspacing;
};

enum hal_freq_band {
	HAL_FREQ_BAND_5GHZ = 0,
	HAL_FREQ_BAND_2GHZ = 1,
};

enum {
	HAL_TRUE_CHIP = 1
};

enum hal_bus_type {
	HAL_BUS_TYPE_PCI,
	HAL_BUS_TYPE_AHB
};

enum hal_ani_cmd {
	HAL_ANI_PRESENT = 0x1,
	HAL_ANI_NOISE_IMMUNITY_LEVEL = 0x2,
	HAL_ANI_OFDM_WEAK_SIGNAL_DETECTION = 0x4,
	HAL_ANI_CCK_WEAK_SIGNAL_THR = 0x8,
	HAL_ANI_FIRSTEP_LEVEL = 0x10,
	HAL_ANI_SPUR_IMMUNITY_LEVEL = 0x20,
	HAL_ANI_MODE = 0x40,
	HAL_ANI_PHYERR_RESET = 0x80,
	HAL_ANI_ALL = 0xff
};

enum phytype {
	PHY_DS,
	PHY_FH,
	PHY_OFDM,
	PHY_HT,
	PHY_MAX
};
#define PHY_CCK PHY_DS

enum start_adhoc_option {
	START_ADHOC_NO_11A,
	START_ADHOC_PER_11D,
	START_ADHOC_IN_11A,
	START_ADHOC_IN_11B,
};

enum hal_tp_scale {
	HAL_TP_SCALE_MAX = 0,
	HAL_TP_SCALE_50,
	HAL_TP_SCALE_25,
	HAL_TP_SCALE_12,
	HAL_TP_SCALE_MIN
};

enum ser_reg_mode {
	SER_REG_MODE_OFF = 0,
	SER_REG_MODE_ON = 1,
	SER_REG_MODE_AUTO = 2,
};

#define AR_PHY_CCA_MAX_GOOD_VALUE      		-85
#define AR_PHY_CCA_MAX_HIGH_VALUE      		-62
#define AR_PHY_CCA_MIN_BAD_VALUE       		-121
#define AR_PHY_CCA_FILTERWINDOW_LENGTH_INIT     3
#define AR_PHY_CCA_FILTERWINDOW_LENGTH          5

#define HAL_NF_CAL_HIST_MAX             5
#define NUM_NF_READINGS                 6

struct hal_nfcal_hist {
	int16_t nfCalBuffer[HAL_NF_CAL_HIST_MAX];
	u_int8_t currIndex;
	int16_t privNF;
	u_int8_t invalidNFcount;
};

struct hal_beacon_state {
	u_int32_t bs_nexttbtt;
	u_int32_t bs_nextdtim;
	u_int32_t bs_intval;
#define HAL_BEACON_PERIOD       0x0000ffff
#define HAL_BEACON_ENA          0x00800000
#define HAL_BEACON_RESET_TSF    0x01000000
	u_int32_t bs_dtimperiod;
	u_int16_t bs_cfpperiod;
	u_int16_t bs_cfpmaxduration;
	u_int32_t bs_cfpnext;
	u_int16_t bs_timoffset;
	u_int16_t bs_bmissthreshold;
	u_int32_t bs_sleepduration;
};

struct hal_node_stats {
	u_int32_t ns_avgbrssi;
	u_int32_t ns_avgrssi;
	u_int32_t ns_avgtxrssi;
	u_int32_t ns_avgtxrate;
};

#define HAL_RSSI_EP_MULTIPLIER  (1<<7)
#define HAL_RATE_EP_MULTIPLIER  (1<<7)

enum hal_gpio_output_mux_type {
	HAL_GPIO_OUTPUT_MUX_AS_OUTPUT,
	HAL_GPIO_OUTPUT_MUX_AS_PCIE_ATTENTION_LED,
	HAL_GPIO_OUTPUT_MUX_AS_PCIE_POWER_LED,
	HAL_GPIO_OUTPUT_MUX_AS_MAC_NETWORK_LED,
	HAL_GPIO_OUTPUT_MUX_AS_MAC_POWER_LED,
	HAL_GPIO_OUTPUT_MUX_NUM_ENTRIES
};

enum {
	HAL_RESET_POWER_ON,
	HAL_RESET_WARM,
	HAL_RESET_COLD,
};

#define AH_USE_EEPROM   0x1
#define AH_IS_HB63      0x2

struct ath_hal {
	u_int32_t ah_magic;
	u_int16_t ah_devid;
	u_int16_t ah_subvendorid;
	void *ah_sc;
	void __iomem *ah_sh;
	u_int16_t ah_countryCode;
	u_int32_t ah_macVersion;
	u_int16_t ah_macRev;
	u_int16_t ah_phyRev;
	u_int16_t ah_analog5GhzRev;
	u_int16_t ah_analog2GhzRev;
	u_int8_t ah_decompMask[HAL_DECOMP_MASK_SIZE];
	u_int32_t ah_flags;
	enum hal_opmode ah_opmode;
	struct hal_ops_config ah_config;
	struct hal_capabilities ah_caps;
	int16_t ah_powerLimit;
	u_int16_t ah_maxPowerLevel;
	u_int ah_tpScale;
	u_int16_t ah_currentRD;
	u_int16_t ah_currentRDExt;
	u_int16_t ah_currentRDInUse;
	u_int16_t ah_currentRD5G;
	u_int16_t ah_currentRD2G;
	char ah_iso[4];
	enum start_adhoc_option ah_adHocMode;
	enum hal_bool ah_commonMode;
	struct hal_channel_internal ah_channels[150];
	u_int ah_nchan;
	struct hal_channel_internal *ah_curchan;
	u_int16_t ah_rfsilent;
	enum hal_bool ah_rfkillEnabled;
	enum hal_bool ah_isPciExpress;
	u_int16_t ah_txTrigLevel;
#ifndef ATH_NF_PER_CHAN
	struct hal_nfcal_hist nfCalHist[NUM_NF_READINGS];
#endif
};

#define HDPRINTF(_ah, _m, _fmt, ...) do {				\
		if (((_ah) == NULL && _m == HAL_DBG_UNMASKABLE) ||	\
		    (((struct ath_hal *)(_ah))->ah_config.ath_hal_debug & _m)) \
			printk(KERN_DEBUG _fmt , ##__VA_ARGS__);	\
	} while (0)

enum wireless_mode {
	WIRELESS_MODE_11a = 0,
	WIRELESS_MODE_11b = 2,
	WIRELESS_MODE_11g = 3,
	WIRELESS_MODE_11NA_HT20 = 6,
	WIRELESS_MODE_11NG_HT20 = 7,
	WIRELESS_MODE_11NA_HT40PLUS = 8,
	WIRELESS_MODE_11NA_HT40MINUS = 9,
	WIRELESS_MODE_11NG_HT40PLUS = 10,
	WIRELESS_MODE_11NG_HT40MINUS = 11,
	WIRELESS_MODE_MAX
};

enum {
	ATH9K_MODE_SEL_11A = 0x00001,
	ATH9K_MODE_SEL_11B = 0x00002,
	ATH9K_MODE_SEL_11G = 0x00004,
	ATH9K_MODE_SEL_11NG_HT20 = 0x00008,
	ATH9K_MODE_SEL_11NA_HT20 = 0x00010,
	ATH9K_MODE_SEL_11NG_HT40PLUS = 0x00020,
	ATH9K_MODE_SEL_11NG_HT40MINUS = 0x00040,
	ATH9K_MODE_SEL_11NA_HT40PLUS = 0x00080,
	ATH9K_MODE_SEL_11NA_HT40MINUS = 0x00100,
	ATH9K_MODE_SEL_2GHZ = (ATH9K_MODE_SEL_11B |
			       ATH9K_MODE_SEL_11G |
			       ATH9K_MODE_SEL_11NG_HT20),
	ATH9K_MODE_SEL_5GHZ = (ATH9K_MODE_SEL_11A |
			       ATH9K_MODE_SEL_11NA_HT20),
	ATH9K_MODE_SEL_ALL = 0xffffffff
};

struct chan_centers {
	u_int16_t synth_center;
	u_int16_t ctl_center;
	u_int16_t ext_center;
};

enum hal_status ath_hal_getcapability(struct ath_hal *ah,
				      enum hal_capability_type type,
				      u_int32_t capability,
				      u_int32_t *result);
const struct hal_rate_table *ath9k_hw_getratetable(struct ath_hal *ah,
						   u_int mode);
void ath9k_hw_detach(struct ath_hal *ah);
struct ath_hal *ath9k_hw_attach(u_int16_t devid, void *sc, void __iomem *mem,
				enum hal_status *error);
enum hal_bool ath9k_regd_init_channels(struct ath_hal *ah,
				       struct hal_channel *chans,
				       u_int maxchans, u_int *nchans,
				       u_int8_t *regclassids,
				       u_int maxregids, u_int *nregids,
				       u_int16_t cc, u_int32_t modeSelect,
				       enum hal_bool enableOutdoor,
				       enum hal_bool
				       enableExtendedChannels);
u_int ath9k_hw_mhz2ieee(struct ath_hal *ah, u_int freq, u_int flags);
enum hal_int ath9k_hw_set_interrupts(struct ath_hal *ah,
				     enum hal_int ints);
enum hal_bool ath9k_hw_reset(struct ath_hal *ah, enum hal_opmode opmode,
			     struct hal_channel *chan,
			     enum hal_ht_macmode macmode,
			     u_int8_t txchainmask, u_int8_t rxchainmask,
			     enum hal_ht_extprotspacing extprotspacing,
			     enum hal_bool bChannelChange,
			     enum hal_status *status);
enum hal_bool ath9k_hw_phy_disable(struct ath_hal *ah);
void ath9k_hw_reset_calvalid(struct ath_hal *ah, struct hal_channel *chan,
			     enum hal_bool *isCalDone);
void ath9k_hw_ani_monitor(struct ath_hal *ah,
			  const struct hal_node_stats *stats,
			  struct hal_channel *chan);
enum hal_bool ath9k_hw_calibrate(struct ath_hal *ah,
				 struct hal_channel *chan,
				 u_int8_t rxchainmask,
				 enum hal_bool longcal,
				 enum hal_bool *isCalDone);
int16_t ath9k_hw_getchan_noise(struct ath_hal *ah,
			       struct hal_channel *chan);
void ath9k_hw_write_associd(struct ath_hal *ah, const u_int8_t *bssid,
			    u_int16_t assocId);
void ath9k_hw_setrxfilter(struct ath_hal *ah, u_int32_t bits);
void ath9k_hw_write_associd(struct ath_hal *ah, const u_int8_t *bssid,
			    u_int16_t assocId);
enum hal_bool ath9k_hw_stoptxdma(struct ath_hal *ah, u_int q);
void ath9k_hw_reset_tsf(struct ath_hal *ah);
enum hal_bool ath9k_hw_keyisvalid(struct ath_hal *ah, u_int16_t entry);
enum hal_bool ath9k_hw_keysetmac(struct ath_hal *ah, u_int16_t entry,
				 const u_int8_t *mac);
enum hal_bool ath9k_hw_set_keycache_entry(struct ath_hal *ah,
					  u_int16_t entry,
					  const struct hal_keyval *k,
					  const u_int8_t *mac,
					  int xorKey);
enum hal_bool ath9k_hw_set_tsfadjust(struct ath_hal *ah,
				     u_int32_t setting);
void ath9k_hw_configpcipowersave(struct ath_hal *ah, int restore);
enum hal_bool ath9k_hw_intrpend(struct ath_hal *ah);
enum hal_bool ath9k_hw_getisr(struct ath_hal *ah, enum hal_int *masked);
enum hal_bool ath9k_hw_updatetxtriglevel(struct ath_hal *ah,
					 enum hal_bool bIncTrigLevel);
void ath9k_hw_procmibevent(struct ath_hal *ah,
			   const struct hal_node_stats *stats);
enum hal_bool ath9k_hw_setrxabort(struct ath_hal *ah, enum hal_bool set);
void ath9k_hw_set11nmac2040(struct ath_hal *ah, enum hal_ht_macmode mode);
enum hal_bool ath9k_hw_setupxtxdesc(struct ath_hal *ah,
				    struct ath_desc *ds,
				    u_int txRate1, u_int txTries1,
				    u_int txRate2, u_int txTries2,
				    u_int txRate3, u_int txTries3);
enum hal_bool ath9k_hw_phycounters(struct ath_hal *ah);
enum hal_bool ath9k_hw_keyreset(struct ath_hal *ah, u_int16_t entry);
enum hal_bool ath9k_hw_getcapability(struct ath_hal *ah,
				     enum hal_capability_type type,
				     u_int32_t capability,
				     u_int32_t *result);
enum hal_bool ath9k_hw_setcapability(struct ath_hal *ah,
				     enum hal_capability_type type,
				     u_int32_t capability,
				     u_int32_t setting,
				     enum hal_status *status);
u_int ath9k_hw_getdefantenna(struct ath_hal *ah);
void ath9k_hw_getmac(struct ath_hal *ah, u_int8_t *mac);
void ath9k_hw_getbssidmask(struct ath_hal *ah, u_int8_t *mask);
enum hal_bool ath9k_hw_setbssidmask(struct ath_hal *ah,
				    const u_int8_t *mask);
enum hal_bool ath9k_hw_setpower(struct ath_hal *ah,
				enum hal_power_mode mode);
enum hal_int ath9k_hw_intrget(struct ath_hal *ah);
u_int64_t ath9k_hw_gettsf64(struct ath_hal *ah);
u_int ath9k_hw_getdefantenna(struct ath_hal *ah);
enum hal_bool ath9k_hw_setslottime(struct ath_hal *ah, u_int us);
enum hal_bool ath9k_hw_setantennaswitch(struct ath_hal *ah,
					enum hal_ant_setting settings,
					struct hal_channel *chan,
					u_int8_t *tx_chainmask,
					u_int8_t *rx_chainmask,
					u_int8_t *antenna_cfgd);
void ath9k_hw_setantenna(struct ath_hal *ah, u_int antenna);
enum hal_status ath9k_hw_select_antconfig(struct ath_hal *ah,
					  u_int32_t cfg);
enum hal_bool ath9k_hw_puttxbuf(struct ath_hal *ah, u_int q,
				u_int32_t txdp);
enum hal_bool ath9k_hw_txstart(struct ath_hal *ah, u_int q);
u_int16_t ath9k_hw_computetxtime(struct ath_hal *ah,
				 const struct hal_rate_table *rates,
				 u_int32_t frameLen, u_int16_t rateix,
				 enum hal_bool shortPreamble);
void ath9k_hw_set11n_ratescenario(struct ath_hal *ah, struct ath_desc *ds,
				  struct ath_desc *lastds,
				  u_int durUpdateEn, u_int rtsctsRate,
				  u_int rtsctsDuration,
				  struct hal_11n_rate_series series[],
				  u_int nseries, u_int flags);
void ath9k_hw_set11n_burstduration(struct ath_hal *ah,
				   struct ath_desc *ds,
				   u_int burstDuration);
void ath9k_hw_cleartxdesc(struct ath_hal *ah, struct ath_desc *ds);
u_int32_t ath9k_hw_reverse_bits(u_int32_t val, u_int32_t n);
enum hal_bool ath9k_hw_resettxqueue(struct ath_hal *ah, u_int q);
u_int ath9k_regd_get_ctl(struct ath_hal *ah, struct hal_channel *chan);
u_int ath9k_regd_get_antenna_allowed(struct ath_hal *ah,
	struct hal_channel *chan);
u_int ath9k_hw_mhz2ieee(struct ath_hal *ah, u_int freq, u_int flags);
enum hal_bool ath9k_hw_gettxqueueprops(struct ath_hal *ah, int q,
	struct hal_txq_info *qInfo);
enum hal_bool ath9k_hw_settxqueueprops(struct ath_hal *ah, int q,
				       const struct hal_txq_info *qInfo);
struct hal_channel_internal *ath9k_regd_check_channel(struct ath_hal *ah,
	const struct hal_channel *c);
void ath9k_hw_set11n_txdesc(struct ath_hal *ah, struct ath_desc *ds,
	u_int pktLen, enum hal_pkt_type type,
	u_int txPower, u_int keyIx,
	enum hal_key_type keyType, u_int flags);
enum hal_bool ath9k_hw_filltxdesc(struct ath_hal *ah, struct ath_desc *ds,
	u_int segLen, enum hal_bool firstSeg,
	enum hal_bool lastSeg,
	const struct ath_desc *ds0);
u_int32_t ath9k_hw_GetMibCycleCountsPct(struct ath_hal *ah,
					u_int32_t *rxc_pcnt,
					u_int32_t *rxf_pcnt,
					u_int32_t *txf_pcnt);
void ath9k_hw_dmaRegDump(struct ath_hal *ah);
void ath9k_hw_beaconinit(struct ath_hal *ah,
			 u_int32_t next_beacon, u_int32_t beacon_period);
void ath9k_hw_set_sta_beacon_timers(struct ath_hal *ah,
				    const struct hal_beacon_state *bs);
enum hal_bool ath9k_hw_setuprxdesc(struct ath_hal *ah, struct ath_desc *ds,
				   u_int32_t size, u_int flags);
void ath9k_hw_putrxbuf(struct ath_hal *ah, u_int32_t rxdp);
void ath9k_hw_rxena(struct ath_hal *ah);
void ath9k_hw_setopmode(struct ath_hal *ah);
enum hal_bool ath9k_hw_setmac(struct ath_hal *ah, const u_int8_t *mac);
void ath9k_hw_setmcastfilter(struct ath_hal *ah, u_int32_t filter0,
			     u_int32_t filter1);
u_int32_t ath9k_hw_getrxfilter(struct ath_hal *ah);
void ath9k_hw_startpcureceive(struct ath_hal *ah);
void ath9k_hw_stoppcurecv(struct ath_hal *ah);
enum hal_bool ath9k_hw_stopdmarecv(struct ath_hal *ah);
enum hal_status ath9k_hw_rxprocdesc(struct ath_hal *ah,
				    struct ath_desc *ds, u_int32_t pa,
				    struct ath_desc *nds, u_int64_t tsf);
u_int32_t ath9k_hw_gettxbuf(struct ath_hal *ah, u_int q);
enum hal_status ath9k_hw_txprocdesc(struct ath_hal *ah,
				    struct ath_desc *ds);
void ath9k_hw_set11n_aggr_middle(struct ath_hal *ah, struct ath_desc *ds,
				 u_int numDelims);
void ath9k_hw_set11n_aggr_first(struct ath_hal *ah, struct ath_desc *ds,
				u_int aggrLen);
void ath9k_hw_set11n_aggr_last(struct ath_hal *ah, struct ath_desc *ds);
enum hal_bool ath9k_hw_releasetxqueue(struct ath_hal *ah, u_int q);
void ath9k_hw_gettxintrtxqs(struct ath_hal *ah, u_int32_t *txqs);
void ath9k_hw_clr11n_aggr(struct ath_hal *ah, struct ath_desc *ds);
void ath9k_hw_set11n_virtualmorefrag(struct ath_hal *ah,
				     struct ath_desc *ds, u_int vmf);
enum hal_bool ath9k_hw_SetTxPowerLimit(struct ath_hal *ah, u_int32_t limit,
				       u_int16_t tpcInDb);
enum hal_bool ath9k_regd_is_public_safety_sku(struct ath_hal *ah);
int ath9k_hw_setuptxqueue(struct ath_hal *ah, enum hal_tx_queue type,
			  const struct hal_txq_info *qInfo);
u_int32_t ath9k_hw_numtxpending(struct ath_hal *ah, u_int q);
const char *ath9k_hw_probe(u_int16_t vendorid, u_int16_t devid);
enum hal_bool ath9k_hw_disable(struct ath_hal *ah);
void ath9k_hw_rfdetach(struct ath_hal *ah);
void ath9k_hw_get_channel_centers(struct ath_hal *ah,
				  struct hal_channel_internal *chan,
				  struct chan_centers *centers);
enum hal_bool ath9k_get_channel_edges(struct ath_hal *ah,
				      u_int16_t flags, u_int16_t *low,
				      u_int16_t *high);
enum hal_bool ath9k_hw_get_chip_power_limits(struct ath_hal *ah,
					     struct hal_channel *chans,
					     u_int32_t nchans);
#endif
