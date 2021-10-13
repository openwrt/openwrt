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
#ifndef HPPE_MIB_REG_H
#define HPPE_MIB_REG_H

/*[register] MAC_MIB_CTRL*/
#define MAC_MIB_CTRL
#define MAC_MIB_CTRL_ADDRESS 0x34
#define MAC_MIB_CTRL_NUM     6
#define MAC_MIB_CTRL_INC     0x200
#define MAC_MIB_CTRL_TYPE    REG_TYPE_RW
#define MAC_MIB_CTRL_DEFAULT 0x0
	/*[field] MIB_EN*/
	#define MAC_MIB_CTRL_MIB_EN
	#define MAC_MIB_CTRL_MIB_EN_OFFSET  0
	#define MAC_MIB_CTRL_MIB_EN_LEN     1
	#define MAC_MIB_CTRL_MIB_EN_DEFAULT 0x0
	/*[field] MIB_RESET*/
	#define MAC_MIB_CTRL_MIB_RESET
	#define MAC_MIB_CTRL_MIB_RESET_OFFSET  1
	#define MAC_MIB_CTRL_MIB_RESET_LEN     1
	#define MAC_MIB_CTRL_MIB_RESET_DEFAULT 0x0
	/*[field] MIB_RD_CLR*/
	#define MAC_MIB_CTRL_MIB_RD_CLR
	#define MAC_MIB_CTRL_MIB_RD_CLR_OFFSET  2
	#define MAC_MIB_CTRL_MIB_RD_CLR_LEN     1
	#define MAC_MIB_CTRL_MIB_RD_CLR_DEFAULT 0x0

struct mac_mib_ctrl {
	a_uint32_t  mib_en:1;
	a_uint32_t  mib_reset:1;
	a_uint32_t  mib_rd_clr:1;
	a_uint32_t  _reserved0:29;
};

union mac_mib_ctrl_u {
	a_uint32_t val;
	struct mac_mib_ctrl bf;
};

/*[register] RXBROAD*/
#define RXBROAD
#define RXBROAD_ADDRESS 0x40
#define RXBROAD_NUM     6
#define RXBROAD_INC     0x200
#define RXBROAD_TYPE    REG_TYPE_RO
#define RXBROAD_DEFAULT 0x0
	/*[field] RXBROAD*/
	#define RXBROAD_RXBROAD
	#define RXBROAD_RXBROAD_OFFSET  0
	#define RXBROAD_RXBROAD_LEN     32
	#define RXBROAD_RXBROAD_DEFAULT 0x0

struct rxbroad {
	a_uint32_t  rxbroad:32;
};

union rxbroad_u {
	a_uint32_t val;
	struct rxbroad bf;
};

/*[register] RXPAUSE*/
#define RXPAUSE
#define RXPAUSE_ADDRESS 0x44
#define RXPAUSE_NUM     6
#define RXPAUSE_INC     0x200
#define RXPAUSE_TYPE    REG_TYPE_RO
#define RXPAUSE_DEFAULT 0x0
	/*[field] RXPAUSE*/
	#define RXPAUSE_RXPAUSE
	#define RXPAUSE_RXPAUSE_OFFSET  0
	#define RXPAUSE_RXPAUSE_LEN     32
	#define RXPAUSE_RXPAUSE_DEFAULT 0x0

struct rxpause {
	a_uint32_t  rxpause:32;
};

union rxpause_u {
	a_uint32_t val;
	struct rxpause bf;
};

/*[register] RXMULTI*/
#define RXMULTI
#define RXMULTI_ADDRESS 0x48
#define RXMULTI_NUM     6
#define RXMULTI_INC     0x200
#define RXMULTI_TYPE    REG_TYPE_RO
#define RXMULTI_DEFAULT 0x0
	/*[field] RXMULTI*/
	#define RXMULTI_RXMULTI
	#define RXMULTI_RXMULTI_OFFSET  0
	#define RXMULTI_RXMULTI_LEN     32
	#define RXMULTI_RXMULTI_DEFAULT 0x0

struct rxmulti {
	a_uint32_t  rxmulti:32;
};

union rxmulti_u {
	a_uint32_t val;
	struct rxmulti bf;
};

/*[register] RXFCSERR*/
#define RXFCSERR
#define RXFCSERR_ADDRESS 0x4c
#define RXFCSERR_NUM     6
#define RXFCSERR_INC     0x200
#define RXFCSERR_TYPE    REG_TYPE_RO
#define RXFCSERR_DEFAULT 0x0
	/*[field] RXFCSERR*/
	#define RXFCSERR_RXFCSERR
	#define RXFCSERR_RXFCSERR_OFFSET  0
	#define RXFCSERR_RXFCSERR_LEN     32
	#define RXFCSERR_RXFCSERR_DEFAULT 0x0

struct rxfcserr {
	a_uint32_t  rxfcserr:32;
};

union rxfcserr_u {
	a_uint32_t val;
	struct rxfcserr bf;
};

/*[register] RXALIGNERR*/
#define RXALIGNERR
#define RXALIGNERR_ADDRESS 0x50
#define RXALIGNERR_NUM     6
#define RXALIGNERR_INC     0x200
#define RXALIGNERR_TYPE    REG_TYPE_RO
#define RXALIGNERR_DEFAULT 0x0
	/*[field] RXALIGNERR*/
	#define RXALIGNERR_RXALIGNERR
	#define RXALIGNERR_RXALIGNERR_OFFSET  0
	#define RXALIGNERR_RXALIGNERR_LEN     32
	#define RXALIGNERR_RXALIGNERR_DEFAULT 0x0

struct rxalignerr {
	a_uint32_t  rxalignerr:32;
};

union rxalignerr_u {
	a_uint32_t val;
	struct rxalignerr bf;
};

/*[register] RXRUNT*/
#define RXRUNT
#define RXRUNT_ADDRESS 0x54
#define RXRUNT_NUM     6
#define RXRUNT_INC     0x200
#define RXRUNT_TYPE    REG_TYPE_RO
#define RXRUNT_DEFAULT 0x0
	/*[field] RXRUNT*/
	#define RXRUNT_RXRUNT
	#define RXRUNT_RXRUNT_OFFSET  0
	#define RXRUNT_RXRUNT_LEN     32
	#define RXRUNT_RXRUNT_DEFAULT 0x0

struct rxrunt {
	a_uint32_t  rxrunt:32;
};

union rxrunt_u {
	a_uint32_t val;
	struct rxrunt bf;
};

/*[register] RXFRAG*/
#define RXFRAG
#define RXFRAG_ADDRESS 0x58
#define RXFRAG_NUM     6
#define RXFRAG_INC     0x200
#define RXFRAG_TYPE    REG_TYPE_RO
#define RXFRAG_DEFAULT 0x0
	/*[field] RXFRAG*/
	#define RXFRAG_RXFRAG
	#define RXFRAG_RXFRAG_OFFSET  0
	#define RXFRAG_RXFRAG_LEN     32
	#define RXFRAG_RXFRAG_DEFAULT 0x0

struct rxfrag {
	a_uint32_t  rxfrag:32;
};

union rxfrag_u {
	a_uint32_t val;
	struct rxfrag bf;
};

/*[register] RXJUMBOFCSERR*/
#define RXJUMBOFCSERR
#define RXJUMBOFCSERR_ADDRESS 0x5c
#define RXJUMBOFCSERR_NUM     6
#define RXJUMBOFCSERR_INC     0x200
#define RXJUMBOFCSERR_TYPE    REG_TYPE_RO
#define RXJUMBOFCSERR_DEFAULT 0x0
	/*[field] RXJUMBOFCSERR*/
	#define RXJUMBOFCSERR_RXJUMBOFCSERR
	#define RXJUMBOFCSERR_RXJUMBOFCSERR_OFFSET  0
	#define RXJUMBOFCSERR_RXJUMBOFCSERR_LEN     32
	#define RXJUMBOFCSERR_RXJUMBOFCSERR_DEFAULT 0x0

struct rxjumbofcserr {
	a_uint32_t  rxjumbofcserr:32;
};

union rxjumbofcserr_u {
	a_uint32_t val;
	struct rxjumbofcserr bf;
};

/*[register] RXJUMBOALIGNERR*/
#define RXJUMBOALIGNERR
#define RXJUMBOALIGNERR_ADDRESS 0x60
#define RXJUMBOALIGNERR_NUM     6
#define RXJUMBOALIGNERR_INC     0x200
#define RXJUMBOALIGNERR_TYPE    REG_TYPE_RO
#define RXJUMBOALIGNERR_DEFAULT 0x0
	/*[field] RXJUMBOALIGNERR*/
	#define RXJUMBOALIGNERR_RXJUMBOALIGNERR
	#define RXJUMBOALIGNERR_RXJUMBOALIGNERR_OFFSET  0
	#define RXJUMBOALIGNERR_RXJUMBOALIGNERR_LEN     32
	#define RXJUMBOALIGNERR_RXJUMBOALIGNERR_DEFAULT 0x0

struct rxjumboalignerr {
	a_uint32_t  rxjumboalignerr:32;
};

union rxjumboalignerr_u {
	a_uint32_t val;
	struct rxjumboalignerr bf;
};

/*[register] RXPKT64*/
#define RXPKT64
#define RXPKT64_ADDRESS 0x64
#define RXPKT64_NUM     6
#define RXPKT64_INC     0x200
#define RXPKT64_TYPE    REG_TYPE_RO
#define RXPKT64_DEFAULT 0x0
	/*[field] RXPKT64*/
	#define RXPKT64_RXPKT64
	#define RXPKT64_RXPKT64_OFFSET  0
	#define RXPKT64_RXPKT64_LEN     32
	#define RXPKT64_RXPKT64_DEFAULT 0x0

struct rxpkt64 {
	a_uint32_t  rxpkt64:32;
};

union rxpkt64_u {
	a_uint32_t val;
	struct rxpkt64 bf;
};

/*[register] RXPKT65TO127*/
#define RXPKT65TO127
#define RXPKT65TO127_ADDRESS 0x68
#define RXPKT65TO127_NUM     6
#define RXPKT65TO127_INC     0x200
#define RXPKT65TO127_TYPE    REG_TYPE_RO
#define RXPKT65TO127_DEFAULT 0x0
	/*[field] RXPKT65TO127*/
	#define RXPKT65TO127_RXPKT65TO127
	#define RXPKT65TO127_RXPKT65TO127_OFFSET  0
	#define RXPKT65TO127_RXPKT65TO127_LEN     32
	#define RXPKT65TO127_RXPKT65TO127_DEFAULT 0x0

struct rxpkt65to127 {
	a_uint32_t  rxpkt65to127:32;
};

union rxpkt65to127_u {
	a_uint32_t val;
	struct rxpkt65to127 bf;
};

/*[register] RXPKT128TO255*/
#define RXPKT128TO255
#define RXPKT128TO255_ADDRESS 0x6c
#define RXPKT128TO255_NUM     6
#define RXPKT128TO255_INC     0x200
#define RXPKT128TO255_TYPE    REG_TYPE_RO
#define RXPKT128TO255_DEFAULT 0x0
	/*[field] RXPKT128TO255*/
	#define RXPKT128TO255_RXPKT128TO255
	#define RXPKT128TO255_RXPKT128TO255_OFFSET  0
	#define RXPKT128TO255_RXPKT128TO255_LEN     32
	#define RXPKT128TO255_RXPKT128TO255_DEFAULT 0x0

struct rxpkt128to255 {
	a_uint32_t  rxpkt128to255:32;
};

union rxpkt128to255_u {
	a_uint32_t val;
	struct rxpkt128to255 bf;
};

/*[register] RXPKT256TO511*/
#define RXPKT256TO511
#define RXPKT256TO511_ADDRESS 0x70
#define RXPKT256TO511_NUM     6
#define RXPKT256TO511_INC     0x200
#define RXPKT256TO511_TYPE    REG_TYPE_RO
#define RXPKT256TO511_DEFAULT 0x0
	/*[field] RXPKT256TO511*/
	#define RXPKT256TO511_RXPKT256TO511
	#define RXPKT256TO511_RXPKT256TO511_OFFSET  0
	#define RXPKT256TO511_RXPKT256TO511_LEN     32
	#define RXPKT256TO511_RXPKT256TO511_DEFAULT 0x0

struct rxpkt256to511 {
	a_uint32_t  rxpkt256to511:32;
};

union rxpkt256to511_u {
	a_uint32_t val;
	struct rxpkt256to511 bf;
};

/*[register] RXPKT512TO1023*/
#define RXPKT512TO1023
#define RXPKT512TO1023_ADDRESS 0x74
#define RXPKT512TO1023_NUM     6
#define RXPKT512TO1023_INC     0x200
#define RXPKT512TO1023_TYPE    REG_TYPE_RO
#define RXPKT512TO1023_DEFAULT 0x0
	/*[field] RXPKT512TO1023*/
	#define RXPKT512TO1023_RXPKT512TO1023
	#define RXPKT512TO1023_RXPKT512TO1023_OFFSET  0
	#define RXPKT512TO1023_RXPKT512TO1023_LEN     32
	#define RXPKT512TO1023_RXPKT512TO1023_DEFAULT 0x0

struct rxpkt512to1023 {
	a_uint32_t  rxpkt512to1023:32;
};

union rxpkt512to1023_u {
	a_uint32_t val;
	struct rxpkt512to1023 bf;
};

/*[register] RXPKT1024TO1518*/
#define RXPKT1024TO1518
#define RXPKT1024TO1518_ADDRESS 0x78
#define RXPKT1024TO1518_NUM     6
#define RXPKT1024TO1518_INC     0x200
#define RXPKT1024TO1518_TYPE    REG_TYPE_RO
#define RXPKT1024TO1518_DEFAULT 0x0
	/*[field] RXPKT1024TO1518*/
	#define RXPKT1024TO1518_RXPKT1024TO1518
	#define RXPKT1024TO1518_RXPKT1024TO1518_OFFSET  0
	#define RXPKT1024TO1518_RXPKT1024TO1518_LEN     32
	#define RXPKT1024TO1518_RXPKT1024TO1518_DEFAULT 0x0

struct rxpkt1024to1518 {
	a_uint32_t  rxpkt1024to1518:32;
};

union rxpkt1024to1518_u {
	a_uint32_t val;
	struct rxpkt1024to1518 bf;
};

/*[register] RXPKT1519TOX*/
#define RXPKT1519TOX
#define RXPKT1519TOX_ADDRESS 0x7c
#define RXPKT1519TOX_NUM     6
#define RXPKT1519TOX_INC     0x200
#define RXPKT1519TOX_TYPE    REG_TYPE_RO
#define RXPKT1519TOX_DEFAULT 0x0
	/*[field] RXPKT1519TOX*/
	#define RXPKT1519TOX_RXPKT1519TOX
	#define RXPKT1519TOX_RXPKT1519TOX_OFFSET  0
	#define RXPKT1519TOX_RXPKT1519TOX_LEN     32
	#define RXPKT1519TOX_RXPKT1519TOX_DEFAULT 0x0

struct rxpkt1519tox {
	a_uint32_t  rxpkt1519tox:32;
};

union rxpkt1519tox_u {
	a_uint32_t val;
	struct rxpkt1519tox bf;
};

/*[register] RXTOOLONG*/
#define RXTOOLONG
#define RXTOOLONG_ADDRESS 0x80
#define RXTOOLONG_NUM     6
#define RXTOOLONG_INC     0x200
#define RXTOOLONG_TYPE    REG_TYPE_RO
#define RXTOOLONG_DEFAULT 0x0
	/*[field] RXTOOLONG*/
	#define RXTOOLONG_RXTOOLONG
	#define RXTOOLONG_RXTOOLONG_OFFSET  0
	#define RXTOOLONG_RXTOOLONG_LEN     32
	#define RXTOOLONG_RXTOOLONG_DEFAULT 0x0

struct rxtoolong {
	a_uint32_t  rxtoolong:32;
};

union rxtoolong_u {
	a_uint32_t val;
	struct rxtoolong bf;
};

/*[register] RXGOODBYTE_L*/
#define RXGOODBYTE_L
#define RXGOODBYTE_L_ADDRESS 0x84
#define RXGOODBYTE_L_NUM     6
#define RXGOODBYTE_L_INC     0x200
#define RXGOODBYTE_L_TYPE    REG_TYPE_RO
#define RXGOODBYTE_L_DEFAULT 0x0
	/*[field] RXGOODBYTE_L*/
	#define RXGOODBYTE_L_RXGOODBYTE_L
	#define RXGOODBYTE_L_RXGOODBYTE_L_OFFSET  0
	#define RXGOODBYTE_L_RXGOODBYTE_L_LEN     32
	#define RXGOODBYTE_L_RXGOODBYTE_L_DEFAULT 0x0

struct rxgoodbyte_l {
	a_uint32_t  rxgoodbyte_l:32;
};

union rxgoodbyte_l_u {
	a_uint32_t val;
	struct rxgoodbyte_l bf;
};

/*[register] RXGOODBYTE_H*/
#define RXGOODBYTE_H
#define RXGOODBYTE_H_ADDRESS 0x88
#define RXGOODBYTE_H_NUM     6
#define RXGOODBYTE_H_INC     0x200
#define RXGOODBYTE_H_TYPE    REG_TYPE_RO
#define RXGOODBYTE_H_DEFAULT 0x0
	/*[field] RXGOODBYTE_H*/
	#define RXGOODBYTE_H_RXGOODBYTE_H
	#define RXGOODBYTE_H_RXGOODBYTE_H_OFFSET  0
	#define RXGOODBYTE_H_RXGOODBYTE_H_LEN     32
	#define RXGOODBYTE_H_RXGOODBYTE_H_DEFAULT 0x0

struct rxgoodbyte_h {
	a_uint32_t  rxgoodbyte_h:32;
};

union rxgoodbyte_h_u {
	a_uint32_t val;
	struct rxgoodbyte_h bf;
};

/*[register] RXBADBYTE_L*/
#define RXBADBYTE_L
#define RXBADBYTE_L_ADDRESS 0x8c
#define RXBADBYTE_L_NUM     6
#define RXBADBYTE_L_INC     0x200
#define RXBADBYTE_L_TYPE    REG_TYPE_RO
#define RXBADBYTE_L_DEFAULT 0x0
	/*[field] RXBADBYTE_L*/
	#define RXBADBYTE_L_RXBADBYTE_L
	#define RXBADBYTE_L_RXBADBYTE_L_OFFSET  0
	#define RXBADBYTE_L_RXBADBYTE_L_LEN     32
	#define RXBADBYTE_L_RXBADBYTE_L_DEFAULT 0x0

struct rxbadbyte_l {
	a_uint32_t  rxbadbyte_l:32;
};

union rxbadbyte_l_u {
	a_uint32_t val;
	struct rxbadbyte_l bf;
};

/*[register] RXBADBYTE_H*/
#define RXBADBYTE_H
#define RXBADBYTE_H_ADDRESS 0x90
#define RXBADBYTE_H_NUM     6
#define RXBADBYTE_H_INC     0x200
#define RXBADBYTE_H_TYPE    REG_TYPE_RO
#define RXBADBYTE_H_DEFAULT 0x0
	/*[field] RXBADBYTE_H*/
	#define RXBADBYTE_H_RXBADBYTE_H
	#define RXBADBYTE_H_RXBADBYTE_H_OFFSET  0
	#define RXBADBYTE_H_RXBADBYTE_H_LEN     32
	#define RXBADBYTE_H_RXBADBYTE_H_DEFAULT 0x0

struct rxbadbyte_h {
	a_uint32_t  rxbadbyte_h:32;
};

union rxbadbyte_h_u {
	a_uint32_t val;
	struct rxbadbyte_h bf;
};

/*[register] RXUNI*/
#define RXUNI
#define RXUNI_ADDRESS 0x94
#define RXUNI_NUM     6
#define RXUNI_INC     0x200
#define RXUNI_TYPE    REG_TYPE_RO
#define RXUNI_DEFAULT 0x0
	/*[field] RXUNI*/
	#define RXUNI_RXUNI
	#define RXUNI_RXUNI_OFFSET  0
	#define RXUNI_RXUNI_LEN     32
	#define RXUNI_RXUNI_DEFAULT 0x0

struct rxuni {
	a_uint32_t  rxuni:32;
};

union rxuni_u {
	a_uint32_t val;
	struct rxuni bf;
};

/*[register] TXBROAD*/
#define TXBROAD
#define TXBROAD_ADDRESS 0xa0
#define TXBROAD_NUM     6
#define TXBROAD_INC     0x200
#define TXBROAD_TYPE    REG_TYPE_RO
#define TXBROAD_DEFAULT 0x0
	/*[field] TXBROAD*/
	#define TXBROAD_TXBROAD
	#define TXBROAD_TXBROAD_OFFSET  0
	#define TXBROAD_TXBROAD_LEN     32
	#define TXBROAD_TXBROAD_DEFAULT 0x0

struct txbroad {
	a_uint32_t  txbroad:32;
};

union txbroad_u {
	a_uint32_t val;
	struct txbroad bf;
};

/*[register] TXPAUSE*/
#define TXPAUSE
#define TXPAUSE_ADDRESS 0xa4
#define TXPAUSE_NUM     6
#define TXPAUSE_INC     0x200
#define TXPAUSE_TYPE    REG_TYPE_RO
#define TXPAUSE_DEFAULT 0x0
	/*[field] TXPAUSE*/
	#define TXPAUSE_TXPAUSE
	#define TXPAUSE_TXPAUSE_OFFSET  0
	#define TXPAUSE_TXPAUSE_LEN     32
	#define TXPAUSE_TXPAUSE_DEFAULT 0x0

struct txpause {
	a_uint32_t  txpause:32;
};

union txpause_u {
	a_uint32_t val;
	struct txpause bf;
};

/*[register] TXMULTI*/
#define TXMULTI
#define TXMULTI_ADDRESS 0xa8
#define TXMULTI_NUM     6
#define TXMULTI_INC     0x200
#define TXMULTI_TYPE    REG_TYPE_RO
#define TXMULTI_DEFAULT 0x0
	/*[field] TXMULTI*/
	#define TXMULTI_TXMULTI
	#define TXMULTI_TXMULTI_OFFSET  0
	#define TXMULTI_TXMULTI_LEN     32
	#define TXMULTI_TXMULTI_DEFAULT 0x0

struct txmulti {
	a_uint32_t  txmulti:32;
};

union txmulti_u {
	a_uint32_t val;
	struct txmulti bf;
};

/*[register] TXUNDERRUN*/
#define TXUNDERRUN
#define TXUNDERRUN_ADDRESS 0xac
#define TXUNDERRUN_NUM     6
#define TXUNDERRUN_INC     0x200
#define TXUNDERRUN_TYPE    REG_TYPE_RO
#define TXUNDERRUN_DEFAULT 0x0
	/*[field] TXUNDERRUN*/
	#define TXUNDERRUN_TXUNDERRUN
	#define TXUNDERRUN_TXUNDERRUN_OFFSET  0
	#define TXUNDERRUN_TXUNDERRUN_LEN     32
	#define TXUNDERRUN_TXUNDERRUN_DEFAULT 0x0

struct txunderrun {
	a_uint32_t  txunderrun:32;
};

union txunderrun_u {
	a_uint32_t val;
	struct txunderrun bf;
};

/*[register] TXPKT64*/
#define TXPKT64
#define TXPKT64_ADDRESS 0xb0
#define TXPKT64_NUM     6
#define TXPKT64_INC     0x200
#define TXPKT64_TYPE    REG_TYPE_RO
#define TXPKT64_DEFAULT 0x0
	/*[field] TXPKT64*/
	#define TXPKT64_TXPKT64
	#define TXPKT64_TXPKT64_OFFSET  0
	#define TXPKT64_TXPKT64_LEN     32
	#define TXPKT64_TXPKT64_DEFAULT 0x0

struct txpkt64 {
	a_uint32_t  txpkt64:32;
};

union txpkt64_u {
	a_uint32_t val;
	struct txpkt64 bf;
};

/*[register] TXPKT65TO127*/
#define TXPKT65TO127
#define TXPKT65TO127_ADDRESS 0xb4
#define TXPKT65TO127_NUM     6
#define TXPKT65TO127_INC     0x200
#define TXPKT65TO127_TYPE    REG_TYPE_RO
#define TXPKT65TO127_DEFAULT 0x0
	/*[field] TXPKT65TO127*/
	#define TXPKT65TO127_TXPKT65TO127
	#define TXPKT65TO127_TXPKT65TO127_OFFSET  0
	#define TXPKT65TO127_TXPKT65TO127_LEN     32
	#define TXPKT65TO127_TXPKT65TO127_DEFAULT 0x0

struct txpkt65to127 {
	a_uint32_t  txpkt65to127:32;
};

union txpkt65to127_u {
	a_uint32_t val;
	struct txpkt65to127 bf;
};

/*[register] TXPKT128TO255*/
#define TXPKT128TO255
#define TXPKT128TO255_ADDRESS 0xb8
#define TXPKT128TO255_NUM     6
#define TXPKT128TO255_INC     0x200
#define TXPKT128TO255_TYPE    REG_TYPE_RO
#define TXPKT128TO255_DEFAULT 0x0
	/*[field] TXPKT128TO255*/
	#define TXPKT128TO255_TXPKT128TO255
	#define TXPKT128TO255_TXPKT128TO255_OFFSET  0
	#define TXPKT128TO255_TXPKT128TO255_LEN     32
	#define TXPKT128TO255_TXPKT128TO255_DEFAULT 0x0

struct txpkt128to255 {
	a_uint32_t  txpkt128to255:32;
};

union txpkt128to255_u {
	a_uint32_t val;
	struct txpkt128to255 bf;
};

/*[register] TXPKT256TO511*/
#define TXPKT256TO511
#define TXPKT256TO511_ADDRESS 0xbc
#define TXPKT256TO511_NUM     6
#define TXPKT256TO511_INC     0x200
#define TXPKT256TO511_TYPE    REG_TYPE_RO
#define TXPKT256TO511_DEFAULT 0x0
	/*[field] TXPKT256TO511*/
	#define TXPKT256TO511_TXPKT256TO511
	#define TXPKT256TO511_TXPKT256TO511_OFFSET  0
	#define TXPKT256TO511_TXPKT256TO511_LEN     32
	#define TXPKT256TO511_TXPKT256TO511_DEFAULT 0x0

struct txpkt256to511 {
	a_uint32_t  txpkt256to511:32;
};

union txpkt256to511_u {
	a_uint32_t val;
	struct txpkt256to511 bf;
};

/*[register] TXPKT512TO1023*/
#define TXPKT512TO1023
#define TXPKT512TO1023_ADDRESS 0xc0
#define TXPKT512TO1023_NUM     6
#define TXPKT512TO1023_INC     0x200
#define TXPKT512TO1023_TYPE    REG_TYPE_RO
#define TXPKT512TO1023_DEFAULT 0x0
	/*[field] TXPKT512TO1023*/
	#define TXPKT512TO1023_TXPKT512TO1023
	#define TXPKT512TO1023_TXPKT512TO1023_OFFSET  0
	#define TXPKT512TO1023_TXPKT512TO1023_LEN     32
	#define TXPKT512TO1023_TXPKT512TO1023_DEFAULT 0x0

struct txpkt512to1023 {
	a_uint32_t  txpkt512to1023:32;
};

union txpkt512to1023_u {
	a_uint32_t val;
	struct txpkt512to1023 bf;
};

/*[register] TXPKT1024TO1518*/
#define TXPKT1024TO1518
#define TXPKT1024TO1518_ADDRESS 0xc4
#define TXPKT1024TO1518_NUM     6
#define TXPKT1024TO1518_INC     0x200
#define TXPKT1024TO1518_TYPE    REG_TYPE_RO
#define TXPKT1024TO1518_DEFAULT 0x0
	/*[field] TXPKT1024TO1518*/
	#define TXPKT1024TO1518_TXPKT1024TO1518
	#define TXPKT1024TO1518_TXPKT1024TO1518_OFFSET  0
	#define TXPKT1024TO1518_TXPKT1024TO1518_LEN     32
	#define TXPKT1024TO1518_TXPKT1024TO1518_DEFAULT 0x0

struct txpkt1024to1518 {
	a_uint32_t  txpkt1024to1518:32;
};

union txpkt1024to1518_u {
	a_uint32_t val;
	struct txpkt1024to1518 bf;
};

/*[register] TXPKT1519TOX*/
#define TXPKT1519TOX
#define TXPKT1519TOX_ADDRESS 0xc8
#define TXPKT1519TOX_NUM     6
#define TXPKT1519TOX_INC     0x200
#define TXPKT1519TOX_TYPE    REG_TYPE_RO
#define TXPKT1519TOX_DEFAULT 0x0
	/*[field] TXPKT1519TOX*/
	#define TXPKT1519TOX_TXPKT1519TOX
	#define TXPKT1519TOX_TXPKT1519TOX_OFFSET  0
	#define TXPKT1519TOX_TXPKT1519TOX_LEN     32
	#define TXPKT1519TOX_TXPKT1519TOX_DEFAULT 0x0

struct txpkt1519tox {
	a_uint32_t  txpkt1519tox:32;
};

union txpkt1519tox_u {
	a_uint32_t val;
	struct txpkt1519tox bf;
};

/*[register] TXBYTE_L*/
#define TXBYTE_L
#define TXBYTE_L_ADDRESS 0xcc
#define TXBYTE_L_NUM     6
#define TXBYTE_L_INC     0x200
#define TXBYTE_L_TYPE    REG_TYPE_RO
#define TXBYTE_L_DEFAULT 0x0
	/*[field] TXBYTE_L*/
	#define TXBYTE_L_TXBYTE_L
	#define TXBYTE_L_TXBYTE_L_OFFSET  0
	#define TXBYTE_L_TXBYTE_L_LEN     32
	#define TXBYTE_L_TXBYTE_L_DEFAULT 0x0

struct txbyte_l {
	a_uint32_t  txbyte_l:32;
};

union txbyte_l_u {
	a_uint32_t val;
	struct txbyte_l bf;
};

/*[register] TXBYTE_H*/
#define TXBYTE_H
#define TXBYTE_H_ADDRESS 0xd0
#define TXBYTE_H_NUM     6
#define TXBYTE_H_INC     0x200
#define TXBYTE_H_TYPE    REG_TYPE_RO
#define TXBYTE_H_DEFAULT 0x0
	/*[field] TXBYTE_H*/
	#define TXBYTE_H_TXBYTE_H
	#define TXBYTE_H_TXBYTE_H_OFFSET  0
	#define TXBYTE_H_TXBYTE_H_LEN     32
	#define TXBYTE_H_TXBYTE_H_DEFAULT 0x0

struct txbyte_h {
	a_uint32_t  txbyte_h:32;
};

union txbyte_h_u {
	a_uint32_t val;
	struct txbyte_h bf;
};

/*[register] TXCOLLISIONS*/
#define TXCOLLISIONS
#define TXCOLLISIONS_ADDRESS 0xd4
#define TXCOLLISIONS_NUM     6
#define TXCOLLISIONS_INC     0x200
#define TXCOLLISIONS_TYPE    REG_TYPE_RO
#define TXCOLLISIONS_DEFAULT 0x0
	/*[field] TXCOLLISIONS*/
	#define TXCOLLISIONS_TXCOLLISIONS
	#define TXCOLLISIONS_TXCOLLISIONS_OFFSET  0
	#define TXCOLLISIONS_TXCOLLISIONS_LEN     32
	#define TXCOLLISIONS_TXCOLLISIONS_DEFAULT 0x0

struct txcollisions {
	a_uint32_t  txcollisions:32;
};

union txcollisions_u {
	a_uint32_t val;
	struct txcollisions bf;
};

/*[register] TXABORTCOL*/
#define TXABORTCOL
#define TXABORTCOL_ADDRESS 0xd8
#define TXABORTCOL_NUM     6
#define TXABORTCOL_INC     0x200
#define TXABORTCOL_TYPE    REG_TYPE_RO
#define TXABORTCOL_DEFAULT 0x0
	/*[field] TXABORTCOL*/
	#define TXABORTCOL_TXABORTCOL
	#define TXABORTCOL_TXABORTCOL_OFFSET  0
	#define TXABORTCOL_TXABORTCOL_LEN     32
	#define TXABORTCOL_TXABORTCOL_DEFAULT 0x0

struct txabortcol {
	a_uint32_t  txabortcol:32;
};

union txabortcol_u {
	a_uint32_t val;
	struct txabortcol bf;
};

/*[register] TXMULTICOL*/
#define TXMULTICOL
#define TXMULTICOL_ADDRESS 0xdc
#define TXMULTICOL_NUM     6
#define TXMULTICOL_INC     0x200
#define TXMULTICOL_TYPE    REG_TYPE_RO
#define TXMULTICOL_DEFAULT 0x0
	/*[field] TXMULTICOL*/
	#define TXMULTICOL_TXMULTICOL
	#define TXMULTICOL_TXMULTICOL_OFFSET  0
	#define TXMULTICOL_TXMULTICOL_LEN     32
	#define TXMULTICOL_TXMULTICOL_DEFAULT 0x0

struct txmulticol {
	a_uint32_t  txmulticol:32;
};

union txmulticol_u {
	a_uint32_t val;
	struct txmulticol bf;
};

/*[register] TXSINGLECOL*/
#define TXSINGLECOL
#define TXSINGLECOL_ADDRESS 0xe0
#define TXSINGLECOL_NUM     6
#define TXSINGLECOL_INC     0x200
#define TXSINGLECOL_TYPE    REG_TYPE_RO
#define TXSINGLECOL_DEFAULT 0x0
	/*[field] TXSINGLECOL*/
	#define TXSINGLECOL_TXSINGLECOL
	#define TXSINGLECOL_TXSINGLECOL_OFFSET  0
	#define TXSINGLECOL_TXSINGLECOL_LEN     32
	#define TXSINGLECOL_TXSINGLECOL_DEFAULT 0x0

struct txsinglecol {
	a_uint32_t  txsinglecol:32;
};

union txsinglecol_u {
	a_uint32_t val;
	struct txsinglecol bf;
};

/*[register] TXEXCESSIVEDEFER*/
#define TXEXCESSIVEDEFER
#define TXEXCESSIVEDEFER_ADDRESS 0xe4
#define TXEXCESSIVEDEFER_NUM     6
#define TXEXCESSIVEDEFER_INC     0x200
#define TXEXCESSIVEDEFER_TYPE    REG_TYPE_RO
#define TXEXCESSIVEDEFER_DEFAULT 0x0
	/*[field] TXEXCESSIVEDEFER*/
	#define TXEXCESSIVEDEFER_TXEXCESSIVEDEFER
	#define TXEXCESSIVEDEFER_TXEXCESSIVEDEFER_OFFSET  0
	#define TXEXCESSIVEDEFER_TXEXCESSIVEDEFER_LEN     32
	#define TXEXCESSIVEDEFER_TXEXCESSIVEDEFER_DEFAULT 0x0

struct txexcessivedefer {
	a_uint32_t  txexcessivedefer:32;
};

union txexcessivedefer_u {
	a_uint32_t val;
	struct txexcessivedefer bf;
};

/*[register] TXDEFER*/
#define TXDEFER
#define TXDEFER_ADDRESS 0xe8
#define TXDEFER_NUM     6
#define TXDEFER_INC     0x200
#define TXDEFER_TYPE    REG_TYPE_RO
#define TXDEFER_DEFAULT 0x0
	/*[field] TXDEFER*/
	#define TXDEFER_TXDEFER
	#define TXDEFER_TXDEFER_OFFSET  0
	#define TXDEFER_TXDEFER_LEN     32
	#define TXDEFER_TXDEFER_DEFAULT 0x0

struct txdefer {
	a_uint32_t  txdefer:32;
};

union txdefer_u {
	a_uint32_t val;
	struct txdefer bf;
};

/*[register] TXLATECOL*/
#define TXLATECOL
#define TXLATECOL_ADDRESS 0xec
#define TXLATECOL_NUM     6
#define TXLATECOL_INC     0x200
#define TXLATECOL_TYPE    REG_TYPE_RO
#define TXLATECOL_DEFAULT 0x0
	/*[field] TXLATECOL*/
	#define TXLATECOL_TXLATECOL
	#define TXLATECOL_TXLATECOL_OFFSET  0
	#define TXLATECOL_TXLATECOL_LEN     32
	#define TXLATECOL_TXLATECOL_DEFAULT 0x0

struct txlatecol {
	a_uint32_t  txlatecol:32;
};

union txlatecol_u {
	a_uint32_t val;
	struct txlatecol bf;
};

/*[register] TXUNI*/
#define TXUNI
#define TXUNI_ADDRESS 0xf0
#define TXUNI_NUM     6
#define TXUNI_INC     0x200
#define TXUNI_TYPE    REG_TYPE_RO
#define TXUNI_DEFAULT 0x0
	/*[field] TXUNI*/
	#define TXUNI_TXUNI
	#define TXUNI_TXUNI_OFFSET  0
	#define TXUNI_TXUNI_LEN     32
	#define TXUNI_TXUNI_DEFAULT 0x0

struct txuni {
	a_uint32_t  txuni:32;
};

union txuni_u {
	a_uint32_t val;
	struct txuni bf;
};



#endif
