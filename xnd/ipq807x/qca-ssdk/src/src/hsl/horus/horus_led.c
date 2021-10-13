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
 * @defgroup horus_led HORUS_LED
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "horus_led.h"
#include "horus_reg.h"

#define MAX_LED_PATTERN_ID   1
#define LED_PATTERN_ADDR     0xB0

static sw_error_t
_horus_led_ctrl_pattern_set(a_uint32_t dev_id, led_pattern_group_t group,
                            led_pattern_id_t id, led_ctrl_pattern_t * pattern)
{
    a_uint32_t data = 0, reg, mode;
    a_uint32_t addr;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (group >= LED_GROUP_BUTT)
    {
        return SW_BAD_PARAM;
    }

    if (id > MAX_LED_PATTERN_ID)
    {
        return SW_BAD_PARAM;
    }

    if ((LED_MAC_PORT_GROUP == group) && (0 != id))
    {
        return SW_BAD_PARAM;
    }

    if (LED_MAC_PORT_GROUP == group)
    {
        addr = LED_PATTERN_ADDR + 8;
    }
    else
    {
        addr = LED_PATTERN_ADDR + (id << 2);
    }

    if (LED_ALWAYS_OFF == pattern->mode)
    {
        mode = 0;
    }
    else if (LED_ALWAYS_BLINK == pattern->mode)
    {
        mode = 1;
    }
    else if (LED_ALWAYS_ON == pattern->mode)
    {
        mode = 2;
    }
    else if (LED_PATTERN_MAP_EN == pattern->mode)
    {
        mode = 3;
    }
    else
    {
        return SW_BAD_PARAM;
    }
    SW_SET_REG_BY_FIELD(LED_CTRL, PATTERN_EN, mode, data);

    if (pattern->map & (1 << FULL_DUPLEX_LIGHT_EN))
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, FULL_LIGHT_EN, 1, data);
    }

    if (pattern->map & (1 << HALF_DUPLEX_LIGHT_EN))
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, HALF_LIGHT_EN, 1, data);
    }

    if (pattern->map & (1 << POWER_ON_LIGHT_EN))
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, POWERON_LIGHT_EN, 1, data);
    }

    if (pattern->map & (1 << LINK_1000M_LIGHT_EN))
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, GE_LIGHT_EN, 1, data);
    }

    if (pattern->map & (1 << LINK_100M_LIGHT_EN))
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, FE_LIGHT_EN, 1, data);
    }

    if (pattern->map & (1 << LINK_10M_LIGHT_EN))
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, ETH_LIGHT_EN, 1, data);
    }

    if (pattern->map & (1 << COLLISION_BLINK_EN))
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, COL_BLINK_EN, 1, data);
    }

    if (pattern->map & (1 << RX_TRAFFIC_BLINK_EN))
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, RX_BLINK_EN, 1, data);
    }

    if (pattern->map & (1 << TX_TRAFFIC_BLINK_EN))
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, TX_BLINK_EN, 1, data);
    }

    if (pattern->map & (1 << LINKUP_OVERRIDE_EN))
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, LINKUP_OVER_EN, 1, data);
    }
    else
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, LINKUP_OVER_EN, 0, data);
    }

    if (LED_BLINK_2HZ == pattern->freq)
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, BLINK_FREQ, 0, data);
    }
    else if (LED_BLINK_4HZ == pattern->freq)
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, BLINK_FREQ, 1, data);
    }
    else if (LED_BLINK_8HZ == pattern->freq)
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, BLINK_FREQ, 2, data);
    }
    else if (LED_BLINK_TXRX == pattern->freq)
    {
        SW_SET_REG_BY_FIELD(LED_CTRL, BLINK_FREQ, 3, data);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&reg),
                          sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (LED_WAN_PORT_GROUP == group)
    {
        reg &= 0xffff;
        reg |= (data << 16);
    }
    else
    {
        reg &= 0xffff0000;
        reg |= data;
    }

    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&reg),
                          sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (LED_WAN_PORT_GROUP == group)
    {
        return SW_OK;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, LED_PATTERN, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (LED_LAN_PORT_GROUP == group)
    {
        if (id)
        {
            SW_SET_REG_BY_FIELD(LED_PATTERN, P3L1_MODE, mode, data);
            SW_SET_REG_BY_FIELD(LED_PATTERN, P2L1_MODE, mode, data);
            SW_SET_REG_BY_FIELD(LED_PATTERN, P1L1_MODE, mode, data);
        }
        else
        {
            SW_SET_REG_BY_FIELD(LED_PATTERN, P3L0_MODE, mode, data);
            SW_SET_REG_BY_FIELD(LED_PATTERN, P2L0_MODE, mode, data);
            SW_SET_REG_BY_FIELD(LED_PATTERN, P1L0_MODE, mode, data);
        }
    }
    else
    {
        SW_SET_REG_BY_FIELD(LED_PATTERN, M5_MODE, mode, data);
    }

    HSL_REG_ENTRY_SET(rv, dev_id, LED_PATTERN, 0,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

static sw_error_t
_horus_led_ctrl_pattern_get(a_uint32_t dev_id, led_pattern_group_t group,
                            led_pattern_id_t id, led_ctrl_pattern_t * pattern)
{
    a_uint32_t data = 0, reg, tmp;
    a_uint32_t addr;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (group >= LED_GROUP_BUTT)
    {
        return SW_BAD_PARAM;
    }

    if (id > MAX_LED_PATTERN_ID)
    {
        return SW_BAD_PARAM;
    }

    if ((LED_MAC_PORT_GROUP == group) && (0 != id))
    {
        return SW_BAD_PARAM;
    }

    aos_mem_zero(pattern, sizeof(led_ctrl_pattern_t));

    if (LED_MAC_PORT_GROUP == group)
    {
        addr = LED_PATTERN_ADDR + 8;
    }
    else
    {
        addr = LED_PATTERN_ADDR + (id << 2);
    }

    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&reg),
                          sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (LED_WAN_PORT_GROUP == group)
    {
        data = (reg >> 16) & 0xffff;
    }
    else
    {
        data = reg & 0xffff;
    }

    SW_GET_FIELD_BY_REG(LED_CTRL, PATTERN_EN, tmp, data);
    if (0 == tmp)
    {
        pattern->mode = LED_ALWAYS_OFF;
    }
    else if (1 == tmp)
    {
        pattern->mode = LED_ALWAYS_BLINK;
    }
    else if (2 == tmp)
    {
        pattern->mode = LED_ALWAYS_ON;
    }
    else
    {
        pattern->mode = LED_PATTERN_MAP_EN;
    }

    SW_GET_FIELD_BY_REG(LED_CTRL, FULL_LIGHT_EN, tmp, data);
    if (1 == tmp)
    {
        pattern->map |= (1 << FULL_DUPLEX_LIGHT_EN);
    }

    SW_GET_FIELD_BY_REG(LED_CTRL, HALF_LIGHT_EN, tmp, data);
    if (1 == tmp)
    {
        pattern->map |= (1 << HALF_DUPLEX_LIGHT_EN);
    }

    SW_GET_FIELD_BY_REG(LED_CTRL, POWERON_LIGHT_EN, tmp, data);
    if (1 == tmp)
    {
        pattern->map |= (1 << POWER_ON_LIGHT_EN);
    }

    SW_GET_FIELD_BY_REG(LED_CTRL, GE_LIGHT_EN, tmp, data);
    if (1 == tmp)
    {
        pattern->map |= (1 << LINK_1000M_LIGHT_EN);
    }

    SW_GET_FIELD_BY_REG(LED_CTRL, FE_LIGHT_EN, tmp, data);
    if (1 == tmp)
    {
        pattern->map |= (1 << LINK_100M_LIGHT_EN);
    }

    SW_GET_FIELD_BY_REG(LED_CTRL, ETH_LIGHT_EN, tmp, data);
    if (1 == tmp)
    {
        pattern->map |= (1 << LINK_10M_LIGHT_EN);
    }

    SW_GET_FIELD_BY_REG(LED_CTRL, COL_BLINK_EN, tmp, data);
    if (1 == tmp)
    {
        pattern->map |= (1 << COLLISION_BLINK_EN);
    }

    SW_GET_FIELD_BY_REG(LED_CTRL, RX_BLINK_EN, tmp, data);
    if (1 == tmp)
    {
        pattern->map |= (1 << RX_TRAFFIC_BLINK_EN);
    }

    SW_GET_FIELD_BY_REG(LED_CTRL, TX_BLINK_EN, tmp, data);
    if (1 == tmp)
    {
        pattern->map |= (1 << TX_TRAFFIC_BLINK_EN);
    }

    SW_GET_FIELD_BY_REG(LED_CTRL, LINKUP_OVER_EN, tmp, data);
    if (1 == tmp)
    {
        pattern->map |= (1 << LINKUP_OVERRIDE_EN);
    }

    SW_GET_FIELD_BY_REG(LED_CTRL, BLINK_FREQ, tmp, data);
    if (0 == tmp)
    {
        pattern->freq = LED_BLINK_2HZ;
    }
    else if (1 == tmp)
    {
        pattern->freq = LED_BLINK_4HZ;
    }
    else if (2 == tmp)
    {
        pattern->freq = LED_BLINK_8HZ;
    }
    else
    {
        pattern->freq = LED_BLINK_TXRX;
    }

    return SW_OK;
}

/**
* @brief Set led control pattern on a particular device.
* @param[in] dev_id device id
* @param[in] group  pattern group, lan or wan
* @param[in] id pattern id
* @param[in] pattern led control pattern
* @return SW_OK or error code
*/
HSL_LOCAL sw_error_t
horus_led_ctrl_pattern_set(a_uint32_t dev_id, led_pattern_group_t group,
                           led_pattern_id_t id, led_ctrl_pattern_t * pattern)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _horus_led_ctrl_pattern_set(dev_id, group, id, pattern);
    HSL_API_UNLOCK;
    return rv;
}

/**
* @brief Get led control pattern on a particular device.
* @param[in] dev_id device id
* @param[in] group  pattern group, lan or wan
* @param[in] id pattern id
* @param[out] pattern led control pattern
* @return SW_OK or error code
*/
HSL_LOCAL sw_error_t
horus_led_ctrl_pattern_get(a_uint32_t dev_id, led_pattern_group_t group,
                           led_pattern_id_t id, led_ctrl_pattern_t * pattern)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _horus_led_ctrl_pattern_get(dev_id, group, id, pattern);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
horus_led_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
        p_api->led_ctrl_pattern_set = horus_led_ctrl_pattern_set;
        p_api->led_ctrl_pattern_get = horus_led_ctrl_pattern_get;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */
