/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _MAC_AX_TYPE_H_
#define _MAC_AX_TYPE_H_

#include "mac_def.h"
#include "mac_reg.h"
#include "mac_hw_info.h"
#include "txdesc.h"
#include "rxdesc.h"
#include "chip_cfg.h"

#include "mac_ax/mac_ax_dfs.h"
#include "mac_ax/mac_ax_mac_info.h"
#include "mac_ax/eco_patch_check.h"

#if MAC_AX_FEATURE_DBGPKG
#include "mac_ax/dbgpkg.h"
#if MAC_AX_8852A_SUPPORT
#include "mac_ax/mac_8852a/dbgpkg_8852a.h"
#endif
#if MAC_AX_8852B_SUPPORT
#include "mac_ax/mac_8852b/dbgpkg_8852b.h"
#endif
#if MAC_AX_8852C_SUPPORT
#include "mac_ax/mac_8852c/dbgpkg_8852c.h"
#endif
#if MAC_AX_8192XB_SUPPORT
#include "mac_ax/mac_8192xb/dbgpkg_8192xb.h"
#endif
#if MAC_AX_8851B_SUPPORT
#include "mac_ax/mac_8851b/dbgpkg_8851b.h"
#endif
#if MAC_AX_8851E_SUPPORT
#include "mac_ax/mac_8851e/dbgpkg_8851e.h"
#endif
#if MAC_AX_8852D_SUPPORT
#include "mac_ax/mac_8852d/dbgpkg_8852d.h"
#endif
#if MAC_AX_1115E_SUPPORT
#include "mac_ax/mac_1115e/dbgpkg_1115e.h"
#endif
#endif
#if MAC_AX_FEATURE_HV
#include "hv_type.h"
#endif

/*--------------------Define -------------------------------------------*/

#ifdef CONFIG_NEW_HALMAC_INTERFACE
#define PLTFM_SDIO_CMD52_R8(addr)                                              \
	hal_sdio_cmd52_r8(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R8(addr)                                              \
	hal_sdio_cmd53_r8(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R16(addr)                                             \
	hal_sdio_cmd53_r16(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R32(addr)                                             \
	hal_sdio_cmd53_r32(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_RN(addr, size, val)                                   \
	hal_sdio_cmd53_rn(adapter->drv_adapter, addr, size, val)
#define PLTFM_SDIO_CMD52_W8(addr, val)                                         \
	hal_sdio_cmd52_w8(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_W8(addr, val)                                         \
	hal_sdio_cmd53_w8(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_WN(addr, size, val)                                   \
	hal_sdio_cmd53_wn(adapter->drv_adapter, addr, size, val)
#define PLTFM_SDIO_CMD53_W16(addr, val)                                        \
	hal_sdio_cmd53_w16(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_W32(addr, val)                                        \
	hal_sdio_cmd53_w32(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD52_CIA_R8(addr)                                          \
	hal_sdio_read_cia_r8(adapter->drv_adapter, addr)

#define PLTFM_TX(buf, len)                                                     \
	hal_tx(adapter->drv_adapter, buf, len)

#define PLTFM_FREE(buf, size)                                                  \
	hal_mem_free(adapter->drv_adapter, buf, size)
#define PLTFM_MALLOC(size)                                                     \
	hal_mem_alloc(adapter->drv_adapter, size)
#define PLTFM_MEMCPY(dest, src, size)                                          \
	hal_mem_cpy(adapter->drv_adapter, dest, src, size)
#define PLTFM_MEMSET(addr, value, size)                                        \
	hal_mem_set(adapter->drv_adapter, addr, value, size)
#define PLTFM_MEMCMP(ptr1, ptr2, num)                                          \
	hal_mem_cmp(adapter->drv_adapter, ptr1, ptr2, num)

#define PLTFM_DELAY_US(us)                                                     \
	hal_udelay(adapter->drv_adapter, us)
#define PLTFM_DELAY_MS(ms)                                                     \
	hal_mdelay(adapter->drv_adapter, ms)
#define PLTFM_MUTEX_INIT(mutex)                                                \
	hal_mutex_init(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_DEINIT(mutex)                                              \
	hal_mutex_deinit(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_LOCK(mutex)                                                \
	hal_mutex_lock(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_UNLOCK(mutex)                                              \
	hal_mutex_unlock(adapter->drv_adapter, mutex)

#define PLTFM_MSG_PRINT(...)	\
	hal_mac_msg_print(drv_adapter, __VA_ARGS__)

#define adapter_to_mac_ops(adapter) ((struct mac_ax_ops *)((adapter)->ops))
#define adapter_to_intf_ops(adapter)                                           \
	((struct mac_ax_intf_ops *)((adapter)->ops->intf_ops))

#define PLTFM_REG_R8(addr)                                                     \
	hal_read8(adapter->drv_adapter, addr)
#define PLTFM_REG_R16(addr)                                                    \
	hal_read16(adapter->drv_adapter, addr)
#define PLTFM_REG_R32(addr)                                                    \
	hal_read32(adapter->drv_adapter, addr)
#define PLTFM_REG_W8(addr, val)                                                \
	hal_write8(adapter->drv_adapter, addr, val)
#define PLTFM_REG_W16(addr, val)                                               \
	hal_write16(adapter->drv_adapter, addr, val)
#define PLTFM_REG_W32(addr, val)                                               \
	hal_write32(adapter->drv_adapter, addr, val)

#define MAC_REG_R8(addr) hal_read8(adapter->drv_adapter, addr)
#define MAC_REG_R16(addr) hal_read16(adapter->drv_adapter, addr)
#define MAC_REG_R32(addr) hal_read32(adapter->drv_adapter, addr)
#define MAC_REG_W8(addr, val) hal_write8(adapter->drv_adapter, addr, val)
#define MAC_REG_W16(addr, val) hal_write16(adapter->drv_adapter, addr, val)
#define MAC_REG_W32(addr, val) hal_write32(adapter->drv_adapter, addr, val)

#if MAC_AX_FEATURE_DBGCMD
#define PLTFM_SNPRINTF(s, sz, fmt, ...)                                               \
	hal_sprintf(adapter->drv_adapter, s, sz, fmt, ##__VA_ARGS__)
#define PLTFM_STRCMP(s1, s2)                                               \
	hal_strcmp(adapter->drv_adapter, s1, s2)
#define PLTFM_STRSEP(s, ct)                                               \
	hal_strsep(adapter->drv_adapter, s, ct)
#define PLTFM_STRLEN(s)                                               \
	hal_strlen(adapter->drv_adapter, s)
#define PLTFM_STRCPY(dest, src)                                               \
	hal_strcpy(adapter->drv_adapter, dest, src)
#define PLTFM_STRPBRK(cs, ct)                                               \
	hal_strpbrk(adapter->drv_adapter, cs, ct)
#define PLTFM_STRTOUL(buf, base)                                               \
	hal_strtoul(adapter->drv_adapter, buf, base)
#endif
#else

/* platform callback */
#define PLTFM_SDIO_CMD52_R8(addr)                                              \
	adapter->pltfm_cb->sdio_cmd52_r8(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R8(addr)                                              \
	adapter->pltfm_cb->sdio_cmd53_r8(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R16(addr)                                             \
	adapter->pltfm_cb->sdio_cmd53_r16(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R32(addr)                                             \
	adapter->pltfm_cb->sdio_cmd53_r32(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_RN(addr, size, val)                                   \
	adapter->pltfm_cb->sdio_cmd53_rn(adapter->drv_adapter, addr, size, val)
#define PLTFM_SDIO_CMD52_W8(addr, val)                                         \
	adapter->pltfm_cb->sdio_cmd52_w8(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_W8(addr, val)                                         \
	adapter->pltfm_cb->sdio_cmd53_w8(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_W16(addr, val)                                        \
	adapter->pltfm_cb->sdio_cmd53_w16(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_W32(addr, val)                                        \
	adapter->pltfm_cb->sdio_cmd53_w32(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_WN(addr, size, val)                                   \
	adapter->pltfm_cb->sdio_cmd53_wn(adapter->drv_adapter, addr, size, val)
#define PLTFM_SDIO_CMD52_CIA_R8(addr)                                          \
	adapter->pltfm_cb->sdio_cmd52_cia_r8(adapter->drv_adapter, addr)

#define PLTFM_REG_R8(addr)                                                     \
	adapter->pltfm_cb->reg_r8(adapter->drv_adapter, addr)
#define PLTFM_REG_R16(addr)                                                    \
	adapter->pltfm_cb->reg_r16(adapter->drv_adapter, addr)
#define PLTFM_REG_R32(addr)                                                    \
	adapter->pltfm_cb->reg_r32(adapter->drv_adapter, addr)
#define PLTFM_REG_W8(addr, val)                                                \
	adapter->pltfm_cb->reg_w8(adapter->drv_adapter, addr, val)
#define PLTFM_REG_W16(addr, val)                                               \
	adapter->pltfm_cb->reg_w16(adapter->drv_adapter, addr, val)
#define PLTFM_REG_W32(addr, val)                                               \
	adapter->pltfm_cb->reg_w32(adapter->drv_adapter, addr, val)

#if MAC_AX_PHL_H2C
#define PLTFM_TX(buf)                                                          \
	adapter->pltfm_cb->tx(adapter->phl_adapter, adapter->drv_adapter, buf)
#define PLTFM_QUERY_H2C(type)                                                  \
	adapter->pltfm_cb->rtl_query_h2c(adapter->phl_adapter,                 \
					 adapter->drv_adapter, type)
#else
#define PLTFM_TX(buf, len)                                                     \
	adapter->pltfm_cb->tx(adapter->drv_adapter, buf, len)
#endif
#define PLTFM_FREE(buf, size)                                                  \
	adapter->pltfm_cb->rtl_free(adapter->drv_adapter, buf, size)
#define PLTFM_MALLOC(size)                                                     \
	adapter->pltfm_cb->rtl_malloc(adapter->drv_adapter, size)
#define PLTFM_MEMCPY(dest, src, size)                                          \
	adapter->pltfm_cb->rtl_memcpy(adapter->drv_adapter, dest, src, size)
#define PLTFM_MEMSET(addr, value, size)                                        \
	adapter->pltfm_cb->rtl_memset(adapter->drv_adapter, addr, value, size)
#define PLTFM_MEMCMP(ptr1, ptr2, num)                                          \
	adapter->pltfm_cb->rtl_memcmp(adapter->drv_adapter, ptr1, ptr2, num)
#define PLTFM_DELAY_US(us)                                                     \
	adapter->pltfm_cb->rtl_delay_us(adapter->drv_adapter, us)
#define PLTFM_DELAY_MS(ms)                                                     \
	adapter->pltfm_cb->rtl_delay_ms(adapter->drv_adapter, ms)
#define PLTFM_SLEEP_US(us)                                                     \
	adapter->pltfm_cb->rtl_sleep_us(adapter->drv_adapter, us)
#define PLTFM_SLEEP_MS(ms)                                                     \
	adapter->pltfm_cb->rtl_sleep_ms(adapter->drv_adapter, ms)

#define PLTFM_MUTEX_INIT(mutex)                                                \
	adapter->pltfm_cb->rtl_mutex_init(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_DEINIT(mutex)                                              \
	adapter->pltfm_cb->rtl_mutex_deinit(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_LOCK(mutex)                                                \
	adapter->pltfm_cb->rtl_mutex_lock(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_UNLOCK(mutex)                                              \
	adapter->pltfm_cb->rtl_mutex_unlock(adapter->drv_adapter, mutex)
#define PLTFM_EVENT_NOTIFY(mac_ft, stat, buf, size)                            \
	adapter->pltfm_cb->event_notify(adapter->drv_adapter, mac_ft, stat,    \
					buf, size)
#define PLTFM_L2_NOTIFY(void)                                                  \
	adapter->pltfm_cb->ser_l2_notify(adapter->phl_adapter, adapter->drv_adapter)

#define PLTFM_LD_FW_SYMBOL(name, buf, buf_size) \
	adapter->pltfm_cb->ld_fw_symbol(adapter->phl_adapter, adapter->drv_adapter,\
	name, buf, buf_size)

#define adapter_to_mac_ops(adapter) ((struct mac_ax_ops *)((adapter)->ops))
#define adapter_to_intf_ops(adapter)                                           \
	((struct mac_ax_intf_ops *)((adapter)->ops->intf_ops))

#define MAC_REG_R8(addr) ops->reg_read8(adapter, addr)
#define MAC_REG_R16(addr) ops->reg_read16(adapter, addr)
#define MAC_REG_R32(addr) ops->reg_read32(adapter, addr)
#define MAC_REG_W8(addr, val) ops->reg_write8(adapter, addr, val)
#define MAC_REG_W16(addr, val) ops->reg_write16(adapter, addr, val)
#define MAC_REG_W32(addr, val) ops->reg_write32(adapter, addr, val)

#if MAC_AX_FEATURE_DBGCMD
#define PLTFM_SNPRINTF(s, sz, fmt, ...)                                               \
	adapter->pltfm_cb->rtl_sprintf(adapter->drv_adapter, s, sz, fmt, ##__VA_ARGS__)
#define PLTFM_STRCMP(s1, s2)                                               \
	adapter->pltfm_cb->rtl_strcmp(adapter->drv_adapter, s1, s2)
#define PLTFM_STRSEP(s, ct)                                               \
	adapter->pltfm_cb->rtl_strsep(adapter->drv_adapter, s, ct)
#define PLTFM_STRLEN(s)                                               \
	adapter->pltfm_cb->rtl_strlen(adapter->drv_adapter, s)
#define PLTFM_STRCPY(dest, src)                                               \
	adapter->pltfm_cb->rtl_strcpy(adapter->drv_adapter, dest, src)
#define PLTFM_STRPBRK(cs, ct)                                               \
	adapter->pltfm_cb->rtl_strpbrk(adapter->drv_adapter, cs, ct)
#define PLTFM_STRTOUL(buf, base)                                               \
	adapter->pltfm_cb->rtl_strtoul(adapter->drv_adapter, buf, base)
#endif
#endif /*CONFIG_NEW_HALMAC_INTERFACE*/

#define MAC_AX_WMM0_SEL		0
#define MAC_AX_WMM1_SEL		1
#define MAC_AX_WMM2_SEL		2
#define MAC_AX_WMM3_SEL		3

#define MAC_AX_HI0_SEL		17
#define MAC_AX_MG0_SEL		18
#define MAC_AX_HI1_SEL		25
#define MAC_AX_MG1_SEL		26

#define MAC_AX_HFC_CH_NUM	12

#define MAC_AX_R32_EA		0xEAEAEAEA
#define MAC_AX_R32_DEAD		0xDEADBEEF
#define MAC_AX_R32_DEAD2	0xDEADDEAD
#define MAC_AX_R32_FF		0xFFFFFFFF
#define MAC_AX_R16_DEAD		0xDEAD

#define MAC_REG_POOL_COUNT	10
#define MAC_REG_OFFSET16	2
#define MAC_REG_OFFSET		4
#define MAC_REG_OFFSET_SH	8
#define MAC_REG_OFFSET_SH_2	3
#define R_AX_CMAC_REG_END	0xFFFF

/* SRAM mem dump */
#define R_AX_INDIR_ACCESS_ENTRY 0x40000
#define MAC_MEM_DUMP_PAGE_SIZE	0x40000

#define CPU_LOCAL_BASE_ADDR		0x18003000
#define AXIDMA_BASE_ADDR		0x18006000
#define	STA_SCHED_BASE_ADDR		0x18808000
#define	RXPLD_FLTR_CAM_BASE_ADDR	0x18813000
#define	SEC_CAM_BASE_ADDR		0x18814000
#define	WOW_CAM_BASE_ADDR		0x18815000
#define	CMAC_TBL_BASE_ADDR		0x18840000
#define	ADDR_CAM_BASE_ADDR		0x18850000
#define	BSSID_CAM_BASE_ADDR		0x18853000
#define	BA_CAM_BASE_ADDR		0x18854000
#define	BCN_IE_CAM0_BASE_ADDR		0x18855000
#define	SHARED_BUF_BASE_ADDR		0x18700000
#define	DMAC_TBL_BASE_ADDR		0x18800000
#define	SHCUT_MACHDR_BASE_ADDR		0x18800800
#define	BCN_IE_CAM1_BASE_ADDR		0x188A0000
#define	TXD_FIFO_0_BASE_ADDR		0x18856200
#define	TXD_FIFO_1_BASE_ADDR		0x188A1080
#define WD_PAGE_BASE_ADDR		0x0
#define	WCPU_DATA_BASE_ADDR			0x18E00000

#define CCTL_INFO_SIZE		32
#define DCTL_INFO_SIZE		16
#define DCTL_INFO_SIZE_V1	32
#define MACHDR_SIZE		56
#define BA_CAM_SIZE		64
#define BA_CAM_NUM_SH		3
#define BCN_IE_CAM_SIZE		8
#define BCN_IE_CAM_NUM		12
#define AXIDMA_REG_SIZE		0x1000

/*--------------------Define Enum---------------------------------------*/

/**
 * @enum mac_ax_data_ch
 *
 * @brief mac_ax_data_ch
 *
 * @var mac_ax_data_ch::MAC_AX_DATA_CH0
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH1
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH2
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH3
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH4
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH5
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH6
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH7
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_HIQ
 * Please Place Description here.
 */

/**
 * @enum mac_ax_data_ch
 *
 * @brief mac_ax_data_ch
 *
 * @var mac_ax_data_ch::MAC_AX_DATA_CH0
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH1
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH2
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH3
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH4
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH5
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH6
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_CH7
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_MGN
 * Please Place Description here.
 * @var mac_ax_data_ch::MAC_AX_DATA_HIQ
 * Please Place Description here.
 */
enum mac_ax_data_ch {
	MAC_AX_DATA_CH0 = 0,
	MAC_AX_DATA_CH1 = 1,
	MAC_AX_DATA_CH2 = 2,
	MAC_AX_DATA_CH3 = 3,
	MAC_AX_DATA_CH4 = 4,
	MAC_AX_DATA_CH5 = 5,
	MAC_AX_DATA_CH6 = 6,
	MAC_AX_DATA_CH7 = 7,
	MAC_AX_DATA_CH8 = 8,
	MAC_AX_DATA_CH9 = 9,
	MAC_AX_DATA_CH10 = 10,
	MAC_AX_DATA_CH11 = 11,
};

/**
 * @enum mac_ax_data_rate
 *
 * @brief mac_ax_data_rate
 *
 * @var mac_ax_data_rate::MAC_AX_CCK1
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_CCK2
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_CCK5_5
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_CCK11
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_OFDM6
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_OFDM9
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_OFDM12
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_OFDM18
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_OFDM24
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_OFDM36
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_OFDM48
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_OFDM54
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS0
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS1
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS2
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS3
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS4
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS5
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS6
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS7
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS8
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS9
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS10
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS11
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS12
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS13
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS14
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS15
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS16
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS17
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS18
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS19
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS20
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS21
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS22
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS23
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS24
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS25
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS26
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS27
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS28
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS29
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS30
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_MCS31
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS1_MCS0
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS1_MCS1
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS1_MCS2
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS1_MCS3
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS1_MCS4
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS1_MCS5
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS1_MCS6
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS1_MCS7
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS1_MCS8
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS1_MCS9
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS2_MCS0
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS2_MCS1
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS2_MCS2
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS2_MCS3
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS2_MCS4
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS2_MCS5
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS2_MCS6
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS2_MCS7
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS2_MCS8
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS2_MCS9
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS3_MCS0
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS3_MCS1
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS3_MCS2
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS3_MCS3
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS3_MCS4
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS3_MCS5
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS3_MCS6
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS3_MCS7
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS3_MCS8
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS3_MCS9
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS4_MCS0
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS4_MCS1
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS4_MCS2
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS4_MCS3
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS4_MCS4
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS4_MCS5
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS4_MCS6
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS4_MCS7
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS4_MCS8
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_VHT_NSS4_MCS9
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS1_MCS0
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS1_MCS1
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS1_MCS2
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS1_MCS3
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS1_MCS4
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS1_MCS5
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS1_MCS6
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS1_MCS7
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS1_MCS8
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS1_MCS9
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS1_MCS10
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS1_MCS11
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS2_MCS0
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS2_MCS1
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS2_MCS2
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS2_MCS3
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS2_MCS4
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS2_MCS5
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS2_MCS6
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS2_MCS7
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS2_MCS8
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS2_MCS9
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS2_MCS10
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS2_MCS11
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS3_MCS0
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS3_MCS1
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS3_MCS2
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS3_MCS3
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS3_MCS4
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS3_MCS5
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS3_MCS6
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS3_MCS7
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS3_MCS8
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS3_MCS9
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS3_MCS10
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS3_MCS11
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS4_MCS0
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS4_MCS1
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS4_MCS2
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS4_MCS3
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS4_MCS4
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS4_MCS5
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS4_MCS6
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS4_MCS7
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS4_MCS8
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS4_MCS9
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS4_MCS10
 * Please Place Description here.
 * @var mac_ax_data_rate::MAC_AX_HE_NSS4_MCS11
 * Please Place Description here.
 */
enum mac_ax_data_rate {
	MAC_AX_CCK1		= 0x0,
	MAC_AX_CCK2		= 0x1,
	MAC_AX_CCK5_5		= 0x2,
	MAC_AX_CCK11		= 0x3,
	MAC_AX_OFDM6		= 0x4,
	MAC_AX_OFDM9		= 0x5,
	MAC_AX_OFDM12		= 0x6,
	MAC_AX_OFDM18		= 0x7,
	MAC_AX_OFDM24		= 0x8,
	MAC_AX_OFDM36		= 0x9,
	MAC_AX_OFDM48		= 0xA,
	MAC_AX_OFDM54		= 0xB,
	MAC_AX_MCS0		= 0x80,
	MAC_AX_MCS1		= 0x81,
	MAC_AX_MCS2		= 0x82,
	MAC_AX_MCS3		= 0x83,
	MAC_AX_MCS4		= 0x84,
	MAC_AX_MCS5		= 0x85,
	MAC_AX_MCS6		= 0x86,
	MAC_AX_MCS7		= 0x87,
	MAC_AX_MCS8		= 0x88,
	MAC_AX_MCS9		= 0x89,
	MAC_AX_MCS10		= 0x8A,
	MAC_AX_MCS11		= 0x8B,
	MAC_AX_MCS12		= 0x8C,
	MAC_AX_MCS13		= 0x8D,
	MAC_AX_MCS14		= 0x8E,
	MAC_AX_MCS15		= 0x8F,
	MAC_AX_MCS16		= 0x90,
	MAC_AX_MCS17		= 0x91,
	MAC_AX_MCS18		= 0x92,
	MAC_AX_MCS19		= 0x93,
	MAC_AX_MCS20		= 0x94,
	MAC_AX_MCS21		= 0x95,
	MAC_AX_MCS22		= 0x96,
	MAC_AX_MCS23		= 0x97,
	MAC_AX_MCS24		= 0x98,
	MAC_AX_MCS25		= 0x99,
	MAC_AX_MCS26		= 0x9A,
	MAC_AX_MCS27		= 0x9B,
	MAC_AX_MCS28		= 0x9C,
	MAC_AX_MCS29		= 0x9D,
	MAC_AX_MCS30		= 0x9E,
	MAC_AX_MCS31		= 0x9F,
	MAC_AX_VHT_NSS1_MCS0	= 0x100,
	MAC_AX_VHT_NSS1_MCS1	= 0x101,
	MAC_AX_VHT_NSS1_MCS2	= 0x102,
	MAC_AX_VHT_NSS1_MCS3	= 0x103,
	MAC_AX_VHT_NSS1_MCS4	= 0x104,
	MAC_AX_VHT_NSS1_MCS5	= 0x105,
	MAC_AX_VHT_NSS1_MCS6	= 0x106,
	MAC_AX_VHT_NSS1_MCS7	= 0x107,
	MAC_AX_VHT_NSS1_MCS8	= 0x108,
	MAC_AX_VHT_NSS1_MCS9	= 0x109,
	MAC_AX_VHT_NSS2_MCS0	= 0x110,
	MAC_AX_VHT_NSS2_MCS1	= 0x111,
	MAC_AX_VHT_NSS2_MCS2	= 0x112,
	MAC_AX_VHT_NSS2_MCS3	= 0x113,
	MAC_AX_VHT_NSS2_MCS4	= 0x114,
	MAC_AX_VHT_NSS2_MCS5	= 0x115,
	MAC_AX_VHT_NSS2_MCS6	= 0x116,
	MAC_AX_VHT_NSS2_MCS7	= 0x117,
	MAC_AX_VHT_NSS2_MCS8	= 0x118,
	MAC_AX_VHT_NSS2_MCS9	= 0x119,
	MAC_AX_VHT_NSS3_MCS0	= 0x120,
	MAC_AX_VHT_NSS3_MCS1	= 0x121,
	MAC_AX_VHT_NSS3_MCS2	= 0x122,
	MAC_AX_VHT_NSS3_MCS3	= 0x123,
	MAC_AX_VHT_NSS3_MCS4	= 0x124,
	MAC_AX_VHT_NSS3_MCS5	= 0x125,
	MAC_AX_VHT_NSS3_MCS6	= 0x126,
	MAC_AX_VHT_NSS3_MCS7	= 0x127,
	MAC_AX_VHT_NSS3_MCS8	= 0x128,
	MAC_AX_VHT_NSS3_MCS9	= 0x129,
	MAC_AX_VHT_NSS4_MCS0	= 0x130,
	MAC_AX_VHT_NSS4_MCS1	= 0x131,
	MAC_AX_VHT_NSS4_MCS2	= 0x132,
	MAC_AX_VHT_NSS4_MCS3	= 0x133,
	MAC_AX_VHT_NSS4_MCS4	= 0x134,
	MAC_AX_VHT_NSS4_MCS5	= 0x135,
	MAC_AX_VHT_NSS4_MCS6	= 0x136,
	MAC_AX_VHT_NSS4_MCS7	= 0x137,
	MAC_AX_VHT_NSS4_MCS8	= 0x138,
	MAC_AX_VHT_NSS4_MCS9	= 0x139,
	MAC_AX_HE_NSS1_MCS0	= 0x180,
	MAC_AX_HE_NSS1_MCS1	= 0x181,
	MAC_AX_HE_NSS1_MCS2	= 0x182,
	MAC_AX_HE_NSS1_MCS3	= 0x183,
	MAC_AX_HE_NSS1_MCS4	= 0x184,
	MAC_AX_HE_NSS1_MCS5	= 0x185,
	MAC_AX_HE_NSS1_MCS6	= 0x186,
	MAC_AX_HE_NSS1_MCS7	= 0x187,
	MAC_AX_HE_NSS1_MCS8	= 0x188,
	MAC_AX_HE_NSS1_MCS9	= 0x189,
	MAC_AX_HE_NSS1_MCS10	= 0x18A,
	MAC_AX_HE_NSS1_MCS11	= 0x18B,
	MAC_AX_HE_NSS2_MCS0	= 0x190,
	MAC_AX_HE_NSS2_MCS1	= 0x191,
	MAC_AX_HE_NSS2_MCS2	= 0x192,
	MAC_AX_HE_NSS2_MCS3	= 0x193,
	MAC_AX_HE_NSS2_MCS4	= 0x194,
	MAC_AX_HE_NSS2_MCS5	= 0x195,
	MAC_AX_HE_NSS2_MCS6	= 0x196,
	MAC_AX_HE_NSS2_MCS7	= 0x197,
	MAC_AX_HE_NSS2_MCS8	= 0x198,
	MAC_AX_HE_NSS2_MCS9	= 0x199,
	MAC_AX_HE_NSS2_MCS10	= 0x19A,
	MAC_AX_HE_NSS2_MCS11	= 0x19B,
	MAC_AX_HE_NSS3_MCS0	= 0x1A0,
	MAC_AX_HE_NSS3_MCS1	= 0x1A1,
	MAC_AX_HE_NSS3_MCS2	= 0x1A2,
	MAC_AX_HE_NSS3_MCS3	= 0x1A3,
	MAC_AX_HE_NSS3_MCS4	= 0x1A4,
	MAC_AX_HE_NSS3_MCS5	= 0x1A5,
	MAC_AX_HE_NSS3_MCS6	= 0x1A6,
	MAC_AX_HE_NSS3_MCS7	= 0x1A7,
	MAC_AX_HE_NSS3_MCS8	= 0x1A8,
	MAC_AX_HE_NSS3_MCS9	= 0x1A9,
	MAC_AX_HE_NSS3_MCS10	= 0x1AA,
	MAC_AX_HE_NSS3_MCS11	= 0x1AB,
	MAC_AX_HE_NSS4_MCS0	= 0x1B0,
	MAC_AX_HE_NSS4_MCS1	= 0x1B1,
	MAC_AX_HE_NSS4_MCS2	= 0x1B2,
	MAC_AX_HE_NSS4_MCS3	= 0x1B3,
	MAC_AX_HE_NSS4_MCS4	= 0x1B4,
	MAC_AX_HE_NSS4_MCS5	= 0x1B5,
	MAC_AX_HE_NSS4_MCS6	= 0x1B6,
	MAC_AX_HE_NSS4_MCS7	= 0x1B7,
	MAC_AX_HE_NSS4_MCS8	= 0x1B8,
	MAC_AX_HE_NSS4_MCS9	= 0x1B9,
	MAC_AX_HE_NSS4_MCS10	= 0x1BA,
	MAC_AX_HE_NSS4_MCS11	= 0x1BB
};

/**
 * @struct wd_body_t
 * @brief wd_body_t
 *
 * @var wd_body_t::dword0
 * Please Place Description here.
 * @var wd_body_t::dword1
 * Please Place Description here.
 * @var wd_body_t::dword2
 * Please Place Description here.
 * @var wd_body_t::dword3
 * Please Place Description here.
 * @var wd_body_t::dword4
 * Please Place Description here.
 * @var wd_body_t::dword5
 * Please Place Description here.
 */
struct wd_body_t {
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
};

/**
 * @struct wd_info_t
 * @brief wd_info_t
 *
 * @var wd_info_t::dword0
 * Please Place Description here.
 * @var wd_info_t::dword1
 * Please Place Description here.
 * @var wd_info_t::dword2
 * Please Place Description here.
 * @var wd_info_t::dword3
 * Please Place Description here.
 * @var wd_info_t::dword4
 * Please Place Description here.
 * @var wd_info_t::dword5
 * Please Place Description here.
 */
struct wd_info_t {
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
};

#define WD_BODY_LEN	(sizeof(struct wd_body_t))
#define WD_INFO_LEN	(sizeof(struct wd_info_t))

/**
 * @struct wd_body_t_v1
 * @brief wd_body_t_v1
 *
 * @var wd_body_t::dword0
 * Please Place Description here.
 * @var wd_body_t::dword1
 * Please Place Description here.
 * @var wd_body_t::dword2
 * Please Place Description here.
 * @var wd_body_t::dword3
 * Please Place Description here.
 * @var wd_body_t::dword4
 * Please Place Description here.
 * @var wd_body_t::dword5
 * Please Place Description here.
 * @var wd_body_t::dword6
 * Please Place Description here.
 * @var wd_body_t::dword7
 * Please Place Description here.
 */
struct wd_body_t_v1 {
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
	u32 dword6;
	u32 dword7;
};

#define WD_BODY_LEN_V1	(sizeof(struct wd_body_t_v1))

/**
 * @struct rxd_short_t
 * @brief rxd_short_t
 *
 * @var rxd_short_t::dword0
 * Please Place Description here.
 * @var rxd_short_t::dword1
 * Please Place Description here.
 * @var rxd_short_t::dword2
 * Please Place Description here.
 * @var rxd_short_t::dword3
 * Please Place Description here.
 */
struct rxd_short_t {
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
};

/**
 * @struct rxd_long_t
 * @brief rxd_long_t
 *
 * @var rxd_long_t::dword0
 * Please Place Description here.
 * @var rxd_long_t::dword1
 * Please Place Description here.
 * @var rxd_long_t::dword2
 * Please Place Description here.
 * @var rxd_long_t::dword3
 * Please Place Description here.
 * @var rxd_long_t::dword4
 * Please Place Description here.
 * @var rxd_long_t::dword5
 * Please Place Description here.
 * @var rxd_long_t::dword6
 * Please Place Description here.
 * @var rxd_long_t::dword7
 * Please Place Description here.
 */
struct rxd_long_t {
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
	u32 dword6;
	u32 dword7;
};

#define RXD_SHORT_LEN	(sizeof(struct rxd_short_t))
#define RXD_LONG_LEN	(sizeof(struct rxd_long_t))

/**
 * @struct txd_proc_type
 * @brief txd_proc_type
 *
 * @var txd_proc_type::type
 * Please Place Description here.
 * @var txd_proc_type::handler
 * Please Place Description here.
 */
struct txd_proc_type {
	enum rtw_packet_type type;
	u32 (*handler)(struct mac_ax_adapter *adapter,
		       struct rtw_t_meta_data *info, u8 *buf, u32 len);
};

/**
 * @struct rxd_parse_type
 * @brief rxd_parse_type
 *
 * @var rxd_parse_type::type
 * Please Place Description here.
 * @var rxd_parse_type::handler
 * Please Place Description here.
 */
struct rxd_parse_type {
	u8 type;
	u32 (*handler)(struct mac_ax_adapter *adapter,
		       struct mac_ax_rxpkt_info *info, u8 *buf, u32 len);
};

/**
 * @enum mac_ax_bw
 *
 * @brief mac_ax_bw
 *
 * @var mac_ax_bw::MAC_AX_BW_20M
 * Please Place Description here.
 * @var mac_ax_bw::MAC_AX_BW_40M
 * Please Place Description here.
 * @var mac_ax_bw::MAC_AX_BW_80M
 * Please Place Description here.
 * @var mac_ax_bw::MAC_AX_BW_160M
 * Please Place Description here.
 * @var mac_ax_bw::MAC_AX_BW_UNDEFINE
 * Please Place Description here.
 */
enum mac_ax_bw {
	MAC_AX_BW_20M = 0,
	MAC_AX_BW_40M = 1,
	MAC_AX_BW_80M = 2,
	MAC_AX_BW_160M = 3,
	MAC_AX_BW_UNDEFINE = 0x7F
};

/**
 * @enum mac_ax_gi_ltf
 *
 * @brief mac_ax_gi_ltf
 *
 * @var mac_ax_gi_ltf::MAC_AX_LGI_4XHE32
 * Please Place Description here.
 * @var mac_ax_gi_ltf::MAC_AX_SGI_4XHE08
 * Please Place Description here.
 * @var mac_ax_gi_ltf::MAC_AX_2XHE16
 * Please Place Description here.
 * @var mac_ax_gi_ltf::MAC_AX_2XHE08
 * Please Place Description here.
 * @var mac_ax_gi_ltf::MAC_AX_1XHE16
 * Please Place Description here.
 * @var mac_ax_gi_ltf::MAC_AX_1XHE08
 * Please Place Description here.
 */
enum mac_ax_gi_ltf {
	MAC_AX_LGI_4XHE32 = 0,
	MAC_AX_SGI_4XHE08 = 1,
	MAC_AX_2XHE16 = 2,
	MAC_AX_2XHE08 = 3,
	MAC_AX_1XHE16 = 4,
	MAC_AX_1XHE08 = 5
};

/**
 * @enum mac_ax_stbc
 *
 * @brief mac_ax_stbc
 *
 * @var mac_ax_stbc::MAC_AX_STBC_DIS
 * Please Place Description here.
 * @var mac_ax_stbc::MAC_AX_STBC_EN
 * Please Place Description here.
 * @var mac_ax_stbc::MAC_AX_STBC_HT2
 * Please Place Description here.
 */
enum mac_ax_stbc {
	MAC_AX_STBC_DIS = 0,
	MAC_AX_STBC_EN = 1,
	MAC_AX_STBC_HT2 = 2
};

/**
 * @enum mac_ax_delay_tx_en
 *
 * @brief mac_ax_delay_tx_en
 *
 * @var mac_ax_delay_tx_en::MAC_AX_DELAY_TX_DIS
 * Please Place Description here.
 * @var mac_ax_delay_tx_en::MAC_AX_DELAY_TX_B0
 * Please Place Description here.
 * @var mac_ax_delay_tx_en::MAC_AX_DELAY_TX_B1
 * Please Place Description here.
 * @var mac_ax_delay_tx_en::MAC_AX_DELAY_TX_BOTH
 * Please Place Description here.
 */
enum mac_ax_delay_tx_en {
	MAC_AX_DELAY_TX_DIS = 0,
	MAC_AX_DELAY_TX_B0 = 1,
	MAC_AX_DELAY_TX_B1 = 2,
	MAC_AX_DELAY_TX_BOTH = 3,
};

/**
 * @enum mac_ax_hcifc_mode
 *
 * @brief mac_ax_hcifc_mode
 *
 * @var mac_ax_hcifc_mode::MAC_AX_HCIFC_POH
 * Please Place Description here.
 * @var mac_ax_hcifc_mode::MAC_AX_HCIFC_STF
 * Please Place Description here.
 * @var mac_ax_hcifc_mode::MAC_AX_HCIFC_SDIO
 * Please Place Description here.
 * @var mac_ax_hcifc_mode::MAC_AX_HCIFC_LAST
 * Please Place Description here.
 * @var mac_ax_hcifc_mode::MAC_AX_HCIFC_MODE_MAX
 * Please Place Description here.
 * @var mac_ax_hcifc_mode::MAC_AX_HCIFC_MODE_INVALID
 * Please Place Description here.
 */
enum mac_ax_hcifc_mode {
	MAC_AX_HCIFC_POH = 0,
	MAC_AX_HCIFC_STF = 1,
	MAC_AX_HCIFC_SDIO = 2,

	/* keep last */
	MAC_AX_HCIFC_LAST,
	MAC_AX_HCIFC_MODE_MAX = MAC_AX_HCIFC_LAST,
	MAC_AX_HCIFC_MODE_INVALID = MAC_AX_HCIFC_LAST,
};

/**
 * @enum mac_ax_bcn_hit_rule
 *
 * @brief mac_ax_bcn_hit_rule
 *
 * @var mac_ax_bcn_hit_rule::MAC_AX_A3
 * Please Place Description here.
 * @var mac_ax_bcn_hit_rule::MAC_AX_A2
 * Please Place Description here.
 * @var mac_ax_bcn_hit_rule::MAC_AX_A2_AND_A3
 * Please Place Description here.
 * @var mac_ax_bcn_hit_rule::MAC_AX_A2_OR_A3
 * Please Place Description here.
 */
enum mac_ax_bcn_hit_rule {
	MAC_AX_A3,
	MAC_AX_A2,
	MAC_AX_A2_AND_A3,
	MAC_AX_A2_OR_A3
};

/**
 * @enum mac_ax_hit_rule
 *
 * @brief mac_ax_hit_rule
 *
 * @var mac_ax_hit_rule::MAC_AX_A1_AND_A2
 * Please Place Description here.
 * @var mac_ax_hit_rule::MAC_AX_A1_AND_A3
 * Please Place Description here.
 */
enum mac_ax_hit_rule {
	MAC_AX_A1_AND_A2,
	MAC_AX_A1_AND_A3
};

/**
 * @enum mac_ax_bb_sel
 *
 * @brief mac_ax_bb_sel
 *
 * @var mac_ax_bb_sel::MAC_AX_PHY_0
 * Please Place Description here.
 * @var mac_ax_bb_sel::MAC_AX_PHY_1
 * Please Place Description here.
 */
enum mac_ax_bb_sel {
	MAC_AX_PHY_0,
	MAC_AX_PHY_1
};

/**
 * @enum mac_ax_pps_sel
 *
 * @brief mac_ax_pps_sel
 *
 * @var mac_ax_pps_sel::MAC_AX_PPS_0
 * Please Place Description here.
 * @var mac_ax_pps_sel::MAC_AX_PPS_1
 * Please Place Description here.
 * @var mac_ax_pps_sel::MAC_AX_PPS_LAST
 * Please Place Description here.
 * @var mac_ax_pps_sel::MAC_AX_PPS_MAX
 * Please Place Description here.
 * @var mac_ax_pps_sel::MAC_AX_PPS_INVALID
 * Please Place Description here.
 */
enum mac_ax_pps_sel {
	MAC_AX_PPS_0 = 0,
	MAC_AX_PPS_1,

	/* keep last */
	MAC_AX_PPS_LAST,
	MAC_AX_PPS_MAX = MAC_AX_PPS_LAST,
	MAC_AX_PPS_INVALID = MAC_AX_PPS_LAST,
};

/**
 * @enum mac_ax_tgt_ind
 *
 * @brief mac_ax_tgt_ind
 *
 * @var mac_ax_tgt_ind::MAC_AX_TO_HOST
 * Please Place Description here.
 * @var mac_ax_tgt_ind::MAC_AX_TO_WLAN_CPU
 * Please Place Description here.
 * @var mac_ax_tgt_ind::MAC_AX_TO_DRIVER
 * Please Place Description here.
 */
enum mac_ax_tgt_ind {
	MAC_AX_TO_HOST,
	MAC_AX_TO_WLAN_CPU,
	MAC_AX_TO_DRIVER
};

/**
 * @enum mac_ax_frm_tgt_ind
 *
 * @brief mac_ax_frm_tgt_ind
 *
 * @var mac_ax_frm_tgt_ind::MAC_AX_DIS_FRAM_TGT
 * Please Place Description here.
 * @var mac_ax_frm_tgt_ind::MAC_AX_MGT_FRAM_TGT
 * Please Place Description here.
 * @var mac_ax_frm_tgt_ind::MAC_AX_CTL_FRAM_TGT
 * Please Place Description here.
 * @var mac_ax_frm_tgt_ind::MAC_AX_MGT_CTL_FRAM_TGT
 * Please Place Description here.
 * @var mac_ax_frm_tgt_ind::MAC_AX_DATA_FRAM_TGT
 * Please Place Description here.
 * @var mac_ax_frm_tgt_ind::MAC_AX_MGT_DATA_FRAM_TGT
 * Please Place Description here.
 * @var mac_ax_frm_tgt_ind::MAC_AX_CTL_DATA_FRAM_TGT
 * Please Place Description here.
 * @var mac_ax_frm_tgt_ind::MAC_AX_MGT_CTL_DATA_FRAM_TGT
 * Please Place Description here.
 */
enum mac_ax_frm_tgt_ind {
	MAC_AX_DIS_FRAM_TGT,
	MAC_AX_MGT_FRAM_TGT,
	MAC_AX_CTL_FRAM_TGT,
	MAC_AX_MGT_CTL_FRAM_TGT,
	MAC_AX_DATA_FRAM_TGT,
	MAC_AX_MGT_DATA_FRAM_TGT,
	MAC_AX_CTL_DATA_FRAM_TGT,
	MAC_AX_MGT_CTL_DATA_FRAM_TGT,
};

/**
 * @enum mac_ax_txcnt_sel
 *
 * @brief mac_ax_txcnt_sel
 *
 * @var mac_ax_txcnt_sel::MAC_AX_TXCNT_LCCK
 * Please Place Description here.
 * @var mac_ax_txcnt_sel::MAC_AX_TXCNT_SCCK
 * Please Place Description here.
 * @var mac_ax_txcnt_sel::MAC_AX_TXCNT_OFDM
 * Please Place Description here.
 * @var mac_ax_txcnt_sel::MAC_AX_TXCNT_HT
 * Please Place Description here.
 * @var mac_ax_txcnt_sel::MAC_AX_TXCNT_HTGF
 * Please Place Description here.
 * @var mac_ax_txcnt_sel::MAC_AX_TXCNT_VHTSU
 * Please Place Description here.
 * @var mac_ax_txcnt_sel::MAC_AX_TXCNT_VHTMU
 * Please Place Description here.
 * @var mac_ax_txcnt_sel::MAC_AX_TXCNT_HESU
 * Please Place Description here.
 * @var mac_ax_txcnt_sel::MAC_AX_TXCNT_HEERSU
 * Please Place Description here.
 * @var mac_ax_txcnt_sel::MAC_AX_TXCNT_HEMU
 * Please Place Description here.
 * @var mac_ax_txcnt_sel::MAC_AX_TXCNT_HETB
 * Please Place Description here.
 */
enum mac_ax_txcnt_sel {
	MAC_AX_TXCNT_LCCK = 0,
	MAC_AX_TXCNT_SCCK = 1,
	MAC_AX_TXCNT_OFDM = 2,
	MAC_AX_TXCNT_HT = 3,
	MAC_AX_TXCNT_HTGF = 4,
	MAC_AX_TXCNT_VHTSU = 5,
	MAC_AX_TXCNT_VHTMU = 6,
	MAC_AX_TXCNT_HESU = 7,
	MAC_AX_TXCNT_HEERSU = 8,
	MAC_AX_TXCNT_HEMU = 9,
	MAC_AX_TXCNT_HETB = 0xA
};

/**
 * @enum mac_ax_rxcnt_sel
 *
 * @brief mac_ax_rxcnt_sel
 *
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_OFDM_OK
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_OFDM_FAIL
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_OFDM_FAM
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_CCK_OK
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_CCK_FAIL
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_CCK_FAM
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HT_OK
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HT_FAIL
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HT_PPDU
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HT_FAM
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_VHTSU_OK
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_VHTSU_FAIL
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_VHTSU_PPDU
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_VHTSU_FAM
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_VHTMU_OK
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_VHTMU_FAIL
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_VHTMU_PPDU
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_VHTMU_FAM
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HESU_OK
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HESU_FAIL
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HESU_PPDU
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HESU_FAM
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HEMU_OK
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HEMU_FAIL
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HEMU_PPDU
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HEMU_FAM
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HETB_OK
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HETB_FAIL
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HETB_PPDU
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_HETB_FAM
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_INVD
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_RECCA
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_FULLDRP
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_FULLDRP_PKT
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_RXDMA
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_USER0
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_USER1
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_USER2
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_USER3
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_CONT_FCS
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_PKTFLTR_DRP
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_CSIPKT_DMA_OK
 * Please Place Description here.
 * @var mac_ax_rxcnt_sel::MAC_AX_RXCNT_CSIPKT_DMA_DROP
 * Please Place Description here.
 */
enum mac_ax_rxcnt_sel {
	MAC_AX_RXCNT_OFDM_OK = 0,
	MAC_AX_RXCNT_OFDM_FAIL = 1,
	MAC_AX_RXCNT_OFDM_FAM = 2,
	MAC_AX_RXCNT_CCK_OK = 3,
	MAC_AX_RXCNT_CCK_FAIL = 4,
	MAC_AX_RXCNT_CCK_FAM = 5,
	MAC_AX_RXCNT_HT_OK = 6,
	MAC_AX_RXCNT_HT_FAIL = 7,
	MAC_AX_RXCNT_HT_PPDU = 8,
	MAC_AX_RXCNT_HT_FAM = 9,
	MAC_AX_RXCNT_VHTSU_OK = 0xA,
	MAC_AX_RXCNT_VHTSU_FAIL = 0xB,
	MAC_AX_RXCNT_VHTSU_PPDU = 0xC,
	MAC_AX_RXCNT_VHTSU_FAM = 0xD,
	MAC_AX_RXCNT_VHTMU_OK = 0xE,
	MAC_AX_RXCNT_VHTMU_FAIL = 0xF,
	MAC_AX_RXCNT_VHTMU_PPDU = 0x10,
	MAC_AX_RXCNT_VHTMU_FAM = 0x11,
	MAC_AX_RXCNT_HESU_OK = 0x12,
	MAC_AX_RXCNT_HESU_FAIL = 0x13,
	MAC_AX_RXCNT_HESU_PPDU = 0x14,
	MAC_AX_RXCNT_HESU_FAM = 0x15,
	MAC_AX_RXCNT_HEMU_OK = 0x16,
	MAC_AX_RXCNT_HEMU_FAIL = 0x17,
	MAC_AX_RXCNT_HEMU_PPDU = 0x18,
	MAC_AX_RXCNT_HEMU_FAM = 0x19,
	MAC_AX_RXCNT_HETB_OK = 0x1A,
	MAC_AX_RXCNT_HETB_FAIL = 0x1B,
	MAC_AX_RXCNT_HETB_PPDU = 0x1C,
	MAC_AX_RXCNT_HETB_FAM = 0x1D,
	MAC_AX_RXCNT_INVD = 0x1E,
	MAC_AX_RXCNT_RECCA = 0x1F,
	MAC_AX_RXCNT_FULLDRP = 0x20,
	MAC_AX_RXCNT_FULLDRP_PKT = 0x21,
	MAC_AX_RXCNT_RXDMA = 0x22,
	MAC_AX_RXCNT_PKTFLTR_DRP = 0x23,
	MAC_AX_RXCNT_CSIPKT_DMA_OK = 0x24,
	MAC_AX_RXCNT_CSIPKT_DMA_DROP = 0x25,
	MAC_AX_RXCNT_NDP_PPDU = 0x26,
	MAC_AX_RXCNT_CONT_FCS = 0x27,
	MAC_AX_RXCNT_USER0 = 0x28,
	MAC_AX_RXCNT_USER1 = 0x29,
	MAC_AX_RXCNT_USER2 = 0x2A,
	MAC_AX_RXCNT_USER3 = 0x2B,
	MAC_AX_RXCNT_USER4 = 0x2C,
	MAC_AX_RXCNT_USER5 = 0x2D,
	MAC_AX_RXCNT_USER6 = 0x2E,
	MAC_AX_RXCNT_USER7 = 0x2F,
};

/**
 * @enum mac_ax_wde_pg_size
 *
 * @brief mac_ax_wde_pg_size
 *
 * @var mac_ax_wde_pg_size::MAC_AX_WDE_PG_64
 * Please Place Description here.
 * @var mac_ax_wde_pg_size::MAC_AX_WDE_PG_128
 * Please Place Description here.
 * @var mac_ax_wde_pg_size::MAC_AX_WDE_PG_256
 * Please Place Description here.
 */
enum mac_ax_wde_pg_size {
	MAC_AX_WDE_PG_64 = 64,
	MAC_AX_WDE_PG_128 = 128,
	MAC_AX_WDE_PG_256 = 256
};

/**
 * @enum mac_ax_ple_pg_size
 *
 * @brief mac_ax_ple_pg_size
 *
 * @var mac_ax_ple_pg_size::MAC_AX_PLE_PG_64
 * Please Place Description here.
 * @var mac_ax_ple_pg_size::MAC_AX_PLE_PG_128
 * Please Place Description here.
 * @var mac_ax_ple_pg_size::MAC_AX_PLE_PG_256
 * Please Place Description here.
 */
enum mac_ax_ple_pg_size {
	MAC_AX_PLE_PG_64 = 64,
	MAC_AX_PLE_PG_128 = 128,
	MAC_AX_PLE_PG_256 = 256
};

/**
 * @enum mac_ax_iecam_type
 *
 * @brief mac_ax_iecam_type
 *
 * @var mac_ax_iecam_type::MAC_AX_IECAM_UNAVAL
 * Please Place Description here.
 * @var mac_ax_iecam_type::MAC_AX_IECAM_IESHW
 * Please Place Description here.
 * @var mac_ax_iecam_type::MAC_AX_IECAM_SPEOFT
 * Please Place Description here.
 * @var mac_ax_iecam_type::MAC_AX_IECAM_CRC
 * Please Place Description here.
 */
enum mac_ax_iecam_type {
	MAC_AX_IECAM_UNAVAL = 0,
	MAC_AX_IECAM_IESHW = 1,
	MAC_AX_IECAM_SPEOFT = 2,
	MAC_AX_IECAM_CRC = 3
};

/**
 * @enum mac_ax_tcpip_chksum_ofd_status
 *
 * @brief mac_ax_tcpip_chksum_ofd_status
 *
 * @var mac_ax_tcpip_chksum_ofd_status::MAC_AX_CHKSUM_OFD_IPV4_TCP_OK
 * Please Place Description here.
 * @var mac_ax_tcpip_chksum_ofd_status::MAC_AX_CHKSUM_OFD_IPV6_TCP_OK
 * Please Place Description here.
 * @var mac_ax_tcpip_chksum_ofd_status::MAC_AX_CHKSUM_OFD_IPV4_UDP_OK
 * Please Place Description here.
 * @var mac_ax_tcpip_chksum_ofd_status::MAC_AX_CHKSUM_OFD_IPV6_UDP_OK
 * Please Place Description here.
 * @var mac_ax_tcpip_chksum_ofd_status::MAC_AX_CHKSUM_OFD_CHKSUM_ERR
 * Please Place Description here.
 * @var mac_ax_tcpip_chksum_ofd_status::MAC_AX_CHKSUM_OFD_HW_NO_SUPPORT
 * Please Place Description here.
 * @var mac_ax_tcpip_chksum_ofd_status::MAC_AX_CHKSUM_OFD_INVALID
 * Please Place Description here.
 */
enum mac_ax_tcpip_chksum_ofd_status {
	MAC_AX_CHKSUM_OFD_IPV4_TCP_OK = 0,
	MAC_AX_CHKSUM_OFD_IPV6_TCP_OK = 1,
	MAC_AX_CHKSUM_OFD_IPV4_UDP_OK = 2,
	MAC_AX_CHKSUM_OFD_IPV6_UDP_OK = 3,
	MAC_AX_CHKSUM_OFD_CHKSUM_ERR = 4,
	MAC_AX_CHKSUM_OFD_HW_NO_SUPPORT = 5,
	MAC_AX_CHKSUM_OFD_INVALID = 6,
};

/**
 * @enum mac_ax_io_byte_sel
 *
 * @brief mac_ax_io_byte_sel
 *
 * @var mac_ax_io_byte_sel::MAC_AX_BYTE_SEL_1
 * Please Place Description here.
 * @var mac_ax_io_byte_sel::MAC_AX_BYTE_SEL_2
 * Please Place Description here.
 * @var mac_ax_io_byte_sel::MAC_AX_BYTE_SEL_4
 * Please Place Description here.
 * @var mac_ax_io_byte_sel::MAC_AX_BYTE_SEL_LAST
 * Please Place Description here.
 * @var mac_ax_io_byte_sel::MAC_AX_BYTE_SEL_MAX
 * Please Place Description here.
 * @var mac_ax_io_byte_sel::MAC_AX_BYTE_SEL_INVALID
 * Please Place Description here.
 */
enum mac_ax_io_byte_sel {
	MAC_AX_BYTE_SEL_1 = 0,
	MAC_AX_BYTE_SEL_2,
	MAC_AX_BYTE_SEL_4,

	/* keep last */
	MAC_AX_BYTE_SEL_LAST,
	MAC_AX_BYTE_SEL_MAX = MAC_AX_BYTE_SEL_LAST,
	MAC_AX_BYTE_SEL_INVALID = MAC_AX_BYTE_SEL_LAST,
};

/**
 * @enum RW_OFLD_BLOCK_ID
 *
 * @brief RW_OFLD_BLOCK_ID
 *
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_UART
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_CPU_LOCAL
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_SPIC
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_RXI300
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_AXIDMA
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_HIOE
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_IDDMA
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_IPSEC
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_DMAC_CTRL
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_SCR_MACHDR
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_STA_SCH_AIRTIME
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_STA_SCH_CAP
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_STA_DL_GRP_TBL
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_STA_UL_GRP_TBL
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_RX_FILTER_CAM
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_SEC_CAM
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_WOW_CAM
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_CMAC_CTRL
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_ADDR_CAM
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_BSSID_CAM
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_BA_CAM
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_BCN_IE_CAM
 * Please Place Description here.
 * @var RW_OFLD_BLOCK_ID::RW_OFLD_MAX
 * Please Place Description here.
 */
enum RW_OFLD_BLOCK_ID {
	RW_OFLD_UART = 0,
	RW_OFLD_CPU_LOCAL = 1,
	RW_OFLD_SPIC,
	RW_OFLD_RXI300,
	RW_OFLD_AXIDMA,
	RW_OFLD_HIOE,
	RW_OFLD_IDDMA,
	RW_OFLD_IPSEC,
	RW_OFLD_DMAC_CTRL,
	RW_OFLD_SCR_MACHDR,
	RW_OFLD_STA_SCH_AIRTIME,
	RW_OFLD_STA_SCH_CAP,
	RW_OFLD_STA_DL_GRP_TBL,
	RW_OFLD_STA_UL_GRP_TBL,
	RW_OFLD_RX_FILTER_CAM,
	RW_OFLD_SEC_CAM,
	RW_OFLD_WOW_CAM,
	RW_OFLD_CMAC_CTRL,
	RW_OFLD_ADDR_CAM,
	RW_OFLD_BSSID_CAM,
	RW_OFLD_BA_CAM,
	RW_OFLD_BCN_IE_CAM,
	RW_OFLD_MAX
};

/**
 * @enum mac_ax_cca
 *
 * @brief mac_ax_cca
 *
 * @var mac_ax_cca::MAC_AX_CCA
 * Please Place Description here.
 * @var mac_ax_cca::MAC_AX_SEC20_CCA
 * Please Place Description here.
 * @var mac_ax_cca::MAC_AX_SEC40_CCA
 * Please Place Description here.
 * @var mac_ax_cca::MAC_AX_SEC80_CCA
 * Please Place Description here.
 * @var mac_ax_cca::MAC_AX_EDCCA
 * Please Place Description here.
 * @var mac_ax_cca::MAC_AX_BTCCA
 * Please Place Description here.
 * @var mac_ax_cca::MAC_AX_CCA_LAST
 * Please Place Description here.
 * @var mac_ax_cca::MAC_AX_CCA_MAX
 * Please Place Description here.
 * @var mac_ax_cca::MAC_AX_CCA_INVALID
 * Please Place Description here.
 */
enum mac_ax_block_tx_sel {
	MAC_AX_CCA,
	MAC_AX_SEC20_CCA,
	MAC_AX_SEC40_CCA,
	MAC_AX_SEC80_CCA,
	MAC_AX_EDCCA,
	MAC_AX_BTCCA,
	MAC_AX_TX_NAV,

	/* keep last */
	MAC_AX_CCA_LAST,
	MAC_AX_CCA_MAX = MAC_AX_CCA_LAST,
	MAC_AX_CCA_INVALID = MAC_AX_CCA_LAST,
};

/**
 * struct mac_ax_pkt_data - packet information of data type
 * @hdr_len: Length of header+LLC.
 *	For example,
 *	1. 802.11 MPDU without encryption
 *	    HEADERwLLC_LEN = (MAC header (without IV ) + LLC ) / 2
 *	2. 802.11 MPDU encryption without HW_AES_IV
 *	    HEADERwLLC_LEN = (MAC header (without IV ) + LLC ) / 2
 *	3.802.11MPDU encryption with HW_AES_IV
 *	   HEADERwLLC_LEN = (MAC header(reserved IV length)+LLC)/2
 *	4.ETHERNET II MSDU without encryption
 *	   HEADERwLLC_LEN = (DA+SA+TYPE) /2
 *	5.ETHERNET II MSDU encryption without HW_AES_IV
 *	   HEADERwLLC_LEN = (DA+SA+TYPE) /2
 *	6.ETHERNET II MSDU encryption with HW_AES_IV
 *	   HEADERwLLC_LEN = (DA+SA+TYPE) /2
 *	7.SNAP MSDU without encryption
 *	   HEADERwLLC_LEN = (DA+SA+LEN+LLC) /2
 *	8.SNAP MSDU encryption without HW_AES_IV
 *	   HEADERwLLC_LEN = (DA+SA+LEN+LLC) /2
 *	9.SNAP MSDU encryption with HW_AES_IV
 *	   HEADERwLLC_LEN = (DA+SA+LEN+LLC) /2
 * @ch: Channel index, MAC_AX_CH_DMA_CH0~MAC_AX_CH_DMA_CH11.
 * @macid: MAC ID.
 */
/*--------------------Define Struct-------------------------------------*/

/**
 * @struct sec_checker
 * @brief sec_checker
 *
 * @var sec_checker::rx_desc_hw_dec
 * Please Place Description here.
 * @var sec_checker::rx_desc_sec_type
 * Please Place Description here.
 * @var sec_checker::rx_desc_icv_err
 * Please Place Description here.
 */
struct sec_checker {
	u8 rx_desc_hw_dec;
	u8 rx_desc_sec_type;
	u8 rx_desc_icv_err;
};

/**
 * @struct mac_ax_delay_tx_cfg
 * @brief mac_ax_delay_tx_cfg
 *
 * @var mac_ax_delay_tx_cfg::en
 * Please Place Description here.
 * @var mac_ax_delay_tx_cfg::vovi_to_b0
 * Please Place Description here.
 * @var mac_ax_delay_tx_cfg::bebk_to_b0
 * Please Place Description here.
 * @var mac_ax_delay_tx_cfg::vovi_to_b1
 * Please Place Description here.
 * @var mac_ax_delay_tx_cfg::bebk_to_b1
 * Please Place Description here.
 * @var mac_ax_delay_tx_cfg::vovi_len_b0
 * Please Place Description here.
 * @var mac_ax_delay_tx_cfg::bebk_len_b0
 * Please Place Description here.
 * @var mac_ax_delay_tx_cfg::vovi_len_b1
 * Please Place Description here.
 * @var mac_ax_delay_tx_cfg::bebk_len_b1
 * Please Place Description here.
 */
struct mac_ax_delay_tx_cfg {
	enum mac_ax_delay_tx_en en;
	u8 vovi_to_b0;
	u8 bebk_to_b0;
	u8 vovi_to_b1;
	u8 bebk_to_b1;
	u8 vovi_len_b0;
	u8 bebk_len_b0;
	u8 vovi_len_b1;
	u8 bebk_len_b1;
};

/**
 * @struct mac_ax_ofld_hdr
 * @brief mac_ax_ofld_hdr
 *
 * @var mac_ax_ofld_hdr::ls
 * Please Place Description here.
 * @var mac_ax_ofld_hdr::masken
 * Please Place Description here.
 * @var mac_ax_ofld_hdr::polling
 * Please Place Description here.
 * @var mac_ax_ofld_hdr::rsvd1
 * Please Place Description here.
 * @var mac_ax_ofld_hdr::value_len
 * Please Place Description here.
 * @var mac_ax_ofld_hdr::ofld_id
 * Please Place Description here.
 * @var mac_ax_ofld_hdr::entry_num
 * Please Place Description here.
 * @var mac_ax_ofld_hdr::offset
 * Please Place Description here.
 * @var mac_ax_ofld_hdr::rsvd2
 * Please Place Description here.
 */
struct mac_ax_ofld_hdr {
//Segment Hdr
	u16 ls:1;
	u16 masken:1;
	u16 polling:1;
	u16 rsvd1:2;
	u16 value_len:11;
	u8 ofld_id;
	u8 entry_num;
	u16 offset;
	u16 rsvd2;
};

/**
 * @struct mac_ax_outsrc_h2c_hdr
 * @brief mac_ax_outsrc_h2c_hdr
 *
 * @var mac_ax_outsrc_h2c_hdr::h2c_class
 * Please Place Description here.
 * @var mac_ax_outsrc_h2c_hdr::h2c_func
 * Please Place Description here.
 * @var mac_ax_outsrc_h2c_hdr::seq_valid
 * Please Place Description here.
 * @var mac_ax_outsrc_h2c_hdr::seq
 * Please Place Description here.
 * @var mac_ax_outsrc_h2c_hdr::seq_stop
 * Please Place Description here.
 * @var mac_ax_outsrc_h2c_hdr::rec_ack
 * Please Place Description here.
 * @var mac_ax_outsrc_h2c_hdr::done_ack
 * Please Place Description here.
 * @var mac_ax_outsrc_h2c_hdr::rsvd1
 * Please Place Description here.
 * @var mac_ax_outsrc_h2c_hdr::content_len
 * Please Place Description here.
 * @var mac_ax_outsrc_h2c_hdr::rsvd2
 * Please Place Description here.
 */
struct mac_ax_outsrc_h2c_hdr {
	u8 h2c_class; //0x0~0x7: Phydm; 0x8~0xF: RF; 0x10~0x17: BTC
	u8 h2c_func;
	u8 seq_valid:1;
	u8 seq:3;
	u8 seq_stop:1;
	u8 rec_ack:1; //Ack when receive H2C
	u8 done_ack:1; //Ack when FW execute H2C cmd done
	u8 rsvd1:1;
	u16 content_len:12;
	u16 rsvd2:4;
};

/**
 * @struct mac_ax_wmmps_info
 * @brief mac_ax_wmmps_info
 *
 * @var mac_ax_wmmps_info::listen_bcn_mode
 * Please Place Description here.
 * @var mac_ax_wmmps_info::awake_interval
 * Please Place Description here.
 * @var mac_ax_wmmps_info::vo_uapsd_en
 * Please Place Description here.
 * @var mac_ax_wmmps_info::vi_uapsd_en
 * Please Place Description here.
 * @var mac_ax_wmmps_info::be_uapsd_en
 * Please Place Description here.
 * @var mac_ax_wmmps_info::bk_uapsd_en
 * Please Place Description here.
 * @var mac_ax_wmmps_info::rsvd
 * Please Place Description here.
 */
struct mac_ax_wmmps_info {
	enum mac_ax_listern_bcn_mode listen_bcn_mode;
	u8 awake_interval;
	u8 vo_uapsd_en: 1;
	u8 vi_uapsd_en: 1;
	u8 be_uapsd_en: 1;
	u8 bk_uapsd_en: 1;
	u8 rsvd: 4;
};

/**
 * @struct mac_ax_err_status
 * @brief mac_ax_err_status
 *
 * @var mac_ax_err_status::err
 * Please Place Description here.
 * @var mac_ax_err_status::rst_en
 * Please Place Description here.
 */
struct mac_ax_err_status {
	enum mac_ax_err_info err;
	u8 rst_en;
};

#endif
