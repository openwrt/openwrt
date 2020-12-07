// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl838x/mach-rtl83xx.h>
#include "rtl83xx.h"


static void rtl83xx_storm_enable(struct rtl838x_switch_priv *priv, int port, bool enable)
{
	// Enable Storm control for that port for UC, MC, and BC
	if (enable)
		sw_w32(0x7, RTL838X_STORM_CTRL_LB_CTRL(port));
	else
		sw_w32(0x0, RTL838X_STORM_CTRL_LB_CTRL(port));
}

void __init rtl83xx_storm_control_init(struct rtl838x_switch_priv *priv)
{
	int i;

	pr_debug("Enabling Storm control\n");
	// TICK_PERIOD_PPS
	if (priv->id == 0x8380)
		sw_w32_mask(0x3ff << 20, 434 << 20, RTL838X_SCHED_LB_TICK_TKN_CTRL_0);

	// Set burst rate
	sw_w32(0x00008000, RTL838X_STORM_CTRL_BURST_0); // UC
	sw_w32(0x80008000, RTL838X_STORM_CTRL_BURST_1); // MC and BC

	// Set burst Packets per Second to 32
	sw_w32(0x00000020, RTL838X_STORM_CTRL_BURST_PPS_0); // UC
	sw_w32(0x00200020, RTL838X_STORM_CTRL_BURST_PPS_1); // MC and BC

	// Include IFG in storm control
	sw_w32_mask(0, BIT(6), RTL838X_STORM_CTRL);
	// Rate control is based on bytes/s (0 = packets)
	sw_w32_mask(0, BIT(5), RTL838X_STORM_CTRL);
	// Bandwidth control includes preamble and IFG (10 Bytes)
	sw_w32_mask(0, 1, RTL838X_SCHED_CTRL);

	// On SoCs except RTL8382M, set burst size of port egress
	if (priv->id != 0x8382)
		sw_w32_mask(0xffff, 0x800, RTL838X_SCHED_LB_THR);

	/* Enable storm control on all ports with a PHY and limit rates,
	 * for UC and MC for both known and unknown addresses */
	for (i = 0; i < priv->cpu_port; i++) {
		if (priv->ports[i].phy) {
			sw_w32(BIT(18) | 0x8000, RTL838X_STORM_CTRL_PORT_UC(i));
			sw_w32(BIT(18) | 0x8000, RTL838X_STORM_CTRL_PORT_MC(i));
			sw_w32(0x000, RTL838X_STORM_CTRL_PORT_BC(i));
			rtl83xx_storm_enable(priv, i, true);
		}
	}

	// Attack prevention, enable all attack prevention measures
	//sw_w32(0x1ffff, RTL838X_ATK_PRVNT_CTRL);
	/* Attack prevention, drop (bit = 0) problematic packets on all ports.
	 * Setting bit = 1 means: trap to CPU
	 */
	//sw_w32(0, RTL838X_ATK_PRVNT_ACT);
	// Enable attack prevention on all ports
	//sw_w32(0x0fffffff, RTL838X_ATK_PRVNT_PORT_EN);
}

