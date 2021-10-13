/*
 * Copyright (c) 2012, 2016, The Linux Foundation. All rights reserved.
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


#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "sd.h"
#include "shiva_reg_access.h"

static hsl_access_mode reg_mode;

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

static sw_error_t
_shiva_mdio_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                    a_uint8_t value[], a_uint32_t value_len)
{
    a_uint32_t reg_val;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

#if 0
    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (a_uint16_t) ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */

    rv = sd_reg_mdio_set(dev_id, phy_addr, phy_reg, phy_val);
    SW_RTN_ON_ERROR(rv);

    /* For some registers such as MIBs, since it is read/clear, we should */
    /* read the lower 16-bit register then the higher one */

    /* read register in lower address */
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    rv = sd_reg_mdio_get(dev_id, phy_addr, phy_reg, &tmp_val);
    SW_RTN_ON_ERROR(rv);
    reg_val = tmp_val;

    /* read register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    rv = sd_reg_mdio_get(dev_id, phy_addr, phy_reg, &tmp_val);
    SW_RTN_ON_ERROR(rv);
    reg_val |= (((a_uint32_t)tmp_val) << 16);
#else
    reg_val = sd_reg_mii_get(dev_id, reg_addr);
#endif
    aos_mem_copy(value, &reg_val, sizeof (a_uint32_t));

    return SW_OK;
}

static sw_error_t
_shiva_mdio_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
                    a_uint32_t value_len)
{
    a_uint32_t reg_val = 0;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    aos_mem_copy(&reg_val, value, sizeof (a_uint32_t));

#if 0
/* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (a_uint16_t) ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */

    rv = sd_reg_mdio_set(dev_id, phy_addr, phy_reg, phy_val);
    SW_RTN_ON_ERROR(rv);

    /* For some registers such as ARL and VLAN, since they include BUSY bit */
    /* in lower address, we should write the higher 16-bit register then the */
    /* lower one */

    /* write register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (a_uint16_t) ((reg_val >> 16) & 0xffff);
    rv = sd_reg_mdio_set(dev_id, phy_addr, phy_reg, phy_val);
    SW_RTN_ON_ERROR(rv);

    /* write register in lower address */
    reg_word_addr--;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (a_uint16_t) (reg_val & 0xffff);
    rv = sd_reg_mdio_set(dev_id, phy_addr, phy_reg, phy_val);
    SW_RTN_ON_ERROR(rv);
#else
    sd_reg_mii_set(dev_id, reg_addr, reg_val);
#endif

    return SW_OK;
}

sw_error_t
shiva_phy_get(a_uint32_t dev_id, a_uint32_t phy_addr,
              a_uint32_t reg, a_uint16_t * value)
{
    sw_error_t rv;

    MDIO_LOCKER_LOCK;
    rv = sd_reg_mdio_get(dev_id, phy_addr, reg, value);
    MDIO_LOCKER_UNLOCK;

    return rv;
}

sw_error_t
shiva_phy_set(a_uint32_t dev_id, a_uint32_t phy_addr,
              a_uint32_t reg, a_uint16_t value)
{
    sw_error_t rv;

    MDIO_LOCKER_LOCK;
    rv = sd_reg_mdio_set(dev_id, phy_addr, reg, value);
    MDIO_LOCKER_UNLOCK;

    return rv;
}

sw_error_t
shiva_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
              a_uint32_t value_len)
{
    sw_error_t rv;
    unsigned long flags;

    MDIO_LOCKER_LOCK;
    if (HSL_MDIO == reg_mode)
    {
        aos_irq_save(flags);
        rv = _shiva_mdio_reg_get(dev_id, reg_addr, value, value_len);
        aos_irq_restore(flags);
    }
    else
    {
        rv = sd_reg_hdr_get(dev_id, reg_addr, value, value_len);
    }
    MDIO_LOCKER_UNLOCK;

    return rv;
}

sw_error_t
shiva_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
              a_uint32_t value_len)
{
    sw_error_t rv;
    unsigned long flags;

    MDIO_LOCKER_LOCK;
    if (HSL_MDIO == reg_mode)
    {
        aos_irq_save(flags);
        rv = _shiva_mdio_reg_set(dev_id, reg_addr, value, value_len);
        aos_irq_restore(flags);
    }
    else
    {
        rv = sd_reg_hdr_set(dev_id, reg_addr, value, value_len);
    }
    MDIO_LOCKER_UNLOCK;

    return rv;
}

sw_error_t
shiva_reg_field_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                    a_uint32_t bit_offset, a_uint32_t field_len,
                    a_uint8_t value[], a_uint32_t value_len)
{
    a_uint32_t reg_val = 0;

    if ((bit_offset >= 32 || (field_len > 32)) || (field_len == 0))
        return SW_OUT_OF_RANGE;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    SW_RTN_ON_ERROR(shiva_reg_get(dev_id, reg_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));

    *((a_uint32_t *) value) = SW_REG_2_FIELD(reg_val, bit_offset, field_len);
    return SW_OK;
}

sw_error_t
shiva_reg_field_set(a_uint32_t dev_id, a_uint32_t reg_addr,
                    a_uint32_t bit_offset, a_uint32_t field_len,
                    const a_uint8_t value[], a_uint32_t value_len)
{
    a_uint32_t reg_val = 0;
    a_uint32_t field_val = *((a_uint32_t *) value);

    if ((bit_offset >= 32 || (field_len > 32)) || (field_len == 0))
        return SW_OUT_OF_RANGE;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    SW_RTN_ON_ERROR(shiva_reg_get(dev_id, reg_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));

    SW_REG_SET_BY_FIELD_U32(reg_val, field_val, bit_offset, field_len);

    SW_RTN_ON_ERROR(shiva_reg_set(dev_id, reg_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));

    return SW_OK;
}

sw_error_t
shiva_reg_access_init(a_uint32_t dev_id, hsl_access_mode mode)
{
    hsl_api_t *p_api;

    MDIO_LOCKER_INIT;
    reg_mode = mode;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
    p_api->phy_get = shiva_phy_get;
    p_api->phy_set = shiva_phy_set;
    p_api->reg_get = shiva_reg_get;
    p_api->reg_set = shiva_reg_set;
    p_api->reg_field_get = shiva_reg_field_get;
    p_api->reg_field_set = shiva_reg_field_set;
    p_api->dev_access_set= shiva_access_mode_set;

    return SW_OK;
}

sw_error_t
shiva_access_mode_set(a_uint32_t dev_id, hsl_access_mode mode)
{
    reg_mode = mode;
    return SW_OK;

}

