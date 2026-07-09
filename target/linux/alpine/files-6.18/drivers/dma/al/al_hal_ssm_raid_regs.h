/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in
	  the documentation and/or other materials provided with the
	  distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef __AL_HAL_RAID_ACCELERATOR_REGS_H
#define __AL_HAL_RAID_ACCELERATOR_REGS_H

#ifdef __cplusplus
extern "C" {
#endif
/*
* Unit Registers
*/

struct raid_accelerator_configuration {
	uint32_t unit_conf;             /* Unit configuration register */
	uint32_t rsrvd[3]; 
};

struct raid_accelerator_error {
	uint32_t unit_status;           /* Error status registerSee  mask_fatal_error register ... */
	uint32_t mask_fatal_error;      /* Interrupt error configuration register: A bit in th ... */
	uint32_t rsrvd[2]; 
};

struct raid_accelerator_gflog_table {
	uint32_t w0_raw;                /* GFLOG Table Word0 */
	uint32_t w1_raw;                /* GFLOG Table Word1R_n referes ro Raw n in the table */
	uint32_t w2_raw;                /* GFLOG Table Word2R_n referes ro Raw n in the table */
	uint32_t w3_raw;                /* GFLOG Table Word3R_n referes ro Raw n in the table */
};

struct raid_accelerator_log {
	uint32_t desc_word0;            /* Descriptor word 0 */
	uint32_t desc_word1;            /* Descriptor word 1 */
	uint32_t trans_info_1;          /* Transaction Information of the command that trigger ... */
	uint32_t trans_info_2;          /* Transaction Information of the command that trigger ... */
	uint32_t rsrvd[4]; 
};

struct raid_accelerator_gfilog_table {
	uint32_t w0_r;                  /* GFILOG Table Word0 */
	uint32_t w1_r;                  /* GFILOG Table Word1R_n referes ro Raw n in the table */
	uint32_t w2_r;                  /* GFILOG Table Word2R_n referes ro Raw n in the table */
	uint32_t w3_r;                  /* GFILOG Table Word3R_n referes ro Raw n in the table */
};

struct raid_accelerator_raid_status {
	uint32_t rsrvd[1];
	uint32_t status;                /* Performance counter control */
};

struct raid_accelerator_raid_perf_counter {
	uint32_t exec_cnt;              /* The execution cycle counter Measure number of cycle ... */
	uint32_t m2s_active_cnt;        /* M2S active cycles counterMeasure number of cycles M ... */
	uint32_t m2s_idle_cnt;          /* M2S idle cycles counterMeasure number of idle cycle ... */
	uint32_t m2s_backp_cnt;         /* M2S back prussure cycles counterMeasure number of   ... */
	uint32_t s2m_active_cnt;        /* S2M active cycles counterMeasure number of cycles r ... */
	uint32_t s2m_idle_cnt;          /* S2M idle cycles counterMeasure number of idle cycle ... */
	uint32_t s2m_backp_cnt;         /* S2M backpressure CounterS2M backpressure cycles cou ... */
	uint32_t cmd_dn_cnt;            /* RAID Command Done CounterTotal Number of RAID comma ... */
	uint32_t src_blocks_cnt;        /* RAID Source Blocks CounterTotal Number of Source Bl ... */
	uint32_t dst_blocks_cnt;        /* RAID Destination Blocks CounterTotal Number of Dest ... */
	uint32_t mem_cmd_dn_cnt;        /* Memory Command Done CounterTotal Number of Non-RAID ... */
	uint32_t recover_err_cnt;       /* Recoverable Errors counterTotal Number of recoverab ... */
	uint32_t src_data_beats;        /* Count the number of the data beats enter to RAID */
	uint32_t dst_data_beats;        /* Count the number of the data beats get out from RAI ... */
	uint32_t rsrvd[6]; 
};

struct raid_accelerator_perfm_cnt_cntl {
	uint32_t conf;                  /* Performance counter control */
	uint32_t rsrvd[27];

};


struct raid_accelerator_regs {
	struct raid_accelerator_configuration configuration;
	struct raid_accelerator_error error;
	struct raid_accelerator_log log;
	struct raid_accelerator_raid_perf_counter raid_perf_counter;
	struct raid_accelerator_perfm_cnt_cntl perfm_cnt_cntl;
	struct raid_accelerator_gflog_table gflog_table[16];
	struct raid_accelerator_gfilog_table gfilog_table[16];
	struct raid_accelerator_raid_status raid_status;
};


/*
* Registers Fields
*/


/**** unit_conf register ****/
/* When this bit is set to 1, the raid engine accept n ... */
#define RAID_ACCELERATOR_CONFIGURATION_UNIT_CONF_MUL_CMD_EN (1 << 0) 
/* When this bit is set to 1, when error occure the pi ... */
#define RAID_ACCELERATOR_CONFIGURATION_UNIT_CONF_HOLD_PIPE_WHEN_ERROR (1 << 1) 
/* When this bit is set to 1, Reset the ack fifo. */
#define RAID_ACCELERATOR_CONFIGURATION_UNIT_CONF_FIFO_ACK_ENABLE_MASK 0x0000007C
#define RAID_ACCELERATOR_CONFIGURATION_UNIT_CONF_FIFO_ACK_ENABLE_SHIFT 2

/**** unit_status register ****/
/* Timeout on S2M */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_S2M_TOUT (1 << 0) 
/* Timeout on M2S */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_M2S_TOUT (1 << 1) 
/* Wrong/Unknown Command */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_CMD_DECODE_ERR (1 << 2) 
/* Multiple Source-Blocks that are not equal in size */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_BLOCK_SIZE_ERR (1 << 3) 
/* Wrong and illegal software configuration of the des ... */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_ILLEGAL_CONF (1 << 4) 
/* source length is bigger from 16Kbytes for p_only or ... */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_SOURCE_ABOVE_16K (1 << 5) 
/* source length is bigger from 8Kbytes for p&q operat ... */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_SOURCE_ABOVE_8K (1 << 6) 
/* Data read frominternal memory has parity error */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_INTERNAL_PARITY_ERR (1 << 7) 
/* Error received from M2S interface */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_M2S_ERR (1 << 8) 
/* Completion acknoledge Fifo overrun */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_ACK_FIFO_OVR_MASK 0x00003E00
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_ACK_FIFO_OVR_SHIFT 9
/* Data FIFO (used in Q operation)  overrun */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_Q_FIFO_OVR (1 << 14) 
/* EOP without SOP */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_EOP_WO_SOP (1 << 15) 
/* SOP without EOP */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_SOP_WO_EOP (1 << 16) 
/* SOP and EOP in the same cycle */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_SOP_EOP_SAME_CYCLE (1 << 17) 
/* Request from strem without SOP */
#define RAID_ACCELERATOR_ERROR_UNIT_STATUS_REQ_VALID_WO_SOP (1 << 18) 

/**** mask_fatal_error register ****/
/* Timeout on S2M */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_S2M_TOUT (1 << 0) 
/* Timeout on M2S */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_M2S_TOUT (1 << 1) 
/* Wrong/Unknown Command */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_CMD_DECODE_ERR (1 << 2) 
/* Multiple Source-Blocks that are not equal in size */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_BLOCK_SIZE_ERR (1 << 3) 
/* Wrong and illegal software configuration of the des ... */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_ILLEGAL_CONF (1 << 4) 
/* source length is bigger from 16Kbytes for p_only or ... */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_SOURCE_ABOVE_16K (1 << 5) 
/* source length is bigger from 8Kbytes for p&q operat ... */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_SOURCE_ABOVE_8K (1 << 6) 
/* Data read frominternal memory has parity error */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_INTERNAL_PARITY_ERR (1 << 7) 
/* Error received from M2S interface */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_M2S_ERR (1 << 8) 
/* Completion acknoledge Fifo overrun */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_ACK_FIFO_OVR_MASK 0x00003E00
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_ACK_FIFO_OVR_SHIFT 9
/* Data FIFO (used in Q operation)  overrun */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_Q_FIFO_OVR (1 << 14) 
/* EOP without SOP */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_EOP_WO_SOP (1 << 15) 
/* SOP without EOP */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_SOP_WO_EOP (1 << 16) 
/* SOP and EOP in the same cycle */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_SOP_EOP_SAME_CYCLE (1 << 17) 
/* Request from strem without SOP */
#define RAID_ACCELERATOR_ERROR_MASK_FATAL_ERROR_REQ_VALID_WO_SOP (1 << 18) 

/**** trans_info_1 register ****/
/* Transaction length in bytes */
#define RAID_ACCELERATOR_LOG_TRANS_INFO_1_TRANS_LEN_MASK 0x000FFFFF
#define RAID_ACCELERATOR_LOG_TRANS_INFO_1_TRANS_LEN_SHIFT 0
/* Number of descriptors in the transaction */
#define RAID_ACCELERATOR_LOG_TRANS_INFO_1_NUM_OF_DESC_MASK 0x00F00000
#define RAID_ACCELERATOR_LOG_TRANS_INFO_1_NUM_OF_DESC_SHIFT 20
/* Reserved */
#define RAID_ACCELERATOR_LOG_TRANS_INFO_1_RESERVED_MASK 0xFF000000
#define RAID_ACCELERATOR_LOG_TRANS_INFO_1_RESERVED_SHIFT 24

/**** trans_info_2 register ****/
/* Queue Number of the transaction */
#define RAID_ACCELERATOR_LOG_TRANS_INFO_2_Q_NUM_MASK 0x00000FFF
#define RAID_ACCELERATOR_LOG_TRANS_INFO_2_Q_NUM_SHIFT 0
/* UDMA ID of the transaction */
#define RAID_ACCELERATOR_LOG_TRANS_INFO_2_DMA_ID_MASK 0x0000F000
#define RAID_ACCELERATOR_LOG_TRANS_INFO_2_DMA_ID_SHIFT 12
/* Internal Serial Number of the transaction */
#define RAID_ACCELERATOR_LOG_TRANS_INFO_2_SERIAL_NUM_MASK 0x03FF0000
#define RAID_ACCELERATOR_LOG_TRANS_INFO_2_SERIAL_NUM_SHIFT 16
/* Reserved */
#define RAID_ACCELERATOR_LOG_TRANS_INFO_2_RESERVED_MASK 0xFC000000
#define RAID_ACCELERATOR_LOG_TRANS_INFO_2_RESERVED_SHIFT 26

/**** conf register ****/
/* Not effect the recover_err_cnt          0: clear pe ... */
#define RAID_ACCELERATOR_PERFM_CNT_CNTL_CONF_CONT_PERFORM_MASK 0x00000003
#define RAID_ACCELERATOR_PERFM_CNT_CNTL_CONF_CONT_PERFORM_SHIFT 0

/**** status register ****/
/* indicate when RAID is empty. */
#define RAID_ACCELERATOR_RAID_STATUS_STATUS_RAID_EMPTY (1 << 0) 

#ifdef __cplusplus
}
#endif

#endif /* __AL_HAL_RAID_ACCELERATOR_REG_H */
