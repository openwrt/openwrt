/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (c) 2025 Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided as is and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */

#ifndef __SMI_H__
#define __SMI_H__

#include <rtk_types.h>
#include <rtk_error.h>

struct mii_bus;

#ifdef EASYSMART_SDK
#include <inttypes.h>
#endif

#define MDC_MDIO_CTRL0_REG          31
#define MDC_MDIO_START_REG          29
#define MDC_MDIO_CTRL1_REG          21
#define MDC_MDIO_ADDRESS_REG        23
#define MDC_MDIO_DATA_WRITE_REG     24
#define MDC_MDIO_DATA_READ_REG      25
#define MDC_MDIO_PREAMBLE_LEN       32

#define MDC_MDIO_START_OP          0xFFFF
#define MDC_MDIO_ADDR_OP           0x000E
#define MDC_MDIO_READ_OP           0x0001
#define MDC_MDIO_WRITE_OP          0x0003

#define SPI_READ_OP                 0x3
#define SPI_WRITE_OP                0x2
#define SPI_READ_OP_LEN             0x8
#define SPI_WRITE_OP_LEN            0x8
#define SPI_REG_LEN                 16
#define SPI_DATA_LEN                16

#define GPIO_DIR_IN                 1
#define GPIO_DIR_OUT                0

#define ack_timer                   5

#define DELAY                        10000
#define CLK_DURATION(clk)            { int i; for(i=0; i<clk; i++); }

rtksw_api_ret_t reg_smi_read(rtksw_uint32 unit, rtksw_uint32 mAddrs, rtksw_uint32 *rData);
rtksw_api_ret_t reg_smi_write(rtksw_uint32 unit, rtksw_uint32 mAddrs, rtksw_uint32 rData);
rtksw_api_ret_t reg_i2c_read(rtksw_uint32 mAddrs, rtksw_uint32 *rData);
rtksw_api_ret_t reg_i2c_write(rtksw_uint32 mAddrs, rtksw_uint32 rData);
rtksw_api_ret_t reg_mdcmdio_read(rtksw_uint32 mAddrs, rtksw_uint32 *rData);
rtksw_api_ret_t reg_mdcmdio_write(rtksw_uint32 mAddrs, rtksw_uint32 rData);
void rtk_set_mdc_mdio(struct mii_bus *bus, int id);
rtksw_api_ret_t reg_spi_read(rtksw_uint32 mAddrs, rtksw_uint32 *rData);
rtksw_api_ret_t reg_spi_write(rtksw_uint32 mAddrs, rtksw_uint32 rData);

#ifdef EASYSMART_SDK
rtksw_api_ret_t reg_soc_read(rtksw_uint32 mAddrs, rtksw_uint32 *rData);
rtksw_api_ret_t reg_soc_write(rtksw_uint32 mAddrs, rtksw_uint32 rData);
#endif

#endif /* __SMI_H__ */


