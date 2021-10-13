/*
 * Copyright (c) 2012, 2017-2019, The Linux Foundation. All rights reserved.
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
#include "hsl_lock.h"
#include "sd.h"
/*qca808x_end*/
#if defined ATHENA
#include "athena_init.h"
#endif
#if defined GARUDA
#include "garuda_init.h"
#endif
#if defined SHIVA
#include "shiva_init.h"
#endif
#if defined HORUS
#include "horus_init.h"
#endif
#if defined ISIS
#include "isis_init.h"
#endif
#if defined ISISC
#include "isisc_init.h"
#endif
#if defined DESS
#include "dess_init.h"
#endif
#if defined HPPE
#include "hppe_init.h"
#endif
#if defined SCOMPHY
/*qca808x_start*/
#include "scomphy_init.h"
/*qca808x_end*/
#endif
/*qca808x_start*/
#include "sw_api.h"
/*qca808x_end*/
#ifdef KERNEL_MODULE
/*qca808x_start*/
#include "sw_api_ks.h"
/*qca808x_end*/
#else
#include "sw_api_us.h"
#endif
#include "ssdk_plat.h"
#ifdef MP
#include "hsl_phy.h"
#endif
/*qca808x_start*/
static hsl_dev_t dev_table[SW_MAX_NR_DEV];
static ssdk_init_cfg *dev_ssdk_cfg[SW_MAX_NR_DEV] = { 0 };
ssdk_chip_type SSDK_CURRENT_CHIP_TYPE = CHIP_UNSPECIFIED;

static sw_error_t hsl_set_current_chip_type(ssdk_chip_type chip_type)
{
    sw_error_t rv = SW_OK;

    SSDK_CURRENT_CHIP_TYPE = chip_type;

    if (SSDK_CURRENT_CHIP_TYPE == CHIP_UNSPECIFIED)
    {
/*qca808x_end*/
#if defined ATHENA
        SSDK_CURRENT_CHIP_TYPE = CHIP_ATHENA;
#elif defined GARUDA
        SSDK_CURRENT_CHIP_TYPE = CHIP_GARUDA;
#elif defined SHIVA
        SSDK_CURRENT_CHIP_TYPE = CHIP_SHIVA;
#elif defined HORUS
        SSDK_CURRENT_CHIP_TYPE = CHIP_HORUS;
#elif defined ISIS
        SSDK_CURRENT_CHIP_TYPE = CHIP_ISIS;
#elif defined ISISC
        SSDK_CURRENT_CHIP_TYPE = CHIP_ISISC;
#elif defined DESS
        SSDK_CURRENT_CHIP_TYPE = CHIP_DESS;
#elif defined HPPE
        SSDK_CURRENT_CHIP_TYPE = CHIP_HPPE;
#elif defined SCOMPHY
/*qca808x_start*/
        SSDK_CURRENT_CHIP_TYPE = CHIP_SCOMPHY;
/*qca808x_end*/
#else
        rv = SW_FAIL;
#endif
/*qca808x_start*/
    }
    return rv;
}

hsl_dev_t *
hsl_dev_ptr_get(a_uint32_t dev_id)
{
    if (dev_id >= SW_MAX_NR_DEV)
        return NULL;

    return &dev_table[dev_id];
}
/*qca808x_end*/
hsl_acl_func_t *
hsl_acl_ptr_get(a_uint32_t dev_id)
{
    if (dev_id >= SW_MAX_NR_DEV)
        return NULL;

    return &(dev_table[dev_id].acl_func);
}

a_uint32_t hsl_dev_wan_port_get(a_uint32_t dev_id)
{
	if(dev_ssdk_cfg[dev_id]) {
		return dev_ssdk_cfg[dev_id]->port_cfg.wan_bmp;
	}
	return 0;
}

a_uint32_t hsl_dev_inner_ports_get(a_uint32_t dev_id)
{
	if(dev_ssdk_cfg[dev_id]) {
		return dev_ssdk_cfg[dev_id]->port_cfg.inner_bmp;
	}
	return 0;
}

/*qca808x_start*/
sw_error_t
hsl_dev_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    sw_error_t rv = SW_OK;
    static int dev_init = 0;

    if (SW_MAX_NR_DEV <= dev_id)
    {
        return SW_BAD_PARAM;
    }

    aos_mem_set(&dev_table[dev_id], 0, sizeof (hsl_dev_t));

    if (!dev_init) {
        SW_RTN_ON_ERROR(sd_init(dev_id,cfg));

#ifdef UK_IF
        SW_RTN_ON_ERROR(sw_uk_init(cfg->nl_prot));
#endif

#if defined API_LOCK
        SW_RTN_ON_ERROR(hsl_api_lock_init());
#endif
        dev_init = 1;
    }
    rv = hsl_set_current_chip_type(cfg->chip_type);
    SW_RTN_ON_ERROR(rv);

    if (NULL == dev_ssdk_cfg[dev_id])
    {
        dev_ssdk_cfg[dev_id] = aos_mem_alloc(sizeof (ssdk_init_cfg));
    }

    if (NULL == dev_ssdk_cfg[dev_id])
    {
        return SW_OUT_OF_MEM;
    }

    aos_mem_copy(dev_ssdk_cfg[dev_id], cfg, sizeof (ssdk_init_cfg));
#if defined UK_MINOR_DEV
    dev_ssdk_cfg[dev_id]->nl_prot = UK_MINOR_DEV;
#endif

    rv = SW_INIT_ERROR;
    switch (cfg->chip_type)
    {
/*qca808x_end*/
        case CHIP_ATHENA:
#if defined ATHENA
            rv = athena_init(dev_id, cfg);
#endif
            break;

        case CHIP_GARUDA:
#if defined GARUDA
            rv = garuda_init(dev_id, cfg);
#endif
            break;

        case CHIP_SHIVA:
#if defined SHIVA
            rv = shiva_init(dev_id, cfg);
#endif
            break;

        case CHIP_HORUS:
#if defined HORUS
            rv = horus_init(dev_id, cfg);
#endif
            break;

        case CHIP_ISIS:
#if defined ISIS
            rv = isis_init(dev_id, cfg);
#endif
            break;
        case CHIP_ISISC:
#if defined ISISC
            rv = isisc_init(dev_id, cfg);
#endif
            break;
        case CHIP_DESS:
#if defined DESS
            rv = dess_init(dev_id, cfg);
#endif
            break;
        case CHIP_HPPE:
#if defined HPPE
            rv = hppe_init(dev_id, cfg);
#endif
            break;
/*qca808x_start*/
	case CHIP_SCOMPHY:
/*qca808x_end*/
#if defined SCOMPHY
/*qca808x_start*/
	    rv = scomphy_init(dev_id, cfg);
/*qca808x_end*/
#endif
/*qca808x_start*/
	    break;
/*qca808x_end*/
        case CHIP_UNSPECIFIED:
#if defined ATHENA
            rv = athena_init(dev_id, cfg);
#elif defined GARUDA
            rv = garuda_init(dev_id, cfg);
#elif defined SHIVA
            rv = shiva_init(dev_id, cfg);
#elif defined HORUS
            rv = horus_init(dev_id, cfg);
#elif defined ISIS
            rv = isis_init(dev_id, cfg);
#elif defined ISISC
            rv = isisc_init(dev_id, cfg);
#endif
            break;
/*qca808x_start*/
        default:
            return SW_BAD_PARAM;
    }

    return rv;
}
/*qca808x_end*/
sw_error_t
hsl_ssdk_cfg(a_uint32_t dev_id, ssdk_cfg_t *ssdk_cfg)
{
    if(!dev_ssdk_cfg[dev_id])
    {
        SSDK_ERROR("the dev%d wasn't initialized\n", dev_id);
        return SW_BAD_VALUE;
    }
    aos_mem_set(&(ssdk_cfg->init_cfg), 0,  sizeof(ssdk_init_cfg));

    aos_mem_copy(&(ssdk_cfg->init_cfg), dev_ssdk_cfg[dev_id], sizeof(ssdk_init_cfg));

#ifdef VERSION
    aos_mem_copy(ssdk_cfg->build_ver, VERSION, sizeof(VERSION));
#endif

#ifdef BUILD_DATE
    aos_mem_copy(ssdk_cfg->build_date, BUILD_DATE, sizeof(BUILD_DATE));
#endif

    switch (dev_ssdk_cfg[dev_id]->chip_type)
    {
        case CHIP_ATHENA:
            aos_mem_copy(ssdk_cfg->chip_type, "athena", sizeof("athena"));
            break;

        case CHIP_GARUDA:
            aos_mem_copy(ssdk_cfg->chip_type, "garuda", sizeof("garuda"));
            break;

        case CHIP_SHIVA:
            aos_mem_copy(ssdk_cfg->chip_type, "shiva", sizeof("shiva"));
            break;

        case CHIP_HORUS:
            aos_mem_copy(ssdk_cfg->chip_type, "horus", sizeof("horus"));
            break;

        case CHIP_ISIS:
            aos_mem_copy(ssdk_cfg->chip_type, "isis", sizeof("isis"));
            break;

        case CHIP_ISISC:
            aos_mem_copy(ssdk_cfg->chip_type, "isisc", sizeof("isisc"));
            break;

        case CHIP_DESS:
            aos_mem_copy(ssdk_cfg->chip_type, "dess", sizeof("dess"));
            break;

        case CHIP_HPPE:
            aos_mem_copy(ssdk_cfg->chip_type, "hppe", sizeof("hppe"));
            break;

        case CHIP_SCOMPHY:
#ifdef MP
            if(dev_ssdk_cfg[dev_id]->chip_revision == MP_GEPHY)
            {
                aos_mem_copy(ssdk_cfg->chip_type, "mp", sizeof("mp"));
            }
#endif
            break;

        case CHIP_UNSPECIFIED:
#if defined ATHENA
            aos_mem_copy(ssdk_cfg->chip_type, "athena", sizeof("athena"));
#elif defined GARUDA
            aos_mem_copy(ssdk_cfg->chip_type, "garuda", sizeof("garuda"));
#elif defined SHIVA
            aos_mem_copy(ssdk_cfg->chip_type, "shiva", sizeof("shiva"));
#elif defined HORUS
            aos_mem_copy(ssdk_cfg->chip_type, "horus", sizeof("horus"));
#elif defined ISIS
            aos_mem_copy(ssdk_cfg->chip_type, "isis", sizeof("isis"));
#elif defined ISISC
            aos_mem_copy(ssdk_cfg->chip_type, "isisc", sizeof("isisc"));
#endif
            break;

        default:
            return SW_BAD_PARAM;
    }

#ifdef CPU
    aos_mem_copy(ssdk_cfg->cpu_type, CPU, sizeof(CPU));
#endif

#ifdef OS
    aos_mem_copy(ssdk_cfg->os_info, OS, sizeof(OS));
#if defined KVER26
    aos_mem_copy(ssdk_cfg->os_info+sizeof(OS)-1, " version 2.6", sizeof(" version 2.6"));
#elif defined KVER24
    aos_mem_copy(ssdk_cfg->os_info+sizeof(OS)-1, " version 2.4", sizeof(" version 2.4"));
#else
    aos_mem_copy(ssdk_cfg->os_info+sizeof(OS)-1, " unknown", sizeof(" unknown"));
#endif
#endif

#ifdef HSL_STANDALONG
    ssdk_cfg->fal_mod = A_FALSE;
#else
    ssdk_cfg->fal_mod = A_TRUE;
#endif

#ifdef USER_MODE
    ssdk_cfg->kernel_mode = A_FALSE;
#else
    ssdk_cfg->kernel_mode = A_TRUE;
#endif

#ifdef UK_IF
    ssdk_cfg->uk_if = A_TRUE;
#else
    ssdk_cfg->uk_if = A_FALSE;
#endif

#ifdef IN_ACL
    ssdk_cfg->features.in_acl = A_TRUE;
#endif
#ifdef IN_FDB
    ssdk_cfg->features.in_fdb = A_TRUE;
#endif
#ifdef IN_IGMP
    ssdk_cfg->features.in_igmp = A_TRUE;
#endif
#ifdef IN_LEAKY
    ssdk_cfg->features.in_leaky = A_TRUE;
#endif
#ifdef IN_LED
    ssdk_cfg->features.in_led = A_TRUE;
#endif
#ifdef IN_MIB
    ssdk_cfg->features.in_mib = A_TRUE;
#endif
#ifdef IN_MIRROR
    ssdk_cfg->features.in_mirror = A_TRUE;
#endif
#ifdef IN_MISC
    ssdk_cfg->features.in_misc = A_TRUE;
#endif
#ifdef IN_PORTCONTROL
    ssdk_cfg->features.in_portcontrol = A_TRUE;
#endif
#ifdef IN_PORTVLAN
    ssdk_cfg->features.in_portvlan = A_TRUE;
#endif
#ifdef IN_QOS
    ssdk_cfg->features.in_qos = A_TRUE;
#endif
#ifdef IN_RATE
    ssdk_cfg->features.in_rate = A_TRUE;
#endif
#ifdef IN_STP
    ssdk_cfg->features.in_stp = A_TRUE;
#endif
#ifdef IN_VLAN
    ssdk_cfg->features.in_vlan = A_TRUE;
#endif
#ifdef IN_REDUCED_ACL
    ssdk_cfg->features.in_reduced_acl = A_TRUE;
#endif
#ifdef IN_IP
    ssdk_cfg->features.in_ip = A_TRUE;
#endif
#ifdef IN_NAT
    ssdk_cfg->features.in_nat = A_TRUE;
#endif
#ifdef IN_COSMAP
    ssdk_cfg->features.in_cosmap = A_TRUE;
#endif
#ifdef IN_SEC
    ssdk_cfg->features.in_sec = A_TRUE;
#endif
#ifdef IN_TRUNK
    ssdk_cfg->features.in_trunk = A_TRUE;
#endif
#ifdef IN_NAT_HELPER
    ssdk_cfg->features.in_nathelper= A_TRUE;
#endif
#ifdef IN_INTERFACECONTROL
    ssdk_cfg->features.in_interfacectrl= A_TRUE;
#endif

    return SW_OK;
}
/*qca808x_start*/
sw_error_t
hsl_dev_cleanup(void)
{
    sw_error_t rv = SW_OK;
    a_uint32_t dev_id;

    for (dev_id = 0; dev_id < SW_MAX_NR_DEV; dev_id++)
    {
        if (dev_ssdk_cfg[dev_id])
        {
            hsl_api_t *p_api;

            SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
            if (p_api->dev_clean)
            {
                rv = p_api->dev_clean(dev_id);
                SW_RTN_ON_ERROR(rv);
            }

            aos_mem_free(dev_ssdk_cfg[dev_id]);
            dev_ssdk_cfg[dev_id] = NULL;
        }
    }

#ifdef UK_IF
    SW_RTN_ON_ERROR(sw_uk_cleanup());
#endif

    return SW_OK;
}
/*qca808x_end*/
sw_error_t
hsl_access_mode_set(a_uint32_t dev_id, hsl_access_mode reg_mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
    if (p_api->dev_access_set)
    {
        rv = p_api->dev_access_set(dev_id, reg_mode);
    }
    else
    {
        return SW_BAD_PARAM;
    }

    return rv;
}

sw_error_t reduce_hsl_reg_entry_get(a_uint32_t dev,a_uint32_t reg,a_uint8_t* value,a_uint8_t val_len)
{
	sw_error_t rv;

	hsl_api_t *p_api = hsl_api_ptr_get(dev);
	if (p_api) {
		rv = p_api->reg_get(dev, reg, (a_uint8_t*)value, (a_uint8_t)val_len);
	} else {
		rv = SW_NOT_INITIALIZED;
	}

	return rv;
}

sw_error_t reduce_hsl_reg_entry_set(a_uint32_t dev,a_uint32_t reg,a_uint8_t* value,a_uint8_t val_len)
{
	sw_error_t rv;

	hsl_api_t *p_api = hsl_api_ptr_get(dev);
	if (p_api) {
	    rv = p_api->reg_set (dev, reg,
	                               (a_uint8_t*)value, (a_uint8_t)val_len);
	} else {
	    rv = SW_NOT_INITIALIZED;
	}

	return rv;

}

sw_error_t reduce_hsl_reg_field_get(a_uint32_t dev,a_uint32_t reg,a_uint32_t reg_offset,
						a_uint32_t reg_offset_len,a_uint8_t* value,a_uint8_t val_len)
{
	sw_error_t rv;

	hsl_api_t *p_api = hsl_api_ptr_get(dev);
	if (p_api) {
		rv = p_api->reg_field_get(dev, reg, reg_offset, reg_offset_len, (a_uint8_t*)value, val_len);
	} else {
		rv = SW_NOT_INITIALIZED;
	}
	return rv;
}

sw_error_t reduce_hsl_reg_field_set(a_uint32_t dev,a_uint32_t reg,a_uint32_t reg_offset,
						a_uint32_t reg_offset_len,a_uint8_t* value,a_uint8_t val_len)
{
	sw_error_t rv;

	hsl_api_t *p_api = hsl_api_ptr_get(dev);
	if (p_api) {
		rv = p_api->reg_field_set(dev, reg,
								  reg_offset,
								  reg_offset_len, (a_uint8_t*)value, val_len);
	} else {
		rv = SW_NOT_INITIALIZED;
	}
	return rv;
}

sw_error_t reduce_hsl_reg_entry_gen_get(a_uint32_t dev,a_uint32_t addr,a_uint8_t* value,a_uint8_t val_len)
{
	sw_error_t rv;

	hsl_api_t *p_api = hsl_api_ptr_get(dev);
	if (p_api) {
		rv = p_api->reg_get(dev, addr, (a_uint8_t*)value, val_len);
	} else {
		rv = SW_NOT_INITIALIZED;
	}

	return rv;
}


sw_error_t reduce_hsl_reg_entry_gen_set(a_uint32_t dev,a_uint32_t addr,a_uint8_t* value,a_uint8_t val_len)
{
	sw_error_t rv;

	hsl_api_t *p_api = hsl_api_ptr_get(dev);
	if (p_api) {
		rv = p_api->reg_set(dev, addr, (a_uint8_t*)value, val_len);
	} else {
		rv = SW_NOT_INITIALIZED;
	}

	return rv;
}

sw_error_t reduce_hsl_reg_field_gen_get(a_uint32_t dev,a_uint32_t reg_addr,a_uint32_t bitoffset,
						a_uint32_t field_len,a_uint8_t* value,a_uint8_t val_len)
{
	sw_error_t rv;

	hsl_api_t *p_api = hsl_api_ptr_get(dev);
	if (p_api) {
		rv = p_api->reg_field_get(dev, reg_addr,
								  bitoffset,
								  field_len, (a_uint8_t*)value, val_len);
	} else {
		rv = SW_NOT_INITIALIZED;
	}
	return rv;
}


sw_error_t reduce_hsl_reg_field_gen_set(a_uint32_t dev,a_uint32_t regaddr,a_uint32_t bitoffset,
						a_uint32_t bitlength,a_uint8_t* value,a_uint8_t val_len)
{
	sw_error_t rv;

	hsl_api_t *p_api = hsl_api_ptr_get(dev);
	if (p_api) {
		rv = p_api->reg_field_set(dev, regaddr,
								  bitoffset,
								  bitlength, (a_uint8_t*)value, val_len);
	} else {
		rv = SW_NOT_INITIALIZED;
	}
	return rv;
}

/*qca808x_start*/
sw_error_t reduce_hsl_phy_set(a_uint32_t dev,a_uint32_t phy_addr,a_uint32_t reg,a_uint16_t value)
{
	sw_error_t rv;

	hsl_api_t *p_api = hsl_api_ptr_get(dev); 
	if (p_api) { 
	    rv = p_api->phy_set(dev, phy_addr, reg, value); 
	} else { 
	    rv = SW_NOT_INITIALIZED; 
	} 

	return rv;
}

sw_error_t reduce_hsl_phy_get(a_uint32_t dev,a_uint32_t phy_addr,a_uint32_t reg,a_uint16_t* value)
{
	sw_error_t rv;

	hsl_api_t *p_api = hsl_api_ptr_get(dev);
	if (p_api) {
	    rv = p_api->phy_get(dev, phy_addr, reg, value);
	} else {
	    rv = SW_NOT_INITIALIZED;
	}

	return rv;
}

sw_error_t hsl_phy_i2c_set(a_uint32_t dev,a_uint32_t phy_addr,a_uint32_t reg,a_uint16_t value)
{
	sw_error_t rv;

	hsl_api_t *p_api = hsl_api_ptr_get(dev);
	if (p_api) {
	    rv = p_api->phy_i2c_set(dev, phy_addr, reg, value);
	} else {
	    rv = SW_NOT_INITIALIZED;
	}

	return rv;
}

sw_error_t hsl_phy_i2c_get(a_uint32_t dev,a_uint32_t phy_addr,a_uint32_t reg,a_uint16_t* value)
{
	sw_error_t rv;

	hsl_api_t *p_api = hsl_api_ptr_get(dev);
	if (p_api) {
	    rv = p_api->phy_i2c_get(dev, phy_addr, reg, value);
	} else {
	    rv = SW_NOT_INITIALIZED;
	}

	return rv;
}

#if 0
void reduce_sw_set_reg_by_field_u32(unsigned int reg_value,unsigned int field_value,
													unsigned int reg_offset,unsigned int reg_len)
{
    do {
        (reg_value) = (((reg_value) & SW_FIELD_MASK_NOT_U32((reg_offset),(reg_offset)))
              | (((field_value) & SW_BIT_MASK_U32(reg_len)) << (reg_offset)));
    } while (0);

}


void reduce_sw_field_get_by_reg_u32(unsigned int reg_value,unsigned int field_value,
													unsigned int reg_offset,unsigned int reg_len)
{
    do {
        (field_value) = (((reg_value) >> (reg_offset)) & SW_BIT_MASK_U32(reg_len));
    } while (0);

}
#endif
/*qca808x_end*/

