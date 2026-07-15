/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (c) 2025 Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided as is and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */

#ifndef __DAL_MAPPER_H__
#define __DAL_MAPPER_H__

/*
 * Include Files
 */
#include <rtk_types.h>
#include <rtk_error.h>
#include <rtk_switch.h>
#include <led.h>
#include <oam.h>
#include <cpu.h>
#include <stat.h>
#include <l2.h>
#include <interrupt.h>
#include <acl.h>
#include <mirror.h>
#include <port.h>
#include <trap.h>
#include <igmp.h>
#include <storm.h>
#include <rate.h>
#include <i2c.h>
#include <ptp.h>
#include <qos.h>
#include <vlan.h>
#include <dot1x.h>
#include <svlan.h>
#include <rldp.h>
#include <trunk.h>
#include <leaky.h>
#include <gpio.h>
#include <wol.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

typedef struct dal_mapper_s {

    /* switch */
    rtksw_api_ret_t   (*switch_init)(rtksw_uint32);
    rtksw_api_ret_t   (*switch_portMaxPktLen_set)(rtksw_uint32, rtksw_port_t, rtksw_switch_maxPktLen_linkSpeed_t, rtksw_uint32);
    rtksw_api_ret_t   (*switch_portMaxPktLen_get)(rtksw_uint32, rtksw_port_t, rtksw_switch_maxPktLen_linkSpeed_t, rtksw_uint32 *);
    rtksw_api_ret_t   (*switch_maxPktLenCfg_set)(rtksw_uint32, rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t   (*switch_maxPktLenCfg_get)(rtksw_uint32, rtksw_uint32, rtksw_uint32 *);
    rtksw_api_ret_t   (*switch_greenEthernet_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t   (*switch_greenEthernet_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t   (*switch_reset)(rtksw_uint32);

    /* eee */
    rtksw_api_ret_t   (*eee_init)(rtksw_uint32);
    rtksw_api_ret_t   (*eee_portEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t   (*eee_portEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);

    /* led */
    rtksw_api_ret_t   (*led_enable_set)(rtksw_uint32, rtksw_led_group_t, rtksw_portmask_t *);
    rtksw_api_ret_t   (*led_enable_get)(rtksw_uint32, rtksw_led_group_t, rtksw_portmask_t *);
    rtksw_api_ret_t   (*led_operation_set)(rtksw_uint32, rtksw_led_operation_t );
    rtksw_api_ret_t   (*led_operation_get)(rtksw_uint32, rtksw_led_operation_t *);
    rtksw_api_ret_t   (*led_modeForce_set)(rtksw_uint32, rtksw_port_t, rtksw_led_group_t, rtksw_led_force_mode_t);
    rtksw_api_ret_t   (*led_modeForce_get)(rtksw_uint32, rtksw_port_t, rtksw_led_group_t, rtksw_led_force_mode_t *);
    rtksw_api_ret_t   (*led_blinkRate_set)(rtksw_uint32, rtksw_led_blink_rate_t);
    rtksw_api_ret_t   (*led_blinkRate_get)(rtksw_uint32, rtksw_led_blink_rate_t *);
    rtksw_api_ret_t   (*led_groupConfig_set)(rtksw_uint32, rtksw_led_group_t, rtksw_led_config_t);
    rtksw_api_ret_t   (*led_groupConfig_get)(rtksw_uint32, rtksw_led_group_t, rtksw_led_config_t *);
    rtksw_api_ret_t   (*led_groupAbility_set)(rtksw_uint32, rtksw_led_group_t, rtksw_led_ability_t *);
    rtksw_api_ret_t   (*led_groupAbility_get)(rtksw_uint32, rtksw_led_group_t, rtksw_led_ability_t *);
    rtksw_api_ret_t   (*led_serialMode_set)(rtksw_uint32, rtksw_led_active_t);
    rtksw_api_ret_t   (*led_serialMode_get)(rtksw_uint32, rtksw_led_active_t *);
    rtksw_api_ret_t   (*led_OutputEnable_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t   (*led_OutputEnable_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t   (*led_serialModePortmask_set)(rtksw_uint32, rtksw_led_serialOutput_t, rtksw_portmask_t *);
    rtksw_api_ret_t   (*led_serialModePortmask_get)(rtksw_uint32, rtksw_led_serialOutput_t *, rtksw_portmask_t *);
    rtksw_api_ret_t   (*led_parallelLEDRemap_set)(rtksw_uint32, rtksw_uint32, rtksw_port_t, rtksw_led_group_t);
    rtksw_api_ret_t   (*led_parallelLEDRemap_get)(rtksw_uint32, rtksw_uint32, rtksw_port_t *, rtksw_led_group_t *);
    rtksw_api_ret_t   (*led_parallelLEDState_set)(rtksw_uint32, rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t   (*led_parallelLEDState_get)(rtksw_uint32, rtksw_uint32, rtksw_enable_t *);

    /* oam */
    rtksw_api_ret_t (*oam_init)(rtksw_uint32);
    rtksw_api_ret_t (*oam_state_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*oam_state_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*oam_parserAction_set)(rtksw_uint32, rtksw_port_t, rtksw_oam_parser_act_t );
    rtksw_api_ret_t (*oam_parserAction_get)(rtksw_uint32, rtksw_port_t, rtksw_oam_parser_act_t *);
    rtksw_api_ret_t (*oam_multiplexerAction_set)(rtksw_uint32, rtksw_port_t, rtksw_oam_multiplexer_act_t );
    rtksw_api_ret_t (*oam_multiplexerAction_get)(rtksw_uint32, rtksw_port_t, rtksw_oam_multiplexer_act_t *);

    /* cpu */
    rtksw_api_ret_t (*cpu_enable_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*cpu_enable_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*cpu_tagPort_set)(rtksw_uint32, rtksw_port_t, rtksw_cpu_insert_t);
    rtksw_api_ret_t (*cpu_tagPort_get)(rtksw_uint32, rtksw_port_t *, rtksw_cpu_insert_t *);
    rtksw_api_ret_t (*cpu_awarePort_set)(rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*cpu_awarePort_get)(rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*cpu_tagPosition_set)(rtksw_uint32, rtksw_cpu_position_t);
    rtksw_api_ret_t (*cpu_tagPosition_get)(rtksw_uint32, rtksw_cpu_position_t *);
    rtksw_api_ret_t (*cpu_tagLength_set)(rtksw_uint32, rtksw_cpu_tag_length_t);
    rtksw_api_ret_t (*cpu_tagLength_get)(rtksw_uint32, rtksw_cpu_tag_length_t *);
    rtksw_api_ret_t (*cpu_acceptLength_set)(rtksw_uint32, rtksw_cpu_rx_length_t);
    rtksw_api_ret_t (*cpu_acceptLength_get)(rtksw_uint32, rtksw_cpu_rx_length_t *);
    rtksw_api_ret_t (*cpu_priRemap_set)(rtksw_uint32, rtksw_pri_t, rtksw_pri_t);
    rtksw_api_ret_t (*cpu_priRemap_get)(rtksw_uint32, rtksw_pri_t, rtksw_pri_t *);

    /* stat */
    rtksw_api_ret_t (*stat_global_reset)(rtksw_uint32);
    rtksw_api_ret_t (*stat_port_reset)(rtksw_uint32, rtksw_port_t);
    rtksw_api_ret_t (*stat_queueManage_reset)(rtksw_uint32);
    rtksw_api_ret_t (*stat_global_get)(rtksw_uint32, rtksw_stat_global_type_t, rtksw_stat_counter_t *);
    rtksw_api_ret_t (*stat_global_getAll)(rtksw_uint32, rtksw_stat_global_cntr_t *);
    rtksw_api_ret_t (*stat_port_get)(rtksw_uint32, rtksw_port_t, rtksw_stat_port_type_t, rtksw_stat_counter_t *);
    rtksw_api_ret_t (*stat_port_getAll)(rtksw_uint32, rtksw_port_t, rtksw_stat_port_cntr_t *);
    rtksw_api_ret_t (*stat_logging_counterCfg_set)(rtksw_uint32, rtksw_uint32, rtksw_logging_counter_mode_t, rtksw_logging_counter_type_t);
    rtksw_api_ret_t (*stat_logging_counterCfg_get)(rtksw_uint32, rtksw_uint32, rtksw_logging_counter_mode_t *, rtksw_logging_counter_type_t *);
    rtksw_api_ret_t (*stat_logging_counter_reset)(rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*stat_logging_counter_get)(rtksw_uint32, rtksw_uint32, rtksw_uint32 *);
    rtksw_api_ret_t (*stat_lengthMode_set)(rtksw_uint32, rtksw_stat_lengthMode_t, rtksw_stat_lengthMode_t);
    rtksw_api_ret_t (*stat_lengthMode_get)(rtksw_uint32, rtksw_stat_lengthMode_t *, rtksw_stat_lengthMode_t *);

    /* l2 */
    rtksw_api_ret_t (*l2_init)(rtksw_uint32);
    rtksw_api_ret_t (*l2_addr_add)(rtksw_uint32, rtksw_mac_t *, rtksw_l2_ucastAddr_t *);
    rtksw_api_ret_t (*l2_addr_get)(rtksw_uint32, rtksw_mac_t *, rtksw_l2_ucastAddr_t *);
    rtksw_api_ret_t (*l2_addr_next_get)(rtksw_uint32, rtksw_l2_read_method_t, rtksw_port_t, rtksw_uint32 *, rtksw_l2_ucastAddr_t *);
    rtksw_api_ret_t (*l2_addr_del)(rtksw_uint32, rtksw_mac_t *, rtksw_l2_ucastAddr_t *);
    rtksw_api_ret_t (*l2_mcastAddr_add)(rtksw_uint32, rtksw_l2_mcastAddr_t *);
    rtksw_api_ret_t (*l2_mcastAddr_get)(rtksw_uint32, rtksw_l2_mcastAddr_t *);
    rtksw_api_ret_t (*l2_mcastAddr_next_get)(rtksw_uint32, rtksw_uint32 *, rtksw_l2_mcastAddr_t *);
    rtksw_api_ret_t (*l2_mcastAddr_del)(rtksw_uint32, rtksw_l2_mcastAddr_t *);
    rtksw_api_ret_t (*l2_ipMcastAddr_add)(rtksw_uint32, rtksw_l2_ipMcastAddr_t *);
    rtksw_api_ret_t (*l2_ipMcastAddr_get)(rtksw_uint32, rtksw_l2_ipMcastAddr_t *);
    rtksw_api_ret_t (*l2_ipMcastAddr_next_get)(rtksw_uint32, rtksw_uint32 *, rtksw_l2_ipMcastAddr_t *);
    rtksw_api_ret_t (*l2_ipMcastAddr_del)(rtksw_uint32, rtksw_l2_ipMcastAddr_t *);
    rtksw_api_ret_t (*l2_ipVidMcastAddr_add)(rtksw_uint32, rtksw_l2_ipVidMcastAddr_t *);
    rtksw_api_ret_t (*l2_ipVidMcastAddr_get)(rtksw_uint32, rtksw_l2_ipVidMcastAddr_t *);
    rtksw_api_ret_t (*l2_ipVidMcastAddr_next_get)(rtksw_uint32, rtksw_uint32 *, rtksw_l2_ipVidMcastAddr_t *);
    rtksw_api_ret_t (*l2_ipVidMcastAddr_del)(rtksw_uint32, rtksw_l2_ipVidMcastAddr_t *);
    rtksw_api_ret_t (*l2_ucastAddr_flush)(rtksw_uint32, rtksw_l2_flushCfg_t *);
    rtksw_api_ret_t (*l2_table_clear)(rtksw_uint32);
    rtksw_api_ret_t (*l2_table_clearStatus_get)(rtksw_uint32, rtksw_l2_clearStatus_t *);
    rtksw_api_ret_t (*l2_flushLinkDownPortAddrEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*l2_flushLinkDownPortAddrEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*l2_agingEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*l2_agingEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*l2_limitLearningCnt_set)(rtksw_uint32, rtksw_port_t, rtksw_mac_cnt_t);
    rtksw_api_ret_t (*l2_limitLearningCnt_get)(rtksw_uint32, rtksw_port_t, rtksw_mac_cnt_t *);
    rtksw_api_ret_t (*l2_limitSystemLearningCnt_set)(rtksw_uint32, rtksw_mac_cnt_t);
    rtksw_api_ret_t (*l2_limitSystemLearningCnt_get)(rtksw_uint32, rtksw_mac_cnt_t *);
    rtksw_api_ret_t (*l2_limitLearningCntAction_set)(rtksw_uint32, rtksw_port_t, rtksw_l2_limitLearnCntAction_t);
    rtksw_api_ret_t (*l2_limitLearningCntAction_get)(rtksw_uint32, rtksw_port_t, rtksw_l2_limitLearnCntAction_t *);
    rtksw_api_ret_t (*l2_limitSystemLearningCntAction_set)(rtksw_uint32, rtksw_l2_limitLearnCntAction_t);
    rtksw_api_ret_t (*l2_limitSystemLearningCntAction_get)(rtksw_uint32, rtksw_l2_limitLearnCntAction_t *);
    rtksw_api_ret_t (*l2_limitSystemLearningCntPortMask_set)(rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*l2_limitSystemLearningCntPortMask_get)(rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*l2_learningCnt_get)(rtksw_uint32, rtksw_port_t port, rtksw_mac_cnt_t *);
    rtksw_api_ret_t (*l2_floodPortMask_set)(rtksw_uint32, rtksw_l2_flood_type_t, rtksw_portmask_t *);
    rtksw_api_ret_t (*l2_floodPortMask_get)(rtksw_uint32, rtksw_l2_flood_type_t, rtksw_portmask_t *);
    rtksw_api_ret_t (*l2_localPktPermit_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*l2_localPktPermit_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*l2_aging_set)(rtksw_uint32, rtksw_l2_age_time_t);
    rtksw_api_ret_t (*l2_aging_get)(rtksw_uint32, rtksw_l2_age_time_t *);
    rtksw_api_ret_t (*l2_ipMcastAddrLookup_set)(rtksw_uint32, rtksw_l2_ipmc_lookup_type_t);
    rtksw_api_ret_t (*l2_ipMcastAddrLookup_get)(rtksw_uint32, rtksw_l2_ipmc_lookup_type_t *);
    rtksw_api_ret_t (*l2_ipMcastForwardRouterPort_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*l2_ipMcastForwardRouterPort_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*l2_ipMcastGroupEntry_add)(rtksw_uint32, ipaddr_t, rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*l2_ipMcastGroupEntry_del)(rtksw_uint32, ipaddr_t, rtksw_uint32);
    rtksw_api_ret_t (*l2_ipMcastGroupEntry_get)(rtksw_uint32, ipaddr_t, rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*l2_entry_get)(rtksw_uint32, rtksw_l2_addr_table_t *);
    rtksw_api_ret_t (*l2_lookupHitIsolationAction_set)(rtksw_uint32, rtksw_l2_lookupHitIsolationAction_t);
    rtksw_api_ret_t (*l2_lookupHitIsolationAction_get)(rtksw_uint32, rtksw_l2_lookupHitIsolationAction_t *);

    /* interrupt */
    rtksw_api_ret_t (*int_polarity_set)(rtksw_uint32, rtksw_int_polarity_t);
    rtksw_api_ret_t (*int_polarity_get)(rtksw_uint32, rtksw_int_polarity_t *);
    rtksw_api_ret_t (*int_control_set)(rtksw_uint32, rtksw_int_type_t, rtksw_enable_t);
    rtksw_api_ret_t (*int_control_get)(rtksw_uint32, rtksw_int_type_t, rtksw_enable_t *);
    rtksw_api_ret_t (*int_status_set)(rtksw_uint32, rtksw_int_status_t *);
    rtksw_api_ret_t (*int_status_get)(rtksw_uint32, rtksw_int_status_t *);
    rtksw_api_ret_t (*int_advanceInfo_get)(rtksw_uint32, rtksw_int_advType_t, rtksw_int_info_t *);

    /* acl */
    rtksw_api_ret_t (*filter_igrAcl_init)(rtksw_uint32);
    rtksw_api_ret_t (*filter_igrAcl_field_add)(rtksw_uint32, rtksw_filter_cfg_t *, rtksw_filter_field_t *);
    rtksw_api_ret_t (*filter_igrAcl_cfg_add)(rtksw_uint32, rtksw_filter_id_t, rtksw_filter_cfg_t *, rtksw_filter_action_t *, rtksw_filter_number_t *);
    rtksw_api_ret_t (*filter_igrAcl_cfg_del)(rtksw_uint32, rtksw_filter_id_t);
    rtksw_api_ret_t (*filter_igrAcl_cfg_delAll)(rtksw_uint32);
    rtksw_api_ret_t (*filter_igrAcl_cfg_get)(rtksw_uint32, rtksw_filter_id_t, rtksw_filter_cfg_raw_t *, rtksw_filter_action_t *);
    rtksw_api_ret_t (*filter_igrAcl_unmatchAction_set)(rtksw_uint32, rtksw_port_t, rtksw_filter_unmatch_action_t);
    rtksw_api_ret_t (*filter_igrAcl_unmatchAction_get)(rtksw_uint32, rtksw_port_t, rtksw_filter_unmatch_action_t *);
    rtksw_api_ret_t (*filter_igrAcl_state_set)(rtksw_uint32, rtksw_port_t, rtksw_filter_state_t);
    rtksw_api_ret_t (*filter_igrAcl_state_get)(rtksw_uint32, rtksw_port_t, rtksw_filter_state_t *);
    rtksw_api_ret_t (*filter_igrAcl_template_set)(rtksw_uint32, rtksw_filter_template_t *);
    rtksw_api_ret_t (*filter_igrAcl_template_get)(rtksw_uint32, rtksw_filter_template_t *);
    rtksw_api_ret_t (*filter_igrAcl_field_sel_set)(rtksw_uint32, rtksw_uint32, rtksw_field_sel_t, rtksw_uint32);
    rtksw_api_ret_t (*filter_igrAcl_field_sel_get)(rtksw_uint32, rtksw_uint32, rtksw_field_sel_t *, rtksw_uint32 *);
    rtksw_api_ret_t (*filter_iprange_set)(rtksw_uint32, rtksw_uint32, rtksw_filter_iprange_t, ipaddr_t, ipaddr_t);
    rtksw_api_ret_t (*filter_iprange_get)(rtksw_uint32, rtksw_uint32, rtksw_filter_iprange_t *, ipaddr_t *, ipaddr_t *);
    rtksw_api_ret_t (*filter_vidrange_set)(rtksw_uint32, rtksw_uint32, rtksw_filter_vidrange_t, rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*filter_vidrange_get)(rtksw_uint32, rtksw_uint32, rtksw_filter_vidrange_t *, rtksw_uint32 *, rtksw_uint32 *);
    rtksw_api_ret_t (*filter_portrange_set)(rtksw_uint32, rtksw_uint32, rtksw_filter_portrange_t, rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*filter_portrange_get)(rtksw_uint32, rtksw_uint32, rtksw_filter_portrange_t *, rtksw_uint32 *, rtksw_uint32 *);
    rtksw_api_ret_t (*filter_igrAclPolarity_set)(rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*filter_igrAclPolarity_get)(rtksw_uint32, rtksw_uint32 *);

    /* mirror */
    rtksw_api_ret_t (*mirror_portBased_set)(rtksw_uint32, rtksw_port_t, rtksw_portmask_t *, rtksw_portmask_t *);
    rtksw_api_ret_t (*mirror_portBased_get)(rtksw_uint32, rtksw_port_t *, rtksw_portmask_t *, rtksw_portmask_t *);
    rtksw_api_ret_t (*mirror_portIso_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*mirror_portIso_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*mirror_vlanLeaky_set)(rtksw_uint32, rtksw_enable_t , rtksw_enable_t);
    rtksw_api_ret_t (*mirror_vlanLeaky_get)(rtksw_uint32, rtksw_enable_t *, rtksw_enable_t *);
    rtksw_api_ret_t (*mirror_isolationLeaky_set)(rtksw_uint32, rtksw_enable_t, rtksw_enable_t );
    rtksw_api_ret_t (*mirror_isolationLeaky_get)(rtksw_uint32, rtksw_enable_t *, rtksw_enable_t *);
    rtksw_api_ret_t (*mirror_keep_set)(rtksw_uint32, rtksw_mirror_keep_t);
    rtksw_api_ret_t (*mirror_keep_get)(rtksw_uint32, rtksw_mirror_keep_t *);
    rtksw_api_ret_t (*mirror_override_set)(rtksw_uint32, rtksw_enable_t, rtksw_enable_t, rtksw_enable_t);
    rtksw_api_ret_t (*mirror_override_get)(rtksw_uint32, rtksw_enable_t *, rtksw_enable_t *, rtksw_enable_t *);
    rtksw_api_ret_t (*mirror_sampling_set)(rtksw_uint32, rtksw_enable_t, rtksw_uint32);
    rtksw_api_ret_t (*mirror_sampling_get)(rtksw_uint32, rtksw_enable_t *, rtksw_uint32 *);
    rtksw_api_ret_t (*mirror_samplingCounter_get)(rtksw_uint32, rtksw_uint32 *, rtksw_uint32 *);

    /* port */
    rtksw_api_ret_t (*port_phyAutoNegoAbility_set)(rtksw_uint32, rtksw_port_t, rtksw_port_phy_ability_t *);
    rtksw_api_ret_t (*port_phyAutoNegoAbility_get)(rtksw_uint32, rtksw_port_t, rtksw_port_phy_ability_t *);
    rtksw_api_ret_t (*port_phyForceModeAbility_set)(rtksw_uint32, rtksw_port_t, rtksw_port_phy_ability_t *);
    rtksw_api_ret_t (*port_phyForceModeAbility_get)(rtksw_uint32, rtksw_port_t, rtksw_port_phy_ability_t *);
    rtksw_api_ret_t (*port_phyStatus_get)(rtksw_uint32, rtksw_port_t, rtksw_port_linkStatus_t *, rtksw_port_speed_t *, rtksw_port_duplex_t *);
    rtksw_api_ret_t (*port_macForceLink_set)(rtksw_uint32, rtksw_port_t, rtksw_port_mac_ability_t *);
    rtksw_api_ret_t (*port_macForceLink_get)(rtksw_uint32, rtksw_port_t, rtksw_port_mac_ability_t *);
    rtksw_api_ret_t (*port_macForceLinkExt_set)(rtksw_uint32, rtksw_port_t, rtksw_mode_ext_t, rtksw_port_mac_ability_t *);
    rtksw_api_ret_t (*port_macForceLinkExt_get)(rtksw_uint32, rtksw_port_t, rtksw_mode_ext_t *, rtksw_port_mac_ability_t *);
    rtksw_api_ret_t (*port_macStatus_get)(rtksw_uint32, rtksw_port_t, rtksw_port_mac_ability_t *);
    rtksw_api_ret_t (*port_macLocalLoopbackEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*port_macLocalLoopbackEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*port_phyReg_set)(rtksw_uint32, rtksw_port_t, rtksw_port_phy_reg_t, rtksw_port_phy_data_t);
    rtksw_api_ret_t (*port_phyReg_get)(rtksw_uint32, rtksw_port_t, rtksw_port_phy_reg_t, rtksw_port_phy_data_t *);
    rtksw_api_ret_t (*port_phyOCPReg_set)(rtksw_uint32, rtksw_port_t, rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*port_phyOCPReg_get)(rtksw_uint32, rtksw_port_t, rtksw_uint32, rtksw_uint32 *);
    rtksw_api_ret_t (*port_backpressureEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*port_backpressureEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*port_adminEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*port_adminEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*port_isolation_set)(rtksw_uint32, rtksw_port_t, rtksw_portmask_t *);
    rtksw_api_ret_t (*port_isolation_get)(rtksw_uint32, rtksw_port_t, rtksw_portmask_t *);
    rtksw_api_ret_t (*port_rgmiiDelayExt_set)(rtksw_uint32, rtksw_port_t, rtksw_data_t, rtksw_data_t);
    rtksw_api_ret_t (*port_rgmiiDelayExt_get)(rtksw_uint32, rtksw_port_t, rtksw_data_t *, rtksw_data_t *);
    rtksw_api_ret_t (*port_phyEnableAll_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*port_phyEnableAll_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*port_efid_set)(rtksw_uint32, rtksw_port_t, rtksw_data_t);
    rtksw_api_ret_t (*port_efid_get)(rtksw_uint32, rtksw_port_t, rtksw_data_t *);
    rtksw_api_ret_t (*port_phyComboPortMedia_set)(rtksw_uint32, rtksw_port_t, rtksw_port_media_t);
    rtksw_api_ret_t (*port_phyComboPortMedia_get)(rtksw_uint32, rtksw_port_t, rtksw_port_media_t *);
    rtksw_api_ret_t (*port_rtctEnable_set)(rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*port_rtctDisable_set)(rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*port_rtctResult_get)(rtksw_uint32, rtksw_port_t, rtksw_rtctResult_t *);
    rtksw_api_ret_t (*port_sds_reset)(rtksw_uint32, rtksw_port_t);
    rtksw_api_ret_t (*port_sgmiiLinkStatus_get)(rtksw_uint32, rtksw_port_t, rtksw_data_t *, rtksw_data_t *, rtksw_port_linkStatus_t *);
    rtksw_api_ret_t (*port_sgmiiNway_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*port_sgmiiNway_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*port_fiberAbilityExt_set)(rtksw_uint32, rtksw_port_t, rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*port_fiberAbilityExt_get)(rtksw_uint32, rtksw_port_t, rtksw_uint32 *, rtksw_uint32 *);
    rtksw_api_ret_t (*port_autoDos_set)(rtksw_uint32, rtksw_port_autoDosType_t, rtksw_enable_t);
    rtksw_api_ret_t (*port_autoDos_get)(rtksw_uint32, rtksw_port_autoDosType_t, rtksw_enable_t *);
    rtksw_api_ret_t (*port_fiberAbility_set)(rtksw_uint32, rtksw_port_t, rtksw_port_fiber_ability_t *);
    rtksw_api_ret_t (*port_fiberAbility_get)(rtksw_uint32, rtksw_port_t, rtksw_port_fiber_ability_t *);
    rtksw_api_ret_t (*port_phyMdx_set)(rtksw_uint32, rtksw_port_t, rtksw_port_phy_mdix_mode_t);
    rtksw_api_ret_t (*port_phyMdx_get)(rtksw_uint32, rtksw_port_t, rtksw_port_phy_mdix_mode_t *);
    rtksw_api_ret_t (*port_phyMdxStatus_get)(rtksw_uint32, rtksw_port_t, rtksw_port_phy_mdix_status_t *);
    rtksw_api_ret_t (*port_phyTestMode_set)(rtksw_uint32, rtksw_port_t, rtksw_port_phy_test_mode_t);
    rtksw_api_ret_t (*port_phyTestMode_get)(rtksw_uint32, rtksw_port_t, rtksw_port_phy_test_mode_t *);
    rtksw_api_ret_t (*port_maxPacketLength_set)(rtksw_uint32, rtksw_port_t, rtksw_uint32);
    rtksw_api_ret_t (*port_maxPacketLength_get)(rtksw_uint32, rtksw_port_t, rtksw_uint32 *);
    rtksw_api_ret_t (*port_phyGreenEthernet_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*port_phyGreenEthernet_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*port_phyLinkDownPowerSaving_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*port_phyLinkDownPowerSaving_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*port_serdesReg_set)(rtksw_uint32, rtksw_uint32, rtksw_uint32, rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*port_serdesReg_get)(rtksw_uint32, rtksw_uint32, rtksw_uint32, rtksw_uint32, rtksw_uint32 *);
    rtksw_api_ret_t (*port_serdesPolarity_set)(rtksw_uint32, rtksw_uint32, rtksw_port_sdsPolarity_t, rtksw_port_sdsPolarity_t);
    rtksw_api_ret_t (*port_serdesPolarity_get)(rtksw_uint32, rtksw_uint32, rtksw_port_sdsPolarity_t *, rtksw_port_sdsPolarity_t *);
    rtksw_api_ret_t (*port_extPhyAutoPollingCfg_set)(rtksw_uint32, rtksw_port_t, rtksw_port_extPhyAutoPollingCfg_t *);
    rtksw_api_ret_t (*port_extPhyAutoPollingCfg_get)(rtksw_uint32, rtksw_port_t, rtksw_port_extPhyAutoPollingCfg_t *);
    rtksw_api_ret_t (*port_extPhySmiState_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*port_extPhySmiState_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*port_extPhyReg_set)(rtksw_uint32, rtksw_uint32, rtksw_port_phy_page_t, rtksw_port_phy_reg_t, rtksw_port_phy_data_t);
    rtksw_api_ret_t (*port_extPhyReg_get)(rtksw_uint32, rtksw_uint32, rtksw_port_phy_page_t, rtksw_port_phy_reg_t, rtksw_port_phy_data_t *);
    rtksw_api_ret_t (*port_extC45PhyReg_set)(rtksw_uint32, rtksw_uint32,  rtksw_uint32, rtksw_port_phy_reg_t, rtksw_port_phy_data_t);
    rtksw_api_ret_t (*port_extC45PhyReg_get)(rtksw_uint32, rtksw_uint32, rtksw_uint32, rtksw_port_phy_reg_t, rtksw_port_phy_data_t *);

    /* trap */
    rtksw_api_ret_t (*trap_unknownUnicastPktAction_set)(rtksw_uint32, rtksw_port_t, rtksw_trap_ucast_action_t);
    rtksw_api_ret_t (*trap_unknownUnicastPktAction_get)(rtksw_uint32, rtksw_port_t, rtksw_trap_ucast_action_t *);
    rtksw_api_ret_t (*trap_unknownMacPktAction_set)(rtksw_uint32, rtksw_trap_ucast_action_t);
    rtksw_api_ret_t (*trap_unknownMacPktAction_get)(rtksw_uint32, rtksw_trap_ucast_action_t *);
    rtksw_api_ret_t (*trap_unmatchMacPktAction_set)(rtksw_uint32, rtksw_trap_ucast_action_t);
    rtksw_api_ret_t (*trap_unmatchMacPktAction_get)(rtksw_uint32, rtksw_trap_ucast_action_t *);
    rtksw_api_ret_t (*trap_unmatchMacMoving_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*trap_unmatchMacMoving_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*trap_unknownMcastPktAction_set)(rtksw_uint32, rtksw_port_t, rtksw_mcast_type_t, rtksw_trap_mcast_action_t);
    rtksw_api_ret_t (*trap_unknownMcastPktAction_get)(rtksw_uint32, rtksw_port_t, rtksw_mcast_type_t, rtksw_trap_mcast_action_t *);
    rtksw_api_ret_t (*trap_lldpEnable_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*trap_lldpEnable_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*trap_reasonTrapToCpuPriority_set)(rtksw_uint32, rtksw_trap_reason_type_t, rtksw_pri_t);
    rtksw_api_ret_t (*trap_reasonTrapToCpuPriority_get)(rtksw_uint32, rtksw_trap_reason_type_t, rtksw_pri_t *);
    rtksw_api_ret_t (*trap_rmaAction_set)(rtksw_uint32, rtksw_trap_type_t, rtksw_trap_rma_action_t);
    rtksw_api_ret_t (*trap_rmaAction_get)(rtksw_uint32, rtksw_trap_type_t, rtksw_trap_rma_action_t *);
    rtksw_api_ret_t (*trap_rmaKeepFormat_set)(rtksw_uint32, rtksw_trap_type_t, rtksw_enable_t);
    rtksw_api_ret_t (*trap_rmaKeepFormat_get)(rtksw_uint32, rtksw_trap_type_t, rtksw_enable_t *);
    rtksw_api_ret_t (*trap_portUnknownMacPktAction_set)(rtksw_uint32, rtksw_port_t, rtksw_trap_ucast_action_t);
    rtksw_api_ret_t (*trap_portUnknownMacPktAction_get)(rtksw_uint32, rtksw_port_t, rtksw_trap_ucast_action_t *);
    rtksw_api_ret_t (*trap_portUnmatchMacPktAction_set)(rtksw_uint32, rtksw_port_t, rtksw_trap_ucast_action_t);
    rtksw_api_ret_t (*trap_portUnmatchMacPktAction_get)(rtksw_uint32, rtksw_port_t, rtksw_trap_ucast_action_t *);

    /* IGMP */
    rtksw_api_ret_t (*igmp_init)(rtksw_uint32);
    rtksw_api_ret_t (*igmp_state_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*igmp_state_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*igmp_static_router_port_set)(rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*igmp_static_router_port_get)(rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*igmp_protocol_set)(rtksw_uint32, rtksw_port_t, rtksw_igmp_protocol_t, rtksw_igmp_action_t);
    rtksw_api_ret_t (*igmp_protocol_get)(rtksw_uint32, rtksw_port_t, rtksw_igmp_protocol_t, rtksw_igmp_action_t *);
    rtksw_api_ret_t (*igmp_fastLeave_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*igmp_fastLeave_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*igmp_maxGroup_set)(rtksw_uint32, rtksw_port_t, rtksw_uint32);
    rtksw_api_ret_t (*igmp_maxGroup_get)(rtksw_uint32, rtksw_port_t, rtksw_uint32 *);
    rtksw_api_ret_t (*igmp_currentGroup_get)(rtksw_uint32, rtksw_port_t, rtksw_uint32 *);
    rtksw_api_ret_t (*igmp_tableFullAction_set)(rtksw_uint32, rtksw_igmp_tableFullAction_t);
    rtksw_api_ret_t (*igmp_tableFullAction_get)(rtksw_uint32, rtksw_igmp_tableFullAction_t *);
    rtksw_api_ret_t (*igmp_checksumErrorAction_set)(rtksw_uint32, rtksw_igmp_checksumErrorAction_t);
    rtksw_api_ret_t (*igmp_checksumErrorAction_get)(rtksw_uint32, rtksw_igmp_checksumErrorAction_t *);
    rtksw_api_ret_t (*igmp_leaveTimer_set)(rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*igmp_leaveTimer_get)(rtksw_uint32, rtksw_uint32 *);
    rtksw_api_ret_t (*igmp_queryInterval_set)(rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*igmp_queryInterval_get)(rtksw_uint32, rtksw_uint32 *);
    rtksw_api_ret_t (*igmp_robustness_set)(rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*igmp_robustness_get)(rtksw_uint32, rtksw_uint32 *);
    rtksw_api_ret_t (*igmp_dynamicRouterPortAllow_set)(rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*igmp_dynamicRouterPortAllow_get)(rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*igmp_dynamicRouterPort_get)(rtksw_uint32, rtksw_igmp_dynamicRouterPort_t *);
    rtksw_api_ret_t (*igmp_suppressionEnable_set)(rtksw_uint32, rtksw_enable_t, rtksw_enable_t);
    rtksw_api_ret_t (*igmp_suppressionEnable_get)(rtksw_uint32, rtksw_enable_t *, rtksw_enable_t *);
    rtksw_api_ret_t (*igmp_portRxPktEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_igmp_rxPktEnable_t *);
    rtksw_api_ret_t (*igmp_portRxPktEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_igmp_rxPktEnable_t *);
    rtksw_api_ret_t (*igmp_groupInfo_get)(rtksw_uint32, rtksw_uint32, rtksw_igmp_groupInfo_t *);
    rtksw_api_ret_t (*igmp_ReportLeaveFwdAction_set)(rtksw_uint32, rtksw_igmp_ReportLeaveFwdAct_t);
    rtksw_api_ret_t (*igmp_ReportLeaveFwdAction_get)(rtksw_uint32, rtksw_igmp_ReportLeaveFwdAct_t *);
    rtksw_api_ret_t (*igmp_dropLeaveZeroEnable_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*igmp_dropLeaveZeroEnable_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*igmp_bypassGroupRange_set)(rtksw_uint32, rtksw_igmp_bypassGroup_t, rtksw_enable_t);
    rtksw_api_ret_t (*igmp_bypassGroupRange_get)(rtksw_uint32, rtksw_igmp_bypassGroup_t, rtksw_enable_t *);

    /* Storm */
    rtksw_api_ret_t (*rate_stormControlMeterIdx_set)(rtksw_uint32, rtksw_port_t, rtksw_rate_storm_group_t, rtksw_uint32);
    rtksw_api_ret_t (*rate_stormControlMeterIdx_get)(rtksw_uint32, rtksw_port_t, rtksw_rate_storm_group_t, rtksw_uint32 *);
    rtksw_api_ret_t (*rate_stormControlPortEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_rate_storm_group_t, rtksw_enable_t);
    rtksw_api_ret_t (*rate_stormControlPortEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_rate_storm_group_t, rtksw_enable_t *);
    rtksw_api_ret_t (*storm_bypass_set)(rtksw_uint32, rtksw_storm_bypass_t, rtksw_enable_t);
    rtksw_api_ret_t (*storm_bypass_get)(rtksw_uint32, rtksw_storm_bypass_t, rtksw_enable_t *);
    rtksw_api_ret_t (*rate_stormControlExtPortmask_set)(rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*rate_stormControlExtPortmask_get)(rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*rate_stormControlExtEnable_set)(rtksw_uint32, rtksw_rate_storm_group_t, rtksw_enable_t);
    rtksw_api_ret_t (*rate_stormControlExtEnable_get)(rtksw_uint32, rtksw_rate_storm_group_t, rtksw_enable_t *);
    rtksw_api_ret_t (*rate_stormControlExtMeterIdx_set)(rtksw_uint32, rtksw_rate_storm_group_t, rtksw_uint32);
    rtksw_api_ret_t (*rate_stormControlExtMeterIdx_get)(rtksw_uint32, rtksw_rate_storm_group_t, rtksw_uint32 *);

    /* Rate */
    rtksw_api_ret_t (*rate_shareMeter_set)(rtksw_uint32, rtksw_meter_id_t, rtksw_meter_type_t, rtksw_rate_t, rtksw_enable_t);
    rtksw_api_ret_t (*rate_shareMeter_get)(rtksw_uint32, rtksw_meter_id_t, rtksw_meter_type_t *, rtksw_rate_t *, rtksw_enable_t *);
    rtksw_api_ret_t (*rate_shareMeterBucket_set)(rtksw_uint32, rtksw_meter_id_t, rtksw_uint32);
    rtksw_api_ret_t (*rate_shareMeterBucket_get)(rtksw_uint32, rtksw_meter_id_t, rtksw_uint32 *);
    rtksw_api_ret_t (*rate_igrBandwidthCtrlRate_set)(rtksw_uint32, rtksw_port_t, rtksw_rate_t,  rtksw_enable_t, rtksw_enable_t);
    rtksw_api_ret_t (*rate_igrBandwidthCtrlRate_get)(rtksw_uint32, rtksw_port_t, rtksw_rate_t *, rtksw_enable_t *, rtksw_enable_t *);
    rtksw_api_ret_t (*rate_egrBandwidthCtrlRate_set)(rtksw_uint32, rtksw_port_t, rtksw_rate_t,  rtksw_enable_t);
    rtksw_api_ret_t (*rate_egrBandwidthCtrlRate_get)(rtksw_uint32, rtksw_port_t, rtksw_rate_t *, rtksw_enable_t *);
    rtksw_api_ret_t (*rate_egrQueueBwCtrlEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_qid_t, rtksw_enable_t);
    rtksw_api_ret_t (*rate_egrQueueBwCtrlEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_qid_t, rtksw_enable_t *);
    rtksw_api_ret_t (*rate_egrQueueBwCtrlRate_set)(rtksw_uint32, rtksw_port_t, rtksw_qid_t, rtksw_meter_id_t);
    rtksw_api_ret_t (*rate_egrQueueBwCtrlRate_get)(rtksw_uint32, rtksw_port_t, rtksw_qid_t, rtksw_meter_id_t *);

    /* I2C */
    rtksw_api_ret_t (*i2c_init)(rtksw_uint32);
    rtksw_api_ret_t (*i2c_data_read)(rtksw_uint32, rtksw_uint8, rtksw_uint32, rtksw_uint32 *);
    rtksw_api_ret_t (*i2c_data_write)(rtksw_uint32, rtksw_uint8, rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*i2c_mode_set)(rtksw_uint32, rtksw_I2C_16bit_mode_t );
    rtksw_api_ret_t (*i2c_mode_get)(rtksw_uint32, rtksw_I2C_16bit_mode_t *);
    rtksw_api_ret_t (*i2c_gpioPinGroup_set)(rtksw_uint32, rtksw_I2C_gpio_pin_t);
    rtksw_api_ret_t (*i2c_gpioPinGroup_get)(rtksw_uint32, rtksw_I2C_gpio_pin_t *);

    /*PTP*/
    rtksw_api_ret_t (*ptp_init)(rtksw_uint32);
    rtksw_api_ret_t (*ptp_mac_set)(rtksw_uint32, rtksw_mac_t );
    rtksw_api_ret_t (*ptp_mac_get)(rtksw_uint32, rtksw_mac_t *);
    rtksw_api_ret_t (*ptp_tpid_set)(rtksw_uint32, rtksw_ptp_tpid_t, rtksw_ptp_tpid_t);
    rtksw_api_ret_t (*ptp_tpid_get)(rtksw_uint32, rtksw_ptp_tpid_t *, rtksw_ptp_tpid_t *);
    rtksw_api_ret_t (*ptp_refTime_set)(rtksw_uint32, rtksw_ptp_timeStamp_t );
    rtksw_api_ret_t (*ptp_refTime_get)(rtksw_uint32, rtksw_ptp_timeStamp_t *);
    rtksw_api_ret_t (*ptp_refTimeAdjust_set)(rtksw_uint32, rtksw_ptp_sys_adjust_t, rtksw_ptp_timeStamp_t);
    rtksw_api_ret_t (*ptp_refTimeEnable_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*ptp_refTimeEnable_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*ptp_portEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t );
    rtksw_api_ret_t (*ptp_portEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*ptp_portTimestamp_get)(rtksw_uint32, rtksw_port_t, rtksw_ptp_msgType_t, rtksw_ptp_info_t *);
    rtksw_api_ret_t (*ptp_intControl_set)(rtksw_uint32, rtksw_ptp_intType_t, rtksw_enable_t);
    rtksw_api_ret_t (*ptp_intControl_get)(rtksw_uint32, rtksw_ptp_intType_t, rtksw_enable_t *);
    rtksw_api_ret_t (*ptp_intStatus_get)(rtksw_uint32, rtksw_ptp_intStatus_t *);
    rtksw_api_ret_t (*ptp_portIntStatus_set)(rtksw_uint32, rtksw_port_t, rtksw_ptp_intStatus_t );
    rtksw_api_ret_t (*ptp_portIntStatus_get)(rtksw_uint32, rtksw_port_t, rtksw_ptp_intStatus_t *);
    rtksw_api_ret_t (*ptp_portTrap_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*ptp_portTrap_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*ptp_portBypassEnable_set)(rtksw_uint32, rtksw_port_t,rtksw_enable_t);    
    rtksw_api_ret_t (*ptp_portBypassEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*ptp_portTypeEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_ptp_header_t, rtksw_enable_t);    
    rtksw_api_ret_t (*ptp_portTypeEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_ptp_header_t, rtksw_enable_t *);    
    rtksw_api_ret_t (*ptp_systemRefTime_set)(rtksw_uint32, rtksw_time_timeStamp_t, rtksw_enable_t);
    rtksw_api_ret_t (*ptp_systemRefTime_get)(rtksw_uint32, rtksw_time_timeStamp_t *);
    rtksw_api_ret_t (*ptp_oper_triger)(rtksw_uint32);
    rtksw_api_ret_t (*ptp_operCfg_set)(rtksw_uint32, rtksw_time_operCfg_t *);
    rtksw_api_ret_t (*ptp_operCfg_get)(rtksw_uint32, rtksw_time_operCfg_t *);
    rtksw_api_ret_t (*ptp_systemRefTimeAdjust_set)(rtksw_uint32, rtksw_uint32, rtksw_time_timeStamp_t, rtksw_enable_t);
    rtksw_api_ret_t (*ptp_vlanTpid_set)(rtksw_uint32, rtksw_vlanType_t, rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*ptp_vlanTpid_get)(rtksw_uint32, rtksw_vlanType_t, rtksw_uint32, rtksw_uint32 *);
    rtksw_api_ret_t (*ptp_latchTime_get)(rtksw_uint32, rtksw_time_timeStamp_t *);
    rtksw_api_ret_t (*ptp_refTimeFreqCfg_set)(rtksw_uint32, rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*ptp_refTimeFreqCfg_get)(rtksw_uint32, rtksw_uint32 *, rtksw_uint32 *);
    rtksw_api_ret_t (*ptp_txTimestampFifo_get)(rtksw_uint32, rtksw_time_txTimeEntry_t *);
    rtksw_api_ret_t (*ptp_1PPSOutput_set)(rtksw_uint32, rtksw_uint32, rtksw_enable_t);    
    rtksw_api_ret_t (*ptp_1PPSOutput_get)(rtksw_uint32, rtksw_uint32 *, rtksw_enable_t *);
    rtksw_api_ret_t (*ptp_todDelay_set)(rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*ptp_todDelay_get)(rtksw_uint32, rtksw_uint32 *);
    rtksw_api_ret_t (*ptp_clockOutput_set)(rtksw_uint32, rtksw_time_clkOutput_t *);
    rtksw_api_ret_t (*ptp_clockOutput_get)(rtksw_uint32, rtksw_time_clkOutput_t *);
    rtksw_api_ret_t (*ptp_portCtrl_set)(rtksw_uint32, rtksw_port_t, rtksw_ptp_port_ctrl_t *);
    rtksw_api_ret_t (*ptp_portCtrl_get)(rtksw_uint32, rtksw_port_t, rtksw_ptp_port_ctrl_t *);
    rtksw_api_ret_t (*ptp_interruptCtrl_set)(rtksw_uint32, rtksw_ptp_intrType_t,rtksw_enable_t);
    rtksw_api_ret_t (*ptp_interruptCtrl_get)(rtksw_uint32, rtksw_ptp_intrType_t,rtksw_enable_t *);
    rtksw_api_ret_t (*ptp_InterruptStatus_set)(rtksw_uint32, rtksw_ptp_intStatus_t);
    rtksw_api_ret_t (*ptp_InterruptStatus_get)(rtksw_uint32, rtksw_ptp_intStatus_t *);    
    rtksw_api_ret_t (*ptp_portPktTrap_set)(rtksw_uint32, rtksw_port_t, rtksw_ptp_pktType_t, rtksw_enable_t);
    rtksw_api_ret_t (*ptp_portPktTrap_get)(rtksw_uint32, rtksw_port_t, rtksw_ptp_pktType_t, rtksw_enable_t *);
    rtksw_api_ret_t (*ptp_portPktAction_set)(rtksw_uint32, rtksw_port_t, rtksw_ptp_header_t, rtksw_trapTarget_t, rtksw_uint32);
    rtksw_api_ret_t (*ptp_portPktAction_get)(rtksw_uint32, rtksw_port_t, rtksw_ptp_header_t, rtksw_trapTarget_t *, rtksw_uint32 *);	
    rtksw_api_ret_t (*ptp_clkSrcCtrl_set)(rtksw_uint32, rtksw_enable_t);    
    rtksw_api_ret_t (*ptp_clkSrcCtrl_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*ptp_timerCompen_set)(rtksw_uint32, rtksw_port_t, rtksw_uint32, rtksw_uint32);    
    rtksw_api_ret_t (*ptp_timerCompen_get)(rtksw_uint32, rtksw_port_t, rtksw_uint32 *,rtksw_uint32 *);

    /*QoS*/
    rtksw_api_ret_t (*qos_init)(rtksw_uint32, rtksw_queue_num_t);
    rtksw_api_ret_t (*qos_priSel_set)(rtksw_uint32, rtksw_qos_priDecTbl_t, rtksw_priority_select_t *);
    rtksw_api_ret_t (*qos_priSel_get)(rtksw_uint32, rtksw_qos_priDecTbl_t, rtksw_priority_select_t *);
    rtksw_api_ret_t (*qos_1pPriRemap_set)(rtksw_uint32, rtksw_pri_t, rtksw_pri_t);
    rtksw_api_ret_t (*qos_1pPriRemap_get)(rtksw_uint32, rtksw_pri_t, rtksw_pri_t *);
    rtksw_api_ret_t (*qos_1pRemarkSrcSel_set)(rtksw_uint32, rtksw_qos_1pRmkSrc_t );
    rtksw_api_ret_t (*qos_1pRemarkSrcSel_get)(rtksw_uint32, rtksw_qos_1pRmkSrc_t *);
    rtksw_api_ret_t (*qos_dscpPriRemap_set)(rtksw_uint32, rtksw_dscp_t, rtksw_pri_t );
    rtksw_api_ret_t (*qos_dscpPriRemap_get)(rtksw_uint32, rtksw_dscp_t, rtksw_pri_t *);
    rtksw_api_ret_t (*qos_portPri_set)(rtksw_uint32, rtksw_port_t, rtksw_pri_t ) ;
    rtksw_api_ret_t (*qos_portPri_get)(rtksw_uint32, rtksw_port_t, rtksw_pri_t *) ;
    rtksw_api_ret_t (*qos_queueNum_set)(rtksw_uint32, rtksw_port_t, rtksw_queue_num_t);
    rtksw_api_ret_t (*qos_queueNum_get)(rtksw_uint32, rtksw_port_t, rtksw_queue_num_t *);
    rtksw_api_ret_t (*qos_priMap_set)(rtksw_uint32, rtksw_queue_num_t, rtksw_qos_pri2queue_t *);
    rtksw_api_ret_t (*qos_priMap_get)(rtksw_uint32, rtksw_queue_num_t, rtksw_qos_pri2queue_t *);
    rtksw_api_ret_t (*qos_schedulingQueue_set)(rtksw_uint32, rtksw_port_t, rtksw_qos_queue_weights_t *);
    rtksw_api_ret_t (*qos_schedulingQueue_get)(rtksw_uint32, rtksw_port_t, rtksw_qos_queue_weights_t *);
    rtksw_api_ret_t (*qos_1pRemarkEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*qos_1pRemarkEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*qos_1pRemark_set)(rtksw_uint32, rtksw_pri_t, rtksw_pri_t);
    rtksw_api_ret_t (*qos_1pRemark_get)(rtksw_uint32, rtksw_pri_t, rtksw_pri_t *);
    rtksw_api_ret_t (*qos_dscpRemarkEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*qos_dscpRemarkEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*qos_dscpRemark_set)(rtksw_uint32, rtksw_pri_t, rtksw_dscp_t);
    rtksw_api_ret_t (*qos_dscpRemark_get)(rtksw_uint32, rtksw_pri_t, rtksw_dscp_t *);
    rtksw_api_ret_t (*qos_dscpRemarkSrcSel_set)(rtksw_uint32, rtksw_qos_dscpRmkSrc_t);
    rtksw_api_ret_t (*qos_dscpRemarkSrcSel_get)(rtksw_uint32, rtksw_qos_dscpRmkSrc_t *);
    rtksw_api_ret_t (*qos_dscpRemark2Dscp_set)(rtksw_uint32, rtksw_dscp_t, rtksw_dscp_t);
    rtksw_api_ret_t (*qos_dscpRemark2Dscp_get)(rtksw_uint32, rtksw_dscp_t, rtksw_dscp_t *);
    rtksw_api_ret_t (*qos_portPriSelIndex_set)(rtksw_uint32, rtksw_port_t, rtksw_qos_priDecTbl_t);
    rtksw_api_ret_t (*qos_portPriSelIndex_get)(rtksw_uint32, rtksw_port_t, rtksw_qos_priDecTbl_t *);
    rtksw_api_ret_t (*qos_schedulingType_set)(rtksw_uint32, rtksw_qos_scheduling_type_t);
    rtksw_api_ret_t (*qos_schedulingType_get)(rtksw_uint32, rtksw_qos_scheduling_type_t *);
    rtksw_api_ret_t (*qos_schedulingQueueType_set)(rtksw_uint32, rtksw_port_t, rtksw_qos_queue_type_t *);
    rtksw_api_ret_t (*qos_schedulingQueueType_get)(rtksw_uint32, rtksw_port_t, rtksw_qos_queue_type_t *);

    /*VLAN*/
    rtksw_api_ret_t (*vlan_init)(rtksw_uint32);
    rtksw_api_ret_t (*vlan_set)(rtksw_uint32, rtksw_vlan_t, rtksw_vlan_cfg_t *);
    rtksw_api_ret_t (*vlan_get)(rtksw_uint32, rtksw_vlan_t, rtksw_vlan_cfg_t *);
    rtksw_api_ret_t (*vlan_egrFilterEnable_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*vlan_egrFilterEnable_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*vlan_mbrCfg_set)(rtksw_uint32, rtksw_uint32, rtksw_vlan_mbrcfg_t *);
    rtksw_api_ret_t (*vlan_mbrCfg_get)(rtksw_uint32, rtksw_uint32, rtksw_vlan_mbrcfg_t *);
    rtksw_api_ret_t (*vlan_portPvid_set)(rtksw_uint32, rtksw_port_t, rtksw_vlan_t, rtksw_pri_t);
    rtksw_api_ret_t (*vlan_portPvid_get)(rtksw_uint32, rtksw_port_t, rtksw_vlan_t *, rtksw_pri_t *);
    rtksw_api_ret_t (*vlan_portIgrFilterEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*vlan_portIgrFilterEnable_get)(rtksw_uint32, rtksw_port_t, rtksw_enable_t *);
    rtksw_api_ret_t (*vlan_portAcceptFrameType_set)(rtksw_uint32, rtksw_port_t, rtksw_vlan_acceptFrameType_t);
    rtksw_api_ret_t (*vlan_portAcceptFrameType_get)(rtksw_uint32, rtksw_port_t, rtksw_vlan_acceptFrameType_t *);
    rtksw_api_ret_t (*vlan_tagMode_set)(rtksw_uint32, rtksw_port_t, rtksw_vlan_tagMode_t);
    rtksw_api_ret_t (*vlan_tagMode_get)(rtksw_uint32, rtksw_port_t, rtksw_vlan_tagMode_t *);
    rtksw_api_ret_t (*vlan_transparent_set)(rtksw_uint32, rtksw_port_t, rtksw_portmask_t *);
    rtksw_api_ret_t (*vlan_transparent_get)(rtksw_uint32, rtksw_port_t , rtksw_portmask_t *);
    rtksw_api_ret_t (*vlan_keep_set)(rtksw_uint32, rtksw_port_t, rtksw_portmask_t *);
    rtksw_api_ret_t (*vlan_keep_get)(rtksw_uint32, rtksw_port_t, rtksw_portmask_t *);
    rtksw_api_ret_t (*vlan_stg_set)(rtksw_uint32, rtksw_vlan_t, rtksw_stp_msti_id_t);
    rtksw_api_ret_t (*vlan_stg_get)(rtksw_uint32, rtksw_vlan_t, rtksw_stp_msti_id_t *);
    rtksw_api_ret_t (*vlan_protoAndPortBasedVlan_add)(rtksw_uint32, rtksw_port_t, rtksw_vlan_protoAndPortInfo_t *);
    rtksw_api_ret_t (*vlan_protoAndPortBasedVlan_get)(rtksw_uint32, rtksw_port_t , rtksw_vlan_proto_type_t, rtksw_vlan_protoVlan_frameType_t, rtksw_vlan_protoAndPortInfo_t *);
    rtksw_api_ret_t (*vlan_protoAndPortBasedVlan_del)(rtksw_uint32, rtksw_port_t , rtksw_vlan_proto_type_t, rtksw_vlan_protoVlan_frameType_t);
    rtksw_api_ret_t (*vlan_protoAndPortBasedVlan_delAll)(rtksw_uint32, rtksw_port_t);
    rtksw_api_ret_t (*vlan_portFid_set)(rtksw_uint32, rtksw_port_t port, rtksw_enable_t, rtksw_fid_t);
    rtksw_api_ret_t (*vlan_portFid_get)(rtksw_uint32, rtksw_port_t port, rtksw_enable_t *, rtksw_fid_t *);
    rtksw_api_ret_t (*vlan_UntagDscpPriorityEnable_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*vlan_UntagDscpPriorityEnable_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*stp_mstpState_set)(rtksw_uint32, rtksw_stp_msti_id_t, rtksw_port_t, rtksw_stp_state_t);
    rtksw_api_ret_t (*stp_mstpState_get)(rtksw_uint32, rtksw_stp_msti_id_t, rtksw_port_t, rtksw_stp_state_t *);
    rtksw_api_ret_t (*vlan_reservedVidAction_set)(rtksw_uint32, rtksw_vlan_resVidAction_t, rtksw_vlan_resVidAction_t);
    rtksw_api_ret_t (*vlan_reservedVidAction_get)(rtksw_uint32, rtksw_vlan_resVidAction_t *, rtksw_vlan_resVidAction_t *);
    rtksw_api_ret_t (*vlan_realKeepRemarkEnable_set)(rtksw_uint32, rtksw_enable_t );
    rtksw_api_ret_t (*vlan_realKeepRemarkEnable_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*vlan_reset)(rtksw_uint32);

    /*dot1x*/
    rtksw_api_ret_t (*dot1x_unauthPacketOper_set)(rtksw_uint32, rtksw_port_t, rtksw_dot1x_unauth_action_t);
    rtksw_api_ret_t (*dot1x_unauthPacketOper_get)(rtksw_uint32, rtksw_port_t, rtksw_dot1x_unauth_action_t *);
    rtksw_api_ret_t (*dot1x_eapolFrame2CpuEnable_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*dot1x_eapolFrame2CpuEnable_get)(rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*dot1x_portBasedEnable_set)(rtksw_uint32, rtksw_port_t port, rtksw_enable_t);
    rtksw_api_ret_t (*dot1x_portBasedEnable_get)(rtksw_uint32, rtksw_port_t port, rtksw_enable_t *);
    rtksw_api_ret_t (*dot1x_portBasedAuthStatus_set)(rtksw_uint32, rtksw_port_t, rtksw_dot1x_auth_status_t);
    rtksw_api_ret_t (*dot1x_portBasedAuthStatus_get)(rtksw_uint32, rtksw_port_t, rtksw_dot1x_auth_status_t *);
    rtksw_api_ret_t (*dot1x_portBasedDirection_set)(rtksw_uint32, rtksw_port_t, rtksw_dot1x_direction_t);
    rtksw_api_ret_t (*dot1x_portBasedDirection_get)(rtksw_uint32, rtksw_port_t, rtksw_dot1x_direction_t *);
    rtksw_api_ret_t (*dot1x_macBasedEnable_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*dot1x_macBasedEnable_get)(rtksw_uint32, rtksw_port_t , rtksw_enable_t *);
    rtksw_api_ret_t (*dot1x_macBasedAuthMac_add)(rtksw_uint32, rtksw_port_t, rtksw_mac_t *, rtksw_fid_t);
    rtksw_api_ret_t (*dot1x_macBasedAuthMac_del)(rtksw_uint32, rtksw_port_t, rtksw_mac_t *, rtksw_fid_t);
    rtksw_api_ret_t (*dot1x_macBasedDirection_set)(rtksw_uint32, rtksw_dot1x_direction_t);
    rtksw_api_ret_t (*dot1x_macBasedDirection_get)(rtksw_uint32, rtksw_dot1x_direction_t *);
    rtksw_api_ret_t (*dot1x_guestVlan_set)(rtksw_uint32, rtksw_vlan_t );
    rtksw_api_ret_t (*dot1x_guestVlan_get)(rtksw_uint32, rtksw_vlan_t *);
    rtksw_api_ret_t (*dot1x_guestVlan2Auth_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*dot1x_guestVlan2Auth_get)(rtksw_uint32, rtksw_enable_t *);

    /*SVLAN*/
    rtksw_api_ret_t (*svlan_init)(rtksw_uint32);
    rtksw_api_ret_t (*svlan_servicePort_add)(rtksw_uint32, rtksw_port_t port);
    rtksw_api_ret_t (*svlan_servicePort_get)(rtksw_uint32, rtksw_portmask_t *);
    rtksw_api_ret_t (*svlan_servicePort_del)(rtksw_uint32, rtksw_port_t);
    rtksw_api_ret_t (*svlan_tpidEntry_set)(rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*svlan_tpidEntry_get)(rtksw_uint32, rtksw_uint32 *);
    rtksw_api_ret_t (*svlan_priorityRef_set)(rtksw_uint32, rtksw_svlan_pri_ref_t);
    rtksw_api_ret_t (*svlan_priorityRef_get)(rtksw_uint32, rtksw_svlan_pri_ref_t *);
    rtksw_api_ret_t (*svlan_memberPortEntry_set)(rtksw_uint32, rtksw_uint32, rtksw_svlan_memberCfg_t *);
    rtksw_api_ret_t (*svlan_memberPortEntry_get)(rtksw_uint32, rtksw_uint32, rtksw_svlan_memberCfg_t *);
    rtksw_api_ret_t (*svlan_memberPortEntry_adv_set)(rtksw_uint32, rtksw_uint32, rtksw_svlan_memberCfg_t *);
    rtksw_api_ret_t (*svlan_memberPortEntry_adv_get)(rtksw_uint32, rtksw_uint32, rtksw_svlan_memberCfg_t *);
    rtksw_api_ret_t (*svlan_defaultSvlan_set)(rtksw_uint32, rtksw_port_t, rtksw_vlan_t);
    rtksw_api_ret_t (*svlan_defaultSvlan_get)(rtksw_uint32, rtksw_port_t, rtksw_vlan_t *);
    rtksw_api_ret_t (*svlan_c2s_add)(rtksw_uint32, rtksw_vlan_t, rtksw_port_t, rtksw_vlan_t);
    rtksw_api_ret_t (*svlan_c2s_del)(rtksw_uint32, rtksw_vlan_t, rtksw_port_t);
    rtksw_api_ret_t (*svlan_c2s_get)(rtksw_uint32, rtksw_vlan_t, rtksw_port_t, rtksw_vlan_t *);
    rtksw_api_ret_t (*svlan_untag_action_set)(rtksw_uint32, rtksw_svlan_untag_action_t, rtksw_vlan_t);
    rtksw_api_ret_t (*svlan_untag_action_get)(rtksw_uint32, rtksw_svlan_untag_action_t *, rtksw_vlan_t *);
    rtksw_api_ret_t (*svlan_unmatch_action_set)(rtksw_uint32, rtksw_svlan_unmatch_action_t, rtksw_vlan_t);
    rtksw_api_ret_t (*svlan_unmatch_action_get)(rtksw_uint32, rtksw_svlan_unmatch_action_t *, rtksw_vlan_t *);
    rtksw_api_ret_t (*svlan_dmac_vidsel_set)(rtksw_uint32, rtksw_port_t, rtksw_enable_t);
    rtksw_api_ret_t (*svlan_dmac_vidsel_get)(rtksw_uint32, rtksw_port_t , rtksw_enable_t *);
    rtksw_api_ret_t (*svlan_ipmc2s_add)(rtksw_uint32, ipaddr_t, ipaddr_t, rtksw_vlan_t);
    rtksw_api_ret_t (*svlan_ipmc2s_del)(rtksw_uint32, ipaddr_t, ipaddr_t);
    rtksw_api_ret_t (*svlan_ipmc2s_get)(rtksw_uint32, ipaddr_t, ipaddr_t, rtksw_vlan_t *);
    rtksw_api_ret_t (*svlan_l2mc2s_add)(rtksw_uint32, rtksw_mac_t, rtksw_mac_t, rtksw_vlan_t);
    rtksw_api_ret_t (*svlan_l2mc2s_del)(rtksw_uint32, rtksw_mac_t, rtksw_mac_t);
    rtksw_api_ret_t (*svlan_l2mc2s_get)(rtksw_uint32, rtksw_mac_t, rtksw_mac_t, rtksw_vlan_t *);
    rtksw_api_ret_t (*svlan_sp2c_add)(rtksw_uint32, rtksw_vlan_t, rtksw_port_t, rtksw_vlan_t);
    rtksw_api_ret_t (*svlan_sp2c_get)(rtksw_uint32, rtksw_vlan_t, rtksw_port_t, rtksw_vlan_t *);
    rtksw_api_ret_t (*svlan_sp2c_del)(rtksw_uint32, rtksw_vlan_t, rtksw_port_t);
    rtksw_api_ret_t (*svlan_lookupType_set)(rtksw_uint32, rtksw_svlan_lookupType_t);
    rtksw_api_ret_t (*svlan_lookupType_get)(rtksw_uint32, rtksw_svlan_lookupType_t *);
    rtksw_api_ret_t (*svlan_trapPri_set)(rtksw_uint32, rtksw_pri_t);
    rtksw_api_ret_t (*svlan_trapPri_get)(rtksw_uint32, rtksw_pri_t *);
    rtksw_api_ret_t (*svlan_unassign_action_set)(rtksw_uint32, rtksw_svlan_unassign_action_t);
    rtksw_api_ret_t (*svlan_unassign_action_get)(rtksw_uint32, rtksw_svlan_unassign_action_t *);

    /*RLDP*/
    rtksw_api_ret_t (*rldp_config_set)(rtksw_uint32, rtksw_rldp_config_t *);
    rtksw_api_ret_t (*rldp_config_get)(rtksw_uint32, rtksw_rldp_config_t *);
    rtksw_api_ret_t (*rldp_portConfig_set)(rtksw_uint32, rtksw_port_t, rtksw_rldp_portConfig_t *);
    rtksw_api_ret_t (*rldp_portConfig_get)(rtksw_uint32, rtksw_port_t, rtksw_rldp_portConfig_t *);
    rtksw_api_ret_t (*rldp_status_get)(rtksw_uint32, rtksw_rldp_status_t *);
    rtksw_api_ret_t (*rldp_portStatus_get)(rtksw_uint32, rtksw_port_t, rtksw_rldp_portStatus_t *);
    rtksw_api_ret_t (*rldp_portStatus_set)(rtksw_uint32, rtksw_port_t, rtksw_rldp_portStatus_t *);
    rtksw_api_ret_t (*rldp_portLoopPair_get)(rtksw_uint32, rtksw_port_t, rtksw_portmask_t *);

    /*trunk*/
    rtksw_api_ret_t (*trunk_port_set)(rtksw_uint32, rtksw_trunk_group_t, rtksw_portmask_t *);
    rtksw_api_ret_t (*trunk_port_get)(rtksw_uint32, rtksw_trunk_group_t, rtksw_portmask_t *);
    rtksw_api_ret_t (*trunk_distributionAlgorithm_set)(rtksw_uint32, rtksw_trunk_group_t, rtksw_uint32);
    rtksw_api_ret_t (*trunk_distributionAlgorithm_get)(rtksw_uint32, rtksw_trunk_group_t, rtksw_uint32 *);
    rtksw_api_ret_t (*trunk_trafficSeparate_set)(rtksw_uint32, rtksw_trunk_group_t, rtksw_trunk_separateType_t);
    rtksw_api_ret_t (*trunk_trafficSeparate_get)(rtksw_uint32, rtksw_trunk_group_t, rtksw_trunk_separateType_t *);
    rtksw_api_ret_t (*trunk_mode_set)(rtksw_uint32, rtksw_trunk_mode_t);
    rtksw_api_ret_t (*trunk_mode_get)(rtksw_uint32, rtksw_trunk_mode_t *);
    rtksw_api_ret_t (*trunk_distributionMode_set)(rtksw_uint32, rtksw_trunk_group_t, rtksw_trunk_mode_t);
    rtksw_api_ret_t (*trunk_distributionMode_get)(rtksw_uint32, rtksw_trunk_group_t, rtksw_trunk_mode_t *);
    rtksw_api_ret_t (*trunk_trafficPause_set)(rtksw_uint32, rtksw_trunk_group_t, rtksw_enable_t);
    rtksw_api_ret_t (*trunk_trafficPause_get)(rtksw_uint32, rtksw_trunk_group_t, rtksw_enable_t *);
    rtksw_api_ret_t (*trunk_hashMappingTable_set)(rtksw_uint32, rtksw_trunk_group_t, rtksw_trunk_hashVal2Port_t *);
    rtksw_api_ret_t (*trunk_hashMappingTable_get)(rtksw_uint32, rtksw_trunk_group_t, rtksw_trunk_hashVal2Port_t *);
    rtksw_api_ret_t (*trunk_portQueueEmpty_get)(rtksw_uint32, rtksw_portmask_t *);

    /*leaky*/
    rtksw_api_ret_t (*leaky_vlan_set)(rtksw_uint32, rtksw_leaky_type_t, rtksw_enable_t);
    rtksw_api_ret_t (*leaky_vlan_get)(rtksw_uint32, rtksw_leaky_type_t, rtksw_enable_t *);
    rtksw_api_ret_t (*leaky_portIsolation_set)(rtksw_uint32, rtksw_leaky_type_t, rtksw_enable_t);
    rtksw_api_ret_t (*leaky_portIsolation_get)(rtksw_uint32, rtksw_leaky_type_t, rtksw_enable_t *);

    /*GPIO*/
    rtksw_api_ret_t (*gpio_input_get)(rtksw_uint32, rtksw_uint32, rtksw_uint32 *);
    rtksw_api_ret_t (*gpio_output_set)(rtksw_uint32, rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*gpio_output_get)(rtksw_uint32, rtksw_uint32, rtksw_uint32 *);
    rtksw_api_ret_t (*gpio_state_set)(rtksw_uint32, rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*gpio_state_get)(rtksw_uint32, rtksw_uint32, rtksw_enable_t *);
    rtksw_api_ret_t (*gpio_mode_set)(rtksw_uint32, rtksw_uint32, rtksw_gpio_mode_t);
    rtksw_api_ret_t (*gpio_mode_get)(rtksw_uint32, rtksw_uint32, rtksw_gpio_mode_t *);
    rtksw_api_ret_t (*gpio_source_set)(rtksw_uint32, rtksw_uint32, rtksw_gpio_source_t);
    rtksw_api_ret_t (*gpio_source_get)(rtksw_uint32, rtksw_uint32, rtksw_gpio_source_t *);
    rtksw_api_ret_t (*gpio_aclEnClear_set)(rtksw_uint32, rtksw_uint32);
    rtksw_api_ret_t (*gpio_aclEnClear_get)(rtksw_uint32, rtksw_uint32, rtksw_enable_t *);

    /*WoL*/
    rtksw_api_ret_t (*wol_init)(rtksw_uint32);
    rtksw_api_ret_t (*wol_state_set)(rtksw_uint32, rtksw_enable_t);
    rtksw_api_ret_t (*wol_systemMac_set)(rtksw_uint32, rtksw_mac_t *);

} dal_mapper_t;


#endif /* __DAL_MAPPER_H __ */
