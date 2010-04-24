#include <linux/cpu.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/pm.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <asm/reboot.h>
#include <asm/system.h>
#include <ifxmips.h>
#include <ifxmips_cgu.h>

#define SYSTEM_DANUBE			"Danube"
#define SYSTEM_DANUBE_CHIPID1	0x00129083
#define SYSTEM_DANUBE_CHIPID2	0x0012B083

#define SYSTEM_TWINPASS			"Twinpass"
#define SYSTEM_TWINPASS_CHIPID	0x0012D083

static unsigned int chiprev = 0;
unsigned char ifxmips_sys_type[IFXMIPS_SYS_TYPE_LEN];

unsigned int
ifxmips_get_cpu_ver(void)
{
	return (ifxmips_r32(IFXMIPS_MPS_CHIPID) & 0xF0000000) >> 28;
}
EXPORT_SYMBOL(ifxmips_get_cpu_ver);

const char*
get_system_type(void)
{
	return ifxmips_sys_type;
}

static void
ifxmips_machine_restart(char *command)
{
	printk(KERN_NOTICE "System restart\n");
	local_irq_disable();
	ifxmips_w32(ifxmips_r32(IFXMIPS_RCU_RST) | IFXMIPS_RCU_RST_ALL,
		IFXMIPS_RCU_RST);
	for(;;);
}

static void
ifxmips_machine_halt(void)
{
	printk(KERN_NOTICE "System halted.\n");
	local_irq_disable();
	for(;;);
}

static void
ifxmips_machine_power_off(void)
{
	printk(KERN_NOTICE "Please turn off the power now.\n");
	local_irq_disable();
	for(;;);
}

void __init
ifxmips_soc_setup(void)
{
	char *name = SYSTEM_DANUBE;
	ioport_resource.start = IOPORT_RESOURCE_START;
	ioport_resource.end = IOPORT_RESOURCE_END;
	iomem_resource.start = IOMEM_RESOURCE_START;
	iomem_resource.end = IOMEM_RESOURCE_END;

	_machine_restart = ifxmips_machine_restart;
	_machine_halt = ifxmips_machine_halt;
	pm_power_off = ifxmips_machine_power_off;

	chiprev = (ifxmips_r32(IFXMIPS_MPS_CHIPID) & 0x0FFFFFFF);

	switch (chiprev)
	{
	case SYSTEM_DANUBE_CHIPID1:
	case SYSTEM_DANUBE_CHIPID2:
		name = SYSTEM_DANUBE;
		break;

	case SYSTEM_TWINPASS_CHIPID:
		name = SYSTEM_TWINPASS;
		break;

	default:
		printk(KERN_ERR "This is not a danube chiprev : 0x%08X\n", chiprev);
		BUG();
		break;
	}
	snprintf(ifxmips_sys_type, IFXMIPS_SYS_TYPE_LEN - 1, "%s rev1.%d %dMhz",
		name, ifxmips_get_cpu_ver(),
		ifxmips_get_cpu_hz() / 1000000);
	ifxmips_sys_type[IFXMIPS_SYS_TYPE_LEN - 1] = '\0';
}
