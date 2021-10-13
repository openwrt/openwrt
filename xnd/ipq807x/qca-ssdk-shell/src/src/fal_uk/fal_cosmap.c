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
#include "fal_cosmap.h"
#include "fal_uk_if.h"


sw_error_t
fal_cosmap_dscp_to_pri_set(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t pri)
{
    sw_error_t rv;

    rv= sw_uk_exec(SW_API_COSMAP_DSCP_TO_PRI_SET, dev_id, dscp, pri);
    return rv;
}

sw_error_t
fal_cosmap_dscp_to_pri_get(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t * pri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_DSCP_TO_PRI_GET, dev_id, dscp, pri);
    return rv;
}

sw_error_t
fal_cosmap_dscp_to_dp_set(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t dp)
{
    sw_error_t rv;

    rv= sw_uk_exec(SW_API_COSMAP_DSCP_TO_DP_SET, dev_id, dscp, dp);
    return rv;
}

sw_error_t
fal_cosmap_dscp_to_dp_get(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t * dp)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_DSCP_TO_DP_GET, dev_id, dscp, dp);
    return rv;
}

sw_error_t
fal_cosmap_up_to_pri_set(a_uint32_t dev_id, a_uint32_t up, a_uint32_t pri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_UP_TO_PRI_SET, dev_id, up, pri);
    return rv;
}

sw_error_t
fal_cosmap_up_to_pri_get(a_uint32_t dev_id, a_uint32_t up, a_uint32_t * pri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_UP_TO_PRI_GET, dev_id, up, pri);
    return rv;
}

sw_error_t
fal_cosmap_up_to_dp_set(a_uint32_t dev_id, a_uint32_t up, a_uint32_t dp)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_UP_TO_DP_SET, dev_id, up, dp);
    return rv;
}

sw_error_t
fal_cosmap_up_to_dp_get(a_uint32_t dev_id, a_uint32_t up, a_uint32_t * dp)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_UP_TO_DP_GET, dev_id, up, dp);
    return rv;
}

sw_error_t
fal_cosmap_pri_to_queue_set(a_uint32_t dev_id, a_uint32_t pri,
                            a_uint32_t queue)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_PRI_TO_QU_SET, dev_id, pri, queue);
    return rv;
}

sw_error_t
fal_cosmap_pri_to_queue_get(a_uint32_t dev_id, a_uint32_t pri,
                            a_uint32_t * queue)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_PRI_TO_QU_GET, dev_id, pri, queue);
    return rv;
}

sw_error_t
fal_cosmap_pri_to_ehqueue_set(a_uint32_t dev_id, a_uint32_t pri,
                              a_uint32_t queue)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_PRI_TO_EHQU_SET, dev_id, pri, queue);
    return rv;
}

sw_error_t
fal_cosmap_pri_to_ehqueue_get(a_uint32_t dev_id, a_uint32_t pri,
                              a_uint32_t * queue)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_PRI_TO_EHQU_GET, dev_id, pri, queue);
    return rv;
}

sw_error_t
fal_cosmap_egress_remark_set(a_uint32_t dev_id, a_uint32_t tbl_id,
                             fal_egress_remark_table_t * tbl)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_EG_REMARK_SET, dev_id, tbl_id, tbl);
    return rv;
}

sw_error_t
fal_cosmap_egress_remark_get(a_uint32_t dev_id, a_uint32_t tbl_id,
                              fal_egress_remark_table_t * tbl)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_EG_REMARK_GET, dev_id, tbl_id, tbl);
    return rv;
}

sw_error_t
fal_cosmap_dscp_to_ehpri_set(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t pri)
{
    sw_error_t rv;

    rv= sw_uk_exec(SW_API_COSMAP_DSCP_TO_EHPRI_SET, dev_id, dscp, pri);
    return rv;
}

sw_error_t
fal_cosmap_dscp_to_ehpri_get(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t * pri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_DSCP_TO_EHPRI_GET, dev_id, dscp, pri);
    return rv;
}

sw_error_t
fal_cosmap_dscp_to_ehdp_set(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t dp)
{
    sw_error_t rv;

    rv= sw_uk_exec(SW_API_COSMAP_DSCP_TO_EHDP_SET, dev_id, dscp, dp);
    return rv;
}

sw_error_t
fal_cosmap_dscp_to_ehdp_get(a_uint32_t dev_id, a_uint32_t dscp, a_uint32_t * dp)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_DSCP_TO_EHDP_GET, dev_id, dscp, dp);
    return rv;
}

sw_error_t
fal_cosmap_up_to_ehpri_set(a_uint32_t dev_id, a_uint32_t up, a_uint32_t pri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_UP_TO_EHPRI_SET, dev_id, up, pri);
    return rv;
}

sw_error_t
fal_cosmap_up_to_ehpri_get(a_uint32_t dev_id, a_uint32_t up, a_uint32_t * pri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_UP_TO_EHPRI_GET, dev_id, up, pri);
    return rv;
}

sw_error_t
fal_cosmap_up_to_ehdp_set(a_uint32_t dev_id, a_uint32_t up, a_uint32_t dp)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_UP_TO_EHDP_SET, dev_id, up, dp);
    return rv;
}

sw_error_t
fal_cosmap_up_to_ehdp_get(a_uint32_t dev_id, a_uint32_t up, a_uint32_t * dp)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_UP_TO_EHDP_GET, dev_id, up, dp);
    return rv;
}