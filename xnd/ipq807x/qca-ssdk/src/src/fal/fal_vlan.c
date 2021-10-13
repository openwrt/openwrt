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



/**
 * @defgroup fal_vlan FAL_VLAN
 * @{
 */

#include "sw.h"
#include "util.h"
#include "fal_vlan.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_api.h"

typedef struct
{
    a_uint32_t   idx;
    fal_vlan_t   entry;
} v_array_t;

static v_array_t  * vlan_db[SW_MAX_NR_DEV] = { 0 };
static sid_pool_t * vlan_pool[SW_MAX_NR_DEV]  = { 0 };
static sll_head_t * vlan_list[SW_MAX_NR_DEV] = { 0 };


static sw_error_t
_fal_vlan_search(a_uint32_t dev_id, fal_vlan_t * vlan_entry)
{
    a_ulong_t iterator;
    v_array_t   v_tbl;
    v_array_t * p_tbl;
    hsl_dev_t * p_dev = NULL;

    p_dev = hsl_dev_ptr_get(dev_id);
    SW_RTN_ON_NULL(p_dev);

    if (A_TRUE == p_dev->hw_vlan_query)
    {
        return SW_NOT_FOUND;
    }

    v_tbl.entry = * vlan_entry;
    p_tbl = sll_nd_find(vlan_list[dev_id], &v_tbl, &iterator);

    if (NULL == p_tbl)
    {
        return SW_NOT_FOUND;
    }
    else
    {
        * vlan_entry = p_tbl->entry;
        return SW_OK;
    }
}

static sw_error_t
_fal_vlan_following(a_uint32_t dev_id, fal_vlan_t * vlan_entry)
{
    a_ulong_t iterator = 0;
    v_array_t   v_tbl;
    v_array_t * p_tbl;
    hsl_dev_t * p_dev = NULL;

    p_dev = hsl_dev_ptr_get(dev_id);
    SW_RTN_ON_NULL(p_dev);

    if (A_TRUE == p_dev->hw_vlan_query)
    {
        return SW_OK;
    }

    sll_lock(vlan_list[dev_id]);

    v_tbl.entry = *vlan_entry;

    if (0 == v_tbl.entry.vid)
    {
        p_tbl = sll_nd_next(vlan_list[dev_id], &iterator);
    }
    else
    {
        p_tbl = sll_nd_find(vlan_list[dev_id], &v_tbl, &iterator);
        if (NULL == p_tbl)
        {
            sll_unlock(vlan_list[dev_id]);
            return SW_NO_MORE;
        }

        p_tbl = sll_nd_next(vlan_list[dev_id], &iterator);
    }

    if (NULL == p_tbl)
    {
        sll_unlock(vlan_list[dev_id]);
        return SW_NO_MORE;
    }

    * vlan_entry = p_tbl->entry;
    sll_unlock(vlan_list[dev_id]);
    return SW_OK;
}

static sw_error_t
_fal_vlan_del(a_uint32_t dev_id, a_uint16_t vlan_id)
{
    v_array_t * p_tbl;
    v_array_t   ent;
    sw_error_t  rv;
    a_ulong_t iterator;
    a_uint32_t  id;
    hsl_dev_t * p_dev = NULL;

    p_dev = hsl_dev_ptr_get(dev_id);
    SW_RTN_ON_NULL(p_dev);

    if (A_TRUE == p_dev->hw_vlan_query)
    {
        return SW_OK;
    }

    ent.entry.vid = vlan_id;
    p_tbl = sll_nd_find(vlan_list[dev_id], &ent, &iterator);
    if (NULL == p_tbl)
    {
        return SW_NOT_FOUND;
    }
    id = p_tbl->idx;

    rv = sll_nd_delete(vlan_list[dev_id], p_tbl);
    SW_RTN_ON_ERROR(rv);

    rv = sid_pool_id_free(vlan_pool[dev_id], id);
    return rv;
}

static sw_error_t
_fal_vlan_creat(a_uint32_t dev_id, const fal_vlan_t * vlan_entry)
{
    v_array_t * v_tbl;
    sw_error_t  rv;
    a_uint32_t  id;
    hsl_dev_t * p_dev = NULL;

    p_dev = hsl_dev_ptr_get(dev_id);
    SW_RTN_ON_NULL(p_dev);

    if (A_TRUE == p_dev->hw_vlan_query)
    {
        return SW_OK;
    }

    rv = sid_pool_id_alloc(vlan_pool[dev_id], &id);
    SW_RTN_ON_ERROR(rv);

    v_tbl = &vlan_db[dev_id][id];
    v_tbl->idx   = id;
    v_tbl->entry = *vlan_entry;
    rv = sll_nd_insert(vlan_list[dev_id], v_tbl);
    return rv;
}

static sw_error_t
_fal_vlan_update(a_uint32_t dev_id, const fal_vlan_t * vlan_entry)
{
    a_ulong_t iterator;
    v_array_t   v_tbl;
    v_array_t * p_tbl;
    hsl_dev_t * p_dev = NULL;

    p_dev = hsl_dev_ptr_get(dev_id);
    SW_RTN_ON_NULL(p_dev);

    if (A_TRUE == p_dev->hw_vlan_query)
    {
        return SW_OK;
    }

    sll_lock(vlan_list[dev_id]);
    v_tbl.entry = *vlan_entry;
    p_tbl = sll_nd_find(vlan_list[dev_id], &v_tbl, &iterator);

    if (NULL == p_tbl)
    {
        sll_unlock(vlan_list[dev_id]);
        return SW_NOT_FOUND;
    }

    p_tbl->entry = * vlan_entry;
    sll_unlock(vlan_list[dev_id]);
    return SW_OK;
}

static ll_cmp_rslt_t
_fal_vlan_entry_cmp(void * src, void * dest)
{
    v_array_t * src_nd, * dest_nd;

    src_nd  = (v_array_t*)src;
    dest_nd = (v_array_t*)dest;

    if (src_nd->entry.vid == dest_nd->entry.vid)
    {
        return LL_CMP_EQUAL;
    }
    else if (src_nd->entry.vid > dest_nd->entry.vid)
    {
        return LL_CMP_GREATER;
    }
    else
    {
        return LL_CMP_SMALLER;
    }
}

static void
_fal_vlan_entry_dump(void * data)
{
    v_array_t * nd;

    nd  = (v_array_t*)data;
    aos_printk("vid = %d  member = 0x%x\n", nd->entry.vid, nd->entry.mem_ports);
}


static sw_error_t
_fal_vlan_entry_append(a_uint32_t dev_id, fal_vlan_t * vlan_entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    rv = _fal_vlan_search(dev_id, vlan_entry);
    if (SW_OK == rv)
    {
        return SW_ALREADY_EXIST;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
    if (NULL == p_api->vlan_entry_append)
        return SW_NOT_SUPPORTED;

    rv = p_api->vlan_entry_append(dev_id, vlan_entry);
    SW_RTN_ON_ERROR(rv);

    rv = _fal_vlan_creat(dev_id, vlan_entry);
    return rv;
}


static sw_error_t
_fal_vlan_create(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;
    fal_vlan_t entry;
    hsl_api_t *p_api;

    aos_mem_zero(&(entry), sizeof(fal_vlan_t));
    entry.vid = vlan_id;
    rv = _fal_vlan_search(dev_id, &entry);
    if (SW_OK == rv)
    {
        return SW_ALREADY_EXIST;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
    if (NULL == p_api->vlan_creat)
        return SW_NOT_SUPPORTED;

    rv = p_api->vlan_creat(dev_id, vlan_id);
    SW_RTN_ON_ERROR(rv);

    rv = _fal_vlan_creat(dev_id, &entry);
    return rv;
}


static sw_error_t
_fal_vlan_next(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->vlan_next)
    {
        p_vlan->vid = vlan_id;
        rv = _fal_vlan_following(dev_id, p_vlan);
    }
    else
    {
        rv = p_api->vlan_next(dev_id, vlan_id, p_vlan);
    }
    return rv;
}


static sw_error_t
_fal_vlan_find(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->vlan_find)
    {
        p_vlan->vid = vlan_id;
        rv = _fal_vlan_search(dev_id, p_vlan);
    }
    else
    {
        rv = p_api->vlan_find(dev_id, vlan_id, p_vlan);
    }
    return rv;
}


static sw_error_t
_fal_vlan_member_update(a_uint32_t dev_id, a_uint32_t vlan_id,
                        fal_pbmp_t member, fal_pbmp_t u_member)
{
    sw_error_t rv;
    fal_vlan_t vlan_entry = {0};
    hsl_api_t *p_api;
    hsl_dev_t *p_dev = NULL;

    p_dev = hsl_dev_ptr_get(dev_id);
    SW_RTN_ON_NULL(p_dev);

    if (A_FALSE == p_dev->hw_vlan_query)
    {
        vlan_entry.vid = vlan_id;
        rv = _fal_vlan_search(dev_id, &vlan_entry);
        SW_RTN_ON_ERROR(rv);
    }

    vlan_entry.mem_ports = member;
    vlan_entry.u_ports   = u_member;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
    if (NULL == p_api->vlan_member_update)
    {
        if ((NULL == p_api->vlan_entry_append)
                || (NULL == p_api->vlan_delete))
        {
            return SW_NOT_SUPPORTED;
        }

        p_api->vlan_delete(dev_id, vlan_id);

	vlan_entry.vid = vlan_id;
	vlan_entry.fid = vlan_id;

	vlan_entry.untagged_ports = u_member;
	vlan_entry.tagged_ports = member & ~u_member;

        rv = p_api->vlan_entry_append(dev_id, &vlan_entry);
        SW_RTN_ON_ERROR(rv);
    }
    else
    {

        rv = p_api->vlan_member_update(dev_id, vlan_id, member, u_member);
        SW_RTN_ON_ERROR(rv);
    }
    rv = _fal_vlan_update(dev_id, &vlan_entry);
    return rv;
}


static sw_error_t
_fal_vlan_delete(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->vlan_delete)
        return SW_NOT_SUPPORTED;

    rv = p_api->vlan_delete(dev_id, vlan_id);
    SW_RTN_ON_ERROR(rv);

    rv = _fal_vlan_del(dev_id, vlan_id);
    return rv;
}

static sw_error_t
_fal_vlan_flush(a_uint32_t dev_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->vlan_flush)
        return SW_NOT_SUPPORTED;

    rv = p_api->vlan_flush(dev_id);
    SW_RTN_ON_ERROR(rv);

    rv = fal_vlan_reset(dev_id);
    return rv;
}

/**
 * @brief Set FID of a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] fid FDB id
 * @return SW_OK or error code
 */
static sw_error_t
_fal_vlan_fid_set(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t fid)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->vlan_fid_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->vlan_fid_set(dev_id, vlan_id, fid);
    return rv;
}

/**
 * @brief Get FID of a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[out] fid FDB id
 * @return SW_OK or error code
 */
static sw_error_t
_fal_vlan_fid_get(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t * fid)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->vlan_fid_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->vlan_fid_get(dev_id, vlan_id, fid);
    return rv;
}

/**
 * @brief Add a port member to a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] port_id port id
 * @param[in] port_info port tag information
 * @return SW_OK or error code
 */
static sw_error_t
_fal_vlan_member_add(a_uint32_t dev_id, a_uint32_t vlan_id,
                     fal_port_t port_id, fal_pt_1q_egmode_t port_info)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->vlan_member_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->vlan_member_add(dev_id, vlan_id, port_id, port_info);
    return rv;
}

/**
 * @brief Del a port member from a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
static sw_error_t
_fal_vlan_member_del(a_uint32_t dev_id, a_uint32_t vlan_id, fal_port_t port_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->vlan_member_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->vlan_member_del(dev_id, vlan_id, port_id);
    return rv;
}

/**
 * @brief Set FDB learning status of a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
static sw_error_t
_fal_vlan_learning_state_set(a_uint32_t dev_id, a_uint32_t vlan_id,
                             a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->vlan_learning_state_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->vlan_learning_state_set(dev_id, vlan_id, enable);
    return rv;
}

/**
 * @brief Get FDB learning status of a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
static sw_error_t
_fal_vlan_learning_state_get(a_uint32_t dev_id, a_uint32_t vlan_id,
                             a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->vlan_learning_state_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->vlan_learning_state_get(dev_id, vlan_id, enable);
    return rv;
}

/**
 * @brief Reset fal vlan module on a paticular device.
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_reset(a_uint32_t dev_id)
{
    a_uint32_t entry_nr;
    hsl_dev_t *p_dev = NULL;

    p_dev = hsl_dev_ptr_get(dev_id);
    SW_RTN_ON_NULL(p_dev);

    if (A_TRUE == p_dev->hw_vlan_query)
    {
        return SW_OK;
    }

    entry_nr = p_dev->nr_vlans;
    if ((0 == entry_nr) || (4096 < entry_nr))
    {
        return SW_FAIL;
    }

    if (NULL != vlan_pool[dev_id])
    {
        sid_pool_destroy(vlan_pool[dev_id]);
        vlan_pool[dev_id] = NULL;
    }

    if (NULL != vlan_list[dev_id])
    {
        sll_destroy(vlan_list[dev_id]);
        vlan_list[dev_id] = NULL;
    }

    aos_mem_zero(vlan_db[dev_id], entry_nr * (sizeof (v_array_t)));

    vlan_pool[dev_id] = sid_pool_creat(entry_nr, 0);
    if (NULL == vlan_pool[dev_id])
    {
        return SW_FAIL;
    }

    vlan_list[dev_id] = sll_creat(_fal_vlan_entry_cmp, _fal_vlan_entry_dump,
                                  LL_FIX_NDNR | LL_IN_ORDER, entry_nr);
    if (NULL == vlan_list[dev_id])
    {
        return SW_FAIL;
    }

    return SW_OK;
}


/**
 * @brief Init fal vlan module on a paticular device.
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_init(a_uint32_t dev_id)
{
    a_uint32_t entry_nr;
    hsl_dev_t *p_dev = NULL;

    p_dev = hsl_dev_ptr_get(dev_id);
    SW_RTN_ON_NULL(p_dev);

    if (A_TRUE == p_dev->hw_vlan_query)
    {
        return SW_OK;
    }

    entry_nr = p_dev->nr_vlans;
    if ((0 == entry_nr) || (4096 < entry_nr))
    {
        return SW_FAIL;
    }

    vlan_pool[dev_id] = sid_pool_creat(entry_nr, 0);
    if (NULL == vlan_pool[dev_id])
    {
        return SW_FAIL;
    }

    /* allocate memory for vlan entry */
    vlan_db[dev_id] = aos_mem_alloc(entry_nr * (sizeof (v_array_t)));
    if (NULL == vlan_db[dev_id])
    {
        return SW_OUT_OF_MEM;
    }
    aos_mem_zero(vlan_db[dev_id], entry_nr * (sizeof (v_array_t)));

    vlan_list[dev_id] = sll_creat(_fal_vlan_entry_cmp, _fal_vlan_entry_dump,
                                  LL_FIX_NDNR | LL_IN_ORDER, entry_nr);

    if (NULL == vlan_list[dev_id])
    {
        return SW_FAIL;
    }

    return SW_OK;
}

sw_error_t
fal_vlan_cleanup(void)
{
    a_uint32_t dev_id;

    for (dev_id = 0; dev_id < SW_MAX_NR_DEV; dev_id++)
    {
        if (vlan_db[dev_id])
        {
            aos_mem_free(vlan_db[dev_id]);
            vlan_db[dev_id] = NULL;
        }

        if (vlan_pool[dev_id])
        {
            sid_pool_destroy(vlan_pool[dev_id]);
            vlan_pool[dev_id] = NULL;
        }

        if (vlan_list[dev_id])
        {
            sll_destroy(vlan_list[dev_id]);
            vlan_list[dev_id] = NULL;
        }
    }

    return SW_OK;
}

/**
 * @brief Append a vlan entry on paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_entry vlan entry
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_entry_append(a_uint32_t dev_id, fal_vlan_t * vlan_entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_entry_append(dev_id, vlan_entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Creat a vlan entry through vlan id on a paticular device.
 *   @details   Comments:
 *    After this operation the member ports of the created vlan entry are null.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_create(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_create(dev_id, vlan_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Next a vlan entry through vlan id on a paticular device.
 *   @details   Comments:
 *    If the value of vid is zero this operation will get the first entry.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[out] p_vlan vlan entry
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_next(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_next(dev_id, vlan_id, p_vlan);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Find a vlan entry through vlan id on paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[out] p_vlan vlan entry
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_find(a_uint32_t dev_id, a_uint32_t vlan_id, fal_vlan_t * p_vlan)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_find(dev_id, vlan_id, p_vlan);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Update a vlan entry member port through vlan id on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] member member ports
 * @param[in] u_member tagged or untagged infomation for member ports
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_member_update(a_uint32_t dev_id, a_uint32_t vlan_id,
                       fal_pbmp_t member, fal_pbmp_t u_member)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_member_update(dev_id, vlan_id, member, u_member);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete a vlan entry through vlan id on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_delete(a_uint32_t dev_id, a_uint32_t vlan_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_delete(dev_id, vlan_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Flush all vlan entries on a paticular device.
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_flush(a_uint32_t dev_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_flush(dev_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set FID of a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] fid FDB id
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_fid_set(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t fid)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_fid_set(dev_id, vlan_id, fid);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get FID of a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[out] fid FDB id
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_fid_get(a_uint32_t dev_id, a_uint32_t vlan_id, a_uint32_t * fid)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_fid_get(dev_id, vlan_id, fid);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add a port member to a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] port_id port id
 * @param[in] port_info port tag information
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_member_add(a_uint32_t dev_id, a_uint32_t vlan_id,
                    fal_port_t port_id, fal_pt_1q_egmode_t port_info)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_member_add(dev_id, vlan_id, port_id, port_info);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Del a port member from a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_member_del(a_uint32_t dev_id, a_uint32_t vlan_id, fal_port_t port_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_member_del(dev_id, vlan_id, port_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set FDB learning status of a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_learning_state_set(a_uint32_t dev_id, a_uint32_t vlan_id,
                            a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_learning_state_set(dev_id, vlan_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get FDB learning status of a paticular vlan entry on a paticular device.
 * @param[in] dev_id device id
 * @param[in] vlan_id vlan id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_vlan_learning_state_get(a_uint32_t dev_id, a_uint32_t vlan_id,
                            a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_vlan_learning_state_get(dev_id, vlan_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @}
 */
