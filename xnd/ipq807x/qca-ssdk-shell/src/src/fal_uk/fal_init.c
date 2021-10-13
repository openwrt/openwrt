/*
 * Copyright (c) 2014, 2016-2017, The Linux Foundation. All rights reserved.
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


/*qca808x_start*/
#include "sw.h"
#include "sw_ioctl.h"
#include "ssdk_init.h"
#include "fal_init.h"
#include "fal_uk_if.h"
/*qca808x_end*/
sw_error_t
fal_reset(a_uint32_t dev_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SWITCH_RESET, dev_id);
    return rv;
}
/*qca808x_start*/
sw_error_t
fal_ssdk_cfg(a_uint32_t dev_id, ssdk_cfg_t *ssdk_cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SSDK_CFG, dev_id, ssdk_cfg);
    return rv;
}
/*qca808x_end*/
sw_error_t
fal_module_func_ctrl_set(a_uint32_t dev_id, a_uint32_t module, fal_func_ctrl_t *func_ctrl)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MODULE_FUNC_CTRL_SET, dev_id, module, func_ctrl);
    return rv;
}

sw_error_t
fal_module_func_ctrl_get(a_uint32_t dev_id, a_uint32_t module, fal_func_ctrl_t *func_ctrl)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_MODULE_FUNC_CTRL_GET, dev_id, module, func_ctrl);
    return rv;
}



