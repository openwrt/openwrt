/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *   Interrupt Controller register definition.
 *
 *  Copyright 2004 IDT Inc. (rischelp@idt.com)
 *         
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 **************************************************************************
 * May 2004 rkt, neb.
 *
 * Initial Release
 *
 * 
 *
 **************************************************************************
 */

#ifndef __IDT_INT_H__
#define __IDT_INT_H__

enum
{
	INT0_PhysicalAddress	= 0x18038000,
	INT_PhysicalAddress	= INT0_PhysicalAddress,		// Default

	INT0_VirtualAddress	= 0xB8038000,
	INT_VirtualAddress	= INT0_VirtualAddress,		// Default
} ;

struct INT_s
{
	u32		ipend ;		//Pending interrupts. use INT?_
	u32		itest ;		//Test bits.		use INT?_
	u32		imask ;		//Interrupt disabled when set. use INT?_
} ;

enum
{
	IPEND2	= 0,			// HW 2 interrupt to core. use INT2_
	IPEND3	= 1,			// HW 3 interrupt to core. use INT3_
	IPEND4	= 2,			// HW 4 interrupt to core. use INT4_
	IPEND5	= 3,			// HW 5 interrupt to core. use INT5_
	IPEND6	= 4,			// HW 6 interrupt to core. use INT6_

	IPEND_count,			// must be last (used in loops)
	IPEND_min	= IPEND2	// min IPEND (used in loops)
};

typedef struct INTC_s
{
	struct INT_s	i [IPEND_count] ;// use i[IPEND?] = INT?_
	u32		nmips ;		// use NMIPS_
} volatile *INT_t ;

enum
{
	INT2_timer0_b			= 0,
	INT2_timer0_m			= 0x00000001,
	INT2_timer1_b			= 1,
	INT2_timer1_m			= 0x00000002,
	INT2_timer2_b			= 2,
	INT2_timer2_m			= 0x00000004,
	INT2_refresh_b			= 3,
	INT2_refresh_m			= 0x00000008,
	INT2_watchdogTimeout_b		= 4,
	INT2_watchdogTimeout_m		= 0x00000010,
	INT2_undecodedCpuWrite_b	= 5,
	INT2_undecodedCpuWrite_m	= 0x00000020,
	INT2_undecodedCpuRead_b		= 6,
	INT2_undecodedCpuRead_m		= 0x00000040,
	INT2_undecodedPciWrite_b	= 7,
	INT2_undecodedPciWrite_m	= 0x00000080,
	INT2_undecodedPciRead_b		= 8,
	INT2_undecodedPciRead_m		= 0x00000100,
	INT2_undecodedDmaWrite_b	= 9,
	INT2_undecodedDmaWrite_m	= 0x00000200,
	INT2_undecodedDmaRead_b		= 10,
	INT2_undecodedDmaRead_m		= 0x00000400,
	INT2_ipBusSlaveAckError_b	= 11,
	INT2_ipBusSlaveAckError_m	= 0x00000800,

	INT3_dmaChannel0_b		= 0,
	INT3_dmaChannel0_m		= 0x00000001,
	INT3_dmaChannel1_b		= 1,
	INT3_dmaChannel1_m		= 0x00000002,
	INT3_dmaChannel2_b		= 2,
	INT3_dmaChannel2_m		= 0x00000004,
	INT3_dmaChannel3_b		= 3,
	INT3_dmaChannel3_m		= 0x00000008,
	INT3_dmaChannel4_b		= 4,
	INT3_dmaChannel4_m		= 0x00000010,
	INT3_dmaChannel5_b		= 5,
	INT3_dmaChannel5_m		= 0x00000020,

	INT5_uartGeneral0_b		= 0,
	INT5_uartGeneral0_m		= 0x00000001,
	INT5_uartTxrdy0_b		= 1,
	INT5_uartTxrdy0_m		= 0x00000002,
	INT5_uartRxrdy0_b		= 2,
	INT5_uartRxrdy0_m		= 0x00000004,
	INT5_pci_b			= 3,
	INT5_pci_m			= 0x00000008,
	INT5_pciDecoupled_b		= 4,
	INT5_pciDecoupled_m		= 0x00000010,
	INT5_spi_b			= 5,
	INT5_spi_m			= 0x00000020,
	INT5_deviceDecoupled_b		= 6,
	INT5_deviceDecoupled_m		= 0x00000040,
	INT5_eth0Ovr_b			= 9,
	INT5_eth0Ovr_m			= 0x00000200,
	INT5_eth0Und_b			= 10,
	INT5_eth0Und_m			= 0x00000400,
	INT5_eth0Pfd_b			= 11,
	INT5_eth0Pfd_m			= 0x00000800,
	INT5_nvram_b			= 12,
	INT5_nvram_m			= 0x00001000,

	INT6_gpio0_b			= 0,
	INT6_gpio0_m			= 0x00000001,
	INT6_gpio1_b			= 1,
	INT6_gpio1_m			= 0x00000002,
	INT6_gpio2_b			= 2,
	INT6_gpio2_m			= 0x00000004,
	INT6_gpio3_b			= 3,
	INT6_gpio3_m			= 0x00000008,
	INT6_gpio4_b			= 4,
	INT6_gpio4_m			= 0x00000010,
	INT6_gpio5_b			= 5,
	INT6_gpio5_m			= 0x00000020,
	INT6_gpio6_b			= 6,
	INT6_gpio6_m			= 0x00000040,
	INT6_gpio7_b			= 7,
	INT6_gpio7_m			= 0x00000080,
	INT6_gpio8_b			= 8,
	INT6_gpio8_m			= 0x00000100,
	INT6_gpio9_b			= 9,
	INT6_gpio9_m			= 0x00000200,
	INT6_gpio10_b			= 10,
	INT6_gpio10_m			= 0x00000400,
	INT6_gpio11_b			= 11,
	INT6_gpio11_m			= 0x00000800,
	INT6_gpio12_b			= 12,
	INT6_gpio12_m			= 0x00001000,
	INT6_gpio13_b			= 13,
	INT6_gpio13_m			= 0x00002000,

	NMIPS_gpio_b			= 0,
	NMIPS_gpio_m			= 0x00000001,
} ;

#endif	// __IDT_INT_H__


