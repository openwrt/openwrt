// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 1999 - 2020 Intel Corporation. */

/*
 * net/core/ethtool.c - Ethtool ioctl handler
 * Copyright (c) 2003 Matthew Wilcox <matthew@wil.cx>
 *
 * This file is where we call all the ethtool_ops commands to get
 * the information ethtool needs.  We fall back to calling do_ioctl()
 * for drivers which haven't been converted to ethtool_ops yet.
 *
 * It's GPL, stupid.
 *
 * Modification by sfeldma@pobox.com to work as backward compat
 * solution for pre-ethtool_ops kernels.
 * 	- copied struct ethtool_ops from ethtool.h
 * 	- defined SET_ETHTOOL_OPS
 * 	- put in some #ifndef NETIF_F_xxx wrappers
 * 	- changes refs to dev->ethtool_ops to ethtool_ops
 * 	- changed dev_ethtool to ethtool_ioctl
 *      - remove EXPORT_SYMBOL()s
 *      - added _kc_ prefix in built-in ethtool_op_xxx ops.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/netdevice.h>
#include <asm/uaccess.h>

#include "kcompat.h"

#undef SUPPORTED_10000baseT_Full
#define SUPPORTED_10000baseT_Full	(1 << 12)
#undef ADVERTISED_10000baseT_Full
#define ADVERTISED_10000baseT_Full	(1 << 12)
#undef SPEED_10000

#undef ethtool_ops
#define ethtool_ops _kc_ethtool_ops

struct _kc_ethtool_ops {
	int (*get_settings) (struct net_device *, struct ethtool_cmd *);
	int (*set_settings) (struct net_device *, struct ethtool_cmd *);
	void (*get_drvinfo) (struct net_device *, struct ethtool_drvinfo *);
	int (*get_regs_len) (struct net_device *);
	void (*get_regs) (struct net_device *, struct ethtool_regs *, void *);
	void (*get_wol) (struct net_device *, struct ethtool_wolinfo *);
	int (*set_wol) (struct net_device *, struct ethtool_wolinfo *);
	u32 (*get_msglevel) (struct net_device *);
	void (*set_msglevel) (struct net_device *, u32);
	int (*nway_reset) (struct net_device *);
	u32 (*get_link) (struct net_device *);
	int (*get_eeprom_len) (struct net_device *);
	int (*get_eeprom) (struct net_device *, struct ethtool_eeprom *, u8 *);
	int (*set_eeprom) (struct net_device *, struct ethtool_eeprom *, u8 *);
	int (*get_coalesce) (struct net_device *, struct ethtool_coalesce *);
	int (*set_coalesce) (struct net_device *, struct ethtool_coalesce *);
	void (*get_ringparam) (struct net_device *, struct ethtool_ringparam *);
	int (*set_ringparam) (struct net_device *, struct ethtool_ringparam *);
	void (*get_pauseparam) (struct net_device *,
				struct ethtool_pauseparam *);
	int (*set_pauseparam) (struct net_device *,
			       struct ethtool_pauseparam *);
	u32 (*get_rx_csum) (struct net_device *);
	int (*set_rx_csum) (struct net_device *, u32);
	u32 (*get_tx_csum) (struct net_device *);
	int (*set_tx_csum) (struct net_device *, u32);
	u32 (*get_sg) (struct net_device *);
	int (*set_sg) (struct net_device *, u32);
	u32 (*get_tso) (struct net_device *);
	int (*set_tso) (struct net_device *, u32);
	int (*self_test_count) (struct net_device *);
	void (*self_test) (struct net_device *, struct ethtool_test *, u64 *);
	void (*get_strings) (struct net_device *, u32 stringset, u8 *);
	int (*phys_id) (struct net_device *, u32);
	int (*get_stats_count) (struct net_device *);
	void (*get_ethtool_stats) (struct net_device *, struct ethtool_stats *,
				   u64 *);
} *ethtool_ops = NULL;

#undef SET_ETHTOOL_OPS
#define SET_ETHTOOL_OPS(netdev, ops) (ethtool_ops = (ops))

/*
 * Some useful ethtool_ops methods that are device independent. If we find that
 * all drivers want to do the same thing here, we can turn these into dev_()
 * function calls.
 */

#undef ethtool_op_get_link
#define ethtool_op_get_link _kc_ethtool_op_get_link
u32 _kc_ethtool_op_get_link(struct net_device *dev)
{
	return netif_carrier_ok(dev) ? 1 : 0;
}

#undef ethtool_op_get_tx_csum
#define ethtool_op_get_tx_csum _kc_ethtool_op_get_tx_csum
u32 _kc_ethtool_op_get_tx_csum(struct net_device *dev)
{
#ifdef NETIF_F_IP_CSUM
	return (dev->features & NETIF_F_IP_CSUM) != 0;
#else
	return 0;
#endif
}

#undef ethtool_op_set_tx_csum
#define ethtool_op_set_tx_csum _kc_ethtool_op_set_tx_csum
int _kc_ethtool_op_set_tx_csum(struct net_device *dev, u32 data)
{
#ifdef NETIF_F_IP_CSUM
	if (data)
#ifdef NETIF_F_IPV6_CSUM
		dev->features |= (NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM);
	else
		dev->features &= ~(NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM);
#else
		dev->features |= NETIF_F_IP_CSUM;
	else
		dev->features &= ~NETIF_F_IP_CSUM;
#endif
#endif

	return 0;
}

#undef ethtool_op_get_sg
#define ethtool_op_get_sg _kc_ethtool_op_get_sg
u32 _kc_ethtool_op_get_sg(struct net_device *dev)
{
#ifdef NETIF_F_SG
	return (dev->features & NETIF_F_SG) != 0;
#else
	return 0;
#endif
}

#undef ethtool_op_set_sg
#define ethtool_op_set_sg _kc_ethtool_op_set_sg
int _kc_ethtool_op_set_sg(struct net_device *dev, u32 data)
{
#ifdef NETIF_F_SG
	if (data)
		dev->features |= NETIF_F_SG;
	else
		dev->features &= ~NETIF_F_SG;
#endif

	return 0;
}

#undef ethtool_op_get_tso
#define ethtool_op_get_tso _kc_ethtool_op_get_tso
u32 _kc_ethtool_op_get_tso(struct net_device *dev)
{
#ifdef NETIF_F_TSO
	return (dev->features & NETIF_F_TSO) != 0;
#else
	return 0;
#endif
}

#undef ethtool_op_set_tso
#define ethtool_op_set_tso _kc_ethtool_op_set_tso
int _kc_ethtool_op_set_tso(struct net_device *dev, u32 data)
{
#ifdef NETIF_F_TSO
	if (data)
		dev->features |= NETIF_F_TSO;
	else
		dev->features &= ~NETIF_F_TSO;
#endif

	return 0;
}

/* Handlers for each ethtool command */

static int ethtool_get_settings(struct net_device *dev, void *useraddr)
{
	struct ethtool_cmd cmd = { ETHTOOL_GSET };
	int err;

	if (!ethtool_ops->get_settings)
		return -EOPNOTSUPP;

	err = ethtool_ops->get_settings(dev, &cmd);
	if (err < 0)
		return err;

	if (copy_to_user(useraddr, &cmd, sizeof(cmd)))
		return -EFAULT;
	return 0;
}

static int ethtool_set_settings(struct net_device *dev, void *useraddr)
{
	struct ethtool_cmd cmd;

	if (!ethtool_ops->set_settings)
		return -EOPNOTSUPP;

	if (copy_from_user(&cmd, useraddr, sizeof(cmd)))
		return -EFAULT;

	return ethtool_ops->set_settings(dev, &cmd);
}

static int ethtool_get_drvinfo(struct net_device *dev, void *useraddr)
{
	struct ethtool_drvinfo info;
	struct ethtool_ops *ops = ethtool_ops;

	if (!ops->get_drvinfo)
		return -EOPNOTSUPP;

	memset(&info, 0, sizeof(info));
	info.cmd = ETHTOOL_GDRVINFO;
	ops->get_drvinfo(dev, &info);

	if (ops->self_test_count)
		info.testinfo_len = ops->self_test_count(dev);
	if (ops->get_stats_count)
		info.n_stats = ops->get_stats_count(dev);
	if (ops->get_regs_len)
		info.regdump_len = ops->get_regs_len(dev);
	if (ops->get_eeprom_len)
		info.eedump_len = ops->get_eeprom_len(dev);

	if (copy_to_user(useraddr, &info, sizeof(info)))
		return -EFAULT;
	return 0;
}

static int ethtool_get_regs(struct net_device *dev, char *useraddr)
{
	struct ethtool_regs regs;
	struct ethtool_ops *ops = ethtool_ops;
	void *regbuf;
	int reglen, ret;

	if (!ops->get_regs || !ops->get_regs_len)
		return -EOPNOTSUPP;

	if (copy_from_user(&regs, useraddr, sizeof(regs)))
		return -EFAULT;

	reglen = ops->get_regs_len(dev);
	if (regs.len > reglen)
		regs.len = reglen;

	regbuf = kmalloc(reglen, GFP_USER);
	if (!regbuf)
		return -ENOMEM;

	ops->get_regs(dev, &regs, regbuf);

	ret = -EFAULT;
	if (copy_to_user(useraddr, &regs, sizeof(regs)))
		goto out;
	useraddr += offsetof(struct ethtool_regs, data);
	if (copy_to_user(useraddr, regbuf, reglen))
		goto out;
	ret = 0;

out:
	kfree(regbuf);
	return ret;
}

static int ethtool_get_wol(struct net_device *dev, char *useraddr)
{
	struct ethtool_wolinfo wol = { ETHTOOL_GWOL };

	if (!ethtool_ops->get_wol)
		return -EOPNOTSUPP;

	ethtool_ops->get_wol(dev, &wol);

	if (copy_to_user(useraddr, &wol, sizeof(wol)))
		return -EFAULT;
	return 0;
}

static int ethtool_set_wol(struct net_device *dev, char *useraddr)
{
	struct ethtool_wolinfo wol;

	if (!ethtool_ops->set_wol)
		return -EOPNOTSUPP;

	if (copy_from_user(&wol, useraddr, sizeof(wol)))
		return -EFAULT;

	return ethtool_ops->set_wol(dev, &wol);
}

static int ethtool_get_msglevel(struct net_device *dev, char *useraddr)
{
	struct ethtool_value edata = { ETHTOOL_GMSGLVL };

	if (!ethtool_ops->get_msglevel)
		return -EOPNOTSUPP;

	edata.data = ethtool_ops->get_msglevel(dev);

	if (copy_to_user(useraddr, &edata, sizeof(edata)))
		return -EFAULT;
	return 0;
}

static int ethtool_set_msglevel(struct net_device *dev, char *useraddr)
{
	struct ethtool_value edata;

	if (!ethtool_ops->set_msglevel)
		return -EOPNOTSUPP;

	if (copy_from_user(&edata, useraddr, sizeof(edata)))
		return -EFAULT;

	ethtool_ops->set_msglevel(dev, edata.data);
	return 0;
}

static int ethtool_nway_reset(struct net_device *dev)
{
	if (!ethtool_ops->nway_reset)
		return -EOPNOTSUPP;

	return ethtool_ops->nway_reset(dev);
}

static int ethtool_get_link(struct net_device *dev, void *useraddr)
{
	struct ethtool_value edata = { ETHTOOL_GLINK };

	if (!ethtool_ops->get_link)
		return -EOPNOTSUPP;

	edata.data = ethtool_ops->get_link(dev);

	if (copy_to_user(useraddr, &edata, sizeof(edata)))
		return -EFAULT;
	return 0;
}

static int ethtool_get_eeprom(struct net_device *dev, void *useraddr)
{
	struct ethtool_eeprom eeprom;
	struct ethtool_ops *ops = ethtool_ops;
	u8 *data;
	int ret;

	if (!ops->get_eeprom || !ops->get_eeprom_len)
		return -EOPNOTSUPP;

	if (copy_from_user(&eeprom, useraddr, sizeof(eeprom)))
		return -EFAULT;

	/* Check for wrap and zero */
	if (eeprom.offset + eeprom.len <= eeprom.offset)
		return -EINVAL;

	/* Check for exceeding total eeprom len */
	if (eeprom.offset + eeprom.len > ops->get_eeprom_len(dev))
		return -EINVAL;

	data = kmalloc(eeprom.len, GFP_USER);
	if (!data)
		return -ENOMEM;

	ret = -EFAULT;
	if (copy_from_user(data, useraddr + sizeof(eeprom), eeprom.len))
		goto out;

	ret = ops->get_eeprom(dev, &eeprom, data);
	if (ret)
		goto out;

	ret = -EFAULT;
	if (copy_to_user(useraddr, &eeprom, sizeof(eeprom)))
		goto out;
	if (copy_to_user(useraddr + sizeof(eeprom), data, eeprom.len))
		goto out;
	ret = 0;

out:
	kfree(data);
	return ret;
}

static int ethtool_set_eeprom(struct net_device *dev, void *useraddr)
{
	struct ethtool_eeprom eeprom;
	struct ethtool_ops *ops = ethtool_ops;
	u8 *data;
	int ret;

	if (!ops->set_eeprom || !ops->get_eeprom_len)
		return -EOPNOTSUPP;

	if (copy_from_user(&eeprom, useraddr, sizeof(eeprom)))
		return -EFAULT;

	/* Check for wrap and zero */
	if (eeprom.offset + eeprom.len <= eeprom.offset)
		return -EINVAL;

	/* Check for exceeding total eeprom len */
	if (eeprom.offset + eeprom.len > ops->get_eeprom_len(dev))
		return -EINVAL;

	data = kmalloc(eeprom.len, GFP_USER);
	if (!data)
		return -ENOMEM;

	ret = -EFAULT;
	if (copy_from_user(data, useraddr + sizeof(eeprom), eeprom.len))
		goto out;

	ret = ops->set_eeprom(dev, &eeprom, data);
	if (ret)
		goto out;

	if (copy_to_user(useraddr + sizeof(eeprom), data, eeprom.len))
		ret = -EFAULT;

out:
	kfree(data);
	return ret;
}

static int ethtool_get_coalesce(struct net_device *dev, void *useraddr)
{
	struct ethtool_coalesce coalesce = { ETHTOOL_GCOALESCE };

	if (!ethtool_ops->get_coalesce)
		return -EOPNOTSUPP;

	ethtool_ops->get_coalesce(dev, &coalesce);

	if (copy_to_user(useraddr, &coalesce, sizeof(coalesce)))
		return -EFAULT;
	return 0;
}

static int ethtool_set_coalesce(struct net_device *dev, void *useraddr)
{
	struct ethtool_coalesce coalesce;

	if (!ethtool_ops->get_coalesce)
		return -EOPNOTSUPP;

	if (copy_from_user(&coalesce, useraddr, sizeof(coalesce)))
		return -EFAULT;

	return ethtool_ops->set_coalesce(dev, &coalesce);
}

static int ethtool_get_ringparam(struct net_device *dev, void *useraddr)
{
	struct ethtool_ringparam ringparam = { ETHTOOL_GRINGPARAM };

	if (!ethtool_ops->get_ringparam)
		return -EOPNOTSUPP;

	ethtool_ops->get_ringparam(dev, &ringparam);

	if (copy_to_user(useraddr, &ringparam, sizeof(ringparam)))
		return -EFAULT;
	return 0;
}

static int ethtool_set_ringparam(struct net_device *dev, void *useraddr)
{
	struct ethtool_ringparam ringparam;

	if (!ethtool_ops->get_ringparam)
		return -EOPNOTSUPP;

	if (copy_from_user(&ringparam, useraddr, sizeof(ringparam)))
		return -EFAULT;

	return ethtool_ops->set_ringparam(dev, &ringparam);
}

static int ethtool_get_pauseparam(struct net_device *dev, void *useraddr)
{
	struct ethtool_pauseparam pauseparam = { ETHTOOL_GPAUSEPARAM };

	if (!ethtool_ops->get_pauseparam)
		return -EOPNOTSUPP;

	ethtool_ops->get_pauseparam(dev, &pauseparam);

	if (copy_to_user(useraddr, &pauseparam, sizeof(pauseparam)))
		return -EFAULT;
	return 0;
}

static int ethtool_set_pauseparam(struct net_device *dev, void *useraddr)
{
	struct ethtool_pauseparam pauseparam;

	if (!ethtool_ops->get_pauseparam)
		return -EOPNOTSUPP;

	if (copy_from_user(&pauseparam, useraddr, sizeof(pauseparam)))
		return -EFAULT;

	return ethtool_ops->set_pauseparam(dev, &pauseparam);
}

static int ethtool_get_rx_csum(struct net_device *dev, char *useraddr)
{
	struct ethtool_value edata = { ETHTOOL_GRXCSUM };

	if (!ethtool_ops->get_rx_csum)
		return -EOPNOTSUPP;

	edata.data = ethtool_ops->get_rx_csum(dev);

	if (copy_to_user(useraddr, &edata, sizeof(edata)))
		return -EFAULT;
	return 0;
}

static int ethtool_set_rx_csum(struct net_device *dev, char *useraddr)
{
	struct ethtool_value edata;

	if (!ethtool_ops->set_rx_csum)
		return -EOPNOTSUPP;

	if (copy_from_user(&edata, useraddr, sizeof(edata)))
		return -EFAULT;

	ethtool_ops->set_rx_csum(dev, edata.data);
	return 0;
}

static int ethtool_get_tx_csum(struct net_device *dev, char *useraddr)
{
	struct ethtool_value edata = { ETHTOOL_GTXCSUM };

	if (!ethtool_ops->get_tx_csum)
		return -EOPNOTSUPP;

	edata.data = ethtool_ops->get_tx_csum(dev);

	if (copy_to_user(useraddr, &edata, sizeof(edata)))
		return -EFAULT;
	return 0;
}

static int ethtool_set_tx_csum(struct net_device *dev, char *useraddr)
{
	struct ethtool_value edata;

	if (!ethtool_ops->set_tx_csum)
		return -EOPNOTSUPP;

	if (copy_from_user(&edata, useraddr, sizeof(edata)))
		return -EFAULT;

	return ethtool_ops->set_tx_csum(dev, edata.data);
}

static int ethtool_get_sg(struct net_device *dev, char *useraddr)
{
	struct ethtool_value edata = { ETHTOOL_GSG };

	if (!ethtool_ops->get_sg)
		return -EOPNOTSUPP;

	edata.data = ethtool_ops->get_sg(dev);

	if (copy_to_user(useraddr, &edata, sizeof(edata)))
		return -EFAULT;
	return 0;
}

static int ethtool_set_sg(struct net_device *dev, char *useraddr)
{
	struct ethtool_value edata;

	if (!ethtool_ops->set_sg)
		return -EOPNOTSUPP;

	if (copy_from_user(&edata, useraddr, sizeof(edata)))
		return -EFAULT;

	return ethtool_ops->set_sg(dev, edata.data);
}

static int ethtool_get_tso(struct net_device *dev, char *useraddr)
{
	struct ethtool_value edata = { ETHTOOL_GTSO };

	if (!ethtool_ops->get_tso)
		return -EOPNOTSUPP;

	edata.data = ethtool_ops->get_tso(dev);

	if (copy_to_user(useraddr, &edata, sizeof(edata)))
		return -EFAULT;
	return 0;
}

static int ethtool_set_tso(struct net_device *dev, char *useraddr)
{
	struct ethtool_value edata;

	if (!ethtool_ops->set_tso)
		return -EOPNOTSUPP;

	if (copy_from_user(&edata, useraddr, sizeof(edata)))
		return -EFAULT;

	return ethtool_ops->set_tso(dev, edata.data);
}

static int ethtool_self_test(struct net_device *dev, char *useraddr)
{
	struct ethtool_test test;
	struct ethtool_ops *ops = ethtool_ops;
	u64 *data;
	int ret;

	if (!ops->self_test || !ops->self_test_count)
		return -EOPNOTSUPP;

	if (copy_from_user(&test, useraddr, sizeof(test)))
		return -EFAULT;

	test.len = ops->self_test_count(dev);
	data = kmalloc(test.len * sizeof(u64), GFP_USER);
	if (!data)
		return -ENOMEM;

	ops->self_test(dev, &test, data);

	ret = -EFAULT;
	if (copy_to_user(useraddr, &test, sizeof(test)))
		goto out;
	useraddr += sizeof(test);
	if (copy_to_user(useraddr, data, test.len * sizeof(u64)))
		goto out;
	ret = 0;

out:
	kfree(data);
	return ret;
}

static int ethtool_get_strings(struct net_device *dev, void *useraddr)
{
	struct ethtool_gstrings gstrings;
	struct ethtool_ops *ops = ethtool_ops;
	u8 *data;
	int ret;

	if (!ops->get_strings)
		return -EOPNOTSUPP;

	if (copy_from_user(&gstrings, useraddr, sizeof(gstrings)))
		return -EFAULT;

	switch (gstrings.string_set) {
	case ETH_SS_TEST:
		if (!ops->self_test_count)
			return -EOPNOTSUPP;
		gstrings.len = ops->self_test_count(dev);
		break;
	case ETH_SS_STATS:
		if (!ops->get_stats_count)
			return -EOPNOTSUPP;
		gstrings.len = ops->get_stats_count(dev);
		break;
	default:
		return -EINVAL;
	}

	data = kmalloc(gstrings.len * ETH_GSTRING_LEN, GFP_USER);
	if (!data)
		return -ENOMEM;

	ops->get_strings(dev, gstrings.string_set, data);

	ret = -EFAULT;
	if (copy_to_user(useraddr, &gstrings, sizeof(gstrings)))
		goto out;
	useraddr += sizeof(gstrings);
	if (copy_to_user(useraddr, data, gstrings.len * ETH_GSTRING_LEN))
		goto out;
	ret = 0;

out:
	kfree(data);
	return ret;
}

static int ethtool_phys_id(struct net_device *dev, void *useraddr)
{
	struct ethtool_value id;

	if (!ethtool_ops->phys_id)
		return -EOPNOTSUPP;

	if (copy_from_user(&id, useraddr, sizeof(id)))
		return -EFAULT;

	return ethtool_ops->phys_id(dev, id.data);
}

static int ethtool_get_stats(struct net_device *dev, void *useraddr)
{
	struct ethtool_stats stats;
	struct ethtool_ops *ops = ethtool_ops;
	u64 *data;
	int ret;

	if (!ops->get_ethtool_stats || !ops->get_stats_count)
		return -EOPNOTSUPP;

	if (copy_from_user(&stats, useraddr, sizeof(stats)))
		return -EFAULT;

	stats.n_stats = ops->get_stats_count(dev);
	data = kmalloc(stats.n_stats * sizeof(u64), GFP_USER);
	if (!data)
		return -ENOMEM;

	ops->get_ethtool_stats(dev, &stats, data);

	ret = -EFAULT;
	if (copy_to_user(useraddr, &stats, sizeof(stats)))
		goto out;
	useraddr += sizeof(stats);
	if (copy_to_user(useraddr, data, stats.n_stats * sizeof(u64)))
		goto out;
	ret = 0;

out:
	kfree(data);
	return ret;
}

/* The main entry point in this file.  Called from net/core/dev.c */

#define ETHTOOL_OPS_COMPAT
int ethtool_ioctl(struct ifreq *ifr)
{
	struct net_device *dev = __dev_get_by_name(ifr->ifr_name);
	void *useraddr = (void *)ifr->ifr_data;
	u32 ethcmd;

	/*
	 * XXX: This can be pushed down into the ethtool_* handlers that
	 * need it.  Keep existing behavior for the moment.
	 */
	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (!dev || !netif_device_present(dev))
		return -ENODEV;

	if (copy_from_user(&ethcmd, useraddr, sizeof(ethcmd)))
		return -EFAULT;

	switch (ethcmd) {
	case ETHTOOL_GSET:
		return ethtool_get_settings(dev, useraddr);
	case ETHTOOL_SSET:
		return ethtool_set_settings(dev, useraddr);
	case ETHTOOL_GDRVINFO:
		return ethtool_get_drvinfo(dev, useraddr);
	case ETHTOOL_GREGS:
		return ethtool_get_regs(dev, useraddr);
	case ETHTOOL_GWOL:
		return ethtool_get_wol(dev, useraddr);
	case ETHTOOL_SWOL:
		return ethtool_set_wol(dev, useraddr);
	case ETHTOOL_GMSGLVL:
		return ethtool_get_msglevel(dev, useraddr);
	case ETHTOOL_SMSGLVL:
		return ethtool_set_msglevel(dev, useraddr);
	case ETHTOOL_NWAY_RST:
		return ethtool_nway_reset(dev);
	case ETHTOOL_GLINK:
		return ethtool_get_link(dev, useraddr);
	case ETHTOOL_GEEPROM:
		return ethtool_get_eeprom(dev, useraddr);
	case ETHTOOL_SEEPROM:
		return ethtool_set_eeprom(dev, useraddr);
	case ETHTOOL_GCOALESCE:
		return ethtool_get_coalesce(dev, useraddr);
	case ETHTOOL_SCOALESCE:
		return ethtool_set_coalesce(dev, useraddr);
	case ETHTOOL_GRINGPARAM:
		return ethtool_get_ringparam(dev, useraddr);
	case ETHTOOL_SRINGPARAM:
		return ethtool_set_ringparam(dev, useraddr);
	case ETHTOOL_GPAUSEPARAM:
		return ethtool_get_pauseparam(dev, useraddr);
	case ETHTOOL_SPAUSEPARAM:
		return ethtool_set_pauseparam(dev, useraddr);
	case ETHTOOL_GRXCSUM:
		return ethtool_get_rx_csum(dev, useraddr);
	case ETHTOOL_SRXCSUM:
		return ethtool_set_rx_csum(dev, useraddr);
	case ETHTOOL_GTXCSUM:
		return ethtool_get_tx_csum(dev, useraddr);
	case ETHTOOL_STXCSUM:
		return ethtool_set_tx_csum(dev, useraddr);
	case ETHTOOL_GSG:
		return ethtool_get_sg(dev, useraddr);
	case ETHTOOL_SSG:
		return ethtool_set_sg(dev, useraddr);
	case ETHTOOL_GTSO:
		return ethtool_get_tso(dev, useraddr);
	case ETHTOOL_STSO:
		return ethtool_set_tso(dev, useraddr);
	case ETHTOOL_TEST:
		return ethtool_self_test(dev, useraddr);
	case ETHTOOL_GSTRINGS:
		return ethtool_get_strings(dev, useraddr);
	case ETHTOOL_PHYS_ID:
		return ethtool_phys_id(dev, useraddr);
	case ETHTOOL_GSTATS:
		return ethtool_get_stats(dev, useraddr);
	default:
		return -EOPNOTSUPP;
	}

	return -EOPNOTSUPP;
}

#define mii_if_info _kc_mii_if_info
struct _kc_mii_if_info {
	int phy_id;
	int advertising;
	int phy_id_mask;
	int reg_num_mask;

	unsigned int full_duplex:1;	/* is full duplex? */
	unsigned int force_media:1;	/* is autoneg. disabled? */

	struct net_device *dev;
	int (*mdio_read) (struct net_device * dev, int phy_id, int location);
	void (*mdio_write) (struct net_device * dev, int phy_id, int location,
			    int val);
};

struct ethtool_cmd;
struct mii_ioctl_data;

#undef mii_link_ok
#define mii_link_ok _kc_mii_link_ok
#undef mii_nway_restart
#define mii_nway_restart _kc_mii_nway_restart
#undef mii_ethtool_gset
#define mii_ethtool_gset _kc_mii_ethtool_gset
#undef mii_ethtool_sset
#define mii_ethtool_sset _kc_mii_ethtool_sset
#undef mii_check_link
#define mii_check_link _kc_mii_check_link
extern int _kc_mii_link_ok(struct mii_if_info *mii);
extern int _kc_mii_nway_restart(struct mii_if_info *mii);
extern int _kc_mii_ethtool_gset(struct mii_if_info *mii,
				struct ethtool_cmd *ecmd);
extern int _kc_mii_ethtool_sset(struct mii_if_info *mii,
				struct ethtool_cmd *ecmd);
extern void _kc_mii_check_link(struct mii_if_info *mii);
#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,4,6) )
#undef generic_mii_ioctl
#define generic_mii_ioctl _kc_generic_mii_ioctl
extern int _kc_generic_mii_ioctl(struct mii_if_info *mii_if,
				 struct mii_ioctl_data *mii_data, int cmd,
				 unsigned int *duplex_changed);
#endif /* > 2.4.6 */

struct _kc_pci_dev_ext {
	struct pci_dev *dev;
	void *pci_drvdata;
	struct pci_driver *driver;
};

struct _kc_net_dev_ext {
	struct net_device *dev;
	unsigned int carrier;
};

/**************************************/
/* mii support */

int _kc_mii_ethtool_gset(struct mii_if_info *mii, struct ethtool_cmd *ecmd)
{
	struct net_device *dev = mii->dev;
	u32 advert, bmcr, lpa, nego;

	ecmd->supported =
	    (SUPPORTED_10baseT_Half | SUPPORTED_10baseT_Full |
	     SUPPORTED_100baseT_Half | SUPPORTED_100baseT_Full |
	     SUPPORTED_Autoneg | SUPPORTED_TP | SUPPORTED_MII);

	/* only supports twisted-pair */
	ecmd->port = PORT_MII;

	/* only supports internal transceiver */
	ecmd->transceiver = XCVR_INTERNAL;

	/* this isn't fully supported at higher layers */
	ecmd->phy_address = mii->phy_id;

	ecmd->advertising = ADVERTISED_TP | ADVERTISED_MII;
	advert = mii->mdio_read(dev, mii->phy_id, MII_ADVERTISE);
	if (advert & ADVERTISE_10HALF)
		ecmd->advertising |= ADVERTISED_10baseT_Half;
	if (advert & ADVERTISE_10FULL)
		ecmd->advertising |= ADVERTISED_10baseT_Full;
	if (advert & ADVERTISE_100HALF)
		ecmd->advertising |= ADVERTISED_100baseT_Half;
	if (advert & ADVERTISE_100FULL)
		ecmd->advertising |= ADVERTISED_100baseT_Full;

	bmcr = mii->mdio_read(dev, mii->phy_id, MII_BMCR);
	lpa = mii->mdio_read(dev, mii->phy_id, MII_LPA);
	if (bmcr & BMCR_ANENABLE) {
		ecmd->advertising |= ADVERTISED_Autoneg;
		ecmd->autoneg = AUTONEG_ENABLE;

		nego = mii_nway_result(advert & lpa);
		if (nego == LPA_100FULL || nego == LPA_100HALF)
			ecmd->speed = SPEED_100;
		else
			ecmd->speed = SPEED_10;
		if (nego == LPA_100FULL || nego == LPA_10FULL) {
			ecmd->duplex = DUPLEX_FULL;
			mii->full_duplex = 1;
		} else {
			ecmd->duplex = DUPLEX_HALF;
			mii->full_duplex = 0;
		}
	} else {
		ecmd->autoneg = AUTONEG_DISABLE;

		ecmd->speed = (bmcr & BMCR_SPEED100) ? SPEED_100 : SPEED_10;
		ecmd->duplex =
		    (bmcr & BMCR_FULLDPLX) ? DUPLEX_FULL : DUPLEX_HALF;
	}

	/* ignore maxtxpkt, maxrxpkt for now */

	return 0;
}

int _kc_mii_ethtool_sset(struct mii_if_info *mii, struct ethtool_cmd *ecmd)
{
	struct net_device *dev = mii->dev;

	if (ecmd->speed != SPEED_10 && ecmd->speed != SPEED_100)
		return -EINVAL;
	if (ecmd->duplex != DUPLEX_HALF && ecmd->duplex != DUPLEX_FULL)
		return -EINVAL;
	if (ecmd->port != PORT_MII)
		return -EINVAL;
	if (ecmd->transceiver != XCVR_INTERNAL)
		return -EINVAL;
	if (ecmd->phy_address != mii->phy_id)
		return -EINVAL;
	if (ecmd->autoneg != AUTONEG_DISABLE && ecmd->autoneg != AUTONEG_ENABLE)
		return -EINVAL;

	/* ignore supported, maxtxpkt, maxrxpkt */

	if (ecmd->autoneg == AUTONEG_ENABLE) {
		u32 bmcr, advert, tmp;

		if ((ecmd->advertising & (ADVERTISED_10baseT_Half |
					  ADVERTISED_10baseT_Full |
					  ADVERTISED_100baseT_Half |
					  ADVERTISED_100baseT_Full)) == 0)
			return -EINVAL;

		/* advertise only what has been requested */
		advert = mii->mdio_read(dev, mii->phy_id, MII_ADVERTISE);
		tmp = advert & ~(ADVERTISE_ALL | ADVERTISE_100BASE4);
		if (ADVERTISED_10baseT_Half)
			tmp |= ADVERTISE_10HALF;
		if (ADVERTISED_10baseT_Full)
			tmp |= ADVERTISE_10FULL;
		if (ADVERTISED_100baseT_Half)
			tmp |= ADVERTISE_100HALF;
		if (ADVERTISED_100baseT_Full)
			tmp |= ADVERTISE_100FULL;
		if (advert != tmp) {
			mii->mdio_write(dev, mii->phy_id, MII_ADVERTISE, tmp);
			mii->advertising = tmp;
		}

		/* turn on autonegotiation, and force a renegotiate */
		bmcr = mii->mdio_read(dev, mii->phy_id, MII_BMCR);
		bmcr |= (BMCR_ANENABLE | BMCR_ANRESTART);
		mii->mdio_write(dev, mii->phy_id, MII_BMCR, bmcr);

		mii->force_media = 0;
	} else {
		u32 bmcr, tmp;

		/* turn off auto negotiation, set speed and duplexity */
		bmcr = mii->mdio_read(dev, mii->phy_id, MII_BMCR);
		tmp = bmcr & ~(BMCR_ANENABLE | BMCR_SPEED100 | BMCR_FULLDPLX);
		if (ecmd->speed == SPEED_100)
			tmp |= BMCR_SPEED100;
		if (ecmd->duplex == DUPLEX_FULL) {
			tmp |= BMCR_FULLDPLX;
			mii->full_duplex = 1;
		} else
			mii->full_duplex = 0;
		if (bmcr != tmp)
			mii->mdio_write(dev, mii->phy_id, MII_BMCR, tmp);

		mii->force_media = 1;
	}
	return 0;
}

int _kc_mii_link_ok(struct mii_if_info *mii)
{
	/* first, a dummy read, needed to latch some MII phys */
	mii->mdio_read(mii->dev, mii->phy_id, MII_BMSR);
	if (mii->mdio_read(mii->dev, mii->phy_id, MII_BMSR) & BMSR_LSTATUS)
		return 1;
	return 0;
}

int _kc_mii_nway_restart(struct mii_if_info *mii)
{
	int bmcr;
	int r = -EINVAL;

	/* if autoneg is off, it's an error */
	bmcr = mii->mdio_read(mii->dev, mii->phy_id, MII_BMCR);

	if (bmcr & BMCR_ANENABLE) {
		bmcr |= BMCR_ANRESTART;
		mii->mdio_write(mii->dev, mii->phy_id, MII_BMCR, bmcr);
		r = 0;
	}

	return r;
}

void _kc_mii_check_link(struct mii_if_info *mii)
{
	int cur_link = mii_link_ok(mii);
	int prev_link = netif_carrier_ok(mii->dev);

	if (cur_link && !prev_link)
		netif_carrier_on(mii->dev);
	else if (prev_link && !cur_link)
		netif_carrier_off(mii->dev);
}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,4,6) )
int _kc_generic_mii_ioctl(struct mii_if_info *mii_if,
			  struct mii_ioctl_data *mii_data, int cmd,
			  unsigned int *duplex_chg_out)
{
	int rc = 0;
	unsigned int duplex_changed = 0;

	if (duplex_chg_out)
		*duplex_chg_out = 0;

	mii_data->phy_id &= mii_if->phy_id_mask;
	mii_data->reg_num &= mii_if->reg_num_mask;

	switch (cmd) {
	case SIOCDEVPRIVATE:	/* binary compat, remove in 2.5 */
	case SIOCGMIIPHY:
		mii_data->phy_id = mii_if->phy_id;
		/* fall through */

	case SIOCDEVPRIVATE + 1:	/* binary compat, remove in 2.5 */
	case SIOCGMIIREG:
		mii_data->val_out =
		    mii_if->mdio_read(mii_if->dev, mii_data->phy_id,
				      mii_data->reg_num);
		break;

	case SIOCDEVPRIVATE + 2:	/* binary compat, remove in 2.5 */
	case SIOCSMIIREG:{
			u16 val = mii_data->val_in;

			if (!capable(CAP_NET_ADMIN))
				return -EPERM;

			if (mii_data->phy_id == mii_if->phy_id) {
				switch (mii_data->reg_num) {
				case MII_BMCR:{
						unsigned int new_duplex = 0;
						if (val &
						    (BMCR_RESET |
						     BMCR_ANENABLE))
							mii_if->force_media = 0;
						else
							mii_if->force_media = 1;
						if (mii_if->force_media &&
						    (val & BMCR_FULLDPLX))
							new_duplex = 1;
						if (mii_if->full_duplex !=
						    new_duplex) {
							duplex_changed = 1;
							mii_if->full_duplex =
							    new_duplex;
						}
						break;
					}
				case MII_ADVERTISE:
					mii_if->advertising = val;
					break;
				default:
					/* do nothing */
					break;
				}
			}

			mii_if->mdio_write(mii_if->dev, mii_data->phy_id,
					   mii_data->reg_num, val);
			break;
		}

	default:
		rc = -EOPNOTSUPP;
		break;
	}

	if ((rc == 0) && (duplex_chg_out) && (duplex_changed))
		*duplex_chg_out = 1;

	return rc;
}
#endif /* > 2.4.6 */
