/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "sw.h"
#include "fal_port_ctrl.h"
#include "hsl_api.h"
#include "hsl.h"
#include "hsl_phy.h"
#include "ssdk_plat.h"
#include "qca808x_phy.h"
#include "mpge_phy.h"
#ifdef IN_LED
#include "mpge_led.h"
#endif

#define PHY_DAC(val) (val<<8)

/******************************************************************************
*
* mpge_phy_mii_read - mii register read
*
*/
static a_uint16_t
mpge_phy_reg_read (a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id)
{
	return qca808x_phy_reg_read (dev_id, phy_id, reg_id);
}

/******************************************************************************
*
* mpge_phy_mii_write - mii register write
*
*/
static sw_error_t
mpge_phy_reg_write (a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id,
	a_uint16_t reg_val)
{
	return qca808x_phy_reg_write (dev_id, phy_id, reg_id, reg_val);
}

/******************************************************************************
*
* mpge_phy_debug_read - debug port read
*
*/
static a_uint16_t
mpge_phy_debug_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id)
{
	return qca808x_phy_debug_read(dev_id, phy_id, reg_id);
}

/******************************************************************************
*
* mpge_phy_debug_write - debug port write
*
*/
static sw_error_t
mpge_phy_debug_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id,
	a_uint16_t reg_val)
{
	return qca808x_phy_debug_write (dev_id, phy_id, reg_id, reg_val);
}

/******************************************************************************
*
* mpge_phy_mmd_read -  PHY MMD register read
*
*/
static a_uint16_t
mpge_phy_mmd_read(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint16_t mmd_num, a_uint16_t reg_id)
{
	return qca808x_phy_mmd_read(dev_id, phy_id, mmd_num, reg_id);
}

/******************************************************************************
*
* mpge_phy_mmd_write - PHY MMD register write
*
*/
static sw_error_t
mpge_phy_mmd_write(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint16_t mmd_num, a_uint16_t reg_id, a_uint16_t reg_val)
{
	return qca808x_phy_mmd_write (dev_id, phy_id, mmd_num,
		reg_id, reg_val);
}

/******************************************************************************
*
* mpge_phy_get_status - get the phy status
*
*/
static sw_error_t
mpge_phy_get_status(a_uint32_t dev_id, a_uint32_t phy_id,
		struct port_phy_status *phy_status)
{
	return qca808x_phy_get_status(dev_id, phy_id, phy_status);
}

/******************************************************************************
*
* mpge_set_autoneg_adv - set the phy autoneg Advertisement
*
*/
static sw_error_t
mpge_phy_set_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
			   a_uint32_t autoneg)
{
	if (autoneg & (~FAL_PHY_GE_ADV_ALL))
	{
		SSDK_ERROR("autoneg adv caps 0x%x is not support for MP\n", autoneg);
		return SW_BAD_PARAM;
	}
	return qca808x_phy_set_autoneg_adv(dev_id, phy_id, autoneg);
}

/******************************************************************************
*
* mpge_get_autoneg_adv - get the phy autoneg Advertisement
*
*/
static sw_error_t
mpge_phy_get_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
			   a_uint32_t * autoneg)
{
	return qca808x_phy_get_autoneg_adv(dev_id, phy_id, autoneg);
}

/******************************************************************************
*
* mpge_phy_get_speed - Determines the speed of phy ports associated with the
* specified device.
*/

static sw_error_t
mpge_phy_get_speed(a_uint32_t dev_id, a_uint32_t phy_id,
		     fal_port_speed_t * speed)
{
	return qca808x_phy_get_speed(dev_id, phy_id, speed);
}

/******************************************************************************
*
* mpge_phy_get_duplex - Determines the duplex of phy ports associated with the
* specified device.
*/
static sw_error_t
mpge_phy_get_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
		      fal_port_duplex_t * duplex)
{
	return qca808x_phy_get_duplex(dev_id, phy_id, duplex);
}

/******************************************************************************
*
* mpge_phy_set_speed - Set the speed of phy ports associated with the
* specified device.
*/
static sw_error_t
mpge_phy_set_speed(a_uint32_t dev_id, a_uint32_t phy_id,
		     fal_port_speed_t speed)
{
	a_uint16_t phy_data = 0;
	a_uint32_t autoneg = 0;
	fal_port_duplex_t old_duplex = MPGE_CTRL_FULL_DUPLEX;
	sw_error_t rv = SW_OK;

	phy_data = mpge_phy_reg_read(dev_id, phy_id, MPGE_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	switch(speed)
	{
		case FAL_SPEED_1000:
			rv = mpge_phy_get_autoneg_adv(dev_id, phy_id, &autoneg);
			PHY_RTN_ON_ERROR(rv);
			if (!(autoneg & FAL_PHY_ADV_1000T_FD)) {
				rv = mpge_phy_set_autoneg_adv(dev_id, phy_id,
						autoneg | FAL_PHY_ADV_1000T_FD);
				PHY_RTN_ON_ERROR(rv);
			}
			phy_data |= MPGE_CTRL_FULL_DUPLEX;
			phy_data |= MPGE_CTRL_AUTONEGOTIATION_ENABLE;
			phy_data |= MPGE_CTRL_RESTART_AUTONEGOTIATION;
			break;
		case FAL_SPEED_100:
		case FAL_SPEED_10:
			phy_data &= ~MPGE_CONTROL_SPEED_MASK;
			if (speed == FAL_SPEED_100) {
				phy_data |= MPGE_CONTROL_100M;
			} else {
				phy_data |= MPGE_CONTROL_10M;
			}
			rv = mpge_phy_get_duplex(dev_id, phy_id, &old_duplex);
			PHY_RTN_ON_ERROR(rv);

			if (old_duplex == FAL_FULL_DUPLEX) {
				phy_data |= MPGE_CTRL_FULL_DUPLEX;
			}
			else if (old_duplex == FAL_HALF_DUPLEX) {
				phy_data &= ~MPGE_CTRL_FULL_DUPLEX;
			}
			phy_data &= ~MPGE_CTRL_AUTONEGOTIATION_ENABLE;
			break;
		default:
			return SW_BAD_PARAM;
	}
	rv = mpge_phy_reg_write(dev_id, phy_id, MPGE_PHY_CONTROL, phy_data);

	return rv;
}

/******************************************************************************
*
* mpge_phy_set_duplex - Set the duplex of phy ports associated with the
* specified device.
*/
static sw_error_t
mpge_phy_set_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
		      fal_port_duplex_t duplex)
{

	a_uint16_t phy_data = 0;
	a_uint32_t autoneg = 0;
	fal_port_speed_t old_speed;
	sw_error_t rv = SW_OK;

	phy_data = mpge_phy_reg_read(dev_id, phy_id, MPGE_PHY_CONTROL);
	PHY_RTN_ON_READ_ERROR(phy_data);

	rv = mpge_phy_get_speed(dev_id, phy_id, &old_speed);
	PHY_RTN_ON_ERROR(rv);

	switch(old_speed)
	{
		case FAL_SPEED_1000:
			if (duplex == FAL_FULL_DUPLEX) {
				phy_data |= MPGE_CTRL_FULL_DUPLEX;
			} else {
				return SW_NOT_SUPPORTED;
			}
			phy_data |= MPGE_CTRL_AUTONEGOTIATION_ENABLE;
			rv = mpge_phy_get_autoneg_adv(dev_id, phy_id, &autoneg);
			PHY_RTN_ON_ERROR(rv);
			if (!(autoneg & FAL_PHY_ADV_1000T_FD)) {
				rv = mpge_phy_set_autoneg_adv(dev_id, phy_id,
						autoneg | FAL_PHY_ADV_1000T_FD);
				PHY_RTN_ON_ERROR(rv);
			}
			break;
		case FAL_SPEED_100:
		case FAL_SPEED_10:
			phy_data &= ~MPGE_CONTROL_SPEED_MASK;
			if (old_speed == FAL_SPEED_100) {
				phy_data |= MPGE_CONTROL_100M;
			} else {
				phy_data |= MPGE_CONTROL_10M;
			}
			phy_data &= ~MPGE_CTRL_AUTONEGOTIATION_ENABLE;
			if (duplex == FAL_FULL_DUPLEX) {
				phy_data |= MPGE_CTRL_FULL_DUPLEX;
			} else {
				phy_data &= ~MPGE_CTRL_FULL_DUPLEX;
			}
			break;
		default:
			return SW_FAIL;
	}
	rv = mpge_phy_reg_write(dev_id, phy_id, MPGE_PHY_CONTROL, phy_data);

	return rv;
}

/******************************************************************************
*
* mpge_phy_enable_autoneg - enable the phy autoneg
*
*/
static sw_error_t
mpge_phy_enable_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
	return qca808x_phy_enable_autoneg(dev_id, phy_id);
}

/******************************************************************************
*
* mpge_restart_autoneg - restart the phy autoneg
*
*/
static sw_error_t
mpge_phy_restart_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
	return qca808x_phy_restart_autoneg(dev_id, phy_id);
}

/******************************************************************************
*
* mpge_phy_autoneg_status - get the phy autoneg status
*
*/
static a_bool_t
mpge_phy_autoneg_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
	return qca808x_phy_autoneg_status(dev_id, phy_id);
}

/******************************************************************************
*
* mpge_phy_status - get the phy link status
*
* RETURNS:
*    A_TRUE  --> link is alive
*    A_FALSE --> link is down
*/
static a_bool_t
mpge_phy_get_link_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
	return qca808x_phy_get_link_status(dev_id, phy_id);
}

/******************************************************************************
*
* mpge_phy_reset - reset the phy
*
*/
static sw_error_t
mpge_phy_reset(a_uint32_t dev_id, a_uint32_t phy_id)
{
	return qca808x_phy_reset(dev_id, phy_id);
}


/******************************************************************************
*
* mpge_phy_get_phy_id - get the phy id
*
*/
static sw_error_t
mpge_phy_get_phy_id(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *phy_data)
{
	return qca808x_phy_get_phy_id (dev_id, phy_id, phy_data);
}

/******************************************************************************
*
* mpge_phy_off - power off the phy
*
*/
static sw_error_t
mpge_phy_poweroff(a_uint32_t dev_id, a_uint32_t phy_id)
{
	return qca808x_phy_poweroff (dev_id, phy_id);
}

/******************************************************************************
*
* mpge_phy_on - power on the phy
*
*/
static sw_error_t
mpge_phy_poweron(a_uint32_t dev_id, a_uint32_t phy_id)
{
	return qca808x_phy_poweron (dev_id, phy_id);
}

#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* mpge_phy_set_hibernate - set hibernate status
*
*/
static sw_error_t
mpge_phy_set_hibernate(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	return qca808x_phy_set_hibernate (dev_id, phy_id, enable);
}

/******************************************************************************
*
* mpge_phy_get_hibernate - get hibernate status
*
*/
static sw_error_t
mpge_phy_get_hibernate(a_uint32_t dev_id, a_uint32_t phy_id,
	a_bool_t * enable)
{
	return qca808x_phy_get_hibernate (dev_id, phy_id, enable);
}

/******************************************************************************
*
* mpge_phy_cdt - cable diagnostic test
*
*/
static sw_error_t
mpge_phy_cdt(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t mdi_pair,
	fal_cable_status_t * cable_status, a_uint32_t * cable_len)
{
	return qca808x_phy_cdt (dev_id, phy_id, mdi_pair,
		cable_status, cable_len);
}

/******************************************************************************
*
* mpge_phy_set_mdix - set phy mdix configuration
*
*/
static sw_error_t
mpge_phy_set_mdix(a_uint32_t dev_id, a_uint32_t phy_id,
		    fal_port_mdix_mode_t mode)
{
	return qca808x_phy_set_mdix(dev_id, phy_id, mode);
}

/******************************************************************************
*
* mpge_phy_get_mdix - get phy mdix configuration
*
*/
static sw_error_t
mpge_phy_get_mdix(a_uint32_t dev_id, a_uint32_t phy_id,
		    fal_port_mdix_mode_t * mode)
{
	return qca808x_phy_get_mdix(dev_id, phy_id, mode);
}

/******************************************************************************
*
* mpge_phy_get_mdix_status - get phy mdix status
*
*/
static sw_error_t
mpge_phy_get_mdix_status(a_uint32_t dev_id, a_uint32_t phy_id,
			   fal_port_mdix_status_t * mode)
{
	return qca808x_phy_get_mdix_status(dev_id, phy_id, mode);
}

/******************************************************************************
*
* mpge_phy_set_local_loopback
*
*/
static sw_error_t
mpge_phy_set_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
	a_bool_t enable)
{
	a_uint16_t phy_data;
	fal_port_speed_t old_speed;
	sw_error_t rv = SW_OK;

	if (enable == A_TRUE)
	{
		mpge_phy_get_speed(dev_id, phy_id, &old_speed);
		if (old_speed == FAL_SPEED_1000)
		{
			phy_data = MPGE_1000M_LOOPBACK;
		}
		else if (old_speed == FAL_SPEED_100)
		{
			phy_data = MPGE_100M_LOOPBACK;
		}
		else if (old_speed == FAL_SPEED_10)
		{
			phy_data = MPGE_10M_LOOPBACK;
		}
		else
		{
			return SW_FAIL;
		}
	}
	else
	{
		phy_data = MPGE_COMMON_CTRL;
	}

	rv = mpge_phy_reg_write(dev_id, phy_id, MPGE_PHY_CONTROL, phy_data);

	return rv;
}

/******************************************************************************
*
* mpge_phy_get_local_loopback
*
*/
static sw_error_t
mpge_phy_get_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
	a_bool_t * enable)
{
	return qca808x_phy_get_local_loopback (dev_id, phy_id, enable);
}

/******************************************************************************
*
* mpge_phy_set_remote_loopback
*
*/
static sw_error_t
mpge_phy_set_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
	a_bool_t enable)
{
	return qca808x_phy_set_remote_loopback (dev_id, phy_id, enable);
}

/******************************************************************************
*
* mpge_phy_get_remote_loopback
*
*/
static sw_error_t
mpge_phy_get_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
	a_bool_t * enable)
{
	return qca808x_phy_get_remote_loopback (dev_id, phy_id, enable);

}

/******************************************************************************
*
* mpge_phy_set_802.3az
*
*/
static sw_error_t
mpge_phy_set_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	return qca808x_phy_set_8023az (dev_id, phy_id, enable);
}

/******************************************************************************
*
* mpge_phy_get_8023az status
*
*/
static sw_error_t
mpge_phy_get_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	return qca808x_phy_get_8023az (dev_id, phy_id, enable);
}

/******************************************************************************
*
* mpge_phy_set wol-frame mac address
*
*/
static sw_error_t
mpge_phy_set_magic_frame_mac(a_uint32_t dev_id, a_uint32_t phy_id,
	fal_mac_addr_t * mac)
{
	return qca808x_phy_set_magic_frame_mac (dev_id, phy_id, mac);
}

/******************************************************************************
*
* mpge_phy_get wol - frame mac address
*
*/
static sw_error_t
mpge_phy_get_magic_frame_mac(a_uint32_t dev_id, a_uint32_t phy_id,
	fal_mac_addr_t * mac)
{
	return qca808x_phy_get_magic_frame_mac (dev_id, phy_id, mac);
}

/******************************************************************************
*
* mpge_phy_set wol - enable or disable
*
*/
static sw_error_t
mpge_phy_set_wol_status(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	return qca808x_phy_set_wol_status (dev_id, phy_id, enable);
}

/******************************************************************************
*
* mpge_phy_get_wol status - get wol status
*
*/
static sw_error_t
mpge_phy_get_wol_status(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	return qca808x_phy_get_wol_status (dev_id, phy_id, enable);
}

/******************************************************************************
*
* mpge_phy_set_counter - set counter status
*
*/
static sw_error_t
mpge_phy_set_counter(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	return qca808x_phy_set_counter (dev_id, phy_id, enable);
}

/******************************************************************************
*
* mpge_phy_get_counter_status - get counter status
*
*/
static sw_error_t
mpge_phy_get_counter(a_uint32_t dev_id, a_uint32_t phy_id,
	a_bool_t * enable)
{
	return qca808x_phy_get_counter (dev_id, phy_id, enable);
}

/******************************************************************************
*
* mpge_phy_show show - counter statistics
*
*/
static sw_error_t
mpge_phy_show_counter(a_uint32_t dev_id, a_uint32_t phy_id,
	fal_port_counter_info_t * counter_infor)
{
	return qca808x_phy_show_counter (dev_id, phy_id, counter_infor);
}

/******************************************************************************
*
* mpge_phy_set_intr_mask - Set interrupt mask with the
* specified device.
*/
sw_error_t
mpge_phy_set_intr_mask(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t intr_mask_flag)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	phy_data = mpge_phy_reg_read(dev_id, phy_id, MPGE_PHY_INTR_MASK);
	PHY_RTN_ON_READ_ERROR(phy_data);

	if (intr_mask_flag & FAL_PHY_INTR_STATUS_UP_CHANGE) {
		phy_data |= MPGE_INTR_STATUS_LINK_UP;
	} else {
		phy_data &= (~MPGE_INTR_STATUS_LINK_UP);
	}

	if (intr_mask_flag & FAL_PHY_INTR_STATUS_DOWN_CHANGE) {
		phy_data |= MPGE_INTR_STATUS_LINK_DOWN;
	} else {
		phy_data &= (~MPGE_INTR_STATUS_LINK_DOWN);
	}

	if (intr_mask_flag & FAL_PHY_INTR_SPEED_CHANGE) {
		phy_data |= MPGE_INTR_SPEED_CHANGE;
	} else {
		phy_data &= (~MPGE_INTR_SPEED_CHANGE);
	}

	if (intr_mask_flag & FAL_PHY_INTR_DUPLEX_CHANGE) {
		phy_data |= MPGE_INTR_DUPLEX_CHANGE;
	} else {
		phy_data &= (~MPGE_INTR_DUPLEX_CHANGE);
	}

	if (intr_mask_flag & FAL_PHY_INTR_WOL_STATUS) {
		phy_data |= MPGE_INTR_WOL;
	} else {
		phy_data &= (~MPGE_INTR_WOL);
	}

	rv = mpge_phy_reg_write(dev_id, phy_id, MPGE_PHY_INTR_MASK, phy_data);

	return rv;
}

/******************************************************************************
*
* mpge_phy_get_intr_mask - Get interrupt mask with the
* specified device.
*/
sw_error_t
mpge_phy_get_intr_mask(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t * intr_mask_flag)
{
	a_uint16_t phy_data = 0;

	phy_data = mpge_phy_reg_read(dev_id, phy_id, MPGE_PHY_INTR_MASK);
	PHY_RTN_ON_READ_ERROR(phy_data);

	*intr_mask_flag = 0;
	if (phy_data & MPGE_INTR_STATUS_LINK_UP) {
		*intr_mask_flag |= FAL_PHY_INTR_STATUS_UP_CHANGE;
	}

	if (phy_data & MPGE_INTR_STATUS_LINK_DOWN) {
		*intr_mask_flag |= FAL_PHY_INTR_STATUS_DOWN_CHANGE;
	}

	if (phy_data & MPGE_INTR_SPEED_CHANGE) {
		*intr_mask_flag |= FAL_PHY_INTR_SPEED_CHANGE;
	}

	if (phy_data & MPGE_INTR_DUPLEX_CHANGE) {
		*intr_mask_flag |= FAL_PHY_INTR_DUPLEX_CHANGE;
	}

	if (phy_data & MPGE_INTR_WOL) {
		*intr_mask_flag |= FAL_PHY_INTR_WOL_STATUS;
	}

	return SW_OK;
}

/******************************************************************************
*
* mpge_phy_get_intr_status - Get interrupt status with the
* specified device.
*/
sw_error_t
mpge_phy_get_intr_status(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t * intr_status_flag)
{
	a_uint16_t phy_data = 0;

	phy_data = mpge_phy_reg_read(dev_id, phy_id, MPGE_PHY_INTR_STATUS);
	PHY_RTN_ON_READ_ERROR(phy_data);

	*intr_status_flag = 0;
	if (phy_data & MPGE_INTR_STATUS_LINK_UP) {
		*intr_status_flag |= FAL_PHY_INTR_STATUS_UP_CHANGE;
	}

	if (phy_data & MPGE_INTR_STATUS_LINK_DOWN) {
		*intr_status_flag |= FAL_PHY_INTR_STATUS_DOWN_CHANGE;
	}

	if (phy_data & MPGE_INTR_SPEED_CHANGE) {
		*intr_status_flag |= FAL_PHY_INTR_SPEED_CHANGE;
	}

	if (phy_data & MPGE_INTR_DUPLEX_CHANGE) {
		*intr_status_flag |= FAL_PHY_INTR_DUPLEX_CHANGE;
	}

	if (phy_data & MPGE_INTR_WOL) {
		*intr_status_flag |= FAL_PHY_INTR_WOL_STATUS;
	}

	return SW_OK;
}
#endif

/******************************************************************************
*
* mpge_phy_set_eee_advertisement
*
*/
static sw_error_t
mpge_phy_set_eee_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t adv)
{
	return qca808x_phy_set_eee_adv (dev_id, phy_id, adv);
}

/******************************************************************************
*
* mpge_phy_get_eee_advertisement
*
*/
static sw_error_t
mpge_phy_get_eee_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *adv)
{
	return qca808x_phy_get_eee_adv (dev_id, phy_id, adv);
}

/******************************************************************************
*
* mpge_phy_get_eee_partner_advertisement
*
*/
static sw_error_t
mpge_phy_get_eee_partner_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *adv)
{
	return qca808x_phy_get_eee_partner_adv (dev_id, phy_id, adv);
}

/******************************************************************************
*
* mpge_phy_get_eee_capability
*
*/
static sw_error_t
mpge_phy_get_eee_cap(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *cap)
{
	return qca808x_phy_get_eee_cap (dev_id, phy_id, cap);
}

/******************************************************************************
*
* mpge_phy_get_eee_status - get eee status
*
*/
static sw_error_t
mpge_phy_get_eee_status(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *status)
{
	return qca808x_phy_get_eee_status (dev_id, phy_id, status);
}
/******************************************************************************
*
* mpge_phy_cdt_thresh_set - set CDT threshold
*
* set CDT threshold
*/
static sw_error_t
mpge_phy_cdt_thresh_init(a_uint32_t dev_id, a_uint32_t phy_id)
{
	sw_error_t rv = SW_OK;

	rv = mpge_phy_mmd_write(dev_id, phy_id, MPGE_PHY_MMD3_NUM,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL3,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL3_VAL);
	SW_RTN_ON_ERROR(rv);
	rv = mpge_phy_mmd_write(dev_id, phy_id, MPGE_PHY_MMD3_NUM,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL4,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL4_VAL);
	SW_RTN_ON_ERROR(rv);
	rv = mpge_phy_mmd_write(dev_id, phy_id, MPGE_PHY_MMD3_NUM,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL5,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL5_VAL);
	SW_RTN_ON_ERROR(rv);
	rv = mpge_phy_mmd_write(dev_id, phy_id, MPGE_PHY_MMD3_NUM,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL6,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL6_VAL);
	SW_RTN_ON_ERROR(rv);
	rv = mpge_phy_mmd_write(dev_id, phy_id, MPGE_PHY_MMD3_NUM,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL7,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL7_VAL);
	SW_RTN_ON_ERROR(rv);
	rv = mpge_phy_mmd_write(dev_id, phy_id, MPGE_PHY_MMD3_NUM,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL9,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL9_VAL);
	SW_RTN_ON_ERROR(rv);
	rv = mpge_phy_mmd_write(dev_id, phy_id, MPGE_PHY_MMD3_NUM,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL13,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL13_VAL);
	SW_RTN_ON_ERROR(rv);
	rv = mpge_phy_mmd_write(dev_id, phy_id, MPGE_PHY_MMD3_NUM,
		MPGE_PHY_MMD3_CDT_THRESH_CTRL14,
		MPGE_PHY_MMD3_NEAR_ECHO_THRESH_VAL);

	return rv;
}

/******************************************************************************
*
* mpge_phy_function_reset - do function reset
*
*/
static sw_error_t
mpge_phy_function_reset(a_uint32_t dev_id, a_uint32_t phy_id,
	hsl_phy_function_reset_t phy_reset_type)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	switch (phy_reset_type)
	{
		case PHY_FIFO_RESET:
			phy_data = mpge_phy_reg_read (dev_id, phy_id, MPGE_PHY_FIFO_CONTROL);
			PHY_RTN_ON_READ_ERROR(phy_data);

			rv = mpge_phy_reg_write(dev_id, phy_id, MPGE_PHY_FIFO_CONTROL,
				phy_data & (~MPGE_PHY_FIFO_RESET));
			SW_RTN_ON_ERROR(rv);

			aos_mdelay(50);

			rv = mpge_phy_reg_write(dev_id, phy_id, MPGE_PHY_FIFO_CONTROL,
			phy_data | MPGE_PHY_FIFO_RESET);
			SW_RTN_ON_ERROR(rv);
			break;
		default:
			return SW_NOT_SUPPORTED;
	}

	return rv;
}

static void
mpge_phy_lock_init(void)
{
	return qca808x_phy_lock_init();
}

static sw_error_t
mpge_phy_dac_set(a_uint32_t dev_id, a_uint32_t phy_id, phy_dac_t phy_dac)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	if(phy_dac.mdac != PHY_INVALID_DAC)
	{
		SSDK_INFO("phy mdac is set as 0x%x\n", phy_dac.mdac);
		/*set mdac value*/
		phy_data = mpge_phy_mmd_read(dev_id, phy_id, MPGE_PHY_MMD1_NUM,
			MPGE_PHY_MMD1_DAC);
		PHY_RTN_ON_READ_ERROR(phy_data);
		phy_data &= ~(BITS(8,8));
		rv = mpge_phy_mmd_write(dev_id, phy_id, MPGE_PHY_MMD1_NUM,
			MPGE_PHY_MMD1_DAC, phy_data | PHY_DAC(phy_dac.mdac));
		SW_RTN_ON_ERROR(rv);
	}
	if(phy_dac.edac != PHY_INVALID_DAC)
	{
		SSDK_INFO("phy edac is set as 0x%x\n", phy_dac.edac);
		/*set edac value*/
		phy_data = mpge_phy_debug_read(dev_id, phy_id, MPGE_PHY_DEBUG_EDAC);
		PHY_RTN_ON_READ_ERROR(phy_data);
		phy_data &= ~(BITS(8,8));
		rv = mpge_phy_debug_write(dev_id, phy_id, MPGE_PHY_DEBUG_EDAC,
			phy_data | PHY_DAC(phy_dac.edac));
		SW_RTN_ON_ERROR(rv);
	}

	return rv;
}

static void
mpge_phy_dac_init(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t port_id)
{
	phy_dac_t  phy_dac;

	hsl_port_phy_dac_get(dev_id, port_id, &phy_dac);
	mpge_phy_dac_set(dev_id, phy_id, phy_dac);

	return;
}

static sw_error_t
mpge_phy_ldo_efuse_set(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t efuse_value)
{
	a_uint16_t phy_data = 0, phy_data1 = 0;
	sw_error_t rv = SW_OK;

	/*when set the register of MPGE_PHY_DEBUG_ANA_LDO_EFUSE, the register of
	MPGE_PHY_DEBUG_ANA_DAC_FILTER will be changed automatically, so need to
	save it and restore it*/
	phy_data1 = mpge_phy_debug_read(dev_id, phy_id, MPGE_PHY_DEBUG_ANA_DAC_FILTER);
	PHY_RTN_ON_READ_ERROR(phy_data1);

	phy_data = mpge_phy_debug_read(dev_id, phy_id, MPGE_PHY_DEBUG_ANA_LDO_EFUSE);
	PHY_RTN_ON_READ_ERROR(phy_data);
	phy_data &= ~(BITS(4,4));
	rv = mpge_phy_debug_write(dev_id, phy_id, MPGE_PHY_DEBUG_ANA_LDO_EFUSE,
		phy_data | efuse_value);
	SW_RTN_ON_ERROR(rv);
	rv = mpge_phy_debug_write(dev_id, phy_id, MPGE_PHY_DEBUG_ANA_DAC_FILTER,
		phy_data1);

	return rv;
}

static sw_error_t
mpge_phy_hw_init(a_uint32_t dev_id,  a_uint32_t port_bmp)
{
	a_uint32_t port_id = 0, phy_addr = 0;
	sw_error_t rv = SW_OK;

	for (port_id = SSDK_PHYSICAL_PORT0; port_id < SW_MAX_NR_PORT; port_id ++)
	{
		if (port_bmp & (0x1 << port_id))
		{
			phy_addr = qca_ssdk_port_to_phy_addr(dev_id, port_id);
			SW_RTN_ON_ERROR(rv);
			/*configure the CDT threshold*/
			rv = mpge_phy_cdt_thresh_init (dev_id, phy_addr);
			SW_RTN_ON_ERROR(rv);
			/*set LDO efuse as default and make ICC efuse take effect only*/
			rv = mpge_phy_ldo_efuse_set(dev_id, phy_addr,
				MPGE_PHY_DEBUG_ANA_LDO_EFUSE_DEFAULT);
			/*special configuration for AZ*/
			rv = mpge_phy_mmd_write(dev_id, phy_addr, MPGE_PHY_MMD3_NUM,
				MPGE_PHY_MMD3_AZ_CTRL1, MPGE_PHY_MMD3_AZ_CTRL1_VAL);
			SW_RTN_ON_ERROR(rv);
			rv = mpge_phy_mmd_write(dev_id, phy_addr, MPGE_PHY_MMD3_NUM,
				MPGE_PHY_MMD3_AZ_CTRL2, MPGE_PHY_MMD3_AZ_CTRL2_VAL);
			SW_RTN_ON_ERROR(rv);
			/*configure MSE threshold and over threshold times*/
			rv = mpge_phy_mmd_write(dev_id, phy_addr, MPGE_PHY_MMD1_NUM,
				MPGE_PHY_MMD1_MSE_THRESH1, MPGE_PHY_MMD1_MSE_THRESH1_VAL);
			SW_RTN_ON_ERROR(rv);
			rv = mpge_phy_mmd_write(dev_id, phy_addr, MPGE_PHY_MMD1_NUM,
				MPGE_PHY_MMD1_MSE_THRESH2, MPGE_PHY_MMD1_MSE_THRESH2_VAL);
			SW_RTN_ON_ERROR(rv);
			mpge_phy_dac_init(dev_id, phy_addr, port_id);
		}
	}

	return rv;
}

static sw_error_t mpge_phy_api_ops_init(void)
{
	sw_error_t ret = SW_OK;
	hsl_phy_ops_t *mpge_phy_api_ops = NULL;

	mpge_phy_api_ops = kzalloc(sizeof(hsl_phy_ops_t), GFP_KERNEL);
	if (mpge_phy_api_ops == NULL)
	{
		SSDK_ERROR("mpge phy ops kzalloc failed!\n");
		return -ENOMEM;
	}

	phy_api_ops_init(MPGE_PHY_CHIP);

	mpge_phy_api_ops->phy_reg_write = mpge_phy_reg_write;
	mpge_phy_api_ops->phy_reg_read = mpge_phy_reg_read;
	mpge_phy_api_ops->phy_debug_write = mpge_phy_debug_write;
	mpge_phy_api_ops->phy_debug_read = mpge_phy_debug_read;
	mpge_phy_api_ops->phy_mmd_write = mpge_phy_mmd_write;
	mpge_phy_api_ops->phy_mmd_read = mpge_phy_mmd_read;
	mpge_phy_api_ops->phy_get_status = mpge_phy_get_status;
	mpge_phy_api_ops->phy_speed_get = mpge_phy_get_speed;
	mpge_phy_api_ops->phy_speed_set = mpge_phy_set_speed;
	mpge_phy_api_ops->phy_duplex_get = mpge_phy_get_duplex;
	mpge_phy_api_ops->phy_duplex_set = mpge_phy_set_duplex;
	mpge_phy_api_ops->phy_autoneg_enable_set = mpge_phy_enable_autoneg;
	mpge_phy_api_ops->phy_restart_autoneg = mpge_phy_restart_autoneg;
	mpge_phy_api_ops->phy_autoneg_status_get = mpge_phy_autoneg_status;
	mpge_phy_api_ops->phy_autoneg_adv_set = mpge_phy_set_autoneg_adv;
	mpge_phy_api_ops->phy_autoneg_adv_get = mpge_phy_get_autoneg_adv;
	mpge_phy_api_ops->phy_link_status_get = mpge_phy_get_link_status;
	mpge_phy_api_ops->phy_reset = mpge_phy_reset;
	mpge_phy_api_ops->phy_id_get = mpge_phy_get_phy_id;
	mpge_phy_api_ops->phy_power_off = mpge_phy_poweroff;
	mpge_phy_api_ops->phy_power_on = mpge_phy_poweron;
#ifndef IN_PORTCONTROL_MINI
	mpge_phy_api_ops->phy_cdt = mpge_phy_cdt;
	mpge_phy_api_ops->phy_mdix_set = mpge_phy_set_mdix;
	mpge_phy_api_ops->phy_mdix_get = mpge_phy_get_mdix;
	mpge_phy_api_ops->phy_mdix_status_get = mpge_phy_get_mdix_status;
	mpge_phy_api_ops->phy_local_loopback_set = mpge_phy_set_local_loopback;
	mpge_phy_api_ops->phy_local_loopback_get = mpge_phy_get_local_loopback;
	mpge_phy_api_ops->phy_remote_loopback_set = mpge_phy_set_remote_loopback;
	mpge_phy_api_ops->phy_remote_loopback_get = mpge_phy_get_remote_loopback;
	mpge_phy_api_ops->phy_8023az_set = mpge_phy_set_8023az;
	mpge_phy_api_ops->phy_8023az_get = mpge_phy_get_8023az;
	mpge_phy_api_ops->phy_hibernation_set = mpge_phy_set_hibernate;
	mpge_phy_api_ops->phy_hibernation_get = mpge_phy_get_hibernate;
	mpge_phy_api_ops->phy_magic_frame_mac_set = mpge_phy_set_magic_frame_mac;
	mpge_phy_api_ops->phy_magic_frame_mac_get = mpge_phy_get_magic_frame_mac;
	mpge_phy_api_ops->phy_counter_set = mpge_phy_set_counter;
	mpge_phy_api_ops->phy_counter_get = mpge_phy_get_counter;
	mpge_phy_api_ops->phy_counter_show = mpge_phy_show_counter;
	mpge_phy_api_ops->phy_wol_status_set = mpge_phy_set_wol_status;
	mpge_phy_api_ops->phy_wol_status_get = mpge_phy_get_wol_status;
	mpge_phy_api_ops->phy_intr_mask_set = mpge_phy_set_intr_mask;
	mpge_phy_api_ops->phy_intr_mask_get = mpge_phy_get_intr_mask;
	mpge_phy_api_ops->phy_intr_status_get = mpge_phy_get_intr_status;
#endif
	mpge_phy_api_ops->phy_eee_adv_set = mpge_phy_set_eee_adv;
	mpge_phy_api_ops->phy_eee_adv_get = mpge_phy_get_eee_adv;
	mpge_phy_api_ops->phy_eee_partner_adv_get = mpge_phy_get_eee_partner_adv;
	mpge_phy_api_ops->phy_eee_cap_get = mpge_phy_get_eee_cap;
	mpge_phy_api_ops->phy_eee_status_get = mpge_phy_get_eee_status;
	mpge_phy_api_ops->phy_function_reset = mpge_phy_function_reset;
#ifdef IN_LED
	mpge_phy_led_api_ops_init(mpge_phy_api_ops);
#endif
	ret = hsl_phy_api_ops_register(MPGE_PHY_CHIP, mpge_phy_api_ops);

	if (ret == SW_OK)
	{
		SSDK_INFO("qca probe mpge phy driver succeeded!\n");
	}
	else
	{
		SSDK_ERROR("qca probe mpge phy driver failed! (code: %d)\n", ret);
	}

	return ret;
}

/******************************************************************************
*
* mpge_phy_init -
*
*/
int mpge_phy_init(a_uint32_t dev_id, a_uint32_t port_bmp)
{
	a_int32_t ret = 0;
	static a_bool_t phy_ops_flag = A_FALSE;

	if(phy_ops_flag == A_FALSE &&
			mpge_phy_api_ops_init() == SW_OK) {
		mpge_phy_lock_init();
		phy_ops_flag = A_TRUE;
	}
	mpge_phy_hw_init(dev_id, port_bmp);

	return ret;
}
