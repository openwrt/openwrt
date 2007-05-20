/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *   Serial Peripheral Interface register definitions.
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

#ifndef __IDT_SPI_H__
#define __IDT_SPI_H__

enum
{
	SPI0_PhysicalAddress	= 0x18070000,
	SPI_PhysicalAddress	= SPI0_PhysicalAddress,

	SPI0_VirtualAddress	= 0xB8070000,
	SPI_VirtualAddress	= SPI0_VirtualAddress,
} ;

typedef struct
{
	u32 spcp ;	// prescalar. 0=off, * spiClk = sysClk/(2*(spcp+1)*SPR)
	u32 spc ;	// spi control reg use SPC_
	u32 sps ;	// spi status reg use SPS_
	u32 spd ;	// spi data reg use SPD_
	u32 siofunc ;	// serial IO function use SIOFUNC_
	u32 siocfg ;	// serial IO config use SIOCFG_
	u32 siod;	// serial IO data use SIOD_
} volatile *SPI_t ;

enum
{
	SPCP_div_b	 = 0,	       
	SPCP_div_m	 = 0x000000ff,
	SPC_spr_b	= 0,	       
	SPC_spr_m	= 0x00000003,
	     SPC_spr_div2_v  = 0,
	     SPC_spr_div4_v  = 1,
	     SPC_spr_div16_v = 2,
	     SPC_spr_div32_v = 3,
	SPC_cpha_b	= 2,	       
	SPC_cpha_m	= 0x00000004,
	SPC_cpol_b	= 3,	       
	SPC_cpol_m	= 0x00000008,
	SPC_mstr_b	= 4,	       
	SPC_mstr_m	= 0x00000010,
	SPC_spe_b	= 6,	       
	SPC_spe_m	= 0x00000040,
	SPC_spie_b	= 7,	       
	SPC_spie_m	= 0x00000080,

	SPS_modf_b	= 4,	       
	SPS_modf_m	= 0x00000010,
	SPS_wcol_b	= 6,	       
	SPS_wcol_m	= 0x00000040,
	SPS_spif_b	= 7,	       
	SPS_spif_m	= 0x00000070,

	SPD_data_b	= 0,	       
	SPD_data_m	= 0x000000ff,

	SIOFUNC_sdo_b	    = 0,	   
	SIOFUNC_sdo_m	    = 0x00000001,
	SIOFUNC_sdi_b	    = 1,	   
	SIOFUNC_sdi_m	    = 0x00000002,
	SIOFUNC_sck_b	    = 2,	   
	SIOFUNC_sck_m	    = 0x00000004,
	SIOFUNC_pci_b	    = 3,	   
	SIOFUNC_pci_m	    = 0x00000008,
	
	SIOCFG_sdo_b	   = 0, 	   
	SIOCFG_sdo_m	   = 0x00000001,
	SIOCFG_sdi_b	   = 1, 	   
	SIOCFG_sdi_m	   = 0x00000002,
	SIOCFG_sck_b	   = 2, 	   
	SIOCFG_sck_m	   = 0x00000004,
	SIOCFG_pci_b	   = 3, 	   
	SIOCFG_pci_m	   = 0x00000008,
	
	SIOD_sdo_b	 = 0,		 
	SIOD_sdo_m	 = 0x00000001,
	SIOD_sdi_b	 = 1,		 
	SIOD_sdi_m	 = 0x00000002,
	SIOD_sck_b	 = 2,		 
	SIOD_sck_m	 = 0x00000004,
	SIOD_pci_b	 = 3,		 
	SIOD_pci_m	 = 0x00000008,
} ;
#endif	// __IDT_SPI_H__
