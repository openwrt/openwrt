/*
 * Copyright (c) 2014, 2017, The Linux Foundation. All rights reserved.
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



#ifndef _SD_H_
#define _SD_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

    sw_error_t
    sd_reg_mdio_set(a_uint32_t dev_id, a_uint32_t phy, a_uint32_t reg,
                    a_uint16_t data);

    sw_error_t
    sd_reg_mdio_get(a_uint32_t dev_id, a_uint32_t phy, a_uint32_t reg,
                    a_uint16_t * data);

    sw_error_t
    sd_reg_hdr_set(a_uint32_t dev_id, a_uint32_t reg_addr,
                   a_uint8_t * reg_data, a_uint32_t len);

    sw_error_t
    sd_reg_hdr_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                   a_uint8_t * reg_data, a_uint32_t len);
	sw_error_t
	sd_reg_uniphy_set(a_uint32_t dev_id, a_uint32_t index,
		a_uint32_t reg_addr, a_uint8_t * reg_data, a_uint32_t len);

	sw_error_t
	sd_reg_uniphy_get(a_uint32_t dev_id, a_uint32_t index,
		a_uint32_t reg_addr, a_uint8_t * reg_data, a_uint32_t len);

	void
	sd_reg_mii_set(a_uint32_t reg, a_uint32_t val);

	a_uint32_t
	sd_reg_mii_get(a_uint32_t reg);

    sw_error_t sd_init(a_uint32_t dev_id, ssdk_init_cfg * cfg);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _SD_H_ */
