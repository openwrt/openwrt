/*****************************************************************************
 **   FILE NAME       : ifxusb_regs.h
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : This file contains the data structures for accessing the IFXUSB core
 **                     registers.
 **                     The application interfaces with the USB core by reading from and
 **                     writing to the Control and Status Register (CSR) space through the
 **                     AHB Slave interface. These registers are 32 bits wide, and the
 **                     addresses are 32-bit-block aligned.
 **                     CSRs are classified as follows:
 **                     - Core Global Registers
 **                     - Device Mode Registers
 **                     - Device Global Registers
 **                     - Device Endpoint Specific Registers
 **                     - Host Mode Registers
 **                     - Host Global Registers
 **                     - Host Port CSRs
 **                     - Host Channel Specific Registers
 **
 **                     Only the Core Global registers can be accessed in both Device and
 **                     Host modes. When the USB core is operating in one mode, either
 **                     Device or Host, the application must not access registers from the
 **                     other mode. When the core switches from one mode to another, the
 **                     registers in the new mode of operation must be reprogrammed as they
 **                     would be after a power-on reset.
 **   FUNCTIONS       :
 **   COMPILER        : gcc
 **   REFERENCE       : Synopsys DWC-OTG Driver 2.7
 **   COPYRIGHT       :  Copyright (c) 2010
 **                      LANTIQ DEUTSCHLAND GMBH,
 **                      Am Campeon 3, 85579 Neubiberg, Germany
 **
 **    This program is free software; you can redistribute it and/or modify
 **    it under the terms of the GNU General Public License as published by
 **    the Free Software Foundation; either version 2 of the License, or
 **    (at your option) any later version.
 **
 **  Version Control Section  **
 **   $Author$
 **   $Date$
 **   $Revisions$
 **   $Log$       Revision history
*****************************************************************************/
/******************************************************************************
** COPYRIGHT    :   Copyright (c) 2006
**      Infineon Technologies AG
**      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 10 NOV 2008  Wu Qi Ming      Initial Version, to comply with COC
*******************************************************************************/


/*
 * This file contains code fragments from Synopsys HS OTG Linux Software Driver.
 * For this code the following notice is applicable:
 *
 * ==========================================================================
 *
 * Synopsys HS OTG Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */


/*!
  \defgroup IFXUSB_CSR_DEFINITION Control and Status Register bit-map definition
  \ingroup IFXUSB_DRIVER_V3
   \brief Data structures for accessing the IFXUSB core registers.
          The application interfaces with the USB core by reading from and
          writing to the Control and Status Register (CSR) space through the
          AHB Slave interface. These registers are 32 bits wide, and the
          addresses are 32-bit-block aligned.
          CSRs are classified as follows:
           - Core Global Registers
           - Device Mode Registers
           - Device Global Registers
           - Device Endpoint Specific Registers
           - Host Mode Registers
           - Host Global Registers
           - Host Port CSRs
           - Host Channel Specific Registers

          Only the Core Global registers can be accessed in both Device andHost modes.
          When the USB core is operating in one mode, either Device or Host, the
          application must not access registers from the other mode. When the core
          switches from one mode to another, the registers in the new mode of operation
          must be reprogrammed as they would be after a power-on reset.
 */

/*!
  \defgroup IFXUSB_CSR_DEVICE_GLOBAL_REG Device Mode Registers
  \ingroup IFXUSB_CSR_DEFINITION
  \brief Bit-mapped structure to access Device Mode Global Registers
 */

/*!
  \defgroup IFXUSB_CSR_DEVICE_EP_REG Device Mode EP Registers
  \ingroup IFXUSB_CSR_DEFINITION
    \brief Bit-mapped structure to access Device Mode EP Registers
     There will be one set of endpoint registers per logical endpoint
     implemented.
     These registers are visible only in Device mode and must not be
     accessed in Host mode, as the results are unknown.
 */

/*!
  \defgroup IFXUSB_CSR_DEVICE_DMA_DESC Device mode scatter dma descriptor strusture
  \ingroup IFXUSB_CSR_DEFINITION
  \brief Bit-mapped structure to DMA descriptor
 */


/*!
  \defgroup IFXUSB_CSR_HOST_GLOBAL_REG Host Mode Registers
  \ingroup IFXUSB_CSR_DEFINITION
  \brief Bit-mapped structure to access Host Mode Global Registers
 */

/*!
  \defgroup IFXUSB_CSR_HOST_HC_REG Host Mode HC Registers
  \ingroup IFXUSB_CSR_DEFINITION
    \brief Bit-mapped structure to access Host Mode Host Channel Registers
     There will be one set of endpoint registers per host channel
     implemented.
     These registers are visible only in Host mode and must not be
     accessed in Device mode, as the results are unknown.
 */

/*!
  \defgroup IFXUSB_CSR_PWR_CLK_GATING_REG Power and Clock Gating Control Register
  \ingroup IFXUSB_CSR_DEFINITION
  \brief Bit-mapped structure to Power and Clock Gating Control Register
 */

/*!
  \defgroup IFXUSB_CSR_CORE_GLOBAL_REG Core Global Registers
  \ingroup IFXUSB_CSR_DEFINITION
  \brief Bit-mapped structure to access Core Global Registers
 */

/*!
  \defgroup IFXUSB_CSR_CORE_GLOBAL_REG Core Global Registers
  \ingroup IFXUSB_CSR_DEFINITION
  \brief Bit-mapped structure to access Core Global Registers
 */



/*!
  \defgroup IFXUSB_CSR_ACCESS_MACROS Macros to manipulate CSRs
  \ingroup IFXUSB_CSR_DEFINITION
  \brief Macros to manipulate CSRs
 */






/*!
  \file ifxusb_regs.h
  \ingroup IFXUSB_DRIVER_V3
  \brief This file contains the data structures for accessing the IFXUSB core registers.
 */


#ifndef __IFXUSB_REGS_H__
#define __IFXUSB_REGS_H__

/****************************************************************************/

#define MAX_PERIO_FIFOS  15  /** Maximum number of Periodic FIFOs */
#define MAX_TX_FIFOS     15  /** Maximum number of Periodic FIFOs */
#define MAX_EPS_CHANNELS 16  /** Maximum number of Endpoints/HostChannels */

/****************************************************************************/

//#define __RecordRegRW__

/*!
   \fn    static __inline__ uint32_t ifxusb_rreg( volatile uint32_t *_reg)
   \brief Reads the content of a register.
   \param  _reg address of register to read.
   \return contents of the register.
   \ingroup IFXUSB_CSR_ACCESS_MACROS
 */
static __inline__ uint32_t ifxusb_rreg( volatile uint32_t *_reg)
{
	#ifdef __RecordRegRW__
		uint32_t r;
		r=*(_reg);
		return (r);
	#else
		return (*(_reg));
	#endif
};


/*!
   \fn    static __inline__ void ifxusb_wreg( volatile uint32_t *_reg, const uint32_t _value)
   \brief Writes a register with a 32 bit value.
   \param _reg   address of register to write.
   \param _value value to write to _reg.
   \ingroup IFXUSB_CSR_ACCESS_MACROS
 */
static __inline__ void ifxusb_wreg( volatile uint32_t *_reg, const uint32_t _value)
{
	#ifdef __RecordRegRW__
		printk(KERN_INFO "[W %p<-%08X]\n",_reg,_value);
	#else
		*(_reg)=_value;
	#endif
};

/*!
   \fn    static __inline__ void ifxusb_mreg( volatile uint32_t *_reg, const uint32_t _clear_mask, const uint32_t _set_mask)
   \brief Modifies bit values in a register.  Using the
          algorithm: (reg_contents & ~clear_mask) | set_mask.
   \param _reg        address of register to modify.
   \param _clear_mask bit mask to be cleared.
   \param _set_mask   bit mask to be set.
   \ingroup IFXUSB_CSR_ACCESS_MACROS
 */
static __inline__ void ifxusb_mreg( volatile uint32_t *_reg, const uint32_t _clear_mask, const uint32_t _set_mask)
{
	uint32_t v;
	#ifdef __RecordRegRW__
		uint32_t r;
		v=  *(_reg);
		r=v;
		r&=(~_clear_mask);
		r|= _set_mask;
		*(_reg)=r ;
		printk(KERN_INFO "[M %p->%08X+%08X/%08X<-%08X]\n",_reg,r,_clear_mask,_set_mask,r);
	#else
		v=  *(_reg);
		v&=(~_clear_mask);
		v|= _set_mask;
		*(_reg)=v ;
	#endif
};

/****************************************************************************/

/*!
  \addtogroup IFXUSB_CSR_CORE_GLOBAL_REG
 */
/*@{*/

/*! typedef ifxusb_core_global_regs_t
 \brief IFXUSB Core registers .
         The ifxusb_core_global_regs structure defines the size
         and relative field offsets for the Core Global registers.
 */
typedef struct ifxusb_core_global_regs
{
	volatile uint32_t gotgctl;             /*!< 000h OTG Control and Status Register. */
	volatile uint32_t gotgint;             /*!< 004h OTG Interrupt Register. */
	volatile uint32_t gahbcfg;             /*!< 008h Core AHB Configuration Register. */
	volatile uint32_t gusbcfg;             /*!< 00Ch Core USB Configuration Register. */
	volatile uint32_t grstctl;             /*!< 010h Core Reset Register. */
	volatile uint32_t gintsts;             /*!< 014h Core Interrupt Register. */
	volatile uint32_t gintmsk;             /*!< 018h Core Interrupt Mask Register. */
	volatile uint32_t grxstsr;             /*!< 01Ch Receive Status Queue Read Register (Read Only). */
	volatile uint32_t grxstsp;             /*!< 020h Receive Status Queue Read & POP Register (Read Only). */
	volatile uint32_t grxfsiz;             /*!< 024h Receive FIFO Size Register. */
	volatile uint32_t gnptxfsiz;           /*!< 028h Non Periodic Transmit FIFO Size Register. */
	volatile uint32_t gnptxsts;            /*!< 02Ch Non Periodic Transmit FIFO/Queue Status Register (Read Only). */
	volatile uint32_t gi2cctl;             /*!< 030h I2C Access Register. */
	volatile uint32_t gpvndctl;            /*!< 034h PHY Vendor Control Register. */
	volatile uint32_t ggpio;               /*!< 038h General Purpose Input/Output Register. */
	volatile uint32_t guid;                /*!< 03Ch User ID Register. */
	volatile uint32_t gsnpsid;             /*!< 040h Synopsys ID Register (Read Only). */
	volatile uint32_t ghwcfg1;             /*!< 044h User HW Config1 Register (Read Only). */
	volatile uint32_t ghwcfg2;             /*!< 048h User HW Config2 Register (Read Only). */
	volatile uint32_t ghwcfg3;             /*!< 04Ch User HW Config3 Register (Read Only). */
	volatile uint32_t ghwcfg4;             /*!< 050h User HW Config4 Register (Read Only). */
	volatile uint32_t reserved[43];        /*!< 054h Reserved  054h-0FFh */
	volatile uint32_t hptxfsiz;            /*!< 100h Host Periodic Transmit FIFO Size Register. */
	volatile uint32_t dptxfsiz_dieptxf[15];/*!< 104h + (FIFO_Number-1)*04h, 1 <= FIFO Number <= 15.
	                                           Device Periodic Transmit FIFO#n Register if dedicated
	                                           fifos are disabled, otherwise Device Transmit FIFO#n
	                                           Register.
	                                         */
} ifxusb_core_global_regs_t;

/*!
 \brief Bits of the Core OTG Control and Status Register (GOTGCTL).
 */
typedef union gotgctl_data
{
	uint32_t d32;
	struct{
		unsigned reserved21_31 : 11;
		unsigned currmod       : 1 ; /*!< 20 */
		unsigned bsesvld       : 1 ; /*!< 19 */
		unsigned asesvld       : 1 ; /*!< 18 */
		unsigned reserved17    : 1 ;
		unsigned conidsts      : 1 ; /*!< 16 */
		unsigned reserved12_15 : 4 ;
		unsigned devhnpen      : 1 ; /*!< 11 */
		unsigned hstsethnpen   : 1 ; /*!< 10 */
		unsigned hnpreq        : 1 ; /*!< 09 */
		unsigned hstnegscs     : 1 ; /*!< 08 */
		unsigned reserved2_7   : 6 ;
		unsigned sesreq        : 1 ; /*!< 01 */
		unsigned sesreqscs     : 1 ; /*!< 00 */
	} b;
} gotgctl_data_t;

/*!
 \brief Bit fields of the Core OTG Interrupt Register (GOTGINT).
 */
typedef union gotgint_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved31_20     : 12;
		unsigned debdone           : 1 ; /*!< 19 Debounce Done */
		unsigned adevtoutchng      : 1 ; /*!< 18 A-Device Timeout Change */
		unsigned hstnegdet         : 1 ; /*!< 17 Host Negotiation Detected */
		unsigned reserver10_16     : 7 ;
		unsigned hstnegsucstschng  : 1 ; /*!< 09 Host Negotiation Success Status Change */
		unsigned sesreqsucstschng  : 1 ; /*!< 08 Session Request Success Status Change */
		unsigned reserved3_7       : 5 ;
		unsigned sesenddet         : 1 ; /*!< 02 Session End Detected */
		unsigned reserved0_1       : 2 ;
	} b;
} gotgint_data_t;

/*!
 \brief Bit fields of the Core AHB Configuration Register (GAHBCFG).
 */
typedef union gahbcfg_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved9_31      : 23;
		unsigned ptxfemplvl        : 1 ; /*!< 08    Periodic FIFO empty level trigger condition*/
		unsigned nptxfemplvl       : 1 ; /*!< 07    Non-Periodic FIFO empty level trigger condition*/
			#define IFXUSB_GAHBCFG_TXFEMPTYLVL_EMPTY     1
			#define IFXUSB_GAHBCFG_TXFEMPTYLVL_HALFEMPTY 0
		unsigned reserved          : 1 ;
		unsigned dmaenable         : 1 ; /*!< 05    DMA enable*/
			#define IFXUSB_GAHBCFG_DMAENABLE             1
		unsigned hburstlen         : 4 ; /*!< 01-04 DMA Burst-length*/
			#define IFXUSB_GAHBCFG_INT_DMA_BURST_SINGLE  0
			#define IFXUSB_GAHBCFG_INT_DMA_BURST_INCR    1
			#define IFXUSB_GAHBCFG_INT_DMA_BURST_INCR4   3
			#define IFXUSB_GAHBCFG_INT_DMA_BURST_INCR8   5
			#define IFXUSB_GAHBCFG_INT_DMA_BURST_INCR16  7
		unsigned glblintrmsk       : 1 ;  /*!< 00    USB Global Interrupt Enable */
			#define IFXUSB_GAHBCFG_GLBINT_ENABLE         1
	} b;
} gahbcfg_data_t;

/*!
 \brief Bit fields of the Core USB Configuration Register (GUSBCFG).
*/
typedef union gusbcfg_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved31              : 1;
		unsigned ForceDevMode            : 1; /*!< 30 Force Device Mode */
		unsigned ForceHstMode            : 1; /*!< 29 Force Host Mode */
		unsigned TxEndDelay              : 1; /*!< 28 Tx End Delay */
		unsigned reserved2723            : 5;
		unsigned term_sel_dl_pulse       : 1; /*!< 22 TermSel DLine Pulsing Selection */
		unsigned reserved2117            : 5;
		unsigned otgutmifssel            : 1; /*!< 16 UTMIFS Select */
		unsigned phylpwrclksel           : 1; /*!< 15 PHY Low-Power Clock Select */
		unsigned reserved14              : 1;
		unsigned usbtrdtim               : 4; /*!< 13-10 USB Turnaround Time */
		unsigned hnpcap                  : 1; /*!< 09 HNP-Capable */
		unsigned srpcap                  : 1; /*!< 08 SRP-Capable */
		unsigned reserved07              : 1;
		unsigned physel                  : 1; /*!< 06 USB 2.0 High-Speed PHY or
		                                             USB 1.1 Full-Speed Serial
		                                             Transceiver Select */
		unsigned fsintf                  : 1; /*!< 05 Full-Speed Serial Interface Select */
		unsigned ulpi_utmi_sel           : 1; /*!< 04 ULPI or UTMI+ Select */
		unsigned phyif                   : 1; /*!< 03 PHY Interface */
		unsigned toutcal                 : 3; /*!< 00-02 HS/FS Timeout Calibration */
	}b;
} gusbcfg_data_t;

/*!
 \brief Bit fields of the Core Reset Register (GRSTCTL).
 */
typedef union grstctl_data
{
	uint32_t d32;
	struct
	{
		unsigned ahbidle         : 1; /*!< 31 AHB Master Idle.  Indicates the AHB Master State
		                                     Machine is in IDLE condition. */
		unsigned dmareq          : 1; /*!< 30 DMA Request Signal.  Indicated DMA request is in
		                                     probress.  Used for debug purpose. */
		unsigned reserved11_29   :19;
		unsigned txfnum          : 5; /*!< 10-06 TxFIFO Number (TxFNum) to be flushed.
		                                  0x00: Non Periodic TxFIFO Flush or TxFIFO 0
		                                  0x01-0x0F: Periodic TxFIFO Flush or TxFIFO n
		                                  0x10: Flush all TxFIFO
		                               */
		unsigned txfflsh         : 1; /*!< 05 TxFIFO Flush */
		unsigned rxfflsh         : 1; /*!< 04 RxFIFO Flush */
		unsigned intknqflsh      : 1; /*!< 03 In Token Sequence Learning Queue Flush (Device Only) */
		unsigned hstfrm          : 1; /*!< 02 Host Frame Counter Reset (Host Only) */
		unsigned hsftrst         : 1; /*!< 01 Hclk Soft Reset */

		unsigned csftrst         : 1; /*!< 00 Core Soft Reset
		                                     The application can flush the control logic in the
		                                     entire core using this bit. This bit resets the
		                                     pipelines in the AHB Clock domain as well as the
		                                     PHY Clock domain.
		                                     The state machines are reset to an IDLE state, the
		                                     control bits in the CSRs are cleared, all the
		                                     transmit FIFOs and the receive FIFO are flushed.
		                                     The status mask bits that control the generation of
		                                     the interrupt, are cleared, to clear the
		                                     interrupt. The interrupt status bits are not
		                                     cleared, so the application can get the status of
		                                     any events that occurred in the core after it has
		                                     set this bit.
		                                     Any transactions on the AHB are terminated as soon
		                                     as possible following the protocol. Any
		                                     transactions on the USB are terminated immediately.
		                                     The configuration settings in the CSRs are
		                                     unchanged, so the software doesn't have to
		                                     reprogram these registers (Device
		                                     Configuration/Host Configuration/Core System
		                                     Configuration/Core PHY Configuration).
		                                     The application can write to this bit, any time it
		                                     wants to reset the core. This is a self clearing
		                                     bit and the core clears this bit after all the
		                                     necessary logic is reset in the core, which may
		                                     take several clocks, depending on the current state
		                                     of the core.
		                               */
	}b;
} grstctl_t;

/*!
 \brief Bit fields of the Core Interrupt Mask Register (GINTMSK) and
        Core Interrupt Register (GINTSTS).
 */
typedef union gint_data
{
	uint32_t d32;
		#define IFXUSB_SOF_INTR_MASK 0x0008
	struct
	{
		unsigned wkupintr      : 1; /*!< 31 Resume/Remote Wakeup Detected Interrupt */
		unsigned sessreqintr   : 1; /*!< 30 Session Request/New Session Detected Interrupt */
		unsigned disconnect    : 1; /*!< 29 Disconnect Detected Interrupt */
		unsigned conidstschng  : 1; /*!< 28 Connector ID Status Change */
		unsigned reserved27    : 1;
		unsigned ptxfempty     : 1; /*!< 26 Periodic TxFIFO Empty */
		unsigned hcintr        : 1; /*!< 25 Host Channels Interrupt */
		unsigned portintr      : 1; /*!< 24 Host Port Interrupt */
		unsigned reserved23    : 1;
		unsigned fetsuspmsk    : 1; /*!< 22 Data Fetch Suspended */
		unsigned incomplisoout : 1; /*!< 21 Incomplete IsochronousOUT/Period Transfer */
		unsigned incomplisoin  : 1; /*!< 20 Incomplete Isochronous IN Transfer */
		unsigned outepintr     : 1; /*!< 19 OUT Endpoints Interrupt */
		unsigned inepintr      : 1; /*!< 18 IN Endpoints Interrupt */
		unsigned epmismatch    : 1; /*!< 17 Endpoint Mismatch Interrupt */
		unsigned reserved16    : 1;
		unsigned eopframe      : 1; /*!< 15 End of Periodic Frame Interrupt */
		unsigned isooutdrop    : 1; /*!< 14 Isochronous OUT Packet Dropped Interrupt */
		unsigned enumdone      : 1; /*!< 13 Enumeration Done */
		unsigned usbreset      : 1; /*!< 12 USB Reset */
		unsigned usbsuspend    : 1; /*!< 11 USB Suspend */
		unsigned erlysuspend   : 1; /*!< 10 Early Suspend */
		unsigned i2cintr       : 1; /*!< 09 I2C Interrupt */
		unsigned reserved8     : 1;
		unsigned goutnakeff    : 1; /*!< 07 Global OUT NAK Effective */
		unsigned ginnakeff     : 1; /*!< 06 Global Non-periodic IN NAK Effective */
		unsigned nptxfempty    : 1; /*!< 05 Non-periodic TxFIFO Empty */
		unsigned rxstsqlvl     : 1; /*!< 04 Receive FIFO Non-Empty */
		unsigned sofintr       : 1; /*!< 03 Start of (u)Frame */
		unsigned otgintr       : 1; /*!< 02 OTG Interrupt */
		unsigned modemismatch  : 1; /*!< 01 Mode Mismatch Interrupt */
		unsigned reserved0     : 1;
	} b;
} gint_data_t;

/*!
  \brief Bit fields in the Receive Status Read and Pop Registers (GRXSTSR, GRXSTSP)
 */
typedef union grxsts_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved : 7;
		unsigned fn       : 4; /*!< 24-21 Frame Number */
		unsigned pktsts   : 4; /*!< 20-17 Packet Status */
			#define IFXUSB_DSTS_DATA_UPDT  	0x2               // OUT Data Packet
			#define IFXUSB_DSTS_XFER_COMP  	0x3               // OUT Data Transfer Complete
			#define IFXUSB_DSTS_GOUT_NAK   	0x1               // Global OUT NAK
			#define IFXUSB_DSTS_SETUP_COMP 	0x4               // Setup Phase Complete
			#define IFXUSB_DSTS_SETUP_UPDT	0x6               // SETUP Packet
		unsigned dpid     : 2; /*!< 16-15 Data PID */
		unsigned bcnt     :11; /*!< 14-04 Byte Count */
		unsigned epnum    : 4; /*!< 03-00 Endpoint Number */
	} db;
	struct
	{
		unsigned reserved :11;
		unsigned pktsts   : 4; /*!< 20-17 Packet Status */
			#define IFXUSB_HSTS_DATA_UPDT        0x2 // OUT Data Packet
			#define IFXUSB_HSTS_XFER_COMP        0x3 // OUT Data Transfer Complete
			#define IFXUSB_HSTS_DATA_TOGGLE_ERR  0x5 // DATA TOGGLE Error
			#define IFXUSB_HSTS_CH_HALTED        0x7 // Channel Halted
		unsigned dpid     : 2; /*!< 16-15 Data PID */
		unsigned bcnt     :11; /*!< 14-04 Byte Count */
		unsigned chnum    : 4; /*!< 03-00 Channel Number */
	} hb;
} grxsts_data_t;

/*!
  \brief Bit fields in the FIFO Size Registers (HPTXFSIZ, GNPTXFSIZ, DPTXFSIZn).
 */
typedef union fifosize_data
{
	uint32_t d32;
	struct
	{
		unsigned depth     : 16; /*!< 31-16 TxFIFO Depth (in DWord)*/
		unsigned startaddr : 16; /*!< 15-00 RAM Starting address */
	} b;
} fifosize_data_t;

/*!
  \brief Bit fields in the Non-Periodic Transmit FIFO/Queue Status Register (GNPTXSTS).
 */

typedef union gnptxsts_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved           : 1;
		unsigned nptxqtop_chnep     : 4; /*!< 30-27 Channel/EP Number of top of the Non-Periodic
		                                     Transmit Request Queue
		                                  */
		unsigned nptxqtop_token     : 2; /*!< 26-25 Token Type top of the Non-Periodic
		                                     Transmit Request Queue
	                                          0 - IN/OUT
	                                          1 - Zero Length OUT
	                                          2 - PING/Complete Split
	                                          3 - Channel Halt
		                                  */
		unsigned nptxqtop_terminate : 1; /*!< 24    Terminate (Last entry for the selected
		                                           channel/EP)*/
		unsigned nptxqspcavail      : 8; /*!< 23-16 Transmit Request Queue Space Available */
		unsigned nptxfspcavail      :16; /*!< 15-00 TxFIFO Space Avail (in DWord)*/
	}b;
} gnptxsts_data_t;


/*!
  \brief Bit fields in the Transmit FIFO Status Register (DTXFSTS).
 */
typedef union dtxfsts_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved    : 16;
		unsigned txfspcavail : 16; /*!< 15-00 TxFIFO Space Avail (in DWord)*/
	}b;
} dtxfsts_data_t;


/*!
  \brief Bit fields in the I2C Control Register (I2CCTL).
 */
typedef union gi2cctl_data
{
	uint32_t d32;
	struct
	{
		unsigned bsydne     : 1; /*!< 31    I2C Busy/Done*/
		unsigned rw         : 1; /*!< 30    Read/Write Indicator */
		unsigned reserved   : 2;
		unsigned i2cdevaddr : 2; /*!< 27-26 I2C Device Address */
		unsigned i2csuspctl : 1; /*!< 25    I2C Suspend Control */
		unsigned ack        : 1; /*!< 24    I2C ACK */
		unsigned i2cen      : 1; /*!< 23    I2C Enable */
		unsigned addr       : 7; /*!< 22-16 I2C Address */
		unsigned regaddr    : 8; /*!< 15-08 I2C Register Addr */
		unsigned rwdata     : 8; /*!< I2C Read/Write Data */
	} b;
} gi2cctl_data_t;


/*!
  \brief Bit fields in the User HW Config1 Register.
 */
typedef union hwcfg1_data
{
	uint32_t d32;
	struct
	{
		unsigned ep_dir15 : 2; /*!< Direction of each EP
		                           0: BIDIR (IN and OUT) endpoint
	                               1: IN endpoint
	                               2: OUT endpoint
	                               3: Reserved
	                            */
		unsigned ep_dir14 : 2;
		unsigned ep_dir13 : 2;
		unsigned ep_dir12 : 2;
		unsigned ep_dir11 : 2;
		unsigned ep_dir10 : 2;
		unsigned ep_dir09 : 2;
		unsigned ep_dir08 : 2;
		unsigned ep_dir07 : 2;
		unsigned ep_dir06 : 2;
		unsigned ep_dir05 : 2;
		unsigned ep_dir04 : 2;
		unsigned ep_dir03 : 2;
		unsigned ep_dir02 : 2;
		unsigned ep_dir01 : 2;
		unsigned ep_dir00 : 2;
	}b;
} hwcfg1_data_t;

/*!
  \brief Bit fields in the User HW Config2 Register.
 */
typedef union hwcfg2_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved31             : 1;
		unsigned dev_token_q_depth      : 5; /*!< 30-26 Device Mode IN Token Sequence Learning Queue Depth */
		unsigned host_perio_tx_q_depth  : 2; /*!< 25-24 Host Mode Periodic Request Queue Depth */
		unsigned nonperio_tx_q_depth    : 2; /*!< 23-22 Non-periodic Request Queue Depth */
		unsigned rx_status_q_depth      : 2; /*!< 21-20 Multi Processor Interrupt Enabled */
		unsigned dynamic_fifo           : 1; /*!< 19    Dynamic FIFO Sizing Enabled */
		unsigned perio_ep_supported     : 1; /*!< 18    Periodic OUT Channels Supported in Host Mode */
		unsigned num_host_chan          : 4; /*!< 17-14 Number of Host Channels */
		unsigned num_dev_ep             : 4; /*!< 13-10 Number of Device Endpoints */
		unsigned fs_phy_type            : 2; /*!< 09-08 Full-Speed PHY Interface Type */
			#define IFXUSB_HWCFG2_FS_PHY_TYPE_NOT_SUPPORTED 0
			#define IFXUSB_HWCFG2_FS_PHY_TYPE_DEDICATE      1
			#define IFXUSB_HWCFG2_FS_PHY_TYPE_UTMI          2
			#define IFXUSB_HWCFG2_FS_PHY_TYPE_ULPI          3
		unsigned hs_phy_type            : 2; /*!< 07-06 High-Speed PHY Interface Type */
			#define IFXUSB_HWCFG2_HS_PHY_TYPE_NOT_SUPPORTED 0
			#define IFXUSB_HWCFG2_HS_PHY_TYPE_UTMI          1
			#define IFXUSB_HWCFG2_HS_PHY_TYPE_ULPI          2
			#define IFXUSB_HWCFG2_HS_PHY_TYPE_UTMI_ULPI     3
		unsigned point2point            : 1; /*!< 05    Point-to-Point */
		unsigned architecture           : 2; /*!< 04-03 Architecture */
			#define IFXUSB_HWCFG2_ARCH_SLAVE_ONLY  0
			#define IFXUSB_HWCFG2_ARCH_EXT_DMA     1
			#define IFXUSB_HWCFG2_ARCH_INT_DMA     2
		unsigned op_mode                : 3; /*!< 02-00 Mode of Operation */
			#define IFXUSB_HWCFG2_OP_MODE_HNP_SRP_CAPABLE_OTG    0
			#define IFXUSB_HWCFG2_OP_MODE_SRP_ONLY_CAPABLE_OTG   1
			#define IFXUSB_HWCFG2_OP_MODE_NO_HNP_SRP_CAPABLE_OTG 2
			#define IFXUSB_HWCFG2_OP_MODE_SRP_CAPABLE_DEVICE     3
			#define IFXUSB_HWCFG2_OP_MODE_NO_SRP_CAPABLE_DEVICE  4
			#define IFXUSB_HWCFG2_OP_MODE_SRP_CAPABLE_HOST       5
			#define IFXUSB_HWCFG2_OP_MODE_NO_SRP_CAPABLE_HOST    6
	} b;
} hwcfg2_data_t;

/*!
  \brief Bit fields in the User HW Config3 Register.
 */
typedef union hwcfg3_data
{
	uint32_t d32;
	struct
	{
		unsigned dfifo_depth            :16; /*!< 31-16 DFIFO Depth  */
		unsigned reserved15_12          : 4;
		unsigned synch_reset_type       : 1; /*!< 11    Reset Style for Clocked always Blocks in RTL */
		unsigned optional_features      : 1; /*!< 10    Optional Features Removed */
		unsigned vendor_ctrl_if         : 1; /*!< 09    Vendor Control Interface Support */
		unsigned i2c                    : 1; /*!< 08    I2C Selection */
		unsigned otg_func               : 1; /*!< 07    OTG Function Enabled */
		unsigned packet_size_cntr_width : 3; /*!< 06-04 Width of Packet Size Counters */
		unsigned xfer_size_cntr_width   : 4; /*!< 03-00 Width of Transfer Size Counters */
	} b;
} hwcfg3_data_t;

/*!
  \brief Bit fields in the User HW Config4
 * Register.  Read the register into the <i>d32</i> element then read
 * out the bits using the <i>b</i>it elements.
 */
typedef union hwcfg4_data
{
	uint32_t d32;
	struct
	{
		unsigned desc_dma_dyn         : 1; /*!< 31    Scatter/Gather DMA */
		unsigned desc_dma             : 1; /*!< 30    Scatter/Gather DMA configuration */
		unsigned num_in_eps           : 4; /*!< 29-26 Number of Device Mode IN Endpoints Including Control Endpoints */
		unsigned ded_fifo_en          : 1; /*!< 25    Enable Dedicated Transmit FIFO for device IN Endpoints */
		unsigned session_end_filt_en  : 1; /*!< 24    session_end Filter Enabled */
		unsigned b_valid_filt_en      : 1; /*!< 23    b_valid Filter Enabled */
		unsigned a_valid_filt_en      : 1; /*!< 22    a_valid Filter Enabled */
		unsigned vbus_valid_filt_en   : 1; /*!< 21    vbus_valid Filter Enabled */
		unsigned iddig_filt_en        : 1; /*!< 20    iddig Filter Enable */
		unsigned num_dev_mode_ctrl_ep : 4; /*!< 19-16 Number of Device Mode Control Endpoints in Addition to Endpoint 0 */
		unsigned utmi_phy_data_width  : 2; /*!< 15-14 UTMI+ PHY/ULPI-to-Internal UTMI+ Wrapper Data Width */
		unsigned reserved13_06        : 8;
		unsigned min_ahb_freq         : 1; /*!< 05    Minimum AHB Frequency Less Than 60 MHz */
		unsigned power_optimiz        : 1; /*!< 04    Enable Power Optimization? */
		unsigned num_dev_perio_in_ep  : 4; /*!< 03-00 Number of Device Mode Periodic IN Endpoints */
	} b;
} hwcfg4_data_t;

/*@}*//*IFXUSB_CSR_CORE_GLOBAL_REG*/

/****************************************************************************/
/*!
  \addtogroup IFXUSB_CSR_DEVICE_GLOBAL_REG
 */
/*@{*/

/*! typedef ifxusb_dev_global_regs_t
 \brief IFXUSB Device Mode Global registers. Offsets 800h-BFFh
        The ifxusb_dev_global_regs structure defines the size
        and relative field offsets for the Device Global registers.
        These registers are visible only in Device mode and must not be
        accessed in Host mode, as the results are unknown.
 */
typedef struct ifxusb_dev_global_regs
{
	volatile uint32_t dcfg;                 /*!< 800h Device Configuration Register. */
	volatile uint32_t dctl;                 /*!< 804h Device Control Register. */
	volatile uint32_t dsts;                 /*!< 808h Device Status Register (Read Only). */
	uint32_t unused;
	volatile uint32_t diepmsk;              /*!< 810h Device IN Endpoint Common Interrupt Mask Register. */
	volatile uint32_t doepmsk;              /*!< 814h Device OUT Endpoint Common Interrupt Mask Register. */
	volatile uint32_t daint;                /*!< 818h Device All Endpoints Interrupt Register. */
	volatile uint32_t daintmsk;             /*!< 81Ch Device All Endpoints Interrupt Mask Register. */
	volatile uint32_t dtknqr1;              /*!< 820h Device IN Token Queue Read Register-1 (Read Only). */
	volatile uint32_t dtknqr2;              /*!< 824h Device IN Token Queue Read Register-2 (Read Only). */
	volatile uint32_t dvbusdis;             /*!< 828h Device VBUS discharge Register.*/
	volatile uint32_t dvbuspulse;           /*!< 82Ch Device VBUS Pulse Register. */
	volatile uint32_t dtknqr3_dthrctl;      /*!< 830h Device IN Token Queue Read Register-3 (Read Only).
	                                                 Device Thresholding control register (Read/Write)
	                                         */
	volatile uint32_t dtknqr4_fifoemptymsk; /*!< 834h Device IN Token Queue Read Register-4 (Read Only).
	 	                                             Device IN EPs empty Inr. Mask Register (Read/Write)
	                                         */
} ifxusb_device_global_regs_t;

/*!
  \brief Bit fields in the Device Configuration Register.
 */

typedef union dcfg_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved31_26   : 6;
		unsigned perschintvl     : 2; /*!< 25-24 Periodic Scheduling Interval */
		unsigned descdma         : 1; /*!< 23    Enable Descriptor DMA in Device mode */
		unsigned epmscnt         : 5; /*!< 22-18 In Endpoint Mis-match count */
		unsigned reserved13_17   : 5;
		unsigned perfrint        : 2; /*!< 12-11 Periodic Frame Interval */
			#define IFXUSB_DCFG_FRAME_INTERVAL_80 0
			#define IFXUSB_DCFG_FRAME_INTERVAL_85 1
			#define IFXUSB_DCFG_FRAME_INTERVAL_90 2
			#define IFXUSB_DCFG_FRAME_INTERVAL_95 3
		unsigned devaddr         : 7; /*!< 10-04 Device Addresses */
		unsigned reserved3       : 1;
		unsigned nzstsouthshk    : 1; /*!< 02    Non Zero Length Status OUT Handshake */
			#define IFXUSB_DCFG_SEND_STALL 1
		unsigned devspd          : 2; /*!< 01-00 Device Speed */
	} b;
} dcfg_data_t;

/*!
  \brief Bit fields in the Device Control Register.
 */
typedef union dctl_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved16_31  :16;
		unsigned ifrmnum        : 1; /*!< 15    Ignore Frame Number for ISOC EPs */
		unsigned gmc            : 2; /*!< 14-13 Global Multi Count */
		unsigned gcontbna       : 1; /*!< 12    Global Continue on BNA */
		unsigned pwronprgdone   : 1; /*!< 11    Power-On Programming Done */
		unsigned cgoutnak       : 1; /*!< 10    Clear Global OUT NAK */
		unsigned sgoutnak       : 1; /*!< 09    Set Global OUT NAK */
		unsigned cgnpinnak      : 1; /*!< 08    Clear Global Non-Periodic IN NAK */
		unsigned sgnpinnak      : 1; /*!< 07    Set Global Non-Periodic IN NAK */
		unsigned tstctl         : 3; /*!< 06-04 Test Control */
		unsigned goutnaksts     : 1; /*!< 03    Global OUT NAK Status */
		unsigned gnpinnaksts    : 1; /*!< 02    Global Non-Periodic IN NAK Status */
		unsigned sftdiscon      : 1; /*!< 01    Soft Disconnect */
		unsigned rmtwkupsig     : 1; /*!< 00    Remote Wakeup */
	} b;
} dctl_data_t;


/*!
  \brief Bit fields in the Device Status Register.
 */
typedef union dsts_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved22_31  :10;
		unsigned soffn          :14; /*!< 21-08 Frame or Microframe Number of the received SOF */
		unsigned reserved4_7    : 4;
		unsigned errticerr      : 1; /*!< 03    Erratic Error */
		unsigned enumspd        : 2; /*!< 02-01 Enumerated Speed */
			#define IFXUSB_DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ 0
			#define IFXUSB_DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ 1
			#define IFXUSB_DSTS_ENUMSPD_LS_PHY_6MHZ           2
			#define IFXUSB_DSTS_ENUMSPD_FS_PHY_48MHZ          3
		unsigned suspsts        : 1; /*!< 00    Suspend Status */
	} b;
} dsts_data_t;

/*!
  \brief Bit fields in the Device IN EP Interrupt Register
         and the Device IN EP Common Mask Register.
 */
typedef union diepint_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved14_31   :18;
		unsigned nakmsk          : 1; /*!< 13 NAK interrupt Mask */
		unsigned reserved10_12   : 3;
		unsigned bna             : 1; /*!< 09 BNA Interrupt mask */
		unsigned txfifoundrn     : 1; /*!< 08 Fifo Underrun Mask */
		unsigned emptyintr       : 1; /*!< 07 IN Endpoint HAK Effective mask */
		unsigned inepnakeff      : 1; /*!< 06 IN Endpoint HAK Effective mask */
		unsigned intknepmis      : 1; /*!< 05 IN Token Received with EP mismatch mask */
		unsigned intktxfemp      : 1; /*!< 04 IN Token received with TxF Empty mask */
		unsigned timeout         : 1; /*!< 03 TimeOUT Handshake mask (non-ISOC EPs) */
		unsigned ahberr          : 1; /*!< 02 AHB Error mask */
		unsigned epdisabled      : 1; /*!< 01 Endpoint disable mask */
		unsigned xfercompl       : 1; /*!< 00 Transfer complete mask */
	} b;
} diepint_data_t;


/*!
  \brief Bit fields in the Device OUT EP Interrupt Register and
         Device OUT EP Common Interrupt Mask Register.
  */
typedef union doepint_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved15_31  :17;
		unsigned nyetmsk        : 1; /*!< 14 NYET Interrupt */
		unsigned nakmsk         : 1; /*!< 13 NAK Interrupt */
		unsigned bbleerrmsk     : 1; /*!< 12 Babble Interrupt */
		unsigned reserved10_11  : 2;
		unsigned bna            : 1; /*!< 09 BNA Interrupt */
		unsigned outpkterr      : 1; /*!< 08 OUT packet Error */
		unsigned reserved07     : 1;
		unsigned back2backsetup : 1; /*!< 06 Back-to-Back SETUP Packets Received */
		unsigned stsphsercvd    : 1; /*!< 05 */
		unsigned outtknepdis    : 1; /*!< 04 OUT Token Received when Endpoint Disabled */
		unsigned setup          : 1; /*!< 03 Setup Phase Done (contorl EPs) */
		unsigned ahberr         : 1; /*!< 02 AHB Error */
		unsigned epdisabled     : 1; /*!< 01 Endpoint disable */
		unsigned xfercompl      : 1; /*!< 00 Transfer complete */
	} b;
} doepint_data_t;


/*!
  \brief Bit fields in the Device All EP Interrupt Registers.
 */
typedef union daint_data
{
	uint32_t d32;
	struct
	{
		unsigned out : 16; /*!< 31-16 OUT Endpoint bits */
		unsigned in  : 16; /*!< 15-00 IN Endpoint bits */
	} eps;
	struct
	{
		/** OUT Endpoint bits */
		unsigned outep15 : 1;
		unsigned outep14 : 1;
		unsigned outep13 : 1;
		unsigned outep12 : 1;
		unsigned outep11 : 1;
		unsigned outep10 : 1;
		unsigned outep09 : 1;
		unsigned outep08 : 1;
		unsigned outep07 : 1;
		unsigned outep06 : 1;
		unsigned outep05 : 1;
		unsigned outep04 : 1;
		unsigned outep03 : 1;
		unsigned outep02 : 1;
		unsigned outep01 : 1;
		unsigned outep00 : 1;
		/** IN Endpoint bits */
		unsigned inep15 : 1;
		unsigned inep14 : 1;
		unsigned inep13 : 1;
		unsigned inep12 : 1;
		unsigned inep11 : 1;
		unsigned inep10 : 1;
		unsigned inep09 : 1;
		unsigned inep08 : 1;
		unsigned inep07 : 1;
		unsigned inep06 : 1;
		unsigned inep05 : 1;
		unsigned inep04 : 1;
		unsigned inep03 : 1;
		unsigned inep02 : 1;
		unsigned inep01 : 1;
		unsigned inep00 : 1;
	} ep;
} daint_data_t;


/*!
  \brief Bit fields in the Device IN Token Queue Read Registers.
 */
typedef union dtknq1_data
{
	uint32_t d32;
	struct
	{
		unsigned epnums0_5     :24; /*!< 31-08 EP Numbers of IN Tokens 0 ... 4 */
		unsigned wrap_bit      : 1; /*!< 07    write pointer has wrapped */
		unsigned reserved05_06 : 2;
		unsigned intknwptr     : 5; /*!< 04-00 In Token Queue Write Pointer */
	}b;
} dtknq1_data_t;


/*!
  \brief Bit fields in Threshold control Register
 */
typedef union dthrctl_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved26_31  : 6;
		unsigned rx_thr_len     : 9; /*!< 25-17 Rx Thr. Length */
		unsigned rx_thr_en      : 1; /*!< 16    Rx Thr. Enable */
		unsigned reserved11_15  : 5;
		unsigned tx_thr_len     : 9; /*!< 10-02 Tx Thr. Length */
		unsigned iso_thr_en     : 1; /*!< 01    ISO Tx Thr. Enable */
		unsigned non_iso_thr_en : 1; /*!< 00    non ISO Tx Thr. Enable */
	} b;
} dthrctl_data_t;

/*@}*//*IFXUSB_CSR_DEVICE_GLOBAL_REG*/

/****************************************************************************/

/*!
  \addtogroup IFXUSB_CSR_DEVICE_EP_REG
 */
/*@{*/

/*! typedef ifxusb_dev_in_ep_regs_t
  \brief Device Logical IN Endpoint-Specific Registers.
   There will be one set of endpoint registers per logical endpoint
   implemented.
   each EP's IN EP Register are offset at :
	       900h + * (ep_num * 20h)
 */

typedef struct ifxusb_dev_in_ep_regs
{
	volatile uint32_t diepctl;    /*!< 00h: Endpoint Control Register */
	uint32_t reserved04;          /*!< 04h: */
	volatile uint32_t diepint;    /*!< 08h: Endpoint Interrupt Register */
	uint32_t reserved0C;          /*!< 0Ch: */
	volatile uint32_t dieptsiz;   /*!< 10h: Endpoint Transfer Size Register.*/
	volatile uint32_t diepdma;    /*!< 14h: Endpoint DMA Address Register. */
	volatile uint32_t dtxfsts;    /*!< 18h: Endpoint Transmit FIFO Status Register. */
	volatile uint32_t diepdmab;   /*!< 1Ch: Endpoint DMA Buffer Register. */
} ifxusb_dev_in_ep_regs_t;

/*! typedef ifxusb_dev_out_ep_regs_t
  \brief Device Logical OUT Endpoint-Specific Registers.
   There will be one set of endpoint registers per logical endpoint
   implemented.
   each EP's OUT EP Register are offset at :
	       B00h + * (ep_num * 20h) + 00h
 */
typedef struct ifxusb_dev_out_ep_regs
{
	volatile uint32_t doepctl;    /*!< 00h: Endpoint Control Register */
	volatile uint32_t doepfn;     /*!< 04h: Endpoint Frame number Register */
	volatile uint32_t doepint;    /*!< 08h: Endpoint Interrupt Register */
	uint32_t reserved0C;          /*!< 0Ch: */
	volatile uint32_t doeptsiz;   /*!< 10h: Endpoint Transfer Size Register.*/
	volatile uint32_t doepdma;    /*!< 14h: Endpoint DMA Address Register. */
	uint32_t reserved18;          /*!< 18h: */
	volatile uint32_t doepdmab;   /*!< 1Ch: Endpoint DMA Buffer Register. */
} ifxusb_dev_out_ep_regs_t;


/*!
  \brief Bit fields in the Device EP Control
  Register.
 */
typedef union depctl_data
{
	uint32_t d32;
	struct
	{
		unsigned epena     : 1; /*!< 31    Endpoint Enable */
		unsigned epdis     : 1; /*!< 30    Endpoint Disable */
		unsigned setd1pid  : 1; /*!< 29    Set DATA1 PID (INTR/Bulk IN and OUT endpoints) */
		unsigned setd0pid  : 1; /*!< 28    Set DATA0 PID (INTR/Bulk IN and OUT endpoints) */
		unsigned snak      : 1; /*!< 27    Set NAK */
		unsigned cnak      : 1; /*!< 26    Clear NAK */
		unsigned txfnum    : 4; /*!< 25-22 Tx Fifo Number */
		unsigned stall     : 1; /*!< 21    Stall Handshake */
		unsigned snp       : 1; /*!< 20    Snoop Mode */
		unsigned eptype    : 2; /*!< 19-18 Endpoint Type
		                                  0: Control
		                                  1: Isochronous
		                                  2: Bulk
		                                  3: Interrupt
		                         */
		unsigned naksts    : 1; /*!< 17    NAK Status */
		unsigned dpid      : 1; /*!< 16    Endpoint DPID (INTR/Bulk IN and OUT endpoints) */
		unsigned usbactep  : 1; /*!< 15    USB Active Endpoint */
		unsigned nextep    : 4; /*!< 14-11 Next Endpoint */
		unsigned mps       :11; /*!< 10-00 Maximum Packet Size */
			#define IFXUSB_DEP0CTL_MPS_64   0
			#define IFXUSB_DEP0CTL_MPS_32   1
			#define IFXUSB_DEP0CTL_MPS_16   2
			#define IFXUSB_DEP0CTL_MPS_8    3
	} b;
} depctl_data_t;


/*!
  \brief Bit fields in the Device EP Transfer Size Register. (EP0 and EPn)
 */
typedef union deptsiz_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved31    : 1;
		unsigned supcnt        : 2; /*!< 30-29 Setup Packet Count */
		#ifdef  __DED_FIFO__
			unsigned reserved21_28 : 8;
			unsigned pktcnt        : 2; /*!< 19-20 Packet Count */
		#else
			unsigned reserved20_28 : 9;
			unsigned pktcnt        : 1; /*!< 19    Packet Count */
		#endif
		unsigned reserved7_18  :12;
		unsigned xfersize      : 7; /*!< 06-00 Transfer size */
	}b0;
	struct
	{
		unsigned reserved      : 1;
		unsigned mc            : 2; /*!< 30-29 Multi Count */
		unsigned pktcnt        :10; /*!< 28-19 Packet Count */
		unsigned xfersize      :19; /*!< 18-00 Transfer size */
	} b;
} deptsiz_data_t;

/*@}*//*IFXUSB_CSR_DEVICE_EP_REG*/
/****************************************************************************/

/*!
  \addtogroup IFXUSB_CSR_DEVICE_DMA_DESC
 */
/*@{*/
/*!
  \brief Bit fields in the DMA Descriptor status quadlet.
 */
typedef union desc_sts_data
{
	struct
	{
		unsigned bs            : 2; /*!< 31-30 Buffer Status */
			#define BS_HOST_READY	0x0
			#define BS_DMA_BUSY		0x1
			#define BS_DMA_DONE		0x2
			#define BS_HOST_BUSY	0x3
		unsigned sts           : 2; /*!< 29-28 Receive/Trasmit Status */
			#define RTS_SUCCESS		0x0
			#define RTS_BUFFLUSH	0x1
			#define RTS_RESERVED	0x2
			#define RTS_BUFERR		0x3
		unsigned l             : 1; /*!< 27    Last */
		unsigned sp            : 1; /*!< 26    Short Packet */
		unsigned ioc           : 1; /*!< 25    Interrupt On Complete */
		unsigned sr            : 1; /*!< 24    Setup Packet received */
		unsigned mtrf          : 1; /*!< 23    Multiple Transfer */
		unsigned reserved16_22 : 7;
		unsigned bytes         :16; /*!< 15-00 Transfer size in bytes */
	} b;
	uint32_t d32;    /*!< DMA Descriptor data buffer pointer */
} desc_sts_data_t;

/*@}*//*IFXUSB_CSR_DEVICE_DMA_DESC*/
/****************************************************************************/

/*!
  \addtogroup IFXUSB_CSR_HOST_GLOBAL_REG
 */
/*@{*/
/*! typedef ifxusb_host_global_regs_t
 \brief IFXUSB Host Mode Global registers. Offsets 400h-7FFh
        The ifxusb_host_global_regs structure defines the size
        and relative field offsets for the Host Global registers.
        These registers are visible only in Host mode and must not be
        accessed in Device mode, as the results are unknown.
 */
typedef struct ifxusb_host_global_regs
{
	volatile uint32_t hcfg;      /*!< 400h Host Configuration Register. */
	volatile uint32_t hfir;      /*!< 404h Host Frame Interval Register. */
	volatile uint32_t hfnum;     /*!< 408h Host Frame Number / Frame Remaining Register. */
	uint32_t reserved40C;
	volatile uint32_t hptxsts;   /*!< 410h Host Periodic Transmit FIFO/ Queue Status Register. */
	volatile uint32_t haint;     /*!< 414h Host All Channels Interrupt Register. */
	volatile uint32_t haintmsk;  /*!< 418h Host All Channels Interrupt Mask Register. */
} ifxusb_host_global_regs_t;

/*!
  \brief Bit fields in the Host Configuration Register.
 */
typedef union hcfg_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved31_03 :29;
		unsigned fslssupp      : 1; /*!< 02    FS/LS Only Support */
		unsigned fslspclksel   : 2; /*!< 01-00 FS/LS Phy Clock Select */
			#define IFXUSB_HCFG_30_60_MHZ 0
			#define IFXUSB_HCFG_48_MHZ    1
			#define IFXUSB_HCFG_6_MHZ     2
	} b;
} hcfg_data_t;

/*!
  \brief Bit fields in the Host Frame Interval Register.
 */
typedef union hfir_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved : 16;
		unsigned frint    : 16; /*!< 15-00 Frame Interval */
	} b;
} hfir_data_t;

/*!
 \brief Bit fields in the Host Frame Time Remaing/Number Register.
 */
typedef union hfnum_data
{
	uint32_t d32;
	struct
	{
		unsigned frrem : 16; /*!< 31-16 Frame Time Remaining */
		unsigned frnum : 16; /*!< 15-00 Frame Number*/
			#define IFXUSB_HFNUM_MAX_FRNUM 0x3FFF
	} b;
} hfnum_data_t;

/*!
  \brief Bit fields in the Host Periodic Transmit FIFO/Queue Status Register
 */
typedef union hptxsts_data
{
	/** raw register data */
	uint32_t d32;
	struct
	{
		/** Top of the Periodic Transmit Request Queue
		 *  - bit 24 - Terminate (last entry for the selected channel)
		 */
		unsigned ptxqtop_odd       : 1; /*!< 31    Top of the Periodic Transmit Request
		                                          Queue Odd/even microframe*/
		unsigned ptxqtop_chnum     : 4; /*!< 30-27 Top of the Periodic Transmit Request
		                                          Channel Number */
		unsigned ptxqtop_token     : 2; /*!< 26-25 Top of the Periodic Transmit Request
		                                          Token Type
		                                          0 - Zero length
		                                          1 - Ping
		                                          2 - Disable
		                                 */
		unsigned ptxqtop_terminate : 1; /*!< 24    Top of the Periodic Transmit Request
		                                          Terminate (last entry for the selected channel)*/
		unsigned ptxqspcavail      : 8; /*!< 23-16 Periodic Transmit Request Queue Space Available */
		unsigned ptxfspcavail      :16; /*!< 15-00 Periodic Transmit Data FIFO Space Available */
	} b;
} hptxsts_data_t;

/*!
  \brief Bit fields in the Host Port Control and Status Register.
 */
typedef union hprt0_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved19_31   :13;
		unsigned prtspd          : 2; /*!< 18-17 Port Speed */
			#define IFXUSB_HPRT0_PRTSPD_HIGH_SPEED 0
			#define IFXUSB_HPRT0_PRTSPD_FULL_SPEED 1
			#define IFXUSB_HPRT0_PRTSPD_LOW_SPEED  2
		unsigned prttstctl       : 4; /*!< 16-13 Port Test Control */
		unsigned prtpwr          : 1; /*!< 12    Port Power */
		unsigned prtlnsts        : 2; /*!< 11-10 Port Line Status */
		unsigned reserved9       : 1;
		unsigned prtrst          : 1; /*!< 08    Port Reset */
		unsigned prtsusp         : 1; /*!< 07    Port Suspend */
		unsigned prtres          : 1; /*!< 06    Port Resume */
		unsigned prtovrcurrchng  : 1; /*!< 05    Port Overcurrent Change */
		unsigned prtovrcurract   : 1; /*!< 04    Port Overcurrent Active */
		unsigned prtenchng       : 1; /*!< 03    Port Enable/Disable Change */
		unsigned prtena          : 1; /*!< 02    Port Enable */
		unsigned prtconndet      : 1; /*!< 01    Port Connect Detected */
		unsigned prtconnsts      : 1; /*!< 00    Port Connect Status */
	}b;
} hprt0_data_t;

/*!
  \brief Bit fields in the Host All Interrupt Register.
 */
typedef union haint_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved : 16;
		unsigned ch15 : 1;
		unsigned ch14 : 1;
		unsigned ch13 : 1;
		unsigned ch12 : 1;
		unsigned ch11 : 1;
		unsigned ch10 : 1;
		unsigned ch09 : 1;
		unsigned ch08 : 1;
		unsigned ch07 : 1;
		unsigned ch06 : 1;
		unsigned ch05 : 1;
		unsigned ch04 : 1;
		unsigned ch03 : 1;
		unsigned ch02 : 1;
		unsigned ch01 : 1;
		unsigned ch00 : 1;
	} b;
	struct
	{
		unsigned reserved : 16;
		unsigned chint    : 16;
	} b2;
} haint_data_t;
/*@}*//*IFXUSB_CSR_HOST_GLOBAL_REG*/
/****************************************************************************/
/*!
  \addtogroup IFXUSB_CSR_HOST_HC_REG
 */
/*@{*/
/*! typedef ifxusb_hc_regs_t
  \brief Host Channel Specific Registers
   There will be one set of hc registers per host channelimplemented.
   each HC's Register are offset at :
	       500h + * (hc_num * 20h)
 */
typedef struct ifxusb_hc_regs
{
	volatile uint32_t hcchar;   /*!< 00h Host Channel Characteristic Register.*/
	volatile uint32_t hcsplt;   /*!< 04h Host Channel Split Control Register.*/
	volatile uint32_t hcint;    /*!< 08h Host Channel Interrupt Register. */
	volatile uint32_t hcintmsk; /*!< 0Ch Host Channel Interrupt Mask Register. */
	volatile uint32_t hctsiz;   /*!< 10h Host Channel Transfer Size Register. */
	volatile uint32_t hcdma;    /*!< 14h Host Channel DMA Address Register. */
	uint32_t reserved[2];       /*!< 18h Reserved.   */
} ifxusb_hc_regs_t;


/*!
  \brief Bit fields in the Host Channel Characteristics Register.
 */
typedef union hcchar_data
{
	uint32_t d32;
	struct
	{
		unsigned chen      : 1; /*!< 31    Channel enable */
		unsigned chdis     : 1; /*!< 30    Channel disable */
		unsigned oddfrm    : 1; /*!< 29    Frame to transmit periodic transaction */
		unsigned devaddr   : 7; /*!< 28-22 Device address */
		unsigned multicnt  : 2; /*!< 21-20 Packets per frame for periodic transfers */
		unsigned eptype    : 2; /*!< 19-18 0: Control, 1: Isoc, 2: Bulk, 3: Intr */
		unsigned lspddev   : 1; /*!< 17    0: Full/high speed device, 1: Low speed device */
		unsigned reserved  : 1;
		unsigned epdir     : 1; /*!< 15    0: OUT, 1: IN */
		unsigned epnum     : 4; /*!< 14-11 Endpoint number */
		unsigned mps       :11; /*!< 10-00 Maximum packet size in bytes */
	} b;
} hcchar_data_t;

/*!
  \brief Bit fields in the Host Channel Split Control Register
 */
typedef union hcsplt_data
{
	uint32_t d32;
	struct
	{
		unsigned spltena  : 1; /*!< 31    Split Enble */
		unsigned reserved :14;
		unsigned compsplt : 1; /*!< 16    Do Complete Split */
		unsigned xactpos  : 2; /*!< 15-14 Transaction Position */
			#define IFXUSB_HCSPLIT_XACTPOS_MID 0
			#define IFXUSB_HCSPLIT_XACTPOS_END 1
			#define IFXUSB_HCSPLIT_XACTPOS_BEGIN 2
			#define IFXUSB_HCSPLIT_XACTPOS_ALL 3
		unsigned hubaddr  : 7; /*!< 13-07 Hub Address */
		unsigned prtaddr  : 7; /*!< 06-00 Port Address */
	} b;
} hcsplt_data_t;

/*!
  \brief Bit fields in the Host Interrupt Register.
 */
typedef union hcint_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved   :21;
		unsigned datatglerr : 1; /*!< 10 Data Toggle Error */
		unsigned frmovrun   : 1; /*!< 09 Frame Overrun */
		unsigned bblerr     : 1; /*!< 08 Babble Error */
		unsigned xacterr    : 1; /*!< 07 Transaction Err */
		unsigned nyet       : 1; /*!< 06 NYET Response Received */
		unsigned ack        : 1; /*!< 05 ACK Response Received */
		unsigned nak        : 1; /*!< 04 NAK Response Received */
		unsigned stall      : 1; /*!< 03 STALL Response Received */
		unsigned ahberr     : 1; /*!< 02 AHB Error */
		unsigned chhltd     : 1; /*!< 01 Channel Halted */
		unsigned xfercomp   : 1; /*!< 00 Channel Halted */
	}b;
} hcint_data_t;


/*!
 \brief Bit fields in the Host Channel Transfer Size
  Register.
 */
typedef union hctsiz_data
{
	uint32_t d32;
	struct
	{
		/** */
		unsigned dopng     : 1; /*!< 31    Do PING protocol when 1  */
		/**
		 * Packet ID for next data packet
		 * 0: DATA0
		 * 1: DATA2
		 * 2: DATA1
		 * 3: MDATA (non-Control), SETUP (Control)
		 */
		unsigned pid       : 2; /*!< 30-29 Packet ID for next data packet
		                                  0: DATA0
		                                  1: DATA2
		                                  2: DATA1
		                                  3: MDATA (non-Control), SETUP (Control)
		                         */
			#define IFXUSB_HCTSIZ_DATA0 0
			#define IFXUSB_HCTSIZ_DATA1 2
			#define IFXUSB_HCTSIZ_DATA2 1
			#define IFXUSB_HCTSIZ_MDATA 3
			#define IFXUSB_HCTSIZ_SETUP 3
		unsigned pktcnt    :10; /*!< 28-19 Data packets to transfer */
		unsigned xfersize  :19; /*!< 18-00 Total transfer size in bytes */
	}b;
} hctsiz_data_t;

/*@}*//*IFXUSB_CSR_HOST_HC_REG*/

/****************************************************************************/

/*!
  \addtogroup IFXUSB_CSR_PWR_CLK_GATING_REG
 */
/*@{*/
/*!
   \brief Bit fields in the Power and Clock Gating Control Register
 */
typedef union pcgcctl_data
{
	uint32_t d32;
	struct
	{
		unsigned reserved      : 27;
		unsigned physuspended  : 1; /*!< 04 PHY Suspended */
		unsigned rstpdwnmodule : 1; /*!< 03 Reset Power Down Modules */
		unsigned pwrclmp       : 1; /*!< 02 Power Clamp */
		unsigned gatehclk      : 1; /*!< 01 Gate Hclk */
		unsigned stoppclk      : 1; /*!< 00 Stop Pclk */
	} b;
} pcgcctl_data_t;
/*@}*//*IFXUSB_CSR_PWR_CLK_GATING_REG*/

/****************************************************************************/

#endif //__IFXUSB_REGS_H__
