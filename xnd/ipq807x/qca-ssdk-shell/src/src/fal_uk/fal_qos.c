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
#include "fal_qos.h"
#include "fal_uk_if.h"

sw_error_t
fal_qos_sch_mode_set(a_uint32_t dev_id,
                     fal_sch_mode_t mode, const a_uint32_t weight[])
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_SCH_MODE_SET, dev_id, mode,
                    weight);
    return rv;
}

sw_error_t
fal_qos_sch_mode_get(a_uint32_t dev_id,
                     fal_sch_mode_t * mode, a_uint32_t weight[])
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_SCH_MODE_GET, dev_id, mode,
                    weight);
    return rv;
}

sw_error_t
fal_qos_queue_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_QU_TX_BUF_ST_SET, dev_id, port_id,
                    (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_qos_queue_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_QU_TX_BUF_ST_GET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_qos_queue_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                            fal_queue_t queue_id, a_uint32_t * number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_QU_TX_BUF_NR_SET, dev_id, port_id, queue_id,
                    number);
    return rv;
}

sw_error_t
fal_qos_queue_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                            fal_queue_t queue_id, a_uint32_t * number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_QU_TX_BUF_NR_GET, dev_id, port_id, queue_id,
                    number);
    return rv;
}

sw_error_t
fal_qos_port_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_TX_BUF_ST_SET, dev_id, port_id,
                    (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_qos_port_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_TX_BUF_ST_GET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_qos_port_red_en_set(a_uint32_t dev_id, fal_port_t port_id,
                        a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_RED_EN_SET, dev_id, port_id,
                    (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_qos_port_red_en_get(a_uint32_t dev_id, fal_port_t port_id,
                        a_bool_t* enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_RED_EN_GET, dev_id, port_id,
                    enable);
    return rv;
}

sw_error_t
fal_qos_port_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_TX_BUF_NR_SET, dev_id, port_id,
                    number);
    return rv;
}

sw_error_t
fal_qos_port_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_TX_BUF_NR_GET, dev_id, port_id,
                    number);
    return rv;
}

sw_error_t
fal_qos_port_rx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_RX_BUF_NR_SET, dev_id, port_id,
                    number);
    return rv;
}

sw_error_t
fal_qos_port_rx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                           a_uint32_t * number)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_RX_BUF_NR_GET, dev_id, port_id,
                    number);
    return rv;
}

sw_error_t
fal_cosmap_up_queue_set(a_uint32_t dev_id, a_uint32_t up, fal_queue_t queue)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_UP_QU_SET, dev_id, up,
                    (a_uint32_t) queue);
    return rv;
}

sw_error_t
fal_cosmap_up_queue_get(a_uint32_t dev_id, a_uint32_t up,
                        fal_queue_t * queue)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_UP_QU_GET, dev_id, up,
                    queue);
    return rv;
}

sw_error_t
fal_cosmap_dscp_queue_set(a_uint32_t dev_id, a_uint32_t dscp, fal_queue_t queue)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_DSCP_QU_SET, dev_id, dscp,
                    (a_uint32_t) queue);
    return rv;
}

sw_error_t
fal_cosmap_dscp_queue_get(a_uint32_t dev_id, a_uint32_t dscp,
                          fal_queue_t * queue)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_COSMAP_DSCP_QU_GET, dev_id, dscp,
                    queue);
    return rv;
}

sw_error_t
fal_qos_port_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                      fal_qos_mode_t mode, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_MODE_SET, dev_id, port_id, mode,
                    (a_uint32_t) enable);
    return rv;
}

sw_error_t
fal_qos_port_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                      fal_qos_mode_t mode, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_MODE_GET, dev_id, port_id, mode,
                    enable);
    return rv;
}

sw_error_t
fal_qos_port_mode_pri_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_qos_mode_t mode, a_uint32_t pri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_MODE_PRI_SET, dev_id, port_id, mode, pri);
    return rv;
}

sw_error_t
fal_qos_port_mode_pri_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_qos_mode_t mode, a_uint32_t * pri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_MODE_PRI_GET, dev_id, port_id, mode,
                    pri);
    return rv;
}

sw_error_t
fal_qos_port_default_up_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t up)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PORT_DEF_UP_SET, dev_id, port_id, up);
    return rv;
}

sw_error_t
fal_qos_port_default_up_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_uint32_t * up)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PORT_DEF_UP_GET, dev_id, port_id, up);
    return rv;
}

sw_error_t
fal_qos_port_sch_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
                          fal_sch_mode_t mode, const a_uint32_t weight[])
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PORT_SCH_MODE_SET, dev_id, port_id, mode,
                    weight);
    return rv;
}

sw_error_t
fal_qos_port_sch_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
                          fal_sch_mode_t * mode, a_uint32_t weight[])
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PORT_SCH_MODE_GET, dev_id, port_id, mode,
                    weight);
    return rv;
}

sw_error_t
fal_qos_port_default_spri_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t spri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_DEF_SPRI_SET, dev_id, port_id, spri);
    return rv;
}

sw_error_t
fal_qos_port_default_spri_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t * spri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_DEF_SPRI_GET, dev_id, port_id, spri);
    return rv;
}

sw_error_t
fal_qos_port_default_cpri_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t cpri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_DEF_CPRI_SET, dev_id, port_id, cpri);
    return rv;
}

sw_error_t
fal_qos_port_default_cpri_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t * cpri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_DEF_CPRI_GET, dev_id, port_id, cpri);
    return rv;
}

sw_error_t
fal_qos_port_force_spri_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_FORCE_SPRI_ST_SET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_qos_port_force_spri_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t* enable)
{
    sw_error_t rv;
    rv = sw_uk_exec(SW_API_QOS_PT_FORCE_SPRI_ST_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_qos_port_force_cpri_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_FORCE_CPRI_ST_SET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_qos_port_force_cpri_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t* enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PT_FORCE_CPRI_ST_GET, dev_id, port_id, enable);
    return rv;
}

sw_error_t
fal_qos_queue_remark_table_set(a_uint32_t dev_id, fal_port_t port_id,
                               fal_queue_t queue_id, a_uint32_t tbl_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_QUEUE_REMARK_SET, dev_id, port_id, queue_id, tbl_id, enable);
    return rv;
}


sw_error_t
fal_qos_queue_remark_table_get(a_uint32_t dev_id, fal_port_t port_id,
                               fal_queue_t queue_id, a_uint32_t * tbl_id, a_bool_t * enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_QUEUE_REMARK_GET, dev_id, port_id, queue_id, tbl_id, enable);
    return rv;
}

sw_error_t
fal_qos_port_group_set(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_group_t *group)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PORT_GROUP_SET, dev_id, port_id, group);
    return rv;
}

sw_error_t
fal_qos_port_group_get(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_group_t *group)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PORT_GROUP_GET, dev_id, port_id, group);
    return rv;
}

sw_error_t
fal_qos_port_pri_precedence_set(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_pri_precedence_t *pri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PORT_PRI_SET, dev_id, port_id, pri);
    return rv;
}

sw_error_t
fal_qos_port_pri_precedence_get(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_pri_precedence_t *pri)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PORT_PRI_GET, dev_id, port_id, pri);
    return rv;
}

sw_error_t
fal_qos_port_remark_set(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_remark_enable_t *remark)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PORT_REMARK_SET, dev_id, port_id, remark);
    return rv;
}

sw_error_t
fal_qos_port_remark_get(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_remark_enable_t *remark)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PORT_REMARK_GET, dev_id, port_id, remark);
    return rv;
}

sw_error_t
fal_qos_cosmap_pcp_set(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t pcp, fal_qos_cosmap_t *cosmap)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PCP_MAP_SET, dev_id, group_id, pcp, cosmap);
    return rv;
}

sw_error_t
fal_qos_cosmap_pcp_get(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t pcp, fal_qos_cosmap_t *cosmap)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PCP_MAP_GET, dev_id, group_id, pcp, cosmap);
    return rv;
}

sw_error_t
fal_qos_cosmap_flow_set(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint16_t flow, fal_qos_cosmap_t *cosmap)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_FLOW_MAP_SET, dev_id, group_id, flow, cosmap);
    return rv;
}

sw_error_t
fal_qos_cosmap_flow_get(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint16_t flow, fal_qos_cosmap_t *cosmap)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_FLOW_MAP_GET, dev_id, group_id, flow, cosmap);
    return rv;
}

sw_error_t
fal_qos_cosmap_dscp_set(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t dscp, fal_qos_cosmap_t *cosmap)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_DSCP_MAP_SET, dev_id, group_id, dscp, cosmap);
    return rv;
}

sw_error_t
fal_qos_cosmap_dscp_get(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t dscp, fal_qos_cosmap_t *cosmap)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_DSCP_MAP_GET, dev_id, group_id, dscp, cosmap);
    return rv;
}

sw_error_t
fal_queue_scheduler_set(a_uint32_t dev_id, a_uint32_t node_id,
					fal_queue_scheduler_level_t level, fal_port_t port_id,
					fal_qos_scheduler_cfg_t *scheduler_cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_QUEUE_SCHEDULER_SET, dev_id, node_id, level, port_id, scheduler_cfg);
    return rv;
}

sw_error_t
fal_queue_scheduler_get(a_uint32_t dev_id, a_uint32_t node_id,
					fal_queue_scheduler_level_t level, fal_port_t *port_id,
					fal_qos_scheduler_cfg_t *scheduler_cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_QUEUE_SCHEDULER_GET, dev_id, node_id, level, port_id, scheduler_cfg);
    return rv;
}

sw_error_t
fal_edma_ring_queue_map_set(a_uint32_t dev_id, 
					a_uint32_t ring_id, fal_queue_bmp_t *queue_bmp)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_RING_QUEUE_MAP_SET, dev_id, ring_id, queue_bmp);
    return rv;
}

sw_error_t
fal_edma_ring_queue_map_get(a_uint32_t dev_id, 
					a_uint32_t ring_id, fal_queue_bmp_t *queue_bmp)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_RING_QUEUE_MAP_GET, dev_id, ring_id, queue_bmp);
    return rv;
}

sw_error_t
fal_port_queues_get(a_uint32_t dev_id, 
				fal_port_t port_id, fal_queue_bmp_t *queue_bmp)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PORT_QUEUES_GET, dev_id, port_id, queue_bmp);
    return rv;
}

sw_error_t
fal_scheduler_dequeue_ctrl_set(a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_SCHEDULER_DEQUEU_CTRL_SET, dev_id, queue_id, enable);
    return rv;
}

sw_error_t
fal_scheduler_dequeue_ctrl_get(a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t *enable)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_SCHEDULER_DEQUEU_CTRL_GET, dev_id, queue_id, enable);
    return rv;
}

sw_error_t
fal_port_scheduler_cfg_reset(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PORT_SCHEDULER_CFG_RESET, dev_id, port_id);
    return rv;
}

sw_error_t
fal_port_scheduler_resource_get(
		a_uint32_t dev_id,
		fal_port_t port_id,
		fal_portscheduler_resource_t *cfg)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_QOS_PORT_SCHEDULER_RESOURCE_GET, dev_id, port_id, cfg);
    return rv;
}