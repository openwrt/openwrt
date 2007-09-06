/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *   PCI register definitio
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

#ifndef __IDT_PCI_H__
#define __IDT_PCI_H__

enum
{
	PCI0_PhysicalAddress	= 0x18080000,
	PCI_PhysicalAddress	= PCI0_PhysicalAddress,

	PCI0_VirtualAddress	= 0xB8080000,
	PCI_VirtualAddress	= PCI0_VirtualAddress,
} ;

enum
{
	PCI_LbaCount	= 4,		// Local base addresses.
} ;

typedef struct
{
	u32	a ;		// Address.
	u32	c ;		// Control.
	u32	m ;		// mapping.
} PCI_Map_s ;

typedef struct
{
	u32		pcic ;
	u32		pcis ;
	u32		pcism ;
	u32		pcicfga ;
	u32		pcicfgd ;
	PCI_Map_s	pcilba [PCI_LbaCount] ;
	u32		pcidac ;
	u32		pcidas ;
	u32		pcidasm ;
	u32		pcidad ;
	u32		pcidma8c ;
	u32		pcidma9c ;
	u32		pcitc ;
} volatile *PCI_t ;

// PCI messaging unit.
enum
{
	PCIM_Count	= 2,
} ;
typedef struct
{
	u32		pciim [PCIM_Count] ;
	u32		pciom [PCIM_Count] ;
	u32		pciid ;
	u32		pciiic ;
	u32		pciiim ;
	u32		pciiod ;
	u32		pciioic ;
	u32		pciioim ;
} volatile *PCIM_t ;

/*******************************************************************************
 *
 * PCI Control Register
 *
 ******************************************************************************/
enum
{
	PCIC_en_b	= 0,
	PCIC_en_m	= 0x00000001,
	PCIC_tnr_b	= 1,
	PCIC_tnr_m	= 0x00000002,
	PCIC_sce_b	= 2,
	PCIC_sce_m	= 0x00000004,
	PCIC_ien_b	= 3,
	PCIC_ien_m	= 0x00000008,
	PCIC_aaa_b	= 4,
	PCIC_aaa_m	= 0x00000010,
	PCIC_eap_b	= 5,
	PCIC_eap_m	= 0x00000020,
	PCIC_pcim_b	= 6,
	PCIC_pcim_m	= 0x000001c0,
		PCIC_pcim_disabled_v	= 0,
		PCIC_pcim_tnr_v 	= 1,	// Satellite - target not ready
		PCIC_pcim_suspend_v	= 2,	// Satellite - suspended CPU.
		PCIC_pcim_extern_v	= 3,	// Host - external arbiter.
		PCIC_pcim_fixed_v	= 4,	// Host - fixed priority arb.
		PCIC_pcim_roundrobin_v	= 5,	// Host - round robin priority.
		PCIC_pcim_reserved6_v	= 6,
		PCIC_pcim_reserved7_v	= 7,
	PCIC_igm_b	= 9,
	PCIC_igm_m	= 0x00000200,
} ;

/*******************************************************************************
 *
 * PCI Status Register
 *
 ******************************************************************************/
enum {
	PCIS_eed_b	= 0,
	PCIS_eed_m	= 0x00000001,
	PCIS_wr_b	= 1,
	PCIS_wr_m	= 0x00000002,
	PCIS_nmi_b	= 2,
	PCIS_nmi_m	= 0x00000004,
	PCIS_ii_b	= 3,
	PCIS_ii_m	= 0x00000008,
	PCIS_cwe_b	= 4,
	PCIS_cwe_m	= 0x00000010,
	PCIS_cre_b	= 5,
	PCIS_cre_m	= 0x00000020,
	PCIS_mdpe_b	= 6,
	PCIS_mdpe_m	= 0x00000040,
	PCIS_sta_b	= 7,
	PCIS_sta_m	= 0x00000080,
	PCIS_rta_b	= 8,
	PCIS_rta_m	= 0x00000100,
	PCIS_rma_b	= 9,
	PCIS_rma_m	= 0x00000200,
	PCIS_sse_b	= 10,
	PCIS_sse_m	= 0x00000400,
	PCIS_ose_b	= 11,
	PCIS_ose_m	= 0x00000800,
	PCIS_pe_b	= 12,
	PCIS_pe_m	= 0x00001000,
	PCIS_tae_b	= 13,
	PCIS_tae_m	= 0x00002000,
	PCIS_rle_b	= 14,
	PCIS_rle_m	= 0x00004000,
	PCIS_bme_b	= 15,
	PCIS_bme_m	= 0x00008000,
	PCIS_prd_b	= 16,
	PCIS_prd_m	= 0x00010000,
	PCIS_rip_b	= 17,
	PCIS_rip_m	= 0x00020000,
} ;

/*******************************************************************************
 *
 * PCI Status Mask Register
 *
 ******************************************************************************/
enum {
	PCISM_eed_b		= 0,
	PCISM_eed_m		= 0x00000001,
	PCISM_wr_b		= 1,
	PCISM_wr_m		= 0x00000002,
	PCISM_nmi_b		= 2,
	PCISM_nmi_m		= 0x00000004,
	PCISM_ii_b		= 3,
	PCISM_ii_m		= 0x00000008,
	PCISM_cwe_b		= 4,
	PCISM_cwe_m		= 0x00000010,
	PCISM_cre_b		= 5,
	PCISM_cre_m		= 0x00000020,
	PCISM_mdpe_b		= 6,
	PCISM_mdpe_m		= 0x00000040,
	PCISM_sta_b		= 7,
	PCISM_sta_m		= 0x00000080,
	PCISM_rta_b		= 8,
	PCISM_rta_m		= 0x00000100,
	PCISM_rma_b		= 9,
	PCISM_rma_m		= 0x00000200,
	PCISM_sse_b		= 10,
	PCISM_sse_m		= 0x00000400,
	PCISM_ose_b		= 11,
	PCISM_ose_m		= 0x00000800,
	PCISM_pe_b		= 12,
	PCISM_pe_m		= 0x00001000,
	PCISM_tae_b		= 13,
	PCISM_tae_m		= 0x00002000,
	PCISM_rle_b		= 14,
	PCISM_rle_m		= 0x00004000,
	PCISM_bme_b		= 15,
	PCISM_bme_m		= 0x00008000,
	PCISM_prd_b		= 16,
	PCISM_prd_m		= 0x00010000,
	PCISM_rip_b		= 17,
	PCISM_rip_m		= 0x00020000,
} ;

/*******************************************************************************
 *
 * PCI Configuration Address Register
 *
 ******************************************************************************/
enum {
	PCICFGA_reg_b		= 2,
	PCICFGA_reg_m		= 0x000000fc,
		PCICFGA_reg_id_v	= 0x00>>2, //use PCFGID_
		PCICFGA_reg_04_v	= 0x04>>2, //use PCFG04_
		PCICFGA_reg_08_v	= 0x08>>2, //use PCFG08_
		PCICFGA_reg_0C_v	= 0x0C>>2, //use PCFG0C_
		PCICFGA_reg_pba0_v	= 0x10>>2, //use PCIPBA_
		PCICFGA_reg_pba1_v	= 0x14>>2, //use PCIPBA_
		PCICFGA_reg_pba2_v	= 0x18>>2, //use PCIPBA_
		PCICFGA_reg_pba3_v	= 0x1c>>2, //use PCIPBA_
		PCICFGA_reg_subsystem_v = 0x2c>>2, //use PCFGSS_
		PCICFGA_reg_3C_v	= 0x3C>>2, //use PCFG3C_
		PCICFGA_reg_pba0c_v	= 0x44>>2, //use PCIPBAC_
		PCICFGA_reg_pba0m_v	= 0x48>>2,
		PCICFGA_reg_pba1c_v	= 0x4c>>2, //use PCIPBAC_
		PCICFGA_reg_pba1m_v	= 0x50>>2,
		PCICFGA_reg_pba2c_v	= 0x54>>2, //use PCIPBAC_
		PCICFGA_reg_pba2m_v	= 0x58>>2,
		PCICFGA_reg_pba3c_v	= 0x5c>>2, //use PCIPBAC_
		PCICFGA_reg_pba3m_v	= 0x60>>2,
		PCICFGA_reg_pmgt_v	= 0x64>>2,
	PCICFGA_func_b		= 8,
	PCICFGA_func_m		= 0x00000700,
	PCICFGA_dev_b		= 11,
	PCICFGA_dev_m		= 0x0000f800,
		PCICFGA_dev_internal_v	= 0,
	PCICFGA_bus_b		= 16,
	PCICFGA_bus_m		= 0x00ff0000,
		PCICFGA_bus_type0_v	= 0,	//local bus
	PCICFGA_en_b		= 31,		// read only
	PCICFGA_en_m		= 0x80000000,
} ;

enum {
	PCFGID_vendor_b 	= 0,
	PCFGID_vendor_m 	= 0x0000ffff,
		PCFGID_vendor_IDT_v		= 0x111d,
	PCFGID_device_b 	= 16,
	PCFGID_device_m 	= 0xffff0000,
		PCFGID_device_Korinade_v	= 0x0214,

	PCFG04_command_ioena_b		= 1,
	PCFG04_command_ioena_m		= 0x00000001,
	PCFG04_command_memena_b 	= 2,
	PCFG04_command_memena_m 	= 0x00000002,
	PCFG04_command_bmena_b		= 3,
	PCFG04_command_bmena_m		= 0x00000004,
	PCFG04_command_mwinv_b		= 5,
	PCFG04_command_mwinv_m		= 0x00000010,
	PCFG04_command_parena_b 	= 7,
	PCFG04_command_parena_m 	= 0x00000040,
	PCFG04_command_serrena_b	= 9,
	PCFG04_command_serrena_m	= 0x00000100,
	PCFG04_command_fastbbena_b	= 10,
	PCFG04_command_fastbbena_m	= 0x00000200,
	PCFG04_status_b 		= 16,
	PCFG04_status_m 		= 0xffff0000,
	PCFG04_status_66MHz_b		= 21,	// 66 MHz enable
	PCFG04_status_66MHz_m		= 0x00200000,
	PCFG04_status_fbb_b		= 23,
	PCFG04_status_fbb_m		= 0x00800000,
	PCFG04_status_mdpe_b		= 24,
	PCFG04_status_mdpe_m		= 0x01000000,
	PCFG04_status_dst_b		= 25,
	PCFG04_status_dst_m		= 0x06000000,
	PCFG04_status_sta_b		= 27,
	PCFG04_status_sta_m		= 0x08000000,
	PCFG04_status_rta_b		= 28,
	PCFG04_status_rta_m		= 0x10000000,
	PCFG04_status_rma_b		= 29,
	PCFG04_status_rma_m		= 0x20000000,
	PCFG04_status_sse_b		= 30,
	PCFG04_status_sse_m		= 0x40000000,
	PCFG04_status_pe_b		= 31,
	PCFG04_status_pe_m		= 0x40000000,

	PCFG08_revId_b			= 0,
	PCFG08_revId_m			= 0x000000ff,
	PCFG08_classCode_b		= 0,
	PCFG08_classCode_m		= 0xffffff00,
		PCFG08_classCode_bridge_v	= 06,
		PCFG08_classCode_proc_v 	= 0x0b3000, // processor-MIPS
	PCFG0C_cacheline_b		= 0,
	PCFG0C_cacheline_m		= 0x000000ff,
	PCFG0C_masterLatency_b		= 8,
	PCFG0C_masterLatency_m		= 0x0000ff00,
	PCFG0C_headerType_b		= 16,
	PCFG0C_headerType_m		= 0x00ff0000,
	PCFG0C_bist_b			= 24,
	PCFG0C_bist_m			= 0xff000000,

	PCIPBA_msi_b			= 0,
	PCIPBA_msi_m			= 0x00000001,
	PCIPBA_p_b			= 3,
	PCIPBA_p_m			= 0x00000004,
	PCIPBA_baddr_b			= 8,
	PCIPBA_baddr_m			= 0xffffff00,

	PCFGSS_vendorId_b		= 0,
	PCFGSS_vendorId_m		= 0x0000ffff,
	PCFGSS_id_b			= 16,
	PCFGSS_id_m			= 0xffff0000,

	PCFG3C_interruptLine_b		= 0,
	PCFG3C_interruptLine_m		= 0x000000ff,
	PCFG3C_interruptPin_b		= 8,
	PCFG3C_interruptPin_m		= 0x0000ff00,
	PCFG3C_minGrant_b		= 16,
	PCFG3C_minGrant_m		= 0x00ff0000,
	PCFG3C_maxLat_b 		= 24,
	PCFG3C_maxLat_m 		= 0xff000000,

	PCIPBAC_msi_b			= 0,
	PCIPBAC_msi_m			= 0x00000001,
	PCIPBAC_p_b			= 1,
	PCIPBAC_p_m			= 0x00000002,
	PCIPBAC_size_b			= 2,
	PCIPBAC_size_m			= 0x0000007c,
	PCIPBAC_sb_b			= 7,
	PCIPBAC_sb_m			= 0x00000080,
	PCIPBAC_pp_b			= 8,
	PCIPBAC_pp_m			= 0x00000100,
	PCIPBAC_mr_b			= 9,
	PCIPBAC_mr_m			= 0x00000600,
		PCIPBAC_mr_read_v	=0,	//no prefetching
		PCIPBAC_mr_readLine_v	=1,
		PCIPBAC_mr_readMult_v	=2,
	PCIPBAC_mrl_b			= 11,
	PCIPBAC_mrl_m			= 0x00000800,
	PCIPBAC_mrm_b			= 12,
	PCIPBAC_mrm_m			= 0x00001000,
	PCIPBAC_trp_b			= 13,
	PCIPBAC_trp_m			= 0x00002000,

	PCFG40_trdyTimeout_b		= 0,
	PCFG40_trdyTimeout_m		= 0x000000ff,
	PCFG40_retryLim_b		= 8,
	PCFG40_retryLim_m		= 0x0000ff00,
};

/*******************************************************************************
 *
 * PCI Local Base Address [0|1|2|3] Register
 *
 ******************************************************************************/
enum {
	PCILBA_baddr_b		= 0,		// In PCI_t -> pcilba [] .a
	PCILBA_baddr_m		= 0xffffff00,
} ;
/*******************************************************************************
 *
 * PCI Local Base Address Control Register
 *
 ******************************************************************************/
enum {
	PCILBAC_msi_b		= 0,		// In pPci->pcilba[i].c
	PCILBAC_msi_m		= 0x00000001,
		PCILBAC_msi_mem_v	= 0,
		PCILBAC_msi_io_v	= 1,
	PCILBAC_size_b		= 2,	// In pPci->pcilba[i].c
	PCILBAC_size_m		= 0x0000007c,
	PCILBAC_sb_b		= 7,	// In pPci->pcilba[i].c
	PCILBAC_sb_m		= 0x00000080,
	PCILBAC_rt_b		= 8,	// In pPci->pcilba[i].c
	PCILBAC_rt_m		= 0x00000100,
		PCILBAC_rt_noprefetch_v = 0, // mem read
		PCILBAC_rt_prefetch_v	= 1, // mem readline
} ;

/*******************************************************************************
 *
 * PCI Local Base Address [0|1|2|3] Mapping Register
 *
 ******************************************************************************/
enum {
	PCILBAM_maddr_b 	= 8,
	PCILBAM_maddr_m 	= 0xffffff00,
} ;

/*******************************************************************************
 *
 * PCI Decoupled Access Control Register
 *
 ******************************************************************************/
enum {
	PCIDAC_den_b		= 0,
	PCIDAC_den_m		= 0x00000001,
} ;

/*******************************************************************************
 *
 * PCI Decoupled Access Status Register
 *
 ******************************************************************************/
enum {
	PCIDAS_d_b	= 0,
	PCIDAS_d_m	= 0x00000001,
	PCIDAS_b_b	= 1,
	PCIDAS_b_m	= 0x00000002,
	PCIDAS_e_b	= 2,
	PCIDAS_e_m	= 0x00000004,
	PCIDAS_ofe_b	= 3,
	PCIDAS_ofe_m	= 0x00000008,
	PCIDAS_off_b	= 4,
	PCIDAS_off_m	= 0x00000010,
	PCIDAS_ife_b	= 5,
	PCIDAS_ife_m	= 0x00000020,
	PCIDAS_iff_b	= 6,
	PCIDAS_iff_m	= 0x00000040,
} ;

/*******************************************************************************
 *
 * PCI DMA Channel 8 Configuration Register
 *
 ******************************************************************************/
enum
{
	PCIDMA8C_mbs_b	= 0,		// Maximum Burst Size.
	PCIDMA8C_mbs_m	= 0x00000fff,	// { pcidma8c }
	PCIDMA8C_our_b	= 12,		// Optimize Unaligned Burst Reads.
	PCIDMA8C_our_m	= 0x00001000,	// { pcidma8c }
} ;

/*******************************************************************************
 *
 * PCI DMA Channel 9 Configuration Register
 *
 ******************************************************************************/
enum
{
	PCIDMA9C_mbs_b	= 0,		// Maximum Burst Size.
	PCIDMA9C_mbs_m	= 0x00000fff, // { pcidma9c }
} ;

/*******************************************************************************
 *
 * PCI to Memory(DMA Channel 8) AND Memory to PCI DMA(DMA Channel 9)Descriptors
 *
 ******************************************************************************/
enum {
	PCIDMAD_pt_b		= 22,		// in DEVCMD field (descriptor)
	PCIDMAD_pt_m		= 0x00c00000,	// preferred transaction field
		// These are for reads (DMA channel 8)
		PCIDMAD_devcmd_mr_v	= 0,	//memory read
		PCIDMAD_devcmd_mrl_v	= 1,	//memory read line
		PCIDMAD_devcmd_mrm_v	= 2,	//memory read multiple
		PCIDMAD_devcmd_ior_v	= 3,	//I/O read
		// These are for writes (DMA channel 9)
		PCIDMAD_devcmd_mw_v	= 0,	//memory write
		PCIDMAD_devcmd_mwi_v	= 1,	//memory write invalidate
		PCIDMAD_devcmd_iow_v	= 3,	//I/O write

	// Swap byte field applies to both DMA channel 8 and 9
	PCIDMAD_sb_b		= 24,		// in DEVCMD field (descriptor)
	PCIDMAD_sb_m		= 0x01000000,	// swap byte field
} ;


/*******************************************************************************
 *
 * PCI Target Control Register
 *
 ******************************************************************************/
enum
{
	PCITC_rtimer_b		= 0,		// In PCITC_t -> pcitc
	PCITC_rtimer_m		= 0x000000ff,
	PCITC_dtimer_b		= 8,		// In PCITC_t -> pcitc
	PCITC_dtimer_m		= 0x0000ff00,
	PCITC_rdr_b		= 18,		// In PCITC_t -> pcitc
	PCITC_rdr_m		= 0x00040000,
	PCITC_ddt_b		= 19,		// In PCITC_t -> pcitc
	PCITC_ddt_m		= 0x00080000,
} ;
/*******************************************************************************
 *
 * PCI messaging unit [applies to both inbound and outbound registers ]
 *
 ******************************************************************************/
enum
{
	PCIM_m0_b	= 0,		// In PCIM_t -> {pci{iic,iim,ioic,ioim}}
	PCIM_m0_m	= 0x00000001,	// inbound or outbound message 0
	PCIM_m1_b	= 1,		// In PCIM_t -> {pci{iic,iim,ioic,ioim}}
	PCIM_m1_m	= 0x00000002,	// inbound or outbound message 1
	PCIM_db_b	= 2,		// In PCIM_t -> {pci{iic,iim,ioic,ioim}}
	PCIM_db_m	= 0x00000004,	// inbound or outbound doorbell
};






#define PCI_MSG_VirtualAddress	     0xB8088010
#define rc32434_pci ((volatile PCI_t) PCI0_VirtualAddress)
#define rc32434_pci_msg ((volatile PCIM_t) PCI_MSG_VirtualAddress)

#define PCIM_SHFT		0x6
#define PCIM_BIT_LEN		0x7
#define PCIM_H_EA		0x3
#define PCIM_H_IA_FIX		0x4
#define PCIM_H_IA_RR		0x5

#define PCI_ADDR_START		0x50000000

#define CPUTOPCI_MEM_WIN	0x02000000
#define CPUTOPCI_IO_WIN		0x00100000
#define PCILBA_SIZE_SHFT	2
#define PCILBA_SIZE_MASK	0x1F
#define SIZE_256MB		0x1C
#define SIZE_128MB		0x1B
#define SIZE_64MB               0x1A
#define SIZE_32MB		0x19
#define SIZE_16MB               0x18
#define SIZE_4MB		0x16
#define SIZE_2MB		0x15
#define SIZE_1MB		0x14
#define KORINA_CONFIG0_ADDR	0x80000000
#define KORINA_CONFIG1_ADDR	0x80000004
#define KORINA_CONFIG2_ADDR	0x80000008
#define KORINA_CONFIG3_ADDR	0x8000000C
#define KORINA_CONFIG4_ADDR	0x80000010
#define KORINA_CONFIG5_ADDR	0x80000014
#define KORINA_CONFIG6_ADDR	0x80000018
#define KORINA_CONFIG7_ADDR	0x8000001C
#define KORINA_CONFIG8_ADDR	0x80000020
#define KORINA_CONFIG9_ADDR	0x80000024
#define KORINA_CONFIG10_ADDR	0x80000028
#define KORINA_CONFIG11_ADDR	0x8000002C
#define KORINA_CONFIG12_ADDR	0x80000030
#define KORINA_CONFIG13_ADDR	0x80000034
#define KORINA_CONFIG14_ADDR	0x80000038
#define KORINA_CONFIG15_ADDR	0x8000003C
#define KORINA_CONFIG16_ADDR	0x80000040
#define KORINA_CONFIG17_ADDR	0x80000044
#define KORINA_CONFIG18_ADDR	0x80000048
#define KORINA_CONFIG19_ADDR	0x8000004C
#define KORINA_CONFIG20_ADDR	0x80000050
#define KORINA_CONFIG21_ADDR	0x80000054
#define KORINA_CONFIG22_ADDR	0x80000058
#define KORINA_CONFIG23_ADDR	0x8000005C
#define KORINA_CONFIG24_ADDR	0x80000060
#define KORINA_CONFIG25_ADDR	0x80000064
#define KORINA_CMD 		(PCFG04_command_ioena_m | \
				 PCFG04_command_memena_m | \
				 PCFG04_command_bmena_m | \
				 PCFG04_command_mwinv_m | \
				 PCFG04_command_parena_m | \
				 PCFG04_command_serrena_m )

#define KORINA_STAT		(PCFG04_status_mdpe_m | \
				 PCFG04_status_sta_m  | \
				 PCFG04_status_rta_m  | \
				 PCFG04_status_rma_m  | \
				 PCFG04_status_sse_m  | \
				 PCFG04_status_pe_m)

#define KORINA_CNFG1		((KORINA_STAT<<16)|KORINA_CMD)

#define KORINA_REVID		0
#define KORINA_CLASS_CODE	0
#define KORINA_CNFG2		((KORINA_CLASS_CODE<<8) | \
				  KORINA_REVID)

#define KORINA_CACHE_LINE_SIZE	4
#define KORINA_MASTER_LAT	0x3c
#define KORINA_HEADER_TYPE	0
#define KORINA_BIST		0

#define KORINA_CNFG3 ((KORINA_BIST << 24) | \
		      (KORINA_HEADER_TYPE<<16) | \
		      (KORINA_MASTER_LAT<<8) | \
		      KORINA_CACHE_LINE_SIZE )

#define KORINA_BAR0	0x00000008 /* 128 MB Memory */
#define KORINA_BAR1	0x18800001 /* 1 MB IO */
#define KORINA_BAR2	0x18000001 /* 2 MB IO window for Korina
					internal Registers */
#define KORINA_BAR3	0x48000008 /* Spare 128 MB Memory */

#define KORINA_CNFG4	KORINA_BAR0
#define KORINA_CNFG5    KORINA_BAR1
#define KORINA_CNFG6 	KORINA_BAR2
#define KORINA_CNFG7	KORINA_BAR3

#define KORINA_SUBSYS_VENDOR_ID 0x011d
#define KORINA_SUBSYSTEM_ID	0x0214
#define KORINA_CNFG8		0
#define KORINA_CNFG9		0
#define KORINA_CNFG10		0
#define KORINA_CNFG11 	((KORINA_SUBSYS_VENDOR_ID<<16) | \
			  KORINA_SUBSYSTEM_ID)
#define KORINA_INT_LINE		1
#define KORINA_INT_PIN		1
#define KORINA_MIN_GNT		8
#define KORINA_MAX_LAT		0x38
#define KORINA_CNFG12		0
#define KORINA_CNFG13 		0
#define KORINA_CNFG14		0
#define KORINA_CNFG15	((KORINA_MAX_LAT<<24) | \
			 (KORINA_MIN_GNT<<16) | \
			 (KORINA_INT_PIN<<8)  | \
			  KORINA_INT_LINE)
#define	KORINA_RETRY_LIMIT	0x80
#define KORINA_TRDY_LIMIT	0x80
#define KORINA_CNFG16 ((KORINA_RETRY_LIMIT<<8) | \
			KORINA_TRDY_LIMIT)
#define PCI_PBAxC_R		0x0
#define PCI_PBAxC_RL		0x1
#define PCI_PBAxC_RM		0x2
#define SIZE_SHFT		2

#if defined(__MIPSEB__)
#define KORINA_PBA0C	( PCIPBAC_mrl_m | PCIPBAC_sb_m | \
			  ((PCI_PBAxC_RM &0x3) << PCIPBAC_mr_b) | \
			  PCIPBAC_pp_m | \
			  (SIZE_128MB<<SIZE_SHFT) | \
			   PCIPBAC_p_m)
#else
#define KORINA_PBA0C	( PCIPBAC_mrl_m | \
			  ((PCI_PBAxC_RM &0x3) << PCIPBAC_mr_b) | \
			  PCIPBAC_pp_m | \
			  (SIZE_128MB<<SIZE_SHFT) | \
			   PCIPBAC_p_m)
#endif
#define KORINA_CNFG17	KORINA_PBA0C
#define KORINA_PBA0M	0x0
#define KORINA_CNFG18	KORINA_PBA0M

#if defined(__MIPSEB__)
#define KORINA_PBA1C	((SIZE_1MB<<SIZE_SHFT) | PCIPBAC_sb_m | \
			  PCIPBAC_msi_m)
#else
#define KORINA_PBA1C	((SIZE_1MB<<SIZE_SHFT) | \
			  PCIPBAC_msi_m)
#endif
#define KORINA_CNFG19	KORINA_PBA1C
#define KORINA_PBA1M	0x0
#define KORINA_CNFG20	KORINA_PBA1M

#if defined(__MIPSEB__)
#define KORINA_PBA2C	((SIZE_2MB<<SIZE_SHFT) | PCIPBAC_sb_m | \
			  PCIPBAC_msi_m)
#else
#define KORINA_PBA2C	((SIZE_2MB<<SIZE_SHFT) | \
			  PCIPBAC_msi_m)
#endif
#define KORINA_CNFG21	KORINA_PBA2C
#define KORINA_PBA2M	0x18000000
#define KORINA_CNFG22	KORINA_PBA2M
#define KORINA_PBA3C	0
#define KORINA_CNFG23	KORINA_PBA3C
#define KORINA_PBA3M	0
#define KORINA_CNFG24	KORINA_PBA3M



#define	PCITC_DTIMER_VAL	8
#define PCITC_RTIMER_VAL	0x10




#endif	// __IDT_PCI_H__



