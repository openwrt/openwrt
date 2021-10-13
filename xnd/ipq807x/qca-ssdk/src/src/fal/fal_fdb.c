/*
 * Copyright (c) 2012, 2015-2018, The Linux Foundation. All rights reserved.
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
 * @defgroup fal_fdb FAL_FDB
 * @{
 */
#include "sw.h"
#include "fal_fdb.h"
#include "hsl_api.h"
#include "adpt.h"

#include <linux/kernel.h>
#include <linux/module.h>


#ifndef IN_FDB_MINI
static sw_error_t
_fal_fdb_entry_add(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_add)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_add(dev_id, entry);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_add(dev_id, entry);
    return rv;
}
#endif

static sw_error_t
_fal_fdb_entry_flush(a_uint32_t dev_id, a_uint32_t flag)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_del_all)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_del_all(dev_id, flag);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_del_all)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_del_all(dev_id, flag);
    return rv;
}

static sw_error_t
_fal_fdb_entry_del_byport(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t flag)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_del_by_port)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_del_by_port(dev_id, port_id, flag);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_del_by_port)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_del_by_port(dev_id, port_id, flag);
    return rv;
}


static sw_error_t
_fal_fdb_entry_del_bymac(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_del_by_mac)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_del_by_mac(dev_id, entry);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_del_by_mac)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_del_by_mac(dev_id, entry);
    return rv;
}

static sw_error_t
_fal_fdb_entry_getfirst(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_first)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_first(dev_id, entry);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_first)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_first(dev_id, entry);
    return rv;
}

#ifndef IN_FDB_MINI
static sw_error_t
_fal_fdb_entry_getnext(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_next)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_next(dev_id, entry);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_next)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_next(dev_id, entry);
    return rv;
}
#endif

static sw_error_t
_fal_fdb_entry_search(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_find)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_find(dev_id, entry);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_find)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_find(dev_id, entry);
    return rv;
}

static sw_error_t
_fal_fdb_port_learn_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_port_learn_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_port_learn_set(dev_id, port_id, enable);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_learn_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_learn_set(dev_id, port_id, enable);
    return rv;
}

#ifndef IN_FDB_MINI
static sw_error_t
_fal_fdb_port_learn_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_port_learn_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_port_learn_get(dev_id, port_id, enable);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_learn_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_learn_get(dev_id, port_id, enable);
    return rv;
}
#endif

static sw_error_t
_fal_fdb_port_learning_ctrl_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable, fal_fwd_cmd_t cmd)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_fdb_port_newaddr_lrn_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_fdb_port_newaddr_lrn_set(dev_id, port_id, enable, cmd);
    return rv;
}

#ifndef IN_FDB_MINI
static sw_error_t
_fal_fdb_port_learning_ctrl_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable, fal_fwd_cmd_t *cmd)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_fdb_port_newaddr_lrn_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_fdb_port_newaddr_lrn_get(dev_id, port_id, enable, cmd);
    return rv;
}
#endif

static sw_error_t
_fal_fdb_port_stamove_ctrl_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable, fal_fwd_cmd_t cmd)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_fdb_port_stamove_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_fdb_port_stamove_set(dev_id, port_id, enable, cmd);
    return rv;
}

#ifndef IN_FDB_MINI
static sw_error_t
_fal_fdb_port_stamove_ctrl_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable, fal_fwd_cmd_t *cmd)
{
    adpt_api_t *p_api;
    sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_fdb_port_stamove_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_fdb_port_stamove_get(dev_id, port_id, enable, cmd);
    return rv;
}
#endif

static sw_error_t
_fal_fdb_aging_ctrl_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_age_ctrl_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_age_ctrl_set(dev_id, enable);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->age_ctrl_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->age_ctrl_set(dev_id, enable);
    return rv;
}

#ifndef IN_FDB_MINI
static sw_error_t
_fal_fdb_aging_ctrl_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_age_ctrl_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_age_ctrl_get(dev_id, enable);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->age_ctrl_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->age_ctrl_get(dev_id, enable);
    return rv;
}

static sw_error_t
_fal_fdb_vlan_ivl_svl_set(a_uint32_t dev_id, fal_fdb_smode smode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->vlan_ivl_svl_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->vlan_ivl_svl_set(dev_id, smode);
    return rv;
}

static sw_error_t
_fal_fdb_vlan_ivl_svl_get(a_uint32_t dev_id, fal_fdb_smode* smode)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->vlan_ivl_svl_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->vlan_ivl_svl_get(dev_id, smode);
    return rv;
}

static sw_error_t
_fal_fdb_aging_time_set(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_age_time_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_age_time_set(dev_id, time);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->age_time_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->age_time_set(dev_id, time);
    return rv;
}


static sw_error_t
_fal_fdb_aging_time_get(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_age_time_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_age_time_get(dev_id, time);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->age_time_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->age_time_get(dev_id, time);
    return rv;
}
#endif

static sw_error_t
_fal_fdb_entry_getnext_byindex(a_uint32_t dev_id, a_uint32_t * iterator, fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_iterate)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_iterate(dev_id, iterator, entry);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_iterate)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_iterate(dev_id, iterator, entry);
    return rv;
}

static sw_error_t
_fal_fdb_entry_extend_getnext(a_uint32_t dev_id, fal_fdb_op_t * option,
                     fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_extend_next)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_extend_next(dev_id, option, entry);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_extend_next)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_extend_next(dev_id, option, entry);
    return rv;
}

static sw_error_t
_fal_fdb_entry_extend_getfirst(a_uint32_t dev_id, fal_fdb_op_t * option,
                      fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_extend_first)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_extend_first(dev_id, option, entry);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_extend_first)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_extend_first(dev_id, option, entry);
    return rv;
}

#ifndef IN_FDB_MINI
static sw_error_t
_fal_fdb_entry_update_byport(a_uint32_t dev_id, fal_port_t old_port, fal_port_t new_port,
                  a_uint32_t fid, fal_fdb_op_t * option)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_transfer)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_transfer(dev_id, old_port, new_port, fid, option);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_transfer)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_transfer(dev_id, old_port, new_port, fid, option);
    return rv;
}

static sw_error_t
_fal_port_fdb_learn_limit_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t enable, a_uint32_t cnt)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_fdb_learn_limit_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_fdb_learn_limit_set(dev_id, port_id, enable, cnt);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_fdb_learn_limit_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_fdb_learn_limit_set(dev_id, port_id, enable, cnt);
    return rv;
}

static sw_error_t
_fal_port_fdb_learn_limit_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t * enable, a_uint32_t * cnt)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_fdb_learn_limit_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_fdb_learn_limit_get(dev_id, port_id, enable, cnt);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_fdb_learn_limit_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_fdb_learn_limit_get(dev_id, port_id, enable, cnt);
    return rv;
}

static sw_error_t
_fal_port_fdb_learn_exceed_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                                   fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_fdb_learn_exceed_cmd_set)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_fdb_learn_exceed_cmd_set(dev_id, port_id, cmd);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_fdb_learn_exceed_cmd_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_fdb_learn_exceed_cmd_set(dev_id, port_id, cmd);
    return rv;
}

static sw_error_t
_fal_port_fdb_learn_exceed_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                                   fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_port_fdb_learn_exceed_cmd_get)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_port_fdb_learn_exceed_cmd_get(dev_id, port_id, cmd);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->port_fdb_learn_exceed_cmd_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->port_fdb_learn_exceed_cmd_get(dev_id, port_id, cmd);
    return rv;
}

static sw_error_t
_fal_fdb_learn_limit_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t cnt)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_learn_limit_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_learn_limit_set(dev_id, enable, cnt);
    return rv;
}

static sw_error_t
_fal_fdb_learn_limit_get(a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * cnt)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_learn_limit_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_learn_limit_get(dev_id, enable, cnt);
    return rv;
}

static sw_error_t
_fal_fdb_learn_exceed_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_learn_exceed_cmd_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_learn_exceed_cmd_set(dev_id, cmd);
    return rv;
}

static sw_error_t
_fal_fdb_learn_exceed_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_learn_exceed_cmd_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_learn_exceed_cmd_get(dev_id, cmd);
    return rv;
}

static sw_error_t
_fal_fdb_resv_add(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_resv_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_resv_add(dev_id, entry);
    return rv;
}

static sw_error_t
_fal_fdb_resv_del(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_resv_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_resv_del(dev_id, entry);
    return rv;
}

static sw_error_t
_fal_fdb_resv_find(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_resv_find)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_resv_find(dev_id, entry);
    return rv;
}

static sw_error_t
_fal_fdb_resv_iterate(a_uint32_t dev_id, a_uint32_t * iterator,
                      fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_resv_iterate)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_resv_iterate(dev_id, iterator, entry);
    return rv;
}

static sw_error_t
_fal_fdb_port_learn_static_set(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_port_learn_static_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_port_learn_static_set(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_fdb_port_learn_static_get(a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * enable)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_port_learn_static_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_port_learn_static_get(dev_id, port_id, enable);
    return rv;
}

static sw_error_t
_fal_fdb_port_add(a_uint32_t dev_id, a_uint32_t fid, fal_mac_addr_t * addr, fal_port_t port_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_port_add)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_port_add(dev_id, fid, addr, port_id);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_port_add)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_port_add(dev_id, fid, addr, port_id);
    return rv;
}

static sw_error_t
_fal_fdb_port_del(a_uint32_t dev_id, a_uint32_t fid, fal_mac_addr_t * addr, fal_port_t port_id)
{
    sw_error_t rv;
    hsl_api_t *p_api;
    adpt_api_t *p_adpt_api;

    if((p_adpt_api = adpt_api_ptr_get(dev_id)) != NULL) {
        if (NULL == p_adpt_api->adpt_fdb_port_del)
            return SW_NOT_SUPPORTED;

        rv = p_adpt_api->adpt_fdb_port_del(dev_id, fid, addr, port_id);
        return rv;
    }

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->fdb_port_del)
        return SW_NOT_SUPPORTED;

    rv = p_api->fdb_port_del(dev_id, fid, addr, port_id);
    return rv;
}

sw_error_t
_fal_fdb_rfs_set(a_uint32_t dev_id, const fal_fdb_rfs_t * entry)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->fdb_rfs_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->fdb_rfs_set(dev_id, entry);
	return rv;
}

sw_error_t
_fal_fdb_rfs_del(a_uint32_t dev_id, const fal_fdb_rfs_t * entry)
{
	sw_error_t rv;
	hsl_api_t *p_api;

	SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	if (NULL == p_api->fdb_rfs_del)
		return SW_NOT_SUPPORTED;

	rv = p_api->fdb_rfs_del(dev_id, entry);
	return rv;
}
#endif

sw_error_t
_fal_fdb_learning_ctrl_set(a_uint32_t dev_id, a_bool_t enable)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_fdb_learn_ctrl_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_fdb_learn_ctrl_set(dev_id, enable);
    return rv;
}

#ifndef IN_FDB_MINI
sw_error_t
_fal_fdb_learning_ctrl_get(a_uint32_t dev_id, a_bool_t * enable)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_fdb_learn_ctrl_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_fdb_learn_ctrl_get(dev_id, enable);
    return rv;
}
sw_error_t
_fal_fdb_port_learned_mac_counter_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t * cnt)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_port_fdb_learn_counter_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_port_fdb_learn_counter_get(dev_id, port_id, cnt);
    return rv;
}
sw_error_t
_fal_fdb_port_maclimit_ctrl_set(a_uint32_t dev_id, fal_port_t port_id, fal_maclimit_ctrl_t * maclimit_ctrl)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_fdb_port_maclimit_ctrl_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_fdb_port_maclimit_ctrl_set(dev_id, port_id, maclimit_ctrl);
    return rv;
}
sw_error_t
_fal_fdb_port_maclimit_ctrl_get(a_uint32_t dev_id, fal_port_t port_id, fal_maclimit_ctrl_t * maclimit_ctrl)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_fdb_port_maclimit_ctrl_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_fdb_port_maclimit_ctrl_get(dev_id, port_id, maclimit_ctrl);
    return rv;
}
#endif
sw_error_t
_fal_fdb_entry_del_byfid(a_uint32_t dev_id, a_uint16_t fid, a_uint32_t flag)
{
    adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

    SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

    if (NULL == p_api->adpt_fdb_del_by_fid)
        return SW_NOT_SUPPORTED;

    rv = p_api->adpt_fdb_del_by_fid(dev_id, fid, flag);
    return rv;
}
#ifndef IN_FDB_MINI
/*insert flag for inner fal, don't remove it*/

/**
 * @brief Add a Fdb entry
 * @param[in] dev_id device id
 * @param[in] entry fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_entry_add(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_entry_add(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}
#endif

/**
 * @brief Delete all Fdb entries
 *   @details   Comments:
 *         If set FAL_FDB_DEL_STATIC bit in flag which means delete all fdb
 *       entries otherwise only delete dynamic entries.
 * @param[in] dev_id device id
 * @param[in] flag delete operation option
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_entry_flush(a_uint32_t dev_id, a_uint32_t flag)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_entry_flush(dev_id, flag);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete Fdb entries on a particular port
 *   @details   Comments:
 *       If set FAL_FDB_DEL_STATIC bit in flag which means delete all fdb
 *       entries otherwise only delete dynamic entries.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] flag delete operation option
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_entry_del_byport(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t flag)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_entry_del_byport(dev_id, port_id, flag);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete a particular Fdb entry through mac address
 *   @details   Comments:
 *       Only addr field in entry is meaning. For IVL learning vid or fid field
 *       also is meaning.
 * @param[in] dev_id device id
 * @param[in] entry fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_entry_del_bymac(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_entry_del_bymac(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get first Fdb entry from particular device
 * @param[in] dev_id device id
 * @param[out] entry fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_entry_getfirst(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_entry_getfirst(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_FDB_MINI
/**
 * @brief Get next Fdb entry from particular device
 *   @details   Comments:
    For input parameter only addr field in entry is meaning.
 * @param[in] dev_id device id
 * @param entry fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_entry_getnext(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_entry_getnext(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}
#endif

/**
 * @brief Find a particular Fdb entry from device through mac address.
 *    @details  Comments:
    For input parameter only addr field in entry is meaning.
 * @param[in] dev_id device id
 * @param[in] entry fdb entry
 * @param[out] entry fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_entry_search(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_entry_search(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dynamic address and station move learning status on a particular port.
 *    @details  Comments:
 *       This operation will enable or disable dynamic address and
 *       station move learning feature on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable enable or disable
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_port_learn_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_port_learn_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_FDB_MINI
/**
 * @brief Get dynamic address and station move learning status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_port_learn_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_port_learn_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}
#endif

/**
 * @brief Set dynamic address learning and forward command on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] learning status
 * @param[in] forward command
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_port_learning_ctrl_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable, fal_fwd_cmd_t cmd)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_fdb_port_learning_ctrl_set(dev_id, port_id, enable, cmd);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_FDB_MINI
/**
 * @brief Get dynamic address learning and forward command on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] learning status
 * @param[out] forward command
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_port_learning_ctrl_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable, fal_fwd_cmd_t *cmd)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_fdb_port_learning_ctrl_get(dev_id, port_id, enable, cmd);
    FAL_API_UNLOCK;
    return rv;
}
#endif

/**
 * @brief Set station move learning and forward command on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] learning status
 * @param[in] forward command
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_port_stamove_ctrl_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable, fal_fwd_cmd_t cmd)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_fdb_port_stamove_ctrl_set(dev_id, port_id, enable, cmd);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_FDB_MINI
/**
 * @brief Get station move learning and forward command on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] learning status
 * @param[out] forward command
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_port_stamove_ctrl_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable, fal_fwd_cmd_t *cmd)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_fdb_port_stamove_ctrl_get(dev_id, port_id, enable, cmd);
    FAL_API_UNLOCK;
    return rv;
}
#endif

/**
 * @brief Set dynamic address aging status on particular device.
 *   @details  Comments:
 *       This operation will enable or disable dynamic address aging
 *       feature on particular device.
 * @param[in] dev_id device id
 * @param[in] enable enable or disable
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_aging_ctrl_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_aging_ctrl_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_FDB_MINI
/**
 * @brief Get dynamic address aging status on particular device.
 * @param[in] dev_id device id
 * @param[in] enable enable or disable
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_aging_ctrl_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_aging_ctrl_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief set arl search mode as ivl or svl when vlan invalid.
 * @param[in] dev_id device id
 * @param[in] smode INVALID_VLAN_IVL or INVALID_VLAN_SVL
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_vlan_ivl_svl_set(a_uint32_t dev_id, fal_fdb_smode smode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_vlan_ivl_svl_set(dev_id, smode);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief get arl search mode when vlan invalid.
 * @param[in] dev_id device id
 * @param[out] smode INVALID_VLAN_IVL or INVALID_VLAN_SVL
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_vlan_ivl_svl_get(a_uint32_t dev_id, fal_fdb_smode* smode)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_vlan_ivl_svl_get(dev_id, smode);
    FAL_API_UNLOCK;
    return rv;
}




/**
 * @brief Set dynamic address aging time on a particular device.
 * @details   Comments:
 *       This operation will set dynamic address aging time on a particular device.
 *       The unit of time is second. Because different device has differnet
 *       hardware granularity function will return actual time in hardware.
 * @param[in] dev_id device id
 * @param time aging time
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_aging_time_set(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_aging_time_set(dev_id, time);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dynamic address aging time on a particular device.
 * @param[in] dev_id device id
 * @param[out] time aging time
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_aging_time_get(a_uint32_t dev_id, a_uint32_t * time)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_aging_time_get(dev_id, time);
    FAL_API_UNLOCK;
    return rv;
}
#endif

/**
 * @brief Iterate all fdb entries on a particular device.
 * @param[in] dev_id device id
 * @param[in] iterator fdb entry index if it's zero means get the first entry
 * @param[out] iterator next valid fdb entry index
 * @param[out] entry fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_entry_getnext_byindex(a_uint32_t dev_id, a_uint32_t * iterator, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_entry_getnext_byindex(dev_id, iterator, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get next Fdb entry from a particular device
 * @param[in] dev_id device id
 * @param[in] option next operation options
 * @param[out] entry fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_entry_extend_getnext(a_uint32_t dev_id, fal_fdb_op_t * option,
                    fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_entry_extend_getnext(dev_id, option, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get first Fdb entry from a particular device
 * @param[in] dev_id device id
 * @param[in] option first operation options
 * @param[out] entry fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_entry_extend_getfirst(a_uint32_t dev_id, fal_fdb_op_t * option,
                     fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_entry_extend_getfirst(dev_id, option, entry);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_FDB_MINI
/**
 * @brief Transfer fdb entries port information on a particular device.
 * @param[in] dev_id device id
 * @param[in] old_port source port id
 * @param[in] new_port destination port id
 * @param[in] fid filter database id
 * @param[in] option transfer operation options
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_entry_update_byport(a_uint32_t dev_id, fal_port_t old_port, fal_port_t new_port,
                 a_uint32_t fid, fal_fdb_op_t * option)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_entry_update_byport(dev_id, old_port, new_port, fid, option);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dynamic address learning count limit on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] cnt limit count
 * @return SW_OK or error code
 */
sw_error_t
fal_port_fdb_learn_limit_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable, a_uint32_t cnt)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_fdb_learn_limit_set(dev_id, port_id, enable, cnt);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dynamic address learning count limit on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @param[out] cnt limit count
 * @return SW_OK or error code
 */
sw_error_t
fal_port_fdb_learn_limit_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable, a_uint32_t * cnt)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_fdb_learn_limit_get(dev_id, port_id, enable, cnt);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dynamic address learning count exceed command on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_port_fdb_learn_exceed_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                                  fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_fdb_learn_exceed_cmd_set(dev_id, port_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dynamic address learning count exceed command on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_port_fdb_learn_exceed_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                                  fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_port_fdb_learn_exceed_cmd_get(dev_id, port_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dynamic address learning count limit on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] cnt limit count
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_learn_limit_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t cnt)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_learn_limit_set(dev_id, enable, cnt);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dynamic address learning count limit on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @param[in] cnt limit count
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_learn_limit_get(a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * cnt)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_learn_limit_get(dev_id, enable, cnt);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dynamic address learning count exceed command on a particular device.
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_learn_exceed_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_learn_exceed_cmd_set(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dynamic address learning count exceed command on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_learn_exceed_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_learn_exceed_cmd_get(dev_id, cmd);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add a particular reserve Fdb entry
 * @param[in] dev_id device id
 * @param[in] entry reserve fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_resv_add(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_resv_add(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete a particular reserve Fdb entry through mac address
 * @param[in] dev_id device id
 * @param[in] entry reserve fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_resv_del(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_resv_del(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Find a particular reserve Fdb entry through mac address
 * @param[in] dev_id device id
 * @param[in] entry reserve fdb entry
 * @param[out] entry reserve fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_resv_find(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_resv_find(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Iterate all reserve fdb entries on a particular device.
 * @param[in] dev_id device id
 * @param[in] iterator reserve fdb entry index if it's zero means get the first entry
 * @param[out] iterator next valid fdb entry index
 * @param[out] entry reserve fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_resv_iterate(a_uint32_t dev_id, a_uint32_t * iterator,
                     fal_fdb_entry_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_resv_iterate(dev_id, iterator, entry);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_fdb_port_learn_static_set(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_port_learn_static_set(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t
fal_fdb_port_learn_static_get(a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t * enable)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_port_learn_static_get(dev_id, port_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add a port to an exsiting entry
 * @param[in] dev_id device id
 * @param[in] fid filtering database id
 * @param[in] addr MAC address
 * @param[in] port_id port id
 * @return SW_OK or error code, If entry not exist will return error.
 */
sw_error_t
fal_fdb_port_add(a_uint32_t dev_id, a_uint32_t fid, fal_mac_addr_t * addr, fal_port_t port_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_port_add(dev_id, fid, addr, port_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete a port from an exsiting entry
 * @param[in] dev_id device id
 * @param[in] fid filtering database id
 * @param[in] addr MAC address
 * @param[in] port_id port id
 * @return SW_OK or error code, If entry not exist will return error.
 */
sw_error_t
fal_fdb_port_del(a_uint32_t dev_id, a_uint32_t fid, fal_mac_addr_t * addr, fal_port_t port_id)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_port_del(dev_id, fid, addr, port_id);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add a Fdb rfs entry
 * @param[in] dev_id device id
 * @param[in] entry fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_rfs_set(a_uint32_t dev_id, const fal_fdb_rfs_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_rfs_set(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

/**
 * @brief Del a Fdb rfs entry
 * @param[in] dev_id device id
 * @param[in] entry fdb entry
 * @return SW_OK or error code
 */
sw_error_t
fal_fdb_rfs_del(a_uint32_t dev_id, const fal_fdb_rfs_t * entry)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_fdb_rfs_del(dev_id, entry);
    FAL_API_UNLOCK;
    return rv;
}

int ssdk_rfs_mac_rule_set(u16 vid, u8* mac, u8 ldb, int is_set)
{
	fal_fdb_rfs_t entry;
	memcpy(&entry.addr, mac, 6);
	entry.fid = vid;
	entry.load_balance = ldb;
	if(is_set)
		return fal_fdb_rfs_set(0, &entry);
	else
		return fal_fdb_rfs_del(0, &entry);
}
#endif

#if 0
int ssdk_rfs_mac_rule_set(ssdk_fdb_rfs_t *rfs)
{
	fal_fdb_rfs_t entry;
	memcpy(&entry.addr, rfs->addr, 6);
	entry.fid = rfs->fid;
	entry.load_balance = rfs->load_balance;
	return fal_fdb_rfs_set(0, &entry);
}

int ssdk_rfs_mac_rule_del(ssdk_fdb_rfs_t *rfs)
{
	fal_fdb_rfs_t entry;
	memcpy(&entry.addr, rfs->addr, 6);
	entry.fid = rfs->fid;
	entry.load_balance = rfs->load_balance;
	return fal_fdb_rfs_del(0, &entry);
}


EXPORT_SYMBOL(ssdk_rfs_mac_rule_set);
EXPORT_SYMBOL(ssdk_rfs_mac_rule_del);
#endif

sw_error_t
fal_fdb_learning_ctrl_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_fdb_learning_ctrl_set(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_FDB_MINI
sw_error_t
fal_fdb_learning_ctrl_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_fdb_learning_ctrl_get(dev_id, enable);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_fdb_port_learned_mac_counter_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t * cnt)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_fdb_port_learned_mac_counter_get(dev_id, port_id, cnt);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_fdb_port_maclimit_ctrl_set(a_uint32_t dev_id, fal_port_t port_id, fal_maclimit_ctrl_t * maclimit_ctrl)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_fdb_port_maclimit_ctrl_set(dev_id, port_id, maclimit_ctrl);
    FAL_API_UNLOCK;
    return rv;
}
sw_error_t
fal_fdb_port_maclimit_ctrl_get(a_uint32_t dev_id, fal_port_t port_id, fal_maclimit_ctrl_t * maclimit_ctrl)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_fdb_port_maclimit_ctrl_get(dev_id, port_id, maclimit_ctrl);
    FAL_API_UNLOCK;
    return rv;
}
#endif
sw_error_t
fal_fdb_entry_del_byfid(a_uint32_t dev_id, a_uint16_t fid, a_uint32_t flag)
{
    sw_error_t rv = SW_OK;

    FAL_API_LOCK;
    rv = _fal_fdb_entry_del_byfid(dev_id, fid, flag);
    FAL_API_UNLOCK;
    return rv;
}

#ifndef IN_FDB_MINI

    EXPORT_SYMBOL(fal_fdb_entry_add);

#endif

    EXPORT_SYMBOL(fal_fdb_entry_del_byport);


    EXPORT_SYMBOL(fal_fdb_entry_del_bymac);


    EXPORT_SYMBOL(fal_fdb_entry_search);

#ifndef IN_FDB_MINI

    EXPORT_SYMBOL(fal_fdb_port_learn_get);

#endif

    EXPORT_SYMBOL(fal_fdb_port_learning_ctrl_set);

    EXPORT_SYMBOL(fal_fdb_entry_flush);

    EXPORT_SYMBOL(fal_fdb_entry_getfirst);

#ifndef IN_FDB_MINI

    EXPORT_SYMBOL(fal_fdb_entry_getnext);

#endif

    EXPORT_SYMBOL(fal_fdb_port_learn_set);

    EXPORT_SYMBOL(fal_fdb_learning_ctrl_set);

#ifndef IN_FDB_MINI

    EXPORT_SYMBOL(fal_fdb_learning_ctrl_get);

#endif

    EXPORT_SYMBOL(fal_fdb_entry_getnext_byindex);

    EXPORT_SYMBOL(fal_fdb_entry_extend_getnext);

    EXPORT_SYMBOL(fal_fdb_entry_extend_getfirst);

#ifndef IN_FDB_MINI

    EXPORT_SYMBOL(fal_fdb_port_learning_ctrl_get);

#endif

    EXPORT_SYMBOL(fal_fdb_port_stamove_ctrl_set);

#ifndef IN_FDB_MINI

    EXPORT_SYMBOL(fal_fdb_port_stamove_ctrl_get);

#endif

    EXPORT_SYMBOL(fal_fdb_aging_ctrl_set);

#ifndef IN_FDB_MINI

    EXPORT_SYMBOL(fal_fdb_aging_ctrl_get);

    EXPORT_SYMBOL(fal_fdb_aging_time_set);

    EXPORT_SYMBOL(fal_fdb_aging_time_get);

    EXPORT_SYMBOL(fal_fdb_entry_update_byport);

    EXPORT_SYMBOL(fal_port_fdb_learn_limit_set);

    EXPORT_SYMBOL(fal_port_fdb_learn_limit_get);

    EXPORT_SYMBOL(fal_port_fdb_learn_exceed_cmd_set);

    EXPORT_SYMBOL(fal_port_fdb_learn_exceed_cmd_get);

    EXPORT_SYMBOL(fal_fdb_port_add);

    EXPORT_SYMBOL(fal_fdb_port_del);

    EXPORT_SYMBOL(fal_fdb_port_maclimit_ctrl_set);

    EXPORT_SYMBOL(fal_fdb_port_maclimit_ctrl_get);

#endif

    EXPORT_SYMBOL(fal_fdb_entry_del_byfid);

#ifndef IN_FDB_MINI

    EXPORT_SYMBOL(fal_fdb_port_learned_mac_counter_get);

#endif

/*insert flag for outter fal, don't remove it*/

/**
 * @}
 */

