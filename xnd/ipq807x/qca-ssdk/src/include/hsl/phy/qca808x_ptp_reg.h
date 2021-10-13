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


/**
 * @defgroup
 * @{
 */
#ifndef PTP_REG_REG_H
#define PTP_REG_REG_H

#define PTP_REG_BIT_FALSE 0
#define PTP_REG_BIT_TRUE  1

/*[register] PTP_IMR_REG*/
#define PTP_IMR_REG
#define PTP_IMR_REG_ADDRESS 0x12
#define PTP_IMR_REG_NUM     1
#define PTP_IMR_REG_INC     0x1
#define PTP_IMR_REG_TYPE    REG_TYPE_RW
#define PTP_IMR_REG_DEFAULT 0x0
	/*[field] MASK_BMP*/
	#define PTP_IMR_REG_MASK_BMP
	#define PTP_IMR_REG_MASK_BMP_OFFSET  0
	#define PTP_IMR_REG_MASK_BMP_LEN     16
	#define PTP_IMR_REG_MASK_BMP_DEFAULT 0x0

struct ptp_imr_reg {
	a_uint16_t  mask_bmp:16;
};

union ptp_imr_reg_u {
	a_uint32_t val;
	struct ptp_imr_reg bf;
};

/*[register] PTP_ISR_REG*/
#define PTP_ISR_REG
#define PTP_ISR_REG_ADDRESS 0x13
#define PTP_ISR_REG_NUM     1
#define PTP_ISR_REG_INC     0x1
#define PTP_ISR_REG_TYPE    REG_TYPE_RW
#define PTP_ISR_REG_DEFAULT 0x0
	/*[field] STATUS_BMP*/
	#define PTP_ISR_REG_STATUS_BMP
	#define PTP_ISR_REG_STATUS_BMP_OFFSET  0
	#define PTP_ISR_REG_STATUS_BMP_LEN     16
	#define PTP_ISR_REG_STATUS_BMP_DEFAULT 0x0

struct ptp_isr_reg {
	a_uint16_t  status_bmp:16;
};

union ptp_isr_reg_u {
	a_uint32_t val;
	struct ptp_isr_reg bf;
};

/*[register] PTP_HW_ENABLE_REG*/
#define PTP_HW_ENABLE_REG
#define PTP_HW_ENABLE_REG_ADDRESS 0x1f
#define PTP_HW_ENABLE_REG_NUM     1
#define PTP_HW_ENABLE_REG_INC     0x1
#define PTP_HW_ENABLE_REG_TYPE    REG_TYPE_RW
#define PTP_HW_ENABLE_REG_DEFAULT 0x0
	/*[field] PTP_HW_ENABLE*/
	#define PTP_HW_ENABLE_REG_PTP_HW_ENABLE
	#define PTP_HW_ENABLE_REG_PTP_HW_ENABLE_OFFSET  1
	#define PTP_HW_ENABLE_REG_PTP_HW_ENABLE_LEN     1
	#define PTP_HW_ENABLE_REG_PTP_HW_ENABLE_DEFAULT 0x0

struct ptp_hw_enable_reg {
	a_uint16_t  _reserved0:1;
	a_uint16_t  ptp_hw_enable:1;
};

union ptp_hw_enable_reg_u {
	a_uint32_t val;
	struct ptp_hw_enable_reg bf;
};

/*[register] PTP_MAIN_CONF_REG*/
#define PTP_MAIN_CONF_REG
#define PTP_MAIN_CONF_REG_ADDRESS 0x8012
#define PTP_MAIN_CONF_REG_NUM     1
#define PTP_MAIN_CONF_REG_INC     0x1
#define PTP_MAIN_CONF_REG_TYPE    REG_TYPE_RW
#define PTP_MAIN_CONF_REG_DEFAULT 0x0
	/*[field] PTP_CLOCK_MODE*/
	#define PTP_MAIN_CONF_REG_PTP_CLOCK_MODE
	#define PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_OFFSET  1
	#define PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_LEN     2
	#define PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_DEFAULT 0x0
	#define PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_OC_TWO_STEP 0x0
	#define PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_OC_ONE_STEP 0x1
	#define PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_TC_TWO_STEP 0x2
	#define PTP_MAIN_CONF_REG_PTP_CLOCK_MODE_TC_ONE_STEP 0x3
	/*[field] PTP_BYPASS*/
	#define PTP_MAIN_CONF_REG_PTP_BYPASS
	#define PTP_MAIN_CONF_REG_PTP_BYPASS_OFFSET  3
	#define PTP_MAIN_CONF_REG_PTP_BYPASS_LEN     1
	#define PTP_MAIN_CONF_REG_PTP_BYPASS_DEFAULT 0x0
	/*[field] TS_ATTACH_MODE*/
	#define PTP_MAIN_CONF_REG_TS_ATTACH_MODE
	#define PTP_MAIN_CONF_REG_TS_ATTACH_MODE_OFFSET  4
	#define PTP_MAIN_CONF_REG_TS_ATTACH_MODE_LEN     1
	#define PTP_MAIN_CONF_REG_TS_ATTACH_MODE_DEFAULT 0x0
	/*[field] WOL_EN*/
	#define PTP_MAIN_CONF_REG_WOL_EN
	#define PTP_MAIN_CONF_REG_WOL_EN_OFFSET  5
	#define PTP_MAIN_CONF_REG_WOL_EN_LEN     1
	#define PTP_MAIN_CONF_REG_WOL_EN_DEFAULT 0x0
	/*[field] PTP_CLK_SEL*/
	#define PTP_MAIN_CONF_REG_PTP_CLK_SEL
	#define PTP_MAIN_CONF_REG_PTP_CLK_SEL_OFFSET  7
	#define PTP_MAIN_CONF_REG_PTP_CLK_SEL_LEN     1
	#define PTP_MAIN_CONF_REG_PTP_CLK_SEL_DEFAULT 0x0
	/*[field] DISABLE_1588_PHY*/
	#define PTP_MAIN_CONF_REG_DISABLE_1588_PHY
	#define PTP_MAIN_CONF_REG_DISABLE_1588_PHY_OFFSET  8
	#define PTP_MAIN_CONF_REG_DISABLE_1588_PHY_LEN     1
	#define PTP_MAIN_CONF_REG_DISABLE_1588_PHY_DEFAULT 0x0
	/*[field] ATTACH_CRC_RECAL*/
	#define PTP_MAIN_CONF_REG_ATTACH_CRC_RECAL
	#define PTP_MAIN_CONF_REG_ATTACH_CRC_RECAL_OFFSET  9
	#define PTP_MAIN_CONF_REG_ATTACH_CRC_RECAL_LEN     1
	#define PTP_MAIN_CONF_REG_ATTACH_CRC_RECAL_DEFAULT 0x0
	/*[field] IPV4_FORCE_CHECKSUM_ZERO*/
	#define PTP_MAIN_CONF_REG_IPV4_FORCE_CHECKSUM_ZERO
	#define PTP_MAIN_CONF_REG_IPV4_FORCE_CHECKSUM_ZERO_OFFSET  10
	#define PTP_MAIN_CONF_REG_IPV4_FORCE_CHECKSUM_ZERO_LEN     1
	#define PTP_MAIN_CONF_REG_IPV4_FORCE_CHECKSUM_ZERO_DEFAULT 0x1
	/*[field] IPV6_EMBED_FORCE_CHECKSUM_ZERO*/
	#define PTP_MAIN_CONF_REG_IPV6_EMBED_FORCE_CHECKSUM_ZERO
	#define PTP_MAIN_CONF_REG_IPV6_EMBED_FORCE_CHECKSUM_ZERO_OFFSET  11
	#define PTP_MAIN_CONF_REG_IPV6_EMBED_FORCE_CHECKSUM_ZERO_LEN     1
	#define PTP_MAIN_CONF_REG_IPV6_EMBED_FORCE_CHECKSUM_ZERO_DEFAULT 0x0

struct ptp_main_conf_reg {
	a_uint16_t  _reserved0:1;
	a_uint16_t  ptp_clock_mode:2;
	a_uint16_t  ptp_bypass:1;
	a_uint16_t  ts_attach_mode:1;
	a_uint16_t  wol_en:1;
	a_uint16_t  _reserved1:1;
	a_uint16_t  ptp_clk_sel:1;
	a_uint16_t  disable_1588_phy:1;
	a_uint16_t  attach_crc_recal:1;
	a_uint16_t  ipv4_force_checksum_zero:1;
	a_uint16_t  ipv6_embed_force_checksum_zero:1;
};

union ptp_main_conf_reg_u {
	a_uint32_t val;
	struct ptp_main_conf_reg bf;
};

/*[register] PTP_RX_SEQID0_REG*/
#define PTP_RX_SEQID0_REG
#define PTP_RX_SEQID0_REG_ADDRESS 0x8013
#define PTP_RX_SEQID0_REG_NUM     1
#define PTP_RX_SEQID0_REG_INC     0x1
#define PTP_RX_SEQID0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_SEQID0_REG_DEFAULT 0x0
	/*[field] RX_SEQID*/
	#define PTP_RX_SEQID0_REG_RX_SEQID
	#define PTP_RX_SEQID0_REG_RX_SEQID_OFFSET  0
	#define PTP_RX_SEQID0_REG_RX_SEQID_LEN     16
	#define PTP_RX_SEQID0_REG_RX_SEQID_DEFAULT 0x0

struct ptp_rx_seqid0_reg {
	a_uint16_t  rx_seqid:16;
};

union ptp_rx_seqid0_reg_u {
	a_uint32_t val;
	struct ptp_rx_seqid0_reg bf;
};

/*[register] PTP_RX_PORTID0_0_REG*/
#define PTP_RX_PORTID0_0_REG
#define PTP_RX_PORTID0_0_REG_ADDRESS 0x8014
#define PTP_RX_PORTID0_0_REG_NUM     1
#define PTP_RX_PORTID0_0_REG_INC     0x1
#define PTP_RX_PORTID0_0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID0_0_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID0_0_REG_RX_PORTID
	#define PTP_RX_PORTID0_0_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID0_0_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID0_0_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid0_0_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid0_0_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid0_0_reg bf;
};

/*[register] PTP_RX_PORTID0_1_REG*/
#define PTP_RX_PORTID0_1_REG
#define PTP_RX_PORTID0_1_REG_ADDRESS 0x8015
#define PTP_RX_PORTID0_1_REG_NUM     1
#define PTP_RX_PORTID0_1_REG_INC     0x1
#define PTP_RX_PORTID0_1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID0_1_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID0_1_REG_RX_PORTID
	#define PTP_RX_PORTID0_1_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID0_1_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID0_1_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid0_1_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid0_1_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid0_1_reg bf;
};

/*[register] PTP_RX_PORTID0_2_REG*/
#define PTP_RX_PORTID0_2_REG
#define PTP_RX_PORTID0_2_REG_ADDRESS 0x8016
#define PTP_RX_PORTID0_2_REG_NUM     1
#define PTP_RX_PORTID0_2_REG_INC     0x1
#define PTP_RX_PORTID0_2_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID0_2_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID0_2_REG_RX_PORTID
	#define PTP_RX_PORTID0_2_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID0_2_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID0_2_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid0_2_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid0_2_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid0_2_reg bf;
};

/*[register] PTP_RX_PORTID0_3_REG*/
#define PTP_RX_PORTID0_3_REG
#define PTP_RX_PORTID0_3_REG_ADDRESS 0x8017
#define PTP_RX_PORTID0_3_REG_NUM     1
#define PTP_RX_PORTID0_3_REG_INC     0x1
#define PTP_RX_PORTID0_3_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID0_3_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID0_3_REG_RX_PORTID
	#define PTP_RX_PORTID0_3_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID0_3_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID0_3_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid0_3_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid0_3_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid0_3_reg bf;
};

/*[register] PTP_RX_PORTID0_4_REG*/
#define PTP_RX_PORTID0_4_REG
#define PTP_RX_PORTID0_4_REG_ADDRESS 0x8018
#define PTP_RX_PORTID0_4_REG_NUM     1
#define PTP_RX_PORTID0_4_REG_INC     0x1
#define PTP_RX_PORTID0_4_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID0_4_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID0_4_REG_RX_PORTID
	#define PTP_RX_PORTID0_4_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID0_4_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID0_4_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid0_4_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid0_4_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid0_4_reg bf;
};


//
/*[register] PTP_RTC_CLK_REG*/
#define PTP_RTC_CLK_REG
#define PTP_RTC_CLK_REG_ADDRESS 0x8017
#define PTP_RTC_CLK_REG_NUM     1
#define PTP_RTC_CLK_REG_INC     0x1
#define PTP_RTC_CLK_REG_TYPE    REG_TYPE_RW
#define PTP_RTC_CLK_REG_DEFAULT 0x0
	/*[field] RTC_CLK_SELECTION*/
	#define PTP_RTC_CLK_REG_RTC_CLK_SELECTION
	#define PTP_RTC_CLK_REG_RTC_CLK_SELECTION_OFFSET  11
	#define PTP_RTC_CLK_REG_RTC_CLK_SELECTION_LEN     1
	#define PTP_RTC_CLK_REG_RTC_CLK_SELECTION_DEFAULT 0x0

struct ptp_rtc_clk_reg {
	a_uint16_t  _reserved0:11;
	a_uint16_t  rtc_clk_selection:1;
};

union ptp_rtc_clk_reg_u {
	a_uint32_t val;
	struct ptp_rtc_clk_reg bf;
};

/*[register] PTP_RX_TS0_0_REG*/
#define PTP_RX_TS0_0_REG
#define PTP_RX_TS0_0_REG_ADDRESS 0x8019
#define PTP_RX_TS0_0_REG_NUM     1
#define PTP_RX_TS0_0_REG_INC     0x1
#define PTP_RX_TS0_0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS0_0_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_RX_TS0_0_REG_RX_TS_SEC
	#define PTP_RX_TS0_0_REG_RX_TS_SEC_OFFSET  0
	#define PTP_RX_TS0_0_REG_RX_TS_SEC_LEN     16
	#define PTP_RX_TS0_0_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_rx_ts0_0_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_rx_ts0_0_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts0_0_reg bf;
};

/*[register] PTP_RX_TS0_1_REG*/
#define PTP_RX_TS0_1_REG
#define PTP_RX_TS0_1_REG_ADDRESS 0x801a
#define PTP_RX_TS0_1_REG_NUM     1
#define PTP_RX_TS0_1_REG_INC     0x1
#define PTP_RX_TS0_1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS0_1_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_RX_TS0_1_REG_RX_TS_SEC
	#define PTP_RX_TS0_1_REG_RX_TS_SEC_OFFSET  0
	#define PTP_RX_TS0_1_REG_RX_TS_SEC_LEN     16
	#define PTP_RX_TS0_1_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_rx_ts0_1_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_rx_ts0_1_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts0_1_reg bf;
};

/*[register] PTP_RX_TS0_2_REG*/
#define PTP_RX_TS0_2_REG
#define PTP_RX_TS0_2_REG_ADDRESS 0x801b
#define PTP_RX_TS0_2_REG_NUM     1
#define PTP_RX_TS0_2_REG_INC     0x1
#define PTP_RX_TS0_2_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS0_2_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_RX_TS0_2_REG_RX_TS_SEC
	#define PTP_RX_TS0_2_REG_RX_TS_SEC_OFFSET  0
	#define PTP_RX_TS0_2_REG_RX_TS_SEC_LEN     16
	#define PTP_RX_TS0_2_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_rx_ts0_2_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_rx_ts0_2_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts0_2_reg bf;
};

/*[register] PTP_RX_TS0_3_REG*/
#define PTP_RX_TS0_3_REG
#define PTP_RX_TS0_3_REG_ADDRESS 0x801c
#define PTP_RX_TS0_3_REG_NUM     1
#define PTP_RX_TS0_3_REG_INC     0x1
#define PTP_RX_TS0_3_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS0_3_REG_DEFAULT 0x0
	/*[field] RX_TS_NSEC*/
	#define PTP_RX_TS0_3_REG_RX_TS_NSEC
	#define PTP_RX_TS0_3_REG_RX_TS_NSEC_OFFSET  0
	#define PTP_RX_TS0_3_REG_RX_TS_NSEC_LEN     16
	#define PTP_RX_TS0_3_REG_RX_TS_NSEC_DEFAULT 0x0

struct ptp_rx_ts0_3_reg {
	a_uint16_t  rx_ts_nsec:16;
};

union ptp_rx_ts0_3_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts0_3_reg bf;
};

/*[register] PTP_RX_TS0_4_REG*/
#define PTP_RX_TS0_4_REG
#define PTP_RX_TS0_4_REG_ADDRESS 0x801d
#define PTP_RX_TS0_4_REG_NUM     1
#define PTP_RX_TS0_4_REG_INC     0x1
#define PTP_RX_TS0_4_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS0_4_REG_DEFAULT 0x0
	/*[field] RX_TS_NSEC*/
	#define PTP_RX_TS0_4_REG_RX_TS_NSEC
	#define PTP_RX_TS0_4_REG_RX_TS_NSEC_OFFSET  0
	#define PTP_RX_TS0_4_REG_RX_TS_NSEC_LEN     16
	#define PTP_RX_TS0_4_REG_RX_TS_NSEC_DEFAULT 0x0

struct ptp_rx_ts0_4_reg {
	a_uint16_t  rx_ts_nsec:16;
};

union ptp_rx_ts0_4_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts0_4_reg bf;
};

/*[register] PTP_RX_TS0_5_REG*/
#define PTP_RX_TS0_5_REG
#define PTP_RX_TS0_5_REG_ADDRESS 0x801e
#define PTP_RX_TS0_5_REG_NUM     1
#define PTP_RX_TS0_5_REG_INC     0x1
#define PTP_RX_TS0_5_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS0_5_REG_DEFAULT 0x0
	/*[field] RX_TS_NFSEC*/
	#define PTP_RX_TS0_5_REG_RX_TS_NFSEC
	#define PTP_RX_TS0_5_REG_RX_TS_NFSEC_OFFSET  0
	#define PTP_RX_TS0_5_REG_RX_TS_NFSEC_LEN     12
	#define PTP_RX_TS0_5_REG_RX_TS_NFSEC_DEFAULT 0x0
	/*[field] RX_MSG_TYPE*/
	#define PTP_RX_TS0_5_REG_RX_MSG_TYPE
	#define PTP_RX_TS0_5_REG_RX_MSG_TYPE_OFFSET  12
	#define PTP_RX_TS0_5_REG_RX_MSG_TYPE_LEN     4
	#define PTP_RX_TS0_5_REG_RX_MSG_TYPE_DEFAULT 0x0

struct ptp_rx_ts0_5_reg {
	a_uint16_t  rx_ts_nfsec:12;
	a_uint16_t  rx_msg_type:4;
};

union ptp_rx_ts0_5_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts0_5_reg bf;
};

/*[register] PTP_RX_TS0_6_REG*/
#define PTP_RX_TS0_6_REG
#define PTP_RX_TS0_6_REG_ADDRESS 0x801f
#define PTP_RX_TS0_6_REG_NUM     1
#define PTP_RX_TS0_6_REG_INC     0x1
#define PTP_RX_TS0_6_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS0_6_REG_DEFAULT 0x0
	/*[field] RX_TS_NFSEC*/
	#define PTP_RX_TS0_6_REG_RX_TS_NFSEC
	#define PTP_RX_TS0_6_REG_RX_TS_NFSEC_OFFSET  0
	#define PTP_RX_TS0_6_REG_RX_TS_NFSEC_LEN     8
	#define PTP_RX_TS0_6_REG_RX_TS_NFSEC_DEFAULT 0x0

struct ptp_rx_ts0_6_reg {
	a_uint16_t  rx_ts_nfsec:8;
};

union ptp_rx_ts0_6_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts0_6_reg bf;
};

/*[register] PTP_TX_SEQID_REG*/
#define PTP_TX_SEQID_REG
#define PTP_TX_SEQID_REG_ADDRESS 0x8020
#define PTP_TX_SEQID_REG_NUM     1
#define PTP_TX_SEQID_REG_INC     0x1
#define PTP_TX_SEQID_REG_TYPE    REG_TYPE_RW
#define PTP_TX_SEQID_REG_DEFAULT 0x0
	/*[field] TX_SEQID*/
	#define PTP_TX_SEQID_REG_TX_SEQID
	#define PTP_TX_SEQID_REG_TX_SEQID_OFFSET  0
	#define PTP_TX_SEQID_REG_TX_SEQID_LEN     16
	#define PTP_TX_SEQID_REG_TX_SEQID_DEFAULT 0x0

struct ptp_tx_seqid_reg {
	a_uint16_t  tx_seqid:16;
};

union ptp_tx_seqid_reg_u {
	a_uint32_t val;
	struct ptp_tx_seqid_reg bf;
};

/*[register] PTP_TX_PORTID0_REG*/
#define PTP_TX_PORTID0_REG
#define PTP_TX_PORTID0_REG_ADDRESS 0x8021
#define PTP_TX_PORTID0_REG_NUM     1
#define PTP_TX_PORTID0_REG_INC     0x1
#define PTP_TX_PORTID0_REG_TYPE    REG_TYPE_RW
#define PTP_TX_PORTID0_REG_DEFAULT 0x0
	/*[field] TX_PORTID*/
	#define PTP_TX_PORTID0_REG_TX_PORTID
	#define PTP_TX_PORTID0_REG_TX_PORTID_OFFSET  0
	#define PTP_TX_PORTID0_REG_TX_PORTID_LEN     16
	#define PTP_TX_PORTID0_REG_TX_PORTID_DEFAULT 0x0

struct ptp_tx_portid0_reg {
	a_uint16_t  tx_portid:16;
};

union ptp_tx_portid0_reg_u {
	a_uint32_t val;
	struct ptp_tx_portid0_reg bf;
};

/*[register] PTP_TX_PORTID1_REG*/
#define PTP_TX_PORTID1_REG
#define PTP_TX_PORTID1_REG_ADDRESS 0x8022
#define PTP_TX_PORTID1_REG_NUM     1
#define PTP_TX_PORTID1_REG_INC     0x1
#define PTP_TX_PORTID1_REG_TYPE    REG_TYPE_RW
#define PTP_TX_PORTID1_REG_DEFAULT 0x0
	/*[field] TX_PORTID*/
	#define PTP_TX_PORTID1_REG_TX_PORTID
	#define PTP_TX_PORTID1_REG_TX_PORTID_OFFSET  0
	#define PTP_TX_PORTID1_REG_TX_PORTID_LEN     16
	#define PTP_TX_PORTID1_REG_TX_PORTID_DEFAULT 0x0

struct ptp_tx_portid1_reg {
	a_uint16_t  tx_portid:16;
};

union ptp_tx_portid1_reg_u {
	a_uint32_t val;
	struct ptp_tx_portid1_reg bf;
};

/*[register] PTP_TX_PORTID2_REG*/
#define PTP_TX_PORTID2_REG
#define PTP_TX_PORTID2_REG_ADDRESS 0x8023
#define PTP_TX_PORTID2_REG_NUM     1
#define PTP_TX_PORTID2_REG_INC     0x1
#define PTP_TX_PORTID2_REG_TYPE    REG_TYPE_RW
#define PTP_TX_PORTID2_REG_DEFAULT 0x0
	/*[field] TX_PORTID*/
	#define PTP_TX_PORTID2_REG_TX_PORTID
	#define PTP_TX_PORTID2_REG_TX_PORTID_OFFSET  0
	#define PTP_TX_PORTID2_REG_TX_PORTID_LEN     16
	#define PTP_TX_PORTID2_REG_TX_PORTID_DEFAULT 0x0

struct ptp_tx_portid2_reg {
	a_uint16_t  tx_portid:16;
};

union ptp_tx_portid2_reg_u {
	a_uint32_t val;
	struct ptp_tx_portid2_reg bf;
};

/*[register] PTP_TX_PORTID3_REG*/
#define PTP_TX_PORTID3_REG
#define PTP_TX_PORTID3_REG_ADDRESS 0x8024
#define PTP_TX_PORTID3_REG_NUM     1
#define PTP_TX_PORTID3_REG_INC     0x1
#define PTP_TX_PORTID3_REG_TYPE    REG_TYPE_RW
#define PTP_TX_PORTID3_REG_DEFAULT 0x0
	/*[field] TX_PORTID*/
	#define PTP_TX_PORTID3_REG_TX_PORTID
	#define PTP_TX_PORTID3_REG_TX_PORTID_OFFSET  0
	#define PTP_TX_PORTID3_REG_TX_PORTID_LEN     16
	#define PTP_TX_PORTID3_REG_TX_PORTID_DEFAULT 0x0

struct ptp_tx_portid3_reg {
	a_uint16_t  tx_portid:16;
};

union ptp_tx_portid3_reg_u {
	a_uint32_t val;
	struct ptp_tx_portid3_reg bf;
};

/*[register] PTP_TX_PORTID4_REG*/
#define PTP_TX_PORTID4_REG
#define PTP_TX_PORTID4_REG_ADDRESS 0x8025
#define PTP_TX_PORTID4_REG_NUM     1
#define PTP_TX_PORTID4_REG_INC     0x1
#define PTP_TX_PORTID4_REG_TYPE    REG_TYPE_RW
#define PTP_TX_PORTID4_REG_DEFAULT 0x0
	/*[field] TX_PORTID*/
	#define PTP_TX_PORTID4_REG_TX_PORTID
	#define PTP_TX_PORTID4_REG_TX_PORTID_OFFSET  0
	#define PTP_TX_PORTID4_REG_TX_PORTID_LEN     16
	#define PTP_TX_PORTID4_REG_TX_PORTID_DEFAULT 0x0

struct ptp_tx_portid4_reg {
	a_uint16_t  tx_portid:16;
};

union ptp_tx_portid4_reg_u {
	a_uint32_t val;
	struct ptp_tx_portid4_reg bf;
};

/*[register] PTP_TX_TS0_REG*/
#define PTP_TX_TS0_REG
#define PTP_TX_TS0_REG_ADDRESS 0x8026
#define PTP_TX_TS0_REG_NUM     1
#define PTP_TX_TS0_REG_INC     0x1
#define PTP_TX_TS0_REG_TYPE    REG_TYPE_RW
#define PTP_TX_TS0_REG_DEFAULT 0x0
	/*[field] TX_TS_SEC*/
	#define PTP_TX_TS0_REG_TX_TS_SEC
	#define PTP_TX_TS0_REG_TX_TS_SEC_OFFSET  0
	#define PTP_TX_TS0_REG_TX_TS_SEC_LEN     16
	#define PTP_TX_TS0_REG_TX_TS_SEC_DEFAULT 0x0

struct ptp_tx_ts0_reg {
	a_uint16_t  tx_ts_sec:16;
};

union ptp_tx_ts0_reg_u {
	a_uint32_t val;
	struct ptp_tx_ts0_reg bf;
};

/*[register] PTP_TX_TS1_REG*/
#define PTP_TX_TS1_REG
#define PTP_TX_TS1_REG_ADDRESS 0x8027
#define PTP_TX_TS1_REG_NUM     1
#define PTP_TX_TS1_REG_INC     0x1
#define PTP_TX_TS1_REG_TYPE    REG_TYPE_RW
#define PTP_TX_TS1_REG_DEFAULT 0x0
	/*[field] TX_TS_SEC*/
	#define PTP_TX_TS1_REG_TX_TS_SEC
	#define PTP_TX_TS1_REG_TX_TS_SEC_OFFSET  0
	#define PTP_TX_TS1_REG_TX_TS_SEC_LEN     16
	#define PTP_TX_TS1_REG_TX_TS_SEC_DEFAULT 0x0

struct ptp_tx_ts1_reg {
	a_uint16_t  tx_ts_sec:16;
};

union ptp_tx_ts1_reg_u {
	a_uint32_t val;
	struct ptp_tx_ts1_reg bf;
};

/*[register] PTP_TX_TS2_REG*/
#define PTP_TX_TS2_REG
#define PTP_TX_TS2_REG_ADDRESS 0x8028
#define PTP_TX_TS2_REG_NUM     1
#define PTP_TX_TS2_REG_INC     0x1
#define PTP_TX_TS2_REG_TYPE    REG_TYPE_RW
#define PTP_TX_TS2_REG_DEFAULT 0x0
	/*[field] TX_TS_SEC*/
	#define PTP_TX_TS2_REG_TX_TS_SEC
	#define PTP_TX_TS2_REG_TX_TS_SEC_OFFSET  0
	#define PTP_TX_TS2_REG_TX_TS_SEC_LEN     16
	#define PTP_TX_TS2_REG_TX_TS_SEC_DEFAULT 0x0

struct ptp_tx_ts2_reg {
	a_uint16_t  tx_ts_sec:16;
};

union ptp_tx_ts2_reg_u {
	a_uint32_t val;
	struct ptp_tx_ts2_reg bf;
};

/*[register] PTP_TX_TS3_REG*/
#define PTP_TX_TS3_REG
#define PTP_TX_TS3_REG_ADDRESS 0x8029
#define PTP_TX_TS3_REG_NUM     1
#define PTP_TX_TS3_REG_INC     0x1
#define PTP_TX_TS3_REG_TYPE    REG_TYPE_RW
#define PTP_TX_TS3_REG_DEFAULT 0x0
	/*[field] TX_TS_NSEC*/
	#define PTP_TX_TS3_REG_TX_TS_NSEC
	#define PTP_TX_TS3_REG_TX_TS_NSEC_OFFSET  0
	#define PTP_TX_TS3_REG_TX_TS_NSEC_LEN     16
	#define PTP_TX_TS3_REG_TX_TS_NSEC_DEFAULT 0x0

struct ptp_tx_ts3_reg {
	a_uint16_t  tx_ts_nsec:16;
};

union ptp_tx_ts3_reg_u {
	a_uint32_t val;
	struct ptp_tx_ts3_reg bf;
};

/*[register] PTP_TX_TS4_REG*/
#define PTP_TX_TS4_REG
#define PTP_TX_TS4_REG_ADDRESS 0x802a
#define PTP_TX_TS4_REG_NUM     1
#define PTP_TX_TS4_REG_INC     0x1
#define PTP_TX_TS4_REG_TYPE    REG_TYPE_RW
#define PTP_TX_TS4_REG_DEFAULT 0x0
	/*[field] TX_TS_NSEC*/
	#define PTP_TX_TS4_REG_TX_TS_NSEC
	#define PTP_TX_TS4_REG_TX_TS_NSEC_OFFSET  0
	#define PTP_TX_TS4_REG_TX_TS_NSEC_LEN     16
	#define PTP_TX_TS4_REG_TX_TS_NSEC_DEFAULT 0x0

struct ptp_tx_ts4_reg {
	a_uint16_t  tx_ts_nsec:16;
};

union ptp_tx_ts4_reg_u {
	a_uint32_t val;
	struct ptp_tx_ts4_reg bf;
};

/*[register] PTP_TX_TS5_REG*/
#define PTP_TX_TS5_REG
#define PTP_TX_TS5_REG_ADDRESS 0x802b
#define PTP_TX_TS5_REG_NUM     1
#define PTP_TX_TS5_REG_INC     0x1
#define PTP_TX_TS5_REG_TYPE    REG_TYPE_RW
#define PTP_TX_TS5_REG_DEFAULT 0x0
	/*[field] TX_TS_NFSEC*/
	#define PTP_TX_TS5_REG_TX_TS_NFSEC
	#define PTP_TX_TS5_REG_TX_TS_NFSEC_OFFSET  0
	#define PTP_TX_TS5_REG_TX_TS_NFSEC_LEN     12
	#define PTP_TX_TS5_REG_TX_TS_NFSEC_DEFAULT 0x0
	/*[field] TX_MSG_TYPE*/
	#define PTP_TX_TS5_REG_TX_MSG_TYPE
	#define PTP_TX_TS5_REG_TX_MSG_TYPE_OFFSET  12
	#define PTP_TX_TS5_REG_TX_MSG_TYPE_LEN     4
	#define PTP_TX_TS5_REG_TX_MSG_TYPE_DEFAULT 0x0

struct ptp_tx_ts5_reg {
	a_uint16_t  tx_ts_nfsec:12;
	a_uint16_t  tx_msg_type:4;
};

union ptp_tx_ts5_reg_u {
	a_uint32_t val;
	struct ptp_tx_ts5_reg bf;
};

/*[register] PTP_TX_TS6_REG*/
#define PTP_TX_TS6_REG
#define PTP_TX_TS6_REG_ADDRESS 0x802c
#define PTP_TX_TS6_REG_NUM     1
#define PTP_TX_TS6_REG_INC     0x1
#define PTP_TX_TS6_REG_TYPE    REG_TYPE_RW
#define PTP_TX_TS6_REG_DEFAULT 0x0
	/*[field] TX_TS_NFSEC*/
	#define PTP_TX_TS6_REG_TX_TS_NFSEC
	#define PTP_TX_TS6_REG_TX_TS_NFSEC_OFFSET  0
	#define PTP_TX_TS6_REG_TX_TS_NFSEC_LEN     8
	#define PTP_TX_TS6_REG_TX_TS_NFSEC_DEFAULT 0x0

struct ptp_tx_ts6_reg {
	a_uint16_t  tx_ts_nfsec:8;
};

union ptp_tx_ts6_reg_u {
	a_uint32_t val;
	struct ptp_tx_ts6_reg bf;
};

/*[register] PTP_ORIG_CORR0_REG*/
#define PTP_ORIG_CORR0_REG
#define PTP_ORIG_CORR0_REG_ADDRESS 0x802d
#define PTP_ORIG_CORR0_REG_NUM     1
#define PTP_ORIG_CORR0_REG_INC     0x1
#define PTP_ORIG_CORR0_REG_TYPE    REG_TYPE_RW
#define PTP_ORIG_CORR0_REG_DEFAULT 0x0
	/*[field] PTP_ORIG_CORR*/
	#define PTP_ORIG_CORR0_REG_PTP_ORIG_CORR
	#define PTP_ORIG_CORR0_REG_PTP_ORIG_CORR_OFFSET  0
	#define PTP_ORIG_CORR0_REG_PTP_ORIG_CORR_LEN     16
	#define PTP_ORIG_CORR0_REG_PTP_ORIG_CORR_DEFAULT 0x0

struct ptp_orig_corr0_reg {
	a_uint16_t  ptp_orig_corr:16;
};

union ptp_orig_corr0_reg_u {
	a_uint32_t val;
	struct ptp_orig_corr0_reg bf;
};

/*[register] PTP_ORIG_CORR1_REG*/
#define PTP_ORIG_CORR1_REG
#define PTP_ORIG_CORR1_REG_ADDRESS 0x802e
#define PTP_ORIG_CORR1_REG_NUM     1
#define PTP_ORIG_CORR1_REG_INC     0x1
#define PTP_ORIG_CORR1_REG_TYPE    REG_TYPE_RW
#define PTP_ORIG_CORR1_REG_DEFAULT 0x0
	/*[field] PTP_ORIG_CORR*/
	#define PTP_ORIG_CORR1_REG_PTP_ORIG_CORR
	#define PTP_ORIG_CORR1_REG_PTP_ORIG_CORR_OFFSET  0
	#define PTP_ORIG_CORR1_REG_PTP_ORIG_CORR_LEN     16
	#define PTP_ORIG_CORR1_REG_PTP_ORIG_CORR_DEFAULT 0x0

struct ptp_orig_corr1_reg {
	a_uint16_t  ptp_orig_corr:16;
};

union ptp_orig_corr1_reg_u {
	a_uint32_t val;
	struct ptp_orig_corr1_reg bf;
};

/*[register] PTP_ORIG_CORR2_REG*/
#define PTP_ORIG_CORR2_REG
#define PTP_ORIG_CORR2_REG_ADDRESS 0x802f
#define PTP_ORIG_CORR2_REG_NUM     1
#define PTP_ORIG_CORR2_REG_INC     0x1
#define PTP_ORIG_CORR2_REG_TYPE    REG_TYPE_RW
#define PTP_ORIG_CORR2_REG_DEFAULT 0x0
	/*[field] PTP_ORIG_CORR*/
	#define PTP_ORIG_CORR2_REG_PTP_ORIG_CORR
	#define PTP_ORIG_CORR2_REG_PTP_ORIG_CORR_OFFSET  0
	#define PTP_ORIG_CORR2_REG_PTP_ORIG_CORR_LEN     16
	#define PTP_ORIG_CORR2_REG_PTP_ORIG_CORR_DEFAULT 0x0

struct ptp_orig_corr2_reg {
	a_uint16_t  ptp_orig_corr:16;
};

union ptp_orig_corr2_reg_u {
	a_uint32_t val;
	struct ptp_orig_corr2_reg bf;
};

/*[register] PTP_ORIG_CORR3_REG*/
#define PTP_ORIG_CORR3_REG
#define PTP_ORIG_CORR3_REG_ADDRESS 0x8030
#define PTP_ORIG_CORR3_REG_NUM     1
#define PTP_ORIG_CORR3_REG_INC     0x1
#define PTP_ORIG_CORR3_REG_TYPE    REG_TYPE_RW
#define PTP_ORIG_CORR3_REG_DEFAULT 0x0
	/*[field] PTP_ORIG_CORR*/
	#define PTP_ORIG_CORR3_REG_PTP_ORIG_CORR
	#define PTP_ORIG_CORR3_REG_PTP_ORIG_CORR_OFFSET  0
	#define PTP_ORIG_CORR3_REG_PTP_ORIG_CORR_LEN     16
	#define PTP_ORIG_CORR3_REG_PTP_ORIG_CORR_DEFAULT 0x0

struct ptp_orig_corr3_reg {
	a_uint16_t  ptp_orig_corr:16;
};

union ptp_orig_corr3_reg_u {
	a_uint32_t val;
	struct ptp_orig_corr3_reg bf;
};

/*[register] PTP_IN_TRIG0_REG*/
#define PTP_IN_TRIG0_REG
#define PTP_IN_TRIG0_REG_ADDRESS 0x8031
#define PTP_IN_TRIG0_REG_NUM     1
#define PTP_IN_TRIG0_REG_INC     0x1
#define PTP_IN_TRIG0_REG_TYPE    REG_TYPE_RW
#define PTP_IN_TRIG0_REG_DEFAULT 0x0
	/*[field] PTP_IN_TRIG_NISEC*/
	#define PTP_IN_TRIG0_REG_PTP_IN_TRIG_NISEC
	#define PTP_IN_TRIG0_REG_PTP_IN_TRIG_NISEC_OFFSET  0
	#define PTP_IN_TRIG0_REG_PTP_IN_TRIG_NISEC_LEN     16
	#define PTP_IN_TRIG0_REG_PTP_IN_TRIG_NISEC_DEFAULT 0x0

struct ptp_in_trig0_reg {
	a_uint16_t  ptp_in_trig_nisec:16;
};

union ptp_in_trig0_reg_u {
	a_uint32_t val;
	struct ptp_in_trig0_reg bf;
};

/*[register] PTP_IN_TRIG1_REG*/
#define PTP_IN_TRIG1_REG
#define PTP_IN_TRIG1_REG_ADDRESS 0x8032
#define PTP_IN_TRIG1_REG_NUM     1
#define PTP_IN_TRIG1_REG_INC     0x1
#define PTP_IN_TRIG1_REG_TYPE    REG_TYPE_RW
#define PTP_IN_TRIG1_REG_DEFAULT 0x0
	/*[field] PTP_IN_TRIG_NISEC*/
	#define PTP_IN_TRIG1_REG_PTP_IN_TRIG_NISEC
	#define PTP_IN_TRIG1_REG_PTP_IN_TRIG_NISEC_OFFSET  0
	#define PTP_IN_TRIG1_REG_PTP_IN_TRIG_NISEC_LEN     16
	#define PTP_IN_TRIG1_REG_PTP_IN_TRIG_NISEC_DEFAULT 0x0

struct ptp_in_trig1_reg {
	a_uint16_t  ptp_in_trig_nisec:16;
};

union ptp_in_trig1_reg_u {
	a_uint32_t val;
	struct ptp_in_trig1_reg bf;
};

/*[register] PTP_IN_TRIG2_REG*/
#define PTP_IN_TRIG2_REG
#define PTP_IN_TRIG2_REG_ADDRESS 0x8033
#define PTP_IN_TRIG2_REG_NUM     1
#define PTP_IN_TRIG2_REG_INC     0x1
#define PTP_IN_TRIG2_REG_TYPE    REG_TYPE_RW
#define PTP_IN_TRIG2_REG_DEFAULT 0x0
	/*[field] PTP_IN_TRIG_NISEC*/
	#define PTP_IN_TRIG2_REG_PTP_IN_TRIG_NISEC
	#define PTP_IN_TRIG2_REG_PTP_IN_TRIG_NISEC_OFFSET  0
	#define PTP_IN_TRIG2_REG_PTP_IN_TRIG_NISEC_LEN     16
	#define PTP_IN_TRIG2_REG_PTP_IN_TRIG_NISEC_DEFAULT 0x0

struct ptp_in_trig2_reg {
	a_uint16_t  ptp_in_trig_nisec:16;
};

union ptp_in_trig2_reg_u {
	a_uint32_t val;
	struct ptp_in_trig2_reg bf;
};

/*[register] PTP_IN_TRIG3_REG*/
#define PTP_IN_TRIG3_REG
#define PTP_IN_TRIG3_REG_ADDRESS 0x8034
#define PTP_IN_TRIG3_REG_NUM     1
#define PTP_IN_TRIG3_REG_INC     0x1
#define PTP_IN_TRIG3_REG_TYPE    REG_TYPE_RW
#define PTP_IN_TRIG3_REG_DEFAULT 0x0
	/*[field] PTP_IN_TRIG_NISEC*/
	#define PTP_IN_TRIG3_REG_PTP_IN_TRIG_NISEC
	#define PTP_IN_TRIG3_REG_PTP_IN_TRIG_NISEC_OFFSET  0
	#define PTP_IN_TRIG3_REG_PTP_IN_TRIG_NISEC_LEN     4
	#define PTP_IN_TRIG3_REG_PTP_IN_TRIG_NISEC_DEFAULT 0x0

struct ptp_in_trig3_reg {
	a_uint16_t  ptp_in_trig_nisec:4;
};

union ptp_in_trig3_reg_u {
	a_uint32_t val;
	struct ptp_in_trig3_reg bf;
};

/*[register] PTP_TX_LATENCY_REG*/
#define PTP_TX_LATENCY_REG
#define PTP_TX_LATENCY_REG_ADDRESS 0x8035
#define PTP_TX_LATENCY_REG_NUM     1
#define PTP_TX_LATENCY_REG_INC     0x1
#define PTP_TX_LATENCY_REG_TYPE    REG_TYPE_RW
#define PTP_TX_LATENCY_REG_DEFAULT 0x0
	/*[field] PTP_TX_LATENCY*/
	#define PTP_TX_LATENCY_REG_PTP_TX_LATENCY
	#define PTP_TX_LATENCY_REG_PTP_TX_LATENCY_OFFSET  0
	#define PTP_TX_LATENCY_REG_PTP_TX_LATENCY_LEN     16
	#define PTP_TX_LATENCY_REG_PTP_TX_LATENCY_DEFAULT 0x0

struct ptp_tx_latency_reg {
	a_uint16_t  ptp_tx_latency:16;
};

union ptp_tx_latency_reg_u {
	a_uint32_t val;
	struct ptp_tx_latency_reg bf;
};

/*[register] PTP_RTC_INC0_REG*/
#define PTP_RTC_INC0_REG
#define PTP_RTC_INC0_REG_ADDRESS 0x8036
#define PTP_RTC_INC0_REG_NUM     1
#define PTP_RTC_INC0_REG_INC     0x1
#define PTP_RTC_INC0_REG_TYPE    REG_TYPE_RW
#define PTP_RTC_INC0_REG_DEFAULT 0x0
	/*[field] PTP_RTC_INC_NFS*/
	#define PTP_RTC_INC0_REG_PTP_RTC_INC_NFS
	#define PTP_RTC_INC0_REG_PTP_RTC_INC_NFS_OFFSET  0
	#define PTP_RTC_INC0_REG_PTP_RTC_INC_NFS_LEN     10
	#define PTP_RTC_INC0_REG_PTP_RTC_INC_NFS_DEFAULT 0x0
	/*[field] PTP_RTC_INC_NIS*/
	#define PTP_RTC_INC0_REG_PTP_RTC_INC_NIS
	#define PTP_RTC_INC0_REG_PTP_RTC_INC_NIS_OFFSET  10
	#define PTP_RTC_INC0_REG_PTP_RTC_INC_NIS_LEN     6
	#define PTP_RTC_INC0_REG_PTP_RTC_INC_NIS_DEFAULT 0x0

struct ptp_rtc_inc0_reg {
	a_uint16_t  ptp_rtc_inc_nfs:10;
	a_uint16_t  ptp_rtc_inc_nis:6;
};

union ptp_rtc_inc0_reg_u {
	a_uint32_t val;
	struct ptp_rtc_inc0_reg bf;
};

/*[register] PTP_RTC_INC1_REG*/
#define PTP_RTC_INC1_REG
#define PTP_RTC_INC1_REG_ADDRESS 0x8037
#define PTP_RTC_INC1_REG_NUM     1
#define PTP_RTC_INC1_REG_INC     0x1
#define PTP_RTC_INC1_REG_TYPE    REG_TYPE_RW
#define PTP_RTC_INC1_REG_DEFAULT 0x0
	/*[field] PTP_RTC_INC_NFS*/
	#define PTP_RTC_INC1_REG_PTP_RTC_INC_NFS
	#define PTP_RTC_INC1_REG_PTP_RTC_INC_NFS_OFFSET  0
	#define PTP_RTC_INC1_REG_PTP_RTC_INC_NFS_LEN     16
	#define PTP_RTC_INC1_REG_PTP_RTC_INC_NFS_DEFAULT 0x0

struct ptp_rtc_inc1_reg {
	a_uint16_t  ptp_rtc_inc_nfs:16;
};

union ptp_rtc_inc1_reg_u {
	a_uint32_t val;
	struct ptp_rtc_inc1_reg bf;
};

/*[register] PTP_RTCOFFS0_REG*/
#define PTP_RTCOFFS0_REG
#define PTP_RTCOFFS0_REG_ADDRESS 0x8038
#define PTP_RTCOFFS0_REG_NUM     1
#define PTP_RTCOFFS0_REG_INC     0x1
#define PTP_RTCOFFS0_REG_TYPE    REG_TYPE_RW
#define PTP_RTCOFFS0_REG_DEFAULT 0x0
	/*[field] PTP_RTCOFFS_NSEC*/
	#define PTP_RTCOFFS0_REG_PTP_RTCOFFS_NSEC
	#define PTP_RTCOFFS0_REG_PTP_RTCOFFS_NSEC_OFFSET  0
	#define PTP_RTCOFFS0_REG_PTP_RTCOFFS_NSEC_LEN     16
	#define PTP_RTCOFFS0_REG_PTP_RTCOFFS_NSEC_DEFAULT 0x0

struct ptp_rtcoffs0_reg {
	a_uint16_t  ptp_rtcoffs_nsec:16;
};

union ptp_rtcoffs0_reg_u {
	a_uint32_t val;
	struct ptp_rtcoffs0_reg bf;
};

/*[register] PTP_RTCOFFS1_REG*/
#define PTP_RTCOFFS1_REG
#define PTP_RTCOFFS1_REG_ADDRESS 0x8039
#define PTP_RTCOFFS1_REG_NUM     1
#define PTP_RTCOFFS1_REG_INC     0x1
#define PTP_RTCOFFS1_REG_TYPE    REG_TYPE_RW
#define PTP_RTCOFFS1_REG_DEFAULT 0x0
	/*[field] PTP_RTCOFFS_NSEC*/
	#define PTP_RTCOFFS1_REG_PTP_RTCOFFS_NSEC
	#define PTP_RTCOFFS1_REG_PTP_RTCOFFS_NSEC_OFFSET  0
	#define PTP_RTCOFFS1_REG_PTP_RTCOFFS_NSEC_LEN     16
	#define PTP_RTCOFFS1_REG_PTP_RTCOFFS_NSEC_DEFAULT 0x0

struct ptp_rtcoffs1_reg {
	a_uint16_t  ptp_rtcoffs_nsec:16;
};

union ptp_rtcoffs1_reg_u {
	a_uint32_t val;
	struct ptp_rtcoffs1_reg bf;
};

/*[register] PTP_RTCOFFS2_REG*/
#define PTP_RTCOFFS2_REG
#define PTP_RTCOFFS2_REG_ADDRESS 0x803a
#define PTP_RTCOFFS2_REG_NUM     1
#define PTP_RTCOFFS2_REG_INC     0x1
#define PTP_RTCOFFS2_REG_TYPE    REG_TYPE_RW
#define PTP_RTCOFFS2_REG_DEFAULT 0x0
	/*[field] PTP_RTCOFFS_SEC*/
	#define PTP_RTCOFFS2_REG_PTP_RTCOFFS_SEC
	#define PTP_RTCOFFS2_REG_PTP_RTCOFFS_SEC_OFFSET  0
	#define PTP_RTCOFFS2_REG_PTP_RTCOFFS_SEC_LEN     16
	#define PTP_RTCOFFS2_REG_PTP_RTCOFFS_SEC_DEFAULT 0x0

struct ptp_rtcoffs2_reg {
	a_uint16_t  ptp_rtcoffs_sec:16;
};

union ptp_rtcoffs2_reg_u {
	a_uint32_t val;
	struct ptp_rtcoffs2_reg bf;
};

/*[register] PTP_RTCOFFS3_REG*/
#define PTP_RTCOFFS3_REG
#define PTP_RTCOFFS3_REG_ADDRESS 0x803b
#define PTP_RTCOFFS3_REG_NUM     1
#define PTP_RTCOFFS3_REG_INC     0x1
#define PTP_RTCOFFS3_REG_TYPE    REG_TYPE_RW
#define PTP_RTCOFFS3_REG_DEFAULT 0x0
	/*[field] PTP_RTCOFFS_SEC*/
	#define PTP_RTCOFFS3_REG_PTP_RTCOFFS_SEC
	#define PTP_RTCOFFS3_REG_PTP_RTCOFFS_SEC_OFFSET  0
	#define PTP_RTCOFFS3_REG_PTP_RTCOFFS_SEC_LEN     16
	#define PTP_RTCOFFS3_REG_PTP_RTCOFFS_SEC_DEFAULT 0x0

struct ptp_rtcoffs3_reg {
	a_uint16_t  ptp_rtcoffs_sec:16;
};

union ptp_rtcoffs3_reg_u {
	a_uint32_t val;
	struct ptp_rtcoffs3_reg bf;
};

/*[register] PTP_RTCOFFS4_REG*/
#define PTP_RTCOFFS4_REG
#define PTP_RTCOFFS4_REG_ADDRESS 0x803c
#define PTP_RTCOFFS4_REG_NUM     1
#define PTP_RTCOFFS4_REG_INC     0x1
#define PTP_RTCOFFS4_REG_TYPE    REG_TYPE_RW
#define PTP_RTCOFFS4_REG_DEFAULT 0x0
	/*[field] PTP_RTCOFFS_SEC*/
	#define PTP_RTCOFFS4_REG_PTP_RTCOFFS_SEC
	#define PTP_RTCOFFS4_REG_PTP_RTCOFFS_SEC_OFFSET  0
	#define PTP_RTCOFFS4_REG_PTP_RTCOFFS_SEC_LEN     16
	#define PTP_RTCOFFS4_REG_PTP_RTCOFFS_SEC_DEFAULT 0x0

struct ptp_rtcoffs4_reg {
	a_uint16_t  ptp_rtcoffs_sec:16;
};

union ptp_rtcoffs4_reg_u {
	a_uint32_t val;
	struct ptp_rtcoffs4_reg bf;
};

/*[register] PTP_RTC0_REG*/
#define PTP_RTC0_REG
#define PTP_RTC0_REG_ADDRESS 0x803d
#define PTP_RTC0_REG_NUM     1
#define PTP_RTC0_REG_INC     0x1
#define PTP_RTC0_REG_TYPE    REG_TYPE_RW
#define PTP_RTC0_REG_DEFAULT 0x0
	/*[field] PTP_RTC_SEC*/
	#define PTP_RTC0_REG_PTP_RTC_SEC
	#define PTP_RTC0_REG_PTP_RTC_SEC_OFFSET  0
	#define PTP_RTC0_REG_PTP_RTC_SEC_LEN     16
	#define PTP_RTC0_REG_PTP_RTC_SEC_DEFAULT 0x0

struct ptp_rtc0_reg {
	a_uint16_t  ptp_rtc_sec:16;
};

union ptp_rtc0_reg_u {
	a_uint32_t val;
	struct ptp_rtc0_reg bf;
};

/*[register] PTP_RTC1_REG*/
#define PTP_RTC1_REG
#define PTP_RTC1_REG_ADDRESS 0x803e
#define PTP_RTC1_REG_NUM     1
#define PTP_RTC1_REG_INC     0x1
#define PTP_RTC1_REG_TYPE    REG_TYPE_RW
#define PTP_RTC1_REG_DEFAULT 0x0
	/*[field] PTP_RTC_SEC*/
	#define PTP_RTC1_REG_PTP_RTC_SEC
	#define PTP_RTC1_REG_PTP_RTC_SEC_OFFSET  0
	#define PTP_RTC1_REG_PTP_RTC_SEC_LEN     16
	#define PTP_RTC1_REG_PTP_RTC_SEC_DEFAULT 0x0

struct ptp_rtc1_reg {
	a_uint16_t  ptp_rtc_sec:16;
};

union ptp_rtc1_reg_u {
	a_uint32_t val;
	struct ptp_rtc1_reg bf;
};

/*[register] PTP_RTC2_REG*/
#define PTP_RTC2_REG
#define PTP_RTC2_REG_ADDRESS 0x803f
#define PTP_RTC2_REG_NUM     1
#define PTP_RTC2_REG_INC     0x1
#define PTP_RTC2_REG_TYPE    REG_TYPE_RW
#define PTP_RTC2_REG_DEFAULT 0x0
	/*[field] PTP_RTC_SEC*/
	#define PTP_RTC2_REG_PTP_RTC_SEC
	#define PTP_RTC2_REG_PTP_RTC_SEC_OFFSET  0
	#define PTP_RTC2_REG_PTP_RTC_SEC_LEN     16
	#define PTP_RTC2_REG_PTP_RTC_SEC_DEFAULT 0x0

struct ptp_rtc2_reg {
	a_uint16_t  ptp_rtc_sec:16;
};

union ptp_rtc2_reg_u {
	a_uint32_t val;
	struct ptp_rtc2_reg bf;
};

/*[register] PTP_RTC3_REG*/
#define PTP_RTC3_REG
#define PTP_RTC3_REG_ADDRESS 0x8040
#define PTP_RTC3_REG_NUM     1
#define PTP_RTC3_REG_INC     0x1
#define PTP_RTC3_REG_TYPE    REG_TYPE_RW
#define PTP_RTC3_REG_DEFAULT 0x0
	/*[field] PTP_RTC_NISEC*/
	#define PTP_RTC3_REG_PTP_RTC_NISEC
	#define PTP_RTC3_REG_PTP_RTC_NISEC_OFFSET  0
	#define PTP_RTC3_REG_PTP_RTC_NISEC_LEN     16
	#define PTP_RTC3_REG_PTP_RTC_NISEC_DEFAULT 0x0

struct ptp_rtc3_reg {
	a_uint16_t  ptp_rtc_nisec:16;
};

union ptp_rtc3_reg_u {
	a_uint32_t val;
	struct ptp_rtc3_reg bf;
};

/*[register] PTP_RTC4_REG*/
#define PTP_RTC4_REG
#define PTP_RTC4_REG_ADDRESS 0x8041
#define PTP_RTC4_REG_NUM     1
#define PTP_RTC4_REG_INC     0x1
#define PTP_RTC4_REG_TYPE    REG_TYPE_RW
#define PTP_RTC4_REG_DEFAULT 0x0
	/*[field] PTP_RTC_NISEC*/
	#define PTP_RTC4_REG_PTP_RTC_NISEC
	#define PTP_RTC4_REG_PTP_RTC_NISEC_OFFSET  0
	#define PTP_RTC4_REG_PTP_RTC_NISEC_LEN     16
	#define PTP_RTC4_REG_PTP_RTC_NISEC_DEFAULT 0x0

struct ptp_rtc4_reg {
	a_uint16_t  ptp_rtc_nisec:16;
};

union ptp_rtc4_reg_u {
	a_uint32_t val;
	struct ptp_rtc4_reg bf;
};

/*[register] PTP_RTC5_REG*/
#define PTP_RTC5_REG
#define PTP_RTC5_REG_ADDRESS 0x8042
#define PTP_RTC5_REG_NUM     1
#define PTP_RTC5_REG_INC     0x1
#define PTP_RTC5_REG_TYPE    REG_TYPE_RW
#define PTP_RTC5_REG_DEFAULT 0x0
	/*[field] PTP_RTC_NFSEC*/
	#define PTP_RTC5_REG_PTP_RTC_NFSEC
	#define PTP_RTC5_REG_PTP_RTC_NFSEC_OFFSET  0
	#define PTP_RTC5_REG_PTP_RTC_NFSEC_LEN     16
	#define PTP_RTC5_REG_PTP_RTC_NFSEC_DEFAULT 0x0

struct ptp_rtc5_reg {
	a_uint16_t  ptp_rtc_nfsec:16;
};

union ptp_rtc5_reg_u {
	a_uint32_t val;
	struct ptp_rtc5_reg bf;
};

/*[register] PTP_RTC6_REG*/
#define PTP_RTC6_REG
#define PTP_RTC6_REG_ADDRESS 0x8043
#define PTP_RTC6_REG_NUM     1
#define PTP_RTC6_REG_INC     0x1
#define PTP_RTC6_REG_TYPE    REG_TYPE_RW
#define PTP_RTC6_REG_DEFAULT 0x0
	/*[field] PTP_RTC_NFSEC*/
	#define PTP_RTC6_REG_PTP_RTC_NFSEC
	#define PTP_RTC6_REG_PTP_RTC_NFSEC_OFFSET  0
	#define PTP_RTC6_REG_PTP_RTC_NFSEC_LEN     4
	#define PTP_RTC6_REG_PTP_RTC_NFSEC_DEFAULT 0x0

struct ptp_rtc6_reg {
	a_uint16_t  ptp_rtc_nfsec:4;
};

union ptp_rtc6_reg_u {
	a_uint32_t val;
	struct ptp_rtc6_reg bf;
};

/*[register] PTP_RTCOFFS_VALID_REG*/
#define PTP_RTCOFFS_VALID_REG
#define PTP_RTCOFFS_VALID_REG_ADDRESS 0x8044
#define PTP_RTCOFFS_VALID_REG_NUM     1
#define PTP_RTCOFFS_VALID_REG_INC     0x1
#define PTP_RTCOFFS_VALID_REG_TYPE    REG_TYPE_RW
#define PTP_RTCOFFS_VALID_REG_DEFAULT 0x0
	/*[field] PTP_RTCOFFS_VALID*/
	#define PTP_RTCOFFS_VALID_REG_PTP_RTCOFFS_VALID
	#define PTP_RTCOFFS_VALID_REG_PTP_RTCOFFS_VALID_OFFSET  0
	#define PTP_RTCOFFS_VALID_REG_PTP_RTCOFFS_VALID_LEN     1
	#define PTP_RTCOFFS_VALID_REG_PTP_RTCOFFS_VALID_DEFAULT 0x0

struct ptp_rtcoffs_valid_reg {
	a_uint16_t  ptp_rtcoffs_valid:1;
};

union ptp_rtcoffs_valid_reg_u {
	a_uint32_t val;
	struct ptp_rtcoffs_valid_reg bf;
};

/*[register] PTP_MISC_CONFIG_REG*/
#define PTP_MISC_CONFIG_REG
#define PTP_MISC_CONFIG_REG_ADDRESS 0x80f0
#define PTP_MISC_CONFIG_REG_NUM     1
#define PTP_MISC_CONFIG_REG_INC     0x1
#define PTP_MISC_CONFIG_REG_TYPE    REG_TYPE_RW
#define PTP_MISC_CONFIG_REG_DEFAULT 0x4
	/*[field] IPV6_UDP_CHK_EN*/
	#define PTP_MISC_CONFIG_REG_IPV6_UDP_CHK_EN
	#define PTP_MISC_CONFIG_REG_IPV6_UDP_CHK_EN_OFFSET  0
	#define PTP_MISC_CONFIG_REG_IPV6_UDP_CHK_EN_LEN     1
	#define PTP_MISC_CONFIG_REG_IPV6_UDP_CHK_EN_DEFAULT 0x0
	/*[field] PTP_VERSION*/
	#define PTP_MISC_CONFIG_REG_PTP_VERSION
	#define PTP_MISC_CONFIG_REG_PTP_VERSION_OFFSET  1
	#define PTP_MISC_CONFIG_REG_PTP_VERSION_LEN     4
	#define PTP_MISC_CONFIG_REG_PTP_VERSION_DEFAULT 0x2
	/*[field] PTP_VER_CHK_EN*/
	#define PTP_MISC_CONFIG_REG_PTP_VER_CHK_EN
	#define PTP_MISC_CONFIG_REG_PTP_VER_CHK_EN_OFFSET  5
	#define PTP_MISC_CONFIG_REG_PTP_VER_CHK_EN_LEN     1
	#define PTP_MISC_CONFIG_REG_PTP_VER_CHK_EN_DEFAULT 0x0
	/*[field] PTP_ADDR_CHK_EN*/
	#define PTP_MISC_CONFIG_REG_PTP_ADDR_CHK_EN
	#define PTP_MISC_CONFIG_REG_PTP_ADDR_CHK_EN_OFFSET  6
	#define PTP_MISC_CONFIG_REG_PTP_ADDR_CHK_EN_LEN     1
	#define PTP_MISC_CONFIG_REG_PTP_ADDR_CHK_EN_DEFAULT 0x0
	/*[field] CRC_VALIDATE_EN*/
	#define PTP_MISC_CONFIG_REG_CRC_VALIDATE_EN
	#define PTP_MISC_CONFIG_REG_CRC_VALIDATE_EN_OFFSET  7
	#define PTP_MISC_CONFIG_REG_CRC_VALIDATE_EN_LEN     1
	#define PTP_MISC_CONFIG_REG_CRC_VALIDATE_EN_DEFAULT 0x0
	/*[field] APPENDED_TIMESTAMP_SIZE*/
	#define PTP_MISC_CONFIG_REG_APPENDED_TIMESTAMP_SIZE
	#define PTP_MISC_CONFIG_REG_APPENDED_TIMESTAMP_SIZE_OFFSET  8
	#define PTP_MISC_CONFIG_REG_APPENDED_TIMESTAMP_SIZE_LEN     2
	#define PTP_MISC_CONFIG_REG_APPENDED_TIMESTAMP_SIZE_DEFAULT 0x0
	/*[field] TS_RTC_SELECT*/
	#define PTP_MISC_CONFIG_REG_TS_RTC_SELECT
	#define PTP_MISC_CONFIG_REG_TS_RTC_SELECT_OFFSET  10
	#define PTP_MISC_CONFIG_REG_TS_RTC_SELECT_LEN     1
	#define PTP_MISC_CONFIG_REG_TS_RTC_SELECT_DEFAULT 0x0
	/*[field] PKT_ONE_STEP_EN*/
	#define PTP_MISC_CONFIG_REG_PKT_ONE_STEP_EN
	#define PTP_MISC_CONFIG_REG_PKT_ONE_STEP_EN_OFFSET  11
	#define PTP_MISC_CONFIG_REG_PKT_ONE_STEP_EN_LEN     1
	#define PTP_MISC_CONFIG_REG_PKT_ONE_STEP_EN_DEFAULT 0x0
	/*[field] CF_FROM_PKT_EN*/
	#define PTP_MISC_CONFIG_REG_CF_FROM_PKT_EN
	#define PTP_MISC_CONFIG_REG_CF_FROM_PKT_EN_OFFSET  12
	#define PTP_MISC_CONFIG_REG_CF_FROM_PKT_EN_LEN     1
	#define PTP_MISC_CONFIG_REG_CF_FROM_PKT_EN_DEFAULT 0x0
	/*[field] EMBED_INGRESS_TIME_EN*/
	#define PTP_MISC_CONFIG_REG_EMBED_INGRESS_TIME_EN
	#define PTP_MISC_CONFIG_REG_EMBED_INGRESS_TIME_EN_OFFSET  13
	#define PTP_MISC_CONFIG_REG_EMBED_INGRESS_TIME_EN_LEN     1
	#define PTP_MISC_CONFIG_REG_EMBED_INGRESS_TIME_EN_DEFAULT 0x0
	/*[field] DROP_NEXT_PREAMBLE_EN*/
	#define PTP_MISC_CONFIG_REG_DROP_NEXT_PREAMBLE_EN
	#define PTP_MISC_CONFIG_REG_DROP_NEXT_PREAMBLE_EN_OFFSET  14
	#define PTP_MISC_CONFIG_REG_DROP_NEXT_PREAMBLE_EN_LEN     1
	#define PTP_MISC_CONFIG_REG_DROP_NEXT_PREAMBLE_EN_DEFAULT 0x0
	/*[field] P2P_TC_OFFLOAD*/
	#define PTP_MISC_CONFIG_REG_P2P_TC_OFFLOAD
	#define PTP_MISC_CONFIG_REG_P2P_TC_OFFLOAD_OFFSET  15
	#define PTP_MISC_CONFIG_REG_P2P_TC_OFFLOAD_LEN     1
	#define PTP_MISC_CONFIG_REG_P2P_TC_OFFLOAD_DEFAULT 0x0

struct ptp_misc_config_reg {
	a_uint16_t  ipv6_udp_chk_en:1;
	a_uint16_t  ptp_version:4;
	a_uint16_t  ptp_ver_chk_en:1;
	a_uint16_t  ptp_addr_chk_en:1;
	a_uint16_t  crc_validate_en:1;
	a_uint16_t  appended_timestamp_size:2;
	a_uint16_t  ts_rtc_select:1;
	a_uint16_t  pkt_one_step_en:1;
	a_uint16_t  cf_from_pkt_en:1;
	a_uint16_t  embed_ingress_time_en:1;
	a_uint16_t  drop_next_preamble_en:1;
	a_uint16_t  tc_offload:1;
};

union ptp_misc_config_reg_u {
	a_uint32_t val;
	struct ptp_misc_config_reg bf;
};

/*[register] PTP_EXT_IMR_REG*/
#define PTP_EXT_IMR_REG
#define PTP_EXT_IMR_REG_ADDRESS 0x80f1
#define PTP_EXT_IMR_REG_NUM     1
#define PTP_EXT_IMR_REG_INC     0x1
#define PTP_EXT_IMR_REG_TYPE    REG_TYPE_RW
#define PTP_EXT_IMR_REG_DEFAULT 0x0
	/*[field] MASK_BMP*/
	#define PTP_EXT_IMR_REG_MASK_BMP
	#define PTP_EXT_IMR_REG_MASK_BMP_OFFSET  0
	#define PTP_EXT_IMR_REG_MASK_BMP_LEN     5
	#define PTP_EXT_IMR_REG_MASK_BMP_DEFAULT 0x0

struct ptp_ext_imr_reg {
	a_uint16_t  mask_bmp:16;
};

union ptp_ext_imr_reg_u {
	a_uint32_t val;
	struct ptp_ext_imr_reg bf;
};

/*[register] PTP_EXT_ISR_REG*/
#define PTP_EXT_ISR_REG
#define PTP_EXT_ISR_REG_ADDRESS 0x80f2
#define PTP_EXT_ISR_REG_NUM     1
#define PTP_EXT_ISR_REG_INC     0x1
#define PTP_EXT_ISR_REG_TYPE    REG_TYPE_RW
#define PTP_EXT_ISR_REG_DEFAULT 0x0
	/*[field] STATUS_BMP*/
	#define PTP_EXT_ISR_REG_STATUS_BMP
	#define PTP_EXT_ISR_REG_STATUS_BMP_OFFSET  0
	#define PTP_EXT_ISR_REG_STATUS_BMP_LEN     5
	#define PTP_EXT_ISR_REG_STATUS_BMP_DEFAULT 0x0

struct ptp_ext_isr_reg {
	a_uint16_t  status_bmp:16;
};

union ptp_ext_isr_reg_u {
	a_uint32_t val;
	struct ptp_ext_isr_reg bf;
};

/*[register] PTP_RTC_EXT_CONF_REG*/
#define PTP_RTC_EXT_CONF_REG
#define PTP_RTC_EXT_CONF_REG_ADDRESS 0x8100
#define PTP_RTC_EXT_CONF_REG_NUM     1
#define PTP_RTC_EXT_CONF_REG_INC     0x1
#define PTP_RTC_EXT_CONF_REG_TYPE    REG_TYPE_RW
#define PTP_RTC_EXT_CONF_REG_DEFAULT 0x0
	/*[field] LOAD_RTC*/
	#define PTP_RTC_EXT_CONF_REG_LOAD_RTC
	#define PTP_RTC_EXT_CONF_REG_LOAD_RTC_OFFSET  0
	#define PTP_RTC_EXT_CONF_REG_LOAD_RTC_LEN     1
	#define PTP_RTC_EXT_CONF_REG_LOAD_RTC_DEFAULT 0x0
	/*[field] CLEAR_RTC*/
	#define PTP_RTC_EXT_CONF_REG_CLEAR_RTC
	#define PTP_RTC_EXT_CONF_REG_CLEAR_RTC_OFFSET  1
	#define PTP_RTC_EXT_CONF_REG_CLEAR_RTC_LEN     1
	#define PTP_RTC_EXT_CONF_REG_CLEAR_RTC_DEFAULT 0x0
	/*[field] SET_INCVAL_VALID*/
	#define PTP_RTC_EXT_CONF_REG_SET_INCVAL_VALID
	#define PTP_RTC_EXT_CONF_REG_SET_INCVAL_VALID_OFFSET  2
	#define PTP_RTC_EXT_CONF_REG_SET_INCVAL_VALID_LEN     1
	#define PTP_RTC_EXT_CONF_REG_SET_INCVAL_VALID_DEFAULT 0x0
	/*[field] SET_INCVAL_MODE*/
	#define PTP_RTC_EXT_CONF_REG_SET_INCVAL_MODE
	#define PTP_RTC_EXT_CONF_REG_SET_INCVAL_MODE_OFFSET  3
	#define PTP_RTC_EXT_CONF_REG_SET_INCVAL_MODE_LEN     1
	#define PTP_RTC_EXT_CONF_REG_SET_INCVAL_MODE_DEFAULT 0x0
	/*[field] RTC_SNAPSHOT*/
	#define PTP_RTC_EXT_CONF_REG_RTC_SNAPSHOT
	#define PTP_RTC_EXT_CONF_REG_RTC_SNAPSHOT_OFFSET  4
	#define PTP_RTC_EXT_CONF_REG_RTC_SNAPSHOT_LEN     1
	#define PTP_RTC_EXT_CONF_REG_RTC_SNAPSHOT_DEFAULT 0x0
	/*[field] RTC_READ_MODE*/
	#define PTP_RTC_EXT_CONF_REG_RTC_READ_MODE
	#define PTP_RTC_EXT_CONF_REG_RTC_READ_MODE_OFFSET  5
	#define PTP_RTC_EXT_CONF_REG_RTC_READ_MODE_LEN     1
	#define PTP_RTC_EXT_CONF_REG_RTC_READ_MODE_DEFAULT 0x0
	/*[field] SELECT_OUTPUT_WAVEFORM*/
	#define PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM
	#define PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_OFFSET  6
	#define PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_LEN     3
	#define PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_DEFAULT 0x0

#define PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_FREQ       0
#define PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_PULSE_10MS 2
#define PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_TRIG0_GPIO 5
#define PTP_RTC_EXT_CONF_REG_SELECT_OUTPUT_WAVEFORM_RXTS_VALID 7

struct ptp_rtc_ext_conf_reg {
	a_uint16_t  load_rtc:1;
	a_uint16_t  clear_rtc:1;
	a_uint16_t  set_incval_valid:1;
	a_uint16_t  set_incval_mode:1;
	a_uint16_t  rtc_snapshot:1;
	a_uint16_t  rtc_read_mode:1;
	a_uint16_t  select_output_waveform:3;
};

union ptp_rtc_ext_conf_reg_u {
	a_uint32_t val;
	struct ptp_rtc_ext_conf_reg bf;
};

/*[register] PTP_RTC_PRELOADED0_REG*/
#define PTP_RTC_PRELOADED0_REG
#define PTP_RTC_PRELOADED0_REG_ADDRESS 0x8101
#define PTP_RTC_PRELOADED0_REG_NUM     1
#define PTP_RTC_PRELOADED0_REG_INC     0x1
#define PTP_RTC_PRELOADED0_REG_TYPE    REG_TYPE_RW
#define PTP_RTC_PRELOADED0_REG_DEFAULT 0x0
	/*[field] PTP_RTC_PRELOADED_SEC*/
	#define PTP_RTC_PRELOADED0_REG_PTP_RTC_PRELOADED_SEC
	#define PTP_RTC_PRELOADED0_REG_PTP_RTC_PRELOADED_SEC_OFFSET  0
	#define PTP_RTC_PRELOADED0_REG_PTP_RTC_PRELOADED_SEC_LEN     16
	#define PTP_RTC_PRELOADED0_REG_PTP_RTC_PRELOADED_SEC_DEFAULT 0x0

struct ptp_rtc_preloaded0_reg {
	a_uint16_t  ptp_rtc_preloaded_sec:16;
};

union ptp_rtc_preloaded0_reg_u {
	a_uint32_t val;
	struct ptp_rtc_preloaded0_reg bf;
};

/*[register] PTP_RTC_PRELOADED1_REG*/
#define PTP_RTC_PRELOADED1_REG
#define PTP_RTC_PRELOADED1_REG_ADDRESS 0x8102
#define PTP_RTC_PRELOADED1_REG_NUM     1
#define PTP_RTC_PRELOADED1_REG_INC     0x1
#define PTP_RTC_PRELOADED1_REG_TYPE    REG_TYPE_RW
#define PTP_RTC_PRELOADED1_REG_DEFAULT 0x0
	/*[field] PTP_RTC_PRELOADED_SEC*/
	#define PTP_RTC_PRELOADED1_REG_PTP_RTC_PRELOADED_SEC
	#define PTP_RTC_PRELOADED1_REG_PTP_RTC_PRELOADED_SEC_OFFSET  0
	#define PTP_RTC_PRELOADED1_REG_PTP_RTC_PRELOADED_SEC_LEN     16
	#define PTP_RTC_PRELOADED1_REG_PTP_RTC_PRELOADED_SEC_DEFAULT 0x0

struct ptp_rtc_preloaded1_reg {
	a_uint16_t  ptp_rtc_preloaded_sec:16;
};

union ptp_rtc_preloaded1_reg_u {
	a_uint32_t val;
	struct ptp_rtc_preloaded1_reg bf;
};

/*[register] PTP_RTC_PRELOADED2_REG*/
#define PTP_RTC_PRELOADED2_REG
#define PTP_RTC_PRELOADED2_REG_ADDRESS 0x8103
#define PTP_RTC_PRELOADED2_REG_NUM     1
#define PTP_RTC_PRELOADED2_REG_INC     0x1
#define PTP_RTC_PRELOADED2_REG_TYPE    REG_TYPE_RW
#define PTP_RTC_PRELOADED2_REG_DEFAULT 0x0
	/*[field] PTP_RTC_PRELOADED_SEC*/
	#define PTP_RTC_PRELOADED2_REG_PTP_RTC_PRELOADED_SEC
	#define PTP_RTC_PRELOADED2_REG_PTP_RTC_PRELOADED_SEC_OFFSET  0
	#define PTP_RTC_PRELOADED2_REG_PTP_RTC_PRELOADED_SEC_LEN     16
	#define PTP_RTC_PRELOADED2_REG_PTP_RTC_PRELOADED_SEC_DEFAULT 0x0

struct ptp_rtc_preloaded2_reg {
	a_uint16_t  ptp_rtc_preloaded_sec:16;
};

union ptp_rtc_preloaded2_reg_u {
	a_uint32_t val;
	struct ptp_rtc_preloaded2_reg bf;
};

/*[register] PTP_RTC_PRELOADED3_REG*/
#define PTP_RTC_PRELOADED3_REG
#define PTP_RTC_PRELOADED3_REG_ADDRESS 0x8104
#define PTP_RTC_PRELOADED3_REG_NUM     1
#define PTP_RTC_PRELOADED3_REG_INC     0x1
#define PTP_RTC_PRELOADED3_REG_TYPE    REG_TYPE_RW
#define PTP_RTC_PRELOADED3_REG_DEFAULT 0x0
	/*[field] PTP_RTC_PRELOADED_NISEC*/
	#define PTP_RTC_PRELOADED3_REG_PTP_RTC_PRELOADED_NISEC
	#define PTP_RTC_PRELOADED3_REG_PTP_RTC_PRELOADED_NISEC_OFFSET  0
	#define PTP_RTC_PRELOADED3_REG_PTP_RTC_PRELOADED_NISEC_LEN     16
	#define PTP_RTC_PRELOADED3_REG_PTP_RTC_PRELOADED_NISEC_DEFAULT 0x0

struct ptp_rtc_preloaded3_reg {
	a_uint16_t  ptp_rtc_preloaded_nisec:16;
};

union ptp_rtc_preloaded3_reg_u {
	a_uint32_t val;
	struct ptp_rtc_preloaded3_reg bf;
};

/*[register] PTP_RTC_PRELOADED4_REG*/
#define PTP_RTC_PRELOADED4_REG
#define PTP_RTC_PRELOADED4_REG_ADDRESS 0x8105
#define PTP_RTC_PRELOADED4_REG_NUM     1
#define PTP_RTC_PRELOADED4_REG_INC     0x1
#define PTP_RTC_PRELOADED4_REG_TYPE    REG_TYPE_RW
#define PTP_RTC_PRELOADED4_REG_DEFAULT 0x0
	/*[field] PTP_RTC_PRELOADED_NISEC*/
	#define PTP_RTC_PRELOADED4_REG_PTP_RTC_PRELOADED_NISEC
	#define PTP_RTC_PRELOADED4_REG_PTP_RTC_PRELOADED_NISEC_OFFSET  0
	#define PTP_RTC_PRELOADED4_REG_PTP_RTC_PRELOADED_NISEC_LEN     16
	#define PTP_RTC_PRELOADED4_REG_PTP_RTC_PRELOADED_NISEC_DEFAULT 0x0

struct ptp_rtc_preloaded4_reg {
	a_uint16_t  ptp_rtc_preloaded_nisec:16;
};

union ptp_rtc_preloaded4_reg_u {
	a_uint32_t val;
	struct ptp_rtc_preloaded4_reg bf;
};

/*[register] PTP_GM_CONF0_REG*/
#define PTP_GM_CONF0_REG
#define PTP_GM_CONF0_REG_ADDRESS 0x8200
#define PTP_GM_CONF0_REG_NUM     1
#define PTP_GM_CONF0_REG_INC     0x1
#define PTP_GM_CONF0_REG_TYPE    REG_TYPE_RW
#define PTP_GM_CONF0_REG_DEFAULT 0x0
	/*[field] GM_MAXFREQ_OFFSET*/
	#define PTP_GM_CONF0_REG_GM_MAXFREQ_OFFSET
	#define PTP_GM_CONF0_REG_GM_MAXFREQ_OFFSET_OFFSET  0
	#define PTP_GM_CONF0_REG_GM_MAXFREQ_OFFSET_LEN     4
	#define PTP_GM_CONF0_REG_GM_MAXFREQ_OFFSET_DEFAULT 0x0
	/*[field] GM_PLL_MODE*/
	#define PTP_GM_CONF0_REG_GM_PLL_MODE
	#define PTP_GM_CONF0_REG_GM_PLL_MODE_OFFSET  4
	#define PTP_GM_CONF0_REG_GM_PLL_MODE_LEN     1
	#define PTP_GM_CONF0_REG_GM_PLL_MODE_DEFAULT 0x0
	/*[field] GM_PPS_SYNC*/
	#define PTP_GM_CONF0_REG_GM_PPS_SYNC
	#define PTP_GM_CONF0_REG_GM_PPS_SYNC_OFFSET  5
	#define PTP_GM_CONF0_REG_GM_PPS_SYNC_LEN     1
	#define PTP_GM_CONF0_REG_GM_PPS_SYNC_DEFAULT 0x0
	/*[field] GRANDMASTER_MODE*/
	#define PTP_GM_CONF0_REG_GRANDMASTER_MODE
	#define PTP_GM_CONF0_REG_GRANDMASTER_MODE_OFFSET  6
	#define PTP_GM_CONF0_REG_GRANDMASTER_MODE_LEN     1
	#define PTP_GM_CONF0_REG_GRANDMASTER_MODE_DEFAULT 0x0

struct ptp_gm_conf0_reg {
	a_uint16_t  gm_maxfreq_offset:4;
	a_uint16_t  gm_pll_mode:1;
	a_uint16_t  gm_pps_sync:1;
	a_uint16_t  grandmaster_mode:1;
};

union ptp_gm_conf0_reg_u {
	a_uint32_t val;
	struct ptp_gm_conf0_reg bf;
};

/*[register] PTP_GM_CONF1_REG*/
#define PTP_GM_CONF1_REG
#define PTP_GM_CONF1_REG_ADDRESS 0x8201
#define PTP_GM_CONF1_REG_NUM     1
#define PTP_GM_CONF1_REG_INC     0x1
#define PTP_GM_CONF1_REG_TYPE    REG_TYPE_RW
#define PTP_GM_CONF1_REG_DEFAULT 0x0
	/*[field] GM_KI_LDN*/
	#define PTP_GM_CONF1_REG_GM_KI_LDN
	#define PTP_GM_CONF1_REG_GM_KI_LDN_OFFSET  0
	#define PTP_GM_CONF1_REG_GM_KI_LDN_LEN     6
	#define PTP_GM_CONF1_REG_GM_KI_LDN_DEFAULT 0x0
	/*[field] GM_KP_LDN*/
	#define PTP_GM_CONF1_REG_GM_KP_LDN
	#define PTP_GM_CONF1_REG_GM_KP_LDN_OFFSET  6
	#define PTP_GM_CONF1_REG_GM_KP_LDN_LEN     6
	#define PTP_GM_CONF1_REG_GM_KP_LDN_DEFAULT 0x0

struct ptp_gm_conf1_reg {
	a_uint16_t  gm_ki_ldn:6;
	a_uint16_t  gm_kp_ldn:6;
};

union ptp_gm_conf1_reg_u {
	a_uint32_t val;
	struct ptp_gm_conf1_reg bf;
};

/*[register] PTP_PPSIN_TS0_REG*/
#define PTP_PPSIN_TS0_REG
#define PTP_PPSIN_TS0_REG_ADDRESS 0x8202
#define PTP_PPSIN_TS0_REG_NUM     1
#define PTP_PPSIN_TS0_REG_INC     0x1
#define PTP_PPSIN_TS0_REG_TYPE    REG_TYPE_RW
#define PTP_PPSIN_TS0_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_PPSIN_TS0_REG_RX_TS_SEC
	#define PTP_PPSIN_TS0_REG_RX_TS_SEC_OFFSET  0
	#define PTP_PPSIN_TS0_REG_RX_TS_SEC_LEN     16
	#define PTP_PPSIN_TS0_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_ppsin_ts0_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_ppsin_ts0_reg_u {
	a_uint32_t val;
	struct ptp_ppsin_ts0_reg bf;
};

/*[register] PTP_PPSIN_TS1_REG*/
#define PTP_PPSIN_TS1_REG
#define PTP_PPSIN_TS1_REG_ADDRESS 0x8203
#define PTP_PPSIN_TS1_REG_NUM     1
#define PTP_PPSIN_TS1_REG_INC     0x1
#define PTP_PPSIN_TS1_REG_TYPE    REG_TYPE_RW
#define PTP_PPSIN_TS1_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_PPSIN_TS1_REG_RX_TS_SEC
	#define PTP_PPSIN_TS1_REG_RX_TS_SEC_OFFSET  0
	#define PTP_PPSIN_TS1_REG_RX_TS_SEC_LEN     16
	#define PTP_PPSIN_TS1_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_ppsin_ts1_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_ppsin_ts1_reg_u {
	a_uint32_t val;
	struct ptp_ppsin_ts1_reg bf;
};

/*[register] PTP_PPSIN_TS2_REG*/
#define PTP_PPSIN_TS2_REG
#define PTP_PPSIN_TS2_REG_ADDRESS 0x8204
#define PTP_PPSIN_TS2_REG_NUM     1
#define PTP_PPSIN_TS2_REG_INC     0x1
#define PTP_PPSIN_TS2_REG_TYPE    REG_TYPE_RW
#define PTP_PPSIN_TS2_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_PPSIN_TS2_REG_RX_TS_SEC
	#define PTP_PPSIN_TS2_REG_RX_TS_SEC_OFFSET  0
	#define PTP_PPSIN_TS2_REG_RX_TS_SEC_LEN     16
	#define PTP_PPSIN_TS2_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_ppsin_ts2_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_ppsin_ts2_reg_u {
	a_uint32_t val;
	struct ptp_ppsin_ts2_reg bf;
};

/*[register] PTP_PPSIN_TS3_REG*/
#define PTP_PPSIN_TS3_REG
#define PTP_PPSIN_TS3_REG_ADDRESS 0x8205
#define PTP_PPSIN_TS3_REG_NUM     1
#define PTP_PPSIN_TS3_REG_INC     0x1
#define PTP_PPSIN_TS3_REG_TYPE    REG_TYPE_RW
#define PTP_PPSIN_TS3_REG_DEFAULT 0x0
	/*[field] RX_TS_NSEC*/
	#define PTP_PPSIN_TS3_REG_RX_TS_NSEC
	#define PTP_PPSIN_TS3_REG_RX_TS_NSEC_OFFSET  0
	#define PTP_PPSIN_TS3_REG_RX_TS_NSEC_LEN     16
	#define PTP_PPSIN_TS3_REG_RX_TS_NSEC_DEFAULT 0x0

struct ptp_ppsin_ts3_reg {
	a_uint16_t  rx_ts_nsec:16;
};

union ptp_ppsin_ts3_reg_u {
	a_uint32_t val;
	struct ptp_ppsin_ts3_reg bf;
};

/*[register] PTP_PPSIN_TS4_REG*/
#define PTP_PPSIN_TS4_REG
#define PTP_PPSIN_TS4_REG_ADDRESS 0x8206
#define PTP_PPSIN_TS4_REG_NUM     1
#define PTP_PPSIN_TS4_REG_INC     0x1
#define PTP_PPSIN_TS4_REG_TYPE    REG_TYPE_RW
#define PTP_PPSIN_TS4_REG_DEFAULT 0x0
	/*[field] RX_TS_NSEC*/
	#define PTP_PPSIN_TS4_REG_RX_TS_NSEC
	#define PTP_PPSIN_TS4_REG_RX_TS_NSEC_OFFSET  0
	#define PTP_PPSIN_TS4_REG_RX_TS_NSEC_LEN     16
	#define PTP_PPSIN_TS4_REG_RX_TS_NSEC_DEFAULT 0x0

struct ptp_ppsin_ts4_reg {
	a_uint16_t  rx_ts_nsec:16;
};

union ptp_ppsin_ts4_reg_u {
	a_uint32_t val;
	struct ptp_ppsin_ts4_reg bf;
};

/*[register] PTP_HWPLL_INC0_REG*/
#define PTP_HWPLL_INC0_REG
#define PTP_HWPLL_INC0_REG_ADDRESS 0x8207
#define PTP_HWPLL_INC0_REG_NUM     1
#define PTP_HWPLL_INC0_REG_INC     0x1
#define PTP_HWPLL_INC0_REG_TYPE    REG_TYPE_RW
#define PTP_HWPLL_INC0_REG_DEFAULT 0x0
	/*[field] PTP_RTC_INC_NFS1*/
	#define PTP_HWPLL_INC0_REG_PTP_RTC_INC_NFS1
	#define PTP_HWPLL_INC0_REG_PTP_RTC_INC_NFS1_OFFSET  0
	#define PTP_HWPLL_INC0_REG_PTP_RTC_INC_NFS1_LEN     10
	#define PTP_HWPLL_INC0_REG_PTP_RTC_INC_NFS1_DEFAULT 0x0
	/*[field] PTP_RTC_INC_NIS*/
	#define PTP_HWPLL_INC0_REG_PTP_RTC_INC_NIS
	#define PTP_HWPLL_INC0_REG_PTP_RTC_INC_NIS_OFFSET  10
	#define PTP_HWPLL_INC0_REG_PTP_RTC_INC_NIS_LEN     6
	#define PTP_HWPLL_INC0_REG_PTP_RTC_INC_NIS_DEFAULT 0x0

struct ptp_hwpll_inc0_reg {
	a_uint16_t  ptp_rtc_inc_nfs1:10;
	a_uint16_t  ptp_rtc_inc_nis:6;
};

union ptp_hwpll_inc0_reg_u {
	a_uint32_t val;
	struct ptp_hwpll_inc0_reg bf;
};

/*[register] PTP_HWPLL_INC1_REG*/
#define PTP_HWPLL_INC1_REG
#define PTP_HWPLL_INC1_REG_ADDRESS 0x8208
#define PTP_HWPLL_INC1_REG_NUM     1
#define PTP_HWPLL_INC1_REG_INC     0x1
#define PTP_HWPLL_INC1_REG_TYPE    REG_TYPE_RW
#define PTP_HWPLL_INC1_REG_DEFAULT 0x0
	/*[field] PTP_RTC_INC_NFS0*/
	#define PTP_HWPLL_INC1_REG_PTP_RTC_INC_NFS0
	#define PTP_HWPLL_INC1_REG_PTP_RTC_INC_NFS0_OFFSET  0
	#define PTP_HWPLL_INC1_REG_PTP_RTC_INC_NFS0_LEN     16
	#define PTP_HWPLL_INC1_REG_PTP_RTC_INC_NFS0_DEFAULT 0x0

struct ptp_hwpll_inc1_reg {
	a_uint16_t  ptp_rtc_inc_nfs0:16;
};

union ptp_hwpll_inc1_reg_u {
	a_uint32_t val;
	struct ptp_hwpll_inc1_reg bf;
};

/*[register] PTP_PPSIN_LATENCY_REG*/
#define PTP_PPSIN_LATENCY_REG
#define PTP_PPSIN_LATENCY_REG_ADDRESS 0x8209
#define PTP_PPSIN_LATENCY_REG_NUM     1
#define PTP_PPSIN_LATENCY_REG_INC     0x1
#define PTP_PPSIN_LATENCY_REG_TYPE    REG_TYPE_RW
#define PTP_PPSIN_LATENCY_REG_DEFAULT 0x0
	/*[field] PTP_PPSIN_LATENCY_VALUE*/
	#define PTP_PPSIN_LATENCY_REG_PTP_PPSIN_LATENCY_VALUE
	#define PTP_PPSIN_LATENCY_REG_PTP_PPSIN_LATENCY_VALUE_OFFSET  0
	#define PTP_PPSIN_LATENCY_REG_PTP_PPSIN_LATENCY_VALUE_LEN     7
	#define PTP_PPSIN_LATENCY_REG_PTP_PPSIN_LATENCY_VALUE_DEFAULT 0x0
	/*[field] PTP_PPSIN_LATENCY_SIGN*/
	#define PTP_PPSIN_LATENCY_REG_PTP_PPSIN_LATENCY_SIGN
	#define PTP_PPSIN_LATENCY_REG_PTP_PPSIN_LATENCY_SIGN_OFFSET  7
	#define PTP_PPSIN_LATENCY_REG_PTP_PPSIN_LATENCY_SIGN_LEN     1
	#define PTP_PPSIN_LATENCY_REG_PTP_PPSIN_LATENCY_SIGN_DEFAULT 0x0

struct ptp_ppsin_latency_reg {
	a_uint16_t  ptp_ppsin_latency_value:7;
	a_uint16_t  ptp_ppsin_latency_sign:1;
};

union ptp_ppsin_latency_reg_u {
	a_uint32_t val;
	struct ptp_ppsin_latency_reg bf;
};

/*[register] PTP_TRIGGER0_CONFIG_REG*/
#define PTP_TRIGGER0_CONFIG_REG
#define PTP_TRIGGER0_CONFIG_REG_ADDRESS 0x8400
#define PTP_TRIGGER0_CONFIG_REG_NUM     1
#define PTP_TRIGGER0_CONFIG_REG_INC     0x1
#define PTP_TRIGGER0_CONFIG_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER0_CONFIG_REG_DEFAULT 0x0
	/*[field] STATUS*/
	#define PTP_TRIGGER0_CONFIG_REG_STATUS
	#define PTP_TRIGGER0_CONFIG_REG_STATUS_OFFSET  0
	#define PTP_TRIGGER0_CONFIG_REG_STATUS_LEN     1
	#define PTP_TRIGGER0_CONFIG_REG_STATUS_DEFAULT 0x0
	/*[field] FORCE_EN*/
	#define PTP_TRIGGER0_CONFIG_REG_FORCE_EN
	#define PTP_TRIGGER0_CONFIG_REG_FORCE_EN_OFFSET  1
	#define PTP_TRIGGER0_CONFIG_REG_FORCE_EN_LEN     1
	#define PTP_TRIGGER0_CONFIG_REG_FORCE_EN_DEFAULT 0x0
	/*[field] FORCE_VALUE*/
	#define PTP_TRIGGER0_CONFIG_REG_FORCE_VALUE
	#define PTP_TRIGGER0_CONFIG_REG_FORCE_VALUE_OFFSET  2
	#define PTP_TRIGGER0_CONFIG_REG_FORCE_VALUE_LEN     1
	#define PTP_TRIGGER0_CONFIG_REG_FORCE_VALUE_DEFAULT 0x0
	/*[field] PATTERN*/
	#define PTP_TRIGGER0_CONFIG_REG_PATTERN
	#define PTP_TRIGGER0_CONFIG_REG_PATTERN_OFFSET  3
	#define PTP_TRIGGER0_CONFIG_REG_PATTERN_LEN     3
	#define PTP_TRIGGER0_CONFIG_REG_PATTERN_DEFAULT 0x0
	/*[field] IF_LATE*/
	#define PTP_TRIGGER0_CONFIG_REG_IF_LATE
	#define PTP_TRIGGER0_CONFIG_REG_IF_LATE_OFFSET  6
	#define PTP_TRIGGER0_CONFIG_REG_IF_LATE_LEN     1
	#define PTP_TRIGGER0_CONFIG_REG_IF_LATE_DEFAULT 0x0
	/*[field] NOTIFY*/
	#define PTP_TRIGGER0_CONFIG_REG_NOTIFY
	#define PTP_TRIGGER0_CONFIG_REG_NOTIFY_OFFSET  7
	#define PTP_TRIGGER0_CONFIG_REG_NOTIFY_LEN     1
	#define PTP_TRIGGER0_CONFIG_REG_NOTIFY_DEFAULT 0x0
	/*[field] SETTING*/
	#define PTP_TRIGGER0_CONFIG_REG_SETTING
	#define PTP_TRIGGER0_CONFIG_REG_SETTING_OFFSET  8
	#define PTP_TRIGGER0_CONFIG_REG_SETTING_LEN     1
	#define PTP_TRIGGER0_CONFIG_REG_SETTING_DEFAULT 0x0

struct ptp_trigger0_config_reg {
	a_uint16_t  status:1;
	a_uint16_t  force_en:1;
	a_uint16_t  force_value:1;
	a_uint16_t  pattern:3;
	a_uint16_t  if_late:1;
	a_uint16_t  notify:1;
	a_uint16_t  setting:1;
};

union ptp_trigger0_config_reg_u {
	a_uint32_t val;
	struct ptp_trigger0_config_reg bf;
};

/*[register] PTP_TRIGGER0_STATUS_REG*/
#define PTP_TRIGGER0_STATUS_REG
#define PTP_TRIGGER0_STATUS_REG_ADDRESS 0x8401
#define PTP_TRIGGER0_STATUS_REG_NUM     1
#define PTP_TRIGGER0_STATUS_REG_INC     0x1
#define PTP_TRIGGER0_STATUS_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER0_STATUS_REG_DEFAULT 0x0
	/*[field] FINISHED*/
	#define PTP_TRIGGER0_STATUS_REG_FINISHED
	#define PTP_TRIGGER0_STATUS_REG_FINISHED_OFFSET  0
	#define PTP_TRIGGER0_STATUS_REG_FINISHED_LEN     1
	#define PTP_TRIGGER0_STATUS_REG_FINISHED_DEFAULT 0x0
	/*[field] ACTIVE*/
	#define PTP_TRIGGER0_STATUS_REG_ACTIVE
	#define PTP_TRIGGER0_STATUS_REG_ACTIVE_OFFSET  1
	#define PTP_TRIGGER0_STATUS_REG_ACTIVE_LEN     1
	#define PTP_TRIGGER0_STATUS_REG_ACTIVE_DEFAULT 0x0
	/*[field] ERROR*/
	#define PTP_TRIGGER0_STATUS_REG_ERROR
	#define PTP_TRIGGER0_STATUS_REG_ERROR_OFFSET  2
	#define PTP_TRIGGER0_STATUS_REG_ERROR_LEN     2
	#define PTP_TRIGGER0_STATUS_REG_ERROR_DEFAULT 0x0

struct ptp_trigger0_status_reg {
	a_uint16_t  finished:1;
	a_uint16_t  active:1;
	a_uint16_t  error:2;
};

union ptp_trigger0_status_reg_u {
	a_uint32_t val;
	struct ptp_trigger0_status_reg bf;
};

/*[register] PTP_TRIGGER1_CONFIG_REG*/
#define PTP_TRIGGER1_CONFIG_REG
#define PTP_TRIGGER1_CONFIG_REG_ADDRESS 0x8402
#define PTP_TRIGGER1_CONFIG_REG_NUM     1
#define PTP_TRIGGER1_CONFIG_REG_INC     0x1
#define PTP_TRIGGER1_CONFIG_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER1_CONFIG_REG_DEFAULT 0x0
	/*[field] STATUS*/
	#define PTP_TRIGGER1_CONFIG_REG_STATUS
	#define PTP_TRIGGER1_CONFIG_REG_STATUS_OFFSET  0
	#define PTP_TRIGGER1_CONFIG_REG_STATUS_LEN     1
	#define PTP_TRIGGER1_CONFIG_REG_STATUS_DEFAULT 0x0
	/*[field] FORCE_EN*/
	#define PTP_TRIGGER1_CONFIG_REG_FORCE_EN
	#define PTP_TRIGGER1_CONFIG_REG_FORCE_EN_OFFSET  1
	#define PTP_TRIGGER1_CONFIG_REG_FORCE_EN_LEN     1
	#define PTP_TRIGGER1_CONFIG_REG_FORCE_EN_DEFAULT 0x0
	/*[field] FORCE_VALUE*/
	#define PTP_TRIGGER1_CONFIG_REG_FORCE_VALUE
	#define PTP_TRIGGER1_CONFIG_REG_FORCE_VALUE_OFFSET  2
	#define PTP_TRIGGER1_CONFIG_REG_FORCE_VALUE_LEN     1
	#define PTP_TRIGGER1_CONFIG_REG_FORCE_VALUE_DEFAULT 0x0
	/*[field] PATTERN*/
	#define PTP_TRIGGER1_CONFIG_REG_PATTERN
	#define PTP_TRIGGER1_CONFIG_REG_PATTERN_OFFSET  3
	#define PTP_TRIGGER1_CONFIG_REG_PATTERN_LEN     3
	#define PTP_TRIGGER1_CONFIG_REG_PATTERN_DEFAULT 0x0
	/*[field] IF_LATE*/
	#define PTP_TRIGGER1_CONFIG_REG_IF_LATE
	#define PTP_TRIGGER1_CONFIG_REG_IF_LATE_OFFSET  6
	#define PTP_TRIGGER1_CONFIG_REG_IF_LATE_LEN     1
	#define PTP_TRIGGER1_CONFIG_REG_IF_LATE_DEFAULT 0x0
	/*[field] NOTIFY*/
	#define PTP_TRIGGER1_CONFIG_REG_NOTIFY
	#define PTP_TRIGGER1_CONFIG_REG_NOTIFY_OFFSET  7
	#define PTP_TRIGGER1_CONFIG_REG_NOTIFY_LEN     1
	#define PTP_TRIGGER1_CONFIG_REG_NOTIFY_DEFAULT 0x0
	/*[field] SETTING*/
	#define PTP_TRIGGER1_CONFIG_REG_SETTING
	#define PTP_TRIGGER1_CONFIG_REG_SETTING_OFFSET  8
	#define PTP_TRIGGER1_CONFIG_REG_SETTING_LEN     1
	#define PTP_TRIGGER1_CONFIG_REG_SETTING_DEFAULT 0x0

struct ptp_trigger1_config_reg {
	a_uint16_t  status:1;
	a_uint16_t  force_en:1;
	a_uint16_t  force_value:1;
	a_uint16_t  pattern:3;
	a_uint16_t  if_late:1;
	a_uint16_t  notify:1;
	a_uint16_t  setting:1;
};

union ptp_trigger1_config_reg_u {
	a_uint32_t val;
	struct ptp_trigger1_config_reg bf;
};

/*[register] PTP_TRIGGER1_STATUS_REG*/
#define PTP_TRIGGER1_STATUS_REG
#define PTP_TRIGGER1_STATUS_REG_ADDRESS 0x8403
#define PTP_TRIGGER1_STATUS_REG_NUM     1
#define PTP_TRIGGER1_STATUS_REG_INC     0x1
#define PTP_TRIGGER1_STATUS_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER1_STATUS_REG_DEFAULT 0x0
	/*[field] FINISHED*/
	#define PTP_TRIGGER1_STATUS_REG_FINISHED
	#define PTP_TRIGGER1_STATUS_REG_FINISHED_OFFSET  0
	#define PTP_TRIGGER1_STATUS_REG_FINISHED_LEN     1
	#define PTP_TRIGGER1_STATUS_REG_FINISHED_DEFAULT 0x0
	/*[field] ACTIVE*/
	#define PTP_TRIGGER1_STATUS_REG_ACTIVE
	#define PTP_TRIGGER1_STATUS_REG_ACTIVE_OFFSET  1
	#define PTP_TRIGGER1_STATUS_REG_ACTIVE_LEN     1
	#define PTP_TRIGGER1_STATUS_REG_ACTIVE_DEFAULT 0x0
	/*[field] ERROR*/
	#define PTP_TRIGGER1_STATUS_REG_ERROR
	#define PTP_TRIGGER1_STATUS_REG_ERROR_OFFSET  2
	#define PTP_TRIGGER1_STATUS_REG_ERROR_LEN     2
	#define PTP_TRIGGER1_STATUS_REG_ERROR_DEFAULT 0x0

struct ptp_trigger1_status_reg {
	a_uint16_t  finished:1;
	a_uint16_t  active:1;
	a_uint16_t  error:2;
};

union ptp_trigger1_status_reg_u {
	a_uint32_t val;
	struct ptp_trigger1_status_reg bf;
};

/*[register] PTP_TRIGGER0_TIMESTAMP0_REG*/
#define PTP_TRIGGER0_TIMESTAMP0_REG
#define PTP_TRIGGER0_TIMESTAMP0_REG_ADDRESS 0x8404
#define PTP_TRIGGER0_TIMESTAMP0_REG_NUM     1
#define PTP_TRIGGER0_TIMESTAMP0_REG_INC     0x1
#define PTP_TRIGGER0_TIMESTAMP0_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER0_TIMESTAMP0_REG_DEFAULT 0x0
	/*[field] TS_SEC*/
	#define PTP_TRIGGER0_TIMESTAMP0_REG_TS_SEC
	#define PTP_TRIGGER0_TIMESTAMP0_REG_TS_SEC_OFFSET  0
	#define PTP_TRIGGER0_TIMESTAMP0_REG_TS_SEC_LEN     16
	#define PTP_TRIGGER0_TIMESTAMP0_REG_TS_SEC_DEFAULT 0x0

struct ptp_trigger0_timestamp0_reg {
	a_uint16_t  ts_sec:16;
};

union ptp_trigger0_timestamp0_reg_u {
	a_uint32_t val;
	struct ptp_trigger0_timestamp0_reg bf;
};

/*[register] PTP_TRIGGER0_TIMESTAMP1_REG*/
#define PTP_TRIGGER0_TIMESTAMP1_REG
#define PTP_TRIGGER0_TIMESTAMP1_REG_ADDRESS 0x8405
#define PTP_TRIGGER0_TIMESTAMP1_REG_NUM     1
#define PTP_TRIGGER0_TIMESTAMP1_REG_INC     0x1
#define PTP_TRIGGER0_TIMESTAMP1_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER0_TIMESTAMP1_REG_DEFAULT 0x0
	/*[field] TS_SEC*/
	#define PTP_TRIGGER0_TIMESTAMP1_REG_TS_SEC
	#define PTP_TRIGGER0_TIMESTAMP1_REG_TS_SEC_OFFSET  0
	#define PTP_TRIGGER0_TIMESTAMP1_REG_TS_SEC_LEN     16
	#define PTP_TRIGGER0_TIMESTAMP1_REG_TS_SEC_DEFAULT 0x0

struct ptp_trigger0_timestamp1_reg {
	a_uint16_t  ts_sec:16;
};

union ptp_trigger0_timestamp1_reg_u {
	a_uint32_t val;
	struct ptp_trigger0_timestamp1_reg bf;
};

/*[register] PTP_TRIGGER0_TIMESTAMP2_REG*/
#define PTP_TRIGGER0_TIMESTAMP2_REG
#define PTP_TRIGGER0_TIMESTAMP2_REG_ADDRESS 0x8406
#define PTP_TRIGGER0_TIMESTAMP2_REG_NUM     1
#define PTP_TRIGGER0_TIMESTAMP2_REG_INC     0x1
#define PTP_TRIGGER0_TIMESTAMP2_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER0_TIMESTAMP2_REG_DEFAULT 0x0
	/*[field] TS_SEC*/
	#define PTP_TRIGGER0_TIMESTAMP2_REG_TS_SEC
	#define PTP_TRIGGER0_TIMESTAMP2_REG_TS_SEC_OFFSET  0
	#define PTP_TRIGGER0_TIMESTAMP2_REG_TS_SEC_LEN     16
	#define PTP_TRIGGER0_TIMESTAMP2_REG_TS_SEC_DEFAULT 0x0

struct ptp_trigger0_timestamp2_reg {
	a_uint16_t  ts_sec:16;
};

union ptp_trigger0_timestamp2_reg_u {
	a_uint32_t val;
	struct ptp_trigger0_timestamp2_reg bf;
};

/*[register] PTP_TRIGGER0_TIMESTAMP3_REG*/
#define PTP_TRIGGER0_TIMESTAMP3_REG
#define PTP_TRIGGER0_TIMESTAMP3_REG_ADDRESS 0x8407
#define PTP_TRIGGER0_TIMESTAMP3_REG_NUM     1
#define PTP_TRIGGER0_TIMESTAMP3_REG_INC     0x1
#define PTP_TRIGGER0_TIMESTAMP3_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER0_TIMESTAMP3_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_TRIGGER0_TIMESTAMP3_REG_TS_NSEC
	#define PTP_TRIGGER0_TIMESTAMP3_REG_TS_NSEC_OFFSET  0
	#define PTP_TRIGGER0_TIMESTAMP3_REG_TS_NSEC_LEN     16
	#define PTP_TRIGGER0_TIMESTAMP3_REG_TS_NSEC_DEFAULT 0x0

struct ptp_trigger0_timestamp3_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_trigger0_timestamp3_reg_u {
	a_uint32_t val;
	struct ptp_trigger0_timestamp3_reg bf;
};

/*[register] PTP_TRIGGER0_TIMESTAMP4_REG*/
#define PTP_TRIGGER0_TIMESTAMP4_REG
#define PTP_TRIGGER0_TIMESTAMP4_REG_ADDRESS 0x8408
#define PTP_TRIGGER0_TIMESTAMP4_REG_NUM     1
#define PTP_TRIGGER0_TIMESTAMP4_REG_INC     0x1
#define PTP_TRIGGER0_TIMESTAMP4_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER0_TIMESTAMP4_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_TRIGGER0_TIMESTAMP4_REG_TS_NSEC
	#define PTP_TRIGGER0_TIMESTAMP4_REG_TS_NSEC_OFFSET  0
	#define PTP_TRIGGER0_TIMESTAMP4_REG_TS_NSEC_LEN     16
	#define PTP_TRIGGER0_TIMESTAMP4_REG_TS_NSEC_DEFAULT 0x0

struct ptp_trigger0_timestamp4_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_trigger0_timestamp4_reg_u {
	a_uint32_t val;
	struct ptp_trigger0_timestamp4_reg bf;
};

/*[register] PTP_TRIGGER1_TIMESTAMP0_REG*/
#define PTP_TRIGGER1_TIMESTAMP0_REG
#define PTP_TRIGGER1_TIMESTAMP0_REG_ADDRESS 0x8409
#define PTP_TRIGGER1_TIMESTAMP0_REG_NUM     1
#define PTP_TRIGGER1_TIMESTAMP0_REG_INC     0x1
#define PTP_TRIGGER1_TIMESTAMP0_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER1_TIMESTAMP0_REG_DEFAULT 0x0
	/*[field] TS_SEC*/
	#define PTP_TRIGGER1_TIMESTAMP0_REG_TS_SEC
	#define PTP_TRIGGER1_TIMESTAMP0_REG_TS_SEC_OFFSET  0
	#define PTP_TRIGGER1_TIMESTAMP0_REG_TS_SEC_LEN     16
	#define PTP_TRIGGER1_TIMESTAMP0_REG_TS_SEC_DEFAULT 0x0

struct ptp_trigger1_timestamp0_reg {
	a_uint16_t  ts_sec:16;
};

union ptp_trigger1_timestamp0_reg_u {
	a_uint32_t val;
	struct ptp_trigger1_timestamp0_reg bf;
};

/*[register] PTP_TRIGGER1_TIMESTAMP1_REG*/
#define PTP_TRIGGER1_TIMESTAMP1_REG
#define PTP_TRIGGER1_TIMESTAMP1_REG_ADDRESS 0x840a
#define PTP_TRIGGER1_TIMESTAMP1_REG_NUM     1
#define PTP_TRIGGER1_TIMESTAMP1_REG_INC     0x1
#define PTP_TRIGGER1_TIMESTAMP1_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER1_TIMESTAMP1_REG_DEFAULT 0x0
	/*[field] TS_SEC*/
	#define PTP_TRIGGER1_TIMESTAMP1_REG_TS_SEC
	#define PTP_TRIGGER1_TIMESTAMP1_REG_TS_SEC_OFFSET  0
	#define PTP_TRIGGER1_TIMESTAMP1_REG_TS_SEC_LEN     16
	#define PTP_TRIGGER1_TIMESTAMP1_REG_TS_SEC_DEFAULT 0x0

struct ptp_trigger1_timestamp1_reg {
	a_uint16_t  ts_sec:16;
};

union ptp_trigger1_timestamp1_reg_u {
	a_uint32_t val;
	struct ptp_trigger1_timestamp1_reg bf;
};

/*[register] PTP_TRIGGER1_TIMESTAMP2_REG*/
#define PTP_TRIGGER1_TIMESTAMP2_REG
#define PTP_TRIGGER1_TIMESTAMP2_REG_ADDRESS 0x840b
#define PTP_TRIGGER1_TIMESTAMP2_REG_NUM     1
#define PTP_TRIGGER1_TIMESTAMP2_REG_INC     0x1
#define PTP_TRIGGER1_TIMESTAMP2_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER1_TIMESTAMP2_REG_DEFAULT 0x0
	/*[field] TS_SEC*/
	#define PTP_TRIGGER1_TIMESTAMP2_REG_TS_SEC
	#define PTP_TRIGGER1_TIMESTAMP2_REG_TS_SEC_OFFSET  0
	#define PTP_TRIGGER1_TIMESTAMP2_REG_TS_SEC_LEN     16
	#define PTP_TRIGGER1_TIMESTAMP2_REG_TS_SEC_DEFAULT 0x0

struct ptp_trigger1_timestamp2_reg {
	a_uint16_t  ts_sec:16;
};

union ptp_trigger1_timestamp2_reg_u {
	a_uint32_t val;
	struct ptp_trigger1_timestamp2_reg bf;
};

/*[register] PTP_TRIGGER1_TIMESTAMP3_REG*/
#define PTP_TRIGGER1_TIMESTAMP3_REG
#define PTP_TRIGGER1_TIMESTAMP3_REG_ADDRESS 0x840c
#define PTP_TRIGGER1_TIMESTAMP3_REG_NUM     1
#define PTP_TRIGGER1_TIMESTAMP3_REG_INC     0x1
#define PTP_TRIGGER1_TIMESTAMP3_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER1_TIMESTAMP3_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_TRIGGER1_TIMESTAMP3_REG_TS_NSEC
	#define PTP_TRIGGER1_TIMESTAMP3_REG_TS_NSEC_OFFSET  0
	#define PTP_TRIGGER1_TIMESTAMP3_REG_TS_NSEC_LEN     16
	#define PTP_TRIGGER1_TIMESTAMP3_REG_TS_NSEC_DEFAULT 0x0

struct ptp_trigger1_timestamp3_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_trigger1_timestamp3_reg_u {
	a_uint32_t val;
	struct ptp_trigger1_timestamp3_reg bf;
};

/*[register] PTP_TRIGGER1_TIMESTAMP4_REG*/
#define PTP_TRIGGER1_TIMESTAMP4_REG
#define PTP_TRIGGER1_TIMESTAMP4_REG_ADDRESS 0x840d
#define PTP_TRIGGER1_TIMESTAMP4_REG_NUM     1
#define PTP_TRIGGER1_TIMESTAMP4_REG_INC     0x1
#define PTP_TRIGGER1_TIMESTAMP4_REG_TYPE    REG_TYPE_RW
#define PTP_TRIGGER1_TIMESTAMP4_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_TRIGGER1_TIMESTAMP4_REG_TS_NSEC
	#define PTP_TRIGGER1_TIMESTAMP4_REG_TS_NSEC_OFFSET  0
	#define PTP_TRIGGER1_TIMESTAMP4_REG_TS_NSEC_LEN     16
	#define PTP_TRIGGER1_TIMESTAMP4_REG_TS_NSEC_DEFAULT 0x0

struct ptp_trigger1_timestamp4_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_trigger1_timestamp4_reg_u {
	a_uint32_t val;
	struct ptp_trigger1_timestamp4_reg bf;
};

/*[register] PTP_EVENT0_CONFIG_REG*/
#define PTP_EVENT0_CONFIG_REG
#define PTP_EVENT0_CONFIG_REG_ADDRESS 0x840e
#define PTP_EVENT0_CONFIG_REG_NUM     1
#define PTP_EVENT0_CONFIG_REG_INC     0x1
#define PTP_EVENT0_CONFIG_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT0_CONFIG_REG_DEFAULT 0x0
	/*[field] CLEAR_STAT*/
	#define PTP_EVENT0_CONFIG_REG_CLEAR_STAT
	#define PTP_EVENT0_CONFIG_REG_CLEAR_STAT_OFFSET  0
	#define PTP_EVENT0_CONFIG_REG_CLEAR_STAT_LEN     1
	#define PTP_EVENT0_CONFIG_REG_CLEAR_STAT_DEFAULT 0x0
	/*[field] NOTIFY*/
	#define PTP_EVENT0_CONFIG_REG_NOTIFY
	#define PTP_EVENT0_CONFIG_REG_NOTIFY_OFFSET  1
	#define PTP_EVENT0_CONFIG_REG_NOTIFY_LEN     1
	#define PTP_EVENT0_CONFIG_REG_NOTIFY_DEFAULT 0x0
	/*[field] SINGLE_CAP*/
	#define PTP_EVENT0_CONFIG_REG_SINGLE_CAP
	#define PTP_EVENT0_CONFIG_REG_SINGLE_CAP_OFFSET  2
	#define PTP_EVENT0_CONFIG_REG_SINGLE_CAP_LEN     1
	#define PTP_EVENT0_CONFIG_REG_SINGLE_CAP_DEFAULT 0x0
	/*[field] FALL_EN*/
	#define PTP_EVENT0_CONFIG_REG_FALL_EN
	#define PTP_EVENT0_CONFIG_REG_FALL_EN_OFFSET  3
	#define PTP_EVENT0_CONFIG_REG_FALL_EN_LEN     1
	#define PTP_EVENT0_CONFIG_REG_FALL_EN_DEFAULT 0x0
	/*[field] RISE_EN*/
	#define PTP_EVENT0_CONFIG_REG_RISE_EN
	#define PTP_EVENT0_CONFIG_REG_RISE_EN_OFFSET  4
	#define PTP_EVENT0_CONFIG_REG_RISE_EN_LEN     1
	#define PTP_EVENT0_CONFIG_REG_RISE_EN_DEFAULT 0x0

struct ptp_event0_config_reg {
	a_uint16_t  clear_stat:1;
	a_uint16_t  notify:1;
	a_uint16_t  single_cap:1;
	a_uint16_t  fall_en:1;
	a_uint16_t  rise_en:1;
};

union ptp_event0_config_reg_u {
	a_uint32_t val;
	struct ptp_event0_config_reg bf;
};

/*[register] PTP_EVENT0_STATUS_REG*/
#define PTP_EVENT0_STATUS_REG
#define PTP_EVENT0_STATUS_REG_ADDRESS 0x840f
#define PTP_EVENT0_STATUS_REG_NUM     1
#define PTP_EVENT0_STATUS_REG_INC     0x1
#define PTP_EVENT0_STATUS_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT0_STATUS_REG_DEFAULT 0x0
	/*[field] DETECTED*/
	#define PTP_EVENT0_STATUS_REG_DETECTED
	#define PTP_EVENT0_STATUS_REG_DETECTED_OFFSET  0
	#define PTP_EVENT0_STATUS_REG_DETECTED_LEN     1
	#define PTP_EVENT0_STATUS_REG_DETECTED_DEFAULT 0x0
	/*[field] DIR_DETECTED*/
	#define PTP_EVENT0_STATUS_REG_DIR_DETECTED
	#define PTP_EVENT0_STATUS_REG_DIR_DETECTED_OFFSET  1
	#define PTP_EVENT0_STATUS_REG_DIR_DETECTED_LEN     1
	#define PTP_EVENT0_STATUS_REG_DIR_DETECTED_DEFAULT 0x0
	/*[field] MUL_EVENT*/
	#define PTP_EVENT0_STATUS_REG_MUL_EVENT
	#define PTP_EVENT0_STATUS_REG_MUL_EVENT_OFFSET  2
	#define PTP_EVENT0_STATUS_REG_MUL_EVENT_LEN     1
	#define PTP_EVENT0_STATUS_REG_MUL_EVENT_DEFAULT 0x0
	/*[field] MISSED_COUNT*/
	#define PTP_EVENT0_STATUS_REG_MISSED_COUNT
	#define PTP_EVENT0_STATUS_REG_MISSED_COUNT_OFFSET  3
	#define PTP_EVENT0_STATUS_REG_MISSED_COUNT_LEN     4
	#define PTP_EVENT0_STATUS_REG_MISSED_COUNT_DEFAULT 0x0

struct ptp_event0_status_reg {
	a_uint16_t  detected:1;
	a_uint16_t  dir_detected:1;
	a_uint16_t  mul_event:1;
	a_uint16_t  missed_count:4;
};

union ptp_event0_status_reg_u {
	a_uint32_t val;
	struct ptp_event0_status_reg bf;
};

/*[register] PTP_EVENT1_CONFIG_REG*/
#define PTP_EVENT1_CONFIG_REG
#define PTP_EVENT1_CONFIG_REG_ADDRESS 0x8410
#define PTP_EVENT1_CONFIG_REG_NUM     1
#define PTP_EVENT1_CONFIG_REG_INC     0x1
#define PTP_EVENT1_CONFIG_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT1_CONFIG_REG_DEFAULT 0x0
	/*[field] CLEAR_STAT*/
	#define PTP_EVENT1_CONFIG_REG_CLEAR_STAT
	#define PTP_EVENT1_CONFIG_REG_CLEAR_STAT_OFFSET  0
	#define PTP_EVENT1_CONFIG_REG_CLEAR_STAT_LEN     1
	#define PTP_EVENT1_CONFIG_REG_CLEAR_STAT_DEFAULT 0x0
	/*[field] NOTIFY*/
	#define PTP_EVENT1_CONFIG_REG_NOTIFY
	#define PTP_EVENT1_CONFIG_REG_NOTIFY_OFFSET  1
	#define PTP_EVENT1_CONFIG_REG_NOTIFY_LEN     1
	#define PTP_EVENT1_CONFIG_REG_NOTIFY_DEFAULT 0x0
	/*[field] SINGLE_CAP*/
	#define PTP_EVENT1_CONFIG_REG_SINGLE_CAP
	#define PTP_EVENT1_CONFIG_REG_SINGLE_CAP_OFFSET  2
	#define PTP_EVENT1_CONFIG_REG_SINGLE_CAP_LEN     1
	#define PTP_EVENT1_CONFIG_REG_SINGLE_CAP_DEFAULT 0x0
	/*[field] FALL_EN*/
	#define PTP_EVENT1_CONFIG_REG_FALL_EN
	#define PTP_EVENT1_CONFIG_REG_FALL_EN_OFFSET  3
	#define PTP_EVENT1_CONFIG_REG_FALL_EN_LEN     1
	#define PTP_EVENT1_CONFIG_REG_FALL_EN_DEFAULT 0x0
	/*[field] RISE_EN*/
	#define PTP_EVENT1_CONFIG_REG_RISE_EN
	#define PTP_EVENT1_CONFIG_REG_RISE_EN_OFFSET  4
	#define PTP_EVENT1_CONFIG_REG_RISE_EN_LEN     1
	#define PTP_EVENT1_CONFIG_REG_RISE_EN_DEFAULT 0x0

struct ptp_event1_config_reg {
	a_uint16_t  clear_stat:1;
	a_uint16_t  notify:1;
	a_uint16_t  single_cap:1;
	a_uint16_t  fall_en:1;
	a_uint16_t  rise_en:1;
};

union ptp_event1_config_reg_u {
	a_uint32_t val;
	struct ptp_event1_config_reg bf;
};

/*[register] PTP_EVENT1_STATUS_REG*/
#define PTP_EVENT1_STATUS_REG
#define PTP_EVENT1_STATUS_REG_ADDRESS 0x8411
#define PTP_EVENT1_STATUS_REG_NUM     1
#define PTP_EVENT1_STATUS_REG_INC     0x1
#define PTP_EVENT1_STATUS_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT1_STATUS_REG_DEFAULT 0x0
	/*[field] DETECTED*/
	#define PTP_EVENT1_STATUS_REG_DETECTED
	#define PTP_EVENT1_STATUS_REG_DETECTED_OFFSET  0
	#define PTP_EVENT1_STATUS_REG_DETECTED_LEN     1
	#define PTP_EVENT1_STATUS_REG_DETECTED_DEFAULT 0x0
	/*[field] DIR_DETECTED*/
	#define PTP_EVENT1_STATUS_REG_DIR_DETECTED
	#define PTP_EVENT1_STATUS_REG_DIR_DETECTED_OFFSET  1
	#define PTP_EVENT1_STATUS_REG_DIR_DETECTED_LEN     1
	#define PTP_EVENT1_STATUS_REG_DIR_DETECTED_DEFAULT 0x0
	/*[field] MUL_EVENT*/
	#define PTP_EVENT1_STATUS_REG_MUL_EVENT
	#define PTP_EVENT1_STATUS_REG_MUL_EVENT_OFFSET  2
	#define PTP_EVENT1_STATUS_REG_MUL_EVENT_LEN     1
	#define PTP_EVENT1_STATUS_REG_MUL_EVENT_DEFAULT 0x0
	/*[field] MISSED_COUNT*/
	#define PTP_EVENT1_STATUS_REG_MISSED_COUNT
	#define PTP_EVENT1_STATUS_REG_MISSED_COUNT_OFFSET  3
	#define PTP_EVENT1_STATUS_REG_MISSED_COUNT_LEN     4
	#define PTP_EVENT1_STATUS_REG_MISSED_COUNT_DEFAULT 0x0

struct ptp_event1_status_reg {
	a_uint16_t  detected:1;
	a_uint16_t  dir_detected:1;
	a_uint16_t  mul_event:1;
	a_uint16_t  missed_count:4;
};

union ptp_event1_status_reg_u {
	a_uint32_t val;
	struct ptp_event1_status_reg bf;
};

/*[register] PTP_EVENT0_TIMESTAMP0_REG*/
#define PTP_EVENT0_TIMESTAMP0_REG
#define PTP_EVENT0_TIMESTAMP0_REG_ADDRESS 0x8412
#define PTP_EVENT0_TIMESTAMP0_REG_NUM     1
#define PTP_EVENT0_TIMESTAMP0_REG_INC     0x1
#define PTP_EVENT0_TIMESTAMP0_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT0_TIMESTAMP0_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_EVENT0_TIMESTAMP0_REG_TS_NSEC
	#define PTP_EVENT0_TIMESTAMP0_REG_TS_NSEC_OFFSET  0
	#define PTP_EVENT0_TIMESTAMP0_REG_TS_NSEC_LEN     16
	#define PTP_EVENT0_TIMESTAMP0_REG_TS_NSEC_DEFAULT 0x0

struct ptp_event0_timestamp0_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_event0_timestamp0_reg_u {
	a_uint32_t val;
	struct ptp_event0_timestamp0_reg bf;
};

/*[register] PTP_EVENT0_TIMESTAMP1_REG*/
#define PTP_EVENT0_TIMESTAMP1_REG
#define PTP_EVENT0_TIMESTAMP1_REG_ADDRESS 0x8413
#define PTP_EVENT0_TIMESTAMP1_REG_NUM     1
#define PTP_EVENT0_TIMESTAMP1_REG_INC     0x1
#define PTP_EVENT0_TIMESTAMP1_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT0_TIMESTAMP1_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_EVENT0_TIMESTAMP1_REG_TS_NSEC
	#define PTP_EVENT0_TIMESTAMP1_REG_TS_NSEC_OFFSET  0
	#define PTP_EVENT0_TIMESTAMP1_REG_TS_NSEC_LEN     16
	#define PTP_EVENT0_TIMESTAMP1_REG_TS_NSEC_DEFAULT 0x0

struct ptp_event0_timestamp1_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_event0_timestamp1_reg_u {
	a_uint32_t val;
	struct ptp_event0_timestamp1_reg bf;
};

/*[register] PTP_EVENT0_TIMESTAMP2_REG*/
#define PTP_EVENT0_TIMESTAMP2_REG
#define PTP_EVENT0_TIMESTAMP2_REG_ADDRESS 0x8414
#define PTP_EVENT0_TIMESTAMP2_REG_NUM     1
#define PTP_EVENT0_TIMESTAMP2_REG_INC     0x1
#define PTP_EVENT0_TIMESTAMP2_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT0_TIMESTAMP2_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_EVENT0_TIMESTAMP2_REG_TS_NSEC
	#define PTP_EVENT0_TIMESTAMP2_REG_TS_NSEC_OFFSET  0
	#define PTP_EVENT0_TIMESTAMP2_REG_TS_NSEC_LEN     16
	#define PTP_EVENT0_TIMESTAMP2_REG_TS_NSEC_DEFAULT 0x0

struct ptp_event0_timestamp2_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_event0_timestamp2_reg_u {
	a_uint32_t val;
	struct ptp_event0_timestamp2_reg bf;
};

/*[register] PTP_EVENT0_TIMESTAMP3_REG*/
#define PTP_EVENT0_TIMESTAMP3_REG
#define PTP_EVENT0_TIMESTAMP3_REG_ADDRESS 0x8415
#define PTP_EVENT0_TIMESTAMP3_REG_NUM     1
#define PTP_EVENT0_TIMESTAMP3_REG_INC     0x1
#define PTP_EVENT0_TIMESTAMP3_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT0_TIMESTAMP3_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_EVENT0_TIMESTAMP3_REG_TS_NSEC
	#define PTP_EVENT0_TIMESTAMP3_REG_TS_NSEC_OFFSET  0
	#define PTP_EVENT0_TIMESTAMP3_REG_TS_NSEC_LEN     16
	#define PTP_EVENT0_TIMESTAMP3_REG_TS_NSEC_DEFAULT 0x0

struct ptp_event0_timestamp3_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_event0_timestamp3_reg_u {
	a_uint32_t val;
	struct ptp_event0_timestamp3_reg bf;
};

/*[register] PTP_EVENT0_TIMESTAMP4_REG*/
#define PTP_EVENT0_TIMESTAMP4_REG
#define PTP_EVENT0_TIMESTAMP4_REG_ADDRESS 0x8416
#define PTP_EVENT0_TIMESTAMP4_REG_NUM     1
#define PTP_EVENT0_TIMESTAMP4_REG_INC     0x1
#define PTP_EVENT0_TIMESTAMP4_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT0_TIMESTAMP4_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_EVENT0_TIMESTAMP4_REG_TS_NSEC
	#define PTP_EVENT0_TIMESTAMP4_REG_TS_NSEC_OFFSET  0
	#define PTP_EVENT0_TIMESTAMP4_REG_TS_NSEC_LEN     16
	#define PTP_EVENT0_TIMESTAMP4_REG_TS_NSEC_DEFAULT 0x0

struct ptp_event0_timestamp4_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_event0_timestamp4_reg_u {
	a_uint32_t val;
	struct ptp_event0_timestamp4_reg bf;
};

/*[register] PTP_EVENT1_TIMESTAMP0_REG*/
#define PTP_EVENT1_TIMESTAMP0_REG
#define PTP_EVENT1_TIMESTAMP0_REG_ADDRESS 0x8417
#define PTP_EVENT1_TIMESTAMP0_REG_NUM     1
#define PTP_EVENT1_TIMESTAMP0_REG_INC     0x1
#define PTP_EVENT1_TIMESTAMP0_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT1_TIMESTAMP0_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_EVENT1_TIMESTAMP0_REG_TS_NSEC
	#define PTP_EVENT1_TIMESTAMP0_REG_TS_NSEC_OFFSET  0
	#define PTP_EVENT1_TIMESTAMP0_REG_TS_NSEC_LEN     16
	#define PTP_EVENT1_TIMESTAMP0_REG_TS_NSEC_DEFAULT 0x0

struct ptp_event1_timestamp0_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_event1_timestamp0_reg_u {
	a_uint32_t val;
	struct ptp_event1_timestamp0_reg bf;
};

/*[register] PTP_EVENT1_TIMESTAMP1_REG*/
#define PTP_EVENT1_TIMESTAMP1_REG
#define PTP_EVENT1_TIMESTAMP1_REG_ADDRESS 0x8418
#define PTP_EVENT1_TIMESTAMP1_REG_NUM     1
#define PTP_EVENT1_TIMESTAMP1_REG_INC     0x1
#define PTP_EVENT1_TIMESTAMP1_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT1_TIMESTAMP1_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_EVENT1_TIMESTAMP1_REG_TS_NSEC
	#define PTP_EVENT1_TIMESTAMP1_REG_TS_NSEC_OFFSET  0
	#define PTP_EVENT1_TIMESTAMP1_REG_TS_NSEC_LEN     16
	#define PTP_EVENT1_TIMESTAMP1_REG_TS_NSEC_DEFAULT 0x0

struct ptp_event1_timestamp1_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_event1_timestamp1_reg_u {
	a_uint32_t val;
	struct ptp_event1_timestamp1_reg bf;
};

/*[register] PTP_EVENT1_TIMESTAMP2_REG*/
#define PTP_EVENT1_TIMESTAMP2_REG
#define PTP_EVENT1_TIMESTAMP2_REG_ADDRESS 0x8419
#define PTP_EVENT1_TIMESTAMP2_REG_NUM     1
#define PTP_EVENT1_TIMESTAMP2_REG_INC     0x1
#define PTP_EVENT1_TIMESTAMP2_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT1_TIMESTAMP2_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_EVENT1_TIMESTAMP2_REG_TS_NSEC
	#define PTP_EVENT1_TIMESTAMP2_REG_TS_NSEC_OFFSET  0
	#define PTP_EVENT1_TIMESTAMP2_REG_TS_NSEC_LEN     16
	#define PTP_EVENT1_TIMESTAMP2_REG_TS_NSEC_DEFAULT 0x0

struct ptp_event1_timestamp2_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_event1_timestamp2_reg_u {
	a_uint32_t val;
	struct ptp_event1_timestamp2_reg bf;
};

/*[register] PTP_EVENT1_TIMESTAMP3_REG*/
#define PTP_EVENT1_TIMESTAMP3_REG
#define PTP_EVENT1_TIMESTAMP3_REG_ADDRESS 0x841a
#define PTP_EVENT1_TIMESTAMP3_REG_NUM     1
#define PTP_EVENT1_TIMESTAMP3_REG_INC     0x4
#define PTP_EVENT1_TIMESTAMP3_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT1_TIMESTAMP3_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_EVENT1_TIMESTAMP3_REG_TS_NSEC
	#define PTP_EVENT1_TIMESTAMP3_REG_TS_NSEC_OFFSET  0
	#define PTP_EVENT1_TIMESTAMP3_REG_TS_NSEC_LEN     16
	#define PTP_EVENT1_TIMESTAMP3_REG_TS_NSEC_DEFAULT 0x0

struct ptp_event1_timestamp3_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_event1_timestamp3_reg_u {
	a_uint32_t val;
	struct ptp_event1_timestamp3_reg bf;
};

/*[register] PTP_EVENT1_TIMESTAMP4_REG*/
#define PTP_EVENT1_TIMESTAMP4_REG
#define PTP_EVENT1_TIMESTAMP4_REG_ADDRESS 0x841b
#define PTP_EVENT1_TIMESTAMP4_REG_NUM     1
#define PTP_EVENT1_TIMESTAMP4_REG_INC     0x1
#define PTP_EVENT1_TIMESTAMP4_REG_TYPE    REG_TYPE_RW
#define PTP_EVENT1_TIMESTAMP4_REG_DEFAULT 0x0
	/*[field] TS_NSEC*/
	#define PTP_EVENT1_TIMESTAMP4_REG_TS_NSEC
	#define PTP_EVENT1_TIMESTAMP4_REG_TS_NSEC_OFFSET  0
	#define PTP_EVENT1_TIMESTAMP4_REG_TS_NSEC_LEN     16
	#define PTP_EVENT1_TIMESTAMP4_REG_TS_NSEC_DEFAULT 0x0

struct ptp_event1_timestamp4_reg {
	a_uint16_t  ts_nsec:16;
};

union ptp_event1_timestamp4_reg_u {
	a_uint32_t val;
	struct ptp_event1_timestamp4_reg bf;
};

/*[register] PTP_RX_SEQID1_REG*/
#define PTP_RX_SEQID1_REG
#define PTP_RX_SEQID1_REG_ADDRESS 0x8500
#define PTP_RX_SEQID1_REG_NUM     1
#define PTP_RX_SEQID1_REG_INC     0x1
#define PTP_RX_SEQID1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_SEQID1_REG_DEFAULT 0x0
	/*[field] RX_SEQID*/
	#define PTP_RX_SEQID1_REG_RX_SEQID
	#define PTP_RX_SEQID1_REG_RX_SEQID_OFFSET  0
	#define PTP_RX_SEQID1_REG_RX_SEQID_LEN     16
	#define PTP_RX_SEQID1_REG_RX_SEQID_DEFAULT 0x0

struct ptp_rx_seqid1_reg {
	a_uint16_t  rx_seqid:16;
};

union ptp_rx_seqid1_reg_u {
	a_uint32_t val;
	struct ptp_rx_seqid1_reg bf;
};

/*[register] PTP_RX_PORTID1_0_REG*/
#define PTP_RX_PORTID1_0_REG
#define PTP_RX_PORTID1_0_REG_ADDRESS 0x8501
#define PTP_RX_PORTID1_0_REG_NUM     1
#define PTP_RX_PORTID1_0_REG_INC     0x1
#define PTP_RX_PORTID1_0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID1_0_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID1_0_REG_RX_PORTID
	#define PTP_RX_PORTID1_0_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID1_0_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID1_0_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid1_0_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid1_0_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid1_0_reg bf;
};

/*[register] PTP_RX_PORTID1_1_REG*/
#define PTP_RX_PORTID1_1_REG
#define PTP_RX_PORTID1_1_REG_ADDRESS 0x8502
#define PTP_RX_PORTID1_1_REG_NUM     1
#define PTP_RX_PORTID1_1_REG_INC     0x1
#define PTP_RX_PORTID1_1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID1_1_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID1_1_REG_RX_PORTID
	#define PTP_RX_PORTID1_1_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID1_1_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID1_1_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid1_1_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid1_1_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid1_1_reg bf;
};

/*[register] PTP_RX_PORTID1_2_REG*/
#define PTP_RX_PORTID1_2_REG
#define PTP_RX_PORTID1_2_REG_ADDRESS 0x8503
#define PTP_RX_PORTID1_2_REG_NUM     1
#define PTP_RX_PORTID1_2_REG_INC     0x1
#define PTP_RX_PORTID1_2_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID1_2_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID1_2_REG_RX_PORTID
	#define PTP_RX_PORTID1_2_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID1_2_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID1_2_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid1_2_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid1_2_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid1_2_reg bf;
};

/*[register] PTP_RX_PORTID1_3_REG*/
#define PTP_RX_PORTID1_3_REG
#define PTP_RX_PORTID1_3_REG_ADDRESS 0x8504
#define PTP_RX_PORTID1_3_REG_NUM     1
#define PTP_RX_PORTID1_3_REG_INC     0x1
#define PTP_RX_PORTID1_3_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID1_3_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID1_3_REG_RX_PORTID
	#define PTP_RX_PORTID1_3_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID1_3_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID1_3_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid1_3_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid1_3_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid1_3_reg bf;
};

/*[register] PTP_RX_PORTID1_4_REG*/
#define PTP_RX_PORTID1_4_REG
#define PTP_RX_PORTID1_4_REG_ADDRESS 0x8505
#define PTP_RX_PORTID1_4_REG_NUM     1
#define PTP_RX_PORTID1_4_REG_INC     0x1
#define PTP_RX_PORTID1_4_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID1_4_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID1_4_REG_RX_PORTID
	#define PTP_RX_PORTID1_4_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID1_4_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID1_4_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid1_4_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid1_4_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid1_4_reg bf;
};

/*[register] PTP_RX_TS1_0_REG*/
#define PTP_RX_TS1_0_REG
#define PTP_RX_TS1_0_REG_ADDRESS 0x8506
#define PTP_RX_TS1_0_REG_NUM     1
#define PTP_RX_TS1_0_REG_INC     0x1
#define PTP_RX_TS1_0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS1_0_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_RX_TS1_0_REG_RX_TS_SEC
	#define PTP_RX_TS1_0_REG_RX_TS_SEC_OFFSET  0
	#define PTP_RX_TS1_0_REG_RX_TS_SEC_LEN     16
	#define PTP_RX_TS1_0_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_rx_ts1_0_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_rx_ts1_0_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts1_0_reg bf;
};

/*[register] PTP_RX_TS1_1_REG*/
#define PTP_RX_TS1_1_REG
#define PTP_RX_TS1_1_REG_ADDRESS 0x8507
#define PTP_RX_TS1_1_REG_NUM     1
#define PTP_RX_TS1_1_REG_INC     0x1
#define PTP_RX_TS1_1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS1_1_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_RX_TS1_1_REG_RX_TS_SEC
	#define PTP_RX_TS1_1_REG_RX_TS_SEC_OFFSET  0
	#define PTP_RX_TS1_1_REG_RX_TS_SEC_LEN     16
	#define PTP_RX_TS1_1_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_rx_ts1_1_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_rx_ts1_1_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts1_1_reg bf;
};

/*[register] PTP_RX_TS1_2_REG*/
#define PTP_RX_TS1_2_REG
#define PTP_RX_TS1_2_REG_ADDRESS 0x8508
#define PTP_RX_TS1_2_REG_NUM     1
#define PTP_RX_TS1_2_REG_INC     0x1
#define PTP_RX_TS1_2_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS1_2_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_RX_TS1_2_REG_RX_TS_SEC
	#define PTP_RX_TS1_2_REG_RX_TS_SEC_OFFSET  0
	#define PTP_RX_TS1_2_REG_RX_TS_SEC_LEN     16
	#define PTP_RX_TS1_2_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_rx_ts1_2_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_rx_ts1_2_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts1_2_reg bf;
};

/*[register] PTP_RX_TS1_3_REG*/
#define PTP_RX_TS1_3_REG
#define PTP_RX_TS1_3_REG_ADDRESS 0x8509
#define PTP_RX_TS1_3_REG_NUM     1
#define PTP_RX_TS1_3_REG_INC     0x1
#define PTP_RX_TS1_3_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS1_3_REG_DEFAULT 0x0
	/*[field] RX_TS_NSEC*/
	#define PTP_RX_TS1_3_REG_RX_TS_NSEC
	#define PTP_RX_TS1_3_REG_RX_TS_NSEC_OFFSET  0
	#define PTP_RX_TS1_3_REG_RX_TS_NSEC_LEN     16
	#define PTP_RX_TS1_3_REG_RX_TS_NSEC_DEFAULT 0x0

struct ptp_rx_ts1_3_reg {
	a_uint16_t  rx_ts_nsec:16;
};

union ptp_rx_ts1_3_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts1_3_reg bf;
};

/*[register] PTP_RX_TS1_4_REG*/
#define PTP_RX_TS1_4_REG
#define PTP_RX_TS1_4_REG_ADDRESS 0x850a
#define PTP_RX_TS1_4_REG_NUM     1
#define PTP_RX_TS1_4_REG_INC     0x1
#define PTP_RX_TS1_4_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS1_4_REG_DEFAULT 0x0
	/*[field] RX_TS_NSEC*/
	#define PTP_RX_TS1_4_REG_RX_TS_NSEC
	#define PTP_RX_TS1_4_REG_RX_TS_NSEC_OFFSET  0
	#define PTP_RX_TS1_4_REG_RX_TS_NSEC_LEN     16
	#define PTP_RX_TS1_4_REG_RX_TS_NSEC_DEFAULT 0x0

struct ptp_rx_ts1_4_reg {
	a_uint16_t  rx_ts_nsec:16;
};

union ptp_rx_ts1_4_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts1_4_reg bf;
};

/*[register] PTP_RX_TS1_5_REG*/
#define PTP_RX_TS1_5_REG
#define PTP_RX_TS1_5_REG_ADDRESS 0x850b
#define PTP_RX_TS1_5_REG_NUM     1
#define PTP_RX_TS1_5_REG_INC     0x1
#define PTP_RX_TS1_5_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS1_5_REG_DEFAULT 0x0
	/*[field] RX_TS_NFSEC*/
	#define PTP_RX_TS1_5_REG_RX_TS_NFSEC
	#define PTP_RX_TS1_5_REG_RX_TS_NFSEC_OFFSET  0
	#define PTP_RX_TS1_5_REG_RX_TS_NFSEC_LEN     12
	#define PTP_RX_TS1_5_REG_RX_TS_NFSEC_DEFAULT 0x0
	/*[field] RX_MSG_TYPE*/
	#define PTP_RX_TS1_5_REG_RX_MSG_TYPE
	#define PTP_RX_TS1_5_REG_RX_MSG_TYPE_OFFSET  12
	#define PTP_RX_TS1_5_REG_RX_MSG_TYPE_LEN     4
	#define PTP_RX_TS1_5_REG_RX_MSG_TYPE_DEFAULT 0x0

struct ptp_rx_ts1_5_reg {
	a_uint16_t  rx_ts_nfsec:12;
	a_uint16_t  rx_msg_type:4;
};

union ptp_rx_ts1_5_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts1_5_reg bf;
};

/*[register] PTP_RX_TS1_6_REG*/
#define PTP_RX_TS1_6_REG
#define PTP_RX_TS1_6_REG_ADDRESS 0x850c
#define PTP_RX_TS1_6_REG_NUM     1
#define PTP_RX_TS1_6_REG_INC     0x1
#define PTP_RX_TS1_6_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS1_6_REG_DEFAULT 0x0
	/*[field] RX_TS_NFSEC*/
	#define PTP_RX_TS1_6_REG_RX_TS_NFSEC
	#define PTP_RX_TS1_6_REG_RX_TS_NFSEC_OFFSET  0
	#define PTP_RX_TS1_6_REG_RX_TS_NFSEC_LEN     8
	#define PTP_RX_TS1_6_REG_RX_TS_NFSEC_DEFAULT 0x0

struct ptp_rx_ts1_6_reg {
	a_uint16_t  rx_ts_nfsec:8;
};

union ptp_rx_ts1_6_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts1_6_reg bf;
};

/*[register] PTP_RX_SEQID2_REG*/
#define PTP_RX_SEQID2_REG
#define PTP_RX_SEQID2_REG_ADDRESS 0x851a
#define PTP_RX_SEQID2_REG_NUM     1
#define PTP_RX_SEQID2_REG_INC     0x1
#define PTP_RX_SEQID2_REG_TYPE    REG_TYPE_RW
#define PTP_RX_SEQID2_REG_DEFAULT 0x0
	/*[field] RX_SEQID*/
	#define PTP_RX_SEQID2_REG_RX_SEQID
	#define PTP_RX_SEQID2_REG_RX_SEQID_OFFSET  0
	#define PTP_RX_SEQID2_REG_RX_SEQID_LEN     16
	#define PTP_RX_SEQID2_REG_RX_SEQID_DEFAULT 0x0

struct ptp_rx_seqid2_reg {
	a_uint16_t  rx_seqid:16;
};

union ptp_rx_seqid2_reg_u {
	a_uint32_t val;
	struct ptp_rx_seqid2_reg bf;
};

/*[register] PTP_RX_PORTID2_0_REG*/
#define PTP_RX_PORTID2_0_REG
#define PTP_RX_PORTID2_0_REG_ADDRESS 0x851b
#define PTP_RX_PORTID2_0_REG_NUM     1
#define PTP_RX_PORTID2_0_REG_INC     0x1
#define PTP_RX_PORTID2_0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID2_0_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID2_0_REG_RX_PORTID
	#define PTP_RX_PORTID2_0_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID2_0_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID2_0_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid2_0_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid2_0_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid2_0_reg bf;
};

/*[register] PTP_RX_PORTID2_1_REG*/
#define PTP_RX_PORTID2_1_REG
#define PTP_RX_PORTID2_1_REG_ADDRESS 0x851c
#define PTP_RX_PORTID2_1_REG_NUM     1
#define PTP_RX_PORTID2_1_REG_INC     0x1
#define PTP_RX_PORTID2_1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID2_1_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID2_1_REG_RX_PORTID
	#define PTP_RX_PORTID2_1_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID2_1_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID2_1_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid2_1_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid2_1_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid2_1_reg bf;
};

/*[register] PTP_RX_PORTID2_2_REG*/
#define PTP_RX_PORTID2_2_REG
#define PTP_RX_PORTID2_2_REG_ADDRESS 0x851d
#define PTP_RX_PORTID2_2_REG_NUM     1
#define PTP_RX_PORTID2_2_REG_INC     0x1
#define PTP_RX_PORTID2_2_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID2_2_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID2_2_REG_RX_PORTID
	#define PTP_RX_PORTID2_2_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID2_2_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID2_2_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid2_2_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid2_2_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid2_2_reg bf;
};

/*[register] PTP_RX_PORTID2_3_REG*/
#define PTP_RX_PORTID2_3_REG
#define PTP_RX_PORTID2_3_REG_ADDRESS 0x851e
#define PTP_RX_PORTID2_3_REG_NUM     1
#define PTP_RX_PORTID2_3_REG_INC     0x1
#define PTP_RX_PORTID2_3_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID2_3_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID2_3_REG_RX_PORTID
	#define PTP_RX_PORTID2_3_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID2_3_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID2_3_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid2_3_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid2_3_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid2_3_reg bf;
};

/*[register] PTP_RX_PORTID2_4_REG*/
#define PTP_RX_PORTID2_4_REG
#define PTP_RX_PORTID2_4_REG_ADDRESS 0x851f
#define PTP_RX_PORTID2_4_REG_NUM     1
#define PTP_RX_PORTID2_4_REG_INC     0x1
#define PTP_RX_PORTID2_4_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID2_4_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID2_4_REG_RX_PORTID
	#define PTP_RX_PORTID2_4_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID2_4_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID2_4_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid2_4_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid2_4_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid2_4_reg bf;
};

/*[register] PTP_RX_TS2_0_REG*/
#define PTP_RX_TS2_0_REG
#define PTP_RX_TS2_0_REG_ADDRESS 0x8520
#define PTP_RX_TS2_0_REG_NUM     1
#define PTP_RX_TS2_0_REG_INC     0x1
#define PTP_RX_TS2_0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS2_0_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_RX_TS2_0_REG_RX_TS_SEC
	#define PTP_RX_TS2_0_REG_RX_TS_SEC_OFFSET  0
	#define PTP_RX_TS2_0_REG_RX_TS_SEC_LEN     16
	#define PTP_RX_TS2_0_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_rx_ts2_0_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_rx_ts2_0_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts2_0_reg bf;
};

/*[register] PTP_RX_TS2_1_REG*/
#define PTP_RX_TS2_1_REG
#define PTP_RX_TS2_1_REG_ADDRESS 0x8521
#define PTP_RX_TS2_1_REG_NUM     1
#define PTP_RX_TS2_1_REG_INC     0x1
#define PTP_RX_TS2_1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS2_1_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_RX_TS2_1_REG_RX_TS_SEC
	#define PTP_RX_TS2_1_REG_RX_TS_SEC_OFFSET  0
	#define PTP_RX_TS2_1_REG_RX_TS_SEC_LEN     16
	#define PTP_RX_TS2_1_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_rx_ts2_1_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_rx_ts2_1_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts2_1_reg bf;
};

/*[register] PTP_RX_TS2_2_REG*/
#define PTP_RX_TS2_2_REG
#define PTP_RX_TS2_2_REG_ADDRESS 0x8522
#define PTP_RX_TS2_2_REG_NUM     1
#define PTP_RX_TS2_2_REG_INC     0x1
#define PTP_RX_TS2_2_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS2_2_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_RX_TS2_2_REG_RX_TS_SEC
	#define PTP_RX_TS2_2_REG_RX_TS_SEC_OFFSET  0
	#define PTP_RX_TS2_2_REG_RX_TS_SEC_LEN     16
	#define PTP_RX_TS2_2_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_rx_ts2_2_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_rx_ts2_2_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts2_2_reg bf;
};

/*[register] PTP_RX_TS2_3_REG*/
#define PTP_RX_TS2_3_REG
#define PTP_RX_TS2_3_REG_ADDRESS 0x8523
#define PTP_RX_TS2_3_REG_NUM     1
#define PTP_RX_TS2_3_REG_INC     0x1
#define PTP_RX_TS2_3_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS2_3_REG_DEFAULT 0x0
	/*[field] RX_TS_NSEC*/
	#define PTP_RX_TS2_3_REG_RX_TS_NSEC
	#define PTP_RX_TS2_3_REG_RX_TS_NSEC_OFFSET  0
	#define PTP_RX_TS2_3_REG_RX_TS_NSEC_LEN     16
	#define PTP_RX_TS2_3_REG_RX_TS_NSEC_DEFAULT 0x0

struct ptp_rx_ts2_3_reg {
	a_uint16_t  rx_ts_nsec:16;
};

union ptp_rx_ts2_3_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts2_3_reg bf;
};

/*[register] PTP_RX_TS2_4_REG*/
#define PTP_RX_TS2_4_REG
#define PTP_RX_TS2_4_REG_ADDRESS 0x8524
#define PTP_RX_TS2_4_REG_NUM     1
#define PTP_RX_TS2_4_REG_INC     0x1
#define PTP_RX_TS2_4_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS2_4_REG_DEFAULT 0x0
	/*[field] RX_TS_NSEC*/
	#define PTP_RX_TS2_4_REG_RX_TS_NSEC
	#define PTP_RX_TS2_4_REG_RX_TS_NSEC_OFFSET  0
	#define PTP_RX_TS2_4_REG_RX_TS_NSEC_LEN     16
	#define PTP_RX_TS2_4_REG_RX_TS_NSEC_DEFAULT 0x0

struct ptp_rx_ts2_4_reg {
	a_uint16_t  rx_ts_nsec:16;
};

union ptp_rx_ts2_4_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts2_4_reg bf;
};

/*[register] PTP_RX_TS2_5_REG*/
#define PTP_RX_TS2_5_REG
#define PTP_RX_TS2_5_REG_ADDRESS 0x8525
#define PTP_RX_TS2_5_REG_NUM     1
#define PTP_RX_TS2_5_REG_INC     0x1
#define PTP_RX_TS2_5_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS2_5_REG_DEFAULT 0x0
	/*[field] RX_TS_NFSEC*/
	#define PTP_RX_TS2_5_REG_RX_TS_NFSEC
	#define PTP_RX_TS2_5_REG_RX_TS_NFSEC_OFFSET  0
	#define PTP_RX_TS2_5_REG_RX_TS_NFSEC_LEN     12
	#define PTP_RX_TS2_5_REG_RX_TS_NFSEC_DEFAULT 0x0
	/*[field] RX_MSG_TYPE*/
	#define PTP_RX_TS2_5_REG_RX_MSG_TYPE
	#define PTP_RX_TS2_5_REG_RX_MSG_TYPE_OFFSET  12
	#define PTP_RX_TS2_5_REG_RX_MSG_TYPE_LEN     4
	#define PTP_RX_TS2_5_REG_RX_MSG_TYPE_DEFAULT 0x0

struct ptp_rx_ts2_5_reg {
	a_uint16_t  rx_ts_nfsec:12;
	a_uint16_t  rx_msg_type:4;
};

union ptp_rx_ts2_5_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts2_5_reg bf;
};

/*[register] PTP_RX_TS2_6_REG*/
#define PTP_RX_TS2_6_REG
#define PTP_RX_TS2_6_REG_ADDRESS 0x8526
#define PTP_RX_TS2_6_REG_NUM     1
#define PTP_RX_TS2_6_REG_INC     0x1
#define PTP_RX_TS2_6_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS2_6_REG_DEFAULT 0x0
	/*[field] RX_TS_NFSEC*/
	#define PTP_RX_TS2_6_REG_RX_TS_NFSEC
	#define PTP_RX_TS2_6_REG_RX_TS_NFSEC_OFFSET  0
	#define PTP_RX_TS2_6_REG_RX_TS_NFSEC_LEN     8
	#define PTP_RX_TS2_6_REG_RX_TS_NFSEC_DEFAULT 0x0

struct ptp_rx_ts2_6_reg {
	a_uint16_t  rx_ts_nfsec:8;
};

union ptp_rx_ts2_6_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts2_6_reg bf;
};

/*[register] PTP_RX_SEQID3_REG*/
#define PTP_RX_SEQID3_REG
#define PTP_RX_SEQID3_REG_ADDRESS 0x8534
#define PTP_RX_SEQID3_REG_NUM     1
#define PTP_RX_SEQID3_REG_INC     0x1
#define PTP_RX_SEQID3_REG_TYPE    REG_TYPE_RW
#define PTP_RX_SEQID3_REG_DEFAULT 0x0
	/*[field] RX_SEQID*/
	#define PTP_RX_SEQID3_REG_RX_SEQID
	#define PTP_RX_SEQID3_REG_RX_SEQID_OFFSET  0
	#define PTP_RX_SEQID3_REG_RX_SEQID_LEN     16
	#define PTP_RX_SEQID3_REG_RX_SEQID_DEFAULT 0x0

struct ptp_rx_seqid3_reg {
	a_uint16_t  rx_seqid:16;
};

union ptp_rx_seqid3_reg_u {
	a_uint32_t val;
	struct ptp_rx_seqid3_reg bf;
};

/*[register] PTP_RX_PORTID3_0_REG*/
#define PTP_RX_PORTID3_0_REG
#define PTP_RX_PORTID3_0_REG_ADDRESS 0x8535
#define PTP_RX_PORTID3_0_REG_NUM     1
#define PTP_RX_PORTID3_0_REG_INC     0x1
#define PTP_RX_PORTID3_0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID3_0_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID3_0_REG_RX_PORTID
	#define PTP_RX_PORTID3_0_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID3_0_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID3_0_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid3_0_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid3_0_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid3_0_reg bf;
};

/*[register] PTP_RX_PORTID3_1_REG*/
#define PTP_RX_PORTID3_1_REG
#define PTP_RX_PORTID3_1_REG_ADDRESS 0x8536
#define PTP_RX_PORTID3_1_REG_NUM     1
#define PTP_RX_PORTID3_1_REG_INC     0x1
#define PTP_RX_PORTID3_1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID3_1_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID3_1_REG_RX_PORTID
	#define PTP_RX_PORTID3_1_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID3_1_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID3_1_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid3_1_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid3_1_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid3_1_reg bf;
};

/*[register] PTP_RX_PORTID3_2_REG*/
#define PTP_RX_PORTID3_2_REG
#define PTP_RX_PORTID3_2_REG_ADDRESS 0x8537
#define PTP_RX_PORTID3_2_REG_NUM     1
#define PTP_RX_PORTID3_2_REG_INC     0x1
#define PTP_RX_PORTID3_2_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID3_2_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID3_2_REG_RX_PORTID
	#define PTP_RX_PORTID3_2_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID3_2_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID3_2_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid3_2_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid3_2_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid3_2_reg bf;
};

/*[register] PTP_RX_PORTID3_3_REG*/
#define PTP_RX_PORTID3_3_REG
#define PTP_RX_PORTID3_3_REG_ADDRESS 0x8538
#define PTP_RX_PORTID3_3_REG_NUM     1
#define PTP_RX_PORTID3_3_REG_INC     0x1
#define PTP_RX_PORTID3_3_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID3_3_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID3_3_REG_RX_PORTID
	#define PTP_RX_PORTID3_3_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID3_3_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID3_3_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid3_3_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid3_3_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid3_3_reg bf;
};

/*[register] PTP_RX_PORTID3_4_REG*/
#define PTP_RX_PORTID3_4_REG
#define PTP_RX_PORTID3_4_REG_ADDRESS 0x8539
#define PTP_RX_PORTID3_4_REG_NUM     1
#define PTP_RX_PORTID3_4_REG_INC     0x1
#define PTP_RX_PORTID3_4_REG_TYPE    REG_TYPE_RW
#define PTP_RX_PORTID3_4_REG_DEFAULT 0x0
	/*[field] RX_PORTID*/
	#define PTP_RX_PORTID3_4_REG_RX_PORTID
	#define PTP_RX_PORTID3_4_REG_RX_PORTID_OFFSET  0
	#define PTP_RX_PORTID3_4_REG_RX_PORTID_LEN     16
	#define PTP_RX_PORTID3_4_REG_RX_PORTID_DEFAULT 0x0

struct ptp_rx_portid3_4_reg {
	a_uint16_t  rx_portid:16;
};

union ptp_rx_portid3_4_reg_u {
	a_uint32_t val;
	struct ptp_rx_portid3_4_reg bf;
};

/*[register] PTP_RX_TS3_0_REG*/
#define PTP_RX_TS3_0_REG
#define PTP_RX_TS3_0_REG_ADDRESS 0x853a
#define PTP_RX_TS3_0_REG_NUM     1
#define PTP_RX_TS3_0_REG_INC     0x1
#define PTP_RX_TS3_0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS3_0_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_RX_TS3_0_REG_RX_TS_SEC
	#define PTP_RX_TS3_0_REG_RX_TS_SEC_OFFSET  0
	#define PTP_RX_TS3_0_REG_RX_TS_SEC_LEN     16
	#define PTP_RX_TS3_0_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_rx_ts3_0_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_rx_ts3_0_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts3_0_reg bf;
};

/*[register] PTP_RX_TS3_1_REG*/
#define PTP_RX_TS3_1_REG
#define PTP_RX_TS3_1_REG_ADDRESS 0x853b
#define PTP_RX_TS3_1_REG_NUM     1
#define PTP_RX_TS3_1_REG_INC     0x1
#define PTP_RX_TS3_1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS3_1_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_RX_TS3_1_REG_RX_TS_SEC
	#define PTP_RX_TS3_1_REG_RX_TS_SEC_OFFSET  0
	#define PTP_RX_TS3_1_REG_RX_TS_SEC_LEN     16
	#define PTP_RX_TS3_1_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_rx_ts3_1_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_rx_ts3_1_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts3_1_reg bf;
};

/*[register] PTP_RX_TS3_2_REG*/
#define PTP_RX_TS3_2_REG
#define PTP_RX_TS3_2_REG_ADDRESS 0x853c
#define PTP_RX_TS3_2_REG_NUM     1
#define PTP_RX_TS3_2_REG_INC     0x1
#define PTP_RX_TS3_2_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS3_2_REG_DEFAULT 0x0
	/*[field] RX_TS_SEC*/
	#define PTP_RX_TS3_2_REG_RX_TS_SEC
	#define PTP_RX_TS3_2_REG_RX_TS_SEC_OFFSET  0
	#define PTP_RX_TS3_2_REG_RX_TS_SEC_LEN     16
	#define PTP_RX_TS3_2_REG_RX_TS_SEC_DEFAULT 0x0

struct ptp_rx_ts3_2_reg {
	a_uint16_t  rx_ts_sec:16;
};

union ptp_rx_ts3_2_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts3_2_reg bf;
};

/*[register] PTP_RX_TS3_3_REG*/
#define PTP_RX_TS3_3_REG
#define PTP_RX_TS3_3_REG_ADDRESS 0x853d
#define PTP_RX_TS3_3_REG_NUM     1
#define PTP_RX_TS3_3_REG_INC     0x1
#define PTP_RX_TS3_3_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS3_3_REG_DEFAULT 0x0
	/*[field] RX_TS_NSEC*/
	#define PTP_RX_TS3_3_REG_RX_TS_NSEC
	#define PTP_RX_TS3_3_REG_RX_TS_NSEC_OFFSET  0
	#define PTP_RX_TS3_3_REG_RX_TS_NSEC_LEN     16
	#define PTP_RX_TS3_3_REG_RX_TS_NSEC_DEFAULT 0x0

struct ptp_rx_ts3_3_reg {
	a_uint16_t  rx_ts_nsec:16;
};

union ptp_rx_ts3_3_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts3_3_reg bf;
};

/*[register] PTP_RX_TS3_4_REG*/
#define PTP_RX_TS3_4_REG
#define PTP_RX_TS3_4_REG_ADDRESS 0x853e
#define PTP_RX_TS3_4_REG_NUM     1
#define PTP_RX_TS3_4_REG_INC     0x1
#define PTP_RX_TS3_4_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS3_4_REG_DEFAULT 0x0
	/*[field] RX_TS_NSEC*/
	#define PTP_RX_TS3_4_REG_RX_TS_NSEC
	#define PTP_RX_TS3_4_REG_RX_TS_NSEC_OFFSET  0
	#define PTP_RX_TS3_4_REG_RX_TS_NSEC_LEN     16
	#define PTP_RX_TS3_4_REG_RX_TS_NSEC_DEFAULT 0x0

struct ptp_rx_ts3_4_reg {
	a_uint16_t  rx_ts_nsec:16;
};

union ptp_rx_ts3_4_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts3_4_reg bf;
};

/*[register] PTP_RX_TS3_5_REG*/
#define PTP_RX_TS3_5_REG
#define PTP_RX_TS3_5_REG_ADDRESS 0x853f
#define PTP_RX_TS3_5_REG_NUM     1
#define PTP_RX_TS3_5_REG_INC     0x1
#define PTP_RX_TS3_5_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS3_5_REG_DEFAULT 0x0
	/*[field] RX_TS_NFSEC*/
	#define PTP_RX_TS3_5_REG_RX_TS_NFSEC
	#define PTP_RX_TS3_5_REG_RX_TS_NFSEC_OFFSET  0
	#define PTP_RX_TS3_5_REG_RX_TS_NFSEC_LEN     12
	#define PTP_RX_TS3_5_REG_RX_TS_NFSEC_DEFAULT 0x0
	/*[field] RX_MSG_TYPE*/
	#define PTP_RX_TS3_5_REG_RX_MSG_TYPE
	#define PTP_RX_TS3_5_REG_RX_MSG_TYPE_OFFSET  12
	#define PTP_RX_TS3_5_REG_RX_MSG_TYPE_LEN     4
	#define PTP_RX_TS3_5_REG_RX_MSG_TYPE_DEFAULT 0x0

struct ptp_rx_ts3_5_reg {
	a_uint16_t  rx_ts_nfsec:12;
	a_uint16_t  rx_msg_type:4;
};

union ptp_rx_ts3_5_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts3_5_reg bf;
};

/*[register] PTP_RX_TS3_6_REG*/
#define PTP_RX_TS3_6_REG
#define PTP_RX_TS3_6_REG_ADDRESS 0x8540
#define PTP_RX_TS3_6_REG_NUM     1
#define PTP_RX_TS3_6_REG_INC     0x1
#define PTP_RX_TS3_6_REG_TYPE    REG_TYPE_RW
#define PTP_RX_TS3_6_REG_DEFAULT 0x0
	/*[field] RX_TS_NFSEC*/
	#define PTP_RX_TS3_6_REG_RX_TS_NFSEC
	#define PTP_RX_TS3_6_REG_RX_TS_NFSEC_OFFSET  0
	#define PTP_RX_TS3_6_REG_RX_TS_NFSEC_LEN     8
	#define PTP_RX_TS3_6_REG_RX_TS_NFSEC_DEFAULT 0x0

struct ptp_rx_ts3_6_reg {
	a_uint16_t  rx_ts_nfsec:8;
};

union ptp_rx_ts3_6_reg_u {
	a_uint32_t val;
	struct ptp_rx_ts3_6_reg bf;
};

/*[register] PTP_PHASE_ADJUST_0*/
#define PTP_PHASE_ADJUST_0_REG
#define PTP_PHASE_ADJUST_0_REG_ADDRESS 0x8300
#define PTP_PHASE_ADJUST_0_REG_NUM     1
#define PTP_PHASE_ADJUST_0_REG_INC     0x1
#define PTP_PHASE_ADJUST_0_REG_TYPE    REG_TYPE_RW
#define PTP_PHASE_ADJUST_0_REG_DEFAULT 0x0
	/*[field] PTP_PHASE_ADJUST_0*/
	#define PTP_PHASE_ADJUST_0_REG_PHASE_VALUE
	#define PTP_PHASE_ADJUST_0_REG_PHASE_VALUE_OFFSET  0
	#define PTP_PHASE_ADJUST_0_REG_PHASE_VALUE_LEN     16
	#define PTP_PHASE_ADJUST_0_REG_PHASE_VALUE_DEFAULT 0x0

struct ptp_phase_adjust_0_reg {
	a_uint16_t  phase_value:16;
};

union ptp_phase_adjust_0_reg_u {
	a_uint32_t val;
	struct ptp_phase_adjust_0_reg bf;
};

/*[register] PTP_PHASE_ADJUST_1*/
#define PTP_PHASE_ADJUST_1_REG
#define PTP_PHASE_ADJUST_1_REG_ADDRESS 0x8301
#define PTP_PHASE_ADJUST_1_REG_NUM     1
#define PTP_PHASE_ADJUST_1_REG_INC     0x1
#define PTP_PHASE_ADJUST_1_REG_TYPE    REG_TYPE_RW
#define PTP_PHASE_ADJUST_1_REG_DEFAULT 0x0
	/*[field] PTP_PHASE_ADJUST_1*/
	#define PTP_PHASE_ADJUST_1_REG_PHASE_VALUE
	#define PTP_PHASE_ADJUST_1_REG_PHASE_VALUE_OFFSET  0
	#define PTP_PHASE_ADJUST_1_REG_PHASE_VALUE_LEN     16
	#define PTP_PHASE_ADJUST_1_REG_PHASE_VALUE_DEFAULT 0x0

struct ptp_phase_adjust_1_reg {
	a_uint16_t  phase_value:16;
};

union ptp_phase_adjust_1_reg_u {
	a_uint32_t val;
	struct ptp_phase_adjust_1_reg bf;
};

/*[register] PTP_PPS_PUL_WIDTH_0*/
#define PTP_PPS_PUL_WIDTH_0_REG
#define PTP_PPS_PUL_WIDTH_0_REG_ADDRESS 0x8303
#define PTP_PPS_PUL_WIDTH_0_REG_NUM     1
#define PTP_PPS_PUL_WIDTH_0_REG_INC     0x1
#define PTP_PPS_PUL_WIDTH_0_REG_TYPE    REG_TYPE_RW
#define PTP_PPS_PUL_WIDTH_0_REG_DEFAULT 0x0
	/*[field] PTP_PPS_PUL_WIDTH_0*/
	#define PTP_PPS_PUL_WIDTH_0_REG_PUL_VALUE
	#define PTP_PPS_PUL_WIDTH_0_REG_PUL_VALUE_OFFSET  0
	#define PTP_PPS_PUL_WIDTH_0_REG_PUL_VALUE_LEN     16
	#define PTP_PPS_PUL_WIDTH_0_REG_PUL_VALUE_DEFAULT 0x0

struct ptp_pps_pul_width_0_reg {
	a_uint16_t  pul_value:16;
};

union ptp_pps_pul_width_0_reg_u {
	a_uint32_t val;
	struct ptp_pps_pul_width_0_reg bf;
};

/*[register] PTP_PPS_PUL_WIDTH_1*/
#define PTP_PPS_PUL_WIDTH_1_REG
#define PTP_PPS_PUL_WIDTH_1_REG_ADDRESS 0x8304
#define PTP_PPS_PUL_WIDTH_1_REG_NUM     1
#define PTP_PPS_PUL_WIDTH_1_REG_INC     0x1
#define PTP_PPS_PUL_WIDTH_1_REG_TYPE    REG_TYPE_RW
#define PTP_PPS_PUL_WIDTH_1_REG_DEFAULT 0x0
	/*[field] PTP_PPS_PUL_WIDTH_1*/
	#define PTP_PPS_PUL_WIDTH_1_REG_PUL_VALUE
	#define PTP_PPS_PUL_WIDTH_1_REG_PUL_VALUE_OFFSET  0
	#define PTP_PPS_PUL_WIDTH_1_REG_PUL_VALUE_LEN     16
	#define PTP_PPS_PUL_WIDTH_1_REG_PUL_VALUE_DEFAULT 0x0

struct ptp_pps_pul_width_1_reg {
	a_uint16_t  pul_value:16;
};

union ptp_pps_pul_width_1_reg_u {
	a_uint32_t val;
	struct ptp_pps_pul_width_1_reg bf;
};

/*[register] PTP_FREQ_WAVEFORM_PERIOD_0_REG*/
#define PTP_FREQ_WAVEFORM_PERIOD_0_REG
#define PTP_FREQ_WAVEFORM_PERIOD_0_REG_ADDRESS 0x8305
#define PTP_FREQ_WAVEFORM_PERIOD_0_REG_NUM     1
#define PTP_FREQ_WAVEFORM_PERIOD_0_REG_INC     0x1
#define PTP_FREQ_WAVEFORM_PERIOD_0_REG_TYPE    REG_TYPE_RW
#define PTP_FREQ_WAVEFORM_PERIOD_0_REG_DEFAULT 0x0
	/*[field] WAVE_PERIOD*/
	#define PTP_FREQ_WAVEFORM_PERIOD_0_REG_WAVE_PERIOD
	#define PTP_FREQ_WAVEFORM_PERIOD_0_REG_WAVE_PERIOD_OFFSET  0
	#define PTP_FREQ_WAVEFORM_PERIOD_0_REG_WAVE_PERIOD_LEN     15
	#define PTP_FREQ_WAVEFORM_PERIOD_0_REG_WAVE_PERIOD_DEFAULT 0x0
	/*[field] PHASE_ALI*/
	#define PTP_FREQ_WAVEFORM_PERIOD_0_REG_PHASE_ALI
	#define PTP_FREQ_WAVEFORM_PERIOD_0_REG_PHASE_ALI_OFFSET  15
	#define PTP_FREQ_WAVEFORM_PERIOD_0_REG_PHASE_ALI_LEN     1
	#define PTP_FREQ_WAVEFORM_PERIOD_0_REG_PHASE_ALI_DEFAULT 0x0

struct ptp_freq_waveform_period_0_reg {
	a_uint16_t  wave_period:15;
	a_uint16_t  phase_ali:1;
};

union ptp_freq_waveform_period_0_reg_u {
	a_uint32_t val;
	struct ptp_freq_waveform_period_0_reg bf;
};

/*[register] PTP_FREQ_WAVEFORM_PERIOD_1_REG*/
#define PTP_FREQ_WAVEFORM_PERIOD_1_REG
#define PTP_FREQ_WAVEFORM_PERIOD_1_REG_ADDRESS 0x8306
#define PTP_FREQ_WAVEFORM_PERIOD_1_REG_NUM     1
#define PTP_FREQ_WAVEFORM_PERIOD_1_REG_INC     0x1
#define PTP_FREQ_WAVEFORM_PERIOD_1_REG_TYPE    REG_TYPE_RW
#define PTP_FREQ_WAVEFORM_PERIOD_1_REG_DEFAULT 0x0
	/*[field] WAVE_PERIOD*/
	#define PTP_FREQ_WAVEFORM_PERIOD_1_REG_WAVE_PERIOD
	#define PTP_FREQ_WAVEFORM_PERIOD_1_REG_WAVE_PERIOD_OFFSET  0
	#define PTP_FREQ_WAVEFORM_PERIOD_1_REG_WAVE_PERIOD_LEN     16
	#define PTP_FREQ_WAVEFORM_PERIOD_1_REG_WAVE_PERIOD_DEFAULT 0x0

struct ptp_freq_waveform_period_1_reg {
	a_uint16_t  wave_period:16;
};

union ptp_freq_waveform_period_1_reg_u {
	a_uint32_t val;
	struct ptp_freq_waveform_period_1_reg bf;
};

/*[register] PTP_FREQ_WAVEFORM_PERIOD_2_REG*/
#define PTP_FREQ_WAVEFORM_PERIOD_2_REG
#define PTP_FREQ_WAVEFORM_PERIOD_2_REG_ADDRESS 0x8307
#define PTP_FREQ_WAVEFORM_PERIOD_2_REG_NUM     1
#define PTP_FREQ_WAVEFORM_PERIOD_2_REG_INC     0x1
#define PTP_FREQ_WAVEFORM_PERIOD_2_REG_TYPE    REG_TYPE_RW
#define PTP_FREQ_WAVEFORM_PERIOD_2_REG_DEFAULT 0x0
	/*[field] WAVE_PERIOD*/
	#define PTP_FREQ_WAVEFORM_PERIOD_2_REG_WAVE_PERIOD
	#define PTP_FREQ_WAVEFORM_PERIOD_2_REG_WAVE_PERIOD_OFFSET  0
	#define PTP_FREQ_WAVEFORM_PERIOD_2_REG_WAVE_PERIOD_LEN     16
	#define PTP_FREQ_WAVEFORM_PERIOD_2_REG_WAVE_PERIOD_DEFAULT 0x0

struct ptp_freq_waveform_period_2_reg {
	a_uint16_t  wave_period:16;
};

union ptp_freq_waveform_period_2_reg_u {
	a_uint32_t val;
	struct ptp_freq_waveform_period_2_reg bf;
};

/*[register] PTP_RX_COM_TS_CTRL_REG*/
#define PTP_RX_COM_TS_CTRL_REG
#define PTP_RX_COM_TS_CTRL_REG_ADDRESS 0x8600
#define PTP_RX_COM_TS_CTRL_REG_NUM     1
#define PTP_RX_COM_TS_CTRL_REG_INC     0x1
#define PTP_RX_COM_TS_CTRL_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_TS_CTRL_REG_DEFAULT 0x0
	/*[field] FILT_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_FILT_EN
	#define PTP_RX_COM_TS_CTRL_REG_FILT_EN_OFFSET  0
	#define PTP_RX_COM_TS_CTRL_REG_FILT_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_FILT_EN_DEFAULT 0x0
	/*[field] MAC_LENGTHTYPE_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_MAC_LENGTHTYPE_EN
	#define PTP_RX_COM_TS_CTRL_REG_MAC_LENGTHTYPE_EN_OFFSET  1
	#define PTP_RX_COM_TS_CTRL_REG_MAC_LENGTHTYPE_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_MAC_LENGTHTYPE_EN_DEFAULT 0x0
	/*[field] MAC_DA_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_MAC_DA_EN
	#define PTP_RX_COM_TS_CTRL_REG_MAC_DA_EN_OFFSET  2
	#define PTP_RX_COM_TS_CTRL_REG_MAC_DA_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_MAC_DA_EN_DEFAULT 0x0
	/*[field] MAC_PTP_FILT_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_MAC_PTP_FILT_EN
	#define PTP_RX_COM_TS_CTRL_REG_MAC_PTP_FILT_EN_OFFSET  3
	#define PTP_RX_COM_TS_CTRL_REG_MAC_PTP_FILT_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_MAC_PTP_FILT_EN_DEFAULT 0x0
	/*[field] IPV4_LAYER4_PROTOCOL_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_IPV4_LAYER4_PROTOCOL_EN
	#define PTP_RX_COM_TS_CTRL_REG_IPV4_LAYER4_PROTOCOL_EN_OFFSET  4
	#define PTP_RX_COM_TS_CTRL_REG_IPV4_LAYER4_PROTOCOL_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_IPV4_LAYER4_PROTOCOL_EN_DEFAULT 0x0
	/*[field] IPV4_DA_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_IPV4_DA_EN
	#define PTP_RX_COM_TS_CTRL_REG_IPV4_DA_EN_OFFSET  5
	#define PTP_RX_COM_TS_CTRL_REG_IPV4_DA_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_IPV4_DA_EN_DEFAULT 0x0
	/*[field] IPV4_PTP_FILT_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_IPV4_PTP_FILT_EN
	#define PTP_RX_COM_TS_CTRL_REG_IPV4_PTP_FILT_EN_OFFSET  6
	#define PTP_RX_COM_TS_CTRL_REG_IPV4_PTP_FILT_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_IPV4_PTP_FILT_EN_DEFAULT 0x0
	/*[field] IPV6_NEXT_HEADER_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_IPV6_NEXT_HEADER_EN
	#define PTP_RX_COM_TS_CTRL_REG_IPV6_NEXT_HEADER_EN_OFFSET  7
	#define PTP_RX_COM_TS_CTRL_REG_IPV6_NEXT_HEADER_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_IPV6_NEXT_HEADER_EN_DEFAULT 0x0
	/*[field] IPV6_DA_FILT_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_IPV6_DA_FILT_EN
	#define PTP_RX_COM_TS_CTRL_REG_IPV6_DA_FILT_EN_OFFSET  8
	#define PTP_RX_COM_TS_CTRL_REG_IPV6_DA_FILT_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_IPV6_DA_FILT_EN_DEFAULT 0x0
	/*[field] IPV6_PTP_FILT_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_IPV6_PTP_FILT_EN
	#define PTP_RX_COM_TS_CTRL_REG_IPV6_PTP_FILT_EN_OFFSET  9
	#define PTP_RX_COM_TS_CTRL_REG_IPV6_PTP_FILT_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_IPV6_PTP_FILT_EN_DEFAULT 0x0
	/*[field] UDP_DPORT_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_UDP_DPORT_EN
	#define PTP_RX_COM_TS_CTRL_REG_UDP_DPORT_EN_OFFSET  10
	#define PTP_RX_COM_TS_CTRL_REG_UDP_DPORT_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_UDP_DPORT_EN_DEFAULT 0x0
	/*[field] UDP_PTP_EVENT_FILT_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_UDP_PTP_EVENT_FILT_EN
	#define PTP_RX_COM_TS_CTRL_REG_UDP_PTP_EVENT_FILT_EN_OFFSET  11
	#define PTP_RX_COM_TS_CTRL_REG_UDP_PTP_EVENT_FILT_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_UDP_PTP_EVENT_FILT_EN_DEFAULT 0x0
	/*[field] Y1731_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_Y1731_EN
	#define PTP_RX_COM_TS_CTRL_REG_Y1731_EN_OFFSET  12
	#define PTP_RX_COM_TS_CTRL_REG_Y1731_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_Y1731_EN_DEFAULT 0x0
	/*[field] Y1731_INSERT_TS_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_Y1731_INSERT_TS_EN
	#define PTP_RX_COM_TS_CTRL_REG_Y1731_INSERT_TS_EN_OFFSET  13
	#define PTP_RX_COM_TS_CTRL_REG_Y1731_INSERT_TS_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_Y1731_INSERT_TS_EN_DEFAULT 0x0
	/*[field] Y1731_DA_CHK_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_Y1731_DA_CHK_EN
	#define PTP_RX_COM_TS_CTRL_REG_Y1731_DA_CHK_EN_OFFSET  14
	#define PTP_RX_COM_TS_CTRL_REG_Y1731_DA_CHK_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_Y1731_DA_CHK_EN_DEFAULT 0x0
	/*[field] PW_MAC_EN*/
	#define PTP_RX_COM_TS_CTRL_REG_PW_MAC_EN
	#define PTP_RX_COM_TS_CTRL_REG_PW_MAC_EN_OFFSET  15
	#define PTP_RX_COM_TS_CTRL_REG_PW_MAC_EN_LEN     1
	#define PTP_RX_COM_TS_CTRL_REG_PW_MAC_EN_DEFAULT 0x0

struct ptp_rx_com_ts_ctrl_reg {
	a_uint32_t  filt_en:1;
	a_uint32_t  mac_lengthtype_en:1;
	a_uint32_t  mac_da_en:1;
	a_uint32_t  mac_ptp_filt_en:1;
	a_uint32_t  ipv4_layer4_protocol_en:1;
	a_uint32_t  ipv4_da_en:1;
	a_uint32_t  ipv4_ptp_filt_en:1;
	a_uint32_t  ipv6_next_header_en:1;
	a_uint32_t  ipv6_da_filt_en:1;
	a_uint32_t  ipv6_ptp_filt_en:1;
	a_uint32_t  udp_dport_en:1;
	a_uint32_t  udp_ptp_event_filt_en:1;
	a_uint32_t  y1731_en:1;
	a_uint32_t  y1731_insert_ts_en:1;
	a_uint32_t  y1731_da_chk_en:1;
	a_uint32_t  pw_mac_en:1;
};

union ptp_rx_com_ts_ctrl_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_ts_ctrl_reg bf;
};

/*[register] PTP_RX_FILT_MAC_DA0_REG*/
#define PTP_RX_FILT_MAC_DA0_REG
#define PTP_RX_FILT_MAC_DA0_REG_ADDRESS 0x8601
#define PTP_RX_FILT_MAC_DA0_REG_NUM     1
#define PTP_RX_FILT_MAC_DA0_REG_INC     0x1
#define PTP_RX_FILT_MAC_DA0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_MAC_DA0_REG_DEFAULT 0x0
	/*[field] MAC_ADDR*/
	#define PTP_RX_FILT_MAC_DA0_REG_MAC_ADDR
	#define PTP_RX_FILT_MAC_DA0_REG_MAC_ADDR_OFFSET  0
	#define PTP_RX_FILT_MAC_DA0_REG_MAC_ADDR_LEN     16
	#define PTP_RX_FILT_MAC_DA0_REG_MAC_ADDR_DEFAULT 0x0

struct ptp_rx_filt_mac_da0_reg {
	a_uint32_t  mac_addr:16;
};

union ptp_rx_filt_mac_da0_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_mac_da0_reg bf;
};

/*[register] PTP_RX_FILT_MAC_DA1_REG*/
#define PTP_RX_FILT_MAC_DA1_REG
#define PTP_RX_FILT_MAC_DA1_REG_ADDRESS 0x8602
#define PTP_RX_FILT_MAC_DA1_REG_NUM     1
#define PTP_RX_FILT_MAC_DA1_REG_INC     0x1
#define PTP_RX_FILT_MAC_DA1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_MAC_DA1_REG_DEFAULT 0x0
	/*[field] MAC_ADDR*/
	#define PTP_RX_FILT_MAC_DA1_REG_MAC_ADDR
	#define PTP_RX_FILT_MAC_DA1_REG_MAC_ADDR_OFFSET  0
	#define PTP_RX_FILT_MAC_DA1_REG_MAC_ADDR_LEN     16
	#define PTP_RX_FILT_MAC_DA1_REG_MAC_ADDR_DEFAULT 0x0

struct ptp_rx_filt_mac_da1_reg {
	a_uint32_t  mac_addr:16;
};

union ptp_rx_filt_mac_da1_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_mac_da1_reg bf;
};

/*[register] PTP_RX_FILT_MAC_DA2_REG*/
#define PTP_RX_FILT_MAC_DA2_REG
#define PTP_RX_FILT_MAC_DA2_REG_ADDRESS 0x8603
#define PTP_RX_FILT_MAC_DA2_REG_NUM     1
#define PTP_RX_FILT_MAC_DA2_REG_INC     0x1
#define PTP_RX_FILT_MAC_DA2_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_MAC_DA2_REG_DEFAULT 0x0
	/*[field] MAC_ADDR*/
	#define PTP_RX_FILT_MAC_DA2_REG_MAC_ADDR
	#define PTP_RX_FILT_MAC_DA2_REG_MAC_ADDR_OFFSET  0
	#define PTP_RX_FILT_MAC_DA2_REG_MAC_ADDR_LEN     16
	#define PTP_RX_FILT_MAC_DA2_REG_MAC_ADDR_DEFAULT 0x0

struct ptp_rx_filt_mac_da2_reg {
	a_uint32_t  mac_addr:16;
};

union ptp_rx_filt_mac_da2_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_mac_da2_reg bf;
};

/*[register] PTP_RX_FILT_IPV4_DA0_REG*/
#define PTP_RX_FILT_IPV4_DA0_REG
#define PTP_RX_FILT_IPV4_DA0_REG_ADDRESS 0x8604
#define PTP_RX_FILT_IPV4_DA0_REG_NUM     1
#define PTP_RX_FILT_IPV4_DA0_REG_INC     0x1
#define PTP_RX_FILT_IPV4_DA0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_IPV4_DA0_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_RX_FILT_IPV4_DA0_REG_IP_ADDR
	#define PTP_RX_FILT_IPV4_DA0_REG_IP_ADDR_OFFSET  0
	#define PTP_RX_FILT_IPV4_DA0_REG_IP_ADDR_LEN     16
	#define PTP_RX_FILT_IPV4_DA0_REG_IP_ADDR_DEFAULT 0x0

struct ptp_rx_filt_ipv4_da0_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_rx_filt_ipv4_da0_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_ipv4_da0_reg bf;
};

/*[register] PTP_RX_FILT_IPV4_DA1_REG*/
#define PTP_RX_FILT_IPV4_DA1_REG
#define PTP_RX_FILT_IPV4_DA1_REG_ADDRESS 0x8605
#define PTP_RX_FILT_IPV4_DA1_REG_NUM     1
#define PTP_RX_FILT_IPV4_DA1_REG_INC     0x1
#define PTP_RX_FILT_IPV4_DA1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_IPV4_DA1_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_RX_FILT_IPV4_DA1_REG_IP_ADDR
	#define PTP_RX_FILT_IPV4_DA1_REG_IP_ADDR_OFFSET  0
	#define PTP_RX_FILT_IPV4_DA1_REG_IP_ADDR_LEN     16
	#define PTP_RX_FILT_IPV4_DA1_REG_IP_ADDR_DEFAULT 0x0

struct ptp_rx_filt_ipv4_da1_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_rx_filt_ipv4_da1_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_ipv4_da1_reg bf;
};

/*[register] PTP_RX_FILT_IPV6_DA0_REG*/
#define PTP_RX_FILT_IPV6_DA0_REG
#define PTP_RX_FILT_IPV6_DA0_REG_ADDRESS 0x8606
#define PTP_RX_FILT_IPV6_DA0_REG_NUM     1
#define PTP_RX_FILT_IPV6_DA0_REG_INC     0x1
#define PTP_RX_FILT_IPV6_DA0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_IPV6_DA0_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_RX_FILT_IPV6_DA0_REG_IP_ADDR
	#define PTP_RX_FILT_IPV6_DA0_REG_IP_ADDR_OFFSET  0
	#define PTP_RX_FILT_IPV6_DA0_REG_IP_ADDR_LEN     16
	#define PTP_RX_FILT_IPV6_DA0_REG_IP_ADDR_DEFAULT 0x0

struct ptp_rx_filt_ipv6_da0_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_rx_filt_ipv6_da0_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_ipv6_da0_reg bf;
};

/*[register] PTP_RX_FILT_IPV6_DA1_REG*/
#define PTP_RX_FILT_IPV6_DA1_REG
#define PTP_RX_FILT_IPV6_DA1_REG_ADDRESS 0x8607
#define PTP_RX_FILT_IPV6_DA1_REG_NUM     1
#define PTP_RX_FILT_IPV6_DA1_REG_INC     0x1
#define PTP_RX_FILT_IPV6_DA1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_IPV6_DA1_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_RX_FILT_IPV6_DA1_REG_IP_ADDR
	#define PTP_RX_FILT_IPV6_DA1_REG_IP_ADDR_OFFSET  0
	#define PTP_RX_FILT_IPV6_DA1_REG_IP_ADDR_LEN     16
	#define PTP_RX_FILT_IPV6_DA1_REG_IP_ADDR_DEFAULT 0x0

struct ptp_rx_filt_ipv6_da1_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_rx_filt_ipv6_da1_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_ipv6_da1_reg bf;
};

/*[register] PTP_RX_FILT_IPV6_DA2_REG*/
#define PTP_RX_FILT_IPV6_DA2_REG
#define PTP_RX_FILT_IPV6_DA2_REG_ADDRESS 0x8608
#define PTP_RX_FILT_IPV6_DA2_REG_NUM     1
#define PTP_RX_FILT_IPV6_DA2_REG_INC     0x1
#define PTP_RX_FILT_IPV6_DA2_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_IPV6_DA2_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_RX_FILT_IPV6_DA2_REG_IP_ADDR
	#define PTP_RX_FILT_IPV6_DA2_REG_IP_ADDR_OFFSET  0
	#define PTP_RX_FILT_IPV6_DA2_REG_IP_ADDR_LEN     16
	#define PTP_RX_FILT_IPV6_DA2_REG_IP_ADDR_DEFAULT 0x0

struct ptp_rx_filt_ipv6_da2_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_rx_filt_ipv6_da2_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_ipv6_da2_reg bf;
};

/*[register] PTP_RX_FILT_IPV6_DA3_REG*/
#define PTP_RX_FILT_IPV6_DA3_REG
#define PTP_RX_FILT_IPV6_DA3_REG_ADDRESS 0x8609
#define PTP_RX_FILT_IPV6_DA3_REG_NUM     1
#define PTP_RX_FILT_IPV6_DA3_REG_INC     0x1
#define PTP_RX_FILT_IPV6_DA3_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_IPV6_DA3_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_RX_FILT_IPV6_DA3_REG_IP_ADDR
	#define PTP_RX_FILT_IPV6_DA3_REG_IP_ADDR_OFFSET  0
	#define PTP_RX_FILT_IPV6_DA3_REG_IP_ADDR_LEN     16
	#define PTP_RX_FILT_IPV6_DA3_REG_IP_ADDR_DEFAULT 0x0

struct ptp_rx_filt_ipv6_da3_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_rx_filt_ipv6_da3_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_ipv6_da3_reg bf;
};

/*[register] PTP_RX_FILT_IPV6_DA4_REG*/
#define PTP_RX_FILT_IPV6_DA4_REG
#define PTP_RX_FILT_IPV6_DA4_REG_ADDRESS 0x860a
#define PTP_RX_FILT_IPV6_DA4_REG_NUM     1
#define PTP_RX_FILT_IPV6_DA4_REG_INC     0x1
#define PTP_RX_FILT_IPV6_DA4_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_IPV6_DA4_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_RX_FILT_IPV6_DA4_REG_IP_ADDR
	#define PTP_RX_FILT_IPV6_DA4_REG_IP_ADDR_OFFSET  0
	#define PTP_RX_FILT_IPV6_DA4_REG_IP_ADDR_LEN     16
	#define PTP_RX_FILT_IPV6_DA4_REG_IP_ADDR_DEFAULT 0x0

struct ptp_rx_filt_ipv6_da4_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_rx_filt_ipv6_da4_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_ipv6_da4_reg bf;
};

/*[register] PTP_RX_FILT_IPV6_DA5_REG*/
#define PTP_RX_FILT_IPV6_DA5_REG
#define PTP_RX_FILT_IPV6_DA5_REG_ADDRESS 0x860b
#define PTP_RX_FILT_IPV6_DA5_REG_NUM     1
#define PTP_RX_FILT_IPV6_DA5_REG_INC     0x1
#define PTP_RX_FILT_IPV6_DA5_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_IPV6_DA5_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_RX_FILT_IPV6_DA5_REG_IP_ADDR
	#define PTP_RX_FILT_IPV6_DA5_REG_IP_ADDR_OFFSET  0
	#define PTP_RX_FILT_IPV6_DA5_REG_IP_ADDR_LEN     16
	#define PTP_RX_FILT_IPV6_DA5_REG_IP_ADDR_DEFAULT 0x0

struct ptp_rx_filt_ipv6_da5_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_rx_filt_ipv6_da5_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_ipv6_da5_reg bf;
};

/*[register] PTP_RX_FILT_IPV6_DA6_REG*/
#define PTP_RX_FILT_IPV6_DA6_REG
#define PTP_RX_FILT_IPV6_DA6_REG_ADDRESS 0x860c
#define PTP_RX_FILT_IPV6_DA6_REG_NUM     1
#define PTP_RX_FILT_IPV6_DA6_REG_INC     0x1
#define PTP_RX_FILT_IPV6_DA6_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_IPV6_DA6_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_RX_FILT_IPV6_DA6_REG_IP_ADDR
	#define PTP_RX_FILT_IPV6_DA6_REG_IP_ADDR_OFFSET  0
	#define PTP_RX_FILT_IPV6_DA6_REG_IP_ADDR_LEN     16
	#define PTP_RX_FILT_IPV6_DA6_REG_IP_ADDR_DEFAULT 0x0

struct ptp_rx_filt_ipv6_da6_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_rx_filt_ipv6_da6_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_ipv6_da6_reg bf;
};

/*[register] PTP_RX_FILT_IPV6_DA7_REG*/
#define PTP_RX_FILT_IPV6_DA7_REG
#define PTP_RX_FILT_IPV6_DA7_REG_ADDRESS 0x860d
#define PTP_RX_FILT_IPV6_DA7_REG_NUM     1
#define PTP_RX_FILT_IPV6_DA7_REG_INC     0x1
#define PTP_RX_FILT_IPV6_DA7_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_IPV6_DA7_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_RX_FILT_IPV6_DA7_REG_IP_ADDR
	#define PTP_RX_FILT_IPV6_DA7_REG_IP_ADDR_OFFSET  0
	#define PTP_RX_FILT_IPV6_DA7_REG_IP_ADDR_LEN     16
	#define PTP_RX_FILT_IPV6_DA7_REG_IP_ADDR_DEFAULT 0x0

struct ptp_rx_filt_ipv6_da7_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_rx_filt_ipv6_da7_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_ipv6_da7_reg bf;
};

/*[register] PTP_RX_FILT_MAC_LENGTHTYPE_REG*/
#define PTP_RX_FILT_MAC_LENGTHTYPE_REG
#define PTP_RX_FILT_MAC_LENGTHTYPE_REG_ADDRESS 0x860e
#define PTP_RX_FILT_MAC_LENGTHTYPE_REG_NUM     1
#define PTP_RX_FILT_MAC_LENGTHTYPE_REG_INC     0x1
#define PTP_RX_FILT_MAC_LENGTHTYPE_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_MAC_LENGTHTYPE_REG_DEFAULT 0x0
	/*[field] LENGTH_TYPE*/
	#define PTP_RX_FILT_MAC_LENGTHTYPE_REG_LENGTH_TYPE
	#define PTP_RX_FILT_MAC_LENGTHTYPE_REG_LENGTH_TYPE_OFFSET  0
	#define PTP_RX_FILT_MAC_LENGTHTYPE_REG_LENGTH_TYPE_LEN     16
	#define PTP_RX_FILT_MAC_LENGTHTYPE_REG_LENGTH_TYPE_DEFAULT 0x0

struct ptp_rx_filt_mac_lengthtype_reg {
	a_uint32_t  length_type:16;
};

union ptp_rx_filt_mac_lengthtype_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_mac_lengthtype_reg bf;
};

/*[register] PTP_RX_FILT_LAYER4_PROTOCOL_REG*/
#define PTP_RX_FILT_LAYER4_PROTOCOL_REG
#define PTP_RX_FILT_LAYER4_PROTOCOL_REG_ADDRESS 0x860f
#define PTP_RX_FILT_LAYER4_PROTOCOL_REG_NUM     1
#define PTP_RX_FILT_LAYER4_PROTOCOL_REG_INC     0x1
#define PTP_RX_FILT_LAYER4_PROTOCOL_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_LAYER4_PROTOCOL_REG_DEFAULT 0x0
	/*[field] L4_PROTOCOL*/
	#define PTP_RX_FILT_LAYER4_PROTOCOL_REG_L4_PROTOCOL
	#define PTP_RX_FILT_LAYER4_PROTOCOL_REG_L4_PROTOCOL_OFFSET  0
	#define PTP_RX_FILT_LAYER4_PROTOCOL_REG_L4_PROTOCOL_LEN     16
	#define PTP_RX_FILT_LAYER4_PROTOCOL_REG_L4_PROTOCOL_DEFAULT 0x0

struct ptp_rx_filt_layer4_protocol_reg {
	a_uint32_t  l4_protocol:16;
};

union ptp_rx_filt_layer4_protocol_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_layer4_protocol_reg bf;
};

/*[register] PTP_RX_FILT_UDP_PORT_REG*/
#define PTP_RX_FILT_UDP_PORT_REG
#define PTP_RX_FILT_UDP_PORT_REG_ADDRESS 0x8610
#define PTP_RX_FILT_UDP_PORT_REG_NUM     1
#define PTP_RX_FILT_UDP_PORT_REG_INC     0x1
#define PTP_RX_FILT_UDP_PORT_REG_TYPE    REG_TYPE_RW
#define PTP_RX_FILT_UDP_PORT_REG_DEFAULT 0x0
	/*[field] UDP_PORT*/
	#define PTP_RX_FILT_UDP_PORT_REG_UDP_PORT
	#define PTP_RX_FILT_UDP_PORT_REG_UDP_PORT_OFFSET  0
	#define PTP_RX_FILT_UDP_PORT_REG_UDP_PORT_LEN     16
	#define PTP_RX_FILT_UDP_PORT_REG_UDP_PORT_DEFAULT 0x0

struct ptp_rx_filt_udp_port_reg {
	a_uint32_t  udp_port:16;
};

union ptp_rx_filt_udp_port_reg_u {
	a_uint32_t val;
	struct ptp_rx_filt_udp_port_reg bf;
};

/*[register] PTP_RX_COM_TS_STATUS_REG*/
#define PTP_RX_COM_TS_STATUS_REG
#define PTP_RX_COM_TS_STATUS_REG_ADDRESS 0x8611
#define PTP_RX_COM_TS_STATUS_REG_NUM     1
#define PTP_RX_COM_TS_STATUS_REG_INC     0x1
#define PTP_RX_COM_TS_STATUS_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_TS_STATUS_REG_DEFAULT 0x0
	/*[field] MAC_LENGTHTYPE*/
	#define PTP_RX_COM_TS_STATUS_REG_MAC_LENGTHTYPE
	#define PTP_RX_COM_TS_STATUS_REG_MAC_LENGTHTYPE_OFFSET  0
	#define PTP_RX_COM_TS_STATUS_REG_MAC_LENGTHTYPE_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_MAC_LENGTHTYPE_DEFAULT 0x0
	/*[field] MAC_DA*/
	#define PTP_RX_COM_TS_STATUS_REG_MAC_DA
	#define PTP_RX_COM_TS_STATUS_REG_MAC_DA_OFFSET  1
	#define PTP_RX_COM_TS_STATUS_REG_MAC_DA_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_MAC_DA_DEFAULT 0x0
	/*[field] MAC_PTP_PRIM_ADDR*/
	#define PTP_RX_COM_TS_STATUS_REG_MAC_PTP_PRIM_ADDR
	#define PTP_RX_COM_TS_STATUS_REG_MAC_PTP_PRIM_ADDR_OFFSET  2
	#define PTP_RX_COM_TS_STATUS_REG_MAC_PTP_PRIM_ADDR_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_MAC_PTP_PRIM_ADDR_DEFAULT 0x0
	/*[field] MAC_PTP_PDELAY_ADDR*/
	#define PTP_RX_COM_TS_STATUS_REG_MAC_PTP_PDELAY_ADDR
	#define PTP_RX_COM_TS_STATUS_REG_MAC_PTP_PDELAY_ADDR_OFFSET  3
	#define PTP_RX_COM_TS_STATUS_REG_MAC_PTP_PDELAY_ADDR_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_MAC_PTP_PDELAY_ADDR_DEFAULT 0x0
	/*[field] IPV4_LAYER4_PROTOCOL*/
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_LAYER4_PROTOCOL
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_LAYER4_PROTOCOL_OFFSET  4
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_LAYER4_PROTOCOL_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_LAYER4_PROTOCOL_DEFAULT 0x0
	/*[field] IPV4_DA*/
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_DA
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_DA_OFFSET  5
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_DA_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_DA_DEFAULT 0x0
	/*[field] IPV4_PTP_PRIM_ADDR*/
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_PTP_PRIM_ADDR
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_PTP_PRIM_ADDR_OFFSET  6
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_PTP_PRIM_ADDR_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_PTP_PRIM_ADDR_DEFAULT 0x0
	/*[field] IPV4_PTP_PDELAY_ADDR*/
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_PTP_PDELAY_ADDR
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_PTP_PDELAY_ADDR_OFFSET  7
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_PTP_PDELAY_ADDR_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_IPV4_PTP_PDELAY_ADDR_DEFAULT 0x0
	/*[field] IPV6_NEXT_HEADER*/
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_NEXT_HEADER
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_NEXT_HEADER_OFFSET  8
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_NEXT_HEADER_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_NEXT_HEADER_DEFAULT 0x0
	/*[field] IPV6_DA*/
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_DA
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_DA_OFFSET  9
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_DA_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_DA_DEFAULT 0x0
	/*[field] IPV6_PTP_PRIM_ADDR*/
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_PTP_PRIM_ADDR
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_PTP_PRIM_ADDR_OFFSET  10
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_PTP_PRIM_ADDR_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_PTP_PRIM_ADDR_DEFAULT 0x0
	/*[field] IPV6_PTP_PDELAY_ADDR*/
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_PTP_PDELAY_ADDR
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_PTP_PDELAY_ADDR_OFFSET  11
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_PTP_PDELAY_ADDR_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_IPV6_PTP_PDELAY_ADDR_DEFAULT 0x0
	/*[field] UDP_DPORT*/
	#define PTP_RX_COM_TS_STATUS_REG_UDP_DPORT
	#define PTP_RX_COM_TS_STATUS_REG_UDP_DPORT_OFFSET  12
	#define PTP_RX_COM_TS_STATUS_REG_UDP_DPORT_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_UDP_DPORT_DEFAULT 0x0
	/*[field] UDP_PTP_EVENT_DPORT*/
	#define PTP_RX_COM_TS_STATUS_REG_UDP_PTP_EVENT_DPORT
	#define PTP_RX_COM_TS_STATUS_REG_UDP_PTP_EVENT_DPORT_OFFSET  13
	#define PTP_RX_COM_TS_STATUS_REG_UDP_PTP_EVENT_DPORT_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_UDP_PTP_EVENT_DPORT_DEFAULT 0x0
	/*[field] Y1731_MACH*/
	#define PTP_RX_COM_TS_STATUS_REG_Y1731_MACH
	#define PTP_RX_COM_TS_STATUS_REG_Y1731_MACH_OFFSET  14
	#define PTP_RX_COM_TS_STATUS_REG_Y1731_MACH_LEN     1
	#define PTP_RX_COM_TS_STATUS_REG_Y1731_MACH_DEFAULT 0x0

struct ptp_rx_com_ts_status_reg {
	a_uint32_t  mac_lengthtype:1;
	a_uint32_t  mac_da:1;
	a_uint32_t  mac_ptp_prim_addr:1;
	a_uint32_t  mac_ptp_pdelay_addr:1;
	a_uint32_t  ipv4_layer4_protocol:1;
	a_uint32_t  ipv4_da:1;
	a_uint32_t  ipv4_ptp_prim_addr:1;
	a_uint32_t  ipv4_ptp_pdelay_addr:1;
	a_uint32_t  ipv6_next_header:1;
	a_uint32_t  ipv6_da:1;
	a_uint32_t  ipv6_ptp_prim_addr:1;
	a_uint32_t  ipv6_ptp_pdelay_addr:1;
	a_uint32_t  udp_dport:1;
	a_uint32_t  udp_ptp_event_dport:1;
	a_uint32_t  y1731_mach:1;
};

union ptp_rx_com_ts_status_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_ts_status_reg bf;
};

/*[register] PTP_RX_COM_TIMESTAMP0_REG*/
#define PTP_RX_COM_TIMESTAMP0_REG
#define PTP_RX_COM_TIMESTAMP0_REG_ADDRESS 0x8612
#define PTP_RX_COM_TIMESTAMP0_REG_NUM     1
#define PTP_RX_COM_TIMESTAMP0_REG_INC     0x1
#define PTP_RX_COM_TIMESTAMP0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_TIMESTAMP0_REG_DEFAULT 0x0
	/*[field] COM_TS*/
	#define PTP_RX_COM_TIMESTAMP0_REG_COM_TS
	#define PTP_RX_COM_TIMESTAMP0_REG_COM_TS_OFFSET  0
	#define PTP_RX_COM_TIMESTAMP0_REG_COM_TS_LEN     16
	#define PTP_RX_COM_TIMESTAMP0_REG_COM_TS_DEFAULT 0x0

struct ptp_rx_com_timestamp0_reg {
	a_uint32_t  com_ts:16;
};

union ptp_rx_com_timestamp0_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_timestamp0_reg bf;
};

/*[register] PTP_RX_COM_TIMESTAMP1_REG*/
#define PTP_RX_COM_TIMESTAMP1_REG
#define PTP_RX_COM_TIMESTAMP1_REG_ADDRESS 0x8613
#define PTP_RX_COM_TIMESTAMP1_REG_NUM     1
#define PTP_RX_COM_TIMESTAMP1_REG_INC     0x1
#define PTP_RX_COM_TIMESTAMP1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_TIMESTAMP1_REG_DEFAULT 0x0
	/*[field] COM_TS*/
	#define PTP_RX_COM_TIMESTAMP1_REG_COM_TS
	#define PTP_RX_COM_TIMESTAMP1_REG_COM_TS_OFFSET  0
	#define PTP_RX_COM_TIMESTAMP1_REG_COM_TS_LEN     16
	#define PTP_RX_COM_TIMESTAMP1_REG_COM_TS_DEFAULT 0x0

struct ptp_rx_com_timestamp1_reg {
	a_uint32_t  com_ts:16;
};

union ptp_rx_com_timestamp1_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_timestamp1_reg bf;
};

/*[register] PTP_RX_COM_TIMESTAMP2_REG*/
#define PTP_RX_COM_TIMESTAMP2_REG
#define PTP_RX_COM_TIMESTAMP2_REG_ADDRESS 0x8614
#define PTP_RX_COM_TIMESTAMP2_REG_NUM     1
#define PTP_RX_COM_TIMESTAMP2_REG_INC     0x1
#define PTP_RX_COM_TIMESTAMP2_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_TIMESTAMP2_REG_DEFAULT 0x0
	/*[field] COM_TS*/
	#define PTP_RX_COM_TIMESTAMP2_REG_COM_TS
	#define PTP_RX_COM_TIMESTAMP2_REG_COM_TS_OFFSET  0
	#define PTP_RX_COM_TIMESTAMP2_REG_COM_TS_LEN     16
	#define PTP_RX_COM_TIMESTAMP2_REG_COM_TS_DEFAULT 0x0

struct ptp_rx_com_timestamp2_reg {
	a_uint32_t  com_ts:16;
};

union ptp_rx_com_timestamp2_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_timestamp2_reg bf;
};

/*[register] PTP_RX_COM_TIMESTAMP3_REG*/
#define PTP_RX_COM_TIMESTAMP3_REG
#define PTP_RX_COM_TIMESTAMP3_REG_ADDRESS 0x8615
#define PTP_RX_COM_TIMESTAMP3_REG_NUM     1
#define PTP_RX_COM_TIMESTAMP3_REG_INC     0x1
#define PTP_RX_COM_TIMESTAMP3_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_TIMESTAMP3_REG_DEFAULT 0x0
	/*[field] COM_TS*/
	#define PTP_RX_COM_TIMESTAMP3_REG_COM_TS
	#define PTP_RX_COM_TIMESTAMP3_REG_COM_TS_OFFSET  0
	#define PTP_RX_COM_TIMESTAMP3_REG_COM_TS_LEN     16
	#define PTP_RX_COM_TIMESTAMP3_REG_COM_TS_DEFAULT 0x0

struct ptp_rx_com_timestamp3_reg {
	a_uint32_t  com_ts:16;
};

union ptp_rx_com_timestamp3_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_timestamp3_reg bf;
};

/*[register] PTP_RX_COM_TIMESTAMP4_REG*/
#define PTP_RX_COM_TIMESTAMP4_REG
#define PTP_RX_COM_TIMESTAMP4_REG_ADDRESS 0x8616
#define PTP_RX_COM_TIMESTAMP4_REG_NUM     1
#define PTP_RX_COM_TIMESTAMP4_REG_INC     0x1
#define PTP_RX_COM_TIMESTAMP4_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_TIMESTAMP4_REG_DEFAULT 0x0
	/*[field] COM_TS*/
	#define PTP_RX_COM_TIMESTAMP4_REG_COM_TS
	#define PTP_RX_COM_TIMESTAMP4_REG_COM_TS_OFFSET  0
	#define PTP_RX_COM_TIMESTAMP4_REG_COM_TS_LEN     16
	#define PTP_RX_COM_TIMESTAMP4_REG_COM_TS_DEFAULT 0x0

struct ptp_rx_com_timestamp4_reg {
	a_uint32_t  com_ts:16;
};

union ptp_rx_com_timestamp4_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_timestamp4_reg bf;
};

/*[register] PTP_RX_COM_FRAC_NANO_REG*/
#define PTP_RX_COM_FRAC_NANO_REG
#define PTP_RX_COM_FRAC_NANO_REG_ADDRESS 0x8617
#define PTP_RX_COM_FRAC_NANO_REG_NUM     1
#define PTP_RX_COM_FRAC_NANO_REG_INC     0x1
#define PTP_RX_COM_FRAC_NANO_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_FRAC_NANO_REG_DEFAULT 0x0
	/*[field] FRAC_NANO*/
	#define PTP_RX_COM_FRAC_NANO_REG_FRAC_NANO
	#define PTP_RX_COM_FRAC_NANO_REG_FRAC_NANO_OFFSET  0
	#define PTP_RX_COM_FRAC_NANO_REG_FRAC_NANO_LEN     16
	#define PTP_RX_COM_FRAC_NANO_REG_FRAC_NANO_DEFAULT 0x0

struct ptp_rx_com_frac_nano_reg {
	a_uint32_t  frac_nano:16;
};

union ptp_rx_com_frac_nano_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_frac_nano_reg bf;
};

/*[register] PTP_RX_COM_TIMESTAMP_PRE0_REG*/
#define PTP_RX_COM_TIMESTAMP_PRE0_REG
#define PTP_RX_COM_TIMESTAMP_PRE0_REG_ADDRESS 0x8618
#define PTP_RX_COM_TIMESTAMP_PRE0_REG_NUM     1
#define PTP_RX_COM_TIMESTAMP_PRE0_REG_INC     0x1
#define PTP_RX_COM_TIMESTAMP_PRE0_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_TIMESTAMP_PRE0_REG_DEFAULT 0x0
	/*[field] COM_TS_PRE*/
	#define PTP_RX_COM_TIMESTAMP_PRE0_REG_COM_TS_PRE
	#define PTP_RX_COM_TIMESTAMP_PRE0_REG_COM_TS_PRE_OFFSET  0
	#define PTP_RX_COM_TIMESTAMP_PRE0_REG_COM_TS_PRE_LEN     16
	#define PTP_RX_COM_TIMESTAMP_PRE0_REG_COM_TS_PRE_DEFAULT 0x0

struct ptp_rx_com_timestamp_pre0_reg {
	a_uint32_t  com_ts_pre:16;
};

union ptp_rx_com_timestamp_pre0_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_timestamp_pre0_reg bf;
};

/*[register] PTP_RX_COM_TIMESTAMP_PRE1_REG*/
#define PTP_RX_COM_TIMESTAMP_PRE1_REG
#define PTP_RX_COM_TIMESTAMP_PRE1_REG_ADDRESS 0x8619
#define PTP_RX_COM_TIMESTAMP_PRE1_REG_NUM     1
#define PTP_RX_COM_TIMESTAMP_PRE1_REG_INC     0x1
#define PTP_RX_COM_TIMESTAMP_PRE1_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_TIMESTAMP_PRE1_REG_DEFAULT 0x0
	/*[field] COM_TS_PRE*/
	#define PTP_RX_COM_TIMESTAMP_PRE1_REG_COM_TS_PRE
	#define PTP_RX_COM_TIMESTAMP_PRE1_REG_COM_TS_PRE_OFFSET  0
	#define PTP_RX_COM_TIMESTAMP_PRE1_REG_COM_TS_PRE_LEN     16
	#define PTP_RX_COM_TIMESTAMP_PRE1_REG_COM_TS_PRE_DEFAULT 0x0

struct ptp_rx_com_timestamp_pre1_reg {
	a_uint32_t  com_ts_pre:16;
};

union ptp_rx_com_timestamp_pre1_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_timestamp_pre1_reg bf;
};

/*[register] PTP_RX_COM_TIMESTAMP_PRE2_REG*/
#define PTP_RX_COM_TIMESTAMP_PRE2_REG
#define PTP_RX_COM_TIMESTAMP_PRE2_REG_ADDRESS 0x861a
#define PTP_RX_COM_TIMESTAMP_PRE2_REG_NUM     1
#define PTP_RX_COM_TIMESTAMP_PRE2_REG_INC     0x1
#define PTP_RX_COM_TIMESTAMP_PRE2_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_TIMESTAMP_PRE2_REG_DEFAULT 0x0
	/*[field] COM_TS_PRE*/
	#define PTP_RX_COM_TIMESTAMP_PRE2_REG_COM_TS_PRE
	#define PTP_RX_COM_TIMESTAMP_PRE2_REG_COM_TS_PRE_OFFSET  0
	#define PTP_RX_COM_TIMESTAMP_PRE2_REG_COM_TS_PRE_LEN     16
	#define PTP_RX_COM_TIMESTAMP_PRE2_REG_COM_TS_PRE_DEFAULT 0x0

struct ptp_rx_com_timestamp_pre2_reg {
	a_uint32_t  com_ts_pre:16;
};

union ptp_rx_com_timestamp_pre2_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_timestamp_pre2_reg bf;
};

/*[register] PTP_RX_COM_TIMESTAMP_PRE3_REG*/
#define PTP_RX_COM_TIMESTAMP_PRE3_REG
#define PTP_RX_COM_TIMESTAMP_PRE3_REG_ADDRESS 0x861b
#define PTP_RX_COM_TIMESTAMP_PRE3_REG_NUM     1
#define PTP_RX_COM_TIMESTAMP_PRE3_REG_INC     0x1
#define PTP_RX_COM_TIMESTAMP_PRE3_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_TIMESTAMP_PRE3_REG_DEFAULT 0x0
	/*[field] COM_TS_PRE*/
	#define PTP_RX_COM_TIMESTAMP_PRE3_REG_COM_TS_PRE
	#define PTP_RX_COM_TIMESTAMP_PRE3_REG_COM_TS_PRE_OFFSET  0
	#define PTP_RX_COM_TIMESTAMP_PRE3_REG_COM_TS_PRE_LEN     16
	#define PTP_RX_COM_TIMESTAMP_PRE3_REG_COM_TS_PRE_DEFAULT 0x0

struct ptp_rx_com_timestamp_pre3_reg {
	a_uint32_t  com_ts_pre:16;
};

union ptp_rx_com_timestamp_pre3_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_timestamp_pre3_reg bf;
};

/*[register] PTP_RX_COM_TIMESTAMP_PRE4_REG*/
#define PTP_RX_COM_TIMESTAMP_PRE4_REG
#define PTP_RX_COM_TIMESTAMP_PRE4_REG_ADDRESS 0x861c
#define PTP_RX_COM_TIMESTAMP_PRE4_REG_NUM     1
#define PTP_RX_COM_TIMESTAMP_PRE4_REG_INC     0x1
#define PTP_RX_COM_TIMESTAMP_PRE4_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_TIMESTAMP_PRE4_REG_DEFAULT 0x0
	/*[field] COM_TS_PRE*/
	#define PTP_RX_COM_TIMESTAMP_PRE4_REG_COM_TS_PRE
	#define PTP_RX_COM_TIMESTAMP_PRE4_REG_COM_TS_PRE_OFFSET  0
	#define PTP_RX_COM_TIMESTAMP_PRE4_REG_COM_TS_PRE_LEN     16
	#define PTP_RX_COM_TIMESTAMP_PRE4_REG_COM_TS_PRE_DEFAULT 0x0

struct ptp_rx_com_timestamp_pre4_reg {
	a_uint32_t  com_ts_pre:16;
};

union ptp_rx_com_timestamp_pre4_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_timestamp_pre4_reg bf;
};

/*[register] PTP_RX_COM_FRAC_NANO_PRE_REG*/
#define PTP_RX_COM_FRAC_NANO_PRE_REG
#define PTP_RX_COM_FRAC_NANO_PRE_REG_ADDRESS 0x861d
#define PTP_RX_COM_FRAC_NANO_PRE_REG_NUM     1
#define PTP_RX_COM_FRAC_NANO_PRE_REG_INC     0x1
#define PTP_RX_COM_FRAC_NANO_PRE_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_FRAC_NANO_PRE_REG_DEFAULT 0x0
	/*[field] FRAC_NANO_PRE*/
	#define PTP_RX_COM_FRAC_NANO_PRE_REG_FRAC_NANO_PRE
	#define PTP_RX_COM_FRAC_NANO_PRE_REG_FRAC_NANO_PRE_OFFSET  0
	#define PTP_RX_COM_FRAC_NANO_PRE_REG_FRAC_NANO_PRE_LEN     16
	#define PTP_RX_COM_FRAC_NANO_PRE_REG_FRAC_NANO_PRE_DEFAULT 0x0

struct ptp_rx_com_frac_nano_pre_reg {
	a_uint32_t  frac_nano_pre:16;
};

union ptp_rx_com_frac_nano_pre_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_frac_nano_pre_reg bf;
};

/*[register] PTP_RX_Y1731_IDENTIFY_REG*/
#define PTP_RX_Y1731_IDENTIFY_REG
#define PTP_RX_Y1731_IDENTIFY_REG_ADDRESS 0x861e
#define PTP_RX_Y1731_IDENTIFY_REG_NUM     1
#define PTP_RX_Y1731_IDENTIFY_REG_INC     0x1
#define PTP_RX_Y1731_IDENTIFY_REG_TYPE    REG_TYPE_RW
#define PTP_RX_Y1731_IDENTIFY_REG_DEFAULT 0x0
	/*[field] IDENTIFY*/
	#define PTP_RX_Y1731_IDENTIFY_REG_IDENTIFY
	#define PTP_RX_Y1731_IDENTIFY_REG_IDENTIFY_OFFSET  0
	#define PTP_RX_Y1731_IDENTIFY_REG_IDENTIFY_LEN     16
	#define PTP_RX_Y1731_IDENTIFY_REG_IDENTIFY_DEFAULT 0x0

struct ptp_rx_y1731_identify_reg {
	a_uint32_t  identify:16;
};

union ptp_rx_y1731_identify_reg_u {
	a_uint32_t val;
	struct ptp_rx_y1731_identify_reg bf;
};

/*[register] PTP_RX_Y1731_IDENTIFY_PRE_REG*/
#define PTP_RX_Y1731_IDENTIFY_PRE_REG
#define PTP_RX_Y1731_IDENTIFY_PRE_REG_ADDRESS 0x861f
#define PTP_RX_Y1731_IDENTIFY_PRE_REG_NUM     1
#define PTP_RX_Y1731_IDENTIFY_PRE_REG_INC     0x1
#define PTP_RX_Y1731_IDENTIFY_PRE_REG_TYPE    REG_TYPE_RW
#define PTP_RX_Y1731_IDENTIFY_PRE_REG_DEFAULT 0x0
	/*[field] IDENTIFY_PRE*/
	#define PTP_RX_Y1731_IDENTIFY_PRE_REG_IDENTIFY_PRE
	#define PTP_RX_Y1731_IDENTIFY_PRE_REG_IDENTIFY_PRE_OFFSET  0
	#define PTP_RX_Y1731_IDENTIFY_PRE_REG_IDENTIFY_PRE_LEN     16
	#define PTP_RX_Y1731_IDENTIFY_PRE_REG_IDENTIFY_PRE_DEFAULT 0x0

struct ptp_rx_y1731_identify_pre_reg {
	a_uint32_t  identify_pre:16;
};

union ptp_rx_y1731_identify_pre_reg_u {
	a_uint32_t val;
	struct ptp_rx_y1731_identify_pre_reg bf;
};

/*[register] PTP_TX_COM_TS_CTRL_REG*/
#define PTP_TX_COM_TS_CTRL_REG
#define PTP_TX_COM_TS_CTRL_REG_ADDRESS 0x8620
#define PTP_TX_COM_TS_CTRL_REG_NUM     1
#define PTP_TX_COM_TS_CTRL_REG_INC     0x1
#define PTP_TX_COM_TS_CTRL_REG_TYPE    REG_TYPE_RW
#define PTP_TX_COM_TS_CTRL_REG_DEFAULT 0x0
	/*[field] FILT_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_FILT_EN
	#define PTP_TX_COM_TS_CTRL_REG_FILT_EN_OFFSET  0
	#define PTP_TX_COM_TS_CTRL_REG_FILT_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_FILT_EN_DEFAULT 0x0
	/*[field] MAC_LENGTHTYPE_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_MAC_LENGTHTYPE_EN
	#define PTP_TX_COM_TS_CTRL_REG_MAC_LENGTHTYPE_EN_OFFSET  1
	#define PTP_TX_COM_TS_CTRL_REG_MAC_LENGTHTYPE_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_MAC_LENGTHTYPE_EN_DEFAULT 0x0
	/*[field] MAC_DA_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_MAC_DA_EN
	#define PTP_TX_COM_TS_CTRL_REG_MAC_DA_EN_OFFSET  2
	#define PTP_TX_COM_TS_CTRL_REG_MAC_DA_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_MAC_DA_EN_DEFAULT 0x0
	/*[field] MAC_PTP_FILT_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_MAC_PTP_FILT_EN
	#define PTP_TX_COM_TS_CTRL_REG_MAC_PTP_FILT_EN_OFFSET  3
	#define PTP_TX_COM_TS_CTRL_REG_MAC_PTP_FILT_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_MAC_PTP_FILT_EN_DEFAULT 0x0
	/*[field] IPV4_LAYER4_PROTOCOL_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_IPV4_LAYER4_PROTOCOL_EN
	#define PTP_TX_COM_TS_CTRL_REG_IPV4_LAYER4_PROTOCOL_EN_OFFSET  4
	#define PTP_TX_COM_TS_CTRL_REG_IPV4_LAYER4_PROTOCOL_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_IPV4_LAYER4_PROTOCOL_EN_DEFAULT 0x0
	/*[field] IPV4_DA_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_IPV4_DA_EN
	#define PTP_TX_COM_TS_CTRL_REG_IPV4_DA_EN_OFFSET  5
	#define PTP_TX_COM_TS_CTRL_REG_IPV4_DA_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_IPV4_DA_EN_DEFAULT 0x0
	/*[field] IPV4_PTP_FILT_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_IPV4_PTP_FILT_EN
	#define PTP_TX_COM_TS_CTRL_REG_IPV4_PTP_FILT_EN_OFFSET  6
	#define PTP_TX_COM_TS_CTRL_REG_IPV4_PTP_FILT_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_IPV4_PTP_FILT_EN_DEFAULT 0x0
	/*[field] IPV6_NEXT_HEADER_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_IPV6_NEXT_HEADER_EN
	#define PTP_TX_COM_TS_CTRL_REG_IPV6_NEXT_HEADER_EN_OFFSET  7
	#define PTP_TX_COM_TS_CTRL_REG_IPV6_NEXT_HEADER_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_IPV6_NEXT_HEADER_EN_DEFAULT 0x0
	/*[field] IPV6_DA_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_IPV6_DA_EN
	#define PTP_TX_COM_TS_CTRL_REG_IPV6_DA_EN_OFFSET  8
	#define PTP_TX_COM_TS_CTRL_REG_IPV6_DA_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_IPV6_DA_EN_DEFAULT 0x0
	/*[field] IPV6_PTP_FILT_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_IPV6_PTP_FILT_EN
	#define PTP_TX_COM_TS_CTRL_REG_IPV6_PTP_FILT_EN_OFFSET  9
	#define PTP_TX_COM_TS_CTRL_REG_IPV6_PTP_FILT_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_IPV6_PTP_FILT_EN_DEFAULT 0x0
	/*[field] UDP_DPORT_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_UDP_DPORT_EN
	#define PTP_TX_COM_TS_CTRL_REG_UDP_DPORT_EN_OFFSET  10
	#define PTP_TX_COM_TS_CTRL_REG_UDP_DPORT_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_UDP_DPORT_EN_DEFAULT 0x0
	/*[field] UDP_PTP_EVENT_FILT_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_UDP_PTP_EVENT_FILT_EN
	#define PTP_TX_COM_TS_CTRL_REG_UDP_PTP_EVENT_FILT_EN_OFFSET  11
	#define PTP_TX_COM_TS_CTRL_REG_UDP_PTP_EVENT_FILT_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_UDP_PTP_EVENT_FILT_EN_DEFAULT 0x0
	/*[field] Y1731_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_Y1731_EN
	#define PTP_TX_COM_TS_CTRL_REG_Y1731_EN_OFFSET  12
	#define PTP_TX_COM_TS_CTRL_REG_Y1731_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_Y1731_EN_DEFAULT 0x0
	/*[field] Y1731_INSERT_TS_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_Y1731_INSERT_TS_EN
	#define PTP_TX_COM_TS_CTRL_REG_Y1731_INSERT_TS_EN_OFFSET  13
	#define PTP_TX_COM_TS_CTRL_REG_Y1731_INSERT_TS_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_Y1731_INSERT_TS_EN_DEFAULT 0x0
	/*[field] Y1731_SA_CHK_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_Y1731_SA_CHK_EN
	#define PTP_TX_COM_TS_CTRL_REG_Y1731_SA_CHK_EN_OFFSET  14
	#define PTP_TX_COM_TS_CTRL_REG_Y1731_SA_CHK_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_Y1731_SA_CHK_EN_DEFAULT 0x0
	/*[field] PW_MAC_EN*/
	#define PTP_TX_COM_TS_CTRL_REG_PW_MAC_EN
	#define PTP_TX_COM_TS_CTRL_REG_PW_MAC_EN_OFFSET  15
	#define PTP_TX_COM_TS_CTRL_REG_PW_MAC_EN_LEN     1
	#define PTP_TX_COM_TS_CTRL_REG_PW_MAC_EN_DEFAULT 0x0

struct ptp_tx_com_ts_ctrl_reg {
	a_uint32_t  filt_en:1;
	a_uint32_t  mac_lengthtype_en:1;
	a_uint32_t  mac_da_en:1;
	a_uint32_t  mac_ptp_filt_en:1;
	a_uint32_t  ipv4_layer4_protocol_en:1;
	a_uint32_t  ipv4_da_en:1;
	a_uint32_t  ipv4_ptp_filt_en:1;
	a_uint32_t  ipv6_next_header_en:1;
	a_uint32_t  ipv6_da_en:1;
	a_uint32_t  ipv6_ptp_filt_en:1;
	a_uint32_t  udp_dport_en:1;
	a_uint32_t  udp_ptp_event_filt_en:1;
	a_uint32_t  y1731_en:1;
	a_uint32_t  y1731_insert_ts_en:1;
	a_uint32_t  y1731_sa_chk_en:1;
	a_uint32_t  pw_mac_en:1;
};

union ptp_tx_com_ts_ctrl_reg_u {
	a_uint32_t val;
	struct ptp_tx_com_ts_ctrl_reg bf;
};

/*[register] PTP_TX_FILT_MAC_DA0_REG*/
#define PTP_TX_FILT_MAC_DA0_REG
#define PTP_TX_FILT_MAC_DA0_REG_ADDRESS 0x8621
#define PTP_TX_FILT_MAC_DA0_REG_NUM     1
#define PTP_TX_FILT_MAC_DA0_REG_INC     0x1
#define PTP_TX_FILT_MAC_DA0_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_MAC_DA0_REG_DEFAULT 0x0
	/*[field] MAC_ADDR*/
	#define PTP_TX_FILT_MAC_DA0_REG_MAC_ADDR
	#define PTP_TX_FILT_MAC_DA0_REG_MAC_ADDR_OFFSET  0
	#define PTP_TX_FILT_MAC_DA0_REG_MAC_ADDR_LEN     16
	#define PTP_TX_FILT_MAC_DA0_REG_MAC_ADDR_DEFAULT 0x0

struct ptp_tx_filt_mac_da0_reg {
	a_uint32_t  mac_addr:16;
};

union ptp_tx_filt_mac_da0_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_mac_da0_reg bf;
};

/*[register] PTP_TX_FILT_MAC_DA1_REG*/
#define PTP_TX_FILT_MAC_DA1_REG
#define PTP_TX_FILT_MAC_DA1_REG_ADDRESS 0x8622
#define PTP_TX_FILT_MAC_DA1_REG_NUM     1
#define PTP_TX_FILT_MAC_DA1_REG_INC     0x1
#define PTP_TX_FILT_MAC_DA1_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_MAC_DA1_REG_DEFAULT 0x0
	/*[field] MAC_ADDR*/
	#define PTP_TX_FILT_MAC_DA1_REG_MAC_ADDR
	#define PTP_TX_FILT_MAC_DA1_REG_MAC_ADDR_OFFSET  0
	#define PTP_TX_FILT_MAC_DA1_REG_MAC_ADDR_LEN     16
	#define PTP_TX_FILT_MAC_DA1_REG_MAC_ADDR_DEFAULT 0x0

struct ptp_tx_filt_mac_da1_reg {
	a_uint32_t  mac_addr:16;
};

union ptp_tx_filt_mac_da1_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_mac_da1_reg bf;
};

/*[register] PTP_TX_FILT_MAC_DA2_REG*/
#define PTP_TX_FILT_MAC_DA2_REG
#define PTP_TX_FILT_MAC_DA2_REG_ADDRESS 0x8623
#define PTP_TX_FILT_MAC_DA2_REG_NUM     1
#define PTP_TX_FILT_MAC_DA2_REG_INC     0x1
#define PTP_TX_FILT_MAC_DA2_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_MAC_DA2_REG_DEFAULT 0x0
	/*[field] MAC_ADDR*/
	#define PTP_TX_FILT_MAC_DA2_REG_MAC_ADDR
	#define PTP_TX_FILT_MAC_DA2_REG_MAC_ADDR_OFFSET  0
	#define PTP_TX_FILT_MAC_DA2_REG_MAC_ADDR_LEN     16
	#define PTP_TX_FILT_MAC_DA2_REG_MAC_ADDR_DEFAULT 0x0

struct ptp_tx_filt_mac_da2_reg {
	a_uint32_t  mac_addr:16;
};

union ptp_tx_filt_mac_da2_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_mac_da2_reg bf;
};

/*[register] PTP_TX_FILT_IPV4_DA0_REG*/
#define PTP_TX_FILT_IPV4_DA0_REG
#define PTP_TX_FILT_IPV4_DA0_REG_ADDRESS 0x8624
#define PTP_TX_FILT_IPV4_DA0_REG_NUM     1
#define PTP_TX_FILT_IPV4_DA0_REG_INC     0x1
#define PTP_TX_FILT_IPV4_DA0_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_IPV4_DA0_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_TX_FILT_IPV4_DA0_REG_IP_ADDR
	#define PTP_TX_FILT_IPV4_DA0_REG_IP_ADDR_OFFSET  0
	#define PTP_TX_FILT_IPV4_DA0_REG_IP_ADDR_LEN     16
	#define PTP_TX_FILT_IPV4_DA0_REG_IP_ADDR_DEFAULT 0x0

struct ptp_tx_filt_ipv4_da0_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_tx_filt_ipv4_da0_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_ipv4_da0_reg bf;
};

/*[register] PTP_TX_FILT_IPV4_DA1_REG*/
#define PTP_TX_FILT_IPV4_DA1_REG
#define PTP_TX_FILT_IPV4_DA1_REG_ADDRESS 0x8625
#define PTP_TX_FILT_IPV4_DA1_REG_NUM     1
#define PTP_TX_FILT_IPV4_DA1_REG_INC     0x1
#define PTP_TX_FILT_IPV4_DA1_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_IPV4_DA1_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_TX_FILT_IPV4_DA1_REG_IP_ADDR
	#define PTP_TX_FILT_IPV4_DA1_REG_IP_ADDR_OFFSET  0
	#define PTP_TX_FILT_IPV4_DA1_REG_IP_ADDR_LEN     16
	#define PTP_TX_FILT_IPV4_DA1_REG_IP_ADDR_DEFAULT 0x0

struct ptp_tx_filt_ipv4_da1_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_tx_filt_ipv4_da1_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_ipv4_da1_reg bf;
};

/*[register] PTP_TX_FILT_IPV6_DA0_REG*/
#define PTP_TX_FILT_IPV6_DA0_REG
#define PTP_TX_FILT_IPV6_DA0_REG_ADDRESS 0x8626
#define PTP_TX_FILT_IPV6_DA0_REG_NUM     1
#define PTP_TX_FILT_IPV6_DA0_REG_INC     0x1
#define PTP_TX_FILT_IPV6_DA0_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_IPV6_DA0_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_TX_FILT_IPV6_DA0_REG_IP_ADDR
	#define PTP_TX_FILT_IPV6_DA0_REG_IP_ADDR_OFFSET  0
	#define PTP_TX_FILT_IPV6_DA0_REG_IP_ADDR_LEN     16
	#define PTP_TX_FILT_IPV6_DA0_REG_IP_ADDR_DEFAULT 0x0

struct ptp_tx_filt_ipv6_da0_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_tx_filt_ipv6_da0_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_ipv6_da0_reg bf;
};

/*[register] PTP_TX_FILT_IPV6_DA1_REG*/
#define PTP_TX_FILT_IPV6_DA1_REG
#define PTP_TX_FILT_IPV6_DA1_REG_ADDRESS 0x8627
#define PTP_TX_FILT_IPV6_DA1_REG_NUM     1
#define PTP_TX_FILT_IPV6_DA1_REG_INC     0x1
#define PTP_TX_FILT_IPV6_DA1_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_IPV6_DA1_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_TX_FILT_IPV6_DA1_REG_IP_ADDR
	#define PTP_TX_FILT_IPV6_DA1_REG_IP_ADDR_OFFSET  0
	#define PTP_TX_FILT_IPV6_DA1_REG_IP_ADDR_LEN     16
	#define PTP_TX_FILT_IPV6_DA1_REG_IP_ADDR_DEFAULT 0x0

struct ptp_tx_filt_ipv6_da1_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_tx_filt_ipv6_da1_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_ipv6_da1_reg bf;
};

/*[register] PTP_TX_FILT_IPV6_DA2_REG*/
#define PTP_TX_FILT_IPV6_DA2_REG
#define PTP_TX_FILT_IPV6_DA2_REG_ADDRESS 0x8628
#define PTP_TX_FILT_IPV6_DA2_REG_NUM     1
#define PTP_TX_FILT_IPV6_DA2_REG_INC     0x1
#define PTP_TX_FILT_IPV6_DA2_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_IPV6_DA2_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_TX_FILT_IPV6_DA2_REG_IP_ADDR
	#define PTP_TX_FILT_IPV6_DA2_REG_IP_ADDR_OFFSET  0
	#define PTP_TX_FILT_IPV6_DA2_REG_IP_ADDR_LEN     16
	#define PTP_TX_FILT_IPV6_DA2_REG_IP_ADDR_DEFAULT 0x0

struct ptp_tx_filt_ipv6_da2_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_tx_filt_ipv6_da2_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_ipv6_da2_reg bf;
};

/*[register] PTP_TX_FILT_IPV6_DA3_REG*/
#define PTP_TX_FILT_IPV6_DA3_REG
#define PTP_TX_FILT_IPV6_DA3_REG_ADDRESS 0x8629
#define PTP_TX_FILT_IPV6_DA3_REG_NUM     1
#define PTP_TX_FILT_IPV6_DA3_REG_INC     0x1
#define PTP_TX_FILT_IPV6_DA3_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_IPV6_DA3_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_TX_FILT_IPV6_DA3_REG_IP_ADDR
	#define PTP_TX_FILT_IPV6_DA3_REG_IP_ADDR_OFFSET  0
	#define PTP_TX_FILT_IPV6_DA3_REG_IP_ADDR_LEN     16
	#define PTP_TX_FILT_IPV6_DA3_REG_IP_ADDR_DEFAULT 0x0

struct ptp_tx_filt_ipv6_da3_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_tx_filt_ipv6_da3_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_ipv6_da3_reg bf;
};

/*[register] PTP_TX_FILT_IPV6_DA4_REG*/
#define PTP_TX_FILT_IPV6_DA4_REG
#define PTP_TX_FILT_IPV6_DA4_REG_ADDRESS 0x862a
#define PTP_TX_FILT_IPV6_DA4_REG_NUM     1
#define PTP_TX_FILT_IPV6_DA4_REG_INC     0x1
#define PTP_TX_FILT_IPV6_DA4_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_IPV6_DA4_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_TX_FILT_IPV6_DA4_REG_IP_ADDR
	#define PTP_TX_FILT_IPV6_DA4_REG_IP_ADDR_OFFSET  0
	#define PTP_TX_FILT_IPV6_DA4_REG_IP_ADDR_LEN     16
	#define PTP_TX_FILT_IPV6_DA4_REG_IP_ADDR_DEFAULT 0x0

struct ptp_tx_filt_ipv6_da4_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_tx_filt_ipv6_da4_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_ipv6_da4_reg bf;
};

/*[register] PTP_TX_FILT_IPV6_DA5_REG*/
#define PTP_TX_FILT_IPV6_DA5_REG
#define PTP_TX_FILT_IPV6_DA5_REG_ADDRESS 0x862b
#define PTP_TX_FILT_IPV6_DA5_REG_NUM     1
#define PTP_TX_FILT_IPV6_DA5_REG_INC     0x1
#define PTP_TX_FILT_IPV6_DA5_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_IPV6_DA5_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_TX_FILT_IPV6_DA5_REG_IP_ADDR
	#define PTP_TX_FILT_IPV6_DA5_REG_IP_ADDR_OFFSET  0
	#define PTP_TX_FILT_IPV6_DA5_REG_IP_ADDR_LEN     16
	#define PTP_TX_FILT_IPV6_DA5_REG_IP_ADDR_DEFAULT 0x0

struct ptp_tx_filt_ipv6_da5_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_tx_filt_ipv6_da5_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_ipv6_da5_reg bf;
};

/*[register] PTP_TX_FILT_IPV6_DA6_REG*/
#define PTP_TX_FILT_IPV6_DA6_REG
#define PTP_TX_FILT_IPV6_DA6_REG_ADDRESS 0x862c
#define PTP_TX_FILT_IPV6_DA6_REG_NUM     1
#define PTP_TX_FILT_IPV6_DA6_REG_INC     0x1
#define PTP_TX_FILT_IPV6_DA6_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_IPV6_DA6_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_TX_FILT_IPV6_DA6_REG_IP_ADDR
	#define PTP_TX_FILT_IPV6_DA6_REG_IP_ADDR_OFFSET  0
	#define PTP_TX_FILT_IPV6_DA6_REG_IP_ADDR_LEN     16
	#define PTP_TX_FILT_IPV6_DA6_REG_IP_ADDR_DEFAULT 0x0

struct ptp_tx_filt_ipv6_da6_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_tx_filt_ipv6_da6_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_ipv6_da6_reg bf;
};

/*[register] PTP_TX_FILT_IPV6_DA7_REG*/
#define PTP_TX_FILT_IPV6_DA7_REG
#define PTP_TX_FILT_IPV6_DA7_REG_ADDRESS 0x862d
#define PTP_TX_FILT_IPV6_DA7_REG_NUM     1
#define PTP_TX_FILT_IPV6_DA7_REG_INC     0x1
#define PTP_TX_FILT_IPV6_DA7_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_IPV6_DA7_REG_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define PTP_TX_FILT_IPV6_DA7_REG_IP_ADDR
	#define PTP_TX_FILT_IPV6_DA7_REG_IP_ADDR_OFFSET  0
	#define PTP_TX_FILT_IPV6_DA7_REG_IP_ADDR_LEN     16
	#define PTP_TX_FILT_IPV6_DA7_REG_IP_ADDR_DEFAULT 0x0

struct ptp_tx_filt_ipv6_da7_reg {
	a_uint32_t  ip_addr:16;
};

union ptp_tx_filt_ipv6_da7_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_ipv6_da7_reg bf;
};

/*[register] PTP_TX_FILT_MAC_LENGTHTYPE_REG*/
#define PTP_TX_FILT_MAC_LENGTHTYPE_REG
#define PTP_TX_FILT_MAC_LENGTHTYPE_REG_ADDRESS 0x862e
#define PTP_TX_FILT_MAC_LENGTHTYPE_REG_NUM     1
#define PTP_TX_FILT_MAC_LENGTHTYPE_REG_INC     0x1
#define PTP_TX_FILT_MAC_LENGTHTYPE_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_MAC_LENGTHTYPE_REG_DEFAULT 0x0
	/*[field] LENGTH_TYPE*/
	#define PTP_TX_FILT_MAC_LENGTHTYPE_REG_LENGTH_TYPE
	#define PTP_TX_FILT_MAC_LENGTHTYPE_REG_LENGTH_TYPE_OFFSET  0
	#define PTP_TX_FILT_MAC_LENGTHTYPE_REG_LENGTH_TYPE_LEN     16
	#define PTP_TX_FILT_MAC_LENGTHTYPE_REG_LENGTH_TYPE_DEFAULT 0x0

struct ptp_tx_filt_mac_lengthtype_reg {
	a_uint32_t  length_type:16;
};

union ptp_tx_filt_mac_lengthtype_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_mac_lengthtype_reg bf;
};

/*[register] PTP_TX_FILT_LAYER4_PROTOCOL_REG*/
#define PTP_TX_FILT_LAYER4_PROTOCOL_REG
#define PTP_TX_FILT_LAYER4_PROTOCOL_REG_ADDRESS 0x862f
#define PTP_TX_FILT_LAYER4_PROTOCOL_REG_NUM     1
#define PTP_TX_FILT_LAYER4_PROTOCOL_REG_INC     0x1
#define PTP_TX_FILT_LAYER4_PROTOCOL_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_LAYER4_PROTOCOL_REG_DEFAULT 0x0
	/*[field] L4_PROTOCOL*/
	#define PTP_TX_FILT_LAYER4_PROTOCOL_REG_L4_PROTOCOL
	#define PTP_TX_FILT_LAYER4_PROTOCOL_REG_L4_PROTOCOL_OFFSET  0
	#define PTP_TX_FILT_LAYER4_PROTOCOL_REG_L4_PROTOCOL_LEN     16
	#define PTP_TX_FILT_LAYER4_PROTOCOL_REG_L4_PROTOCOL_DEFAULT 0x0

struct ptp_tx_filt_layer4_protocol_reg {
	a_uint32_t  l4_protocol:16;
};

union ptp_tx_filt_layer4_protocol_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_layer4_protocol_reg bf;
};

/*[register] PTP_TX_FILT_UDP_PORT_REG*/
#define PTP_TX_FILT_UDP_PORT_REG
#define PTP_TX_FILT_UDP_PORT_REG_ADDRESS 0x8630
#define PTP_TX_FILT_UDP_PORT_REG_NUM     1
#define PTP_TX_FILT_UDP_PORT_REG_INC     0x1
#define PTP_TX_FILT_UDP_PORT_REG_TYPE    REG_TYPE_RW
#define PTP_TX_FILT_UDP_PORT_REG_DEFAULT 0x0
	/*[field] UDP_PORT*/
	#define PTP_TX_FILT_UDP_PORT_REG_UDP_PORT
	#define PTP_TX_FILT_UDP_PORT_REG_UDP_PORT_OFFSET  0
	#define PTP_TX_FILT_UDP_PORT_REG_UDP_PORT_LEN     16
	#define PTP_TX_FILT_UDP_PORT_REG_UDP_PORT_DEFAULT 0x0

struct ptp_tx_filt_udp_port_reg {
	a_uint32_t  udp_port:16;
};

union ptp_tx_filt_udp_port_reg_u {
	a_uint32_t val;
	struct ptp_tx_filt_udp_port_reg bf;
};

/*[register] PTP_TX_COM_TS_STATUS_REG*/
#define PTP_TX_COM_TS_STATUS_REG
#define PTP_TX_COM_TS_STATUS_REG_ADDRESS 0x8631
#define PTP_TX_COM_TS_STATUS_REG_NUM     1
#define PTP_TX_COM_TS_STATUS_REG_INC     0x1
#define PTP_TX_COM_TS_STATUS_REG_TYPE    REG_TYPE_RW
#define PTP_TX_COM_TS_STATUS_REG_DEFAULT 0x0
	/*[field] MAC_LENGTHTYPE*/
	#define PTP_TX_COM_TS_STATUS_REG_MAC_LENGTHTYPE
	#define PTP_TX_COM_TS_STATUS_REG_MAC_LENGTHTYPE_OFFSET  0
	#define PTP_TX_COM_TS_STATUS_REG_MAC_LENGTHTYPE_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_MAC_LENGTHTYPE_DEFAULT 0x0
	/*[field] MAC_DA*/
	#define PTP_TX_COM_TS_STATUS_REG_MAC_DA
	#define PTP_TX_COM_TS_STATUS_REG_MAC_DA_OFFSET  1
	#define PTP_TX_COM_TS_STATUS_REG_MAC_DA_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_MAC_DA_DEFAULT 0x0
	/*[field] MAC_PTP_PRIM_ADDR*/
	#define PTP_TX_COM_TS_STATUS_REG_MAC_PTP_PRIM_ADDR
	#define PTP_TX_COM_TS_STATUS_REG_MAC_PTP_PRIM_ADDR_OFFSET  2
	#define PTP_TX_COM_TS_STATUS_REG_MAC_PTP_PRIM_ADDR_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_MAC_PTP_PRIM_ADDR_DEFAULT 0x0
	/*[field] MAC_PTP_PDELAY_ADDR*/
	#define PTP_TX_COM_TS_STATUS_REG_MAC_PTP_PDELAY_ADDR
	#define PTP_TX_COM_TS_STATUS_REG_MAC_PTP_PDELAY_ADDR_OFFSET  3
	#define PTP_TX_COM_TS_STATUS_REG_MAC_PTP_PDELAY_ADDR_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_MAC_PTP_PDELAY_ADDR_DEFAULT 0x0
	/*[field] IPV4_LAYER4_PROTOCOL*/
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_LAYER4_PROTOCOL
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_LAYER4_PROTOCOL_OFFSET  4
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_LAYER4_PROTOCOL_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_LAYER4_PROTOCOL_DEFAULT 0x0
	/*[field] IPV4_DA*/
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_DA
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_DA_OFFSET  5
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_DA_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_DA_DEFAULT 0x0
	/*[field] IPV4_PTP_PRIM_ADDR*/
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_PTP_PRIM_ADDR
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_PTP_PRIM_ADDR_OFFSET  6
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_PTP_PRIM_ADDR_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_PTP_PRIM_ADDR_DEFAULT 0x0
	/*[field] IPV4_PTP_PDELAY_ADDR*/
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_PTP_PDELAY_ADDR
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_PTP_PDELAY_ADDR_OFFSET  7
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_PTP_PDELAY_ADDR_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_IPV4_PTP_PDELAY_ADDR_DEFAULT 0x0
	/*[field] IPV6_NEXT_HEADER*/
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_NEXT_HEADER
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_NEXT_HEADER_OFFSET  8
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_NEXT_HEADER_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_NEXT_HEADER_DEFAULT 0x0
	/*[field] IPV6_DA*/
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_DA
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_DA_OFFSET  9
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_DA_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_DA_DEFAULT 0x0
	/*[field] IPV6_PTP_PRIM_ADDR*/
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_PTP_PRIM_ADDR
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_PTP_PRIM_ADDR_OFFSET  10
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_PTP_PRIM_ADDR_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_PTP_PRIM_ADDR_DEFAULT 0x0
	/*[field] IPV6_PTP_PDELAY_ADDR*/
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_PTP_PDELAY_ADDR
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_PTP_PDELAY_ADDR_OFFSET  11
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_PTP_PDELAY_ADDR_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_IPV6_PTP_PDELAY_ADDR_DEFAULT 0x0
	/*[field] UDP_DPORT*/
	#define PTP_TX_COM_TS_STATUS_REG_UDP_DPORT
	#define PTP_TX_COM_TS_STATUS_REG_UDP_DPORT_OFFSET  12
	#define PTP_TX_COM_TS_STATUS_REG_UDP_DPORT_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_UDP_DPORT_DEFAULT 0x0
	/*[field] UDP_PTP_EVENT_DPORT*/
	#define PTP_TX_COM_TS_STATUS_REG_UDP_PTP_EVENT_DPORT
	#define PTP_TX_COM_TS_STATUS_REG_UDP_PTP_EVENT_DPORT_OFFSET  13
	#define PTP_TX_COM_TS_STATUS_REG_UDP_PTP_EVENT_DPORT_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_UDP_PTP_EVENT_DPORT_DEFAULT 0x0
	/*[field] Y1731_MACH*/
	#define PTP_TX_COM_TS_STATUS_REG_Y1731_MACH
	#define PTP_TX_COM_TS_STATUS_REG_Y1731_MACH_OFFSET  14
	#define PTP_TX_COM_TS_STATUS_REG_Y1731_MACH_LEN     1
	#define PTP_TX_COM_TS_STATUS_REG_Y1731_MACH_DEFAULT 0x0

struct ptp_tx_com_ts_status_reg {
	a_uint32_t  mac_lengthtype:1;
	a_uint32_t  mac_da:1;
	a_uint32_t  mac_ptp_prim_addr:1;
	a_uint32_t  mac_ptp_pdelay_addr:1;
	a_uint32_t  ipv4_layer4_protocol:1;
	a_uint32_t  ipv4_da:1;
	a_uint32_t  ipv4_ptp_prim_addr:1;
	a_uint32_t  ipv4_ptp_pdelay_addr:1;
	a_uint32_t  ipv6_next_header:1;
	a_uint32_t  ipv6_da:1;
	a_uint32_t  ipv6_ptp_prim_addr:1;
	a_uint32_t  ipv6_ptp_pdelay_addr:1;
	a_uint32_t  udp_dport:1;
	a_uint32_t  udp_ptp_event_dport:1;
	a_uint32_t  y1731_mach:1;
};

union ptp_tx_com_ts_status_reg_u {
	a_uint32_t val;
	struct ptp_tx_com_ts_status_reg bf;
};

/*[register] PTP_TX_COM_TIMESTAMP0_REG*/
#define PTP_TX_COM_TIMESTAMP0_REG
#define PTP_TX_COM_TIMESTAMP0_REG_ADDRESS 0x8632
#define PTP_TX_COM_TIMESTAMP0_REG_NUM     1
#define PTP_TX_COM_TIMESTAMP0_REG_INC     0x1
#define PTP_TX_COM_TIMESTAMP0_REG_TYPE    REG_TYPE_RW
#define PTP_TX_COM_TIMESTAMP0_REG_DEFAULT 0x0
	/*[field] COM_TS*/
	#define PTP_TX_COM_TIMESTAMP0_REG_COM_TS
	#define PTP_TX_COM_TIMESTAMP0_REG_COM_TS_OFFSET  0
	#define PTP_TX_COM_TIMESTAMP0_REG_COM_TS_LEN     16
	#define PTP_TX_COM_TIMESTAMP0_REG_COM_TS_DEFAULT 0x0

struct ptp_tx_com_timestamp0_reg {
	a_uint32_t  com_ts:16;
};

union ptp_tx_com_timestamp0_reg_u {
	a_uint32_t val;
	struct ptp_tx_com_timestamp0_reg bf;
};

/*[register] PTP_TX_COM_TIMESTAMP1_REG*/
#define PTP_TX_COM_TIMESTAMP1_REG
#define PTP_TX_COM_TIMESTAMP1_REG_ADDRESS 0x8633
#define PTP_TX_COM_TIMESTAMP1_REG_NUM     1
#define PTP_TX_COM_TIMESTAMP1_REG_INC     0x1
#define PTP_TX_COM_TIMESTAMP1_REG_TYPE    REG_TYPE_RW
#define PTP_TX_COM_TIMESTAMP1_REG_DEFAULT 0x0
	/*[field] COM_TS*/
	#define PTP_TX_COM_TIMESTAMP1_REG_COM_TS
	#define PTP_TX_COM_TIMESTAMP1_REG_COM_TS_OFFSET  0
	#define PTP_TX_COM_TIMESTAMP1_REG_COM_TS_LEN     16
	#define PTP_TX_COM_TIMESTAMP1_REG_COM_TS_DEFAULT 0x0

struct ptp_tx_com_timestamp1_reg {
	a_uint32_t  com_ts:16;
};

union ptp_tx_com_timestamp1_reg_u {
	a_uint32_t val;
	struct ptp_tx_com_timestamp1_reg bf;
};

/*[register] PTP_TX_COM_TIMESTAMP2_REG*/
#define PTP_TX_COM_TIMESTAMP2_REG
#define PTP_TX_COM_TIMESTAMP2_REG_ADDRESS 0x8634
#define PTP_TX_COM_TIMESTAMP2_REG_NUM     1
#define PTP_TX_COM_TIMESTAMP2_REG_INC     0x1
#define PTP_TX_COM_TIMESTAMP2_REG_TYPE    REG_TYPE_RW
#define PTP_TX_COM_TIMESTAMP2_REG_DEFAULT 0x0
	/*[field] COM_TS*/
	#define PTP_TX_COM_TIMESTAMP2_REG_COM_TS
	#define PTP_TX_COM_TIMESTAMP2_REG_COM_TS_OFFSET  0
	#define PTP_TX_COM_TIMESTAMP2_REG_COM_TS_LEN     16
	#define PTP_TX_COM_TIMESTAMP2_REG_COM_TS_DEFAULT 0x0

struct ptp_tx_com_timestamp2_reg {
	a_uint32_t  com_ts:16;
};

union ptp_tx_com_timestamp2_reg_u {
	a_uint32_t val;
	struct ptp_tx_com_timestamp2_reg bf;
};

/*[register] PTP_TX_COM_TIMESTAMP3_REG*/
#define PTP_TX_COM_TIMESTAMP3_REG
#define PTP_TX_COM_TIMESTAMP3_REG_ADDRESS 0x8635
#define PTP_TX_COM_TIMESTAMP3_REG_NUM     1
#define PTP_TX_COM_TIMESTAMP3_REG_INC     0x1
#define PTP_TX_COM_TIMESTAMP3_REG_TYPE    REG_TYPE_RW
#define PTP_TX_COM_TIMESTAMP3_REG_DEFAULT 0x0
	/*[field] COM_TS*/
	#define PTP_TX_COM_TIMESTAMP3_REG_COM_TS
	#define PTP_TX_COM_TIMESTAMP3_REG_COM_TS_OFFSET  0
	#define PTP_TX_COM_TIMESTAMP3_REG_COM_TS_LEN     16
	#define PTP_TX_COM_TIMESTAMP3_REG_COM_TS_DEFAULT 0x0

struct ptp_tx_com_timestamp3_reg {
	a_uint32_t  com_ts:16;
};

union ptp_tx_com_timestamp3_reg_u {
	a_uint32_t val;
	struct ptp_tx_com_timestamp3_reg bf;
};

/*[register] PTP_TX_COM_TIMESTAMP4_REG*/
#define PTP_TX_COM_TIMESTAMP4_REG
#define PTP_TX_COM_TIMESTAMP4_REG_ADDRESS 0x8636
#define PTP_TX_COM_TIMESTAMP4_REG_NUM     1
#define PTP_TX_COM_TIMESTAMP4_REG_INC     0x1
#define PTP_TX_COM_TIMESTAMP4_REG_TYPE    REG_TYPE_RW
#define PTP_TX_COM_TIMESTAMP4_REG_DEFAULT 0x0
	/*[field] COM_TS*/
	#define PTP_TX_COM_TIMESTAMP4_REG_COM_TS
	#define PTP_TX_COM_TIMESTAMP4_REG_COM_TS_OFFSET  0
	#define PTP_TX_COM_TIMESTAMP4_REG_COM_TS_LEN     16
	#define PTP_TX_COM_TIMESTAMP4_REG_COM_TS_DEFAULT 0x0

struct ptp_tx_com_timestamp4_reg {
	a_uint32_t  com_ts:16;
};

union ptp_tx_com_timestamp4_reg_u {
	a_uint32_t val;
	struct ptp_tx_com_timestamp4_reg bf;
};

/*[register] PTP_TX_COM_FRAC_NANO_REG*/
#define PTP_TX_COM_FRAC_NANO_REG
#define PTP_TX_COM_FRAC_NANO_REG_ADDRESS 0x8637
#define PTP_TX_COM_FRAC_NANO_REG_NUM     1
#define PTP_TX_COM_FRAC_NANO_REG_INC     0x1
#define PTP_TX_COM_FRAC_NANO_REG_TYPE    REG_TYPE_RW
#define PTP_TX_COM_FRAC_NANO_REG_DEFAULT 0x0
	/*[field] FRAC_NANO*/
	#define PTP_TX_COM_FRAC_NANO_REG_FRAC_NANO
	#define PTP_TX_COM_FRAC_NANO_REG_FRAC_NANO_OFFSET  0
	#define PTP_TX_COM_FRAC_NANO_REG_FRAC_NANO_LEN     16
	#define PTP_TX_COM_FRAC_NANO_REG_FRAC_NANO_DEFAULT 0x0

struct ptp_tx_com_frac_nano_reg {
	a_uint32_t  frac_nano:16;
};

union ptp_tx_com_frac_nano_reg_u {
	a_uint32_t val;
	struct ptp_tx_com_frac_nano_reg bf;
};

/*[register] PTP_TX_Y1731_IDENTIFY_REG*/
#define PTP_TX_Y1731_IDENTIFY_REG
#define PTP_TX_Y1731_IDENTIFY_REG_ADDRESS 0x863e
#define PTP_TX_Y1731_IDENTIFY_REG_NUM     1
#define PTP_TX_Y1731_IDENTIFY_REG_INC     0x1
#define PTP_TX_Y1731_IDENTIFY_REG_TYPE    REG_TYPE_RW
#define PTP_TX_Y1731_IDENTIFY_REG_DEFAULT 0x0
	/*[field] IDENTIFY*/
	#define PTP_TX_Y1731_IDENTIFY_REG_IDENTIFY
	#define PTP_TX_Y1731_IDENTIFY_REG_IDENTIFY_OFFSET  0
	#define PTP_TX_Y1731_IDENTIFY_REG_IDENTIFY_LEN     16
	#define PTP_TX_Y1731_IDENTIFY_REG_IDENTIFY_DEFAULT 0x0

struct ptp_tx_y1731_identify_reg {
	a_uint32_t  identify:16;
};

union ptp_tx_y1731_identify_reg_u {
	a_uint32_t val;
	struct ptp_tx_y1731_identify_reg bf;
};

/*[register] PTP_Y1731_DM_CONTROL_REG*/
#define PTP_Y1731_DM_CONTROL_REG
#define PTP_Y1731_DM_CONTROL_REG_ADDRESS 0x8640
#define PTP_Y1731_DM_CONTROL_REG_NUM     1
#define PTP_Y1731_DM_CONTROL_REG_INC     0x1
#define PTP_Y1731_DM_CONTROL_REG_TYPE    REG_TYPE_RW
#define PTP_Y1731_DM_CONTROL_REG_DEFAULT 0x0
	/*[field] VALID_MSG_LEV_BMP*/
	#define PTP_Y1731_DM_CONTROL_REG_VALID_MSG_LEV_BMP
	#define PTP_Y1731_DM_CONTROL_REG_VALID_MSG_LEV_BMP_OFFSET  0
	#define PTP_Y1731_DM_CONTROL_REG_VALID_MSG_LEV_BMP_LEN     8
	#define PTP_Y1731_DM_CONTROL_REG_VALID_MSG_LEV_BMP_DEFAULT 0x0
	/*[field] Y1731_DMM_LPBK_EN*/
	#define PTP_Y1731_DM_CONTROL_REG_Y1731_DMM_LPBK_EN
	#define PTP_Y1731_DM_CONTROL_REG_Y1731_DMM_LPBK_EN_OFFSET  8
	#define PTP_Y1731_DM_CONTROL_REG_Y1731_DMM_LPBK_EN_LEN     1
	#define PTP_Y1731_DM_CONTROL_REG_Y1731_DMM_LPBK_EN_DEFAULT 0x0

struct ptp_y1731_dm_control_reg {
	a_uint32_t  valid_msg_lev_bmp:8;
	a_uint32_t  y1731_dmm_lpbk_en:1;
};

union ptp_y1731_dm_control_reg_u {
	a_uint32_t val;
	struct ptp_y1731_dm_control_reg bf;
};

/*[register] PTP_RX_COM_TS_STATUS_PRE_REG*/
#define PTP_RX_COM_TS_STATUS_PRE_REG
#define PTP_RX_COM_TS_STATUS_PRE_REG_ADDRESS 0x8641
#define PTP_RX_COM_TS_STATUS_PRE_REG_NUM     1
#define PTP_RX_COM_TS_STATUS_PRE_REG_INC     0x1
#define PTP_RX_COM_TS_STATUS_PRE_REG_TYPE    REG_TYPE_RW
#define PTP_RX_COM_TS_STATUS_PRE_REG_DEFAULT 0x0
	/*[field] TS_STATUS*/
	#define PTP_RX_COM_TS_STATUS_PRE_REG_TS_STATUS
	#define PTP_RX_COM_TS_STATUS_PRE_REG_TS_STATUS_OFFSET  0
	#define PTP_RX_COM_TS_STATUS_PRE_REG_TS_STATUS_LEN     15
	#define PTP_RX_COM_TS_STATUS_PRE_REG_TS_STATUS_DEFAULT 0x0

struct ptp_rx_com_ts_status_pre_reg {
	a_uint32_t  mac_lengthtype:1;
	a_uint32_t  mac_da:1;
	a_uint32_t  mac_ptp_prim_addr:1;
	a_uint32_t  mac_ptp_pdelay_addr:1;
	a_uint32_t  ipv4_layer4_protocol:1;
	a_uint32_t  ipv4_da:1;
	a_uint32_t  ipv4_ptp_prim_addr:1;
	a_uint32_t  ipv4_ptp_pdelay_addr:1;
	a_uint32_t  ipv6_next_header:1;
	a_uint32_t  ipv6_da:1;
	a_uint32_t  ipv6_ptp_prim_addr:1;
	a_uint32_t  ipv6_ptp_pdelay_addr:1;
	a_uint32_t  udp_dport:1;
	a_uint32_t  udp_ptp_event_dport:1;
	a_uint32_t  y1731_mach:1;
};

union ptp_rx_com_ts_status_pre_reg_u {
	a_uint32_t val;
	struct ptp_rx_com_ts_status_pre_reg bf;
};

/*[register] PTP_BAUD_CONFIG_REG*/
#define PTP_BAUD_CONFIG_REG
#define PTP_BAUD_CONFIG_REG_ADDRESS 0x8700
#define PTP_BAUD_CONFIG_REG_NUM     1
#define PTP_BAUD_CONFIG_REG_INC     0x1
#define PTP_BAUD_CONFIG_REG_TYPE    REG_TYPE_RW
#define PTP_BAUD_CONFIG_REG_DEFAULT 0x0
	/*[field] BAUD_RATE*/
	#define PTP_BAUD_CONFIG_REG_BAUD_RATE
	#define PTP_BAUD_CONFIG_REG_BAUD_RATE_OFFSET  0
	#define PTP_BAUD_CONFIG_REG_BAUD_RATE_LEN     16
	#define PTP_BAUD_CONFIG_REG_BAUD_RATE_DEFAULT 0x0

struct ptp_baud_config_reg {
	a_uint32_t  baud_rate:16;
};

union ptp_baud_config_reg_u {
	a_uint32_t val;
	struct ptp_baud_config_reg bf;
};

/*[register] PTP_UART_CONFIGURATION_REG*/
#define PTP_UART_CONFIGURATION_REG
#define PTP_UART_CONFIGURATION_REG_ADDRESS 0x8701
#define PTP_UART_CONFIGURATION_REG_NUM     1
#define PTP_UART_CONFIGURATION_REG_INC     0x1
#define PTP_UART_CONFIGURATION_REG_TYPE    REG_TYPE_RW
#define PTP_UART_CONFIGURATION_REG_DEFAULT 0x0
	/*[field] START_POLARITY*/
	#define PTP_UART_CONFIGURATION_REG_START_POLARITY
	#define PTP_UART_CONFIGURATION_REG_START_POLARITY_OFFSET  0
	#define PTP_UART_CONFIGURATION_REG_START_POLARITY_LEN     1
	#define PTP_UART_CONFIGURATION_REG_START_POLARITY_DEFAULT 0x0
	/*[field] MSB_FIRST*/
	#define PTP_UART_CONFIGURATION_REG_MSB_FIRST
	#define PTP_UART_CONFIGURATION_REG_MSB_FIRST_OFFSET  1
	#define PTP_UART_CONFIGURATION_REG_MSB_FIRST_LEN     1
	#define PTP_UART_CONFIGURATION_REG_MSB_FIRST_DEFAULT 0x0
	/*[field] PARITY_EN*/
	#define PTP_UART_CONFIGURATION_REG_PARITY_EN
	#define PTP_UART_CONFIGURATION_REG_PARITY_EN_OFFSET  2
	#define PTP_UART_CONFIGURATION_REG_PARITY_EN_LEN     1
	#define PTP_UART_CONFIGURATION_REG_PARITY_EN_DEFAULT 0x0
	/*[field] AUTO_TOD_OUT_EN*/
	#define PTP_UART_CONFIGURATION_REG_AUTO_TOD_OUT_EN
	#define PTP_UART_CONFIGURATION_REG_AUTO_TOD_OUT_EN_OFFSET  3
	#define PTP_UART_CONFIGURATION_REG_AUTO_TOD_OUT_EN_LEN     1
	#define PTP_UART_CONFIGURATION_REG_AUTO_TOD_OUT_EN_DEFAULT 0x0
	/*[field] AUTO_TOD_IN_EN*/
	#define PTP_UART_CONFIGURATION_REG_AUTO_TOD_IN_EN
	#define PTP_UART_CONFIGURATION_REG_AUTO_TOD_IN_EN_OFFSET  4
	#define PTP_UART_CONFIGURATION_REG_AUTO_TOD_IN_EN_LEN     1
	#define PTP_UART_CONFIGURATION_REG_AUTO_TOD_IN_EN_DEFAULT 0x0

struct ptp_uart_configuration_reg {
	a_uint32_t  start_polarity:1;
	a_uint32_t  msb_first:1;
	a_uint32_t  parity_en:1;
	a_uint32_t  auto_tod_out_en:1;
	a_uint32_t  auto_tod_in_en:1;
};

union ptp_uart_configuration_reg_u {
	a_uint32_t val;
	struct ptp_uart_configuration_reg bf;
};

/*[register] PTP_RESET_BUFFER_REG*/
#define PTP_RESET_BUFFER_REG
#define PTP_RESET_BUFFER_REG_ADDRESS 0x8702
#define PTP_RESET_BUFFER_REG_NUM     1
#define PTP_RESET_BUFFER_REG_INC     0x1
#define PTP_RESET_BUFFER_REG_TYPE    REG_TYPE_RW
#define PTP_RESET_BUFFER_REG_DEFAULT 0x0
	/*[field] RESET*/
	#define PTP_RESET_BUFFER_REG_RESET
	#define PTP_RESET_BUFFER_REG_RESET_OFFSET  0
	#define PTP_RESET_BUFFER_REG_RESET_LEN     1
	#define PTP_RESET_BUFFER_REG_RESET_DEFAULT 0x0

struct ptp_reset_buffer_reg {
	a_uint32_t  reset:1;
};

union ptp_reset_buffer_reg_u {
	a_uint32_t val;
	struct ptp_reset_buffer_reg bf;
};

/*[register] PTP_BUFFER_STATUS_REG*/
#define PTP_BUFFER_STATUS_REG
#define PTP_BUFFER_STATUS_REG_ADDRESS 0x8703
#define PTP_BUFFER_STATUS_REG_NUM     1
#define PTP_BUFFER_STATUS_REG_INC     0x1
#define PTP_BUFFER_STATUS_REG_TYPE    REG_TYPE_RW
#define PTP_BUFFER_STATUS_REG_DEFAULT 0x0
	/*[field] TX_BUFFER_ALMOST_EMPTY*/
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_ALMOST_EMPTY
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_ALMOST_EMPTY_OFFSET  0
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_ALMOST_EMPTY_LEN     1
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_ALMOST_EMPTY_DEFAULT 0x0
	/*[field] TX_BUFFER_ALMOST_FULL*/
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_ALMOST_FULL
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_ALMOST_FULL_OFFSET  1
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_ALMOST_FULL_LEN     1
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_ALMOST_FULL_DEFAULT 0x0
	/*[field] TX_BUFFER_HALF_FULL*/
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_HALF_FULL
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_HALF_FULL_OFFSET  2
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_HALF_FULL_LEN     1
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_HALF_FULL_DEFAULT 0x0
	/*[field] TX_BUFFER_FULL*/
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_FULL
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_FULL_OFFSET  3
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_FULL_LEN     1
	#define PTP_BUFFER_STATUS_REG_TX_BUFFER_FULL_DEFAULT 0x0
	/*[field] RX_BUFFER_ALMOST_EMPTY*/
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_ALMOST_EMPTY
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_ALMOST_EMPTY_OFFSET  4
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_ALMOST_EMPTY_LEN     1
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_ALMOST_EMPTY_DEFAULT 0x0
	/*[field] RX_BUFFER_ALMOST_FULL*/
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_ALMOST_FULL
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_ALMOST_FULL_OFFSET  5
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_ALMOST_FULL_LEN     1
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_ALMOST_FULL_DEFAULT 0x0
	/*[field] RX_BUFFER_HALF_FULL*/
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_HALF_FULL
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_HALF_FULL_OFFSET  6
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_HALF_FULL_LEN     1
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_HALF_FULL_DEFAULT 0x0
	/*[field] RX_BUFFER_FULL*/
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_FULL
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_FULL_OFFSET  7
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_FULL_LEN     1
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_FULL_DEFAULT 0x0
	/*[field] RX_BUFFER_DATA_PRESENT*/
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_DATA_PRESENT
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_DATA_PRESENT_OFFSET  8
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_DATA_PRESENT_LEN     1
	#define PTP_BUFFER_STATUS_REG_RX_BUFFER_DATA_PRESENT_DEFAULT 0x0

struct ptp_buffer_status_reg {
	a_uint32_t  tx_buffer_almost_empty:1;
	a_uint32_t  tx_buffer_almost_full:1;
	a_uint32_t  tx_buffer_half_full:1;
	a_uint32_t  tx_buffer_full:1;
	a_uint32_t  rx_buffer_almost_empty:1;
	a_uint32_t  rx_buffer_almost_full:1;
	a_uint32_t  rx_buffer_half_full:1;
	a_uint32_t  rx_buffer_full:1;
	a_uint32_t  rx_buffer_data_present:1;
};

union ptp_buffer_status_reg_u {
	a_uint32_t val;
	struct ptp_buffer_status_reg bf;
};

/*[register] PTP_TX_BUFFER_WRITE_REG*/
#define PTP_TX_BUFFER_WRITE_REG
#define PTP_TX_BUFFER_WRITE_REG_ADDRESS 0x8704
#define PTP_TX_BUFFER_WRITE_REG_NUM     1
#define PTP_TX_BUFFER_WRITE_REG_INC     0x1
#define PTP_TX_BUFFER_WRITE_REG_TYPE    REG_TYPE_RW
#define PTP_TX_BUFFER_WRITE_REG_DEFAULT 0x0
	/*[field] TX_BUFFER*/
	#define PTP_TX_BUFFER_WRITE_REG_TX_BUFFER
	#define PTP_TX_BUFFER_WRITE_REG_TX_BUFFER_OFFSET  0
	#define PTP_TX_BUFFER_WRITE_REG_TX_BUFFER_LEN     8
	#define PTP_TX_BUFFER_WRITE_REG_TX_BUFFER_DEFAULT 0x0

struct ptp_tx_buffer_write_reg {
	a_uint32_t  tx_buffer:8;
};

union ptp_tx_buffer_write_reg_u {
	a_uint32_t val;
	struct ptp_tx_buffer_write_reg bf;
};

/*[register] PTP_RX_BUFFER_READ_REG*/
#define PTP_RX_BUFFER_READ_REG
#define PTP_RX_BUFFER_READ_REG_ADDRESS 0x8705
#define PTP_RX_BUFFER_READ_REG_NUM     1
#define PTP_RX_BUFFER_READ_REG_INC     0x1
#define PTP_RX_BUFFER_READ_REG_TYPE    REG_TYPE_RW
#define PTP_RX_BUFFER_READ_REG_DEFAULT 0x0
	/*[field] RX_DATA*/
	#define PTP_RX_BUFFER_READ_REG_RX_DATA
	#define PTP_RX_BUFFER_READ_REG_RX_DATA_OFFSET  0
	#define PTP_RX_BUFFER_READ_REG_RX_DATA_LEN     8
	#define PTP_RX_BUFFER_READ_REG_RX_DATA_DEFAULT 0x0
	/*[field] RX_BUFFER_ALMOST_EMPTY*/
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_ALMOST_EMPTY
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_ALMOST_EMPTY_OFFSET  8
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_ALMOST_EMPTY_LEN     1
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_ALMOST_EMPTY_DEFAULT 0x0
	/*[field] RX_BUFFER_ALMOST_FULL*/
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_ALMOST_FULL
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_ALMOST_FULL_OFFSET  9
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_ALMOST_FULL_LEN     1
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_ALMOST_FULL_DEFAULT 0x0
	/*[field] RX_BUFFER_HALF_FULL*/
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_HALF_FULL
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_HALF_FULL_OFFSET  10
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_HALF_FULL_LEN     1
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_HALF_FULL_DEFAULT 0x0
	/*[field] RX_BUFFER_FULL*/
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_FULL
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_FULL_OFFSET  11
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_FULL_LEN     1
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_FULL_DEFAULT 0x0
	/*[field] RX_BUFFER_DATA_PRESENT*/
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_DATA_PRESENT
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_DATA_PRESENT_OFFSET  12
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_DATA_PRESENT_LEN     1
	#define PTP_RX_BUFFER_READ_REG_RX_BUFFER_DATA_PRESENT_DEFAULT 0x0

struct ptp_rx_buffer_read_reg {
	a_uint32_t  rx_data:8;
	a_uint32_t  rx_buffer_almost_empty:1;
	a_uint32_t  rx_buffer_almost_full:1;
	a_uint32_t  rx_buffer_half_full:1;
	a_uint32_t  rx_buffer_full:1;
	a_uint32_t  rx_buffer_data_present:1;
};

union ptp_rx_buffer_read_reg_u {
	a_uint32_t val;
	struct ptp_rx_buffer_read_reg bf;
};

/*[register] PTP_LOC_MAC_ADDR_0_REG*/
#define PTP_LOC_MAC_ADDR_0_REG
#define PTP_LOC_MAC_ADDR_0_REG_ADDRESS 0x804a
#define PTP_LOC_MAC_ADDR_0_REG_NUM     1
#define PTP_LOC_MAC_ADDR_0_REG_INC     0x1
#define PTP_LOC_MAC_ADDR_0_REG_TYPE    REG_TYPE_RW
#define PTP_LOC_MAC_ADDR_0_REG_DEFAULT 0x0
	/*[field] MAC_ADDR*/
	#define PTP_LOC_MAC_ADDR_0_REG_MAC_ADDR
	#define PTP_LOC_MAC_ADDR_0_REG_MAC_ADDR_OFFSET  0
	#define PTP_LOC_MAC_ADDR_0_REG_MAC_ADDR_LEN     16
	#define PTP_LOC_MAC_ADDR_0_REG_MAC_ADDR_DEFAULT 0x0

struct ptp_loc_mac_addr_0_reg {
	a_uint32_t  mac_addr:16;
};

union ptp_loc_mac_addr_0_reg_u {
	a_uint32_t val;
	struct ptp_loc_mac_addr_0_reg bf;
};

/*[register] PTP_LOC_MAC_ADDR_1_REG*/
#define PTP_LOC_MAC_ADDR_1_REG
#define PTP_LOC_MAC_ADDR_1_REG_ADDRESS 0x804b
#define PTP_LOC_MAC_ADDR_1_REG_NUM     1
#define PTP_LOC_MAC_ADDR_1_REG_INC     0x1
#define PTP_LOC_MAC_ADDR_1_REG_TYPE    REG_TYPE_RW
#define PTP_LOC_MAC_ADDR_1_REG_DEFAULT 0x0
	/*[field] MAC_ADDR*/
	#define PTP_LOC_MAC_ADDR_1_REG_MAC_ADDR
	#define PTP_LOC_MAC_ADDR_1_REG_MAC_ADDR_OFFSET  0
	#define PTP_LOC_MAC_ADDR_1_REG_MAC_ADDR_LEN     16
	#define PTP_LOC_MAC_ADDR_1_REG_MAC_ADDR_DEFAULT 0x0

struct ptp_loc_mac_addr_1_reg {
	a_uint32_t  mac_addr:16;
};

union ptp_loc_mac_addr_1_reg_u {
	a_uint32_t val;
	struct ptp_loc_mac_addr_1_reg bf;
};

/*[register] PTP_LOC_MAC_ADDR_2_REG*/
#define PTP_LOC_MAC_ADDR_2_REG
#define PTP_LOC_MAC_ADDR_2_REG_ADDRESS 0x804c
#define PTP_LOC_MAC_ADDR_2_REG_NUM     1
#define PTP_LOC_MAC_ADDR_2_REG_INC     0x1
#define PTP_LOC_MAC_ADDR_2_REG_TYPE    REG_TYPE_RW
#define PTP_LOC_MAC_ADDR_2_REG_DEFAULT 0x0
	/*[field] MAC_ADDR*/
	#define PTP_LOC_MAC_ADDR_2_REG_MAC_ADDR
	#define PTP_LOC_MAC_ADDR_2_REG_MAC_ADDR_OFFSET  0
	#define PTP_LOC_MAC_ADDR_2_REG_MAC_ADDR_LEN     16
	#define PTP_LOC_MAC_ADDR_2_REG_MAC_ADDR_DEFAULT 0x0

struct ptp_loc_mac_addr_2_reg {
	a_uint32_t  mac_addr:16;
};

union ptp_loc_mac_addr_2_reg_u {
	a_uint32_t val;
	struct ptp_loc_mac_addr_2_reg bf;
};

/*[register] PTP_LINK_DELAY_0_REG*/
#define PTP_LINK_DELAY_0_REG
#define PTP_LINK_DELAY_0_REG_ADDRESS 0x80f3
#define PTP_LINK_DELAY_0_REG_NUM     1
#define PTP_LINK_DELAY_0_REG_INC     0x1
#define PTP_LINK_DELAY_0_REG_TYPE    REG_TYPE_RW
#define PTP_LINK_DELAY_0_REG_DEFAULT 0x0
	/*[field] LINK_DELAY*/
	#define PTP_LINK_DELAY_0_REG_LINK_DELAY
	#define PTP_LINK_DELAY_0_REG_LINK_DELAY_OFFSET  0
	#define PTP_LINK_DELAY_0_REG_LINK_DELAY_LEN     16
	#define PTP_LINK_DELAY_0_REG_LINK_DELAY_DEFAULT 0x0

struct ptp_link_delay_0_reg {
	a_uint32_t  link_delay:16;
};

union ptp_link_delay_0_reg_u {
	a_uint32_t val;
	struct ptp_link_delay_0_reg bf;
};

/*[register] PTP_LINK_DELAY_1_REG*/
#define PTP_LINK_DELAY_1_REG
#define PTP_LINK_DELAY_1_REG_ADDRESS 0x80f4
#define PTP_LINK_DELAY_1_REG_NUM     1
#define PTP_LINK_DELAY_1_REG_INC     0x1
#define PTP_LINK_DELAY_1_REG_TYPE    REG_TYPE_RW
#define PTP_LINK_DELAY_1_REG_DEFAULT 0x0
	/*[field] LINK_DELAY*/
	#define PTP_LINK_DELAY_1_REG_LINK_DELAY
	#define PTP_LINK_DELAY_1_REG_LINK_DELAY_OFFSET  0
	#define PTP_LINK_DELAY_1_REG_LINK_DELAY_LEN     16
	#define PTP_LINK_DELAY_1_REG_LINK_DELAY_DEFAULT 0x0

struct ptp_link_delay_1_reg {
	a_uint32_t  link_delay:16;
};

union ptp_link_delay_1_reg_u {
	a_uint32_t val;
	struct ptp_link_delay_1_reg bf;
};

/*[register] PTP_MISC_CONTROL_REG*/
#define PTP_MISC_CONTROL_REG
#define PTP_MISC_CONTROL_REG_ADDRESS 0x80f5
#define PTP_MISC_CONTROL_REG_NUM     1
#define PTP_MISC_CONTROL_REG_INC     0x1
#define PTP_MISC_CONTROL_REG_TYPE    REG_TYPE_RW
#define PTP_MISC_CONTROL_REG_DEFAULT 0x0
	/*[field] EG_ASYM_EN*/
	#define PTP_MISC_CONTROL_REG_EG_ASYM_EN
	#define PTP_MISC_CONTROL_REG_EG_ASYM_EN_OFFSET  0
	#define PTP_MISC_CONTROL_REG_EG_ASYM_EN_LEN     1
	#define PTP_MISC_CONTROL_REG_EG_ASYM_EN_DEFAULT 0x0
	/*[field] IN_ASYM_EN*/
	#define PTP_MISC_CONTROL_REG_IN_ASYM_EN
	#define PTP_MISC_CONTROL_REG_IN_ASYM_EN_OFFSET  1
	#define PTP_MISC_CONTROL_REG_IN_ASYM_EN_LEN     1
	#define PTP_MISC_CONTROL_REG_IN_ASYM_EN_DEFAULT 0x0

struct ptp_misc_control_reg {
	a_uint32_t  eg_asym_en:1;
	a_uint32_t  in_asym_en:1;
};

union ptp_misc_control_reg_u {
	a_uint32_t val;
	struct ptp_misc_control_reg bf;
};

/*[register] PTP_INGRESS_ASYMMETRY_0_REG*/
#define PTP_INGRESS_ASYMMETRY_0_REG
#define PTP_INGRESS_ASYMMETRY_0_REG_ADDRESS 0x80f6
#define PTP_INGRESS_ASYMMETRY_0_REG_NUM     1
#define PTP_INGRESS_ASYMMETRY_0_REG_INC     0x1
#define PTP_INGRESS_ASYMMETRY_0_REG_TYPE    REG_TYPE_RW
#define PTP_INGRESS_ASYMMETRY_0_REG_DEFAULT 0x0
	/*[field] IN_ASYM*/
	#define PTP_INGRESS_ASYMMETRY_0_REG_IN_ASYM
	#define PTP_INGRESS_ASYMMETRY_0_REG_IN_ASYM_OFFSET  0
	#define PTP_INGRESS_ASYMMETRY_0_REG_IN_ASYM_LEN     16
	#define PTP_INGRESS_ASYMMETRY_0_REG_IN_ASYM_DEFAULT 0x0

struct ptp_ingress_asymmetry_0_reg {
	a_uint32_t  in_asym:16;
};

union ptp_ingress_asymmetry_0_reg_u {
	a_uint32_t val;
	struct ptp_ingress_asymmetry_0_reg bf;
};

/*[register] PTP_INGRESS_ASYMMETRY_1_REG*/
#define PTP_INGRESS_ASYMMETRY_1_REG
#define PTP_INGRESS_ASYMMETRY_1_REG_ADDRESS 0x80f7
#define PTP_INGRESS_ASYMMETRY_1_REG_NUM     1
#define PTP_INGRESS_ASYMMETRY_1_REG_INC     0x1
#define PTP_INGRESS_ASYMMETRY_1_REG_TYPE    REG_TYPE_RW
#define PTP_INGRESS_ASYMMETRY_1_REG_DEFAULT 0x0
	/*[field] IN_ASYM*/
	#define PTP_INGRESS_ASYMMETRY_1_REG_IN_ASYM
	#define PTP_INGRESS_ASYMMETRY_1_REG_IN_ASYM_OFFSET  0
	#define PTP_INGRESS_ASYMMETRY_1_REG_IN_ASYM_LEN     16
	#define PTP_INGRESS_ASYMMETRY_1_REG_IN_ASYM_DEFAULT 0x0

struct ptp_ingress_asymmetry_1_reg {
	a_uint32_t  in_asym:16;
};

union ptp_ingress_asymmetry_1_reg_u {
	a_uint32_t val;
	struct ptp_ingress_asymmetry_1_reg bf;
};

/*[register] PTP_EGRESS_ASYMMETRY_0_REG*/
#define PTP_EGRESS_ASYMMETRY_0_REG
#define PTP_EGRESS_ASYMMETRY_0_REG_ADDRESS 0x80f8
#define PTP_EGRESS_ASYMMETRY_0_REG_NUM     1
#define PTP_EGRESS_ASYMMETRY_0_REG_INC     0x1
#define PTP_EGRESS_ASYMMETRY_0_REG_TYPE    REG_TYPE_RW
#define PTP_EGRESS_ASYMMETRY_0_REG_DEFAULT 0x0
	/*[field] EG_ASYM*/
	#define PTP_EGRESS_ASYMMETRY_0_REG_EG_ASYM
	#define PTP_EGRESS_ASYMMETRY_0_REG_EG_ASYM_OFFSET  0
	#define PTP_EGRESS_ASYMMETRY_0_REG_EG_ASYM_LEN     16
	#define PTP_EGRESS_ASYMMETRY_0_REG_EG_ASYM_DEFAULT 0x0

struct ptp_egress_asymmetry_0_reg {
	a_uint32_t  eg_asym:16;
};

union ptp_egress_asymmetry_0_reg_u {
	a_uint32_t val;
	struct ptp_egress_asymmetry_0_reg bf;
};

/*[register] PTP_EGRESS_ASYMMETRY_1_REG*/
#define PTP_EGRESS_ASYMMETRY_1_REG
#define PTP_EGRESS_ASYMMETRY_1_REG_ADDRESS 0x80f9
#define PTP_EGRESS_ASYMMETRY_1_REG_NUM     1
#define PTP_EGRESS_ASYMMETRY_1_REG_INC     0x1
#define PTP_EGRESS_ASYMMETRY_1_REG_TYPE    REG_TYPE_RW
#define PTP_EGRESS_ASYMMETRY_1_REG_DEFAULT 0x0
	/*[field] EG_ASYM*/
	#define PTP_EGRESS_ASYMMETRY_1_REG_EG_ASYM
	#define PTP_EGRESS_ASYMMETRY_1_REG_EG_ASYM_OFFSET  0
	#define PTP_EGRESS_ASYMMETRY_1_REG_EG_ASYM_LEN     16
	#define PTP_EGRESS_ASYMMETRY_1_REG_EG_ASYM_DEFAULT 0x0

struct ptp_egress_asymmetry_1_reg {
	a_uint32_t  eg_asym:16;
};

union ptp_egress_asymmetry_1_reg_u {
	a_uint32_t val;
	struct ptp_egress_asymmetry_1_reg bf;
};

/*[register] PTP_BACKUP_REG*/
#define PTP_BACKUP_REG
#define PTP_BACKUP_REG_ADDRESS 0x9036
#define PTP_BACKUP_REG_NUM     1
#define PTP_BACKUP_REG_INC     0x1
#define PTP_BACKUP_REG_TYPE    REG_TYPE_RW
#define PTP_BACKUP_REG_DEFAULT 0x0
	/*[field] P2P_TC_EN*/
	#define PTP_BACKUP_REG_P2P_TC_EN
	#define PTP_BACKUP_REG_P2P_TC_EN_OFFSET  0
	#define PTP_BACKUP_REG_P2P_TC_EN_LEN     1
	#define PTP_BACKUP_REG_P2P_TC_EN_DEFAULT 0x0

struct ptp_backup_reg {
	a_uint32_t  p2p_tc_en:1;
};

union ptp_backup_reg_u {
	a_uint32_t val;
	struct ptp_backup_reg bf;
};

#endif
