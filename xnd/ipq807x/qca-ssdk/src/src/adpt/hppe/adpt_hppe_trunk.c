/*
 * Copyright (c) 2016-2017, 2020, The Linux Foundation. All rights reserved.
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

#include "hppe_trunk_reg.h"
#include "hppe_trunk.h"
#include "adpt.h"
#include "hppe_fdb_reg.h"
#include "hppe_fdb.h"

sw_error_t
adpt_hppe_trunk_fail_over_en_get(a_uint32_t dev_id, a_bool_t * fail_over)
{
	union l2_global_conf_u l2_global_conf;

	memset(&l2_global_conf, 0, sizeof(l2_global_conf));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(fail_over);

	SW_RTN_ON_ERROR(hppe_l2_global_conf_get(dev_id, &l2_global_conf));

	*fail_over = l2_global_conf.bf.failover_en;

	return SW_OK;
}

sw_error_t
adpt_hppe_trunk_hash_mode_get(a_uint32_t dev_id, a_uint32_t * hash_mode)
{
	union trunk_hash_field_reg_u trunk_hash_field;

	memset(&trunk_hash_field, 0, sizeof(trunk_hash_field));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(hash_mode);

	SW_RTN_ON_ERROR(hppe_trunk_hash_field_reg_get(dev_id, &trunk_hash_field));

	*hash_mode = 0;

	if (trunk_hash_field.bf.mac_da_incl)
		*hash_mode |= FAL_TRUNK_HASH_KEY_DA;

	if (trunk_hash_field.bf.mac_sa_incl)
		*hash_mode |= FAL_TRUNK_HASH_KEY_SA;

	if (trunk_hash_field.bf.src_ip_incl)
		*hash_mode |= FAL_TRUNK_HASH_KEY_SIP;

	if (trunk_hash_field.bf.dst_ip_incl)
		*hash_mode |= FAL_TRUNK_HASH_KEY_DIP;

	if (trunk_hash_field.bf.src_port_incl)
		*hash_mode |= FAL_TRUNK_HASH_KEY_SRC_PORT;

	if (trunk_hash_field.bf.l4_src_port_incl)
		*hash_mode |= FAL_TRUNK_HASH_KEY_L4_SRC_PORT;

	if (trunk_hash_field.bf.l4_dst_port_incl)
		*hash_mode |= FAL_TRUNK_HASH_KEY_L4_DST_PORT;

	if (trunk_hash_field.bf.udf0_incl)
		*hash_mode |= FAL_TRUNK_HASH_KEY_UDF0;

	if (trunk_hash_field.bf.udf1_incl)
		*hash_mode |= FAL_TRUNK_HASH_KEY_UDF1;

	if (trunk_hash_field.bf.udf2_incl)
		*hash_mode |= FAL_TRUNK_HASH_KEY_UDF2;

	if (trunk_hash_field.bf.udf3_incl)
		*hash_mode |= FAL_TRUNK_HASH_KEY_UDF3;

	return SW_OK;
}

sw_error_t
adpt_hppe_trunk_group_get(a_uint32_t dev_id, a_uint32_t trunk_id,
                        a_bool_t * enable, fal_pbmp_t * member)
{
	union trunk_filter_u trunk_filter;
	union port_trunk_id_u port_trunk_id;
	a_uint32_t port_id;

	memset(&trunk_filter, 0, sizeof(trunk_filter));
	memset(&port_trunk_id, 0, sizeof(port_trunk_id));

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);
	ADPT_NULL_POINT_CHECK(member);

	*enable = A_FALSE;
	for (port_id = 0; port_id < FAL_MAX_PORT_NUMBER; port_id ++)
	{
		SW_RTN_ON_ERROR(hppe_port_trunk_id_get(dev_id, port_id, &port_trunk_id));
		if ((trunk_id == port_trunk_id.bf.trunk_id) && (A_TRUE == port_trunk_id.bf.trunk_en))
		{
			*enable = A_TRUE;
			break;
		}
	}

	SW_RTN_ON_ERROR(hppe_trunk_filter_get(dev_id, trunk_id, &trunk_filter));

	*member = trunk_filter.bf.mem_bitmap;

	return SW_OK;
}

sw_error_t
adpt_hppe_trunk_group_set(a_uint32_t dev_id, a_uint32_t trunk_id,
                        a_bool_t enable, fal_pbmp_t member)
{
	union trunk_filter_u trunk_filter;
	union port_trunk_id_u port_trunk_id;
	union trunk_member_u trunk_member;
	a_uint32_t i, j, cnt = 0, data[FAL_MAX_PORT_NUMBER] = {0};

	memset(&trunk_filter, 0, sizeof(trunk_filter));
	memset(&port_trunk_id, 0, sizeof(port_trunk_id));
	memset(&trunk_member, 0, sizeof(trunk_member));

	ADPT_DEV_ID_CHECK(dev_id);

	if (trunk_id >= TRUNK_FILTER_MAX_ENTRY)
	{
		return SW_OUT_OF_RANGE;
	}

	if(enable == A_TRUE && member == 0)
	{
		SSDK_ERROR("trunk member cannot be 0 when trunk group was enabled\n");
		return SW_BAD_PARAM;
	}

	if (A_TRUE == enable)
	{
		for (i = 0; i < FAL_MAX_PORT_NUMBER; i++)
		{
			if (member & (0x1 << i))
			{
				if (FAL_TRUNK_GROUP_MAX_MEMEBER <= cnt)
				{
					return SW_BAD_PARAM;
				}
				data[cnt] = i;
				cnt++;
			}
		}
	}

	for (i = 0; i < FAL_MAX_PORT_NUMBER; i++)
	{
		SW_RTN_ON_ERROR(hppe_port_trunk_id_get(dev_id, i, &port_trunk_id));
		if (port_trunk_id.bf.trunk_id == trunk_id) {
			port_trunk_id.bf.trunk_en = A_FALSE;
			SW_RTN_ON_ERROR(hppe_port_trunk_id_set(dev_id, i, &port_trunk_id));
		}
	}

	for (j = 0; j < FAL_MAX_PORT_NUMBER; j++)
	{
		if (member & (0x1 << j))
		{
			port_trunk_id.bf.trunk_en = enable;
			port_trunk_id.bf.trunk_id = trunk_id;
			SW_RTN_ON_ERROR(hppe_port_trunk_id_set(dev_id, j, &port_trunk_id));
		}
	}

	if (A_TRUE == enable)
		trunk_filter.bf.mem_bitmap = member;
	else
		trunk_filter.bf.mem_bitmap = 0;

	SW_RTN_ON_ERROR(hppe_trunk_filter_set(dev_id, trunk_id, &trunk_filter));

	if (A_TRUE == enable)
	{
		for(i = SSDK_PHYSICAL_PORT0; i <= SSDK_PHYSICAL_PORT7; i+=cnt)
		{
			for(j = 0; j < cnt; j++)
			{
				if((i+j) < FAL_MAX_PORT_NUMBER)
				{
					data[i+j] = data[j];
				}
			}
		}
	}
	trunk_member.bf.member_0_port_id = data[0];
	trunk_member.bf.member_1_port_id = data[1];
	trunk_member.bf.member_2_port_id = data[2];
	trunk_member.bf.member_3_port_id = data[3];
	trunk_member.bf.member_4_port_id = data[4];
	trunk_member.bf.member_5_port_id = data[5];
	trunk_member.bf.member_6_port_id = data[6];
	trunk_member.bf.member_7_port_id = data[7];

	SW_RTN_ON_ERROR(hppe_trunk_member_set(dev_id, trunk_id, &trunk_member));

	return SW_OK;
}
sw_error_t
adpt_hppe_trunk_fail_over_en_set(a_uint32_t dev_id, a_bool_t fail_over)
{
	union l2_global_conf_u l2_global_conf;

	memset(&l2_global_conf, 0, sizeof(l2_global_conf));
	ADPT_DEV_ID_CHECK(dev_id);

	SW_RTN_ON_ERROR(hppe_l2_global_conf_get(dev_id, &l2_global_conf));

	l2_global_conf.bf.failover_en = fail_over;

	SW_RTN_ON_ERROR(hppe_l2_global_conf_set(dev_id, &l2_global_conf));

	return SW_OK;
}
sw_error_t
adpt_hppe_trunk_hash_mode_set(a_uint32_t dev_id, a_uint32_t hash_mode)
{
	union trunk_hash_field_reg_u trunk_hash_field;

	memset(&trunk_hash_field, 0, sizeof(trunk_hash_field));
	ADPT_DEV_ID_CHECK(dev_id);

	if (FAL_TRUNK_HASH_KEY_DA & hash_mode)
		trunk_hash_field.bf.mac_da_incl = 1;

	if (FAL_TRUNK_HASH_KEY_SA & hash_mode)
		trunk_hash_field.bf.mac_sa_incl = 1;

	if (FAL_TRUNK_HASH_KEY_DIP & hash_mode)
		trunk_hash_field.bf.dst_ip_incl = 1;

	if (FAL_TRUNK_HASH_KEY_SIP & hash_mode)
		trunk_hash_field.bf.src_ip_incl = 1;

	if (FAL_TRUNK_HASH_KEY_SRC_PORT & hash_mode)
		trunk_hash_field.bf.src_port_incl = 1;

	if (FAL_TRUNK_HASH_KEY_L4_SRC_PORT & hash_mode)
		trunk_hash_field.bf.l4_src_port_incl = 1;

	if (FAL_TRUNK_HASH_KEY_L4_DST_PORT & hash_mode)
		trunk_hash_field.bf.l4_dst_port_incl = 1;

	if (FAL_TRUNK_HASH_KEY_UDF0 & hash_mode)
		trunk_hash_field.bf.udf0_incl = 1;

	if (FAL_TRUNK_HASH_KEY_UDF1 & hash_mode)
		trunk_hash_field.bf.udf1_incl = 1;

	if (FAL_TRUNK_HASH_KEY_UDF2 & hash_mode)
		trunk_hash_field.bf.udf2_incl = 1;

	if (FAL_TRUNK_HASH_KEY_UDF3 & hash_mode)
		trunk_hash_field.bf.udf3_incl = 1;

	SW_RTN_ON_ERROR(hppe_trunk_hash_field_reg_set(dev_id, &trunk_hash_field));

	return SW_OK;
}

void adpt_hppe_trunk_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_trunk_func_bitmap = ((1 << FUNC_TRUNK_GROUP_SET) |
						(1 << FUNC_TRUNK_GROUP_GET) |
						(1 << FUNC_TRUNK_HASH_MODE_SET) |
						(1 << FUNC_TRUNK_HASH_MODE_GET) |
						(1 << FUNC_TRUNK_FAILOVER_ENABLE) |
						(1 << FUNC_TRUNK_FAILOVER_STATUS_GET));

	return;
}

static void adpt_hppe_trunk_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_trunk_fail_over_en_get = NULL;
	p_adpt_api->adpt_trunk_hash_mode_get = NULL;
	p_adpt_api->adpt_trunk_group_get = NULL;
	p_adpt_api->adpt_trunk_group_set = NULL;
	p_adpt_api->adpt_trunk_fail_over_en_set = NULL;
	p_adpt_api->adpt_trunk_hash_mode_set = NULL;

	return;
}

sw_error_t adpt_hppe_trunk_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_trunk_func_unregister(dev_id, p_adpt_api);

	if (p_adpt_api->adpt_trunk_func_bitmap & (1 << FUNC_TRUNK_FAILOVER_STATUS_GET))
		p_adpt_api->adpt_trunk_fail_over_en_get = adpt_hppe_trunk_fail_over_en_get;
	if (p_adpt_api->adpt_trunk_func_bitmap & (1 << FUNC_TRUNK_HASH_MODE_GET))
		p_adpt_api->adpt_trunk_hash_mode_get = adpt_hppe_trunk_hash_mode_get;
	if (p_adpt_api->adpt_trunk_func_bitmap & (1 << FUNC_TRUNK_GROUP_GET))
		p_adpt_api->adpt_trunk_group_get = adpt_hppe_trunk_group_get;
	if (p_adpt_api->adpt_trunk_func_bitmap & (1 << FUNC_TRUNK_GROUP_SET))
		p_adpt_api->adpt_trunk_group_set = adpt_hppe_trunk_group_set;
	if (p_adpt_api->adpt_trunk_func_bitmap & (1 << FUNC_TRUNK_FAILOVER_ENABLE))
		p_adpt_api->adpt_trunk_fail_over_en_set = adpt_hppe_trunk_fail_over_en_set;
	if (p_adpt_api->adpt_trunk_func_bitmap & (1 << FUNC_TRUNK_HASH_MODE_SET))
		p_adpt_api->adpt_trunk_hash_mode_set = adpt_hppe_trunk_hash_mode_set;

	return SW_OK;
}

/**
 * @}
 */
