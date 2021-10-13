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
#ifndef HPPE_MIRROR_REG_H
#define HPPE_MIRROR_REG_H

/*[register] MIRROR_ANALYZER*/
#define MIRROR_ANALYZER
#define MIRROR_ANALYZER_ADDRESS 0x40
#define MIRROR_ANALYZER_NUM     1
#define MIRROR_ANALYZER_INC     0x4
#define MIRROR_ANALYZER_TYPE    REG_TYPE_RW
#define MIRROR_ANALYZER_DEFAULT 0x0
	/*[field] IN_ANALYZER_PORT*/
	#define MIRROR_ANALYZER_IN_ANALYZER_PORT
	#define MIRROR_ANALYZER_IN_ANALYZER_PORT_OFFSET  0
	#define MIRROR_ANALYZER_IN_ANALYZER_PORT_LEN     6
	#define MIRROR_ANALYZER_IN_ANALYZER_PORT_DEFAULT 0x0
	/*[field] EG_ANALYZER_PORT*/
	#define MIRROR_ANALYZER_EG_ANALYZER_PORT
	#define MIRROR_ANALYZER_EG_ANALYZER_PORT_OFFSET  8
	#define MIRROR_ANALYZER_EG_ANALYZER_PORT_LEN     6
	#define MIRROR_ANALYZER_EG_ANALYZER_PORT_DEFAULT 0x0

struct mirror_analyzer {
	a_uint32_t  in_analyzer_port:6;
	a_uint32_t  _reserved0:2;
	a_uint32_t  eg_analyzer_port:6;
	a_uint32_t  _reserved1:18;
};

union mirror_analyzer_u {
	a_uint32_t val;
	struct mirror_analyzer bf;
};

/*[register] PORT_MIRROR*/
#define PORT_MIRROR
#define PORT_MIRROR_ADDRESS 0x800
#define PORT_MIRROR_NUM     8
#define PORT_MIRROR_INC     0x4
#define PORT_MIRROR_TYPE    REG_TYPE_RW
#define PORT_MIRROR_DEFAULT 0x0
	/*[field] IN_MIRR_EN*/
	#define PORT_MIRROR_IN_MIRR_EN
	#define PORT_MIRROR_IN_MIRR_EN_OFFSET  0
	#define PORT_MIRROR_IN_MIRR_EN_LEN     1
	#define PORT_MIRROR_IN_MIRR_EN_DEFAULT 0x0
	/*[field] EG_MIRR_EN*/
	#define PORT_MIRROR_EG_MIRR_EN
	#define PORT_MIRROR_EG_MIRR_EN_OFFSET  1
	#define PORT_MIRROR_EG_MIRR_EN_LEN     1
	#define PORT_MIRROR_EG_MIRR_EN_DEFAULT 0x0

struct port_mirror {
	a_uint32_t  in_mirr_en:1;
	a_uint32_t  eg_mirr_en:1;
	a_uint32_t  _reserved0:30;
};

union port_mirror_u {
	a_uint32_t val;
	struct port_mirror bf;
};


#endif
