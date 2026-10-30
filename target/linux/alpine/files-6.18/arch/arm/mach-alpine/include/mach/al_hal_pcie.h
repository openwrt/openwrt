/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

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

	/* Internally used */
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

/** PCIe link status */
struct al_pcie_link_status {
	al_bool			link_up;
	enum al_pcie_link_speed	speed;
	uint8_t			lanes;
	uint8_t			ltssm_state;
};

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
 * @brief return current function mode (root complex or endpoint)
 *
 * @param pcie_port pcie port handle
 *
 * @return pcie port current mode.
 */
enum al_pcie_function_mode
al_pcie_function_type_get(struct al_pcie_port *pcie_port);

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

/**
 * @brief   get link status
 *
 * @param   pcie_port pcie port handle
 * @param   status structure for link status
 *
 * @return  0 if no error found
 */
int al_pcie_link_status(struct al_pcie_port *pcie_port, struct al_pcie_link_status *status);

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

/********************** Interrupt generation (Endpoint mode Only) ************/

/********************** Loopback mode (RC and Endpoint modes) ************/

#endif
/** @} end of grouppcie group */
