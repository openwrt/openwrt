#include <linux/init.h>
#include <linux/kernel.h>

#include <asm/time.h>
#include <adm8668.h>

void __init plat_time_init(void)
{
	int adj = (ADM8668_CONFIG_REG(ADM8668_CR3) >> 11) & 0xf;

	/* adjustable clock selection
	   CR3 bit 14~11, 0000 -> 175MHz, 0001 -> 180MHz, etc... */

	mips_hpt_frequency = (SYS_CLOCK + adj * 5000000) / 2;
	printk("ADM8668 CPU clock: %d MHz\n", 2*mips_hpt_frequency / 1000000);
}

