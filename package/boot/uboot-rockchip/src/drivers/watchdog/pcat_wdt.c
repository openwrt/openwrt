// SPDX-License-Identifier: GPL-2.0+

#include <dm.h>
#include <dm/device_compat.h>
#include <wdt.h>
#include <asm/gpio.h>
#include <linux/delay.h>
#include <serial.h>
#include <dm/device-internal.h>

struct pcat_wdt_priv {
	struct udevice *serial_dev;
	struct dm_serial_ops *serial_ops;
	uint16_t packet_count;
};

static uint16_t pcat_pmu_serial_compute_crc16(const uint8_t *data,
    size_t len)
{
	uint16_t crc = 0xFFFF;
	size_t i;
	unsigned int j;

	for(i=0;i<len;i++) {
		crc ^= data[i];
		for(j=0;j<8;j++) {
			if(crc & 1) {
				crc = (crc >> 1) ^ 0xA001;
			} else {
				crc >>= 1;
			}
		}
	}

	return crc;
}

static int pcat_wdt_reset(struct udevice *dev)
{
	struct pcat_wdt_priv *priv = dev_get_priv(dev);
	uint8_t packet[13] = "\xA5\x01\x81\x00\x00\x03\x00\x01\x00"
		"\x00\x00\x00\x5A";
	uint16_t crc;
	int err = 0;
	unsigned int i;

	packet[3] = priv->packet_count & 0xFF;
	packet[4] = (priv->packet_count >> 8) & 0xFF;
	priv->packet_count++;

	crc = pcat_pmu_serial_compute_crc16(packet+1, 9);
	packet[10] = crc & 0xFF;
	packet[11] = (crc >> 8)& 0xFF;

	i = 0;
	while(i < 13) {
		err = priv->serial_ops->putc(
			priv->serial_dev, ((const char *)packet)[i]);
		if(!err) {
			i++;
		} else if(err==-EAGAIN) {
			;
		} else {
			pr_err("%s: unable to send watchdog setup "
				"request: %d\n", __func__, err);
			break;
		}
	}

	return err;
}

static int pcat_wdt_setup(struct pcat_wdt_priv *priv, u64 timeout) {
	uint8_t packet[16] = "\xA5\x01\x81\x00\x00\x06\x00\x13\x00"
		"\x3C\x3C\x00\x00\x00\x00\x5A";
	uint16_t crc;
	int err = 0;
	unsigned int i;

	if(timeout > 255) {
		pr_warn("%s: timeout cannot be more than 255s\n",
			__func__);
		timeout = 255;
	}

	packet[3] = priv->packet_count & 0xFF;
	packet[4] = (priv->packet_count >> 8) & 0xFF;
	priv->packet_count++;
	packet[11] = timeout & 0xFF;

	crc = pcat_pmu_serial_compute_crc16(packet+1, 12);
	packet[13] = crc & 0xFF;
	packet[14] = (crc >> 8)& 0xFF;

	i = 0;
	while(i < 16) {
		err = priv->serial_ops->putc(
			priv->serial_dev, ((const char *)packet)[i]);
		if(!err) {
			i++;
		} else if(err==-EAGAIN) {
			;
		} else {
			pr_err("%s: unable to send watchdog setup "
				"request: %d\n", __func__, err);
			break;
		}
	}

	return err;
}

static int pcat_wdt_start(struct udevice *dev, u64 timeout, ulong flags)
{
	struct pcat_wdt_priv *priv = dev_get_priv(dev);

	pcat_wdt_setup(priv, timeout);

	return 0;
}

static int pcat_wdt_stop(struct udevice *dev)
{
	struct pcat_wdt_priv *priv = dev_get_priv(dev);

	pcat_wdt_setup(priv, 0);

	return 0;
}

static int dm_probe(struct udevice *dev)
{
	struct pcat_wdt_priv *priv = dev_get_priv(dev);
	struct udevice *serial_dev = NULL;
	struct dm_serial_ops *ops;
	int ret;

	ret = uclass_get_device_by_phandle(UCLASS_SERIAL, dev, "port",
		&serial_dev);
	if(ret) {
		pr_err("%s: unable to find serial port device: %d\n",
			__func__, ret);
		return ret;
	}

	ret = device_probe(serial_dev);
	if(ret) {
		pr_err("%s: unable to probe serial port device: %d\n",
			__func__, ret);
		return ret;
	}

	ops = serial_get_ops(serial_dev);
	if(!ops) {
		printf("Cannot get ops for PMU serial port!\n");
		return -EINVAL;
	}

	if(ops->setconfig) {
		ops->setconfig(serial_dev, SERIAL_DEFAULT_CONFIG);
	}
	if(ops->setbrg) {
		ops->setbrg(serial_dev, 115200);
	}

	priv->serial_dev = serial_dev;
	priv->serial_ops = ops;
	priv->packet_count = 0;

	pcat_wdt_stop(dev);

	return 0;
}

static const struct wdt_ops pcat_wdt_ops = {
	.start = pcat_wdt_start,
	.reset = pcat_wdt_reset,
	.stop = pcat_wdt_stop,
};

static const struct udevice_id pcat_wdt_ids[] = {
	{ .compatible = "linux,wdt-pcat" },
	{}
};

U_BOOT_DRIVER(wdt_pcat) = {
	.name = "wdt_pcat",
	.id = UCLASS_WDT,
	.of_match = pcat_wdt_ids,
	.ops = &pcat_wdt_ops,
	.probe	= dm_probe,
	.priv_auto = sizeof(struct pcat_wdt_priv),
};
