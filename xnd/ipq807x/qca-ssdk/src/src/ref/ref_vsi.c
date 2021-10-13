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
#include "ref_vsi.h"
#include "ssdk_init.h"
#include "ssdk_plat.h"

#define PPE_VSI_MAX FAL_VSI_MAX
#define PPE_VSI_RESERVE_MAX 6

static ref_vsi_t ref_vsi_mapping[SW_MAX_NR_DEV][PPE_VSI_MAX+1] ={{{0, 0, NULL},
								{0, 0, NULL},
								{0, 0, NULL}}};
static a_uint32_t default_port_vsi[PPE_VSI_PPORT_NR] = {0, 1, 2, 3, 4, 5, 6};/*PPORT*/

static aos_lock_t ppe_vlan_vsi_lock[SW_MAX_NR_DEV];

static sw_error_t
_ppe_vsi_member_init(a_uint32_t dev_id, a_uint32_t vsi_id)
{
	fal_vsi_member_t vsi_member;

	aos_mem_zero(&vsi_member, sizeof(vsi_member));

	vsi_member.member_ports = 0x1;
	vsi_member.umc_ports    = 0x1;
	vsi_member.uuc_ports    = 0x1;
	vsi_member.bc_ports     = 0x1;

	fal_vsi_member_set(dev_id, vsi_id, &vsi_member);

	return SW_OK;
}

static sw_error_t
_ppe_vsi_member_update(a_uint32_t dev_id, a_uint32_t vsi_id,
				fal_port_t port_id, a_uint32_t op)
{
	sw_error_t rv;
	fal_vsi_member_t vsi_member;

	rv = fal_vsi_member_get( dev_id, vsi_id, &vsi_member);
	if( rv != SW_OK )
		return rv;
	if( PPE_VSI_DEL == op )
	{
		vsi_member.member_ports &= (~(1<<port_id));
		/*vsi_member.bc_ports &= (~(1<<port_id));
		vsi_member.umc_ports &= (~(1<<port_id));
		vsi_member.uuc_ports &= (~(1<<port_id));*/
	}
	else if( PPE_VSI_ADD == op )
	{
		vsi_member.member_ports |= (1<<port_id);
		/*vsi_member.bc_ports |= (1<<port_id);
		vsi_member.umc_ports |= (1<<port_id);
		vsi_member.uuc_ports |= (1<<port_id);*/
	}
	rv = fal_vsi_member_set(dev_id, vsi_id, &vsi_member);
	if( rv != SW_OK )
		return rv;

	return SW_OK;
}

static sw_error_t _ppe_vlan_vsi_mapping_add(a_uint32_t dev_id, fal_port_t port_id,
					a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t vsi_id)
{
	ref_vlan_info_t *p_vsi_info = NULL;
	sw_error_t rv;

	rv = fal_port_vlan_vsi_set(dev_id, port_id, stag_vid, ctag_vid, vsi_id);
	if( rv != SW_OK )
	{
		SSDK_ERROR("port %d svlan %d cvlan %d vsi %d set fail, rv = %d\n",
				port_id, stag_vid, ctag_vid, vsi_id, rv);
		return rv;
	}
	rv = _ppe_vsi_member_update(dev_id, vsi_id, port_id, PPE_VSI_ADD);
	if( rv != SW_OK )
	{
		return rv;
	}

	/*vlan based vsi update*/
	p_vsi_info = ref_vsi_mapping[dev_id][vsi_id].pHead;
	while(p_vsi_info != NULL)
	{
		if((stag_vid == p_vsi_info->stag_vid) &&
			(ctag_vid == p_vsi_info->ctag_vid))
		{
			p_vsi_info->vport_bitmap |= 1 << port_id;
			break;
		}
		p_vsi_info = p_vsi_info->pNext;
	}

	if(p_vsi_info == NULL)/*create a new entry if no match*/
	{
		SSDK_DEBUG("port %d svlan %d cvlan %d vsi %d create new entry\n",
				port_id, stag_vid, ctag_vid, vsi_id);
		p_vsi_info = aos_mem_alloc(sizeof(ref_vlan_info_t));
		if(p_vsi_info == NULL)
		{
			SSDK_ERROR("port %d svlan %d cvlan %d vsi %d aos_mem_alloc fail\n",
					port_id, stag_vid, ctag_vid, vsi_id);
			return SW_NO_RESOURCE;
		}
		p_vsi_info->vport_bitmap = 1 << port_id;
		p_vsi_info->stag_vid = stag_vid;
		p_vsi_info->ctag_vid = ctag_vid;
		p_vsi_info->pNext = (ref_vsi_mapping[dev_id][vsi_id].pHead);
		ref_vsi_mapping[dev_id][vsi_id].pHead = p_vsi_info;
	}

	return rv;
}

static sw_error_t _ppe_vlan_vsi_mapping_del(a_uint32_t dev_id, fal_port_t port_id,
					a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t vsi_id)
{
	ref_vlan_info_t *p_vsi_info = NULL;
	ref_vlan_info_t *p_prev = NULL;
	sw_error_t rv;
	a_bool_t in_vsi = 0;

	rv = fal_port_vlan_vsi_set(dev_id, port_id, stag_vid, ctag_vid, PPE_VSI_INVALID);
	if( rv != SW_OK )
	{
		SSDK_ERROR("port %d svlan %d cvlan %d vsi %d set fail, rv = %d\n",
				port_id, stag_vid, ctag_vid, vsi_id, rv);
		return rv;
	}

	/*vlan based vsi update*/
	p_vsi_info = ref_vsi_mapping[dev_id][vsi_id].pHead;
	p_prev = p_vsi_info;
	while(p_vsi_info != NULL)
	{
		if(p_vsi_info->vport_bitmap & (1 << port_id))
		{
			if((ctag_vid == p_vsi_info->ctag_vid) &&
				(stag_vid == p_vsi_info->stag_vid))
			{
				/*update software data*/
				p_vsi_info->vport_bitmap &= (~(1 << port_id));
				if(p_vsi_info->vport_bitmap == 0)/*free node if bitmap is 0*/
				{
					if(p_vsi_info == ref_vsi_mapping[dev_id][vsi_id].pHead)
					{
						ref_vsi_mapping[dev_id][vsi_id].pHead =
								p_vsi_info->pNext;
					}
					else
					{
						p_prev->pNext = p_vsi_info->pNext;
					}
					aos_mem_free(p_vsi_info);
					p_vsi_info = NULL;
					break;
				}
			}
			else
			{
				in_vsi = 1;/*port + another vlan --> vsi*/
			}
		}
		p_prev = p_vsi_info;
		p_vsi_info = p_vsi_info->pNext;
	}

	if(in_vsi == 0 &&
		(!(1 << port_id & ref_vsi_mapping[dev_id][vsi_id].pport_bitmap)))
	{
		rv = _ppe_vsi_member_update(dev_id, vsi_id, port_id, PPE_VSI_DEL);
		if( rv != SW_OK )
		{
			SSDK_ERROR("port %d svlan %d cvlan %d vsi %d fail, rv = %d\n",
					port_id, stag_vid, ctag_vid, vsi_id, rv);
			return rv;
		}
	}

	return SW_OK;
}

static sw_error_t _ppe_port_vsi_mapping_update(a_uint32_t dev_id,
					fal_port_t port_id, a_uint32_t vsi_id)
{
	ref_vsi_t *p_vsi = NULL;
	sw_error_t rv;
	a_uint32_t i = 0;


	if(ref_vsi_mapping[dev_id][vsi_id].valid == 0)
	{
		SSDK_ERROR("port %d vsi %d entry not found\n", port_id, vsi_id);
		return SW_NOT_FOUND;
	}

	/*check port previous vsi*/
	for( i = 0; i <= PPE_VSI_MAX; i++ )
	{
		p_vsi = &(ref_vsi_mapping[dev_id][i]);
		if(p_vsi->valid != 0 &&	(p_vsi->pport_bitmap & (1 << port_id)))
		{
			/*remmove from preious vsi*/
			rv = _ppe_vsi_member_update(dev_id, i, port_id, PPE_VSI_DEL);
			if( rv != SW_OK )
				return rv;
			p_vsi->pport_bitmap &= (~(1<<port_id));
		}
	}
	SSDK_DEBUG("port %d, vsi %d set\n", port_id, vsi_id);

	/*port based vsi update*/
	rv = _ppe_vsi_member_update(dev_id, vsi_id, port_id, PPE_VSI_ADD);
	if( rv != SW_OK )
		return rv;
	ref_vsi_mapping[dev_id][vsi_id].pport_bitmap |= 1 << port_id;

	return SW_OK;
}


sw_error_t
ppe_port_vlan_vsi_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t vsi_id)
{
	sw_error_t rv;
	a_uint32_t cur_vsi = PPE_VSI_INVALID;

	REF_DEV_ID_CHECK(dev_id);

	SSDK_DEBUG("port %d svlan %d cvlan %d vsi %d \n", 
			port_id, stag_vid, ctag_vid, vsi_id);

	if((vsi_id != PPE_VSI_INVALID) &&
		(ref_vsi_mapping[dev_id][vsi_id].valid == 0))
	{
		SSDK_ERROR("port %d svlan %d cvlan %d vsi %d entry not found\n",
				port_id, stag_vid, ctag_vid, vsi_id);
		return SW_NOT_FOUND;
	}

	ppe_port_vlan_vsi_get(dev_id, port_id, stag_vid, ctag_vid, &cur_vsi);
	if(cur_vsi == vsi_id)
		return SW_OK;
	aos_lock_bh(&ppe_vlan_vsi_lock[dev_id]);
	if(PPE_VSI_INVALID == vsi_id || cur_vsi != PPE_VSI_INVALID)
	{
		SSDK_DEBUG("Deleting port %d svlan %d cvlan %d vsi %d\n",
				port_id, stag_vid, ctag_vid, cur_vsi);
		rv = _ppe_vlan_vsi_mapping_del(dev_id, port_id, stag_vid, ctag_vid, cur_vsi);
		if( rv != SW_OK )
		{
			aos_unlock_bh(&ppe_vlan_vsi_lock[dev_id]);
			return rv;
		}
	}

	if(PPE_VSI_INVALID != vsi_id)
	{
		SSDK_DEBUG("Adding port %d svlan %d cvlan %d vsi %d\n",
				port_id, stag_vid, ctag_vid, vsi_id);

		rv = _ppe_vlan_vsi_mapping_add(dev_id, port_id, stag_vid, ctag_vid, vsi_id);
	}
	aos_unlock_bh(&ppe_vlan_vsi_lock[dev_id]);
	return rv;
}

sw_error_t ppe_port_vlan_vsi_get(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t stag_vid, a_uint32_t ctag_vid, a_uint32_t *vsi_id)
{
	ref_vlan_info_t *p_vsi_info = NULL;
	a_uint32_t i = 0;

	SSDK_DEBUG("Getting port %d svlan %d cvlan %d\n", port_id, stag_vid, ctag_vid);

	REF_DEV_ID_CHECK(dev_id);
	REF_NULL_POINT_CHECK(vsi_id);

	aos_lock_bh(&ppe_vlan_vsi_lock[dev_id]);
	for( i = 0; i <= PPE_VSI_MAX; i++ )
	{
		p_vsi_info = ref_vsi_mapping[dev_id][i].pHead;
		while(p_vsi_info != NULL)
		{
			if((p_vsi_info->vport_bitmap & (1 << port_id)) &&
				(ctag_vid== p_vsi_info->ctag_vid) &&
				(stag_vid== p_vsi_info->stag_vid))
			{
				*vsi_id = i;
				SSDK_DEBUG("Returned port %d svlan %d cvlan %d vsi %d\n",
						port_id, stag_vid, ctag_vid, *vsi_id);
				aos_unlock_bh(&ppe_vlan_vsi_lock[dev_id]);

				return SW_OK;
			}
			p_vsi_info = p_vsi_info->pNext;
		}
	}
	aos_unlock_bh(&ppe_vlan_vsi_lock[dev_id]);
	return SW_NOT_FOUND;
}


/*called when
	1. switchdev create physical interface for port
	2. add physical interface to a bridge*/
sw_error_t
ppe_port_vsi_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t vsi_id)
{
	sw_error_t rv = SW_OK;

	REF_DEV_ID_CHECK(dev_id);
	SSDK_DEBUG("port %d, vsi %d\n", port_id, vsi_id);
	if(!(FAL_IS_PPORT(port_id)) && !(FAL_IS_VPORT(port_id)))
		return SW_BAD_VALUE;

	if(vsi_id > PPE_VSI_MAX){
		SSDK_ERROR("invalid VSI port %d, vsi %d\n", port_id, vsi_id);
		return SW_BAD_VALUE;
	}

	if(FAL_IS_PPORT(port_id)){
		rv = _ppe_port_vsi_mapping_update(dev_id, FAL_PORT_ID_VALUE(port_id), vsi_id);
		if( rv != SW_OK )
			return rv;
	}
	return rv;
}

sw_error_t
ppe_port_vsi_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t *vsi_id)
{
	if(FAL_IS_PPORT(port_id))
	{
		a_uint32_t i = 0;
		for( i = 0; i <= PPE_VSI_MAX; i++ )
		{
			if((ref_vsi_mapping[dev_id][i].valid != 0)&&
				(ref_vsi_mapping[dev_id][i].pport_bitmap & (1 << port_id)))
			{
				*vsi_id = i;
				SSDK_DEBUG("returned port %d, vsi %d\n", port_id, *vsi_id);
				return SW_OK;
			}
		}
	}
	SSDK_ERROR("VSI is not configured on port %d\n", port_id);
	return SW_NOT_FOUND;
}

sw_error_t ppe_vsi_alloc(a_uint32_t dev_id, a_uint32_t *vsi)
{
	a_uint32_t vsi_id;

	REF_DEV_ID_CHECK(dev_id);
	REF_NULL_POINT_CHECK(vsi);
	SSDK_DEBUG("requesting vsi\n");

	for( vsi_id = PPE_VSI_RESERVE_MAX+1; vsi_id <= PPE_VSI_MAX; vsi_id++ )
	{
		if(ref_vsi_mapping[dev_id][vsi_id].valid == 0)
		{
			fal_vsi_newaddr_lrn_t newaddr_lrn;
			fal_vsi_stamove_t stamove;
			ref_vsi_mapping[dev_id][vsi_id].valid = 1;
  			ref_vsi_mapping[dev_id][vsi_id].pport_bitmap = 0;
  			ref_vsi_mapping[dev_id][vsi_id].pHead = NULL;
			*vsi = vsi_id;
			_ppe_vsi_member_init(dev_id, vsi_id);
			newaddr_lrn.lrn_en = 1;
			newaddr_lrn.action = 0;
			fal_vsi_newaddr_lrn_set(dev_id, vsi_id, &newaddr_lrn);
			stamove.stamove_en = 1;
			stamove.action = 0;
			fal_vsi_stamove_set(dev_id, vsi_id, &stamove);
			SSDK_DEBUG("vsi %d allocated\n", *vsi);
			return SW_OK;
		}
	}

	return SW_NO_RESOURCE;
}

sw_error_t ppe_vsi_free(a_uint32_t dev_id, a_uint32_t vsi_id)
{
	ref_vlan_info_t *p_vsi_info = NULL;
	REF_DEV_ID_CHECK(dev_id);

	if( vsi_id <= PPE_VSI_RESERVE_MAX || vsi_id > PPE_VSI_MAX )
		return SW_OUT_OF_RANGE;

	p_vsi_info = ref_vsi_mapping[dev_id][vsi_id].pHead;
	while(p_vsi_info != NULL)
	{
		ref_vlan_info_t *ptemp = p_vsi_info;
		p_vsi_info = p_vsi_info->pNext;
		SSDK_DEBUG("port 0x%x svlan %d cvlan %d, vsi %d free vsi info\n",
				ptemp->vport_bitmap, ptemp->stag_vid, ptemp->ctag_vid, vsi_id);
		aos_mem_free(ptemp);
	}
	ref_vsi_mapping[dev_id][vsi_id].valid = 0;
	ref_vsi_mapping[dev_id][vsi_id].pHead = NULL;
	ref_vsi_mapping[dev_id][vsi_id].pport_bitmap = 0;

	SSDK_DEBUG("vsi %d released\n", vsi_id);

	return SW_OK;
}

static void ppe_init_one_vsi(a_uint32_t dev_id, a_uint32_t vsi_id)
{
	if(ref_vsi_mapping[dev_id][vsi_id].valid == 0)
	{
		ref_vsi_mapping[dev_id][vsi_id].valid = 1;
		ref_vsi_mapping[dev_id][vsi_id].pport_bitmap = 0;
		ref_vsi_mapping[dev_id][vsi_id].pHead = NULL;
		_ppe_vsi_member_init(dev_id, vsi_id);
	}

	return;
}

sw_error_t ppe_vsi_init(a_uint32_t dev_id)
{
	fal_port_t port_id;
	fal_vsi_newaddr_lrn_t newaddr_lrn = {0};
	fal_vsi_stamove_t stamove = {0};

	newaddr_lrn.action = 0;
	newaddr_lrn.lrn_en = 1;
	stamove.action = 0;
	stamove.stamove_en = 1;
	for(port_id = SSDK_PHYSICAL_PORT1; port_id <= SSDK_PHYSICAL_PORT7; port_id++)
	{
		ppe_init_one_vsi(dev_id, default_port_vsi[port_id-1]);
		fal_vsi_newaddr_lrn_set(dev_id, default_port_vsi[port_id-1], &newaddr_lrn);
		fal_vsi_stamove_set(dev_id, default_port_vsi[port_id-1], &stamove);
		/*fal_port_vsi_set(0, port_id, default_port_vsi[port_id-1]);*/
		ppe_port_vsi_set(dev_id, port_id, default_port_vsi[port_id-1]);
	}

	aos_lock_init(&ppe_vlan_vsi_lock[dev_id]);

	return SW_OK;
}


sw_error_t ppe_vsi_tbl_dump(a_uint32_t dev_id)
{
	a_uint32_t vsi_id;
	ref_vlan_info_t *p_vsi_info = NULL;

	REF_DEV_ID_CHECK(dev_id);
	printk("########Software VSI mapping table\n");
	for( vsi_id = 0; vsi_id <= PPE_VSI_MAX; vsi_id++ )
	{
		if(ref_vsi_mapping[dev_id][vsi_id].valid == 0)
			continue;
		p_vsi_info = ref_vsi_mapping[dev_id][vsi_id].pHead;
		printk("vsi %d, port bitmap 0x%x\n",vsi_id, ref_vsi_mapping[dev_id][vsi_id].pport_bitmap);
		while(p_vsi_info != NULL)
		{
			printk("%8s svlan %d, cvlan %d, port bitmap 0x%x\n","",
				p_vsi_info->stag_vid, p_vsi_info->ctag_vid, p_vsi_info->vport_bitmap);
			p_vsi_info = p_vsi_info->pNext;
		}
	}

	return SW_OK;
}

EXPORT_SYMBOL(ppe_port_vlan_vsi_set);
EXPORT_SYMBOL(ppe_port_vlan_vsi_get);
EXPORT_SYMBOL(ppe_port_vsi_set);
EXPORT_SYMBOL(ppe_port_vsi_get);
EXPORT_SYMBOL(ppe_vsi_alloc);
EXPORT_SYMBOL(ppe_vsi_free);


