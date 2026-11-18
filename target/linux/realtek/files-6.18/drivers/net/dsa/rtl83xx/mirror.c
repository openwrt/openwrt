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

int otto_mirror_838x_get_config(struct otto_mirror_config *config, int group, int port)
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

int otto_mirror_839x_get_config(struct otto_mirror_config *config, int group, int port)
{
	config->ctrl = RTL839X_MIR_CTRL + group * 4;
	config->spm = RTL839X_MIR_SPM_CTRL + group * 8;
	config->dpm = RTL839X_MIR_DPM_CTRL + group * 8;

	/* Enable mirroring to destination port */
	config->val = BIT(0);
	config->val |= port << 4;

	return 0;
}

int otto_mirror_930x_get_config(struct otto_mirror_config *config, int group, int port)
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

int otto_mirror_931x_get_config(struct otto_mirror_config *config, int group, int port)
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
static int otto_mirror_group_find(struct otto_dsa_priv *priv, int port)
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
static void otto_mirror_group_release(struct otto_dsa_priv *priv, int group,
				      struct otto_mirror_config *config)
{
	u32 val;

	if (priv->mirror_group_flower[group])
		return;

	for (int i = 0; i < DIV_ROUND_UP(priv->r->cpu_port + 1, 32); i++) {
		if (regmap_read(priv->map, config->spm + i * 4, &val) || val)
			return;
		if (regmap_read(priv->map, config->dpm + i * 4, &val) || val)
			return;
	}

	priv->mirror_group_ports[group] = -1;
	regmap_write(priv->map, config->ctrl, 0);
}

int otto_mirror_port_add(struct dsa_switch *ds, int port,
			 struct dsa_mall_mirror_tc_entry *mirror,
			 bool ingress, struct netlink_ext_ack *extack)
{
	/* We support 4 mirror groups, one destination port per group */
	struct otto_dsa_priv *priv = ds->priv;
	struct otto_mirror_config config;
	u32 pm_bit = BIT(port % 32), val;
	int pm_offset, pm_reg, group, r;

	if (!priv->r->get_mirror_config)
		return -EOPNOTSUPP;

	pm_offset = (priv->r->cpu_port >= 32 && port < 32) ? 4 : 0;
	scoped_guard(mutex, &priv->reg_mutex) {
		group = otto_mirror_group_find(priv, mirror->to_local_port);
		if (group < 0)
			return group;

		r = priv->r->get_mirror_config(&config, group, mirror->to_local_port);
		if (r < 0)
			return r;

		regmap_write(priv->map, config.ctrl, config.val);

		pm_reg = ingress ? config.spm : config.dpm;
		r = regmap_read(priv->map, pm_reg + pm_offset, &val);
		if (r)
			return r;
		if (val & pm_bit)
			return -EEXIST;

		regmap_set_bits(priv->map, pm_reg + pm_offset, pm_bit);
		priv->mirror_group_ports[group] = mirror->to_local_port;
	}

	return 0;
}

void otto_mirror_port_del(struct dsa_switch *ds, int port,
			  struct dsa_mall_mirror_tc_entry *mirror)
{
	struct otto_dsa_priv *priv = ds->priv;
	struct otto_mirror_config config;
	int pm_offset, pm_reg, group, r;
	u32 pm_bit = BIT(port % 32);

	if (!priv->r->get_mirror_config)
		return;

	pm_offset = (priv->r->cpu_port >= 32 && port < 32) ? 4 : 0;
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

		pm_reg = mirror->ingress ? config.spm : config.dpm;
		regmap_clear_bits(priv->map, pm_reg + pm_offset, pm_bit);

		otto_mirror_group_release(priv, group, &config);
	}
}

/* A flower rule mirrors through the group of its mirror port: the ACL mirror
 * action picks the frames, the group's port masks stay as they are.
 */
int otto_mirror_group_get(struct otto_dsa_priv *priv, int port)
{
	struct otto_mirror_config config;
	int group, r;

	if (!priv->r->get_mirror_config)
		return -EOPNOTSUPP;

	scoped_guard(mutex, &priv->reg_mutex) {
		group = otto_mirror_group_find(priv, port);
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

void otto_mirror_group_put(struct otto_dsa_priv *priv, int group)
{
	struct otto_mirror_config config;

	scoped_guard(mutex, &priv->reg_mutex) {
		priv->mirror_group_flower[group]--;
		if (!priv->r->get_mirror_config(&config, group, priv->mirror_group_ports[group]))
			otto_mirror_group_release(priv, group, &config);
	}
}
