/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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
 */

#ifndef _ADPT_MP_H_
#define _ADPT_MP_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#define MP_PORT_TO_GMAC_ID(port_id) (port_id -1)
#define MP_MAX_PORT 2
#define MP_GMAC0 0
#define MP_GMAC1 1
#define MP_PORT_ID_CHECK(port_id) \
do { \
    if (port_id > MP_MAX_PORT) \
        return SW_OUT_OF_RANGE; \
} while (0)

sw_error_t adpt_mp_intr_init(a_uint32_t dev_id);
sw_error_t adpt_mp_mib_init(a_uint32_t dev_id);
sw_error_t adpt_mp_portctrl_init(a_uint32_t dev_id);
sw_error_t adpt_mp_uniphy_init(a_uint32_t dev_id);
sw_error_t adpt_mp_led_init(a_uint32_t dev_id);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif
