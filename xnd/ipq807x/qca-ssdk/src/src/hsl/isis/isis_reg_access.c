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
#include "isis_reg_access.h"

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

int
isis_reg_config_header (a_uint8_t *header,  a_uint8_t wr_flag,
                        a_uint32_t reg_addr, a_uint8_t cmd_len,
                        a_uint8_t *val, a_uint32_t seq_num)
{
    athrs_header_t athrs_header;
    athrs_header_regcmd_t reg_cmd;
    a_uint16_t head_offset = ISIS_HEADER_CMD_LEN + ISIS_HEADER_DATA_LEN;
    a_uint8_t buf[ISIS_HEADER_CMD_LEN+ISIS_HEADER_LEN+ISIS_HEADER_MAX_DATA_LEN] = { 0 };
    a_uint16_t data2_offset = ISIS_HEADER_CMD_LEN + ISIS_HEADER_DATA_LEN +  ISIS_HEADER_LEN;

    aos_mem_set(&athrs_header, 0, sizeof(athrs_header));
    aos_mem_set(&reg_cmd, 0, sizeof(reg_cmd));
    aos_mem_set(buf,    0, sizeof(buf));

    /*fill atheros header*/
    athrs_header.version =  2;
    athrs_header.priority =  0;
    athrs_header.type =      1;/*READ_WRITE_REG*/
    athrs_header.broadcast = 0;
    athrs_header.from_cpu =  1;
    athrs_header.port_num =  0;

    /*fill in 4 byte type atheros header witch specific header type
    must config reg 0x98*/
    buf[head_offset] = (ATHRS_HEADER_4BYTE_VAL & 0xff00)>>8;
    buf[head_offset+1] = ATHRS_HEADER_4BYTE_VAL & 0xff;

    buf[head_offset+2] = athrs_header.type;
    buf[head_offset+2] |= athrs_header.priority << 3;
    buf[head_offset+2] |= athrs_header.version << 6;
    buf[head_offset+3] = athrs_header.port_num;
    buf[head_offset+3] |= athrs_header.from_cpu << 7;

    /*fill reg cmd*/
    if(cmd_len > ISIS_HEADER_MAX_DATA_LEN)
        cmd_len = ISIS_HEADER_MAX_DATA_LEN;//maximum data length is16 bytes
    reg_cmd.reg_addr = reg_addr&0x7fffc; /*bit 0:18, lower 2 bits must be 0*/
    reg_cmd.cmd_len = cmd_len;
    reg_cmd.cmd = wr_flag;
    reg_cmd.check_code = 5;
    reg_cmd.seq_num = seq_num;

    /*bit[0:18], reg addr*/
    buf[0] = reg_cmd.reg_addr & 0xff;
    buf[1] = (reg_cmd.reg_addr & 0xff00) >> 8;
    buf[2] = (reg_cmd.reg_addr & 0x70000) >> 16;
    /*bit[19:23], cmd data length*/
    buf[2] |= reg_cmd.cmd_len << 3;
    /*bit[28], cmd type, read/write*/
    buf[3] = reg_cmd.cmd << 4;
    /*bit[29:31],  check code, must be 3'b101*/
    buf[3] |= reg_cmd.check_code << 5;
    /*bit[32:63], sequence num*/
    buf[4] = (reg_cmd.seq_num & 0xff);
    buf[5] = (reg_cmd.seq_num & 0xff00) >> 8;
    buf[6] = (reg_cmd.seq_num & 0xff0000) >> 16;
    buf[7] = (reg_cmd.seq_num & 0xff000000) >> 24;

    if(!wr_flag)//write
    {
        aos_mem_copy(buf+ ISIS_HEADER_CMD_LEN , val, ISIS_HEADER_DATA_LEN);
        if (cmd_len >4 )
            aos_mem_copy(buf+ data2_offset , val + ISIS_HEADER_DATA_LEN, cmd_len - ISIS_HEADER_DATA_LEN);
    }

    aos_mem_copy(header, buf, sizeof(buf));
    return 0;
}


sw_error_t isis_reg_parser_header_skb(a_uint8_t *header_buf, athrs_cmd_resp_t *cmd_resp)
{
    a_uint16_t data2_offset = ISIS_HEADER_CMD_LEN + ISIS_HEADER_DATA_LEN + ISIS_HEADER_LEN;
    aos_mem_set(cmd_resp, 0, sizeof(cmd_resp));
    cmd_resp->len = header_buf[2] >> 3;
    if (cmd_resp->len > ISIS_HEADER_MAX_DATA_LEN)
        return SW_BAD_LEN;

    cmd_resp->seq = 0;
    cmd_resp->seq = header_buf[4];
    cmd_resp->seq |= header_buf[5] << 8;
    cmd_resp->seq |= header_buf[6] << 16;
    cmd_resp->seq |= header_buf[7] << 24;

    aos_mem_copy (cmd_resp->data, (header_buf + ISIS_HEADER_CMD_LEN), ISIS_HEADER_DATA_LEN);
    if (cmd_resp->len > 4)
        aos_mem_copy ((cmd_resp->data+ISIS_HEADER_DATA_LEN), (header_buf + data2_offset), cmd_resp->len-4);
    return SW_OK;
}

static sw_error_t
_isis_mdio_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr,
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
_isis_mdio_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
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
isis_phy_get(a_uint32_t dev_id, a_uint32_t phy_addr,
             a_uint32_t reg, a_uint16_t * value)
{
    sw_error_t rv;

    MDIO_LOCKER_LOCK;
    rv = sd_reg_mdio_get(dev_id, phy_addr, reg, value);
    MDIO_LOCKER_UNLOCK;
    return rv;
}

sw_error_t
isis_phy_set(a_uint32_t dev_id, a_uint32_t phy_addr,
             a_uint32_t reg, a_uint16_t value)
{
    sw_error_t rv;

    MDIO_LOCKER_LOCK;
    rv = sd_reg_mdio_set(dev_id, phy_addr, reg, value);
    MDIO_LOCKER_UNLOCK;
    return rv;
}

sw_error_t
isis_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
             a_uint32_t value_len)
{
    sw_error_t rv;

    MDIO_LOCKER_LOCK;
    if (HSL_MDIO == reg_mode)
    {
        rv = _isis_mdio_reg_get(dev_id, reg_addr, value, value_len);
    }
    else
    {
        rv = sd_reg_hdr_get(dev_id, reg_addr, value, value_len);
    }
    MDIO_LOCKER_UNLOCK;

    return rv;
}

sw_error_t
isis_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
             a_uint32_t value_len)
{
    sw_error_t rv;

    MDIO_LOCKER_LOCK;
    if (HSL_MDIO == reg_mode)
    {
        rv = _isis_mdio_reg_set(dev_id, reg_addr, value, value_len);
    }
    else
    {
        rv = sd_reg_hdr_set(dev_id, reg_addr, value, value_len);
    }
    MDIO_LOCKER_UNLOCK;

    return rv;
}

sw_error_t
isis_reg_field_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                   a_uint32_t bit_offset, a_uint32_t field_len,
                   a_uint8_t value[], a_uint32_t value_len)
{
    a_uint32_t reg_val = 0;

    if ((bit_offset >= 32 || (field_len > 32)) || (field_len == 0))
        return SW_OUT_OF_RANGE;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    SW_RTN_ON_ERROR(isis_reg_get(dev_id, reg_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));

    *((a_uint32_t *) value) = SW_REG_2_FIELD(reg_val, bit_offset, field_len);
    return SW_OK;
}

sw_error_t
isis_reg_field_set(a_uint32_t dev_id, a_uint32_t reg_addr,
                   a_uint32_t bit_offset, a_uint32_t field_len,
                   const a_uint8_t value[], a_uint32_t value_len)
{
    a_uint32_t reg_val = 0;
    a_uint32_t field_val = *((a_uint32_t *) value);

    if ((bit_offset >= 32 || (field_len > 32)) || (field_len == 0))
        return SW_OUT_OF_RANGE;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    SW_RTN_ON_ERROR(isis_reg_get(dev_id, reg_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));

    SW_REG_SET_BY_FIELD_U32(reg_val, field_val, bit_offset, field_len);

    SW_RTN_ON_ERROR(isis_reg_set(dev_id, reg_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));

    return SW_OK;
}

static sw_error_t
_isis_regsiter_dump(a_uint32_t dev_id,a_uint32_t register_idx, fal_reg_dump_t * reg_dump)
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
				rv = isis_reg_get(dev_id, dump_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t));
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
_isis_debug_regsiter_dump(a_uint32_t dev_id,fal_debug_reg_dump_t * dbg_reg_dump)
{
    sw_error_t rv = SW_OK;
    a_uint32_t reg;
	a_uint32_t  reg_count, reg_val = 0;

	reg_count = 0;

	for(reg=0;reg<=0x1F;reg++)
	{
		isis_reg_set(dev_id, 0x820, (a_uint8_t *) & reg, sizeof (a_uint32_t));
		rv = isis_reg_get(dev_id, 0x824, (a_uint8_t *) & reg_val, sizeof (a_uint32_t));
		dbg_reg_dump->reg_value[reg_count] = reg_val;
		dbg_reg_dump->reg_addr[reg_count] = reg;
		reg_count++;
	}
	dbg_reg_dump->reg_count = reg_count;

	snprintf((char *)dbg_reg_dump->reg_name,sizeof(dbg_reg_dump->reg_name),"QM debug registers");

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
isis_regsiter_dump(a_uint32_t dev_id,a_uint32_t register_idx, fal_reg_dump_t * reg_dump)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _isis_regsiter_dump(dev_id,register_idx,reg_dump);
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
isis_debug_regsiter_dump(a_uint32_t dev_id, fal_debug_reg_dump_t * dbg_reg_dump)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _isis_debug_regsiter_dump(dev_id,dbg_reg_dump);
    FAL_API_UNLOCK;
    return rv;
}


sw_error_t
isis_reg_access_init(a_uint32_t dev_id, hsl_access_mode mode)
{
    hsl_api_t *p_api;

    MDIO_LOCKER_INIT;
    reg_mode = mode;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
    p_api->phy_get = isis_phy_get;
    p_api->phy_set = isis_phy_set;
    p_api->reg_get = isis_reg_get;
    p_api->reg_set = isis_reg_set;
    p_api->reg_field_get = isis_reg_field_get;
    p_api->reg_field_set = isis_reg_field_set;
	p_api->register_dump = isis_regsiter_dump;
	p_api->debug_register_dump = isis_debug_regsiter_dump;

    return SW_OK;
}

sw_error_t
isis_access_mode_set(a_uint32_t dev_id, hsl_access_mode mode)
{
    reg_mode = mode;
    return SW_OK;

}

