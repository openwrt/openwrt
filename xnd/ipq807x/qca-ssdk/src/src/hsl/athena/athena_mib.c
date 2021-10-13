/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
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
 * @defgroup athena_mib ATHENA_MIB
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "athena_mib.h"
#include "athena_reg.h"

static sw_error_t
_athena_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                     fal_mib_info_t * mib_info)
{
    a_uint32_t val0 = 0, val1 = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_OUT_OF_RANGE;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBROAD, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBROAD_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxBroad = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXPAUSE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXPAUSE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxPause = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXMULTI, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXMULTI_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxMulti = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXFCSERR, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXFCSERR_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxFcsErr = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXALLIGNERR, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXALLIGNERR_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxAllignErr = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXRUNT, port_id, (a_uint8_t *) (&val0),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXRUNT_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxRunt = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXFRAGMENT, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXFRAGMENT_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxFragment = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX64BYTE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX64BYTE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx64Byte = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX128BYTE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX128BYTE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx128Byte = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX256BYTE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX256BYTE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx256Byte = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX512BYTE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX512BYTE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx512Byte = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX1024BYTE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX1024BYTE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx1024Byte = val0 + val1;

    mib_info->Rx1518Byte = 0;    //reserved for s16

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXMAXBYTE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXMAXBYTE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxMaxByte = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXTOOLONG, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXTOOLONG_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxTooLong = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXGOODBYTE_LO, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXGOODBYTE_LO_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxGoodByte_lo = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXGOODBYTE_HI, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXGOODBYTE_HI_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxGoodByte_hi = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBADBYTE_LO, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBADBYTE_LO_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxBadByte_lo = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBADBYTE_HI, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBADBYTE_HI_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxBadByte_hi = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXOVERFLOW, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXOVERFLOW_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxOverFlow = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_FILTERED, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_FILTERED_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Filtered = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBROAD, port_id, (a_uint8_t *) (&val0),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBROAD_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxBroad = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXPAUSE, port_id, (a_uint8_t *) (&val0),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXPAUSE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxPause = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMULTI, port_id, (a_uint8_t *) (&val0),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMULTI_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxMulti = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXUNDERRUN, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXUNDERRUN_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxUnderRun = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX64BYTE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX64BYTE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx64Byte = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX128BYTE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX128BYTE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx128Byte = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX256BYTE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX256BYTE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx256Byte = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX512BYTE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX512BYTE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx512Byte = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX1024BYTE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX1024BYTE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx1024Byte = val0 + val1;

    mib_info->Tx1518Byte = 0;    //reserved for s16

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMAXBYTE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMAXBYTE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxMaxByte = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXOVERSIZE, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXOVERSIZE_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxOverSize = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBYTE_LO, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBYTE_LO_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxByte_lo = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBYTE_HI, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBYTE_HI_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxByte_hi = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXCOLLISION, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXCOLLISION_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxCollision = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXABORTCOL, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXABORTCOL_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxAbortCol = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMULTICOL, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMULTICOL_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxMultiCol = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXSINGALCOL, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXSINGALCOL_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxSingalCol = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXEXCDEFER, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXEXCDEFER_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxExcDefer = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXDEFER, port_id, (a_uint8_t *) (&val0),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXDEFER_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxDefer = val0 + val1;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXLATECOL, port_id,
                      (a_uint8_t *) (&val0), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXLATECOL_2, port_id,
                      (a_uint8_t *) (&val1), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxLateCol = val0 + val1;

    return SW_OK;
}

/**
 * @brief Get mib infomation on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
athena_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                    fal_mib_info_t * mib_info)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _athena_get_mib_info(dev_id, port_id, mib_info);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
athena_mib_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    hsl_api_t *p_api;

    SW_RTN_ON_NULL (p_api = hsl_api_ptr_get(dev_id));

    p_api->get_mib_info = athena_get_mib_info;
#endif

    return SW_OK;
}

/**
 * @}
 */

