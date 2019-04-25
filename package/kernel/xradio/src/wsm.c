/*
 * WSM host interfaces for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/skbuff.h>
#include <linux/wait.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/random.h>

#include "xradio.h"
#include "wsm.h"
#include "bh.h"
#include "ap.h"
#include "sta.h"
#include "rx.h"

#define WSM_CMD_TIMEOUT		(2 * HZ) /* With respect to interrupt loss */
#define WSM_CMD_JOIN_TIMEOUT	(7 * HZ) /* Join timeout is 5 sec. in FW   */
#define WSM_CMD_START_TIMEOUT	(7 * HZ)
#define WSM_CMD_RESET_TIMEOUT	(3 * HZ) /* 2 sec. timeout was observed.   */
#define WSM_CMD_DEFAULT_TIMEOUT	(3 * HZ)
#define WSM_SKIP(buf, size)						\
	do {								\
		if (unlikely((buf)->data + size > (buf)->end))		\
			goto underflow;					\
		(buf)->data += size;					\
	} while (0)

#define WSM_GET(buf, ptr, size)						\
	do {								\
		if (unlikely((buf)->data + size > (buf)->end))		\
			goto underflow;					\
		memcpy(ptr, (buf)->data, size);				\
		(buf)->data += size;					\
	} while (0)

#define __WSM_GET(buf, type, cvt)					\
	({								\
		type val;						\
		if (unlikely((buf)->data + sizeof(type) > (buf)->end))	\
			goto underflow;					\
		val = cvt(*(type *)(buf)->data);			\
		(buf)->data += sizeof(type);				\
		val;							\
	})

#define WSM_GET8(buf)  __WSM_GET(buf, u8, (u8))
#define WSM_GET16(buf) __WSM_GET(buf, u16, __le16_to_cpu)
#define WSM_GET32(buf) __WSM_GET(buf, u32, __le32_to_cpu)

#define WSM_PUT(buf, ptr, size)						\
	do {								\
		if (unlikely((buf)->data + size > (buf)->end))		\
			if (unlikely(wsm_buf_reserve((buf), size)))	\
				goto nomem;				\
		memcpy((buf)->data, ptr, size);				\
		(buf)->data += size;					\
	} while (0)

#define __WSM_PUT(buf, val, type, cvt)					\
	do {								\
		if (unlikely((buf)->data + sizeof(type) > (buf)->end))	\
			if (unlikely(wsm_buf_reserve((buf), sizeof(type)))) \
				goto nomem;				\
		*(type *)(buf)->data = cvt(val);			\
		(buf)->data += sizeof(type);				\
	} while (0)

#define WSM_PUT8(buf, val)  __WSM_PUT(buf, val, u8, (u8))
#define WSM_PUT16(buf, val) __WSM_PUT(buf, val, u16, __cpu_to_le16)
#define WSM_PUT32(buf, val) __WSM_PUT(buf, val, u32, __cpu_to_le32)

static void wsm_buf_reset(struct wsm_buf *buf);
static int wsm_buf_reserve(struct wsm_buf *buf, size_t extra_size);
static int get_interface_id_scanning(struct xradio_common *hw_priv);

static int wsm_cmd_send(struct xradio_common *hw_priv,
			struct wsm_buf *buf,
			void *arg, u16 cmd, long tmo, int if_id);

static struct xradio_vif
	*wsm_get_interface_for_tx(struct xradio_common *hw_priv);

static inline void wsm_cmd_lock(struct xradio_common *hw_priv)
{
	mutex_lock(&hw_priv->wsm_cmd_mux);
}

static inline void wsm_cmd_unlock(struct xradio_common *hw_priv)
{
	mutex_unlock(&hw_priv->wsm_cmd_mux);
}

static inline void wsm_oper_lock(struct xradio_common *hw_priv)
{
	mutex_lock(&hw_priv->wsm_oper_lock);
}

static inline void wsm_oper_unlock(struct xradio_common *hw_priv)
{
	mutex_unlock(&hw_priv->wsm_oper_lock);
}

/* ******************************************************************** */
/* WSM API implementation						*/

static int wsm_generic_confirm(struct xradio_common *hw_priv,
			     void *arg,
			     struct wsm_buf *buf)
{
	u32 status = WSM_GET32(buf);
	if (status != WSM_STATUS_SUCCESS)
		return -EINVAL;
	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

#ifdef XR_RRM//RadioResourceMeasurement
static int wsm_start_measure_requset(struct xradio_common *hw_priv,
		                                MEASUREMENT_PARAMETERS *arg,
		                                              int  if_id)
{
		int ret;
		struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

		wsm_cmd_lock(hw_priv);

        WSM_PUT(buf, arg, sizeof(*arg));
		ret = wsm_cmd_send(hw_priv, buf, arg, 0x000E, WSM_CMD_TIMEOUT, if_id);

		wsm_cmd_unlock(hw_priv);
		return ret;

	nomem:
		wsm_cmd_unlock(hw_priv);
		return -ENOMEM;

}

int wsm_11k_measure_requset(struct xradio_common  *hw_priv,
                                               u8  measure_type,
                                              u16  ChannelNum,
                                              u16  Duration)
{
    int ret;
    u8 type, sub_type;
    MEASUREMENT_PARAMETERS rrm_paras;
    LMAC_MEAS_REQUEST *rrm_req = &rrm_paras.MeasurementRequest;
//    LMAC_MEAS_CHANNEL_LOAD_PARAMS *rrm_req = &rrm_paras.MeasurementRequest;
    rrm_paras.TxPowerLevel = 0x11;
    rrm_paras.DurationMandatory = 0x22;
    rrm_paras.MeasurementRequestLength = 0x33;
    
    type     = (measure_type&0xf0)>>4;
    sub_type =  measure_type&0xf;
    rrm_paras.MeasurementType = type;
//    if (measure_type == ChannelLoadMeasurement) {
    if (type == ChannelLoadMeasurement) {
        rrm_req->ChannelLoadParams.Reserved = 0;
        rrm_req->ChannelLoadParams.ChannelLoadCCA = sub_type;
        rrm_req->ChannelLoadParams.ChannelNum = ChannelNum;
        //valid when channelload measure, interval bettween request&start
        rrm_req->ChannelLoadParams.RandomInterval = 0;
        //unit:1TU=1024us
        rrm_req->ChannelLoadParams.MeasurementDuration = Duration;
        rrm_req->ChannelLoadParams.MeasurementStartTimel = 0;
        rrm_req->ChannelLoadParams.MeasurementStartTimeh = 0;
    } else if (type == NoiseHistrogramMeasurement) {
        rrm_req->NoisHistogramParams.Reserved = 0;
        rrm_req->NoisHistogramParams.IpiRpi = sub_type;
        rrm_req->NoisHistogramParams.ChannelNum = ChannelNum;
        rrm_req->NoisHistogramParams.RandomInterval = 0;
        rrm_req->NoisHistogramParams.MeasurementDuration = Duration;
        rrm_req->NoisHistogramParams.MeasurementStartTimel = 0;
        rrm_req->NoisHistogramParams.MeasurementStartTimeh = 0;
    }
    ret = wsm_start_measure_requset(hw_priv, &rrm_paras, 0);
    
    return ret;
}


#endif//RadioResourceMeasurement
int wsm_configuration(struct xradio_common *hw_priv,
		      struct wsm_configuration *arg,
		      int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_cmd_lock(hw_priv);

	WSM_PUT32(buf, arg->dot11MaxTransmitMsduLifeTime);
	WSM_PUT32(buf, arg->dot11MaxReceiveLifeTime);
	WSM_PUT32(buf, arg->dot11RtsThreshold);

	/* DPD block. */
	WSM_PUT16(buf, arg->dpdData_size + 12);
	WSM_PUT16(buf, 1); /* DPD version */
	WSM_PUT(buf, arg->dot11StationId, ETH_ALEN);
	WSM_PUT16(buf, 5); /* DPD flags */
	WSM_PUT(buf, arg->dpdData, arg->dpdData_size);

	ret = wsm_cmd_send(hw_priv, buf, arg, 0x0009, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

static int wsm_configuration_confirm(struct xradio_common *hw_priv,
				     struct wsm_configuration *arg,
				     struct wsm_buf *buf)
{
	int i;
	int status;

	status = WSM_GET32(buf);
	if (WARN_ON(status != WSM_STATUS_SUCCESS))
		return -EINVAL;

	WSM_GET(buf, arg->dot11StationId, ETH_ALEN);
	arg->dot11FrequencyBandsSupported = WSM_GET8(buf);
	WSM_SKIP(buf, 1);
	arg->supportedRateMask = WSM_GET32(buf);
	for (i = 0; i < 2; ++i) {
		arg->txPowerRange[i].min_power_level = WSM_GET32(buf);
		arg->txPowerRange[i].max_power_level = WSM_GET32(buf);
		arg->txPowerRange[i].stepping = WSM_GET32(buf);
	}
	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

void wsm_query_work(struct work_struct *work)
{
	struct xradio_common *hw_priv =
		container_of(work, struct xradio_common, query_work);
	u8 ret[100] = {0};


	*(u32*)&ret[0] = hw_priv->query_packetID;
	wsm_read_mib(hw_priv, WSM_MIB_ID_REQ_PKT_STATUS, (void*)&ret[0], sizeof(ret), 4);
	if(!ret[4]) {
		wsm_printk(XRADIO_DBG_ERROR,"QuerypktID=0x%08x, status=0x%x, retry=%d, flags=0x%x, PktDebug=0x%x\n" \
		           "pktqueue=0x%x, ext1=%d, ext2=%d, ext3=%d, ext4=0x%x, ext5=0x%x\n",
		           *(u32*)&ret[0], ret[6], ret[7], *(u32*)&ret[8], *(u32*)&ret[12],
		           ret[44], ret[45], ret[46], ret[47], ret[48], ret[49]);
		wsm_printk(XRADIO_DBG_ERROR,"interdebug=0x%x, 0x%x, 0x%x, Soure=0x%x, 0x%x, 0x%x\n" \
		           "interuse=%d, external=%d, TxOutstanding=%d, QueueStatus=0x%x, BA0=0x%x, BA1=0x%x\n" \
		           "ScanStatus=0x%x, scanNULL=0x%x, wr_state=0x%x,0x%x,0x%x,0x%x," \
		           "wr_cnt=%d, %d, %d, %d\n",
		           *(u32*)&ret[16], *(u32*)&ret[20], *(u32*)&ret[24], ret[28], ret[29], ret[30],
		           ret[32], ret[33], ret[34], ret[35], *(u32*)&ret[36], *(u32*)&ret[40],
		           ret[50], ret[51], ret[52], ret[53], ret[54], ret[55],
		           *(u16*)&ret[56], *(u16*)&ret[58], *(u16*)&ret[60], *(u16*)&ret[62]);
	} else {
		ret[5] = 0;
		wsm_printk(XRADIO_DBG_ERROR,"No req packid=0x%08x!\n", *(u32*)&ret[0]);
	}
	//hardware error occurs, try to restart wifi.
	if(ret[5] & 0x4) {
		wsm_printk(XRADIO_DBG_ERROR,"Hardware need to reset 0x%x.\n", ret[5]);
		hw_priv->bh_error = 1;
		wake_up(&hw_priv->bh_wq);
	}
	hw_priv->query_packetID = 0;
}

/* ******************************************************************** */

int wsm_reset(struct xradio_common *hw_priv, const struct wsm_reset *arg,
		int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	u16 cmd = 0x000A | WSM_TX_LINK_ID(arg->link_id);

	wsm_cmd_lock(hw_priv);

	WSM_PUT32(buf, arg->reset_statistics ? 0 : 1);
	ret = wsm_cmd_send(hw_priv, buf, NULL, cmd, WSM_CMD_RESET_TIMEOUT,
				if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

struct wsm_mib {
	u16 mibId;
	void *buf;
	size_t buf_size;
};

int wsm_read_mib(struct xradio_common *hw_priv, u16 mibId, void *_buf,
			size_t buf_size, size_t arg_size)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	struct wsm_mib mib_buf = {
		.mibId = mibId,
		.buf = _buf,
		.buf_size = buf_size,
	};
	wsm_cmd_lock(hw_priv);

	WSM_PUT16(buf, mibId);
	WSM_PUT16(buf, arg_size);
	WSM_PUT(buf, _buf, arg_size);

	ret = wsm_cmd_send(hw_priv, buf, &mib_buf, 0x0005, WSM_CMD_TIMEOUT, -1);
	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

static int wsm_read_mib_confirm(struct xradio_common *hw_priv,
				struct wsm_mib *arg,
				struct wsm_buf *buf)
{
	u16 size;
	if (WARN_ON(WSM_GET32(buf) != WSM_STATUS_SUCCESS))
		return -EINVAL;

	if (WARN_ON(WSM_GET16(buf) != arg->mibId))
		return -EINVAL;

	size = WSM_GET16(buf);
	if (size > arg->buf_size)
		size = arg->buf_size;

	WSM_GET(buf, arg->buf, size);
	arg->buf_size = size;
	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

/* ******************************************************************** */

int wsm_write_mib(struct xradio_common *hw_priv, u16 mibId, void *_buf,
			size_t buf_size, int if_id)
{
	int ret = 0;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	struct wsm_mib mib_buf = {
		.mibId = mibId,
		.buf = _buf,
		.buf_size = buf_size,
	};

	wsm_cmd_lock(hw_priv);

	WSM_PUT16(buf, mibId);
	WSM_PUT16(buf, buf_size);
	WSM_PUT(buf, _buf, buf_size);

	ret = wsm_cmd_send(hw_priv, buf, &mib_buf, 0x0006, WSM_CMD_TIMEOUT,
			if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

static int wsm_write_mib_confirm(struct xradio_common *hw_priv,
				struct wsm_mib *arg,
				struct wsm_buf *buf,
				int interface_link_id)
{
	int ret;
	int i;
	struct xradio_vif *priv;
	ret = wsm_generic_confirm(hw_priv, arg, buf);
	if (ret)
		return ret;

	/*wsm_set_operational_mode confirm.*/
	if (arg->mibId == 0x1006) {
		const char *p = arg->buf;
		bool powersave_enabled = (p[0] & 0x0F) ? true : false;

		/* update vif PM status. */
		priv = xrwl_hwpriv_to_vifpriv(hw_priv, interface_link_id);
		if (priv) {
			xradio_enable_powersave(priv, powersave_enabled);
			spin_unlock(&priv->vif_lock);
		}

		/* HW powersave base on vif except for generic vif. */
		spin_lock(&hw_priv->vif_list_lock);
		xradio_for_each_vif(hw_priv, priv, i) {
			if (!priv)
				continue;
			powersave_enabled &= !!priv->powersave_enabled;
		}
		hw_priv->powersave_enabled = powersave_enabled;
		spin_unlock(&hw_priv->vif_list_lock);

	}
	return 0;
}

/* ******************************************************************** */

int wsm_scan(struct xradio_common *hw_priv, const struct wsm_scan *arg,
		int if_id)
{
	int i;
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	if (unlikely(arg->numOfChannels > 48))
		return -EINVAL;

	if (unlikely(arg->numOfSSIDs > WSM_SCAN_MAX_NUM_OF_SSIDS))
		return -EINVAL;

	if (unlikely(arg->band > 1))
		return -EINVAL;

	wsm_oper_lock(hw_priv);
	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, arg->band);
	WSM_PUT8(buf, arg->scanType);
	WSM_PUT8(buf, arg->scanFlags);
	WSM_PUT8(buf, arg->maxTransmitRate);
	WSM_PUT32(buf, arg->autoScanInterval);
	WSM_PUT8(buf, arg->numOfProbeRequests);
	WSM_PUT8(buf, arg->numOfChannels);
	WSM_PUT8(buf, arg->numOfSSIDs);
	WSM_PUT8(buf, arg->probeDelay);

	for (i = 0; i < arg->numOfChannels; ++i) {
		WSM_PUT16(buf, arg->ch[i].number);
		WSM_PUT16(buf, 0);
		WSM_PUT32(buf, arg->ch[i].minChannelTime);
		WSM_PUT32(buf, arg->ch[i].maxChannelTime);
		WSM_PUT32(buf, 0);
	}

	for (i = 0; i < arg->numOfSSIDs; ++i) {
		WSM_PUT32(buf, arg->ssids[i].length);
		WSM_PUT(buf, &arg->ssids[i].ssid[0],
				sizeof(arg->ssids[i].ssid));
	}

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0007, WSM_CMD_TIMEOUT,
			   if_id);
	wsm_cmd_unlock(hw_priv);
	if (ret)
		wsm_oper_unlock(hw_priv);

	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	wsm_oper_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_stop_scan(struct xradio_common *hw_priv, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	wsm_cmd_lock(hw_priv);
	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0008, WSM_CMD_TIMEOUT,
			   if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;
}


static int wsm_tx_confirm(struct xradio_common *hw_priv,
			  struct wsm_buf *buf,
			  int interface_link_id)
{
	struct wsm_tx_confirm tx_confirm;

	tx_confirm.packetID = WSM_GET32(buf);
	tx_confirm.status = WSM_GET32(buf);
	tx_confirm.txedRate = WSM_GET8(buf);
	tx_confirm.ackFailures = WSM_GET8(buf);
	tx_confirm.flags = WSM_GET16(buf);
	tx_confirm.rate_try[0] = WSM_GET32(buf);
	tx_confirm.rate_try[1] = WSM_GET32(buf);
	tx_confirm.rate_try[2] = WSM_GET32(buf);
	tx_confirm.mediaDelay = WSM_GET32(buf);
	tx_confirm.txQueueDelay = WSM_GET32(buf);

	if (is_hardware_xradio(hw_priv)) {
		/* TODO:COMBO:linkID will be stored in packetID*/
		/* TODO:COMBO: Extract traffic resumption map */
		tx_confirm.if_id = xradio_queue_get_if_id(tx_confirm.packetID);
		tx_confirm.link_id = xradio_queue_get_link_id(
				tx_confirm.packetID);
	} else {
		tx_confirm.link_id = interface_link_id;
		tx_confirm.if_id = 0;
	}

	wsm_release_vif_tx_buffer(hw_priv, tx_confirm.if_id, 1);

	xradio_tx_confirm_cb(hw_priv, &tx_confirm);
	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

static int wsm_multi_tx_confirm(struct xradio_common *hw_priv,
				struct wsm_buf *buf, int interface_link_id)
{
	struct xradio_vif *priv;
	int ret;
	int count;
	int i;

	count = WSM_GET32(buf);
	if (WARN_ON(count <= 0))
		return -EINVAL;
	else if (count > 1) {
		ret = wsm_release_tx_buffer(hw_priv, count - 1);
		if (ret < 0)
			return ret;
		else if (ret > 0)
			xradio_bh_wakeup(hw_priv);
	}
	priv = xrwl_hwpriv_to_vifpriv(hw_priv, interface_link_id);
	if (priv) {
		spin_unlock(&priv->vif_lock);
	}
	for (i = 0; i < count; ++i) {
		ret = wsm_tx_confirm(hw_priv, buf, interface_link_id);
		if (ret)
			return ret;
	}
	return ret;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

/* ******************************************************************** */

static int wsm_join_confirm(struct xradio_common *hw_priv,
			    struct wsm_join *arg,
			    struct wsm_buf *buf)
{
	if (WSM_GET32(buf) != WSM_STATUS_SUCCESS)
		return -EINVAL;
	arg->minPowerLevel = WSM_GET32(buf);
	arg->maxPowerLevel = WSM_GET32(buf);

	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

int wsm_join(struct xradio_common *hw_priv, struct wsm_join *arg,
	     int if_id)
/*TODO: combo: make it work per vif.*/
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_oper_lock(hw_priv);
	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, arg->mode);
	WSM_PUT8(buf, arg->band);
	WSM_PUT16(buf, arg->channelNumber);
	WSM_PUT(buf, &arg->bssid[0], sizeof(arg->bssid));
	WSM_PUT16(buf, arg->atimWindow);
	WSM_PUT8(buf, arg->preambleType);
	WSM_PUT8(buf, arg->probeForJoin);
	WSM_PUT8(buf, arg->dtimPeriod);
	WSM_PUT8(buf, arg->flags);
	WSM_PUT32(buf, arg->ssidLength);
	WSM_PUT(buf, &arg->ssid[0], sizeof(arg->ssid));
	WSM_PUT32(buf, arg->beaconInterval);
	WSM_PUT32(buf, arg->basicRateSet);

	hw_priv->tx_burst_idx = -1;
	ret = wsm_cmd_send(hw_priv, buf, arg, 0x000B, WSM_CMD_JOIN_TIMEOUT,
			   if_id);
	wsm_cmd_unlock(hw_priv);
	wsm_oper_unlock(hw_priv); /*confirm, not indcation.*/
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	wsm_oper_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_set_bss_params(struct xradio_common *hw_priv,
			const struct wsm_set_bss_params *arg,
			int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, 0);
	WSM_PUT8(buf, arg->beaconLostCount);
	WSM_PUT16(buf, arg->aid);
	WSM_PUT32(buf, arg->operationalRateSet);

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0011, WSM_CMD_TIMEOUT,
			if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_add_key(struct xradio_common *hw_priv, const struct wsm_add_key *arg,
			int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_cmd_lock(hw_priv);

	WSM_PUT(buf, arg, sizeof(*arg));

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x000C, WSM_CMD_TIMEOUT,
				if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_remove_key(struct xradio_common *hw_priv,
		   const struct wsm_remove_key *arg, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, arg->entryIndex);
	WSM_PUT8(buf, 0);
	WSM_PUT16(buf, 0);

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x000D, WSM_CMD_TIMEOUT,
			   if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_set_tx_queue_params(struct xradio_common *hw_priv,
				const struct wsm_set_tx_queue_params *arg,
				u8 id, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	u8 queue_id_to_wmm_aci[] = {3, 2, 0, 1};

	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, queue_id_to_wmm_aci[id]);
	WSM_PUT8(buf, 0);
	WSM_PUT8(buf, arg->ackPolicy);
	WSM_PUT8(buf, 0);
	WSM_PUT32(buf, arg->maxTransmitLifetime);
	WSM_PUT16(buf, arg->allowedMediumTime);
	WSM_PUT16(buf, 0);

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0012, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_set_edca_params(struct xradio_common *hw_priv,
				const struct wsm_edca_params *arg,
				int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_cmd_lock(hw_priv);

	/* Implemented according to specification. */

	WSM_PUT16(buf, arg->params[3].cwMin);
	WSM_PUT16(buf, arg->params[2].cwMin);
	WSM_PUT16(buf, arg->params[1].cwMin);
	WSM_PUT16(buf, arg->params[0].cwMin);

	WSM_PUT16(buf, arg->params[3].cwMax);
	WSM_PUT16(buf, arg->params[2].cwMax);
	WSM_PUT16(buf, arg->params[1].cwMax);
	WSM_PUT16(buf, arg->params[0].cwMax);

	WSM_PUT8(buf, arg->params[3].aifns);
	WSM_PUT8(buf, arg->params[2].aifns);
	WSM_PUT8(buf, arg->params[1].aifns);
	WSM_PUT8(buf, arg->params[0].aifns);

	WSM_PUT16(buf, arg->params[3].txOpLimit);
	WSM_PUT16(buf, arg->params[2].txOpLimit);
	WSM_PUT16(buf, arg->params[1].txOpLimit);
	WSM_PUT16(buf, arg->params[0].txOpLimit);

	WSM_PUT32(buf, arg->params[3].maxReceiveLifetime);
	WSM_PUT32(buf, arg->params[2].maxReceiveLifetime);
	WSM_PUT32(buf, arg->params[1].maxReceiveLifetime);
	WSM_PUT32(buf, arg->params[0].maxReceiveLifetime);

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0013, WSM_CMD_TIMEOUT, if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_switch_channel(struct xradio_common *hw_priv,
		       const struct wsm_switch_channel *arg,
		       int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_lock_tx(hw_priv);
	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, arg->channelMode);
	WSM_PUT8(buf, arg->channelSwitchCount);
	WSM_PUT16(buf, arg->newChannelNumber);

	hw_priv->channel_switch_in_progress = 1;

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0016, WSM_CMD_TIMEOUT, if_id);
	wsm_cmd_unlock(hw_priv);
	if (ret) {
		wsm_unlock_tx(hw_priv);
		hw_priv->channel_switch_in_progress = 0;
	}
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	wsm_unlock_tx(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_set_pm(struct xradio_common *hw_priv, const struct wsm_set_pm *arg,
		int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_oper_lock(hw_priv);

	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, arg->pmMode);
	WSM_PUT8(buf, arg->fastPsmIdlePeriod);
	WSM_PUT8(buf, arg->apPsmChangePeriod);
	WSM_PUT8(buf, arg->minAutoPsPollPeriod);

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0010, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	if (ret)
		wsm_oper_unlock(hw_priv);

	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	wsm_oper_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_start(struct xradio_common *hw_priv, const struct wsm_start *arg,
		int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, arg->mode);
	WSM_PUT8(buf, arg->band);
	WSM_PUT16(buf, arg->channelNumber);
	WSM_PUT32(buf, arg->CTWindow);
	WSM_PUT32(buf, arg->beaconInterval);
	WSM_PUT8(buf, arg->DTIMPeriod);
	WSM_PUT8(buf, arg->preambleType);
	WSM_PUT8(buf, arg->probeDelay);
	WSM_PUT8(buf, arg->ssidLength);
	WSM_PUT(buf, arg->ssid, sizeof(arg->ssid));
	WSM_PUT32(buf, arg->basicRateSet);

	hw_priv->tx_burst_idx = -1;
	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0017, WSM_CMD_START_TIMEOUT,
			if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

#if 0
/* This API is no longer present in WSC */
/* ******************************************************************** */

int wsm_beacon_transmit(struct xradio_common *hw_priv,
			const struct wsm_beacon_transmit *arg,
			int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_cmd_lock(hw_priv);

	WSM_PUT32(buf, arg->enableBeaconing ? 1 : 0);

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0018, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}
#endif

/* ******************************************************************** */

int wsm_start_find(struct xradio_common *hw_priv, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_cmd_lock(hw_priv);
	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0019, WSM_CMD_TIMEOUT, if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;
}

/* ******************************************************************** */

int wsm_stop_find(struct xradio_common *hw_priv, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_cmd_lock(hw_priv);
	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x001A, WSM_CMD_TIMEOUT, if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;
}

/* ******************************************************************** */

int wsm_map_link(struct xradio_common *hw_priv, const struct wsm_map_link *arg,
		int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	u16 cmd = 0x001C;

	wsm_cmd_lock(hw_priv);

	WSM_PUT(buf, &arg->mac_addr[0], sizeof(arg->mac_addr));

	if (is_hardware_xradio(hw_priv)) {
		WSM_PUT8(buf, arg->unmap);
		WSM_PUT8(buf, arg->link_id);
	} else {
		cmd |= WSM_TX_LINK_ID(arg->link_id);
		WSM_PUT16(buf, 0);
	}

	ret = wsm_cmd_send(hw_priv, buf, NULL, cmd, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_update_ie(struct xradio_common *hw_priv,
		  const struct wsm_update_ie *arg, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_cmd_lock(hw_priv);

	WSM_PUT16(buf, arg->what);
	WSM_PUT16(buf, arg->count);
	WSM_PUT(buf, arg->ies, arg->length);

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x001B, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;

}
/* ******************************************************************** */
#ifdef MCAST_FWDING
/* 3.66 */
static int wsm_give_buffer_confirm(struct xradio_common *hw_priv,
                            struct wsm_buf *buf)
{
	wsm_printk(XRADIO_DBG_MSG, "HW Buf count %d\n", hw_priv->hw_bufs_used);
	if (!hw_priv->hw_bufs_used)
		wake_up(&hw_priv->bh_evt_wq);

	return 0;
}

/* 3.65 */
int wsm_init_release_buffer_request(struct xradio_common *hw_priv, u8 index)
{
	struct wsm_buf *buf = &hw_priv->wsm_release_buf[index];
	u16 cmd = 0x0022; /* Buffer Request */
	u8 flags;
	size_t buf_len;

	wsm_buf_init(buf);

	flags = index ? 0: 0x1;

	WSM_PUT8(buf, flags);
	WSM_PUT8(buf, 0);
	WSM_PUT16(buf, 0);

	buf_len = buf->data - buf->begin;

	/* Fill HI message header */
	((__le16 *)buf->begin)[0] = __cpu_to_le16(buf_len);
	((__le16 *)buf->begin)[1] = __cpu_to_le16(cmd);

	return 0;
nomem:
	return -ENOMEM;
}

/* 3.65 fixed memory leakage by yangfh*/
int wsm_deinit_release_buffer(struct xradio_common *hw_priv)
{
	struct wsm_buf *buf = NULL;
	int i, err = 0;
	
	for (i = 0; i < WSM_MAX_BUF; i++) {
		buf = &hw_priv->wsm_release_buf[i];
		if(likely(buf)) {
			if(likely(buf->begin))
				kfree(buf->begin);
			buf->begin = buf->data = buf->end = NULL;
		} else {
			err++;
		}
	}
	if(err) wsm_printk(XRADIO_DBG_ERROR, "%s, NULL buf=%d!\n", __func__, err);
	return 0;
}

/* 3.68 */
static int wsm_request_buffer_confirm(struct xradio_vif *priv,
                            u8 *arg,
                            struct wsm_buf *buf)
{
	u8 count;
	u32 sta_asleep_mask = 0;
	int i;
	u32 mask = 0;
	u32 change_mask = 0;
	struct xradio_common *hw_priv = priv->hw_priv;

	/* There is no status field in this message */
	sta_asleep_mask = WSM_GET32(buf);
	count = WSM_GET8(buf);
	count -= 1; /* Current workaround for FW issue */

	spin_lock_bh(&priv->ps_state_lock);
	change_mask = (priv->sta_asleep_mask ^ sta_asleep_mask);
	wsm_printk(XRADIO_DBG_MSG, "CM %x, HM %x, FWM %x\n", change_mask,priv->sta_asleep_mask, sta_asleep_mask);
	spin_unlock_bh(&priv->ps_state_lock);

	if (change_mask) {
		struct ieee80211_sta *sta;
		int ret = 0;


		for (i = 0; i < MAX_STA_IN_AP_MODE ; ++i) {

			if(XRADIO_LINK_HARD != priv->link_id_db[i].status)
				continue;

			mask = BIT(i + 1);

			/* If FW state and host state for this link are different then notify OMAC */
			if(change_mask & mask) {
				wsm_printk(XRADIO_DBG_MSG, "PS State Changed %d for sta %pM\n", (sta_asleep_mask & mask) ? 1:0, priv->link_id_db[i].mac);
				rcu_read_lock();
				sta = ieee80211_find_sta(priv->vif, priv->link_id_db[i].mac);
				if (!sta) {
					wsm_printk(XRADIO_DBG_MSG, "WRBC - could not find sta %pM\n",
							priv->link_id_db[i].mac);
				} else {
					ret = ieee80211_sta_ps_transition_ni(sta, (sta_asleep_mask & mask) ? true: false);
					wsm_printk(XRADIO_DBG_MSG, "PS State NOTIFIED %d\n", ret);
					WARN_ON(ret);
				}
				rcu_read_unlock();			
			}
		}
		/* Replace STA mask with one reported by FW */
		spin_lock_bh(&priv->ps_state_lock);
		priv->sta_asleep_mask = sta_asleep_mask;
		spin_unlock_bh(&priv->ps_state_lock);
	}

	wsm_printk(XRADIO_DBG_MSG, "WRBC - HW Buf count %d SleepMask %d\n",
					hw_priv->hw_bufs_used, sta_asleep_mask);
	hw_priv->buf_released = 0;
	WARN_ON(count != (hw_priv->wsm_caps.numInpChBufs - 1));

    return 0;

underflow:
    WARN_ON(1);
    return -EINVAL;
}

/* 3.67 */
int wsm_request_buffer_request(struct xradio_vif *priv,
				u8 *arg)
{
	int ret;
	struct wsm_buf *buf = &priv->hw_priv->wsm_cmd_buf;

	wsm_cmd_lock(priv->hw_priv);

	WSM_PUT8(buf, (*arg));
	WSM_PUT8(buf, 0);
	WSM_PUT16(buf, 0);

	ret = wsm_cmd_send(priv->hw_priv, buf, arg, 0x0023, WSM_CMD_JOIN_TIMEOUT,priv->if_id);

	wsm_cmd_unlock(priv->hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(priv->hw_priv);
	return -ENOMEM;
}

#endif

int wsm_set_keepalive_filter(struct xradio_vif *priv, bool enable)
{
        struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);

        priv->rx_filter.keepalive = enable;
        return wsm_set_rx_filter(hw_priv, &priv->rx_filter, priv->if_id);
}

int wsm_set_probe_responder(struct xradio_vif *priv, bool enable)
{
        struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);

        priv->rx_filter.probeResponder = enable;
        return wsm_set_rx_filter(hw_priv, &priv->rx_filter, priv->if_id);
}
/* ******************************************************************** */
/* WSM indication events implementation					*/

static int wsm_startup_indication(struct xradio_common *hw_priv,
					struct wsm_buf *buf)
{
	u16 status;
	static const char * const fw_types[] = {
			"ETF",
			"WFM",
			"WSM",
			"HI test",
			"Platform test"
	};

	hw_priv->wsm_caps.numInpChBufs	= WSM_GET16(buf);
	hw_priv->wsm_caps.sizeInpChBuf	= WSM_GET16(buf);
	hw_priv->wsm_caps.hardwareId	= WSM_GET16(buf);
	hw_priv->wsm_caps.hardwareSubId	= WSM_GET16(buf);
	status				= WSM_GET16(buf);
	hw_priv->wsm_caps.firmwareCap	= WSM_GET16(buf);
	hw_priv->wsm_caps.firmwareType	= WSM_GET16(buf);
	hw_priv->wsm_caps.firmwareApiVer	= WSM_GET16(buf);
	hw_priv->wsm_caps.firmwareBuildNumber = WSM_GET16(buf);
	hw_priv->wsm_caps.firmwareVersion	= WSM_GET16(buf);
	WSM_GET(buf, &hw_priv->wsm_caps.fw_label[0], WSM_FW_LABEL);
	hw_priv->wsm_caps.fw_label[WSM_FW_LABEL+1] = 0; /* Do not trust FW too much. */

	if (WARN_ON(status))
		return -EINVAL;

	if (WARN_ON(hw_priv->wsm_caps.firmwareType > 4))
		return -EINVAL;

	dev_info(hw_priv->pdev,
		"   Input buffers: %d x %d bytes\n"
		"   Hardware: %d.%d\n"
		"   %s firmware ver: %d, build: %d,"
		    " api: %d, cap: 0x%.4X\n",
		hw_priv->wsm_caps.numInpChBufs,
		hw_priv->wsm_caps.sizeInpChBuf,
		hw_priv->wsm_caps.hardwareId,
		hw_priv->wsm_caps.hardwareSubId,
		fw_types[hw_priv->wsm_caps.firmwareType],
		hw_priv->wsm_caps.firmwareVersion,
		hw_priv->wsm_caps.firmwareBuildNumber,
		hw_priv->wsm_caps.firmwareApiVer,
		hw_priv->wsm_caps.firmwareCap);
	
	dev_info(hw_priv->pdev, "Firmware Label:%s\n", &hw_priv->wsm_caps.fw_label[0]);

	hw_priv->wsm_caps.firmwareReady = 1;

	wake_up(&hw_priv->wsm_startup_done);
	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

//add by yangfh 2014-10-31 16:58:53
void wms_send_deauth_to_self(struct xradio_common *hw_priv, struct xradio_vif *priv)
{
	struct sk_buff *skb = NULL;
	struct ieee80211_mgmt *deauth = NULL;

	if (priv->join_status == XRADIO_JOIN_STATUS_AP) {
		int i = 0;
		wsm_printk(XRADIO_DBG_WARN, "AP mode, send_deauth_to_self\n");
		for (i = 0; i<MAX_STA_IN_AP_MODE; i++) {
			if (priv->link_id_db[i].status == XRADIO_LINK_HARD) {
				skb = dev_alloc_skb(sizeof(struct ieee80211_mgmt) + 64);
				if (!skb)
					return;
				skb_reserve(skb, 64);
				deauth = (struct ieee80211_mgmt *)skb_put(skb, sizeof(struct ieee80211_mgmt));
				if(!deauth) {
					WARN_ON(1);
					return;
				}
				deauth->frame_control =
				    cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_DEAUTH);
				deauth->duration = 0;
				memcpy(deauth->da, priv->vif->addr, ETH_ALEN);
				memcpy(deauth->sa, priv->link_id_db[i].mac, ETH_ALEN);
				memcpy(deauth->bssid, priv->vif->addr, ETH_ALEN);
				deauth->seq_ctrl = 0;
				deauth->u.deauth.reason_code = WLAN_REASON_DEAUTH_LEAVING;
				ieee80211_rx_irqsafe(priv->hw, skb);
			}
		}
	} else if (priv->join_status == XRADIO_JOIN_STATUS_STA) {
		wsm_printk(XRADIO_DBG_WARN, "STA mode, send_deauth_to_self\n");
		skb = dev_alloc_skb(sizeof(struct ieee80211_mgmt) + 64);
		if (!skb)
			return;
		skb_reserve(skb, 64);
		deauth = (struct ieee80211_mgmt *)skb_put(skb, sizeof(struct ieee80211_mgmt));
		if(!deauth) {
			WARN_ON(1);
			return;
		}
		deauth->frame_control =
		    cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_DEAUTH);
		deauth->duration = 0;
		memcpy(deauth->da, priv->vif->addr, ETH_ALEN);
		memcpy(deauth->sa, priv->join_bssid, ETH_ALEN);
		memcpy(deauth->bssid, priv->join_bssid, ETH_ALEN);
		deauth->seq_ctrl = 0;
		deauth->u.deauth.reason_code = WLAN_REASON_DEAUTH_LEAVING;
		ieee80211_rx_irqsafe(priv->hw, skb);
	}
}

void wms_send_disassoc_to_self(struct xradio_common *hw_priv, struct xradio_vif *priv)
{
	struct sk_buff *skb = NULL;
	struct ieee80211_mgmt *disassoc = NULL;
	if (priv->join_status == XRADIO_JOIN_STATUS_AP) {
		int i = 0;
		wsm_printk(XRADIO_DBG_WARN, "AP mode, wms_send_disassoc_to_self\n");
		for (i = 0; i<MAX_STA_IN_AP_MODE; i++) {
			if (priv->link_id_db[i].status == XRADIO_LINK_HARD) {
				skb = dev_alloc_skb(sizeof(struct ieee80211_mgmt) + 64);
				if (!skb)
					return;
				skb_reserve(skb, 64);
				disassoc = (struct ieee80211_mgmt *)skb_put(skb, sizeof(struct ieee80211_mgmt));
				if(!disassoc) {
					WARN_ON(1);
					return;
				}
				disassoc->frame_control =
					cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_DISASSOC);
				disassoc->duration = 0;
				memcpy(disassoc->da, priv->vif->addr, ETH_ALEN);
				memcpy(disassoc->sa, priv->link_id_db[i].mac, ETH_ALEN);
				memcpy(disassoc->bssid, priv->vif->addr, ETH_ALEN);
				disassoc->seq_ctrl = 0;
				disassoc->u.disassoc.reason_code = WLAN_REASON_DISASSOC_STA_HAS_LEFT;
				ieee80211_rx_irqsafe(priv->hw, skb);
			}
		}
	} else if (priv->join_status == XRADIO_JOIN_STATUS_STA) {
		wsm_printk(XRADIO_DBG_WARN, "STA mode, wms_send_disassoc_to_self\n");
		skb = dev_alloc_skb(sizeof(struct ieee80211_mgmt) + 64);
		if (!skb)
			return;
		skb_reserve(skb, 64);
		disassoc = (struct ieee80211_mgmt *)skb_put(skb, sizeof(struct ieee80211_mgmt));
		if(!disassoc) {
			WARN_ON(1);
			return;
		}
		disassoc->frame_control =
		     cpu_to_le16(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_DISASSOC);
		disassoc->duration = 0;
		memcpy(disassoc->da, priv->vif->addr, ETH_ALEN);
		memcpy(disassoc->sa, priv->join_bssid, ETH_ALEN);
		memcpy(disassoc->bssid, priv->join_bssid, ETH_ALEN);
		disassoc->seq_ctrl = 0;
		disassoc->u.disassoc.reason_code = WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY;
		ieee80211_rx_irqsafe(priv->hw, skb);
	}
}

static int wsm_receive_indication(struct xradio_common *hw_priv,
					int interface_link_id,
					struct wsm_buf *buf,
					struct sk_buff **skb_p)
{
	struct xradio_vif *priv;
	struct wsm_rx rx;
	struct ieee80211_hdr *hdr;
	size_t hdr_len;

	hw_priv->rx_timestamp = jiffies;

		rx.status = WSM_GET32(buf);
		rx.channelNumber = WSM_GET16(buf);
		rx.rxedRate = WSM_GET8(buf);
		rx.rcpiRssi = WSM_GET8(buf);
		rx.flags = WSM_GET32(buf);

		/* TODO:COMBO: Frames received from scanning are received
		* with interface ID == 2 */
		if (is_hardware_xradio(hw_priv)) {
			if (interface_link_id == XRWL_GENERIC_IF_ID) {
				/* Frames received in response to SCAN
				 * Request */
				interface_link_id =
					get_interface_id_scanning(hw_priv);
				if (interface_link_id == -1) {
					interface_link_id = hw_priv->roc_if_id;
				}
#ifdef ROAM_OFFLOAD
				if (hw_priv->auto_scanning) {
					interface_link_id = hw_priv->scan.if_id;
				}
#endif/*ROAM_OFFLOAD*/
			}
			/* linkid (peer sta id is encoded in bit 25-28 of
			   flags field */
			rx.link_id = ((rx.flags & (0xf << 25)) >> 25);
			rx.if_id = interface_link_id;
		} else {
			rx.link_id = interface_link_id;
			rx.if_id = 0;
		}
		priv = xrwl_hwpriv_to_vifpriv(hw_priv, rx.if_id);
		if (!priv) {
			dev_dbg(hw_priv->pdev, "got frame on a vif we don't have, dropped\n");
			return 0;
		}
		//remove wsm hdr of skb
		hdr_len = buf->data - buf->begin;
		skb_pull(*skb_p, hdr_len);
		
		/* FW Workaround: Drop probe resp or
		beacon when RSSI is 0 */
		hdr = (struct ieee80211_hdr *) (*skb_p)->data;

		if (!rx.rcpiRssi &&
		    (ieee80211_is_probe_resp(hdr->frame_control) ||
		    ieee80211_is_beacon(hdr->frame_control))) {
			spin_unlock(&priv->vif_lock);
			return 0;
		}

		/* If no RSSI subscription has been made,
		* convert RCPI to RSSI here */
		if (!priv->cqm_use_rssi)
			rx.rcpiRssi = rx.rcpiRssi / 2 - 110;

		if (!rx.status && unlikely(ieee80211_is_deauth(hdr->frame_control))) {
			if (priv->join_status == XRADIO_JOIN_STATUS_STA) {
				/* Shedule unjoin work */
				dev_dbg(hw_priv->pdev,
					"Issue unjoin command (RX).\n");
				wsm_lock_tx_async(hw_priv);
				if (queue_work(hw_priv->workqueue,
						&priv->unjoin_work) <= 0)
					wsm_unlock_tx(hw_priv);
			}
		}
		xradio_rx_cb(priv, &rx, skb_p);
		if (*skb_p)
			skb_push(*skb_p, hdr_len);
		spin_unlock(&priv->vif_lock);

	return 0;

underflow:
	return -EINVAL;
}

static int wsm_event_indication(struct xradio_common *hw_priv,
				struct wsm_buf *buf,
				int interface_link_id)
{
	int first;
	struct xradio_wsm_event *event = NULL;
	struct xradio_vif *priv;

	if (!is_hardware_xradio(hw_priv))
		interface_link_id = 0;

	priv = xrwl_hwpriv_to_vifpriv(hw_priv, interface_link_id);

	if (unlikely(!priv)) {
		dev_warn(hw_priv->pdev, "Event: %d(%d) for removed "
			   "interface, ignoring\n", __le32_to_cpu(WSM_GET32(buf)),
			   __le32_to_cpu(WSM_GET32(buf)));
		return 0;
	}

	if (unlikely(priv->mode == NL80211_IFTYPE_UNSPECIFIED)) {
		/* STA is stopped. */
		return 0;
	}
	spin_unlock(&priv->vif_lock);

	event = kzalloc(sizeof(struct xradio_wsm_event), GFP_KERNEL);
	if (event == NULL) {
		dev_err(hw_priv->pdev, "xr_kzalloc failed!");
		return -EINVAL;
	}

	event->evt.eventId = __le32_to_cpu(WSM_GET32(buf));
	event->evt.eventData = __le32_to_cpu(WSM_GET32(buf));
	event->if_id = interface_link_id;

	dev_dbg(hw_priv->pdev, "Event: %d(%d)\n",
		event->evt.eventId, event->evt.eventData);

	spin_lock(&hw_priv->event_queue_lock);
	first = list_empty(&hw_priv->event_queue);
	list_add_tail(&event->link, &hw_priv->event_queue);
	spin_unlock(&hw_priv->event_queue_lock);

	if (first)
		queue_work(hw_priv->workqueue, &hw_priv->event_handler);

	return 0;

underflow:
	kfree(event);
	return -EINVAL;
}

#define PRINT_11K_MEASRURE 1
static int wsm_measure_cmpl_indication(struct xradio_common *hw_priv,
				                       struct wsm_buf *buf)
{
    MEASUREMENT_COMPLETE measure_cmpl;
    u8 cca_chanload;
    u32 buf_len = 0;
    u32 *data;
	
    LMAC_MEAS_CHANNEL_LOAD_RESULTS *chanload_res;
    LMAC_MEAS_NOISE_HISTOGRAM_RESULTS *noise_res;
	WSM_GET(buf, &measure_cmpl, 12);

    switch (measure_cmpl.MeasurementType) {
	    case ChannelLoadMeasurement:
	        buf_len = sizeof(LMAC_MEAS_CHANNEL_LOAD_RESULTS);
	        break;
	    case NoiseHistrogramMeasurement:
	        buf_len = sizeof(LMAC_MEAS_NOISE_HISTOGRAM_RESULTS);
	        break;
	    case BeaconReport:
	        buf_len = sizeof(LMAC_MEAS_BEACON_RESULTS);
	        break;
	    case STAstatisticsReport:
	        buf_len = sizeof(LMAC_MEAS_STA_STATS_RESULTS);
	        break;
	    case LinkMeasurement:
	        buf_len = sizeof(LMAC_MEAS_LINK_MEASUREMENT_RESULTS);
	        break;
	}
	wsm_printk(XRADIO_DBG_ERROR, "[11K]buf_len = %d\n", buf_len);
    WSM_GET(buf, &measure_cmpl.MeasurementReport, buf_len);
	
	data = (u32 *)(&measure_cmpl);
//	wsm_printk(XRADIO_DBG_ERROR, "[***HL***]data[0]=%08x\n", data[0]);
//	wsm_printk(XRADIO_DBG_ERROR, "[***HL***]data[1]=%08x\n", data[1]);
//	wsm_printk(XRADIO_DBG_ERROR, "[***HL***]data[2]=%08x\n", data[2]);
//	wsm_printk(XRADIO_DBG_ERROR, "[***HL***]data[3]=%08x\n", data[3]);
//	wsm_printk(XRADIO_DBG_ERROR, "[***HL***]data[4]=%08x\n", data[4]);
//	wsm_printk(XRADIO_DBG_ERROR, "[***HL***]data[5]=%08x\n", data[5]);
//	wsm_printk(XRADIO_DBG_ERROR, "[***HL***]data[6]=%08x\n", data[6]);
	wsm_printk(XRADIO_DBG_ERROR, "[***HL***]MeasurementType=%0d\n", measure_cmpl.MeasurementType);
	
	if (measure_cmpl.Status == WSM_STATUS_SUCCESS){
	    switch (measure_cmpl.MeasurementType) {
	        case ChannelLoadMeasurement:
	            chanload_res = &measure_cmpl.MeasurementReport.ChannelLoadResults;
	            cca_chanload = (chanload_res->ChannelLoadCCA == MEAS_CCA) ? 
	                            chanload_res->CCAbusyFraction : 
	                            chanload_res->ChannelLoad;
	            #ifdef PRINT_11K_MEASRURE
	                wsm_printk(XRADIO_DBG_ERROR, "[11K] ChannelLoadMeasurement Result:\n"\
	                                             "ChannelLoadCCA = %d\n"\
	                                             "ChannelNum     = %d\n"\
	                                             "Duration       = %d\n"\
	                                             "Fraction       = %d\n", \
            	               chanload_res->ChannelLoadCCA,\
            	               chanload_res->ChannelNum,\
            	               chanload_res->MeasurementDuration,\
            	               cca_chanload
            	               );
	            #endif
	            break;
	        case NoiseHistrogramMeasurement:
	            noise_res = &measure_cmpl.MeasurementReport.NoiseHistogramResults;
//	            IpiRpi = (noise_res->IpiRpi == MEAS_RPI) ? 
//	                            chanload_res->CCAbusyFraction : 
//	                            chanload_res->ChannelLoad;
	            #ifdef PRINT_11K_MEASRURE
	                wsm_printk(XRADIO_DBG_ERROR, "[11K] NoiseHistogramResults:\n"\
	                                             "IpiRpi = %d\n"\
	                                             "ChannelNum = %d\n"\
	                                             "PI_0__Density = %d\n"\
	                                             "PI_1__Density = %d\n"\
	                                             "PI_2__Density = %d\n"\
	                                             "PI_3__Density = %d\n"\
	                                             "PI_4__Density = %d\n"\
	                                             "PI_5__Density = %d\n"\
	                                             "PI_6__Density = %d\n"\
	                                             "PI_7__Density = %d\n"\
	                                             "PI_8__Density = %d\n"\
	                                             "PI_9__Density = %d\n"\
	                                             "PI_10_Density = %d\n", \
            	               noise_res->IpiRpi,\
            	               noise_res->ChannelNum,\
            	               noise_res->PI_0_Density,\
            	               noise_res->PI_1_Density,\
            	               noise_res->PI_2_Density,\
            	               noise_res->PI_3_Density,\
            	               noise_res->PI_4_Density,\
            	               noise_res->PI_5_Density,\
            	               noise_res->PI_6_Density,\
            	               noise_res->PI_7_Density,\
            	               noise_res->PI_8_Density,\
            	               noise_res->PI_9_Density,\
            	               noise_res->PI_10_Density
            	               );
	            #endif
	            break;
	        case BeaconReport:
	            break;
	        case STAstatisticsReport:
	            break;
	        case LinkMeasurement:
	            break;
	    }
	} else {
	    wsm_printk(XRADIO_DBG_ERROR, "11K Measure(type=%d) Fail\n", measure_cmpl.MeasurementType);
	}

	return 0;

underflow:
	return -EINVAL;
}
/* TODO:COMBO:Make this perVIFF once mac80211 support is available */
static int wsm_channel_switch_indication(struct xradio_common *hw_priv,
						struct wsm_buf *buf)
{
	wsm_unlock_tx(hw_priv); /* Re-enable datapath */
	WARN_ON(WSM_GET32(buf));

	hw_priv->channel_switch_in_progress = 0;
	wake_up(&hw_priv->channel_switch_done);


	xradio_channel_switch_cb(hw_priv);
	return 0;

underflow:
	return -EINVAL;
}

static int wsm_set_pm_indication(struct xradio_common *hw_priv,
					struct wsm_buf *buf)
{
	wsm_oper_unlock(hw_priv);
	return 0;
}

static int wsm_scan_complete_indication(struct xradio_common *hw_priv,
					struct wsm_buf *buf)
{
	struct wsm_scan_complete arg;
#ifdef ROAM_OFFLOAD
	if(hw_priv->auto_scanning == 0)
		wsm_oper_unlock(hw_priv);
#else
	wsm_oper_unlock(hw_priv);
#endif /*ROAM_OFFLOAD*/

	arg.status = WSM_GET32(buf);
	arg.psm = WSM_GET8(buf);
	arg.numChannels = WSM_GET8(buf);
	xradio_scan_complete_cb(hw_priv, &arg);

	return 0;

underflow:
	return -EINVAL;
}

static int wsm_find_complete_indication(struct xradio_common *hw_priv,
					struct wsm_buf *buf)
{
	/* TODO: Implement me. */
	//STUB();
	return 0;
}

static int wsm_suspend_resume_indication(struct xradio_common *hw_priv,
					 int interface_link_id,
					 struct wsm_buf *buf)
{
		u32 flags;
		struct wsm_suspend_resume arg;
		struct xradio_vif *priv;

		if (is_hardware_xradio(hw_priv)) {
			int i;
			arg.if_id = interface_link_id;
			/* TODO:COMBO: Extract bitmap from suspend-resume
			* TX indication */
			xradio_for_each_vif(hw_priv, priv, i) {
				if (!priv)
					continue;
				if (priv->join_status ==
						XRADIO_JOIN_STATUS_AP) {
					 arg.if_id = priv->if_id;
					 break;
				}
				arg.link_id = 0;
			}
		} else {
			arg.if_id = 0;
			arg.link_id = interface_link_id;
		}

		flags = WSM_GET32(buf);
		arg.stop = !(flags & 1);
		arg.multicast = !!(flags & 8);
		arg.queue = (flags >> 1) & 3;

		priv = xrwl_hwpriv_to_vifpriv(hw_priv, arg.if_id);
		if (unlikely(!priv)) {
			wsm_printk(XRADIO_DBG_MSG, "suspend-resume indication"
				   " for removed interface!\n");
			return 0;
		}
		xradio_suspend_resume(priv, &arg);
		spin_unlock(&priv->vif_lock);

	return 0;

underflow:
	return -EINVAL;
}


/* ******************************************************************** */
/* WSM TX								*/

int wsm_cmd_send(struct xradio_common *hw_priv,
		 struct wsm_buf *buf,
		 void *arg, u16 cmd, long tmo, int if_id)
{
	size_t buf_len = buf->data - buf->begin;
	int ret;

	if (cmd == 0x0006 || cmd == 0x0005) /* Write/Read MIB */
		wsm_printk(XRADIO_DBG_MSG, ">>> 0x%.4X [MIB: 0x%.4X] (%d)\n",
			cmd, __le16_to_cpu(((__le16 *)buf->begin)[2]),
			buf_len);
	else
		wsm_printk(XRADIO_DBG_MSG, ">>> 0x%.4X (%d)\n", cmd, buf_len);

	if (unlikely(hw_priv->bh_error)) {
		wsm_buf_reset(buf);
		wsm_printk(XRADIO_DBG_ERROR, "bh error!>>> 0x%.4X (%d)\n", cmd, buf_len);
		return -ETIMEDOUT;
	}

	/* Fill HI message header */
	/* BH will add sequence number */

	/* TODO:COMBO: Add if_id from  to the WSM header */
	/* if_id == -1 indicates that command is HW specific,
	 * eg. wsm_configuration which is called during driver initialzation
	 *  (mac80211 .start callback called when first ifce is created. )*/

	/* send hw specific commands on if 0 */
	if (if_id == -1)
		if_id = 0;

	((__le16 *)buf->begin)[0] = __cpu_to_le16(buf_len);
	((__le16 *)buf->begin)[1] = __cpu_to_le16(cmd |
					((is_hardware_xradio(hw_priv)) ? (if_id << 6) : 0));

	spin_lock(&hw_priv->wsm_cmd.lock);
	BUG_ON(hw_priv->wsm_cmd.ptr);
	hw_priv->wsm_cmd.done = 0;
	hw_priv->wsm_cmd.ptr = buf->begin;
	hw_priv->wsm_cmd.len = buf_len;
	hw_priv->wsm_cmd.arg = arg;
	hw_priv->wsm_cmd.cmd = cmd;
	spin_unlock(&hw_priv->wsm_cmd.lock);

	xradio_bh_wakeup(hw_priv);

	if (unlikely(hw_priv->bh_error)) {
		/* Do not wait for timeout if BH is dead. Exit immediately. */
		ret = 0;
	} else {
		unsigned long wsm_cmd_max_tmo; 

		/* Give start cmd a little more time */
		if (unlikely(tmo == WSM_CMD_START_TIMEOUT))
			wsm_cmd_max_tmo = WSM_CMD_START_TIMEOUT;
		else
			wsm_cmd_max_tmo = WSM_CMD_DEFAULT_TIMEOUT;

		/*Set max timeout.*/
		wsm_cmd_max_tmo = jiffies + wsm_cmd_max_tmo;

		/* Firmware prioritizes data traffic over control confirm.
		 * Loop below checks if data was RXed and increases timeout
		 * accordingly. */
		do {
			/* It's safe to use unprotected access to wsm_cmd.done here */
			ret = wait_event_timeout(hw_priv->wsm_cmd_wq, hw_priv->wsm_cmd.done, tmo);

			/* check time since last rxed and max timeout.*/
		} while (!ret && 
		         time_before_eq(jiffies, hw_priv->rx_timestamp+tmo) && 
		         time_before(jiffies, wsm_cmd_max_tmo));

	}

	if (unlikely(ret == 0)) {
		u16 raceCheck;

		spin_lock(&hw_priv->wsm_cmd.lock);
		raceCheck = hw_priv->wsm_cmd.cmd;
		hw_priv->wsm_cmd.arg = NULL;
		hw_priv->wsm_cmd.ptr = NULL;
		spin_unlock(&hw_priv->wsm_cmd.lock);

		dev_err(hw_priv->pdev, "***CMD timeout!>>> 0x%.4X (%d), buf_use=%d, bh_state=%d\n",
			   cmd, buf_len, hw_priv->hw_bufs_used, hw_priv->bh_error);
		/* Race condition check to make sure _confirm is not called
		 * after exit of _send */
		if (raceCheck == 0xFFFF) {
			/* If wsm_handle_rx got stuck in _confirm we will hang
			 * system there. It's better than silently currupt
			 * stack or heap, isn't it? */
			BUG_ON(wait_event_timeout(
					hw_priv->wsm_cmd_wq,
					hw_priv->wsm_cmd.done,
					WSM_CMD_LAST_CHANCE_TIMEOUT) <= 0);
		}

		/* Kill BH thread to report the error to the top layer. */
		hw_priv->bh_error = 1;
#ifdef BH_USE_SEMAPHORE
		up(&hw_priv->bh_sem);
#else
		wake_up(&hw_priv->bh_wq);
#endif
		ret = -ETIMEDOUT;
	} else {
		spin_lock(&hw_priv->wsm_cmd.lock);
		BUG_ON(!hw_priv->wsm_cmd.done);
		ret = hw_priv->wsm_cmd.ret;
		spin_unlock(&hw_priv->wsm_cmd.lock);
	}
	wsm_buf_reset(buf);
	return ret;
}

/* ******************************************************************** */
/* WSM TX port control							*/

void wsm_lock_tx(struct xradio_common *hw_priv)
{
	down(&hw_priv->tx_lock_sem);
	atomic_add(1, &hw_priv->tx_lock);
	/* always check event if wsm_vif_lock_tx.*/
	if (wsm_flush_tx(hw_priv)) 
		wsm_printk(XRADIO_DBG_MSG, "TX is locked.\n");
	up(&hw_priv->tx_lock_sem);
}

void wsm_vif_lock_tx(struct xradio_vif *priv)
{
	struct xradio_common *hw_priv = priv->hw_priv;
	down(&hw_priv->tx_lock_sem);
	if (atomic_add_return(1, &hw_priv->tx_lock) == 1) {
		if (wsm_vif_flush_tx(priv))
			wsm_printk(XRADIO_DBG_MSG, "TX is locked for"
					" if_id %d.\n", priv->if_id);
	}
	up(&hw_priv->tx_lock_sem);
}

void wsm_lock_tx_async(struct xradio_common *hw_priv)
{
	if (atomic_add_return(1, &hw_priv->tx_lock) == 1)
		wsm_printk(XRADIO_DBG_MSG, "TX is locked (async).\n");
}

bool wsm_flush_tx(struct xradio_common *hw_priv)
{
	long timeout = WSM_CMD_LAST_CHANCE_TIMEOUT;

	/* Flush must be called with TX lock held. */
	BUG_ON(!atomic_read(&hw_priv->tx_lock));

	/* First check if we really need to do something.
	 * It is safe to use unprotected access, as hw_bufs_used
	 * can only decrements. */
	if (!hw_priv->hw_bufs_used)
		return true;

	if (hw_priv->bh_error) {
		/* In case of failure do not wait for magic. */
		wsm_printk(XRADIO_DBG_ERROR, "Fatal error occured, "
				"will not flush TX.\n");
		return false;
	} else {
		/* Get "oldest" frame, if any frames stuck in firmware, 
		   query all of them until max timeout. */
		int num = hw_priv->hw_bufs_used + 1;
		while (xradio_query_txpkt_timeout(hw_priv, XRWL_ALL_IFS, 
		                                  0xffffffff, &timeout)) {
			if (timeout < 0 || !num) {
				/* Hmmm... Not good. Frame had stuck in firmware. */
				wsm_printk(XRADIO_DBG_ERROR,
						   "%s:hw_bufs_used=%d, num=%d, timeout=%ld\n",
						   __func__, hw_priv->hw_bufs_used, num, timeout);
				hw_priv->bh_error = 1;
#ifdef BH_USE_SEMAPHORE
				up(&hw_priv->bh_sem);
#else
				wake_up(&hw_priv->bh_wq);
#endif
				return false;
			} else if (wait_event_timeout(hw_priv->bh_evt_wq, 
			                       !hw_priv->hw_bufs_used, timeout) > 0) {
				return true;
			}
			--num;
		}
		if (hw_priv->hw_bufs_used)
			wsm_printk(XRADIO_DBG_ERROR, "%s:No pengding, but hw_bufs_used=%d\n",
			           __func__, hw_priv->hw_bufs_used);
		/* Ok, everything is flushed. */
		return true;
	}
}

bool wsm_vif_flush_tx(struct xradio_vif *priv)
{
	struct xradio_common *hw_priv = priv->hw_priv;
	long timeout = WSM_CMD_LAST_CHANCE_TIMEOUT;
	int if_id = priv->if_id;

	/* Flush must be called with TX lock held. */
	BUG_ON(!atomic_read(&hw_priv->tx_lock));

	/* First check if we really need to do something.
	 * It is safe to use unprotected access, as hw_bufs_used
	 * can only decrements. */
	if (!hw_priv->hw_bufs_used_vif[if_id])
		return true;

	if (hw_priv->bh_error) {
		/* In case of failure do not wait for magic. */
		wsm_printk(XRADIO_DBG_ERROR, "Fatal error occured, "
				"will not flush TX.\n");
		return false;
	} else {
		/* Get "oldest" frame, if any frames stuck in firmware, 
		   query all of them until max timeout. */
		int num = hw_priv->hw_bufs_used_vif[if_id] + 1;
		while (xradio_query_txpkt_timeout(hw_priv, if_id, 0xffffffff, &timeout)) {
			if (timeout < 0 || !num) {
				/* Hmmm... Not good. Frame had stuck in firmware. */
				wsm_printk(XRADIO_DBG_ERROR, "%s: if_id=%d, hw_bufs_used_vif=%d, num=%d\n", 
				           __func__, if_id, hw_priv->hw_bufs_used_vif[priv->if_id],
				           num);
				hw_priv->bh_error = 1; 
	#ifdef BH_USE_SEMAPHORE
				up(&hw_priv->bh_sem);
	#else
				wake_up(&hw_priv->bh_wq);
	#endif
				return false;
			} else if (wait_event_timeout(hw_priv->bh_evt_wq, 
			              !hw_priv->hw_bufs_used_vif[if_id], timeout) > 0) {
				return true;
			}
			--num;
		}
		if (hw_priv->hw_bufs_used_vif[if_id])
			wsm_printk(XRADIO_DBG_ERROR, "%s:No pengding, but hw_bufs_used_vif=%d\n",
			           __func__, hw_priv->hw_bufs_used_vif[priv->if_id]);
		/* Ok, everything is flushed. */
		return true;
	}
}


void wsm_unlock_tx(struct xradio_common *hw_priv)
{
	int tx_lock;
	if (hw_priv->bh_error)
		wsm_printk(XRADIO_DBG_ERROR, "bh_error=%d, wsm_unlock_tx is unsafe\n",
		           hw_priv->bh_error);
	else {
		tx_lock = atomic_sub_return(1, &hw_priv->tx_lock);
		if (tx_lock < 0) {
			BUG_ON(1);
		} else if (tx_lock == 0) {
			xradio_bh_wakeup(hw_priv);
			wsm_printk(XRADIO_DBG_MSG, "TX is unlocked.\n");
		}
	}
}

/* ******************************************************************** */
/* WSM RX								*/

int wsm_handle_exception(struct xradio_common *hw_priv, u8 *data, size_t len)
{
	struct wsm_buf buf;
	u32 reason;
	u32 reg[18];
	char fname[48];
	int i = 0;
#if defined(CONFIG_XRADIO_USE_EXTENSIONS)
	struct xradio_vif *priv = NULL;
#endif

	static const char * const reason_str[] = {
		"undefined instruction",
		"prefetch abort",
		"data abort",
		"unknown error",
	};

#if defined(CONFIG_XRADIO_USE_EXTENSIONS)
	/* Send the event upwards on the FW exception */
	xradio_pm_stay_awake(&hw_priv->pm_state, 3*HZ);

	spin_lock(&hw_priv->vif_list_lock);
	xradio_for_each_vif(hw_priv, priv, i) {
		if (!priv)
			continue;
		//ieee80211_driver_hang_notify(priv->vif, GFP_KERNEL);
	}
	spin_unlock(&hw_priv->vif_list_lock);
#endif

	buf.begin = buf.data = data;
	buf.end = &buf.begin[len];

	reason = WSM_GET32(&buf);
	for (i = 0; i < ARRAY_SIZE(reg); ++i)
		reg[i] = WSM_GET32(&buf);
	WSM_GET(&buf, fname, sizeof(fname));

	if (reason < 4) {
		dev_err(hw_priv->pdev, "Firmware exception: %s.\n",
		           reason_str[reason]);
	} else {
		dev_err(hw_priv->pdev, "Firmware assert at %.*s, line %d, reason=0x%x\n",
			       sizeof(fname), fname, reg[1], reg[2]);
	}

	for (i = 0; i < 12; i += 4) {
		dev_err(hw_priv->pdev, "Firmware:" \
		           "R%d: 0x%.8X, R%d: 0x%.8X, R%d: 0x%.8X, R%d: 0x%.8X,\n",
		           i + 0, reg[i + 0], i + 1, reg[i + 1],
		           i + 2, reg[i + 2], i + 3, reg[i + 3]);
	}
	dev_err(hw_priv->pdev, "Firmware:" \
	           "R12: 0x%.8X, SP: 0x%.8X, LR: 0x%.8X, PC: 0x%.8X,\n",
	           reg[i + 0], reg[i + 1], reg[i + 2], reg[i + 3]);
	i += 4;
	dev_err(hw_priv->pdev, "Firmware:CPSR: 0x%.8X, SPSR: 0x%.8X\n",
	           reg[i + 0], reg[i + 1]);
	
	return 0;

underflow:
	dev_err(hw_priv->pdev, "Firmware exception.\n");
	print_hex_dump_bytes("Exception: ", DUMP_PREFIX_NONE, data, len);
	return -EINVAL;
}

static int wsm_debug_indication(struct xradio_common *hw_priv,
				                struct wsm_buf       *buf)
{
    //for only one debug item.
    u32 dbg_id, buf_data=0; 
    u16 dbg_buf_len; 
    u8  dbg_len;
    u8 *dbg_buf;
    dbg_id = WSM_GET32(buf);

    dbg_buf_len = buf->end - buf->data;
    
    if (dbg_id == 5) {
	    do {
	        dbg_buf_len = buf->end - buf->data;
	        dbg_len = WSM_GET8(buf);
	        if (dbg_len > dbg_buf_len - sizeof(dbg_len)) {
	            wsm_printk(XRADIO_DBG_ERROR, "[FW]dbg_len     = %d\n", dbg_len);
	            wsm_printk(XRADIO_DBG_ERROR, "[FW]dbg_buf_len = %d\n", dbg_buf_len);
	            wsm_printk(XRADIO_DBG_ERROR, "[FW]debug ind err\n");
	            //ret = -EINVAL;
	            //return ret;
	            
	            
	            break;
	        }
        
        dbg_buf = buf->data;
        //print it;
        wsm_printk(XRADIO_DBG_ALWY,  "[FW-LOG] %s", dbg_buf);
        //
        buf->data += dbg_len;
        
    } while (buf->data < buf->end);
	} else {
		if (dbg_buf_len >= 4) {
			buf_data = WSM_GET32(buf);
			wsm_printk(XRADIO_DBG_ALWY,  "[FW-DEBUG] DbgId = %d, data = %d", dbg_id, buf_data);
		} else {
			wsm_printk(XRADIO_DBG_ALWY,  "[FW-DEBUG] DbgId = %d", dbg_id);
		}
	}

    return 0;
    
underflow:
	WARN_ON(1);
	return -EINVAL;
}

int wsm_handle_rx(struct xradio_common *hw_priv, int id,
		  struct wsm_hdr *wsm, struct sk_buff **skb_p)
{
	int ret = 0;
	struct wsm_buf wsm_buf;
	/* struct xradio_vif *priv = NULL; 	MRK: unused variable, see if 0 below */
	/* int i = 0;				MRK: unused variable, see if 0 below */
	int interface_link_id = (id >> 6) & 0x0F;
#ifdef ROAM_OFFLOAD
#if 0
	struct xradio_vif *priv;
	priv = xrwl_hwpriv_to_vifpriv(hw_priv, interface_link_id);
	if (unlikely(!priv)) {
		WARN_ON(1);
		return 0;
	}
	spin_unlock(&priv->vif_lock);
#endif
#endif/*ROAM_OFFLOAD*/

	/* Strip link id. */
	id &= ~WSM_TX_LINK_ID(WSM_TX_LINK_ID_MAX);

	wsm_buf.begin = (u8 *)&wsm[0];
	wsm_buf.data = (u8 *)&wsm[1];
	wsm_buf.end = &wsm_buf.begin[__le32_to_cpu(wsm->len)];

	wsm_printk(XRADIO_DBG_MSG, "<<< 0x%.4X (%d)\n", id,
			wsm_buf.end - wsm_buf.begin);

#if defined(DGB_XRADIO_HWT)
/***************************for HWT ********************************/
	if (id == 0x0424) {
		u16 TestID = *(u16 *)(wsm_buf.data);
		if (TestID == 1)  //test frame confirm.
			wsm_hwt_tx_confirm(hw_priv, &wsm_buf);
		else {
			spin_lock(&hw_priv->wsm_cmd.lock);
			hw_priv->wsm_cmd.ret = *((u16 *)(wsm_buf.data) + 1);
			hw_priv->wsm_cmd.done = 1;
			spin_unlock(&hw_priv->wsm_cmd.lock);
			wake_up(&hw_priv->wsm_cmd_wq);
			wsm_printk(XRADIO_DBG_ALWY, "HWT TestID=0x%x Confirm ret=%d\n", 
			           *(u16 *)(wsm_buf.data), hw_priv->wsm_cmd.ret);
		}
		return 0;
	} else if (id == 0x0824) {
		u16 TestID = *(u16 *)(wsm_buf.data);
		switch (TestID) {
		case 2:  //recieve a test frame.
			wsm_hwt_rx_frames(hw_priv, &wsm_buf);
			break;
		case 3:  //enc test result.
			wsm_hwt_enc_results(hw_priv, &wsm_buf);
			break;
		case 4:  //mic test result.
			wsm_hwt_mic_results(hw_priv, &wsm_buf);
			break;
		default:
			wsm_printk(XRADIO_DBG_ERROR, "HWT ERROR Indication TestID=0x%x\n", TestID);
			break;
		}
		return 0;
	}
/***************************for HWT ********************************/
#endif //DGB_XRADIO_HWT

	if (id == 0x404) {
		ret = wsm_tx_confirm(hw_priv, &wsm_buf, interface_link_id);
#ifdef MCAST_FWDING
#if 1 
	} else if (id == 0x422) {
		ret = wsm_give_buffer_confirm(hw_priv, &wsm_buf);
#endif
#endif

	} else if (id == 0x41E) {
		ret = wsm_multi_tx_confirm(hw_priv, &wsm_buf,
					   interface_link_id);
	} else if (id & 0x0400) {
		void *wsm_arg;
		u16 wsm_cmd;

		/* Do not trust FW too much. Protection against repeated
		 * response and race condition removal (see above). */
		spin_lock(&hw_priv->wsm_cmd.lock);
		wsm_arg = hw_priv->wsm_cmd.arg;
		wsm_cmd = hw_priv->wsm_cmd.cmd &
				~WSM_TX_LINK_ID(WSM_TX_LINK_ID_MAX);
		hw_priv->wsm_cmd.cmd = 0xFFFF;
		spin_unlock(&hw_priv->wsm_cmd.lock);

		if (WARN_ON((id & ~0x0400) != wsm_cmd)) {
			/* Note that any non-zero is a fatal retcode. */
			ret = -EINVAL;
			goto out;
		}

		switch (id) {
		case 0x0409:
			/* Note that wsm_arg can be NULL in case of timeout in
			 * wsm_cmd_send(). */
			if (likely(wsm_arg))
				ret = wsm_configuration_confirm(hw_priv,
								wsm_arg,
								&wsm_buf);
			break;
		case 0x0405:
			if (likely(wsm_arg))
				ret = wsm_read_mib_confirm(hw_priv, wsm_arg,
								&wsm_buf);
			break;
		case 0x0406:
			if (likely(wsm_arg))
				ret = wsm_write_mib_confirm(hw_priv, wsm_arg,
							    &wsm_buf,
							    interface_link_id);
			break;
		case 0x040B:
			if (likely(wsm_arg))
				ret = wsm_join_confirm(hw_priv, wsm_arg, &wsm_buf);
			if (ret) 
				wsm_printk(XRADIO_DBG_WARN, "Join confirm Failed!\n");
			break;
		case 0x040E: /* 11K measure*/
			if (likely(wsm_arg))
				ret = wsm_generic_confirm(hw_priv, wsm_arg, &wsm_buf);
			if (ret) 
				wsm_printk(XRADIO_DBG_ERROR, "[***HL***]11K Confirm Error\n");

			break;

#ifdef MCAST_FWDING
		case 0x0423: /* req buffer cfm*/
			if (likely(wsm_arg)){
				xradio_for_each_vif(hw_priv, priv, i) {
					if (priv && (priv->join_status == XRADIO_JOIN_STATUS_AP))
						ret = wsm_request_buffer_confirm(priv,
								wsm_arg, &wsm_buf);
				}
			}
			break;
#endif
		case 0x0407: /* start-scan */
#ifdef ROAM_OFFLOAD
			if (hw_priv->auto_scanning) {
				if (atomic_read(&hw_priv->scan.in_progress)) {
					hw_priv->auto_scanning = 0;
				}
				else {
					wsm_oper_unlock(hw_priv);
					up(&hw_priv->scan.lock);
				}
			}
#endif /*ROAM_OFFLOAD*/
		case 0x0408: /* stop-scan */
		case 0x040A: /* wsm_reset */
		case 0x040C: /* add_key */
		case 0x040D: /* remove_key */
		case 0x0410: /* wsm_set_pm */
		case 0x0411: /* set_bss_params */
		case 0x0412: /* set_tx_queue_params */
		case 0x0413: /* set_edca_params */
		case 0x0416: /* switch_channel */
		case 0x0417: /* start */
		case 0x0418: /* beacon_transmit */
		case 0x0419: /* start_find */
		case 0x041A: /* stop_find */
		case 0x041B: /* update_ie */
		case 0x041C: /* map_link */
			WARN_ON(wsm_arg != NULL);
			ret = wsm_generic_confirm(hw_priv, wsm_arg, &wsm_buf);
			if (ret)
				wsm_printk(XRADIO_DBG_ERROR, 
					"wsm_generic_confirm "
    					"failed for request 0x%.4X.\n",
					id & ~0x0400);
			break;
		default:
			BUG_ON(1);
		}

		spin_lock(&hw_priv->wsm_cmd.lock);
		hw_priv->wsm_cmd.ret = ret;
		hw_priv->wsm_cmd.done = 1;
		spin_unlock(&hw_priv->wsm_cmd.lock);
		ret = 0; /* Error response from device should ne stop BH. */

		wake_up(&hw_priv->wsm_cmd_wq);
	} else if (id & 0x0800) {
		switch (id) {
		case 0x0801:
			ret = wsm_startup_indication(hw_priv, &wsm_buf);
			break;
		case 0x0804:
			ret = wsm_receive_indication(hw_priv, interface_link_id,
					&wsm_buf, skb_p);
			break;
		case 0x0805:
			ret = wsm_event_indication(hw_priv, &wsm_buf,
					interface_link_id);
			break;
		case 0x0807:
		    wsm_printk(XRADIO_DBG_ERROR, "[11K]wsm_measure_cmpl_indication\n");
//		    wsm_printk(XRADIO_DBG_ERROR, "[11K]wsm->len = %d\n",wsm->len);
			
			ret = wsm_measure_cmpl_indication(hw_priv, &wsm_buf);
			break;
		case 0x080A:
			ret = wsm_channel_switch_indication(hw_priv, &wsm_buf);
			break;
		case 0x0809:
			ret = wsm_set_pm_indication(hw_priv, &wsm_buf);
			break;
		case 0x0806:
#ifdef ROAM_OFFLOAD
			if(hw_priv->auto_scanning && hw_priv->frame_rcvd) {
				struct xradio_vif *priv;
				hw_priv->frame_rcvd = 0;
				priv = xrwl_hwpriv_to_vifpriv(hw_priv, hw_priv->scan.if_id);
				if (unlikely(!priv)) {
					WARN_ON(1);
					return 0;
				}
					spin_unlock(&priv->vif_lock);
				if (hw_priv->beacon) {
					struct wsm_scan_complete *scan_cmpl = \
						(struct wsm_scan_complete *) \
						((u8 *)wsm + sizeof(struct wsm_hdr));
					struct ieee80211_rx_status *rhdr = \
						IEEE80211_SKB_RXCB(hw_priv->beacon);
					rhdr->signal = (s8)scan_cmpl->reserved;
					if (!priv->cqm_use_rssi) {
						rhdr->signal = rhdr->signal / 2 - 110;
					}
					if (!hw_priv->beacon_bkp)
						hw_priv->beacon_bkp = \
						skb_copy(hw_priv->beacon, GFP_ATOMIC);
					ieee80211_rx_irqsafe(hw_priv->hw, hw_priv->beacon);
					hw_priv->beacon = hw_priv->beacon_bkp;

					hw_priv->beacon_bkp = NULL;
				}
				wsm_printk(XRADIO_DBG_MSG, \
				"Send Testmode Event.\n");
				xradio_testmode_event(priv->hw->wiphy,
					NL80211_CMD_NEW_SCAN_RESULTS, 0,
					0, GFP_KERNEL);

			}
#endif /*ROAM_OFFLOAD*/
			ret = wsm_scan_complete_indication(hw_priv, &wsm_buf);
			break;
		case 0x080B:
			ret = wsm_find_complete_indication(hw_priv, &wsm_buf);
			break;
		case 0x080C:
			ret = wsm_suspend_resume_indication(hw_priv,
					interface_link_id, &wsm_buf);
			break;
		case 0x080E:
			wsm_printk(XRADIO_DBG_MSG,  "wsm_debug_indication");
			ret = wsm_debug_indication(hw_priv, &wsm_buf);
			break;

		default:
			wsm_printk(XRADIO_DBG_ERROR,  "unknown Indmsg ID=0x%04x,len=%d\n", 
			           wsm->id, wsm->len);
			break;
		}
	} else {
		WARN_ON(1);
		ret = -EINVAL;
	}
out:
	return ret;
}

static bool wsm_handle_tx_data(struct xradio_vif *priv,
			       const struct wsm_tx *wsm,
			       const struct ieee80211_tx_info *tx_info,
			       struct xradio_txpriv *txpriv,
			       struct xradio_queue *queue)
{
	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);
	bool handled = false;
	const struct ieee80211_hdr *frame =
		(struct ieee80211_hdr *) &((u8 *)wsm)[txpriv->offset];
	__le16 fctl = frame->frame_control;
	enum {
		doProbe,
		doDrop,
		doJoin,
		doOffchannel,
		doWep,
		doTx,
	} action = doTx;

	hw_priv = xrwl_vifpriv_to_hwpriv(priv);
	frame =  (struct ieee80211_hdr *) &((u8 *)wsm)[txpriv->offset];
	fctl  = frame->frame_control;

	switch (priv->mode) {
	case NL80211_IFTYPE_STATION:
		if (unlikely(priv->bss_loss_status == XRADIO_BSS_LOSS_CHECKING && 
			         priv->join_status     == XRADIO_JOIN_STATUS_STA) &&
			         ieee80211_is_data(fctl)) {
			spin_lock(&priv->bss_loss_lock);
			priv->bss_loss_confirm_id = wsm->packetID;
			priv->bss_loss_status = XRADIO_BSS_LOSS_CONFIRMING;
			spin_unlock(&priv->bss_loss_lock);
		} else if (unlikely((priv->join_status <= XRADIO_JOIN_STATUS_MONITOR) ||
		           memcmp(frame->addr1, priv->join_bssid,sizeof(priv->join_bssid)))) {
			if (ieee80211_is_auth(fctl))
				action = doJoin;
			else if ((ieee80211_is_deauth(fctl) || ieee80211_is_disassoc(fctl))&&
			         priv->join_status < XRADIO_JOIN_STATUS_MONITOR)
				action = doDrop;  //no need to send deauth when STA-unjoined, yangfh 2014-10-31 16:32:16.
			else if (ieee80211_is_probe_req(fctl))
				action = doTx;
			else if (memcmp(frame->addr1, priv->join_bssid,
					sizeof(priv->join_bssid)) &&
					(priv->join_status ==
					XRADIO_JOIN_STATUS_STA) &&
					(ieee80211_is_data(fctl))) {
				action = doDrop;
			}
			else if (priv->join_status >=
					XRADIO_JOIN_STATUS_MONITOR)
				action = doTx;
			else if (get_interface_id_scanning(hw_priv) != -1) {
				wsm_printk(XRADIO_DBG_WARN, "Scan ONGOING dropping"
				           " offchannel eligible frame.\n");
				action = doDrop;
			} else {
				if (ieee80211_is_probe_resp(fctl))
					action = doDrop;
				else
					action = doOffchannel;
				wsm_printk(XRADIO_DBG_WARN, "Offchannel fctl=0x%04x", fctl);
			}
		}
		break;
	case NL80211_IFTYPE_AP:
		if (unlikely(!priv->join_status))
			action = doDrop;
		else if (unlikely(!(BIT(txpriv->raw_link_id) &
				(BIT(0) | priv->link_id_map)))) {
			wsm_printk(XRADIO_DBG_WARN, 
					"A frame with expired link id "
					"is dropped.\n");
			action = doDrop;
		}
		if (xradio_queue_get_generation(wsm->packetID) >
				XRADIO_MAX_REQUEUE_ATTEMPTS) {
			/* HACK!!! WSM324 firmware has tendency to requeue
			 * multicast frames in a loop, causing performance
			 * drop and high power consumption of the driver.
			 * In this situation it is better just to drop
			 * the problematic frame. */
			wsm_printk(XRADIO_DBG_WARN, 
					"Too many attempts "
					"to requeue a frame. "
					"Frame is dropped, fctl=0x%04x.\n", fctl);
			action = doDrop;
		}
		break;
	case NL80211_IFTYPE_ADHOC:
	case NL80211_IFTYPE_MESH_POINT:
		//STUB();
	case NL80211_IFTYPE_MONITOR:
	default:
		action = doDrop;
		break;
	}

	if (action == doTx) {
		if (unlikely(ieee80211_is_probe_req(fctl))) {
			action = doProbe;
		} else if ((fctl & __cpu_to_le32(IEEE80211_FCTL_PROTECTED)) &&
			tx_info->control.hw_key &&
			unlikely(tx_info->control.hw_key->keyidx !=
					priv->wep_default_key_id) &&
			(tx_info->control.hw_key->cipher ==
					WLAN_CIPHER_SUITE_WEP40 ||
			 tx_info->control.hw_key->cipher ==
					WLAN_CIPHER_SUITE_WEP104)) {
			action = doWep;
		}
	}

	switch (action) {
	case doProbe:
	{
		/* An interesting FW "feature". Device filters
		 * probe responses.
		 * The easiest way to get it back is to convert
		 * probe request into WSM start_scan command. */
		wsm_printk(XRADIO_DBG_MSG, \
			"Convert probe request to scan.\n");
		wsm_lock_tx_async(hw_priv);
		hw_priv->pending_frame_id = __le32_to_cpu(wsm->packetID);
		queue_delayed_work(hw_priv->workqueue,
				&hw_priv->scan.probe_work, 0);
		handled = true;
	}
	break;
	case doDrop:
	{
		/* See detailed description of "join" below.
		 * We are dropping everything except AUTH in non-joined mode. */
		wsm_printk(XRADIO_DBG_MSG, "Drop frame (0x%.4X).\n", fctl);
		BUG_ON(xradio_queue_remove(queue,
			__le32_to_cpu(wsm->packetID)));
		handled = true;
	}
	break;
	case doJoin:
	{
		/* p2p should disconnect when sta try to join a different channel AP, 
		 * because no good performance in this case.
		 */
		struct xradio_vif *p2p_tmp_vif = __xrwl_hwpriv_to_vifpriv(hw_priv, 1);
		if (priv->if_id == 0 && p2p_tmp_vif) {
			if (p2p_tmp_vif->join_status >= XRADIO_JOIN_STATUS_STA && 
			    hw_priv->channel_changed) {
				wsm_printk(XRADIO_DBG_WARN, "combo with different channels, p2p disconnect.\n");
				wms_send_disassoc_to_self(hw_priv, p2p_tmp_vif);
			}
		}

		/* There is one more interesting "feature"
		 * in FW: it can't do RX/TX before "join".
		 * "Join" here is not an association,
		 * but just a syncronization between AP and STA.
		 * priv->join_status is used only in bh thread and does
		 * not require protection */
		wsm_printk(XRADIO_DBG_NIY, "Issue join command.\n");
		wsm_lock_tx_async(hw_priv);
		hw_priv->pending_frame_id = __le32_to_cpu(wsm->packetID);
		if (queue_work(hw_priv->workqueue, &priv->join_work) <= 0)
			wsm_unlock_tx(hw_priv);
		handled = true;
	}
	break;
	case doOffchannel:
	{
		wsm_printk(XRADIO_DBG_MSG, "Offchannel TX request.\n");
		wsm_lock_tx_async(hw_priv);
		hw_priv->pending_frame_id = __le32_to_cpu(wsm->packetID);
		if (queue_work(hw_priv->workqueue, &priv->offchannel_work) <= 0)
			wsm_unlock_tx(hw_priv);
		handled = true;
	}
	break;
	case doWep:
	{
		wsm_printk(XRADIO_DBG_MSG, "Issue set_default_wep_key.\n");
		wsm_lock_tx_async(hw_priv);
		priv->wep_default_key_id = tx_info->control.hw_key->keyidx;
		hw_priv->pending_frame_id = __le32_to_cpu(wsm->packetID);
		if (queue_work(hw_priv->workqueue, &priv->wep_key_work) <= 0)
			wsm_unlock_tx(hw_priv);
		handled = true;
	}
	break;
	case doTx:
	{
#if 0
		/* Kept for history. If you want to implement wsm->more,
		 * make sure you are able to send a frame after that. */
		wsm->more = (count > 1) ? 1 : 0;
		if (wsm->more) {
			/* HACK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			 * It's undocumented in WSM spec, but XRADIO hangs
			 * if 'more' is set and no TX is performed due to TX
			 * buffers limitation. */
			if (priv->hw_bufs_used + 1 ==
					priv->wsm_caps.numInpChBufs)
				wsm->more = 0;
		}

		/* BUG!!! FIXME: we can't use 'more' at all: we don't know
		 * future. It could be a request from upper layer with TX lock
		 * requirements (scan, for example). If "more" is set device
		 * will not send data and wsm_tx_lock() will fail...
		 * It's not obvious how to fix this deadlock. Any ideas?
		 * As a workaround more is set to 0. */
		wsm->more = 0;
#endif /* 0 */

		if (ieee80211_is_deauth(fctl) &&
				priv->mode != NL80211_IFTYPE_AP) {
			/* Shedule unjoin work */
			wsm_printk(XRADIO_DBG_WARN, "Issue unjoin command(TX).\n");
#if 0
			wsm->more = 0;
#endif /* 0 */
			wsm_lock_tx_async(hw_priv);
			if (queue_work(hw_priv->workqueue, &priv->unjoin_work) <= 0)
				wsm_unlock_tx(hw_priv);
		}
	}
	break;
	}
	return handled;
}

static int xradio_get_prio_queue(struct xradio_vif *priv,
				 u32 link_id_map, int *total)
{
	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);
	static u32 urgent;
	struct wsm_edca_queue_params *edca;
	unsigned score, best = -1;
	int winner = -1;
	int queued;
	int i;
	urgent = BIT(priv->link_id_after_dtim) | BIT(priv->link_id_uapsd);

	/* search for a winner using edca params */
	for (i = 0; i < 4; ++i) {
		queued = xradio_queue_get_num_queued(priv,
				&hw_priv->tx_queue[i],
				link_id_map);
		if (!queued)
			continue;
		*total += queued;
		edca = &priv->edca.params[i];
		score = ((edca->aifns + edca->cwMin) << 16) +
				(edca->cwMax - edca->cwMin) *
				(prandom_u32() & 0xFFFF);
		if (score < best && (winner < 0 || i != 3)) {
			best = score;
			winner = i;
		}
	}

	/* override winner if bursting */
	if (winner >= 0 && hw_priv->tx_burst_idx >= 0 &&
			winner != hw_priv->tx_burst_idx &&
			!xradio_queue_get_num_queued(priv,
				&hw_priv->tx_queue[winner],
				link_id_map & urgent) &&
			xradio_queue_get_num_queued(priv,
				&hw_priv->tx_queue[hw_priv->tx_burst_idx],
				link_id_map))
		winner = hw_priv->tx_burst_idx;

	return winner;
}

static int wsm_get_tx_queue_and_mask(struct xradio_vif *priv,
				     struct xradio_queue **queue_p,
				     u32 *tx_allowed_mask_p,
				     bool *more)
{
	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);
	int idx;
	u32 tx_allowed_mask;
	int total = 0;

	/* Search for a queue with multicast frames buffered */
	if (priv->tx_multicast) {
		tx_allowed_mask = BIT(priv->link_id_after_dtim);
		idx = xradio_get_prio_queue(priv,
				tx_allowed_mask, &total);
		if (idx >= 0) {
			*more = total > 1;
			goto found;
		}
	}

	/* Search for unicast traffic */
	tx_allowed_mask = ~priv->sta_asleep_mask;
	tx_allowed_mask |= BIT(priv->link_id_uapsd);
	if (priv->sta_asleep_mask) {
		tx_allowed_mask |= priv->pspoll_mask;
		tx_allowed_mask &= ~BIT(priv->link_id_after_dtim);
	} else {
		tx_allowed_mask |= BIT(priv->link_id_after_dtim);
	}
	idx = xradio_get_prio_queue(priv,
			tx_allowed_mask, &total);
	if (idx < 0)
		return -ENOENT;

found:
	*queue_p = &hw_priv->tx_queue[idx];
	*tx_allowed_mask_p = tx_allowed_mask;
	return 0;
}

int wsm_get_tx(struct xradio_common *hw_priv, u8 **data,
	       size_t *tx_len, int *burst, int *vif_selected)
{
	struct wsm_tx *wsm = NULL;
	struct ieee80211_tx_info *tx_info;
	struct xradio_queue *queue = NULL;
	int queue_num;
	u32 tx_allowed_mask = 0;
	struct xradio_txpriv *txpriv = NULL;
	/*
	 * Count was intended as an input for wsm->more flag.
	 * During implementation it was found that wsm->more
	 * is not usable, see details above. It is kept just
	 * in case you would like to try to implement it again.
	 */
	int count = 0;
	int if_pending = 1;

	/* More is used only for broadcasts. */
	bool more = false;

	if (hw_priv->wsm_cmd.ptr) {
		++count;
		spin_lock(&hw_priv->wsm_cmd.lock);
		BUG_ON(!hw_priv->wsm_cmd.ptr);
		*data = hw_priv->wsm_cmd.ptr;
		*tx_len = hw_priv->wsm_cmd.len;
		*burst = 1;
		*vif_selected = -1;
		spin_unlock(&hw_priv->wsm_cmd.lock);
	} else {
		for (;;) {
			int ret;
			struct xradio_vif *priv;
#if 0
			int num_pending_vif0, num_pending_vif1;
#endif
			if (atomic_add_return(0, &hw_priv->tx_lock))
				break;
			/* Keep one buffer reserved for commands. Note
			   that, hw_bufs_used has already been incremented
			   before reaching here. */
			if (hw_priv->hw_bufs_used >=
					hw_priv->wsm_caps.numInpChBufs)
				break;
			priv = wsm_get_interface_for_tx(hw_priv);
			/* go to next interface ID to select next packet */
				hw_priv->if_id_selected ^= 1;

			/* There might be no interface before add_interface
			 * call */
			if (!priv) {
				if (if_pending) {
					if_pending = 0;
					continue;
				}
				break;
			}

#if 0
			if (((priv->if_id == 0) &&
			(hw_priv->hw_bufs_used_vif[0] >=
						XRWL_FW_VIF0_THROTTLE)) ||
			((priv->if_id == 1) &&
			(hw_priv->hw_bufs_used_vif[1] >=
						XRWL_FW_VIF1_THROTTLE))) {
				spin_unlock(&priv->vif_lock);
				if (if_pending) {
					if_pending = 0;
					continue;
				}
				break;
			}
#endif

			/* This can be removed probably: xradio_vif will not
			 * be in hw_priv->vif_list (as returned from
			 * wsm_get_interface_for_tx) until it's fully
			 * enabled, so statement above will take case of that*/
			if (!atomic_read(&priv->enabled)) {
				spin_unlock(&priv->vif_lock);
				break;
			}

			/* TODO:COMBO: Find the next interface for which
			* packet needs to be found */
			spin_lock_bh(&priv->ps_state_lock);
			ret = wsm_get_tx_queue_and_mask(priv, &queue,
					&tx_allowed_mask, &more);
			queue_num = queue - hw_priv->tx_queue;

			if (priv->buffered_multicasts &&
					(ret || !more) &&
					(priv->tx_multicast ||
					 !priv->sta_asleep_mask)) {
				priv->buffered_multicasts = false;
				if (priv->tx_multicast) {
					priv->tx_multicast = false;
					queue_work(hw_priv->workqueue,
						&priv->multicast_stop_work);
				}
			}

			spin_unlock_bh(&priv->ps_state_lock);

			if (ret) {
				spin_unlock(&priv->vif_lock);
				if (if_pending == 1) {
					if_pending = 0;
					continue;
				}
				break;
			}

			if (xradio_queue_get(queue,
					priv->if_id,
					tx_allowed_mask,
					&wsm, &tx_info, &txpriv)) {
				spin_unlock(&priv->vif_lock);
				if_pending = 0;
				continue;
			}

// #ifdef ROC_DEBUG
// 			{
// 				struct ieee80211_hdr *hdr =
// 				(struct ieee80211_hdr *)
// 					&((u8 *)wsm)[txpriv->offset];
// 
// 				wsm_printk(XRADIO_DBG_ERROR, "QGET-1 %x, off_id %d,"
// 					       " if_id %d\n",
// 						hdr->frame_control,
// 						txpriv->offchannel_if_id,
// 						priv->if_id);
// 			}
// #else
// 			{
// 				struct ieee80211_hdr *hdr =
// 				(struct ieee80211_hdr *)
// 					&((u8 *)wsm)[txpriv->offset];
// 
// 				wsm_printk(XRADIO_DBG_ERROR, "QGET-1 %x, off_id %d,"
// 						   " if_id %d\n",
// 						hdr->frame_control,
// 						txpriv->raw_if_id,
// 						priv->if_id);
// 			}
// #endif

			if (wsm_handle_tx_data(priv, wsm,
					tx_info, txpriv, queue)) {
				spin_unlock(&priv->vif_lock);
				if_pending = 0;
				continue;  /* Handled by WSM */
			}

			wsm->hdr.id &= __cpu_to_le16(
					~WSM_TX_IF_ID(WSM_TX_IF_ID_MAX));
			if (txpriv->offchannel_if_id)
				wsm->hdr.id |= cpu_to_le16(
					WSM_TX_IF_ID(txpriv->offchannel_if_id));
			else
				wsm->hdr.id |= cpu_to_le16(
					WSM_TX_IF_ID(priv->if_id));

			*vif_selected = priv->if_id;
// #ifdef ROC_DEBUG
// /* remand the roc debug. */
// 			{
// 				struct ieee80211_hdr *hdr =
// 				(struct ieee80211_hdr *)
// 					&((u8 *)wsm)[txpriv->offset];
// 
// 				wsm_printk(XRADIO_DBG_ERROR, "QGET-2 %x, off_id %d,"
// 					       " if_id %d\n",
// 						hdr->frame_control,
// 						txpriv->offchannel_if_id,
// 						priv->if_id);
// 			}
// #else
// 			{
// 				struct ieee80211_hdr *hdr =
// 				(struct ieee80211_hdr *)
// 					&((u8 *)wsm)[txpriv->offset];
// 
// 				wsm_printk(XRADIO_DBG_ERROR, "QGET-2 %x, off_id %d,"
// 						   " if_id %d\n",
// 						hdr->frame_control,
// 						txpriv->raw_if_id,
// 						priv->if_id);
// 			}
// #endif

			priv->pspoll_mask &= ~BIT(txpriv->raw_link_id);

			*data = (u8 *)wsm;
			*tx_len = __le16_to_cpu(wsm->hdr.len);

			/* allow bursting if txop is set */
			if (priv->edca.params[queue_num].txOpLimit)
				*burst = min(*burst,
					(int)xradio_queue_get_num_queued(priv,
						queue, tx_allowed_mask) + 1);
			else
				*burst = 1;

			/* store index of bursting queue */
			if (*burst > 1)
				hw_priv->tx_burst_idx = queue_num;
			else
				hw_priv->tx_burst_idx = -1;

			if (more) {
				struct ieee80211_hdr *hdr =
					(struct ieee80211_hdr *)
					&((u8 *)wsm)[txpriv->offset];
				if(strstr(&priv->ssid[0], "6.1.12")) {
					if(hdr->addr1[0] & 0x01 ) {
						hdr->frame_control |=
						cpu_to_le16(IEEE80211_FCTL_MOREDATA);
					}
				}
				else {
					/* more buffered multicast/broadcast frames
					*  ==> set MoreData flag in IEEE 802.11 header
					*  to inform PS STAs */
					hdr->frame_control |=
					cpu_to_le16(IEEE80211_FCTL_MOREDATA);
				}
			}
			wsm_printk(XRADIO_DBG_MSG, ">>> 0x%.4X (%d) %p %c\n",
				0x0004, *tx_len, *data,
				wsm->more ? 'M' : ' ');
			++count;
			spin_unlock(&priv->vif_lock);
			break;
		}
	}

	return count;
}

void wsm_txed(struct xradio_common *hw_priv, u8 *data)
{
	if (data == hw_priv->wsm_cmd.ptr) {
		spin_lock(&hw_priv->wsm_cmd.lock);
		hw_priv->wsm_cmd.ptr = NULL;
		spin_unlock(&hw_priv->wsm_cmd.lock);
	}
}

/* ******************************************************************** */
/* WSM buffer								*/

void wsm_buf_init(struct wsm_buf *buf)
{
	int size = (SDIO_BLOCK_SIZE<<1); //for sdd file big than SDIO_BLOCK_SIZE
	BUG_ON(buf->begin);
	buf->begin = kmalloc(size, GFP_KERNEL);
	buf->end = buf->begin ? &buf->begin[size] : buf->begin;
	wsm_buf_reset(buf);
}

void wsm_buf_deinit(struct wsm_buf *buf)
{
	if(buf->begin)
		kfree(buf->begin);
	buf->begin = buf->data = buf->end = NULL;
}

static void wsm_buf_reset(struct wsm_buf *buf)
{
	if (buf->begin) {
		buf->data = &buf->begin[4];
		*(u32 *)buf->begin = 0;
	} else
		buf->data = buf->begin;
}

static int wsm_buf_reserve(struct wsm_buf *buf, size_t extra_size)
{
	size_t pos = buf->data - buf->begin;
	size_t size = pos + extra_size;


	if (size & (SDIO_BLOCK_SIZE - 1)) {
		size &= SDIO_BLOCK_SIZE;
		size += SDIO_BLOCK_SIZE;
	}

	buf->begin = krealloc(buf->begin, size, GFP_KERNEL);
	if (buf->begin) {
		buf->data = &buf->begin[pos];
		buf->end = &buf->begin[size];
		return 0;
	} else {
		buf->end = buf->data = buf->begin;
		return -ENOMEM;
	}
}

static struct xradio_vif 
	*wsm_get_interface_for_tx(struct xradio_common *hw_priv)
{
	struct xradio_vif *priv = NULL, *i_priv;
	int i = hw_priv->if_id_selected;

	if ( 1 /*TODO:COMBO*/) {
		spin_lock(&hw_priv->vif_list_lock);
		i_priv = hw_priv->vif_list[i] ? 
		         xrwl_get_vif_from_ieee80211(hw_priv->vif_list[i]) : NULL;
		if (i_priv) {
			priv = i_priv;
			spin_lock(&priv->vif_lock);
		}
		/* TODO:COMBO:
		* Find next interface based on TX bitmap announced by the FW
		* Find next interface based on load balancing */
		spin_unlock(&hw_priv->vif_list_lock);
	} else {
		priv = xrwl_hwpriv_to_vifpriv(hw_priv, 0);
	}

	return priv;
}

static inline int get_interface_id_scanning(struct xradio_common *hw_priv)
{
	if (hw_priv->scan.req || hw_priv->scan.direct_probe)
		return hw_priv->scan.if_id;
	else
		return -1;
}
