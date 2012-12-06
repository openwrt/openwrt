#include <linux/init.h>
#include <linux/reboot.h>

#include <asm/reboot.h>
#include <adm8668.h>

static void adm8668_restart(char *cmd)
{
	int i;

	/* stop eth0 and eth1 */
	ADM8668_LAN_REG(NETCSR6) = (1 << 13) | (1 << 1);
	ADM8668_LAN_REG(NETCSR7) = 0;
	ADM8668_WAN_REG(NETCSR6) = (1 << 13) | (1 << 1);
	ADM8668_WAN_REG(NETCSR7) = 0;

	/* reset PHY */
	ADM8668_WAN_REG(NETCSR37) = 0x20;
	for (i = 0; i < 10000; i++)
		;
	ADM8668_WAN_REG(NETCSR37) = 0;
	for (i = 0; i < 10000; i++)
		;

	/* the real deal */
	for (i = 0; i < 1000; i++)
		;
	ADM8668_CONFIG_REG(ADM8668_CR1) = 1;
}

void __init plat_mem_setup(void)
{
	_machine_restart = adm8668_restart;
}

const char *get_system_type(void)
{
        unsigned long chipid = ADM8668_CONFIG_REG(ADM8668_CR0);
        int adj = (ADM8668_CONFIG_REG(ADM8668_CR3) >> 11) & 0xf;
        int product, revision, mhz;
	static char ret[32];

        product = chipid >> 16;
        revision = chipid & 0xffff;
	mhz = (SYS_CLOCK/1000000) + (adj * 5);

	/* i getting fancy :\ */
	snprintf(ret, sizeof(ret), "ADM%xr%x %dMHz", product, revision, mhz);

	return ret;
}

