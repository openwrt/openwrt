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
 * @defgroup group_eth_macsec_api API
 * Ethernet Controller MacSec HAL driver API
 * @ingroup group_eth
 * @{
 * @file   al_hal_eth_macsec.h
 *
 * @brief Header file for unified 10GbE/1GbE Ethernet Controller's MacSec engine.
 */

#ifndef __AL_HAL_ETH_MACSEC_H__
#define __AL_HAL_ETH_MACSEC_H__

#include <mach/al_hal_common.h>
#include "al_hal_eth.h"


/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */


/*******************************************************************************
This file defines the API for MAC Security protocol (MacSec), which provides
the possibility to maintain a secured L2 or overlay (tunnel) link, at line rate
bandwidth (10Gbps).
The advanced Ethernet Controller in Alpine Platform-on-Chip, implementes an
inline MacSec accelerator, which enables each Tx/Rx pkt to be
encrypted/decrypted and signed/authenticated, as defined in IEEE 802.1AE, as
well as in novel extensions, enabling inline AES-GCM-GMAC on alternative payload
inluding L3 or passenger tunnels.
The MacSec engine is designed to provide L2 security, with minimum SW changes,
in order to help the user achieve maximum performance & security, with minimum
overhead.  The engine supports AES-128, AES-192 and AES-256 encryption, and
could be configured for Authentication-mode only or full data protection that
provides encryption and authentication.


Terminology:
=================
1. SA - Security Association. A set of cryptographic parameters needed to secure
	the traffic (such as key, initialization-vector, packet-number, etc).
	For improved performance, SAs are stored in an internal scratch-pad
	memory named SAD (Security Association Data-Structure).
	Each SAD entry stores a single SA.
2. SC - Secured Channel. A point-to-point secured link.
	Each SC comprises a succession of up to 4 SAs, which are indexed by a
	two-bit integer called AN (Association Number).
	Each TX MacSec packet is given a SC-index and an AN, from which the
	relevant SA index is constructed.
3. Association Number - index of currently used SA in the Secured Channel (used
			for hot swap).
4. Hot Swap - Switching SAs (key, IV, etc) in runtime.
	Hot-Swap is supported by pre-loading up-to 4 SAs per SC, and using the
	AN to switch between SAs in runtime.
	+ TX side:	The transmitter is responsible for initiating hot-swap.
			Once the TX packet-number reaches a pre-configured
			threshold, an IRQ is raised, signaling the upper layer
			to switch keys. This is performed by simply incrementing
			the AN of the next packets to be transmitted.
			Note: AN is encoded in the  MacSec SECTAG header.
	+ RX side:	The receiver acts as slave in the hot-swap mechanism. It
			receives the SC & AN indication from the SECTAG header,
			and thus when the transmitter performs hot-swap
			(increments AN), the receiver immediately follows.
5. SAD - Security Association Data-Structure, scratch-pad memory for storing SAs.
	 TX-SAD & RX-SAD are always written in pairs, and maintained by the HW.
	 Maintanance includes incrementing of PN value (Packet-Number).
	 + TX side: next PN to be coded in MacSec header
	 + RX side: next expected PN (used for replay attack protection)

	 AN SAD can store up to 128 SAs (== SAD entries).
	 In common usage (i.e. sad_access_mode=0, num_an_per_sc=0),  SAD will
	 be organized as follows:
	 =================================================
	            |  SA_INDEX  |   SC_INDEX  |  AN   |
	            ====================================
	PF SAs:     |    0       |      0      |   0   |
	=======     |    1       |      0      |   1   |
	            |    2       |      1      |   0   |
	                           ....
	                           ....
	            |   30       |     15      |   0   |
	            |   31       |     15      |   1   |

	VF1 SAs:    |   32       |     16      |   0   |
	========    |   33       |     16      |   1   |
	                           ....
	                           ....
	            |   62       |     31      |   0   |
	            |   63       |     31      |   1   |

	VF2 SAs:    |   64       |     32      |   0   |
	========    |   65       |     32      |   1   |
	                           ....
	                           ....
	            |   94       |     47      |   0   |
	            |   95       |     47      |   1   |

	VF3 SAs:    |   96       |     48      |   0   |
	========    |   97       |     48      |   1   |
	                           ....
	                           ....
	            |  126       |     63      |   0   |
	            |  127       |     63      |   1   |
	 =================================================


Common Usage:
=================

1. Initialization stage
	initialization should be performed by by the the main driver, and in
	case of IO Virtualization/SRIOV, the MacSec engine must be initialized
	by the PF driver (vs. the VF driver)
	During initialization, user must make sure that no traffic is sent/received.
	--------------------------
	+ common usage example:
	--------------------------
		al_eth_macsec_init (adapter, 0, 0, 0xFF000000, 1);

2. Loading crypto parameters
	MacSec engine contains a scratch-pad memory, called SAD, for storing
	cryptographic parameters (SAs) of each Secured Channel.
	SAD should be updated periodically by the PF, according to an upper-
	layer protocl such as 802.1AF.
	--------------------------
	+ common usage example:
	--------------------------
		struct al_eth_macsec_sa sa =  {
			.valid				=  1,
			.key_size			= AL_ETH_KEY_SIZE_128,
			.key				= <ptr_prepared_key>,

			.iv				= <ptr_prepared_iv>,
			.sci				= <ptr_prepared_sci>,
			.pn_rx				= <prepared rx pn>,
			.pn_tx				= <prepared tx pn>,
			.signature_size 		= 16,

			.replay_offload_en		=  1,
			.macsec_encapsulation_en	=  1,
			.sectag_tci_end_station		=  0,
			.sectag_tci_encode_sci		=  1,
			.sectag_offset			= 12
		};
		al_eth_macsec_sad_entry_write (adapter, &sa, <some sad index>);
		al_eth_macsec_sc_map_entry_write (adapter, sa.sci, sci_mask, <some sc index>);
		// sc_index should be construct as sad_index >> 1/2 (depending on number of AN/SC)


	* NOTE1:  index should be constructed as follows
		  (2 options, dependent on initialization):
		    + option #1: 5-bit secured channel, 2-bit association number
		    + option #2: 6-bit secured_channel, 1-bit association number
	* NOTE2:  in common usage, each VF receives 32 dedicated SA entries.
		  in such a case, the relevant offset (VF ID x 32) should be
		  added to index when calling al_eth_macsec_sad_entry_write.

3. Sending macsec packets is performed independently by the VFs.
	--------------------------
	+ common usage example:
	--------------------------
		struct al_eth_pkt pkt {
		...
		.macsec_secure_channel	   = <relevant secure channel idx>,
		.macsec_association_number = <relevant association number>,
		.macsec_flags              = 7  // encrypt & sign pkt flags
		// for further details regarding macsec_flags, see:
		// AL_ETH_MACSEC_TX_FLAGS_* and AL_ETH_MACSEC_RX_FLAGS_*
		...
		};
		pkt.flags |= AL_ETH_TX_FLAGS_L2_MACSEC_PKT;  // pkt is macsec-pkt
		pkt.macsec_flags = AL_ETH_MACSEC_TX_FLAGS_SIGN; // pkt should be signed

		pkt.macsec_secure_channel = <secure channel>;
		pkt.macsec_association_number = <association number>;
		al_eth_tx_pkt_prepare(tx_dma_q, &pkt);

4. Receiving macsec packets is performed independetly by the VFs.
	--------------------------
	+ common usage example:
	--------------------------
	// ... code for receiving a pkt:... //

	// checking if packet is macsec:
	al_bool is_macsec_pkt = pkt.macsec_flags & AL_ETH_MACSEC_RX_FLAGS_IS_MACSEC;

	// checking for authentication success:
	al_bool is_auth_sucess = is_macsec_pkt &
				(pkt.macsec_flags & AL_ETH_MACSEC_RX_FLAGS_AUTH_SUCCESS);

*******************************************************************************/


/** MacSec Constants */
#define AL_ETH_MACSEC_KEY_NUM_WORDS			  8	/**< 32 Bytes */
#define AL_ETH_MACSEC_IV_NUM_WORDS			  4	/**< 16 Bytes */
#define AL_ETH_MACSEC_SCI_NUM_WORDS			  2	/**<  8 Bytes */

/** Packet Rx Macsec flags */
#define AL_ETH_MACSEC_RX_FLAGS_MSB_SHIFT		 31
#define AL_ETH_MACSEC_RX_FLAGS_LSB_SHIFT		 16
#define AL_ETH_MACSEC_RX_FLAGS_MASK			AL_FIELD_MASK(AL_ETH_MACSEC_RX_FLAGS_MSB_SHIFT, \
									AL_ETH_MACSEC_RX_FLAGS_LSB_SHIFT)
#define AL_ETH_MACSEC_RX_FLAGS_IS_MACSEC		AL_BIT(0)	/**< Sign TX pkt */
#define AL_ETH_MACSEC_RX_FLAGS_SL_MSB_SHIFT		  6		/**< Short Length */
#define AL_ETH_MACSEC_RX_FLAGS_SL_LSB_SHIFT		  1
#define AL_ETH_MACSEC_RX_FLAGS_SL_MASK			AL_FIELD_MASK(AL_ETH_MACSEC_RX_FLAGS_SL_MSB_SHIFT, \
									AL_ETH_MACSEC_RX_FLAGS_SL_LSB_SHIFT)
#define AL_ETH_MACSEC_RX_FLAGS_AN_MSB_SHIFT		  8		/*< Association Number */
#define AL_ETH_MACSEC_RX_FLAGS_AN_LSB_SHIFT		  7
#define AL_ETH_MACSEC_RX_FLAGS_AN_MASK			AL_FIELD_MASK(AL_ETH_MACSEC_RX_FLAGS_AN_MSB_SHIFT, \
									AL_ETH_MACSEC_RX_FLAGS_AN_LSB_SHIFT)
#define AL_ETH_MACSEC_RX_FLAGS_IS_ENCRYPTED_SHIFT	  9		/**< if asserted, rx L2 pkt was encrypted */
#define AL_ETH_MACSEC_RX_FLAGS_IS_ENCRYPTED_MASK	AL_BIT(AL_ETH_MACSEC_RX_FLAGS_IS_ENCRYPTED_SHIFT)
#define AL_ETH_MACSEC_RX_FLAGS_IS_SIGNED_SHIFT		 10		/**< if asserted, rx L2 pkt was signed */
#define AL_ETH_MACSEC_RX_FLAGS_IS_SIGNED_MASK		AL_BIT(AL_ETH_MACSEC_RX_FLAGS_IS_SIGNED_SHIFT)
#define AL_ETH_MACSEC_RX_FLAGS_IS_REPLAY_PROTECTED_SHIFT 11		/**< if asserted, replay protection mechanism was applied to rx pkt */
#define AL_ETH_MACSEC_RX_FLAGS_IS_REPLAY_PROTECTED_MASK	AL_BIT(AL_ETH_MACSEC_RX_FLAGS_IS_REPLAY_PROTECTED_SHIFT)
#define AL_ETH_MACSEC_RX_FLAGS_AUTH_SUCCESS_SHIFT	 12		/**< if asserted, rx pkt's signature was verified to be correct */
#define AL_ETH_MACSEC_RX_FLAGS_AUTH_SUCCESS_MASK	AL_BIT(AL_ETH_MACSEC_RX_FLAGS_AUTH_SUCCESS_SHIFT)
#define AL_ETH_MACSEC_RX_FLAGS_REPLAY_SUCCESS_SHIFT	 13		/**< if asserted, pkt is verified to not be a replay-attack pkt */
#define AL_ETH_MACSEC_RX_FLAGS_REPLAY_SUCCESS_MASK	AL_BIT(AL_ETH_MACSEC_RX_FLAGS_REPLAY_SUCCESS_SHIFT)



/** Encryption key size.
 * AES key size (traditionally referring to bit=length)
 */
enum al_eth_macsec_key_size	{AL_ETH_KEY_SIZE_128, AL_ETH_KEY_SIZE_192, AL_ETH_KEY_SIZE_256};


/** MacSec statistics counters.
 * Statistics counters interface
 */
struct al_eth_macsec_stats_cntr {
	uint32_t tx_pkts;		/* number of tx pkts processed by the MacSec engine */
	uint32_t tx_secured_pkts;	/* number of tx pkts processed by the MacSec engine
					   which are either encrypted or authenticated (or both) */
	uint32_t tx_unsecured_pkts;	/* number of tx pkts processed by the MacSec engine
					   which are neither encrypted nor authenticated */
	uint32_t rx_pkts;		/* number of rx pkts processed by the MacSec engine */
	uint32_t rx_secured_pkts;        /* number of rx pkts processed by the MacSec engine
					   which are either encrypted or authenticated (or both) */
	uint32_t rx_unsecured_pkts;	/* number of rx pkts processed by the MacSec engine
					   which are neither encrypted nor authenticated */
	uint32_t rx_replay_fail_pkts;	/* number of rx pkts processed by the MacSec engine
					   which failed replay check */
	uint32_t rx_auth_fail_pkts;	/* number of rx pkts processed by the MacSec engine
					   which failed authentication */
	};


/** MacSec SAD (Security Association Data-Structure).
 * MacSec engine has an internal scratch-pad memory, used to cache security
 * parameters for active channels. A SAD entry is called SA, and is described
 * by the structure below.
 * NOTE: for all multiple-words fields, word #0 is LSW.
 */
struct al_eth_macsec_sa {
	al_bool				valid;					/* entry is valid */

	/** cryptographic parameters */
	enum al_eth_macsec_key_size	key_size;
	uint32_t			key[AL_ETH_MACSEC_KEY_NUM_WORDS];	/* Encryption Key. For 128/192-bit keys, pad LSWs with zeros */

	uint32_t			iv[AL_ETH_MACSEC_IV_NUM_WORDS];		/* == AES_k(0). Meaning, this field should be written with
										  the result of encrypting 16 zero-bytes, with the SA's key.
										  This is not abstracted from the user, in order not to insert
										  encryption algorithm into a low-level HAL code
										  */

	uint32_t			sci[AL_ETH_MACSEC_SCI_NUM_WORDS];	/* Secure Channel Identifier */
	uint32_t			pn_tx;					/* Packet Number for TX packets */
	uint32_t			pn_rx;					/* Packet Number for RX packets (maintained for replay attack protection) */
	uint8_t				signature_size:5;			/* legal values are 1-16 */

	/** macsec protocol parameters */
	al_bool				replay_offload_en;			/* when asserted, hw will perform replay protection for incoming pkts */
	al_bool				macsec_encapsulation_en;		/* when asserted, macsec will seamlessly add (tx) & remove (rx)
										   macsec headers to/from packet.
										   */
	al_bool				sectag_tci_end_station;			/* current node is an end-station. See 802.1AE for further details. */
	al_bool				sectag_tci_encode_sci;			/* when asserted, sci-field will be encoded in all tx macsec packets
										   see 802.1AE for further details
										   */
	uint16_t			sectag_offset;				/* location of MacSec header (SECTAG). for regular (non-tunneled) traffic,
										   this field should be written with 0xC (SA len + DA len).
										   otherwise, GRE header length should be taken into account.
										   */
};


/* forward declaration */
struct al_hal_eth_adapter;


/**
 * @brief	This function initializes and enables the inline MacSec engine.
 *		- Initialize TX and RX inline Crypto engines
 *		- Enable TX and RX inline Crypto engines
 *
 * @param	adapter		Pointer to eth's private data-structure. The
 *				adapter is used in order to extract the base
 *				macsec configuration address.
 * @param	sad_access_mode	if asserted, all SAD entries will be available
 *				to all VFs.  Otherwise, each VF will get
 *				AL_ETH_MACSEC_SAD_DEPTH/4 dedicated entries
 *				for caching SAs.
 * @param	num_an_per_sc	Number of ANs (Association Numbers) per SC
 *				(Secure Channel). ANs are aimed to allow
 *				hot-swap of encryption keys.
 *				** 802.1AE allows only 2 or 4 ANs per SC.
 * @param	pn_threshold	when Packet-Number (which is maintained by HW)
 *				passes this threshold, an IRQ will be raised
 *				signaling the driver that it is time to load a
 *				new SA to SAD
 * @param	enable		if asserted, enable the inline MacSec engine
 *				(which is disabled at reset)
 *
 */
void al_eth_macsec_init	(	struct al_hal_eth_adapter	*adapter,
				al_bool				sad_access_mode,
				uint8_t				num_an_per_sc,
				uint32_t			pn_threshold,
				al_bool				enable
			);


/**
 * @brief	this function writes a Security Association (SA) to the
 *		Security Association Data-Structure (SAD). The SAD can store up
 *		to 128 SAs.
 *		This function writes both TX-SAD & RX-SAD, since SA should always
 *		come in pairs. the only value that is allowed to differ between
 *		TX-SAD & RX-SAD is PN (packet-number).
 *		As explained in the terminology section, user can store up to 4
 *		SAs per SC, to support hot swap of crypto parameters.
 *		When calling this function, user should consider the
 *		configuration of the MacSec engine:
 *		+ if sad_access_mode was set to 1'b0, user should arrange SAD as
 *		  4 distinct index-spaces, one for each VF.
 *		  [ PF : sa0 , sa1 , ..,. sa31  ]
 *		  [ VF1: sa32, sa33, ...  sa63  ]
 *		  [ VF2: sa64, sa65, ...  sa95  ]
 *		  [ VF3: sa96, sa97, ...  sa127 ]
 *		+ if num_an_per_sc was set to 1'b1, each SC should have 4 SAs.
 *		  otherwise, each SC should have 2 ANs.
 *
 *		Note: This function is allowed to be called by the PF driver only.
 *
 * @param	adapter		Pointer to eth's private data-structure. The
 *				adapter is used in order to extract the base
 *				macsec configuration address.
 * @param	sa		SA to be written to SAD
 * @param	index		SAD index to be written. Legal values are 0-127.
 */
void al_eth_macsec_sad_entry_write (	struct al_hal_eth_adapter	*adapter,
					struct al_eth_macsec_sa		*sa,
					uint8_t				index);


/**
 * @brief	this function reads a Security Association (SA) from the
 *		Security Association Data-Structure (SAD).
 *
 *		Note: This function is allowed to be called by the PF driver only.
 *
 * @param	adapter		Pointer to eth's private data-structure. The
 *				adapter is used in order to extract the base
 *				macsec configuration address.
 * @param	sa		read SA from SAD
 * @param	index		SAD index to be read. Legal values are 0-127.
 */
void al_eth_macsec_sad_entry_read (	struct al_hal_eth_adapter	*adapter,
					struct al_eth_macsec_sa		*sa,
					uint8_t				index);


/**
 * @brief	this function checks if a SAD entry (SA) is valid.
 *
 *		Note: This function is allowed to be called by the PF driver only.
 *
 * @param	adapter		Pointer to eth's private data-structure. The
 *				adapter is used in order to extract the base
 *				macsec configuration address.
 * @param	index		SAD index to be checked. Legal values are 0-127.
 */
al_bool al_eth_macsec_sad_entry_is_valid (struct al_hal_eth_adapter *adapter, uint8_t index);


/**
 * @brief	this function invalidates a SAD entry (SA).
 *
 *		Note: This function is allowed to be called by the PF driver only.
 *
 * @param	adapter		Pointer to eth's private data-structure. The
 *				adapter is used in order to extract the base
 *				macsec configuration address.
 * @param	index		SAD index to be invalidated.
 *				Legal values are 0-127.
 */
void al_eth_macsec_sad_entry_invalidate (struct al_hal_eth_adapter *adapter, uint8_t index);


/**
 * @brief	this function writes an entry to the SC map table.
 *		The SC map is responsible for mapping between SCI and SC indexes
 *		and is used to extract SA index from the (rx) packet's macsec header
 *		(namely, its SCI & AN fields).
 *		This function should be called immediately after loading a new
 *		SA to the RX-SAD.
 * @param	adapter		Pointer to eth's private data-structure. The
 *				adapter is used in order to extract the base
 *				macsec configuration address.
 * @param	sci		pointer to an array of two uint32_t elements,
 *				which holds the 64-bit SCI (Secured Channel Identifier)
 *				that will be recorded in the SC map
 * @param	sci_mask	pointer to an array of two uint32_t elements
 *				which holds the 64-bit SCI_MASK that will be
 *				recorded in the SC map.
 *				Typically, this field will be written with all-ones
 * @param	sc_index	Secured Channel Index - should be the same as relevant
 *				SA address, but w/o the 1/2 LSBs which are used for
 *				AN (Association Number)
 */
void al_eth_macsec_sc_map_entry_write (struct al_hal_eth_adapter		*adapter,
					uint32_t				*sci,
					uint32_t				*sci_mask,
					uint8_t					sc_index);


/**
 * @brief	this function returns the index of the last TX SA which has been exhausted.
 *		It should be called upon receiving a tx_sa_exahusted IRQ.
 *		Upon receiving tx_sa_exhausted IRQ, upper layer should initiate
 *		key-negotiation, and load a new SA to replace the exhausted one.
 *
 * @param	adapter		Pointer to eth's private data-structure. The
 *				adapter is used in order to extract the base
 *				macsec configuration address.
 *
 * @returns	index of the exhausted SA.
 */
unsigned int al_eth_macsec_read_exhausted_tx_sa_index (struct al_hal_eth_adapter *adapter);


/**
 * @brief	this function returns the index of the last SA which has been exhausted.
 *		It should be called upon receiving a rx_sa_exahusted IRQ.
 *		Upon receiving rx_sa_exhausted IRQ, upper layer should initiate
 *		key-negotiation, and load a new SA to replace the exhausted one.
 *
 * @param	adapter		Pointer to eth's private data-structure. The
 *				adapter is used in order to extract the base
 *				macsec configuration address.
 *
 * @returns	index of the exhausted SA.
 */
unsigned int al_eth_macsec_read_exhausted_rx_sa_index (struct al_hal_eth_adapter *adapter);


/**
 * @brief	this function reads the MacSec engine's statistics counters
 *
 * @param	adapter		Pointer to eth's private data-structure. The
 *				adapter is used in order to extract the base
 *				macsec configuration address.
 * @param	stats_cntrs	a struct filled by the function with statistics
 *				counters values
 */
void al_eth_macsec_read_stats_cntr (struct al_hal_eth_adapter *adapter,
				   struct al_eth_macsec_stats_cntr *stats_cntrs);


#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
#endif		/* __AL_HAL_ETH_MACSEC_H__ */
/** @} end of Ethernet Macsec group */
