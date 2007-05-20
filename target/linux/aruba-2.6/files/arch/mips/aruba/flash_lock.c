#include <linux/module.h>
#include <linux/types.h>
#include <asm/bootinfo.h>

#define AP70_PROT_ADDR 0xb8010008
#define AP70_PROT_DATA 0x8
#define AP60_PROT_ADDR 0xB8400000
#define AP60_PROT_DATA 0x04000000

void unlock_ap60_70_flash(void)
{
	volatile __u32 val;
	switch (mips_machtype) {
		case MACH_ARUBA_AP70:
			val = *(volatile __u32 *)AP70_PROT_ADDR;
			val &= ~(AP70_PROT_DATA);
			*(volatile __u32 *)AP70_PROT_ADDR = val;
			break;
		case MACH_ARUBA_AP65:
		case MACH_ARUBA_AP60:
		default:
			val = *(volatile __u32 *)AP60_PROT_ADDR;
			val &= ~(AP60_PROT_DATA);
			*(volatile __u32 *)AP60_PROT_ADDR = val;
			break;
	}
}
