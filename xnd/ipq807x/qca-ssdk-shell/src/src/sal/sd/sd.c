/*
 * Copyright (c) 2014, 2017, The Linux Foundation. All rights reserved.
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
#include "ssdk_init.h"
#include "sd.h"
#include "sw_api.h"
#if ((!defined(KERNEL_MODULE)) && defined(UK_IF))
#include "sw_api_us.h"
#endif

mdio_reg_set ssdk_mdio_set    = NULL;
mdio_reg_get ssdk_mdio_get    = NULL;
hdr_reg_set  ssdk_hdr_reg_set = NULL;
hdr_reg_get  ssdk_hdr_reg_get = NULL;
uniphy_reg_set  ssdk_uniphy_reg_set = NULL;
uniphy_reg_get  ssdk_uniphy_reg_get = NULL;
mii_reg_set	ssdk_mii_reg_set = NULL;
mii_reg_get	ssdk_mii_reg_get = NULL;

sw_error_t
sd_reg_mdio_set(a_uint32_t dev_id, a_uint32_t phy, a_uint32_t reg,
                a_uint16_t data)
{
    sw_error_t rv = SW_OK;

    if (NULL != ssdk_mdio_set)
    {
        rv = ssdk_mdio_set(dev_id, phy, reg, data);
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    return rv;
}

sw_error_t
sd_reg_mdio_get(a_uint32_t dev_id, a_uint32_t phy, a_uint32_t reg, a_uint16_t * data)
{
    sw_error_t rv = SW_OK;

    if (NULL != ssdk_mdio_get)
    {
        rv = ssdk_mdio_get(dev_id, phy, reg, data);
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    return rv;
}

sw_error_t
sd_reg_hdr_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t * reg_data, a_uint32_t len)
{
    sw_error_t rv;

    if (NULL != ssdk_hdr_reg_set)
    {
        rv = ssdk_hdr_reg_set(dev_id, reg_addr, reg_data, len);
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    return rv;
}

sw_error_t
sd_reg_hdr_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t * reg_data, a_uint32_t len)
{
    sw_error_t rv;

    if (NULL != ssdk_hdr_reg_get)
    {
        rv = ssdk_hdr_reg_get(dev_id, reg_addr, reg_data, len);
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    return rv;
}

sw_error_t
sd_reg_uniphy_set(a_uint32_t dev_id, a_uint32_t index, a_uint32_t reg_addr,
		a_uint8_t * reg_data, a_uint32_t len)
{
    sw_error_t rv;

    if (NULL != ssdk_uniphy_reg_set)
    {
        rv = ssdk_uniphy_reg_set(dev_id, index, reg_addr, reg_data, len);
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    return rv;
}

sw_error_t
sd_reg_uniphy_get(a_uint32_t dev_id, a_uint32_t index, a_uint32_t reg_addr,
		a_uint8_t * reg_data, a_uint32_t len)
{
    sw_error_t rv;

    if (NULL != ssdk_uniphy_reg_get)
    {
        rv = ssdk_uniphy_reg_get(dev_id, index, reg_addr, reg_data, len);
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    return rv;
}

void
sd_reg_mii_set(a_uint32_t reg, a_uint32_t val)
{
    if (NULL != ssdk_mii_reg_set)
    {
        ssdk_mii_reg_set(reg, val);
    }
}

a_uint32_t
sd_reg_mii_get(a_uint32_t reg)
{
    a_uint32_t value = 0;

    if (NULL != ssdk_mii_reg_get)
    {
        value = ssdk_mii_reg_get(reg);
    }

    return value;
}

sw_error_t
sd_init(a_uint32_t dev_id, ssdk_init_cfg * cfg)
{
    if (NULL != cfg->reg_func.mdio_set)
    {
        ssdk_mdio_set = cfg->reg_func.mdio_set;
    }

    if (NULL != cfg->reg_func.mdio_get)
    {
        ssdk_mdio_get = cfg->reg_func.mdio_get;
    }

    if (NULL != cfg->reg_func.header_reg_set)
    {
        ssdk_hdr_reg_set = cfg->reg_func.header_reg_set;
    }

    if (NULL != cfg->reg_func.header_reg_get)
    {
        ssdk_hdr_reg_get = cfg->reg_func.header_reg_get;
    }
    if (NULL != cfg->reg_func.uniphy_reg_set)
    {
        ssdk_uniphy_reg_set = cfg->reg_func.uniphy_reg_set;
    }

    if (NULL != cfg->reg_func.uniphy_reg_get)
    {
        ssdk_uniphy_reg_get = cfg->reg_func.uniphy_reg_get;
    }

    if (NULL != cfg->reg_func.mii_reg_set)
    {
        ssdk_mii_reg_set = cfg->reg_func.mii_reg_set;
    }

    if (NULL != cfg->reg_func.mii_reg_get)
    {
        ssdk_mii_reg_get = cfg->reg_func.mii_reg_get;
    }

    return SW_OK;
}

