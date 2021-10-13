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


#ifndef _GARUDA_LED_H_
#define _GARUDA_LED_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "fal/fal_led.h"

    sw_error_t
    garuda_led_init(a_uint32_t dev_id);

#ifdef IN_LED
#define GARUDA_LED_INIT(rv, dev_id) \
    { \
        rv = garuda_led_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define GARUDA_LED_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG


    HSL_LOCAL sw_error_t
    garuda_led_ctrl_pattern_set(a_uint32_t dev_id, led_pattern_group_t group,
                                led_pattern_id_t id, led_ctrl_pattern_t * pattern);



    HSL_LOCAL sw_error_t
    garuda_led_ctrl_pattern_get(a_uint32_t dev_id, led_pattern_group_t group,
                                led_pattern_id_t id, led_ctrl_pattern_t * pattern);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _GARUDA_LED_H_ */
