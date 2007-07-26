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
	Abstract: rt2x00 rfkill specific routines.
	Supported chipsets: RT2460, RT2560, rt2561, rt2561s, rt2661.
 */

/*
 * Set enviroment defines for rt2x00.h
 */
#define DRV_NAME "rt2x00lib"

#include <linux/rfkill.h>

#include "rt2x00.h"

static int rt2x00lib_toggle_radio(void *data, enum rfkill_state state)
{
	struct rt2x00_dev* rt2x00dev = data;
	int retval = 0;

	if (unlikely(!rt2x00dev))
		return 0;

	/*
	 * Only continue if we have an active interface,
	 * either monitor or non-monitor should be present.
	 */
	if (!is_interface_present(&rt2x00dev->interface) &&
	    !is_monitor_present(&rt2x00dev->interface))
		return 0;

	if (state == RFKILL_STATE_ON) {
		INFO(rt2x00dev, "Hardware button pressed, enabling radio.\n");
		__set_bit(DEVICE_ENABLED_RADIO_HW, &rt2x00dev->flags);
		retval = rt2x00lib_enable_radio(rt2x00dev);
	} else if (state == RFKILL_STATE_OFF) {
		INFO(rt2x00dev, "Hardware button pressed, disabling radio.\n");
		__clear_bit(DEVICE_ENABLED_RADIO_HW, &rt2x00dev->flags);
		rt2x00lib_disable_radio(rt2x00dev);
	}

	return retval;
}

static void rt2x00lib_rfkill_poll(struct work_struct *work)
{
	struct rt2x00_dev *rt2x00dev =
		container_of(work, struct rt2x00_dev, rfkill_work.work);

	rfkill_switch_all(rt2x00dev->rfkill->type,
		rt2x00dev->ops->lib->rfkill_poll(rt2x00dev));

	queue_delayed_work(rt2x00dev->workqueue, &rt2x00dev->rfkill_work,
		RFKILL_POLL_INTERVAL);
}

int rt2x00lib_register_rfkill(struct rt2x00_dev *rt2x00dev)
{
	int status = rfkill_register(rt2x00dev->rfkill);
	if (status) {
		ERROR(rt2x00dev, "Failed to register rfkill handler.\n");
		return status;
	}

	rt2x00lib_rfkill_poll(&rt2x00dev->rfkill_work.work);

	return !schedule_delayed_work(&rt2x00dev->rfkill_work,
		RFKILL_POLL_INTERVAL);
}

void rt2x00lib_unregister_rfkill(struct rt2x00_dev *rt2x00dev)
{
	if (delayed_work_pending(&rt2x00dev->rfkill_work))
		cancel_rearming_delayed_workqueue(
			rt2x00dev->workqueue, &rt2x00dev->rfkill_work);

	rfkill_unregister(rt2x00dev->rfkill);
}

int rt2x00lib_allocate_rfkill(struct rt2x00_dev *rt2x00dev)
{
	struct rfkill *rfkill;

	if (!test_bit(DEVICE_SUPPORT_HW_BUTTON, &rt2x00dev->flags))
		return 0;

	rfkill = rfkill_allocate(rt2x00dev->device, RFKILL_TYPE_WLAN);
	if (!rfkill) {
		ERROR(rt2x00dev, "Failed to allocate rfkill handler.\n");
		return -ENOMEM;
	}

	rfkill->name = rt2x00dev->ops->name;
	rfkill->data = rt2x00dev;
	rfkill->toggle_radio = rt2x00lib_toggle_radio;
	rt2x00dev->rfkill = rfkill;

	INIT_DELAYED_WORK(&rt2x00dev->rfkill_work, rt2x00lib_rfkill_poll);

	return 0;
}

void rt2x00lib_free_rfkill(struct rt2x00_dev *rt2x00dev)
{
	if (!test_bit(DEVICE_SUPPORT_HW_BUTTON, &rt2x00dev->flags))
		return;

	rfkill_free(rt2x00dev->rfkill);
}
