/*
 * Copyright (c) 2012, 2017-2018, The Linux Foundation. All rights reserved.
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
/**
 * @defgroup fal_reg_access FAL_REG_ACCESS
 * @{
 */
#include "sw.h"
#include "fal_reg_access.h"
#include "hsl_api.h"
#include "hsl_phy.h"

static sw_error_t
_fal_phy_get(a_uint32_t dev_id, a_uint32_t phy_addr,
             a_uint32_t reg, a_uint16_t * value)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    a_uint8_t phy_addr_type;
    hsl_phy_get phy_get_func;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    /* the MSB first byte of phy_addr marks the type of
     * phy address, such as the i2c address, the value of
     * MSB first byte should be 1 */
    phy_addr_type = (phy_addr & 0xff000000) >> 24;
    phy_addr = phy_addr & 0xff;
    switch (phy_addr_type) {
	    case PHY_I2C_ACCESS:
		    phy_get_func = p_api->phy_i2c_get;
		    break;
	    default:
		    phy_get_func = p_api->phy_get;
		    break;
    }

    if (NULL == phy_get_func) {
	    return SW_NOT_SUPPORTED;
    }

    rv = phy_get_func(dev_id, phy_addr, reg, value);
    return rv;
}

static sw_error_t
_fal_phy_set(a_uint32_t dev_id, a_uint32_t phy_addr,
             a_uint32_t reg, a_uint16_t value)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    a_uint8_t phy_addr_type;
    hsl_phy_set phy_set_func;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    /* the MSB first byte of phy_addr marks the type of
     * phy address, such as the i2c address, the value of
     * MSB first byte should be 1 */
    phy_addr_type = (phy_addr & 0xff000000) >> 24;
    phy_addr = phy_addr & 0xff;
    switch (phy_addr_type) {
	    case PHY_I2C_ACCESS:
		    phy_set_func = p_api->phy_i2c_set;
		    break;
	    default:
		    phy_set_func = p_api->phy_set;
		    break;
    }

    if (NULL == phy_set_func) {
	    return SW_NOT_SUPPORTED;
    }

    rv = phy_set_func(dev_id, phy_addr, reg, value);
    return rv;
}
/*qca808x_end*/
static sw_error_t
_fal_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
             a_uint32_t value_len)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->reg_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->reg_get(dev_id, reg_addr, value, value_len);
    return rv;
}

static sw_error_t
_fal_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
             a_uint32_t value_len)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->reg_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->reg_set(dev_id, reg_addr, value, value_len);
    return rv;
}


static sw_error_t
_fal_psgmii_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
             a_uint32_t value_len)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->psgmii_reg_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->psgmii_reg_get(dev_id, reg_addr, value, value_len);
    return rv;
}

static sw_error_t
_fal_psgmii_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
             a_uint32_t value_len)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->psgmii_reg_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->psgmii_reg_set(dev_id, reg_addr, value, value_len);
    return rv;
}

static sw_error_t
_fal_reg_field_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                   a_uint32_t bit_offset, a_uint32_t field_len,
                   a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->reg_field_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->reg_field_get(dev_id, reg_addr, bit_offset, field_len, value, value_len);
    return rv;
}

static sw_error_t
_fal_reg_field_set(a_uint32_t dev_id, a_uint32_t reg_addr,
                   a_uint32_t bit_offset, a_uint32_t field_len,
                   const a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->reg_field_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->reg_field_set(dev_id, reg_addr, bit_offset, field_len, value, value_len);
    return rv;
}

static sw_error_t
_fal_reg_dump(a_uint32_t dev_id, a_uint32_t reg_idx,fal_reg_dump_t *reg_dump)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->register_dump)
        return SW_NOT_SUPPORTED;

    rv = p_api->register_dump(dev_id, reg_idx,reg_dump);
    return rv;
}


static sw_error_t
_fal_debug_reg_dump(a_uint32_t dev_id, fal_debug_reg_dump_t *reg_dump)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->debug_register_dump)
        return SW_NOT_SUPPORTED;

    rv = p_api->debug_register_dump(dev_id, reg_dump);
    return rv;
}

static sw_error_t
_fal_debug_psgmii_self_test(a_uint32_t dev_id, a_bool_t enable,
            a_uint32_t times, a_uint32_t *result)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->debug_psgmii_self_test)
        return SW_NOT_SUPPORTED;

    rv = p_api->debug_psgmii_self_test(dev_id, enable, times, result);
    return rv;
}

static sw_error_t
_fal_phy_dump(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint32_t idx,fal_phy_dump_t *phy_dump)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->phy_dump)
        return SW_NOT_SUPPORTED;

    rv = p_api->phy_dump(dev_id, phy_addr,idx,phy_dump);
    return rv;
}
static sw_error_t
_fal_uniphy_reg_get(a_uint32_t dev_id, a_uint32_t index, a_uint32_t reg_addr,
		a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->uniphy_reg_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->uniphy_reg_get(dev_id, index, reg_addr, value, value_len);
    return rv;
}

static sw_error_t
_fal_uniphy_reg_set(a_uint32_t dev_id, a_uint32_t index, a_uint32_t reg_addr,
		a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->uniphy_reg_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->uniphy_reg_set(dev_id, index, reg_addr, value, value_len);
    return rv;
}
/*qca808x_start*/
/**
  * fal_phy_get - get value of specific phy device
  * @phy_addr: id of the phy device
  * @reg: register id of phy device
  * @value: pointer to the memory storing the value.
  * @return SW_OK or error code
  */
sw_error_t
fal_phy_get(a_uint32_t dev_id, a_uint32_t phy_addr,
            a_uint32_t reg, a_uint16_t * value)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_phy_get(dev_id, phy_addr, reg, value);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * fal_phy_set - set value of specific phy device
  * @phy_addr: id of the phy device
  * @reg: register id of phy device
  * @value: register value.
  * @return SW_OK or error code
  */
sw_error_t
fal_phy_set(a_uint32_t dev_id, a_uint32_t phy_addr,
            a_uint32_t reg, a_uint16_t value)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_phy_set(dev_id, phy_addr, reg, value);
    FAL_API_UNLOCK;
    return rv;
}
/*qca808x_end*/
/**
  * fal_reg_get - get value of specific register
  * @reg_addr: address of the register
  * @value: pointer to the memory storing the value.
  * @value_len: length of the value.
  *
  * Get the value of a specific register field with related parameter
  */
sw_error_t
fal_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
            a_uint32_t value_len)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_reg_get(dev_id, reg_addr, value, value_len);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * fal_reg_set - set value of specific register
  * @reg_addr: address of the register
  * @value: pointer to the memory storing the value.
  * @value_len: length of the value.
  *
  * Get the value of a specific register field with related parameter
  */
sw_error_t
fal_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
            a_uint32_t value_len)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_reg_set(dev_id, reg_addr, value, value_len);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * fal_psgmii_reg_get - get value of specific register in psgmii module
  * @reg_addr: address of the register
  * @value: pointer to the memory storing the value.
  * @value_len: length of the value.
  *
  * Get the value of a specific register field with related parameter
  */
sw_error_t
fal_psgmii_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
            a_uint32_t value_len)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_psgmii_reg_get(dev_id, reg_addr, value, value_len);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * fal_psgmii_reg_set - set value of specific register in psgmii module
  * @reg_addr: address of the register
  * @value: pointer to the memory storing the value.
  * @value_len: length of the value.
  *
  * Get the value of a specific register field with related parameter
  */
sw_error_t
fal_psgmii_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
            a_uint32_t value_len)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_psgmii_reg_set(dev_id, reg_addr, value, value_len);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * fal_reg_field_get - get value of specific register field
  * @reg_addr: address of the register
  * @bit_offset: position of the field in bit
  * @field_len: length of the field in bit
  * @value: pointer to the memory storing the value.
  * @value_len: length of the value.
  *
  * Get the value of a specific register field with related parameter
  */
sw_error_t
fal_reg_field_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                  a_uint32_t bit_offset, a_uint32_t field_len,
                  a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_reg_field_get(dev_id, reg_addr, bit_offset, field_len, value, value_len);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * fal_reg_field_set - set value of specific register field
  * @reg_addr: address of the register
  * @bit_offset: position of the field in bit
  * @field_len: length of the field in bit
  * @value: pointer to the memory storing the value.
  * @value_len: length of the value.
  *
  * Set the value of a specific register field with related parameter
  */
sw_error_t
fal_reg_field_set(a_uint32_t dev_id, a_uint32_t reg_addr,
                  a_uint32_t bit_offset, a_uint32_t field_len,
                  const a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_reg_field_set(dev_id, reg_addr, bit_offset, field_len, value, value_len);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief dump device register group
 * @details   Comments:
 *    The unit of packets size is byte.
 * @param[in] dev_id device id
 * @param[out] reg_dump dump out register group
 * @return SW_OK or error code
 */
sw_error_t
fal_reg_dump(a_uint32_t dev_id, a_uint32_t reg_idx,fal_reg_dump_t *reg_dump)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_reg_dump(dev_id, reg_idx,reg_dump);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief dump device debug register
 * @details   Comments:
 *    The unit of packets size is byte.
 * @param[in] dev_id device id
 * @param[out] reg_dump dump out debub register
 * @return SW_OK or error code
 */
sw_error_t
fal_debug_reg_dump(a_uint32_t dev_id, fal_debug_reg_dump_t *reg_dump)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_debug_reg_dump(dev_id,reg_dump);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief psgmii self test
 * @details   Comments:
 *    The unit of packets size is byte.
 * @param[in] dev_id device id, enable, times
 * @param[out] status
 * @return SW_OK or error code
 */
sw_error_t
fal_debug_psgmii_self_test(a_uint32_t dev_id, a_bool_t enable,
            a_uint32_t times, a_uint32_t *result)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_debug_psgmii_self_test(dev_id, enable, times, result);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief phy dump
 * @details   Comments:
 *    The unit of packets size is byte.
 * @param[in] dev_id device id, phy addr, phy reg group
 * @param[out] reg value
 * @return SW_OK or error code
 */
sw_error_t
fal_phy_dump(a_uint32_t dev_id, a_uint32_t phy_addr,
		a_uint32_t idx, fal_phy_dump_t * phy_dump)

{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_phy_dump(dev_id, phy_addr, idx, phy_dump);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * fal_uniphy_reg_get - get value of specific register in uniphy module
  * @reg_addr: address of the register
  * @uniphy_index: index of uniphy
  * @value: pointer to the memory storing the value.
  * @value_len: length of the value.
  *
  * Get the value of a specific register field with related parameter
  */
sw_error_t
fal_uniphy_reg_get(a_uint32_t dev_id, a_uint32_t index, a_uint32_t reg_addr,
		a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_uniphy_reg_get(dev_id, index, reg_addr, value, value_len);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * fal_uniphy_reg_set - set value of specific register in uniphy module
  * @reg_addr: address of the register
  * @uniphy_index: index of uniphy
  * @value: pointer to the memory storing the value.
  * @value_len: length of the value.
  *
  * Get the value of a specific register field with related parameter
  */
sw_error_t
fal_uniphy_reg_set(a_uint32_t dev_id, a_uint32_t index, a_uint32_t reg_addr,
		a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_uniphy_reg_set(dev_id, index, reg_addr, value, value_len);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @}
 */
