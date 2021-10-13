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
#include "sw_ioctl.h"
#include "fal_sec.h"
#include "fal_uk_if.h"

sw_error_t
fal_sec_norm_item_set(a_uint32_t dev_id, fal_norm_item_t item, void * value)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SEC_NORM_SET, dev_id, item, value);
    return rv;
}

sw_error_t
fal_sec_norm_item_get(a_uint32_t dev_id, fal_norm_item_t item, void * value)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SEC_NORM_GET, dev_id, item, value);
    return rv;
}

sw_error_t
fal_sec_l3_excep_ctrl_set(a_uint32_t dev_id, a_uint32_t excep_type, fal_l3_excep_ctrl_t *ctrl)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SEC_EXP_CTRL_SET, dev_id, excep_type, ctrl);
    return rv;
}

sw_error_t
fal_sec_l3_excep_ctrl_get(a_uint32_t dev_id, a_uint32_t excep_type, fal_l3_excep_ctrl_t *ctrl)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SEC_EXP_CTRL_GET, dev_id, excep_type, ctrl);
    return rv;
}

sw_error_t
fal_sec_l4_excep_parser_ctrl_get(a_uint32_t dev_id, fal_l4_excep_parser_ctrl *ctrl)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SEC_L4_PARSER_CTRL_GET, dev_id, ctrl);
    return rv;
}

sw_error_t
fal_sec_l4_excep_parser_ctrl_set(a_uint32_t dev_id, fal_l4_excep_parser_ctrl *ctrl)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SEC_L4_PARSER_CTRL_SET, dev_id, ctrl);
    return rv;
}

sw_error_t
fal_sec_l3_excep_parser_ctrl_set(a_uint32_t dev_id, fal_l3_excep_parser_ctrl *ctrl)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SEC_L3_PARSER_CTRL_SET, dev_id, ctrl);
    return rv;
}

sw_error_t
fal_sec_l3_excep_parser_ctrl_get(a_uint32_t dev_id, fal_l3_excep_parser_ctrl *ctrl)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_SEC_L3_PARSER_CTRL_GET, dev_id, ctrl);
    return rv;
}


