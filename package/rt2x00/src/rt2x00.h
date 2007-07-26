/*
	Copyright (C) 2004 - 2007 rt2x00 SourceForge Project
	<http://rt2x00.serialmonkey.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the
	Free Software Foundation, Inc.,
	59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	Module: rt2x00
	Abstract: rt2x00 global information.
	Supported chipsets: RT2460, RT2560, RT2570,
	rt2561, rt2561s, rt2661, rt2571W & rt2671.
 */

#ifndef RT2X00_H
#define RT2X00_H

#include <linux/bitops.h>
#include <linux/skbuff.h>
#include <linux/workqueue.h>

#include <net/mac80211.h>

#include "rt2x00lib.h"
#include "rt2x00debug.h"

/*
 * Module information.
 */
#ifndef DRV_NAME
#define DRV_NAME	"rt2x00"
#endif /* DRV_NAME */
#define DRV_VERSION	"2.0.1"
#define DRV_PROJECT	"http://rt2x00.serialmonkey.com"

/*
 * Debug definitions.
 * Debug output has to be enabled during compile time.
 */
#define DEBUG_PRINTK_MSG(__dev, __kernlvl, __lvl, __msg, __args...)	\
	printk(__kernlvl "%s -> %s: %s - " __msg,			\
		wiphy_name(rt2x00dev->hw->wiphy),			\
		__FUNCTION__, __lvl, ##__args)

#define DEBUG_PRINTK_PROBE(__kernlvl, __lvl, __msg, __args...)		\
	printk(__kernlvl "%s -> %s: %s - " __msg,			\
		DRV_NAME, __FUNCTION__, __lvl, 	##__args)

#ifdef CONFIG_RT2X00_DEBUG
#define DEBUG_PRINTK(__dev, __kernlvl, __lvl, __msg, __args...) \
	DEBUG_PRINTK_MSG(__dev, __kernlvl, __lvl, __msg, ##__args);
#else /* CONFIG_RT2X00_DEBUG */
#define DEBUG_PRINTK(__dev, __kernlvl, __lvl, __msg, __args...) \
	do { } while (0)
#endif /* CONFIG_RT2X00_DEBUG */

/*
 * Various debug levels.
 * The debug levels PANIC and ERROR both indicate serious problems,
 * for this reason they should never be ignored.
 * The special ERROR_PROBE message is for messages that are generated
 * when the rt2x00_dev is not yet initialized.
 */
#define PANIC(__dev, __msg, __args...) \
	DEBUG_PRINTK_MSG(__dev, KERN_CRIT, "Panic", __msg, ##__args)
#define ERROR(__dev, __msg, __args...)	\
	DEBUG_PRINTK_MSG(__dev, KERN_ERR, "Error", __msg, ##__args)
#define ERROR_PROBE(__msg, __args...) \
	DEBUG_PRINTK_PROBE(KERN_ERR, "Error", __msg, ##__args)
#define WARNING(__dev, __msg, __args...) \
	DEBUG_PRINTK(__dev, KERN_WARNING, "Warning", __msg, ##__args)
#define NOTICE(__dev, __msg, __args...) \
	DEBUG_PRINTK(__dev, KERN_NOTICE, "Notice", __msg, ##__args)
#define INFO(__dev, __msg, __args...) \
	DEBUG_PRINTK(__dev, KERN_INFO, "Info", __msg, ##__args)
#define DEBUG(__dev, __msg, __args...) \
	DEBUG_PRINTK(__dev, KERN_DEBUG, "Debug", __msg, ##__args)
#define EEPROM(__dev, __msg, __args...) \
	DEBUG_PRINTK(__dev, KERN_DEBUG, "EEPROM recovery", __msg, ##__args)

/*
 * Ring sizes.
 * Ralink PCI devices demand the Frame size to be a multiple of 128 bytes.
 * DATA_FRAME_SIZE is used for TX, RX, ATIM and PRIO rings.
 * MGMT_FRAME_SIZE is used for the BEACON ring.
 */
#define DATA_FRAME_SIZE	2432
#define MGMT_FRAME_SIZE	256

/*
 * Number of entries in a packet ring.
 */
#define RX_ENTRIES	12
#define TX_ENTRIES	12
#define ATIM_ENTRIES	1
#define BEACON_ENTRIES	1

/*
 * Standard timing and size defines.
 */
#define ACK_SIZE		14
#define IEEE80211_HEADER	24
#define PLCP			48
#define BEACON			100
#define PREAMBLE		144
#define SHORT_PREAMBLE		72
#define SLOT_TIME		20
#define SHORT_SLOT_TIME		9
#define SIFS			10
#define PIFS			( SIFS + SLOT_TIME )
#define SHORT_PIFS		( SIFS + SHORT_SLOT_TIME )
#define DIFS			( PIFS + SLOT_TIME )
#define SHORT_DIFS		( SHORT_PIFS + SHORT_SLOT_TIME )
#define EIFS			( SIFS + (8 * (IEEE80211_HEADER + ACK_SIZE)) )

/*
 * IEEE802.11 header defines
 */
#define is_rts_frame(__fc) \
	( !!((((__fc) &  IEEE80211_FCTL_FTYPE) == IEEE80211_FTYPE_CTL) && \
	     (((__fc) &  IEEE80211_FCTL_STYPE) == IEEE80211_STYPE_RTS)) )
#define is_cts_frame(__fc) \
	( !!((((__fc) &  IEEE80211_FCTL_FTYPE) == IEEE80211_FTYPE_CTL) && \
	     (((__fc) &  IEEE80211_FCTL_STYPE) == IEEE80211_STYPE_CTS)) )
#define is_probe_resp(__fc) \
	( !!((((__fc) & IEEE80211_FCTL_FTYPE) == IEEE80211_FTYPE_MGMT) && \
	     (((__fc) & IEEE80211_FCTL_STYPE) == IEEE80211_STYPE_PROBE_RESP)) )

/*
 * Interval defines
 */
#define LINK_TUNE_INTERVAL	( 1 * HZ )
#define RFKILL_POLL_INTERVAL	( HZ / 4 )

/*
 * TX result flags.
 */
enum TX_STATUS {
	TX_SUCCESS = 0,
	TX_SUCCESS_RETRY = 1,
	TX_FAIL_RETRY = 2,
	TX_FAIL_INVALID = 3,
	TX_FAIL_OTHER = 4,
};

/*
 * Antenna values
 */
enum antenna {
	ANTENNA_DIVERSITY = 0,
	ANTENNA_A = 1,
	ANTENNA_B = 2,
};

/*
 * Led mode values.
 */
enum led_mode {
	LED_MODE_DEFAULT = 0,
	LED_MODE_TXRX_ACTIVITY = 1,
	LED_MODE_SIGNAL_STRENGTH = 2,
	LED_MODE_ASUS = 3,
	LED_MODE_ALPHA = 4,
};

/*
 * Device states
 */
enum dev_state {
	STATE_DEEP_SLEEP = 0,
	STATE_SLEEP = 1,
	STATE_STANDBY = 2,
	STATE_AWAKE = 3,

/*
 * Additional device states, these values are
 * not strict since they are not directly passed
 * into the device.
 */
	STATE_RADIO_ON,
	STATE_RADIO_OFF,
	STATE_RADIO_RX_ON,
	STATE_RADIO_RX_OFF,
};

/*
 * IFS backoff values
 */
enum ifs {
	IFS_BACKOFF = 0,
	IFS_SIFS = 1,
	IFS_NEW_BACKOFF = 2,
	IFS_NONE = 3,
};

/*
 * Cipher types for hardware encryption
 */
enum cipher {
	CIPHER_NONE = 0,
	CIPHER_WEP64 = 1,
	CIPHER_WEP128 = 2,
	CIPHER_TKIP = 3,
	CIPHER_AES = 4,
/*
 * The following fields were added by rt61pci and rt73usb.
 */
	CIPHER_CKIP64 = 5,
	CIPHER_CKIP128 = 6,
	CIPHER_TKIP_NO_MIC = 7,
};

/*
 * Register handlers.
 * We store the position of a register field inside a field structure,
 * This will simplify the process of setting and reading a certain field
 * inside the register while making sure the process remains byte order safe.
 */
struct rt2x00_field8 {
	u8 bit_offset;
	u8 bit_mask;
};

struct rt2x00_field16 {
	u16 bit_offset;
	u16 bit_mask;
};

struct rt2x00_field32 {
	u32 bit_offset;
	u32 bit_mask;
};

/*
 * Power of two check from Linus Torvalds,
 * this will check if the mask that has been
 * given contains and contiguous set of bits.
 */
#define is_power_of_two(x)	( !((x) & ((x)-1)) )
#define low_bit_mask(x)		( ((x)-1) & ~(x) )
#define is_valid_mask(x)	is_power_of_two(1 + (x) + low_bit_mask(x))

#define FIELD8(__mask)				\
({						\
	BUILD_BUG_ON(!(__mask) ||		\
		     !is_valid_mask(__mask) ||	\
		     (__mask) != (u8)(__mask));	\
	(struct rt2x00_field8) {		\
		__ffs(__mask), (__mask)		\
	};					\
})

#define FIELD16(__mask)				\
({						\
	BUILD_BUG_ON(!(__mask) ||		\
		     !is_valid_mask(__mask) ||	\
		     (__mask) != (u16)(__mask));\
	(struct rt2x00_field16) {		\
		__ffs(__mask), (__mask)		\
	};					\
})

#define FIELD32(__mask)				\
({						\
	BUILD_BUG_ON(!(__mask) ||		\
		     !is_valid_mask(__mask) ||	\
		     (__mask) != (u32)(__mask));\
	(struct rt2x00_field32) {		\
		__ffs(__mask), (__mask)		\
	};					\
})

static inline void rt2x00_set_field32(u32 *reg,
	const struct rt2x00_field32 field, const u32 value)
{
	*reg &= ~(field.bit_mask);
	*reg |= (value << field.bit_offset) & field.bit_mask;
}

static inline u32 rt2x00_get_field32(const u32 reg,
	const struct rt2x00_field32 field)
{
	return (reg & field.bit_mask) >> field.bit_offset;
}

static inline void rt2x00_set_field16(u16 *reg,
	const struct rt2x00_field16 field, const u16 value)
{
	*reg &= ~(field.bit_mask);
	*reg |= (value << field.bit_offset) & field.bit_mask;
}

static inline u16 rt2x00_get_field16(const u16 reg,
	const struct rt2x00_field16 field)
{
	return (reg & field.bit_mask) >> field.bit_offset;
}

static inline void rt2x00_set_field8(u8 *reg,
	const struct rt2x00_field8 field, const u8 value)
{
	*reg &= ~(field.bit_mask);
	*reg |= (value << field.bit_offset) & field.bit_mask;
}

static inline u8 rt2x00_get_field8(const u8 reg,
	const struct rt2x00_field8 field)
{
	return (reg & field.bit_mask) >> field.bit_offset;
}

/*
 * Chipset identification
 * The chipset on the device is composed of a RT and RF chip.
 * The chipset combination is important for determining device capabilities.
 */
struct rt2x00_chip {
	u16 rt;
#define RT2460		0x0101
#define RT2560		0x0201
#define RT2570		0x1201
#define RT2561		0x0301
#define RT2561s		0x0302
#define RT2661		0x0401
#define RT2571		0x1300

	u16 rf;
	u32 rev;
};

/*
 * data_desc
 * Each data entry also contains a descriptor which is used by the
 * device to determine what should be done with the packet and
 * what the current status is.
 * This structure is greatly simplified, but the descriptors
 * are basically a list of little endian 32 bit values.
 * Make the array by default 1 word big, this will allow us
 * to use sizeof() correctly.
 */
struct data_desc {
	__le32 word[1];
};

/*
 * data_entry_desc
 * Summary of information that should be written into the
 * descriptor for sending a TX frame.
 */
struct data_entry_desc {
	/*
	 * PLCP values.
	 */
	u16 length_high;
	u16 length_low;
	u16 signal;
	u16 service;

	int queue;
	int ifs;
};

/*
 * data_entry
 * The data ring is a list of data entries.
 * Each entry holds a reference to the descriptor
 * and the data buffer. For TX rings the reference to the
 * sk_buff of the packet being transmitted is also stored here.
 */
struct data_entry {
	/*
	 * Status flags
	 */
	unsigned long flags;
#define ENTRY_OWNER_NIC		1
#define ENTRY_TXDONE		2
#define ENTRY_TXD_RTS_FRAME	3
#define ENTRY_TXD_OFDM_RATE	4
#define ENTRY_TXD_MORE_FRAG	5
#define ENTRY_TXD_REQ_TIMESTAMP	6
#define ENTRY_TXD_REQ_ACK	7
#define ENTRY_TXD_NEW_SEQ	8

	/*
	 * Ring we belong to.
	 */
	struct data_ring *ring;

	/*
	 * sk_buff for the packet which is being transmitted
	 * in this entry (Only used with TX related rings).
	 */
	struct sk_buff *skb;

	/*
	 * Store a ieee80211_tx_status structure in each
	 * ring entry, this will optimize the txdone
	 * handler.
	 */
	struct ieee80211_tx_status tx_status;

	/*
	 * private pointer specific to driver.
	 */
	void *priv;

	/*
	 * Data address for this entry.
	 */
	void *data_addr;
	dma_addr_t data_dma;
};

/*
 * data_ring
 * Data rings are used by the device to send and receive packets.
 * The data_addr is the base address of the data memory.
 * To determine at which point in the ring we are,
 * have to use the rt2x00_ring_index_*() functions.
 */
struct data_ring {
	/*
	 * Pointer to main rt2x00dev structure where this
	 * ring belongs to.
	 */
	struct rt2x00_dev *rt2x00dev;

	/*
	 * Base address for the device specific data entries.
	 */
	struct data_entry *entry;

	/*
	 * TX queue statistic info.
	 */
	struct ieee80211_tx_queue_stats_data stats;

	/*
	 * TX Queue parameters.
	 */
	struct ieee80211_tx_queue_params tx_params;

	/*
	 * Base address for data ring.
	 */
	dma_addr_t data_dma;
	void *data_addr;

	/*
	 * Index variables.
	 */
	u16 index;
	u16 index_done;

	/*
	 * Size of packet and descriptor in bytes.
	 */
	u16 data_size;
	u16 desc_size;
};

/*
 * Handlers to determine the address of the current device specific
 * data entry, where either index or index_done points to.
 */
static inline struct data_entry* rt2x00_get_data_entry(
	struct data_ring *ring)
{
	return &ring->entry[ring->index];
}

static inline struct data_entry* rt2x00_get_data_entry_done(
	struct data_ring *ring)
{
	return &ring->entry[ring->index_done];
}

/*
 * Total ring memory
 */
static inline int rt2x00_get_ring_size(struct data_ring *ring)
{
	return ring->stats.limit * (ring->desc_size + ring->data_size);
}

/*
 * Ring index manipulation functions.
 */
static inline void rt2x00_ring_index_inc(struct data_ring *ring)
{
	ring->index++;
	if (ring->index >= ring->stats.limit)
		ring->index = 0;
	ring->stats.len++;
}

static inline void rt2x00_ring_index_done_inc(struct data_ring *ring)
{
	ring->index_done++;
	if (ring->index_done >= ring->stats.limit)
		ring->index_done = 0;
	ring->stats.len--;
	ring->stats.count++;
}

static inline void rt2x00_ring_index_clear(struct data_ring *ring)
{
	ring->index = 0;
	ring->index_done = 0;
	ring->stats.len = 0;
	ring->stats.count = 0;
}

static inline int rt2x00_ring_empty(struct data_ring *ring)
{
	return ring->stats.len == 0;
}

static inline int rt2x00_ring_full(struct data_ring *ring)
{
	return ring->stats.len == ring->stats.limit;
}

static inline int rt2x00_ring_free(struct data_ring *ring)
{
	if (ring->index_done >= ring->index)
		return ring->index_done - ring->index;
	return ring->stats.len - (ring->index - ring->index_done);
}

/*
 * TX/RX Descriptor access functions.
 */
static inline void rt2x00_desc_read(struct data_desc *desc,
	const u8 word, u32 *value)
{
	*value = le32_to_cpu(desc->word[word]);
}

static inline void rt2x00_desc_write(struct data_desc *desc,
	const u8 word, const u32 value)
{
	desc->word[word] = cpu_to_le32(value);
}

/*
 * To optimize the quality of the link we need to store
 * the quality of received frames and periodically
 * optimize the link.
 */
struct link {
	/*
	 * Link tuner counter
	 * The number of times the link has been tuned
	 * since the radio has been switched on.
	 */
	u32 count;

	/*
	 * RSSI statistics.
	 */
	u32 count_rssi;
	u32 total_rssi;

	/*
	 * Misc statistics.
	 */
	u32 curr_noise;
	u32 false_cca;

	/*
	 * Work structure for scheduling periodic link tuning.
	 */
	struct delayed_work work;
};

/*
 * Interface structure
 * Configuration details about the current interface.
 */
struct interface {
	/*
	 * Interface identification. The value is assigned
	 * to us by the 80211 stack, and is used to request
	 * new beacons.
	 */
	int id;

	/*
	 * Current working type (IEEE80211_IF_TYPE_*).
	 * This excludes the type IEEE80211_IF_TYPE_MNTR
	 * since that is counted seperately in the monitor_count
	 * field.
	 */
	int type;

	/*
	 * BBSID of the AP to associate with.
	 */
	u8 bssid[ETH_ALEN];

	/*
	 * Store the promisc mode for the current interface.
	 * monitor mode always forces promisc mode to be enabled,
	 * so we need to store the promisc mode seperately.
	 */
	short promisc;

	/*
	 * Monitor mode count, the number of interfaces
	 * in monitor mode that that have been added.
	 */
	short monitor_count;
};

static inline int is_interface_present(struct interface *intf)
{
	return !!intf->id;
}

static inline int is_monitor_present(struct interface *intf)
{
	return !!intf->monitor_count;
}

/*
 * rt2x00lib callback functions.
 */
struct rt2x00lib_ops {
	/*
	 * Interrupt handlers.
	 */
	irq_handler_t irq_handler;

	/*
	 * Device init handlers.
	 */
	int (*init_hw)(struct rt2x00_dev *rt2x00dev);
	char* (*get_fw_name)(struct rt2x00_dev *rt2x00dev);
	int (*load_firmware)(struct rt2x00_dev *rt2x00dev, void *data,
		const size_t len);

	/*
	 * Device initialization/deinitialization handlers.
	 */
	int (*initialize)(struct rt2x00_dev *rt2x00dev);
	void (*uninitialize)(struct rt2x00_dev *rt2x00dev);

	/*
	 * Radio control handlers.
	 */
	int (*set_device_state)(struct rt2x00_dev *rt2x00dev,
		enum dev_state state);
	int (*rfkill_poll)(struct rt2x00_dev *rt2x00dev);
	void (*link_tuner)(struct rt2x00_dev *rt2x00dev, int rssi);

	/*
	 * TX control handlers
	 */
	void (*write_tx_desc)(struct rt2x00_dev *rt2x00dev,
		struct data_entry *entry, struct data_desc *txd,
		struct data_entry_desc *desc,
		struct ieee80211_hdr *ieee80211hdr, unsigned int length,
		struct ieee80211_tx_control *control);
	int (*write_tx_data)(struct rt2x00_dev *rt2x00dev,
		struct data_ring *ring, struct sk_buff *skb,
		struct ieee80211_tx_control *control);
	void (*kick_tx_queue)(struct rt2x00_dev *rt2x00dev, int queue);

	/*
	 * Configuration handlers.
	 */
	void (*config_type)(struct rt2x00_dev *rt2x00dev, const int type);
	void (*config_phymode)(struct rt2x00_dev *rt2x00dev, const int phy);
	void (*config_channel)(struct rt2x00_dev *rt2x00dev, const int value,
	const int channel, const int txpower);
	void (*config_mac_addr)(struct rt2x00_dev *rt2x00dev, u8 *mac);
	void (*config_bssid)(struct rt2x00_dev *rt2x00dev, u8 *bssid);
	void (*config_promisc)(struct rt2x00_dev *rt2x00dev, const int promisc);
	void (*config_txpower)(struct rt2x00_dev *rt2x00dev, const int txpower);
	void (*config_antenna)(struct rt2x00_dev *rt2x00dev,
		const int antenna_tx, const int antenna_rx);
	void (*config_duration)(struct rt2x00_dev *rt2x00dev,
		const int short_slot_time, const int beacon_int);
};

/*
 * rt2x00 driver callback operation structure.
 */
struct rt2x00_ops {
	const char *name;
	const unsigned int rxd_size;
	const unsigned int txd_size;
	const struct rt2x00lib_ops *lib;
	const struct ieee80211_ops *hw;
#ifdef CONFIG_RT2X00_LIB_DEBUGFS
	const struct rt2x00debug *debugfs;
#endif /* CONFIG_RT2X00_LIB_DEBUGFS */
};

/*
 * rt2x00 device structure.
 */
struct rt2x00_dev {
	/*
	 * Device structure.
	 * The structure stored in here depends on the
	 * system bus (PCI or USB).
	 * When accessing this variable, the rt2x00dev_{pci,usb}
	 * macro's should be used for correct typecasting.
	 */
	void *dev;
	struct device *device;
#define rt2x00dev_pci(__dev)	( (struct pci_dev*)(__dev)->dev )
#define rt2x00dev_usb(__dev)	( (struct usb_interface*)(__dev)->dev )

	/*
	 * Callback functions.
	 */
	const struct rt2x00_ops *ops;

	/*
	 * IEEE80211 control structure.
	 */
	struct ieee80211_hw *hw;
	struct ieee80211_hw_mode *hwmodes;
	unsigned int curr_hwmode;
#define HWMODE_B	0
#define HWMODE_G	1
#define HWMODE_A	2

	/*
	 * rfkill structure for RF state switching support.
	 * This will only be compiled in when required.
	 */
#ifdef CONFIG_RT2X00_LIB_RFKILL
	struct rfkill *rfkill;
	struct delayed_work rfkill_work;
#endif /* CONFIG_RT2X00_LIB_RFKILL */

	/*
	 * Device flags.
	 * In these flags the current status and some
	 * of the device capabilities are stored.
	 */
	unsigned long flags;
#define DEVICE_ENABLED_RADIO		1
#define DEVICE_ENABLED_RADIO_HW		2
#define DEVICE_INITIALIZED		3
#define DEVICE_INITIALIZED_HW		4
#define FIRMWARE_REQUIRED		5
#define FIRMWARE_LOADED			6
#define FIRMWARE_FAILED			7
#define INTERFACE_ENABLED		8
#define INTERFACE_ENABLED_MONITOR	9
#define INTERFACE_ENABLED_PROMISC	10
#define DEVICE_SUPPORT_ATIM		11
#define DEVICE_SUPPORT_HW_BUTTON	12
#define CONFIG_FRAME_TYPE		13
#define CONFIG_RF_SEQUENCE		14
#define CONFIG_EXTERNAL_LNA		15
#define CONFIG_EXTERNAL_LNA_A		16
#define CONFIG_EXTERNAL_LNA_BG		17
#define CONFIG_DOUBLE_ANTENNA		18
#define CONFIG_DISABLE_LINK_TUNING	19

	/*
	 * Chipset identification.
	 */
	struct rt2x00_chip chip;

	/*
	 * hw capability specifications.
	 */
	struct hw_mode_spec spec;

	/*
	 * Base address of device registers (PCI devices only).
	 */
	void __iomem *csr_addr;

	/*
	 * If enabled, the debugfs interface structures
	 * required for deregistration of debugfs.
	 */
	const struct rt2x00debug_intf *debugfs_intf;

	/*
	 * Queue for deferred work.
	 */
	struct workqueue_struct *workqueue;

	/*
	 * Interface configuration.
	 */
	struct interface interface;

	/*
	 * Link quality
	 */
	struct link link;

	/*
	 * EEPROM data.
	 */
	__le16 *eeprom;

	/*
	 * Active RF register values.
	 * These are stored here for easier working
	 * with the rf registers.
	 */
	u32 rf1;
	u32 rf2;
	u32 rf3;
	u32 rf4;

	/*
	 * Current TX power value.
	 */
	u16 tx_power;

	/*
	 * LED register (for rt61pci & rt73usb).
	 */
	u16 led_reg;

	/*
	 * Led mode (LED_MODE_*)
	 */
	u8 led_mode;

	/*
	 * EEPROM bus width (PCI devices only).
	 */
	u8 eeprom_width;

	/*
	 * Frequency offset (for rt61pci & rt73usb).
	 */
	u8 freq_offset;

	/*
	 * Low level statistics which will have
	 * to be kept up to date while device is running.
	 */
	struct ieee80211_low_level_stats low_level_stats;

	/*
	 * RX configuration information.
	 */
	struct ieee80211_rx_status rx_status;

	/*
	 * Data ring arrays for RX, TX and Beacon.
	 * The Beacon array also contains the Atim ring
	 * if that is supported by the device.
	 */
	struct data_ring *rx;
	struct data_ring *tx;
	struct data_ring *bcn;
};

static inline struct data_ring* rt2x00_get_ring(
	struct rt2x00_dev *rt2x00dev, const unsigned int queue)
{
	int atim = test_bit(DEVICE_SUPPORT_ATIM, &rt2x00dev->flags);

	/*
	 * Check if the rings have been allocated.
	 */
	if (!rt2x00dev->tx || !rt2x00dev->bcn)
		return NULL;

	/*
	 * Check if we are requesting a reqular TX ring,
	 * or if we are requesting a Beacon or Atim ring.
	 * For Atim rings, we should check if it is supported.
	 */
	if (queue < rt2x00dev->hw->queues)
		return &rt2x00dev->tx[queue];
	else if (queue == IEEE80211_TX_QUEUE_BEACON)
		return &rt2x00dev->bcn[0];
	else if (queue == IEEE80211_TX_QUEUE_AFTER_BEACON && atim)
		return &rt2x00dev->bcn[1];

	return NULL;
}

/*
 * For-each loop for the ring array.
 * Since the all rings are allocated as a single array,
 * we can start at the rx pointer and move forward to the tx rings.
 * The 1 + Atim check will assure that the address directly after
 * the ring array is obtained and the for-each loop exits correctly.
 */
#define ring_for_each(__dev, __entry)		\
	for ((__entry) = (__dev)->rx;		\
		(__entry) != &(__dev)->bcn[1 +	\
			test_bit(DEVICE_SUPPORT_ATIM, &rt2x00dev->flags)]; \
		(__entry)++)

#define txring_for_each(__dev, __entry)		\
	for ((__entry) = (__dev)->tx; (__entry) != (__dev)->bcn; (__entry)++)

/*
 * EEPROM access.
 * The EEPROM is being accessed by word index.
 */
static inline void* rt2x00_eeprom_addr(const struct rt2x00_dev *rt2x00dev,
	const u8 word)
{
	return (void*)&rt2x00dev->eeprom[word];
}

static inline void rt2x00_eeprom_read(const struct rt2x00_dev *rt2x00dev,
	const u8 word, u16 *data)
{
	*data = le16_to_cpu(rt2x00dev->eeprom[word]);
}

static inline void rt2x00_eeprom_write(const struct rt2x00_dev *rt2x00dev,
	const u8 word, u16 data)
{
	rt2x00dev->eeprom[word] = cpu_to_le16(data);
}

/*
 * Link tuning handlers
 */
static inline void rt2x00_start_link_tune(struct rt2x00_dev *rt2x00dev)
{
	rt2x00dev->link.count = 0;
	rt2x00dev->link.count_rssi = 0;
	rt2x00dev->link.total_rssi = 0;
	rt2x00dev->link.curr_noise = 0;

	queue_delayed_work(rt2x00dev->workqueue,
		&rt2x00dev->link.work, LINK_TUNE_INTERVAL);
}

static inline void rt2x00_stop_link_tune(struct rt2x00_dev *rt2x00dev)
{
	if (work_pending(&rt2x00dev->link.work.work))
		cancel_rearming_delayed_workqueue(
			rt2x00dev->workqueue, &rt2x00dev->link.work);
}

static inline void rt2x00_update_link_rssi(struct link *link, u32 rssi)
{
	link->count_rssi++;
	link->total_rssi += rssi;
}

static inline u32 rt2x00_get_link_rssi(struct link *link)
{
	u32 average = 0;

	if (link->count_rssi && link->total_rssi)
		average = link->total_rssi / link->count_rssi;

	link->count_rssi = 0;
	link->total_rssi = 0;

	return average;
}

/*
 * Chipset handlers
 */
static inline void rt2x00_set_chip(struct rt2x00_dev *rt2x00dev,
	const u16 rt, const u16 rf, const u32 rev)
{
	INFO(rt2x00dev,
		"Chipset detected - rt: %04x, rf: %04x, rev: %08x.\n",
		rt, rf, rev);

	rt2x00dev->chip.rt = rt;
	rt2x00dev->chip.rf = rf;
	rt2x00dev->chip.rev = rev;
}

static inline char rt2x00_rt(const struct rt2x00_chip *chipset, const u16 chip)
{
	return (chipset->rt == chip);
}

static inline char rt2x00_rf(const struct rt2x00_chip *chipset, const u16 chip)
{
	return (chipset->rf == chip);
}

static inline u16 rt2x00_rev(const struct rt2x00_chip *chipset)
{
	return chipset->rev;
}

/*
 * Device specific rate value.
 * We will have to create the device specific rate value
 * passed to the ieee80211 kernel. We need to make it a consist of
 * multiple fields because we want to store more then 1 device specific
 * values inside the value.
 *	1 - rate, stored as 100 kbit/s.
 *	2 - preamble, short_preamble enabled flag.
 *	3 - MASK_RATE, which rates are enabled in this mode, this mask
 *	corresponds with the TX register format for the current device.
 *	4 - plcp, 802.11b rates are device specific,
 *	802.11g rates are set according to the ieee802.11a-1999 p.14.
 * The bit to enable preamble is set in a seperate define.
 */
#define DEV_RATE	FIELD32(0x000007ff)
#define DEV_PREAMBLE	FIELD32(0x00000800)
#define DEV_RATEMASK	FIELD32(0x00fff000)
#define DEV_PLCP	FIELD32(0xff000000)

/*
 * Bitmask for MASK_RATE
 */
#define DEV_RATE_1MB	0x00000001
#define DEV_RATE_2MB	0x00000002
#define DEV_RATE_5_5MB	0x00000004
#define DEV_RATE_11MB	0x00000008
#define DEV_RATE_6MB	0x00000010
#define DEV_RATE_9MB	0x00000020
#define DEV_RATE_12MB	0x00000040
#define DEV_RATE_18MB	0x00000080
#define DEV_RATE_24MB	0x00000100
#define DEV_RATE_36MB	0x00000200
#define DEV_RATE_48MB	0x00000400
#define DEV_RATE_54MB	0x00000800

/*
 * Bitmask groups of bitrates
 */
#define DEV_BASIC_RATE \
	( DEV_RATE_1MB | DEV_RATE_2MB | DEV_RATE_5_5MB | DEV_RATE_11MB | \
	  DEV_RATE_6MB | DEV_RATE_12MB | DEV_RATE_24MB )

#define DEV_CCK_RATE \
	( DEV_RATE_1MB | DEV_RATE_2MB | DEV_RATE_5_5MB | DEV_RATE_11MB )

#define DEV_OFDM_RATE \
	( DEV_RATE_6MB | DEV_RATE_9MB | DEV_RATE_12MB | DEV_RATE_18MB | \
	  DEV_RATE_24MB | DEV_RATE_36MB | DEV_RATE_48MB | DEV_RATE_54MB )

/*
 * Macro's to set and get specific fields from the device specific val and val2
 * fields inside the ieee80211_rate entry.
 */
#define DEVICE_SET_RATE_FIELD(__value, __mask) \
	(int)( ((__value) << DEV_##__mask.bit_offset) & DEV_##__mask.bit_mask )

#define DEVICE_GET_RATE_FIELD(__value, __mask) \
	(int)( ((__value) & DEV_##__mask.bit_mask) >> DEV_##__mask.bit_offset )

/*
 * Duration calculations
 * The rate variable passed is: 100kbs.
 * To convert from bytes to bits we multiply size with 8,
 * then the size is multiplied with 10 to make the
 * real rate -> rate argument correction.
 */
static inline u16 get_duration(const unsigned int size, const u8 rate)
{
	return ((size * 8 * 10) / rate);
}

static inline u16 get_duration_res(const unsigned int size, const u8 rate)
{
	return ((size * 8 * 10) % rate);
}

#endif /* RT2X00_H */
