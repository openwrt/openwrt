/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
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
 * @defgroup
 * @{
 */
#include "sw.h"
#include "hppe_fdb_reg.h"
#include "hppe_fdb.h"
#include "hppe_ip_reg.h"
#include "hppe_ip.h"
#include "adpt.h"

sw_error_t
adpt_cppe_flow_copy_escape_set(a_uint32_t dev_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union l2_global_conf_u l2_global_conf;
	union l3_route_ctrl_ext_u l3_route_ctrl_ext;

	ADPT_DEV_ID_CHECK(dev_id);

	memset(&l2_global_conf, 0, sizeof(l2_global_conf));
	rv = hppe_l2_global_conf_get(dev_id, &l2_global_conf);
	SW_RTN_ON_ERROR(rv);
	l2_global_conf.bf.l2_flow_copy_escape = enable;
	rv = hppe_l2_global_conf_set(dev_id, &l2_global_conf);
	SW_RTN_ON_ERROR(rv);

	memset(&l3_route_ctrl_ext, 0, sizeof(l3_route_ctrl_ext));
	rv = hppe_l3_route_ctrl_ext_get(dev_id, &l3_route_ctrl_ext);
	SW_RTN_ON_ERROR(rv);
	l3_route_ctrl_ext.bf.l3_flow_copy_escape = enable;
	rv = hppe_l3_route_ctrl_ext_set(dev_id, &l3_route_ctrl_ext);

	return rv;
}

sw_error_t
adpt_cppe_flow_copy_escape_get(a_uint32_t dev_id, a_bool_t *enable)
{

	sw_error_t rv = SW_OK;
	union l2_global_conf_u l2_global_conf;
	union l3_route_ctrl_ext_u l3_route_ctrl_ext;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	memset(&l2_global_conf, 0, sizeof(l2_global_conf));
	rv = hppe_l2_global_conf_get(dev_id, &l2_global_conf);
	SW_RTN_ON_ERROR(rv);

	memset(&l3_route_ctrl_ext, 0, sizeof(l3_route_ctrl_ext));
	rv = hppe_l3_route_ctrl_ext_get(dev_id, &l3_route_ctrl_ext);
	SW_RTN_ON_ERROR(rv);

	*enable = l2_global_conf.bf.l2_flow_copy_escape & l3_route_ctrl_ext.bf.l3_flow_copy_escape;
	return rv;
}

/**
 * @}
 */
