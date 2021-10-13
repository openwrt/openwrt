/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
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
#include "sw_ioctl.h"
#include "fal_interface_ctrl.h"
#include "fal_uk_if.h"

sw_error_t
fal_port_3az_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_3AZ_STATUS_SET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_port_3az_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_3AZ_STATUS_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_interface_mac_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_mac_config_t * config)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MAC_MODE_SET, dev_id, port_id, config);
    return rv;
}

sw_error_t
fal_interface_mac_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_mac_config_t * config)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MAC_MODE_GET, dev_id, port_id, config);
    return rv;
}

sw_error_t
fal_interface_phy_mode_set(a_uint32_t dev_id, a_uint32_t phy_id, fal_phy_config_t * config)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PHY_MODE_SET, dev_id, phy_id, config);
    return rv;
}

sw_error_t
fal_interface_phy_mode_get(a_uint32_t dev_id, a_uint32_t phy_id, fal_phy_config_t * config)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PHY_MODE_GET, dev_id, phy_id, config);
    return rv;
}

sw_error_t
fal_interface_fx100_ctrl_set(a_uint32_t dev_id, fal_fx100_ctrl_config_t * config)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FX100_CTRL_SET, dev_id, config);
    return rv;
}

sw_error_t
fal_interface_fx100_ctrl_get(a_uint32_t dev_id, fal_fx100_ctrl_config_t * config)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FX100_CTRL_GET, dev_id, config);
    return rv;
}

sw_error_t
fal_interface_fx100_status_get(a_uint32_t dev_id, a_uint32_t *status)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FX100_STATUS_GET, dev_id, status);
    return rv;
}

sw_error_t
fal_interface_mac06_exch_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MAC06_EXCH_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_interface_mac06_exch_get(a_uint32_t dev_id, a_bool_t* enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MAC06_EXCH_GET, dev_id, enable);
    return rv;
}
