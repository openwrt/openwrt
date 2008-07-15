#ifndef __IDT_DDR_H__
#define __IDT_DDR_H__

/*******************************************************************************
 *
 * Copyright 2002 Integrated Device Technology, Inc.
 *		All rights reserved.
 *
 * DDR register definition.
 *
 * File   : $Id: ddr.h,v 1.2 2002/06/06 18:34:03 astichte Exp $
 *
 * Author : ryan.holmQVist@idt.com
 * Date   : 20011005
 * Update :
 *          $Log: ddr.h,v $
 *          Revision 1.2  2002/06/06 18:34:03  astichte
 *          Added XXX_PhysicalAddress and XXX_VirtualAddress
 *
 *          Revision 1.1  2002/05/29 17:33:21  sysarch
 *          jba File moved from vcode/include/idt/acacia
 *
 *
 ******************************************************************************/

enum
{
	DDR0_PhysicalAddress	= 0x18018000,
	DDR_PhysicalAddress	= DDR0_PhysicalAddress,		// Default

	DDR0_VirtualAddress	= 0xb8018000,
	DDR_VirtualAddress	= DDR0_VirtualAddress,		// Default
} ;

typedef struct DDR_s
{
	u32	ddrbase ;
	u32	ddrmask ;
	u32	res1;
	u32	res2;
	u32	ddrc ;
	u32	ddrabase ;
	u32	ddramask ;
	u32	ddramap ;
	u32	ddrcust;
	u32	ddrrdc;
	u32	ddrspare;
} volatile *DDR_t ;

enum
{
	DDR0BASE_baseaddr_b	= 16,
	DDR0BASE_baseaddr_m	= 0xffff0000,

	DDR0MASK_mask_b		= 16,
	DDR0MASK_mask_m		= 0xffff0000,

	DDR1BASE_baseaddr_b	= 16,
	DDR1BASE_baseaddr_m	= 0xffff0000,

	DDR1MASK_mask_b		= 16,
	DDR1MASK_mask_m		= 0xffff0000,

	DDRC_ata_b		= 5,
	DDRC_ata_m		= 0x000000E0,
	DDRC_dbw_b		= 8,
	DDRC_dbw_m		= 0x00000100,
	DDRC_wr_b		= 9,
	DDRC_wr_m		= 0x00000600,
	DDRC_ps_b		= 11,
	DDRC_ps_m		= 0x00001800,
	DDRC_dtype_b		= 13,
	DDRC_dtype_m		= 0x0000e000,
	DDRC_rfc_b		= 16,
	DDRC_rfc_m		= 0x000f0000,
	DDRC_rp_b		= 20,
	DDRC_rp_m		= 0x00300000,
	DDRC_ap_b		= 22,
	DDRC_ap_m		= 0x00400000,
	DDRC_rcd_b		= 23,
	DDRC_rcd_m		= 0x01800000,
	DDRC_cl_b		= 25,
	DDRC_cl_m		= 0x06000000,
	DDRC_dbm_b		= 27,
	DDRC_dbm_m		= 0x08000000,
	DDRC_sds_b		= 28,
	DDRC_sds_m		= 0x10000000,
	DDRC_atp_b		= 29,
	DDRC_atp_m		= 0x60000000,
	DDRC_re_b		= 31,
	DDRC_re_m		= 0x80000000,

	DDRRDC_ces_b		= 0,
	DDRRDC_ces_m		= 0x00000001,
	DDRRDC_ace_b		= 1,
	DDRRDC_ace_m		= 0x00000002,

	DDRABASE_baseaddr_b	= 16,
	DDRABASE_baseaddr_m	= 0xffff0000,

	DDRAMASK_mask_b		= 16,
	DDRAMASK_mask_m		= 0xffff0000,

	DDRAMAP_map_b		= 16,
	DDRAMAP_map_m		= 0xffff0000,

	DDRCUST_cs_b		= 0,
	DDRCUST_cs_m		= 0x00000003,
	DDRCUST_we_b		= 2,
	DDRCUST_we_m		= 0x00000004,
	DDRCUST_ras_b		= 3,
	DDRCUST_ras_m		= 0x00000008,
	DDRCUST_cas_b		= 4,
	DDRCUST_cas_m		= 0x00000010,
	DDRCUST_cke_b		= 5,
	DDRCUST_cke_m		= 0x00000020,
	DDRCUST_ba_b		= 6,
	DDRCUST_ba_m		= 0x000000c0,

	RCOUNT_rcount_b		= 0,
	RCOUNT_rcount_m		= 0x0000ffff,

	RCOMPARE_rcompare_b	= 0,
	RCOMPARE_rcompare_m	= 0x0000ffff,

	RTC_ce_b		= 0,
	RTC_ce_m		= 0x00000001,
	RTC_to_b		= 1,
	RTC_to_m		= 0x00000002,
	RTC_rqe_b		= 2,
	RTC_rqe_m		= 0x00000004,

	DDRDQSC_dm_b		= 0,
	DDRDQSC_dm_m		= 0x00000003,
	DDRDQSC_dqsbs_b		= 2,
	DDRDQSC_dqsbs_m		= 0x000000fc,
	DDRDQSC_db_b		= 8,
	DDRDQSC_db_m		= 0x00000100,
	DDRDQSC_dbsp_b		= 9,
	DDRDQSC_dbsp_m		= 0x01fffe00,
	DDRDQSC_bdp_b		= 25,
	DDRDQSC_bdp_m		= 0x7e000000,

	DDRDLLC_eao_b		= 0,
	DDRDLLC_eao_m		= 0x00000001,
	DDRDLLC_eo_b		= 1,
	DDRDLLC_eo_m		= 0x0000003e,
	DDRDLLC_fs_b		= 6,
	DDRDLLC_fs_m		= 0x000000c0,
	DDRDLLC_as_b		= 8,
	DDRDLLC_as_m		= 0x00000700,
	DDRDLLC_sp_b		= 11,
	DDRDLLC_sp_m		= 0x001ff800,

	DDRDLLFC_men_b		= 0,
	DDRDLLFC_men_m		= 0x00000001,
	DDRDLLFC_aen_b		= 1,
	DDRDLLFC_aen_m		= 0x00000002,
	DDRDLLFC_ff_b		= 2,
	DDRDLLFC_ff_m		= 0x00000004,

	DDRDLLTA_addr_b		= 2,
	DDRDLLTA_addr_m		= 0xfffffffc,

	DDRDLLED_dbe_b		= 0,
	DDRDLLED_dbe_m		= 0x00000001,
	DDRDLLED_dte_b		= 1,
	DDRDLLED_dte_m		= 0x00000002,
	
		
} ;

#endif	// __IDT_DDR_H__
