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
 * @defgroup isis_trunk ISIS_TRUNK
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "isis_trunk.h"
#include "isis_reg.h"

#define ISIS_MAX_TRUNK_ID    3

/*feature on/off for manipulating dp within trunk group*/
#define ISIS_TRUNK_MANIPULATE_DP_ON       1
#define ISIS_TRUNK_MANIPULATE_HEADER_LEN 12
#define MAC_LEN                           6
#define HASH_SIZE                         4

enum isis_trunk_reg_id
{
    ISIS_TRUNK_HASH_EN = 0, /*0x270*/
    ISIS_TRUNK_CTRL_0,        /*0x700*/
    ISIS_TRUNK_CTRL_1,        /*0x704*/
    ISIS_TRUNK_CTRL_2,        /*0x708*/
    ISIS_TRUNK_REG_MAX
};

static a_uint32_t isis_trunk_regs[ISIS_TRUNK_REG_MAX] =
{
    0xf, 0x0, 0x0, 0x0
};

static a_uint8_t sa_hash[HASH_SIZE][MAC_LEN] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x03 }
};

static sw_error_t
_isis_trunk_group_set(a_uint32_t dev_id, a_uint32_t trunk_id,
                      a_bool_t enable, fal_pbmp_t member)
{
    sw_error_t rv;
    a_uint32_t i, reg = 0, cnt = 0, data0 = 0, data1 = 0;

    if (ISIS_MAX_TRUNK_ID < trunk_id)
    {
        return SW_BAD_PARAM;
    }

    if (A_FALSE == hsl_mports_prop_check(dev_id, member, HSL_PP_INCL_CPU))
    {
        return SW_BAD_PARAM;
    }

    if (A_TRUE == enable)
    {
        data0 = (0x1 << 7) | member;

        for (i = 0; i < 7; i++)
        {
            if (member & (0x1 << i))
            {
                if (4 <= cnt)
                {
                    return SW_BAD_PARAM;
                }

                data1 |= (i << (cnt << 2));
                data1 |= (1 << (3 + (cnt << 2)));
                cnt++;
            }
        }
    }
    else if (A_FALSE == enable)
    {

    }
    else
    {
        return SW_BAD_PARAM;
    }

    /* set trunk port member bitmap info */
    HSL_REG_ENTRY_GET(rv, dev_id, GOL_TRUNK_CTL0, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    reg &= (~(0xff << (trunk_id << 3)));
    reg |= (data0 << (trunk_id << 3));

    HSL_REG_ENTRY_SET(rv, dev_id, GOL_TRUNK_CTL0, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    isis_trunk_regs[ISIS_TRUNK_CTRL_0] = reg;

    /* set trunk port member id info */
    HSL_REG_ENTRY_GET(rv, dev_id, GOL_TRUNK_CTL1, (trunk_id >> 1),
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    reg &= (~(0xffff << ((trunk_id % 2) << 4)));
    reg |= (data1 << ((trunk_id % 2) << 4));

    HSL_REG_ENTRY_SET(rv, dev_id, GOL_TRUNK_CTL1, (trunk_id >> 1),
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    isis_trunk_regs[ISIS_TRUNK_CTRL_1 + (trunk_id >> 1)] = reg;

    return SW_OK;
}

static sw_error_t
_isis_trunk_group_get(a_uint32_t dev_id, a_uint32_t trunk_id,
                      a_bool_t * enable, fal_pbmp_t * member)
{
    sw_error_t rv;
    a_uint32_t data, reg = 0;

    if (ISIS_MAX_TRUNK_ID < trunk_id)
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, GOL_TRUNK_CTL0, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data = (reg >> (trunk_id << 3)) & 0xff;
    if (0x80 & data)
    {
        *enable = A_TRUE;
        *member = data & 0x7f;
    }
    else
    {
        *enable = A_FALSE;
        *member = 0;
    }

    return SW_OK;
}

#if 0
static sw_error_t
_isis_trunk_group_sw_get(a_uint32_t dev_id, a_uint32_t trunk_id,
                         a_bool_t * enable, fal_pbmp_t * member)
{
    a_uint32_t data, reg;

    if (ISIS_MAX_TRUNK_ID < trunk_id)
    {
        return SW_BAD_PARAM;
    }

    reg = isis_trunk_regs[ISIS_TRUNK_CTRL_0];

    data = (reg >> (trunk_id << 3)) & 0xff;
    if (0x80 & data)
    {
        *enable = A_TRUE;
        *member = data & 0x7f;
    }
    else
    {
        *enable = A_FALSE;
        *member = 0;
    }

    return SW_OK;
}
#endif

static sw_error_t
_isis_trunk_hash_mode_set(a_uint32_t dev_id, a_uint32_t hash_mode)
{
    sw_error_t rv;
    a_uint32_t data = 0;

    if (FAL_TRUNK_HASH_KEY_DA & hash_mode)
    {
        SW_SET_REG_BY_FIELD(TRUNK_HASH_MODE, DA_EN, 1, data);
    }

    if (FAL_TRUNK_HASH_KEY_SA & hash_mode)
    {
        SW_SET_REG_BY_FIELD(TRUNK_HASH_MODE, SA_EN, 1, data);
    }

    if (FAL_TRUNK_HASH_KEY_DIP & hash_mode)
    {
        SW_SET_REG_BY_FIELD(TRUNK_HASH_MODE, DIP_EN, 1, data);
    }

    if (FAL_TRUNK_HASH_KEY_SIP & hash_mode)
    {
        SW_SET_REG_BY_FIELD(TRUNK_HASH_MODE, SIP_EN, 1, data);
    }

    HSL_REG_ENTRY_SET(rv, dev_id, TRUNK_HASH_MODE, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    isis_trunk_regs[ISIS_TRUNK_HASH_EN] = data;

    return rv;
}

static sw_error_t
_isis_trunk_hash_mode_get(a_uint32_t dev_id, a_uint32_t * hash_mode)
{
    sw_error_t rv;
    a_uint32_t reg = 0, data = 0;

    HSL_REG_ENTRY_GET(rv, dev_id, TRUNK_HASH_MODE, 0,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *hash_mode = 0;

    SW_GET_FIELD_BY_REG(TRUNK_HASH_MODE, DA_EN, data, reg);
    if (data)
    {
        *hash_mode |= FAL_TRUNK_HASH_KEY_DA;
    }

    SW_GET_FIELD_BY_REG(TRUNK_HASH_MODE, SA_EN, data, reg);
    if (data)
    {
        *hash_mode |= FAL_TRUNK_HASH_KEY_SA;
    }

    SW_GET_FIELD_BY_REG(TRUNK_HASH_MODE, DIP_EN, data, reg);
    if (data)
    {
        *hash_mode |= FAL_TRUNK_HASH_KEY_DIP;
    }

    SW_GET_FIELD_BY_REG(TRUNK_HASH_MODE, SIP_EN, data, reg);
    if (data)
    {
        *hash_mode |= FAL_TRUNK_HASH_KEY_SIP;
    }

    return SW_OK;
}

#define BYTE_B2R(x, mask) ((x) ^ (mask))
#define BYTE_B1C(x) ((((((x&0x55)+((x&0xaa)>>1))&0x33)+((((x&0x55)+((x&0xaa)>>1))&0xcc)>>2))&0x0f)+((((((x&0x55)+((x&0xaa)>>1))&0x33)+((((x&0x55)+((x&0xaa)>>1))&0xcc)>>2))&0xf0)>>4))

static sw_error_t
_isis_trunk_manipulate_sa_set(a_uint32_t dev_id, fal_mac_addr_t * addr)
{
    a_uint32_t i;

    for (i = 0; i < HASH_SIZE; i++)
    {
        memcpy(sa_hash[i], addr->uc, MAC_LEN);
        sa_hash[i][MAC_LEN - 1] = BYTE_B2R(sa_hash[i][MAC_LEN - 1], i);
    }

    return SW_OK;
}

static sw_error_t
_isis_trunk_manipulate_sa_get(a_uint32_t dev_id, fal_mac_addr_t * addr)
{
    memcpy(addr->uc, sa_hash[0], MAC_LEN);
    return SW_OK;
}

#if 0
static sw_error_t
_isis_trunk_hash_mode_sw_get(a_uint32_t dev_id, a_uint32_t * hash_mode)
{
    a_uint32_t reg, data = 0;

    reg = isis_trunk_regs[ISIS_TRUNK_HASH_EN];

    *hash_mode = 0;

    SW_GET_FIELD_BY_REG(TRUNK_HASH_MODE, DA_EN, data, reg);
    if (data)
    {
        *hash_mode |= FAL_TRUNK_HASH_KEY_DA;
    }

    SW_GET_FIELD_BY_REG(TRUNK_HASH_MODE, SA_EN, data, reg);
    if (data)
    {
        *hash_mode |= FAL_TRUNK_HASH_KEY_SA;
    }

    SW_GET_FIELD_BY_REG(TRUNK_HASH_MODE, DIP_EN, data, reg);
    if (data)
    {
        *hash_mode |= FAL_TRUNK_HASH_KEY_DIP;
    }

    SW_GET_FIELD_BY_REG(TRUNK_HASH_MODE, SIP_EN, data, reg);
    if (data)
    {
        *hash_mode |= FAL_TRUNK_HASH_KEY_SIP;
    }

    return SW_OK;
}

static sw_error_t
_isis_trunk_id_member_get(a_uint32_t dev_id, a_uint8_t expect_dp,
                          a_uint32_t * trunk_id, fal_pbmp_t * member)
{
    sw_error_t rv;
    a_bool_t   enable;
    a_uint32_t i;

    for (i = 0; i <= ISIS_MAX_TRUNK_ID; i++)
    {
        rv = _isis_trunk_group_sw_get(dev_id, i, &enable, member);
        SW_RTN_ON_ERROR(rv);
        if (enable && (*member & expect_dp))
        {
            *trunk_id = i;
            return SW_OK;
        }
    }

    return SW_NOT_FOUND;
}

static sw_error_t
_isis_trunk_hash_dp_get(a_uint32_t dev_id, a_uint8_t * header, a_uint32_t len,
                        a_uint32_t trunk_id, a_uint32_t mode, a_uint8_t * hash_dp)
{
#define BIT2_MASK 0x03
#define TRUNK_MEM_EN_MASK 0x8
#define TRUNK_MEM_PT_MASK 0x7
#define TRUNK_HASH_DP_SEL   4
    sw_error_t rv;
    a_uint32_t i, hash_mode, reg, data1 = 0;
    a_uint32_t da_xor = 0, sa_xor = 0; /*consider da-hash & sa-hash (TBD: dip-hash & sip-hash)*/
    a_uint8_t  xor_dp = 0;

    rv = _isis_trunk_hash_mode_sw_get(dev_id, &hash_mode);
    SW_RTN_ON_ERROR(rv);

    if (!hash_mode)
    {
        return SW_DISABLE;
    }

    *hash_dp = 0;

    if ((mode & FAL_TRUNK_HASH_KEY_DA) && (hash_mode & FAL_TRUNK_HASH_KEY_DA))
    {
        for (i = 0; i < MAC_LEN; i++)
        {
            da_xor ^= (header[i] & BIT2_MASK) ^
                      ((header[i] >> 2) & BIT2_MASK) ^
                      ((header[i] >> 4) & BIT2_MASK) ^
                      ((header[i] >> 6) & BIT2_MASK);
        }
        *hash_dp = da_xor;
    }
    if ((mode & FAL_TRUNK_HASH_KEY_SA) && (hash_mode & FAL_TRUNK_HASH_KEY_SA))
    {
        for (i = 6; i < 2 * MAC_LEN; i++)
        {
            sa_xor ^= (header[i] & BIT2_MASK) ^
                      ((header[i] >> 2) & BIT2_MASK) ^
                      ((header[i] >> 4) & BIT2_MASK) ^
                      ((header[i] >> 6) & BIT2_MASK);
        }
        *hash_dp = (*hash_dp) ^ sa_xor;
    }

    /*dp translation*/
#if 0
    HSL_REG_ENTRY_GET(rv, dev_id, GOL_TRUNK_CTL1, (trunk_id >> 1),
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
#else /*sw*/
    reg = isis_trunk_regs[ISIS_TRUNK_CTRL_1 + (trunk_id >> 1)];
#endif

    for (i = 0; i < TRUNK_HASH_DP_SEL; i++)
    {
        xor_dp = BYTE_B2R(*hash_dp, i);
        data1 = (0x0f & (reg >> (((trunk_id % 2) << 4) + (xor_dp << 2))));
        if (data1 & TRUNK_MEM_EN_MASK)
        {
            *hash_dp = data1 & TRUNK_MEM_PT_MASK;
            *hash_dp = 0x01 << (*hash_dp); /*bmp*/
            return SW_OK;
        }
    }

    return SW_NOT_FOUND;
}

static sw_error_t
_isis_trunk_sa_spoofing( a_uint32_t dev_id, a_uint8_t * header, a_uint32_t len,
                         a_uint8_t expect_dp, a_uint32_t trunk_id, fal_pbmp_t member)
{
    sw_error_t rv;
    a_uint32_t i, hash_mode;
    a_uint8_t  hash_dp;
    a_uint8_t  ori_sa[MAC_LEN];

    rv = _isis_trunk_hash_mode_sw_get(dev_id, &hash_mode);
    SW_RTN_ON_ERROR(rv);

    if (!(hash_mode & FAL_TRUNK_HASH_KEY_SA))
    {
        return SW_DISABLE;
    }

    memcpy(ori_sa, &header[MAC_LEN], MAC_LEN);

    for (i = 0; i < HASH_SIZE/*not HASH_SIZE, for RAD only*/; i++)
    {
        memcpy(&header[MAC_LEN], sa_hash[i], MAC_LEN);
        rv = _isis_trunk_hash_dp_get(dev_id, header, len, trunk_id,
                                     FAL_TRUNK_HASH_KEY_DA | FAL_TRUNK_HASH_KEY_SA, &hash_dp);
        SW_RTN_ON_ERROR(rv);
        if (expect_dp == hash_dp)
        {
            // printk("expect_dp = 0x%x, hash_dp(DA+SA) = 0x%x, sa_id = %d\n", expect_dp, hash_dp, i);
            return SW_OK;
        }
    }

    /*should never here*/
    memcpy(&header[MAC_LEN], ori_sa, MAC_LEN);
    return SW_FAIL;
}

static sw_error_t
_isis_trunk_manipulate_dp(a_uint32_t dev_id, a_uint8_t * header,
                          a_uint32_t len, fal_pbmp_t dp_member)
{
    sw_error_t rv;
    a_uint8_t  expect_dp, hash_dp; /*bitmap*/
    a_uint32_t i, trunk_id;
    fal_pbmp_t member;

    if (!ISIS_TRUNK_MANIPULATE_DP_ON)
    {
        return SW_OK; /*feature not enabled*/
    }

    if (!header || len < ISIS_TRUNK_MANIPULATE_HEADER_LEN)
    {
        return SW_BAD_VALUE;
    }

#if 0 /*de-comment this to ignore broadcast packets*/
    const a_uint8_t  bc_mac[MAC_LEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    if (!memcmp(header, bc_mac, MAC_LEN))   /*not for broadcast*/
    {
        return SW_OK;
    }
#endif

    /*expect_dp within trunk group*/
    expect_dp = dp_member & 0x7f;
    for (i = 0; i < 7; i++)
    {
        if (expect_dp & (0x01 << i))
        {
            rv = _isis_trunk_id_member_get(dev_id, (0x01 << i), &trunk_id, &member);
            if (rv != SW_OK)
            {
                expect_dp &= ~(0x01 << i); /*not the dp doesn't belong to trunk*/
            }
        }
    }

    if (BYTE_B1C(expect_dp) != 1)   /*supports 1 dp only*/
    {
        return SW_OK; /*ignore none-dp or multi-dp*/
    }

    rv = _isis_trunk_id_member_get(dev_id, expect_dp, &trunk_id, &member);
    SW_RTN_ON_ERROR(rv);

    member &= 0x7f;
    if (BYTE_B1C(member) == 1)   /*trunk group w/ one port*/
    {
        return SW_OK;
    }

    rv = _isis_trunk_hash_dp_get(dev_id, header, len, trunk_id,
                                 FAL_TRUNK_HASH_KEY_DA | FAL_TRUNK_HASH_KEY_SA, &hash_dp);
    SW_RTN_ON_ERROR(rv);

    // printk("expect_dp = 0x%x, hash_dp(DA+SA) = 0x%x, member = 0x%x\n", expect_dp, hash_dp, member);
    if (expect_dp == hash_dp)
    {
        return SW_OK;
    }

    rv = _isis_trunk_sa_spoofing(dev_id, header, len, expect_dp, trunk_id, member);
    SW_RTN_ON_ERROR(rv);

    return rv;
}
#endif

/**
 * @brief Set particular trunk group information on particular device.
 * @param[in] dev_id device id
 * @param[in] trunk_id trunk group id
 * @param[in] enable trunk group status, enable or disable
 * @param[in] member port member information
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_trunk_group_set(a_uint32_t dev_id, a_uint32_t trunk_id,
                     a_bool_t enable, fal_pbmp_t member)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_trunk_group_set(dev_id, trunk_id, enable, member);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get particular trunk group information on particular device.
 * @param[in] dev_id device id
 * @param[in] trunk_id trunk group id
 * @param[out] enable trunk group status, enable or disable
 * @param[out] member port member information
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_trunk_group_get(a_uint32_t dev_id, a_uint32_t trunk_id,
                     a_bool_t * enable, fal_pbmp_t * member)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_trunk_group_get(dev_id, trunk_id, enable, member);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set trunk hash mode on particular device.
 * @details   Comments:
    hash mode is listed below
    FAL_TRUNK_HASH_KEY_DA, FAL_TRUNK_HASH_KEY_SA, FAL_TRUNK_HASH_KEY_DIP and FAL_TRUNK_HASH_KEY_SIP
 * @param[in] dev_id device id
 * @param[in] hash_mode trunk hash mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_trunk_hash_mode_set(a_uint32_t dev_id, a_uint32_t hash_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_trunk_hash_mode_set(dev_id, hash_mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get trunk hash mode on particular device.
 * @param[in] dev_id device id
 * @param[out] hash_mode trunk hash mode
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_trunk_hash_mode_get(a_uint32_t dev_id, a_uint32_t * hash_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_trunk_hash_mode_get(dev_id, hash_mode);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set trunk manipulate SA on particular device.
 * @param[in] dev_id device id
 * @param[in] addr   manipulate SA
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_trunk_manipulate_sa_set(a_uint32_t dev_id, fal_mac_addr_t * addr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_trunk_manipulate_sa_set(dev_id, addr);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get trunk manipulate SA on particular device.
 * @param[in]  dev_id device id
 * @param[out] addr   manipulate SA
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_trunk_manipulate_sa_get(a_uint32_t dev_id, fal_mac_addr_t * addr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_trunk_manipulate_sa_get(dev_id, addr);
    HSL_API_UNLOCK;
    return rv;
}

#if 0
/**
 * @brief manipulate destination port within a trunk group
 * @details Comments:
 *  supporting hash mode include: FAL_TRUNK_HASH_KEY_DA & FAL_TRUNK_HASH_KEY_SA;
 *  FAL_TRUNK_HASH_KEY_DIP & FAL_TRUNK_HASH_KEY_SIP are NOT covered in current design
 * @param[in]     dev_id    device id
 * @param[in-out] header    packet header, accept format: [DA:6B][SA:6B]
 * @param[in]     len       length of packet header, should be 12 in current design (6B DA + 6B SA)
 * @param[in]     dp_member expect destination port members, bitmap format
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_trunk_manipulate_dp(a_uint32_t dev_id, a_uint8_t * header,
                         a_uint32_t len, fal_pbmp_t dp_member)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_trunk_manipulate_dp(dev_id, header, len, dp_member);
    HSL_API_UNLOCK;
    return rv;
}
#endif

sw_error_t
isis_trunk_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
        p_api->trunk_group_set = isis_trunk_group_set;
        p_api->trunk_group_get = isis_trunk_group_get;
        p_api->trunk_hash_mode_set = isis_trunk_hash_mode_set;
        p_api->trunk_hash_mode_get = isis_trunk_hash_mode_get;
        p_api->trunk_manipulate_sa_set = isis_trunk_manipulate_sa_set;
        p_api->trunk_manipulate_sa_get = isis_trunk_manipulate_sa_get;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

