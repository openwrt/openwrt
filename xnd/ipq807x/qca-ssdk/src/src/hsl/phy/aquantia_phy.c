/*
 * Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
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
#include "aquantia_phy.h"
#include "hsl_phy.h"
#include "ssdk_plat.h"

/* #define aquantia_phy_reg_read _phy_reg_read  */
/* #define aquantia_phy_reg_write _phy_reg_write */

/******************************************************************************
*
* aquantia_phy_mii_read - mii register read
*
* mii register read
*/
static sw_error_t
aquantia_phy_reg_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_mmd,
	a_uint32_t reg_id, a_uint16_t *phy_data)
{
	sw_error_t rv;

	reg_id = AQUANTIA_REG_ADDRESS(reg_mmd, reg_id);
	HSL_PHY_GET(rv, dev_id, phy_id, reg_id, phy_data);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_mii_write - mii register write
*
* mii register write
*/
static sw_error_t
aquantia_phy_reg_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_mmd,
	a_uint32_t reg_id, a_uint16_t reg_val)
{
	sw_error_t rv;

	reg_id = AQUANTIA_REG_ADDRESS(reg_mmd, reg_id);
	HSL_PHY_SET(rv, dev_id,  phy_id, reg_id, reg_val);

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* aquantia_phy_get_phy_id - get the phy id
*
*/
sw_error_t
aquantia_phy_get_phy_id(a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *phy_data)
{
	sw_error_t rv;
	a_uint16_t org_id, rev_id;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PHY_XS_REGISTERS,
		AQUANTIA_PHY_ID1, &org_id);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PHY_XS_REGISTERS,
		AQUANTIA_PHY_ID2, &rev_id);
	SW_RTN_ON_ERROR(rv);

	*phy_data = ((org_id & 0xffff) << 16) | (rev_id & 0xffff);

	return rv;
}
#endif
sw_error_t
aquantia_phy_get_speed(a_uint32_t dev_id, a_uint32_t phy_id,
		     fal_port_speed_t * speed)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;
	a_bool_t link_status;

	link_status = aquantia_phy_get_link_status(dev_id, phy_id);
	if (link_status != A_TRUE) {
		/*the speed register(0x4007c800) is not stable when aquantia phy is down,
		 but some APIs such as aquantia_phy_set_duplex() aquantia_phy_interface_set_mode()
		 need to get the speed, so set the speed default value as 100M when link down*/
		*speed = FAL_SPEED_100;
		return SW_OK;
	}

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_REG_AUTONEG_VENDOR_STATUS, &phy_data);
	SW_RTN_ON_ERROR(rv);
	switch ((phy_data & AQUANTIA_STATUS_SPEED_MASK) >> 1) {
		case AQUANTIA_STATUS_SPEED_100MBS:
			*speed = FAL_SPEED_100;
			break;
		case AQUANTIA_STATUS_SPEED_1000MBS:
			*speed = FAL_SPEED_1000;
			break;
		case AQUANTIA_STATUS_SPEED_10000MBS:
			*speed = FAL_SPEED_10000;
			break;
		case AQUANTIA_STATUS_SPEED_2500MBS:
			*speed = FAL_SPEED_2500;
			break;
		case AQUANTIA_STATUS_SPEED_5000MBS:
			*speed = FAL_SPEED_5000;
			break;
		default:
			return SW_READ_ERROR;
	}

	return rv;
}

/******************************************************************************
*
* aquantia_phy_get_duplex - Determines the speed of phy ports associated with the
* specified device.
*/
sw_error_t
aquantia_phy_get_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
		      fal_port_duplex_t * duplex)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;
	a_bool_t link_status;

	link_status = aquantia_phy_get_link_status(dev_id, phy_id);
	if (link_status != A_TRUE)
	{
		return SW_OK;
	}

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_REG_AUTONEG_VENDOR_STATUS, &phy_data);
	SW_RTN_ON_ERROR(rv);
	//read duplex
	if (phy_data & AQUANTIA_STATUS_FULL_DUPLEX)
	{
		*duplex = FAL_FULL_DUPLEX;
	}
	else
	{
		*duplex = FAL_HALF_DUPLEX;
	}

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* aquantia_phy_reset - reset the phy
*
* reset the phy
*/
sw_error_t aquantia_phy_reset(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_GLOBAL_STANDARD_CONTROL1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
	AQUANTIA_GLOBAL_STANDARD_CONTROL1, phy_data | AQUANTIA_CTRL_SOFTWARE_RESET);
	aos_mdelay(100);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_set_powersave - set power saving status
*
* set power saving status
*/
sw_error_t
aquantia_phy_set_powersave(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;
	sw_error_t rv;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_GLOBAL_RESERVED_PROVISIONING6, &phy_data);
	SW_RTN_ON_ERROR(rv);
	if (enable == A_TRUE)
	{
		phy_data |= AQUANTIA_POWER_SAVE;
	}
	else
	{
		phy_data &= ~AQUANTIA_POWER_SAVE;
	}
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_GLOBAL_RESERVED_PROVISIONING6,phy_data);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_restart_autoneg(dev_id, phy_id);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_get_powersave - get power saving status
*
* set power saving status
*/
sw_error_t
aquantia_phy_get_powersave(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_bool_t * enable)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_GLOBAL_RESERVED_PROVISIONING6, &phy_data);
	SW_RTN_ON_ERROR(rv);
	if (phy_data& AQUANTIA_POWER_SAVE)
	{
		*enable = A_TRUE;
	}
	else
	{
		*enable = A_FALSE;
	}

	return rv;
}

/******************************************************************************
*
* aquantia_phy_set_mdix -
*
* set phy mdix configuraiton
*/
sw_error_t
aquantia_phy_set_mdix(a_uint32_t dev_id, a_uint32_t phy_id,
		    fal_port_mdix_mode_t mode)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_RESERVED_VENDOR_PROVISIONING1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= ~(BITS(0,2));
	switch(mode)
	{
		case PHY_MDIX_AUTO:
			phy_data |= AQUANTIA_PHY_MDIX_AUTO;
			break;
		case PHY_MDIX_MDIX:
			phy_data |= AQUANTIA_PHY_MDIX;
			break;
		case PHY_MDIX_MDI:
			phy_data |= AQUANTIA_PHY_MDI;
			break;
		default:
			return SW_BAD_PARAM;
	}
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_RESERVED_VENDOR_PROVISIONING1,phy_data);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_restart_autoneg(dev_id, phy_id);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_get_mdix
*
* get phy mdix configuration
*/
sw_error_t
aquantia_phy_get_mdix(a_uint32_t dev_id, a_uint32_t phy_id,
		    fal_port_mdix_mode_t * mode)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_RESERVED_VENDOR_PROVISIONING1,&phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data  &= BITS(0,2);
	switch(phy_data)
	{
		case AQUANTIA_PHY_MDIX_AUTO:
			*mode = PHY_MDIX_AUTO;
			break;
		case AQUANTIA_PHY_MDIX:
			*mode = PHY_MDIX_MDIX;
			break;
		case AQUANTIA_PHY_MDI:
			*mode = PHY_MDIX_MDI;
			break;
		default:
			return SW_NOT_SUPPORTED;
	}

	return rv;
}

/******************************************************************************
*
* aquantia_phy_get_mdix status
*
* get phy mdix status
*/
sw_error_t
aquantia_phy_get_mdix_status(a_uint32_t dev_id, a_uint32_t phy_id,
			   fal_port_mdix_status_t * mode)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_RESERVED_VENDOR_STATUS1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	*mode = (phy_data &  AQUANTIA_PHY_MDIX_STATUS) ? PHY_MDIX_STATUS_MDIX :
	    PHY_MDIX_STATUS_MDI;

	return rv;
}

/******************************************************************************
*
* aquantia_phy_set_local_loopback
*
* set phy local loopback
*/
sw_error_t
aquantia_phy_set_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			      a_bool_t enable)
{
	a_uint16_t phy_data;
	fal_port_speed_t old_speed;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PHY_XS_REGISTERS,
		AQUANTIA_PHY_XS_TRANAMIT_RESERVED_VENDOR_PROVISION5, &phy_data);
	SW_RTN_ON_ERROR(rv);
	if (enable == A_TRUE)
	{
		phy_data |= AQUANTIA_INTERNAL_LOOPBACK;
		rv = aquantia_phy_get_speed(dev_id, phy_id, &old_speed);
		SW_RTN_ON_ERROR(rv);
		switch(old_speed)
		{
			case FAL_SPEED_100:
				phy_data |= AQUANTIA_100M_LOOPBACK;
				break;
			case FAL_SPEED_1000:
				 phy_data |= AQUANTIA_1000M_LOOPBACK;
				 break;
			case FAL_SPEED_10000:
				phy_data |= AQUANTIA_10000M_LOOPBACK;
				break;
			case FAL_SPEED_2500:
				 phy_data |= AQUANTIA_2500M_LOOPBACK;
				 break;
			case FAL_SPEED_5000:
				phy_data |= AQUANTIA_5000M_LOOPBACK;
				break;
			default:
				return SW_FAIL;
		}
	}
	else
	{
		phy_data &= ~(AQUANTIA_INTERNAL_LOOPBACK | AQUANTIA_ALL_SPEED_LOOPBACK);
	}
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_PHY_XS_REGISTERS,
		AQUANTIA_PHY_XS_TRANAMIT_RESERVED_VENDOR_PROVISION5,phy_data);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_get_local_loopback
*
* get phy local loopback
*/
sw_error_t
aquantia_phy_get_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			      a_bool_t * enable)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PHY_XS_REGISTERS,
		AQUANTIA_PHY_XS_TRANAMIT_RESERVED_VENDOR_PROVISION5, &phy_data);
	SW_RTN_ON_ERROR(rv);
	if (phy_data & AQUANTIA_INTERNAL_LOOPBACK)
	{
		*enable = A_TRUE;
	}
	else
	{
		*enable = A_FALSE;
	}

	return rv;
}

sw_error_t
aquantia_phy_set_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
				   a_bool_t enable)
{
	a_uint16_t phy_data;
	fal_port_speed_t speed;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PHY_XS_REGISTERS,
		AQUANTIA_PHY_XS_TRANAMIT_RESERVED_VENDOR_PROVISION5, &phy_data);
	SW_RTN_ON_ERROR(rv);

	if (enable == A_TRUE)
	{
		rv = aquantia_phy_get_speed(dev_id,  phy_id, &speed);
		SW_RTN_ON_ERROR(rv);
		switch(speed)
		{
			case FAL_SPEED_100:
				phy_data |= AQUANTIA_100M_LOOPBACK;
				break;
			case FAL_SPEED_1000:
				phy_data |= AQUANTIA_1000M_LOOPBACK;
				break;
			case FAL_SPEED_2500:
				phy_data |= AQUANTIA_2500M_LOOPBACK;
				break;
			case FAL_SPEED_5000:
				phy_data |= AQUANTIA_5000M_LOOPBACK;
				break;
			case FAL_SPEED_10000:
				phy_data |= AQUANTIA_10000M_LOOPBACK;
				break;
			default:
				break;
		}
		phy_data |= AQUANTIA_PHY_REMOTE_LOOPBACK;
	}
	else
	{
		phy_data &= ~(AQUANTIA_PHY_REMOTE_LOOPBACK |AQUANTIA_ALL_SPEED_LOOPBACK);
	}
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_PHY_XS_REGISTERS,
		AQUANTIA_PHY_XS_TRANAMIT_RESERVED_VENDOR_PROVISION5, phy_data);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_get_remote_loopback
*
* get phy remote loopback
*/
sw_error_t
aquantia_phy_get_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
				   a_bool_t * enable)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PHY_XS_REGISTERS,
		AQUANTIA_PHY_XS_TRANAMIT_RESERVED_VENDOR_PROVISION5, &phy_data);
	SW_RTN_ON_ERROR(rv);
	if (phy_data & AQUANTIA_PHY_REMOTE_LOOPBACK)
	{
		*enable = A_TRUE;
	}
	else
	{
		*enable = A_FALSE;
	}

	return rv;
}

/******************************************************************************
*
* aquantia_phy_cdt - cable diagnostic test
*
* cable diagnostic test
*/
static inline fal_cable_status_t _phy_cdt_status_mapping(a_uint32_t pair_type, a_uint16_t status)
{
	fal_cable_status_t status_mapping = FAL_CABLE_STATUS_INVALID;

	switch(status)
	{
		case 0:
			status_mapping = FAL_CABLE_STATUS_NORMAL;
			break;
		case 1:
			if(pair_type == CABLE_PAIR_B)
				status_mapping = FAL_CABLE_STATUS_CROSSOVERA;
			else if(pair_type == CABLE_PAIR_C)
				status_mapping = FAL_CABLE_STATUS_CROSSOVERB;
			else if(pair_type == CABLE_PAIR_D)
				status_mapping = FAL_CABLE_STATUS_CROSSOVERC;
			else
				status_mapping = FAL_CABLE_STATUS_INVALID;
			break;
		case 2:
			if(pair_type == CABLE_PAIR_C)
				status_mapping = FAL_CABLE_STATUS_CROSSOVERA;
			else if(pair_type == CABLE_PAIR_D)
				status_mapping = FAL_CABLE_STATUS_CROSSOVERB;
			else
				status_mapping = FAL_CABLE_STATUS_INVALID;
			break;
		case 3:
			if(pair_type == CABLE_PAIR_D)
				status_mapping = FAL_CABLE_STATUS_CROSSOVERA;
			else
				status_mapping = FAL_CABLE_STATUS_INVALID;
			break;
		case 4:
			status_mapping = FAL_CABLE_STATUS_SHORT;
			break;
		case 5:
			status_mapping = FAL_CABLE_STATUS_LOW_MISMATCH;
			break;
		case 6:
			status_mapping = FAL_CABLE_STATUS_HIGH_MISMATCH;
			break;
		case 7:
			status_mapping = FAL_CABLE_STATUS_OPENED;
			break;
		default:
			status_mapping = FAL_CABLE_STATUS_INVALID;
			break;
	}

	return status_mapping;
}

sw_error_t
aquantia_phy_cdt_get(a_uint32_t dev_id, a_uint32_t phy_id,
		   fal_port_cdt_t * port_cdt)
{
	a_uint16_t status = 0;
	sw_error_t rv = SW_OK;
	a_uint16_t phy_data;

	if ((!port_cdt) || (phy_id > 7)) {
		return SW_FAIL;
	}
	/* Get cable status */
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_CABLE_DIAGNOSTIC_STATUS1, &status);
	SW_RTN_ON_ERROR(rv);
	port_cdt->pair_a_status =  (status & AQUANTIA_CABLE_DIAGNOSTIC_STATUS_PAIRA) >> 12
		& BITS(0, 3);
	port_cdt->pair_b_status = (status & AQUANTIA_CABLE_DIAGNOSTIC_STATUS_PAIRB) >> 8
		& BITS(0, 3);
	port_cdt->pair_c_status = (status & AQUANTIA_CABLE_DIAGNOSTIC_STATUS_PAIRC) >> 4
		& BITS(0, 3);
	port_cdt->pair_d_status = (status & AQUANTIA_CABLE_DIAGNOSTIC_STATUS_PAIRD)
		& BITS(0, 3);
	SSDK_DEBUG("status:%x, pair_a_status:%x,pair_b_status:%x,pair_c_status:%x, pair_d_status:%x\n",
		status, port_cdt->pair_a_status,port_cdt->pair_b_status,
		port_cdt->pair_c_status, port_cdt->pair_d_status);
	/* Get Cable Length value */
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_CABLE_DIAGNOSTIC_STATUS2, &phy_data);
	SW_RTN_ON_ERROR(rv);
	port_cdt->pair_a_len = phy_data >> 8 & BITS(0, 8);

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_CABLE_DIAGNOSTIC_STATUS4, &phy_data);
	SW_RTN_ON_ERROR(rv);
	port_cdt->pair_b_len = phy_data >> 8 & BITS(0, 8);

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_CABLE_DIAGNOSTIC_STATUS6, &phy_data);
	SW_RTN_ON_ERROR(rv);
	port_cdt->pair_c_len = phy_data >> 8 & BITS(0, 8);

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_CABLE_DIAGNOSTIC_STATUS8, &phy_data);
	SW_RTN_ON_ERROR(rv);
	port_cdt->pair_d_len = phy_data >> 8 & BITS(0, 8);

	return rv;
}

sw_error_t aquatia_phy_cdt_start(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t status = 0, phy_data = 0;
	a_uint32_t aq_phy_id;
	a_uint16_t ii = 300;
	sw_error_t rv = SW_OK;

	/*select mode0 if aq107, and select mode2 if aq109*/
	rv = aquantia_phy_get_phy_id(dev_id, phy_id, &aq_phy_id);
	SW_RTN_ON_ERROR(rv);
	rv  = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_GLOBAL_CDT_CONTROL, &phy_data);
	SW_RTN_ON_ERROR(rv);
	if(aq_phy_id == AQUANTIA_PHY_109)
	{
		phy_data |= AQUANTIA_PHY_CDT_MODE2;
	}
	else
	{
		phy_data |= AQUANTIA_PHY_CDT_MODE0;
	}

	phy_data |= AQUANTIA_NORMAL_CABLE_DIAGNOSTICS;
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		 AQUANTIA_GLOBAL_CDT_CONTROL, phy_data);
	SW_RTN_ON_ERROR(rv);
	do {
		aos_mdelay(30);
		rv  = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
			AQUANTIA_GLOBAL_GENERAL_STATUS, &status);
		SW_RTN_ON_ERROR(rv);
	}
	while ((status & AQUANTIA_CABLE_DIAGNOSTICS_STATUS) && (--ii));

	return rv;
}

sw_error_t
aquantia_phy_cdt(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t mdi_pair,
	       fal_cable_status_t * cable_status, a_uint32_t * cable_len)
{
	fal_port_cdt_t aquantia_port_cdt;
	sw_error_t rv = SW_OK;

	if ((mdi_pair >= 4) || (phy_id > 7)) {
		return SW_BAD_PARAM;
	}
	rv = aquatia_phy_cdt_start(dev_id, phy_id);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_cdt_get(dev_id, phy_id, &aquantia_port_cdt);
	SW_RTN_ON_ERROR(rv);
	switch (mdi_pair)
	{
		case 0:
			*cable_status =
				 _phy_cdt_status_mapping(CABLE_PAIR_A, aquantia_port_cdt.pair_a_status);
			/* Get Cable Length value */
			*cable_len = aquantia_port_cdt.pair_a_len;
			break;
		case 1:
			*cable_status =
				 _phy_cdt_status_mapping(CABLE_PAIR_B, aquantia_port_cdt.pair_b_status);
			/* Get Cable Length value */
			*cable_len = aquantia_port_cdt.pair_b_len;
			break;
		case 2:
			*cable_status =
				_phy_cdt_status_mapping(CABLE_PAIR_C, aquantia_port_cdt.pair_c_status);
			/* Get Cable Length value */
			*cable_len = aquantia_port_cdt.pair_c_len;
			break;
		case 3:
			*cable_status =
				 _phy_cdt_status_mapping(CABLE_PAIR_D, aquantia_port_cdt.pair_d_status);
			/* Get Cable Length value */
			*cable_len = aquantia_port_cdt.pair_d_len;
			break;
		default:
			break;
	}

	return rv;
}
#endif
/******************************************************************************
*
* AQUANTIA_autoneg_done
*
* AQUANTIA_autoneg_done
*/
a_bool_t aquantia_autoneg_done(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	a_uint16_t ii = 200;
	sw_error_t rv = SW_OK;

	do {
		rv =  aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
			AQUANTIA_AUTONEG_STANDARD_STATUS1, &phy_data);
		SW_RTN_ON_ERROR(rv);
		aos_mdelay(10);
	}
	while ((!AQUANTIA_AUTONEG_DONE(phy_data)) && --ii);

	if (ii == 0)
		return A_FALSE;

	return A_TRUE;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* aquantia_phy_get_ability - get the phy ability
*
*
*/
sw_error_t
aquantia_phy_get_partner_ability(a_uint32_t dev_id, a_uint32_t phy_id,
			       a_uint32_t * ability)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	*ability = 0;
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_LINK_PARTNER_ABILITY, &phy_data);
	SW_RTN_ON_ERROR(rv);
	if (phy_data & AQUANTIA_LINK_10BASETX_HALF_DUPLEX)
	{
		*ability |= FAL_PHY_PART_10T_HD;
	}
	if (phy_data & AQUANTIA_LINK_10BASETX_FULL_DUPLEX)
	{
		*ability |= FAL_PHY_PART_10T_FD;
	}
	if (phy_data & AQUANTIA_LINK_100BASETX_HALF_DUPLEX)
	{
		*ability |= FAL_PHY_PART_100TX_HD;
	}
	if (phy_data & AQUANTIA_LINK_100BASETX_FULL_DUPLEX)
	{
		*ability |= FAL_PHY_PART_100TX_FD;
	}

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_LINK_PARTNER_5G_ABILITY, &phy_data);
	SW_RTN_ON_ERROR(rv);
	if (phy_data & AQUANTIA_LINK_1000BASETX_FULL_DUPLEX)
	{
		*ability |= FAL_PHY_PART_1000T_FD;
	}
	if (phy_data & AQUANTIA_LINK_5000BASETX_FULL_DUPLEX)
	{
		*ability |= FAL_PHY_PART_5000T_FD;
	}
	if (phy_data & AQUANTIA_LINK_2500BASETX_FULL_DUPLEX)
	{
		*ability |= FAL_PHY_PART_2500T_FD;
	}

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_LINK_PARTNER_10G_ABILITY, &phy_data);
	if (phy_data & AQUANTIA_LINK_10000BASETX_FULL_DUPLEX)
	{
		*ability |= FAL_PHY_PART_10000T_FD;
	}

	return rv;
}
#endif
/******************************************************************************
*
* aquantia_phy_status - test to see if the specified phy link is alive
*
* RETURNS:
*    A_TRUE  --> link is alive
*    A_FALSE --> link is down
*/
a_bool_t aquantia_phy_get_link_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	/*in order to get the  link status of real time, need to read the link status two times */
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_STANDARD_STATUS1, &phy_data);
	if(rv != SW_OK)
	{
		return A_FALSE;
	}
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_STANDARD_STATUS1, &phy_data);
	if(rv != SW_OK)
	{
		return A_FALSE;
	}
	if (phy_data & AQUANTIA_STATUS_LINK)
	{
		return A_TRUE;
	}

	return A_FALSE;
}

/******************************************************************************
*
* AQUANTIA_set_autoneg_adv - set the phy autoneg Advertisement
*
*/
sw_error_t
aquantia_phy_set_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
			   a_uint32_t autoneg)
{
	a_uint16_t phy_data = 0, phy_data1 = 0 ;
	sw_error_t rv = SW_OK;

	if ((autoneg & FAL_PHY_ADV_10T_FD) ||(autoneg & FAL_PHY_ADV_10T_HD)||
		(autoneg & FAL_PHY_ADV_100TX_HD))
	{
		return SW_NOT_SUPPORTED;
	}

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_ADVERTISEMENT_REGISTER, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= ~AQUANTIA_ADVERTISE_MEGA_ALL;
	phy_data &=
	    ~(AQUANTIA_ADVERTISE_PAUSE | AQUANTIA_ADVERTISE_ASYM_PAUSE);
	if (autoneg & FAL_PHY_ADV_100TX_FD)
	{
		phy_data |= AQUANTIA_ADVERTISE_100FULL;
	}
	if (autoneg & FAL_PHY_ADV_PAUSE)
	{
		phy_data |= AQUANTIA_ADVERTISE_PAUSE;
	}
	if (autoneg & FAL_PHY_ADV_ASY_PAUSE)
	{
		phy_data |= AQUANTIA_ADVERTISE_ASYM_PAUSE;
	}
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_ADVERTISEMENT_REGISTER, phy_data);
	SW_RTN_ON_ERROR(rv);

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_VENDOR_PROVISION1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= ~AQUANTIA_ADVERTISE_GIGA_ALL;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_10GBASE_T_CONTROL_REGISTER, &phy_data1);
	SW_RTN_ON_ERROR(rv);
	phy_data1 &= ~AQUANTIA_ADVERTISE_GIGA_PLUS_ALL;
	if (autoneg & FAL_PHY_ADV_1000T_FD)
	{
		phy_data |= AQUANTIA_ADVERTISE_1000FULL;
	}
	if (autoneg & FAL_PHY_ADV_2500T_FD)
	{
		phy_data |= AQUANTIA_ADVERTISE_2500FULL;
		phy_data1 |= AQUANTIA_ADVERTISE_8023BZ_2500FULL;
	}
	if (autoneg & FAL_PHY_ADV_5000T_FD)
	{
		phy_data |= AQUANTIA_ADVERTISE_5000FULL;
		phy_data1 |= AQUANTIA_ADVERTISE_8023BZ_5000FULL;
	}
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_VENDOR_PROVISION1, phy_data);
	SW_RTN_ON_ERROR(rv);

	if (autoneg & FAL_PHY_ADV_10000T_FD)
		phy_data1 |= AQUANTIA_ADVERTISE_10000FULL;
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_10GBASE_T_CONTROL_REGISTER,phy_data1);

	return rv;
}

/******************************************************************************
*
* AQUANTIA_get_autoneg_adv - get the phy autoneg Advertisement
*
*/
sw_error_t
aquantia_phy_get_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
			   a_uint32_t * autoneg)
{
	a_uint16_t phy_data = 0, phy_data1 = 0;
	sw_error_t rv = SW_OK;

	*autoneg = 0;
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_ADVERTISEMENT_REGISTER, &phy_data);
	SW_RTN_ON_ERROR(rv);
	if (phy_data & AQUANTIA_ADVERTISE_100FULL)
	{
		*autoneg |= FAL_PHY_ADV_100TX_FD;
	}
	if (phy_data & AQUANTIA_ADVERTISE_PAUSE)
	{
		*autoneg |= FAL_PHY_ADV_PAUSE;
	}
	if (phy_data & AQUANTIA_ADVERTISE_ASYM_PAUSE)
	{
		*autoneg |= FAL_PHY_ADV_ASY_PAUSE;
	}

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_VENDOR_PROVISION1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_10GBASE_T_CONTROL_REGISTER, &phy_data1);
	SW_RTN_ON_ERROR(rv);
	if (phy_data & AQUANTIA_ADVERTISE_1000FULL)
	{
		*autoneg |= FAL_PHY_ADV_1000T_FD;
	}
	if ((phy_data & AQUANTIA_ADVERTISE_2500FULL) &&
		(phy_data1 & AQUANTIA_ADVERTISE_8023BZ_2500FULL))
	{
		*autoneg |= FAL_PHY_ADV_2500T_FD;
	}
	if ((phy_data & AQUANTIA_ADVERTISE_5000FULL) &&
		(phy_data1 & AQUANTIA_ADVERTISE_8023BZ_5000FULL))
	{
		*autoneg |= FAL_PHY_ADV_5000T_FD;
	}

	if (phy_data1 & AQUANTIA_ADVERTISE_10000FULL)
	{
		*autoneg |= FAL_PHY_ADV_10000T_FD;
	}

	return rv;
}

/******************************************************************************
*
* aquantia_phy_enable_autonego
*
*/
a_bool_t aquantia_phy_autoneg_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_STANDARD_CONTROL1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	if (phy_data & AQUANTIA_CTRL_AUTONEGOTIATION_ENABLE)
	{
		return A_TRUE;
	}

	return A_FALSE;
}
/******************************************************************************
*
* AQUANTIA_restart_autoneg - restart the phy autoneg
*
*/
sw_error_t aquantia_phy_restart_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PHY_XS_REGISTERS,
		AQUANTIA_PHY_XS_USX_TRANSMIT, &phy_data);
	SW_RTN_ON_ERROR(rv);
	if (!(phy_data & AQUANTIA_PHY_USX_AUTONEG_ENABLE))
	{
		rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_PHY_XS_REGISTERS,
			AQUANTIA_PHY_XS_USX_TRANSMIT,
			phy_data | AQUANTIA_PHY_USX_AUTONEG_ENABLE);
		SW_RTN_ON_ERROR(rv);
	}
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_STANDARD_CONTROL1, &phy_data);
	phy_data |= AQUANTIA_CTRL_AUTONEGOTIATION_ENABLE;
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_STANDARD_CONTROL1,
		phy_data | AQUANTIA_CTRL_RESTART_AUTONEGOTIATION);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_enable_autonego
*
*/
sw_error_t aquantia_phy_enable_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_STANDARD_CONTROL1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_STANDARD_CONTROL1,
		phy_data | AQUANTIA_CTRL_AUTONEGOTIATION_ENABLE);

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* aquantia_phy_set_802.3az
*
* set 802.3az status
*/
sw_error_t
aquantia_phy_set_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data = 0, phy_data1 = 0;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_ADVERTISTMENT_REGISTER, &phy_data);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_ADVERTISTMENT_REGISTER1, &phy_data1);
	SW_RTN_ON_ERROR(rv);
	if(enable == A_TRUE)
	{
		phy_data |= (AQUANTIA_EEE_ADV_10000M | AQUANTIA_EEE_ADV_1000M);
		phy_data1 |= (AQUANTIA_EEE_ADV_2500M | AQUANTIA_EEE_ADV_5000M);
	}
	else
	{
		phy_data &= ~(AQUANTIA_EEE_ADV_10000M | AQUANTIA_EEE_ADV_1000M);
		phy_data1 &= ~(AQUANTIA_EEE_ADV_2500M | AQUANTIA_EEE_ADV_5000M);
	}
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_ADVERTISTMENT_REGISTER, phy_data);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_ADVERTISTMENT_REGISTER1, phy_data1);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_restart_autoneg(dev_id, phy_id);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_get_8023az status
*
* get 8023az status
*/
sw_error_t
aquantia_phy_get_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	a_uint16_t phy_data = 0, phy_data1 = 0;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_ADVERTISTMENT_REGISTER, &phy_data);
	SW_RTN_ON_ERROR(rv);

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_ADVERTISTMENT_REGISTER1, &phy_data1);
	SW_RTN_ON_ERROR(rv);

	if((phy_data & (AQUANTIA_EEE_ADV_1000M | AQUANTIA_EEE_ADV_10000M)) &&
		(phy_data1 & (AQUANTIA_EEE_ADV_2500M | AQUANTIA_EEE_ADV_5000M)))
	{
		*enable = A_TRUE;
	}
	else
	{
		*enable = A_FALSE;
	}

	return rv;
}
#endif
/******************************************************************************
*
* aquantia_phy_set_speed - Determines the speed of phy ports associated with the
* specified device.
*/
static sw_error_t _aquantia_phy_set_100speed(a_uint32_t dev_id, a_uint32_t phy_id, fal_port_duplex_t duplex)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;
	/*set 100M */
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_ADVERTISEMENT_REGISTER, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= ~(AQUANTIA_ADVERTISE_MEGA_ALL);
	if(duplex == FAL_FULL_DUPLEX)
	{
		phy_data |= AQUANTIA_ADVERTISE_100FULL;
	}
	else
	{
		phy_data |= AQUANTIA_ADVERTISE_100HALF;
	}
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_ADVERTISEMENT_REGISTER, phy_data);
	SW_RTN_ON_ERROR(rv);
	/*disable 1000M, 2500M, 5000M speed*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_VENDOR_PROVISION1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= ~(AQUANTIA_ADVERTISE_GIGA_ALL);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_VENDOR_PROVISION1, phy_data);
	SW_RTN_ON_ERROR(rv);
	/*disable 10000M speed*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_10GBASE_T_CONTROL_REGISTER, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= ~(AQUANTIA_ADVERTISE_GIGA_PLUS_ALL);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_10GBASE_T_CONTROL_REGISTER, phy_data);

	return rv;
}

static sw_error_t _aquantia_phy_set_giga_speed(a_uint32_t dev_id, a_uint32_t phy_id, fal_port_speed_t speed)
{
	a_uint16_t phy_data = 0, phy_data1 = 0;
	sw_error_t rv = SW_OK;
	/*set 1000M and disable 2500M, 5000M */
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_VENDOR_PROVISION1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= ~(AQUANTIA_ADVERTISE_GIGA_ALL);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_10GBASE_T_CONTROL_REGISTER, &phy_data1);
	SW_RTN_ON_ERROR(rv);
	phy_data1 &= ~(AQUANTIA_ADVERTISE_GIGA_PLUS_ALL);
	switch(speed)
	{
		case FAL_SPEED_1000:
			phy_data |= AQUANTIA_ADVERTISE_1000FULL;
			break;
		case FAL_SPEED_2500:
			phy_data |= AQUANTIA_ADVERTISE_2500FULL;
			phy_data1 |= AQUANTIA_ADVERTISE_8023BZ_2500FULL;
			break;
		case FAL_SPEED_5000:
			phy_data |= AQUANTIA_ADVERTISE_5000FULL;
			phy_data1 |= AQUANTIA_ADVERTISE_8023BZ_5000FULL;
			break;
		default:
			return SW_NOT_SUPPORTED;
	}
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_VENDOR_PROVISION1, phy_data);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_10GBASE_T_CONTROL_REGISTER, phy_data1);
	SW_RTN_ON_ERROR(rv);

	/*disable 100M speed*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_ADVERTISEMENT_REGISTER, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= ~(AQUANTIA_ADVERTISE_MEGA_ALL);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_ADVERTISEMENT_REGISTER, phy_data);

	return rv;
}

static sw_error_t _aquantia_phy_set_10g_speed(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;
	/*set giga speed */
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_10GBASE_T_CONTROL_REGISTER,&phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= ~(AQUANTIA_ADVERTISE_GIGA_PLUS_ALL);
	phy_data |= AQUANTIA_ADVERTISE_10000FULL;
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_10GBASE_T_CONTROL_REGISTER, phy_data);
	SW_RTN_ON_ERROR(rv);

	/*disable 100M speed*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_ADVERTISEMENT_REGISTER, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= ~(AQUANTIA_ADVERTISE_MEGA_ALL);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_ADVERTISEMENT_REGISTER, phy_data);
	SW_RTN_ON_ERROR(rv);

	/*disable 1000M 2500M 5000M speed*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_VENDOR_PROVISION1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= ~(AQUANTIA_ADVERTISE_GIGA_ALL);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_VENDOR_PROVISION1, phy_data);

	return rv;
}

sw_error_t
aquantia_phy_set_speed(a_uint32_t dev_id, a_uint32_t phy_id,
		     fal_port_speed_t speed)
{
	fal_port_duplex_t old_duplex;
	sw_error_t rv;

	rv = aquantia_phy_get_duplex(dev_id, phy_id, &old_duplex);
	SW_RTN_ON_ERROR(rv);
	if (old_duplex == FAL_FULL_DUPLEX) {
		if (FAL_SPEED_100 == speed) {
			rv = _aquantia_phy_set_100speed(dev_id, phy_id, FAL_FULL_DUPLEX);
			SW_RTN_ON_ERROR(rv);
		} else if(FAL_SPEED_2500 == speed ||FAL_SPEED_5000 == speed || FAL_SPEED_1000 == speed){
			rv = _aquantia_phy_set_giga_speed(dev_id, phy_id, speed);
			SW_RTN_ON_ERROR(rv);
		} else if(FAL_SPEED_10000 == speed){
			rv = _aquantia_phy_set_10g_speed(dev_id, phy_id);
			SW_RTN_ON_ERROR(rv);
		} else {
			return SW_BAD_PARAM;
		}
	} else if (old_duplex == FAL_HALF_DUPLEX) {
		if (FAL_SPEED_100 == speed) {
			rv = _aquantia_phy_set_100speed(dev_id, phy_id, FAL_HALF_DUPLEX);
			SW_RTN_ON_ERROR(rv);
		} else {
			return SW_BAD_PARAM;
		}
	} else {
		return SW_FAIL;
	}
	rv = aquantia_phy_restart_autoneg(dev_id, phy_id);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_set_duplex - Determines the speed of phy ports associated with the
* specified device.
*/
sw_error_t
aquantia_phy_set_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
		      fal_port_duplex_t duplex)
{
	a_uint16_t phy_data = 0;
	fal_port_speed_t old_speed;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_get_speed(dev_id, phy_id, &old_speed);
	SW_RTN_ON_ERROR(rv);
	if (old_speed == FAL_SPEED_100){
		rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
			AQUANTIA_AUTONEG_ADVERTISEMENT_REGISTER, &phy_data);
		SW_RTN_ON_ERROR(rv);
		if (duplex == FAL_FULL_DUPLEX) {
			phy_data &= ~(AQUANTIA_ADVERTISE_MEGA_ALL);
			phy_data |= AQUANTIA_ADVERTISE_100FULL;
			 rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
				AQUANTIA_AUTONEG_ADVERTISEMENT_REGISTER, phy_data);
			 SW_RTN_ON_ERROR(rv);
		 } else {
		 	phy_data &= ~(AQUANTIA_ADVERTISE_MEGA_ALL);
			phy_data |= AQUANTIA_ADVERTISE_100HALF;
			rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
				AQUANTIA_AUTONEG_ADVERTISEMENT_REGISTER, phy_data);
			SW_RTN_ON_ERROR(rv);
		 }
	} else {
	 	return SW_NOT_SUPPORTED;
	}
	rv = aquantia_phy_restart_autoneg(dev_id, phy_id);

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* aquantia_phy_set wol enable or disable
*
* set phy wol enable or disable
*/
sw_error_t
aquantia_phy_set_wol_status(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data0, phy_data1, phy_data2;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_RESERVED_VENDOR_PROVISIONING1, &phy_data0);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GBE_STANDARD_REGISTERS,
		AQUANTIA_MAGIC_ENGINE_REGISTER1, &phy_data1);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GBE_STANDARD_REGISTERS,
		AQUANTIA_MAGIC_ENGINE_REGISTER2, &phy_data2);

	if (enable == A_TRUE)
	{
		phy_data0 |= AQUANTIA_PHY_WOL_ENABLE;
		phy_data1 |= AQUANTIA_MAGIC_PACKETS_ENABLE;
		phy_data2 |= AQUANTIA_MAGIC_PACKETS_ENABLE;
	}
	else
	{
		phy_data0 &= ~AQUANTIA_PHY_WOL_ENABLE;
		phy_data1 &= ~AQUANTIA_MAGIC_PACKETS_ENABLE;
		phy_data2 &= ~AQUANTIA_MAGIC_PACKETS_ENABLE;
	}
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_RESERVED_VENDOR_PROVISIONING1, phy_data0);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_GBE_STANDARD_REGISTERS,
		AQUANTIA_MAGIC_ENGINE_REGISTER1, phy_data1);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_GBE_STANDARD_REGISTERS,
		AQUANTIA_MAGIC_ENGINE_REGISTER2, phy_data2);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_get_wol status
*
* get wol status
*/
sw_error_t
aquantia_phy_get_wol_status(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	*enable = A_FALSE;
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_RESERVED_VENDOR_PROVISIONING1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	if (phy_data & AQUANTIA_PHY_WOL_ENABLE)
	{
		*enable = A_TRUE;
	}

	return rv;
}

/******************************************************************************
*
* aquantia_phy_set wol frame mac address
*
* set phy wol frame mac address
*/
sw_error_t
aquantia_phy_set_magic_frame_mac(a_uint32_t dev_id, a_uint32_t phy_id,
			       fal_mac_addr_t * mac)
{
	a_uint16_t phy_data1;
	a_uint16_t phy_data2;
	a_uint16_t phy_data3;
	sw_error_t rv = SW_OK;

	phy_data1 = (mac->uc[1] << 8) | mac->uc[0];
	phy_data2 = (mac->uc[3] << 8) | mac->uc[2];
	phy_data3 = (mac->uc[5] << 8) | mac->uc[4];
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_GBE_STANDARD_REGISTERS,
		AQUANTIA_MAGIC_FRAME_MAC0, phy_data1);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_GBE_STANDARD_REGISTERS,
		AQUANTIA_MAGIC_FRAME_MAC1, phy_data2);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_GBE_STANDARD_REGISTERS,
		AQUANTIA_MAGIC_FRAME_MAC2, phy_data3);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_get wol frame mac address
*
* get phy wol frame mac address
*/
sw_error_t
aquantia_phy_get_magic_frame_mac(a_uint32_t dev_id, a_uint32_t phy_id,
			       fal_mac_addr_t * mac)
{
	a_uint16_t phy_data1;
	a_uint16_t phy_data2;
	a_uint16_t phy_data3;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GBE_STANDARD_REGISTERS,
		AQUANTIA_MAGIC_FRAME_MAC0, &phy_data1);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GBE_STANDARD_REGISTERS,
		AQUANTIA_MAGIC_FRAME_MAC1, &phy_data2);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GBE_STANDARD_REGISTERS,
		AQUANTIA_MAGIC_FRAME_MAC2, &phy_data3);
	SW_RTN_ON_ERROR(rv);
	mac->uc[0] = (phy_data1 & BITS(0, 8));
	mac->uc[1] = (phy_data1 >> 8) & BITS(0, 8);
	mac->uc[2] = (phy_data2 & BITS(0, 8));
	mac->uc[3] = (phy_data2 >> 8) & BITS(0, 8);
	mac->uc[4] = (phy_data3 & BITS(0, 8));
	mac->uc[5] = (phy_data3 >> 8) & BITS(0, 8);

	return rv;
}
#endif
sw_error_t
aquantia_phy_interface_set_mode(a_uint32_t dev_id, a_uint32_t phy_id, fal_port_interface_mode_t interface_mode)
{
	a_uint16_t phy_data;
	a_uint32_t phy_register;
	fal_port_speed_t speed;
	sw_error_t rv =SW_OK;

	rv = aquantia_phy_get_speed(dev_id, phy_id, &speed);
	SW_RTN_ON_ERROR(rv);
	switch (speed)
	{
		case FAL_SPEED_100:
			phy_register = AQUANTIA_GLOBAL_SYS_CONFIG_FOR_100M;
			break;
		case FAL_SPEED_1000:
			phy_register = AQUANTIA_GLOBAL_SYS_CONFIG_FOR_1000M;
			break;
		case FAL_SPEED_2500:
			phy_register = AQUANTIA_GLOBAL_SYS_CONFIG_FOR_2500M;
			break;
		case FAL_SPEED_5000:
			phy_register = AQUANTIA_GLOBAL_SYS_CONFIG_FOR_5000M;
			break;
		case FAL_SPEED_10000:
			phy_register = AQUANTIA_GLOBAL_SYS_CONFIG_FOR_10000M;
			break;
		default:
			return SW_NOT_SUPPORTED;
	}
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
					phy_register, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= ~(BITS(0, 3));
	switch(interface_mode)
	{
		case PHY_SGMII_BASET:
			if(speed == FAL_SPEED_100 || speed == FAL_SPEED_1000)
			{
				phy_data |= AQUANTIA_SERDES_MODE_SGMII;
			}
			else
			{
				return SW_NOT_SUPPORTED;
			}
			break;
		case PORT_USXGMII:
			phy_data |= AQUANTIA_SERDES_MODE_XFI;
			break;
		case PORT_SGMII_PLUS:
			if(speed == FAL_SPEED_2500)
			{
				phy_data |= AQUANTIA_SERDES_MODE_OCSGMII;
			}
			else
			{
				return SW_NOT_SUPPORTED;
			}
			break;
		default:
			return SW_NOT_SUPPORTED;
	}
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		phy_register, phy_data);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_interface mode status get
*
* get aquantia phy interface mode status
*/
sw_error_t
aquantia_phy_interface_get_mode_status(a_uint32_t dev_id, a_uint32_t phy_id,
		fal_port_interface_mode_t *interface_mode_status)
{
	a_uint16_t phy_data;
	a_uint32_t phy_register;
	fal_port_speed_t speed;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_get_speed(dev_id, phy_id, &speed);
	SW_RTN_ON_ERROR(rv);
	switch (speed)
	{
		case FAL_SPEED_100:
			phy_register = AQUANTIA_GLOBAL_SYS_CONFIG_FOR_100M;
			break;
		case FAL_SPEED_1000:
			phy_register = AQUANTIA_GLOBAL_SYS_CONFIG_FOR_1000M;
			break;
		case FAL_SPEED_2500:
			phy_register = AQUANTIA_GLOBAL_SYS_CONFIG_FOR_2500M;
			break;
		case FAL_SPEED_5000:
			phy_register = AQUANTIA_GLOBAL_SYS_CONFIG_FOR_5000M;
			break;
		case FAL_SPEED_10000:
			phy_register = AQUANTIA_GLOBAL_SYS_CONFIG_FOR_10000M;
			break;
		default:
			return SW_NOT_SUPPORTED;
	}
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		phy_register, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= (BITS(0, 3));
	switch(phy_data)
	{
		case AQUANTIA_SERDES_MODE_SGMII:
			*interface_mode_status = PHY_SGMII_BASET;
			break;
		case AQUANTIA_SERDES_MODE_XFI:
			*interface_mode_status = PORT_USXGMII;
			break;
		case AQUANTIA_SERDES_MODE_OCSGMII:
			*interface_mode_status = PORT_SGMII_PLUS;
			break;
		default:
			return SW_NOT_SUPPORTED;
	}

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* aquantia_phy_intr_mask_set - Set interrupt mask with the
* specified device.
*/
sw_error_t
aquantia_phy_intr_mask_set(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_uint32_t intr_mask_flag)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	if ((FAL_PHY_INTR_STATUS_DOWN_CHANGE |FAL_PHY_INTR_STATUS_UP_CHANGE)
		& intr_mask_flag)
	{
		rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
			AQUANTIA_AUTONEG_TRANSMIT_VENDOR_INTR_MASK, &phy_data);
		SW_RTN_ON_ERROR(rv);
		phy_data |= AQUANTIA_INTR_LINK_STATUS_CHANGE;
		rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
			AQUANTIA_AUTONEG_TRANSMIT_VENDOR_INTR_MASK, phy_data);
		SW_RTN_ON_ERROR(rv);

		rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
			AQUANTIA_GLOBAL_INTR_VENDOR_MASK, &phy_data);
		SW_RTN_ON_ERROR(rv);
		phy_data |= AQUANTIA_AUTO_AND_ALARMS_INTR_MASK;
		rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
			AQUANTIA_GLOBAL_INTR_VENDOR_MASK, phy_data);
		SW_RTN_ON_ERROR(rv);

		rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
			AQUANTIA_GLOBAL_INTR_STANDARD_MASK, &phy_data);
		SW_RTN_ON_ERROR(rv);
		phy_data |= AQUANTIA_ALL_VENDOR_ALARMS_INTR_MASK;
		rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
			AQUANTIA_GLOBAL_INTR_STANDARD_MASK, phy_data);
	}

	return rv;
}

/******************************************************************************
*
* aquantia_phy_intr_mask_get - Get interrupt mask with the
* specified device.
*/
sw_error_t
aquantia_phy_intr_mask_get(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_uint32_t * intr_mask_flag)
{
	a_uint16_t phy_data1 = 0, phy_data2 = 0, phy_data3 = 0;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_AUTONEG_TRANSMIT_VENDOR_INTR_MASK, &phy_data1);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_GLOBAL_INTR_VENDOR_MASK, &phy_data2);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_GLOBAL_INTR_STANDARD_MASK, &phy_data3);
	if ((AQUANTIA_INTR_LINK_STATUS_CHANGE & phy_data1) &&
		(AQUANTIA_AUTO_AND_ALARMS_INTR_MASK & phy_data2) &&
		(AQUANTIA_ALL_VENDOR_ALARMS_INTR_MASK & phy_data3))
	{
		*intr_mask_flag = FAL_PHY_INTR_STATUS_DOWN_CHANGE |
			FAL_PHY_INTR_STATUS_UP_CHANGE;
	}

	return rv;
}
#endif
/******************************************************************************
*
* aquantia_phy_off - power off the phy
*
* Power off the phy
*/
sw_error_t aquantia_phy_poweroff(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	sw_error_t rv;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_GLOBAL_STANDARD_CONTROL1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data |= AQUANTIA_POWER_DOWN;
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_GLOBAL_STANDARD_CONTROL1,phy_data);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_on - power on the phy
*
* Power on the phy
*/
sw_error_t aquantia_phy_poweron(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	sw_error_t rv;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_GLOBAL_STANDARD_CONTROL1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	phy_data &= ~AQUANTIA_POWER_DOWN;
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_GLOBAL_REGISTERS,
		AQUANTIA_GLOBAL_STANDARD_CONTROL1,phy_data);

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
static sw_error_t
_aquantia_phy_line_side_counter_get(a_uint32_t dev_id, a_uint32_t phy_id,
			 fal_port_counter_info_t * counter_infor)
{
	a_uint16_t msw_counter;
	a_uint16_t lsw_counter;
	sw_error_t rv = SW_OK;

	/*get line side tx good packets*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_LINE_SIDE_TRANSMIT_GOOD_FRAME_COUNTER2, &msw_counter);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_LINE_SIDE_TRANSMIT_GOOD_FRAME_COUNTER1, &lsw_counter);
	SW_RTN_ON_ERROR(rv);
	counter_infor->TxGoodFrame = (msw_counter << 16) | lsw_counter;

	/*get line side tx bad packets*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_LINE_SIDE_TRANSMIT_ERROR_FRAME_COUNTER2, &msw_counter);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_LINE_SIDE_TRANSMIT_ERROR_FRAME_COUNTER1, &lsw_counter);
	SW_RTN_ON_ERROR(rv);
	counter_infor->TxBadCRC = (msw_counter << 16) | lsw_counter;

	/*get line side rx good packets*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_LINE_SIDE_RECEIVE_GOOD_FRAME_COUNTER2, &msw_counter);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_LINE_SIDE_RECEIVE_GOOD_FRAME_COUNTER1, &lsw_counter);
	SW_RTN_ON_ERROR(rv);
	counter_infor->RxGoodFrame = (msw_counter << 16) | lsw_counter;

	/*get line side rx bad packets*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_LINE_SIDE_RECEIVE_ERROR_FRAME_COUNTER2, &msw_counter);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_LINE_SIDE_RECEIVE_ERROR_FRAME_COUNTER1, &lsw_counter);
	SW_RTN_ON_ERROR(rv);
	counter_infor->RxBadCRC = (msw_counter << 16) | lsw_counter;

	return rv;
}

static sw_error_t
_aquantia_phy_system_side_counter_get(a_uint32_t dev_id, a_uint32_t phy_id,
			 fal_port_counter_info_t * counter_infor)
{
	a_uint16_t msw_counter;
	a_uint16_t lsw_counter;
	sw_error_t rv = SW_OK;

	/*get system tx good packets*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_SYSTEM_SIDE_TRANSMIT_GOOD_FRAME_COUNTER2, &msw_counter);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_SYSTEM_SIDE_TRANSMIT_GOOD_FRAME_COUNTER1, &lsw_counter);
	SW_RTN_ON_ERROR(rv);
	counter_infor->SysTxGoodFrame = (msw_counter << 16) | lsw_counter;

	/*get system tx bad packets*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_SYSTEM_SIDE_TRANSMIT_ERROR_FRAME_COUNTER2, &msw_counter);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_SYSTEM_SIDE_TRANSMIT_ERROR_FRAME_COUNTER1, &lsw_counter);
	SW_RTN_ON_ERROR(rv);
	counter_infor->SysTxBadCRC = (msw_counter << 16) | lsw_counter;

	/*get system rx good packets*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_SYSTEM_SIDE_RECEIVE_GOOD_FRAME_COUNTER2, &msw_counter);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_SYSTEM_SIDE_RECEIVE_GOOD_FRAME_COUNTER1, &lsw_counter);
	SW_RTN_ON_ERROR(rv);
	counter_infor->SysRxGoodFrame = (msw_counter << 16) | lsw_counter;

	/*get system rx bad packets*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_SYSTEM_SIDE_RECEIVE_ERROR_FRAME_COUNTER2, &msw_counter);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_SYSTEM_SIDE_RECEIVE_ERROR_FRAME_COUNTER1, &lsw_counter);
	SW_RTN_ON_ERROR(rv);
	counter_infor->SysRxBadCRC = (msw_counter << 16) | lsw_counter;

	return rv;
}

/******************************************************************************
*
* aquantia_phy_show show counter statistics
*
* show counter statistics
*/
sw_error_t
aquantia_phy_show_counter(a_uint32_t dev_id, a_uint32_t phy_id,
			 fal_port_counter_info_t * counter_infor)
{
	sw_error_t rv = SW_OK;
	fal_port_speed_t speed;

	rv = aquantia_phy_get_speed(dev_id, phy_id, &speed);
	SW_RTN_ON_ERROR(rv);
	if(speed == FAL_SPEED_2500 || speed == FAL_SPEED_5000 || speed == FAL_SPEED_10000)
	{
		rv = _aquantia_phy_line_side_counter_get(dev_id, phy_id, counter_infor);
		SW_RTN_ON_ERROR(rv);
	}
	rv = _aquantia_phy_system_side_counter_get(dev_id, phy_id, counter_infor);

	return rv;
}
#endif
/******************************************************************************
*
* aquantia_phy_get_status
*
* get phy status
*/
sw_error_t
aquantia_phy_get_status(a_uint32_t dev_id, a_uint32_t phy_id,
		struct port_phy_status *phy_status)
{
	sw_error_t rv = SW_OK;
	a_uint16_t phy_data;

	/*get phy link status*/
	phy_status->link_status = aquantia_phy_get_link_status(dev_id, phy_id);
	if(phy_status->link_status != A_TRUE)
	{
		return SW_OK;
	}
	/*get phy speed and duplex*/
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_REG_AUTONEG_VENDOR_STATUS, &phy_data);
	SW_RTN_ON_ERROR(rv);
	switch ((phy_data & AQUANTIA_STATUS_SPEED_MASK) >>1)
	{
		case AQUANTIA_STATUS_SPEED_100MBS:
			phy_status->speed = FAL_SPEED_100;
			break;
		case AQUANTIA_STATUS_SPEED_1000MBS:
			phy_status->speed = FAL_SPEED_1000;
			break;
		case AQUANTIA_STATUS_SPEED_2500MBS:
			phy_status->speed = FAL_SPEED_2500;
			break;
		case AQUANTIA_STATUS_SPEED_5000MBS:
			phy_status->speed = FAL_SPEED_5000;
			break;
		case AQUANTIA_STATUS_SPEED_10000MBS:
			phy_status->speed = FAL_SPEED_10000;
			break;
		default:
			return SW_READ_ERROR;
	}
	if (phy_data & AQUANTIA_STATUS_FULL_DUPLEX)
	{
		phy_status->duplex = FAL_FULL_DUPLEX;
	}
	else
	{
		phy_status->duplex = FAL_HALF_DUPLEX;
	}
	/* get phy tx flowctrl and rx flowctrl resolution status */
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_RESERVED_VENDOR_STATUS1, &phy_data);
	SW_RTN_ON_ERROR(rv);
	if(phy_data & AQUANTIA_PHY_TX_FLOWCTRL_STATUS)
	{
		phy_status->tx_flowctrl = A_TRUE;
	}
	else
	{
		phy_status->tx_flowctrl = A_FALSE;
	}
	if(phy_data & AQUANTIA_PHY_RX_FLOWCTRL_STATUS)
	{
		phy_status->rx_flowctrl = A_TRUE;
	}
	else
	{
		phy_status->rx_flowctrl = A_FALSE;
	}

	return rv;
}

/******************************************************************************
*
* aquantia_phy_set_eee_advertisement
*
* set eee advertisement
*/
sw_error_t
aquantia_phy_set_eee_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t adv)
{
	a_uint16_t phy_data = 0, phy_data1 = 0;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_ADVERTISTMENT_REGISTER, &phy_data);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_ADVERTISTMENT_REGISTER1, &phy_data1);
	SW_RTN_ON_ERROR(rv);

	phy_data &= ~(AQUANTIA_EEE_ADV_1000M | AQUANTIA_EEE_ADV_10000M);
	if (adv & FAL_PHY_EEE_1000BASE_T) {
		phy_data |= AQUANTIA_EEE_ADV_1000M;
	}
	if (adv & FAL_PHY_EEE_10000BASE_T) {
		phy_data |= AQUANTIA_EEE_ADV_10000M;
	}

	phy_data1 &= ~(AQUANTIA_EEE_ADV_2500M | AQUANTIA_EEE_ADV_5000M);
	if (adv & FAL_PHY_EEE_2500BASE_T) {
		phy_data1 |= AQUANTIA_EEE_ADV_2500M;
	}
	if (adv & FAL_PHY_EEE_5000BASE_T) {
		phy_data1 |= AQUANTIA_EEE_ADV_5000M;
	}

	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_ADVERTISTMENT_REGISTER, phy_data);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_reg_write(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_ADVERTISTMENT_REGISTER1, phy_data1);
	SW_RTN_ON_ERROR(rv);
	rv = aquantia_phy_restart_autoneg(dev_id, phy_id);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_get_eee_advertisement
*
* get eee advertisement
*/
sw_error_t
aquantia_phy_get_eee_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *adv)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	*adv = 0;
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_ADVERTISTMENT_REGISTER, &phy_data);
	SW_RTN_ON_ERROR(rv);

	if (phy_data & AQUANTIA_EEE_ADV_1000M) {
		*adv |= FAL_PHY_EEE_1000BASE_T;
	}
	if (phy_data & AQUANTIA_EEE_ADV_10000M){
		*adv |= FAL_PHY_EEE_10000BASE_T;
	}
	phy_data = 0;
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_ADVERTISTMENT_REGISTER1, &phy_data);
	SW_RTN_ON_ERROR(rv);

	if (phy_data & AQUANTIA_EEE_ADV_2500M) {
		*adv |= FAL_PHY_EEE_2500BASE_T;
	}
	if (phy_data & AQUANTIA_EEE_ADV_5000M) {
		*adv |= FAL_PHY_EEE_5000BASE_T;
	}

	return rv;
}
/******************************************************************************
*
* aquantia_phy_get_eee_partner_advertisement
*
* get eee partner advertisement
*/
sw_error_t
aquantia_phy_get_eee_partner_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *adv)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	*adv = 0;
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_PARTNER_ADVERTISTMENT_REGISTER, &phy_data);
	SW_RTN_ON_ERROR(rv);

	if (phy_data & AQUANTIA_EEE_PARTNER_ADV_1000M) {
		*adv |= FAL_PHY_EEE_1000BASE_T;
	}
	if (phy_data & AQUANTIA_EEE_PARTNER_ADV_10000M){
		*adv |= FAL_PHY_EEE_10000BASE_T;
	}
	phy_data = 0;
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_AUTONEG,
		AQUANTIA_EEE_PARTNER_ADVERTISTMENT_REGISTER1, &phy_data);
	SW_RTN_ON_ERROR(rv);

	if (phy_data & AQUANTIA_EEE_PARTNER_ADV_2500M) {
		*adv |= FAL_PHY_EEE_2500BASE_T;
	}
	if (phy_data & AQUANTIA_EEE_PARTNER_ADV_5000M) {
		*adv |= FAL_PHY_EEE_5000BASE_T;
	}

	return rv;
}
/******************************************************************************
*
* aquantia_phy_get_eee_capability
*
* get eee capability
*/
sw_error_t
aquantia_phy_get_eee_cap(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *cap)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	*cap = 0;
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_EEE_CAPABILITY_REGISTER, &phy_data);
	SW_RTN_ON_ERROR(rv);

	if (phy_data & AQUANTIA_EEE_CAPABILITY_1000M) {
		*cap |= FAL_PHY_EEE_1000BASE_T;
	}
	if (phy_data & AQUANTIA_EEE_CAPABILITY_10000M){
		*cap |= FAL_PHY_EEE_10000BASE_T;
	}
	phy_data = 0;
	rv = aquantia_phy_reg_read(dev_id, phy_id, AQUANTIA_MMD_PCS_REGISTERS,
		AQUANTIA_EEE_CAPABILITY_REGISTER1, &phy_data);
	SW_RTN_ON_ERROR(rv);

	if (phy_data & AQUANTIA_EEE_CAPABILITY_2500M) {
		*cap |= FAL_PHY_EEE_2500BASE_T;
	}
	if (phy_data & AQUANTIA_EEE_CAPABILITY_5000M) {
		*cap |= FAL_PHY_EEE_5000BASE_T;
	}

	return rv;
}
/******************************************************************************
*
* aquantia_phy_get_eee_status
*
* get eee status
*/
sw_error_t
aquantia_phy_get_eee_status(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *status)
{
	a_uint32_t adv = 0, lp_adv = 0;
	sw_error_t rv = SW_OK;

	rv = aquantia_phy_get_eee_adv(dev_id, phy_id, &adv);
	SW_RTN_ON_ERROR(rv);

	rv = aquantia_phy_get_eee_partner_adv(dev_id, phy_id, &lp_adv);
	SW_RTN_ON_ERROR(rv);

	*status = (adv & lp_adv);

	return rv;
}

/******************************************************************************
*
* aquantia_phy_hw_register init to avoid packet loss
*
*/
sw_error_t
aquantia_phy_hw_init(a_uint32_t dev_id,  a_uint32_t port_bmp)
{
	a_uint16_t phy_data = 0;
	a_uint32_t port_id = 0, phy_addr = 0;
	sw_error_t rv = SW_OK;

	for (port_id = 0; port_id < SW_MAX_NR_PORT; port_id ++)
	{
		if (port_bmp & (0x1 << port_id))
		{
			phy_addr = qca_ssdk_port_to_phy_addr(dev_id, port_id);
			/*set auto neg of aq*/
			rv = aquantia_phy_reg_read(dev_id, phy_addr, AQUANTIA_MMD_PHY_XS_REGISTERS,
				AQUANTIA_PHY_XS_USX_TRANSMIT, &phy_data);
			SW_RTN_ON_ERROR(rv);
			phy_data |= AQUANTIA_PHY_USX_AUTONEG_ENABLE;
			rv = aquantia_phy_reg_write(dev_id, phy_addr, AQUANTIA_MMD_PHY_XS_REGISTERS,
				AQUANTIA_PHY_XS_USX_TRANSMIT,phy_data);
			SW_RTN_ON_ERROR(rv);
			/*config interrupt of aq*/
			rv = aquantia_phy_reg_read(dev_id, phy_addr, AQUANTIA_MMD_AUTONEG,
				AQUANTIA_AUTONEG_TRANSMIT_VENDOR_INTR_MASK, &phy_data);
			SW_RTN_ON_ERROR(rv);
			phy_data |= AQUANTIA_INTR_LINK_STATUS_CHANGE;
			rv = aquantia_phy_reg_write(dev_id, phy_addr, AQUANTIA_MMD_AUTONEG,
				AQUANTIA_AUTONEG_TRANSMIT_VENDOR_INTR_MASK, phy_data);
			SW_RTN_ON_ERROR(rv);
			rv = aquantia_phy_reg_read(dev_id, phy_addr, AQUANTIA_MMD_GLOBAL_REGISTERS,
				AQUANTIA_GLOBAL_INTR_STANDARD_MASK, &phy_data);
			SW_RTN_ON_ERROR(rv);
			phy_data |= AQUANTIA_ALL_VENDOR_ALARMS_INTR_MASK;
			rv = aquantia_phy_reg_write(dev_id, phy_addr, AQUANTIA_MMD_GLOBAL_REGISTERS,
				AQUANTIA_GLOBAL_INTR_STANDARD_MASK, phy_data);
			SW_RTN_ON_ERROR(rv);
			rv = aquantia_phy_reg_read(dev_id, phy_addr, AQUANTIA_MMD_GLOBAL_REGISTERS,
				AQUANTIA_GLOBAL_INTR_VENDOR_MASK, &phy_data);
			SW_RTN_ON_ERROR(rv);
			phy_data |= AQUANTIA_AUTO_AND_ALARMS_INTR_MASK;
			rv = aquantia_phy_reg_write(dev_id, phy_addr, AQUANTIA_MMD_GLOBAL_REGISTERS,
				AQUANTIA_GLOBAL_INTR_VENDOR_MASK, phy_data);
			SW_RTN_ON_ERROR(rv);

			/* config aq phy ACT and LINK led behavior*/
			phy_data = AQUANTIA_ACT_LED_VALUE;
			rv = aquantia_phy_reg_write(dev_id, phy_addr, AQUANTIA_MMD_GLOBAL_REGISTERS,
				AQUANTIA_ACT_LED_STATUS, phy_data);
			SW_RTN_ON_ERROR(rv);
			phy_data = AQUANTIA_LINK_LED_VALUE;
			rv = aquantia_phy_reg_write(dev_id, phy_addr, AQUANTIA_MMD_GLOBAL_REGISTERS,
				AQUANTIA_LINK_LED_STATUS, phy_data);
			SW_RTN_ON_ERROR(rv);
			/*add all ability of aq phy*/
			rv = aquantia_phy_set_autoneg_adv(dev_id, phy_addr,
				FAL_PHY_ADV_XGE_SPEED_ALL | FAL_PHY_ADV_100TX_FD |
				FAL_PHY_ADV_1000T_FD);
			SW_RTN_ON_ERROR(rv);
#if 0
			rv = aquantia_phy_set_eee_adv(dev_id, phy_addr, FAL_PHY_EEE_1000BASE_T
				| FAL_PHY_EEE_2500BASE_T | FAL_PHY_EEE_5000BASE_T |
				FAL_PHY_EEE_10000BASE_T);
			SW_RTN_ON_ERROR(rv);
#endif
		}
	}

	return rv;
}

static int aquantia_phy_api_ops_init(void)
{
	int ret;
	hsl_phy_ops_t *aquantia_phy_api_ops = NULL;

	aquantia_phy_api_ops = kzalloc(sizeof(hsl_phy_ops_t), GFP_KERNEL);
	if (aquantia_phy_api_ops == NULL) {
		SSDK_ERROR("aquantia phy ops kzalloc failed!\n");
		return -ENOMEM;
	}

	phy_api_ops_init(AQUANTIA_PHY_CHIP);

	aquantia_phy_api_ops->phy_speed_get = aquantia_phy_get_speed;
	aquantia_phy_api_ops->phy_speed_set = aquantia_phy_set_speed;
	aquantia_phy_api_ops->phy_duplex_get = aquantia_phy_get_duplex;
	aquantia_phy_api_ops->phy_duplex_set = aquantia_phy_set_duplex;
	aquantia_phy_api_ops->phy_autoneg_enable_set = aquantia_phy_enable_autoneg;
	aquantia_phy_api_ops->phy_restart_autoneg = aquantia_phy_restart_autoneg;
	aquantia_phy_api_ops->phy_autoneg_status_get = aquantia_phy_autoneg_status;
	aquantia_phy_api_ops->phy_autoneg_adv_set = aquantia_phy_set_autoneg_adv;
	aquantia_phy_api_ops->phy_autoneg_adv_get = aquantia_phy_get_autoneg_adv;
#ifndef IN_PORTCONTROL_MINI
	aquantia_phy_api_ops->phy_powersave_set = aquantia_phy_set_powersave;
	aquantia_phy_api_ops->phy_powersave_get = aquantia_phy_get_powersave;
	aquantia_phy_api_ops->phy_8023az_set = aquantia_phy_set_8023az;
	aquantia_phy_api_ops->phy_8023az_get = aquantia_phy_get_8023az;
#endif
	aquantia_phy_api_ops->phy_power_on = aquantia_phy_poweron;
	aquantia_phy_api_ops->phy_power_off = aquantia_phy_poweroff;
#ifndef IN_PORTCONTROL_MINI
	aquantia_phy_api_ops->phy_cdt = aquantia_phy_cdt;
#endif
	aquantia_phy_api_ops->phy_link_status_get = aquantia_phy_get_link_status;
#ifndef IN_PORTCONTROL_MINI
	aquantia_phy_api_ops->phy_mdix_set = aquantia_phy_set_mdix;
	aquantia_phy_api_ops->phy_mdix_get = aquantia_phy_get_mdix;
	aquantia_phy_api_ops->phy_mdix_status_get = aquantia_phy_get_mdix_status;
	aquantia_phy_api_ops->phy_local_loopback_set = aquantia_phy_set_local_loopback;
	aquantia_phy_api_ops->phy_local_loopback_get = aquantia_phy_get_local_loopback;
	aquantia_phy_api_ops->phy_remote_loopback_set = aquantia_phy_set_remote_loopback;
	aquantia_phy_api_ops->phy_remote_loopback_get = aquantia_phy_get_remote_loopback;
	aquantia_phy_api_ops->phy_reset = aquantia_phy_reset;
	aquantia_phy_api_ops->phy_wol_status_set = aquantia_phy_set_wol_status;
	aquantia_phy_api_ops->phy_wol_status_get = aquantia_phy_get_wol_status;
	aquantia_phy_api_ops->phy_magic_frame_mac_get = aquantia_phy_get_magic_frame_mac;
	aquantia_phy_api_ops->phy_magic_frame_mac_set = aquantia_phy_set_magic_frame_mac;
	aquantia_phy_api_ops->phy_intr_mask_set = aquantia_phy_intr_mask_set;
	aquantia_phy_api_ops->phy_intr_mask_get = aquantia_phy_intr_mask_get;
	aquantia_phy_api_ops->phy_id_get = aquantia_phy_get_phy_id;
#endif
	aquantia_phy_api_ops->phy_interface_mode_set = aquantia_phy_interface_set_mode;
	aquantia_phy_api_ops->phy_interface_mode_status_get=aquantia_phy_interface_get_mode_status;
	aquantia_phy_api_ops->phy_get_status = aquantia_phy_get_status;
#ifndef IN_PORTCONTROL_MINI
	aquantia_phy_api_ops->phy_counter_show = aquantia_phy_show_counter;
#endif
	aquantia_phy_api_ops->phy_eee_adv_set = aquantia_phy_set_eee_adv;
	aquantia_phy_api_ops->phy_eee_adv_get = aquantia_phy_get_eee_adv;
	aquantia_phy_api_ops->phy_eee_partner_adv_get = aquantia_phy_get_eee_partner_adv;
	aquantia_phy_api_ops->phy_eee_cap_get = aquantia_phy_get_eee_cap;
	aquantia_phy_api_ops->phy_eee_status_get = aquantia_phy_get_eee_status;
	ret = hsl_phy_api_ops_register(AQUANTIA_PHY_CHIP, aquantia_phy_api_ops);
	if (ret == 0)
		SSDK_INFO("qca probe aquantia phy driver succeeded!\n");
	else
		SSDK_ERROR("qca probe aquantia phy driver failed! (code: %d)\n", ret);

	return ret;
}

/******************************************************************************
*
* aquantia_phy_init -
*
*/
int aquantia_phy_init(a_uint32_t dev_id, a_uint32_t port_bmp)
{
	static a_uint32_t phy_ops_flag = 0;

	if(phy_ops_flag == 0) {
		aquantia_phy_api_ops_init();
		phy_ops_flag = 1;
	}
	aquantia_phy_hw_init(dev_id, port_bmp);

	return 0;

}

