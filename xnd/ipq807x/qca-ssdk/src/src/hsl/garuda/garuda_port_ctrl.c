/*
 * Copyright (c) 2012, 2015, The Linux Foundation. All rights reserved.
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


/**
 * @defgroup garuda_port_ctrl GARUDA_PORT_CONTROL
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "garuda_port_ctrl.h"
#include "garuda_reg.h"
#include "hsl_phy.h"

static sw_error_t
_garuda_port_duplex_set(a_uint32_t dev_id, fal_port_t port_id,
                        fal_port_duplex_t duplex)
{
    sw_error_t rv;
    a_uint32_t phy_id = 0;
    a_uint32_t reg_save = 0;
    a_uint32_t reg_val = 0;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
    if (NULL == phy_drv->phy_duplex_set)
    	  return SW_NOT_SUPPORTED;

    if (FAL_DUPLEX_BUTT <= duplex)
    {
        return SW_BAD_PARAM;
    }

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    //save reg value
    HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, port_id,
                      (a_uint8_t *) (&reg_val), sizeof (a_uint32_t));
    reg_save = reg_val;

    SW_SET_REG_BY_FIELD(PORT_STATUS, LINK_EN, 0, reg_val);
    SW_SET_REG_BY_FIELD(PORT_STATUS, RXMAC_EN, 0, reg_val);
    SW_SET_REG_BY_FIELD(PORT_STATUS, TXMAC_EN, 0, reg_val);

    //set mac be config by sw   and turn off RX TX MAC_EN
    HSL_REG_ENTRY_SET(rv, dev_id, PORT_STATUS, port_id,
                      (a_uint8_t *) (&reg_val), sizeof (a_uint32_t));

    rv = phy_drv->phy_duplex_set(dev_id, phy_id, duplex);

    //retore reg value
    HSL_REG_ENTRY_SET(rv, dev_id, PORT_STATUS, port_id,
                      (a_uint8_t *) (&reg_save), sizeof (a_uint32_t));

    return rv;
}


static sw_error_t
_garuda_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id,
                        fal_port_duplex_t * pduplex)
{
    sw_error_t rv;
    a_uint32_t phy_id;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
    if (NULL == phy_drv->phy_duplex_get)
         return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    rv = phy_drv->phy_duplex_get(dev_id, phy_id, pduplex);
    return rv;
}

static sw_error_t
_garuda_port_speed_set(a_uint32_t dev_id, fal_port_t port_id,
                       fal_port_speed_t speed)
{
    sw_error_t rv;
    a_uint32_t phy_id = 0;
    hsl_phy_ops_t *phy_drv;
 
    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
    if (NULL == phy_drv->phy_speed_set)
         return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_SPEED_1000 < speed)
    {
        return SW_BAD_PARAM;
    }

    rv = phy_drv->phy_speed_set(dev_id, phy_id, speed);

    return rv;
}


static sw_error_t
_garuda_port_speed_get(a_uint32_t dev_id, fal_port_t port_id,
                       fal_port_speed_t * pspeed)
{
    sw_error_t rv;
    a_uint32_t phy_id;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
    if (NULL == phy_drv->phy_speed_get)
        return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    rv = phy_drv->phy_speed_get(dev_id, phy_id, pspeed);

    return rv;
}

static sw_error_t
_garuda_port_autoneg_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t * status)
{
    a_uint32_t phy_id;
    sw_error_t rv;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
    if (NULL == phy_drv->phy_autoneg_status_get)
        return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    *status = phy_drv->phy_autoneg_status_get (dev_id, phy_id);

    return SW_OK;
}

static sw_error_t
_garuda_port_autoneg_enable(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;
    a_uint32_t phy_id;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
    if (NULL == phy_drv->phy_autoneg_enable_set)
 	  return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    rv = phy_drv->phy_autoneg_enable_set(dev_id, phy_id);
    return rv;
}

static sw_error_t
_garuda_port_autoneg_restart(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;
    a_uint32_t phy_id;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
    if (NULL == phy_drv->phy_restart_autoneg)
	 return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    rv = phy_drv->phy_restart_autoneg(dev_id, phy_id);
    return rv;
}


static sw_error_t
_garuda_port_autoneg_adv_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t autoadv)
{
    sw_error_t rv;
    a_uint32_t phy_id;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
    if (NULL == phy_drv->phy_autoneg_adv_set)
    	 return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    rv = phy_drv->phy_autoneg_adv_set(dev_id, phy_id, autoadv);
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}


static sw_error_t
_garuda_port_autoneg_adv_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_uint32_t * autoadv)
{
    sw_error_t rv;
    a_uint32_t phy_id;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
    if (NULL == phy_drv->phy_autoneg_adv_get)
        return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    *autoadv = 0;
    rv = phy_drv->phy_autoneg_adv_get(dev_id, phy_id, autoadv);
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

static sw_error_t
_garuda_port_hdr_status_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (A_TRUE == enable)
    {
        val = 1;
    }
    else if (A_FALSE == enable)
    {
        val = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_CTL, port_id, HEAD_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));

    return rv;
}

static sw_error_t
_garuda_port_hdr_status_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_CTL, port_id, HEAD_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == val)
    {
        *enable = A_TRUE;
    }
    else
    {
        *enable = A_FALSE;
    }

    return rv;
}

static sw_error_t
_garuda_port_flowctrl_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val, force, reg;

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (A_TRUE == enable)
    {
        val = 1;
    }
    else if (A_FALSE == enable)
    {
        val = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, port_id,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(PORT_STATUS, FLOW_LINK_EN, force, reg);
    if (force)
    {
        /* flow control isn't in force mode so can't set */
        return SW_DISABLE;
    }

    SW_SET_REG_BY_FIELD(PORT_STATUS, RX_FLOW_EN, val, reg);
    SW_SET_REG_BY_FIELD(PORT_STATUS, TX_FLOW_EN, val, reg);
    SW_SET_REG_BY_FIELD(PORT_STATUS, TX_HALF_FLOW_EN, val, reg);

    HSL_REG_ENTRY_SET(rv, dev_id, PORT_STATUS, port_id,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    return rv;
}


static sw_error_t
_garuda_port_flowctrl_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t tx, rx, reg;

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, port_id,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(PORT_STATUS, RX_FLOW_EN, rx, reg);
    SW_GET_FIELD_BY_REG(PORT_STATUS, TX_FLOW_EN, tx, reg);

    if (1 == rx)
    {
        *enable = A_TRUE;
    }
    else
    {
        *enable = A_FALSE;
    }

    return SW_OK;
}

static sw_error_t
_garuda_port_flowctrl_forcemode_set(a_uint32_t dev_id, fal_port_t port_id,
                                    a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t force, reg;

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, port_id,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(PORT_STATUS, FLOW_LINK_EN, force, reg);
    if (force != (a_uint32_t) enable)
    {
        return SW_OK;
    }

    if (A_TRUE == enable)
    {
        SW_SET_REG_BY_FIELD(PORT_STATUS, FLOW_LINK_EN, 0, reg);
        SW_SET_REG_BY_FIELD(PORT_STATUS, TX_FLOW_EN, 0, reg);
        SW_SET_REG_BY_FIELD(PORT_STATUS, RX_FLOW_EN, 0, reg);
    }
    else if (A_FALSE == enable)
    {
        SW_SET_REG_BY_FIELD(PORT_STATUS, FLOW_LINK_EN, 1, reg);
    }
    else
    {
        return SW_BAD_PARAM;
    }
    SW_SET_REG_BY_FIELD(PORT_STATUS, TX_HALF_FLOW_EN, 0, reg);

    HSL_REG_ENTRY_SET(rv, dev_id, PORT_STATUS, port_id,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_garuda_port_flowctrl_forcemode_get(a_uint32_t dev_id, fal_port_t port_id,
                                    a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t force, reg;

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, port_id,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_GET_FIELD_BY_REG(PORT_STATUS, FLOW_LINK_EN, force, reg);
    if (0 == force)
    {
        *enable = A_TRUE;
    }
    else
    {
        *enable = A_FALSE;
    }

    return SW_OK;
}

static sw_error_t
_garuda_port_powersave_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t phy_id = 0;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

   SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
   if (NULL == phy_drv->phy_powersave_set)
	 return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    rv = phy_drv->phy_powersave_set(dev_id, phy_id, enable);

    return rv;
}

static sw_error_t
_garuda_port_powersave_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t *enable)
{
    sw_error_t rv;
    a_uint32_t phy_id = 0;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
    if (NULL == phy_drv->phy_powersave_get)
         return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    rv = phy_drv->phy_powersave_get(dev_id, phy_id, enable);

    return rv;
}

static sw_error_t
_garuda_port_hibernate_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t phy_id = 0;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
    if (NULL == phy_drv->phy_hibernation_set)
        return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    rv = phy_drv->phy_hibernation_set(dev_id, phy_id, enable);

    return rv;
}

static sw_error_t
_garuda_port_hibernate_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t *enable)
{
    sw_error_t rv;
    a_uint32_t phy_id = 0;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
    if (NULL == phy_drv->phy_hibernation_get)
        return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    rv = phy_drv->phy_hibernation_get(dev_id, phy_id, enable);

    return rv;
}

static sw_error_t
_garuda_port_cdt(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t mdi_pair,
                 fal_cable_status_t *cable_status, a_uint32_t *cable_len)
{
    sw_error_t rv;
    a_uint32_t phy_id = 0;
    hsl_phy_ops_t *phy_drv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
    {
        return SW_BAD_PARAM;
    }

    SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id));
    if (NULL == phy_drv->phy_cdt)
        return SW_NOT_SUPPORTED;

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    rv = phy_drv->phy_cdt(dev_id, phy_id, mdi_pair, cable_status, cable_len);

    return rv;
}

/**
 * @brief Set duplex mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] duplex duplex mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_duplex_set(a_uint32_t dev_id, fal_port_t port_id,
                       fal_port_duplex_t duplex)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_duplex_set(dev_id, port_id, duplex);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get duplex mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] duplex duplex mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id,
                       fal_port_duplex_t * pduplex)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_duplex_get(dev_id, port_id, pduplex);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set speed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] speed port speed
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_speed_set(a_uint32_t dev_id, fal_port_t port_id,
                      fal_port_speed_t speed)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_speed_set(dev_id, port_id, speed);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get speed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] speed port speed
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_speed_get(a_uint32_t dev_id, fal_port_t port_id,
                      fal_port_speed_t * pspeed)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_speed_get(dev_id, port_id, pspeed);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get auto negotiation status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] status A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_autoneg_status_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * status)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_autoneg_status_get(dev_id, port_id, status);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Enable auto negotiation status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_autoneg_enable(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_autoneg_enable(dev_id, port_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Restart auto negotiation procedule on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_autoneg_restart(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_autoneg_restart(dev_id, port_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set auto negotiation advtisement ability on a particular port.
 *   @details  Comments:
 *   auto negotiation advtisement ability is defined by macro such as
 *   FAL_PHY_ADV_10T_HD, FAL_PHY_ADV_PAUSE...
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] autoadv auto negotiation advtisement ability bit map
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_autoneg_adv_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t autoadv)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_autoneg_adv_set(dev_id, port_id, autoadv);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get auto negotiation advtisement ability on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] autoadv auto negotiation advtisement ability bit map
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_autoneg_adv_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t * autoadv)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_autoneg_adv_get(dev_id, port_id, autoadv);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set status of Atheros header packets parsed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_hdr_status_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_hdr_status_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get status of Atheros header packets parsed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_hdr_status_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_hdr_status_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set flow control status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_flowctrl_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_flowctrl_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get flow control status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_flowctrl_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_flowctrl_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set flow control force mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_flowctrl_forcemode_set(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_flowctrl_forcemode_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get flow control force mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_flowctrl_forcemode_get(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_flowctrl_forcemode_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set powersaving status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_powersave_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_powersave_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get powersaving status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_powersave_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_powersave_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set hibernate status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_hibernate_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_hibernate_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get hibernate status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_hibernate_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_hibernate_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Run cable diagnostic test on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] mdi_pair mdi pair id
 * @param[out] cable_status cable status
 * @param[out] cable_len cable len
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_cdt(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t mdi_pair,
                fal_cable_status_t *cable_status, a_uint32_t *cable_len)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_port_cdt(dev_id, port_id, mdi_pair, cable_status, cable_len);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
garuda_port_ctrl_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->port_duplex_get = garuda_port_duplex_get;
        p_api->port_duplex_set = garuda_port_duplex_set;
        p_api->port_speed_get = garuda_port_speed_get;
        p_api->port_speed_set = garuda_port_speed_set;
        p_api->port_autoneg_status_get = garuda_port_autoneg_status_get;
        p_api->port_autoneg_enable = garuda_port_autoneg_enable;
        p_api->port_autoneg_restart = garuda_port_autoneg_restart;
        p_api->port_autoneg_adv_get = garuda_port_autoneg_adv_get;
        p_api->port_autoneg_adv_set = garuda_port_autoneg_adv_set;
        p_api->port_hdr_status_set = garuda_port_hdr_status_set;
        p_api->port_hdr_status_get = garuda_port_hdr_status_get;
        p_api->port_flowctrl_set = garuda_port_flowctrl_set;
        p_api->port_flowctrl_get = garuda_port_flowctrl_get;
        p_api->port_flowctrl_forcemode_set = garuda_port_flowctrl_forcemode_set;
        p_api->port_flowctrl_forcemode_get = garuda_port_flowctrl_forcemode_get;
        p_api->port_powersave_set = garuda_port_powersave_set;
        p_api->port_powersave_get = garuda_port_powersave_get;
        p_api->port_hibernate_set = garuda_port_hibernate_set;
        p_api->port_hibernate_get = garuda_port_hibernate_get;
        p_api->port_cdt           = garuda_port_cdt;

    }
#endif

    return SW_OK;
}

/**
 * @}
 */

