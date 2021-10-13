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
 * @defgroup shiva_init SHIVA_INIT
 * @{
 */
#ifndef _SHIVA_INIT_H_
#define _SHIVA_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "init/ssdk_init.h"


    sw_error_t
    shiva_init(a_uint32_t dev_id, ssdk_init_cfg *cfg);


    sw_error_t
    shiva_cleanup(a_uint32_t dev_id);

#ifdef HSL_STANDALONG

    HSL_LOCAL sw_error_t
    shiva_reset(a_uint32_t dev_id);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SHIVA_INIT_H_ */
/**
 * @}
 */
