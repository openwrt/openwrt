/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
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
#ifndef CPPE_LOOPBACK_REG_H
#define CPPE_LOOPBACK_REG_H

/*[register] LPBK_ENABLE*/
#define LPBK_ENABLE
#define LPBK_ENABLE_ADDRESS 0x00
#define LPBK_ENABLE_NUM     6
#define LPBK_ENABLEL_INC     0x200
#define LPBK_ENABLE_TYPE    REG_TYPE_RW
#define LPBK_ENABLE_DEFAULT 0x0
	/*[field] LPBK_EN*/
	#define LPBK_EN_LPBK_EN
	#define LPBK_EN_LPBK_EN_OFFSET  0
	#define LPBK_EN_LPBK_EN_LEN     1
	#define LLPBK_EN_LPBK_EN_DEFAULT 0x0
	/*[field] FLOWCTRL_EN*/
	#define LPBK_ENABLE_FLOWCTRL_EN
	#define LPBK_ENABLE_FLOWCTRL_EN_OFFSET  1
	#define LPBK_ENABLE_FLOWCTRL_EN_LEN     1
	#define LPBK_ENABLE_FLOWCTRL_EN_DEFAULT 0x1
	/*[field] FLOWCTRL_MODE*/
	#define LPBK_EN_FLOWCTRL_MODE
	#define LPBK_EN_FLOWCTRL_MODE_OFFSET  2
	#define LPBK_EN_FLOWCTRL_MODE_LEN     1
	#define LPBK_EN_FLOWCTRL_MODE_DEFAULT 0x0
	/*[field] CRC_STRIP_EN*/
	#define LPBK_EN_CRC_STRIP_EN
	#define LPBK_EN_CRC_STRIP_EN_OFFSET  2
	#define LPBK_EN_CRC_STRIP_EN_LEN     1
	#define LPBK_EN_CRC_STRIP_EN_DEFAULT 0x1
struct lpbk_enable {
	a_uint32_t lpbk_en:1;
	a_uint32_t flowctrl_en :1;
	a_uint32_t flowctrl_mode:1;
	a_uint32_t crc_strip_en:1;
	a_uint32_t _reserved0:28;
};
union lpbk_enable_u {
	a_uint32_t val;
	struct lpbk_enable bf;
};

/*[register] LPBK_FIFO_1_CTRL*/
#define LPBK_FIFO_1_CTRL
#define LPBK_FIFO_1_CTRL_ADDRESS 0x04
#define LPBK_FIFO_1_CTRL_NUM     6
#define LPBK_FIFO_1_CTRL_INC     0x200
#define LPBK_FIFO_1_CTRL_TYPE    REG_TYPE_RW
#define LPBK_FIFO_1_CTRL_DEFAULT 0x3
	/*[field] LPBK_FIFO_1_THRESHOLD*/
	#define LPBK_FIFO_1_CTRL_LPBK_FIFO_1_THRESHOLD
	#define LPBK_FIFO_1_CTRL_LPBK_FIFO_1_THRESHOLD_OFFSET  0
	#define LPBK_FIFO_1_CTRL_LPBK_FIFO_1_THRESHOLD_LEN     3
	#define LPBK_FIFO_1_CTRL_LPBK_FIFO_1_THRESHOLD_DEFAULT 0x0
struct lpbk_fifo_1_ctrl {
	a_uint32_t lpbk_fifo_1_threshold:3;
	a_uint32_t _reserved0:29;
};
union lpbk_fifo_1_ctrl_u {
	a_uint32_t val;
	struct lpbk_fifo_1_ctrl bf;
};

/*[register] LPBK_FIFO_2_CTRL*/
#define LPBK_FIFO_2_CTRL
#define LPBK_FIFO_2_CTRL_ADDRESS 0x08
#define LPBK_FIFO_2_CTRL_NUM     6
#define LPBK_FIFO_2_CTRL_INC     0x200
#define LPBK_FIFO_2_CTRL_TYPE    REG_TYPE_RW
#define LPBK_FIFO_2_CTRL_DEFAULT 0x1
	/*[field] LPBK_FIFO_2_THRESHOLD*/
	#define LPBK_FIFO_2_CTRL_LPBK_FIFO_2_THRESHOLD
	#define LPBK_FIFO_2_CTRL_LPBK_FIFO_2_THRESHOLD_OFFSET  0
	#define LPBK_FIFO_2_CTRL_LPBK_FIFO_2_THRESHOLD_LEN     3
	#define LPBK_FIFO_2_CTRL_LPBK_FIFO_2_THRESHOLD_DEFAULT 0x0
struct lpbk_fifo_2_ctrl {
	a_uint32_t lpbk_fifo_2_threshold:3;
	a_uint32_t _reserved0:29;
};
union lpbk_fifo_2_ctrl_u {
	a_uint32_t val;
	struct lpbk_fifo_2_ctrl bf;
};

/*[register] LPBK_PPS_CTRL*/
#define LPBK_PPS_CTRL
#define LPBK_PPS_CTRL_ADDRESS 0x0c
#define LPBK_PPS_CTRL_NUM     6
#define LPBK_PPS_CTRL_INC     0x200
#define LPBK_PPS_CTRL_TYPE    REG_TYPE_RW
#define LPBK_PPS_CTRL_DEFAULT 0x0
	/*[field] LPBK_PPS_THRESHOLD*/
	#define LPBK_PPS_CTRL_LPBK_PPS_THRESHOLD
	#define LPBK_PPS_CTRL_LPBK_PPS_THRESHOLD_OFFSET  0
	#define LPBK_PPS_CTRL_LPBK_PPS_THRESHOLD_LEN     9
	#define LPBK_PPS_CTRL_LPBK_PPS_THRESHOLD_DEFAULT 0x16
struct lpbk_pps_ctrl {
	a_uint32_t lpbk_pps_threshold:9;
	a_uint32_t _reserved0:13;
};
union lpbk_pps_ctrl_u {
	a_uint32_t val;
	struct lpbk_pps_ctrl bf;
};

/*[register] LPBK_MAC_JUNMO_SIZE*/
#define LPBK_MAC_JUNMO_SIZE
#define LPBK_MAC_JUNMO_SIZE_ADDRESS 0x10
#define LPBK_MAC_JUNMO_SIZE_NUM     6
#define LPBK_MAC_JUNMO_SIZE_INC     0x200
#define LPBK_MAC_JUNMO_SIZE_TYPE    REG_TYPE_RW
#define LPBK_MAC_JUNMO_SIZE_DEFAULT 0x0
	/*[field] LPBK_MAC_JUMBO_SIZE*/
	#define LPBK_MAC_JUNMO_SIZE_LPBK_MAC_JUMBO_SIZE
	#define LPBK_MAC_JUNMO_SIZE_LPBK_MAC_JUMBO_SIZE_OFFSET  0
	#define LPBK_MAC_JUNMO_SIZE_LPBK_MAC_JUMBO_SIZE_LEN     14
	#define LPBK_MAC_JUNMO_SIZE_LPBK_MAC_JUMBO_SIZE_DEFAULT 0x5EA
struct lpbk_mac_junmo_size {
	a_uint32_t lpbk_mac_jumbo_size:14;
	a_uint32_t _reserved0:18;
};
union lpbk_mac_junmo_size_u {
	a_uint32_t val;
	struct lpbk_mac_junmo_size bf;
};

/*[register] LPBK_MIB_CTRL*/
#define LPBK_MIB_CTRL
#define LPBK_MIB_CTRL_ADDRESS 0x14
#define LPBK_MIB_CTRL_NUM     6
#define LPBK_MIB_CTRL_INC     0x200
#define LPBK_MIB_CTRL_TYPE    REG_TYPE_RW
#define LPBK_MIB_CTRL_DEFAULT 0x0
	/*[field] MIB_EN*/
	#define LPBK_MIB_CTRL_MIB_EN
	#define LPBK_MIB_CTRL_MIB_EN_OFFSET  0
	#define LPBK_MIB_CTRL_MIB_EN_LEN     1
	#define LPBK_MIB_CTRL_MIB_EN_DEFAULT 0x1
	/*[field] MIB_RESET*/
	#define LPBK_MIB_CTRL_MIB_RESET
	#define LPBK_MIB_CTRL_MIB_RESET_OFFSET  1
	#define LPBK_MIB_CTRL_MIB_RESET_LEN     1
	#define LPBK_MIB_CTRL_MIB_RESET_DEFAULT 0x0
	/*[field] MIB_RD_CLR*/
	#define LPBK_MIB_CTRL_MIB_RD_CLR
	#define LPBK_MIB_CTRL_MIB_RD_CLR_OFFSET  2
	#define LPBK_MIB_CTRL_MIB_RD_CLR_LEN     1
	#define LPBK_MIB_CTRL_MIB_RD_CLR_DEFAULT 0x0
struct lpbk_mib_ctrl {
	a_uint32_t  mib_en:1;
	a_uint32_t  mib_reset:1;
	a_uint32_t  mib_rd_clr:1;
	a_uint32_t  _reserved0:29;
};
union lpbk_mib_ctrl_u {
	a_uint32_t val;
	struct lpbk_mib_ctrl bf;
};

/*[register] LPBKUNI*/
#define LPBKUNI
#define LPBKUNI_ADDRESS 0x20
#define LPBKUNI_NUM     6
#define LPBKUNI_INC     0x200
#define LPBKUNI_TYPE    REG_TYPE_RO
#define LPBKUNI_DEFAULT 0x0
	/*[field] LPBKUNI*/
	#define LPBKUNI_LPBKUNI
	#define LPBKUNI_LPBKUNI_OFFSET  0
	#define LPBKUNI_LPBKUNI_LEN     32
	#define LPBKUNI_LPBKUNI_DEFAULT 0x0
struct lpbkuni {
	a_uint32_t  lpbkuni:32;
};
union lpbkuni_u {
	a_uint32_t val;
	struct lpbkuni bf;
};

/*[register] LPBKMULTI*/
#define LPBKMULTI
#define LPBKMULTI_ADDRESS 0x24
#define LPBKMULTI_NUM     6
#define LPBKMULTI_INC     0x200
#define LPBKMULTI_TYPE    REG_TYPE_RO
#define LPBKMULTI_DEFAULT 0x0
	/*[field] LPBKMULTI*/
	#define LPBKMULTI_LPBKMULTI
	#define LPBKMULTI_LPBKMULTI_OFFSET  0
	#define LPBKMULTI_LPBKMULTI_LEN     32
	#define LPBKMULTI_LPBKMULTI_DEFAULT 0x0
struct lpbkmulti {
	a_uint32_t  lpbkmulti:32;
};
union lpbkmulti_u {
	a_uint32_t val;
	struct lpbkmulti bf;
};

/*[register] LPBKBROAD*/
#define LPBKBROAD
#define LPBKBROAD_ADDRESS 0x28
#define LPBKBROAD_NUM     6
#define LPBKBROAD_INC     0x200
#define LPBKBROAD_TYPE    REG_TYPE_RO
#define LPBKBROAD_DEFAULT 0x0
	/*[field] LPBK_BROAD*/
	#define LPBKBROAD_LPBKBROAD
	#define LPBKBROAD_LPBKBROAD_OFFSET  0
	#define LPBKBROAD_LPBKBROAD_LEN     32
	#define LPBKBROAD_LPBKBROAD_DEFAULT 0x0
struct lpbkbroad {
	a_uint32_t  lpbkbroad:32;
};
union lpbkbroad_u {
	a_uint32_t val;
	struct lpbkbroad bf;
};

/*[register] LPBKPKT64*/
#define LPBKPKT64
#define LPBKPKT64_ADDRESS 0x2c
#define LPBKPKT64_NUM     6
#define LPBKPKT64_INC     0x200
#define LPBKPKT64_TYPE    REG_TYPE_RO
#define LPBKPKT64_DEFAULT 0x0
	/*[field] LPBKPKT64*/
	#define LPBKPKT64_LPBKPKT64
	#define LPBKPKT64_LPBKPKT64_OFFSET  0
	#define LPBKPKT64_LPBKPKT64_LEN     32
	#define LPBKPKT64_LPBKPKT64_DEFAULT 0x0
struct lpbkpkt64 {
	a_uint32_t  lpbkpkt64:32;
};
union lpbkpkt64_u {
	a_uint32_t val;
	struct lpbkpkt64 bf;
};

/*[register] LPBKPKT65TO127*/
#define LPBKPKT65TO127
#define LPBKPKT65TO127_ADDRESS 0x30
#define LPBKPKT65TO127_NUM     6
#define LPBKPKT65TO127_INC     0x200
#define LPBKPKT65TO127_TYPE    REG_TYPE_RO
#define LPBKPKT65TO1277_DEFAULT 0x0
	/*[field] LPBKPKT65TO127*/
	#define LPBKPKT65TO127_LPBKPKT65TO127
	#define LPBKPKT65TO127_LPBKPKT65TO127_OFFSET  0
	#define LPBKPKT65TO127_LPBKPKT65TO127_LEN     32
	#define LPBKPKT65TO127_LPBKPKT65TO127_DEFAULT 0x0
struct lpbkpkt65to127 {
	a_uint32_t  lpbkpkt65to127:32;
};
union lpbkpkt65to127_u {
	a_uint32_t val;
	struct lpbkpkt65to127 bf;
};

/*[register] LPBKPKT128TO255*/
#define LPBKPKT128TO255
#define LPBKPKT128TO255_ADDRESS 0x34
#define LPBKPKT128TO255_NUM     6
#define LPBKPKT128TO255_INC     0x200
#define LPBKPKT128TO255_TYPE    REG_TYPE_RO
#define LPBKPKT128TO255_DEFAULT 0x0
	/*[field] LPBKPKT128TO255*/
	#define LPBKPKT128TO255_LPBKPKT128TO255
	#define LPBKPKT128TO255_LPBKPKT128TO255_OFFSET  0
	#define LPBKPKT128TO255_LPBKPKT128TO255_LEN     32
	#define LPBKPKT128TO255_LPBKPKT128TO255_DEFAULT 0x0
struct lpbkpkt128to255 {
	a_uint32_t  lpbkpkt128to255:32;
};
union lpbkpkt128to255_u {
	a_uint32_t val;
	struct lpbkpkt128to255 bf;
};

/*[register] LPBKPKT256TO511*/
#define LPBKPKT256TO511
#define LPBKPKT256TO511_ADDRESS 0x38
#define LPBKPKT256TO511_NUM     6
#define LPBKPKT256TO511_INC     0x200
#define LPBKPKT256TO511_TYPE    REG_TYPE_RO
#define LPBKPKT256TO511_DEFAULT 0x0
	/*[field] LPBKPKT256TO511*/
	#define LPBKPKT256TO511_LPBKPKT256TO511
	#define LPBKPKT256TO511_LPBKPKT256TO511_OFFSET  0
	#define LPBKPKT256TO511_LPBKPKT256TO511_LEN     32
	#define LPBKPKT256TO511_LPBKPKT256TO511_DEFAULT 0x0
struct lpbkpkt256to511 {
	a_uint32_t  lpbkpkt256to511:32;
};
union lpbkpkt256to511_u {
	a_uint32_t val;
	struct lpbkpkt256to511 bf;
};

/*[register] LPBKPKT512TO1023*/
#define LPBKPKT512TO1023
#define LPBKPKT512TO1023_ADDRESS 0x3c
#define LPBKPKT512TO1023_NUM     6
#define LPBKPKT512TO1023_INC     0x200
#define LPBKPKT512TO1023_TYPE    REG_TYPE_RO
#define LPBKPKT512TO1023_DEFAULT 0x0
	/*[field] LPBKPKT512TO1023*/
	#define LPBKPKT512TO1023_LPBKPKT512TO1023
	#define LPBKPKT512TO1023_LPBKPKT512TO1023_OFFSET  0
	#define LPBKPKT512TO1023_LPBKPKT512TO1023_LEN     32
	#define LPBKPKT512TO1023_LPBKPKT512TO1023_DEFAULT 0x0
struct lpbkpkt512to1023 {
	a_uint32_t  lpbkpkt512to1023:32;
};
union lpbkpkt512to1023_u {
	a_uint32_t val;
	struct lpbkpkt512to1023 bf;
};

/*[register] LPBKPKT1024TO1518*/
#define LPBKPKT1024TO1518
#define LPBKPKT1024TO1518_ADDRESS 0x40
#define LPBKPKT1024TO1518_NUM     6
#define LPBKPKT1024TO1518_INC     0x200
#define LPBKPKT1024TO1518_TYPE    REG_TYPE_RO
#define LPBKPKT1024TO1518_DEFAULT 0x0
	/*[field] RXPKT1024TO1518*/
	#define LPBKPKT1024TO1518_LPBKPKT1024TO1518
	#define LPBKPKT1024TO1518_OFFSET  0
	#define LPBKPKT1024TO1518_LEN     32
	#define LPBKPKT1024TO1518_DEFAULT 0x0

struct lpbkpkt1024to1518 {
	a_uint32_t  lpbkpkt1024to1518:32;
};
union lpbkpkt1024to1518_u {
	a_uint32_t val;
	struct lpbkpkt1024to1518 bf;
};

/*[register] LPBKPKT1519TOX*/
#define LPBKPKT1519TOX
#define LPBKPKT1519TOX_ADDRESS 0x44
#define LPBKPKT1519TOX_NUM     6
#define LPBKPKT1519TOX_INC     0x200
#define LPBKPKT1519TOX_TYPE    REG_TYPE_RO
#define LPBKPKT1519TOX_DEFAULT 0x0
	/*[field] RXPKT1519TOX*/
	#define LPBKPKT1519TOX_LPBKPKT1519TOX
	#define LPBKPKT1519TOX_LPBKPKT1519TOX_OFFSET  0
	#define LPBKPKT1519TOX_LPBKPKT1519TOX_LEN     32
	#define LPBKPKT1519TOX_LPBKPKT1519TOX_DEFAULT 0x0
struct lpbkpkt1519tox {
	a_uint32_t  lpbkpkt1519tox:32;
};
union lpbkpkt1519tox_u {
	a_uint32_t val;
	struct lpbkpkt1519tox bf;
};

/*[register] LPBKPKTTOOLONG*/
#define LPBKPKTTOOLONG
#define LPBKPKTTOOLONG_ADDRESS 0x48
#define LPBKPKTTOOLONG_NUM     6
#define LPBKPKTTOOLONG_INC     0x200
#define LPBKPKTTOOLONG_TYPE    REG_TYPE_RO
#define LPBKPKTTOOLONG_DEFAULT 0x0
	/*[field] RXTOOLONG*/
	#define LPBKPKTTOOLONG_LPBKPKTTOOLONG
	#define LPBKPKTTOOLONG_LPBKPKTTOOLONG_OFFSET  0
	#define LPBKPKTTOOLONG_LPBKPKTTOOLONG_LEN     32
	#define LPBKPKTTOOLONG_LPBKPKTTOOLONG_DEFAULT 0x0
struct lpbkpkttoolong {
	a_uint32_t  lpbkpkttoolong:32;
};
union lpbkpkttoolong_u {
	a_uint32_t val;
	struct lpbkpkttoolong bf;
};

/*[register] LPBKBYTE_L*/
#define LPBKBYTE_L
#define LPBKBYTE_L_ADDRESS 0x4c
#define LPBKBYTE_L_NUM     6
#define LPBKBYTE_L_INC     0x200
#define LPBKBYTE_L_TYPE    REG_TYPE_RO
#define LPBKBYTE_L_DEFAULT 0x0
	/*[field] LPBKBYTE_L*/
	#define LPBKBYTE_L_LPBKBYTE_L
	#define LPBKBYTE_L_LPBKBYTE_L_OFFSET  0
	#define LPBKBYTE_L_LPBKBYTE_L_LEN     32
	#define LPBKBYTE_L_LPBKBYTE_L_DEFAULT 0x0
struct lpbkbyte_l {
	a_uint32_t  lpbkbyte_l:32;
};
union lpbkbyte_l_u {
	a_uint32_t val;
	struct lpbkbyte_l bf;
};

/*[register] LPBKBYTE_H*/
#define LPBKBYTE_H
#define LPBKBYTE_H_ADDRESS 0x50
#define LPBKBYTE_H_NUM     6
#define LPBKBYTE_H_INC     0x200
#define LPBKBYTE_H_TYPE    REG_TYPE_RO
#define LPBKBYTE_H_DEFAULT 0x0
	/*[field] LPBKBYTE_H*/
	#define LPBKBYTE_H_LPBKBYTE_H
	#define LPBKBYTE_H_LPBKBYTE_H_OFFSET  0
	#define LPBKBYTE_H_LPBKBYTE_H_LEN     32
	#define LPBKBYTE_H_LPBKBYTE_H_DEFAULT 0x0

struct lpbkbyte_h {
	a_uint32_t  lpbkbyte_h:32;
};
union lpbkbyte_h_u {
	a_uint32_t val;
	struct lpbkbyte_h bf;
};

/*[register] LPBKDROPCOUNTER*/
#define LPBKDROPCOUNTER
#define LPBKDROPCOUNTER_ADDRESS 0x54
#define LPBKDROPCOUNTER_NUM     6
#define LPBKDROPCOUNTER_INC     0x200
#define LPBKDROPCOUNTER_TYPE    REG_TYPE_RO
#define LPBKDROPCOUNTER_DEFAULT 0x0
	/*[field] LPBKDROPCOUNTER*/
	#define LPBKDROPCOUNTER_LPBKDROPCOUNTER
	#define LPBKDROPCOUNTER_OFFSET  0
	#define LPBKDROPCOUNTER_LEN     32
	#define LPBKDROPCOUNTER_DEFAULT 0x0
struct lpbkdropcounter {
	a_uint32_t  lpbkdropcounter:32;
};
union lpbkdropcounter_u {
	a_uint32_t val;
	struct lpbkdropcounter bf;
};

/*[register] LPBKTOOSHORT*/
#define LPBKPKTTOOSHORT
#define LPBKPKTTOOSHORT_ADDRESS 0x68
#define LPBKPKTTOOSHORT_NUM     6
#define LPBKPKTTOOSHORT_INC     0x200
#define LPBKPKTTOOSHORT_TYPE    REG_TYPE_RO
#define LPBKPKTTOOSHORT_DEFAULT 0x0
	/*[field] LPBKPKTTOOSHORT*/
	#define LPBKPKTTOOSHORT_LPBKPKTTOOSHORT
	#define LPBKPKTTOOSHORT_OFFSET  0
	#define LPBKPKTTOOSHORT_LEN     32
	#define LPBKPKTTOOSHORT_DEFAULT 0x0
struct lpbkpkttooshort {
	a_uint32_t  lpbkpkttooshort:32;
};
union lpbkpkttooshort_u {
	a_uint32_t val;
	struct lpbkpkttooshort bf;
};

/*[register] LPBKPKT14TO63*/
#define LPBKPKT14TO63
#define LPBKPKT14TO63_ADDRESS 0x6c
#define LPBKPKT14TO63_NUM     6
#define LPBKPKT14TO63_INC     0x200
#define LPBKPKT14TO63_TYPE    REG_TYPE_RO
#define LPBKPKT14TO63_DEFAULT 0x0
	/*[field] LPBKPKT14TO63*/
	#define LPBKPKT14TO63_LPBKPKT14TO63
	#define LPBKPKT14TO63_OFFSET  0
	#define LPBKPKT14TO63_LEN     32
	#define LPBKPKT14TO63_DEFAULT 0x0
struct lpbkpkt14to63 {
	a_uint32_t  lpbkpkt14to63:32;
};
union lpbkpkt14to63_u {
	a_uint32_t val;
	struct lpbkpkt14to63 bf;
};

/*[register] LPBKTOOLONGBYTE_L*/
#define LPBKTOOLONGBYTE_L
#define LPBKTOOLONGBYTE_L_ADDRESS 0x70
#define LPBKTOOLONGBYTE_L_NUM     6
#define LPBKTOOLONGBYTE_L_INC     0x200
#define LPBKTOOLONGBYTE_L_TYPE    REG_TYPE_RO
#define LPBKTOOLONGBYTE_L_DEFAULT 0x0
	/*[field] LPBKTOOLONGBYTE_L*/
	#define LPBKTOOLONGBYTE_L_LPBKTOOLONGBYTE_L
	#define LPBKTOOLONGBYTE_L_LPBKTOOLONGBYTE_L_OFFSET  0
	#define LPBKTOOLONGBYTE_L_LPBKTOOLONGBYTE_L_LEN     32
	#define LPBKTOOLONGBYTE_L_LPBKTOOLONGBYTE_L_DEFAULT 0x0
struct lpbktoolongbyte_l {
	a_uint32_t  lpbktoolongbyte_l:32;
};
union lpbktoolongbyte_l_u {
	a_uint32_t val;
	struct lpbktoolongbyte_l bf;
};

/*[register] LPBKTOOLONGBYTE_H*/
#define LPBKTOOLONGBYTE_H
#define LPBKTOOLONGBYTE_H_ADDRESS 0x74
#define LPBKTOOLONGBYTE_H_NUM     6
#define LPBKTOOLONGBYTE_H_INC     0x200
#define LPBKTOOLONGBYTE_H_TYPE    REG_TYPE_RO
#define LPBKTOOLONGBYTE_H_DEFAULT 0x0
	/*[field] LPBKTOOLONGBYTE_L*/
	#define LPBKTOOLONGBYTE_H_LPBKTOOLONGBYTE_H
	#define LPBKTOOLONGBYTE_H_LPBKTOOLONGBYTE_H_OFFSET  0
	#define LPBKTOOLONGBYTE_H_LPBKTOOLONGBYTE_H_LEN     32
	#define LPBKTOOLONGBYTE_H_LPBKTOOLONGBYTE_H_DEFAULT 0x0
struct lpbktoolongbyte_h {
	a_uint32_t  lpbktoolongbyte_h:32;
};
union lpbktoolongbyte_h_u {
	a_uint32_t val;
	struct lpbktoolongbyte_h bf;
};

/*[register] LPBKTOOSHORTBYTE_L*/
#define LPBKTOOSHORTBYTE_L
#define LPBKTOOSHORTBYTE_L_ADDRESS 0x78
#define LPBKTOOSHORTBYTE_L_NUM     6
#define LPBKTOOSHORTBYTE_L_INC     0x200
#define LPBKTOOLONGBYTE_L_TYPE    REG_TYPE_RO
#define LPBKTOOLONGBYTE_L_DEFAULT 0x0
	/*[field] LPBKTOOLONGBYTE_L*/
	#define LPBKTOOSHORTBYTE_L_LPBKTOOSHORTBYTE_L
	#define LPBKTOOSHORTBYTE_L_LPBKTOOSHORTBYTE_L_OFFSET  0
	#define LPBKTOOSHORTBYTE_L_LPBKTOOSHORTBYTE_L_LEN     32
	#define LPBKTOOSHORTBYTE_L_LPBKTOOSHORTBYTE_L_DEFAULT 0x0
struct lpbktooshortbyte_l {
	a_uint32_t  lpbktooshortbyte_l:32;
};
union lpbktooshortbyte_l_u {
	a_uint32_t val;
	struct lpbktooshortbyte_l bf;
};

/*[register] LPBKTOOSHORTBYTE_H*/
#define LPBKTOOSHORTBYTE_H
#define LPBKTOOSHORTBYTE_H_ADDRESS 0x7c
#define LPBKTOOSHORTBYTE_H_NUM     6
#define LPBKTOOSHORTBYTE_H_INC     0x200
#define LPBKTOOSHORTBYTE_H_TYPE    REG_TYPE_RO
#define LPBKTOOSHORTBYTE_H_DEFAULT 0x0
	/*[field] LPBKTOOLONGBYTE_L*/
	#define LPBKTOOSHORTBYTE_H_LPBKTOOSHORTBYTE_H
	#define LPBKTOOSHORTBYTE_H_OFFSET  0
	#define LPBKTOOSHORTBYTE_H_LEN     32
	#define LPBKTOOSHORTBYTE_H_DEFAULT 0x0
struct lpbktooshortbyte_h {
	a_uint32_t  lpbktooshortbyte_h:32;
};
union lpbktooshortbyte_h_u {
	a_uint32_t val;
	struct lpbktooshortbyte_h bf;
};
#endif
