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
#include "fal_pppoe.h"
#include "fal_uk_if.h"

sw_error_t
fal_pppoe_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_CMD_SET, dev_id, (a_uint32_t) cmd);
    return rv;
}

sw_error_t
fal_pppoe_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_CMD_GET, dev_id, cmd);
    return rv;
}

sw_error_t
fal_pppoe_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_STATUS_SET, dev_id, (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_pppoe_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_STATUS_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_pppoe_session_add(a_uint32_t dev_id, a_uint32_t session_id, a_bool_t strip_hdr)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_SESSION_ADD, dev_id, session_id, (a_uint32_t)strip_hdr);
    return rv;
}

sw_error_t
fal_pppoe_session_del(a_uint32_t dev_id, a_uint32_t session_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_SESSION_DEL, dev_id, session_id);
    return rv;
}

sw_error_t
fal_pppoe_session_get(a_uint32_t dev_id, a_uint32_t session_id, a_bool_t * strip_hdr)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_SESSION_GET, dev_id, session_id, strip_hdr);
    return rv;
}

sw_error_t
fal_pppoe_session_table_add(a_uint32_t dev_id, fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_SESSION_TABLE_ADD, dev_id, session_tbl);
    return rv;
}

sw_error_t
fal_pppoe_session_table_del(a_uint32_t dev_id, fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_SESSION_TABLE_DEL, dev_id, session_tbl);
    return rv;
}

sw_error_t
fal_pppoe_session_table_get(a_uint32_t dev_id, fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_SESSION_TABLE_GET, dev_id, session_tbl);
    return rv;
}

sw_error_t
fal_pppoe_session_id_set(a_uint32_t dev_id, a_uint32_t index,
                         a_uint32_t id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_SESSION_ID_SET, dev_id, index, id);
    return rv;
}

sw_error_t
fal_pppoe_session_id_get(a_uint32_t dev_id, a_uint32_t index,
                         a_uint32_t * id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_SESSION_ID_GET, dev_id, index, id);
    return rv;
}

sw_error_t
fal_rtd_pppoe_en_set(a_uint32_t dev_id, a_bool_t  enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RTD_PPPOE_EN_SET, dev_id, (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_rtd_pppoe_en_get(a_uint32_t dev_id, a_bool_t *enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RTD_PPPOE_EN_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_pppoe_l3intf_enable(a_uint32_t dev_id, a_uint32_t l3_if, a_uint32_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_EN_SET, dev_id, l3_if, (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_pppoe_l3intf_status_get(a_uint32_t dev_id, a_uint32_t l3_if, a_uint32_t *enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PPPOE_EN_GET, dev_id, l3_if, enable);
    return rv;
}

