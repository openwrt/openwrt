#include <linux/init.h>
#include <linux/cpu.h>

#include <asm/time.h>
#include <asm/traps.h>
#include <asm/irq.h>
#include <asm/bootinfo.h>

#include <ifxmips.h>
#include <ifxmips_irq.h>
#include <ifxmips_pmu.h>
#include <ifxmips_cgu.h>
#include <ifxmips_prom.h>

#include <machine.h>

DEFINE_SPINLOCK(ebu_lock);
EXPORT_SYMBOL_GPL(ebu_lock);

static unsigned int r4k_offset;
static unsigned int r4k_cur;

static unsigned int ifxmips_ram_clocks[] = {CLOCK_167M, CLOCK_133M, CLOCK_111M, CLOCK_83M };
#define DDR_HZ ifxmips_ram_clocks[ifxmips_r32(IFXMIPS_CGU_SYS) & 0x3]

extern void __init ifxmips_soc_setup(void);

static inline u32
ifxmips_get_counter_resolution(void)
{
	u32 res;
	__asm__ __volatile__(
		".set   push\n"
		".set   mips32r2\n"
		".set   noreorder\n"
		"rdhwr  %0, $3\n"
		"ehb\n"
		".set pop\n"
		: "=&r" (res)
		: /* no input */
		: "memory");
	instruction_hazard();
	return res;
}

void __init
plat_time_init(void)
{
	mips_hpt_frequency = ifxmips_get_cpu_hz() / ifxmips_get_counter_resolution();
	r4k_cur = (read_c0_count() + r4k_offset);
	write_c0_compare(r4k_cur);

	ifxmips_pmu_enable(IFXMIPS_PMU_PWDCR_GPT | IFXMIPS_PMU_PWDCR_FPI);
	ifxmips_w32(0x100, IFXMIPS_GPTU_GPT_CLC); /* set clock divider to 1 */
}

void __init
plat_mem_setup(void)
{
	u32 status;

	/* make sure to have no "reverse endian" for user mode! */
	status = read_c0_status();
	status &= (~(1<<25));
	write_c0_status(status);

	/* call the chip specific init code */
	ifxmips_soc_setup();
}


unsigned int
ifxmips_get_cpu_hz(void)
{
	switch (ifxmips_r32(IFXMIPS_CGU_SYS) & 0xc)
	{
	case 0:
		return CLOCK_333M;
	case 4:
		return DDR_HZ;
	case 8:
		return DDR_HZ << 1;
	default:
		return DDR_HZ >> 1;
	}
}
EXPORT_SYMBOL(ifxmips_get_cpu_hz);

static int __init
ifxmips_machine_setup(void)
{
	mips_machine_setup();
	return 0;
}

arch_initcall(ifxmips_machine_setup);

static void __init
ifxmips_generic_init(void)
{
}

MIPS_MACHINE(IFXMIPS_MACH_GENERIC, "Generic", "Generic Infineon board",
	ifxmips_generic_init);

__setup("board=", mips_machtype_setup);

