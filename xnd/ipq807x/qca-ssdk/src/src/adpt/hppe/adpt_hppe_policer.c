/*
 * Copyright (c) 2016-2017, 2021, The Linux Foundation. All rights reserved.
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
#include "hppe_policer_reg.h"
#include "hppe_policer.h"
#include "adpt.h"

#define NR_ADPT_HPPE_POLICER_METER_UNIT         2
#define NR_ADPT_HPPE_POLICER_METER_TOKEN_UNIT         8
#define ADPT_HPPE_POLICER_METER_UNIT_BYTE       0
#define ADPT_HPPE_POLICER_METER_UNIT_FRAME      1
#define ADPT_HPPE_FREQUENCY      300  /*MHZ*/
#define ADPT_HPPE_POLICER_BURST_SIZE_UNIT      65536
#define ADPT_HPPE_POLICER_REFRESH_BITS  18
#define ADPT_HPPE_POLICER_BUCKET_SIZE_BITS  16
#define ADPT_HPPE_POLICER_REFRESH_MAX  ((1 << ADPT_HPPE_POLICER_REFRESH_BITS) - 1)
#define ADPT_HPPE_POLICER_BUCKET_SIZE_MAX  ((1 << ADPT_HPPE_POLICER_BUCKET_SIZE_BITS) - 1)
#define BYTE_POLICER_MAX_RATE 10000000
#define BYTE_POLICER_MIN_RATE 64
#define FRAME_POLICER_MAX_RATE 14881000
#define FRAME_POLICER_MIN_RATE 6


static a_uint32_t hppe_policer_token_unit[NR_ADPT_HPPE_POLICER_METER_UNIT]
	[NR_ADPT_HPPE_POLICER_METER_TOKEN_UNIT] = {{2048 * 8,
	512 * 8,128 * 8,32 * 8,8 * 8,2 * 8, 4, 1},
	{2097152,524288,131072,32768,8192,2048,512,128}};

typedef struct
{
    a_uint64_t rate_1bit;
    a_uint64_t rate_max;
} adpt_hppe_policer_rate_t;

typedef struct
{
    a_uint64_t burst_size_1bit;
    a_uint64_t burst_size_max;
} adpt_hppe_policer_burst_size_t;

static adpt_hppe_policer_rate_t
hppe_policer_rate[NR_ADPT_HPPE_POLICER_METER_UNIT][NR_ADPT_HPPE_POLICER_METER_TOKEN_UNIT] =
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

static adpt_hppe_policer_burst_size_t
hppe_policer_burst_size[NR_ADPT_HPPE_POLICER_METER_UNIT][NR_ADPT_HPPE_POLICER_METER_TOKEN_UNIT] =
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

#ifndef IN_POLICER_MINI
sw_error_t
adpt_hppe_acl_policer_counter_get(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_counter_t *counter)
{
	union in_acl_meter_cnt_tbl_u in_acl_meter_cnt_tbl;

	memset(&in_acl_meter_cnt_tbl, 0, sizeof(in_acl_meter_cnt_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(counter);

	if (index < 0 || index > 511)
		return SW_BAD_PARAM;

	hppe_in_acl_meter_cnt_tbl_get(dev_id, index * 3, &in_acl_meter_cnt_tbl);
	counter->green_packet_counter = in_acl_meter_cnt_tbl.bf.pkt_cnt;
	counter->green_byte_counter = in_acl_meter_cnt_tbl.bf.byte_cnt_1;
	counter->green_byte_counter = (counter->green_byte_counter << 32) | in_acl_meter_cnt_tbl.bf.byte_cnt_0;

	hppe_in_acl_meter_cnt_tbl_get(dev_id, index * 3 + 1, &in_acl_meter_cnt_tbl);
	counter->yellow_packet_counter = in_acl_meter_cnt_tbl.bf.pkt_cnt;
	counter->yellow_byte_counter = in_acl_meter_cnt_tbl.bf.byte_cnt_1;
	counter->yellow_byte_counter = (counter->yellow_byte_counter << 32) | in_acl_meter_cnt_tbl.bf.byte_cnt_0;

	hppe_in_acl_meter_cnt_tbl_get(dev_id, index * 3 + 2, &in_acl_meter_cnt_tbl);
	counter->red_packet_counter = in_acl_meter_cnt_tbl.bf.pkt_cnt;
	counter->red_byte_counter = in_acl_meter_cnt_tbl.bf.byte_cnt_1;
	counter->red_byte_counter = (counter->red_byte_counter << 32) | in_acl_meter_cnt_tbl.bf.byte_cnt_0;

	return SW_OK;
}

sw_error_t
adpt_hppe_port_policer_counter_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_counter_t *counter)
{
	union in_port_meter_cnt_tbl_u in_port_meter_cnt_tbl;

	memset(&in_port_meter_cnt_tbl, 0, sizeof(in_port_meter_cnt_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(counter);

	if (port_id < 0 || port_id > 7)
		return SW_BAD_PARAM;

	hppe_in_port_meter_cnt_tbl_get(dev_id, port_id * 3, &in_port_meter_cnt_tbl);
	counter->green_packet_counter = in_port_meter_cnt_tbl.bf.pkt_cnt;
	counter->green_byte_counter = in_port_meter_cnt_tbl.bf.byte_cnt_1;
	counter->green_byte_counter = (counter->green_byte_counter << 32) | in_port_meter_cnt_tbl.bf.byte_cnt_0;

	hppe_in_port_meter_cnt_tbl_get(dev_id, port_id * 3 + 1, &in_port_meter_cnt_tbl);
	counter->yellow_packet_counter = in_port_meter_cnt_tbl.bf.pkt_cnt;
	counter->yellow_byte_counter = in_port_meter_cnt_tbl.bf.byte_cnt_1;
	counter->yellow_byte_counter = (counter->yellow_byte_counter << 32) | in_port_meter_cnt_tbl.bf.byte_cnt_0;

	hppe_in_port_meter_cnt_tbl_get(dev_id, port_id * 3 + 2, &in_port_meter_cnt_tbl);
	counter->red_packet_counter = in_port_meter_cnt_tbl.bf.pkt_cnt;
	counter->red_byte_counter = in_port_meter_cnt_tbl.bf.byte_cnt_1;
	counter->red_byte_counter = (counter->red_byte_counter << 32) | in_port_meter_cnt_tbl.bf.byte_cnt_0;

	return SW_OK;
}

sw_error_t
adpt_hppe_port_compensation_byte_get(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t  *length)
{
	sw_error_t rv = SW_OK;
	union meter_cmpst_length_reg_u meter_cmpst_length_reg;

	memset(&meter_cmpst_length_reg, 0, sizeof(meter_cmpst_length_reg));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(length);

	if (port_id < 0 || port_id > 7)
		return SW_BAD_PARAM;

	rv = hppe_meter_cmpst_length_reg_get(dev_id, port_id, &meter_cmpst_length_reg);

	if( rv != SW_OK )
		return rv;

	*length = meter_cmpst_length_reg.bf.cmpst_length;

	return SW_OK;
}

static sw_error_t
__adpt_hppe_policer_rate_to_refresh(a_uint32_t rate,
							a_uint32_t *refresh,
							a_bool_t meter_unit,
							a_uint32_t  token_unit)
{
	a_uint32_t temp_refresh;
	a_uint64_t temp_rate;

	if(hppe_policer_rate[meter_unit][token_unit].rate_1bit > 0)
	{
		temp_rate = ((a_uint64_t)rate) * 1000;
		do_div(temp_rate, hppe_policer_rate[meter_unit][token_unit].rate_1bit);
		temp_refresh = temp_rate;
	}
	else
	{
		return SW_BAD_PARAM;;
	}

	if (temp_refresh > ADPT_HPPE_POLICER_REFRESH_MAX)
	{
		temp_refresh = ADPT_HPPE_POLICER_REFRESH_MAX;
	}

	*refresh = temp_refresh;

	return SW_OK;
}

static sw_error_t
__adpt_hppe_policer_burst_size_to_bucket_size(a_uint32_t burst_size,
							a_uint32_t *bucket_size,
							a_bool_t meter_unit,
							a_uint32_t  token_unit)
{
	a_uint32_t temp_bucket_size;
	a_uint64_t temp_burst_size;

	if(hppe_policer_burst_size[meter_unit][token_unit].burst_size_1bit > 0)
	{
		temp_burst_size = ((a_uint64_t)burst_size) * 1000;
		do_div(temp_burst_size, hppe_policer_burst_size[meter_unit][token_unit].burst_size_1bit);
		temp_bucket_size = temp_burst_size;
	}
	else
	{
		return SW_BAD_PARAM;
	}

	if(temp_bucket_size > ADPT_HPPE_POLICER_BUCKET_SIZE_MAX)
	{
		temp_bucket_size = ADPT_HPPE_POLICER_BUCKET_SIZE_MAX;
	}

	*bucket_size = temp_bucket_size;

	return SW_OK;
}

static sw_error_t
__adpt_hppe_policer_refresh_to_rate(a_uint32_t refresh,
							a_uint32_t *rate,
							a_bool_t meter_unit,
							a_uint32_t  token_unit)
{
	a_uint32_t temp_rate;
	a_uint64_t temp_refresh;

	if(hppe_policer_rate[meter_unit][token_unit].rate_1bit > 0)
	{
		temp_refresh = ((a_uint64_t)refresh) * hppe_policer_rate[meter_unit][token_unit].rate_1bit;
		do_div(temp_refresh, 1000);
		temp_rate = temp_refresh;
	}
	else
	{
		return SW_BAD_PARAM;;
	}

	*rate = temp_rate;

	return SW_OK;
}

static sw_error_t
__adpt_hppe_policer_bucket_size_to_burst_size(a_uint32_t bucket_size,
							a_uint32_t *burst_size,
							a_bool_t meter_unit,
							a_uint32_t  token_unit)
{
	a_uint32_t temp_burst_size;
	a_uint64_t temp_bucket_size;

	if(hppe_policer_burst_size[meter_unit][token_unit].burst_size_1bit > 0)
	{
		temp_bucket_size = ((a_uint64_t)bucket_size) * hppe_policer_burst_size[meter_unit][token_unit].burst_size_1bit;
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
#endif

static sw_error_t
__adpt_hppe_policer_max_rate(a_uint32_t time_slot)
{
	a_uint32_t i = 0, j = 0;
	a_uint32_t time_cycle = 0;
	a_uint64_t temp = 0, temp1 = 0, temp2 = 0;


	/* time_cycle is ns*/
	time_cycle = ( time_slot * 8)/ ADPT_HPPE_FREQUENCY;

	for (j = 0; j < 8; j++)
	{
		/*max rate is bps*/
		temp1 = (a_uint64_t)(ADPT_HPPE_POLICER_REFRESH_MAX  * 1000 * 8) * 1000;
		temp2 =  hppe_policer_token_unit[i][j] * time_cycle;

		do_div(temp1, temp2);
		hppe_policer_rate[i][j].rate_max= temp1;

		temp = temp1;
		do_div(temp, ADPT_HPPE_POLICER_REFRESH_MAX);
		hppe_policer_rate[i][j].rate_1bit = temp;

	//	printk("hppe policer byte max_rate generating =%llu\n", hppe_policer_rate[i][j].rate_max);
	//	printk("hppe policer byte based step =%llu\n", hppe_policer_rate[i][j].rate_1bit);
	}

	i = i + 1;
	for (j = 0; j < 8; j++)
	{
		/* max rate unit  is 1/1000 pps*/
		temp1 = (a_uint64_t)(ADPT_HPPE_POLICER_REFRESH_MAX * 1000) * 1000 * 1000;
		temp2 = (a_uint64_t)hppe_policer_token_unit[i][j] * time_cycle;

		do_div(temp1, temp2);
		hppe_policer_rate[i][j].rate_max = temp1;

		temp = temp1;
		do_div(temp, ADPT_HPPE_POLICER_REFRESH_MAX);
		hppe_policer_rate[i][j].rate_1bit = temp;

	//	printk("policer frame hppe_max_rate generating =%llu\n", hppe_policer_rate[i][j].rate_max);
	//	printk("policer frame step rate =%llu\n", hppe_policer_rate[i][j].rate_1bit);
	}

	return SW_OK;
}

static sw_error_t
__adpt_hppe_policer_max_burst_size(void)
{
	a_uint32_t i = 0, j = 0;
	a_uint64_t temp = 0, temp1 = 0;


	for (j = 0; j < 8; j++)
	{
		/*max size unit is 1/1000 byte based*/
		temp = ((a_uint64_t)(ADPT_HPPE_POLICER_BURST_SIZE_UNIT)) *
			ADPT_HPPE_POLICER_BUCKET_SIZE_MAX;
		do_div(temp, hppe_policer_token_unit[i][j]);
		hppe_policer_burst_size[i][j].burst_size_max = (a_uint64_t)(temp * 1000);

		temp1 = hppe_policer_burst_size[i][j].burst_size_max;
		do_div(temp1, ADPT_HPPE_POLICER_BUCKET_SIZE_MAX);
		hppe_policer_burst_size[i][j].burst_size_1bit = temp1;

		/*
		printk("policer byte hppe_max_burst_size generating =%llu\n",
				hppe_policer_burst_size[i][j].burst_size_max);
		printk("policer byte hppe_max_burst_size step  =%llu\n",
				hppe_policer_burst_size[i][j].burst_size_1bit);
		*/

	}

	i = i + 1;
	for (j = 0; j < 8; j++)
	{
		/* max size unit is 1/1000 frame based */
		temp = ((a_uint64_t)(ADPT_HPPE_POLICER_BURST_SIZE_UNIT)) *
			ADPT_HPPE_POLICER_BUCKET_SIZE_MAX;
		do_div(temp, hppe_policer_token_unit[i][j]);
		hppe_policer_burst_size[i][j].burst_size_max = (a_uint64_t)(temp * 1000);

		temp1 = hppe_policer_burst_size[i][j].burst_size_max;
		do_div(temp1, ADPT_HPPE_POLICER_BUCKET_SIZE_MAX);
		hppe_policer_burst_size[i][j].burst_size_1bit = temp1;

		/*
		printk("policer frame hppe_max_burst_size generating =%llu\n",
				hppe_policer_burst_size[i][j].burst_size_max);
		printk("policer frame hppe_max_burst_size step  =%llu\n",
				hppe_policer_burst_size[i][j].burst_size_1bit);
		*/

	}

	return SW_OK;
}

#ifndef IN_POLICER_MINI
static sw_error_t
__adpt_hppe_policer_two_bucket_parameter_select(a_uint64_t c_rate,
						a_uint64_t c_burst_size,
						a_uint64_t e_rate,
						a_uint64_t e_burst_size,
						a_uint32_t meter_unit,
						a_uint32_t *token_unit)
{
	a_uint32_t  temp_token_unit;
	a_uint32_t match = A_FALSE;
	a_uint64_t temp_rate = 0, temp_burst_size = 0;

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
		 if (temp_rate > hppe_policer_rate[meter_unit][temp_token_unit].rate_max)
		 {
		 	continue;
		 }
		 else if(temp_burst_size <= hppe_policer_burst_size[meter_unit][temp_token_unit].burst_size_max)
		{
			*token_unit = temp_token_unit;
			match = A_TRUE;
			break;
		}
	}

	if (match == A_FALSE)
	{
		printk("Not match C and E token bucket parameter rate configuration \n");
		return SW_BAD_PARAM;
	}

	return SW_OK;
}

#if 0
static sw_error_t
__adpt_hppe_policer_one_bucket_parameter_select(a_uint64_t c_rate,
						a_uint64_t c_burst_size,
						a_uint32_t meter_unit,
						a_uint32_t *token_unit)
{
	a_uint32_t  temp_token_unit;
	a_uint32_t match = A_FALSE;

	for (temp_token_unit = 0; temp_token_unit < 8; temp_token_unit++)
	{
		if (c_rate > hppe_policer_rate[meter_unit][temp_token_unit].rate_max)
		{
			continue;
		}
		else if (c_burst_size <= hppe_policer_burst_size[meter_unit][temp_token_unit].burst_size_max)
		{
			*token_unit = temp_token_unit;
			match = A_TRUE;
			break;
		}
	}

	if (match == A_FALSE)
	{
		printk("Not match policer C token bucket parameter rate configuration \n");
		return SW_BAD_PARAM;
	}

	return SW_OK;
}
#endif

sw_error_t
adpt_hppe_port_policer_entry_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
	union in_port_meter_cfg_tbl_u in_port_meter_cfg_tbl;
	a_uint32_t hppe_cir =0, hppe_cbs = 0, hppe_eir = 0,hppe_ebs = 0;

	memset(&in_port_meter_cfg_tbl, 0, sizeof(in_port_meter_cfg_tbl));

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(policer);
	ADPT_NULL_POINT_CHECK(action);

	if (port_id < 0 || port_id > 7)
		return SW_BAD_PARAM;

	hppe_in_port_meter_cfg_tbl_get(dev_id, port_id, &in_port_meter_cfg_tbl);

	hppe_cir = (in_port_meter_cfg_tbl.bf.cir_1 << 3) | in_port_meter_cfg_tbl.bf.cir_0;
	hppe_cbs = in_port_meter_cfg_tbl.bf.cbs;
	hppe_eir = (in_port_meter_cfg_tbl.bf.eir_1 << 1) | in_port_meter_cfg_tbl.bf.eir_0;
	hppe_ebs = in_port_meter_cfg_tbl.bf.ebs;


	__adpt_hppe_policer_refresh_to_rate(hppe_cir,
								&policer->cir,
								in_port_meter_cfg_tbl.bf.meter_unit,
								in_port_meter_cfg_tbl.bf.token_unit);

	__adpt_hppe_policer_refresh_to_rate(hppe_eir,
								&policer->eir,
								in_port_meter_cfg_tbl.bf.meter_unit,
								in_port_meter_cfg_tbl.bf.token_unit);

	__adpt_hppe_policer_bucket_size_to_burst_size(hppe_cbs,
								&policer->cbs,
								in_port_meter_cfg_tbl.bf.meter_unit,
								in_port_meter_cfg_tbl.bf.token_unit);

	__adpt_hppe_policer_bucket_size_to_burst_size(hppe_ebs,
								&policer->ebs,
								in_port_meter_cfg_tbl.bf.meter_unit,
								in_port_meter_cfg_tbl.bf.token_unit);

	policer->meter_en = in_port_meter_cfg_tbl.bf.meter_en;
	policer->color_mode = in_port_meter_cfg_tbl.bf.color_mode;
	policer->frame_type = in_port_meter_cfg_tbl.bf.meter_flag;
	policer->couple_en = in_port_meter_cfg_tbl.bf.coupling_flag;
	policer->meter_mode = in_port_meter_cfg_tbl.bf.meter_mode;
	policer->meter_unit = in_port_meter_cfg_tbl.bf.meter_unit;
	action->yellow_priority_en = in_port_meter_cfg_tbl.bf.exceed_chg_pri_cmd;
	action->yellow_drop_priority_en = in_port_meter_cfg_tbl.bf.exceed_chg_dp_cmd;
	action->yellow_pcp_en = in_port_meter_cfg_tbl.bf.exceed_chg_pcp_cmd;
	action->yellow_dei_en = in_port_meter_cfg_tbl.bf.exceed_chg_dei_cmd;
	action->yellow_priority = in_port_meter_cfg_tbl.bf.exceed_pri;
	action->yellow_drop_priority = in_port_meter_cfg_tbl.bf.exceed_dp;
	action->yellow_pcp = in_port_meter_cfg_tbl.bf.exceed_pcp;
	action->yellow_dei = in_port_meter_cfg_tbl.bf.exceed_dei;
	if (in_port_meter_cfg_tbl.bf.violate_cmd == 0)
		action->red_action = FAL_MAC_DROP;
	else
		action->red_action = FAL_MAC_FRWRD;
	action->red_priority_en = in_port_meter_cfg_tbl.bf.violate_chg_pri_cmd;
	action->red_drop_priority_en = in_port_meter_cfg_tbl.bf.violate_chg_dp_cmd;
	action->red_pcp_en = in_port_meter_cfg_tbl.bf.violate_chg_pcp_cmd;
	action->red_dei_en = in_port_meter_cfg_tbl.bf.violate_chg_dei_cmd;
	action->red_priority = in_port_meter_cfg_tbl.bf.violate_pri;
	action->red_drop_priority = in_port_meter_cfg_tbl.bf.violate_dp;
	action->red_pcp = in_port_meter_cfg_tbl.bf.violate_pcp;
	action->red_dei = in_port_meter_cfg_tbl.bf.violate_dei;

	return SW_OK;
}

sw_error_t
adpt_hppe_port_policer_entry_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
	union in_port_meter_cfg_tbl_u in_port_meter_cfg_tbl;
	a_uint32_t hppe_cir = 0, hppe_cbs = 0, hppe_eir = 0,hppe_ebs = 0;
	a_uint64_t temp_cir = 0, temp_eir =0, temp_cbs =0, temp_ebs = 0;
	a_uint32_t token_unit = 0;
	sw_error_t rv = SW_OK;

	memset(&in_port_meter_cfg_tbl, 0, sizeof(in_port_meter_cfg_tbl));

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(policer);
	ADPT_NULL_POINT_CHECK(action);

	if (port_id < 0 || port_id > 7)
		return SW_BAD_PARAM;

	if(ADPT_HPPE_POLICER_METER_UNIT_BYTE == policer->meter_unit)
	{
		if ((policer->cir > BYTE_POLICER_MAX_RATE) || (policer->eir > BYTE_POLICER_MAX_RATE))
			return SW_BAD_PARAM;
		if ((policer->cir < BYTE_POLICER_MIN_RATE) && (policer->cir != 0))
			return SW_BAD_PARAM;
		if ((policer->eir < BYTE_POLICER_MIN_RATE) && (policer->eir != 0))
			return SW_BAD_PARAM;
	}

	if(ADPT_HPPE_POLICER_METER_UNIT_FRAME == policer->meter_unit)
	{
		if ((policer->cir > FRAME_POLICER_MAX_RATE) || (policer->eir > FRAME_POLICER_MAX_RATE))
			return SW_BAD_PARAM;
		if ((policer->cir < FRAME_POLICER_MIN_RATE) && (policer->cir != 0))
			return SW_BAD_PARAM;
		if ((policer->eir < FRAME_POLICER_MIN_RATE) && (policer->eir != 0))
			return SW_BAD_PARAM;
	}

	if((0 == policer->meter_mode) && (policer->cir > policer->eir))
		return SW_BAD_PARAM;

	temp_cir = ((a_uint64_t)policer->cir) * 1000;
	temp_cbs = ((a_uint64_t)policer->cbs) * 1000;
	temp_eir = ((a_uint64_t)policer->eir) * 1000;
	temp_ebs = ((a_uint64_t)policer->ebs) * 1000;

	rv = __adpt_hppe_policer_two_bucket_parameter_select(temp_cir,
								temp_cbs,
								temp_eir,
								temp_ebs,
								policer->meter_unit,
								&token_unit);
	if( rv != SW_OK )
		return rv;

//	printk("current meter unit is = %d\n", policer->meter_unit);
//	printk("current token unit is = %d\n", token_unit);

	__adpt_hppe_policer_rate_to_refresh(policer->cir,
								&hppe_cir,
								policer->meter_unit,
								token_unit);

	__adpt_hppe_policer_rate_to_refresh(policer->eir,
								&hppe_eir,
								policer->meter_unit,
								token_unit);

	__adpt_hppe_policer_burst_size_to_bucket_size(policer->cbs,
								&hppe_cbs,
								policer->meter_unit,
								token_unit);

	__adpt_hppe_policer_burst_size_to_bucket_size(policer->ebs,
								&hppe_ebs,
								policer->meter_unit,
								token_unit);

	in_port_meter_cfg_tbl.bf.meter_en = policer->meter_en;
	in_port_meter_cfg_tbl.bf.color_mode = policer->color_mode;
	in_port_meter_cfg_tbl.bf.meter_flag = policer->frame_type;
	in_port_meter_cfg_tbl.bf.coupling_flag = policer->couple_en;
	in_port_meter_cfg_tbl.bf.meter_mode = policer->meter_mode;
	in_port_meter_cfg_tbl.bf.token_unit = token_unit;
	in_port_meter_cfg_tbl.bf.meter_unit = (a_uint32_t)policer->meter_unit;
	in_port_meter_cfg_tbl.bf.cbs = hppe_cbs;
	in_port_meter_cfg_tbl.bf.cir_0 = hppe_cir & 0x7;
	in_port_meter_cfg_tbl.bf.cir_1 = hppe_cir >> 3;
	in_port_meter_cfg_tbl.bf.ebs = hppe_ebs;
	in_port_meter_cfg_tbl.bf.eir_0 = hppe_eir & 0x1;
	in_port_meter_cfg_tbl.bf.eir_1 = hppe_eir >> 1;
	in_port_meter_cfg_tbl.bf.exceed_chg_pri_cmd = action->yellow_priority_en;
	in_port_meter_cfg_tbl.bf.exceed_chg_dp_cmd = action->yellow_drop_priority_en;
	in_port_meter_cfg_tbl.bf.exceed_chg_pcp_cmd = action->yellow_pcp_en;
	in_port_meter_cfg_tbl.bf.exceed_chg_dei_cmd = action->yellow_dei_en;
	in_port_meter_cfg_tbl.bf.exceed_pri = action->yellow_priority;
	in_port_meter_cfg_tbl.bf.exceed_dp = action->yellow_drop_priority;
	in_port_meter_cfg_tbl.bf.exceed_pcp = action->yellow_pcp;
	in_port_meter_cfg_tbl.bf.exceed_dei = action->yellow_dei;
	if (action->red_action == FAL_MAC_DROP)
		in_port_meter_cfg_tbl.bf.violate_cmd = 0;
	else
		in_port_meter_cfg_tbl.bf.violate_cmd = 1;
	in_port_meter_cfg_tbl.bf.violate_chg_pri_cmd = action->red_priority_en;
	in_port_meter_cfg_tbl.bf.violate_chg_dp_cmd = action->red_drop_priority_en;
	in_port_meter_cfg_tbl.bf.violate_chg_pcp_cmd = action->red_pcp_en;
	in_port_meter_cfg_tbl.bf.violate_chg_dei_cmd = action->red_dei_en;
	in_port_meter_cfg_tbl.bf.violate_pri = action->red_priority;
	in_port_meter_cfg_tbl.bf.violate_dp = action->red_drop_priority;
	in_port_meter_cfg_tbl.bf.violate_pcp = action->red_pcp;
	in_port_meter_cfg_tbl.bf.violate_dei = action->red_dei;

	hppe_in_port_meter_cfg_tbl_set(dev_id, port_id, &in_port_meter_cfg_tbl);

	return SW_OK;

}
sw_error_t
adpt_hppe_acl_policer_entry_get(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
	union in_acl_meter_cfg_tbl_u in_acl_meter_cfg_tbl;
	a_uint32_t hppe_cir =0, hppe_cbs = 0, hppe_eir = 0,hppe_ebs = 0;

	memset(&in_acl_meter_cfg_tbl, 0, sizeof(in_acl_meter_cfg_tbl));

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(policer);
	ADPT_NULL_POINT_CHECK(action);

	if (index < 0 || index > 511)
		return SW_BAD_PARAM;

	hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &in_acl_meter_cfg_tbl);

	hppe_cir = (in_acl_meter_cfg_tbl.bf.cir_1 << 8) | in_acl_meter_cfg_tbl.bf.cir_0;
	hppe_cbs = in_acl_meter_cfg_tbl.bf.cbs;
	hppe_eir = (in_acl_meter_cfg_tbl.bf.eir_1 << 6) | in_acl_meter_cfg_tbl.bf.eir_0;
	hppe_ebs = in_acl_meter_cfg_tbl.bf.ebs;

	__adpt_hppe_policer_refresh_to_rate(hppe_cir,
								&policer->cir,
								in_acl_meter_cfg_tbl.bf.meter_unit,
								in_acl_meter_cfg_tbl.bf.token_unit);

	__adpt_hppe_policer_refresh_to_rate(hppe_eir,
								&policer->eir,
								in_acl_meter_cfg_tbl.bf.meter_unit,
								in_acl_meter_cfg_tbl.bf.token_unit);

	__adpt_hppe_policer_bucket_size_to_burst_size(hppe_cbs,
								&policer->cbs,
								in_acl_meter_cfg_tbl.bf.meter_unit,
								in_acl_meter_cfg_tbl.bf.token_unit);

	__adpt_hppe_policer_bucket_size_to_burst_size(hppe_ebs,
								&policer->ebs,
								in_acl_meter_cfg_tbl.bf.meter_unit,
								in_acl_meter_cfg_tbl.bf.token_unit);

	policer->meter_en = in_acl_meter_cfg_tbl.bf.meter_en;
	policer->color_mode = in_acl_meter_cfg_tbl.bf.color_mode;
	policer->couple_en= in_acl_meter_cfg_tbl.bf.coupling_flag;
	policer->meter_mode = in_acl_meter_cfg_tbl.bf.meter_mode;
	policer->meter_unit = in_acl_meter_cfg_tbl.bf.meter_unit;
	action->yellow_priority_en = in_acl_meter_cfg_tbl.bf.exceed_chg_pri_cmd;
	action->yellow_drop_priority_en = in_acl_meter_cfg_tbl.bf.exceed_chg_dp_cmd;
	action->yellow_pcp_en = in_acl_meter_cfg_tbl.bf.exceed_chg_pcp_cmd;
	action->yellow_dei_en = in_acl_meter_cfg_tbl.bf.exceed_chg_dei_cmd;
	action->yellow_priority = in_acl_meter_cfg_tbl.bf.exceed_pri;
	action->yellow_drop_priority = in_acl_meter_cfg_tbl.bf.exceed_dp;
	action->yellow_pcp = in_acl_meter_cfg_tbl.bf.exceed_pcp;
	action->yellow_dei = in_acl_meter_cfg_tbl.bf.exceed_dei;
	if (in_acl_meter_cfg_tbl.bf.violate_cmd == 0)
		action->red_action = FAL_MAC_DROP;
	else
		action->red_action = FAL_MAC_FRWRD;
	action->red_priority_en = in_acl_meter_cfg_tbl.bf.violate_chg_pri_cmd;
	action->red_drop_priority_en = in_acl_meter_cfg_tbl.bf.violate_chg_dp_cmd;
	action->red_pcp_en = in_acl_meter_cfg_tbl.bf.violate_chg_pcp_cmd;
	action->red_dei_en = in_acl_meter_cfg_tbl.bf.violate_chg_dei_cmd;
	action->red_priority = (in_acl_meter_cfg_tbl.bf.violate_pri_1 << 1) |in_acl_meter_cfg_tbl.bf.violate_pri_0;
	action->red_drop_priority = in_acl_meter_cfg_tbl.bf.violate_dp;
	action->red_pcp = in_acl_meter_cfg_tbl.bf.violate_pcp;
	action->red_dei = in_acl_meter_cfg_tbl.bf.violate_dei;

	return SW_OK;
}

sw_error_t
adpt_hppe_acl_policer_entry_set(a_uint32_t dev_id, a_uint32_t index,
		fal_policer_config_t *policer, fal_policer_action_t *action)
{
	union in_acl_meter_cfg_tbl_u in_acl_meter_cfg_tbl;
	a_uint32_t hppe_cir =0, hppe_cbs = 0, hppe_eir = 0,hppe_ebs = 0;
	a_uint64_t temp_cir = 0, temp_eir =0, temp_cbs =0, temp_ebs = 0;
	a_uint32_t token_unit = 0;
	sw_error_t rv = SW_OK;

	memset(&in_acl_meter_cfg_tbl, 0, sizeof(in_acl_meter_cfg_tbl));

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(policer);
	ADPT_NULL_POINT_CHECK(action);

	if (index < 0 || index > 511)
		return SW_BAD_PARAM;

	if(ADPT_HPPE_POLICER_METER_UNIT_BYTE == policer->meter_unit)
	{
		if ((policer->cir > BYTE_POLICER_MAX_RATE) || (policer->eir > BYTE_POLICER_MAX_RATE))
			return SW_BAD_PARAM;
		if ((policer->cir < BYTE_POLICER_MIN_RATE) && (policer->cir != 0))
			return SW_BAD_PARAM;
		if ((policer->eir < BYTE_POLICER_MIN_RATE) && (policer->eir != 0))
			return SW_BAD_PARAM;
	}

	if(ADPT_HPPE_POLICER_METER_UNIT_FRAME == policer->meter_unit)
	{
		if ((policer->cir > FRAME_POLICER_MAX_RATE) || (policer->eir > FRAME_POLICER_MAX_RATE))
			return SW_BAD_PARAM;
		if ((policer->cir < FRAME_POLICER_MIN_RATE) && (policer->cir != 0))
			return SW_BAD_PARAM;
		if ((policer->eir < FRAME_POLICER_MIN_RATE) && (policer->eir != 0))
			return SW_BAD_PARAM;
	}

	if((0 == policer->meter_mode) && (policer->cir > policer->eir))
		return SW_BAD_PARAM;

	temp_cir = ((a_uint64_t)policer->cir) * 1000;
	temp_cbs = ((a_uint64_t)policer->cbs) * 1000;
	temp_eir = ((a_uint64_t)policer->eir) * 1000;
	temp_ebs = ((a_uint64_t)policer->ebs) * 1000;

	rv = __adpt_hppe_policer_two_bucket_parameter_select(temp_cir,
								temp_cbs,
								temp_eir,
								temp_ebs,
								policer->meter_unit,
								&token_unit);
	if( rv != SW_OK )
		return rv;

//	printk("current meter unit is = %d\n", policer->meter_unit);
//	printk("current token unit is = %d\n", token_unit);

	__adpt_hppe_policer_rate_to_refresh(policer->cir,
								&hppe_cir,
								policer->meter_unit,
								token_unit);

	__adpt_hppe_policer_rate_to_refresh(policer->eir,
								&hppe_eir,
								policer->meter_unit,
								token_unit);

	__adpt_hppe_policer_burst_size_to_bucket_size(policer->cbs,
								&hppe_cbs,
								policer->meter_unit,
								token_unit);

	__adpt_hppe_policer_burst_size_to_bucket_size(policer->ebs,
								&hppe_ebs,
								policer->meter_unit,
								token_unit);

	in_acl_meter_cfg_tbl.bf.meter_en = policer->meter_en;
	in_acl_meter_cfg_tbl.bf.color_mode = policer->color_mode;
	in_acl_meter_cfg_tbl.bf.coupling_flag = policer->couple_en;
	in_acl_meter_cfg_tbl.bf.meter_mode = policer->meter_mode;
	in_acl_meter_cfg_tbl.bf.token_unit = token_unit;
	in_acl_meter_cfg_tbl.bf.meter_unit = policer->meter_unit;
	in_acl_meter_cfg_tbl.bf.cbs = hppe_cbs;
	in_acl_meter_cfg_tbl.bf.cir_0 = hppe_cir & 0xff;
	in_acl_meter_cfg_tbl.bf.cir_1 = hppe_cir >> 8;
	in_acl_meter_cfg_tbl.bf.ebs = hppe_ebs;
	in_acl_meter_cfg_tbl.bf.eir_0 = hppe_eir & 0x3f;
	in_acl_meter_cfg_tbl.bf.eir_1 = hppe_eir >> 6;
	in_acl_meter_cfg_tbl.bf.exceed_chg_pri_cmd = action->yellow_priority_en;
	in_acl_meter_cfg_tbl.bf.exceed_chg_dp_cmd = action->yellow_drop_priority_en;
	in_acl_meter_cfg_tbl.bf.exceed_chg_pcp_cmd = action->yellow_pcp_en;
	in_acl_meter_cfg_tbl.bf.exceed_chg_dei_cmd = action->yellow_dei_en;
	in_acl_meter_cfg_tbl.bf.exceed_pri = action->yellow_priority;
	in_acl_meter_cfg_tbl.bf.exceed_dp = action->yellow_drop_priority;
	in_acl_meter_cfg_tbl.bf.exceed_pcp = action->yellow_pcp;
	in_acl_meter_cfg_tbl.bf.exceed_dei = action->yellow_dei;
	if (action->red_action == FAL_MAC_DROP)
		in_acl_meter_cfg_tbl.bf.violate_cmd = 0;
	else
		in_acl_meter_cfg_tbl.bf.violate_cmd = 1;
	in_acl_meter_cfg_tbl.bf.violate_chg_pri_cmd = action->red_priority_en;
	in_acl_meter_cfg_tbl.bf.violate_chg_dp_cmd = action->red_drop_priority_en;
	in_acl_meter_cfg_tbl.bf.violate_chg_pcp_cmd = action->red_pcp_en;
	in_acl_meter_cfg_tbl.bf.violate_chg_dei_cmd = action->red_dei_en;
	in_acl_meter_cfg_tbl.bf.violate_pri_0 = action->red_priority & 0x1;
	in_acl_meter_cfg_tbl.bf.violate_pri_1 = action->red_priority >>1;
	in_acl_meter_cfg_tbl.bf.violate_dp = action->red_drop_priority;
	in_acl_meter_cfg_tbl.bf.violate_pcp = action->red_pcp;
	in_acl_meter_cfg_tbl.bf.violate_dei = action->red_dei;

	hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &in_acl_meter_cfg_tbl);

	return SW_OK;
}

sw_error_t
adpt_hppe_policer_time_slot_get(a_uint32_t dev_id, a_uint32_t *time_slot)
{
	sw_error_t rv = SW_OK;
	union time_slot_reg_u time_slot_reg;

	memset(&time_slot_reg, 0, sizeof(time_slot_reg));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time_slot);


	rv = hppe_time_slot_reg_get(dev_id, &time_slot_reg);

	if( rv != SW_OK )
		return rv;

	*time_slot = time_slot_reg.bf.time_slot;

	return SW_OK;
}
#endif

sw_error_t
adpt_hppe_port_compensation_byte_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t  length)
{
	union meter_cmpst_length_reg_u meter_cmpst_length_reg;

	memset(&meter_cmpst_length_reg, 0, sizeof(meter_cmpst_length_reg));
	ADPT_DEV_ID_CHECK(dev_id);

	if (port_id < 0 || port_id > 7)
		return SW_BAD_PARAM;

	if (length > 0x1f)
		return SW_BAD_PARAM;

	hppe_meter_cmpst_length_reg_get(dev_id, port_id, &meter_cmpst_length_reg);
	meter_cmpst_length_reg.bf.cmpst_length = length;

	hppe_meter_cmpst_length_reg_set(dev_id, port_id, &meter_cmpst_length_reg);

	return SW_OK;
}
sw_error_t
adpt_hppe_policer_time_slot_set(a_uint32_t dev_id, a_uint32_t time_slot)
{
	union time_slot_reg_u time_slot_reg;

	memset(&time_slot_reg, 0, sizeof(time_slot_reg));
	ADPT_DEV_ID_CHECK(dev_id);

	if ((time_slot > 1024) || (time_slot < 512))
		return SW_BAD_PARAM;

	time_slot_reg.bf.time_slot = time_slot;
	hppe_time_slot_reg_set(dev_id, &time_slot_reg);

	__adpt_hppe_policer_max_rate(time_slot);

	__adpt_hppe_policer_max_burst_size();

	return SW_OK;
}
#ifndef IN_POLICER_MINI
sw_error_t
adpt_hppe_policer_global_counter_get(a_uint32_t dev_id,
		fal_policer_global_counter_t *counter)
{
	union pc_global_cnt_tbl_u pc_global_cnt_tbl;
	a_uint32_t index = 0;

	memset(&pc_global_cnt_tbl, 0, sizeof(pc_global_cnt_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(counter);

	hppe_pc_global_cnt_tbl_get(dev_id, index, &pc_global_cnt_tbl);
	counter->policer_drop_packet_counter = pc_global_cnt_tbl.bf.pkt_cnt;
	counter->policer_drop_byte_counter = pc_global_cnt_tbl.bf.byte_cnt_1;
	counter->policer_drop_byte_counter = (counter->policer_drop_byte_counter << 32) |
		pc_global_cnt_tbl.bf.byte_cnt_0;

	hppe_pc_global_cnt_tbl_get(dev_id, index + 1, &pc_global_cnt_tbl);
	counter->policer_forward_packet_counter = pc_global_cnt_tbl.bf.pkt_cnt;
	counter->policer_forward_byte_counter = pc_global_cnt_tbl.bf.byte_cnt_1;
	counter->policer_forward_byte_counter = (counter->policer_forward_byte_counter << 32) |
		pc_global_cnt_tbl.bf.byte_cnt_0;

	hppe_pc_global_cnt_tbl_get(dev_id, index + 2, &pc_global_cnt_tbl);
	counter->policer_bypass_packet_counter = pc_global_cnt_tbl.bf.pkt_cnt;
	counter->policer_bypass_byte_counter = pc_global_cnt_tbl.bf.byte_cnt_1;
	counter->policer_bypass_byte_counter = (counter->policer_bypass_byte_counter << 32) |
		pc_global_cnt_tbl.bf.byte_cnt_0;

	return SW_OK;
}

sw_error_t
adpt_hppe_policer_bypass_en_get(a_uint32_t dev_id, fal_policer_frame_type_t frame_type,
	a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	union pc_drop_bypass_reg_u drop_bypass_reg;

	memset(&drop_bypass_reg, 0, sizeof(drop_bypass_reg));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	rv = hppe_pc_drop_bypass_reg_get(dev_id, &drop_bypass_reg);
	SW_RTN_ON_ERROR (rv);

	if (frame_type == FAL_FRAME_DROPPED) {
		*enable = drop_bypass_reg.bf.drop_bypass_en;
	} else {
		return SW_BAD_PARAM;
	}

	return SW_OK;
}

#endif

sw_error_t
adpt_hppe_policer_bypass_en_set(a_uint32_t dev_id, fal_policer_frame_type_t frame_type,
	a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union pc_drop_bypass_reg_u drop_bypass_reg;

	memset(&drop_bypass_reg, 0, sizeof(drop_bypass_reg));
	ADPT_DEV_ID_CHECK(dev_id);

	if (frame_type == FAL_FRAME_DROPPED) {
		drop_bypass_reg.bf.drop_bypass_en = enable;
	} else {
		return SW_BAD_PARAM;
	}

	rv = hppe_pc_drop_bypass_reg_set(dev_id, &drop_bypass_reg);
	SW_RTN_ON_ERROR (rv);

	return SW_OK;
}

void adpt_hppe_policer_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_policer_func_bitmap = ((1 << FUNC_ADPT_ACL_POLICER_COUNTER_GET)|
						(1 << FUNC_ADPT_PORT_POLICER_COUNTER_GET)|
						(1 << FUNC_ADPT_PORT_COMPENSATION_BYTE_GET)|
						(1 << FUNC_ADPT_PORT_POLICER_ENTRY_GET)|
						(1 << FUNC_ADPT_PORT_POLICER_ENTRY_SET)|
						(1 << FUNC_ADPT_ACL_POLICER_ENTRY_GET)|
						(1 << FUNC_ADPT_ACL_POLICER_ENTRY_SET)|
						(1 << FUNC_ADPT_POLICER_TIME_SLOT_GET)|
						(1 << FUNC_ADPT_PORT_COMPENSATION_BYTE_SET)|
						(1 << FUNC_ADPT_POLICER_TIME_SLOT_SET) |
						(1 << FUNC_ADPT_POLICER_GLOBAL_COUNTER_GET)|
						(1 << FUNC_ADPT_POLICER_BYPASS_EN_SET)|
						(1 << FUNC_ADPT_POLICER_BYPASS_EN_GET));

	return;

}

static void adpt_hppe_policer_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_acl_policer_counter_get = NULL;
	p_adpt_api->adpt_port_policer_counter_get = NULL;
	p_adpt_api->adpt_port_compensation_byte_get = NULL;
	p_adpt_api->adpt_port_policer_entry_get = NULL;
	p_adpt_api->adpt_port_policer_entry_set = NULL;
	p_adpt_api->adpt_acl_policer_entry_get = NULL;
	p_adpt_api->adpt_acl_policer_entry_set = NULL;
	p_adpt_api->adpt_policer_time_slot_get = NULL;
	p_adpt_api->adpt_port_compensation_byte_set = NULL;
	p_adpt_api->adpt_policer_time_slot_set = NULL;
	p_adpt_api->adpt_policer_global_counter_get = NULL;
	p_adpt_api->adpt_policer_bypass_en_set = NULL;
	p_adpt_api->adpt_policer_bypass_en_get = NULL;

	return;

}


sw_error_t adpt_hppe_policer_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_policer_func_unregister(dev_id, p_adpt_api);

#ifndef IN_POLICER_MINI
	if(p_adpt_api->adpt_policer_func_bitmap & (1 << FUNC_ADPT_ACL_POLICER_COUNTER_GET))
	{
		p_adpt_api->adpt_acl_policer_counter_get = adpt_hppe_acl_policer_counter_get;
	}
	if(p_adpt_api->adpt_policer_func_bitmap & (1 << FUNC_ADPT_PORT_POLICER_COUNTER_GET))
	{
		p_adpt_api->adpt_port_policer_counter_get = adpt_hppe_port_policer_counter_get;
	}
	if(p_adpt_api->adpt_policer_func_bitmap & (1 << FUNC_ADPT_PORT_COMPENSATION_BYTE_GET))
	{
		p_adpt_api->adpt_port_compensation_byte_get = adpt_hppe_port_compensation_byte_get;
	}
	if(p_adpt_api->adpt_policer_func_bitmap & (1 << FUNC_ADPT_PORT_POLICER_ENTRY_GET))
	{
		p_adpt_api->adpt_port_policer_entry_get = adpt_hppe_port_policer_entry_get;
	}
	if(p_adpt_api->adpt_policer_func_bitmap & (1 << FUNC_ADPT_PORT_POLICER_ENTRY_SET))
	{
		p_adpt_api->adpt_port_policer_entry_set = adpt_hppe_port_policer_entry_set;
	}
	if(p_adpt_api->adpt_policer_func_bitmap & (1 << FUNC_ADPT_ACL_POLICER_ENTRY_GET))
	{
		p_adpt_api->adpt_acl_policer_entry_get = adpt_hppe_acl_policer_entry_get;
	}
	if(p_adpt_api->adpt_policer_func_bitmap & (1 << FUNC_ADPT_ACL_POLICER_ENTRY_SET))
	{
		p_adpt_api->adpt_acl_policer_entry_set = adpt_hppe_acl_policer_entry_set;
	}
	if(p_adpt_api->adpt_policer_func_bitmap & (1 << FUNC_ADPT_POLICER_TIME_SLOT_GET))
	{
		p_adpt_api->adpt_policer_time_slot_get = adpt_hppe_policer_time_slot_get;
	}
	if(p_adpt_api->adpt_policer_func_bitmap & (1 << FUNC_ADPT_POLICER_GLOBAL_COUNTER_GET))
	{
		p_adpt_api->adpt_policer_global_counter_get = adpt_hppe_policer_global_counter_get;
	}
	if(p_adpt_api->adpt_policer_func_bitmap & (1 << FUNC_ADPT_POLICER_BYPASS_EN_GET))
	{
		p_adpt_api->adpt_policer_bypass_en_get = adpt_hppe_policer_bypass_en_get;
	}
#endif
	if(p_adpt_api->adpt_policer_func_bitmap & (1 << FUNC_ADPT_PORT_COMPENSATION_BYTE_SET))
	{
		p_adpt_api->adpt_port_compensation_byte_set = adpt_hppe_port_compensation_byte_set;
	}
	if(p_adpt_api->adpt_policer_func_bitmap & (1 << FUNC_ADPT_POLICER_TIME_SLOT_SET))
	{
		p_adpt_api->adpt_policer_time_slot_set = adpt_hppe_policer_time_slot_set;
	}
	if(p_adpt_api->adpt_policer_func_bitmap & (1 << FUNC_ADPT_POLICER_BYPASS_EN_SET))
	{
		p_adpt_api->adpt_policer_bypass_en_set = adpt_hppe_policer_bypass_en_set;
	}

	return SW_OK;
}

/**
 * @}
 */

