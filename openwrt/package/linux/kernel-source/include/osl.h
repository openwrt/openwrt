/*
 * OS Independent Layer
 * 
 * Copyright 2004, Broadcom Corporation      
 * All Rights Reserved.      
 *       
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY      
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM      
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS      
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.      
 * $Id$
 */

#ifndef _osl_h_
#define _osl_h_

#ifdef V2_HAL
#include <v2hal_osl.h>
#elif defined(linux)
#include <linux_osl.h>
#elif PMON
#include <pmon_osl.h>
#elif defined(NDIS)
#include <ndis_osl.h>
#elif defined(_CFE_)
#include <cfe_osl.h>
#elif defined(MACOS9)
#include <macos9_osl.h>
#elif defined(MACOSX)
#include <macosx_osl.h>
#else
#error "Unsupported OSL requested"
#endif

/* handy */
#define	SET_REG(r, mask, val)	W_REG((r), ((R_REG(r) & ~(mask)) | (val)))

#endif	/* _osl_h_ */
