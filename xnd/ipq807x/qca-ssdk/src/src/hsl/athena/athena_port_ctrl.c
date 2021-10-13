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
 * @defgroup athena_port_ctrl ATHENA_PORT_CTRL
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "athena_port_ctrl.h"
#include "athena_reg.h"
#include "hsl_phy.h"



static sw_error_t
_athena_port_duplex_set(a_uint32_t dev_id, fal_port_t port_id,
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
_athena_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id,
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
_athena_port_speed_set(a_uint32_t dev_id, fal_port_t port_id,
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

    if (FAL_SPEED_100 < speed)
    {
        return SW_BAD_PARAM;
    }

    rv = phy_drv->phy_speed_set(dev_id, phy_id, speed);

    return rv;
}


static sw_error_t
_athena_port_speed_get(a_uint32_t dev_id, fal_port_t port_id,
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
_athena_port_autoneg_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t * status)
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
    if (NULL == phy_drv->phy_autoneg_status_get)
         return SW_NOT_SUPPORTED;


    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    *status = phy_drv->phy_autoneg_status_get(dev_id, phy_id);

    return SW_OK;
}


static sw_error_t
_athena_port_autoneg_enable(a_uint32_t dev_id, fal_port_t port_id)
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
_athena_port_autoneg_restart(a_uint32_t dev_id, fal_port_t port_id)
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
_athena_port_autoneg_adv_set(a_uint32_t dev_id, fal_port_t port_id,
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
_athena_port_autoneg_adv_get(a_uint32_t dev_id, fal_port_t port_id,
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
_athena_port_igmps_status_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_EXCL_CPU))
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

    HSL_REG_FIELD_SET(rv, dev_id, PORT_CTL, port_id, IGMP_MLD_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));

    return rv;
}


static sw_error_t
_athena_port_igmps_status_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_EXCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, PORT_CTL, port_id, IGMP_MLD_EN,
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

    return SW_OK;
}

static sw_error_t
_athena_port_powersave_set(a_uint32_t dev_id, fal_port_t port_id,
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
_athena_port_powersave_get(a_uint32_t dev_id, fal_port_t port_id,
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
_athena_port_hibernate_set(a_uint32_t dev_id, fal_port_t port_id,
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
_athena_port_hibernate_get(a_uint32_t dev_id, fal_port_t port_id,
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

/**
 * @brief Set duplex mode on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] duplex duplex mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
athena_port_duplex_set(a_uint32_t dev_id, fal_port_t port_id,
                       fal_port_duplex_t duplex)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_duplex_set(dev_id, port_id, duplex);
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
athena_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id,
                       fal_port_duplex_t * pduplex)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_duplex_get(dev_id, port_id, pduplex);
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
athena_port_speed_set(a_uint32_t dev_id, fal_port_t port_id,
                      fal_port_speed_t speed)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_speed_set(dev_id, port_id, speed);
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
athena_port_speed_get(a_uint32_t dev_id, fal_port_t port_id,
                      fal_port_speed_t * pspeed)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_speed_get(dev_id, port_id, pspeed);
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
athena_port_autoneg_status_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * status)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_autoneg_status_get(dev_id, port_id, status);
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
athena_port_autoneg_enable(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_autoneg_enable(dev_id, port_id);
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
athena_port_autoneg_restart(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_autoneg_restart(dev_id, port_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set auto negotiation advtisement ability on a particular port.
 * @details Comments:
 *    Auto negotiation advtisement ability is defined by macro such as
 *    FAL_PHY_ADV_10T_HD, FAL_PHY_ADV_PAUSE...
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] autoadv auto negotiation advtisement ability bit map
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
athena_port_autoneg_adv_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t autoadv)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_autoneg_adv_set(dev_id, port_id, autoadv);
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
athena_port_autoneg_adv_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t * autoadv)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_autoneg_adv_get(dev_id, port_id, autoadv);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set igmp/mld packets snooping status on a particular port.
 * @details Comments:
 *    After enabling igmp snooping feature on a particular port all kinds
 *    igmp packets received on this port would be acknowledged by hardware.
 *    Athena only supports igmp packets, it doesn't support mld packets.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
athena_port_igmps_status_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_igmps_status_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get igmp packets snooping status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
athena_port_igmps_status_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_igmps_status_get(dev_id, port_id, enable);
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
athena_port_powersave_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_powersave_set(dev_id, port_id, enable);
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
athena_port_powersave_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_powersave_get(dev_id, port_id, enable);
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
athena_port_hibernate_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_hibernate_set(dev_id, port_id, enable);
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
athena_port_hibernate_get(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_port_hibernate_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
athena_port_ctrl_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    p_api->port_duplex_get = athena_port_duplex_get;
    p_api->port_duplex_set = athena_port_duplex_set;
    p_api->port_speed_get = athena_port_speed_get;
    p_api->port_speed_set = athena_port_speed_set;
    p_api->port_autoneg_status_get = athena_port_autoneg_status_get;
    p_api->port_autoneg_enable = athena_port_autoneg_enable;
    p_api->port_autoneg_restart = athena_port_autoneg_restart;
    p_api->port_autoneg_adv_get = athena_port_autoneg_adv_get;
    p_api->port_autoneg_adv_set = athena_port_autoneg_adv_set;
    p_api->port_igmps_status_set = athena_port_igmps_status_set;
    p_api->port_igmps_status_get = athena_port_igmps_status_get;
    p_api->port_powersave_set = athena_port_powersave_set;
    p_api->port_powersave_get = athena_port_powersave_get;
    p_api->port_hibernate_set = athena_port_hibernate_set;
    p_api->port_hibernate_get = athena_port_hibernate_get;
#endif

    return SW_OK;
}

/**
 * @}
 */

