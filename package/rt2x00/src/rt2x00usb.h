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
	Module: rt2x00usb
	Abstract: Data structures for the rt2x00usb module.
	Supported chipsets: rt2570, rt2571W & rt2671.
 */

#ifndef RT2X00USB_H
#define RT2X00USB_H

/*
 * This variable should be used with the
 * usb_driver structure initialization.
 */
#define USB_DEVICE_DATA(__ops)	.driver_info = (kernel_ulong_t)(__ops)

/*
 * Register defines.
 * When register access attempts should be repeated
 * only REGISTER_BUSY_COUNT attempts with a delay
 * of REGISTER_BUSY_DELAY us should be taken.
 * For USB vendor requests we need to pass a timeout
 * time in ms, for this we use the REGISTER_TIMEOUT,
 * however when loading firmware a higher value is
 * required. For that we use the REGISTER_TIMEOUT_FIRMWARE.
 */
#define REGISTER_BUSY_COUNT		5
#define REGISTER_BUSY_DELAY		100
#define REGISTER_TIMEOUT		20
#define REGISTER_TIMEOUT_FIRMWARE	1000

/*
 * USB request types.
 */
#define USB_VENDOR_REQUEST	( USB_TYPE_VENDOR | USB_RECIP_DEVICE )
#define USB_VENDOR_REQUEST_IN	( USB_DIR_IN | USB_VENDOR_REQUEST )
#define USB_VENDOR_REQUEST_OUT	( USB_DIR_OUT | USB_VENDOR_REQUEST )

/*
 * USB vendor commands.
 */
#define USB_DEVICE_MODE		0x01
#define USB_SINGLE_WRITE	0x02
#define USB_SINGLE_READ		0x03
#define USB_MULTI_WRITE		0x06
#define USB_MULTI_READ		0x07
#define USB_EEPROM_WRITE	0x08
#define USB_EEPROM_READ		0x09
#define USB_LED_CONTROL		0x0a	/* RT73USB */
#define USB_RX_CONTROL		0x0c

/*
 * Device modes offset
 */
#define USB_MODE_RESET		0x01
#define USB_MODE_UNPLUG		0x02
#define USB_MODE_FUNCTION	0x03
#define USB_MODE_TEST		0x04
#define USB_MODE_SLEEP		0x07	/* RT73USB */
#define USB_MODE_FIRMWARE	0x08	/* RT73USB */
#define USB_MODE_WAKEUP		0x09	/* RT73USB */

/*
 * USB devices need an additional Beacon (guardian beacon) to be generated.
 */
#undef BEACON_ENTRIES
#define BEACON_ENTRIES 2

/*
 * Interfacing with the HW.
 */
int rt2x00usb_vendor_request(const struct rt2x00_dev *rt2x00dev,
	const u8 request, const u8 type, const u16 offset,
	u32 value, void *buffer, const u16 buffer_length, const u16 timeout);

/*
 * Radio handlers
 */
void rt2x00usb_enable_radio(struct rt2x00_dev *rt2x00dev);
void rt2x00usb_disable_radio(struct rt2x00_dev *rt2x00dev);

/*
 * Beacon handlers.
 */
int rt2x00usb_beacon_update(struct ieee80211_hw *hw, struct sk_buff *skb,
	struct ieee80211_tx_control *control);
void rt2x00usb_beacondone(struct urb *urb);

/*
 * TX data handlers.
 */
int rt2x00usb_write_tx_data(struct rt2x00_dev *rt2x00dev,
	struct data_ring *ring, struct sk_buff *skb,
	struct ieee80211_tx_control *control);

/*
 * Device initialization handlers.
 */
int rt2x00usb_initialize(struct rt2x00_dev *rt2x00dev);
void rt2x00usb_uninitialize(struct rt2x00_dev *rt2x00dev);

/*
 * USB driver handlers.
 */
int rt2x00usb_probe(struct usb_interface *usb_intf,
	const struct usb_device_id *id);
void rt2x00usb_disconnect(struct usb_interface *usb_intf);
#ifdef CONFIG_PM
int rt2x00usb_suspend(struct usb_interface *usb_intf, pm_message_t state);
int rt2x00usb_resume(struct usb_interface *usb_intf);
#endif /* CONFIG_PM */

#endif /* RT2X00USB_H */
