/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
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



#ifndef _ATHENA_API_H_
#define _ATHENA_API_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#ifdef IN_PORTCONTROL
#define PORTCONTROL_API \
    SW_API_DEF(SW_API_PT_DUPLEX_GET, athena_port_duplex_get), \
    SW_API_DEF(SW_API_PT_DUPLEX_SET, athena_port_duplex_set), \
    SW_API_DEF(SW_API_PT_SPEED_GET, athena_port_speed_get), \
    SW_API_DEF(SW_API_PT_SPEED_SET, athena_port_speed_set), \
    SW_API_DEF(SW_API_PT_AN_GET, athena_port_autoneg_status_get), \
    SW_API_DEF(SW_API_PT_AN_ENABLE, athena_port_autoneg_enable), \
    SW_API_DEF(SW_API_PT_AN_RESTART, athena_port_autoneg_restart), \
    SW_API_DEF(SW_API_PT_AN_ADV_GET, athena_port_autoneg_adv_get), \
    SW_API_DEF(SW_API_PT_AN_ADV_SET, athena_port_autoneg_adv_set), \
    SW_API_DEF(SW_API_PT_IGMPS_MODE_SET, athena_port_igmps_status_set), \
    SW_API_DEF(SW_API_PT_IGMPS_MODE_GET, athena_port_igmps_status_get), \
    SW_API_DEF(SW_API_PT_POWERSAVE_SET, athena_port_powersave_set), \
    SW_API_DEF(SW_API_PT_POWERSAVE_GET, athena_port_powersave_get), \
    SW_API_DEF(SW_API_PT_HIBERNATE_SET, athena_port_hibernate_set), \
    SW_API_DEF(SW_API_PT_HIBERNATE_GET, athena_port_hibernate_get),


#define PORTCONTROL_API_PARAM \
    SW_API_DESC(SW_API_PT_DUPLEX_GET)  \
    SW_API_DESC(SW_API_PT_DUPLEX_SET)  \
    SW_API_DESC(SW_API_PT_SPEED_GET)  \
    SW_API_DESC(SW_API_PT_SPEED_SET)  \
    SW_API_DESC(SW_API_PT_AN_GET)  \
    SW_API_DESC(SW_API_PT_AN_ENABLE)  \
    SW_API_DESC(SW_API_PT_AN_RESTART) \
    SW_API_DESC(SW_API_PT_AN_ADV_GET) \
    SW_API_DESC(SW_API_PT_AN_ADV_SET) \
    SW_API_DESC(SW_API_PT_IGMPS_MODE_SET) \
    SW_API_DESC(SW_API_PT_IGMPS_MODE_GET) \
    SW_API_DESC(SW_API_PT_POWERSAVE_SET) \
    SW_API_DESC(SW_API_PT_POWERSAVE_GET) \
    SW_API_DESC(SW_API_PT_HIBERNATE_SET) \
    SW_API_DESC(SW_API_PT_HIBERNATE_GET)
#else
#define PORTCONTROL_API
#define PORTCONTROL_API_PARAM
#endif

#ifdef IN_VLAN
#define VLAN_API \
    SW_API_DEF(SW_API_VLAN_ADD, athena_vlan_create), \
    SW_API_DEF(SW_API_VLAN_DEL, athena_vlan_delete), \
    SW_API_DEF(SW_API_VLAN_MEM_UPDATE, athena_vlan_member_update), \
    SW_API_DEF(SW_API_VLAN_FIND, athena_vlan_find), \
    SW_API_DEF(SW_API_VLAN_NEXT, athena_vlan_next), \
    SW_API_DEF(SW_API_VLAN_APPEND, athena_vlan_entry_append),

#define VLAN_API_PARAM \
    SW_API_DESC(SW_API_VLAN_ADD)  \
    SW_API_DESC(SW_API_VLAN_DEL)  \
    SW_API_DESC(SW_API_VLAN_MEM_UPDATE) \
    SW_API_DESC(SW_API_VLAN_FIND) \
    SW_API_DESC(SW_API_VLAN_NEXT) \
    SW_API_DESC(SW_API_VLAN_APPEND)
#else
#define VLAN_API
#define VLAN_API_PARAM
#endif

#ifdef IN_PORTVLAN
#define PORTVLAN_API \
    SW_API_DEF(SW_API_PT_ING_MODE_GET, athena_port_1qmode_get), \
    SW_API_DEF(SW_API_PT_ING_MODE_SET, athena_port_1qmode_set), \
    SW_API_DEF(SW_API_PT_EG_MODE_GET, athena_port_egvlanmode_get), \
    SW_API_DEF(SW_API_PT_EG_MODE_SET, athena_port_egvlanmode_set), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_ADD, athena_portvlan_member_add), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_DEL, athena_portvlan_member_del), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_UPDATE, athena_portvlan_member_update), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_GET, athena_portvlan_member_get), \
 
#define PORTVLAN_API_PARAM \
    SW_API_DESC(SW_API_PT_ING_MODE_GET)  \
    SW_API_DESC(SW_API_PT_ING_MODE_SET)  \
    SW_API_DESC(SW_API_PT_EG_MODE_GET)  \
    SW_API_DESC(SW_API_PT_EG_MODE_SET)  \
    SW_API_DESC(SW_API_PT_VLAN_MEM_ADD) \
    SW_API_DESC(SW_API_PT_VLAN_MEM_DEL) \
    SW_API_DESC(SW_API_PT_VLAN_MEM_UPDATE)  \
    SW_API_DESC(SW_API_PT_VLAN_MEM_GET)
#else
#define PORTVLAN_API
#define PORTVLAN_API_PARAM
#endif

#ifdef IN_FDB
#define FDB_API \
    SW_API_DEF(SW_API_FDB_ADD, athena_fdb_add), \
    SW_API_DEF(SW_API_FDB_DELALL, athena_fdb_del_all), \
    SW_API_DEF(SW_API_FDB_DELPORT,athena_fdb_del_by_port), \
    SW_API_DEF(SW_API_FDB_DELMAC, athena_fdb_del_by_mac), \
    SW_API_DEF(SW_API_FDB_FIRST,  athena_fdb_first), \
    SW_API_DEF(SW_API_FDB_NEXT,   athena_fdb_next),

#define FDB_API_PARAM \
    SW_API_DESC(SW_API_FDB_ADD)  \
    SW_API_DESC(SW_API_FDB_DELALL)  \
    SW_API_DESC(SW_API_FDB_DELPORT) \
    SW_API_DESC(SW_API_FDB_DELMAC)  \
    SW_API_DESC(SW_API_FDB_FIRST)   \
    SW_API_DESC(SW_API_FDB_NEXT)
#else
#define FDB_API
#define FDB_API_PARAM
#endif

#ifdef IN_MIB
#define MIB_API \
    SW_API_DEF(SW_API_PT_MIB_GET, athena_get_mib_info),

#define MIB_API_PARAM \
    SW_API_DESC(SW_API_PT_MIB_GET)
#else
#define MIB_API
#define MIB_API_PARAM
#endif

#define REG_API \
    SW_API_DEF(SW_API_PHY_GET, athena_phy_get), \
    SW_API_DEF(SW_API_PHY_SET, athena_phy_set), \
    SW_API_DEF(SW_API_REG_GET, athena_reg_get), \
    SW_API_DEF(SW_API_REG_SET, athena_reg_set), \
    SW_API_DEF(SW_API_REG_FIELD_GET, athena_reg_field_get), \
    SW_API_DEF(SW_API_REG_FIELD_SET, athena_reg_field_set),

#define REG_API_PARAM \
    SW_API_DESC(SW_API_PHY_GET)  \
    SW_API_DESC(SW_API_PHY_SET)  \
    SW_API_DESC(SW_API_REG_GET)  \
    SW_API_DESC(SW_API_REG_SET)  \
    SW_API_DESC(SW_API_REG_FIELD_GET) \
    SW_API_DESC(SW_API_REG_FIELD_SET)

#define SSDK_API \
    SW_API_DEF(SW_API_SWITCH_RESET, athena_reset), \
    SW_API_DEF(SW_API_SSDK_CFG, hsl_ssdk_cfg), \
    MIB_API \
    PORTCONTROL_API \
    PORTVLAN_API \
    VLAN_API \
    FDB_API \
    REG_API \
    SW_API_DEF(SW_API_MAX, NULL)

#define SSDK_PARAM  \
    SW_PARAM_DEF(SW_API_SWITCH_RESET, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"), \
    SW_PARAM_DEF(SW_API_SSDK_CFG, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"), \
    SW_PARAM_DEF(SW_API_SSDK_CFG, SW_SSDK_CFG, sizeof(ssdk_cfg_t), SW_PARAM_PTR|SW_PARAM_OUT, "ssdk configuration"), \
    MIB_API_PARAM \
    PORTCONTROL_API_PARAM \
    PORTVLAN_API_PARAM \
    VLAN_API_PARAM \
    FDB_API_PARAM \
    REG_API_PARAM \
    SW_PARAM_DEF(SW_API_MAX, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"),

#if (defined(USER_MODE) && defined(KERNEL_MODULE))
#undef SSDK_API
#undef SSDK_PARAM

#define SSDK_API \
    REG_API \
    SW_API_DEF(SW_API_MAX, NULL),

#define SSDK_PARAM  \
    REG_API_PARAM \
    SW_PARAM_DEF(SW_API_MAX, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"),
#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _ATHENA_API_H_ */
