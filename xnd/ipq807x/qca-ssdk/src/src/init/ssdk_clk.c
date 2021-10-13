/*
 * Copyright (c) 2017-2020, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include "sw.h"
#include "ssdk_init.h"
#include "ssdk_plat.h"
#include "ssdk_clk.h"
#include "ssdk_dts.h"
#if defined(HPPE)
#include "adpt_hppe.h"
#endif
#include "fal.h"
#include <linux/kconfig.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
#include <linux/of.h>
#include <linux/reset.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#endif

#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
struct device_node *clock_node = NULL;
static struct clk *uniphy_port_clks[UNIPHYT_CLK_MAX] = {0};

struct device_node *rst_node = NULL;
struct reset_control *uniphy_rsts[UNIPHY_RST_MAX] = {0};
struct reset_control *port_rsts[SSDK_MAX_PORT_NUM] = {0};

/* below 3 routines to be used as common */
void ssdk_clock_rate_set_and_enable(
	struct device_node *node, a_uint8_t* clock_id, a_uint32_t rate)
{
	struct clk *clk;

	if(ssdk_is_emulation(0)){
		SSDK_INFO("clock_id %s rate %d on emulation platform\n",clock_id, rate);
		return;
	}

	clk = of_clk_get_by_name(node, clock_id);
	if (!IS_ERR(clk)) {
		if (rate)
			clk_set_rate(clk, rate);

		clk_prepare_enable(clk);
	}
}

void ssdk_gcc_reset(struct reset_control *rst, a_uint32_t action)
{
	if(ssdk_is_emulation(0)){
		SSDK_INFO("action %d on emulation platform\n",action);
		return;
	}

	if (action == SSDK_RESET_ASSERT)
		reset_control_assert(rst);
	else
		reset_control_deassert(rst);

}
#endif

void ssdk_uniphy_reset(
	a_uint32_t dev_id,
	enum unphy_rst_type rst_type,
	a_uint32_t action)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	struct reset_control *rst;

	rst = uniphy_rsts[rst_type];
	if (IS_ERR(rst)) {
		SSDK_ERROR("reset(%d) nof exist!\n", rst_type);
		return;
	}

	ssdk_gcc_reset(rst, action);
#endif

}

void ssdk_port_reset(
	a_uint32_t dev_id,
	a_uint32_t port_id,
	a_uint32_t action)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	struct reset_control *rst;

	if ((port_id < SSDK_PHYSICAL_PORT1) || (port_id > SSDK_PHYSICAL_PORT6))
		return;

	rst = port_rsts[port_id-1];
	if (IS_ERR(rst)) {
		SSDK_ERROR("reset(%d) not exist!\n", port_id);
		return;
	}

	ssdk_gcc_reset(rst, action);
#endif

}

void ssdk_uniphy_clock_rate_set(
	a_uint32_t dev_id,
	enum unphy_clk_type clock_type,
	a_uint32_t rate)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	struct clk *uniphy_clk;

	if(ssdk_is_emulation(dev_id)){
		SSDK_INFO("clock_type %d rate %d on emulation platform\n",
					clock_type, rate);
		return;
	}

	uniphy_clk = uniphy_port_clks[clock_type];
	if (!IS_ERR(uniphy_clk)) {
		if (rate)
			if (clk_set_rate(uniphy_clk, rate))
				SSDK_INFO("%d set rate=%d fail\n", clock_type, rate);
	} else
		SSDK_INFO("%d set rate %x fail!\n", clock_type, rate);
#endif

}

void ssdk_uniphy_clock_enable(
	a_uint32_t dev_id,
	enum unphy_clk_type clock_type,
	a_bool_t enable)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	struct clk *uniphy_clk;

	if(ssdk_is_emulation(dev_id)){
		SSDK_INFO("clock_type %d enable %d on emulation platform\n",
					clock_type, enable);
		return;
	}

	uniphy_clk = uniphy_port_clks[clock_type];
	if (!IS_ERR(uniphy_clk)) {
		if (enable) {
			if (clk_prepare_enable(uniphy_clk))
				SSDK_ERROR("clock enable fail!\n");
		} else
			clk_disable_unprepare(uniphy_clk);
	} else {
		SSDK_DEBUG("clock_type= %d enable=%d not find\n",
				clock_type, enable);
	}
#endif

}

#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
#if defined(HPPE) || defined(MP)
struct clk_uniphy {
	struct clk_hw hw;
	u8 uniphy_index;
	u8 dir;
	unsigned long rate;
};

#define to_clk_uniphy(_hw) container_of(_hw, struct clk_uniphy, hw)

static unsigned long
uniphy_clks_recalc_rate(struct clk_hw *hw, unsigned long parent_rate)
{
	struct clk_uniphy *uniphy = to_clk_uniphy(hw);

	return uniphy->rate;
}

static int
uniphy_clks_determine_rate(struct clk_hw *hw, struct clk_rate_request *req)
{
	/* add logic for checking the current mode */
	if (req->rate <= UNIPHY_CLK_RATE_125M)
		req->rate = UNIPHY_CLK_RATE_125M;
	else
		req->rate = UNIPHY_CLK_RATE_312M;

	return 0;
}

static int
uniphy_clks_set_rate(struct clk_hw *hw, unsigned long rate,
		     unsigned long parent_rate)
{
	struct clk_uniphy *uniphy = to_clk_uniphy(hw);

	if (rate != UNIPHY_CLK_RATE_125M && rate != UNIPHY_CLK_RATE_312M)
		return -1;

	uniphy->rate = rate;

	return 0;
}

static const struct clk_ops clk_uniphy_ops = {
	.recalc_rate = uniphy_clks_recalc_rate,
	.determine_rate = uniphy_clks_determine_rate,
	.set_rate = uniphy_clks_set_rate,
};
#endif

#if defined(HPPE)

static struct clk_uniphy uniphy0_gcc_rx_clk = {
                .hw.init = &(struct clk_init_data){
                        .name = "uniphy0_gcc_rx_clk",
                        .ops = &clk_uniphy_ops,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0))
			.flags = CLK_IS_ROOT,
#endif
                },
		.uniphy_index = 0,
		.dir = UNIPHY_RX,
		.rate = UNIPHY_DEFAULT_RATE,
};

static struct clk_uniphy uniphy0_gcc_tx_clk = {
                .hw.init = &(struct clk_init_data){
                        .name = "uniphy0_gcc_tx_clk",
                        .ops = &clk_uniphy_ops,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0))
			.flags = CLK_IS_ROOT,
#endif
                },
		.uniphy_index = 0,
		.dir = UNIPHY_TX,
		.rate = UNIPHY_DEFAULT_RATE,
};

static struct clk_uniphy uniphy1_gcc_rx_clk = {
                .hw.init = &(struct clk_init_data){
                        .name = "uniphy1_gcc_rx_clk",
                        .ops = &clk_uniphy_ops,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0))
			.flags = CLK_IS_ROOT,
#endif
                },
		.uniphy_index = 1,
		.dir = UNIPHY_RX,
		.rate = UNIPHY_DEFAULT_RATE,
};

static struct clk_uniphy uniphy1_gcc_tx_clk = {
                .hw.init = &(struct clk_init_data){
                        .name = "uniphy1_gcc_tx_clk",
                        .ops = &clk_uniphy_ops,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0))
			.flags = CLK_IS_ROOT,
#endif
                },
		.uniphy_index = 1,
		.dir = UNIPHY_TX,
		.rate = UNIPHY_DEFAULT_RATE,
};

static struct clk_uniphy uniphy2_gcc_rx_clk = {
                .hw.init = &(struct clk_init_data){
                        .name = "uniphy2_gcc_rx_clk",
                        .ops = &clk_uniphy_ops,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0))
			.flags = CLK_IS_ROOT,
#endif
                },
		.uniphy_index = 2,
		.dir = UNIPHY_RX,
		.rate = UNIPHY_DEFAULT_RATE,
};

static struct clk_uniphy uniphy2_gcc_tx_clk = {
                .hw.init = &(struct clk_init_data){
                        .name = "uniphy2_gcc_tx_clk",
                        .ops = &clk_uniphy_ops,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0))
			.flags = CLK_IS_ROOT,
#endif
                },
		.uniphy_index = 2,
		.dir = UNIPHY_TX,
		.rate = UNIPHY_DEFAULT_RATE,
};

static struct clk_hw *uniphy_raw_clks[SSDK_MAX_UNIPHY_INSTANCE * 2] = {
	&uniphy0_gcc_rx_clk.hw, &uniphy0_gcc_tx_clk.hw,
	&uniphy1_gcc_rx_clk.hw, &uniphy1_gcc_tx_clk.hw,
	&uniphy2_gcc_rx_clk.hw, &uniphy2_gcc_tx_clk.hw,
};

static char *ppe_clk_ids[UNIPHYT_CLK_MAX] = {
	NSS_PORT1_RX_CLK,
	NSS_PORT1_TX_CLK,
	NSS_PORT2_RX_CLK,
	NSS_PORT2_TX_CLK,
	NSS_PORT3_RX_CLK,
	NSS_PORT3_TX_CLK,
	NSS_PORT4_RX_CLK,
	NSS_PORT4_TX_CLK,
	NSS_PORT5_RX_CLK,
	NSS_PORT5_TX_CLK,
	NSS_PORT6_RX_CLK,
	NSS_PORT6_TX_CLK,
	UNIPHY0_PORT1_RX_CLK,
	UNIPHY0_PORT1_TX_CLK,
	UNIPHY0_PORT2_RX_CLK,
	UNIPHY0_PORT2_TX_CLK,
	UNIPHY0_PORT3_RX_CLK,
	UNIPHY0_PORT3_TX_CLK,
	UNIPHY0_PORT4_RX_CLK,
	UNIPHY0_PORT4_TX_CLK,
	UNIPHY0_PORT5_RX_CLK,
	UNIPHY0_PORT5_TX_CLK,
	UNIPHY1_PORT5_RX_CLK,
	UNIPHY1_PORT5_TX_CLK,
	UNIPHY2_PORT6_RX_CLK,
	UNIPHY2_PORT6_TX_CLK,
	PORT5_RX_SRC,
	PORT5_TX_SRC
};

static void ssdk_ppe_uniphy_clock_init(a_uint32_t revision)
{
	a_uint32_t i, inst_num;
	struct clk *clk;

	if (revision == HPPE_REVISION) {
		inst_num = SSDK_MAX_UNIPHY_INSTANCE;
	} else {
		inst_num = SSDK_MAX_UNIPHY_INSTANCE - 1;
	}

	for (i = 0; i < inst_num * 2; i++) {
		clk = clk_register(NULL, uniphy_raw_clks[i]);
		if (IS_ERR(clk))
			SSDK_ERROR("Clk register %d fail!\n", i);
	}

	for (i = NSS_PORT1_RX_CLK_E; i < UNIPHYT_CLK_MAX; i++)
		uniphy_port_clks[i] = of_clk_get_by_name(clock_node,
							ppe_clk_ids[i]);

	/* enable uniphy and mac clock */
	for (i = NSS_PORT1_RX_CLK_E; i < PORT5_RX_SRC_E; i++)
		ssdk_uniphy_clock_enable(0, i, A_TRUE);
}

static void ssdk_ppe_fixed_clock_init(a_uint32_t revision)
{
	/* AHB and sys clk */
	ssdk_clock_rate_set_and_enable(clock_node, CMN_AHB_CLK, 0);
	ssdk_clock_rate_set_and_enable(clock_node, CMN_SYS_CLK, 0);
	ssdk_clock_rate_set_and_enable(clock_node, UNIPHY0_AHB_CLK,
					UNIPHY_AHB_CLK_RATE);
	if (revision == HPPE_REVISION) {
		ssdk_clock_rate_set_and_enable(clock_node,
					UNIPHY0_SYS_CLK,
					UNIPHY_SYS_CLK_RATE);
	} else {
		ssdk_clock_rate_set_and_enable(clock_node,
					UNIPHY0_SYS_CLK,
					CPPE_UNIPHY_SYS_CLK_RATE);
	}
	ssdk_clock_rate_set_and_enable(clock_node, UNIPHY1_AHB_CLK,
					UNIPHY_AHB_CLK_RATE);
	if (revision == HPPE_REVISION) {
		ssdk_clock_rate_set_and_enable(clock_node,
					UNIPHY1_SYS_CLK,
					UNIPHY_SYS_CLK_RATE);
	} else {
		ssdk_clock_rate_set_and_enable(clock_node,
					UNIPHY1_SYS_CLK,
					CPPE_UNIPHY_SYS_CLK_RATE);
	}
	if (revision == HPPE_REVISION) {
		ssdk_clock_rate_set_and_enable(clock_node,
					UNIPHY2_AHB_CLK,
					UNIPHY_AHB_CLK_RATE);
		ssdk_clock_rate_set_and_enable(clock_node,
					UNIPHY2_SYS_CLK,
					UNIPHY_SYS_CLK_RATE);
	}

	/* ppe related fixed clock init */
	ssdk_clock_rate_set_and_enable(clock_node,
					PORT1_MAC_CLK, PPE_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
					PORT2_MAC_CLK, PPE_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
					PORT3_MAC_CLK, PPE_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
					PORT4_MAC_CLK, PPE_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
					PORT5_MAC_CLK, PPE_CLK_RATE);
	if (revision == HPPE_REVISION) {
		ssdk_clock_rate_set_and_enable(clock_node,
					PORT6_MAC_CLK, PPE_CLK_RATE);
	}
	ssdk_clock_rate_set_and_enable(clock_node,
					NSS_PPE_CLK, PPE_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
					NSS_PPE_CFG_CLK, PPE_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
					NSSNOC_PPE_CLK, PPE_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
					NSSNOC_PPE_CFG_CLK, PPE_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
					NSS_EDMA_CLK, PPE_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
					NSS_EDMA_CFG_CLK, PPE_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
					NSS_PPE_IPE_CLK, PPE_CLK_RATE);
	if (revision == HPPE_REVISION) {
		ssdk_clock_rate_set_and_enable(clock_node,
					NSS_PPE_BTQ_CLK, PPE_CLK_RATE);
	}
	ssdk_clock_rate_set_and_enable(clock_node,
					MDIO_AHB_CLK, MDIO_AHB_RATE);
	if (revision == HPPE_REVISION) {
		ssdk_clock_rate_set_and_enable(clock_node,
					NSSNOC_CLK, NSS_NOC_RATE);
	} else {
		ssdk_clock_rate_set_and_enable(clock_node,
					NSSNOC_CLK, NSSNOC_SNOC_RATE);
	}
	ssdk_clock_rate_set_and_enable(clock_node,
					NSSNOC_SNOC_CLK, NSSNOC_SNOC_RATE);
	if (revision == HPPE_REVISION) {
		ssdk_clock_rate_set_and_enable(clock_node,
					MEM_NOC_NSSAXI_CLK, NSS_AXI_RATE);
	}
	ssdk_clock_rate_set_and_enable(clock_node,
					CRYPTO_PPE_CLK, PPE_CLK_RATE);
	if (revision == HPPE_REVISION) {
		ssdk_clock_rate_set_and_enable(clock_node,
					NSS_IMEM_CLK, NSS_IMEM_RATE);
	}
	ssdk_clock_rate_set_and_enable(clock_node,
					NSS_PTP_REF_CLK, PTP_REF_RARE);
	if (revision == CPPE_REVISION) {
		ssdk_clock_rate_set_and_enable(clock_node,
					SNOC_NSSNOC_CLK, NSSNOC_SNOC_RATE);
	}
}
#endif

#if defined(MP)
#define TCSR_ETH_ADDR               0x19475C0
#define TCSR_ETH_SIZE               0x4
#define TCSR_GEPHY_LDO_BIAS_EN      0
#define TCSR_ETH_LDO_RDY            0x4

#define GEPHY_LDO_BIAS_EN           0x1
#define ETH_LDO_RDY                 0x1
#define CMN_PLL_LOCKED_ADDR         0x9B064
#define CMN_PLL_LOCKED_SIZE         0x4
#define CMN_PLL_LOCKED              0x4
#define MP_RAW_CLOCK_INSTANCE       0x2

static char *mp_rst_ids[MP_BCR_RST_MAX] = {
	GEHPY_BCR_RESET_ID,
	UNIPHY_BCR_RESET_ID,
	GMAC0_BCR_RESET_ID,
	GMAC1_BCR_RESET_ID,
	GEPHY_MISC_RESET_ID
};

static struct clk_uniphy gephy_gcc_rx_clk = {
	.hw.init = &(struct clk_init_data){
	.name = "gephy_gcc_rx",
	.ops = &clk_uniphy_ops,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0))
	.flags = CLK_IS_ROOT,
#endif
	},
	.uniphy_index = 0,
	.dir = UNIPHY_RX,
	.rate = UNIPHY_DEFAULT_RATE,
};

static struct clk_uniphy gephy_gcc_tx_clk = {
	.hw.init = &(struct clk_init_data){
	.name = "gephy_gcc_tx",
	.ops = &clk_uniphy_ops,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0))
	.flags = CLK_IS_ROOT,
#endif
	},
	.uniphy_index = 0,
	.dir = UNIPHY_TX,
	.rate = UNIPHY_DEFAULT_RATE,
};

static struct clk_uniphy uniphy_gcc_rx_clk = {
	.hw.init = &(struct clk_init_data){
	.name = "uniphy_gcc_rx",
	.ops = &clk_uniphy_ops,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0))
	.flags = CLK_IS_ROOT,
#endif
	},
	.uniphy_index = 1,
	.dir = UNIPHY_RX,
	.rate = UNIPHY_DEFAULT_RATE,
};

static struct clk_uniphy uniphy_gcc_tx_clk = {
	.hw.init = &(struct clk_init_data){
	.name = "uniphy_gcc_tx",
	.ops = &clk_uniphy_ops,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0))
	.flags = CLK_IS_ROOT,
#endif
	},
	.uniphy_index = 1,
	.dir = UNIPHY_TX,
	.rate = UNIPHY_DEFAULT_RATE,
};

static struct clk_hw *mp_raw_clks[MP_RAW_CLOCK_INSTANCE * 2] = {
	&gephy_gcc_rx_clk.hw, &gephy_gcc_tx_clk.hw,
	&uniphy_gcc_rx_clk.hw, &uniphy_gcc_tx_clk.hw,
};

static void ssdk_mp_fixed_clock_init(void)
{
	ssdk_clock_rate_set_and_enable(clock_node, CMN_AHB_CLK, 0);
	ssdk_clock_rate_set_and_enable(clock_node, CMN_SYS_CLK, 0);
	ssdk_clock_rate_set_and_enable(clock_node, UNIPHY_AHB_CLK,
				UNIPHY_AHB_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
				UNIPHY_SYS_CLK,
				MP_UNIPHY_SYS_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
				MDIO0_AHB_CLK, MDIO_AHB_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
				MDIO1_AHB_CLK, MDIO_AHB_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
				GMAC0_CFG_CLK, GMAC_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
				GMAC0_SYS_CLK, GMAC_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
				GMAC1_CFG_CLK, GMAC_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
				GMAC1_SYS_CLK, GMAC_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
				GMAC0_PTP_CLK, GMAC_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
				GMAC1_PTP_CLK, GMAC_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
				SNOC_GMAC0_AHB_CLK, GMAC_CLK_RATE);
	ssdk_clock_rate_set_and_enable(clock_node,
				SNOC_GMAC1_AHB_CLK, GMAC_CLK_RATE);
}

static void ssdk_mp_uniphy_clock_init(void)
{
	a_uint32_t i, inst_num;
	struct clk *clk;
	struct clk **ports;

	inst_num = sizeof(mp_raw_clks) / sizeof(struct clk_hw *);

	for (i = 0; i < inst_num; i++) {
		clk = clk_register(NULL, mp_raw_clks[i]);
		if (IS_ERR(clk))
			SSDK_ERROR("Clk register %d fail!\n", i);
	}

	ports = uniphy_port_clks;
	ports[NSS_PORT1_RX_CLK_E] = of_clk_get_by_name(clock_node,
					NSS_PORT1_RX_CLK);
	ports[NSS_PORT1_TX_CLK_E] = of_clk_get_by_name(clock_node,
					NSS_PORT1_TX_CLK);
	ports[NSS_PORT2_RX_CLK_E] = of_clk_get_by_name(clock_node,
					NSS_PORT2_RX_CLK);
	ports[NSS_PORT2_TX_CLK_E] = of_clk_get_by_name(clock_node,
					NSS_PORT2_TX_CLK);
	ports[UNIPHY0_PORT1_RX_CLK_E] = of_clk_get_by_name(clock_node,
					UNIPHY0_PORT1_RX_CLK);
	ports[UNIPHY0_PORT1_TX_CLK_E] = of_clk_get_by_name(clock_node,
					UNIPHY0_PORT1_TX_CLK);
	ports[UNIPHY1_PORT5_RX_CLK_E] = of_clk_get_by_name(clock_node,
					UNIPHY1_PORT5_RX_CLK);
	ports[UNIPHY1_PORT5_TX_CLK_E] = of_clk_get_by_name(clock_node,
					UNIPHY1_PORT5_TX_CLK);
}

static void ssdk_mp_uniphy_clock_enable(void)
{
	ssdk_uniphy_clock_enable(0, NSS_PORT1_RX_CLK_E, A_TRUE);
	ssdk_uniphy_clock_enable(0, NSS_PORT1_TX_CLK_E, A_TRUE);
	ssdk_uniphy_clock_enable(0, NSS_PORT2_RX_CLK_E, A_TRUE);
	ssdk_uniphy_clock_enable(0, NSS_PORT2_TX_CLK_E, A_TRUE);
	ssdk_uniphy_clock_enable(0, UNIPHY0_PORT1_RX_CLK_E, A_TRUE);
	ssdk_uniphy_clock_enable(0, UNIPHY0_PORT1_TX_CLK_E, A_TRUE);
	ssdk_uniphy_clock_enable(0, UNIPHY1_PORT5_RX_CLK_E, A_TRUE);
	ssdk_uniphy_clock_enable(0, UNIPHY1_PORT5_TX_CLK_E, A_TRUE);
}

static void
ssdk_mp_tcsr_get(a_uint32_t tcsr_offset, a_uint32_t *tcsr_val)
{
	void __iomem *tcsr_base = NULL;

	tcsr_base = ioremap_nocache(TCSR_ETH_ADDR, TCSR_ETH_SIZE);
	if (!tcsr_base)
	{
		SSDK_ERROR("Failed to map tcsr eth address!\n");
		return;
	}
	*tcsr_val = readl(tcsr_base + tcsr_offset);
	iounmap(tcsr_base);

	return;
}

static void
ssdk_mp_tcsr_set(a_uint32_t tcsr_offset, a_uint32_t tcsr_val)
{
	void __iomem *tcsr_base = NULL;

	tcsr_base = ioremap_nocache(TCSR_ETH_ADDR, TCSR_ETH_SIZE);
	if (!tcsr_base)
	{
		SSDK_ERROR("Failed to map tcsr eth address!\n");
		return;
	}
	writel(tcsr_val, tcsr_base + tcsr_offset);
	iounmap(tcsr_base);

	return;
}

static void
ssdk_mp_cmnblk_enable(void)
{
	a_uint32_t reg_val;

	ssdk_mp_tcsr_get(TCSR_ETH_LDO_RDY, &reg_val);
	reg_val |= ETH_LDO_RDY;
	ssdk_mp_tcsr_set(TCSR_ETH_LDO_RDY, reg_val);

	return;
}

void
ssdk_mp_gephy_icc_efuse_load_enable(a_bool_t enable)
{
	a_uint32_t reg_val;

	ssdk_mp_tcsr_get(TCSR_GEPHY_LDO_BIAS_EN, &reg_val);
	if(!enable)
	{
		reg_val |= GEPHY_LDO_BIAS_EN;
	}
	else
	{
		reg_val &= ~GEPHY_LDO_BIAS_EN;
	}
	ssdk_mp_tcsr_set(TCSR_GEPHY_LDO_BIAS_EN, reg_val);
}

static a_bool_t
ssdk_mp_cmnblk_stable_check(void)
{
	void __iomem *pll_lock = NULL;
	a_uint32_t reg_val;
	int i, loops = 20;

	pll_lock = ioremap_nocache(CMN_PLL_LOCKED_ADDR, CMN_PLL_LOCKED_SIZE);
	if (!pll_lock) {
		SSDK_ERROR("Failed to map CMN PLL LOCK register!\n");
		return A_FALSE;
	}

	for (i = 0; i < loops; i++) {
		reg_val = readl(pll_lock);
		if (reg_val & CMN_PLL_LOCKED) {
			break;
		}
		msleep(10);
	}

	iounmap(pll_lock);

	if (i >= loops) {
		return A_FALSE;
	} else {
		return A_TRUE;
	}
}

static void
ssdk_mp_reset_init(void)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	struct reset_control *rst;
	a_uint32_t i;

	rst_node = of_find_node_by_name(NULL, "ess-switch");

	for (i = 0; i < MP_BCR_RST_MAX; i++) {
		rst = of_reset_control_get(rst_node, mp_rst_ids[i]);
		if (IS_ERR(rst)) {
			SSDK_ERROR("%s not exist!\n", mp_rst_ids[i]);
			return;
		}
		ssdk_gcc_reset(rst, SSDK_RESET_ASSERT);
		msleep(200);
		ssdk_gcc_reset(rst, SSDK_RESET_DEASSERT);
		msleep(200);
		reset_control_put(rst);
	}

	i = UNIPHY1_SOFT_RESET_E;
	uniphy_rsts[i] = of_reset_control_get(rst_node, UNIPHY1_SOFT_RESET_ID);

	SSDK_INFO("MP reset successfully!\n");
#endif
}

static void ssdk_cmnblk_pll_src_set(enum cmnblk_pll_src_type pll_source)
{
	void __iomem *cmn_pll_src_base = NULL;
	a_uint32_t reg_val;

	cmn_pll_src_base = ioremap_nocache(CMN_BLK_PLL_SRC_ADDR, CMN_BLK_SIZE);
	if (!cmn_pll_src_base) {
		SSDK_ERROR("Failed to map cmn pll source address!\n");
		return;
	}
	reg_val = readl(cmn_pll_src_base);
	reg_val = (reg_val & PLL_CTRL_SRC_MASK) | (pll_source << 0x8);
	writel(reg_val, cmn_pll_src_base);
	iounmap(cmn_pll_src_base);

	return;
}
#endif
#endif

#if defined(HPPE) || defined(MP)
static void ssdk_cmnblk_init(enum cmnblk_clk_type mode)
{
	void __iomem *gcc_pll_base = NULL;
	a_uint32_t reg_val;

	gcc_pll_base = ioremap_nocache(CMN_BLK_ADDR, CMN_BLK_SIZE);
	if (!gcc_pll_base) {
		SSDK_ERROR("Failed to map gcc pll address!\n");
		return;
	}
	reg_val = readl(gcc_pll_base + 4);

	switch (mode) {
		case INTERNAL_48MHZ:
			reg_val = (reg_val & FREQUENCY_MASK) | INTERNAL_48MHZ_CLOCK;
			break;
		case EXTERNAL_50MHZ:
			reg_val = (reg_val & FREQUENCY_MASK) | EXTERNAL_50MHZ_CLOCK;
			break;
		case EXTERNAL_25MHZ:
			reg_val = (reg_val & FREQUENCY_MASK) | EXTERNAL_25MHZ_CLOCK;
			break;
		case EXTERNAL_31250KHZ:
			reg_val = (reg_val & FREQUENCY_MASK) | EXTERNAL_31250KHZ_CLOCK;
			break;
		case EXTERNAL_40MHZ:
			reg_val = (reg_val & FREQUENCY_MASK) | EXTERNAL_40MHZ_CLOCK;
			break;
		case EXTERNAL_48MHZ:
			reg_val = (reg_val & FREQUENCY_MASK) | EXTERNAL_48MHZ_CLOCK;
			break;
#if defined(MP)
		case INTERNAL_96MHZ:
			ssdk_cmnblk_pll_src_set(CMN_BLK_PLL_SRC_SEL_FROM_REG);
			reg_val = (reg_val & PLL_REFCLK_DIV_MASK) | PLL_REFCLK_DIV_2;
			break;
#endif
		default:
			return;
	}

	writel(reg_val, gcc_pll_base + 0x4);
	reg_val = readl(gcc_pll_base);
	reg_val = reg_val | 0x40;
	writel(reg_val, gcc_pll_base);
	msleep(1);
	reg_val = reg_val & (~0x40);
	writel(reg_val, gcc_pll_base);
	msleep(1);
	writel(0xbf, gcc_pll_base);
	msleep(1);
	writel(0xff, gcc_pll_base);
	msleep(1);

	iounmap(gcc_pll_base);
}

void ssdk_port_mac_clock_reset(
	a_uint32_t dev_id,
	a_uint32_t port_id)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	ssdk_port_reset(dev_id, port_id, SSDK_RESET_ASSERT);
	msleep(150);
	ssdk_port_reset(dev_id, port_id, SSDK_RESET_DEASSERT);
	msleep(150);
#endif
	return;
}
#endif

#if defined(HPPE)
static
void ssdk_uniphy1_clock_source_set(void)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	clk_set_parent(uniphy_port_clks[PORT5_RX_SRC_E],
			uniphy_raw_clks[2]->clk);
	clk_set_parent(uniphy_port_clks[PORT5_TX_SRC_E],
			uniphy_raw_clks[3]->clk);
#endif
}

void ssdk_uniphy_raw_clock_reset(a_uint8_t uniphy_index)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	a_uint32_t id;

	if (uniphy_index >= SSDK_MAX_UNIPHY_INSTANCE)
		return;

	id = uniphy_index*2;
	if (clk_set_rate(uniphy_raw_clks[id]->clk, UNIPHY_DEFAULT_RATE))
		SSDK_ERROR("set rate for %d fail!\n", id);
	if (clk_set_rate(uniphy_raw_clks[id+1]->clk, UNIPHY_DEFAULT_RATE))
		SSDK_ERROR("set rate for %d fail!\n", id+1);
#endif
}

void ssdk_uniphy_raw_clock_set(
	a_uint8_t uniphy_index,
	a_uint8_t direction,
	a_uint32_t clock)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	a_uint32_t old_clock, id, mode;

	if ((uniphy_index >= SSDK_MAX_UNIPHY_INSTANCE) ||
	     (direction > UNIPHY_TX) ||
	     (clock != UNIPHY_CLK_RATE_125M &&
	      clock != UNIPHY_CLK_RATE_312M))
		return;

	id = uniphy_index*2 + direction;
	old_clock = clk_get_rate(uniphy_raw_clks[id]->clk);

	if (clock != old_clock) {
		if (uniphy_index == SSDK_UNIPHY_INSTANCE1) {
			if (UNIPHY_RX == direction)
				ssdk_uniphy_clock_rate_set(0,
						NSS_PORT5_RX_CLK_E,
						NSS_PORT5_DFLT_RATE);
			else
				ssdk_uniphy_clock_rate_set(0,
						NSS_PORT5_TX_CLK_E,
						NSS_PORT5_DFLT_RATE);
		}
		if (clk_set_rate(uniphy_raw_clks[id]->clk, clock))
			SSDK_ERROR("set rate: %d fail!\n", clock);
	}

	mode = ssdk_dt_global_get_mac_mode(0, SSDK_UNIPHY_INSTANCE1);
	if (((uniphy_index == SSDK_UNIPHY_INSTANCE0) &&
	     (mode == PORT_INTERFACE_MODE_MAX)) ||
	    (uniphy_index == SSDK_UNIPHY_INSTANCE1)) {
		if (clk_set_parent(uniphy_port_clks[PORT5_RX_SRC_E + direction],
				uniphy_raw_clks[id]->clk))
			SSDK_ERROR("set parent fail!\n");
	}
#endif
}

void ssdk_uniphy_port5_clock_source_set(void)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	a_uint32_t id, mode, i;

	mode = ssdk_dt_global_get_mac_mode(0, SSDK_UNIPHY_INSTANCE1);

	for (i = UNIPHY_RX; i <= UNIPHY_TX; i++) {
		if (mode == PORT_INTERFACE_MODE_MAX) {
			id = SSDK_UNIPHY_INSTANCE0*2 + i;
		} else {
			id = SSDK_UNIPHY_INSTANCE1*2 + i;
		}
		if (clk_set_parent(uniphy_port_clks[PORT5_RX_SRC_E + i],
			uniphy_raw_clks[id]->clk)) {
			SSDK_ERROR("set parent fail!\n");
		}
	}
#endif
}

static
void ssdk_gcc_ppe_clock_init(a_uint32_t revision, enum cmnblk_clk_type mode)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	ssdk_ppe_fixed_clock_init(revision);
	/*fixme for cmn clock init*/
	ssdk_cmnblk_init(mode);
	ssdk_ppe_uniphy_clock_init(revision);
	ssdk_uniphy_port5_clock_source_set();
#endif
}
#endif

#if defined(MP)
void ssdk_gcc_mp_clock_init(enum cmnblk_clk_type mode)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	ssdk_mp_fixed_clock_init();
	ssdk_mp_uniphy_clock_init();
	ssdk_cmnblk_init(mode);
	msleep(200);
	ssdk_mp_cmnblk_enable();
	if (ssdk_mp_cmnblk_stable_check()) {
		ssdk_mp_reset_init();
		ssdk_mp_uniphy_clock_enable();
	} else {
		SSDK_ERROR("Cmnblock is still not stable!\n");
	}
#endif
}

void ssdk_mp_raw_clock_set(
	a_uint8_t uniphy_index,
	a_uint8_t direction,
	a_uint32_t clock)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	a_uint32_t old_clock, id;

	if ((uniphy_index >= MP_RAW_CLOCK_INSTANCE) ||
	     (direction > UNIPHY_TX) ||
	     (clock != UNIPHY_CLK_RATE_125M &&
	      clock != UNIPHY_CLK_RATE_312M))
		return;

	if(ssdk_is_emulation(0)){
		SSDK_INFO("uniphy_index %d direction %d clock %d on emulation platform\n",
					uniphy_index, direction, clock);
		return;
	}

	id = uniphy_index*2 + direction;
	old_clock = clk_get_rate(mp_raw_clks[id]->clk);

	if (clock != old_clock) {
		if (clk_set_rate(mp_raw_clks[id]->clk, clock))
			SSDK_ERROR("set rate: %d fail!\n", clock);
	}
#endif
}

#endif

#if defined(HPPE) || defined(MP)
void ssdk_gcc_clock_init(void)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	enum cmnblk_clk_type cmnblk_clk_mode = INTERNAL_48MHZ;
	a_uint8_t *mode = NULL;

	clock_node = of_find_node_by_name(NULL, "ess-switch");
	if (of_property_read_string(clock_node, "cmnblk_clk",
				    (const char **)&mode)) {
		cmnblk_clk_mode = INTERNAL_48MHZ;
	} else {
		if (!strcmp(mode, "external_50MHz")) {
			cmnblk_clk_mode = EXTERNAL_50MHZ;
		} else if (!strcmp(mode, "external_25MHz")) {
			cmnblk_clk_mode = EXTERNAL_25MHZ;
		} else if (!strcmp(mode, "external_31250KHz")) {
			cmnblk_clk_mode = EXTERNAL_31250KHZ;
		} else if (!strcmp(mode, "external_40MHz")) {
			cmnblk_clk_mode = EXTERNAL_40MHZ;
		} else if (!strcmp(mode, "external_48MHz")) {
			cmnblk_clk_mode = EXTERNAL_48MHZ;
		} else if (!strcmp(mode, "internal_96MHz")) {
			cmnblk_clk_mode = INTERNAL_96MHZ;
		}
	}

	if (of_device_is_compatible(clock_node, "qcom,ess-switch-ipq807x")) {
#if defined(HPPE)
		ssdk_gcc_ppe_clock_init(HPPE_REVISION, cmnblk_clk_mode);
#endif
	} else if (of_device_is_compatible(clock_node,
			"qcom,ess-switch-ipq60xx")) {
#if defined(HPPE)
		ssdk_gcc_ppe_clock_init(CPPE_REVISION, cmnblk_clk_mode);
#endif
	} else if (of_device_is_compatible(clock_node,
			"qcom,ess-switch-ipq50xx")) {
#if defined(MP)
		ssdk_gcc_mp_clock_init(cmnblk_clk_mode);
#endif
	}
#endif
	SSDK_INFO("SSDK gcc clock init successfully!\n");
}

void
qca_gcc_uniphy_port_clock_set(
	a_uint32_t dev_id, a_uint32_t uniphy_index,
	a_uint32_t port_id, a_bool_t enable)
{

	if (uniphy_index == SSDK_UNIPHY_INSTANCE2) {
		ssdk_uniphy_clock_enable(dev_id,
					UNIPHY2_PORT6_RX_CLK_E, enable);
		ssdk_uniphy_clock_enable(dev_id,
					UNIPHY2_PORT6_TX_CLK_E, enable);
	} else if (uniphy_index == SSDK_UNIPHY_INSTANCE1) {
		ssdk_uniphy_clock_enable(dev_id,
					UNIPHY1_PORT5_RX_CLK_E, enable);
		ssdk_uniphy_clock_enable(dev_id,
					UNIPHY1_PORT5_TX_CLK_E, enable);
	} else if (uniphy_index == SSDK_UNIPHY_INSTANCE0) {
		switch (port_id) {
			case SSDK_PHYSICAL_PORT1:
				ssdk_uniphy_clock_enable(dev_id,
							UNIPHY0_PORT1_RX_CLK_E,
							enable);
				ssdk_uniphy_clock_enable(dev_id,
							UNIPHY0_PORT1_TX_CLK_E,
							enable);
				break;
			case SSDK_PHYSICAL_PORT2:
				ssdk_uniphy_clock_enable(dev_id,
							UNIPHY0_PORT2_RX_CLK_E,
							enable);
				ssdk_uniphy_clock_enable(dev_id,
							UNIPHY0_PORT2_TX_CLK_E,
							enable);
				break;
			case SSDK_PHYSICAL_PORT3:
				ssdk_uniphy_clock_enable(dev_id,
							UNIPHY0_PORT3_RX_CLK_E,
							enable);
				ssdk_uniphy_clock_enable(dev_id,
							UNIPHY0_PORT3_TX_CLK_E,
							enable);
				break;
			case SSDK_PHYSICAL_PORT4:
				ssdk_uniphy_clock_enable(dev_id,
							UNIPHY0_PORT4_RX_CLK_E,
							enable);
				ssdk_uniphy_clock_enable(dev_id,
							UNIPHY0_PORT4_TX_CLK_E,
							enable);
				break;
			case SSDK_PHYSICAL_PORT5:
				ssdk_uniphy_clock_enable(dev_id,
							UNIPHY0_PORT5_RX_CLK_E,
							enable);
				ssdk_uniphy_clock_enable(dev_id,
							UNIPHY0_PORT5_TX_CLK_E,
							enable);
				break;
			default:
				break;
		}
	}
}

void
qca_gcc_mac_port_clock_set(
	a_uint32_t dev_id,
	a_uint32_t port_id,
	a_bool_t enable)
{

	switch (port_id) {
		case SSDK_PHYSICAL_PORT1:
			ssdk_uniphy_clock_enable(dev_id,
						NSS_PORT1_RX_CLK_E,
						enable);
			ssdk_uniphy_clock_enable(dev_id,
						NSS_PORT1_TX_CLK_E,
						enable);
			break;
		case SSDK_PHYSICAL_PORT2:
			ssdk_uniphy_clock_enable(dev_id,
						NSS_PORT2_RX_CLK_E,
						enable);
			ssdk_uniphy_clock_enable(dev_id,
						NSS_PORT2_TX_CLK_E,
						enable);
			break;
		case SSDK_PHYSICAL_PORT3:
			ssdk_uniphy_clock_enable(dev_id,
						NSS_PORT3_RX_CLK_E,
						enable);
			ssdk_uniphy_clock_enable(dev_id,
						NSS_PORT3_TX_CLK_E,
						enable);
			break;
		case SSDK_PHYSICAL_PORT4:
			ssdk_uniphy_clock_enable(dev_id,
						NSS_PORT4_RX_CLK_E,
						enable);
			ssdk_uniphy_clock_enable(dev_id,
						NSS_PORT4_TX_CLK_E,
						enable);
			break;
		case SSDK_PHYSICAL_PORT5:
			ssdk_uniphy_clock_enable(dev_id,
						NSS_PORT5_RX_CLK_E,
						enable);
			ssdk_uniphy_clock_enable(dev_id,
						NSS_PORT5_TX_CLK_E,
						enable);
			break;
		case SSDK_PHYSICAL_PORT6:
			ssdk_uniphy_clock_enable(dev_id,
						NSS_PORT6_RX_CLK_E,
						enable);
			ssdk_uniphy_clock_enable(dev_id,
						NSS_PORT6_TX_CLK_E,
						enable);
			break;
		default:
			break;
	}
}

void
ssdk_port_speed_clock_set(
	a_uint32_t dev_id,
	a_uint32_t port_id,
	a_uint32_t rate)
{
#if defined(HPPE)
	a_uint32_t mode = 0;
#endif

               switch (port_id ) {
		case SSDK_PHYSICAL_PORT1:
			ssdk_uniphy_clock_rate_set(dev_id,
					NSS_PORT1_RX_CLK_E, rate);
			ssdk_uniphy_clock_rate_set(dev_id,
					NSS_PORT1_TX_CLK_E, rate);
			break;
		case SSDK_PHYSICAL_PORT2:
			ssdk_uniphy_clock_rate_set(dev_id,
					NSS_PORT2_RX_CLK_E, rate);
			ssdk_uniphy_clock_rate_set(dev_id,
					NSS_PORT2_TX_CLK_E, rate);
			break;
#if defined(HPPE)
		case SSDK_PHYSICAL_PORT3:
			ssdk_uniphy_clock_rate_set(dev_id,
					NSS_PORT3_RX_CLK_E, rate);
			ssdk_uniphy_clock_rate_set(dev_id,
					NSS_PORT3_TX_CLK_E, rate);
			break;
		case SSDK_PHYSICAL_PORT4:
			ssdk_uniphy_clock_rate_set(dev_id,
					NSS_PORT4_RX_CLK_E, rate);
			ssdk_uniphy_clock_rate_set(dev_id,
					NSS_PORT4_TX_CLK_E, rate);
			break;
		case SSDK_PHYSICAL_PORT5:
			ssdk_uniphy_clock_rate_set(dev_id,
					NSS_PORT5_RX_CLK_E, rate);
			ssdk_uniphy_clock_rate_set(dev_id,
					NSS_PORT5_TX_CLK_E, rate);
			mode = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE1);
			if (mode != PORT_INTERFACE_MODE_MAX)
				ssdk_uniphy1_clock_source_set();
			break;
		case SSDK_PHYSICAL_PORT6:
			ssdk_uniphy_clock_rate_set(dev_id,
					NSS_PORT6_RX_CLK_E, rate);
			ssdk_uniphy_clock_rate_set(dev_id,
					NSS_PORT6_TX_CLK_E, rate);
			break;
#endif
		default:
			break;
	}
}
#endif

#if defined(HPPE)
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
static char *ppe_rst_ids[UNIPHY_RST_MAX] = {
	UNIPHY0_SOFT_RESET_ID,
	UNIPHY0_XPCS_RESET_ID,
	UNIPHY1_SOFT_RESET_ID,
	UNIPHY1_XPCS_RESET_ID,
	UNIPHY2_SOFT_RESET_ID,
	UNIPHY2_XPCS_RESET_ID,
	UNIPHY0_PORT1_DISABLE_ID,
	UNIPHY0_PORT2_DISABLE_ID,
	UNIPHY0_PORT3_DISABLE_ID,
	UNIPHY0_PORT4_DISABLE_ID,
	UNIPHY0_PORT5_DISABLE_ID,
	UNIPHY0_PORT_4_5_RESET_ID,
	UNIPHY0_PORT_4_RESET_ID
};
static char *port_rst_ids[SSDK_MAX_PORT_NUM] = {
	SSDK_PORT1_RESET_ID,
	SSDK_PORT2_RESET_ID,
	SSDK_PORT3_RESET_ID,
	SSDK_PORT4_RESET_ID,
	SSDK_PORT5_RESET_ID,
	SSDK_PORT6_RESET_ID,
	NULL, NULL
};
#endif

void ssdk_ppe_reset_init(void)
{
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	struct reset_control *rst;
	a_uint32_t i;

	rst_node = of_find_node_by_name(NULL, "ess-switch");
	rst = of_reset_control_get(rst_node, PPE_RESET_ID);
	if (IS_ERR(rst)) {
		SSDK_ERROR("%s not exist!\n", PPE_RESET_ID);
		return;
	}

	ssdk_gcc_reset(rst, SSDK_RESET_ASSERT);
	msleep(100);
	ssdk_gcc_reset(rst, SSDK_RESET_DEASSERT);
	msleep(100);
	reset_control_put(rst);
	SSDK_INFO("ppe reset successfully!\n");

	for (i = UNIPHY0_SOFT_RESET_E; i < UNIPHY_RST_MAX; i++)
		uniphy_rsts[i] = of_reset_control_get(rst_node,
							ppe_rst_ids[i]);

	for (i = SSDK_PHYSICAL_PORT1; i < SSDK_PHYSICAL_PORT7; i++)
		port_rsts[i-1] = of_reset_control_get(rst_node,
							port_rst_ids[i-1]);
#endif
}

void ssdk_gcc_uniphy_sys_set(a_uint32_t dev_id, a_uint32_t uniphy_index,
	a_bool_t enable)
{
	enum unphy_rst_type rst_type;

	if (of_device_is_compatible(clock_node, "qcom,ess-switch-ipq60xx")){
		if (uniphy_index > SSDK_UNIPHY_INSTANCE1) {
			return;
		}
	}

	if (uniphy_index == SSDK_UNIPHY_INSTANCE0) {
		rst_type = UNIPHY0_SOFT_RESET_E;
	} else if (uniphy_index == SSDK_UNIPHY_INSTANCE1) {
		rst_type = UNIPHY1_SOFT_RESET_E;
	} else {
		rst_type = UNIPHY2_SOFT_RESET_E;
	}

	if (enable == A_TRUE) {
		ssdk_uniphy_reset(dev_id, rst_type, SSDK_RESET_DEASSERT);
	} else {
		ssdk_uniphy_reset(dev_id, rst_type, SSDK_RESET_ASSERT);
	}

	return;
}
#endif

