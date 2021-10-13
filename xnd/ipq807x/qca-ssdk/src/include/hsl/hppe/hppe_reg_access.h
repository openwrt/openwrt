/*
 * Copyright (c) 2014-2018, The Linux Foundation. All rights reserved.
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



#ifndef _HPPE_REG_ACCESS_H_
#define _HPPE_REG_ACCESS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "sw.h"

#define EDMA_CSR_BASE_ADDR           0xb00000
#define IPE_L3_BASE_ADDR             0x200000
#define QUEUE_MANAGER_BASE_ADDR      0x800000
#define TRAFFIC_MANAGER_BASE_ADDR    0x400000
#define INGRESS_POLICER_BASE_ADDR    0x100000
#define INGRESS_VLAN_BASE_ADDR       0x00f000
#define IPE_L2_BASE_ADDR             0x060000
#define IPO_CSR_BASE_ADDR            0x0b0000
#define IPR_CSR_BASE_ADDR            0x002000
#define NSS_MAC_CSR_BASE_ADDR        0x001000
#define NSS_PRX_CSR_BASE_ADDR        0x00b000
#define NSS_PTX_CSR_BASE_ADDR        0x020000
#define NSS_BM_CSR_BASE_ADDR         0x600000
#define NSS_XGMAC_CSR_BASE_ADDR      0x003000
#define NSS_GLOBAL_BASE_ADDR    	 0x0
#define NSS_UNIPHY_BASE_ADDR    0x0
#define NSS_LPI_BASE_ADDR    0x400

sw_error_t hppe_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t *val);
sw_error_t hppe_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t val);
sw_error_t hppe_reg_tbl_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t *val, a_uint32_t num);
sw_error_t hppe_reg_tbl_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t *val, a_uint32_t num);
sw_error_t hppe_uniphy_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t index, a_uint32_t *val);
sw_error_t hppe_uniphy_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t index, a_uint32_t val);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HPPE_REG_ACCESS_H_ */
