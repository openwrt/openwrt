/*
 * Copyright (c) 2016-2018, The Linux Foundation. All rights reserved.
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
#ifndef REF_VSI_H
#define REF_VSI_H
#include "sw.h"
#include "fal/fal_type.h"
#include "fal/fal_vsi.h"


#define REF_DEV_ID_CHECK(dev_id) \
do { \
    if (dev_id >= SW_MAX_NR_DEV) \
        return SW_OUT_OF_RANGE; \
} while (0)

#define REF_PORT_ID_CHECK(port_id) \
do { \
    if (port_id >= SW_MAX_NR_PORT) \
        return SW_OUT_OF_RANGE; \
} while (0)

#define REF_NULL_POINT_CHECK(point) \
do { \
    if (point == NULL) \
        return SW_BAD_PTR; \
} while (0)


enum{
	PPE_VSI_ADD,
	PPE_VSI_DEL
};
typedef struct REF_VLAN_INFO_T {
	a_uint32_t stag_vid;
	a_uint32_t ctag_vid;
	a_uint32_t vport_bitmap; /*vlan based vsi*/
	struct REF_VLAN_INFO_T *pNext;
}ref_vlan_info_t;

typedef struct{
	a_uint32_t valid;
	a_uint32_t pport_bitmap; /*port based vsi*/
	ref_vlan_info_t *pHead;
}ref_vsi_t;

#define PPE_VSI_PPORT_NR 7
#define PPE_VSI_INVALID FAL_VSI_INVALID

sw_error_t ppe_port_vlan_vsi_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t vsi_id);
sw_error_t ppe_port_vlan_vsi_get(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t *vsi_id);
sw_error_t ppe_port_vsi_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t vsi_id);
sw_error_t ppe_port_vsi_get(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t *vsi_id);
sw_error_t ppe_vsi_alloc(a_uint32_t dev_id, a_uint32_t *vsi);
sw_error_t ppe_vsi_free(a_uint32_t dev_id, a_uint32_t vsi_id);
sw_error_t ppe_vsi_tbl_dump(a_uint32_t dev_id);
sw_error_t ppe_vsi_init(a_uint32_t dev_id);
#endif

