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
 * @defgroup horus_init HORUS_INIT
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "horus_mib.h"
#include "horus_port_ctrl.h"
#include "horus_portvlan.h"
#include "horus_vlan.h"
#include "horus_fdb.h"
#include "horus_qos.h"
#include "horus_mirror.h"
#include "horus_stp.h"
#include "horus_rate.h"
#include "horus_misc.h"
#include "horus_leaky.h"
#include "horus_igmp.h"
#include "horus_led.h"
#include "horus_reg_access.h"
#include "horus_reg.h"
#include "f2_phy.h"

static ssdk_init_cfg * horus_cfg[SW_MAX_NR_DEV] = { 0 };

#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
static sw_error_t
horus_portproperty_init(a_uint32_t dev_id, hsl_init_mode mode)
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
                    /* include cpu port and wan port in some cases */
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

static sw_error_t
horus_hw_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    hsl_dev_t *pdev = NULL;
    a_uint32_t port_id;
    a_uint32_t data;
    sw_error_t rv;

    pdev = hsl_dev_ptr_get(dev_id);
    if (NULL == pdev)
    {
        return SW_NOT_INITIALIZED;
    }

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
horus_bist_test(a_uint32_t dev_id)
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
horus_dev_init(a_uint32_t dev_id, hsl_init_mode cpu_mode)
{
    hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;

    pdev->nr_ports = 6;
    pdev->nr_phy = 5;
    pdev->cpu_port_nr = 0;
    pdev->nr_vlans = 16;
    pdev->hw_vlan_query = A_TRUE;
    pdev->nr_queue = 4;
    pdev->cpu_mode = cpu_mode;

    return SW_OK;
}

static sw_error_t
_horus_reset(a_uint32_t dev_id)
{
#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    val = 0x1;
    HSL_REG_FIELD_SET(rv, dev_id, MASK_CTL, 0, SOFT_RST,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = horus_hw_init(dev_id, horus_cfg[dev_id]);
    SW_RTN_ON_ERROR(rv);
#endif

    return SW_OK;
}


sw_error_t
horus_cleanup(a_uint32_t dev_id)
{

    if (horus_cfg[dev_id])
    {
        aos_mem_free(horus_cfg[dev_id]);
        horus_cfg[dev_id] = NULL;
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
sw_error_t
horus_reset(a_uint32_t dev_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _horus_reset(dev_id);
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
horus_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    HSL_DEV_ID_CHECK(dev_id);

    if (NULL == horus_cfg[dev_id])
    {
        horus_cfg[dev_id] = aos_mem_alloc(sizeof (ssdk_init_cfg));
    }

    if (NULL == horus_cfg[dev_id])
    {
        return SW_OUT_OF_MEM;
    }

    aos_mem_copy(horus_cfg[dev_id], cfg, sizeof (ssdk_init_cfg));

    SW_RTN_ON_ERROR(horus_reg_access_init(dev_id, cfg->reg_mode));

    SW_RTN_ON_ERROR(horus_dev_init(dev_id, cfg->cpu_mode));

#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
    {
        a_uint32_t i, entry;
        sw_error_t rv;

        SW_RTN_ON_ERROR(horus_bist_test(dev_id));

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

        SW_RTN_ON_ERROR(hsl_port_prop_init(dev_id));
        SW_RTN_ON_ERROR(hsl_port_prop_init_by_dev(dev_id));
        SW_RTN_ON_ERROR(horus_portproperty_init(dev_id, cfg->cpu_mode));

        HORUS_MIB_INIT(rv, dev_id);
        HORUS_PORT_CTRL_INIT(rv, dev_id);
        HORUS_PORTVLAN_INIT(rv, dev_id);
        HORUS_VLAN_INIT(rv, dev_id);
        HORUS_FDB_INIT(rv, dev_id);
        HORUS_QOS_INIT(rv, dev_id);
        HORUS_STP_INIT(rv, dev_id);
        HORUS_MIRR_INIT(rv, dev_id);
        HORUS_RATE_INIT(rv, dev_id);
        HORUS_MISC_INIT(rv, dev_id);
        HORUS_LEAKY_INIT(rv, dev_id);
        HORUS_IGMP_INIT(rv, dev_id);
        HORUS_LED_INIT(rv, dev_id);

        {
            hsl_api_t *p_api;

            SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
            p_api->dev_reset = horus_reset;
            p_api->dev_clean = horus_cleanup;
        }

        SW_RTN_ON_ERROR(horus_hw_init(dev_id, cfg));
    }
#endif

    return SW_OK;
}


/**
 * @}
 */
