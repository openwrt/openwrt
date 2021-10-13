/*
 * Copyright (c) 2017-2019, The Linux Foundation. All rights reserved.
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


/*qca808x_start*/

#ifndef _SCOMPHY_REG_ACCESS_H_
#define _SCOMPHY_REG_ACCESS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "sw.h"

sw_error_t
scomphy_phy_get(a_uint32_t dev_id, a_uint32_t phy_addr,
		a_uint32_t reg, a_uint16_t * value);

sw_error_t
scomphy_phy_set(a_uint32_t dev_id, a_uint32_t phy_addr,
		a_uint32_t reg, a_uint16_t value);
/*qca808x_end*/

sw_error_t
scomphy_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr,
		a_uint8_t *val, a_uint32_t len);

sw_error_t
scomphy_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr,
		a_uint8_t *val, a_uint32_t len);

sw_error_t
scomphy_uniphy_reg_get(a_uint32_t dev_id, a_uint32_t index,
		a_uint32_t reg_addr, a_uint8_t *val, a_uint32_t len);

sw_error_t
scomphy_uniphy_reg_set(a_uint32_t dev_id, a_uint32_t index,
		a_uint32_t reg_addr, a_uint8_t *val, a_uint32_t len);

/*qca808x_start*/
sw_error_t
scomphy_reg_access_init(a_uint32_t dev_id, ssdk_init_cfg *cfg);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SCOMPHY_REG_ACCESS_H_ */
/*qca808x_end*/
