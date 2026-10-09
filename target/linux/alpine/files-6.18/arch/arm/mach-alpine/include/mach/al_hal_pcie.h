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
 * @defgroup grouppcie PCI Express Controller
 *  @{
 * @section overview Overview
 * This header file provide API for the HAL driver of the pcie port, the driver
 * provides the following functionalities:
 * - Port initialization
 * - Link operation
 * - Interrupts transactions generation (Endpoint mode).
 * - Configuration Access management functions
 * - Internal Translation Unit programming
 *
 * This API does not provide the following:
 * - PCIe transactions generation and reception (except interrupts as mentioned
 *   above) as this functionality is done by the port without need for sw
 *   intervention.
 * - Configuration Access: those transactions are generated automatically by
 *   the port (ECAM or ATU mode) when the CPU issues memory transaction
 *   through the fabric toward the PCIe port. This API provides management
 *   function for controlling the Configuration Access type and bus destination
 * - Interrupt Handling.
 * - Message Generation: common used messages are automatically generated, also,
 *   the ATU generic mechanism for generating various kind of messages.
 * - PCIe Port Management: both link and port power management features can be
 *   managed using the PCI/PCIe standard power management and PCIe capabilities
 *   registers.
 * - PCIe link and protocol error handling: the feature can be managed using
 *   the Advanced Error Handling PCIe capability registers.
 *
 * @section flows Software Flows
 * @subsection init Initialization
 *   - allocation and set zeros al_pcie_port structure handle
 *   - call al_pcie_handle_init() with pointer to the allocated al_pcie_port handle,
 *   address of the port internal registers space, and port id.
 *   - set the port mode, End-Point or Root-Compex (default).
 *   - set number of lanes connected to the controller.
 *   - enable the controller using the al_pcie_port_enable(). note that this function
 *     expect the virtual address of the PBS registers as first parameter.
 *   - wait for 2000 Southbridge cycles.
 *   - prepare al_pcie_config_params structure depending on chip, board and system
 *     configuration.
 *     for example, when using the port as root complex, the function_mode field
 *     should be set to AL_PCIE_FUNCTION_MODE_RC. In this example we prepare the
 *     following configuration:
 *     - Root Complex mode
 *     - Set the Max Link Speed to Gen2
 *     - Set the max lanes width to 2 (x2)
 *     - Disable reversal mode
 *     - Enable Snoops to support I/O Hardware cache coherency
 *     - Enable pcie core RAM parity
 *     - Enable pcie core AXI parity
 *     - Keep transaction layer default credits
 *     so the structures we prepare:
 *     @code
 *     - struct al_pcie_link_params link_params = { AL_PCIE_LINK_SPEED_GEN2,
 *     						    AL_FALSE}; // disable reversal mode
 *
 *     - struct al_pcie_config_params config_params = { AL_PCIE_FUNCTION_MODE_RC,
 *     							&link_params,
 *     							AL_TRUE, // enable Snoop for inbound memory transactions
 *     							AL_TRUE, // enable pcie port RAM parity
 *							AL_TRUE, // enable pcie port AXI parity
 *							NULL, // use default latency/replay timers
 *							NULL, // use default gen2 pipe params
 *     							NULL, // gen3_params not needed when max speed set to Gen2
 *							NULL, // don't change TL credits
 *							NULL, // end point params not needed
 *							AL_FALSE, //no fast link
 *							AL_FALSE  //return 0xFFFFFFFF for read transactions with pci target error
 *							}
 *	@endcode
 *	- now call al_pcie_port_config() with the handle and the config_params structure.
 * @subsection linkinit Link Initialization
 *  - once the port configured, we can start PCIe link:
 *  - call al_pcie_link_start()
 *  - call al_pcie_link_up_wait()
 *  - allocate al_pcie_link_status struct and call al_pcie_link_status() and
 *    check the link is established.
 *
 *  @subsection  cap Configuration Access Preparation
 *  - Once the link is established, we can prepare the port for pci
 *  configuration access, this stage requires system knowledge about the PCI
 *  buses enumeration. For example, if 5 buses were discovered on previously
 *  scanned root complex port, then we should start enumeration from bus 5 (PCI
 *  secondary bus), the sub-ordenary bus will be temporarily set to maximum
 *  value (255) until the scan process under this bus is finished, then it will
 *  updated to the maximum bus value found. So we use the following sequence:
 *  - call al_pcie_secondary_bus_set() with secbus = 5
 *  - call al_pcie_subordinary_bus_set() with subbus = 255
 *
 *  @subsection cfg Configuration (Cfg) Access Generation
 *  - we assume using ECAM method, in this method, the software issues pcie Cfg
 *  access by accessing the ECAM memory space of the pcie port. For example, to
 *  issue 4 byte Cfg Read from bus B, Device D, Function F and register R, the
 *  software issues 4 byte read access to the following physical address
 *  ECAM base address of the port + (B << 20) + (D << 15) + (F << 12) + R.
 *  But, as the default size of the ECAM address space is less than
 *  needed full range (256MB), we modify the target_bus value prior to Cfg
 *  access in order make the port generate Cfg access with bus value set to the
 *  value of the target_bus rather than bits 27:20 of the physical address.
 *  - call al_pcie_target_bus_set() with target_bus set to the required bus of
 *   the next Cfg access to be issued, mask_target_bus will be set to 0xff.
 *   no need to call that function if the next Cfg access bus equals to the last
 *   value set to target_bus.
 *
 *      @file  al_hal_pcie.h
 *      @brief HAL Driver Header for the Annapurna Labs PCI Express port.
 */

#ifndef _AL_HAL_PCIE_H_
#define _AL_HAL_PCIE_H_

#include <mach/al_hal_common.h>

/****************************** Constants ***********************************/
/** Inbound header credits sum */
#define AL_PCIE_IB_HCRD_SUM				97

/**
 * Minimal ratio between outstanding header completions and the number of
 * outstanding outbound reads
 * (max request size / cache line) + 1 = 256/64+1
 */
#define AL_PCIE_NOF_CPL_HDR_NOF_OS_OB_READS_MIN_RATIO	5

/** Maximal value for outstanding headers reads and header writes */
#define AL_PCIE_NOF_P_NP_HDR_MAX			24

/*********************** Data Structures and Types **************************/

/**
 * al_pcie_port: data structure used by the HAL to handle a specific pcie port.
 * this structure is allocated and set to zeros by the upper layer, then it is
 * initialized by the al_pcie_handle_init() that should be called before any
 * other function of this API. later, this handle passed to the API functions.
 */
struct al_pcie_port {
	struct al_pcie_regs __iomem *regs;

	/* Revision ID - valid after calling 'al_pcie_port_config' */
	uint8_t		rev_id;

	unsigned int	port_id;
	al_bool		write_to_read_only_enabled;
	uint8_t		max_lanes;

	/**
	 * Determine whether configuring 'nof_np_hdr' and 'nof_p_hdr' is
	 * required in the core
	 */
	al_bool		ib_hcrd_config_required;

	/* Internally used - see 'al_pcie_ib_hcrd_os_ob_reads_config' */
	unsigned int	nof_np_hdr;

	/* Internally used - see 'al_pcie_ib_hcrd_os_ob_reads_config' */
	unsigned int	nof_p_hdr;

	/* Internally used - see al_hal_pcie_interrupts.c */
	uint32_t __iomem *app_int_grp_a_base;
	uint32_t __iomem *app_int_grp_b_base;
	uint32_t __iomem *axi_int_grp_a_base;
};


/** Function mode (endpoint, root complex) */
enum al_pcie_function_mode {
	AL_PCIE_FUNCTION_MODE_EP,
	AL_PCIE_FUNCTION_MODE_RC,
	AL_PCIE_FUNCTION_MODE_UNKNOWN
};

/* The maximum link speed, measured GT/s (Giga transfer / second)
 *   DEFAULT: do not change the current speed
 *   GEN1: 2.5 GT/s
 *   GEN2: 5 GT/s
 *   GEN3: 8GT/s
 */
enum al_pcie_link_speed {
	AL_PCIE_LINK_SPEED_DEFAULT,
	AL_PCIE_LINK_SPEED_GEN1 = 1,
	AL_PCIE_LINK_SPEED_GEN2 = 2,
	AL_PCIE_LINK_SPEED_GEN3 = 3
};

/** PCIe capabilities that supported by a specific port */
struct al_pcie_max_capability {
	al_bool		end_point_mode_supported;
	al_bool		root_complex_mode_supported;
	enum al_pcie_link_speed	max_speed;
	uint8_t		max_lanes;
	al_bool		reversal_supported;
	uint8_t		atu_regions_num;
	uint32_t	atu_min_size;
};


/** PCIe link related parameters */
struct al_pcie_link_params {
	enum al_pcie_link_speed	max_speed;
	al_bool			enable_reversal;
};

/** PCIe gen2 link parameters */
struct al_pcie_gen2_params {
	al_bool	tx_swing_low; /* set tx swing low when true, and tx swing full when false */
	al_bool	tx_compliance_receive_enable;
	al_bool	set_deemphasis;
};

/** PCIe gen 3 standard per lane equalization parameters */
struct al_pcie_gen3_lane_eq_params {
	uint8_t		downstream_port_transmitter_preset;
	uint8_t		downstream_port_receiver_preset_hint;
	uint8_t		upstream_port_transmitter_preset;
	uint8_t		upstream_port_receiver_preset_hint;
};

/** PCIe gen 3 equalization parameters */
struct al_pcie_gen3_params {
	al_bool	perform_eq;
	al_bool	interrupt_enable_on_link_eq_request;
	struct al_pcie_gen3_lane_eq_params *eq_params; /* array of lanes params */
	int	eq_params_elements; /* number of elements in the eq_params array */

	al_bool	eq_disable; /* disables the equalization feature */
	al_bool eq_phase2_3_disable; /* Equalization Phase 2 and Phase 3 */
				     /* Disable (RC mode only) */
	uint8_t local_lf; /* Full Swing (FS) Value for Gen3 Transmit Equalization */
			  /* Value Range: 12 through 63 (decimal).*/

	uint8_t	local_fs; /* Low Frequency (LF) Value for Gen3 Transmit Equalization */
};

/** Transport Layer credits parameters */
struct al_pcie_tl_credits_params {
};

/** BAR register configuration parameters (Endpoint Mode only) */
struct al_pcie_ep_bar_params {
	al_bool		enable;
	al_bool		memory_space; /**< memory or io */
	al_bool		memory_64_bit; /**< is memory space is 64 bit */
	al_bool		memory_is_prefetchable;
	uint64_t	size; /* the bar size in bytes */
};

/** BARs configuration parameters (Endpoint Mode only) */
struct al_pcie_ep_params {
	al_bool				cap_d1_d3hot_dis;
	al_bool				cap_flr_dis;
	al_bool				cap_aspm_dis;
	al_bool				relaxed_pcie_ordering;
	al_bool				bar_params_valid;
	struct al_pcie_ep_bar_params	bar_params[6];
	struct al_pcie_ep_bar_params	exp_bar_params;/* expansion ROM BAR*/
};

/** Various configuration features */
struct al_pcie_features {
	/**
	 * Enable MSI fix from the SATA to the PCIe EP
	 * Only valid for port 0, when enabled as EP
	 */
	al_bool sata_ep_msi_fix;
};

/**
 * Inbound posted/non-posted header credits and outstanding outbound reads
 * completion header configuration
 *
 * Constraints:
 * - nof_cpl_hdr + nof_np_hdr + nof_p_hdr == AL_PCIE_IB_HCRD_SUM
 * - (nof_outstanding_ob_reads x AL_PCIE_NOF_CPL_HDR_NOF_OS_OB_READS_MIN_RATIO)
 *   <= nof_cpl_hdr
 * - nof_p_hdr <= AL_PCIE_NOF_P_NP_HDR_MAX
 * - nof_np_hdr <= AL_PCIE_NOF_P_NP_HDR_MAX
 * - nof_cpl_hdr > 0
 * - nof_p_hdr > 0
 * - nof_np_hdr > 0
 */
struct al_pcie_ib_hcrd_os_ob_reads_config {
	/** Max number of outstanding outbound reads */
	uint8_t nof_outstanding_ob_reads;

	/**
	 * This value set the possible outstanding headers CMPLs , the core
	 * can get (the core always advertise infinite credits for CMPLs).
	 */
	unsigned int nof_cpl_hdr;

	/**
	 * This value set the possible outstanding headers reads (non-posted
	 * transactions), the core can get  (it set the value in the init FC
	 * process).
	 */
	unsigned int nof_np_hdr;

	/**
	 * This value set the possible outstanding headers writes (posted
	 * transactions), the core can get  (it set the value in the init FC
	 * process).
	 */
	unsigned int nof_p_hdr;
};

/** I/O Virtualization support in EP configuration */
struct al_pcie_ep_iov_params {
	/**
	 * Enable multiple Virtual Functions support by propogating VMID to
	 * outbound requests
	 */

	al_bool sriov_vfunc_en;

	/**
	 * Fix client1 FMT bits after cutting address 63:56, fix address format
	 * to 32-bits if original request is 32-bit address.
	 */
	al_bool support_32b_address_in_iov;
};

/** PCIe Ack/Nak Latency and Replay timers */
struct al_pcie_latency_replay_timers {
	uint16_t	round_trip_lat_limit;
	uint16_t	replay_timer_limit;
};

/** PCIe port configuration parameters
 * This structure includes the parameters that the HAL should apply to the port
 * (by al_pcie_port_config()).
 * The fields that are pointers (e.g. link_params) can be set to NULL, in that
 * case, the al_pcie_port_config() will keep the current HW settings.
 */
struct al_pcie_config_params {
	enum al_pcie_function_mode	function_mode; /**< indicates at which mode the controller operates */
	struct al_pcie_link_params	*link_params;
	al_bool				enable_axi_snoop;
	al_bool				enable_ram_parity_int;
	al_bool				enable_axi_parity_int;
	struct al_pcie_latency_replay_timers *lat_rply_timers;
	struct al_pcie_gen2_params *gen2_params;
	struct al_pcie_gen3_params	*gen3_params;
	struct al_pcie_tl_credits_params	*tl_credits;
	struct al_pcie_ep_params	*ep_params;
	struct al_pcie_features		*features;
	struct al_pcie_ep_iov_params	*ep_iov_params;
	al_bool				fast_link_mode; /* Sets all internal timers to Fast Mode for speeding up simulation.*/
	al_bool                         enable_axi_slave_err_resp; /**< when true, the PCI unit will return Slave Error/Decoding Error to the master unit in case of error. when false, the value 0xFFFFFFFF will be returned without error indication. */
};

/** PCIe link status */
struct al_pcie_link_status {
	al_bool			link_up;
	enum al_pcie_link_speed	speed;
	uint8_t			lanes;
	uint8_t			ltssm_state;
};

/** PCIe MSIX capability configuration parameters */
struct al_pcie_msix_params {
	uint16_t	table_size;
	uint16_t	table_offset;
	uint8_t		table_bar;
	uint16_t	pba_offset;
	uint16_t	pba_bar;
};

/*********************** PCIe Port Initialization API **************/
/** Enable PCIe unit (deassert reset)
 *
 * @param   pcie_port pcie port handle
 * @param   pbs_reg_base the virtual base address of the pbs registers
 *
 * @return 0 if no error found.
 */
int al_pcie_port_enable(
	struct al_pcie_port	*pcie_port,
	void __iomem		*pbs_reg_base);

/** Disable PCIe unit (assert reset)
 *
 * @param   pcie_port pcie port handle
 * @param   pbs_reg_base the virtual base address of the pbs registers
 */
void al_pcie_port_disable(
	struct al_pcie_port	*pcie_port,
	void __iomem		*pbs_reg_base);

/**
 * Initializes a PCIe handle structure.
 *
 * @param   pcie_port an allocated, non-initialized instance.
 * @param   pcie_reg_base the virtual base address of the port internal registers
 * @param   port_id the port id (used mainly for debug messages)
 *
 * @return 0 if no error found.
 */
int al_pcie_handle_init(struct al_pcie_port *pcie_port,
			 void __iomem *pcie_reg_base,
			 unsigned int port_id);

/**
 * Configure number of lanes connected to this port.
 * This function can be called only before enabling the controller using al_pcie_port_enable().
 *
 * @param pcie_port pcie port handle
 * @param lanes number of lanes
 *
 * @return 0 if no error found.
 */
int al_pcie_port_max_lanes_set(struct al_pcie_port *pcie_port, uint8_t lanes);

/**
 * Port memory shutdown/up
 * This function can be called only when the controller is disabled
 *
 * @param pcie_port pcie port handle
 * @param enable memory shutdown enable or disable
 *
 */
void al_pcie_port_memory_shutdown_set(
	struct al_pcie_port	*pcie_port,
	al_bool			enable);

/**
 * @brief set current function mode (root complex or endpoint)
 * This function can be called only before enabling the controller using al_pcie_port_enable().
 *
 * @param pcie_port pcie port handle
 * @param mode pcie port mode
 *
 * @return 0 if no error found.
 */
int al_pcie_port_func_mode_config(struct al_pcie_port *pcie_port,
				  enum al_pcie_function_mode mode);

/**
 * @brief Inbound posted/non-posted header credits and outstanding outbound
 *        reads completion header configuration
 *
 * @param	pcie_port pcie port handle
 * @param	ib_hcrd_os_ob_reads_config
 * 		Inbound header credits and outstanding outbound reads
 * 		configuration
 */
void al_pcie_port_ib_hcrd_os_ob_reads_config(
	struct al_pcie_port *pcie_port,
	struct al_pcie_ib_hcrd_os_ob_reads_config *ib_hcrd_os_ob_reads_config);

/**
 * @brief return current function mode (root complex or endpoint)
 *
 * @param pcie_port pcie port handle
 *
 * @return pcie port current mode.
 */
enum al_pcie_function_mode
al_pcie_function_type_get(struct al_pcie_port *pcie_port);


/**
 * @brief   configure pcie port (mode, link params, etc..)
 * this function must be called before initializing the link
 *
 * @param pcie_port pcie port handle
 * @param params configuration structure.
 *
 * @return  0 if no error found
 */
int al_pcie_port_config(struct al_pcie_port *pcie_port,
			struct al_pcie_config_params *params);

/**
 * @brief Enable/disable deferring incoming configuration requests until
 * initialization is complete. When enabled, the core completes incoming
 * configuration requests with a Configuration Request Retry Status.
 * Other incoming Requests complete with Unsupported Request status.
 *
 * @param pcie_port pcie port handle
 * @param en enable/disable
 */
void al_pcie_app_req_retry_set(
	struct al_pcie_port	*pcie_port,
	al_bool			en);

/**
 * @brief   configure pcie port axi snoop
 *
 * @param pcie_port pcie port handle
 * @param enable_axi_snoop enable snoop.
 *
 * @return  0 if no error found
 */
int al_pcie_port_snoop_config(struct al_pcie_port *pcie_port,
				al_bool enable_axi_snoop);

/********************** PCIE Link Operations API ********************/
/**
 * @brief   start pcie link
 *
 * @param   pcie_port pcie port handle
 *
 * @return  0 if no error found
 */
int al_pcie_link_start(struct al_pcie_port *pcie_port);

/**
 * @brief   stop pcie link
 *
 * @param   pcie_port pcie port handle
 *
 * @return  0 if no error found
 */
int al_pcie_link_stop(struct al_pcie_port *pcie_port);

/**
 * @brief   wait for link up indication
 * this function waits for link up indication, it polls LTSSM state until link is ready
 *
 * @param   pcie_port pcie port handle
 * @param   timeout_ms maximum timeout in milli-seconds to wait for link up
 *
 * @return  0 if link up indication detected
 * 	    -ETIME if not.
 */
int al_pcie_link_up_wait(struct al_pcie_port *pcie_port, uint32_t timeout_ms);

/**
 * @brief   get link status
 *
 * @param   pcie_port pcie port handle
 * @param   status structure for link status
 *
 * @return  0 if no error found
 */
int al_pcie_link_status(struct al_pcie_port *pcie_port, struct al_pcie_link_status *status);

/**
 * @brief   trigger hot reset
 * this function triggers hot-reset, it doesn't wait for link re-establishment
 *
 * @param   pcie_port pcie port handle
 *
 * @return  0 if no error found
 */
int al_pcie_link_hot_reset(struct al_pcie_port *pcie_port);

/* TODO: check if this function needed */
int al_pcie_link_change_speed(struct al_pcie_port *pcie_port, enum al_pcie_link_speed new_speed);

/* TODO: check if this function needed */
int al_pcie_link_change_width(struct al_pcie_port *pcie_port, uint8_t width);


/* Configuration Space Access Through PCI-E_ECAM_Ext PASW (RC mode only) */

/**
 * @brief   set target_bus and mask_target_bus
 * @param   pcie_port pcie port handle
 * @param   target_bus
 * @param   mask_target_bus
 * @return  0 if no error found
 */
int al_pcie_target_bus_set(struct al_pcie_port *pcie_port,
			   uint8_t target_bus,
			   uint8_t mask_target_bus);

/**
 * @brief   get target_bus and mask_target_bus
 * @param   pcie_port pcie port handle
 * @param   target_bus
 * @param   mask_target_bus
 * @return  0 if no error found
 */
int al_pcie_target_bus_get(struct al_pcie_port *pcie_port,
			   uint8_t *target_bus,
			   uint8_t *mask_target_bus);

/**
 * Set secondary bus number
 *
 * @param pcie_port pcie port handle
 * @param secbus pci secondary bus number
 *
 * @return 0 if no error found.
 */
int al_pcie_secondary_bus_set(struct al_pcie_port *pcie_port, uint8_t secbus);

/**
 * Set subordinary bus number
 *
 * @param   pcie_port pcie port handle
 * @param   subbus the highest bus number of all of the buses that can be reached
 *		downstream of the PCIE instance.
 *
 * @return 0 if no error found.
 */
int al_pcie_subordinary_bus_set(struct al_pcie_port *pcie_port,uint8_t subbus);


/**
 * @brief   get base address of pci configuration space header
 * @param   pcie_port pcie port handle
 * @param   addr pointer for returned address;
 * @return  0 if no error found
 */
int al_pcie_config_space_get(struct al_pcie_port *pcie_port,
			     uint8_t __iomem **addr);

/**
 * Read data from the local configuration space
 *
 * @param	pcie_port
 *		PCIe port handle
 * @param	reg_offset
 *		Configuration space register offset
 *
 * @return	Read data
 */
uint32_t al_pcie_cfg_emul_local_cfg_space_read(
	struct al_pcie_port	*pcie_port,
	unsigned int	reg_offset);

/**
 * Write data to the local configuration space
 *
 * @param	pcie_port
 *		PCIe port handle
 * @param	reg_offset
 *		Configuration space register offset
 * @param	data
 *		Data to write
 * @param	ro
 *		Is a read-only register according to PCIe specification
 *
 */
void al_pcie_cfg_emul_local_cfg_space_write(
	struct al_pcie_port	*pcie_port,
	unsigned int	reg_offset,
	uint32_t	data,
	al_bool		ro);

/******************* Internal Address Translation Unit (ATU) *************/
enum al_pcie_atu_dir {
	al_pcie_atu_dir_outbound = 0,
	al_pcie_atu_dir_inbound = 1,
};

enum al_pcie_atu_tlp {
	AL_PCIE_TLP_TYPE_MEM = 0,
	AL_PCIE_TLP_TYPE_IO = 2,
	AL_PCIE_TLP_TYPE_CFG0 = 4,
	AL_PCIE_TLP_TYPE_CFG1 = 5,
	AL_PCIE_TLP_TYPE_MSG = 0x10,
	AL_PCIE_TLP_TYPE_RESERVED = 0x1f
};

struct al_pcie_atu_region {
	al_bool			enable;
	enum al_pcie_atu_dir	direction; /* outbound or inbound */
	uint8_t			index; /* region index */
	uint64_t		base_addr;
	uint64_t		limit; /* only bits [39:0] are valid given the Alpine PoC maximum physical address space */
	uint64_t		target_addr; /* the address that matches will be translated to this address + offset */
	al_bool			invert_matching;
	enum al_pcie_atu_tlp	tlp_type; /* pcie tlp type*/
	uint8_t			attr; /* pcie frame header attr field*/
	/* outbound specific params */
	uint8_t			msg_code; /* pcie message code */
	al_bool			cfg_shift_mode;
	/* inbound specific params*/
	uint8_t			bar_number;
	uint8_t			match_mode; /* BAR match mode, used in EP for MEM and IO tlps*/
	al_bool			enable_attr_match_mode;
	al_bool			enable_msg_match_mode;
};

/**
 * @brief   program internal ATU region entry
 * @param   pcie_port pcie port handle
 * @param   atu_region data structure that contains the region index and the translation parameters
 * @return
 */
int al_pcie_atu_region_set(struct al_pcie_port *pcie_port, struct al_pcie_atu_region *atu_region);

/**
 * @brief   Configure axi io bar. every hit to this bar will override size to 4 bytes.
 * @param   pcie_port pcie port handle
 * @param   start the first address of the memory
 * @param   end the last address of the memory
 * @return
 */
void al_pcie_axi_io_config(struct al_pcie_port *pcie_port, al_phys_addr_t start, al_phys_addr_t end);

/********************** Interrupt generation (Endpoint mode Only) ************/

enum al_pcie_legacy_int_type{
	AL_PCIE_LEGACY_INTA = 0,
	AL_PCIE_LEGACY_INTB,
	AL_PCIE_LEGACY_INTC,
	AL_PCIE_LEGACY_INTD
};

/**
 * @brief   generate INTx Assert/DeAssert Message
 * @param   pcie_port pcie port handle
 * @param   assert when true, Assert Message is sent.
 * @param   type of message (INTA, INTB, etc)
 * @return  0 if no error found
 */
int al_pcie_legacy_int_gen(struct al_pcie_port *pcie_port, al_bool assert,
			   enum al_pcie_legacy_int_type type /*A,B,..*/);

/**
 * @brief   generate MSI interrupt
 * @param   pcie_port pcie port handle
 * @param   vector the vector index to send interrupt for.
 * @return  0 if no error found
 */
int al_pcie_msi_int_gen(struct al_pcie_port *pcie_port, uint8_t vector);

/**
 * @brief   configure MSIX capability
 * @param   pcie_port pcie port handle
 * @param   msix_params MSIX capability configuration parameters
 * @return  0 if no error found
 */
int al_pcie_msix_config(
	struct al_pcie_port *pcie_port,
	struct al_pcie_msix_params *msix_params);

/**
 * @brief   check whether MSIX capability is enabled
 * @param   pcie_port pcie port handle
 * @return  AL_TRUE if MSIX capability is enabled, AL_FALSE otherwise
 */
al_bool al_pcie_msix_enabled(struct al_pcie_port *pcie_port);

/**
 * @brief   check whether MSIX capability is masked
 * @param   pcie_port pcie port handle
 * @return  AL_TRUE if MSIX capability is masked, AL_FALSE otherwise
 */
al_bool al_pcie_msix_masked(struct al_pcie_port *pcie_port);

/********************** Loopback mode (RC and Endpoint modes) ************/

/**
 * @brief   enter local pipe loopback mode
 *  This mode will connect the pipe RX signals to TX.
 *  no need to start link when using this mode.
 *  Gen3 equalization must be disabled before enabling this mode
 *  The caller must make sure the port is ready to accept the TLPs it sends to
 *  itself. for example, BARs should be initialized before sending memory TLPs.
 *
 * @param   pcie_port pcie port handle
 * @return  0 if no error found
 */
int al_pcie_local_pipe_loopback_enter(struct al_pcie_port *pcie_port);

/**
 * @brief   exit local pipe loopback mode
 *
 * @param   pcie_port pcie port handle
 * @return  0 if no error found
 */
int al_pcie_local_pipe_loopback_exit(struct al_pcie_port *pcie_port);

/**
 * @brief   enter master remote loopback mode
 *  No need to configure the link partner to enter slave remote loopback mode
 *  as this should be done as response to special training sequence directives
 *  when master works in remote loopback mode.
 *  The caller must make sure the port is ready to accept the TLPs it sends to
 *  itself. for example, BARs should be initialized before sending memory TLPs.
 *
 * @param   pcie_port pcie port handle
 * @return  0 if no error found
 */
int al_pcie_remote_loopback_enter(struct al_pcie_port *pcie_port);

/**
 * @brief   exit remote loopback mode
 *
 * @param   pcie_port pcie port handle
 * @return  0 if no error found
 */
int al_pcie_remote_loopback_exit(struct al_pcie_port *pcie_port);

#endif
/** @} end of grouppcie group */
