/*
 * Copyright (c) 2012, 2016-2018, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_init FAL_INIT
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_api.h"
/*qca808x_end*/
#include "fal_vlan.h"
#include "adpt.h"
/*qca808x_start*/
/**
 * @brief Init fal layer.
 * @details Comments:
 *   This operation will init fal layer and hsl layer
 * @param[in] dev_id device id
 * @param[in] cfg configuration for initialization
 * @return SW_OK or error code
 */
sw_error_t
fal_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    sw_error_t rv;
    HSL_DEV_ID_CHECK(dev_id);

    rv = hsl_api_init(dev_id);
    SW_RTN_ON_ERROR(rv);

    rv = hsl_dev_init(dev_id, cfg);
    SW_RTN_ON_ERROR(rv);
/*qca808x_end*/
#ifdef IN_VLAN
    rv = fal_vlan_init(dev_id);
    SW_RTN_ON_ERROR(rv);
#endif

    rv = adpt_init(dev_id, cfg);
    SW_RTN_ON_ERROR(rv);
/*qca808x_start*/

    return rv;
}
/*qca808x_end*/

static sw_error_t
_fal_reset(a_uint32_t dev_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->dev_reset)
        return SW_NOT_SUPPORTED;

#ifdef IN_VLAN
    rv = fal_vlan_reset(dev_id);
    SW_RTN_ON_ERROR(rv);
#endif

    rv = p_api->dev_reset(dev_id);
    return rv;
}

static sw_error_t
_fal_ssdk_cfg(a_uint32_t dev_id, ssdk_cfg_t *ssdk_cfg)
{
    sw_error_t rv;
    HSL_DEV_ID_CHECK(dev_id);

    rv = hsl_ssdk_cfg(dev_id, ssdk_cfg);

    return rv;
}

static sw_error_t
_fal_module_func_ctrl_set(a_uint32_t dev_id, a_uint32_t module, fal_func_ctrl_t *func_ctrl)
{
    return adpt_module_func_ctrl_set(dev_id, module, func_ctrl);
}

static sw_error_t
_fal_module_func_ctrl_get(a_uint32_t dev_id, a_uint32_t module, fal_func_ctrl_t *func_ctrl)
{
    return adpt_module_func_ctrl_get(dev_id, module, func_ctrl);
}
/*qca808x_start*/
sw_error_t
fal_cleanup(void)
{
    sw_error_t rv;

    rv = hsl_dev_cleanup();
    SW_RTN_ON_ERROR(rv);
/*qca808x_end*/
#ifdef IN_VLAN
    rv = fal_vlan_cleanup();
    SW_RTN_ON_ERROR(rv);
#endif
/*qca808x_start*/
    return SW_OK;
}
/*qca808x_end*/
/**
 * @brief Reset fal layer.
 * @details Comments:
 *   This operation will reset fal layer and hsl layer
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
sw_error_t
fal_reset(a_uint32_t dev_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_reset(dev_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get SSDK config infomation.
 * @param[in] dev_id device id
 * @param[out] ssdk_cfg SSDK config infomation
 * @return SW_OK or error code
 */
sw_error_t
fal_ssdk_cfg(a_uint32_t dev_id, ssdk_cfg_t *ssdk_cfg)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_ssdk_cfg(dev_id, ssdk_cfg);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_switch_devid_get(ssdk_chip_type chip_type, a_uint32_t *pdev_id)
{
	sw_error_t rv = SW_OK;
	ssdk_cfg_t cfg = {0};
	a_uint32_t dev_id = 0;

	for(dev_id = 0; dev_id < SW_MAX_NR_DEV; dev_id++) {
		rv = _fal_ssdk_cfg(dev_id, &cfg);
		if(rv == SW_OK && cfg.init_cfg.chip_type == chip_type) {
			*pdev_id = dev_id;
			return rv;
		}
	}
	return SW_FAIL;
}

sw_error_t
fal_module_func_ctrl_set(a_uint32_t dev_id, a_uint32_t module, fal_func_ctrl_t *func_ctrl)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_module_func_ctrl_set(dev_id, module, func_ctrl);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_module_func_ctrl_get(a_uint32_t dev_id, a_uint32_t module, fal_func_ctrl_t *func_ctrl)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_module_func_ctrl_get(dev_id, module, func_ctrl);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_module_func_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    sw_error_t rv;
    HSL_DEV_ID_CHECK(dev_id);

    rv = adpt_module_func_init(dev_id, cfg);
    SW_RTN_ON_ERROR(rv);

    return rv;
}

EXPORT_SYMBOL(fal_switch_devid_get);
EXPORT_SYMBOL(fal_module_func_ctrl_set);
EXPORT_SYMBOL(fal_module_func_ctrl_get);

/**
 * @}
 */
