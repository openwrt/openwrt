/*
 * Copyright (c) 2012, 2017, The Linux Foundation. All rights reserved.
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

#include "sw.h"
#include "fal_mib.h"
#include "hsl_api.h"
#include "adpt.h"

#include <linux/kernel.h>
#include <linux/module.h>


static sw_error_t
_fal_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                  fal_mib_info_t * mib_Info)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_get_mib_info)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_get_mib_info(dev_id, port_id, mib_Info);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->get_mib_info)
        return SW_NOT_SUPPORTED;

    rv = p_api->get_mib_info(dev_id, port_id, mib_Info);
    return rv;
}

static sw_error_t
_fal_get_rx_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                  fal_mib_info_t * mib_Info)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_get_rx_mib_info)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_get_rx_mib_info(dev_id, port_id, mib_Info);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->get_rx_mib_info)
        return SW_NOT_SUPPORTED;

    rv = p_api->get_rx_mib_info(dev_id, port_id, mib_Info);
    return rv;
}

static sw_error_t
_fal_get_tx_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                  fal_mib_info_t * mib_Info)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_get_tx_mib_info)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_get_tx_mib_info(dev_id, port_id, mib_Info);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->get_tx_mib_info)
        return SW_NOT_SUPPORTED;

    rv = p_api->get_tx_mib_info(dev_id, port_id, mib_Info);
    return rv;
}

static sw_error_t
_fal_mib_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_mib_status_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_mib_status_set(dev_id, enable);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mib_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->mib_status_set(dev_id, enable);
    return rv;
}


static sw_error_t
_fal_mib_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_mib_status_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_mib_status_get(dev_id, enable);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mib_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->mib_status_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_mib_port_flush_counters(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_mib_port_flush_counters)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_mib_port_flush_counters(dev_id, port_id);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mib_port_flush_counters)
        return SW_NOT_SUPPORTED;

    rv = p_api->mib_port_flush_counters(dev_id, port_id);
    return rv;
}

static sw_error_t
_fal_mib_cpukeep_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_mib_cpukeep_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_mib_cpukeep_set(dev_id, enable);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mib_cpukeep_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->mib_cpukeep_set(dev_id, enable);
    return rv;
}


static sw_error_t
_fal_mib_cpukeep_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_mib_cpukeep_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_mib_cpukeep_get(dev_id, enable);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->mib_cpukeep_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->mib_cpukeep_get(dev_id, enable);
    return rv;
}
static sw_error_t
_fal_get_xgmib_info(a_uint32_t dev_id, fal_port_t port_id,
                  fal_xgmib_info_t * mib_Info)
{
	sw_error_t rv = SW_OK;
	adpt_api_t *p_api;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_get_xgmib_info)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_get_xgmib_info(dev_id, port_id, mib_Info);
	return rv;
}

static sw_error_t
_fal_get_rx_xgmib_info(a_uint32_t dev_id, fal_port_t port_id,
                  fal_xgmib_info_t * mib_Info)
{
	sw_error_t rv = SW_OK;
	adpt_api_t *p_api;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

        if (NULL == p_api->adpt_get_rx_xgmib_info)
            return SW_NOT_SUPPORTED;

        rv = p_api->adpt_get_rx_xgmib_info(dev_id, port_id, mib_Info);
        return rv;
}

static sw_error_t
_fal_get_tx_xgmib_info(a_uint32_t dev_id, fal_port_t port_id,
                  fal_xgmib_info_t * mib_Info)
{
	sw_error_t rv = SW_OK;
	adpt_api_t *p_api;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

        if (NULL == p_api->adpt_get_tx_xgmib_info)
            return SW_NOT_SUPPORTED;

        rv = p_api->adpt_get_tx_xgmib_info(dev_id, port_id, mib_Info);
        return rv;
}

static fal_mib_counter_t *g_mibcounter[SW_MAX_NR_DEV];

sw_error_t
fal_mib_counter_alloc(a_uint32_t dev_id, a_uint64_t **p_mibcounter)
{
	*p_mibcounter = kzalloc(SW_MAX_NR_PORT * sizeof(fal_mib_counter_t),
			GFP_KERNEL);

	if(NULL == *p_mibcounter)
		return SW_OUT_OF_MEM;

	g_mibcounter[dev_id] = (fal_mib_counter_t*)*p_mibcounter;

	return SW_OK;
}

static void _fal_rx_mib_update(a_uint32_t dev_id, fal_port_t port_id,
                 fal_mib_info_t * mib_Info)
{
	if(NULL == g_mibcounter[dev_id])
		return;

	g_mibcounter[dev_id][port_id].RxBroad += mib_Info->RxBroad;
	g_mibcounter[dev_id][port_id].RxPause += mib_Info->RxPause;
	g_mibcounter[dev_id][port_id].RxMulti += mib_Info->RxMulti;
	g_mibcounter[dev_id][port_id].RxFcsErr += mib_Info->RxFcsErr;
	g_mibcounter[dev_id][port_id].RxAllignErr += mib_Info->RxAllignErr;
	g_mibcounter[dev_id][port_id].RxRunt += mib_Info->RxRunt;
	g_mibcounter[dev_id][port_id].RxFragment += mib_Info->RxFragment;
	g_mibcounter[dev_id][port_id].Rx64Byte += mib_Info->Rx64Byte;
	g_mibcounter[dev_id][port_id].Rx128Byte += mib_Info->Rx128Byte;
	g_mibcounter[dev_id][port_id].Rx256Byte += mib_Info->Rx256Byte;
	g_mibcounter[dev_id][port_id].Rx512Byte += mib_Info->Rx512Byte;
	g_mibcounter[dev_id][port_id].Rx1024Byte += mib_Info->Rx1024Byte;
	g_mibcounter[dev_id][port_id].Rx1518Byte += mib_Info->Rx1518Byte;
	g_mibcounter[dev_id][port_id].RxMaxByte += mib_Info->RxMaxByte;
	g_mibcounter[dev_id][port_id].RxTooLong += mib_Info->RxTooLong;
	g_mibcounter[dev_id][port_id].RxGoodByte +=
		(((u64)mib_Info->RxGoodByte_hi) << 32) | mib_Info->RxGoodByte_lo;
	g_mibcounter[dev_id][port_id].RxBadByte +=
		(((u64)mib_Info->RxBadByte_hi) << 32) | mib_Info->RxBadByte_lo;
	g_mibcounter[dev_id][port_id].RxOverFlow += mib_Info->RxOverFlow;
	g_mibcounter[dev_id][port_id].Filtered += mib_Info->Filtered;
	g_mibcounter[dev_id][port_id].RxUniCast += mib_Info->RxUniCast;
	g_mibcounter[dev_id][port_id].RxTooLongByte +=
		(((u64)mib_Info->RxTooLongByte_hi) << 32) | mib_Info->RxTooLongByte_lo;
	g_mibcounter[dev_id][port_id].RxRuntByte +=
		(((u64)mib_Info->RxRuntByte_hi) << 32) | mib_Info->RxRuntByte_lo;
	g_mibcounter[dev_id][port_id].Rx14To63 += mib_Info->Rx14To63;

	return;
}

static void _fal_tx_mib_update(a_uint32_t dev_id, fal_port_t port_id,
                 fal_mib_info_t * mib_Info)
{
	if(NULL == g_mibcounter[dev_id])
		return;

	g_mibcounter[dev_id][port_id].TxBroad += mib_Info->TxBroad;
	g_mibcounter[dev_id][port_id].TxPause += mib_Info->TxPause;
	g_mibcounter[dev_id][port_id].TxMulti += mib_Info->TxMulti;
	g_mibcounter[dev_id][port_id].TxUnderRun += mib_Info->TxUnderRun;
	g_mibcounter[dev_id][port_id].Tx64Byte += mib_Info->Tx64Byte;
	g_mibcounter[dev_id][port_id].Tx128Byte += mib_Info->Tx128Byte;
	g_mibcounter[dev_id][port_id].Tx256Byte += mib_Info->Tx256Byte;
	g_mibcounter[dev_id][port_id].Tx512Byte += mib_Info->Tx512Byte;
	g_mibcounter[dev_id][port_id].Tx1024Byte += mib_Info->Tx1024Byte;
	g_mibcounter[dev_id][port_id].Tx1518Byte += mib_Info->Tx1518Byte;
	g_mibcounter[dev_id][port_id].TxMaxByte += mib_Info->TxMaxByte;
	g_mibcounter[dev_id][port_id].TxOverSize += mib_Info->TxOverSize;
	g_mibcounter[dev_id][port_id].TxByte +=
		(((u64)mib_Info->TxByte_hi) << 32) | mib_Info->TxByte_lo;
	g_mibcounter[dev_id][port_id].TxCollision += mib_Info->TxCollision;
	g_mibcounter[dev_id][port_id].TxAbortCol += mib_Info->TxAbortCol;
	g_mibcounter[dev_id][port_id].TxMultiCol += mib_Info->TxMultiCol;
	g_mibcounter[dev_id][port_id].TxSingalCol += mib_Info->TxSingalCol;
	g_mibcounter[dev_id][port_id].TxExcDefer += mib_Info->TxExcDefer;
	g_mibcounter[dev_id][port_id].TxDefer += mib_Info->TxDefer;
	g_mibcounter[dev_id][port_id].TxLateCol += mib_Info->TxLateCol;
	g_mibcounter[dev_id][port_id].TxUniCast += mib_Info->TxUniCast;

	return;
}

/*insert flag for inner fal, don't remove it*/
/**
 * @brief Get mib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
sw_error_t
fal_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                 fal_mib_info_t * mib_Info)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_get_mib_info(dev_id, port_id, mib_Info);
    _fal_rx_mib_update(dev_id, port_id, mib_Info);
    _fal_tx_mib_update(dev_id, port_id, mib_Info);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_mib_counter_get(a_uint32_t dev_id, fal_port_t port_id,
				fal_mib_counter_t *mib_counter)
{
	sw_error_t rv;
	fal_mib_info_t mib_info = {0};

	if(NULL == g_mibcounter[dev_id])
		return SW_BAD_PTR;

	rv = fal_get_mib_info(dev_id, port_id, &mib_info);

	if(rv != SW_OK)
		return rv;

	mib_counter->RxBroad        = g_mibcounter[dev_id][port_id].RxBroad;
	mib_counter->RxPause        = g_mibcounter[dev_id][port_id].RxPause;
	mib_counter->RxMulti        = g_mibcounter[dev_id][port_id].RxMulti;
	mib_counter->RxFcsErr       = g_mibcounter[dev_id][port_id].RxFcsErr;
	mib_counter->RxAllignErr    = g_mibcounter[dev_id][port_id].RxAllignErr;
	mib_counter->RxRunt         = g_mibcounter[dev_id][port_id].RxRunt;
	mib_counter->RxFragment     = g_mibcounter[dev_id][port_id].RxFragment ;
	mib_counter->Rx64Byte       = g_mibcounter[dev_id][port_id].Rx64Byte ;
	mib_counter->Rx128Byte      = g_mibcounter[dev_id][port_id].Rx128Byte;
	mib_counter->Rx256Byte      = g_mibcounter[dev_id][port_id].Rx256Byte;
	mib_counter->Rx512Byte      = g_mibcounter[dev_id][port_id].Rx512Byte;
	mib_counter->Rx1024Byte     = g_mibcounter[dev_id][port_id].Rx1024Byte;
	mib_counter->Rx1518Byte     = g_mibcounter[dev_id][port_id].Rx1518Byte;
	mib_counter->RxMaxByte      = g_mibcounter[dev_id][port_id].RxMaxByte;
	mib_counter->RxTooLong      = g_mibcounter[dev_id][port_id].RxTooLong;
	mib_counter->RxGoodByte     = g_mibcounter[dev_id][port_id].RxGoodByte;
	mib_counter->RxBadByte      = g_mibcounter[dev_id][port_id].RxBadByte;
	mib_counter->RxOverFlow     = g_mibcounter[dev_id][port_id].RxOverFlow;
	mib_counter->Filtered       = g_mibcounter[dev_id][port_id].Filtered;
	mib_counter->TxBroad        = g_mibcounter[dev_id][port_id].TxBroad;
	mib_counter->TxPause        = g_mibcounter[dev_id][port_id].TxPause;
	mib_counter->TxMulti        = g_mibcounter[dev_id][port_id].TxMulti;
	mib_counter->TxUnderRun     = g_mibcounter[dev_id][port_id].TxUnderRun;
	mib_counter->Tx64Byte       = g_mibcounter[dev_id][port_id].Tx64Byte;
	mib_counter->Tx128Byte      = g_mibcounter[dev_id][port_id].Tx128Byte;
	mib_counter->Tx256Byte      = g_mibcounter[dev_id][port_id].Tx256Byte;
	mib_counter->Tx512Byte      = g_mibcounter[dev_id][port_id].Tx512Byte;
	mib_counter->Tx1024Byte     = g_mibcounter[dev_id][port_id].Tx1024Byte;
	mib_counter->Tx1518Byte     = g_mibcounter[dev_id][port_id].Tx1518Byte;
	mib_counter->TxMaxByte      = g_mibcounter[dev_id][port_id].TxMaxByte;
	mib_counter->TxOverSize     = g_mibcounter[dev_id][port_id].TxOverSize;
	mib_counter->TxByte         = g_mibcounter[dev_id][port_id].TxByte;
	mib_counter->TxCollision    = g_mibcounter[dev_id][port_id].TxCollision;
	mib_counter->TxAbortCol     = g_mibcounter[dev_id][port_id].TxAbortCol;
	mib_counter->TxMultiCol     = g_mibcounter[dev_id][port_id].TxMultiCol;
	mib_counter->TxSingalCol    = g_mibcounter[dev_id][port_id].TxSingalCol;
	mib_counter->TxExcDefer     = g_mibcounter[dev_id][port_id].TxExcDefer;
	mib_counter->TxDefer        = g_mibcounter[dev_id][port_id].TxDefer;
	mib_counter->TxLateCol      = g_mibcounter[dev_id][port_id].TxLateCol;
	mib_counter->RxUniCast      = g_mibcounter[dev_id][port_id].RxUniCast;
	mib_counter->TxUniCast      = g_mibcounter[dev_id][port_id].TxUniCast;
	mib_counter->Rx14To63       = g_mibcounter[dev_id][port_id].Rx14To63;
	mib_counter->RxTooLongByte  = g_mibcounter[dev_id][port_id].RxTooLongByte;
	mib_counter->RxRuntByte     = g_mibcounter[dev_id][port_id].RxRuntByte;

	return SW_OK;
}

/**
 * @brief Get RX mib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
sw_error_t
fal_get_rx_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                 fal_mib_info_t * mib_Info)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_get_rx_mib_info(dev_id, port_id, mib_Info);
    _fal_rx_mib_update(dev_id, port_id, mib_Info);
    FAL_API_UNLOCK;

    return rv;
}

/**
 * @brief Get TX mib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
sw_error_t
fal_get_tx_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                 fal_mib_info_t * mib_Info)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_get_tx_mib_info(dev_id, port_id, mib_Info);
    _fal_tx_mib_update(dev_id, port_id, mib_Info);
    FAL_API_UNLOCK;

    return rv;
}

/**
 * @brief Set mib status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_mib_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mib_status_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get mib status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_mib_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mib_status_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Flush mib counters on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
sw_error_t
fal_mib_port_flush_counters(a_uint32_t dev_id, fal_port_t port_id )
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mib_port_flush_counters(dev_id, port_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set mib status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_mib_cpukeep_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mib_cpukeep_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get mib status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_mib_cpukeep_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_mib_cpukeep_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get xgmacmib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
sw_error_t
fal_get_xgmib_info(a_uint32_t dev_id, fal_port_t port_id,
                 fal_xgmib_info_t * mib_Info)
{
    sw_error_t rv;
    fal_mib_counter_t gmac_mib = { 0 };

    FAL_API_LOCK;
    rv = _fal_get_xgmib_info(dev_id, port_id, mib_Info);
    FAL_API_UNLOCK;
    SW_RTN_ON_ERROR(rv);

    rv = fal_mib_counter_get(dev_id, port_id, &gmac_mib);
    SW_RTN_ON_ERROR(rv);

    mib_Info->RxFrame += (gmac_mib.RxBroad +
				gmac_mib.RxMulti + gmac_mib.RxUniCast);
    mib_Info->RxByte += (gmac_mib.RxGoodByte+gmac_mib.RxBadByte);
    mib_Info->RxByteGood += gmac_mib.RxGoodByte;
    mib_Info->RxBroadGood += gmac_mib.RxBroad;
    mib_Info->RxMultiGood += gmac_mib.RxMulti;
    mib_Info->RxFcsErr += gmac_mib.RxFcsErr;
    mib_Info->RxRuntErr += gmac_mib.RxRunt;
    mib_Info->Rx64Byte += gmac_mib.Rx64Byte;
    mib_Info->Rx128Byte += gmac_mib.Rx128Byte;
    mib_Info->Rx256Byte += gmac_mib.Rx256Byte;
    mib_Info->Rx512Byte += gmac_mib.Rx512Byte;
    mib_Info->Rx1024Byte += gmac_mib.Rx1024Byte;
    mib_Info->RxMaxByte += (gmac_mib.Rx1518Byte + gmac_mib.RxMaxByte);
    mib_Info->RxUnicastGood += gmac_mib.RxUniCast;
    mib_Info->RxLengthError += gmac_mib.RxTooLong;
    mib_Info->RxPause += gmac_mib.RxPause;
    mib_Info->RxOverFlow += gmac_mib.RxOverFlow;

    mib_Info->TxByte += gmac_mib.TxByte;
    mib_Info->TxFrame += (gmac_mib.TxBroad +
				gmac_mib.TxMulti + gmac_mib.TxUniCast);
    mib_Info->TxBroadGood += gmac_mib.TxBroad;
    mib_Info->TxMultiGood += gmac_mib.TxMulti;
    mib_Info->Tx64Byte += gmac_mib.Tx64Byte;
    mib_Info->Tx128Byte += gmac_mib.Tx128Byte;
    mib_Info->Tx256Byte += gmac_mib.Tx256Byte;
    mib_Info->Tx512Byte += gmac_mib.Tx512Byte;
    mib_Info->Tx1024Byte += gmac_mib.Tx1024Byte;
    mib_Info->TxMaxByte += (gmac_mib.Tx1518Byte + gmac_mib.TxMaxByte);
    mib_Info->TxUnicast += gmac_mib.TxUniCast;
    mib_Info->TxMulti += gmac_mib.TxMulti;
    mib_Info->TxBroad += gmac_mib.TxBroad;
    mib_Info->TxByteGood += gmac_mib.TxByte;
    mib_Info->TxFrameGood += (gmac_mib.TxBroad +
				gmac_mib.TxMulti + gmac_mib.TxUniCast);
    mib_Info->TxPause += gmac_mib.TxPause;

    return rv;
}

/**
 * @brief Get RX xgmacmib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
sw_error_t
fal_get_rx_xgmib_info(a_uint32_t dev_id, fal_port_t port_id,
                 fal_xgmib_info_t * mib_Info)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_get_rx_xgmib_info(dev_id, port_id, mib_Info);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get TX xgmacmib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
sw_error_t
fal_get_tx_xgmib_info(a_uint32_t dev_id, fal_port_t port_id,
                 fal_xgmib_info_t * mib_Info)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_get_tx_xgmib_info(dev_id, port_id, mib_Info);
    FAL_API_UNLOCK;
    return rv;
}

EXPORT_SYMBOL(fal_get_mib_info);
EXPORT_SYMBOL(fal_get_rx_mib_info);
EXPORT_SYMBOL(fal_get_tx_mib_info);
EXPORT_SYMBOL(fal_get_xgmib_info);
EXPORT_SYMBOL(fal_get_rx_xgmib_info);
EXPORT_SYMBOL(fal_get_tx_xgmib_info);
EXPORT_SYMBOL(fal_mib_status_set);
EXPORT_SYMBOL(fal_mib_status_get);
EXPORT_SYMBOL(fal_mib_port_flush_counters);
EXPORT_SYMBOL(fal_mib_cpukeep_set);
EXPORT_SYMBOL(fal_mib_cpukeep_get);
EXPORT_SYMBOL(fal_mib_counter_get);

/*insert flag for outter fal, don't remove it*/
/**
 * @}
 */
