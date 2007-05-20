/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *   DMA register definition
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

#ifndef __IDT_DMA_H__
#define __IDT_DMA_H__

enum
{
	DMA0_PhysicalAddress	= 0x18040000,
	DMA_PhysicalAddress	= DMA0_PhysicalAddress,		// Default

	DMA0_VirtualAddress	= 0xb8040000,
	DMA_VirtualAddress	= DMA0_VirtualAddress,		// Default
} ;

/*
 * DMA descriptor (in physical memory).
 */

typedef struct DMAD_s
{
	u32			control ;	// Control. use DMAD_*
	u32			ca ;		// Current Address.
	u32			devcs ; 	// Device control and status.
	u32			link ;		// Next descriptor in chain.
} volatile *DMAD_t ;

enum
{
	DMAD_size		= sizeof (struct DMAD_s),
	DMAD_count_b		= 0,		// in DMAD_t -> control
	DMAD_count_m		= 0x0003ffff,	// in DMAD_t -> control
	DMAD_ds_b		= 20,		// in DMAD_t -> control
	DMAD_ds_m		= 0x00300000,	// in DMAD_t -> control
		DMAD_ds_ethRcv0_v	= 0,
		DMAD_ds_ethXmt0_v	= 0,
		DMAD_ds_memToFifo_v	= 0,
		DMAD_ds_fifoToMem_v	= 0,
		DMAD_ds_pciToMem_v	= 0,
		DMAD_ds_memToPci_v	= 0,
	
	DMAD_devcmd_b		= 22,		// in DMAD_t -> control
	DMAD_devcmd_m		= 0x01c00000,	// in DMAD_t -> control
		DMAD_devcmd_byte_v	= 0,	//memory-to-memory
		DMAD_devcmd_halfword_v	= 1,	//memory-to-memory
		DMAD_devcmd_word_v	= 2,	//memory-to-memory
		DMAD_devcmd_2words_v	= 3,	//memory-to-memory
		DMAD_devcmd_4words_v	= 4,	//memory-to-memory
		DMAD_devcmd_6words_v	= 5,	//memory-to-memory
		DMAD_devcmd_8words_v	= 6,	//memory-to-memory
		DMAD_devcmd_16words_v	= 7,	//memory-to-memory
	DMAD_cof_b		= 25,		// chain on finished
	DMAD_cof_m		= 0x02000000,	// 
	DMAD_cod_b		= 26,		// chain on done
	DMAD_cod_m		= 0x04000000,	// 
	DMAD_iof_b		= 27,		// interrupt on finished
	DMAD_iof_m		= 0x08000000,	// 
	DMAD_iod_b		= 28,		// interrupt on done
	DMAD_iod_m		= 0x10000000,	// 
	DMAD_t_b		= 29,		// terminated
	DMAD_t_m		= 0x20000000,	// 
	DMAD_d_b		= 30,		// done
	DMAD_d_m		= 0x40000000,	// 
	DMAD_f_b		= 31,		// finished
	DMAD_f_m		= 0x80000000,	// 
} ;

/*
 * DMA register (within Internal Register Map).
 */

struct DMA_Chan_s
{
	u32		dmac ;		// Control.
	u32		dmas ;		// Status.	
	u32		dmasm ; 	// Mask.
	u32		dmadptr ;	// Descriptor pointer.
	u32		dmandptr ;	// Next descriptor pointer.
};

typedef struct DMA_Chan_s volatile *DMA_Chan_t ;

//DMA_Channels	  use DMACH_count instead

enum
{
	DMAC_run_b	= 0,		// 
	DMAC_run_m	= 0x00000001,	// 
	DMAC_dm_b	= 1,		// done mask
	DMAC_dm_m	= 0x00000002,	// 
	DMAC_mode_b	= 2,		// 
	DMAC_mode_m	= 0x0000000c,	// 
		DMAC_mode_auto_v	= 0,
		DMAC_mode_burst_v	= 1,
		DMAC_mode_transfer_v	= 2, //usually used
		DMAC_mode_reserved_v	= 3,
	DMAC_a_b	= 4,		// 
	DMAC_a_m	= 0x00000010,	// 

	DMAS_f_b	= 0,		// finished (sticky) 
	DMAS_f_m	= 0x00000001,	//		     
	DMAS_d_b	= 1,		// done (sticky)     
	DMAS_d_m	= 0x00000002,	//		     
	DMAS_c_b	= 2,		// chain (sticky)    
	DMAS_c_m	= 0x00000004,	//		     
	DMAS_e_b	= 3,		// error (sticky)    
	DMAS_e_m	= 0x00000008,	//		     
	DMAS_h_b	= 4,		// halt (sticky)     
	DMAS_h_m	= 0x00000010,	//		     

	DMASM_f_b	= 0,		// finished (1=mask)
	DMASM_f_m	= 0x00000001,	// 
	DMASM_d_b	= 1,		// done (1=mask)
	DMASM_d_m	= 0x00000002,	// 
	DMASM_c_b	= 2,		// chain (1=mask)
	DMASM_c_m	= 0x00000004,	// 
	DMASM_e_b	= 3,		// error (1=mask)
	DMASM_e_m	= 0x00000008,	// 
	DMASM_h_b	= 4,		// halt (1=mask)
	DMASM_h_m	= 0x00000010,	// 
} ;

/*
 * DMA channel definitions
 */

enum
{
	DMACH_ethRcv0 = 0,
	DMACH_ethXmt0 = 1,
	DMACH_memToFifo = 2,
	DMACH_fifoToMem = 3,
	DMACH_pciToMem = 4,
	DMACH_memToPci = 5,

	DMACH_count //must be last
};


typedef struct DMAC_s
{
	struct DMA_Chan_s ch [DMACH_count] ; //use ch[DMACH_]
} volatile *DMA_t ;


/*
 * External DMA parameters
*/

enum
{
	DMADEVCMD_ts_b	= 0,		// ts field in devcmd
	DMADEVCMD_ts_m	= 0x00000007,	// ts field in devcmd
		DMADEVCMD_ts_byte_v	= 0,
		DMADEVCMD_ts_halfword_v	= 1,
		DMADEVCMD_ts_word_v	= 2,
		DMADEVCMD_ts_2word_v	= 3,
		DMADEVCMD_ts_4word_v	= 4,
		DMADEVCMD_ts_6word_v	= 5,
		DMADEVCMD_ts_8word_v	= 6,
		DMADEVCMD_ts_16word_v	= 7
};


#endif	// __IDT_DMA_H__





