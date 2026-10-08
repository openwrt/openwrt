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

#ifndef MISC_H
#define MISC_H

#define EP_TIMEOUT	10000

void dc_ep_clkod_disable(struct dc_ep_priv *priv);
void dc_ep_icu_init(struct dc_ep_priv *priv);
void dc_ep_icu_disable(struct dc_ep_priv *priv);
void dc_ep_assert_device(struct dc_ep_priv *priv, u32 bits);
void dc_ep_deassert_device(struct dc_ep_priv *priv, u32 bits);
int dc_ep_reset_device(struct dc_ep_priv *priv, u32 bits);
int dc_ep_clk_on(struct dc_ep_priv *priv, u32 bits);
int dc_ep_clk_off(struct dc_ep_priv *priv, u32 bits);
int dc_ep_clk_set(struct dc_ep_priv *priv, u32 sysclk, u32 ppeclk);
int dc_ep_clk_get(struct dc_ep_priv *priv, u32 *sysclk, u32 *ppeclk);
int dc_ep_gpio_dir(struct dc_ep_priv *priv, u32 gpio, int dir);
int dc_ep_gpio_set(struct dc_ep_priv *priv, u32 gpio, int val);
int dc_ep_gpio_get(struct dc_ep_priv *priv, u32 gpio, int *val);
int dc_ep_pinmux_set(struct dc_ep_priv *priv, u32 gpio, int func);
int dc_ep_pinmux_get(struct dc_ep_priv *priv, u32 gpio, int *func);
int dc_ep_gpio_pupd_set(struct dc_ep_priv *priv, u32 gpio, u32 val);
int dc_ep_gpio_od_set(struct dc_ep_priv *priv, u32 gpio, int val);
int dc_ep_gpio_src_set(struct dc_ep_priv *priv, u32 gpio, int val);
int dc_ep_gpio_dcc_set(struct dc_ep_priv *priv, u32 gpio, u32 val);
void dc_ep_icu_dis_intr(struct dc_ep_priv *priv, u32 bits);
void dc_ep_icu_en_intr(struct dc_ep_priv *priv, u32 bits);

#endif /* MISC_H */
