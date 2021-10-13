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


/**
 * @defgroup fal_pppoe FAL_PPPOE
 * @{
 */
#ifndef _FAL_PPPOE_H_
#define _FAL_PPPOE_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"


enum {
	FUNC_PPPOE_SESSION_TABLE_ADD = 0,
	FUNC_PPPOE_SESSION_TABLE_DEL,
	FUNC_PPPOE_SESSION_TABLE_GET,
	FUNC_PPPOE_EN_SET,
	FUNC_PPPOE_EN_GET,
};

typedef struct
{
	a_uint32_t entry_id;
	a_uint32_t session_id;
	a_bool_t   multi_session;
	a_bool_t   uni_session;
	a_uint32_t vrf_id;
	a_uint32_t port_bitmap;
	a_uint32_t l3_if_index;
	a_bool_t   l3_if_valid;
	a_bool_t   smac_valid;
	fal_mac_addr_t smac_addr;
} fal_pppoe_session_t;

sw_error_t
fal_pppoe_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd);

sw_error_t
fal_pppoe_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

sw_error_t
fal_pppoe_status_set(a_uint32_t dev_id, a_bool_t enable);

sw_error_t
fal_pppoe_status_get(a_uint32_t dev_id, a_bool_t * enable);

sw_error_t
fal_pppoe_session_add(a_uint32_t dev_id, a_uint32_t session_id, a_bool_t strip_hdr);

sw_error_t
fal_pppoe_session_del(a_uint32_t dev_id, a_uint32_t session_id);

sw_error_t
fal_pppoe_session_get(a_uint32_t dev_id, a_uint32_t session_id, a_bool_t * strip_hdr);

sw_error_t
fal_pppoe_session_table_add(a_uint32_t dev_id, fal_pppoe_session_t * session_tbl);

sw_error_t
fal_pppoe_session_table_del(a_uint32_t dev_id, fal_pppoe_session_t * session_tbl);

sw_error_t
fal_pppoe_session_table_get(a_uint32_t dev_id, fal_pppoe_session_t * session_tbl);

sw_error_t
fal_pppoe_session_id_set(a_uint32_t dev_id, a_uint32_t index,
                         a_uint32_t id);

sw_error_t
fal_pppoe_session_id_get(a_uint32_t dev_id, a_uint32_t index,
                         a_uint32_t * id);

sw_error_t
fal_rtd_pppoe_en_set(a_uint32_t dev_id, a_bool_t enable);

sw_error_t
fal_rtd_pppoe_en_get(a_uint32_t dev_id, a_bool_t * enable);

sw_error_t
fal_pppoe_l3intf_enable(a_uint32_t dev_id, a_uint32_t l3_if, a_uint32_t enable);

sw_error_t
fal_pppoe_l3intf_status_get(a_uint32_t dev_id, a_uint32_t l3_if, a_uint32_t *enable);


#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_PPPOE_H_ */
/**
 * @}
 */
