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
#include "hppe_mirror_reg.h"
#include "hppe_mirror.h"
#include "hppe_qm_reg.h"
#include "hppe_qm.h"
#include "adpt.h"

sw_error_t
adpt_hppe_mirr_port_in_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	union port_mirror_u port_mirror;

	memset(&port_mirror, 0, sizeof(port_mirror));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	/* mirror port just support physical port, not support trunk and virtual port */
	if (FAL_PORT_ID_TYPE(port_id) != 0)
		return SW_BAD_PARAM;

	rv = hppe_port_mirror_get(dev_id, port_id, &port_mirror);

	if( rv != SW_OK )
		return rv;

	*enable = port_mirror.bf.in_mirr_en;

	return SW_OK;
}

sw_error_t
adpt_hppe_mirr_port_eg_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	union port_mirror_u port_mirror;

	memset(&port_mirror, 0, sizeof(port_mirror));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	/* mirror port just support physical port, not support trunk and virtual port */
	if (FAL_PORT_ID_TYPE(port_id) != 0)
		return SW_BAD_PARAM;

	rv = hppe_port_mirror_get(dev_id, port_id, &port_mirror);

	if( rv != SW_OK )
		return rv;

	*enable = port_mirror.bf.eg_mirr_en;

	return SW_OK;
}

sw_error_t
adpt_hppe_mirr_analysis_port_get(a_uint32_t dev_id, fal_port_t * port_id)
{
	sw_error_t rv = SW_OK;
	union mirror_analyzer_u mirror_analyzer;

	memset(&mirror_analyzer, 0, sizeof(mirror_analyzer));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(port_id);

	/* analysis port just support physical port and trunk port, not support virtual port */
	if (FAL_PORT_ID_TYPE(*port_id) != 0 && FAL_PORT_ID_TYPE(*port_id) != 1)
		return SW_BAD_PARAM;

	rv = hppe_mirror_analyzer_get(dev_id, &mirror_analyzer);

	if( rv != SW_OK )
		return rv;

	if (mirror_analyzer.bf.in_analyzer_port != mirror_analyzer.bf.eg_analyzer_port)
		return SW_FAIL;

	*port_id = mirror_analyzer.bf.eg_analyzer_port;

	if (*port_id == 32 || *port_id == 33)
		*port_id = FAL_PORT_ID(FAL_PORT_TYPE_TRUNK, *port_id);

	return SW_OK;
}

sw_error_t
adpt_hppe_mirr_port_in_set(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t enable)
{
	union port_mirror_u port_mirror;

	memset(&port_mirror, 0, sizeof(port_mirror));
	ADPT_DEV_ID_CHECK(dev_id);

	/* mirror port just support physical port, not support trunk and virtual port */
	if (FAL_PORT_ID_TYPE(port_id) != 0)
		return SW_BAD_PARAM;

	hppe_port_mirror_get(dev_id, port_id, &port_mirror);
	port_mirror.bf.in_mirr_en = enable;

	hppe_port_mirror_set(dev_id, port_id, &port_mirror);

	return SW_OK;
}
sw_error_t
adpt_hppe_mirr_port_eg_set(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t enable)
{
	union port_mirror_u port_mirror;

	memset(&port_mirror, 0, sizeof(port_mirror));
	ADPT_DEV_ID_CHECK(dev_id);

	/* mirror port just support physical port, not support trunk and virtual port */
	if (FAL_PORT_ID_TYPE(port_id) != 0)
		return SW_BAD_PARAM;

	hppe_port_mirror_get(dev_id, port_id, &port_mirror);
	port_mirror.bf.eg_mirr_en = enable;

	hppe_port_mirror_set(dev_id, port_id, &port_mirror);

	return SW_OK;
}
sw_error_t
adpt_hppe_mirr_analysis_port_set(a_uint32_t dev_id, fal_port_t port_id)
{
	union mirror_analyzer_u mirror_analyzer;

	memset(&mirror_analyzer, 0, sizeof(mirror_analyzer));
	ADPT_DEV_ID_CHECK(dev_id);

	/* analysis port just support physical port and trunk port, not support port */
	if (FAL_PORT_ID_TYPE(port_id) != 0 && FAL_PORT_ID_TYPE(port_id) != 1)
		return SW_BAD_PARAM;

	port_id = FAL_PORT_ID_VALUE(port_id);

	hppe_mirror_analyzer_get(dev_id, &mirror_analyzer);

	mirror_analyzer.bf.in_analyzer_port = port_id;
	mirror_analyzer.bf.eg_analyzer_port = port_id;

	hppe_mirror_analyzer_set(dev_id, &mirror_analyzer);

	return SW_OK;
}

sw_error_t
adpt_hppe_mirr_analysis_config_set(a_uint32_t dev_id, fal_mirr_direction_t direction, fal_mirr_analysis_config_t * config)
{
	union mirror_analyzer_u mirror_analyzer;
	union in_mirror_priority_ctrl_u in_mirror_priority_ctrl;
	union eg_mirror_priority_ctrl_u eg_mirror_priority_ctrl;

	memset(&mirror_analyzer, 0, sizeof(mirror_analyzer));
	memset(&in_mirror_priority_ctrl, 0, sizeof(in_mirror_priority_ctrl));
	memset(&eg_mirror_priority_ctrl, 0, sizeof(eg_mirror_priority_ctrl));

	ADPT_DEV_ID_CHECK(dev_id);

	/* analysis port just support physical port and trunk port, not support port */
	if (FAL_PORT_ID_TYPE(config->port_id) != 0 && FAL_PORT_ID_TYPE(config->port_id) != 1)
		return SW_BAD_PARAM;

	config->port_id = FAL_PORT_ID_VALUE(config->port_id);

	hppe_mirror_analyzer_get(dev_id, &mirror_analyzer);

	if (direction == FAL_MIRR_BOTH)
	{
		mirror_analyzer.bf.in_analyzer_port = config->port_id;
		mirror_analyzer.bf.eg_analyzer_port = config->port_id;
		in_mirror_priority_ctrl.bf.priority = config->priority;
		eg_mirror_priority_ctrl.bf.priority = config->priority;
		hppe_mirror_analyzer_set(dev_id, &mirror_analyzer);
		hppe_in_mirror_priority_ctrl_set(dev_id, &in_mirror_priority_ctrl);
		hppe_eg_mirror_priority_ctrl_set(dev_id, &eg_mirror_priority_ctrl);
	}
	else if (direction == FAL_MIRR_INGRESS)
	{
		mirror_analyzer.bf.in_analyzer_port = config->port_id;
		in_mirror_priority_ctrl.bf.priority = config->priority;
		hppe_mirror_analyzer_set(dev_id, &mirror_analyzer);
		hppe_in_mirror_priority_ctrl_set(dev_id, &in_mirror_priority_ctrl);
	}
	else if (direction == FAL_MIRR_EGRESS)
	{
		mirror_analyzer.bf.eg_analyzer_port = config->port_id;
		eg_mirror_priority_ctrl.bf.priority = config->priority;
		hppe_mirror_analyzer_set(dev_id, &mirror_analyzer);
		hppe_eg_mirror_priority_ctrl_set(dev_id, &eg_mirror_priority_ctrl);
	}
	else
		return SW_NOT_SUPPORTED;

	return SW_OK;
}

sw_error_t
adpt_hppe_mirr_analysis_config_get(a_uint32_t dev_id, fal_mirr_direction_t direction, fal_mirr_analysis_config_t * config)
{
	union mirror_analyzer_u mirror_analyzer;
	union in_mirror_priority_ctrl_u in_mirror_priority_ctrl;
	union eg_mirror_priority_ctrl_u eg_mirror_priority_ctrl;

	memset(&mirror_analyzer, 0, sizeof(mirror_analyzer));
	memset(&in_mirror_priority_ctrl, 0, sizeof(in_mirror_priority_ctrl));
	memset(&eg_mirror_priority_ctrl, 0, sizeof(eg_mirror_priority_ctrl));

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(config);
	/* analysis port just support physical port and trunk port, not support virtual port */
	if (FAL_PORT_ID_TYPE(config->port_id) != 0 && FAL_PORT_ID_TYPE(config->port_id) != 1)
		return SW_BAD_PARAM;

	hppe_mirror_analyzer_get(dev_id, &mirror_analyzer);
	hppe_in_mirror_priority_ctrl_get(dev_id, &in_mirror_priority_ctrl);
	hppe_eg_mirror_priority_ctrl_get(dev_id, &eg_mirror_priority_ctrl);

	if (direction == FAL_MIRR_BOTH)
	{
		if ((mirror_analyzer.bf.in_analyzer_port != mirror_analyzer.bf.eg_analyzer_port) ||
			(in_mirror_priority_ctrl.bf.priority != eg_mirror_priority_ctrl.bf.priority))
			return SW_FAIL;

		config->port_id = mirror_analyzer.bf.in_analyzer_port;
		config->priority = in_mirror_priority_ctrl.bf.priority;
	}
	else if (direction == FAL_MIRR_INGRESS)
	{
		config->port_id = mirror_analyzer.bf.in_analyzer_port;
		config->priority = in_mirror_priority_ctrl.bf.priority;
	}
	else if (direction == FAL_MIRR_EGRESS)
	{
		config->port_id = mirror_analyzer.bf.eg_analyzer_port;
		config->priority = eg_mirror_priority_ctrl.bf.priority;
	}
	else
		return SW_NOT_SUPPORTED;

	if (config->port_id == 32 || config->port_id == 33)
		config->port_id = FAL_PORT_ID(FAL_PORT_TYPE_TRUNK, config->port_id);

	return SW_OK;
}

void adpt_hppe_mirror_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_mirror_func_bitmap = ((1 << FUNC_MIRR_ANALYSIS_PORT_SET) |
						(1 << FUNC_MIRR_ANALYSIS_PORT_GET) |
						(1 << FUNC_MIRR_PORT_IN_SET) |
						(1 << FUNC_MIRR_PORT_IN_GET) |
						(1 << FUNC_MIRR_PORT_EG_SET) |
						(1 << FUNC_MIRR_PORT_EG_GET) |
						(1 << FUNC_MIRR_ANALYSIS_CONFIG_SET) |
						(1 << FUNC_MIRR_ANALYSIS_CONFIG_GET));

	return;
}

static void adpt_hppe_mirror_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_mirr_port_in_set = NULL;
	p_adpt_api->adpt_mirr_port_in_get = NULL;
	p_adpt_api->adpt_mirr_port_eg_set = NULL;
	p_adpt_api->adpt_mirr_port_eg_get = NULL;
	p_adpt_api->adpt_mirr_analysis_port_set = NULL;
	p_adpt_api->adpt_mirr_analysis_port_get = NULL;
	p_adpt_api->adpt_mirr_analysis_config_set = NULL;
	p_adpt_api->adpt_mirr_analysis_config_get = NULL;

	return;
}




sw_error_t adpt_hppe_mirror_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_mirror_func_unregister(dev_id, p_adpt_api);

	if (p_adpt_api->adpt_mirror_func_bitmap & (1 << FUNC_MIRR_PORT_IN_SET))
		p_adpt_api->adpt_mirr_port_in_set = adpt_hppe_mirr_port_in_set;
	if (p_adpt_api->adpt_mirror_func_bitmap & (1 << FUNC_MIRR_PORT_IN_GET))
		p_adpt_api->adpt_mirr_port_in_get = adpt_hppe_mirr_port_in_get;
	if (p_adpt_api->adpt_mirror_func_bitmap & (1 << FUNC_MIRR_PORT_EG_SET))
		p_adpt_api->adpt_mirr_port_eg_set = adpt_hppe_mirr_port_eg_set;
	if (p_adpt_api->adpt_mirror_func_bitmap & (1 << FUNC_MIRR_PORT_EG_GET))
		p_adpt_api->adpt_mirr_port_eg_get = adpt_hppe_mirr_port_eg_get;
	if (p_adpt_api->adpt_mirror_func_bitmap & (1 << FUNC_MIRR_ANALYSIS_PORT_SET))
		p_adpt_api->adpt_mirr_analysis_port_set = adpt_hppe_mirr_analysis_port_set;
	if (p_adpt_api->adpt_mirror_func_bitmap & (1 << FUNC_MIRR_ANALYSIS_PORT_GET))
		p_adpt_api->adpt_mirr_analysis_port_get = adpt_hppe_mirr_analysis_port_get;
	if (p_adpt_api->adpt_mirror_func_bitmap & (1 << FUNC_MIRR_ANALYSIS_CONFIG_SET))
		p_adpt_api->adpt_mirr_analysis_config_set = adpt_hppe_mirr_analysis_config_set;
	if (p_adpt_api->adpt_mirror_func_bitmap & (1 << FUNC_MIRR_ANALYSIS_CONFIG_GET))
		p_adpt_api->adpt_mirr_analysis_config_get = adpt_hppe_mirr_analysis_config_get;

	return SW_OK;
}

/**
 * @}
 */
