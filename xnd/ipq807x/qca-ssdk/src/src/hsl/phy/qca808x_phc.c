/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
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

#include <linux/if_vlan.h>
#include <linux/net_tstamp.h>
#include <linux/ptp_classify.h>

#include "qca808x.h"

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
#include <linux/time64.h>
#else
#include <linux/time.h>
#define ns_to_timespec64 ns_to_timespec
#define timespec64_to_ns timespec_to_ns
#define timespec64 timespec
#endif

#include "qca808x_ptp.h"
#include "qca808x_ptp_reg.h"
#include "qca808x_ptp_api.h"

#define QCA808X_PTP_EMBEDDED_MODE    0xa

#define QCA808X_PTP_INCVAL_SYNC_MODE 0x8
#define QCA808X_PTP_TICK_RATE_125M   8
#define QCA808X_PTP_TICK_RATE_200M   5

#define PTP_HDR_RESERVED0_OFFSET	1
#define PTP_HDR_RESERVED1_OFFSET	5
#define PTP_HDR_CORRECTIONFIELD_OFFSET	8
#define PTP_HDR_RESERVED2_OFFSET	16

#define SKB_TIMESTAMP_TIMEOUT        1 /* jiffies */
#define GPS_WORK_TIMEOUT             HZ

#define HWTSTAMP_TX_ONESTEP_P2P     (HWTSTAMP_TX_ONESTEP_SYNC + 1)

extern struct list_head g_qca808x_phy_list;
void qca808x_ptp_gm_gps_seconds_sync_enable(a_uint32_t dev_id,
		a_uint32_t phy_addr, a_bool_t en)
{
	struct qca808x_phy_info *pdata;
#if defined(IN_PHY_I2C_MODE)
	a_uint32_t port_id;
	port_id = qca_ssdk_phy_addr_to_port(dev_id, phy_addr);
	if (hsl_port_phy_access_type_get(dev_id, port_id) == PHY_I2C_ACCESS) {
		phy_addr = qca_ssdk_port_to_phy_mdio_fake_addr(dev_id, port_id);
	}
#endif

	pdata = qca808x_phy_info_get(phy_addr);
	if (pdata) {
		pdata->gps_seconds_sync_en = en;
	}

	if (en == A_TRUE) {
		schedule_delayed_work(&pdata->ts_schedule_work, GPS_WORK_TIMEOUT);
	}

	return;
}

a_bool_t qca808x_ptp_gm_gps_seconds_sync_status_get(a_uint32_t dev_id,
		a_uint32_t phy_addr)
{
	struct qca808x_phy_info *pdata;
#if defined(IN_PHY_I2C_MODE)
	a_uint32_t port_id;
	port_id = qca_ssdk_phy_addr_to_port(dev_id, phy_addr);
	if (hsl_port_phy_access_type_get(dev_id, port_id) == PHY_I2C_ACCESS) {
		phy_addr = qca_ssdk_port_to_phy_mdio_fake_addr(dev_id, port_id);
	}
#endif

	pdata = qca808x_phy_info_get(phy_addr);
	if (pdata) {
		return pdata->gps_seconds_sync_en;
	}

	return A_FALSE;
}

void qca808x_ptp_clock_mode_config(a_uint32_t dev_id,
		a_uint32_t phy_addr, a_uint16_t clock_mode, a_uint16_t step_mode)
{
	struct qca808x_phy_info *pdata;
#if defined(IN_PHY_I2C_MODE)
	a_uint32_t port_id;
	port_id = qca_ssdk_phy_addr_to_port(dev_id, phy_addr);
	if (hsl_port_phy_access_type_get(dev_id, port_id) == PHY_I2C_ACCESS) {
		phy_addr = qca_ssdk_port_to_phy_mdio_fake_addr(dev_id, port_id);
	}
#endif

	pdata = qca808x_phy_info_get(phy_addr);

	if (pdata) {
		pdata->clock_mode = clock_mode;
		pdata->step_mode = step_mode;
	}

	return;
}

void qca808x_ptp_stat_update(struct qca808x_phy_info *pdata, fal_ptp_direction_t direction,
		a_int32_t msg_type, a_int32_t seqid_matched)
{
	ptp_packet_stat *pkt_stat = NULL;

	if (((direction != FAL_RX_DIRECTION) && (direction != FAL_TX_DIRECTION)) ||
			(seqid_matched < PTP_PKT_SEQID_UNMATCHED) ||
			(seqid_matched >= PTP_PKT_SEQID_MATCH_MAX)) {
		return;
	}

	pkt_stat = &pdata->pkt_stat[direction];
	switch (msg_type) {
		case QCA808X_PTP_MSG_SYNC:
			pkt_stat->sync_cnt[seqid_matched]++;
			break;
		case QCA808X_PTP_MSG_DREQ:
			pkt_stat->delay_req_cnt[seqid_matched]++;
			break;
		case QCA808X_PTP_MSG_PREQ:
			pkt_stat->pdelay_req_cnt[seqid_matched]++;
			break;
		case QCA808X_PTP_MSG_PRESP:
			pkt_stat->pdelay_resp_cnt[seqid_matched]++;
			break;
		case QCA808X_PTP_MSG_MAX:
			pkt_stat->event_pkt_cnt++;
			break;
		default:
			SSDK_DEBUG("%s: msg %x is not event frame\n",
					__func__, msg_type);
	}
}

void qca808x_ptp_stat_get(void)
{
	int i = 0;
	struct qca808x_phy_info *pdata = NULL;
	ptp_packet_stat *pkt_stat;

	list_for_each_entry(pdata, &g_qca808x_phy_list, list) {
		pkt_stat = pdata->pkt_stat;
		SSDK_INFO("PHY [%#x] PTP event packet statistics:\n", pdata->phy_addr);
		for (i=0; i <= FAL_TX_DIRECTION; i++)
		{
			if (i == FAL_TX_DIRECTION) {
				SSDK_INFO("----------TX direction----------\n");
			} else {
				SSDK_INFO("----------RX direction----------\n");
			}

			SSDK_INFO("even sum:    %lld\n",
					pkt_stat[i].event_pkt_cnt);
			SSDK_INFO("seq id matched stat:\n");
			SSDK_INFO("sync:        %lld\n",
					pkt_stat[i].sync_cnt[PTP_PKT_SEQID_MATCHED]);
			SSDK_INFO("delay_req:   %lld\n",
					pkt_stat[i].delay_req_cnt[PTP_PKT_SEQID_MATCHED]);
			SSDK_INFO("pdelay_req:  %lld\n",
					pkt_stat[i].pdelay_req_cnt[PTP_PKT_SEQID_MATCHED]);
			SSDK_INFO("pdelay_resp: %lld\n\n",
					pkt_stat[i].pdelay_resp_cnt[PTP_PKT_SEQID_MATCHED]);

			SSDK_INFO("seq id unmatched stat:\n");
			SSDK_INFO("sync:        %lld\n",
					pkt_stat[i].sync_cnt[PTP_PKT_SEQID_UNMATCHED]);
			SSDK_INFO("delay_req:   %lld\n",
					pkt_stat[i].delay_req_cnt[PTP_PKT_SEQID_UNMATCHED]);
			SSDK_INFO("pdelay_req:  %lld\n",
					pkt_stat[i].pdelay_req_cnt[PTP_PKT_SEQID_UNMATCHED]);
			SSDK_INFO("pdelay_resp: %lld\n\n",
					pkt_stat[i].pdelay_resp_cnt[PTP_PKT_SEQID_UNMATCHED]);
		}
	}
}

void qca808x_ptp_stat_set(void)
{
	struct qca808x_phy_info *pdata;

	list_for_each_entry(pdata, &g_qca808x_phy_list, list) {
		memset(pdata->pkt_stat, 0, sizeof(pdata->pkt_stat));
	}
}

static sw_error_t qca808x_ptp_clock_synce_clock_enable(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data, phy_data1;
	sw_error_t ret = SW_OK;

	phy_data = qca808x_phy_debug_read(dev_id, phy_id,
			QCA808X_DEBUG_ANA_CLOCK_CTRL_REG);

	phy_data1 = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
			QCA808X_MMD7_CLOCK_CTRL_REG);

	if (enable == A_TRUE) {
		/* enable analog synce clock output */
		phy_data |= QCA808X_ANALOG_PHY_SYNCE_CLOCK_EN;
		/* enable digital synce clock output */
		phy_data1 |= QCA808X_DIGITAL_PHY_SYNCE_CLOCK_EN;
	} else {
		phy_data &= ~QCA808X_ANALOG_PHY_SYNCE_CLOCK_EN;
		phy_data1 &= ~QCA808X_DIGITAL_PHY_SYNCE_CLOCK_EN;
	}

	ret = qca808x_phy_debug_write(dev_id, phy_id,
			QCA808X_DEBUG_ANA_CLOCK_CTRL_REG, phy_data);

	ret |= qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
			QCA808X_MMD7_CLOCK_CTRL_REG, phy_data1);
	return ret;
}

static sw_error_t qca808x_ptp_clock_incval_mode_set(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;
	sw_error_t ret = SW_OK;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
			PTP_RTC_EXT_CONF_REG_ADDRESS);

	if (enable == A_TRUE) {
		phy_data |= QCA808X_PTP_INCVAL_SYNC_MODE;
	} else {
		phy_data &= ~QCA808X_PTP_INCVAL_SYNC_MODE;
	}

	ret = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
			PTP_RTC_EXT_CONF_REG_ADDRESS, phy_data);

	return ret;
}

sw_error_t qca808x_ptp_config_init(struct phy_device *phydev)
{
	fal_ptp_config_t ptp_config;
	fal_ptp_reference_clock_t ptp_ref_clock;
	fal_ptp_rx_timestamp_mode_t rx_ts_mode;
	fal_ptp_time_t ptp_time = {0};
	sw_error_t ret = SW_OK;
	a_uint32_t dev_id = 0, phy_id = 0;
	qca808x_priv *priv = phydev->priv;
	const struct qca808x_phy_info *pdata = priv->phy_info;
	struct qca808x_ptp_info *ptp_info = &priv->ptp_info;

	if (!pdata) {
		return SW_FAIL;
	}

	dev_id = pdata->dev_id;
	phy_id = pdata->phy_addr;

	/* disable ptp function by default */
	ptp_info->hwts_tx_type = HWTSTAMP_TX_OFF;
	ptp_info->hwts_rx_type = PTP_CLASS_NONE;

	ptp_config.ptp_en = A_FALSE;
	ptp_config.clock_mode = FAL_OC_CLOCK_MODE;
	ptp_config.step_mode = FAL_TWO_STEP_MODE;
	ret = qca808x_phy_ptp_config_set(dev_id, phy_id, &ptp_config);

	qca808x_ptp_clock_mode_config(dev_id, phy_id, ptp_config.clock_mode,
			ptp_config.step_mode);

	/* set rtc clock to asynchronization mode*/
	ret |= qca808x_ptp_clock_incval_mode_set(dev_id, phy_id, A_FALSE);

	/* adjust frequency to 8ns(125MHz) */
	ptp_time.nanoseconds = QCA808X_PTP_TICK_RATE_125M;
	ret |= qca808x_phy_ptp_rtc_adjfreq_set(dev_id, phy_id, &ptp_time);

	/* use SyncE reference clock */
	ptp_ref_clock = FAL_REF_CLOCK_SYNCE;
	ret |= qca808x_phy_ptp_reference_clock_set(dev_id, phy_id, ptp_ref_clock);

	/* use Embed mode to get RX timestamp */
	rx_ts_mode = FAL_RX_TS_EMBED;
	ret |= qca808x_phy_ptp_rx_timestamp_mode_set(dev_id, phy_id, rx_ts_mode);

	/* disable SYNCE clock output */
	ret |= qca808x_ptp_clock_synce_clock_enable(dev_id, phy_id, A_FALSE);

	if (ret != SW_OK) {
		SSDK_ERROR("%s failed\n", __func__);
	}

	return ret;
}

static a_uint8_t* skb_ptp_header(struct sk_buff *skb, int type)
{
	a_uint8_t *data = skb_mac_header(skb);
	a_uint32_t offset = 0;

	if (type & PTP_CLASS_VLAN) {
		offset += VLAN_HLEN;
	}

	switch (type & PTP_CLASS_PMASK) {
		case PTP_CLASS_IPV4:
			offset += ETH_HLEN + IPV4_HLEN(data + offset) + UDP_HLEN;
			break;
		case PTP_CLASS_IPV6:
			offset += ETH_HLEN + IP6_HLEN + UDP_HLEN;
			break;
		case PTP_CLASS_L2:
			offset += ETH_HLEN;
			break;
		default:
			return NULL;
	}

	if (skb->len + ETH_HLEN < offset +
			OFF_PTP_SEQUENCE_ID + sizeof(a_uint16_t)) {
		return NULL;
	}

	return data + offset;
}

void qca808x_pkt_info_get(struct sk_buff *skb,
		unsigned int type, fal_ptp_pkt_info_t *pkt_info)
{
	a_uint16_t *seqid, seqid_pkt;
	a_uint32_t *clockid;
	a_uint32_t clockid_lo;
	a_uint64_t clockid_pkt;
	a_uint16_t *portid, portid_pkt;
	a_uint8_t msgtype_pkt;
	a_uint8_t *ptp_header = NULL;

	ptp_header = skb_ptp_header(skb, type);
	if (!ptp_header) {
		return;
	}

#define OFF_PTP_CLOCK_ID 20
#define OFF_PTP_PORT_ID 28

	seqid = (a_uint16_t *)(ptp_header + OFF_PTP_SEQUENCE_ID);
	seqid_pkt = ntohs(*seqid);
	clockid  = (a_uint32_t *)(ptp_header + OFF_PTP_CLOCK_ID);
	clockid_pkt = ntohl(*clockid);

	clockid  = (a_uint32_t *)(ptp_header + OFF_PTP_CLOCK_ID + 4);
	clockid_lo = ntohl(*clockid);
	clockid_pkt =  (clockid_pkt << 32) | clockid_lo;
	portid = (a_uint16_t *)(ptp_header + OFF_PTP_PORT_ID);
	portid_pkt = ntohs(*portid);
	msgtype_pkt = (*ptp_header) & 0xf;

	pkt_info->sequence_id = seqid_pkt;
	pkt_info->clock_identify = clockid_pkt;
	pkt_info->port_number = portid_pkt;
	pkt_info->msg_type = msgtype_pkt;
	return ;
}

static void tx_timestamp_work(struct work_struct *work)
{
	struct sk_buff *skb;
	struct skb_shared_hwtstamps shhwtstamps;
	struct timespec64 ts;
	a_uint64_t ns;
	a_uint32_t dev_id, phy_id;
	qca808x_ptp_cb *ptp_cb;
	struct qca808x_phy_info *pdata;
	fal_ptp_pkt_info_t pkt_info;
	fal_ptp_time_t tx_time = {0};
	sw_error_t ret = SW_OK;
	a_uint16_t times = 0;
	a_uint16_t seqid = 0;
	struct qca808x_ptp_info *ptp_data =
		container_of(work, struct qca808x_ptp_info, tx_ts_work.work);

	qca808x_priv *priv =
		container_of(ptp_data, qca808x_priv, ptp_info);

	pdata = priv->phy_info;

	if (!pdata) {
		return;
	}

	dev_id = pdata->dev_id;
	phy_id = pdata->phy_addr;

	memset(&shhwtstamps, 0, sizeof(shhwtstamps));

	while ((skb = skb_dequeue(&ptp_data->tx_queue))) {
		ptp_cb = (qca808x_ptp_cb *)skb->cb;
		qca808x_pkt_info_get(skb, ptp_cb->ptp_type, &pkt_info);

		times = 0;
		do {
			/* poll the seqid of the transmitted ptp packet to
			 * acquire the correspoding tx time stamp.
			 */
			seqid = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
					PTP_TX_SEQID_REG_ADDRESS);
			udelay(1);
			times++;
		} while (seqid != pkt_info.sequence_id && times < 100);

		ret = qca808x_phy_ptp_timestamp_get(dev_id, phy_id,
				FAL_TX_DIRECTION, &pkt_info, &tx_time);

		if (ret == SW_NOT_FOUND) {
			qca808x_ptp_stat_update(pdata, FAL_TX_DIRECTION,
					pkt_info.msg_type, PTP_PKT_SEQID_UNMATCHED);
			SSDK_DEBUG("Fail to get tx_ts: sequence_id:%x, clock_identify:%llx,"
					" port_number:%x, msg_type:%x\n",
					pkt_info.sequence_id, pkt_info.clock_identify,
					pkt_info.port_number, pkt_info.msg_type);
		} else {
			qca808x_ptp_stat_update(pdata, FAL_TX_DIRECTION,
					pkt_info.msg_type, PTP_PKT_SEQID_MATCHED);
		}
		ts.tv_sec = tx_time.seconds;
		ts.tv_nsec = tx_time.nanoseconds;

		ns = timespec64_to_ns(&ts);

		shhwtstamps.hwtstamp = ns_to_ktime(ns);
		skb_complete_tx_timestamp(skb, &shhwtstamps);
	}

	if (!skb_queue_empty(&ptp_data->tx_queue))
		schedule_delayed_work(&ptp_data->tx_ts_work, SKB_TIMESTAMP_TIMEOUT);
}

static void ptp_ingress_time_sync(a_uint32_t phy_addr, a_uint32_t ingress_time,
		a_bool_t forward)
{
	fal_ptp_time_t ingress_trig_time = {0};
	struct qca808x_phy_info *pdata = NULL;

	ingress_trig_time.nanoseconds = ingress_time;

	if (forward == A_FALSE) {
		list_for_each_entry(pdata, &g_qca808x_phy_list, list) {
			if (pdata->phydev_addr == phy_addr) {
				qca808x_phy_ptp_pkt_timestamp_set(pdata->dev_id,
						pdata->phy_addr, &ingress_trig_time);
				break;
			}
		}
	} else {
		list_for_each_entry(pdata, &g_qca808x_phy_list, list) {
			if (pdata->phydev_addr != phy_addr) {
				qca808x_phy_ptp_pkt_timestamp_set(pdata->dev_id,
						pdata->phy_addr, &ingress_trig_time);
			}
		}
	}
}

static void rx_timestamp_work(struct work_struct *work)
{
	struct sk_buff *skb;
	struct skb_shared_hwtstamps *shhwtstamps = NULL;
	struct timespec64 ts;
	a_uint64_t ns;
	a_uint32_t dev_id, phy_id;
	qca808x_ptp_cb *ptp_cb;
	struct qca808x_phy_info *pdata;
	fal_ptp_pkt_info_t pkt_info;
	fal_ptp_time_t rx_time = {0};
	sw_error_t ret = SW_OK;

	struct qca808x_ptp_info *ptp_data =
		container_of(work, struct qca808x_ptp_info, rx_ts_work.work);

	qca808x_priv *priv =
		container_of(ptp_data, qca808x_priv, ptp_info);

	pdata = priv->phy_info;

	if (!pdata) {
		return;
	}
	dev_id = pdata->dev_id;
	phy_id = pdata->phy_addr;

	/* Deliver packets */
	while ((skb = skb_dequeue(&ptp_data->rx_queue))) {
		ptp_cb = (qca808x_ptp_cb *)skb->cb;
		qca808x_pkt_info_get(skb, ptp_cb->ptp_type, &pkt_info);

		ret = qca808x_phy_ptp_timestamp_get(dev_id, phy_id,
				FAL_RX_DIRECTION, &pkt_info, &rx_time);
		if (ret == SW_NOT_FOUND) {
			qca808x_ptp_stat_update(pdata, FAL_RX_DIRECTION,
					pkt_info.msg_type, PTP_PKT_SEQID_UNMATCHED);
			SSDK_DEBUG("Fail to get rx_ts: sequence_id:%x, clock_identify:%llx, "
					"port_number:%x, msg_type:%x\n",
					pkt_info.sequence_id, pkt_info.clock_identify,
					pkt_info.port_number, pkt_info.msg_type);
		} else {
			qca808x_ptp_stat_update(pdata, FAL_RX_DIRECTION,
					pkt_info.msg_type, PTP_PKT_SEQID_MATCHED);
		}

		ts.tv_sec = rx_time.seconds;
		ts.tv_nsec = rx_time.nanoseconds;
		ns = timespec64_to_ns(&ts);
		shhwtstamps = skb_hwtstamps(skb);
		memset(shhwtstamps, 0, sizeof(*shhwtstamps));
		shhwtstamps->hwtstamp = ns_to_ktime(ns);

		/* OC/BC needs record ingress time stamp on receiving
		 * peer delay request message under one-step mode.
		 * TC one step mode should use the embeded mode for offloading
		 * function.
		 */
		if (pdata->step_mode == FAL_ONE_STEP_MODE) {
			switch (pdata->clock_mode) {
				case FAL_OC_CLOCK_MODE:
				case FAL_BC_CLOCK_MODE:
					if (pkt_info.msg_type == QCA808X_PTP_MSG_PREQ) {
						ptp_ingress_time_sync(phy_id,
								ts.tv_nsec, A_FALSE);
					}
					break;
				default:
					break;
			}
		}
		netif_rx_ni(skb);
	}

	if (!skb_queue_empty(&ptp_data->rx_queue))
		schedule_delayed_work(&ptp_data->rx_ts_work, SKB_TIMESTAMP_TIMEOUT);
}

static void qca808x_gps_second_sync(struct qca808x_phy_info *pdata, a_int32_t *buf)
{
	fal_ptp_time_t time, old_time;
	a_uint32_t dev_id, phy_id;

	if (!pdata) {
		return;
	}
	dev_id = pdata->dev_id;
	phy_id = pdata->phy_addr;
	/* 0-3: time of week; 4-5: week number; 6-7: UTC offset
	 * Time(UTC) = Time(GPS) - UTC offset */
#define WEEK_TIME  604800
	time.seconds = ((a_int64_t)buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]) +
		((buf[4] << 8 | buf[5]) * WEEK_TIME) - (buf[6] << 8 | buf[7]);

	time.nanoseconds = 0;
	time.fracnanoseconds = 0;

	qca808x_phy_ptp_rtc_time_get(dev_id, phy_id, &old_time);
	time.seconds -= old_time.seconds;
	qca808x_phy_ptp_rtc_adjtime_set(dev_id, phy_id, &time);

	qca808x_ptp_gm_gps_seconds_sync_enable(dev_id, phy_id, A_FALSE);
	return;
}

static void gps_seconds_sync_thread(struct qca808x_phy_info *pdata)
{
	struct file *filp;
	mm_segment_t fs;
	a_uint32_t nread;
	char buff[128];
	char *dev ="/dev/ttyMSM0";
	a_int32_t data[32];
	a_uint32_t i = 0, j = 0;
	a_bool_t is_time_pkt = A_FALSE;
	a_int32_t try_cycle = 32;

	filp = filp_open(dev, O_RDONLY, 0);
	if (IS_ERR(filp))
	{
		SSDK_ERROR("Open %s error\n", dev);
		return;
	}

	fs=get_fs();
	set_fs(KERNEL_DS);
	while(1)
	{
		memset(data, 0, sizeof(data));
		memset(buff, 0, sizeof(buff));
		is_time_pkt = A_FALSE;
		filp->f_pos = 0;
		nread = filp->f_op->read(filp, buff, sizeof(buff), &filp->f_pos);
		if (nread > 0)
		{
	/* the packet format: <PKT_DLE><PKT_ID><DATA STRING><PKT_DLE><PKT_ETX> */
#define PKT_DLE       0x10
#define PKT_ETX       0x3
#define PKT_PTIME_ID  0x8f
#define PKT_PTIME_SID 0xab
#define PKT_PTIME_LEN 0x10
			buff[nread+1]='\0';
			for(i = 0; i < nread; i++)
			{
				if (is_time_pkt == A_FALSE)
				{
					if (buff[i] == PKT_DLE && i+2 < nread &&
							buff[i+1] == PKT_PTIME_ID &&
							buff[i+2] == PKT_PTIME_SID) {
						is_time_pkt = A_TRUE;
						i = i + 2;
						j = 0;
					}
				} else {
					data[j++] = buff[i];
					if (j >= PKT_PTIME_LEN && data[j-2] == PKT_DLE &&
							data[j-1] == PKT_ETX)
					{
						qca808x_gps_second_sync(pdata, data);
						goto gps_time_sync_exit;
					}

					if (j > PKT_PTIME_LEN+2) {
						is_time_pkt = A_FALSE;
					}
				}
			}
		}

		if (--try_cycle <= 0) {
			break;
		}
	}

gps_time_sync_exit:
	set_fs(fs);
	filp_close(filp, NULL);
}

static void qca808x_ptp_schedule_work(struct work_struct *work)
{
	struct qca808x_phy_info *pdata =
		container_of(work, struct qca808x_phy_info, ts_schedule_work.work);

	if (!pdata) {
		return;
	}
	gps_seconds_sync_thread(pdata);

	if (pdata->gps_seconds_sync_en == A_TRUE) {
		schedule_delayed_work(&pdata->ts_schedule_work, GPS_WORK_TIMEOUT);
	}
}

static void ingress_trig_time_work(struct work_struct *work)
{
	const struct qca808x_phy_info *pdata;
	struct qca808x_ptp_info *ptp_data =
		container_of(work, struct qca808x_ptp_info, ingress_trig_work.work);

	qca808x_priv *priv =
		container_of(ptp_data, qca808x_priv, ptp_info);

	pdata = priv->phy_info;

	if (!pdata) {
		return;
	}

	switch (pdata->clock_mode) {
		case FAL_P2PTC_CLOCK_MODE:
			/* p2p tc one step just use ingress trig time for pdelay resp,
			 * the ingress timestamp should be recorded in the local phy.
			 */
			ptp_ingress_time_sync(pdata->phy_addr, ptp_data->ingress_time, A_FALSE);
			break;
		case FAL_E2ETC_CLOCK_MODE:
			ptp_ingress_time_sync(pdata->phy_addr, ptp_data->ingress_time, A_TRUE);
			break;
		case FAL_OC_CLOCK_MODE:
		case FAL_BC_CLOCK_MODE:
			ptp_ingress_time_sync(pdata->phy_addr, ptp_data->ingress_time, A_FALSE);
			break;
		default:
			break;
	}

	return;
}

/******************************************************************************
*
* qca808x_ptp_settime - reset the rtc timecounter
*
* ptp: the ptp clock info structure
* ts:  the new rtc timecounter
*
*/
static int qca808x_ptp_settime(struct ptp_clock_info *ptp,
			       const struct timespec64 *ts)
{
	const struct qca808x_phy_info *pdata;
	struct qca808x_ptp_clock *clock =
		container_of(ptp, struct qca808x_ptp_clock, caps);

	fal_ptp_time_t ptp_time = {0};

	qca808x_priv *priv = clock->priv;
	pdata = priv->phy_info;

	if (!pdata) {
		return SW_FAIL;
	}
	ptp_time.seconds = ts->tv_sec;
	ptp_time.nanoseconds = ts->tv_nsec;

	mutex_lock(&clock->tsreg_lock);
	qca808x_phy_ptp_rtc_time_set(pdata->dev_id, pdata->phy_addr, &ptp_time);
	mutex_unlock(&clock->tsreg_lock);
	return 0;
}

/******************************************************************************
*
* qca808x_ptp_gettime - read the rtc timecounter
*
* ptp: the ptp clock info structure
* ts:  the timespace to hold the current rtc time
*
*/
static int qca808x_ptp_gettime(struct ptp_clock_info *ptp,
			       struct timespec64 *ts)
{
	const struct qca808x_phy_info *pdata;
	struct qca808x_ptp_clock *clock =
		container_of(ptp, struct qca808x_ptp_clock, caps);

	fal_ptp_time_t ptp_time = {0};

	qca808x_priv *priv = clock->priv;
	pdata = priv->phy_info;

	if (!pdata) {
		return SW_FAIL;
	}
	mutex_lock(&clock->tsreg_lock);
	qca808x_phy_ptp_rtc_time_get(pdata->dev_id, pdata->phy_addr, &ptp_time);
	mutex_unlock(&clock->tsreg_lock);

	ts->tv_sec = ptp_time.seconds;
	ts->tv_nsec = ptp_time.nanoseconds;
	return 0;
}

/******************************************************************************
*
* qca808x_ptp_adjtime - adjust the rtc timecounter offset
*
* ptp: the ptp clock info structure
* delta:  offset to be adjusted per cycle counter
*
*/
static int qca808x_ptp_adjtime(struct ptp_clock_info *ptp, s64 delta)
{
	struct timespec64 ts;
	const struct qca808x_phy_info *pdata;
	struct qca808x_ptp_clock *clock =
		container_of(ptp, struct qca808x_ptp_clock, caps);

	fal_ptp_time_t ptp_time = {0};

	qca808x_priv *priv = clock->priv;
	pdata = priv->phy_info;

	if (!pdata) {
		return SW_FAIL;
	}
	ts = ns_to_timespec64(delta);
	ptp_time.seconds = ts.tv_sec;
	ptp_time.nanoseconds = ts.tv_nsec;

	mutex_lock(&clock->tsreg_lock);
	qca808x_phy_ptp_rtc_adjtime_set(pdata->dev_id, pdata->phy_addr, &ptp_time);
	mutex_unlock(&clock->tsreg_lock);
	return 0;
}

static void qca808x_ppb_to_freq (a_int32_t speed, a_int32_t ppb, fal_ptp_time_t *ptp_time)
{
	a_uint64_t rate;
	a_uint16_t ns, ns_tmp;
	a_int32_t neg_adj = 0, tmp = 0;

	if (ppb < 0) {
		neg_adj = 1;
		ppb = -ppb;
	}

	rate = ppb;
	rate <<= 20;
	/* divided by (200Mhz-ppb/8)/64 */
	if (speed == FAL_SPEED_2500) {
		tmp = (ppb/5)/64;
		ns_tmp = QCA808X_PTP_TICK_RATE_200M;
		rate = div_u64(rate, 3125000 + tmp);
	} else {
		tmp = (ppb/8)/64;
		ns_tmp = QCA808X_PTP_TICK_RATE_125M;
		rate = div_u64(rate, 1953125 + tmp);
	}

	if(neg_adj && rate != 0) {
		ns = ns_tmp - 1;
		rate = (2<<26)-rate;
	} else {
		ns = ns_tmp;
	}

	/* remove the redundant bits, only 26 bits for fracnanoseconds */
	while (rate & 0xfc000000) {
		rate >>= 1;
	}

	ptp_time->seconds = 0;
	ptp_time->nanoseconds = ns;
	ptp_time->fracnanoseconds = rate;

	return;
}

static void qca808x_ptp_adjfreq_sync(a_uint32_t phy_addr,
		a_int32_t ppb, fal_ptp_time_t ptp_time_org)
{
	fal_ptp_reference_clock_t ref_clock = FAL_REF_CLOCK_LOCAL;
	fal_ptp_time_t ptp_time = {0};
	struct qca808x_phy_info *pdata = NULL;
	sw_error_t ret = SW_OK;
	a_uint32_t gm_mode = 0;

	/*
	 * In BC mode, the SYNC clock, PPS and Toduart PINs are connected,
	 * the adjust frequency should be same among the ports to guaranteeing
	 * the RTC consistent.
	 */
	list_for_each_entry(pdata, &g_qca808x_phy_list, list) {
		if (pdata->phydev_addr != phy_addr) {
			ret = qca808x_ptp_gm_conf0_reg_grandmaster_mode_get(pdata->dev_id,
					pdata->phy_addr, &gm_mode);
			/* The grandmaster mode should be configured to sync RTC */
			if (ret == SW_OK && gm_mode == PTP_REG_BIT_TRUE) {
				ptp_time = ptp_time_org;
				ret = qca808x_phy_ptp_reference_clock_get(pdata->dev_id,
						pdata->phy_addr, &ref_clock);
				/*
				 * BC ports share the same RTC clock in the external mode
				 * so the adjust frequency should be also same, otherwise
				 * the ppb should be converted to the corresponding value
				 * of the adjust frequency.
				 */
				if (ret == SW_OK && ref_clock != FAL_REF_CLOCK_EXTERNAL) {
					qca808x_ppb_to_freq(pdata->speed, ppb, &ptp_time);
				}
				qca808x_phy_ptp_rtc_adjfreq_set(pdata->dev_id,
						pdata->phy_addr, &ptp_time);
			}
		}
	}

	return;
}

/******************************************************************************
*
* qca808x_ptp_adjfreq - adjust the frequency of cycle counter
*
* ptp: the ptp clock info structure
* ppb:  parts per billion adjustment from master
*
*/
static int qca808x_ptp_adjfreq(struct ptp_clock_info *ptp, s32 ppb)
{
	const struct qca808x_phy_info *pdata;
	struct qca808x_ptp_clock *clock =
		container_of(ptp, struct qca808x_ptp_clock, caps);

	fal_ptp_time_t ptp_time = {0};

	qca808x_priv *priv = clock->priv;
	struct phy_device *phydev = priv->phydev;
	pdata = priv->phy_info;

	if (!pdata || !phydev) {
		return SW_FAIL;
	}

	qca808x_ppb_to_freq(pdata->speed, ppb, &ptp_time);

	mutex_lock(&clock->tsreg_lock);
	qca808x_phy_ptp_rtc_adjfreq_set(pdata->dev_id, pdata->phy_addr, &ptp_time);
	if (pdata->clock_mode == FAL_BC_CLOCK_MODE) {
		/* Keep RTC time consistent among BC ports */
		qca808x_ptp_adjfreq_sync(pdata->phy_addr, ppb, ptp_time);
	}
	mutex_unlock(&clock->tsreg_lock);
	return 0;
}

static int qca808x_ptp_enable(struct ptp_clock_info *ptp,
			      struct ptp_clock_request *rq, int on)
{
	return -EOPNOTSUPP;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
static int qca808x_ptp_verify(struct ptp_clock_info *ptp, unsigned int pin,
			      enum ptp_pin_function func, unsigned int chan)
{
	return 1;
}
#endif

void qca808x_ptp_change_notify(struct phy_device *phydev)
{
	fal_ptp_reference_clock_t ptp_ref_clock = FAL_REF_CLOCK_EXTERNAL;
	fal_ptp_time_t ptp_cycle_time = {0};
	a_uint32_t nanoseconds = 0;
	qca808x_priv *priv = phydev->priv;
	struct qca808x_phy_info *pdata = priv->phy_info;

	if (!pdata) {
		return;
	}

	if (pdata->speed != phydev->speed) {
		if (phydev->speed == SPEED_2500 &&
				pdata->speed < SPEED_2500) {
			/* adjust frequency to 5ns(200MHz) */
			nanoseconds = QCA808X_PTP_TICK_RATE_200M;
		} else if (pdata->speed == SPEED_2500 &&
				phydev->speed < SPEED_2500) {
			/* adjust frequency to 8ns(125MHz) */
			nanoseconds = QCA808X_PTP_TICK_RATE_125M;
		}

		if (phydev->speed == SPEED_10) {
			/* local free running clock for 10M */
			ptp_ref_clock = FAL_REF_CLOCK_LOCAL;
		} else if (pdata->speed == SPEED_10) {
			ptp_ref_clock = FAL_REF_CLOCK_SYNCE;
		}

		if (ptp_ref_clock != FAL_REF_CLOCK_EXTERNAL) {
			qca808x_phy_ptp_reference_clock_set(pdata->dev_id,
					pdata->phy_addr, ptp_ref_clock);
		}

		pdata->speed = phydev->speed;
		if (nanoseconds != 0) {
			ptp_cycle_time.nanoseconds = nanoseconds;
			qca808x_phy_ptp_rtc_adjfreq_set(pdata->dev_id,
					pdata->phy_addr, &ptp_cycle_time);
		}
	}
}

int qca808x_hwtstamp(struct phy_device *phydev, struct ifreq *ifr)
{
	struct hwtstamp_config cfg;
	a_uint32_t gm_mode = 0;
	fal_ptp_reference_clock_t ref_clock = FAL_REF_CLOCK_LOCAL;
	sw_error_t ret = SW_OK;
	fal_ptp_config_t ptp_config = {0};
	qca808x_priv *priv = phydev->priv;
	struct qca808x_phy_info *pdata = priv->phy_info;
	struct qca808x_ptp_info *ptp_info = &priv->ptp_info;
	struct qca808x_ptp_clock *clock = ptp_info->clock;

	if (!pdata || !clock) {
		return -EFAULT;
	}

	if (copy_from_user(&cfg, ifr->ifr_data, sizeof(cfg)))
		return -EFAULT;
	if (cfg.flags) /* reserved for future extensions */
		return -EINVAL;

	if (cfg.tx_type < 0 || cfg.tx_type > HWTSTAMP_TX_ONESTEP_P2P)
		return -ERANGE;

	ptp_info->hwts_tx_type = cfg.tx_type;

	switch (cfg.rx_filter) {
		case HWTSTAMP_FILTER_NONE:
			ptp_info->hwts_rx_type = PTP_CLASS_NONE;
			break;
		case HWTSTAMP_FILTER_PTP_V2_EVENT:
		case HWTSTAMP_FILTER_PTP_V2_SYNC:
		case HWTSTAMP_FILTER_PTP_V2_DELAY_REQ:
			ptp_info->hwts_rx_type = PTP_CLASS_L4 | PTP_CLASS_L2;
			break;
		case HWTSTAMP_FILTER_PTP_V2_L4_EVENT:
		case HWTSTAMP_FILTER_PTP_V2_L4_SYNC:
		case HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ:
			ptp_info->hwts_rx_type = PTP_CLASS_L4;
			break;
		case HWTSTAMP_FILTER_PTP_V2_L2_EVENT:
		case HWTSTAMP_FILTER_PTP_V2_L2_SYNC:
		case HWTSTAMP_FILTER_PTP_V2_L2_DELAY_REQ:
			ptp_info->hwts_rx_type = PTP_CLASS_L2;
			break;
		default:
			return -ERANGE;
	}

	mutex_lock(&clock->tsreg_lock);
	ptp_config.clock_mode = pdata->clock_mode;
	switch (ptp_info->hwts_tx_type) {
		case HWTSTAMP_TX_ON:
			ptp_config.ptp_en = A_TRUE;
			ptp_config.step_mode = FAL_TWO_STEP_MODE;
			break;
		case HWTSTAMP_TX_ONESTEP_SYNC:
		case HWTSTAMP_TX_ONESTEP_P2P:
			ptp_config.ptp_en = A_TRUE;
			ptp_config.step_mode = FAL_ONE_STEP_MODE;
			break;
		case HWTSTAMP_TX_OFF:
		default:
			ptp_config.ptp_en = A_FALSE;
			ptp_config.step_mode = FAL_TWO_STEP_MODE;
			break;
	}
	ret = qca808x_phy_ptp_config_set(pdata->dev_id, pdata->phy_addr, &ptp_config);
	mutex_unlock(&clock->tsreg_lock);
	if (ret != SW_OK) {
		return -EFAULT;
	}

	/*
	 * disable SYNCE clock output by default,
	 * only enabling the clock output under the
	 * BC mode && not in external reference mode
	 */
	qca808x_ptp_clock_synce_clock_enable(pdata->dev_id, pdata->phy_addr, A_FALSE);

	if (pdata->clock_mode == FAL_BC_CLOCK_MODE) {
		ret = qca808x_ptp_gm_conf0_reg_grandmaster_mode_get(pdata->dev_id,
				pdata->phy_addr, &gm_mode);
		/* The grandmaster mode should be configured to sync RTC */
		if (ret == SW_OK && gm_mode != PTP_REG_BIT_TRUE) {
			ret = qca808x_phy_ptp_reference_clock_get(pdata->dev_id,
					pdata->phy_addr, &ref_clock);
			/*
			 * The PHC should be with below PINs connected for clock synchronized
			 * so NAPA1 should be configured as sync or local reference clock,
			 * and NAPA2 is configured as FAL_REF_CLOCK_EXTERNAL & grandmaster mode.
			 *
			 * Napa1 ToD out  --- >  Napa2 ToD in.
			 * Napa1 PPS out  --- >  Napa2 PPS in.
			 * Napa1 sync clock out --- > Napa2 reference clock in.
			 */
			if (ret == SW_OK && ref_clock != FAL_REF_CLOCK_EXTERNAL) {
				/* enable SYNCE clock output */
				qca808x_ptp_clock_synce_clock_enable(pdata->dev_id,
						pdata->phy_addr, A_TRUE);
			}
		}
	}

	pdata->step_mode = ptp_config.step_mode;

	return copy_to_user(ifr->ifr_data, &cfg, sizeof(cfg)) ? -EFAULT : 0;
}

bool qca808x_rxtstamp(struct phy_device *phydev,
			     struct sk_buff *nskb, int type)
{
	struct skb_shared_hwtstamps *shhwtstamps = NULL;
	struct timespec64 ts = {0};
	a_bool_t ingress_trig_flag = A_FALSE;
	a_uint64_t ns;
	a_int64_t *correction;
	a_uint16_t *seqid;
	a_int32_t ptp_class;
	a_uint32_t *reserved2;
	a_uint8_t *reserved0, *reserved1;
	a_uint32_t *cf1;
	a_uint8_t *ptp_header;
	a_uint8_t embed_val, pkt_type;
	qca808x_ptp_cb *ptp_cb = (qca808x_ptp_cb *)nskb->cb;
	qca808x_priv *priv = phydev->priv;
	struct qca808x_phy_info *pdata = priv->phy_info;
	struct qca808x_ptp_info *ptp_info = &priv->ptp_info;

	if (ptp_info->hwts_rx_type == PTP_CLASS_NONE || !pdata) {
		return false;
	}

	/* The PTP_CLASS_NONE is passed, which indicates that the
	 * PTP class is not determined, calling ptp_classify_raw to
	 * classfy the packet.
	 */
	if (type == PTP_CLASS_NONE) {
		__skb_push(nskb, ETH_HLEN);
		/* dissecting the packet content to get ptp class */
		ptp_class = ptp_classify_raw(nskb);
		__skb_pull(nskb, ETH_HLEN);
		if (ptp_class == PTP_CLASS_NONE) {
			/* this case should not happen, only ptp event packet passed */
			SSDK_ERROR("%s: No PTP event packet received\n", __func__);
			return false;
		}
		type = ptp_class;
	}

	if ((ptp_info->hwts_rx_type & type) == PTP_CLASS_NONE) {
		return false;
	}

	ptp_header = skb_ptp_header(nskb, type);
	if (!ptp_header) {
		return false;
	}
	shhwtstamps = skb_hwtstamps(nskb);
	memset(shhwtstamps, 0, sizeof(*shhwtstamps));

#define PTP_HDR_CORRECTIONFIELD_CPY_SRC 11
#define PTP_HDR_CORRECTIONFIELD_CPY_DST 9
#define PTP_HDR_CORRECTIONFIELD_CPY_LEN 5

	reserved0 = ptp_header + PTP_HDR_RESERVED0_OFFSET;
	reserved1 = ptp_header + PTP_HDR_RESERVED1_OFFSET;
	cf1 = (a_uint32_t *)(ptp_header + PTP_HDR_CORRECTIONFIELD_OFFSET);
	correction = (a_uint64_t *)(ptp_header + PTP_HDR_CORRECTIONFIELD_OFFSET);
	seqid = (a_uint16_t *)(ptp_header + OFF_PTP_SEQUENCE_ID);
	reserved2 = (a_uint32_t *)(ptp_header + PTP_HDR_RESERVED2_OFFSET);

	embed_val = (*reserved0 & 0xf0) >> 4;
	pkt_type = *ptp_header & 0xf;

	qca808x_ptp_stat_update(pdata, FAL_RX_DIRECTION,
			QCA808X_PTP_MSG_MAX, PTP_PKT_SEQID_UNMATCHED);

	if (embed_val == QCA808X_PTP_EMBEDDED_MODE) {
		ts.tv_sec = ntohl(*reserved2);
		ts.tv_nsec = ((a_uint32_t)*reserved1 << 24) | (ntohl(*cf1) >> 8);

		if (pdata->step_mode == FAL_ONE_STEP_MODE) {
			switch (pdata->clock_mode) {
				case FAL_OC_CLOCK_MODE:
				case FAL_BC_CLOCK_MODE:
				case FAL_P2PTC_CLOCK_MODE:
					/* message sync with the timestamp inserted into the ptp
					 * header, do not use the ingress_trig_time register, the
					 * ingress time will be acquired from ptp header.
					 * the ingress time of pdealy request msg should be
					 * recorded and will be copied the corresponding pdealy
					 * response msg.
					 */
					if (pkt_type == QCA808X_PTP_MSG_PREQ) {
						ptp_info->embeded_ts.reserved0 = *reserved0;
						ptp_info->embeded_ts.reserved1 = *reserved1;
						ptp_info->embeded_ts.reserved2 = *reserved2;
						ptp_info->embeded_ts.correction = *correction;
						ptp_info->embeded_ts.seqid = *seqid;
						ptp_info->embeded_ts.msg_type = pkt_type;
					}
					break;
				case FAL_E2ETC_CLOCK_MODE:
					ingress_trig_flag = A_TRUE;
					break;
				default:
					break;
			}
			if (ingress_trig_flag == A_TRUE) {
				ptp_info->ingress_time = ts.tv_nsec;
				schedule_delayed_work(&ptp_info->ingress_trig_work, 0);
			}
		}

		/* restore the original correctionfield value except for
		 * the TC one-step mode offloading*/
		if (!(((pdata->clock_mode == FAL_P2PTC_CLOCK_MODE &&
					pkt_type == QCA808X_PTP_MSG_SYNC) ||
					pdata->clock_mode == FAL_E2ETC_CLOCK_MODE) &&
					pdata->step_mode == FAL_ONE_STEP_MODE))
		{
			/* in embeded mode for the rx time stamp, the correction field
			 * is modfied to keep the low 50 bit of nanosecond and the
			 * fractional nanoseconds should be dropped
			 */
			*reserved0 = *reserved0 & 0xf;
			memmove(ptp_header + PTP_HDR_CORRECTIONFIELD_CPY_DST,
					ptp_header + PTP_HDR_CORRECTIONFIELD_CPY_SRC,
					PTP_HDR_CORRECTIONFIELD_CPY_LEN);
			memset(ptp_header + PTP_HDR_CORRECTIONFIELD_OFFSET, 0, 1);
			memset(ptp_header + PTP_HDR_RESERVED2_OFFSET - 2, 0, 6);
		}
	} else {
		ptp_cb->ptp_type = type;
		ptp_cb->pkt_type = pkt_type;
		skb_queue_tail(&ptp_info->rx_queue, nskb);
		schedule_delayed_work(&ptp_info->rx_ts_work, 0);
		return true;
	}
	ns = timespec64_to_ns(&ts);

	qca808x_ptp_stat_update(pdata, FAL_RX_DIRECTION,
			pkt_type, PTP_PKT_SEQID_MATCHED);

	shhwtstamps->hwtstamp = ns_to_ktime(ns);
	netif_rx_ni(nskb);

	return true;
}

void qca808x_txtstamp(struct phy_device *phydev,
			     struct sk_buff *org_skb, int type)
{
	a_uint8_t msg_type;
	struct sk_buff *skb;
	qca808x_ptp_cb *ptp_cb;
	a_uint8_t *ptp_header;
	a_int64_t *correction;
	a_uint32_t *reserved2;
	a_uint8_t *reserved0, *reserved1;
	a_uint16_t *seqid;
	a_int32_t ptp_class;
	qca808x_priv *priv = phydev->priv;
	struct qca808x_ptp_info *ptp_info = &priv->ptp_info;

	/* The PTP_CLASS_NONE is passed, which indicates that the
	 * PTP class is not determined, calling ptp_classify_raw to
	 * classfy the packet.
	 */
	if (type == PTP_CLASS_NONE) {
		ptp_class = ptp_classify_raw(org_skb);
		if (ptp_class == PTP_CLASS_NONE) {
			return;
		}
		skb = skb_clone_sk(org_skb);
		if (!skb) {
			SSDK_ERROR("%s: skb_clone_sk failed\n", __func__);
			return;
		}
		type = ptp_class;
	} else {
		skb = org_skb;
	}

	ptp_header = skb_ptp_header(skb, type);
	if (!ptp_header) {
		kfree_skb(skb);
		return;
	}

	ptp_cb = (qca808x_ptp_cb *)skb->cb;
	seqid = (a_uint16_t *)(ptp_header + OFF_PTP_SEQUENCE_ID);
	reserved0 = ptp_header + PTP_HDR_RESERVED0_OFFSET;
	reserved1 = ptp_header + PTP_HDR_RESERVED1_OFFSET;
	reserved2 = (a_uint32_t *)(ptp_header + PTP_HDR_RESERVED2_OFFSET);
	correction = (a_uint64_t *)(ptp_header + PTP_HDR_CORRECTIONFIELD_OFFSET);
	msg_type = *ptp_header & 0xf;
	switch (ptp_info->hwts_tx_type) {
		case HWTSTAMP_TX_ONESTEP_SYNC:
			if (msg_type == QCA808X_PTP_MSG_SYNC) {
				skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS;
				kfree_skb(skb);
				return;
			}
			break;
		case HWTSTAMP_TX_ONESTEP_P2P:
			switch (msg_type) {
				case QCA808X_PTP_MSG_PRESP:
					if (ptp_info->embeded_ts.seqid == *seqid &&
							ptp_info->embeded_ts.msg_type ==
							QCA808X_PTP_MSG_PREQ) {
						*reserved0 = ptp_info->embeded_ts.reserved0;
						*reserved1 = ptp_info->embeded_ts.reserved1;
						*reserved2 = ptp_info->embeded_ts.reserved2;
						*correction = ptp_info->embeded_ts.correction;
					}
					/* fall down */
				case QCA808X_PTP_MSG_SYNC:
					skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS;
					kfree_skb(skb);
					return;
				default:
					break;
			}
			break;
		case HWTSTAMP_TX_ON:
			break;
			/* enqueue skb to get tx timestamp */
		case HWTSTAMP_TX_OFF:
		default:
			kfree_skb(skb);
			return;
	}

	skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS;
	skb_queue_tail(&ptp_info->tx_queue, skb);
	ptp_cb->ptp_type = type;
	qca808x_ptp_stat_update(priv->phy_info, FAL_TX_DIRECTION,
			QCA808X_PTP_MSG_MAX, PTP_PKT_SEQID_UNMATCHED);
	schedule_delayed_work(&ptp_info->tx_ts_work, 0);
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
int qca808x_ts_info(struct phy_device *phydev,
		struct ethtool_ts_info *info)
{
	qca808x_priv *priv = phydev->priv;
	struct qca808x_ptp_info *ptp_info = &priv->ptp_info;
	struct qca808x_ptp_clock *clock = ptp_info->clock;

	if (clock) {
		info->phc_index = ptp_clock_index(clock->ptp_clock);
	}

	info->so_timestamping =
		SOF_TIMESTAMPING_TX_HARDWARE |
		SOF_TIMESTAMPING_RX_HARDWARE |
		SOF_TIMESTAMPING_RAW_HARDWARE;
	info->tx_types =
		(1 << HWTSTAMP_TX_OFF) |
		(1 << HWTSTAMP_TX_ON) |
		(1 << HWTSTAMP_TX_ONESTEP_SYNC) |
		(1 << HWTSTAMP_TX_ONESTEP_P2P);

	info->rx_filters =
		(1 << HWTSTAMP_FILTER_NONE) |
		(1 << HWTSTAMP_FILTER_PTP_V2_L4_EVENT) |
		(1 << HWTSTAMP_FILTER_PTP_V2_L2_EVENT) |
		(1 << HWTSTAMP_FILTER_PTP_V2_EVENT);

	return 0;
}
#endif

static int qca808x_ptp_register(struct phy_device *phydev)
{
	int err;
	struct qca808x_ptp_clock *clock;
	qca808x_priv *priv = phydev->priv;
	struct qca808x_ptp_info *ptp_info = &priv->ptp_info;

	clock = kzalloc(sizeof(struct qca808x_ptp_clock), GFP_KERNEL);
	if (!clock) {
		return -ENOMEM;
	}

	mutex_init(&clock->tsreg_lock);
	clock->caps.owner = THIS_MODULE;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0))
	snprintf(clock->caps.name, sizeof(clock->caps.name), "qca808x timer %x", phydev->mdio.addr);
#else
	snprintf(clock->caps.name, sizeof(clock->caps.name), "qca808x timer %x", phydev->addr);
#endif
	clock->caps.max_adj	= 3124999;
	clock->caps.n_alarm	= 0;
	clock->caps.n_ext_ts	= 6;
	clock->caps.n_per_out	= 7;
	clock->caps.pps		= 0;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	clock->caps.n_pins	= 0;
	clock->caps.verify	= qca808x_ptp_verify;
	clock->caps.gettime64	= qca808x_ptp_gettime;
	clock->caps.settime64	= qca808x_ptp_settime;
#else
	clock->caps.gettime	= qca808x_ptp_gettime;
	clock->caps.settime	= qca808x_ptp_settime;
#endif
	clock->caps.adjfreq	= qca808x_ptp_adjfreq;
	clock->caps.adjtime	= qca808x_ptp_adjtime;
	clock->caps.enable	= qca808x_ptp_enable;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0))
	clock->ptp_clock = ptp_clock_register(&clock->caps, &phydev->mdio.dev);
#else
	clock->ptp_clock = ptp_clock_register(&clock->caps, &phydev->dev);
#endif
	if (IS_ERR(clock->ptp_clock)) {
		err = PTR_ERR(clock->ptp_clock);
		kfree(clock);
		return err;
	}
	ptp_info->clock = clock;
	clock->priv = priv;

	SSDK_INFO("qca808x ptp clock registered\n");
	return 0;
}

static void qca808x_ptp_unregister(struct phy_device *phydev)
{
	qca808x_priv *priv = phydev->priv;
	struct qca808x_ptp_info *ptp_info = &priv->ptp_info;
	struct qca808x_ptp_clock *clock = ptp_info->clock;

	if (clock) {
		ptp_clock_unregister(clock->ptp_clock);
		mutex_destroy(&clock->tsreg_lock);
		kfree(clock);
	}
}

int qca808x_ptp_init(qca808x_priv *priv)
{
	int err;
	struct qca808x_ptp_info *ptp_info;
	struct qca808x_phy_info *pdata;

	if (!priv) {
		return -1;
	}

	ptp_info = &priv->ptp_info;
	pdata = priv->phy_info;
	INIT_DELAYED_WORK(&ptp_info->tx_ts_work, tx_timestamp_work);
	INIT_DELAYED_WORK(&ptp_info->rx_ts_work, rx_timestamp_work);
	skb_queue_head_init(&ptp_info->tx_queue);
	skb_queue_head_init(&ptp_info->rx_queue);

	INIT_DELAYED_WORK(&ptp_info->ingress_trig_work, ingress_trig_time_work);
	INIT_DELAYED_WORK(&pdata->ts_schedule_work, qca808x_ptp_schedule_work);

	err = qca808x_ptp_register(priv->phydev);
	if (err <0) {
		SSDK_ERROR("qca808x ptp clock register failed\n");
		kfree(ptp_info);
		return err;
	}

	return err;
}

void qca808x_ptp_deinit(qca808x_priv *priv)
{
	struct qca808x_ptp_info *ptp_info;
	struct qca808x_phy_info *pdata;
	if (!priv) {
		return;
	}

	ptp_info = &priv->ptp_info;
	pdata = priv->phy_info;
	cancel_delayed_work_sync(&ptp_info->tx_ts_work);
	cancel_delayed_work_sync(&ptp_info->rx_ts_work);
	cancel_delayed_work_sync(&ptp_info->ingress_trig_work);
	cancel_delayed_work_sync(&pdata->ts_schedule_work);
	skb_queue_purge(&ptp_info->tx_queue);
	skb_queue_purge(&ptp_info->rx_queue);

	qca808x_ptp_unregister(priv->phydev);
}
