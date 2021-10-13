/*
 * Copyright (c) 2014, 2017, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/**
 * @defgroup fal_mib FAL_MIB
 * @{
 */
#ifndef _FAL_MIB_H
#define _FAL_MIB_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

    /**@brief This structure defines the mib infomation.
    */
    typedef struct
    {
        a_uint32_t RxBroad;
        a_uint32_t RxPause;
        a_uint32_t RxMulti;
        a_uint32_t RxFcsErr;
        a_uint32_t RxAllignErr;
        a_uint32_t RxRunt;
        a_uint32_t RxFragment;
        a_uint32_t Rx64Byte;
        a_uint32_t Rx128Byte;
        a_uint32_t Rx256Byte;
        a_uint32_t Rx512Byte;
        a_uint32_t Rx1024Byte;
        a_uint32_t Rx1518Byte;
        a_uint32_t RxMaxByte;
        a_uint32_t RxTooLong;
        a_uint32_t RxGoodByte_lo;       /**<  low 32 bits of RxGoodByte statistc item */
        a_uint32_t RxGoodByte_hi;       /**<   high 32 bits of RxGoodByte statistc item*/
        a_uint32_t RxBadByte_lo;        /**<   low 32 bits of RxBadByte statistc item */
        a_uint32_t RxBadByte_hi;        /**<   high 32 bits of RxBadByte statistc item */
        a_uint32_t RxOverFlow;
        a_uint32_t Filtered;
        a_uint32_t TxBroad;
        a_uint32_t TxPause;
        a_uint32_t TxMulti;
        a_uint32_t TxUnderRun;
        a_uint32_t Tx64Byte;
        a_uint32_t Tx128Byte;
        a_uint32_t Tx256Byte;
        a_uint32_t Tx512Byte;
        a_uint32_t Tx1024Byte;
        a_uint32_t Tx1518Byte;
        a_uint32_t TxMaxByte;
        a_uint32_t TxOverSize;
        a_uint32_t TxByte_lo;       /**<  low 32 bits of TxByte statistc item */
        a_uint32_t TxByte_hi;       /**<  high 32 bits of TxByte statistc item */
        a_uint32_t TxCollision;
        a_uint32_t TxAbortCol;
        a_uint32_t TxMultiCol;
        a_uint32_t TxSingalCol;
        a_uint32_t TxExcDefer;
        a_uint32_t TxDefer;
        a_uint32_t TxLateCol;
        a_uint32_t RxUniCast;
        a_uint32_t TxUniCast;
        a_uint32_t RxJumboFcsErr;	/* add for  Hawkeye*/
        a_uint32_t RxJumboAligenErr;		/* add for Hawkeye*/
        a_uint32_t Rx14To63;	/*add for ipq60xx lpbk port*/
        a_uint32_t RxTooLongByte_lo;	/*add for ipq60xx lpbk port*/
        a_uint32_t RxTooLongByte_hi;	/*add for ipq60xx lpbk port*/
        a_uint32_t RxRuntByte_lo;	/*add for ipq60xx lpbk port*/
        a_uint32_t RxRuntByte_hi;	/*add for ipq60xx lpbk port*/
    } fal_mib_info_t;

/*define structure for software with 64bit*/
typedef struct
{
	a_uint64_t RxBroad;
	a_uint64_t RxPause;
	a_uint64_t RxMulti;
	a_uint64_t RxFcsErr;
	a_uint64_t RxAllignErr;
	a_uint64_t RxRunt;
	a_uint64_t RxFragment;
	a_uint64_t Rx64Byte;
	a_uint64_t Rx128Byte;
	a_uint64_t Rx256Byte;
	a_uint64_t Rx512Byte;
	a_uint64_t Rx1024Byte;
	a_uint64_t Rx1518Byte;
	a_uint64_t RxMaxByte;
	a_uint64_t RxTooLong;
	a_uint64_t RxGoodByte;
	a_uint64_t RxBadByte;
	a_uint64_t RxOverFlow;		/* no this counter for Hawkeye*/
	a_uint64_t Filtered;			/*no this counter for Hawkeye*/
	a_uint64_t TxBroad;
	a_uint64_t TxPause;
	a_uint64_t TxMulti;
	a_uint64_t TxUnderRun;
	a_uint64_t Tx64Byte;
	a_uint64_t Tx128Byte;
	a_uint64_t Tx256Byte;
	a_uint64_t Tx512Byte;
	a_uint64_t Tx1024Byte;
	a_uint64_t Tx1518Byte;
	a_uint64_t TxMaxByte;
	a_uint64_t TxOverSize;	/*no this counter for Hawkeye*/
	a_uint64_t TxByte;
	a_uint64_t TxCollision;
	a_uint64_t TxAbortCol;
	a_uint64_t TxMultiCol;
	a_uint64_t TxSingalCol;
	a_uint64_t TxExcDefer;
	a_uint64_t TxDefer;
	a_uint64_t TxLateCol;
	a_uint64_t RxUniCast;
	a_uint64_t TxUniCast;
	a_uint64_t RxJumboFcsErr;	/* add for  Hawkeye*/
	a_uint64_t RxJumboAligenErr;	/* add for Hawkeye*/
	a_uint64_t Rx14To63;	/*add for ipq60xx lpbk port*/
	a_uint64_t RxTooLongByte;	/*add for ipq60xx lpbk port*/
	a_uint64_t RxRuntByte;	/*add for ipq60xx lpbk port*/
} fal_mib_counter_t;

typedef struct
{
	a_uint64_t	RxFrame;
	a_uint64_t	RxByte;
	a_uint64_t	RxByteGood;
	a_uint64_t	RxBroadGood;
	a_uint64_t	RxMultiGood;
	a_uint64_t	RxFcsErr;
	a_uint64_t	RxRuntErr;
	a_uint64_t	RxJabberError;
	a_uint64_t	RxUndersizeGood;
	a_uint64_t	RxOversizeGood;
	a_uint64_t	Rx64Byte;
	a_uint64_t	Rx128Byte;
	a_uint64_t	Rx256Byte;
	a_uint64_t	Rx512Byte;
	a_uint64_t	Rx1024Byte;
	a_uint64_t	RxMaxByte;
	a_uint64_t	RxUnicastGood;
	a_uint64_t	RxLengthError;
	a_uint64_t	RxOutOfRangeError;
	a_uint64_t	RxPause;
	a_uint64_t	RxOverFlow;
	a_uint64_t	RxVLANFrameGoodBad;
	a_uint64_t	RxWatchDogError;
	a_uint64_t	RxLPIUsec;
	a_uint64_t	RxLPITran;
	a_uint64_t	RxDropFrameGoodBad;
	a_uint64_t	RxDropByteGoodBad;

	a_uint64_t	TxByte;
	a_uint64_t	TxFrame;
	a_uint64_t	TxBroadGood;
	a_uint64_t	TxMultiGood;
	a_uint64_t	Tx64Byte;
	a_uint64_t	Tx128Byte;
	a_uint64_t	Tx256Byte;
	a_uint64_t	Tx512Byte;
	a_uint64_t	Tx1024Byte;
	a_uint64_t	TxMaxByte;
	a_uint64_t	TxUnicast;
	a_uint64_t	TxMulti;
	a_uint64_t	TxBroad;
	a_uint64_t	TxUnderFlowError;
	a_uint64_t	TxByteGood;
	a_uint64_t	TxFrameGood;
	a_uint64_t	TxPause;
	a_uint64_t	TxVLANFrameGood;
	a_uint64_t	TxLPIUsec;
	a_uint64_t	TxLPITran;
} fal_xgmib_info_t;

	enum
	{
		/*mib*/
		FUNC_GET_MIB_INFO = 0 ,
		FUNC_GET_RX_MIB_INFO,
		FUNC_GET_TX_MIB_INFO,
		FUNC_GET_XGMIB_INFO,
		FUNC_GET_TX_XGMIB_INFO,
		FUNC_GET_RX_XGMIB_INFO,
		FUNC_MIB_STATUS_SET,
		FUNC_MIB_STATUS_GET,
		FUNC_MIB_PORT_FLUSH_COUNTERS,
		FUNC_MIB_CPUKEEP_SET,
		FUNC_MIB_CPUKEEP_GET,
	};

    sw_error_t
    fal_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                     fal_mib_info_t * mib_info );

	sw_error_t
	fal_get_xgmib_info(a_uint32_t dev_id, fal_port_t port_id,
					 fal_xgmib_info_t * mib_Info);


    sw_error_t
    fal_mib_status_set(a_uint32_t dev_id, a_bool_t enable);



    sw_error_t
    fal_mib_status_get(a_uint32_t dev_id, a_bool_t * enable);


    sw_error_t
    fal_mib_port_flush_counters(a_uint32_t dev_id, fal_port_t port_id);


    sw_error_t
    fal_mib_cpukeep_set(a_uint32_t dev_id, a_bool_t  enable);

    sw_error_t
    fal_mib_cpukeep_get(a_uint32_t dev_id, a_bool_t * enable);

sw_error_t
fal_mib_counter_get(a_uint32_t dev_id, fal_port_t port_id,
				fal_mib_counter_t *mib_counter);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FAL_MIB_H */
/**
 * @}
 */
