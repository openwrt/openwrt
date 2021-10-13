/*
 * Copyright (c) 2014,2018, The Linux Foundation. All rights reserved.
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
#include "fal.h"
/*qca808x_end*/
#include "ref_vlan.h"
#if (defined(KERNEL_MODULE))
#include "hsl.h"
#include "hsl_dev.h"
#if defined ATHENA
#include "fal_igmp.h"
#include "fal_leaky.h"
#include "athena_mib.h"
#include "athena_port_ctrl.h"
#include "athena_portvlan.h"
#include "athena_fdb.h"
#include "athena_vlan.h"
#include "athena_init.h"
#include "athena_reg_access.h"
#include "athena_reg.h"
#elif defined GARUDA
#include "garuda_mib.h"
#include "garuda_qos.h"
#include "garuda_rate.h"
#include "garuda_port_ctrl.h"
#include "garuda_portvlan.h"
#include "garuda_fdb.h"
#include "garuda_vlan.h"
#include "garuda_mirror.h"
#include "garuda_stp.h"
#include "garuda_misc.h"
#include "garuda_leaky.h"
#include "garuda_igmp.h"
#include "garuda_acl.h"
#include "garuda_led.h"
#include "garuda_init.h"
#include "garuda_reg_access.h"
#include "garuda_reg.h"
#elif defined SHIVA
#include "shiva_mib.h"
#include "shiva_qos.h"
#include "shiva_rate.h"
#include "shiva_port_ctrl.h"
#include "shiva_portvlan.h"
#include "shiva_fdb.h"
#include "shiva_vlan.h"
#include "shiva_mirror.h"
#include "shiva_stp.h"
#include "shiva_misc.h"
#include "shiva_leaky.h"
#include "shiva_igmp.h"
#include "shiva_acl.h"
#include "shiva_led.h"
#include "shiva_init.h"
#include "shiva_reg_access.h"
#include "shiva_reg.h"
#elif defined HORUS
#include "horus_mib.h"
#include "horus_qos.h"
#include "horus_rate.h"
#include "horus_port_ctrl.h"
#include "horus_portvlan.h"
#include "horus_fdb.h"
#include "horus_vlan.h"
#include "horus_mirror.h"
#include "horus_stp.h"
#include "horus_misc.h"
#include "horus_leaky.h"
#include "horus_igmp.h"
#include "horus_led.h"
#include "horus_init.h"
#include "horus_reg_access.h"
#include "horus_reg.h"
#elif defined ISIS
#include "isis_mib.h"
#include "isis_qos.h"
#include "isis_cosmap.h"
#include "isis_rate.h"
#include "isis_port_ctrl.h"
#include "isis_portvlan.h"
#include "isis_fdb.h"
#include "isis_vlan.h"
#include "isis_mirror.h"
#include "isis_stp.h"
#include "isis_misc.h"
#include "isis_leaky.h"
#include "isis_igmp.h"
#include "isis_acl.h"
#include "isis_led.h"
#include "isis_cosmap.h"
#include "isis_ip.h"
#include "isis_nat.h"
#include "isis_trunk.h"
#include "isis_sec.h"
#include "isis_interface_ctrl.h"
#include "isis_init.h"
#include "isis_reg_access.h"
#include "isis_reg.h"
#elif defined ISISC
#include "isisc_mib.h"
#include "isisc_qos.h"
#include "isisc_cosmap.h"
#include "isisc_rate.h"
#include "isisc_port_ctrl.h"
#include "isisc_portvlan.h"
#include "isisc_fdb.h"
#include "isisc_vlan.h"
#include "isisc_mirror.h"
#include "isisc_stp.h"
#include "isisc_misc.h"
#include "isisc_leaky.h"
#include "isisc_igmp.h"
#include "isisc_acl.h"
#include "isisc_led.h"
#include "isisc_cosmap.h"
#include "isisc_ip.h"
#include "isisc_nat.h"
#include "isisc_trunk.h"
#include "isisc_sec.h"
#include "isisc_interface_ctrl.h"
#include "isisc_init.h"
#include "isisc_reg_access.h"
#include "isisc_reg.h"
#endif
#endif
/*qca808x_start*/
#include "sw_api.h"
#include "api_desc.h"
/*qca808x_end*/
#if (((!defined(USER_MODE)) && defined(KERNEL_MODULE)) || (defined(USER_MODE) && (!defined(KERNEL_MODULE))))
#ifdef HSL_STANDALONG
#if defined ATHENA
#include "athena_api.h"
#elif defined GARUDA
#include "garuda_api.h"
#elif defined SHIVA
#include "shiva_api.h"
#elif defined HORUS
#include "horus_api.h"
#elif defined ISIS
#include "isis_api.h"
#elif defined ISISC
#include "isisc_api.h"
#endif
#else
#include "ref_api.h"
#include "fal_api.h"
#endif
#elif (defined(USER_MODE))
#if defined ATHENA
#include "athena_api.h"
#elif defined GARUDA
#include "garuda_api.h"
#elif defined SHIVA
#include "shiva_api.h"
#elif defined HORUS
#include "horus_api.h"
#elif defined ISIS
#include "isis_api.h"
#elif defined ISISC
#include "isisc_api.h"
#endif
#else
#include "ref_api.h"
/*qca808x_start*/
#include "fal_api.h"
/*qca808x_end*/
#endif
/*qca808x_start*/
static sw_api_func_t sw_api_func[] = {
/*qca808x_end*/
	SSDK_REF_API
/*qca808x_start*/
	SSDK_API };
static sw_api_param_t sw_api_param[] = {
/*qca808x_end*/
	SSDK_REF_PARAM
/*qca808x_start*/
	SSDK_PARAM };

sw_api_func_t *
sw_api_func_find(a_uint32_t api_id)
{
    a_uint32_t i = 0;
    static a_uint32_t save = 0;

    if(api_id == sw_api_func[save].api_id)
        return &sw_api_func[save];

    do
    {
        if (api_id == sw_api_func[i].api_id)
        {
            save = i;
            return &sw_api_func[i];
        }

    }
    while (++i < (sizeof(sw_api_func)/sizeof(sw_api_func[0])));

    return NULL;
}

sw_api_param_t *
sw_api_param_find(a_uint32_t api_id)
{
    a_uint32_t i = 0;
    static a_uint32_t save = 0;

    if(api_id == sw_api_param[save].api_id)
        return &sw_api_param[save];

    do
    {
        if (api_id == sw_api_param[i].api_id)
        {
            save = i;
            return &sw_api_param[i];
        }
    }
    while (++i < (sizeof(sw_api_param)/sizeof(sw_api_param[0])));

    return NULL;
}

a_uint32_t
sw_api_param_nums(a_uint32_t api_id)
{
    a_uint32_t i = 0;
    sw_api_param_t *p = NULL;
    static sw_api_param_t *savep = NULL;
    static a_uint32_t save = 0;

    p = sw_api_param_find(api_id);
    if (!p)
    {
        return 0;
    }

    if (p == savep)
    {
        return save;
    }

    savep = p;
    while (api_id == p->api_id)
    {
        p++;
        i++;
    }

    /*error*/
    if(i >= sizeof(sw_api_param)/sizeof(sw_api_param[0]))
    {
        savep = NULL;
        save = 0;
        return 0;
    }
    save = i;

    return i;
}

sw_error_t
sw_api_get(sw_api_t *sw_api)
{
    if(!sw_api)
        return SW_FAIL;

    if ((sw_api->api_fp = sw_api_func_find(sw_api->api_id)) == NULL)
        return SW_NOT_SUPPORTED;

    if ((sw_api->api_pp = sw_api_param_find(sw_api->api_id)) == NULL)
        return SW_NOT_SUPPORTED;

    if((sw_api->api_nr = sw_api_param_nums(sw_api->api_id)) == 0)
        return SW_NOT_SUPPORTED;

    return SW_OK;
}
/*qca808x_end*/
