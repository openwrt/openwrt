/*
 * Copyright (c) 2015-2019, The Linux Foundation. All rights reserved.
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
#include "malibu_phy.h"
#include "hsl_phy.h"
#include "ssdk_plat.h"

static a_uint32_t first_phy_addr = MAX_PHY_ADDR;

static a_uint16_t
_phy_reg_read(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint32_t reg)
{
	sw_error_t rv;
	a_uint16_t phy_data = 0;

	HSL_PHY_GET(rv, dev_id, phy_addr, reg, &phy_data);
	if (SW_OK != rv)
		return 0xFFFF;

	return phy_data;

}

static sw_error_t
_phy_reg_write(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint32_t reg,
	       a_uint16_t phy_data)
{
	sw_error_t rv;

	HSL_PHY_SET(rv, dev_id, phy_addr, reg, phy_data);

	return rv;
}

/* #define malibu_phy_reg_read _phy_reg_read  */
/* #define malibu_phy_reg_write _phy_reg_write */

/******************************************************************************
*
* malibu_phy_mii_read - mii register read
*
* mii register read
*/
a_uint16_t
malibu_phy_reg_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id)
{
 	return _phy_reg_read(dev_id, phy_id, reg_id);

}

/******************************************************************************
*
* malibu_phy_mii_write - mii register write
*
* mii register write
*/
sw_error_t
malibu_phy_reg_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id,
		       a_uint16_t reg_val)
{

       _phy_reg_write(dev_id,phy_id, reg_id, reg_val);

	return SW_OK;
}

/******************************************************************************
*
*  phy4 medium is fiber 100fx
*
*  get phy4 medium is 100fx
*/
static a_bool_t __medium_is_fiber_100fx(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data = 0;

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_SGMII_STATUS);

	if (phy_data & MALIBU_PHY4_AUTO_FX100_SELECT) {
		return A_TRUE;
	}
	/* Link down */
	if ((!(phy_data & MALIBU_PHY4_AUTO_COPPER_SELECT)) &&
	    (!(phy_data & MALIBU_PHY4_AUTO_BX1000_SELECT)) &&
	    (!(phy_data & MALIBU_PHY4_AUTO_SGMII_SELECT))) {

		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CHIP_CONFIG);
		if ((phy_data & MALIBU_PHY4_PREFER_FIBER)
		    && (!(phy_data & MALIBU_PHY4_FIBER_MODE_1000BX))) {
			return A_TRUE;
		}
	}

	return A_FALSE;
}

/******************************************************************************
*
*  phy4 prfer medium
*
*  get phy4 prefer medum, fiber or copper;
*/
static malibu_phy_medium_t __phy_prefer_medium_get(a_uint32_t dev_id,
						   a_uint32_t phy_id)
{
	a_uint16_t phy_medium = 0;

	phy_medium =
	    malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CHIP_CONFIG);

	return ((phy_medium & MALIBU_PHY4_PREFER_FIBER) ?
		MALIBU_PHY_MEDIUM_FIBER : MALIBU_PHY_MEDIUM_COPPER);
}

/******************************************************************************
*
*  phy4 activer medium
*
*  get phy4 current active medium, fiber or copper;
*/
static malibu_phy_medium_t __phy_active_medium_get(a_uint32_t dev_id,
						   a_uint32_t phy_id)
{
	a_uint16_t phy_data = 0;
	a_uint16_t phy_mode = 0;

	phy_mode = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CHIP_CONFIG);
	phy_mode &= 0x000f;

       if (phy_mode == MALIBU_PHY_PSGMII_AMDET)  {

		phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_SGMII_STATUS);

		if ((phy_data & MALIBU_PHY4_AUTO_COPPER_SELECT)) {
			return MALIBU_PHY_MEDIUM_COPPER;
		} else if ((phy_data & MALIBU_PHY4_AUTO_BX1000_SELECT)) {
			return MALIBU_PHY_MEDIUM_FIBER;	/*PHY_MEDIUM_FIBER_BX1000 */
		} else if ((phy_data & MALIBU_PHY4_AUTO_FX100_SELECT)) {
			return MALIBU_PHY_MEDIUM_FIBER;	/*PHY_MEDIUM_FIBER_FX100 */
		}
		/* link down */
		return __phy_prefer_medium_get(dev_id, phy_id);
       }  else if ((phy_mode == MALIBU_PHY_PSGMII_BASET) ||(phy_mode == MALIBU_PHY_SGMII_BASET) )  {
       	return MALIBU_PHY_MEDIUM_COPPER;
       }  else if ((phy_mode == MALIBU_PHY_PSGMII_BX1000) ||(phy_mode == MALIBU_PHY_PSGMII_FX100)) {
       	return MALIBU_PHY_MEDIUM_FIBER;
       } else {
       	return MALIBU_PHY_MEDIUM_COPPER;
       }
}

/******************************************************************************
*
*  phy4 copper page or fiber page select
*
*  set phy4 copper or fiber page
*/

static sw_error_t __phy_reg_pages_sel(a_uint32_t dev_id, a_uint32_t phy_id,
				      malibu_phy_reg_pages_t phy_reg_pages)
{
	a_uint16_t reg_pages = 0;
	reg_pages = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CHIP_CONFIG);

	if (phy_reg_pages == MALIBU_PHY_COPPER_PAGES) {
		reg_pages |= 0x8000;
	} else if (phy_reg_pages == MALIBU_PHY_SGBX_PAGES) {
		reg_pages &= ~0x8000;
	} else
		return SW_BAD_PARAM;

	malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CHIP_CONFIG, reg_pages);

	return SW_OK;
}

/******************************************************************************
*
*  phy4 reg pages selection by active medium
*
*  phy4 reg pages selection
*/
static sw_error_t __phy_reg_pages_sel_by_active_medium(a_uint32_t dev_id,
						       a_uint32_t phy_id)
{
	malibu_phy_medium_t phy_medium;
	malibu_phy_reg_pages_t reg_pages;

	phy_medium = __phy_active_medium_get(dev_id, phy_id);
	if (phy_medium == MALIBU_PHY_MEDIUM_FIBER) {
		reg_pages = MALIBU_PHY_SGBX_PAGES;
	} else if (phy_medium == MALIBU_PHY_MEDIUM_COPPER) {

		reg_pages = MALIBU_PHY_COPPER_PAGES;
	} else

		return SW_BAD_VALUE;

	return __phy_reg_pages_sel(dev_id, phy_id, reg_pages);
}

/******************************************************************************
*
* malibu_phy_debug_write - debug port write
*
* debug port write
*/
sw_error_t
malibu_phy_debug_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id,
		       a_uint16_t reg_val)
{
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_DEBUG_PORT_ADDRESS, reg_id);
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_DEBUG_PORT_DATA, reg_val);

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_debug_read - debug port read
*
* debug port read
*/
a_uint16_t
malibu_phy_debug_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id)
{
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_DEBUG_PORT_ADDRESS, reg_id);
	return malibu_phy_reg_read(dev_id, phy_id, MALIBU_DEBUG_PORT_DATA);
}

/******************************************************************************
*
* malibu_phy_mmd_write - PHY MMD register write
*
* PHY MMD register write
*/
sw_error_t
malibu_phy_mmd_write(a_uint32_t dev_id, a_uint32_t phy_id,
		     a_uint16_t mmd_num, a_uint16_t reg_id, a_uint16_t reg_val)
{
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_MMD_CTRL_REG, mmd_num);
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_MMD_DATA_REG, reg_id);
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_MMD_CTRL_REG,
			     0x4000 | mmd_num);
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_MMD_DATA_REG, reg_val);

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_mmd_read -  PHY MMD register read
*
* PHY MMD register read
*/
a_uint16_t
malibu_phy_mmd_read(a_uint32_t dev_id, a_uint32_t phy_id,
		    a_uint16_t mmd_num, a_uint16_t reg_id)
{
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_MMD_CTRL_REG, mmd_num);
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_MMD_DATA_REG, reg_id);
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_MMD_CTRL_REG,
			     0x4000 | mmd_num);

	return malibu_phy_reg_read(dev_id, phy_id, MALIBU_MMD_DATA_REG);
}

/******************************************************************************
*
* malibu_phy_get_speed - Determines the speed of phy ports associated with the
* specified device.
*/

sw_error_t
malibu_phy_get_speed(a_uint32_t dev_id, a_uint32_t phy_id,
		     fal_port_speed_t * speed)
{
	a_uint16_t phy_data;

	if (phy_id == COMBO_PHY_ID) {
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);
	}
	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_SPEC_STATUS);

	switch (phy_data & MALIBU_STATUS_SPEED_MASK) {
	case MALIBU_STATUS_SPEED_1000MBS:
		*speed = FAL_SPEED_1000;
		break;
	case MALIBU_STATUS_SPEED_100MBS:
		*speed = FAL_SPEED_100;
		break;
	case MALIBU_STATUS_SPEED_10MBS:
		*speed = FAL_SPEED_10;
		break;
	default:
		return SW_READ_ERROR;
	}

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_get_duplex - Determines the speed of phy ports associated with the
* specified device.
*/
sw_error_t
malibu_phy_get_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
		      fal_port_duplex_t * duplex)
{
	a_uint16_t phy_data;

	if (phy_id == COMBO_PHY_ID) {
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);
	}

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_SPEC_STATUS);

	//read duplex
	if (phy_data & MALIBU_STATUS_FULL_DUPLEX)
		*duplex = FAL_FULL_DUPLEX;
	else
		*duplex = FAL_HALF_DUPLEX;

	return SW_OK;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* malibu_phy_reset - reset the phy
*
* reset the phy
*/
sw_error_t malibu_phy_reset(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;

	if (phy_id == COMBO_PHY_ID)
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL,
			     phy_data | MALIBU_CTRL_SOFTWARE_RESET);

	return SW_OK;
}
#endif
/******************************************************************************
*
* malibu_phy_set_powersave - set power saving status
*
* set power saving status
*/
sw_error_t
malibu_phy_set_powersave(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;
	a_bool_t  status;

	if (phy_id == COMBO_PHY_ID) {
		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id))
			return SW_NOT_SUPPORTED;
	}

	if (enable == A_TRUE) {
		malibu_phy_get_8023az (dev_id,phy_id,&status);
		if (status == A_FALSE) {
			phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
						   MALIBU_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL);
			phy_data &= ~(1<<14);
			malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
			     MALIBU_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL, phy_data);
		}
		phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
						   MALIBU_PHY_MMD3_ADDR_CLD_CTRL5);
		phy_data &= ~(1<<14);
		malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
			     MALIBU_PHY_MMD3_ADDR_CLD_CTRL5, phy_data);
		phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
						   MALIBU_PHY_MMD3_ADDR_CLD_CTRL3);
		phy_data &= ~(1<<15);
		malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
			     MALIBU_PHY_MMD3_ADDR_CLD_CTRL3, phy_data);

	} else {
		phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
						   MALIBU_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL);
		phy_data|= (1<<14);
		malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
			     MALIBU_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL, phy_data);
		phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
						   MALIBU_PHY_MMD3_ADDR_CLD_CTRL5);
		phy_data|= (1<<14);
		malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
			     MALIBU_PHY_MMD3_ADDR_CLD_CTRL5, phy_data);
		phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
						   MALIBU_PHY_MMD3_ADDR_CLD_CTRL3);
		phy_data|= (1<<15);
		malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
			     MALIBU_PHY_MMD3_ADDR_CLD_CTRL3, phy_data);

	}
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL, 0x9040);

	return SW_OK;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* malibu_phy_get_powersave - get power saving status
*
* set power saving status
*/
sw_error_t
malibu_phy_get_powersave(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_bool_t * enable)
{
	a_uint16_t phy_data;
	a_uint16_t phy_data1;

	if (phy_id == COMBO_PHY_ID) {
		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id))
			return SW_NOT_SUPPORTED;
	}

	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
					   MALIBU_PHY_MMD3_ADDR_CLD_CTRL5);
	phy_data1 = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
					   MALIBU_PHY_MMD3_ADDR_CLD_CTRL3);
	if (!(phy_data& 0x4000) && !(phy_data1 & 0x8000)) {
		*enable = A_TRUE;
	}
	if ((phy_data& 0x4000) && (phy_data1 & 0x8000)) {
		*enable = A_FALSE;
	}
	return SW_OK;
}
#endif
/******************************************************************************
*
* malibu_phy_set_802.3az
*
* set 802.3az status
*/
sw_error_t
malibu_phy_set_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;

	if (phy_id == COMBO_PHY_ID) {
		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id))
			return SW_NOT_SUPPORTED;
	}

	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
				       MALIBU_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);
	if (enable == A_TRUE) {
		phy_data |= 0x0006;

		malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
			     MALIBU_PHY_MMD7_ADDR_8023AZ_EEE_CTRL, phy_data);
	} else {
		phy_data &= ~0x0006;

		malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
			     MALIBU_PHY_MMD7_ADDR_8023AZ_EEE_CTRL, phy_data);
	}

	malibu_phy_restart_autoneg(dev_id, phy_id);

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_get_8023az status
*
* get 8023az status
*/
sw_error_t
malibu_phy_get_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	a_uint16_t phy_data;
	if (phy_id == COMBO_PHY_ID) {
		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id))
			return SW_NOT_SUPPORTED;
	}

	*enable = A_FALSE;

	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
				       MALIBU_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);

	if ((phy_data & 0x0004) && (phy_data & 0x0002))
		*enable = A_TRUE;

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_set_hibernate - set hibernate status
*
* set hibernate status
*/
sw_error_t
malibu_phy_set_hibernate(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;

	malibu_phy_reg_write(dev_id, phy_id, MALIBU_DEBUG_PORT_ADDRESS,
			     MALIBU_DEBUG_PHY_HIBERNATION_CTRL);
	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_DEBUG_PORT_DATA);

	if (enable == A_TRUE) {
		phy_data |= 0x8000;
	} else {
		phy_data &= ~0x8000;
	}

	malibu_phy_reg_write(dev_id, phy_id, MALIBU_DEBUG_PORT_DATA, phy_data);

	return SW_OK;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* malibu_phy_get_hibernate - get hibernate status
*
* get hibernate status
*/
sw_error_t
malibu_phy_get_hibernate(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_bool_t * enable)
{
	a_uint16_t phy_data;

	*enable = A_FALSE;

	malibu_phy_reg_write(dev_id, phy_id, MALIBU_DEBUG_PORT_ADDRESS,
			     MALIBU_DEBUG_PHY_HIBERNATION_CTRL);
	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_DEBUG_PORT_DATA);

	if (phy_data & 0x8000)
		*enable = A_TRUE;

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_set combo medium type
*
* set combo medium fiber or copper
*/
sw_error_t
malibu_phy_set_combo_prefer_medium(a_uint32_t dev_id, a_uint32_t phy_id,
				   fal_port_medium_t phy_medium)
{
	a_uint16_t phy_data;
	if (phy_id != COMBO_PHY_ID)
		return SW_NOT_SUPPORTED;

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CHIP_CONFIG);

	if (phy_medium == PHY_MEDIUM_FIBER) {
		phy_data |= MALIBU_PHY4_PREFER_FIBER;
		phy_data &= ~0x8000;
	} else if (phy_medium == PHY_MEDIUM_COPPER) {
		phy_data &= ~MALIBU_PHY4_PREFER_FIBER;
		phy_data |= 0x8000;
	} else {
		return SW_BAD_PARAM;
	}
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CHIP_CONFIG, phy_data);

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_get combo medium type
*
* get combo medium fiber or copper
*/
sw_error_t
malibu_phy_get_combo_prefer_medium(a_uint32_t dev_id, a_uint32_t phy_id,
				   fal_port_medium_t * phy_medium)
{
	a_uint16_t phy_data;
	if (phy_id != COMBO_PHY_ID)
		return SW_NOT_SUPPORTED;

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CHIP_CONFIG);

	*phy_medium =
	    (phy_data & MALIBU_PHY4_PREFER_FIBER) ? PHY_MEDIUM_FIBER :
	    PHY_MEDIUM_COPPER;

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_get current combo medium type copper or fiber
*
* get current combo medium type
*/
sw_error_t
malibu_phy_get_combo_current_medium_type(a_uint32_t dev_id, a_uint32_t phy_id,
					 fal_port_medium_t * phy_medium)
{

	if (phy_id != COMBO_PHY_ID)
		return SW_NOT_SUPPORTED;

	*phy_medium = __phy_active_medium_get(dev_id, phy_id);

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_set fiber mode 1000bx or 100fx
*
* set combo fbier mode
*/
sw_error_t
malibu_phy_set_combo_fiber_mode(a_uint32_t dev_id, a_uint32_t phy_id,
				fal_port_fiber_mode_t fiber_mode)
{
	a_uint16_t phy_data;
	if (phy_id != COMBO_PHY_ID)
		return SW_NOT_SUPPORTED;

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CHIP_CONFIG);

	if (fiber_mode == PHY_FIBER_1000BX) {
		phy_data |= MALIBU_PHY4_FIBER_MODE_1000BX;
	} else if (fiber_mode == PHY_FIBER_100FX) {
		phy_data &= ~MALIBU_PHY4_FIBER_MODE_1000BX;
	} else {
		return SW_BAD_PARAM;
	}

	malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CHIP_CONFIG, phy_data);

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_get fiber mode 1000bx or 100fx
*
* get combo fbier mode
*/
sw_error_t
malibu_phy_get_combo_fiber_mode(a_uint32_t dev_id, a_uint32_t phy_id,
				fal_port_fiber_mode_t * fiber_mode)
{
	a_uint16_t phy_data;
	if (phy_id != COMBO_PHY_ID)
		return SW_NOT_SUPPORTED;

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CHIP_CONFIG);

	*fiber_mode =
	    (phy_data & MALIBU_PHY4_FIBER_MODE_1000BX) ? PHY_FIBER_1000BX :
	    PHY_FIBER_100FX;

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_set_mdix - 
*
* set phy mdix configuraiton
*/
sw_error_t
malibu_phy_set_mdix(a_uint32_t dev_id, a_uint32_t phy_id,
		    fal_port_mdix_mode_t mode)
{
	a_uint16_t phy_data;

	if ((phy_id == COMBO_PHY_ID)
	    && (MALIBU_PHY_MEDIUM_COPPER !=
		__phy_active_medium_get(dev_id, phy_id)))

		return SW_NOT_SUPPORTED;

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_SPEC_CONTROL);

	if (mode == PHY_MDIX_AUTO) {
		phy_data |= MALIBU_PHY_MDIX_AUTO;
	} else if (mode == PHY_MDIX_MDIX) {
		phy_data &= ~MALIBU_PHY_MDIX_AUTO;
		phy_data |= MALIBU_PHY_MDIX;
	} else if (mode == PHY_MDIX_MDI) {
		phy_data &= ~MALIBU_PHY_MDIX_AUTO;
	} else {
		return SW_BAD_PARAM;
	}

	malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_SPEC_CONTROL, phy_data);

	malibu_phy_reset(dev_id, phy_id);
	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_get_mdix 
*
* get phy mdix configuration
*/
sw_error_t
malibu_phy_get_mdix(a_uint32_t dev_id, a_uint32_t phy_id,
		    fal_port_mdix_mode_t * mode)
{
	a_uint16_t phy_data;

	if ((phy_id == COMBO_PHY_ID)
	    && (MALIBU_PHY_MEDIUM_COPPER !=
		__phy_active_medium_get(dev_id, phy_id)))

		return SW_NOT_SUPPORTED;

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_SPEC_CONTROL);

	if ((phy_data & MALIBU_PHY_MDIX_AUTO) == MALIBU_PHY_MDIX_AUTO) {
		*mode = PHY_MDIX_AUTO;
	} else if ((phy_data & MALIBU_PHY_MDIX) == MALIBU_PHY_MDIX) {
		*mode = PHY_MDIX_MDIX;
	} else {
		*mode = PHY_MDIX_MDI;
	}

	return SW_OK;

}

/******************************************************************************
*
* malibu_phy_get_mdix status
*
* get phy mdix status
*/
sw_error_t
malibu_phy_get_mdix_status(a_uint32_t dev_id, a_uint32_t phy_id,
			   fal_port_mdix_status_t * mode)
{
	a_uint16_t phy_data;

	if (phy_id == COMBO_PHY_ID) {

		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id))
			return SW_NOT_SUPPORTED;

		__phy_reg_pages_sel(dev_id, phy_id, MALIBU_PHY_COPPER_PAGES);
	}

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_SPEC_STATUS);

	*mode =
	    (phy_data & MALIBU_PHY_MDIX_STATUS) ? PHY_MDIX_STATUS_MDIX :
	    PHY_MDIX_STATUS_MDI;

	return SW_OK;

}

/******************************************************************************
*
* malibu_phy_set_local_loopback
*
* set phy local loopback
*/
sw_error_t
malibu_phy_set_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			      a_bool_t enable)
{
	a_uint16_t phy_data;
	fal_port_speed_t old_speed;

       if (enable == A_TRUE) {
	     if (phy_id == COMBO_PHY_ID) {

		 if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id)) {

			__phy_reg_pages_sel(dev_id, phy_id,
					    MALIBU_PHY_SGBX_PAGES);

			if (__medium_is_fiber_100fx(dev_id, phy_id)) {

				phy_data = MALIBU_100M_LOOPBACK;

				} else {
                                   phy_data = MALIBU_1000M_LOOPBACK;
					}
				malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL, phy_data);
				return SW_OK;
			}
                 __phy_reg_pages_sel(dev_id, phy_id,
					    MALIBU_PHY_COPPER_PAGES);
		}

			  malibu_phy_get_speed(dev_id, phy_id, &old_speed);
			  if (old_speed == FAL_SPEED_1000) {
			  	phy_data = MALIBU_1000M_LOOPBACK;
			  	} else if (old_speed == FAL_SPEED_100) {
			  	   phy_data = MALIBU_100M_LOOPBACK;
			  	} else if (old_speed == FAL_SPEED_10) {
			  	   phy_data = MALIBU_10M_LOOPBACK;
			  	} else {
                                return SW_FAIL;
			  	}
         } else {
            if (phy_id == COMBO_PHY_ID) {
		 if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id)) {

			__phy_reg_pages_sel(dev_id, phy_id,
					    MALIBU_PHY_SGBX_PAGES);
			
			} else {
	                 __phy_reg_pages_sel(dev_id, phy_id,
					    MALIBU_PHY_COPPER_PAGES);
			}
           	 }
			  phy_data = MALIBU_COMMON_CTRL;
		 }


	malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL, phy_data);
	return SW_OK;

}

/******************************************************************************
*
* malibu_phy_get_local_loopback
*
* get phy local loopback
*/
sw_error_t
malibu_phy_get_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			      a_bool_t * enable)
{
	a_uint16_t phy_data;

	if (phy_id == COMBO_PHY_ID) {

		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);

	}

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);

	if (phy_data & MALIBU_LOCAL_LOOPBACK_ENABLE) {
		*enable = A_TRUE;
	} else {
		*enable = A_FALSE;
	}

	return SW_OK;

}

/******************************************************************************
*
* malibu_phy_set_remote_loopback
*
* set phy remote loopback
*/
sw_error_t
malibu_phy_set_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			       a_bool_t enable)
{
	a_uint16_t phy_data;

	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
					MALIBU_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL);

	if (enable == A_TRUE) {
		phy_data |= 0x0001;
	} else {
		phy_data &= ~0x0001;
	}

	malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
			     MALIBU_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL,
			     phy_data);
	return SW_OK;

}

/******************************************************************************
*
* malibu_phy_get_remote_loopback
*
* get phy remote loopback
*/
sw_error_t
malibu_phy_get_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
			       a_bool_t * enable)
{
	a_uint16_t phy_data;

	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
					MALIBU_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL);

	if (phy_data & 0x0001) {
		*enable = A_TRUE;
	} else {
		*enable = A_FALSE;
	}

	return SW_OK;

}

/******************************************************************************
*
* malibu_phy_cdt - cable diagnostic test
*
* cable diagnostic test
*/

static inline fal_cable_status_t _phy_cdt_status_mapping(a_uint16_t status)
{
	fal_cable_status_t status_mapping = FAL_CABLE_STATUS_INVALID;

	if (0 == status)
		status_mapping = FAL_CABLE_STATUS_INVALID;
	else if (1 == status)
		status_mapping = FAL_CABLE_STATUS_NORMAL;
	else if (2 == status)
		status_mapping = FAL_CABLE_STATUS_OPENED;
	else if (3 == status)
		status_mapping = FAL_CABLE_STATUS_SHORT;

	return status_mapping;
}

static sw_error_t malibu_phy_cdt_start(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t status = 0;
	a_uint16_t ii = 100;

	if (phy_id == COMBO_PHY_ID) {

		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id))
			return SW_NOT_SUPPORTED;

		__phy_reg_pages_sel(dev_id, phy_id, MALIBU_PHY_COPPER_PAGES);
	}

	/* RUN CDT */
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CDT_CONTROL,
			     RUN_CDT | CABLE_LENGTH_UNIT);
	do {
		aos_mdelay(30);
		status =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CDT_CONTROL);
	}
	while ((status & RUN_CDT) && (--ii));

	return SW_OK;
}

sw_error_t
malibu_phy_cdt_get(a_uint32_t dev_id, a_uint32_t phy_id,
		   fal_port_cdt_t * port_cdt)
{
	a_uint16_t status = 0;
	a_uint16_t cable_delta_time = 0;
	int i;

	if ((!port_cdt) || (phy_id > 4)) {
		return SW_FAIL;
	}


	malibu_phy_cdt_start(dev_id, phy_id);

	/* Get cable status */
	status = malibu_phy_mmd_read(dev_id, phy_id, 3, 0x8064);



	for (i = 0; i < 4; i++) {
		switch (i) {
		case 0:
			port_cdt->pair_a_status = (status >> 12) & 0x3;
			/* Get Cable Length value */
			cable_delta_time =
			    malibu_phy_mmd_read(dev_id, phy_id, 3, 0x8065);
			/* the actual cable length equals to CableDeltaTime * 0.824 */
			port_cdt->pair_a_len =
			    ((cable_delta_time & 0xff) * 800) / 1000;

			break;
		case 1:
			port_cdt->pair_b_status = (status >> 8) & 0x3;
			/* Get Cable Length value */
			cable_delta_time =
			    malibu_phy_mmd_read(dev_id, phy_id, 3, 0x8066);
			/* the actual cable length equals to CableDeltaTime * 0.824 */
			port_cdt->pair_b_len =
			    ((cable_delta_time & 0xff) * 800) / 1000;
			break;
		case 2:
			port_cdt->pair_c_status = (status >> 4) & 0x3;
			/* Get Cable Length value */
			cable_delta_time =
			    malibu_phy_mmd_read(dev_id, phy_id, 3, 0x8067);
			/* the actual cable length equals to CableDeltaTime * 0.824 */
			port_cdt->pair_c_len =
			    ((cable_delta_time & 0xff) * 800) / 1000;
			break;
		case 3:
			port_cdt->pair_d_status = status & 0x3;
			/* Get Cable Length value */
			cable_delta_time =
			    malibu_phy_mmd_read(dev_id, phy_id, 3, 0x8068);
			/* the actual cable length equals to CableDeltaTime * 0.824 */
			port_cdt->pair_d_len =
			    ((cable_delta_time & 0xff) * 800) / 1000;
			break;
		default:
			break;
		}
	}


	return SW_OK;
}

sw_error_t
malibu_phy_cdt(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t mdi_pair,
	       fal_cable_status_t * cable_status, a_uint32_t * cable_len)
{
	fal_port_cdt_t malibu_port_cdt;

	if (phy_id == COMBO_PHY_ID) {

		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id))
			return SW_NOT_SUPPORTED;

		__phy_reg_pages_sel(dev_id, phy_id, MALIBU_PHY_COPPER_PAGES);
	}

	if ((mdi_pair >= 4) || (phy_id > 4)) {
		//There are only 4 mdi pairs in 1000BASE-T
		return SW_BAD_PARAM;
	}

	malibu_phy_cdt_get(dev_id, phy_id, &malibu_port_cdt);

	switch (mdi_pair) {
	case 0:
		*cable_status =
		    _phy_cdt_status_mapping(malibu_port_cdt.pair_a_status);
		/* Get Cable Length value */
		*cable_len = malibu_port_cdt.pair_a_len;
		break;
	case 1:
		*cable_status =
		    _phy_cdt_status_mapping(malibu_port_cdt.pair_b_status);
		/* Get Cable Length value */
		*cable_len = malibu_port_cdt.pair_b_len;
		break;
	case 2:
		*cable_status =
		    _phy_cdt_status_mapping(malibu_port_cdt.pair_c_status);
		/* Get Cable Length value */
		*cable_len = malibu_port_cdt.pair_c_len;
		break;
	case 3:
		*cable_status =
		    _phy_cdt_status_mapping(malibu_port_cdt.pair_d_status);
		/* Get Cable Length value */
		*cable_len = malibu_port_cdt.pair_d_len;
		break;
	default:
		break;
	}

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_reset_done - reset the phy
*
* reset the phy
*/
a_bool_t malibu_phy_reset_done(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	a_uint16_t ii = 200;

	if (phy_id == COMBO_PHY_ID)
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);

	do {
		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);
		aos_mdelay(10);
	}
	while ((!MALIBU_RESET_DONE(phy_data)) && --ii);

	if (ii == 0)
		return A_FALSE;

	return A_TRUE;
}

/******************************************************************************
*
* malibu_autoneg_done
*
* malibu_autoneg_done
*/
a_bool_t malibu_autoneg_done(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	a_uint16_t ii = 200;

	if (phy_id == COMBO_PHY_ID)
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);

	do {
		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_STATUS);
		aos_mdelay(10);
	}
	while ((!MALIBU_AUTONEG_DONE(phy_data)) && --ii);

	if (ii == 0)
		return A_FALSE;

	return A_TRUE;
}

/******************************************************************************
*
* malibu_phy_Speed_Duplex_Resolved
 - reset the phy
*
* reset the phy
*/
a_bool_t malibu_phy_speed_duplex_resolved(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	a_uint16_t ii = 200;

	if (phy_id == COMBO_PHY_ID)
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);

	do {
		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_SPEC_STATUS);
		aos_mdelay(10);
	}
	while ((!MALIBU_SPEED_DUPLEX_RESOVLED(phy_data)) && --ii);

	if (ii == 0)
		return A_FALSE;

	return A_TRUE;
}
#endif
/******************************************************************************
*
* malibu_phy_off - power off the phy 
*
* Power off the phy
*/
sw_error_t malibu_phy_poweroff(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	if (phy_id == COMBO_PHY_ID) {
		__phy_reg_pages_sel(dev_id, phy_id, MALIBU_PHY_SGBX_PAGES);
		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);
		malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL,
				     phy_data | MALIBU_CTRL_POWER_DOWN);

		__phy_reg_pages_sel(dev_id, phy_id, MALIBU_PHY_COPPER_PAGES);
		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);
		malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL,
				     phy_data | MALIBU_CTRL_POWER_DOWN);
	} else {
		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);
		malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL,
				     phy_data | MALIBU_CTRL_POWER_DOWN);
	}
	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_on - power on the phy 
*
* Power on the phy
*/
sw_error_t malibu_phy_poweron(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	if (phy_id == COMBO_PHY_ID) {
		__phy_reg_pages_sel(dev_id, phy_id, MALIBU_PHY_SGBX_PAGES);
		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);
		malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL,
				     phy_data & ~MALIBU_CTRL_POWER_DOWN);

		__phy_reg_pages_sel(dev_id, phy_id, MALIBU_PHY_COPPER_PAGES);
		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);
		malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL,
				     phy_data & ~MALIBU_CTRL_POWER_DOWN);

	} else {
		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);
		malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL,
				     phy_data & ~MALIBU_CTRL_POWER_DOWN);
	}

	aos_mdelay(200);

	return SW_OK;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* malibu_phy_get_ability - get the phy ability
*
*
*/
sw_error_t
malibu_phy_get_ability(a_uint32_t dev_id, a_uint32_t phy_id,
		       a_uint32_t * ability)
{
	a_uint16_t phy_data;

	*ability = 0;
	if (phy_id == COMBO_PHY_ID)
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_STATUS);

	if (phy_data & MALIBU_STATUS_AUTONEG_CAPS)
		*ability |= FAL_PHY_AUTONEG_CAPS;

	if (phy_data & MALIBU_STATUS_100T2_HD_CAPS)
		*ability |= FAL_PHY_100T2_HD_CAPS;

	if (phy_data & MALIBU_STATUS_100T2_FD_CAPS)
		*ability |= FAL_PHY_100T2_FD_CAPS;

	if (phy_data & MALIBU_STATUS_10T_HD_CAPS)
		*ability |= FAL_PHY_10T_HD_CAPS;

	if (phy_data & MALIBU_STATUS_10T_FD_CAPS)
		*ability |= FAL_PHY_10T_FD_CAPS;

	if (phy_data & MALIBU_STATUS_100X_HD_CAPS)
		*ability |= FAL_PHY_100X_HD_CAPS;

	if (phy_data & MALIBU_STATUS_100X_FD_CAPS)
		*ability |= FAL_PHY_100X_FD_CAPS;

	if (phy_data & MALIBU_STATUS_100T4_CAPS)
		*ability |= FAL_PHY_100T4_CAPS;

	if (phy_data & MALIBU_STATUS_EXTENDED_STATUS) {
		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_EXTENDED_STATUS);

		if (phy_data & MALIBU_STATUS_1000T_FD_CAPS) {
			*ability |= FAL_PHY_1000T_FD_CAPS;
		}

		if (phy_data & MALIBU_STATUS_1000X_FD_CAPS) {
			*ability |= FAL_PHY_1000X_FD_CAPS;
		}
	}

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_get_ability - get the phy ability
*
*
*/
sw_error_t
malibu_phy_get_partner_ability(a_uint32_t dev_id, a_uint32_t phy_id,
			       a_uint32_t * ability)
{
	a_uint16_t phy_data;

	*ability = 0;

	if (phy_id == COMBO_PHY_ID)
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);

	phy_data =
	    malibu_phy_reg_read(dev_id, phy_id, MALIBU_LINK_PARTNER_ABILITY);

	if (phy_data & MALIBU_LINK_10BASETX_HALF_DUPLEX)
		*ability |= FAL_PHY_PART_10T_HD;

	if (phy_data & MALIBU_LINK_10BASETX_FULL_DUPLEX)
		*ability |= FAL_PHY_PART_10T_FD;

	if (phy_data & MALIBU_LINK_100BASETX_HALF_DUPLEX)
		*ability |= FAL_PHY_PART_100TX_HD;

	if (phy_data & MALIBU_LINK_100BASETX_FULL_DUPLEX)
		*ability |= FAL_PHY_PART_100TX_FD;

	if (phy_data & MALIBU_LINK_NPAGE) {
		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id,
					MALIBU_1000BASET_STATUS);

		if (phy_data & MALIBU_LINK_1000BASETX_FULL_DUPLEX)
			*ability |= FAL_PHY_PART_1000T_FD;
	}

	return SW_OK;
}
#endif
/******************************************************************************
*
* malibu_phy_status - test to see if the specified phy link is alive
*
* RETURNS:
*    A_TRUE  --> link is alive
*    A_FALSE --> link is down
*/
a_bool_t malibu_phy_get_link_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;
	if (phy_id == COMBO_PHY_ID)
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);
	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_SPEC_STATUS);

	if (phy_data & MALIBU_STATUS_LINK_PASS)
		return A_TRUE;

	return A_FALSE;
}

/******************************************************************************
*
* malibu_set_autoneg_adv - set the phy autoneg Advertisement
*
*/
sw_error_t
malibu_phy_set_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
			   a_uint32_t autoneg)
{
	a_uint16_t phy_data = 0;
	if (phy_id == COMBO_PHY_ID) {
		if (__medium_is_fiber_100fx(dev_id, phy_id))
			return SW_NOT_SUPPORTED;

		if (MALIBU_PHY_MEDIUM_COPPER ==
		    __phy_active_medium_get(dev_id, phy_id)) {

			__phy_reg_pages_sel(dev_id, phy_id,
					    MALIBU_PHY_COPPER_PAGES);
			phy_data =
			    malibu_phy_reg_read(dev_id, phy_id,
						MALIBU_AUTONEG_ADVERT);
			phy_data &= ~MALIBU_ADVERTISE_MEGA_ALL;
			phy_data &=
			    ~(MALIBU_ADVERTISE_PAUSE |
			      MALIBU_ADVERTISE_ASYM_PAUSE);

			if (autoneg & FAL_PHY_ADV_100TX_FD) {
				phy_data |= MALIBU_ADVERTISE_100FULL;
			}
			if (autoneg & FAL_PHY_ADV_100TX_HD) {
				phy_data |= MALIBU_ADVERTISE_100HALF;
			}
			if (autoneg & FAL_PHY_ADV_10T_FD) {
				phy_data |= MALIBU_ADVERTISE_10FULL;
			}
			if (autoneg & FAL_PHY_ADV_10T_HD) {
				phy_data |= MALIBU_ADVERTISE_10HALF;
			}
			if (autoneg & FAL_PHY_ADV_PAUSE) {
				phy_data |= MALIBU_ADVERTISE_PAUSE;
			}
			if (autoneg & FAL_PHY_ADV_ASY_PAUSE) {
				phy_data |= MALIBU_ADVERTISE_ASYM_PAUSE;
			}
			__phy_reg_pages_sel(dev_id, phy_id,
					    MALIBU_PHY_COPPER_PAGES);
			if (autoneg & FAL_PHY_ADV_1000T_FD) {
				phy_data |= MALIBU_EXTENDED_NEXT_PAGE_EN;
			} else {
				phy_data &= ~MALIBU_EXTENDED_NEXT_PAGE_EN;
			}
			malibu_phy_reg_write(dev_id, phy_id,
					     MALIBU_AUTONEG_ADVERT, phy_data);

			phy_data =
			    malibu_phy_reg_read(dev_id, phy_id,
						MALIBU_1000BASET_CONTROL);
			phy_data &= ~MALIBU_ADVERTISE_1000FULL;
			phy_data &= ~MALIBU_ADVERTISE_1000HALF;

			if (autoneg & FAL_PHY_ADV_1000T_FD) {
				phy_data |= MALIBU_ADVERTISE_1000FULL;
			}
			malibu_phy_reg_write(dev_id, phy_id,
					     MALIBU_1000BASET_CONTROL,
					     phy_data);
		} else {

			__phy_reg_pages_sel(dev_id, phy_id,
					    MALIBU_PHY_SGBX_PAGES);
			phy_data =
			    malibu_phy_reg_read(dev_id, phy_id,
						MALIBU_AUTONEG_ADVERT);
			phy_data &= ~MALIBU_BX_ADVERTISE_ALL;

			if (autoneg & FAL_PHY_ADV_1000BX_FD) {
				phy_data |= MALIBU_BX_ADVERTISE_1000FULL;
			}
			if (autoneg & FAL_PHY_ADV_1000BX_HD) {
				phy_data |= MALIBU_BX_ADVERTISE_1000HALF;
			}
			if (autoneg & FAL_PHY_ADV_PAUSE) {
				phy_data |= MALIBU_BX_ADVERTISE_PAUSE;
			}
			if (autoneg & FAL_PHY_ADV_ASY_PAUSE) {
				phy_data |= MALIBU_BX_ADVERTISE_ASYM_PAUSE;
			}
			__phy_reg_pages_sel(dev_id, phy_id,
					    MALIBU_PHY_SGBX_PAGES);

			malibu_phy_reg_write(dev_id, phy_id,
					     MALIBU_AUTONEG_ADVERT, phy_data);
		}
	} else {

		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_AUTONEG_ADVERT);
		phy_data &= ~MALIBU_ADVERTISE_MEGA_ALL;
		phy_data &=
		    ~(MALIBU_ADVERTISE_PAUSE | MALIBU_ADVERTISE_ASYM_PAUSE);

		if (autoneg & FAL_PHY_ADV_100TX_FD) {
			phy_data |= MALIBU_ADVERTISE_100FULL;
		}
		if (autoneg & FAL_PHY_ADV_100TX_HD) {
			phy_data |= MALIBU_ADVERTISE_100HALF;
		}
		if (autoneg & FAL_PHY_ADV_10T_FD) {
			phy_data |= MALIBU_ADVERTISE_10FULL;
		}
		if (autoneg & FAL_PHY_ADV_10T_HD) {
			phy_data |= MALIBU_ADVERTISE_10HALF;
		}
		if (autoneg & FAL_PHY_ADV_PAUSE) {
			phy_data |= MALIBU_ADVERTISE_PAUSE;
		}
		if (autoneg & FAL_PHY_ADV_ASY_PAUSE) {
			phy_data |= MALIBU_ADVERTISE_ASYM_PAUSE;
		}
		malibu_phy_reg_write(dev_id, phy_id, MALIBU_AUTONEG_ADVERT,
				     phy_data);

		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id,
					MALIBU_1000BASET_CONTROL);
		phy_data &= ~MALIBU_ADVERTISE_1000FULL;
		phy_data &= ~MALIBU_ADVERTISE_1000HALF;

		if (autoneg & FAL_PHY_ADV_1000T_FD) {
			phy_data |= MALIBU_ADVERTISE_1000FULL;
		}
		malibu_phy_reg_write(dev_id, phy_id, MALIBU_1000BASET_CONTROL,
				     phy_data);

	}

	return SW_OK;
}

/******************************************************************************
*
* malibu_get_autoneg_adv - get the phy autoneg Advertisement
*
*/
sw_error_t
malibu_phy_get_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
			   a_uint32_t * autoneg)
{
	a_uint16_t phy_data = 0;

	*autoneg = 0;

	if (phy_id == COMBO_PHY_ID) {
		if (__medium_is_fiber_100fx(dev_id, phy_id))
			return SW_NOT_SUPPORTED;

		if (MALIBU_PHY_MEDIUM_COPPER ==
		    __phy_active_medium_get(dev_id, phy_id)) {

			__phy_reg_pages_sel(dev_id, phy_id,
					    MALIBU_PHY_COPPER_PAGES);

			phy_data =
			    malibu_phy_reg_read(dev_id, phy_id,
						MALIBU_AUTONEG_ADVERT);

			if (phy_data & MALIBU_ADVERTISE_100FULL)
				*autoneg |= FAL_PHY_ADV_100TX_FD;

			if (phy_data & MALIBU_ADVERTISE_100HALF)
				*autoneg |= FAL_PHY_ADV_100TX_HD;

			if (phy_data & MALIBU_ADVERTISE_10FULL)
				*autoneg |= FAL_PHY_ADV_10T_FD;

			if (phy_data & MALIBU_ADVERTISE_10HALF)
				*autoneg |= FAL_PHY_ADV_10T_HD;

			if (phy_data & MALIBU_ADVERTISE_PAUSE)
				*autoneg |= FAL_PHY_ADV_PAUSE;

			if (phy_data & MALIBU_ADVERTISE_ASYM_PAUSE)
				*autoneg |= FAL_PHY_ADV_ASY_PAUSE;

			phy_data =
			    malibu_phy_reg_read(dev_id, phy_id,
						MALIBU_1000BASET_CONTROL);

			if (phy_data & MALIBU_ADVERTISE_1000FULL)
				*autoneg |= FAL_PHY_ADV_1000T_FD;

		} else {

			__phy_reg_pages_sel(dev_id, phy_id,
					    MALIBU_PHY_SGBX_PAGES);
			phy_data =
			    malibu_phy_reg_read(dev_id, phy_id,
						MALIBU_AUTONEG_ADVERT);

			if (phy_data & MALIBU_BX_ADVERTISE_PAUSE)
				*autoneg |= FAL_PHY_ADV_PAUSE;

			if (phy_data & MALIBU_BX_ADVERTISE_ASYM_PAUSE)
				*autoneg |= FAL_PHY_ADV_ASY_PAUSE;

			if (phy_data & MALIBU_BX_ADVERTISE_1000HALF)
				*autoneg |= FAL_PHY_ADV_1000BX_HD;

			if (phy_data & MALIBU_BX_ADVERTISE_1000FULL)
				*autoneg |= FAL_PHY_ADV_1000BX_FD;
		}
	} else {

		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id, MALIBU_AUTONEG_ADVERT);

		if (phy_data & MALIBU_ADVERTISE_100FULL)
			*autoneg |= FAL_PHY_ADV_100TX_FD;

		if (phy_data & MALIBU_ADVERTISE_100HALF)
			*autoneg |= FAL_PHY_ADV_100TX_HD;

		if (phy_data & MALIBU_ADVERTISE_10FULL)
			*autoneg |= FAL_PHY_ADV_10T_FD;

		if (phy_data & MALIBU_ADVERTISE_10HALF)
			*autoneg |= FAL_PHY_ADV_10T_HD;

		if (phy_data & MALIBU_ADVERTISE_PAUSE)
			*autoneg |= FAL_PHY_ADV_PAUSE;

		if (phy_data & MALIBU_ADVERTISE_ASYM_PAUSE)
			*autoneg |= FAL_PHY_ADV_ASY_PAUSE;

		phy_data =
		    malibu_phy_reg_read(dev_id, phy_id,
					MALIBU_1000BASET_CONTROL);
		if (phy_data & MALIBU_ADVERTISE_1000FULL)
			*autoneg |= FAL_PHY_ADV_1000T_FD;
	}

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_enable_autonego 
*
* Power off the phy
*/
a_bool_t malibu_phy_autoneg_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data;

	if (phy_id == COMBO_PHY_ID) {

		if (__medium_is_fiber_100fx(dev_id, phy_id))
			return A_FALSE;
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);
	}

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);

	if (phy_data & MALIBU_CTRL_AUTONEGOTIATION_ENABLE)
		return A_TRUE;

	return A_FALSE;
}

/******************************************************************************
*
* malibu_restart_autoneg - restart the phy autoneg
*
*/
sw_error_t malibu_phy_restart_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data = 0;

	if (phy_id == COMBO_PHY_ID) {
		if (__medium_is_fiber_100fx(dev_id, phy_id))
			return SW_NOT_SUPPORTED;
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);
	}
	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);

	phy_data |= MALIBU_CTRL_AUTONEGOTIATION_ENABLE;
	malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL,
			     phy_data | MALIBU_CTRL_RESTART_AUTONEGOTIATION);

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_enable_autonego 
*
*/
sw_error_t malibu_phy_enable_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
	a_uint16_t phy_data = 0;

	if (phy_id == COMBO_PHY_ID) {
		if (__medium_is_fiber_100fx(dev_id, phy_id))
			return SW_NOT_SUPPORTED;
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);
	}
	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);

	malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL,
			     phy_data | MALIBU_CTRL_AUTONEGOTIATION_ENABLE);

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_set_speed - Determines the speed of phy ports associated with the
* specified device.
*/
sw_error_t
malibu_phy_set_speed(a_uint32_t dev_id, a_uint32_t phy_id,
		     fal_port_speed_t speed)
{
	a_uint16_t phy_data = 0;
	fal_port_duplex_t old_duplex;

	if (phy_id == COMBO_PHY_ID) {
		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id))

			return SW_NOT_SUPPORTED;

		__phy_reg_pages_sel(dev_id, phy_id, MALIBU_PHY_COPPER_PAGES);
	}

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);

	malibu_phy_get_duplex(dev_id, phy_id, &old_duplex);

	if (old_duplex == FAL_FULL_DUPLEX) {
		phy_data |= MALIBU_CTRL_FULL_DUPLEX;

		if (FAL_SPEED_1000 == speed) {
			phy_data |= MALIBU_CTRL_SPEED_1000;
			phy_data &= ~MALIBU_CTRL_SPEED_100;
			phy_data |= MALIBU_CTRL_AUTONEGOTIATION_ENABLE;
		} else if (FAL_SPEED_100 == speed) {
			phy_data |= MALIBU_CTRL_SPEED_100;
			phy_data &= ~MALIBU_CTRL_SPEED_1000;
			phy_data &= ~MALIBU_CTRL_AUTONEGOTIATION_ENABLE;
		} else if (FAL_SPEED_10 == speed){
			phy_data &= ~MALIBU_CTRL_SPEED_100;
			phy_data &= ~MALIBU_CTRL_SPEED_1000;
			phy_data &= ~MALIBU_CTRL_AUTONEGOTIATION_ENABLE;
		} else {
		return SW_BAD_PARAM;
		}
	} else if (old_duplex == FAL_HALF_DUPLEX) {
		phy_data &= ~MALIBU_CTRL_FULL_DUPLEX;

		if (FAL_SPEED_100 == speed) {
			phy_data |= MALIBU_CTRL_SPEED_100;
			phy_data &= ~MALIBU_CTRL_SPEED_1000;
			phy_data &= ~MALIBU_CTRL_AUTONEGOTIATION_ENABLE;
		} else if (FAL_SPEED_10 == speed) {
			phy_data &= ~MALIBU_CTRL_SPEED_100;
			phy_data &= ~MALIBU_CTRL_SPEED_1000;
			phy_data &= ~MALIBU_CTRL_AUTONEGOTIATION_ENABLE;
		} else if (FAL_SPEED_1000 == speed){
            phy_data |= MALIBU_CTRL_FULL_DUPLEX;
            phy_data |= MALIBU_CTRL_SPEED_1000;
		    phy_data &= ~MALIBU_CTRL_SPEED_100;
		    phy_data |= MALIBU_CTRL_AUTONEGOTIATION_ENABLE;
		} else {
		return SW_BAD_PARAM;
	       }
	} else {
		return SW_FAIL;
	}

	malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL, phy_data);
	return SW_OK;

}

/******************************************************************************
*
* malibu_phy_set_duplex - Determines the speed of phy ports associated with the
* specified device.
*/
sw_error_t
malibu_phy_set_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
		      fal_port_duplex_t duplex)
{
	a_uint16_t phy_data = 0;
	fal_port_speed_t old_speed;

	if (phy_id == COMBO_PHY_ID) {

		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id)) {

			__phy_reg_pages_sel(dev_id, phy_id,
					    MALIBU_PHY_SGBX_PAGES);

			phy_data =
			    malibu_phy_reg_read(dev_id, phy_id,
						MALIBU_PHY_CONTROL);

			if (__medium_is_fiber_100fx(dev_id, phy_id)) {

				if (duplex == FAL_FULL_DUPLEX) {
					phy_data |= MALIBU_CTRL_FULL_DUPLEX;
				} else if (duplex == FAL_HALF_DUPLEX) {
					phy_data &= ~MALIBU_CTRL_FULL_DUPLEX;
				} else {
					return SW_BAD_PARAM;
				}
				} else {

					if (duplex == FAL_FULL_DUPLEX) {
					phy_data |= MALIBU_CTRL_FULL_DUPLEX;
					phy_data &= ~MALIBU_CTRL_AUTONEGOTIATION_ENABLE;
				} else if (duplex == FAL_HALF_DUPLEX) {
					phy_data &= ~MALIBU_CTRL_FULL_DUPLEX;
					phy_data &= ~MALIBU_CTRL_AUTONEGOTIATION_ENABLE;
				} else {
					return SW_BAD_PARAM;
				}
					}
				malibu_phy_reg_write(dev_id, phy_id,
						     MALIBU_PHY_CONTROL,
						     phy_data);

				return SW_OK;
			}
			__phy_reg_pages_sel(dev_id, phy_id,
					    MALIBU_PHY_COPPER_PAGES);
		}

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_CONTROL);

       malibu_phy_get_speed(dev_id, phy_id, &old_speed);

       if (old_speed == FAL_SPEED_1000) {

	    phy_data |= MALIBU_CTRL_SPEED_1000;
	    phy_data &= ~MALIBU_CTRL_SPEED_100;
	    phy_data |= MALIBU_CTRL_AUTONEGOTIATION_ENABLE;

		if (duplex == FAL_FULL_DUPLEX) {
		phy_data |= MALIBU_CTRL_FULL_DUPLEX;

		} else if (duplex == FAL_HALF_DUPLEX) {

		return SW_NOT_SUPPORTED;
		} else {
		return SW_BAD_PARAM;
		}
         } else if (old_speed == FAL_SPEED_100) {
	       phy_data |= MALIBU_CTRL_SPEED_100;
		phy_data &= ~MALIBU_CTRL_SPEED_1000;
		phy_data &= ~MALIBU_CTRL_AUTONEGOTIATION_ENABLE;

         if (duplex == FAL_FULL_DUPLEX) {
		phy_data |= MALIBU_CTRL_FULL_DUPLEX;
		
		} else if (duplex == FAL_HALF_DUPLEX) {

		phy_data &= ~MALIBU_CTRL_FULL_DUPLEX;

		} else {
		return SW_BAD_PARAM;
		} 
          } else if (old_speed == FAL_SPEED_10) {
          	phy_data &= ~MALIBU_CTRL_SPEED_100;
		phy_data &= ~MALIBU_CTRL_SPEED_1000;
		phy_data &= ~MALIBU_CTRL_AUTONEGOTIATION_ENABLE;

         if (duplex == FAL_FULL_DUPLEX) {
		phy_data |= MALIBU_CTRL_FULL_DUPLEX;
		
		} else if (duplex == FAL_HALF_DUPLEX) {

		phy_data &= ~MALIBU_CTRL_FULL_DUPLEX;

		} else {
		return SW_BAD_PARAM;
		}
          } else {
             return SW_FAIL;
          }
		malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_CONTROL, phy_data);

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_get_phy_id - get the phy id
*
*/
sw_error_t
malibu_phy_get_phy_id(a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *phy_data)
{
	a_uint16_t org_id, rev_id;

	org_id = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_ID1);
	rev_id = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_ID2);

	*phy_data = ((org_id & 0xffff) << 16) | (rev_id & 0xffff);

	return SW_OK;
}
#ifndef IN_PORTCONTROL_MINI
/******************************************************************************
*
* malibu_phy_set wol frame mac address
*
* set phy wol frame mac address
*/
sw_error_t
malibu_phy_set_magic_frame_mac(a_uint32_t dev_id, a_uint32_t phy_id,
			       fal_mac_addr_t * mac)
{
	a_uint16_t phy_data1;
	a_uint16_t phy_data2;
	a_uint16_t phy_data3;

	phy_data1 = (mac->uc[0] << 8) | mac->uc[1];
	phy_data2 = (mac->uc[2] << 8) | mac->uc[3];
	phy_data3 = (mac->uc[4] << 8) | mac->uc[5];

	malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
			     MALIBU_PHY_MMD3_WOL_MAGIC_MAC_CTRL1, phy_data1);

	malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
			     MALIBU_PHY_MMD3_WOL_MAGIC_MAC_CTRL2, phy_data2);

	malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
			     MALIBU_PHY_MMD3_WOL_MAGIC_MAC_CTRL3, phy_data3);

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_get wol frame mac address
*
* get phy wol frame mac address
*/
sw_error_t
malibu_phy_get_magic_frame_mac(a_uint32_t dev_id, a_uint32_t phy_id,
			       fal_mac_addr_t * mac)
{
	a_uint16_t phy_data1;
	a_uint16_t phy_data2;
	a_uint16_t phy_data3;

	phy_data1 = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
					MALIBU_PHY_MMD3_WOL_MAGIC_MAC_CTRL1);

	phy_data2 = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
					MALIBU_PHY_MMD3_WOL_MAGIC_MAC_CTRL2);

	phy_data3 = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
					MALIBU_PHY_MMD3_WOL_MAGIC_MAC_CTRL3);

	mac->uc[0] = (phy_data1 >> 8);
	mac->uc[1] = (phy_data1 & 0x00ff);
	mac->uc[2] = (phy_data2 >> 8);
	mac->uc[3] = (phy_data2 & 0x00ff);
	mac->uc[4] = (phy_data3 >> 8);
	mac->uc[5] = (phy_data3 & 0x00ff);
	phy_data2 = (mac->uc[2] << 8) | mac->uc[3];
	phy_data3 = (mac->uc[4] << 8) | mac->uc[5];

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_set wol enable or disable
*
* set phy wol enable or disable
*/
sw_error_t
malibu_phy_set_wol_status(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;
	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
				       MALIBU_PHY_MMD3_WOL_CTRL);

	if (enable == A_TRUE) {
		phy_data |= 0x0020;
	} else {
		phy_data &= ~0x0020;
	}

	malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
			     MALIBU_PHY_MMD3_WOL_CTRL, phy_data);

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_get_wol status
*
* get wol status
*/
sw_error_t
malibu_phy_get_wol_status(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	a_uint16_t phy_data;

	*enable = A_FALSE;

	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
				       MALIBU_PHY_MMD3_WOL_CTRL);

	if (phy_data & 0x0020)
		*enable = A_TRUE;

	return SW_OK;
}
#endif
/******************************************************************************
*
* malibu_serdes_reset - malibu psgmii serdes reset
*
* reset serdes
*/
sw_error_t
malibu_phy_serdes_reset(a_uint32_t dev_id)
{

	malibu_phy_reg_write(dev_id, first_phy_addr + MALIBU_PHY_PSGMII_ADDR_INC,
				MALIBU_MODE_RESET_REG, MALIBU_MODE_CHANAGE_RESET);
	mdelay(100);
	malibu_phy_reg_write(dev_id, first_phy_addr + MALIBU_PHY_PSGMII_ADDR_INC,
				MALIBU_MODE_RESET_REG, MALIBU_MODE_RESET_DEFAULT_VALUE);

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_interface mode set
*
* set malibu phy interface mode
*/
sw_error_t
malibu_phy_interface_set_mode(a_uint32_t dev_id, a_uint32_t phy_id, fal_port_interface_mode_t interface_mode)
{
	a_uint16_t phy_data;

	if ((phy_id < first_phy_addr) ||
				(phy_id > (first_phy_addr + MALIBU_PHY_MAX_ADDR_INC)))
		return SW_NOT_SUPPORTED;

	phy_data = malibu_phy_reg_read(dev_id,
		first_phy_addr + MALIBU_PHY_MAX_ADDR_INC, MALIBU_PHY_CHIP_CONFIG);
	phy_data &= 0xfff0;

	if (interface_mode == PHY_PSGMII_BASET) {
		phy_data |= MALIBU_PHY_PSGMII_BASET;
	} else if (interface_mode == PHY_PSGMII_BX1000) {
		phy_data |= MALIBU_PHY_PSGMII_BX1000;
	} else if (interface_mode == PHY_PSGMII_FX100) {
		phy_data |= MALIBU_PHY_PSGMII_FX100;
	} else if (interface_mode == PHY_PSGMII_AMDET) {
	       phy_data |= MALIBU_PHY_PSGMII_AMDET;
	} else if (interface_mode == PHY_SGMII_BASET ||
		interface_mode == PORT_QSGMII) {
	       phy_data |= MALIBU_PHY_SGMII_BASET;
	} else if (interface_mode == PHY_PSGMII_FIBER) {
		phy_data |= MALIBU_PHY_PSGMII_AMDET;
	} else {
		return SW_BAD_PARAM;
	}

	malibu_phy_reg_write(dev_id,
		first_phy_addr + MALIBU_PHY_MAX_ADDR_INC, MALIBU_PHY_CHIP_CONFIG, phy_data);

	/* reset operation */
	malibu_phy_serdes_reset(dev_id);

	if (interface_mode == PHY_PSGMII_FIBER) {
		malibu_phy_reg_write(dev_id, first_phy_addr + MALIBU_PHY_MAX_ADDR_INC,
			MALIBU_PHY_CHIP_CONFIG, MALIBU_MODECTRL_DFLT);
		malibu_phy_reg_write(dev_id, first_phy_addr + MALIBU_PHY_MAX_ADDR_INC,
			MALIBU_PHY_CONTROL, MALIBU_MIICTRL_DFLT);
	}

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_interface mode get
*
* get malibu phy interface mode
*/
sw_error_t
malibu_phy_interface_get_mode(a_uint32_t dev_id, a_uint32_t phy_id, fal_port_interface_mode_t *interface_mode)
{
	a_uint16_t phy_data;
	a_uint16_t copper_mode;

	if ((phy_id < first_phy_addr) ||
				(phy_id > (first_phy_addr + MALIBU_PHY_MAX_ADDR_INC))) {
		return SW_NOT_SUPPORTED;
	}

	phy_data = malibu_phy_reg_read(dev_id,
		first_phy_addr + MALIBU_PHY_MAX_ADDR_INC, MALIBU_PHY_CHIP_CONFIG);
	copper_mode = ((phy_data & MALIBU_PHY_COPPER_MODE) >> 0xf);
	phy_data &= 0x000f;

	switch (phy_data) {
		case MALIBU_PHY_PSGMII_BASET:
			*interface_mode = PHY_PSGMII_BASET;
			break;
		case  MALIBU_PHY_PSGMII_BX1000:
			*interface_mode = PHY_PSGMII_BX1000;
			break;
		case MALIBU_PHY_PSGMII_FX100:
			*interface_mode = PHY_PSGMII_FX100;
			break;
		case MALIBU_PHY_PSGMII_AMDET:
			if (copper_mode) {
				*interface_mode = PHY_PSGMII_AMDET;
			 } else {
				if (phy_id == first_phy_addr + MALIBU_PHY_MAX_ADDR_INC)
					*interface_mode = PHY_PSGMII_FIBER;
				else
					*interface_mode = PHY_PSGMII_AMDET;
			 }
			break;
		case MALIBU_PHY_SGMII_BASET:
			if (phy_id == first_phy_addr + MALIBU_PHY_MAX_ADDR_INC)
				*interface_mode = PHY_SGMII_BASET;
			else
				*interface_mode = PORT_QSGMII;
			break;
		default:
			*interface_mode = PORT_INTERFACE_MODE_MAX;
			break;
	}

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_interface mode status get
*
* get malibu phy interface mode status
*/
sw_error_t
malibu_phy_interface_get_mode_status(a_uint32_t dev_id, a_uint32_t phy_id, fal_port_interface_mode_t *interface_mode_status)
{
	a_uint16_t phy_data;
	a_uint16_t copper_mode;

	if ((phy_id < first_phy_addr) ||
			       (phy_id > (first_phy_addr + MALIBU_PHY_MAX_ADDR_INC))) {
		return SW_NOT_SUPPORTED;
	}

	phy_data = malibu_phy_reg_read(dev_id,
		first_phy_addr + MALIBU_PHY_MAX_ADDR_INC, MALIBU_PHY_CHIP_CONFIG);
	copper_mode = ((phy_data & MALIBU_PHY_COPPER_MODE) >> 0xf);
	phy_data &= 0x00f0;
	phy_data = (phy_data >>4);

	switch (phy_data) {
		case MALIBU_PHY_PSGMII_BASET:
			*interface_mode_status = PHY_PSGMII_BASET;
			break;
		case  MALIBU_PHY_PSGMII_BX1000:
			*interface_mode_status = PHY_PSGMII_BX1000;
			break;
		case MALIBU_PHY_PSGMII_FX100:
			*interface_mode_status = PHY_PSGMII_FX100;
			break;
		case MALIBU_PHY_PSGMII_AMDET:
			if (copper_mode) {
				*interface_mode_status = PHY_PSGMII_BASET;
			} else {
				if (phy_id == first_phy_addr + MALIBU_PHY_MAX_ADDR_INC)
					*interface_mode_status = PHY_PSGMII_FIBER;
				else
					*interface_mode_status = PHY_PSGMII_BASET;
			}
			break;
		case MALIBU_PHY_SGMII_BASET:
			if (phy_id == first_phy_addr + MALIBU_PHY_MAX_ADDR_INC)
				*interface_mode_status = PHY_SGMII_BASET;
			else
				*interface_mode_status = PORT_QSGMII;
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
* malibu_phy_intr_mask_set - Set interrupt mask with the
* specified device.
*/
sw_error_t
malibu_phy_intr_mask_set(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_uint32_t intr_mask_flag)
{
	a_uint16_t phy_data = 0;

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_INTR_MASK);

	if (FAL_PHY_INTR_STATUS_UP_CHANGE & intr_mask_flag) {
		phy_data |= MALIBU_INTR_STATUS_UP_CHANGE;
	} else {
		phy_data &= (~MALIBU_INTR_STATUS_UP_CHANGE);
	}

	if (FAL_PHY_INTR_STATUS_DOWN_CHANGE & intr_mask_flag) {
		phy_data |= MALIBU_INTR_STATUS_DOWN_CHANGE;
	} else {
		phy_data &= (~MALIBU_INTR_STATUS_DOWN_CHANGE);
	}

	if (FAL_PHY_INTR_SPEED_CHANGE & intr_mask_flag) {
		phy_data |= MALIBU_INTR_SPEED_CHANGE;
	} else {
		phy_data &= (~MALIBU_INTR_SPEED_CHANGE);
	}

	if (FAL_PHY_INTR_DUPLEX_CHANGE & intr_mask_flag) {
		phy_data |= MALIBU_INTR_DUPLEX_CHANGE;
	} else {
		phy_data &= (~MALIBU_INTR_DUPLEX_CHANGE);
	}

	if (FAL_PHY_INTR_BX_FX_STATUS_UP_CHANGE & intr_mask_flag) {
		phy_data |= MALIBU_INTR_BX_FX_STATUS_UP_CHANGE;
	} else {
		phy_data &= (~MALIBU_INTR_BX_FX_STATUS_UP_CHANGE);
	}

	if (FAL_PHY_INTR_BX_FX_STATUS_DOWN_CHANGE & intr_mask_flag) {
		phy_data |= MALIBU_INTR_BX_FX_STATUS_DOWN_CHANGE;
	} else {
		phy_data &= (~MALIBU_INTR_BX_FX_STATUS_DOWN_CHANGE);
	}

	if (FAL_PHY_INTR_MEDIA_STATUS_CHANGE & intr_mask_flag) {
		phy_data |= MALIBU_INTR_MEDIA_STATUS_CHANGE;
	} else {
		phy_data &= (~MALIBU_INTR_MEDIA_STATUS_CHANGE);
	}

	if (FAL_PHY_INTR_WOL_STATUS & intr_mask_flag) {
		phy_data |= MALIBU_INTR_WOL;
	} else {
		phy_data &= (~MALIBU_INTR_WOL);
	}

	if (FAL_PHY_INTR_POE_STATUS & intr_mask_flag) {
		phy_data |= MALIBU_INTR_POE;
	} else {
		phy_data &= (~MALIBU_INTR_POE);
	}

	malibu_phy_reg_write(dev_id, phy_id, MALIBU_PHY_INTR_MASK, phy_data);
	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_intr_mask_get - Get interrupt mask with the
* specified device.
*/
sw_error_t
malibu_phy_intr_mask_get(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_uint32_t * intr_mask_flag)
{
	a_uint16_t phy_data = 0;

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_INTR_MASK);

	*intr_mask_flag = 0;
	if (MALIBU_INTR_STATUS_UP_CHANGE & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_STATUS_UP_CHANGE;
	}

	if (MALIBU_INTR_STATUS_DOWN_CHANGE & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_STATUS_DOWN_CHANGE;
	}

	if (MALIBU_INTR_SPEED_CHANGE & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_SPEED_CHANGE;
	}

	if (MALIBU_INTR_DUPLEX_CHANGE & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_DUPLEX_CHANGE;
	}

	if (MALIBU_INTR_BX_FX_STATUS_UP_CHANGE & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_BX_FX_STATUS_UP_CHANGE;
	}

	if (MALIBU_INTR_BX_FX_STATUS_DOWN_CHANGE & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_BX_FX_STATUS_DOWN_CHANGE;
	}

	if (MALIBU_INTR_MEDIA_STATUS_CHANGE  & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_MEDIA_STATUS_CHANGE;
	}

	if (MALIBU_INTR_WOL  & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_WOL_STATUS;
	}

	if (MALIBU_INTR_POE  & phy_data) {
		*intr_mask_flag |= FAL_PHY_INTR_POE_STATUS;
	}

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_intr_status_get - Get interrupt status with the
* specified device.
*/
sw_error_t
malibu_phy_intr_status_get(a_uint32_t dev_id, a_uint32_t phy_id,
			   a_uint32_t * intr_status_flag)
{
	a_uint16_t phy_data = 0;

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_INTR_STATUS);

	*intr_status_flag = 0;
	if (MALIBU_INTR_STATUS_UP_CHANGE & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_STATUS_UP_CHANGE;
	}

	if (MALIBU_INTR_STATUS_DOWN_CHANGE & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_STATUS_DOWN_CHANGE;
	}

	if (MALIBU_INTR_SPEED_CHANGE & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_SPEED_CHANGE;
	}

	if (MALIBU_INTR_DUPLEX_CHANGE & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_DUPLEX_CHANGE;
	}

	if (MALIBU_INTR_BX_FX_STATUS_UP_CHANGE & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_BX_FX_STATUS_UP_CHANGE;
	}

	if (MALIBU_INTR_BX_FX_STATUS_DOWN_CHANGE & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_BX_FX_STATUS_DOWN_CHANGE;
	}
	if (MALIBU_INTR_MEDIA_STATUS_CHANGE  & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_MEDIA_STATUS_CHANGE;
	}

	if (MALIBU_INTR_WOL  & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_WOL_STATUS;
	}

	if (MALIBU_INTR_POE  & phy_data) {
		*intr_status_flag |= FAL_PHY_INTR_POE_STATUS;
	}

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_set_counter - set counter status
*
* set counter  status
*/
sw_error_t
malibu_phy_set_counter(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;

	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
					MALIBU_PHY_MMD7_COUNTER_CTRL);

	if (enable == A_TRUE) {
		phy_data |= 0x0003;
	} else {
		phy_data &= ~0x0003;
	}

	malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
			     MALIBU_PHY_MMD7_COUNTER_CTRL, phy_data);

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_get_counter_status - get counter status
*
* set counter status
*/
sw_error_t
malibu_phy_get_counter(a_uint32_t dev_id, a_uint32_t phy_id,
			 a_bool_t * enable)
{
	a_uint16_t phy_data;

	*enable = A_FALSE;

	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
					MALIBU_PHY_MMD7_COUNTER_CTRL);

	if (phy_data & 0x0001)
		*enable = A_TRUE;

	return SW_OK;
}

/******************************************************************************
*
* malibu_phy_show show counter statistics
*
* show counter statistics
*/
sw_error_t
malibu_phy_show_counter(a_uint32_t dev_id, a_uint32_t phy_id,
			 fal_port_counter_info_t * counter_infor)
{
	a_uint16_t ingress_high_counter;
	a_uint16_t ingress_low_counter;
	a_uint16_t egress_high_counter;
	a_uint16_t egress_low_counter;

	ingress_high_counter = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
					MALIBU_PHY_MMD7_INGRESS_COUNTER_HIGH);
	ingress_low_counter = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
					MALIBU_PHY_MMD7_INGRESS_COUNTER_LOW);
	counter_infor->RxGoodFrame = (ingress_high_counter << 16 ) | ingress_low_counter;
	counter_infor->RxBadCRC = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
					MALIBU_PHY_MMD7_INGRESS_ERROR_COUNTER);

	egress_high_counter = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
					MALIBU_PHY_MMD7_EGRESS_COUNTER_HIGH);
	egress_low_counter = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
					MALIBU_PHY_MMD7_EGRESS_COUNTER_LOW);
	counter_infor->TxGoodFrame = (egress_high_counter << 16 ) | egress_low_counter;
	counter_infor->TxBadCRC = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
					MALIBU_PHY_MMD7_EGRESS_ERROR_COUNTER);

	return SW_OK;
}
#endif
/******************************************************************************
*
* malibu_phy_get status
*
* get phy status
*/
sw_error_t
malibu_phy_get_status(a_uint32_t dev_id, a_uint32_t phy_id,
		struct port_phy_status *phy_status)
{
	a_uint16_t phy_data;

	if (phy_id == COMBO_PHY_ID) {
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);
	}

	phy_data = malibu_phy_reg_read(dev_id, phy_id, MALIBU_PHY_SPEC_STATUS);

	/*get phy link status*/
	if (phy_data & MALIBU_STATUS_LINK_PASS) {
		phy_status->link_status = A_TRUE;
	} else {
		phy_status->link_status = A_FALSE;
		return SW_OK;
	}

	/*get phy speed*/
	switch (phy_data & MALIBU_STATUS_SPEED_MASK) {
	case MALIBU_STATUS_SPEED_1000MBS:
		phy_status->speed = FAL_SPEED_1000;
		break;
	case MALIBU_STATUS_SPEED_100MBS:
		phy_status->speed = FAL_SPEED_100;
		break;
	case MALIBU_STATUS_SPEED_10MBS:
		phy_status->speed = FAL_SPEED_10;
		break;
	default:
		return SW_READ_ERROR;
	}

	/*get phy duplex*/
	if (phy_data & MALIBU_STATUS_FULL_DUPLEX) {
		phy_status->duplex = FAL_FULL_DUPLEX;
	} else {
		phy_status->duplex = FAL_HALF_DUPLEX;
	}

	/* get phy flowctrl resolution status */
	if (phy_data & MALIBU_PHY_RX_FLOWCTRL_STATUS) {
		phy_status->rx_flowctrl = A_TRUE;
	} else {
		phy_status->rx_flowctrl = A_FALSE;
	}

	if (phy_data & MALIBU_PHY_TX_FLOWCTRL_STATUS) {
		phy_status->tx_flowctrl = A_TRUE;
	} else {
		phy_status->tx_flowctrl = A_FALSE;
	}

	return SW_OK;
}
/******************************************************************************
*
* malibu_phy_set_eee_advertisement
*
* set eee advertisement
*/
sw_error_t
malibu_phy_set_eee_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t adv)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	if (phy_id == COMBO_PHY_ID) {
		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id))
			return SW_NOT_SUPPORTED;
	}

	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
				       MALIBU_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);
	phy_data &= ~(MALIBU_PHY_EEE_ADV_100M | MALIBU_PHY_EEE_ADV_1000M);

	if (adv & FAL_PHY_EEE_100BASE_T) {
		phy_data |= MALIBU_PHY_EEE_ADV_100M;
	}
	if (adv & FAL_PHY_EEE_1000BASE_T) {
		phy_data |= MALIBU_PHY_EEE_ADV_1000M;
	}

	rv = malibu_phy_mmd_write(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
		     MALIBU_PHY_MMD7_ADDR_8023AZ_EEE_CTRL, phy_data);

	rv = malibu_phy_restart_autoneg(dev_id, phy_id);

	return rv;

}

/******************************************************************************
*
* malibu_phy_get_eee_advertisement
*
* get eee advertisement
*/
sw_error_t
malibu_phy_get_eee_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *adv)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	if (phy_id == COMBO_PHY_ID) {
		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id))
			return SW_NOT_SUPPORTED;
	}

	*adv = 0;
	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
				       MALIBU_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);

	if (phy_data & MALIBU_PHY_EEE_ADV_100M) {
		*adv |= FAL_PHY_EEE_100BASE_T;
	}
	if (phy_data & MALIBU_PHY_EEE_ADV_1000M) {
		*adv |= FAL_PHY_EEE_1000BASE_T;
	}

	return rv;
}
/******************************************************************************
*
* malibu_phy_get_eee_partner_advertisement
*
* get eee partner advertisement
*/
sw_error_t
malibu_phy_get_eee_partner_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *adv)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	if (phy_id == COMBO_PHY_ID) {
		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id))
			return SW_NOT_SUPPORTED;
	}

	*adv = 0;
	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
				       MALIBU_PHY_MMD7_ADDR_8023AZ_EEE_PARTNER);

	if (phy_data & MALIBU_PHY_EEE_PARTNER_ADV_100M) {
		*adv |= FAL_PHY_EEE_100BASE_T;
	}
	if (phy_data & MALIBU_PHY_EEE_PARTNER_ADV_1000M) {
		*adv |= FAL_PHY_EEE_1000BASE_T;
	}

	return rv;
}
/******************************************************************************
*
* malibu_phy_get_eee_capability
*
* get eee capability
*/
sw_error_t
malibu_phy_get_eee_cap(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *cap)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	if (phy_id == COMBO_PHY_ID) {
		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id))
			return SW_NOT_SUPPORTED;
	}

	*cap = 0;
	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD3_NUM,
				       MALIBU_PHY_MMD3_ADDR_8023AZ_EEE_CAPABILITY);

	if (phy_data & MALIBU_PHY_EEE_CAPABILITY_100M) {
		*cap |= FAL_PHY_EEE_100BASE_T;
	}
	if (phy_data & MALIBU_PHY_EEE_CAPABILITY_1000M) {
		*cap |= FAL_PHY_EEE_1000BASE_T;
	}

	return rv;
}
/******************************************************************************
*
* malibu_phy_get_eee_status
*
* get eee status
*/
sw_error_t
malibu_phy_get_eee_status(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *status)
{
	a_uint16_t phy_data = 0;
	sw_error_t rv = SW_OK;

	if (phy_id == COMBO_PHY_ID) {
		if (MALIBU_PHY_MEDIUM_COPPER !=
		    __phy_active_medium_get(dev_id, phy_id))
			return SW_NOT_SUPPORTED;
	}

	*status = 0;
	phy_data = malibu_phy_mmd_read(dev_id, phy_id, MALIBU_PHY_MMD7_NUM,
				       MALIBU_PHY_MMD7_ADDR_8023AZ_EEE_STATUS);

	if (phy_data & MALIBU_PHY_EEE_STATUS_100M) {
		*status |= FAL_PHY_EEE_100BASE_T;
	}
	if (phy_data & MALIBU_PHY_EEE_STATUS_1000M) {
		*status |= FAL_PHY_EEE_1000BASE_T;
	}

	return rv;
}
/******************************************************************************
*
* malibu_phy_hw_register init
*
*/
sw_error_t
malibu_phy_hw_init(a_uint32_t dev_id, a_uint32_t port_bmp)
{
	a_uint32_t port_id = 0, phy_addr = 0, phy_cnt = 0;
	a_uint16_t dac_value,led_status, phy_data;
	a_uint32_t mode;

	for (port_id = 0; port_id < SW_MAX_NR_PORT; port_id ++)
	{
		if (port_bmp & (0x1 << port_id))
		{
			phy_cnt ++;
			phy_addr = qca_ssdk_port_to_phy_addr(dev_id, port_id);
			if (phy_addr < first_phy_addr)
			{
				first_phy_addr = phy_addr;
			}
			/*enable phy power saving function by default */
			malibu_phy_set_8023az(dev_id, phy_addr, A_TRUE);
			malibu_phy_set_powersave(dev_id, phy_addr, A_TRUE);
			malibu_phy_set_hibernate(dev_id, phy_addr, A_TRUE);
			/*change malibu control_dac[2:0] of MMD7 0x801A bit[9:7] from 111 to 101*/
			dac_value = malibu_phy_mmd_read(dev_id, phy_addr, MALIBU_PHY_MMD7_NUM,
				MALIBU_PHY_MMD7_DAC_CTRL);
			dac_value &= ~MALIBU_DAC_CTRL_MASK;
			dac_value |= MALIBU_DAC_CTRL_VALUE;
			malibu_phy_mmd_write(dev_id, phy_addr, MALIBU_PHY_MMD7_NUM,
				MALIBU_PHY_MMD7_DAC_CTRL, dac_value);

			/* add 10M and 100M link LED behavior for QFN board*/
			led_status = malibu_phy_mmd_read(dev_id, phy_addr, MALIBU_PHY_MMD7_NUM,
				MALIBU_PHY_MMD7_LED_1000_CTRL1);
			led_status &= ~MALIBU_LED_1000_CTRL1_100_10_MASK;
			led_status |= MALIBU_LED_1000_CTRL1_100_10_MASK;
			malibu_phy_mmd_write(dev_id, phy_addr, MALIBU_PHY_MMD7_NUM,
				MALIBU_PHY_MMD7_LED_1000_CTRL1, led_status);
			/*disable Extended next page*/
			phy_data = malibu_phy_reg_read(dev_id, phy_addr, MALIBU_AUTONEG_ADVERT);
			phy_data &= ~MALIBU_EXTENDED_NEXT_PAGE_EN;
			malibu_phy_reg_write(dev_id, phy_addr, MALIBU_AUTONEG_ADVERT, phy_data);
		}
	}
	/* qca 8072 two ports phy chip's firstly address to init phy chip */
	if ((phy_cnt == QCA8072_PHY_NUM) && (first_phy_addr >= 0x3)) {
		first_phy_addr = first_phy_addr - 0x3;
	}

	/*workaround to enable AZ transmitting ability*/
	malibu_phy_mmd_write(dev_id, first_phy_addr + 5, MALIBU_PHY_MMD1_NUM,
			     MALIBU_PSGMII_MODE_CTRL, MALIBU_PHY_PSGMII_MODE_CTRL_ADJUST_VALUE);

	/* adjust psgmii serdes tx amp */
	malibu_phy_reg_write(dev_id, first_phy_addr + 5, MALIBU_PSGMII_TX_DRIVER_1_CTRL,
		MALIBU_PHY_PSGMII_REDUCE_SERDES_TX_AMP);

	/* to avoid psgmii module goes into hibernation, work with psgmii self test*/
	phy_data = malibu_phy_mmd_read(dev_id, first_phy_addr + 4, MALIBU_PHY_MMD3_NUM,
		MALIBU_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL);
	phy_data &= (~(1<<1));
	malibu_phy_mmd_write(dev_id, first_phy_addr + 4, MALIBU_PHY_MMD3_NUM,
		MALIBU_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL, phy_data);


	mode = ssdk_dt_global_get_mac_mode(dev_id, 0);
	if (mode == PORT_WRAPPER_PSGMII_FIBER)
		malibu_phy_interface_set_mode(dev_id, first_phy_addr, PHY_PSGMII_FIBER);
	return SW_OK;
}

static int malibu_phy_api_ops_init(void)
{

	int ret;
	hsl_phy_ops_t *malibu_phy_api_ops = NULL;

	malibu_phy_api_ops = kzalloc(sizeof(hsl_phy_ops_t), GFP_KERNEL);
	if (malibu_phy_api_ops == NULL) {
		SSDK_ERROR("malibu phy ops kzalloc failed!\n");
		return -ENOMEM;
	}

	phy_api_ops_init(MALIBU_PHY_CHIP);
#ifndef IN_PORTCONTROL_MINI
	malibu_phy_api_ops->phy_hibernation_set = malibu_phy_set_hibernate;
	malibu_phy_api_ops->phy_hibernation_get = malibu_phy_get_hibernate;
#endif
	malibu_phy_api_ops->phy_speed_get = malibu_phy_get_speed;
	malibu_phy_api_ops->phy_speed_set = malibu_phy_set_speed;
	malibu_phy_api_ops->phy_duplex_get = malibu_phy_get_duplex;
	malibu_phy_api_ops->phy_duplex_set = malibu_phy_set_duplex;
	malibu_phy_api_ops->phy_autoneg_enable_set = malibu_phy_enable_autoneg;
	malibu_phy_api_ops->phy_restart_autoneg = malibu_phy_restart_autoneg;
	malibu_phy_api_ops->phy_autoneg_status_get = malibu_phy_autoneg_status;
	malibu_phy_api_ops->phy_autoneg_adv_set = malibu_phy_set_autoneg_adv;
	malibu_phy_api_ops->phy_autoneg_adv_get = malibu_phy_get_autoneg_adv;
#ifndef IN_PORTCONTROL_MINI
	malibu_phy_api_ops->phy_powersave_set = malibu_phy_set_powersave;
	malibu_phy_api_ops->phy_powersave_get = malibu_phy_get_powersave;
	malibu_phy_api_ops->phy_cdt = malibu_phy_cdt;
#endif
	malibu_phy_api_ops->phy_link_status_get = malibu_phy_get_link_status;
#ifndef IN_PORTCONTROL_MINI
	malibu_phy_api_ops->phy_mdix_set = malibu_phy_set_mdix;
	malibu_phy_api_ops->phy_mdix_get = malibu_phy_get_mdix;
	malibu_phy_api_ops->phy_mdix_status_get = malibu_phy_get_mdix_status;
	malibu_phy_api_ops->phy_8023az_set = malibu_phy_set_8023az;
	malibu_phy_api_ops->phy_8023az_get = malibu_phy_get_8023az;
	malibu_phy_api_ops->phy_local_loopback_set = malibu_phy_set_local_loopback;
	malibu_phy_api_ops->phy_local_loopback_get = malibu_phy_get_local_loopback;
	malibu_phy_api_ops->phy_remote_loopback_set = malibu_phy_set_remote_loopback;
	malibu_phy_api_ops->phy_remote_loopback_get = malibu_phy_get_remote_loopback;
	malibu_phy_api_ops->phy_combo_prefer_medium_set = malibu_phy_set_combo_prefer_medium;
	malibu_phy_api_ops->phy_combo_prefer_medium_get = malibu_phy_get_combo_prefer_medium;
	malibu_phy_api_ops->phy_combo_medium_status_get = malibu_phy_get_combo_current_medium_type;
	malibu_phy_api_ops->phy_combo_fiber_mode_set = malibu_phy_set_combo_fiber_mode;
	malibu_phy_api_ops->phy_combo_fiber_mode_get = malibu_phy_get_combo_fiber_mode;
	malibu_phy_api_ops->phy_reset = malibu_phy_reset;
#endif
	malibu_phy_api_ops->phy_power_off = malibu_phy_poweroff;
	malibu_phy_api_ops->phy_power_on = 	malibu_phy_poweron;
	malibu_phy_api_ops->phy_id_get = malibu_phy_get_phy_id;
	malibu_phy_api_ops->phy_reg_write = malibu_phy_reg_write;
	malibu_phy_api_ops->phy_reg_read = malibu_phy_reg_read;
	malibu_phy_api_ops->phy_debug_write = malibu_phy_debug_write;
	malibu_phy_api_ops->phy_debug_read = malibu_phy_debug_read;
	malibu_phy_api_ops->phy_mmd_write = malibu_phy_mmd_write;
	malibu_phy_api_ops->phy_mmd_read = malibu_phy_mmd_read;
#ifndef IN_PORTCONTROL_MINI
	malibu_phy_api_ops->phy_magic_frame_mac_set = malibu_phy_set_magic_frame_mac;
	malibu_phy_api_ops->phy_magic_frame_mac_get = malibu_phy_get_magic_frame_mac;
	malibu_phy_api_ops->phy_wol_status_set = malibu_phy_set_wol_status;
	malibu_phy_api_ops->phy_wol_status_get = malibu_phy_get_wol_status;
#endif
	malibu_phy_api_ops->phy_interface_mode_set = malibu_phy_interface_set_mode;
	malibu_phy_api_ops->phy_interface_mode_get = malibu_phy_interface_get_mode;
	malibu_phy_api_ops->phy_interface_mode_status_get = malibu_phy_interface_get_mode_status;
#ifndef IN_PORTCONTROL_MINI
	malibu_phy_api_ops->phy_intr_mask_set = malibu_phy_intr_mask_set;
	malibu_phy_api_ops->phy_intr_mask_get = malibu_phy_intr_mask_get;
	malibu_phy_api_ops->phy_intr_status_get = malibu_phy_intr_status_get;
	malibu_phy_api_ops->phy_counter_set = malibu_phy_set_counter;
	malibu_phy_api_ops->phy_counter_get = malibu_phy_get_counter;
	malibu_phy_api_ops->phy_counter_show = malibu_phy_show_counter;
#endif
	malibu_phy_api_ops->phy_serdes_reset = malibu_phy_serdes_reset;
	malibu_phy_api_ops->phy_get_status = malibu_phy_get_status;
	malibu_phy_api_ops->phy_eee_adv_set = malibu_phy_set_eee_adv;
	malibu_phy_api_ops->phy_eee_adv_get = malibu_phy_get_eee_adv;
	malibu_phy_api_ops->phy_eee_partner_adv_get = malibu_phy_get_eee_partner_adv;
	malibu_phy_api_ops->phy_eee_cap_get = malibu_phy_get_eee_cap;
	malibu_phy_api_ops->phy_eee_status_get = malibu_phy_get_eee_status;

	ret = hsl_phy_api_ops_register(MALIBU_PHY_CHIP, malibu_phy_api_ops);

	if (ret == 0)
		SSDK_INFO("qca probe malibu phy driver succeeded!\n");
	else
		SSDK_ERROR("qca probe malibu phy driver failed! (code: %d)\n", ret);
	return ret;
}

/******************************************************************************
*
* malibu_phy_init -
*
*/
int malibu_phy_init(a_uint32_t dev_id, a_uint32_t port_bmp)
{
	static a_uint32_t phy_ops_flag = 0;

	if(phy_ops_flag == 0) {
		malibu_phy_api_ops_init();
		phy_ops_flag = 1;
	}
	malibu_phy_hw_init(dev_id, port_bmp);

	return 0;
}

