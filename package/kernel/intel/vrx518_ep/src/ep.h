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

#ifndef EP_H
#define EP_H

#include <net/dc_ep.h>

#include "aca.h"

#define DC_EP_MAX_NUM		(DC_EP_MAX_PEER + 1)
#define DC_EP_BAR_NUM		0

/* Maximum 8, if PCIe switch attached, 4 is used. 8 is also default one */
#ifdef CONFIG_VRX518_PCIE_SWITCH_BONDING
#define DC_PCIE_SWITCH_ATTACH		1
#else
#define DC_PCIE_SWITCH_ATTACH		0
#endif /* CONFIG_VRX518_PCIE_SWITCH_BONDING */

#define DC_EP_DEFAULT_MSI_VECTOR	4

#define DC_EP_MAX_REFCNT	DC_EP_INT_MAX

#define MS(_v, _f)  (((_v) & (_f)) >> _f##_S)
#define SM(_v, _f)  (((_v) << _f##_S) & (_f))

enum dc_ep_msi_mode {
	DC_EP_8_MSI_MODE = 0,
	DC_EP_4_MSI_MODE,
	DC_EP_1_MSI_MODE,
};

/* Structure used to extract attached EP detailed information for
 * PPE/DSL_MEI driver/Bonding
 */
struct dc_ep_priv {
	struct pci_dev *pdev;
	struct device *dev;
	u32 ep_idx; /*!< EP logical index, the first found one will be 0
			regardless of RC physical index
			*/
	u32 irq_base; /*!< The first MSI interrupt number */
	u32 irq_num; /*!< How many MSI interrupt supported */
	enum dc_ep_msi_mode msi_mode;
	u8 __iomem *mem;  /*!< The EP inbound memory base address
				derived from BAR0, SoC virtual address
				for PPE/DSL_MEI driver
				*/
	u32 phymem; /*!< The EP inbound memory base address
				derived from BAR0, physical address for
				PPE FW
				*/
	size_t memsize; /*!< The EP inbound memory window size */
	u32 peer_num;  /*!< Bonding peer number available */
	/*!< The bonding peer EP inbound memory base address derived from
	 * its BAR0, SoC virtual address for PPE/DSL_MEI driver
	 */

	u8 __iomem *peer_mem[DC_EP_MAX_PEER];

	/*!< The bonding peer EP inbound memory base address derived from
	 * its BAR0, physical address for PPE FW
	 */
	u32 peer_phymem[DC_EP_MAX_PEER];

	/*!< The bonding peer inbound memory window size */
	size_t peer_memsize[DC_EP_MAX_PEER];
	atomic_t refcnt; /*!< The EP mapping driver referenced times
				by other modules
				*/
	u16 device_id; /* Potential usage for different EP */
	bool switch_attached;
	struct dc_aca aca;
};

struct dc_ep_info {
	int dev_num;
	int msi_mode;
	struct dc_ep_priv pcie_ep[DC_EP_MAX_NUM];
};

static inline struct dc_aca *to_aca(struct dc_ep_priv *priv)
{
	return &priv->aca;
}

void dc_aca_shutdown(struct dc_ep_priv *priv);
void dc_aca_info_init(struct dc_ep_priv *priv);
int dc_aca_start(struct dc_ep_priv *priv, u32 func, int start);
int dc_aca_stop(struct dc_ep_priv *priv, u32 *func, int reset);
int dc_aca_init(struct dc_ep_priv *priv, struct aca_param *aca,
	struct aca_modem_param *mdm);
void dc_aca_event_addr_get(struct dc_ep_priv *priv,
	struct aca_event_reg_addr *regs);
void dc_aca_txin_sub_ack(struct dc_ep_priv *priv, u32 val);
u32 aca_umt_msg_addr(struct dc_ep_priv *priv, u32 endian, u32 type);
u32 dc_aca_txin_hd_cnt(struct dc_ep_priv *priv);
void dc_aca_free_fw_file(struct dc_ep_priv *priv);

/* Card specific private data structure */
struct pcie_ep_adapter {
	struct platform_device *mei_dev; /* the mei driver */
};

#endif /* EP_H */

