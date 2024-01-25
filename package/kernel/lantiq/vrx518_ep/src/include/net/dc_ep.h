/*******************************************************************************

  Intel SmartPHY DSL PCIe Endpoint/ACA Linux driver
  Copyright(c) 2016 Intel Corporation.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

*******************************************************************************/

#ifndef DC_EP_H
#define DC_EP_H

#include <linux/types.h>
#include <linux/pci.h>
#include <linux/device.h>

/* @{ */
/*! \def DC_EP_MAX_PEER
 *  \brief how many EP partners existed. In most cases, this number should be
 *  one for bonding application. For the future extension, it could be bigger
 *  value. For example, multiple bonding
 */
#define DC_EP_MAX_PEER		1

/* Reset related module bit definition */
#define RST_GPIO		BIT(2)
#define RST_DSL_IF		BIT(3)
#define RST_DFE			BIT(7)
#define RST_PPE			BIT(8)
#define RST_CDMA		BIT(9)
#define RST_SPI			BIT(10)
#define RST_IMCU		BIT(11)
#define RST_ACA_DMA		BIT(14)
#define RST_AFE			BIT(16)
#define RST_ACA_HOSTIF		BIT(17)
#define RST_PCIE		BIT(22)
#define RST_PPE_ATM_TC		BIT(23)
#define RST_FPI_SLAVE		BIT(25)
#define RST_GLOBAL		BIT(30)

/* PMU related module definition */
#define PMU_ADMA		BIT(0)
#define PMU_CDMA		BIT(2)
#define PMU_SPI			BIT(8)
#define PMU_DSL			BIT(9)
#define PMU_PPE_QSB		BIT(18)
#define PMU_PPE_SLL01		BIT(19)
#define PMU_PPE_TC		BIT(21)
#define PMU_EMA			BIT(22)
#define PMU_PPM2		BIT(23)
#define PMU_PPE_TOP		BIT(29)

/* IMER bit definition */
#define PPE2HOST_INT0		BIT(0)
#define PPE2HOST_INT1		BIT(1)
#define DYING_GASP_INT		BIT(3)
#define MEI_IRQ			BIT(8)
#define ACA_XBAR_INT		BIT(9)
#define MODEM_XBAR_INT		BIT(12)
#define LED0_INT		BIT(13)
#define LED1_INT		BIT(14)
#define NMI_PLL			BIT(15)
#define DMA_TX			BIT(16)
#define DMA_RX			BIT(17)
#define ACA_HOSTIF_TX		BIT(20)
#define ACA_HOSTIF_RX		BIT(21)
#define ACA_RXOUT_PD_RING_FULL	BIT(22)
#define ACA_TXOUT_PD_RING_FULL	BIT(23)

/*
 * Structure used to specify available pin mux functions for gpio pinx
 * It will be used in pinmux_set() function
 */
enum gpio_padc_func {
	MUX_FUNC_GPIO = 0,
	MUX_FUNC_ALT1,
	MUX_FUNC_ALT2,
	MUX_FUNC_RES,
};

/*
 * Structure used to specify interrupt source so that EP can assign unique
 *  interruot to it
*/
enum dc_ep_int {
	DC_EP_INT_PPE, /*!< PPE2HOST_INT 0/1 */
	DC_EP_INT_MEI, /*!< DSL MEI_IRQ */
	DC_EP_INT_MAX,
};

/* Clock setting for system clock */
enum {
	SYS_CLK_36MHZ	= 0,
	SYS_CLK_288MHZ,
	SYS_CLK_MAX,
};

/* Clock setting for PPE clock */
enum {
	PPE_CLK_36MHZ	= 0,
	PPE_CLK_576MHZ,
	PPE_CLK_494MHZ,
	PPE_CLK_432MHZ,
	PPE_CLK_288MHZ,
	PPE_CLK_MAX,
};

/* GPIO direction IN/OUT */
enum {
	GPIO_DIR_IN = 0,
	GPIO_DIR_OUT,
	GPIO_DIR_MAX,
};

/* GPIO Pullup/Pulldown setting */
enum {
	GPIO_PUPD_DISABLE = 0,
	GPIO_PULL_UP,
	GPIO_PULL_DOWN,
	GPIO_PUPD_BOTH,
};

/* GPIO slew rate setting */
enum {
	GPIO_SLEW_RATE_SLOW = 0,
	GPIO_SLEW_RATE_FAST,
};

/* GPIO driver current setting */
enum {
	GPIO_DRV_CUR_2MA = 0,
	GPIO_DRV_CUR_4MA,
	GPIO_DRV_CUR_8MA,
	GPIO_DRV_CUR_12MA,
	GPIO_DRV_CUR_MAX,
};

enum {
	ACA_LITTLE_ENDIAN = 0,
	ACA_BIG_ENDIAN,
	ACA_ENDIAN_MAX,
};

enum {
	ACA_TXIN = 0,
	ACA_TXOUT,
	ACA_RXIN,
	ACA_RXOUT,
	ACA_MAX,
};

/* ACA four major direction functions for start/stop */
#define ACA_TXIN_EN	BIT(0)
#define ACA_TXOUT_EN	BIT(1)
#define ACA_RXIN_EN	BIT(2)
#define ACA_RXOUT_EN	BIT(3)
#define ACA_ALL_EN	0xF

struct dc_ep_dev;

/*
 * ACA SoC specific parameters. The caller needs to fill up all necessary info
 * according to specific SoC and specific project
 * For each function, different parameters are needed.
 */
struct aca_cfg_param {
	u32 soc_desc_base; /*!< SoC CBM or DDR descriptor base address */
	u32 soc_desc_num; /*!< SoC and HostIF (same) descriptor number */
	u32 soc_cmlt_cnt_addr; /*! SoC cumulative counter address */
	u32 pp_buf_desc_num; /*!< ACA ping pong buffer descriptor number */
	u32 pd_desc_base; /*!< Packet Descriptor base address in modem */
	u32 pd_desc_num; /*!< Packet Descriptor number in modem */
	u32 hd_size_in_dw; /*!< Host(SoC) descriptor size in dwords */
	u32 pd_size_in_dw; /*!< Packet descriptor size in dwords */
	u32 byteswap; /*!< Byte swap enabled or not in ACA FW */
	u32 prefill_cnt; /*!< Prefill counter special required for some platform */
};

struct aca_param {
	struct aca_cfg_param aca_txin;
	struct aca_cfg_param aca_txout;
	struct aca_cfg_param aca_rxin;
	struct aca_cfg_param aca_rxout;
};

/* ACA project/modem specific parameters. It is only valid for VRX518 */
struct aca_proj_param {
	u32 stat; /*!< Target state */
	u32 pd; /*!< Target packet descripor */
	u32 acc_cnt; /*!< Target accumulate counter */
};

/* Project specific configuration */
struct aca_modem_param {
	struct aca_proj_param mdm_txout;
	struct aca_proj_param mdm_rxin;
	struct aca_proj_param mdm_rxout;
};

/* Event trigger register address <offset> */
struct aca_event_reg_addr {
	u32 txin_acc_sub;
	u32 txout_acc_add;
	u32 rxin_acc_sub;
	u32 rxout_acc_add;
};

/*
 * ACA common hardware low level APIs, presented as callbacks instead of
 * separate APIs to support mulitple instances
 */
struct aca_hw_ops {
	/* RCU Callbacks */
	void (*reset_assert)(struct dc_ep_dev *pdev, u32 rd);
	void (*reset_deassert)(struct dc_ep_dev *pdev, u32 rd);
	/* For hardware self-clear reset, most apply except PCIe */
	int (*reset_device)(struct dc_ep_dev *pdev, u32 hd);

	/* PMU Callbacks */
	int (*clk_on)(struct dc_ep_dev *pdev, u32 cd);
	int (*clk_off)(struct dc_ep_dev *pdev, u32 cd);

	/* CGU Callbacks */
	int (*clk_set)(struct dc_ep_dev *pdev, u32 sysclk, u32 ppeclk);
	int (*clk_get)(struct dc_ep_dev *pdev, u32 *sysclk, u32 *ppeclk);

	/* GPIO Callbacks */
	int (*gpio_dir)(struct dc_ep_dev *pdev, u32 gpio, int dir);
	int (*gpio_set)(struct dc_ep_dev *pdev, u32 gpio, int val);
	int (*gpio_get)(struct dc_ep_dev *pdev, u32 gpio, int *val);

	/* PinMux Callbacks */
	int (*pinmux_set)(struct dc_ep_dev *pdev, u32 gpio, int func);
	int (*pinmux_get)(struct dc_ep_dev *pdev, u32 gpio, int *func);
	int (*gpio_pupd_set)(struct dc_ep_dev *pdev, u32 gpio, u32 val);
	int (*gpio_od_set)(struct dc_ep_dev *pdev, u32 gpio, int val);
	int (*gpio_src_set)(struct dc_ep_dev *pdev, u32 gpio, int val);
	int (*gpio_dcc_set)(struct dc_ep_dev *pdev, u32 gpio, u32 val);

	/* ICU Callbacks */
	void (*icu_en)(struct dc_ep_dev *pdev, u32 bit);
	void (*icu_mask)(struct dc_ep_dev *pdev, u32 bit);

	/* ACA related stuff */
	int (*aca_start)(struct dc_ep_dev *pdev, u32 func, int start);
	int (*aca_stop)(struct dc_ep_dev *pdev, u32 *func, int reset);
	/* If there is no project specific parameters, input NULL */
	int (*aca_init)(struct dc_ep_dev *pdev, struct aca_param *aca,
		struct aca_modem_param *mdm);
	void (*aca_event_addr_get)(struct dc_ep_dev *pdev,
		struct aca_event_reg_addr *regs);
	/* UMT address needed for SoC filled in to trigger UMT msg */
	u32 (*umt_msg_addr)(struct dc_ep_dev *pdev, u32 endian, u32 type);
	/* TXIN accum sub to ack PPE already processed */
	void (*aca_txin_ack_sub)(struct dc_ep_dev *pdev, u32 val);
	u32 (*aca_txin_hd_cnt)(struct dc_ep_dev *pdev);
};

/*
 * Structure used to extract attached EP detailed information
 * for PPE/DSL_MEI driver/Bonding
 */
struct dc_ep_dev {
	struct device *dev;
	u32 irq;          /*!< MSI interrupt number for this device */
	u32 aca_tx_irq; /*!< ACA Non-empty TX irq number for PPE driver */
	u32 aca_rx_irq; /*!< ACA Non-empty RX irq number for PPE driver */
	/*!< The EP inbound memory base address derived from BAR0, SoC
	     virtual address for PPE/DSL_MEI driver
	 */
	bool switch_attached; /*!< EP attach switch */
	u8 __iomem *membase; /*!< virtual memory base address to access EP */
	u32 phy_membase;  /*!< The EP inbound memory base address derived
			from BAR0, physical address for PPE FW
			*/
	u32 peer_num;    /*!< Bonding peer number available */
	/*!< The bonding peer EP inbound memory base address derived from
	 its BAR0, SoC virtual address for PPE/DSL_MEI driver
	 */
	u8 __iomem *peer_membase[DC_EP_MAX_PEER];
	/*!< The bonding peer EP inbound memory base address derived from
	     its BAR0, physical address for PPE FW
	*/
	u32 peer_phy_membase[DC_EP_MAX_PEER];
	const struct aca_hw_ops *hw_ops;
	void *priv; /* Pointer to driver proprietary data for internal use */
};

/*
 * This function returns the total number of EPs attached. Normally,
 * the number should be one <standard smartPHY EP> or two <smartPHY
 * off-chip bonding cases>. Extended case is also considered

 * \param[in/out]  dev_num   Pointer to detected EP numbers in total.
 * \return         -EIO      Invalid total EP number which means this
 * 		     module is not initialized properly
 * \return         0         Successfully return the detected EP numbers
 */
int dc_ep_dev_num_get(int *dev_num);

/*
 * This function returns detailed EP device information for PPE/DSL/Bonding
 * partner by its logical index obtained
 * by \ref dc_ep_dev_num_get and its interrupt module number
 * \ref dc_ep_int

 * \param[in]      dev_idx   Logical device index referred to the related
 * 		     device
 * \param[in]      module    EP interrupt module user<PPE/MEI>
 * \param[in/out]  dev       Pointer to returned detail device structure
 * 		     \ref dc_ep_dev
 * \return         -EIO      Invalid logical device index or too many modules
 * 		     referred to this module
 * \return         0         Successfully return required device information

 * \remarks This function normally will be called to trace the detailed device
 *     information after calling \ref dc_ep_dev_num_get
 */
int dc_ep_dev_info_req(int dev_idx, enum dc_ep_int module,
			struct dc_ep_dev *dev);

/*
 * This function releases the usage of this module by PPE/DSL

 * \param[in]  dev_idx   Logical device index referred to the related device
 * \return     -EIO      Invalid logical device index or release too many
 * 		 times to refer to this module
 * \return     0         Successfully release the usage of this module

 * \remarks This function should be called once their reference is over.
 *     The reference usage must matches \ref dc_ep_dev_info_req
 */
int dc_ep_dev_info_release(int dev_idx);

#endif /* DC_EP_H */
