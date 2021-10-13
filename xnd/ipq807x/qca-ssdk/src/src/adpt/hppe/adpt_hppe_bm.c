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
#include "fal_bm.h"
#include "hppe_bm_reg.h"
#include "hppe_bm.h"
#include "hppe_portctrl_reg.h"
#include "hppe_portctrl.h"
#include "adpt.h"

#ifndef IN_BM_MINI
sw_error_t
adpt_hppe_port_bufgroup_map_get(a_uint32_t dev_id, fal_port_t port,
			a_uint8_t *group)
{
	sw_error_t rv = SW_OK;
	union port_group_id_u port_group_id;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(group);
	memset(&port_group_id, 0, sizeof(port_group_id));

	rv = hppe_port_group_id_get(dev_id, port, &port_group_id);
	if( rv != SW_OK )
		return rv;

	*group = port_group_id.bf.port_shared_group_id;

	return SW_OK;
}

sw_error_t
adpt_hppe_bm_port_reserved_buffer_get(a_uint32_t dev_id, fal_port_t port,
			a_uint16_t *prealloc_buff, a_uint16_t *react_buff)
{
	sw_error_t rv = SW_OK;
	union port_fc_cfg_u port_fc_cfg;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(prealloc_buff);
	ADPT_NULL_POINT_CHECK(react_buff);
	memset(&port_fc_cfg, 0, sizeof(port_fc_cfg));

	rv = hppe_port_fc_cfg_get(dev_id, port, &port_fc_cfg);
	if (rv)
		return rv;

	*prealloc_buff = port_fc_cfg.bf.port_pre_alloc;
	*react_buff = port_fc_cfg.bf.port_react_limit;

	return SW_OK;
}

sw_error_t
adpt_hppe_bm_bufgroup_buffer_get(a_uint32_t dev_id, a_uint8_t group,
			a_uint16_t *buff_num)
{
	sw_error_t rv = SW_OK;
	union shared_group_cfg_u shared_group_cfg;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(buff_num);
	memset(&shared_group_cfg, 0, sizeof(shared_group_cfg));

	rv = hppe_shared_group_cfg_get(dev_id, group, &shared_group_cfg);
	if( rv != SW_OK )
		return rv;

	*buff_num = shared_group_cfg.bf.shared_group_limit;

	return SW_OK;
}

sw_error_t
adpt_hppe_bm_port_dynamic_thresh_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_dynamic_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;
	union port_fc_cfg_u port_fc_cfg;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cfg);
	memset(&port_fc_cfg, 0, sizeof(port_fc_cfg));

	rv = hppe_port_fc_cfg_get(dev_id, port, &port_fc_cfg);
	if( rv != SW_OK )
		return rv;

	if (!port_fc_cfg.bf.port_shared_dynamic)
		return SW_FAIL;

	cfg->weight = port_fc_cfg.bf.port_shared_weight;
	cfg->shared_ceiling = port_fc_cfg.bf.port_shared_ceiling_0 |
				port_fc_cfg.bf.port_shared_ceiling_1 << 3;
	cfg->resume_off = port_fc_cfg.bf.port_resume_offset;
	cfg->resume_min_thresh = port_fc_cfg.bf.port_resume_floor_th;

	return SW_OK;
}

sw_error_t
adpt_hppe_port_bm_ctrl_get(a_uint32_t dev_id, fal_port_t port, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	union port_fc_mode_u port_fc_mode;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);
	memset(&port_fc_mode, 0, sizeof(port_fc_mode));

	rv = hppe_port_fc_mode_get(dev_id, port, &port_fc_mode);
	if( rv != SW_OK )
		return rv;

	*enable =  port_fc_mode.bf.fc_en;

	return SW_OK;
}
#endif

sw_error_t
adpt_hppe_bm_bufgroup_buffer_set(a_uint32_t dev_id, a_uint8_t group,
			a_uint16_t buff_num)
{
	sw_error_t rv = SW_OK;
	union shared_group_cfg_u shared_group_cfg;

	ADPT_DEV_ID_CHECK(dev_id);
	memset(&shared_group_cfg, 0, sizeof(shared_group_cfg));

	shared_group_cfg.bf.shared_group_limit = buff_num;
	rv = hppe_shared_group_cfg_set(dev_id, group, &shared_group_cfg);
	if( rv != SW_OK )
		return rv;

	return SW_OK;
}

sw_error_t
adpt_hppe_port_bufgroup_map_set(a_uint32_t dev_id, fal_port_t port,
			a_uint8_t group)
{
	sw_error_t rv = SW_OK;
	union port_group_id_u port_group_id;

	ADPT_DEV_ID_CHECK(dev_id);
	memset(&port_group_id, 0, sizeof(port_group_id));

	port_group_id.bf.port_shared_group_id = group;
	rv = hppe_port_group_id_set(dev_id, port, &port_group_id);
	if( rv != SW_OK )
		return rv;

	return SW_OK;
}

#ifndef IN_BM_MINI
sw_error_t
adpt_hppe_bm_port_static_thresh_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_static_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;
	union port_fc_cfg_u port_fc_cfg;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cfg);
	memset(&port_fc_cfg, 0, sizeof(port_fc_cfg));

	rv = hppe_port_fc_cfg_get(dev_id, port, &port_fc_cfg);
	if( rv != SW_OK )
		return rv;

	if (port_fc_cfg.bf.port_shared_dynamic)
		return SW_FAIL;

	cfg->resume_off = port_fc_cfg.bf.port_resume_offset;
	cfg->max_thresh = port_fc_cfg.bf.port_shared_ceiling_0 |
			port_fc_cfg.bf.port_shared_ceiling_1 << 3;

	return SW_OK;
}
#endif

sw_error_t
adpt_hppe_bm_port_reserved_buffer_set(a_uint32_t dev_id, fal_port_t port,
			a_uint16_t prealloc_buff, a_uint16_t react_buff)
{
	sw_error_t rv = SW_OK;
	union port_fc_cfg_u port_fc_cfg;

	ADPT_DEV_ID_CHECK(dev_id);
	memset(&port_fc_cfg, 0, sizeof(port_fc_cfg));

	rv = hppe_port_fc_cfg_get(dev_id, port, &port_fc_cfg);
	if (rv)
		return rv;

	port_fc_cfg.bf.port_pre_alloc = prealloc_buff;
	port_fc_cfg.bf.port_react_limit = react_buff;

	return hppe_port_fc_cfg_set(dev_id, port, &port_fc_cfg);
}

sw_error_t
adpt_hppe_bm_port_static_thresh_set(a_uint32_t dev_id, fal_port_t port,
			fal_bm_static_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;
	union port_fc_cfg_u port_fc_cfg;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cfg);
	memset(&port_fc_cfg, 0, sizeof(port_fc_cfg));

	rv = hppe_port_fc_cfg_get(dev_id, port, &port_fc_cfg);
	if( rv != SW_OK )
		return rv;

	port_fc_cfg.bf.port_resume_offset = cfg->resume_off;
	port_fc_cfg.bf.port_shared_ceiling_0 = cfg->max_thresh;
	port_fc_cfg.bf.port_shared_ceiling_1 = cfg->max_thresh >> 3;
	port_fc_cfg.bf.port_shared_dynamic = 0;

	return hppe_port_fc_cfg_set(dev_id, port, &port_fc_cfg);;
}

sw_error_t
adpt_hppe_bm_port_dynamic_thresh_set(a_uint32_t dev_id, fal_port_t port,
			fal_bm_dynamic_cfg_t *cfg)
{
	sw_error_t rv = SW_OK;
	union port_fc_cfg_u port_fc_cfg;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cfg);
	memset(&port_fc_cfg, 0, sizeof(port_fc_cfg));

	rv = hppe_port_fc_cfg_get(dev_id, port, &port_fc_cfg);
	if( rv != SW_OK )
		return rv;

	port_fc_cfg.bf.port_shared_weight = cfg->weight;
	port_fc_cfg.bf.port_shared_ceiling_0 = cfg->shared_ceiling;
	port_fc_cfg.bf.port_shared_ceiling_1 = cfg->shared_ceiling >> 3;
	port_fc_cfg.bf.port_resume_offset = cfg->resume_off;
	port_fc_cfg.bf.port_resume_floor_th = cfg->resume_min_thresh;
	port_fc_cfg.bf.port_shared_dynamic = 1;

	return hppe_port_fc_cfg_set(dev_id, port, &port_fc_cfg);;
}

sw_error_t
adpt_hppe_port_bm_ctrl_set(a_uint32_t dev_id, fal_port_t port, a_bool_t enable)
{
	union port_fc_mode_u port_fc_mode;

	ADPT_DEV_ID_CHECK(dev_id);
	memset(&port_fc_mode, 0, sizeof(port_fc_mode));

	port_fc_mode.bf.fc_en = enable;
	return hppe_port_fc_mode_set(dev_id, port, &port_fc_mode);
}

sw_error_t
adpt_hppe_port_tdm_ctrl_set(a_uint32_t dev_id, fal_port_tdm_ctrl_t *ctrl)
{
	union tdm_ctrl_u tdm_ctrl;

	ADPT_DEV_ID_CHECK(dev_id);
	memset(&tdm_ctrl, 0, sizeof(tdm_ctrl));

	tdm_ctrl.bf.tdm_en = ctrl->enable;
	tdm_ctrl.bf.tdm_offset = ctrl->offset;
	tdm_ctrl.bf.tdm_depth = ctrl->depth;
	return hppe_tdm_ctrl_set(dev_id, &tdm_ctrl);
}

sw_error_t
adpt_hppe_port_tdm_tick_cfg_set(a_uint32_t dev_id, a_uint32_t tick_index,
			fal_port_tdm_tick_cfg_t *cfg)
{
	union tdm_cfg_u tdm_cfg;

	ADPT_DEV_ID_CHECK(dev_id);
	memset(&tdm_cfg, 0, sizeof(tdm_cfg));

	tdm_cfg.bf.valid = cfg->valid;
	tdm_cfg.bf.dir = cfg->direction;
	tdm_cfg.bf.port_num = cfg->port;
	return hppe_tdm_cfg_set(dev_id, tick_index, &tdm_cfg);
}

#ifndef IN_BM_MINI
sw_error_t
adpt_hppe_bm_port_counter_get(a_uint32_t dev_id, fal_port_t port,
			fal_bm_port_counter_t *counter)
{
	sw_error_t rv = SW_OK;
	union port_cnt_u port_cnt;
	union port_reacted_cnt_u reacted_cnt;
	union drop_stat_u drop_stat;
	a_uint32_t index = FAL_PORT_ID_VALUE(port);

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(counter);
	memset(&port_cnt, 0, sizeof(port_cnt));
	memset(&reacted_cnt, 0, sizeof(reacted_cnt));

	rv = hppe_port_cnt_get(dev_id, index, &port_cnt);
	if( rv != SW_OK )
		return rv;
	counter->used_counter = port_cnt.bf.port_cnt;

	rv = hppe_port_reacted_cnt_get(dev_id, index, &reacted_cnt);
	if( rv != SW_OK )
		return rv;
	counter->react_counter = reacted_cnt.bf.port_reacted_cnt;

	rv = hppe_drop_stat_get(dev_id, index, &drop_stat);
	if( rv != SW_OK )
		return rv;
	counter->drop_byte_counter = drop_stat.bf.bytes_0 | ((a_uint64_t)drop_stat.bf.bytes_1 << 32);
	counter->drop_packet_counter = drop_stat.bf.pkts;
	rv = hppe_drop_stat_get(dev_id, index + 15, &drop_stat);
	if( rv != SW_OK )
		return rv;
	counter->fc_drop_byte_counter = drop_stat.bf.bytes_0 | ((a_uint64_t)drop_stat.bf.bytes_1 << 32);
	counter->fc_drop_packet_counter = drop_stat.bf.pkts;

	return SW_OK;
}
#endif

void adpt_hppe_bm_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_bm_func_bitmap = ((1 << FUNC_PORT_BUFGROUP_MAP_GET) |
						(1 << FUNC_BM_PORT_RESERVED_BUFFER_GET) |
						(1 << FUNC_BM_BUFGROUP_BUFFER_GET) |
						(1 << FUNC_BM_PORT_DYNAMIC_THRESH_GET) |
						(1 << FUNC_PORT_BM_CTRL_GET) |
						(1 << FUNC_BM_BUFGROUP_BUFFER_SET) |
						(1 << FUNC_PORT_BUFGROUP_MAP_SET) |
						(1 << FUNC_BM_PORT_STATIC_THRESH_GET) |
						(1 << FUNC_BM_PORT_RESERVED_BUFFER_SET) |
						(1 << FUNC_BM_PORT_STATIC_THRESH_SET) |
						(1 << FUNC_BM_PORT_DYNAMIC_THRESH_SET) |
						(1 << FUNC_PORT_BM_CTRL_SET) |
						(1 << FUNC_PORT_TDM_CTRL_SET) |
						(1 << FUNC_PORT_TDM_TICK_CFG_SET) |
						(1 << FUNC_BM_PORT_COUNTER_GET));
	return;
}

static void adpt_hppe_bm_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_port_bufgroup_map_get = NULL;
	p_adpt_api->adpt_bm_port_reserved_buffer_get = NULL;
	p_adpt_api->adpt_bm_bufgroup_buffer_get = NULL;
	p_adpt_api->adpt_bm_port_dynamic_thresh_get = NULL;
	p_adpt_api->adpt_port_bm_ctrl_get = NULL;
	p_adpt_api->adpt_bm_bufgroup_buffer_set = NULL;
	p_adpt_api->adpt_port_bufgroup_map_set = NULL;
	p_adpt_api->adpt_bm_port_static_thresh_get = NULL;
	p_adpt_api->adpt_bm_port_reserved_buffer_set = NULL;
	p_adpt_api->adpt_bm_port_static_thresh_set = NULL;
	p_adpt_api->adpt_bm_port_dynamic_thresh_set = NULL;
	p_adpt_api->adpt_port_bm_ctrl_set = NULL;
	p_adpt_api->adpt_port_tdm_ctrl_set = NULL;
	p_adpt_api->adpt_port_tdm_tick_cfg_set = NULL;
	p_adpt_api->adpt_bm_port_counter_get = NULL;

	return;
}


sw_error_t adpt_hppe_bm_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_bm_func_unregister(dev_id, p_adpt_api);

#ifndef IN_BM_MINI
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_PORT_BUFGROUP_MAP_GET))
		p_adpt_api->adpt_port_bufgroup_map_get = adpt_hppe_port_bufgroup_map_get;
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_BM_PORT_RESERVED_BUFFER_GET))
		p_adpt_api->adpt_bm_port_reserved_buffer_get = adpt_hppe_bm_port_reserved_buffer_get;
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_BM_BUFGROUP_BUFFER_GET))
		p_adpt_api->adpt_bm_bufgroup_buffer_get = adpt_hppe_bm_bufgroup_buffer_get;
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_BM_PORT_DYNAMIC_THRESH_GET))
		p_adpt_api->adpt_bm_port_dynamic_thresh_get = adpt_hppe_bm_port_dynamic_thresh_get;
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_PORT_BM_CTRL_GET))
		p_adpt_api->adpt_port_bm_ctrl_get = adpt_hppe_port_bm_ctrl_get;
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_BM_PORT_STATIC_THRESH_GET))
		p_adpt_api->adpt_bm_port_static_thresh_get = adpt_hppe_bm_port_static_thresh_get;
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_BM_PORT_COUNTER_GET))
		p_adpt_api->adpt_bm_port_counter_get = adpt_hppe_bm_port_counter_get;
#endif
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_BM_BUFGROUP_BUFFER_SET))
		p_adpt_api->adpt_bm_bufgroup_buffer_set = adpt_hppe_bm_bufgroup_buffer_set;
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_PORT_BUFGROUP_MAP_SET))
		p_adpt_api->adpt_port_bufgroup_map_set = adpt_hppe_port_bufgroup_map_set;
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_BM_PORT_RESERVED_BUFFER_SET))
		p_adpt_api->adpt_bm_port_reserved_buffer_set = adpt_hppe_bm_port_reserved_buffer_set;
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_BM_PORT_STATIC_THRESH_SET))
		p_adpt_api->adpt_bm_port_static_thresh_set = adpt_hppe_bm_port_static_thresh_set;
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_BM_PORT_DYNAMIC_THRESH_SET))
		p_adpt_api->adpt_bm_port_dynamic_thresh_set = adpt_hppe_bm_port_dynamic_thresh_set;
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_PORT_BM_CTRL_SET))
		p_adpt_api->adpt_port_bm_ctrl_set = adpt_hppe_port_bm_ctrl_set;
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_PORT_TDM_CTRL_SET))
		p_adpt_api->adpt_port_tdm_ctrl_set = adpt_hppe_port_tdm_ctrl_set;
	if (p_adpt_api->adpt_bm_func_bitmap & (1 << FUNC_PORT_TDM_TICK_CFG_SET))
		p_adpt_api->adpt_port_tdm_tick_cfg_set = adpt_hppe_port_tdm_tick_cfg_set;

	return SW_OK;
}

/**
 * @}
 */
