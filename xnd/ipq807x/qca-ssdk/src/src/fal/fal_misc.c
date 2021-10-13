/*
 * Copyright (c) 2012, 2017, 2020, The Linux Foundation. All rights reserved.
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
* @defgroup fal_gen FAL_MISC
* @{
*/
#include "sw.h"
#include "fal_misc.h"
#include "hsl_api.h"
#include "adpt.h"

#ifndef IN_MISC_MINI
static sw_error_t
_fal_arp_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->arp_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->arp_status_set(dev_id, enable);
    return rv;
}


static sw_error_t
_fal_arp_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->arp_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->arp_status_get(dev_id, enable);
    return rv;
}
#endif

static sw_error_t
_fal_frame_max_size_set(a_uint32_t dev_id, a_uint32_t size)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->frame_max_size_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->frame_max_size_set(dev_id, size);
    return rv;
}


static sw_error_t
_fal_frame_max_size_get(a_uint32_t dev_id, a_uint32_t * size)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->frame_max_size_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->frame_max_size_get(dev_id, size);
    return rv;
}


static sw_error_t
_fal_port_unk_sa_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                         fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_unk_sa_cmd_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_unk_sa_cmd_set(dev_id, port_id, cmd);
    return rv;
}

static sw_error_t
_fal_port_unk_uc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_unk_uc_filter_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_unk_uc_filter_set(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_port_unk_mc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_unk_mc_filter_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_unk_mc_filter_set(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_port_bc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                        a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_bc_filter_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_bc_filter_set(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_cpu_port_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cpu_port_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cpu_port_status_set(dev_id, enable);
    return rv;
}

#ifndef IN_MISC_MINI
static sw_error_t
_fal_port_unk_sa_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                         fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_unk_sa_cmd_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_unk_sa_cmd_get(dev_id, port_id, cmd);
    return rv;
}


static sw_error_t
_fal_port_unk_uc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_unk_uc_filter_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_unk_uc_filter_get(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_port_unk_mc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_unk_mc_filter_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_unk_mc_filter_get(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_port_bc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                        a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_bc_filter_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_bc_filter_get(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_cpu_port_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cpu_port_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cpu_port_status_get(dev_id, enable);
    return rv;
}


static sw_error_t
_fal_bc_to_cpu_port_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->bc_to_cpu_port_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->bc_to_cpu_port_set(dev_id, enable);
    return rv;
}


static sw_error_t
_fal_bc_to_cpu_port_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->bc_to_cpu_port_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->bc_to_cpu_port_get(dev_id, enable);
    return rv;
}


static sw_error_t
_fal_port_dhcp_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_dhcp_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_dhcp_set(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_port_dhcp_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_dhcp_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_dhcp_get(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_arp_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->arp_cmd_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->arp_cmd_set(dev_id, cmd);
    return rv;
}

static sw_error_t
_fal_arp_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->arp_cmd_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->arp_cmd_get(dev_id, cmd);
    return rv;
}
#endif

static sw_error_t
_fal_eapol_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->eapol_cmd_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->eapol_cmd_set(dev_id, cmd);
    return rv;
}

#ifndef IN_MISC_MINI
static sw_error_t
_fal_eapol_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->eapol_cmd_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->eapol_cmd_get(dev_id, cmd);
    return rv;
}
#endif

static sw_error_t
_fal_eapol_status_set(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->eapol_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->eapol_status_set(dev_id, port_id, enable);
    return rv;
}

#ifndef IN_MISC_MINI
static sw_error_t
_fal_eapol_status_get(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->eapol_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->eapol_status_get(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_ripv1_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ripv1_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->ripv1_status_set(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_ripv1_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->ripv1_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->ripv1_status_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_port_arp_req_status_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_arp_req_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_arp_req_status_set(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_port_arp_req_status_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_arp_req_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_arp_req_status_get(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_port_arp_ack_status_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_arp_ack_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_arp_ack_status_set(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_port_arp_ack_status_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_arp_ack_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_arp_ack_status_get(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_intr_mask_set(a_uint32_t dev_id, a_uint32_t intr_mask)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->intr_mask_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->intr_mask_set(dev_id, intr_mask);
    return rv;
}

static sw_error_t
_fal_intr_mask_get(a_uint32_t dev_id, a_uint32_t * intr_mask)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->intr_mask_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->intr_mask_get(dev_id, intr_mask);
    return rv;
}

static sw_error_t
_fal_intr_status_get(a_uint32_t dev_id, a_uint32_t * intr_status)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->intr_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->intr_status_get(dev_id, intr_status);
    return rv;
}

static sw_error_t
_fal_intr_status_clear(a_uint32_t dev_id, a_uint32_t intr_status)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->intr_status_clear)
        return SW_NOT_SUPPORTED;

    rv = p_api->intr_status_clear(dev_id, intr_status);
    return rv;
}

static sw_error_t
_fal_intr_port_link_mask_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t intr_mask)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_intr_port_link_mask_set != NULL) {
        rv = p_adpt_api->adpt_intr_port_link_mask_set(dev_id, port_id, intr_mask);
        return rv;
   }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->intr_port_link_mask_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->intr_port_link_mask_set(dev_id, port_id, intr_mask);
    return rv;
}

static sw_error_t
_fal_intr_port_link_mask_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t * intr_mask)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_intr_port_link_mask_get != NULL) {
        rv = p_adpt_api->adpt_intr_port_link_mask_get(dev_id, port_id, intr_mask);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->intr_port_link_mask_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->intr_port_link_mask_get(dev_id, port_id, intr_mask);
    return rv;
}

static sw_error_t
_fal_intr_port_link_status_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t * intr_mask)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL &&
        p_adpt_api->adpt_intr_port_link_status_get != NULL) {
        rv = p_adpt_api->adpt_intr_port_link_status_get(dev_id, port_id, intr_mask);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->intr_port_link_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->intr_port_link_status_get(dev_id, port_id, intr_mask);
    return rv;
}



static sw_error_t
_fal_intr_mask_mac_linkchg_set(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->intr_mask_mac_linkchg_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->intr_mask_mac_linkchg_set(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_intr_mask_mac_linkchg_get(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->intr_mask_mac_linkchg_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->intr_mask_mac_linkchg_get(dev_id, port_id, enable);
    return rv;
}


static sw_error_t
_fal_intr_status_mac_linkchg_get(a_uint32_t dev_id, fal_pbmp_t* port_bitmap)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->intr_status_mac_linkchg_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->intr_status_mac_linkchg_get(dev_id, port_bitmap);
    return rv;
}

static sw_error_t
_fal_cpu_vid_en_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cpu_vid_en_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->cpu_vid_en_set(dev_id, enable);
    return rv;
}


static sw_error_t
_fal_cpu_vid_en_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->cpu_vid_en_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->cpu_vid_en_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_intr_status_mac_linkchg_clear(a_uint32_t dev_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->intr_status_mac_linkchg_clear)
        return SW_NOT_SUPPORTED;

    rv = p_api->intr_status_mac_linkchg_clear(dev_id);
    return rv;
}


static sw_error_t
_fal_global_macaddr_set(a_uint32_t dev_id, fal_mac_addr_t * addr)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->global_macaddr_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->global_macaddr_set(dev_id, addr);
    return rv;
}

static sw_error_t
_fal_global_macaddr_get(a_uint32_t dev_id, fal_mac_addr_t * addr)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->global_macaddr_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->global_macaddr_get(dev_id, addr);
    return rv;
}

static sw_error_t
_fal_lldp_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->lldp_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->lldp_status_set(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_lldp_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->lldp_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->lldp_status_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_frame_crc_reserve_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->frame_crc_reserve_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->frame_crc_reserve_set(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_frame_crc_reserve_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->frame_crc_reserve_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->frame_crc_reserve_get(dev_id, enable);
    return rv;
}

sw_error_t
_fal_debug_port_counter_enable(a_uint32_t dev_id, fal_port_t port_id, fal_counter_en_t * cnt_en)
{
    sw_error_t rv;
    adpt_api_t *p_api;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_debug_port_counter_enable)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_debug_port_counter_enable(dev_id, port_id, cnt_en);
    return rv;
}

sw_error_t
_fal_debug_port_counter_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_counter_en_t * cnt_en)
{
    sw_error_t rv;
    adpt_api_t *p_api;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_debug_port_counter_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_debug_port_counter_status_get(dev_id, port_id, cnt_en);
    return rv;
}

/**
 * @brief Set arp packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_arp_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_arp_status_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_arp_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_arp_status_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}
#endif

/**
 * @brief Set max frame size which device can received on a particular device.
 * @details    Comments:
 * The granularity of packets size is byte.
 * @param[in] dev_id device id
 * @param[in] size packet size
 * @return SW_OK or error code
 */
sw_error_t
fal_frame_max_size_set(a_uint32_t dev_id, a_uint32_t size)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_frame_max_size_set(dev_id, size);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get max frame size which device can received on a particular device.
 * @details   Comments:
 *    The unit of packets size is byte.
 * @param[in] dev_id device id
 * @param[out] size packet size
 * @return SW_OK or error code
 */
sw_error_t
fal_frame_max_size_get(a_uint32_t dev_id, a_uint32_t * size)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_frame_max_size_get(dev_id, size);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set forwarding command for packets which source address is unknown on a particular port.
 * @details Comments:
 *    Particular device may only support parts of forwarding commands.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_port_unk_sa_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                        fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_unk_sa_cmd_set(dev_id, port_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set flooding status of unknown unicast packets on a particular port.
 * @details  Comments:
 *   If enable unknown unicast packets filter on one port then unknown
 *   unicast packets can't flood out from this port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_unk_uc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_unk_uc_filter_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set flooding status of unknown multicast packets on a particular port.
 * @details  Comments:
 *   If enable unknown multicast packets filter on one port then unknown
 *   multicast packets can't flood out from this port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_unk_mc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_unk_mc_filter_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set flooding status of broadcast packets on a particular port.
 * @details  Comments:
 *   If enable unknown multicast packets filter on one port then unknown
 *   multicast packets can't flood out from this port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_bc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                       a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_bc_filter_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set cpu port status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_cpu_port_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cpu_port_status_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_MISC_MINI
/**
 * @brief Get forwarding command for packets which source address is unknown on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_port_unk_sa_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                        fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_unk_sa_cmd_get(dev_id, port_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get flooding status of unknown unicast packets on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_unk_uc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_unk_uc_filter_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}



/**
 * @brief Get flooding status of unknown multicast packets on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_unk_mc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_unk_mc_filter_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}



/**
 * @brief Get flooding status of broadcast packets on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_bc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                       a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_bc_filter_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get cpu port status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_cpu_port_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cpu_port_status_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set status of braodcast packets broadcasting to cpu on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_bc_to_cpu_port_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_bc_to_cpu_port_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get status of braodcast packets broadcasting to cpu on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_bc_to_cpu_port_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_bc_to_cpu_port_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dhcp packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_dhcp_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_dhcp_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dhcp packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_dhcp_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_dhcp_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp packets forwarding command on a particular device.
 * @details    Comments:
 *    Particular device may only support parts of forwarding commands.
 *    This operation will take effect only after enabling arp
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_arp_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_arp_cmd_set(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp packets forwarding command on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_arp_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_arp_cmd_get(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}
#endif
/**
 * @brief Set eapol packets forwarding command on a particular device.
 * @details    Comments:
 *    Particular device may only support parts of forwarding commands.
 *    This operation will take effect only after enabling eapol
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_eapol_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_eapol_cmd_set(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_MISC_MINI
/**
 * @brief Get eapol packets forwarding command on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_eapol_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_eapol_cmd_get(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}
#endif
/**
 * @brief Set eapol packets hardware acknowledgement on a particular port.
 * @details    Comments:
 *    Particular device may only support parts of forwarding commands.
 *    This operation will take effect only after enabling eapol
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_eapol_status_set(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_eapol_status_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}
#ifndef IN_MISC_MINI
/**
 * @brief Get eapol packets hardware acknowledgement on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_eapol_status_get(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_eapol_status_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set ripv1 packets hardware acknowledgement on a particular port.
 * @details    Comments:
 *    Particular device may only support parts of forwarding commands.
 *    This operation will take effect only after enabling eapol
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_ripv1_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ripv1_status_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get ripv1 packets hardware acknowledgement on a particular port.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_ripv1_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ripv1_status_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp req packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_arp_req_status_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_arp_req_status_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp req packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_arp_req_status_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_arp_req_status_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp ack packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_arp_ack_status_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_arp_ack_status_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp ack packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_port_arp_ack_status_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_arp_ack_status_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set switch interrupt mask on one particular device.
 * @param[in] dev_id device id
 * @param[in] intr_mask mask
 * @return SW_OK or error code
 */
sw_error_t
fal_intr_mask_set(a_uint32_t dev_id, a_uint32_t intr_mask)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_intr_mask_set(dev_id, intr_mask);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get switch interrupt mask on one particular device.
 * @param[in] dev_id device id
 * @param[in] intr_mask mask
 * @return SW_OK or error code
 */
sw_error_t
fal_intr_mask_get(a_uint32_t dev_id, a_uint32_t * intr_mask)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_intr_mask_get(dev_id, intr_mask);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get switch interrupt status on one particular device.
 * @param[in] dev_id device id
 * @param[in] intr_status status
 * @return SW_OK or error code
 */
sw_error_t
fal_intr_status_get(a_uint32_t dev_id, a_uint32_t * intr_status)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_intr_status_get(dev_id, intr_status);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Clear switch interrupt status on one particular device.
 * @param[in] dev_id device id
 * @param[in] intr_status status
 * @return SW_OK or error code
 */
sw_error_t
fal_intr_status_clear(a_uint32_t dev_id, a_uint32_t intr_status)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_intr_status_clear(dev_id, intr_status);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set link interrupt mask on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] intr_mask_flag interrupt mask
 * @return SW_OK or error code
 */
sw_error_t
fal_intr_port_link_mask_set(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t intr_mask_flag)
{
    sw_error_t rv;
    FAL_API_LOCK;
    rv = _fal_intr_port_link_mask_set(dev_id, port_id, intr_mask_flag);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get link interrupt mask on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] intr_mask_flag interrupt mask
 * @return SW_OK or error code
 */
sw_error_t
fal_intr_port_link_mask_get(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t * intr_mask_flag)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_intr_port_link_mask_get(dev_id, port_id, intr_mask_flag);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get link interrupt status on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] intr_mask_flag interrupt mask
 * @return SW_OK or error code
 */
sw_error_t
fal_intr_port_link_status_get(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t * intr_mask_flag)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_intr_port_link_status_get(dev_id, port_id, intr_mask_flag);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set mac link change interrupt mask on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable ports intr mask enabled
 * @return SW_OK or error code
 */
sw_error_t
fal_intr_mask_mac_linkchg_set(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    FAL_API_LOCK;
    rv = _fal_intr_mask_mac_linkchg_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get mac link change interrupt mask on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] port interrupt mask or not
 * @return SW_OK or error code
 */
sw_error_t
fal_intr_mask_mac_linkchg_get(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_intr_mask_mac_linkchg_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get link change interrupt status for all ports.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] ports bitmap which generates interrupt
 * @return SW_OK or error code
 */
sw_error_t
fal_intr_status_mac_linkchg_get(a_uint32_t dev_id, fal_pbmp_t* port_bitmap)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_intr_status_mac_linkchg_get(dev_id, port_bitmap);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set to cpu vid enable status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_cpu_vid_en_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cpu_vid_en_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get to cpu vid enable status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_cpu_vid_en_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_cpu_vid_en_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get mac link change interrupt mask on particular port.
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
sw_error_t
fal_intr_status_mac_linkchg_clear(a_uint32_t dev_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_intr_status_mac_linkchg_clear(dev_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set global macaddr on particular device.
 * @param[in] dev_id device id
 * @param[in] addr   addr
 * @return SW_OK or error code
 */
sw_error_t
fal_global_macaddr_set(a_uint32_t dev_id, fal_mac_addr_t * addr)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_global_macaddr_set(dev_id, addr);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get global macaddr on particular device.
 * @param[in]  dev_id device id
 * @param[out] addr   addr
 * @return SW_OK or error code
 */
sw_error_t
fal_global_macaddr_get(a_uint32_t dev_id, fal_mac_addr_t * addr)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_global_macaddr_get(dev_id, addr);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set lldp packets hardware acknowledgement status on particular device.
 * @details     comments:
 *   Particular device may only support parts of pppoe packets.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_lldp_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_lldp_status_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get lldp packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_lldp_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_lldp_status_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set frame crc reserve enable on particular device.
 * @details     comments:
 *   CRC reseve enable.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_frame_crc_reserve_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_frame_crc_reserve_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get frame crc reserve enable on particular device.
 * @details     comments:
 *   CRC reseve enable.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_frame_crc_reserve_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_frame_crc_reserve_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_debug_port_counter_enable(a_uint32_t dev_id, fal_port_t port_id, fal_counter_en_t * cnt_en)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_debug_port_counter_enable(dev_id, port_id, cnt_en);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_debug_port_counter_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_counter_en_t * cnt_en)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_debug_port_counter_status_get(dev_id, port_id, cnt_en);
    FAL_API_UNLOCK;
    return rv;
}
#endif



/**
 * @}
 */

