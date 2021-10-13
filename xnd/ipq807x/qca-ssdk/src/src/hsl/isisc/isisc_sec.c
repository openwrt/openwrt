/*
 * Copyright (c) 2012, 2016, The Linux Foundation. All rights reserved.
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
 * @defgroup isisc_sec ISISC_SEC
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "isisc_sec.h"
#include "isisc_reg.h"

#define NORM_CTRL0_ADDR 0x0200
#define NORM_CTRL1_ADDR 0x0204
#define NORM_CTRL2_ADDR 0x0208
#define NORM_CTRL3_ADDR 0x0c00

static sw_error_t
_isisc_sec_norm_item_set(a_uint32_t dev_id, fal_norm_item_t item, void *value)
{
    sw_error_t rv;
    fal_fwd_cmd_t cmd;
    a_bool_t enable;
    a_uint32_t addr, offset, len, reg = 0, val;

    HSL_DEV_ID_CHECK(dev_id);

    cmd = *((fal_fwd_cmd_t *) value);
    enable = *((a_bool_t *) value);
    val = *((a_uint32_t *) value);

    len = 1;
    switch (item)
    {
        case FAL_NORM_MAC_RESV_VID_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 0;
            goto cmd_chk;

        case FAL_NORM_MAC_INVALID_SRC_ADDR_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 20;
            goto cmd_chk;

        case FAL_NORM_IP_INVALID_VER_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 1;
            goto cmd_chk;

        case FAL_NROM_IP_SAME_ADDR_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 2;
            goto cmd_chk;
            break;

        case FAL_NROM_IP_TTL_CHANGE_STATUS:
            addr = NORM_CTRL3_ADDR;
            offset = 11;
            goto sts_chk;

        case FAL_NROM_IP_TTL_VALUE:
            addr = NORM_CTRL3_ADDR;
            offset = 12;
            len = 8;
            goto set_reg;

        case FAL_NROM_IP4_INVALID_HL_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 3;
            goto cmd_chk;

        case FAL_NROM_IP4_HDR_OPTIONS_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 4;
            len = 2;
            goto s_cmd_chk;

        case FAL_NROM_IP4_INVALID_DF_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 7;
            goto cmd_chk;

        case FAL_NROM_IP4_FRAG_OFFSET_MIN_LEN_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 8;
            goto cmd_chk;

        case FAL_NROM_IP4_FRAG_OFFSET_MIN_SIZE:
            addr = NORM_CTRL1_ADDR;
            offset = 24;
            len = 8;
            goto set_reg;

        case FAL_NROM_IP4_FRAG_OFFSET_MAX_LEN_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 9;
            goto cmd_chk;

        case FAL_NROM_IP4_INVALID_FRAG_OFFSET_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 10;
            goto cmd_chk;

        case FAL_NROM_IP4_INVALID_SIP_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 11;
            len = 1;
            goto cmd_chk;

        case FAL_NROM_IP4_INVALID_DIP_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 12;
            goto cmd_chk;

        case FAL_NROM_IP4_INVALID_CHKSUM_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 13;
            goto cmd_chk;

        case FAL_NROM_IP4_INVALID_PL_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 19;
            goto cmd_chk;

        case FAL_NROM_IP4_DF_CLEAR_STATUS:
            addr = NORM_CTRL3_ADDR;
            offset = 9;
            goto sts_chk;

        case FAL_NROM_IP4_IPID_RANDOM_STATUS:
            addr = NORM_CTRL3_ADDR;
            offset = 10;
            goto sts_chk;

        case FAL_NROM_IP6_INVALID_DIP_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 16;
            goto cmd_chk;

        case FAL_NROM_IP6_INVALID_SIP_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 17;
            goto cmd_chk;

        case FAL_NROM_IP6_INVALID_PL_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 18;
            goto cmd_chk;

        case FAL_NROM_TCP_BLAT_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 14;
            goto cmd_chk;

        case FAL_NROM_TCP_INVALID_HL_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 15;
            goto cmd_chk;

        case FAL_NROM_TCP_MIN_HDR_SIZE:
            addr = NORM_CTRL1_ADDR;
            offset = 12;
            len = 4;
            goto set_reg;

        case FAL_NROM_TCP_INVALID_SYN_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 16;
            goto cmd_chk;
            break;

        case FAL_NROM_TCP_SU_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 17;
            goto cmd_chk;

        case FAL_NROM_TCP_SP_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 18;
            goto cmd_chk;

        case FAL_NROM_TCP_SAP_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 19;
            goto cmd_chk;

        case FAL_NROM_TCP_XMAS_SCAN_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 20;
            goto cmd_chk;

        case FAL_NROM_TCP_NULL_SCAN_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 21;
            goto cmd_chk;

        case FAL_NROM_TCP_SR_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 22;
            goto cmd_chk;

        case FAL_NROM_TCP_SF_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 23;
            goto cmd_chk;

        case FAL_NROM_TCP_SAR_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 24;
            goto cmd_chk;

        case FAL_NROM_TCP_RST_SCAN_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 25;
            goto cmd_chk;

        case FAL_NROM_TCP_SYN_WITH_DATA_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 26;
            goto cmd_chk;

        case FAL_NROM_TCP_RST_WITH_DATA_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 27;
            goto cmd_chk;

        case FAL_NROM_TCP_FA_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 28;
            goto cmd_chk;

        case FAL_NROM_TCP_PA_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 29;
            goto cmd_chk;

        case FAL_NROM_TCP_UA_BLOCK_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 0;
            goto cmd_chk;

        case FAL_NROM_TCP_INVALID_CHKSUM_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 1;
            goto cmd_chk;

        case FAL_NROM_TCP_INVALID_URGPTR_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 2;
            goto cmd_chk;

        case FAL_NROM_TCP_INVALID_OPTIONS_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 3;
            goto cmd_chk;

        case FAL_NROM_UDP_BLAT_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 4;
            goto cmd_chk;

        case FAL_NROM_UDP_INVALID_LEN_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 5;
            goto cmd_chk;

        case FAL_NROM_UDP_INVALID_CHKSUM_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 6;
            goto cmd_chk;

        case FAL_NROM_ICMP4_PING_PL_EXCEED_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 7;
            goto cmd_chk;

        case FAL_NROM_ICMP6_PING_PL_EXCEED_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 8;
            goto cmd_chk;

        case FAL_NROM_ICMP4_PING_FRAG_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 9;
            goto cmd_chk;

        case FAL_NROM_ICMP6_PING_FRAG_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 10;
            goto cmd_chk;

        case FAL_NROM_ICMP4_PING_MAX_PL_VALUE:
            addr = NORM_CTRL2_ADDR;
            offset = 0;
            len = 14;
            goto set_reg;

        case FAL_NROM_ICMP6_PING_MAX_PL_VALUE:
            addr = NORM_CTRL2_ADDR;
            offset = 16;
            len = 14;
            goto set_reg;

        default:
            return SW_BAD_PARAM;
    }

sts_chk:
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
    goto set_reg;

s_cmd_chk:
    if (FAL_MAC_FRWRD == cmd)
    {
        val = 0;
    }
    else if (FAL_MAC_DROP == cmd)
    {
        val = 3;
    }
    else if (FAL_MAC_RDT_TO_CPU == cmd)
    {
        val = 2;
    }
    else
    {
        return SW_BAD_PARAM;
    }
    goto set_reg;

cmd_chk:
    if (FAL_MAC_FRWRD == cmd)
    {
        val = 0;
    }
    else if (FAL_MAC_DROP == cmd)
    {
        val = 1;
    }
    else
    {
        return SW_BAD_PARAM;
    }

set_reg:
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_REG_SET_BY_FIELD_U32(reg, val, offset, len);

    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_sec_norm_item_get(a_uint32_t dev_id, fal_norm_item_t item, void *value)
{
    sw_error_t rv;
    a_uint32_t addr, offset, len, reg = 0, val;
    a_uint32_t status_chk = 0, val_chk = 0, scmd_chk = 0;

    HSL_DEV_ID_CHECK(dev_id);

    len = 1;
    switch (item)
    {
        case FAL_NORM_MAC_RESV_VID_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 0;
            break;

        case FAL_NORM_MAC_INVALID_SRC_ADDR_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 20;
            break;

        case FAL_NORM_IP_INVALID_VER_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 1;
            break;

        case FAL_NROM_IP_SAME_ADDR_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 2;
            break;

        case FAL_NROM_IP_TTL_CHANGE_STATUS:
            addr = NORM_CTRL3_ADDR;
            offset = 11;
            status_chk = 1;
            break;

        case FAL_NROM_IP_TTL_VALUE:
            addr = NORM_CTRL3_ADDR;
            offset = 12;
            len = 8;
            val_chk = 1;
            break;

        case FAL_NROM_IP4_INVALID_HL_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 3;
            break;

        case FAL_NROM_IP4_HDR_OPTIONS_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 4;
            len = 2;
            scmd_chk = 1;
            break;

        case FAL_NROM_IP4_INVALID_DF_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 7;
            break;

        case FAL_NROM_IP4_FRAG_OFFSET_MIN_LEN_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 8;
            break;

        case FAL_NROM_IP4_FRAG_OFFSET_MIN_SIZE:
            addr = NORM_CTRL1_ADDR;
            offset = 24;
            len = 8;
            val_chk = 1;
            break;

        case FAL_NROM_IP4_FRAG_OFFSET_MAX_LEN_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 9;
            break;

        case FAL_NROM_IP4_INVALID_FRAG_OFFSET_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 10;
            break;

        case FAL_NROM_IP4_INVALID_SIP_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 11;
            len = 1;
            break;

        case FAL_NROM_IP4_INVALID_DIP_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 12;
            break;

        case FAL_NROM_IP4_INVALID_CHKSUM_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 13;
            break;

        case FAL_NROM_IP4_INVALID_PL_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 19;
            break;

        case FAL_NROM_IP4_DF_CLEAR_STATUS:
            addr = NORM_CTRL3_ADDR;
            offset = 9;
            status_chk = 1;
            break;

        case FAL_NROM_IP4_IPID_RANDOM_STATUS:
            addr = NORM_CTRL3_ADDR;
            offset = 10;
            status_chk = 1;
            break;

        case FAL_NROM_IP6_INVALID_DIP_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 16;
            break;

        case FAL_NROM_IP6_INVALID_SIP_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 17;
            break;

        case FAL_NROM_IP6_INVALID_PL_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 18;
            break;

        case FAL_NROM_TCP_BLAT_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 14;
            break;

        case FAL_NROM_TCP_INVALID_HL_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 15;
            break;

        case FAL_NROM_TCP_MIN_HDR_SIZE:
            addr = NORM_CTRL1_ADDR;
            offset = 12;
            len = 4;
            val_chk = 1;
            break;

        case FAL_NROM_TCP_INVALID_SYN_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 16;
            break;

        case FAL_NROM_TCP_SU_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 17;
            break;

        case FAL_NROM_TCP_SP_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 18;
            break;

        case FAL_NROM_TCP_SAP_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 19;
            break;

        case FAL_NROM_TCP_XMAS_SCAN_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 20;
            break;

        case FAL_NROM_TCP_NULL_SCAN_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 21;
            break;

        case FAL_NROM_TCP_SR_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 22;
            break;

        case FAL_NROM_TCP_SF_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 23;
            break;

        case FAL_NROM_TCP_SAR_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 24;
            break;

        case FAL_NROM_TCP_RST_SCAN_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 25;
            break;

        case FAL_NROM_TCP_SYN_WITH_DATA_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 26;
            break;

        case FAL_NROM_TCP_RST_WITH_DATA_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 27;
            break;

        case FAL_NROM_TCP_FA_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 28;
            break;

        case FAL_NROM_TCP_PA_BLOCK_CMD:
            addr = NORM_CTRL0_ADDR;
            offset = 29;
            break;

        case FAL_NROM_TCP_UA_BLOCK_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 0;
            break;

        case FAL_NROM_TCP_INVALID_CHKSUM_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 1;
            break;

        case FAL_NROM_TCP_INVALID_URGPTR_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 2;
            break;

        case FAL_NROM_TCP_INVALID_OPTIONS_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 3;
            break;

        case FAL_NROM_UDP_BLAT_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 4;
            break;

        case FAL_NROM_UDP_INVALID_LEN_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 5;
            break;

        case FAL_NROM_UDP_INVALID_CHKSUM_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 6;
            break;

        case FAL_NROM_ICMP4_PING_PL_EXCEED_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 7;
            break;

        case FAL_NROM_ICMP6_PING_PL_EXCEED_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 8;
            break;

        case FAL_NROM_ICMP4_PING_FRAG_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 9;
            break;

        case FAL_NROM_ICMP6_PING_FRAG_CMD:
            addr = NORM_CTRL1_ADDR;
            offset = 10;
            break;

        case FAL_NROM_ICMP4_PING_MAX_PL_VALUE:
            addr = NORM_CTRL2_ADDR;
            offset = 0;
            len = 14;
            val_chk = 1;
            break;

        case FAL_NROM_ICMP6_PING_MAX_PL_VALUE:
            addr = NORM_CTRL2_ADDR;
            offset = 16;
            len = 14;
            val_chk = 1;
            break;

        default:
            return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_FIELD_GET_BY_REG_U32(reg, val, offset, len);

    if (val_chk)
    {
        *((a_uint32_t *) value) = val;
    }
    else if (status_chk)
    {
        if (val)
        {
            *((a_bool_t *) value) = A_TRUE;
        }
        else
        {
            *((a_bool_t *) value) = A_FALSE;
        }
    }
    else if (scmd_chk)
    {
        if (2 == val)
        {
            *((fal_fwd_cmd_t *) value) = FAL_MAC_RDT_TO_CPU;
        }
        else if (3 == val)
        {
            *((fal_fwd_cmd_t *) value) = FAL_MAC_DROP;
        }
        else
        {
            *((fal_fwd_cmd_t *) value) = FAL_MAC_FRWRD;
        }
    }
    else
    {
        if (val)
        {
            *((fal_fwd_cmd_t *) value) = FAL_MAC_DROP;
        }
        else
        {
            *((fal_fwd_cmd_t *) value) = FAL_MAC_FRWRD;
        }
    }

    return SW_OK;
}

/**
 * @brief Set normalization particular item types value.
 * @details   Comments:
 *       This operation will set normalization item values on a particular device.
 *       The prototye of value based on the item type.
 * @param[in] dev_id device id
 * @param[in] item normalizaton item type
 * @param[in] value normalizaton item value
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_sec_norm_item_set(a_uint32_t dev_id, fal_norm_item_t item, void *value)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_sec_norm_item_set(dev_id, item, value);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get normalization particular item types value.
 * @details   Comments:
 *       This operation will set normalization item values on a particular device.
 *       The prototye of value based on the item type.
 * @param[in] dev_id device id
 * @param[in] item normalizaton item type
 * @param[out] value normalizaton item value
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_sec_norm_item_get(a_uint32_t dev_id, fal_norm_item_t item, void *value)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_sec_norm_item_get(dev_id, item, value);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
isisc_sec_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
        p_api->sec_norm_item_set = isisc_sec_norm_item_set;
        p_api->sec_norm_item_get = isisc_sec_norm_item_get;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */
