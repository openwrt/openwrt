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


#ifndef _ISISC_MIB_H_
#define _ISISC_MIB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "fal/fal_mib.h"

    sw_error_t
    isisc_mib_init(a_uint32_t dev_id);

#ifdef IN_MIB
#define ISISC_MIB_INIT(rv, dev_id) \
    { \
        rv = isisc_mib_init(dev_id); \
        SW_RTN_ON_ERROR(rv); \
    }
#else
#define ISISC_MIB_INIT(rv, dev_id)
#endif

#ifdef HSL_STANDALONG

    HSL_LOCAL sw_error_t
    isisc_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                      fal_mib_info_t * mib_info );


    HSL_LOCAL sw_error_t
    isisc_mib_status_set(a_uint32_t dev_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    isisc_mib_status_get(a_uint32_t dev_id, a_bool_t * enable);

    HSL_LOCAL sw_error_t
    isisc_mib_port_flush_counters(a_uint32_t dev_id, fal_port_t port_id);

    HSL_LOCAL sw_error_t
    isisc_mib_cpukeep_set(a_uint32_t dev_id, a_bool_t enable);


    HSL_LOCAL sw_error_t
    isisc_mib_cpukeep_get(a_uint32_t dev_id, a_bool_t * enable);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ISISC_MIB_H_ */

