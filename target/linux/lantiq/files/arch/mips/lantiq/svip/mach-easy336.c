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

static struct mtd_partition easy336_sflash_partitions[] = {
	{
		.name		= "SPI flash",
		.size		= MTDPART_SIZ_FULL,
		.offset		= 0,
	},
};

static struct flash_platform_data easy336_sflash_data = {
	.name = "m25p32",
	.parts = (void *)&easy336_sflash_partitions,
	.nr_parts = ARRAY_SIZE(easy336_sflash_partitions),
	.type = "m25p32",
};

static struct spi_board_info bdinfo[] __initdata = {
	{
		.modalias = "m25p80",
		.platform_data = &easy336_sflash_data,
		.mode = SPI_MODE_0,
		.irq = -1,
		.max_speed_hz = 25000000,
		.bus_num = 0,
		.chip_select = 0,
	}
};

static struct mtd_partition easy336_partitions[] = {
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x40000,
	},
	{
		.name	= "uboot_env",
		.offset	= 0x40000,
		.size	= 0x20000,
	},
	{
		.name	= "linux",
		.offset	= 0x60000,
		.size	= 0x1a0000,
	},
	{
		.name	= "rootfs",
		.offset	= 0x200000,
		.size	= 0x500000,
	},
};

static struct physmap_flash_data easy336_flash_data = {
	.nr_parts       = ARRAY_SIZE(easy336_partitions),
	.parts          = easy336_partitions,
};

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
	LTQ_MUX_P0_9_SSC0_CS3,
	LTQ_MUX_P0_10_SSC0_CS4,
	LTQ_MUX_P0_11_SSC0_CS5,
	LTQ_MUX_P0_12_EXINT5,
	LTQ_MUX_P0_13_EXINT6,
	LTQ_MUX_P0_14_ASC0_TXD,
	LTQ_MUX_P0_15_ASC0_RXD,
	LTQ_MUX_P0_16_EXINT9,
	LTQ_MUX_P0_17_EXINT10,
	LTQ_MUX_P0_18_EJ_BRKIN,
	LTQ_MUX_P0_19_EXINT16
};

static const struct ltq_mux_pin mux_p2[LTQ_MUX_P2_PINS] = {
	LTQ_MUX_P2_0_EBU_A0,
	LTQ_MUX_P2_1_EBU_A1,
	LTQ_MUX_P2_2_EBU_A2,
	LTQ_MUX_P2_3_EBU_A3,
	LTQ_MUX_P2_4_EBU_A4,
	LTQ_MUX_P2_5_EBU_A5,
	LTQ_MUX_P2_6_EBU_A6,
	LTQ_MUX_P2_7_EBU_A7,
	LTQ_MUX_P2_8_EBU_A8,
	LTQ_MUX_P2_9_EBU_A9,
	LTQ_MUX_P2_10_EBU_A10,
	LTQ_MUX_P2_11_EBU_A11,
	LTQ_MUX_P2_12_EBU_RD,
	LTQ_MUX_P2_13_EBU_WR,
	LTQ_MUX_P2_14_EBU_ALE,
	LTQ_MUX_P2_15_EBU_WAIT,
	LTQ_MUX_P2_16_EBU_RDBY,
	LTQ_MUX_P2_17_EBU_BC0,
	LTQ_MUX_P2_18_EBU_BC1
};

static const struct ltq_mux_pin mux_p3[LTQ_MUX_P3_PINS] = {
	LTQ_MUX_P3_0_EBU_AD0,
	LTQ_MUX_P3_1_EBU_AD1,
	LTQ_MUX_P3_2_EBU_AD2,
	LTQ_MUX_P3_3_EBU_AD3,
	LTQ_MUX_P3_4_EBU_AD4,
	LTQ_MUX_P3_5_EBU_AD5,
	LTQ_MUX_P3_6_EBU_AD6,
	LTQ_MUX_P3_7_EBU_AD7,
	LTQ_MUX_P3_8_EBU_AD8,
	LTQ_MUX_P3_9_EBU_AD9,
	LTQ_MUX_P3_10_EBU_AD10,
	LTQ_MUX_P3_11_EBU_AD11,
	LTQ_MUX_P3_12_EBU_AD12,
	LTQ_MUX_P3_13_EBU_AD13,
	LTQ_MUX_P3_14_EBU_AD14,
	LTQ_MUX_P3_15_EBU_AD15,
	LTQ_MUX_P3_16_EBU_CS0,
	LTQ_MUX_P3_17_EBU_CS1,
	LTQ_MUX_P3_18_EBU_CS2,
	LTQ_MUX_P3_19_EBU_CS3
};

static void __init easy336_init_common(void)
{
	svip_sys1_clk_enable(SYS1_CLKENR_L2C |
			     SYS1_CLKENR_DDR2 |
			     SYS1_CLKENR_SMI2 |
			     SYS1_CLKENR_SMI1 |
			     SYS1_CLKENR_SMI0 |
			     SYS1_CLKENR_FMI0 |
			     SYS1_CLKENR_DMA |
			     SYS1_CLKENR_GPTC |
			     SYS1_CLKENR_EBU);

	svip_sys2_clk_enable(SYS2_CLKENR_HWSYNC |
			     SYS2_CLKENR_MBS |
			     SYS2_CLKENR_SWINT |
			     SYS2_CLKENR_HWACC3 |
			     SYS2_CLKENR_HWACC2 |
			     SYS2_CLKENR_HWACC1 |
			     SYS2_CLKENR_HWACC0 |
			     SYS2_CLKENR_SIF7 |
			     SYS2_CLKENR_SIF6 |
			     SYS2_CLKENR_SIF5 |
			     SYS2_CLKENR_SIF4 |
			     SYS2_CLKENR_SIF3 |
			     SYS2_CLKENR_SIF2 |
			     SYS2_CLKENR_SIF1 |
			     SYS2_CLKENR_SIF0 |
			     SYS2_CLKENR_DFEV7 |
			     SYS2_CLKENR_DFEV6 |
			     SYS2_CLKENR_DFEV5 |
			     SYS2_CLKENR_DFEV4 |
			     SYS2_CLKENR_DFEV3 |
			     SYS2_CLKENR_DFEV2 |
			     SYS2_CLKENR_DFEV1 |
			     SYS2_CLKENR_DFEV0);

	svip_register_mux(mux_p0, NULL, mux_p2, mux_p3, NULL);
	svip_register_asc(0);
	svip_register_eth();
	svip_register_virtual_eth();
	/* ltq_register_wdt(); - conflicts with lq_switch */
	svip_register_gpio();
	svip_register_spi();
	ltq_register_tapi();
}

static void __init easy336_init(void)
{
	easy336_init_common();
	ltq_register_nor(&easy336_flash_data);
}

static void __init easy336sf_init(void)
{
	easy336_init_common();
	svip_register_spi_flash(bdinfo);
}

static void __init easy336nand_init(void)
{
	easy336_init_common();
	svip_register_nand();
}

MIPS_MACHINE(LANTIQ_MACH_EASY336,
	     "EASY336",
	     "EASY336",
	     easy336_init);

MIPS_MACHINE(LANTIQ_MACH_EASY336SF,
	     "EASY336SF",
	     "EASY336 (Serial Flash)",
	     easy336sf_init);

MIPS_MACHINE(LANTIQ_MACH_EASY336NAND,
	     "EASY336NAND",
	     "EASY336 (NAND Flash)",
	     easy336nand_init);

