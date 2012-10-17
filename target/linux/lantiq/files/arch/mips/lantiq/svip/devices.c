#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/mtd/physmap.h>
#include <linux/kernel.h>
#include <linux/reboot.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/etherdevice.h>
#include <linux/reboot.h>
#include <linux/time.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/leds.h>
#include <linux/spi/spi.h>
#include <linux/mtd/nand.h>

#include <asm/bootinfo.h>
#include <asm/irq.h>

#include <lantiq.h>

#include <base_reg.h>
#include <sys1_reg.h>
#include <sys2_reg.h>
#include <ebu_reg.h>

#include "devices.h"

#include <lantiq_soc.h>
#include <svip_mux.h>
#include <svip_pms.h>

/* ASC */
void __init svip_register_asc(int port)
{
	switch (port) {
	case 0:
		ltq_register_asc(0);
		svip_sys1_clk_enable(SYS1_CLKENR_ASC0);
		break;
	case 1:
		ltq_register_asc(1);
		svip_sys1_clk_enable(SYS1_CLKENR_ASC1);
		break;
	default:
		break;
	};
}

/* Ethernet */
static unsigned char svip_ethaddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static struct platform_device ltq_mii = {
	.name = "ifxmips_mii0",
	.dev = {
		.platform_data = svip_ethaddr,
	},
};

static int __init svip_set_ethaddr(char *str)
{
	sscanf(str, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
	       &svip_ethaddr[0], &svip_ethaddr[1], &svip_ethaddr[2],
	       &svip_ethaddr[3], &svip_ethaddr[4], &svip_ethaddr[5]);
	return 0;
}
__setup("ethaddr=", svip_set_ethaddr);

void __init svip_register_eth(void)
{
	if (!is_valid_ether_addr(svip_ethaddr))
		random_ether_addr(svip_ethaddr);

	platform_device_register(&ltq_mii);
	svip_sys1_clk_enable(SYS1_CLKENR_ETHSW);
}

/* Virtual Ethernet */
static struct platform_device ltq_ve = {
	.name = "ifxmips_svip_ve",
};

void __init svip_register_virtual_eth(void)
{
	platform_device_register(&ltq_ve);
}

/* SPI */
static void __init ltq_register_ssc(int bus_num, unsigned long base, int irq_rx,
				     int irq_tx, int irq_err, int irq_frm)
{
	struct resource res[] = {
		{
			.name   = "regs",
			.start  = base,
			.end    = base + 0x20 - 1,
			.flags  = IORESOURCE_MEM,
		}, {
			.name   = "rx",
			.start  = irq_rx,
			.flags  = IORESOURCE_IRQ,
		}, {
			.name   = "tx",
			.start  = irq_tx,
			.flags  = IORESOURCE_IRQ,
		}, {
			.name   = "err",
			.start  = irq_err,
			.flags  = IORESOURCE_IRQ,
		}, {
			.name   = "frm",
			.start  = irq_frm,
			.flags  = IORESOURCE_IRQ,
		},
	};

	platform_device_register_simple("ifx_ssc", bus_num, res,
					ARRAY_SIZE(res));
}

static struct spi_board_info bdinfo[] __initdata = {
	{
		.modalias = "xt16",
		.mode = SPI_MODE_3,
		.irq = INT_NUM_IM5_IRL0 + 28,
		.max_speed_hz = 1000000,
		.bus_num = 0,
		.chip_select = 1,
	},
	{
		.modalias = "xt16",
		.mode = SPI_MODE_3,
		.irq = INT_NUM_IM5_IRL0 + 19,
		.max_speed_hz = 1000000,
		.bus_num = 0,
		.chip_select = 2,
	},
	{
		.modalias = "loop",
		.mode = SPI_MODE_0 | SPI_LOOP,
		.irq = -1,
		.max_speed_hz = 10000000,
		.bus_num = 0,
		.chip_select = 3,
	},
};

void __init svip_register_spi(void)
{

	ltq_register_ssc(0, LTQ_SSC0_BASE, INT_NUM_IM1_IRL0 + 6,
			  INT_NUM_IM1_IRL0 + 7, INT_NUM_IM1_IRL0 + 8,
			  INT_NUM_IM1_IRL0 + 9);

	ltq_register_ssc(1, LTQ_SSC1_BASE, INT_NUM_IM1_IRL0 + 10,
			  INT_NUM_IM1_IRL0 + 11, INT_NUM_IM1_IRL0 + 12,
			  INT_NUM_IM1_IRL0 + 13);

	spi_register_board_info(bdinfo, ARRAY_SIZE(bdinfo));

	svip_sys1_clk_enable(SYS1_CLKENR_SSC0 | SYS1_CLKENR_SSC1);
}

void __init svip_register_spi_flash(struct spi_board_info *bdinfo)
{
	spi_register_board_info(bdinfo, 1);
}

/* GPIO */
static struct platform_device ltq_gpio = {
	.name = "ifxmips_gpio",
};

void __init svip_register_gpio(void)
{
	platform_device_register(&ltq_gpio);
}

/* MUX */
static struct ltq_mux_settings ltq_mux_settings;

static struct platform_device ltq_mux = {
	.name = "ltq_mux",
	.dev = {
		.platform_data = &ltq_mux_settings,
	}
};

void __init svip_register_mux(const struct ltq_mux_pin mux_p0[LTQ_MUX_P0_PINS],
			      const struct ltq_mux_pin mux_p1[LTQ_MUX_P1_PINS],
			      const struct ltq_mux_pin mux_p2[LTQ_MUX_P2_PINS],
			      const struct ltq_mux_pin mux_p3[LTQ_MUX_P3_PINS],
			      const struct ltq_mux_pin mux_p4[LTQ_MUX_P4_PINS])
{
	ltq_mux_settings.mux_p0 = mux_p0;
	ltq_mux_settings.mux_p1 = mux_p1;
	ltq_mux_settings.mux_p2 = mux_p2;
	ltq_mux_settings.mux_p3 = mux_p3;
	ltq_mux_settings.mux_p4 = mux_p4;

	if (mux_p0)
		svip_sys1_clk_enable(SYS1_CLKENR_PORT0);

	if (mux_p1)
		svip_sys1_clk_enable(SYS1_CLKENR_PORT1);

	if (mux_p2)
		svip_sys1_clk_enable(SYS1_CLKENR_PORT2);

	if (mux_p3)
		svip_sys1_clk_enable(SYS1_CLKENR_PORT3);

	if (mux_p4)
		svip_sys2_clk_enable(SYS2_CLKENR_PORT4);

	platform_device_register(&ltq_mux);
}

/* NAND */
#define NAND_ADDR_REGION_BASE		(LTQ_EBU_SEG1_BASE)
#define NAND_CLE_BIT			(1 << 3)
#define NAND_ALE_BIT			(1 << 2)

static struct svip_reg_ebu *const ebu = (struct svip_reg_ebu *)LTQ_EBU_BASE;

static int svip_nand_probe(struct platform_device *pdev)
{
	ebu_w32(LTQ_EBU_ADDR_SEL_0_BASE_VAL(CPHYSADDR(NAND_ADDR_REGION_BASE)
					    >> 12)
		| LTQ_EBU_ADDR_SEL_0_MASK_VAL(15)
		| LTQ_EBU_ADDR_SEL_0_MRME_VAL(0)
		| LTQ_EBU_ADDR_SEL_0_REGEN_VAL(1),
		addr_sel_0);

	ebu_w32(LTQ_EBU_CON_0_WRDIS_VAL(0)
		| LTQ_EBU_CON_0_ADSWP_VAL(1)
		| LTQ_EBU_CON_0_AGEN_VAL(0x00)
		| LTQ_EBU_CON_0_SETUP_VAL(1)
		| LTQ_EBU_CON_0_WAIT_VAL(0x00)
		| LTQ_EBU_CON_0_WINV_VAL(0)
		| LTQ_EBU_CON_0_PW_VAL(0x00)
		| LTQ_EBU_CON_0_ALEC_VAL(0)
		| LTQ_EBU_CON_0_BCGEN_VAL(0x01)
		| LTQ_EBU_CON_0_WAITWRC_VAL(1)
		| LTQ_EBU_CON_0_WAITRDC_VAL(1)
		| LTQ_EBU_CON_0_HOLDC_VAL(1)
		| LTQ_EBU_CON_0_RECOVC_VAL(0)
		| LTQ_EBU_CON_0_CMULT_VAL(0x01),
		con_0);

	/*
	 * ECC disabled
	 * CLE, ALE and CS are pulse, all other signal are latches based
	 * CLE and ALE are active high, PRE, WP, SE and CS/CE are active low
	 * OUT_CS_S is disabled
	 * NAND mode is disabled
	 */
	ebu_w32(LTQ_EBU_NAND_CON_ECC_ON_VAL(0)
		| LTQ_EBU_NAND_CON_LAT_EN_VAL(0x38)
		| LTQ_EBU_NAND_CON_OUT_CS_S_VAL(0)
		| LTQ_EBU_NAND_CON_IN_CS_S_VAL(0)
		| LTQ_EBU_NAND_CON_PRE_P_VAL(1)
		| LTQ_EBU_NAND_CON_WP_P_VAL(1)
		| LTQ_EBU_NAND_CON_SE_P_VAL(1)
		| LTQ_EBU_NAND_CON_CS_P_VAL(1)
		| LTQ_EBU_NAND_CON_CLE_P_VAL(0)
		| LTQ_EBU_NAND_CON_ALE_P_VAL(0)
		| LTQ_EBU_NAND_CON_CSMUX_E_VAL(0)
		| LTQ_EBU_NAND_CON_NANDMODE_VAL(0),
		nand_con);

	return 0;
}

static void svip_nand_hwcontrol(struct mtd_info *mtd, int cmd,
				unsigned int ctrl)
{
	struct nand_chip *this = mtd->priv;

	if (ctrl & NAND_CTRL_CHANGE) {
		unsigned long adr;
		/* Coming here means to change either the enable state or
		 * the address for controlling ALE or CLE */

		/* NAND_NCE: Select the chip by setting nCE to low.
		 * This is done in CON register */
		if (ctrl & NAND_NCE)
			ebu_w32_mask(0, LTQ_EBU_NAND_CON_NANDMODE_VAL(1),
				     nand_con);
		else
			ebu_w32_mask(LTQ_EBU_NAND_CON_NANDMODE_VAL(1),
				     0, nand_con);

		/* The addressing of CLE or ALE is done via different addresses.
		   We are now changing the address depending on the given action
		   SVIPs NAND_CLE_BIT = (1 << 3), NAND_CLE = 0x02
		   NAND_ALE_BIT = (1 << 2) = NAND_ALE (0x04) */
		adr = (unsigned long)this->IO_ADDR_W;
		adr &= ~(NAND_CLE_BIT | NAND_ALE_BIT);
		adr |= (ctrl & NAND_CLE) << 2 | (ctrl & NAND_ALE);
		this->IO_ADDR_W = (void __iomem *)adr;
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, this->IO_ADDR_W);
}

static int svip_nand_ready(struct mtd_info *mtd)
{
	return (ebu_r32(nand_wait) & 0x01) == 0x01;
}

static inline void svip_nand_wait(void)
{
	static const int nops = 150;
	int i;

	for (i = 0; i < nops; i++)
		asm("nop");
}

static void svip_nand_write_buf(struct mtd_info *mtd,
				const u_char *buf, int len)
{
	int i;
	struct nand_chip *this = mtd->priv;

	for (i = 0; i < len; i++) {
		writeb(buf[i], this->IO_ADDR_W);
		svip_nand_wait();
	}
}

static void svip_nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	int i;
	struct nand_chip *this = mtd->priv;

	for (i = 0; i < len; i++) {
		buf[i] = readb(this->IO_ADDR_R);
		svip_nand_wait();
	}
}

static const char *part_probes[] = { "cmdlinepart", NULL };

static struct platform_nand_data svip_flash_nand_data = {
	.chip = {
		.nr_chips		= 1,
		.part_probe_types 	= part_probes,
	},
	.ctrl = {
		.probe			= svip_nand_probe,
		.cmd_ctrl 		= svip_nand_hwcontrol,
		.dev_ready		= svip_nand_ready,
		.write_buf		= svip_nand_write_buf,
		.read_buf		= svip_nand_read_buf,
	}
};

static struct resource svip_nand_resources[] = {
	MEM_RES("nand", LTQ_FLASH_START, LTQ_FLASH_MAX),
};

static struct platform_device svip_flash_nand = {
	.name		= "gen_nand",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(svip_nand_resources),
	.resource	= svip_nand_resources,
	.dev		= {
		.platform_data = &svip_flash_nand_data,
	},
};

void __init svip_register_nand(void)
{
	platform_device_register(&svip_flash_nand);
}
