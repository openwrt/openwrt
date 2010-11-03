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

static int ifxmips_brn = 0;

int __init
ifxmix_detect_brn_block(unsigned int offset)
{
	unsigned char temp[8];
	memcpy_fromio(temp, (void *)KSEG1ADDR(IFXMIPS_FLASH_START + offset), 8);
	if (!memcmp(temp, "BRN-BOOT", 8))
		ifxmips_brn = 1;
	return !ifxmips_brn;
}

int __init
ifxmips_find_brn_mac(unsigned int offset, unsigned char *ifxmips_ethaddr)
{
	if(!ifxmips_brn)
		return 1;
	memcpy_fromio(ifxmips_ethaddr,
		(void *)KSEG1ADDR(IFXMIPS_FLASH_START +	offset), 6);
	return is_valid_ether_addr(ifxmips_ethaddr);
}

/* used by madwifi to know if eeprom is located in flash */
int
ifxmips_has_brn_block(void)
{
	return ifxmips_brn;
}
EXPORT_SYMBOL(ifxmips_has_brn_block);
