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

#ifndef _SSDK_PHY_I2C_H_
#define _SSDK_PHY_I2C_H_

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

#define QCA_PHY_I2C_MMD_OR_MII_SHIFT	6
#define QCA_PHY_I2C_IS_MMD		1
#define QCA_PHY_I2C_IS_MII		0
#define QCA_PHY_I2C_MMD_ADDR_OR_DATA_SHIFT		5
#define QCA_PHY_I2C_MMD_IS_ADDR		1
#define QCA_PHY_I2C_MMD_IS_DATA		0

#define QCA_PHY_MII_ADDR_C45  			(1<<30)
#define QCA_PHY_MII_ADDR_C45_IS_MMD(reg_addr_c45) ((reg_addr_c45) & QCA_PHY_MII_ADDR_C45)
#define QCA_PHY_MII_ADDR_C45_MMD_NUM(reg_addr_c45) (((reg_addr_c45) >> 16) & 0x1f)
#define QCA_PHY_MII_ADDR_C45_REG_ADDR(reg_addr_c45) ((reg_addr_c45) & 0xffff)

#define QCA_PHY_I2C_PHYCORE_DEVADDR		0x44
#define QCA_PHY_I2C_SERDES_DEVADDR		0x45
#define QCA_PHY_I2C_DEVADDR_MASK		0x47

#define QCA_PHY_MMD1_NUM  1
#define QCA_PHY_MMD3_NUM  3
#define QCA_PHY_MMD7_NUM  7

sw_error_t
qca_phy_i2c_mii_read(a_uint32_t dev_id, a_uint32_t phy_addr,
                          a_uint32_t reg_addr, a_uint16_t *reg_data);
sw_error_t
qca_phy_i2c_mii_write(a_uint32_t dev_id, a_uint32_t phy_addr,
                           a_uint32_t reg_addr, a_uint16_t reg_data);
sw_error_t
qca_i2c_data_get(a_uint32_t dev_id, a_uint32_t i2c_slave,
		a_uint32_t data_addr, a_uint8_t *buf, a_uint32_t count);

sw_error_t
qca_i2c_data_set(a_uint32_t dev_id, a_uint32_t i2c_slave,
		a_uint32_t data_addr, a_uint8_t *buf, a_uint32_t count);

#ifdef IN_PHY_I2C_MODE
sw_error_t
qca_phy_i2c_mmd_read(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint16_t mmd_num,
                            a_uint32_t reg_addr, a_uint16_t *reg_data);
sw_error_t
qca_phy_i2c_mmd_write(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint16_t mmd_num,
                            a_uint32_t reg_addr, a_uint16_t reg_data);
a_bool_t
qca_phy_is_i2c_addr(a_uint32_t phy_addr);
sw_error_t
qca_phy_i2c_read(a_uint32_t dev_id, a_uint32_t phy_addr,
                           a_uint32_t reg_addr_c45, a_uint16_t *reg_data);
sw_error_t
qca_phy_i2c_write(a_uint32_t dev_id, a_uint32_t phy_addr,
                            a_uint32_t reg_addr_c45, a_uint16_t reg_data);
#endif
#ifdef __cplusplus
}
#endif				/* __cplusplus */
#endif				/* _SSDK_PHY_I2C_H_ */
