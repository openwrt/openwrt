#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/mtd/physmap.h>
#include <linux/kernel.h>
#include <linux/reboot.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/etherdevice.h>
#include <linux/reboot.h>
#include <linux/time.h>
#include <linux/io.h>
#include <linux/gpio.h>

#include <ifxmips.h>
#include <ifxmips_prom.h>

#include "arcaydian.h"

static int ifxmips_brn = 1;

int __init
ifxmips_find_brn_mac(unsigned char *ifxmips_ethaddr)
{
	unsigned char temp[8];
	memcpy_fromio(temp,
		(void *)KSEG1ADDR(IFXMIPS_FLASH_START + 0x800000 - 0x10000), 8);
	if (!memcmp(temp, "BRN-BOOT", 8))
	{
		memcpy_fromio(ifxmips_ethaddr,
			(void *)KSEG1ADDR(IFXMIPS_FLASH_START +
			0x800000 - 0x10000 + 0x16), 6);
		if (is_valid_ether_addr(ifxmips_ethaddr))
			return 1;
	} else {
		ifxmips_brn = 0;
	}
	return 0;
}

int
ifxmips_has_brn_block(void)
{
	return ifxmips_brn;
}
EXPORT_SYMBOL(ifxmips_has_brn_block);
