#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>

#include <ifxmips.h>

void
ifxmips_pmu_enable(unsigned int module)
{
	int err = 1000000;

	ifxmips_w32(ifxmips_r32(IFXMIPS_PMU_PWDCR) & ~module, IFXMIPS_PMU_PWDCR);
	while (--err && (ifxmips_r32(IFXMIPS_PMU_PWDSR) & module));

	if (!err)
		panic("activating PMU module failed!");
}
EXPORT_SYMBOL(ifxmips_pmu_enable);

void
ifxmips_pmu_disable(unsigned int module)
{
	ifxmips_w32(ifxmips_r32(IFXMIPS_PMU_PWDCR) | module, IFXMIPS_PMU_PWDCR);
}
EXPORT_SYMBOL(ifxmips_pmu_disable);
