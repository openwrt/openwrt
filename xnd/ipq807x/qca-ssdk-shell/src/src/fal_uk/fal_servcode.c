/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
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
#include "fal_servcode.h"
#include "fal_uk_if.h"

sw_error_t fal_servcode_config_set(a_uint32_t dev_id, a_uint32_t servcode_index,
					fal_servcode_config_t *entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SERVCODE_CONFIG_SET, dev_id, servcode_index,
                    entry);
    return rv;
}

sw_error_t fal_servcode_config_get(a_uint32_t dev_id, a_uint32_t servcode_index,
					fal_servcode_config_t *entry)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SERVCODE_CONFIG_GET, dev_id, servcode_index,
                    entry);
    return rv;
}

sw_error_t fal_servcode_loopcheck_en(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SERVCODE_LOOPCHECK_EN, dev_id, (a_uint32_t) enable);
    return rv;
}

sw_error_t fal_servcode_loopcheck_status_get(a_uint32_t dev_id, a_bool_t *enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SERVCODE_LOOPCHECK_STATUS_GET, dev_id, enable);
    return rv;
}

