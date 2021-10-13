/*
 * Copyright (c) 2012, 2016-2018, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_qos FAL_QOS
 * @{
 */
#ifndef _FAL_QOS_H_
#define _FAL_QOS_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "sw.h"
#include "fal/fal_type.h"

    /**
    @brief This enum defines traffic scheduling mode.
    */
    typedef enum {
        FAL_SCH_SP_MODE = 0,    /**<  strict priority scheduling mode */
        FAL_SCH_WRR_MODE,       /**<   weight round robin scheduling mode*/
        FAL_SCH_MIX_MODE,       /**<  sp and wrr mixed scheduling mode */
        FAL_SCH_MIX_PLUS_MODE,  /**<  sp and wrr mixed plus scheduling mode */
        FAL_SCH_MODE_BUTT
    }
    fal_sch_mode_t;

    /**
    @brief This enum defines qos assignment mode.
    */
    typedef enum
    {
        FAL_QOS_DA_MODE = 0,    /**<   qos assignment based on destination mac address*/
        FAL_QOS_UP_MODE,        /**<   qos assignment based on 802.1p field in vlan tag*/
        FAL_QOS_DSCP_MODE,      /**<  qos assignment based on dscp field in ip header */
        FAL_QOS_PORT_MODE,      /**<  qos assignment based on port */
        FAL_QOS_FLOW_MODE,      /**<  qos assignment based on flow */
        FAL_QOS_MODE_BUTT
    } fal_qos_mode_t;

typedef struct {
	a_uint8_t pcp_group; /* 0: group 0 1: group 1 */
	a_uint8_t dscp_group;
	a_uint8_t flow_group;
} fal_qos_group_t;

typedef struct {
	a_uint8_t pcp_pri;
	a_uint8_t dscp_pri;
	a_int8_t preheader_pri;
	a_uint8_t flow_pri;
	a_uint8_t acl_pri;
	a_uint8_t post_acl_pri;
	a_bool_t pcp_pri_force;
	a_bool_t dscp_pri_force;
} fal_qos_pri_precedence_t;

typedef struct {
	a_bool_t pcp_change_en;
	a_bool_t dei_chage_en;
	a_bool_t dscp_change_en;
} fal_qos_remark_enable_t;

typedef struct {
	a_uint8_t internal_pcp;
	a_uint8_t internal_dei;
	a_uint8_t internal_pri;
	a_uint8_t internal_dscp;
	a_uint8_t internal_dp;
	a_uint8_t dscp_mask;
	a_bool_t dscp_en;
	a_bool_t pcp_en;
	a_bool_t dei_en;
	a_bool_t pri_en;
	a_bool_t dp_en;
	a_uint8_t qos_prec; /* resolution precedence */
} fal_qos_cosmap_t;

typedef enum {
	FAL_DRR_IPG_PREAMBLE_FRAME_CRC = 0, /* IPG + Preamble + Frame + CRC */
	FAL_DRR_FRAME_CRC, /* Frame + CRC */
	FAL_DRR_L3_EXCLUDE_CRC  /* after Ethernet type excude CRC*/
} fal_qos_drr_frame_mode_t;

typedef struct {
	a_uint8_t sp_id; /* SP id L0:0~63 L1:0~7 */
	a_uint8_t e_pri; /*SP priority for E path:0~7 low to high */
	a_uint8_t c_pri; /* SP priority for C path: 0~7 low to high */
	a_uint8_t c_drr_id; /*C DRR ID L0:0~159 L1:0~35*/
	a_uint8_t e_drr_id; /*E DRR ID L0:0~159 L1:0~35*/
	a_uint16_t e_drr_wt; /* DRR weight in E DRR: 0~1023 */
	a_uint16_t c_drr_wt; /* DRR weight in C DRR: 0~1023 */
	a_uint8_t c_drr_unit; /* 0:byte based; 1:packet based */
	a_uint8_t e_drr_unit; /* 0:byte based; 1:packet based */
	fal_qos_drr_frame_mode_t drr_frame_mode;
} fal_qos_scheduler_cfg_t;

typedef struct {
	a_uint32_t en_scheduler_port_bmp; /* port bitmap of en-scheduler */
	a_uint32_t en_scheduler_port; /* port of en-scheduler */
	a_uint32_t de_scheduler_port; /* port of de-shceduler */
} fal_port_scheduler_cfg_t;

typedef enum {
	FAL_QUEUE_SCHEDULER_LEVEL0 = 0,
	FAL_QUEUE_SCHEDULER_LEVEL1,
} fal_queue_scheduler_level_t;


typedef struct {
	a_uint32_t bmp[10];
} fal_queue_bmp_t;

enum {
	FUNC_QOS_PORT_PRI_SET = 0,
	FUNC_QOS_PORT_PRI_GET,
	FUNC_QOS_COSMAP_PCP_GET,
	FUNC_QUEUE_SCHEDULER_SET,
	FUNC_QUEUE_SCHEDULER_GET,
	FUNC_PORT_QUEUES_GET,
	FUNC_QOS_COSMAP_PCP_SET,
	FUNC_QOS_PORT_REMARK_GET,
	FUNC_QOS_COSMAP_DSCP_GET,
	FUNC_QOS_COSMAP_FLOW_SET,
	FUNC_QOS_PORT_GROUP_SET,
	FUNC_RING_QUEUE_MAP_SET,
	FUNC_QOS_COSMAP_DSCP_SET,
	FUNC_QOS_PORT_REMARK_SET,
	FUNC_QOS_COSMAP_FLOW_GET,
	FUNC_QOS_PORT_GROUP_GET,
	FUNC_RING_QUEUE_MAP_GET,
	FUNC_TDM_TICK_NUM_SET,
	FUNC_TDM_TICK_NUM_GET,
	FUNC_PORT_SCHEDULER_CFG_SET,
	FUNC_PORT_SCHEDULER_CFG_GET,
	FUNC_SCHEDULER_DEQUEUE_CTRL_GET,
	FUNC_SCHEDULER_DEQUEUE_CTRL_SET,
	FUNC_QOS_PORT_MODE_PRI_GET,
	FUNC_QOS_PORT_MODE_PRI_SET,
	FUNC_QOS_PORT_SCHEDULER_CFG_RESET,
	FUNC_QOS_PORT_SCHEDULER_RESOURCE_GET,
};

typedef struct {
	a_uint16_t ucastq_start;
	a_uint16_t ucastq_num;
	a_uint16_t mcastq_start;
	a_uint16_t mcastq_num;
	a_uint16_t l0sp_start;
	a_uint16_t l0sp_num;
	a_uint16_t l0cdrr_start;
	a_uint16_t l0cdrr_num;
	a_uint16_t l0edrr_start;
	a_uint16_t l0edrr_num;
	a_uint16_t l1sp_start;
	a_uint16_t l1sp_num;
	a_uint16_t l1cdrr_start;
	a_uint16_t l1cdrr_num;
	a_uint16_t l1edrr_start;
	a_uint16_t l1edrr_num;
} fal_portscheduler_resource_t;

#define FAL_DOT1P_MIN    0
#define FAL_DOT1P_MAX    7

#define FAL_DSCP_MIN     0
#define FAL_DSCP_MAX     63

#ifndef IN_QOS_MINI
    sw_error_t
    fal_qos_sch_mode_set(a_uint32_t dev_id,
                         fal_sch_mode_t mode, const a_uint32_t weight[]);


    sw_error_t
    fal_qos_sch_mode_get(a_uint32_t dev_id,
                         fal_sch_mode_t * mode, a_uint32_t weight[]);


    sw_error_t
    fal_qos_queue_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                    a_bool_t enable);

    sw_error_t
    fal_qos_queue_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                    a_bool_t * enable);


    sw_error_t
    fal_qos_queue_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                                fal_queue_t queue_id, a_uint32_t * number);


    sw_error_t
    fal_qos_port_tx_buf_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t enable);


    sw_error_t
    fal_qos_port_tx_buf_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                   a_bool_t * enable);

    

    sw_error_t
    fal_qos_port_red_en_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable);


    


    sw_error_t
    fal_qos_port_tx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t * number);

    sw_error_t
    fal_qos_port_rx_buf_nr_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t * number);

    sw_error_t
    fal_cosmap_up_queue_set(a_uint32_t dev_id, a_uint32_t up,
                            fal_queue_t queue);


    sw_error_t
    fal_cosmap_up_queue_get(a_uint32_t dev_id, a_uint32_t up,
                            fal_queue_t * queue);


    sw_error_t
    fal_cosmap_dscp_queue_set(a_uint32_t dev_id, a_uint32_t dscp,
                              fal_queue_t queue);


    sw_error_t
    fal_cosmap_dscp_queue_get(a_uint32_t dev_id, a_uint32_t dscp,
                              fal_queue_t * queue);
#endif
	sw_error_t
    fal_qos_port_rx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t * number);
	sw_error_t
    fal_qos_port_red_en_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable);
    sw_error_t
    fal_qos_port_mode_set(a_uint32_t dev_id, fal_port_t port_id,
                          fal_qos_mode_t mode, a_bool_t enable);

	sw_error_t
    fal_qos_port_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t * number);

    
	sw_error_t
    fal_qos_queue_tx_buf_nr_set(a_uint32_t dev_id, fal_port_t port_id,
                                fal_queue_t queue_id, a_uint32_t * number);

#ifndef IN_QOS_MINI
sw_error_t
    fal_qos_port_mode_get(a_uint32_t dev_id, fal_port_t port_id,
                          fal_qos_mode_t mode, a_bool_t * enable);

    sw_error_t
    fal_qos_port_mode_pri_set(a_uint32_t dev_id, fal_port_t port_id,
                              fal_qos_mode_t mode, a_uint32_t pri);


    sw_error_t
    fal_qos_port_mode_pri_get(a_uint32_t dev_id, fal_port_t port_id,
                              fal_qos_mode_t mode, a_uint32_t * pri);


    sw_error_t
    fal_qos_port_default_up_set(a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t up);


    sw_error_t
    fal_qos_port_default_up_get(a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t * up);

    sw_error_t
    fal_qos_port_sch_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
                              fal_sch_mode_t mode, const a_uint32_t weight[]);

    sw_error_t
    fal_qos_port_sch_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
                              fal_sch_mode_t * mode, a_uint32_t weight[]);

    sw_error_t
    fal_qos_port_default_spri_set(a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t spri);

    sw_error_t
    fal_qos_port_default_spri_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t * spri);

    sw_error_t
    fal_qos_port_default_cpri_set(a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t cpri);

    sw_error_t
    fal_qos_port_default_cpri_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t * cpri);

    sw_error_t
    fal_qos_port_force_spri_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                       a_bool_t enable);

    sw_error_t
    fal_qos_port_force_spri_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                       a_bool_t* enable);

    sw_error_t
    fal_qos_port_force_cpri_status_set(a_uint32_t dev_id, fal_port_t port_id,
                                       a_bool_t enable);

    sw_error_t
    fal_qos_port_force_cpri_status_get(a_uint32_t dev_id, fal_port_t port_id,
                                       a_bool_t* enable);

    sw_error_t
    fal_qos_queue_remark_table_set(a_uint32_t dev_id, fal_port_t port_id,
                                   fal_queue_t queue_id, a_uint32_t tbl_id, a_bool_t enable);


    sw_error_t
    fal_qos_queue_remark_table_get(a_uint32_t dev_id, fal_port_t port_id,
                                   fal_queue_t queue_id, a_uint32_t * tbl_id, a_bool_t * enable);
#endif

sw_error_t
fal_qos_port_group_set(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_group_t *group);

sw_error_t
fal_qos_port_group_get(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_group_t *group);

sw_error_t
fal_qos_port_pri_precedence_set(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_pri_precedence_t *pri);

sw_error_t
fal_qos_port_pri_precedence_get(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_pri_precedence_t *pri);

sw_error_t
fal_qos_port_remark_set(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_remark_enable_t *remark);

sw_error_t
fal_qos_port_remark_get(a_uint32_t dev_id, fal_port_t port_id,
					fal_qos_remark_enable_t *remark);

sw_error_t
fal_qos_cosmap_pcp_set(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t pcp, fal_qos_cosmap_t *cosmap);

sw_error_t
fal_qos_cosmap_pcp_get(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t pcp, fal_qos_cosmap_t *cosmap);

sw_error_t
fal_qos_cosmap_flow_set(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint16_t flow, fal_qos_cosmap_t *cosmap);

sw_error_t
fal_qos_cosmap_flow_get(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint16_t flow, fal_qos_cosmap_t *cosmap);

sw_error_t
fal_qos_cosmap_dscp_set(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t dscp, fal_qos_cosmap_t *cosmap);

sw_error_t
fal_qos_cosmap_dscp_get(a_uint32_t dev_id, a_uint8_t group_id,
					a_uint8_t dscp, fal_qos_cosmap_t *cosmap);


sw_error_t
fal_queue_scheduler_set(a_uint32_t dev_id,
					a_uint32_t node_id, fal_queue_scheduler_level_t level,
					fal_port_t port_id,
					fal_qos_scheduler_cfg_t *scheduler_cfg);

sw_error_t
fal_queue_scheduler_get(a_uint32_t dev_id,
					a_uint32_t node_id, fal_queue_scheduler_level_t level,
					fal_port_t *port_id,
					fal_qos_scheduler_cfg_t *scheduler_cfg);

sw_error_t
fal_edma_ring_queue_map_get(a_uint32_t dev_id, 
					a_uint32_t ring_id, fal_queue_bmp_t *queue_bmp);

sw_error_t
fal_edma_ring_queue_map_set(a_uint32_t dev_id, 
					a_uint32_t ring_id, fal_queue_bmp_t *queue_bmp);
sw_error_t
fal_port_queues_get(a_uint32_t dev_id, 
				fal_port_t port_id, fal_queue_bmp_t *queue_bmp);

sw_error_t
fal_scheduler_dequeue_ctrl_set(a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t enable);

sw_error_t
fal_scheduler_dequeue_ctrl_get(a_uint32_t dev_id, a_uint32_t queue_id, a_bool_t *enable);

sw_error_t
fal_port_scheduler_cfg_reset(
		a_uint32_t dev_id,
		fal_port_t port_id);

sw_error_t
fal_port_scheduler_resource_get(
		a_uint32_t dev_id,
		fal_port_t port_id,
		fal_portscheduler_resource_t *cfg);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _PORT_QOS_H_ */
/**
 * @}
 */
