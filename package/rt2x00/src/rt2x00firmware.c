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
	Abstract: rt2x00 firmware loading specific routines.
	Supported chipsets: rt2561, rt2561s, rt2661, rt2571W & rt2671.
 */

/*
 * Set enviroment defines for rt2x00.h
 */
#define DRV_NAME "rt2x00lib"

#include <linux/crc-itu-t.h>
#include <linux/firmware.h>

#include "rt2x00.h"
#include "rt2x00firmware.h"

static void rt2x00lib_load_firmware_continued(const struct firmware *fw,
	void *context)
{
	struct rt2x00_dev *rt2x00dev = context;
	u16 crc;
	u16 tmp;

	if (!fw || !fw->size || !fw->data) {
		ERROR(rt2x00dev, "Failed to read Firmware.\n");
		goto exit_failed;
	}

	/*
	 * Validate the firmware using 16 bit CRC.
	 * The last 2 bytes of the firmware are the CRC
	 * so substract those 2 bytes from the CRC checksum,
	 * and set those 2 bytes to 0 when calculating CRC.
	 */
	tmp = 0;
	crc = crc_itu_t(0, fw->data, fw->size - 2);
	crc = crc_itu_t(crc, (u8*)&tmp, 2);

	if (crc != (fw->data[fw->size - 2] << 8 | fw->data[fw->size - 1])) {
		ERROR(rt2x00dev, "Firmware CRC error.\n");
		goto exit_failed;
	}

	/*
	 * Send firmware to the device.
	 */
	if (rt2x00dev->ops->lib->load_firmware(rt2x00dev, fw->data, fw->size))
		goto exit_failed;

	INFO(rt2x00dev, "Firmware detected - version: %d.%d.\n",
		fw->data[fw->size - 4], fw->data[fw->size - 3]);

	__set_bit(FIRMWARE_LOADED, &rt2x00dev->flags);

	return;

exit_failed:
	rt2x00debug_deregister(rt2x00dev);

	__set_bit(FIRMWARE_FAILED, &rt2x00dev->flags);
}

int rt2x00lib_load_firmware(struct rt2x00_dev *rt2x00dev)
{
	char *fw_name;
	int status = -EINVAL;

	/*
	 * Read correct firmware from harddisk.
	 */
	fw_name = rt2x00dev->ops->lib->get_fw_name(rt2x00dev);
	BUG_ON(fw_name == NULL);

	INFO(rt2x00dev, "Loading firmware file '%s'.\n", fw_name);

	status = request_firmware_nowait(THIS_MODULE, FW_ACTION_HOTPLUG,
		fw_name, rt2x00dev->device, rt2x00dev,
		&rt2x00lib_load_firmware_continued);

	if (status)
		ERROR(rt2x00dev, "Failed to request Firmware.\n");

	return status;
}

int rt2x00lib_load_firmware_wait(struct rt2x00_dev *rt2x00dev)
{
	unsigned int i;

	if (!test_bit(FIRMWARE_REQUIRED, &rt2x00dev->flags))
		return 0;

	for (i = 0; i < 150; i++) {
		if (test_bit(FIRMWARE_FAILED, &rt2x00dev->flags))
			return -EIO;
		if (test_bit(FIRMWARE_LOADED, &rt2x00dev->flags))
			return 0;
		msleep(20);
	}

	ERROR(rt2x00dev, "Firmware loading timed out.\n");
	return -ETIMEDOUT;
}
