#ifndef __IDT_INTEG_H__
#define __IDT_INTEG_H__

/*******************************************************************************
 *
 * Copyright 2002 Integrated Device Technology, Inc.
 *		All rights reserved.
 *
 * System Integrity register definition.
 *
 * File   : $Id: integ.h,v 1.3 2002/06/06 18:34:04 astichte Exp $
 *
 * Author : ryan.holmQVist@idt.com
 * Date   : 20011005
 * Update :
 *	    $Log: integ.h,v $
 *	    Revision 1.3  2002/06/06 18:34:04  astichte
 *	    Added XXX_PhysicalAddress and XXX_VirtualAddress
 *	
 *	    Revision 1.2  2002/06/05 18:32:33  astichte
 *	    Removed IDTField
 *	
 *	    Revision 1.1  2002/05/29 17:33:22  sysarch
 *	    jba File moved from vcode/include/idt/acacia
 *
 ******************************************************************************/

enum
{
	INTEG0_PhysicalAddress	= 0x18030000,
	INTEG_PhysicalAddress	= INTEG0_PhysicalAddress,	// Default

	INTEG0_VirtualAddress	= 0xb8030000,
	INTEG_VirtualAddress	= INTEG0_VirtualAddress,	// Default
} ;

// if you are looing for CEA, try rst.h
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
