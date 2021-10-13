/*
 * Copyright (c) 2012, 2015-2018, The Linux Foundation. All rights reserved.
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
#include "f2_phy.h"
#include "hsl_phy.h"
#include "ssdk_plat.h"

static a_uint16_t
_phy_reg_read(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint8_t reg)
{
    sw_error_t rv;
    a_uint16_t val = 0;

    HSL_PHY_GET(rv, dev_id, phy_addr, reg, &val);
    if (SW_OK != rv)
        return 0xFFFF;

    return val;
}


static sw_error_t
_phy_reg_write(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint8_t reg,
               a_uint16_t val)
{
    sw_error_t rv;

    HSL_PHY_SET(rv, dev_id, phy_addr, reg, val);

    return rv;
}

/* #define f2_phy_reg_read _phy_reg_read */
/* #define f2_phy_reg_write _phy_reg_write */

/******************************************************************************
*
* f2_phy_mii_read - mii register read
*
* mil register read
*/
a_uint16_t
f2_phy_reg_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id)
{
 	return _phy_reg_read(dev_id, phy_id, reg_id);

}

/******************************************************************************
*
* f2_phy_reg_write - mii register write
*
* mii register write
*/
sw_error_t
f2_phy_reg_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id,
		       a_uint16_t reg_val)
{

       _phy_reg_write(dev_id,phy_id, reg_id, reg_val);

	return SW_OK;
}


/******************************************************************************
*
* f2_phy_debug_write - debug port write
*
* debug port write
*/
sw_error_t
f2_phy_debug_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id,
                   a_uint16_t reg_val)
{
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_ADDRESS, reg_id);
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_DATA, reg_val);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_debug_read - debug port read
*
* debug port read
*/
a_uint16_t
f2_phy_debug_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id)
{
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_ADDRESS, reg_id);
    return f2_phy_reg_read(dev_id, phy_id, F2_DEBUG_PORT_DATA);
}

/******************************************************************************
*
* f2_phy_set_powersave - set power saving status
*
* set power saving status
*/
sw_error_t
f2_phy_set_powersave(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
    a_uint16_t phy_data;
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_ADDRESS, 0x29);
    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_DEBUG_PORT_DATA);

    if(enable == A_TRUE)
    {
        phy_data |= 0x8000;
    }
    else
    {
        phy_data &= ~0x8000;
    }

    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_DATA, phy_data);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_get_powersave - get power saving status
*
* set power saving status
*/
sw_error_t
f2_phy_get_powersave(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t *enable)
{
    a_uint16_t phy_data;
    *enable = A_FALSE;

    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_ADDRESS, 0x29);
    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_DEBUG_PORT_DATA);

    if(phy_data & 0x8000)
        *enable =  A_TRUE;

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_set_hibernate - set hibernate status
*
* set hibernate status
*/
sw_error_t
f2_phy_set_hibernate(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
    a_uint16_t phy_data;
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_ADDRESS, 0xb);
    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_DEBUG_PORT_DATA);

    if(enable == A_TRUE)
    {
        phy_data |= 0x8000;
    }
    else
    {
        phy_data &= ~0x8000;
    }

    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_DATA, phy_data);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_get_hibernate - get hibernate status
*
* get hibernate status
*/
sw_error_t
f2_phy_get_hibernate(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t *enable)
{
    a_uint16_t phy_data;
    *enable = A_FALSE;

    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_ADDRESS, 0xb);
    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_DEBUG_PORT_DATA);

    if(phy_data & 0x8000)
        *enable =  A_TRUE;

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_cdt - cable diagnostic test
*
* cable diagnostic test
*/
sw_error_t
f2_phy_cdt(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t mdi_pair,
           fal_cable_status_t *cable_status, a_uint32_t *cable_len)
{
    a_uint16_t status = 0;
    a_uint16_t ii = 100;
    a_uint16_t cable_delta_time;

    if(!cable_status || !cable_len)
    {
        return SW_FAIL;
    }

    if(mdi_pair >= 2)
    {
        //There are only 4 mdi pairs in 1000BASE-T
        return SW_BAD_PARAM;
    }

    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CDT_CONTROL, (mdi_pair << 8) | 0x0001);

    do
    {
        aos_mdelay(30);
        status = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CDT_CONTROL);
    }
    while ((status & 0x0001) && (--ii));

    status = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CDT_STATUS);
    *cable_status = (status & 0x300) >> 8;//(00:normal  01:short 10:opened 11:invalid)

    /*the actual cable length equals to CableDeltaTime * 0.824*/
    cable_delta_time = status & 0xff;
    *cable_len = (cable_delta_time * 824) /1000;

    /*workaround*/
    if(*cable_len <= 2 && *cable_status == 1)
        *cable_status = 2;

    //f2_phy_reg_write(dev_id, phy_id, 0x00, 0x9000);  //Reset the PHY if necessary

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_reset_done - reset the phy
*
* reset the phy
*/
a_bool_t
f2_phy_reset_done(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;
    a_uint16_t ii = 200;

    do
    {
        phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);
        aos_mdelay(10);
    }
    while ((!F2_RESET_DONE(phy_data)) && --ii);

    if (ii == 0)
        return A_FALSE;

    return A_TRUE;
}

/******************************************************************************
*
* f2_autoneg_done
*
* f2_autoneg_done
*/
a_bool_t
f2_autoneg_done(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;
    a_uint16_t ii = 200;

    do
    {
        phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_STATUS);
        aos_mdelay(10);
    }
    while ((!F2_AUTONEG_DONE(phy_data)) && --ii);

    if (ii == 0)
        return A_FALSE;

    return A_TRUE;
}

/******************************************************************************
*
* f2_phy_Speed_Duplex_Resolved
 - reset the phy
*
* reset the phy
*/
a_bool_t
f2_phy_speed_duplex_resolved(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;
    a_uint16_t ii = 200;

    do
    {
        phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_SPEC_STATUS);
        aos_mdelay(10);
    }
    while ((!F2_SPEED_DUPLEX_RESOVLED(phy_data)) && --ii);

    if (ii == 0)
        return A_FALSE;

    return A_TRUE;
}

/******************************************************************************
*
* f2_phy_reset - reset the phy
*
* reset the phy
*/
sw_error_t
f2_phy_reset(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);
    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL,
                     phy_data | F2_CTRL_SOFTWARE_RESET);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_off - power off the phy to change its speed
*
* Power off the phy
*/
sw_error_t
f2_phy_poweroff(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);
    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL,
                     phy_data | F2_CTRL_POWER_DOWN);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_on - power on the phy after speed changed
*
* Power on the phy
*/
sw_error_t
f2_phy_poweron(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);
    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL,
                     phy_data & ~F2_CTRL_POWER_DOWN);

    aos_mdelay(200);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_get_ability - get the phy ability
*
*
*/
sw_error_t
f2_phy_get_ability(a_uint32_t dev_id, a_uint32_t phy_id,
                   a_uint16_t * ability)
{
    a_uint16_t phy_data;

    *ability = 0;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_STATUS);

    if (phy_data & F2_STATUS_AUTONEG_CAPS)
        *ability |= FAL_PHY_AUTONEG_CAPS;

    if (phy_data & F2_STATUS_100T2_HD_CAPS)
        *ability |= FAL_PHY_100T2_HD_CAPS;

    if (phy_data & F2_STATUS_100T2_FD_CAPS)
        *ability |= FAL_PHY_100T2_FD_CAPS;

    if (phy_data & F2_STATUS_10T_HD_CAPS)
        *ability |= FAL_PHY_10T_HD_CAPS;

    if (phy_data & F2_STATUS_10T_FD_CAPS)
        *ability |= FAL_PHY_10T_FD_CAPS;

    if (phy_data & F2_STATUS_100X_HD_CAPS)
        *ability |= FAL_PHY_100X_HD_CAPS;

    if (phy_data & F2_STATUS_100X_FD_CAPS)
        *ability |= FAL_PHY_100X_FD_CAPS;

    if (phy_data & F2_STATUS_100T4_CAPS)
        *ability |= FAL_PHY_100T4_CAPS;

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_get_ability - get the phy ability
*
*
*/
sw_error_t
f2_phy_get_partner_ability(a_uint32_t dev_id, a_uint32_t phy_id,
                           a_uint16_t * ability)
{
    a_uint16_t phy_data;

    *ability = 0;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_LINK_PARTNER_ABILITY);

    if (phy_data & F2_LINK_10BASETX_HALF_DUPLEX)
        *ability |= FAL_PHY_PART_10T_HD;

    if (phy_data & F2_LINK_10BASETX_FULL_DUPLEX)
        *ability |= FAL_PHY_PART_10T_FD;

    if (phy_data & F2_LINK_100BASETX_HALF_DUPLEX)
        *ability |= FAL_PHY_PART_100TX_HD;

    if (phy_data & F2_LINK_100BASETX_FULL_DUPLEX)
        *ability |= FAL_PHY_PART_100TX_FD;

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_status - test to see if the specified phy link is alive
*
* RETURNS:
*    A_TRUE  --> link is alive
*    A_FALSE --> link is down
*/
a_bool_t
f2_phy_get_link_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_STATUS);

    if (phy_data & F2_STATUS_LINK_STATUS_UP)
        return A_TRUE;

    return A_FALSE;
}

/******************************************************************************
*
* f2_set_autoneg_adv - set the phy autoneg Advertisement
*
*/
sw_error_t
f2_phy_set_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
                       a_uint32_t autoneg)
{
    a_uint16_t phy_data = 0;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_AUTONEG_ADVERT);
    phy_data &= ~F2_ADVERTISE_ALL;
    phy_data &= ~(F2_ADVERTISE_PAUSE | F2_ADVERTISE_ASYM_PAUSE);

    if (autoneg & FAL_PHY_ADV_100TX_FD)
        phy_data |= F2_ADVERTISE_100FULL;

    if (autoneg & FAL_PHY_ADV_100TX_HD)
        phy_data |= F2_ADVERTISE_100HALF;

    if (autoneg & FAL_PHY_ADV_10T_FD)
        phy_data |= F2_ADVERTISE_10FULL;

    if (autoneg & FAL_PHY_ADV_10T_HD)
        phy_data |= F2_ADVERTISE_10HALF;

    if (autoneg & FAL_PHY_ADV_PAUSE)
        phy_data |= F2_ADVERTISE_PAUSE;

    if (autoneg & FAL_PHY_ADV_ASY_PAUSE)
        phy_data |= F2_ADVERTISE_ASYM_PAUSE;

    f2_phy_reg_write(dev_id, phy_id, F2_AUTONEG_ADVERT, phy_data);

    return SW_OK;
}

/******************************************************************************
*
* f2_get_autoneg_adv - get the phy autoneg Advertisement
*
*/
sw_error_t
f2_phy_get_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
                       a_uint32_t * autoneg)
{
    a_uint16_t phy_data = 0;

    *autoneg = 0;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_AUTONEG_ADVERT);

    if (phy_data & F2_ADVERTISE_100FULL)
        *autoneg |= FAL_PHY_ADV_100TX_FD;

    if (phy_data & F2_ADVERTISE_100HALF)
        *autoneg |= FAL_PHY_ADV_100TX_HD;

    if (phy_data & F2_ADVERTISE_10FULL)
        *autoneg |= FAL_PHY_ADV_10T_FD;

    if (phy_data & F2_ADVERTISE_10HALF)
        *autoneg |= FAL_PHY_ADV_10T_HD;

    if (phy_data & F2_ADVERTISE_PAUSE)
        *autoneg |= FAL_PHY_ADV_PAUSE;

    if (phy_data & F2_ADVERTISE_ASYM_PAUSE)
        *autoneg |= FAL_PHY_ADV_ASY_PAUSE;

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_enable_autonego - power off the phy to change its speed
*
* Power off the phy
*/
a_bool_t
f2_phy_autoneg_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);

    if (phy_data & F2_CTRL_AUTONEGOTIATION_ENABLE)
        return A_TRUE;

    return A_FALSE;
}

/******************************************************************************
*
* f2_restart_autoneg - restart the phy autoneg
*
*/
sw_error_t
f2_phy_restart_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data = 0;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);

    phy_data |= F2_CTRL_AUTONEGOTIATION_ENABLE;
    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL,
                     phy_data | F2_CTRL_RESTART_AUTONEGOTIATION);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_enable_autonego - power off the phy to change its speed
*
* Power off the phy
*/
sw_error_t
f2_phy_enable_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data = 0;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);

    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL,
                     phy_data | F2_CTRL_AUTONEGOTIATION_ENABLE);

    return SW_OK;
}


/******************************************************************************
*
* f2_phy_get_speed - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*               AG7100_PHY_SPEED_10T, AG7100_PHY_SPEED_100TX;
*               AG7100_PHY_SPEED_1000T;
*/

sw_error_t
f2_phy_get_speed(a_uint32_t dev_id, a_uint32_t phy_id,
                 fal_port_speed_t * speed)
{
    a_uint16_t phy_data;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_SPEC_STATUS);

    //read speed
    switch (phy_data & F2_STATUS_SPEED_MASK)
    {
        case F2_STATUS_SPEED_1000MBS:
            *speed = FAL_SPEED_1000;
            break;
        case F2_STATUS_SPEED_100MBS:
            *speed = FAL_SPEED_100;
            break;
        case F2_STATUS_SPEED_10MBS:
            *speed = FAL_SPEED_10;
            break;
        default:
            return SW_READ_ERROR;
    }

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_set_speed - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*               AG7100_PHY_SPEED_10T, AG7100_PHY_SPEED_100TX;
*               AG7100_PHY_SPEED_1000T;
*/
sw_error_t
f2_phy_set_speed(a_uint32_t dev_id, a_uint32_t phy_id,
                 fal_port_speed_t speed)
{
    a_uint16_t phy_data = 0;
    a_uint16_t phy_status = 0;
    a_uint32_t autoneg, oldneg;
    fal_port_duplex_t old_duplex;

    phy_data &= ~F2_CTRL_AUTONEGOTIATION_ENABLE;

    (void)f2_phy_get_autoneg_adv(dev_id, phy_id, &autoneg);
    oldneg = autoneg;
    autoneg &= ~FAL_PHY_ADV_FE_SPEED_ALL;

    (void)f2_phy_get_duplex(dev_id, phy_id, &old_duplex);

    if (old_duplex == FAL_FULL_DUPLEX)
    {
        phy_data |= F2_CTRL_FULL_DUPLEX;

        if (speed == FAL_SPEED_100)
            autoneg |= FAL_PHY_ADV_100TX_FD;
        else
            autoneg |= FAL_PHY_ADV_10T_FD;
    }
    else if (old_duplex == FAL_HALF_DUPLEX)
    {
        phy_data &= ~F2_CTRL_FULL_DUPLEX;

        if (speed == FAL_SPEED_100)
            autoneg |= FAL_PHY_ADV_100TX_HD;
        else
            autoneg |= FAL_PHY_ADV_10T_HD;
    }
    else
        return SW_FAIL;

    (void)f2_phy_set_autoneg_adv(dev_id, phy_id, autoneg);
    (void)f2_phy_restart_autoneg(dev_id, phy_id);

    if(f2_phy_get_link_status(dev_id, phy_id))
    {
        do
        {
            phy_status = f2_phy_reg_read(dev_id, phy_id, F2_PHY_STATUS);
        }
        while(!F2_AUTONEG_DONE(phy_status));
    }

    if (speed == FAL_SPEED_100)
        phy_data |= F2_CTRL_SPEED_100;
    else if (speed == FAL_SPEED_10)
        phy_data |= F2_CTRL_SPEED_10;
    else
        return SW_BAD_PARAM;

    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL, phy_data);
    (void)f2_phy_set_autoneg_adv(dev_id, phy_id, oldneg);

    return SW_OK;

}

/******************************************************************************
*
* f2_phy_get_duplex - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*               AG7100_PHY_SPEED_10T, AG7100_PHY_SPEED_100TX;
*               AG7100_PHY_SPEED_1000T;
*/
sw_error_t
f2_phy_get_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
                  fal_port_duplex_t * duplex)
{
    a_uint16_t phy_data;

#if 0
    //a_uint16_t ii = 200;
    a_uint16_t ii = 2;

    if (phy_id >= F2_PHY_MAX)
        return SW_BAD_PARAM;

    do
    {
        phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_SPEC_STATUS);
        aos_mdelay(10);
    }
    while ((!(phy_data & F2_STATUS_RESOVLED)) && --ii);

    //read time out
    if (ii == 0)
        return SW_DISABLE;
#endif

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_SPEC_STATUS);

    //read duplex
    if (phy_data & F2_STATUS_FULL_DUPLEX)
        *duplex = FAL_FULL_DUPLEX;
    else
        *duplex = FAL_HALF_DUPLEX;

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_set_duplex - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*               AG7100_PHY_SPEED_10T, AG7100_PHY_SPEED_100TX;
*               AG7100_PHY_SPEED_1000T;
*/
sw_error_t
f2_phy_set_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
                  fal_port_duplex_t duplex)
{
    a_uint16_t phy_data = 0;
    a_uint16_t phy_status = 0;

    fal_port_speed_t old_speed = FAL_SPEED_10;
    a_uint32_t autoneg, oldneg;

    if (f2_phy_autoneg_status(dev_id, phy_id))
        phy_data &= ~F2_CTRL_AUTONEGOTIATION_ENABLE;

    (void)f2_phy_get_autoneg_adv(dev_id, phy_id, &autoneg);
    oldneg = autoneg;
    autoneg &= ~FAL_PHY_ADV_FE_SPEED_ALL;

    (void)f2_phy_get_speed(dev_id, phy_id, &old_speed);

    if (old_speed == FAL_SPEED_100)
        phy_data |= F2_CTRL_SPEED_100;
    else if (old_speed == FAL_SPEED_10)
        phy_data |= F2_CTRL_SPEED_10;
    else
        return SW_FAIL;

    if (duplex == FAL_FULL_DUPLEX)
    {
        phy_data |= F2_CTRL_FULL_DUPLEX;

        if (old_speed == FAL_SPEED_100)
            autoneg = FAL_PHY_ADV_100TX_FD;
        else
            autoneg = FAL_PHY_ADV_10T_FD;
    }
    else if (duplex == FAL_HALF_DUPLEX)
    {
        phy_data &= ~F2_CTRL_FULL_DUPLEX;

        if (old_speed == FAL_SPEED_100)
            autoneg = FAL_PHY_ADV_100TX_HD;
        else
            autoneg = FAL_PHY_ADV_10T_HD;
    }
    else
        return SW_BAD_PARAM;

    (void)f2_phy_set_autoneg_adv(dev_id, phy_id, autoneg);
    (void)f2_phy_restart_autoneg(dev_id, phy_id);

    if(f2_phy_get_link_status(dev_id, phy_id))
    {
        do
        {
            phy_status = f2_phy_reg_read(dev_id, phy_id, F2_PHY_STATUS);
        }
        while(!F2_AUTONEG_DONE(phy_status));
    }

    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL, phy_data);
    (void)f2_phy_set_autoneg_adv(dev_id, phy_id, oldneg);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_get_phy_id - get the phy id
*
*/
static sw_error_t
f2_phy_get_phy_id(a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *phy_data)
{
    a_uint16_t org_id, rev_id;

    org_id = f2_phy_reg_read(dev_id, phy_id, F2_PHY_ID1);
    rev_id = f2_phy_reg_read(dev_id, phy_id, F2_PHY_ID2);
    *phy_data = ((org_id & 0xffff) << 16) | (rev_id & 0xffff);

    return SW_OK;
}
static int f2_phy_api_ops_init(void)
{
	int ret;
	hsl_phy_ops_t *f2_phy_api_ops = NULL;

	f2_phy_api_ops = kzalloc(sizeof(hsl_phy_ops_t), GFP_KERNEL);
	if (f2_phy_api_ops == NULL) {
		SSDK_ERROR("f2 phy ops kzalloc failed!\n");
		return -ENOMEM;
	}

	phy_api_ops_init(F2_PHY_CHIP);

	f2_phy_api_ops->phy_hibernation_set = f2_phy_set_hibernate;
	f2_phy_api_ops->phy_hibernation_get = f2_phy_get_hibernate;
	f2_phy_api_ops->phy_speed_get = f2_phy_get_speed;
	f2_phy_api_ops->phy_speed_set = f2_phy_set_speed;
	f2_phy_api_ops->phy_duplex_get = f2_phy_get_duplex;
	f2_phy_api_ops->phy_duplex_set = f2_phy_set_duplex;
	f2_phy_api_ops->phy_autoneg_enable_set = f2_phy_enable_autoneg;
	f2_phy_api_ops->phy_restart_autoneg = f2_phy_restart_autoneg;
	f2_phy_api_ops->phy_autoneg_status_get = f2_phy_autoneg_status;
	f2_phy_api_ops->phy_autoneg_adv_set = f2_phy_set_autoneg_adv;
	f2_phy_api_ops->phy_autoneg_adv_get = f2_phy_get_autoneg_adv;
	f2_phy_api_ops->phy_powersave_set = f2_phy_set_powersave;
	f2_phy_api_ops->phy_powersave_get = f2_phy_get_powersave;
	f2_phy_api_ops->phy_cdt = f2_phy_cdt;
	f2_phy_api_ops->phy_link_status_get = f2_phy_get_link_status;
	f2_phy_api_ops->phy_reset = f2_phy_reset;
	f2_phy_api_ops->phy_power_off = f2_phy_poweroff;
	f2_phy_api_ops->phy_power_on = f2_phy_poweron;
	f2_phy_api_ops->phy_id_get = f2_phy_get_phy_id;
	f2_phy_api_ops->phy_reg_write = f2_phy_reg_write;
	f2_phy_api_ops->phy_reg_read = f2_phy_reg_read;
	f2_phy_api_ops->phy_debug_write = f2_phy_debug_write;
	f2_phy_api_ops->phy_debug_read = f2_phy_debug_read;

	ret = hsl_phy_api_ops_register(F2_PHY_CHIP, f2_phy_api_ops);

	if (ret == 0)
		SSDK_INFO("qca probe f2 phy driver succeeded!\n");
	else
		SSDK_ERROR("qca probe f2 phy driver failed! (code: %d)\n", ret);

	return ret;
}
int f2_phy_init(a_uint32_t dev_id, a_uint32_t port_bmp)
{
	static a_uint32_t phy_ops_flag = 0;

	if(phy_ops_flag == 0) {
		f2_phy_api_ops_init();
		phy_ops_flag = 1;
	}

	return 0;
}

