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

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/of_fdt.h>
#include <linux/libfdt.h>
#include <asm/bootinfo.h>
#include <asm/addrspace.h>
#include <asm/page.h>
#include <asm/cpu.h>

#include <mach-rtl838x.h>

extern char arcs_cmdline[];
const void *fdt;
extern const char __appended_dtb;

void prom_console_init(void)
{
	/* UART 16550A is initialized by the bootloader */
}

#ifdef CONFIG_EARLY_PRINTK
#define rtl838x_r8(reg)		__raw_readb(reg)
#define rtl838x_w8(val, reg)	__raw_writeb(val, reg)

void unregister_prom_console(void)
{

}

void disable_early_printk(void)
{

}

void prom_putchar(char c)
{
	unsigned int retry = 0;

	do {
		if (retry++ >= 30000) {
			/* Reset Tx FIFO */
			rtl838x_w8(TXRST | CHAR_TRIGGER_14, UART0_FCR);
			return;
		}
	} while ((rtl838x_r8(UART0_LSR) & LSR_THRE) == TxCHAR_AVAIL);

	/* Send Character */
	rtl838x_w8(c, UART0_THR);
}

char prom_getchar(void)
{
	return '\0';
}
#endif

const char *get_system_type(void)
{
	return soc_info.name;
}


void __init prom_free_prom_memory(void)
{

}

void __init device_tree_init(void)
{
	pr_info("%s called\r\n", __func__);
	if (!fdt_check_header(&__appended_dtb)) {
		fdt = &__appended_dtb;
		pr_info("Using appended Device Tree.\n");
	}
	initial_boot_params = (void *)fdt;
	unflatten_and_copy_device_tree();
}

static void __init prom_init_cmdline(void)
{
	int argc = fw_arg0;
	char **argv = (char **) KSEG1ADDR(fw_arg1);
	int i;

	arcs_cmdline[0] = '\0';

	for (i = 0; i < argc; i++) {
		char *p = (char *) KSEG1ADDR(argv[i]);

		if (CPHYSADDR(p) && *p) {
			strlcat(arcs_cmdline, p, sizeof(arcs_cmdline));
			strlcat(arcs_cmdline, " ", sizeof(arcs_cmdline));
		}
	}
	pr_info("Kernel command line: %s\n", arcs_cmdline);
}

/* Do basic initialization */
void __init prom_init(void)
{
	uint32_t model;

	pr_info("%s called\n", __func__);
	soc_info.sw_base = RTL838X_SW_BASE;

	model = sw_r32(RTL838X_MODEL_NAME_INFO);
	pr_info("RTL838X model is %x\n", model);
	model = model >> 16 & 0xFFFF;

	if ((model != 0x8328) && (model != 0x8330) && (model != 0x8332)
	    && (model != 0x8380) && (model != 0x8382)) {
		model = sw_r32(RTL839X_MODEL_NAME_INFO);
		pr_info("RTL839X model is %x\n", model);
		model = model >> 16 & 0xFFFF;
	}

	soc_info.id = model;

	switch (model) {
	case 0x8328:
		soc_info.name = "RTL8328";
		soc_info.family = RTL8328_FAMILY_ID;
		break;
	case 0x8332:
		soc_info.name = "RTL8332";
		soc_info.family = RTL8380_FAMILY_ID;
		break;
	case 0x8380:
		soc_info.name = "RTL8380";
		soc_info.family = RTL8380_FAMILY_ID;
		break;
	case 0x8382:
		soc_info.name = "RTL8382";
		soc_info.family = RTL8380_FAMILY_ID;
		break;
	case 0x8390:
		soc_info.name = "RTL8390";
		soc_info.family = RTL8390_FAMILY_ID;
		break;
	case 0x8391:
		soc_info.name = "RTL8391";
		soc_info.family = RTL8390_FAMILY_ID;
		break;
	case 0x8392:
		soc_info.name = "RTL8392";
		soc_info.family = RTL8390_FAMILY_ID;
		break;
	case 0x8393:
		soc_info.name = "RTL8393";
		soc_info.family = RTL8390_FAMILY_ID;
		break;
	default:
		soc_info.name = "DEFAULT";
		soc_info.family = 0;
	}
	pr_info("SoC Type: %s\n", get_system_type());
	prom_init_cmdline();
}
