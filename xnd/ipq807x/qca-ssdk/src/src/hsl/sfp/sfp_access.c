/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
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
 * @defgroup
 * @{
 */

#include "sw.h"
#include "ssdk_init.h"
#include "ssdk_phy_i2c.h"

sw_error_t
sfp_data_tbl_get(a_uint32_t dev_id, a_uint32_t index, a_uint32_t i2c_slave,
		a_uint32_t data_addr, a_uint8_t *buf, a_uint32_t count)
{
	sw_error_t rv;
	rv = qca_i2c_data_get(dev_id, i2c_slave, data_addr, buf, count);
	return rv;
}

sw_error_t
sfp_data_get(a_uint32_t dev_id, a_uint32_t index, a_uint32_t i2c_slave,
		a_uint32_t data_addr, a_uint8_t *buf)
{
	sw_error_t rv;
	rv = qca_i2c_data_get(dev_id, i2c_slave, data_addr, buf, 1);
	return rv;
}

sw_error_t
sfp_data_tbl_set(a_uint32_t dev_id, a_uint32_t index, a_uint32_t i2c_slave,
		a_uint32_t data_addr, a_uint8_t *buf, a_uint32_t count)
{
	sw_error_t rv;
	rv = qca_i2c_data_set(dev_id, i2c_slave, data_addr, buf, count);
	return rv;
}

sw_error_t
sfp_data_set(a_uint32_t dev_id, a_uint32_t index, a_uint32_t i2c_slave,
		a_uint32_t data_addr, a_uint8_t *buf)
{
	sw_error_t rv;
	rv = qca_i2c_data_set(dev_id, i2c_slave, data_addr, buf, 1);
	return rv;
}
