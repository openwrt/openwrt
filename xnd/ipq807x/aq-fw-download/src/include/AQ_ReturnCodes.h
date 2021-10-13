/* AQ_ReturnCodes.h */

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
*   This file defines the AQ_API functions' integral return codes.
*
*
************************************************************************************/

/*! \file
  This file defines the AQ_API functions' integral return codes.
*/

#ifndef AQ_RETURNCODES_TOKEN
#define AQ_RETURNCODES_TOKEN


/*! \defgroup ReturnCodes
  @{
*/

/*! Most AQ_API functions return AQ_Retcode to report success or failure.
 * The values used are defined as preprocessor symbols in AQ_ReturnCodes.h. 
 * Callers should prefer to test the return values by equivalence to these
 * symbols, rather than using the integer values directly, as these may 
 * not be stable across releases.  The set of possible return codes that may 
 * be returned by a particular API function can be found in the function's 
 * documentation, as well as information on how to interpret each of the 
 * possible return codes. */
typedef unsigned int AQ_Retcode;

/*! \defgroup Success 
  @{ */
#define AQ_RET_OK 0
/*@}*/


/*! \defgroup GeneralErrors 
  @{ */
#define AQ_RET_ERROR 100
#define AQ_RET_UP_BUSY_TIMEOUT 101
/*@}*/

/*! \defgroup FunctionSpecificResults 
  @{ */
#define AQ_RET_FLASH_READY 200
#define AQ_RET_FLASH_READINESS_TIMEOUT 204

#define AQ_RET_FLASHINTF_READY 201
#define AQ_RET_FLASHINTF_NOTREADY 202
#define AQ_RET_FLASHINTF_READINESS_TIMEOUT 203

#define AQ_RET_FLASH_TYPE_UNKNOWN 205
#define AQ_RET_FLASH_TYPE_BAD 206

#define AQ_RET_FLASH_IMAGE_CORRUPT 207
#define AQ_RET_FLASH_IMAGE_TOO_LARGE 208
#define AQ_RET_FLASH_IMAGE_MISMATCH 209

#define AQ_RET_FLASH_PAGE_SIZE_CHANGED 210

#define AQ_RET_BOOTLOAD_PROVADDR_OOR 211
#define AQ_RET_BOOTLOAD_NONUNIFORM_REGVALS 212
#define AQ_RET_BOOTLOAD_CRC_MISMATCH 213
#define AQ_RET_BOOTLOAD_PROVTABLE_TOO_LARGE 228

#define AQ_RET_LOOPBACK_BAD_ENTRY_STATE 214

#define AQ_RET_DEBUGTRACE_FREEZE_TIMEOUT 215
#define AQ_RET_DEBUGTRACE_UNFREEZE_TIMEOUT 216

#define AQ_RET_CABLEDIAG_ALREADY_RUNNING 217
#define AQ_RET_CABLEDIAG_STILL_RUNNING 218
#define AQ_RET_CABLEDIAG_BAD_PAIRSTATUS 219
#define AQ_RET_CABLEDIAG_RESULTS_ALREDY_COLLECTED 220
#define AQ_RET_CABLEDIAG_BAD_NUM_SAMPLES 221
#define AQ_RET_CABLEDIAG_REPORTEDPAIR_MISMATCH 222
#define AQ_RET_CABLEDIAG_REPORTEDPAIR_OOR 223
#define AQ_RET_CABLEDIAG_STARTED_PAIR_B 224
#define AQ_RET_CABLEDIAG_STARTED_PAIR_C 225
#define AQ_RET_CABLEDIAG_STARTED_PAIR_D 226
#define AQ_RET_CABLEDIAG_TXENABLE_MISMATCH 227

#define AQ_RET_SERDESEYE_BAD_SERDES_MODE 229
#define AQ_RET_SERDESEYE_BAD_MEAS_COUNT 230
#define AQ_RET_SERDESEYE_MEAS_TIMEOUT 231
#define AQ_RET_SERDESEYE_LANE_OOR 232
#define AQ_RET_SERDESEYE_COORD_OOR 233

#define AQ_RET_PIFMAILBOX_ERROR 234
#define AQ_RET_PIFMAILBOX_TIMEOUT 235

#define AQ_RET_SEC_TABLE_INDEX_OOR 236
/*@}*/

/*@}*/
 
#endif
