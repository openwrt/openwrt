/*
 **************************************************************************
 * Copyright (c) 2016-2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */
#include <linux/delay.h>
#include <linux/ethtool.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <nss_dp_hal_if.h>
#include <nss_dp_dev.h>
#include "syn_dev.h"

#define SYN_STAT(m)	offsetof(fal_xgmib_info_t, m)

struct syn_ethtool_stats {
	uint8_t stat_string[ETH_GSTRING_LEN];
	uint64_t stat_offset;
};

/*
 * Array of strings describing statistics
 */
static const struct syn_ethtool_stats syn_gstrings_stats[] = {
	{"rx_frame", SYN_STAT(RxFrame)},
	{"rx_bytes", SYN_STAT(RxByte)},
	{"rx_bytes_g", SYN_STAT(RxByteGood)},
	{"rx_broadcast", SYN_STAT(RxBroadGood)},
	{"rx_multicast", SYN_STAT(RxMultiGood)},
	{"rx_crc_err", SYN_STAT(RxFcsErr)},
	{"rx_runt_err", SYN_STAT(RxRuntErr)},
	{"rx_jabber_err", SYN_STAT(RxJabberError)},
	{"rx_undersize", SYN_STAT(RxUndersizeGood)},
	{"rx_oversize", SYN_STAT(RxOversizeGood)},
	{"rx_pkt64", SYN_STAT(Rx64Byte)},
	{"rx_pkt65to127", SYN_STAT(Rx128Byte)},
	{"rx_pkt128to255", SYN_STAT(Rx256Byte)},
	{"rx_pkt256to511", SYN_STAT(Rx512Byte)},
	{"rx_pkt512to1023", SYN_STAT(Rx1024Byte)},
	{"rx_pkt1024tomax", SYN_STAT(RxMaxByte)},
	{"rx_unicast", SYN_STAT(RxUnicastGood)},
	{"rx_len_err", SYN_STAT(RxLengthError)},
	{"rx_outofrange_err_ctr", SYN_STAT(RxOutOfRangeError)},
	{"rx_pause", SYN_STAT(RxPause)},
	{"rx_fifo_overflow", SYN_STAT(RxOverFlow)},
	{"rx_vlan", SYN_STAT(RxVLANFrameGoodBad)},
	{"rx_wdog", SYN_STAT(RxWatchDogError)},
	{"rx_lpi_usec_ctr", SYN_STAT(RxLPIUsec)},
	{"rx_lpi_tran_ctr", SYN_STAT(RxLPITran)},
	{"rx_drop_frame_ctr", SYN_STAT(RxDropFrameGoodBad)},
	{"rx_drop_byte_ctr", SYN_STAT(RxDropByteGoodBad)},
	{"tx_bytes", SYN_STAT(TxByte)},
	{"tx_frame", SYN_STAT(TxFrame)},
	{"tx_broadcast", SYN_STAT(TxBroadGood)},
	{"tx_broadcast_gb", SYN_STAT(TxBroad)},
	{"tx_multicast", SYN_STAT(TxMultiGood)},
	{"tx_multicast_gb", SYN_STAT(TxMulti)},
	{"tx_pkt64", SYN_STAT(Tx64Byte)},
	{"tx_pkt65to127", SYN_STAT(Tx128Byte)},
	{"tx_pkt128to255", SYN_STAT(Tx256Byte)},
	{"tx_pkt256to511", SYN_STAT(Tx512Byte)},
	{"tx_pkt512to1023", SYN_STAT(Tx1024Byte)},
	{"tx_pkt1024tomax", SYN_STAT(TxMaxByte)},
	{"tx_unicast", SYN_STAT(TxUnicast)},
	{"tx_underflow_err", SYN_STAT(TxUnderFlowError)},
	{"tx_bytes_g", SYN_STAT(TxByteGood)},
	{"tx_frame_g", SYN_STAT(TxFrameGood)},
	{"tx_pause", SYN_STAT(TxPause)},
	{"tx_vlan", SYN_STAT(TxVLANFrameGood)},
	{"tx_lpi_usec_ctr", SYN_STAT(TxLPIUsec)},
	{"tx_lpi_tran_ctr", SYN_STAT(TxLPITran)},
};

/*
 * Array of strings describing private flag names
 */
static const char *const syn_strings_priv_flags[] = {
	"test",
};

#define SYN_STATS_LEN	ARRAY_SIZE(syn_gstrings_stats)
#define SYN_PRIV_FLAGS_LEN	ARRAY_SIZE(syn_strings_priv_flags)

/*
 * syn_rx_flow_control()
 */
static void syn_rx_flow_control(struct nss_gmac_hal_dev *nghd,
				     bool enabled)
{
	BUG_ON(nghd == NULL);

	if (enabled)
		syn_set_rx_flow_ctrl(nghd);
	else
		syn_clear_rx_flow_ctrl(nghd);
}

/*
 * syn_tx_flow_control()
 */
static void syn_tx_flow_control(struct nss_gmac_hal_dev *nghd,
				     bool enabled)
{
	BUG_ON(nghd == NULL);

	if (enabled)
		syn_set_tx_flow_ctrl(nghd);
	else
		syn_clear_tx_flow_ctrl(nghd);
}

/*
 * syn_get_mmc_stats()
 */
static int32_t syn_get_mmc_stats(struct nss_gmac_hal_dev *nghd)
{
	BUG_ON(nghd == NULL);

	if (syn_get_stats(nghd))
		return -1;

	return 0;
}

/*
 * syn_get_max_frame_size()
 */
static int32_t syn_get_max_frame_size(struct nss_gmac_hal_dev *nghd)
{
	int ret;
	uint32_t mtu;

	ret = fal_port_max_frame_size_get(0, nghd->mac_id, &mtu);

	if (!ret)
		return mtu;

	return ret;
}

/*
 * syn_set_max_frame_size()
 */
static int32_t syn_set_max_frame_size(struct nss_gmac_hal_dev *nghd,
					uint32_t val)
{
	return fal_port_max_frame_size_set(0, nghd->mac_id, val);
}

/*
 * syn_set_mac_speed()
 */
static int32_t syn_set_mac_speed(struct nss_gmac_hal_dev *nghd,
				   uint32_t mac_speed)
{
	struct net_device *netdev = nghd->netdev;

	netdev_warn(netdev, "API deprecated\n");
	return 0;
}

/*
 * syn_get_mac_speed()
 */
static uint32_t syn_get_mac_speed(struct nss_gmac_hal_dev *nghd)
{
	struct net_device *netdev = nghd->netdev;

	netdev_warn(netdev, "API deprecated\n");
	return 0;
}

/*
 * syn_set_duplex_mode()
 */
static void syn_set_duplex_mode(struct nss_gmac_hal_dev *nghd,
				uint8_t duplex_mode)
{
	struct net_device *netdev = nghd->netdev;

	netdev_warn(netdev, "API deprecated\n");
}

/*
 * syn_get_duplex_mode()
 */
static uint8_t syn_get_duplex_mode(struct nss_gmac_hal_dev *nghd)
{
	struct net_device *netdev = nghd->netdev;

	netdev_warn(netdev, "API deprecated\n");
	return 0;
}

/*
 * syn_get_netdev_stats()
 */
static int syn_get_netdev_stats(struct nss_gmac_hal_dev *nghd,
		struct rtnl_link_stats64 *stats)
{
	struct syn_hal_dev *shd;
	fal_xgmib_info_t *hal_stats;

	BUG_ON(nghd == NULL);

	shd = (struct syn_hal_dev *)nghd;
	hal_stats = &(shd->stats);

	if (syn_get_stats(nghd))
		return -1;

	stats->rx_packets = hal_stats->RxUnicastGood
		+ hal_stats->RxBroadGood + hal_stats->RxMultiGood;
	stats->tx_packets = hal_stats->TxUnicast
		+ hal_stats->TxBroadGood + hal_stats->TxMultiGood;
	stats->rx_bytes = hal_stats->RxByte;
	stats->tx_bytes = hal_stats->TxByte;
	stats->multicast =
		hal_stats->RxMultiGood;
	stats->rx_dropped =
		hal_stats->RxDropFrameGoodBad;
	stats->rx_length_errors =
		hal_stats->RxLengthError;
	stats->rx_crc_errors =
		hal_stats->RxFcsErr;
	stats->rx_fifo_errors =
		hal_stats->RxOverFlow;

	return 0;
}

/*
 * syn_get_eth_stats()
 */
static int32_t syn_get_eth_stats(struct nss_gmac_hal_dev *nghd,
				   uint64_t *data)
{
	struct syn_hal_dev *shd;
	fal_xgmib_info_t *stats;
	uint8_t *p = NULL;
	int i;

	BUG_ON(nghd == NULL);

	shd = (struct syn_hal_dev *)nghd;
	stats = &(shd->stats);

	if (syn_get_stats(nghd))
		return -1;

	for (i = 0; i < SYN_STATS_LEN; i++) {
		p = ((uint8_t *)(stats) +
				syn_gstrings_stats[i].stat_offset);
		data[i] = *(uint32_t *)p;
	}

	return 0;
}

/*
 * syn_get_strset_count()
 */
static int32_t syn_get_strset_count(struct nss_gmac_hal_dev *nghd,
					 int32_t sset)
{
	struct net_device *netdev;

	BUG_ON(nghd == NULL);

	netdev = nghd->netdev;

	switch (sset) {
	case ETH_SS_STATS:
		return SYN_STATS_LEN;

	case ETH_SS_PRIV_FLAGS:
		return SYN_PRIV_FLAGS_LEN;
	}

	netdev_dbg(netdev, "%s: Invalid string set\n", __func__);
	return -EPERM;
}

/*
 * syn_get_strings()
 */
static int32_t syn_get_strings(struct nss_gmac_hal_dev *nghd,
				    int32_t stringset, uint8_t *data)
{
	struct net_device *netdev;
	int i;

	BUG_ON(nghd == NULL);

	netdev = nghd->netdev;

	switch (stringset) {
	case ETH_SS_STATS:
		for (i = 0; i < SYN_STATS_LEN; i++) {
			memcpy(data, syn_gstrings_stats[i].stat_string,
				strlen(syn_gstrings_stats[i].stat_string));
			data += ETH_GSTRING_LEN;
		}
		break;

	case ETH_SS_PRIV_FLAGS:
		for (i = 0; i < SYN_PRIV_FLAGS_LEN; i++) {
			memcpy(data, syn_strings_priv_flags[i],
				strlen(syn_strings_priv_flags[i]));
			data += ETH_GSTRING_LEN;
		}

		break;
	default:
		netdev_dbg(netdev, "%s: Invalid string set\n", __func__);
		return -EPERM;
	}

	return 0;
}

/*
 * syn_send_pause_frame()
 */
static void syn_send_pause_frame(struct nss_gmac_hal_dev *nghd)
{
	BUG_ON(nghd == NULL);

	syn_send_tx_pause_frame(nghd);
}

/*
 * syn_start
 */
static int32_t syn_start(struct nss_gmac_hal_dev *nghd)
{
	BUG_ON(nghd == NULL);

	syn_tx_enable(nghd);
	syn_rx_enable(nghd);
	syn_set_full_duplex(nghd);
	if (syn_set_mac_speed(nghd, SPEED_10000))
		return -1;

	netdev_dbg(nghd->netdev,
			"%s: mac_base:0x%px tx_enable:0x%x rx_enable:0x%x\n",
			__func__,
			nghd->mac_base,
			hal_read_reg(nghd->mac_base,
				SYN_MAC_TX_CONFIG),
			hal_read_reg(nghd->mac_base,
				SYN_MAC_RX_CONFIG));

	return 0;
}

/*
 * syn_stop
 */
static int32_t syn_stop(struct nss_gmac_hal_dev *nghd)
{
	BUG_ON(nghd == NULL);

	syn_tx_disable(nghd);
	syn_rx_disable(nghd);

	netdev_dbg(nghd->netdev, "%s: Stopping mac_base:0x%px\n", __func__,
		   nghd->mac_base);

	return 0;
}

/*
 * syn_init()
 */
static void *syn_init(struct gmac_hal_platform_data *gmacpdata)
{
	struct syn_hal_dev *shd = NULL;
	struct net_device *ndev = NULL;
	struct nss_dp_dev *dp_priv = NULL;
	struct resource *res;

	ndev = gmacpdata->netdev;
	dp_priv = netdev_priv(ndev);

	res = platform_get_resource(dp_priv->pdev, IORESOURCE_MEM, 0);
	if (!res) {
		netdev_dbg(ndev, "Resource get failed.\n");
		return NULL;
	}

	if (!devm_request_mem_region(&dp_priv->pdev->dev, res->start,
				     resource_size(res), ndev->name)) {
		netdev_dbg(ndev, "Request mem region failed. Returning...\n");
		return NULL;
	}

	shd = (struct syn_hal_dev *)devm_kzalloc(&dp_priv->pdev->dev,
					sizeof(struct syn_hal_dev),
					GFP_KERNEL);
	if (!shd) {
		netdev_dbg(ndev, "kzalloc failed. Returning...\n");
		return NULL;
	}

	/* Save netdev context in syn HAL context */
	shd->nghd.netdev = gmacpdata->netdev;
	shd->nghd.mac_id = gmacpdata->macid;

	/* Populate the mac base addresses */
	shd->nghd.mac_base =
		devm_ioremap_nocache(&dp_priv->pdev->dev, res->start,
				     resource_size(res));
	if (!shd->nghd.mac_base) {
		netdev_dbg(ndev, "ioremap fail.\n");
		return NULL;
	}

	spin_lock_init(&shd->nghd.slock);

	netdev_dbg(ndev, "ioremap OK.Size 0x%x Ndev base 0x%lx macbase 0x%px\n",
			gmacpdata->reg_len,
			ndev->base_addr,
			shd->nghd.mac_base);

	/* Reset MIB Stats */
	if (fal_mib_port_flush_counters(0, shd->nghd.mac_id)) {
		netdev_dbg(ndev, "MIB stats Reset fail.\n");
	}

	return (struct nss_gmac_hal_dev *)shd;
}

/*
 * syn_set_mac_address()
 */
static void syn_set_mac_address(struct nss_gmac_hal_dev *nghd,
				     uint8_t *macaddr)
{
	uint32_t data;

	BUG_ON(nghd == NULL);

	data = (macaddr[5] << 8) | macaddr[4] | SYN_MAC_ADDR_RSVD_BIT;
	hal_write_reg(nghd->mac_base, SYN_MAC_ADDR0_HIGH, data);
	data = (macaddr[3] << 24) | (macaddr[2] << 16) | (macaddr[1] << 8)
		| macaddr[0];
	hal_write_reg(nghd->mac_base, SYN_MAC_ADDR0_LOW, data);
}

/*
 * syn_get_mac_address()
 */
static void syn_get_mac_address(struct nss_gmac_hal_dev *nghd,
				     uint8_t *macaddr)
{
	uint32_t data;

	BUG_ON(nghd == NULL);

	data = hal_read_reg(nghd->mac_base, SYN_MAC_ADDR0_HIGH);
	macaddr[5] = (data >> 8) & 0xff;
	macaddr[4] = (data) & 0xff;

	data = hal_read_reg(nghd->mac_base, SYN_MAC_ADDR0_LOW);
	macaddr[3] = (data >> 24) & 0xff;
	macaddr[2] = (data >> 16) & 0xff;
	macaddr[1] = (data >> 8) & 0xff;
	macaddr[0] = (data) & 0xff;
}

struct nss_gmac_hal_ops syn_hal_ops = {
	.init = &syn_init,
	.start = &syn_start,
	.stop = &syn_stop,
	.setmacaddr = &syn_set_mac_address,
	.getmacaddr = &syn_get_mac_address,
	.rxflowcontrol = &syn_rx_flow_control,
	.txflowcontrol = &syn_tx_flow_control,
	.setspeed = &syn_set_mac_speed,
	.getspeed = &syn_get_mac_speed,
	.setduplex = &syn_set_duplex_mode,
	.getduplex = &syn_get_duplex_mode,
	.getstats = &syn_get_mmc_stats,
	.setmaxframe = &syn_set_max_frame_size,
	.getmaxframe = &syn_get_max_frame_size,
	.getndostats = &syn_get_netdev_stats,
	.getssetcount = &syn_get_strset_count,
	.getstrings = &syn_get_strings,
	.getethtoolstats = &syn_get_eth_stats,
	.sendpause = &syn_send_pause_frame,
};
