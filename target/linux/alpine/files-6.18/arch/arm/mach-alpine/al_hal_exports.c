/*
* Copyright (C) 2015 Annapurna Labs Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <linux/export.h>

#include <mach/al_hal_udma.h>
#include <mach/al_hal_udma_config.h>
#include <mach/al_hal_iofic.h>
#include <mach/al_hal_udma_iofic.h>
#include <mach/al_hal_udma_debug.h>
#include <mach/al_hal_udma_fast.h>
#include <mach/al_hal_m2m_udma.h>
#include <mach/al_hal_serdes.h>
#include <mach/al_hal_ssm.h>

EXPORT_SYMBOL(al_iofic_moder_res_config);
EXPORT_SYMBOL(al_udma_q_handle_get);
EXPORT_SYMBOL(al_udma_m2s_packet_size_cfg_set);
EXPORT_SYMBOL(al_udma_q_init);
EXPORT_SYMBOL(al_iofic_read_cause);
EXPORT_SYMBOL(al_udma_cdesc_packet_get);
EXPORT_SYMBOL(al_iofic_msix_moder_interval_config);
EXPORT_SYMBOL(al_udma_iofic_config);
EXPORT_SYMBOL(al_udma_init);
EXPORT_SYMBOL(al_iofic_config);
EXPORT_SYMBOL(al_udma_states_name);
EXPORT_SYMBOL(al_udma_state_set);
EXPORT_SYMBOL(al_udma_iofic_unmask_offset_get);
EXPORT_SYMBOL(al_iofic_mask);
EXPORT_SYMBOL(al_iofic_unmask);
EXPORT_SYMBOL(al_iofic_clear_cause);
EXPORT_SYMBOL(al_udma_state_get);
EXPORT_SYMBOL(al_udma_q_struct_print);
EXPORT_SYMBOL(al_udma_regs_print);
EXPORT_SYMBOL(al_udma_ring_print);
EXPORT_SYMBOL(al_m2m_udma_handle_get);
EXPORT_SYMBOL(al_m2m_udma_state_set);
EXPORT_SYMBOL(al_m2m_udma_q_init);
EXPORT_SYMBOL(al_m2m_udma_init);
EXPORT_SYMBOL(al_serdes_tx_deemph_inc);
EXPORT_SYMBOL(al_serdes_signal_is_detected);
EXPORT_SYMBOL(al_serdes_rx_advanced_params_set);
EXPORT_SYMBOL(al_serdes_tx_advanced_params_set);
EXPORT_SYMBOL(al_serdes_eye_measure_run);
EXPORT_SYMBOL(al_udma_m2s_max_descs_set);
EXPORT_SYMBOL(al_serdes_tx_deemph_dec);
EXPORT_SYMBOL(al_serdes_handle_init);
EXPORT_SYMBOL(al_serdes_tx_deemph_preset);
EXPORT_SYMBOL(al_serdes_pma_hard_reset_lane);
EXPORT_SYMBOL(al_ssm_dma_init);
EXPORT_SYMBOL(al_ssm_dma_q_init);
EXPORT_SYMBOL(al_ssm_dma_state_set);
EXPORT_SYMBOL(al_ssm_dma_handle_get);
EXPORT_SYMBOL(al_ssm_dma_rx_queue_handle_get);
EXPORT_SYMBOL(al_ssm_dma_tx_queue_handle_get);
EXPORT_SYMBOL(al_udma_fast_memcpy_q_prepare);
EXPORT_SYMBOL(al_udma_q_enable);
EXPORT_SYMBOL(al_serdes_reg_read);
EXPORT_SYMBOL(al_udma_s2m_compl_desc_burst_config);
EXPORT_SYMBOL(al_udma_gen_vmid_conf_set);
EXPORT_SYMBOL(al_serdes_rx_equalization);
EXPORT_SYMBOL(al_serdes_tx_advanced_params_get);
EXPORT_SYMBOL(al_serdes_rx_advanced_params_get);
EXPORT_SYMBOL(al_udma_q_reset);
EXPORT_SYMBOL(al_serdes_reg_write);
EXPORT_SYMBOL(al_udma_s2m_max_descs_set);
EXPORT_SYMBOL(al_udma_s2m_q_compl_coal_config);
