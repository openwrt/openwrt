// SPDX-License-Identifier: GPL-2.0-only
/*
 * prom.c
 * Early initialization code for the Realtek RTL838X SoC
 *
 * based on the original BSP by
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 * Copyright (C) 2020 B. Koblitz
 *
 */

#include <asm/fw/fw.h>
#include <asm/mips-cps.h>
#include <asm/prom.h>
#include <asm/smp-ops.h>
#include <linux/smp.h>

#include <mach-rtl83xx.h>

#define RTL_SOC_BASE			((volatile void *) 0xB8000000)

#define RTL83XX_DRAM_CONFIG		0x1004

#define RTL9300_SRAMSAR0		0x4000
#define RTL9300_SRAMSAR1		0x4010
#define RTL9300_SRAMSAR2		0x4020
#define RTL9300_SRAMSAR3		0x4030
#define RTL9300_UMSAR0			0x1300
#define RTL9300_UMSAR1			0x1310
#define RTL9300_UMSAR2			0x1320
#define RTL9300_UMSAR3			0x1330
#define RTL9300_O0DOR2			0x4220
#define RTL9300_O0DMAR2			0x4224

#define RTL931X_DRAM_CONFIG		0x14304c

#define soc_r32(reg)			readl(RTL_SOC_BASE + reg)
#define soc_w32(val, reg)		writel(val, RTL_SOC_BASE + reg)

struct rtl83xx_soc_info soc_info;
const void *fdt;

static char rtl_soc_name[16];
static char rtl_system_type[48];

#ifdef CONFIG_MIPS_MT_SMP

extern const struct plat_smp_ops vsmp_smp_ops;
static struct plat_smp_ops rtl_smp_ops;

static void rtlsmp_init_secondary(void)
{
	/*
	 * Enable all CPU interrupts, as everything is managed by the external controller.
	 * TODO: Standard vsmp_init_secondary() has special treatment for Malta if external
	 * GIC is available. Maybe we need this too.
	 */
	if (mips_gic_present())
		pr_warn("%s: GIC present. Maybe interrupt enabling required.\n", __func__);
	else
		set_c0_status(ST0_IM);
}

static void rtlsmp_finish(void)
{
	/* These devices are low on resources. There might be the chance that CEVT_R4K is
	 * not enabled in kernel build. Nevertheless the timer and interrupt 7 might be
	 * active by default after startup of secondary VPEs. With no registered handler
	 * that leads to continuous unhandeled interrupts. Disable it but keep the counter
	 * running so it can still be used as an entropy source.
	 */
	if (!IS_ENABLED(CONFIG_CEVT_R4K)) {
		write_c0_status(read_c0_status() & ~CAUSEF_IP7);
		write_c0_compare(read_c0_count() - 1);
	}

	local_irq_enable();
}

static int rtlsmp_register(void)
{
	if (!cpu_has_mipsmt)
		return 1;

	rtl_smp_ops = vsmp_smp_ops;
	rtl_smp_ops.init_secondary = rtlsmp_init_secondary;
	rtl_smp_ops.smp_finish = rtlsmp_finish;
	register_smp_ops(&rtl_smp_ops);

	return 0;
}

#else /* !CONFIG_MIPS_MT_SMP */

#define rtlsmp_register() (1)

#endif

void __init device_tree_init(void)
{
	if (!fdt_check_header(&__appended_dtb)) {
		fdt = &__appended_dtb;
		pr_info("Using appended Device Tree.\n");
	}
	initial_boot_params = (void *)fdt;
	unflatten_and_copy_device_tree();

	/* delay cpc & smp probing to allow devicetree access */
	mips_cpc_probe();

	if (!register_cps_smp_ops())
		return;

	if (!rtlsmp_register())
		return;

	register_up_smp_ops();
}

const char *get_system_type(void)
{
	return rtl_system_type;
}

static void __init rtl838x_read_details(u32 model)
{
	u32 chip_info, ext_version, tmp;

	sw_w32(0x3, RTL838X_INT_RW_CTRL);
	sw_w32(0xa << 28, RTL838X_CHIP_INFO);

	chip_info = sw_r32(RTL838X_CHIP_INFO);
	soc_info.cpu = chip_info & 0xffff;

	ext_version = sw_r32(RTL838X_EXT_VERSION);
	tmp = ext_version & 0x1f;

	if (tmp == 2) {
		soc_info.revision = 1;
	} else {
		tmp = (chip_info >> 16) & 0x1f;
		if (soc_info.cpu == 0x0477) {
			soc_info.revision = tmp;
			soc_info.testchip = true;
		} else {
			soc_info.revision = tmp - 1;
		}
	}
}

static void __init rtl839x_read_details(u32 model)
{
	u32 chip_info;

	sw_w32(0xa << 28, RTL839X_CHIP_INFO);

	chip_info = sw_r32(RTL839X_CHIP_INFO);
	soc_info.cpu = chip_info & 0xffff;

	soc_info.revision = (model >> 1) & 0x1f;

	if (!(model & 0x3e))
		soc_info.testchip = true;
}

static void __init rtl93xx_read_details(u32 model)
{
	u32 chip_info;

	sw_w32(0xa << 16, RTL93XX_CHIP_INFO);

	chip_info = sw_r32(RTL93XX_CHIP_INFO);
	soc_info.cpu = chip_info & 0xffff;

	soc_info.revision = model & 0xf;

	if (model & 0x30)
		soc_info.testchip = true;
}

static u32 __init read_model(void)
{
	u32 model, id;

	model = sw_r32(RTL838X_MODEL_NAME_INFO);
	id = model >> 16 & 0xffff;
	if ((id >= 0x8380 && id <= 0x8382) || id == 0x8330 || id == 0x8332) {
		soc_info.id = id;
		soc_info.family = RTL8380_FAMILY_ID;
		soc_info.cpu_port = RTL838X_CPU_PORT;
		rtl838x_read_details(model);
		return model;
	}

	model = sw_r32(RTL839X_MODEL_NAME_INFO);
	id = model >> 16 & 0xffff;
	if ((id >= 0x8391 && id <= 0x8396) || (id >= 0x8351 && id <= 0x8353)) {
		soc_info.id = id;
		soc_info.family = RTL8390_FAMILY_ID;
		soc_info.cpu_port = RTL839X_CPU_PORT;
		rtl839x_read_details(model);
		return model;
	}

	model = sw_r32(RTL93XX_MODEL_NAME_INFO);
	id = model >> 16 & 0xffff;
	if (id >= 0x9301 && id <= 0x9303) {
		soc_info.id = id;
		soc_info.family = RTL9300_FAMILY_ID;
		soc_info.cpu_port = RTL930X_CPU_PORT;
		rtl93xx_read_details(model);
		return model;
	} else if (id >= 0x9311 && id <= 0x9313) {
		soc_info.id = id;
		soc_info.family = RTL9310_FAMILY_ID;
		soc_info.cpu_port = RTL931X_CPU_PORT;
		rtl93xx_read_details(model);
		return model;
	}

	return 0;
}

static void __init parse_model(u32 model)
{
	int val;
	char suffix = 0;

	val = (model >> 11) & 0x1f;
	if (val > 0 && val <= 26)
		suffix = 'A' + (val - 1);

	snprintf(rtl_soc_name, sizeof(rtl_soc_name), "RTL%04X%c",
		 soc_info.id, suffix);

	soc_info.name = rtl_soc_name;
}

static void __init set_system_type(void)
{
	char revision = '?';
	char *es = "";

	if (soc_info.revision >= 0 && soc_info.revision < 26)
		revision = 'A' + soc_info.revision;

	if (soc_info.testchip)
		es = " ES";

	snprintf(rtl_system_type, sizeof(rtl_system_type),
		 "Realtek %s%s rev %c (%04X)",
		 soc_info.name, es, revision, soc_info.cpu);
}

static void get_system_memory(void)
{
	unsigned int dcr, bits;

	if (soc_info.family == RTL9310_FAMILY_ID) {
		dcr = soc_r32(RTL931X_DRAM_CONFIG);
		bits = (dcr >> 12) + ((dcr >> 6) & 0x3f) + (dcr & 0x3f);
	} else {
		dcr = soc_r32(RTL83XX_DRAM_CONFIG);
		bits = ((dcr >> 28) & 0x3) + ((dcr >> 24) & 0x3) +
		       ((dcr >> 20) & 0xf) + ((dcr >> 16) & 0xf) + 20;
	}

	soc_info.memory_size = 1 << bits;
}

static void prepare_highmem(void)
{
	if ((soc_info.family != RTL9300_FAMILY_ID) ||
	    (soc_info.memory_size <= 256 * 1024 * 1024) ||
	    !IS_ENABLED(CONFIG_HIGHMEM))
		return;

	/*
	 * To use highmem on RTL930x, SRAM must be deactivated and the highmem mapping
	 * registers must be setup properly. The hardcoded 0x70000000 might be strange
	 * but at least it conforms somehow to the RTL931x devices.
	 *
	 * - RTL930x: highmem start 0x20000000 + offset 0x70000000 = 0x90000000
	 * - RTL931x: highmem start 0x90000000 + no offset at all  = 0x90000000
	 */

	pr_info("highmem kernel on RTL930x with > 256 MB RAM, adapt SoC memory mapping\n");

	soc_w32(0, RTL9300_UMSAR0);
	soc_w32(0, RTL9300_UMSAR1);
	soc_w32(0, RTL9300_UMSAR2);
	soc_w32(0, RTL9300_UMSAR3);
	soc_w32(0, RTL9300_SRAMSAR0);
	soc_w32(0, RTL9300_SRAMSAR1);
	soc_w32(0, RTL9300_SRAMSAR2);
	soc_w32(0, RTL9300_SRAMSAR3);
	__sync();

	soc_w32(0x70000000, RTL9300_O0DOR2);
	soc_w32(0x7fffffff, RTL9300_O0DMAR2);
	__sync();
}

void __init prom_init(void)
{
	u32 model = read_model();

	parse_model(model);
	set_system_type();
	get_system_memory();

	pr_info("%s SoC with %d MB\n", get_system_type(), soc_info.memory_size >> 20);

	prepare_highmem();

	/*
	 * fw_arg2 is be the pointer to the environment. Some devices (e.g. HP JG924A) hand
	 * over other than expected kernel boot arguments. Something like 0xfffdffff looks
	 * suspicous. Do extra cleanup for fw_init_cmdline() to avoid a hang during boot.
	 */
	if (fw_arg2 >= CKSEG2)
		fw_arg2 = 0;

	fw_init_cmdline();
}
