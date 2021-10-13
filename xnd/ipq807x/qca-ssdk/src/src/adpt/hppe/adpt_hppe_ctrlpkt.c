/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
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
 * @defgroup
 * @{
 */
#include "sw.h"
#include "hppe_ctrlpkt_reg.h"
#include "hppe_ctrlpkt.h"
#include "hppe_fdb_reg.h"
#include "hppe_fdb.h"
#include "adpt.h"

a_uint32_t
_get_mgmtctrl_ctrlpkt_profile_by_index(a_uint32_t dev_id, a_uint32_t index, fal_ctrlpkt_profile_t *ctrlpkt)
{
	union app_ctrl_u entry;

	SW_RTN_ON_ERROR(hppe_app_ctrl_get(dev_id, index, &entry));

	ctrlpkt->action.action = entry.bf.cmd;
	ctrlpkt->action.sg_bypass = entry.bf.sg_byp;
	ctrlpkt->action.l2_filter_bypass = entry.bf.l2_sec_byp;
	ctrlpkt->action.in_stp_bypass = entry.bf.in_stg_byp;
	ctrlpkt->action.in_vlan_fltr_bypass = entry.bf.in_vlan_fltr_byp;

	if (entry.bf.portbitmap_include)
		ctrlpkt->port_map = entry.bf.portbitmap;

	if (entry.bf.ethertype_include)
		ctrlpkt->ethtype_profile_bitmap = entry.bf.ethertype_index_bitmap_0 | (entry.bf.ethertype_index_bitmap_1 << 2);

	if (entry.bf.rfdb_include)
		ctrlpkt->rfdb_profile_bitmap = entry.bf.rfdb_index_bitmap_0| (entry.bf.rfdb_index_bitmap_1 << 30);

	if (entry.bf.protocol_include) {
		ctrlpkt->protocol_types.mgt_eapol = (entry.bf.protocol_bitmap & (0x1 << 0))?1:0;
		ctrlpkt->protocol_types.mgt_pppoe = (entry.bf.protocol_bitmap & (0x1 << 1))?1:0;
		ctrlpkt->protocol_types.mgt_igmp = (entry.bf.protocol_bitmap & (0x1 << 2))?1:0;
		ctrlpkt->protocol_types.mgt_arp_req = (entry.bf.protocol_bitmap & (0x1 << 3))?1:0;
		ctrlpkt->protocol_types.mgt_arp_rep = (entry.bf.protocol_bitmap & (0x1 << 4))?1:0;
		ctrlpkt->protocol_types.mgt_dhcp4 = (entry.bf.protocol_bitmap & (0x1 << 5))?1:0;
		ctrlpkt->protocol_types.mgt_mld = (entry.bf.protocol_bitmap & (0x1 << 6))?1:0;
		ctrlpkt->protocol_types.mgt_ns = (entry.bf.protocol_bitmap & (0x1 << 7))?1:0;
		ctrlpkt->protocol_types.mgt_na = (entry.bf.protocol_bitmap & (0x1 << 8))?1:0;
		ctrlpkt->protocol_types.mgt_dhcp6 = (entry.bf.protocol_bitmap & (0x1 << 9))?1:0;
	}

	return entry.bf.valid;
}

a_uint32_t
_check_if_ctrlpkt_equal(fal_ctrlpkt_profile_t *ctrlpkt1, fal_ctrlpkt_profile_t *ctrlpkt2)
{
	if (ctrlpkt1->action.action == ctrlpkt2->action.action &&
		ctrlpkt1->action.sg_bypass == ctrlpkt2->action.sg_bypass &&
		ctrlpkt1->action.l2_filter_bypass == ctrlpkt2->action.l2_filter_bypass &&
		ctrlpkt1->action.in_stp_bypass == ctrlpkt2->action.in_stp_bypass &&
		ctrlpkt1->action.in_vlan_fltr_bypass == ctrlpkt2->action.in_vlan_fltr_bypass &&
		ctrlpkt1->port_map == ctrlpkt2->port_map &&
		ctrlpkt1->ethtype_profile_bitmap == ctrlpkt2->ethtype_profile_bitmap &&
		ctrlpkt1->rfdb_profile_bitmap == ctrlpkt2->rfdb_profile_bitmap &&
		ctrlpkt1->protocol_types.mgt_eapol == ctrlpkt2->protocol_types.mgt_eapol &&
		ctrlpkt1->protocol_types.mgt_pppoe == ctrlpkt2->protocol_types.mgt_pppoe &&
		ctrlpkt1->protocol_types.mgt_igmp == ctrlpkt2->protocol_types.mgt_igmp &&
		ctrlpkt1->protocol_types.mgt_arp_req == ctrlpkt2->protocol_types.mgt_arp_req &&
		ctrlpkt1->protocol_types.mgt_arp_rep == ctrlpkt2->protocol_types.mgt_arp_rep &&
		ctrlpkt1->protocol_types.mgt_dhcp4 == ctrlpkt2->protocol_types.mgt_dhcp4 &&
		ctrlpkt1->protocol_types.mgt_mld == ctrlpkt2->protocol_types.mgt_mld &&
		ctrlpkt1->protocol_types.mgt_ns == ctrlpkt2->protocol_types.mgt_ns &&
		ctrlpkt1->protocol_types.mgt_na == ctrlpkt2->protocol_types.mgt_na &&
		ctrlpkt1->protocol_types.mgt_dhcp6 == ctrlpkt2->protocol_types.mgt_dhcp6)
		return 1;

	return 0;
}

sw_error_t
adpt_hppe_mgmtctrl_ethtype_profile_set(a_uint32_t dev_id, a_uint32_t profile_id, a_uint32_t ethtype)
{
	sw_error_t rtn = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);

	SW_RTN_ON_ERROR(hppe_ethertype_ctrl_ethertype_set(dev_id, profile_id, ethtype));
	SW_RTN_ON_ERROR(hppe_ethertype_ctrl_ethertype_en_set(dev_id, profile_id, A_TRUE));

	return rtn;
}

sw_error_t
adpt_hppe_mgmtctrl_ethtype_profile_get(a_uint32_t dev_id, a_uint32_t profile_id, a_uint32_t *ethtype)
{
	sw_error_t rtn = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ethtype);

	SW_RTN_ON_ERROR(hppe_ethertype_ctrl_ethertype_get(dev_id, profile_id, ethtype));

	return rtn;
}

sw_error_t
adpt_hppe_mgmtctrl_rfdb_profile_set(a_uint32_t dev_id, a_uint32_t profile_id, fal_mac_addr_t *addr)
{
	sw_error_t rtn = SW_OK;
	a_uint64_t value = 0;

	ADPT_DEV_ID_CHECK(dev_id);

	if (profile_id >= RFDB_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;

	value = ((((a_uint64_t)(addr->uc[5])) << 0) |
			(((a_uint64_t)(addr->uc[4])) << 8) |
			(((a_uint64_t)(addr->uc[3])) << 16) |
			(((a_uint64_t)(addr->uc[2])) << 24) |
			(((a_uint64_t)(addr->uc[1])) << 32) |
			(((a_uint64_t)(addr->uc[0])) << 40));

	SW_RTN_ON_ERROR(hppe_rfdb_tbl_mac_addr_set(dev_id, profile_id, value));
	SW_RTN_ON_ERROR(hppe_rfdb_tbl_valid_set(dev_id, profile_id, A_TRUE));

	return rtn;
}

sw_error_t
adpt_hppe_mgmtctrl_rfdb_profile_get(a_uint32_t dev_id, a_uint32_t profile_id, fal_mac_addr_t *addr)
{
	sw_error_t rtn = SW_OK;
	a_uint64_t value = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(addr);

	if (profile_id >= RFDB_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;

	SW_RTN_ON_ERROR(hppe_rfdb_tbl_mac_addr_get(dev_id, profile_id, &value));
	addr->uc[0] = (a_uint8_t)((value >> 40)& 0xff);
	addr->uc[1] = (a_uint8_t)((value >> 32) & 0xff);
	addr->uc[2] = (a_uint8_t)((value >> 24) & 0xff);
	addr->uc[3] = (a_uint8_t)((value >> 16) & 0xff);
	addr->uc[4] = (a_uint8_t)((value >> 8) & 0xff);
	addr->uc[5] = (a_uint8_t)((value >> 0) & 0xff);

	return rtn;
}

sw_error_t
adpt_hppe_mgmtctrl_ctrlpkt_profile_add(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt)
{
	union app_ctrl_u entry;
	a_uint32_t index, ctrlpkt_valid, entry_sign, entry_index;
	fal_ctrlpkt_profile_t ctrlpkt_temp;

	ADPT_DEV_ID_CHECK(dev_id);

	entry_index = 0;
	entry_sign = 0;
	for (index = 0; index < APP_CTRL_MAX_ENTRY; index++)
	{
		memset(&ctrlpkt_temp, 0, sizeof(fal_ctrlpkt_profile_t));
		ctrlpkt_valid = _get_mgmtctrl_ctrlpkt_profile_by_index(dev_id, index, &ctrlpkt_temp);
		if (ctrlpkt_valid == 1)
		{
			if (_check_if_ctrlpkt_equal(&ctrlpkt_temp, ctrlpkt))
				return SW_ALREADY_EXIST;
		}
		else
		{
			if (entry_sign == 0) {
				entry_index = index;
				entry_sign = 1;
			}
		}
	}

	if (entry_sign == 0)
		return SW_NO_RESOURCE;

	memset(&entry, 0, sizeof(union app_ctrl_u));

	entry.bf.valid = A_TRUE;
	entry.bf.rfdb_include = ctrlpkt->rfdb_profile_bitmap?1:0;
	entry.bf.rfdb_index_bitmap_0 = (ctrlpkt->rfdb_profile_bitmap & 0x3fffffff);
	entry.bf.rfdb_index_bitmap_1 = (ctrlpkt->rfdb_profile_bitmap >> 30);

	if (ctrlpkt->protocol_types.mgt_eapol)
		entry.bf.protocol_bitmap |= (0x1 << 0);
	if (ctrlpkt->protocol_types.mgt_pppoe)
		entry.bf.protocol_bitmap |= (0x1 << 1);
	if (ctrlpkt->protocol_types.mgt_igmp)
		entry.bf.protocol_bitmap |= (0x1 << 2);
	if (ctrlpkt->protocol_types.mgt_arp_req)
		entry.bf.protocol_bitmap |= (0x1 << 3);
	if (ctrlpkt->protocol_types.mgt_arp_rep)
		entry.bf.protocol_bitmap |= (0x1 << 4);
	if (ctrlpkt->protocol_types.mgt_dhcp4)
		entry.bf.protocol_bitmap |= (0x1 << 5);
	if (ctrlpkt->protocol_types.mgt_mld)
		entry.bf.protocol_bitmap |= (0x1 << 6);
	if (ctrlpkt->protocol_types.mgt_ns)
		entry.bf.protocol_bitmap |= (0x1 << 7);
	if (ctrlpkt->protocol_types.mgt_na)
		entry.bf.protocol_bitmap |= (0x1 << 8);
	if (ctrlpkt->protocol_types.mgt_dhcp6)
		entry.bf.protocol_bitmap |= (0x1 << 9);

	entry.bf.protocol_include = entry.bf.protocol_bitmap?1:0;

	entry.bf.ethertype_include = ctrlpkt->ethtype_profile_bitmap?1:0;
	entry.bf.ethertype_index_bitmap_0 = (ctrlpkt->ethtype_profile_bitmap & 0x3);
	entry.bf.ethertype_index_bitmap_1 = (ctrlpkt->ethtype_profile_bitmap >> 2);

	entry.bf.portbitmap_include = ctrlpkt->port_map?1:0;
	entry.bf.portbitmap = ctrlpkt->port_map;

	entry.bf.in_vlan_fltr_byp = ctrlpkt->action.in_vlan_fltr_bypass?1:0;
	entry.bf.in_stg_byp = ctrlpkt->action.in_stp_bypass?1:0;
	entry.bf.l2_sec_byp = ctrlpkt->action.l2_filter_bypass?1:0;
	entry.bf.sg_byp = ctrlpkt->action.sg_bypass?1:0;
	entry.bf.cmd = (a_uint32_t)ctrlpkt->action.action;
	SW_RTN_ON_ERROR(hppe_app_ctrl_set(dev_id, entry_index, &entry));

	return SW_OK;
}

sw_error_t
adpt_hppe_mgmtctrl_ctrlpkt_profile_del(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt)
{
	a_uint32_t index, ctrlpkt_valid;
	union app_ctrl_u entry;
	fal_ctrlpkt_profile_t ctrlpkt_temp;

	memset(&entry, 0, sizeof(union app_ctrl_u));

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrlpkt);

	for (index = 0; index < APP_CTRL_MAX_ENTRY; index++)
	{
		memset(&ctrlpkt_temp, 0, sizeof(fal_ctrlpkt_profile_t));
		ctrlpkt_valid = _get_mgmtctrl_ctrlpkt_profile_by_index(dev_id, index, &ctrlpkt_temp);
		if (ctrlpkt_valid == 1)
		{
			if (_check_if_ctrlpkt_equal(&ctrlpkt_temp, ctrlpkt))
			{
				SW_RTN_ON_ERROR(hppe_app_ctrl_set(dev_id, index, &entry));
				return SW_OK;
			}
		}
	}

	return SW_NOT_FOUND;
}

sw_error_t
adpt_hppe_mgmtctrl_ctrlpkt_profile_getfirst(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt)
{
	a_uint32_t index, ctrlpkt_valid;

	for (index = 0; index < APP_CTRL_MAX_ENTRY; index++)
	{
		ctrlpkt_valid = _get_mgmtctrl_ctrlpkt_profile_by_index(dev_id, index, ctrlpkt);
		if (ctrlpkt_valid == 1)
			return SW_OK;
	}

	return SW_NO_MORE;
}

sw_error_t
adpt_hppe_mgmtctrl_ctrlpkt_profile_getnext(a_uint32_t dev_id, fal_ctrlpkt_profile_t *ctrlpkt)
{
	a_uint32_t index, ctrlpkt_valid, sign_tag;
	fal_ctrlpkt_profile_t ctrlpkt_temp;

	sign_tag = 0;

	for (index = 0; index < APP_CTRL_MAX_ENTRY; index++)
	{
		memset(&ctrlpkt_temp, 0, sizeof(fal_ctrlpkt_profile_t));
		ctrlpkt_valid = _get_mgmtctrl_ctrlpkt_profile_by_index(dev_id, index, &ctrlpkt_temp);
		if (ctrlpkt_valid == 1)
		{
			if (sign_tag == 1) {
				aos_mem_copy(ctrlpkt, &ctrlpkt_temp, sizeof(fal_ctrlpkt_profile_t));
				return SW_OK;
			}
			if (_check_if_ctrlpkt_equal(&ctrlpkt_temp, ctrlpkt))
				sign_tag = 1;
		}
	}

	return SW_NO_MORE;
}

void adpt_hppe_ctrlpkt_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_ctrlpkt_func_bitmap = ((1 << FUNC_MGMTCTRL_ETHTYPE_PROFILE_SET) |
						(1 << FUNC_MGMTCTRL_ETHTYPE_PROFILE_GET) |
						(1 << FUNC_MGMTCTRL_RFDB_PROFILE_SET) |
						(1 << FUNC_MGMTCTRL_RFDB_PROFILE_GET) |
						(1 << FUNC_MGMTCTRL_CTRLPKT_PROFILE_ADD) |
						(1 << FUNC_MGMTCTRL_CTRLPKT_PROFILE_DEL) |
						(1 << FUNC_MGMTCTRL_CTRLPKT_PROFILE_GETFIRST) |
						(1 << FUNC_MGMTCTRL_CTRLPKT_PROFILE_GETNEXT));

	return;
}

static void adpt_hppe_ctrlpkt_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_mgmtctrl_ethtype_profile_set = NULL;
	p_adpt_api->adpt_mgmtctrl_ethtype_profile_get = NULL;
	p_adpt_api->adpt_mgmtctrl_rfdb_profile_set = NULL;
	p_adpt_api->adpt_mgmtctrl_rfdb_profile_get = NULL;
	p_adpt_api->adpt_mgmtctrl_ctrlpkt_profile_add = NULL;
	p_adpt_api->adpt_mgmtctrl_ctrlpkt_profile_del = NULL;
	p_adpt_api->adpt_mgmtctrl_ctrlpkt_profile_getfirst = NULL;
	p_adpt_api->adpt_mgmtctrl_ctrlpkt_profile_getnext = NULL;

	return;
}

sw_error_t adpt_hppe_ctrlpkt_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_ctrlpkt_func_unregister(dev_id, p_adpt_api);

	if (p_adpt_api->adpt_ctrlpkt_func_bitmap & (1 << FUNC_MGMTCTRL_ETHTYPE_PROFILE_SET))
		p_adpt_api->adpt_mgmtctrl_ethtype_profile_set = adpt_hppe_mgmtctrl_ethtype_profile_set;
	if (p_adpt_api->adpt_ctrlpkt_func_bitmap & (1 << FUNC_MGMTCTRL_ETHTYPE_PROFILE_GET))
		p_adpt_api->adpt_mgmtctrl_ethtype_profile_get = adpt_hppe_mgmtctrl_ethtype_profile_get;
	if (p_adpt_api->adpt_ctrlpkt_func_bitmap & (1 << FUNC_MGMTCTRL_RFDB_PROFILE_SET))
		p_adpt_api->adpt_mgmtctrl_rfdb_profile_set = adpt_hppe_mgmtctrl_rfdb_profile_set;
	if (p_adpt_api->adpt_ctrlpkt_func_bitmap & (1 << FUNC_MGMTCTRL_RFDB_PROFILE_GET))
		p_adpt_api->adpt_mgmtctrl_rfdb_profile_get = adpt_hppe_mgmtctrl_rfdb_profile_get;
	if (p_adpt_api->adpt_ctrlpkt_func_bitmap & (1 << FUNC_MGMTCTRL_CTRLPKT_PROFILE_ADD))
		p_adpt_api->adpt_mgmtctrl_ctrlpkt_profile_add = adpt_hppe_mgmtctrl_ctrlpkt_profile_add;
	if (p_adpt_api->adpt_ctrlpkt_func_bitmap & (1 << FUNC_MGMTCTRL_CTRLPKT_PROFILE_DEL))
		p_adpt_api->adpt_mgmtctrl_ctrlpkt_profile_del = adpt_hppe_mgmtctrl_ctrlpkt_profile_del;
	if (p_adpt_api->adpt_ctrlpkt_func_bitmap & (1 << FUNC_MGMTCTRL_CTRLPKT_PROFILE_GETFIRST))
		p_adpt_api->adpt_mgmtctrl_ctrlpkt_profile_getfirst = adpt_hppe_mgmtctrl_ctrlpkt_profile_getfirst;
	if (p_adpt_api->adpt_ctrlpkt_func_bitmap & (1 << FUNC_MGMTCTRL_CTRLPKT_PROFILE_GETNEXT))
		p_adpt_api->adpt_mgmtctrl_ctrlpkt_profile_getnext = adpt_hppe_mgmtctrl_ctrlpkt_profile_getnext;

	return SW_OK;
}

/**
 * @}
 */

