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

/**
 * @defgroup
 * @{
 */

#ifndef MP_UNIPHY_REG_H
#define MP_UNIPHY_REG_H

#define UNIPHY_CLOCK_OUTPUT_CONTROL_MAX_ENTRY                   1
/*[register] UNIPHY_ALLREG_DEC_REFCLKOUTPUTCONTROLREGISTERS*/
#define UNIPHY_CLOCK_OUTPUT_CONTROL
#define UNIPHY_CLOCK_OUTPUT_CONTROL_ADDRESS                     0x74
#define UNIPHY_CLOCK_OUTPUT_CONTROL_NUM                         0x1
#define UNIPHY_CLOCK_OUTPUT_CONTROL_INC                         0x1
#define UNIPHY_CLOCK_OUTPUT_CONTROL_TYPE                        REG_TYPE_RW
#define UNIPHY_CLOCK_OUTPUT_CONTROL_DEFAULT                     0x5
	/*[field] MMD1_REG_REFCLK_OUTPUT_EN*/
	#define MMD1_REG_REFCLK_OUTPUT_EN
	#define MMD1_REG_REFCLK_OUTPUT_EN_OFFSET                    0
	#define MMD1_REG_REFCLK_OUTPUT_EN_LEN                       1
	#define MMD1_REG_REFCLK_OUTPUT_EN_DEFAULT                   0x1
	/*[field] MMD1_REG_REFCLK_OUTPUT_DIV*/
	#define MMD1_REG_REFCLK_OUTPUT_DIV
	#define MMD1_REG_REFCLK_OUTPUT_DIV_OFFSET                   1
	#define MMD1_REG_REFCLK_OUTPUT_DIV_LEN                      1
	#define MMD1_REG_REFCLK_OUTPUT_DIV_DEFAULT                  0x0
	/*[field] MMD1_REG_REFCLK_OUTPUT_DRV*/
	#define MMD1_REG_REFCLK_OUTPUT_DRV
	#define MMD1_REG_REFCLK_OUTPUT_DRV_OFFSET                   2
	#define MMD1_REG_REFCLK_OUTPUT_DRV_LEN                      2
	#define MMD1_REG_REFCLK_OUTPUT_DRV_DEFAULT                  0x1

struct uniphy_clock_output_control {
	a_uint32_t  ref_clk_output_en:1;
	a_uint32_t  ref_clk_output_div:1;
	a_uint32_t  ref_clk_output_drv:2;
	a_uint32_t  _reserved1:28;
};

union uniphy_clock_output_control_u {
	a_uint32_t val;
	struct uniphy_clock_output_control bf;
};
#endif
