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
#include "sw_ioctl.h"
#include "fal_vsi.h"
#include "fal_uk_if.h"

sw_error_t
fal_port_vsi_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t *vsi_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_VSI_GET, dev_id, port_id, vsi_id);
    return rv;
}

sw_error_t
fal_port_vlan_vsi_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t vsi_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_VLAN_VSI_SET, dev_id, port_id, stag_vid, ctag_vid, vsi_id);
    return rv;
}

sw_error_t
fal_vsi_free(a_uint32_t dev_id, a_uint32_t vsi)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VSI_FREE, dev_id, vsi);
    return rv;
}

sw_error_t
fal_port_vlan_vsi_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t *vsi_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_VLAN_VSI_GET, dev_id, port_id, stag_vid, ctag_vid, vsi_id);
    return rv;
}

sw_error_t
fal_port_vsi_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t vsi_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PORT_VSI_SET, dev_id, port_id, vsi_id);
    return rv;
}

sw_error_t
fal_vsi_alloc(a_uint32_t dev_id, a_uint32_t *vsi)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VSI_ALLOC, dev_id, vsi);
    return rv;
}

sw_error_t
fal_vsi_tbl_dump(a_uint32_t dev_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VSI_TBL_DUMP, dev_id);
    return rv;
}

sw_error_t
fal_vsi_stamove_set(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_stamove_t *stamove)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VSI_STAMOVE_SET, dev_id, vsi_id, stamove);
    return rv;
}
sw_error_t
fal_vsi_stamove_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_stamove_t *stamove)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VSI_STAMOVE_GET, dev_id, vsi_id, stamove);
    return rv;
}
sw_error_t
fal_vsi_newaddr_lrn_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_newaddr_lrn_t *newaddr_lrn)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VSI_NEWADDR_LRN_GET, dev_id, vsi_id, newaddr_lrn);
    return rv;
}
sw_error_t
fal_vsi_newaddr_lrn_set(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_newaddr_lrn_t *newaddr_lrn)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VSI_NEWADDR_LRN_SET, dev_id, vsi_id, newaddr_lrn);
    return rv;
}
sw_error_t
fal_vsi_member_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_member_t *vsi_member)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VSI_MEMBER_GET, dev_id, vsi_id, vsi_member);
    return rv;
}
sw_error_t
fal_vsi_member_set(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_member_t *vsi_member)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VSI_MEMBER_SET, dev_id, vsi_id, vsi_member);
    return rv;
}

sw_error_t
fal_vsi_counter_get(a_uint32_t dev_id, a_uint32_t vsi_id, fal_vsi_counter_t *counter)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VSI_COUNTER_GET, dev_id, vsi_id, counter);
    return rv;
}

sw_error_t
fal_vsi_counter_cleanup(a_uint32_t dev_id, a_uint32_t vsi_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_VSI_COUNTER_CLEANUP, dev_id, vsi_id);
    return rv;
}


