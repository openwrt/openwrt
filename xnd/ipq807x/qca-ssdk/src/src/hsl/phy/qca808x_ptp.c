/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
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
#include "sw.h"
#include "hsl_api.h"
#include "hsl.h"
#include "hsl_phy.h"
#include "ssdk_plat.h"
#include "qca808x_phy.h"
#include "qca808x_ptp_reg.h"
#include "qca808x_ptp_api.h"
#include "qca808x_ptp.h"

sw_error_t
qca808x_phy_ptp_config_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_config_t *config)
{
	union ptp_main_conf_reg_u ptp_main_conf_reg = {0};
	union ptp_misc_config_reg_u ptp_misc_config_reg = {0};
	union ptp_backup_reg_u ptp_backup_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &ptp_main_conf_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &ptp_misc_config_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_backup_reg_get(dev_id, phy_id, &ptp_backup_reg));
	if (config->ptp_en == A_TRUE)
	{
		ptp_main_conf_reg.bf.ptp_bypass = PTP_REG_BIT_FALSE;
		ptp_main_conf_reg.bf.disable_1588_phy = PTP_REG_BIT_FALSE;
	}
	else
	{
		ptp_main_conf_reg.bf.ptp_bypass = PTP_REG_BIT_TRUE;
		ptp_main_conf_reg.bf.disable_1588_phy = PTP_REG_BIT_TRUE;
	}
	if (config->clock_mode == FAL_OC_CLOCK_MODE || config->clock_mode == FAL_BC_CLOCK_MODE)
	{
		if (config->step_mode == FAL_ONE_STEP_MODE)
		{
			ptp_main_conf_reg.bf.ptp_clock_mode =
				PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_OC_ONE_STEP;
		}
		else
		{
			ptp_main_conf_reg.bf.ptp_clock_mode =
				PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_OC_TWO_STEP;
		}
	}
	else
	{
		if (config->clock_mode == FAL_P2PTC_CLOCK_MODE)
		{
			ptp_backup_reg.bf.p2p_tc_en = PTP_REG_BIT_TRUE;
		}
		else
		{
			ptp_backup_reg.bf.p2p_tc_en = PTP_REG_BIT_FALSE;
		}

		if (config->step_mode == FAL_ONE_STEP_MODE &&
		    ptp_misc_config_reg.bf.embed_ingress_time_en == PTP_REG_BIT_TRUE)
		{
			ptp_misc_config_reg.bf.tc_offload = PTP_REG_BIT_TRUE;
		}
		else
		{
			ptp_misc_config_reg.bf.tc_offload = PTP_REG_BIT_FALSE;
		}

		if (config->step_mode == FAL_ONE_STEP_MODE)
		{
			ptp_main_conf_reg.bf.ptp_clock_mode =
				PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_TC_ONE_STEP;
		}
		else
		{
			ptp_main_conf_reg.bf.ptp_clock_mode =
				PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_TC_TWO_STEP;
		}
	}
	if (config->step_mode == FAL_AUTO_MODE)
	{
		ptp_misc_config_reg.bf.pkt_one_step_en = PTP_REG_BIT_TRUE;
	}
	else
	{
		ptp_misc_config_reg.bf.pkt_one_step_en = PTP_REG_BIT_FALSE;
	}
#if defined(IN_LINUX_STD_PTP)
	qca808x_ptp_clock_mode_config(dev_id, phy_id, config->clock_mode, config->step_mode);
#endif
	SW_RTN_ON_ERROR(qca808x_ptp_main_conf_reg_set(dev_id, phy_id, &ptp_main_conf_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &ptp_misc_config_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_backup_reg_set(dev_id, phy_id, &ptp_backup_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_config_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_config_t *config)
{
	union ptp_main_conf_reg_u ptp_main_conf_reg = {0};
	union ptp_misc_config_reg_u ptp_misc_config_reg = {0};
	union ptp_backup_reg_u ptp_backup_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &ptp_main_conf_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &ptp_misc_config_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_backup_reg_get(dev_id, phy_id, &ptp_backup_reg));
	if (ptp_main_conf_reg.bf.ptp_bypass == PTP_REG_BIT_FALSE &&
			ptp_main_conf_reg.bf.disable_1588_phy == PTP_REG_BIT_FALSE)
	{
		config->ptp_en = A_TRUE;
	}
	if (ptp_main_conf_reg.bf.ptp_clock_mode ==
			PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_OC_TWO_STEP)
	{
		config->clock_mode = FAL_OC_CLOCK_MODE;
		config->step_mode = FAL_TWO_STEP_MODE;
	}
	else if (ptp_main_conf_reg.bf.ptp_clock_mode ==
			PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_OC_ONE_STEP)
	{
		config->clock_mode = FAL_OC_CLOCK_MODE;
		config->step_mode = FAL_ONE_STEP_MODE;
	}
	else if (ptp_main_conf_reg.bf.ptp_clock_mode ==
			PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_TC_TWO_STEP)
	{
		if (ptp_backup_reg.bf.p2p_tc_en == PTP_REG_BIT_TRUE)
		{
			config->clock_mode = FAL_P2PTC_CLOCK_MODE;
		}
		else
		{
			config->clock_mode = FAL_E2ETC_CLOCK_MODE;
		}

		config->step_mode = FAL_TWO_STEP_MODE;
	}
	else
	{
		if (ptp_backup_reg.bf.p2p_tc_en == PTP_REG_BIT_TRUE)
		{
			config->clock_mode = FAL_P2PTC_CLOCK_MODE;
		}
		else
		{
			config->clock_mode = FAL_E2ETC_CLOCK_MODE;
		}

		config->step_mode = FAL_ONE_STEP_MODE;
	}
	if (ptp_misc_config_reg.bf.pkt_one_step_en == PTP_REG_BIT_TRUE)
	{
		config->step_mode = FAL_AUTO_MODE;
	}

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_reference_clock_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_reference_clock_t ref_clock)
{
	union ptp_main_conf_reg_u ptp_main_conf_reg = {0};
	union ptp_rtc_clk_reg_u ptp_rtc_clk_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &ptp_main_conf_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_clk_reg_get(dev_id, phy_id, &ptp_rtc_clk_reg));
	if (ref_clock == FAL_REF_CLOCK_LOCAL)
	{
		ptp_main_conf_reg.bf.ptp_clk_sel = PTP_REG_BIT_FALSE;
		ptp_rtc_clk_reg.bf.rtc_clk_selection = PTP_REG_BIT_FALSE;
	}
	else if (ref_clock == FAL_REF_CLOCK_SYNCE)
	{
		ptp_main_conf_reg.bf.ptp_clk_sel = PTP_REG_BIT_TRUE;
		ptp_rtc_clk_reg.bf.rtc_clk_selection = PTP_REG_BIT_FALSE;
	}
	else
	{
		ptp_main_conf_reg.bf.ptp_clk_sel = PTP_REG_BIT_FALSE;
		ptp_rtc_clk_reg.bf.rtc_clk_selection = PTP_REG_BIT_TRUE;
	}
	SW_RTN_ON_ERROR(qca808x_ptp_main_conf_reg_set(dev_id, phy_id, &ptp_main_conf_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_clk_reg_set(dev_id, phy_id, &ptp_rtc_clk_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_reference_clock_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_reference_clock_t *ref_clock)
{
	union ptp_main_conf_reg_u ptp_main_conf_reg = {0};
	union ptp_rtc_clk_reg_u ptp_rtc_clk_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &ptp_main_conf_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_clk_reg_get(dev_id, phy_id, &ptp_rtc_clk_reg));
	if (ptp_main_conf_reg.bf.ptp_clk_sel == PTP_REG_BIT_FALSE &&
			ptp_rtc_clk_reg.bf.rtc_clk_selection == PTP_REG_BIT_FALSE)
	{
		*ref_clock = FAL_REF_CLOCK_LOCAL;
	}
	else if (ptp_main_conf_reg.bf.ptp_clk_sel == PTP_REG_BIT_TRUE &&
			ptp_rtc_clk_reg.bf.rtc_clk_selection == PTP_REG_BIT_FALSE)
	{
		*ref_clock = FAL_REF_CLOCK_SYNCE;
	}
	else
	{
		*ref_clock = FAL_REF_CLOCK_EXTERNAL;
	}

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_rx_timestamp_mode_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_rx_timestamp_mode_t ts_mode)
{
	union ptp_main_conf_reg_u ptp_main_conf_reg = {0};
	union ptp_misc_config_reg_u ptp_misc_config_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &ptp_main_conf_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &ptp_misc_config_reg));
	if (ts_mode == FAL_RX_TS_MDIO)
	{
		ptp_main_conf_reg.bf.ts_attach_mode = PTP_REG_BIT_FALSE;
		ptp_main_conf_reg.bf.ipv6_embed_force_checksum_zero = PTP_REG_BIT_FALSE;
		ptp_misc_config_reg.bf.embed_ingress_time_en = PTP_REG_BIT_FALSE;
		ptp_misc_config_reg.bf.cf_from_pkt_en = PTP_REG_BIT_TRUE;
	}
	else
	{
		ptp_main_conf_reg.bf.ts_attach_mode = PTP_REG_BIT_FALSE;
		ptp_main_conf_reg.bf.ipv6_embed_force_checksum_zero = PTP_REG_BIT_TRUE;
		ptp_misc_config_reg.bf.embed_ingress_time_en = PTP_REG_BIT_TRUE;
		ptp_misc_config_reg.bf.cf_from_pkt_en = PTP_REG_BIT_FALSE;
	}
	SW_RTN_ON_ERROR(qca808x_ptp_main_conf_reg_set(dev_id, phy_id, &ptp_main_conf_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &ptp_misc_config_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_rx_timestamp_mode_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_rx_timestamp_mode_t *ts_mode)
{
	union ptp_main_conf_reg_u ptp_main_conf_reg = {0};
	union ptp_misc_config_reg_u ptp_misc_config_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &ptp_main_conf_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &ptp_misc_config_reg));
	if (ptp_main_conf_reg.bf.ts_attach_mode == PTP_REG_BIT_FALSE &&
			ptp_misc_config_reg.bf.embed_ingress_time_en == PTP_REG_BIT_FALSE)
	{
		*ts_mode = FAL_RX_TS_MDIO;
	}
	else if (ptp_main_conf_reg.bf.ts_attach_mode == PTP_REG_BIT_FALSE &&
			ptp_misc_config_reg.bf.embed_ingress_time_en == PTP_REG_BIT_TRUE)
	{
		*ts_mode = FAL_RX_TS_EMBED;
	}
	else
	{
		return SW_BAD_VALUE;
	}

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_rx_timestamp0_get(a_uint32_t dev_id, a_uint32_t phy_id,
		fal_ptp_pkt_info_t *pkt_info, fal_ptp_time_t *time)
{
	union ptp_rx_seqid0_reg_u ptp_rx_seqid0_reg = {0};

	union ptp_rx_portid0_0_reg_u ptp_rx_portid0_0_reg = {0};
	union ptp_rx_portid0_1_reg_u ptp_rx_portid0_1_reg = {0};
	union ptp_rx_portid0_2_reg_u ptp_rx_portid0_2_reg = {0};
	union ptp_rx_portid0_3_reg_u ptp_rx_portid0_3_reg = {0};
	union ptp_rx_portid0_4_reg_u ptp_rx_portid0_4_reg = {0};
	union ptp_rx_ts0_0_reg_u ptp_rx_ts0_0_reg = {0};
	union ptp_rx_ts0_1_reg_u ptp_rx_ts0_1_reg = {0};
	union ptp_rx_ts0_2_reg_u ptp_rx_ts0_2_reg = {0};
	union ptp_rx_ts0_3_reg_u ptp_rx_ts0_3_reg = {0};
	union ptp_rx_ts0_4_reg_u ptp_rx_ts0_4_reg = {0};
	union ptp_rx_ts0_5_reg_u ptp_rx_ts0_5_reg = {0};
	union ptp_rx_ts0_6_reg_u ptp_rx_ts0_6_reg = {0};

	a_uint64_t clock_id;
	a_uint32_t port_num, msgtype;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_seqid0_reg_get(dev_id, phy_id, &ptp_rx_seqid0_reg));
	if (pkt_info->sequence_id != ptp_rx_seqid0_reg.bf.rx_seqid)
	{
		return SW_NOT_FOUND;
	}

	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid0_0_reg_get(dev_id,
				phy_id, &ptp_rx_portid0_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid0_1_reg_get(dev_id,
				phy_id, &ptp_rx_portid0_1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid0_2_reg_get(dev_id,
				phy_id, &ptp_rx_portid0_2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid0_3_reg_get(dev_id,
				phy_id, &ptp_rx_portid0_3_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid0_4_reg_get(dev_id,
				phy_id, &ptp_rx_portid0_4_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts0_0_reg_get(dev_id,
				phy_id, &ptp_rx_ts0_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts0_1_reg_get(dev_id,
				phy_id, &ptp_rx_ts0_1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts0_2_reg_get(dev_id,
				phy_id, &ptp_rx_ts0_2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts0_3_reg_get(dev_id,
				phy_id, &ptp_rx_ts0_3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts0_4_reg_get(dev_id,
				phy_id, &ptp_rx_ts0_4_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts0_5_reg_get(dev_id,
				phy_id, &ptp_rx_ts0_5_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts0_6_reg_get(dev_id,
				phy_id, &ptp_rx_ts0_6_reg));

	clock_id = ((a_uint64_t)ptp_rx_portid0_0_reg.bf.rx_portid << 48) |
		((a_uint64_t)ptp_rx_portid0_1_reg.bf.rx_portid << 32) |
		((a_uint64_t)ptp_rx_portid0_2_reg.bf.rx_portid << 16) |
		ptp_rx_portid0_3_reg.bf.rx_portid;
	port_num = ptp_rx_portid0_4_reg.bf.rx_portid;
	msgtype = ptp_rx_ts0_5_reg.bf.rx_msg_type;

	if (pkt_info->clock_identify == clock_id &&
			pkt_info->port_number == port_num &&
			pkt_info->msg_type == msgtype)
	{
		time->seconds =
			((a_int64_t)ptp_rx_ts0_0_reg.bf.rx_ts_sec << 32) |
			((a_int64_t)ptp_rx_ts0_1_reg.bf.rx_ts_sec << 16) |
			ptp_rx_ts0_2_reg.bf.rx_ts_sec;
		time->nanoseconds =
			((a_int32_t)ptp_rx_ts0_3_reg.bf.rx_ts_nsec << 16) |
			ptp_rx_ts0_4_reg.bf.rx_ts_nsec;
		time->fracnanoseconds =
			((a_int32_t)ptp_rx_ts0_5_reg.bf.rx_ts_nfsec << 8) |
			ptp_rx_ts0_6_reg.bf.rx_ts_nfsec;

		return SW_OK;
	}
	return SW_NOT_FOUND;
}

sw_error_t
qca808x_phy_ptp_rx_timestamp1_get(a_uint32_t dev_id, a_uint32_t phy_id,
		fal_ptp_pkt_info_t *pkt_info, fal_ptp_time_t *time)
{
	union ptp_rx_seqid1_reg_u ptp_rx_seqid1_reg = {0};

	union ptp_rx_portid1_0_reg_u ptp_rx_portid1_0_reg = {0};
	union ptp_rx_portid1_1_reg_u ptp_rx_portid1_1_reg = {0};
	union ptp_rx_portid1_2_reg_u ptp_rx_portid1_2_reg = {0};
	union ptp_rx_portid1_3_reg_u ptp_rx_portid1_3_reg = {0};
	union ptp_rx_portid1_4_reg_u ptp_rx_portid1_4_reg = {0};
	union ptp_rx_ts1_0_reg_u ptp_rx_ts1_0_reg = {0};
	union ptp_rx_ts1_1_reg_u ptp_rx_ts1_1_reg = {0};
	union ptp_rx_ts1_2_reg_u ptp_rx_ts1_2_reg = {0};
	union ptp_rx_ts1_3_reg_u ptp_rx_ts1_3_reg = {0};
	union ptp_rx_ts1_4_reg_u ptp_rx_ts1_4_reg = {0};
	union ptp_rx_ts1_5_reg_u ptp_rx_ts1_5_reg = {0};
	union ptp_rx_ts1_6_reg_u ptp_rx_ts1_6_reg = {0};

	a_uint64_t clock_id;
	a_uint32_t port_num, msgtype;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_seqid1_reg_get(dev_id, phy_id, &ptp_rx_seqid1_reg));
	if (pkt_info->sequence_id != ptp_rx_seqid1_reg.bf.rx_seqid)
	{
		return SW_NOT_FOUND;
	}

	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid1_0_reg_get(dev_id,
				phy_id, &ptp_rx_portid1_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid1_1_reg_get(dev_id,
				phy_id, &ptp_rx_portid1_1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid1_2_reg_get(dev_id,
				phy_id, &ptp_rx_portid1_2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid1_3_reg_get(dev_id,
				phy_id, &ptp_rx_portid1_3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid1_4_reg_get(dev_id,
				phy_id, &ptp_rx_portid1_4_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts1_0_reg_get(dev_id,
				phy_id, &ptp_rx_ts1_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts1_1_reg_get(dev_id,
				phy_id, &ptp_rx_ts1_1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts1_2_reg_get(dev_id,
				phy_id, &ptp_rx_ts1_2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts1_3_reg_get(dev_id,
				phy_id, &ptp_rx_ts1_3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts1_4_reg_get(dev_id,
				phy_id, &ptp_rx_ts1_4_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts1_5_reg_get(dev_id,
				phy_id, &ptp_rx_ts1_5_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts1_6_reg_get(dev_id,
				phy_id, &ptp_rx_ts1_6_reg));

	clock_id = ((a_uint64_t)ptp_rx_portid1_0_reg.bf.rx_portid << 48) |
		((a_uint64_t)ptp_rx_portid1_1_reg.bf.rx_portid << 32) |
		((a_uint64_t)ptp_rx_portid1_2_reg.bf.rx_portid << 16) |
		ptp_rx_portid1_3_reg.bf.rx_portid;

	port_num = ptp_rx_portid1_4_reg.bf.rx_portid;
	msgtype = ptp_rx_ts1_5_reg.bf.rx_msg_type;
	if (pkt_info->clock_identify == clock_id &&
			pkt_info->port_number == port_num &&
			pkt_info->msg_type == msgtype)
	{
		time->seconds =
			((a_int64_t)ptp_rx_ts1_0_reg.bf.rx_ts_sec << 32) |
			((a_int64_t)ptp_rx_ts1_1_reg.bf.rx_ts_sec << 16) |
			ptp_rx_ts1_2_reg.bf.rx_ts_sec;
		time->nanoseconds =
			((a_int32_t)ptp_rx_ts1_3_reg.bf.rx_ts_nsec << 16) |
			ptp_rx_ts1_4_reg.bf.rx_ts_nsec;
		time->fracnanoseconds =
			((a_int32_t)ptp_rx_ts1_5_reg.bf.rx_ts_nfsec << 8) |
			ptp_rx_ts1_6_reg.bf.rx_ts_nfsec;

		return SW_OK;
	}
	return SW_NOT_FOUND;
}

sw_error_t
qca808x_phy_ptp_rx_timestamp2_get(a_uint32_t dev_id, a_uint32_t phy_id,
		fal_ptp_pkt_info_t *pkt_info, fal_ptp_time_t *time)
{
	union ptp_rx_seqid2_reg_u ptp_rx_seqid2_reg = {0};
	union ptp_rx_portid2_0_reg_u ptp_rx_portid2_0_reg = {0};
	union ptp_rx_portid2_1_reg_u ptp_rx_portid2_1_reg = {0};
	union ptp_rx_portid2_2_reg_u ptp_rx_portid2_2_reg = {0};
	union ptp_rx_portid2_3_reg_u ptp_rx_portid2_3_reg = {0};
	union ptp_rx_portid2_4_reg_u ptp_rx_portid2_4_reg = {0};
	union ptp_rx_ts2_0_reg_u ptp_rx_ts2_0_reg = {0};
	union ptp_rx_ts2_1_reg_u ptp_rx_ts2_1_reg = {0};
	union ptp_rx_ts2_2_reg_u ptp_rx_ts2_2_reg = {0};
	union ptp_rx_ts2_3_reg_u ptp_rx_ts2_3_reg = {0};
	union ptp_rx_ts2_4_reg_u ptp_rx_ts2_4_reg = {0};
	union ptp_rx_ts2_5_reg_u ptp_rx_ts2_5_reg = {0};
	union ptp_rx_ts2_6_reg_u ptp_rx_ts2_6_reg = {0};

	a_uint64_t clock_id;
	a_uint32_t port_num, msgtype;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_seqid2_reg_get(dev_id, phy_id, &ptp_rx_seqid2_reg));
	if (pkt_info->sequence_id != ptp_rx_seqid2_reg.bf.rx_seqid)
	{
		return SW_NOT_FOUND;
	}

	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid2_0_reg_get(dev_id,
				phy_id, &ptp_rx_portid2_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid2_1_reg_get(dev_id,
				phy_id, &ptp_rx_portid2_1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid2_2_reg_get(dev_id,
				phy_id, &ptp_rx_portid2_2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid2_3_reg_get(dev_id,
				phy_id, &ptp_rx_portid2_3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid2_4_reg_get(dev_id,
				phy_id, &ptp_rx_portid2_4_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts2_0_reg_get(dev_id,
				phy_id, &ptp_rx_ts2_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts2_1_reg_get(dev_id,
				phy_id, &ptp_rx_ts2_1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts2_2_reg_get(dev_id,
				phy_id, &ptp_rx_ts2_2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts2_3_reg_get(dev_id,
				phy_id, &ptp_rx_ts2_3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts2_4_reg_get(dev_id,
				phy_id, &ptp_rx_ts2_4_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts2_5_reg_get(dev_id,
				phy_id, &ptp_rx_ts2_5_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts2_6_reg_get(dev_id,
				phy_id, &ptp_rx_ts2_6_reg));

	clock_id = ((a_uint64_t)ptp_rx_portid2_0_reg.bf.rx_portid << 48) |
		((a_uint64_t)ptp_rx_portid2_1_reg.bf.rx_portid << 32) |
		((a_uint64_t)ptp_rx_portid2_2_reg.bf.rx_portid << 16) |
		ptp_rx_portid2_3_reg.bf.rx_portid;
	port_num = ptp_rx_portid2_4_reg.bf.rx_portid;
	msgtype = ptp_rx_ts2_5_reg.bf.rx_msg_type;

	if (pkt_info->clock_identify == clock_id &&
			pkt_info->port_number == port_num &&
			pkt_info->msg_type == msgtype)
	{
		time->seconds =
			((a_int64_t)ptp_rx_ts2_0_reg.bf.rx_ts_sec << 32) |
			((a_int64_t)ptp_rx_ts2_1_reg.bf.rx_ts_sec << 16) |
			ptp_rx_ts2_2_reg.bf.rx_ts_sec;
		time->nanoseconds =
			((a_int32_t)ptp_rx_ts2_3_reg.bf.rx_ts_nsec << 16) |
			ptp_rx_ts2_4_reg.bf.rx_ts_nsec;
		time->fracnanoseconds =
			((a_int32_t)ptp_rx_ts2_5_reg.bf.rx_ts_nfsec << 8) |
			ptp_rx_ts2_6_reg.bf.rx_ts_nfsec;

		return SW_OK;
	}
	return SW_NOT_FOUND;
}

sw_error_t
qca808x_phy_ptp_rx_timestamp3_get(a_uint32_t dev_id, a_uint32_t phy_id,
		fal_ptp_pkt_info_t *pkt_info, fal_ptp_time_t *time)
{
	union ptp_rx_seqid3_reg_u ptp_rx_seqid3_reg = {0};
	union ptp_rx_portid3_0_reg_u ptp_rx_portid3_0_reg = {0};
	union ptp_rx_portid3_1_reg_u ptp_rx_portid3_1_reg = {0};
	union ptp_rx_portid3_2_reg_u ptp_rx_portid3_2_reg = {0};
	union ptp_rx_portid3_3_reg_u ptp_rx_portid3_3_reg = {0};
	union ptp_rx_portid3_4_reg_u ptp_rx_portid3_4_reg = {0};
	union ptp_rx_ts3_0_reg_u ptp_rx_ts3_0_reg = {0};
	union ptp_rx_ts3_1_reg_u ptp_rx_ts3_1_reg = {0};
	union ptp_rx_ts3_2_reg_u ptp_rx_ts3_2_reg = {0};
	union ptp_rx_ts3_3_reg_u ptp_rx_ts3_3_reg = {0};
	union ptp_rx_ts3_4_reg_u ptp_rx_ts3_4_reg = {0};
	union ptp_rx_ts3_5_reg_u ptp_rx_ts3_5_reg = {0};
	union ptp_rx_ts3_6_reg_u ptp_rx_ts3_6_reg = {0};

	a_uint64_t clock_id;
	a_uint32_t port_num, msgtype;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_seqid3_reg_get(dev_id, phy_id, &ptp_rx_seqid3_reg));
	if (pkt_info->sequence_id != ptp_rx_seqid3_reg.bf.rx_seqid)
	{
		return SW_NOT_FOUND;
	}
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid3_0_reg_get(dev_id,
				phy_id, &ptp_rx_portid3_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid3_1_reg_get(dev_id,
				phy_id, &ptp_rx_portid3_1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid3_2_reg_get(dev_id,
				phy_id, &ptp_rx_portid3_2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid3_3_reg_get(dev_id,
				phy_id, &ptp_rx_portid3_3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_portid3_4_reg_get(dev_id,
				phy_id, &ptp_rx_portid3_4_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts3_0_reg_get(dev_id,
				phy_id, &ptp_rx_ts3_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts3_1_reg_get(dev_id,
				phy_id, &ptp_rx_ts3_1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts3_2_reg_get(dev_id,
				phy_id, &ptp_rx_ts3_2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts3_3_reg_get(dev_id,
				phy_id, &ptp_rx_ts3_3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts3_4_reg_get(dev_id,
				phy_id, &ptp_rx_ts3_4_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts3_5_reg_get(dev_id,
				phy_id, &ptp_rx_ts3_5_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_ts3_6_reg_get(dev_id,
				phy_id, &ptp_rx_ts3_6_reg));

	clock_id = ((a_uint64_t)ptp_rx_portid3_0_reg.bf.rx_portid << 48) |
		((a_uint64_t)ptp_rx_portid3_1_reg.bf.rx_portid << 32) |
		((a_uint64_t)ptp_rx_portid3_2_reg.bf.rx_portid << 16) |
		ptp_rx_portid3_3_reg.bf.rx_portid;
	port_num = ptp_rx_portid3_4_reg.bf.rx_portid;
	msgtype = ptp_rx_ts3_5_reg.bf.rx_msg_type;
	if (pkt_info->clock_identify == clock_id &&
			pkt_info->port_number == port_num &&
			pkt_info->msg_type == msgtype)
	{
		time->seconds =
			((a_int64_t)ptp_rx_ts3_0_reg.bf.rx_ts_sec << 32) |
			((a_int64_t)ptp_rx_ts3_1_reg.bf.rx_ts_sec << 16) |
			ptp_rx_ts3_2_reg.bf.rx_ts_sec;
		time->nanoseconds =
			((a_int32_t)ptp_rx_ts3_3_reg.bf.rx_ts_nsec << 16) |
			ptp_rx_ts3_4_reg.bf.rx_ts_nsec;
		time->fracnanoseconds =
			((a_int32_t)ptp_rx_ts3_5_reg.bf.rx_ts_nfsec << 8) |
			ptp_rx_ts3_6_reg.bf.rx_ts_nfsec;

		return SW_OK;
	}
	return SW_NOT_FOUND;
}

sw_error_t
qca808x_phy_ptp_tx_timestamp0_get(a_uint32_t dev_id, a_uint32_t phy_id,
		fal_ptp_pkt_info_t *pkt_info, fal_ptp_time_t *time)
{
	union ptp_tx_seqid_reg_u ptp_tx_seqid_reg = {0};
	union ptp_tx_portid0_reg_u ptp_tx_portid0_reg = {0};
	union ptp_tx_portid1_reg_u ptp_tx_portid1_reg = {0};
	union ptp_tx_portid2_reg_u ptp_tx_portid2_reg = {0};
	union ptp_tx_portid3_reg_u ptp_tx_portid3_reg = {0};
	union ptp_tx_portid4_reg_u ptp_tx_portid4_reg = {0};
	union ptp_tx_ts0_reg_u ptp_tx_ts0_reg = {0};
	union ptp_tx_ts1_reg_u ptp_tx_ts1_reg = {0};
	union ptp_tx_ts2_reg_u ptp_tx_ts2_reg = {0};
	union ptp_tx_ts3_reg_u ptp_tx_ts3_reg = {0};
	union ptp_tx_ts4_reg_u ptp_tx_ts4_reg = {0};
	union ptp_tx_ts5_reg_u ptp_tx_ts5_reg = {0};
	union ptp_tx_ts6_reg_u ptp_tx_ts6_reg = {0};

	a_uint64_t clock_id;
	a_uint32_t port_num, msgtype;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_seqid_reg_get(dev_id, phy_id, &ptp_tx_seqid_reg));
	if (pkt_info->sequence_id != ptp_tx_seqid_reg.bf.tx_seqid)
	{
		return SW_NOT_FOUND;
	}

	SW_RTN_ON_ERROR(qca808x_ptp_tx_portid0_reg_get(dev_id,
				phy_id, &ptp_tx_portid0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_portid1_reg_get(dev_id,
				phy_id, &ptp_tx_portid1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_portid2_reg_get(dev_id,
				phy_id, &ptp_tx_portid2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_portid3_reg_get(dev_id,
				phy_id, &ptp_tx_portid3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_portid4_reg_get(dev_id,
				phy_id, &ptp_tx_portid4_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_ts0_reg_get(dev_id,
				phy_id, &ptp_tx_ts0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_ts1_reg_get(dev_id,
				phy_id, &ptp_tx_ts1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_ts2_reg_get(dev_id,
				phy_id, &ptp_tx_ts2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_ts3_reg_get(dev_id,
				phy_id, &ptp_tx_ts3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_ts4_reg_get(dev_id,
				phy_id, &ptp_tx_ts4_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_ts5_reg_get(dev_id,
				phy_id, &ptp_tx_ts5_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_ts6_reg_get(dev_id,
				phy_id, &ptp_tx_ts6_reg));
	clock_id = ((a_uint64_t)ptp_tx_portid0_reg.bf.tx_portid << 48) |
		((a_uint64_t)ptp_tx_portid1_reg.bf.tx_portid << 32) |
		((a_uint64_t)ptp_tx_portid2_reg.bf.tx_portid << 16) |
		ptp_tx_portid3_reg.bf.tx_portid;
	port_num = ptp_tx_portid4_reg.bf.tx_portid;
	msgtype = ptp_tx_ts5_reg.bf.tx_msg_type;
	if (pkt_info->clock_identify == clock_id &&
			pkt_info->port_number == port_num &&
			pkt_info->msg_type == msgtype)
	{
		time->seconds =
			((a_int64_t)ptp_tx_ts0_reg.bf.tx_ts_sec << 32) |
			((a_int64_t)ptp_tx_ts1_reg.bf.tx_ts_sec << 16) |
			ptp_tx_ts2_reg.bf.tx_ts_sec;
		time->nanoseconds =
			((a_int32_t)ptp_tx_ts3_reg.bf.tx_ts_nsec << 16) |
			ptp_tx_ts4_reg.bf.tx_ts_nsec;
		time->fracnanoseconds =
			((a_int32_t)ptp_tx_ts5_reg.bf.tx_ts_nfsec << 8) |
			ptp_tx_ts6_reg.bf.tx_ts_nfsec;

		return SW_OK;
	}
	return SW_NOT_FOUND;
}

sw_error_t
qca808x_phy_ptp_timestamp_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_direction_t direction,
		fal_ptp_pkt_info_t *pkt_info, fal_ptp_time_t *time)
{
	sw_error_t ret = SW_NOT_FOUND;
	a_uint32_t seq = 0;
	if (direction == FAL_RX_DIRECTION)
	{
		for (seq = 0; seq < 4; seq++) {
			switch (seq) {
				case 0:
					ret = qca808x_phy_ptp_rx_timestamp0_get(dev_id, phy_id,
							pkt_info, time);
					if (ret == SW_OK) {
						return ret;
					}
					break;
				case 1:
					ret = qca808x_phy_ptp_rx_timestamp1_get(dev_id, phy_id,
							pkt_info, time);
					if (ret == SW_OK) {
						return ret;
					}
					break;
				case 2:
					ret = qca808x_phy_ptp_rx_timestamp2_get(dev_id, phy_id,
							pkt_info, time);
					if (ret == SW_OK) {
						return ret;
					}
					break;
				case 3:
					ret = qca808x_phy_ptp_rx_timestamp3_get(dev_id, phy_id,
							pkt_info, time);
					if (ret == SW_OK) {
						return ret;
					}
					break;
				default:
					break;
			}
		}
	} else {
		ret = qca808x_phy_ptp_tx_timestamp0_get(dev_id, phy_id, pkt_info, time);
	}

	return ret;
}

sw_error_t
qca808x_phy_ptp_pkt_timestamp_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time)
{
	union ptp_in_trig0_reg_u ptp_in_trig0_reg = {0};
	union ptp_in_trig1_reg_u ptp_in_trig1_reg = {0};
	union ptp_in_trig2_reg_u ptp_in_trig2_reg = {0};
	union ptp_in_trig3_reg_u ptp_in_trig3_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_in_trig0_reg_get(dev_id, phy_id, &ptp_in_trig0_reg));
	ptp_in_trig0_reg.bf.ptp_in_trig_nisec = time->nanoseconds >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_in_trig0_reg_set(dev_id, phy_id, &ptp_in_trig0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_in_trig1_reg_get(dev_id, phy_id, &ptp_in_trig1_reg));
	ptp_in_trig1_reg.bf.ptp_in_trig_nisec = time->nanoseconds & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_in_trig1_reg_set(dev_id, phy_id, &ptp_in_trig1_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_in_trig2_reg_get(dev_id, phy_id, &ptp_in_trig2_reg));
	ptp_in_trig2_reg.bf.ptp_in_trig_nisec = time->fracnanoseconds >> 4;
	SW_RTN_ON_ERROR(qca808x_ptp_in_trig2_reg_set(dev_id, phy_id, &ptp_in_trig2_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_in_trig3_reg_get(dev_id, phy_id, &ptp_in_trig3_reg));
	ptp_in_trig3_reg.bf.ptp_in_trig_nisec = time->fracnanoseconds & 0xf;
	SW_RTN_ON_ERROR(qca808x_ptp_in_trig3_reg_set(dev_id, phy_id, &ptp_in_trig3_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_pkt_timestamp_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time)
{
	union ptp_in_trig0_reg_u ptp_in_trig0_reg = {0};
	union ptp_in_trig1_reg_u ptp_in_trig1_reg = {0};
	union ptp_in_trig2_reg_u ptp_in_trig2_reg = {0};
	union ptp_in_trig3_reg_u ptp_in_trig3_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_in_trig0_reg_get(dev_id, phy_id, &ptp_in_trig0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_in_trig1_reg_get(dev_id, phy_id, &ptp_in_trig1_reg));
	time->nanoseconds = (ptp_in_trig0_reg.bf.ptp_in_trig_nisec << 16) |
		ptp_in_trig1_reg.bf.ptp_in_trig_nisec;

	SW_RTN_ON_ERROR(qca808x_ptp_in_trig2_reg_get(dev_id, phy_id, &ptp_in_trig2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_in_trig3_reg_get(dev_id, phy_id, &ptp_in_trig3_reg));
	time->fracnanoseconds = (ptp_in_trig2_reg.bf.ptp_in_trig_nisec << 4) |
			(ptp_in_trig3_reg.bf.ptp_in_trig_nisec & 0xf);

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_grandmaster_mode_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_grandmaster_mode_t *gm_mode)
{
	union ptp_gm_conf0_reg_u ptp_gm_conf0_reg = {0};
	union ptp_gm_conf1_reg_u ptp_gm_conf1_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_gm_conf0_reg_get(dev_id, phy_id, &ptp_gm_conf0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_gm_conf1_reg_get(dev_id, phy_id, &ptp_gm_conf1_reg));
	ptp_gm_conf0_reg.bf.grandmaster_mode = gm_mode->grandmaster_mode_en;
	if (gm_mode->ns_sync_mode == FAL_GM_PPSIN_MODE)
	{
		ptp_gm_conf0_reg.bf.gm_pps_sync = PTP_REG_BIT_TRUE;
		ptp_gm_conf0_reg.bf.gm_pll_mode = PTP_REG_BIT_FALSE;
	}
	else if (gm_mode->ns_sync_mode == FAL_GM_HWPLL_MODE)
	{
		ptp_gm_conf0_reg.bf.gm_pps_sync = PTP_REG_BIT_FALSE;
		ptp_gm_conf0_reg.bf.gm_pll_mode = PTP_REG_BIT_TRUE;
	}
	else
	{
		ptp_gm_conf0_reg.bf.gm_pps_sync = PTP_REG_BIT_FALSE;
		ptp_gm_conf0_reg.bf.gm_pll_mode = PTP_REG_BIT_FALSE;
	}
	ptp_gm_conf0_reg.bf.gm_maxfreq_offset = gm_mode->freq_offset;
	ptp_gm_conf1_reg.bf.gm_kp_ldn =
		(gm_mode->right_shift_in_kp << (PTP_GM_CONF1_REG_GM_KP_LDN_LEN-1)) |
		gm_mode->kp_value;
	ptp_gm_conf1_reg.bf.gm_ki_ldn =
		(gm_mode->right_shift_in_ki << (PTP_GM_CONF1_REG_GM_KI_LDN_LEN-1)) |
		gm_mode->ki_value;
	SW_RTN_ON_ERROR(qca808x_ptp_gm_conf0_reg_set(dev_id, phy_id, &ptp_gm_conf0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_gm_conf1_reg_set(dev_id, phy_id, &ptp_gm_conf1_reg));

#if defined(IN_LINUX_STD_PTP)
	if (gm_mode->grandmaster_second_sync_en == A_TRUE) {
		qca808x_ptp_gm_gps_seconds_sync_enable(dev_id, phy_id, A_TRUE);
	} else {
		qca808x_ptp_gm_gps_seconds_sync_enable(dev_id, phy_id, A_FALSE);
	}
#endif

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_grandmaster_mode_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_grandmaster_mode_t *gm_mode)
{
	union ptp_gm_conf0_reg_u ptp_gm_conf0_reg = {0};
	union ptp_gm_conf1_reg_u ptp_gm_conf1_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_gm_conf0_reg_get(dev_id, phy_id, &ptp_gm_conf0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_gm_conf1_reg_get(dev_id, phy_id, &ptp_gm_conf1_reg));
	gm_mode->grandmaster_mode_en = ptp_gm_conf0_reg.bf.grandmaster_mode;
	if (ptp_gm_conf0_reg.bf.gm_pps_sync == 1 && ptp_gm_conf0_reg.bf.gm_pll_mode == 0)
	{
		gm_mode->ns_sync_mode = FAL_GM_PPSIN_MODE;
	}
	else if (ptp_gm_conf0_reg.bf.gm_pps_sync == 0 && ptp_gm_conf0_reg.bf.gm_pll_mode == 1)
	{
		gm_mode->ns_sync_mode = FAL_GM_HWPLL_MODE;
	}
	else
	{
		gm_mode->ns_sync_mode = FAL_GM_SWPLL_MODE;
	}
	gm_mode->freq_offset = ptp_gm_conf0_reg.bf.gm_maxfreq_offset;
	gm_mode->right_shift_in_kp = ptp_gm_conf1_reg.bf.gm_kp_ldn >> 5;
	gm_mode->kp_value = ptp_gm_conf1_reg.bf.gm_kp_ldn & 0x1f;
	gm_mode->right_shift_in_ki = ptp_gm_conf1_reg.bf.gm_ki_ldn >> 5;
	gm_mode->ki_value = ptp_gm_conf1_reg.bf.gm_ki_ldn & 0x1f;

#if defined(IN_LINUX_STD_PTP)
	if (qca808x_ptp_gm_gps_seconds_sync_status_get(dev_id, phy_id) == A_TRUE) {
		gm_mode->grandmaster_second_sync_en = A_TRUE;
	} else {
		gm_mode->grandmaster_second_sync_en = A_FALSE;
	}
#endif

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_rtc_time_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time)
{
	union ptp_rtc0_reg_u ptp_rtc0_reg = {0};
	union ptp_rtc1_reg_u ptp_rtc1_reg = {0};
	union ptp_rtc2_reg_u ptp_rtc2_reg = {0};
	union ptp_rtc3_reg_u ptp_rtc3_reg = {0};
	union ptp_rtc4_reg_u ptp_rtc4_reg = {0};
	union ptp_rtc5_reg_u ptp_rtc5_reg = {0};
	union ptp_rtc6_reg_u ptp_rtc6_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rtc0_reg_get(dev_id, phy_id, &ptp_rtc0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtc1_reg_get(dev_id, phy_id, &ptp_rtc1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtc2_reg_get(dev_id, phy_id, &ptp_rtc2_reg));
	time->seconds = ((a_int64_t)ptp_rtc0_reg.bf.ptp_rtc_sec << 32) |
			(ptp_rtc1_reg.bf.ptp_rtc_sec << 16) | ptp_rtc2_reg.bf.ptp_rtc_sec;

	SW_RTN_ON_ERROR(qca808x_ptp_rtc3_reg_get(dev_id, phy_id, &ptp_rtc3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtc4_reg_get(dev_id, phy_id, &ptp_rtc4_reg));
	time->nanoseconds = (ptp_rtc3_reg.bf.ptp_rtc_nisec << 16) | ptp_rtc4_reg.bf.ptp_rtc_nisec;

	SW_RTN_ON_ERROR(qca808x_ptp_rtc5_reg_get(dev_id, phy_id, &ptp_rtc5_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtc6_reg_get(dev_id, phy_id, &ptp_rtc6_reg));
	time->fracnanoseconds = (ptp_rtc5_reg.bf.ptp_rtc_nfsec << 4) |
		ptp_rtc6_reg.bf.ptp_rtc_nfsec;

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_rtc_time_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time)
{
	union ptp_rtc_preloaded0_reg_u ptp_rtc_preloaded0_reg = {0};
	union ptp_rtc_preloaded1_reg_u ptp_rtc_preloaded1_reg = {0};
	union ptp_rtc_preloaded2_reg_u ptp_rtc_preloaded2_reg = {0};
	union ptp_rtc_preloaded3_reg_u ptp_rtc_preloaded3_reg = {0};
	union ptp_rtc_preloaded4_reg_u ptp_rtc_preloaded4_reg = {0};
	union ptp_rtc_ext_conf_reg_u ptp_rtc_ext_conf_reg = {0};

	ptp_rtc_preloaded0_reg.bf.ptp_rtc_preloaded_sec = (time->seconds >> 32) & 0xffff;
	ptp_rtc_preloaded1_reg.bf.ptp_rtc_preloaded_sec = (time->seconds >> 16) & 0xffff;
	ptp_rtc_preloaded2_reg.bf.ptp_rtc_preloaded_sec = time->seconds & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_preloaded0_reg_set(dev_id,
				phy_id, &ptp_rtc_preloaded0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_preloaded1_reg_set(dev_id,
				phy_id, &ptp_rtc_preloaded1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_preloaded2_reg_set(dev_id,
				phy_id, &ptp_rtc_preloaded2_reg));

	ptp_rtc_preloaded3_reg.bf.ptp_rtc_preloaded_nisec = time->nanoseconds >> 16;
	ptp_rtc_preloaded4_reg.bf.ptp_rtc_preloaded_nisec = time->nanoseconds & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_preloaded3_reg_set(dev_id,
				phy_id, &ptp_rtc_preloaded3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_preloaded4_reg_set(dev_id,
				phy_id, &ptp_rtc_preloaded4_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_get(dev_id,
				phy_id, &ptp_rtc_ext_conf_reg));
	ptp_rtc_ext_conf_reg.bf.load_rtc = 1;
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_set(dev_id,
				phy_id, &ptp_rtc_ext_conf_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_rtc_time_clear(a_uint32_t dev_id,
		a_uint32_t phy_id)
{
	union ptp_rtc_ext_conf_reg_u ptp_rtc_ext_conf_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &ptp_rtc_ext_conf_reg));
	ptp_rtc_ext_conf_reg.bf.clear_rtc = PTP_REG_BIT_TRUE;
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_set(dev_id, phy_id, &ptp_rtc_ext_conf_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_rtc_adjtime_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time)
{
	union ptp_rtcoffs0_reg_u ptp_rtcoffs0_reg = {0};
	union ptp_rtcoffs1_reg_u ptp_rtcoffs1_reg = {0};
	union ptp_rtcoffs2_reg_u ptp_rtcoffs2_reg = {0};
	union ptp_rtcoffs3_reg_u ptp_rtcoffs3_reg = {0};
	union ptp_rtcoffs4_reg_u ptp_rtcoffs4_reg = {0};
	union ptp_rtcoffs_valid_reg_u ptp_rtcoffs_valid_reg = {0};

	ptp_rtcoffs0_reg.bf.ptp_rtcoffs_nsec = time->nanoseconds >> 16;
	ptp_rtcoffs1_reg.bf.ptp_rtcoffs_nsec = time->nanoseconds & 0xffff;
	ptp_rtcoffs2_reg.bf.ptp_rtcoffs_sec = (time->seconds >> 32) & 0xffff;
	ptp_rtcoffs3_reg.bf.ptp_rtcoffs_sec = (time->seconds >> 16) & 0xffff;
	ptp_rtcoffs4_reg.bf.ptp_rtcoffs_sec = time->seconds & 0xffff;
	ptp_rtcoffs_valid_reg.bf.ptp_rtcoffs_valid = 1;
	SW_RTN_ON_ERROR(qca808x_ptp_rtcoffs0_reg_set(dev_id, phy_id, &ptp_rtcoffs0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtcoffs1_reg_set(dev_id, phy_id, &ptp_rtcoffs1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtcoffs2_reg_set(dev_id, phy_id, &ptp_rtcoffs2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtcoffs3_reg_set(dev_id, phy_id, &ptp_rtcoffs3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtcoffs4_reg_set(dev_id, phy_id, &ptp_rtcoffs4_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtcoffs_valid_reg_set(dev_id, phy_id, &ptp_rtcoffs_valid_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_rtc_adjfreq_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time)
{
	union ptp_rtc_ext_conf_reg_u ptp_rtc_ext_conf_reg = {0};
	union ptp_rtc_inc0_reg_u ptp_rtc_inc0_reg = {0};
	union ptp_rtc_inc1_reg_u ptp_rtc_inc1_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rtc_inc0_reg_get(dev_id, phy_id, &ptp_rtc_inc0_reg));
	ptp_rtc_inc0_reg.bf.ptp_rtc_inc_nis = time->nanoseconds & 0x3f;
	ptp_rtc_inc0_reg.bf.ptp_rtc_inc_nfs = (time->fracnanoseconds >> 16) & 0x3ff;
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_inc0_reg_set(dev_id, phy_id, &ptp_rtc_inc0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rtc_inc1_reg_get(dev_id, phy_id, &ptp_rtc_inc1_reg));
	ptp_rtc_inc1_reg.bf.ptp_rtc_inc_nfs = time->fracnanoseconds & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_inc1_reg_set(dev_id, phy_id, &ptp_rtc_inc1_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &ptp_rtc_ext_conf_reg));
	ptp_rtc_ext_conf_reg.bf.set_incval_valid = PTP_REG_BIT_TRUE;
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_set(dev_id, phy_id, &ptp_rtc_ext_conf_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_rtc_adjfreq_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time)
{
	union ptp_rtc_inc0_reg_u ptp_rtc_inc0_reg = {0};
	union ptp_rtc_inc1_reg_u ptp_rtc_inc1_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rtc_inc0_reg_get(dev_id, phy_id, &ptp_rtc_inc0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_inc1_reg_get(dev_id, phy_id, &ptp_rtc_inc1_reg));
	time->nanoseconds = ptp_rtc_inc0_reg.bf.ptp_rtc_inc_nis;
	time->fracnanoseconds = (ptp_rtc_inc0_reg.bf.ptp_rtc_inc_nfs << 16) |
		ptp_rtc_inc1_reg.bf.ptp_rtc_inc_nfs;

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_link_delay_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time)
{
	union ptp_link_delay_0_reg_u ptp_link_delay_0_reg = {0};
	union ptp_link_delay_1_reg_u ptp_link_delay_1_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_link_delay_0_reg_get(dev_id, phy_id, &ptp_link_delay_0_reg));
	ptp_link_delay_0_reg.bf.link_delay = time->nanoseconds >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_link_delay_0_reg_set(dev_id, phy_id, &ptp_link_delay_0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_link_delay_1_reg_get(dev_id, phy_id, &ptp_link_delay_1_reg));
	ptp_link_delay_1_reg.bf.link_delay = time->nanoseconds & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_link_delay_1_reg_set(dev_id, phy_id, &ptp_link_delay_1_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_link_delay_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time)
{
	union ptp_link_delay_0_reg_u ptp_link_delay_0_reg = {0};
	union ptp_link_delay_1_reg_u ptp_link_delay_1_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_link_delay_0_reg_get(dev_id, phy_id, &ptp_link_delay_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_link_delay_1_reg_get(dev_id, phy_id, &ptp_link_delay_1_reg));
	time->nanoseconds = (ptp_link_delay_0_reg.bf.link_delay << 16) |
		ptp_link_delay_1_reg.bf.link_delay;

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_security_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_security_t *sec)
{
	union ptp_misc_config_reg_u ptp_misc_config_reg = {0};
	union ptp_main_conf_reg_u ptp_main_conf_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &ptp_misc_config_reg));
	ptp_misc_config_reg.bf.ptp_addr_chk_en = sec->address_check_en;
	ptp_misc_config_reg.bf.ipv6_udp_chk_en = sec->ipv6_udp_checksum_recal_en;
	ptp_misc_config_reg.bf.ptp_ver_chk_en = sec->version_check_en;
	ptp_misc_config_reg.bf.ptp_version = sec->ptp_version;
	SW_RTN_ON_ERROR(qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &ptp_misc_config_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &ptp_main_conf_reg));
	ptp_main_conf_reg.bf.ipv4_force_checksum_zero =
		sec->ipv4_udp_checksum_force_zero_en;
	ptp_main_conf_reg.bf.ipv6_embed_force_checksum_zero =
		sec->ipv6_embed_udp_checksum_force_zero_en;
	SW_RTN_ON_ERROR(qca808x_ptp_main_conf_reg_set(dev_id, phy_id, &ptp_main_conf_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_security_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_security_t *sec)
{
	union ptp_misc_config_reg_u ptp_misc_config_reg = {0};
	union ptp_main_conf_reg_u ptp_main_conf_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &ptp_misc_config_reg));
	sec->address_check_en = ptp_misc_config_reg.bf.ptp_addr_chk_en;
	sec->ipv6_udp_checksum_recal_en = ptp_misc_config_reg.bf.ipv6_udp_chk_en;
	sec->version_check_en = ptp_misc_config_reg.bf.ptp_ver_chk_en;
	sec->ptp_version = ptp_misc_config_reg.bf.ptp_version;

	SW_RTN_ON_ERROR(qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &ptp_main_conf_reg));
	sec->ipv4_udp_checksum_force_zero_en = ptp_main_conf_reg.bf.ipv4_force_checksum_zero;
	sec->ipv6_embed_udp_checksum_force_zero_en =
		ptp_main_conf_reg.bf.ipv6_embed_force_checksum_zero;

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_pps_signal_control_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_pps_signal_control_t *sig_control)
{
	union ptp_ppsin_latency_reg_u ptp_ppsin_latency_reg = {0};
	union ptp_phase_adjust_0_reg_u ptp_phase_adjust_0_reg = {0};
	union ptp_phase_adjust_1_reg_u ptp_phase_adjust_1_reg = {0};
	union ptp_pps_pul_width_0_reg_u ptp_pps_pul_width_0_reg = {0};
	union ptp_pps_pul_width_1_reg_u ptp_pps_pul_width_1_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_ppsin_latency_reg_get(dev_id,
				phy_id, &ptp_ppsin_latency_reg));
	ptp_ppsin_latency_reg.bf.ptp_ppsin_latency_sign = sig_control->negative_in_latency;
	ptp_ppsin_latency_reg.bf.ptp_ppsin_latency_value = sig_control->in_latency;
	SW_RTN_ON_ERROR(qca808x_ptp_ppsin_latency_reg_set(dev_id,
				phy_id, &ptp_ppsin_latency_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_phase_adjust_0_reg_get(dev_id,
				phy_id, &ptp_phase_adjust_0_reg));
	ptp_phase_adjust_0_reg.bf.phase_value = sig_control->out_phase >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_phase_adjust_0_reg_set(dev_id,
				phy_id, &ptp_phase_adjust_0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_phase_adjust_1_reg_get(dev_id,
				phy_id, &ptp_phase_adjust_1_reg));
	ptp_phase_adjust_1_reg.bf.phase_value = sig_control->out_phase & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_phase_adjust_1_reg_set(dev_id,
				phy_id, &ptp_phase_adjust_1_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_pps_pul_width_0_reg_get(dev_id,
				phy_id, &ptp_pps_pul_width_0_reg));
	ptp_pps_pul_width_0_reg.bf.pul_value = sig_control->out_pulse_width >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_pps_pul_width_0_reg_set(dev_id,
				phy_id, &ptp_pps_pul_width_0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_pps_pul_width_1_reg_get(dev_id,
				phy_id, &ptp_pps_pul_width_1_reg));
	ptp_pps_pul_width_1_reg.bf.pul_value = sig_control->out_pulse_width & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_pps_pul_width_1_reg_set(dev_id,
				phy_id, &ptp_pps_pul_width_1_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_pps_signal_control_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_pps_signal_control_t *sig_control)
{
	union ptp_ppsin_latency_reg_u ptp_ppsin_latency_reg = {0};
	union ptp_phase_adjust_0_reg_u ptp_phase_adjust_0_reg = {0};
	union ptp_phase_adjust_1_reg_u ptp_phase_adjust_1_reg = {0};
	union ptp_pps_pul_width_0_reg_u ptp_pps_pul_width_0_reg = {0};
	union ptp_pps_pul_width_1_reg_u ptp_pps_pul_width_1_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_ppsin_latency_reg_get(dev_id,
				phy_id, &ptp_ppsin_latency_reg));
	sig_control->negative_in_latency = ptp_ppsin_latency_reg.bf.ptp_ppsin_latency_sign;
	sig_control->in_latency = ptp_ppsin_latency_reg.bf.ptp_ppsin_latency_value;

	SW_RTN_ON_ERROR(qca808x_ptp_phase_adjust_0_reg_get(dev_id,
				phy_id, &ptp_phase_adjust_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_phase_adjust_1_reg_get(dev_id,
				phy_id, &ptp_phase_adjust_1_reg));
	sig_control->out_phase = (ptp_phase_adjust_0_reg.bf.phase_value << 16) |
			ptp_phase_adjust_1_reg.bf.phase_value;

	SW_RTN_ON_ERROR(qca808x_ptp_pps_pul_width_0_reg_get(dev_id,
				phy_id, &ptp_pps_pul_width_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_pps_pul_width_1_reg_get(dev_id,
				phy_id, &ptp_pps_pul_width_1_reg));
	sig_control->out_pulse_width = (ptp_pps_pul_width_0_reg.bf.pul_value << 16) |
			ptp_pps_pul_width_1_reg.bf.pul_value;

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_rx_crc_recalc_enable(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t status)
{
	union ptp_misc_config_reg_u ptp_misc_config_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &ptp_misc_config_reg));
	ptp_misc_config_reg.bf.crc_validate_en = status;
	SW_RTN_ON_ERROR(qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &ptp_misc_config_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_rx_crc_recalc_status_get(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t *status)
{
	union ptp_misc_config_reg_u ptp_misc_config_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &ptp_misc_config_reg));
	*status = ptp_misc_config_reg.bf.crc_validate_en;

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_asym_correction_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_asym_correction_t *asym_cf)
{
	union ptp_misc_control_reg_u ptp_misc_control_reg = {0};
	union ptp_ingress_asymmetry_0_reg_u ptp_ingress_asymmetry_0_reg = {0};
	union ptp_ingress_asymmetry_1_reg_u ptp_ingress_asymmetry_1_reg = {0};
	union ptp_egress_asymmetry_0_reg_u ptp_egress_asymmetry_0_reg = {0};
	union ptp_egress_asymmetry_1_reg_u ptp_egress_asymmetry_1_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_misc_control_reg_get(dev_id,
				phy_id, &ptp_misc_control_reg));
	ptp_misc_control_reg.bf.eg_asym_en = asym_cf->eg_asym_en;
	ptp_misc_control_reg.bf.in_asym_en = asym_cf->in_asym_en;
	SW_RTN_ON_ERROR(qca808x_ptp_misc_control_reg_set(dev_id,
				phy_id, &ptp_misc_control_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_ingress_asymmetry_0_reg_get(dev_id,
				phy_id, &ptp_ingress_asymmetry_0_reg));
	ptp_ingress_asymmetry_0_reg.bf.in_asym = asym_cf->in_asym_value >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_ingress_asymmetry_0_reg_set(dev_id,
				phy_id, &ptp_ingress_asymmetry_0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_ingress_asymmetry_1_reg_get(dev_id,
				phy_id, &ptp_ingress_asymmetry_1_reg));
	ptp_ingress_asymmetry_1_reg.bf.in_asym = asym_cf->in_asym_value & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_ingress_asymmetry_1_reg_set(dev_id,
				phy_id, &ptp_ingress_asymmetry_1_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_egress_asymmetry_0_reg_get(dev_id,
				phy_id, &ptp_egress_asymmetry_0_reg));
	ptp_egress_asymmetry_0_reg.bf.eg_asym = asym_cf->eg_asym_value >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_egress_asymmetry_0_reg_get(dev_id,
				phy_id, &ptp_egress_asymmetry_0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_egress_asymmetry_1_reg_get(dev_id,
				phy_id, &ptp_egress_asymmetry_1_reg));
	ptp_egress_asymmetry_1_reg.bf.eg_asym = asym_cf->eg_asym_value & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_egress_asymmetry_1_reg_set(dev_id,
				phy_id, &ptp_egress_asymmetry_1_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_asym_correction_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_asym_correction_t* asym_cf)
{
	union ptp_misc_control_reg_u ptp_misc_control_reg = {0};
	union ptp_ingress_asymmetry_0_reg_u ptp_ingress_asymmetry_0_reg = {0};
	union ptp_ingress_asymmetry_1_reg_u ptp_ingress_asymmetry_1_reg = {0};
	union ptp_egress_asymmetry_0_reg_u ptp_egress_asymmetry_0_reg = {0};
	union ptp_egress_asymmetry_1_reg_u ptp_egress_asymmetry_1_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_misc_control_reg_get(dev_id,
				phy_id, &ptp_misc_control_reg));
	asym_cf->eg_asym_en = ptp_misc_control_reg.bf.eg_asym_en;
	asym_cf->in_asym_en = ptp_misc_control_reg.bf.in_asym_en;

	SW_RTN_ON_ERROR(qca808x_ptp_ingress_asymmetry_0_reg_get(dev_id,
				phy_id, &ptp_ingress_asymmetry_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_ingress_asymmetry_1_reg_get(dev_id,
				phy_id, &ptp_ingress_asymmetry_1_reg));
	asym_cf->in_asym_value = (ptp_ingress_asymmetry_0_reg.bf.in_asym << 16) |
		ptp_ingress_asymmetry_1_reg.bf.in_asym;

	SW_RTN_ON_ERROR(qca808x_ptp_egress_asymmetry_0_reg_get(dev_id,
				phy_id, &ptp_egress_asymmetry_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_egress_asymmetry_1_reg_get(dev_id,
				phy_id, &ptp_egress_asymmetry_1_reg));
	asym_cf->eg_asym_value = (ptp_egress_asymmetry_0_reg.bf.eg_asym << 16) |
		ptp_egress_asymmetry_1_reg.bf.eg_asym;

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_output_waveform_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_output_waveform_t *waveform)
{
	union ptp_rtc_ext_conf_reg_u ptp_rtc_ext_conf_reg = {0};
	union ptp_freq_waveform_period_0_reg_u ptp_freq_waveform_period_0_reg = {0};
	union ptp_freq_waveform_period_1_reg_u ptp_freq_waveform_period_1_reg = {0};
	union ptp_freq_waveform_period_2_reg_u ptp_freq_waveform_period_2_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_get(dev_id,
				phy_id, &ptp_rtc_ext_conf_reg));
	if (waveform->waveform_type == FAL_WAVE_FREQ)
	{
		ptp_rtc_ext_conf_reg.bf.select_output_waveform =
			PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_FREQ;
	}
	else if (waveform->waveform_type == FAL_PULSE_10MS)
	{
		ptp_rtc_ext_conf_reg.bf.select_output_waveform =
			PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_PULSE_10MS;
	}
	else if (waveform->waveform_type == FAL_TRIGGER0_GPIO)
	{
		ptp_rtc_ext_conf_reg.bf.select_output_waveform =
			PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_TRIG0_GPIO;
	}
	else
	{
		ptp_rtc_ext_conf_reg.bf.select_output_waveform =
			PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_RXTS_VALID;
	}
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_set(dev_id, phy_id, &ptp_rtc_ext_conf_reg));

	ptp_freq_waveform_period_0_reg.bf.phase_ali = waveform->wave_align_pps_out_en;
	ptp_freq_waveform_period_0_reg.bf.wave_period = (waveform->wave_period >> 32) & 0x7fff;
	ptp_freq_waveform_period_1_reg.bf.wave_period = (waveform->wave_period >> 16) & 0xffff;
	ptp_freq_waveform_period_2_reg.bf.wave_period = waveform->wave_period & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_freq_waveform_period_0_reg_set(dev_id,
				phy_id, &ptp_freq_waveform_period_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_freq_waveform_period_1_reg_set(dev_id,
				phy_id, &ptp_freq_waveform_period_1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_freq_waveform_period_2_reg_set(dev_id,
				phy_id, &ptp_freq_waveform_period_2_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_output_waveform_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_output_waveform_t *waveform)
{
	union ptp_rtc_ext_conf_reg_u ptp_rtc_ext_conf_reg = {0};
	union ptp_freq_waveform_period_0_reg_u ptp_freq_waveform_period_0_reg = {0};
	union ptp_freq_waveform_period_1_reg_u ptp_freq_waveform_period_1_reg = {0};
	union ptp_freq_waveform_period_2_reg_u ptp_freq_waveform_period_2_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_get(dev_id,
				phy_id, &ptp_rtc_ext_conf_reg));
	if (ptp_rtc_ext_conf_reg.bf.select_output_waveform ==
			PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_FREQ)
	{
		waveform->waveform_type = FAL_WAVE_FREQ;
	}
	else if (ptp_rtc_ext_conf_reg.bf.select_output_waveform ==
			PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_PULSE_10MS)
	{
		waveform->waveform_type = FAL_PULSE_10MS;
	}
	else if (ptp_rtc_ext_conf_reg.bf.select_output_waveform ==
			PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_TRIG0_GPIO)
	{
		waveform->waveform_type = FAL_TRIGGER0_GPIO;
	}
	else
	{
		waveform->waveform_type = FAL_RX_PTP_STATE;
	}

	SW_RTN_ON_ERROR(qca808x_ptp_freq_waveform_period_0_reg_get(dev_id,
				phy_id, &ptp_freq_waveform_period_0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_freq_waveform_period_1_reg_get(dev_id,
				phy_id, &ptp_freq_waveform_period_1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_freq_waveform_period_2_reg_get(dev_id,
				phy_id, &ptp_freq_waveform_period_2_reg));
	waveform->wave_align_pps_out_en = ptp_freq_waveform_period_0_reg.bf.phase_ali;
	waveform->wave_period = ((a_int64_t)ptp_freq_waveform_period_0_reg.bf.wave_period << 32) |
		(ptp_freq_waveform_period_1_reg.bf.wave_period  << 16) |
		ptp_freq_waveform_period_2_reg.bf.wave_period;

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_rtc_time_snapshot_enable(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t status)
{
	union ptp_rtc_ext_conf_reg_u ptp_rtc_ext_conf_reg= {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &ptp_rtc_ext_conf_reg));
	ptp_rtc_ext_conf_reg.bf.rtc_read_mode = status;
	if (status == A_TRUE)
	{
		ptp_rtc_ext_conf_reg.bf.rtc_snapshot = PTP_REG_BIT_TRUE;
	}

	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_set(dev_id, phy_id, &ptp_rtc_ext_conf_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_rtc_time_snapshot_status_get(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t *status)
{
	union ptp_rtc_ext_conf_reg_u ptp_rtc_ext_conf_reg= {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &ptp_rtc_ext_conf_reg));
	*status = ptp_rtc_ext_conf_reg.bf.rtc_read_mode;

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_increment_sync_from_clock_enable(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t status)
{
	union ptp_rtc_ext_conf_reg_u ptp_rtc_ext_conf_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &ptp_rtc_ext_conf_reg));
	ptp_rtc_ext_conf_reg.bf.set_incval_mode = status;
	if (status == A_TRUE)
	{
		ptp_rtc_ext_conf_reg.bf.set_incval_valid = PTP_REG_BIT_TRUE;
	}
	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_set(dev_id, phy_id, &ptp_rtc_ext_conf_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_increment_sync_from_clock_status_get(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t *status)
{
	union ptp_rtc_ext_conf_reg_u ptp_rtc_ext_conf_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &ptp_rtc_ext_conf_reg));
	*status = ptp_rtc_ext_conf_reg.bf.set_incval_mode;

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_tod_uart_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_tod_uart_t *tod_uart)
{
	union ptp_baud_config_reg_u  ptp_baud_config_reg = {0};
	union ptp_uart_configuration_reg_u ptp_uart_configuration_reg = {0};
	union ptp_reset_buffer_reg_u ptp_reset_buffer_reg = {0};
	union ptp_tx_buffer_write_reg_u ptp_tx_buffer_write_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_baud_config_reg_get(dev_id, phy_id, &ptp_baud_config_reg));
	ptp_baud_config_reg.bf.baud_rate = tod_uart->baud_config;
	SW_RTN_ON_ERROR(qca808x_ptp_baud_config_reg_set(dev_id, phy_id, &ptp_baud_config_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_uart_configuration_reg_get(dev_id,
				phy_id, &ptp_uart_configuration_reg));
	ptp_uart_configuration_reg.bf.start_polarity = tod_uart->uart_config_bmp & 0x1;
	ptp_uart_configuration_reg.bf.msb_first =
	(tod_uart->uart_config_bmp >> PTP_UART_CONFIGURATION_REG_MSB_FIRST_OFFSET) & 0x1;
	ptp_uart_configuration_reg.bf.parity_en =
	(tod_uart->uart_config_bmp >> PTP_UART_CONFIGURATION_REG_PARITY_EN_OFFSET) & 0x1;
	ptp_uart_configuration_reg.bf.auto_tod_out_en =
	(tod_uart->uart_config_bmp >> PTP_UART_CONFIGURATION_REG_AUTO_TOD_OUT_EN_OFFSET) & 0x1;
	ptp_uart_configuration_reg.bf.auto_tod_in_en =
	(tod_uart->uart_config_bmp >> PTP_UART_CONFIGURATION_REG_AUTO_TOD_IN_EN_OFFSET) & 0x1;

	SW_RTN_ON_ERROR(qca808x_ptp_uart_configuration_reg_set(dev_id,
				phy_id, &ptp_uart_configuration_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_reset_buffer_reg_get(dev_id, phy_id, &ptp_reset_buffer_reg));
	ptp_reset_buffer_reg.bf.reset = tod_uart->reset_buf_en;
	SW_RTN_ON_ERROR(qca808x_ptp_reset_buffer_reg_set(dev_id, phy_id, &ptp_reset_buffer_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_buffer_write_reg_get(dev_id,
				phy_id, &ptp_tx_buffer_write_reg));
	ptp_tx_buffer_write_reg.bf.tx_buffer = tod_uart->tx_buf_value;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_buffer_write_reg_set(dev_id,
				phy_id, &ptp_tx_buffer_write_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_tod_uart_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_tod_uart_t *tod_uart)
{
	union ptp_baud_config_reg_u  ptp_baud_config_reg = {0};
	union ptp_uart_configuration_reg_u ptp_uart_configuration_reg = {0};
	union ptp_buffer_status_reg_u ptp_buffer_status_reg = {0};
	union ptp_tx_buffer_write_reg_u ptp_tx_buffer_write_reg = {0};
	union ptp_rx_buffer_read_reg_u ptp_rx_buffer_read_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_baud_config_reg_get(dev_id, phy_id, &ptp_baud_config_reg));
	tod_uart->baud_config = ptp_baud_config_reg.bf.baud_rate;

	SW_RTN_ON_ERROR(qca808x_ptp_uart_configuration_reg_get(dev_id,
				phy_id, &ptp_uart_configuration_reg));
	tod_uart->uart_config_bmp = ptp_uart_configuration_reg.bf.start_polarity |
		(ptp_uart_configuration_reg.bf.msb_first <<
		 PTP_UART_CONFIGURATION_REG_MSB_FIRST_OFFSET) |
		(ptp_uart_configuration_reg.bf.parity_en <<
		 PTP_UART_CONFIGURATION_REG_PARITY_EN_OFFSET) |
		(ptp_uart_configuration_reg.bf.auto_tod_out_en <<
		 PTP_UART_CONFIGURATION_REG_AUTO_TOD_OUT_EN_OFFSET) |
		(ptp_uart_configuration_reg.bf.auto_tod_in_en <<
		 PTP_UART_CONFIGURATION_REG_AUTO_TOD_IN_EN_OFFSET);

	/* reset buffer is self clearing, always read as 0 */
	tod_uart->reset_buf_en = 0;

	SW_RTN_ON_ERROR(qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &ptp_buffer_status_reg));
	tod_uart->buf_status_bmp = ptp_buffer_status_reg.bf.tx_buffer_almost_empty |
		(ptp_buffer_status_reg.bf.tx_buffer_almost_full <<
		 PTP_BUFFER_STATUS_REG_TX_BUFFER_ALMOST_FULL_OFFSET) |
		(ptp_buffer_status_reg.bf.tx_buffer_half_full <<
		 PTP_BUFFER_STATUS_REG_TX_BUFFER_HALF_FULL_OFFSET) |
		(ptp_buffer_status_reg.bf.tx_buffer_full <<
		 PTP_BUFFER_STATUS_REG_TX_BUFFER_FULL_OFFSET) |
		(ptp_buffer_status_reg.bf.rx_buffer_almost_empty <<
		 PTP_BUFFER_STATUS_REG_RX_BUFFER_ALMOST_EMPTY_OFFSET) |
		(ptp_buffer_status_reg.bf.rx_buffer_almost_full <<
		 PTP_BUFFER_STATUS_REG_RX_BUFFER_ALMOST_FULL_OFFSET) |
		(ptp_buffer_status_reg.bf.rx_buffer_half_full <<
		 PTP_BUFFER_STATUS_REG_RX_BUFFER_HALF_FULL_OFFSET) |
		(ptp_buffer_status_reg.bf.rx_buffer_full <<
		 PTP_BUFFER_STATUS_REG_RX_BUFFER_FULL_OFFSET) |
		(ptp_buffer_status_reg.bf.rx_buffer_data_present <<
		 PTP_BUFFER_STATUS_REG_RX_BUFFER_DATA_PRESENT_OFFSET);


	SW_RTN_ON_ERROR(qca808x_ptp_tx_buffer_write_reg_get(dev_id,
				phy_id, &ptp_tx_buffer_write_reg));
	tod_uart->tx_buf_value = ptp_tx_buffer_write_reg.bf.tx_buffer;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_buffer_read_reg_get(dev_id,
				phy_id, &ptp_rx_buffer_read_reg));
	tod_uart->rx_buf_value = ptp_rx_buffer_read_reg.bf.rx_data;

	return SW_OK;
}

sw_error_t
_qca808x_phy_ptp_enhanced_timestamp_engine_rx_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	union ptp_rx_com_ts_ctrl_reg_u ptp_rx_com_ts_ctrl_reg = {0};
	union ptp_rx_filt_mac_da0_reg_u ptp_rx_filt_mac_da0_reg = {0};
	union ptp_rx_filt_mac_da1_reg_u ptp_rx_filt_mac_da1_reg = {0};
	union ptp_rx_filt_mac_da2_reg_u ptp_rx_filt_mac_da2_reg = {0};
	union ptp_rx_filt_ipv4_da0_reg_u ptp_rx_filt_ipv4_da0_reg = {0};
	union ptp_rx_filt_ipv4_da1_reg_u ptp_rx_filt_ipv4_da1_reg = {0};
	union ptp_rx_filt_ipv6_da0_reg_u ptp_rx_filt_ipv6_da0_reg = {0};
	union ptp_rx_filt_ipv6_da1_reg_u ptp_rx_filt_ipv6_da1_reg = {0};
	union ptp_rx_filt_ipv6_da2_reg_u ptp_rx_filt_ipv6_da2_reg = {0};
	union ptp_rx_filt_ipv6_da3_reg_u ptp_rx_filt_ipv6_da3_reg = {0};
	union ptp_rx_filt_ipv6_da4_reg_u ptp_rx_filt_ipv6_da4_reg = {0};
	union ptp_rx_filt_ipv6_da5_reg_u ptp_rx_filt_ipv6_da5_reg = {0};
	union ptp_rx_filt_ipv6_da6_reg_u ptp_rx_filt_ipv6_da6_reg = {0};
	union ptp_rx_filt_ipv6_da7_reg_u ptp_rx_filt_ipv6_da7_reg = {0};
	union ptp_rx_filt_mac_lengthtype_reg_u ptp_rx_filt_mac_lengthtype_reg = {0};
	union ptp_rx_filt_layer4_protocol_reg_u ptp_rx_filt_layer4_protocol_reg = {0};
	union ptp_rx_filt_udp_port_reg_u ptp_rx_filt_udp_port_reg = {0};

	union ptp_loc_mac_addr_0_reg_u ptp_loc_mac_addr_0_reg = {0};
	union ptp_loc_mac_addr_1_reg_u ptp_loc_mac_addr_1_reg = {0};
	union ptp_loc_mac_addr_2_reg_u ptp_loc_mac_addr_2_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id,
				phy_id, &ptp_rx_com_ts_ctrl_reg));
	ptp_rx_com_ts_ctrl_reg.bf.filt_en = ts_engine->filt_en;
	ptp_rx_com_ts_ctrl_reg.bf.mac_lengthtype_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_MAC_LENGTHTYPE_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_rx_com_ts_ctrl_reg.bf.mac_da_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_MAC_DA_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_rx_com_ts_ctrl_reg.bf.mac_ptp_filt_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_MAC_PTP_FILT_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_rx_com_ts_ctrl_reg.bf.ipv4_layer4_protocol_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_IPV4_LAYER4_PROTOCOL_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_rx_com_ts_ctrl_reg.bf.ipv4_da_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_IPV4_DA_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_rx_com_ts_ctrl_reg.bf.ipv4_ptp_filt_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_IPV4_PTP_FILT_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_rx_com_ts_ctrl_reg.bf.ipv6_next_header_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_IPV6_NEXT_HEADER_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_rx_com_ts_ctrl_reg.bf.ipv6_da_filt_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_IPV6_DA_FILT_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_rx_com_ts_ctrl_reg.bf.ipv6_ptp_filt_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_IPV6_PTP_FILT_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_rx_com_ts_ctrl_reg.bf.udp_dport_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_UDP_DPORT_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_rx_com_ts_ctrl_reg.bf.udp_ptp_event_filt_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_UDP_PTP_EVENT_FILT_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_rx_com_ts_ctrl_reg.bf.y1731_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_Y1731_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_rx_com_ts_ctrl_reg.bf.y1731_insert_ts_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_Y1731_INSERT_TS_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_rx_com_ts_ctrl_reg.bf.y1731_da_chk_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_RX_COM_TS_CTRL_REG_Y1731_DA_CHK_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id,
				phy_id, &ptp_rx_com_ts_ctrl_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_mac_lengthtype_reg_get(dev_id,
				phy_id, &ptp_rx_filt_mac_lengthtype_reg));
	ptp_rx_filt_mac_lengthtype_reg.bf.length_type = ts_engine->eth_type;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_mac_lengthtype_reg_set(dev_id,
				phy_id, &ptp_rx_filt_mac_lengthtype_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_mac_da0_reg_get(dev_id,
				phy_id, &ptp_rx_filt_mac_da0_reg));
	ptp_rx_filt_mac_da0_reg.bf.mac_addr = (ts_engine->dmac_addr.uc[0] << 8) |
		ts_engine->dmac_addr.uc[1];
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_mac_da0_reg_set(dev_id,
				phy_id, &ptp_rx_filt_mac_da0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_mac_da1_reg_get(dev_id,
				phy_id, &ptp_rx_filt_mac_da1_reg));
	ptp_rx_filt_mac_da1_reg.bf.mac_addr = (ts_engine->dmac_addr.uc[2] << 8) |
		ts_engine->dmac_addr.uc[3];
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_mac_da1_reg_set(dev_id,
				phy_id, &ptp_rx_filt_mac_da1_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_mac_da2_reg_get(dev_id,
				phy_id, &ptp_rx_filt_mac_da2_reg));
	ptp_rx_filt_mac_da2_reg.bf.mac_addr = (ts_engine->dmac_addr.uc[4] << 8) |
		ts_engine->dmac_addr.uc[5];
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_mac_da2_reg_set(dev_id,
				phy_id, &ptp_rx_filt_mac_da2_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_layer4_protocol_reg_get(dev_id,
				phy_id, &ptp_rx_filt_layer4_protocol_reg));
	ptp_rx_filt_layer4_protocol_reg.bf.l4_protocol = ts_engine->ipv4_l4_proto;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_layer4_protocol_reg_set(dev_id,
				phy_id, &ptp_rx_filt_layer4_protocol_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv4_da0_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv4_da0_reg));
	ptp_rx_filt_ipv4_da0_reg.bf.ip_addr = ts_engine->ipv4_dip >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv4_da0_reg_set(dev_id,
				phy_id, &ptp_rx_filt_ipv4_da0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv4_da1_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv4_da1_reg));
	ptp_rx_filt_ipv4_da1_reg.bf.ip_addr = ts_engine->ipv4_dip & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv4_da1_reg_set(dev_id,
				phy_id, &ptp_rx_filt_ipv4_da1_reg));;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da0_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da0_reg));
	ptp_rx_filt_ipv6_da0_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[0] >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da0_reg_set(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da1_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da1_reg));
	ptp_rx_filt_ipv6_da1_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[0] & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da1_reg_set(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da1_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da2_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da2_reg));
	ptp_rx_filt_ipv6_da2_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[1] >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da2_reg_set(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da2_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da3_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da3_reg));
	ptp_rx_filt_ipv6_da3_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[1] & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da3_reg_set(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da3_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da4_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da4_reg));
	ptp_rx_filt_ipv6_da4_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[2] >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da4_reg_set(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da4_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da5_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da5_reg));
	ptp_rx_filt_ipv6_da5_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[2] & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da5_reg_set(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da5_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da6_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da6_reg));
	ptp_rx_filt_ipv6_da6_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[3] >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da6_reg_set(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da6_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da7_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da7_reg));
	ptp_rx_filt_ipv6_da7_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[3] & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da7_reg_set(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da7_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_udp_port_reg_get(dev_id,
				phy_id, &ptp_rx_filt_udp_port_reg));
	ptp_rx_filt_udp_port_reg.bf.udp_port = ts_engine->udp_dport;
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_udp_port_reg_set(dev_id,
				phy_id, &ptp_rx_filt_udp_port_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_0_reg_get(dev_id,
				phy_id, &ptp_loc_mac_addr_0_reg));
	ptp_loc_mac_addr_0_reg.bf.mac_addr = (ts_engine->y1731_mac_addr.uc[0] << 8) |
		ts_engine->y1731_mac_addr.uc[1];
	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_0_reg_set(dev_id,
				phy_id, &ptp_loc_mac_addr_0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_1_reg_get(dev_id,
				phy_id, &ptp_loc_mac_addr_1_reg));
	ptp_loc_mac_addr_1_reg.bf.mac_addr = (ts_engine->y1731_mac_addr.uc[2] << 8) |
		ts_engine->y1731_mac_addr.uc[3];
	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_1_reg_set(dev_id,
				phy_id, &ptp_loc_mac_addr_1_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_2_reg_get(dev_id,
				phy_id, &ptp_loc_mac_addr_2_reg));
	ptp_loc_mac_addr_2_reg.bf.mac_addr = (ts_engine->y1731_mac_addr.uc[4] << 8) |
		ts_engine->y1731_mac_addr.uc[5];
	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_2_reg_set(dev_id,
				phy_id, &ptp_loc_mac_addr_2_reg));

	return SW_OK;
}


sw_error_t
_qca808x_phy_ptp_enhanced_timestamp_engine_tx_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	union ptp_tx_com_ts_ctrl_reg_u ptp_tx_com_ts_ctrl_reg = {0};
	union ptp_tx_filt_mac_da0_reg_u ptp_tx_filt_mac_da0_reg = {0};
	union ptp_tx_filt_mac_da1_reg_u ptp_tx_filt_mac_da1_reg = {0};
	union ptp_tx_filt_mac_da2_reg_u ptp_tx_filt_mac_da2_reg = {0};
	union ptp_tx_filt_ipv4_da0_reg_u ptp_tx_filt_ipv4_da0_reg = {0};
	union ptp_tx_filt_ipv4_da1_reg_u ptp_tx_filt_ipv4_da1_reg = {0};
	union ptp_tx_filt_ipv6_da0_reg_u ptp_tx_filt_ipv6_da0_reg = {0};
	union ptp_tx_filt_ipv6_da1_reg_u ptp_tx_filt_ipv6_da1_reg = {0};
	union ptp_tx_filt_ipv6_da2_reg_u ptp_tx_filt_ipv6_da2_reg = {0};
	union ptp_tx_filt_ipv6_da3_reg_u ptp_tx_filt_ipv6_da3_reg = {0};
	union ptp_tx_filt_ipv6_da4_reg_u ptp_tx_filt_ipv6_da4_reg = {0};
	union ptp_tx_filt_ipv6_da5_reg_u ptp_tx_filt_ipv6_da5_reg = {0};
	union ptp_tx_filt_ipv6_da6_reg_u ptp_tx_filt_ipv6_da6_reg = {0};
	union ptp_tx_filt_ipv6_da7_reg_u ptp_tx_filt_ipv6_da7_reg = {0};
	union ptp_tx_filt_mac_lengthtype_reg_u ptp_tx_filt_mac_lengthtype_reg = {0};
	union ptp_tx_filt_layer4_protocol_reg_u ptp_tx_filt_layer4_protocol_reg = {0};
	union ptp_tx_filt_udp_port_reg_u ptp_tx_filt_udp_port_reg = {0};

	union ptp_loc_mac_addr_0_reg_u ptp_loc_mac_addr_0_reg = {0};
	union ptp_loc_mac_addr_1_reg_u ptp_loc_mac_addr_1_reg = {0};
	union ptp_loc_mac_addr_2_reg_u ptp_loc_mac_addr_2_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id,
				phy_id, &ptp_tx_com_ts_ctrl_reg));
	ptp_tx_com_ts_ctrl_reg.bf.filt_en = ts_engine->filt_en;
	ptp_tx_com_ts_ctrl_reg.bf.mac_lengthtype_en = ts_engine->enhance_ts_conf_bmp &
		PTP_REG_BIT_TRUE;
	ptp_tx_com_ts_ctrl_reg.bf.mac_da_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_TX_COM_TS_CTRL_REG_MAC_DA_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_tx_com_ts_ctrl_reg.bf.mac_ptp_filt_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_TX_COM_TS_CTRL_REG_MAC_PTP_FILT_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_tx_com_ts_ctrl_reg.bf.ipv4_layer4_protocol_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_TX_COM_TS_CTRL_REG_IPV4_LAYER4_PROTOCOL_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_tx_com_ts_ctrl_reg.bf.ipv4_da_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_TX_COM_TS_CTRL_REG_IPV4_DA_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_tx_com_ts_ctrl_reg.bf.ipv4_ptp_filt_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_TX_COM_TS_CTRL_REG_IPV4_PTP_FILT_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_tx_com_ts_ctrl_reg.bf.ipv6_next_header_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_TX_COM_TS_CTRL_REG_IPV6_NEXT_HEADER_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_tx_com_ts_ctrl_reg.bf.ipv6_da_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_TX_COM_TS_CTRL_REG_IPV6_DA_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_tx_com_ts_ctrl_reg.bf.ipv6_ptp_filt_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_TX_COM_TS_CTRL_REG_IPV6_PTP_FILT_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_tx_com_ts_ctrl_reg.bf.udp_dport_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_TX_COM_TS_CTRL_REG_UDP_DPORT_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_tx_com_ts_ctrl_reg.bf.udp_ptp_event_filt_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_TX_COM_TS_CTRL_REG_UDP_PTP_EVENT_FILT_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_tx_com_ts_ctrl_reg.bf.y1731_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_TX_COM_TS_CTRL_REG_Y1731_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_tx_com_ts_ctrl_reg.bf.y1731_insert_ts_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_TX_COM_TS_CTRL_REG_Y1731_INSERT_TS_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	ptp_tx_com_ts_ctrl_reg.bf.y1731_sa_chk_en =
	(ts_engine->enhance_ts_conf_bmp >> (PTP_TX_COM_TS_CTRL_REG_Y1731_SA_CHK_EN_OFFSET-1)
	 ) & PTP_REG_BIT_TRUE;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id,
				phy_id, &ptp_tx_com_ts_ctrl_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_mac_lengthtype_reg_get(dev_id,
				phy_id, &ptp_tx_filt_mac_lengthtype_reg));
	ptp_tx_filt_mac_lengthtype_reg.bf.length_type = ts_engine->eth_type;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_mac_lengthtype_reg_set(dev_id,
				phy_id, &ptp_tx_filt_mac_lengthtype_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_mac_da0_reg_get(dev_id,
				phy_id, &ptp_tx_filt_mac_da0_reg));
	ptp_tx_filt_mac_da0_reg.bf.mac_addr = (ts_engine->dmac_addr.uc[0] << 8) |
		ts_engine->dmac_addr.uc[1];
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_mac_da0_reg_set(dev_id,
				phy_id, &ptp_tx_filt_mac_da0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_mac_da1_reg_get(dev_id,
				phy_id, &ptp_tx_filt_mac_da1_reg));
	ptp_tx_filt_mac_da1_reg.bf.mac_addr = (ts_engine->dmac_addr.uc[2] << 8) |
		ts_engine->dmac_addr.uc[3];
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_mac_da1_reg_set(dev_id,
				phy_id, &ptp_tx_filt_mac_da1_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_mac_da2_reg_get(dev_id,
				phy_id, &ptp_tx_filt_mac_da2_reg));
	ptp_tx_filt_mac_da2_reg.bf.mac_addr = (ts_engine->dmac_addr.uc[4] << 8) |
		ts_engine->dmac_addr.uc[5];
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_mac_da2_reg_set(dev_id,
				phy_id, &ptp_tx_filt_mac_da2_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_layer4_protocol_reg_get(dev_id,
				phy_id, &ptp_tx_filt_layer4_protocol_reg));
	ptp_tx_filt_layer4_protocol_reg.bf.l4_protocol = ts_engine->ipv4_l4_proto;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_layer4_protocol_reg_set(dev_id,
				phy_id, &ptp_tx_filt_layer4_protocol_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv4_da0_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv4_da0_reg));
	ptp_tx_filt_ipv4_da0_reg.bf.ip_addr = ts_engine->ipv4_dip >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv4_da0_reg_set(dev_id,
				phy_id, &ptp_tx_filt_ipv4_da0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv4_da1_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv4_da1_reg));
	ptp_tx_filt_ipv4_da1_reg.bf.ip_addr = ts_engine->ipv4_dip & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv4_da1_reg_set(dev_id,
				phy_id, &ptp_tx_filt_ipv4_da1_reg));;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da0_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da0_reg));
	ptp_tx_filt_ipv6_da0_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[0] >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da0_reg_set(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da1_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da1_reg));
	ptp_tx_filt_ipv6_da1_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[0] & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da1_reg_set(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da1_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da2_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da2_reg));
	ptp_tx_filt_ipv6_da2_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[1] >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da2_reg_set(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da2_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da3_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da3_reg));
	ptp_tx_filt_ipv6_da3_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[1] & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da3_reg_set(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da3_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da4_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da4_reg));
	ptp_tx_filt_ipv6_da4_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[2] >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da4_reg_set(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da4_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da5_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da5_reg));
	ptp_tx_filt_ipv6_da5_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[2] & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da5_reg_set(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da5_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da6_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da6_reg));
	ptp_tx_filt_ipv6_da6_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[3] >> 16;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da6_reg_set(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da6_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da7_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da7_reg));
	ptp_tx_filt_ipv6_da7_reg.bf.ip_addr = ts_engine->ipv6_dip.ul[3] & 0xffff;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da7_reg_set(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da7_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_udp_port_reg_get(dev_id,
				phy_id, &ptp_tx_filt_udp_port_reg));
	ptp_tx_filt_udp_port_reg.bf.udp_port = ts_engine->udp_dport;
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_udp_port_reg_set(dev_id,
				phy_id, &ptp_tx_filt_udp_port_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_0_reg_get(dev_id,
				phy_id, &ptp_loc_mac_addr_0_reg));
	ptp_loc_mac_addr_0_reg.bf.mac_addr = (ts_engine->y1731_mac_addr.uc[0] << 8) |
		ts_engine->y1731_mac_addr.uc[1];
	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_0_reg_set(dev_id,
				phy_id, &ptp_loc_mac_addr_0_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_1_reg_get(dev_id,
				phy_id, &ptp_loc_mac_addr_1_reg));
	ptp_loc_mac_addr_1_reg.bf.mac_addr = (ts_engine->y1731_mac_addr.uc[2] << 8) |
		ts_engine->y1731_mac_addr.uc[3];
	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_1_reg_set(dev_id,
				phy_id, &ptp_loc_mac_addr_1_reg));

	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_2_reg_get(dev_id,
				phy_id, &ptp_loc_mac_addr_2_reg));
	ptp_loc_mac_addr_2_reg.bf.mac_addr = (ts_engine->y1731_mac_addr.uc[4] << 8) |
		ts_engine->y1731_mac_addr.uc[5];
	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_2_reg_set(dev_id,
				phy_id, &ptp_loc_mac_addr_2_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_enhanced_timestamp_engine_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	if (direction == FAL_RX_DIRECTION)
	{
		return _qca808x_phy_ptp_enhanced_timestamp_engine_rx_set(dev_id, phy_id, ts_engine);
	}
	else
	{
		return _qca808x_phy_ptp_enhanced_timestamp_engine_tx_set(dev_id, phy_id, ts_engine);
	}

	return SW_BAD_PARAM;
}

sw_error_t
_qca808x_phy_ptp_enhanced_timestamp_engine_rx_com_ts_get(a_uint32_t dev_id, a_uint32_t phy_id,
		fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	sw_error_t ret = SW_OK;

	union ptp_rx_com_timestamp0_reg_u ptp_rx_com_timestamp0_reg = {0};
	union ptp_rx_com_timestamp1_reg_u ptp_rx_com_timestamp1_reg = {0};
	union ptp_rx_com_timestamp2_reg_u ptp_rx_com_timestamp2_reg = {0};
	union ptp_rx_com_timestamp3_reg_u ptp_rx_com_timestamp3_reg = {0};
	union ptp_rx_com_timestamp4_reg_u ptp_rx_com_timestamp4_reg = {0};
	union ptp_rx_com_frac_nano_reg_u ptp_rx_com_frac_nano_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_timestamp0_reg_get(dev_id,
				phy_id, &ptp_rx_com_timestamp0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_timestamp1_reg_get(dev_id,
				phy_id, &ptp_rx_com_timestamp1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_timestamp2_reg_get(dev_id,
				phy_id, &ptp_rx_com_timestamp2_reg));
	ts_engine->timestamp.seconds =
		((a_int64_t)ptp_rx_com_timestamp0_reg.bf.com_ts << 32) |
		(ptp_rx_com_timestamp1_reg.bf.com_ts << 16) | ptp_rx_com_timestamp2_reg.bf.com_ts;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_timestamp3_reg_get(dev_id,
				phy_id, &ptp_rx_com_timestamp3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_timestamp4_reg_get(dev_id,
				phy_id, &ptp_rx_com_timestamp4_reg));
	ts_engine->timestamp.nanoseconds = (ptp_rx_com_timestamp3_reg.bf.com_ts << 16) |
		ptp_rx_com_timestamp4_reg.bf.com_ts;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_frac_nano_reg_get(dev_id,
				phy_id, &ptp_rx_com_frac_nano_reg));
	ts_engine->timestamp.fracnanoseconds = ptp_rx_com_frac_nano_reg.bf.frac_nano;

	return ret;
}

sw_error_t
_qca808x_phy_ptp_enhanced_timestamp_engine_rx_com_ts_pre_get(a_uint32_t dev_id, a_uint32_t phy_id,
		fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	sw_error_t ret = SW_OK;
	union ptp_rx_com_timestamp_pre0_reg_u ptp_rx_com_timestamp_pre0_reg = {0};
	union ptp_rx_com_timestamp_pre1_reg_u ptp_rx_com_timestamp_pre1_reg = {0};
	union ptp_rx_com_timestamp_pre2_reg_u ptp_rx_com_timestamp_pre2_reg = {0};
	union ptp_rx_com_timestamp_pre3_reg_u ptp_rx_com_timestamp_pre3_reg = {0};
	union ptp_rx_com_timestamp_pre4_reg_u ptp_rx_com_timestamp_pre4_reg = {0};
	union ptp_rx_com_frac_nano_pre_reg_u ptp_rx_com_frac_nano_pre_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_timestamp_pre0_reg_get(dev_id,
				phy_id, &ptp_rx_com_timestamp_pre0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_timestamp_pre1_reg_get(dev_id,
				phy_id, &ptp_rx_com_timestamp_pre1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_timestamp_pre2_reg_get(dev_id,
				phy_id, &ptp_rx_com_timestamp_pre2_reg));
	ts_engine->timestamp_pre.seconds =
		((a_int64_t)ptp_rx_com_timestamp_pre0_reg.bf.com_ts_pre << 32) |
		(ptp_rx_com_timestamp_pre1_reg.bf.com_ts_pre << 16) |
		ptp_rx_com_timestamp_pre2_reg.bf.com_ts_pre;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_timestamp_pre3_reg_get(dev_id,
				phy_id, &ptp_rx_com_timestamp_pre3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_timestamp_pre4_reg_get(dev_id,
				phy_id, &ptp_rx_com_timestamp_pre4_reg));
	ts_engine->timestamp_pre.nanoseconds =
		(ptp_rx_com_timestamp_pre3_reg.bf.com_ts_pre << 16) |
		ptp_rx_com_timestamp_pre4_reg.bf.com_ts_pre;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_frac_nano_pre_reg_get(dev_id,
				phy_id, &ptp_rx_com_frac_nano_pre_reg));
	ts_engine->timestamp_pre.fracnanoseconds =
		ptp_rx_com_frac_nano_pre_reg.bf.frac_nano_pre;

	return ret;
}

sw_error_t
_qca808x_phy_ptp_enhanced_timestamp_engine_rx_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	sw_error_t ret = SW_OK;
	union ptp_rx_com_ts_ctrl_reg_u ctrl_reg = {0};
	union ptp_rx_filt_mac_da0_reg_u ptp_rx_filt_mac_da0_reg = {0};
	union ptp_rx_filt_mac_da1_reg_u ptp_rx_filt_mac_da1_reg = {0};
	union ptp_rx_filt_mac_da2_reg_u ptp_rx_filt_mac_da2_reg = {0};
	union ptp_rx_filt_ipv4_da0_reg_u ptp_rx_filt_ipv4_da0_reg = {0};
	union ptp_rx_filt_ipv4_da1_reg_u ptp_rx_filt_ipv4_da1_reg = {0};
	union ptp_rx_filt_ipv6_da0_reg_u ptp_rx_filt_ipv6_da0_reg = {0};
	union ptp_rx_filt_ipv6_da1_reg_u ptp_rx_filt_ipv6_da1_reg = {0};
	union ptp_rx_filt_ipv6_da2_reg_u ptp_rx_filt_ipv6_da2_reg = {0};
	union ptp_rx_filt_ipv6_da3_reg_u ptp_rx_filt_ipv6_da3_reg = {0};
	union ptp_rx_filt_ipv6_da4_reg_u ptp_rx_filt_ipv6_da4_reg = {0};
	union ptp_rx_filt_ipv6_da5_reg_u ptp_rx_filt_ipv6_da5_reg = {0};
	union ptp_rx_filt_ipv6_da6_reg_u ptp_rx_filt_ipv6_da6_reg = {0};
	union ptp_rx_filt_ipv6_da7_reg_u ptp_rx_filt_ipv6_da7_reg = {0};
	union ptp_rx_filt_mac_lengthtype_reg_u ptp_rx_filt_mac_lengthtype_reg = {0};
	union ptp_rx_filt_layer4_protocol_reg_u ptp_rx_filt_layer4_protocol_reg = {0};
	union ptp_rx_filt_udp_port_reg_u ptp_rx_filt_udp_port_reg = {0};
	union ptp_rx_com_ts_status_reg_u ptp_rx_com_ts_status_reg = {0};
	union ptp_rx_com_ts_status_pre_reg_u ptp_rx_com_ts_status_pre_reg = {0};
	union ptp_rx_y1731_identify_reg_u ptp_rx_y1731_identify_reg = {0};
	union ptp_rx_y1731_identify_pre_reg_u ptp_rx_y1731_identify_pre_reg = {0};

	union ptp_loc_mac_addr_0_reg_u ptp_loc_mac_addr_0_reg = {0};
	union ptp_loc_mac_addr_1_reg_u ptp_loc_mac_addr_1_reg = {0};
	union ptp_loc_mac_addr_2_reg_u ptp_loc_mac_addr_2_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id,
				phy_id, &ctrl_reg));
	ts_engine->filt_en = ctrl_reg.bf.filt_en;
	ts_engine->enhance_ts_conf_bmp = ctrl_reg.bf.mac_lengthtype_en |
	(ctrl_reg.bf.mac_da_en << (PTP_RX_COM_TS_CTRL_REG_MAC_DA_EN_OFFSET-1)) |
	(ctrl_reg.bf.mac_ptp_filt_en << (PTP_RX_COM_TS_CTRL_REG_MAC_PTP_FILT_EN_OFFSET-1)) |
	(ctrl_reg.bf.
	 ipv4_layer4_protocol_en << (PTP_RX_COM_TS_CTRL_REG_IPV4_LAYER4_PROTOCOL_EN_OFFSET-1)) |
	(ctrl_reg.bf.ipv4_da_en << (PTP_RX_COM_TS_CTRL_REG_IPV4_DA_EN_OFFSET-1)) |
	(ctrl_reg.bf.ipv4_ptp_filt_en << (PTP_RX_COM_TS_CTRL_REG_IPV4_PTP_FILT_EN_OFFSET-1)) |
	(ctrl_reg.bf.ipv6_next_header_en << (PTP_RX_COM_TS_CTRL_REG_IPV6_NEXT_HEADER_EN_OFFSET-1)) |
	(ctrl_reg.bf.ipv6_da_filt_en << (PTP_RX_COM_TS_CTRL_REG_IPV6_DA_FILT_EN_OFFSET-1)) |
	(ctrl_reg.bf.ipv6_ptp_filt_en << (PTP_RX_COM_TS_CTRL_REG_IPV6_PTP_FILT_EN_OFFSET-1)) |
	(ctrl_reg.bf.udp_dport_en << (PTP_RX_COM_TS_CTRL_REG_UDP_DPORT_EN_OFFSET-1)) |
	(ctrl_reg.bf.
	 udp_ptp_event_filt_en << (PTP_RX_COM_TS_CTRL_REG_UDP_PTP_EVENT_FILT_EN_OFFSET-1)) |
	(ctrl_reg.bf.y1731_en << (PTP_RX_COM_TS_CTRL_REG_Y1731_EN_OFFSET-1)) |
	(ctrl_reg.bf.y1731_insert_ts_en << (PTP_RX_COM_TS_CTRL_REG_Y1731_INSERT_TS_EN_OFFSET-1)) |
	(ctrl_reg.bf.y1731_da_chk_en << (PTP_RX_COM_TS_CTRL_REG_Y1731_DA_CHK_EN_OFFSET-1));

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_mac_lengthtype_reg_get(dev_id,
				phy_id, &ptp_rx_filt_mac_lengthtype_reg));
	ts_engine->eth_type = ptp_rx_filt_mac_lengthtype_reg.bf.length_type;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_mac_da0_reg_get(dev_id,
				phy_id, &ptp_rx_filt_mac_da0_reg));
	ts_engine->dmac_addr.uc[0] = ptp_rx_filt_mac_da0_reg.bf.mac_addr >> 8;
	ts_engine->dmac_addr.uc[1] = ptp_rx_filt_mac_da0_reg.bf.mac_addr & 0xff;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_mac_da1_reg_get(dev_id,
				phy_id, &ptp_rx_filt_mac_da1_reg));
	ts_engine->dmac_addr.uc[2] = ptp_rx_filt_mac_da1_reg.bf.mac_addr >> 8;
	ts_engine->dmac_addr.uc[3] = ptp_rx_filt_mac_da1_reg.bf.mac_addr & 0xff;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_mac_da2_reg_get(dev_id,
				phy_id, &ptp_rx_filt_mac_da2_reg));
	ts_engine->dmac_addr.uc[4] = ptp_rx_filt_mac_da2_reg.bf.mac_addr >> 8;
	ts_engine->dmac_addr.uc[5] = ptp_rx_filt_mac_da2_reg.bf.mac_addr & 0xff;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_layer4_protocol_reg_get(dev_id,
				phy_id, &ptp_rx_filt_layer4_protocol_reg));
	ts_engine->ipv4_l4_proto = ptp_rx_filt_layer4_protocol_reg.bf.l4_protocol;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv4_da0_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv4_da0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv4_da1_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv4_da1_reg));
	ts_engine->ipv4_dip = (ptp_rx_filt_ipv4_da0_reg.bf.ip_addr << 16) |
		ptp_rx_filt_ipv4_da1_reg.bf.ip_addr;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da0_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da1_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da1_reg));
	ts_engine->ipv6_dip.ul[0] = (ptp_rx_filt_ipv6_da0_reg.bf.ip_addr << 16) |
		ptp_rx_filt_ipv6_da1_reg.bf.ip_addr;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da2_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da3_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da3_reg));
	ts_engine->ipv6_dip.ul[1] = (ptp_rx_filt_ipv6_da2_reg.bf.ip_addr << 16) |
		ptp_rx_filt_ipv6_da3_reg.bf.ip_addr;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da4_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da4_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da5_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da5_reg));
	ts_engine->ipv6_dip.ul[2] = (ptp_rx_filt_ipv6_da4_reg.bf.ip_addr << 16) |
		ptp_rx_filt_ipv6_da5_reg.bf.ip_addr;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da6_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da6_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_ipv6_da7_reg_get(dev_id,
				phy_id, &ptp_rx_filt_ipv6_da7_reg));
	ts_engine->ipv6_dip.ul[3] = (ptp_rx_filt_ipv6_da6_reg.bf.ip_addr << 16) |
		ptp_rx_filt_ipv6_da7_reg.bf.ip_addr;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_filt_udp_port_reg_get(dev_id,
				phy_id, &ptp_rx_filt_udp_port_reg));
	ts_engine->udp_dport = ptp_rx_filt_udp_port_reg.bf.udp_port;

	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_0_reg_get(dev_id,
				phy_id, &ptp_loc_mac_addr_0_reg));
	ts_engine->y1731_mac_addr.uc[0] = ptp_loc_mac_addr_0_reg.bf.mac_addr >> 8;
	ts_engine->y1731_mac_addr.uc[1] = ptp_loc_mac_addr_0_reg.bf.mac_addr & 0xff;

	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_1_reg_get(dev_id,
				phy_id, &ptp_loc_mac_addr_1_reg));
	ts_engine->y1731_mac_addr.uc[2] = ptp_loc_mac_addr_1_reg.bf.mac_addr >> 8;
	ts_engine->y1731_mac_addr.uc[3] = ptp_loc_mac_addr_1_reg.bf.mac_addr & 0xff;

	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_2_reg_get(dev_id,
				phy_id, &ptp_loc_mac_addr_2_reg));
	ts_engine->y1731_mac_addr.uc[4] = ptp_loc_mac_addr_2_reg.bf.mac_addr >> 8;
	ts_engine->y1731_mac_addr.uc[5] = ptp_loc_mac_addr_2_reg.bf.mac_addr & 0xff;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_ts_status_reg_get(dev_id,
				phy_id, &ptp_rx_com_ts_status_reg));
	ts_engine->enhance_ts_status_bmp = ptp_rx_com_ts_status_reg.bf.mac_lengthtype |
		(ptp_rx_com_ts_status_reg.bf.mac_da <<
		 PTP_RX_COM_TS_STATUS_REG_MAC_DA_OFFSET) |
		(ptp_rx_com_ts_status_reg.bf.mac_ptp_prim_addr <<
		 PTP_RX_COM_TS_STATUS_REG_MAC_PTP_PRIM_ADDR_OFFSET) |
		(ptp_rx_com_ts_status_reg.bf.mac_ptp_pdelay_addr <<
		 PTP_RX_COM_TS_STATUS_REG_MAC_PTP_PDELAY_ADDR_OFFSET) |
		(ptp_rx_com_ts_status_reg.bf.ipv4_layer4_protocol <<
		 PTP_RX_COM_TS_STATUS_REG_IPV4_LAYER4_PROTOCOL_OFFSET) |
		(ptp_rx_com_ts_status_reg.bf.ipv4_da <<
		 PTP_RX_COM_TS_STATUS_REG_IPV4_DA_OFFSET) |
		(ptp_rx_com_ts_status_reg.bf.ipv4_ptp_prim_addr <<
		 PTP_RX_COM_TS_STATUS_REG_IPV4_PTP_PRIM_ADDR_OFFSET) |
		(ptp_rx_com_ts_status_reg.bf.ipv4_ptp_pdelay_addr <<
		 PTP_RX_COM_TS_STATUS_REG_IPV4_PTP_PDELAY_ADDR_OFFSET) |
		(ptp_rx_com_ts_status_reg.bf.ipv6_next_header <<
		 PTP_RX_COM_TS_STATUS_REG_IPV6_NEXT_HEADER_OFFSET) |
		(ptp_rx_com_ts_status_reg.bf.ipv6_da <<
		 PTP_RX_COM_TS_STATUS_REG_IPV6_DA_OFFSET) |
		(ptp_rx_com_ts_status_reg.bf.ipv6_ptp_prim_addr <<
		 PTP_RX_COM_TS_STATUS_REG_IPV6_PTP_PRIM_ADDR_OFFSET) |
		(ptp_rx_com_ts_status_reg.bf.ipv6_ptp_pdelay_addr <<
		 PTP_RX_COM_TS_STATUS_REG_IPV6_PTP_PDELAY_ADDR_OFFSET) |
		(ptp_rx_com_ts_status_reg.bf.udp_dport <<
		 PTP_RX_COM_TS_STATUS_REG_UDP_DPORT_OFFSET) |
		(ptp_rx_com_ts_status_reg.bf.udp_ptp_event_dport <<
		 PTP_RX_COM_TS_STATUS_REG_UDP_PTP_EVENT_DPORT_OFFSET) |
		(ptp_rx_com_ts_status_reg.bf.y1731_mach <<
		 PTP_RX_COM_TS_STATUS_REG_Y1731_MACH_OFFSET);

	SW_RTN_ON_ERROR(qca808x_ptp_rx_com_ts_status_pre_reg_get(dev_id,
				phy_id, &ptp_rx_com_ts_status_pre_reg));
	ts_engine->enhance_ts_status_pre_bmp = ptp_rx_com_ts_status_pre_reg.bf.mac_lengthtype |
		(ptp_rx_com_ts_status_pre_reg.bf.mac_da <<
		 PTP_RX_COM_TS_STATUS_REG_MAC_DA_OFFSET) |
		(ptp_rx_com_ts_status_pre_reg.bf.mac_ptp_prim_addr <<
		 PTP_RX_COM_TS_STATUS_REG_MAC_PTP_PRIM_ADDR_OFFSET) |
		(ptp_rx_com_ts_status_pre_reg.bf.mac_ptp_pdelay_addr <<
		 PTP_RX_COM_TS_STATUS_REG_MAC_PTP_PDELAY_ADDR_OFFSET) |
		(ptp_rx_com_ts_status_pre_reg.bf.ipv4_layer4_protocol <<
		 PTP_RX_COM_TS_STATUS_REG_IPV4_LAYER4_PROTOCOL_OFFSET) |
		(ptp_rx_com_ts_status_pre_reg.bf.ipv4_da <<
		 PTP_RX_COM_TS_STATUS_REG_IPV4_DA_OFFSET) |
		(ptp_rx_com_ts_status_pre_reg.bf.ipv4_ptp_prim_addr <<
		 PTP_RX_COM_TS_STATUS_REG_IPV4_PTP_PRIM_ADDR_OFFSET) |
		(ptp_rx_com_ts_status_pre_reg.bf.ipv4_ptp_pdelay_addr <<
		 PTP_RX_COM_TS_STATUS_REG_IPV4_PTP_PDELAY_ADDR_OFFSET) |
		(ptp_rx_com_ts_status_pre_reg.bf.ipv6_next_header <<
		 PTP_RX_COM_TS_STATUS_REG_IPV6_NEXT_HEADER_OFFSET) |
		(ptp_rx_com_ts_status_pre_reg.bf.ipv6_da <<
		 PTP_RX_COM_TS_STATUS_REG_IPV6_DA_OFFSET) |
		(ptp_rx_com_ts_status_pre_reg.bf.ipv6_ptp_prim_addr <<
		 PTP_RX_COM_TS_STATUS_REG_IPV6_PTP_PRIM_ADDR_OFFSET) |
		(ptp_rx_com_ts_status_pre_reg.bf.ipv6_ptp_pdelay_addr <<
		 PTP_RX_COM_TS_STATUS_REG_IPV6_PTP_PDELAY_ADDR_OFFSET) |
		(ptp_rx_com_ts_status_pre_reg.bf.udp_dport <<
		 PTP_RX_COM_TS_STATUS_REG_UDP_DPORT_OFFSET) |
		(ptp_rx_com_ts_status_pre_reg.bf.udp_ptp_event_dport <<
		 PTP_RX_COM_TS_STATUS_REG_UDP_PTP_EVENT_DPORT_OFFSET) |
		(ptp_rx_com_ts_status_pre_reg.bf.y1731_mach <<
		 PTP_RX_COM_TS_STATUS_REG_Y1731_MACH_OFFSET);

	SW_RTN_ON_ERROR(qca808x_ptp_rx_y1731_identify_reg_get(dev_id,
				phy_id, &ptp_rx_y1731_identify_reg));
	ts_engine->y1731_identity = ptp_rx_y1731_identify_reg.bf.identify;

	SW_RTN_ON_ERROR(qca808x_ptp_rx_y1731_identify_pre_reg_get(dev_id,
				phy_id, &ptp_rx_y1731_identify_pre_reg));
	ts_engine->y1731_identity_pre = ptp_rx_y1731_identify_pre_reg.bf.identify_pre;

	ret = _qca808x_phy_ptp_enhanced_timestamp_engine_rx_com_ts_get(dev_id, phy_id,
			ts_engine);
	if (ret != SW_OK)
	{
		return ret;
	}

	ret = _qca808x_phy_ptp_enhanced_timestamp_engine_rx_com_ts_pre_get(dev_id, phy_id,
			ts_engine);

	return ret;
}

sw_error_t
_qca808x_phy_ptp_enhanced_timestamp_engine_tx_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	union ptp_tx_com_ts_ctrl_reg_u ctrl_reg = {0};
	union ptp_tx_filt_mac_da0_reg_u ptp_tx_filt_mac_da0_reg = {0};
	union ptp_tx_filt_mac_da1_reg_u ptp_tx_filt_mac_da1_reg = {0};
	union ptp_tx_filt_mac_da2_reg_u ptp_tx_filt_mac_da2_reg = {0};
	union ptp_tx_filt_ipv4_da0_reg_u ptp_tx_filt_ipv4_da0_reg = {0};
	union ptp_tx_filt_ipv4_da1_reg_u ptp_tx_filt_ipv4_da1_reg = {0};
	union ptp_tx_filt_ipv6_da0_reg_u ptp_tx_filt_ipv6_da0_reg = {0};
	union ptp_tx_filt_ipv6_da1_reg_u ptp_tx_filt_ipv6_da1_reg = {0};
	union ptp_tx_filt_ipv6_da2_reg_u ptp_tx_filt_ipv6_da2_reg = {0};
	union ptp_tx_filt_ipv6_da3_reg_u ptp_tx_filt_ipv6_da3_reg = {0};
	union ptp_tx_filt_ipv6_da4_reg_u ptp_tx_filt_ipv6_da4_reg = {0};
	union ptp_tx_filt_ipv6_da5_reg_u ptp_tx_filt_ipv6_da5_reg = {0};
	union ptp_tx_filt_ipv6_da6_reg_u ptp_tx_filt_ipv6_da6_reg = {0};
	union ptp_tx_filt_ipv6_da7_reg_u ptp_tx_filt_ipv6_da7_reg = {0};
	union ptp_tx_filt_mac_lengthtype_reg_u ptp_tx_filt_mac_lengthtype_reg = {0};
	union ptp_tx_filt_layer4_protocol_reg_u ptp_tx_filt_layer4_protocol_reg = {0};
	union ptp_tx_filt_udp_port_reg_u ptp_tx_filt_udp_port_reg = {0};
	union ptp_tx_com_ts_status_reg_u ptp_tx_com_ts_status_reg = {0};
	union ptp_tx_com_timestamp0_reg_u ptp_tx_com_timestamp0_reg = {0};
	union ptp_tx_com_timestamp1_reg_u ptp_tx_com_timestamp1_reg = {0};
	union ptp_tx_com_timestamp2_reg_u ptp_tx_com_timestamp2_reg = {0};
	union ptp_tx_com_timestamp3_reg_u ptp_tx_com_timestamp3_reg = {0};
	union ptp_tx_com_timestamp4_reg_u ptp_tx_com_timestamp4_reg = {0};
	union ptp_tx_com_frac_nano_reg_u ptp_tx_com_frac_nano_reg = {0};
	union ptp_tx_y1731_identify_reg_u ptp_tx_y1731_identify_reg = {0};

	union ptp_loc_mac_addr_0_reg_u ptp_loc_mac_addr_0_reg = {0};
	union ptp_loc_mac_addr_1_reg_u ptp_loc_mac_addr_1_reg = {0};
	union ptp_loc_mac_addr_2_reg_u ptp_loc_mac_addr_2_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id,
				phy_id, &ctrl_reg));
	ts_engine->filt_en = ctrl_reg.bf.filt_en;
	ts_engine->enhance_ts_conf_bmp = ctrl_reg.bf.mac_lengthtype_en |
	(ctrl_reg.bf.mac_da_en << (PTP_TX_COM_TS_CTRL_REG_MAC_DA_EN_OFFSET-1)) |
	(ctrl_reg.bf.mac_ptp_filt_en << (PTP_TX_COM_TS_CTRL_REG_MAC_PTP_FILT_EN_OFFSET-1)) |
	(ctrl_reg.bf.
	 ipv4_layer4_protocol_en << (PTP_TX_COM_TS_CTRL_REG_IPV4_LAYER4_PROTOCOL_EN_OFFSET-1)) |
	(ctrl_reg.bf.ipv4_da_en << (PTP_TX_COM_TS_CTRL_REG_IPV4_DA_EN_OFFSET-1)) |
	(ctrl_reg.bf.ipv4_ptp_filt_en << (PTP_TX_COM_TS_CTRL_REG_IPV4_PTP_FILT_EN_OFFSET-1)) |
	(ctrl_reg.bf.ipv6_next_header_en << (PTP_TX_COM_TS_CTRL_REG_IPV6_NEXT_HEADER_EN_OFFSET-1)) |
	(ctrl_reg.bf.ipv6_da_en << (PTP_TX_COM_TS_CTRL_REG_IPV6_DA_EN_OFFSET-1)) |
	(ctrl_reg.bf.ipv6_ptp_filt_en << (PTP_TX_COM_TS_CTRL_REG_IPV6_PTP_FILT_EN_OFFSET-1)) |
	(ctrl_reg.bf.udp_dport_en << (PTP_TX_COM_TS_CTRL_REG_UDP_DPORT_EN_OFFSET-1)) |
	(ctrl_reg.bf.
	 udp_ptp_event_filt_en << (PTP_TX_COM_TS_CTRL_REG_UDP_PTP_EVENT_FILT_EN_OFFSET-1)) |
	(ctrl_reg.bf.y1731_en << (PTP_TX_COM_TS_CTRL_REG_Y1731_EN_OFFSET-1)) |
	(ctrl_reg.bf.y1731_insert_ts_en << (PTP_TX_COM_TS_CTRL_REG_Y1731_INSERT_TS_EN_OFFSET-1)) |
	(ctrl_reg.bf.y1731_sa_chk_en << (PTP_TX_COM_TS_CTRL_REG_Y1731_SA_CHK_EN_OFFSET-1));

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_mac_lengthtype_reg_get(dev_id,
				phy_id, &ptp_tx_filt_mac_lengthtype_reg));
	ts_engine->eth_type = ptp_tx_filt_mac_lengthtype_reg.bf.length_type;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_mac_da0_reg_get(dev_id,
				phy_id, &ptp_tx_filt_mac_da0_reg));
	ts_engine->dmac_addr.uc[0] = ptp_tx_filt_mac_da0_reg.bf.mac_addr >> 8;
	ts_engine->dmac_addr.uc[1] = ptp_tx_filt_mac_da0_reg.bf.mac_addr & 0xff;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_mac_da1_reg_get(dev_id,
				phy_id, &ptp_tx_filt_mac_da1_reg));
	ts_engine->dmac_addr.uc[2] = ptp_tx_filt_mac_da1_reg.bf.mac_addr >> 8;
	ts_engine->dmac_addr.uc[3] = ptp_tx_filt_mac_da1_reg.bf.mac_addr & 0xff;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_mac_da2_reg_get(dev_id,
				phy_id, &ptp_tx_filt_mac_da2_reg));
	ts_engine->dmac_addr.uc[4] = ptp_tx_filt_mac_da2_reg.bf.mac_addr >> 8;
	ts_engine->dmac_addr.uc[5] = ptp_tx_filt_mac_da2_reg.bf.mac_addr & 0xff;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_layer4_protocol_reg_get(dev_id,
				phy_id, &ptp_tx_filt_layer4_protocol_reg));
	ts_engine->ipv4_l4_proto = ptp_tx_filt_layer4_protocol_reg.bf.l4_protocol;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv4_da0_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv4_da0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv4_da1_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv4_da1_reg));
	ts_engine->ipv4_dip = (ptp_tx_filt_ipv4_da0_reg.bf.ip_addr << 16) |
		ptp_tx_filt_ipv4_da1_reg.bf.ip_addr;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da0_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da1_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da1_reg));
	ts_engine->ipv6_dip.ul[0] = (ptp_tx_filt_ipv6_da0_reg.bf.ip_addr << 16) |
		ptp_tx_filt_ipv6_da1_reg.bf.ip_addr;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da2_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da2_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da3_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da3_reg));
	ts_engine->ipv6_dip.ul[1] = (ptp_tx_filt_ipv6_da2_reg.bf.ip_addr << 16) |
		ptp_tx_filt_ipv6_da3_reg.bf.ip_addr;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da4_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da4_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da5_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da5_reg));
	ts_engine->ipv6_dip.ul[2] = (ptp_tx_filt_ipv6_da4_reg.bf.ip_addr << 16) |
		ptp_tx_filt_ipv6_da5_reg.bf.ip_addr;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da6_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da6_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_ipv6_da7_reg_get(dev_id,
				phy_id, &ptp_tx_filt_ipv6_da7_reg));
	ts_engine->ipv6_dip.ul[3] = (ptp_tx_filt_ipv6_da6_reg.bf.ip_addr << 16) |
		ptp_tx_filt_ipv6_da7_reg.bf.ip_addr;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_filt_udp_port_reg_get(dev_id,
				phy_id, &ptp_tx_filt_udp_port_reg));
	ts_engine->udp_dport = ptp_tx_filt_udp_port_reg.bf.udp_port;

	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_0_reg_get(dev_id,
				phy_id, &ptp_loc_mac_addr_0_reg));
	ts_engine->y1731_mac_addr.uc[0] = ptp_loc_mac_addr_0_reg.bf.mac_addr >> 8;
	ts_engine->y1731_mac_addr.uc[1] = ptp_loc_mac_addr_0_reg.bf.mac_addr & 0xff;

	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_1_reg_get(dev_id,
				phy_id, &ptp_loc_mac_addr_1_reg));
	ts_engine->y1731_mac_addr.uc[2] = ptp_loc_mac_addr_1_reg.bf.mac_addr >> 8;
	ts_engine->y1731_mac_addr.uc[3] = ptp_loc_mac_addr_1_reg.bf.mac_addr & 0xff;

	SW_RTN_ON_ERROR(qca808x_ptp_loc_mac_addr_2_reg_get(dev_id,
				phy_id, &ptp_loc_mac_addr_2_reg));
	ts_engine->y1731_mac_addr.uc[4] = ptp_loc_mac_addr_2_reg.bf.mac_addr >> 8;
	ts_engine->y1731_mac_addr.uc[5] = ptp_loc_mac_addr_2_reg.bf.mac_addr & 0xff;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_com_ts_status_reg_get(dev_id,
				phy_id, &ptp_tx_com_ts_status_reg));
	ts_engine->enhance_ts_status_bmp = ptp_tx_com_ts_status_reg.bf.mac_lengthtype |
		(ptp_tx_com_ts_status_reg.bf.mac_da <<
		 PTP_TX_COM_TS_STATUS_REG_MAC_DA_OFFSET) |
		(ptp_tx_com_ts_status_reg.bf.mac_ptp_prim_addr <<
		 PTP_TX_COM_TS_STATUS_REG_MAC_PTP_PRIM_ADDR_OFFSET) |
		(ptp_tx_com_ts_status_reg.bf.mac_ptp_pdelay_addr <<
		 PTP_TX_COM_TS_STATUS_REG_MAC_PTP_PDELAY_ADDR_OFFSET) |
		(ptp_tx_com_ts_status_reg.bf.ipv4_layer4_protocol <<
		 PTP_TX_COM_TS_STATUS_REG_IPV4_LAYER4_PROTOCOL_OFFSET) |
		(ptp_tx_com_ts_status_reg.bf.ipv4_da <<
		 PTP_TX_COM_TS_STATUS_REG_IPV4_DA_OFFSET) |
		(ptp_tx_com_ts_status_reg.bf.ipv4_ptp_prim_addr <<
		 PTP_TX_COM_TS_STATUS_REG_IPV4_PTP_PRIM_ADDR_OFFSET) |
		(ptp_tx_com_ts_status_reg.bf.ipv4_ptp_pdelay_addr <<
		 PTP_TX_COM_TS_STATUS_REG_IPV4_PTP_PDELAY_ADDR_OFFSET) |
		(ptp_tx_com_ts_status_reg.bf.ipv6_next_header <<
		 PTP_TX_COM_TS_STATUS_REG_IPV6_NEXT_HEADER_OFFSET) |
		(ptp_tx_com_ts_status_reg.bf.ipv6_da <<
		 PTP_TX_COM_TS_STATUS_REG_IPV6_DA_OFFSET) |
		(ptp_tx_com_ts_status_reg.bf.ipv6_ptp_prim_addr <<
		 PTP_TX_COM_TS_STATUS_REG_IPV6_PTP_PRIM_ADDR_OFFSET) |
		(ptp_tx_com_ts_status_reg.bf.ipv6_ptp_pdelay_addr <<
		 PTP_TX_COM_TS_STATUS_REG_IPV6_PTP_PDELAY_ADDR_OFFSET) |
		(ptp_tx_com_ts_status_reg.bf.udp_dport <<
		 PTP_TX_COM_TS_STATUS_REG_UDP_DPORT_OFFSET) |
		(ptp_tx_com_ts_status_reg.bf.udp_ptp_event_dport <<
		 PTP_TX_COM_TS_STATUS_REG_UDP_PTP_EVENT_DPORT_OFFSET) |
		(ptp_tx_com_ts_status_reg.bf.y1731_mach <<
		 PTP_TX_COM_TS_STATUS_REG_Y1731_MACH_OFFSET);

	ts_engine->enhance_ts_status_pre_bmp = 0;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_y1731_identify_reg_get(dev_id,
				phy_id, &ptp_tx_y1731_identify_reg));
	ts_engine->y1731_identity = ptp_tx_y1731_identify_reg.bf.identify;

	ts_engine->y1731_identity_pre = 0;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_com_timestamp0_reg_get(dev_id,
				phy_id, &ptp_tx_com_timestamp0_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_com_timestamp1_reg_get(dev_id,
				phy_id, &ptp_tx_com_timestamp1_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_com_timestamp2_reg_get(dev_id,
				phy_id, &ptp_tx_com_timestamp2_reg));
	ts_engine->timestamp.seconds =
		((a_int64_t)ptp_tx_com_timestamp0_reg.bf.com_ts << 32) |
		(ptp_tx_com_timestamp1_reg.bf.com_ts << 16) |
		ptp_tx_com_timestamp2_reg.bf.com_ts;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_com_timestamp3_reg_get(dev_id,
				phy_id, &ptp_tx_com_timestamp3_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_tx_com_timestamp4_reg_get(dev_id,
				phy_id, &ptp_tx_com_timestamp4_reg));
	ts_engine->timestamp.nanoseconds = (ptp_tx_com_timestamp3_reg.bf.com_ts << 16) |
		ptp_tx_com_timestamp4_reg.bf.com_ts;

	SW_RTN_ON_ERROR(qca808x_ptp_tx_com_frac_nano_reg_get(dev_id,
				phy_id, &ptp_tx_com_frac_nano_reg));
	ts_engine->timestamp.fracnanoseconds = ptp_tx_com_frac_nano_reg.bf.frac_nano;

	ts_engine->timestamp_pre.seconds = 0;
	ts_engine->timestamp_pre.nanoseconds = 0;
	ts_engine->timestamp_pre.fracnanoseconds = 0;

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_enhanced_timestamp_engine_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	if (direction == FAL_RX_DIRECTION)
	{
		return _qca808x_phy_ptp_enhanced_timestamp_engine_rx_get(dev_id,
				phy_id, ts_engine);
	}
	else
	{
		return _qca808x_phy_ptp_enhanced_timestamp_engine_tx_get(dev_id,
				phy_id, ts_engine);
	}

	return SW_BAD_PARAM;
}

sw_error_t
qca808x_phy_ptp_trigger_set(a_uint32_t dev_id,
		a_uint32_t phy_id, a_uint32_t trigger_id,
		fal_ptp_trigger_t *triger)
{
	union ptp_trigger0_config_reg_u ptp_trigger0_config_reg = {0};
	union ptp_trigger1_config_reg_u ptp_trigger1_config_reg = {0};
	union ptp_trigger0_timestamp0_reg_u ptp_trigger0_timestamp0_reg = {0};
	union ptp_trigger0_timestamp1_reg_u ptp_trigger0_timestamp1_reg = {0};
	union ptp_trigger0_timestamp2_reg_u ptp_trigger0_timestamp2_reg = {0};
	union ptp_trigger0_timestamp3_reg_u ptp_trigger0_timestamp3_reg = {0};
	union ptp_trigger0_timestamp4_reg_u ptp_trigger0_timestamp4_reg = {0};
	union ptp_trigger1_timestamp0_reg_u ptp_trigger1_timestamp0_reg = {0};
	union ptp_trigger1_timestamp1_reg_u ptp_trigger1_timestamp1_reg = {0};
	union ptp_trigger1_timestamp2_reg_u ptp_trigger1_timestamp2_reg = {0};
	union ptp_trigger1_timestamp3_reg_u ptp_trigger1_timestamp3_reg = {0};
	union ptp_trigger1_timestamp4_reg_u ptp_trigger1_timestamp4_reg = {0};

	if (trigger_id == 0)
	{
		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_config_reg_get(dev_id,
					phy_id, &ptp_trigger0_config_reg));
		ptp_trigger0_config_reg.bf.status = triger->trigger_conf.trigger_en;
		ptp_trigger0_config_reg.bf.force_en = triger->trigger_conf.output_force_en;
		ptp_trigger0_config_reg.bf.force_value = triger->trigger_conf.output_force_value;
		ptp_trigger0_config_reg.bf.pattern = triger->trigger_conf.patten_select;
		ptp_trigger0_config_reg.bf.if_late = triger->trigger_conf.late_operation;
		ptp_trigger0_config_reg.bf.notify = triger->trigger_conf.notify;
		ptp_trigger0_config_reg.bf.setting = triger->trigger_conf.trigger_effect;
		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_config_reg_set(dev_id,
					phy_id, &ptp_trigger0_config_reg));

		ptp_trigger0_timestamp0_reg.bf.ts_sec =
			(triger->trigger_conf.tim.seconds >> 32) & 0xffff;
		ptp_trigger0_timestamp1_reg.bf.ts_sec =
			(triger->trigger_conf.tim.seconds >> 16) & 0xffff;
		ptp_trigger0_timestamp2_reg.bf.ts_sec =
			triger->trigger_conf.tim.seconds & 0xffff;
		ptp_trigger0_timestamp3_reg.bf.ts_nsec =
			triger->trigger_conf.tim.nanoseconds >> 16;
		ptp_trigger0_timestamp4_reg.bf.ts_nsec =
			triger->trigger_conf.tim.nanoseconds & 0xffff;
		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_timestamp0_reg_set(dev_id,
					phy_id, &ptp_trigger0_timestamp0_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_timestamp1_reg_set(dev_id,
					phy_id, &ptp_trigger0_timestamp1_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_timestamp2_reg_set(dev_id,
					phy_id, &ptp_trigger0_timestamp2_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_timestamp3_reg_set(dev_id,
					phy_id, &ptp_trigger0_timestamp3_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_timestamp4_reg_set(dev_id,
					phy_id, &ptp_trigger0_timestamp4_reg));
	} else {
		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_config_reg_get(dev_id,
					phy_id, &ptp_trigger1_config_reg));
		ptp_trigger1_config_reg.bf.status = triger->trigger_conf.trigger_en;
		ptp_trigger1_config_reg.bf.force_en = triger->trigger_conf.output_force_en;
		ptp_trigger1_config_reg.bf.force_value = triger->trigger_conf.output_force_value;
		ptp_trigger1_config_reg.bf.pattern = triger->trigger_conf.patten_select;
		ptp_trigger1_config_reg.bf.if_late = triger->trigger_conf.late_operation;
		ptp_trigger1_config_reg.bf.notify = triger->trigger_conf.notify;
		ptp_trigger1_config_reg.bf.setting = triger->trigger_conf.trigger_effect;
		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_config_reg_set(dev_id,
					phy_id, &ptp_trigger1_config_reg));

		ptp_trigger1_timestamp0_reg.bf.ts_sec =
			(triger->trigger_conf.tim.seconds >> 32) & 0xffff;
		ptp_trigger1_timestamp1_reg.bf.ts_sec =
			(triger->trigger_conf.tim.seconds >> 16) & 0xffff;
		ptp_trigger1_timestamp2_reg.bf.ts_sec =
			triger->trigger_conf.tim.seconds & 0xffff;
		ptp_trigger1_timestamp3_reg.bf.ts_nsec =
			triger->trigger_conf.tim.nanoseconds >> 16;
		ptp_trigger1_timestamp4_reg.bf.ts_nsec =
			triger->trigger_conf.tim.nanoseconds & 0xffff;
		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_timestamp0_reg_set(dev_id,
					phy_id, &ptp_trigger1_timestamp0_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_timestamp1_reg_set(dev_id,
					phy_id, &ptp_trigger1_timestamp1_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_timestamp2_reg_set(dev_id,
					phy_id, &ptp_trigger1_timestamp2_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_timestamp3_reg_set(dev_id,
					phy_id, &ptp_trigger1_timestamp3_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_timestamp4_reg_set(dev_id,
					phy_id, &ptp_trigger1_timestamp4_reg));
	}

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_trigger_get(a_uint32_t dev_id,
		a_uint32_t phy_id, a_uint32_t trigger_id,
		fal_ptp_trigger_t *triger)
{
	union ptp_trigger0_config_reg_u ptp_trigger0_config_reg = {0};
	union ptp_trigger0_status_reg_u ptp_trigger0_status_reg = {0};
	union ptp_trigger1_config_reg_u ptp_trigger1_config_reg = {0};
	union ptp_trigger1_status_reg_u ptp_trigger1_status_reg = {0};
	union ptp_trigger0_timestamp0_reg_u ptp_trigger0_timestamp0_reg = {0};
	union ptp_trigger0_timestamp1_reg_u ptp_trigger0_timestamp1_reg = {0};
	union ptp_trigger0_timestamp2_reg_u ptp_trigger0_timestamp2_reg = {0};
	union ptp_trigger0_timestamp3_reg_u ptp_trigger0_timestamp3_reg = {0};
	union ptp_trigger0_timestamp4_reg_u ptp_trigger0_timestamp4_reg = {0};
	union ptp_trigger1_timestamp0_reg_u ptp_trigger1_timestamp0_reg = {0};
	union ptp_trigger1_timestamp1_reg_u ptp_trigger1_timestamp1_reg = {0};
	union ptp_trigger1_timestamp2_reg_u ptp_trigger1_timestamp2_reg = {0};
	union ptp_trigger1_timestamp3_reg_u ptp_trigger1_timestamp3_reg = {0};
	union ptp_trigger1_timestamp4_reg_u ptp_trigger1_timestamp4_reg = {0};

	if (trigger_id == 0)
	{
		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_config_reg_get(dev_id,
					phy_id, &ptp_trigger0_config_reg));
		triger->trigger_conf.trigger_en = ptp_trigger0_config_reg.bf.status;
		triger->trigger_conf.output_force_en = ptp_trigger0_config_reg.bf.force_en;
		triger->trigger_conf.output_force_value = ptp_trigger0_config_reg.bf.force_value;
		triger->trigger_conf.patten_select = ptp_trigger0_config_reg.bf.pattern;
		triger->trigger_conf.late_operation = ptp_trigger0_config_reg.bf.if_late;
		triger->trigger_conf.notify = ptp_trigger0_config_reg.bf.notify;
		triger->trigger_conf.trigger_effect = ptp_trigger0_config_reg.bf.setting;

		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_timestamp0_reg_get(dev_id,
					phy_id, &ptp_trigger0_timestamp0_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_timestamp1_reg_get(dev_id,
					phy_id, &ptp_trigger0_timestamp1_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_timestamp2_reg_get(dev_id,
					phy_id, &ptp_trigger0_timestamp2_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_timestamp3_reg_get(dev_id,
					phy_id, &ptp_trigger0_timestamp3_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_timestamp4_reg_get(dev_id,
					phy_id, &ptp_trigger0_timestamp4_reg));
		triger->trigger_conf.tim.seconds =
			((a_int64_t)ptp_trigger0_timestamp0_reg.bf.ts_sec << 32) |
			(ptp_trigger0_timestamp1_reg.bf.ts_sec << 16) |
			ptp_trigger0_timestamp2_reg.bf.ts_sec;
		triger->trigger_conf.tim.nanoseconds =
			(ptp_trigger0_timestamp3_reg.bf.ts_nsec << 16) |
			ptp_trigger0_timestamp4_reg.bf.ts_nsec;

		SW_RTN_ON_ERROR(qca808x_ptp_trigger0_status_reg_get(dev_id,
					phy_id, &ptp_trigger0_status_reg));
		triger->trigger_status.trigger_finished = ptp_trigger0_status_reg.bf.finished;
		triger->trigger_status.trigger_active = ptp_trigger0_status_reg.bf.active;
		triger->trigger_status.trigger_error = ptp_trigger0_status_reg.bf.error;
	} else {
		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_config_reg_get(dev_id,
					phy_id, &ptp_trigger1_config_reg));
		triger->trigger_conf.trigger_en = ptp_trigger1_config_reg.bf.status;
		triger->trigger_conf.output_force_en = ptp_trigger1_config_reg.bf.force_en;
		triger->trigger_conf.output_force_value = ptp_trigger1_config_reg.bf.force_value;
		triger->trigger_conf.patten_select = ptp_trigger1_config_reg.bf.pattern;
		triger->trigger_conf.late_operation = ptp_trigger1_config_reg.bf.if_late;
		triger->trigger_conf.notify = ptp_trigger1_config_reg.bf.notify;
		triger->trigger_conf.trigger_effect = ptp_trigger1_config_reg.bf.setting;

		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_timestamp0_reg_get(dev_id,
					phy_id, &ptp_trigger1_timestamp0_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_timestamp1_reg_get(dev_id,
					phy_id, &ptp_trigger1_timestamp1_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_timestamp2_reg_get(dev_id,
					phy_id, &ptp_trigger1_timestamp2_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_timestamp3_reg_get(dev_id,
					phy_id, &ptp_trigger1_timestamp3_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_timestamp4_reg_get(dev_id,
					phy_id, &ptp_trigger1_timestamp4_reg));
		triger->trigger_conf.tim.seconds =
			((a_int64_t)ptp_trigger1_timestamp0_reg.bf.ts_sec << 32) |
			(ptp_trigger1_timestamp1_reg.bf.ts_sec << 16) |
			ptp_trigger1_timestamp2_reg.bf.ts_sec;
		triger->trigger_conf.tim.nanoseconds =
			(ptp_trigger1_timestamp3_reg.bf.ts_nsec << 16) |
			ptp_trigger1_timestamp4_reg.bf.ts_nsec;

		SW_RTN_ON_ERROR(qca808x_ptp_trigger1_status_reg_get(dev_id,
					phy_id, &ptp_trigger1_status_reg));
		triger->trigger_status.trigger_finished = ptp_trigger1_status_reg.bf.finished;
		triger->trigger_status.trigger_active = ptp_trigger1_status_reg.bf.active;
		triger->trigger_status.trigger_error = ptp_trigger1_status_reg.bf.error;
	}

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_capture_set(a_uint32_t dev_id,
		a_uint32_t phy_id, a_uint32_t capture_id,
		fal_ptp_capture_t *capture)
{
	union ptp_event0_config_reg_u ptp_event0_config_reg = {0};
	union ptp_event1_config_reg_u ptp_event1_config_reg = {0};

	if (capture_id == 0)
	{
		SW_RTN_ON_ERROR(qca808x_ptp_event0_config_reg_get(dev_id,
					phy_id, &ptp_event0_config_reg));
		ptp_event0_config_reg.bf.clear_stat = capture->capture_conf.status_clear;
		ptp_event0_config_reg.bf.notify = capture->capture_conf.notify_event;
		ptp_event0_config_reg.bf.single_cap = capture->capture_conf.single_multi_select;
		ptp_event0_config_reg.bf.fall_en = capture->capture_conf.fall_edge_en;
		ptp_event0_config_reg.bf.rise_en = capture->capture_conf.rise_edge_en;
		SW_RTN_ON_ERROR(qca808x_ptp_event0_config_reg_set(dev_id,
					phy_id, &ptp_event0_config_reg));
	}
	else
	{
		SW_RTN_ON_ERROR(qca808x_ptp_event1_config_reg_get(dev_id,
					phy_id, &ptp_event1_config_reg));
		ptp_event1_config_reg.bf.clear_stat = capture->capture_conf.status_clear;
		ptp_event1_config_reg.bf.notify = capture->capture_conf.notify_event;
		ptp_event1_config_reg.bf.single_cap = capture->capture_conf.single_multi_select;
		ptp_event1_config_reg.bf.fall_en = capture->capture_conf.fall_edge_en;
		ptp_event1_config_reg.bf.rise_en = capture->capture_conf.rise_edge_en;
		SW_RTN_ON_ERROR(qca808x_ptp_event1_config_reg_set(dev_id,
					phy_id, &ptp_event1_config_reg));
	}

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_capture_get(a_uint32_t dev_id,
		a_uint32_t phy_id, a_uint32_t capture_id,
		fal_ptp_capture_t *capture)
{
	union ptp_event0_config_reg_u ptp_event0_config_reg = {0};
	union ptp_event0_status_reg_u ptp_event0_status_reg = {0};
	union ptp_event1_config_reg_u ptp_event1_config_reg = {0};
	union ptp_event1_status_reg_u ptp_event1_status_reg = {0};
	union ptp_event0_timestamp0_reg_u ptp_event0_timestamp0_reg = {0};
	union ptp_event0_timestamp1_reg_u ptp_event0_timestamp1_reg = {0};
	union ptp_event0_timestamp2_reg_u ptp_event0_timestamp2_reg = {0};
	union ptp_event0_timestamp3_reg_u ptp_event0_timestamp3_reg = {0};
	union ptp_event0_timestamp4_reg_u ptp_event0_timestamp4_reg = {0};
	union ptp_event1_timestamp0_reg_u ptp_event1_timestamp0_reg = {0};
	union ptp_event1_timestamp1_reg_u ptp_event1_timestamp1_reg = {0};
	union ptp_event1_timestamp2_reg_u ptp_event1_timestamp2_reg = {0};
	union ptp_event1_timestamp3_reg_u ptp_event1_timestamp3_reg = {0};
	union ptp_event1_timestamp4_reg_u ptp_event1_timestamp4_reg = {0};

	if (capture_id == 0)
	{
		SW_RTN_ON_ERROR(qca808x_ptp_event0_config_reg_get(dev_id,
					phy_id, &ptp_event0_config_reg));
		capture->capture_conf.status_clear = ptp_event0_config_reg.bf.clear_stat;
		capture->capture_conf.notify_event = ptp_event0_config_reg.bf.notify;
		capture->capture_conf.single_multi_select = ptp_event0_config_reg.bf.single_cap;
		capture->capture_conf.fall_edge_en = ptp_event0_config_reg.bf.fall_en;
		capture->capture_conf.rise_edge_en = ptp_event0_config_reg.bf.rise_en;

		SW_RTN_ON_ERROR(qca808x_ptp_event0_status_reg_get(dev_id,
					phy_id, &ptp_event0_status_reg));
		capture->capture_status.event_detected = ptp_event0_status_reg.bf.detected;
		capture->capture_status.fall_rise_edge_detected =
			ptp_event0_status_reg.bf.dir_detected;
		capture->capture_status.single_multi_detected = ptp_event0_status_reg.bf.mul_event;
		capture->capture_status.event_missed_cnt = ptp_event0_status_reg.bf.missed_count;

		SW_RTN_ON_ERROR(qca808x_ptp_event0_timestamp0_reg_get(dev_id,
					phy_id, &ptp_event0_timestamp0_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_event0_timestamp1_reg_get(dev_id,
					phy_id, &ptp_event0_timestamp1_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_event0_timestamp2_reg_get(dev_id,
					phy_id, &ptp_event0_timestamp2_reg));
		capture->capture_status.tim.seconds =
			((a_int64_t)ptp_event0_timestamp0_reg.bf.ts_nsec << 32) |
			(ptp_event0_timestamp1_reg.bf.ts_nsec << 16) |
			ptp_event0_timestamp2_reg.bf.ts_nsec;
		SW_RTN_ON_ERROR(qca808x_ptp_event0_timestamp3_reg_get(dev_id,
					phy_id, &ptp_event0_timestamp3_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_event0_timestamp4_reg_get(dev_id,
					phy_id, &ptp_event0_timestamp4_reg));
		capture->capture_status.tim.nanoseconds =
			(ptp_event0_timestamp3_reg.bf.ts_nsec << 16) |
			ptp_event0_timestamp4_reg.bf.ts_nsec;
	} else {
		SW_RTN_ON_ERROR(qca808x_ptp_event1_config_reg_get(dev_id,
					phy_id, &ptp_event1_config_reg));
		capture->capture_conf.status_clear = ptp_event1_config_reg.bf.clear_stat;
		capture->capture_conf.notify_event = ptp_event1_config_reg.bf.notify;
		capture->capture_conf.single_multi_select = ptp_event1_config_reg.bf.single_cap;
		capture->capture_conf.fall_edge_en = ptp_event1_config_reg.bf.fall_en;
		capture->capture_conf.rise_edge_en = ptp_event1_config_reg.bf.rise_en;

		SW_RTN_ON_ERROR(qca808x_ptp_event1_status_reg_get(dev_id,
					phy_id, &ptp_event1_status_reg));
		capture->capture_status.event_detected = ptp_event1_status_reg.bf.detected;
		capture->capture_status.fall_rise_edge_detected =
			ptp_event1_status_reg.bf.dir_detected;
		capture->capture_status.single_multi_detected = ptp_event1_status_reg.bf.mul_event;
		capture->capture_status.event_missed_cnt = ptp_event1_status_reg.bf.missed_count;

		SW_RTN_ON_ERROR(qca808x_ptp_event1_timestamp0_reg_get(dev_id,
					phy_id, &ptp_event1_timestamp0_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_event1_timestamp1_reg_get(dev_id,
					phy_id, &ptp_event1_timestamp1_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_event1_timestamp2_reg_get(dev_id,
					phy_id, &ptp_event1_timestamp2_reg));
		capture->capture_status.tim.seconds =
			((a_int64_t)ptp_event1_timestamp0_reg.bf.ts_nsec << 32) |
			(ptp_event1_timestamp1_reg.bf.ts_nsec << 16) |
			ptp_event1_timestamp2_reg.bf.ts_nsec;
		SW_RTN_ON_ERROR(qca808x_ptp_event1_timestamp3_reg_get(dev_id,
					phy_id, &ptp_event1_timestamp3_reg));
		SW_RTN_ON_ERROR(qca808x_ptp_event1_timestamp4_reg_get(dev_id,
					phy_id, &ptp_event1_timestamp4_reg));
		capture->capture_status.tim.nanoseconds =
			(ptp_event1_timestamp3_reg.bf.ts_nsec << 16) |
			ptp_event1_timestamp4_reg.bf.ts_nsec;
	}

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_interrupt_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_interrupt_t *interrupt)
{
	union ptp_imr_reg_u ptp_imr_reg = {0};
	union ptp_ext_imr_reg_u ptp_ext_imr_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_imr_reg_get(dev_id, phy_id, &ptp_imr_reg));
	ptp_imr_reg.bf.mask_bmp &= (~(0x7 << 2)) & 0xffff;
	ptp_imr_reg.bf.mask_bmp |= (interrupt->intr_mask & 0x7) << 2;
	SW_RTN_ON_ERROR(qca808x_ptp_imr_reg_set(dev_id, phy_id, &ptp_imr_reg));

	ptp_ext_imr_reg.bf.mask_bmp = interrupt->intr_mask >> 3;
	SW_RTN_ON_ERROR(qca808x_ptp_ext_imr_reg_set(dev_id, phy_id, &ptp_ext_imr_reg));

	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_interrupt_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_interrupt_t *interrupt)
{
	union ptp_imr_reg_u ptp_imr_reg = {0};
	union ptp_isr_reg_u ptp_isr_reg = {0};
	union ptp_ext_imr_reg_u ptp_ext_imr_reg = {0};
	union ptp_ext_isr_reg_u ptp_ext_isr_reg = {0};

	SW_RTN_ON_ERROR(qca808x_ptp_imr_reg_get(dev_id, phy_id, &ptp_imr_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_ext_imr_reg_get(dev_id, phy_id, &ptp_ext_imr_reg));
	interrupt->intr_mask = ((ptp_imr_reg.bf.mask_bmp >> 2) & 0x7) |
			((ptp_ext_imr_reg.bf.mask_bmp & 0x7ff) << 3);

	SW_RTN_ON_ERROR(qca808x_ptp_isr_reg_get(dev_id, phy_id, &ptp_isr_reg));
	SW_RTN_ON_ERROR(qca808x_ptp_ext_isr_reg_get(dev_id, phy_id, &ptp_ext_isr_reg));
	interrupt->intr_status = ((ptp_isr_reg.bf.status_bmp >> 2) & 0x7) |
			((ptp_ext_isr_reg.bf.status_bmp & 0x7ff) << 3);

	return SW_OK;
}

void qca808x_phy_ptp_api_ops_init(hsl_phy_ptp_ops_t *ptp_ops)
{
	if (!ptp_ops) {
		return;
	}
	ptp_ops->phy_ptp_security_set = qca808x_phy_ptp_security_set;
	ptp_ops->phy_ptp_link_delay_set = qca808x_phy_ptp_link_delay_set;
	ptp_ops->phy_ptp_rx_crc_recalc_status_get = qca808x_phy_ptp_rx_crc_recalc_status_get;
	ptp_ops->phy_ptp_tod_uart_set = qca808x_phy_ptp_tod_uart_set;
	ptp_ops->phy_ptp_pps_signal_control_set = qca808x_phy_ptp_pps_signal_control_set;
	ptp_ops->phy_ptp_timestamp_get = qca808x_phy_ptp_timestamp_get;
	ptp_ops->phy_ptp_asym_correction_get = qca808x_phy_ptp_asym_correction_get;
	ptp_ops->phy_ptp_capture_set = qca808x_phy_ptp_capture_set;
	ptp_ops->phy_ptp_rtc_adjfreq_set = qca808x_phy_ptp_rtc_adjfreq_set;
	ptp_ops->phy_ptp_asym_correction_set = qca808x_phy_ptp_asym_correction_set;
	ptp_ops->phy_ptp_pkt_timestamp_set = qca808x_phy_ptp_pkt_timestamp_set;
	ptp_ops->phy_ptp_rtc_time_get = qca808x_phy_ptp_rtc_time_get;
	ptp_ops->phy_ptp_rtc_time_set = qca808x_phy_ptp_rtc_time_set;
	ptp_ops->phy_ptp_pkt_timestamp_get = qca808x_phy_ptp_pkt_timestamp_get;
	ptp_ops->phy_ptp_interrupt_set = qca808x_phy_ptp_interrupt_set;
	ptp_ops->phy_ptp_trigger_set = qca808x_phy_ptp_trigger_set;
	ptp_ops->phy_ptp_pps_signal_control_get = qca808x_phy_ptp_pps_signal_control_get;
	ptp_ops->phy_ptp_capture_get = qca808x_phy_ptp_capture_get;
	ptp_ops->phy_ptp_rx_crc_recalc_enable = qca808x_phy_ptp_rx_crc_recalc_enable;
	ptp_ops->phy_ptp_security_get = qca808x_phy_ptp_security_get;
	ptp_ops->phy_ptp_tod_uart_get = qca808x_phy_ptp_tod_uart_get;
	ptp_ops->phy_ptp_rtc_time_clear = qca808x_phy_ptp_rtc_time_clear;
	ptp_ops->phy_ptp_reference_clock_set = qca808x_phy_ptp_reference_clock_set;
	ptp_ops->phy_ptp_output_waveform_set = qca808x_phy_ptp_output_waveform_set;
	ptp_ops->phy_ptp_rx_timestamp_mode_set = qca808x_phy_ptp_rx_timestamp_mode_set;
	ptp_ops->phy_ptp_grandmaster_mode_set = qca808x_phy_ptp_grandmaster_mode_set;
	ptp_ops->phy_ptp_config_set = qca808x_phy_ptp_config_set;
	ptp_ops->phy_ptp_trigger_get = qca808x_phy_ptp_trigger_get;
	ptp_ops->phy_ptp_rtc_adjfreq_get = qca808x_phy_ptp_rtc_adjfreq_get;
	ptp_ops->phy_ptp_grandmaster_mode_get = qca808x_phy_ptp_grandmaster_mode_get;
	ptp_ops->phy_ptp_rx_timestamp_mode_get = qca808x_phy_ptp_rx_timestamp_mode_get;
	ptp_ops->phy_ptp_rtc_adjtime_set = qca808x_phy_ptp_rtc_adjtime_set;
	ptp_ops->phy_ptp_link_delay_get = qca808x_phy_ptp_link_delay_get;
	ptp_ops->phy_ptp_config_get = qca808x_phy_ptp_config_get;
	ptp_ops->phy_ptp_output_waveform_get = qca808x_phy_ptp_output_waveform_get;
	ptp_ops->phy_ptp_interrupt_get = qca808x_phy_ptp_interrupt_get;
	ptp_ops->phy_ptp_rtc_time_snapshot_enable = qca808x_phy_ptp_rtc_time_snapshot_enable;
	ptp_ops->phy_ptp_reference_clock_get = qca808x_phy_ptp_reference_clock_get;
	ptp_ops->phy_ptp_enhanced_timestamp_engine_set =
		qca808x_phy_ptp_enhanced_timestamp_engine_set;
	ptp_ops->phy_ptp_rtc_time_snapshot_status_get =
		qca808x_phy_ptp_rtc_time_snapshot_status_get;
	ptp_ops->phy_ptp_enhanced_timestamp_engine_get =
		qca808x_phy_ptp_enhanced_timestamp_engine_get;
	ptp_ops->phy_ptp_increment_sync_from_clock_enable =
		qca808x_phy_ptp_increment_sync_from_clock_enable;
	ptp_ops->phy_ptp_increment_sync_from_clock_status_get =
		qca808x_phy_ptp_increment_sync_from_clock_status_get;
}
