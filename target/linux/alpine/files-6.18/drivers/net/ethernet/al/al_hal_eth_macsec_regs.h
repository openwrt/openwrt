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

/**
 *  @{
 * @file   al_hal_eth_macsec_regs.h
 *
 * @brief ... registers
 *
 */

#ifndef __AL_HAL_ETH_MACSEC_REGS_H
#define __AL_HAL_ETH_MACSEC_REGS_H

#ifdef __cplusplus
extern "C" {
#endif
/*
* Unit Registers
*/



struct al_macsec_configuration {
	/* [0x0] TX MACSEC configuration */
	uint32_t conf_macsec_tx;
	/* [0x4] TX MACSEC PN thrshold */
	uint32_t conf_macsec_tx_pn_thr;
	/* [0x8] RX MACsec configuration */
	uint32_t conf_macsec_rx;
	/* [0xc] RX MACsec PN threshold */
	uint32_t conf_macsec_rx_pn_thr;
	/* [0x10] Ethertype field */
	uint32_t conf_macsec_ethertype;
	/* [0x14] sectag header location in parser bus */
	uint32_t conf_macsec_rx_parse_info_sectag_header;
	/* [0x18] source mac location in parser bus */
	uint32_t conf_macsec_rx_parse_info_source_mac;
	/* [0x1c] dest mac location in parser bus */
	uint32_t conf_macsec_rx_parse_info_dest_mac;
	/* [0x20] sectag offset location in parser bus */
	uint32_t conf_macsec_rx_parse_info_sectag_offset;
	/* [0x24] auth start location */
	uint32_t conf_macsec_auth_start;
	uint32_t rsrvd[10];
};
struct al_macsec_sectag_encoding {
	/* [0x0] Table's address3-bit address is encoded as {ICV_siz ... */
	uint32_t sectag_encoding_control;
	/* [0x4] Table's data:
{TCI_E, TCI_C} */
	uint32_t sectag_encoding_line;
};
struct al_macsec_sectag_decoding {
	/* [0x0] Table's address6-bit address is decoded as SECTAG_T ... */
	uint32_t sectag_decoding_control;
	/* [0x4] Table's data:{sci_sel[1:0], bypass, drop, icv_size[ ... */
	uint32_t sectag_decoding_line;
};
struct al_macsec_cache_access_tx_sad_control {
	/* [0x0] TX_SAD's address & control bits */
	uint32_t tx_sad_control;
};
struct al_macsec_cache_access_tx_sad_data {
	/* [0x0] Tx SAD's data */
	uint32_t tx_sad_data;
};
struct al_macsec_cache_access_rx_sad_control {
	/* [0x0] RX_SAD's address & control bits */
	uint32_t rx_sad_control;
};
struct al_macsec_cache_access_rx_sad_data {
	/* [0x0] Rx SAD's data */
	uint32_t rx_sad_data;
};
struct al_macsec_cam_access_rx_sci_build_control {
	/* [0x0] CAM's address */
	uint32_t sci_build_control;
};
struct al_macsec_cam_access_rx_sci_build_data {
	/* [0x0]  */
	uint32_t sci_build_data;
};
struct al_macsec_cam_access_rx_sa_index_build_control {
	/* [0x0] CAM's address */
	uint32_t sa_index_build_control;
};
struct al_macsec_cam_access_rx_sa_index_build_data {
	/* [0x0] CAM's data CAM line is written only when perfrom_co ... */
	uint32_t sa_index_build_data;
};
struct al_macsec_status {
	/* [0x0] TxMACsec configuration */
	uint32_t status_macsec_tx;
	/* [0x4] Rx MACsec configuration */
	uint32_t status_macsec_rx;
	uint32_t rsrvd[0];
};
struct al_macsec_statistics {
	/* [0x0] Number of Tx packets */
	uint32_t macsec_tx_pkts;
	/* [0x4] Number of controlled (secured) Tx packets */
	uint32_t macsec_tx_controlled_pkts;
	/* [0x8] Number of uncontrolled (unsecured) Tx packets */
	uint32_t macsec_tx_uncontrolled_pkts;
	/* [0xc] Number of dropped Tx packets */
	uint32_t macsec_tx_dropped_pkts;
	/* [0x10] Number of Rx packets */
	uint32_t macsec_rx_pkts;
	/* [0x14] Number of controlled (secured) Rx packets */
	uint32_t macsec_rx_controlled_pkts;
	/* [0x18] Number of uncontrolled (unsecured) Rx packets */
	uint32_t macsec_rx_uncontrolled_pkts;
	/* [0x1c] Number of dropped Rx packets */
	uint32_t macsec_rx_dropped_pkts;
	/* [0x20] Number of detected Rx replay packets */
	uint32_t macsec_rx_replay_pkts;
	/* [0x24] Number of Rx packets that failed authentication */
	uint32_t macsec_rx_auth_fail_pkts;
};

struct al_macsec_regs {
	struct al_macsec_configuration configuration; /* [0x0] */
	struct al_macsec_sectag_encoding sectag_encoding; /* [0x50] */
	uint32_t rsrvd_0[2];
	struct al_macsec_sectag_decoding sectag_decoding; /* [0x60] */
	struct al_macsec_cache_access_tx_sad_control cache_access_tx_sad_control; /* [0x68] */
	struct al_macsec_cache_access_tx_sad_data cache_access_tx_sad_data[16]; /* [0x6c] */
	uint32_t rsrvd_1;
	struct al_macsec_cache_access_rx_sad_control cache_access_rx_sad_control; /* [0xb0] */
	struct al_macsec_cache_access_rx_sad_data cache_access_rx_sad_data[16]; /* [0xb4] */
	struct al_macsec_cam_access_rx_sci_build_control cam_access_rx_sci_build_control; /* [0xf4] */
	struct al_macsec_cam_access_rx_sci_build_data cam_access_rx_sci_build_data[8]; /* [0xf8] */
	struct al_macsec_cam_access_rx_sa_index_build_control cam_access_rx_sa_index_build_control; /* [0x118] */
	struct al_macsec_cam_access_rx_sa_index_build_data cam_access_rx_sa_index_build_data[5]; /* [0x11c] */
	struct al_macsec_status status;             /* [0x130] */
	uint32_t rsrvd_2[2];
	struct al_macsec_statistics statistics;     /* [0x140] */
};


/*
* Registers Fields
*/


/**** conf_macsec_tx register ****/
/* 1 - MACsec mechanism enabled for TX packets0 - Bypass Tx MACs ... */
#define MACSEC_CONFIGURATION_CONF_MACSEC_TX_ENABLE_ENGINE (1 << 0)
/* Number of Security Associations per Secure Channel1'b0 - 2 SA ... */
#define MACSEC_CONFIGURATION_CONF_MACSEC_TX_NUM_SA_PER_SC (1 << 1)
/* If set, VF_ind will not be used in addressing the TX_SAD. */
#define MACSEC_CONFIGURATION_CONF_MACSEC_TX_DIRECT_ACCESS (1 << 2)
/* Enable dropping of Tx packets */
#define MACSEC_CONFIGURATION_CONF_MACSEC_TX_DROP_EN (1 << 3)
/* Drop all Tx packets arriving at the MACsec accelerator */
#define MACSEC_CONFIGURATION_CONF_MACSEC_TX_DROP_ALL (1 << 4)
/* Enable "smart" clock gating in AES */
#define MACSEC_CONFIGURATION_CONF_MACSEC_TX_CLK_GATE_EN (1 << 5)
/* 4 - AES_MODE_ECB5 - AES_MODE_CBC6 - AES_MODE_CTR;7 - AES_MODE ... */
#define MACSEC_CONFIGURATION_CONF_MACSEC_TX_CRYPTO_ENGINE_AES_MODE_MASK 0x000003C0
#define MACSEC_CONFIGURATION_CONF_MACSEC_TX_CRYPTO_ENGINE_AES_MODE_SHIFT 6
/* 0 - 16 bit
1 - 32 bit
2 - 64 bit
3 - 128 bit */
#define MACSEC_CONFIGURATION_CONF_MACSEC_TX_CRYPTO_ENGINE_AES_CNTR_STEP_MASK 0x00000C00
#define MACSEC_CONFIGURATION_CONF_MACSEC_TX_CRYPTO_ENGINE_AES_CNTR_STEP_SHIFT 10
/* Clear AP_ALIGNER */
#define MACSEC_CONFIGURATION_CONF_MACSEC_TX_DEBUG_CLR_ASSEMBLER (1 << 30)
/* Clear AP_SPLITTER */
#define MACSEC_CONFIGURATION_CONF_MACSEC_TX_DEBUG_CLR_SPLITTER (1 << 31)

/**** conf_macsec_rx register ****/
/* 1 - MACsec mechanism enabled for Rx packets0 - Bypass Rx MACs ... */
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_ENABLE_ENGINE (1 << 0)
/* Number of Security Associations per Secure Channel1'b0 - 2 Sa ... */
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_NUM_SA_PER_SC (1 << 1)
/* If set, VF_ind will not be used in addressing the RX_SAD. */
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_DIRECT_ACCESS (1 << 2)
/* Enable dropping of Rx packets. */
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_DROP_EN (1 << 3)
/* Drop all Rx packets arriving at the MACsec accelerator. */
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_DROP_ALL (1 << 4)
/* Enable "smart" clock gating in AES */
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_CLK_GATE_EN (1 << 5)
/* 4 - AES_MODE_ECB5 - AES_MODE_CBC6 - AES_MODE_CTR;7 - AES_MODE ... */
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_CRYPTO_ENGINE_AES_MODE_MASK 0x000003C0
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_CRYPTO_ENGINE_AES_MODE_SHIFT 6
/* 0 - 16 bit
1 - 32 bit
2 - 64 bit
3 - 128 bit */
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_CRYPTO_ENGINE_AES_CNTR_STEP_MASK 0x00000C00
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_CRYPTO_ENGINE_AES_CNTR_STEP_SHIFT 10
/* Method of SCI creation:0 - according to Sectag Parsing Table  ... */
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_SCI_SEL_MASK 0x00003000
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_SCI_SEL_SHIFT 12
/* if set, take sectag start offset from parser results, otherwi ... */
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_TUNNEL_ENABLE (1 << 14)
/* Clear AP_ALIGNER */
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_DEBUG_CLR_ASSEMBLER (1 << 30)
/* Clear AP_SPLITTER */
#define MACSEC_CONFIGURATION_CONF_MACSEC_RX_DEBUG_CLR_SPLITTER (1 << 31)

/**** conf_macsec_ethertype register ****/
/* MACsec Ethertype constant */
#define MACSEC_CONFIGURATION_CONF_MACSEC_ETHERTYPE_DATA_MASK 0x0000FFFF
#define MACSEC_CONFIGURATION_CONF_MACSEC_ETHERTYPE_DATA_SHIFT 0

/**** conf_macsec_auth_start register ****/
/* 2'b00 - auth starts at sop2'b01 - auth starts according to au ... */
#define MACSEC_CONFIGURATION_CONF_MACSEC_AUTH_START_CTRL_MASK 0x00000003
#define MACSEC_CONFIGURATION_CONF_MACSEC_AUTH_START_CTRL_SHIFT 0
/* auth start offset (relevant if ctrl == 2'b01) */
#define MACSEC_CONFIGURATION_CONF_MACSEC_AUTH_START_FROM_START_MASK 0x0003FFFC
#define MACSEC_CONFIGURATION_CONF_MACSEC_AUTH_START_FROM_START_SHIFT 2
/* auth start location relative to sectag header location (relev ... */
#define MACSEC_CONFIGURATION_CONF_MACSEC_AUTH_START_FROM_SECTAG_MASK 0xFFFC0000
#define MACSEC_CONFIGURATION_CONF_MACSEC_AUTH_START_FROM_SECTAG_SHIFT 18

/**** sectag_encoding_control register ****/
/* Address */
#define MACSEC_SECTAG_ENCODING_SECTAG_ENCODING_CONTROL_ADDR_MASK 0x00000007
#define MACSEC_SECTAG_ENCODING_SECTAG_ENCODING_CONTROL_ADDR_SHIFT 0
/* copy data registers content to relevant line in table */
#define MACSEC_SECTAG_ENCODING_SECTAG_ENCODING_CONTROL_PERFORM_WRITE (1 << 8)
/* copy relevant line in table to data registers */
#define MACSEC_SECTAG_ENCODING_SECTAG_ENCODING_CONTROL_PERFORM_READ (1 << 9)

/**** sectag_encoding_line register ****/
/* Data */
#define MACSEC_SECTAG_ENCODING_SECTAG_ENCODING_LINE_DATA_MASK 0x00000003
#define MACSEC_SECTAG_ENCODING_SECTAG_ENCODING_LINE_DATA_SHIFT 0

/**** sectag_decoding_control register ****/
/* Address */
#define MACSEC_SECTAG_DECODING_SECTAG_DECODING_CONTROL_ADDR_MASK 0x0000003F
#define MACSEC_SECTAG_DECODING_SECTAG_DECODING_CONTROL_ADDR_SHIFT 0
/* copy data registers content to relevant line in table */
#define MACSEC_SECTAG_DECODING_SECTAG_DECODING_CONTROL_PERFORM_WRITE (1 << 8)
/* copy relevant line in table to data registers */
#define MACSEC_SECTAG_DECODING_SECTAG_DECODING_CONTROL_PERFORM_READ (1 << 9)

/**** sectag_decoding_line register ****/
/* Data */
#define MACSEC_SECTAG_DECODING_SECTAG_DECODING_LINE_DATA_MASK 0x000007FF
#define MACSEC_SECTAG_DECODING_SECTAG_DECODING_LINE_DATA_SHIFT 0

/**** tx_sad_control register ****/
/* Address */
#define MACSEC_CACHE_ACCESS_TX_SAD_CONTROL_TX_SAD_CONTROL_ADDR_MASK 0x0000007F
#define MACSEC_CACHE_ACCESS_TX_SAD_CONTROL_TX_SAD_CONTROL_ADDR_SHIFT 0
/* copy data registers content to relevant line in table */
#define MACSEC_CACHE_ACCESS_TX_SAD_CONTROL_TX_SAD_CONTROL_PERFORM_WRITE (1 << 8)
/* copy relevant line in table to data registers */
#define MACSEC_CACHE_ACCESS_TX_SAD_CONTROL_TX_SAD_CONTROL_PERFORM_READ (1 << 9)

/**** rx_sad_control register ****/
/* Address */
#define MACSEC_CACHE_ACCESS_RX_SAD_CONTROL_RX_SAD_CONTROL_ADDR_MASK 0x0000007F
#define MACSEC_CACHE_ACCESS_RX_SAD_CONTROL_RX_SAD_CONTROL_ADDR_SHIFT 0
/* copy data registers content to relevant line in table */
#define MACSEC_CACHE_ACCESS_RX_SAD_CONTROL_RX_SAD_CONTROL_PERFORM_WRITE (1 << 8)
/* copy relevant line in table to data registers */
#define MACSEC_CACHE_ACCESS_RX_SAD_CONTROL_RX_SAD_CONTROL_PERFORM_READ (1 << 9)

/**** sci_build_control register ****/
/* Address */
#define MACSEC_CAM_ACCESS_RX_SCI_BUILD_CONTROL_SCI_BUILD_CONTROL_ADDR_MASK 0x0000000F
#define MACSEC_CAM_ACCESS_RX_SCI_BUILD_CONTROL_SCI_BUILD_CONTROL_ADDR_SHIFT 0
/* copy data registers content to relevant line in table */
#define MACSEC_CAM_ACCESS_RX_SCI_BUILD_CONTROL_SCI_BUILD_CONTROL_PERFORM_WRITE (1 << 8)
/* copy relevant line in table to data registers */
#define MACSEC_CAM_ACCESS_RX_SCI_BUILD_CONTROL_SCI_BUILD_CONTROL_PERFORM_READ (1 << 9)

/**** sa_index_build_control register ****/
/* Address */
#define MACSEC_CAM_ACCESS_RX_SA_INDEX_BUILD_CONTROL_SA_INDEX_BUILD_CONTROL_ADDR_MASK 0x0000003F
#define MACSEC_CAM_ACCESS_RX_SA_INDEX_BUILD_CONTROL_SA_INDEX_BUILD_CONTROL_ADDR_SHIFT 0
/* copy data registers to relevant line in table */
#define MACSEC_CAM_ACCESS_RX_SA_INDEX_BUILD_CONTROL_SA_INDEX_BUILD_CONTROL_PERFORM_WRITE (1 << 8)
/* copy relevant line in table to data registers */
#define MACSEC_CAM_ACCESS_RX_SA_INDEX_BUILD_CONTROL_SA_INDEX_BUILD_CONTROL_PERFORM_READ (1 << 9)

/**** status_macsec_tx register ****/
/* SA index of Tx packet that reached PN threshold */
#define MACSEC_STATUS_STATUS_MACSEC_TX_PN_THR_SA_INDEX_MASK 0x000000FF
#define MACSEC_STATUS_STATUS_MACSEC_TX_PN_THR_SA_INDEX_SHIFT 0
/* macsec_tx_tmo FSM state */
#define MACSEC_STATUS_STATUS_MACSEC_TX_TMO_STATE_MASK 0x00000F00
#define MACSEC_STATUS_STATUS_MACSEC_TX_TMO_STATE_SHIFT 8
/* macsec_tx_tmi FSM state */
#define MACSEC_STATUS_STATUS_MACSEC_TX_TMI_STATE_MASK 0x0000F000
#define MACSEC_STATUS_STATUS_MACSEC_TX_TMI_STATE_SHIFT 12

/**** status_macsec_rx register ****/
/* SA index of Rx packet that reached PN threshold */
#define MACSEC_STATUS_STATUS_MACSEC_RX_PN_THR_SA_INDEX_MASK 0x000000FF
#define MACSEC_STATUS_STATUS_MACSEC_RX_PN_THR_SA_INDEX_SHIFT 0
/* macsec_rx_rmo FSM state */
#define MACSEC_STATUS_STATUS_MACSEC_RX_RMO_STATE_MASK 0x00000F00
#define MACSEC_STATUS_STATUS_MACSEC_RX_RMO_STATE_SHIFT 8
/* macsec_rx_rmi FSM state */
#define MACSEC_STATUS_STATUS_MACSEC_RX_RMI_STATE_MASK 0x0000F000
#define MACSEC_STATUS_STATUS_MACSEC_RX_RMI_STATE_SHIFT 12

#ifdef __cplusplus
}
#endif

#endif /* __AL_HAL_ETH_MACSEC_REGS_H */

/** @} end of ... group */


