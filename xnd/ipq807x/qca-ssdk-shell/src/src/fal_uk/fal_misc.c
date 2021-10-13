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
#include "fal_misc.h"
#include "fal_uk_if.h"

sw_error_t
fal_arp_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_ARP_STATUS_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_arp_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_ARP_STATUS_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_frame_max_size_set(a_uint32_t dev_id, a_uint32_t size)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FRAME_MAX_SIZE_SET, dev_id, size);
    return rv;
}

sw_error_t
fal_frame_max_size_get(a_uint32_t dev_id, a_uint32_t * size)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FRAME_MAX_SIZE_GET, dev_id, size);
    return rv;
}

sw_error_t
fal_port_unk_sa_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                        fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_UNK_SA_CMD_SET, dev_id, port_id,
                    cmd);
    return rv;
}

sw_error_t
fal_port_unk_sa_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                        fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_UNK_SA_CMD_GET, dev_id, port_id,
                    cmd);
    return rv;
}

sw_error_t
fal_port_unk_uc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_UNK_UC_FILTER_SET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_port_unk_uc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_UNK_UC_FILTER_GET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_port_unk_mc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_UNK_MC_FILTER_SET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_port_unk_mc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_UNK_MC_FILTER_GET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_port_bc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                       a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_BC_FILTER_SET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_port_bc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                       a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_BC_FILTER_GET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_cpu_port_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_CPU_PORT_STATUS_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_cpu_port_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_CPU_PORT_STATUS_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_bc_to_cpu_port_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BC_TO_CPU_PORT_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_bc_to_cpu_port_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_BC_TO_CPU_PORT_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_port_dhcp_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_DHCP_SET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_port_dhcp_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_DHCP_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_arp_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_ARP_CMD_SET, dev_id, cmd);
    return rv;
}

sw_error_t
fal_arp_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_ARP_CMD_GET, dev_id, cmd);
    return rv;
}

sw_error_t
fal_eapol_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_EAPOL_CMD_SET, dev_id, cmd);
    return rv;
}

sw_error_t
fal_eapol_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_EAPOL_CMD_GET, dev_id, cmd);
    return rv;
}

sw_error_t
fal_eapol_status_set(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_EAPOL_STATUS_SET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_eapol_status_get(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_EAPOL_STATUS_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_ripv1_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RIPV1_STATUS_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_ripv1_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_RIPV1_STATUS_GET, dev_id, enable);
    return rv;
}


sw_error_t
fal_port_arp_req_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_ARP_REQ_STATUS_SET, dev_id, port_id, enable);
    return rv;
}


sw_error_t
fal_port_arp_req_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_ARP_REQ_STATUS_GET, dev_id, port_id, enable);
    return rv;
}


sw_error_t
fal_port_arp_ack_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_ARP_ACK_STATUS_SET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_port_arp_ack_status_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PT_ARP_ACK_STATUS_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_intr_mask_set(a_uint32_t dev_id, a_uint32_t intr_mask)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_INTR_MASK_SET, dev_id, intr_mask);
    return rv;
}

sw_error_t
fal_intr_mask_get(a_uint32_t dev_id, a_uint32_t * intr_mask)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_INTR_MASK_GET, dev_id, intr_mask);
    return rv;
}

sw_error_t
fal_intr_status_get(a_uint32_t dev_id, a_uint32_t * intr_status)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_INTR_STATUS_GET, dev_id, intr_status);
    return rv;
}

sw_error_t
fal_intr_status_clear(a_uint32_t dev_id, a_uint32_t intr_status)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_INTR_STATUS_CLEAR, dev_id, intr_status);
    return rv;
}

sw_error_t
fal_intr_port_link_mask_set(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t intr_mask)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_INTR_PORT_LINK_MASK_SET, dev_id, port_id, intr_mask);
    return rv;
}

sw_error_t
fal_intr_port_link_mask_get(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t * intr_mask)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_INTR_PORT_LINK_MASK_GET, dev_id, port_id, intr_mask);
    return rv;
}

sw_error_t
fal_intr_port_link_status_get(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t * intr_mask)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_INTR_PORT_LINK_STATUS_GET, dev_id, port_id, intr_mask);
    return rv;
}

sw_error_t
fal_intr_mask_mac_linkchg_set(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_INTR_MASK_MAC_LINKCHG_SET, dev_id, port_id, enable);
    return rv;
}


sw_error_t
fal_intr_mask_mac_linkchg_get(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_INTR_MASK_MAC_LINKCHG_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_intr_status_mac_linkchg_get(a_uint32_t dev_id, fal_pbmp_t *port_bitmap)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_INTR_STATUS_MAC_LINKCHG_GET, dev_id, port_bitmap);
    return rv;
}

sw_error_t
fal_cpu_vid_en_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_CPU_VID_EN_SET, dev_id,  enable);
    return rv;
}

sw_error_t
fal_cpu_vid_en_get(a_uint32_t dev_id, a_bool_t *enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_CPU_VID_EN_GET, dev_id,  enable);
    return rv;
}

sw_error_t
fal_intr_status_mac_linkchg_clear(a_uint32_t dev_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_INTR_STATUS_MAC_LINKCHG_CLEAR, dev_id);
    return rv;
}

sw_error_t
fal_global_macaddr_set(a_uint32_t dev_id, fal_mac_addr_t * addr)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_GLOBAL_MACADDR_SET, dev_id, addr);
    return rv;
}

sw_error_t
fal_global_macaddr_get(a_uint32_t dev_id, fal_mac_addr_t * addr)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_GLOBAL_MACADDR_GET, dev_id, addr);
    return rv;
}

sw_error_t
fal_lldp_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_LLDP_STATUS_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_lldp_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_LLDP_STATUS_GET, dev_id, enable);
    return rv;
}


sw_error_t
fal_frame_crc_reserve_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FRAME_CRC_RESERVE_SET, dev_id, enable);
    return rv;
}

sw_error_t
fal_frame_crc_reserve_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_FRAME_CRC_RESERVE_GET, dev_id, enable);
    return rv;
}

sw_error_t
fal_debug_port_counter_enable(a_uint32_t dev_id, fal_port_t port_id, fal_counter_en_t * cnt_en)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_DEBUG_PORT_COUNTER_ENABLE, dev_id, port_id, cnt_en);
    return rv;
}

sw_error_t
fal_debug_port_counter_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_counter_en_t * cnt_en)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_DEBUG_PORT_COUNTER_STATUS_GET, dev_id, port_id, cnt_en);
    return rv;
}
