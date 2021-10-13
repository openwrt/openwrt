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
#include "f1_phy.h"
#include "hsl_phy.h"
#include "ssdk_plat.h"

static a_uint16_t
_phy_reg_read(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint32_t reg)
{
    sw_error_t rv;
    a_uint16_t val = 0;

    HSL_PHY_GET(rv, dev_id, phy_addr, reg, &val);
    if (SW_OK != rv)
        return 0xFFFF;

    return val;
}

static sw_error_t
_phy_reg_write(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint32_t reg,
               a_uint16_t val)
{
    sw_error_t rv;

    HSL_PHY_SET(rv, dev_id, phy_addr, reg, val);

    return rv;
}

/* #define f1_phy_reg_read _phy_reg_read */
/* #define f1_phy_reg_write _phy_reg_write */

/******************************************************************************
*
* f1_phy_mii_read - mii register read
*
* mil register read
*/
a_uint16_t
f1_phy_reg_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id)
{
 	return _phy_reg_read(dev_id, phy_id, reg_id);

}

/******************************************************************************
*
* f1_phy_reg_write - mii register write
*
* mii register write
*/
sw_error_t
f1_phy_reg_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id,
		       a_uint16_t reg_val)
{

       _phy_reg_write(dev_id,phy_id, reg_id, reg_val);

	return SW_OK;
}


/******************************************************************************
*
* f1_phy_debug_write - debug port write
*
* debug port write
*/
sw_error_t
f1_phy_debug_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id,
                   a_uint16_t reg_val)
{
    f1_phy_reg_write(dev_id, phy_id, F1_DEBUG_PORT_ADDRESS, reg_id);
    f1_phy_reg_write(dev_id, phy_id, F1_DEBUG_PORT_DATA, reg_val);

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_debug_read - debug port read
*
* debug port read
*/
a_uint16_t
f1_phy_debug_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id)
{
    f1_phy_reg_write(dev_id, phy_id, F1_DEBUG_PORT_ADDRESS, reg_id);
    return f1_phy_reg_read(dev_id, phy_id, F1_DEBUG_PORT_DATA);
}

/******************************************************************************
*
* f1_phy_mmd_write - PHY MMD register write
*
* PHY MMD register write
*/
sw_error_t
f1_phy_mmd_write(a_uint32_t dev_id, a_uint32_t phy_id,
                 a_uint16_t mmd_num,
                 a_uint16_t reg_id,
                 a_uint16_t reg_val)
{
    f1_phy_reg_write(dev_id, phy_id, F1_MMD_CTRL_REG, mmd_num);
    f1_phy_reg_write(dev_id, phy_id, F1_MMD_DATA_REG, reg_id);
    f1_phy_reg_write(dev_id, phy_id, F1_MMD_CTRL_REG, 0x4000|mmd_num);
    f1_phy_reg_write(dev_id, phy_id, F1_MMD_DATA_REG, reg_val);

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_mmd_read -  PHY MMD register read
*
* PHY MMD register read
*/
a_uint16_t
f1_phy_mmd_read(a_uint32_t dev_id, a_uint32_t phy_id,
                  a_uint16_t mmd_num,
                  a_uint16_t reg_id)
{
    f1_phy_reg_write(dev_id, phy_id, F1_MMD_CTRL_REG, mmd_num);
    f1_phy_reg_write(dev_id, phy_id, F1_MMD_DATA_REG, reg_id);
    f1_phy_reg_write(dev_id, phy_id, F1_MMD_CTRL_REG, 0x4000|mmd_num);

    return f1_phy_reg_read(dev_id, phy_id, F1_MMD_DATA_REG);
}


/******************************************************************************
*
* f1_phy_set_powersave - set power saving status
*
* set power saving status
*/
sw_error_t
f1_phy_set_powersave(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
    a_uint16_t phy_data;
    f1_phy_reg_write(dev_id, phy_id, F1_DEBUG_PORT_ADDRESS, 0x29);
    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_DEBUG_PORT_DATA);

    if(enable == A_TRUE)
    {
        phy_data |= 0x8000;
    }
    else
    {
        phy_data &= ~0x8000;
    }

    f1_phy_reg_write(dev_id, phy_id, F1_DEBUG_PORT_DATA, phy_data);

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_get_powersave - get power saving status
*
* set power saving status
*/
sw_error_t
f1_phy_get_powersave(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t *enable)
{
    a_uint16_t phy_data;
    *enable = A_FALSE;

    f1_phy_reg_write(dev_id, phy_id, F1_DEBUG_PORT_ADDRESS, 0x29);
    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_DEBUG_PORT_DATA);

    if(phy_data & 0x8000)
        *enable =  A_TRUE;

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_set_hibernate - set hibernate status
*
* set hibernate status
*/
sw_error_t
f1_phy_set_hibernate(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
    a_uint16_t phy_data;
    f1_phy_reg_write(dev_id, phy_id, F1_DEBUG_PORT_ADDRESS, 0xb);
    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_DEBUG_PORT_DATA);

    if(enable == A_TRUE)
    {
        phy_data |= 0x8000;
    }
    else
    {
        phy_data &= ~0x8000;
    }

    f1_phy_reg_write(dev_id, phy_id, F1_DEBUG_PORT_DATA, phy_data);

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_get_hibernate - get hibernate status
*
* get hibernate status
*/
sw_error_t
f1_phy_get_hibernate(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t *enable)
{
    a_uint16_t phy_data;
    *enable = A_FALSE;

    f1_phy_reg_write(dev_id, phy_id, F1_DEBUG_PORT_ADDRESS, 0xb);
    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_DEBUG_PORT_DATA);

    if(phy_data & 0x8000)
        *enable =  A_TRUE;

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_cdt - cable diagnostic test
*
* cable diagnostic test
*/
#ifdef ISISC
#define RUN_CDT 0x8000
#define CABLE_LENGTH_UNIT 0x0400
sw_error_t f1_phy_reset(a_uint32_t dev_id, a_uint32_t phy_id);
a_bool_t f1_phy_reset_done(a_uint32_t dev_id, a_uint32_t phy_id);
a_bool_t f1_phy_get_link_status(a_uint32_t dev_id, a_uint32_t phy_id);

static inline fal_cable_status_t
_fal_cdt_status_mapping(a_uint16_t status)
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

static sw_error_t
f1_phy_cdt_start(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t status = 0;
    a_uint16_t ii = 100;

    /* RUN CDT */
    f1_phy_reg_write(dev_id, phy_id, F1_PHY_CDT_CONTROL, RUN_CDT|CABLE_LENGTH_UNIT);
    do
    {
        aos_mdelay(30);
        status = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CDT_CONTROL);
    }
    while ((status & RUN_CDT) && (--ii));

    return SW_OK;
}

sw_error_t
f1_phy_cdt_get(a_uint32_t dev_id, a_uint32_t phy_id, fal_port_cdt_t *port_cdt)
{
    a_uint16_t status = 0;
    a_uint16_t cable_delta_time = 0;
    a_uint16_t org_debug_value = 0;
    int ii = 100;
    a_bool_t link_st = A_FALSE;
    a_uint16_t reg806e = 0;
    int i;

    if((!port_cdt) || (phy_id > 4))
    {
        return SW_FAIL;
    }

    /*disable clock gating*/
    org_debug_value = f1_phy_debug_read(dev_id, phy_id, 0x3f);
    f1_phy_debug_write(dev_id, phy_id, 0x3f, 0);

    f1_phy_cdt_start(dev_id, phy_id);

    /* Get cable status */
    status = f1_phy_mmd_read(dev_id, phy_id, 3, 0x8064);

    /* Workaround for cable lenth less than 20M  */
    port_cdt->pair_c_status = (status >> 4) & 0x3;
    /* Get Cable Length value */
    cable_delta_time = f1_phy_mmd_read(dev_id, phy_id, 3, 0x8067);
    /* the actual cable length equals to CableDeltaTime * 0.824*/
    port_cdt->pair_c_len = (cable_delta_time * 824) /1000;
    if ((1 == port_cdt->pair_c_status) &&
        (port_cdt->pair_c_len > 0) && (port_cdt->pair_c_len <= 20))
    {
        reg806e = f1_phy_mmd_read(dev_id, phy_id, 3, 0x806e);
	f1_phy_mmd_write(dev_id, phy_id, 3, 0x806e, reg806e & (~0x8000));

        f1_phy_reset(dev_id, phy_id);
        f1_phy_reset_done(dev_id, phy_id);
	do
        {
            link_st = f1_phy_get_link_status(dev_id, phy_id);
	    aos_mdelay(100);
        } while ((A_FALSE == link_st) && (--ii));

	f1_phy_cdt_start(dev_id, phy_id);
        /* Get cable status */
        status = f1_phy_mmd_read(dev_id, phy_id, 3, 0x8064);
    }

    for (i=0;i<4;i++)
    {
        switch(i)
        {
            case 0:
                port_cdt->pair_a_status = (status >> 12) & 0x3;
                /* Get Cable Length value */
                cable_delta_time = f1_phy_mmd_read(dev_id, phy_id, 3, 0x8065);
                /* the actual cable length equals to CableDeltaTime * 0.824*/
                port_cdt->pair_a_len = (cable_delta_time * 824) /1000;

                break;
            case 1:
                port_cdt->pair_b_status = (status >> 8) & 0x3;
                /* Get Cable Length value */
                cable_delta_time = f1_phy_mmd_read(dev_id, phy_id, 3, 0x8066);
                /* the actual cable length equals to CableDeltaTime * 0.824*/
                port_cdt->pair_b_len = (cable_delta_time * 824) /1000;
                break;
            case 2:
                port_cdt->pair_c_status = (status >> 4) & 0x3;
                /* Get Cable Length value */
                cable_delta_time = f1_phy_mmd_read(dev_id, phy_id, 3, 0x8067);
                /* the actual cable length equals to CableDeltaTime * 0.824*/
                port_cdt->pair_c_len = (cable_delta_time * 824) /1000;
                break;
            case 3:
                port_cdt->pair_d_status = status & 0x3;
                /* Get Cable Length value */
                cable_delta_time = f1_phy_mmd_read(dev_id, phy_id, 3, 0x8068);
                /* the actual cable length equals to CableDeltaTime * 0.824*/
                port_cdt->pair_d_len = (cable_delta_time * 824) /1000;
                break;
            default:
                break;
        }
    }

    /*restore debug port value*/
    f1_phy_debug_write(dev_id, phy_id, 0x3f, org_debug_value);

    return SW_OK;
}

sw_error_t
f1_phy_cdt(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t mdi_pair,
           fal_cable_status_t *cable_status, a_uint32_t *cable_len)
{
    fal_port_cdt_t f1_port_cdt;

    if((mdi_pair >= 4) || (phy_id > 4))
    {
        //There are only 4 mdi pairs in 1000BASE-T
        return SW_BAD_PARAM;
    }

    f1_phy_cdt_get(dev_id, phy_id, &f1_port_cdt);

    switch(mdi_pair)
    {
        case 0:
            *cable_status = _fal_cdt_status_mapping(f1_port_cdt.pair_a_status);
            /* Get Cable Length value */
	    *cable_len = f1_port_cdt.pair_a_len;
            break;
        case 1:
            *cable_status = _fal_cdt_status_mapping(f1_port_cdt.pair_b_status);
            /* Get Cable Length value */
	    *cable_len = f1_port_cdt.pair_b_len;
            break;
        case 2:
            *cable_status = _fal_cdt_status_mapping(f1_port_cdt.pair_c_status);
            /* Get Cable Length value */
	    *cable_len = f1_port_cdt.pair_c_len;
            break;
        case 3:
            *cable_status = _fal_cdt_status_mapping(f1_port_cdt.pair_d_status);
            /* Get Cable Length value */
	    *cable_len = f1_port_cdt.pair_d_len;
            break;
        default:
            break;
    }

    return SW_OK;
}
#else
sw_error_t
f1_phy_cdt(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t mdi_pair,
           fal_cable_status_t *cable_status, a_uint32_t *cable_len)
{
    a_uint16_t status = 0;
    a_uint16_t ii = 100;
    a_uint16_t org_debug_value;
    a_uint16_t cable_delta_time;

    if(!cable_status || !cable_len)
    {
        return SW_FAIL;
    }

    if(mdi_pair >= 4)
    {
        //There are only 4 mdi pairs in 1000BASE-T
        return SW_BAD_PARAM;
    }

    org_debug_value = f1_phy_debug_read(dev_id, phy_id, 0x3f);

    /*disable clock gating*/
    f1_phy_debug_write(dev_id, phy_id, 0x3f, 0);
    f1_phy_reg_write(dev_id, phy_id, F1_PHY_CDT_CONTROL, (mdi_pair << 8) | 0x0001);

    do
    {
        aos_mdelay(30);
        status = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CDT_CONTROL);
    }
    while ((status & 0x0001) && (--ii));

    status = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CDT_STATUS);

    *cable_status = (status&0x300) >> 8;
    if ( (*cable_status == 1) || (*cable_status == 2))
    {
        if ( mdi_pair == 1 || mdi_pair == 3 )
        {
            /*Reverse the mdi status for channel 1 and channel 3*/
            *cable_status = (~(*cable_status)) & 0x3;
        }
    }

    /* the actual cable length equals to CableDeltaTime * 0.824*/
     cable_delta_time = status & 0xff;
    *cable_len = (cable_delta_time * 824) /1000;

    /*restore debug port value*/
    f1_phy_debug_write(dev_id, phy_id, 0x3f, org_debug_value);
    //f1_phy_reg_write(dev_id, phy_id, 0x00, 0x9000);//Reset the PHY if necessary

    return SW_OK;
}
#endif

/******************************************************************************
*
* f1_phy_reset_done - reset the phy
*
* reset the phy
*/
a_bool_t
f1_phy_reset_done(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;
    a_uint16_t ii = 200;

    do
    {
        phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CONTROL);
        aos_mdelay(10);
    }
    while ((!F1_RESET_DONE(phy_data)) && --ii);

    if (ii == 0)
        return A_FALSE;

    return A_TRUE;
}

/******************************************************************************
*
* f1_autoneg_done
*
* f1_autoneg_done
*/
a_bool_t
f1_autoneg_done(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;
    a_uint16_t ii = 200;

    do
    {
        phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_STATUS);
        aos_mdelay(10);
    }
    while ((!F1_AUTONEG_DONE(phy_data)) && --ii);

    if (ii == 0)
        return A_FALSE;

    return A_TRUE;
}

/******************************************************************************
*
* f1_phy_Speed_Duplex_Resolved
 - reset the phy
*
* reset the phy
*/
a_bool_t
f1_phy_speed_duplex_resolved(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;
    a_uint16_t ii = 200;

    do
    {
        phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_SPEC_STATUS);
        aos_mdelay(10);
    }
    while ((!F1_SPEED_DUPLEX_RESOVLED(phy_data)) && --ii);

    if (ii == 0)
        return A_FALSE;

    return A_TRUE;
}

/******************************************************************************
*
* f1_phy_reset - reset the phy
*
* reset the phy
*/
sw_error_t
f1_phy_reset(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CONTROL);
    f1_phy_reg_write(dev_id, phy_id, F1_PHY_CONTROL,
                     phy_data | F1_CTRL_SOFTWARE_RESET);

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_off - power off the phy to change its speed
*
* Power off the phy
*/
sw_error_t
f1_phy_poweroff(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CONTROL);
    f1_phy_reg_write(dev_id, phy_id, F1_PHY_CONTROL,
                     phy_data | F1_CTRL_POWER_DOWN);

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_on - power on the phy after speed changed
*
* Power on the phy
*/
sw_error_t
f1_phy_poweron(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CONTROL);
    f1_phy_reg_write(dev_id, phy_id, F1_PHY_CONTROL,
                     phy_data & ~F1_CTRL_POWER_DOWN);

    aos_mdelay(200);

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_get_ability - get the phy ability
*
*
*/
sw_error_t
f1_phy_get_ability(a_uint32_t dev_id, a_uint32_t phy_id,
                   a_uint32_t * ability)
{
    a_uint16_t phy_data;

    *ability = 0;
    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_STATUS);

    if (phy_data & F1_STATUS_AUTONEG_CAPS)
        *ability |= FAL_PHY_AUTONEG_CAPS;

    if (phy_data & F1_STATUS_100T2_HD_CAPS)
        *ability |= FAL_PHY_100T2_HD_CAPS;

    if (phy_data & F1_STATUS_100T2_FD_CAPS)
        *ability |= FAL_PHY_100T2_FD_CAPS;

    if (phy_data & F1_STATUS_10T_HD_CAPS)
        *ability |= FAL_PHY_10T_HD_CAPS;

    if (phy_data & F1_STATUS_10T_FD_CAPS)
        *ability |= FAL_PHY_10T_FD_CAPS;

    if (phy_data & F1_STATUS_100X_HD_CAPS)
        *ability |= FAL_PHY_100X_HD_CAPS;

    if (phy_data & F1_STATUS_100X_FD_CAPS)
        *ability |= FAL_PHY_100X_FD_CAPS;

    if (phy_data & F1_STATUS_100T4_CAPS)
        *ability |= FAL_PHY_100T4_CAPS;

    if (phy_data & F1_STATUS_EXTENDED_STATUS)
    {
        phy_data = f1_phy_reg_read(dev_id, phy_id, F1_EXTENDED_STATUS);

        if (phy_data & F1_STATUS_1000T_FD_CAPS)
        {
            *ability |= FAL_PHY_1000T_FD_CAPS;
        }

        if (phy_data & F1_STATUS_1000X_FD_CAPS)
        {
            *ability |= FAL_PHY_1000X_FD_CAPS;
        }
    }

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_get_ability - get the phy ability
*
*
*/
sw_error_t
f1_phy_get_partner_ability(a_uint32_t dev_id, a_uint32_t phy_id,
                           a_uint32_t * ability)
{
    a_uint16_t phy_data;

    *ability = 0;
    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_LINK_PARTNER_ABILITY);

    if (phy_data & F1_LINK_10BASETX_HALF_DUPLEX)
        *ability |= FAL_PHY_PART_10T_HD;

    if (phy_data & F1_LINK_10BASETX_FULL_DUPLEX)
        *ability |= FAL_PHY_PART_10T_FD;

    if (phy_data & F1_LINK_100BASETX_HALF_DUPLEX)
        *ability |= FAL_PHY_PART_100TX_HD;

    if (phy_data & F1_LINK_100BASETX_FULL_DUPLEX)
        *ability |= FAL_PHY_PART_100TX_FD;

    if (phy_data & F1_LINK_NPAGE)
    {
        phy_data = f1_phy_reg_read(dev_id, phy_id, F1_1000BASET_STATUS);

        if (phy_data & F1_LINK_1000BASETX_FULL_DUPLEX)
            *ability |= FAL_PHY_PART_1000T_FD;
    }

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_status - test to see if the specified phy link is alive
*
* RETURNS:
*    A_TRUE  --> link is alive
*    A_FALSE --> link is down
*/
a_bool_t
f1_phy_get_link_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_SPEC_STATUS);

    if (phy_data & F1_STATUS_LINK_PASS)
        return A_TRUE;

    return A_FALSE;
}

/******************************************************************************
*
* f1_set_autoneg_adv - set the phy autoneg Advertisement
*
*/
sw_error_t
f1_phy_set_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
                       a_uint32_t autoneg)
{
    a_uint16_t phy_data = 0;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_AUTONEG_ADVERT);
    phy_data &= ~F1_ADVERTISE_MEGA_ALL;
    phy_data &= ~(F1_ADVERTISE_PAUSE | F1_ADVERTISE_ASYM_PAUSE);

    if (autoneg & FAL_PHY_ADV_100TX_FD)
        phy_data |= F1_ADVERTISE_100FULL;

    if (autoneg & FAL_PHY_ADV_100TX_HD)
        phy_data |= F1_ADVERTISE_100HALF;

    if (autoneg & FAL_PHY_ADV_10T_FD)
        phy_data |= F1_ADVERTISE_10FULL;

    if (autoneg & FAL_PHY_ADV_10T_HD)
        phy_data |= F1_ADVERTISE_10HALF;

    if (autoneg & FAL_PHY_ADV_PAUSE)
        phy_data |= F1_ADVERTISE_PAUSE;

    if (autoneg & FAL_PHY_ADV_ASY_PAUSE)
        phy_data |= F1_ADVERTISE_ASYM_PAUSE;

    f1_phy_reg_write(dev_id, phy_id, F1_AUTONEG_ADVERT, phy_data);

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_1000BASET_CONTROL);
    phy_data &= ~F1_ADVERTISE_1000FULL;
    phy_data &= ~F1_ADVERTISE_1000HALF;

    if (autoneg & FAL_PHY_ADV_1000T_FD)
        phy_data |= F1_ADVERTISE_1000FULL;

    f1_phy_reg_write(dev_id, phy_id, F1_1000BASET_CONTROL, phy_data);

    return SW_OK;
}

/******************************************************************************
*
* f1_get_autoneg_adv - get the phy autoneg Advertisement
*
*/
sw_error_t
f1_phy_get_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
                       a_uint32_t * autoneg)
{
    a_uint16_t phy_data = 0;

    *autoneg = 0;
    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_AUTONEG_ADVERT);

    if (phy_data & F1_ADVERTISE_100FULL)
        *autoneg |= FAL_PHY_ADV_100TX_FD;

    if (phy_data & F1_ADVERTISE_100HALF)
        *autoneg |= FAL_PHY_ADV_100TX_HD;

    if (phy_data & F1_ADVERTISE_10FULL)
        *autoneg |= FAL_PHY_ADV_10T_FD;

    if (phy_data & F1_ADVERTISE_10HALF)
        *autoneg |= FAL_PHY_ADV_10T_HD;

    if (phy_data & F1_ADVERTISE_PAUSE)
        *autoneg |= FAL_PHY_ADV_PAUSE;

    if (phy_data & F1_ADVERTISE_ASYM_PAUSE)
        *autoneg |= FAL_PHY_ADV_ASY_PAUSE;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_1000BASET_CONTROL);

    if (phy_data & F1_ADVERTISE_1000FULL)
        *autoneg |= FAL_PHY_ADV_1000T_FD;

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_enable_autonego - power off the phy to change its speed
*
* Power off the phy
*/
a_bool_t
f1_phy_autoneg_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CONTROL);

    if (phy_data & F1_CTRL_AUTONEGOTIATION_ENABLE)
        return A_TRUE;

    return A_FALSE;
}

/******************************************************************************
*
* f1_restart_autoneg - restart the phy autoneg
*
*/
sw_error_t
f1_phy_restart_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data = 0;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CONTROL);

    phy_data |= F1_CTRL_AUTONEGOTIATION_ENABLE;
    f1_phy_reg_write(dev_id, phy_id, F1_PHY_CONTROL,
                     phy_data | F1_CTRL_RESTART_AUTONEGOTIATION);

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_enable_autonego - power off the phy to change its speed
*
* Power off the phy
*/
sw_error_t
f1_phy_enable_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data = 0;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CONTROL);

    f1_phy_reg_write(dev_id, phy_id, F1_PHY_CONTROL,
                     phy_data | F1_CTRL_AUTONEGOTIATION_ENABLE);

    return SW_OK;
}


/******************************************************************************
*
* f1_phy_get_speed - Determines the speed of phy ports associated with the
* specified device.
*/

sw_error_t
f1_phy_get_speed(a_uint32_t dev_id, a_uint32_t phy_id,
                 fal_port_speed_t * speed)
{
	a_uint16_t phy_data;
	a_bool_t auto_neg;

	auto_neg = f1_phy_autoneg_status(dev_id, phy_id);
	if (A_TRUE == auto_neg ) {
		phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_SPEC_STATUS);
		switch (phy_data & F1_STATUS_SPEED_MASK)
    		{
			case F1_STATUS_SPEED_1000MBS:
				*speed = FAL_SPEED_1000;
				break;
			case F1_STATUS_SPEED_100MBS:
				*speed = FAL_SPEED_100;
				break;
			case F1_STATUS_SPEED_10MBS:
				*speed = FAL_SPEED_10;
				break;
			default:
				return SW_READ_ERROR;
		}
	}
	else
	{
		phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CONTROL);
		switch (phy_data & F1_CTRL_SPEED_MASK)
    		{
			case F1_CTRL_SPEED_1000:
				*speed = FAL_SPEED_1000;
				break;
			case F1_CTRL_SPEED_100:
				*speed = FAL_SPEED_100;
				break;
			case F1_CTRL_SPEED_10:
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
* f1_phy_set_speed - Determines the speed of phy ports associated with the
* specified device.
*/
sw_error_t
f1_phy_set_speed(a_uint32_t dev_id, a_uint32_t phy_id,
                 fal_port_speed_t speed)
{
    a_uint16_t phy_data = 0;
    a_uint16_t phy_status = 0;

    a_uint32_t autoneg, oldneg;
    fal_port_duplex_t old_duplex;

    if (FAL_SPEED_1000 == speed)
    {
        phy_data |= F1_CTRL_SPEED_1000;
	phy_data |= F1_CTRL_AUTONEGOTIATION_ENABLE;
    }
    else if (FAL_SPEED_100 == speed)
    {
        phy_data |= F1_CTRL_SPEED_100;
	phy_data &= ~F1_CTRL_AUTONEGOTIATION_ENABLE;
    }
    else if (FAL_SPEED_10 == speed)
    {
        phy_data |= F1_CTRL_SPEED_10;
	phy_data &= ~F1_CTRL_AUTONEGOTIATION_ENABLE;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    (void)f1_phy_get_autoneg_adv(dev_id, phy_id, &autoneg);
    oldneg = autoneg;
    autoneg &= ~FAL_PHY_ADV_GE_SPEED_ALL;

    (void)f1_phy_get_duplex(dev_id, phy_id, &old_duplex);

    if (old_duplex == FAL_FULL_DUPLEX)
    {
        phy_data |= F1_CTRL_FULL_DUPLEX;

        if (FAL_SPEED_1000 == speed)
        {
            autoneg |= FAL_PHY_ADV_1000T_FD;
        }
        else if (FAL_SPEED_100 == speed)
        {
            autoneg |= FAL_PHY_ADV_100TX_FD;
        }
        else
        {
            autoneg |= FAL_PHY_ADV_10T_FD;
        }
    }
    else if (old_duplex == FAL_HALF_DUPLEX)
    {
        phy_data &= ~F1_CTRL_FULL_DUPLEX;

        if (FAL_SPEED_100 == speed)
        {
            autoneg |= FAL_PHY_ADV_100TX_HD;
        }
        else
        {
            autoneg |= FAL_PHY_ADV_10T_HD;
        }
    }
    else
    {
        return SW_FAIL;
    }

    (void)f1_phy_set_autoneg_adv(dev_id, phy_id, autoneg);
    (void)f1_phy_restart_autoneg(dev_id, phy_id);
    if(f1_phy_get_link_status(dev_id, phy_id))
    {
        do
        {
            phy_status = f1_phy_reg_read(dev_id, phy_id, F1_PHY_STATUS);
        }
        while(!F1_AUTONEG_DONE(phy_status));
    }

    f1_phy_reg_write(dev_id, phy_id, F1_PHY_CONTROL, phy_data);
    (void)f1_phy_set_autoneg_adv(dev_id, phy_id, oldneg);

    return SW_OK;

}

/******************************************************************************
*
* f1_phy_get_duplex - Determines the speed of phy ports associated with the
* specified device.
*/
sw_error_t
f1_phy_get_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
                  fal_port_duplex_t * duplex)
{
	a_uint16_t phy_data;
	a_bool_t auto_neg;

	auto_neg = f1_phy_autoneg_status(dev_id, phy_id);
	if (A_TRUE == auto_neg ) {
		phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_SPEC_STATUS);

		//read duplex
		if (phy_data & F1_STATUS_FULL_DUPLEX)
			*duplex = FAL_FULL_DUPLEX;
		else
			*duplex = FAL_HALF_DUPLEX;
	}
	else
	{
		phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CONTROL);
		//read duplex
		if (phy_data & F1_CTRL_FULL_DUPLEX)
			*duplex = FAL_FULL_DUPLEX;
		else
			*duplex = FAL_HALF_DUPLEX;
	}
	return SW_OK;
}

/******************************************************************************
*
* f1_phy_set_duplex - Determines the speed of phy ports associated with the
* specified device.
*/
sw_error_t
f1_phy_set_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
                  fal_port_duplex_t duplex)
{
    a_uint16_t phy_data = 0;
    a_uint16_t phy_status = 0;

    fal_port_speed_t old_speed = FAL_SPEED_10;
    a_uint32_t oldneg, autoneg;

    if (A_TRUE == f1_phy_autoneg_status(dev_id, phy_id))
        phy_data &= ~F1_CTRL_AUTONEGOTIATION_ENABLE;

    (void)f1_phy_get_autoneg_adv(dev_id, phy_id, &autoneg);
    oldneg = autoneg;
    autoneg &= ~FAL_PHY_ADV_GE_SPEED_ALL;
    (void)f1_phy_get_speed(dev_id, phy_id, &old_speed);

    if (FAL_SPEED_1000 == old_speed)
    {
        phy_data |= F1_CTRL_SPEED_1000;
        phy_data |= F1_CTRL_AUTONEGOTIATION_ENABLE;
    }
    else if (FAL_SPEED_100 == old_speed)
    {
        phy_data |= F1_CTRL_SPEED_100;
    }
    else if (FAL_SPEED_10 == old_speed)
    {
        phy_data |= F1_CTRL_SPEED_10;
    }
    else
    {
        return SW_FAIL;
    }

    if (duplex == FAL_FULL_DUPLEX)
    {
        phy_data |= F1_CTRL_FULL_DUPLEX;

        if (FAL_SPEED_1000 == old_speed)
        {
            autoneg = FAL_PHY_ADV_1000T_FD;
        }
        else if (FAL_SPEED_100 == old_speed)
        {
            autoneg = FAL_PHY_ADV_100TX_FD;
        }
        else
        {
            autoneg = FAL_PHY_ADV_10T_FD;
        }
    }
    else if (duplex == FAL_HALF_DUPLEX)
    {
        phy_data &= ~F1_CTRL_FULL_DUPLEX;

        if (FAL_SPEED_100 == old_speed)
        {
            autoneg = FAL_PHY_ADV_100TX_HD;
        }
        else
        {
            autoneg = FAL_PHY_ADV_10T_HD;
        }
    }
    else
    {
        return SW_BAD_PARAM;
    }

    (void)f1_phy_set_autoneg_adv(dev_id, phy_id, autoneg);
    (void)f1_phy_restart_autoneg(dev_id, phy_id);
    if(f1_phy_get_link_status(dev_id, phy_id))
    {
        do
        {
            phy_status = f1_phy_reg_read(dev_id, phy_id, F1_PHY_STATUS);
        }
        while(!F1_AUTONEG_DONE(phy_status));
    }

    f1_phy_reg_write(dev_id, phy_id, F1_PHY_CONTROL, phy_data);
    (void)f1_phy_set_autoneg_adv(dev_id, phy_id, oldneg);

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_get_phy_id - get the phy id
*
*/
static sw_error_t
f1_phy_get_phy_id(a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *phy_data)
{
    a_uint16_t org_id, rev_id;

    org_id = f1_phy_reg_read(dev_id, phy_id, F1_PHY_ID1);
    rev_id = f1_phy_reg_read(dev_id, phy_id, F1_PHY_ID2);
    *phy_data = ((org_id & 0xffff) << 16) | (rev_id & 0xffff);

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_intr_mask_set - Set interrupt mask with the
* specified device.
*/
sw_error_t
f1_phy_intr_mask_set(a_uint32_t dev_id, a_uint32_t phy_id,
                     a_uint32_t intr_mask_flag)
{
    a_uint16_t phy_data = 0;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_INTR_MASK);

    if (FAL_PHY_INTR_STATUS_UP_CHANGE & intr_mask_flag)
    {
        phy_data |= F1_INTR_STATUS_UP_CHANGE;
    }
    else
    {
        phy_data &= (~F1_INTR_STATUS_UP_CHANGE);
    }

    if (FAL_PHY_INTR_STATUS_DOWN_CHANGE & intr_mask_flag)
    {
        phy_data |= F1_INTR_STATUS_DOWN_CHANGE;
    }
    else
    {
        phy_data &= (~F1_INTR_STATUS_DOWN_CHANGE);
    }

    if (FAL_PHY_INTR_SPEED_CHANGE & intr_mask_flag)
    {
        phy_data |= F1_INTR_SPEED_CHANGE;
    }
    else
    {
        phy_data &= (~F1_INTR_SPEED_CHANGE);
    }

    if (FAL_PHY_INTR_DUPLEX_CHANGE & intr_mask_flag)
    {
        phy_data |= F1_INTR_DUPLEX_CHANGE;
    }
    else
    {
        phy_data &= (~F1_INTR_DUPLEX_CHANGE);
    }

    f1_phy_reg_write(dev_id, phy_id, F1_PHY_INTR_MASK, phy_data);
    return SW_OK;
}

/******************************************************************************
*
* f1_phy_intr_mask_get - Get interrupt mask with the
* specified device.
*/
sw_error_t
f1_phy_intr_mask_get(a_uint32_t dev_id, a_uint32_t phy_id,
                     a_uint32_t * intr_mask_flag)
{
    a_uint16_t phy_data = 0;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_INTR_MASK);

    *intr_mask_flag = 0;
    if (F1_INTR_STATUS_UP_CHANGE & phy_data)
    {
        *intr_mask_flag |= FAL_PHY_INTR_STATUS_UP_CHANGE;
    }

    if (F1_INTR_STATUS_DOWN_CHANGE & phy_data)
    {
        *intr_mask_flag |= FAL_PHY_INTR_STATUS_DOWN_CHANGE;
    }

    if (F1_INTR_SPEED_CHANGE & phy_data)
    {
        *intr_mask_flag |= FAL_PHY_INTR_SPEED_CHANGE;
    }

    if (F1_INTR_DUPLEX_CHANGE & phy_data)
    {
        *intr_mask_flag |= FAL_PHY_INTR_DUPLEX_CHANGE;
    }

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_intr_status_get - Get interrupt status with the
* specified device.
*/
sw_error_t
f1_phy_intr_status_get(a_uint32_t dev_id, a_uint32_t phy_id,
                       a_uint32_t * intr_status_flag)
{
    a_uint16_t phy_data = 0;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_INTR_STATUS);

    *intr_status_flag = 0;
    if (F1_INTR_STATUS_UP_CHANGE & phy_data)
    {
        *intr_status_flag |= FAL_PHY_INTR_STATUS_UP_CHANGE;
    }

    if (F1_INTR_STATUS_DOWN_CHANGE & phy_data)
    {
        *intr_status_flag |= FAL_PHY_INTR_STATUS_DOWN_CHANGE;
    }

    if (F1_INTR_SPEED_CHANGE & phy_data)
    {
        *intr_status_flag |= FAL_PHY_INTR_SPEED_CHANGE;
    }

    if (F1_INTR_DUPLEX_CHANGE & phy_data)
    {
        *intr_status_flag |= FAL_PHY_INTR_DUPLEX_CHANGE;
    }

    return SW_OK;
}
/******************************************************************************
*
*f1_phy_set_8023az status
*
* get 8023az status
*/

sw_error_t
f1_phy_set_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;

	phy_data = f1_phy_mmd_read(dev_id, phy_id, F1_PHY_MMD7_NUM,
				       F1_PHY_8023AZ_EEE_CTRL);
	if (enable == A_TRUE) {
		phy_data |= F1_PHY_AZ_ENABLE;

		f1_phy_mmd_write(dev_id, phy_id, F1_PHY_MMD7_NUM,
			     F1_PHY_8023AZ_EEE_CTRL, phy_data);
	} else {
		phy_data &= ~F1_PHY_AZ_ENABLE;

		f1_phy_mmd_write(dev_id, phy_id, F1_PHY_MMD7_NUM,
			     F1_PHY_8023AZ_EEE_CTRL, phy_data);
	}

	f1_phy_restart_autoneg(dev_id, phy_id);

	return SW_OK;
}

/******************************************************************************
*
*f1_phy_get_8023az status
*
* get 8023az status
*/
sw_error_t
f1_phy_get_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	a_uint16_t phy_data;

	*enable = A_FALSE;

	phy_data = f1_phy_mmd_read(dev_id, phy_id, F1_PHY_MMD7_NUM,
				       F1_PHY_8023AZ_EEE_CTRL);

	if ((phy_data & 0x6) == F1_PHY_AZ_ENABLE)
		*enable = A_TRUE;

	return SW_OK;
}

/******************************************************************************
*
* f1_phy_set_local_loopback
*
* set phy local loopback
*/
sw_error_t
f1_phy_set_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;
	fal_port_speed_t old_speed;
	sw_error_t rv = SW_OK;

	if (enable == A_TRUE) {
		rv = f1_phy_get_speed(dev_id, phy_id, &old_speed);
		SW_RTN_ON_ERROR(rv);

		if (old_speed == FAL_SPEED_1000) {
			phy_data = F1_1000M_LOOPBACK;
		} else if (old_speed == FAL_SPEED_100) {
			phy_data = F1_100M_LOOPBACK;
		} else if (old_speed == FAL_SPEED_10) {
			phy_data = F1_10M_LOOPBACK;
		} else {
			return SW_FAIL;
		}
		phy_data |= F1_CTRL_FULL_DUPLEX;
	} else {
		phy_data = F1_COMMON_CTRL;
	}

	rv = f1_phy_reg_write(dev_id, phy_id, F1_PHY_CONTROL, phy_data);

	return rv;
}

/******************************************************************************
*
* f1_phy_get_local_loopback
*
* get phy local loopback
*/
sw_error_t
f1_phy_get_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	a_uint16_t phy_data;

	phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CONTROL);

	if (phy_data & F1_LOCAL_LOOPBACK_ENABLE)
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
* f1_phy_set_remote_loopback
*
* set phy remote loopback
*/
sw_error_t
f1_phy_set_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
	a_uint16_t phy_data;
	sw_error_t rv = SW_OK;

	phy_data = f1_phy_mmd_read(dev_id, phy_id, F1_PHY_MMD3_NUM,
		F1_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL);

	if (enable == A_TRUE)
	{
		phy_data |= F1_PHY_REMOTE_LOOPBACK_ENABLE;
	}
	else
	{
		phy_data &= ~F1_PHY_REMOTE_LOOPBACK_ENABLE;
	}

	rv = f1_phy_mmd_write(dev_id, phy_id, F1_PHY_MMD3_NUM,
		F1_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL, phy_data);

	return rv;
}

/******************************************************************************
*
* f1_phy_get_remote_loopback
*
* get phy remote loopback
*/
sw_error_t
f1_phy_get_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable)
{
	a_uint16_t phy_data;

	phy_data = f1_phy_mmd_read(dev_id, phy_id, F1_PHY_MMD3_NUM,
		F1_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL);

	if (phy_data & F1_PHY_REMOTE_LOOPBACK_ENABLE)
	{
		*enable = A_TRUE;
	}
	else
	{
		*enable = A_FALSE;
	}

	return SW_OK;
}

static int f1_phy_api_ops_init(void)
{
	int ret;
	hsl_phy_ops_t *f1_phy_api_ops = NULL;

	f1_phy_api_ops = kzalloc(sizeof(hsl_phy_ops_t), GFP_KERNEL);
	if (f1_phy_api_ops == NULL) {
		SSDK_ERROR("f1 phy ops kzalloc failed!\n");
		return -ENOMEM;
	}

	phy_api_ops_init(F1_PHY_CHIP);

	f1_phy_api_ops->phy_hibernation_set = f1_phy_set_hibernate;
	f1_phy_api_ops->phy_hibernation_get = f1_phy_get_hibernate;
	f1_phy_api_ops->phy_speed_get = f1_phy_get_speed;
	f1_phy_api_ops->phy_speed_set = f1_phy_set_speed;
	f1_phy_api_ops->phy_duplex_get = f1_phy_get_duplex;
	f1_phy_api_ops->phy_duplex_set = f1_phy_set_duplex;
	f1_phy_api_ops->phy_autoneg_enable_set = f1_phy_enable_autoneg;
	f1_phy_api_ops->phy_restart_autoneg = f1_phy_restart_autoneg;
	f1_phy_api_ops->phy_autoneg_status_get = f1_phy_autoneg_status;
	f1_phy_api_ops->phy_autoneg_adv_set = f1_phy_set_autoneg_adv;
	f1_phy_api_ops->phy_autoneg_adv_get = f1_phy_get_autoneg_adv;
	f1_phy_api_ops->phy_powersave_set = f1_phy_set_powersave;
	f1_phy_api_ops->phy_powersave_get = f1_phy_get_powersave;
	f1_phy_api_ops->phy_cdt = f1_phy_cdt;
	f1_phy_api_ops->phy_link_status_get = f1_phy_get_link_status;
	f1_phy_api_ops->phy_reset = f1_phy_reset;
	f1_phy_api_ops->phy_power_off = f1_phy_poweroff;
	f1_phy_api_ops->phy_power_on = 	f1_phy_poweron;
	f1_phy_api_ops->phy_id_get = f1_phy_get_phy_id;
	f1_phy_api_ops->phy_reg_write = f1_phy_reg_write;
	f1_phy_api_ops->phy_reg_read = f1_phy_reg_read;
	f1_phy_api_ops->phy_debug_write = f1_phy_debug_write;
	f1_phy_api_ops->phy_debug_read = f1_phy_debug_read;
	f1_phy_api_ops->phy_mmd_write = f1_phy_mmd_write;
	f1_phy_api_ops->phy_mmd_read = f1_phy_mmd_read;
	f1_phy_api_ops->phy_local_loopback_set = f1_phy_set_local_loopback;
	f1_phy_api_ops->phy_local_loopback_get = f1_phy_get_local_loopback;
	f1_phy_api_ops->phy_remote_loopback_set = f1_phy_set_remote_loopback;
	f1_phy_api_ops->phy_remote_loopback_get = f1_phy_get_remote_loopback;
	f1_phy_api_ops->phy_intr_mask_set = f1_phy_intr_mask_set;
	f1_phy_api_ops->phy_intr_mask_get = f1_phy_intr_mask_get;
	f1_phy_api_ops->phy_intr_status_get = f1_phy_intr_status_get;
	f1_phy_api_ops->phy_8023az_set = f1_phy_set_8023az;
	f1_phy_api_ops->phy_8023az_get = f1_phy_get_8023az;

	ret = hsl_phy_api_ops_register(F1_PHY_CHIP, f1_phy_api_ops);

	if (ret == 0)
		SSDK_INFO("qca probe f1 phy driver succeeded!\n");
	else
		SSDK_ERROR("qca probe f1 phy driver failed! (code: %d)\n", ret);
	return ret;
}

int f1_phy_init(a_uint32_t dev_id, a_uint32_t port_bmp)
{
	static a_uint32_t phy_ops_flag = 0;

	if(phy_ops_flag == 0) {
		f1_phy_api_ops_init();
		phy_ops_flag = 1;
	}

	return 0;
}

