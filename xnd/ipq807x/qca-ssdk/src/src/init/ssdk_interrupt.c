/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
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
#include "fal_init.h"
#include "fal_reg_access.h"
#include "sw.h"
#include "ssdk_init.h"
#include "fal_init.h"
#include <linux/phy.h>
#include <linux/kernel.h>
#include <linux/kconfig.h>
#include <linux/version.h>
#include <linux/module.h>
#include <generated/autoconf.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/if_arp.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/gpio.h>
#if defined(ISIS)
#include <isis/isis_reg.h>
#elif defined(ISISC)
#include <isisc/isisc_reg.h>
#else
#include <dess/dess_reg.h>
#endif
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,1,0))
#include <linux/of.h>
#elif defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
#include <linux/of.h>
#include <drivers/leds/leds-ipq40xx.h>
#include <linux/of_platform.h>
#include <linux/reset.h>
#else
#include <linux/ar8216_platform.h>
#endif
#include "ssdk_plat.h"
#include "ref_port_ctrl.h"

#define LINK_CHANGE_INTR  0x8000
/*phy interrupt enable and status register*/
#define INTERRUPT_ENABLE_REGISTER  0X12
#define INTERRUPT_STATUS_REGISTER 0X13

extern void qca_ar8327_sw_mac_polling_task(struct qca_phy_priv *priv);

static int qca_phy_disable_intr(struct qca_phy_priv *priv)
{
	a_uint32_t  phy_number = 0;
	a_uint16_t  value;

	for(phy_number = 0; phy_number < 5;  phy_number++)
	{
		value = 0;
		priv->phy_write(priv->device_id, phy_number, INTERRUPT_ENABLE_REGISTER, value);
		priv->phy_read(priv->device_id, phy_number, INTERRUPT_STATUS_REGISTER, &value);
	}

	return 0;
}

static int qca_mac_disable_intr(struct qca_phy_priv *priv)
{
	a_uint32_t data;

	fal_reg_get(priv->device_id, GBL_INT_MASK1_OFFSET, (a_uint8_t *)&data, 4);
	if (data )
	{
		data = 0;
		fal_reg_set(priv->device_id, GBL_INT_MASK1_OFFSET,(a_uint8_t *)&data, 4);
	}
	/*fal_reg_get(0, 0x20, (a_uint8_t *)&data, 4);
	 if (data )
	  {
		   data = 0;
		   fal_reg_set(0, 0x20,(a_uint8_t *)&data, 4);
	  }

	fal_reg_get(0, 0x28, (a_uint8_t *)&data, 4);
	fal_reg_set(0, 0x28,(a_uint8_t *)&data, 4);
   */

	fal_reg_get(priv->device_id, GBL_INT_STATUS1_OFFSET, (a_uint8_t *)&data, 4);
	fal_reg_set(priv->device_id, GBL_INT_STATUS1_OFFSET,(a_uint8_t *)&data, 4);

	return 0;
}

static int qca_phy_enable_intr(struct qca_phy_priv *priv)
{
	a_uint16_t  value = 0;
	a_uint32_t phy_number;

	for(phy_number = 0;  phy_number < 5; phy_number++)
	{
		priv->phy_read(priv->device_id, phy_number, INTERRUPT_STATUS_REGISTER, &value);
		/*enable link change intr*/
		if( !priv->link_polling_required)
			value = 0xc00;
		priv->phy_write(priv->device_id,phy_number, INTERRUPT_ENABLE_REGISTER, value);
	}

	return 0;
}

int qca_mac_enable_intr(struct qca_phy_priv *priv)
{
	a_uint32_t data = 0;

	/*enable link change intr*/
	if( !priv->link_polling_required)
		data = 0x8000;
	fal_reg_set(priv->device_id, GBL_INT_MASK1_OFFSET, (a_uint8_t *)&data, 4);

	return 0;
}
static int qca_phy_clean_intr(struct qca_phy_priv *priv)
{
	a_uint32_t phy_number;
	a_uint16_t value;

	for(phy_number = 0; phy_number < 5; phy_number++)
		priv->phy_read(priv->device_id, phy_number, INTERRUPT_STATUS_REGISTER, &value);

	return 0;
}

static int qca_mac_clean_intr(struct qca_phy_priv *priv)
{
	a_uint32_t data;

	fal_reg_get(priv->device_id, GBL_INT_STATUS1_OFFSET, (a_uint8_t *) &data, 4);
	fal_reg_set(priv->device_id,  GBL_INT_STATUS1_OFFSET, (a_uint8_t *)&data, 4);

	return 0;
}

static void
qca_link_change_task(struct qca_phy_priv *priv)
{
	SSDK_DEBUG("qca_link_change_task is running\n");
	mutex_lock(&priv->qm_lock);
	qca_ar8327_sw_mac_polling_task(priv);
	mutex_unlock(&priv->qm_lock);
}

static void
qca_intr_workqueue_task(struct work_struct *work)
{
	a_uint32_t data;
	struct qca_phy_priv *priv = container_of(work, struct qca_phy_priv,  intr_workqueue);

	fal_reg_get(priv->device_id,  GBL_INT_STATUS1_OFFSET, (a_uint8_t*)&data, 4);
	qca_phy_clean_intr(priv);
	qca_mac_clean_intr(priv);
	SSDK_DEBUG("data:%x, priv->version:%x\n", data, priv->version);
	switch(priv->version)
	{
		case QCA_VER_DESS:
			qca_link_change_task(priv);
			break;
		default:
			if((data &LINK_CHANGE_INTR))
				qca_link_change_task(priv);
			break;
	}
	enable_irq(priv->link_interrupt_no);
}

 static irqreturn_t  qca_link_intr_handle(int irq, void *phy_priv)
 {
	struct qca_phy_priv *priv = (struct qca_phy_priv *)phy_priv;

	 disable_irq_nosync(irq);
	 schedule_work(&priv->intr_workqueue);
        SSDK_DEBUG("irq number is :%x\n",irq);

	 return IRQ_HANDLED;
 }

 int qca_intr_init(struct qca_phy_priv *priv)
{
	SSDK_DEBUG("start to  init the interrupt!\n");
	mutex_init(&priv->qm_lock);
	INIT_WORK(&priv->intr_workqueue, qca_intr_workqueue_task);
	qca_phy_disable_intr(priv);
	qca_mac_disable_intr(priv);
	if(request_irq(priv->link_interrupt_no, qca_link_intr_handle, priv->interrupt_flag, priv->link_intr_name, priv))
		return -1;
	qca_phy_enable_intr(priv);
	qca_mac_enable_intr(priv);

	return 0;
}

