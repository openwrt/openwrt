/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
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
#include "fal_qos.h"
#include "cppe_portctrl_reg.h"
#include "cppe_portctrl.h"
#include "adpt.h"

#ifndef IN_QM_MINI
sw_error_t
adpt_cppe_qm_port_source_profile_set(
		a_uint32_t dev_id, fal_port_t port, a_uint32_t src_profile)
{
	union cppe_mru_mtu_ctrl_tbl_u mru_mtu_ctrl_tbl;
	a_uint32_t index = FAL_PORT_ID_VALUE(port);

	ADPT_DEV_ID_CHECK(dev_id);
	memset(&mru_mtu_ctrl_tbl, 0, sizeof(mru_mtu_ctrl_tbl));


	return cppe_mru_mtu_ctrl_tbl_src_profile_set(dev_id, index,
				src_profile);
}

sw_error_t
adpt_cppe_qm_port_source_profile_get(
		a_uint32_t dev_id, fal_port_t port, a_uint32_t *src_profile)
{
	union cppe_mru_mtu_ctrl_tbl_u mru_mtu_ctrl_tbl;
	a_uint32_t index = FAL_PORT_ID_VALUE(port);

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(src_profile);
	memset(&mru_mtu_ctrl_tbl, 0, sizeof(mru_mtu_ctrl_tbl));

	return cppe_mru_mtu_ctrl_tbl_src_profile_get(dev_id, index,
				src_profile);
}
#endif

/**
 * @}
 */
