#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/gpio_buttons.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include "../machtypes.h"

#include <sys1_reg.h>
#include <sys2_reg.h>
#include <svip_pms.h>

#include "devices.h"

static const struct ltq_mux_pin mux_p0[LTQ_MUX_P0_PINS] = {
	LTQ_MUX_P0_0_SSC0_MTSR,
	LTQ_MUX_P0_1_SSC0_MRST,
	LTQ_MUX_P0_2_SSC0_SCLK,
	LTQ_MUX_P0_3_SSC1_MTSR,
	LTQ_MUX_P0_4_SSC1_MRST,
	LTQ_MUX_P0_5_SSC1_SCLK,
	LTQ_MUX_P0_6_SSC0_CS0,
	LTQ_MUX_P0_7_SSC0_CS1,
	LTQ_MUX_P0_8_SSC0_CS2,
	LTQ_MUX_P0_9,
	LTQ_MUX_P0_10,
	LTQ_MUX_P0_11_EXINT4,
	LTQ_MUX_P0_12,
	LTQ_MUX_P0_13,
	LTQ_MUX_P0_14_ASC0_TXD,
	LTQ_MUX_P0_15_ASC0_RXD,
	LTQ_MUX_P0_16_EXINT9,
	LTQ_MUX_P0_17_EXINT10,
	LTQ_MUX_P0_18_EJ_BRKIN,
	LTQ_MUX_P0_19_EXINT16
};

static void __init easy33016_init(void)
{
	svip_sys1_clk_enable(SYS1_CLKENR_L2C |
			     SYS1_CLKENR_DDR2 |
			     SYS1_CLKENR_SMI2 |
			     SYS1_CLKENR_SMI1 |
			     SYS1_CLKENR_SMI0 |
			     SYS1_CLKENR_FMI0 |
			     SYS1_CLKENR_DMA |
			     SYS1_CLKENR_SSC0 |
			     SYS1_CLKENR_SSC1 |
			     SYS1_CLKENR_EBU);

	svip_sys2_clk_enable(SYS2_CLKENR_HWSYNC |
			     SYS2_CLKENR_MBS |
			     SYS2_CLKENR_SWINT);

	svip_register_mux(mux_p0, NULL, NULL, NULL, NULL);
	svip_register_asc(0);
	svip_register_eth();
	svip_register_virtual_eth();
	ltq_register_wdt();
	svip_register_gpio();
	svip_register_spi();
	svip_register_nand();
}

MIPS_MACHINE(LANTIQ_MACH_EASY33016,
	     "EASY33016",
	     "EASY33016",
	     easy33016_init);
