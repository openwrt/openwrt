/*
 * Copyright (c) 2012, 2017, The Linux Foundation. All rights reserved.
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
 * @defgroup garuda_init GARUDA_INIT
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "garuda_mib.h"
#include "garuda_port_ctrl.h"
#include "garuda_portvlan.h"
#include "garuda_vlan.h"
#include "garuda_fdb.h"
#include "garuda_qos.h"
#include "garuda_mirror.h"
#include "garuda_stp.h"
#include "garuda_rate.h"
#include "garuda_misc.h"
#include "garuda_leaky.h"
#include "garuda_igmp.h"
#include "garuda_acl.h"
#include "garuda_led.h"
#include "garuda_reg_access.h"
#include "garuda_reg.h"
#include "garuda_init.h"
#include "f1_phy.h"

static ssdk_init_cfg * garuda_cfg[SW_MAX_NR_DEV] = { 0 };

#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
static sw_error_t
garuda_portproperty_init(a_uint32_t dev_id, hsl_init_mode mode)
{
    hsl_port_prop_t p_type;
    hsl_dev_t *pdev = NULL;
    fal_port_t port_id;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;

    for (port_id = 0; port_id < pdev->nr_ports; port_id++)
    {
        hsl_port_prop_portmap_set(dev_id, port_id);

        for (p_type = HSL_PP_PHY; p_type < HSL_PP_BUTT; p_type++)
        {
            if (HSL_NO_CPU == mode)
            {
                SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
                continue;
            }

            switch (p_type)
            {
                case HSL_PP_PHY:
                    if (HSL_CPU_1 != mode)
                    {
                        if ((port_id != pdev->cpu_port_nr)
                                && (port_id != (pdev->nr_ports -1)))
                        {
                            SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
                        }
                    }
                    else
                    {
                        if (port_id != pdev->cpu_port_nr)
                        {
                            SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
                        }
                    }
                    break;

                case HSL_PP_INCL_CPU:
                    /* include cpu port but exclude wan port in some cases */
                    if (!((HSL_CPU_2 == mode) && (port_id == (pdev->nr_ports - 1))))
                        SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));

                    break;

                case HSL_PP_EXCL_CPU:
                    /* exclude cpu port and wan port in some cases */
                    if ((port_id != pdev->cpu_port_nr)
                            && (!((HSL_CPU_2 == mode) && (port_id == (pdev->nr_ports - 1)))))
                        SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
                    break;

                default:
                    break;
            }
        }

        if (HSL_NO_CPU == mode)
        {
            SW_RTN_ON_ERROR(hsl_port_prop_set_phyid
                            (dev_id, port_id, port_id + 1));
        }
        else
        {
            if (port_id != pdev->cpu_port_nr)
            {
                SW_RTN_ON_ERROR(hsl_port_prop_set_phyid
                                (dev_id, port_id, port_id - 1));
            }
        }
    }

    return SW_OK;
}

static void
phy_dport_set(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t dport_addr, a_uint16_t val_mask)
{
    a_uint16_t phy_data;
    sw_error_t rv;

    HSL_PHY_SET(rv, dev_id, phy_id, F1_DEBUG_PORT_ADDRESS, dport_addr);
    HSL_PHY_GET(rv, dev_id, phy_id, F1_DEBUG_PORT_DATA, &phy_data);
    phy_data |= val_mask;
    HSL_PHY_SET(rv, dev_id, phy_id, F1_DEBUG_PORT_DATA, phy_data);
}

static void
phy_dport_clear(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t dport_addr, a_uint16_t val_mask)
{
    a_uint16_t phy_data;
    sw_error_t rv;

    HSL_PHY_SET(rv, dev_id, phy_id, F1_DEBUG_PORT_ADDRESS, dport_addr);
    HSL_PHY_GET(rv, dev_id, phy_id, F1_DEBUG_PORT_DATA, &phy_data);
    phy_data &= ~val_mask;
    HSL_PHY_SET(rv, dev_id, phy_id, F1_DEBUG_PORT_DATA, phy_data);
}

static sw_error_t
garuda_hw_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    garuda_init_spec_cfg *garuda_init_cfg = NULL;
    hsl_dev_t    *pdev = NULL;
    hsl_init_mode cpu_mode;
    a_uint32_t port_id;
    a_uint32_t data;
    sw_error_t rv;

    pdev = hsl_dev_ptr_get(dev_id);
    if (NULL == pdev)
    {
        return SW_NOT_INITIALIZED;
    }
    cpu_mode = cfg->cpu_mode;

    HSL_REG_ENTRY_GET(rv, dev_id, POSTRIP, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    /* phy pll on */
    SW_SET_REG_BY_FIELD(POSTRIP, PHY_PLL_ON, 1, data);

    garuda_init_cfg = (garuda_init_spec_cfg* )(cfg->chip_spec_cfg);
    if (!garuda_init_cfg)
    {
        return SW_BAD_PARAM;
    }

    /* delay */
    if (A_TRUE == garuda_init_cfg->rx_delay_s1)
    {
        SW_SET_REG_BY_FIELD(POSTRIP, RXDELAY_S1, 1, data);
    }
    else
    {
        SW_SET_REG_BY_FIELD(POSTRIP, RXDELAY_S1, 0, data);
    }

    if (A_TRUE == garuda_init_cfg->rx_delay_s0)
    {
        SW_SET_REG_BY_FIELD(POSTRIP, RXDELAY_S0, 1, data);
    }
    else
    {
        SW_SET_REG_BY_FIELD(POSTRIP, RXDELAY_S0, 0, data);
    }

    if (A_TRUE ==  garuda_init_cfg->tx_delay_s1)
    {
        SW_SET_REG_BY_FIELD(POSTRIP, TXDELAY_S1, 1, data);
    }
    else
    {
        SW_SET_REG_BY_FIELD(POSTRIP, TXDELAY_S1, 0, data);
    }

    if (A_TRUE ==  garuda_init_cfg->tx_delay_s0)
    {
        SW_SET_REG_BY_FIELD(POSTRIP, TXDELAY_S0, 1, data);
    }
    else
    {
        SW_SET_REG_BY_FIELD(POSTRIP, TXDELAY_S0, 0, data);
    }

    /* tx/rx delay enable */
    if (A_TRUE ==  garuda_init_cfg->rgmii_txclk_delay)
    {
        SW_SET_REG_BY_FIELD(POSTRIP, RGMII_TXCLK_DELAY_EN, 1, data);
    }
    else
    {
        SW_SET_REG_BY_FIELD(POSTRIP, RGMII_TXCLK_DELAY_EN, 0, data);
    }

    /* tx/rx delay enable */
    if (A_TRUE ==  garuda_init_cfg->rgmii_rxclk_delay)
    {
        SW_SET_REG_BY_FIELD(POSTRIP, RGMII_RXCLK_DELAY_EN, 1, data);
    }
    else
    {
        SW_SET_REG_BY_FIELD(POSTRIP, RGMII_RXCLK_DELAY_EN, 0, data);
    }

    /* mac5 default mode */
    /*SW_SET_REG_BY_FIELD(POSTRIP, MAC5_PHY_MODE, 0, data);
    SW_SET_REG_BY_FIELD(POSTRIP, MAC5_MAC_MODE, 0, data);*/

    /* mac0 default phy mode */
    SW_SET_REG_BY_FIELD(POSTRIP, MAC0_MAC_MODE, 0, data);

    /* mac0 default rgmii mode */
    SW_SET_REG_BY_FIELD(POSTRIP, MAC0_RGMII_EN, 1, data);
    SW_SET_REG_BY_FIELD(POSTRIP, MAC0_GMII_EN, 0, data);

    /* mac5 default disable mode */
    SW_SET_REG_BY_FIELD(POSTRIP, MAC5_PHY_MODE, 0, data);
    SW_SET_REG_BY_FIELD(POSTRIP, MAC5_MAC_MODE, 0, data);

    /* phy default mode */
    SW_SET_REG_BY_FIELD(POSTRIP, PHY4_RGMII_EN, 0, data);
    SW_SET_REG_BY_FIELD(POSTRIP, PHY4_GMII_EN, 0, data);

    /* modify default mode */
    if (A_FALSE == garuda_init_cfg->mac0_rgmii)
    {
        SW_SET_REG_BY_FIELD(POSTRIP, MAC0_RGMII_EN, 0, data);
        SW_SET_REG_BY_FIELD(POSTRIP, MAC0_GMII_EN,  1, data);

        /*invert clock output for port0 gmii pad.*/
        a_uint32_t temp;
        HSL_REG_ENTRY_GET(rv, dev_id, MASK_CTL, 0,
                          (a_uint8_t *) (&temp), sizeof (a_uint32_t));
        temp |= 1<<MASK_CTL_MII_CLK0_SEL_BOFFSET;
        HSL_REG_ENTRY_SET(rv, dev_id, MASK_CTL, 0,
                          (a_uint8_t *) (&temp), sizeof (a_uint32_t));
    }

    if(HSL_CPU_2 == cpu_mode)
    {
        if (A_TRUE == garuda_init_cfg->mac5_rgmii)
        {

            SW_SET_REG_BY_FIELD(POSTRIP, PHY4_RGMII_EN, 1, data);
            SW_SET_REG_BY_FIELD(POSTRIP, PHY4_GMII_EN, 0, data);

            a_uint32_t phy_id = 4;
            /* phy4 rgmii mode enable */
            phy_dport_set(dev_id, phy_id, F1_DEBUG_PORT_RGMII_MODE, F1_DEBUG_PORT_RGMII_MODE_EN);

            /* Rx delay enable */
            if (A_TRUE ==  garuda_init_cfg->phy4_rx_delay)
            {
                phy_dport_set(dev_id, phy_id, F1_DEBUG_PORT_RX_DELAY, F1_DEBUG_PORT_RX_DELAY_EN);
            }
            else
            {
                phy_dport_clear(dev_id, phy_id, F1_DEBUG_PORT_RX_DELAY, F1_DEBUG_PORT_RX_DELAY_EN);
            }

            /* Tx delay enable */
            if (A_TRUE ==  garuda_init_cfg->phy4_tx_delay)
            {
                phy_dport_set(dev_id, phy_id, F1_DEBUG_PORT_TX_DELAY, F1_DEBUG_PORT_TX_DELAY_EN);
            }
            else
            {
                phy_dport_clear(dev_id, phy_id, F1_DEBUG_PORT_TX_DELAY, F1_DEBUG_PORT_TX_DELAY_EN);
            }

        }
        else
        {
            SW_SET_REG_BY_FIELD(POSTRIP, PHY4_RGMII_EN, 0, data);
            SW_SET_REG_BY_FIELD(POSTRIP, PHY4_GMII_EN, 1, data);
        }
    }
    else if (HSL_CPU_1 == cpu_mode)
    {
        //SW_SET_REG_BY_FIELD(POSTRIP, TXDELAY_S0, 0, data);

    }
    else if (HSL_CPU_1_PLUS == cpu_mode)
    {
        SW_SET_REG_BY_FIELD(POSTRIP, MAC5_MAC_MODE, 1, data);

    }
    else if (HSL_NO_CPU == cpu_mode)
    {

    }

    HSL_REG_ENTRY_SET(rv, dev_id, POSTRIP, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    for (port_id = 0; port_id < pdev->nr_ports; port_id++)
    {
        if (port_id == pdev->cpu_port_nr)
        {
            continue;
        }

        HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, port_id,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_SET_REG_BY_FIELD(PORT_STATUS, LINK_EN, 1, data);

        HSL_REG_ENTRY_SET(rv, dev_id, PORT_STATUS, port_id,
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

static sw_error_t
garuda_bist_test(a_uint32_t dev_id)
{
    a_uint32_t entry, data, i;
    sw_error_t rv;

    data = 1;
    i = 0x1000;
    while (data && --i)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, BIST_CTRL, 0,
                          (a_uint8_t *) (&entry), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        SW_GET_FIELD_BY_REG(BIST_CTRL, BIST_BUSY, data, entry);
        aos_udelay(5);
    }

    if (0 == i)
    {
        return SW_INIT_ERROR;
    }

    entry = 0;
    SW_SET_REG_BY_FIELD(BIST_CTRL, BIST_BUSY, 1, entry);
    SW_SET_REG_BY_FIELD(BIST_CTRL, PTN_EN2, 1, entry);
    SW_SET_REG_BY_FIELD(BIST_CTRL, PTN_EN1, 1, entry);
    SW_SET_REG_BY_FIELD(BIST_CTRL, PTN_EN0, 1, entry);

    HSL_REG_ENTRY_SET(rv, dev_id, BIST_CTRL, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data = 1;
    i = 0x1000;
    while (data && --i)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, BIST_CTRL, 0,
                          (a_uint8_t *) (&entry), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        SW_GET_FIELD_BY_REG(BIST_CTRL, BIST_BUSY, data, entry);
        aos_udelay(5);
    }

    if (0 == i)
    {
        return SW_INIT_ERROR;
    }

    SW_GET_FIELD_BY_REG(BIST_CTRL, ERR_CNT, data, entry);
    if (data)
    {
        SW_GET_FIELD_BY_REG(BIST_CTRL, ONE_ERR, data, entry);
        if (!data)
        {
            return SW_INIT_ERROR;
        }

        SW_GET_FIELD_BY_REG(BIST_CTRL, ERR_ADDR, data, entry);

        entry = 0;
        SW_SET_REG_BY_FIELD(BIST_RCV, RCV_EN, 1, entry);
        SW_SET_REG_BY_FIELD(BIST_RCV, RCV_ADDR, data, entry);
        HSL_REG_ENTRY_SET(rv, dev_id, BIST_RCV, 0,
                          (a_uint8_t *) (&entry), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    else
    {
        return SW_OK;
    }

    entry = 0;
    SW_SET_REG_BY_FIELD(BIST_CTRL, BIST_BUSY, 1, entry);
    SW_SET_REG_BY_FIELD(BIST_CTRL, PTN_EN2, 1, entry);
    SW_SET_REG_BY_FIELD(BIST_CTRL, PTN_EN1, 1, entry);
    SW_SET_REG_BY_FIELD(BIST_CTRL, PTN_EN0, 1, entry);

    HSL_REG_ENTRY_SET(rv, dev_id, BIST_CTRL, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data = 1;
    i = 0x1000;
    while (data && --i)
    {
        HSL_REG_ENTRY_GET(rv, dev_id, BIST_CTRL, 0,
                          (a_uint8_t *) (&entry), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        SW_GET_FIELD_BY_REG(BIST_CTRL, BIST_BUSY, data, entry);
        aos_udelay(5);
    }

    if (0 == i)
    {
        return SW_INIT_ERROR;
    }

    SW_GET_FIELD_BY_REG(BIST_CTRL, ERR_CNT, data, entry);
    if (data)
    {
        return SW_INIT_ERROR;
    }

    return SW_OK;
}


#endif

static sw_error_t
garuda_dev_init(a_uint32_t dev_id, hsl_init_mode cpu_mode)
{
    hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;

    pdev->nr_ports = 6;
    pdev->nr_phy = 5;
    pdev->cpu_port_nr = 0;
    pdev->nr_vlans = 4096;
    pdev->hw_vlan_query = A_TRUE;
    pdev->nr_queue = 4;
    pdev->cpu_mode = cpu_mode;

    return SW_OK;
}

static sw_error_t
_garuda_reset(a_uint32_t dev_id)
{
#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    val = 0x1;
    HSL_REG_FIELD_SET(rv, dev_id, MASK_CTL, 0, SOFT_RST,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = garuda_hw_init(dev_id, garuda_cfg[dev_id]);
    SW_RTN_ON_ERROR(rv);

    GARUDA_ACL_RESET(rv, dev_id);
#endif

    return SW_OK;
}

sw_error_t
garuda_cleanup(a_uint32_t dev_id)
{
    if (garuda_cfg[dev_id])
    {
        aos_mem_free(garuda_cfg[dev_id]);
        garuda_cfg[dev_id] = NULL;
    }

    return SW_OK;
}

/**
 * @brief reset hsl layer.
 * @details Comments:
 *   This operation will reset hsl layer
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_reset(a_uint32_t dev_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _garuda_reset(dev_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Init hsl layer.
 * @details Comments:
 *   This operation will init hsl layer and hsl layer
 * @param[in] dev_id device id
 * @param[in] cfg configuration for initialization
 * @return SW_OK or error code
 */
sw_error_t
garuda_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    a_uint8_t *p_mem;

    HSL_DEV_ID_CHECK(dev_id);

    p_mem = (a_uint8_t *)garuda_cfg[dev_id];
    if (NULL == p_mem)
    {
        p_mem = aos_mem_alloc(sizeof (ssdk_init_cfg)
                              + sizeof(garuda_init_spec_cfg));
        garuda_cfg[dev_id] = (ssdk_init_cfg *)p_mem;
        garuda_cfg[dev_id]->chip_spec_cfg = (garuda_init_spec_cfg *)
                                            (p_mem + sizeof (ssdk_init_cfg));
    }

    if (NULL == p_mem)
    {
        return SW_OUT_OF_MEM;
    }

    aos_mem_copy(garuda_cfg[dev_id]->chip_spec_cfg,
                 cfg->chip_spec_cfg, sizeof (garuda_init_spec_cfg));
    aos_mem_copy(garuda_cfg[dev_id], cfg, sizeof (ssdk_init_cfg));
    garuda_cfg[dev_id]->chip_spec_cfg = (garuda_init_spec_cfg *)
                                        (p_mem + sizeof (ssdk_init_cfg));

    SW_RTN_ON_ERROR(garuda_reg_access_init(dev_id, cfg->reg_mode));

    SW_RTN_ON_ERROR(garuda_dev_init(dev_id, cfg->cpu_mode));

#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
    {
        a_uint32_t i, entry;
        sw_error_t rv;

        if(HSL_MDIO == cfg->reg_mode)
        {
            SW_RTN_ON_ERROR(garuda_bist_test(dev_id));

            entry = 0x1;
            HSL_REG_FIELD_SET(rv, dev_id, MASK_CTL, 0, SOFT_RST,
                              (a_uint8_t *) (&entry), sizeof (a_uint32_t));
            SW_RTN_ON_ERROR(rv);

            i = 0x10;
            do
            {
                HSL_REG_FIELD_GET(rv, dev_id, MASK_CTL, 0, SOFT_RST,
                                  (a_uint8_t *) (&entry), sizeof (a_uint32_t));
                SW_RTN_ON_ERROR(rv);

                aos_mdelay(10);
            }
            while (entry && --i);

            if (0 == i)
            {
                return SW_INIT_ERROR;
            }
        }
        SW_RTN_ON_ERROR(hsl_port_prop_init(dev_id));
        SW_RTN_ON_ERROR(hsl_port_prop_init_by_dev(dev_id));
        SW_RTN_ON_ERROR(garuda_portproperty_init(dev_id, cfg->cpu_mode));

        GARUDA_MIB_INIT(rv, dev_id);
        GARUDA_PORT_CTRL_INIT(rv, dev_id);
        GARUDA_PORTVLAN_INIT(rv, dev_id);
        GARUDA_VLAN_INIT(rv, dev_id);
        GARUDA_FDB_INIT(rv, dev_id);
        GARUDA_QOS_INIT(rv, dev_id);
        GARUDA_STP_INIT(rv, dev_id);
        GARUDA_MIRR_INIT(rv, dev_id);
        GARUDA_RATE_INIT(rv, dev_id);
        GARUDA_MISC_INIT(rv, dev_id);
        GARUDA_LEAKY_INIT(rv, dev_id);
        GARUDA_IGMP_INIT(rv, dev_id);
        GARUDA_ACL_INIT(rv, dev_id);
        GARUDA_LED_INIT(rv, dev_id);

        {
            hsl_api_t *p_api;

            SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
            p_api->dev_reset   = garuda_reset;
            p_api->dev_clean   = garuda_cleanup;
        }

        if(cfg->reg_mode == HSL_MDIO)
        {
            SW_RTN_ON_ERROR(garuda_hw_init(dev_id, cfg));
        }
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

