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
#include "hppe_shaper_reg.h"
#include "hppe_shaper.h"
#include "adpt.h"

#define NR_ADPT_HPPE_SHAPER_METER_UNIT         2
#define NR_ADPT_HPPE_SHAPER_METER_TOKEN_UNIT         8
#define ADPT_HPPE_SHAPER_METER_UNIT_BYTE       0
#define ADPT_HPPE_SHAPER_METER_UNIT_FRAME      1
#define ADPT_HPPE_FREQUENCY      300  /*MHZ*/
#define ADPT_HPPE_SHAPER_BURST_SIZE_UNIT      65536
#define ADPT_HPPE_SHAPER_REFRESH_BITS  18
#define ADPT_HPPE_SHAPER_BUCKET_SIZE_BITS  14
#define ADPT_HPPE_SHAPER_REFRESH_MAX  ((1 << ADPT_HPPE_SHAPER_REFRESH_BITS) - 1)
#define ADPT_HPPE_SHAPER_BUCKET_SIZE_MAX  ((1 << ADPT_HPPE_SHAPER_BUCKET_SIZE_BITS) - 1)
#define ADPT_HPPE_PORT_SHAPER         0
#define ADPT_HPPE_FLOW_SHAPER         1
#define ADPT_HPPE_QUEUE_SHAPER       2
#define BYTE_SHAPER_MAX_RATE 10000000
#define BYTE_SHAPER_MIN_RATE 64
#define FRAME_SHAPER_MAX_RATE 14881000
#define FRAME_SHAPER_MIN_RATE 6



static a_uint32_t hppe_shaper_token_unit[NR_ADPT_HPPE_SHAPER_METER_UNIT]
	[NR_ADPT_HPPE_SHAPER_METER_TOKEN_UNIT] = {{2048 * 8,
	512 * 8,128 * 8,32 * 8,8 * 8,2 * 8, 4, 1},
	{2097152,524288,131072,32768,8192,2048,512,128}};

typedef struct
{
    a_uint64_t rate_1bit;
    a_uint64_t rate_max;
} adpt_hppe_shaper_rate_t;

typedef struct
{
    a_uint64_t burst_size_1bit;
    a_uint64_t burst_size_max;
} adpt_hppe_shaper_burst_size_t;

static adpt_hppe_shaper_rate_t
hppe_port_shaper_rate[NR_ADPT_HPPE_SHAPER_METER_UNIT][NR_ADPT_HPPE_SHAPER_METER_TOKEN_UNIT] =
{
	/* byte based*/
	{
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
	},

	/*frame based */
	{
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
	},
};

static adpt_hppe_shaper_rate_t
hppe_flow_shaper_rate[NR_ADPT_HPPE_SHAPER_METER_UNIT][NR_ADPT_HPPE_SHAPER_METER_TOKEN_UNIT] =
{
	/* byte based*/
	{
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
	},

	/*frame based */
	{
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
	},
};

static adpt_hppe_shaper_rate_t
hppe_queue_shaper_rate[NR_ADPT_HPPE_SHAPER_METER_UNIT][NR_ADPT_HPPE_SHAPER_METER_TOKEN_UNIT] =
{
	/* byte based*/
	{
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
	},

	/*frame based */
	{
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
	},
};

static adpt_hppe_shaper_burst_size_t
hppe_shaper_burst_size[NR_ADPT_HPPE_SHAPER_METER_UNIT][NR_ADPT_HPPE_SHAPER_METER_TOKEN_UNIT] =
{
	{	{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
	},
	{	{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
		{0,0},
	},
};

static sw_error_t
__adpt_hppe_port_shaper_max_rate(a_uint32_t time_slot)
{
	a_uint32_t i = 0, j = 0;
	a_uint32_t time_cycle;
	a_uint64_t temp, temp1,temp2;

	/* time_cycle is ns*/
	time_cycle = ( time_slot * 8);

	for (j = 0; j < 8; j++)
	{
		/*max rate unit is bps*/
		temp1 = (a_uint64_t)(ADPT_HPPE_SHAPER_REFRESH_MAX  * 1000 * 8) * (a_uint64_t)(ADPT_HPPE_FREQUENCY * 1000);
		temp2 =  hppe_shaper_token_unit[i][j] * time_cycle;

		do_div(temp1, temp2);
		hppe_port_shaper_rate[i][j].rate_max = temp1;

		temp = temp1;
		do_div(temp, ADPT_HPPE_SHAPER_REFRESH_MAX);
		hppe_port_shaper_rate[i][j].rate_1bit = temp;

		//printk("port shaper hppe_max_rate generating =%llu\n", hppe_port_shaper_rate[i][j].rate_max);
		//printk("port shaper byte step rate =%llu\n", hppe_port_shaper_rate[i][j].rate_1bit);
	}

	i = i + 1;
	for (j = 0; j < 8; j++)
	{
		/* max rate unit  is 1/1000 pps*/
		temp1 = (a_uint64_t)(ADPT_HPPE_SHAPER_REFRESH_MAX * 1000) * 1000 * (a_uint64_t)(ADPT_HPPE_FREQUENCY * 1000);
		temp2 = (a_uint64_t)hppe_shaper_token_unit[i][j] * time_cycle;

		do_div(temp1, temp2);
		hppe_port_shaper_rate[i][j].rate_max = temp1;

		temp = temp1;
		do_div(temp, ADPT_HPPE_SHAPER_REFRESH_MAX);
		hppe_port_shaper_rate[i][j].rate_1bit = temp;

		//printk("port shaper hppe_max_rate generating =%llu\n", hppe_port_shaper_rate[i][j].rate_max);
		//printk("port shaper frame step rate =%llu\n", hppe_port_shaper_rate[i][j].rate_1bit);
	}

	return SW_OK;
}

static sw_error_t
__adpt_hppe_flow_shaper_max_rate(a_uint32_t time_slot)
{
	a_uint32_t i = 0, j = 0;
	a_uint32_t time_cycle;
	a_uint64_t temp, temp1,temp2;

	/* time_cycle is ns*/
	time_cycle = ( time_slot * 8);

	for (j = 0; j < 8; j++)
	{
		/*max rate unit is bps*/
		temp1 = (a_uint64_t)(ADPT_HPPE_SHAPER_REFRESH_MAX  * 1000 * 8) * (a_uint64_t)(ADPT_HPPE_FREQUENCY * 1000);
		temp2 =  hppe_shaper_token_unit[i][j] * time_cycle;

		do_div(temp1, temp2);
		hppe_flow_shaper_rate[i][j].rate_max = temp1;

		temp = temp1;
		do_div(temp, ADPT_HPPE_SHAPER_REFRESH_MAX);
		hppe_flow_shaper_rate[i][j].rate_1bit = temp;

		//printk("flow shaper hppe_max_rate generating =%llu\n", hppe_flow_shaper_rate[i][j].rate_max);
		//printk("flow shaper byte step rate =%llu\n", hppe_flow_shaper_rate[i][j].rate_1bit);
	}

	i = i + 1;
	for (j = 0; j < 8; j++)
	{
		/* max rate unit  is 1/1000 pps*/
		temp1 = (a_uint64_t)(ADPT_HPPE_SHAPER_REFRESH_MAX * 1000) * 1000 * (a_uint64_t)(ADPT_HPPE_FREQUENCY * 1000);
		temp2 = (a_uint64_t)hppe_shaper_token_unit[i][j] * time_cycle;

		do_div(temp1, temp2);
		hppe_flow_shaper_rate[i][j].rate_max = temp1;

		temp = temp1;
		do_div(temp, ADPT_HPPE_SHAPER_REFRESH_MAX);
		hppe_flow_shaper_rate[i][j].rate_1bit = temp;

		//printk("flow shaper hppe_max_rate generating =%llu\n", hppe_flow_shaper_rate[i][j].rate_max);
		//printk("flow shaper frame step rate =%llu\n", hppe_flow_shaper_rate[i][j].rate_1bit);
	}

	return SW_OK;
}

static sw_error_t
__adpt_hppe_queue_shaper_max_rate(a_uint32_t time_slot)
{
	a_uint32_t i = 0, j = 0;
	a_uint32_t time_cycle;
	a_uint64_t temp, temp1,temp2;


	/* time_cycle is ns*/
	time_cycle = ( time_slot * 8) /ADPT_HPPE_FREQUENCY;

	for (j = 0; j < 8; j++)
	{
		/*max rate unit is bps*/
		temp1 = (a_uint64_t)(ADPT_HPPE_SHAPER_REFRESH_MAX  * 1000 * 8) * 1000;
		temp2 =  hppe_shaper_token_unit[i][j] * time_cycle;

		do_div(temp1, temp2);
		hppe_queue_shaper_rate[i][j].rate_max = temp1;

		temp = temp1;
		do_div(temp, ADPT_HPPE_SHAPER_REFRESH_MAX);
		hppe_queue_shaper_rate[i][j].rate_1bit = temp;

		//printk("queue shaper hppe_max_rate generating =%llu\n", hppe_queue_shaper_rate[i][j].rate_max);
		//printk("queue shaper byte step rate =%llu\n", hppe_queue_shaper_rate[i][j].rate_1bit);
	}

	i = i + 1;
	for (j = 0; j < 8; j++)
	{
		/* max rate unit  is 1/1000 pps*/
		temp1 = (a_uint64_t)(ADPT_HPPE_SHAPER_REFRESH_MAX * 1000) * 1000 * 1000;
		temp2 = (a_uint64_t)hppe_shaper_token_unit[i][j] * time_cycle;

		do_div(temp1, temp2);
		hppe_queue_shaper_rate[i][j].rate_max = temp1;

		temp = temp1;
		do_div(temp, ADPT_HPPE_SHAPER_REFRESH_MAX);
		hppe_queue_shaper_rate[i][j].rate_1bit = temp;

		//printk("queue shaper hppe_max_rate generating =%llu\n", hppe_queue_shaper_rate[i][j].rate_max);
		//printk("queue shaper frame step rate =%llu\n", hppe_queue_shaper_rate[i][j].rate_1bit);
	}

	return SW_OK;
}

static sw_error_t
__adpt_hppe_shaper_max_burst_size(void)
{
	a_uint32_t i = 0, j = 0;
	a_uint64_t temp = 0, temp1 = 0;

	for (j = 0; j < 8; j++)
	{
		/*max size unit is 1/1000 byte based*/
		temp = (a_uint64_t)(ADPT_HPPE_SHAPER_BURST_SIZE_UNIT * ADPT_HPPE_SHAPER_BUCKET_SIZE_MAX);
		do_div(temp, hppe_shaper_token_unit[i][j]);
		hppe_shaper_burst_size[i][j].burst_size_max = (a_uint64_t)(temp * 1000);

		temp1 = hppe_shaper_burst_size[i][j].burst_size_max;
		do_div(temp1, ADPT_HPPE_SHAPER_BUCKET_SIZE_MAX);
		hppe_shaper_burst_size[i][j].burst_size_1bit = temp1;

		//printk("shpaer byte hppe_max_burst_size generating =%llu\n", hppe_shaper_burst_size[i][j].burst_size_max);
		//printk("shpaer byte hppe_max_burst_size step  =%llu\n", hppe_shaper_burst_size[i][j].burst_size_1bit);
	}

	i = i + 1;
	for (j = 0; j < 8; j++)
	{
		/* max size unit is 1/1000 frame based */
		temp = (a_uint64_t)(ADPT_HPPE_SHAPER_BURST_SIZE_UNIT * ADPT_HPPE_SHAPER_BUCKET_SIZE_MAX);
		do_div(temp, hppe_shaper_token_unit[i][j]);
		hppe_shaper_burst_size[i][j].burst_size_max = (a_uint64_t)(temp * 1000);

		temp1 = hppe_shaper_burst_size[i][j].burst_size_max;
		do_div(temp1, ADPT_HPPE_SHAPER_BUCKET_SIZE_MAX);
		hppe_shaper_burst_size[i][j].burst_size_1bit = temp1;

		//printk("shaper frame hppe_max_burst_size generating =%llu\n", hppe_shaper_burst_size[i][j].burst_size_max);
		//printk("shpaer frame hppe_max_burst_size step  =%llu\n", hppe_shaper_burst_size[i][j].burst_size_1bit);
	}

	return SW_OK;
}

static sw_error_t
__adpt_hppe_shaper_one_bucket_parameter_select(a_uint32_t shaper_type,
						a_uint64_t c_rate,
						a_uint64_t c_burst_size,
						a_uint32_t meter_unit,
						a_uint32_t *token_unit)
{
	a_uint32_t  temp_token_unit = 0;
	a_uint32_t  match = A_FALSE;
	a_uint64_t  max_rate =0;

	for (temp_token_unit = 0; temp_token_unit < 8; temp_token_unit++)
	{
		if (ADPT_HPPE_PORT_SHAPER == shaper_type)
			max_rate = hppe_port_shaper_rate[meter_unit][temp_token_unit].rate_max;

		if (ADPT_HPPE_FLOW_SHAPER == shaper_type)
			max_rate = hppe_flow_shaper_rate[meter_unit][temp_token_unit].rate_max;

		if (ADPT_HPPE_QUEUE_SHAPER == shaper_type)
			max_rate = hppe_queue_shaper_rate[meter_unit][temp_token_unit].rate_max;

		if (c_rate > max_rate)
		{
			continue;
		}
		else if (c_burst_size <= hppe_shaper_burst_size[meter_unit][temp_token_unit].burst_size_max)
		{
			*token_unit = temp_token_unit;
			match = A_TRUE;
			break;
		}
	}

	if (match == A_FALSE)
	{
		printk("Not match  shaper C token bucket parameter rate configuration\n");
		return SW_BAD_PARAM;
	}

	return SW_OK;
}

static sw_error_t
__adpt_hppe_shaper_two_bucket_parameter_select(a_uint32_t shaper_type,
						a_uint64_t c_rate,
						a_uint64_t c_burst_size,
						a_uint64_t e_rate,
						a_uint64_t e_burst_size,
						a_uint32_t meter_unit,
						a_uint32_t *token_unit)
{
	a_uint32_t  temp_token_unit;
	a_uint32_t  match = A_FALSE;
	a_uint64_t  max_rate = 0, temp_rate = 0, temp_burst_size = 0;

	if(c_rate > e_rate)
		temp_rate = c_rate;
	else
		temp_rate = e_rate;

	if(c_burst_size > e_burst_size)
		temp_burst_size = c_burst_size;
	else
		temp_burst_size = e_burst_size;

	for (temp_token_unit = 0; temp_token_unit < 8; temp_token_unit++)
	{
		if (ADPT_HPPE_PORT_SHAPER == shaper_type)
			max_rate = hppe_port_shaper_rate[meter_unit][temp_token_unit].rate_max;

		if (ADPT_HPPE_FLOW_SHAPER == shaper_type)
			max_rate = hppe_flow_shaper_rate[meter_unit][temp_token_unit].rate_max;

		if (ADPT_HPPE_QUEUE_SHAPER == shaper_type)
			max_rate = hppe_queue_shaper_rate[meter_unit][temp_token_unit].rate_max;

		 if (temp_rate > max_rate)
		 {
		 	continue;
		 }
		 else if(temp_burst_size <= hppe_shaper_burst_size[meter_unit][temp_token_unit].burst_size_max)
		{
			*token_unit = temp_token_unit;
			match = A_TRUE;
			break;
		}
	}

	if (match == A_FALSE)
	{
		printk("Not match shaper  C and E token bucket parameter rate configuration \n");
		return SW_BAD_PARAM;
	}

	return SW_OK;
}

static sw_error_t
__adpt_hppe_shaper_rate_to_refresh(a_uint32_t shaper_type,
							a_uint32_t rate,
							a_uint32_t *refresh,
							a_bool_t meter_unit,
							a_uint32_t  token_unit)
{
	a_uint32_t temp_refresh;
	a_uint64_t temp_rate, temp_rate_1bit;

	temp_rate_1bit = 0;

	if (ADPT_HPPE_PORT_SHAPER == shaper_type)
	{
		temp_rate_1bit = hppe_port_shaper_rate[meter_unit][token_unit].rate_1bit;
	}

	if (ADPT_HPPE_FLOW_SHAPER == shaper_type)
	{
		temp_rate_1bit = hppe_flow_shaper_rate[meter_unit][token_unit].rate_1bit;
	}

	if (ADPT_HPPE_QUEUE_SHAPER == shaper_type)
	{
		temp_rate_1bit = hppe_queue_shaper_rate[meter_unit][token_unit].rate_1bit;
	}

	if(temp_rate_1bit > 0)
	{
		temp_rate = ((a_uint64_t)rate) * 1000;
		do_div(temp_rate, temp_rate_1bit);
		temp_refresh = temp_rate;
	}
	else
	{
		return SW_BAD_PARAM;
	}

	if (temp_refresh > ADPT_HPPE_SHAPER_REFRESH_MAX)
	{
		temp_refresh = ADPT_HPPE_SHAPER_REFRESH_MAX;
	}

	*refresh = temp_refresh;

	return SW_OK;
}

static sw_error_t
__adpt_hppe_shaper_burst_size_to_bucket_size(a_uint32_t burst_size,
							a_uint32_t *bucket_size,
							a_bool_t meter_unit,
							a_uint32_t  token_unit)
{
	a_uint32_t temp_bucket_size;
	a_uint64_t temp_burst_size;

	if(hppe_shaper_burst_size[meter_unit][token_unit].burst_size_1bit > 0)
	{
		temp_burst_size = ((a_uint64_t)burst_size) * 1000;
		do_div(temp_burst_size, hppe_shaper_burst_size[meter_unit][token_unit].burst_size_1bit);
		temp_bucket_size = temp_burst_size;
	}
	else
	{
		return SW_BAD_PARAM;
	}

	if(temp_bucket_size > ADPT_HPPE_SHAPER_BUCKET_SIZE_MAX)
	{
		temp_bucket_size = ADPT_HPPE_SHAPER_BUCKET_SIZE_MAX;
	}

	*bucket_size = temp_bucket_size;

	return SW_OK;
}

static sw_error_t
__adpt_hppe_shaper_refresh_to_rate(a_uint32_t shaper_type,
							a_uint32_t refresh,
							a_uint32_t *rate,
							a_bool_t meter_unit,
							a_uint32_t  token_unit)
{
	a_uint32_t temp_rate;
	a_uint64_t temp_refresh, temp_rate_1bit;

	temp_rate_1bit = 0;

	if (ADPT_HPPE_PORT_SHAPER == shaper_type)
	{
		temp_rate_1bit = hppe_port_shaper_rate[meter_unit][token_unit].rate_1bit;
	}

	if (ADPT_HPPE_FLOW_SHAPER == shaper_type)
	{
		temp_rate_1bit = hppe_flow_shaper_rate[meter_unit][token_unit].rate_1bit;
	}

	if (ADPT_HPPE_QUEUE_SHAPER == shaper_type)
	{
		temp_rate_1bit = hppe_queue_shaper_rate[meter_unit][token_unit].rate_1bit;
	}

	if(temp_rate_1bit > 0)
	{
		temp_refresh = ((a_uint64_t)refresh) * temp_rate_1bit;
		do_div(temp_refresh, 1000);
		temp_rate = temp_refresh;
	}
	else
	{
		return SW_BAD_PARAM;
	}

	*rate = temp_rate;

	return SW_OK;
}
static sw_error_t
__adpt_hppe_shaper_bucket_size_to_burst_size(a_uint32_t bucket_size,
							a_uint32_t *burst_size,
							a_bool_t meter_unit,
							a_uint32_t  token_unit)
{
	a_uint32_t temp_burst_size;
	a_uint64_t temp_bucket_size;

	if(hppe_shaper_burst_size[meter_unit][token_unit].burst_size_1bit > 0)
	{
		temp_bucket_size = ((a_uint64_t)bucket_size) * hppe_shaper_burst_size[meter_unit][token_unit].burst_size_1bit;
		do_div(temp_bucket_size, 1000);
		temp_burst_size = temp_bucket_size;
	}
	else
	{
		return SW_BAD_PARAM;
	}

	*burst_size = temp_burst_size;

	return SW_OK;
}

sw_error_t
adpt_hppe_queue_shaper_get(a_uint32_t dev_id, a_uint32_t queue_id,
		fal_shaper_config_t * shaper)
{
	sw_error_t rv = SW_OK;
	union l0_shp_cfg_tbl_u l0_shp_cfg_tbl;
	union l0_comp_cfg_tbl_u l0_comp_cfg_tbl;
	a_uint32_t hppe_cir =0, hppe_cbs = 0, hppe_eir = 0, hppe_ebs = 0;

	memset(&l0_shp_cfg_tbl, 0, sizeof(l0_shp_cfg_tbl));
	memset(&l0_comp_cfg_tbl, 0, sizeof(l0_comp_cfg_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(shaper);

	if ((queue_id < 0) || (queue_id > 299))
		return SW_BAD_PARAM;

	hppe_l0_comp_cfg_tbl_get(dev_id, queue_id, &l0_comp_cfg_tbl);


	rv = hppe_l0_shp_cfg_tbl_get(dev_id, queue_id, &l0_shp_cfg_tbl);

	if( rv != SW_OK )
		return rv;

	hppe_cir = l0_shp_cfg_tbl.bf.cir;
	hppe_cbs = l0_shp_cfg_tbl.bf.cbs;
	hppe_eir = l0_shp_cfg_tbl.bf.eir;
	hppe_ebs = l0_shp_cfg_tbl.bf.ebs;

	__adpt_hppe_shaper_refresh_to_rate(ADPT_HPPE_QUEUE_SHAPER,
								hppe_cir,
								&shaper->cir,
								l0_shp_cfg_tbl.bf.meter_unit,
								l0_shp_cfg_tbl.bf.token_unit);

	__adpt_hppe_shaper_refresh_to_rate(ADPT_HPPE_QUEUE_SHAPER,
								hppe_eir,
								&shaper->eir,
								l0_shp_cfg_tbl.bf.meter_unit,
								l0_shp_cfg_tbl.bf.token_unit);


	__adpt_hppe_shaper_bucket_size_to_burst_size(hppe_cbs,
								&shaper->cbs,
								l0_shp_cfg_tbl.bf.meter_unit,
								l0_shp_cfg_tbl.bf.token_unit);

	__adpt_hppe_shaper_bucket_size_to_burst_size(hppe_ebs,
								&shaper->ebs,
								l0_shp_cfg_tbl.bf.meter_unit,
								l0_shp_cfg_tbl.bf.token_unit);


	shaper->couple_en = l0_shp_cfg_tbl.bf.cf;
	shaper->meter_unit = l0_shp_cfg_tbl.bf.meter_unit;
	shaper->c_shaper_en = l0_shp_cfg_tbl.bf.c_shaper_enable;
	shaper->e_shaper_en = l0_shp_cfg_tbl.bf.e_shaper_enable;
	shaper->shaper_frame_mode = l0_comp_cfg_tbl.bf.shaper_meter_len;

	return SW_OK;
}

sw_error_t
adpt_hppe_queue_shaper_token_number_set(a_uint32_t dev_id,a_uint32_t queue_id,
		fal_shaper_token_number_t *token_number)
{
	union l0_shp_credit_tbl_u l0_shp_credit_tbl;

	memset(&l0_shp_credit_tbl, 0, sizeof(l0_shp_credit_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(token_number);

	if ((queue_id < 0) || (queue_id > 299))
		return SW_BAD_PARAM;

	hppe_l0_shp_credit_tbl_get(dev_id, queue_id, &l0_shp_credit_tbl);


	l0_shp_credit_tbl.bf.c_shaper_credit_neg = token_number->c_token_number_negative_en;
	l0_shp_credit_tbl.bf.c_shaper_credit = token_number->c_token_number;
	l0_shp_credit_tbl.bf.e_shaper_credit_neg = token_number->e_token_number_negative_en;
	l0_shp_credit_tbl.bf.e_shaper_credit_0 = token_number->e_token_number & 0x1;
	l0_shp_credit_tbl.bf.e_shaper_credit_1 = token_number->e_token_number >> 1;

	hppe_l0_shp_credit_tbl_set(dev_id, queue_id, &l0_shp_credit_tbl);

	return SW_OK;
}
#ifndef IN_SHAPER_MINI
sw_error_t
adpt_hppe_port_shaper_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_config_t * shaper)
{
	union psch_shp_cfg_tbl_u psch_shp_cfg_tbl;
	union psch_comp_cfg_tbl_u psch_comp_cfg_tbl;
	a_uint32_t hppe_cir =0, hppe_cbs = 0;

	memset(&psch_shp_cfg_tbl, 0, sizeof(psch_shp_cfg_tbl));
	memset(&psch_comp_cfg_tbl, 0, sizeof(psch_comp_cfg_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(shaper);

	if (port_id < 0 || port_id > 7)
		return SW_BAD_PARAM;

	hppe_psch_shp_cfg_tbl_get(dev_id, port_id, &psch_shp_cfg_tbl);

	hppe_cir = psch_shp_cfg_tbl.bf.cir;
	hppe_cbs = psch_shp_cfg_tbl.bf.cbs;

	hppe_psch_comp_cfg_tbl_get(dev_id, port_id, &psch_comp_cfg_tbl);


	__adpt_hppe_shaper_refresh_to_rate(ADPT_HPPE_PORT_SHAPER,
								hppe_cir,
								&shaper->cir,
								psch_shp_cfg_tbl.bf.meter_unit,
								psch_shp_cfg_tbl.bf.token_unit);


	__adpt_hppe_shaper_bucket_size_to_burst_size(hppe_cbs,
								&shaper->cbs,
								psch_shp_cfg_tbl.bf.meter_unit,
								psch_shp_cfg_tbl.bf.token_unit);

	shaper->meter_unit = psch_shp_cfg_tbl.bf.meter_unit;
	shaper->c_shaper_en = psch_shp_cfg_tbl.bf.shaper_enable;

	shaper->shaper_frame_mode = psch_comp_cfg_tbl.bf.shaper_meter_len;

	return SW_OK;
}

sw_error_t
adpt_hppe_flow_shaper_time_slot_get(a_uint32_t dev_id, a_uint32_t *time_slot)
{
	sw_error_t rv = SW_OK;
	union shp_slot_cfg_l1_u shp_slot_cfg_l1;

	memset(&shp_slot_cfg_l1, 0, sizeof(shp_slot_cfg_l1));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time_slot);


	rv = hppe_shp_slot_cfg_l1_get(dev_id, &shp_slot_cfg_l1);

	if( rv != SW_OK )
		return rv;

	*time_slot = shp_slot_cfg_l1.bf.l1_shp_slot_time;

	return SW_OK;
}

sw_error_t
adpt_hppe_port_shaper_time_slot_get(a_uint32_t dev_id, a_uint32_t *time_slot)
{
	sw_error_t rv = SW_OK;
	union shp_slot_cfg_port_u shp_slot_cfg_port;

	memset(&shp_slot_cfg_port, 0, sizeof(shp_slot_cfg_port));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time_slot);

	rv = hppe_shp_slot_cfg_port_get(dev_id, &shp_slot_cfg_port);

	if( rv != SW_OK )
		return rv;

	*time_slot = shp_slot_cfg_port.bf.port_shp_slot_time;

	return SW_OK;
}
#endif

sw_error_t
adpt_hppe_flow_shaper_time_slot_set(a_uint32_t dev_id, a_uint32_t time_slot)
{
	union shp_slot_cfg_l1_u shp_slot_cfg_l1;
	sw_error_t rv = SW_OK;

	memset(&shp_slot_cfg_l1, 0, sizeof(shp_slot_cfg_l1));
	ADPT_DEV_ID_CHECK(dev_id);

	if ((time_slot < 0x40) || (time_slot > 0xfff))
		return SW_BAD_PARAM;

	rv = hppe_shp_slot_cfg_l1_get(dev_id, &shp_slot_cfg_l1);

	if( rv != SW_OK )
		return rv;

	shp_slot_cfg_l1.bf.l1_shp_slot_time = time_slot;
	hppe_shp_slot_cfg_l1_set(dev_id, &shp_slot_cfg_l1);

	__adpt_hppe_flow_shaper_max_rate(time_slot);

	return SW_OK;

}
sw_error_t
adpt_hppe_port_shaper_token_number_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_token_number_t *token_number)
{
	union psch_shp_credit_tbl_u psch_shp_credit_tbl;
	union psch_shp_sign_tbl_u psch_shp_sign_tbl;

	memset(&psch_shp_credit_tbl, 0, sizeof(psch_shp_credit_tbl));
	memset(&psch_shp_sign_tbl, 0, sizeof(psch_shp_sign_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(token_number);

	if (port_id < 0 || port_id > 7)
		return SW_BAD_PARAM;

	hppe_psch_shp_credit_tbl_get(dev_id, port_id, &psch_shp_credit_tbl);
	hppe_psch_shp_sign_tbl_get(dev_id, port_id, &psch_shp_sign_tbl);


	psch_shp_sign_tbl.bf.shaper_credit_neg = token_number->c_token_number_negative_en;

	psch_shp_credit_tbl.bf.shaper_credit = token_number->c_token_number;

	hppe_psch_shp_credit_tbl_set(dev_id, port_id, &psch_shp_credit_tbl);

	hppe_psch_shp_sign_tbl_set(dev_id, port_id, &psch_shp_sign_tbl);

	return SW_OK;
}
#ifndef IN_SHAPER_MINI
sw_error_t
adpt_hppe_queue_shaper_token_number_get(a_uint32_t dev_id, a_uint32_t queue_id,
		fal_shaper_token_number_t *token_number)
{
	sw_error_t rv = SW_OK;
	union l0_shp_credit_tbl_u l0_shp_credit_tbl;

	memset(&l0_shp_credit_tbl, 0, sizeof(l0_shp_credit_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(token_number);

	if ((queue_id < 0) || (queue_id > 299))
		return SW_BAD_PARAM;

	rv = hppe_l0_shp_credit_tbl_get(dev_id, queue_id, &l0_shp_credit_tbl);

	if( rv != SW_OK )
		return rv;

	token_number->c_token_number_negative_en = l0_shp_credit_tbl.bf.c_shaper_credit_neg;
	token_number->c_token_number = l0_shp_credit_tbl.bf.c_shaper_credit;
	token_number->e_token_number_negative_en = l0_shp_credit_tbl.bf.e_shaper_credit_neg;
	token_number->e_token_number = l0_shp_credit_tbl.bf.e_shaper_credit_0 | (l0_shp_credit_tbl.bf.e_shaper_credit_1 << 1);


	return SW_OK;
}

sw_error_t
adpt_hppe_queue_shaper_time_slot_get(a_uint32_t dev_id, a_uint32_t *time_slot)
{
	sw_error_t rv = SW_OK;
	union shp_slot_cfg_l0_u shp_slot_cfg_l0;

	memset(&shp_slot_cfg_l0, 0, sizeof(shp_slot_cfg_l0));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time_slot);


	rv = hppe_shp_slot_cfg_l0_get(dev_id, &shp_slot_cfg_l0);

	if( rv != SW_OK )
		return rv;

	*time_slot = shp_slot_cfg_l0.bf.l0_shp_slot_time;

	return SW_OK;
}

sw_error_t
adpt_hppe_port_shaper_token_number_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_token_number_t *token_number)
{
	union psch_shp_credit_tbl_u psch_shp_credit_tbl;
	union psch_shp_sign_tbl_u psch_shp_sign_tbl;

	memset(&psch_shp_credit_tbl, 0, sizeof(psch_shp_credit_tbl));
	memset(&psch_shp_sign_tbl, 0, sizeof(psch_shp_sign_tbl));

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(token_number);

	if (port_id < 0 || port_id > 7)
		return SW_BAD_PARAM;

	hppe_psch_shp_credit_tbl_get(dev_id, port_id, &psch_shp_credit_tbl);

	hppe_psch_shp_sign_tbl_get(dev_id, port_id, &psch_shp_sign_tbl);


	token_number->c_token_number_negative_en = psch_shp_sign_tbl.bf.shaper_credit_neg;
	token_number->c_token_number = psch_shp_credit_tbl.bf.shaper_credit;

	return SW_OK;
}
#endif

sw_error_t
adpt_hppe_flow_shaper_token_number_set(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_token_number_t *token_number)
{
	union l1_shp_credit_tbl_u l1_shp_credit_tbl;

	memset(&l1_shp_credit_tbl, 0, sizeof(l1_shp_credit_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(token_number);

	if ((flow_id < 0) || (flow_id > 63))
		return SW_BAD_PARAM;


	l1_shp_credit_tbl.bf.c_shaper_credit_neg = token_number->c_token_number_negative_en;
	l1_shp_credit_tbl.bf.c_shaper_credit = token_number->c_token_number;
	l1_shp_credit_tbl.bf.e_shaper_credit_neg = token_number->e_token_number_negative_en;
	l1_shp_credit_tbl.bf.e_shaper_credit_0 = token_number->e_token_number & 0x1;
	l1_shp_credit_tbl.bf.e_shaper_credit_1 = token_number->e_token_number >> 1;

	hppe_l1_shp_credit_tbl_set(dev_id, flow_id, &l1_shp_credit_tbl);

	return SW_OK;
}
#ifndef IN_SHAPER_MINI
sw_error_t
adpt_hppe_flow_shaper_token_number_get(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_token_number_t *token_number)
{
	sw_error_t rv = SW_OK;
	union l1_shp_credit_tbl_u l1_shp_credit_tbl;

	memset(&l1_shp_credit_tbl, 0, sizeof(l1_shp_credit_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(token_number);

	if ((flow_id < 0) || (flow_id > 63))
		return SW_BAD_PARAM;

	rv = hppe_l1_shp_credit_tbl_get(dev_id, flow_id, &l1_shp_credit_tbl);

	if( rv != SW_OK )
		return rv;

	token_number->c_token_number_negative_en = l1_shp_credit_tbl.bf.c_shaper_credit_neg;
	token_number->c_token_number = l1_shp_credit_tbl.bf.c_shaper_credit;
	token_number->e_token_number_negative_en = l1_shp_credit_tbl.bf.e_shaper_credit_neg;
	token_number->e_token_number = l1_shp_credit_tbl.bf.e_shaper_credit_0 |
		(l1_shp_credit_tbl.bf.e_shaper_credit_1 << 1);

	return SW_OK;
}
#endif

sw_error_t
adpt_hppe_flow_shaper_set(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_config_t * shaper)
{
	sw_error_t rv = SW_OK;
	union l1_shp_cfg_tbl_u l1_shp_cfg_tbl;
	union l1_comp_cfg_tbl_u l1_comp_cfg_tbl;
	a_uint32_t hppe_cir = 0, hppe_cbs = 0, hppe_eir= 0, hppe_ebs = 0;
	a_uint64_t temp_cir = 0, temp_eir =0, temp_cbs =0, temp_ebs = 0;
	a_uint32_t token_unit = 0;
	fal_shaper_token_number_t token_number;

	memset(&l1_shp_cfg_tbl, 0, sizeof(l1_shp_cfg_tbl));
	memset(&l1_comp_cfg_tbl, 0, sizeof(l1_comp_cfg_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(shaper);

	if ((flow_id < 0) || (flow_id > 63))
		return SW_BAD_PARAM;

	if(ADPT_HPPE_SHAPER_METER_UNIT_BYTE == shaper->meter_unit)
	{
		if ((shaper->cir > BYTE_SHAPER_MAX_RATE) || (shaper->eir > BYTE_SHAPER_MAX_RATE))
			return SW_BAD_PARAM;
		if ((shaper->cir < BYTE_SHAPER_MIN_RATE) && (shaper->cir != 0))
			return SW_BAD_PARAM;
		if ((shaper->eir < BYTE_SHAPER_MIN_RATE) && (shaper->eir != 0))
			return SW_BAD_PARAM;
	}
	if(ADPT_HPPE_SHAPER_METER_UNIT_FRAME == shaper->meter_unit)
	{
		if ((shaper->cir > FRAME_SHAPER_MAX_RATE) || (shaper->eir > FRAME_SHAPER_MAX_RATE))
			return SW_BAD_PARAM;
		if ((shaper->cir < FRAME_SHAPER_MIN_RATE) && (shaper->cir != 0))
			return SW_BAD_PARAM;
		if ((shaper->eir < FRAME_SHAPER_MIN_RATE) && (shaper->eir != 0))
			return SW_BAD_PARAM;
	}

	hppe_l1_comp_cfg_tbl_get(dev_id, flow_id, &l1_comp_cfg_tbl);

	temp_cir = ((a_uint64_t)shaper->cir) * 1000;
	temp_cbs = ((a_uint64_t)shaper->cbs) * 1000;
	temp_eir = ((a_uint64_t)shaper->eir) * 1000;
	temp_ebs = ((a_uint64_t)shaper->ebs) * 1000;

	rv = __adpt_hppe_shaper_two_bucket_parameter_select(ADPT_HPPE_FLOW_SHAPER,
								temp_cir,
								temp_cbs,
								temp_eir,
								temp_ebs,
								shaper->meter_unit,
								&token_unit);
	if( rv != SW_OK )
		return rv;
	//printk("flow shaper meter unit is = %d\n", shaper->meter_unit);
	//printk("flow shaper token unit is = %d\n", token_unit);

		__adpt_hppe_shaper_rate_to_refresh(ADPT_HPPE_FLOW_SHAPER,
									shaper->cir,
									&hppe_cir,
									shaper->meter_unit,
									token_unit);

		__adpt_hppe_shaper_rate_to_refresh(ADPT_HPPE_FLOW_SHAPER,
									shaper->eir,
									&hppe_eir,
									shaper->meter_unit,
									token_unit);

		__adpt_hppe_shaper_burst_size_to_bucket_size(shaper->cbs,
									&hppe_cbs,
									shaper->meter_unit,
									token_unit);

		__adpt_hppe_shaper_burst_size_to_bucket_size(shaper->ebs,
									&hppe_ebs,
									shaper->meter_unit,
									token_unit);

	l1_shp_cfg_tbl.bf.cf = shaper->couple_en;
	l1_shp_cfg_tbl.bf.meter_unit = shaper->meter_unit;
	l1_shp_cfg_tbl.bf.c_shaper_enable = shaper->c_shaper_en;
	l1_shp_cfg_tbl.bf.cbs = hppe_cbs;
	l1_shp_cfg_tbl.bf.cir = hppe_cir;
	l1_shp_cfg_tbl.bf.e_shaper_enable = shaper->e_shaper_en;
	l1_shp_cfg_tbl.bf.ebs = hppe_ebs;
	l1_shp_cfg_tbl.bf.eir = hppe_eir;
	l1_shp_cfg_tbl.bf.token_unit = token_unit;
	l1_comp_cfg_tbl.bf.shaper_meter_len = shaper->shaper_frame_mode;

	hppe_l1_shp_cfg_tbl_set(dev_id, flow_id, &l1_shp_cfg_tbl);

	hppe_l1_comp_cfg_tbl_set(dev_id, flow_id, &l1_comp_cfg_tbl);

	if( A_FALSE == shaper->c_shaper_en)
	{
		token_number.c_token_number_negative_en = 0;
		token_number.c_token_number = 0;
		adpt_hppe_flow_shaper_token_number_set(dev_id, flow_id, &token_number);
	}
	if( A_FALSE == shaper->e_shaper_en)
	{
		token_number.e_token_number_negative_en = 0;
		token_number.e_token_number = 0;
		adpt_hppe_flow_shaper_token_number_set(dev_id, flow_id, &token_number);
	}

	return SW_OK;
}
sw_error_t
adpt_hppe_port_shaper_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_shaper_config_t * shaper)
{
	sw_error_t rv = SW_OK;
	union psch_shp_cfg_tbl_u psch_shp_cfg_tbl;
	union psch_comp_cfg_tbl_u psch_comp_cfg_tbl;
	a_uint64_t temp_cir = 0,temp_cbs;
	a_uint32_t hppe_cir = 0, hppe_cbs = 0;
	a_uint32_t token_unit = 0;
	fal_shaper_token_number_t token_number;

	memset(&psch_shp_cfg_tbl, 0, sizeof(psch_shp_cfg_tbl));
	memset(&psch_comp_cfg_tbl, 0, sizeof(psch_comp_cfg_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(shaper);

	if (port_id < 0 || port_id > 7)
		return SW_BAD_PARAM;

	if(ADPT_HPPE_SHAPER_METER_UNIT_BYTE == shaper->meter_unit)
	{
		if (shaper->cir > BYTE_SHAPER_MAX_RATE)
			return SW_BAD_PARAM;
		if ((shaper->cir < BYTE_SHAPER_MIN_RATE) && (shaper->cir != 0))
			return SW_BAD_PARAM;
	}
	if(ADPT_HPPE_SHAPER_METER_UNIT_FRAME == shaper->meter_unit)
	{
		if (shaper->cir > FRAME_SHAPER_MAX_RATE)
			return SW_BAD_PARAM;
		if ((shaper->cir < FRAME_SHAPER_MIN_RATE) && (shaper->cir != 0))
			return SW_BAD_PARAM;
	}

	temp_cir = ((a_uint64_t)shaper->cir) * 1000;
	temp_cbs = ((a_uint64_t)shaper->cbs) * 1000;

	rv = __adpt_hppe_shaper_one_bucket_parameter_select(ADPT_HPPE_PORT_SHAPER,
								temp_cir,
								temp_cbs,
								shaper->meter_unit,
								&token_unit);
	if( rv != SW_OK )
		return rv;

	// printk("current shaper meter unit is = %d\n", shaper->meter_unit);
	// printk("current shaper token unit is = %d\n", token_unit);

	__adpt_hppe_shaper_rate_to_refresh(ADPT_HPPE_PORT_SHAPER,
								shaper->cir,
								&hppe_cir,
								shaper->meter_unit,
								token_unit);

	__adpt_hppe_shaper_burst_size_to_bucket_size(shaper->cbs,
								&hppe_cbs,
								shaper->meter_unit,
								token_unit);

	psch_shp_cfg_tbl.bf.meter_unit = shaper->meter_unit;
	psch_shp_cfg_tbl.bf.shaper_enable = shaper->c_shaper_en;
	psch_shp_cfg_tbl.bf.cbs = hppe_cbs;
	psch_shp_cfg_tbl.bf.cir = hppe_cir;
	psch_shp_cfg_tbl.bf.token_unit = token_unit;

	psch_comp_cfg_tbl.bf.shaper_meter_len = shaper->shaper_frame_mode;

	hppe_psch_shp_cfg_tbl_set(dev_id, port_id, &psch_shp_cfg_tbl);

	hppe_psch_comp_cfg_tbl_set(dev_id, port_id, &psch_comp_cfg_tbl);

	if( A_FALSE == shaper->c_shaper_en)
	{
		token_number.c_token_number_negative_en = 0;
		token_number.c_token_number = 0;
		adpt_hppe_port_shaper_token_number_set(dev_id, port_id, &token_number);
	}

	return SW_OK;
}
sw_error_t
adpt_hppe_port_shaper_time_slot_set(a_uint32_t dev_id, a_uint32_t time_slot)
{
	sw_error_t rv = SW_OK;
	union shp_slot_cfg_port_u shp_slot_cfg_port;

	memset(&shp_slot_cfg_port, 0, sizeof(shp_slot_cfg_port));
	ADPT_DEV_ID_CHECK(dev_id);

	if ((time_slot < 0x8) || (time_slot > 0xfff))
		return SW_BAD_PARAM;

	rv = hppe_shp_slot_cfg_port_get(dev_id, &shp_slot_cfg_port);

	if( rv != SW_OK )
		return rv;

	shp_slot_cfg_port.bf.port_shp_slot_time = time_slot;
	hppe_shp_slot_cfg_port_set(dev_id, &shp_slot_cfg_port);

	__adpt_hppe_port_shaper_max_rate(time_slot);

	__adpt_hppe_shaper_max_burst_size();

	return SW_OK;

}
#ifndef IN_SHAPER_MINI
sw_error_t
adpt_hppe_flow_shaper_get(a_uint32_t dev_id, a_uint32_t flow_id,
		fal_shaper_config_t * shaper)
{
	sw_error_t rv = SW_OK;
	union l1_shp_cfg_tbl_u l1_shp_cfg_tbl;
	union l1_comp_cfg_tbl_u l1_comp_cfg_tbl;
	a_uint32_t hppe_cir =0, hppe_cbs = 0, hppe_eir = 0, hppe_ebs = 0;

	memset(&l1_shp_cfg_tbl, 0, sizeof(l1_shp_cfg_tbl));
	memset(&l1_comp_cfg_tbl, 0, sizeof(l1_comp_cfg_tbl));

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(shaper);

	if (flow_id < 0 || flow_id > 63)
		return SW_BAD_PARAM;

	hppe_l1_comp_cfg_tbl_get(dev_id, flow_id, &l1_comp_cfg_tbl);

	rv = hppe_l1_shp_cfg_tbl_get(dev_id, flow_id, &l1_shp_cfg_tbl);

	if( rv != SW_OK )
		return rv;

	hppe_cir = l1_shp_cfg_tbl.bf.cir;
	hppe_cbs = l1_shp_cfg_tbl.bf.cbs;
	hppe_eir = l1_shp_cfg_tbl.bf.eir;
	hppe_ebs= l1_shp_cfg_tbl.bf.ebs;

	__adpt_hppe_shaper_refresh_to_rate(ADPT_HPPE_FLOW_SHAPER,
								hppe_cir,
								&shaper->cir,
								l1_shp_cfg_tbl.bf.meter_unit,
								l1_shp_cfg_tbl.bf.token_unit);

	__adpt_hppe_shaper_refresh_to_rate(ADPT_HPPE_FLOW_SHAPER,
								hppe_eir,
								&shaper->eir,
								l1_shp_cfg_tbl.bf.meter_unit,
								l1_shp_cfg_tbl.bf.token_unit);


	__adpt_hppe_shaper_bucket_size_to_burst_size(hppe_cbs,
								&shaper->cbs,
								l1_shp_cfg_tbl.bf.meter_unit,
								l1_shp_cfg_tbl.bf.token_unit);

	__adpt_hppe_shaper_bucket_size_to_burst_size(hppe_ebs,
								&shaper->ebs,
								l1_shp_cfg_tbl.bf.meter_unit,
								l1_shp_cfg_tbl.bf.token_unit);

	shaper->couple_en = l1_shp_cfg_tbl.bf.cf;
	shaper->meter_unit = l1_shp_cfg_tbl.bf.meter_unit;
	shaper->c_shaper_en = l1_shp_cfg_tbl.bf.c_shaper_enable;
	shaper->e_shaper_en = l1_shp_cfg_tbl.bf.e_shaper_enable;

	shaper->shaper_frame_mode = l1_comp_cfg_tbl.bf.shaper_meter_len;

	return SW_OK;
}
#endif

sw_error_t
adpt_hppe_queue_shaper_set(a_uint32_t dev_id,a_uint32_t queue_id,
		fal_shaper_config_t * shaper)
{
	sw_error_t rv = SW_OK;
	union l0_shp_cfg_tbl_u l0_shp_cfg_tbl;
	union l0_comp_cfg_tbl_u l0_comp_cfg_tbl;
	a_uint32_t hppe_cir = 0, hppe_cbs = 0, hppe_eir= 0, hppe_ebs = 0;
	a_uint32_t token_unit = 0;
	a_uint64_t temp_cir = 0, temp_eir =0, temp_cbs =0, temp_ebs = 0;
	fal_shaper_token_number_t token_number;

	memset(&l0_shp_cfg_tbl, 0, sizeof(l0_shp_cfg_tbl));
	memset(&l0_comp_cfg_tbl, 0, sizeof(l0_comp_cfg_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(shaper);

	if (queue_id < 0 || queue_id > 299)
		return SW_BAD_PARAM;

	if(ADPT_HPPE_SHAPER_METER_UNIT_BYTE == shaper->meter_unit)
	{
		if ((shaper->cir > BYTE_SHAPER_MAX_RATE) || (shaper->eir > BYTE_SHAPER_MAX_RATE))
			return SW_BAD_PARAM;
		if ((shaper->cir < BYTE_SHAPER_MIN_RATE) && (shaper->cir != 0))
			return SW_BAD_PARAM;
		if ((shaper->eir < BYTE_SHAPER_MIN_RATE) && (shaper->eir != 0))
			return SW_BAD_PARAM;
	}
	if(ADPT_HPPE_SHAPER_METER_UNIT_FRAME == shaper->meter_unit)
	{
		if ((shaper->cir > FRAME_SHAPER_MAX_RATE) || (shaper->eir > FRAME_SHAPER_MAX_RATE))
			return SW_BAD_PARAM;
		if ((shaper->cir < FRAME_SHAPER_MIN_RATE) && (shaper->cir != 0))
			return SW_BAD_PARAM;
		if ((shaper->eir < FRAME_SHAPER_MIN_RATE) && (shaper->eir != 0))
			return SW_BAD_PARAM;
	}

	hppe_l0_comp_cfg_tbl_get(dev_id, queue_id, &l0_comp_cfg_tbl);

	temp_cir = ((a_uint64_t)shaper->cir) * 1000;
	temp_cbs = ((a_uint64_t)shaper->cbs) * 1000;
	temp_eir = ((a_uint64_t)shaper->eir) * 1000;
	temp_ebs = ((a_uint64_t)shaper->ebs) * 1000;

	rv = __adpt_hppe_shaper_two_bucket_parameter_select(ADPT_HPPE_QUEUE_SHAPER,
								temp_cir,
								temp_cbs,
								temp_eir,
								temp_ebs,
								shaper->meter_unit,
								&token_unit);
	if( rv != SW_OK )
		return rv;
	//printk("queue shaper meter unit is = %d\n", shaper->meter_unit);
	//printk("queue shaper token unit is = %d\n", token_unit);

		__adpt_hppe_shaper_rate_to_refresh(ADPT_HPPE_QUEUE_SHAPER,
									shaper->cir,
									&hppe_cir,
									shaper->meter_unit,
									token_unit);

		__adpt_hppe_shaper_rate_to_refresh(ADPT_HPPE_QUEUE_SHAPER,
									shaper->eir,
									&hppe_eir,
									shaper->meter_unit,
									token_unit);

		__adpt_hppe_shaper_burst_size_to_bucket_size(shaper->cbs,
									&hppe_cbs,
									shaper->meter_unit,
									token_unit);

		__adpt_hppe_shaper_burst_size_to_bucket_size(shaper->ebs,
									&hppe_ebs,
									shaper->meter_unit,
									token_unit);

	l0_shp_cfg_tbl.bf.cf = shaper->couple_en;
	l0_shp_cfg_tbl.bf.meter_unit = shaper->meter_unit;
	l0_shp_cfg_tbl.bf.c_shaper_enable = shaper->c_shaper_en;
	l0_shp_cfg_tbl.bf.cbs = hppe_cbs;
	l0_shp_cfg_tbl.bf.cir = hppe_cir;
	l0_shp_cfg_tbl.bf.e_shaper_enable = shaper->e_shaper_en;
	l0_shp_cfg_tbl.bf.ebs = hppe_ebs;
	l0_shp_cfg_tbl.bf.eir = hppe_eir;
	l0_shp_cfg_tbl.bf.token_unit = token_unit;
	l0_comp_cfg_tbl.bf.shaper_meter_len = shaper->shaper_frame_mode;

	hppe_l0_shp_cfg_tbl_set(dev_id, queue_id, &l0_shp_cfg_tbl);

	hppe_l0_comp_cfg_tbl_set(dev_id, queue_id, &l0_comp_cfg_tbl);

	if( A_FALSE == shaper->c_shaper_en)
	{
		token_number.c_token_number_negative_en = 0;
		token_number.c_token_number = 0;
		adpt_hppe_queue_shaper_token_number_set(dev_id, queue_id, &token_number);
	}
	if( A_FALSE == shaper->e_shaper_en)
	{
		token_number.e_token_number_negative_en = 0;
		token_number.e_token_number = 0;
		adpt_hppe_queue_shaper_token_number_set(dev_id, queue_id, &token_number);
	}

	return SW_OK;

}
sw_error_t
adpt_hppe_queue_shaper_time_slot_set(a_uint32_t dev_id, a_uint32_t time_slot)
{
	union shp_slot_cfg_l0_u shp_slot_cfg_l0;
	sw_error_t rv = SW_OK;

	memset(&shp_slot_cfg_l0, 0, sizeof(shp_slot_cfg_l0));
	ADPT_DEV_ID_CHECK(dev_id);

	if ((time_slot < 0x12c) || (time_slot > 0xfff))
		return SW_BAD_PARAM;

	rv = hppe_shp_slot_cfg_l0_get(dev_id, &shp_slot_cfg_l0);

	if( rv != SW_OK )
		return rv;

	shp_slot_cfg_l0.bf.l0_shp_slot_time = time_slot;
	hppe_shp_slot_cfg_l0_set(dev_id, &shp_slot_cfg_l0);

	__adpt_hppe_queue_shaper_max_rate(time_slot);

	return SW_OK;

}

sw_error_t
adpt_hppe_shaper_ipg_preamble_length_set(a_uint32_t dev_id, a_uint32_t ipg_pre_length)
{
	union ipg_pre_len_cfg_u ipg_pre_len_cfg;

	memset(&ipg_pre_len_cfg, 0, sizeof(ipg_pre_len_cfg));
	ADPT_DEV_ID_CHECK(dev_id);

	if (ipg_pre_length > 0x1f)
		return SW_BAD_PARAM;

	hppe_ipg_pre_len_cfg_get(dev_id, &ipg_pre_len_cfg);
	ipg_pre_len_cfg.bf.ipg_pre_len = ipg_pre_length;

	hppe_ipg_pre_len_cfg_set(dev_id, &ipg_pre_len_cfg);

	return SW_OK;
}

#ifndef IN_SHAPER_MINI
sw_error_t
adpt_hppe_shaper_ipg_preamble_length_get(a_uint32_t dev_id, a_uint32_t *ipg_pre_length)
{
	sw_error_t rv = SW_OK;
	union ipg_pre_len_cfg_u ipg_pre_len_cfg;

	memset(&ipg_pre_len_cfg, 0, sizeof(ipg_pre_len_cfg));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ipg_pre_length);

	rv = hppe_ipg_pre_len_cfg_get(dev_id, &ipg_pre_len_cfg);

	if( rv != SW_OK )
		return rv;

	*ipg_pre_length = ipg_pre_len_cfg.bf.ipg_pre_len;

	return SW_OK;
}
#endif

void adpt_hppe_shaper_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_shaper_func_bitmap = ((1 << FUNC_ADPT_FLOW_SHAPER_SET)|
						(1 << FUNC_ADPT_QUEUE_SHAPER_GET)|
						(1 << FUNC_ADPT_QUEUE_SHAPER_TOKEN_NUMBER_SET)|
						(1 << FUNC_ADPT_PORT_SHAPER_GET)|
						(1 << FUNC_ADPT_FLOW_SHAPER_TIME_SLOT_GET)|
						(1 << FUNC_ADPT_PORT_SHAPER_TIME_SLOT_GET)|
						(1 << FUNC_ADPT_FLOW_SHAPER_TIME_SLOT_SET)|
						(1 << FUNC_ADPT_PORT_SHAPER_TOKEN_NUMBER_SET)|
						(1 << FUNC_ADPT_QUEUE_SHAPER_TOKEN_NUMBER_GET)|
						(1 << FUNC_ADPT_QUEUE_SHAPER_TIME_SLOT_GET)|
						(1 << FUNC_ADPT_PORT_SHAPER_TOKEN_NUMBER_GET)|
						(1 << FUNC_ADPT_FLOW_SHAPER_TOKEN_NUMBER_SET)|
						(1 << FUNC_ADPT_FLOW_SHAPER_TOKEN_NUMBER_GET)|
						(1 << FUNC_ADPT_PORT_SHAPER_SET)|
						(1 << FUNC_ADPT_PORT_SHAPER_TIME_SLOT_SET)|
						(1 << FUNC_ADPT_FLOW_SHAPER_GET)|
						(1 << FUNC_ADPT_QUEUE_SHAPER_SET)|
						(1 << FUNC_ADPT_QUEUE_SHAPER_TIME_SLOT_SET)|
						(1 << FUNC_ADPT_SHAPER_IPG_PREAMBLE_LENGTH_SET)|
						(1 << FUNC_ADPT_SHAPER_IPG_PREAMBLE_LENGTH_GET));

	return;

}

static void adpt_hppe_shaper_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_flow_shaper_set = NULL;
	p_adpt_api->adpt_queue_shaper_get = NULL;
	p_adpt_api->adpt_queue_shaper_token_number_set = NULL;
	p_adpt_api->adpt_port_shaper_get = NULL;
	p_adpt_api->adpt_flow_shaper_time_slot_get = NULL;
	p_adpt_api->adpt_port_shaper_time_slot_get = NULL;
	p_adpt_api->adpt_flow_shaper_time_slot_set = NULL;
	p_adpt_api->adpt_port_shaper_token_number_set = NULL;
	p_adpt_api->adpt_queue_shaper_token_number_get = NULL;
	p_adpt_api->adpt_queue_shaper_time_slot_get = NULL;
	p_adpt_api->adpt_port_shaper_token_number_get = NULL;
	p_adpt_api->adpt_flow_shaper_token_number_set = NULL;
	p_adpt_api->adpt_flow_shaper_token_number_get = NULL;
	p_adpt_api->adpt_port_shaper_set = NULL;
	p_adpt_api->adpt_port_shaper_time_slot_set = NULL;
	p_adpt_api->adpt_flow_shaper_get = NULL;
	p_adpt_api->adpt_queue_shaper_set = NULL;
	p_adpt_api->adpt_queue_shaper_time_slot_set = NULL;
	p_adpt_api->adpt_shaper_ipg_preamble_length_set = NULL;
	p_adpt_api->adpt_shaper_ipg_preamble_length_get = NULL;

	return;

}

sw_error_t adpt_hppe_shaper_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_shaper_func_unregister(dev_id, p_adpt_api);

#ifndef IN_SHAPER_MINI
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_PORT_SHAPER_GET))
	{
		p_adpt_api->adpt_port_shaper_get = adpt_hppe_port_shaper_get;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_FLOW_SHAPER_TIME_SLOT_GET))
	{
		p_adpt_api->adpt_flow_shaper_time_slot_get = adpt_hppe_flow_shaper_time_slot_get;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_PORT_SHAPER_TIME_SLOT_GET))
	{
		p_adpt_api->adpt_port_shaper_time_slot_get = adpt_hppe_port_shaper_time_slot_get;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_QUEUE_SHAPER_TOKEN_NUMBER_GET))
	{
		p_adpt_api->adpt_queue_shaper_token_number_get =
			adpt_hppe_queue_shaper_token_number_get;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_QUEUE_SHAPER_TIME_SLOT_GET))
	{
		p_adpt_api->adpt_queue_shaper_time_slot_get = adpt_hppe_queue_shaper_time_slot_get;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_PORT_SHAPER_TOKEN_NUMBER_GET))
	{
		p_adpt_api->adpt_port_shaper_token_number_get =
			adpt_hppe_port_shaper_token_number_get;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_FLOW_SHAPER_TOKEN_NUMBER_GET))
	{
		p_adpt_api->adpt_flow_shaper_token_number_get =
			adpt_hppe_flow_shaper_token_number_get;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_FLOW_SHAPER_GET))
	{
		p_adpt_api->adpt_flow_shaper_get = adpt_hppe_flow_shaper_get;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_SHAPER_IPG_PREAMBLE_LENGTH_GET))
	{
		p_adpt_api->adpt_shaper_ipg_preamble_length_get =
			adpt_hppe_shaper_ipg_preamble_length_get;
	}
#endif
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_FLOW_SHAPER_SET))
	{
		p_adpt_api->adpt_flow_shaper_set = adpt_hppe_flow_shaper_set;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_QUEUE_SHAPER_SET))
	{
		p_adpt_api->adpt_queue_shaper_set = adpt_hppe_queue_shaper_set;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_QUEUE_SHAPER_GET))
	{
		p_adpt_api->adpt_queue_shaper_get = adpt_hppe_queue_shaper_get;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_PORT_SHAPER_SET))
	{
		p_adpt_api->adpt_port_shaper_set = adpt_hppe_port_shaper_set;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_SHAPER_IPG_PREAMBLE_LENGTH_SET))
	{
		p_adpt_api->adpt_shaper_ipg_preamble_length_set =
			adpt_hppe_shaper_ipg_preamble_length_set;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_QUEUE_SHAPER_TOKEN_NUMBER_SET))
	{
		p_adpt_api->adpt_queue_shaper_token_number_set =
			adpt_hppe_queue_shaper_token_number_set;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_PORT_SHAPER_TOKEN_NUMBER_SET))
	{
		p_adpt_api->adpt_port_shaper_token_number_set =
			adpt_hppe_port_shaper_token_number_set;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_FLOW_SHAPER_TOKEN_NUMBER_SET))
	{
		p_adpt_api->adpt_flow_shaper_token_number_set =
			adpt_hppe_flow_shaper_token_number_set;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_PORT_SHAPER_TIME_SLOT_SET))
	{
		p_adpt_api->adpt_port_shaper_time_slot_set = adpt_hppe_port_shaper_time_slot_set;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_FLOW_SHAPER_TIME_SLOT_SET))
	{
		p_adpt_api->adpt_flow_shaper_time_slot_set = adpt_hppe_flow_shaper_time_slot_set;
	}
	if(p_adpt_api->adpt_shaper_func_bitmap & (1 << FUNC_ADPT_QUEUE_SHAPER_TIME_SLOT_SET))
	{
		p_adpt_api->adpt_queue_shaper_time_slot_set = adpt_hppe_queue_shaper_time_slot_set;
	}

	return SW_OK;
}

/**
 * @}
 */
