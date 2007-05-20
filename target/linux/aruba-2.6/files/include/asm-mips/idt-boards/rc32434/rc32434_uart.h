/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *   UART register definitions
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

#ifndef __IDT_UART_H__
#define __IDT_UART_H__

enum
{
	UART0_PhysicalAddress	= 0x1c000000,
	UART_PhysicalAddress	= UART0_PhysicalAddress,	// Default

	UART0_VirtualAddress	= 0xbc000000,
	UART_VirtualAddress	= UART0_VirtualAddress,		// Default
} ;

/*
 * Register definitions are in bytes so we can handle endian problems.
 */

typedef struct UART_s
{
	union
	{
		u32 const	uartrb ;	// 0x00 - DLAB=0, read.
		u32		uartth ;	// 0x00 - DLAB=0, write.
		u32		uartdll ;	// 0x00 - DLAB=1, read/write.
	} ;

	union
	{
		u32		uartie ;	// 0x04 - DLAB=0, read/write.
		u32		uartdlh ;	// 0x04 - DLAB=1, read/write.
	} ;
	union
	{
		u32 const	uartii ;	// 0x08 - DLAB=0, read.
		u32		uartfc ;	// 0x08 - DLAB=0, write.
	} ;

	u32		uartlc ;		// 0x0c
	u32		uartmc ;		// 0x10
	u32		uartls ;		// 0x14
	u32		uartms ;		// 0x18
	u32		uarts ;			// 0x1c
} volatile *UART_t ;

// Reset registers.
typedef u32	volatile *UARTRR_t ;

enum
{
	UARTIE_rda_b	= 0,
	UARTIE_rda_m	= 0x00000001,
	UARTIE_the_b	= 1,
	UARTIE_the_m	= 0x00000002,
	UARTIE_rls_b	= 2,
	UARTIE_rls_m	= 0x00000004,
	UARTIE_ems_b	= 3,
	UARTIE_ems_m	= 0x00000008,

	UARTII_pi_b	= 0,
	UARTII_pi_m	= 0x00000001,
	UARTII_iid_b	= 1,
	UARTII_iid_m	= 0x0000000e,
		UARTII_iid_ms_v		= 0,	// Modem stat-CTS,DSR,RI or DCD.
		UARTII_iid_thre_v	= 1,	// Trans. Holding Reg. empty.
		UARTII_iid_rda_v	= 2,	// Receive data available
		UARTII_iid_rls_v	= 3,	// Overrun, parity, etc, error.
		UARTII_iid_res4_v	= 4,	// reserved.
		UARTII_iid_res5_v	= 5,	// reserved.
		UARTII_iid_cto_v	= 6,	// Character timeout.
		UARTII_iid_res7_v	= 7,	// reserved.

	UARTFC_en_b	= 0,
	UARTFC_en_m	= 0x00000001,
	UARTFC_rr_b	= 1,
	UARTFC_rr_m	= 0x00000002,
	UARTFC_tr_b	= 2,
	UARTFC_tr_m	= 0x00000004,
	UARTFC_dms_b	= 3,
	UARTFC_dms_m	= 0x00000008,
	UARTFC_rt_b	= 6,
	UARTFC_rt_m	= 0x000000c0,
		UARTFC_rt_1Byte_v	= 0,
		UARTFC_rt_4Byte_v	= 1,
		UARTFC_rt_8Byte_v	= 2,
		UARTFC_rt_14Byte_v	= 3,

	UARTLC_wls_b	= 0,
	UARTLC_wls_m	= 0x00000003,
		UARTLC_wls_5Bits_v	= 0,
		UARTLC_wls_6Bits_v	= 1,
		UARTLC_wls_7Bits_v	= 2,
		UARTLC_wls_8Bits_v	= 3,
	UARTLC_stb_b	= 2,
	UARTLC_stb_m	= 0x00000004,
	UARTLC_pen_b	= 3,
	UARTLC_pen_m	= 0x00000008,
	UARTLC_eps_b	= 4,
	UARTLC_eps_m	= 0x00000010,
	UARTLC_sp_b	= 5,
	UARTLC_sp_m	= 0x00000020,
	UARTLC_sb_b	= 6,
	UARTLC_sb_m	= 0x00000040,
	UARTLC_dlab_b	= 7,
	UARTLC_dlab_m	= 0x00000080,

	UARTMC_dtr_b	= 0,
	UARTMC_dtr_m	= 0x00000001,
	UARTMC_rts_b	= 1,
	UARTMC_rts_m	= 0x00000002,
	UARTMC_o1_b	= 2,
	UARTMC_o1_m	= 0x00000004,
	UARTMC_o2_b	= 3,
	UARTMC_o2_m	= 0x00000008,
	UARTMC_lp_b	= 4,
	UARTMC_lp_m	= 0x00000010,

	UARTLS_dr_b	= 0,
	UARTLS_dr_m	= 0x00000001,
	UARTLS_oe_b	= 1,
	UARTLS_oe_m	= 0x00000002,
	UARTLS_pe_b	= 2,
	UARTLS_pe_m	= 0x00000004,
	UARTLS_fe_b	= 3,
	UARTLS_fe_m	= 0x00000008,
	UARTLS_bi_b	= 4,
	UARTLS_bi_m	= 0x00000010,
	UARTLS_thr_b	= 5,
	UARTLS_thr_m	= 0x00000020,
	UARTLS_te_b	= 6,
	UARTLS_te_m	= 0x00000040,
	UARTLS_rfe_b	= 7,
	UARTLS_rfe_m	= 0x00000080,

	UARTMS_dcts_b	= 0,
	UARTMS_dcts_m	= 0x00000001,
	UARTMS_ddsr_b	= 1,
	UARTMS_ddsr_m	= 0x00000002,
	UARTMS_teri_b	= 2,
	UARTMS_teri_m	= 0x00000004,
	UARTMS_ddcd_b	= 3,
	UARTMS_ddcd_m	= 0x00000008,
	UARTMS_cts_b	= 4,
	UARTMS_cts_m	= 0x00000010,
	UARTMS_dsr_b	= 5,
	UARTMS_dsr_m	= 0x00000020,
	UARTMS_ri_b	= 6,
	UARTMS_ri_m	= 0x00000040,
	UARTMS_dcd_b	= 7,
	UARTMS_dcd_m	= 0x00000080,
} ;

#endif	// __IDT_UART_H__
