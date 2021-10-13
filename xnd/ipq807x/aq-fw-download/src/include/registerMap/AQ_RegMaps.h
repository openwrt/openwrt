/*AQ_RegMaps.h*/

/************************************************************************************
* Copyright (c) 2015, Aquantia
*
* Permission to use, copy, modify, and/or distribute this software for any 
* purpose with or without fee is hereby granted, provided that the above 
* copyright notice and this permission notice appear in all copies.
* 
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES 
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR 
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN 
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF 
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*
* Description:
*
*   This file contains includes all appropriate Aquantia PHY device-specific 
*   register map headers.
*
************************************************************************************/

/*! \file
*   This file contains includes all appropriate Aquantia PHY device-specific 
*   register map headers.
 */

#ifndef AQ_REGISTERMAPS_HEADER
#define AQ_REGISTERMAPS_HEADER

#include "AQ_User.h"
#include "AQ_RegGroupMaxSizes.h"


#ifndef AQ_REVERSED_BITFIELD_ORDERING
/* 
 * Include non-reversed header files (bitfields ordered from LSbit to MSbit) 
 */

/* APPIA */
#include "AQ_APPIA_Global_registers.h"

#include "AQ_APPIA_Global_registers_Defines.h"

/* HHD */
#include "AQ_HHD_Global_registers.h"

#include "AQ_HHD_Global_registers_Defines.h"

#else
/* 
 * Include reversed header files (bitfields ordered from MSbit to LSbit) 
 */

/* APPIA */
#include "AQ_APPIA_Global_registers_reversed.h"

#include "AQ_APPIA_Global_registers_Defines.h"

/* HHD */
#include "AQ_HHD_Global_registers_reversed.h"

#include "AQ_HHD_Global_registers_Defines.h"

#endif

#endif
