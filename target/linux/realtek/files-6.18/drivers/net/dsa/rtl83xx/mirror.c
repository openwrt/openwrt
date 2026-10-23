// SPDX-License-Identifier: GPL-2.0-only

#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/regmap.h>
#include <net/dsa.h>

#include "mirror.h"
#include "rtl-otto.h"

#define RTL930X_MIR_CTRL			(0xA2A0)
#define RTL930X_MIR_DPM_CTRL			(0xA2C0)
#define RTL930X_MIR_SPM_CTRL			(0xA2B0)

#define RTL931X_MIR_CTRL			(0xAF00)
#define RTL931X_MIR_DPM_CTRL			(0xAF30)
#define RTL931X_MIR_SPM_CTRL			(0xAF10)

int rtldsa_838x_get_mirror_config(struct rtldsa_mirror_config *config, int group, int port)
{
	config->ctrl = RTL838X_MIR_CTRL + group * 4;
	config->spm = RTL838X_MIR_SPM_CTRL + group * 4;
	config->dpm = RTL838X_MIR_DPM_CTRL + group * 4;

	/* Enable mirroring to destination port */
	config->val = BIT(0);
	config->val |= port << 4;

	/* Enable mirroring to port across VLANs */
	config->val |= BIT(11);

	return 0;
}

int rtldsa_839x_get_mirror_config(struct rtldsa_mirror_config *config, int group, int port)
{
	config->ctrl = RTL839X_MIR_CTRL + group * 4;
	config->spm = RTL839X_MIR_SPM_CTRL + group * 8;
	config->dpm = RTL839X_MIR_DPM_CTRL + group * 8;

	/* Enable mirroring to destination port */
	config->val = BIT(0);
	config->val |= port << 4;

	return 0;
}

int rtldsa_930x_get_mirror_config(struct rtldsa_mirror_config *config, int group, int port)
{
	config->ctrl = RTL930X_MIR_CTRL + group * 4;
	config->spm = RTL930X_MIR_SPM_CTRL + group * 4;
	config->dpm = RTL930X_MIR_DPM_CTRL + group * 4;

	/* Enable mirroring to destination port */
	config->val = BIT(0);
	config->val |= port << 9;

	/* mirror mode: let mirrored packets follow TX settings of
	 * mirroring port
	 */
	config->val |= BIT(5);

	/* direction of traffic to be mirrored when a packet
	 * hits both SPM and DPM ports: prefer egress
	 */
	config->val |= BIT(4);

	return 0;
}

int rtldsa_931x_get_mirror_config(struct rtldsa_mirror_config *config, int group, int port)
{
	config->ctrl = RTL931X_MIR_CTRL + group * 4;
	config->spm = RTL931X_MIR_SPM_CTRL + group * 8;
	config->dpm = RTL931X_MIR_DPM_CTRL + group * 8;

	/* Enable mirroring to destination port */
	config->val = BIT(0);
	config->val |= port << 9;

	/* mirror mode: let mirrored packets follow TX settings of
	 * mirroring port
	 */
	config->val |= BIT(5);

	/* direction of traffic to be mirrored when a packet
	 * hits both SPM and DPM ports: prefer egress
	 */
	config->val |= BIT(4);

	return 0;
}

/* Caller must hold priv->reg_mutex */
static int rtldsa_mirror_group_find(struct otto_dsa_priv *priv, int port)
{
	int group;

	for (group = 0; group < 4; group++)
		if (priv->mirror_group_ports[group] == port)
			return group;
	for (group = 0; group < 4; group++)
		if (priv->mirror_group_ports[group] < 0)
			return group;

	return -ENOSPC;
}

/* Caller must hold priv->reg_mutex */
static void rtldsa_mirror_group_release(struct otto_dsa_priv *priv, int group,
					struct rtldsa_mirror_config *config)
{
	if (priv->mirror_group_flower[group] ||
	    priv->r->get_port_reg_be(config->spm) ||
	    priv->r->get_port_reg_be(config->dpm))
		return;

	priv->mirror_group_ports[group] = -1;
	regmap_write(priv->map, config->ctrl, 0);
}

int rtldsa_port_mirror_add(struct dsa_switch *ds, int port,
			   struct dsa_mall_mirror_tc_entry *mirror,
			   bool ingress, struct netlink_ext_ack *extack)
{
	/* We support 4 mirror groups, one destination port per group */
	struct otto_dsa_priv *priv = ds->priv;
	struct rtldsa_mirror_config config;
	int pm_reg;
	int group;
	int r;

	if (!priv->r->get_mirror_config)
		return -EOPNOTSUPP;

	pr_debug("In %s\n", __func__);

	scoped_guard(mutex, &priv->reg_mutex) {
		group = rtldsa_mirror_group_find(priv, mirror->to_local_port);
		if (group < 0)
			return group;

		pr_debug("Using group %d\n", group);

		r = priv->r->get_mirror_config(&config, group, mirror->to_local_port);
		if (r < 0)
			return r;

		if (ingress)
			pm_reg = config.spm;
		else
			pm_reg = config.dpm;

		regmap_write(priv->map, config.ctrl, config.val);

		if (priv->r->get_port_reg_be(pm_reg) & (1ULL << port))
			return -EEXIST;

		priv->r->mask_port_reg_be(0, 1ULL << port, pm_reg);
		priv->mirror_group_ports[group] = mirror->to_local_port;
	}

	return 0;
}

void rtldsa_port_mirror_del(struct dsa_switch *ds, int port,
			    struct dsa_mall_mirror_tc_entry *mirror)
{
	struct otto_dsa_priv *priv = ds->priv;
	struct rtldsa_mirror_config config;
	int group = 0;
	int r;

	if (!priv->r->get_mirror_config)
		return;

	pr_debug("In %s\n", __func__);

	scoped_guard(mutex, &priv->reg_mutex) {
		for (group = 0; group < 4; group++) {
			if (priv->mirror_group_ports[group] == mirror->to_local_port)
				break;
		}
		if (group >= 4)
			return;

		r = priv->r->get_mirror_config(&config, group, mirror->to_local_port);
		if (r < 0)
			return;

		if (mirror->ingress) {
			/* Ingress, clear source port matrix */
			priv->r->mask_port_reg_be(1ULL << port, 0, config.spm);
		} else {
			/* Egress, clear destination port matrix */
			priv->r->mask_port_reg_be(1ULL << port, 0, config.dpm);
		}

		rtldsa_mirror_group_release(priv, group, &config);
	}
}

/* A flower rule mirrors through the group of its mirror port: the ACL mirror
 * action picks the frames, the group's port masks stay as they are.
 */
int rtldsa_mirror_group_get(struct otto_dsa_priv *priv, int port)
{
	struct rtldsa_mirror_config config;
	int group, r;

	if (!priv->r->get_mirror_config)
		return -EOPNOTSUPP;

	scoped_guard(mutex, &priv->reg_mutex) {
		group = rtldsa_mirror_group_find(priv, port);
		if (group < 0)
			return group;

		r = priv->r->get_mirror_config(&config, group, port);
		if (r < 0)
			return r;

		regmap_write(priv->map, config.ctrl, config.val);
		priv->mirror_group_ports[group] = port;
		priv->mirror_group_flower[group]++;
	}

	return group;
}

void rtldsa_mirror_group_put(struct otto_dsa_priv *priv, int group)
{
	struct rtldsa_mirror_config config;

	scoped_guard(mutex, &priv->reg_mutex) {
		priv->mirror_group_flower[group]--;
		if (!priv->r->get_mirror_config(&config, group, priv->mirror_group_ports[group]))
			rtldsa_mirror_group_release(priv, group, &config);
	}
}
