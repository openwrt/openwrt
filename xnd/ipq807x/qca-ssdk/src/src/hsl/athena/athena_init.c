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
 * @defgroup athena_init ATHENA_INIT
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "athena_mib.h"
#include "athena_port_ctrl.h"
#include "athena_portvlan.h"
#include "athena_vlan.h"
#include "athena_fdb.h"
#include "athena_reg_access.h"
#include "athena_reg.h"
#include "athena_init.h"

static ssdk_init_cfg * athena_cfg[SW_MAX_NR_DEV] = { 0 };

#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
static sw_error_t
athena_portproperty_init(a_uint32_t dev_id, hsl_init_mode mode)
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
                    if (HSL_CPU_1_PLUS == mode)
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

static sw_error_t
athena_hw_init(a_uint32_t dev_id)
{
    hsl_dev_t  *pdev = NULL;
    a_uint32_t port_id, data;
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


#endif

static sw_error_t
athena_dev_init(a_uint32_t dev_id)
{
    hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;

    pdev->nr_ports = 6;
    pdev->nr_phy = 5;
    pdev->cpu_port_nr = 0;
    pdev->nr_vlans = 16;
    pdev->hw_vlan_query = A_FALSE;
    pdev->nr_queue = 4;

    return SW_OK;
}

static sw_error_t
_athena_reset(a_uint32_t dev_id)
{
#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
    a_uint32_t val;
    sw_error_t rv;

    val = 0x1;
    HSL_REG_FIELD_SET(rv, dev_id, MASK_CTL, 0, SOFT_RST,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    ATHENA_VLAN_RESET(rv, dev_id);

    rv = athena_hw_init(dev_id);
    SW_RTN_ON_ERROR(rv);
#endif

    return SW_OK;
}

sw_error_t
athena_cleanup(a_uint32_t dev_id)
{
    if (athena_cfg[dev_id])
    {
        aos_mem_free(athena_cfg[dev_id]);
        athena_cfg[dev_id] = NULL;
    }

#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
    {
        sw_error_t rv;
        ATHENA_VLAN_CLEANUP(rv, dev_id);
    }
#endif

    return SW_OK;
}

/**
 * @brief Reset Athena module.
 * @details Comments:
 *    This operation will reset athena.
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
athena_reset(a_uint32_t dev_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_reset(dev_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Init Athena module.
 * @details Comments:
 *    This operation will init athena.
 * @param[in] dev_id device id
 * @param[in] cfg configuration for initialization
 * @return SW_OK or error code
 */
sw_error_t
athena_init(a_uint32_t dev_id, ssdk_init_cfg * cfg)
{
    HSL_DEV_ID_CHECK(dev_id);

    if (NULL == athena_cfg[dev_id])
    {
        athena_cfg[dev_id] = (ssdk_init_cfg *)aos_mem_alloc(sizeof (ssdk_init_cfg));
    }

    if (NULL == athena_cfg[dev_id])
    {
        return SW_OUT_OF_MEM;
    }

    aos_mem_copy(athena_cfg[dev_id], cfg, sizeof (ssdk_init_cfg));

    SW_RTN_ON_ERROR(athena_reg_access_init(dev_id, cfg->reg_mode));

    SW_RTN_ON_ERROR(athena_dev_init(dev_id));

#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
    {
        sw_error_t rv;
        SW_RTN_ON_ERROR(hsl_port_prop_init(dev_id));
        SW_RTN_ON_ERROR(hsl_port_prop_init_by_dev(dev_id));
        SW_RTN_ON_ERROR(athena_portproperty_init(dev_id, cfg->cpu_mode));

        ATHENA_MIB_INIT(rv, dev_id);
        ATHENA_PORT_CTRL_INIT(rv, dev_id);
        ATHENA_PORTVLAN_INIT(rv, dev_id);
        ATHENA_VLAN_INIT(rv, dev_id);
        ATHENA_FDB_INIT(rv, dev_id);

        {
            hsl_api_t *p_api;

            SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
            p_api->dev_reset   = athena_reset;
            p_api->dev_clean   = athena_cleanup;
        }

        SW_RTN_ON_ERROR(athena_hw_init(dev_id));
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

