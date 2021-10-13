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
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "sd.h"
#include "scomphy_reg_access.h"
#include "hsl_phy.h"

#if defined(API_LOCK)
static aos_lock_t mdio_lock;
#define MDIO_LOCKER_INIT    aos_lock_init(&mdio_lock)
#define MDIO_LOCKER_LOCK    aos_lock(&mdio_lock)
#define MDIO_LOCKER_UNLOCK  aos_unlock(&mdio_lock)
#else
#define MDIO_LOCKER_INIT
#define MDIO_LOCKER_LOCK
#define MDIO_LOCKER_UNLOCK
#endif

sw_error_t
scomphy_phy_get(a_uint32_t dev_id, a_uint32_t phy_addr,
             a_uint32_t reg, a_uint16_t * value)
{
	sw_error_t rv;

	MDIO_LOCKER_LOCK;
	rv = sd_reg_mdio_get(dev_id, phy_addr, reg, value);
	MDIO_LOCKER_UNLOCK;
	return rv;
}

sw_error_t
scomphy_phy_set(a_uint32_t dev_id, a_uint32_t phy_addr,
             a_uint32_t reg, a_uint16_t value)
{
	sw_error_t rv;

	MDIO_LOCKER_LOCK;
	rv = sd_reg_mdio_set(dev_id, phy_addr, reg, value);
	MDIO_LOCKER_UNLOCK;
	return rv;
}

sw_error_t
scomphy_phy_i2c_get(a_uint32_t dev_id, a_uint32_t phy_addr,
             a_uint32_t reg, a_uint16_t * value)
{
	sw_error_t rv;

	rv = sd_reg_i2c_get(dev_id, phy_addr, reg, value);

	return rv;
}

sw_error_t
scomphy_phyi2c_set(a_uint32_t dev_id, a_uint32_t phy_addr,
             a_uint32_t reg, a_uint16_t value)
{
	sw_error_t rv;

	rv = sd_reg_i2c_set(dev_id, phy_addr, reg, value);

	return rv;
}
/*qca808x_end*/
sw_error_t
scomphy_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr,
	a_uint8_t *val, a_uint32_t len)
{
	sw_error_t rv;

	rv = sd_reg_hdr_get(dev_id, reg_addr, val, len);

	return rv;
}

sw_error_t
scomphy_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr,
	a_uint8_t *val, a_uint32_t len)
{
	sw_error_t rv;

	rv = sd_reg_hdr_set(dev_id, reg_addr, val, len);

	return rv;
}

sw_error_t
scomphy_uniphy_reg_get(a_uint32_t dev_id, a_uint32_t index,
	a_uint32_t reg_addr, a_uint8_t *val, a_uint32_t len)
{
	sw_error_t rv;

	rv = sd_reg_uniphy_get(dev_id, index, reg_addr, val, len);

	return rv;
}

sw_error_t
scomphy_uniphy_reg_set(a_uint32_t dev_id, a_uint32_t index,
	a_uint32_t reg_addr, a_uint8_t *val, a_uint32_t len)
{
	sw_error_t rv;

	rv = sd_reg_uniphy_set(dev_id, index, reg_addr, val, len);

	return rv;
}
/*qca808x_start*/
sw_error_t
scomphy_reg_access_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
	hsl_api_t *p_api;

	MDIO_LOCKER_INIT;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
	p_api->phy_get = scomphy_phy_get;
	p_api->phy_set = scomphy_phy_set;
	p_api->phy_i2c_get = scomphy_phy_i2c_get;
	p_api->phy_i2c_set = scomphy_phyi2c_set;
/*qca808x_end*/
	if(cfg->phy_id == MP_GEPHY)
	{
		p_api->reg_get = scomphy_reg_get;
		p_api->reg_set = scomphy_reg_set;
		p_api->uniphy_reg_get = scomphy_uniphy_reg_get;
		p_api->uniphy_reg_set = scomphy_uniphy_reg_set;
	}
/*qca808x_start*/
	return SW_OK;
}
/*qca808x_end*/
