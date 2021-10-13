/*
 * Copyright (c) 2014, 2016, The Linux Foundation. All rights reserved.
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
 * @defgroup dess_mib DESS_MIB
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "dess_mib.h"
#include "dess_reg.h"


#define MIB_FLUSH_ALL_PORTS          0x1
#define MIB_FLUSH_ONE_PORT           0x2
#define MIB_AUTOCAST_ALL_PORTS   0x3

static sw_error_t
_dess_mib_op_commit(a_uint32_t dev_id, a_uint32_t op)
{
    a_uint32_t mib_busy = 1, i = 0x1000, val;
    sw_error_t rv;

    while (mib_busy && --i)
    {
        HSL_REG_FIELD_GET(rv, dev_id, MIB_FUNC, 0, MIB_BUSY,
                          (a_uint8_t *) (&mib_busy), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        aos_udelay(5);
    }

    if (i == 0)
        return SW_BUSY;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_FUNC, 0,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_SET_REG_BY_FIELD(MIB_FUNC, MIB_FUN, op, val);
    SW_SET_REG_BY_FIELD(MIB_FUNC, MIB_BUSY, 1, val);

    HSL_REG_ENTRY_SET(rv, dev_id, MIB_FUNC, 0,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));

    SW_RTN_ON_ERROR(rv);

    mib_busy = 1;
    i = 0x1000;
    while (mib_busy && --i)
    {
        HSL_REG_FIELD_GET(rv, dev_id, MIB_FUNC, 0, MIB_BUSY,
                          (a_uint8_t *) (&mib_busy), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        aos_udelay(5);
    }

    if (i == 0)
        return SW_FAIL;

    return SW_OK;
}

static sw_error_t
_dess_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                   fal_mib_info_t * mib_info)
{
    a_uint32_t val = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_OUT_OF_RANGE;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBROAD, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxBroad = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXPAUSE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxPause = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXMULTI, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxMulti = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXFCSERR, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxFcsErr = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXALLIGNERR, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxAllignErr = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXRUNT, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxRunt = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXFRAGMENT, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxFragment = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX64BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx64Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX128BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx128Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX256BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx256Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX512BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx512Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX1024BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx1024Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX1518BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx1518Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXMAXBYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxMaxByte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXTOOLONG, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxTooLong = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXGOODBYTE_LO, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxGoodByte_lo = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXGOODBYTE_HI, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxGoodByte_hi = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBADBYTE_LO, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxBadByte_lo = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBADBYTE_HI, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxBadByte_hi = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXOVERFLOW, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxOverFlow = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_FILTERED, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Filtered = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBROAD, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxBroad = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXPAUSE, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxPause = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMULTI, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxMulti = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXUNDERRUN, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxUnderRun = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX64BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx64Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX128BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx128Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX256BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx256Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX512BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx512Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX1024BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx1024Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX1518BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx1518Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMAXBYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxMaxByte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXOVERSIZE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxOverSize = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBYTE_LO, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxByte_lo = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBYTE_HI, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxByte_hi = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXCOLLISION, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxCollision = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXABORTCOL, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxAbortCol = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMULTICOL, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxMultiCol = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXSINGALCOL, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxSingalCol = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXEXCDEFER, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxExcDefer = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXDEFER, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxDefer = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXLATECOL, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxLateCol = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXUNICAST, port_id,
                      (a_uint8_t *) (&val), sizeof
                      (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxUniCast = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXUNICAST, port_id,
                      (a_uint8_t *) (&val), sizeof
                      (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxUniCast = val;

    return SW_OK;
}

static sw_error_t
_dess_get_rx_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                   fal_mib_info_t * mib_info)
{
    a_uint32_t val = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_OUT_OF_RANGE;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBROAD, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxBroad = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXPAUSE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxPause = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXMULTI, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxMulti = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXFCSERR, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxFcsErr = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXALLIGNERR, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxAllignErr = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXRUNT, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxRunt = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXFRAGMENT, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxFragment = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX64BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx64Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX128BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx128Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX256BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx256Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX512BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx512Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX1024BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx1024Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX1518BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx1518Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXMAXBYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxMaxByte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXTOOLONG, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxTooLong = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXGOODBYTE_LO, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxGoodByte_lo = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXGOODBYTE_HI, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxGoodByte_hi = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBADBYTE_LO, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxBadByte_lo = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBADBYTE_HI, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxBadByte_hi = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXOVERFLOW, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxOverFlow = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_FILTERED, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Filtered = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXUNICAST, port_id,
                      (a_uint8_t *) (&val), sizeof
                      (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxUniCast = val;

    return SW_OK;
}

static sw_error_t
_dess_get_tx_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                   fal_mib_info_t * mib_info)
{
    a_uint32_t val = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_OUT_OF_RANGE;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBROAD, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxBroad = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXPAUSE, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxPause = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMULTI, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxMulti = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXUNDERRUN, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxUnderRun = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX64BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx64Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX128BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx128Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX256BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx256Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX512BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx512Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX1024BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx1024Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX1518BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx1518Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMAXBYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxMaxByte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXOVERSIZE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxOverSize = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBYTE_LO, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxByte_lo = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBYTE_HI, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxByte_hi = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXCOLLISION, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxCollision = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXABORTCOL, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxAbortCol = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMULTICOL, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxMultiCol = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXSINGALCOL, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxSingalCol = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXEXCDEFER, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxExcDefer = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXDEFER, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxDefer = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXLATECOL, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxLateCol = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXUNICAST, port_id,
                      (a_uint8_t *) (&val), sizeof
                      (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxUniCast = val;

    return SW_OK;
}

static sw_error_t
_dess_mib_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE == enable)
    {
        val = 1;
    }
    else if (A_FALSE == enable)
    {
        val = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, MOD_ENABLE, 0, MIB_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_dess_mib_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, MOD_ENABLE, 0, MIB_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == val)
    {
        *enable = A_TRUE;
    }
    else
    {
        *enable = A_FALSE;
    }

    return SW_OK;
}

static sw_error_t
_dess_mib_cpukeep_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE == enable)
    {
        val = 1;
    }
    else if (A_FALSE == enable)
    {
        val = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, MIB_FUNC, 0, MIB_CPU_KEEP,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return rv;
}

static sw_error_t
_dess_mib_cpukeep_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, MIB_FUNC, 0, MIB_CPU_KEEP,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == val)
    {
        *enable = A_TRUE;
    }
    else
    {
        *enable = A_FALSE;
    }

    return SW_OK;
}

static sw_error_t
_dess_mib_port_flush_counters(a_uint32_t dev_id, fal_port_t port_id)
{
    a_uint32_t val;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU))
    {
        return SW_OUT_OF_RANGE;
    }

    if (port_id>7)
        return SW_BAD_PARAM;

    val =  port_id;
    HSL_REG_FIELD_SET(rv, dev_id, MIB_FUNC, 0, MIB_FLUSH_PORT,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));

    rv = _dess_mib_op_commit( dev_id,  MIB_FLUSH_ONE_PORT);

    return rv;
}

/**
 * @brief Get mib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                  fal_mib_info_t * mib_info)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_get_mib_info(dev_id, port_id, mib_info);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get RX mib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_get_rx_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                  fal_mib_info_t * mib_info)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_get_rx_mib_info(dev_id, port_id, mib_info);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get TX mib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_get_tx_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                  fal_mib_info_t * mib_info)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_get_tx_mib_info(dev_id, port_id, mib_info);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set mib status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_mib_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_mib_status_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get mib status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_mib_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_mib_status_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

HSL_LOCAL sw_error_t
dess_mib_port_flush_counters(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_mib_port_flush_counters(dev_id, port_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set mib cpu keep bit on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_mib_cpukeep_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_mib_cpukeep_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get mib keep bit on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
dess_mib_cpukeep_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _dess_mib_cpukeep_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
dess_mib_init(a_uint32_t dev_id)
{

#ifndef HSL_STANDALONG
	hsl_api_t *p_api;
#endif
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    p_api->get_mib_info   = dess_get_mib_info;
    p_api->get_rx_mib_info   = dess_get_rx_mib_info;
    p_api->get_tx_mib_info   = dess_get_tx_mib_info;
    p_api->mib_status_set = dess_mib_status_set;
    p_api->mib_status_get = dess_mib_status_get;
    p_api->mib_port_flush_counters = dess_mib_port_flush_counters;
    p_api->mib_cpukeep_set = dess_mib_cpukeep_set;
    p_api->mib_cpukeep_get = dess_mib_cpukeep_get;
#endif

    return SW_OK;
}

/**
 * @}
 */

