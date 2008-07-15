#ifndef	__IDT_ETH_H__
#define	__IDT_ETH_H__

/*******************************************************************************
 *
 * Copyright 2002 Integrated Device Technology, Inc.
 *		All rights reserved.
 *
 * Ethernet register definition.
 *
 * File   : $Id: eth.h,v 1.3 2002/06/06 18:34:04 astichte Exp $
 *
 * Author : Allen.Stichter@idt.com
 * Date   : 20020605
 * Update :
 *	    $Log: eth.h,v $
 *	    Revision 1.3  2002/06/06 18:34:04  astichte
 *	    Added XXX_PhysicalAddress and XXX_VirtualAddress
 *	
 *	    Revision 1.2  2002/06/05 18:19:46  astichte
 *	    Added
 *	
 *	    Revision 1.1  2002/05/29 17:33:22  sysarch
 *	    jba File moved from vcode/include/idt/acacia
 *
 ******************************************************************************/

enum
{
	ETH0_PhysicalAddress	= 0x18060000,
	ETH_PhysicalAddress	= ETH0_PhysicalAddress,		// Default

	ETH0_VirtualAddress	= 0xb8060000,
	ETH_VirtualAddress	= ETH0_VirtualAddress,		// Default
} ;

typedef struct
{
	u32 ethintfc		;
	u32 ethfifott		;
	u32 etharc		;
	u32 ethhash0		;
	u32 ethhash1		;
	u32 ethu0 [4]		;	// Reserved.	
	u32 ethpfs		;
	u32 ethmcp		;
	u32 eth_u1 [10]		;	// Reserved.
	u32 ethspare		;
	u32 eth_u2 [42]		;	// Reserved. 
	u32 ethsal0		;
	u32 ethsah0		;
	u32 ethsal1		;
	u32 ethsah1		;
	u32 ethsal2		;
	u32 ethsah2		;
	u32 ethsal3		;
	u32 ethsah3		;
	u32 ethrbc		;
	u32 ethrpc		;
	u32 ethrupc		;
	u32 ethrfc		;
	u32 ethtbc		;
	u32 ethgpf		;
	u32 eth_u9 [50]		;	// Reserved.	
	u32 ethmac1		;
	u32 ethmac2		;
	u32 ethipgt		;
	u32 ethipgr		;
	u32 ethclrt		;
	u32 ethmaxf		;
	u32 eth_u10		;	// Reserved.	
	u32 ethmtest		;
	u32 miimcfg		;
	u32 miimcmd		;
	u32 miimaddr		;
	u32 miimwtd		;
	u32 miimrdd		;
	u32 miimind		;
	u32 eth_u11		;	// Reserved.
	u32 eth_u12		;	// Reserved.
	u32 ethcfsa0		;
	u32 ethcfsa1		;
	u32 ethcfsa2		;
} volatile *ETH_t;

enum
{
	ETHINTFC_en_b		= 0,
	ETHINTFC_en_m		= 0x00000001,
	ETHINTFC_its_b		= 1,
	ETHINTFC_its_m		= 0x00000002,
	ETHINTFC_rip_b		= 2,
	ETHINTFC_rip_m		= 0x00000004,
	ETHINTFC_jam_b		= 3,
	ETHINTFC_jam_m		= 0x00000008,
	ETHINTFC_ovr_b		= 4,
	ETHINTFC_ovr_m		= 0x00000010,
	ETHINTFC_und_b		= 5,
	ETHINTFC_und_m		= 0x00000020,
	ETHINTFC_iom_b 		= 6,
	ETHINTFC_iom_m		= 0x000000c0,

	ETHFIFOTT_tth_b		= 0,
	ETHFIFOTT_tth_m		= 0x0000007f,

	ETHARC_pro_b		= 0,
	ETHARC_pro_m		= 0x00000001,
	ETHARC_am_b		= 1,
	ETHARC_am_m		= 0x00000002,
	ETHARC_afm_b		= 2,
	ETHARC_afm_m		= 0x00000004,
	ETHARC_ab_b		= 3,
	ETHARC_ab_m		= 0x00000008,

	ETHSAL_byte5_b		= 0,
	ETHSAL_byte5_m		= 0x000000ff,
	ETHSAL_byte4_b		= 8,
	ETHSAL_byte4_m		= 0x0000ff00,
	ETHSAL_byte3_b		= 16,
	ETHSAL_byte3_m		= 0x00ff0000,
	ETHSAL_byte2_b		= 24,
	ETHSAL_byte2_m		= 0xff000000,

	ETHSAH_byte1_b		= 0,
	ETHSAH_byte1_m		= 0x000000ff,
	ETHSAH_byte0_b		= 8,
	ETHSAH_byte0_m		= 0x0000ff00,
	
	ETHGPF_ptv_b		= 0,
	ETHGPF_ptv_m		= 0x0000ffff,

	ETHPFS_pfd_b		= 0,
	ETHPFS_pfd_m		= 0x00000001,

	ETHCFSA0_cfsa4_b	= 0,
	ETHCFSA0_cfsa4_m	= 0x000000ff,
	ETHCFSA0_cfsa5_b	= 8,
	ETHCFSA0_cfsa5_m	= 0x0000ff00,

	ETHCFSA1_cfsa2_b	= 0,
	ETHCFSA1_cfsa2_m	= 0x000000ff,
	ETHCFSA1_cfsa3_b	= 8,
	ETHCFSA1_cfsa3_m	= 0x0000ff00,

	ETHCFSA2_cfsa0_b	= 0,
	ETHCFSA2_cfsa0_m	= 0x000000ff,
	ETHCFSA2_cfsa1_b	= 8,
	ETHCFSA2_cfsa1_m	= 0x0000ff00,

	ETHMAC1_re_b		= 0,
	ETHMAC1_re_m		= 0x00000001,
	ETHMAC1_paf_b		= 1,
	ETHMAC1_paf_m		= 0x00000002,
	ETHMAC1_rfc_b		= 2,
	ETHMAC1_rfc_m		= 0x00000004,
	ETHMAC1_tfc_b		= 3,
	ETHMAC1_tfc_m		= 0x00000008,
	ETHMAC1_lb_b		= 4,
	ETHMAC1_lb_m		= 0x00000010,
	ETHMAC1_mr_b		= 31,
	ETHMAC1_mr_m		= 0x80000000,

	ETHMAC2_fd_b		= 0,
	ETHMAC2_fd_m		= 0x00000001,
	ETHMAC2_flc_b		= 1,
	ETHMAC2_flc_m		= 0x00000002,
	ETHMAC2_hfe_b		= 2,
	ETHMAC2_hfe_m		= 0x00000004,
	ETHMAC2_dc_b		= 3,
	ETHMAC2_dc_m		= 0x00000008,
	ETHMAC2_cen_b		= 4,
	ETHMAC2_cen_m		= 0x00000010,
	ETHMAC2_pe_b		= 5,
	ETHMAC2_pe_m		= 0x00000020,
	ETHMAC2_vpe_b		= 6,
	ETHMAC2_vpe_m		= 0x00000040,
	ETHMAC2_ape_b		= 7,
	ETHMAC2_ape_m		= 0x00000080,
	ETHMAC2_ppe_b		= 8,
	ETHMAC2_ppe_m		= 0x00000100,
	ETHMAC2_lpe_b		= 9,
	ETHMAC2_lpe_m		= 0x00000200,
	ETHMAC2_nb_b		= 12,
	ETHMAC2_nb_m		= 0x00001000,
	ETHMAC2_bp_b		= 13,
	ETHMAC2_bp_m		= 0x00002000,
	ETHMAC2_ed_b		= 14,
	ETHMAC2_ed_m		= 0x00004000,

	ETHIPGT_ipgt_b		= 0,
	ETHIPGT_ipgt_m		= 0x0000007f,

	ETHIPGR_ipgr2_b		= 0,
	ETHIPGR_ipgr2_m		= 0x0000007f,
	ETHIPGR_ipgr1_b		= 8,
	ETHIPGR_ipgr1_m		= 0x00007f00,

	ETHCLRT_maxret_b	= 0,
	ETHCLRT_maxret_m	= 0x0000000f,
	ETHCLRT_colwin_b	= 8,
	ETHCLRT_colwin_m	= 0x00003f00,

	ETHMAXF_maxf_b		= 0,
	ETHMAXF_maxf_m		= 0x0000ffff,

	ETHMTEST_tb_b		= 2,
	ETHMTEST_tb_m		= 0x00000004,

	ETHMCP_div_b		= 0,
	ETHMCP_div_m		= 0x000000ff,
	
	MIIMCFG_rsv_b		= 0,
	MIIMCFG_rsv_m		= 0x0000000c,

	MIIMCMD_rd_b		= 0,
	MIIMCMD_rd_m		= 0x00000001,
	MIIMCMD_scn_b		= 1,
	MIIMCMD_scn_m		= 0x00000002,

	MIIMADDR_regaddr_b	= 0,
	MIIMADDR_regaddr_m	= 0x0000001f,
	MIIMADDR_phyaddr_b	= 8,
	MIIMADDR_phyaddr_m	= 0x00001f00,

	MIIMWTD_wdata_b		= 0,
	MIIMWTD_wdata_m		= 0x0000ffff,

	MIIMRDD_rdata_b		= 0,
	MIIMRDD_rdata_m		= 0x0000ffff,

	MIIMIND_bsy_b		= 0,
	MIIMIND_bsy_m		= 0x00000001,
	MIIMIND_scn_b		= 1,
	MIIMIND_scn_m		= 0x00000002,
	MIIMIND_nv_b		= 2,
	MIIMIND_nv_m		= 0x00000004,

} ;

/*
 * Values for the DEVCS field of the Ethernet DMA Rx and Tx descriptors.
 */
enum
{
	ETHRX_fd_b		= 0,
	ETHRX_fd_m		= 0x00000001,
	ETHRX_ld_b		= 1,
	ETHRX_ld_m		= 0x00000002,
	ETHRX_rok_b		= 2,
	ETHRX_rok_m		= 0x00000004,
	ETHRX_fm_b		= 3,
	ETHRX_fm_m		= 0x00000008,
	ETHRX_mp_b		= 4,
	ETHRX_mp_m		= 0x00000010,
	ETHRX_bp_b		= 5,
	ETHRX_bp_m		= 0x00000020,
	ETHRX_vlt_b		= 6,
	ETHRX_vlt_m		= 0x00000040,
	ETHRX_cf_b		= 7,
	ETHRX_cf_m		= 0x00000080,
	ETHRX_ovr_b		= 8,
	ETHRX_ovr_m		= 0x00000100,
	ETHRX_crc_b		= 9,
	ETHRX_crc_m		= 0x00000200,
	ETHRX_cv_b		= 10,
	ETHRX_cv_m		= 0x00000400,
	ETHRX_db_b		= 11,
	ETHRX_db_m		= 0x00000800,
	ETHRX_le_b		= 12,
	ETHRX_le_m		= 0x00001000,
	ETHRX_lor_b		= 13,
	ETHRX_lor_m		= 0x00002000,
	ETHRX_ces_b		= 14,
	ETHRX_ces_m		= 0x00004000,
	ETHRX_length_b		= 16,
	ETHRX_length_m		= 0xffff0000,

	ETHTX_fd_b		= 0,
	ETHTX_fd_m		= 0x00000001,
	ETHTX_ld_b		= 1,
	ETHTX_ld_m		= 0x00000002,
	ETHTX_oen_b		= 2,
	ETHTX_oen_m		= 0x00000004,
	ETHTX_pen_b		= 3,
	ETHTX_pen_m		= 0x00000008,
	ETHTX_cen_b		= 4,
	ETHTX_cen_m		= 0x00000010,
	ETHTX_hen_b		= 5,
	ETHTX_hen_m		= 0x00000020,
	ETHTX_tok_b		= 6,
	ETHTX_tok_m		= 0x00000040,
	ETHTX_mp_b		= 7,
	ETHTX_mp_m		= 0x00000080,
	ETHTX_bp_b		= 8,
	ETHTX_bp_m		= 0x00000100,
	ETHTX_und_b		= 9,
	ETHTX_und_m		= 0x00000200,
	ETHTX_of_b		= 10,
	ETHTX_of_m		= 0x00000400,
	ETHTX_ed_b		= 11,
	ETHTX_ed_m		= 0x00000800,
	ETHTX_ec_b		= 12,
	ETHTX_ec_m		= 0x00001000,
	ETHTX_lc_b		= 13,
	ETHTX_lc_m		= 0x00002000,
	ETHTX_td_b		= 14,
	ETHTX_td_m		= 0x00004000,
	ETHTX_crc_b		= 15,
	ETHTX_crc_m		= 0x00008000,
	ETHTX_le_b		= 16,
	ETHTX_le_m		= 0x00010000,
	ETHTX_cc_b		= 17,
	ETHTX_cc_m		= 0x001E0000,
} ;

#endif	// __IDT_ETH_H__




