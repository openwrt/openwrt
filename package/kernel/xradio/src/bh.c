/*
 * Data Transmission thread implementation for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <net/mac80211.h>
#include <linux/kthread.h>

#include "xradio.h"
#include "bh.h"
#include "hwio.h"
#include "wsm.h"
#include "sdio.h"

/* TODO: Verify these numbers with WSM specification. */
#define DOWNLOAD_BLOCK_SIZE_WR	(0x1000 - 4)
/* an SPI message cannot be bigger than (2"12-1)*2 bytes
 * "*2" to cvt to bytes */
#define MAX_SZ_RD_WR_BUFFERS	(DOWNLOAD_BLOCK_SIZE_WR*2)
#define PIGGYBACK_CTRL_REG	(2)
#define EFFECTIVE_BUF_SIZE	(MAX_SZ_RD_WR_BUFFERS - PIGGYBACK_CTRL_REG)

/* Suspend state privates */
enum xradio_bh_pm_state {
	XRADIO_BH_RESUMED = 0,
	XRADIO_BH_SUSPEND,
	XRADIO_BH_SUSPENDED,
	XRADIO_BH_RESUME,
};
typedef int (*xradio_wsm_handler)(struct xradio_common *hw_priv, u8 *data, size_t size);

#ifdef MCAST_FWDING
int wsm_release_buffer_to_fw(struct xradio_vif *priv, int count);
#endif
static int xradio_bh(void *arg);

int xradio_register_bh(struct xradio_common *hw_priv)
{
	int ret = 0;

	atomic_set(&hw_priv->bh_tx, 0);
	atomic_set(&hw_priv->bh_term, 0);
	atomic_set(&hw_priv->bh_suspend, XRADIO_BH_RESUMED);
	hw_priv->buf_id_tx = 0;
	hw_priv->buf_id_rx = 0;
	init_waitqueue_head(&hw_priv->bh_wq);
	init_waitqueue_head(&hw_priv->bh_evt_wq);

	hw_priv->bh_thread = kthread_run(&xradio_bh, hw_priv, XRADIO_BH_THREAD);
	if (IS_ERR(hw_priv->bh_thread)) {
		ret = PTR_ERR(hw_priv->bh_thread);
		hw_priv->bh_thread = NULL;
	}

	return ret;
}

void xradio_unregister_bh(struct xradio_common *hw_priv)
{
	struct task_struct *thread = hw_priv->bh_thread;

	if (WARN_ON(!thread))
		return;

	hw_priv->bh_thread = NULL;
	kthread_stop(thread);
#ifdef HAS_PUT_TASK_STRUCT
	put_task_struct(thread);
#endif
	dev_dbg(hw_priv->pdev, "Unregister success.\n");
}

void xradio_irq_handler(struct xradio_common *hw_priv)
{
	xradio_bh_wakeup(hw_priv);
}

void xradio_bh_wakeup(struct xradio_common *hw_priv)
{
	atomic_set(&hw_priv->bh_tx, 1);
	wake_up(&hw_priv->bh_wq);
}

int xradio_bh_suspend(struct xradio_common *hw_priv)
{
#ifdef MCAST_FWDING
	int i =0;
	struct xradio_vif *priv = NULL;
#endif

	if (hw_priv->bh_error) {
		return -EINVAL;
	}

#ifdef MCAST_FWDING
 	xradio_for_each_vif(hw_priv, priv, i) {
		if (!priv)
			continue;	
		if ( (priv->multicast_filter.enable)
			&& (priv->join_status == XRADIO_JOIN_STATUS_AP) ) {
			wsm_release_buffer_to_fw(priv,
				(hw_priv->wsm_caps.numInpChBufs - 1));
			break;
		}
	}
#endif

	atomic_set(&hw_priv->bh_suspend, XRADIO_BH_SUSPEND);
	wake_up(&hw_priv->bh_wq);
	return wait_event_timeout(hw_priv->bh_evt_wq, (hw_priv->bh_error || 
	                          XRADIO_BH_SUSPENDED == atomic_read(&hw_priv->bh_suspend)),
	                          1 * HZ)?  0 : -ETIMEDOUT;
}

int xradio_bh_resume(struct xradio_common *hw_priv)
{
#ifdef MCAST_FWDING
	int ret;
	int i =0; 
	struct xradio_vif *priv =NULL;
#endif


	if (hw_priv->bh_error) {
		return -EINVAL;
	}

	atomic_set(&hw_priv->bh_suspend, XRADIO_BH_RESUME);
	wake_up(&hw_priv->bh_wq);

#ifdef MCAST_FWDING
	ret = wait_event_timeout(hw_priv->bh_evt_wq, (hw_priv->bh_error ||
	                         XRADIO_BH_RESUMED == atomic_read(&hw_priv->bh_suspend))
	                         ,1 * HZ)? 0 : -ETIMEDOUT;

	xradio_for_each_vif(hw_priv, priv, i) {
		if (!priv)
			continue;
		if ((priv->join_status == XRADIO_JOIN_STATUS_AP) && 
			  (priv->multicast_filter.enable)) {
			u8 count = 0;
			WARN_ON(wsm_request_buffer_request(priv, &count));
			dev_dbg(hw_priv->pdev, "Reclaim Buff %d \n",count);
			break;
		}
	}

	return ret;
#else
	return wait_event_timeout(hw_priv->bh_evt_wq,hw_priv->bh_error ||
		(XRADIO_BH_RESUMED == atomic_read(&hw_priv->bh_suspend)),
		1 * HZ) ? 0 : -ETIMEDOUT;
#endif

}

static inline void wsm_alloc_tx_buffer(struct xradio_common *hw_priv)
{
	++hw_priv->hw_bufs_used;
}

int wsm_release_tx_buffer(struct xradio_common *hw_priv, int count)
{
	int ret = 0;
	int hw_bufs_used = hw_priv->hw_bufs_used;

	hw_priv->hw_bufs_used -= count;
	if (WARN_ON(hw_priv->hw_bufs_used < 0)) {
		/* Tx data patch stops when all but one hw buffers are used.
		   So, re-start tx path in case we find hw_bufs_used equals
		   numInputChBufs - 1.
		*/
		dev_err(hw_priv->pdev, "hw_bufs_used=%d, count=%d.\n",
				hw_priv->hw_bufs_used, count);
		ret = -1;
	} else if (hw_bufs_used >= (hw_priv->wsm_caps.numInpChBufs - 1))
		ret = 1;
	if (!hw_priv->hw_bufs_used)
		wake_up(&hw_priv->bh_evt_wq);
	return ret;
}

int wsm_release_vif_tx_buffer(struct xradio_common *hw_priv, int if_id, int count)
{
	int ret = 0;

	hw_priv->hw_bufs_used_vif[if_id] -= count;
	if (!hw_priv->hw_bufs_used_vif[if_id])
		wake_up(&hw_priv->bh_evt_wq);

	if (WARN_ON(hw_priv->hw_bufs_used_vif[if_id] < 0))
		ret = -1;
	return ret;
}
#ifdef MCAST_FWDING
int wsm_release_buffer_to_fw(struct xradio_vif *priv, int count)
{
	int i;
	u8 flags;
	struct wsm_buf *buf;
	size_t buf_len;
	struct wsm_hdr *wsm;
	struct xradio_common *hw_priv = priv->hw_priv;

	if (priv->join_status != XRADIO_JOIN_STATUS_AP) {
		return 0;
	}
	dev_dbg(hw_priv->pdev, "Rel buffer to FW %d, %d\n", count, hw_priv->hw_bufs_used);

	for (i = 0; i < count; i++) {
		if ((hw_priv->hw_bufs_used + 1) < hw_priv->wsm_caps.numInpChBufs) {
			flags = i ? 0: 0x1;

			wsm_alloc_tx_buffer(hw_priv);
			buf = &hw_priv->wsm_release_buf[i];
			buf_len = buf->data - buf->begin;

			/* Add sequence number */
			wsm = (struct wsm_hdr *)buf->begin;
			BUG_ON(buf_len < sizeof(*wsm));

			wsm->id &= __cpu_to_le32(~WSM_TX_SEQ(WSM_TX_SEQ_MAX));
			wsm->id |= cpu_to_le32(WSM_TX_SEQ(hw_priv->wsm_tx_seq));

			dev_dbg(hw_priv->pdev, "REL %d\n", hw_priv->wsm_tx_seq);
			if (WARN_ON(xradio_data_write(hw_priv, buf->begin, buf_len))) {
				break;
			}
			hw_priv->buf_released = 1;
			hw_priv->wsm_tx_seq = (hw_priv->wsm_tx_seq + 1) & WSM_TX_SEQ_MAX;
		} else
			break;
	}

	if (i == count) {
		return 0;
	}

	/* Should not be here */
	dev_dbg(hw_priv->pdev, "Error, Less HW buf %d,%d.\n",
	          hw_priv->hw_bufs_used, hw_priv->wsm_caps.numInpChBufs);
	WARN_ON(1);
	return -1;
}
#endif

/* reserve a packet for the case dev_alloc_skb failed in bh.*/
int xradio_init_resv_skb(struct xradio_common *hw_priv)
{
	int len = (SDIO_BLOCK_SIZE<<2) + WSM_TX_EXTRA_HEADROOM + \
	           8 + 12;	/* TKIP IV + ICV and MIC */

	hw_priv->skb_reserved = dev_alloc_skb(len);
	if (hw_priv->skb_reserved) {
		hw_priv->skb_resv_len = len;
	} else {
		dev_warn(hw_priv->pdev, "xr_alloc_skb failed(%d)\n", len);
	}
	return 0;
}

void xradio_deinit_resv_skb(struct xradio_common *hw_priv)
{
	if (hw_priv->skb_reserved) {
		dev_kfree_skb(hw_priv->skb_reserved);
		hw_priv->skb_reserved = NULL;
		hw_priv->skb_resv_len = 0;
	}
}

int xradio_realloc_resv_skb(struct xradio_common *hw_priv,
							struct sk_buff *skb)
{
	if (!hw_priv->skb_reserved && hw_priv->skb_resv_len) {
		hw_priv->skb_reserved = dev_alloc_skb(hw_priv->skb_resv_len);
		if (!hw_priv->skb_reserved) {
			hw_priv->skb_reserved = skb;
			dev_warn(hw_priv->pdev, "xr_alloc_skb failed(%d)\n",
					hw_priv->skb_resv_len);
			return -1;
		}
	}
	return 0; /* realloc sbk success, deliver to upper.*/
}

static inline struct sk_buff *xradio_get_resv_skb(struct xradio_common *hw_priv,
												  size_t len)
{	struct sk_buff *skb = NULL;
	if (hw_priv->skb_reserved && len <= hw_priv->skb_resv_len) {
		skb = hw_priv->skb_reserved;
		hw_priv->skb_reserved = NULL;
	}
	return skb;
}

static inline int xradio_put_resv_skb(struct xradio_common *hw_priv,
									  struct sk_buff *skb)
{
	if (!hw_priv->skb_reserved && hw_priv->skb_resv_len) {
		hw_priv->skb_reserved = skb;
		return 0;
	}
	return 1; /* sbk not put to reserve*/
}

static struct sk_buff *xradio_get_skb(struct xradio_common *hw_priv, size_t len)
{
	struct sk_buff *skb = NULL;
	size_t alloc_len = (len > SDIO_BLOCK_SIZE) ? len : SDIO_BLOCK_SIZE;

	/* TKIP IV + TKIP ICV and MIC - Piggyback.*/
	alloc_len += WSM_TX_EXTRA_HEADROOM + 8 + 12- 2;
	if (len > SDIO_BLOCK_SIZE || !hw_priv->skb_cache) {
		skb = dev_alloc_skb(alloc_len);
		/* In AP mode RXed SKB can be looped back as a broadcast.
		 * Here we reserve enough space for headers. */
		if (skb) {
			skb_reserve(skb, WSM_TX_EXTRA_HEADROOM + 8 /* TKIP IV */
					    - WSM_RX_EXTRA_HEADROOM);
		} else {
			skb = xradio_get_resv_skb(hw_priv, alloc_len);
			if (skb) {
				dev_dbg(hw_priv->pdev, "get skb_reserved(%d)!\n", alloc_len);
				skb_reserve(skb, WSM_TX_EXTRA_HEADROOM + 8 /* TKIP IV */
						    - WSM_RX_EXTRA_HEADROOM);
			} else {
				dev_dbg(hw_priv->pdev, "xr_alloc_skb failed(%d)!\n", alloc_len);
			}
		}
	} else {
		skb = hw_priv->skb_cache;
		hw_priv->skb_cache = NULL;
	}
	return skb;
}

static void xradio_put_skb(struct xradio_common *hw_priv, struct sk_buff *skb)
{
	if (hw_priv->skb_cache)
		dev_kfree_skb(skb);
	else
		hw_priv->skb_cache = skb;
}

static int xradio_bh_read_ctrl_reg(struct xradio_common *hw_priv,
					  u16 *ctrl_reg)
{
	int ret;
	ret = xradio_reg_read_16(hw_priv, HIF_CONTROL_REG_ID, ctrl_reg);
	if (ret) {
		ret = xradio_reg_read_16(hw_priv, HIF_CONTROL_REG_ID, ctrl_reg);
		if (ret) {
			hw_priv->bh_error = 1;
			dev_err(hw_priv->pdev, "Failed to read control register.\n");
		}
	}

	return ret;
}

static int xradio_device_wakeup(struct xradio_common *hw_priv)
{
	u16 ctrl_reg;
	int ret, i=0;

	/* To force the device to be always-on, the host sets WLAN_UP to 1 */
	ret = xradio_reg_write_16(hw_priv, HIF_CONTROL_REG_ID, HIF_CTRL_WUP_BIT);
	if (WARN_ON(ret))
		return ret;

	ret = xradio_bh_read_ctrl_reg(hw_priv, &ctrl_reg);
	if (WARN_ON(ret))
		return ret;

	/* If the device returns WLAN_RDY as 1, the device is active and will
	 * remain active. */
	while (!(ctrl_reg & HIF_CTRL_RDY_BIT) && i < 500) {
		ret = xradio_bh_read_ctrl_reg(hw_priv, &ctrl_reg);
		msleep(1);
		i++;
	}
	if (unlikely(i >= 500)) {
		dev_err(hw_priv->pdev, "Device cannot wakeup.\n");
		return -1;
	} else if (unlikely(i >= 50))
		dev_warn(hw_priv->pdev, "Device wakeup time=%dms.\n", i);
	dev_dbg(hw_priv->pdev, "Device awake, t=%dms.\n", i);
	return 1;
}

/* Must be called from BH thraed. */
void xradio_enable_powersave(struct xradio_vif *priv,
			     bool enable)
{
	priv->powersave_enabled = enable;
}

static void xradio_bh_rx_dump(struct device *dev, u8 *data, size_t len){
#ifdef DEBUG
	static const char *msgnames[0xffff] = {
			// 0x4?? is a sync response to a command
			[0x0404] = "tx confirm",
			[0x0406] = "mib confirm",
			[0x0407] = "scan started",
			[0x0409] = "configuration confirm",
			[0x040a] = "reset confirm",
			[0x040b] = "join confirm",
			[0x040c] = "key added",
			[0x040d] = "key removed",
			[0x0410] = "pm confirm",
			[0x0411] = "set bss params",
			[0x0412] = "tx queue params",
			[0x0413] = "edca confirm",
			[0x0417] = "start confirm",
			[0x041b] = "update ie confirm",
			[0x041c] = "map link confirm",
			// 0x8?? seem to be async responses or events
			[0x0801] = "firmware startup complete",
			[0x0804] = "rx",
			[0x0805] = "event",
			[0x0806] = "scan complete",
			[0x0810] = "set pm indication"
	};

	u16 msgid, ifid;
	u16 *p = (u16 *)data;
	msgid = (*(p + 1)) & 0xC3F;
	ifid  = (*(p + 1)) >> 6;
	ifid &= 0xF;
	const char *msgname = msgnames[msgid];
	if(msgid == 0x804 && ifid == 2){
		msgname = "scan result";
	}

	dev_dbg(dev, "vif %d: sdio rx, msgid %s(0x%.4X) len %d\n",
			ifid, msgname, msgid, *p);
//	print_hex_dump_bytes("<-- ", DUMP_PREFIX_NONE,
//	                     data, min(len, (size_t) 64));
#endif
}

#define READLEN(ctrl) ((ctrl & HIF_CTRL_NEXT_LEN_MASK) << 1) //read_len=ctrl_reg*2.

static int xradio_bh_rx_availlen(struct xradio_common *hw_priv){
	u16 ctrl_reg = 0;
	if (xradio_bh_read_ctrl_reg(hw_priv, &ctrl_reg)) {
		return -EIO;
	}
	return READLEN(ctrl_reg);
}

static int xradio_bh_rx(struct xradio_common *hw_priv, u16* nextlen) {
	size_t read_len = 0;
	struct sk_buff *skb_rx = NULL;
	struct wsm_hdr *wsm;
	size_t wsm_len;
	int wsm_id;
	u8 wsm_seq;
	size_t alloc_len;
	u8 *data;
	int ret;

	read_len = *nextlen > 0 ? *nextlen : xradio_bh_rx_availlen(hw_priv);
	if(read_len <= 0)
		return read_len;

	if (read_len < sizeof(struct wsm_hdr) || (read_len > EFFECTIVE_BUF_SIZE)) {
		dev_err(hw_priv->pdev, "Invalid read len: %d", read_len);
		return -1;
	}

	/* Add SIZE of PIGGYBACK reg (CONTROL Reg)
	 * to the NEXT Message length + 2 Bytes for SKB */
	read_len = read_len + 2;

	alloc_len = sdio_align_len(hw_priv, read_len);
	/* Check if not exceeding XRADIO capabilities */
	if (WARN_ON_ONCE(alloc_len > EFFECTIVE_BUF_SIZE)) {
		dev_err(hw_priv->pdev, "Read aligned len: %d\n", alloc_len);
	}

	/* Get skb buffer. */
	skb_rx = xradio_get_skb(hw_priv, alloc_len);
	if (!skb_rx) {
		dev_err(hw_priv->pdev, "xradio_get_skb failed.\n");
		return -ENOMEM;
	}
	skb_trim(skb_rx, 0);
	skb_put(skb_rx, read_len);
	data = skb_rx->data;
	if (!data) {
		dev_err(hw_priv->pdev, "skb data is NULL.\n");
		ret = -ENOMEM;
		goto out;
	}

	/* Read data from device. */
	if (xradio_data_read(hw_priv, data, alloc_len)) {
		ret = -EIO;
		goto out;
	}

	/* the ctrl register is appened to the end of the wsm frame
	 * so we can use this to avoid reading the control register
	 * again for the next read .. but if this is invalid we'll
	 * do an invalid read and the firmware will crash so this
	 * probably needs some sort of validation */
	*nextlen = READLEN(__le16_to_cpu(((__le16 *) data)[(alloc_len >> 1) - 1]));

	/* check wsm length. */
	wsm = (struct wsm_hdr *) data;
	wsm_len = __le32_to_cpu(wsm->len);

	if (WARN_ON(wsm_len > read_len)) {
		dev_err(hw_priv->pdev, "wsm is bigger than data read, read %d but frame is %d\n",
				read_len, wsm_len);
		ret = -1;
		goto out;
	}

	/* dump rx data. */
	xradio_bh_rx_dump(hw_priv->pdev, data, wsm_len);

	/* extract wsm id and seq. */
	wsm_id = __le32_to_cpu(wsm->id) & 0xFFF;
	wsm_seq = (__le32_to_cpu(wsm->id) >> 13) & 7;
	skb_trim(skb_rx, wsm_len);

	/* process exceptions. */
	if (wsm_id == 0) {
		printk("wtf?\n");
		ret = 0;
		goto out;
	} else if (unlikely(wsm_id == 0x0800)) {
		dev_err(hw_priv->pdev, "firmware exception!\n");
		wsm_handle_exception(hw_priv, &data[sizeof(*wsm)],
				wsm_len - sizeof(*wsm));
		ret = -1;
		goto out;
	}

	hw_priv->wsm_rx_seq = (wsm_seq + 1) & 7;

	/* Process tx frames confirm. */
	if (wsm_id & 0x0400) {
		if (wsm_release_tx_buffer(hw_priv, 1) < 0) {
			dev_err(hw_priv->pdev, "tx buffer < 0.\n");
			ret  = -1;
			goto out;
		}
	}

	/* WSM processing frames. */
	if (wsm_handle_rx(hw_priv, wsm_id, wsm, &skb_rx)) {
		dev_err(hw_priv->pdev, "wsm_handle_rx failed.\n");
		ret = -1;
		goto out;
	}

	ret = 1;

out:
	/* Reclaim the SKB buffer */
	if (skb_rx) {
		if (xradio_put_resv_skb(hw_priv, skb_rx))
			xradio_put_skb(hw_priv, skb_rx);
	}

	return ret;
}

static void xradio_bh_tx_dump(struct device *dev, u8 *data, size_t len){
#ifdef DEBUG
	static const char *msgnames[0xffff] = {
			[0x0004] = "tx",
			[0x0006] = "MIB",
			[0x0007] = "start scan",
			[0x0009] = "configure",
			[0x000A] = "reset",
			[0x000B] = "join",
			[0x000C] = "add key",
			[0x000D] = "remove key",
			[0x0010] = "set pm",
			[0x0011] = "set bss params",
			[0x0012] = "set tx queue params",
			[0x0013] = "set edca",
			[0x0017] = "start",
			[0x001b] = "update ie",
			[0x001c] = "map link",
	};
	static const char *mibnames[0xffff] = {
			[0x0003] = "DOT11_SLOT_TIME",
			[0x1002] = "TEMPLATE_FRAME",
			[0x1003] = "RX_FILTER",
			[0x1004] = "BEACON_FILTER_TABLE",
			[0x1005] = "BEACON_FILTER_ENABLE",
			[0x1006] = "OPERATIONAL POWER MODE",
			[0x1007] = "BEACON_WAKEUP_PERIOD",
			[0x1009] = "RCPI_RSSI_THRESHOLD",
			[0x1010] = "SET_ASSOCIATION_MODE",
			[0x100e] = "BLOCK_ACK_POLICY",
			[0x100f] = "OVERRIDE_INTERNAL_TX_RATE",
			[0x1013] = "SET_UAPSD_INFORMATION",
			[0x1016] = "SET_TX_RATE_RETRY_POLICY",
			[0x1020] = "PROTECTED_MGMT_POLICY",
			[0x1021] = "SET_HT_PROTECTION",
			[0x1024] = "USE_MULTI_TX_CONF",
			[0x1025] = "KEEP_ALIVE_PERIOD",
			[0x1026] = "DISABLE_BSSID_FILTER",
			[0x1035] = "SET_INACTIVITY",
	};

	u16 msgid, ifid, mib;
	u16 *p = (u16 *)data;
	msgid = (*(p + 1)) & 0x3F;
	ifid  = (*(p + 1)) >> 6;
	ifid &= 0xF;
	mib = *(p + 2);

	WARN_ON(msgnames[msgid] == NULL);

	if (msgid == 0x0006) {
		dev_dbg(dev, "vif %d: sdio tx, msgid %s(0x%.4X) len %d MIB %s(0x%.4X)\n",
				ifid, msgnames[msgid], msgid,*p, mibnames[mib], mib);
	} else {
		dev_dbg(dev, "vif %d: sdio tx, msgid %s(0x%.4X) len %d\n", ifid, msgnames[msgid], msgid, *p);
	}

//	print_hex_dump_bytes("--> ", DUMP_PREFIX_NONE, data,
//	                     min(len, (size_t) 64));
#endif
}

static int xradio_bh_tx(struct xradio_common *hw_priv){
	int txavailable;
	int txburst;
	int vif_selected;
	struct wsm_hdr *wsm;
	size_t tx_len;
	int ret;
	u8 *data;

	BUG_ON(hw_priv->hw_bufs_used > hw_priv->wsm_caps.numInpChBufs);
	txavailable = hw_priv->wsm_caps.numInpChBufs - hw_priv->hw_bufs_used;
	if (txavailable) {
		/* Wake up the devices */
		if (hw_priv->device_can_sleep) {
			ret = xradio_device_wakeup(hw_priv);
			if (WARN_ON(ret < 0)) {
				return -1;
			} else if (ret) {
				hw_priv->device_can_sleep = false;
			} else { /* Wait for "awake" interrupt */
				dev_dbg(hw_priv->pdev,
						"need to wait for device to wake before doing tx\n");
				return 0;
			}
		}
		/* Increase Tx buffer*/
		wsm_alloc_tx_buffer(hw_priv);

		/* Get data to send and send it. */
		ret = wsm_get_tx(hw_priv, &data, &tx_len, &txburst, &vif_selected);
		if (ret <= 0) {
			wsm_release_tx_buffer(hw_priv, 1);
			if (WARN_ON(ret < 0)) {
				dev_err(hw_priv->pdev, "wsm_get_tx=%d.\n", ret);
				return -ENOMEM;
			} else {
				return 0;
			}
		} else {
			wsm = (struct wsm_hdr *) data;
			BUG_ON(tx_len < sizeof(*wsm));
			BUG_ON(__le32_to_cpu(wsm->len) != tx_len);

			/* Align tx length and check it. */
			if (tx_len <= 8)
			tx_len = 16;
			tx_len = sdio_align_len(hw_priv, tx_len);

			/* Check if not exceeding XRADIO capabilities */
			if (tx_len > EFFECTIVE_BUF_SIZE) {
				dev_warn(hw_priv->pdev, "Write aligned len: %d\n", tx_len);
			}

			/* Make sequence number. */
			wsm->id &= __cpu_to_le32(~WSM_TX_SEQ(WSM_TX_SEQ_MAX));
			wsm->id |= cpu_to_le32(WSM_TX_SEQ(hw_priv->wsm_tx_seq));

			/* Send the data to devices. */
			if (WARN_ON(xradio_data_write(hw_priv, data, tx_len))) {
				wsm_release_tx_buffer(hw_priv, 1);
				dev_err(hw_priv->pdev, "xradio_data_write failed\n");
				return -EIO;
			}

			xradio_bh_tx_dump(hw_priv->pdev, data, tx_len);

			/* Process after data have sent. */
			if (vif_selected != -1) {
				hw_priv->hw_bufs_used_vif[vif_selected]++;
			}
			wsm_txed(hw_priv, data);
			hw_priv->wsm_tx_seq = (hw_priv->wsm_tx_seq + 1) & WSM_TX_SEQ_MAX;

			return 1;
		}
	} else
		return 0;
}

static int xradio_bh_exchange(struct xradio_common *hw_priv) {
	int rxdone;
	int txdone;
	u16 nextlen = 0;

	/* query stuck frames in firmware. */
	if (atomic_xchg(&hw_priv->query_cnt, 0)) {
		if (schedule_work(&hw_priv->query_work) <= 0)
			atomic_add(1, &hw_priv->query_cnt);
	}

	/* keep doing tx and rx until they both stop or we are told
	 * to terminate */
	do {
		txdone = xradio_bh_tx(hw_priv);
		if (txdone < 0) {
			break;
		}
		rxdone = xradio_bh_rx(hw_priv, &nextlen);
		if (rxdone < 0) {
			break;
		}
	} while ((txdone > 0 || rxdone > 0) && !kthread_should_stop());
	return 0;
}

static int xradio_bh(void *arg)
{
	struct xradio_common *hw_priv = arg;
	int term, suspend;
	int wake = 0;
	long timeout;
	long status;

	for (;;) {
		timeout = HZ / 30;

		// wait for something to happen or a timeout
		status = wait_event_interruptible_timeout(hw_priv->bh_wq, ( {
					wake = atomic_xchg(&hw_priv->bh_tx, 0);
					term = kthread_should_stop();
					suspend = atomic_read(&hw_priv->bh_suspend);
					(wake || term || suspend);}), timeout);

		if (wake) {
			if(xradio_bh_exchange(hw_priv) < 0){
				break;
			}
		} else if (term) {
			dev_dbg(hw_priv->pdev, "xradio_bh exit!\n");
			break;
		} else if (status < 0) {
			dev_err(hw_priv->pdev, "bh_error=%d, status=%ld\n",
					hw_priv->bh_error, status);
			break;
		} else if (!status) {
			/* check if there is data waiting but we missed the interrupt*/
			if (xradio_bh_rx_availlen(hw_priv) > 0) {
//				dev_warn(hw_priv->pdev, "missed interrupt\n");
				if(xradio_bh_exchange(hw_priv) < 0){
					break;
				}
			}
			/* There are some frames to be confirmed. */
			else if (hw_priv->hw_bufs_used) {
				long timeout = 0;
				bool pending = 0;
				dev_dbg(hw_priv->pdev, "Need confirm:%d!\n",
						hw_priv->hw_bufs_used);
				/* Check if frame transmission is timed out. */
				pending = xradio_query_txpkt_timeout(hw_priv, XRWL_ALL_IFS,
						hw_priv->pending_frame_id, &timeout);
				/* There are some frames confirm time out. */
				if (pending && timeout < 0) {
					dev_err(hw_priv->pdev, "query_txpkt_timeout:%ld!\n",
							timeout);
					break;
				}
			} //else if (!txpending){
			  //if (hw_priv->powersave_enabled && !hw_priv->device_can_sleep && !atomic_read(&hw_priv->recent_scan)) {
			  //	/* Device is idle, we can go to sleep. */
			  //	dev_dbg(hw_priv->pdev, "Device idle(timeout), can sleep.\n");
			  //	WARN_ON(xradio_reg_write_16(hw_priv, HIF_CONTROL_REG_ID, 0));
			  //	hw_priv->device_can_sleep = true;
			  //}
			  //continue;
			  //}
		} else if (suspend) {
			dev_dbg(hw_priv->pdev, "Host suspend request.\n");
			/* Check powersave setting again. */
			if (hw_priv->powersave_enabled) {
				dev_dbg(hw_priv->pdev,
						"Device idle(host suspend), can sleep.\n");
				WARN_ON(xradio_reg_write_16(hw_priv, HIF_CONTROL_REG_ID, 0));
				hw_priv->device_can_sleep = true;
			}

			/* bh thread go to suspend. */
			atomic_set(&hw_priv->bh_suspend, XRADIO_BH_SUSPENDED);
			wake_up(&hw_priv->bh_evt_wq);
			status = wait_event_interruptible(hw_priv->bh_wq,
					XRADIO_BH_RESUME == atomic_read(&hw_priv->bh_suspend));

			if (status < 0) {
				dev_err(hw_priv->pdev, "Failed to wait for resume: %ld.\n",
						status);
				break;
			}
			dev_dbg(hw_priv->pdev, "Host resume.\n");
			atomic_set(&hw_priv->bh_suspend, XRADIO_BH_RESUMED);
			wake_up(&hw_priv->bh_evt_wq);
		}
	} /* for (;;)*/

	dev_err(hw_priv->pdev, "bh thread exiting\n");

	return 0;
}
