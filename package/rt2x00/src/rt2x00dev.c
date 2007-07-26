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
	Module: rt2x00lib
	Abstract: rt2x00 generic device routines.
	Supported chipsets: RT2460, RT2560, RT2570,
	rt2561, rt2561s, rt2661, rt2571W & rt2671.
 */

/*
 * Set enviroment defines for rt2x00.h
 */
#define DRV_NAME "rt2x00lib"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>

#include "rt2x00.h"
#include "rt2x00dev.h"

/*
 * Radio control handlers.
 */
int rt2x00lib_enable_radio(struct rt2x00_dev *rt2x00dev)
{
	int status;

	/*
	 * Don't enable the radio twice.
	 * or if the hardware button has been disabled.
	 */
	if (test_bit(DEVICE_ENABLED_RADIO, &rt2x00dev->flags) ||
	    (test_bit(DEVICE_SUPPORT_HW_BUTTON, &rt2x00dev->flags) &&
	     !test_bit(DEVICE_ENABLED_RADIO_HW, &rt2x00dev->flags)))
		return 0;

	status = rt2x00dev->ops->lib->set_device_state(
		rt2x00dev, STATE_RADIO_ON);
	if (status)
		return status;

	__set_bit(DEVICE_ENABLED_RADIO, &rt2x00dev->flags);

	rt2x00lib_toggle_rx(rt2x00dev, 1);

	ieee80211_start_queues(rt2x00dev->hw);

	return 0;
}

void rt2x00lib_disable_radio(struct rt2x00_dev *rt2x00dev)
{
	if (!__test_and_clear_bit(DEVICE_ENABLED_RADIO, &rt2x00dev->flags))
		return;

	ieee80211_stop_queues(rt2x00dev->hw);

	rt2x00lib_toggle_rx(rt2x00dev, 0);

	rt2x00dev->ops->lib->set_device_state(rt2x00dev, STATE_RADIO_OFF);
}

void rt2x00lib_toggle_rx(struct rt2x00_dev *rt2x00dev, int enable)
{
	/*
	 * When we are disabling the rx, we should also stop the link tuner.
	 */
	if (!enable && work_pending(&rt2x00dev->link.work.work))
		rt2x00_stop_link_tune(rt2x00dev);

	rt2x00dev->ops->lib->set_device_state(rt2x00dev,
		enable ? STATE_RADIO_RX_ON : STATE_RADIO_RX_OFF);

	/*
	 * When we are enabling the rx, we should also start the link tuner.
	 */
	if (enable)
		rt2x00_start_link_tune(rt2x00dev);
}

static void rt2x00lib_link_tuner(struct work_struct *work)
{
	struct rt2x00_dev *rt2x00dev =
		container_of(work, struct rt2x00_dev, link.work.work);
	int rssi;

	/*
	 * Update promisc mode (this function will first check
	 * if updating is really required).
	 */
	rt2x00lib_config_promisc(rt2x00dev, rt2x00dev->interface.promisc);

	/*
	 * Cancel all link tuning if the eeprom has indicated
	 * it is not required.
	 */
	if (test_bit(CONFIG_DISABLE_LINK_TUNING, &rt2x00dev->flags))
		return;

	/*
	 * Retrieve link quality.
	 * Also convert rssi to dBm using the max_rssi value.
	 */
	rssi = rt2x00_get_link_rssi(&rt2x00dev->link);
	rssi -= rt2x00dev->hw->max_rssi;

	rt2x00dev->ops->lib->link_tuner(rt2x00dev, rssi);

	/*
	 * Increase tuner counter, and reschedule the next link tuner run.
	 */
	rt2x00dev->link.count++;
	queue_delayed_work(rt2x00dev->workqueue, &rt2x00dev->link.work,
		LINK_TUNE_INTERVAL);
}

/*
 * Config handlers
 */
void rt2x00lib_config_type(struct rt2x00_dev *rt2x00dev, const int type)
{
	if (!(is_interface_present(&rt2x00dev->interface) ^
	      test_bit(INTERFACE_ENABLED, &rt2x00dev->flags)) &&
	    !(is_monitor_present(&rt2x00dev->interface) ^
	      test_bit(INTERFACE_ENABLED_MONITOR, &rt2x00dev->flags)))
		return;

	rt2x00dev->ops->lib->config_type(rt2x00dev, type);

	if (type != IEEE80211_IF_TYPE_MNTR) {
		if (is_interface_present(&rt2x00dev->interface))
			__set_bit(INTERFACE_ENABLED, &rt2x00dev->flags);
		else
			__clear_bit(INTERFACE_ENABLED, &rt2x00dev->flags);
	} else {
		if (is_monitor_present(&rt2x00dev->interface))
			__set_bit(INTERFACE_ENABLED_MONITOR,
				&rt2x00dev->flags);
		else
			__clear_bit(INTERFACE_ENABLED_MONITOR,
				&rt2x00dev->flags);
	}
}

void rt2x00lib_config_phymode(struct rt2x00_dev *rt2x00dev, const int phymode)
{
	if (rt2x00dev->rx_status.phymode == phymode)
		return;

	rt2x00dev->ops->lib->config_phymode(rt2x00dev, phymode);

	rt2x00dev->rx_status.phymode = phymode;
}

void rt2x00lib_config_channel(struct rt2x00_dev *rt2x00dev, const int value,
	const int channel, const int freq, const int txpower)
{
	if (channel == rt2x00dev->rx_status.channel)
		return;

	rt2x00dev->ops->lib->config_channel(rt2x00dev, value, channel, txpower);

	INFO(rt2x00dev, "Switching channel. "
		"RF1: 0x%08x, RF2: 0x%08x, RF3: 0x%08x, RF3: 0x%08x.\n",
		rt2x00dev->rf1, rt2x00dev->rf2,
		rt2x00dev->rf3, rt2x00dev->rf4);

	rt2x00dev->rx_status.freq = freq;
	rt2x00dev->rx_status.channel = channel;
}

void rt2x00lib_config_promisc(struct rt2x00_dev *rt2x00dev, const int promisc)
{
	/*
	 * Monitor mode implies promisc mode enabled.
	 * In all other instances, check if we need to toggle promisc mode.
	 */
	if (is_monitor_present(&rt2x00dev->interface) &&
	    !test_bit(INTERFACE_ENABLED_PROMISC, &rt2x00dev->flags)) {
		rt2x00dev->ops->lib->config_promisc(rt2x00dev, 1);
		__set_bit(INTERFACE_ENABLED_PROMISC, &rt2x00dev->flags);
	}

	if (test_bit(INTERFACE_ENABLED_PROMISC, &rt2x00dev->flags) != promisc) {
		rt2x00dev->ops->lib->config_promisc(rt2x00dev, promisc);
		__change_bit(INTERFACE_ENABLED_PROMISC, &rt2x00dev->flags);
	}
}

void rt2x00lib_config_txpower(struct rt2x00_dev *rt2x00dev, const int txpower)
{
	if (txpower == rt2x00dev->tx_power)
		return;

	rt2x00dev->ops->lib->config_txpower(rt2x00dev, txpower);

	rt2x00dev->tx_power = txpower;
}

void rt2x00lib_config_antenna(struct rt2x00_dev *rt2x00dev,
	const int antenna_tx, const int antenna_rx)
{
	if (rt2x00dev->rx_status.antenna == antenna_rx)
		return;

	rt2x00dev->ops->lib->config_antenna(rt2x00dev, antenna_tx, antenna_rx);

	rt2x00dev->rx_status.antenna = antenna_rx;
}

/*
 * Driver initialization handlers.
 */
static void rt2x00lib_channel(struct ieee80211_channel *entry,
	const int channel, const int tx_power, const int value)
{
	entry->chan = channel;
	if (channel <= 14)
		entry->freq = 2407 + (5 * channel);
	else
		entry->freq = 5000 + (5 * channel);
	entry->val = value;
	entry->flag =
		IEEE80211_CHAN_W_IBSS |
		IEEE80211_CHAN_W_ACTIVE_SCAN |
		IEEE80211_CHAN_W_SCAN;
	entry->power_level = tx_power;
	entry->antenna_max = 0xff;
}

static void rt2x00lib_rate(struct ieee80211_rate *entry,
	const int rate,const int mask, const int plcp, const int flags)
{
	entry->rate = rate;
	entry->val =
		DEVICE_SET_RATE_FIELD(rate, RATE) |
		DEVICE_SET_RATE_FIELD(mask, RATEMASK) |
		DEVICE_SET_RATE_FIELD(plcp, PLCP);
	entry->flags = flags;
	entry->val2 = entry->val;
	if (entry->flags & IEEE80211_RATE_PREAMBLE2)
		entry->val2 |= DEVICE_SET_RATE_FIELD(1, PREAMBLE);
	entry->min_rssi_ack = 0;
	entry->min_rssi_ack_delta = 0;
}

static int rt2x00lib_init_hw_modes(struct rt2x00_dev *rt2x00dev,
	struct hw_mode_spec *spec)
{
	struct ieee80211_hw *hw = rt2x00dev->hw;
	struct ieee80211_hw_mode *hwmodes;
	struct ieee80211_channel *channels;
	struct ieee80211_rate *rates;
	unsigned int i;
	unsigned char tx_power;

	hwmodes = kzalloc(sizeof(*hwmodes) * spec->num_modes, GFP_KERNEL);
	if (!hwmodes)
		goto exit;

	channels = kzalloc(sizeof(*channels) * spec->num_channels, GFP_KERNEL);
	if (!channels)
		goto exit_free_modes;

	rates = kzalloc(sizeof(*rates) * spec->num_rates, GFP_KERNEL);
	if (!rates)
		goto exit_free_channels;

	/*
	 * Initialize Rate list.
	 */
	rt2x00lib_rate(&rates[0], 10, 0x001, 0x00, IEEE80211_RATE_CCK);
	rt2x00lib_rate(&rates[1], 20, 0x003, 0x01, IEEE80211_RATE_CCK_2);
	rt2x00lib_rate(&rates[2], 55, 0x007, 0x02, IEEE80211_RATE_CCK_2);
	rt2x00lib_rate(&rates[3], 110, 0x00f, 0x03, IEEE80211_RATE_CCK_2);

	if (spec->num_rates > 4) {
		rt2x00lib_rate(&rates[4], 60, 0x01f, 0x0b, IEEE80211_RATE_OFDM);
		rt2x00lib_rate(&rates[5], 90, 0x03f, 0x0f, IEEE80211_RATE_OFDM);
		rt2x00lib_rate(&rates[6], 120, 0x07f, 0x0a, IEEE80211_RATE_OFDM);
		rt2x00lib_rate(&rates[7], 180, 0x0ff, 0x0e, IEEE80211_RATE_OFDM);
		rt2x00lib_rate(&rates[8], 240, 0x1ff, 0x09, IEEE80211_RATE_OFDM);
		rt2x00lib_rate(&rates[9], 360, 0x3ff, 0x0d, IEEE80211_RATE_OFDM);
		rt2x00lib_rate(&rates[10], 480, 0x7ff, 0x08, IEEE80211_RATE_OFDM);
		rt2x00lib_rate(&rates[11], 540, 0xfff, 0x0c, IEEE80211_RATE_OFDM);
	}

	/*
	 * Initialize Channel list.
	 */
	for (i = 0; i < 14; i++)
		rt2x00lib_channel(&channels[i], i + 1,
			spec->tx_power_bg[i], spec->chan_val_bg[i]);

	if (spec->num_channels > 14) {
		for (i = 14; i < spec->num_channels; i++) {
			if (i < 22)
				channels[i].chan = 36;
			else if (i < 33)
				channels[i].chan = 100;
			else
				channels[i].chan = 149;
			channels[i].chan += ((i - 14) * 4);

			if (spec->tx_power_a)
				tx_power = spec->tx_power_a[i];
			else
				tx_power = spec->tx_power_default;

			rt2x00lib_channel(&channels[i],
				channels[i].chan, tx_power,
				spec->chan_val_a[i]);
		}
	}

	/*
	 * Intitialize 802.11b
	 * Rates: CCK.
	 * Channels: OFDM.
	 */
	if (spec->num_modes > HWMODE_B) {
		hwmodes[HWMODE_B].mode = MODE_IEEE80211B;
		hwmodes[HWMODE_B].num_channels = 14;
		hwmodes[HWMODE_B].num_rates = 4;
		hwmodes[HWMODE_B].channels = channels;
		hwmodes[HWMODE_B].rates = rates;
	}

	/*
	 * Intitialize 802.11g
	 * Rates: CCK, OFDM.
	 * Channels: OFDM.
	 */
	if (spec->num_modes > HWMODE_G) {
		hwmodes[HWMODE_G].mode = MODE_IEEE80211G;
		hwmodes[HWMODE_G].num_channels = 14;
		hwmodes[HWMODE_G].num_rates = spec->num_rates;
		hwmodes[HWMODE_G].channels = channels;
		hwmodes[HWMODE_G].rates = rates;
	}

	/*
	 * Intitialize 802.11a
	 * Rates: OFDM.
	 * Channels: OFDM, UNII, HiperLAN2.
	 */
	if (spec->num_modes > HWMODE_A) {
		hwmodes[HWMODE_A].mode = MODE_IEEE80211A;
		hwmodes[HWMODE_A].num_channels = spec->num_channels - 14;
		hwmodes[HWMODE_A].num_rates = spec->num_rates - 4;
		hwmodes[HWMODE_A].channels = &channels[14];
		hwmodes[HWMODE_A].rates = &rates[4];
	}

	if (spec->num_modes > HWMODE_G &&
	    ieee80211_register_hwmode(hw, &hwmodes[HWMODE_G]))
		goto exit_free_rates;

	if (spec->num_modes > HWMODE_B &&
	    ieee80211_register_hwmode(hw, &hwmodes[HWMODE_B]))
		goto exit_free_rates;

	if (spec->num_modes > HWMODE_A &&
	    ieee80211_register_hwmode(hw, &hwmodes[HWMODE_A]))
		goto exit_free_rates;

	rt2x00dev->hwmodes = hwmodes;

	return 0;

exit_free_rates:
	kfree(rates);

exit_free_channels:
	kfree(channels);

exit_free_modes:
	kfree(hwmodes);

exit:
	ERROR(rt2x00dev, "Allocation ieee80211 modes failed.\n");
	return -ENOMEM;
}

static void rt2x00lib_deinit_hw(struct rt2x00_dev *rt2x00dev)
{
	if (test_bit(DEVICE_INITIALIZED_HW, &rt2x00dev->flags))
		ieee80211_unregister_hw(rt2x00dev->hw);

	if (likely(rt2x00dev->hwmodes)) {
		kfree(rt2x00dev->hwmodes->channels);
		kfree(rt2x00dev->hwmodes->rates);
		kfree(rt2x00dev->hwmodes);
		rt2x00dev->hwmodes = NULL;
	}
}

static int rt2x00lib_init_hw(struct rt2x00_dev *rt2x00dev)
{
	struct hw_mode_spec *spec = &rt2x00dev->spec;
	int status;

	/*
	 * Initialize device.
	 */
	SET_IEEE80211_DEV(rt2x00dev->hw, rt2x00dev->device);

	/*
	 * Initialize MAC address.
	 */
	if (!is_valid_ether_addr(spec->mac_addr)) {
		ERROR(rt2x00dev, "Invalid MAC addr: " MAC_FMT ".\n",
			MAC_ARG(spec->mac_addr));
		return -EINVAL;
	}

	rt2x00dev->ops->lib->config_mac_addr(rt2x00dev, spec->mac_addr);
	SET_IEEE80211_PERM_ADDR(rt2x00dev->hw, spec->mac_addr);

	/*
	 * Initialize HW modes.
	 */
	status = rt2x00lib_init_hw_modes(rt2x00dev, spec);
	if (status)
		return status;

	/*
	 * Register HW.
	 */
	status = ieee80211_register_hw(rt2x00dev->hw);
	if (status) {
		rt2x00lib_deinit_hw(rt2x00dev);
		return status;
	}

	__set_bit(DEVICE_INITIALIZED_HW, &rt2x00dev->flags);

	return 0;
}

/*
 * Initialization/uninitialization handlers.
 */
static int rt2x00lib_alloc_ring(struct data_ring *ring,
	const u16 max_entries, const u16 data_size, const u16 desc_size)
{
	struct data_entry *entry;
	unsigned int i;

	ring->stats.limit = max_entries;
	ring->data_size = data_size;
	ring->desc_size = desc_size;

	/*
	 * Allocate all ring entries.
	 */
	entry = kzalloc(ring->stats.limit * sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;

	for (i = 0; i < ring->stats.limit; i++) {
		entry[i].flags = 0;
		entry[i].ring = ring;
		entry[i].skb = NULL;
	}

	ring->entry = entry;

	return 0;
}

static int rt2x00lib_allocate_rings(struct rt2x00_dev *rt2x00dev)
{
	struct data_ring *ring;

	/*
	 * Allocate the RX ring.
	 */
	if (rt2x00lib_alloc_ring(rt2x00dev->rx,
		RX_ENTRIES, DATA_FRAME_SIZE, rt2x00dev->ops->rxd_size))
		return -ENOMEM;

	/*
	 * First allocate the TX rings.
	 */
	txring_for_each(rt2x00dev, ring) {
		if (rt2x00lib_alloc_ring(ring,
			TX_ENTRIES, DATA_FRAME_SIZE, rt2x00dev->ops->txd_size))
			return -ENOMEM;
	}

	/*
	 * Allocate the BEACON ring.
	 */
	if (rt2x00lib_alloc_ring(&rt2x00dev->bcn[0],
		BEACON_ENTRIES, MGMT_FRAME_SIZE, rt2x00dev->ops->txd_size))
		return -ENOMEM;

	/*
	 * Allocate the Atim ring.
	 */
	if (test_bit(DEVICE_SUPPORT_ATIM, &rt2x00dev->flags)) {
		if (rt2x00lib_alloc_ring(&rt2x00dev->bcn[1],
			ATIM_ENTRIES, DATA_FRAME_SIZE, rt2x00dev->ops->txd_size))
			return -ENOMEM;
	}

	return 0;
}

static void rt2x00lib_free_rings(struct rt2x00_dev *rt2x00dev)
{
	struct data_ring *ring;

	ring_for_each(rt2x00dev, ring) {
		kfree(ring->entry);
		ring->entry = NULL;
	}
}

int rt2x00lib_initialize(struct rt2x00_dev *rt2x00dev)
{
	int status;

	if (test_bit(DEVICE_INITIALIZED, &rt2x00dev->flags))
		return 0;

	/*
	 * Allocate all data rings.
	 */
	status = rt2x00lib_allocate_rings(rt2x00dev);
	if (status) {
		ERROR(rt2x00dev, "DMA allocation failed.\n");
		return status;
	}

	/*
	 * Initialize the device.
	 */
	status = rt2x00dev->ops->lib->initialize(rt2x00dev);
	if (status)
		goto exit;

	__set_bit(DEVICE_INITIALIZED, &rt2x00dev->flags);

	/*
	 * Register the rfkill handler.
	 */
	status = rt2x00lib_register_rfkill(rt2x00dev);
	if (status)
		goto exit_unitialize;

	return 0;

exit_unitialize:
	rt2x00lib_uninitialize(rt2x00dev);

exit:
	rt2x00lib_free_rings(rt2x00dev);

	return status;
}

void rt2x00lib_uninitialize(struct rt2x00_dev *rt2x00dev)
{
	if (!__test_and_clear_bit(DEVICE_INITIALIZED, &rt2x00dev->flags))
		return;

	/*
	 * Flush out all pending work.
	 */
	flush_workqueue(rt2x00dev->workqueue);

	/*
	 * Unregister rfkill.
	 */
	rt2x00lib_unregister_rfkill(rt2x00dev);

	/*
	 * Allow the HW to uninitialize.
	 */
	rt2x00dev->ops->lib->uninitialize(rt2x00dev);

	/*
	 * Free allocated datarings.
	 */
	rt2x00lib_free_rings(rt2x00dev);
}

/*
 * driver allocation handlers.
 */
static int rt2x00lib_alloc_rings(struct rt2x00_dev *rt2x00dev)
{
	struct data_ring *ring;
	unsigned int ring_num;

	/*
	 * We need the following rings:
	 * RX: 1
	 * TX: hw->queues
	 * Beacon: 1
	 * Atim: 1 (if supported)
	 */
	ring_num = 2 + rt2x00dev->hw->queues +
		test_bit(DEVICE_SUPPORT_ATIM, &rt2x00dev->flags);

	ring = kzalloc(sizeof(*ring) * ring_num, GFP_KERNEL);
	if (!ring) {
		ERROR(rt2x00dev, "Ring allocation failed.\n");
		return -ENOMEM;
	}

	/*
	 * Initialize pointers
	 */
	rt2x00dev->rx = &ring[0];
	rt2x00dev->tx = &ring[1];
	rt2x00dev->bcn = &ring[1 + rt2x00dev->hw->queues];

	/*
	 * Initialize ring parameters.
	 * cw_min: 2^5 = 32.
	 * cw_max: 2^10 = 1024.
	 */
	ring_for_each(rt2x00dev, ring) {
		ring->rt2x00dev = rt2x00dev;
		ring->tx_params.aifs = 2;
		ring->tx_params.cw_min = 5;
		ring->tx_params.cw_max = 10;
	}

	return 0;
}

int rt2x00lib_probe_dev(struct rt2x00_dev *rt2x00dev)
{
	int retval = -ENOMEM;

	/*
	 * Create workqueue.
	 */
	rt2x00dev->workqueue = create_singlethread_workqueue(DRV_NAME);
	if (!rt2x00dev->workqueue)
		goto exit;

	/*
	 * Let the driver probe the device to detect the capabilities.
	 */
	retval = rt2x00dev->ops->lib->init_hw(rt2x00dev);
	if (retval) {
		ERROR(rt2x00dev, "Failed to allocate device.\n");
		goto exit;
	}

	/*
	 * Initialize configuration work.
	 */
	INIT_DELAYED_WORK(&rt2x00dev->link.work, rt2x00lib_link_tuner);

	/*
	 * Reset current working type.
	 */
	rt2x00dev->interface.type = -EINVAL;

	/*
	 * Allocate ring array.
	 */
	retval = rt2x00lib_alloc_rings(rt2x00dev);
	if (retval)
		goto exit;

	/*
	 * Initialize ieee80211 structure.
	 */
	retval = rt2x00lib_init_hw(rt2x00dev);
	if (retval) {
		ERROR(rt2x00dev, "Failed to initialize hw.\n");
		goto exit;
	}

	/*
	 * Allocatie rfkill.
	 */
	retval = rt2x00lib_allocate_rfkill(rt2x00dev);
	if (retval)
		goto exit;

	/*
	 * Open the debugfs entry.
	 */
	rt2x00debug_register(rt2x00dev);

	/*
	 * Check if we need to load the firmware.
	 */
	if (test_bit(FIRMWARE_REQUIRED, &rt2x00dev->flags)) {
		/*
		 * Request firmware and wait with further
		 * initializing of the card until the firmware
		 * has been loaded.
		 */
		retval = rt2x00lib_load_firmware(rt2x00dev);
		if (retval)
			goto exit;
	}

	return 0;

exit:
	rt2x00lib_remove_dev(rt2x00dev);

	return retval;
}
EXPORT_SYMBOL_GPL(rt2x00lib_probe_dev);

void rt2x00lib_remove_dev(struct rt2x00_dev *rt2x00dev)
{
	/*
	 * Disable radio.
	 */
	rt2x00lib_disable_radio(rt2x00dev);

	/*
	 * Uninitialize device.
	 */
	rt2x00lib_uninitialize(rt2x00dev);

	/*
	 * Close debugfs entry.
	 */
	rt2x00debug_deregister(rt2x00dev);

	/*
	 * Free rfkill
	 */
	rt2x00lib_free_rfkill(rt2x00dev);

	/*
	 * Free ieee80211_hw memory.
	 */
	rt2x00lib_deinit_hw(rt2x00dev);

	/*
	 * Free workqueue.
	 */
	if (likely(rt2x00dev->workqueue)) {
		destroy_workqueue(rt2x00dev->workqueue);
		rt2x00dev->workqueue = NULL;
	}

	/*
	 * Free ring structures.
	 */
	kfree(rt2x00dev->rx);
	rt2x00dev->rx = NULL;
	rt2x00dev->tx = NULL;
	rt2x00dev->bcn = NULL;

	/*
	 * Free EEPROM memory.
	 */
	kfree(rt2x00dev->eeprom);
	rt2x00dev->eeprom = NULL;
}
EXPORT_SYMBOL_GPL(rt2x00lib_remove_dev);

/*
 * Device state handlers
 */
int rt2x00lib_suspend(struct rt2x00_dev *rt2x00dev,
	pm_message_t state)
{
	int retval;

	NOTICE(rt2x00dev, "Going to sleep.\n");

	rt2x00lib_disable_radio(rt2x00dev);

	/*
	 * Set device mode to sleep for power management.
	 */
	retval = rt2x00dev->ops->lib->set_device_state(rt2x00dev, STATE_SLEEP);
	if (retval)
		return retval;

	rt2x00lib_remove_dev(rt2x00dev);

	return 0;
}
EXPORT_SYMBOL_GPL(rt2x00lib_suspend);

int rt2x00lib_resume(struct rt2x00_dev *rt2x00dev)
{
	int retval;

	NOTICE(rt2x00dev, "Waking up.\n");

	retval = rt2x00lib_probe_dev(rt2x00dev);
	if (retval) {
		ERROR(rt2x00dev, "Failed to allocate device.\n");
		return retval;
	}

	/*
	 * Set device mode to awake for power management.
	 */
	retval = rt2x00dev->ops->lib->set_device_state(rt2x00dev, STATE_AWAKE);
	if (retval)
		return retval;

	return 0;
}
EXPORT_SYMBOL_GPL(rt2x00lib_resume);

/*
 * Interrupt context handlers.
 */
void rt2x00lib_txdone(struct data_entry *entry,
	const int status, const int retry)
{
	struct rt2x00_dev *rt2x00dev = entry->ring->rt2x00dev;
	struct ieee80211_tx_status *tx_status = &entry->tx_status;
	struct ieee80211_low_level_stats *stats = &rt2x00dev->low_level_stats;

	/*
	 * Update TX statistics.
	 */
	tx_status->flags = 0;
	tx_status->ack_signal = 0;
	tx_status->excessive_retries = (status == TX_FAIL_RETRY);
	tx_status->retry_count = retry;

	if (!(tx_status->control.flags & IEEE80211_TXCTL_NO_ACK)) {
		if (status == TX_SUCCESS || status == TX_SUCCESS_RETRY)
			tx_status->flags |= IEEE80211_TX_STATUS_ACK;
		else
			stats->dot11ACKFailureCount++;
	}

	tx_status->queue_length = entry->ring->stats.limit;
	tx_status->queue_number = tx_status->control.queue;

	if (tx_status->control.flags & IEEE80211_TXCTL_USE_RTS_CTS) {
		if (status == TX_SUCCESS || status == TX_SUCCESS_RETRY)
			stats->dot11RTSSuccessCount++;
		else
			stats->dot11RTSFailureCount++;
	}

	/*
	 * Send the tx_status to mac80211,
	 * that method also cleans up the skb structure.
	 */
	ieee80211_tx_status_irqsafe(rt2x00dev->hw, entry->skb, tx_status);

	entry->skb = NULL;
}
EXPORT_SYMBOL_GPL(rt2x00lib_txdone);

void rt2x00lib_rxdone(struct data_entry *entry, char *data,
	const int size, const int signal, const int rssi, const int ofdm)
{
	struct rt2x00_dev *rt2x00dev = entry->ring->rt2x00dev;
	struct ieee80211_rx_status *rx_status = &rt2x00dev->rx_status;
	struct ieee80211_hw_mode *mode;
	struct ieee80211_rate *rate;
	struct sk_buff *skb;
	unsigned int i;
	int val = 0;

	/*
	 * Update RX statistics.
	 */
	mode = &rt2x00dev->hwmodes[rt2x00dev->curr_hwmode];
	for (i = 0; i < mode->num_rates; i++) {
		rate = &mode->rates[i];

		/*
		 * When frame was received with an OFDM bitrate,
		 * the signal is the PLCP value. If it was received with
		 * a CCK bitrate the signal is the rate in 0.5kbit/s.
		 */
		if (!ofdm)
			val = DEVICE_GET_RATE_FIELD(rate->val, RATE);
		else
			val = DEVICE_GET_RATE_FIELD(rate->val, PLCP);

		if (val == signal) {
			/*
			 * Check for preamble bit.
			 */
			if (signal & 0x08)
				val = rate->val2;
			val = rate->val;
			break;
		}
	}

	rx_status->rate = val;
	rx_status->ssi = rssi;
	rx_status->noise = rt2x00dev->link.curr_noise;
	rt2x00_update_link_rssi(&rt2x00dev->link, rssi);

	/*
	 * Let's allocate a sk_buff where we can store the received data in,
	 * note that if data is NULL, we still have to allocate a sk_buff
	 * but that we should use that to replace the sk_buff which is already
	 * inside the entry.
	 */
	skb = dev_alloc_skb(size + NET_IP_ALIGN);
	if (!skb)
		return;

	skb_reserve(skb, NET_IP_ALIGN);
	skb_put(skb, size);

	if (data) {
		memcpy(skb->data, data, size);
		entry->skb = skb;
		skb = NULL;
	}

	ieee80211_rx_irqsafe(rt2x00dev->hw, entry->skb, rx_status);
	entry->skb = skb;
}
EXPORT_SYMBOL_GPL(rt2x00lib_rxdone);

/*
 * TX descriptor initializer
 */
void rt2x00lib_write_tx_desc(struct rt2x00_dev *rt2x00dev,
	struct data_entry *entry, struct data_desc *txd,
	struct ieee80211_hdr *ieee80211hdr, unsigned int length,
	struct ieee80211_tx_control *control)
{
	struct data_entry_desc desc;
	int tx_rate;
	int bitrate;
	int duration;
	int residual;
	u16 frame_control;
	u16 seq_ctrl;

	/*
	 * Identify queue
	 */
	if (control->queue < rt2x00dev->hw->queues)
		desc.queue = control->queue;
	else
		desc.queue = 15;

	/*
	 * Read required fields from ieee80211 header.
	 */
	frame_control = le16_to_cpu(ieee80211hdr->frame_control);
	seq_ctrl = le16_to_cpu(ieee80211hdr->seq_ctrl);

	tx_rate = control->tx_rate;

	/*
	 * Check if this is a rts frame
	 */
	if (is_rts_frame(frame_control)) {
		__set_bit(ENTRY_TXD_RTS_FRAME, &entry->flags);
		if (control->rts_cts_rate)
			tx_rate = control->rts_cts_rate;
	}

	/*
	 * Check for OFDM
	 */
	if (DEVICE_GET_RATE_FIELD(tx_rate, RATEMASK) & DEV_OFDM_RATE)
		__set_bit(ENTRY_TXD_OFDM_RATE, &entry->flags);

	/*
	 * Check if more fragments are pending
	 */
	if (ieee80211_get_morefrag(ieee80211hdr))
		__set_bit(ENTRY_TXD_MORE_FRAG, &entry->flags);

	/*
	 * Check if this is a new sequence
	 */
	if ((seq_ctrl & IEEE80211_SCTL_FRAG) == 0)
		__set_bit(ENTRY_TXD_NEW_SEQ, &entry->flags);

	/*
	 * Beacons and probe responses require the tsf timestamp
	 * to be inserted into the frame.
	 */
	if (control->queue == IEEE80211_TX_QUEUE_BEACON ||
	    is_probe_resp(frame_control))
		__set_bit(ENTRY_TXD_REQ_TIMESTAMP, &entry->flags);

	/*
	 * Check if ACK is required
	 */
	if (!(control->flags & IEEE80211_TXCTL_NO_ACK))
		__set_bit(ENTRY_TXD_REQ_ACK, &entry->flags);

	/*
	 * Determine with what IFS priority this frame should be send.
	 * Set ifs to IFS_SIFS when the this is not the first fragment,
	 * or this fragment came after RTS/CTS.
	 */
	if ((seq_ctrl & IEEE80211_SCTL_FRAG) > 0 ||
	    test_bit(ENTRY_TXD_RTS_FRAME, &entry->flags))
		desc.ifs = IFS_SIFS;
	else
		desc.ifs = IFS_BACKOFF;

	/*
	 * How the length should be processed depends
	 * on if we are working with OFDM rates or not.
	 */
	if (test_bit(ENTRY_TXD_OFDM_RATE, &entry->flags)) {
		residual = 0;
		desc.length_high = ((length + FCS_LEN) >> 6) & 0x3f;
		desc.length_low = ((length + FCS_LEN) & 0x3f);

	} else {
		bitrate = DEVICE_GET_RATE_FIELD(tx_rate, RATE);

		/*
		 * Convert length to microseconds.
		 */
		residual = get_duration_res(length + FCS_LEN, bitrate);
		duration = get_duration(length + FCS_LEN, bitrate);

		if (residual != 0)
			duration++;

		desc.length_high = duration >> 8;
		desc.length_low = duration & 0xff;
	}

	/*
	 * Create the signal and service values.
	 */
	desc.signal = DEVICE_GET_RATE_FIELD(tx_rate, PLCP);
	if (DEVICE_GET_RATE_FIELD(tx_rate, PREAMBLE))
		desc.signal |= 0x08;

	desc.service = 0x04;
	if (residual <= (8 % 11))
		desc.service |= 0x80;

	rt2x00dev->ops->lib->write_tx_desc(rt2x00dev, entry, txd, &desc,
		ieee80211hdr, length, control);
}
EXPORT_SYMBOL_GPL(rt2x00lib_write_tx_desc);

/*
 * rt2x00lib module information.
 */
MODULE_AUTHOR(DRV_PROJECT);
MODULE_VERSION(DRV_VERSION);
MODULE_DESCRIPTION("rt2x00 library");
MODULE_LICENSE("GPL");
