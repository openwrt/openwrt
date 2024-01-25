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

#include <linux/delay.h>
#include <linux/mutex.h>

#include "regs.h"
#include "ep.h"
#include "misc.h"

#define padc_getbit(p, r)	(!!(rd32(r) & (1 << p)))
#define padc_setbit(p, r)	wr32_mask(0, BIT(p), r)
#define padc_clearbit(p, r)	wr32_mask(BIT(p), 0, r)

void dc_ep_clkod_disable(struct dc_ep_priv *priv)
{
	wr32_mask(0, IF_CLKOD_ALL, IF_CLK);
}

void dc_ep_icu_init(struct dc_ep_priv *priv)
{
	/* Enable all interrupts in ICU level */
	wr32(ICU_DMA_TX_ALL, ICU_DMA_TX_IMER);
	wr32(ICU_DMA_RX_ALL, ICU_DMA_RX_IMER);
	wr32(ICU_TOP_ALL, ICU_IMER);

	if (priv->msi_mode == DC_EP_4_MSI_MODE)
		wr32(PCI_MSI_4_MODE, RCU_MSI);
	else
		wr32(PCI_MSI_8_MODE, RCU_MSI);

	/* PCIe app has to enable all MSI interrupts regardless of MSI mode */
	wr32(PCIE_MSI_EN_ALL, PCIE_APPL_MSI_EN);
}

void dc_ep_icu_disable(struct dc_ep_priv *priv)
{
	/* Disable all PCIe related interrupts */
	wr32(0, PCIE_APPL_MSI_EN);

	wr32(PCI_MSI_8_MODE, RCU_MSI);

	/* Disable all interrupts in ICU level */
	wr32(0, ICU_DMA_TX_IMER);
	wr32(0, ICU_DMA_RX_IMER);
	wr32(0, ICU_IMER);
}

void dc_ep_icu_dis_intr(struct dc_ep_priv *priv, u32 bits)
{
	wr32_mask(~bits, 0, ICU_IMER);
}

void dc_ep_icu_en_intr(struct dc_ep_priv *priv, u32 bits)
{
	wr32_mask(0, bits, ICU_IMER);
}

void dc_ep_assert_device(struct dc_ep_priv *priv, u32 bits)
{
	struct dc_aca *aca = to_aca(priv);

	spin_lock(&aca->rcu_lock);
	wr32_mask(0, bits, RCU_REQ);
	spin_unlock(&aca->rcu_lock);
}

void dc_ep_deassert_device(struct dc_ep_priv *priv, u32 bits)
{
	struct dc_aca *aca = to_aca(priv);

	spin_lock(&aca->rcu_lock);
	wr32_mask(bits, 0, RCU_REQ);
	spin_unlock(&aca->rcu_lock);
}

int dc_ep_reset_device(struct dc_ep_priv *priv, u32 bits)
{
	int retry = EP_TIMEOUT;

	wr32(bits, RCU_REQ);
	do { } while (retry-- && (!(rd32(RCU_STAT) & bits)));

	if (retry == 0) {
		dev_err(priv->dev, "%s failed to reset\n", __func__);
		return -ETIME;
	}
	return 0;
}

int dc_ep_clk_on(struct dc_ep_priv *priv, u32 bits)
{
	int retry = EP_TIMEOUT;
	struct dc_aca *aca = to_aca(priv);

	spin_lock(&aca->clk_lock);
	wr32_mask(bits, 0, PMU_PWDCR);
	spin_unlock(&aca->clk_lock);

	do { } while (--retry && (rd32(PMU_SR) & bits));

	if (!retry) {
		dev_err(priv->dev, "%s failed\n", __func__);
		return -ETIME;
	}
	return 0;
}

int dc_ep_clk_off(struct dc_ep_priv *priv, u32 bits)
{
	int retry = EP_TIMEOUT;
	struct dc_aca *aca = to_aca(priv);

	spin_lock(&aca->clk_lock);
	wr32_mask(0, bits, PMU_PWDCR);
	spin_unlock(&aca->clk_lock);

	do {} while (--retry
		&& (!(rd32(PMU_SR) & bits)));
	if (!retry) {
		dev_err(priv->dev, "%s failed\n", __func__);
		return -ETIME;
	}
	return 0;
}

int dc_ep_clk_set(struct dc_ep_priv *priv, u32 sysclk, u32 ppeclk)
{
	struct dc_aca *aca = to_aca(priv);

	if (sysclk > SYS_CLK_MAX || ppeclk > PPE_CLK_MAX)
		return -EINVAL;

	spin_lock(&aca->clk_lock);
	wr32_mask(PPE_CLK | SYS_CLK,
		SM(sysclk, SYS_CLK) | SM(ppeclk, PPE_CLK), PLL_OMCFG);
	spin_unlock(&aca->clk_lock);
	return 0;
}

int dc_ep_clk_get(struct dc_ep_priv *priv, u32 *sysclk, u32 *ppeclk)
{
	u32 val;

	val = rd32(PLL_OMCFG);
	*sysclk = MS(val, SYS_CLK);
	*ppeclk = MS(val, PPE_CLK);
	return 0;
}

int dc_ep_gpio_dir(struct dc_ep_priv *priv, u32 gpio, int dir)
{
	struct dc_aca *aca = to_aca(priv);

	if (gpio > aca->max_gpio)
		return -EINVAL;

	if ((dir != GPIO_DIR_IN) && (dir != GPIO_DIR_OUT))
		return -EINVAL;

	if (dir == GPIO_DIR_IN)
		wr32(BIT(gpio), GPIO_DIRCLR);
	else
		wr32(BIT(gpio), GPIO_DIRSET);
	return 0;
}

int dc_ep_gpio_set(struct dc_ep_priv *priv, u32 gpio, int val)
{
	struct dc_aca *aca = to_aca(priv);

	if (gpio > aca->max_gpio)
		return -EINVAL;

	dc_ep_gpio_dir(priv, gpio, GPIO_DIR_OUT);

	if (val)
		wr32(BIT(gpio), GPIO_OUTSET);
	else
		wr32(BIT(gpio), GPIO_OUTCLR);
	return 0;
}

int dc_ep_gpio_get(struct dc_ep_priv *priv, u32 gpio, int *val)
{
	u32 dir;
	struct dc_aca *aca = to_aca(priv);

	if (gpio > aca->max_gpio)
		return -EINVAL;

	dir = rd32(GPIO_DIR);
	if ((dir >> gpio) & 0x1)
		*val = (rd32(GPIO_OUT) >> gpio) & 0x1;
	else
		*val = (rd32(GPIO_IN) >> gpio) & 0x1;
	return 0;
}

int dc_ep_pinmux_set(struct dc_ep_priv *priv, u32 gpio, int func)
{
	struct dc_aca *aca = to_aca(priv);

	if (gpio > aca->max_gpio)
		return -EINVAL;

	if (func >= MUX_FUNC_RES)
		return -EINVAL;

	mutex_lock(&aca->pin_lock);
	wr32_mask(PADC_MUX_M, func, PADC_MUX(gpio));
	mutex_unlock(&aca->pin_lock);
	return 0;
}

int dc_ep_pinmux_get(struct dc_ep_priv *priv, u32 gpio, int *func)
{
	struct dc_aca *aca = to_aca(priv);

	if (gpio > aca->max_gpio)
		return -EINVAL;

	*func = rd32(PADC_MUX(gpio));
	return 0;
}

int dc_ep_gpio_pupd_set(struct dc_ep_priv *priv, u32 gpio, u32 val)
{
	struct dc_aca *aca = to_aca(priv);

	if (gpio > aca->max_gpio)
		return -EINVAL;

	/* Not support for both enabled */
	if (val >= GPIO_PUPD_BOTH)
		return -EINVAL;

	mutex_lock(&aca->pin_lock);
	switch (val) {
	case GPIO_PUPD_DISABLE:
		padc_clearbit(gpio, PADC_PUEN);
		padc_clearbit(gpio, PADC_PDEN);
		break;
	case GPIO_PULL_UP:
		padc_setbit(gpio, PADC_PUEN);
		padc_clearbit(gpio, PADC_PDEN);
		break;
	case GPIO_PULL_DOWN:
		padc_setbit(gpio, PADC_PDEN);
		padc_clearbit(gpio, PADC_PUEN);
		break;
	default:
		break;
	}
	mutex_unlock(&aca->pin_lock);
	return 0;
}

int dc_ep_gpio_od_set(struct dc_ep_priv *priv, u32 gpio, int val)
{
	struct dc_aca *aca = to_aca(priv);

	if (gpio > aca->max_gpio)
		return -EINVAL;

	mutex_lock(&aca->pin_lock);
	if (!!val)
		padc_setbit(gpio, PADC_OD);
	else
		padc_clearbit(gpio, PADC_OD);
	mutex_unlock(&aca->pin_lock);
	return 0;
}

int dc_ep_gpio_src_set(struct dc_ep_priv *priv, u32 gpio, int val)
{
	struct dc_aca *aca = to_aca(priv);

	if (gpio > aca->max_gpio)
		return -EINVAL;

	mutex_lock(&aca->pin_lock);
	if (!!val)
		padc_setbit(gpio, PADC_SRC);
	else
		padc_clearbit(gpio, PADC_SRC);
	mutex_unlock(&aca->pin_lock);
	return 0;
}

int dc_ep_gpio_dcc_set(struct dc_ep_priv *priv, u32 gpio, u32 val)
{
	struct dc_aca *aca = to_aca(priv);

	if (gpio > aca->max_gpio)
		return -EINVAL;

	if (val >= GPIO_DRV_CUR_MAX)
		return -EINVAL;

	mutex_lock(&aca->pin_lock);
	wr32_mask((0x3 << (gpio * 2)), (val << (gpio * 2)), PADC_DCC);
	mutex_unlock(&aca->pin_lock);
	return 0;
}
