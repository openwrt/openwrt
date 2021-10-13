/*
 * Copyright (c) 2012,2018,2020, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_led FAL_LED
 * @{
 */
#ifndef _FAL_LED_H_
#define _FAL_LED_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "sw.h"
#include "fal/fal_type.h"

    /**
    @brief This enum defines the led group.
    */
    typedef enum {
        LED_LAN_PORT_GROUP = 0,   /**<   control lan ports*/
        LED_WAN_PORT_GROUP,       /**<   control wan ports*/
        LED_MAC_PORT_GROUP,       /**<   control mac ports*/
        LED_GROUP_BUTT
    }
    led_pattern_group_t;

    /**
    @brief This enum defines the led pattern id, each ports has three led
           and pattern0 relates to led0, pattern1 relates to led1, pattern2 relates to led2.
    */
    typedef a_uint32_t led_pattern_id_t;


    /**
    @brief This enum defines the led control pattern mode.
    */
    typedef enum
    {
        LED_ALWAYS_OFF = 0,
        LED_ALWAYS_BLINK,
        LED_ALWAYS_ON,
        LED_PATTERN_MAP_EN,
        LED_PATTERN_MODE_BUTT
    } led_pattern_mode_t;


#define     FULL_DUPLEX_LIGHT_EN   0
#define     HALF_DUPLEX_LIGHT_EN   1
#define     POWER_ON_LIGHT_EN      2
#define     LINK_1000M_LIGHT_EN    3
#define     LINK_100M_LIGHT_EN     4
#define     LINK_10M_LIGHT_EN      5
#define     COLLISION_BLINK_EN     6
#define     RX_TRAFFIC_BLINK_EN    7
#define     TX_TRAFFIC_BLINK_EN    8
#define     LINKUP_OVERRIDE_EN     9
#define     LED_ACTIVE_HIGH        10
#define     LINK_2500M_LIGHT_EN    11
#define     LED_MAP_10M_SPEED \
    (BIT(POWER_ON_LIGHT_EN) | BIT(LINK_10M_LIGHT_EN) | BIT(COLLISION_BLINK_EN) |\
    BIT(RX_TRAFFIC_BLINK_EN) | BIT(TX_TRAFFIC_BLINK_EN) | BIT(LINKUP_OVERRIDE_EN))
#define     LED_MAP_100M_SPEED \
    (BIT(POWER_ON_LIGHT_EN) | BIT(LINK_100M_LIGHT_EN) | BIT(COLLISION_BLINK_EN) |\
    BIT(RX_TRAFFIC_BLINK_EN) | BIT(TX_TRAFFIC_BLINK_EN) | BIT(LINKUP_OVERRIDE_EN))
#define     LED_MAP_1000M_SPEED \
    (BIT(POWER_ON_LIGHT_EN) | BIT(LINK_1000M_LIGHT_EN) | BIT(COLLISION_BLINK_EN) |\
    BIT(RX_TRAFFIC_BLINK_EN) | BIT(TX_TRAFFIC_BLINK_EN) | BIT(LINKUP_OVERRIDE_EN))
#define     LED_MAP_2500M_SPEED \
    (BIT(POWER_ON_LIGHT_EN) | BIT(LINK_2500M_LIGHT_EN) | BIT(COLLISION_BLINK_EN) |\
    BIT(RX_TRAFFIC_BLINK_EN) | BIT(TX_TRAFFIC_BLINK_EN) | BIT(LINKUP_OVERRIDE_EN))
#define     LED_MAP_ALL_SPEED \
    (LED_MAP_10M_SPEED | LED_MAP_100M_SPEED | LED_MAP_1000M_SPEED |\
    LED_MAP_2500M_SPEED)

#define     PORT_LED_SOURCE_MAX    0x3

    /**
    @brief This enum defines the led control pattern map.
    */
    typedef a_uint32_t led_pattern_map_t;


    /**
    @brief This enum defines the led control pattern mode.
    */
    typedef enum
    {
        LED_BLINK_2HZ = 0,
        LED_BLINK_4HZ,
        LED_BLINK_8HZ,
        LED_BLINK_TXRX,    /**< Frequency relates to speed, 1000M-8HZ,100M->4HZ,10M->2HZ,Others->4HZ */
        LED_BLINK_FREQ_BUTT
    } led_blink_freq_t;


    typedef struct
    {
        led_pattern_mode_t  mode;
        led_pattern_map_t   map;
        led_blink_freq_t    freq;
    } led_ctrl_pattern_t;





    sw_error_t
    fal_led_ctrl_pattern_set(a_uint32_t dev_id, led_pattern_group_t group,
                             led_pattern_id_t id, led_ctrl_pattern_t * pattern);



    sw_error_t
    fal_led_ctrl_pattern_get(a_uint32_t dev_id, led_pattern_group_t group,
                             led_pattern_id_t id, led_ctrl_pattern_t * pattern);

	sw_error_t
	fal_led_source_pattern_set(a_uint32_t dev_id, a_uint32_t source_id,
                            led_ctrl_pattern_t * pattern);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_LED_H_ */
/**
 * @}
 */

