#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <linux/errno.h>

#include <asm/irq.h>
#include <asm/div64.h>

#include <ifxmips.h>
#include <ifxmips_cgu.h>

void
cgu_setup_pci_clk(int external_clock)
{
	/* set clock to 33Mhz */
	ifxmips_w32(ifxmips_r32(IFXMIPS_CGU_IFCCR) & ~0xf00000,
		IFXMIPS_CGU_IFCCR);
	ifxmips_w32(ifxmips_r32(IFXMIPS_CGU_IFCCR) | 0x800000,
		IFXMIPS_CGU_IFCCR);
	if (external_clock)
	{
		ifxmips_w32(ifxmips_r32(IFXMIPS_CGU_IFCCR) & ~(1 << 16),
			IFXMIPS_CGU_IFCCR);
		ifxmips_w32((1 << 30), IFXMIPS_CGU_PCICR);
	} else {
		ifxmips_w32(ifxmips_r32(IFXMIPS_CGU_IFCCR) | (1 << 16),
			IFXMIPS_CGU_IFCCR);
		ifxmips_w32((1 << 31) | (1 << 30), IFXMIPS_CGU_PCICR);
	}
}


