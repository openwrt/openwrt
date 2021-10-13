/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
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

#ifndef _SFP_PHY_H_
#define _SFP_PHY_H_

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */


#define SFP_ANEG_DONE	0x20

#define SFP_E2PROM_ADDR 0x50
#define SFP_SPEED_ADDR  0xc
#define SFP_SPEED_1000M 10
#define SFP_SPEED_2500M 25
#define SFP_SPEED_10000M 100

#define SFP_TO_SFP_SPEED(reg_data) ((reg_data >> 8) & 0xff)

int sfp_phy_device_setup(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t phy_id);
void sfp_phy_device_remove(a_uint32_t dev_id, a_uint32_t port);

int sfp_phy_init(a_uint32_t dev_id, a_uint32_t port_bmp);
void sfp_phy_exit(a_uint32_t dev_id, a_uint32_t port_bmp);

sw_error_t sfp_phy_interface_get_mode_status(a_uint32_t dev_id, a_uint32_t phy_id,
	fal_port_interface_mode_t *interface_mode);

#ifdef __cplusplus
}
#endif				/* __cplusplus */
#endif				/* _SFP_PHY_H_ */

