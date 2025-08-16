// SPDX-License-Identifier: GPL-2.0-only
/*
 * prom.c
 * Early intialization code for the Realtek RTL838X SoC
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

struct rtl83xx_soc_info soc_info;
const void *fdt;

static char soc_name[16];
static char rtl83xx_system_type[48];

#ifdef CONFIG_MIPS_MT_SMP

extern const struct plat_smp_ops vsmp_smp_ops;
static struct plat_smp_ops rtlops;

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

	rtlops = vsmp_smp_ops;
	rtlops.init_secondary = rtlsmp_init_secondary;
	rtlops.smp_finish = rtlsmp_finish;
	register_smp_ops(&rtlops);

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
	return rtl83xx_system_type;
}

static void __init rtl838x_read_details(uint32_t model)
{
	uint32_t chip_info, ext_version, tmp;

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

static void __init rtl839x_read_details(uint32_t model)
{
	uint32_t chip_info;

	sw_w32(0xa << 28, RTL839X_CHIP_INFO);

	chip_info = sw_r32(RTL839X_CHIP_INFO);
	soc_info.cpu = chip_info & 0xffff;

	soc_info.revision = (model >> 1) & 0x1f;

	if (!(model & 0x3e))
		soc_info.testchip = true;
}

static void __init rtl93xx_read_details(uint32_t model)
{
	uint32_t chip_info;

	sw_w32(0xa << 16, RTL93XX_CHIP_INFO);

	chip_info = sw_r32(RTL93XX_CHIP_INFO);
	soc_info.cpu = chip_info & 0xffff;

	soc_info.revision = model & 0xf;

	if (model & 0x30)
		soc_info.testchip = true;
}

static uint32_t __init read_model(void)
{
	uint32_t model, id;

	model = sw_r32(RTL838X_MODEL_NAME_INFO);
	id = model >> 16 & 0xffff;
	if ((id >= 0x8380 && id <= 0x8382) || id == 0x8330 || id == 0x8332) {
		soc_info.id = id;
		soc_info.family = RTL8380_FAMILY_ID;
		rtl838x_read_details(model);
		return model;
	}

	model = sw_r32(RTL839X_MODEL_NAME_INFO);
	id = model >> 16 & 0xffff;
	if ((id >= 0x8391 && id <= 0x8396) || (id >= 0x8351 && id <= 0x8353)) {
		soc_info.id = id;
		soc_info.family = RTL8390_FAMILY_ID;
		rtl839x_read_details(model);
		return model;
	}

	model = sw_r32(RTL93XX_MODEL_NAME_INFO);
	id = model >> 16 & 0xffff;
	if (id >= 0x9301 && id <= 0x9303) {
		soc_info.id = id;
		soc_info.family = RTL9300_FAMILY_ID;
		rtl93xx_read_details(model);
		return model;
	} else if (id >= 0x9311 && id <= 0x9313) {
		soc_info.id = id;
		soc_info.family = RTL9310_FAMILY_ID;
		rtl93xx_read_details(model);
		return model;
	}

	return 0;
}

static void __init parse_model(uint32_t model)
{
	int val;
	char suffix = 0;

	val = (model >> 11) & 0x1f;
	if (val > 0 && val <= 26)
		suffix = 'A' + (val - 1);

	snprintf(soc_name, sizeof(soc_name), "RTL%04X%c",
		 soc_info.id, suffix);

	soc_info.name = soc_name;
}

static void __init rtl83xx_set_system_type(void) {
	char revision = '?';
	char *es = "";

	if (soc_info.revision >= 0 && soc_info.revision < 26)
		revision = 'A' + soc_info.revision;

	if (soc_info.testchip)
		es = " ES";

	snprintf(rtl83xx_system_type, sizeof(rtl83xx_system_type),
		 "Realtek %s%s rev %c (%04X)",
		 soc_info.name, es, revision, soc_info.cpu);
}

void __init prom_init(void)
{
	uint32_t model = read_model();
	parse_model(model);
	rtl83xx_set_system_type();

	pr_info("SoC Type: %s\n", get_system_type());

	/*
	 * fw_arg2 is be the pointer to the environment. Some devices (e.g. HP JG924A) hand
	 * over other than expected kernel boot arguments. Something like 0xfffdffff looks
	 * suspicous. Do extra cleanup for fw_init_cmdline() to avoid a hang during boot.
	 */
	if (fw_arg2 >= CKSEG2)
		fw_arg2 = 0;

	fw_init_cmdline();
}
