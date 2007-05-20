/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *   IP Arbiter register definitions
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
 * May 2004 rkt,neb
 *
 * Initial Release
 *
 * 
 *
 **************************************************************************
 */

#ifndef __IDT_IPARB_H__
#define __IDT_IPARB_H__

enum
{
	IPARB0_PhysicalAddress	= 0x18048000,
	IPARB_PhysicalAddress	= IPARB0_PhysicalAddress,	// Default

	IPARB0_VirtualAddress	= 0xB8048000,
	IPARB_VirtualAddress	= IPARB0_VirtualAddress,	// Default
} ;

enum
{
	IPABMXC_ethernet0Receive	= 0,
	IPABMXC_ethernet0Transmit	= 1,
	IPABMXC_memoryToHoldFifo	= 2,
	IPABMXC_holdFifoToMemory	= 3,
	IPABMXC_pciToMemory		= 4,
	IPABMXC_memoryToPci		= 5,
	IPABMXC_pciTarget		= 6,
	IPABMXC_pciTargetStart		= 7,
	IPABMXC_cpuToIpBus		= 8,

	IPABMXC_Count,				// Must be last in list !
	IPABMXC_Min			= IPABMXC_ethernet0Receive,

	IPAPXC_PriorityCount	= 4,		// 3-highest, 0-lowest.
} ;

typedef struct
{
	u32	ipapc [IPAPXC_PriorityCount] ;	// ipapc[IPAPXC_] = IPAPC_
	u32	ipabmc [IPABMXC_Count] ;	// ipabmc[IPABMXC_] = IPABMC_
	u32	ipac ;				// use IPAC_
	u32	ipaitcc;			// use IPAITCC_
	u32	ipaspare ;
} volatile * IPARB_t ;

enum
{
	IPAC_dp_b			= 0,
	IPAC_dp_m			= 0x00000001,
	IPAC_dep_b			= 1,
	IPAC_dep_m			= 0x00000002,
	IPAC_drm_b			= 2,
	IPAC_drm_m			= 0x00000004,
	IPAC_dwm_b			= 3,
	IPAC_dwm_m			= 0x00000008,
	IPAC_msk_b			= 4,
	IPAC_msk_m			= 0x00000010,

	IPAPC_ptc_b			= 0,
	IPAPC_ptc_m			= 0x00003fff,
	IPAPC_mf_b			= 14,
	IPAPC_mf_m			= 0x00004000,
	IPAPC_cptc_b			= 16,
	IPAPC_cptc_m			= 0x3fff0000,

	IPAITCC_itcc			= 0,
	IPAITCC_itcc,			= 0x000001ff,

	IPABMC_mtc_b			= 0,
	IPABMC_mtc_m			= 0x00000fff,
	IPABMC_p_b			= 12,
	IPABMC_p_m			= 0x00003000,
	IPABMC_msk_b			= 14,
	IPABMC_msk_m			= 0x00004000,
	IPABMC_cmtc_b			= 16,
	IPABMC_cmtc_m			= 0x0fff0000,
};

#endif	// __IDT_IPARB_H__
