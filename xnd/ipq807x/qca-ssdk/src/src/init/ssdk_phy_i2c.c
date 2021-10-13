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
/*qca808x_start*/
#include "sw.h"
#include "ssdk_phy_i2c.h"
#include <linux/i2c.h>
#include "ssdk_init.h"
#include "ssdk_plat.h"

#define I2C_RW_LIMIT           8
#define I2C_ADAPTER_DEFAULT_ID 0

/******************************************************************************
*
* _qca_i2c_read - read data per i2c bus
*
* read data per i2c bus
*/
static inline a_int16_t
_qca_i2c_read(a_uint32_t i2c_bus_id, a_uint32_t i2c_slave,
		a_uint32_t data_addr, a_uint8_t *buf, a_uint32_t count)
{
	a_int16_t ret, i;
	struct i2c_adapter *adapt;
	a_uint8_t addrbuf[2];
	struct i2c_msg msg[2];

	ret =0;
	i = 0;

	if (data_addr & 0xff00) {
		addrbuf[i++] = (data_addr >> 8) & 0xff;
	}

	addrbuf[i++] = data_addr & 0xff;

	if (count > I2C_RW_LIMIT) {
		count = I2C_RW_LIMIT;
	}

	/*
	 * msg for configuring the address
	 */
	aos_mem_set(msg, 0, sizeof(msg));
	msg[0].addr = i2c_slave;
	msg[0].len = i;
	msg[0].buf = addrbuf;

	/*
	 * msg for acquiring data
	 */
	msg[1].addr = i2c_slave;
	msg[1].flags = I2C_M_RD;
	msg[1].len = count;
	msg[1].buf = buf;

	adapt = i2c_get_adapter(i2c_bus_id);
	if (adapt) {
		ret = i2c_transfer(adapt, msg, ARRAY_SIZE(msg));
		i2c_put_adapter(adapt);
	}

	if (ret == ARRAY_SIZE(msg)) {
		return count;
	}

	return ret;
}

/******************************************************************************
*
* qca_i2c_data_get - wrapper of reading data per i2c bus
*
* wrapper of reading data per i2c bus
*/
sw_error_t
qca_i2c_data_get(a_uint32_t dev_id, a_uint32_t i2c_slave,
		a_uint32_t data_addr, a_uint8_t *buf, a_uint32_t count)
{
	a_int16_t ret = 0, cur = 0;
	a_uint16_t cnt = count;

	while (cnt) {
		cur = _qca_i2c_read(I2C_ADAPTER_DEFAULT_ID,
				i2c_slave, data_addr, buf, cnt);

		/* No such i2c_slave device */
		if (cur == -ENXIO) {
			return SW_NO_RESOURCE;
		}

		if (cur <= 0) {
			break;
		}
		/*
		 * loop to acquire the data from the new
		 * address based on the returned count.
		 */
		cnt -= cur;
		buf += cur;
		data_addr += cur;
		ret += cur;
	}

	if (ret != count) {
		return SW_FAIL;
	}

	return SW_OK;
}

/******************************************************************************
*
* _qca_i2c_write - write data per i2c bus
*
* write data per i2c bus
*/
static inline a_int16_t
_qca_i2c_write(a_uint32_t i2c_bus_id, a_uint32_t i2c_slave,
		a_uint32_t data_addr, a_uint8_t *buf, a_uint32_t count)
{
	a_int16_t ret, i;
	struct i2c_adapter *adapt;
	struct i2c_msg msg;
	a_uint8_t i2c_wbuf[I2C_RW_LIMIT+2];

	ret = 0;
	i = 0;

	if (data_addr & 0xff00) {
		i2c_wbuf[i++] = (data_addr >> 8) & 0xff;
	}

	/*
	 * the write buffer is for saving address
	 * and the data to sent.
	 */
	i2c_wbuf[i++] = data_addr & 0xff;

	if (count > I2C_RW_LIMIT) {
		count = I2C_RW_LIMIT;
	}
	memcpy(&i2c_wbuf[i], buf, count);

	aos_mem_set(&msg, 0, sizeof(msg));
	msg.addr = i2c_slave;
	msg.len = i + count;
	msg.buf = i2c_wbuf;

	adapt = i2c_get_adapter(i2c_bus_id);
	if (adapt) {
		ret = i2c_transfer(adapt, &msg, 1);
		i2c_put_adapter(adapt);
	}

	if (ret == 1) {
		return count;
	}

	return ret;
}

/******************************************************************************
*
* qca_i2c_data_set - wrapper of writting data per i2c bus
*
* wrapper of writting data per i2c bus
*/
sw_error_t
qca_i2c_data_set(a_uint32_t dev_id, a_uint32_t i2c_slave,
		a_uint32_t data_addr, a_uint8_t *buf, a_uint32_t count)
{
	a_int16_t ret = 0, cur = 0;
	a_uint16_t cnt = count;

	while (cnt) {
		cur = _qca_i2c_write(I2C_ADAPTER_DEFAULT_ID,
				i2c_slave, data_addr, buf, cnt);

		/* No such i2c_slave device */
		if (cur == -ENXIO) {
			return SW_NO_RESOURCE;
		}

		if (cur <= 0) {
			break;
		}
		/*
		 * loop to write the data to the new
		 * address based on the returned count.
		 */
		cnt -= cur;
		buf += cur;
		data_addr += cur;
		ret += cur;
	}

	if (ret != count) {
		return SW_FAIL;
	}

	return SW_OK;
}

/******************************************************************************
*
* _qca_phy_i2c_mii_read - mii register i2c read
*
* mii register i2c read
*/
sw_error_t
qca_phy_i2c_mii_read(a_uint32_t dev_id, a_uint32_t phy_addr,
                          a_uint32_t reg_addr, a_uint16_t *reg_data)
{
	sw_error_t ret;
	a_uint8_t rx[2] = { 0 };

	reg_addr &= 0xff;

	ret = qca_i2c_data_get(dev_id, phy_addr,
			reg_addr, rx, sizeof(rx));

	if (ret == SW_OK) {
		*reg_data = (rx[0] << 8) | rx[1];
	} else {
		*reg_data = 0xffff;
	}

	return ret;
}

/******************************************************************************
*
* _qca_phy_i2c_mii_write - mii register i2c write
*
* mii register i2c write
*/
sw_error_t
qca_phy_i2c_mii_write(a_uint32_t dev_id, a_uint32_t phy_addr,
                           a_uint32_t reg_addr, a_uint16_t reg_data)
{
	sw_error_t ret;
	a_uint8_t tx[2] = {(reg_data >> 8) & 0xff, reg_data & 0xff};

	reg_addr &= 0xff;

	ret = qca_i2c_data_set(dev_id, phy_addr,
			reg_addr, tx, sizeof(tx));

	return ret;
}

#ifdef IN_PHY_I2C_MODE
/******************************************************************************
*
* _qca_phy_i2c_mmd_read - mmd register i2c read
*
* mmd register i2c read
*/
sw_error_t
qca_phy_i2c_mmd_read(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint16_t mmd_num,
                            a_uint32_t reg_addr, a_uint16_t *reg_data)
{
	a_uint8_t ret;
	struct i2c_adapter *adapt;
	/*
		TX buffer to send:
		1st byte: Bit[6](1:MMD 0:MII) Bit[5](1:MMD address 0:MMD data) Bit[4:0](MMD num)
		2nd byte is high 8bits of reg addr
		3rd byte is low 8bits of reg addr
		4th byte: add one extra byte at the end of address writing msg to avoid qca808x
                          treat it as data writing operation
	*/
	a_uint8_t tx[4] = { ((QCA_PHY_I2C_IS_MMD << QCA_PHY_I2C_MMD_OR_MII_SHIFT) |
		(QCA_PHY_I2C_MMD_IS_ADDR << QCA_PHY_I2C_MMD_ADDR_OR_DATA_SHIFT) | mmd_num),
		(reg_addr >> 8) & 0xff, reg_addr & 0xff, 0 };
	/*
		RX buffer to receive:
		1st byte is high 8bits of reg data
		2nd byte is low 8bits of reg data
	*/
	a_uint8_t rx[2] = { 0 };
	struct i2c_msg msg[] = {
		{ .addr = phy_addr, .flags = 0,
		  .buf = tx, .len = sizeof(tx) },
		{ .addr = phy_addr, .flags = I2C_M_RD,
		  .buf = rx, .len = sizeof(rx) } };

	if((mmd_num != QCA_PHY_MMD1_NUM) && (mmd_num != QCA_PHY_MMD3_NUM) &&
		(mmd_num != QCA_PHY_MMD7_NUM)) {
		SSDK_ERROR("wrong MMD number:[%d]\n", mmd_num);
		return SW_FAIL;
	}

	adapt = i2c_get_adapter(I2C_ADAPTER_DEFAULT_ID);
	if (!adapt) {
		return SW_FAIL;
	}

	ret = i2c_transfer(adapt, msg, ARRAY_SIZE(msg));
	*reg_data = (rx[0] << 8) | rx[1];

	i2c_put_adapter(adapt);

	if (ret != ARRAY_SIZE(msg)) {
		return SW_FAIL;
	}

	return SW_OK;
}

/******************************************************************************
*
* _qca_phy_i2c_mmd_write - mmd register i2c write
*
* mmd register i2c write
*/
sw_error_t
qca_phy_i2c_mmd_write(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint16_t mmd_num,
                            a_uint32_t reg_addr, a_uint16_t reg_data)
{
	a_uint8_t ret;
	struct i2c_adapter *adapt;
	/*
		First TX buffer to send:
		1st byte: Bit[6](1:MMD 0:MII) Bit[5](1:MMD address 0:MMD data) Bit[4:0](MMD num)
		2nd byte is high 8bits of reg addr
		3rd byte is low 8bits of reg addr
		4th byte: add one extra byte at the end of address writing msg to avoid qca808x
                          treat it as data writing operation
	*/
	a_uint8_t tx[4] = { ((QCA_PHY_I2C_IS_MMD << QCA_PHY_I2C_MMD_OR_MII_SHIFT) |
		(QCA_PHY_I2C_MMD_IS_ADDR << QCA_PHY_I2C_MMD_ADDR_OR_DATA_SHIFT) | mmd_num),
		(reg_addr >> 8) & 0xff, reg_addr & 0xff, 0 };

	/*
		Second TX buffer to send:
		1st byte: Bit[6](1:MMD 0:MII) Bit[5](1:MMD address 0:MMD data) Bit[4:0](MMD num)
		2nd byte is high 8bits of reg data
		3rd byte is low 8bits of reg data
	*/
	a_uint8_t tx2[3] = { ((QCA_PHY_I2C_IS_MMD << QCA_PHY_I2C_MMD_OR_MII_SHIFT) |
		(QCA_PHY_I2C_MMD_IS_DATA << QCA_PHY_I2C_MMD_ADDR_OR_DATA_SHIFT) | mmd_num),
		(reg_data >> 8) & 0xff, reg_data & 0xff };

	struct i2c_msg msg[] = {
		{ .addr = phy_addr, .flags = 0,
		  .buf = tx, .len = sizeof(tx) },
		{ .addr = phy_addr, .flags = 0,
		  .buf = tx2, .len = sizeof(tx2) } };

	if((mmd_num != QCA_PHY_MMD1_NUM) && (mmd_num != QCA_PHY_MMD3_NUM) &&
		(mmd_num != QCA_PHY_MMD7_NUM)) {
		SSDK_ERROR("wrong MMD number:[%d]\n", mmd_num);
		return SW_FAIL;
	}

	adapt = i2c_get_adapter(I2C_ADAPTER_DEFAULT_ID);
	if (!adapt) {
		return SW_FAIL;
	}

	ret = i2c_transfer(adapt, msg, ARRAY_SIZE(msg));

	i2c_put_adapter(adapt);

	if (ret != ARRAY_SIZE(msg)) {
		return SW_FAIL;
	}

	return SW_OK;
}

/******************************************************************************
*
* qca_phy_is_i2c_addr -  check if phy addr is i2c addr
*
* check if phy addr is i2c addr
*/
a_bool_t
qca_phy_is_i2c_addr(a_uint32_t phy_addr)
{
	if(((phy_addr & QCA_PHY_I2C_DEVADDR_MASK) ==
					QCA_PHY_I2C_PHYCORE_DEVADDR) ||
		((phy_addr & QCA_PHY_I2C_DEVADDR_MASK) ==
					QCA_PHY_I2C_SERDES_DEVADDR)) {
		return A_TRUE;
	} else {
		return A_FALSE;
	}
}

/******************************************************************************
*
* qca_phy_i2c_read -  PHY register i2c read
*
* PHY register i2c read
*/
sw_error_t
qca_phy_i2c_read(a_uint32_t dev_id, a_uint32_t phy_addr,
                           a_uint32_t reg_addr_c45, a_uint16_t *reg_data)
{
	a_uint32_t mmd_num = QCA_PHY_MII_ADDR_C45_MMD_NUM(reg_addr_c45);
	a_uint32_t reg_addr = QCA_PHY_MII_ADDR_C45_REG_ADDR(reg_addr_c45);

	if(qca_phy_is_i2c_addr(phy_addr) == A_FALSE) {
		return SW_BAD_PARAM;
	}

	if(QCA_PHY_MII_ADDR_C45_IS_MMD(reg_addr_c45)) {
		qca_phy_i2c_mmd_read(dev_id, phy_addr, mmd_num, reg_addr, reg_data);

	} else {
		qca_phy_i2c_mii_read(dev_id, phy_addr, reg_addr, reg_data);
	}

	return SW_OK;
}

/******************************************************************************
*
* qca_phy_i2c_write -  PHY register i2c write
*
* PHY register i2c write
*/
sw_error_t
qca_phy_i2c_write(a_uint32_t dev_id, a_uint32_t phy_addr,
                            a_uint32_t reg_addr_c45, a_uint16_t reg_data)
{
	a_uint32_t mmd_num = QCA_PHY_MII_ADDR_C45_MMD_NUM(reg_addr_c45);
	a_uint32_t reg_addr = QCA_PHY_MII_ADDR_C45_REG_ADDR(reg_addr_c45);

	if(qca_phy_is_i2c_addr(phy_addr) == A_FALSE) {
		return SW_BAD_PARAM;
	}

	if(QCA_PHY_MII_ADDR_C45_IS_MMD(reg_addr_c45)) {
		qca_phy_i2c_mmd_write(dev_id, phy_addr, mmd_num, reg_addr, reg_data);

	} else {
		qca_phy_i2c_mii_write(dev_id, phy_addr, reg_addr, reg_data);
	}

	return SW_OK;
}
/*qca808x_end*/
EXPORT_SYMBOL(qca_phy_i2c_read);
EXPORT_SYMBOL(qca_phy_i2c_write);
/*qca808x_start*/
#endif
/*qca808x_start*/
