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

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

/**
 *  @{
 * @file   al_hal_eth_macsec.c
 *
 * @brief  XG Ethernet unit HAL driver for macsec support (encryption/authentication)
 *
 *
 */


#include "al_hal_eth.h"
#include "al_hal_eth_macsec_regs.h"
#include "al_hal_eth_ec_regs.h"



/* MacSec Constants */
#define AL_ETH_MACSEC_REGS_BASE_OFFSET_FROM_EC			0x3000
#define AL_ETH_MACSEC_SAD_DEPTH					128
#define AL_ETH_MACSEC_SC_MAP_DEPTH				 64
#define AL_ETH_MACSEC_CIPHER_MODE_GCM				  8
#define AL_ETH_MACSEC_CIPHER_CNTR_SIZE_32			  1
#define AL_ETH_MACSEC_EC_GEN_L2_SIZE_802_3_MS_8			 12


/* MacSec SAD parameters */
#define AL_ETH_MACSEC_SA_KEY_LSW				  0
#define AL_ETH_MACSEC_SA_KEY_MSW				  7
#define AL_ETH_MACSEC_SA_IV_LSW					  8
#define AL_ETH_MACSEC_SA_IV_MSW					 11
#define AL_ETH_MACSEC_SA_SCI_LSW				 12
#define AL_ETH_MACSEC_SA_SCI_MSW				 13
#define AL_ETH_MACSEC_SA_PN_MSW					 14
#define AL_ETH_MACSEC_SA_PARAMS_MSW				 15

#define AL_ETH_MACSEC_SA_PARAMS_ENTRY_VALID_SHIFT		 31
#define AL_ETH_MACSEC_SA_PARAMS_KEY_SIZE_MSB_SHIFT		 30
#define AL_ETH_MACSEC_SA_PARAMS_KEY_SIZE_LSB_SHIFT		 29
#define AL_ETH_MACSEC_SA_PARAMS_KEY_SIZE_MASK			 AL_FIELD_MASK(AL_ETH_MACSEC_SA_PARAMS_KEY_SIZE_MSB_SHIFT, \
										AL_ETH_MACSEC_SA_PARAMS_KEY_SIZE_LSB_SHIFT)
#define AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_MSB_SHIFT		 28
#define AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_LSB_SHIFT		 25
#define AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_MASK			 AL_FIELD_MASK(AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_MSB_SHIFT, \
									AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_LSB_SHIFT)
#define AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_MAX_ALLOWED		 16
#define AL_ETH_MACSEC_SA_PARAMS_END_STATION_SHIFT		 23	/* current node is an End-Station */
#define AL_ETH_MACSEC_SA_PARAMS_SECTAG_SCI_SHIFT		 22	/* if asserted, SCI field is to be encoded in the Sectag header
									 --> Sectag header is 16 Bytes long
									 (if deasserted, Sectag header length is 8 Bytes long)
									 */
#define AL_ETH_MACSEC_SA_PARAMS_REPLAY_EN_SHIFT			 20	/* Engine will perform replay-check on every RX pkt */
#define AL_ETH_MACSEC_SA_PARAMS_ENCAPSULATION_EN_SHIFT		 17	/* Add/Remove MacSec headers on TX/RX pkts */
#define AL_ETH_MACSEC_SA_PARAMS_SECTAG_OFFSET_MSB_SHIFT		 15
#define AL_ETH_MACSEC_SA_PARAMS_SECTAG_OFFSET_LSB_SHIFT		  0
#define AL_ETH_MACSEC_SA_PARAMS_SECTAG_OFFSET_MASK		AL_FIELD_MASK(AL_ETH_MACSEC_SA_PARAMS_SECTAG_OFFSET_MSB_SHIFT, \
									AL_ETH_MACSEC_SA_PARAMS_SECTAG_OFFSET_LSB_SHIFT)

/* MacSec SA-CAM parameters */
#define AL_ETH_MACSEC_SC_MAP_SCI_LSW				  0
#define AL_ETH_MACSEC_SC_MAP_SCI_MSW				  1
#define AL_ETH_MACSEC_SC_MAP_SCI_MASK_LSW			  2
#define AL_ETH_MACSEC_SC_MAP_SCI_MASK_MSW			  3
#define AL_ETH_MACSEC_SC_MAP_SC_INDEX_MSW			  4


/* internal function for performing write of a prepared SA entry to TX & RX SADs.
 *  This function handles relevant write to control register.
 */
static INLINE void al_eth_macsec_sad_entry_perform_write (struct al_macsec_regs	*macsec_regs_base, uint8_t index)
{
	al_reg_write32(&macsec_regs_base->cache_access_tx_sad_control.tx_sad_control, index | MACSEC_CACHE_ACCESS_TX_SAD_CONTROL_TX_SAD_CONTROL_PERFORM_WRITE);
	al_reg_write32(&macsec_regs_base->cache_access_rx_sad_control.rx_sad_control, index | MACSEC_CACHE_ACCESS_RX_SAD_CONTROL_RX_SAD_CONTROL_PERFORM_WRITE);
}

/* internal function for performing read of a SA entry from TX & RX SADs.
 *  This function handles relevant write to control register.
 */
static INLINE void al_eth_macsec_sad_entry_perform_read (struct al_macsec_regs	*macsec_regs_base, uint8_t index)
{
	al_reg_write32(&macsec_regs_base->cache_access_tx_sad_control.tx_sad_control, index | MACSEC_CACHE_ACCESS_TX_SAD_CONTROL_TX_SAD_CONTROL_PERFORM_READ);
	al_reg_write32(&macsec_regs_base->cache_access_rx_sad_control.rx_sad_control, index | MACSEC_CACHE_ACCESS_RX_SAD_CONTROL_RX_SAD_CONTROL_PERFORM_READ);
}

/* internal function for performing read of an SA *FIELD* from both TX & RX SADs.
 * if tx_field or rx_field are NULL, the relevant read will not take place
 */
static INLINE void al_eth_macsec_sa_field_read (struct al_macsec_regs	*macsec_regs_base,
						 uint8_t offset, uint32_t *tx_field, uint32_t *rx_field)
{
	if (tx_field != NULL)
		*tx_field = al_reg_read32(&macsec_regs_base->cache_access_tx_sad_data[offset].tx_sad_data);
	if (rx_field != NULL)
		*rx_field = al_reg_read32(&macsec_regs_base->cache_access_rx_sad_data[offset].rx_sad_data);
}

/* internal function for performing write of an SA *FIELD* to both TX & RX SADs */
static INLINE void al_eth_macsec_sa_field_write (struct al_macsec_regs	*macsec_regs_base,
						 uint8_t offset, uint32_t tx_field, uint32_t rx_field)
{
	al_reg_write32(&macsec_regs_base->cache_access_tx_sad_data[offset].tx_sad_data, tx_field);
	al_reg_write32(&macsec_regs_base->cache_access_rx_sad_data[offset].rx_sad_data, rx_field);
}

/* internal function extracting macsec regfile base address from adapter struct */
static INLINE struct al_macsec_regs *al_eth_macsec_get_regfile_base (struct al_hal_eth_adapter *adapter)
{
	return (struct al_macsec_regs *)((void __iomem*)(adapter->ec_regs_base) + AL_ETH_MACSEC_REGS_BASE_OFFSET_FROM_EC);
}


void al_eth_macsec_init	(	struct al_hal_eth_adapter	*adapter,
				al_bool				sad_access_mode,
				uint8_t				num_an_per_sc,
				uint32_t			pn_threshold,
				al_bool				enable
			)
{
	uint32_t conf = 0;
	struct al_macsec_regs *macsec_regs_base = al_eth_macsec_get_regfile_base(adapter);

	/* configuring size of 802_3_macsec_8 header */
	conf = al_reg_read32(&adapter->ec_regs_base->gen.l2);
	AL_REG_FIELD_SET(conf, EC_GEN_L2_SIZE_802_3_MS_8_MASK,
				EC_GEN_L2_SIZE_802_3_MS_8_SHIFT,
				AL_ETH_MACSEC_EC_GEN_L2_SIZE_802_3_MS_8);
	al_reg_write32(&adapter->ec_regs_base->gen.l2, conf);

	/* allowing macsec to modify completion descriptor */
	conf = al_reg_read32(&adapter->ec_regs_base->rfw.out_cfg);
	conf |= EC_RFW_OUT_CFG_EN_MACSEC_DEC;
	al_reg_write32(&adapter->ec_regs_base->rfw.out_cfg, conf);

	/* macsec crypto engine initialization */
	al_dbg("eth [%s]: enable macsec crypto engine\n", adapter->name);
	conf = 0;
	AL_REG_FIELD_SET(conf, MACSEC_CONFIGURATION_CONF_MACSEC_TX_CRYPTO_ENGINE_AES_MODE_MASK,
				MACSEC_CONFIGURATION_CONF_MACSEC_TX_CRYPTO_ENGINE_AES_MODE_SHIFT,
				AL_ETH_MACSEC_CIPHER_MODE_GCM);
	AL_REG_FIELD_SET(conf, MACSEC_CONFIGURATION_CONF_MACSEC_TX_CRYPTO_ENGINE_AES_CNTR_STEP_MASK,
				MACSEC_CONFIGURATION_CONF_MACSEC_TX_CRYPTO_ENGINE_AES_CNTR_STEP_SHIFT,
				AL_ETH_MACSEC_CIPHER_CNTR_SIZE_32);

	/* sad_access_mode */
	if (sad_access_mode)
		conf |= MACSEC_CONFIGURATION_CONF_MACSEC_TX_DIRECT_ACCESS;

	/* num_an_per_sc.
	 * according to 802.1AE, the legal values for num_an_per_sc are 2,4 */
	al_assert((num_an_per_sc == 2) || (num_an_per_sc == 4));
	if (num_an_per_sc == 4) {
		conf |= MACSEC_CONFIGURATION_CONF_MACSEC_TX_NUM_SA_PER_SC;
	}

	if (enable)
		conf |= MACSEC_CONFIGURATION_CONF_MACSEC_TX_ENABLE_ENGINE;

	/* enabling tunnel-mode (macsec/GRE) */
	conf |= MACSEC_CONFIGURATION_CONF_MACSEC_RX_TUNNEL_ENABLE;

	/* Initialize the TX engine */
	al_reg_write32(&macsec_regs_base->configuration.conf_macsec_tx_pn_thr, pn_threshold);
	al_reg_write32(&macsec_regs_base->configuration.conf_macsec_tx, conf);

	/* Initialize the RX engine (same fields as in TX configuration) */
	al_reg_write32(&macsec_regs_base->configuration.conf_macsec_rx_pn_thr, pn_threshold);
	al_reg_write32(&macsec_regs_base->configuration.conf_macsec_rx, conf);
}


void al_eth_macsec_sad_entry_write (	struct al_hal_eth_adapter	*adapter,
					struct al_eth_macsec_sa		*sa,
					uint8_t				index)
{
	int			i;
	uint32_t		params;
	struct al_macsec_regs *macsec_regs_base = al_eth_macsec_get_regfile_base(adapter);

	/* verifying that index is legal */
	al_assert (index < AL_ETH_MACSEC_SAD_DEPTH);

	/* set key */
	for (i = AL_ETH_MACSEC_SA_KEY_LSW ; i <= AL_ETH_MACSEC_SA_KEY_MSW ; i++) {
		al_eth_macsec_sa_field_write(macsec_regs_base, i,
						sa->key[AL_ETH_MACSEC_SA_KEY_MSW-i],
						sa->key[AL_ETH_MACSEC_SA_KEY_MSW-i]);
	}

	/* set IV (Initialization Vector) */
	for (i = AL_ETH_MACSEC_SA_IV_LSW ; i <= AL_ETH_MACSEC_SA_IV_MSW ; i++) {
		al_eth_macsec_sa_field_write(macsec_regs_base, i,
						sa->iv[AL_ETH_MACSEC_SA_IV_MSW-i],
						sa->iv[AL_ETH_MACSEC_SA_IV_MSW-i]);
	}

	/* set SCI (Secure Channel Identifier) */
	for (i = AL_ETH_MACSEC_SA_SCI_LSW ; i <= AL_ETH_MACSEC_SA_SCI_MSW ; i++) {
		al_eth_macsec_sa_field_write(macsec_regs_base, i,
						sa->sci[AL_ETH_MACSEC_SA_SCI_MSW-i],
						sa->sci[AL_ETH_MACSEC_SA_SCI_MSW-i]);
	}

	/* set PN (Packet Number) */
	al_eth_macsec_sa_field_write(macsec_regs_base, AL_ETH_MACSEC_SA_PN_MSW, sa->pn_tx, sa->pn_rx);

	/* adjusting signature_size.
	 *  value 0 means signature size of AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_MAX_ALLOWED Bytes
	 */
	al_assert(sa->signature_size <= AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_MAX_ALLOWED);
	if (sa->signature_size == AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_MAX_ALLOWED)
		sa->signature_size = 0;

	/* set macsec Params */
	params = (sa->sectag_offset		<< AL_ETH_MACSEC_SA_PARAMS_SECTAG_OFFSET_LSB_SHIFT	) |
		 (sa->sectag_tci_encode_sci	<< AL_ETH_MACSEC_SA_PARAMS_SECTAG_SCI_SHIFT		) |
		 (sa->sectag_tci_end_station	<< AL_ETH_MACSEC_SA_PARAMS_END_STATION_SHIFT		) |
		 (sa->macsec_encapsulation_en	<< AL_ETH_MACSEC_SA_PARAMS_ENCAPSULATION_EN_SHIFT	) |
		 (sa->replay_offload_en		<< AL_ETH_MACSEC_SA_PARAMS_REPLAY_EN_SHIFT		) |
		 (sa->signature_size		<< AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_LSB_SHIFT		) |
		 (sa->key_size			<< AL_ETH_MACSEC_SA_PARAMS_KEY_SIZE_LSB_SHIFT		) |
		 (sa->valid			<< AL_ETH_MACSEC_SA_PARAMS_ENTRY_VALID_SHIFT		);

	al_eth_macsec_sa_field_write(macsec_regs_base, AL_ETH_MACSEC_SA_PARAMS_MSW, params, params);

	/* Perform write to SAD */
	al_eth_macsec_sad_entry_perform_write(macsec_regs_base, index);
}


void al_eth_macsec_sad_entry_read (	struct al_hal_eth_adapter	*adapter,
					struct al_eth_macsec_sa		*sa,
					uint8_t				index)
{
	int			i;
	uint32_t		params;
	struct al_macsec_regs *macsec_regs_base = al_eth_macsec_get_regfile_base(adapter);

	/* verifying that index is legal */
	al_assert (index < AL_ETH_MACSEC_SAD_DEPTH);

	/* Perform read from SAD */
	al_eth_macsec_sad_entry_perform_read(macsec_regs_base, index);

	/* get key (enough to read from TX SAD only, since TX & RX SADs are guaranteed by design to have equal keys) */
	for (i = AL_ETH_MACSEC_SA_KEY_LSW ; i <= AL_ETH_MACSEC_SA_KEY_MSW ; i++)
		al_eth_macsec_sa_field_read(macsec_regs_base, i, &sa->key[AL_ETH_MACSEC_SA_KEY_MSW-i], NULL);

	/* get IV (Initialization Vector) */
	for (i = AL_ETH_MACSEC_SA_IV_LSW ; i <= AL_ETH_MACSEC_SA_IV_MSW ; i++)
		al_eth_macsec_sa_field_read(macsec_regs_base, i, &sa->iv[AL_ETH_MACSEC_SA_IV_MSW-i], NULL);

	/* get SCI (Secure Channel Identifier) */
	for (i = AL_ETH_MACSEC_SA_SCI_LSW ; i <= AL_ETH_MACSEC_SA_SCI_MSW ; i++)
		al_eth_macsec_sa_field_read(macsec_regs_base, i, &sa->sci[AL_ETH_MACSEC_SA_SCI_MSW-i], NULL);

	/* get PN (Packet Number) */
	al_eth_macsec_sa_field_read(macsec_regs_base, AL_ETH_MACSEC_SA_PN_MSW, &sa->pn_tx, &sa->pn_rx);

	/* get macsec Params */
	al_eth_macsec_sa_field_read(macsec_regs_base, AL_ETH_MACSEC_SA_PARAMS_MSW, &params, NULL);

	/* parsing params: */
	sa->sectag_offset		= AL_REG_FIELD_GET(params,
						AL_ETH_MACSEC_SA_PARAMS_SECTAG_OFFSET_MASK,
						AL_ETH_MACSEC_SA_PARAMS_SECTAG_OFFSET_LSB_SHIFT);
	sa->sectag_tci_encode_sci	= AL_REG_FIELD_GET(params,
						AL_BIT(AL_ETH_MACSEC_SA_PARAMS_SECTAG_SCI_SHIFT),
						AL_ETH_MACSEC_SA_PARAMS_SECTAG_SCI_SHIFT);
	sa->sectag_tci_end_station	= AL_REG_FIELD_GET(params,
						AL_BIT(AL_ETH_MACSEC_SA_PARAMS_END_STATION_SHIFT),
						AL_ETH_MACSEC_SA_PARAMS_END_STATION_SHIFT);
	sa->macsec_encapsulation_en	= AL_REG_FIELD_GET(params,
						AL_BIT(AL_ETH_MACSEC_SA_PARAMS_ENCAPSULATION_EN_SHIFT),
						AL_ETH_MACSEC_SA_PARAMS_ENCAPSULATION_EN_SHIFT);
	sa->replay_offload_en		= AL_REG_FIELD_GET(params,
						AL_BIT(AL_ETH_MACSEC_SA_PARAMS_REPLAY_EN_SHIFT),
						AL_ETH_MACSEC_SA_PARAMS_REPLAY_EN_SHIFT);
	sa->signature_size		= AL_REG_FIELD_GET(params,
						AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_MASK,
						AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_LSB_SHIFT);
	sa->key_size			= AL_REG_FIELD_GET(params,
						AL_ETH_MACSEC_SA_PARAMS_KEY_SIZE_MASK,
						AL_ETH_MACSEC_SA_PARAMS_KEY_SIZE_LSB_SHIFT);
	sa->valid			= AL_REG_FIELD_GET(params,
						AL_BIT(AL_ETH_MACSEC_SA_PARAMS_ENTRY_VALID_SHIFT),
						AL_ETH_MACSEC_SA_PARAMS_ENTRY_VALID_SHIFT);

	/* adjusting signature_size.
	 * since value 0 means signature size of AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_MAX_ALLOWED Bytes
	 */
	if (sa->signature_size == 0)
		sa->signature_size = AL_ETH_MACSEC_SA_PARAMS_SIGN_SIZE_MAX_ALLOWED;

}


al_bool al_eth_macsec_sad_entry_is_valid (struct al_hal_eth_adapter *adapter, uint8_t index)
{
	uint32_t		params_tx, params_rx;
	struct al_macsec_regs *macsec_regs_base = al_eth_macsec_get_regfile_base(adapter);

	/* verifying that index is legal */
	al_assert (index < AL_ETH_MACSEC_SAD_DEPTH);

	/* Perform read from SAD */
	al_eth_macsec_sad_entry_perform_read(macsec_regs_base, index);

	/* read SA's params, to check if entry is valid */
	al_eth_macsec_sa_field_read(macsec_regs_base, AL_ETH_MACSEC_SA_PARAMS_MSW, &params_tx, &params_rx);

	return ((params_tx & AL_BIT(AL_ETH_MACSEC_SA_PARAMS_ENTRY_VALID_SHIFT)) &&
		(params_rx & AL_BIT(AL_ETH_MACSEC_SA_PARAMS_ENTRY_VALID_SHIFT))	)	?  AL_TRUE : AL_FALSE;
}


void al_eth_macsec_sad_entry_invalidate (struct al_hal_eth_adapter *adapter, uint8_t index)
{
	uint32_t		params;
	struct al_macsec_regs *macsec_regs_base = al_eth_macsec_get_regfile_base(adapter);

	/* verifying that index is legal */
	al_assert (index < AL_ETH_MACSEC_SAD_DEPTH);

	/* invalidate SA entry */
	params = 0;
	al_eth_macsec_sa_field_write(macsec_regs_base, AL_ETH_MACSEC_SA_PARAMS_MSW, params, params);

	/* Perform write to SAD */
	al_eth_macsec_sad_entry_perform_write(macsec_regs_base, index);
}

void al_eth_macsec_sc_map_entry_write (struct al_hal_eth_adapter		*adapter,
					uint32_t				*sci,
					uint32_t				*sci_mask,
					uint8_t					sc_index)
{
	int			i;
	struct al_macsec_regs *macsec_regs_base = al_eth_macsec_get_regfile_base(adapter);

	/* verifying that sc index is legal */
	al_assert (sc_index < AL_ETH_MACSEC_SC_MAP_DEPTH);

	/* set SCI (Secure Channel Identifier) */
	for (i = AL_ETH_MACSEC_SC_MAP_SCI_LSW ; i <= AL_ETH_MACSEC_SC_MAP_SCI_MSW ; i++) {
		al_reg_write32(&macsec_regs_base->cam_access_rx_sa_index_build_data[i].sa_index_build_data ,
				sci[AL_ETH_MACSEC_SC_MAP_SCI_MSW-i]);
	}

	/* set SCI Mask (Secure Channel Identifier's Mask) */
	for (i = AL_ETH_MACSEC_SC_MAP_SCI_MASK_LSW ; i <= AL_ETH_MACSEC_SC_MAP_SCI_MASK_MSW ; i++) {
		al_reg_write32(&macsec_regs_base->cam_access_rx_sa_index_build_data[i].sa_index_build_data ,
				sci_mask[AL_ETH_MACSEC_SC_MAP_SCI_MASK_MSW-i]);
	}

	/* set SC Index (Secure Channel Index) */
	al_reg_write32(&macsec_regs_base->cam_access_rx_sa_index_build_data[AL_ETH_MACSEC_SC_MAP_SC_INDEX_MSW].sa_index_build_data ,
			sc_index);

	/* perform write */
	al_reg_write32(&macsec_regs_base->cam_access_rx_sa_index_build_control.sa_index_build_control,
			sc_index | MACSEC_CAM_ACCESS_RX_SA_INDEX_BUILD_CONTROL_SA_INDEX_BUILD_CONTROL_PERFORM_WRITE);
}

unsigned int al_eth_macsec_read_exhausted_tx_sa_index (struct al_hal_eth_adapter *adapter)
{
	struct al_macsec_regs *macsec_regs_base = al_eth_macsec_get_regfile_base(adapter);
	uint32_t tx_status;
	unsigned int exhausted_sa_index;

	tx_status = al_reg_read32(&macsec_regs_base->status.status_macsec_tx);
	exhausted_sa_index = (unsigned int)((tx_status & MACSEC_STATUS_STATUS_MACSEC_TX_PN_THR_SA_INDEX_MASK)
							>> MACSEC_STATUS_STATUS_MACSEC_TX_PN_THR_SA_INDEX_SHIFT);

	return exhausted_sa_index;
}

unsigned int al_eth_macsec_read_exhausted_rx_sa_index (struct al_hal_eth_adapter *adapter)
{
	struct al_macsec_regs *macsec_regs_base = al_eth_macsec_get_regfile_base(adapter);
	uint32_t rx_status;
	unsigned int exhausted_sa_index;

	rx_status = al_reg_read32(&macsec_regs_base->status.status_macsec_rx);
	exhausted_sa_index = (unsigned int)((rx_status & MACSEC_STATUS_STATUS_MACSEC_RX_PN_THR_SA_INDEX_MASK) >>
								MACSEC_STATUS_STATUS_MACSEC_RX_PN_THR_SA_INDEX_SHIFT);

	return exhausted_sa_index;
}

void al_eth_macsec_read_stats_cntr (struct al_hal_eth_adapter *adapter, struct al_eth_macsec_stats_cntr *stats_cntrs)
{
	struct al_macsec_regs *macsec_regs_base = al_eth_macsec_get_regfile_base(adapter);

	al_assert (stats_cntrs != NULL);

	stats_cntrs->tx_pkts			= al_reg_read32(&macsec_regs_base->statistics.macsec_tx_pkts);
	stats_cntrs->tx_secured_pkts		= al_reg_read32(&macsec_regs_base->statistics.macsec_tx_controlled_pkts);
	stats_cntrs->tx_unsecured_pkts		= al_reg_read32(&macsec_regs_base->statistics.macsec_tx_uncontrolled_pkts);
	stats_cntrs->rx_pkts			= al_reg_read32(&macsec_regs_base->statistics.macsec_rx_pkts);
	stats_cntrs->rx_secured_pkts		= al_reg_read32(&macsec_regs_base->statistics.macsec_rx_controlled_pkts);
	stats_cntrs->rx_unsecured_pkts		= al_reg_read32(&macsec_regs_base->statistics.macsec_rx_uncontrolled_pkts);
	stats_cntrs->rx_replay_fail_pkts	= al_reg_read32(&macsec_regs_base->statistics.macsec_rx_replay_pkts);
	stats_cntrs->rx_auth_fail_pkts		= al_reg_read32(&macsec_regs_base->statistics.macsec_rx_auth_fail_pkts);
}

