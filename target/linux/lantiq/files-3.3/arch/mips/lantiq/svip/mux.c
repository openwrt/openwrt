/************************************************************************
 *
 * Copyright (c) 2007
 * Infineon Technologies AG
 * St. Martin Strasse 53; 81669 Muenchen; Germany
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 ************************************************************************/

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <asm/addrspace.h>
#include <linux/platform_device.h>

#include <lantiq_soc.h>
#include <svip_mux.h>
#include <sys1_reg.h>
#include <sys2_reg.h>
#include <svip_pms.h>

#define DRV_NAME "ltq_mux"

static void ltq_mux_port_init(const int port,
			      const struct ltq_mux_pin *pins,
			      const int pin_max)
{
	unsigned int i;

	for (i = 0; i < pin_max; i++)
		ltq_gpio_configure(port,
				   i,
				   pins[i].dirin,
				   pins[i].puen,
				   pins[i].altsel0,
				   pins[i].altsel1);
}

static int ltq_mux_probe(struct platform_device *pdev)
{
	struct ltq_mux_settings *mux_settings = dev_get_platdata(&pdev->dev);

	if (mux_settings->mux_p0)
		ltq_mux_port_init(0,
				  mux_settings->mux_p0,
				  LTQ_MUX_P0_PINS);

	if (mux_settings->mux_p1)
		ltq_mux_port_init(1,
				  mux_settings->mux_p1,
				  LTQ_MUX_P1_PINS);

	if (mux_settings->mux_p2)
		ltq_mux_port_init(2,
				  mux_settings->mux_p2,
				  LTQ_MUX_P2_PINS);

	if (mux_settings->mux_p3)
		ltq_mux_port_init(3,
				  mux_settings->mux_p3,
				  LTQ_MUX_P3_PINS);

	if (mux_settings->mux_p4)
		ltq_mux_port_init(4,
				  mux_settings->mux_p4,
				  LTQ_MUX_P4_PINS);

	return 0;
}

int ltq_mux_read_procmem(char *buf, char **start, off_t offset,
			 int count, int *eof, void *data)
{
	int len = 0;
	int t = 0, i = 0;
	u32 port_clk[5] = {
		SYS1_CLKENR_PORT0,
		SYS1_CLKENR_PORT1,
		SYS1_CLKENR_PORT2,
		SYS1_CLKENR_PORT3,
		SYS2_CLKENR_PORT4,
	};

#define PROC_PRINT(fmt, args...) \
	do { \
		int c_len = 0; \
		c_len = snprintf(buf + len, count - len, fmt, ## args); \
		if (c_len <= 0) \
			goto out; \
		if (c_len >= (count - len)) { \
			len += (count - len); \
			goto out; \
		} \
		len += c_len; \
		if (offset > 0) { \
			if (len > offset) { \
				len -= offset; \
				memmove(buf, buf + offset, len); \
				offset = 0; \
			} else { \
				offset -= len; \
				len = 0; \
			} \
		} \
	} while (0)

	PROC_PRINT("\nVINETIC-SVIP Multiplex Settings\n");
	PROC_PRINT("              3         2         1         0\n");
	PROC_PRINT("             10987654321098765432109876543210\n");
	PROC_PRINT("             --------------------------------\n");

	for (i = 0; i < ARRAY_SIZE(port_clk); i++) {
		if (i < 4) {
			if (!svip_sys1_clk_is_enabled(port_clk[i]))
				continue;
		} else {
			if (!svip_sys2_clk_is_enabled(port_clk[i]))
				continue;
		}

		PROC_PRINT("P%d.%-10s", i, "DIR:");

		for (t = 31; t != -1; t--)
			PROC_PRINT("%d", ltq_port_get_dir(i, t) == 1 ? 1 : 0);
		PROC_PRINT("\n");

		PROC_PRINT("P%d.%-10s", i, "PUEN:");
		for (t = 31; t != -1; t--)
			PROC_PRINT("%d", ltq_port_get_puden(i, t) == 1 ? 1 : 0);
		PROC_PRINT("\n");

		PROC_PRINT("P%d.%-10s", i, "ALTSEL0:");
		for (t = 31; t != -1; t--)
			PROC_PRINT("%d",
				   ltq_port_get_altsel0(i, t) == 1 ? 1 : 0);
		PROC_PRINT("\n");

		PROC_PRINT("P%d.%-10s", i, "ALTSEL1:");
		for (t = 31; t != -1; t--)
			PROC_PRINT("%d",
				   ltq_port_get_altsel1(i, t) == 1 ? 1 : 0);
		PROC_PRINT("\n\n");
	}

out:
	if (len < 0) {
		len = 0;
		*eof = 1;
	} else if (len < count) {
		*eof = 1;
	} else {
		len = count;
	}

	*start = buf;

	return len;
}

static struct platform_driver ltq_mux_driver = {
	.probe = ltq_mux_probe,
	.driver = {
		.name = DRV_NAME,
		.owner = THIS_MODULE,
	},
};

int __init ltq_mux_init(void)
{
	int ret = platform_driver_register(&ltq_mux_driver);
	if (ret) {
		printk(KERN_INFO DRV_NAME
		       ": Error registering platform driver!");
		return ret;
	}

	return create_proc_read_entry("driver/ltq_mux", 0, NULL,
				      ltq_mux_read_procmem, NULL) == NULL;
}

module_init(ltq_mux_init);
