/*
 * Copyright (c) 2014, 2016-2017, The Linux Foundation. All rights reserved.
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
#include "dess_reg_access.h"
#include "dess_psgmii.h"

#if 0
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mm.h>
#endif

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

#if defined(REG_ACCESS_SPEEDUP)
static a_uint32_t mdio_base_addr = 0xffffffff;
#endif

extern void ssdk_psgmii_self_test(a_uint32_t dev_id, a_bool_t enable, a_uint32_t times, a_uint32_t *result);
extern void clear_self_test_config(a_uint32_t dev_id);

static sw_error_t
_dess_mdio_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                   a_uint8_t value[], a_uint32_t value_len)
{
#if 0
	 a_uint32_t reg_word_addr;
	 a_uint32_t phy_addr, reg_val;
	 a_uint16_t phy_val, tmp_val;
	 a_uint8_t phy_reg;
	 sw_error_t rv;
#else
	a_uint32_t reg_val;
#endif

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

#if 0
    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (a_uint16_t) ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */

#if defined(REG_ACCESS_SPEEDUP)
    if (phy_val != mdio_base_addr)
    {
        rv = sd_reg_mdio_set(dev_id, phy_addr, phy_reg, phy_val);
        SW_RTN_ON_ERROR(rv);

        mdio_base_addr = phy_val;
    }
#else
    rv = sd_reg_mdio_set(dev_id, phy_addr, phy_reg, phy_val);
    SW_RTN_ON_ERROR(rv);
#endif

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
_dess_mdio_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
                   a_uint32_t value_len)
{
#if 0
	 a_uint32_t reg_word_addr;
	 a_uint32_t phy_addr, reg_val;
	 a_uint16_t phy_val;
	 a_uint8_t phy_reg;
	 sw_error_t rv;
#else
	a_uint32_t reg_val = 0;
#endif


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

#if defined(REG_ACCESS_SPEEDUP)
    if (phy_val != mdio_base_addr)
    {
        rv = sd_reg_mdio_set(dev_id, phy_addr, phy_reg, phy_val);
        SW_RTN_ON_ERROR(rv);

        mdio_base_addr = phy_val;
    }
#else
    rv = sd_reg_mdio_set(dev_id, phy_addr, phy_reg, phy_val);
    SW_RTN_ON_ERROR(rv);
#endif

    /* For some registers such as ARL and VLAN, since they include BUSY bit */
    /* in higher address, we should write the lower 16-bit register then the */
    /* higher one */

    /* write register in lower address */
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (a_uint16_t) (reg_val & 0xffff);
    rv = sd_reg_mdio_set(dev_id, phy_addr, phy_reg, phy_val);
    SW_RTN_ON_ERROR(rv);

    /* write register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (a_uint16_t) ((reg_val >> 16) & 0xffff);
    rv = sd_reg_mdio_set(dev_id, phy_addr, phy_reg, phy_val);
    SW_RTN_ON_ERROR(rv);
#else
    sd_reg_mii_set(dev_id, reg_addr, reg_val);
#endif
    return SW_OK;
}

sw_error_t
dess_phy_get(a_uint32_t dev_id, a_uint32_t phy_addr,
             a_uint32_t reg, a_uint16_t * value)
{
    sw_error_t rv;

    MDIO_LOCKER_LOCK;
    rv = sd_reg_mdio_get(dev_id, phy_addr, reg, value);
    MDIO_LOCKER_UNLOCK;
    return rv;
}

sw_error_t
dess_phy_set(a_uint32_t dev_id, a_uint32_t phy_addr,
             a_uint32_t reg, a_uint16_t value)
{
    sw_error_t rv;

    MDIO_LOCKER_LOCK;
    rv = sd_reg_mdio_set(dev_id, phy_addr, reg, value);
    MDIO_LOCKER_UNLOCK;
    return rv;
}

sw_error_t
dess_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
             a_uint32_t value_len)
{
    sw_error_t rv;

    MDIO_LOCKER_LOCK;
    if (HSL_MDIO == reg_mode)
    {
        rv = _dess_mdio_reg_get(dev_id, reg_addr, value, value_len);
    }
    else
    {
        rv = sd_reg_hdr_get(dev_id, reg_addr, value, value_len);
    }
    MDIO_LOCKER_UNLOCK;

    return rv;
}

sw_error_t
dess_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
             a_uint32_t value_len)
{
    sw_error_t rv;
#if 0
    unsigned long flags;

    struct file *filp;
    // mm_segment_t fs;
    a_uint32_t rt_value = 0;
    a_uint32_t write_flag = 0;
    char s[20]= {0};
    a_uint32_t tmp_val = *((a_uint32_t *) value);
#else
	a_uint32_t rt_value = 0;
#endif

    /*get MODULE_EN reg rsv */
    SW_RTN_ON_ERROR(dess_reg_get(dev_id, 0x30,(void *)&rt_value,4));
//    write_flag = (rt_value>>15) & 0x1;

    MDIO_LOCKER_LOCK;
    if (HSL_MDIO == reg_mode)
    {
        rv = _dess_mdio_reg_set(dev_id, reg_addr, value, value_len);
    }
    else
    {
        rv = sd_reg_hdr_set(dev_id, reg_addr, value, value_len);
    }
    MDIO_LOCKER_UNLOCK;

#if 0
    if(write_flag)
    {
        filp = filp_open("/tmp/asic_output", O_RDWR|O_APPEND, 0644);
        if(IS_ERR(filp))
        {
            printk("open error...\n");
            return;
        }

        fs=get_fs();

        set_fs(KERNEL_DS);
        sprintf(s,"%08x %08x\n",reg_addr,tmp_val);
        filp->f_op->write(filp, s, strlen(s),&filp->f_pos);

        set_fs(fs);

        filp_close(filp,NULL);
    }
#endif

    return rv;
}

sw_error_t
dess_reg_field_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                   a_uint32_t bit_offset, a_uint32_t field_len,
                   a_uint8_t value[], a_uint32_t value_len)
{
    a_uint32_t reg_val = 0;

    if ((bit_offset >= 32 || (field_len > 32)) || (field_len == 0))
        return SW_OUT_OF_RANGE;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    SW_RTN_ON_ERROR(dess_reg_get(dev_id, reg_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));


    if(32 == field_len)
    {
        *((a_uint32_t *) value) = reg_val;
    }
    else
    {
        *((a_uint32_t *) value) = SW_REG_2_FIELD(reg_val, bit_offset, field_len);
    }
    return SW_OK;
}

sw_error_t
dess_reg_field_set(a_uint32_t dev_id, a_uint32_t reg_addr,
                   a_uint32_t bit_offset, a_uint32_t field_len,
                   const a_uint8_t value[], a_uint32_t value_len)
{
    a_uint32_t reg_val = 0;
    a_uint32_t field_val = *((a_uint32_t *) value);

    if ((bit_offset >= 32 || (field_len > 32)) || (field_len == 0))
        return SW_OUT_OF_RANGE;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    SW_RTN_ON_ERROR(dess_reg_get(dev_id, reg_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));

    if(32 == field_len)
    {
        reg_val = field_val;
    }
    else
    {
        SW_REG_SET_BY_FIELD_U32(reg_val, field_val, bit_offset, field_len);
    }


    SW_RTN_ON_ERROR(dess_reg_set(dev_id, reg_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));

    return SW_OK;
}

static sw_error_t
_dess_regsiter_dump(a_uint32_t dev_id,a_uint32_t register_idx, fal_reg_dump_t * reg_dump)
{
    sw_error_t rv = SW_OK;
	typedef struct {
		a_uint32_t reg_base;
		a_uint32_t reg_end;
		char name[30];
	} regdump;

	regdump reg_dumps[8] =
	{
		{0x0, 0xE4, "0.Global control registers"},
		{0x100, 0x168, "1.EEE control registers"},
		{0x200, 0x270, "2.Parser control registers"},
		{0x400, 0x474, "3.ACL control registers"},
		{0x600, 0x718, "4.Lookup control registers"},
		{0x800, 0xb70, "5.QM control registers"},
		{0xc00, 0xc80, "6.PKT edit control registers"},
		{0x820, 0x820, "7.QM debug registers"}
	};

	a_uint32_t dump_addr, reg_count, reg_val = 0;
	switch (register_idx)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			reg_count = 0;
			for (dump_addr = reg_dumps[register_idx].reg_base; dump_addr <= reg_dumps[register_idx].reg_end; reg_count++)
			{
				rv = dess_reg_get(dev_id, dump_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t));
				reg_dump->reg_value[reg_count] = reg_val;
				dump_addr += 4;
			}
			reg_dump->reg_count = reg_count;
			reg_dump->reg_base = reg_dumps[register_idx].reg_base;
			reg_dump->reg_end = reg_dumps[register_idx].reg_end;
			snprintf((char *)reg_dump->reg_name,sizeof(reg_dump->reg_name),"%s",reg_dumps[register_idx].name);
			break;
		default:
			return SW_BAD_PARAM;
	}

    return rv;
}

static sw_error_t
_dess_debug_regsiter_dump(a_uint32_t dev_id,fal_debug_reg_dump_t * dbg_reg_dump)
{
    sw_error_t rv = SW_OK;
    a_uint32_t reg;
	a_uint32_t  reg_count, reg_val = 0;

	reg_count = 0;

	for(reg=0;reg<=0x1F;reg++)
	{
		dess_reg_set(dev_id, 0x820, (a_uint8_t *) & reg, sizeof (a_uint32_t));
		rv = dess_reg_get(dev_id, 0x824, (a_uint8_t *) & reg_val, sizeof (a_uint32_t));
		dbg_reg_dump->reg_value[reg_count] = reg_val;
		dbg_reg_dump->reg_addr[reg_count] = reg;
		reg_count++;
	}
	dbg_reg_dump->reg_count = reg_count;

	snprintf((char *)dbg_reg_dump->reg_name,sizeof(dbg_reg_dump->reg_name),"QM debug registers");
    return rv;
}
static sw_error_t
_dess_debug_psgmii_self_test(a_uint32_t dev_id, a_bool_t enable,
					a_uint32_t times, a_uint32_t * result)
{
	sw_error_t rv = SW_OK;
	ssdk_psgmii_self_test(dev_id, enable, times, result);
	clear_self_test_config(dev_id);

	return rv;
}

static sw_error_t
_dess_phy_dump(a_uint32_t dev_id, a_uint32_t phy_addr,
		a_uint32_t idx, fal_phy_dump_t * phy_dump)
{
	sw_error_t rv = SW_OK;
	typedef struct {
		a_uint32_t phy_base;
		a_uint32_t phy_end;
		char name[30];
	} phydump;

	phydump dump[2] =
	{
		{0x0, 0x1f, "0.mii registers"},
		{0x0, 0x3f, "1.debug registers"},
	};

	a_uint32_t mmd1[] = {0x0,0x5,0x8,0x1800,0x1801,0x1802,0x1803,0x1804,
		0x1805,0x1806,0x1807,0x1808,0x9000,0x9001,0x9002,0x9003,
		0x9004,0x9905,0x9006,0x9007,0x9008,0x9009,0x900a,0x900b};
	char mmd1name[30] = {"2.mmd1 register"};

	a_uint32_t mmd3[] = {0x0,0x1,0x5,0x8,0x14,0x16,0x8001,0x8002,
		0x8003,0x8004,0x8005,0x8006,0x8007,0x8008,0x8009,0x800a,
		0x8045,0x8046,0x8047,0x8048,0x8049,0x804a,0x804b,0x804c,
		0x804d,0x804e,0x804f};
	char mmd3name[30] = {"3.mmd3 register"};

	a_uint32_t mmd7[] = {0x0,0x1,0x5,0x16,0x17,0x18,0x19,0x1a,0x1b,
		0x3c,0x3d,0x8000,0x801a};
	char mmd7name[30] = {"4.mmd7 register"};

	a_uint32_t reg = 0, phy_count = 0,i = 0;
	a_uint16_t phy_val = 0;
	switch (idx)
	{
		case 0:
			for (reg = dump[idx].phy_base; reg <= dump[idx].phy_end; reg++)
			{
				rv = dess_phy_get(dev_id, phy_addr, reg, &phy_val);
				phy_dump->phy_value[phy_count] = phy_val;
				phy_count ++;
			}
			phy_dump->phy_count = phy_count;
			phy_dump->phy_base = dump[idx].phy_base;
			phy_dump->phy_end = dump[idx].phy_end;
			snprintf((char *)phy_dump->phy_name,
				sizeof(phy_dump->phy_name),"%s",dump[idx].name);
			break;
		case 1:
			for (reg = dump[idx].phy_base; reg <= dump[idx].phy_end; reg++)
			{
				rv = dess_phy_set(dev_id, phy_addr, 0x1d, (a_uint16_t)reg);
				rv = dess_phy_get(dev_id, phy_addr, 0x1e, &phy_val);
				phy_dump->phy_value[phy_count] = phy_val;
				phy_count ++;
			}
			phy_dump->phy_count = phy_count;
			phy_dump->phy_base = dump[idx].phy_base;
			phy_dump->phy_end = dump[idx].phy_end;
			snprintf((char *)phy_dump->phy_name,
				sizeof(phy_dump->phy_name),"%s",dump[idx].name);
			break;
		case 2:
			phy_count = sizeof (mmd1)/sizeof(a_uint32_t);
			for (i = 0; i < phy_count; i++ )
			{
				rv = dess_phy_set(dev_id, phy_addr, 0xd, 1);
				rv = dess_phy_set(dev_id, phy_addr, 0xe, (a_uint16_t)mmd1[i]);
				rv = dess_phy_set(dev_id, phy_addr, 0xd, 0x4001);
				rv = dess_phy_get(dev_id, phy_addr, 0xe, &phy_val);
				phy_dump->phy_value[i] = phy_val;
			}
			phy_dump->phy_count = phy_count - 1;
			phy_dump->phy_base = mmd1[0];
			phy_dump->phy_end = mmd1[phy_count - 1];
			snprintf((char *)phy_dump->phy_name,
				sizeof(phy_dump->phy_name),"%s",mmd1name);
			break;
		case 3:
			phy_count = sizeof (mmd3)/sizeof(a_uint32_t);
			for (i = 0; i < phy_count; i++ )
			{
				rv = dess_phy_set(dev_id, phy_addr, 0xd, 3);
				rv = dess_phy_set(dev_id, phy_addr, 0xe, (a_uint16_t)mmd3[i]);
				rv = dess_phy_set(dev_id, phy_addr, 0xd, 0x4003);
				rv = dess_phy_get(dev_id, phy_addr, 0xe, &phy_val);
				phy_dump->phy_value[i] = phy_val;
			}
			phy_dump->phy_count = phy_count - 1;
			phy_dump->phy_base = mmd3[0];
			phy_dump->phy_end = mmd3[phy_count - 1];
			snprintf((char *)phy_dump->phy_name,
				sizeof(phy_dump->phy_name),"%s",mmd3name);
			break;
		case 4:
			phy_count = sizeof (mmd7)/sizeof(a_uint32_t);
			for (i = 0; i < phy_count; i++ )
			{
				rv = dess_phy_set(dev_id, phy_addr, 0xd, 7);
				rv = dess_phy_set(dev_id, phy_addr, 0xe, (a_uint16_t)mmd7[i]);
				rv = dess_phy_set(dev_id, phy_addr, 0xd, 0x4007);
				rv = dess_phy_get(dev_id, phy_addr, 0xe, &phy_val);
				phy_dump->phy_value[i] = phy_val;
			}
			phy_dump->phy_count = phy_count - 1;
			phy_dump->phy_base = mmd7[0];
			phy_dump->phy_end = mmd7[phy_count - 1];
			snprintf((char *)phy_dump->phy_name,
				sizeof(phy_dump->phy_name),"%s",mmd7name);
			break;
		default:
			return SW_BAD_PARAM;
	}

    return rv;
}

/**
 * @brief dump registers.
 * @param[in] dev_id device id
 * @param[in] register_idx register group id
 * @param[out] reg_dump register dump result
 * @return SW_OK or error code
 */
sw_error_t
dess_regsiter_dump(a_uint32_t dev_id,a_uint32_t register_idx, fal_reg_dump_t * reg_dump)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _dess_regsiter_dump(dev_id,register_idx,reg_dump);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief dump registers.
 * @param[in] dev_id device id
 * @param[out] reg_dump debug register dump
 * @return SW_OK or error code
 */
sw_error_t
dess_debug_regsiter_dump(a_uint32_t dev_id, fal_debug_reg_dump_t * dbg_reg_dump)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _dess_debug_regsiter_dump(dev_id,dbg_reg_dump);
    FAL_API_UNLOCK;
    return rv;
}


/**
 * @brief debug psgmii self test.
 * @param[in] dev_id, enable, times
 * @param[out] status
 * @return SW_OK or error code
 */
sw_error_t
dess_debug_psgmii_self_test(a_uint32_t dev_id, a_bool_t enable,
				a_uint32_t times, a_uint32_t * result)

{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _dess_debug_psgmii_self_test(dev_id, enable, times, result);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
dess_phy_dump(a_uint32_t dev_id, a_uint32_t phy_addr,
		a_uint32_t idx, fal_phy_dump_t * phy_dump)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _dess_phy_dump(dev_id, phy_addr, idx, phy_dump);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
dess_reg_access_init(a_uint32_t dev_id, hsl_access_mode mode)
{
    hsl_api_t *p_api;

    MDIO_LOCKER_INIT;
    reg_mode = mode;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
    p_api->phy_get = dess_phy_get;
    p_api->phy_set = dess_phy_set;
    p_api->reg_get = dess_reg_get;
    p_api->reg_set = dess_reg_set;
    p_api->reg_field_get = dess_reg_field_get;
    p_api->reg_field_set = dess_reg_field_set;
   #ifdef IN_INTERFACECONTROL
    p_api->psgmii_reg_get = dess_psgmii_reg_get;
    p_api->psgmii_reg_set = dess_psgmii_reg_set;
   #endif
	p_api->register_dump = dess_regsiter_dump;
	p_api->debug_register_dump = dess_debug_regsiter_dump;
	p_api->debug_psgmii_self_test = dess_debug_psgmii_self_test;
	p_api->phy_dump = dess_phy_dump;


    return SW_OK;
}

sw_error_t
dess_access_mode_set(a_uint32_t dev_id, hsl_access_mode mode)
{
    reg_mode = mode;
    return SW_OK;

}

