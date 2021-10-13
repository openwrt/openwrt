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
#ifndef HPPE_PPPOE_REG_H
#define HPPE_PPPOE_REG_H


/*[register] PPPOE_SESSION*/
#define PPPOE_SESSION
#define PPPOE_SESSION_ADDRESS 0xc20
#define PPPOE_SESSION_NUM     16
#define PPPOE_SESSION_INC     0x4
#define PPPOE_SESSION_TYPE    REG_TYPE_RW
#define PPPOE_SESSION_DEFAULT 0x0
	/*[field] SESSION_ID*/
	#define PPPOE_SESSION_SESSION_ID
	#define PPPOE_SESSION_SESSION_ID_OFFSET  0
	#define PPPOE_SESSION_SESSION_ID_LEN     16
	#define PPPOE_SESSION_SESSION_ID_DEFAULT 0x0
	/*[field] PORT_BITMAP*/
	#define PPPOE_SESSION_PORT_BITMAP
	#define PPPOE_SESSION_PORT_BITMAP_OFFSET  16
	#define PPPOE_SESSION_PORT_BITMAP_LEN     8
	#define PPPOE_SESSION_PORT_BITMAP_DEFAULT 0x0
	/*[field] L3_IF_INDEX*/
	#define PPPOE_SESSION_L3_IF_INDEX
	#define PPPOE_SESSION_L3_IF_INDEX_OFFSET  24
	#define PPPOE_SESSION_L3_IF_INDEX_LEN     8
	#define PPPOE_SESSION_L3_IF_INDEX_DEFAULT 0x0

struct pppoe_session {
	a_uint32_t  session_id:16;
	a_uint32_t  port_bitmap:8;
	a_uint32_t  l3_if_index:8;
};

union pppoe_session_u {
	a_uint32_t val;
	struct pppoe_session bf;
};

/*[register] PPPOE_SESSION_EXT*/
#define PPPOE_SESSION_EXT
#define PPPOE_SESSION_EXT_ADDRESS 0xc60
#define PPPOE_SESSION_EXT_NUM     16
#define PPPOE_SESSION_EXT_INC     0x4
#define PPPOE_SESSION_EXT_TYPE    REG_TYPE_RW
#define PPPOE_SESSION_EXT_DEFAULT 0x0
	/*[field] L3_IF_VALID*/
	#define PPPOE_SESSION_EXT_L3_IF_VALID
	#define PPPOE_SESSION_EXT_L3_IF_VALID_OFFSET  0
	#define PPPOE_SESSION_EXT_L3_IF_VALID_LEN     1
	#define PPPOE_SESSION_EXT_L3_IF_VALID_DEFAULT 0x0
	/*[field] MC_VALID*/
	#define PPPOE_SESSION_EXT_MC_VALID
	#define PPPOE_SESSION_EXT_MC_VALID_OFFSET  1
	#define PPPOE_SESSION_EXT_MC_VALID_LEN     1
	#define PPPOE_SESSION_EXT_MC_VALID_DEFAULT 0x0
	/*[field] UC_VALID*/
	#define PPPOE_SESSION_EXT_UC_VALID
	#define PPPOE_SESSION_EXT_UC_VALID_OFFSET  2
	#define PPPOE_SESSION_EXT_UC_VALID_LEN     1
	#define PPPOE_SESSION_EXT_UC_VALID_DEFAULT 0x0
	/*[field] SMAC_VALID*/
	#define PPPOE_SESSION_EXT_SMAC_VALID
	#define PPPOE_SESSION_EXT_SMAC_VALID_OFFSET  3
	#define PPPOE_SESSION_EXT_SMAC_VALID_LEN     1
	#define PPPOE_SESSION_EXT_SMAC_VALID_DEFAULT 0x0
	/*[field] SMAC*/
	#define PPPOE_SESSION_EXT_SMAC
	#define PPPOE_SESSION_EXT_SMAC_OFFSET  16
	#define PPPOE_SESSION_EXT_SMAC_LEN     16
	#define PPPOE_SESSION_EXT_SMAC_DEFAULT 0x0

struct pppoe_session_ext {
	a_uint32_t  l3_if_valid:1;
	a_uint32_t  mc_valid:1;
	a_uint32_t  uc_valid:1;
	a_uint32_t  smac_valid:1;
	a_uint32_t  _reserved0:12;
	a_uint32_t  smac:16;
};

union pppoe_session_ext_u {
	a_uint32_t val;
	struct pppoe_session_ext bf;
};

/*[register] PPPOE_SESSION_EXT1*/
#define PPPOE_SESSION_EXT1
#define PPPOE_SESSION_EXT1_ADDRESS 0xca0
#define PPPOE_SESSION_EXT1_NUM     16
#define PPPOE_SESSION_EXT1_INC     0x4
#define PPPOE_SESSION_EXT1_TYPE    REG_TYPE_RW
#define PPPOE_SESSION_EXT1_DEFAULT 0x0
	/*[field] SMAC*/
	#define PPPOE_SESSION_EXT1_SMAC
	#define PPPOE_SESSION_EXT1_SMAC_OFFSET  0
	#define PPPOE_SESSION_EXT1_SMAC_LEN     32
	#define PPPOE_SESSION_EXT1_SMAC_DEFAULT 0x0

struct pppoe_session_ext1 {
	a_uint32_t  smac:32;
};

union pppoe_session_ext1_u {
	a_uint32_t val;
	struct pppoe_session_ext1 bf;
};

#endif
