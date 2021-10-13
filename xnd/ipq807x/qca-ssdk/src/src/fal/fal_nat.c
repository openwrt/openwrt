/*
 * Copyright (c) 2012, 2015, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_nat FAL_NAT
 * @{
 */

#include "sw.h"
#include "fal_nat.h"
#include "hsl_api.h"

#include <linux/kernel.h>
#include <linux/module.h>

int nf_athrs17_hnat_sync_counter_en = 0;

static sw_error_t
_fal_nat_add(a_uint32_t dev_id, fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_add(dev_id, nat_entry);
    return rv;
}

static sw_error_t
_fal_nat_del(a_uint32_t dev_id, a_uint32_t del_mode,
             fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_del(dev_id, del_mode, nat_entry);
    return rv;
}

static sw_error_t
_fal_nat_get(a_uint32_t dev_id, a_uint32_t get_mode,
             fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_get(dev_id, get_mode, nat_entry);
    return rv;
}

static sw_error_t
_fal_nat_next(a_uint32_t dev_id, a_uint32_t next_mode,
              fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_next)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_next(dev_id, next_mode, nat_entry);
    return rv;
}

static sw_error_t
_fal_nat_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id, a_uint32_t cnt_id,
                      a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_counter_bind)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_counter_bind(dev_id, entry_id, cnt_id, enable);
    return rv;
}

static sw_error_t
_fal_napt_add(a_uint32_t dev_id, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->napt_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->napt_add(dev_id, napt_entry);
    return rv;
}

static sw_error_t
_fal_napt_del(a_uint32_t dev_id, a_uint32_t del_mode,
              fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->napt_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->napt_del(dev_id, del_mode, napt_entry);
    return rv;
}

static sw_error_t
_fal_napt_get(a_uint32_t dev_id, a_uint32_t get_mode,
              fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->napt_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->napt_get(dev_id, get_mode, napt_entry);
    return rv;
}

static sw_error_t
_fal_napt_next(a_uint32_t dev_id, a_uint32_t next_mode,
               fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->napt_next)
        return SW_NOT_SUPPORTED;

    rv = p_api->napt_next(dev_id, next_mode, napt_entry);
    return rv;
}

static sw_error_t
_fal_napt_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                       a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->napt_counter_bind)
        return SW_NOT_SUPPORTED;

    rv = p_api->napt_counter_bind(dev_id, entry_id, cnt_id, enable);
    return rv;
}

static sw_error_t
_fal_flow_add(a_uint32_t dev_id, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->flow_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->flow_add(dev_id, napt_entry);
    return rv;
}

static sw_error_t

_fal_flow_cookie_set(a_uint32_t dev_id, fal_flow_cookie_t * flow_cookie)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->flow_cookie_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->flow_cookie_set(dev_id, flow_cookie);
	return rv;
}

static sw_error_t

_fal_flow_rfs_set(a_uint32_t dev_id, a_uint8_t action, fal_flow_rfs_t * rfs)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->flow_rfs_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->flow_rfs_set(dev_id, action, rfs);
	return rv;
}



static sw_error_t
_fal_flow_del(a_uint32_t dev_id, a_uint32_t del_mode,
              fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->flow_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->flow_del(dev_id, del_mode, napt_entry);
    return rv;
}

static sw_error_t
_fal_flow_get(a_uint32_t dev_id, a_uint32_t get_mode,
              fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->flow_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->flow_get(dev_id, get_mode, napt_entry);
    return rv;
}

static sw_error_t
_fal_flow_next(a_uint32_t dev_id, a_uint32_t next_mode,
               fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->flow_next)
        return SW_NOT_SUPPORTED;

    rv = p_api->flow_next(dev_id, next_mode, napt_entry);
    return rv;
}

static sw_error_t
_fal_flow_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                       a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->flow_counter_bind)
        return SW_NOT_SUPPORTED;

    rv = p_api->flow_counter_bind(dev_id, entry_id, cnt_id, enable);
    return rv;
}

static sw_error_t
_fal_nat_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_status_set(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_nat_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_status_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_nat_hash_mode_set(a_uint32_t dev_id, a_uint32_t mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_hash_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_hash_mode_set(dev_id, mode);
    return rv;
}

static sw_error_t
_fal_nat_hash_mode_get(a_uint32_t dev_id, a_uint32_t * mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_hash_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_hash_mode_get(dev_id, mode);
    return rv;
}

static sw_error_t
_fal_napt_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->napt_status_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->napt_status_set(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_napt_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->napt_status_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->napt_status_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_napt_mode_set(a_uint32_t dev_id, fal_napt_mode_t mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->napt_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->napt_mode_set(dev_id, mode);
    return rv;
}

static sw_error_t
_fal_napt_mode_get(a_uint32_t dev_id, fal_napt_mode_t * mode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->napt_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->napt_mode_get(dev_id, mode);
    return rv;
}

static sw_error_t
_fal_nat_prv_base_addr_set(a_uint32_t dev_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_prv_base_addr_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_prv_base_addr_set(dev_id, addr);
    return rv;
}

static sw_error_t
_fal_nat_prv_base_addr_get(a_uint32_t dev_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_prv_base_addr_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_prv_base_addr_get(dev_id, addr);
    return rv;
}

static sw_error_t
_fal_nat_prv_base_mask_set(a_uint32_t dev_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_prv_base_mask_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_prv_base_mask_set(dev_id, addr);
    return rv;
}

static sw_error_t
_fal_nat_prv_base_mask_get(a_uint32_t dev_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_prv_base_mask_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_prv_base_mask_get(dev_id, addr);
    return rv;
}

static sw_error_t
_fal_nat_prv_addr_mode_set(a_uint32_t dev_id, a_bool_t map_en)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_prv_addr_mode_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_prv_addr_mode_set(dev_id, map_en);
    return rv;
}

static sw_error_t
_fal_nat_prv_addr_mode_get(a_uint32_t dev_id, a_bool_t * map_en)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_prv_addr_mode_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_prv_addr_mode_get(dev_id, map_en);
    return rv;
}

static sw_error_t
_fal_nat_pub_addr_add(a_uint32_t dev_id, fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_pub_addr_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_pub_addr_add(dev_id, entry);
    return rv;
}

static sw_error_t
_fal_nat_pub_addr_del(a_uint32_t dev_id, a_uint32_t del_mode,
                      fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_pub_addr_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_pub_addr_del(dev_id, del_mode, entry);
    return rv;
}

static sw_error_t
_fal_nat_pub_addr_next(a_uint32_t dev_id, a_uint32_t next_mode,
                       fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_pub_addr_next)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_pub_addr_next(dev_id, next_mode, entry);
    return rv;
}

static sw_error_t
_fal_nat_unk_session_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_unk_session_cmd_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_unk_session_cmd_set(dev_id, cmd);
    return rv;
}

static sw_error_t
_fal_nat_unk_session_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_unk_session_cmd_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_unk_session_cmd_get(dev_id, cmd);
    return rv;
}

static sw_error_t
_fal_nat_global_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t portbmp)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->nat_global_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->nat_global_set(dev_id, enable, portbmp);
    return rv;
}

/**
 * @brief Add one NAT entry to one particular device.
 *   @details Comments:
       Before NAT entry added ip4 private base address must be set
       at first.
       In parameter nat_entry entry flags must be set
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in] nat_entry NAT entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_add(a_uint32_t dev_id, fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_add(dev_id, nat_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Del NAT entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] del_mode NAT entry delete operation mode
 * @param[in] nat_entry NAT entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_del(a_uint32_t dev_id, a_uint32_t del_mode,
            fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_del(dev_id, del_mode, nat_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get one NAT entry from one particular device.
 * @param[in] dev_id device id
 * @param[in] get_mode NAT entry get operation mode
 * @param[in] nat_entry NAT entry parameter
 * @param[out] nat_entry NAT entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_get(a_uint32_t dev_id, a_uint32_t get_mode,
            fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_get(dev_id, get_mode, nat_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next NAT entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] next_mode NAT entry next operation mode
 * @param[in] nat_entry NAT entry parameter
 * @param[out] nat_entry NAT entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_next(a_uint32_t dev_id, a_uint32_t next_mode,
             fal_nat_entry_t * nat_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_next(dev_id, next_mode, nat_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Bind one counter entry to one NAT entry to one particular device.
 * @param[in] dev_id device id
 * @param[in] entry_id NAT entry id
 * @param[in] cnt_id counter entry id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id, a_uint32_t cnt_id,
                     a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_counter_bind(dev_id, entry_id, cnt_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add one NAPT entry to one particular device.
 *   @details Comments:
       Before NAPT entry added related ip4 private base address must be set
       at first.
       In parameter napt_entry related entry flags must be set
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in] napt_entry NAPT entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_napt_add(a_uint32_t dev_id, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_napt_add(dev_id, napt_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Del NAPT entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] del_mode NAPT entry delete operation mode
 * @param[in] napt_entry NAPT entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_napt_del(a_uint32_t dev_id, a_uint32_t del_mode,
             fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_napt_del(dev_id, del_mode, napt_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get one NAPT entry from one particular device.
 * @param[in] dev_id device id
 * @param[in] get_mode NAPT entry get operation mode
 * @param[in] nat_entry NAPT entry parameter
 * @param[out] nat_entry NAPT entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_napt_get(a_uint32_t dev_id, a_uint32_t get_mode,
             fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_napt_get(dev_id, get_mode, napt_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next NAPT entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] next_mode NAPT entry next operation mode
 * @param[in] napt_entry NAPT entry parameter
 * @param[out] napt_entry NAPT entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_napt_next(a_uint32_t dev_id, a_uint32_t next_mode,
              fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_napt_next(dev_id, next_mode, napt_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Bind one counter entry to one NAPT entry to one particular device.
 * @param[in] dev_id device id
 * @param[in] entry_id NAPT entry id
 * @param[in] cnt_id counter entry id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_napt_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                      a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_napt_counter_bind(dev_id, entry_id, cnt_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add one FLOW entry to one particular device.
 *   @details Comments:
       Before FLOW entry added related ip4 private base address must be set
       at first.
       In parameter napt_entry related entry flags must be set
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in] napt_entry FLOW entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_flow_add(a_uint32_t dev_id, fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_flow_add(dev_id, napt_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Del FLOW entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] del_mode NAPT entry delete operation mode
 * @param[in] napt_entry NAPT entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_flow_del(a_uint32_t dev_id, a_uint32_t del_mode,
             fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_flow_del(dev_id, del_mode, napt_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get one FLOW entry from one particular device.
 * @param[in] dev_id device id
 * @param[in] get_mode FLOW entry get operation mode
 * @param[in] nat_entry FLOW entry parameter
 * @param[out] nat_entry FLOW entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_flow_get(a_uint32_t dev_id, a_uint32_t get_mode,
             fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_flow_get(dev_id, get_mode, napt_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next FLOW entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] next_mode FLOW entry next operation mode
 * @param[in] napt_entry FLOW entry parameter
 * @param[out] napt_entry FLOW entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_flow_next(a_uint32_t dev_id, a_uint32_t next_mode,
              fal_napt_entry_t * napt_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_flow_next(dev_id, next_mode, napt_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Bind one counter entry to one FLOW entry to one particular device.
 * @param[in] dev_id device id
 * @param[in] entry_id FLOW entry id
 * @param[in] cnt_id counter entry id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_flow_counter_bind(a_uint32_t dev_id, a_uint32_t entry_id,
                      a_uint32_t cnt_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_flow_counter_bind(dev_id, entry_id, cnt_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set working status of NAT engine on a particular device
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_status_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get working status of NAT engine on a particular device
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_status_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set NAT hash mode on a particular device
 * @param[in] dev_id device id
 * @param[in] mode NAT hash mode
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_hash_mode_set(a_uint32_t dev_id, a_uint32_t mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_hash_mode_set(dev_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get NAT hash mode on a particular device
 * @param[in] dev_id device id
 * @param[out] mode NAT hash mode
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_hash_mode_get(a_uint32_t dev_id, a_uint32_t * mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_hash_mode_get(dev_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set working status of NAPT engine on a particular device
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_napt_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_napt_status_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get working status of NAPT engine on a particular device
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_napt_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_napt_status_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set working mode of NAPT engine on a particular device
 * @param[in] dev_id device id
 * @param[in] mode NAPT mode
 * @return SW_OK or error code
 */
sw_error_t
fal_napt_mode_set(a_uint32_t dev_id, fal_napt_mode_t mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_napt_mode_set(dev_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get working mode of NAPT engine on a particular device
 * @param[in] dev_id device id
 * @param[out] mode NAPT mode
 * @return SW_OK or error code
 */
sw_error_t
fal_napt_mode_get(a_uint32_t dev_id, fal_napt_mode_t * mode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_napt_mode_get(dev_id, mode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP4 private base address on a particular device
 *   @details Comments:
        Only 20bits is meaning which 20bits is determined by private address mode.
 * @param[in] dev_id device id
 * @param[in] addr private base address
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_prv_base_addr_set(a_uint32_t dev_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_prv_base_addr_set(dev_id, addr);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP4 private base address on a particular device
 * @param[in] dev_id device id
 * @param[out] addr private base address
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_prv_base_addr_get(a_uint32_t dev_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_prv_base_addr_get(dev_id, addr);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_nat_prv_base_mask_set(a_uint32_t dev_id, fal_ip4_addr_t addr)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_prv_base_mask_set(dev_id, addr);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_nat_prv_base_mask_get(a_uint32_t dev_id, fal_ip4_addr_t * addr)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_prv_base_mask_get(dev_id, addr);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set IP4 private base address mode on a particular device
 *   @details Comments:
        If map_en equal true means bits31-20 bits15-8 are base address
        else bits31-12 are base address.
 * @param[in] dev_id device id
 * @param[in] map_en private base mapping mode
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_prv_addr_mode_set(a_uint32_t dev_id, a_bool_t map_en)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_prv_addr_mode_set(dev_id, map_en);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get IP4 private base address mode on a particular device
 * @param[in] dev_id device id
 * @param[out] map_en private base mapping mode
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_prv_addr_mode_get(a_uint32_t dev_id, a_bool_t * map_en)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_prv_addr_mode_get(dev_id, map_en);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add one public address entry to one particular device.
 *   @details Comments:
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in] entry public address entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_pub_addr_add(a_uint32_t dev_id, fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_pub_addr_add(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete one public address entry from one particular device.
 * @param[in] dev_id device id
 * @param[in] del_mode delete operaton mode
 * @param[in] entry public address entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_pub_addr_del(a_uint32_t dev_id, a_uint32_t del_mode,
                     fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_pub_addr_del(dev_id, del_mode, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next public address entries from one particular device.
 * @param[in] dev_id device id
 * @param[in] next_mode next operaton mode
 * @param[out] entry public address entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_pub_addr_next(a_uint32_t dev_id, a_uint32_t next_mode,
                      fal_nat_pub_addr_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_pub_addr_next(dev_id, next_mode, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set forwarding command for those packets miss NAT entries on a particular device.
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_unk_session_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_unk_session_cmd_set(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get forwarding command for those packets miss NAT entries on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_unk_session_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_nat_unk_session_cmd_get(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set working status of NAPT engine on a particular device
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] sync_cnt_enable A_TRUE or A_FALSE
 * @param[in] portbmp port bitmap
 * @return SW_OK or error code
 */
sw_error_t
fal_nat_global_set(a_uint32_t dev_id, a_bool_t enable,
		a_bool_t sync_cnt_enable, a_uint32_t portbmp)
{
    sw_error_t rv;

    FAL_API_LOCK;
	nf_athrs17_hnat_sync_counter_en = (int)sync_cnt_enable;
    rv = _fal_nat_global_set(dev_id, enable, portbmp);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add/del one FLOW cookie entry to one particular device.
 *   @details Comments:
       Before FLOW entry added related ip4 private base address must be set
       at first.
       In parameter napt_entry related entry flags must be set
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in] napt_entry FLOW entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_flow_cookie_set(a_uint32_t dev_id, fal_flow_cookie_t * flow_cookie)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_flow_cookie_set(dev_id, flow_cookie);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add/del one FLOW rfs entry to one particular device.
 *   @details Comments:
       Before FLOW entry added related ip4 private base address must be set
       at first.
       In parameter napt_entry related entry flags must be set
       Hardware entry id will be returned.
 * @param[in] dev_id device id
 * @param[in] napt_entry FLOW entry parameter
 * @return SW_OK or error code
 */
sw_error_t
fal_flow_rfs_set(a_uint32_t dev_id, a_uint8_t action, fal_flow_rfs_t * rfs)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_flow_rfs_set(dev_id, action, rfs);
    FAL_API_UNLOCK;
    return rv;
}


int ssdk_flow_cookie_set(
		u32 protocol, __be32 src_ip,
		__be16 src_port, __be32 dst_ip,
		__be16 dst_port, u16 flowcookie)
{
	fal_flow_cookie_t flow_cookie;
	if(protocol == 17) {
		flow_cookie.proto = 0x2;
	} else {
		flow_cookie.proto = 0x1;
	}
	flow_cookie.src_addr = ntohl(src_ip);
	flow_cookie.dst_addr = ntohl(dst_ip);
	flow_cookie.src_port = ntohs(src_port);
	flow_cookie.dst_port = ntohs(dst_port);
	flow_cookie.flow_cookie = flowcookie;
	return fal_flow_cookie_set(0, &flow_cookie);
}

int ssdk_rfs_ipct_rule_set(
	__be32 ip_src, __be32 ip_dst,
	__be16 sport, __be16 dport, uint8_t proto,
	u16 loadbalance, bool action)
{
	fal_flow_rfs_t rfs;
	if(proto == 17) {
		rfs.proto = 0x2;
	} else {
		rfs.proto = 0x1;
	}
	rfs.src_addr = ntohl(ip_src);
	rfs.dst_addr = ntohl(ip_dst);
	rfs.src_port = ntohs(sport);
	rfs.dst_port = ntohs(dport);
	rfs.load_balance = loadbalance;
	if(fal_flow_rfs_set(0, action, &rfs))
		return -1;
	return 0;
}

#if 0
EXPORT_SYMBOL(ssdk_flow_cookie_set);
EXPORT_SYMBOL(ssdk_rfs_ipct_rule_set);
#endif

/**
 * @}
 */
