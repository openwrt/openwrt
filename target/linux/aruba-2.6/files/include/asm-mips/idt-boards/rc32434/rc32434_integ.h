/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *   System Integrity register definition
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
 * May 2004 rkt, neb
 *
 * Initial Release
 *
 * 
 *
 **************************************************************************
 */

#ifndef __IDT_INTEG_H__
#define __IDT_INTEG_H__

enum
{
	INTEG0_PhysicalAddress	= 0x18030000,
	INTEG_PhysicalAddress	= INTEG0_PhysicalAddress,	// Default

	INTEG0_VirtualAddress	= 0xB8030000,
	INTEG_VirtualAddress	= INTEG0_VirtualAddress,	// Default
} ;

// if you are looking for CEA, try rst.h
typedef struct
{
	u32 filler [0xc] ;		// 0x30 bytes unused.
	u32 errcs ;			// sticky use ERRCS_
	u32 wtcount ;			// Watchdog timer count reg.
	u32 wtcompare ;			// Watchdog timer timeout value.
	u32 wtc ;			// Watchdog timer control. use WTC_
} volatile *INTEG_t ;

enum
{
	ERRCS_wto_b		= 0,		// In INTEG_t -> errcs
	ERRCS_wto_m		= 0x00000001,
	ERRCS_wne_b		= 1,		// In INTEG_t -> errcs
	ERRCS_wne_m		= 0x00000002,
	ERRCS_ucw_b		= 2,		// In INTEG_t -> errcs
	ERRCS_ucw_m		= 0x00000004,
	ERRCS_ucr_b		= 3,		// In INTEG_t -> errcs
	ERRCS_ucr_m		= 0x00000008,
	ERRCS_upw_b		= 4,		// In INTEG_t -> errcs
	ERRCS_upw_m		= 0x00000010,
	ERRCS_upr_b		= 5,		// In INTEG_t -> errcs
	ERRCS_upr_m		= 0x00000020,
	ERRCS_udw_b		= 6,		// In INTEG_t -> errcs
	ERRCS_udw_m		= 0x00000040,
	ERRCS_udr_b		= 7,		// In INTEG_t -> errcs
	ERRCS_udr_m		= 0x00000080,
	ERRCS_sae_b		= 8,		// In INTEG_t -> errcs
	ERRCS_sae_m		= 0x00000100,
	ERRCS_wre_b		= 9,		// In INTEG_t -> errcs
	ERRCS_wre_m		= 0x00000200,

	WTC_en_b		= 0,		// In INTEG_t -> wtc
	WTC_en_m		= 0x00000001,
	WTC_to_b		= 1,		// In INTEG_t -> wtc
	WTC_to_m		= 0x00000002,
} ;

#endif	// __IDT_INTEG_H__
