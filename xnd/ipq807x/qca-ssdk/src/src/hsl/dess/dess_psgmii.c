/*
 * Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
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
#include "sd.h"
#include "dess_psgmii.h"
#include "aos_timer.h"

static psgmii_interface_mac_mode_t psgmii_mac_mode = {0};
static a_uint32_t sgmii_ch_id = 0;

sw_error_t
dess_psgmii_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
             a_uint32_t value_len)
{
    sw_error_t rv;

    rv = sd_reg_psgmii_get(dev_id, reg_addr, value, value_len);
    return rv;
}

sw_error_t
dess_psgmii_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
             a_uint32_t value_len)
{
    sw_error_t rv;

    rv = sd_reg_psgmii_set(dev_id, reg_addr, value, value_len);
    return rv;
}

sw_error_t
dess_psgmii_reg_field_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                   a_uint32_t bit_offset, a_uint32_t field_len,
                   a_uint8_t value[], a_uint32_t value_len)
{
    a_uint32_t reg_val = 0;

    if ((bit_offset >= 32 || (field_len > 32)) || (field_len == 0))
        return SW_OUT_OF_RANGE;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    SW_RTN_ON_ERROR(dess_psgmii_reg_get(dev_id, reg_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));


    if(32 == field_len)
    {
        *((a_uint32_t *) value) = reg_val;
    }
    else
    {
        *((a_uint32_t *) value) = SW_REG_2_FIELD(reg_val, bit_offset, field_len);
    }
    return SW_OK;
}

sw_error_t
dess_psgmii_reg_field_set(a_uint32_t dev_id, a_uint32_t reg_addr,
                   a_uint32_t bit_offset, a_uint32_t field_len,
                   const a_uint8_t value[], a_uint32_t value_len)
{
    a_uint32_t reg_val = 0;
    a_uint32_t field_val = *((a_uint32_t *) value);

    if ((bit_offset >= 32 || (field_len > 32)) || (field_len == 0))
        return SW_OUT_OF_RANGE;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    SW_RTN_ON_ERROR(dess_psgmii_reg_get(dev_id, reg_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));

    if(32 == field_len)
    {
        reg_val = field_val;
    }
    else
    {
        SW_REG_SET_BY_FIELD_U32(reg_val, field_val, bit_offset, field_len);
    }

    SW_RTN_ON_ERROR(dess_psgmii_reg_set(dev_id, reg_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));

    return SW_OK;
}


/******************************************************************************
*
* psgmii_set_powersave - set power saving status
*
* set power saving status
*/
sw_error_t
dess_psgmii_set_lpi(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    if (phy_id == 0)
    {
        rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_EEE_CH0_5,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else
    {
        rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_EEE_CH1_1 + phy_id * 0xc,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    if (enable)
    {
		data &= ~(PSGMIIPHY_EEE_DIS_LPI);
		data |= PSGMIIPHY_EEE_EN_LPI;
    }
    else
    {
		data |= PSGMIIPHY_EEE_DIS_LPI;
		data &= ~(PSGMIIPHY_EEE_EN_LPI);
    }

    if (phy_id == 0)
    {
        rv = dess_psgmii_reg_set(dev_id, PSGMIIPHY_EEE_CH0_5,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else
    {
        rv = dess_psgmii_reg_set(dev_id, PSGMIIPHY_EEE_CH1_1 + phy_id * 0xc,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

/******************************************************************************
*
*psgmii_get_powersave - get power saving status
*
* set power saving status
*/
sw_error_t
dess_psgmii_get_lpi(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t *enable)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    if (phy_id == 0)
    {
        rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_EEE_CH0_5,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else
    {
        rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_EEE_CH1_1 + phy_id * 0xc,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    if (!(data & PSGMIIPHY_EEE_DIS_LPI) && (data & PSGMIIPHY_EEE_EN_LPI))
    {
		*enable = A_TRUE;
    }
    else
    {
		*enable = A_FALSE;
    }

    return SW_OK;
}

/******************************************************************************
*
*psgmii_set_hibernate - set hibernate status
*
* set hibernate status
*/
sw_error_t
dess_psgmii_set_interface_type(a_uint32_t dev_id, a_uint32_t phy_id,
								psgmii_interface_mac_mode_t mode)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_MODE_CONTROL,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    switch(mode)
    {
		case PSGMII_MAC_MODE_PSGMII:
		{
			data |= PSGMIIPHY_MODE_CH0_PSGMII_QSGMII;
			data &= (~PSGMIIPHY_MODE_CH0_QSGMII_SGMII);
			data &= (~PSGMIIPHY_MODE_CH4_CH1_0_SGMII);
			data &= (~PSGMIIPHY_MODE_CH1_CH0_SGMII);
			break;
		}
		case PSGMII_MAC_MODE_QSGMII:
		{
			data &= (~PSGMIIPHY_MODE_CH0_PSGMII_QSGMII);
			data |= PSGMIIPHY_MODE_CH0_QSGMII_SGMII;
			if (phy_id == 0)
			{
				data &= (~PSGMIIPHY_MODE_CH4_CH1_0_SGMII);
				data &= (~PSGMIIPHY_MODE_CH1_CH0_SGMII);
			}
			else if (phy_id == 1)
			{
				data &= (~PSGMIIPHY_MODE_CH4_CH1_0_SGMII);
				data |= PSGMIIPHY_MODE_CH1_CH0_SGMII;
			}
			else if (phy_id == 4)
			{
				data |= PSGMIIPHY_MODE_CH4_CH1_0_SGMII;
				data &= (~PSGMIIPHY_MODE_CH1_CH0_SGMII);
			}
			else
			{
				return SW_NOT_SUPPORTED;
			}

			break;
		}
		case PSGMII_MAC_MODE_SGMII:
		{
			data &= (~PSGMIIPHY_MODE_CH0_PSGMII_QSGMII);
			data &= (~PSGMIIPHY_MODE_CH0_QSGMII_SGMII);
			if (phy_id == 0)
			{
				data &= (~PSGMIIPHY_MODE_CH4_CH1_0_SGMII);
				data &= (~PSGMIIPHY_MODE_CH1_CH0_SGMII);
			}
			else if (phy_id == 1)
			{
				data &= (~PSGMIIPHY_MODE_CH4_CH1_0_SGMII);
				data |= PSGMIIPHY_MODE_CH1_CH0_SGMII;
			}
			else if (phy_id == 4)
			{
				data |= PSGMIIPHY_MODE_CH4_CH1_0_SGMII;
				data &= (~PSGMIIPHY_MODE_CH1_CH0_SGMII);
			}
			else
			{
				return SW_NOT_SUPPORTED;
			}

			break;
		}
		default:
			break;
	}

    rv = dess_psgmii_reg_set(dev_id, PSGMIIPHY_MODE_CONTROL,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    psgmii_mac_mode = mode;
    sgmii_ch_id = phy_id;

    return SW_OK;
}

/******************************************************************************
*
* psgmii_get_hibernate - get hibernate status
*
* get hibernate status
*/
sw_error_t
dess_psgmii_get_interface_type(a_uint32_t dev_id, a_uint32_t * phy_id,
								psgmii_interface_mac_mode_t * mode)
{
    *phy_id = sgmii_ch_id;
    *mode = psgmii_mac_mode;

    return SW_OK;
}

/******************************************************************************
*
* psgmii_autoneg_done
*
*psgmii_autoneg_done
*/
a_bool_t
dess_psgmii_autoneg_done(a_uint32_t dev_id, a_uint32_t phy_id)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_1 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data & PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_1_MR_AN_COMPLETE)
        return A_TRUE;
    else
        return A_FALSE;

    return A_TRUE;
}

/******************************************************************************
*
*psgmii_reset - reset the psgmii
*
* reset the psgmii
*/
sw_error_t
dess_psgmii_reset(a_uint32_t dev_id, a_uint32_t phy_id)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data |= PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4_MR_MAIN_RESET_25M;

    rv = dess_psgmii_reg_set(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

/******************************************************************************
*
* psgmii_off - power off the psgmii to change its speed
*
* Power off the psgmii
*/
sw_error_t
dess_psgmii_poweroff(a_uint32_t dev_id, a_uint32_t phy_id)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data &= (~PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4_POWER_ON_25M);

    rv = dess_psgmii_reg_set(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

/******************************************************************************
*
* psgmii_on - power on the psgmii after speed changed
*
* Power on the psgmii
*/
sw_error_t
dess_psgmii_poweron(a_uint32_t dev_id, a_uint32_t phy_id)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data |= PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4_POWER_ON_25M;

    rv = dess_psgmii_reg_set(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

/******************************************************************************
*
* psgmii_status - test to see if the specified psgmii link is alive
*
* RETURNS:
*    A_TRUE  --> link is alive
*    A_FALSE --> link is down
*/
a_bool_t
dess_psgmii_get_link_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_5 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data & PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_5_LINK_25M)
        return A_TRUE;
    else
        return A_FALSE;

    return SW_OK;
}

/******************************************************************************
*
* psgmii_set_loopback - set the psgmii loopback
*
*/
sw_error_t
dess_psgmii_set_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
                       a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (enable)
        data |= PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4_MR_MR_LOOPBACK_25M;
    else
        data &= (~PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4_MR_MR_LOOPBACK_25M);

    rv = dess_psgmii_reg_set(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

/******************************************************************************
*
* psgmii_get_loopback - get the psgmii loopback
*
*/
sw_error_t
dess_psgmii_get_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
                       a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (data & PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4_MR_MR_LOOPBACK_25M)
        *enable = A_TRUE;
    else
        *enable = A_FALSE;

    return SW_OK;
}

/******************************************************************************
*
* psgmii_enable_autonego - power off the psgmii to change its speed
*
* Power off the phy
*/
a_bool_t
dess_psgmii_autoneg_status(a_uint32_t dev_id, a_uint32_t phy_id)
{

    return A_FALSE;
}

/******************************************************************************
*
* psgmii_restart_autoneg - restart the psgmii autoneg
*
*/
sw_error_t
dess_psgmii_restart_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data |= PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4_MR_RESTART_AN_25M;

    rv = dess_psgmii_reg_set(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

/******************************************************************************
*
* psgmii_enable_autonego - power off the psgmii to change its speed
*
* Power off the phy
*/
sw_error_t
dess_psgmii_enable_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data |= PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4_MR_AN_ENABLE_25M;

    rv = dess_psgmii_reg_set(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}


/******************************************************************************
*
* psgmii_get_speed - Determines the speed of psgmii ports associated with the
* specified device.
*/

sw_error_t
dess_psgmii_get_speed(a_uint32_t dev_id, a_uint32_t phy_id,
                 fal_port_speed_t * speed)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    /*Force speed mode*/
    if (data & PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4_FORCE_SPEED_25M)
    {
	    switch (data & PSGMIIPHY_CHANNEL_3_INPUT_OUTPUT_4_SPEED_25M_MASK)
	    {
	        case PSGMIIPHY_CHANNEL_3_INPUT_OUTPUT_4_SPEED_25M_1000M:
	            *speed = FAL_SPEED_1000;
	            break;
	        case PSGMIIPHY_CHANNEL_3_INPUT_OUTPUT_4_SPEED_25M_100M:
	            *speed = FAL_SPEED_100;
	            break;
	        case PSGMIIPHY_CHANNEL_3_INPUT_OUTPUT_4_SPEED_25M_10M:
	            *speed = FAL_SPEED_10;
	            break;
	        default:
	            return SW_READ_ERROR;
	    }
    }
    else
    {
	    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_5 + phy_id * 0x18,
							(a_uint8_t *) (&data), sizeof (a_uint32_t));
	    SW_RTN_ON_ERROR(rv);
	    switch (data & PSGMIIPHY_CHANNEL_3_INPUT_OUTPUT_5_SPEED_MODE_25M)
	    {
	        case PSGMIIPHY_CHANNEL_3_INPUT_OUTPUT_5_SPEED_25M_1000M:
	            *speed = FAL_SPEED_1000;
	            break;
	        case PSGMIIPHY_CHANNEL_3_INPUT_OUTPUT_5_SPEED_25M_100M:
	            *speed = FAL_SPEED_100;
	            break;
	        case PSGMIIPHY_CHANNEL_3_INPUT_OUTPUT_5_SPEED_25M_10M:
	            *speed = FAL_SPEED_10;
	            break;
	        default:
	            return SW_READ_ERROR;
	    }
    }

    return SW_OK;
}

/******************************************************************************
*
* psgmii_set_speed - Determines the speed of psgmii ports associated with the
* specified device.
*/
sw_error_t
dess_psgmii_set_speed(a_uint32_t dev_id, a_uint32_t phy_id,
                 fal_port_speed_t speed)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data |= PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4_FORCE_SPEED_25M;
    data &= ~(PSGMIIPHY_CHANNEL_3_INPUT_OUTPUT_4_SPEED_25M_MASK);

    if (FAL_SPEED_1000 == speed)
    {
        data |= PSGMIIPHY_CHANNEL_3_INPUT_OUTPUT_4_SPEED_25M_1000M;
    }
    else if (FAL_SPEED_100 == speed)
    {
        data |= PSGMIIPHY_CHANNEL_3_INPUT_OUTPUT_4_SPEED_25M_100M;
    }
    else if (FAL_SPEED_10 == speed)
    {
        data |= PSGMIIPHY_CHANNEL_3_INPUT_OUTPUT_4_SPEED_25M_10M;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    rv = dess_psgmii_reg_set(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_4 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;

}

/******************************************************************************
*
* psgmii_get_duplex - Determines the speed of psgmii ports associated with the
* specified device.
*/
sw_error_t
dess_psgmii_get_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
                  fal_port_duplex_t * duplex)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_5 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    /*Force speed mode*/
    if (data & PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_5_DUPLEX_MODE_25M)
    {
	    *duplex = FAL_FULL_DUPLEX;
    }
    else
    {
	    *duplex = FAL_HALF_DUPLEX;
    }


    return SW_OK;
}

/******************************************************************************
*
*psgmii_set_duplex - Determines the speed of psgmii ports associated with the
* specified device.
*/
sw_error_t
dess_psgmii_set_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
                  fal_port_duplex_t duplex)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    rv = dess_psgmii_reg_get(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_5 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (FAL_FULL_DUPLEX == duplex)
    {
        data |= PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_5_FULL_DUPLEX_25M;
        data &= (~PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_5_HALF_DUPLEX_25M);
    }
    else
    {
        data |= PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_5_HALF_DUPLEX_25M;
        data &= (~PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_5_FULL_DUPLEX_25M);
    }

    rv = dess_psgmii_reg_set(dev_id, PSGMIIPHY_CHANNEL_0_INPUT_OUTPUT_5 + phy_id * 0x18,
						(a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

/******************************************************************************
*
* psgmii_init -
*
*/
a_bool_t
dess_psgmii_init(a_uint32_t dev_id, a_uint32_t phy_id,
            a_uint16_t org_id, a_uint16_t rev_id)
{

    return SW_OK;
}
