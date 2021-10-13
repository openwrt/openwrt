/*
 * Copyright (c) 2018, 2020-2021, The Linux Foundation. All rights reserved.
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
/*qca808x_start*/
#include "sw.h"
#include "fal_port_ctrl.h"
#include "hsl_api.h"
#include "hsl.h"
#include "hsl_phy.h"
#include "ssdk_plat.h"
#include "qca808x_phy.h"
/*qca808x_end*/
#if defined(IN_PTP)
#include "qca808x_ptp.h"
#endif
#include "qca808x.h"
#ifdef IN_LED
#include "qca808x_led.h"
#endif

static a_bool_t phy_dev_drv_init_flag = A_FALSE;
/*qca808x_start*/
static a_bool_t phy_ops_flag = A_FALSE;

static struct mutex qca808x_reg_lock;

#define QCA808X_LOCKER_INIT		mutex_init(&qca808x_reg_lock)
#define QCA808X_REG_LOCK		mutex_lock(&qca808x_reg_lock)
#define QCA808X_REG_UNLOCK		mutex_unlock(&qca808x_reg_lock)

/******************************************************************************
*
* qca808x_phy_mii_read - mii register read
*
* mii register read
*/
a_uint16_t
qca808x_phy_reg_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id)
{
	sw_error_t rv = SW_OK;
	a_uint16_t phy_data = 0;
#if defined(IN_PHY_I2C_MODE)
	a_uint32_t port_id = qca_ssdk_phy_addr_to_port(dev_id, phy_id);
	a_uint8_t phy_access_type = hsl_port_phy_access_type_get(dev_id, port_id);

	if (phy_access_type == PHY_I2C_ACCESS) {
		HSL_PHY_I2C_GET(rv, dev_id, phy_id, reg_id, &phy_data);
	}
	else
#endif
	{
		HSL_PHY_GET(rv, dev_id, phy_id, reg_id, &phy_data);
	}

	if (rv != SW_OK) {
		return PHY_INVALID_DATA;
	}

	return phy_data;
}

/******************************************************************************
*
* qca808x_phy_mii_write - mii register write
*
* mii register write
*/
sw_error_t
qca808x_phy_reg_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id,
		       a_uint16_t reg_val)
{
	sw_error_t rv;
#if defined(IN_PHY_I2C_MODE)
	a_uint32_t port_id = qca_ssdk_phy_addr_to_port(dev_id, phy_id);
	a_uint8_t phy_access_type = hsl_port_phy_access_type_get(dev_id, port_id);

	if (phy_access_type == PHY_I2C_ACCESS) {
		HSL_PHY_I2C_SET(rv, dev_id, phy_id, reg_id, reg_val);
	}
	else
#endif
	{
		HSL_PHY_SET(rv, dev_id, phy_id, reg_id, reg_val);
	}

	return rv;

}

/******************************************************************************
*
* qca808x_phy_debug_write - debug port write
*
* debug port write
*/
sw_error_t
qca808x_phy_debug_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id,
		       a_uint16_t reg_val)
{
	sw_error_t rv = SW_OK;

	QCA808X_REG_LOCK;
	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_DEBUG_PORT_ADDRESS, reg_id);
	if (rv != SW_OK)
	{
		QCA808X_REG_UNLOCK;
		SSDK_ERROR("qca808x_phy_reg_write failed\n");
		return SW_FAIL;
	}

	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_DEBUG_PORT_DATA, reg_val);
	if (rv != SW_OK)
	{
		QCA808X_REG_UNLOCK;
		SSDK_ERROR("qca808x_phy_reg_write failed\n");
		return SW_FAIL;
	}
	QCA808X_REG_UNLOCK;

	return rv;
}

/******************************************************************************
*
* qca808x_phy_debug_read - debug port read
*
* debug port read
*/
a_uint16_t
qca808x_phy_debug_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id)
{
	sw_error_t rv = SW_OK;
	a_uint16_t phy_data = 0;

	QCA808X_REG_LOCK;
	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_DEBUG_PORT_ADDRESS, reg_id);
	if (rv != SW_OK) {
		QCA808X_REG_UNLOCK;
		SSDK_DEBUG("qca808x_phy_reg_write failed\n");
		return PHY_INVALID_DATA;
	}
	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_DEBUG_PORT_DATA);
	if (phy_data == PHY_INVALID_DATA) {
		QCA808X_REG_UNLOCK;
		SSDK_DEBUG("qca808x_phy_reg_read failed\n");
		return PHY_INVALID_DATA;
	}
	QCA808X_REG_UNLOCK;

	return phy_data;
}

/******************************************************************************
*
* qca808x_phy_mmd_write - PHY MMD register write
*
* PHY MMD register write
*/
sw_error_t
qca808x_phy_mmd_write(a_uint32_t dev_id, a_uint32_t phy_id,
		     a_uint16_t mmd_num, a_uint16_t reg_id, a_uint16_t reg_val)
{
	sw_error_t rv;
	a_uint32_t reg_id_c45 = QCA808X_REG_C45_ADDRESS(mmd_num, reg_id);
#if defined(IN_PHY_I2C_MODE)
	a_uint32_t port_id = qca_ssdk_phy_addr_to_port(dev_id, phy_id);
	a_uint8_t phy_access_type = hsl_port_phy_access_type_get(dev_id, port_id);

	if (phy_access_type == PHY_I2C_ACCESS) {
		HSL_PHY_I2C_SET(rv, dev_id, phy_id, reg_id_c45, reg_val);
	}
	else
#endif
	{
		HSL_PHY_SET(rv, dev_id, phy_id, reg_id_c45, reg_val);
	}

	return rv;
}

/******************************************************************************
*
* qca808x_phy_mmd_read -  PHY MMD register read
*
* PHY MMD register read
*/
a_uint16_t
qca808x_phy_mmd_read(a_uint32_t dev_id, a_uint32_t phy_id,
		    a_uint16_t mmd_num, a_uint16_t reg_id)
{
	sw_error_t rv = SW_OK;
	a_uint16_t phy_data = 0;
	a_uint32_t reg_id_c45 = QCA808X_REG_C45_ADDRESS(mmd_num, reg_id);
#if defined(IN_PHY_I2C_MODE)
	a_uint32_t port_id = qca_ssdk_phy_addr_to_port(dev_id, phy_id);
	a_uint8_t phy_access_type = hsl_port_phy_access_type_get(dev_id, port_id);

	if (phy_access_type == PHY_I2C_ACCESS) {
		HSL_PHY_I2C_GET(rv, dev_id, phy_id, reg_id_c45, &phy_data);
	}
	else
#endif
	{
		HSL_PHY_GET(rv, dev_id, phy_id, reg_id_c45, &phy_data);
	}

	if (rv != SW_OK) {
		return PHY_INVALID_DATA;
	}

	return phy_data;
}

static sw_error_t
qca808x_phy_ms_random_seed_set(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_debug_read(dev_id, phy_id,
		QCA808X_DEBUG_LOCAL_SEED);
	phy_data &= ~(QCA808X_MASTER_SLAVE_SEED_CFG);
	phy_data |= (prandom_u32()%QCA808X_MASTER_SLAVE_SEED_RANGE) << 2;
	SSDK_DEBUG("QCA808X_DEBUG_LOCAL_SEED:%x\n", phy_data);
	rv = qca808x_phy_debug_write(dev_id, phy_id,
		QCA808X_DEBUG_LOCAL_SEED, phy_data);

	return rv;
}

static sw_error_t
qca808x_phy_ms_seed_enable(a_uint32_t dev_id, a_uint32_t phy_id,
	a_bool_t enable)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_debug_read(dev_id, phy_id,
		QCA808X_DEBUG_LOCAL_SEED);
	if(enable)
	{
		phy_data |= QCA808X_MASTER_SLAVE_SEED_ENABLE;
	}
	else
	{
		phy_data &= ~(QCA808X_MASTER_SLAVE_SEED_ENABLE);
	}
	rv = qca808x_phy_debug_write(dev_id, phy_id,
		QCA808X_DEBUG_LOCAL_SEED, phy_data);

	return rv;
}

a_bool_t
qca808x_phy_2500caps(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD1_NUM,
							QCA808X_MMD1_PMA_CAP_REG);

	if (phy_data & QCA808X_STATUS_2500T_FD_CAPS) {
		return A_TRUE;
	}

	return A_FALSE;

}

/******************************************************************************
*
* qca808x_phy_get status
*
* get phy status
*/
sw_error_t
qca808x_phy_get_status(a_uint32_t dev_id, a_uint32_t phy_id,
		struct port_phy_status *phy_status)
{
	a_uint16_t phy_data;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_SPEC_STATUS);
	PHY_RTN_ON_READ_ERROR(phy_data);

	/*get phy link status*/
	if (phy_data & QCA808X_STATUS_LINK_PASS) {
		phy_status->link_status = A_TRUE;
	}
	else {
		phy_status->link_status = A_FALSE;
		if (qca808x_phy_2500caps(dev_id, phy_id) == A_TRUE) {
			SW_RTN_ON_ERROR(
				qca808x_phy_ms_random_seed_set (dev_id, phy_id));
			/*protect logic, if MASTER_SLAVE_CONFIG_FAULT is 1,
				then disable this logic*/
			phy_data = qca808x_phy_reg_read(dev_id, phy_id,
				QCA808X_1000BASET_STATUS);
			if ((phy_data & QCA808X_MASTER_SLAVE_CONFIG_FAULT) >> 15)
			{
				SW_RTN_ON_ERROR(
					qca808x_phy_ms_seed_enable (dev_id, phy_id, A_FALSE));
				SSDK_INFO("master_slave_config_fault was set\n");
			}
		}

		return SW_OK;
	}

	/*get phy speed*/
	switch (phy_data & QCA808X_STATUS_SPEED_MASK) {
		case QCA808X_STATUS_SPEED_2500MBS:
			phy_status->speed = FAL_SPEED_2500;
			break;
		case QCA808X_STATUS_SPEED_1000MBS:
			phy_status->speed = FAL_SPEED_1000;
			break;
		case QCA808X_STATUS_SPEED_100MBS:
			phy_status->speed = FAL_SPEED_100;
			break;
		case QCA808X_STATUS_SPEED_10MBS:
			phy_status->speed = FAL_SPEED_10;
			break;
		default:
			return SW_READ_ERROR;
	}

	/*get phy duplex*/
	if (phy_data & QCA808X_STATUS_FULL_DUPLEX) {
		phy_status->duplex = FAL_FULL_DUPLEX;
	} else {
		phy_status->duplex = FAL_HALF_DUPLEX;
	}

	/* get phy flowctrl resolution status */
	if (phy_data & QCA808X_PHY_RX_FLOWCTRL_STATUS) {
		phy_status->rx_flowctrl = A_TRUE;
	} else {
		phy_status->rx_flowctrl = A_FALSE;
	}

	if (phy_data & QCA808X_PHY_TX_FLOWCTRL_STATUS) {
		phy_status->tx_flowctrl = A_TRUE;
	} else {
		phy_status->tx_flowctrl = A_FALSE;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca808x_phy_get_speed - Determines the speed of phy ports associated with the
* specified device.
*/

sw_error_t
qca808x_phy_get_speed(a_uint32_t dev_id, a_uint32_t phy_id,
		     fal_port_speed_t * speed)
{
	sw_error_t rv = SW_OK;
	struct port_phy_status phy_status = {0};

	rv = qca808x_phy_get_status(dev_id, phy_id, &phy_status);
	PHY_RTN_ON_ERROR(rv);

	if (phy_status.link_status == A_TRUE) {
		*speed = phy_status.speed;
	} else {
		*speed = FAL_SPEED_10;
	}

	return rv;
}

/******************************************************************************
*
* qca808x_phy_set_force_speed - Force the speed of qca808x phy ports associated with the
* specified device.
*/
sw_error_t
qca808x_phy_set_force_speed(a_uint32_t dev_id, a_uint32_t phy_id,
		     fal_port_speed_t speed)
{
	a_uint16_t phy_data1 = 0;
	a_uint16_t phy_data2 = 0;
	sw_error_t rv = SW_OK;

	/* the speed of qca808x controled by MMD1 PMA/PMD control register */
	phy_data1 = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD1_NUM,
			QCA808X_PHY_MMD1_PMA_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data1);
	phy_data1 &= ~QCA808X_PMA_CONTROL_SPEED_MASK;

	phy_data2 = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD1_NUM,
			QCA808X_PHY_MMD1_PMA_TYPE);
	PHY_RTN_ON_READ_ERROR(phy_data2);
	phy_data2 &= ~QCA808X_PMA_TYPE_MASK;

	switch(speed)
	{
		case FAL_SPEED_2500:
			phy_data1 |= QCA808X_PMA_CONTROL_2500M;
			phy_data2 |= QCA808X_PMA_TYPE_2500M;
			break;
		case FAL_SPEED_1000:
			phy_data1 |= QCA808X_PMA_CONTROL_1000M;
			phy_data2 |= QCA808X_PMA_TYPE_1000M;
			break;
		case FAL_SPEED_100:
			phy_data1 |= QCA808X_PMA_CONTROL_100M;
			phy_data2 |= QCA808X_PMA_TYPE_100M;
			break;
		case FAL_SPEED_10:
			phy_data1 |= QCA808X_PMA_CONTROL_10M;
			phy_data2 |= QCA808X_PMA_TYPE_10M;
			break;
		default:
			return SW_BAD_PARAM;
	}

	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD1_NUM,
			QCA808X_PHY_MMD1_PMA_CONTROL, phy_data1);
	PHY_RTN_ON_ERROR(rv);

	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD1_NUM,
			QCA808X_PHY_MMD1_PMA_TYPE, phy_data2);

	return rv;
}

sw_error_t
_qca808x_phy_set_autoneg_adv_ext(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t autoneg)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
			QCA808X_PHY_MMD7_AUTONEGOTIATION_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (autoneg & FAL_PHY_ADV_2500T_FD) {
		phy_data |= QCA808X_ADVERTISE_2500FULL;
	} else {
		phy_data &= ~QCA808X_ADVERTISE_2500FULL;
	}

	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
			QCA808X_PHY_MMD7_AUTONEGOTIATION_CONTROL, phy_data);

	return rv;

}

/******************************************************************************
*
* qca808x_phy_set_speed - Determines the speed of phy ports associated with the
* specified device.
*/
sw_error_t
qca808x_phy_set_speed(a_uint32_t dev_id, a_uint32_t phy_id,
		     fal_port_speed_t speed)
{
	a_uint16_t phy_data = 0;
	fal_port_duplex_t old_duplex = QCA808X_CTRL_FULL_DUPLEX;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	switch(speed)
	{
		case FAL_SPEED_2500:
		case FAL_SPEED_1000:
			if (speed == FAL_SPEED_2500) {
				rv = _qca808x_phy_set_autoneg_adv_ext(dev_id, phy_id,
						FAL_PHY_ADV_2500T_FD);
				PHY_RTN_ON_ERROR(rv);
			} else {
				rv = _qca808x_phy_set_autoneg_adv_ext(dev_id, phy_id,
						~FAL_PHY_ADV_2500T_FD);
				PHY_RTN_ON_ERROR(rv);
			}
			phy_data |= QCA808X_CTRL_FULL_DUPLEX;
			phy_data |= QCA808X_CTRL_AUTONEGOTIATION_ENABLE;
			phy_data |= QCA808X_CTRL_RESTART_AUTONEGOTIATION;
			break;
		case FAL_SPEED_100:
		case FAL_SPEED_10:
			/* set qca808x phy speed by pma control registers */
			rv = qca808x_phy_set_force_speed(dev_id, phy_id, speed);
			PHY_RTN_ON_ERROR(rv);
			rv = qca808x_phy_get_duplex(dev_id, phy_id, &old_duplex);
			PHY_RTN_ON_ERROR(rv);

			if (old_duplex == FAL_FULL_DUPLEX) {
				phy_data |= QCA808X_CTRL_FULL_DUPLEX;
			}
			else if (old_duplex == FAL_HALF_DUPLEX) {
				phy_data &= ~QCA808X_CTRL_FULL_DUPLEX;
			}
			phy_data &= ~QCA808X_CTRL_AUTONEGOTIATION_ENABLE;
			break;
		default:
			return SW_BAD_PARAM;
	}

	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_PHY_CONTROL, phy_data);

	return rv;
}

/******************************************************************************
*
* qca808x_phy_set_duplex - Determines the duplex of phy ports associated with the
* specified device.
*/
sw_error_t
qca808x_phy_set_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
		      fal_port_duplex_t duplex)
{
	a_uint16_t phy_data = 0;
	fal_port_speed_t old_speed;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	rv = qca808x_phy_get_speed(dev_id, phy_id, &old_speed);
	PHY_RTN_ON_ERROR(rv);

	switch(old_speed)
	{
		case FAL_SPEED_2500:
		case FAL_SPEED_1000:
			if (duplex == FAL_FULL_DUPLEX) {
				phy_data |= QCA808X_CTRL_FULL_DUPLEX;
			} else {
				return SW_NOT_SUPPORTED;
			}
			phy_data |= QCA808X_CTRL_AUTONEGOTIATION_ENABLE;

			if (old_speed == FAL_SPEED_2500) {
				rv = _qca808x_phy_set_autoneg_adv_ext(dev_id, phy_id,
						FAL_PHY_ADV_2500T_FD);
				PHY_RTN_ON_ERROR(rv);
			} else {
				rv = _qca808x_phy_set_autoneg_adv_ext(dev_id, phy_id,
						~FAL_PHY_ADV_2500T_FD);
				PHY_RTN_ON_ERROR(rv);
			}
			break;
		case FAL_SPEED_100:
		case FAL_SPEED_10:
			/* force the speed */
			rv = qca808x_phy_set_force_speed(dev_id, phy_id, old_speed);
			PHY_RTN_ON_ERROR(rv);
			phy_data &= ~QCA808X_CTRL_AUTONEGOTIATION_ENABLE;
			if (duplex == FAL_FULL_DUPLEX) {
				phy_data |= QCA808X_CTRL_FULL_DUPLEX;
			} else {
				phy_data &= ~QCA808X_CTRL_FULL_DUPLEX;
			}
			break;
		default:
			return SW_FAIL;
	}
	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_PHY_CONTROL, phy_data);

	return rv;
}

/******************************************************************************
*
* qca808x_phy_get_duplex - Determines the duplex of phy ports associated with the
* specified device.
*/
sw_error_t
qca808x_phy_get_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
		      fal_port_duplex_t * duplex)
{
	sw_error_t rv = SW_OK;
	struct port_phy_status phy_status = {0};

	rv = qca808x_phy_get_status(dev_id, phy_id, &phy_status);
	PHY_RTN_ON_ERROR(rv);

	if (phy_status.link_status == A_TRUE) {
		*duplex = phy_status.duplex;
	} else {
		*duplex = FAL_HALF_DUPLEX;
	}

	return rv;
}

/******************************************************************************
*
* qca808x_phy_reset - reset the phy
*
* reset the phy
*/
sw_error_t qca808x_phy_reset(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_PHY_CONTROL,
			     phy_data | QCA808X_CTRL_SOFTWARE_RESET);
	SW_RTN_ON_ERROR(rv);
	/*the configure will lost when reset.*/
	rv = qca808x_phy_ms_seed_enable(dev_id, phy_id, A_TRUE);

	return rv;
}
/******************************************************************************
*
* qca808x_phy_status - test to see if the specified phy link is alive
*
* RETURNS:
*    A_TRUE  --> link is alive
*    A_FALSE --> link is down
*/
a_bool_t qca808x_phy_get_link_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
	struct port_phy_status phy_status = {0};
	sw_error_t rv = SW_OK;

	rv = qca808x_phy_get_status(dev_id, phy_id, &phy_status);
	if (rv == SW_OK) {
		return phy_status.link_status;
	} else {
		return A_FALSE;
	}
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* qca808x_phy_cdt - cable diagnostic test
*
* cable diagnostic test
*/

static inline fal_cable_status_t _phy_cdt_status_mapping(a_uint16_t status)
{
	fal_cable_status_t status_mapping = FAL_CABLE_STATUS_INVALID;

	switch (status) {
		case 0:
			status_mapping = FAL_CABLE_STATUS_INVALID;
			break;
		case 1:
			status_mapping = FAL_CABLE_STATUS_NORMAL;
			break;
		case 2:
			status_mapping = FAL_CABLE_STATUS_OPENED;
			break;
		case 3:
			status_mapping = FAL_CABLE_STATUS_SHORT;
			break;
	}

	return status_mapping;
}

static sw_error_t qca808x_phy_cdt_start(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t status = 0;
	a_uint16_t ii = 100;
	sw_error_t rv = SW_OK;

	/* RUN CDT */
	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_PHY_CDT_CONTROL,
			     QCA808X_RUN_CDT | QCA808X_CABLE_LENGTH_UNIT);
	PHY_RTN_ON_ERROR(rv);

	do {
		aos_mdelay(30);
		status =
		    qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CDT_CONTROL);
		PHY_RTN_ON_READ_ERROR(status);
	}
	while ((status & QCA808X_RUN_CDT) && (--ii));

	if (ii == 0) {
		return SW_TIMEOUT;
	} else {
		return SW_OK;
	}
}

sw_error_t
qca808x_phy_cdt(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t mdi_pair,
	       fal_cable_status_t * cable_status, a_uint32_t * cable_len)
{
	a_uint16_t cable_delta_time = 0;
	a_uint16_t status;
	sw_error_t rv;

	if ((mdi_pair >= QCA808X_MDI_PAIR_NUM)) {
		return SW_BAD_PARAM;
	}

	rv = qca808x_phy_cdt_start(dev_id, phy_id);

	if (rv != SW_OK) {
		*cable_status = FAL_CABLE_STATUS_INVALID;
		*cable_len = 0;
		return rv;
	}

	/* Get cable status */
	status = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
			QCA808X_PHY_CDT_STATUS);
	PHY_RTN_ON_READ_ERROR(status);

	switch (mdi_pair) {
		case 0:
			*cable_status =
				_phy_cdt_status_mapping((status >> 12) & 0x3);
			cable_delta_time =
				qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
						QCA808X_PHY_CDT_DIAG_PAIR0);
			PHY_RTN_ON_READ_ERROR(cable_delta_time);

			break;
		case 1:
			*cable_status =
				_phy_cdt_status_mapping((status >> 8) & 0x3);
			cable_delta_time =
				qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
						QCA808X_PHY_CDT_DIAG_PAIR1);
			PHY_RTN_ON_READ_ERROR(cable_delta_time);

			break;
		case 2:
			*cable_status =
				_phy_cdt_status_mapping((status >> 4) & 0x3);
			cable_delta_time =
				qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
						QCA808X_PHY_CDT_DIAG_PAIR2);
			PHY_RTN_ON_READ_ERROR(cable_delta_time);

			break;
		case 3:
			*cable_status =
				_phy_cdt_status_mapping(status & 0x3);
			cable_delta_time =
				qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
						QCA808X_PHY_CDT_DIAG_PAIR3);
			PHY_RTN_ON_READ_ERROR(cable_delta_time);

			break;
	}

	/* the actual cable length equals to CableDeltaTime * 0.824 */
	*cable_len = ((cable_delta_time & 0xff) * 824) / 1000;

	return rv;
}

/******************************************************************************
*
* qca808x_phy_set_mdix -
*
* set phy mdix configuraiton
*/
sw_error_t
qca808x_phy_set_mdix(a_uint32_t dev_id, a_uint32_t phy_id,
		    fal_port_mdix_mode_t mode)
{
	a_uint16_t phy_data;
	sw_error_t rv;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_SPEC_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (mode == PHY_MDIX_AUTO) {
		phy_data |= QCA808X_PHY_MDIX_AUTO;
	} else if (mode == PHY_MDIX_MDIX) {
		phy_data &= ~QCA808X_PHY_MDIX_AUTO;
		phy_data |= QCA808X_PHY_MDIX;
	} else if (mode == PHY_MDIX_MDI) {
		phy_data &= ~QCA808X_PHY_MDIX_AUTO;
	} else {
		return SW_BAD_PARAM;
	}

	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_PHY_SPEC_CONTROL, phy_data);
	PHY_RTN_ON_ERROR(rv);

	rv = qca808x_phy_reset(dev_id, phy_id);
	return rv;
}

/******************************************************************************
*
* qca808x_phy_get_mdix
*
* get phy mdix configuration
*/
sw_error_t
qca808x_phy_get_mdix(a_uint32_t dev_id, a_uint32_t phy_id,
		    fal_port_mdix_mode_t * mode)
{
	a_uint16_t phy_data;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_SPEC_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if ((phy_data & QCA808X_PHY_MDIX_AUTO) == QCA808X_PHY_MDIX_AUTO) {
		*mode = PHY_MDIX_AUTO;
	} else if ((phy_data & QCA808X_PHY_MDIX) == QCA808X_PHY_MDIX) {
		*mode = PHY_MDIX_MDIX;
	} else {
		*mode = PHY_MDIX_MDI;
	}

	return SW_OK;

}

/******************************************************************************
*
* qca808x_phy_get_mdix status
*
* get phy mdix status
*/
sw_error_t
qca808x_phy_get_mdix_status(a_uint32_t dev_id, a_uint32_t phy_id,
			   fal_port_mdix_status_t * mode)
{
	a_uint16_t phy_data;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_SPEC_STATUS);
	PHY_RTN_ON_READ_ERROR(phy_data);

	*mode =
	    (phy_data & QCA808X_PHY_MDIX_STATUS) ? PHY_MDIX_STATUS_MDIX :
	    PHY_MDIX_STATUS_MDI;

	return SW_OK;

}

/******************************************************************************
*
* qca808x_phy_set_local_loopback
*
* set phy local loopback
*/
sw_error_t
qca808x_phy_set_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			      a_bool_t enable)
{
	a_uint16_t phy_data;
	fal_port_speed_t old_speed;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (enable == A_TRUE) {
		/* get the link speed first, then force the corresponding
		 * speed to enable local loopback */
		rv = qca808x_phy_get_speed(dev_id, phy_id, &old_speed);
		PHY_RTN_ON_ERROR(rv);
		rv = qca808x_phy_set_force_speed(dev_id, phy_id, old_speed);
		PHY_RTN_ON_ERROR(rv);

		phy_data &= ~QCA808X_CTRL_AUTONEGOTIATION_ENABLE;
		phy_data |= QCA808X_LOCAL_LOOPBACK_ENABLE;
		phy_data |= QCA808X_CTRL_FULL_DUPLEX;
	} else {
		phy_data = QCA808X_COMMON_CTRL;
	}

	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_PHY_CONTROL, phy_data);
	return rv;
}

/******************************************************************************
*
* qca808x_phy_get_local_loopback
*
* get phy local loopback
*/
sw_error_t
qca808x_phy_get_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			      a_bool_t * enable)
{
	a_uint16_t phy_data;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA808X_LOCAL_LOOPBACK_ENABLE) {
		*enable = A_TRUE;
	} else {
		*enable = A_FALSE;
	}

	return SW_OK;

}

/******************************************************************************
*
* qca808x_phy_set_remote_loopback
*
* set phy remote loopback
*/
sw_error_t
qca808x_phy_set_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			       a_bool_t enable)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
					QCA808X_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (enable == A_TRUE) {
		phy_data |= QCA808X_PHY_REMOTE_LOOPBACK_EN;
	} else {
		phy_data &= ~QCA808X_PHY_REMOTE_LOOPBACK_EN;
	}

	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
			     QCA808X_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL,
			     phy_data);
	return rv;

}

/******************************************************************************
*
* qca808x_phy_get_remote_loopback
*
* get phy remote loopback
*/
sw_error_t
qca808x_phy_get_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			       a_bool_t * enable)
{
	a_uint16_t phy_data;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
					QCA808X_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA808X_PHY_REMOTE_LOOPBACK_EN) {
		*enable = A_TRUE;
	} else {
		*enable = A_FALSE;
	}

	return SW_OK;

}
#endif
/******************************************************************************
*
* qca808x_set_autoneg_adv - set the phy autoneg Advertisement
*
*/
sw_error_t
qca808x_phy_set_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
			   a_uint32_t autoneg)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id,
			QCA808X_AUTONEG_ADVERT);
	PHY_RTN_ON_READ_ERROR(phy_data);

	phy_data &= ~QCA808X_ADVERTISE_MEGA_ALL;

	if (autoneg & FAL_PHY_ADV_100TX_FD) {
		phy_data |= QCA808X_ADVERTISE_100FULL;
	}

	if (autoneg & FAL_PHY_ADV_100TX_HD) {
		phy_data |= QCA808X_ADVERTISE_100HALF;
	}

	if (autoneg & FAL_PHY_ADV_10T_FD) {
		phy_data |= QCA808X_ADVERTISE_10FULL;
	}

	if (autoneg & FAL_PHY_ADV_10T_HD) {
		phy_data |= QCA808X_ADVERTISE_10HALF;
	}

	if (autoneg & FAL_PHY_ADV_PAUSE) {
		phy_data |= QCA808X_ADVERTISE_PAUSE;
	}

	if (autoneg & FAL_PHY_ADV_ASY_PAUSE) {
		phy_data |= QCA808X_ADVERTISE_ASYM_PAUSE;
	}
	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_AUTONEG_ADVERT,
			phy_data);
	PHY_RTN_ON_ERROR(rv);

	phy_data = qca808x_phy_reg_read(dev_id, phy_id,
				QCA808X_1000BASET_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	phy_data &= ~QCA808X_ADVERTISE_1000FULL;
	phy_data &= ~QCA808X_ADVERTISE_1000HALF;

	if (autoneg & FAL_PHY_ADV_1000T_FD) {
		phy_data |= QCA808X_ADVERTISE_1000FULL;
	}

	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_1000BASET_CONTROL,
			phy_data);
	PHY_RTN_ON_ERROR(rv);

	if (qca808x_phy_2500caps(dev_id, phy_id) == A_TRUE) {
		rv = _qca808x_phy_set_autoneg_adv_ext(dev_id, phy_id, autoneg);
	}

	return rv;
}

sw_error_t
_qca808x_phy_get_autoneg_adv_ext(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t *phy_data)
{
	*phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
			QCA808X_PHY_MMD7_AUTONEGOTIATION_CONTROL);
	PHY_RTN_ON_READ_ERROR(*phy_data);

	return SW_OK;
}

/******************************************************************************
*
* qca808x_get_autoneg_adv - get the phy autoneg Advertisement
*
*/
sw_error_t
qca808x_phy_get_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
			   a_uint32_t * autoneg)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	*autoneg = 0;
	phy_data =
		qca808x_phy_reg_read(dev_id, phy_id, QCA808X_AUTONEG_ADVERT);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA808X_ADVERTISE_100FULL) {
		*autoneg |= FAL_PHY_ADV_100TX_FD;
	}

	if (phy_data & QCA808X_ADVERTISE_100HALF) {
		*autoneg |= FAL_PHY_ADV_100TX_HD;
	}

	if (phy_data & QCA808X_ADVERTISE_10FULL) {
		*autoneg |= FAL_PHY_ADV_10T_FD;
	}

	if (phy_data & QCA808X_ADVERTISE_10HALF) {
		*autoneg |= FAL_PHY_ADV_10T_HD;
	}

	if (phy_data & QCA808X_ADVERTISE_PAUSE) {
		*autoneg |= FAL_PHY_ADV_PAUSE;
	}

	if (phy_data & QCA808X_ADVERTISE_ASYM_PAUSE) {
		*autoneg |= FAL_PHY_ADV_ASY_PAUSE;
	}

	phy_data = qca808x_phy_reg_read(dev_id, phy_id,
				QCA808X_1000BASET_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA808X_ADVERTISE_1000FULL) {
		*autoneg |= FAL_PHY_ADV_1000T_FD;
	}

	if (qca808x_phy_2500caps(dev_id, phy_id) == A_TRUE) {
		rv = _qca808x_phy_get_autoneg_adv_ext(dev_id, phy_id, &phy_data);
		if ((rv == SW_OK) &&
				(phy_data & QCA808X_ADVERTISE_2500FULL)) {
			*autoneg |= FAL_PHY_ADV_2500T_FD;
		}
	}

	return rv;
}

/******************************************************************************
*
* qca808x_phy_autoneg_status - get the phy autoneg status
*
*/
a_bool_t qca808x_phy_autoneg_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CONTROL);

	if (phy_data & QCA808X_CTRL_AUTONEGOTIATION_ENABLE) {
		return A_TRUE;
	}

	return A_FALSE;
}

/******************************************************************************
*
* qca808x_restart_autoneg - restart the phy autoneg
*
*/
sw_error_t qca808x_phy_restart_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	phy_data |= QCA808X_CTRL_AUTONEGOTIATION_ENABLE;
	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_PHY_CONTROL,
			     phy_data | QCA808X_CTRL_RESTART_AUTONEGOTIATION);

	return rv;
}

/******************************************************************************
*
* qca808x_phy_enable_autonego
*
*/
sw_error_t qca808x_phy_enable_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_PHY_CONTROL,
			     phy_data | QCA808X_CTRL_AUTONEGOTIATION_ENABLE);

	return rv;
}

/******************************************************************************
*
* qca808x_phy_get_phy_id - get the phy id
*
*/
sw_error_t
qca808x_phy_get_phy_id(a_uint32_t dev_id, a_uint32_t phy_id,
		      a_uint32_t *phy_data)
{
	a_uint16_t org_id, rev_id;
	org_id = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_ID1);
	PHY_RTN_ON_READ_ERROR(org_id);

	rev_id = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_ID2);
	PHY_RTN_ON_READ_ERROR(rev_id);

	*phy_data = ((org_id & 0xffff) << 16) | (rev_id & 0xffff);

	return SW_OK;
}

/******************************************************************************
*
* qca808x_phy_off - power off the phy
*
* Power off the phy
*/
sw_error_t qca808x_phy_poweroff(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_PHY_CONTROL,
				     phy_data | QCA808X_CTRL_POWER_DOWN);
	return rv;
}

/******************************************************************************
*
* qca808x_phy_on - power on the phy
*
* Power on the phy
*/
sw_error_t qca808x_phy_poweron(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_PHY_CONTROL,
			phy_data & ~QCA808X_CTRL_POWER_DOWN);

	aos_mdelay(200);

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* qca808x_phy_set_802.3az
*
* set 802.3az status
*/
sw_error_t
qca808x_phy_set_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
				       QCA808X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (enable == A_TRUE) {
		phy_data |= QCA808X_PHY_8023AZ_EEE_1000BT;
		phy_data |= QCA808X_PHY_8023AZ_EEE_100BT;
	} else {
		phy_data &= ~QCA808X_PHY_8023AZ_EEE_1000BT;
		phy_data &= ~QCA808X_PHY_8023AZ_EEE_100BT;
	}

	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
			QCA808X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL, phy_data);
	PHY_RTN_ON_ERROR(rv);

	rv = qca808x_phy_restart_autoneg(dev_id, phy_id);

	return rv;
}

/******************************************************************************
*
* qca808x_phy_get_8023az status
*
* get 8023az status
*/
sw_error_t
qca808x_phy_get_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	a_uint16_t phy_data;
	*enable = A_FALSE;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
				       QCA808X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if ((phy_data & QCA808X_PHY_8023AZ_EEE_1000BT) &&
			(phy_data & QCA808X_PHY_8023AZ_EEE_100BT)) {
		*enable = A_TRUE;
	}

	return SW_OK;
}

/******************************************************************************
*
* _qca808x_phy_get_8023az_status status
*
* get 8023az status
*/
sw_error_t
_qca808x_phy_get_8023az_status(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	a_uint16_t phy_data;
	*enable = A_FALSE;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
				       QCA808X_PHY_MMD3_ADDR_8023AZ_EEE_DB);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA808X_PHY_8023AZ_EEE_LP_STAT) {
		*enable = A_TRUE;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca808x_phy_set wol frame mac address
*
* set phy wol frame mac address
*/
sw_error_t
qca808x_phy_set_magic_frame_mac(a_uint32_t dev_id, a_uint32_t phy_id,
			       fal_mac_addr_t * mac)
{
	a_uint16_t phy_data1;
	a_uint16_t phy_data2;
	a_uint16_t phy_data3;
	sw_error_t rv = SW_OK;

	phy_data1 = (mac->uc[0] << 8) | mac->uc[1];
	phy_data2 = (mac->uc[2] << 8) | mac->uc[3];
	phy_data3 = (mac->uc[4] << 8) | mac->uc[5];

	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
			     QCA808X_PHY_MMD3_WOL_MAGIC_MAC_CTRL1, phy_data1);
	PHY_RTN_ON_ERROR(rv);

	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
			     QCA808X_PHY_MMD3_WOL_MAGIC_MAC_CTRL2, phy_data2);
	PHY_RTN_ON_ERROR(rv);

	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
			     QCA808X_PHY_MMD3_WOL_MAGIC_MAC_CTRL3, phy_data3);

	return rv;
}

/******************************************************************************
*
* qca808x_phy_get wol frame mac address
*
* get phy wol frame mac address
*/
sw_error_t
qca808x_phy_get_magic_frame_mac(a_uint32_t dev_id, a_uint32_t phy_id,
			       fal_mac_addr_t * mac)
{
	a_uint16_t phy_data1;
	a_uint16_t phy_data2;
	a_uint16_t phy_data3;

	phy_data1 = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
					QCA808X_PHY_MMD3_WOL_MAGIC_MAC_CTRL1);
	PHY_RTN_ON_READ_ERROR(phy_data1);

	phy_data2 = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
					QCA808X_PHY_MMD3_WOL_MAGIC_MAC_CTRL2);
	PHY_RTN_ON_READ_ERROR(phy_data2);

	phy_data3 = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
					QCA808X_PHY_MMD3_WOL_MAGIC_MAC_CTRL3);
	PHY_RTN_ON_READ_ERROR(phy_data3);

	mac->uc[0] = (phy_data1 >> 8);
	mac->uc[1] = (phy_data1 & 0x00ff);
	mac->uc[2] = (phy_data2 >> 8);
	mac->uc[3] = (phy_data2 & 0x00ff);
	mac->uc[4] = (phy_data3 >> 8);
	mac->uc[5] = (phy_data3 & 0x00ff);

	return SW_OK;
}

/******************************************************************************
*
* qca808x_phy_set wol enable or disable
*
* set phy wol enable or disable
*/
sw_error_t
qca808x_phy_set_wol_status(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
				       QCA808X_PHY_MMD3_WOL_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (enable == A_TRUE) {
		phy_data |= QCA808X_PHY_WOL_EN;
	} else {
		phy_data &= ~QCA808X_PHY_WOL_EN;
	}

	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
			     QCA808X_PHY_MMD3_WOL_CTRL, phy_data);

	return rv;
}

/******************************************************************************
*
* qca808x_phy_get_wol status
*
* get wol status
*/
sw_error_t
qca808x_phy_get_wol_status(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	a_uint16_t phy_data;

	*enable = A_FALSE;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
				       QCA808X_PHY_MMD3_WOL_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA808X_PHY_WOL_EN) {
		*enable = A_TRUE;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca808x_phy_set_hibernate - set hibernate status
*
* set hibernate status
*/
sw_error_t
qca808x_phy_set_hibernate(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_debug_read(dev_id, phy_id,
			QCA808X_DEBUG_PHY_HIBERNATION_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (enable == A_TRUE) {
		phy_data |= QCA808X_PHY_HIBERNATION_CFG;
	} else {
		phy_data &= ~QCA808X_PHY_HIBERNATION_CFG;
	}

	rv = qca808x_phy_debug_write(dev_id, phy_id,
			QCA808X_DEBUG_PHY_HIBERNATION_CTRL, phy_data);

	return rv;
}

/******************************************************************************
*
* qca808x_phy_get_hibernate - get hibernate status
*
* get hibernate status
*/
sw_error_t
qca808x_phy_get_hibernate(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_bool_t * enable)
{
	a_uint16_t phy_data;

	*enable = A_FALSE;

	phy_data = qca808x_phy_debug_read(dev_id, phy_id,
			QCA808X_DEBUG_PHY_HIBERNATION_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA808X_PHY_HIBERNATION_CFG) {
		*enable = A_TRUE;
	}

	return SW_OK;
}

/******************************************************************************
*
* _qca808x_phy_get_hibernate_status - get hibernate status
*
* get hibernate status
*/
sw_error_t
_qca808x_phy_get_hibernate_status(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_bool_t * enable)
{
	a_uint16_t phy_data;

	*enable = A_TRUE;

	phy_data = qca808x_phy_debug_read(dev_id, phy_id,
			QCA808X_DEBUG_PHY_HIBERNATION_STAT);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA808X_PHY_HIBERNATION_STAT_EN) {
		*enable = A_FALSE;
	}

	return SW_OK;
}
#endif
/******************************************************************************
*
* qca808x_phy_interface mode set
*
* set qca808x phy interface mode
*/
sw_error_t
qca808x_phy_interface_set_mode(a_uint32_t dev_id, a_uint32_t phy_id,
		fal_port_interface_mode_t interface_mode)
{
	/* qca808x phy will automatically switch the interface mode according
	 * to the speed, 2.5G works on SGMII+, other works on SGMII.
	 */

	return SW_OK;
}

/******************************************************************************
*
* qca808x_phy_interface mode get
*
* get qca808x phy interface mode
*/
sw_error_t
qca808x_phy_interface_get_mode(a_uint32_t dev_id, a_uint32_t phy_id,
		fal_port_interface_mode_t *interface_mode)
{
	a_uint16_t phy_data;
	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CHIP_CONFIG);
	PHY_RTN_ON_READ_ERROR(phy_data);

	phy_data &= QCA808X_PHY_CHIP_MODE_CFG;
	if (phy_data == QCA808X_PHY_SGMII_BASET) {
		*interface_mode = PHY_SGMII_BASET;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca808x_phy_interface mode status get
*
* get qca808x phy interface mode status
*/
sw_error_t
qca808x_phy_interface_get_mode_status(a_uint32_t dev_id, a_uint32_t phy_id,
		fal_port_interface_mode_t *interface_mode_status)
{
	a_uint16_t phy_data;
	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CHIP_CONFIG);
	PHY_RTN_ON_READ_ERROR(phy_data);

	phy_data &= QCA808X_PHY_MODE_MASK;
	switch (phy_data) {
		case QCA808X_PHY_SGMII_PLUS_MODE:
			*interface_mode_status = PORT_SGMII_PLUS;
			break;
		case QCA808X_PHY_SGMII_MODE:
			*interface_mode_status = PHY_SGMII_BASET;
			break;
		default:
			*interface_mode_status = PORT_INTERFACE_MODE_MAX;
			break;
	}

	return SW_OK;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* qca808x_phy_set_intr_mask - Set interrupt mask with the
* specified device.
*/
sw_error_t
qca808x_phy_set_intr_mask(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_uint32_t intr_mask_flag)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_INTR_MASK);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (intr_mask_flag & FAL_PHY_INTR_STATUS_UP_CHANGE) {
		phy_data |= QCA808X_INTR_STATUS_UP_CHANGE;
	} else {
		phy_data &= (~QCA808X_INTR_STATUS_UP_CHANGE);
	}

	if (intr_mask_flag & FAL_PHY_INTR_STATUS_DOWN_CHANGE) {
		phy_data |= QCA808X_INTR_STATUS_DOWN_CHANGE;
	} else {
		phy_data &= (~QCA808X_INTR_STATUS_DOWN_CHANGE);
	}

	if (intr_mask_flag & FAL_PHY_INTR_SPEED_CHANGE) {
		phy_data |= QCA808X_INTR_SPEED_CHANGE;
	} else {
		phy_data &= (~QCA808X_INTR_SPEED_CHANGE);
	}

	if (intr_mask_flag & FAL_PHY_INTR_BX_FX_STATUS_UP_CHANGE) {
		phy_data |= QCA808X_INTR_LINK_SUCCESS_SG;
	} else {
		phy_data &= (~QCA808X_INTR_LINK_SUCCESS_SG);
	}

	if (intr_mask_flag & FAL_PHY_INTR_BX_FX_STATUS_DOWN_CHANGE) {
		phy_data |= QCA808X_INTR_LINK_FAIL_SG;
	} else {
		phy_data &= (~QCA808X_INTR_LINK_FAIL_SG);
	}

	if (intr_mask_flag & FAL_PHY_INTR_WOL_STATUS) {
		phy_data |= QCA808X_INTR_WOL;
	} else {
		phy_data &= (~QCA808X_INTR_WOL);
	}

	if (intr_mask_flag & FAL_PHY_INTR_POE_STATUS) {
		phy_data |= QCA808X_INTR_POE;
	} else {
		phy_data &= (~QCA808X_INTR_POE);
	}

	rv = qca808x_phy_reg_write(dev_id, phy_id, QCA808X_PHY_INTR_MASK, phy_data);
	return rv;
}

/******************************************************************************
*
* qca808x_phy_get_intr_mask - Get interrupt mask with the
* specified device.
*/
sw_error_t
qca808x_phy_get_intr_mask(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_uint32_t * intr_mask_flag)
{
	a_uint16_t phy_data = 0;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_INTR_MASK);
	PHY_RTN_ON_READ_ERROR(phy_data);

	*intr_mask_flag = 0;
	if (phy_data & QCA808X_INTR_STATUS_UP_CHANGE) {
		*intr_mask_flag |= FAL_PHY_INTR_STATUS_UP_CHANGE;
	}

	if (phy_data & QCA808X_INTR_STATUS_DOWN_CHANGE) {
		*intr_mask_flag |= FAL_PHY_INTR_STATUS_DOWN_CHANGE;
	}

	if (phy_data & QCA808X_INTR_SPEED_CHANGE) {
		*intr_mask_flag |= FAL_PHY_INTR_SPEED_CHANGE;
	}

	if (phy_data & QCA808X_INTR_LINK_SUCCESS_SG) {
		*intr_mask_flag |= FAL_PHY_INTR_BX_FX_STATUS_UP_CHANGE;
	}

	if (phy_data & QCA808X_INTR_LINK_FAIL_SG) {
		*intr_mask_flag |= FAL_PHY_INTR_BX_FX_STATUS_DOWN_CHANGE;
	}

	if (phy_data & QCA808X_INTR_WOL) {
		*intr_mask_flag |= FAL_PHY_INTR_WOL_STATUS;
	}

	if (phy_data & QCA808X_INTR_POE) {
		*intr_mask_flag |= FAL_PHY_INTR_POE_STATUS;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca808x_phy_get_intr_status - Get interrupt status with the
* specified device.
*/
sw_error_t
qca808x_phy_get_intr_status(a_uint32_t dev_id, a_uint32_t phy_id,
			   a_uint32_t * intr_status_flag)
{
	a_uint16_t phy_data = 0;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_INTR_STATUS);
	PHY_RTN_ON_READ_ERROR(phy_data);

	*intr_status_flag = 0;
	if (phy_data & QCA808X_INTR_STATUS_UP_CHANGE) {
		*intr_status_flag |= FAL_PHY_INTR_STATUS_UP_CHANGE;
	}

	if (phy_data & QCA808X_INTR_STATUS_DOWN_CHANGE) {
		*intr_status_flag |= FAL_PHY_INTR_STATUS_DOWN_CHANGE;
	}

	if (phy_data & QCA808X_INTR_SPEED_CHANGE) {
		*intr_status_flag |= FAL_PHY_INTR_SPEED_CHANGE;
	}

	if (phy_data & QCA808X_INTR_LINK_SUCCESS_SG) {
		*intr_status_flag |= FAL_PHY_INTR_BX_FX_STATUS_UP_CHANGE;
	}

	if (phy_data & QCA808X_INTR_LINK_FAIL_SG) {
		*intr_status_flag |= FAL_PHY_INTR_BX_FX_STATUS_DOWN_CHANGE;
	}

	if (phy_data & QCA808X_INTR_WOL) {
		*intr_status_flag |= FAL_PHY_INTR_WOL_STATUS;
	}

	if (phy_data & QCA808X_INTR_POE) {
		*intr_status_flag |= FAL_PHY_INTR_POE_STATUS;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca808x_phy_set_counter - set counter status
*
* set counter  status
*/
sw_error_t
qca808x_phy_set_counter(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
					QCA808X_PHY_MMD7_COUNTER_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (enable == A_TRUE) {
		phy_data |= QCA808X_PHY_FRAME_CHECK_EN;
		phy_data |= QCA808X_PHY_XMIT_MAC_CNT_SELFCLR;
	} else {
		phy_data &= ~QCA808X_PHY_FRAME_CHECK_EN;
		phy_data &= ~QCA808X_PHY_XMIT_MAC_CNT_SELFCLR;
	}

	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
			     QCA808X_PHY_MMD7_COUNTER_CTRL, phy_data);

	return rv;
}

/******************************************************************************
*
* qca808x_phy_get_counter_status - get counter status
*
* set counter status
*/
sw_error_t
qca808x_phy_get_counter(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_bool_t * enable)
{
	a_uint16_t phy_data;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
					QCA808X_PHY_MMD7_COUNTER_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA808X_PHY_FRAME_CHECK_EN) {
		*enable = A_TRUE;
	} else {
		*enable = A_FALSE;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca808x_phy_show show counter statistics
*
* show counter statistics
*/
sw_error_t
qca808x_phy_show_counter(a_uint32_t dev_id, a_uint32_t phy_id,
			 fal_port_counter_info_t * counter_infor)
{
	a_uint16_t ingress_high_counter = 0;
	a_uint16_t ingress_low_counter = 0;
	a_uint16_t egress_high_counter = 0;
	a_uint16_t egress_low_counter = 0;

	ingress_high_counter = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
					QCA808X_PHY_MMD7_INGRESS_COUNTER_HIGH);
	ingress_low_counter = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
					QCA808X_PHY_MMD7_INGRESS_COUNTER_LOW);
	counter_infor->RxGoodFrame = (ingress_high_counter << 16 ) | ingress_low_counter;
	counter_infor->RxBadCRC = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
					QCA808X_PHY_MMD7_INGRESS_ERROR_COUNTER);

	egress_high_counter = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
					QCA808X_PHY_MMD7_EGRESS_COUNTER_HIGH);
	egress_low_counter = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
					QCA808X_PHY_MMD7_EGRESS_COUNTER_LOW);
	counter_infor->TxGoodFrame = (egress_high_counter << 16 ) | egress_low_counter;
	counter_infor->TxBadCRC = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
					QCA808X_PHY_MMD7_EGRESS_ERROR_COUNTER);

	return SW_OK;
}
#endif
/******************************************************************************
*
* qca808x_phy_set_eee_advertisement
*
* set eee advertisement
*/
sw_error_t
qca808x_phy_set_eee_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t adv)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
				       QCA808X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);
	phy_data &= ~(QCA808X_PHY_EEE_ADV_100M | QCA808X_PHY_EEE_ADV_1000M);

	if (adv & FAL_PHY_EEE_100BASE_T) {
		phy_data |= QCA808X_PHY_EEE_ADV_100M;
	}
	if (adv & FAL_PHY_EEE_1000BASE_T) {
		phy_data |= QCA808X_PHY_EEE_ADV_1000M;
	}

	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
		     QCA808X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL, phy_data);

	rv = qca808x_phy_restart_autoneg(dev_id, phy_id);

	return rv;
}

/******************************************************************************
*
* qca808x_phy_get_eee_advertisement
*
* get eee advertisement
*/
sw_error_t
qca808x_phy_get_eee_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *adv)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	*adv = 0;
	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
				       QCA808X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);

	if (phy_data & QCA808X_PHY_EEE_ADV_100M) {
		*adv |= FAL_PHY_EEE_100BASE_T;
	}
	if (phy_data & QCA808X_PHY_EEE_ADV_1000M) {
		*adv |= FAL_PHY_EEE_1000BASE_T;
	}

	return rv;
}
/******************************************************************************
*
* qca808x_phy_get_eee_partner_advertisement
*
* get eee partner advertisement
*/
sw_error_t
qca808x_phy_get_eee_partner_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *adv)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	*adv = 0;
	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
				       QCA808X_PHY_MMD7_ADDR_8023AZ_EEE_PARTNER);

	if (phy_data & QCA808X_PHY_EEE_PARTNER_ADV_100M) {
		*adv |= FAL_PHY_EEE_100BASE_T;
	}
	if (phy_data & QCA808X_PHY_EEE_PARTNER_ADV_1000M) {
		*adv |= FAL_PHY_EEE_1000BASE_T;
	}

	return rv;
}
/******************************************************************************
*
* qca808x_phy_get_eee_capability
*
* get eee capability
*/
sw_error_t
qca808x_phy_get_eee_cap(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *cap)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	*cap = 0;
	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
				       QCA808X_PHY_MMD3_ADDR_8023AZ_EEE_CAPABILITY);

	if (phy_data & QCA808X_PHY_EEE_CAPABILITY_100M) {
		*cap |= FAL_PHY_EEE_100BASE_T;
	}
	if (phy_data & QCA808X_PHY_EEE_CAPABILITY_1000M) {
		*cap |= FAL_PHY_EEE_1000BASE_T;
	}

	return rv;
}
/******************************************************************************
*
* qca808x_phy_get_eee_status
*
* get eee status
*/
sw_error_t
qca808x_phy_get_eee_status(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *status)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	*status = 0;
	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
				       QCA808X_PHY_MMD7_ADDR_8023AZ_EEE_STATUS);

	if (phy_data & QCA808X_PHY_EEE_STATUS_100M) {
		*status |= FAL_PHY_EEE_100BASE_T;
	}
	if (phy_data & QCA808X_PHY_EEE_STATUS_1000M) {
		*status |= FAL_PHY_EEE_1000BASE_T;
	}

	return rv;
}

/******************************************************************************
*
* qca808x_phy_led_init - set led behavior
*
* set led behavior
*/
static sw_error_t
qca808x_phy_led_init(a_uint32_t dev_id, a_uint32_t phy_id)
{
	sw_error_t rv = SW_OK;

	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
			QCA808X_PHY_MMD7_LED_POLARITY_CTRL,
				QCA808X_PHY_MMD7_LED_POLARITY_ACTIVE_HIGH);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
			QCA808X_PHY_MMD7_LED0_CTRL,
				QCA808X_PHY_MMD7_LED0_CTRL_ENABLE);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
			QCA808X_PHY_MMD7_LED1_CTRL,
				QCA808X_PHY_MMD7_LED1_CTRL_DISABLE);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
			QCA808X_PHY_MMD7_LED2_CTRL,
				QCA808X_PHY_MMD7_LED2_CTRL_DISABLE);
	SW_RTN_ON_ERROR(rv);

	return rv;
}

static sw_error_t
qca808x_phy_fast_retrain_cfg(a_uint32_t dev_id, a_uint32_t phy_id)
{
	sw_error_t rv = SW_OK;

	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
		QCA808X_PHY_MMD7_AUTONEGOTIATION_CONTROL,
		QCA808X_ADVERTISE_2500FULL |
		QCA808X_PHY_FAST_RETRAIN_2500BT |
		QCA808X_PHY_ADV_LOOP_TIMING);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD1_NUM,
		QCA808X_PHY_MMD1_FAST_RETRAIN_STATUS_CTL,
		QCA808X_PHY_FAST_RETRAIN_CTRL);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD1_NUM,
		QCA808X_PHY_MMD1_MSE_THRESHOLD_20DB,
		QCA808X_PHY_MSE_THRESHOLD_20DB_VALUE);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD1_NUM,
		QCA808X_PHY_MMD1_MSE_THRESHOLD_17DB,
		QCA808X_PHY_MSE_THRESHOLD_17DB_VALUE);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD1_NUM,
		QCA808X_PHY_MMD1_MSE_THRESHOLD_27DB,
		QCA808X_PHY_MSE_THRESHOLD_27DB_VALUE);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD1_NUM,
		QCA808X_PHY_MMD1_MSE_THRESHOLD_28DB,
		QCA808X_PHY_MSE_THRESHOLD_28DB_VALUE);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
		QCA808X_PHY_MMD7_ADDR_EEE_LP_ADVERTISEMENT,
		QCA808X_PHY_EEE_ADV_THP);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD7_NUM,
		QCA808X_PHY_MMD7_TOP_OPTION1,
		QCA808X_PHY_TOP_OPTION1_DATA);
	SW_RTN_ON_ERROR(rv);
	/*adjust the threshold for link down*/
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
		0xa100, 0x9203);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
		0xa105, 0x8001);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
		0xa106, 0x1111);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
		0xa103, 0x1698);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
		0xa011, 0x5f85);
	SW_RTN_ON_ERROR(rv);
	rv = qca808x_phy_mmd_write(dev_id, phy_id, QCA808X_PHY_MMD3_NUM,
		0xa101, 0x48ad);

	return rv;
}

void qca808x_phy_lock_init(void)
{
	static a_bool_t is_init = A_FALSE;

	if(!is_init)
	{
		QCA808X_LOCKER_INIT;
		is_init = A_TRUE;
	}

	return;
}

static sw_error_t
qca808x_phy_adc_threshold_set(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t adc_thresold)
{
	sw_error_t rv = SW_OK;
	a_uint16_t phy_data = 0;

	phy_data = qca808x_phy_debug_read(dev_id, phy_id,
		QCA808X_PHY_ADC_THRESHOLD);
	PHY_RTN_ON_READ_ERROR(phy_data);
	phy_data &= ~(BITS(0, 8));
	rv = qca808x_phy_debug_write (dev_id, phy_id,
		QCA808X_PHY_ADC_THRESHOLD, phy_data | adc_thresold);

	return rv;
}

static sw_error_t
qca808x_phy_hw_init(a_uint32_t dev_id,  a_uint32_t port_bmp)
{
	a_uint16_t phy_data = 0;
	a_uint32_t port_id = 0, phy_addr = 0;
	sw_error_t rv = SW_OK;

	for (port_id = SSDK_PHYSICAL_PORT0; port_id < SW_MAX_NR_PORT; port_id ++)
	{
		if (port_bmp & (0x1 << port_id))
		{
			phy_addr = qca_ssdk_port_to_phy_addr(dev_id, port_id);
			/*enable vga when init napa to fix 8023az issue*/
			phy_data = qca808x_phy_mmd_read(dev_id, phy_addr, QCA808X_PHY_MMD3_NUM,
				QCA808X_PHY_MMD3_ADDR_CLD_CTRL7);
			phy_data &= (~QCA808X_PHY_8023AZ_AFE_CTRL_MASK);
			phy_data |= QCA808X_PHY_8023AZ_AFE_EN;
			rv = qca808x_phy_mmd_write(dev_id, phy_addr, QCA808X_PHY_MMD3_NUM,
				QCA808X_PHY_MMD3_ADDR_CLD_CTRL7, phy_data);
			SW_RTN_ON_ERROR(rv);
			/*set napa led pin behavior on HK board*/
			rv = qca808x_phy_led_init(dev_id, phy_addr);
			SW_RTN_ON_ERROR(rv);
			/*special configuration for AZ under 1G speed mode*/
			phy_data = QCA808X_PHY_MMD3_AZ_TRAINING_VAL;
			rv = qca808x_phy_mmd_write(dev_id, phy_addr, QCA808X_PHY_MMD3_NUM,
				QCA808X_PHY_MMD3_AZ_TRAINING_CTRL, phy_data);
			SW_RTN_ON_ERROR(rv);
			/*config the fast retrain*/
			rv = qca808x_phy_fast_retrain_cfg(dev_id, phy_addr);
			SW_RTN_ON_ERROR(rv);
			/*enable seed and configure ramdom seed in order that napa can be
				as slave easier*/
			rv = qca808x_phy_ms_seed_enable(dev_id, phy_addr, A_TRUE);
			SW_RTN_ON_ERROR(rv);
			rv = qca808x_phy_ms_random_seed_set(dev_id, phy_addr);
			SW_RTN_ON_ERROR(rv);
			/*set adc threshold as 100mv for 10M*/
			rv = qca808x_phy_adc_threshold_set(dev_id, phy_addr,
				QCA808X_PHY_ADC_THRESHOLD_100MV);
			SW_RTN_ON_ERROR(rv);
		}
	}

	return rv;
}

static sw_error_t qca808x_phy_api_ops_init(void)
{
	sw_error_t  ret = SW_OK;
	hsl_phy_ops_t *qca808x_phy_api_ops = NULL;

	qca808x_phy_api_ops = kzalloc(sizeof(hsl_phy_ops_t), GFP_KERNEL);
	if (qca808x_phy_api_ops == NULL) {
		SSDK_ERROR("qca808x phy ops kzalloc failed!\n");
		return -ENOMEM;
	}

	phy_api_ops_init(QCA808X_PHY_CHIP);

	qca808x_phy_api_ops->phy_reg_write = qca808x_phy_reg_write;
	qca808x_phy_api_ops->phy_reg_read = qca808x_phy_reg_read;
	qca808x_phy_api_ops->phy_debug_write = qca808x_phy_debug_write;
	qca808x_phy_api_ops->phy_debug_read = qca808x_phy_debug_read;
	qca808x_phy_api_ops->phy_mmd_write = qca808x_phy_mmd_write;
	qca808x_phy_api_ops->phy_mmd_read = qca808x_phy_mmd_read;
	qca808x_phy_api_ops->phy_get_status = qca808x_phy_get_status;
	qca808x_phy_api_ops->phy_speed_get = qca808x_phy_get_speed;
	qca808x_phy_api_ops->phy_speed_set = qca808x_phy_set_speed;
	qca808x_phy_api_ops->phy_duplex_get = qca808x_phy_get_duplex;
	qca808x_phy_api_ops->phy_duplex_set = qca808x_phy_set_duplex;
	qca808x_phy_api_ops->phy_autoneg_enable_set = qca808x_phy_enable_autoneg;
	qca808x_phy_api_ops->phy_restart_autoneg = qca808x_phy_restart_autoneg;
	qca808x_phy_api_ops->phy_autoneg_status_get = qca808x_phy_autoneg_status;
	qca808x_phy_api_ops->phy_autoneg_adv_set = qca808x_phy_set_autoneg_adv;
	qca808x_phy_api_ops->phy_autoneg_adv_get = qca808x_phy_get_autoneg_adv;
	qca808x_phy_api_ops->phy_link_status_get = qca808x_phy_get_link_status;
	qca808x_phy_api_ops->phy_reset = qca808x_phy_reset;
#ifndef IN_PORTCONTROL_MINI
	qca808x_phy_api_ops->phy_cdt = qca808x_phy_cdt;
	qca808x_phy_api_ops->phy_mdix_set = qca808x_phy_set_mdix;
	qca808x_phy_api_ops->phy_mdix_get = qca808x_phy_get_mdix;
	qca808x_phy_api_ops->phy_mdix_status_get = qca808x_phy_get_mdix_status;
	qca808x_phy_api_ops->phy_local_loopback_set = qca808x_phy_set_local_loopback;
	qca808x_phy_api_ops->phy_local_loopback_get = qca808x_phy_get_local_loopback;
	qca808x_phy_api_ops->phy_remote_loopback_set = qca808x_phy_set_remote_loopback;
	qca808x_phy_api_ops->phy_remote_loopback_get = qca808x_phy_get_remote_loopback;
#endif
	qca808x_phy_api_ops->phy_id_get = qca808x_phy_get_phy_id;
	qca808x_phy_api_ops->phy_power_off = qca808x_phy_poweroff;
	qca808x_phy_api_ops->phy_power_on = qca808x_phy_poweron;
#ifndef IN_PORTCONTROL_MINI
	qca808x_phy_api_ops->phy_8023az_set = qca808x_phy_set_8023az;
	qca808x_phy_api_ops->phy_8023az_get = qca808x_phy_get_8023az;
	qca808x_phy_api_ops->phy_hibernation_set = qca808x_phy_set_hibernate;
	qca808x_phy_api_ops->phy_hibernation_get = qca808x_phy_get_hibernate;
	qca808x_phy_api_ops->phy_magic_frame_mac_set = qca808x_phy_set_magic_frame_mac;
	qca808x_phy_api_ops->phy_magic_frame_mac_get = qca808x_phy_get_magic_frame_mac;
	qca808x_phy_api_ops->phy_wol_status_set = qca808x_phy_set_wol_status;
	qca808x_phy_api_ops->phy_wol_status_get = qca808x_phy_get_wol_status;
#endif
	qca808x_phy_api_ops->phy_interface_mode_set = qca808x_phy_interface_set_mode;
	qca808x_phy_api_ops->phy_interface_mode_get = qca808x_phy_interface_get_mode;
	qca808x_phy_api_ops->phy_interface_mode_status_get = qca808x_phy_interface_get_mode_status;
#ifndef IN_PORTCONTROL_MINI
	qca808x_phy_api_ops->phy_intr_mask_set = qca808x_phy_set_intr_mask;
	qca808x_phy_api_ops->phy_intr_mask_get = qca808x_phy_get_intr_mask;
	qca808x_phy_api_ops->phy_intr_status_get = qca808x_phy_get_intr_status;
	qca808x_phy_api_ops->phy_counter_set = qca808x_phy_set_counter;
	qca808x_phy_api_ops->phy_counter_get = qca808x_phy_get_counter;
	qca808x_phy_api_ops->phy_counter_show = qca808x_phy_show_counter;
#endif
	qca808x_phy_api_ops->phy_eee_adv_set = qca808x_phy_set_eee_adv;
	qca808x_phy_api_ops->phy_eee_adv_get = qca808x_phy_get_eee_adv;
	qca808x_phy_api_ops->phy_eee_partner_adv_get = qca808x_phy_get_eee_partner_adv;
	qca808x_phy_api_ops->phy_eee_cap_get = qca808x_phy_get_eee_cap;
	qca808x_phy_api_ops->phy_eee_status_get = qca808x_phy_get_eee_status;
#ifdef IN_LED
	qca808x_phy_led_api_ops_init(qca808x_phy_api_ops);
#endif
/*qca808x_end*/
#if defined(IN_PTP)
	qca808x_phy_ptp_api_ops_init(&qca808x_phy_api_ops->phy_ptp_ops);
#endif
/*qca808x_start*/
	ret = hsl_phy_api_ops_register(QCA808X_PHY_CHIP, qca808x_phy_api_ops);

	if (ret == SW_OK) {
		SSDK_INFO("qca probe qca808x phy driver succeeded!\n");
	} else {
		SSDK_ERROR("qca probe qca808x phy driver failed! (code: %d)\n", ret);
	}

	return ret;
}

/******************************************************************************
*
* qca808x_phy_init -
*
*/
int qca808x_phy_init(a_uint32_t dev_id, a_uint32_t port_bmp)
{
/*qca808x_end*/
	a_uint32_t port_id = 0;
/*qca808x_start*/
	a_int32_t ret = 0;

	if(phy_ops_flag == A_FALSE &&
			qca808x_phy_api_ops_init() == SW_OK) {
		qca808x_phy_lock_init();
		phy_ops_flag = A_TRUE;
	}
	qca808x_phy_hw_init(dev_id, port_bmp);

/*qca808x_end*/
	if(phy_dev_drv_init_flag == A_FALSE)
	{
		for (port_id = 0; port_id < SW_MAX_NR_PORT; port_id ++)
		{
			if (port_bmp & (0x1 << port_id)) {
				qca808x_phydev_init(dev_id, port_id);
			}
		}
		ret = qca808x_phy_driver_register();
		phy_dev_drv_init_flag = A_TRUE;
	}
/*qca808x_start*/
	return ret;
}

void qca808x_phy_exit(a_uint32_t dev_id, a_uint32_t port_bmp)
{
/*qca808x_end*/
	a_uint32_t port_id = 0;

	qca808x_phy_driver_unregister();
	for (port_id = 0; port_id < SW_MAX_NR_PORT; port_id ++)
	{
		if (port_bmp & (0x1 << port_id)) {
			qca808x_phydev_deinit(dev_id, port_id);
		}
	}
/*qca808x_start*/
}
/*qca808x_end*/
