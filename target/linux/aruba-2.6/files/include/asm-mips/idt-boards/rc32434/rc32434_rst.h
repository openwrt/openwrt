/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *   Reset register definitions.
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

#ifndef __IDT_RST_H__
#define __IDT_RST_H__

enum
{
	RST0_PhysicalAddress	= 0x18000000,
	RST_PhysicalAddress	= RST0_PhysicalAddress,		// Default

	RST0_VirtualAddress	= 0xb8000000,
	RST_VirtualAddress	= RST0_VirtualAddress,		// Default
} ;

typedef struct RST_s
{
	u32	filler [0x0006] ;
	u32	sysid ;
	u32	filler2 [0x2000-8] ;		// Pad out to offset 0x8000
	u32	reset ;
	u32	bcv ;
	u32	cea ;
} volatile * RST_t ;

enum
{
	SYSID_rev_b		= 0,
	SYSID_rev_m		= 0x000000ff,
	SYSID_imp_b		= 8,
	SYSID_imp_m		= 0x000fff00,
	SYSID_vendor_b		= 8,
	SYSID_vendor_m		= 0xfff00000,

	BCV_pll_b		= 0,
	BCV_pll_m		= 0x0000000f,
		BCV_pll_PLLBypass_v	= 0x0,	// PCLK=1*CLK.
		BCV_pll_Mul3_v		= 0x1,	// PCLK=3*CLK.
		BCV_pll_Mul4_v		= 0x2,	// PCLK=4*CLK.
		BCV_pll_SlowMul5_v	= 0x3,	// PCLK=5*CLK.
		BCV_pll_Mul5_v		= 0x4,	// PCLK=5*CLK.
		BCV_pll_SlowMul6_v	= 0x5,	// PCLK=6*CLK.
		BCV_pll_Mul6_v		= 0x6,	// PCLK=6*CLK.
		BCV_pll_Mul8_v		= 0x7,	// PCLK=8*CLK.
		BCV_pll_Mul10_v		= 0x8,	// PCLK=10*CLK.
	        BCV_pll_Res9_v	        = 0x9,
		BCV_pll_Res10_v	        = 0xa,
		BCV_pll_Res11_v	        = 0xb,
		BCV_pll_Res12_v	        = 0xc,
		BCV_pll_Res13_v		= 0xd,
		BCV_pll_Res14_v		= 0xe,
		BCV_pll_Res15_v		= 0xf,
	BCV_clkDiv_b		= 4,
	BCV_clkDiv_m		= 0x00000030,
		BCV_clkDiv_Div1_v	= 0x0,
		BCV_clkDiv_Div2_v	= 0x1,
		BCV_clkDiv_Div4_v	= 0x2,
		BCV_clkDiv_Res3_v	= 0x3,
	BCV_bigEndian_b		= 6,
	BCV_bigEndian_m		= 0x00000040,
	BCV_resetFast_b		= 7,
	BCV_resetFast_m		= 0x00000080,
	BCV_pciMode_b		= 8,
	BCV_pciMode_m		= 0x00000700,
		BCV_pciMode_disabled_v	= 0,	// PCI is disabled.
		BCV_pciMode_tnr_v	= 1,	// satellite Target Not Ready.
		BCV_pciMode_suspended_v	= 2,	// satellite with suspended CPU.
		BCV_pciMode_external_v	= 3,	// host, external arbiter.
		BCV_pciMode_fixed_v	= 4,	// host, fixed priority arbiter.
		BCV_pciMode_roundRobin_v= 5,	// host, round robin arbiter.
		BCV_pciMode_res6_v	= 6,
		BCV_pciMode_res7_v	= 7,
	BCV_watchDisable_b	= 11,
	BCV_watchDisable_m	= 0x00000800,
	BCV_res12_b		= 12,
	BCV_res12_m		= 0x00001000,
	BCV_res13_b		= 13,
	BCV_res13_m		= 0x00002000,
	BCV_res14_b		= 14,
	BCV_res14_m		= 0x00004000,
	BCV_res15_b		= 15,
	BCV_res15_m		= 0x00008000,
} ;
#endif	// __IDT_RST_H__
