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
#ifndef _FAL_FDB_H_
#define _FAL_FDB_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "sw.h"
#include "fal/fal_type.h"

    /**
      @details  Fields description:

        portmap_en - If value of portmap_en is A_TRUE then port.map is valid
        otherwise port.id is valid.


        leaky_en - If value of leaky_en is A_TRUE then packets which
        destination address equals addr in this entry would be leaky.


        mirror_en - If value of mirror_en is A_TRUE then packets which
        destination address equals addr in this entry would be mirrored.


        clone_en - If value of clone_en is A_TRUE which means this address is
        a mac clone address.
    @brief This structure defines the Fdb entry.

    */
    typedef struct
    {
        fal_mac_addr_t addr; /* mac address of fdb entry */
        a_uint16_t    fid; /* vlan_id/vsi value of fdb entry */
        fal_fwd_cmd_t dacmd; /* source address command */
        fal_fwd_cmd_t sacmd; /* dest address command */
        union
        {
            fal_port_t id; /* union value is port id value */
            fal_pbmp_t map; /* union value is bitmap value */
        } port;
        a_bool_t portmap_en; /* use port bitmap or not */
        a_bool_t is_multicast; /* if it is a multicast mac fdb entry */
        a_bool_t static_en; /* enable static or not */
        a_bool_t leaky_en; /* enable leaky or not */
        a_bool_t mirror_en; /* enable mirror or not */
        a_bool_t clone_en; /* enable clone or not */
        a_bool_t cross_pt_state; /* cross port state */
        a_bool_t da_pri_en; /* enable da pri or not */
        a_uint8_t da_queue; /* da queue value */
        a_bool_t white_list_en; /* enable white list or not */
        a_bool_t load_balance_en; /* enable load balance value or not */
        a_uint8_t load_balance; /* load balance value */
        a_bool_t entry_valid; /* check if entry is value */
        a_bool_t lookup_valid; /* check if entry is lookup */
    } fal_fdb_entry_t;

    typedef struct
    {
        fal_mac_addr_t addr;
        a_uint16_t    fid;
        a_uint8_t load_balance;
    } fal_fdb_rfs_t;

    typedef struct
    {
        a_bool_t enable; /* enable port learn limit or not */
        a_uint32_t limit_num; /* port learn limit number */
        fal_fwd_cmd_t action; /* the action when port learn number exceed limit*/
    } fal_maclimit_ctrl_t;

#define FAL_FDB_DEL_STATIC   0x1

    typedef struct
    {
        a_bool_t port_en; /* enable port value matching or not */
        a_bool_t fid_en; /* enable fid value matching or not */
        a_bool_t multicast_en; /* enable multicast value matching or not */
    } fal_fdb_op_t;

    typedef enum
    {
        INVALID_VLAN_SVL=0,
        INVALID_VLAN_IVL
    } fal_fdb_smode;

enum {
	FUNC_FDB_ENTRY_ADD = 0,
	FUNC_FDB_ENTRY_FLUSH,
	FUNC_FDB_ENTRY_DEL_BYPORT,
	FUNC_FDB_ENTRY_DEL_BYMAC,
	FUNC_FDB_ENTRY_GETFIRST,
	FUNC_FDB_ENTRY_GETNEXT,
	FUNC_FDB_ENTRY_SEARCH,
	FUNC_FDB_PORT_LEARN_SET,
	FUNC_FDB_PORT_LEARN_GET,
	FUNC_FDB_PORT_LEARNING_CTRL_SET,
	FUNC_FDB_PORT_LEARNING_CTRL_GET,
	FUNC_FDB_PORT_STAMOVE_CTRL_SET,
	FUNC_FDB_PORT_STAMOVE_CTRL_GET,
	FUNC_FDB_AGING_CTRL_SET,
	FUNC_FDB_AGING_CTRL_GET,
	FUNC_FDB_LEARNING_CTRL_SET,
	FUNC_FDB_LEARNING_CTRL_GET,
	FUNC_FDB_AGING_TIME_SET,
	FUNC_FDB_AGING_TIME_GET,
	FUNC_FDB_ENTRY_GETNEXT_BYINDEX,
	FUNC_FDB_ENTRY_EXTEND_GETNEXT,
	FUNC_FDB_ENTRY_EXTEND_GETFIRST,
	FUNC_FDB_ENTRY_UPDATE_BYPORT,
	FUNC_PORT_FDB_LEARN_LIMIT_SET,
	FUNC_PORT_FDB_LEARN_LIMIT_GET,
	FUNC_PORT_FDB_LEARN_EXCEED_CMD_SET,
	FUNC_PORT_FDB_LEARN_EXCEED_CMD_GET,
	FUNC_FDB_PORT_LEARNED_MAC_COUNTER_GET,
	FUNC_FDB_PORT_ADD,
	FUNC_FDB_PORT_DEL,
	FUNC_FDB_PORT_MACLIMIT_CTRL_SET,
	FUNC_FDB_PORT_MACLIMIT_CTRL_GET,
	FUNC_FDB_DEL_BY_FID,
};

#ifndef IN_FDB_MINI
    sw_error_t
    fal_fdb_entry_add(a_uint32_t dev_id, const fal_fdb_entry_t * entry);

	sw_error_t
    fal_fdb_rfs_set(a_uint32_t dev_id, const fal_fdb_rfs_t * entry);

	sw_error_t
    fal_fdb_rfs_del(a_uint32_t dev_id, const fal_fdb_rfs_t * entry);
#endif

    sw_error_t
    fal_fdb_entry_flush(a_uint32_t dev_id, a_uint32_t flag);


    sw_error_t
    fal_fdb_entry_del_byport(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t flag);

    sw_error_t
    fal_fdb_entry_del_bymac(a_uint32_t dev_id, const fal_fdb_entry_t *entry);

    sw_error_t
    fal_fdb_entry_getfirst(a_uint32_t dev_id, fal_fdb_entry_t * entry);


#ifndef IN_FDB_MINI
    sw_error_t
    fal_fdb_entry_getnext(a_uint32_t dev_id, fal_fdb_entry_t * entry);
#endif

    sw_error_t
    fal_fdb_entry_search(a_uint32_t dev_id, fal_fdb_entry_t * entry);


sw_error_t
    fal_fdb_port_learn_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


#ifndef IN_FDB_MINI
    sw_error_t
    fal_fdb_port_learn_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);
#endif
    sw_error_t
    fal_fdb_port_learning_ctrl_set(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t enable, fal_fwd_cmd_t cmd);
#ifndef IN_FDB_MINI
    sw_error_t
    fal_fdb_port_learning_ctrl_get(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t *enable, fal_fwd_cmd_t *cmd);
#endif

    sw_error_t
    fal_fdb_port_stamove_ctrl_set(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t enable, fal_fwd_cmd_t cmd);

#ifndef IN_FDB_MINI
    sw_error_t
    fal_fdb_port_stamove_ctrl_get(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t *enable, fal_fwd_cmd_t *cmd);
#endif

    sw_error_t
    fal_fdb_aging_ctrl_set(a_uint32_t dev_id, a_bool_t enable);

#ifndef IN_FDB_MINI
    sw_error_t
    fal_fdb_aging_ctrl_get(a_uint32_t dev_id, a_bool_t * enable);
#endif

    sw_error_t
    fal_fdb_learning_ctrl_set(a_uint32_t dev_id, a_bool_t enable);

#ifndef IN_FDB_MINI
    sw_error_t
    fal_fdb_learning_ctrl_get(a_uint32_t dev_id, a_bool_t * enable);

    sw_error_t
    fal_fdb_vlan_ivl_svl_set(a_uint32_t dev_id, fal_fdb_smode smode);

    sw_error_t
    fal_fdb_vlan_ivl_svl_get(a_uint32_t dev_id, fal_fdb_smode * smode);

    sw_error_t
    fal_fdb_aging_time_set(a_uint32_t dev_id, a_uint32_t * time);


    sw_error_t
    fal_fdb_aging_time_get(a_uint32_t dev_id, a_uint32_t * time);
#endif
    sw_error_t
    fal_fdb_entry_getnext_byindex(a_uint32_t dev_id, a_uint32_t * iterator, fal_fdb_entry_t * entry);

    sw_error_t
    fal_fdb_entry_extend_getnext(a_uint32_t dev_id, fal_fdb_op_t * option,
                        fal_fdb_entry_t * entry);

    sw_error_t
    fal_fdb_entry_extend_getfirst(a_uint32_t dev_id, fal_fdb_op_t * option,
                         fal_fdb_entry_t * entry);

#ifndef IN_FDB_MINI
    sw_error_t
    fal_fdb_entry_update_byport(a_uint32_t dev_id, fal_port_t old_port, fal_port_t new_port,
                     a_uint32_t fid, fal_fdb_op_t * option);

    sw_error_t
    fal_port_fdb_learn_limit_set(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t enable, a_uint32_t cnt);

    sw_error_t
    fal_port_fdb_learn_limit_get(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t * enable, a_uint32_t * cnt);

    sw_error_t
    fal_port_fdb_learn_exceed_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                                      fal_fwd_cmd_t cmd);


    sw_error_t
    fal_port_fdb_learn_exceed_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                                      fal_fwd_cmd_t * cmd);

    sw_error_t
    fal_fdb_port_learned_mac_counter_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t * cnt);

    sw_error_t
    fal_fdb_learn_limit_set(a_uint32_t dev_id, a_bool_t enable, a_uint32_t cnt);

    sw_error_t
    fal_fdb_learn_limit_get(a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * cnt);

    sw_error_t
    fal_fdb_learn_exceed_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd);

    sw_error_t
    fal_fdb_learn_exceed_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

    sw_error_t
    fal_fdb_resv_add(a_uint32_t dev_id, fal_fdb_entry_t * entry);

    sw_error_t
    fal_fdb_resv_del(a_uint32_t dev_id, fal_fdb_entry_t * entry);

    sw_error_t
    fal_fdb_resv_find(a_uint32_t dev_id, fal_fdb_entry_t * entry);

    sw_error_t
    fal_fdb_resv_iterate(a_uint32_t dev_id, a_uint32_t * iterator, fal_fdb_entry_t * entry);

    sw_error_t
    fal_fdb_port_learn_static_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


    sw_error_t
    fal_fdb_port_learn_static_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t * enable);

    sw_error_t
    fal_fdb_port_add(a_uint32_t dev_id, a_uint32_t fid, fal_mac_addr_t * addr, fal_port_t port_id);

    sw_error_t
    fal_fdb_port_del(a_uint32_t dev_id, a_uint32_t fid, fal_mac_addr_t * addr, fal_port_t port_id);

    sw_error_t
    fal_fdb_port_maclimit_ctrl_set(a_uint32_t dev_id, fal_port_t port_id, fal_maclimit_ctrl_t * maclimit_ctrl);

    sw_error_t
    fal_fdb_port_maclimit_ctrl_get(a_uint32_t dev_id, fal_port_t port_id, fal_maclimit_ctrl_t * maclimit_ctrl);
#endif

    sw_error_t
    fal_fdb_entry_del_byfid(a_uint32_t dev_id, a_uint16_t fid, a_uint32_t flag);

#define fal_fdb_add 	fal_fdb_entry_add
#define fal_fdb_del_all	fal_fdb_entry_flush
#define fal_fdb_del_by_port	fal_fdb_entry_del_byport
#define fal_fdb_del_by_mac	fal_fdb_entry_del_bymac
#define fal_fdb_first		fal_fdb_entry_getfirst
#define fal_fdb_next		fal_fdb_entry_getnext
#define fal_fdb_find		fal_fdb_entry_search
#define fal_fdb_age_ctrl_set		fal_fdb_aging_ctrl_set
#define fal_fdb_age_ctrl_get		fal_fdb_aging_ctrl_get
#define fal_fdb_age_time_set		fal_fdb_aging_time_set
#define fal_fdb_age_time_get		fal_fdb_aging_time_get
#define fal_fdb_iterate			fal_fdb_entry_getnext_byindex
#define fal_fdb_extend_next		fal_fdb_entry_extend_getnext
#define fal_fdb_extend_first		fal_fdb_entry_extend_getfirst
#define fal_fdb_transfer		fal_fdb_entry_update_byport

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_FDB_H_ */
/**
 * @}
 */
