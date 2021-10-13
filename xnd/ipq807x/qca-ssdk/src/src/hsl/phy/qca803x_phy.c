/*
 * Copyright (c) 2017, 2019, The Linux Foundation. All rights reserved.
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
#include "sw.h"
#include "fal_port_ctrl.h"
#include "hsl_api.h"
#include "hsl.h"
#include "qca803x_phy.h"
#include "hsl_phy.h"
#include "ssdk_plat.h"

#define QCA803X_PHY_DELAYED_INIT_TICKS msecs_to_jiffies(1000)

typedef struct {
	a_uint32_t dev_id;
	a_uint32_t combo_phy_bmp;
	qca803x_phy_medium_t combo_cfg[SW_MAX_NR_PORT];
	struct delayed_work phy_sync_dwork;
} qca803x_priv_t;

static qca803x_priv_t g_qca803x_phy;
static struct mutex qca803x_reg_lock;

#define QCA803X_LOCKER_INIT		mutex_init(&qca803x_reg_lock)
#define QCA803X_REG_LOCK		mutex_lock(&qca803x_reg_lock)
#define QCA803X_REG_UNLOCK		mutex_unlock(&qca803x_reg_lock)


/******************************************************************************
*
* qca803x_phy_mii_read - mii register read
*
* mii register read
*/
a_uint16_t
qca803x_phy_reg_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id)
{
	sw_error_t rv = SW_OK;
	a_uint16_t phy_data = 0;

	HSL_PHY_GET(rv, dev_id, phy_id, reg_id, &phy_data);
	if (SW_OK != rv) {
		return 0xffff;
	}

	return phy_data;
}

/******************************************************************************
*
* qca803x_phy_mii_write - mii register write
*
* mii register write
*/
sw_error_t
qca803x_phy_reg_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id,
		       a_uint16_t reg_val)
{
	sw_error_t rv;

	HSL_PHY_SET(rv, dev_id, phy_id, reg_id, reg_val);

	return rv;

}

/******************************************************************************
*
* qca803x_phy_debug_write - debug port write
*
* debug port write
*/
sw_error_t
qca803x_phy_debug_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id,
		       a_uint16_t reg_val)
{
	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_DEBUG_PORT_ADDRESS, reg_id);
	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_DEBUG_PORT_DATA, reg_val);

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_debug_read - debug port read
*
* debug port read
*/
a_uint16_t
qca803x_phy_debug_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id)
{
	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_DEBUG_PORT_ADDRESS, reg_id);
	return qca803x_phy_reg_read(dev_id, phy_id, QCA803X_DEBUG_PORT_DATA);
}

/******************************************************************************
*
* qca803x_phy_mmd_write - PHY MMD register write
*
* PHY MMD register write
*/
sw_error_t
qca803x_phy_mmd_write(a_uint32_t dev_id, a_uint32_t phy_id,
		     a_uint16_t mmd_num, a_uint16_t reg_id, a_uint16_t reg_val)
{
	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_MMD_CTRL_REG, mmd_num);
	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_MMD_DATA_REG, reg_id);
	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_MMD_CTRL_REG,
			     0x4000 | mmd_num);
	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_MMD_DATA_REG, reg_val);

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_mmd_read -  PHY MMD register read
*
* PHY MMD register read
*/
a_uint16_t
qca803x_phy_mmd_read(a_uint32_t dev_id, a_uint32_t phy_id,
		    a_uint16_t mmd_num, a_uint16_t reg_id)
{
	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_MMD_CTRL_REG, mmd_num);
	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_MMD_DATA_REG, reg_id);
	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_MMD_CTRL_REG,
			     0x4000 | mmd_num);

	return qca803x_phy_reg_read(dev_id, phy_id, QCA803X_MMD_DATA_REG);
}

/******************************************************************************
*
* qca803x_phy_get_speed - Determines the speed of phy ports associated with the
* specified device.
*/

sw_error_t
qca803x_phy_get_speed(a_uint32_t dev_id, a_uint32_t phy_id,
		     fal_port_speed_t * speed)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_SPEC_STATUS);
	PHY_RTN_ON_READ_ERROR(phy_data);

	switch (phy_data & QCA803X_STATUS_SPEED_MASK)
	{
		case QCA803X_STATUS_SPEED_1000MBS:
			*speed = FAL_SPEED_1000;
			break;
		case QCA803X_STATUS_SPEED_100MBS:
			*speed = FAL_SPEED_100;
			break;
		case QCA803X_STATUS_SPEED_10MBS:
			*speed = FAL_SPEED_10;
			break;
		default:
			return SW_READ_ERROR;
	}

	return rv;
}

/******************************************************************************
*
* qca803x_phy_set_speed - Determines the speed of phy ports associated with the
* specified device.
*/
sw_error_t
qca803x_phy_set_speed(a_uint32_t dev_id, a_uint32_t phy_id,
		     fal_port_speed_t speed)
{
	a_uint16_t phy_data = 0;
	fal_port_duplex_t old_duplex;
	sw_error_t rv = SW_OK;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	rv = qca803x_phy_get_duplex(dev_id, phy_id, &old_duplex);
	if(rv != SW_OK) {
		return rv;
	}

	if (old_duplex == FAL_FULL_DUPLEX) {
		phy_data |= QCA803X_CTRL_FULL_DUPLEX;
		switch(speed)
		{
			case FAL_SPEED_1000:
				phy_data |= QCA803X_CTRL_SPEED_1000;
				phy_data &= ~QCA803X_CTRL_SPEED_100;
				phy_data |= QCA803X_CTRL_AUTONEGOTIATION_ENABLE;
				break;
			case FAL_SPEED_100:
			case FAL_SPEED_10:
				phy_data &= ~QCA803X_CTRL_SPEED_1000;
				phy_data &= ~QCA803X_CTRL_AUTONEGOTIATION_ENABLE;
				if (FAL_SPEED_100 == speed) {
					phy_data |= QCA803X_CTRL_SPEED_100;
				} else {
					phy_data &= ~QCA803X_CTRL_SPEED_100;
				}
				break;
			default:
				return SW_BAD_PARAM;
		}
	} else if (old_duplex == FAL_HALF_DUPLEX) {
		phy_data &= ~QCA803X_CTRL_FULL_DUPLEX;
		switch(speed)
		{
			case FAL_SPEED_100:
			case FAL_SPEED_10:
				phy_data &= ~QCA803X_CTRL_SPEED_1000;
				phy_data &= ~QCA803X_CTRL_AUTONEGOTIATION_ENABLE;
				if (FAL_SPEED_100 == speed) {
					phy_data |= QCA803X_CTRL_SPEED_100;
				} else {
					phy_data &= ~QCA803X_CTRL_SPEED_100;
				}
				break;
			case FAL_SPEED_1000:
				phy_data |= QCA803X_CTRL_FULL_DUPLEX;
				phy_data |= QCA803X_CTRL_SPEED_1000;
				phy_data &= ~QCA803X_CTRL_SPEED_100;
				phy_data |= QCA803X_CTRL_AUTONEGOTIATION_ENABLE;
				break;
			default:
				return SW_BAD_PARAM;
		}
	} else {
		return SW_FAIL;
	}
	rv = qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_CONTROL, phy_data);

	return rv;
}

/******************************************************************************
*
* qca803x_phy_set_duplex - Determines the speed of phy ports associated with the
* specified device.
*/
sw_error_t
qca803x_phy_set_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
		      fal_port_duplex_t duplex)
{
	a_uint16_t phy_data = 0;
	fal_port_speed_t old_speed;
	sw_error_t rv = SW_OK;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	qca803x_phy_get_speed(dev_id, phy_id, &old_speed);
	switch(old_speed)
	{
		case FAL_SPEED_1000:
			phy_data |= QCA803X_CTRL_SPEED_1000;
			phy_data &= ~QCA803X_CTRL_SPEED_100;
			phy_data |= QCA803X_CTRL_AUTONEGOTIATION_ENABLE;
			if (duplex == FAL_FULL_DUPLEX) {
				phy_data |= QCA803X_CTRL_FULL_DUPLEX;
			} else {
				return SW_NOT_SUPPORTED;
			}
			break;
		case FAL_SPEED_100:
		case FAL_SPEED_10:
			if(old_speed == FAL_SPEED_100) {
				phy_data |= QCA803X_CTRL_SPEED_100;
			} else {
				phy_data &= ~QCA803X_CTRL_SPEED_100;
			}
			phy_data &= ~QCA803X_CTRL_SPEED_1000;
			phy_data &= ~QCA803X_CTRL_AUTONEGOTIATION_ENABLE;
			if (duplex == FAL_FULL_DUPLEX) {
				phy_data |= QCA803X_CTRL_FULL_DUPLEX;
			} else {
				phy_data &= ~QCA803X_CTRL_FULL_DUPLEX;
			}
			break;
		default:
			return SW_FAIL;
	}
	rv = qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_CONTROL, phy_data);

	return rv;
}

/******************************************************************************
*
* qca803x_phy_get_duplex - Determines the speed of phy ports associated with the
* specified device.
*/
sw_error_t
qca803x_phy_get_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
		      fal_port_duplex_t * duplex)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_SPEC_STATUS);
	PHY_RTN_ON_READ_ERROR(phy_data);

	//read duplex
	if (phy_data & QCA803X_STATUS_FULL_DUPLEX)
		*duplex = FAL_FULL_DUPLEX;
	else
		*duplex = FAL_HALF_DUPLEX;

	return rv;
}

/******************************************************************************
*
* qca803x_phy_reset - reset the phy
*
* reset the phy
*/
sw_error_t qca803x_phy_reset(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	rv = qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_CONTROL,
			     phy_data | QCA803X_CTRL_SOFTWARE_RESET);

	return rv;
}
/******************************************************************************
*
* qca803x_phy_status - test to see if the specified phy link is alive
*
* RETURNS:
*    A_TRUE  --> link is alive
*    A_FALSE --> link is down
*/
a_bool_t qca803x_phy_get_link_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	a_bool_t rv = A_TRUE;
	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_SPEC_STATUS);

	if (phy_data & QCA803X_STATUS_LINK_PASS) {
		rv = A_TRUE;
	} else {
		rv = A_FALSE;
	}
	return rv;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* qca803x_phy_cdt - cable diagnostic test
*
* cable diagnostic test
*/

static inline fal_cable_status_t _phy_cdt_status_mapping(a_uint16_t status)
{
	fal_cable_status_t status_mapping = FAL_CABLE_STATUS_INVALID;

	switch (status) {
		case 3:
			status_mapping = FAL_CABLE_STATUS_INVALID;
			break;
		case 2:
			status_mapping = FAL_CABLE_STATUS_OPENED;
			break;
		case 1:
			status_mapping = FAL_CABLE_STATUS_SHORT;
			break;
		case 0:
			status_mapping = FAL_CABLE_STATUS_NORMAL;
			break;
	}
	return status_mapping;
}

static sw_error_t qca803x_phy_cdt_start(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t mdi_pair)
{
	a_uint16_t status = 0;
	a_uint16_t ii = 100;
	a_uint16_t MDI_PAIR_S = (mdi_pair << 8) & CDT_PAIR_MASK;

	/* RUN CDT */
	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_CDT_CONTROL,
			QCA803X_RUN_CDT | MDI_PAIR_S);
	do {
		aos_mdelay(30);
		status = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CDT_CONTROL);
	}
	while ((status & QCA803X_RUN_CDT) && (--ii));

	return SW_OK;
}

sw_error_t
qca803x_phy_cdt(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t mdi_pair,
	       fal_cable_status_t * cable_status, a_uint32_t * cable_len)
{
	a_uint16_t status = 0;

	if (mdi_pair >= QCA803X_MDI_PAIR_NUM) {
		//There are only 4 mdi pairs in 1000BASE-T
		return SW_BAD_PARAM;
	}

	qca803x_phy_cdt_start(dev_id, phy_id, mdi_pair);

	/* Get cable status */
	status = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CDT_STATUS);
	*cable_status = _phy_cdt_status_mapping((status >> 8) & 0x3);
	/* the actual cable length equals to CableDeltaTime * 0.824 */
	*cable_len = ((status & 0xff) * 824) / 1000;

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_set_mdix -
*
* set phy mdix configuraiton
*/
sw_error_t
qca803x_phy_set_mdix(a_uint32_t dev_id, a_uint32_t phy_id,
		    fal_port_mdix_mode_t mode)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_SPEC_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (mode == PHY_MDIX_AUTO) {
		phy_data |= QCA803X_PHY_MDIX_AUTO;
	} else if (mode == PHY_MDIX_MDIX) {
		phy_data &= ~QCA803X_PHY_MDIX_AUTO;
		phy_data |= QCA803X_PHY_MDIX;
	} else if (mode == PHY_MDIX_MDI) {
		phy_data &= ~QCA803X_PHY_MDIX_AUTO;
	} else {
		return SW_BAD_PARAM;
	}

	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_SPEC_CONTROL, phy_data);

	qca803x_phy_reset(dev_id, phy_id);
	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_get_mdix
*
* get phy mdix configuration
*/
sw_error_t
qca803x_phy_get_mdix(a_uint32_t dev_id, a_uint32_t phy_id,
		    fal_port_mdix_mode_t * mode)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_SPEC_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if ((phy_data & QCA803X_PHY_MDIX_AUTO) == QCA803X_PHY_MDIX_AUTO) {
		*mode = PHY_MDIX_AUTO;
	} else if ((phy_data & QCA803X_PHY_MDIX) == QCA803X_PHY_MDIX) {
		*mode = PHY_MDIX_MDIX;
	} else {
		*mode = PHY_MDIX_MDI;
	}

	return SW_OK;

}

/******************************************************************************
*
* qca803x_phy_get_mdix status
*
* get phy mdix status
*/
sw_error_t
qca803x_phy_get_mdix_status(a_uint32_t dev_id, a_uint32_t phy_id,
			   fal_port_mdix_status_t * mode)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_SPEC_STATUS);
	PHY_RTN_ON_READ_ERROR(phy_data);

	*mode =
	    (phy_data & QCA803X_PHY_MDIX_STATUS) ? PHY_MDIX_STATUS_MDIX :
	    PHY_MDIX_STATUS_MDI;

	return SW_OK;

}

/******************************************************************************
*
* qca803x_phy_set_local_loopback
*
* set phy local loopback
*/
sw_error_t
qca803x_phy_set_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			      a_bool_t enable)
{
	a_uint16_t phy_data;
	fal_port_speed_t old_speed;

	if (enable == A_TRUE) {
		qca803x_phy_get_speed(dev_id, phy_id, &old_speed);
		if (old_speed == FAL_SPEED_1000) {
			phy_data = QCA803X_1000M_LOOPBACK;
		} else if (old_speed == FAL_SPEED_100) {
			phy_data = QCA803X_100M_LOOPBACK;
		} else if (old_speed == FAL_SPEED_10) {
			phy_data = QCA803X_10M_LOOPBACK;
		} else {
			return SW_FAIL;
		}
	} else {
		phy_data = QCA803X_COMMON_CTRL;
	}

	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_CONTROL, phy_data);
	return SW_OK;

}

/******************************************************************************
*
* qca803x_phy_get_local_loopback
*
* get phy local loopback
*/
sw_error_t
qca803x_phy_get_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			      a_bool_t * enable)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA803X_LOCAL_LOOPBACK_ENABLE) {
		*enable = A_TRUE;
	} else {
		*enable = A_FALSE;
	}

	return SW_OK;

}

/******************************************************************************
*
* qca803x_phy_set_remote_loopback
*
* set phy remote loopback
*/
sw_error_t
qca803x_phy_set_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			       a_bool_t enable)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD3_NUM,
					QCA803X_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (enable == A_TRUE) {
		phy_data |= 0x0001;
	} else {
		phy_data &= ~0x0001;
	}

	qca803x_phy_mmd_write(dev_id, phy_id, QCA803X_PHY_MMD3_NUM,
			     QCA803X_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL,
			     phy_data);
	return SW_OK;

}

/******************************************************************************
*
* qca803x_phy_get_remote_loopback
*
* get phy remote loopback
*/
sw_error_t
qca803x_phy_get_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			       a_bool_t * enable)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD3_NUM,
					QCA803X_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & 0x0001) {
		*enable = A_TRUE;
	} else {
		*enable = A_FALSE;
	}

	return SW_OK;

}
#endif
/******************************************************************************
*
* qca803x_set_autoneg_adv - set the phy autoneg Advertisement
*
*/
sw_error_t
qca803x_phy_set_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
			   a_uint32_t autoneg)
{
	a_uint16_t phy_data = 0;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id,
			QCA803X_AUTONEG_ADVERT);
	PHY_RTN_ON_READ_ERROR(phy_data);

	phy_data &= ~QCA803X_ADVERTISE_MEGA_ALL;

	if (autoneg & FAL_PHY_ADV_100TX_FD) {
		phy_data |= QCA803X_ADVERTISE_100FULL;
	}
	if (autoneg & FAL_PHY_ADV_100TX_HD) {
		phy_data |= QCA803X_ADVERTISE_100HALF;
	}
	if (autoneg & FAL_PHY_ADV_10T_FD) {
		phy_data |= QCA803X_ADVERTISE_10FULL;
	}
	if (autoneg & FAL_PHY_ADV_10T_HD) {
		phy_data |= QCA803X_ADVERTISE_10HALF;
	}
	if (autoneg & FAL_PHY_ADV_PAUSE) {
		phy_data |= QCA803X_ADVERTISE_PAUSE;
	}
	if (autoneg & FAL_PHY_ADV_ASY_PAUSE) {
		phy_data |= QCA803X_ADVERTISE_ASYM_PAUSE;
	}
	if (autoneg & FAL_PHY_ADV_1000T_FD) {
		phy_data |= QCA803X_EXTENDED_NEXT_PAGE_EN;
	} else {
		phy_data &= ~QCA803X_EXTENDED_NEXT_PAGE_EN;
	}
	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_AUTONEG_ADVERT,
			phy_data);

	phy_data = qca803x_phy_reg_read(dev_id, phy_id,
				QCA803X_1000BASET_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	phy_data &= ~QCA803X_ADVERTISE_1000FULL;
	phy_data &= ~QCA803X_ADVERTISE_1000HALF;

	if (autoneg & FAL_PHY_ADV_1000T_FD) {
		phy_data |= QCA803X_ADVERTISE_1000FULL;
	}
	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_1000BASET_CONTROL,
			phy_data);

	return SW_OK;
}

/******************************************************************************
*
* qca803x_get_autoneg_adv - get the phy autoneg Advertisement
*
*/
sw_error_t
qca803x_phy_get_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
			   a_uint32_t * autoneg)
{
	a_uint16_t phy_data = 0;

	*autoneg = 0;
	phy_data =
		qca803x_phy_reg_read(dev_id, phy_id, QCA803X_AUTONEG_ADVERT);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA803X_ADVERTISE_100FULL)
		*autoneg |= FAL_PHY_ADV_100TX_FD;

	if (phy_data & QCA803X_ADVERTISE_100HALF)
		*autoneg |= FAL_PHY_ADV_100TX_HD;

	if (phy_data & QCA803X_ADVERTISE_10FULL)
		*autoneg |= FAL_PHY_ADV_10T_FD;

	if (phy_data & QCA803X_ADVERTISE_10HALF)
		*autoneg |= FAL_PHY_ADV_10T_HD;

	if (phy_data & QCA803X_ADVERTISE_PAUSE)
		*autoneg |= FAL_PHY_ADV_PAUSE;

	if (phy_data & QCA803X_ADVERTISE_ASYM_PAUSE)
		*autoneg |= FAL_PHY_ADV_ASY_PAUSE;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id,
				QCA803X_1000BASET_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA803X_ADVERTISE_1000FULL)
		*autoneg |= FAL_PHY_ADV_1000T_FD;

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_autoneg_status
*
* Power off the phy
*/
a_bool_t qca803x_phy_autoneg_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CONTROL);

	if (phy_data & QCA803X_CTRL_AUTONEGOTIATION_ENABLE)
		return A_TRUE;

	return A_FALSE;
}

/******************************************************************************
*
* qca803x_restart_autoneg - restart the phy autoneg
*
*/
sw_error_t qca803x_phy_restart_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	phy_data |= QCA803X_CTRL_AUTONEGOTIATION_ENABLE;
	rv = qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_CONTROL,
			     phy_data | QCA803X_CTRL_RESTART_AUTONEGOTIATION);

	return rv;
}
/******************************************************************************
*
* qca803x_phy_enable_autonego
*
*/
sw_error_t qca803x_phy_enable_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	rv = qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_CONTROL,
			     phy_data | QCA803X_CTRL_AUTONEGOTIATION_ENABLE);

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* qca803x_phy_get_ability - get the phy ability
*
*
*/
sw_error_t
qca803x_phy_get_ability(a_uint32_t dev_id, a_uint32_t phy_id,
		       a_uint32_t * ability)
{
	a_uint16_t phy_data;

	*ability = 0;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_STATUS);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA803X_STATUS_AUTONEG_CAPS)
		*ability |= FAL_PHY_AUTONEG_CAPS;

	if (phy_data & QCA803X_STATUS_100T2_HD_CAPS)
		*ability |= FAL_PHY_100T2_HD_CAPS;

	if (phy_data & QCA803X_STATUS_100T2_FD_CAPS)
		*ability |= FAL_PHY_100T2_FD_CAPS;

	if (phy_data & QCA803X_STATUS_10T_HD_CAPS)
		*ability |= FAL_PHY_10T_HD_CAPS;

	if (phy_data & QCA803X_STATUS_10T_FD_CAPS)
		*ability |= FAL_PHY_10T_FD_CAPS;

	if (phy_data & QCA803X_STATUS_100X_HD_CAPS)
		*ability |= FAL_PHY_100X_HD_CAPS;

	if (phy_data & QCA803X_STATUS_100X_FD_CAPS)
		*ability |= FAL_PHY_100X_FD_CAPS;

	if (phy_data & QCA803X_STATUS_100T4_CAPS)
		*ability |= FAL_PHY_100T4_CAPS;

	if (phy_data & QCA803X_STATUS_EXTENDED_STATUS) {
		phy_data =
		    qca803x_phy_reg_read(dev_id, phy_id, QCA803X_EXTENDED_STATUS);
		PHY_RTN_ON_READ_ERROR(phy_data);

		if (phy_data & QCA803X_STATUS_1000T_FD_CAPS) {
			*ability |= FAL_PHY_1000T_FD_CAPS;
		}

		if (phy_data & QCA803X_STATUS_1000X_FD_CAPS) {
			*ability |= FAL_PHY_1000X_FD_CAPS;
		}
	}

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_get_partner_ability - get the phy ability
*
*
*/
sw_error_t
qca803x_phy_get_partner_ability(a_uint32_t dev_id, a_uint32_t phy_id,
			       a_uint32_t * ability)
{
	a_uint16_t phy_data;

	*ability = 0;

	phy_data =
	    qca803x_phy_reg_read(dev_id, phy_id, QCA803X_LINK_PARTNER_ABILITY);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA803X_LINK_10BASETX_HALF_DUPLEX)
		*ability |= FAL_PHY_PART_10T_HD;

	if (phy_data & QCA803X_LINK_10BASETX_FULL_DUPLEX)
		*ability |= FAL_PHY_PART_10T_FD;

	if (phy_data & QCA803X_LINK_100BASETX_HALF_DUPLEX)
		*ability |= FAL_PHY_PART_100TX_HD;

	if (phy_data & QCA803X_LINK_100BASETX_FULL_DUPLEX)
		*ability |= FAL_PHY_PART_100TX_FD;

	if (phy_data & QCA803X_LINK_NPAGE) {
		phy_data =
		    qca803x_phy_reg_read(dev_id, phy_id,
					QCA803X_1000BASET_STATUS);
		PHY_RTN_ON_READ_ERROR(phy_data);

		if (phy_data & QCA803X_LINK_1000BASETX_FULL_DUPLEX)
			*ability |= FAL_PHY_PART_1000T_FD;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_reset_done - reset the phy
*
* reset the phy
*/
a_bool_t qca803x_phy_reset_done(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	a_uint16_t ii = 200;

	do {
		phy_data =
		    qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CONTROL);
		aos_mdelay(10);
	}
	while ((!QCA803X_RESET_DONE(phy_data)) && --ii);

	if (ii == 0)
		return A_FALSE;

	return A_TRUE;
}

/******************************************************************************
*
* qca803x_autoneg_done
*
* qca803x_autoneg_done
*/
a_bool_t qca803x_autoneg_done(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	a_uint16_t ii = 200;

	do {
		phy_data =
		    qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_STATUS);
		aos_mdelay(10);
	}
	while ((!QCA803X_AUTONEG_DONE(phy_data)) && --ii);

	if (ii == 0)
		return A_FALSE;

	return A_TRUE;
}

/******************************************************************************
*
* qca803x_phy_Speed_Duplex_Resolved
 - reset the phy
*
* reset the phy
*/
a_bool_t qca803x_phy_speed_duplex_resolved(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	a_uint16_t ii = 200;

	do {
		phy_data =
		    qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_SPEC_STATUS);
		aos_mdelay(10);
	}
	while ((!QCA803X_SPEED_DUPLEX_RESOVLED(phy_data)) && --ii);

	if (ii == 0)
		return A_FALSE;

	return A_TRUE;
}
#endif
/******************************************************************************
*
* qca803x_phy_get_phy_id - get the phy id
*
*/
sw_error_t
qca803x_phy_get_phy_id(a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *phy_data)
{
	a_uint16_t org_id, rev_id;

	org_id = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_ID1);
	rev_id = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_ID2);

	*phy_data = ((org_id & 0xffff) << 16) | (rev_id & 0xffff);

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_off - power off the phy
*
* Power off the phy
*/
sw_error_t qca803x_phy_poweroff(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_CONTROL,
				     phy_data | QCA803X_CTRL_POWER_DOWN);
	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_on - power on the phy
*
* Power on the phy
*/
sw_error_t qca803x_phy_poweron(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_CONTROL,
			phy_data & ~QCA803X_CTRL_POWER_DOWN);

	aos_mdelay(200);

	return SW_OK;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* qca803x_phy_set_802.3az
*
* set 802.3az status
*/
sw_error_t
qca803x_phy_set_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD7_NUM,
				       QCA803X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (enable == A_TRUE) {
		phy_data |= 0x0006;

		qca803x_phy_mmd_write(dev_id, phy_id, QCA803X_PHY_MMD7_NUM,
			     QCA803X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL, phy_data);
	} else {
		phy_data &= ~0x0006;

		qca803x_phy_mmd_write(dev_id, phy_id, QCA803X_PHY_MMD7_NUM,
			     QCA803X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL, phy_data);
	}

	qca803x_phy_restart_autoneg(dev_id, phy_id);

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_get_8023az status
*
* get 8023az status
*/
sw_error_t
qca803x_phy_get_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	a_uint16_t phy_data;
	*enable = A_FALSE;

	phy_data = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD7_NUM,
				       QCA803X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if ((phy_data & 0x0004) && (phy_data & 0x0002))
		*enable = A_TRUE;

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_set_powersave - set power saving status
*
* set power saving status
*/
sw_error_t
qca803x_phy_set_powersave(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_debug_read(dev_id, phy_id, QCA803X_PWR_SAVE);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (enable == A_TRUE)
		phy_data = phy_data | QCA803X_PWR_SAVE_EN;
	else
		phy_data = phy_data & ~QCA803X_PWR_SAVE_EN;

	qca803x_phy_debug_write(dev_id, phy_id, QCA803X_PWR_SAVE, phy_data);

	qca803x_phy_restart_autoneg(dev_id, phy_id);

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_get_powersave - get power saving status
*
* set power saving status
*/
sw_error_t
qca803x_phy_get_powersave(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_bool_t * enable)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_debug_read(dev_id, phy_id, QCA803X_PWR_SAVE);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & QCA803X_PWR_SAVE_EN)
		*enable = A_FALSE;
	else
		*enable = A_TRUE;

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_set wol frame mac address
*
* set phy wol frame mac address
*/
sw_error_t
qca803x_phy_set_magic_frame_mac(a_uint32_t dev_id, a_uint32_t phy_id,
			       fal_mac_addr_t * mac)
{
	a_uint16_t phy_data1;
	a_uint16_t phy_data2;
	a_uint16_t phy_data3;

	phy_data1 = (mac->uc[0] << 8) | mac->uc[1];
	phy_data2 = (mac->uc[2] << 8) | mac->uc[3];
	phy_data3 = (mac->uc[4] << 8) | mac->uc[5];

	qca803x_phy_mmd_write(dev_id, phy_id, QCA803X_PHY_MMD3_NUM,
			     QCA803X_PHY_MMD3_WOL_MAGIC_MAC_CTRL1, phy_data1);

	qca803x_phy_mmd_write(dev_id, phy_id, QCA803X_PHY_MMD3_NUM,
			     QCA803X_PHY_MMD3_WOL_MAGIC_MAC_CTRL2, phy_data2);

	qca803x_phy_mmd_write(dev_id, phy_id, QCA803X_PHY_MMD3_NUM,
			     QCA803X_PHY_MMD3_WOL_MAGIC_MAC_CTRL3, phy_data3);

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_get wol frame mac address
*
* get phy wol frame mac address
*/
sw_error_t
qca803x_phy_get_magic_frame_mac(a_uint32_t dev_id, a_uint32_t phy_id,
			       fal_mac_addr_t * mac)
{
	a_uint16_t phy_data1;
	a_uint16_t phy_data2;
	a_uint16_t phy_data3;

	phy_data1 = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD3_NUM,
					QCA803X_PHY_MMD3_WOL_MAGIC_MAC_CTRL1);

	phy_data2 = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD3_NUM,
					QCA803X_PHY_MMD3_WOL_MAGIC_MAC_CTRL2);

	phy_data3 = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD3_NUM,
					QCA803X_PHY_MMD3_WOL_MAGIC_MAC_CTRL3);

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
* qca803x_phy_set wol enable or disable
*
* set phy wol enable or disable
*/
sw_error_t
qca803x_phy_set_wol_status(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;
	phy_data = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD3_NUM,
				       QCA803X_PHY_MMD3_WOL_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (enable == A_TRUE) {
		phy_data |= 0x0020;
	} else {
		phy_data &= ~0x0020;
	}

	qca803x_phy_mmd_write(dev_id, phy_id, QCA803X_PHY_MMD3_NUM,
			     QCA803X_PHY_MMD3_WOL_CTRL, phy_data);

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_get_wol status
*
* get wol status
*/
sw_error_t
qca803x_phy_get_wol_status(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	a_uint16_t phy_data;

	*enable = A_FALSE;

	phy_data = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD3_NUM,
				       QCA803X_PHY_MMD3_WOL_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & 0x0020)
		*enable = A_TRUE;

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_set_hibernate - set hibernate status
*
* set hibernate status
*/
sw_error_t
qca803x_phy_set_hibernate(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_debug_read(dev_id, phy_id,
			QCA803X_DEBUG_PHY_HIBERNATION_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (enable == A_TRUE) {
		phy_data |= 0x8000;
	} else {
		phy_data &= ~0x8000;
	}

	qca803x_phy_debug_write(dev_id, phy_id,
			QCA803X_DEBUG_PHY_HIBERNATION_CTRL, phy_data);

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_get_hibernate - get hibernate status
*
* get hibernate status
*/
sw_error_t
qca803x_phy_get_hibernate(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_bool_t * enable)
{
	a_uint16_t phy_data;

	*enable = A_FALSE;

	phy_data = qca803x_phy_debug_read(dev_id, phy_id,
			QCA803X_DEBUG_PHY_HIBERNATION_CTRL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_data & 0x8000)
		*enable = A_TRUE;

	return SW_OK;
}
#endif
sw_error_t
__phy_chip_config_get(a_uint32_t dev_id, a_uint32_t phy_id,
		qca803x_cfg_type_t cfg_sel, qca803x_cfg_t *cfg_value)
{
	a_uint16_t phy_data;
	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CHIP_CONFIG);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (cfg_sel == QCA803X_CHIP_CFG_STAT)
		*cfg_value = (phy_data & QCA803X_PHY_CHIP_MODE_STAT) >> 4;
	else
		*cfg_value = phy_data & QCA803X_PHY_CHIP_MODE_CFG;

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_interface mode set
*
* set qca803x phy interface mode
*/
sw_error_t
qca803x_phy_interface_set_mode(a_uint32_t dev_id, a_uint32_t phy_id, fal_port_interface_mode_t interface_mode)
{
	a_uint16_t phy_data;

	QCA803X_REG_LOCK;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CHIP_CONFIG);

	phy_data &= 0xfff0;

	switch (interface_mode) {
		case PORT_RGMII_BASET:
			phy_data |= QCA803X_PHY_RGMII_BASET;
			break;
		case PHY_SGMII_BASET:
			phy_data |= QCA803X_PHY_SGMII_BASET;
			break;
		case PORT_RGMII_BX1000:
			phy_data |= QCA803X_PHY_BX1000_RGMII_50;
			break;
		case PORT_RGMII_FX100:
			phy_data |= QCA803X_PHY_FX100_RGMII_50;
			break;
		case PORT_RGMII_AMDET:
			phy_data |= QCA803X_PHY_RGMII_AMDET;
			break;
		default:
			QCA803X_REG_UNLOCK;
			return SW_BAD_PARAM;
	}

	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_CHIP_CONFIG, phy_data);

	QCA803X_REG_UNLOCK;

	/* reset operation */
	qca803x_phy_reset(dev_id, phy_id);

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_interface mode get
*
* get qca803x phy interface mode
*/
sw_error_t
qca803x_phy_interface_get_mode(a_uint32_t dev_id, a_uint32_t phy_id, fal_port_interface_mode_t *interface_mode)
{
	qca803x_cfg_t cfg_value;
	SW_RTN_ON_ERROR(__phy_chip_config_get(dev_id, phy_id,
				QCA803X_CHIP_CFG_SET, &cfg_value));

	switch (cfg_value) {
		case QCA803X_PHY_RGMII_BASET:
			*interface_mode = PORT_RGMII_BASET;
			break;
		case  QCA803X_PHY_SGMII_BASET:
			*interface_mode = PHY_SGMII_BASET;
			break;
		case QCA803X_PHY_BX1000_RGMII_50:
			*interface_mode = PORT_RGMII_BX1000;
			break;
		case QCA803X_PHY_FX100_RGMII_50:
			*interface_mode = PORT_RGMII_FX100;
			break;
		case QCA803X_PHY_RGMII_AMDET:
			*interface_mode = PORT_RGMII_AMDET;
			break;
		default:
			*interface_mode = PORT_INTERFACE_MODE_MAX;
			break;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_interface mode status get
*
* get qca803x phy interface mode status
*/
sw_error_t
qca803x_phy_interface_get_mode_status(a_uint32_t dev_id, a_uint32_t phy_id, fal_port_interface_mode_t *interface_mode_status)
{
	qca803x_cfg_t cfg_value;

	SW_RTN_ON_ERROR(__phy_chip_config_get(dev_id, phy_id,
				QCA803X_CHIP_CFG_STAT, &cfg_value));

	switch (cfg_value) {
		case QCA803X_PHY_RGMII_BASET:
			*interface_mode_status = PORT_RGMII_BASET;
			break;
		case QCA803X_PHY_SGMII_BASET:
			*interface_mode_status = PHY_SGMII_BASET;
			break;
		case QCA803X_PHY_BX1000_RGMII_50:
			*interface_mode_status = PORT_RGMII_BX1000;
			break;
		case QCA803X_PHY_FX100_RGMII_50:
			*interface_mode_status = PORT_RGMII_FX100;
			break;
		case QCA803X_PHY_RGMII_AMDET:
			*interface_mode_status = PORT_RGMII_AMDET;
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
* qca803x_phy_set_intr_mask - Set interrupt mask with the
* specified device.
*/
sw_error_t
qca803x_phy_set_intr_mask(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_uint32_t intr_mask_flag)
{
	a_uint16_t phy_data = 0;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_INTR_MASK);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (FAL_PHY_INTR_STATUS_UP_CHANGE & intr_mask_flag) {
		phy_data |= QCA803X_INTR_STATUS_UP_CHANGE;
	} else {
		phy_data &= (~QCA803X_INTR_STATUS_UP_CHANGE);
	}

	if (FAL_PHY_INTR_STATUS_DOWN_CHANGE & intr_mask_flag) {
		phy_data |= QCA803X_INTR_STATUS_DOWN_CHANGE;
	} else {
		phy_data &= (~QCA803X_INTR_STATUS_DOWN_CHANGE);
	}

	if (FAL_PHY_INTR_SPEED_CHANGE & intr_mask_flag) {
		phy_data |= QCA803X_INTR_SPEED_CHANGE;
	} else {
		phy_data &= (~QCA803X_INTR_SPEED_CHANGE);
	}

	/* DUPLEX INTR bit is reserved for AR803X phy
	if (FAL_PHY_INTR_DUPLEX_CHANGE & intr_mask_flag) {
		phy_data |= QCA803X_INTR_DUPLEX_CHANGE;
	} else {
		phy_data &= (~QCA803X_INTR_DUPLEX_CHANGE);
	}
	*/

	if (FAL_PHY_INTR_BX_FX_STATUS_UP_CHANGE & intr_mask_flag) {
		phy_data |= QCA803X_INTR_BX_FX_STATUS_UP_CHANGE;
	} else {
		phy_data &= (~QCA803X_INTR_BX_FX_STATUS_UP_CHANGE);
	}

	if (FAL_PHY_INTR_BX_FX_STATUS_DOWN_CHANGE & intr_mask_flag) {
		phy_data |= QCA803X_INTR_BX_FX_STATUS_DOWN_CHANGE;
	} else {
		phy_data &= (~QCA803X_INTR_BX_FX_STATUS_DOWN_CHANGE);
	}

	if (FAL_PHY_INTR_MEDIA_STATUS_CHANGE & intr_mask_flag) {
		phy_data |= QCA803X_INTR_MEDIA_STATUS_CHANGE;
	} else {
		phy_data &= (~QCA803X_INTR_MEDIA_STATUS_CHANGE);
	}

	if (FAL_PHY_INTR_WOL_STATUS & intr_mask_flag) {
		phy_data |= QCA803X_INTR_WOL;
	} else {
		phy_data &= (~QCA803X_INTR_WOL);
	}

	if (FAL_PHY_INTR_POE_STATUS & intr_mask_flag) {
		phy_data |= QCA803X_INTR_POE;
	} else {
		phy_data &= (~QCA803X_INTR_POE);
	}

	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_INTR_MASK, phy_data);
	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_get_intr_mask - Get interrupt mask with the
* specified device.
*/
sw_error_t
qca803x_phy_get_intr_mask(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_uint32_t * intr_mask_flag)
{
	a_uint16_t phy_data = 0;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_INTR_MASK);
	PHY_RTN_ON_READ_ERROR(phy_data);

	*intr_mask_flag = 0;
	if (QCA803X_INTR_STATUS_UP_CHANGE & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_STATUS_UP_CHANGE;
	}

	if (QCA803X_INTR_STATUS_DOWN_CHANGE & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_STATUS_DOWN_CHANGE;
	}

	if (QCA803X_INTR_SPEED_CHANGE & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_SPEED_CHANGE;
	}

	/* DUPLEX INTR bit is reserved for AR803X phy
	if (QCA803X_INTR_DUPLEX_CHANGE & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_DUPLEX_CHANGE;
	}
	*/

	if (QCA803X_INTR_BX_FX_STATUS_UP_CHANGE & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_BX_FX_STATUS_UP_CHANGE;
	}

	if (QCA803X_INTR_BX_FX_STATUS_DOWN_CHANGE & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_BX_FX_STATUS_DOWN_CHANGE;
	}

	if (QCA803X_INTR_MEDIA_STATUS_CHANGE  & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_MEDIA_STATUS_CHANGE;
	}

	if (QCA803X_INTR_WOL  & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_WOL_STATUS;
	}

	if (QCA803X_INTR_POE  & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_POE_STATUS;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_get_intr_status - Get interrupt status with the
* specified device.
*/
sw_error_t
qca803x_phy_get_intr_status(a_uint32_t dev_id, a_uint32_t phy_id,
			   a_uint32_t * intr_status_flag)
{
	a_uint16_t phy_data = 0;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_INTR_STATUS);
	PHY_RTN_ON_READ_ERROR(phy_data);

	*intr_status_flag = 0;
	if (QCA803X_INTR_STATUS_UP_CHANGE & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_STATUS_UP_CHANGE;
	}

	if (QCA803X_INTR_STATUS_DOWN_CHANGE & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_STATUS_DOWN_CHANGE;
	}

	if (QCA803X_INTR_SPEED_CHANGE & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_SPEED_CHANGE;
	}

	/* DUPLEX INTR bit is reserved for AR803X phy
	if (QCA803X_INTR_DUPLEX_CHANGE & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_DUPLEX_CHANGE;
	}
	*/

	if (QCA803X_INTR_BX_FX_STATUS_UP_CHANGE & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_BX_FX_STATUS_UP_CHANGE;
	}

	if (QCA803X_INTR_BX_FX_STATUS_DOWN_CHANGE & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_BX_FX_STATUS_DOWN_CHANGE;
	}
	if (QCA803X_INTR_MEDIA_STATUS_CHANGE  & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_MEDIA_STATUS_CHANGE;
	}

	if (QCA803X_INTR_WOL  & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_WOL_STATUS;
	}

	if (QCA803X_INTR_POE  & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_POE_STATUS;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_set combo medium type
*
* set combo medium fiber or copper
*/
sw_error_t
qca803x_phy_set_combo_prefer_medium(a_uint32_t dev_id, a_uint32_t phy_id,
				   fal_port_medium_t phy_medium)
{
	a_uint16_t phy_data;

	QCA803X_REG_LOCK;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CHIP_CONFIG);

	if (phy_medium == PHY_MEDIUM_FIBER)
		phy_data |= QCA803X_PHY_PREFER_FIBER;
	else if (phy_medium == PHY_MEDIUM_COPPER)
		phy_data &= ~QCA803X_PHY_PREFER_FIBER;
	else {
		QCA803X_REG_UNLOCK;
		return SW_BAD_PARAM;
	}

	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_CHIP_CONFIG, phy_data);

	QCA803X_REG_UNLOCK;

	/* soft reset after switching combo medium*/
	qca803x_phy_reset(dev_id, phy_id);

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_get combo medium type
*
* get combo medium fiber or copper
*/
sw_error_t
qca803x_phy_get_combo_prefer_medium(a_uint32_t dev_id, a_uint32_t phy_id,
				   fal_port_medium_t * phy_medium)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CHIP_CONFIG);
	PHY_RTN_ON_READ_ERROR(phy_data);

	*phy_medium =
	    (phy_data & QCA803X_PHY_PREFER_FIBER) ? PHY_MEDIUM_FIBER :
	    PHY_MEDIUM_COPPER;

	return SW_OK;
}

/******************************************************************************
*
*  qca803x phy activer medium
*
*  get qca803x phy current active medium, fiber or copper;
*/
static qca803x_phy_medium_t __phy_active_medium_get(a_uint32_t dev_id,
						   a_uint32_t phy_id)
{
	qca803x_cfg_t cfg_value;

	SW_RTN_ON_ERROR(__phy_chip_config_get(dev_id, phy_id,
				QCA803X_CHIP_CFG_STAT, &cfg_value));

	switch (cfg_value) {
		case QCA803X_PHY_RGMII_BASET:
		case QCA803X_PHY_SGMII_BASET:
			return QCA803X_PHY_MEDIUM_COPPER;
		case QCA803X_PHY_BX1000_RGMII_50:
		case QCA803X_PHY_FX100_RGMII_50:
			return QCA803X_PHY_MEDIUM_FIBER;
		case QCA803X_PHY_RGMII_AMDET:
		default:
			return QCA803X_PHY_MEDIUM_MAX;
	}
}

/******************************************************************************
*
* qca803x_phy_get current combo medium type copper or fiber
*
* get current combo medium type
*/
sw_error_t
qca803x_phy_get_combo_current_medium_type(a_uint32_t dev_id, a_uint32_t phy_id,
					 fal_port_medium_t * phy_medium)
{
	qca803x_phy_medium_t phy_cur_meduim = __phy_active_medium_get(dev_id, phy_id);

	/* auto media select is not done
	 * or link down, then return prefer medium */
	if (phy_cur_meduim == QCA803X_PHY_MEDIUM_MAX)
		qca803x_phy_get_combo_prefer_medium(dev_id, phy_id, phy_medium);
	else
		*phy_medium = phy_cur_meduim;

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_set fiber mode 1000bx or 100fx
*
* set combo fbier mode
*/
sw_error_t
qca803x_phy_set_combo_fiber_mode(a_uint32_t dev_id, a_uint32_t phy_id,
				fal_port_fiber_mode_t fiber_mode)
{
	a_uint16_t phy_data;

	QCA803X_REG_LOCK;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CHIP_CONFIG);

	if (fiber_mode == PHY_FIBER_1000BX) {
		phy_data |= QCA803X_PHY_FIBER_MODE_1000BX;
	} else if (fiber_mode == PHY_FIBER_100FX) {
		phy_data &= ~QCA803X_PHY_FIBER_MODE_1000BX;
	} else {
		QCA803X_REG_UNLOCK;
		return SW_BAD_PARAM;
	}

	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_CHIP_CONFIG, phy_data);

	QCA803X_REG_UNLOCK;

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_get fiber mode 1000bx or 100fx
*
* get combo fbier mode
*/
sw_error_t
qca803x_phy_get_combo_fiber_mode(a_uint32_t dev_id, a_uint32_t phy_id,
				fal_port_fiber_mode_t * fiber_mode)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CHIP_CONFIG);
	PHY_RTN_ON_READ_ERROR(phy_data);

	*fiber_mode =
	    (phy_data & QCA803X_PHY_FIBER_MODE_1000BX) ? PHY_FIBER_1000BX :
	    PHY_FIBER_100FX;

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_set_counter_status - set counter status
*
*/
sw_error_t
qca803x_phy_set_counter_status(a_uint32_t dev_id, a_uint32_t phy_id,
			a_bool_t enable)
{
	a_uint16_t phy_data;
	a_uint16_t frame_dir = 0;

	phy_data = qca803x_phy_mmd_read(dev_id, phy_id,
					QCA803X_PHY_MMD7_NUM,
					QCA803X_PHY_MMD7_FRAME_CTRL);
	frame_dir = (phy_data & QCA803X_PHY_MMD7_FRAME_DIR) >> 14;

	/*for qca803x phy, tx and rx cannot be all enabled one time,
	  so we will enable tx or rx based current state, enable rx if
	  current is tx and enable tx if current is rx*/
	if (enable == A_TRUE)
	{
		phy_data |= QCA803X_PHY_MMD7_FRAME_CHECK;
		/*enable RX counter*/
		if(frame_dir)
		{
			SSDK_INFO("ENABLE QCA803X RX COUNTER\n");
			phy_data &= ~QCA803X_PHY_MMD7_FRAME_DIR;
		}
		/*enable TX counter*/
		else
		{
			SSDK_INFO("ENABLE QCA803X TX COUNTER\n");
			phy_data |= QCA803X_PHY_MMD7_FRAME_DIR;
		}
	}
	else
	{
		phy_data &= ~QCA803X_PHY_MMD7_FRAME_CHECK;
	}

	qca803x_phy_mmd_write(dev_id, phy_id, QCA803X_PHY_MMD7_NUM,
			     QCA803X_PHY_MMD7_FRAME_CTRL, phy_data);

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_get_counter_status - get counter status
*
*/
sw_error_t
qca803x_phy_get_counter_status(a_uint32_t dev_id, a_uint32_t phy_id,
			a_bool_t * enable)
{
	a_uint16_t phy_data;
	*enable = A_FALSE;

	phy_data = qca803x_phy_mmd_read(dev_id, phy_id,
					QCA803X_PHY_MMD7_NUM,
					QCA803X_PHY_MMD7_FRAME_CTRL);

	if (phy_data & QCA803X_PHY_MMD7_FRAME_CHECK) {
		*enable = A_TRUE;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_show_counter - show counter statistics
*
*/
sw_error_t
qca803x_phy_show_counter(a_uint32_t dev_id, a_uint32_t phy_id,
			 fal_port_counter_info_t * counter_infor)
{
	a_uint16_t phy_data, phy_data1;
	a_uint16_t frame_dir = 0;

	phy_data = qca803x_phy_mmd_read(dev_id, phy_id,
					QCA803X_PHY_MMD7_NUM,
					QCA803X_PHY_MMD7_FRAME_CTRL);
	phy_data1 = qca803x_phy_mmd_read(dev_id, phy_id,
					QCA803X_PHY_MMD7_NUM,
					QCA803X_PHY_MMD7_FRAME_DATA);
	frame_dir = (phy_data & QCA803X_PHY_MMD7_FRAME_DIR) >> 14;

	if(frame_dir)
	{
		/*get the counter of tx*/
		counter_infor->TxGoodFrame = phy_data1 & QCA803X_PHY_FRAME_CNT;
		counter_infor->TxBadCRC = (phy_data1 & QCA803X_PHY_FRAME_ERROR) >> 8;
		counter_infor->RxGoodFrame = 0;
		counter_infor->RxBadCRC = 0;
	}
	else
	{
		/*get the counter of rx*/
		counter_infor->TxGoodFrame = 0;
		counter_infor->TxBadCRC = 0;
		counter_infor->RxGoodFrame = phy_data1 & QCA803X_PHY_FRAME_CNT;
		counter_infor->RxBadCRC = (phy_data1 & QCA803X_PHY_FRAME_ERROR) >> 8;
	}

	return SW_OK;
}

#endif
/******************************************************************************
*
* qca803x_phy_get status
*
* get phy status
*/
sw_error_t
qca803x_phy_get_status(a_uint32_t dev_id, a_uint32_t phy_id,
		struct port_phy_status *phy_status)
{
	a_uint16_t phy_data, phy_data1;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_SPEC_STATUS);
	PHY_RTN_ON_READ_ERROR(phy_data);
	phy_data1 = qca803x_phy_debug_read(dev_id, phy_id,
		QCA803X_DEBUG_MSE_THRESH);
	PHY_RTN_ON_READ_ERROR(phy_data1);

	/*get phy link status*/
	if (phy_data & QCA803X_STATUS_LINK_PASS) {
		phy_status->link_status = A_TRUE;
		if((phy_data1 & QCA803X_PHY_MSE_THRESH_MASK) !=
			QCA803X_PHY_MSE_THRESH_LINK_UP) {
			phy_data1 &= ~QCA803X_PHY_MSE_THRESH_MASK;
			SW_RTN_ON_ERROR(qca803x_phy_debug_write(dev_id,
				phy_id, QCA803X_DEBUG_MSE_THRESH,
				phy_data1 | QCA803X_PHY_MSE_THRESH_LINK_UP));
		}
	}
	else {
		phy_status->link_status = A_FALSE;
		if((phy_data1 & QCA803X_PHY_MSE_THRESH_MASK) !=
			QCA803X_PHY_MSE_THRESH_LINK_DOWN) {
			phy_data1 &= ~QCA803X_PHY_MSE_THRESH_MASK;
			SW_RTN_ON_ERROR(qca803x_phy_debug_write(dev_id,
				phy_id, QCA803X_DEBUG_MSE_THRESH,
				phy_data1 | QCA803X_PHY_MSE_THRESH_LINK_DOWN));
		}
		return SW_OK;
	}

	/*get phy speed*/
	switch (phy_data & QCA803X_STATUS_SPEED_MASK) {
	case QCA803X_STATUS_SPEED_1000MBS:
		phy_status->speed = FAL_SPEED_1000;
		break;
	case QCA803X_STATUS_SPEED_100MBS:
		phy_status->speed = FAL_SPEED_100;
		break;
	case QCA803X_STATUS_SPEED_10MBS:
		phy_status->speed = FAL_SPEED_10;
		break;
	default:
		return SW_READ_ERROR;
	}

	/*get phy duplex*/
	if (phy_data & QCA803X_STATUS_FULL_DUPLEX)
		phy_status->duplex = FAL_FULL_DUPLEX;
	else
		phy_status->duplex = FAL_HALF_DUPLEX;

	/* get phy flowctrl resolution status */
	if (phy_data & QCA803X_PHY_RX_FLOWCTRL_STATUS)
		phy_status->rx_flowctrl = A_TRUE;
	else
		phy_status->rx_flowctrl = A_FALSE;

	if (phy_data & QCA803X_PHY_TX_FLOWCTRL_STATUS)
		phy_status->tx_flowctrl = A_TRUE;
	else
		phy_status->tx_flowctrl = A_FALSE;

	return SW_OK;
}
/******************************************************************************
*
* qca803x_phy_set_eee_advertisement
*
* set eee advertisement
*/
sw_error_t
qca803x_phy_set_eee_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t adv)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	phy_data = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD7_NUM,
				       QCA803X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);
	phy_data &= ~(QCA803X_PHY_EEE_ADV_100M | QCA803X_PHY_EEE_ADV_1000M);

	if (adv & FAL_PHY_EEE_100BASE_T) {
		phy_data |= QCA803X_PHY_EEE_ADV_100M;
	}
	if (adv & FAL_PHY_EEE_1000BASE_T) {
		phy_data |= QCA803X_PHY_EEE_ADV_1000M;
	}

	rv = qca803x_phy_mmd_write(dev_id, phy_id, QCA803X_PHY_MMD7_NUM,
		     QCA803X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL, phy_data);

	rv = qca803x_phy_restart_autoneg(dev_id, phy_id);

	return rv;
}

/******************************************************************************
*
* qca803x_phy_get_eee_advertisement
*
* get eee advertisement
*/
sw_error_t
qca803x_phy_get_eee_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *adv)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	*adv = 0;
	phy_data = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD7_NUM,
				       QCA803X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);

	if (phy_data & QCA803X_PHY_EEE_ADV_100M) {
		*adv |= FAL_PHY_EEE_100BASE_T;
	}
	if (phy_data & QCA803X_PHY_EEE_ADV_1000M) {
		*adv |= FAL_PHY_EEE_1000BASE_T;
	}

	return rv;
}
/******************************************************************************
*
* qca803x_phy_get_eee_partner_advertisement
*
* get eee partner advertisement
*/
sw_error_t
qca803x_phy_get_eee_partner_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *adv)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	*adv = 0;
	phy_data = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD7_NUM,
				       QCA803X_PHY_MMD7_ADDR_8023AZ_EEE_PARTNER);

	if (phy_data & QCA803X_PHY_EEE_PARTNER_ADV_100M) {
		*adv |= FAL_PHY_EEE_100BASE_T;
	}
	if (phy_data & QCA803X_PHY_EEE_PARTNER_ADV_1000M) {
		*adv |= FAL_PHY_EEE_1000BASE_T;
	}

	return rv;
}
/******************************************************************************
*
* qca803x_phy_get_eee_capability
*
* get eee capability
*/
sw_error_t
qca803x_phy_get_eee_cap(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *cap)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	*cap = 0;
	phy_data = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD3_NUM,
				       QCA803X_PHY_MMD3_ADDR_8023AZ_EEE_CAPABILITY);

	if (phy_data & QCA803X_PHY_EEE_CAPABILITY_100M) {
		*cap |= FAL_PHY_EEE_100BASE_T;
	}
	if (phy_data & QCA803X_PHY_EEE_CAPABILITY_1000M) {
		*cap |= FAL_PHY_EEE_1000BASE_T;
	}

	return rv;
}
/******************************************************************************
*
* qca803x_phy_get_eee_status
*
* get eee status
*/
sw_error_t
qca803x_phy_get_eee_status(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *status)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	*status = 0;
	phy_data = qca803x_phy_mmd_read(dev_id, phy_id, QCA803X_PHY_MMD7_NUM,
				       QCA803X_PHY_MMD7_ADDR_8023AZ_EEE_STATUS);

	if (phy_data & QCA803X_PHY_EEE_STATUS_100M) {
		*status |= FAL_PHY_EEE_100BASE_T;
	}
	if (phy_data & QCA803X_PHY_EEE_STATUS_1000M) {
		*status |= FAL_PHY_EEE_1000BASE_T;
	}

	return rv;
}

static sw_error_t qca803x_phy_api_ops_init(void)
{
	sw_error_t  ret = SW_OK;
	hsl_phy_ops_t *qca803x_phy_api_ops = NULL;

	qca803x_phy_api_ops = kzalloc(sizeof(hsl_phy_ops_t), GFP_KERNEL);
	if (qca803x_phy_api_ops == NULL) {
		SSDK_ERROR("qca803x phy ops kzalloc failed!\n");
		return -ENOMEM;
	}

	phy_api_ops_init(QCA803X_PHY_CHIP);

	qca803x_phy_api_ops->phy_speed_get = qca803x_phy_get_speed;
	qca803x_phy_api_ops->phy_speed_set = qca803x_phy_set_speed;
	qca803x_phy_api_ops->phy_duplex_get = qca803x_phy_get_duplex;
	qca803x_phy_api_ops->phy_duplex_set = qca803x_phy_set_duplex;
	qca803x_phy_api_ops->phy_autoneg_enable_set = qca803x_phy_enable_autoneg;
	qca803x_phy_api_ops->phy_restart_autoneg = qca803x_phy_restart_autoneg;
	qca803x_phy_api_ops->phy_autoneg_status_get = qca803x_phy_autoneg_status;
	qca803x_phy_api_ops->phy_autoneg_adv_set = qca803x_phy_set_autoneg_adv;
	qca803x_phy_api_ops->phy_autoneg_adv_get = qca803x_phy_get_autoneg_adv;
	qca803x_phy_api_ops->phy_link_status_get = qca803x_phy_get_link_status;
	qca803x_phy_api_ops->phy_reset = qca803x_phy_reset;
#ifndef IN_PORTCONTROL_MINI
	qca803x_phy_api_ops->phy_powersave_set = qca803x_phy_set_powersave;
	qca803x_phy_api_ops->phy_powersave_get = qca803x_phy_get_powersave;
	qca803x_phy_api_ops->phy_cdt = qca803x_phy_cdt;
	qca803x_phy_api_ops->phy_mdix_set = qca803x_phy_set_mdix;
	qca803x_phy_api_ops->phy_mdix_get = qca803x_phy_get_mdix;
	qca803x_phy_api_ops->phy_mdix_status_get = qca803x_phy_get_mdix_status;
	qca803x_phy_api_ops->phy_local_loopback_set = qca803x_phy_set_local_loopback;
	qca803x_phy_api_ops->phy_local_loopback_get = qca803x_phy_get_local_loopback;
	qca803x_phy_api_ops->phy_remote_loopback_set = qca803x_phy_set_remote_loopback;
	qca803x_phy_api_ops->phy_remote_loopback_get = qca803x_phy_get_remote_loopback;
#endif
	qca803x_phy_api_ops->phy_reg_write = qca803x_phy_reg_write;
	qca803x_phy_api_ops->phy_reg_read = qca803x_phy_reg_read;
	qca803x_phy_api_ops->phy_debug_write = qca803x_phy_debug_write;
	qca803x_phy_api_ops->phy_debug_read = qca803x_phy_debug_read;
	qca803x_phy_api_ops->phy_mmd_write = qca803x_phy_mmd_write;
	qca803x_phy_api_ops->phy_mmd_read = qca803x_phy_mmd_read;
	qca803x_phy_api_ops->phy_id_get = qca803x_phy_get_phy_id;
	qca803x_phy_api_ops->phy_power_off = qca803x_phy_poweroff;
	qca803x_phy_api_ops->phy_power_on = qca803x_phy_poweron;
#ifndef IN_PORTCONTROL_MINI
	qca803x_phy_api_ops->phy_8023az_set = qca803x_phy_set_8023az;
	qca803x_phy_api_ops->phy_8023az_get = qca803x_phy_get_8023az;
	qca803x_phy_api_ops->phy_hibernation_set = qca803x_phy_set_hibernate;
	qca803x_phy_api_ops->phy_hibernation_get = qca803x_phy_get_hibernate;
	qca803x_phy_api_ops->phy_magic_frame_mac_set = qca803x_phy_set_magic_frame_mac;
	qca803x_phy_api_ops->phy_magic_frame_mac_get = qca803x_phy_get_magic_frame_mac;
	qca803x_phy_api_ops->phy_wol_status_set = qca803x_phy_set_wol_status;
	qca803x_phy_api_ops->phy_wol_status_get = qca803x_phy_get_wol_status;
#endif
	qca803x_phy_api_ops->phy_interface_mode_set = qca803x_phy_interface_set_mode;
	qca803x_phy_api_ops->phy_interface_mode_get = qca803x_phy_interface_get_mode;
	qca803x_phy_api_ops->phy_interface_mode_status_get = qca803x_phy_interface_get_mode_status;
#ifndef IN_PORTCONTROL_MINI
	qca803x_phy_api_ops->phy_intr_mask_set = qca803x_phy_set_intr_mask;
	qca803x_phy_api_ops->phy_intr_mask_get = qca803x_phy_get_intr_mask;
	qca803x_phy_api_ops->phy_intr_status_get = qca803x_phy_get_intr_status;
	qca803x_phy_api_ops->phy_combo_prefer_medium_set = qca803x_phy_set_combo_prefer_medium;
	qca803x_phy_api_ops->phy_combo_prefer_medium_get = qca803x_phy_get_combo_prefer_medium;
	qca803x_phy_api_ops->phy_combo_medium_status_get = qca803x_phy_get_combo_current_medium_type;
	qca803x_phy_api_ops->phy_combo_fiber_mode_set = qca803x_phy_set_combo_fiber_mode;
	qca803x_phy_api_ops->phy_combo_fiber_mode_get = qca803x_phy_get_combo_fiber_mode;
	qca803x_phy_api_ops->phy_counter_set = qca803x_phy_set_counter_status;
	qca803x_phy_api_ops->phy_counter_get = qca803x_phy_get_counter_status;
	qca803x_phy_api_ops->phy_counter_show = qca803x_phy_show_counter;
#endif
	qca803x_phy_api_ops->phy_get_status = qca803x_phy_get_status;
	qca803x_phy_api_ops->phy_eee_adv_set = qca803x_phy_set_eee_adv;
	qca803x_phy_api_ops->phy_eee_adv_get = qca803x_phy_get_eee_adv;
	qca803x_phy_api_ops->phy_eee_partner_adv_get = qca803x_phy_get_eee_partner_adv;
	qca803x_phy_api_ops->phy_eee_cap_get = qca803x_phy_get_eee_cap;
	qca803x_phy_api_ops->phy_eee_status_get = qca803x_phy_get_eee_status;

	ret = hsl_phy_api_ops_register(QCA803X_PHY_CHIP, qca803x_phy_api_ops);

	if (ret == SW_OK)
		SSDK_INFO("qca probe qca803x phy driver succeeded!\n");
	else
		SSDK_ERROR("qca probe qca803x phy driver failed! (code: %d)\n", ret);

	return ret;
}

static sw_error_t
_qca803x_phy_set_combo_page_regs(a_uint32_t dev_id, a_uint32_t phy_id,
				   qca803x_phy_medium_t phy_medium)
{
	a_uint16_t phy_data;

	phy_data = qca803x_phy_reg_read(dev_id, phy_id, QCA803X_PHY_CHIP_CONFIG);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (phy_medium == QCA803X_PHY_MEDIUM_FIBER) {
		phy_data &= ~QCA803X_PHY_COPPER_PAGE_SEL;
	}
	else if (phy_medium == QCA803X_PHY_MEDIUM_COPPER) {
		phy_data |= QCA803X_PHY_COPPER_PAGE_SEL;
	}
	else {
		return SW_BAD_PARAM;
	}

	qca803x_phy_reg_write(dev_id, phy_id, QCA803X_PHY_CHIP_CONFIG, phy_data);

	return SW_OK;
}

void qca803x_combo_phy_polling(qca803x_priv_t *priv)
{

	qca803x_cfg_t cfg_value;
	a_uint32_t combo_phy_addr = 0;
	a_uint32_t combo_bits = priv->combo_phy_bmp;
	qca803x_phy_medium_t combo_cfg_new = QCA803X_PHY_MEDIUM_COPPER;

	while (combo_bits) {
		if (combo_bits & 1) {
			QCA803X_REG_LOCK;
			__phy_chip_config_get(priv->dev_id, combo_phy_addr,
					QCA803X_CHIP_CFG_STAT, &cfg_value);

			switch (cfg_value) {
				case QCA803X_PHY_RGMII_BASET:
				case QCA803X_PHY_SGMII_BASET:
					combo_cfg_new = QCA803X_PHY_MEDIUM_COPPER;
					break;
				case QCA803X_PHY_BX1000_RGMII_50:
				case QCA803X_PHY_FX100_RGMII_50:
					combo_cfg_new = QCA803X_PHY_MEDIUM_FIBER;
					break;
				default:
					combo_cfg_new = QCA803X_PHY_MEDIUM_COPPER;
			}

			if (priv->combo_cfg[combo_phy_addr] != combo_cfg_new) {
				priv->combo_cfg[combo_phy_addr] = combo_cfg_new;
				_qca803x_phy_set_combo_page_regs(priv->dev_id, combo_phy_addr, combo_cfg_new);
			}

			QCA803X_REG_UNLOCK;
		}
		combo_bits >>= 1;
		combo_phy_addr++;
	}
}

void
qca803x_phy_polling_work(struct work_struct *work)
{
	qca803x_priv_t *priv = container_of(work, qca803x_priv_t,
					phy_sync_dwork.work);
	qca803x_combo_phy_polling(priv);

	schedule_delayed_work(&priv->phy_sync_dwork,
					QCA803X_PHY_DELAYED_INIT_TICKS);
}

sw_error_t
qca803x_phy_work_start(a_uint32_t dev_id)
{
	qca803x_priv_t *priv = &g_qca803x_phy;
	priv->dev_id = dev_id;

	INIT_DELAYED_WORK(&priv->phy_sync_dwork,
					qca803x_phy_polling_work);
	schedule_delayed_work(&priv->phy_sync_dwork,
					QCA803X_PHY_DELAYED_INIT_TICKS);
	return SW_OK;
}

/******************************************************************************
*
* qca803x_phy_hw_init
*
*/
sw_error_t
qca803x_phy_hw_init(a_uint32_t dev_id, a_uint32_t port_bmp)
{
	sw_error_t  ret = SW_OK;
	a_uint32_t port_id = 0, phy_addr = 0, mac_mode = 0;
	a_uint16_t phy_data= 0;

	for (port_id = 0; port_id < SW_MAX_NR_PORT; port_id ++)
	{
		if (port_bmp & (0x1 << port_id))
		{
			/*config phy mode based on the mac mode DT*/
			switch (port_id) {
				case SSDK_PHYSICAL_PORT0:
					mac_mode = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE0);
					break;
				case SSDK_PHYSICAL_PORT6:
					mac_mode = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE2);
					break;
				default:
					mac_mode = PORT_WRAPPER_MAX;
			}

			phy_addr = qca_ssdk_port_to_phy_addr(dev_id, port_id);
			if (mac_mode == PORT_WRAPPER_SGMII_CHANNEL0)
				qca803x_phy_interface_set_mode(dev_id, phy_addr, PHY_SGMII_BASET);
			else if (mac_mode == PORT_WRAPPER_RGMII)
				qca803x_phy_interface_set_mode(dev_id, phy_addr, PORT_RGMII_BASET);

			if (A_TRUE == hsl_port_phy_combo_capability_get(dev_id, port_id)) {
				g_qca803x_phy.combo_phy_bmp |= (0x1 << phy_addr);
				qca803x_phy_interface_set_mode(dev_id, phy_addr, PORT_RGMII_AMDET);
			}
			/*config the times that MSE is over threshold as max*/
			phy_data = qca803x_phy_debug_read(dev_id, phy_addr,
				QCA803X_DEBUG_MSE_OVER_THRESH_TIMES);
			PHY_RTN_ON_READ_ERROR(phy_data);
			ret = qca803x_phy_debug_write(dev_id, phy_addr,
				QCA803X_DEBUG_MSE_OVER_THRESH_TIMES, phy_data |
				QCA803X_PHY_MSE_OVER_THRESH_TIMES_MAX);
			SW_RTN_ON_ERROR(ret);
		}
	}

	/* start polling task for the combo port */
	if (g_qca803x_phy.combo_phy_bmp)
		qca803x_phy_work_start(dev_id);

	return ret;
}

/******************************************************************************
*
* qca803x_phy_init -
*
*/
int qca803x_phy_init(a_uint32_t dev_id, a_uint32_t port_bmp)
{
	static a_uint32_t phy_ops_flag = 0;

	if(phy_ops_flag == 0) {
		QCA803X_LOCKER_INIT;
		qca803x_phy_api_ops_init();
		phy_ops_flag = 1;
	}

	qca803x_phy_hw_init(dev_id, port_bmp);
	return 0;
}
