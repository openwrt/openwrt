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

#ifndef _MAC_AX_MAC_DEF_H_
#define _MAC_AX_MAC_DEF_H_

#include "pltfm_cfg.h"
#include "feature_cfg.h"
#include "chip_cfg.h"
#include "mac_ax/state_mach.h"
#include "errors.h"
#include "mac2drv_def.h"
#include "mac_exp_def.h"
#include "mac_outsrc_def.h"

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
#define PLTFM_SLEEP_US(us)                                                     \
	hal_usleep(adapter->drv_adapter, us)
#define PLTFM_SLEEP_MS(ms)                                                     \
	hal_msleep(adapter->drv_adapter, ms)
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
#define PLTFM_RECYCLE_H2C(buf)                                                 \
	adapter->pltfm_cb->rtl_recycle_h2c(adapter->phl_adapter, buf)
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
#define PLTFM_L2_NOTIFY(void)                                                          \
	adapter->pltfm_cb->ser_l2_notify(adapter->phl_adapter, adapter->drv_adapter)

#define PLTFM_LD_FW_SYMBOL(name, buf, buf_size) \
	adapter->pltfm_cb->ld_fw_symbol(adapter->phl_adapter, adapter->drv_adapter,\
	name, buf, buf_size)

#define PLTFM_MSG_PRINT(...)	\
	adapter->pltfm_cb->msg_print(drv_adapter, u8 dbg_level, __VA_ARGS__)
#define PLTFM_GET_CHIP_ID(void)                                                \
	adapter->pltfm_cb->get_chip_id(adapter->drv_adapter)

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

/*--------------------Define MACRO--------------------------------------*/
#define MAC_AX_MAX_RU_NUM	8
#define WLAN_ADDR_LEN			6
#define MAX_VHT_SUPPORT_SOUND_STA	4
#define MAX_HE_SUPPORT_SOUND_STA	16
#define MAC_AX_BCN_INTERVAL_DEFAULT 100
#define MAC_RX_USB_AGG_MODE_UNIT	4096
#define MAC_RX_DMA_AGG_MODE_UNIT	1024
#define MAC_MAX_ARGC		20
#define MAC_MAX_ARGV		16
#define MAC_AX_DP_SEL_NUM	2

#ifdef PHL_FEATURE_AP
#define MAC_STA_NUM	128
#else /*for NIC mode setting*/
#define MAC_STA_NUM	32
#endif

#define MAC_AX_FAST_CH_SW_MAX_STA_NUM 4
#define SCANOFLD_MAX_ADDITION_PKT_NUM 8
#define SCANOFLD_MAX_TARGET_PKT_NUM 4
#define SCANOFLD_MAX_SSID_NUM 16
#define SCANOFLD_MAX_SSID_LEN 32
#define UL_PER_STA_DBGINFO_NUM 0x10

#define SS_LINK_SIZE 256

#define DLRU_MAX_USER_STS 0x10
#define lps_pwr_state ps_pwr_state //temp, should remove

#define MAC_AX_IECAM_NUM	12

#define BASE_BOARD_ID_LEN 32

/*--------------------Define Enum---------------------------------------*/

/**
 * @enum mac_ax_intf
 *
 * @brief mac_ax_intf
 *
 * @var mac_ax_intf::MAC_AX_INTF_USB
 * Please Place Description here.
 * @var mac_ax_intf::MAC_AX_INTF_SDIO
 * Please Place Description here.
 * @var mac_ax_intf::MAC_AX_INTF_PCIE
 * Please Place Description here.
 * @var mac_ax_intf::MAC_AX_INTF_LAST
 * Please Place Description here.
 * @var mac_ax_intf::MAC_AX_INTF_MAX
 * Please Place Description here.
 * @var mac_ax_intf::MAC_AX_INTF_INVALID
 * Please Place Description here.
 */
enum mac_ax_intf {
	MAC_AX_INTF_USB,
	MAC_AX_INTF_SDIO,
	MAC_AX_INTF_PCIE,

	/* keep last */
	MAC_AX_INTF_LAST,
	MAC_AX_INTF_MAX = MAC_AX_INTF_LAST,
	MAC_AX_INTF_INVALID = MAC_AX_INTF_LAST,
};

/**
 * @enum mac_ax_feature
 *
 * @brief mac_ax_feature
 *
 * @var mac_ax_feature::MAC_AX_FT_DUMP_EFUSE
 * Please Place Description here.
 * @var mac_ax_feature::MAC_AX_FT_LAST
 * Please Place Description here.
 * @var mac_ax_feature::MAC_AX_FT_MAX
 * Please Place Description here.
 * @var mac_ax_feature::MAC_AX_FT_INVALID
 * Please Place Description here.
 */
enum mac_ax_feature {
	MAC_AX_FT_DUMP_EFUSE,

	/* keep last */
	MAC_AX_FT_LAST,
	MAC_AX_FT_MAX = MAC_AX_FT_LAST,
	MAC_AX_FT_INVALID = MAC_AX_FT_LAST,
};

/**
 * @enum mac_ax_status
 *
 * @brief mac_ax_status
 *
 * @var mac_ax_status::MAC_AX_STATUS_IDLE
 * Please Place Description here.
 * @var mac_ax_status::MAC_AX_STATUS_PROC
 * Please Place Description here.
 * @var mac_ax_status::MAC_AX_STATUS_DONE
 * Please Place Description here.
 * @var mac_ax_status::MAC_AX_STATUS_ERR
 * Please Place Description here.
 */
enum mac_ax_status {
	MAC_AX_STATUS_IDLE,
	MAC_AX_STATUS_PROC,
	MAC_AX_STATUS_DONE,
	MAC_AX_STATUS_ERR,
};

/**
 * @enum mac_ax_sdio_4byte_mode
 *
 * @brief mac_ax_sdio_4byte_mode
 *
 * @var mac_ax_sdio_4byte_mode::MAC_AX_SDIO_4BYTE_MODE_DISABLE
 * Please Place Description here.
 * @var mac_ax_sdio_4byte_mode::MAC_AX_SDIO_4BYTE_MODE_RW
 * Please Place Description here.
 * @var mac_ax_sdio_4byte_mode::MAC_AX_SDIO_4BYTE_MODE_LAST
 * Please Place Description here.
 * @var mac_ax_sdio_4byte_mode::MAC_AX_SDIO_4BYTE_MODE_MAX
 * Please Place Description here.
 * @var mac_ax_sdio_4byte_mode::MAC_AX_SDIO_4BYTE_MODE_INVALID
 * Please Place Description here.
 */
enum mac_ax_sdio_4byte_mode {
	MAC_AX_SDIO_4BYTE_MODE_DISABLE,
	MAC_AX_SDIO_4BYTE_MODE_RW,

	/* keep last */
	MAC_AX_SDIO_4BYTE_MODE_LAST,
	MAC_AX_SDIO_4BYTE_MODE_MAX = MAC_AX_SDIO_4BYTE_MODE_LAST,
	MAC_AX_SDIO_4BYTE_MODE_INVALID = MAC_AX_SDIO_4BYTE_MODE_LAST,
};

/**
 * @enum mac_ax_sdio_tx_mode
 *
 * @brief mac_ax_sdio_tx_mode
 *
 * @var mac_ax_sdio_tx_mode::MAC_AX_SDIO_TX_MODE_AGG
 * Please Place Description here.
 * @var mac_ax_sdio_tx_mode::MAC_AX_SDIO_TX_MODE_DUMMY_BLOCK
 * Please Place Description here.
 * @var mac_ax_sdio_tx_mode::MAC_AX_SDIO_TX_MODE_DUMMY_AUTO
 * Please Place Description here.
 * @var mac_ax_sdio_tx_mode::MAC_AX_SDIO_TX_MODE_LAST
 * Please Place Description here.
 * @var mac_ax_sdio_tx_mode::MAC_AX_SDIO_TX_MODE_MAX
 * Please Place Description here.
 * @var mac_ax_sdio_tx_mode::MAC_AX_SDIO_TX_MODE_INVALID
 * Please Place Description here.
 */
enum mac_ax_sdio_tx_mode {
	MAC_AX_SDIO_TX_MODE_AGG,
	MAC_AX_SDIO_TX_MODE_DUMMY_BLOCK,
	MAC_AX_SDIO_TX_MODE_DUMMY_AUTO,

	/* keep last */
	MAC_AX_SDIO_TX_MODE_LAST,
	MAC_AX_SDIO_TX_MODE_MAX = MAC_AX_SDIO_TX_MODE_LAST,
	MAC_AX_SDIO_TX_MODE_INVALID = MAC_AX_SDIO_TX_MODE_LAST,
};

/**
 * @enum mac_ax_sdio_opn_mode
 *
 * @brief mac_ax_sdio_opn_mode
 *
 * @var mac_ax_sdio_opn_mode::MAC_AX_SDIO_OPN_MODE_BYTE
 * Please Place Description here.
 * @var mac_ax_sdio_opn_mode::MAC_AX_SDIO_OPN_MODE_BLOCK
 * Please Place Description here.
 * @var mac_ax_sdio_opn_mode::MAC_AX_SDIO_OPN_MODE_UNKNOWN
 * Please Place Description here.
 */
enum mac_ax_sdio_opn_mode {
	MAC_AX_SDIO_OPN_MODE_BYTE = 0,
	MAC_AX_SDIO_OPN_MODE_BLOCK,
	MAC_AX_SDIO_OPN_MODE_UNKNOWN,
};

/**
 * @enum mac_ax_sdio_spec_ver
 *
 * @brief mac_ax_sdio_spec_ver
 *
 * @var mac_ax_sdio_spec_ver::MAC_AX_SDIO_SPEC_VER_2_00
 * Please Place Description here.
 * @var mac_ax_sdio_spec_ver::MAC_AX_SDIO_SPEC_VER_3_00
 * Please Place Description here.
 * @var mac_ax_sdio_spec_ver::MAC_AX_SDIO_SPEC_VER_LAST
 * Please Place Description here.
 * @var mac_ax_sdio_spec_ver::MAC_AX_SDIO_SPEC_VER_MAX
 * Please Place Description here.
 * @var mac_ax_sdio_spec_ver::MAC_AX_SDIO_SPEC_VER_INVALID
 * Please Place Description here.
 */
enum mac_ax_sdio_spec_ver {
	MAC_AX_SDIO_SPEC_VER_2_00,
	MAC_AX_SDIO_SPEC_VER_3_00,

	/* keep last */
	MAC_AX_SDIO_SPEC_VER_LAST,
	MAC_AX_SDIO_SPEC_VER_MAX = MAC_AX_SDIO_SPEC_VER_LAST,
	MAC_AX_SDIO_SPEC_VER_INVALID = MAC_AX_SDIO_SPEC_VER_LAST,
};

/**
 * @enum mac_ax_use_ver
 *
 * @brief mac_ax_use_ver
 *
 * @var mac_ax_use_ver::MAC_AX_USB11
 * Please Place Description here.
 * @var mac_ax_use_ver::MAC_AX_USB2
 * Please Place Description here.
 * @var mac_ax_use_ver::MAC_AX_USB3
 * Please Place Description here.
 * @var mac_ax_use_ver::MAC_AX_USB_LAST
 * Please Place Description here.
 * @var mac_ax_use_ver::MAC_AX_USB_MAX
 * Please Place Description here.
 * @var mac_ax_use_ver::MAC_AX_USB_INVALID
 * Please Place Description here.
 */
enum mac_ax_use_ver {
	MAC_AX_USB10,
	MAC_AX_USB11,
	MAC_AX_USB2,
	MAC_AX_USB3,

	MAC_AX_USB_LAST,
	MAC_AX_USB_MAX = MAC_AX_USB_LAST,
	MAC_AX_USB_INVALID = MAC_AX_USB_LAST,
};

/**
 * @enum mac_ax_use_mode
 *
 * @brief mac_ax_use_mode
 *
 * @var mac_ax_use_ver::MAC_AX_USB_NORM
 * Please Place Description here.
 * @var mac_ax_use_ver::MAC_AX_USB_AUTOINSTALL
 * Please Place Description here.
 * @var mac_ax_use_ver::MAC_AX_USB_LAST
 * Please Place Description here.
 * @var mac_ax_use_ver::MAC_AX_USB_MAX
 * Please Place Description here.
 * @var mac_ax_use_ver::MAC_AX_USB_INVALID
 * Please Place Description here.
 */
enum mac_ax_use_mode {
	MAC_AX_USB_NORM,
	MAC_AX_USB_AUTOINSTALL,

	MAC_AX_USB_MODE_LAST,
	MAC_AX_USB_MODE_MAX = MAC_AX_USB_LAST,
	MAC_AX_USB_MODE_INVALID = MAC_AX_USB_LAST,
};

/**
 * @enum mac_ax_lv1_rcvy_step
 *
 * @brief mac_ax_lv1_rcvy_step
 *
 * @var mac_ax_lv1_rcvy_step::MAC_AX_LV1_RCVY_STEP_1
 * Please Place Description here.
 * @var mac_ax_lv1_rcvy_step::MAC_AX_LV1_RCVY_STEP_2
 * Please Place Description here.
 * @var mac_ax_lv1_rcvy_step::MAC_AX_LV1_RCVY_STEP_LAST
 * Please Place Description here.
 * @var mac_ax_lv1_rcvy_step::MAC_AX_LV1_RCVY_STEP_MAX
 * Please Place Description here.
 * @var mac_ax_lv1_rcvy_step::MAC_AX_LV1_RCVY_STEP_INVALID
 * Please Place Description here.
 */
enum mac_ax_lv1_rcvy_step {
	MAC_AX_LV1_RCVY_STEP_1 = 0,
	MAC_AX_LV1_RCVY_STEP_2,

	/* keep last */
	MAC_AX_LV1_RCVY_STEP_LAST,
	MAC_AX_LV1_RCVY_STEP_MAX = MAC_AX_LV1_RCVY_STEP_LAST,
	MAC_AX_LV1_RCVY_STEP_INVALID = MAC_AX_LV1_RCVY_STEP_LAST,
};

/**
 * @enum mac_ax_ex_shift
 *
 * @brief mac_ax_ex_shift
 *
 * @var mac_ax_ex_shift::MAC_AX_NO_SHIFT
 * Please Place Description here.
 * @var mac_ax_ex_shift::MAC_AX_BYTE_ALIGNED_4
 * Please Place Description here.
 * @var mac_ax_ex_shift::MAC_AX_BYTE_ALIGNED_8
 * Please Place Description here.
 */
enum mac_ax_ex_shift {
	MAC_AX_NO_SHIFT    = 0,
	MAC_AX_BYTE_ALIGNED_4 = 1,
	MAC_AX_BYTE_ALIGNED_8   = 2
};

/**
 * @enum mac_ax_ps_mode
 *
 * @brief mac_ax_ps_mode
 *
 * @var mac_ax_ps_mode::MAC_AX_PS_MODE_ACTIVE
 * Please Place Description here.
 * @var mac_ax_ps_mode::MAC_AX_PS_MODE_LEGACY
 * Please Place Description here.
 * @var mac_ax_ps_mode::MAC_AX_PS_MODE_WMMPS
 * Please Place Description here.
 * @var mac_ax_ps_mode::MAC_AX_PS_MODE_MAX
 * Please Place Description here.
 */
enum mac_ax_ps_mode {
	MAC_AX_PS_MODE_ACTIVE = 0,
	MAC_AX_PS_MODE_LEGACY = 1,
	MAC_AX_PS_MODE_WMMPS  = 2,
	MAC_AX_PS_MODE_MAX    = 3,
};

/**
 * @enum mac_ax_pwr_state_action
 *
 * @brief mac_ax_pwr_state_action
 *
 * @var mac_ax_pwr_state_action::MAC_AX_PWR_STATE_ACT_REQ
 * Please Place Description here.
 * @var mac_ax_pwr_state_action::MAC_AX_PWR_STATE_ACT_CHK
 * Please Place Description here.
 * @var mac_ax_pwr_state_action::MAC_AX_PWR_STATE_ACT_MAX
 * Please Place Description here.
 */
enum mac_ax_pwr_state_action {
	MAC_AX_PWR_STATE_ACT_REQ = 0,
	MAC_AX_PWR_STATE_ACT_CHK = 1,
	MAC_AX_PWR_STATE_ACT_MAX,
};

/**
 * @enum mac_ax_rpwm_req_pwr_state
 *
 * @brief mac_ax_rpwm_req_pwr_state
 *
 * @var mac_ax_rpwm_req_pwr_state::MAC_AX_RPWM_REQ_PWR_STATE_ACTIVE
 * Please Place Description here.
 * @var mac_ax_rpwm_req_pwr_state::MAC_AX_RPWM_REQ_PWR_STATE_BAND0_RFON
 * Please Place Description here.
 * @var mac_ax_rpwm_req_pwr_state::MAC_AX_RPWM_REQ_PWR_STATE_BAND1_RFON
 * Please Place Description here.
 * @var mac_ax_rpwm_req_pwr_state::MAC_AX_RPWM_REQ_PWR_STATE_BAND0_RFOFF
 * Please Place Description here.
 * @var mac_ax_rpwm_req_pwr_state::MAC_AX_RPWM_REQ_PWR_STATE_BAND1_RFOFF
 * Please Place Description here.
 * @var mac_ax_rpwm_req_pwr_state::MAC_AX_RPWM_REQ_PWR_STATE_CLK_GATED
 * Please Place Description here.
 * @var mac_ax_rpwm_req_pwr_state::MAC_AX_RPWM_REQ_PWR_STATE_PWR_GATED
 * Please Place Description here.
 * @var mac_ax_rpwm_req_pwr_state::MAC_AX_RPWM_REQ_PWR_STATE_HIOE_PWR_GATED
 * Please Place Description here.
 * @var mac_ax_rpwm_req_pwr_state::MAC_AX_RPWM_REQ_PWR_STATE_MAX
 * Please Place Description here.
 */
enum mac_ax_rpwm_req_pwr_state {
	MAC_AX_RPWM_REQ_PWR_STATE_ACTIVE = 0,
	MAC_AX_RPWM_REQ_PWR_STATE_BAND0_RFON = 1,
	MAC_AX_RPWM_REQ_PWR_STATE_BAND1_RFON = 2,
	MAC_AX_RPWM_REQ_PWR_STATE_BAND0_RFOFF = 3,
	MAC_AX_RPWM_REQ_PWR_STATE_BAND1_RFOFF = 4,
	MAC_AX_RPWM_REQ_PWR_STATE_CLK_GATED = 5,
	MAC_AX_RPWM_REQ_PWR_STATE_PWR_GATED = 6,
	MAC_AX_RPWM_REQ_PWR_STATE_HIOE_PWR_GATED = 7,
	MAC_AX_RPWM_REQ_PWR_STATE_MAX,
};

/**
 * @enum mac_ax_port_cfg_type
 *
 * @brief mac_ax_port_cfg_type
 *
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_FUNC_SW
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_TX_SW
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_TX_RPT
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_RX_SW
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_RX_RPT
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_RX_SYNC
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_BCN_PRCT
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_TBTT_AGG
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_TBTT_SHIFT
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_RST_TSF
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_RST_TPR
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_BCAID
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_HIQ_WIN
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_HIQ_DTIM
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_HIQ_NOLIMIT
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_NET_TYPE
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_BCN_INTV
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_BCN_SETUP_TIME
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_BCN_HOLD_TIME
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_MBSSID_EN
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_BCN_ERLY
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_BCN_MASK_AREA
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_TBTT_ERLY
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_BSS_CLR
 * Please Place Description here.
 * @var mac_ax_port_cfg_type::MAC_AX_PCFG_BCN_DRP_ALL
 * Please Place Description here.
 */
enum mac_ax_port_cfg_type {
	MAC_AX_PCFG_FUNC_SW = 0,
	MAC_AX_PCFG_TX_SW,
	MAC_AX_PCFG_TX_RPT,
	MAC_AX_PCFG_RX_SW,
	MAC_AX_PCFG_RX_RPT,
	MAC_AX_PCFG_RX_SYNC,
	MAC_AX_PCFG_BCN_PRCT,
	MAC_AX_PCFG_TBTT_AGG,
	MAC_AX_PCFG_TBTT_SHIFT,
	MAC_AX_PCFG_RST_TSF,
	MAC_AX_PCFG_RST_TPR,
	MAC_AX_PCFG_BCAID,
	MAC_AX_PCFG_HIQ_WIN,
	MAC_AX_PCFG_HIQ_DTIM,
	MAC_AX_PCFG_HIQ_NOLIMIT,
	MAC_AX_PCFG_NET_TYPE,
	MAC_AX_PCFG_BCN_INTV,
	MAC_AX_PCFG_BCN_SETUP_TIME,
	MAC_AX_PCFG_BCN_HOLD_TIME,
	MAC_AX_PCFG_MBSSID_EN,
	MAC_AX_PCFG_BCN_ERLY,
	MAC_AX_PCFG_BCN_MASK_AREA,
	MAC_AX_PCFG_TBTT_ERLY,
	MAC_AX_PCFG_BSS_CLR,
	MAC_AX_PCFG_MBSSID_NUM,
	MAC_AX_PCFG_BCN_DRP_ALL,
};

/**
 * @enum mac_ax_band
 *
 * @brief mac_ax_band
 *
 * @var mac_ax_band::MAC_AX_BAND_0
 * Please Place Description here.
 * @var mac_ax_band::MAC_AX_BAND_1
 * Please Place Description here.
 * @var mac_ax_band::MAC_AX_BAND_NUM
 * Please Place Description here.
 */
enum mac_ax_band {
	MAC_AX_BAND_0 = 0,
	MAC_AX_BAND_1 = 1,
	MAC_AX_BAND_NUM = 2
};

/**
 * @enum mac_ax_port
 *
 * @brief mac_ax_port
 *
 * @var mac_ax_port::MAC_AX_PORT_0
 * Please Place Description here.
 * @var mac_ax_port::MAC_AX_PORT_1
 * Please Place Description here.
 * @var mac_ax_port::MAC_AX_PORT_2
 * Please Place Description here.
 * @var mac_ax_port::MAC_AX_PORT_3
 * Please Place Description here.
 * @var mac_ax_port::MAC_AX_PORT_4
 * Please Place Description here.
 * @var mac_ax_port::MAC_AX_PORT_NUM
 * Please Place Description here.
 */
enum mac_ax_port {
	MAC_AX_PORT_0 = 0,
	MAC_AX_PORT_1 = 1,
	MAC_AX_PORT_2 = 2,
	MAC_AX_PORT_3 = 3,
	MAC_AX_PORT_4 = 4,
	MAC_AX_PORT_NUM
};

/**
 * @enum mac_ax_addr_msk_sel
 *
 * @brief mac_ax_addr_msk_sel
 *
 * @var mac_ax_addr_msk_sel::MAC_AX_NO_MSK
 * Please Place Description here.
 * @var mac_ax_addr_msk_sel::MAC_AX_SMA_MSK
 * Please Place Description here.
 * @var mac_ax_addr_msk_sel::MAC_AX_TMA_MSK
 * Please Place Description here.
 * @var mac_ax_addr_msk_sel::MAC_AX_BSSID_MSK
 * Please Place Description here.
 */
enum mac_ax_addr_msk_sel {
	MAC_AX_NO_MSK,
	MAC_AX_SMA_MSK,
	MAC_AX_TMA_MSK,
	MAC_AX_BSSID_MSK
};

/**
 * @enum mac_ax_addr_msk
 *
 * @brief mac_ax_addr_msk
 *
 * @var mac_ax_addr_msk::MAC_AX_MASK_BYTE5_TO_BYTE5
 * Please Place Description here.
 * @var mac_ax_addr_msk::MAC_AX_MASK_BYTE5_TO_BYTE4
 * Please Place Description here.
 * @var mac_ax_addr_msk::MAC_AX_MASK_BYTE5_TO_BYTE3
 * Please Place Description here.
 * @var mac_ax_addr_msk::MAC_AX_MASK_BYTE5_TO_BYTE2
 * Please Place Description here.
 * @var mac_ax_addr_msk::MAC_AX_MASK_BYTE5_TO_BYTE1
 * Please Place Description here.
 */
enum mac_ax_addr_msk {
	MAC_AX_MSK_NONE = 0x3f,
	MAC_AX_BYTE5 = 0x1f,
	MAC_AX_BYTE5_TO_BYTE4 = 0xf,
	MAC_AX_BYTE5_TO_BYTE3 = 0x7,
	MAC_AX_BYTE5_TO_BYTE2 = 0x3,
	MAC_AX_BYTE5_TO_BYTE1 = 0x1,
	MAC_AX_MSK_ALL = 0x0
};

/**
 * @enum mac_ax_mbssid_idx
 *
 * @brief mac_ax_mbssid_idx
 *
 * @var mac_ax_mbssid_idx::MAC_AX_P0_ROOT
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID1
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID2
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID3
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID4
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID5
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID6
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID7
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID8
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID9
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID10
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID11
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID12
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID13
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID14
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID15
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID_LAST
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID_MAX
 * Please Place Description here.
 * @var mac_ax_mbssid_idx::MAC_AX_P0_MBID_INVALID
 * Please Place Description here.
 */
enum mac_ax_mbssid_idx {
	MAC_AX_P0_ROOT = 0,
	MAC_AX_P0_MBID1,
	MAC_AX_P0_MBID2,
	MAC_AX_P0_MBID3,
	MAC_AX_P0_MBID4,
	MAC_AX_P0_MBID5,
	MAC_AX_P0_MBID6,
	MAC_AX_P0_MBID7,
	MAC_AX_P0_MBID8,
	MAC_AX_P0_MBID9,
	MAC_AX_P0_MBID10,
	MAC_AX_P0_MBID11,
	MAC_AX_P0_MBID12,
	MAC_AX_P0_MBID13,
	MAC_AX_P0_MBID14,
	MAC_AX_P0_MBID15,

	/* keep last */
	MAC_AX_P0_MBID_LAST,
	MAC_AX_P0_MBID_MAX = MAC_AX_P0_MBID_LAST,
	MAC_AX_P0_MBID_INVALID = MAC_AX_P0_MBID_LAST,
};

/**
 * @enum mac_ax_hwmod_sel
 *
 * @brief mac_ax_hwmod_sel
 *
 * @var mac_ax_hwmod_sel::MAC_AX_DMAC_SEL
 * Please Place Description here.
 * @var mac_ax_hwmod_sel::MAC_AX_CMAC_SEL
 * Please Place Description here.
 * @var mac_ax_hwmod_sel::MAC_AX_MAC_LAST
 * Please Place Description here.
 * @var mac_ax_hwmod_sel::MAC_AX_MAC_MAX
 * Please Place Description here.
 * @var mac_ax_hwmod_sel::MAC_AX_MAC_INVALID
 * Please Place Description here.
 */
enum mac_ax_hwmod_sel {
	MAC_AX_DMAC_SEL = 0,
	MAC_AX_CMAC_SEL = 1,

	/* keep last */
	MAC_AX_MAC_LAST,
	MAC_AX_MAC_MAX = MAC_AX_MAC_LAST,
	MAC_AX_MAC_INVALID = MAC_AX_MAC_LAST,
};

/**
 * @enum mac_ax_ss_wmm
 *
 * @brief mac_ax_ss_wmm
 *
 * @var mac_ax_ss_wmm::MAC_AX_SS_WMM0
 * Please Place Description here.
 * @var mac_ax_ss_wmm::MAC_AX_SS_WMM1
 * Please Place Description here.
 * @var mac_ax_ss_wmm::MAC_AX_SS_WMM2
 * Please Place Description here.
 * @var mac_ax_ss_wmm::MAC_AX_SS_WMM3
 * Please Place Description here.
 * @var mac_ax_ss_wmm::MAC_AX_SS_UL
 * Please Place Description here.
 */
enum mac_ax_ss_wmm {
	MAC_AX_SS_WMM0,
	MAC_AX_SS_WMM1,
	MAC_AX_SS_WMM2,
	MAC_AX_SS_WMM3,
	MAC_AX_SS_UL,
};

/**
 * @enum mac_ax_ss_quota_mode
 *
 * @brief mac_ax_ss_quota_mode
 *
 * @var mac_ax_ss_quota_mode::MAC_AX_SS_QUOTA_MODE_TIME
 * Please Place Description here.
 * @var mac_ax_ss_quota_mode::MAC_AX_SS_QUOTA_MODE_CNT
 * Please Place Description here.
 */
enum mac_ax_ss_quota_mode {
	MAC_AX_SS_QUOTA_MODE_TIME = 0,
	MAC_AX_SS_QUOTA_MODE_CNT = 1,
};

/**
 * @enum mac_ax_issue_uldl_type
 *
 * @brief mac_ax_issue_uldl_type
 *
 * @var mac_ax_issue_uldl_type::mac_ax_issue_dl
 * Please Place Description here.
 * @var mac_ax_issue_uldl_type::mac_ax_issue_ul
 * Please Place Description here.
 */
enum mac_ax_issue_uldl_type {
	mac_ax_issue_dl = 0,
	mac_ax_issue_ul = 1,
};

/**
 * @enum mac_ax_hw_id
 *
 * @brief mac_ax_hw_id
 *
 * @var mac_ax_hw_id::MAC_AX_HW_MAPPING
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SDIO_MON_INT
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SDIO_MON_CNT
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SDIO_TX_AGG_SIZE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_AMPDU_CFG
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_EDCA_PARAM
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_EDCCA_PARAM
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_MUEDCA_PARAM
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_MUEDCA_TIMER
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_TBPPDU_CTRL
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_MUEDCA_CTRL
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_DELAYTX_CFG
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_SS_WMM_TBL
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_EFUSE_SIZE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_LOGICAL_EFUSE_SIZE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_LIMIT_LOG_EFUSE_SIZE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_BT_EFUSE_SIZE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_BT_LOGICAL_EFUSE_SIZE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_EFUSE_MASK_SIZE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_LIMIT_EFUSE_MASK_SIZE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_BT_EFUSE_MASK_SIZE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_CH_STAT_CNT
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_LIFETIME_CFG
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_APP_FCS
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_RX_ICVERR
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_PWR_STATE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_WAKE_REASON
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_SCOREBOARD
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_COEX_GNT
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_RRSR
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_COEX_CTRL
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_TX_CNT
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_TX_TF_INFO
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_TSF
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_MAX_TX_TIME
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_SS_QUOTA_MODE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_SS_QUOTA_SETTING
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_POLLUTED_CNT
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_DATA_RTY_LMT
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_DFLT_NAV
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_BACAM_MODE_SEL
 * To get the BACAM option mode.
 * @var mac_ax_hw_id::MAC_AX_HW_GET_RRSR_CFG
 * for Get Response rate cfg
 * @var mac_ax_hw_id::MAC_AX_HW_GET_CTS_RRSR_CFG
 * for Get CTS Response rate cfg
 * @var mac_ax_hw_id::MAC_AX_HW_SDIO_INFO
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SDIO_TX_MODE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SDIO_RX_AGG
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SDIO_TX_AGG
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SDIO_AVAL_PAGE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SDIO_MON_WT
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SDIO_MON_CLK
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_PCIE_CFGSPC_SET
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_PCIE_RST_BDRAM
 * Please Place Description here.
 * @var mac_ax_hw_id::MAX_AX_HW_PCIE_LTR_SW_TRIGGER
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_ID_PAUSE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_AMPDU_CFG
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_USR_EDCA_PARAM
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_EDCA_PARAM
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_EDCCA_PARAM
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_MUEDCA_PARAM
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_TBPPDU_CTRL
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_SCH_TXEN_CFG
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_HOST_RPR
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_MUEDCA_CTRL
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_DELAYTX_CFG
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_BW_CFG
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_BT_BLOCK_TX
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_LIFETIME_CFG
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_EN_BB_RF
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_APP_FCS
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_RX_ICVERR
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_CCTL_RTY_LMT
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_COEX_GNT
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_SCOREBOARD
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_POLLUTED
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_COEX_CTRL
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_CLR_TX_CNT
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_SLOT_TIME
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_XTAL_AAC_MODE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_NAV_PADDING
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_MAX_TX_TIME
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_SS_QUOTA_MODE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_SS_QUOTA_SETTING
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_TX_RU26_TB
 * To enable or disable responding TB in RU26
 * @var mac_ax_hw_id::MAC_AX_HW_SET_BACAM_MODE_SEL
 * To change the BACAM option mode
 * @var mac_ax_hw_id::MAC_AX_HW_SET_CORE_SWR_VOLT
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_PARTIAL_PLD_MODE
 * Please Place Description here.
 * @var mac_ax_hw_id::MAC_AX_HW_SET_RRSR_CFG
 * for Response rate cfg
 * @var mac_ax_hw_id::MAC_AX_HW_SET_CTS_RRSR_CFG
 * for CTS Response rate cfg
 */
enum mac_ax_hw_id {
	/* Get HW value */
	MAC_AX_HW_MAPPING = 0x00,
	MAC_AX_HW_SDIO_MON_INT,
	MAC_AX_HW_SDIO_MON_CNT,
	MAC_AX_HW_GET_ID_PAUSE,
	MAC_AX_HW_SDIO_TX_AGG_SIZE,
	MAC_AX_HW_GET_EDCA_PARAM,
	MAC_AX_HW_GET_TBPPDU_CTRL,
	MAC_AX_HW_GET_SCH_TXEN_STATUS,
	MAC_AX_HW_GET_DELAYTX_CFG,
	MAC_AX_HW_GET_SS_WMM_TBL,
	MAC_AX_HW_GET_EFUSE_SIZE,
	MAC_AX_HW_GET_LOGICAL_EFUSE_SIZE,
	MAC_AX_HW_GET_LIMIT_LOG_EFUSE_SIZE,
	MAC_AX_HW_GET_BT_EFUSE_SIZE,
	MAC_AX_HW_GET_BT_LOGICAL_EFUSE_SIZE,
	MAC_AX_HW_GET_EFUSE_MASK_SIZE,
	MAC_AX_HW_GET_LIMIT_EFUSE_MASK_SIZE,
	MAC_AX_HW_GET_BT_EFUSE_MASK_SIZE,
	MAC_AX_HW_GET_EFUSE_VERSION_SIZE,
	MAC_AX_HW_GET_DAV_LOG_EFUSE_SIZE,
	MAC_AX_HW_GET_CH_STAT_CNT,
	MAC_AX_HW_GET_LIFETIME_CFG,
	MAC_AX_HW_GET_APP_FCS,
	MAC_AX_HW_GET_RX_ICVERR,
	MAC_AX_HW_GET_PWR_STATE,
	MAC_AX_HW_GET_WAKE_REASON,
	MAC_AX_HW_GET_SCOREBOARD,
	MAC_AX_HW_GET_COEX_GNT,
	MAC_AX_HW_GET_COEX_CTRL,
	MAC_AX_HW_GET_TX_CNT,
	MAC_AX_HW_GET_TSF,
	MAC_AX_HW_GET_MAX_TX_TIME,
	MAC_AX_HW_GET_POLLUTED_CNT,
	MAC_AX_HW_GET_DATA_RTY_LMT,
	MAC_AX_HW_GET_DFLT_NAV,
	MAC_AX_HW_GET_SCHE_PREBKF,
	MAC_AX_HW_GET_FW_CAP,
	MAC_AX_HW_GET_BACAM_MODE_SEL,
	MAC_AX_HW_GET_RRSR_CFG,
	MAC_AX_HW_GET_CTS_RRSR_CFG,
	MAC_AX_HW_GET_USB_STS,
	MAC_AX_HW_GET_WD_PAGE_NUM,
	MAC_AX_HW_GET_SDIO_RX_REQ_LEN,
	MAC_AX_HW_GET_SDIO_LPS_FLG,
	MAC_AX_HW_GET_FREERUN_CNT,
	/* Set HW value */
	MAC_AX_HW_SETTING = 0x60,
	MAC_AX_HW_SDIO_INFO,
	MAC_AX_HW_SDIO_TX_MODE,
	MAC_AX_HW_SDIO_RX_AGG,
	MAC_AX_HW_SDIO_TX_AGG,
	MAC_AX_HW_SDIO_AVAL_PAGE,
	MAC_AX_HW_SDIO_MON_WT,
	MAC_AX_HW_SDIO_MON_CLK,
	MAC_AX_HW_PCIE_CFGSPC_SET,
	MAC_AX_HW_PCIE_RST_BDRAM,
	MAX_AX_HW_PCIE_LTR_SW_TRIGGER,
	MAX_AX_HW_PCIE_MIT,
	MAX_AX_HW_PCIE_L2_LEAVE,
	MAC_AX_HW_PCIE_DRIVING_MPONLY,
	MAC_AX_HW_SET_ID_PAUSE,
	MAC_AX_HW_SET_MULTI_ID_PAUSE,
	MAC_AX_HW_SET_ID_PAUSE_SLEEP,
	MAC_AX_HW_SET_MULTI_ID_PAUSE_SLEEP,
	MAC_AX_HW_SET_AMPDU_CFG,
	MAC_AX_HW_SET_USR_EDCA_PARAM,
	MAC_AX_HW_SET_USR_TX_RPT_CFG,
	MAC_AX_HW_SET_EDCA_PARAM,
	MAC_AX_HW_SET_EDCCA_PARAM,
	MAC_AX_HW_SET_MUEDCA_PARAM,
	MAC_AX_HW_SET_TBPPDU_CTRL,
	MAC_AX_HW_SET_SCH_TXEN_CFG,
	MAC_AX_HW_SET_HOST_RPR,
	MAC_AX_HW_SET_MUEDCA_CTRL,
	MAC_AX_HW_SET_DELAYTX_CFG,
	MAC_AX_HW_SET_BW_CFG,
	MAC_AX_HW_SET_CH_BUSY_STAT_CFG,
	MAC_AX_HW_SET_LIFETIME_CFG,
	MAC_AX_HW_EN_BB_RF,
	MAC_AX_HW_SET_APP_FCS,
	MAC_AX_HW_SET_RX_ICVERR,
	MAC_AX_HW_SET_CCTL_RTY_LMT,
	MAC_AX_HW_SET_COEX_GNT,
	MAC_AX_HW_SET_SCOREBOARD,
	MAC_AX_HW_SET_POLLUTED,
	MAC_AX_HW_SET_COEX_CTRL,
	MAC_AX_HW_SET_CLR_TX_CNT,
	MAC_AX_HW_SET_SLOT_TIME,
	MAC_AX_HW_SET_XTAL_AAC_MODE,
	MAC_AX_HW_SET_NAV_PADDING,
	MAC_AX_HW_SET_MAX_TX_TIME,
	MAC_AX_HW_SET_SS_QUOTA_MODE,
	MAC_AX_HW_SET_SS_QUOTA_SETTING,
	MAC_AX_HW_SET_SCHE_PREBKF,
	MAC_AX_HW_SET_WDT_ISR_RST,
	MAC_AX_HW_SET_RESP_ACK,
	MAC_AX_HW_SET_HW_RTS_TH,
	MAC_AX_HW_SET_TX_RU26_TB,
	MAC_AX_HW_SET_BACAM_MODE_SEL,
	MAC_AX_HW_SET_CORE_SWR_VOLT,
	MAC_AX_HW_SET_PARTIAL_PLD_MODE,
	MAC_AX_HW_SET_CCTL_PRELD,
	MAC_AX_HW_SET_GT3_TIMER,
	MAC_AX_HW_SET_RRSR_CFG,
	MAC_AX_HW_SET_CTS_RRSR_CFG,
	MAC_AX_HW_SET_ADAPTER,
	MAC_AX_HW_SET_RESP_ACK_CHK_CCA,
	MAC_AX_HW_SET_SIFS_R2T_T2T,
	MAC_AX_HW_SET_RXD_ZLD_EN,
	MAC_AX_HW_SET_SER_DBG_LVL,
	MAC_AX_HW_SET_DATA_RTY_LMT,
	MAC_AX_HW_SET_CTS2SELF,
};

/**
 * @enum mac_ax_rx_agg_mode
 *
 * @brief mac_ax_rx_agg_mode
 *
 * @var mac_ax_rx_agg_mode::MAC_AX_RX_AGG_MODE_NONE
 * Please Place Description here.
 * @var mac_ax_rx_agg_mode::MAC_AX_RX_AGG_MODE_DMA
 * Please Place Description here.
 * @var mac_ax_rx_agg_mode::MAC_AX_RX_AGG_MODE_USB
 * Please Place Description here.
 * @var mac_ax_rx_agg_mode::MAC_AX_RX_AGG_MODE_LAST
 * Please Place Description here.
 * @var mac_ax_rx_agg_mode::MAC_AX_RX_AGG_MODE_MAX
 * Please Place Description here.
 * @var mac_ax_rx_agg_mode::MAC_AX_RX_AGG_MODE_INVALID
 * Please Place Description here.
 */
enum mac_ax_rx_agg_mode {
	MAC_AX_RX_AGG_MODE_NONE,
	MAC_AX_RX_AGG_MODE_DMA,
	MAC_AX_RX_AGG_MODE_USB,

	/* keep last */
	MAC_AX_RX_AGG_MODE_LAST,
	MAC_AX_RX_AGG_MODE_MAX = MAC_AX_RX_AGG_MODE_LAST,
	MAC_AX_RX_AGG_MODE_INVALID = MAC_AX_RX_AGG_MODE_LAST,
};

/**
 * @enum mac_ax_usr_tx_rpt_mode
 *
 * @brief mac_ax_usr_tx_rpt_mode
 *
 * @var mac_ax_usr_tx_rpt_mode::MAC_AX_USR_TX_RPT_DIS
 * disable report
 * @var mac_ax_usr_tx_rpt_mode::MAC_AX_USR_TX_RPT_PERIOD
 * period mode
 * @var mac_ax_usr_tx_rpt_mode::MAC_AX_USR_TX_RPT_LAST_PKT
 * report after last packet Tx
 */
enum mac_ax_usr_tx_rpt_mode {
	MAC_AX_USR_TX_RPT_DIS = 0,
	MAC_AX_USR_TX_RPT_PERIOD = 1,
	MAC_AX_USR_TX_RPT_LAST_PKT = 2,
};

/**
 * @enum mac_ax_usr_tx_rpt_mode
 *
 * @brief mac_ax_usr_tx_rpt_mode
 *
 * @var mac_ax_usr_tx_rpt_mode::MAC_AX_USR_TX_RPT_DIS
 * disable report
 * @var mac_ax_usr_tx_rpt_mode::MAC_AX_USR_TX_RPT_PERIOD
 * period mode
 * @var mac_ax_usr_tx_rpt_mode::MAC_AX_USR_TX_RPT_LAST_PKT
 * report after last packet Tx
 */
enum mac_ax_ofld_mode {
	MAC_AX_OFLD_MODE_DU_DIS = 0,
	MAC_AX_OFLD_MODE_DU_VAL = 1,
};

/**
 * @enum mac_ax_cmac_ac_sel
 *
 * @brief mac_ax_cmac_ac_sel
 *
 * @var mac_ax_cmac_ac_sel::MAC_AX_CMAC_AC_SEL_BE
 * Please Place Description here.
 * @var mac_ax_cmac_ac_sel::MAC_AX_CMAC_AC_SEL_BK
 * Please Place Description here.
 * @var mac_ax_cmac_ac_sel::MAC_AX_CMAC_AC_SEL_VI
 * Please Place Description here.
 * @var mac_ax_cmac_ac_sel::MAC_AX_CMAC_AC_SEL_VO
 * Please Place Description here.
 * @var mac_ax_cmac_ac_sel::MAC_AX_CMAC_AC_SEL_LAST
 * Please Place Description here.
 * @var mac_ax_cmac_ac_sel::MAC_AX_CMAC_AC_SEL_MAX
 * Please Place Description here.
 * @var mac_ax_cmac_ac_sel::MAC_AX_CMAC_AC_SEL_INVALID
 * Please Place Description here.
 */
enum mac_ax_cmac_ac_sel {
	MAC_AX_CMAC_AC_SEL_BE = 0,
	MAC_AX_CMAC_AC_SEL_BK = 1,
	MAC_AX_CMAC_AC_SEL_VI = 2,
	MAC_AX_CMAC_AC_SEL_VO = 3,

	/* keep last */
	MAC_AX_CMAC_AC_SEL_LAST,
	MAC_AX_CMAC_AC_SEL_MAX = MAC_AX_CMAC_AC_SEL_LAST,
	MAC_AX_CMAC_AC_SEL_INVALID = MAC_AX_CMAC_AC_SEL_LAST,
};

/**
 * @enum mac_ax_cmac_path_sel
 *
 * @brief mac_ax_cmac_path_sel
 *
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_BE0
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_BK0
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_VI0
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_VO0
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_BE1
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_BK1
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_VI1
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_VO1
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_MG0_1
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_MG2
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_BCN
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_TF
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_TWT0
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_TWT1
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_TWT2
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_TWT3
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_LAST
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_MAX
 * Please Place Description here.
 * @var mac_ax_cmac_path_sel::MAC_AX_CMAC_PATH_SEL_INVALID
 * Please Place Description here.
 */
enum mac_ax_cmac_path_sel {
	MAC_AX_CMAC_PATH_SEL_BE0,
	MAC_AX_CMAC_PATH_SEL_BK0,
	MAC_AX_CMAC_PATH_SEL_VI0,
	MAC_AX_CMAC_PATH_SEL_VO0,
	MAC_AX_CMAC_PATH_SEL_BE1,
	MAC_AX_CMAC_PATH_SEL_BK1,
	MAC_AX_CMAC_PATH_SEL_VI1,
	MAC_AX_CMAC_PATH_SEL_VO1,
	MAC_AX_CMAC_PATH_SEL_MG0_1,
	MAC_AX_CMAC_PATH_SEL_MG2,
	MAC_AX_CMAC_PATH_SEL_BCN,
	MAC_AX_CMAC_PATH_SEL_TF,
	MAC_AX_CMAC_PATH_SEL_TWT0,
	MAC_AX_CMAC_PATH_SEL_TWT1,
	MAC_AX_CMAC_PATH_SEL_TWT2,
	MAC_AX_CMAC_PATH_SEL_TWT3,

	/* keep last */
	MAC_AX_CMAC_PATH_SEL_LAST,
	MAC_AX_CMAC_PATH_SEL_MAX = MAC_AX_CMAC_PATH_SEL_LAST,
	MAC_AX_CMAC_PATH_SEL_INVALID = MAC_AX_CMAC_PATH_SEL_LAST,
};

/**
 * @enum mac_ax_cmac_usr_edca_idx
 *
 * @brief mac_ax_cmac_usr_edca_idx
 *
 * @var mac_ax_cmac_usr_edca_idx::MAC_AX_CMAC_USR_EDCA_IDX_0
 * Please Place Description here.
 * @var mac_ax_cmac_usr_edca_idx::MAC_AX_CMAC_USR_EDCA_IDX_1
 * Please Place Description here.
 * @var mac_ax_cmac_usr_edca_idx::MAC_AX_CMAC_USR_EDCA_IDX_2
 * Please Place Description here.
 * @var mac_ax_cmac_usr_edca_idx::MAC_AX_CMAC_USR_EDCA_IDX_3
 * Please Place Description here.
 */
enum mac_ax_cmac_usr_edca_idx {
	MAC_AX_CMAC_USR_EDCA_IDX_0 = 0,
	MAC_AX_CMAC_USR_EDCA_IDX_1 = 1,
	MAC_AX_CMAC_USR_EDCA_IDX_2 = 2,
	MAC_AX_CMAC_USR_EDCA_IDX_3 = 3,
};

/**
 * @enum mac_ax_cmac_wmm_sel
 *
 * @brief mac_ax_cmac_wmm_sel
 *
 * @var mac_ax_cmac_wmm_sel::MAC_AX_CMAC_WMM0_SEL
 * Please Place Description here.
 * @var mac_ax_cmac_wmm_sel::MAC_AX_CMAC_WMM1_SEL
 * Please Place Description here.
 */
enum mac_ax_cmac_wmm_sel {
	MAC_AX_CMAC_WMM0_SEL = 0,
	MAC_AX_CMAC_WMM1_SEL = 1,
};

/**
 * @enum mac_ax_ss_wmm_tbl
 *
 * @brief mac_ax_ss_wmm_tbl
 *
 * @var mac_ax_ss_wmm_tbl::MAC_AX_SS_WMM_TBL_C0_WMM0
 * Please Place Description here.
 * @var mac_ax_ss_wmm_tbl::MAC_AX_SS_WMM_TBL_C0_WMM1
 * Please Place Description here.
 * @var mac_ax_ss_wmm_tbl::MAC_AX_SS_WMM_TBL_C1_WMM0
 * Please Place Description here.
 * @var mac_ax_ss_wmm_tbl::MAC_AX_SS_WMM_TBL_C1_WMM1
 * Please Place Description here.
 */
enum mac_ax_ss_wmm_tbl {
	MAC_AX_SS_WMM_TBL_C0_WMM0 = 0,
	MAC_AX_SS_WMM_TBL_C0_WMM1 = 1,
	MAC_AX_SS_WMM_TBL_C1_WMM0 = 2,
	MAC_AX_SS_WMM_TBL_C1_WMM1 = 3,
};

enum mac_ax_tx_idle_poll_sel {
	MAC_AX_TX_IDLE_POLL_SEL_BAND,
};

/**
 * @enum mac_ax_mcc_status
 *
 * @brief mac_ax_mcc_status
 *
 * @var mac_ax_mcc_status::MAC_AX_MCC_ADD_ROLE_OK
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_START_GROUP_OK
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_STOP_GROUP_OK
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_DEL_GROUP_OK
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_RESET_GROUP_OK
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_SWITCH_CH_OK
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_TXNULL0_OK
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_TXNULL1_OK
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_SWITCH_EARLY
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_TBTT
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_DURATION_START
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_DURATION_END
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_ADD_ROLE_FAIL
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_START_GROUP_FAIL
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_STOP_GROUP_FAIL
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_DEL_GROUP_FAIL
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_RESET_GROUP_FAIL
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_SWITCH_CH_FAIL
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_TXNULL0_FAIL
 * Please Place Description here.
 * @var mac_ax_mcc_status::MAC_AX_MCC_TXNULL1_FAIL
 * Please Place Description here.
 */
enum mac_ax_mcc_status {
	MAC_AX_MCC_ADD_ROLE_OK = 0,
	MAC_AX_MCC_START_GROUP_OK = 1,
	MAC_AX_MCC_STOP_GROUP_OK = 2,
	MAC_AX_MCC_DEL_GROUP_OK = 3,
	MAC_AX_MCC_RESET_GROUP_OK = 4,

	//fail status
	MAC_AX_MCC_EMPTY_GRP_FAIL = 16,
	MAC_AX_MCC_ROLE_NOT_EXIST_FAIL = 17,
	MAC_AX_MCC_DATA_NOT_FOUND_FAIL = 18,
	MAC_AX_MCC_ACT_INVALID_FAIL = 19,
	MAC_AX_MCC_BANDTYPE_INVALID_FAIL = 20,
	MAC_AX_MCC_ADD_PSTIMER_FAIL = 21,
	MAC_AX_MCC_MALLOC_FAIL = 22,
	MAC_AX_MCC_SWITCH_CH_FAIL = 23,
	MAC_AX_MCC_TXNULL0_FAIL = 24,

};

/**
 * @enum mac_ax_trx_mitigation_timer_unit
 *
 * @brief mac_ax_trx_mitigation_timer_unit
 *
 * @var mac_ax_trx_mitigation_timer_unit::MAC_AX_MIT_64US
 * Please Place Description here.
 * @var mac_ax_trx_mitigation_timer_unit::MAC_AX_MIT_128US
 * Please Place Description here.
 * @var mac_ax_trx_mitigation_timer_unit::MAC_AX_MIT_256US
 * Please Place Description here.
 * @var mac_ax_trx_mitigation_timer_unit::MAC_AX_MIT_512US
 * Please Place Description here.
 */
enum mac_ax_trx_mitigation_timer_unit {
	MAC_AX_MIT_64US,
	MAC_AX_MIT_128US,
	MAC_AX_MIT_256US,
	MAC_AX_MIT_512US
};

/**
 * @enum mac_ax_wow_fw_status
 *
 * @brief mac_ax_wow_fw_status
 *
 * @var mac_ax_wow_fw_status::MAC_AX_WOW_NOT_READY
 * Please Place Description here.
 * @var mac_ax_wow_fw_status::MAC_AX_WOW_SLEEP
 * Please Place Description here.
 * @var mac_ax_wow_fw_status::MAC_AX_WOW_RESUME
 * Please Place Description here.
 */
enum mac_ax_wow_fw_status {
	MAC_AX_WOW_NOT_READY,
	MAC_AX_WOW_SLEEP,
	MAC_AX_WOW_RESUME
};

/**
 * @enum mac_ax_wow_ctrl
 *
 * @brief mac_ax_wow_ctrl
 *
 * @var mac_ax_wow_ctrl::MAC_AX_WOW_ENTER
 * Please Place Description here.
 * @var mac_ax_wow_ctrl::MAC_AX_WOW_LEAVE
 * Please Place Description here.
 */
enum mac_ax_wow_ctrl {
	MAC_AX_WOW_ENTER,
	MAC_AX_WOW_LEAVE
};

/**
 * @enum mac_ax_mac_pwr_st
 *
 * @brief mac_ax_mac_pwr_st
 *
 * @var mac_ax_mac_pwr_st::MAC_AX_MAC_OFF
 * Please Place Description here.
 * @var mac_ax_mac_pwr_st::MAC_AX_MAC_ON
 * Please Place Description here.
 * @var mac_ax_mac_pwr_st::MAC_AX_MAC_LPS
 * Please Place Description here.
 */
enum mac_ax_mac_pwr_st {
	MAC_AX_MAC_OFF = 0,
	MAC_AX_MAC_ON = 1,
	MAC_AX_MAC_LPS = 2
};

/**
 * @enum mac_ax_sys_pwr_st
 *
 * @brief mac_ax_sys_pwr_st
 *
 * @var mac_ax_sys_pwr_st::MAC_AX_SYS_ACT
 * Please Place Description here.
 * @var mac_ax_sys_pwr_st::MAC_AX_SYS_LPS
 * Please Place Description here.
 * @var mac_ax_sys_pwr_st::MAC_AX_SYS_SUS
 * Please Place Description here.
 * @var mac_ax_sys_pwr_st::MAC_AX_SYS_PDN
 * Please Place Description here.
 */
enum mac_ax_sys_pwr_st {
	MAC_AX_SYS_ACT = 0x220,
	MAC_AX_SYS_LPS = 0x224,
	MAC_AX_SYS_SUS = 0x224,
	MAC_AX_SYS_PDN = 0x228,
};

/**
 * @enum mac_ax_core_swr_volt
 *
 * @brief mac_ax_core_swr_volt
 *
 * @var mac_ax_core_swr_volt::MAC_AX_SWR_LOW
 * Please Place Description here.
 * @var mac_ax_core_swr_volt::MAC_AX_SWR_NORM
 * Please Place Description here.
 * @var mac_ax_core_swr_volt::MAC_AX_SWR_HIGH
 * Please Place Description here.
 */
enum mac_ax_core_swr_volt {
	MAC_AX_SWR_LOW = 0,
	MAC_AX_SWR_NORM = 3,
	MAC_AX_SWR_HIGH = 6
};

/**
 * @enum mac_txd_ofld_hw_hdr_conv_type
 *
 * @brief TXD offload
 *
 * @var mac_txd_ofld_hw_hdr_conv_type::MAC_TXD_OFLD_HW_HDR_CONV_CONF_MISSING
 * Please Place Description here.
 * @var mac_txd_ofld_hw_hdr_conv_type::MAC_TXD_OFLD_HW_HDR_CONV_OFF
 * Please Place Description here.
 * @var mac_txd_ofld_hw_hdr_conv_type::MAC_TXD_OFLD_HW_HDR_CONV_ETHII_TO_WLAN
 * Please Place Description here.
 * @var mac_txd_ofld_hw_hdr_conv_type::MAC_TXD_OFLD_HW_HDR_CONV_SNAP_TO_WLAN
 * Please Place Description here.
 * @var mac_txd_ofld_hw_hdr_conv_type::MAC_TXD_OFLD_HW_HDR_CONV_WLAN_TO_WLAN
 * Please Place Description here.
 */
enum mac_txd_ofld_hw_hdr_conv_type {
	MAC_TXD_OFLD_HW_HDR_CONV_CONF_MISSING = 0x0,
	MAC_TXD_OFLD_HW_HDR_CONV_OFF = 0x1,
	MAC_TXD_OFLD_HW_HDR_CONV_ETHII_TO_WLAN = 0x2,
	MAC_TXD_OFLD_HW_HDR_CONV_SNAP_TO_WLAN = 0x3,
	MAC_TXD_OFLD_HW_HDR_CONV_WLAN_TO_WLAN = 0x4,
};

/**
 * @enum mac_txd_ofld_enc_type
 *
 * @brief TXD offload
 *
 * @var mac_txd_ofld_enc_type::MAC_TXD_OFLD_HW_ENC_CONF_MISSING
 * Please Place Description here.
 * @var mac_txd_ofld_enc_type::MAC_TXD_OFLD_SW_ENC
 * Please Place Description here.
 * @var mac_txd_ofld_enc_type::MAC_TXD_OFLD_ENC_NONE
 * Please Place Description here.
 * @var mac_txd_ofld_enc_type::MAC_TXD_OFLD_HW_ENC_WEP40
 * Please Place Description here.
 * @var mac_txd_ofld_enc_type::MAC_TXD_OFLD_HW_ENC_WEP104
 * Please Place Description here.
 * @var mac_txd_ofld_enc_type::MAC_TXD_OFLD_HW_ENC_TKIP
 * Please Place Description here.
 * @var mac_txd_ofld_enc_type::MAC_TXD_OFLD_HW_ENC_WAPI
 * Please Place Description here.
 * @var mac_txd_ofld_enc_type::MAC_TXD_OFLD_HW_ENC_GCMSMS4
 * Please Place Description here.
 * @var mac_txd_ofld_enc_type::MAC_TXD_OFLD_HW_ENC_CCMP128
 * Please Place Description here.
 * @var mac_txd_ofld_enc_type::MAC_TXD_OFLD_HW_ENC_CCMP256
 * Please Place Description here.
 * @var mac_txd_ofld_enc_type::MAC_TXD_OFLD_HW_ENC_GCMP128
 * Please Place Description here.
 * @var mac_txd_ofld_enc_type::MAC_TXD_OFLD_HW_ENC_GCMP256
 * Please Place Description here.
 * @var mac_txd_ofld_enc_type::MAC_TXD_OFLD_HW_ENC_BIP128
 * Please Place Description here.
 */
enum mac_txd_ofld_enc_type {
	MAC_TXD_OFLD_HW_ENC_CONF_MISSING = 0x0,
	MAC_TXD_OFLD_SW_ENC = 0x1,
	MAC_TXD_OFLD_HW_ENC_NONE = 0x2,
	MAC_TXD_OFLD_HW_ENC_WEP40 = 0x3,
	MAC_TXD_OFLD_HW_ENC_WEP104 = 0x4,
	MAC_TXD_OFLD_HW_ENC_TKIP = 0x5,
	MAC_TXD_OFLD_HW_ENC_WAPI = 0x6,
	MAC_TXD_OFLD_HW_ENC_GCMSMS4 = 0x7,
	MAC_TXD_OFLD_HW_ENC_CCMP128 = 0x8,
	MAC_TXD_OFLD_HW_ENC_CCMP256 = 0x9,
	MAC_TXD_OFLD_HW_ENC_GCMP128 = 0xA,
	MAC_TXD_OFLD_HW_ENC_GCMP256 = 0xB,
	MAC_TXD_OFLD_HW_ENC_BIP128 = 0xC,
};

/**
 * @enum mac_txd_ofld_hw_amsdu_type
 *
 * @brief TXD offload
 *
 * @var mac_txd_ofld_hw_amsdu_type::MAC_TXD_OFLD_HW_AMSDU_CONF_MISSING
 * Please Place Description here.
 * @var mac_txd_ofld_hw_amsdu_type::MAC_TXD_OFLD_HW_AMSDU_OFF
 * Please Place Description here.
 * @var mac_txd_ofld_hw_amsdu_type::MAC_TXD_OFLD_HW_AMSDU_ON
 * Please Place Description here.
 */
enum mac_txd_ofld_hw_amsdu_type {
	MAC_TXD_OFLD_HW_AMSDU_CONF_MISSING = 0x0,
	MAC_TXD_OFLD_HW_AMSDU_OFF = 0x1,
	MAC_TXD_OFLD_HW_AMSDU_ON = 0x2,
};

/**
 * @enum mac_bcn_ofld_ctrl_type
 *
 * @brief mac_bcn_ofld_ctrl_type
 *
 * @var mac_bcn_ofld_ctrl_type::MAC_BCN_OFLD_DIS
 * Please Place Description here.
 * @var mac_bcn_ofld_ctrl_type::MAC_BCN_OFLD_EN
 * Please Place Description here.
 * @var mac_bcn_ofld_ctrl_type::MAC_BCN_OFLD_UPD_PARAM
 * Please Place Description here.
 * @var mac_bcn_ofld_ctrl_type::MAC_BCN_OFLD_UPD_CAM
 * Please Place Description here.
 */
enum mac_bcn_ofld_ctrl_type {
	MAC_BCN_OFLD_DIS = 0,
	MAC_BCN_OFLD_EN,
	MAC_BCN_OFLD_UPD_PARAM,
	MAC_BCN_OFLD_UPD_CAM,
};

/**
 * @enum mac_iecam_frwd_sel
 *
 * @brief mac_iecam_frwd_sel
 *
 * @var mac_iecam_frwd_sel::MAC_IECAM_FWD_DROP
 * Please Place Description here.
 * @var mac_iecam_frwd_sel::MAC_IECAM_FWD_HOST
 * Please Place Description here.
 * @var mac_iecam_frwd_sel::MAC_IECAM_FWD_WCPU
 * Please Place Description here.
 */
enum mac_iecam_frwd_sel {
	MAC_IECAM_FWD_DROP = 0,
	MAC_IECAM_FWD_HOST,
	MAC_IECAM_FWD_WCPU
};

/**
 * @enum mac_iecam_ent_type
 *
 * @brief mac_iecam_ent_type
 *
 * @var mac_iecam_ent_type::MAC_IECAM_DIS
 * Please Place Description here.
 * @var mac_iecam_ent_type::MAC_IECAM_IE
 * Please Place Description here.
 * @var mac_iecam_ent_type::MAC_IECAM_OFST
 * Please Place Description here.
 * @var mac_iecam_ent_type::MAC_IECAM_CRC
 * Please Place Description here.
 */
enum mac_iecam_ent_type {
	MAC_IECAM_DIS = 0,
	MAC_IECAM_IE,
	MAC_IECAM_OFST,
	MAC_IECAM_CRC
};

/*--------------------Define DBG and recovery related enum--------------------*/

/**
 * @enum mac_ax_err_info
 *
 * @brief mac_ax_err_info
 *
 * @var mac_ax_err_info::MAC_AX_ERR_L0_ERR_CMAC0
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L0_ERR_CMAC1
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L0_RESET_DONE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L0_PROMOTE_TO_L1
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L1_ERR_DMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L1_RESET_DISABLE_DMAC_DONE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L1_RESET_RECOVERY_DONE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L1_PROMOTE_TO_L2
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AH_DMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AH_HCI
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AH_RLX4081
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AH_IDDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AH_HIOE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AH_IPSEC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AH_RX4281
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AH_OTHERS
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AHB_TO_DMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AHB_TO_HCI
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AHB_TO_RLX4081
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AHB_TO_IDDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AHB_TO_HIOE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AHB_TO_IPSEC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AHB_TO_RX4281
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_AHB_TO_OTHERS
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_WVA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_UART
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_CPULOCAL
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_AXIDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_HIOE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_IDDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_IPSEC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_WON
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_WDMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_WCMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_OTHERS
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_WVA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_UART
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_CPULOCAL
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_AXIDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_HIOE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_IDDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_IPSEC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_WDMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_WCMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_OTHERS
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_WVA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_UART
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_CPULOCAL
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_AXIDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_HIOE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_IDDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_IPSEC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_WON
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_WDMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_WCMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_OTHERS
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_WVA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_UART
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_CPULOCAL
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_AXIDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_HIOE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_IDDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_IPSEC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_WON
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_WDMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_WCMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_OTHERS
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_WVA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_UART
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_CPULOCAL
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_AXIDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_HIOE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_IDDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_IPSEC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_WON
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_WDMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_WCMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_OTHERS
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_WVA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_UART
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_CPULOCAL
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_AXIDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_HIOE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_IDDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_IPSEC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_WON
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_WDMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_WCMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_OTHERS
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_WVA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_UART
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_CPULOCAL
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_AXIDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_HIOE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_IDDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_IPSEC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_WON
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_WDMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_WCMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_OTHERS
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_WVA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_UART
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_CPULOCAL
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_AXIDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_HIOE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_IDDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_IPSEC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_WON
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_WDMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_WCMAC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_OTHERS
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_BBRF_TO_DMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_BBRF_TO_HCI
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_BBRF_TO_RLX4081
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_BBRF_TO_IDDMA
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_BBRF_TO_HIOE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_BBRF_TO_IPSEC
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_BBRF_TO_RX4281
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_ERR_APB_BBRF_TO_OTHERS
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L2_RESET_DONE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_GET_ERR_MAX
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L1_DISABLE_EN
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L1_RCVY_EN
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L0_CFG_NOTIFY
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L0_CFG_DIS_NOTIFY
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L0_CFG_HANDSHAKE
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_ERR_L0_RCVY_EN
 * Please Place Description here.
 * @var mac_ax_err_info::MAC_AX_SET_ERR_MAX
 * Please Place Description here.
 */
enum mac_ax_err_info {
	// Get error info
	// L0
	MAC_AX_ERR_L0_ERR_CMAC0 = 0x0001,
	MAC_AX_ERR_L0_ERR_CMAC1 = 0x0002,
	MAC_AX_ERR_L0_RESET_DONE = 0x0003,
	MAC_AX_ERR_L0_PROMOTE_TO_L1 = 0x0010,
	// L1
	MAC_AX_ERR_L1_ERR_DMAC = 0x1000,
	MAC_AX_ERR_L1_RESET_DISABLE_DMAC_DONE = 0x1001,
	MAC_AX_ERR_L1_RESET_RECOVERY_DONE = 0x1002,
	MAC_AX_ERR_L1_PROMOTE_TO_L2 = 0x1010,
	MAC_AX_ERR_L1_RCVY_STOP_DONE = 0x1011,
	// L2
	// address hole (master)
	MAC_AX_ERR_L2_ERR_AH_DMA = 0x2000,
	MAC_AX_ERR_L2_ERR_AH_HCI = 0x2010,
	MAC_AX_ERR_L2_ERR_AH_RLX4081 = 0x2020,
	MAC_AX_ERR_L2_ERR_AH_IDDMA = 0x2030,
	MAC_AX_ERR_L2_ERR_AH_HIOE = 0x2040,
	MAC_AX_ERR_L2_ERR_AH_IPSEC = 0x2050,
	MAC_AX_ERR_L2_ERR_AH_RX4281 = 0x2060,
	MAC_AX_ERR_L2_ERR_AH_OTHERS = 0x2070,
	// AHB bridge timeout (master)
	MAC_AX_ERR_L2_ERR_AHB_TO_DMA = 0x2100,
	MAC_AX_ERR_L2_ERR_AHB_TO_HCI = 0x2110,
	MAC_AX_ERR_L2_ERR_AHB_TO_RLX4081 = 0x2120,
	MAC_AX_ERR_L2_ERR_AHB_TO_IDDMA = 0x2130,
	MAC_AX_ERR_L2_ERR_AHB_TO_HIOE = 0x2140,
	MAC_AX_ERR_L2_ERR_AHB_TO_IPSEC = 0x2150,
	MAC_AX_ERR_L2_ERR_AHB_TO_RX4281 = 0x2160,
	MAC_AX_ERR_L2_ERR_AHB_TO_OTHERS = 0x2170,
	// APB_SA bridge timeout (master + slave)
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_WVA = 0x2200,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_UART = 0x2201,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_CPULOCAL = 0x2202,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_AXIDMA = 0x2203,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_HIOE = 0x2204,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_IDDMA = 0x2205,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_IPSEC = 0x2206,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_WON = 0x2207,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_WDMAC = 0x2208,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_WCMAC = 0x2209,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_OTHERS = 0x220A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_WVA = 0x2210,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_UART = 0x2211,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_CPULOCAL = 0x2212,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_AXIDMA = 0x2213,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_HIOE = 0x2214,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_IDDMA = 0x2215,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_IPSEC = 0x2216,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_WDMAC = 0x2218,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_WCMAC = 0x2219,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_OTHERS = 0x221A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_WVA = 0x2220,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_UART = 0x2221,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_CPULOCAL = 0x2222,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_AXIDMA = 0x2223,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_HIOE = 0x2224,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_IDDMA = 0x2225,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_IPSEC = 0x2226,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_WON = 0x2227,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_WDMAC = 0x2228,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_WCMAC = 0x2229,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_OTHERS = 0x222A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_WVA = 0x2230,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_UART = 0x2231,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_CPULOCAL = 0x2232,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_AXIDMA = 0x2233,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_HIOE = 0x2234,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_IDDMA = 0x2235,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_IPSEC = 0x2236,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_WON = 0x2237,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_WDMAC = 0x2238,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_WCMAC = 0x2239,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_OTHERS = 0x223A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_WVA = 0x2240,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_UART = 0x2241,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_CPULOCAL = 0x2242,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_AXIDMA = 0x2243,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_HIOE = 0x2244,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_IDDMA = 0x2245,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_IPSEC = 0x2246,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_WON = 0x2247,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_WDMAC = 0x2248,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_WCMAC = 0x2249,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_OTHERS = 0x224A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_WVA = 0x2250,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_UART = 0x2251,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_CPULOCAL = 0x2252,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_AXIDMA = 0x2253,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_HIOE = 0x2254,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_IDDMA = 0x2255,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_IPSEC = 0x2256,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_WON = 0x2257,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_WDMAC = 0x2258,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_WCMAC = 0x2259,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_OTHERS = 0x225A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_WVA = 0x2260,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_UART = 0x2261,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_CPULOCAL = 0x2262,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_AXIDMA = 0x2263,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_HIOE = 0x2264,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_IDDMA = 0x2265,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_IPSEC = 0x2266,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_WON = 0x2267,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_WDMAC = 0x2268,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_WCMAC = 0x2269,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_OTHERS = 0x226A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_WVA = 0x2270,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_UART = 0x2271,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_CPULOCAL = 0x2272,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_AXIDMA = 0x2273,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_HIOE = 0x2274,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_IDDMA = 0x2275,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_IPSEC = 0x2276,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_WON = 0x2277,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_WDMAC = 0x2278,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_WCMAC = 0x2279,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_OTHERS = 0x227A,
	// APB_BBRF bridge timeout (master)
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_DMA = 0x2300,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_HCI = 0x2310,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_RLX4081 = 0x2320,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_IDDMA = 0x2330,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_HIOE = 0x2340,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_IPSEC = 0x2350,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_RX4281 = 0x2360,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_OTHERS = 0x2370,
	MAC_AX_ERR_L2_RESET_DONE = 0x2400,
	MAC_AX_ERR_CPU_EXCEPTION = 0x3000,
	MAC_AX_ERR_ASSERTION = 0x4000,
	MAC_AX_ERR_RXI300 = 0x5000,
	MAC_AX_GET_ERR_MAX,
	//Use the special code to indicate phl should dump share buffer
	MAC_AX_DUMP_SHAREBUFF_INDICATOR = 0x80000000,
	// set error info
	MAC_AX_ERR_L1_DISABLE_EN = 0x0001,
	MAC_AX_ERR_L1_RCVY_EN = 0x0002,
	MAC_AX_ERR_L1_RCVY_STOP_REQ = 0x0003,
	MAC_AX_ERR_L1_RCVY_START_REQ = 0x0004,
	MAC_AX_ERR_L1_RCVY_STOP_WO_RECVY_REQ = 0x0005,
	MAC_AX_ERR_L1_RCVY_START_WO_RECVY_REQ = 0x0006,
	MAC_AX_ERR_L0_CFG_NOTIFY = 0x0010,
	MAC_AX_ERR_L0_CFG_DIS_NOTIFY = 0x0011,
	MAC_AX_ERR_L0_CFG_HANDSHAKE = 0x0012,
	MAC_AX_ERR_L0_RCVY_EN = 0x0013,
	MAC_AX_SET_ERR_MAX,
};

/**
 * @enum mac_ax_ser_status
 *
 * @brief mac_ax_ser_status
 *
 * @var mac_ax_ser_status::MAC_AX_L1_FALSE
 * Please Place Description here.
 * @var mac_ax_ser_status::MAC_AX_L1_TRUE
 * Please Place Description here.
 */
enum mac_ax_ser_status {
	MAC_AX_L1_FALSE = 0,
	MAC_AX_L1_TRUE = 1,
};

/**
 * @enum mac_ax_mem_sel
 *
 * @brief mac_ax_mem_sel
 *
 * @var mac_ax_mem_sel::MAC_AX_MEM_CPU_LOCAL
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_WCPU_DATA
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_AXIDMA
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_SHARED_BUF
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_DMAC_TBL
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_SHCUT_MACHDR
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_STA_SCHED
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_RXPLD_FLTR_CAM
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_SECURITY_CAM
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_WOW_CAM
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_CMAC_TBL
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_ADDR_CAM
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_BA_CAM
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_BCN_IE_CAM0
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_BCN_IE_CAM1
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_TXD_FIFO_0
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_TXD_FIFO_1
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_LAST
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_MAX
 * Please Place Description here.
 * @var mac_ax_mem_sel::MAC_AX_MEM_INVALID
 * Please Place Description here.
 */
enum mac_ax_mem_sel {
	MAC_AX_MEM_CPU_LOCAL,
	MAC_AX_MEM_WCPU_DATA,
	MAC_AX_MEM_AXIDMA,
	MAC_AX_MEM_SHARED_BUF,
	MAC_AX_MEM_DMAC_TBL,
	MAC_AX_MEM_SHCUT_MACHDR,
	MAC_AX_MEM_STA_SCHED,
	MAC_AX_MEM_RXPLD_FLTR_CAM,
	MAC_AX_MEM_SECURITY_CAM,
	MAC_AX_MEM_WOW_CAM,
	MAC_AX_MEM_CMAC_TBL,
	MAC_AX_MEM_ADDR_CAM,
	MAC_AX_MEM_BA_CAM,
	MAC_AX_MEM_BCN_IE_CAM0,
	MAC_AX_MEM_BCN_IE_CAM1,
	MAC_AX_MEM_TXD_FIFO_0,
	MAC_AX_MEM_TXD_FIFO_1,

	/* keep last */
	MAC_AX_MEM_LAST,
	MAC_AX_MEM_MAX = MAC_AX_MEM_LAST,
	MAC_AX_MEM_INVALID = MAC_AX_MEM_LAST,
};

/**
 * @enum mac_ax_reg_sel
 *
 * @brief mac_ax_reg_sel
 *
 * @var mac_ax_reg_sel::MAC_AX_REG_MAC
 * Please Place Description here.
 * @var mac_ax_reg_sel::MAC_AX_REG_BB
 * Please Place Description here.
 * @var mac_ax_reg_sel::MAC_AX_REG_IQK
 * Please Place Description here.
 * @var mac_ax_reg_sel::MAC_AX_REG_RFC
 * Please Place Description here.
 * @var mac_ax_reg_sel::MAC_AX_REG_LAST
 * Please Place Description here.
 * @var mac_ax_reg_sel::MAC_AX_REG_MAX
 * Please Place Description here.
 * @var mac_ax_reg_sel::MAC_AX_REG_INVALID
 * Please Place Description here.
 */
enum mac_ax_reg_sel {
	MAC_AX_REG_MAC,
	MAC_AX_REG_BB,
	MAC_AX_REG_IQK,
	MAC_AX_REG_RFC,

	/* keep last */
	MAC_AX_REG_LAST,
	MAC_AX_REG_MAX = MAC_AX_REG_LAST,
	MAC_AX_REG_INVALID = MAC_AX_REG_LAST,
};

/*--------------------Define GPIO related enum-------------------------------*/

/**
 * @enum mac_ax_gpio_func
 *
 * @brief mac_ax_gpio_func
 *
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_0
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_1
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_2
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_3
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_4
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_5
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_6
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_7
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_8
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_9
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_10
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_11
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_12
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_13
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_14
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_SW_IO_15
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_UART_TX_GPIO5
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_UART_TX_GPIO7
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_UART_TX_GPIO8
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_UART_RX_GPIO6
 * Please Place Description here.
 * @var mac_ax_gpio_func::MAC_AX_GPIO_UART_RX_GPIO14
 * Please Place Description here.
 */
enum mac_ax_gpio_func {
	MAC_AX_GPIO_SW_IO_0,
	MAC_AX_GPIO_SW_IO_1,
	MAC_AX_GPIO_SW_IO_2,
	MAC_AX_GPIO_SW_IO_3,
	MAC_AX_GPIO_SW_IO_4,
	MAC_AX_GPIO_SW_IO_5,
	MAC_AX_GPIO_SW_IO_6,
	MAC_AX_GPIO_SW_IO_7,
	MAC_AX_GPIO_SW_IO_8,
	MAC_AX_GPIO_SW_IO_9,
	MAC_AX_GPIO_SW_IO_10,
	MAC_AX_GPIO_SW_IO_11,
	MAC_AX_GPIO_SW_IO_12,
	MAC_AX_GPIO_SW_IO_13,
	MAC_AX_GPIO_SW_IO_14,
	MAC_AX_GPIO_SW_IO_15,
	MAC_AX_GPIO_UART_TX_GPIO5,
	MAC_AX_GPIO_UART_TX_GPIO7,
	MAC_AX_GPIO_UART_TX_GPIO8,
	MAC_AX_GPIO_UART_RX_GPIO6,
	MAC_AX_GPIO_UART_RX_GPIO14,
};

/**
 * @enum mac_ax_uart_tx_pin
 *
 * @brief mac_ax_uart_tx_pin
 *
 * @var mac_ax_uart_tx_pin::MAC_AX_UART_TX_GPIO5
 * Please Place Description here.
 * @var mac_ax_uart_tx_pin::MAC_AX_UART_TX_GPIO7
 * Please Place Description here.
 * @var mac_ax_uart_tx_pin::MAC_AX_UART_TX_GPIO8
 * Please Place Description here.
 * @var mac_ax_uart_tx_pin::MAC_AX_UART_TX_GPIO5_GPIO8
 * Please Place Description here.
 */
enum mac_ax_uart_tx_pin {
	MAC_AX_UART_TX_GPIO5,
	MAC_AX_UART_TX_GPIO7,
	MAC_AX_UART_TX_GPIO8,
	MAC_AX_UART_TX_GPIO5_GPIO8,
};

/**
 * @enum mac_ax_uart_rx_pin
 *
 * @brief mac_ax_uart_rx_pin
 *
 * @var mac_ax_uart_rx_pin::MAC_AX_UART_RX_GPIO6
 * Please Place Description here.
 * @var mac_ax_uart_rx_pin::MAC_AX_UART_RX_GPIO14
 * Please Place Description here.
 */
enum mac_ax_uart_rx_pin {
	MAC_AX_UART_RX_GPIO6,
	MAC_AX_UART_RX_GPIO14,
};

/**
 * @enum mac_ax_led_mode
 *
 * @brief mac_ax_led_mode
 *
 * @var mac_ax_led_mode::MAC_AX_LED_MODE_TRX_ON
 * Please Place Description here.
 * @var mac_ax_led_mode::MAC_AX_LED_MODE_TX_ON
 * Please Place Description here.
 * @var mac_ax_led_mode::MAC_AX_LED_MODE_RX_ON
 * Please Place Description here.
 * @var mac_ax_led_mode::MAC_AX_LED_MODE_SW_CTRL_OD
 * Please Place Description here.
 * @var mac_ax_led_mode::MAC_AX_LED_MODE_SW_CTRL_PP
 * Please Place Description here.
 * @var mac_ax_led_mode::MAC_AX_LED_MODE_LAST
 * Please Place Description here.
 * @var mac_ax_led_mode::MAC_AX_LED_MODE_MAX
 * Please Place Description here.
 * @var mac_ax_led_mode::MAC_AX_LED_MODE_INVALID
 * Please Place Description here.
 */
enum mac_ax_led_mode {
	MAC_AX_LED_MODE_TRX_ON,
	MAC_AX_LED_MODE_TX_ON,
	MAC_AX_LED_MODE_RX_ON,
	MAC_AX_LED_MODE_SW_CTRL_OD,
	MAC_AX_LED_MODE_SW_CTRL_PP,

	/* keep last */
	MAC_AX_LED_MODE_LAST,
	MAC_AX_LED_MODE_MAX = MAC_AX_LED_MODE_LAST,
	MAC_AX_LED_MODE_INVALID = MAC_AX_LED_MODE_LAST,
};

/**
 * @enum mac_ax_sw_io_mode
 *
 * @brief mac_ax_sw_io_mode
 *
 * @var mac_ax_sw_io_mode::MAC_AX_SW_IO_MODE_INPUT
 * Please Place Description here.
 * @var mac_ax_sw_io_mode::MAC_AX_SW_IO_MODE_OUTPUT_OD
 * Please Place Description here.
 * @var mac_ax_sw_io_mode::MAC_AX_SW_IO_MODE_OUTPUT_PP
 * Please Place Description here.
 * @var mac_ax_sw_io_mode::MAC_AX_SW_IO_MODE_LAST
 * Please Place Description here.
 * @var mac_ax_sw_io_mode::MAC_AX_SW_IO_MODE_MAX
 * Please Place Description here.
 * @var mac_ax_sw_io_mode::MAC_AX_SW_IO_MODE_INVALID
 * Please Place Description here.
 */
enum mac_ax_sw_io_mode {
	MAC_AX_SW_IO_MODE_INPUT,
	MAC_AX_SW_IO_MODE_OUTPUT_OD,
	MAC_AX_SW_IO_MODE_OUTPUT_PP,

	/* keep last */
	MAC_AX_SW_IO_MODE_LAST,
	MAC_AX_SW_IO_MODE_MAX = MAC_AX_SW_IO_MODE_LAST,
	MAC_AX_SW_IO_MODE_INVALID = MAC_AX_SW_IO_MODE_LAST,
};

/*--------------------Define Efuse related enum-------------------------------*/

/**
 * @enum mac_ax_efuse_read_cfg
 *
 * @brief mac_ax_efuse_read_cfg
 *
 * @var mac_ax_efuse_read_cfg::MAC_AX_EFUSE_R_AUTO
 * Please Place Description here.
 * @var mac_ax_efuse_read_cfg::MAC_AX_EFUSE_R_DRV
 * Please Place Description here.
 * @var mac_ax_efuse_read_cfg::MAC_AX_EFUSE_R_FW
 * Please Place Description here.
 * @var mac_ax_efuse_read_cfg::MAC_AX_EFUSE_R_LAST
 * Please Place Description here.
 * @var mac_ax_efuse_read_cfg::MAC_AX_EFUSE_R_MAX
 * Please Place Description here.
 * @var mac_ax_efuse_read_cfg::MAC_AX_EFUSE_R_INVALID
 * Please Place Description here.
 */
enum mac_ax_efuse_read_cfg {
	MAC_AX_EFUSE_R_AUTO,
	MAC_AX_EFUSE_R_DRV,
	MAC_AX_EFUSE_R_FW,

	/* keep last */
	MAC_AX_EFUSE_R_LAST,
	MAC_AX_EFUSE_R_MAX = MAC_AX_EFUSE_R_LAST,
	MAC_AX_EFUSE_R_INVALID = MAC_AX_EFUSE_R_LAST,
};

/**
 * @enum mac_ax_efuse_bank
 *
 * @brief mac_ax_efuse_bank
 *
 * @var mac_ax_efuse_bank::MAC_AX_EFUSE_BANK_WIFI
 * Please Place Description here.
 * @var mac_ax_efuse_bank::MAC_AX_EFUSE_BANK_BT
 * Please Place Description here.
 * @var mac_ax_efuse_bank::MAC_AX_EFUSE_BANK_LAST
 * Please Place Description here.
 * @var mac_ax_efuse_bank::MAC_AX_EFUSE_BANK_MAX
 * Please Place Description here.
 * @var mac_ax_efuse_bank::MAC_AX_EFUSE_BANK_INVALID
 * Please Place Description here.
 */
enum mac_ax_efuse_bank {
	MAC_AX_EFUSE_BANK_WIFI,
	MAC_AX_EFUSE_BANK_BT,

	/* keep last */
	MAC_AX_EFUSE_BANK_LAST,
	MAC_AX_EFUSE_BANK_MAX = MAC_AX_EFUSE_BANK_LAST,
	MAC_AX_EFUSE_BANK_INVALID = MAC_AX_EFUSE_BANK_LAST,
};

/**
 * @enum mac_ax_efuse_parser_cfg
 *
 * @brief mac_ax_efuse_parser_cfg
 *
 * @var mac_ax_efuse_parser_cfg::MAC_AX_EFUSE_PARSER_MAP
 * Please Place Description here.
 * @var mac_ax_efuse_parser_cfg::MAC_AX_EFUSE_PARSER_MASK
 * Please Place Description here.
 * @var mac_ax_efuse_parser_cfg::MAC_AX_EFUSE_PARSER_LAST
 * Please Place Description here.
 * @var mac_ax_efuse_parser_cfg::MAC_AX_EFUSE_PARSER_MAX
 * Please Place Description here.
 * @var mac_ax_efuse_parser_cfg::MAC_AX_EFUSE_PARSER_INVALID
 * Please Place Description here.
 */
enum mac_ax_efuse_parser_cfg {
	MAC_AX_EFUSE_PARSER_MAP,
	MAC_AX_EFUSE_PARSER_MASK,

	/* keep last */
	MAC_AX_EFUSE_PARSER_LAST,
	MAC_AX_EFUSE_PARSER_MAX = MAC_AX_EFUSE_PARSER_LAST,
	MAC_AX_EFUSE_PARSER_INVALID = MAC_AX_EFUSE_PARSER_LAST,
};

/**
 * @enum mac_ax_efuse_feature_id
 *
 * @brief mac_ax_efuse_feature_id
 *
 * @var mac_ax_efuse_feature_id::MAC_AX_DUMP_PHYSICAL_EFUSE
 * Please Place Description here.
 * @var mac_ax_efuse_feature_id::MAC_AX_DUMP_LOGICAL_EFUSE
 * Please Place Description here.
 * @var mac_ax_efuse_feature_id::MAC_AX_DUMP_LOGICAL_EFUSE_MASK
 * Please Place Description here.
 */
enum mac_ax_efuse_feature_id {
	MAC_AX_DUMP_PHYSICAL_EFUSE,     /* Support */
	MAC_AX_DUMP_LOGICAL_EFUSE,      /* Support */
	MAC_AX_DUMP_LOGICAL_EFUSE_MASK, /* Support */
};

/**
 * @enum mac_ax_efuse_hidden_cfg
 *
 * @brief mac_ax_efuse_hidden_cfg
 *
 * @var mac_ax_efuse_hidden_cfg::MAC_AX_EFUSE_HIDDEN_RF
 * Please Place Description here.
 * @var mac_ax_efuse_hidden_cfg::MAC_AX_EFUSE_HIDDEN_MAC
 * Please Place Description here.
 */
enum mac_ax_efuse_hidden_cfg {
	MAC_AX_EFUSE_HIDDEN_RF,
	MAC_AX_EFUSE_HIDDEN_MAC,
};

/*--------------------Define TRX PKT INFO/RPT related enum--------------------*/

/**
 * @enum mac_ax_trx_mode
 *
 * @brief mac_ax_trx_mode
 *
 * @var mac_ax_trx_mode::MAC_AX_TRX_SW_MODE
 * Please Place Description here.
 * @var mac_ax_trx_mode::MAC_AX_TRX_HW_MODE
 * Please Place Description here.
 * @var mac_ax_trx_mode::MAC_AX_TRX_LOOPBACK
 * Please Place Description here.
 * @var mac_ax_trx_mode::MAC_AX_TRX_LAST
 * Please Place Description here.
 * @var mac_ax_trx_mode::MAC_AX_TRX_MAX
 * Please Place Description here.
 * @var mac_ax_trx_mode::MAC_AX_TRX_INVALID
 * Please Place Description here.
 */
enum mac_ax_trx_mode {
	MAC_AX_TRX_SW_MODE,
	MAC_AX_TRX_HW_MODE,
	MAC_AX_TRX_LOOPBACK,

	MAC_AX_TRX_NORMAL, /* shall remove when v0_22 release, Rick */

	/* keep last */
	MAC_AX_TRX_LAST,
	MAC_AX_TRX_MAX = MAC_AX_TRX_LAST,
	MAC_AX_TRX_INVALID = MAC_AX_TRX_LAST,
};

/**
 * @enum mac_ax_qta_mode
 *
 * @brief mac_ax_qta_mode
 *
 * @var mac_ax_qta_mode::MAC_AX_QTA_SCC
 * Please Place Description here.
 * @var mac_ax_qta_mode::MAC_AX_QTA_DBCC
 * Please Place Description here.
 * @var mac_ax_qta_mode::MAC_AX_QTA_SCC_STF
 * Please Place Description here.
 * @var mac_ax_qta_mode::MAC_AX_QTA_DBCC_STF
 * Please Place Description here.
 * @var mac_ax_qta_mode::MAC_AX_QTA_SU_TP
 * Please Place Description here.
 * @var mac_ax_qta_mode::MAC_AX_QTA_DLFW
 * Please Place Description here.
 * @var mac_ax_qta_mode::MAC_AX_QTA_BCN_TEST
 * Please Place Description here.
 * @var mac_ax_qta_mode::MAC_AX_QTA_LAMODE
 * Please Place Description here.
 * @var mac_ax_qta_mode::MAC_AX_QTA_LAST
 * Please Place Description here.
 * @var mac_ax_qta_mode::MAC_AX_QTA_MAX
 * Please Place Description here.
 * @var mac_ax_qta_mode::MAC_AX_QTA_INVALID
 * Please Place Description here.
 */
enum mac_ax_qta_mode {
	MAC_AX_QTA_SCC,
	MAC_AX_QTA_DBCC,
	MAC_AX_QTA_SCC_STF,
	MAC_AX_QTA_DBCC_STF,
	MAC_AX_QTA_SU_TP,
	MAC_AX_QTA_DLFW,
	MAC_AX_QTA_BCN_TEST,
	MAC_AX_QTA_LAMODE,
	MAC_AX_QTA_SCC_TURBO,

	/* keep last */
	MAC_AX_QTA_LAST,
	MAC_AX_QTA_MAX = MAC_AX_QTA_LAST,
	MAC_AX_QTA_INVALID = MAC_AX_QTA_LAST,
};

/**
 * @enum mac_ax_pkt_t
 *
 * @brief mac_ax_pkt_t
 *
 * @var mac_ax_pkt_t::MAC_AX_PKT_DATA
 * Please Place Description here.
 * @var mac_ax_pkt_t::MAC_AX_PKT_MGNT
 * Please Place Description here.
 * @var mac_ax_pkt_t::MAC_AX_PKT_CTRL
 * Please Place Description here.
 * @var mac_ax_pkt_t::MAC_AX_PKT_8023
 * Please Place Description here.
 * @var mac_ax_pkt_t::MAC_AX_PKT_H2C
 * Please Place Description here.
 * @var mac_ax_pkt_t::MAC_AX_PKT_FWDL
 * Please Place Description here.
 * @var mac_ax_pkt_t::MAC_AX_PKT_C2H
 * Please Place Description here.
 * @var mac_ax_pkt_t::MAC_AX_PKT_PPDU
 * Please Place Description here.
 * @var mac_ax_pkt_t::MAC_AX_PKT_CH_INFO
 * Please Place Description here.
 * @var mac_ax_pkt_t::MAC_AX_PKT_DFS
 * Please Place Description here.
 * @var mac_ax_pkt_t::MAC_AX_PKT_LAST
 * Please Place Description here.
 * @var mac_ax_pkt_t::MAC_AX_PKT_MAX
 * Please Place Description here.
 * @var mac_ax_pkt_t::MAC_AX_PKT_INVALID
 * Please Place Description here.
 */
enum mac_ax_pkt_t {
	MAC_AX_PKT_DATA,
	MAC_AX_PKT_MGNT,
	MAC_AX_PKT_CTRL,
	MAC_AX_PKT_8023,
	MAC_AX_PKT_H2C,
	MAC_AX_PKT_FWDL,
	MAC_AX_PKT_C2H,
	MAC_AX_PKT_PPDU,
	MAC_AX_PKT_CH_INFO,
	MAC_AX_PKT_DFS,

	/* keep last */
	MAC_AX_PKT_LAST,
	MAC_AX_PKT_MAX = MAC_AX_PKT_LAST,
	MAC_AX_PKT_INVALID = MAC_AX_PKT_LAST,
};

/**
 * @enum mac_ax_amsdu_pkt_num
 *
 * @brief mac_ax_amsdu_pkt_num
 *
 * @var mac_ax_amsdu_pkt_num::MAC_AX_AMSDU_AGG_NUM_1
 * Please Place Description here.
 * @var mac_ax_amsdu_pkt_num::MAC_AX_AMSDU_AGG_NUM_2
 * Please Place Description here.
 * @var mac_ax_amsdu_pkt_num::MAC_AX_AMSDU_AGG_NUM_3
 * Please Place Description here.
 * @var mac_ax_amsdu_pkt_num::MAC_AX_AMSDU_AGG_NUM_4
 * Please Place Description here.
 * @var mac_ax_amsdu_pkt_num::MAC_AX_AMSDU_AGG_NUM_MAX
 * Please Place Description here.
 */
enum mac_ax_amsdu_pkt_num {
	MAC_AX_AMSDU_AGG_NUM_1 = 0,
	MAC_AX_AMSDU_AGG_NUM_2 = 1,
	MAC_AX_AMSDU_AGG_NUM_3 = 2,
	MAC_AX_AMSDU_AGG_NUM_4 = 3,
	MAC_AX_AMSDU_AGG_NUM_MAX
};

/**
 * @enum mac_ax_phy_rpt
 *
 * @brief mac_ax_phy_rpt
 *
 * @var mac_ax_phy_rpt::MAC_AX_PPDU_STATUS
 * Please Place Description here.
 * @var mac_ax_phy_rpt::MAC_AX_CH_INFO
 * Please Place Description here.
 * @var mac_ax_phy_rpt::MAC_AX_DFS
 * Please Place Description here.
 */
enum mac_ax_phy_rpt {
	MAC_AX_PPDU_STATUS,
	MAC_AX_CH_INFO,
	MAC_AX_DFS,
};

/**
 * @enum mac_ax_pkt_drop_sel
 *
 * @brief mac_ax_pkt_drop_sel
 *
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_MACID_BE_ONCE
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_MACID_BK_ONCE
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_MACID_VI_ONCE
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_MACID_VO_ONCE
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_MACID_ALL
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_MG0_ONCE
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_HIQ_ONCE
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_HIQ_PORT
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_HIQ_MBSSID
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_BAND
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_REL_MACID
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_REL_HIQ_PORT
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_REL_HIQ_MBSSID
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_LAST
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_MAX
 * Please Place Description here.
 * @var mac_ax_pkt_drop_sel::MAC_AX_PKT_DROP_SEL_INVALID
 * Please Place Description here.
 */
enum mac_ax_pkt_drop_sel {
	MAC_AX_PKT_DROP_SEL_MACID_BE_ONCE,
	MAC_AX_PKT_DROP_SEL_MACID_BK_ONCE,
	MAC_AX_PKT_DROP_SEL_MACID_VI_ONCE,
	MAC_AX_PKT_DROP_SEL_MACID_VO_ONCE,
	MAC_AX_PKT_DROP_SEL_MACID_ALL,
	MAC_AX_PKT_DROP_SEL_MG0_ONCE,
	MAC_AX_PKT_DROP_SEL_HIQ_ONCE,
	MAC_AX_PKT_DROP_SEL_HIQ_PORT,
	MAC_AX_PKT_DROP_SEL_HIQ_MBSSID,
	MAC_AX_PKT_DROP_SEL_BAND,
	MAC_AX_PKT_DROP_SEL_BAND_ONCE,
	MAC_AX_PKT_DROP_SEL_REL_MACID,
	MAC_AX_PKT_DROP_SEL_REL_HIQ_PORT,
	MAC_AX_PKT_DROP_SEL_REL_HIQ_MBSSID,

	/* keep last */
	MAC_AX_PKT_DROP_SEL_LAST,
	MAC_AX_PKT_DROP_SEL_MAX = MAC_AX_PKT_DROP_SEL_LAST,
	MAC_AX_PKT_DROP_SEL_INVALID = MAC_AX_PKT_DROP_SEL_LAST,
};

/*need to check and move to other */

/**
 * @enum mac_ax_fwd_target
 *
 * @brief mac_ax_fwd_target
 *
 * @var mac_ax_fwd_target::MAC_AX_FWD_DONT_CARE
 * Please Place Description here.
 * @var mac_ax_fwd_target::MAC_AX_FWD_TO_HOST
 * Please Place Description here.
 * @var mac_ax_fwd_target::MAC_AX_FWD_TO_WLAN_CPU
 * Please Place Description here.
 */
enum mac_ax_fwd_target {
	MAC_AX_FWD_DONT_CARE    = 0,
	MAC_AX_FWD_TO_HOST      = 1,
	MAC_AX_FWD_TO_WLAN_CPU  = 2
};

/**
 * @enum mac_ax_action_frame
 *
 * @brief mac_ax_action_frame
 *
 * @var mac_ax_action_frame::MAC_AX_AF_CSA
 * Please Place Description here.
 * @var mac_ax_action_frame::MAC_AX_AF_ADDTS_REQ
 * Please Place Description here.
 * @var mac_ax_action_frame::MAC_AX_AF_ADDTS_RES
 * Please Place Description here.
 * @var mac_ax_action_frame::MAC_AX_AF_DELTS
 * Please Place Description here.
 * @var mac_ax_action_frame::MAC_AX_AF_ADDBA_REQ
 * Please Place Description here.
 * @var mac_ax_action_frame::MAC_AX_AF_ADDBA_RES
 * Please Place Description here.
 * @var mac_ax_action_frame::MAC_AX_AF_DELBA
 * Please Place Description here.
 * @var mac_ax_action_frame::MAC_AX_AF_NCW
 * Please Place Description here.
 * @var mac_ax_action_frame::MAC_AX_AF_GID_MGNT
 * Please Place Description here.
 * @var mac_ax_action_frame::MAC_AX_AF_OP_MODE
 * Please Place Description here.
 * @var mac_ax_action_frame::MAC_AX_AF_CSI
 * Please Place Description here.
 * @var mac_ax_action_frame::MAC_AX_AF_HT_CBFM
 * Please Place Description here.
 * @var mac_ax_action_frame::MAC_AX_AF_VHT_CBFM
 * Please Place Description here.
 */
enum mac_ax_action_frame {
	MAC_AX_AF_CSA       = 0,
	MAC_AX_AF_ADDTS_REQ = 1,
	MAC_AX_AF_ADDTS_RES = 2,
	MAC_AX_AF_DELTS     = 3,
	MAC_AX_AF_ADDBA_REQ = 4,
	MAC_AX_AF_ADDBA_RES = 5,
	MAC_AX_AF_DELBA     = 6,
	MAC_AX_AF_NCW       = 7,
	MAC_AX_AF_GID_MGNT  = 8,
	MAC_AX_AF_OP_MODE   = 9,
	MAC_AX_AF_CSI       = 10,
	MAC_AX_AF_HT_CBFM   = 11,
	MAC_AX_AF_VHT_CBFM  = 12
};

/**
 * @enum mac_ax_af_user_define_index
 *
 * @brief mac_ax_af_user_define_index
 *
 * @var mac_ax_af_user_define_index::MAC_AX_AF_UD_0
 * Please Place Description here.
 * @var mac_ax_af_user_define_index::MAC_AX_AF_UD_1
 * Please Place Description here.
 * @var mac_ax_af_user_define_index::MAC_AX_AF_UD_2
 * Please Place Description here.
 * @var mac_ax_af_user_define_index::MAC_AX_AF_UD_3
 * Please Place Description here.
 */
enum mac_ax_af_user_define_index {
	MAC_AX_AF_UD_0      = 0,
	MAC_AX_AF_UD_1      = 1,
	MAC_AX_AF_UD_2      = 2,
	MAC_AX_AF_UD_3      = 3
};

/**
 * @enum mac_ax_trigger_frame
 *
 * @brief mac_ax_trigger_frame
 *
 * @var mac_ax_trigger_frame::MAC_AX_TF_BT
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_BFRP
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_MU_BAR
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_MU_RTS
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_BSRP
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_GCR_MU_BAR
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_BQRP
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_NFRP
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_TF8
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_TF9
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_TF10
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_TF11
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_TF12
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_TF13
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_TF14
 * Please Place Description here.
 * @var mac_ax_trigger_frame::MAC_AX_TF_TF15
 * Please Place Description here.
 */
enum mac_ax_trigger_frame {
	MAC_AX_TF_BT            = 0,
	MAC_AX_TF_BFRP          = 1,
	MAC_AX_TF_MU_BAR        = 2,
	MAC_AX_TF_MU_RTS        = 3,
	MAC_AX_TF_BSRP          = 4,
	MAC_AX_TF_GCR_MU_BAR    = 5,
	MAC_AX_TF_BQRP          = 6,
	MAC_AX_TF_NFRP          = 7,
	MAC_AX_TF_TF8           = 8,
	MAC_AX_TF_TF9           = 9,
	MAC_AX_TF_TF10          = 10,
	MAC_AX_TF_TF11          = 11,
	MAC_AX_TF_TF12          = 12,
	MAC_AX_TF_TF13          = 13,
	MAC_AX_TF_TF14          = 14,
	MAC_AX_TF_TF15          = 15
};

/**
 * @enum mac_ax_frame_type
 *
 * @brief mac_ax_frame_type
 *
 * @var mac_ax_frame_type::MAC_AX_FT_ACTION
 * Please Place Description here.
 * @var mac_ax_frame_type::MAC_AX_FT_ACTION_UD
 * Please Place Description here.
 * @var mac_ax_frame_type::MAC_AX_FT_TRIGGER
 * Please Place Description here.
 * @var mac_ax_frame_type::MAC_AX_FT_PM_CAM
 * Please Place Description here.
 */
enum mac_ax_frame_type {
	MAC_AX_FT_ACTION    = 0,
	MAC_AX_FT_ACTION_UD = 1,
	MAC_AX_FT_TRIGGER   = 2,
	MAC_AX_FT_PM_CAM    = 3
};

/**
 * @enum mac_ax_bd_trunc_mode
 *
 * @brief mac_ax_bd_trunc_mode
 *
 * @var mac_ax_bd_trunc_mode::MAC_AX_BD_NORM
 * Please Place Description here.
 * @var mac_ax_bd_trunc_mode::MAC_AX_BD_TRUNC
 * Please Place Description here.
 * @var mac_ax_bd_trunc_mode::MAC_AX_BD_DEF
 * Please Place Description here.
 */
enum mac_ax_bd_trunc_mode {
	MAC_AX_BD_NORM,
	MAC_AX_BD_TRUNC,
	MAC_AX_BD_DEF = 0xFE
};

/**
 * @enum mac_ax_rxbd_mode
 *
 * @brief mac_ax_rxbd_mode
 *
 * @var mac_ax_rxbd_mode::MAC_AX_RXBD_PKT
 * Please Place Description here.
 * @var mac_ax_rxbd_mode::MAC_AX_RXBD_SEP
 * Please Place Description here.
 * @var mac_ax_rxbd_mode::MAC_AX_RXBD_DEF
 * Please Place Description here.
 */
enum mac_ax_rxbd_mode {
	MAC_AX_RXBD_PKT,
	MAC_AX_RXBD_SEP,
	MAC_AX_RXBD_DEF = 0xFE
};

/**
 * @enum mac_ax_tag_mode
 *
 * @brief mac_ax_tag_mode
 *
 * @var mac_ax_tag_mode::MAC_AX_TAG_SGL
 * Please Place Description here.
 * @var mac_ax_tag_mode::MAC_AX_TAG_MULTI
 * Please Place Description here.
 * @var mac_ax_tag_mode::MAC_AX_TAG_DEF
 * Please Place Description here.
 */
enum mac_ax_tag_mode {
	MAC_AX_TAG_SGL,
	MAC_AX_TAG_MULTI,
	MAC_AX_TAG_DEF = 0xFE
};

/**
 * @enum mac_ax_rx_fecth
 *
 * @brief mac_ax_rx_fecth
 *
 * @var mac_ax_rx_fecth::MAC_AX_RX_NORM_FETCH
 * Please Place Description here.
 * @var mac_ax_rx_fecth::MAC_AX_RX_PRE_FETCH
 * Please Place Description here.
 * @var mac_ax_rx_fecth::MAC_AX_RX_FETCH_DEF
 * Please Place Description here.
 */
enum mac_ax_rx_fecth {
	MAC_AX_RX_NORM_FETCH,
	MAC_AX_RX_PRE_FETCH,
	MAC_AX_RX_FETCH_DEF = 0xFE
};

/**
 * @enum mac_ax_tx_burst
 *
 * @brief mac_ax_tx_burst
 *
 * @var mac_ax_tx_burst::MAC_AX_TX_BURST_16B
 * Please Place Description here.
 * @var mac_ax_tx_burst::MAC_AX_TX_BURST_32B
 * Please Place Description here.
 * @var mac_ax_tx_burst::MAC_AX_TX_BURST_64B
 * Please Place Description here.
 * @var mac_ax_tx_burst::MAC_AX_TX_BURST_128B
 * Please Place Description here.
 * @var mac_ax_tx_burst::MAC_AX_TX_BURST_256B
 * Please Place Description here.
 * @var mac_ax_tx_burst::MAC_AX_TX_BURST_512B
 * Please Place Description here.
 * @var mac_ax_tx_burst::MAC_AX_TX_BURST_1024B
 * Please Place Description here.
 * @var mac_ax_tx_burst::MAC_AX_TX_BURST_2048B
 * Please Place Description here.
 * @var mac_ax_tx_burst::MAC_AX_TX_BURST_DEF
 * Please Place Description here.
 */
enum mac_ax_tx_burst {
	MAC_AX_TX_BURST_16B = 0,
	MAC_AX_TX_BURST_32B = 1,
	MAC_AX_TX_BURST_64B = 2,
	MAC_AX_TX_BURST_V1_64B = 0,
	MAC_AX_TX_BURST_128B = 3,
	MAC_AX_TX_BURST_V1_128B = 1,
	MAC_AX_TX_BURST_256B = 4,
	MAC_AX_TX_BURST_V1_256B = 2,
	MAC_AX_TX_BURST_512B = 5,
	MAC_AX_TX_BURST_1024B = 6,
	MAC_AX_TX_BURST_2048B = 7,
	MAC_AX_TX_BURST_DEF = 0xFE
};

/**
 * @enum mac_ax_rx_burst
 *
 * @brief mac_ax_rx_burst
 *
 * @var mac_ax_rx_burst::MAC_AX_RX_BURST_16B
 * Please Place Description here.
 * @var mac_ax_rx_burst::MAC_AX_RX_BURST_32B
 * Please Place Description here.
 * @var mac_ax_rx_burst::MAC_AX_RX_BURST_64B
 * Please Place Description here.
 * @var mac_ax_rx_burst::MAC_AX_RX_BURST_128B
 * Please Place Description here.
 * @var mac_ax_rx_burst::MAC_AX_RX_BURST_DEF
 * Please Place Description here.
 */
enum mac_ax_rx_burst {
	MAC_AX_RX_BURST_16B = 0,
	MAC_AX_RX_BURST_32B = 1,
	MAC_AX_RX_BURST_64B = 2,
	MAC_AX_RX_BURST_V1_64B = 0,
	MAC_AX_RX_BURST_128B = 3,
	MAC_AX_RX_BURST_V1_128B = 1,
	MAC_AX_RX_BURST_V1_256B = 0,
	MAC_AX_RX_BURST_DEF = 0xFE
};

/**
 * @enum mac_ax_wd_dma_intvl
 *
 * @brief mac_ax_wd_dma_intvl
 *
 * @var mac_ax_wd_dma_intvl::MAC_AX_WD_DMA_INTVL_0S
 * Please Place Description here.
 * @var mac_ax_wd_dma_intvl::MAC_AX_WD_DMA_INTVL_256NS
 * Please Place Description here.
 * @var mac_ax_wd_dma_intvl::MAC_AX_WD_DMA_INTVL_512NS
 * Please Place Description here.
 * @var mac_ax_wd_dma_intvl::MAC_AX_WD_DMA_INTVL_768NS
 * Please Place Description here.
 * @var mac_ax_wd_dma_intvl::MAC_AX_WD_DMA_INTVL_1US
 * Please Place Description here.
 * @var mac_ax_wd_dma_intvl::MAC_AX_WD_DMA_INTVL_1_5US
 * Please Place Description here.
 * @var mac_ax_wd_dma_intvl::MAC_AX_WD_DMA_INTVL_2US
 * Please Place Description here.
 * @var mac_ax_wd_dma_intvl::MAC_AX_WD_DMA_INTVL_4US
 * Please Place Description here.
 * @var mac_ax_wd_dma_intvl::MAC_AX_WD_DMA_INTVL_8US
 * Please Place Description here.
 * @var mac_ax_wd_dma_intvl::MAC_AX_WD_DMA_INTVL_16US
 * Please Place Description here.
 * @var mac_ax_wd_dma_intvl::MAC_AX_WD_DMA_INTVL_DEF
 * Please Place Description here.
 */
enum mac_ax_wd_dma_intvl {
	MAC_AX_WD_DMA_INTVL_0S,
	MAC_AX_WD_DMA_INTVL_256NS,
	MAC_AX_WD_DMA_INTVL_512NS,
	MAC_AX_WD_DMA_INTVL_768NS,
	MAC_AX_WD_DMA_INTVL_1US,
	MAC_AX_WD_DMA_INTVL_1_5US,
	MAC_AX_WD_DMA_INTVL_2US,
	MAC_AX_WD_DMA_INTVL_4US,
	MAC_AX_WD_DMA_INTVL_8US,
	MAC_AX_WD_DMA_INTVL_16US,
	MAC_AX_WD_DMA_INTVL_DEF = 0xFE
};

/**
 * @enum mac_ax_multi_tag_num
 *
 * @brief mac_ax_multi_tag_num
 *
 * @var mac_ax_multi_tag_num::MAC_AX_TAG_NUM_1
 * Please Place Description here.
 * @var mac_ax_multi_tag_num::MAC_AX_TAG_NUM_2
 * Please Place Description here.
 * @var mac_ax_multi_tag_num::MAC_AX_TAG_NUM_3
 * Please Place Description here.
 * @var mac_ax_multi_tag_num::MAC_AX_TAG_NUM_4
 * Please Place Description here.
 * @var mac_ax_multi_tag_num::MAC_AX_TAG_NUM_5
 * Please Place Description here.
 * @var mac_ax_multi_tag_num::MAC_AX_TAG_NUM_6
 * Please Place Description here.
 * @var mac_ax_multi_tag_num::MAC_AX_TAG_NUM_7
 * Please Place Description here.
 * @var mac_ax_multi_tag_num::MAC_AX_TAG_NUM_8
 * Please Place Description here.
 * @var mac_ax_multi_tag_num::MAC_AX_TAG_NUM_DEF
 * Please Place Description here.
 */
enum mac_ax_multi_tag_num {
	MAC_AX_TAG_NUM_1,
	MAC_AX_TAG_NUM_2,
	MAC_AX_TAG_NUM_3,
	MAC_AX_TAG_NUM_4,
	MAC_AX_TAG_NUM_5,
	MAC_AX_TAG_NUM_6,
	MAC_AX_TAG_NUM_7,
	MAC_AX_TAG_NUM_8,
	MAC_AX_TAG_NUM_DEF = 0xFE
};

/**
 * @enum mac_ax_rx_adv_pref
 *
 * @brief mac_ax_rx_adv_pref
 *
 * @var mac_ax_rx_adv_pref::MAC_AX_RX_APREF_1BD
 * Please Place Description here.
 * @var mac_ax_rx_adv_pref::MAC_AX_RX_APREF_2BD
 * Please Place Description here.
 * @var mac_ax_rx_adv_pref::MAC_AX_RX_APREF_4BD
 * Please Place Description here.
 * @var mac_ax_rx_adv_pref::MAC_AX_RX_APREF_8BD
 * Please Place Description here.
 * @var mac_ax_rx_adv_pref::MAC_AX_RX_APREF_LAST
 * Please Place Description here.
 * @var mac_ax_rx_adv_pref::MAC_AX_RX_APREF_MAX
 * Please Place Description here.
 * @var mac_ax_rx_adv_pref::MAC_AX_RX_APREF_INVALID
 * Please Place Description here.
 */
enum mac_ax_rx_adv_pref {
	MAC_AX_RX_APREF_1BD = 0,
	MAC_AX_RX_APREF_2BD,
	MAC_AX_RX_APREF_4BD,
	MAC_AX_RX_APREF_8BD,
	/* keep last */
	MAC_AX_RX_APREF_LAST,
	MAC_AX_RX_APREF_MAX = MAC_AX_RX_APREF_LAST,
	MAC_AX_RX_APREF_INVALID = MAC_AX_RX_APREF_LAST,
};

/**
 * @enum mac_ax_lbc_tmr
 *
 * @brief mac_ax_lbc_tmr
 *
 * @var mac_ax_lbc_tmr::MAC_AX_LBC_TMR_8US
 * Please Place Description here.
 * @var mac_ax_lbc_tmr::MAC_AX_LBC_TMR_16US
 * Please Place Description here.
 * @var mac_ax_lbc_tmr::MAC_AX_LBC_TMR_32US
 * Please Place Description here.
 * @var mac_ax_lbc_tmr::MAC_AX_LBC_TMR_64US
 * Please Place Description here.
 * @var mac_ax_lbc_tmr::MAC_AX_LBC_TMR_128US
 * Please Place Description here.
 * @var mac_ax_lbc_tmr::MAC_AX_LBC_TMR_256US
 * Please Place Description here.
 * @var mac_ax_lbc_tmr::MAC_AX_LBC_TMR_512US
 * Please Place Description here.
 * @var mac_ax_lbc_tmr::MAC_AX_LBC_TMR_1MS
 * Please Place Description here.
 * @var mac_ax_lbc_tmr::MAC_AX_LBC_TMR_2MS
 * Please Place Description here.
 * @var mac_ax_lbc_tmr::MAC_AX_LBC_TMR_4MS
 * Please Place Description here.
 * @var mac_ax_lbc_tmr::MAC_AX_LBC_TMR_8MS
 * Please Place Description here.
 * @var mac_ax_lbc_tmr::MAC_AX_LBC_TMR_DEF
 * Please Place Description here.
 */
enum mac_ax_lbc_tmr {
	MAC_AX_LBC_TMR_8US = 0,
	MAC_AX_LBC_TMR_16US,
	MAC_AX_LBC_TMR_32US,
	MAC_AX_LBC_TMR_64US,
	MAC_AX_LBC_TMR_128US,
	MAC_AX_LBC_TMR_256US,
	MAC_AX_LBC_TMR_512US,
	MAC_AX_LBC_TMR_1MS,
	MAC_AX_LBC_TMR_2MS,
	MAC_AX_LBC_TMR_4MS,
	MAC_AX_LBC_TMR_8MS,
	MAC_AX_LBC_TMR_DEF = 0xFE
};

/**
 * @enum mac_ax_io_rcv_tmr
 *
 * @brief mac_ax_io_rcv_tmr
 *
 * @var mac_ax_lbc_tmr::MAC_AX_IO_RCV_ANA_TMR_200US
 * Please Place Description here.
 * @var mac_ax_lbc_tmr::MAC_AX_IO_RCV_ANA_TMR_300US
 * Please Place Description here.
 */
enum mac_ax_io_rcy_tmr {
	MAC_AX_IO_RCY_ANA_TMR_2MS = 24000,
	MAC_AX_IO_RCY_ANA_TMR_4MS = 48000,
	MAC_AX_IO_RCY_ANA_TMR_6MS = 72000,
	MAC_AX_IO_RCY_ANA_TMR_DEF = 0xFE
};

/*END need to check and move to other */

/**
 * @enum mac_ax_edcca_sel
 *
 * @brief mac_ax_edcca_sel
 *
 * @var mac_ax_edcca_sel::MAC_AX_EDCCA_IN_TB_CHK
 * Please Place Description here.
 * @var mac_ax_edcca_sel::MAC_AX_EDCCA_IN_SIFS_CHK
 * Please Place Description here.
 * @var mac_ax_edcca_sel::MAC_AX_EDCCA_IN_CTN_CHK
 * Please Place Description here.
 * @var mac_ax_edcca_sel::MAC_AX_EDCCA_SEL_LAST
 * Please Place Description here.
 * @var mac_ax_edcca_sel::MAC_AX_EDCCA_SEL_MAX
 * Please Place Description here.
 * @var mac_ax_edcca_sel::MAC_AX_EDCCA_SEL_INVALID
 * Please Place Description here.
 */
enum mac_ax_edcca_sel {
	MAC_AX_EDCCA_IN_TB_CHK,
	MAC_AX_EDCCA_IN_SIFS_CHK,
	MAC_AX_EDCCA_IN_CTN_CHK,

	/* keep last */
	MAC_AX_EDCCA_SEL_LAST,
	MAC_AX_EDCCA_SEL_MAX = MAC_AX_EDCCA_SEL_LAST,
	MAC_AX_EDCCA_SEL_INVALID = MAC_AX_EDCCA_SEL_LAST,
};

/**
 * @enum mac_ax_chip_id
 *
 * @brief mac_ax_chip_id
 *
 * @var mac_ax_chip_id::MAC_AX_CHIP_ID_8852A
 * Please Place Description here.
 * @var mac_ax_chip_id::MAC_AX_CHIP_ID_8852B
 * Please Place Description here.
 * @var mac_ax_chip_id::MAC_AX_CHIP_ID_8852C
 * Please Place Description here.
 * @var mac_ax_chip_id::MAC_AX_CHIP_ID_8192XB
 * Please Place Description here.
 * @var mac_ax_chip_id::MAC_AX_CHIP_ID_8851B
 * Please Place Description here.
 * @var mac_ax_chip_id::MAC_AX_CHIP_ID_LAST
 * Please Place Description here.
 * @var mac_ax_chip_id::MAC_AX_CHIP_ID_MAX
 * Please Place Description here.
 * @var mac_ax_chip_id::MAC_AX_CHIP_ID_INVALID
 * Please Place Description here.
 */
enum mac_ax_chip_id {
	MAC_AX_CHIP_ID_8852A = 0,
	MAC_AX_CHIP_ID_8852B,
	MAC_AX_CHIP_ID_8852C,
	MAC_AX_CHIP_ID_8192XB,
	MAC_AX_CHIP_ID_8851B,
	MAC_AX_CHIP_ID_8851E,
	MAC_AX_CHIP_ID_8852D,
	MAC_BE_CHIP_ID_1115E, /*for BE test chip*/
	MAC_BE_CHIP_ID_8922A,
	MAC_BE_CHIP_ID_8952A,
	MAC_BE_CHIP_ID_8934A,

	/* keep last */
	MAC_AX_CHIP_ID_LAST,
	MAC_AX_CHIP_ID_MAX = MAC_AX_CHIP_ID_LAST,
	MAC_AX_CHIP_ID_INVALID = MAC_AX_CHIP_ID_LAST,
};

/**
 * @enum mac_ax_wdbk_mode
 *
 * @brief mac_ax_wdbk_mode
 *
 * @var mac_ax_wdbk_mode::MAC_AX_WDBK_MODE_SINGLE_BK
 * Please Place Description here.
 * @var mac_ax_wdbk_mode::MAC_AX_WDBK_MODE_GRP_BK
 * Please Place Description here.
 * @var mac_ax_wdbk_mode::MAC_AX_WDBK_MODE_LAST
 * Please Place Description here.
 * @var mac_ax_wdbk_mode::MAC_AX_WDBK_MODE_MAX
 * Please Place Description here.
 * @var mac_ax_wdbk_mode::MAC_AX_WDBK_MODE_INVALID
 * Please Place Description here.
 */
enum mac_ax_wdbk_mode {
	MAC_AX_WDBK_MODE_SINGLE_BK = 0,
	MAC_AX_WDBK_MODE_GRP_BK = 1,

	/* keep last */
	MAC_AX_WDBK_MODE_LAST,
	MAC_AX_WDBK_MODE_MAX = MAC_AX_WDBK_MODE_LAST,
	MAC_AX_WDBK_MODE_INVALID = MAC_AX_WDBK_MODE_LAST,
};

/**
 * @enum mac_ax_rty_bk_mode
 *
 * @brief mac_ax_rty_bk_mode
 *
 * @var mac_ax_rty_bk_mode::MAC_AX_RTY_BK_MODE_AGG
 * Please Place Description here.
 * @var mac_ax_rty_bk_mode::MAC_AX_RTY_BK_MODE_RATE_FB
 * Please Place Description here.
 * @var mac_ax_rty_bk_mode::MAC_AX_RTY_BK_MODE_BK
 * Please Place Description here.
 * @var mac_ax_rty_bk_mode::MAC_AX_RTY_BK_MODE_LAST
 * Please Place Description here.
 * @var mac_ax_rty_bk_mode::MAC_AX_RTY_BK_MODE_MAX
 * Please Place Description here.
 * @var mac_ax_rty_bk_mode::MAC_AX_RTY_BK_MODE_INVALID
 * Please Place Description here.
 */
enum mac_ax_rty_bk_mode {
	MAC_AX_RTY_BK_MODE_AGG = 0x0,
	MAC_AX_RTY_BK_MODE_RATE_FB = 0x1,
	MAC_AX_RTY_BK_MODE_BK = 0x2,

	/* keep last */
	MAC_AX_RTY_BK_MODE_LAST,
	MAC_AX_RTY_BK_MODE_MAX = MAC_AX_RTY_BK_MODE_LAST,
	MAC_AX_RTY_BK_MODE_INVALID = MAC_AX_RTY_BK_MODE_LAST,
};

/**
 * @enum mac_ax_ch_busy_cnt_ctrl
 *
 * @brief mac_ax_ch_busy_cnt_ctrl
 *
 * @var mac_ax_ch_busy_cnt_ctrl::MAC_AX_CH_BUSY_CNT_CTRL_CNT_REF
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ctrl::MAC_AX_CH_BUSY_CNT_CTRL_CNT_BUSY_RST
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ctrl::MAC_AX_CH_BUSY_CNT_CTRL_CNT_IDLE_RST
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ctrl::MAC_AX_CH_BUSY_CNT_CTRL_CNT_EN
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ctrl::MAC_AX_CH_BUSY_CNT_CTRL_CNT_DIS
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ctrl::MAC_AX_CH_BUSY_CNT_CTRL_LAST
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ctrl::MAC_AX_CH_BUSY_CNT_CTRL_MAX
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ctrl::MAC_AX_CH_BUSY_CNT_CTRL_INVALID
 * Please Place Description here.
 */
enum mac_ax_ch_busy_cnt_ctrl {
	MAC_AX_CH_BUSY_CNT_CTRL_CNT_REF,
	MAC_AX_CH_BUSY_CNT_CTRL_CNT_BUSY_RST,
	MAC_AX_CH_BUSY_CNT_CTRL_CNT_IDLE_RST,
	MAC_AX_CH_BUSY_CNT_CTRL_CNT_RST,
	MAC_AX_CH_BUSY_CNT_CTRL_CNT_EN,
	MAC_AX_CH_BUSY_CNT_CTRL_CNT_DIS,

	/* keep last */
	MAC_AX_CH_BUSY_CNT_CTRL_LAST,
	MAC_AX_CH_BUSY_CNT_CTRL_MAX = MAC_AX_CH_BUSY_CNT_CTRL_LAST,
	MAC_AX_CH_BUSY_CNT_CTRL_INVALID = MAC_AX_CH_BUSY_CNT_CTRL_LAST,
};

/**
 * @enum mac_ax_func_sw
 *
 * @brief mac_ax_func_sw
 *
 * @var mac_ax_func_sw::MAC_AX_FUNC_DIS
 * Please Place Description here.
 * @var mac_ax_func_sw::MAC_AX_FUNC_EN
 * Please Place Description here.
 * @var mac_ax_func_sw::MAC_AX_FUNC_DEF
 * Please Place Description here.
 */
enum mac_ax_func_sw {
	MAC_AX_FUNC_DIS = 0,
	MAC_AX_FUNC_EN,
	MAC_AX_FUNC_DEF,

	/* for SER stop without L0 SER dedicated */
	MAC_AX_FUNC_WO_RECVY_DIS,
	MAC_AX_FUNC_WO_RECVY_EN,
};

/**
 * @enum mac_ax_twt_nego_tp
 *
 * @brief mac_ax_twt_nego_tp
 *
 * @var mac_ax_twt_nego_tp::MAC_AX_TWT_NEGO_TP_IND
 * Please Place Description here.
 * @var mac_ax_twt_nego_tp::MAC_AX_TWT_NEGO_TP_WAKE
 * Please Place Description here.
 * @var mac_ax_twt_nego_tp::MAC_AX_TWT_NEGO_TP_BRC
 * Please Place Description here.
 * @var mac_ax_twt_nego_tp::MAC_AX_TWT_NEGO_TP_LAST
 * Please Place Description here.
 * @var mac_ax_twt_nego_tp::MAC_AX_TWT_NEGO_TP_MAX
 * Please Place Description here.
 * @var mac_ax_twt_nego_tp::MAC_AX_TWT_NEGO_TP_INVALID
 * Please Place Description here.
 */
enum mac_ax_twt_nego_tp {
	MAC_AX_TWT_NEGO_TP_IND,
	MAC_AX_TWT_NEGO_TP_WAKE,
	MAC_AX_TWT_NEGO_TP_BRC,

	/* keep last */
	MAC_AX_TWT_NEGO_TP_LAST,
	MAC_AX_TWT_NEGO_TP_MAX = MAC_AX_TWT_NEGO_TP_LAST,
	MAC_AX_TWT_NEGO_TP_INVALID = MAC_AX_TWT_NEGO_TP_LAST,
};

/**
 * @enum mac_ax_twt_act_tp
 *
 * @brief mac_ax_twt_act_tp
 *
 * @var mac_ax_twt_act_tp::MAC_AX_TWT_ACT_TP_ADD
 * Please Place Description here.
 * @var mac_ax_twt_act_tp::MAC_AX_TWT_ACT_TP_DEL
 * Please Place Description here.
 * @var mac_ax_twt_act_tp::MAC_AX_TWT_ACT_TP_MOD
 * Please Place Description here.
 * @var mac_ax_twt_act_tp::MAC_AX_TWT_ACT_TP_LAST
 * Please Place Description here.
 * @var mac_ax_twt_act_tp::MAC_AX_TWT_ACT_TP_MAX
 * Please Place Description here.
 * @var mac_ax_twt_act_tp::MAC_AX_TWT_ACT_TP_INVALID
 * Please Place Description here.
 */
enum mac_ax_twt_act_tp {
	MAC_AX_TWT_ACT_TP_ADD,
	MAC_AX_TWT_ACT_TP_DEL,
	MAC_AX_TWT_ACT_TP_MOD,

	/* keep last */
	MAC_AX_TWT_ACT_TP_LAST,
	MAC_AX_TWT_ACT_TP_MAX = MAC_AX_TWT_ACT_TP_LAST,
	MAC_AX_TWT_ACT_TP_INVALID = MAC_AX_TWT_ACT_TP_LAST,
};

/**
 * @enum mac_ax_twtact_act_tp
 *
 * @brief mac_ax_twtact_act_tp
 *
 * @var mac_ax_twtact_act_tp::MAC_AX_TWTACT_ACT_TP_ADD
 * Please Place Description here.
 * @var mac_ax_twtact_act_tp::MAC_AX_TWTACT_ACT_TP_DEL
 * Please Place Description here.
 * @var mac_ax_twtact_act_tp::MAC_AX_TWTACT_ACT_TP_TRMNT
 * Please Place Description here.
 * @var mac_ax_twtact_act_tp::MAC_AX_TWTACT_ACT_TP_SUS
 * Please Place Description here.
 * @var mac_ax_twtact_act_tp::MAC_AX_TWTACT_ACT_TP_RSUM
 * Please Place Description here.
 * @var mac_ax_twtact_act_tp::MAC_AX_TWTACT_ACT_TP_LAST
 * Please Place Description here.
 * @var mac_ax_twtact_act_tp::MAC_AX_TWTACT_ACT_TP_MAX
 * Please Place Description here.
 * @var mac_ax_twtact_act_tp::MAC_AX_TWTACT_ACT_TP_INVALID
 * Please Place Description here.
 */
enum mac_ax_twtact_act_tp {
	MAC_AX_TWTACT_ACT_TP_ADD,
	MAC_AX_TWTACT_ACT_TP_DEL,
	MAC_AX_TWTACT_ACT_TP_TRMNT,
	MAC_AX_TWTACT_ACT_TP_SUS,
	MAC_AX_TWTACT_ACT_TP_RSUM,

	/* keep last */
	MAC_AX_TWTACT_ACT_TP_LAST,
	MAC_AX_TWTACT_ACT_TP_MAX = MAC_AX_TWTACT_ACT_TP_LAST,
	MAC_AX_TWTACT_ACT_TP_INVALID = MAC_AX_TWTACT_ACT_TP_LAST,
};

/**
 * @enum mac_ax_twt_waitanno_tp
 *
 * @brief mac_ax_twt_waitanno_tp
 *
 * @var mac_ax_twt_waitanno_tp::MAC_AX_TWT_ANNOWAIT_DIS_MACID
 * Please Place Description here.
 * @var mac_ax_twt_waitanno_tp::MAC_AX_TWT_ANNOWAIT_EN_MACID
 * Please Place Description here.
 */
enum mac_ax_twt_waitanno_tp {
	MAC_AX_TWT_ANNOWAIT_DIS_MACID,
	MAC_AX_TWT_ANNOWAIT_EN_MACID,
};

/**
 * @enum mac_ax_tsf_sync_act
 *
 * @brief mac_ax_tsf_sync_act
 *
 * @var mac_ax_tsf_sync_act::MAC_AX_TSF_SYNC_NOW_ONCE
 * Please Place Description here.
 * @var mac_ax_tsf_sync_act::MAC_AX_TSF_EN_SYNC_AUTO
 * Please Place Description here.
 * @var mac_ax_tsf_sync_act::MAC_AX_TSF_DIS_SYNC_AUTO
 * Please Place Description here.
 */
enum mac_ax_tsf_sync_act {
	MAC_AX_TSF_SYNC_NOW_ONCE,
	MAC_AX_TSF_EN_SYNC_AUTO,
	MAC_AX_TSF_DIS_SYNC_AUTO
};

/**
 * @enum mac_ax_slot_time
 *
 * @brief mac_ax_slot_time
 *
 * @var mac_ax_slot_time::MAC_AX_SLOT_TIME_BAND0_9US
 * Please Place Description here.
 * @var mac_ax_slot_time::MAC_AX_SLOT_TIME_BAND0_20US
 * Please Place Description here.
 * @var mac_ax_slot_time::MAC_AX_SLOT_TIME_BAND1_9US
 * Please Place Description here.
 * @var mac_ax_slot_time::MAC_AX_SLOT_TIME_BAND1_20US
 * Please Place Description here.
 */
enum mac_ax_slot_time {
	MAC_AX_SLOT_TIME_BAND0_9US,
	MAC_AX_SLOT_TIME_BAND0_20US,
	MAC_AX_SLOT_TIME_BAND1_9US,
	MAC_AX_SLOT_TIME_BAND1_20US,
};

/*------------------------Define HCI related enum ----------------------------*/

/**
 * @enum mac_ax_pcie_func_ctrl
 *
 * @brief mac_ax_pcie_func_ctrl
 *
 * @var mac_ax_pcie_func_ctrl::MAC_AX_PCIE_DISABLE
 * Please Place Description here.
 * @var mac_ax_pcie_func_ctrl::MAC_AX_PCIE_ENABLE
 * Please Place Description here.
 * @var mac_ax_pcie_func_ctrl::MAC_AX_PCIE_DEFAULT
 * Please Place Description here.
 * @var mac_ax_pcie_func_ctrl::MAC_AX_PCIE_IGNORE
 * Please Place Description here.
 */
enum mac_ax_pcie_func_ctrl {
	MAC_AX_PCIE_DISABLE = 0,
	MAC_AX_PCIE_ENABLE = 1,
	MAC_AX_PCIE_DEFAULT = 0xFE,
	MAC_AX_PCIE_IGNORE = 0xFF
};

/**
 * @enum mac_ax_pcie_phy
 *
 * @brief mac_ax_pcie_phy
 *
 * @var mac_ax_pcie_phy::MAC_AX_PCIE_PHY_GEN1
 * Please Place Description here.
 * @var mac_ax_pcie_phy::MAC_AX_PCIE_PHY_GEN2
 * Please Place Description here.
 * @var mac_ax_pcie_phy::MAC_AX_PCIE_PHY_GEN1_UNDEFINE
 * Please Place Description here.
 */
enum mac_ax_pcie_phy {
	MAC_AX_PCIE_PHY_GEN1 = 1,
	MAC_AX_PCIE_PHY_GEN2 = 2,
	MAC_AX_PCIE_PHY_GEN1_UNDEFINE = 0x7F,
};

/**
 * @enum mac_ax_pcie_clkdly
 *
 * @brief mac_ax_pcie_clkdly
 *
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_0
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_5US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_6US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_11US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_15US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_19US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_25US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_30US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_38US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_50US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_64US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_100US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_128US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_150US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_192US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_200US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_300US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_400US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_500US
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_1MS
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_3MS
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_5MS
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_10MS
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_R_ERR
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_DEF
 * Please Place Description here.
 * @var mac_ax_pcie_clkdly::MAC_AX_PCIE_CLKDLY_IGNORE
 * Please Place Description here.
 */
enum mac_ax_pcie_clkdly {
	MAC_AX_PCIE_CLKDLY_0 = 0,
	MAC_AX_PCIE_CLKDLY_5US = 1,
	MAC_AX_PCIE_CLKDLY_6US = 2,
	MAC_AX_PCIE_CLKDLY_11US = 3,
	MAC_AX_PCIE_CLKDLY_15US = 4,
	MAC_AX_PCIE_CLKDLY_19US = 5,
	MAC_AX_PCIE_CLKDLY_25US = 6,
	MAC_AX_PCIE_CLKDLY_30US = 7,
	MAC_AX_PCIE_CLKDLY_38US = 8,
	MAC_AX_PCIE_CLKDLY_50US = 9,
	MAC_AX_PCIE_CLKDLY_64US = 10,
	MAC_AX_PCIE_CLKDLY_100US = 11,
	MAC_AX_PCIE_CLKDLY_128US = 12,
	MAC_AX_PCIE_CLKDLY_150US = 13,
	MAC_AX_PCIE_CLKDLY_192US = 14,
	MAC_AX_PCIE_CLKDLY_200US = 15,
	MAC_AX_PCIE_CLKDLY_300US = 16,
	MAC_AX_PCIE_CLKDLY_400US = 17,
	MAC_AX_PCIE_CLKDLY_500US = 18,
	MAC_AX_PCIE_CLKDLY_1MS = 19,
	MAC_AX_PCIE_CLKDLY_3MS = 20,
	MAC_AX_PCIE_CLKDLY_5MS = 21,
	MAC_AX_PCIE_CLKDLY_10MS = 22,
	MAC_AX_PCIE_CLKDLY_V1_0 = 0,
	MAC_AX_PCIE_CLKDLY_V1_16US = 1,
	MAC_AX_PCIE_CLKDLY_V1_32US = 2,
	MAC_AX_PCIE_CLKDLY_V1_64US = 3,
	MAC_AX_PCIE_CLKDLY_V1_80US = 4,
	MAC_AX_PCIE_CLKDLY_V1_96US = 5,
	MAC_AX_PCIE_CLKDLY_R_ERR = 0xFD,
	MAC_AX_PCIE_CLKDLY_DEF = 0xFE,
	MAC_AX_PCIE_CLKDLY_IGNORE = 0xFF
};

/**
 * @enum mac_ax_rx_ch
 *
 * @brief mac_ax_rx_ch
 *
 * @var mac_ax_rx_ch::MAC_AX_RX_CH_RXQ
 * Please Place Description here.
 * @var mac_ax_rx_ch::MAC_AX_RX_CH_RPQ
 * Please Place Description here.
 * @var mac_ax_rx_ch::MAC_AX_RX_CH_NUM
 * Please Place Description here.
 */
enum mac_ax_rx_ch {
	MAC_AX_RX_CH_RXQ = 0,
	MAC_AX_RX_CH_RPQ,
	MAC_AX_RX_CH_NUM
};

/**
 * @enum mac_be_rx_ch
 *
 * @brief mac_be_rx_ch
 *
 * @var mac_be_rx_ch::MAC_BE_RX_CH_RXQ
 * Please Place Description here.
 * @var mac_be_rx_ch::MAC_BE_RX_CH_RPQ
 * Please Place Description here.
 * @var mac_be_rx_ch::MAC_BE_RX_CH_RXQ1
 * Please Place Description here.
 * @var mac_be_rx_ch::MAC_BE_RX_CH_RPQ1
 * Please Place Description here.
 * @var mac_be_rx_ch::MAC_BE_RX_CH_NUM
 * Please Place Description here.
 */
enum mac_be_rx_ch {
	MAC_BE_RX_CH_RXQ = 0,
	MAC_BE_RX_CH_RPQ,
	MAC_BE_RX_CH_RXQ1,
	MAC_BE_RX_CH_RPQ1,
	MAC_BE_RX_CH_NUM
};

/**
 * @enum mac_ax_pcie_l1dly
 *
 * @brief mac_ax_pcie_l1dly
 *
 * @var mac_ax_pcie_l1dly::MAC_AX_PCIE_L1DLY_16US
 * Please Place Description here.
 * @var mac_ax_pcie_l1dly::MAC_AX_PCIE_L1DLY_32US
 * Please Place Description here.
 * @var mac_ax_pcie_l1dly::MAC_AX_PCIE_L1DLY_64US
 * Please Place Description here.
 * @var mac_ax_pcie_l1dly::MAC_AX_PCIE_L1DLY_INFI
 * Please Place Description here.
 * @var mac_ax_pcie_l1dly::MAC_AX_PCIE_L1DLY_R_ERR
 * Please Place Description here.
 * @var mac_ax_pcie_l1dly::MAC_AX_PCIE_L1DLY_DEF
 * Please Place Description here.
 * @var mac_ax_pcie_l1dly::MAC_AX_PCIE_L1DLY_IGNORE
 * Please Place Description here.
 */
enum mac_ax_pcie_l1dly {
	MAC_AX_PCIE_L1DLY_16US = 0,
	MAC_AX_PCIE_L1DLY_32US = 1,
	MAC_AX_PCIE_L1DLY_64US = 2,
	MAC_AX_PCIE_L1DLY_INFI = 3,
	MAC_AX_PCIE_L1DLY_R_ERR = 0xFD,
	MAC_AX_PCIE_L1DLY_DEF = 0xFE,
	MAC_AX_PCIE_L1DLY_IGNORE = 0xFF
};

/**
 * @enum mac_ax_pcie_l0sdly
 *
 * @brief mac_ax_pcie_l0sdly
 *
 * @var mac_ax_pcie_l0sdly::MAC_AX_PCIE_L0SDLY_1US
 * Please Place Description here.
 * @var mac_ax_pcie_l0sdly::MAC_AX_PCIE_L0SDLY_2US
 * Please Place Description here.
 * @var mac_ax_pcie_l0sdly::MAC_AX_PCIE_L0SDLY_3US
 * Please Place Description here.
 * @var mac_ax_pcie_l0sdly::MAC_AX_PCIE_L0SDLY_4US
 * Please Place Description here.
 * @var mac_ax_pcie_l0sdly::MAC_AX_PCIE_L0SDLY_5US
 * Please Place Description here.
 * @var mac_ax_pcie_l0sdly::MAC_AX_PCIE_L0SDLY_6US
 * Please Place Description here.
 * @var mac_ax_pcie_l0sdly::MAC_AX_PCIE_L0SDLY_7US
 * Please Place Description here.
 * @var mac_ax_pcie_l0sdly::MAC_AX_PCIE_L0SDLY_R_ERR
 * Please Place Description here.
 * @var mac_ax_pcie_l0sdly::MAC_AX_PCIE_L0SDLY_DEF
 * Please Place Description here.
 * @var mac_ax_pcie_l0sdly::MAC_AX_PCIE_L0SDLY_IGNORE
 * Please Place Description here.
 */
enum mac_ax_pcie_l0sdly {
	MAC_AX_PCIE_L0SDLY_1US = 0,
	MAC_AX_PCIE_L0SDLY_2US = 1,
	MAC_AX_PCIE_L0SDLY_3US = 2,
	MAC_AX_PCIE_L0SDLY_4US = 3,
	MAC_AX_PCIE_L0SDLY_5US = 4,
	MAC_AX_PCIE_L0SDLY_6US = 5,
	MAC_AX_PCIE_L0SDLY_7US = 6,
	MAC_AX_PCIE_L0SDLY_R_ERR = 0xFD,
	MAC_AX_PCIE_L0SDLY_DEF = 0xFE,
	MAC_AX_PCIE_L0SDLY_IGNORE = 0xFF
};

/**
 * @enum mac_ax_pcie_ltr_spc
 *
 * @brief mac_ax_pcie_ltr_spc
 *
 * @var mac_ax_pcie_ltr_spc::MAC_AX_PCIE_LTR_SPC_10US
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_spc::MAC_AX_PCIE_LTR_SPC_100US
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_spc::MAC_AX_PCIE_LTR_SPC_500US
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_spc::MAC_AX_PCIE_LTR_SPC_1MS
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_spc::MAC_AX_PCIE_LTR_SPC_R_ERR
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_spc::MAC_AX_PCIE_LTR_SPC_DEF
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_spc::MAC_AX_PCIE_LTR_SPC_IGNORE
 * Please Place Description here.
 */
enum mac_ax_pcie_ltr_spc {
	MAC_AX_PCIE_LTR_SPC_10US = 0,
	MAC_AX_PCIE_LTR_SPC_100US = 1,
	MAC_AX_PCIE_LTR_SPC_500US = 2,
	MAC_AX_PCIE_LTR_SPC_1MS = 3,
	MAC_AX_PCIE_LTR_SPC_R_ERR = 0xFD,
	MAC_AX_PCIE_LTR_SPC_DEF = 0xFE,
	MAC_AX_PCIE_LTR_SPC_IGNORE = 0xFF
};

/**
 * @enum mac_ax_pcie_ltr_idle_timer
 *
 * @brief mac_ax_pcie_ltr_idle_timer
 *
 * @var mac_ax_pcie_ltr_idle_timer::MAC_AX_PCIE_LTR_IDLE_TIMER_1US
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_idle_timer::MAC_AX_PCIE_LTR_IDLE_TIMER_10US
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_idle_timer::MAC_AX_PCIE_LTR_IDLE_TIMER_100US
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_idle_timer::MAC_AX_PCIE_LTR_IDLE_TIMER_200US
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_idle_timer::MAC_AX_PCIE_LTR_IDLE_TIMER_400US
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_idle_timer::MAC_AX_PCIE_LTR_IDLE_TIMER_800US
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_idle_timer::MAC_AX_PCIE_LTR_IDLE_TIMER_1_6MS
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_idle_timer::MAC_AX_PCIE_LTR_IDLE_TIMER_3_2MS
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_idle_timer::MAC_AX_PCIE_LTR_IDLE_TIMER_R_ERR
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_idle_timer::MAC_AX_PCIE_LTR_IDLE_TIMER_DEF
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_idle_timer::MAC_AX_PCIE_LTR_IDLE_TIMER_IGNORE
 * Please Place Description here.
 */
enum mac_ax_pcie_ltr_idle_timer {
	MAC_AX_PCIE_LTR_IDLE_TIMER_1US = 0,
	MAC_AX_PCIE_LTR_IDLE_TIMER_10US = 1,
	MAC_AX_PCIE_LTR_IDLE_TIMER_100US = 2,
	MAC_AX_PCIE_LTR_IDLE_TIMER_200US = 3,
	MAC_AX_PCIE_LTR_IDLE_TIMER_400US = 4,
	MAC_AX_PCIE_LTR_IDLE_TIMER_800US = 5,
	MAC_AX_PCIE_LTR_IDLE_TIMER_1_6MS = 6,
	MAC_AX_PCIE_LTR_IDLE_TIMER_3_2MS = 7,
	MAC_AX_PCIE_LTR_IDLE_TIMER_R_ERR = 0xFD,
	MAC_AX_PCIE_LTR_IDLE_TIMER_DEF = 0xFE,
	MAC_AX_PCIE_LTR_IDLE_TIMER_IGNORE = 0xFF
};

/**
 * @enum mac_ax_pcie_ltr_sw_ctrl
 *
 * @brief mac_ax_pcie_ltr_sw_ctrl
 *
 * @var mac_ax_pcie_ltr_sw_ctrl::MAC_AX_PCIE_LTR_SW_ACT
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_sw_ctrl::MAC_AX_PCIE_LTR_SW_IDLE
 * Please Place Description here.
 */
enum mac_ax_pcie_ltr_sw_ctrl {
	MAC_AX_PCIE_LTR_SW_ACT,
	MAC_AX_PCIE_LTR_SW_IDLE
};

/**
 * @enum mac_ax_pcie_driving_ctrl
 *
 * @brief mac_ax_pcie_driving_ctrl
 *
 * @var mac_ax_pcie_driving_ctrl::MAC_AX_PCIE_DRIVING_3
 * Please Place Description here.
 * @var mac_ax_pcie_driving_ctrl::MAC_AX_PCIE_DRIVING_4
 * Please Place Description here.
 * @var mac_ax_pcie_driving_ctrl::MAC_AX_PCIE_DRIVING_5
 * Please Place Description here.
 * @var mac_ax_pcie_driving_ctrl::MAC_AX_PCIE_DRIVING_6
 * Please Place Description here.
 * @var mac_ax_pcie_driving_ctrl::MAC_AX_PCIE_DRIVING_7
 * Please Place Description here.
 */
enum mac_ax_pcie_driving_ctrl {
	MAC_AX_PCIE_DRIVING_3 = 0x44C3,
	MAC_AX_PCIE_DRIVING_4 = 0x44C4,
	MAC_AX_PCIE_DRIVING_5 = 0x44C5,
	MAC_AX_PCIE_DRIVING_6 = 0x44C6,
	MAC_AX_PCIE_DRIVING_7 = 0x44C7
};

/**
 * @enum mac_ax_sdio_clk_mon
 *
 * @brief mac_ax_sdio_clk_mon
 *
 * @var mac_ax_sdio_clk_mon::MAC_AX_SDIO_CLK_MON_SHORT
 * Please Place Description here.
 * @var mac_ax_sdio_clk_mon::MAC_AX_SDIO_CLK_MON_LONG
 * Please Place Description here.
 * @var mac_ax_sdio_clk_mon::MAC_AX_SDIO_CLK_MON_USER_DEFINE
 * Please Place Description here.
 * @var mac_ax_sdio_clk_mon::MAC_AX_SDIO_CLK_MON_LAST
 * Please Place Description here.
 * @var mac_ax_sdio_clk_mon::MAC_AX_SDIO_CLK_MON_MAX
 * Please Place Description here.
 * @var mac_ax_sdio_clk_mon::MAC_AX_SDIO_CLK_MON_INVALID
 * Please Place Description here.
 */
enum mac_ax_sdio_clk_mon {
	MAC_AX_SDIO_CLK_MON_SHORT,
	MAC_AX_SDIO_CLK_MON_LONG,
	MAC_AX_SDIO_CLK_MON_USER_DEFINE,

	/* keep last */
	MAC_AX_SDIO_CLK_MON_LAST,
	MAC_AX_SDIO_CLK_MON_MAX = MAC_AX_SDIO_CLK_MON_LAST,
	MAC_AX_SDIO_CLK_MON_INVALID = MAC_AX_SDIO_CLK_MON_LAST,
};

/**
 * @enum mac_ax_rx_ppdu_type
 *
 * @brief mac_ax_rx_ppdu_type
 *
 * @var mac_ax_rx_ppdu_type::MAC_AX_RX_CCK
 * Please Place Description here.
 * @var mac_ax_rx_ppdu_type::MAC_AX_RX_OFDM
 * Please Place Description here.
 * @var mac_ax_rx_ppdu_type::MAC_AX_RX_HT
 * Please Place Description here.
 * @var mac_ax_rx_ppdu_type::MAC_AX_RX_VHT_SU
 * Please Place Description here.
 * @var mac_ax_rx_ppdu_type::MAC_AX_RX_VHT_MU
 * Please Place Description here.
 * @var mac_ax_rx_ppdu_type::MAC_AX_RX_HE_SU
 * Please Place Description here.
 * @var mac_ax_rx_ppdu_type::MAC_AX_RX_HE_MU
 * Please Place Description here.
 * @var mac_ax_rx_ppdu_type::MAC_AX_RX_HE_TB
 * Please Place Description here.
 * @var mac_ax_rx_ppdu_type::MAC_AX_RX_PPDU_LAST
 * Please Place Description here.
 * @var mac_ax_rx_ppdu_type::MAC_AX_RX_PPDU_MAX
 * Please Place Description here.
 * @var mac_ax_rx_ppdu_type::MAC_AX_RX_PPDU_INVLAID
 * Please Place Description here.
 */
enum mac_ax_rx_ppdu_type {
	MAC_AX_RX_CCK,
	MAC_AX_RX_OFDM,
	MAC_AX_RX_HT,
	MAC_AX_RX_VHT_SU,
	MAC_AX_RX_VHT_MU,
	MAC_AX_RX_HE_SU,
	MAC_AX_RX_HE_MU,
	MAC_AX_RX_HE_TB,

	MAC_AX_RX_PPDU_LAST,
	MAC_AX_RX_PPDU_MAX = MAC_AX_RX_PPDU_LAST,
	MAC_AX_RX_PPDU_INVLAID = MAC_AX_RX_PPDU_LAST,
};

/**
 * @enum mac_ax_net_type
 *
 * @brief mac_ax_net_type
 *
 * @var mac_ax_net_type::MAC_AX_NET_TYPE_NO_LINK
 * Please Place Description here.
 * @var mac_ax_net_type::MAC_AX_NET_TYPE_ADHOC
 * Please Place Description here.
 * @var mac_ax_net_type::MAC_AX_NET_TYPE_INFRA
 * Please Place Description here.
 * @var mac_ax_net_type::MAC_AX_NET_TYPE_AP
 * Please Place Description here.
 */
enum mac_ax_net_type {
	MAC_AX_NET_TYPE_NO_LINK,
	MAC_AX_NET_TYPE_ADHOC,
	MAC_AX_NET_TYPE_INFRA,
	MAC_AX_NET_TYPE_AP,

	MAC_AX_NET_TYPE_LAST,
	MAC_AX_NET_TYPE_MAX = MAC_AX_NET_TYPE_LAST,
	MAC_AX_NET_TYPE_INVLAID = MAC_AX_NET_TYPE_LAST,
};

/**
 * @enum mac_ax_disable_rf_func
 *
 * @brief mac_ax_disable_rf_func
 *
 * @var mac_ax_disable_rf_func::MAC_AX_DISABLE_RF_FUNC_FBVR
 * Please Place Description here.
 * @var mac_ax_disable_rf_func::MAC_AX_DISABLE_RF_FUNC_MAX
 * Please Place Description here.
 */
enum mac_ax_disable_rf_func {
	MAC_AX_DISABLE_RF_FUNC_FBVR,
	MAC_AX_DISABLE_RF_FUNC_MAX
};

/**
 * @enum mac_ax_self_role
 *
 * @brief mac_ax_self_role
 *
 * @var mac_ax_self_role::MAC_AX_SELF_ROLE_CLIENT
 * Please Place Description here.
 * @var mac_ax_self_role::MAC_AX_SELF_ROLE_AP
 * Please Place Description here.
 * @var mac_ax_self_role::MAC_AX_SELF_ROLE_AP_CLIENT
 * Please Place Description here.
 */
enum mac_ax_self_role {
	MAC_AX_SELF_ROLE_CLIENT,
	MAC_AX_SELF_ROLE_AP,
	MAC_AX_SELF_ROLE_AP_CLIENT
};

/**
 * @enum mac_ax_wifi_role
 *
 * @brief mac_ax_wifi_role
 *
 * @var mac_ax_wifi_role::MAC_AX_WIFI_ROLE_NONE
 * Please Place Description here.
 * @var mac_ax_wifi_role::MAC_AX_WIFI_ROLE_STATION
 * Please Place Description here.
 * @var mac_ax_wifi_role::MAC_AX_WIFI_ROLE_AP
 * Please Place Description here.
 * @var mac_ax_wifi_role::MAC_AX_WIFI_ROLE_VAP
 * Please Place Description here.
 * @var mac_ax_wifi_role::MAC_AX_WIFI_ROLE_ADHOC
 * Please Place Description here.
 * @var mac_ax_wifi_role::MAC_AX_WIFI_ROLE_ADHOC_MASTER
 * Please Place Description here.
 * @var mac_ax_wifi_role::MAC_AX_WIFI_ROLE_MESH
 * Please Place Description here.
 * @var mac_ax_wifi_role::MAC_AX_WIFI_ROLE_MONITOR
 * Please Place Description here.
 * @var mac_ax_wifi_role::MAC_AX_WIFI_ROLE_P2P_DEVICE
 * Please Place Description here.
 * @var mac_ax_wifi_role::MAC_AX_WIFI_ROLE_P2P_GC
 * Please Place Description here.
 * @var mac_ax_wifi_role::MAC_AX_WIFI_ROLE_P2P_GO
 * Please Place Description here.
 * @var mac_ax_wifi_role::MAC_AX_WIFI_ROLE_NAN
 * Please Place Description here.
 * @var mac_ax_wifi_role::MAC_AX_WIFI_ROLE_MLME_MAX
 * Please Place Description here.
 */
enum mac_ax_wifi_role {
	MAC_AX_WIFI_ROLE_NONE,
	MAC_AX_WIFI_ROLE_STATION,
	MAC_AX_WIFI_ROLE_AP,
	MAC_AX_WIFI_ROLE_VAP,
	MAC_AX_WIFI_ROLE_ADHOC,
	MAC_AX_WIFI_ROLE_ADHOC_MASTER,
	MAC_AX_WIFI_ROLE_MESH,
	MAC_AX_WIFI_ROLE_MONITOR,
	MAC_AX_WIFI_ROLE_P2P_DEVICE,
	MAC_AX_WIFI_ROLE_P2P_GC,
	MAC_AX_WIFI_ROLE_P2P_GO,
	MAC_AX_WIFI_ROLE_NAN,
	MAC_AX_WIFI_ROLE_MLME_MAX
};

/**
 * @enum mac_ax_opmode
 *
 * @brief mac_ax_opmode
 *
 * @var mac_ax_opmode::MAC_AX_ROLE_CONNECT
 * Please Place Description here.
 * @var mac_ax_opmode::MAC_AX_ROLE_DISCONN
 * Please Place Description here.
 */
enum mac_ax_opmode {
	MAC_AX_ROLE_CONNECT,
	MAC_AX_ROLE_DISCONN
};

/**
 * @enum mac_ax_upd_mode
 *
 * @brief mac_ax_upd_mode
 *
 * @var mac_ax_upd_mode::MAC_AX_ROLE_CREATE
 * Please Place Description here.
 * @var mac_ax_upd_mode::MAC_AX_ROLE_REMOVE
 * Please Place Description here.
 * @var mac_ax_upd_mode::MAC_AX_ROLE_TYPE_CHANGE
 * Please Place Description here.
 * @var mac_ax_upd_mode::MAC_AX_ROLE_INFO_CHANGE
 * Please Place Description here.
 * @var mac_ax_upd_mode::MAC_AX_ROLE_CON_DISCONN
 * Please Place Description here.
 * @var mac_ax_upd_mode::MAC_AX_ROLE_BAND_SW
 * Please Place Description here.
 */
enum mac_ax_upd_mode {
	MAC_AX_ROLE_CREATE,
	MAC_AX_ROLE_REMOVE,
	MAC_AX_ROLE_TYPE_CHANGE,
	MAC_AX_ROLE_INFO_CHANGE,
	MAC_AX_ROLE_CON_DISCONN,
	MAC_AX_ROLE_BAND_SW,
	MAC_AX_ROLE_FW_RESTORE
};

/**
 * @enum mac_ax_host_rpr_mode
 *
 * @brief mac_ax_host_rpr_mode
 *
 * @var mac_ax_host_rpr_mode::MAC_AX_RPR_MODE_POH
 * Please Place Description here.
 * @var mac_ax_host_rpr_mode::MAC_AX_RPR_MODE_STF
 * Please Place Description here.
 */
enum mac_ax_host_rpr_mode {
	MAC_AX_RPR_MODE_POH = 0,
	MAC_AX_RPR_MODE_STF
};

/**
 * @enum h2c_buf_class
 *
 * @brief h2c_buf_class
 *
 * @var h2c_buf_class::H2CB_CLASS_CMD
 * Please Place Description here.
 * @var h2c_buf_class::H2CB_CLASS_DATA
 * Please Place Description here.
 * @var h2c_buf_class::H2CB_CLASS_LONG_DATA
 * Please Place Description here.
 * @var h2c_buf_class::H2CB_CLASS_LAST
 * Please Place Description here.
 * @var h2c_buf_class::H2CB_CLASS_MAX
 * Please Place Description here.
 * @var h2c_buf_class::H2CB_CLASS_INVALID
 * Please Place Description here.
 */
enum h2c_buf_class {
	H2CB_CLASS_CMD,		/* FW command */
	H2CB_CLASS_DATA,	/* FW command + data */
	H2CB_CLASS_LONG_DATA,	/* FW command + long data */

	/* keep last */
	H2CB_CLASS_LAST,
	H2CB_CLASS_MAX = H2CB_CLASS_LAST,
	H2CB_CLASS_INVALID = H2CB_CLASS_LAST,
};

/**
 * @struct mac_ax_role_opmode
 * @brief mac_ax_role_opmode
 *
 * @var mac_ax_role_opmode::ADD
 * Please Place Description here.
 * @var mac_ax_role_opmode::CHG
 * Please Place Description here.
 * @var mac_ax_role_opmode::RMV
 * Please Place Description here.
 */
enum mac_ax_role_opmode {
	ADD = 0,
	CHG,
	RMV
};

/**
 * @enum mac_ax_preld_macid
 *
 * @brief mac_ax_preld_macid
 *
 * @var mac_ax_preld_macid::MAC_AX_PRELD_MACID0
 * Please Place Description here.
 * @var mac_ax_preld_macid::MAC_AX_PRELD_MACID1
 * Please Place Description here.
 * @var mac_ax_preld_macid::MAC_AX_PRELD_MACID2
 * Please Place Description here.
 * @var mac_ax_preld_macid::MAC_AX_PRELD_MACID3
 * Please Place Description here.
 * @var mac_ax_preld_macid::MAC_AX_PRELD_MACID_LAST
 * Please Place Description here.
 * @var mac_ax_preld_macid::MAC_AX_PRELD_MACID_MAX
 * Please Place Description here.
 * @var mac_ax_preld_macid::MAC_AX_PRELD_MACID_INVALID
 * Please Place Description here.
 */
enum mac_ax_preld_macid {
	MAC_AX_PRELD_MACID0 = 0,
	MAC_AX_PRELD_MACID1,
	MAC_AX_PRELD_MACID2,
	MAC_AX_PRELD_MACID3,

	MAC_AX_PRELD_MACID_LAST,
	MAC_AX_PRELD_MACID_MAX = MAC_AX_PRELD_MACID_LAST,
	MAC_AX_PRELD_MACID_INVLAID = MAC_AX_PRELD_MACID_LAST,
};

/**
 * @enum mac_ax_dbcc_wmm
 *
 * @brief mac_ax_dbcc_wmm
 *
 * @var mac_ax_dbcc_wmm::MAC_AX_DBCC_WMM0
 * Please Place Description here.
 * @var mac_ax_dbcc_wmm::MAC_AX_DBCC_WMM1
 * Please Place Description here.
 * @var mac_ax_dbcc_wmm::MAC_AX_DBCC_WMM2
 * Please Place Description here.
 * @var mac_ax_dbcc_wmm::MAC_AX_DBCC_WMM3
 * Please Place Description here.
 * @var mac_ax_dbcc_wmm::MAC_AX_DBCC_WMM_LAST
 * Please Place Description here.
 * @var mac_ax_dbcc_wmm::MAC_AX_DBCC_WMM_MAX
 * Please Place Description here.
 * @var mac_ax_dbcc_wmm::MAC_AX_DBCC_WMM_INVALID
 * Please Place Description here.
 */
enum mac_ax_dbcc_wmm {
	MAC_AX_DBCC_WMM0 = 0,
	MAC_AX_DBCC_WMM1,
	MAC_AX_DBCC_WMM2,
	MAC_AX_DBCC_WMM3,

	MAC_AX_DBCC_WMM_LAST,
	MAC_AX_DBCC_WMM_MAX = MAC_AX_DBCC_WMM_LAST,
	MAC_AX_DBCC_WMM_INVALID = MAC_AX_DBCC_WMM_LAST,
};

/*--------------------Define FAST_CH_SW related enum-------------------------------------*/
/**
 * @struct mac_ax_fast_ch_sw_status_code
 * @brief mac_ax_fast_ch_sw_status_code
 *
 * @var mac_ax_fast_ch_sw_status_code::MAC_AX_FAST_CH_SW_STATUS_OK
 * FAST_CH_SW done without error
 * @var mac_ax_fast_ch_sw_status_code::MAC_AX_FAST_CH_SW_STATUS_PART_MALLOC_FAIL
 * fail when malloc pkt for part of stas
 * @var mac_ax_fast_ch_sw_status_code::MAC_AX_FAST_CH_SW_STATUS_ALL_MALLOC_FAIL
 * fail when malloc pkt for all of stas
 * @var mac_ax_fast_ch_sw_status_code::MAC_AX_FAST_CH_SW_STATUS_PART_READOFLD_FAIL
 * fail when reading offloaded pkt for part of stas
 * @var mac_ax_fast_ch_sw_status_code::MAC_AX_FAST_CH_SW_STATUS_ALL_READOFLD_FAIL
 * fail when reading offloaded pkt for all of stas
 * @var mac_ax_fast_ch_sw_status_code::MAC_AX_FAST_CH_SW_STATUS_PART_SENDPKT_FAIL
 * fail when sending pkt to part of stas
 * @var mac_ax_fast_ch_sw_status_code::MAC_AX_FAST_CH_SW_STATUS_ALL_SENDPKT_FAIL
 * fail when sending pkt to all of stas
 * @var mac_ax_fast_ch_sw_status_code::MAC_AX_FAST_CH_SW_STATUS_PART_NO_ACK
 * not receiving ack from part of stas
 * @var mac_ax_fast_ch_sw_status_code::MAC_AX_FAST_CH_SW_STATUS_ALL_NO_ACK
 * not receiving ack from all of stas
 * @var mac_ax_fast_ch_sw_status_code::MAC_AX_FAST_CH_SW_STATUS_SWITCH_CH_FAIL
 * AP fail to switch channel
 * @var mac_ax_fast_ch_sw_status_code::MAC_AX_FAST_CH_SW_STATUS_RF_PARAM_ERR
 * AP fail to restore RF params
 * @var mac_ax_fast_ch_sw_status_code::MAC_AX_FAST_CH_SW_STATUS_MAX
 * Please Place Description here.
 */
enum mac_ax_fast_ch_sw_status_code {
	MAC_AX_FAST_CH_SW_STATUS_OK = 0,
	MAC_AX_FAST_CH_SW_STATUS_PART_READCSA_FAIL = 3,
	MAC_AX_FAST_CH_SW_STATUS_ALL_READCSA_FAIL = 4,
	MAC_AX_FAST_CH_SW_STATUS_PART_ENQCSA_FAIL = 5,
	MAC_AX_FAST_CH_SW_STATUS_ALL_ENQCSA_FAIL = 6,
	MAC_AX_FAST_CH_SW_STATUS_PART_CSA_NOACK = 7,
	MAC_AX_FAST_CH_SW_STATUS_ALL_CSA_NOACK = 8,
	MAC_AX_FAST_CH_SW_STATUS_SWITCH_CH_FAIL = 9,
	MAC_AX_FAST_CH_SW_STATUS_BUSY = 10,
	MAC_AX_FAST_CH_SW_STATUS_MAX
};

enum mac_ax_fw_state {
	FS_SLEEP_IN       = 0x10,
	FS_ERRHDL_IN      = 0x11,
	FS_ASSERT_IN      = 0x12,
	FS_EXCEP_IN       = 0x13,
	FS_L2ERR_IN       = 0x14,
	FS_L2ERR_CPU_IN   = 0x15,//CPU address hole
	FS_L2ERR_HCI_IN   = 0x16,//HCI timeout
	FS_L2ERR_ELSE_IN  = 0x17,
	FS_WOW_FWDL_IN    = 0x18,

	FS_SLEEP_OUT      = 0x80,
	FS_ERRHDL_OUT     = 0x81,
	FS_ASSERT_OUT     = 0x82,
	//rsvd: 0xFFFFFF83
	FS_L2ERR_OUT      = 0x84,

	// Defeature
	FS_DEFEA_RXNSS    = 0xEA01,
	FS_DEFEA_BW5      = 0xEA02,
	FS_DEFEA_BW10     = 0xEA03,
	FS_DEFEA_BW20     = 0xEA04,
	FS_DEFEA_BW40     = 0xEA05,
	FS_DEFEA_BW80     = 0xEA06,
	FS_DEFEA_BW160    = 0xEA07,
	FS_DEFEA_TXNSS    = 0xEA08,
	FS_DEFEA_PTCL     = 0xEA09,
	FS_DEFEA_DBLABORT = 0xEA0A,
};

/**
 * @struct mac_ax_scanofld_notify_reason
 * @brief mac_ax_scanofld_notify_reason
 *
 * @var mac_ax_scanofld_notify_reason::MAC_AX_SCAN_DWELL_NOTIFY
 * channel dwell due to rcv bcn
 * @var mac_ax_scanofld_notify_reason::MAC_AX_SCAN_PRE_TX_NOTIFY
 * before tx pkt
 * @var mac_ax_scanofld_notify_reason::MAC_AX_SCAN_POST_TX_NOTIFY
 * after tx pkt
 * @var mac_ax_scanofld_notify_reason::MAC_AX_SCAN_ENTER_CH_NOTIFY
 * enter ch
 * @var mac_ax_scanofld_notify_reason::MAC_AX_SCAN_LEAVE_CH_NOTIFY
 * leace ch
 * @var mac_ax_scanofld_notify_reason::MAC_AX_SCAN_END_SCAN_NOTIFY
 * scan stop
 * @var mac_ax_scanofld_notify_reason::MAC_AX_SCAN_GET_RPT_NOTIFY
 * scan stop
 */
enum mac_ax_scanofld_notify_reason {
	MAC_AX_SCAN_DWELL_NOTIFY = 0,
	MAC_AX_SCAN_PRE_TX_NOTIFY = 1,
	MAC_AX_SCAN_POST_TX_NOTIFY = 2,
	MAC_AX_SCAN_ENTER_CH_NOTIFY = 3,
	MAC_AX_SCAN_LEAVE_CH_NOTIFY = 4,
	MAC_AX_SCAN_END_SCAN_NOTIFY = 5,
	MAC_AX_SCAN_GET_RPT_NOTIFY = 6,
	MAC_AX_SCAN_CHKPT_TIMER = 7,
};

/**
 * @struct mac_ax_scanofld_notify_status
 * @brief mac_ax_scanofld_notify_status
 *
 * @var mac_ax_scanofld_notify_status::MAC_AX_SCAN_STATUS_NOTIFY
 * just notify
 * @var mac_ax_scanofld_notify_status::MAC_AX_SCAN_STATUS_SUCCESS
 * success
 * @var mac_ax_scanofld_notify_status::MAC_AX_SCAN_STATUS_FAIL
 * fail
 * @var mac_ax_scanofld_notify_status::MAC_AX_SCAN_STATUS_MAX
 * max
 */
enum mac_ax_scanofld_notify_status {
	MAC_AX_SCAN_STATUS_NOTIFY = 0,
	MAC_AX_SCAN_STATUS_SUCCESS = 1,
	MAC_AX_SCAN_STATUS_FAIL = 2,
	MAC_AX_SCAN_STATUS_MAX
};

/**
 * @struct mac_ax_scan_ofld_op
 * @brief mac_ax_scan_ofld_op
 *
 * @var mac_ax_scan_ofld_op::MAC_AX_SCAN_OP_STOP
 * stop scan
 * @var mac_ax_scan_ofld_op::MAC_AX_SCAN_OP_START
 * start scan
 * @var mac_ax_scan_ofld_op::MAC_AX_SCAN_OP_SETPARM
 * set parameter only
 * @var mac_ax_scan_ofld_op::MAC_AX_SCAN_OP_GETRPT
 * get rpt during scanning
 * @var mac_ax_scan_ofld_op::MAC_AX_SCAN_OP_MAX
 * max
 */
enum mac_ax_scan_ofld_op {
	MAC_AX_SCAN_OP_STOP = 0,
	MAC_AX_SCAN_OP_START = 1,
	MAC_AX_SCAN_OP_SETPARM = 2,
	MAC_AX_SCAN_OP_GETRPT = 3,
	MAC_AX_SCAN_OP_MAX
};

/**
 * @struct mac_ax_scan_ofld_scantype
 * @brief mac_ax_scan_ofld_scantype
 *
 * @var mac_ax_scan_ofld_scantype::MAC_AX_SCAN_TYPE_SCANONCE
 * scan once
 * @var mac_ax_scan_ofld_scantype::MAC_AX_SCAN_TYPE_NORMAL
 * repeat. period=normal pd
 * @var mac_ax_scan_ofld_scantype::MAC_AX_SCAN_TYPE_NORMAL_SLOW
 * repeat. normal pd * normal cycle + slow pd
 * @var mac_ax_scan_ofld_scantype::MAC_AX_SCAN_TYPE_SEAMLESS
 * repeat seamless
 * @var mac_ax_scan_ofld_scantype::MAC_AX_SCAN_TYPE_MAX
 * max
 */
enum mac_ax_scan_ofld_scantype {
	MAC_AX_SCAN_TYPE_SCANONCE = 0,
	MAC_AX_SCAN_TYPE_NORMAL = 1,
	MAC_AX_SCAN_TYPE_NORMAL_SLOW = 2,
	MAC_AX_SCAN_TYPE_SEAMLESS = 3,
	MAC_AX_SCAN_TYPE_MAX
};

/**
 * @struct mac_ax_scanofld_start_mode
 * @brief mac_ax_scanofld_start_mode
 *
 * @var mac_ax_scanofld_start_mode::MAC_AX_SCAN_START_NOW
 * start now
 * @var mac_ax_scanofld_start_mode::MAC_AX_SCAN_START_TSF
 * start at assigned tsf
 * @var mac_ax_scanofld_start_mode::MAC_AX_SCAN_START_MAX
 * max
 */
enum mac_ax_scanofld_start_mode {
	MAC_AX_SCAN_START_NOW = 0,
	MAC_AX_SCAN_START_TSF = 1,
	MAC_AX_SCAN_START_MAX
};

/**
 * @struct mac_ax_scanofld_ctrl
 * @brief mac_ax_scanofld_ctrl
 *
 * @var mac_ax_scanofld_ctrl::MAC_AX_SCAN_START_NOW
 * start now
 * @var mac_ax_scanofld_ctrl::MAC_AX_SCAN_START_MAX
 * max
 */
enum mac_ax_scanofld_ctrl {
	MAC_AX_SCAN_CTRL_NEXT_CH = 0,
	MAC_AX_SCAN_CTRL_MAX
};

/**
 * @struct mac_ax_dev2hst_gpio
 * @brief mac_ax_dev2hst_gpio
 *
 * @var mac_ax_dev2hst_gpio::MAC_AX_DEV2HST_GPIO_OUTPUT
 * set gpio as data output
 * @var mac_ax_dev2hst_gpio::MAC_AX_DEV2HST_GPIO_INPUT
 * set gpio as data input
 * @var mac_ax_dev2hst_gpio::MAC_AX_DEV2HST_GPIO_MAX
 * max
 */
enum mac_ax_dev2hst_gpio {
	MAC_AX_DEV2HST_GPIO_OUTPUT = 0,
	MAC_AX_DEV2HST_GPIO_INPUT = 1,
	MAC_AX_DEV2HST_GPIO_MAX
};

/**
 * @struct mac_ax_dev2hst_active
 * @brief mac_ax_dev2hst_active
 *
 * @var mac_ax_dev2hst_active::MAC_AX_DEV2HST_LOW_ACTIVE
 * low (falling edge) as active
 * @var mac_ax_dev2hst_active::MAC_AX_DEV2HST_HIGH_ACTIVE
 * high (rising edge) as active
 * @var mac_ax_dev2hst_active::MAC_AX_DEV2HST_ACTIVE_MAX
 * max
 */
enum mac_ax_dev2hst_active {
	MAC_AX_DEV2HST_LOW_ACTIVE = 0,
	MAC_AX_DEV2HST_HIGH_ACTIVE = 1,
	MAC_AX_DEV2HST_ACTIVE_MAX
};

/**
 * @struct mac_ax_dev2hst_toggle_pulse
 * @brief mac_ax_dev2hst_toggle_pulse
 *
 * @var mac_ax_dev2hst_toggle_pulse::MAC_AX_DEV2HST_TOGGLE
 * toggle mode
 * @var mac_ax_dev2hst_toggle_pulse::MAC_AX_DEV2HST_PULSE
 * pulse mode
 * @var mac_ax_dev2hst_toggle_pulse::MAC_AX_DEV2HST_TOGGLE_PULSE_MAX
 * max
 */
enum mac_ax_dev2hst_toggle_pulse {
	MAC_AX_DEV2HST_TOGGLE = 0,
	MAC_AX_DEV2HST_PULSE = 1,
	MAC_AX_DEV2HST_TOGGLE_PULSE_MAX
};

/**
 * @struct mac_ax_drv_info_size
 * @brief mac_ax_drv_info_size
 * for halmac internal usage, records current drv info type
 *
 * @var mac_ax_drv_info_size::MAC_AX_DRV_INFO_NONE
 * there is no drv info
 * @var mac_ax_drv_info_size::MAC_AX_DRV_INFO_PHY_RPT
 * current drv info is for phy report (16 bytes)
 * @var mac_ax_drv_info_size::MAC_AX_DRV_INFO_BSSID
 * current drv info is for bssid (16 bytes)
 * @var mac_ax_drv_info_size::MAC_AX_DRV_INFO_MAC_HDR
 * current drv info is for mac header
 * (48 bytes, 8852C ACUT must use 80 bytes to prevent from sec access null)
 * @var mac_ax_drv_info_size::MAC_AX_DRV_INFO_PHY_RPT_BSSID
 * current drv info is for phy rpt & bssid
 * (48 bytes, 8852C ACUT must use 80 bytes to prevent from sec access null)
 * @var mac_ax_drv_info_size::MAC_AX_DRV_INFO_PHY_RPT_MAC_HDR
 * current drv info is for phy rpt & mac header
 * (48 bytes, 8852C ACUT must use 80 bytes to prevent from sec access null)
 * @var mac_ax_drv_info_size::MAC_AX_DRV_INFO_LAST
 * current drv info is invalid
 */
enum mac_ax_drv_info_size {
	MAC_AX_DRV_INFO_NONE,
	MAC_AX_DRV_INFO_PHY_RPT,
	MAC_AX_DRV_INFO_BSSID,
	MAC_AX_DRV_INFO_MAC_HDR,
	MAC_AX_DRV_INFO_PHY_RPT_BSSID,
	MAC_AX_DRV_INFO_PHY_RPT_MAC_HDR,

	MAC_AX_DRV_INFO_LAST,
	MAC_AX_DRV_INFO_MAX = MAC_AX_DRV_INFO_LAST,
	MAC_AX_DRV_INFO_INVALID = MAC_AX_DRV_INFO_LAST,
};

/**
 * @struct mac_ax_dev2hst_time_unit
 * @brief mac_ax_dev2hst_time_unit
 *
 * @var mac_ax_dev2hst_time_unit::MAC_AX_DEV2HST_32US
 * 32us
 * @var mac_ax_dev2hst_time_unit::MAC_AX_DEV2HST_4MS
 * 4ms
 * @var mac_ax_dev2hst_time_unit::MAC_AX_DEV2HST_TIME_UNIT_MAX
 * max
 */
enum mac_ax_dev2hst_time_unit {
	MAC_AX_DEV2HST_US = 0,
	MAC_AX_DEV2HST_MS = 1,
	MAC_AX_DEV2HST_TIME_UNIT_MAX
};

/**
 * @enum mac_ax_rx_hdr_trans_type
 *
 * @brief mac_ax_rx_hdr_trans_type
 *
 * @var mac_ax_rx_hdr_trans_type::MAC_AX_RX_HDR_TYPE_802P3_SNAP
 * Please Place Description here.
 * @var mac_ax_rx_hdr_trans_type::MAC_AX_RX_HDR_TYPE_ETH_II
 * Please Place Description here.
 * @var mac_ax_rx_hdr_trans_type::MAC_AX_RX_HDR_TYPE_MAX
 * Please Place Description here.
 */
enum mac_ax_rx_hdr_trans_type {
	MAC_AX_RX_HDR_TYPE_802P3_SNAP = 0,
	MAC_AX_RX_HDR_TYPE_ETH_II = 1,
	MAC_AX_RX_HDR_TYPE_MAX = 2,
};

/**
 * @enum mac_ax_rx_driv_info_hdr_type
 *
 * @brief mac_ax_rx_driv_info_hdr_type
 *
 * @var mac_ax_rx_driv_info_hdr_type::MAC_AX_RX_DRV_INFO_HDR_BSSID
 * Please Place Description here.
 * @var mac_ax_rx_driv_info_hdr_type::MAC_AX_RX_DRV_INFO_HDR_FULLY_HDR
 * Please Place Description here.
 * @var mac_ax_rx_driv_info_hdr_type::MAC_AX_RX_DRV_INFO_HDR_MAX
 * Please Place Description here.
 */
enum mac_ax_rx_driv_info_hdr_type {
	MAC_AX_RX_DRIV_INFO_NONE = 0,
	MAC_AX_RX_DRIV_INFO_HDR_BSSID = 1,
	MAC_AX_RX_DRIV_INFO_HDR_FULLY_HDR = 2,
	MAC_AX_RX_DRIV_INFO_HDR_MAX = 3,
};

/*--------------------Define Power Saving related enum-------------------------------------*/
/**
 * @enum mac_ax_listern_bcn_mode
 *
 * @brief mac_ax_listern_bcn_mode
 *
 * @var mac_ax_listern_bcn_mode::MAC_AX_RLBM_MIN
 * Please Place Description here.
 * @var mac_ax_listern_bcn_mode::MAC_AX_RLBM_MAX
 * Please Place Description here.
 * @var mac_ax_listern_bcn_mode::MAC_AX_RLBM_USERDEFINE
 * Please Place Description here.
 */
enum mac_ax_listern_bcn_mode {
	MAC_AX_RLBM_MIN         = 0,
	MAC_AX_RLBM_MAX         = 1,
	MAC_AX_RLBM_USERDEFINE  = 2,
};

/**
 * @enum mac_ax_smart_ps_mode
 *
 * @brief mac_ax_smart_ps_mode
 *
 * @var mac_ax_smart_ps_mode::MAC_AX_SMART_PS_MODE_LEGACY_PWR1
 * Please Place Description here.
 * @var mac_ax_smart_ps_mode::MAC_AX_SMART_PS_MODE_TRX_PWR0
 * Please Place Description here.
 */
enum mac_ax_smart_ps_mode {
	MAC_AX_SMART_PS_MODE_LEGACY_PWR1 = 0,
	MAC_AX_SMART_PS_MODE_TRX_PWR0 = 1,
};

/**
 * @enum mac_ax_ps_advance_parm_op
 *
 * @brief mac_ax_ps_advance_parm_op
 *
 * @var mac_ax_ps_advance_parm_op::PS_ADVANCE_PARM_OP_NO_CHANGE
 * Please Place Description here.
 * @var mac_ax_ps_advance_parm_op::PS_ADVANCE_PARM_OP_SET
 * Please Place Description here.
 * @var mac_ax_ps_advance_parm_op::PS_ADVANCE_PARM_OP_DEFAULT
 * Please Place Description here.
 */
enum mac_ax_ps_advance_parm_op{
	PS_ADVANCE_PARM_OP_NO_CHANGE = 0,
	PS_ADVANCE_PARM_OP_SET = 1,
	PS_ADVANCE_PARM_OP_DEFAULT = 2,
};

/**
 * @enum mac_ax_dma_ch
 *
 * @brief mac_ax_dma_ch
 *
 * @var mac_ax_dma_ch::MAC_AX_DMA_ACH0
 * Please Place Description here.
 * @var mac_ax_dma_ch::MAC_AX_DMA_ACH1
 * Please Place Description here.
 * @var mac_ax_dma_ch::MAC_AX_DMA_ACH2
 * Please Place Description here.
 * @var mac_ax_dma_ch::MAC_AX_DMA_ACH3
 * Please Place Description here.
 * @var mac_ax_dma_ch::MAC_AX_DMA_ACH4
 * Please Place Description here.
 * @var mac_ax_dma_ch::MAC_AX_DMA_ACH5
 * Please Place Description here.
 * @var mac_ax_dma_ch::MAC_AX_DMA_ACH6
 * Please Place Description here.
 * @var mac_ax_dma_ch::MAC_AX_DMA_ACH7
 * Please Place Description here.
 * @var mac_ax_dma_ch::MAC_AX_DMA_B0MG
 * Please Place Description here.
 * @var mac_ax_dma_ch::MAC_AX_DMA_B0HI
 * Please Place Description here.
 * @var mac_ax_dma_ch::MAC_AX_DMA_B1MG
 * Please Place Description here.
 * @var mac_ax_dma_ch::MAC_AX_DMA_B1HI
 * Please Place Description here.
 * @var mac_ax_dma_ch::MAC_AX_DMA_H2C
 * Please Place Description here.
 * @var mac_ax_dma_ch::MAC_AX_DMA_CH_NUM
 * Please Place Description here.
 */
enum mac_ax_dma_ch {
	MAC_AX_DMA_ACH0 = 0,
	MAC_AX_DMA_ACH1,
	MAC_AX_DMA_ACH2,
	MAC_AX_DMA_ACH3,
	MAC_AX_DMA_ACH4,
	MAC_AX_DMA_ACH5,
	MAC_AX_DMA_ACH6,
	MAC_AX_DMA_ACH7,
	MAC_AX_DMA_B0MG,
	MAC_AX_DMA_B0HI,
	MAC_AX_DMA_B1MG,
	MAC_AX_DMA_B1HI,
	MAC_AX_DMA_H2C,
	MAC_AX_DMA_CH_NUM
};

/**
 * @enum mac_ax_bcn_fltr_notify
 *
 * @brief mac_ax_bcn_fltr_notify
 *
 * @var mac_ax_bcn_fltr_notify::BCNFLTR_NOTI_BCN_LOSS
 * Please Place Description here.
 * @var mac_ax_bcn_fltr_notify::BCNFLTR_NOTI_RSSI
 * Please Place Description here.
 * @var mac_ax_bcn_fltr_notify::BCNFLTR_NOTI_DENY_SCAN
 * Please Place Description here.
 */
enum mac_ax_bcn_fltr_notify {
	BCNFLTR_NOTI_BCN_LOSS = 0,
	BCNFLTR_NOTI_RSSI,
	BCNFLTR_NOTI_DENY_SCAN,
	BCNFLTR_NOTI_MAX,
};

/**
 * @enum mac_ax_bcn_fltr_rssi_evt
 *
 * @brief mac_ax_bcn_fltr_rssi_evt
 *
 * @var mac_ax_bcn_fltr_rssi_evt::BCNFLTR_RSSI_EVT_NOT_CHANGED
 * Please Place Description here.
 * @var mac_ax_bcn_fltr_rssi_evt::BCNFLTR_RSSI_EVT_HIGH
 * Please Place Description here.
 * @var mac_ax_bcn_fltr_rssi_evt::BCNFLTR_RSSI_EVT_LOW
 * Please Place Description here.
 */
enum mac_ax_bcn_fltr_rssi_evt {
	BCNFLTR_RSSI_EVT_NOT_CHANGED = 0,
	BCNFLTR_RSSI_EVT_HIGH,
	BCNFLTR_RSSI_EVT_LOW,
	BCNFLTR_RSSI_EVT_MAX,
};

/**
 * @enum mac_ax_bcn_fltr_tp_thld
 *
 * @brief mac_ax_bcn_fltr_tp_thld
 *
 * @var mac_ax_bcn_fltr_tp_thld::BCNFLTR_TP_THLD_1M
 * Please Place Description here.
 * @var mac_ax_bcn_fltr_tp_thld::BCNFLTR_TP_THLD_3M
 * Please Place Description here.
 * @var mac_ax_bcn_fltr_tp_thld::BCNFLTR_TP_THLD_10M
 * Please Place Description here.
 * @var mac_ax_bcn_fltr_tp_thld::BCNFLTR_TP_THLD_50M
 * Please Place Description here.
 */
enum mac_ax_bcn_fltr_tp_thld {
	BCNFLTR_TP_THLD_1M = 0,
	BCNFLTR_TP_THLD_3M,
	BCNFLTR_TP_THLD_10M,
	BCNFLTR_TP_THLD_50M,
	BCNFLTR_TP_THLD_MAX
};

/**
 * @enum cmd_ofld_ver
 *
 * @brief cmd_ofld_ver
 *
 * @var cmd_ofld_ver::MAC_AX_CMD_OFLD
 * Please Place Description here.
 * @var cmd_ofld_ver::MAC_AX_CMD_OFLD_V1
 * Please Place Description here.
 */
enum cmd_ofld_ver {
	MAC_AX_CMD_OFLD,
	MAC_AX_CMD_OFLD_V1
};

/**
 * @enum mac_req_pwr_st
 *
 * @brief mac_req_pwr_st
 *
 * @var mac_req_pwr_st::REQ_PWR_ST_OPEN_RF
 * Please Place Description here.
 * @var mac_req_pwr_st::REQ_PWR_ST_CLOSE_RF
 * Please Place Description here.
 */
enum mac_req_pwr_st {
	REQ_PWR_ST_OPEN_RF	= 0,
	REQ_PWR_ST_CLOSE_RF	= 1,
	REQ_PWR_ST_ADC_OFF	= 2,
	REQ_PWR_ST_BB_OFF	= 3,
	REQ_PWR_ST_CPU_OFF	= 4,
	REQ_PWR_ST_MAC_OFF	= 5,
	REQ_PWR_ST_PLL_OFF	= 6,
	REQ_PWR_ST_SWRD_OFF	= 7,
	REQ_PWR_ST_XTAL_OFF	= 8,
	REQ_PWR_ST_ADIE_OFF	= 9,
	REQ_PWR_ST_BYPASS_DATA_ON	= 87,
};

/**
 * @enum mac_req_bcn_to_lvl
 *
 * @brief mac_req_bcn_to_lvl
 *
 * @var mac_req_bcn_to_lvl::REQ_BCN_TO_LVL_0
 * Please Place Description here.
 * @var mac_req_bcn_to_lvl::REQ_BCN_TO_LVL_1
 * Please Place Description here.
 * @var mac_req_bcn_to_lvl::REQ_BCN_TO_LVL_2
 * Please Place Description here.
 * @var mac_req_bcn_to_lvl::REQ_BCN_TO_LVL_3
 * Please Place Description here.
 * @var mac_req_bcn_to_lvl::REQ_BCN_TO_LVL_4
 * Please Place Description here.
 * @var mac_req_bcn_to_lvl::REQ_BCN_TO_LVL_MAX
 * Please Place Description here.
 */
enum mac_req_bcn_to_lvl {
	REQ_BCN_TO_LVL_0,
	REQ_BCN_TO_LVL_1,
	REQ_BCN_TO_LVL_2,
	REQ_BCN_TO_LVL_3,
	REQ_BCN_TO_LVL_4,
	REQ_BCN_TO_LVL_MAX,
};

/**
 * @enum mac_req_ps_lvl
 *
 * @brief mac_req_ps_lvl
 *
 * @var mac_req_ps_lvl::REQ_PS_LVL_0
 * Please Place Description here.
 * @var mac_req_ps_lvl::REQ_PS_LVL_1
 * Please Place Description here.
 * @var mac_req_ps_lvl::REQ_PS_LVL_2
 * Please Place Description here.
 * @var mac_req_ps_lvl::REQ_PS_LVL_MAX
 * Please Place Description here.
 */
enum mac_req_ps_lvl {
	REQ_PS_LVL_0,
	REQ_PS_LVL_1,
	REQ_PS_LVL_2,
	REQ_PS_LVL_MAX,
};

/**
 * @enum mac_req_xtal_option
 *
 * @brief mac_req_xtal_option
 *
 * @var mac_req_xtal_option::REQ_XTAL_NORMAL_MODE
 * Please Place Description here.
 * @var mac_req_xtal_option::REQ_XTAL_LOW_PWR_MODE
 */
enum mac_req_xtal_option {
	REQ_XTAL_NORMAL_MODE	= 0,
	REQ_XTAL_LOW_PWR_MODE	= 1,
};

/**
 * @enum mac_req_trx_lvl
 *
 * @brief mac_req_trx_lvl
 *
 * @var mac_req_trx_lvl::REQ_TRX_LVL_0
 * Please Place Description here.
 * @var mac_req_trx_lvl::REQ_TRX_LVL_1
 * Please Place Description here.
 * @var mac_req_trx_lvl::REQ_PS_LVL_MAX
 * Please Place Description here.
 */
enum mac_req_trx_lvl {
	REQ_TRX_LVL_0,
	REQ_TRX_LVL_1,
	REQ_TRX_LVL_MAX,
};

/*--------------------Define Struct-------------------------------------*/

/**
 * @struct mac_ax_sch_tx_en
 * @brief mac_ax_sch_tx_en
 *
 * @var mac_ax_sch_tx_en::be0
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::bk0
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::vi0
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::vo0
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::be1
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::bk1
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::vi1
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::vo1
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::mg0
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::mg1
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::mg2
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::hi
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::bcn
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::ul
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::twt0
 * Please Place Description here.
 * @var mac_ax_sch_tx_en::twt1
 * Please Place Description here.
 */
struct mac_ax_sch_tx_en {
	u8 be0:1;
	u8 bk0:1;
	u8 vi0:1;
	u8 vo0:1;
	u8 be1:1;
	u8 bk1:1;
	u8 vi1:1;
	u8 vo1:1;
	u8 mg0:1;
	u8 mg1:1;
	u8 mg2:1;
	u8 hi:1;
	u8 bcn:1;
	u8 ul:1;
	u8 twt0:1;
	u8 twt1:1;
	u8 twt2:1;
	u8 twt3:1;
};

/**
 * @var mac_ax_drv_wdt_ctrl::autok_wdt_ctrl
 * Please Place Description here.
 * @var mac_ax_drv_wdt_ctrl::tp_wdt_ctrl
 * Please Place Description here.
 */
struct mac_ax_drv_wdt_ctrl {
	enum mac_ax_pcie_func_ctrl autok_wdt_ctrl;
	enum mac_ax_pcie_func_ctrl tp_wdt_ctrl;
};

/**
 * @var mac_ax_processor_id::proc_id_h
 * Please Place Description here.
 * @var mac_ax_processor_id::proc_id_l
 * Please Place Description here.
 */
struct mac_ax_processor_id {
	u32 proc_id_h;
	u32 proc_id_l;
};

/**
 * @var mac_ax_cust_proc_id::proc_id
 * Please Place Description here.
 * @var mac_ax_cust_proc_id::customer_id
 * Please Place Description here.
 */
struct mac_ax_cust_proc_id {
	struct mac_ax_processor_id proc_id;
	u8 base_board_id[BASE_BOARD_ID_LEN];
	u32 customer_id;
};

/**
 * @var mac_ax_tp_ctrl::tx_tp
 * Please Place Description here.
 * @var mac_ax_tp_ctrl::rx_tp
 * Please Place Description here.
 */
struct mac_ax_tp_param {
	u16 tx_tp;
	u16 rx_tp;
};

/**
 * @var mac_ax_wdt_param::tp
 * Please Place Description here.
 */
struct mac_ax_wdt_param {
	struct mac_ax_drv_wdt_ctrl drv_ctrl;
	struct mac_ax_tp_param tp;
};

/**
 * @var mac_ax_adapter_info::cust_proc_id
 * Please Place Description here.
 */
struct mac_ax_adapter_info {
	struct mac_ax_cust_proc_id cust_proc_id;
};

/**
 * @struct mac_debug_log
 * @brief mac_debug_log
 *
 * @var mac_debug_log::dbgprt_dump
 * Please Place Description here.
 * @var mac_debug_log::dbgprt_dump
 * Please Place Description here.
 * @var mac_debug_log::mac_reg_dump
 * Please Place Description here.
 */
struct mac_debug_log_lvl {
	u8 dbgprt_dump:1;
	u8 share_mem_dump:1;
	u8 cmac_dbg_dump:1;
	u8 dmac_dbg_dump:1;
	u8 txflow_dbg_dump:1;
	u8 mac_reg_dump:1;
	u8 rsvd:2;
};

/**
 * @struct mac_ax_hw_info
 * @brief mac_ax_hw_info
 *
 * @var mac_ax_hw_info::done
 * Please Place Description here.
 * @var mac_ax_hw_info::chip_id
 * Please Place Description here.
 * @var mac_ax_hw_info::cv
 * Please Place Description here.
 * @var mac_ax_hw_info::intf
 * Please Place Description here.
 * @var mac_ax_hw_info::tx_ch_num
 * Please Place Description here.
 * @var mac_ax_hw_info::tx_data_ch_num
 * Please Place Description here.
 * @var mac_ax_hw_info::wd_body_len
 * Please Place Description here.
 * @var mac_ax_hw_info::wd_info_len
 * Please Place Description here.
 * @var mac_ax_hw_info::pwr_on_seq
 * Please Place Description here.
 * @var mac_ax_hw_info::pwr_off_seq
 * Please Place Description here.
 * @var mac_ax_hw_info::pwr_seq_ver
 * Please Place Description here.
 * @var mac_ax_hw_info::fifo_size
 * Please Place Description here.
 * @var mac_ax_hw_info::macid_num
 * Please Place Description here.
 * @var mac_ax_hw_info::bssid_num
 * Please Place Description here.
 * @var mac_ax_hw_info::wl_efuse_size
 * Please Place Description here.
 * @var mac_ax_hw_info::efuse_size
 * Please Place Description here.
 * @var mac_ax_hw_info::log_efuse_size
 * Please Place Description here.
 * @var mac_ax_hw_info::limit_efuse_size_pcie
 * Please Place Description here.
 * @var mac_ax_hw_info::limit_efuse_size_usb
 * Please Place Description here.
 * @var mac_ax_hw_info::limit_efuse_size_sdio
 * Please Place Description here.
 * @var mac_ax_hw_info::bt_efuse_size
 * Please Place Description here.
 * @var mac_ax_hw_info::bt_log_efuse_size
 * Please Place Description here.
 * @var mac_ax_hw_info::hidden_efuse_size
 * Please Place Description here.
 * @var mac_ax_hw_info::sec_ctrl_efuse_size
 * Please Place Description here.
 * @var mac_ax_hw_info::sec_data_efuse_size
 * Please Place Description here.
 * @var mac_ax_hw_info::sec_cam_table
 * Please Place Description here.
 * @var mac_ax_hw_info::ple_rsvd_space
 * Please Place Description here.
 * @var mac_ax_hw_info::payload_desc_size
 * Please Place Description here.
 * @var mac_ax_hw_info::wd_checksum_en
 * Please Place Description here.
 * @var mac_ax_hw_info::sw_amsdu_max_size
 * Please Place Description here.
 * @var mac_ax_hw_info::core_swr_volt
 * Please Place Description here.
 * @var mac_ax_hw_info::core_swr_volt_sel
 * Please Place Description here.
 * @var mac_ax_hw_info::cmac0_drv_info
 * Please Place Description here.
 * @var mac_ax_hw_info::cmac1_drv_info
 * Please Place Description here.
 */
struct mac_ax_hw_info {
	u8 done;
	u8 chip_id;
	u8 cv;
	u8 acv;
	enum mac_ax_intf intf;
	u8 tx_ch_num;
	u8 tx_data_ch_num;
	u8 wd_body_len;
	u8 wd_info_len;
	struct mac_pwr_cfg **pwr_on_seq;
	struct mac_pwr_cfg **pwr_off_seq;
	u8 pwr_seq_ver;
	u32 fifo_size;
	u16 macid_num;
	u8 port_num;
	u8 mbssid_num;
	u8 bssid_num;
	u32 wl_efuse_size;
	u32 efuse_size;
	u32 log_efuse_size;
	u32 limit_efuse_size_pcie;
	u32 limit_efuse_size_usb;
	u32 limit_efuse_size_sdio;
	u32 bt_efuse_size;
	u32 bt_log_efuse_size;
	u8 hidden_efuse_rf_size;
	u8 hidden_efuse_mac_size;
	u32 sec_ctrl_efuse_size;
	u32 sec_data_efuse_size;
	struct sec_cam_table_t *sec_cam_table;
	struct dctl_sec_info_t *dctl_sec_info;
	u8 ple_rsvd_space;
	u8 payload_desc_size;
	u8 efuse_version_size;
	u32 dav_full_efuse_size;
	u32 dav_efuse_size;
	u32 dav_hidden_efuse_size;
	u32 dav_log_efuse_size;
	u32 wl_efuse_start_addr;
	u32 dav_efuse_start_addr;
	u32 bt_efuse_start_addr;
	u8 wd_checksum_en;
	u32 sw_amsdu_max_size;
	u32 ind_aces_cnt;
	u32 dbg_port_cnt;
	u8 core_swr_volt;
	u8 is_sec_ic;
	u8 sta_empty_flg;
	struct mac_ax_adapter_info adpt_info;
	enum mac_ax_core_swr_volt core_swr_volt_sel;
	enum mac_ax_drv_info_size cmac0_drv_info;
	enum mac_ax_drv_info_size cmac1_drv_info;
	mac_ax_mutex ind_access_lock;
	mac_ax_mutex lte_rlock;
	mac_ax_mutex lte_wlock;
	mac_ax_mutex dbg_port_lock;
	mac_ax_mutex err_set_lock;
	mac_ax_mutex err_get_lock;
	mac_ax_mutex dbi_lock;
	mac_ax_mutex mdio_lock;
};

/**
 * @struct mac_ax_fw_info
 * @brief mac_ax_fw_info
 *
 * @var mac_ax_fw_info::major_ver
 * Please Place Description here.
 * @var mac_ax_fw_info::minor_ver
 * Please Place Description here.
 * @var mac_ax_fw_info::sub_ver
 * Please Place Description here.
 * @var mac_ax_fw_info::sub_idx
 * Please Place Description here.
 * @var mac_ax_fw_info::build_year
 * Please Place Description here.
 * @var mac_ax_fw_info::build_mon
 * Please Place Description here.
 * @var mac_ax_fw_info::build_date
 * Please Place Description here.
 * @var mac_ax_fw_info::build_hour
 * Please Place Description here.
 * @var mac_ax_fw_info::build_min
 * Please Place Description here.
 * @var mac_ax_fw_info::h2c_seq
 * Please Place Description here.
 * @var mac_ax_fw_info::rec_seq
 * Please Place Description here.
 * @var mac_ax_fw_info::seq_lock
 * Please Place Description here.
 */
struct mac_ax_fw_info {
	u8 major_ver;
	u8 minor_ver;
	u8 sub_ver;
	u8 sub_idx;
	u16 build_year;
	u16 build_mon;
	u16 build_date;
	u16 build_hour;
	u16 build_min;
	u8 h2c_seq;
	u8 rec_seq;
	mac_ax_mutex seq_lock;
	mac_ax_mutex msg_reg;
	u8 cap_buff[MAC_WLANFW_CAP_MAX_SIZE];
	u32 cap_size;
};

/**
 * @struct mac_ax_ser_info
 * @brief mac_ax_ser_info
 *
 * @var mac_ax_ser_info::mac_debug_log_lvl
 * for debug log level component adjust.
 */
struct mac_ax_ser_info {
	struct mac_debug_log_lvl dbg_lvl;
};

/**
 * @struct mac_ax_h2c_agg_node
 * @brief mac_ax_h2c_agg_node
 *
 * @var mac_ax_h2c_agg_node::next
 * Please Place Description here.
 * @var mac_ax_h2c_agg_node::h2c_pkt
 * Please Place Description here.
 */
struct mac_ax_h2c_agg_node {
	struct mac_ax_h2c_agg_node *next;
	u8 *h2c_pkt;
};

/**
 * @struct mac_ax_h2c_agg_info
 * @brief mac_ax_h2c_agg_info
 *
 * @var mac_ax_h2c_agg_info::h2c_agg_en
 * Please Place Description here.
 * @var mac_ax_h2c_agg_info::h2c_agg_pkt_num
 * Please Place Description here.
 * @var mac_ax_h2c_agg_info::h2c_agg_queue_head
 * Please Place Description here.
 * @var mac_ax_h2c_agg_info::h2c_agg_queue_last
 * Please Place Description here.
 * @var mac_ax_h2c_agg_info::h2c_agg_lock
 * Please Place Description here.
 */
struct mac_ax_h2c_agg_info {
	u8 h2c_agg_en;
	u32 h2c_agg_pkt_num;
	struct mac_ax_h2c_agg_node *h2c_agg_queue_head;
	struct mac_ax_h2c_agg_node *h2c_agg_queue_last;
	mac_ax_mutex h2c_agg_lock;
};

/**
 * @struct mac_ax_mac_pwr_info
 * @brief mac_ax_mac_pwr_info
 *
 * @var mac_ax_mac_pwr_info::pwr_seq_proc
 * Please Place Description here.
 * @var mac_ax_mac_pwr_info::pwr_in_lps
 * Please Place Description here.
 * @var mac_ax_mac_pwr_info::intf_pwr_switch
 * Please Place Description here.
 */
struct mac_ax_mac_pwr_info {
	u8 pwr_seq_proc;
	u8 pwr_in_lps;
	u32 (*intf_pwr_switch)(void *vadapter,
			       u8 pre_switch, u8 on);
};

/**
 * @struct mac_ax_ft_status
 * @brief mac_ax_ft_status
 *
 * @var mac_ax_ft_status::mac_ft
 * Please Place Description here.
 * @var mac_ax_ft_status::status
 * Please Place Description here.
 * @var mac_ax_ft_status::buf
 * Please Place Description here.
 * @var mac_ax_ft_status::size
 * Please Place Description here.
 */
struct mac_ax_ft_status {
	enum mac_ax_feature mac_ft;
	enum mac_ax_status status;
	u8 *buf;
	u32 size;
};

/**
 * @struct mac_ax_dle_info
 * @brief mac_ax_dle_info
 *
 * @var mac_ax_dle_info::qta_mode
 * Please Place Description here.
 * @var mac_ax_dle_info::wde_pg_size
 * Please Place Description here.
 * @var mac_ax_dle_info::ple_pg_size
 * Please Place Description here.
 * @var mac_ax_dle_info::c0_rx_qta
 * Please Place Description here.
 * @var mac_ax_dle_info::c1_rx_qta
 * Please Place Description here.
 * @var mac_ax_dle_info::hif_min
 * Please Place Description here.
 */
struct mac_ax_dle_info {
	enum mac_ax_qta_mode qta_mode;
	u16 wde_pg_size;
	u16 ple_pg_size;
	u16 c0_rx_qta;
	u16 c1_rx_qta;
	u16 c0_ori_max;
	u16 c1_ori_max;
	u16 c0_tx_min;
	u16 c0_tx_max;
	u16 c1_tx_min;
	u16 c1_tx_max;
	u16 hif_min;
};

/**
 * @struct mac_ax_gpio_info
 * @brief mac_ax_gpio_info
 *
 * @var mac_ax_gpio_info::sw_io_0
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_1
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_2
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_3
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_4
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_5
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_6
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_7
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_8
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_9
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_10
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_11
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_12
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_13
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_14
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_15
 * Please Place Description here.
 * @var mac_ax_gpio_info::uart_tx_gpio5
 * Please Place Description here.
 * @var mac_ax_gpio_info::uart_tx_gpio7
 * Please Place Description here.
 * @var mac_ax_gpio_info::uart_tx_gpio8
 * Please Place Description here.
 * @var mac_ax_gpio_info::uart_rx_gpio6
 * Please Place Description here.
 * @var mac_ax_gpio_info::uart_rx_gpio14
 * Please Place Description here.
 * @var mac_ax_gpio_info::status
 * Please Place Description here.
 * @var mac_ax_gpio_info::sw_io_output
 * Please Place Description here.
 */
struct mac_ax_gpio_info {
#define MAC_AX_GPIO_NUM 19
	/* byte0 */
	u8 sw_io_0:1;
	u8 sw_io_1:1;
	u8 sw_io_2:1;
	u8 sw_io_3:1;
	u8 sw_io_4:1;
	u8 sw_io_5:1;
	u8 sw_io_6:1;
	u8 sw_io_7:1;
	/* byte1 */
	u8 sw_io_8:1;
	u8 sw_io_9:1;
	u8 sw_io_10:1;
	u8 sw_io_11:1;
	u8 sw_io_12:1;
	u8 sw_io_13:1;
	u8 sw_io_14:1;
	u8 sw_io_15:1;
	/* byte2 */
	u8 uart_tx_gpio5:1;
	u8 uart_tx_gpio7:1;
	u8 uart_tx_gpio8:1;
	u8 uart_rx_gpio6:1;
	u8 uart_rx_gpio14:1;
	u8 uart_tx_gpio;
	u8 uart_rx_gpio;
	enum rtw_mac_gfunc status[MAC_AX_GPIO_NUM];
#define MAC_AX_SW_IO_OUT_PP 0
#define MAC_AX_SW_IO_OUT_OD 1
	u8 sw_io_output[MAC_AX_GPIO_NUM];
};

/**
 * @struct mac_ax_trx_info
 * @brief mac_ax_trx_info
 *
 * @var mac_ax_trx_info::trx_mode
 * Please Place Description here.
 * @var mac_ax_trx_info::qta_mode
 * Please Place Description here.
 * @var mac_ax_trx_info::rpr_cfg
 * Please Place Description here.
 */
struct mac_ax_trx_info {
	enum mac_ax_trx_mode trx_mode;
	enum mac_ax_qta_mode qta_mode;
	struct mac_ax_host_rpr_cfg *rpr_cfg;
	enum rtw_mac_env_mode env_mode;
};

/**
 * @struct mac_ax_fwdl_info
 * @brief mac_ax_fwdl_info
 *
 * @var mac_ax_fwdl_info::fw_en
 * Please Place Description here.
 * @var mac_ax_fwdl_info::dlrom_en
 * Please Place Description here.
 * @var mac_ax_fwdl_info::dlram_en
 * Please Place Description here.
 * @var mac_ax_fwdl_info::fw_from_hdr
 * Please Place Description here.
 * @var mac_ax_fwdl_info::fw_cat
 * Please Place Description here.
 * @var mac_ax_fwdl_info::rom_buff
 * Please Place Description here.
 * @var mac_ax_fwdl_info::rom_size
 * Please Place Description here.
 * @var mac_ax_fwdl_info::ram_buff
 * Please Place Description here.
 * @var mac_ax_fwdl_info::ram_size
 * Please Place Description here.
 */
struct mac_ax_fwdl_info {
	u8 fw_en;
	u8 dlrom_en;
	u8 dlram_en;
	u8 fw_from_hdr;
	enum rtw_fw_type fw_cat;
	u8 *rom_buff;
	u32 rom_size;
	u8 *ram_buff;
	u32 ram_size;
};

/**
 * @struct mac_ax_txdma_ch_map
 * @brief mac_ax_txdma_ch_map
 *
 * @var mac_ax_txdma_ch_map::ch0
 * Please Place Description here.
 * @var mac_ax_txdma_ch_map::ch1
 * Please Place Description here.
 * @var mac_ax_txdma_ch_map::ch2
 * Please Place Description here.
 * @var mac_ax_txdma_ch_map::ch3
 * Please Place Description here.
 * @var mac_ax_txdma_ch_map::ch4
 * Please Place Description here.
 * @var mac_ax_txdma_ch_map::ch5
 * Please Place Description here.
 * @var mac_ax_txdma_ch_map::ch6
 * Please Place Description here.
 * @var mac_ax_txdma_ch_map::ch7
 * Please Place Description here.
 * @var mac_ax_txdma_ch_map::ch8
 * Please Place Description here.
 * @var mac_ax_txdma_ch_map::ch9
 * Please Place Description here.
 * @var mac_ax_txdma_ch_map::ch10
 * Please Place Description here.
 * @var mac_ax_txdma_ch_map::ch11
 * Please Place Description here.
 * @var mac_ax_txdma_ch_map::ch12
 * Please Place Description here.
 */
struct mac_ax_txdma_ch_map {
	enum mac_ax_pcie_func_ctrl ch0;
	enum mac_ax_pcie_func_ctrl ch1;
	enum mac_ax_pcie_func_ctrl ch2;
	enum mac_ax_pcie_func_ctrl ch3;
	enum mac_ax_pcie_func_ctrl ch4;
	enum mac_ax_pcie_func_ctrl ch5;
	enum mac_ax_pcie_func_ctrl ch6;
	enum mac_ax_pcie_func_ctrl ch7;
	enum mac_ax_pcie_func_ctrl ch8;
	enum mac_ax_pcie_func_ctrl ch9;
	enum mac_ax_pcie_func_ctrl ch10;
	enum mac_ax_pcie_func_ctrl ch11;
	enum mac_ax_pcie_func_ctrl ch12;
};

/**
 * @struct mac_ax_rxdma_ch_map
 * @brief mac_ax_rxdma_ch_map
 *
 * @var mac_ax_rxdma_ch_map::rxq
 * Please Place Description here.
 * @var mac_ax_rxdma_ch_map::rpq
 * Please Place Description here.
 */
struct mac_ax_rxdma_ch_map {
	enum mac_ax_pcie_func_ctrl rxq;
	enum mac_ax_pcie_func_ctrl rpq;
};

/**
 * @struct mac_ax_intf_info
 * @brief mac_ax_intf_info
 *
 * @var mac_ax_intf_info::txbd_trunc_mode
 * Please Place Description here.
 * @var mac_ax_intf_info::rxbd_trunc_mode
 * Please Place Description here.
 * @var mac_ax_intf_info::rxbd_mode
 * Please Place Description here.
 * @var mac_ax_intf_info::tag_mode
 * Please Place Description here.
 * @var mac_ax_intf_info::tx_burst
 * Please Place Description here.
 * @var mac_ax_intf_info::rx_burst
 * Please Place Description here.
 * @var mac_ax_intf_info::wd_dma_idle_intvl
 * Please Place Description here.
 * @var mac_ax_intf_info::wd_dma_act_intvl
 * Please Place Description here.
 * @var mac_ax_intf_info::multi_tag_num
 * Please Place Description here.
 * @var mac_ax_intf_info::rx_sep_append_len
 * Please Place Description here.
 * @var mac_ax_intf_info::txbd_buf
 * Please Place Description here.
 * @var mac_ax_intf_info::rxbd_buf
 * Please Place Description here.
 * @var mac_ax_intf_info::skip_all
 * Please Place Description here.
 * @var mac_ax_intf_info::txch_map
 * Please Place Description here.
 * @var mac_ax_intf_info::lbc_en
 * Please Place Description here.
 * @var mac_ax_intf_info::lbc_tmr
 * Please Place Description here.
 * @var mac_ax_intf_info::autok_en
 * Please Place Description here.
 */
struct mac_ax_intf_info {
	enum mac_ax_bd_trunc_mode txbd_trunc_mode;
	enum mac_ax_bd_trunc_mode rxbd_trunc_mode;
	enum mac_ax_rxbd_mode rxbd_mode;
	enum mac_ax_tag_mode tag_mode;
	enum mac_ax_tx_burst tx_burst;
	enum mac_ax_rx_burst rx_burst;
	enum mac_ax_wd_dma_intvl wd_dma_idle_intvl;
	enum mac_ax_wd_dma_intvl wd_dma_act_intvl;
	enum mac_ax_multi_tag_num multi_tag_num;
	u16 rx_sep_append_len;
	u8 *txbd_buf;
	u8 *rxbd_buf;
	u8 skip_all;
	struct mac_ax_txdma_ch_map *txch_map;
	enum mac_ax_pcie_func_ctrl lbc_en;
	enum mac_ax_lbc_tmr lbc_tmr;
	enum mac_ax_pcie_func_ctrl autok_en;
	enum mac_ax_pcie_func_ctrl io_rcy_en;
	enum mac_ax_io_rcy_tmr io_rcy_tmr;
	u16 rxbd_num;
	u16 rpbd_num;
	u16 txbd_num;
};

/**
 * @struct mac_ax_pcie_trx_mitigation
 * @brief mac_ax_pcie_trx_mitigation
 *
 * @var mac_ax_pcie_trx_mitigation::txch_map
 * Please Place Description here.
 * @var mac_ax_pcie_trx_mitigation::tx_timer_unit
 * Please Place Description here.
 * @var mac_ax_pcie_trx_mitigation::tx_timer
 * Please Place Description here.
 * @var mac_ax_pcie_trx_mitigation::tx_counter
 * Please Place Description here.
 * @var mac_ax_pcie_trx_mitigation::rxch_map
 * Please Place Description here.
 * @var mac_ax_pcie_trx_mitigation::rx_timer_unit
 * Please Place Description here.
 * @var mac_ax_pcie_trx_mitigation::rx_timer
 * Please Place Description here.
 * @var mac_ax_pcie_trx_mitigation::rx_counter
 * Please Place Description here.
 */
struct mac_ax_pcie_trx_mitigation {
	struct mac_ax_txdma_ch_map *txch_map;
	enum mac_ax_trx_mitigation_timer_unit tx_timer_unit;
	u8 tx_timer;
	u8 tx_counter;
	struct mac_ax_rxdma_ch_map *rxch_map;
	enum mac_ax_trx_mitigation_timer_unit rx_timer_unit;
	u8 rx_timer;
	u8 rx_counter;
};

/**
 * @struct mac_mu_table
 * @brief mac_mu_table
 *
 * @var mac_mu_table::mu_score_tbl_ctrl
 * Please Place Description here.
 * @var mac_mu_table::mu_score_tbl_0
 * Please Place Description here.
 * @var mac_mu_table::mu_score_tbl_1
 * Please Place Description here.
 * @var mac_mu_table::mu_score_tbl_2
 * Please Place Description here.
 * @var mac_mu_table::mu_score_tbl_3
 * Please Place Description here.
 * @var mac_mu_table::mu_score_tbl_4
 * Please Place Description here.
 * @var mac_mu_table::mu_score_tbl_5
 * Please Place Description here.
 */
struct mac_mu_table {
	u32 mu_score_tbl_ctrl;
	u32 mu_score_tbl_0;
	u32 mu_score_tbl_1;
	u32 mu_score_tbl_2;
	u32 mu_score_tbl_3;
	u32 mu_score_tbl_4;
	u32 mu_score_tbl_5;
	struct mac_ax_ss_dl_grp_upd *dlmu_grp_info;
};

/**
 * @struct mac_ax_ss_dl_grp_upd
 * @brief mac_ax_ss_dl_grp_upd
 *
 * @var mac_ax_ss_dl_grp_upd::grp_valid
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::grp_id
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::is_hwgrp
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::rsvd
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::macid_u0
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::macid_u1
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::macid_u2
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::macid_u3
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::macid_u4
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::macid_u5
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::macid_u6
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::macid_u7
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::ac_bitmap_u0
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::ac_bitmap_u1
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::ac_bitmap_u2
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::ac_bitmap_u3
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::ac_bitmap_u4
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::ac_bitmap_u5
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::ac_bitmap_u6
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::ac_bitmap_u7
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::next_protecttype
 * Please Place Description here.
 * @var mac_ax_ss_dl_grp_upd::next_rsptype
 * Please Place Description here.
 */
struct mac_ax_ss_dl_grp_upd {
	u8 grp_valid:1; //0: non valid 1: valid
	u8 grp_id:5; //grp 0~16
	u8 is_hwgrp:1;
	u8 mru:1;
	u8 macid_u0;
	u8 macid_u1;
	u8 macid_u2;
	u8 macid_u3;
	u8 macid_u4;
	u8 macid_u5;
	u8 macid_u6;
	u8 macid_u7;
	u8 ac_bitmap_u0:4;
	u8 ac_bitmap_u1:4;
	u8 ac_bitmap_u2:4;
	u8 ac_bitmap_u3:4;
	u8 ac_bitmap_u4:4;
	u8 ac_bitmap_u5:4;
	u8 ac_bitmap_u6:4;
	u8 ac_bitmap_u7:4;
	u8 next_protecttype:4;
	u8 next_rsptype:4;
	u8 w_idx;

	u8 dcm_u0:1;
	u8 dcm_u1:1;
	u8 dcm_u2:1;
	u8 dcm_u3:1;
	u8 dcm_u4:1;
	u8 dcm_u5:1;
	u8 dcm_u6:1;
	u8 dcm_u7:1;

	u8 ss_u0:2;
	u8 ss_u1:2;
	u8 ss_u2:2;
	u8 ss_u3:2;
	u8 ss_u4:2;
	u8 ss_u5:2;
	u8 ss_u6:2;
	u8 ss_u7:2;

	u8 rate_idx_u0;
	u8 rate_idx_u1;
	u8 rate_idx_u2;
	u8 rate_idx_u3;
	u8 rate_idx_u4;
	u8 rate_idx_u5;
	u8 rate_idx_u6;
	u8 rate_idx_u7;

	u8 trigger_len;
	u8 trigger_ul_threshold;
};

/**
 * @struct mac_ax_ss_ul_grp_upd
 * @brief mac_ax_ss_ul_grp_upd
 *
 * @var mac_ax_ss_ul_grp_upd::macid_u0
 * Please Place Description here.
 * @var mac_ax_ss_ul_grp_upd::macid_u1
 * Please Place Description here.
 * @var mac_ax_ss_ul_grp_upd::grp_bitmap
 * Please Place Description here.
 */
struct mac_ax_ss_ul_grp_upd {
	u8 macid_u0;
	u8 macid_u1;
	u16 grp_bitmap;
};

/**
 * @struct mac_ax_ss_ul_sta_upd
 * @brief mac_ax_ss_ul_sta_upd
 *
 * @var mac_ax_ss_ul_sta_upd::mode
 * Please Place Description here.
 * @var mac_ax_ss_ul_sta_upd::rsvd
 * Please Place Description here.
 * @var mac_ax_ss_ul_sta_upd::macid
 * Please Place Description here.
 * @var mac_ax_ss_ul_sta_upd::bsr_len
 * Please Place Description here.
 */
struct mac_ax_ss_ul_sta_upd {
	u32 mode:8; //0:del; 1: add
	u32 rsvd:24;
	u8 macid[4];
	u16 bsr_len[2];
};

/**
 * @struct mac_ax_2nav_info
 * @brief mac_ax_2nav_info
 *
 * @var mac_ax_2nav_info::plcp_upd_nav_en
 * Please Place Description here.
 * @var mac_ax_2nav_info::tgr_fram_upd_nav_en
 * Please Place Description here.
 * @var mac_ax_2nav_info::nav_up
 * Please Place Description here.
 */
struct mac_ax_2nav_info {
	u8 plcp_upd_nav_en;
	u8 tgr_fram_upd_nav_en;
	u8 nav_up;
};

/**
 * @struct mac_ax_bcn_info
 * @brief mac_ax_bcn_info
 *
 * @var mac_ax_bcn_info::port
 * Please Place Description here.
 * @var mac_ax_bcn_info::mbssid
 * Please Place Description here.
 * @var mac_ax_bcn_info::band
 * Please Place Description here.
 * @var mac_ax_bcn_info::grp_ie_ofst
 * Please Place Description here.
 * @var mac_ax_bcn_info::macid
 * Please Place Description here.
 * @var mac_ax_bcn_info::ssn_sel
 * Please Place Description here.
 * @var mac_ax_bcn_info::ssn_mode
 * Please Place Description here.
 * @var mac_ax_bcn_info::rate_sel
 * Please Place Description here.
 * @var mac_ax_bcn_info::txpwr
 * Please Place Description here.
 * @var mac_ax_bcn_info::txinfo_ctrl_en
 * Please Place Description here.
 * @var mac_ax_bcn_info::ntx_path_en
 * Please Place Description here.
 * @var mac_ax_bcn_info::path_map_a
 * Please Place Description here.
 * @var mac_ax_bcn_info::path_map_b
 * Please Place Description here.
 * @var mac_ax_bcn_info::path_map_c
 * Please Place Description here.
 * @var mac_ax_bcn_info::path_map_d
 * Please Place Description here.
 * @var mac_ax_bcn_info::antsel_a
 * Please Place Description here.
 * @var mac_ax_bcn_info::antsel_b
 * Please Place Description here.
 * @var mac_ax_bcn_info::antsel_c
 * Please Place Description here.
 * @var mac_ax_bcn_info::antsel_d
 * Please Place Description here.
 * @var mac_ax_bcn_info::sw_tsf
 * Please Place Description here.
 * @var mac_ax_bcn_info::pld_buf
 * Please Place Description here.
 * @var mac_ax_bcn_info::pld_len
 * Please Place Description here.
 * @var mac_ax_bcn_info::csa_ofst
 * Please Place Description here.
 */
struct mac_ax_bcn_info {
	u8 port;
	u8 mbssid;
	u8 band;
	u8 grp_ie_ofst;
	u8 macid;
	u8 ssn_sel;
	u8 ssn_mode;
	u16 rate_sel;
	u8 txpwr;
	u8 txinfo_ctrl_en;
	u8 ntx_path_en;
	u8 path_map_a;
	u8 path_map_b;
	u8 path_map_c;
	u8 path_map_d;
	u8 antsel_a;
	u8 antsel_b;
	u8 antsel_c;
	u8 antsel_d;
	u8 sw_tsf;
	u8 *pld_buf;
	u16 pld_len;
	u16 csa_ofst;
};

/**
 * @struct mac_ax_twt_para
 * @brief mac_ax_twt_para
 *
 * @var mac_ax_twt_para::nego_tp
 * Please Place Description here.
 * @var mac_ax_twt_para::act
 * Please Place Description here.
 * @var mac_ax_twt_para::trig
 * Please Place Description here.
 * @var mac_ax_twt_para::flow_tp
 * Please Place Description here.
 * @var mac_ax_twt_para::proct
 * Please Place Description here.
 * @var mac_ax_twt_para::flow_id
 * Please Place Description here.
 * @var mac_ax_twt_para::id
 * Please Place Description here.
 * @var mac_ax_twt_para::wake_exp
 * Please Place Description here.
 * @var mac_ax_twt_para::band
 * Please Place Description here.
 * @var mac_ax_twt_para::port
 * Please Place Description here.
 * @var mac_ax_twt_para::rsp_pm
 * Please Place Description here.
 * @var mac_ax_twt_para::wake_unit
 * Please Place Description here.
 * @var mac_ax_twt_para::impt
 * Please Place Description here.
 * @var mac_ax_twt_para::twtulfixmode
 * Please Place Description here.
 * @var mac_ax_twt_para::rsvd
 * Please Place Description here.
 * @var mac_ax_twt_para::wake_man
 * Please Place Description here.
 * @var mac_ax_twt_para::dur
 * Please Place Description here.
 * @var mac_ax_twt_para::trgt_l
 * Please Place Description here.
 * @var mac_ax_twt_para::trgt_h
 * Please Place Description here.
 */
struct mac_ax_twt_para {
	enum mac_ax_twt_nego_tp nego_tp;
	enum mac_ax_twt_act_tp act;
	u32 trig:1;
	u32 flow_tp:1;
	u32 proct:1;
	u32 flow_id:3;
	u32 id:3;
	u32 wake_exp:5;
	u32 band:1;
	u32 port:3;
	u32 rsp_pm:1;
	u32 wake_unit:1;
	u32 impt:1;
	u32 twtulfixmode:3;
	u32 rsvd:8;

	u16 wake_man;
	u8 dur;
	u32 trgt_l;
	u32 trgt_h;
};

/**
 * @struct mac_ax_twtact_para
 * @brief mac_ax_twtact_para
 *
 * @var mac_ax_twtact_para::act
 * Please Place Description here.
 * @var mac_ax_twtact_para::macid
 * Please Place Description here.
 * @var mac_ax_twtact_para::id
 * Please Place Description here.
 * @var mac_ax_twtact_para::rsvd
 * Please Place Description here.
 */
struct mac_ax_twtact_para {
	enum mac_ax_twtact_act_tp act;
	u16 macid;
	u8 id:3;
	u8 rsvd:5;
};

/**
 * @struct mac_ax_twtanno_para
 * @brief mac_ax_twtanno_para
 *
 * @var mac_ax_twtanno_para::macid
 * Please Place Description here.
 */
struct mac_ax_twtanno_para {
	u8 macid;
};

/**
 * @struct mac_ax_twtanno_c2hpara
 * @brief mac_ax_twtanno_c2hpara
 *
 * @var mac_ax_twtanno_c2hpara::wait_case
 * Please Place Description here.
 * @var mac_ax_twtanno_c2hpara::rsvd
 * Please Place Description here.
 * @var mac_ax_twtanno_c2hpara::macid0
 * Please Place Description here.
 * @var mac_ax_twtanno_c2hpara::macid1
 * Please Place Description here.
 * @var mac_ax_twtanno_c2hpara::macid2
 * Please Place Description here.
 */
struct mac_ax_twtanno_c2hpara {
	u32 wait_case:4;
	u32 rsvd:4;
	u32 macid0:8;
	u32 macid1:8;
	u32 macid2:8;
};

/**
 * @struct mac_ax_port_cfg_para
 * @brief mac_ax_port_cfg_para
 *
 * @var mac_ax_port_cfg_para::mbssid_idx
 * Please Place Description here.
 * @var mac_ax_port_cfg_para::val
 * Please Place Description here.
 * @var mac_ax_port_cfg_para::port
 * Please Place Description here.
 * @var mac_ax_port_cfg_para::band
 * Please Place Description here.
 */
struct mac_ax_port_cfg_para {
	u32 mbssid_idx;
	u32 val;
	u8 port;
	u8 band;
};

/**
 * @struct mac_ax_port_init_para
 * @brief mac_ax_port_init_para
 *
 * @var mac_ax_port_init_para::port_idx
 * Please Place Description here.
 * @var mac_ax_port_init_para::band_idx
 * Please Place Description here.
 * @var mac_ax_port_init_para::net_type
 * Please Place Description here.
 * @var mac_ax_port_init_para::dtim_period
 * Please Place Description here.
 * @var mac_ax_port_init_para::mbid_num
 * Please Place Description here.
 * @var mac_ax_port_init_para::bss_color
 * Please Place Description here.
 * @var mac_ax_port_init_para::bcn_interval
 * Please Place Description here.
 * @var mac_ax_port_init_para::hiq_win
 * Please Place Description here.
 */
struct mac_ax_port_init_para {
	enum mac_ax_port port_idx;
	enum mac_ax_band band_idx;
	enum mac_ax_net_type net_type;
	u8 dtim_period;
	u8 mbid_num;
	u8 bss_color;
	u16 bcn_interval;
	u32 hiq_win;
};

/**
 * @struct mac_ax_dbgpkg
 * @brief mac_ax_dbgpkg
 *
 * @var mac_ax_dbgpkg::ss_dbg_0
 * Please Place Description here.
 * @var mac_ax_dbgpkg::ss_dbg_1
 * Please Place Description here.
 */
struct mac_ax_dbgpkg {
	u32 ss_dbg_0;
	u32 ss_dbg_1;
};

/**
 * @struct mac_ax_dbgport_hw_en
 * @brief mac_ax_dbgport_hw_en
 *
 * @var mac_ax_dbgport_hw_en::system
 * Please Place Description here.
 * @var mac_ax_dbgport_hw_en::wl_cpu
 * Please Place Description here.
 */
struct mac_ax_dbgport_hw_en {
	u8 system:1;
	u8 pinmux:1;
	u8 loader:1;
	u8 hmux:1;
	u8 pcie:1;
	u8 usb:1;
	u8 sdio:1;
	u8 bt:1;

	// WLAN_MAC
	u8 axidma:1;
	u8 wlphydbg_gpio:1;
	u8 btcoexist:1;
	u8 ltecoex:1;
	u8 wlphydbg:1;
	u8 wlan_mac_reg:1;
	u8 wlan_mac_pmc:1;
	u8 calib_top:1;

	//DMAC
	u16 dispatcher_top:1;
	u16 wde_dle:1;
	u16 ple_dle:1;
	u16 wdrls:1;
	u16 dle_cpuio:1;
	u16 bbrpt:1;
	u16 txpktctl:1;
	u16 pktbuffer:1;

	u16 dmac_table:1;
	u16 sta_scheduler:1;
	u16 dmac_pktin:1;
	u16 wsec_top:1;
	u16 mpdu_processor:1;
	u16 dmac_apb_bridge:1;
	u16 ltr_ctrl:1;
	u16 rsvd0:1;

	//CMAC 0
	u8 cmac0_cmac_dma_top:1;
	u8 cmac0_ptcltop:1;
	u8 cmac0_schedulertop:1;
	u8 cmac0_txpwr_ctrl:1;
	u8 cmac0_cmac_apb_bridge:1;
	u8 cmac0_mactx:1;
	u8 cmac0_macrx:1;
	u8 cmac0_wmac_trxptcl:1;

	//CMAC 1
	u8 cmac1_cmac_dma_top:1;
	u8 cmac1_ptcltop:1;
	u8 cmac1_schedulertop:1;
	u8 cmac1_txpwr_ctrl:1;
	u8 cmac1_cmac_apb_bridge:1;
	u8 cmac1_mactx:1;
	u8 cmac1_macrx:1;
	u8 cmac1_wmac_trxptcl:1;

	//others
	u8 cmac_share:1;
	u8 wl_cpu:1;
	u8 rsvd1:6;
};

/**
 * @struct mac_ax_dbgpkg_en
 * @brief mac_ax_dbgpkg_en
 *
 * @var mac_ax_dbgpkg_en::ss_dbg
 * Please Place Description here.
 * @var mac_ax_dbgpkg_en::dle_dbg
 * Please Place Description here.
 * @var mac_ax_dbgpkg_en::dmac_dbg
 * Please Place Description here.
 * @var mac_ax_dbgpkg_en::cmac_dbg
 * Please Place Description here.
 * @var mac_ax_dbgpkg_en::mac_dbg_port
 * Please Place Description here.
 * @var mac_ax_dbgpkg_en::plersvd_dbg
 * Please Place Description here.
 * @var mac_ax_dbgpkg_en::tx_flow_dbg
 * Please Place Description here.
 */
struct mac_ax_dbgpkg_en {
	u8 ss_dbg:1;
	u8 dle_dbg:1;
	u8 dmac_dbg:1;
	u8 cmac_dbg:1;
	u8 mac_dbg_port:1;
	u8 plersvd_dbg:1;
	u8 tx_flow_dbg:1;
	u8 rsvd:1;
	struct mac_ax_dbgport_hw_en dp_hw_en;
};

/**
 * @struct mac_ax_dbgport_hw
 * @brief mac_ax_dbgport_hw
 *
 * @var mac_ax_dbgport_hw::dbg_sel
 * valid value: enum mac_ax_dbgport_sel
 * @var mac_ax_dbgport_hw::dbg_sel_16b
 * valid value: enum mac_ax_dbgport_sel0_16b
 *	         enum mac_ax_dbgport_sel1_16b
 * @var mac_ax_dbgport_hw::dbg_sel_4b
 * valid value: enum mac_ax_dbgport_sel_4b
 * @var mac_ax_dbgport_hw::intn_idx
 * valid value: #define MAC_AX_DP_INTN_IDX_XXX_XXX
 * @var mac_ax_dbgport_hw::mode
 * 0x0: for dump mode
 * 0x1: for LA mode
 * @var mac_ax_dbgport_hw::rsp_val
 * return value: debug port info
 */
struct mac_ax_dbgport_hw {
	// input
	u8 dbg_sel[MAC_AX_DP_SEL_NUM];
	u8 dbg_sel_16b[MAC_AX_DP_SEL_NUM];
	u8 dbg_sel_4b[MAC_AX_DP_SEL_NUM];
	u8 intn_idx[MAC_AX_DP_SEL_NUM];
	u8 mode;
	// output
	u32 rsp_val;
};

/**
 * @struct mac_ax_fwdbg_en
 * @brief mac_ax_fwdbg_en
 *
 * @var mac_ax_fwdbg_en::status_dbg
 * Please Place Description here.
 * @var mac_ax_fwdbg_en::rsv_ple_dbg
 * Please Place Description here.
 * @var mac_ax_fwdbg_en::ps_dbg
 * Please Place Description here.
 */
struct mac_ax_fwdbg_en {
	u8 status_dbg:1;
	u8 rsv_ple_dbg:1;
	u8 ps_dbg:1;
};

union mac_conf_ofld_hioe_param0 {
	u32 register_addr;
	u32 delay_value;
};

union mac_conf_ofld_hioe_param1 {
	u16 byte_data_h;
	u16 bit_mask;
};

union mac_conf_ofld_hioe_param2 {
	u16 byte_data_l;
	u16 bit_data;
};

/**
 * @struct mac_conf_ofld_hioe
 * @brief mac_conf_ofld_hioe
 *
 * @var mac_conf_ofld_hioe::hioe_op
 * Please Place Description here.
 * @var mac_conf_ofld_hioe::inst_type
 * Please Place Description here.
 * @var mac_conf_ofld_hioe::rsvd
 * Please Place Description here.
 * @var mac_conf_ofld_hioe::data_mode
 * Please Place Description here.
 * @var mac_conf_ofld_hioe::param0
 * Please Place Description here.
 * @var mac_conf_ofld_hioe::param1
 * Please Place Description here.
 * @var mac_conf_ofld_hioe::param2
 * Please Place Description here.
 */
struct mac_conf_ofld_hioe {
#define CONF_OFLD_HIOE_OP_RESTORE 0
#define CONF_OFLD_HIOE_OP_BACKUP 1
#define CONF_OFLD_HIOE_OP_BOTH 2
	u8 hioe_op;
#define CONF_OFLD_HIOE_INST_IO 0
#define CONF_OFLD_HIOE_INST_POLLING 1
#define CONF_OFLD_HIOE_INST_DELAY 2
	u8 inst_type;
	u8 rsvd;
#define CONF_OFLD_HIOE_INST_DATA_BYTE 0
#define CONF_OFLD_HIOE_INST_DATA_BIT 3
	u8 data_mode;
	union mac_conf_ofld_hioe_param0 param0;
	union mac_conf_ofld_hioe_param1 param1;
	union mac_conf_ofld_hioe_param2 param2;
};

/**
 * @struct mac_conf_ofld_ddma
 * @brief mac_conf_ofld_ddma
 *
 * @var mac_conf_ofld_ddma::ddma_mode
 * Please Place Description here.
 * @var mac_conf_ofld_ddma::finish
 * Please Place Description here.
 * @var mac_conf_ofld_ddma::dma_len
 * Please Place Description here.
 * @var mac_conf_ofld_ddma::dma_src_addr
 * Please Place Description here.
 * @var mac_conf_ofld_ddma::dma_dst_addr
 * Please Place Description here.
 */
struct mac_conf_ofld_ddma {
#define CONF_OFLD_DDMA_OP_RESTORE 0
#define CONF_OFLD_DDMA_OP_BACKUP 1
#define CONF_OFLD_DDMA_OP_BOTH 2
	u8 ddma_mode;
	u8 finish;
	u16 dma_len;
	u32 dma_src_addr;
	u32 dma_dst_addr;
};

union mac_conf_ofld_req_bd {
	struct mac_conf_ofld_hioe hioe;
	struct mac_conf_ofld_ddma ddma;
};

/**
 * @struct mac_ax_conf_ofld_req
 * @brief mac_ax_conf_ofld_req
 *
 * @var mac_ax_conf_ofld_req::device
 * Please Place Description here.
 * @var mac_ax_conf_ofld_req::rsvd
 * Please Place Description here.
 * @var mac_ax_conf_ofld_req::req
 * Please Place Description here.
 */
struct mac_ax_conf_ofld_req {
#define CONF_OFLD_DEVICE_HIOE 0
#define CONF_OFLD_DEVICE_DDMA 1
	u32 device:8;
	u32 rsvd:24;
	union mac_conf_ofld_req_bd req;
};

/**
 * @struct mac_defeature_value
 * @brief mac_defeature_value
 *
 * @var mac_defeature_value::rx_spatial_stream
 * Please Place Description here.
 * @var mac_defeature_value::bandwidth
 * Please Place Description here.
 * @var mac_defeature_value::tx_spatial_stream
 * Please Place Description here.
 * @var mac_defeature_value::protocol_80211
 * Please Place Description here.
 * @var mac_defeature_value::NIC_router
 * Please Place Description here.
 * @var mac_defeature_value::wl_func_support
 * Please Place Description here.
 * @var mac_defeature_value::hw_special_type
 * Please Place Description here.
 * @var mac_defeature_value::uuid
 * Please Place Description here.
 * @var mac_defeature_value::tx_path_num
 * Please Place Description here.
 * @var mac_defeature_value::rx_path_num
 * Please Place Description here.
 */
struct mac_defeature_value {
	u8 rx_spatial_stream;
	u8 bandwidth;
	u8 tx_spatial_stream;
	u8 protocol_80211;
	u8 NIC_router;
	u8 wl_func_support;
	u8 hw_special_type;
	u32 uuid;
	u8 tx_path_num;
	u8 rx_path_num;
};

/**
 * @struct mac_ax_wowlan_info
 * @brief mac_ax_wowlan_info
 *
 * @var mac_ax_wowlan_info::aoac_report
 * Please Place Description here.
 */
struct mac_ax_wowlan_info {
	u8 *aoac_report;
};

/**
 * @struct mac_ax_p2p_info
 * @brief mac_ax_p2p_info
 *
 * @var mac_ax_p2p_info::macid
 * Please Place Description here.
 * @var mac_ax_p2p_info::p2pid
 * Please Place Description here.
 */
struct mac_ax_p2p_info {
	u8 macid;

	u8 run:1;
	u8 wait_dack:1;
	u8 wait_init:1;
	u8 wait_term:1;
	u8 rsvd:4;
};

/**
 * @struct mac_ax_p2p_act_info
 * @brief mac_ax_p2p_act_info
 *
 * @var mac_ax_p2p_act_info::macid
 * Please Place Description here.
 * @var mac_ax_p2p_act_info::noaid
 * Please Place Description here.
 * @var mac_ax_p2p_act_info::act
 * Please Place Description here.
 * @var mac_ax_p2p_act_info::type
 * Please Place Description here.
 * @var mac_ax_p2p_act_info::all_slep
 * Please Place Description here.
 * @var mac_ax_p2p_act_info::srt
 * Please Place Description here.
 * @var mac_ax_p2p_act_info::itvl
 * Please Place Description here.
 * @var mac_ax_p2p_act_info::dur
 * Please Place Description here.
 * @var mac_ax_p2p_act_info::cnt
 * Please Place Description here.
 * @var mac_ax_p2p_act_info::ctw
 * Please Place Description here.
 */
struct mac_ax_p2p_act_info {
	u8 macid;
	u8 noaid;
	u8 act;
	u8 type;
	u8 all_slep;
	u32 srt;
	u32 itvl;
	u32 dur;
	u8 cnt;
	u16 ctw;
};

struct mac_ax_p2p_macid_info {
	u8 main_macid;
	u8 ctrl_type;
	u8 *bitmap;
	u32 bmap_len;
};

struct mac_ax_t32_togl_info {
	u8 band;
	u8 port;
	u8 en;
	u16 early;
};

struct mac_ax_t32_togl_rpt {
	u8 band;
	u8 port;
	u8 valid;
	u16 early;
	u16 status;
	u32 tsf_l;
	u32 tsf_h;
};

struct mac_ax_port_info {
	u8 stat;
#define MAC_AX_PORT_H2C_IDLE 0
#define MAC_AX_PORT_H2C_BUSY 1
#define MAC_AX_PORT_H2C_FAIL 2
	u8 h2c_sm;
#define MAC_AX_MBSSID_INIT 0
#define MAC_AX_MBSSID_ENABLED 1
	u8 mbssid_en_stat;
};

struct mac_ax_int_stats {
	u32 h2c_reg_uninit;
	u32 h2c_pkt_uninit;
	u32 c2h_reg_uninit;
};

struct mac_ax_twt_info {
	u32 err_rec;
	u8 *pdbg_info;
};

struct mac_ax_pcie_err_info {
	u32 txbd_len_zero:1;
	u32 tx_stuck:1;
	u32 rx_stuck:1;
	u32 rsvd:29;
};

/**
 * @struct mac_txd_ofld_wp_offset
 * @brief txd offload for wp_offset
 *
 * @var mac_txd_ofld_enc_type::enc_type
 * Please Place Description here.
 * @var mac_txd_ofld_hw_amsdu_type::hw_amsdu_type
 * Please Place Description here.
 * @var mac_txd_ofld_hw_hdr_conv_type::hw_hdr_conv_type
 * Please Place Description here.
 */
struct mac_txd_ofld_wp_offset {
	enum mac_txd_ofld_enc_type enc_type;
	enum mac_txd_ofld_hw_amsdu_type hw_amsdu_type;
	enum mac_txd_ofld_hw_hdr_conv_type hw_hdr_conv_type;
};

/*-------------------- Define Struct needed to be moved-----------------------*/

/**
 * @struct mac_ax_tbl_hdr
 * @brief mac_ax_tbl_hdr
 *
 * @var mac_ax_tbl_hdr::rw
 * Please Place Description here.
 * @var mac_ax_tbl_hdr::idx
 * Please Place Description here.
 * @var mac_ax_tbl_hdr::offset
 * Please Place Description here.
 * @var mac_ax_tbl_hdr::len
 * Please Place Description here.
 * @var mac_ax_tbl_hdr::type
 * Please Place Description here.
 */
struct mac_ax_tbl_hdr {
	u8 rw:1;
	u8 idx:7;
	u16 offset:5;
	u16 len:10;
	u16 type:1;
};

/**
 * @struct mac_ax_ru_rate_ent
 * @brief mac_ax_ru_rate_ent
 *
 * @var mac_ax_ru_rate_ent::dcm
 * Please Place Description here.
 * @var mac_ax_ru_rate_ent::ss
 * Please Place Description here.
 * @var mac_ax_ru_rate_ent::mcs
 * Please Place Description here.
 */
struct mac_ax_ru_rate_ent {
	u8 dcm:1;
	u8 ss:3;
	u8 mcs:4;
};

/**
 * @struct mac_ax_dl_fix_sta_ent
 * @brief mac_ax_dl_fix_sta_ent
 *
 * @var mac_ax_dl_fix_sta_ent::mac_id
 * Please Place Description here.
 * @var mac_ax_dl_fix_sta_ent::ru_pos
 * Please Place Description here.
 * @var mac_ax_dl_fix_sta_ent::fix_rate
 * Please Place Description here.
 * @var mac_ax_dl_fix_sta_ent::fix_coding
 * Please Place Description here.
 * @var mac_ax_dl_fix_sta_ent::fix_txbf
 * Please Place Description here.
 * @var mac_ax_dl_fix_sta_ent::fix_pwr_fac
 * Please Place Description here.
 * @var mac_ax_dl_fix_sta_ent::rsvd0
 * Please Place Description here.
 * @var mac_ax_dl_fix_sta_ent::rate
 * Please Place Description here.
 * @var mac_ax_dl_fix_sta_ent::txbf
 * Please Place Description here.
 * @var mac_ax_dl_fix_sta_ent::coding
 * Please Place Description here.
 * @var mac_ax_dl_fix_sta_ent::pwr_boost_fac
 * Please Place Description here.
 * @var mac_ax_dl_fix_sta_ent::rsvd1
 * Please Place Description here.
 * @var mac_ax_dl_fix_sta_ent::rsvd2
 * Please Place Description here.
 */
struct mac_ax_dl_fix_sta_ent {
	u8 mac_id;
	u8 ru_pos[3];
	u8 fix_rate:1;
	u8 fix_coding:1;
	u8 fix_txbf:1;
	u8 fix_pwr_fac:1;
	u8 rsvd0: 4;
	struct mac_ax_ru_rate_ent rate;
	u8 txbf:1;
	u8 coding:1;
	u8 pwr_boost_fac:5;
	u8 rsvd1: 1;
	u8 rsvd2;
};

/**
 * @struct mac_ax_dlru_fixtbl
 * @brief mac_ax_dlru_fixtbl
 *
 * @var mac_ax_dlru_fixtbl::tbl_hdr
 * Please Place Description here.
 * @var mac_ax_dlru_fixtbl::max_sta_num
 * Please Place Description here.
 * @var mac_ax_dlru_fixtbl::min_sta_num
 * Please Place Description here.
 * @var mac_ax_dlru_fixtbl::doppler
 * Please Place Description here.
 * @var mac_ax_dlru_fixtbl::stbc
 * Please Place Description here.
 * @var mac_ax_dlru_fixtbl::gi_ltf
 * Please Place Description here.
 * @var mac_ax_dlru_fixtbl::ma_type
 * Please Place Description here.
 * @var mac_ax_dlru_fixtbl::fixru_flag
 * Please Place Description here.
 * @var mac_ax_dlru_fixtbl::sta
 * Please Place Description here.
 */
struct mac_ax_dlru_fixtbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	u8 max_sta_num:3;
	u8 min_sta_num:3;
	u8 doppler:1;
	u8 stbc:1;
	u8 gi_ltf:3;
	u8 ma_type:1;
	u8 fixru_flag:1;
	struct mac_ax_dl_fix_sta_ent sta[MAC_AX_MAX_RU_NUM];
};

/**
 * @struct mac_ax_ul_fix_sta_ent
 * @brief mac_ax_ul_fix_sta_ent
 *
 * @var mac_ax_ul_fix_sta_ent::mac_id
 * Please Place Description here.
 * @var mac_ax_ul_fix_sta_ent::ru_pos
 * Please Place Description here.
 * @var mac_ax_ul_fix_sta_ent::tgt_rssi
 * Please Place Description here.
 * @var mac_ax_ul_fix_sta_ent::fix_tgt_rssi
 * Please Place Description here.
 * @var mac_ax_ul_fix_sta_ent::fix_rate
 * Please Place Description here.
 * @var mac_ax_ul_fix_sta_ent::fix_coding
 * Please Place Description here.
 * @var mac_ax_ul_fix_sta_ent::coding
 * Please Place Description here.
 * @var mac_ax_ul_fix_sta_ent::rsvd1
 * Please Place Description here.
 * @var mac_ax_ul_fix_sta_ent::rate
 * Please Place Description here.
 */
struct mac_ax_ul_fix_sta_ent {
	u8 mac_id;
	u8 ru_pos[3];
	u8 tgt_rssi[3];
	u8 fix_tgt_rssi: 1;
	u8 fix_rate: 1;
	u8 fix_coding: 1;
	u8 coding: 1;
	u8 rsvd1: 4;
	struct mac_ax_ru_rate_ent rate;
};

/**
 * @struct mac_ax_ulru_fixtbl
 * @brief mac_ax_ulru_fixtbl
 *
 * @var mac_ax_ulru_fixtbl::tbl_hdr
 * Please Place Description here.
 * @var mac_ax_ulru_fixtbl::max_sta_num
 * Please Place Description here.
 * @var mac_ax_ulru_fixtbl::min_sta_num
 * Please Place Description here.
 * @var mac_ax_ulru_fixtbl::doppler
 * Please Place Description here.
 * @var mac_ax_ulru_fixtbl::ma_type
 * Please Place Description here.
 * @var mac_ax_ulru_fixtbl::gi_ltf
 * Please Place Description here.
 * @var mac_ax_ulru_fixtbl::stbc
 * Please Place Description here.
 * @var mac_ax_ulru_fixtbl::fix_tb_t_pe_nom
 * Please Place Description here.
 * @var mac_ax_ulru_fixtbl::tb_t_pe_nom
 * Please Place Description here.
 * @var mac_ax_ulru_fixtbl::fixru_flag
 * Please Place Description here.
 * @var mac_ax_ulru_fixtbl::rsvd
 * Please Place Description here.
 * @var mac_ax_ulru_fixtbl::sta
 * Please Place Description here.
 */
struct mac_ax_ulru_fixtbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	u8 max_sta_num: 3;
	u8 min_sta_num: 3;
	u8 doppler: 1;
	u8 ma_type: 1;
	u8 gi_ltf: 3;
	u8 stbc: 1;
	u8 fix_tb_t_pe_nom: 1;
	u8 tb_t_pe_nom: 2;
	u8 fixru_flag: 1;
	u16 rsvd;
	struct mac_ax_ul_fix_sta_ent sta[MAC_AX_MAX_RU_NUM];
};

/**
 * @struct mac_ax_scanofld_chinfo
 * @brief mac_ax_scanofld_chinfo
 *
 * @var mac_ax_scanofld_chinfo::period
 * how long to stay on this ch. unit: ms
 * @var mac_ax_scanofld_chinfo::dwell_time
 * dwell time if recv bcn. unit: ms. set 0 to disable dwell
 * @var mac_ax_scanofld_chinfo::central_ch
 * central ch
 * @var mac_ax_scanofld_chinfo::pri_ch
 * pri ch
 * @var mac_ax_scanofld_chinfo::bw
 * bandwidth
 * @var mac_ax_scanofld_chinfo::c2h_notify_dwell
 * notify if dwell
 * @var mac_ax_scanofld_chinfo::c2h_notify_preTX
 * notify before tx pkt
 * @var mac_ax_scanofld_chinfo::c2h_notify_postTX
 * notify after tx pkt
 * @var mac_ax_scanofld_chinfo::c2h_notify_enterCH
 * notify when entering ch
 * @var mac_ax_scanofld_chinfo::c2h_notify_leaveCH
 * notify when leaving ch
 * @var mac_ax_scanofld_chinfo::num_addition_pkt
 * num of additional pkt to send. Max=8
 * @var mac_ax_scanofld_chinfo::tx_pkt
 * whether tx probe req + additional pkt or not
 * @var mac_ax_scanofld_chinfo::pause_tx_data
 * wherther disable tx (except manage pkt) after sending probe req
 * @var mac_ax_scanofld_chinfo::ch_band
 * 0:2.4G; 1:5G; 2:6G
 * @var mac_ax_scanofld_chinfo::probe_req_pkt_id
 * probe req pkt id
 * @var mac_ax_scanofld_chinfo::dfs_ch
 * this is dfs ch
 * @var mac_ax_scanofld_chinfo::tx_null
 * tx null when entering / leaving ch
 * @var mac_ax_scanofld_chinfo::rand_seq_num
 * enable random seq num for probe req
 * @var mac_ax_scanofld_chinfo::cfg_tx_pwr
 * cfg tx pwr or not
 * @var mac_ax_scanofld_chinfo::rsvd0
 * rsvd
 * @var mac_ax_scanofld_chinfo::additional_pkt_id
 * id of additional pkts to send
 * @var mac_ax_scanofld_chinfo::tx_pwr_idx
 * tx pwr idx to write
 * @var mac_ax_scanofld_chinfo::rsvd1
 * rsvd
 */
#pragma pack(push)
#pragma pack(1)
struct mac_ax_scanofld_chinfo {
	/* dword 0 */
	u8 period;
	u8 dwell_time;
	u8 central_ch;
	u8 pri_ch;
	/* dword 1 */
	u8 bw: 3;
	u8 c2h_notify_dwell: 1;
	u8 c2h_notify_preTX: 1;
	u8 c2h_notify_postTX: 1;
	u8 c2h_notify_enterCH: 1;
	u8 c2h_notify_leaveCH: 1;
	u8 num_addition_pkt:4;
	u8 tx_pkt: 1;
	u8 pause_tx_data: 1;
	u8 ch_band: 2;
	u8 probe_req_pkt_id;
	u8 dfs_ch: 1;
	u8 tx_null: 1;
	u8 rand_seq_num: 1;
	u8 cfg_tx_pwr: 1;
	u8 tx_probe_req: 1;
	u8 rsvd0: 3;
	/* dword 2 3*/
	u8 additional_pkt_id[SCANOFLD_MAX_ADDITION_PKT_NUM];
	/* dword 4 */
	u16 tx_pwr_idx;
	u8 chkpt_timer;
	u8 additional_pkt_acking;
	/* dword 5 */
	u16 fw_probe0_ssids;
	u8 fw_probe0_tplt_id;
	u8 rsvd1;
};

/**
 * @struct mac_ax_scanofld_chrpt
 * @brief mac_ax_scanofld_chrpt
 *
 * @var mac_ax_scanofld_chrpt::pri_ch
 * pri ch
 * @var mac_ax_scanofld_chrpt::rx_cnt
 * beacon / probe rsp rx in this ch during scanning lifetime
 * @var mac_ax_scanofld_chrpt::tx_fail
 * num of tx fail caused by scanofld in this ch during scanning lifetime
 * @var mac_ax_scanofld_chrpt::parsed
 * have ever parsed a matched SSID in this ch during scanning lifetime
 */
struct mac_ax_scanofld_chrpt {
	/* dword 0 */
	u8 pri_ch;
	u8 rx_cnt;
	u8 tx_fail: 7;
	u8 parsed: 1;
	u8 rsvd;
};

#pragma pack(pop)

struct mac_ax_scanofld_ssid {
	u32 len: 8;
	u32 rsvd: 24;
	u8 content[SCANOFLD_MAX_SSID_LEN];
};

/**
 * @struct mac_ax_scanofld_param
 * @brief mac_ax_scanofld_param
 *
 * @var mac_ax_scanofld_param::macid
 * macid
 * @var mac_ax_scanofld_param::norm_cy
 * normal cycle. available when scan_type=2
 * @var mac_ax_scanofld_param::port_id
 * port id
 * @var mac_ax_scanofld_param::band
 * band
 * @var mac_ax_scanofld_param::operation
 * 1: start scanning. 0: stop. 2: set param
 * @var mac_ax_scanofld_param::target_ch_ch_band
 * ch band of target channel (0: 2.4, 1:5, 2:6)
 * @var mac_ax_scanofld_param::rsvd0
 * rsvd
 * @var mac_ax_scanofld_param::c2h_end
 * whether notify when scan end
 * @var mac_ax_scanofld_param::target_ch_mode
 * whether switch to target (op) channel after walking through list. available when scan_type!=3
 * @var mac_ax_scanofld_param::start_mode
 * 0: start immediatly. 1: start at assigned TSF
 * @var mac_ax_scanofld_param::scan_type
 * 0: scan once.
 * 1: normal repeat mode. period = normal_pd.
 * 2: normal_slow repeat mode. normal_pd for norm_cy times, then slow_pd
 * 3: seamless repeat mode
 * @var mac_ax_scanofld_param::target_ch_bw
 * BW of target (op) channel. available when target_ch_mode = 1
 * @var mac_ax_scanofld_param::target_pri_ch
 * pri ch of target (op) channel. available when target_ch_mode = 1
 * @var mac_ax_scanofld_param::target_central_ch
 * central ch of target (op) channel. available when target_ch_mode = 1
 * @var mac_ax_scanofld_param::norm_pd
 * normal period. available when scan_type = 1,2
 * @var mac_ax_scanofld_param::slow_pd
 * slow period. available when scan_type = 2
 * @var mac_ax_scanofld_param::tsf_high
 * higher 32 bit of start tsf. available when start_mode = 1
 * @var mac_ax_scanofld_param::tsf_low
 * lower 32 bit of start tsf. available when start_mode = 1
 */
struct mac_ax_scanofld_param {
	/* dword0 */
	u32 macid:8;
	u32 norm_cy:8;
	u32 port_id:3;
	u32 band:1;
	u32 operation:2;
	u32 target_ch_ch_band:2;
	u32 rsvd0:8;
	/* dword1 */
	u32 c2h_end:1;
	u32 target_ch_mode:1;
	u32 start_mode:1;
	u32 scan_type:2;
	u32 target_ch_bw:3;
	u32 target_pri_ch:8;
	u32 target_central_ch:8;
	u32 target_num_pkts:8;
	/* dword2 */
	u32 norm_pd:16;
	u32 slow_pd:16;
	/* dword3 */
	u32 tsf_high;
	/* dword4 */
	u32 tsf_low;
	/* dword5 */
	u8 target_ch_pkt[SCANOFLD_MAX_TARGET_PKT_NUM];
	/* dword 6 */
	u32 numssid:8;
	u32 rsvd1: 8;
	u32 rsvd2: 16;
	/* will be repackeds as len-val */
	struct mac_ax_scanofld_ssid ssid[SCANOFLD_MAX_SSID_NUM];
};

/**
 * @struct mac_ax_scanofld_rsp
 * @brief mac_ax_scanofld_rsp
 *
 * @var mac_ax_scanofld_rsp::pri_ch
 * pri ch
 * @var mac_ax_scanofld_rsp::scanned_round
 * current passed round (valid when reason in [5, 6])
 * @var mac_ax_scanofld_rsp::notify reason
 * ref to mac_ax_scanofld_notify_reason
 * @var mac_ax_scanofld_rsp::status
 * ref to mac_ax_scanofld_notify_status
 * @var mac_ax_scanofld_rsp::actual_period
 * actual period of this channel (valid when reason in [4, 5, 6])
 * @var mac_ax_scanofld_rsp::spent_low
 * lower 32 of spent time from scan start (valid when reason in [5, 6])
 * @var mac_ax_scanofld_rsp::spent_high
 * higher 32 of spent time from scan start (valid when reason in [5, 6])
 * @var mac_ax_scanofld_rsp::tx_fail_cnt
 * tx fail count caused by scanofld (valid when reason in [4, 5, 6])
 * @var mac_ax_scanofld_rsp::air_density
 * air_density in this channel (valid when reason in [5, 6])
 * @var mac_ax_scanofld_rsp::num_ch_rpt
 * num of ch rpt below (valid when reason in [5, 6] and wow scanofld)
 * @var mac_ax_scanofld_rsp::ch_rpt_size
 * size of an chrpt in dword (valid when reason in [5, 6] and wow scanofld)
 * @var mac_ax_scanofld_rsp::rsvd0
 * rsvd
 */
struct mac_ax_scanofld_rsp {
	/* dword0 */
	u32 pri_ch:8;
	u32 scanned_round:8;
	u32 notify_reason:4;
	u32 status:4;
	u32 actual_period:8;
	/* dword1 */
	u32 spent_low;
	/* dword2 */
	u32 spent_high;
	/* dword3 */
	u32 tx_fail_cnt:4;
	u32 air_density:4;
	u32 num_ch_rpt:8;
	u32 ch_rpt_size:8;
	u32 ch_band: 2;
	u32 band: 1;
	u32 rsvd0: 5;
};

/**
 * @struct mac_ax_scanofld_info
 * @brief mac_ax_scanofld_info
 *
 * @var mac_ax_scanofld_info::list
 * halmac-hold CHlist
 * @var mac_ax_scanofld_info::HalmacChListBusy
 * halmac CHlist is busy or not
 * @var mac_ax_scanofld_info::FwChListBusy
 * fw CHlist is busy or not
 * @var mac_ax_scanofld_info::last_fw_chlist_busy
 * fw CHlist is busy or not
 * @var mac_ax_scanofld_info::clearHalmacList
 * clear halmac-hold CHlist after sending to fw or not
 * @var mac_ax_scanofld_info::scanBusy
 * fw scanning or not
 * @var mac_ax_scanofld_info::last_fw_scan_busy
 * fw scanning or not
 */
struct mac_ax_scanofld_info{
	struct scan_chinfo_list *list[MAC_AX_BAND_NUM];
	mac_ax_mutex drv_chlist_state_lock;
	mac_ax_mutex fw_chlist_state_lock;
	u8 drv_chlist_busy[MAC_AX_BAND_NUM];
	u8 fw_chlist_busy[MAC_AX_BAND_NUM];
	u8 last_fw_chlist_busy[MAC_AX_BAND_NUM];
	u8 fw_scan_busy[MAC_AX_BAND_NUM];
	u8 last_fw_scan_busy[MAC_AX_BAND_NUM];
};

/*--------------------END Define Struct needed to be moved--------------------*/
/*--------------------Define HCI related structure----------------------------*/

/**
 * @struct mac_ax_hfc_ch_cfg
 * @brief mac_ax_hfc_ch_cfg
 *
 * @var mac_ax_hfc_ch_cfg::min
 * Please Place Description here.
 * @var mac_ax_hfc_ch_cfg::max
 * Please Place Description here.
 * @var mac_ax_hfc_ch_cfg::grp
 * Please Place Description here.
 */
struct mac_ax_hfc_ch_cfg {
	u16 min;
	u16 max;
#define grp_0 0
#define grp_1 1
#define grp_num 2
	u8 grp;
};

/**
 * @struct mac_ax_hfc_ch_info
 * @brief mac_ax_hfc_ch_info
 *
 * @var mac_ax_hfc_ch_info::aval
 * Please Place Description here.
 * @var mac_ax_hfc_ch_info::used
 * Please Place Description here.
 */
struct mac_ax_hfc_ch_info {
	u16 aval;
	u16 used;
};

/**
 * @struct mac_ax_hfc_pub_cfg
 * @brief mac_ax_hfc_pub_cfg
 *
 * @var mac_ax_hfc_pub_cfg::group0
 * Please Place Description here.
 * @var mac_ax_hfc_pub_cfg::group1
 * Please Place Description here.
 * @var mac_ax_hfc_pub_cfg::pub_max
 * Please Place Description here.
 * @var mac_ax_hfc_pub_cfg::wp_thrd
 * Please Place Description here.
 */
struct mac_ax_hfc_pub_cfg {
	u16 group0;
	u16 group1;
	u16 pub_max;
	u16 wp_thrd;
};

/**
 * @struct mac_ax_hfc_pub_info
 * @brief mac_ax_hfc_pub_info
 *
 * @var mac_ax_hfc_pub_info::g0_used
 * Please Place Description here.
 * @var mac_ax_hfc_pub_info::g1_used
 * Please Place Description here.
 * @var mac_ax_hfc_pub_info::g0_aval
 * Please Place Description here.
 * @var mac_ax_hfc_pub_info::g1_aval
 * Please Place Description here.
 * @var mac_ax_hfc_pub_info::pub_aval
 * Please Place Description here.
 * @var mac_ax_hfc_pub_info::wp_aval
 * Please Place Description here.
 */
struct mac_ax_hfc_pub_info {
	u16 g0_used;
	u16 g1_used;
	u16 g0_aval;
	u16 g1_aval;
	u16 pub_aval;
	u16 wp_aval;
};

/**
 * @struct mac_ax_hfc_prec_cfg
 * @brief mac_ax_hfc_prec_cfg
 *
 * @var mac_ax_hfc_prec_cfg::ch011_prec
 * Please Place Description here.
 * @var mac_ax_hfc_prec_cfg::h2c_prec
 * Please Place Description here.
 * @var mac_ax_hfc_prec_cfg::wp_ch07_prec
 * Please Place Description here.
 * @var mac_ax_hfc_prec_cfg::wp_ch811_prec
 * Please Place Description here.
 * @var mac_ax_hfc_prec_cfg::ch011_full_cond
 * Please Place Description here.
 * @var mac_ax_hfc_prec_cfg::h2c_full_cond
 * Please Place Description here.
 * @var mac_ax_hfc_prec_cfg::wp_ch07_full_cond
 * Please Place Description here.
 * @var mac_ax_hfc_prec_cfg::wp_ch811_full_cond
 * Please Place Description here.
 */
struct mac_ax_hfc_prec_cfg {
	u16 ch011_prec;
	u16 h2c_prec;
	u16 wp_ch07_prec;
	u16 wp_ch811_prec;
	u8 ch011_full_cond;
	u8 h2c_full_cond;
	u8 wp_ch07_full_cond;
	u8 wp_ch811_full_cond;
};

/**
 * @struct mac_ax_hfc_param
 * @brief mac_ax_hfc_param
 *
 * @var mac_ax_hfc_param::en
 * Please Place Description here.
 * @var mac_ax_hfc_param::h2c_en
 * Please Place Description here.
 * @var mac_ax_hfc_param::mode
 * Please Place Description here.
 * @var mac_ax_hfc_param::ch_cfg
 * Please Place Description here.
 * @var mac_ax_hfc_param::ch_info
 * Please Place Description here.
 * @var mac_ax_hfc_param::pub_cfg
 * Please Place Description here.
 * @var mac_ax_hfc_param::pub_info
 * Please Place Description here.
 * @var mac_ax_hfc_param::prec_cfg
 * Please Place Description here.
 */
struct mac_ax_hfc_param {
	u8 en;
	u8 h2c_en;
	u8 mode;
	struct mac_ax_hfc_ch_cfg *ch_cfg;
	struct mac_ax_hfc_ch_info *ch_info;
	struct mac_ax_hfc_pub_cfg *pub_cfg;
	struct mac_ax_hfc_pub_info *pub_info;
	struct mac_ax_hfc_prec_cfg *prec_cfg;
};

/**
 * @struct mac_ax_sdio_tx_info
 * @brief mac_ax_sdio_tx_info
 *
 * @var mac_ax_sdio_tx_info::total_size
 * Please Place Description here.
 * @var mac_ax_sdio_tx_info::dma_txagg_num
 * Please Place Description here.
 * @var mac_ax_sdio_tx_info::ch_dma
 * Please Place Description here.
 * @var mac_ax_sdio_tx_info::pkt_size
 * Please Place Description here.
 * @var mac_ax_sdio_tx_info::wp_offset
 * Please Place Description here.
 * @var mac_ax_sdio_tx_info::chk_cnt
 * Please Place Description here.
 * @var mac_ax_sdio_tx_info::wde_rqd_num
 * Please Place Description here.
 * @var mac_ax_sdio_tx_info::ple_rqd_num
 * Please Place Description here.
 */
struct mac_ax_sdio_tx_info {
	u32 total_size;
	u8 dma_txagg_num;
	u8 ch_dma;
	u16 *pkt_size;
	u8 *wp_offset;
	u8 chk_cnt;
	u16 wde_rqd_num;
	u16 ple_rqd_num;
};

/**
 * @struct mac_ax_sdio_clk_mon_cfg
 * @brief mac_ax_sdio_clk_mon_cfg
 *
 * @var mac_ax_sdio_clk_mon_cfg::mon
 * Please Place Description here.
 * @var mac_ax_sdio_clk_mon_cfg::cycle
 * Please Place Description here.
 */
struct mac_ax_sdio_clk_mon_cfg {
	enum mac_ax_sdio_clk_mon mon;
	u32 cycle;
};

/**
 * @struct mac_ax_pcie_ltr_rx_th_ctrl
 * @brief mac_ax_pcie_ltr_rx_th_ctrl
 *
 * @var mac_ax_pcie_ltr_rx_th_ctrl::ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_rx_th_ctrl::val
 * Please Place Description here.
 */
struct mac_ax_pcie_ltr_rx_th_ctrl {
	enum mac_ax_pcie_func_ctrl ctrl;
	u16 val;
};

/**
 * @struct mac_ax_pcie_ltr_lat_ctrl
 * @brief mac_ax_pcie_ltr_lat_ctrl
 *
 * @var mac_ax_pcie_ltr_lat_ctrl::ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_lat_ctrl::val
 * Please Place Description here.
 */
struct mac_ax_pcie_ltr_lat_ctrl {
	enum mac_ax_pcie_func_ctrl ctrl;
	u32 val;
};

/**
 * @struct mac_ax_pcie_ltr_param
 * @brief mac_ax_pcie_ltr_param
 *
 * @var mac_ax_pcie_ltr_param::write
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_param::read
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_param::ltr_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_param::ltr_hw_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_param::ltr_spc_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_param::ltr_idle_timer_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_param::ltr_rx0_th_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_param::ltr_rx1_th_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_param::ltr_idle_lat_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_ltr_param::ltr_act_lat_ctrl
 * Please Place Description here.
 */
struct mac_ax_pcie_ltr_param {
	u8 write;
	u8 read;
	enum mac_ax_pcie_func_ctrl ltr_ctrl;
	enum mac_ax_pcie_func_ctrl ltr_hw_ctrl;
	enum mac_ax_pcie_ltr_spc ltr_spc_ctrl;
	enum mac_ax_pcie_ltr_idle_timer ltr_idle_timer_ctrl;
	struct mac_ax_pcie_ltr_rx_th_ctrl ltr_rx0_th_ctrl;
	struct mac_ax_pcie_ltr_rx_th_ctrl ltr_rx1_th_ctrl;
	struct mac_ax_pcie_ltr_lat_ctrl ltr_idle_lat_ctrl;
	struct mac_ax_pcie_ltr_lat_ctrl ltr_act_lat_ctrl;
};

/**
 * @struct mac_ax_usb_tx_agg_cfg
 * @brief mac_ax_usb_tx_agg_cfg
 *
 * @var mac_ax_usb_tx_agg_cfg::pkt
 * Please Place Description here.
 * @var mac_ax_usb_tx_agg_cfg::agg_num
 * Please Place Description here.
 */
struct mac_ax_usb_tx_agg_cfg {
	u8 *pkt;
	u32 agg_num;
};

/**
 * @struct mac_ax_pcie_cfgspc_param
 * @brief mac_ax_pcie_cfgspc_param
 *
 * @var mac_ax_pcie_cfgspc_param::write
 * Please Place Description here.
 * @var mac_ax_pcie_cfgspc_param::read
 * Please Place Description here.
 * @var mac_ax_pcie_cfgspc_param::l0s_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_cfgspc_param::l1_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_cfgspc_param::l1ss_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_cfgspc_param::wake_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_cfgspc_param::crq_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_cfgspc_param::clkdly_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_cfgspc_param::l0sdly_ctrl
 * Please Place Description here.
 * @var mac_ax_pcie_cfgspc_param::l1dly_ctrl
 * Please Place Description here.
 */
struct mac_ax_pcie_cfgspc_param {
	u8 write;
	u8 read;
	enum mac_ax_pcie_func_ctrl l0s_ctrl;
	enum mac_ax_pcie_func_ctrl l1_ctrl;
	enum mac_ax_pcie_func_ctrl l1ss_ctrl;
	enum mac_ax_pcie_func_ctrl wake_ctrl;
	enum mac_ax_pcie_func_ctrl crq_ctrl;
	enum mac_ax_pcie_clkdly clkdly_ctrl;
	enum mac_ax_pcie_l0sdly l0sdly_ctrl;
	enum mac_ax_pcie_l1dly l1dly_ctrl;
};

/**
 * @struct mac_ax_rx_agg_thold
 * @brief mac_ax_rx_agg_thold
 *
 * @var mac_ax_rx_agg_thold::drv_define
 * Please Place Description here.
 * @var mac_ax_rx_agg_thold::timeout
 * Please Place Description here.
 * @var mac_ax_rx_agg_thold::size
 * Please Place Description here.
 * @var mac_ax_rx_agg_thold::pkt_num
 * Please Place Description here.
 */
struct mac_ax_rx_agg_thold {
	u8 drv_define;
	u8 timeout;
	u8 size;
	u8 pkt_num;
};

/**
 * @struct mac_ax_lifetime_en
 * @brief mac_ax_lifetime_en
 *
 * @var mac_ax_lifetime_en::acq_en
 * Please Place Description here.
 * @var mac_ax_lifetime_en::mgq_en
 * Please Place Description here.
 */
struct mac_ax_lifetime_en {
	u8 acq_en;
	u8 mgq_en;
};

/**
 * @struct mac_ax_lifetime_val
 * @brief mac_ax_lifetime_val
 *
 * @var mac_ax_lifetime_val::acq_val_1
 * Please Place Description here.
 * @var mac_ax_lifetime_val::acq_val_2
 * Please Place Description here.
 * @var mac_ax_lifetime_val::acq_val_3
 * Please Place Description here.
 * @var mac_ax_lifetime_val::acq_val_4
 * Please Place Description here.
 * @var mac_ax_lifetime_val::mgq_val
 * Please Place Description here.
 */
struct mac_ax_lifetime_val {
	u16 acq_val_1;
	u16 acq_val_2;
	u16 acq_val_3;
	u16 acq_val_4;
	u16 mgq_val;
};

/**
 * @struct mac_ax_cfg_bw
 * @brief mac_ax_cfg_bw
 *
 * @var mac_ax_cfg_bw::pri_ch
 * Please Place Description here.
 * @var mac_ax_cfg_bw::central_ch
 * Please Place Description here.
 * @var mac_ax_cfg_bw::band
 * Please Place Description here.
 * @var mac_ax_cfg_bw::rsvd
 * Please Place Description here.
 * @var mac_ax_cfg_bw::cbw
 * Please Place Description here.
 */
struct mac_ax_cfg_bw {
	u8 pri_ch;
	u8 central_ch;
	u16 band: 1; /*hw_band*/
	u16 band_type:2; /*2_4G= 0; 5G= 1,6G= 2,*/
	u16 rsvd: 13;
	enum channel_width cbw;
};

/*-------------------- Define Efuse related structure ------------------------*/

/**
 * @struct mac_ax_pg_efuse_info
 * @brief mac_ax_pg_efuse_info
 *
 * @var mac_ax_pg_efuse_info::efuse_map
 * Please Place Description here.
 * @var mac_ax_pg_efuse_info::efuse_map_size
 * Please Place Description here.
 * @var mac_ax_pg_efuse_info::efuse_mask
 * Please Place Description here.
 * @var mac_ax_pg_efuse_info::efuse_mask_size
 * Please Place Description here.
 */
struct mac_ax_pg_efuse_info {
	u8 *efuse_map;
	u32 efuse_map_size;
	u8 *efuse_mask;
	u32 efuse_mask_size;
};

/**
 * @struct mac_ax_efuse_param
 * @brief mac_ax_efuse_param
 *
 * @var mac_ax_efuse_param::efuse_map
 * Please Place Description here.
 * @var mac_ax_efuse_param::bt_efuse_map
 * Please Place Description here.
 * @var mac_ax_efuse_param::log_efuse_map
 * Please Place Description here.
 * @var mac_ax_efuse_param::bt_log_efuse_map
 * Please Place Description here.
 * @var mac_ax_efuse_param::efuse_end
 * Please Place Description here.
 * @var mac_ax_efuse_param::bt_efuse_end
 * Please Place Description here.
 * @var mac_ax_efuse_param::efuse_map_valid
 * Please Place Description here.
 * @var mac_ax_efuse_param::bt_efuse_map_valid
 * Please Place Description here.
 * @var mac_ax_efuse_param::log_efuse_map_valid
 * Please Place Description here.
 * @var mac_ax_efuse_param::bt_log_efuse_map_valid
 * Please Place Description here.
 * @var mac_ax_efuse_param::auto_ck_en
 * Please Place Description here.
 * @var mac_ax_efuse_param::dav_efuse_map
 * Please Place Description here.
 * @var mac_ax_efuse_param::dav_log_efuse_map
 * Please Place Description here.
 * @var mac_ax_efuse_param::dav_efuse_end
 * Please Place Description here.
 * @var mac_ax_efuse_param::dav_efuse_map_valid
 * Please Place Description here.
 * @var mac_ax_efuse_param::dav_log_efuse_map_valid
 * Please Place Description here.
 * @var mac_ax_efuse_param::hidden_rf_map
 * Please Place Description here.
 * @var mac_ax_efuse_param::hidden_rf_map_valid
 * Please Place Description here.
 */
struct mac_ax_efuse_param {
	u8 *efuse_map;
	u8 *bt_efuse_map;
	u8 *log_efuse_map;
	u8 *bt_log_efuse_map;
	u32 efuse_end;
	u32 bt_efuse_end;
	u8 efuse_map_valid;
	u8 bt_efuse_map_valid;
	u8 log_efuse_map_valid;
	u8 bt_log_efuse_map_valid;
	u8 auto_ck_en;
	u8 *dav_efuse_map;
	u8 *dav_log_efuse_map;
	u32 dav_efuse_end;
	u8 dav_efuse_map_valid;
	u8 dav_log_efuse_map_valid;
	u8 *hidden_rf_map;
	u8 hidden_rf_map_valid;
};

/**
 * @struct mac_disable_rf_ofld_info
 * @brief mac_disable_rf_ofld_info
 *
 * @var mac_disable_rf_ofld_info::func
 * Please Place Description here.
 * @var mac_disable_rf_ofld_info::type
 * Please Place Description here.
 */
struct mac_disable_rf_ofld_info {
	enum mac_ax_disable_rf_func func;
	enum mac_ax_net_type type;
};

/*-------------------- Define offload related Struct -------------------------*/

/**
 * @struct mac_ax_read_req
 * @brief mac_ax_read_req
 *
 * @var mac_ax_read_req::value_len
 * Please Place Description here.
 * @var mac_ax_read_req::rsvd0
 * Please Place Description here.
 * @var mac_ax_read_req::ls
 * Please Place Description here.
 * @var mac_ax_read_req::ofld_id
 * Please Place Description here.
 * @var mac_ax_read_req::entry_num
 * Please Place Description here.
 * @var mac_ax_read_req::offset
 * Please Place Description here.
 * @var mac_ax_read_req::rsvd1
 * Please Place Description here.
 */
struct mac_ax_read_req {
	u16 value_len:11;
	u16 rsvd0: 4;
	u16 ls: 1;
	u8 ofld_id;
	u8 entry_num;
	u16 offset;
	u16 rsvd1;
};

/**
 * @struct mac_ax_read_ofld_info
 * @brief mac_ax_read_ofld_info
 *
 * @var mac_ax_read_ofld_info::buf
 * Please Place Description here.
 * @var mac_ax_read_ofld_info::buf_wptr
 * Please Place Description here.
 * @var mac_ax_read_ofld_info::last_req
 * Please Place Description here.
 * @var mac_ax_read_ofld_info::buf_size
 * Please Place Description here.
 * @var mac_ax_read_ofld_info::avl_buf_size
 * Please Place Description here.
 * @var mac_ax_read_ofld_info::used_size
 * Please Place Description here.
 * @var mac_ax_read_ofld_info::req_num
 * Please Place Description here.
 */
struct mac_ax_read_ofld_info {
	u8 *buf;
	u8 *buf_wptr;
	struct mac_ax_read_req *last_req;
	u32 buf_size;
	u32 avl_buf_size;
	u32 used_size;
	u32 req_num;
};

/**
 * @struct mac_ax_read_ofld_value
 * @brief mac_ax_read_ofld_value
 *
 * @var mac_ax_read_ofld_value::len
 * Please Place Description here.
 * @var mac_ax_read_ofld_value::rsvd
 * Please Place Description here.
 * @var mac_ax_read_ofld_value::buf
 * Please Place Description here.
 */
struct mac_ax_read_ofld_value {
	u16 len;
	u16 rsvd;
	u8 *buf;
};

/**
 * @struct mac_ax_efuse_ofld_info
 * @brief mac_ax_efuse_ofld_info
 *
 * @var mac_ax_efuse_ofld_info::buf
 * Please Place Description here.
 */
struct mac_ax_efuse_ofld_info {
	u8 *buf;
};

/**
 * @struct mac_ax_write_req
 * @brief mac_ax_write_req
 *
 * @var mac_ax_write_req::value_len
 * Please Place Description here.
 * @var mac_ax_write_req::rsvd0
 * Please Place Description here.
 * @var mac_ax_write_req::polling
 * Please Place Description here.
 * @var mac_ax_write_req::mask_en
 * Please Place Description here.
 * @var mac_ax_write_req::ls
 * Please Place Description here.
 * @var mac_ax_write_req::ofld_id
 * Please Place Description here.
 * @var mac_ax_write_req::entry_num
 * Please Place Description here.
 * @var mac_ax_write_req::offset
 * Please Place Description here.
 * @var mac_ax_write_req::rsvd1
 * Please Place Description here.
 */
struct mac_ax_write_req {
	u16 value_len:11;
	u16 rsvd0: 2;
	u16 polling: 1;
	u16 mask_en: 1;
	u16 ls: 1;
	u8 ofld_id;
	u8 entry_num;
	u16 offset;
	u16 rsvd1;
};

/**
 * @struct mac_ax_write_ofld_info
 * @brief mac_ax_write_ofld_info
 *
 * @var mac_ax_write_ofld_info::buf
 * Please Place Description here.
 * @var mac_ax_write_ofld_info::buf_wptr
 * Please Place Description here.
 * @var mac_ax_write_ofld_info::last_req
 * Please Place Description here.
 * @var mac_ax_write_ofld_info::buf_size
 * Please Place Description here.
 * @var mac_ax_write_ofld_info::avl_buf_size
 * Please Place Description here.
 * @var mac_ax_write_ofld_info::used_size
 * Please Place Description here.
 * @var mac_ax_write_ofld_info::req_num
 * Please Place Description here.
 */
struct mac_ax_write_ofld_info {
	u8 *buf;
	u8 *buf_wptr;
	struct mac_ax_write_req *last_req;
	u32 buf_size;
	u32 avl_buf_size;
	u32 used_size;
	u32 req_num;
};

/**
 * @struct mac_ax_conf_ofld_info
 * @brief mac_ax_conf_ofld_info
 *
 * @var mac_ax_conf_ofld_info::buf
 * Please Place Description here.
 * @var mac_ax_conf_ofld_info::buf_wptr
 * Please Place Description here.
 * @var mac_ax_conf_ofld_info::buf_size
 * Please Place Description here.
 * @var mac_ax_conf_ofld_info::avl_buf_size
 * Please Place Description here.
 * @var mac_ax_conf_ofld_info::used_size
 * Please Place Description here.
 * @var mac_ax_conf_ofld_info::req_num
 * Please Place Description here.
 */
struct mac_ax_conf_ofld_info {
	u8 *buf;
	u8 *buf_wptr;
	u32 buf_size;
	u32 avl_buf_size;
	u32 used_size;
	u16 req_num;
};

/**
 * @struct mac_ax_pkt_ofld_info
 * @brief mac_ax_pkt_ofld_info
 *
 * @var mac_ax_pkt_ofld_info::last_op
 * Please Place Description here.
 * @var mac_ax_pkt_ofld_info::free_id_count
 * Please Place Description here.
 * @var mac_ax_pkt_ofld_info::used_id_count
 * Please Place Description here.
 * @var mac_ax_pkt_ofld_info::id_bitmap
 * Please Place Description here.
 */
struct mac_ax_pkt_ofld_info {
#define PKT_OFLD_MAX_COUNT 256
#define PKT_OFLD_NOT_EXISTS_ID 0xFF
#define PKT_OFLD_MAX_VALID_ID_NUM (PKT_OFLD_MAX_COUNT - 1)
	u8 last_op;
	u16 free_id_count;
	u16 used_id_count;
	u8 id_bitmap[PKT_OFLD_MAX_COUNT >> 3];
};

/**
 * @struct mac_ax_pkt_ofld_pkt
 * @brief mac_ax_pkt_ofld_pkt
 *
 * @var mac_ax_pkt_ofld_pkt::pkt_id
 * Please Place Description here.
 * @var mac_ax_pkt_ofld_pkt::rsvd
 * Please Place Description here.
 * @var mac_ax_pkt_ofld_pkt::pkt_len
 * Please Place Description here.
 * @var mac_ax_pkt_ofld_pkt::pkt
 * Please Place Description here.
 */
struct mac_ax_pkt_ofld_pkt {
	u8 pkt_id;
	u8 rsvd;
	u16 pkt_len;
	u8 *pkt;
};

/**
 * @struct mac_ax_general_pkt_ids
 * @brief mac_ax_general_pkt_ids
 *
 * @var mac_ax_general_pkt_ids::macid
 * Please Place Description here.
 * @var mac_ax_general_pkt_ids::probersp
 * Please Place Description here.
 * @var mac_ax_general_pkt_ids::pspoll
 * Please Place Description here.
 * @var mac_ax_general_pkt_ids::nulldata
 * Please Place Description here.
 * @var mac_ax_general_pkt_ids::qosnull
 * Please Place Description here.
 * @var mac_ax_general_pkt_ids::cts2self
 * Please Place Description here.
 * @var mac_ax_general_pkt_ids::probereq
 * offloaded probe request pkt id
 * @var mac_ax_general_pkt_ids::apcsa
 * offloaded CSA frame id
 */
struct mac_ax_general_pkt_ids {
	u8 macid;
	u8 probersp;
	u8 pspoll;
	u8 nulldata;
	u8 qosnull;
	u8 cts2self;
	u8 probereq;
	u8 apcsa;
};

/**
 * @struct mac_ax_cmd_ofld_info
 * @brief mac_ax_cmd_ofld_info
 *
 * @var mac_ax_cmd_ofld_info::buf
 * Please Place Description here.
 * @var mac_ax_cmd_ofld_info::end_ptr
 * Please Place Description here.
 * @var mac_ax_cmd_ofld_info::buf_size
 * Please Place Description here.
 * @var mac_ax_cmd_ofld_info::avl_buf_size
 * Please Place Description here.
 * @var mac_ax_cmd_ofld_info::used_size
 * Please Place Description here.
 * @var mac_ax_cmd_ofld_info::cmd_num
 * Please Place Description here.
 * @var mac_ax_cmd_ofld_info::last_ver
 * Please Place Description here.
 */
struct mac_ax_cmd_ofld_info {
	u8 *buf;
	u8 *buf_wptr;
	u8 *last_wptr;
	u16 buf_size;
	u16 avl_buf_size;
	u16 used_size;
	u8 cmd_num;
	u8 last_ver;
	u8 result;
	u32 accu_delay;
	mac_ax_mutex cmd_ofld_lock;
};

/**
 * @struct mac_ax_tx_duty_ofld_info
 * @brief mac_ax_tx_duty_ofld_info
 *
 * @var mac_ax_tx_duty_ofld_info::timer_err
 * Please Place Description here.
 */
struct mac_ax_tx_duty_ofld_info {
	u8 timer_err;
};

/*--------------------Define OutSrc related ----------------------------------*/

/**
 * @struct mac_ax_la_cfg
 * @brief mac_ax_la_cfg
 *
 * @var mac_ax_la_cfg::la_func_en
 * Please Place Description here.
 * @var mac_ax_la_cfg::la_restart_en
 * Please Place Description here.
 * @var mac_ax_la_cfg::la_timeout_en
 * Please Place Description here.
 * @var mac_ax_la_cfg::la_timeout_val
 * Please Place Description here.
 * @var mac_ax_la_cfg::la_data_loss_imr
 * Please Place Description here.
 * @var mac_ax_la_cfg::la_tgr_tu_sel
 * Please Place Description here.
 * @var mac_ax_la_cfg::la_tgr_time_val
 * Please Place Description here.
 * @var mac_ax_la_cfg::rsvd
 * Please Place Description here.
 */
struct mac_ax_la_cfg {
	u32 la_func_en:1;
	u32 la_restart_en:1;
	u32 la_timeout_en:1;
	/* 2'h0: 1s, 2'h1: 2s, 2'h2: 4s, 2'h3: 8s */
	u32 la_timeout_val:2;
	/*Error flag mask bit for LA data loss due to pktbuffer busy */
	u32 la_data_loss_imr:1;
	/* TU (time unit) = 2^ B_AX_LA_TRIG_TU_SEL */
	u32 la_tgr_tu_sel:4;
	/* 6'h0: No delay, 6'h1: 1 TU, 6'h2: 2TU, ??*/
	u32 la_tgr_time_val:7;
	u32 rsvd:15;
};

/**
 * @struct mac_ax_la_status
 * @brief mac_ax_la_status
 *
 * @var mac_ax_la_status::la_buf_wptr
 * Please Place Description here.
 * @var mac_ax_la_status::la_buf_rndup_ind
 * Please Place Description here.
 * @var mac_ax_la_status::la_sw_fsmst
 * Please Place Description here.
 * @var mac_ax_la_status::la_data_loss
 * Please Place Description here.
 */
struct mac_ax_la_status {
	/* LA data dump finish address = (la_buf_wptr -1) */
	u16 la_buf_wptr;
	/*1: round up, 0: No round up */
	u8 la_buf_rndup_ind:1;
	/*3'h0: LA idle ; 3'h1: LA start; 3'h2: LA finish stop;*/
	/*3'h3:LA finish timeout; 3'h4: LA re-start*/
	u8 la_sw_fsmst:3;
	/* LA data loss due to pktbuffer busy */
	u8 la_data_loss:1;
};

/**
 * @struct mac_ax_la_buf_param
 * @brief mac_ax_la_buf_param
 *
 * @var mac_ax_la_buf_param::start_addr
 * Please Place Description here.
 * @var mac_ax_la_buf_param::end_addr
 * Please Place Description here.
 * @var mac_ax_la_buf_param::la_buf_sel
 * Please Place Description here.
 */
struct mac_ax_la_buf_param {
	u32 start_addr;
	u32 end_addr;
	u8 la_buf_sel; /*0: 64KB; 1: 128KB; 2: 192KB; 3: 256KB; 4: 320KB*/
};

/* need to modify for style later */
#if !MAC_AX_PCIE_SUPPORT
struct tx_base_desc {
	u8 *vir_addr;
	u32 phy_addr_l;
	u32 phy_addr_h;
	u8 cache;
	u32 buf_len;
	u16 host_idx;
	u16 avail_num;
};

struct rx_base_desc {
	u8 *vir_addr;
	u32 phy_addr_l;
	u32 phy_addr_h;
	u8 cache;
	u32 buf_len;
	u16 host_idx;
	u16 avail_num;
};
#endif

/*--------------------Define TRX PKT INFO/RPT---------------------------------*/

/**
 * @struct mac_ax_pkt_data
 * @brief mac_ax_pkt_data
 *
 * @var mac_ax_pkt_data::wifi_seq
 * Please Place Description here.
 * @var mac_ax_pkt_data::hw_ssn_sel
 * Please Place Description here.
 * @var mac_ax_pkt_data::hw_seq_mode
 * Please Place Description here.
 * @var mac_ax_pkt_data::chk_en
 * Please Place Description here.
 * @var mac_ax_pkt_data::hw_amsdu
 * Please Place Description here.
 * @var mac_ax_pkt_data::shcut_camid
 * Please Place Description here.
 * @var mac_ax_pkt_data::headerwllc_len
 * Please Place Description here.
 * @var mac_ax_pkt_data::smh_en
 * Please Place Description here.
 * @var mac_ax_pkt_data::wd_page
 * Please Place Description here.
 * @var mac_ax_pkt_data::wp_offset
 * Please Place Description here.
 * @var mac_ax_pkt_data::wdinfo_en
 * Please Place Description here.
 * @var mac_ax_pkt_data::hw_aes_iv
 * Please Place Description here.
 * @var mac_ax_pkt_data::hdr_len
 * Please Place Description here.
 * @var mac_ax_pkt_data::ch
 * Please Place Description here.
 * @var mac_ax_pkt_data::macid
 * Please Place Description here.
 * @var mac_ax_pkt_data::agg_en
 * Please Place Description here.
 * @var mac_ax_pkt_data::bk
 * Please Place Description here.
 * @var mac_ax_pkt_data::max_agg_num
 * Please Place Description here.
 * @var mac_ax_pkt_data::bmc
 * Please Place Description here.
 * @var mac_ax_pkt_data::lifetime_sel
 * Please Place Description here.
 * @var mac_ax_pkt_data::ampdu_density
 * Please Place Description here.
 * @var mac_ax_pkt_data::userate
 * Please Place Description here.
 * @var mac_ax_pkt_data::data_rate
 * Please Place Description here.
 * @var mac_ax_pkt_data::data_bw
 * Please Place Description here.
 * @var mac_ax_pkt_data::er_bw
 * Please Place Description here.
 * @var mac_ax_pkt_data::data_gi_ltf
 * Please Place Description here.
 * @var mac_ax_pkt_data::data_er
 * Please Place Description here.
 * @var mac_ax_pkt_data::data_dcm
 * Please Place Description here.
 * @var mac_ax_pkt_data::data_stbc
 * Please Place Description here.
 * @var mac_ax_pkt_data::data_ldpc
 * Please Place Description here.
 * @var mac_ax_pkt_data::hw_sec_en
 * Please Place Description here.
 * @var mac_ax_pkt_data::sec_cam_idx
 * Please Place Description here.
 * @var mac_ax_pkt_data::sec_type
 * Please Place Description here.
 * @var mac_ax_pkt_data::dis_data_fb
 * Please Place Description here.
 * @var mac_ax_pkt_data::dis_rts_fb
 * Please Place Description here.
 * @var mac_ax_pkt_data::tid
 * Please Place Description here.
 * @var mac_ax_pkt_data::rts_en
 * Please Place Description here.
 * @var mac_ax_pkt_data::cts2self
 * Please Place Description here.
 * @var mac_ax_pkt_data::cca_rts
 * Please Place Description here.
 * @var mac_ax_pkt_data::hw_rts_en
 * Please Place Description here.
 * @var mac_ax_pkt_data::ndpa
 * Please Place Description here.
 * @var mac_ax_pkt_data::snd_pkt_sel
 * Please Place Description here.
 * @var mac_ax_pkt_data::sifs_tx
 * Please Place Description here.
 * @var mac_ax_pkt_data::tx_cnt_lmt_sel
 * Please Place Description here.
 * @var mac_ax_pkt_data::tx_cnt_lmt
 * Please Place Description here.
 * @var mac_ax_pkt_data::ndpa_dur
 * Please Place Description here.
 * @var mac_ax_pkt_data::nav_use_hdr
 * Please Place Description here.
 * @var mac_ax_pkt_data::multiport_id
 * Please Place Description here.
 * @var mac_ax_pkt_data::mbssid
 * Please Place Description here.
 * @var mac_ax_pkt_data::null_0
 * Please Place Description here.
 * @var mac_ax_pkt_data::null_1
 * Please Place Description here.
 * @var mac_ax_pkt_data::tri_frame
 * Please Place Description here.
 * @var mac_ax_pkt_data::ack_ch_info
 * Please Place Description here.
 * @var mac_ax_pkt_data::pkt_offset
 * Please Place Description here.
 * @var mac_ax_pkt_data::a_ctrl_uph
 * Please Place Description here.
 * @var mac_ax_pkt_data::a_ctrl_bsr
 * Please Place Description here.
 * @var mac_ax_pkt_data::a_ctrl_cas
 * Please Place Description here.
 * @var mac_ax_pkt_data::rtt
 * Please Place Description here.
 * @var mac_ax_pkt_data::ht_data_snd
 * Please Place Description here.
 * @var mac_ax_pkt_data::no_ack
 * Please Place Description here.
 * @var mac_ax_pkt_data::sw_define
 * Please Place Description here.
 */
struct mac_ax_pkt_data {
	u16 wifi_seq;
	u8 hw_ssn_sel;
	u8 hw_seq_mode;
	u8 chk_en;
	u8 hw_amsdu;
	u8 shcut_camid;
	u8 headerwllc_len;
	u8 smh_en;
	u8 wd_page;
	u8 wp_offset;
	u8 wdinfo_en;
	u8 hw_aes_iv;
	u8 hdr_len;
	u8 ch;
	u8 macid;
	u8 wmm;
	u8 band;
	u8 agg_en;
	u8 bk;
	u8 max_agg_num;
	u8 bmc;
	u8 lifetime_sel;
	u8 ampdu_density;
	u8 userate;
	u16 data_rate;
	u8 data_bw;
	u8 er_bw;
	u8 data_gi_ltf;
	u8 data_er;
	u8 data_dcm;
	u8 data_stbc;
	u8 data_ldpc;
	u8 hw_sec_en;
	u8 sec_cam_idx;
	u8 sec_type;
	u8 dis_data_fb;
	u8 dis_rts_fb;
	u8 tid;
	u8 rts_en;
	u8 cts2self;
	u8 cca_rts;
	u8 hw_rts_en;
	u8 ndpa;
	u8 snd_pkt_sel;
	u8 sifs_tx;
	u8 tx_cnt_lmt_sel;
	u8 tx_cnt_lmt;
	u16 ndpa_dur;
	u8 nav_use_hdr;
	u8 multiport_id;
	u8 mbssid;
	u8 null_0;
	u8 null_1;
	u8 tri_frame;
	u8 ack_ch_info;
	u8 pkt_offset;
	u8 a_ctrl_uph;
	u8 a_ctrl_bsr;
	u8 a_ctrl_cas;
	u8 rtt;
	u8 ht_data_snd;
	u8 no_ack;
	u8 sw_define;
	u8 addr_info_num;
	u8 reuse_start_num;
	u8 reuse_size;
	u8 reuse_num;
	u8 hw_sec_iv;
	u8 sw_sec_iv;
	u8 sec_keyid;
	u8 rls_to_cpuio;
	u8 force_key_en;
	u8 upd_wlan_hdr;
	u16 data_rty_lowest_rate;
	u8 spe_rpt;
};

/**
 * @struct mac_ax_pkt_mgnt
 * @brief mac_ax_pkt_mgnt
 *
 * @var mac_ax_pkt_mgnt::wifi_seq
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::hw_ssn_sel
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::hw_seq_mode
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::chk_en
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::hw_amsdu
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::shcut_camid
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::headerwllc_len
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::smh_en
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::wd_page
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::wp_offset
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::wdinfo_en
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::hw_aes_iv
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::hdr_len
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::rsvd0
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::macid
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::rsvd1
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::bk
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::max_agg_num
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::bmc
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::lifetime_sel
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::ampdu_density
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::userate
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::data_rate
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::data_bw
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::er_bw
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::data_gi_ltf
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::data_er
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::data_dcm
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::data_stbc
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::data_ldpc
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::hw_sec_en
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::sec_cam_idx
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::sec_type
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::dis_data_fb
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::dis_rts_fb
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::tid
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::rts_en
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::cts2self
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::cca_rts
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::hw_rts_en
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::ndpa
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::snd_pkt_sel
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::sifs_tx
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::tx_cnt_lmt_sel
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::tx_cnt_lmt
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::ndpa_dur
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::nav_use_hdr
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::multiport_id
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::mbssid
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::null_0
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::null_1
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::tri_frame
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::ack_ch_info
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::pkt_offset
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::a_ctrl_bsr
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::rtt
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::ht_data_snd
 * Please Place Description here.
 * @var mac_ax_pkt_mgnt::no_ack
 * Please Place Description here.
 */
struct mac_ax_pkt_mgnt {
	u16 wifi_seq;
	u8 hw_ssn_sel;
	u8 hw_seq_mode;
	u8 chk_en;
	u8 hw_amsdu;
	u8 shcut_camid;
	u8 headerwllc_len;
	u8 smh_en;
	u8 wd_page;
	u8 wp_offset;
	u8 wdinfo_en;
	u8 hw_aes_iv;
	u8 hdr_len;
	u8 ch;
	u8 macid;
	u8 wmm;
	u8 band;
	u8 agg_en;
	u8 bk;
	u8 max_agg_num;
	u8 bmc;
	u8 lifetime_sel;
	u8 ampdu_density;
	u8 userate;
	u16 data_rate;
	u8 data_bw;
	u8 er_bw;
	u8 data_gi_ltf;
	u8 data_er;
	u8 data_dcm;
	u8 data_stbc;
	u8 data_ldpc;
	u8 hw_sec_en;
	u8 sec_cam_idx;
	u8 sec_type;
	u8 dis_data_fb;
	u8 dis_rts_fb;
	u8 tid;
	u8 rts_en;
	u8 cts2self;
	u8 cca_rts;
	u8 hw_rts_en;
	u8 ndpa;
	u8 snd_pkt_sel;
	u8 sifs_tx;
	u8 tx_cnt_lmt_sel;
	u8 tx_cnt_lmt;
	u16 ndpa_dur;
	u8 nav_use_hdr;
	u8 multiport_id;
	u8 mbssid;
	u8 null_0;
	u8 null_1;
	u8 tri_frame;
	u8 ack_ch_info;
	u8 pkt_offset;
	u8 a_ctrl_uph;
	u8 a_ctrl_bsr;
	u8 a_ctrl_cas;
	u8 rtt;
	u8 ht_data_snd;
	u8 no_ack;
	u8 sw_define;
	u8 addr_info_num;
	u8 reuse_start_num;
	u8 reuse_size;
	u8 reuse_num;
	u8 hw_sec_iv;
	u8 sw_sec_iv;
	u8 sec_keyid;
	u8 rls_to_cpuio;
	u8 force_key_en;
	u8 upd_wlan_hdr;
	u16 data_rty_lowest_rate;
	u8 spe_rpt;
};

/**
 * @struct mac_ax_rpkt_data
 * @brief mac_ax_rpkt_data
 *
 * @var mac_ax_rpkt_data::crc_err
 * Please Place Description here.
 * @var mac_ax_rpkt_data::icv_err
 * Please Place Description here.
 */
struct mac_ax_rpkt_data {
	u8 crc_err;
	u8 icv_err;
};

/**
 * @struct mac_ax_txpkt_info
 * @brief mac_ax_txpkt_info
 *
 * @var mac_ax_txpkt_info::type
 * Please Place Description here.
 * @var mac_ax_txpkt_info::pktsize
 * Please Place Description here.
 * @var mac_ax_txpkt_info::data
 * Please Place Description here.
 * @var mac_ax_txpkt_info::mgnt
 * Please Place Description here.
 * @var mac_ax_txpkt_info::u
 * Please Place Description here.
 */
struct mac_ax_txpkt_info {
	enum mac_ax_pkt_t type;
	u32 pktsize;
	union {
		struct mac_ax_pkt_data data;
		struct mac_ax_pkt_mgnt mgnt;
	} u;
};

/**
 * @struct mac_ax_bcn_cnt
 * @brief mac_ax_bcn_cnt
 *
 * @var mac_ax_bcn_cnt::port
 * Please Place Description here.
 * @var mac_ax_bcn_cnt::mbssid
 * Please Place Description here.
 * @var mac_ax_bcn_cnt::ok_cnt
 * Please Place Description here.
 * @var mac_ax_bcn_cnt::fail_cnt
 * Please Place Description here.
 */
struct mac_ax_bcn_cnt {
	u8 port;
	u8 mbssid;
	u8 band;
	u8 ok_cnt;
	u8 cca_cnt;
	u8 edcca_cnt;
	u8 nav_cnt;
	u8 txon_cnt;
	u8 mac_cnt;
	u8 others_cnt;
	u8 lock_cnt;
	u8 cmp_cnt;
	u8 invalid_cnt;
	u8 srchend_cnt;
};

/**
 * @struct mac_ax_refill_info
 * @brief mac_ax_refill_info
 *
 * @var mac_ax_refill_info::pkt
 * Please Place Description here.
 * @var mac_ax_refill_info::agg_num
 * Please Place Description here.
 * @var mac_ax_refill_info::packet_offset
 * Please Place Description here.
 */
struct mac_ax_refill_info {
	u8 *pkt;
	u32 agg_num;
	u8 packet_offset;
};

/**
 * @struct mac_ax_rpkt_ppdu
 * @brief mac_ax_rpkt_ppdu
 *
 * @var mac_ax_rpkt_ppdu::mac_info
 * Please Place Description here.
 */
struct mac_ax_rpkt_ppdu {
	u8 mac_info;
};

/**
 * @struct mac_ax_mac_tx_mode_sel
 * @brief mac_ax_mac_tx_mode_sel
 *
 * @var mac_ax_mac_tx_mode_sel::txop_rot_wmm0_en
 * Please Place Description here.
 * @var mac_ax_mac_tx_mode_sel::txop_rot_wmm1_en
 * Please Place Description here.
 * @var mac_ax_mac_tx_mode_sel::txop_rot_wmm2_en
 * Please Place Description here.
 * @var mac_ax_mac_tx_mode_sel::txop_rot_wmm3_en
 * Please Place Description here.
 */
struct mac_ax_mac_tx_mode_sel {
	u8 txop_rot_wmm0_en;
	u8 txop_rot_wmm1_en;
	u8 txop_rot_wmm2_en;
	u8 txop_rot_wmm3_en;

	u8 sw_mode_band0_en; /* shall remove when v0_22 release, Rick */
};

/**
 * @struct mac_ax_rxpkt_info
 * @brief mac_ax_rxpkt_info
 *
 * @var mac_ax_rxpkt_info::type
 * Please Place Description here.
 * @var mac_ax_rxpkt_info::rxdlen
 * Please Place Description here.
 * @var mac_ax_rxpkt_info::drvsize
 * Please Place Description here.
 * @var mac_ax_rxpkt_info::shift
 * Please Place Description here.
 * @var mac_ax_rxpkt_info::pktsize
 * Please Place Description here.
 * @var mac_ax_rxpkt_info::data
 * Please Place Description here.
 * @var mac_ax_rxpkt_info::ppdu
 * Please Place Description here.
 * @var mac_ax_rxpkt_info::u
 * Please Place Description here.
 */
struct mac_ax_rxpkt_info {
	enum mac_ax_pkt_t type;
	u16 rxdlen;
	u8 drvsize;
	u8 shift;
	u32 pktsize;
	union {
		struct mac_ax_rpkt_data data;
		struct mac_ax_rpkt_ppdu ppdu;
	} u;
};

/**
 * @struct mac_ax_pm_cam_ctrl_t
 * @brief mac_ax_pm_cam_ctrl_t
 *
 * @var mac_ax_pm_cam_ctrl_t::pld_mask0
 * Please Place Description here.
 * @var mac_ax_pm_cam_ctrl_t::pld_mask1
 * Please Place Description here.
 * @var mac_ax_pm_cam_ctrl_t::pld_mask2
 * Please Place Description here.
 * @var mac_ax_pm_cam_ctrl_t::pld_mask3
 * Please Place Description here.
 * @var mac_ax_pm_cam_ctrl_t::entry_index
 * Please Place Description here.
 * @var mac_ax_pm_cam_ctrl_t::valid
 * Please Place Description here.
 * @var mac_ax_pm_cam_ctrl_t::type
 * Please Place Description here.
 * @var mac_ax_pm_cam_ctrl_t::subtype
 * Please Place Description here.
 * @var mac_ax_pm_cam_ctrl_t::skip_mac_iv_hdr
 * Please Place Description here.
 * @var mac_ax_pm_cam_ctrl_t::target_ind
 * Please Place Description here.
 * @var mac_ax_pm_cam_ctrl_t::crc16
 * Please Place Description here.
 */
struct mac_ax_pm_cam_ctrl_t {
	u32 pld_mask0;
	u32 pld_mask1;
	u32 pld_mask2;
	u32 pld_mask3;
	u8 entry_index;
	u8 valid;
	u8 type;
	u8 subtype;
	u8 skip_mac_iv_hdr;
	u8 target_ind;
	u16 crc16;
};

/**
 * @struct mac_ax_af_ud_ctrl_t
 * @brief mac_ax_af_ud_ctrl_t
 *
 * @var mac_ax_af_ud_ctrl_t::index
 * Please Place Description here.
 * @var mac_ax_af_ud_ctrl_t::fwd_tg
 * Please Place Description here.
 * @var mac_ax_af_ud_ctrl_t::category
 * Please Place Description here.
 * @var mac_ax_af_ud_ctrl_t::action_field
 * Please Place Description here.
 */
struct mac_ax_af_ud_ctrl_t {
	u8 index;
	u8 fwd_tg;
	u8 category;
	u8 action_field;
};

/**
 * @struct mac_ax_rx_fwd_ctrl_t
 * @brief mac_ax_rx_fwd_ctrl_t
 *
 * @var mac_ax_rx_fwd_ctrl_t::pm_cam_ctrl
 * Please Place Description here.
 * @var mac_ax_rx_fwd_ctrl_t::af_ud_ctrl
 * Please Place Description here.
 * @var mac_ax_rx_fwd_ctrl_t::type
 * Please Place Description here.
 * @var mac_ax_rx_fwd_ctrl_t::frame
 * Please Place Description here.
 * @var mac_ax_rx_fwd_ctrl_t::fwd_tg
 * Please Place Description here.
 */
struct mac_ax_rx_fwd_ctrl_t {
	struct mac_ax_pm_cam_ctrl_t pm_cam_ctrl;
	struct mac_ax_af_ud_ctrl_t af_ud_ctrl;
	u8 type;
	u8 frame;
	u8 fwd_tg;
};

/**
 * @struct mac_ax_rx_fltr_ctrl_t
 * @brief mac_ax_rx_fltr_ctrl_t
 *
 * @var mac_ax_rx_fltr_ctrl_t::sniffer_mode
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::acpt_a1_match_pkt
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::acpt_bc_pkt
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::acpt_mc_pkt
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::uc_pkt_chk_cam_match
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::bc_pkt_chk_cam_match
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::mc_pkt_white_lst_mode
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::bcn_chk_en
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::bcn_chk_rule
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::acpt_pwr_mngt_pkt
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::acpt_crc32_err_pkt
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::acpt_unsupport_pkt
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::acpt_mac_hdr_content_err_pkt
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::acpt_ftm_req_pkt
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::pkt_len_fltr
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::unsp_pkt_target
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::uid_fltr
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::cck_crc_chk_enable
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::cck_sig_chk_enable
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::lsig_parity_chk_enable
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::siga_crc_chk_enable
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::vht_su_sigb_crc_chk_enable
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::vht_mu_sigb_crc_chk_enable
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::he_sigb_crc_chk_enable
 * Please Place Description here.
 * @var mac_ax_rx_fltr_ctrl_t::min_len_chk_disable
 * Please Place Description here.
 */
struct mac_ax_rx_fltr_ctrl_t {
	// mac fltr
	u8 sniffer_mode:1;
	u8 acpt_a1_match_pkt:1;
	u8 acpt_bc_pkt:1;
	u8 acpt_mc_pkt:1;
	u8 uc_pkt_chk_cam_match:1;
	u8 bc_pkt_chk_cam_match:1;
	u8 mc_pkt_white_lst_mode:1;
	u8 bcn_chk_en:1;

	u8 bcn_chk_rule:2;
	u8 acpt_pwr_mngt_pkt:1;
	u8 acpt_crc32_err_pkt:1;
	u8 acpt_unsupport_pkt:1;
	u8 acpt_mac_hdr_content_err_pkt:1;
	u8 acpt_ftm_req_pkt:1;
	u8 rsvd0:1;
	u8 pkt_len_fltr:6;
	u8 unsp_pkt_target:2;
	u8 uid_fltr:4;
	u8 rsvd1:4;
	// plcp fltr
	u8 cck_crc_chk_enable:1;
	u8 cck_sig_chk_enable:1;
	u8 lsig_parity_chk_enable:1;
	u8 siga_crc_chk_enable:1;
	u8 vht_su_sigb_crc_chk_enable:1;
	u8 vht_mu_sigb_crc_chk_enable:1;
	u8 he_sigb_crc_chk_enable:1;
	u8 min_len_chk_disable:1;
};

struct mac_ax_rx_fltr_elem {
	u16 subtype_mask;
	enum mac_ax_fwd_target target_arr[16];
};

/**
 * @struct mac_ax_addrcam_ctrl_t
 * @brief mac_ax_addrcam_ctrl_t
 */
struct mac_ax_addrcam_ctrl_t {
	u8 addrcam_en:1;
	u8 srch_per_mpdu:1;
	u8 a2_bit0_cmp_en:1;
	u8 rsvd1:5;
	u8 clr_all_content:1;
	u8 rsvd2:3;
	u8 srch_time_lmt:4;
	u8 srch_range_lmt;
	u8 rsvd3;
};

/**
 * @struct mac_ax_addrcam_dis_ctrl_t
 * @brief mac_ax_addrcam_dis_ctrl_t
 *
 */
struct mac_ax_addrcam_dis_ctrl_t {
	u8 def_hit_idx;
	u8 def_hit_result : 1;
	u8 def_a1_hit_result : 1;
	u8 def_a2_hit_result : 1;
	u8 def_a3_hit_result : 1;
	u8 def_port : 3;
	u8 rsvd : 1;
	u8 def_sec_idx;
	u8 def_macid;
};

/**
 * @struct mac_ax_dfs_rpt
 * @brief mac_ax_dfs_rpt
 *
 * @var mac_ax_dfs_rpt::dfs_ptr
 * Please Place Description here.
 * @var mac_ax_dfs_rpt::drop_num
 * Please Place Description here.
 * @var mac_ax_dfs_rpt::max_cont_drop
 * Please Place Description here.
 * @var mac_ax_dfs_rpt::total_drop
 * Please Place Description here.
 * @var mac_ax_dfs_rpt::dfs_num
 * Please Place Description here.
 */
struct mac_ax_dfs_rpt {
	u8 *dfs_ptr;
	u16 drop_num;
	u16 max_cont_drop;
	u16 total_drop;
	u16 dfs_num;
};

/**
 * @struct mac_ax_ppdu_usr
 * @brief mac_ax_ppdu_usr
 *
 * @var mac_ax_ppdu_usr::vld
 * Please Place Description here.
 * @var mac_ax_ppdu_usr::has_data
 * Please Place Description here.
 * @var mac_ax_ppdu_usr::has_ctrl
 * Please Place Description here.
 * @var mac_ax_ppdu_usr::has_mgnt
 * Please Place Description here.
 * @var mac_ax_ppdu_usr::has_bcn
 * Please Place Description here.
 * @var mac_ax_ppdu_usr::macid
 * Please Place Description here.
 */
struct mac_ax_ppdu_usr {
	u8 vld:1;
	u8 has_data:1;
	u8 has_ctrl:1;
	u8 has_mgnt:1;
	u8 has_bcn:1;
	u8 macid;
};

/**
 * @struct mac_ax_ppdu_stat
 * @brief mac_ax_ppdu_stat
 *
 * @var mac_ax_ppdu_stat::band
 * Please Place Description here.
 * @var mac_ax_ppdu_stat::bmp_append_info
 * Please Place Description here.
 * @var mac_ax_ppdu_stat::bmp_filter
 * Please Place Description here.
 * @var mac_ax_ppdu_stat::dup2fw_en
 * Please Place Description here.
 * @var mac_ax_ppdu_stat::dup2fw_len
 * Please Place Description here.
 */
struct mac_ax_ppdu_stat {
	u8 band;
#define MAC_AX_PPDU_MAC_INFO BIT(1)
#define MAC_AX_PPDU_PLCP BIT(3)
#define MAC_AX_PPDU_RX_CNT BIT(2)
	u8 bmp_append_info;
#define MAC_AX_PPDU_HAS_A1M BIT(4)
#define MAC_AX_PPDU_HAS_CRC_OK BIT(5)
#define MAC_AX_PPDU_HAS_DMA_OK BIT(6)
	u8 bmp_filter;
	u8 dup2fw_en;
	u8 dup2fw_len;
};

/**
 * @struct mac_ax_ch_info
 * @brief mac_ax_ch_info
 *
 * @var mac_ax_ch_info::trigger
 * Please Place Description here.
 * @var mac_ax_ch_info::macid
 * Please Place Description here.
 * @var mac_ax_ch_info::bmp_filter
 * Please Place Description here.
 * @var mac_ax_ch_info::dis_to
 * Please Place Description here.
 * @var mac_ax_ch_info::seg_size
 * Please Place Description here.
 */
struct mac_ax_ch_info {
#define MAC_AX_CH_INFO_MACID 0
#define MAC_AX_CH_INFO_NDP 1
#define MAC_AX_CH_INFO_SND 2
#define MAC_AX_CH_INFO_ACK 3
	u8 trigger;
	u8 macid;
#define MAC_AX_CH_INFO_CRC_FAIL BIT(0)
#define MAC_AX_CH_INFO_DATA_FRM BIT(1)
#define MAC_AX_CH_INFO_CTRL_FRM BIT(2)
#define MAC_AX_CH_INFO_MGNT_FRM BIT(3)
	u8 bmp_filter;
	u8 dis_to;
#define MAC_AX_CH_IFNO_SEG_128 0
#define MAC_AX_CH_IFNO_SEG_256 1
#define MAC_AX_CH_IFNO_SEG_512 2
#define MAC_AX_CH_IFNO_SEG_1024 3
	u8 seg_size;
};

/**
 * @struct mac_ax_dfs
 * @brief mac_ax_dfs
 *
 * @var mac_ax_dfs::num_th
 * Please Place Description here.
 * @var mac_ax_dfs::en_timeout
 * Please Place Description here.
 */
struct mac_ax_dfs {
#define MAC_AX_DFS_TH_29 0
#define MAC_AX_DFS_TH_61 1
#define MAC_AX_DFS_TH_93 2
#define MAC_AX_DFS_TH_125 3
	u8 num_th;
	u8 en_timeout;
#define MAC_AX_DFS_TO_20MS 1
#define MAC_AX_DFS_TO_40MS 2
#define MAC_AX_DFS_TO_80MS 3
	u8 dfs_to;
};

/**
 * @struct mac_ax_ppdu_rpt
 * @brief mac_ax_ppdu_rpt
 *
 * @var mac_ax_ppdu_rpt::rx_cnt_ptr
 * Please Place Description here.
 * @var mac_ax_ppdu_rpt::plcp_ptr
 * Please Place Description here.
 * @var mac_ax_ppdu_rpt::phy_st_ptr
 * Please Place Description here.
 * @var mac_ax_ppdu_rpt::phy_st_size
 * Please Place Description here.
 * @var mac_ax_ppdu_rpt::rx_cnt_size
 * Please Place Description here.
 * @var mac_ax_ppdu_rpt::lsig_len
 * Please Place Description here.
 * @var mac_ax_ppdu_rpt::service
 * Please Place Description here.
 * @var mac_ax_ppdu_rpt::usr_num
 * Please Place Description here.
 * @var mac_ax_ppdu_rpt::fw_def
 * Please Place Description here.
 * @var mac_ax_ppdu_rpt::is_to_self
 * Please Place Description here.
 * @var mac_ax_ppdu_rpt::plcp_size
 * Please Place Description here.
 * @var mac_ax_ppdu_rpt::usr
 * Please Place Description here.
 */
struct mac_ax_ppdu_rpt {
#define MAC_AX_PPDU_MAX_USR 8
	u8 *rx_cnt_ptr;
	u8 *plcp_ptr;
	u8 *phy_st_ptr;
	u32 phy_st_size;
	u32 rx_cnt_size;
	u16 lsig_len;
	u16 service;
	u8 usr_num;
	u8 fw_def;
	u8 is_to_self;
	u8 plcp_size;
	struct mac_ax_ppdu_usr usr[MAC_AX_PPDU_MAX_USR];
};

/**
 * @struct mac_ax_phy_rpt_cfg
 * @brief mac_ax_phy_rpt_cfg
 *
 * @var mac_ax_phy_rpt_cfg::type
 * Please Place Description here.
 * @var mac_ax_phy_rpt_cfg::en
 * Please Place Description here.
 * @var mac_ax_phy_rpt_cfg::dest
 * Please Place Description here.
 * @var mac_ax_phy_rpt_cfg::ppdu
 * Please Place Description here.
 * @var mac_ax_phy_rpt_cfg::chif
 * Please Place Description here.
 * @var mac_ax_phy_rpt_cfg::dfs
 * Please Place Description here.
 * @var mac_ax_phy_rpt_cfg::u
 * Please Place Description here.
 */
struct mac_ax_phy_rpt_cfg {
	enum mac_ax_phy_rpt type;
	u8 en;
#define MAC_AX_PRPT_DEST_HOST 0
#define MAC_AX_PRPT_DEST_WLCPU 1
	u8 dest;
	union {
		struct mac_ax_ppdu_stat ppdu;
		struct mac_ax_ch_info chif;
		struct mac_ax_dfs dfs;
	} u;
};

/**
 * @struct mac_ax_pkt_drop_info
 * @brief mac_ax_pkt_drop_info
 *
 * @var mac_ax_pkt_drop_info::sel
 * Please Place Description here.
 * @var mac_ax_pkt_drop_info::macid
 * Please Place Description here.
 * @var mac_ax_pkt_drop_info::band
 * Please Place Description here.
 * @var mac_ax_pkt_drop_info::port
 * Please Place Description here.
 * @var mac_ax_pkt_drop_info::mbssid
 * Please Place Description here.
 */
struct mac_ax_pkt_drop_info {
	enum mac_ax_pkt_drop_sel sel;
	u8 macid;
	u8 band;
	u8 port;
	u8 mbssid;
};

/**
 * @struct mac_ax_ch_busy_cnt_ref
 * @brief mac_ax_ch_busy_cnt_ref
 *
 * @var mac_ax_ch_busy_cnt_ref::basic_nav
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ref::intra_nav
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ref::data_on
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ref::edcca_p20
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ref::cca_p20
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ref::cca_s20
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ref::cca_s40
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_ref::cca_s80
 * Please Place Description here.
 */
struct mac_ax_ch_busy_cnt_ref {
	u16 basic_nav:1;
	u16 intra_nav:1;
	u16 data_on:1;
	u16 edcca_p20:1;
	u16 cca_p20:1;
	u16 cca_s20:1;
	u16 cca_s40:1;
	u16 cca_s80:1;
	u16 phy_txon:1;
	u16 rsvd:7;
};

/**
 * @struct mac_ax_tx_queue_empty
 * @brief mac_ax_tx_queue_empty
 *
 * @var mac_ax_tx_queue_empty::macid_txq_empty
 * Please Place Description here.
 * @var mac_ax_tx_queue_empty::band0_mgnt_empty
 * Please Place Description here.
 * @var mac_ax_tx_queue_empty::band1_mgnt_empty
 * Please Place Description here.
 * @var mac_ax_tx_queue_empty::fw_txq_empty
 * Please Place Description here.
 * @var mac_ax_tx_queue_empty::h2c_empty
 * Please Place Description here.
 * @var mac_ax_tx_queue_empty::others_empty
 * Please Place Description here.
 * @var mac_ax_tx_queue_empty::rsvd
 * Please Place Description here.
 */
struct mac_ax_tx_queue_empty {
#define WDE_QEMPTY_ACQ_NUM_MAX 16 /* shall be the max num of all chip */
	u8 macid_txq_empty[WDE_QEMPTY_ACQ_NUM_MAX];
	u8 band0_mgnt_empty:1;
	u8 band1_mgnt_empty:1;
	u8 fw_txq_empty:1;
	u8 h2c_empty:1;
	u8 others_empty:1;
	u8 rsvd:3;
};

/**
 * @struct mac_ax_rx_queue_empty
 * @brief mac_ax_rx_queue_empty
 *
 * @var mac_ax_rx_queue_empty::band0_rxq_empty
 * Please Place Description here.
 * @var mac_ax_rx_queue_empty::band1_rxq_empty
 * Please Place Description here.
 * @var mac_ax_rx_queue_empty::c2h_empty
 * Please Place Description here.
 * @var mac_ax_rx_queue_empty::others_empty
 * Please Place Description here.
 * @var mac_ax_rx_queue_empty::rsvd
 * Please Place Description here.
 */
struct mac_ax_rx_queue_empty {
	u8 band0_rxq_empty:1;
	u8 band1_rxq_empty:1;
	u8 c2h_empty:1;
	u8 others_empty:1;
	u8 rsvd:4;
};

/**
 * @struct mac_ax_dbcc_info
 * @brief mac_ax_dbcc_info
 *
 * @var mac_ax_dbcc_info::ppdu_rpt_bkp
 * Please Place Description here.
 * @var mac_ax_dbcc_info::chinfo_bkp
 * Please Place Description here.
 * @var mac_ax_dbcc_info::dbcc_role_cnt
 * Please Place Description here.
 * @var mac_ax_dbcc_info::dbcc_wmm_type
 * Please Place Description here.
 * @var mac_ax_dbcc_info::dbcc_wmm_bp
 * Please Place Description here.
 * @var mac_ax_dbcc_info::bkp_flag
 * Please Place Description here.
 * @var mac_ax_dbcc_info::dbcc_wmm_list
 * Please Place Description here.
 * @var mac_ax_dbcc_info::notify_fw_flag
 * Please Place Description here.
 */
struct mac_ax_dbcc_info {
	struct mac_ax_phy_rpt_cfg ppdu_rpt_bkp[MAC_AX_BAND_NUM];
	struct mac_ax_phy_rpt_cfg chinfo_bkp[MAC_AX_BAND_NUM];
	u8 dbcc_role_cnt[MAC_AX_DBCC_WMM_MAX];
	u8 dbcc_wmm_type[MAC_AX_DBCC_WMM_MAX]; // enum mac_ax_net_type
	u8 dbcc_wmm_bp[MAC_AX_DBCC_WMM_MAX];
	u8 bkp_flag[MAC_AX_BAND_NUM];
	u8 *dbcc_wmm_list;
	u8 notify_fw_flag;
};

/**
 * @struct mac_dbcc_cfg_info
 * @brief mac_dbcc_cfg_info
 *
 * @var mac_dbcc_cfg_info::trx_mode
 * Please Place Description here.
 * @var mac_dbcc_cfg_info::qta_mode
 * Please Place Description here.
 * @var mac_dbcc_cfg_info::dbcc_en
 * Please Place Description here.
 */
struct mac_dbcc_cfg_info {
	enum mac_ax_trx_mode trx_mode;
	enum mac_ax_qta_mode qta_mode;
	u8 dbcc_en;
};

/**
 * @struct sensing_csi_info
 * @brief sensing_csi_info
 *
 * @var sensing_csi_info::state_lock
 * Lock of state
 * @var sensing_csi_info::func_en
 * Function is enabled or not
 */
struct sensing_csi_info{
	u8 func_en;
	u8 start_cmd_send;
	u8 stop_cmd_send;
	mac_ax_mutex state_lock;
};

/*--------------------Define TF2PCMD related struct --------------------------*/

/**
 * @struct mac_ax_rura_report
 * @brief mac_ax_rura_report
 *
 * @var mac_ax_rura_report::rt_tblcol
 * Please Place Description here.
 * @var mac_ax_rura_report::prtl_alloc
 * Please Place Description here.
 * @var mac_ax_rura_report::rate_chg
 * Please Place Description here.
 */
struct mac_ax_rura_report {
	u8 rt_tblcol: 6;
	u8 prtl_alloc: 1;
	u8 rate_chg: 1;
};

//for ul rua output

/**
 * @struct mac_ax_ulru_out_sta_ent
 * @brief mac_ax_ulru_out_sta_ent
 *
 * @var mac_ax_ulru_out_sta_ent::dropping
 * Please Place Description here.
 * @var mac_ax_ulru_out_sta_ent::tgt_rssi
 * Please Place Description here.
 * @var mac_ax_ulru_out_sta_ent::mac_id
 * Please Place Description here.
 * @var mac_ax_ulru_out_sta_ent::ru_pos
 * Please Place Description here.
 * @var mac_ax_ulru_out_sta_ent::coding
 * Please Place Description here.
 * @var mac_ax_ulru_out_sta_ent::vip_flag
 * Please Place Description here.
 * @var mac_ax_ulru_out_sta_ent::rsvd1
 * Please Place Description here.
 * @var mac_ax_ulru_out_sta_ent::bsr_length
 * Please Place Description here.
 * @var mac_ax_ulru_out_sta_ent::rsvd2
 * Please Place Description here.
 * @var mac_ax_ulru_out_sta_ent::rate
 * Please Place Description here.
 * @var mac_ax_ulru_out_sta_ent::rpt
 * Please Place Description here.
 */
struct mac_ax_ulru_out_sta_ent {
	u8 dropping: 1;
	u8 tgt_rssi: 7;
	u8 mac_id;
	u8 ru_pos;
	u8 coding: 1;
	u8 vip_flag: 1;
	u8 rsvd1: 6;
	u16 bsr_length: 15;
	u16 rsvd2: 1;
	struct mac_ax_ru_rate_ent rate;
	struct mac_ax_rura_report rpt;
};

/**
 * @struct mac_ax_ulrua_output
 * @brief mac_ax_ulrua_output
 *
 * @var mac_ax_ulrua_output::ru2su
 * Please Place Description here.
 * @var mac_ax_ulrua_output::ppdu_bw
 * Please Place Description here.
 * @var mac_ax_ulrua_output::gi_ltf
 * Please Place Description here.
 * @var mac_ax_ulrua_output::stbc
 * Please Place Description here.
 * @var mac_ax_ulrua_output::doppler
 * Please Place Description here.
 * @var mac_ax_ulrua_output::n_ltf_and_ma
 * Please Place Description here.
 * @var mac_ax_ulrua_output::sta_num
 * Please Place Description here.
 * @var mac_ax_ulrua_output::rsvd1
 * Please Place Description here.
 * @var mac_ax_ulrua_output::rf_gain_fix
 * Please Place Description here.
 * @var mac_ax_ulrua_output::rf_gain_idx
 * Please Place Description here.
 * @var mac_ax_ulrua_output::tb_t_pe_nom
 * Please Place Description here.
 * @var mac_ax_ulrua_output::rsvd2
 * Please Place Description here.
 * @var mac_ax_ulrua_output::grp_mode
 * Please Place Description here.
 * @var mac_ax_ulrua_output::grp_id
 * Please Place Description here.
 * @var mac_ax_ulrua_output::fix_mode
 * Please Place Description here.
 * @var mac_ax_ulrua_output::rsvd3
 * Please Place Description here.
 * @var mac_ax_ulrua_output::sta
 * Please Place Description here.
 */
struct mac_ax_ulrua_output {
	u8 ru2su: 1;
	u8 ppdu_bw: 2;
	u8 gi_ltf: 3;
	u8 stbc: 1;
	u8 doppler: 1;
	u8 n_ltf_and_ma: 3;
	u8 sta_num: 4;
	u8 rsvd1: 1;
	u16 rf_gain_fix: 1;
	u16 rf_gain_idx: 10;
	u16 tb_t_pe_nom: 2;
	u16 rsvd2: 3;

	u32 grp_mode: 1;
	u32 grp_id: 6;
	u32 fix_mode: 1;
	u32 rsvd3: 24;
	struct mac_ax_ulru_out_sta_ent sta[MAC_AX_MAX_RU_NUM];
};

/**
 * @struct mac_ul_macid_info
 * @brief mac_ul_macid_info
 *
 * @var mac_ul_macid_info::macid
 * Please Place Description here.
 * @var mac_ul_macid_info::pref_AC
 * Please Place Description here.
 * @var mac_ul_macid_info::rsvd
 * Please Place Description here.
 */
struct mac_ul_macid_info {
	u8 macid;
	u8 pref_AC:2;
	u8 rsvd:6;
};

/**
 * @struct mac_ul_mode_cfg
 * @brief mac_ul_mode_cfg
 *
 * @var mac_ul_mode_cfg::mode
 * Please Place Description here.
 * @var mac_ul_mode_cfg::interval
 * Please Place Description here.
 * @var mac_ul_mode_cfg::bsr_thold
 * Please Place Description here.
 * @var mac_ul_mode_cfg::storemode
 * Please Place Description here.
 * @var mac_ul_mode_cfg::rsvd
 * Please Place Description here.
 */
struct mac_ul_mode_cfg {
	u32 mode:2; /* 0: peoridic ; 1: normal ; 2: non_tgr */
	u32 interval:6; /* unit: sec */
	u32 bsr_thold:8;
	u32 storemode:2;
	u32 rsvd:14;
};

/**
 * @struct mac_ax_ul_fixinfo
 * @brief mac_ax_ul_fixinfo
 *
 * @var mac_ax_ul_fixinfo::tbl_hdr
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::cfg
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::ndpa_dur
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::tf_type
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::sig_ta_pkten
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::sig_ta_pktsc
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::murts_flag
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::ndpa
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::snd_pkt_sel
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::gi_ltf
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::data_rate
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::data_er
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::data_bw
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::data_stbc
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::data_ldpc
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::data_dcm
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::apep_len
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::more_tf
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::data_bw_er
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::istwt
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::rsvd0
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::multiport_id
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::mbssid
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::txpwr_mode
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::ulfix_usage
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::twtgrp_stanum_sel
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::store_idx
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::rsvd1
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::sta
 * Please Place Description here.
 * @var mac_ax_ul_fixinfo::ulrua
 * Please Place Description here.
 */
struct mac_ax_ul_fixinfo {
	struct mac_ax_tbl_hdr tbl_hdr;
	struct mac_ul_mode_cfg cfg;

	u32 ndpa_dur:16;
	u32 tf_type:3;
	u32 sig_ta_pkten:1;
	u32 sig_ta_pktsc:4;
	u32 murts_flag:1;
	u32 ndpa:2;
	u32 snd_pkt_sel:2;
	u32 gi_ltf:3;

	u32 data_rate:9;
	u32 data_er:1;
	u32 data_bw:2;
	u32 data_stbc:2;
	u32 data_ldpc:1;
	u32 data_dcm:1;
	u32 apep_len:12;
	u32 more_tf:1;
	u32 data_bw_er:1;
	u32 istwt:1;
	u32 rsvd0:1;

	u32 multiport_id:3;
	u32 mbssid:4;
	u32 txpwr_mode:3;
	u32 ulfix_usage:3;
	u32 twtgrp_stanum_sel:2;
	u32 store_idx:4;
	u32 rsvd1:13;
	struct mac_ul_macid_info sta[4];
	struct mac_ax_ulrua_output ulrua;
};

/**
 * @struct mac_ax_mudecision_para
 * @brief mac_ax_mudecision_para
 *
 * @var mac_ax_mudecision_para::tbl_hdr
 * Please Place Description here.
 * @var mac_ax_mudecision_para::mu_thold
 * Please Place Description here.
 * @var mac_ax_mudecision_para::bypass_thold
 * Please Place Description here.
 * @var mac_ax_mudecision_para::bypass_tp
 * Please Place Description here.
 */
struct mac_ax_mudecision_para {
	struct mac_ax_tbl_hdr tbl_hdr;
	u32 mu_thold:30;
	u32 bypass_thold:1; //macid bypass tx time thold check
	u32 bypass_tp:1; //T1 unit:us
	u32 init_rate: 4;
	u32 retry_th: 3;
	u32 rsvd: 25;
};

/**
 * @struct mac_ax_protect_rsp_field
 * @brief mac_ax_protect_rsp_field
 *
 * @var mac_ax_protect_rsp_field::protect
 * Please Place Description here.
 * @var mac_ax_protect_rsp_field::rsp
 * Please Place Description here.
 */
struct mac_ax_protect_rsp_field {
	u8 protect: 4;
	u8 rsp: 4;
};

/**
 * @struct mac_ax_mu_protect_rsp_type
 * @brief mac_ax_mu_protect_rsp_type
 *
 * @var mac_ax_mu_protect_rsp_type::byte_type
 * Please Place Description here.
 * @var mac_ax_mu_protect_rsp_type::feld_type
 * Please Place Description here.
 * @var mac_ax_mu_protect_rsp_type::u
 * Please Place Description here.
 */
struct mac_ax_mu_protect_rsp_type {
	union {
		u8 byte_type;
		struct mac_ax_protect_rsp_field feld_type;
	} u;
};

/**
 * @struct mac_ax_mu_sta_upd
 * @brief mac_ax_mu_sta_upd
 *
 * @var mac_ax_mu_sta_upd::macid
 * Please Place Description here.
 * @var mac_ax_mu_sta_upd::mu_idx
 * Please Place Description here.
 * @var mac_ax_mu_sta_upd::prot_rsp_type
 * Please Place Description here.
 * @var mac_ax_mu_sta_upd::mugrp_bitmap
 * Please Place Description here.
 * @var mac_ax_mu_sta_upd::dis_256q
 * Please Place Description here.
 * @var mac_ax_mu_sta_upd::dis_1024q
 * Please Place Description here.
 * @var mac_ax_mu_sta_upd::rsvd
 * Please Place Description here.
 */
struct mac_ax_mu_sta_upd {
	u8 macid;
	u8 mu_idx;
	struct mac_ax_mu_protect_rsp_type prot_rsp_type[5];
	u8 mugrp_bitmap: 5;
	u8 dis_256q: 1;
	u8 dis_1024q: 1;
	u8 rsvd: 1;
};

/**
 * @struct mac_ax_wlaninfo_get
 * @brief mac_ax_wlaninfo_get
 *
 * @var mac_ax_wlaninfo_get::info_sel
 * Please Place Description here.
 * @var mac_ax_wlaninfo_get::rsvd0
 * Please Place Description here.
 * @var mac_ax_wlaninfo_get::argv0
 * Please Place Description here.
 * @var mac_ax_wlaninfo_get::argv1
 * Please Place Description here.
 * @var mac_ax_wlaninfo_get::argv2
 * Please Place Description here.
 * @var mac_ax_wlaninfo_get::argv3
 * Please Place Description here.
 * @var mac_ax_wlaninfo_get::argv4
 * Please Place Description here.
 * @var mac_ax_wlaninfo_get::argv5
 * Please Place Description here.
 * @var mac_ax_wlaninfo_get::argv6
 * Please Place Description here.
 * @var mac_ax_wlaninfo_get::argv7
 * Please Place Description here.
 * @var mac_ax_wlaninfo_get::rsvd1
 * Please Place Description here.
 */
struct mac_ax_wlaninfo_get {
	u32 info_sel:4;
	u32 rsvd0:4;
	u32 argv0:8;
	u32 argv1:8;
	u32 argv2:8;
	u32 argv3:8;
	u32 argv4:8;
	u32 argv5:8;
	u32 argv6:8;
	u32 argv7:8;
	u32 rsvd1:24;
};

/**
 * @struct mac_ax_ccxrpt
 * @brief mac_ax_ccxrpt
 *
 * @var mac_ax_ccxrpt::macid
 * Please Place Description here.
 * @var mac_ax_ccxrpt::tx_state
 * Please Place Description here.
 * @var mac_ax_ccxrpt::sw_define
 * Please Place Description here.
 * @var mac_ax_ccxrpt::pkt_ok_num
 * Please Place Description here.
 * @var mac_ax_ccxrpt::rsvd0
 * Please Place Description here.
 */
struct mac_ax_ccxrpt {
	u32 macid:7;
	u32 tx_state:2;
	u32 sw_define:4;
	u32 pkt_ok_num:8;
	u32 data_txcnt:6;
	u32 rsvd0:5;
};

/**
 * @struct mac_ax_dumpwlanc
 * @brief mac_ax_dumpwlanc
 *
 * @var mac_ax_dumpwlanc::cmdid
 * Please Place Description here.
 * @var mac_ax_dumpwlanc::rsvd0
 * Please Place Description here.
 */
struct mac_ax_dumpwlanc {
	u32 cmdid:8;
	u32 rsvd0:24;
};

/**
 * @struct mac_ax_dumpwlans
 * @brief mac_ax_dumpwlans
 *
 * @var mac_ax_dumpwlans::cmdid
 * Please Place Description here.
 * @var mac_ax_dumpwlans::macid_grp
 * Please Place Description here.
 * @var mac_ax_dumpwlans::rsvd0
 * Please Place Description here.
 */
struct mac_ax_dumpwlans {
	u32 cmdid:8;
	u32 macid_grp:8;
	u32 rsvd0:16;
};

/**
 * @struct mac_ax_dumpwland
 * @brief mac_ax_dumpwland
 *
 * @var mac_ax_dumpwland::cmdid
 * Please Place Description here.
 * @var mac_ax_dumpwland::grp_type
 * Please Place Description here.
 * @var mac_ax_dumpwland::grp_id
 * Please Place Description here.
 * @var mac_ax_dumpwland::muru
 * Please Place Description here.
 * @var mac_ax_dumpwland::macid
 * Please Place Description here.
 */
struct mac_ax_dumpwland {
	u32 cmdid:8;
	u32 grp_type:8;
	u32 grp_id:8;
	u32 muru:8;
	u8 macid[4];
};

/**
 * @struct mac_ax_fixmode_para
 * @brief mac_ax_fixmode_para
 *
 * @var mac_ax_fixmode_para::tbl_hdr
 * Please Place Description here.
 * @var mac_ax_fixmode_para::force_sumuru_en
 * Please Place Description here.
 * @var mac_ax_fixmode_para::forcesu
 * Please Place Description here.
 * @var mac_ax_fixmode_para::forcemu
 * Please Place Description here.
 * @var mac_ax_fixmode_para::forceru
 * Please Place Description here.
 * @var mac_ax_fixmode_para::fix_fe_su_en
 * Please Place Description here.
 * @var mac_ax_fixmode_para::fix_fe_vhtmu_en
 * Please Place Description here.
 * @var mac_ax_fixmode_para::fix_fe_hemu_en
 * Please Place Description here.
 * @var mac_ax_fixmode_para::fix_fe_heru_en
 * Please Place Description here.
 * @var mac_ax_fixmode_para::fix_fe_ul_en
 * Please Place Description here.
 * @var mac_ax_fixmode_para::fix_frame_seq_su
 * Please Place Description here.
 * @var mac_ax_fixmode_para::fix_frame_seq_vhtmu
 * Please Place Description here.
 * @var mac_ax_fixmode_para::fix_frame_seq_hemu
 * Please Place Description here.
 * @var mac_ax_fixmode_para::fix_frame_seq_heru
 * Please Place Description here.
 * @var mac_ax_fixmode_para::fix_frame_seq_ul
 * Please Place Description here.
 * @var mac_ax_fixmode_para::is_dlruhwgrp
 * Please Place Description here.
 * @var mac_ax_fixmode_para::is_ulruhwgrp
 * Please Place Description here.
 * @var mac_ax_fixmode_para::prot_type_su
 * Please Place Description here.
 * @var mac_ax_fixmode_para::prot_type_vhtmu
 * Please Place Description here.
 * @var mac_ax_fixmode_para::resp_type_vhtmu
 * Please Place Description here.
 * @var mac_ax_fixmode_para::prot_type_hemu
 * Please Place Description here.
 * @var mac_ax_fixmode_para::resp_type_hemu
 * Please Place Description here.
 * @var mac_ax_fixmode_para::prot_type_heru
 * Please Place Description here.
 * @var mac_ax_fixmode_para::resp_type_heru
 * Please Place Description here.
 * @var mac_ax_fixmode_para::ul_prot_type
 * Please Place Description here.
 * @var mac_ax_fixmode_para::rugrpid
 * Please Place Description here.
 * @var mac_ax_fixmode_para::mugrpid
 * Please Place Description here.
 * @var mac_ax_fixmode_para::ulgrpid
 * Please Place Description here.
 * @var mac_ax_fixmode_para::rsvd1
 * Please Place Description here.
 */
struct mac_ax_fixmode_para {
	struct mac_ax_tbl_hdr tbl_hdr;
	u32 force_sumuru_en: 1;
	u32 forcesu: 1;
	u32 forcemu: 1;
	u32 forceru: 1;
	u32 fix_fe_su_en:1;
	u32 fix_fe_vhtmu_en:1;
	u32 fix_fe_hemu_en:1;
	u32 fix_fe_heru_en:1;
	u32 fix_fe_ul_en:1;
	u32 fix_frame_seq_su: 1;
	u32 fix_frame_seq_vhtmu: 1;
	u32 fix_frame_seq_hemu: 1;
	u32 fix_frame_seq_heru: 1;
	u32 fix_frame_seq_ul: 1;
	u32 is_dlruhwgrp: 1;
	u32 is_ulruhwgrp:1;
	u32 prot_type_su: 4;
	u32 prot_type_vhtmu: 4;
	u32 resp_type_vhtmu: 4;
	u32 prot_type_hemu: 4;
	u32 resp_type_hemu: 4;
	u32 prot_type_heru: 4;
	u32 resp_type_heru: 4;
	u32 ul_prot_type: 4;
	u32 rugrpid: 5;
	u32 mugrpid:5;
	u32 ulgrpid:5;
	u32 rsvd1:1;
	u32 fix_txcmdnum_en:1;
	u32 force_to_one:1;
};

/**
 * @struct mac_ax_tf_ba
 * @brief mac_ax_tf_ba
 *
 * @var mac_ax_tf_ba::fix_ba
 * Please Place Description here.
 * @var mac_ax_tf_ba::ru_psd
 * Please Place Description here.
 * @var mac_ax_tf_ba::tf_rate
 * Please Place Description here.
 * @var mac_ax_tf_ba::rf_gain_fix
 * Please Place Description here.
 * @var mac_ax_tf_ba::rf_gain_idx
 * Please Place Description here.
 * @var mac_ax_tf_ba::tb_ppdu_bw
 * Please Place Description here.
 * @var mac_ax_tf_ba::rate
 * Please Place Description here.
 * @var mac_ax_tf_ba::gi_ltf
 * Please Place Description here.
 * @var mac_ax_tf_ba::doppler
 * Please Place Description here.
 * @var mac_ax_tf_ba::stbc
 * Please Place Description here.
 * @var mac_ax_tf_ba::sta_coding
 * Please Place Description here.
 * @var mac_ax_tf_ba::tb_t_pe_nom
 * Please Place Description here.
 * @var mac_ax_tf_ba::pr20_bw_en
 * Please Place Description here.
 * @var mac_ax_tf_ba::ma_type
 * Please Place Description here.
 * @var mac_ax_tf_ba::rsvd1
 * Please Place Description here.
 */
struct mac_ax_tf_ba {
	u32 fix_ba:1;
	u32 ru_psd:9;
	u32 tf_rate:9;
	u32 rf_gain_fix:1;
	u32 rf_gain_idx:10;
	u32 tb_ppdu_bw:2;
	struct mac_ax_ru_rate_ent rate;
	u8 gi_ltf:3;
	u8 doppler:1;
	u8 stbc:1;
	u8 sta_coding:1;
	u8 tb_t_pe_nom:2;
	u8 pr20_bw_en:1;
	u8 ma_type: 1;
	u8 rsvd1: 6;
};

/**
 * @struct mac_ax_ba_infotbl
 * @brief mac_ax_ba_infotbl
 *
 * @var mac_ax_ba_infotbl::tbl_hdr
 * Please Place Description here.
 * @var mac_ax_ba_infotbl::tfba
 * Please Place Description here.
 */
struct mac_ax_ba_infotbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	struct mac_ax_tf_ba tfba;
};

/**
 * @struct mac_ax_dl_ru_grptbl
 * @brief mac_ax_dl_ru_grptbl
 *
 * @var mac_ax_dl_ru_grptbl::tbl_hdr
 * Please Place Description here.
 * @var mac_ax_dl_ru_grptbl::ppdu_bw
 * Please Place Description here.
 * @var mac_ax_dl_ru_grptbl::tx_pwr
 * Please Place Description here.
 * @var mac_ax_dl_ru_grptbl::pwr_boost_fac
 * Please Place Description here.
 * @var mac_ax_dl_ru_grptbl::fix_mode_flag
 * Please Place Description here.
 * @var mac_ax_dl_ru_grptbl::rsvd1
 * Please Place Description here.
 * @var mac_ax_dl_ru_grptbl::rsvd
 * Please Place Description here.
 * @var mac_ax_dl_ru_grptbl::tf
 * Please Place Description here.
 */
struct mac_ax_dl_ru_grptbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	u16 ppdu_bw:2;
	u16 tx_pwr:9;
	u16 pwr_boost_fac:5;
	u8 fix_mode_flag:1;
	u8 rsvd1:7;
	u8 rsvd;
	struct mac_ax_tf_ba tf;
};

/**
 * @struct mac_ax_ul_ru_grptbl
 * @brief mac_ax_ul_ru_grptbl
 *
 * @var mac_ax_ul_ru_grptbl::tbl_hdr
 * Please Place Description here.
 * @var mac_ax_ul_ru_grptbl::grp_psd_max
 * Please Place Description here.
 * @var mac_ax_ul_ru_grptbl::grp_psd_min
 * Please Place Description here.
 * @var mac_ax_ul_ru_grptbl::tf_rate
 * Please Place Description here.
 * @var mac_ax_ul_ru_grptbl::fix_tf_rate
 * Please Place Description here.
 * @var mac_ax_ul_ru_grptbl::rsvd2
 * Please Place Description here.
 * @var mac_ax_ul_ru_grptbl::ppdu_bw
 * Please Place Description here.
 * @var mac_ax_ul_ru_grptbl::rf_gain_fix
 * Please Place Description here.
 * @var mac_ax_ul_ru_grptbl::rf_gain_idx
 * Please Place Description here.
 * @var mac_ax_ul_ru_grptbl::fix_mode_flag
 * Please Place Description here.
 * @var mac_ax_ul_ru_grptbl::rsvd1
 * Please Place Description here.
 */
struct mac_ax_ul_ru_grptbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	u32 grp_psd_max: 9;
	u32 grp_psd_min: 9;
	u32 tf_rate: 9;
	u32 fix_tf_rate: 1;
	u32 rsvd2: 4;
	u16 ppdu_bw: 2;
	u16 rf_gain_fix: 1;
	u16 rf_gain_idx: 10;
	u16 fix_mode_flag: 1;
	u16 rsvd1: 2;
};

/**
 * @struct mac_ax_bb_stainfo
 * @brief mac_ax_bb_stainfo
 *
 * @var mac_ax_bb_stainfo::tbl_hdr
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::gi_ltf_48spt
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::gi_ltf_18spt
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::rsvd3
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::dlsu_info_en
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::dlsu_bw
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::dlsu_gi_ltf
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::dlsu_doppler_ctrl
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::dlsu_coding
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::dlsu_txbf
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::dlsu_stbc
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::dl_fwcqi_flag
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::dlru_ratetbl_ridx
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::csi_info_bitmap
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::dl_swgrp_bitmap
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::dlsu_dcm
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::rsvd1
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::dlsu_rate
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::dlsu_pwr
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::rsvd2
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::rsvd4
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::ulsu_info_en
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::ulsu_bw
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::ulsu_gi_ltf
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::ulsu_doppler_ctrl
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::ulsu_dcm
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::ulsu_ss
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::ulsu_mcs
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::ul_fwcqi_flag
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::ulru_ratetbl_ridx
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::ulsu_stbc
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::ulsu_coding
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::ulsu_rssi_m
 * Please Place Description here.
 * @var mac_ax_bb_stainfo::ul_swgrp_bitmap
 * Please Place Description here.
 */
struct mac_ax_bb_stainfo {
	struct mac_ax_tbl_hdr tbl_hdr;
//sta capability
	u8 gi_ltf_48spt:1;
	u8 gi_ltf_18spt:1;
	u8 rsvd3:6;
//downlink su
	u8 dlsu_info_en:1;
	u8 dlsu_bw:2;
	u8 dlsu_gi_ltf:3;
	u8 dlsu_doppler_ctrl:2;
	u8 dlsu_coding:1;
	u8 dlsu_txbf:1;
	u8 dlsu_stbc:1;
	u8 dl_fwcqi_flag:1;
	u8 dlru_ratetbl_ridx:4;
	u8 csi_info_bitmap;
	u32 dl_swgrp_bitmap;
	u16 dlsu_dcm:1;
	u16 rsvd1:6;
	u16 dlsu_rate:9;
	u8 dlsu_pwr:6;
	u8 rsvd2:2;
	u8 rsvd4;
//uplink su
	u8 ulsu_info_en:1;
	u8 ulsu_bw:2;
	u8 ulsu_gi_ltf:3;
	u8 ulsu_doppler_ctrl:2;
	u8 ulsu_dcm:1;
	u8 ulsu_ss:3;
	u8 ulsu_mcs:4;
	u16 ul_fwcqi_flag:1;
	u16 ulru_ratetbl_ridx:4;
	u16 ulsu_stbc:1;
	u16 ulsu_coding:1;
	u16 ulsu_rssi_m:9;
	u32 ul_swgrp_bitmap;
//tb info
};

/**
 * @struct mac_ax_tf_depend_user_para
 * @brief mac_ax_tf_depend_user_para
 *
 * @var mac_ax_tf_depend_user_para::pref_AC
 * Please Place Description here.
 * @var mac_ax_tf_depend_user_para::rsvd
 * Please Place Description here.
 */
struct mac_ax_tf_depend_user_para {
	u8 pref_AC: 2;
	u8 rsvd: 6;
};

/**
 * @struct mac_ax_tf_user_para
 * @brief mac_ax_tf_user_para
 *
 * @var mac_ax_tf_user_para::aid12
 * Please Place Description here.
 * @var mac_ax_tf_user_para::ul_mcs
 * Please Place Description here.
 * @var mac_ax_tf_user_para::macid
 * Please Place Description here.
 * @var mac_ax_tf_user_para::ru_pos
 * Please Place Description here.
 * @var mac_ax_tf_user_para::ul_fec_code
 * Please Place Description here.
 * @var mac_ax_tf_user_para::ul_dcm
 * Please Place Description here.
 * @var mac_ax_tf_user_para::ss_alloc
 * Please Place Description here.
 * @var mac_ax_tf_user_para::ul_tgt_rssi
 * Please Place Description here.
 * @var mac_ax_tf_user_para::rsvd
 * Please Place Description here.
 * @var mac_ax_tf_user_para::rsvd2
 * Please Place Description here.
 */
struct mac_ax_tf_user_para {
	u16 aid12: 12;
	u16 ul_mcs: 4;
	u8 macid;
	u8 ru_pos;

	u8 ul_fec_code: 1;
	u8 ul_dcm: 1;
	u8 ss_alloc: 6;
	u8 ul_tgt_rssi: 7;
	u8 rsvd: 1;
	u16 rsvd2;
};

/**
 * @struct mac_ax_tf_pkt_para
 * @brief mac_ax_tf_pkt_para
 *
 * @var mac_ax_tf_pkt_para::ul_bw
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para::gi_ltf
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para::num_he_ltf
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para::ul_stbc
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para::doppler
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para::ap_tx_power
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para::rsvd0
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para::user_num
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para::pktnum
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para::rsvd1
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para::pri20_bitmap
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para::user
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para::dep_user
 * Please Place Description here.
 */
struct mac_ax_tf_pkt_para {
	u8 ul_bw: 2;
	u8 gi_ltf: 2;
	u8 num_he_ltf: 3;
	u8 ul_stbc: 1;
	u8 doppler: 1;
	u8 ap_tx_power: 6;
	u8 rsvd0: 1;
	u8 user_num: 3;
	u8 pktnum: 3;
	u8 rsvd1: 2;
	u8 pri20_bitmap;

	struct mac_ax_tf_user_para user[4];
	struct mac_ax_tf_depend_user_para dep_user[4];
};

/**
 * @struct mac_ax_tf_pkt_para
 * @brief mac_ax_tf_pkt_para
 *
 * @var mac_ax_tf_pkt_para_v1::ul_bw
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para_v1::gi_ltf
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para_v1::num_he_ltf
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para_v1::ul_stbc
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para_v1::doppler
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para_v1::ap_tx_power
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para_v1::rsvd0
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para_v1::user_num
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para_v1::pktnum
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para_v1::rsvd1
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para_v1::pri20_bitmap
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para_v1::user
 * Please Place Description here.
 * @var mac_ax_tf_pkt_para_v1::dep_user
 * Please Place Description here.
 */
struct mac_ax_tf_pkt_para_v1 {
	u8 ul_bw: 2;
	u8 gi_ltf: 2;
	u8 num_he_ltf: 3;
	u8 ul_stbc: 1;
	u8 doppler: 1;
	u8 ap_tx_power: 6;
	u8 rsvd0: 1;
	u8 user_num: 4;
	u8 pktnum: 4;
	u8 pri20_bitmap;

	struct mac_ax_tf_user_para user[8];
	struct mac_ax_tf_depend_user_para dep_user[8];
};

/**
 * @struct mac_ax_tf_wd_para
 * @brief mac_ax_tf_wd_para
 *
 * @var mac_ax_tf_wd_para::datarate
 * Please Place Description here.
 * @var mac_ax_tf_wd_para::mulport_id
 * Please Place Description here.
 * @var mac_ax_tf_wd_para::pwr_ofset
 * Please Place Description here.
 * @var mac_ax_tf_wd_para::rsvd
 * Please Place Description here.
 */
struct mac_ax_tf_wd_para {
	u16 datarate: 9;
	u16 mulport_id: 3;
	u16 pwr_ofset: 3;
	u16 rsvd: 1;
};

/**
 * @struct mac_ax_f2p_test_para
 * @brief mac_ax_f2p_test_para
 *
 * @var mac_ax_f2p_test_para::tf_pkt
 * Please Place Description here.
 * @var mac_ax_f2p_test_para::tf_wd
 * Please Place Description here.
 * @var mac_ax_f2p_test_para::mode
 * Please Place Description here.
 * @var mac_ax_f2p_test_para::frexch_type
 * Please Place Description here.
 * @var mac_ax_f2p_test_para::sigb_len
 * Please Place Description here.
 */
struct mac_ax_f2p_test_para {
	struct mac_ax_tf_pkt_para tf_pkt;
	struct mac_ax_tf_wd_para tf_wd;
	u8 mode: 2;
	u8 frexch_type: 6;
	u8 sigb_len;
};

/**
 * @struct mac_ax_f2p_test_para_v1
 * @brief mac_ax_f2p_test_para_v1
 *
 * @var mac_ax_f2p_test_para_v1::tf_pkt
 * Please Place Description here.
 * @var mac_ax_f2p_test_para_v1::tf_wd
 * Please Place Description here.
 * @var mac_ax_f2p_test_para_v1::mode
 * Please Place Description here.
 * @var mac_ax_f2p_test_para_v1::frexch_type
 * Please Place Description here.
 * @var mac_ax_f2p_test_para_v1::sigb_len
 * Please Place Description here.
 */
struct mac_ax_f2p_test_para_v1 {
	struct mac_ax_tf_pkt_para_v1 tf_pkt;
	struct mac_ax_tf_wd_para tf_wd;
	u8 mode: 2;
	u8 frexch_type: 6;
	u8 sigb_len;
};

/**
 * @struct mac_ax_f2p_wd
 * @brief mac_ax_f2p_wd
 *
 * @var mac_ax_f2p_wd::cmd_qsel
 * Please Place Description here.
 * @var mac_ax_f2p_wd::rsvd0
 * Please Place Description here.
 * @var mac_ax_f2p_wd::rsvd1
 * Please Place Description here.
 * @var mac_ax_f2p_wd::ls
 * Please Place Description here.
 * @var mac_ax_f2p_wd::fs
 * Please Place Description here.
 * @var mac_ax_f2p_wd::total_number
 * Please Place Description here.
 * @var mac_ax_f2p_wd::seq
 * Please Place Description here.
 * @var mac_ax_f2p_wd::length
 * Please Place Description here.
 * @var mac_ax_f2p_wd::rsvd2
 * Please Place Description here.
 */
struct mac_ax_f2p_wd {
	/* dword 0 */
	u32 cmd_qsel:6;
	u32 rsvd0:2;
	u32 rsvd1:2;
	u32 ls:1;
	u32 fs:1;
	u32 total_number:4;
	u32 seq:8;
	u32 length:8;
	/* dword 1 */
	u32 rsvd2;
};

/**
 * @struct mac_ax_f2p_tx_cmd
 * @brief mac_ax_f2p_tx_cmd
 *
 * @var mac_ax_f2p_tx_cmd::cmd_type
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::cmd_sub_type
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::dl_user_num
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::bw
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::tx_power
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::fw_define
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ss_sel_mode
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::next_qsel
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::twt_group
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::dis_chk_slp
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ru_mu_2_su
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::dl_t_pe
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::sigb_ch1_len
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::sigb_ch2_len
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::sigb_sym_num
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::sigb_ch2_ofs
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::dis_htp_ack
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::tx_time_ref
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::pri_user_idx
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ampdu_max_txtime
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::group_id
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::twt_chk_en
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::twt_port_id
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::twt_start_time
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::twt_end_time
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::apep_len
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::tri_pad
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_t_pe
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rf_gain_idx
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::fixed_gain_en
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_gi_ltf
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_doppler
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_stbc
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_mid_per
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_cqi_rrp_tri
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd4
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::sigb_dcm
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::sigb_comp
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::doppler
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::stbc
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::mid_per
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::gi_ltf_size
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::sigb_mcs
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd5
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::macid_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ac_type_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::mu_sta_pos_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::dl_rate_idx_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::dl_dcm_en_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd6
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ru_alo_idx_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::pwr_boost_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::agg_bmp_alo_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ampdu_max_txnum_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::user_define_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::user_define_ext_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_addr_idx_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_dcm_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_fec_cod_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_ru_rate_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd8
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_ru_alo_idx_u0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd9
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::macid_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ac_type_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::mu_sta_pos_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::dl_rate_idx_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::dl_dcm_en_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd10
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ru_alo_idx_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::pwr_boost_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::agg_bmp_alo_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ampdu_max_txnum_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::user_define_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::user_define_ext_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_addr_idx_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_dcm_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_fec_cod_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_ru_rate_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd12
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_ru_alo_idx_u1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd13
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::macid_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ac_type_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::mu_sta_pos_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::dl_rate_idx_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::dl_dcm_en_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd14
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ru_alo_idx_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::pwr_boost_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::agg_bmp_alo_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ampdu_max_txnum_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::user_define_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::user_define_ext_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_addr_idx_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_dcm_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_fec_cod_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_ru_rate_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd16
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_ru_alo_idx_u2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd17
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::macid_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ac_type_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::mu_sta_pos_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::dl_rate_idx_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::dl_dcm_en_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd18
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ru_alo_idx_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::pwr_boost_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::agg_bmp_alo_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ampdu_max_txnum_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::user_define_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::user_define_ext_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_addr_idx_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_dcm_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_fec_cod_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_ru_rate_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd20
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_ru_alo_idx_u3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd21
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::pkt_id_0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd22
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::valid_0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_user_num_0
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd23
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::pkt_id_1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd24
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::valid_1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_user_num_1
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd25
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::pkt_id_2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd26
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::valid_2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_user_num_2
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd27
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::pkt_id_3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd28
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::valid_3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_user_num_3
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd29
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::pkt_id_4
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd30
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::valid_4
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_user_num_4
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd31
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::pkt_id_5
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd32
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::valid_5
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::ul_user_num_5
 * Please Place Description here.
 * @var mac_ax_f2p_tx_cmd::rsvd33
 * Please Place Description here.
 */
struct mac_ax_f2p_tx_cmd {
	/* dword 0 */
	u32 cmd_type:8;
	u32 cmd_sub_type:8;
	u32 dl_user_num:5;
	u32 bw:2;
	u32 tx_power:9;
	/* dword 1 */
	u32 fw_define:16;
	u32 ss_sel_mode:2;
	u32 next_qsel:6;
	u32 twt_group:4;
	u32 dis_chk_slp:1;
	u32 ru_mu_2_su:1;
	u32 dl_t_pe:2;
	/* dword 2 */
	u32 sigb_ch1_len:8;
	u32 sigb_ch2_len:8;
	u32 sigb_sym_num:6;
	u32 sigb_ch2_ofs:5;
	u32 dis_htp_ack:1;
	u32 tx_time_ref:2;
	u32 pri_user_idx:2;
	/* dword 3 */
	u32 ampdu_max_txtime:14;
	u32 rsvd0:2;
	u32 group_id:6;
	u32 rsvd1:2;
	u32 rsvd2:4;
	u32 twt_chk_en:1;
	u32 twt_port_id:3;
	/* dword 4 */
	u32 twt_start_time:32;
	/* dword 5 */
	u32 twt_end_time:32;
	/* dword 6 */
	u32 apep_len:12;
	u32 tri_pad:2;
	u32 ul_t_pe:2;
	u32 rf_gain_idx:10;
	u32 fixed_gain_en:1;
	u32 ul_gi_ltf:3;
	u32 ul_doppler:1;
	u32 ul_stbc:1;
	/* dword 7 */
	u32 ul_mid_per:1;
	u32 ul_cqi_rrp_tri:1;
	u32 rsvd3:6;
	u32 rsvd4:8;
	u32 sigb_dcm:1;
	u32 sigb_comp:1;
	u32 doppler:1;
	u32 stbc:1;
	u32 mid_per:1;
	u32 gi_ltf_size:3;
	u32 sigb_mcs:3;
	u32 rsvd5:5;
	/* dword 8 */
	u32 macid_u0:8;
	u32 ac_type_u0:2;
	u32 mu_sta_pos_u0:2;
	u32 dl_rate_idx_u0:9;
	u32 dl_dcm_en_u0:1;
	u32 rsvd6:2;
	u32 ru_alo_idx_u0:8;
	/* dword 9 */
	u32 pwr_boost_u0:5;
	u32 agg_bmp_alo_u0:3;
	u32 ampdu_max_txnum_u0:8;
	u32 user_define_u0:8;
	u32 user_define_ext_u0:8;
	/* dword 10 */
	u32 ul_addr_idx_u0:8;
	u32 ul_dcm_u0:1;
	u32 ul_fec_cod_u0:1;
	u32 ul_ru_rate_u0:7;
	u32 rsvd8:7;
	u32 ul_ru_alo_idx_u0:8;
	/* dword 11 */
	u32 rsvd9:32;
	/* dword 12 */
	u32 macid_u1:8;
	u32 ac_type_u1:2;
	u32 mu_sta_pos_u1:2;
	u32 dl_rate_idx_u1:9;
	u32 dl_dcm_en_u1:1;
	u32 rsvd10:2;
	u32 ru_alo_idx_u1:8;
	/* dword 13 */
	u32 pwr_boost_u1:5;
	u32 agg_bmp_alo_u1:3;
	u32 ampdu_max_txnum_u1:8;
	u32 user_define_u1:8;
	u32 user_define_ext_u1:8;
	/* dword 14 */
	u32 ul_addr_idx_u1:8;
	u32 ul_dcm_u1:1;
	u32 ul_fec_cod_u1:1;
	u32 ul_ru_rate_u1:7;
	u32 rsvd12:7;
	u32 ul_ru_alo_idx_u1:8;
	/* dword 15 */
	u32 rsvd13:32;
	/* dword 16 */
	u32 macid_u2:8;
	u32 ac_type_u2:2;
	u32 mu_sta_pos_u2:2;
	u32 dl_rate_idx_u2:9;
	u32 dl_dcm_en_u2:1;
	u32 rsvd14:2;
	u32 ru_alo_idx_u2:8;
	/* dword 17 */
	u32 pwr_boost_u2:5;
	u32 agg_bmp_alo_u2:3;
	u32 ampdu_max_txnum_u2:8;
	u32 user_define_u2:8;
	u32 user_define_ext_u2:8;
	/* dword 18 */
	u32 ul_addr_idx_u2:8;
	u32 ul_dcm_u2:1;
	u32 ul_fec_cod_u2:1;
	u32 ul_ru_rate_u2:7;
	u32 rsvd16:7;
	u32 ul_ru_alo_idx_u2:8;
	/* dword 19 */
	u32 rsvd17:32;
	/* dword 20 */
	u32 macid_u3:8;
	u32 ac_type_u3:2;
	u32 mu_sta_pos_u3:2;
	u32 dl_rate_idx_u3:9;
	u32 dl_dcm_en_u3:1;
	u32 rsvd18:2;
	u32 ru_alo_idx_u3:8;
	/* dword 21 */
	u32 pwr_boost_u3:5;
	u32 agg_bmp_alo_u3:3;
	u32 ampdu_max_txnum_u3:8;
	u32 user_define_u3:8;
	u32 user_define_ext_u3:8;
	/* dword 22 */
	u32 ul_addr_idx_u3:8;
	u32 ul_dcm_u3:1;
	u32 ul_fec_cod_u3:1;
	u32 ul_ru_rate_u3:7;
	u32 rsvd20:7;
	u32 ul_ru_alo_idx_u3:8;
	/* dword 23 */
	u32 rsvd21:32;
	/* dword 24 */
	u32 pkt_id_0:12;
	u32 rsvd22:3;
	u32 valid_0:1;
	u32 ul_user_num_0:4;
	u32 rsvd23:12;
	/* dword 25 */
	u32 pkt_id_1:12;
	u32 rsvd24:3;
	u32 valid_1:1;
	u32 ul_user_num_1:4;
	u32 rsvd25:12;
	/* dword 26 */
	u32 pkt_id_2:12;
	u32 rsvd26:3;
	u32 valid_2:1;
	u32 ul_user_num_2:4;
	u32 rsvd27:12;
	/* dword 27 */
	u32 pkt_id_3:12;
	u32 rsvd28:3;
	u32 valid_3:1;
	u32 ul_user_num_3:4;
	u32 rsvd29:12;
	/* dword 28 */
	u32 pkt_id_4:12;
	u32 rsvd30:3;
	u32 valid_4:1;
	u32 ul_user_num_4:4;
	u32 rsvd31:12;
	/* dword 29 */
	u32 pkt_id_5:12;
	u32 rsvd32:3;
	u32 valid_5:1;
	u32 ul_user_num_5:4;
	u32 rsvd33:12;
};

//todo: doxygen
struct mac_ax_f2p_tx_cmd_v1 {
	/* dword 0 */
	u32 cmd_type:8;
	u32 cmd_sub_type:8;
	u32 dl_user_num:5;
	u32 bw:2;
	u32 tx_power:9;
	/* dword 1 */
	u32 fw_define:16;
	u32 ss_sel_mode:2;
	u32 next_qsel:6;
	u32 twt_group:4;
	u32 dis_chk_slp:1;
	u32 ru_mu_2_su:1;
	u32 dl_t_pe:2;
	/* dword 2 */
	u32 ch20_with_data:8;
	u32 s_idx:8;
	u32 ru_grp_ntx:3;
	u32 rsvd0:6;
	u32 retry_brk:1;
	u32 txop_brk:1;
	u32 dis_htp_ack:1;
	u32 pri_user_idx:4;
	/* dword 3 */
	u32 ampdu_max_txtime:14;
	u32 rsvd1:1;
	u32 fix_tx_time_mode:1;
	u32 group_id:6;
	u32 ul_ap_pwr:6;
	u32 twt_chk_en:1;
	u32 twt_port_id:3;
	/* dword 4 */
	u32 twt_start_time:32;
	/* dword 5 */
	u32 twt_end_time:32;
	/* dword 6 */
	u32 apep_len:12;
	u32 tri_pad:2;
	u32 ul_t_pe:2;
	u32 rf_gain_idx:10;
	u32 fixed_gain_en:1;
	u32 ul_gi_ltf:3;
	u32 ul_doppler:1;
	u32 ul_stbc:1;
	/* dword 7 */
	u32 ul_mid_per:1;
	u32 ul_cqi_rrp_tri:1;
	u32 ul_len_ref:4;
	u32 rsvd2:2;
	u32 pri_exp_rssi_dbm:7;
	u32 elna_idx:1;
	u32 rsvd3:2;
	u32 doppler:1;
	u32 stbc:1;
	u32 mid_per:1;
	u32 gi_ltf_size:3;
	u32 rsvd4:7;
	u32 dis_force_cts2:1;

	/* dword 8 */
	u32 macid_u0:8;
	u32 ac_type_u0:2;
	u32 rsvd5:2;
	u32 dl_rate_idx_u0:9;
	u32 dl_dcm_en_u0:1;
	u32 dl_ldpc_en_u0:1;
	u32 txbf_en_u0:1;
	u32 ru_alo_idx_u0:8;
	/* dword 9 */
	u32 pwr_boost_u0:5;
	u32 agg_bmp_alo_u0:3;
	u32 ampdu_max_txnum_u0:8;
	u32 aid_u0:12;
	u32 rsvd6:3;
	u32 preload_en_u0:1;
	/* dword 10 */
	u32 ul_addr_idx_u0:8;
	u32 ul_dcm_u0:1;
	u32 ul_fec_cod_u0:1;
	u32 ul_ru_rate_u0:7;
	u32 rsvd7:7;
	u32 ul_ru_alo_idx_u0:8;
	/* dword 11 */
	u32 user_define_u0:32;

	/* dword 12 */
	u32 macid_u1:8;
	u32 ac_type_u1:2;
	u32 rsvd8:2;
	u32 dl_rate_idx_u1:9;
	u32 dl_dcm_en_u1:1;
	u32 dl_ldpc_en_u1:1;
	u32 txbf_en_u1:1;
	u32 ru_alo_idx_u1:8;
	/* dword 13 */
	u32 pwr_boost_u1:5;
	u32 agg_bmp_alo_u1:3;
	u32 ampdu_max_txnum_u1:8;
	u32 aid_u1:12;
	u32 rsvd9:3;
	u32 preload_en_u1:1;
	/* dword 14 */
	u32 ul_addr_idx_u1:8;
	u32 ul_dcm_u1:1;
	u32 ul_fec_cod_u1:1;
	u32 ul_ru_rate_u1:7;
	u32 rsvd10:7;
	u32 ul_ru_alo_idx_u1:8;
	/* dword 15 */
	u32 user_define_u1:32;

	/* dword 16 */
	u32 macid_u2:8;
	u32 ac_type_u2:2;
	u32 rsvd11:2;
	u32 dl_rate_idx_u2:9;
	u32 dl_dcm_en_u2:1;
	u32 dl_ldpc_en_u2:1;
	u32 txbf_en_u2:1;
	u32 ru_alo_idx_u2:8;
	/* dword 17 */
	u32 pwr_boost_u2:5;
	u32 agg_bmp_alo_u2:3;
	u32 ampdu_max_txnum_u2:8;
	u32 aid_u2:12;
	u32 rsvd12:3;
	u32 preload_en_u2:1;
	/* dword 18 */
	u32 ul_addr_idx_u2:8;
	u32 ul_dcm_u2:1;
	u32 ul_fec_cod_u2:1;
	u32 ul_ru_rate_u2:7;
	u32 rsvd13:7;
	u32 ul_ru_alo_idx_u2:8;
	/* dword 19 */
	u32 user_define_u2:32;

	/* dword 20 */
	u32 macid_u3:8;
	u32 ac_type_u3:2;
	u32 rsvd14:2;
	u32 dl_rate_idx_u3:9;
	u32 dl_dcm_en_u3:1;
	u32 dl_ldpc_en_u3:1;
	u32 txbf_en_u3:1;
	u32 ru_alo_idx_u3:8;
	/* dword 21 */
	u32 pwr_boost_u3:5;
	u32 agg_bmp_alo_u3:3;
	u32 ampdu_max_txnum_u3:8;
	u32 aid_u3:12;
	u32 rsvd15:3;
	u32 preload_en_u3:1;
	/* dword 22 */
	u32 ul_addr_idx_u3:8;
	u32 ul_dcm_u3:1;
	u32 ul_fec_cod_u3:1;
	u32 ul_ru_rate_u3:7;
	u32 rsvd16:7;
	u32 ul_ru_alo_idx_u3:8;
	/* dword 23 */
	u32 user_define_u3:32;

	/* dword 24 */
	u32 macid_u4:8;
	u32 ac_type_u4:2;
	u32 rsvd17:2;
	u32 dl_rate_idx_u4:9;
	u32 dl_dcm_en_u4:1;
	u32 dl_ldpc_en_u4:1;
	u32 txbf_en_u4:1;
	u32 ru_alo_idx_u4:8;
	/* dword 25 */
	u32 pwr_boost_u4:5;
	u32 agg_bmp_alo_u4:3;
	u32 ampdu_max_txnum_u4:8;
	u32 aid_u4:12;
	u32 rsvd18:3;
	u32 preload_en_u4:1;
	/* dword 26 */
	u32 ul_addr_idx_u4:8;
	u32 ul_dcm_u4:1;
	u32 ul_fec_cod_u4:1;
	u32 ul_ru_rate_u4:7;
	u32 rsvd19:7;
	u32 ul_ru_alo_idx_u4:8;
	/* dword 27 */
	u32 user_define_u4:32;

	/* dword 28 */
	u32 macid_u5:8;
	u32 ac_type_u5:2;
	u32 rsvd20:2;
	u32 dl_rate_idx_u5:9;
	u32 dl_dcm_en_u5:1;
	u32 dl_ldpc_en_u5:1;
	u32 txbf_en_u5:1;
	u32 ru_alo_idx_u5:8;
	/* dword 29 */
	u32 pwr_boost_u5:5;
	u32 agg_bmp_alo_u5:3;
	u32 ampdu_max_txnum_u5:8;
	u32 aid_u5:12;
	u32 rsvd21:3;
	u32 preload_en_u5:1;
	/* dword 30 */
	u32 ul_addr_idx_u5:8;
	u32 ul_dcm_u5:1;
	u32 ul_fec_cod_u5:1;
	u32 ul_ru_rate_u5:7;
	u32 rsvd22:7;
	u32 ul_ru_alo_idx_u5:8;
	/* dword 31 */
	u32 user_define_u5:32;

	/* dword 32 */
	u32 macid_u6:8;
	u32 ac_type_u6:2;
	u32 rsvd23:2;
	u32 dl_rate_idx_u6:9;
	u32 dl_dcm_en_u6:1;
	u32 dl_ldpc_en_u6:1;
	u32 txbf_en_u6:1;
	u32 ru_alo_idx_u6:8;
	/* dword 33 */
	u32 pwr_boost_u6:5;
	u32 agg_bmp_alo_u6:3;
	u32 ampdu_max_txnum_u6:8;
	u32 aid_u6:12;
	u32 rsvd24:3;
	u32 preload_en_u6:1;
	/* dword 34 */
	u32 ul_addr_idx_u6:8;
	u32 ul_dcm_u6:1;
	u32 ul_fec_cod_u6:1;
	u32 ul_ru_rate_u6:7;
	u32 rsvd25:7;
	u32 ul_ru_alo_idx_u6:8;
	/* dword 35 */
	u32 user_define_u6:32;

	/* dword 36 */
	u32 macid_u7:8;
	u32 ac_type_u7:2;
	u32 rsvd26:2;
	u32 dl_rate_idx_u7:9;
	u32 dl_dcm_en_u7:1;
	u32 dl_ldpc_en_u7:1;
	u32 txbf_en_u7:1;
	u32 ru_alo_idx_u7:8;
	/* dword 37 */
	u32 pwr_boost_u7:5;
	u32 agg_bmp_alo_u7:3;
	u32 ampdu_max_txnum_u7:8;
	u32 aid_u7:12;
	u32 rsvd27:3;
	u32 preload_en_u7:1;
	/* dword 38 */
	u32 ul_addr_idx_u7:8;
	u32 ul_dcm_u7:1;
	u32 ul_fec_cod_u7:1;
	u32 ul_ru_rate_u7:7;
	u32 rsvd28:7;
	u32 ul_ru_alo_idx_u7:8;
	/* dword 39 */
	u32 user_define_u7:32;

	/* dword 40 */
	u32 pkt_id_0:12;
	u32 rsvd29:3;
	u32 valid_0:1;
	u32 ul_user_num_0:4;
	u32 rsvd30:12;
	/* dword 41 */
	u32 pkt_id_1:12;
	u32 rsvd31:3;
	u32 valid_1:1;
	u32 ul_user_num_1:4;
	u32 rsvd32:12;
	/* dword 42 */
	u32 pkt_id_2:12;
	u32 rsvd33:3;
	u32 valid_2:1;
	u32 ul_user_num_2:4;
	u32 rsvd34:12;
	/* dword 43 */
	u32 pkt_id_3:12;
	u32 rsvd35:3;
	u32 valid_3:1;
	u32 ul_user_num_3:4;
	u32 rsvd36:12;
	/* dword 44 */
	u32 pkt_id_4:12;
	u32 rsvd37:3;
	u32 valid_4:1;
	u32 ul_user_num_4:4;
	u32 rsvd38:12;
	/* dword 45 */
	u32 pkt_id_5:12;
	u32 rsvd39:3;
	u32 valid_5:1;
	u32 ul_user_num_5:4;
	u32 rsvd40:12;
	/* dword 46 */
	u32 pkt_id_6:12;
	u32 rsvd41:3;
	u32 valid_6:1;
	u32 ul_user_num_6:4;
	u32 rsvd42:12;
	/* dword 47 */
	u32 pkt_id_7:12;
	u32 rsvd43:3;
	u32 valid_7:1;
	u32 ul_user_num_7:4;
	u32 rsvd44:12;
	/* dword 48 */
	u32 pkt_id_8:12;
	u32 rsvd45:3;
	u32 valid_8:1;
	u32 ul_user_num_8:4;
	u32 rsvd46:12;
	/* dword 49 */
	u32 pkt_id_9:12;
	u32 rsvd47:3;
	u32 valid_9:1;
	u32 ul_user_num_9:4;
	u32 rsvd48:12;
};

/*--------------------Define Sounding related struct -------------------------*/

/**
 * @struct mac_reg_csi_para
 * @brief mac_reg_csi_para
 *
 * @var mac_reg_csi_para::band
 * Please Place Description here.
 * @var mac_reg_csi_para::portsel
 * Please Place Description here.
 * @var mac_reg_csi_para::nc
 * Please Place Description here.
 * @var mac_reg_csi_para::nr
 * Please Place Description here.
 * @var mac_reg_csi_para::ng
 * Please Place Description here.
 * @var mac_reg_csi_para::cb
 * Please Place Description here.
 * @var mac_reg_csi_para::cs
 * Please Place Description here.
 * @var mac_reg_csi_para::ldpc_en
 * Please Place Description here.
 * @var mac_reg_csi_para::stbc_en
 * Please Place Description here.
 * @var mac_reg_csi_para::bf_en
 * Please Place Description here.
 */
struct mac_reg_csi_para {
	u32 band: 1;
	u32 portsel: 1;
	u32 nc: 3;
	u32 nr: 3;
	u32 ng: 2;
	u32 cb: 2;
	u32 cs: 2;
	u32 ldpc_en: 1;
	u32 stbc_en: 1;
	u32 bf_en: 1;
};

/**
 * @struct mac_cctl_csi_para
 * @brief mac_cctl_csi_para
 *
 * @var mac_cctl_csi_para::macid
 * Please Place Description here.
 * @var mac_cctl_csi_para::band
 * Please Place Description here.
 * @var mac_cctl_csi_para::nc
 * Please Place Description here.
 * @var mac_cctl_csi_para::nr
 * Please Place Description here.
 * @var mac_cctl_csi_para::ng
 * Please Place Description here.
 * @var mac_cctl_csi_para::cb
 * Please Place Description here.
 * @var mac_cctl_csi_para::cs
 * Please Place Description here.
 * @var mac_cctl_csi_para::bf_en
 * Please Place Description here.
 * @var mac_cctl_csi_para::stbc_en
 * Please Place Description here.
 * @var mac_cctl_csi_para::ldpc_en
 * Please Place Description here.
 * @var mac_cctl_csi_para::rate
 * Please Place Description here.
 * @var mac_cctl_csi_para::gi_ltf
 * Please Place Description here.
 * @var mac_cctl_csi_para::gid_sel
 * Please Place Description here.
 * @var mac_cctl_csi_para::bw
 * Please Place Description here.
 */
struct mac_cctl_csi_para {
	u8 macid;
	u32 band: 1;
	u32 nc: 3;
	u32 nr: 3;
	u32 ng: 2;
	u32 cb: 2;
	u32 cs: 2;
	u32 bf_en: 1;
	u32 stbc_en: 1;
	u32 ldpc_en: 1;
	u32 rate: 9;
	u32 gi_ltf: 3;
	u32 gid_sel: 1;
	u32 bw: 2;
};

/**
 * @struct mac_bf_sup
 * @brief mac_bf_sup
 *
 * @var mac_bf_sup::bf_entry_num
 * Please Place Description here.
 * @var mac_bf_sup::su_buffer_num
 * Please Place Description here.
 * @var mac_bf_sup::mu_buffer_num
 * Please Place Description here.
 */
struct mac_bf_sup {
	u32 bf_entry_num;
	u32 su_buffer_num;
	u32 mu_buffer_num;
};

/**
 * @struct mac_gid_pos
 * @brief mac_gid_pos
 *
 * @var mac_gid_pos::gid_en
 * Please Place Description here.
 * @var mac_gid_pos::gid_pos
 * Please Place Description here.
 */
struct mac_gid_pos {
	u32 band;
	u32 gid_tab[2];
	u32 user_pos[4];
};

/**
 * @struct mac_ax_ndpa_hdr
 * @brief mac_ax_ndpa_hdr
 *
 * @var mac_ax_ndpa_hdr::frame_ctl
 * Please Place Description here.
 * @var mac_ax_ndpa_hdr::duration
 * Please Place Description here.
 * @var mac_ax_ndpa_hdr::addr1
 * Please Place Description here.
 * @var mac_ax_ndpa_hdr::addr2
 * Please Place Description here.
 */
struct mac_ax_ndpa_hdr {
	u16 frame_ctl;
	u16 duration;
	u8 addr1[6];
	u8 addr2[6];
};

/**
 * @struct mac_ax_snd_dialog
 * @brief mac_ax_snd_dialog
 *
 * @var mac_ax_snd_dialog::he
 * Please Place Description here.
 * @var mac_ax_snd_dialog::dialog
 * Please Place Description here.
 * @var mac_ax_snd_dialog::rsvd
 * Please Place Description here.
 */
struct mac_ax_snd_dialog {
	u32 he: 1;
	u32 dialog: 6;
	u32 rsvd: 25;
};

/**
 * @struct mac_ax_ht_ndpa_para
 * @brief mac_ax_ht_ndpa_para
 *
 * @var mac_ax_ht_ndpa_para::addr3
 * Please Place Description here.
 * @var mac_ax_ht_ndpa_para::seq_control
 * Please Place Description here.
 */
struct mac_ax_ht_ndpa_para {
	u8 addr3[WLAN_ADDR_LEN];
	u16 seq_control;
};

/**
 * @struct mac_ax_vht_ndpa_sta_info
 * @brief mac_ax_vht_ndpa_sta_info
 *
 * @var mac_ax_vht_ndpa_sta_info::aid
 * Please Place Description here.
 * @var mac_ax_vht_ndpa_sta_info::fb_type
 * Please Place Description here.
 * @var mac_ax_vht_ndpa_sta_info::nc
 * Please Place Description here.
 */
struct mac_ax_vht_ndpa_sta_info {
	u16 aid: 12;
	u16 fb_type: 1;
	u16 nc: 3;
};

/**
 * @struct mac_ax_vht_ndpa_para
 * @brief mac_ax_vht_ndpa_para
 *
 * @var mac_ax_vht_ndpa_para::sta_info
 * Please Place Description here.
 */
struct mac_ax_vht_ndpa_para {
	struct mac_ax_vht_ndpa_sta_info sta_info[MAX_VHT_SUPPORT_SOUND_STA];
};

/**
 * @struct mac_ax_he_ndpa_sta_info
 * @brief mac_ax_he_ndpa_sta_info
 *
 * @var mac_ax_he_ndpa_sta_info::aid
 * Please Place Description here.
 * @var mac_ax_he_ndpa_sta_info::bw
 * Please Place Description here.
 * @var mac_ax_he_ndpa_sta_info::fb_ng
 * Please Place Description here.
 * @var mac_ax_he_ndpa_sta_info::disambiguation
 * Please Place Description here.
 * @var mac_ax_he_ndpa_sta_info::cb
 * Please Place Description here.
 * @var mac_ax_he_ndpa_sta_info::nc
 * Please Place Description here.
 */
struct mac_ax_he_ndpa_sta_info {
	u32 aid: 11;
	u32 bw: 14;
	u32 fb_ng: 2;
	u32 disambiguation: 1;
	u32 cb: 1;
	u32 nc: 3;
};

/**
 * @struct mac_ax_he_ndpa_para_V1
 * @brief mac_ax_he_ndpa_para_V1
 *
 * @var mac_ax_he_ndpa_para_V1::sta_info
 * Please Place Description here.
 */
struct mac_ax_he_ndpa_para {
	struct mac_ax_he_ndpa_sta_info sta_info[MAX_HE_SUPPORT_SOUND_STA];
};

/**
 * @struct mac_ax_ndpa_para_v1
 * @brief mac_ax_ndpa_para_v1
 *
 * @var mac_ax_ndpa_para_v1::common
 * Please Place Description here.
 * @var mac_ax_ndpa_para_v1::snd_dialog
 * Please Place Description here.
 * @var mac_ax_ndpa_para_v1::ht_para
 * Please Place Description here.
 * @var mac_ax_ndpa_para_v1::vht_para
 * Please Place Description here.
 * @var mac_ax_ndpa_para_v1::he_para
 * Please Place Description here.
 */
struct mac_ax_ndpa_para {
	struct mac_ax_ndpa_hdr common;
	struct mac_ax_snd_dialog snd_dialog;
	struct mac_ax_ht_ndpa_para ht_para;
	struct mac_ax_vht_ndpa_para vht_para;
	struct mac_ax_he_ndpa_para he_para;
};

/**
 * @struct mac_ax_bfrp_hdr
 * @brief mac_ax_bfrp_hdr
 *
 * @var mac_ax_bfrp_hdr::frame_ctl
 * Please Place Description here.
 * @var mac_ax_bfrp_hdr::duration
 * Please Place Description here.
 * @var mac_ax_bfrp_hdr::addr1
 * Please Place Description here.
 * @var mac_ax_bfrp_hdr::addr2
 * Please Place Description here.
 */
struct mac_ax_bfrp_hdr {
	u16 frame_ctl;
	u16 duration;
	u8 addr1[WLAN_ADDR_LEN];
	u8 addr2[WLAN_ADDR_LEN];
};

/**
 * @struct mac_ax_vht_bfrp_para
 * @brief mac_ax_vht_bfrp_para
 *
 * @var mac_ax_vht_bfrp_para::retransmission_bitmap
 * Please Place Description here.
 */
struct mac_ax_vht_bfrp_para {
	u8 retransmission_bitmap;
};

/**
 * @struct mac_ax_he_bfrp_common
 * @brief mac_ax_he_bfrp_common
 *
 * @var mac_ax_he_bfrp_common::tgr_info
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::ul_len
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::more_tf
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::cs_rqd
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::ul_bw
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::gi_ltf
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::mimo_ltfmode
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::num_heltf
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::ul_pktext
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::ul_stbc
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::ldpc_extra_sym
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::dplr
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::ap_tx_pwr
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::ul_sr
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::ul_siga2_rsvd
 * Please Place Description here.
 * @var mac_ax_he_bfrp_common::rsvd
 * Please Place Description here.
 */
struct mac_ax_he_bfrp_common {
	u32 tgr_info: 4;
	u32 ul_len: 12;
	u32 more_tf: 1;
	u32 cs_rqd: 1;
	u32 ul_bw: 2;
	u32 gi_ltf: 2;
	u32 mimo_ltfmode: 1;
	u32 num_heltf: 3;
	u32 ul_pktext: 3;
	u32 ul_stbc: 1;
	u32 ldpc_extra_sym: 1;
	u32 dplr: 1;

	u32 ap_tx_pwr: 6;
	u32 ul_sr: 16;
	u32 ul_siga2_rsvd: 9;
	u32 rsvd: 1;
};

/**
 * @struct mac_ax_he_bfrp_user
 * @brief mac_ax_he_bfrp_user
 *
 * @var mac_ax_he_bfrp_user::aid12
 * Please Place Description here.
 * @var mac_ax_he_bfrp_user::ru_pos
 * Please Place Description here.
 * @var mac_ax_he_bfrp_user::ul_fec_code
 * Please Place Description here.
 * @var mac_ax_he_bfrp_user::ul_mcs
 * Please Place Description here.
 * @var mac_ax_he_bfrp_user::ul_dcm
 * Please Place Description here.
 * @var mac_ax_he_bfrp_user::ss_alloc
 * Please Place Description here.
 * @var mac_ax_he_bfrp_user::fbseg_rexmit_bmp
 * Please Place Description here.
 * @var mac_ax_he_bfrp_user::ul_tgt_rssi
 * Please Place Description here.
 * @var mac_ax_he_bfrp_user::rsvd
 * Please Place Description here.
 */
struct mac_ax_he_bfrp_user {
	u32 aid12: 12;
	u32 ru_pos: 8;
	u32 ul_fec_code: 1;
	u32 ul_mcs: 4;
	u32 ul_dcm: 1;
	u32 ss_alloc: 6;

	u32 fbseg_rexmit_bmp: 8;
	u32 ul_tgt_rssi: 7;
	u32 rsvd: 17;
};

/**
 * @struct mac_ax_he_bfrp_para_v1
 * @brief mac_ax_he_bfrp_para_v1
 *
 * @var mac_ax_he_bfrp_para_v1::common
 * Please Place Description here.
 * @var mac_ax_he_bfrp_para_v1::user
 * Please Place Description here.
 */
struct mac_ax_he_bfrp_para {
	struct mac_ax_he_bfrp_common common;
	struct mac_ax_he_bfrp_user user[8];
};

/**
 * @struct mac_ax_bfrp_para_v1
 * @brief mac_ax_bfrp_para_v1
 *
 * @var mac_ax_bfrp_para_v1::hdr
 * Please Place Description here.
 * @var mac_ax_bfrp_para_v1::he_para
 * Please Place Description here.
 * @var mac_ax_bfrp_para_v1::vht_para
 * Please Place Description here.
 * @var mac_ax_bfrp_para_v1::rsvd
 * Please Place Description here.
 */
struct mac_ax_bfrp_para {
	struct mac_ax_bfrp_hdr hdr[3];
	struct mac_ax_he_bfrp_para he_para[2];
	struct mac_ax_vht_bfrp_para vht_para[3];
	u8 rsvd;
};

/**
 * @struct mac_ax_snd_wd_para
 * @brief mac_ax_snd_wd_para
 *
 * @var mac_ax_snd_wd_para::txpktsize
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::ndpa_duration
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::datarate
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::macid
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::force_txop
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::data_bw
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::gi_ltf
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::data_er
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::data_dcm
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::data_stbc
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::data_ldpc
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::data_bw_er
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::multiport_id
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::mbssid
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::signaling_ta_pkt_sc
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::sw_define
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::txpwr_ofset_type
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::lifetime_sel
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::stf_mode
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::disdatafb
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::data_txcnt_lmt_sel
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::data_txcnt_lmt
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::sifs_tx
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::snd_pkt_sel
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::ndpa
 * Please Place Description here.
 * @var mac_ax_snd_wd_para::rsvd
 * Please Place Description here.
 */
struct mac_ax_snd_wd_para {
	u16 txpktsize;
	u16 ndpa_duration;

	u16 datarate: 9;
	u16 macid: 7;//wd
	u8 force_txop: 1;
	u8 data_bw: 2;
	u8 gi_ltf: 3;
	u8 data_er: 1;
	u8 data_dcm: 1;
	u8 data_stbc: 1;
	u8 data_ldpc: 1;
	u8 data_bw_er : 1;
	u8 multiport_id: 1;
	u8 mbssid: 4;

	u8 signaling_ta_pkt_sc: 4;
	u8 sw_define: 4;
	u8 txpwr_ofset_type: 3;
	u8 lifetime_sel: 3;
	u8 stf_mode: 1;
	u8 disdatafb: 1;
	u8 data_txcnt_lmt_sel: 1;
	u8 data_txcnt_lmt: 6;
	u8 sifs_tx: 1;
	u8 snd_pkt_sel: 3;
	u8 ndpa: 2;
	u8 rsvd: 3;
};

/**
 * @struct mac_ax_snd_f2P
 * @brief mac_ax_snd_f2P
 *
 * @var mac_ax_snd_f2P::csi_len_bfrp
 * Please Place Description here.
 * @var mac_ax_snd_f2P::tb_t_pe_bfrp
 * Please Place Description here.
 * @var mac_ax_snd_f2P::tri_pad_bfrp
 * Please Place Description here.
 * @var mac_ax_snd_f2P::ul_cqi_rpt_tri_bfrp
 * Please Place Description here.
 * @var mac_ax_snd_f2P::rf_gain_idx_bfrp
 * Please Place Description here.
 * @var mac_ax_snd_f2P::fix_gain_en_bfrp
 * Please Place Description here.
 * @var mac_ax_snd_f2P::rsvd
 * Please Place Description here.
 */
struct mac_ax_snd_f2P {
	u16 csi_len_bfrp: 12;
	u16 tb_t_pe_bfrp: 2;
	u16 tri_pad_bfrp: 2;

	u16 ul_cqi_rpt_tri_bfrp: 1;
	u16 rf_gain_idx_bfrp: 10;
	u16 fix_gain_en_bfrp: 1;
	u16 ul_len_ref: 4;
};

/**
 * @struct mac_ax_snd_f2P
 * @brief mac_ax_snd_f2P
 *
 * @var mac_ax_snd_f2p_period::f2p_type
 * Please Place Description here.
 * @var mac_ax_snd_f2p_period::f2p_index
 * Please Place Description here.
 * @var mac_ax_snd_f2p_period::f2p_period
 * Please Place Description here.
 * @var mac_ax_snd_f2p_period::idx
 * Please Place Description here.
 * @var mac_ax_snd_f2p_period::rsvd1
 * Please Place Description here.
 * @var mac_ax_snd_f2p_period::rsvd2
 * Please Place Description here.
 * @var mac_ax_snd_f2p_period::rsvd3
 * Please Place Description here.
 */
struct mac_ax_snd_f2p_period {
	u16 f2p_type: 8;
	u16 f2p_index: 8;
	u16 f2p_period;

	u32 f2p_updcnt: 8;
	u32 cr_idx: 22;
	u32 rsvd: 2;

};

/**
 * @struct mac_ax_fwcmd_snd_V1
 * @brief mac_ax_fwcmd_snd_V1
 *
 * @var mac_ax_fwcmd_snd_v1::frexgtype
 * Please Place Description here.
 * @var mac_ax_fwcmd_snd_v1::mode
 * Please Place Description here.
 * @var mac_ax_fwcmd_snd_v1::bfrp0_user_num
 * Please Place Description here.
 * @var mac_ax_fwcmd_snd_v1::bfrp1_user_num
 * Please Place Description here.
 * @var mac_ax_fwcmd_snd_v1::rsvd
 * Please Place Description here.
 * @var mac_ax_fwcmd_snd_v1::macid
 * Please Place Description here.
 * @var mac_ax_fwcmd_snd_v1::pndpa
 * Please Place Description here.
 * @var mac_ax_fwcmd_snd_v1::pbfrp
 * Please Place Description here.
 * @var mac_ax_fwcmd_snd_v1::wd
 * Please Place Description here.
 * @var mac_ax_fwcmd_snd_v1::f2p
 * Please Place Description here.
 */
struct mac_ax_fwcmd_snd {
	u32 frexgtype: 6;
	u32 mode: 2;
	u32 bfrp0_user_num: 8;
	u32 bfrp1_user_num: 8;
	u32 rsvd: 8;
	u8 macid[16];
	struct mac_ax_ndpa_para pndpa;
	struct mac_ax_bfrp_para pbfrp;
	struct mac_ax_snd_wd_para wd[5];
	struct mac_ax_snd_f2P f2p[2];
	struct mac_ax_snd_f2p_period sfp;
};

/**
 * @struct mac_ax_ie_cam_info
 * @brief mac_ax_ie_cam_info
 *
 * @var mac_ax_ie_cam_info::type
 * Please Place Description here.
 * @var mac_ax_ie_cam_info::ienum_ie
 * Please Place Description here.
 * @var mac_ax_ie_cam_info::ie_ofst_len
 * Please Place Description here.
 * @var mac_ax_ie_cam_info::ie_msk_crc
 * Please Place Description here.
 * @var mac_ax_ie_cam_info::ie_val
 * Please Place Description here.
 * @var mac_ax_ie_cam_info::rsvd0
 * Please Place Description here.
 * @var mac_ax_ie_cam_info::rsvd1
 * Please Place Description here.
 * @var mac_ax_ie_cam_info::rsvd2
 * Please Place Description here.
 */
struct mac_ax_ie_cam_info {
	u8 type;
	u8 ienum_ie;
	u8 ie_ofst_len;
	u8 ie_msk_crc;
	u8 ie_val;
	u8 rsvd0;
	u8 rsvd1;
	u8 rsvd2;
};

/**
 * @struct mac_iecam_type_ie_t
 * @brief mac_iecam_type_ie_t
 *
 * @var mac_iecam_type_ie_t::type
 * Please Place Description here.
 * @var mac_iecam_type_ie_t::ienum
 * Please Place Description here.
 * @var mac_iecam_type_ie_t::ie_list
 * Please Place Description here.
 */
struct mac_iecam_type_ie_t {
	u8 type; /* reference to enum mac_iecam_ent_type */
	u8 ienum;
	u8 ie_list[3];
};

/**
 * @struct mac_iecam_type_ofst_t
 * @brief mac_iecam_type_ofst_t
 *
 * @var mac_iecam_type_ofst_t::type
 * Please Place Description here.
 * @var mac_iecam_type_ofst_t::ie
 * Please Place Description here.
 * @var mac_iecam_type_ofst_t::ofst
 * Please Place Description here.
 * @var mac_iecam_type_ofst_t::msk
 * Please Place Description here.
 * @var mac_iecam_type_ofst_t::val
 * Please Place Description here.
 */
struct mac_iecam_type_ofst_t {
	u8 type; /* reference to enum mac_iecam_ent_type */
	u8 ie;
	u8 ofst;
	u8 msk;
	u8 val;
};

/**
 * @struct mac_ie_cam_type_crc_t
 * @brief mac_ie_cam_type_crc_t
 *
 * @var mac_ie_cam_type_crc_t::type
 * Please Place Description here.
 * @var mac_ie_cam_type_crc_t::ie
 * Please Place Description here.
 * @var mac_ie_cam_type_crc_t::ie_len
 * Please Place Description here.
 * @var mac_ie_cam_type_crc_t::crc8
 * Please Place Description here.
 */
struct mac_iecam_type_crc_t {
	u8 type; /* reference to enum mac_iecam_ent_type */
	u8 ie;
	u8 ie_len;
	u8 crc8;
};

/**
 * @struct mac_iecam_data_t
 * @brief mac_iecam_data_t
 *
 * @var mac_iecam_data_t::dw0
 * Please Place Description here.
 * @var mac_iecam_data_t::dw1
 * Please Place Description here.
 */
struct mac_iecam_data_t {
	u32 dw0;
	u32 dw1;
};

/**
 * @struct mac_iecam_hdr_t
 * @brief mac_iecam_hdr_t
 *
 * @var mac_iecam_hdr_t::dw0
 * Please Place Description here.
 * @var mac_iecam_hdr_t::dw1
 * Please Place Description here.
 */
struct mac_iecam_hdr_t {
	u32 cam_idx:8;
	u32 rsvd:24;
};

/**
 * @struct mac_ie_cam_ent
 * @brief mac_ie_cam_ent
 *
 * @var mac_ie_cam_ent::cam_idx
 * Please Place Description here.
 * @var mac_ie_cam_ent::type
 * Please Place Description here.
 * @var mac_ie_cam_ent::ie_t
 * Please Place Description here.
 * @var mac_ie_cam_ent::ofst_t
 * Please Place Description here.
 * @var mac_ie_cam_ent::crc_t
 * Please Place Description here.
 */
struct mac_ie_cam_ent {
	struct mac_iecam_hdr_t hdr;
	union {
		struct mac_iecam_type_ie_t ie_t;
		struct mac_iecam_type_ofst_t ofst_t;
		struct mac_iecam_type_crc_t crc_t;
		struct mac_iecam_data_t data;
	} u;
};

/**
 * @struct mac_bcn_ofld_info
 * @brief mac_bcn_ofld_info
 *
 * @var mac_bcn_ofld_info::ctrl_type
 * Please Place Description here.
 * @var mac_bcn_ofld_info::band
 * Please Place Description here.
 * @var mac_bcn_ofld_info::port
 * Please Place Description here.
 * @var mac_bcn_ofld_info::hit_en
 * Please Place Description here.
 * @var mac_bcn_ofld_info::miss_en
 * Please Place Description here.
 * @var mac_bcn_ofld_info::rst_iecam
 * Please Place Description here.
 * @var mac_bcn_ofld_info::hit_sel
 * Please Place Description here.
 * @var mac_bcn_ofld_info::miss_sel
 * Please Place Description here.
 * @var mac_bcn_ofld_info::cam_list
 * Please Place Description here.
 * @var mac_bcn_ofld_info::cam_num
 * Please Place Description here.
 */
struct mac_bcn_ofld_info {
	enum mac_bcn_ofld_ctrl_type ctrl_type;
	enum mac_ax_band band;
	enum mac_ax_port port;
	enum mac_ax_func_sw hit_en;
	enum mac_ax_func_sw miss_en;
	enum mac_ax_func_sw rst_iecam;
	enum mac_iecam_frwd_sel hit_sel;
	enum mac_iecam_frwd_sel miss_sel;
	struct mac_ie_cam_ent cam_list[MAC_AX_IECAM_NUM];
	u8 cam_num;
};

/**
 * @struct mac_calc_crc_info
 * @brief mac_calc_crc_info
 *
 * @var mac_calc_crc_info::buf
 * Please Place Description here.
 * @var mac_bcn_ofld_info::len
 * Please Place Description here.
 * @var mac_bcn_ofld_info::crc
 * Please Place Description here.
 */
struct mac_calc_crc_info {
	u8 *buf;
	u32 len;
	u8 crc;
};

/*--------------------Define wowlan related struct ---------------------------*/

/**
 * @struct mac_ax_keep_alive_info
 * @brief mac_ax_keep_alive_info
 *
 * @var mac_ax_keep_alive_info::keepalive_en
 * Please Place Description here.
 * @var mac_ax_keep_alive_info::rsvd
 * Please Place Description here.
 * @var mac_ax_keep_alive_info::packet_id
 * Please Place Description here.
 * @var mac_ax_keep_alive_info::period
 * Please Place Description here.
 */
struct mac_ax_keep_alive_info {
	u8 keepalive_en: 1;
	u8 rsvd: 7;
	u8 packet_id;
	u8 period;
};

/**
 * @struct mac_ax_disconnect_det_info
 * @brief mac_ax_disconnect_det_info
 *
 * @var mac_ax_disconnect_det_info::disconnect_detect_en
 * Please Place Description here.
 * @var mac_ax_disconnect_det_info::tryok_bcnfail_count_en
 * Please Place Description here.
 * @var mac_ax_disconnect_det_info::disconnect_en
 * Please Place Description here.
 * @var mac_ax_disconnect_det_info::rsvd
 * Please Place Description here.
 * @var mac_ax_disconnect_det_info::check_period
 * Please Place Description here.
 * @var mac_ax_disconnect_det_info::try_pkt_count
 * Please Place Description here.
 * @var mac_ax_disconnect_det_info::tryok_bcnfail_count_limit
 * Please Place Description here.
 */
struct mac_ax_disconnect_det_info {
	u8 disconnect_detect_en: 1;
	u8 tryok_bcnfail_count_en: 1;
	u8 disconnect_en: 1;
	u8 rsvd: 5;
	u8 check_period;
	u8 try_pkt_count;
	u8 tryok_bcnfail_count_limit;
};

/**
 * @enum mac_ax_enc_alg
 *
 * @brief mac_ax_enc_alg
 *
 * @var mac_ax_enc_alg::MAC_AX_RTW_ENC_NONE
 * Please Place Description here.
 * @var mac_ax_enc_alg::MAC_AX_RTW_ENC_WEP40
 * Please Place Description here.
 * @var mac_ax_enc_alg::MAC_AX_RTW_ENC_WEP104
 * Please Place Description here.
 * @var mac_ax_enc_alg::MAC_AX_RTW_ENC_TKIP
 * Please Place Description here.
 * @var mac_ax_enc_alg::MAC_AX_RTW_ENC_WAPI
 * Please Place Description here.
 * @var mac_ax_enc_alg::MAC_AX_RTW_ENC_GCMSMS4
 * Please Place Description here.
 * @var mac_ax_enc_alg::MAC_AX_RTW_ENC_CCMP
 * Please Place Description here.
 * @var mac_ax_enc_alg::MAC_AX_RTW_ENC_CCMP256
 * Please Place Description here.
 * @var mac_ax_enc_alg::MAC_AX_RTW_ENC_GCMP
 * Please Place Description here.
 * @var mac_ax_enc_alg::MAC_AX_RTW_ENC_GCMP256
 * Please Place Description here.
 * @var mac_ax_enc_alg::MAC_AX_RTW_ENC_BIP_CCMP128
 * Please Place Description here.
 * @var mac_ax_enc_alg::MAC_AX_RTW_ENC_MAX
 * Please Place Description here.
 */
enum mac_ax_enc_alg {
	MAC_AX_RTW_ENC_NONE = 0,
	MAC_AX_RTW_ENC_WEP40 = 1,
	MAC_AX_RTW_ENC_WEP104,
	MAC_AX_RTW_ENC_TKIP,
	MAC_AX_RTW_ENC_WAPI,
	MAC_AX_RTW_ENC_GCMSMS4,
	MAC_AX_RTW_ENC_CCMP,
	MAC_AX_RTW_ENC_CCMP256,
	MAC_AX_RTW_ENC_GCMP,
	MAC_AX_RTW_ENC_GCMP256,
	MAC_AX_RTW_ENC_BIP_CCMP128,
	MAC_AX_RTW_ENC_MAX
};

/**
 * @enum bip_sec_algo_type
 *
 * @brief bip_sec_algo_type
 *
 * @var bip_sec_algo_type::BIP_CMAC_128
 * Please Place Description here.
 * @var bip_sec_algo_type::BIP_CMAC_256
 * Please Place Description here.
 * @var bip_sec_algo_type::BIP_GMAC_128
 * Please Place Description here.
 * @var bip_sec_algo_type::BIP_GMAC_256
 * Please Place Description here.
 */
enum bip_sec_algo_type {
	BIP_CMAC_128 = 0,
	BIP_CMAC_256 = 1,
	BIP_GMAC_128 = 2,
	BIP_GMAC_256 = 3
};

/**
 * @struct mac_ax_wow_wake_info
 * @brief mac_ax_wow_wake_info
 *
 * @var mac_ax_wow_wake_info::wow_en
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::drop_all_pkt
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::rx_parse_after_wake
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::rsvd
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::pairwise_sec_algo
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::group_sec_algo
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::remotectrl_info_content
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::pattern_match_en
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::magic_en
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::hw_unicast_en
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::fw_unicast_en
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::deauth_wakeup
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::rekey_wakeup
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::eap_wakeup
 * Please Place Description here.
 * @var mac_ax_wow_wake_info::all_data_wakeup
 * Please Place Description here.
 */
struct mac_ax_wow_wake_info {
	u8 wow_en: 1;
	u8 drop_all_pkt: 1;
	u8 rx_parse_after_wake: 1;
	u8 rsvd: 5;
	enum mac_ax_enc_alg pairwise_sec_algo;
	enum mac_ax_enc_alg group_sec_algo;
	u32 remotectrl_info_content;
	u8 pattern_match_en: 1;
	u8 magic_en: 1;
	u8 hw_unicast_en: 1;
	u8 fw_unicast_en: 1;
	u8 deauth_wakeup: 1;
	u8 rekey_wakeup: 1;
	u8 eap_wakeup: 1;
	u8 all_data_wakeup: 1;
};

#define IV_LENGTH 8

/**
 * @struct mac_ax_remotectrl_info_parm_
 * @brief mac_ax_remotectrl_info_parm_
 *
 * @var mac_ax_remotectrl_info_parm_::ptktxiv
 * Please Place Description here.
 * @var mac_ax_remotectrl_info_parm_::validcheck
 * Please Place Description here.
 * @var mac_ax_remotectrl_info_parm_::symbolchecken
 * Please Place Description here.
 * @var mac_ax_remotectrl_info_parm_::lastkeyid
 * Please Place Description here.
 * @var mac_ax_remotectrl_info_parm_::rsvd
 * Please Place Description here.
 * @var mac_ax_remotectrl_info_parm_::rxptkiv
 * Please Place Description here.
 * @var mac_ax_remotectrl_info_parm_::rxgtkiv_0
 * Please Place Description here.
 * @var mac_ax_remotectrl_info_parm_::rxgtkiv_1
 * Please Place Description here.
 * @var mac_ax_remotectrl_info_parm_::rxgtkiv_2
 * Please Place Description here.
 * @var mac_ax_remotectrl_info_parm_::rxgtkiv_3
 * Please Place Description here.
 */
struct mac_ax_remotectrl_info_parm_ {
	u8  ptktxiv[IV_LENGTH];
	/* value = 0xdd */
	u8  validcheck;
	/* bit0 : check ptk, bit1 : check gtk */
	u8  symbolchecken;
	/* the last gtk index used by driver */
	u8  lastkeyid;
	u8  rsvd[5];
	/* unicast iv */
	u8  rxptkiv[IV_LENGTH];
	/* broadcast/mulicast iv, 4 gtk index */
	u8  rxgtkiv_0[IV_LENGTH];
	u8  rxgtkiv_1[IV_LENGTH];
	u8  rxgtkiv_2[IV_LENGTH];
	u8  rxgtkiv_3[IV_LENGTH];
};

/**
 * @struct mac_ax_wake_ctrl_info
 * @brief mac_ax_wake_ctrl_info
 *
 * @var mac_ax_wake_ctrl_info::pattern_match_en
 * Please Place Description here.
 * @var mac_ax_wake_ctrl_info::magic_en
 * Please Place Description here.
 * @var mac_ax_wake_ctrl_info::hw_unicast_en
 * Please Place Description here.
 * @var mac_ax_wake_ctrl_info::fw_unicast_en
 * Please Place Description here.
 * @var mac_ax_wake_ctrl_info::deauth_wakeup
 * Please Place Description here.
 * @var mac_ax_wake_ctrl_info::rekey_wakeup
 * Please Place Description here.
 * @var mac_ax_wake_ctrl_info::eap_wakeup
 * Please Place Description here.
 * @var mac_ax_wake_ctrl_info::all_data_wakeup
 * Please Place Description here.
 */
struct mac_ax_wake_ctrl_info {
	u8 pattern_match_en: 1;
	u8 magic_en: 1;
	u8 hw_unicast_en: 1;
	u8 fw_unicast_en: 1;
	u8 deauth_wakeup: 1;
	u8 rekey_wakeup: 1;
	u8 eap_wakeup: 1;
	u8 all_data_wakeup: 1;
};

/**
 * @struct mac_ax_gtk_ofld_info
 * @brief mac_ax_gtk_ofld_info
 *
 * @var mac_ax_gtk_ofld_info::gtk_en
 * Please Place Description here.
 * @var mac_ax_gtk_ofld_info::tkip_en
 * Please Place Description here.
 * @var mac_ax_gtk_ofld_info::ieee80211w_en
 * Please Place Description here.
 * @var mac_ax_gtk_ofld_info::pairwise_wakeup
 * Please Place Description here.
 * @var mac_ax_gtk_ofld_info::norekey_wakeup
 * Please Place Description here.
 * @var mac_ax_gtk_ofld_info::bip_sec_algo
 * Please Place Description here.
 * @var mac_ax_gtk_ofld_info::rsvd
 * Please Place Description here.
 * @var mac_ax_gtk_ofld_info::gtk_rsp_id
 * Please Place Description here.
 * @var mac_ax_gtk_ofld_info::pmf_sa_query_id
 * Please Place Description here.
 */
struct mac_ax_gtk_ofld_info {
	u8 gtk_en: 1;
	u8 tkip_en: 1;
	u8 ieee80211w_en: 1;
	u8 pairwise_wakeup: 1;
	u8 norekey_wakeup: 1;
	u8 bip_sec_algo: 2;
	u8 rsvd: 1;
	u8 gtk_rsp_id: 8;
	u8 pmf_sa_query_id: 8;
	u8 algo_akm_suit: 8;
};

#define AOAC_REPORT_VERSION 1

/**
 * @struct mac_ax_aoac_report
 * @brief mac_ax_aoac_report
 *
 * @var mac_ax_aoac_report::rpt_ver
 * Please Place Description here.
 * @var mac_ax_aoac_report::sec_type
 * Please Place Description here.
 * @var mac_ax_aoac_report::key_idx
 * Please Place Description here.
 * @var mac_ax_aoac_report::pattern_idx
 * Please Place Description here.
 * @var mac_ax_aoac_report::rekey_ok
 * Please Place Description here.
 * @var mac_ax_aoac_report::rsvd0
 * Please Place Description here.
 * @var mac_ax_aoac_report::rsvd1
 * Please Place Description here.
 * @var mac_ax_aoac_report::ptk_tx_iv
 * Please Place Description here.
 * @var mac_ax_aoac_report::eapol_key_replay_count
 * Please Place Description here.
 * @var mac_ax_aoac_report::gtk
 * Please Place Description here.
 * @var mac_ax_aoac_report::ptk_rx_iv
 * Please Place Description here.
 * @var mac_ax_aoac_report::gtk_rx_iv_0
 * Please Place Description here.
 * @var mac_ax_aoac_report::gtk_rx_iv_1
 * Please Place Description here.
 * @var mac_ax_aoac_report::gtk_rx_iv_2
 * Please Place Description here.
 * @var mac_ax_aoac_report::gtk_rx_iv_3
 * Please Place Description here.
 * @var mac_ax_aoac_report::igtk_key_id
 * Please Place Description here.
 * @var mac_ax_aoac_report::igtk_ipn
 * Please Place Description here.
 * @var mac_ax_aoac_report::igtk
 * Please Place Description here.
 */
struct mac_ax_aoac_report {
	u8 rpt_ver;
	u8 sec_type;
	u8 key_idx;
	u8 pattern_idx;
	u8 rekey_ok: 1;
	u8 rsvd0: 7;
	u8 rsvd1[3];
	u8 ptk_tx_iv[IV_LENGTH];
	u8 eapol_key_replay_count[8];
	u8 gtk[32];
	u8 ptk_rx_iv[IV_LENGTH];
	u8 gtk_rx_iv_0[IV_LENGTH];
	u8 gtk_rx_iv_1[IV_LENGTH];
	u8 gtk_rx_iv_2[IV_LENGTH];
	u8 gtk_rx_iv_3[IV_LENGTH];
	u8 igtk_key_id[8];
	u8 igtk_ipn[8];
	u8 igtk[32];
};

#define EAPOL_KCK_LENGTH 32
#define EAPOL_KEK_LENGTH 32
#define TKIP_TK_LENGTH 16
#define TKIP_MIC_KEY_LENGTH 8
#define IGTK_KEY_ID_LENGTH 4
#define IGTK_PKT_NUM_LENGTH 8
#define IGTK_LENGTH 16
#define IGTK_OFFSET 4

union keytype {
	u8 SKEY[32];
	u32 LKEY[4];
};

/**
 * @struct mac_ax_gtk_info_parm_
 * @brief mac_ax_gtk_info_parm_
 *
 * @var mac_ax_gtk_info_parm_::kck
 * Please Place Description here.
 * @var mac_ax_gtk_info_parm_::kek
 * Please Place Description here.
 * @var mac_ax_gtk_info_parm_::tk1
 * Please Place Description here.
 * @var mac_ax_gtk_info_parm_::txmickey
 * Please Place Description here.
 * @var mac_ax_gtk_info_parm_::rxmickey
 * Please Place Description here.
 * @var mac_ax_gtk_info_parm_::igtk_keyid
 * Please Place Description here.
 * @var mac_ax_gtk_info_parm_::ipn
 * Please Place Description here.
 * @var mac_ax_gtk_info_parm_::igtk
 * Please Place Description here.
 * @var mac_ax_gtk_info_parm_::sk
 * Please Place Description here.
 */
struct mac_ax_gtk_info_parm_ {
	/* eapol - key confirmation key (kck) */
	u8  kck[EAPOL_KCK_LENGTH];
	/* eapol - key encryption key (kek) */
	u8  kek[EAPOL_KEK_LENGTH];
	/* temporal key 1 (tk1) */
	u8  tk1[TKIP_TK_LENGTH];
	u8  txmickey[TKIP_MIC_KEY_LENGTH];
	u8  rxmickey[TKIP_MIC_KEY_LENGTH];
	u8 igtk_keyid[IGTK_KEY_ID_LENGTH];
	u8 ipn[IGTK_PKT_NUM_LENGTH];
	union keytype igtk[2];
	union keytype sk[1];
};

/**
 * @struct mac_ax_arp_ofld_info
 * @brief mac_ax_arp_ofld_info
 *
 * @var mac_ax_arp_ofld_info::arp_en
 * Please Place Description here.
 * @var mac_ax_arp_ofld_info::arp_action
 * Please Place Description here.
 * @var mac_ax_arp_ofld_info::rsvd
 * Please Place Description here.
 * @var mac_ax_arp_ofld_info::arp_rsp_id
 * Please Place Description here.
 */
struct mac_ax_arp_ofld_info {
	u8 arp_en: 1;
	u8 arp_action: 1;
	u8 rsvd: 6;
	u8 arp_rsp_id: 8;
};

/**
 * @struct mac_ax_ndp_ofld_info
 * @brief mac_ax_ndp_ofld_info
 *
 * @var mac_ax_ndp_ofld_info::ndp_en
 * Please Place Description here.
 * @var mac_ax_ndp_ofld_info::rsvd
 * Please Place Description here.
 * @var mac_ax_ndp_ofld_info::na_id
 * Please Place Description here.
 */
struct mac_ax_ndp_ofld_info {
	u8 ndp_en: 1;
	u8 rsvd: 7;
	u8 na_id: 8;
};

#define MAC_ADDRESS_LENGTH    6
#define IPV6_ADDRESS_LENGTH   16

/**
 * @struct mac_ax_ndp_info_parm_
 * @brief mac_ax_ndp_info_parm_
 *
 * @var mac_ax_ndp_info_parm_::enable
 * Please Place Description here.
 * @var mac_ax_ndp_info_parm_::checkremoveip
 * Please Place Description here.
 * @var mac_ax_ndp_info_parm_::rsvd
 * Please Place Description here.
 * @var mac_ax_ndp_info_parm_::numberoftargetip
 * Please Place Description here.
 * @var mac_ax_ndp_info_parm_::targetlinkaddress
 * Please Place Description here.
 * @var mac_ax_ndp_info_parm_::remoteipv6address
 * Please Place Description here.
 * @var mac_ax_ndp_info_parm_::targetip
 * Please Place Description here.
 */
struct mac_ax_ndp_info_parm_ {
	u8 enable: 1;
	/* need to check sender ip or not */
	u8 checkremoveip: 1;
	/* need to check sender ip or not */
	u8 rsvd: 6;
	/* number of check ip which na query ip */
	u8 numberoftargetip;
	/* maybe support change mac address !! */
	u8 targetlinkaddress[MAC_ADDRESS_LENGTH];
	/* just respond ip */
	u8 remoteipv6address[IPV6_ADDRESS_LENGTH];
	/* target ip */
	u8 targetip[2][IPV6_ADDRESS_LENGTH];
};

/**
 * @struct mac_ax_realwow_info
 * @brief mac_ax_realwow_info
 *
 * @var mac_ax_realwow_info::realwow_en
 * Please Place Description here.
 * @var mac_ax_realwow_info::auto_wakeup
 * Please Place Description here.
 * @var mac_ax_realwow_info::rsvd0
 * Please Place Description here.
 * @var mac_ax_realwow_info::keepalive_id
 * Please Place Description here.
 * @var mac_ax_realwow_info::wakeup_pattern_id
 * Please Place Description here.
 * @var mac_ax_realwow_info::ack_pattern_id
 * Please Place Description here.
 */
struct mac_ax_realwow_info {
	u8 realwow_en: 1;
	u8 auto_wakeup: 1;
	u8 rsvd0: 6;
	u8 keepalive_id: 8;
	u8 wakeup_pattern_id: 8;
	u8 ack_pattern_id: 8;
};

/**
 * @struct mac_ax_realwowv2_info_parm_
 * @brief mac_ax_realwowv2_info_parm_
 *
 * @var mac_ax_realwowv2_info_parm_::interval
 * Please Place Description here.
 * @var mac_ax_realwowv2_info_parm_::kapktsize
 * Please Place Description here.
 * @var mac_ax_realwowv2_info_parm_::acklostlimit
 * Please Place Description here.
 * @var mac_ax_realwowv2_info_parm_::ackpatternsize
 * Please Place Description here.
 * @var mac_ax_realwowv2_info_parm_::wakeuppatternsize
 * Please Place Description here.
 * @var mac_ax_realwowv2_info_parm_::rsvd
 * Please Place Description here.
 * @var mac_ax_realwowv2_info_parm_::wakeupsecnum
 * Please Place Description here.
 */
struct mac_ax_realwowv2_info_parm_ {
	u16 interval;   /*unit : 1 ms */
	u16 kapktsize;
	u16 acklostlimit;
	u16 ackpatternsize;
	u16 wakeuppatternsize;
	u16 rsvd;
	u32 wakeupsecnum;
};

/**
 * @struct mac_ax_nlo_info
 * @brief mac_ax_nlo_info
 *
 * @var mac_ax_nlo_info::nlo_en
 * Please Place Description here.
 * @var mac_ax_nlo_info::nlo_32k_en
 * Please Place Description here.
 * @var mac_ax_nlo_info::compare_cipher_type
 * Please Place Description here.
 * @var mac_ax_nlo_info::rsvd
 * Please Place Description here.
 */
struct mac_ax_nlo_info {
	u8 nlo_en: 1;
	u8 nlo_32k_en: 1;
	u8 compare_cipher_type: 1;
	u8 rsvd: 5;
};

#define MAX_SUPPORT_NL_NUM   16
#define MAX_PROBE_REQ_NUM    8
#define SSID_MAX_LEN         32

/**
 * @struct mac_ax_nlo_networklist_parm_
 * @brief mac_ax_nlo_networklist_parm_
 *
 * @var mac_ax_nlo_networklist_parm_::numofentries
 * Please Place Description here.
 * @var mac_ax_nlo_networklist_parm_::numofhiddenap
 * Please Place Description here.
 * @var mac_ax_nlo_networklist_parm_::rsvd
 * Please Place Description here.
 * @var mac_ax_nlo_networklist_parm_::patterncheck
 * Please Place Description here.
 * @var mac_ax_nlo_networklist_parm_::rsvd1
 * Please Place Description here.
 * @var mac_ax_nlo_networklist_parm_::rsvd2
 * Please Place Description here.
 * @var mac_ax_nlo_networklist_parm_::ssidlen
 * Please Place Description here.
 * @var mac_ax_nlo_networklist_parm_::chipertype
 * Please Place Description here.
 * @var mac_ax_nlo_networklist_parm_::rsvd3
 * Please Place Description here.
 * @var mac_ax_nlo_networklist_parm_::locprobereq
 * Please Place Description here.
 * @var mac_ax_nlo_networklist_parm_::ssid
 * Please Place Description here.
 */
struct mac_ax_nlo_networklist_parm_ {
	u8  numofentries;
	u8  numofhiddenap;
	u8  rsvd[2];
	u32 patterncheck;
	u32 rsvd1;
	u32 rsvd2;
	u8  ssidlen[MAX_SUPPORT_NL_NUM];
	u8  chipertype[MAX_SUPPORT_NL_NUM];
	u8  rsvd3[MAX_SUPPORT_NL_NUM];
	u8  locprobereq[MAX_PROBE_REQ_NUM];
	u8  ssid[MAX_SUPPORT_NL_NUM][SSID_MAX_LEN];
};

/**
 * @struct mac_ax_negative_pattern_info
 * @brief mac_ax_negative_pattern_info
 *
 * @var mac_ax_negative_pattern_info::negative_pattern_en
 * Please Place Description here.
 * @var mac_ax_negative_pattern_info::rsvd
 * Please Place Description here.
 * @var mac_ax_negative_pattern_info::pattern_count
 * Please Place Description here.
 */
struct mac_ax_negative_pattern_info {
	u8 negative_pattern_en: 1;
	u8 rsvd: 3;
	u8 pattern_count: 4;
};

/**
 * @struct mac_ax_dev2hst_gpio_info
 * @brief mac_ax_dev2hst_gpio_info
 *
 * @var mac_ax_dev2hst_gpio_info::dev2hst_gpio_en
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::disable_inband
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_output_input
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_active
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::toggle_pulse
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::data_pin_wakeup
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_pulse_nonstop
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_time_unit
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_num
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_pulse_dura
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_pulse_period
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_pulse_count
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::rsvd0
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::customer_id
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::rsvd1
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_pulse_en_a
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_duration_unit_a
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_pulse_nonstop_a
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::rsvd2
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::special_reason_a
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_duration_a
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_pulse_count_a
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_pulse_en_b
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_duration_unit_b
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_pulse_nonstop_b
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::rsvd3
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::special_reason_b
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_duration_b
 * Please Place Description here.
 * @var mac_ax_dev2hst_gpio_info::gpio_pulse_count_b
 * Please Place Description here.
 */
struct mac_ax_dev2hst_gpio_info {
	/* dword0 */
	u32 dev2hst_gpio_en:1;
	u32 disable_inband:1;
	u32 gpio_output_input:1;
	u32 gpio_active:1;
	u32 toggle_pulse:1;
	u32 data_pin_wakeup:1;
	u32 gpio_pulse_nonstop:1;
	u32 gpio_time_unit:1;
	u32 gpio_num:8;
	u32 gpio_pulse_dura:8;
	u32 gpio_pulse_period:8;
	/* dword1 */
	u32 gpio_pulse_count:8;
	u32 rsvd0:24;
	/* dword2 */
	u32 customer_id:8;
	u32 rsvd1:24;
	/* dword3 */
	u32 rsn_a_en:1;
	u32 rsn_a_toggle_pulse:1;
	u32 rsn_a_pulse_nonstop:1;
	u32 rsn_a_time_unit:1;
	u32 rsvd2:28;
	/* dword4 */
	u32 rsn_a:8;
	u32 rsn_a_pulse_duration:8;
	u32 rsn_a_pulse_period:8;
	u32 rsn_a_pulse_count:8;
	/* dword5 */
	u32 rsn_b_en:1;
	u32 rsn_b_toggle_pulse:1;
	u32 rsn_b_pulse_nonstop:1;
	u32 rsn_b_time_unit:1;
	u32 rsvd3:28;
	/* dword6 */
	u32 rsn_b:8;
	u32 rsn_b_pulse_duration:8;
	u32 rsn_b_pulse_period:8;
	u32 rsn_b_pulse_count:8;
	/* dword7 */
	u32 gpio_pulse_en_a:8; /*deprecated*/
	u32 gpio_duration_unit_a:8; /*deprecated*/
	u32 gpio_pulse_nonstop_a:8; /*deprecated*/
	u32 special_reason_a:8; /*deprecated*/
	u32 gpio_duration_a:8; /*deprecated*/
	u32 gpio_pulse_count_a:8; /*deprecated*/

};

/**
 * @struct mac_ax_uphy_ctrl_info
 * @brief mac_ax_uphy_ctrl_info
 *
 * @var mac_ax_uphy_ctrl_info::disable_uphy
 * Please Place Description here.
 * @var mac_ax_uphy_ctrl_info::handshake_mode
 * Please Place Description here.
 * @var mac_ax_uphy_ctrl_info::rsvd0
 * Please Place Description here.
 * @var mac_ax_uphy_ctrl_info::rise_hst2dev_dis_uphy
 * Please Place Description here.
 * @var mac_ax_uphy_ctrl_info::uphy_dis_delay_unit
 * Please Place Description here.
 * @var mac_ax_uphy_ctrl_info::pdn_as_uphy_dis
 * Please Place Description here.
 * @var mac_ax_uphy_ctrl_info::pdn_to_enable_uphy
 * Please Place Description here.
 * @var mac_ax_uphy_ctrl_info::rsvd1
 * Please Place Description here.
 * @var mac_ax_uphy_ctrl_info::hst2dev_gpio_num
 * Please Place Description here.
 * @var mac_ax_uphy_ctrl_info::uphy_dis_delay_count
 * Please Place Description here.
 */
struct mac_ax_uphy_ctrl_info {
	u8 disable_uphy: 1;
	u8 handshake_mode: 3;
	u8 rsvd0: 4;
	u8 rise_hst2dev_dis_uphy: 1;
	u8 uphy_dis_delay_unit: 1;
	u8 pdn_as_uphy_dis: 1;
	u8 pdn_to_enable_uphy: 1;
	u8 rsvd1: 4;
	u8 hst2dev_gpio_num: 8;
	u8 uphy_dis_delay_count: 8;
};

/**
 * @struct mac_ax_wowcam_upd_info
 * @brief mac_ax_wowcam_upd_info
 *
 * @var mac_ax_wowcam_upd_info::r_w
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::idx
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::rsvd0
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::wkfm1
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::wkfm2
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::wkfm3
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::wkfm4
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::crc
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::rsvd1
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::negative_pattern_match
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::skip_mac_hdr
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::uc
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::mc
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::bc
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::rsvd2
 * Please Place Description here.
 * @var mac_ax_wowcam_upd_info::valid
 * Please Place Description here.
 */
struct mac_ax_wowcam_upd_info {
	u8 r_w: 1;
	u8 idx: 7;
	u8 rsvd0[3];
	u32 wkfm1: 32;
	u32 wkfm2: 32;
	u32 wkfm3: 32;
	u32 wkfm4: 32;
	u16 crc: 16;
	u8 rsvd1: 6;
	u8 negative_pattern_match: 1;
	u8 skip_mac_hdr: 1;
	u8 uc: 1;
	u8 mc: 1;
	u8 bc: 1;
	u8 rsvd2: 4;
	u8 valid: 1;
};

/*--------------------Define SET/GET HW VALUE struct -------------------------*/
/**
 * @struct mac_ax_dctl_seq_cfg
 * @brief mac_ax_dctl_seq_cfg
 *
 * @var seq0_val::seq0
 * Please Place Description here.
 * @var seq1_val::seq1
 * Please Place Description here.
 * @var seq2_val::seq2
 * Please Place Description here.
 * @var seq3_val::seq3
 * Please Place Description here.
 * @var hw_exseq_macid::ext macid
 * Please Place Description here.
 */
struct mac_ax_dctl_seq_cfg {
	u32 seq0_val;
	u32 seq1_val;
	u32 seq2_val;
	u32 seq3_val;
	u32 hw_exseq_macid;
};

/**
 * @struct mac_ax_dctl_seq_cfg
 * @brief mac_ax_dctl_seq_cfg
 *
 * @var qos_field_h::qos_field_h
 * Please Place Description here.
 * @var qos_field_h_en::qos_field_h_en
 * Please Place Description here.
 * @var mhdr_len::mhdr_len
 * Please Place Description here.
 * @var vlan_tag_valid::vlan_tag_valid
 * Please Place Description here.
 */
struct mac_ax_hdr_conv_dctl_cfg {
	u8 qos_field_h;
	u8 qos_field_h_en:1;
	u8 mhdr_len:5;
	u8 vlan_tag_valid:1;
	u8 rsvd:1;
};

/**
 * @struct mac_ax_sdio_info
 * @brief mac_ax_sdio_info
 *
 * @var mac_ax_sdio_info::sdio_4byte
 * Please Place Description here.
 * @var mac_ax_sdio_info::tx_mode
 * Please Place Description here.
 * @var mac_ax_sdio_info::spec_ver
 * Please Place Description here.
 * @var mac_ax_sdio_info::block_size
 * Please Place Description here.
 * @var mac_ax_sdio_info::tx_seq
 * Please Place Description here.
 * @var mac_ax_sdio_info::tx_align_size
 * Please Place Description here.
 * @var mac_ax_sdio_info::rpwm_bak
 * Please Place Description here.
 */
struct mac_ax_sdio_info {
	enum mac_ax_sdio_4byte_mode sdio_4byte;
	enum mac_ax_sdio_tx_mode tx_mode;
	enum mac_ax_sdio_spec_ver spec_ver;
	enum mac_ax_sdio_opn_mode opn_mode;
	u16 block_size;
	u8 tx_seq;
	u16 tx_align_size;
	u32 rpwm_bak;
};

/**
 * @struct mac_ax_sdio_txagg_cfg
 * @brief mac_ax_sdio_txagg_cfg
 *
 * @var mac_ax_sdio_txagg_cfg::en
 * Please Place Description here.
 * @var mac_ax_sdio_txagg_cfg::align_size
 * Please Place Description here.
 */
struct mac_ax_sdio_txagg_cfg {
	u8 en;
	u16 align_size;
};

/**
 * @struct mac_ax_usb_info
 * @brief mac_ax_usb_info
 *
 * @var mac_ax_usb_info::ep5
 * Please Place Description here.
 * @var mac_ax_usb_info::ep6
 * Please Place Description here.
 * @var mac_ax_usb_info::ep10
 * Please Place Description here.
 * @var mac_ax_usb_info::ep11
 * Please Place Description here.
 * @var mac_ax_usb_info::ep12
 * Please Place Description here.
 * @var mac_ax_usb_info::max_bulkout_wd_num
 * Please Place Description here.
 * @var mac_ax_usb_info::usb_mode
 * Please Place Description here.
 */
struct mac_ax_usb_info {
	u16 max_dma_txagg_msk;
	u8 ep5;
	u8 ep6;
	u8 ep10;
	u8 ep11;
	u8 ep12;
	u8 max_bulkout_wd_num;
	enum mac_ax_use_mode usb_mode;
};

/**
 * @struct mac_ax_usb_ep
 * @brief mac_ax_usb_ep
 *
 * @var mac_ax_usb_ep::band
 * Please Place Description here.

 */
struct mac_ax_usb_ep {
	u8 ep4;
	u8 ep5;
	u8 ep6;
	u8 ep7;
	u8 ep8;
	u8 ep9;
	u8 ep10;
	u8 ep11;
	u8 ep12;
	u8 ep13;
	u8 ep14;
	u8 ep15;
};

/**
 * @struct mac_ax_pcie_info
 * @brief mac_ax_pcie_info
 *
 * @var mac_ax_pcie_info::txbd_bndy
 * Please Place Description here.
 */
struct mac_ax_pcie_info {
	u16 txbd_bndy;
	u16 rxbd_bndy;
	u16 rpbd_bndy;
	u32 autok_total;
	u8 autok_2s_cnt;
};

struct mac_ax_flash_info {
	u8 read_done;
	u8 reading;
	u32 read_addr;
	u8 write_done;
	u8 writing;
	u32 write_addr;
	u8 erasing;
	u8 erase_done;
	u32 erase_addr;
	u8 *buf_addr;
	mac_ax_mutex lock;
};

/**
 * @struct mac_ax_fw_dbgcmd
 * @brief mac_ax_fw_dbgcmd
 *
 * @var mac_ax_fw_dbgcmd::buf
 * Please Place Description here.
 * @var mac_ax_fw_dbgcmd::out_len
 * Please Place Description here.
 * @var mac_ax_fw_dbgcmd::used
 * Please Place Description here.
 * @var mac_ax_fw_dbgcmd::cmd_idle
 * Please Place Description here.
 * @var mac_ax_fw_dbgcmd::lock
 * Please Place Description here.
 */
struct mac_ax_fw_dbgcmd {
	char *buf;
	u32 out_len;
	u32 used;
	u32 cmd_idle;
	mac_ax_mutex lock;
};

/**
 * @struct mac_ax_aval_page_cfg
 * @brief mac_ax_aval_page_cfg
 *
 * @var mac_ax_aval_page_cfg::thold_wd
 * Please Place Description here.
 * @var mac_ax_aval_page_cfg::thold_wp
 * Please Place Description here.
 * @var mac_ax_aval_page_cfg::ch_dma
 * Please Place Description here.
 * @var mac_ax_aval_page_cfg::en
 * Please Place Description here.
 */
struct mac_ax_aval_page_cfg {
	u32 thold_wd;
	u32 thold_wp;
	u8 ch_dma;
	u8 en;
};

/**
 * @struct mac_ax_rx_agg_cfg
 * @brief mac_ax_rx_agg_cfg
 *
 * @var mac_ax_rx_agg_cfg::mode
 * Please Place Description here.
 * @var mac_ax_rx_agg_cfg::thold
 * Please Place Description here.
 */
struct mac_ax_rx_agg_cfg {
	enum mac_ax_rx_agg_mode mode;
	struct mac_ax_rx_agg_thold thold;
};

/**
 * @struct mac_ax_usr_tx_rpt_cfg
 * @brief mac_ax_usr_tx_rpt_cfg
 *
 * @var mac_ax_usr_tx_rpt_cfg::mode
 * sel report mode
 * @var mac_ax_usr_tx_rpt_cfg::rpt_start
 * flag of first packet
 * @var mac_ax_usr_tx_rpt_cfg::macid
 * target macid
 * @var mac_ax_usr_tx_rpt_cfg::rpt_period_us
 * period of report, unit:us
 */
struct mac_ax_usr_tx_rpt_cfg {
	enum mac_ax_usr_tx_rpt_mode mode;
	u8 rpt_start;
	u8 macid;
	u8 band;
	u8 port;
	u32 rpt_period_us;
};

/**
 * @struct mac_ax_ofld_cfg
 * @brief disable ofld feature
 *
 * @var mac_ax_ofld_cfg::usr_edca_dis
 * 1: disable; 0:enable
 */
struct mac_ax_ofld_cfg {
	enum mac_ax_ofld_mode mode;
	u8 usr_txop_be;
	u16 usr_txop_be_val;
};

/**
 * @struct mac_ax_ac_edca_param
 * @brief mac_ax_ac_edca_param
 *
 * @var mac_ax_ac_edca_param::txop_32us
 * Please Place Description here.
 * @var mac_ax_ac_edca_param::ecw_max
 * Please Place Description here.
 * @var mac_ax_ac_edca_param::ecw_min
 * Please Place Description here.
 * @var mac_ax_ac_edca_param::aifs_us
 * Please Place Description here.
 */
struct mac_ax_ac_edca_param {
	u16 txop_32us;
	u8 ecw_max;
	u8 ecw_min;
	u8 aifs_us;
};

/**
 * @struct mac_ax_usr_edca_param
 * @brief mac_ax_usr_edca_param
 *
 * @var mac_ax_usr_edca_param::idx
 * Please Place Description here.
 * @var mac_ax_usr_edca_param::enable
 * Please Place Description here.
 * @var mac_ax_usr_edca_param::band
 * Please Place Description here.
 * @var mac_ax_usr_edca_param::wmm
 * Please Place Description here.
 * @var mac_ax_usr_edca_param::ac
 * Please Place Description here.
 * @var mac_ax_usr_edca_param::aggressive
 * Please Place Description here.
 * @var mac_ax_usr_edca_param::moderate
 * Please Place Description here.
 */
struct mac_ax_usr_edca_param {
	enum mac_ax_cmac_usr_edca_idx idx;
	u8 enable;
	u8 band;
	enum mac_ax_cmac_wmm_sel wmm;
	enum mac_ax_cmac_ac_sel ac;
	struct mac_ax_ac_edca_param aggressive;
	struct mac_ax_ac_edca_param moderate;
};

/**
 * @struct mac_ax_edca_param
 * @brief mac_ax_edca_param
 *
 * @var mac_ax_edca_param::band
 * Please Place Description here.
 * @var mac_ax_edca_param::path
 * Please Place Description here.
 * @var mac_ax_edca_param::txop_32us
 * Please Place Description here.
 * @var mac_ax_edca_param::ecw_max
 * Please Place Description here.
 * @var mac_ax_edca_param::ecw_min
 * Please Place Description here.
 * @var mac_ax_edca_param::aifs_us
 * Please Place Description here.
 */
struct mac_ax_edca_param {
	u8 band;
	enum mac_ax_cmac_path_sel path;
	u16 txop_32us;
	u8 ecw_max;
	u8 ecw_min;
	u8 aifs_us;
};

/**
 * @struct mac_ax_muedca_param
 * @brief mac_ax_muedca_param
 *
 * @var mac_ax_muedca_param::band
 * Please Place Description here.
 * @var mac_ax_muedca_param::ac
 * Please Place Description here.
 * @var mac_ax_muedca_param::muedca_timer_32us
 * Please Place Description here.
 * @var mac_ax_muedca_param::ecw_max
 * Please Place Description here.
 * @var mac_ax_muedca_param::ecw_min
 * Please Place Description here.
 * @var mac_ax_muedca_param::aifs_us
 * Please Place Description here.
 */
struct mac_ax_muedca_param {
	u8 band;
	enum mac_ax_cmac_ac_sel ac;
	u16 muedca_timer_32us;
	u8 ecw_max;
	u8 ecw_min;
	u8 aifs_us;
};

/**
 * @struct mac_ax_muedca_timer
 * @brief mac_ax_muedca_timer
 *
 * @var mac_ax_muedca_timer::band
 * Please Place Description here.
 * @var mac_ax_muedca_timer::ac
 * Please Place Description here.
 * @var mac_ax_muedca_timer::muedca_timer_32us
 * Please Place Description here.
 */
struct mac_ax_muedca_timer {
	u8 band;
	enum mac_ax_cmac_ac_sel ac;
	u16 muedca_timer_32us;
};

/**
 * @struct mac_ax_muedca_cfg
 * @brief mac_ax_muedca_cfg
 *
 * @var mac_ax_muedca_cfg::band
 * Please Place Description here.
 * @var mac_ax_muedca_cfg::wmm_sel
 * Please Place Description here.
 * @var mac_ax_muedca_cfg::countdown_en
 * Please Place Description here.
 * @var mac_ax_muedca_cfg::tb_update_en
 * Please Place Description here.
 */
struct mac_ax_muedca_cfg {
	u8 band;
	enum mac_ax_cmac_wmm_sel wmm_sel;
	u8 countdown_en;
	u8 tb_update_en;
};

/**
 * @struct mac_ax_sch_tx_en_cfg
 * @brief mac_ax_sch_tx_en_cfg
 *
 * @var mac_ax_sch_tx_en_cfg::band
 * Please Place Description here.
 * @var mac_ax_sch_tx_en_cfg::tx_en
 * Please Place Description here.
 * @var mac_ax_sch_tx_en_cfg::tx_en_mask
 * Please Place Description here.
 */
struct mac_ax_sch_tx_en_cfg {
	u8 band;
	struct mac_ax_sch_tx_en tx_en;
	struct mac_ax_sch_tx_en tx_en_mask;
};

struct mac_ax_tx_idle_poll_cfg {
	u8 band;
	enum mac_ax_tx_idle_poll_sel sel;
};

/**
 * @struct mac_ax_lifetime_cfg
 * @brief mac_ax_lifetime_cfg
 *
 * @var mac_ax_lifetime_cfg::band
 * Please Place Description here.
 * @var mac_ax_lifetime_cfg::en
 * Please Place Description here.
 * @var mac_ax_lifetime_cfg::val
 * Please Place Description here.
 */
struct mac_ax_lifetime_cfg {
	u8 band;
	struct mac_ax_lifetime_en en;
	struct mac_ax_lifetime_val val;
};

/**
 * @struct mac_ax_tb_ppdu_ctrl
 * @brief mac_ax_tb_ppdu_ctrl
 *
 * @var mac_ax_tb_ppdu_ctrl::band
 * Please Place Description here.
 * @var mac_ax_tb_ppdu_ctrl::pri_ac
 * Please Place Description here.
 * @var mac_ax_tb_ppdu_ctrl::be_dis
 * Please Place Description here.
 * @var mac_ax_tb_ppdu_ctrl::bk_dis
 * Please Place Description here.
 * @var mac_ax_tb_ppdu_ctrl::vi_dis
 * Please Place Description here.
 * @var mac_ax_tb_ppdu_ctrl::vo_dis
 * Please Place Description here.
 */
struct mac_ax_tb_ppdu_ctrl {
	u8 band;
	enum mac_ax_cmac_ac_sel pri_ac;
	u8 be_dis;
	u8 bk_dis;
	u8 vi_dis;
	u8 vo_dis;
};

/**
 * @struct mac_ax_sifs_r2t_t2t_ctrl
 * @brief mac_ax_sifs_r2t_t2t_ctrl
 *
 * @var mac_ax_sifs_r2t_t2t_ctrl::band
 * mac0/mac1.
 * @var mac_ax_sifs_r2t_t2t_ctrl::mactxen
 * time param.
 */
struct mac_ax_sifs_r2t_t2t_ctrl {
	u8 band;
	u32 mactxen;
};

/**
 * @struct macid_tx_bak
 * @brief macid_tx_bak
 *
 * @var macid_tx_bak::sch_bak
 * Please Place Description here.
 * @var macid_tx_bak::ac_dis_bak
 * Please Place Description here.
 */
struct macid_tx_bak {
	struct mac_ax_sch_tx_en_cfg sch_bak;
	struct mac_ax_tb_ppdu_ctrl ac_dis_bak;
};

/**
 * @struct mac_ax_edcca_param
 * @brief mac_ax_edcca_param
 *
 * @var mac_ax_edcca_param::band
 * Please Place Description here.
 * @var mac_ax_edcca_param::tb_check_en
 * Please Place Description here.
 * @var mac_ax_edcca_param::sifs_check_en
 * Please Place Description here.
 * @var mac_ax_edcca_param::ctn_check_en
 * Please Place Description here.
 * @var mac_ax_edcca_param::rsvd
 * Please Place Description here.
 * @var mac_ax_edcca_param::sel
 * Please Place Description here.
 */
struct mac_ax_edcca_param {
	u8 band:1;
	u8 tb_check_en:1;
	u8 sifs_check_en:1;
	u8 ctn_check_en:1;
	u8 rsvd:4;
	enum mac_ax_edcca_sel sel;
};

/**
 * @struct mac_ax_host_rpr_cfg
 * @brief mac_ax_host_rpr_cfg
 *
 * @var mac_ax_host_rpr_cfg::agg
 * Please Place Description here.
 * @var mac_ax_host_rpr_cfg::tmr
 * Please Place Description here.
 * @var mac_ax_host_rpr_cfg::agg_def
 * Please Place Description here.
 * @var mac_ax_host_rpr_cfg::tmr_def
 * Please Place Description here.
 * @var mac_ax_host_rpr_cfg::rsvd
 * Please Place Description here.
 * @var mac_ax_host_rpr_cfg::txok_en
 * Please Place Description here.
 * @var mac_ax_host_rpr_cfg::rty_lmt_en
 * Please Place Description here.
 * @var mac_ax_host_rpr_cfg::lft_drop_en
 * Please Place Description here.
 * @var mac_ax_host_rpr_cfg::macid_drop_en
 * Please Place Description here.
 */
struct mac_ax_host_rpr_cfg {
	u8 agg;
	u8 tmr;
	u8 agg_def:1;
	u8 tmr_def:1;
	u8 rsvd:5;
	enum mac_ax_func_sw txok_en;
	enum mac_ax_func_sw rty_lmt_en;
	enum mac_ax_func_sw lft_drop_en;
	enum mac_ax_func_sw macid_drop_en;
};

/**
 * @struct mac_ax_macid_pause_cfg
 * @brief mac_ax_macid_pause_cfg
 *
 * @var mac_ax_macid_pause_cfg::macid
 * Please Place Description here.
 * @var mac_ax_macid_pause_cfg::pause
 * Please Place Description here.
 */
struct mac_ax_macid_pause_cfg {
	u8 macid;
	u8 pause;
};

/**
 * @struct mac_ax_macid_pause_sleep_cfg
 * @brief mac_ax_macid_pause_sleep_cfg
 *
 * @var mac_ax_macid_pause_sleep_cfg::macid
 * Please Place Description here.
 * @var mac_ax_macid_pause_sleep_cfg::pause
 * Please Place Description here.
 * @var mac_ax_macid_pause_sleep_cfg::sleep
 * Please Place Description here.
 */
struct mac_ax_macid_pause_sleep_cfg {
	u8 macid;
	u8 pause;
	u8 sleep;
};

/**
 * @struct mac_ax_macid_pause_grp
 * @brief mac_ax_macid_pause_grp
 *
 * @var mac_ax_macid_pause_grp::pause_grp
 * Please Place Description here.
 * @var mac_ax_macid_pause_grp::mask_grp
 * Please Place Description here.
 */
struct mac_ax_macid_pause_grp {
	u32 pause_grp[4];
	u32 mask_grp[4];
};

/**
 * @struct mac_ax_macid_pause_sleep_grp
 * @brief mac_ax_macid_pause_sleep_grp
 *
 * @var mac_ax_macid_pause_sleep_grp::pause_grp
 * Please Place Description here.
 * @var mac_ax_macid_pause_sleep_grp::mask_grp
 * Please Place Description here.
 */
struct mac_ax_macid_pause_sleep_grp {
	u32 pause_grp[4];
	u32 pause_grp_mask[4];
	u32 sleep_grp[4];
	u32 sleep_grp_mask[4];
};

/**
 * @struct mac_ax_ampdu_cfg
 * @brief mac_ax_ampdu_cfg
 *
 * @var mac_ax_ampdu_cfg::band
 * Please Place Description here.
 * @var mac_ax_ampdu_cfg::wdbk_mode
 * Please Place Description here.
 * @var mac_ax_ampdu_cfg::rty_bk_mode
 * Please Place Description here.
 * @var mac_ax_ampdu_cfg::max_agg_num
 * Please Place Description here.
 * @var mac_ax_ampdu_cfg::max_agg_time_32us
 * Please Place Description here.
 */
struct mac_ax_ampdu_cfg {
	u8 band;
	enum mac_ax_wdbk_mode wdbk_mode;
	enum mac_ax_rty_bk_mode rty_bk_mode;
	u16 max_agg_num;
	u8 max_agg_time_32us;
};

/**
 * @struct mac_ax_ch_stat_cnt
 * @brief mac_ax_ch_stat_cnt
 *
 * @var mac_ax_ch_stat_cnt::band
 * Please Place Description here.
 * @var mac_ax_ch_stat_cnt::busy_cnt
 * Please Place Description here.
 * @var mac_ax_ch_stat_cnt::idle_cnt
 * Please Place Description here.
 */
struct mac_ax_ch_stat_cnt {
	u8 band;
	u32 busy_cnt;
	u32 idle_cnt;
};

/**
 * @struct mac_ax_ch_busy_cnt_cfg
 * @brief mac_ax_ch_busy_cnt_cfg
 *
 * @var mac_ax_ch_busy_cnt_cfg::band
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_cfg::cnt_ctrl
 * Please Place Description here.
 * @var mac_ax_ch_busy_cnt_cfg::ref
 * Please Place Description here.
 */
struct mac_ax_ch_busy_cnt_cfg {
	u8 band;
	enum mac_ax_ch_busy_cnt_ctrl cnt_ctrl;
	struct mac_ax_ch_busy_cnt_ref ref;
};

/**
 * @struct mac_ax_ss_wmm_tbl_ctrl
 * @brief mac_ax_ss_wmm_tbl_ctrl
 *
 * @var mac_ax_ss_wmm_tbl_ctrl::wmm
 * Please Place Description here.
 * @var mac_ax_ss_wmm_tbl_ctrl::wmm_mapping
 * Please Place Description here.
 */
struct mac_ax_ss_wmm_tbl_ctrl {
	u8 wmm;
	enum mac_ax_ss_wmm_tbl wmm_mapping;
};

/**
 * @struct mac_ax_bt_block_tx
 * @brief mac_ax_bt_block_tx
 *
 * @var mac_ax_bt_block_tx::band
 * Please Place Description here.
 * @var mac_ax_bt_block_tx::en
 * Please Place Description here.
 */
struct mac_ax_block_tx {
	u8 band;
	u8 en;
};

struct mac_ax_resp_chk_cca {
	u8 band;
	u8 resp_ack_chk_cca_en;
};

/**
 * @struct mac_ax_gt3_cfg
 * @brief mac_ax_gt3_cfg
 *
 * @var mac_ax_gt3_cfg::count_en
 * Please Place Description here.
 * @var mac_ax_gt3_cfg::mode
 * Please Place Description here.
 * @var mac_ax_gt3_cfg::gt3_en
 * Please Place Description here.
 * @var mac_ax_gt3_cfg::sort_en
 * Please Place Description here.
 * @var mac_ax_gt3_cfg::timeout
 * Please Place Description here.
 */
struct mac_ax_gt3_cfg {
	u32 count_en:1;
	u32 mode:1; /*0 = counter mode; 1 = timer mode"*/
	u32 gt3_en:1; /*HW clear when count down to zero in counter mode."*/
	u32 sort_en:1;
	u32 timeout:28;
};

/**
 * @struct mac_ax_rty_lmt
 * @brief mac_ax_rty_lmt
 *
 * @var mac_ax_rty_lmt::tx_cnt
 * Please Place Description here.
 * @var mac_ax_rty_lmt::macid
 * Please Place Description here.
 */
struct mac_ax_rty_lmt {
	u8 tx_cnt; //long tx cnt
	u8 short_tx_cnt;
	u8 macid;
};

/**
 * @struct mac_ax_cctl_rty_lmt_cfg
 * @brief mac_ax_cctl_rty_lmt_cfg
 *
 * @var mac_ax_cctl_rty_lmt_cfg::macid
 * Please Place Description here.
 * @var mac_ax_cctl_rty_lmt_cfg::data_lmt_sel
 * Please Place Description here.
 * @var mac_ax_cctl_rty_lmt_cfg::data_lmt_val
 * Please Place Description here.
 * @var mac_ax_cctl_rty_lmt_cfg::rsvd0
 * Please Place Description here.
 * @var mac_ax_cctl_rty_lmt_cfg::rts_lmt_sel
 * Please Place Description here.
 * @var mac_ax_cctl_rty_lmt_cfg::rts_lmt_val
 * Please Place Description here.
 * @var mac_ax_cctl_rty_lmt_cfg::rsvd1
 * Please Place Description here.
 */
struct mac_ax_cctl_rty_lmt_cfg {
	u8 macid;
	u8 data_lmt_sel:1;
	u8 data_lmt_val:6;
	u8 rsvd0:1;
	u8 rts_lmt_sel:1;
	u8 rts_lmt_val:4;
	u8 rsvd1:3;
};

/**
 * @struct mac_ax_cr_rty_lmt_cfg
 * @brief mac_ax_cr_rty_lmt_cfg
 *
 * @var mac_ax_cr_rty_lmt_cfg::long_tx_cnt_lmt
 * Please Place Description here.
 * @var mac_ax_cr_rty_lmt_cfg::short_tx_cnt_lmt
 * Please Place Description here.
 * @var mac_ax_cr_rty_lmt_cfg::band
 * Please Place Description here.
 */
struct mac_ax_cr_rty_lmt_cfg {
	u16 long_tx_cnt_lmt:6; /*CR: long rty*/
	u16 short_tx_cnt_lmt:6; /*CR: short rty*/
	enum mac_ax_band band;
};

/**
 * @struct mac_ax_rrsr_cfg
 * @brief mac_ax_rrsr_cfg
 *
 * @var mac_ax_rrsr_cfg::rrsr_rate_en
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::rsc
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::doppler_en
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::dcm_en
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::ref_rate_sel
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::ref_rate
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::cck_cfg
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::rsvd
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::ofdm_cfg
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::ht_cfg
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::vht_cfg
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::he_cfg
 * Please Place Description here.
 */
struct mac_ax_rrsr_cfg {
	u32 rrsr_rate_en:4;
	u32 rsc:2;
	u32 doppler_en:1;
	u32 dcm_en:1;
	u32 ref_rate_sel:1;
	u32 ref_rate:9;
	u32 cck_cfg:4;
	u32 ftm_rrsr_rate_en:4;
	u32 rsvd:6;

	u32 ofdm_cfg:8;
	u32 ht_cfg:8;
	u32 vht_cfg:8;
	u32 he_cfg:8;
	u8 band;
};

/**
 * @struct mac_ax_cts_rrsr_cfg
 * @brief mac_ax_cts_rrsr_cfg
 *
 * @var mac_ax_rrsr_cfg::cts_rrsr_rsc
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::cts_rrsr_opt
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::cts_rrsr_cck_cfg
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::cts_rrsr_ofdm_cfg
 * Please Place Description here.
 * @var mac_ax_rrsr_cfg::rsvd
 * Please Place Description here.
 */
struct mac_ax_cts_rrsr_cfg {
	u32 cts_rrsr_rsc:2;
	u32 cts_rrsr_opt:1;
	u32 cts_rrsr_cck_cfg:4;
	u32 cts_rrsr_ofdm_cfg:8;
	u32 rsvd:17;
	u8 band;
};

/**
 * @struct mac_ax_ss_quota_mode_ctrl
 * @brief mac_ax_ss_quota_mode_ctrl
 *
 * @var mac_ax_ss_quota_mode_ctrl::wmm
 * Please Place Description here.
 * @var mac_ax_ss_quota_mode_ctrl::mode
 * Please Place Description here.
 */
struct mac_ax_ss_quota_mode_ctrl {
	enum mac_ax_ss_wmm wmm;
	enum mac_ax_ss_quota_mode mode;
};

/**
 * @struct mac_ax_ss_quota_setting
 * @brief mac_ax_ss_quota_setting
 *
 * @var mac_ax_ss_quota_setting::macid
 * Please Place Description here.
 * @var mac_ax_ss_quota_setting::ac_type
 * Please Place Description here.
 * @var mac_ax_ss_quota_setting::val
 * Please Place Description here.
 * @var mac_ax_ss_quota_setting::ul_dl
 * Please Place Description here.
 */
struct mac_ax_ss_quota_setting {
	u8 macid;
	u8 ac_type;
	u8 val;
	enum mac_ax_issue_uldl_type ul_dl;
};

/**
 * @struct mac_ax_bt_polt_cnt
 * @brief mac_ax_bt_polt_cnt
 *
 * @var mac_ax_bt_polt_cnt::band
 * Please Place Description here.
 * @var mac_ax_bt_polt_cnt::cnt
 * Please Place Description here.
 */
struct mac_ax_bt_polt_cnt {
	u8 band;
	u16 cnt;
};

/**
 * @struct mac_ax_prebkf_setting
 * @brief mac_ax_prebkf_setting
 *
 * @var mac_ax_prebkf_setting::band
 * Please Place Description here.
 * @var mac_ax_prebkf_setting::val
 * Please Place Description here.
 */
struct mac_ax_prebkf_setting {
	u8 band;
	u8 val;
};

/**
 * @struct mac_ax_cctl_preld_cfg
 * @brief mac_ax_cctl_preld_cfg
 *
 * @var mac_ax_cctl_preld_cfg::macid
 * Please Place Description here.
 * @var mac_ax_cctl_preld_cfg::en
 * Please Place Description here.
 * @var mac_ax_cctl_preld_cfg::rsvd
 * Please Place Description here.
 */
struct mac_ax_cctl_preld_cfg {
	u8 macid;
	u8 en:1;
	u8 rsvd:7;
};

/*--------------------Define SRAM FIFO ---------------------------------------*/

/**
 * @struct mac_ax_avl_std_bacam_info
 * @brief  mac_ax_avl_std_bacam_info
 *
 * @var  mac_ax_avl_std_bacam_info::min_avl_idx
 * Please Place Description here.
 * @var  mac_ax_avl_std_bacam_info::max_avl_idx
 */
struct mac_ax_avl_std_bacam_info {
	u32 min_avl_idx: 7;
	u32 rsvd0: 9;
	u32 max_avl_idx: 7;
	u32 rsvd1: 9;
};

/**
 * @struct mac_ax_bacam_info
 * @brief mac_ax_bacam_info
 *
 * @var mac_ax_bacam_info::valid
 * Please Place Description here.
 * @var mac_ax_bacam_info::init_req
 * Please Place Description here.
 * @var mac_ax_bacam_info::entry_idx
 * Tha var is only for 8852A.
 * @var mac_ax_bacam_info::tid
 * Please Place Description here.
 * @var mac_ax_bacam_info::macid
 * Please Place Description here.
 * @var mac_ax_bacam_info::bmap_size
 * Please Place Description here.
 * @var mac_ax_bacam_info::ssn
 * Please Place Description here.
 * @var mac_ax_bacam_info::uid_value
 * The var is for 8852C.
 * @var mac_ax_bacam_info::std_entry_en
 * The var is for 8852C.
 * @var mac_ax_bacam_info::band_sel
 * The var is for 8852C.
 * @var mac_ax_bacam_info::entry_idx_v1
 * The var is for 8852C.
 */
struct mac_ax_bacam_info {
	u32 valid: 1;
	u32 init_req: 1;
	u32 entry_idx: 2;
	u32 tid: 4;
	u32 macid: 8;
	u32 bmap_size: 4;
	u32 ssn: 12;
	u32 uid_value: 8;
	u32 std_entry_en: 1;
	u32 band_sel: 1;
	u32 rsvd: 18;
	u32 entry_idx_v1: 4;
};

/**
 * @struct mac_ax_shcut_mhdr
 * @brief mac_ax_shcut_mhdr
 *
 * @var mac_ax_shcut_mhdr::mac_header_length
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword0
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword1
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword2
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword3
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword4
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword5
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword6
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword7
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword8
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword9
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword10
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword11
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword12
 * Please Place Description here.
 * @var mac_ax_shcut_mhdr::dword13
 * Please Place Description here.
 */
struct mac_ax_shcut_mhdr {/*need to revise note by kkbomb 0204*/
// dword 0
	u32 mac_header_length:8;
	u32 dword0:24;
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
	u32 dword6;
	u32 dword7;
	u32 dword8;
	u32 dword9;
	u32 dword10;
	u32 dword11;
	u32 dword12;
	u32 dword13;
};

/**
 * @struct mac_ax_fwstatus_payload
 * @brief mac_ax_fwstatus_payload
 *
 * @var mac_ax_fwstatus_payload::dword0
 * Please Place Description here.
 * @var mac_ax_fwstatus_payload::dword1
 * Please Place Description here.
 */
struct mac_ax_fwstatus_payload {
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
	u32 dword6;
	u32 dword7;
	u32 dword8;
	u32 dword9;
	u32 dword10;
	u32 dword11;
};

/**
 * @struct mac_ax_ie_cam_cmd_info
 * @brief mac_ax_ie_cam_cmd_info
 *
 * @var mac_ax_ie_cam_cmd_info::en
 * Please Place Description here.
 * @var mac_ax_ie_cam_cmd_info::band
 * Please Place Description here.
 * @var mac_ax_ie_cam_cmd_info::port
 * Please Place Description here.
 * @var mac_ax_ie_cam_cmd_info::hit_en
 * Please Place Description here.
 * @var mac_ax_ie_cam_cmd_info::miss_en
 * Please Place Description here.
 * @var mac_ax_ie_cam_cmd_info::rst
 * Please Place Description here.
 * @var mac_ax_ie_cam_cmd_info::hit_sel
 * Please Place Description here.
 * @var mac_ax_ie_cam_cmd_info::miss_sel
 * Please Place Description here.
 * @var mac_ax_ie_cam_cmd_info::rsvd0
 * Please Place Description here.
 * @var mac_ax_ie_cam_cmd_info::num
 * Please Place Description here.
 * @var mac_ax_ie_cam_cmd_info::rsvd1
 * Please Place Description here.
 * @var mac_ax_ie_cam_cmd_info::buf
 * Please Place Description here.
 * @var mac_ax_ie_cam_cmd_info::buf_len
 * Please Place Description here.
 */
struct mac_ax_ie_cam_cmd_info {
	u8 en:1;
	u8 band:1;
	u8 port:3;
	u8 hit_en:1;
	u8 miss_en:1;
	u8 rst:1;
	u8 hit_sel:2;
	u8 miss_sel:2;
	u8 rsvd0:4;
	u8 num:5;
	u8 rsvd1:3;
	u8 *buf;
	u32 buf_len;
};

/**
 * @struct mac_ax_addr_cam_info
 * @brief mac_ax_addr_cam_info
 *
 * @var mac_ax_addr_cam_info::addr_cam_idx
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::offset
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::len
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::valid
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::net_type
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::bcn_hit_cond
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::hit_rule
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::bb_sel
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::addr_mask
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::mask_sel
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::bssid_cam_idx
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::is_mul_ent
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::sma
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::tma
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::macid
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::port_int
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::tsf_sync
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::tf_trs
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::lsig_txop
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::tgt_ind
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::frm_tgt_ind
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::aid12
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::wol_pattern
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::wol_uc
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::wol_magic
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::wapi
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::sec_ent_mode
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::sec_ent_keyid
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::sec_ent_valid
 * Please Place Description here.
 * @var mac_ax_addr_cam_info::sec_ent
 * Please Place Description here.
 */
struct mac_ax_addr_cam_info {
	u8 addr_cam_idx;	/* Addr cam entry index */
	u8 offset;		/* Offset */
	u8 len;			/* Length */
	u8 valid : 1;
	u8 net_type : 2;
	u8 bcn_hit_cond : 2;
	u8 hit_rule : 2;
	u8 bb_sel : 1;
	u8 addr_mask : 6;
	u8 mask_sel : 2;
	u8 bssid_cam_idx : 6;
	u8 is_mul_ent : 1;
	u8 sma[6];
	u8 tma[6];
	u8 macid;
	u8 port_int: 3;
	u8 tsf_sync: 3;
	u8 tf_trs: 1;
	u8 lsig_txop: 1;
	u8 tgt_ind: 3;
	u8 frm_tgt_ind: 3;
	u16 aid12: 12;
	u8 wol_pattern: 1;
	u8 wol_uc: 1;
	u8 wol_magic: 1;
	u8 wapi: 1;
	u8 sec_ent_mode: 2;
	u8 sec_ent_keyid[7];
	u8 sec_ent_valid;
	u8 sec_ent[7];
};

/**
 * @struct mac_ax_bssid_cam_info
 * @brief mac_ax_bssid_cam_info
 *
 * @var mac_ax_bssid_cam_info::bssid_cam_idx
 * Please Place Description here.
 * @var mac_ax_bssid_cam_info::offset
 * Please Place Description here.
 * @var mac_ax_bssid_cam_info::len
 * Please Place Description here.
 * @var mac_ax_bssid_cam_info::valid
 * Please Place Description here.
 * @var mac_ax_bssid_cam_info::bb_sel
 * Please Place Description here.
 * @var mac_ax_bssid_cam_info::bss_color
 * Please Place Description here.
 * @var mac_ax_bssid_cam_info::bssid
 * Please Place Description here.
 */
struct mac_ax_bssid_cam_info {
	u8 bssid_cam_idx;	/* BSSID cam entry index */
	u8 offset;		/* Offset */
	u8 len;			/* Length */
	u8 valid : 1;
	u8 bb_sel : 1;
	u8 addr_mask : 6;
	u8 bss_color : 7;
	u8 bssid[6];
};

/**
 * @struct mac_ax_sec_cam_info
 * @brief mac_ax_sec_cam_info
 *
 * @var mac_ax_sec_cam_info::sec_cam_idx
 * Please Place Description here.
 * @var mac_ax_sec_cam_info::offset
 * Please Place Description here.
 * @var mac_ax_sec_cam_info::len
 * Please Place Description here.
 * @var mac_ax_sec_cam_info::type
 * Please Place Description here.
 * @var mac_ax_sec_cam_info::ext_key
 * Please Place Description here.
 * @var mac_ax_sec_cam_info::spp_mode
 * Please Place Description here.
 * @var mac_ax_sec_cam_info::key
 * Please Place Description here.
 */
struct mac_ax_sec_cam_info {
	u8 sec_cam_idx;		/* Security cam entry index */
	u8 offset;		/* Offset */
	u8 len;			/* Length */
	u8 type : 4;
	u8 ext_key : 1;
	u8 spp_mode : 1;
	u32 key[4];
};

struct mac_ax_sec_iv_info {
	u8 ptktxiv[IV_LENGTH];	/* Security IV */
	u8 macid;
	u8 opcode;
};

/**
 * @struct mac_ax_macaddr
 * @brief mac_ax_macaddr
 *
 * @var mac_ax_macaddr::macaddr
 * Please Place Description here.
 */
struct mac_ax_macaddr {
	u8 macaddr[6];
};

/**
 * @struct mac_ax_sta_init_info
 * @brief mac_ax_sta_init_info
 *
 * @var mac_ax_sta_init_info::macid
 * Please Place Description here.
 * @var mac_ax_sta_init_info::opmode
 * Please Place Description here.
 * @var mac_ax_sta_init_info::band
 * Please Place Description here.
 * @var mac_ax_sta_init_info::wmm
 * Please Place Description here.
 * @var mac_ax_sta_init_info::trigger
 * Please Place Description here.
 * @var mac_ax_sta_init_info::is_hesta
 * Please Place Description here.
 * @var mac_ax_sta_init_info::dl_bw
 * Please Place Description here.
 * @var mac_ax_sta_init_info::tf_mac_padding
 * Please Place Description here.
 * @var mac_ax_sta_init_info::dl_t_pe
 * Please Place Description here.
 * @var mac_ax_sta_init_info::port_id
 * Please Place Description here.
 * @var mac_ax_sta_init_info::net_type
 * Please Place Description here.
 * @var mac_ax_sta_init_info::wifi_role
 * Please Place Description here.
 * @var mac_ax_sta_init_info::self_role
 * Please Place Description here.
 */
struct mac_ax_sta_init_info {
	u8 macid;
	u8 opmode:1;
	u8 band:1;
	u8 wmm:2;
	u8 trigger:1;
	u8 is_hesta: 1;
	u8 dl_bw: 2;
	u8 tf_mac_padding:2;
	u8 dl_t_pe:3;
	u8 port_id:3;
	u8 net_type:2;
	u8 wifi_role:4;
	u8 self_role:2;
};

/**
 * @struct mac_ax_fwrole_maintain
 * @brief mac_ax_fwrole_maintain
 *
 * @var mac_ax_fwrole_maintain::macid
 * Please Place Description here.
 * @var mac_ax_fwrole_maintain::self_role
 * Please Place Description here.
 * @var mac_ax_fwrole_maintain::upd_mode
 * Please Place Description here.
 * @var mac_ax_fwrole_maintain::wifi_role
 * Please Place Description here.
 */
struct mac_ax_fwrole_maintain {
	u8 macid;
	u8 self_role : 2;
	u8 upd_mode : 3;
	u8 wifi_role : 4;
	u8 band: 2;
	u8 port: 3;
};

struct mac_ax_cctl_info {
	/* dword 0 */
	u32 datarate:9;
	u32 force_txop:1;
	u32 data_bw:2;
	u32 data_gi_ltf:3;
	u32 darf_tc_index:1;
	u32 arfr_ctrl:4;
	u32 acq_rpt_en:1;
	u32 mgq_rpt_en:1;
	u32 ulq_rpt_en:1;
	u32 twtq_rpt_en:1;
	u32 rsvd0:1;
	u32 disrtsfb:1;
	u32 disdatafb:1;
	u32 tryrate:1;
	u32 ampdu_density:4;
	/* dword 1 */
	u32 data_rty_lowest_rate:9;
	u32 ampdu_time_sel:1;
	u32 ampdu_len_sel:1;
	u32 rts_txcnt_lmt_sel:1;
	u32 rts_txcnt_lmt:4;
	u32 rtsrate:9;
	u32 rsvd1:2;
	u32 vcs_stbc:1;
	u32 rts_rty_lowest_rate:4;
	/* dword 2 */
	u32 data_tx_cnt_lmt:6;
	u32 data_txcnt_lmt_sel:1;
	u32 max_agg_num_sel:1;
	u32 rts_en:1;
	u32 cts2self_en:1;
	u32 cca_rts:2;
	u32 hw_rts_en:1;
	u32 rts_drop_data_mode:2;
	u32 preld_en:1;
	u32 ampdu_max_len:11;
	u32 ul_mu_dis:1;
	u32 ampdu_max_time:4;
	/* dword 3 */
	u32 max_agg_num:9;
	u32 ba_bmap:2;
	u32 rsvd3:5;
	u32 vo_lftime_sel:3;
	u32 vi_lftime_sel:3;
	u32 be_lftime_sel:3;
	u32 bk_lftime_sel:3;
	u32 sectype:4;
	/* dword 4 */
	u32 multi_port_id:3;
	u32 bmc:1;
	u32 mbssid:4;
	u32 navusehdr:1;
	u32 txpwr_mode:3;
	u32 data_dcm:1;
	u32 data_er:1;
	u32 data_ldpc:1;
	u32 data_stbc:1;
	u32 a_ctrl_bqr:1;
	u32 a_ctrl_uph:1;
	u32 a_ctrl_bsr:1;
	u32 a_ctrl_cas:1;
	u32 data_bw_er:1;
	u32 lsig_txop_en:1;
	u32 rsvd4:5;
	u32 ctrl_cnt_vld:1;
	u32 ctrl_cnt:4;
	/* dword 5 */
	u32 resp_ref_rate:9;
	u32 rsvd5:3;
	u32 all_ack_support:1;
	u32 bsr_queue_size_format:1;
	u32 bsr_om_upd_en:1;
	u32 macid_fwd_idc:1;
	u32 ntx_path_en:4;
	u32 path_map_a:2;
	u32 path_map_b:2;
	u32 path_map_c:2;
	u32 path_map_d:2;
	u32 antsel_a:1;
	u32 antsel_b:1;
	u32 antsel_c:1;
	u32 antsel_d:1;
	/* dword 6 */
	u32 addr_cam_index:8;
	u32 paid:9;
	u32 uldl:1;
	u32 doppler_ctrl:2;
	u32 nominal_pkt_padding:2;
	u32 nominal_pkt_padding40:2;
	u32 txpwr_tolerence:6;
	//u32 rsvd9:2;
	u32 nominal_pkt_padding80:2;
	/* dword 7 */
	u32 nc:3;
	u32 nr:3;
	u32 ng:2;
	u32 cb:2;
	u32 cs:2;
	u32 csi_txbf_en:1;
	u32 csi_stbc_en:1;
	u32 csi_ldpc_en:1;
	u32 csi_para_en:1;
	u32 csi_fix_rate:9;
	u32 csi_gi_ltf:3;
	u32 nominal_pkt_padding160:2;
	u32 csi_bw:2;
};

/**
 * @struct mac_ax_dctl_info
 * @brief mac_ax_dctl_info
 *
 * @var mac_ax_dctl_info::qos_field_h
 * Please Place Description here.
 * @var mac_ax_dctl_info::hw_exseq_macid
 * Please Place Description here.
 * @var mac_ax_dctl_info::qos_field_h_en
 * Please Place Description here.
 * @var mac_ax_dctl_info::aes_iv_l
 * Please Place Description here.
 * @var mac_ax_dctl_info::aes_iv_h
 * Please Place Description here.
 * @var mac_ax_dctl_info::seq0
 * Please Place Description here.
 * @var mac_ax_dctl_info::seq1
 * Please Place Description here.
 * @var mac_ax_dctl_info::amsdu_max_length
 * Please Place Description here.
 * @var mac_ax_dctl_info::sta_amsdu_en
 * Please Place Description here.
 * @var mac_ax_dctl_info::chksum_offload_en
 * Please Place Description here.
 * @var mac_ax_dctl_info::with_llc
 * Please Place Description here.
 * @var mac_ax_dctl_info::rsvd0
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_hw_enc
 * Please Place Description here.
 * @var mac_ax_dctl_info::seq2
 * Please Place Description here.
 * @var mac_ax_dctl_info::seq3
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_cam_idx
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent0_keyid
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent1_keyid
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent2_keyid
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent3_keyid
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent4_keyid
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent5_keyid
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent6_keyid
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent_valid
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent0
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent1
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent2
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent3
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent4
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent5
 * Please Place Description here.
 * @var mac_ax_dctl_info::sec_ent6
 * Please Place Description here.
 * @var mac_ax_dctl_info::rsvd3
 * Please Place Description here.
 */
struct mac_ax_dctl_info {
	/* dword 0 */
	u32 qos_field_h:8;
	u32 hw_exseq_macid:7;
	u32 qos_field_h_en:1; // qos_data
	u32 aes_iv_l:16;
	/* dword 1 */
	u32 aes_iv_h:32;
	/* dword 2 */
	u32 seq0:12;
	u32 seq1:12;
	u32 amsdu_max_length:3;
	u32 sta_amsdu_en:1;
	u32 chksum_offload_en:1;
	u32 with_llc:1;
	u32 rsvd0:1;
	u32 sec_hw_enc:1;
	/* dword 3 */
	u32 seq2:12;
	u32 seq3:12;
	u32 sec_cam_idx:8;
	/* dword 4 */
	u32 mhdr_len:5;
	u32 vlan_tag_valid:1;
	u32 vlan_tag_sel:2;
	u32 htc_order:1;
	u32 sec_key_id:2;
	u32 rsvd2:4;
	u32 wapi:1;
	u32 sec_ent_mode:2;
	u32 sec_ent0_keyid:2;
	u32 sec_ent1_keyid:2;
	u32 sec_ent2_keyid:2;
	u32 sec_ent3_keyid:2;
	u32 sec_ent4_keyid:2;
	u32 sec_ent5_keyid:2;
	u32 sec_ent6_keyid:2;
	/* dword 5 */
	u32 sec_ent_valid:8;
	u32 sec_ent0:8;
	u32 sec_ent1:8;
	u32 sec_ent2:8;
	/* dword 6 */
	u32 sec_ent3:8;
	u32 sec_ent4:8;
	u32 sec_ent5:8;
	u32 sec_ent6:8;
	/* dword 7 */
	u32 tgt_ind:4;    // for 52c dw3
	u32 tgt_ind_en:1; // for 52c dw3
	u32 htc_lb:3;     // for 52c dw3
	u32 rsvd3:24;
};

/**
 * struct mac_ax_role_info - role information
 * @macid: MAC ID.
 * @band: Band selection, band0 or band1.
 * @wmm: WMM selection, wmm0 ow wmm1.
 *	There are four sets about band and wmm,
 *	band0+wmm0, band0+wmm1, band1+wmm0,band1+wmm1.
 */

/**
 * @struct mac_ax_role_info
 * @brief mac_ax_role_info
 *
 * @var mac_ax_role_info::self_role
 * Please Place Description here.
 * @var mac_ax_role_info::wifi_role
 * Please Place Description here.
 * @var mac_ax_role_info::net_type
 * Please Place Description here.
 * @var mac_ax_role_info::upd_mode
 * Please Place Description here.
 * @var mac_ax_role_info::opmode
 * Please Place Description here.
 * @var mac_ax_role_info::band
 * Please Place Description here.
 * @var mac_ax_role_info::port
 * Please Place Description here.
 * @var mac_ax_role_info::macid
 * Please Place Description here.
 * @var mac_ax_role_info::self_mac
 * Please Place Description here.
 * @var mac_ax_role_info::target_mac
 * Please Place Description here.
 * @var mac_ax_role_info::bssid
 * Please Place Description here.
 * @var mac_ax_role_info::bss_color
 * Please Place Description here.
 * @var mac_ax_role_info::bcn_hit_cond
 * Please Place Description here.
 * @var mac_ax_role_info::hit_rule
 * Please Place Description here.
 * @var mac_ax_role_info::is_mul_ent
 * Please Place Description here.
 * @var mac_ax_role_info::tsf_sync
 * Please Place Description here.
 * @var mac_ax_role_info::trigger
 * Please Place Description here.
 * @var mac_ax_role_info::lsig_txop
 * Please Place Description here.
 * @var mac_ax_role_info::tgt_ind
 * Please Place Description here.
 * @var mac_ax_role_info::frm_tgt_ind
 * Please Place Description here.
 * @var mac_ax_role_info::wol_pattern
 * Please Place Description here.
 * @var mac_ax_role_info::wol_uc
 * Please Place Description here.
 * @var mac_ax_role_info::wol_magic
 * Please Place Description here.
 * @var mac_ax_role_info::wapi
 * Please Place Description here.
 * @var mac_ax_role_info::sec_ent_mode
 * Please Place Description here.
 * @var mac_ax_role_info::wmm
 * Please Place Description here.
 * @var mac_ax_role_info::dbcc_role
 * Please Place Description here.
 * @var mac_ax_role_info::is_hesta
 * Please Place Description here.
 * @var mac_ax_role_info::dl_bw
 * Please Place Description here.
 * @var mac_ax_role_info::tf_mac_padding
 * Please Place Description here.
 * @var mac_ax_role_info::dl_t_pe
 * Please Place Description here.
 * @var mac_ax_role_info::aid
 * Please Place Description here.
 * @var mac_ax_role_info::a_info
 * Please Place Description here.
 * @var mac_ax_role_info::b_info
 * Please Place Description here.
 * @var mac_ax_role_info::s_info
 * Please Place Description here.
 * @var mac_ax_role_info::c_info
 * Please Place Description here.
 */
struct mac_ax_role_info {
	enum mac_ax_self_role self_role;
	enum mac_ax_wifi_role wifi_role;
	enum mac_ax_net_type net_type;
	enum mac_ax_upd_mode upd_mode;
	enum mac_ax_opmode opmode;
	enum mac_ax_band band;
	enum mac_ax_port port;
	enum mac_ax_addr_msk_sel mask_sel;
	enum mac_ax_addr_msk addr_mask;
	u8 macid;
	u8 self_mac[6];
	u8 target_mac[6];
	u8 bssid[6];

	u8 bss_color:6;
	u8 bcn_hit_cond:2;

	u8 hit_rule:2;
	u8 is_mul_ent:1;
	u8 tsf_sync:3;
	u8 trigger:1;
	u8 lsig_txop:1;

	u8 tgt_ind:3;
	u8 frm_tgt_ind:3;
	u8 wol_pattern:1;
	u8 wol_uc:1;

	u8 wol_magic:1;
	u8 wapi:1;
	u8 sec_ent_mode:2;
	u8 wmm:2;
	u8 dbcc_role:1;
	u8 rsvd:1;

	u8 is_hesta:1;
	u8 dl_bw:2;
	u8 tf_mac_padding:2;
	u8 dl_t_pe: 3;

	u16 aid;
	struct mac_ax_addr_cam_info a_info;
	struct mac_ax_bssid_cam_info b_info;
	struct mac_ax_sec_cam_info s_info;
	struct rtw_hal_mac_ax_cctl_info c_info;
};

/**
 * @struct mac_role_tbl
 * @brief mac_role_tbl
 *
 * @var mac_role_tbl::next
 * Please Place Description here.
 * @var mac_role_tbl::prev
 * Please Place Description here.
 * @var mac_role_tbl::info
 * Please Place Description here.
 * @var mac_role_tbl::macid
 * Please Place Description here.
 * @var mac_role_tbl::wmm
 * Please Place Description here.
 */
struct mac_role_tbl {
	/* keep first */
	struct mac_role_tbl *next;
	struct mac_role_tbl *prev;
	struct mac_ax_role_info info;
	u8 macid;
	u8 wmm;
};

/**
 * @struct mac_role_tbl_head
 * @brief mac_role_tbl_head
 *
 * @var mac_role_tbl_head::next
 * Please Place Description here.
 * @var mac_role_tbl_head::prev
 * Please Place Description here.
 * @var mac_role_tbl_head::role_tbl_pool
 * Please Place Description here.
 * @var mac_role_tbl_head::qlen
 * Please Place Description here.
 * @var mac_role_tbl_head::lock
 * Please Place Description here.
 */
struct mac_role_tbl_head {
	/* keep first */
	struct mac_role_tbl *next;
	struct mac_role_tbl *prev;
	struct mac_role_tbl_head *role_tbl_pool;
	u32 qlen;
	mac_ax_mutex lock;
};

/**
 * @struct mac_ax_coex
 * @brief mac_ax_coex
 *
 * @var mac_ax_coex::pta_mode
 * Please Place Description here.
 * @var mac_ax_coex::direction
 * Please Place Description here.
 */
struct mac_ax_coex {
#define MAC_AX_COEX_RTK_MODE 0
#define MAC_AX_COEX_CSR_MODE 1
	u8 pta_mode;
#define MAC_AX_COEX_INNER 0
#define MAC_AX_COEX_OUTPUT 1
#define MAC_AX_COEX_INPUT 2
	u8 direction;
};

/**
 * @struct mac_ax_port_tsf
 * @brief mac_ax_port_tsf
 *
 * @var mac_ax_port_tsf::tsf_l
 * Please Place Description here.
 * @var mac_ax_port_tsf::tsf_h
 * Please Place Description here.
 * @var mac_ax_port_tsf::port
 * Please Place Description here.
 * @var mac_ax_port_tsf::band
 * Please Place Description here.
 */
struct mac_ax_port_tsf {
	u32 tsf_l;
	u32 tsf_h;
	u8 port;
	u8 band;
};

/**
 * @struct mac_ax_freerun
 * @brief mac_ax_freerun
 *
 * @var mac_ax_freerun::freerun_l
 * Please Place Description here.
 * @var mac_ax_freerun::freerun_h
 * Please Place Description here.
 * @var mac_ax_freerun::band
 * Please Place Description here.
 */
struct mac_ax_freerun {
	u32 freerun_l;
	u32 freerun_h;
	u8 band;
};

/**
 * @struct mac_ax_gnt
 * @brief mac_ax_gnt
 *
 * @var mac_ax_gnt::gnt_bt_sw_en
 * Please Place Description here.
 * @var mac_ax_gnt::gnt_bt
 * Please Place Description here.
 * @var mac_ax_gnt::gnt_wl_sw_en
 * Please Place Description here.
 * @var mac_ax_gnt::gnt_wl
 * Please Place Description here.
 */
struct mac_ax_gnt {
	u8 gnt_bt_sw_en;
	u8 gnt_bt;
	u8 gnt_wl_sw_en;
	u8 gnt_wl;
};

/**
 * @struct mac_ax_coex_gnt
 * @brief mac_ax_coex_gnt
 *
 * @var mac_ax_coex_gnt::band0
 * Please Place Description here.
 * @var mac_ax_coex_gnt::band1
 * Please Place Description here.
 */
struct mac_ax_coex_gnt {
	struct mac_ax_gnt band0;
	struct mac_ax_gnt band1;
};

/**
 * @struct mac_ax_plt
 * @brief mac_ax_plt
 *
 * @var mac_ax_plt::band
 * Please Place Description here.
 * @var mac_ax_plt::tx
 * Please Place Description here.
 * @var mac_ax_plt::rx
 * Please Place Description here.
 */
struct mac_ax_plt {
#define MAC_AX_PLT_LTE_RX BIT(0)
#define MAC_AX_PLT_GNT_BT_TX BIT(1)
#define MAC_AX_PLT_GNT_BT_RX BIT(2)
#define MAC_AX_PLT_GNT_WL BIT(3)
	u8 band;
	u8 tx;
	u8 rx;
};

/**
 * @struct mac_ax_rx_cnt
 * @brief mac_ax_rx_cnt
 *
 * @var mac_ax_rx_cnt::type
 * Please Place Description here.
 * @var mac_ax_rx_cnt::op
 * Please Place Description here.
 * @var mac_ax_rx_cnt::idx
 * Please Place Description here.
 * @var mac_ax_rx_cnt::band
 * Please Place Description here.
 * @var mac_ax_rx_cnt::buf
 * Please Place Description here.
 */
struct mac_ax_rx_cnt {
#define MAC_AX_RX_CRC_OK 0
#define MAC_AX_RX_CRC_FAIL 1
#define MAC_AX_RX_FA 2
#define MAC_AX_RX_PPDU 3
#define MAC_AX_RX_IDX 4
	u8 type;
#define MAC_AX_RXCNT_R 0
#define MAC_AX_RXCNT_RST_ALL 1
	u8 op;
	u8 idx;
	u8 band;
	u16 *buf;
};

/**
 * @struct mac_ax_tx_cnt
 * @brief mac_ax_tx_cnt
 *
 * @var mac_ax_tx_cnt::band
 * Please Place Description here.
 * @var mac_ax_tx_cnt::sel
 * Please Place Description here.
 * @var mac_ax_tx_cnt::txcnt
 * Please Place Description here.
 */
struct mac_ax_tx_cnt {
#define MAC_AX_TX_LCCK 0
#define MAC_AX_TX_SCCK 1
#define MAC_AX_TX_OFDM 2
#define MAC_AX_TX_HT 3
#define MAC_AX_TX_HTGF 4
#define MAC_AX_TX_VHTSU 5
#define MAC_AX_TX_VHTMU 6
#define MAC_AX_TX_HESU 7
#define MAC_AX_TX_HEERSU 8
#define MAC_AX_TX_HEMU 9
#define MAC_AX_TX_HETB 10
#define MAC_AX_TX_ALLTYPE 11
	u8 band;
	u8 sel;
	u16 txcnt[MAC_AX_TX_ALLTYPE];
};

/**
 * @struct mac_ax_mcc_role
 * @brief mac_ax_mcc_role
 *
 * @var mac_ax_mcc_role::macid
 * Please Place Description here.
 * @var mac_ax_mcc_role::central_ch_seg0
 * Please Place Description here.
 * @var mac_ax_mcc_role::central_ch_seg1
 * Please Place Description here.
 * @var mac_ax_mcc_role::primary_ch
 * Please Place Description here.
 * @var mac_ax_mcc_role::bandwidth
 * Please Place Description here.
 * @var mac_ax_mcc_role::group
 * Please Place Description here.
 * @var mac_ax_mcc_role::c2h_rpt
 * Please Place Description here.
 * @var mac_ax_mcc_role::dis_tx_null
 * Please Place Description here.
 * @var mac_ax_mcc_role::dis_sw_retry
 * Please Place Description here.
 * @var mac_ax_mcc_role::in_curr_ch
 * Please Place Description here.
 * @var mac_ax_mcc_role::sw_retry_count
 * Please Place Description here.
 * @var mac_ax_mcc_role::tx_null_early
 * Please Place Description here.
 * @var mac_ax_mcc_role::rsvd0
 * Please Place Description here.
 * @var mac_ax_mcc_role::duration
 * Please Place Description here.
 */
struct mac_ax_mcc_role {
	/* dword0 */
	u32 macid: 8;
	u32 central_ch_seg0: 8;
	u32 central_ch_seg1: 8;
	u32 primary_ch: 8;
	/* dword1 */
	enum channel_width bandwidth: 4;
	u32 group: 2;
#define MCC_C2H_RPT_OFF 0
#define MCC_C2H_RPT_FAIL_ONLY 1
#define MCC_C2H_RPT_ALL 2
	u32 c2h_rpt: 2;
	u32 dis_tx_null: 1;
	u32 dis_sw_retry: 1;
	u32 in_curr_ch: 1;
	u32 sw_retry_count: 3;
	u32 tx_null_early: 4;
	u32 btc_in_2g: 1;
	u32 pta_en: 1;
	u32 rfk_by_pass: 1;
	u32 ch_band_type: 2;
	u32 rsvd0: 9;
	/* dword2 */
	u32 duration: 32;
	/* dword3 */
	u8 courtesy_en;
	u8 courtesy_num;
	u8 courtesy_target;
	u8 rsvd1;
};

struct mac_ax_mcc_start {
	/* dword0 */
	u32 group: 2;
	u32 btc_in_group: 1;
	u32 old_group_action: 2;
	u32 old_group:2;
	u32 rsvd0:9;
	u32 notify_cnt:3;
	u32 rsvd1:2;
	u32 notify_rxdbg_en:1;
	u32 rsvd2:2;
	u32 macid: 8;
	/* dword1 */
	u32 tsf_low;
	/* dword2 */
	u32 tsf_high;
};

/**
 * @struct mac_ax_mcc_duration_info
 * @brief mac_ax_mcc_duration_info
 *
 * @var mac_ax_mcc_duration_info::group
 * Please Place Description here.
 * @var mac_ax_mcc_duration_info::rsvd0
 * Please Place Description here.
 * @var mac_ax_mcc_duration_info::start_macid
 * Please Place Description here.
 * @var mac_ax_mcc_duration_info::macid_x
 * Please Place Description here.
 * @var mac_ax_mcc_duration_info::macid_y
 * Please Place Description here.
 * @var mac_ax_mcc_duration_info::start_tsf_low
 * Please Place Description here.
 * @var mac_ax_mcc_duration_info::start_tsf_high
 * Please Place Description here.
 * @var mac_ax_mcc_duration_info::duration_x
 * Please Place Description here.
 * @var mac_ax_mcc_duration_info::duration_y
 * Please Place Description here.
 */
struct mac_ax_mcc_duration_info {
	/* dword0 */
	u32 group: 2;
	u32 btc_in_group:1;
	u32 rsvd0: 5;
	u32 start_macid: 8;
	u32 macid_x: 8;
	u32 macid_y: 8;
	/* dword1 */
	u32 start_tsf_low;
	/* dword2 */
	u32 start_tsf_high;
	/* dword3 */
	u32 duration_x;
	/* dword4 */
	u32 duration_y;
};

/**
 * @struct mac_ax_mcc_group
 * @brief mac_ax_mcc_group
 *
 * @var mac_ax_mcc_group::rpt_status
 * Please Place Description here.
 * @var mac_ax_mcc_group::rpt_macid
 * Please Place Description here.
 * @var mac_ax_mcc_group::macid_x
 * Please Place Description here.
 * @var mac_ax_mcc_group::macid_y
 * Please Place Description here.
 * @var mac_ax_mcc_group::rpt_tsf_high
 * Please Place Description here.
 * @var mac_ax_mcc_group::rpt_tsf_low
 * Please Place Description here.
 * @var mac_ax_mcc_group::tsf_x_high
 * Please Place Description here.
 * @var mac_ax_mcc_group::tsf_x_low
 * Please Place Description here.
 * @var mac_ax_mcc_group::tsf_y_high
 * Please Place Description here.
 * @var mac_ax_mcc_group::tsf_y_low
 * Please Place Description here.
 */
struct mac_ax_mcc_group {
	u8 rpt_status;
	u8 rpt_macid;
	u8 macid_x;
	u8 macid_y;
	u32 rpt_tsf_high;
	u32 rpt_tsf_low;
	u32 tsf_x_high;
	u32 tsf_x_low;
	u32 tsf_y_high;
	u32 tsf_y_low;
};

/**
 * @struct mac_ax_mcc_group_info
 * @brief mac_ax_mcc_group_info
 *
 * @var mac_ax_mcc_group_info::groups
 * Please Place Description here.
 */
struct mac_ax_mcc_group_info {
	struct mac_ax_mcc_group groups[4];
};

/**
 * @struct mac_ax_tx_tf_info
 * @brief mac_ax_tx_tf_info
 *
 * @var mac_ax_tx_tf_info::tx_tf_infol
 * Please Place Description here.
 * @var mac_ax_tx_tf_info::tx_tf_infoh
 * Please Place Description here.
 * @var mac_ax_tx_tf_info::tx_tf_infosel
 * Please Place Description here.
 */
struct mac_ax_tx_tf_info {
	u32 tx_tf_infol;
	u32 tx_tf_infoh;
	u8 tx_tf_infosel;//4:common info; 0~3: user0 ~ user3 info
};

/**
 * @enum sec_cfg_sel
 *
 * @brief sec_cfg_sel
 *
 * @var sec_threshold_sel::CTS2SELF_DISABLE
 * Please Place Description here.
 * @var sec_threshold_sel::CTS2SELF_NON_SEC_THRESHOLD
 * Please Place Description here.
 * @var sec_threshold_sel::CTS2SELF_SEC_THRESHOLD
 * Please Place Description here.
 * @var sec_threshold_sel::CTS2SELF_BOTH_THRESHOLD
 * Please Place Description here.
 */
enum mac_ax_threshold_sel {
	MAC_AX_CTS2SELF_DISABLE,
	MAC_AX_CTS2SELF_NON_SEC_THRESHOLD, /* sec type != WEP/TKIP/WAP */
	MAC_AX_CTS2SELF_SEC_THRESHOLD,     /* sec type == WEP/TKIP/WAP */
	MAC_AX_CTS2SELF_BOTH_THRESHOLD,
};

/**
 * @struct mac_ax_sr_info
 * @brief mac_ax_sr_info
 *
 * @var mac_ax_sr_info::sr_en
 * Please Place Description here.
 * @var mac_ax_sr_info::sr_field_v15_allowed
 * Please Place Description here.
 * @var mac_ax_sr_info::srg_obss_pd_min
 * Please Place Description here.
 * @var mac_ax_sr_info::srg_obss_pd_max
 * Please Place Description here.
 * @var mac_ax_sr_info::non_srg_obss_pd_min
 * Please Place Description here.
 * @var mac_ax_sr_info::non_srg_obss_pd_max
 * Please Place Description here.
 * @var mac_ax_sr_info::srg_bsscolor_bitmap_0
 * Please Place Description here.
 * @var mac_ax_sr_info::srg_bsscolor_bitmap_1
 * Please Place Description here.
 * @var mac_ax_sr_info::srg_partbsid_bitmap_0
 * Please Place Description here.
 * @var mac_ax_sr_info::srg_partbsid_bitmap_1
 * Please Place Description here.
 */
struct mac_ax_sr_info {
	u8 sr_en: 1;
	u8 sr_field_v15_allowed: 1;
	u8 srg_obss_pd_min;
	u8 srg_obss_pd_max;
	u8 non_srg_obss_pd_min;
	u8 non_srg_obss_pd_max;
	u32 srg_bsscolor_bitmap_0;
	u32 srg_bsscolor_bitmap_1;
	u32 srg_partbsid_bitmap_0;
	u32 srg_partbsid_bitmap_1;
};

/**
 * @struct mac_ax_nav_padding
 * @brief mac_ax_nav_padding
 *
 * @var mac_ax_nav_padding::band
 * Please Place Description here.
 * @var mac_ax_nav_padding::nav_pad_en
 * Please Place Description here.
 * @var mac_ax_nav_padding::over_txop_en
 * Please Place Description here.
 * @var mac_ax_nav_padding::nav_padding
 * Please Place Description here.
 */
struct mac_ax_nav_padding {
	u8 band;
	u8 nav_pad_en;
	u8 over_txop_en;
	u16 nav_padding;
};

/**
 * @struct mac_ax_max_tx_time
 * @brief mac_ax_max_tx_time
 *
 * @var mac_ax_max_tx_time::macid
 * Please Place Description here.
 * @var mac_ax_max_tx_time::is_cctrl
 * Please Place Description here.
 * @var mac_ax_max_tx_time::max_tx_time
 * Please Place Description here.
 */
struct mac_ax_max_tx_time {
	u8 macid;
	u8 is_cctrl;
	u32 max_tx_time; /* us */
};

/**
 * @struct mac_ax_hw_rts_th
 * @brief Config HW RTS time/len threshold
 *
 * @var mac_ax_hw_rts_th::band
 * the mac_band to setup/query
 * @var mac_ax_hw_rts_th::time_th
 * HW RTS time threshold
 * @var mac_ax_hw_rts_th::time_th
 * HW RTS length threshold
 */
struct mac_ax_hw_rts_th {
	u8 band;
	u16 time_th; /* us */
	u16 len_th; /* byte */
};

/**
 * @struct mac_ax_io_stat
 * @brief Get IO state from HCI (PCIE: LBC)
 *
 * @var mac_ax_io_stat::to_flag
 * timeout flag is set
 * @var mac_ax_io_stat::io_st
 * IO state from sm.io_st
 * @var mac_ax_io_stat::rsvd
 * reserved
 * @var mac_ax_io_stat::addr
 * the last timeout addr when timeout flag is set
 */
struct mac_ax_io_stat {
	u8 to_flag:1;
	u8 io_st:1;
	u8 rsvd:6;
	u32 addr;
};

/**
 * @struct mac_ax_drv_stats
 * @brief
 *
 * The driver status in halmac
 *
 * @var mac_ax_drv_stats::rx_ok
 * RX status
 * @var mac_ax_drv_stats::drv_rm
 * Driver is removed
 */
struct mac_ax_drv_stats {
	u8 rx_ok;
	u8 drv_rm;
};

/**
 * @struct mac_ax_wps_cfg
 * @brief
 *
 * WPS is a driver feature to detect button pressed or released.
 * In HW view, the feature is to check the GPIO input value is 0->1 or 1->0
 * We use FW to detect GPIO val.
 * In a specified interval, if FW detects value changed, it will send a C2H
 *
 * @var mac_ax_wps_cfg::en
 * Enable WPS function i.e, Enable FW reports C2H
 * @var mac_ax_wps_cfg::gpio
 * The GPIO to be detected
 * @var mac_ax_wps_cfg::interval
 * The detecting interval in ms
 */
struct mac_ax_cfg_wps {
	u8 en;
	u8 gpio;
	u8 interval; /* ms */
};

/**
 * @struct mac_fw_msg
 * @brief
 *
 * fw message encode/decode table
 *
 * @var mac_fw_msg::msgno
 * @var mac_fw_msg::msg
 */

struct mac_fw_msg {
	u32 msgno;
	char *msg;
};

/**
 * @struct mac_ax_preld_cfg
 * @brief mac_ax_preld_cfg
 *
 * @var mac_ax_preld_cfg::macid
 * Please Place Description here.
 * @var mac_ax_preld_cfg::en
 * Please Place Description here.
 * @var mac_ax_preld_cfg::rsvd
 * Please Place Description here.
 */
struct mac_ax_preld_cfg {
	u8 macid[MAC_AX_PRELD_MACID_MAX];
	u8 macid_en[MAC_AX_PRELD_MACID_MAX];
	u16 mgq_en:1;
	u16 hiq_en:1;
	u16 hiq_port:3;
	u16 hiq_mbid:5;
	u16 rsvd:6;
};

/**
 * @struct mac_ax_ss_link_info
 * @brief mac_ax_ss_link_info
 *
 * @var mac_ax_ss_link_info::wmm
 * Please Place Description here.
 * @var mac_ax_ss_link_info::ac
 * Please Place Description here.
 * @var mac_ax_ss_link_info::ul
 * Please Place Description here.
 * @var mac_ax_ss_link_info::link_list
 * Please Place Description here.
 * @var mac_ax_ss_link_info::link_head
 * Please Place Description here.
 * @var mac_ax_ss_link_info::link_tail
 * Please Place Description here.
 * @var mac_ax_ss_link_info::link_len
 * Please Place Description here.
 * @var mac_ax_ss_link_info::macid0
 * Please Place Description here.
 * @var mac_ax_ss_link_info::macid1
 * Please Place Description here.
 * @var mac_ax_ss_link_info::macid2
 * Please Place Description here.
 * @var mac_ax_ss_link_info::link_bitmap
 * Please Place Description here.
 */
struct mac_ax_ss_link_info {
	u8 wmm;
	u8 ac;
	u8 ul;
	u8 link_list[SS_LINK_SIZE];
	u8 link_head;
	u8 link_tail;
	u8 link_len;
	u8 macid0;
	u8 macid1;
	u8 macid2;
	u8 link_bitmap[SS_LINK_SIZE];
};

/**
 * @struct mac_ax_dbg_port_info
 * @brief mac_ax_dbg_port_info
 *
 * @var mac_ax_dbg_port_info::sel_addr
 * Please Place Description here.
 * @var mac_ax_dbg_port_info::sel_byte
 * Please Place Description here.
 * @var mac_ax_dbg_port_info::sel_sh
 * Please Place Description here.
 * @var mac_ax_dbg_port_info::sel_msk
 * Please Place Description here.
 * @var mac_ax_dbg_port_info::srt
 * Please Place Description here.
 * @var mac_ax_dbg_port_info::end
 * Please Place Description here.
 * @var mac_ax_dbg_port_info::inc_num
 * Please Place Description here.
 * @var mac_ax_dbg_port_info::rd_addr
 * Please Place Description here.
 * @var mac_ax_dbg_port_info::rd_byte
 * Please Place Description here.
 * @var mac_ax_dbg_port_info::rd_sh
 * Please Place Description here.
 * @var mac_ax_dbg_port_info::rd_msk
 * Please Place Description here.
 */
struct mac_ax_dbg_port_info {
	u32 sel_addr;
	u8 sel_byte;
	u32 sel_sh;
	u32 sel_msk;
	u32 srt;
	u32 end;
	u32 inc_num;
	u32 rd_addr;
	u8 rd_byte;
	u32 rd_sh;
	u32 rd_msk;
};

/**
 * @struct mac_ax_dbcc_pcie_ctrl
 * @brief mac_ax_dbcc_pcie_ctrl
 *
 * @var mac_ax_dbcc_pcie_ctrl::out_host_idx_l
 * Please Place Description here.
 * @var mac_ax_dbcc_pcie_ctrl::out_hw_idx_l
 * Please Place Description here.
 * @var mac_ax_dbcc_pcie_ctrl::clr_txch_map
 * Please Place Description here.
 */
struct mac_ax_dbcc_pcie_ctrl {
	u16 out_host_idx_l[MAC_AX_DMA_CH_NUM];
	u16 out_hw_idx_l[MAC_AX_DMA_CH_NUM];
	struct mac_ax_txdma_ch_map clr_txch_map;
};

/**
 * @struct mac_ax_dbcc_usb_ctrl
 * @brief mac_ax_dbcc_usb_ctrl
 *
 * @var mac_ax_dbcc_usb_ctrl::rsvd
 * Please Place Description here.
 */
struct mac_ax_dbcc_usb_ctrl {
	u32 rsvd;
};

/**
 * @struct mac_ax_dbcc_sdio_ctrl
 * @brief mac_ax_dbcc_sdio_ctrl
 *
 * @var mac_ax_dbcc_sdio_ctrl::rsvd
 * Please Place Description here.
 */
struct mac_ax_dbcc_sdio_ctrl {
	u32 rsvd;
};

/**
 * @struct mac_ax_dbcc_hci_ctrl
 * @brief mac_ax_dbcc_hci_ctrl
 *
 * @var mac_ax_dbcc_hci_ctrl::band
 * Please Place Description here.
 * @var mac_ax_dbcc_hci_ctrl::pause
 * Please Place Description here.
 * @var mac_ax_dbcc_hci_ctrl::u
 * Please Place Description here.
 */
struct mac_ax_dbcc_hci_ctrl {
	enum mac_ax_band band;
	u8 pause;
	union {
		struct mac_ax_dbcc_pcie_ctrl pcie_ctrl;
		struct mac_ax_dbcc_usb_ctrl usb_ctrl;
		struct mac_ax_dbcc_sdio_ctrl sdio_ctrl;
	} u;
};

/*------------------- Define FAST_CH_SW related structure ---------------------------*/

/**
 * @struct mac_ax_fast_ch_sw_param
 * @brief
 *
 * FAST_CH_SW H2C params in driver
 *
 * @var mac_ax_fast_ch_sw_param::ap_port_id
 * ap port id
 * @var mac_ax_fast_ch_sw_param::ch_idx
 * mapped channel idx for restoring rf param
 * @var mac_ax_fast_ch_sw_param::thermal_idx
 * thermal idx for restoring rf param
 * @var mac_ax_fast_ch_sw_param::pause_rel_mode
 * pause and release mode
 * @var mac_ax_fast_ch_sw_param::con_sta_num
 * num of connected sta currently
 * @var mac_ax_fast_ch_sw_param::band
 * PHY band
 * @var mac_ax_fast_ch_sw_param::bandwidth
 * bw of 20/40/80
 * @var mac_ax_fast_ch_sw_param::pri_ch
 * pri channel of target channel
 * @var mac_ax_fast_ch_sw_param::central_ch
 * central channel of target channel
 * @var mac_ax_fast_ch_sw_param::rel_pause_tsfl
 * release pause tsfl
 * @var mac_ax_fast_ch_sw_param::rel_pause_tsfh
 * release pause tsfh
 * @var mac_ax_fast_ch_sw_param::rel_pause_delay_time
 * release pause delay time
 * @var mac_ax_fast_ch_sw_param::csa_pkt_id[MAC_AX_FAST_CH_SW_MAX_STA_NUM]
 * offloaded CSA packet id for at most 4 stas
 */
struct mac_ax_fast_ch_sw_param {
	/* dword0 */
	u8 ap_port_id:4;
	u8 ch_idx:4;
	u8 thermal_idx:4;
	u8 pause_rel_mode:4;
	u8 con_sta_num;
	u8 band:1;
	u8 bandwidth:2;
	u8 ch_band: 2;
	u8 rsvd0:3;
	/* dword1 */
	u8 pri_ch;
	u8 central_ch;
	u16 rsvd1;
	/* dword2 */
	u32 rel_pause_tsfl;
	/* dword3 */
	u32 rel_pause_tsfh;
	/* dword4 */
	u32 rel_pause_delay_time;
	/* dword5 */
	u8 csa_pkt_id[MAC_AX_FAST_CH_SW_MAX_STA_NUM];
};

/**
 * @struct mac_ax_fast_ch_sw_info
 * @brief
 *
 * FAST_CH_SW status in driver
 *
 * @var mac_ax_fast_ch_sw_info::busy
 * FW handling or not
 * @var mac_ax_fast_ch_sw_info::status
 * Last status of FCS
 */
struct mac_ax_fast_ch_sw_info{
	bool busy;
	u32 status;
};

/*------------------- END Define FAST_CH_SW related structure ---------------------------*/

/*------------------- Define ch_switch related structure ---------------------------*/
/**
 * @struct ch_switch_rpt
 * @brief ch_switch_rpt
 *
 * @var ch_switch_rpt::result
 * result
 * @var ch_switch_rpt::mac_time
 * mac_time
 * @var ch_switch_rpt::bb_time
 * bb_time
 * @var ch_switch_rpt::rf_time
 * rf_time
 * @var ch_switch_rpt::rf_reload_time
 * rf_reload_time
 * @var ch_switch_rpt::total_time
 * total_time
 */
struct mac_ax_ch_switch_rpt {
	u8 result;
};

/**
 * @struct ch_switch_parm
 * @brief ch_switch_parm
 *
 * @var ch_switch_parm::pri_ch
 * pri ch
 * @var ch_switch_parm::central_ch
 * central ch
 * @var ch_switch_parm::port
 * port
 * @var ch_switch_parm::bw
 * bw
 * @var ch_switch_parm::ch_band
 * ch_band
 * @var ch_switch_parm::band
 * band
 * @var ch_switch_parm::reload_rf
 * reload rf
 * @var ch_switch_parm::c2h_rpt
 * c2h report
 * @var ch_switch_parm::rsvd
 * rsvd
 */
struct mac_ax_ch_switch_parm {
	u8 pri_ch;
	u8 central_ch;
	u8 bw:3;
	u8 ch_band:2;
	u8 band:1;
	u8 reload_rf:1;
	u16 rsvd:9;
};

/*------------------- ENDDefine ch_switch related structure ---------------------------*/

struct mac_ax_tf_user_sts {
	u8 macid;
	u8 tb_rate;
	u8 tb_fail_per;
	u8 avg_tb_rssi;
	u8 cca_miss_per;
	u8 avg_uph;
	u8 minflag_per;
	u8 avg_tb_evm;
	u32 tf_num;
	u16 bsr_len;
	u16 rsvd;
};

struct mac_ax_tf_sts {
	u8 user_num;
	u8 ru_su_per;
	u16 rsvd;
	struct mac_ax_tf_user_sts tf_user_sts[UL_PER_STA_DBGINFO_NUM];
};

struct mac_ax_dlru_user_sts {
	u8 macid;             // STA macid
	u8 su_ru_ratio;
	u8 su_fail_ratio;
	u8 ru_fail_ratio;
	u8 ru_avg_agg;
	u8 NSS : 4;
	u8 MCS : 4;
};

struct mac_ax_dlru_sts {
	u8 user_num;
	u8 total_su_ru_ratio;
	u8 total_ru_fail_ratio;
	u8 total_su_fail_ratio;
	struct mac_ax_dlru_user_sts user_sts[DLRU_MAX_USER_STS];
};

struct mac_ax_fwc2h_sts {
	struct mac_ax_tf_sts tfsts;
	struct mac_ax_dlru_sts dlrusts;
};

struct mac_ax_fwsts_para {
	u16 en:1;
	u16 rsvd:15;
	u16 intvl_ms;
};

/**
 * @struct mac_ax_per_pkt_phy_rpt
 * @brief mac_ax_per_pkt_phy_rpt
 *
 * @var mac_ax_per_pkt_phy_rpt::en
 * 0: enable per-pkt phy report.
 * 1: disable per-pkt phy report.
 * @var mac_ax_per_pkt_phy_rpt::band
 * 0: config band0 per-pkt phy report.
 * 1: config band1 per-pkt phy report.
 */
struct mac_ax_per_pkt_phy_rpt {
	u8 en;
	u8 band;
};

/**
 * @struct mac_ax_rx_driver_info_hdr_cfg
 * @brief mac_ax_rx_driver_info_hdr_cfg
 *
 * @var mac_ax_rx_driver_info_hdr_cfg::en
 * 0: enable per-pkt phy report.
 * 1: disable per-pkt phy report.
 * @var mac_ax_rx_driver_info_hdr_cfg::driv_info_hdr_type
 * config type of driv_info_hdr.
 * Please refer to mac_ax_rx_driv_info_hdr_type
 */
struct mac_ax_rx_driv_info_hdr_cfg {
	u8 en;
	enum mac_ax_rx_driv_info_hdr_type driv_info_hdr_type;
};

/**
 * @struct mac_ax_rx_hdr_conv_cfg
 * @brief mac_ax_rx_hdr_conv_cfg
 *
 * @var mac_ax_rx_hdr_conv_cfg::en
 * 0: enable rx hdr conv.
 * 1: disable rx hdr conv.
 * @var mac_ax_rx_hdr_conv_cfg::chk_addr_cam_hit
 * 0: always do rx hdr conv .
 * 1: do rx hdr conv only if the RX frames hit address cam.
 * @var mac_ax_rx_hdr_conv_cfg::rx_hdr_conv_type
 * config type of rx hdr conv.
 * Please refer to mac_ax_rx_hdr_trans_type
 */
struct mac_ax_rx_hdr_conv_cfg {
	u8 en;
	u8 chk_addr_cam_hit;
	enum mac_ax_rx_hdr_trans_type rx_hdr_conv_type;
};

/*--------------------Define power saving related struct -------------------------*/
/**
 * @struct mac_ax_lps_info
 * @brief mac_ax_lps_info
 *
 * @var mac_ax_lps_info::listen_bcn_mode
 * Please Place Description here.
 * @var mac_ax_lps_info::awake_interval
 * Please Place Description here.
 * @var mac_ax_lps_info::smart_ps_mode
 * Please Place Description here.
 * @var mac_ax_lps_info::bcnnohit_en
 * Please Place Description here.
 * @var mac_ax_lps_info::vouapsd_en
 * Please Place Description here.
 * @var mac_ax_lps_info::viuapsd_en
 * Please Place Description here.
 * @var mac_ax_lps_info::beuapsd_en
 * Please Place Description here.
 * @var mac_ax_lps_info::bkuapsd_en
 * Please Place Description here.
 * @var mac_ax_lps_info::nulltype
 * Please Place Description here.
 */
struct mac_ax_lps_info {
	enum mac_ax_listern_bcn_mode listen_bcn_mode;
	u8 awake_interval;
	enum mac_ax_smart_ps_mode smart_ps_mode;
	u8 bcnnohit_en;
	u8 vouapsd_en;
	u8 viuapsd_en;
	u8 beuapsd_en;
	u8 bkuapsd_en;
	u8 nulltype;
};

/**
 * @struct mac_ax_ps_adv_parm
 * @brief mac_ax_ps_adv_parm
 *
 * @var mac_ax_ps_adv_parm::macid
 * Please Place Description here.
 * @var mac_ax_ps_adv_parm::TRXTimeOutTimeSet
 * Please Place Description here.
 * @var mac_ax_ps_adv_parm::rsvd0
 * Please Place Description here.
 * @var mac_ax_ps_adv_parm::TRXTimeOutTimeVal
 * Please Place Description here.
 * @var mac_ax_ps_adv_parm::rsvd1
 * Please Place Description here.
 */
struct mac_ax_ps_adv_parm {
	u32 macid:8;
	u32 trxtimeouttimeset:2;
	u32 ensmartpsdtimrx:1;
	u32 rsvd0:21;
	u32 trxtimeouttimeval:8;
	u32 rsvd1:24;
};

/**
 * @struct mac_ax_periodic_wake_info
 * @brief mac_ax_periodic_wake_info
 *
 * @var mac_ax_periodic_wake_info::macid
 * Please Place Description here.
 * @var mac_ax_periodic_wake_info::enable
 * Please Place Description here.
 * @var mac_ax_periodic_wake_info::band
 * Please Place Description here.
 * @var mac_ax_periodic_wake_info::port
 * Please Place Description here.
 * @var mac_ax_periodic_wake_info::rsvd
 * Please Place Description here.
 * @var mac_ax_periodic_wake_info::sleep_interval
 * Please Place Description here.
 * @var mac_ax_periodic_wake_info::wake_duration
 * Please Place Description here.
 */
struct mac_ax_periodic_wake_info {
	u32 macid: 8;
	u32 enable: 1;
	u32 band: 1;
	u32 port: 3;
	u32 rsvd: 19;
	u32 wake_period;
	u32 wake_duration;
};

/**
 * @struct mac_ax_bcn_fltr
 * @brief mac_ax_bcn_fltr
 *
 * @var mac_ax_bcn_fltr::mon_rssi
 * Please Place Description here.
 * @var mac_ax_bcn_fltr::mon_bcn
 * Please Place Description here.
 * @var mac_ax_bcn_fltr::mon_tp
 * Please Place Description here.
 * @var mac_ax_bcn_fltr::tp_thld
 * Please Place Description here.
 * @var mac_ax_bcn_fltr::bcn_loss_cnt
 * Please Place Description here.
 * @var mac_ax_bcn_fltr::rssi_hys
 * Please Place Description here.
 * @var mac_ax_bcn_fltr::rssi_thld
 * Please Place Description here.
 * @var mac_ax_bcn_fltr::macid
 * Please Place Description here.
 */
struct mac_ax_bcn_fltr {
	u32 mon_rssi: 1;
	u32 mon_bcn: 1;
	u32 mon_tp: 1;
	u32 tp_thld: 2;
	u32 rsvd0: 3;
	u32 bcn_loss_cnt: 4;
	u32 rssi_hys: 4;
	u32 rssi_thld: 8;
	u32 macid: 8;
};

/**
 * @struct mac_ax_bcn_fltr_rpt
 * @brief mac_ax_bcn_fltr_rpt
 *
 * @var mac_ax_bcn_fltr_rpt::macid
 * Please Place Description here.
 * @var mac_ax_bcn_fltr_rpt::type
 * Please Place Description here.
 * @var mac_ax_bcn_fltr_rpt::rssi_evt
 * Please Place Description here.
 * @var mac_ax_bcn_fltr_rpt::rssi_ma
 * Please Place Description here.
 */
struct mac_ax_bcn_fltr_rpt {
	u32 macid: 8;
	u32 type: 2;
	u32 rssi_evt: 2;
	u32 rsvd0: 4;
	u32 rssi_ma: 8;
	u32 rsvd1: 8;
	u8 notified;
};

struct mac_ax_cts2self_cfg {
	u8 band_sel;
	enum mac_ax_threshold_sel threshold_sel;
	u16 non_sec_threshold;
	u16 sec_threshold;
};

struct mac_ax_multicast_info {
	u8 mc_addr[6];
	u8 bssid[6];
	enum mac_ax_addr_msk mc_msk;
	/* the mask is for multicast address */
	/* each bit is mapped to one-byte mc_addr */
	/* 0: do not compare the mc_addr byte. 1: compare the mc_addr byte */
	/* mc_msk bit0 is mapped to mc_addr[0] */
	/* mc_msk bit1 is mapped to mc_addr[1] */
};

/**
 * @struct mac_ax_req_pwr_lvl_info
 * @brief mac_ax_req_pwr_lvl_info
 *
 * @var mac_ax_req_pwr_lvl_info::macid
 * Please Place Description here.
 * @var mac_ax_req_pwr_lvl_info::bcn_to_val
 * Please Place Description here.
 * @var mac_ax_req_pwr_lvl_info::ps_lvl
 * Please Place Description here.
 * @var mac_ax_req_pwr_lvl_info::trx_lvl
 * Please Place Description here.
 * @var mac_ax_req_pwr_lvl_info::bcn_to_lvl
 * Please Place Description here.
 * @var mac_ax_req_pwr_lvl_info::rsvd0
 * Please Place Description here.
 */
struct mac_ax_req_pwr_lvl_info {
	u32 macid:8;
	u32 bcn_to_val:8;
	u32 ps_lvl:4;
	u32 trx_lvl:4;
	u32 bcn_to_lvl:4;
	u32 rsvd0:4;
};

struct mac_lps_option {
	enum mac_req_xtal_option req_xtal_option;
};

/*--------------------Define NAN related struct -------------------------*/
/**
 * @struct mac_ax_act_ack_info
 * @brief mac_ax_act_ack_info
 *
 * @var mac_ax_act_ack_info::schedule_id
 * Please Place Description here.
 */
struct mac_ax_act_ack_info {
	u32 schedule_id;
};

/**
 * @struct mac_ax_nan_info
 * @brief mac_ax_nan_info
 *
 * @var mac_ax_nan_info::rpt_cluster_id
 * Please Place Description here.
 * @var mac_ax_nan_info::rpt_master_pref
 * Please Place Description here.
 * @var mac_ax_nan_info::rpt_random_factor
 * Please Place Description here.
 * @var mac_ax_nan_info::rpt_amr
 * Please Place Description here.
 * @var mac_ax_nan_info::rpt_ambtt
 * Please Place Description here.
 * @var mac_ax_nan_info::rpt_hop_count
 * Please Place Description here.
 * @var mac_ax_nan_info::rpt_port_dwst_low
 * Please Place Description here.
 * @var mac_ax_nan_info::rpt_fr_dwst_low
 * Please Place Description here.
 * @var mac_ax_nan_sched_info::mac_ax_act_ack_info
 * Please Place Description here.
 */
struct mac_ax_nan_info {
	u8 rpt_cluster_id[6];
	u8 rpt_master_pref;
	u8 rpt_random_factor;
	u32 rpt_amr;
	u32 rpt_ambtt;
	u8 rpt_hop_count;
	u32 rpt_port_dwst_low;
	u32 rpt_fr_dwst_low;
	struct mac_ax_act_ack_info nan_act_ack_info;
};

/**
 * @struct mac_ax_nan_sched_info
 * @brief mac_ax_nan_sched_info
 *
 * @var mac_ax_nan_sched_info::module_id
 * Please Place Description here.
 * @var mac_ax_nan_sched_info::priority
 * Please Place Description here.
 * @var mac_ax_nan_sched_info::options
 * Please Place Description here.
 * @var mac_ax_nan_sched_info::faw_en
 * Please Place Description here.
 * @var mac_ax_mac_ax_nan_sched_infonan_info::start_time
 * Please Place Description here.
 * @var mac_ax_nan_sched_info::duration
 * Please Place Description here.
 * @var mac_ax_nan_sched_info::period
 * Please Place Description here.
 * @var mac_ax_nan_sched_info::tsf_idx
 * Please Place Description here.
 * @var mac_ax_nan_sched_info::channel
 * Please Place Description here.
 * @var mac_ax_nan_sched_info::bw
 * Please Place Description here.
 * @var mac_ax_nan_sched_info::primary_ch_idx
 * Please Place Description here.
 */
struct mac_ax_nan_sched_info {
	/* dword0 */
	u8 module_id;
	u8 priority;
	u8 options;
	u8 faw_en;
	/* dword1 */
	u32 start_time;
	/* dword2 */
	u32 duration;
	/* dword3 */
	u32 period;
	/* dword4 */
	u8 tsf_idx;
	u8 channel;
	u8 bw;
	u8 primary_ch_idx;
};

/**
 * @struct mac_ax_nan_bcn
 * @brief mac_ax_nan_bcn
 *
 * @var mac_ax_nan_bcn::module_id
 * Please Place Description here.
 * @var mac_ax_nan_bcn::bcn_rsvd_offset
 * Please Place Description here.
 * @var mac_ax_nan_bcn::bcn_intvl_ms
 * Please Place Description here.
 * @var mac_ax_nan_bcn::priority
 * Please Place Description here.
 * @var mac_ax_nan_bcn::bcn_offset_us
 * Please Place Description here.
 * @var mac_ax_nan_bcn::cur_tbtt
 * Please Place Description here.
 * @var mac_ax_nan_bcn::cur_tbtt_fr
 * Please Place Description here.
 * @var mac_ax_nan_bcn::prohibit_before_ms
 * Please Place Description here.
 * @var mac_ax_nan_bcn::prohibit_after_ms
 * Please Place Description here.
 * @var mac_ax_nan_bcn::port_idx
 * Please Place Description here.
 * @var mac_ax_nan_bcn::options
 * Please Place Description here.
 */
struct mac_ax_nan_bcn {
	/* dword0 */
	u8 module_id;
	u8 bcn_rsvd_offset;
	u8 bcn_intvl_ms;
	u8 priority;
	/* dword1 */
	u32 bcn_offset_us;
	/* dword2 */
	u32 cur_tbtt;
	/* dword3 */
	u32 cur_tbtt_fr;
	/* dword4 */
	u8 prohibit_before_ms;
	u8 prohibit_after_ms;
	u8 port_idx;
	u8 options;
};

/**
 * @struct mac_ax_nan_func_info
 * @brief mac_ax_nan_func_info
 *
 * @var mac_ax_nan_func_info::port_idx
 * Please Place Description here.
 * @var mac_ax_nan_func_info::mac_id
 * Please Place Description here.
 * @var mac_ax_nan_func_info::master_pref
 * Please Place Description here.
 * @var mac_ax_nan_func_info::random_factor
 * Please Place Description here.
 * @var mac_ax_nan_func_info::op_ch_24g
 * Please Place Description here.
 * @var mac_ax_nan_func_info::op_ch_5g
 * Please Place Description here.
 * @var mac_ax_nan_func_info::options
 * Please Place Description here.
 * @var mac_ax_nan_func_info::time_indicate_period
 * Please Place Description here.
 * @var mac_ax_nan_func_info::cluster_id
 * Please Place Description here.
 * @var mac_ax_nan_func_info::para_options
 * Please Place Description here.
 */
struct mac_ax_nan_func_info {
	/* dword0 */
	u8 port_idx;
	u8 mac_id;
	u8 master_pref;
	u8 random_factor;
	/* dword1 */
	u8 op_ch_24g;
	u8 op_ch_5g;
	u16 options;
	/* dword2 */
	u32 time_indicate_period;
	u8 cluster_id[6];
	u8 para_options;
};

/**
 * @struct mac_ax_ss_dl_rpt_info
 * @brief mac_ax_ss_dl_rpt_info
 *
 * @var mac_ax_ss_dl_rpt_info::wmm0_max
 * Please Place Description here.
 * @var mac_ax_ss_dl_rpt_info::wmm1_max
 * Please Place Description here.
 * @var mac_ax_ss_dl_rpt_info::twt_wmm0_max
 * Please Place Description here.
 * @var mac_ax_ss_dl_rpt_info::twt_wmm1_max
 * Please Place Description here.
 */
struct mac_ax_ss_dl_rpt_info {
	u8 wmm0_max;
	u8 wmm1_max;
	u8 twt_wmm0_max;
	u8 twt_wmm1_max;
};

enum mac_ax_ss_rpt_cfg {
	MAC_AX_SS_DL_SU_RPT_CFG_GET,
	MAC_AX_SS_DL_SU_RPT_CFG_SET,
	MAC_AX_SS_DL_MU_RPT_CFG_GET,
	MAC_AX_SS_DL_MU_RPT_CFG_SET,
	MAC_AX_SS_DL_RU_RPT_CFG_GET,
	MAC_AX_SS_DL_RU_RPT_CFG_SET,
};

/**
 * @struct mac_ax_bcn_erly_rpt
 * @brief mac_ax_bcn_erly_rpt
 *
 * @var mac_ax_bcn_erly_rpt::band
 * Please Place Description here.
 * @var mac_ax_bcn_erly_rpt::port
 * Please Place Description here.
 */
struct mac_ax_bcn_erly_rpt {
	u8 band;
	u8 port;
};

/*--------------------Define Adapter & OPs------------------------------------*/
#ifndef CONFIG_NEW_HALMAC_INTERFACE

/**
 * @struct mac_ax_pltfm_cb
 * @brief mac_ax_pltfm_cb
 *
 * @var mac_ax_pltfm_cb::sdio_cmd52_r8
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::sdio_cmd53_r8
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::sdio_cmd53_r16
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::sdio_cmd53_r32
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::sdio_cmd53_rn
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::sdio_cmd52_w8
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::sdio_cmd53_w8
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::sdio_cmd53_w16
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::sdio_cmd53_w32
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::sdio_cmd53_wn
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::sdio_cmd52_cia_r8
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::reg_r8
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::reg_r16
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::reg_r32
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::reg_w8
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::reg_w16
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::reg_w32
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::tx
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_query_h2c
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::tx
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_free
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_malloc
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_memcpy
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_memset
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_memcmp
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_delay_us
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_delay_ms
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_sleep_us
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_sleep_ms
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_mutex_init
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_mutex_deinit
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_mutex_lock
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::rtl_mutex_unlock
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::msg_print
 * Please Place Description here.
 * @var mac_ax_pltfm_cb::event_notify
 * Please Place Description here.
 */
struct mac_ax_pltfm_cb {
#if MAC_AX_SDIO_SUPPORT
	u8 (*sdio_cmd52_r8)(void *drv_adapter, u32 addr);
	u8 (*sdio_cmd53_r8)(void *drv_adapter, u32 addr);
	u16 (*sdio_cmd53_r16)(void *drv_adapter, u32 addr);
	u32 (*sdio_cmd53_r32)(void *drv_adapter, u32 addr);
	u8 (*sdio_cmd53_rn)(void *drv_adapter, u32 addr, u32 size, u8 *val);
	void (*sdio_cmd52_w8)(void *drv_adapter, u32 addr, u8 val);
	void (*sdio_cmd53_w8)(void *drv_adapter, u32 addr, u8 val);
	void (*sdio_cmd53_w16)(void *drv_adapter, u32 addr, u16 val);
	void (*sdio_cmd53_w32)(void *drv_adapter, u32 addr, u32 val);
	u8 (*sdio_cmd53_wn)(void *drv_adapter, u32 addr, u32 size, u8 *val);
	u8 (*sdio_cmd52_cia_r8)(void *drv_adapter, u32 addr);
#endif
#if (MAC_AX_USB_SUPPORT || MAC_AX_PCIE_SUPPORT)
	u8 (*reg_r8)(void *drv_adapter, u32 addr);
	u16 (*reg_r16)(void *drv_adapter, u32 addr);
	u32 (*reg_r32)(void *drv_adapter, u32 addr);
	void (*reg_w8)(void *drv_adapter, u32 addr, u8 val);
	void (*reg_w16)(void *drv_adapter, u32 addr, u16 val);
	void (*reg_w32)(void *drv_adapter, u32 addr, u32 val);
#endif
#if MAC_AX_PHL_H2C
	enum rtw_hal_status (*tx)(struct rtw_phl_com_t *phl_com,
				  struct rtw_hal_com_t *hal_com,
				  struct rtw_h2c_pkt *pkt);
	struct rtw_h2c_pkt *(*rtl_query_h2c)(struct rtw_phl_com_t *phl_com,
					     struct rtw_hal_com_t *hal_com,
					     enum h2c_buf_class  type);
	enum rtw_hal_status (*rtl_recycle_h2c)(struct rtw_phl_com_t *phl_com,
					       struct rtw_h2c_pkt *h2c_pkt);
#else
	u32 (*tx)(void *drv_adapter, u8 *buf, u32 len);
#endif
	void (*rtl_free)(void *drv_adapter, void *buf, u32 size);
	void* (*rtl_malloc)(void *drv_adapter, u32 size);
	void (*rtl_memcpy)(void *drv_adapter, void *dest, void *src, u32 size);
	void (*rtl_memset)(void *drv_adapter, void *addr, u8 val, u32 size);
	s32 (*rtl_memcmp)(void *drv_adapter, void *ptr1, void *ptr2, u32 num);
	void (*rtl_delay_us)(void *drv_adapter, u32 us);
	void (*rtl_delay_ms)(void *drv_adapter, u32 ms);
	void (*rtl_sleep_us)(void *drv_adapter, u32 us);
	void (*rtl_sleep_ms)(void *drv_adapter, u32 ms);

	void (*rtl_mutex_init)(void *drv_adapter, mac_ax_mutex *mutex);
	void (*rtl_mutex_deinit)(void *drv_adapter, mac_ax_mutex *mutex);
	void (*rtl_mutex_lock)(void *drv_adapter, mac_ax_mutex *mutex);
	void (*rtl_mutex_unlock)(void *drv_adapter, mac_ax_mutex *mutex);
	void (*msg_print)(void *drv_adapter, u8 dbg_level, s8 *fmt, ...);
	void (*event_notify)(void *drv_adapter,
			     enum mac_ax_feature mac_ft,
			     enum mac_ax_status stat, u8 *buf, u32 size);
#if MAC_AX_FEATURE_DBGCMD
	s32 (*rtl_sprintf)(void *drv_adapter, char *buf, size_t size, const char *fmt, ...);
	s32 (*rtl_strcmp)(void *drv_adapter, const char *s1, const char *s2);
	char* (*rtl_strsep)(void *drv_adapter, char **s, const char *ct);
	u32 (*rtl_strlen)(void *drv_adapter, char *buf);
	char* (*rtl_strcpy)(void *drv_adapter, char *dest, const char *src);
	char* (*rtl_strpbrk)(void *drv_adapter, const char *cs, const char *ct);
	u32 (*rtl_strtoul)(void *drv_adapter, const char *buf, u32 base);
#endif
	void (*ser_l2_notify)(void *phl_com,
			      void *hal_com);

	u8 (*ld_fw_symbol)(void *phl_adapter, void *drv_adapter,
			   const char *name, u8 **buf, u32 *buf_size);
	u8 (*get_chip_id)(void *hal_com);
};
#endif/*CONFIG_NEW_HALMAC_INTERFACE*/

/**
 * @struct mac_ax_adapter
 * @brief mac_ax_adapter
 *
 * @var mac_ax_adapter::ops
 * Please Place Description here.
 * @var mac_ax_adapter::drv_adapter
 * Please Place Description here.
 * @var mac_ax_adapter::phl_adapter
 * Please Place Description here.
 * @var mac_ax_adapter::pltfm_cb
 * Please Place Description here.
 * @var mac_ax_adapter::sm
 * Please Place Description here.
 * @var mac_ax_adapter::hw_info
 * Please Place Description here.
 * @var mac_ax_adapter::fw_info
 * Please Place Description here.
 * @var mac_ax_adapter::efuse_param
 * Please Place Description here.
 * @var mac_ax_adapter::mac_pwr_info
 * Please Place Description here.
 * @var mac_ax_adapter::ft_stat
 * Please Place Description here.
 * @var mac_ax_adapter::hfc_param
 * Please Place Description here.
 * @var mac_ax_adapter::dle_info
 * Please Place Description here.
 * @var mac_ax_adapter::gpio_info
 * Please Place Description here.
 * @var mac_ax_adapter::role_tbl
 * Please Place Description here.
 * @var mac_ax_adapter::read_ofld_info
 * Please Place Description here.
 * @var mac_ax_adapter::read_ofld_value
 * Please Place Description here.
 * @var mac_ax_adapter::write_ofld_info
 * Please Place Description here.
 * @var mac_ax_adapter::efuse_ofld_info
 * Please Place Description here.
 * @var mac_ax_adapter::conf_ofld_info
 * Please Place Description here.
 * @var mac_ax_adapter::pkt_ofld_info
 * Please Place Description here.
 * @var mac_ax_adapter::pkt_ofld_pkt
 * Please Place Description here.
 * @var mac_ax_adapter::mcc_group_info
 * Please Place Description here.
 * @var mac_ax_adapter::wowlan_info
 * Please Place Description here.
 * @var mac_ax_adapter::sdio_info
 * Please Place Description here.
 * @var mac_ax_adapter::usb_info
 * Please Place Description here.
 * @var mac_ax_adapter::hv_ops
 * Please Place Description here.
 */
struct mac_ax_adapter {
	struct mac_ax_ops *ops;
	void *drv_adapter; //hal_com adapter
	void *phl_adapter; //phl_com adapter
	struct mac_ax_pltfm_cb *pltfm_cb;
	struct mac_ax_state_mach sm;
	struct mac_ax_hw_info *hw_info;
	struct mac_ax_fw_info fw_info;
	struct mac_ax_ser_info ser_info;
	struct mac_ax_efuse_param efuse_param;
	struct mac_ax_mac_pwr_info mac_pwr_info;
	struct mac_ax_ft_status *ft_stat;
	struct mac_ax_hfc_param *hfc_param;
	struct mac_ax_dle_info dle_info;
	struct mac_ax_gpio_info gpio_info;
	struct mac_role_tbl_head *role_tbl;
	struct mac_ax_read_ofld_info read_ofld_info;
	struct mac_ax_read_ofld_value read_ofld_value;
	struct mac_ax_write_ofld_info write_ofld_info;
	struct mac_ax_efuse_ofld_info efuse_ofld_info;
	struct mac_ax_conf_ofld_info conf_ofld_info;
	struct mac_ax_pkt_ofld_info pkt_ofld_info;
	struct mac_ax_pkt_ofld_pkt pkt_ofld_pkt;
	struct mac_ax_cmd_ofld_info cmd_ofld_info;
	struct mac_ax_mcc_group_info mcc_group_info;
	struct mac_ax_wowlan_info wowlan_info;
	struct mac_ax_p2p_info *p2p_info;
	struct mac_ax_t32_togl_rpt *t32_togl_rpt;
	struct mac_ax_port_info *port_info;
	struct mac_ax_int_stats stats;
	struct mac_ax_h2c_agg_info h2c_agg_info;
	struct mac_ax_drv_stats drv_stats;
	struct sensing_csi_info csi_info;
	struct mac_ax_nan_info nan_info;
#if MAC_AX_SDIO_SUPPORT
	struct mac_ax_sdio_info sdio_info;
#endif
#if MAC_AX_USB_SUPPORT
	struct mac_ax_usb_info usb_info;
#endif
#if MAC_AX_PCIE_SUPPORT
	struct mac_ax_pcie_info pcie_info;
#endif
	struct mac_ax_flash_info flash_info;
	struct mac_ax_fast_ch_sw_info fast_ch_sw_info;
#if MAC_AX_FEATURE_HV
	struct hv_ax_ops *hv_ops;
	u8 env;
#endif
#if MAC_AX_FEATURE_DBGCMD
	struct mac_ax_fw_dbgcmd fw_dbgcmd;
#endif
#if MAC_AX_FEATURE_DBGDEC
	struct mac_fw_msg *fw_log_array;
	struct mac_fw_msg *fw_log_array_dl;
	u32 fw_log_array_dl_size;
#endif
	struct mac_ax_scanofld_info scanofld_info;
	struct mac_ax_fw_log log_cfg;
	struct mac_ax_twt_info *twt_info;
	struct mac_ax_ch_switch_rpt ch_switch_rpt;
	struct mac_ax_dbcc_info *dbcc_info;
	struct mac_ax_bcn_fltr_rpt bcn_fltr_rpt;
};

/**
 * mac_ax_intf_ops - interface related callbacks
 * @reg_read8:
 * @reg_write8:
 * @reg_read16:
 * @reg_write16:
 * @reg_read32:
 * @reg_write32:
 * @tx_allow_sdio:
 * @tx_cmd_addr_sdio:
 * @init_intf:
 * @reg_read_n_sdio:
 * @get_bulkout_id:
 */

/**
 * @struct mac_ax_intf_ops
 * @brief mac_ax_intf_ops
 *
 * @var mac_ax_intf_ops::reg_read8
 * Please Place Description here.
 * @var mac_ax_intf_ops::reg_write8
 * Please Place Description here.
 * @var mac_ax_intf_ops::reg_read16
 * Please Place Description here.
 * @var mac_ax_intf_ops::reg_write16
 * Please Place Description here.
 * @var mac_ax_intf_ops::reg_read32
 * Please Place Description here.
 * @var mac_ax_intf_ops::reg_write32
 * Please Place Description here.
 * @var mac_ax_intf_ops::tx_allow_sdio
 * Please Place Description here.
 * @var mac_ax_intf_ops::tx_cmd_addr_sdio
 * Please Place Description here.
 * @var mac_ax_intf_ops::intf_pre_init
 * Please Place Description here.
 * @var mac_ax_intf_ops::intf_init
 * Please Place Description here.
 * @var mac_ax_intf_ops::intf_deinit
 * Please Place Description here.
 * @var mac_ax_intf_ops::reg_read_n_sdio
 * Please Place Description here.
 * @var mac_ax_intf_ops::get_bulkout_id
 * Please Place Description here.
 * @var mac_ax_intf_ops::ltr_set_pcie
 * Please Place Description here.
 * @var mac_ax_intf_ops::u2u3_switch
 * Please Place Description here.
 * @var mac_ax_intf_ops::get_usb_mode
 * Please Place Description here.
 * @var mac_ax_intf_ops::get_usb_support_ability
 * Please Place Description here.
 * @var mac_ax_intf_ops::usb_tx_agg_cfg
 * Please Place Description here.
 * @var mac_ax_intf_ops::usb_rx_agg_cfg
 * Please Place Description here.
 * @var mac_ax_intf_ops::set_wowlan
 * Please Place Description here.
 */
struct mac_ax_intf_ops {
	u8 (*reg_read8)(struct mac_ax_adapter *adapter, u32 addr);
	void (*reg_write8)(struct mac_ax_adapter *adapter, u32 addr, u8 val);
	u16 (*reg_read16)(struct mac_ax_adapter *adapter, u32 addr);
	void (*reg_write16)(struct mac_ax_adapter *adapter, u32 addr, u16 val);
	u32 (*reg_read32)(struct mac_ax_adapter *adapter, u32 addr);
	void (*reg_write32)(struct mac_ax_adapter *adapter, u32 addr, u32 val);
	/**
	 * @tx_allow_sdio
	 * Only support SDIO interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*tx_allow_sdio)(struct mac_ax_adapter *adapter,
			     struct mac_ax_sdio_tx_info *info);
	/**
	 * @tx_cmd_addr_sdio
	 * Only support SDIO interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*tx_cmd_addr_sdio)(struct mac_ax_adapter *adapter,
				struct mac_ax_sdio_tx_info *info,
				u32 *cmd_addr);
	u32 (*intf_pre_init)(struct mac_ax_adapter *adapter, void *param);
	u32 (*intf_init)(struct mac_ax_adapter *adapter, void *param);
	u32 (*intf_deinit)(struct mac_ax_adapter *adapter, void *param);
	/**
	 * @reg_read_n_sdio
	 * Only support SDIO interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*reg_read_n_sdio)(struct mac_ax_adapter *adapter, u32 addr,
			       u32 size, u8 *val);
	/**
	 * @get_bulkout_id
	 * Only support USB interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u8 (*get_bulkout_id)(struct mac_ax_adapter *adapter, u8 ch_dma,
			     u8 mode);
	/**
	 * @ltr_set_pcie
	 * Only support PCIe interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*ltr_set_pcie)(struct mac_ax_adapter *adapter,
			    struct mac_ax_pcie_ltr_param *param);
	/**
	 * @u2u3_switch
	 * Only support USB interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*u2u3_switch)(struct mac_ax_adapter *adapter);
	/**
	 * @get_usb_mode
	 * Only support USB interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*get_usb_mode)(struct mac_ax_adapter *adapter);
	/**
	 * @get_usb_support_ability
	 * Only support USB interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*get_usb_support_ability)(struct mac_ax_adapter *adapter);
	/**
	 * @usb_tx_agg_cfg
	 * Only support USB interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*usb_tx_agg_cfg)(struct mac_ax_adapter *adapter,
			      struct mac_ax_usb_tx_agg_cfg *agg);
	/**
	 * @usb_rx_agg_cfg
	 * Only support USB interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*usb_rx_agg_cfg)(struct mac_ax_adapter *adapter,
			      struct mac_ax_rx_agg_cfg *cfg);

	u32 (*set_wowlan)(struct mac_ax_adapter *adapter,
			  enum mac_ax_wow_ctrl w_c);
	/**
	 * @ctrl_txdma_ch
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*ctrl_txdma_ch)(struct mac_ax_adapter *adapter,
			     struct mac_ax_txdma_ch_map *ch_map);
	/**
	 * @clr_idx_all
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*clr_idx_all)(struct mac_ax_adapter *adapter);
	/**
	 * @poll_txdma_ch_idle
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*poll_txdma_ch_idle)(struct mac_ax_adapter *adapter,
				  struct mac_ax_txdma_ch_map *ch_map);
	/**
	 * @poll_rxdma_ch_idle
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*poll_rxdma_ch_idle)(struct mac_ax_adapter *adapter,
				  struct mac_ax_rxdma_ch_map *ch_map);
	/**
	 * @set_pcie_speed
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*set_pcie_speed)(struct mac_ax_adapter *adapter,
			      enum mac_ax_pcie_phy speed);
	/**
	 * @get_pcie_speed
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*get_pcie_speed)(struct mac_ax_adapter *adapter,
			      u8 *speed);
	/**
	 * @ctrl_txhci
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*ctrl_txhci)(struct mac_ax_adapter *adapter,
			  enum mac_ax_func_sw en);
	/**
	 * @ctrl_rxhci
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*ctrl_rxhci)(struct mac_ax_adapter *adapter,
			  enum mac_ax_func_sw en);
	/**
	 * @ctrl_dma_io
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*ctrl_dma_io)(struct mac_ax_adapter *adapter,
			   enum mac_ax_func_sw en);
	/**
	 * @get_io_stat
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*get_io_stat)(struct mac_ax_adapter *adapter,
			   struct mac_ax_io_stat *out_st);
	/**
	 * @get_txagg_num
	 */
	u32 (*get_txagg_num)(struct mac_ax_adapter *adapter,
			     u8 band);
	/**
	 * @get_avail_txbd
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*get_avail_txbd)(struct mac_ax_adapter *adapter, u8 ch_idx,
			      u16 *host_idx, u16 *hw_idx, u16 *avail_txbd);
	/**
	 * @get_avail_rxbd
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*get_avail_rxbd)(struct mac_ax_adapter *adapter, u8 ch_idx,
			      u16 *host_idx, u16 *hw_idx, u16 *avail_rxbd);
	/**
	 * @trigger_txdma
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*trigger_txdma)(struct mac_ax_adapter *adapter,
			     struct tx_base_desc *txbd_ring, u8 ch_idx);
	/**
	 * @notify_rxdone
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*notify_rxdone)(struct mac_ax_adapter *adapter,
			     struct rx_base_desc *rxbd, u8 ch);
	/**
	 * @get_rx_state
	 * Only support USB interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*get_rx_state)(struct mac_ax_adapter *adapter, u32 *val);
	/**
	 * @dbcc_hci_pause
	 * Support all interface.
	 */
	u32 (*dbcc_hci_ctrl)(struct mac_ax_adapter *adapter,
			     struct mac_ax_dbcc_hci_ctrl *info);
	/**
	 * @pcie_autok_counter_avg
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*pcie_autok_counter_avg)(struct mac_ax_adapter *adapter);

	/**
	 * @tp_adjust
	 * Only support PCIE interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*tp_adjust)(struct mac_ax_adapter *adapter,
			 struct mac_ax_tp_param tp);
};

/**
 * struct mac_ax_ops - callbacks for mac control
 * All callbacks can be used after initializing mac_ax_ops by mac_ax_ops_init.
 * @intf_ops: interface related callbacks, refer struct mac_ax_intf_ops to get
 *	more deatails.
 * @get_hw_info: get mac hardware information
 * @mac_txdesc_len:
 * @build_mac_txdesc:
 */

/**
 * @struct mac_ax_ops
 * @brief mac_ax_ops
 *
 * @var mac_ax_ops::intf_ops
 * Please Place Description here.
 * @var mac_ax_ops::hal_init
 * Please Place Description here.
 * @var mac_ax_ops::hal_fast_init
 * Please Place Description here.
 * @var mac_ax_ops::hal_deinit
 * Please Place Description here.
 * @var mac_ax_ops::hal_fast_deinit
 * Please Place Description here.
 * @var mac_ax_ops::add_role
 * Please Place Description here.
 * @var mac_ax_ops::remove_role
 * Please Place Description here.
 * @var mac_ax_ops::change_role
 * Please Place Description here.
 * @var mac_ax_ops::pwr_switch
 * Please Place Description here.
 * @var mac_ax_ops::sys_init
 * Please Place Description here.
 * @var mac_ax_ops::trx_init
 * Please Place Description here.
 * @var mac_ax_ops::feat_init
 * Please Place Description here.
 * @var mac_ax_ops::romdl
 * Please Place Description here.
 * @var mac_ax_ops::enable_cpu
 * Please Place Description here.
 * @var mac_ax_ops::disable_cpu
 * Please Place Description here.
 * @var mac_ax_ops::fwredl
 * Please Place Description here.
 * @var mac_ax_ops::fwdl
 * Please Place Description here.
 * @var mac_ax_ops::enable_fw
 * Please Place Description here.
 * @var mac_ax_ops::lv1_rcvy
 * Please Place Description here.
 * @var mac_ax_ops::get_macaddr
 * Please Place Description here.
 * @var mac_ax_ops::build_txdesc
 * Please Place Description here.
 * @var mac_ax_ops::refill_txdesc
 * Please Place Description here.
 * @var mac_ax_ops::parse_rxdesc
 * Please Place Description here.
 * @var mac_ax_ops::reset_fwofld_state
 * Please Place Description here.
 * @var mac_ax_ops::check_fwofld_done
 * Please Place Description here.
 * @var mac_ax_ops::read_pkt_ofld
 * Please Place Description here.
 * @var mac_ax_ops::del_pkt_ofld
 * Please Place Description here.
 * @var mac_ax_ops::add_pkt_ofld
 * Please Place Description here.
 * @var mac_ax_ops::pkt_ofld_packet
 * Please Place Description here.
 * @var mac_ax_ops::dump_efuse_ofld
 * Please Place Description here.
 * @var mac_ax_ops::efuse_ofld_map
 * Please Place Description here.
 * @var mac_ax_ops::upd_dctl_info
 * Please Place Description here.
 * @var mac_ax_ops::upd_cctl_info
 * Please Place Description here.
 * @var mac_ax_ops::ie_cam_upd
 * Please Place Description here.
 * @var mac_ax_ops::twt_info_upd_h2c
 * Please Place Description here.
 * @var mac_ax_ops::twt_act_h2c
 * Please Place Description here.
 * @var mac_ax_ops::twt_anno_h2c
 * Please Place Description here.
 * @var mac_ax_ops::twt_wait_anno
 * Please Place Description here.
 * @var mac_ax_ops::mac_host_getpkt_h2c
 * Please Place Description here.
 * @var mac_ax_ops::p2p_act_h2c
 * Please Place Description here.
 * @var mac_ax_ops::sta_add_key
 * Please Place Description here.
 * @var mac_ax_ops::sta_del_key
 * Please Place Description here.
 * @var mac_ax_ops::sta_search_key_idx
 * Please Place Description here.
 * @var mac_ax_ops::sta_hw_security_support
 * Please Place Description here.
 * @var mac_ax_ops::set_mu_table
 * Please Place Description here.
 * @var mac_ax_ops::ss_dl_grp_upd
 * Please Place Description here.
 * @var mac_ax_ops::ss_ul_grp_upd
 * Please Place Description here.
 * @var mac_ax_ops::ss_ul_sta_upd
 * Please Place Description here.
 * @var mac_ax_ops::bacam_avl_std_entry_idx
 * Please Place Description here.
 * @var mac_ax_ops::bacam_info
 * Please Place Description here.
 * @var mac_ax_ops::txdesc_len
 * Please Place Description here.
 * @var mac_ax_ops::upd_shcut_mhdr
 * Please Place Description here.
 * @var mac_ax_ops::enable_hwmasdu
 * Please Place Description here.
 * @var mac_ax_ops::enable_cut_hwamsdu
 * Please Place Description here.
 * @var mac_ax_ops::hdr_conv
 * Please Place Description here.
 * @var mac_ax_ops::set_hwseq_reg
 * Please Place Description here.
 * @var mac_ax_ops::process_c2h
 * Please Place Description here.
 * @var mac_ax_ops::parse_dfs
 * Please Place Description here.
 * @var mac_ax_ops::parse_ppdu
 * Please Place Description here.
 * @var mac_ax_ops::cfg_phy_rpt
 * Please Place Description here.
 * @var mac_ax_ops::set_rx_forwarding
 * Please Place Description here.
 * @var mac_ax_ops::get_rx_fltr_opt
 * Please Place Description here.
 * @var mac_ax_ops::set_rx_fltr_opt
 * Please Place Description here.
 * @var mac_ax_ops::set_rx_fltr_typ_opt
 * Please Place Description here.
 * @var mac_ax_ops::set_rx_fltr_typstyp_opt
 * Please Place Description here.
 * @var mac_ax_ops::sr_update
 * Please Place Description here.
 * @var mac_ax_ops::two_nav_cfg
 * Please Place Description here.
 * @var mac_ax_ops::pkt_drop
 * Please Place Description here.
 * @var mac_ax_ops::send_bcn_h2c
 * Please Place Description here.
 * @var mac_ax_ops::tx_mode_sel
 * Please Place Description here.
 * @var mac_ax_ops::tcpip_chksum_ofd
 * Please Place Description here.
 * @var mac_ax_ops::chk_rx_tcpip_chksum_ofd
 * Please Place Description here.
 * @var mac_ax_ops::chk_allq_empty
 * Please Place Description here.
 * @var mac_ax_ops::is_txq_empty
 * Please Place Description here.
 * @var mac_ax_ops::is_rxq_empty
 * Please Place Description here.
 * @var mac_ax_ops::parse_bcn_stats_c2h
 * Please Place Description here.
 * @var mac_ax_ops::upd_mudecision_para
 * Please Place Description here.
 * @var mac_ax_ops::mu_sta_upd
 * Please Place Description here.
 * @var mac_ax_ops::upd_ul_fixinfo
 * Please Place Description here.
 * @var mac_ax_ops::f2p_test_cmd
 * Please Place Description here.
 * @var mac_ax_ops::snd_test_cmd
 * Please Place Description here.
 * @var mac_ax_ops::set_fw_fixmode
 * Please Place Description here.
 * @var mac_ax_ops::mac_dumpwlanc
 * Please Place Description here.
 * @var mac_ax_ops::mac_dumpwlans
 * Please Place Description here.
 * @var mac_ax_ops::mac_dumpwland
 * Please Place Description here.
 * @var mac_ax_ops::outsrc_h2c_common
 * Please Place Description here.
 * @var mac_ax_ops::read_pwr_reg
 * Please Place Description here.
 * @var mac_ax_ops::write_pwr_reg
 * Please Place Description here.
 * @var mac_ax_ops::write_msk_pwr_reg
 * Please Place Description here.
 * @var mac_ax_ops::write_pwr_ofst_mode
 * Please Place Description here.
 * @var mac_ax_ops::write_pwr_ofst_bw
 * Please Place Description here.
 * @var mac_ax_ops::write_pwr_ref_reg
 * Please Place Description here.
 * @var mac_ax_ops::write_pwr_limit_en
 * Please Place Description here.
 * @var mac_ax_ops::write_pwr_limit_rua_reg
 * Please Place Description here.
 * @var mac_ax_ops::write_pwr_limit_reg
 * Please Place Description here.
 * @var mac_ax_ops::write_pwr_by_rate_reg
 * Please Place Description here.
 * @var mac_ax_ops::lamode_cfg
 * Please Place Description here.
 * @var mac_ax_ops::lamode_trigger
 * Please Place Description here.
 * @var mac_ax_ops::lamode_buf_cfg
 * Please Place Description here.
 * @var mac_ax_ops::get_lamode_st
 * Please Place Description here.
 * @var mac_ax_ops::read_xcap_reg
 * Please Place Description here.
 * @var mac_ax_ops::write_xcap_reg
 * Please Place Description here.
 * @var mac_ax_ops::write_bbrst_reg
 * Please Place Description here.
 * @var mac_ax_ops::get_csi_buffer_index
 * Please Place Description here.
 * @var mac_ax_ops::set_csi_buffer_index
 * Please Place Description here.
 * @var mac_ax_ops::get_snd_sts_index
 * Please Place Description here.
 * @var mac_ax_ops::set_snd_sts_index
 * Please Place Description here.
 * @var mac_ax_ops::init_snd_mer
 * Please Place Description here.
 * @var mac_ax_ops::init_snd_mee
 * Please Place Description here.
 * @var mac_ax_ops::csi_force_rate
 * Please Place Description here.
 * @var mac_ax_ops::csi_rrsc
 * Please Place Description here.
 * @var mac_ax_ops::set_snd_para
 * Please Place Description here.
 * @var mac_ax_ops::set_csi_para_reg
 * Please Place Description here.
 * @var mac_ax_ops::set_csi_para_cctl
 * Please Place Description here.
 * @var mac_ax_ops::hw_snd_pause_release
 * Please Place Description here.
 * @var mac_ax_ops::bypass_snd_sts
 * Please Place Description here.
 * @var mac_ax_ops::deinit_mee
 * Please Place Description here.
 * @var mac_ax_ops::cfg_lps
 * Please Place Description here.
 * @var mac_ax_ops::ps_pwr_state
 * Please Place Description here.
 * @var mac_ax_ops::chk_leave_lps
 * Please Place Description here.
 * @var mac_ax_ops::cfg_ips
 * Please Place Description here.
 * @var mac_ax_ops::chk_leave_ips
 * Please Place Description here.
 * @var mac_ax_ops::lps_chk_access
 * Please Place Description here.
 * @var mac_ax_ops::cfg_wow_wake
 * Please Place Description here.
 * @var mac_ax_ops::cfg_disconnect_det
 * Please Place Description here.
 * @var mac_ax_ops::cfg_keepalive
 * Please Place Description here.
 * @var mac_ax_ops::cfg_gtk_ofld
 * Please Place Description here.
 * @var mac_ax_ops::cfg_arp_ofld
 * Please Place Description here.
 * @var mac_ax_ops::cfg_ndp_ofld
 * Please Place Description here.
 * @var mac_ax_ops::cfg_realwow
 * Please Place Description here.
 * @var mac_ax_ops::cfg_nlo
 * Please Place Description here.
 * @var mac_ax_ops::cfg_dev2hst_gpio
 * Please Place Description here.
 * @var mac_ax_ops::cfg_uphy_ctrl
 * Please Place Description here.
 * @var mac_ax_ops::cfg_wowcam_upd
 * Please Place Description here.
 * @var mac_ax_ops::cfg_wow_sleep
 * Please Place Description here.
 * @var mac_ax_ops::get_wow_fw_status
 * Please Place Description here.
 * @var mac_ax_ops::request_aoac_report
 * Please Place Description here.
 * @var mac_ax_ops::read_aoac_report
 * Please Place Description here.
 * @var mac_ax_ops::check_aoac_report_done
 * Please Place Description here.
 * @var mac_ax_ops::dbcc_enable
 * Please Place Description here.
 * @var mac_ax_ops::port_cfg
 * Please Place Description here.
 * @var mac_ax_ops::port_init
 * Please Place Description here.
 * @var mac_ax_ops::enable_imr
 * Please Place Description here.
 * @var mac_ax_ops::dump_efuse_map_wl
 * Please Place Description here.
 * @var mac_ax_ops::dump_efuse_map_bt
 * Please Place Description here.
 * @var mac_ax_ops::write_efuse
 * Please Place Description here.
 * @var mac_ax_ops::read_efuse
 * Please Place Description here.
 * @var mac_ax_ops::read_hidden_efuse
 * Please Place Description here.
 * @var mac_ax_ops::get_efuse_avl_size
 * Please Place Description here.
 * @var mac_ax_ops::get_efuse_avl_size_bt
 * Please Place Description here.
 * @var mac_ax_ops::dump_log_efuse
 * Please Place Description here.
 * @var mac_ax_ops::read_log_efuse
 * Please Place Description here.
 * @var mac_ax_ops::write_log_efuse
 * Please Place Description here.
 * @var mac_ax_ops::dump_log_efuse_bt
 * Please Place Description here.
 * @var mac_ax_ops::read_log_efuse_bt
 * Please Place Description here.
 * @var mac_ax_ops::write_log_efuse_bt
 * Please Place Description here.
 * @var mac_ax_ops::pg_efuse_by_map
 * Please Place Description here.
 * @var mac_ax_ops::pg_efuse_by_map_bt
 * Please Place Description here.
 * @var mac_ax_ops::mask_log_efuse
 * Please Place Description here.
 * @var mac_ax_ops::pg_sec_data_by_map
 * Please Place Description here.
 * @var mac_ax_ops::cmp_sec_data_by_map
 * Please Place Description here.
 * @var mac_ax_ops::get_efuse_info
 * Please Place Description here.
 * @var mac_ax_ops::set_efuse_info
 * Please Place Description here.
 * @var mac_ax_ops::read_hidden_rpt
 * Please Place Description here.
 * @var mac_ax_ops::check_efuse_autoload
 * Please Place Description here.
 * @var mac_ax_ops::pg_simulator
 * Please Place Description here.
 * @var mac_ax_ops::checksum_update
 * Please Place Description here.
 * @var mac_ax_ops::checksum_rpt
 * Please Place Description here.
 * @var mac_ax_ops::set_efuse_ctrl
 * Please Place Description here.
 * @var mac_ax_ops::otp_test
 * Please Place Description here.
 * @var mac_ax_ops::get_mac_ft_status
 * Please Place Description here.
 * @var mac_ax_ops::fw_log_cfg
 * Please Place Description here.
 * @var mac_ax_ops::pinmux_set_func
 * Please Place Description here.
 * @var mac_ax_ops::pinmux_free_func
 * Please Place Description here.
 * @var mac_ax_ops::sel_uart_tx_pin
 * Please Place Description here.
 * @var mac_ax_ops::sel_uart_rx_pin
 * Please Place Description here.
 * @var mac_ax_ops::set_gpio_func
 * Please Place Description here.
 * @var mac_ax_ops::get_gpio_val
 * Please Place Description here.
 * @var mac_ax_ops::get_hw_info
 * Please Place Description here.
 * @var mac_ax_ops::set_hw_value
 * Please Place Description here.
 * @var mac_ax_ops::get_hw_value
 * Please Place Description here.
 * @var mac_ax_ops::get_err_status
 * Please Place Description here.
 * @var mac_ax_ops::set_err_status
 * Please Place Description here.
 * @var mac_ax_ops::general_pkt_ids
 * Please Place Description here.
 * @var mac_ax_ops::coex_init
 * Please Place Description here.
 * @var mac_ax_ops::coex_read
 * Please Place Description here.
 * @var mac_ax_ops::coex_write
 * Please Place Description here.
 * @var mac_ax_ops::trigger_cmac_err
 * Please Place Description here.
 * @var mac_ax_ops::trigger_cmac1_err
 * Please Place Description here.
 * @var mac_ax_ops::trigger_dmac_err
 * Please Place Description here.
 * @var mac_ax_ops::tsf_sync
 * Please Place Description here.
 * @var mac_ax_ops::reset_mcc_group
 * Please Place Description here.
 * @var mac_ax_ops::reset_mcc_request
 * Please Place Description here.
 * @var mac_ax_ops::add_mcc
 * Please Place Description here.
 * @var mac_ax_ops::start_mcc
 * Please Place Description here.
 * @var mac_ax_ops::stop_mcc
 * Please Place Description here.
 * @var mac_ax_ops::del_mcc_group
 * Please Place Description here.
 * @var mac_ax_ops::mcc_request_tsf
 * Please Place Description here.
 * @var mac_ax_ops::mcc_macid_bitmap
 * Please Place Description here.
 * @var mac_ax_ops::mcc_sync_enable
 * Please Place Description here.
 * @var mac_ax_ops::mcc_set_duration
 * Please Place Description here.
 * @var mac_ax_ops::get_mcc_tsf_rpt
 * Please Place Description here.
 * @var mac_ax_ops::get_mcc_status_rpt
 * Please Place Description here.
 * @var mac_ax_ops::check_add_mcc_done
 * Please Place Description here.
 * @var mac_ax_ops::check_start_mcc_done
 * Please Place Description here.
 * @var mac_ax_ops::check_stop_mcc_done
 * Please Place Description here.
 * @var mac_ax_ops::check_del_mcc_group_done
 * Please Place Description here.
 * @var mac_ax_ops::check_mcc_request_tsf_done
 * Please Place Description here.
 * @var mac_ax_ops::check_mcc_macid_bitmap_done
 * Please Place Description here.
 * @var mac_ax_ops::check_mcc_sync_enable_done
 * Please Place Description here.
 * @var mac_ax_ops::check_mcc_set_duration_done
 * Please Place Description here.
 * @var mac_ax_ops::check_access
 * Please Place Description here.
 * @var mac_ax_ops::set_led_mode
 * Please Place Description here.
 * @var mac_ax_ops::led_ctrl
 * Please Place Description here.
 * @var mac_ax_ops::set_sw_gpio_mode
 * Please Place Description here.
 * @var mac_ax_ops::sw_gpio_ctrl
 * Please Place Description here.
 * @var mac_ax_ops::fwcmd_lb
 * Please Place Description here.
 * @var mac_ax_ops::mem_dump
 * Please Place Description here.
 * @var mac_ax_ops::get_mem_size
 * Please Place Description here.
 * @var mac_ax_ops::dbg_status_dump
 * Please Place Description here.
 * @var mac_ax_ops::reg_dump
 * Please Place Description here.
 * @var mac_ax_ops::rx_cnt
 * Please Place Description here.
 * @var mac_ax_ops::dump_fw_rsvd_ple
 * Please Place Description here.
 * @var mac_ax_ops::fw_dbg_dump
 * Please Place Description here.
 * @var mac_ax_ops::event_notify
 * Please Place Description here.
 * @var mac_ax_ops::ram_boot
 * Please Place Description here.
 * @var mac_ax_ops::clear_write_request
 * Please Place Description here.
 * @var mac_ax_ops::add_write_request
 * Please Place Description here.
 * @var mac_ax_ops::write_ofld
 * Please Place Description here.
 * @var mac_ax_ops::clear_conf_request
 * Please Place Description here.
 * @var mac_ax_ops::add_conf_request
 * Please Place Description here.
 * @var mac_ax_ops::conf_ofld
 * Please Place Description here.
 * @var mac_ax_ops::clear_read_request
 * Please Place Description here.
 * @var mac_ax_ops::add_read_request
 * Please Place Description here.
 * @var mac_ax_ops::read_ofld
 * Please Place Description here.
 * @var mac_ax_ops::read_ofld_value
 * Please Place Description here.
 * @var mac_ax_ops::get_fw_status
 * Please Place Description here.
 */
struct mac_ax_ops {
	struct mac_ax_intf_ops *intf_ops;
	/*System level*/
	u32 (*hal_init)(struct mac_ax_adapter *adapter,
			struct mac_ax_trx_info *trx_info,
			struct mac_ax_fwdl_info *fwdl_info,
			struct mac_ax_intf_info *intf_info);
	u32 (*hal_fast_init)(struct mac_ax_adapter *adapter,
			     struct mac_ax_trx_info *trx_info,
			     struct mac_ax_fwdl_info *fwdl_info,
			     struct mac_ax_intf_info *intf_info);
	u32 (*hal_deinit)(struct mac_ax_adapter *adapter);
	u32 (*hal_fast_deinit)(struct mac_ax_adapter *adapter);
	u32 (*add_role)(struct mac_ax_adapter *adapter,
			struct mac_ax_role_info *info);
	u32 (*remove_role)(struct mac_ax_adapter *adapter, u8 macid);
	u32 (*change_role)(struct mac_ax_adapter *adapter,
			   struct mac_ax_role_info *info);
	u32 (*pwr_switch)(struct mac_ax_adapter *adapter, u8 on);
	u32 (*sys_init)(struct mac_ax_adapter *adapter);
	u32 (*trx_init)(struct mac_ax_adapter *adapter,
			struct mac_ax_trx_info *info);
	u32 (*feat_init)(struct mac_ax_adapter *adapter,
			 struct mac_ax_trx_info *info);
	u32 (*romdl)(struct mac_ax_adapter *adapter, u8 *rom, u32 romaddr,
		     u32 len);
	u32 (*enable_cpu)(struct mac_ax_adapter *adapter,
			  u8 boot_reason, u8 dlfw);
	u32 (*disable_cpu)(struct mac_ax_adapter *adapter);
	u32 (*fwredl)(struct mac_ax_adapter *adapter, u8 *fw, u32 len);
	u32 (*fwdl)(struct mac_ax_adapter *adapter, u8 *fw, u32 len);
	u32 (*query_fw_buff)(struct mac_ax_adapter *adapter,
			     enum rtw_fw_type cat, u8 **fw, u32 *fw_len);
	u32 (*enable_fw)(struct mac_ax_adapter *adapter,
			 enum rtw_fw_type cat);
	u32 (*get_dynamic_hdr)(struct mac_ax_adapter *adapter, u8 *fw, u32 fw_len);
	u32 (*lv1_rcvy)(struct mac_ax_adapter *adapter,
			enum mac_ax_lv1_rcvy_step step);
	u32 (*get_macaddr)(struct mac_ax_adapter *adapter,
			   struct mac_ax_macaddr *macaddr,
			   u8 role_idx);
	u32 (*build_txdesc)(struct mac_ax_adapter *adapter,
			    struct rtw_t_meta_data *info, u8 *buf, u32 len);
	u32 (*refill_txdesc)(struct mac_ax_adapter *adapter,
			     struct rtw_t_meta_data *txpkt_info,
			     struct mac_ax_refill_info *mask,
			     struct mac_ax_refill_info *info);
	u32 (*parse_rxdesc)(struct mac_ax_adapter *adapter,
			    struct mac_ax_rxpkt_info *info, u8 *buf, u32 len);
	u32 (*watchdog)(struct mac_ax_adapter *adapter,
			struct mac_ax_wdt_param *wdt_param);
	/*FW offload related*/
	u32 (*reset_fwofld_state)(struct mac_ax_adapter *adapter, u8 op);
	u32 (*check_fwofld_done)(struct mac_ax_adapter *adapter, u8 op);
	u32 (*read_pkt_ofld)(struct mac_ax_adapter *adapter, u8 id);
	u32 (*del_pkt_ofld)(struct mac_ax_adapter *adapter, u8 id);
	u32 (*add_pkt_ofld)(struct mac_ax_adapter *adapter, u8 *pkt,
			    u16 len, u8 *id);
	u32 (*pkt_ofld_packet)(struct mac_ax_adapter *adapter,
			       u8 **pkt_buf, u16 *pkt_len, u8 *pkt_id);
	u32 (*dump_efuse_ofld)(struct mac_ax_adapter *adapter, u32 efuse_size,
			       u8 type);
	u32 (*efuse_ofld_map)(struct mac_ax_adapter *adapter, u8 *efuse_map,
			      u32 efuse_size);
	u32 (*upd_dctl_info)(struct mac_ax_adapter *adapter,
			     struct mac_ax_dctl_info *info,
			     struct mac_ax_dctl_info *mask, u8 macid,
			     u8 operation);
	u32 (*upd_cctl_info)(struct mac_ax_adapter *adapter,
			     struct rtw_hal_mac_ax_cctl_info *info,
			     struct rtw_hal_mac_ax_cctl_info *mask, u8 macid,
			     u8 operation);
	u32 (*ie_cam_upd)(struct mac_ax_adapter *adapter,
			  struct mac_ax_ie_cam_cmd_info *info);
	u32 (*twt_info_upd_h2c)(struct mac_ax_adapter *adapter,
				struct mac_ax_twt_para *info);
	u32 (*twt_act_h2c)(struct mac_ax_adapter *adapter,
			   struct mac_ax_twtact_para *info);
	u32 (*twt_anno_h2c)(struct mac_ax_adapter *adapter,
			    struct mac_ax_twtanno_para *info);
	void (*twt_wait_anno)(struct mac_ax_adapter *adapter,
			      u8 *c2h_content, u8 *upd_addr);
	u32 (*mac_host_getpkt_h2c)(struct mac_ax_adapter *adapter,
				   u8 macid, u8 pkttype);
	u32 (*p2p_act_h2c)(struct mac_ax_adapter *adapter,
			   struct mac_ax_p2p_act_info *info);
	u32 (*p2p_macid_ctrl_h2c)(struct mac_ax_adapter *adapter,
				  struct mac_ax_p2p_macid_info *info);
	u32 (*get_p2p_stat)(struct mac_ax_adapter *adapter);
	u32 (*tsf32_togl_h2c)(struct mac_ax_adapter *adapter,
			      struct mac_ax_t32_togl_info *info);
	u32 (*get_t32_togl_rpt)(struct mac_ax_adapter *adapter,
				struct mac_ax_t32_togl_rpt *ret_rpt);
	u32 (*ccxrpt_parsing)(struct mac_ax_adapter *adapter,
			      u8 *buf, struct mac_ax_ccxrpt *info);
	u32 (*host_efuse_rec)(struct mac_ax_adapter *adapter,
			      u32 host_id, u32 efuse_val);
	u32 (*cfg_sensing_csi)(struct mac_ax_adapter *adapter,
			       struct rtw_hal_mac_sensing_csi_param *param);
	u32 (*chk_sensing_csi_done)(struct mac_ax_adapter *adapter,
				    u8 chk_state);
	u32 (*calc_crc)(struct mac_ax_adapter *adapter, struct mac_calc_crc_info *info);
	u32 (*bcn_ofld_ctrl)(struct mac_ax_adapter *adapter, struct mac_bcn_ofld_info *info);
	/*Association, de-association related*/
	u32 (*sta_add_key)(struct mac_ax_adapter *adapter,
			   struct mac_ax_sec_cam_info *sec_cam_content,
			   u8 mac_id, u8 key_id, u8 key_type);
	u32 (*sta_del_key)(struct mac_ax_adapter *adapter,
			   u8 mac_id, u8 key_id, u8 key_type);
	u32 (*sta_search_key_idx)(struct mac_ax_adapter *adapter,
				  u8 mac_id, u8 key_id, u8 key_type);
	u32 (*sta_hw_security_support)(struct mac_ax_adapter *adapter,
				       u8 hw_security_support_type, u8 enable);
	u32 (*set_mu_table)(struct mac_ax_adapter *adapter,
			    struct mac_mu_table *mu_table);
	u32 (*ss_dl_grp_upd)(struct mac_ax_adapter *adapter,
			     struct mac_ax_ss_dl_grp_upd *info);
	u32 (*ss_ul_grp_upd)(struct mac_ax_adapter *adapter,
			     struct mac_ax_ss_ul_grp_upd *info);
	u32 (*ss_ul_sta_upd)(struct mac_ax_adapter *adapter,
			     struct mac_ax_ss_ul_sta_upd *info);
	u32 (*bacam_avl_std_entry_idx)(struct mac_ax_adapter *adapter,
				       struct mac_ax_avl_std_bacam_info *info);
	u32 (*bacam_info)(struct mac_ax_adapter *adapter,
			  struct mac_ax_bacam_info *info);
	/*TRX related*/
	u32 (*txdesc_len)(struct mac_ax_adapter *adapter,
			  struct rtw_t_meta_data *info);
	u32 (*upd_shcut_mhdr)(struct mac_ax_adapter *adapter,
			      struct mac_ax_shcut_mhdr *info, u8 macid);
	u32 (*enable_hwamsdu)(struct mac_ax_adapter *adapter,
			      u8 enable,
			      enum mac_ax_amsdu_pkt_num max_num,
			      u8 en_single_amsdu,
			      u8 en_last_amsdu_padding);
	u32 (*hwamsdu_fwd_search_en)(struct mac_ax_adapter *adapter,
				     u8 enable);
	u32 (*hwamsdu_macid_en)(struct mac_ax_adapter *adapter,
				u8 macid, u8 enable);
	u8 (*hwamsdu_get_macid_en)(struct mac_ax_adapter *adapter,
				   u8 macid);
	u32 (*hwamsdu_max_len)(struct mac_ax_adapter *adapter,
			       u8 macid, u8 amsdu_max_len);
	u8 (*hwamsdu_get_max_len)(struct mac_ax_adapter *adapter,
				  u8 macid);
	u32 (*enable_cut_hwamsdu)(struct mac_ax_adapter *adapter,
				  u8 enable,
				  enum mac_ax_ex_shift aligned);
	u32 (*cut_hwamsdu_chk_mpdu_len_en)(struct mac_ax_adapter *adapter,
					   u8 enable,
					   u8 low_th,
					   u16 high_th);
	u32 (*hdr_conv)(struct mac_ax_adapter *adapter,
			u8 en_hdr_conv);
	u32 (*hdr_conv_tx_set_eth_type)(struct mac_ax_adapter *adapter,
					u8 eth_type_idx, u16 eth_type);
	u32 (*hdr_conv_tx_get_eth_type)(struct mac_ax_adapter *adapter,
					u8 eth_type_idx, u16 *eth_type);
	u32 (*hdr_conv_tx_set_oui)(struct mac_ax_adapter *adapter,
				   u8 oui_idx, u32 oui_val, u8 enable);
	u32 (*hdr_conv_tx_get_oui)(struct mac_ax_adapter *adapter,
				   u8 oui_idx, u32 *oui_val, u8 *enable);
	u32 (*hdr_conv_tx_macid_en)(struct mac_ax_adapter *adapter,
				    u8 macid,
				    struct mac_ax_hdr_conv_dctl_cfg dctl_hdr_conv_cfg);
	u32 (*hdr_conv_tx_vlan_tag_valid_en)(struct mac_ax_adapter *adapter,
					     u8 macid, u8 vlan_tag_valid_en);
	u8 (*hdr_conv_tx_get_vlan_tag_valid)(struct mac_ax_adapter *adapter,
					     u8 macid);
	u32 (*hdr_conv_tx_qos_field_en)(struct mac_ax_adapter *adapter,
					u8 macid, u8 wlan_with_qosc_en, u8 qos_field_h);
	u8 (*hdr_conv_tx_get_qos_field_en)(struct mac_ax_adapter *adapter,
					   u8 macid);
	u8 (*hdr_conv_tx_get_qos_field_h)(struct mac_ax_adapter *adapter,
					  u8 macid);
	u32 (*hdr_conv_tx_target_wlan_hdr_len)(struct mac_ax_adapter *adapter,
					       u8 macid, u8 wlan_hdr_len);
	u8 (*hdr_conv_tx_get_target_wlan_hdr_len)(struct mac_ax_adapter *adapter,
						  u8 macid);
	u32 (*hdr_conv_rx_en)(struct mac_ax_adapter *adapter,
			      struct mac_ax_rx_hdr_conv_cfg *cfg);
	u32 (*hdr_conv_rx_en_driv_info_hdr)(struct mac_ax_adapter *adapter,
					    struct mac_ax_rx_driv_info_hdr_cfg *cfg);
	u32 (*set_hwseq_reg)(struct mac_ax_adapter *adapter,
			     u8 reg_seq_idx,
			     u16 reg_seq_val);
	u32 (*set_hwseq_dctrl)(struct mac_ax_adapter *adapter,
			       u8 macid,
			       struct mac_ax_dctl_seq_cfg *seq_info);
	u32 (*get_hwseq_cfg)(struct mac_ax_adapter *adapter,
			     u8 macid, u8 ref_sel,
			     struct mac_ax_dctl_seq_cfg *seq_info);
	u32 (*process_c2h)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			   u8 *ret);
	u32 (*parse_dfs)(struct mac_ax_adapter *adapter,
			 u8 *buf, u32 dfs_len, struct mac_ax_dfs_rpt *rpt);
	u32 (*parse_ppdu)(struct mac_ax_adapter *adapter,
			  u8 *buf, u32 ppdu_len, u8 mac_info,
			  struct mac_ax_ppdu_rpt *rpt);
	u32 (*cfg_phy_rpt)(struct mac_ax_adapter *adapter,
			   struct mac_ax_phy_rpt_cfg *cfg);
	u32 (*set_rx_forwarding)(struct mac_ax_adapter *adapter,
				 struct mac_ax_rx_fwd_ctrl_t *rf_ctrl_p);
	u32 (*get_rx_fltr_opt)(struct mac_ax_adapter *adapter,
			       struct mac_ax_rx_fltr_ctrl_t *opt,
			       enum mac_ax_band band);
	u32 (*set_rx_fltr_opt)(struct mac_ax_adapter *adapter,
			       struct mac_ax_rx_fltr_ctrl_t *opt,
			       struct mac_ax_rx_fltr_ctrl_t *opt_msk,
			       enum mac_ax_band band);
	u32 (*set_rx_fltr_typ_opt)(struct mac_ax_adapter *adapter,
				   enum mac_ax_pkt_t type,
				   enum mac_ax_fwd_target fwd_target,
				   enum mac_ax_band band);
	u32 (*set_rx_fltr_typstyp_opt)(struct mac_ax_adapter *adapter,
				       enum mac_ax_pkt_t type,
				       u8 subtype,
				       enum mac_ax_fwd_target fwd_target,
				       enum mac_ax_band band);
	u32 (*set_typsbtyp_fltr_detail)(struct mac_ax_adapter *adapter,
					enum mac_ax_pkt_t type,
					struct mac_ax_rx_fltr_elem *elem,
					enum mac_ax_band band);
	u32 (*get_cfg_addr_cam)(struct mac_ax_adapter *adapter,
				struct mac_ax_addrcam_ctrl_t *opt,
				enum mac_ax_band band);
	u32 (*get_cfg_addr_cam_dis)(struct mac_ax_adapter *adapter,
				    struct mac_ax_addrcam_dis_ctrl_t *opt,
				    enum mac_ax_band band);
	u32 (*cfg_addr_cam)(struct mac_ax_adapter *adapter,
			    struct mac_ax_addrcam_ctrl_t *ctl_opt,
			    struct mac_ax_addrcam_ctrl_t *ctl_msk,
			    enum mac_ax_band band);
	u32 (*cfg_addr_cam_dis)(struct mac_ax_adapter *adapter,
				struct mac_ax_addrcam_dis_ctrl_t *ctl_opt,
				struct mac_ax_addrcam_dis_ctrl_t *ctl_msk,
				enum mac_ax_band band);
	u32 (*sr_update)(struct mac_ax_adapter *adapter,
			 struct mac_ax_sr_info *sr_info,
			 enum mac_ax_band band);
	u32 (*two_nav_cfg)(struct mac_ax_adapter *adapter,
			   struct mac_ax_2nav_info *info);
	u32 (*pkt_drop)(struct mac_ax_adapter *adapter,
			struct mac_ax_pkt_drop_info *info);
	u32 (*send_bcn_h2c)(struct mac_ax_adapter *adapter,
			    struct mac_ax_bcn_info *info);
	u32 (*tx_mode_sel)(struct mac_ax_adapter *adapter,
			   struct mac_ax_mac_tx_mode_sel *mode_sel);
	u32 (*tcpip_chksum_ofd)(struct mac_ax_adapter *adapter,
				u8 en_tx_chksum_ofd,
				u8 en_rx_chksum_ofd);
	u32 (*chk_rx_tcpip_chksum_ofd)(struct mac_ax_adapter *adapter,
				       u8 chksum_status);
	u32 (*chk_allq_empty)(struct mac_ax_adapter *adapter, u8 *empty);
	u32 (*is_txq_empty)(struct mac_ax_adapter *adapter,
			    struct mac_ax_tx_queue_empty *val);
	u32 (*is_rxq_empty)(struct mac_ax_adapter *adapter,
			    struct mac_ax_rx_queue_empty *val);
	u32 (*parse_bcn_stats_c2h)(struct mac_ax_adapter *adapter,
				   u8 *content,
				   struct mac_ax_bcn_cnt *val);
	u32 (*tx_idle_poll)(struct mac_ax_adapter *adapter,
			    struct mac_ax_tx_idle_poll_cfg *poll_cfg);
	u32 (*sifs_chk_cca_en)(struct mac_ax_adapter *adapter,
			       u8 band, u8 en);
	u32 (*patch_rx_rate)(struct mac_ax_adapter *adapter,
			     struct rtw_r_meta_data *info);
	u32 (*get_wp_offset)(struct mac_ax_adapter *adapter,
			     struct mac_txd_ofld_wp_offset *ofld_conf, u16 *val);
	/*frame exchange related*/
	u32 (*upd_mudecision_para)(struct mac_ax_adapter *adapter,
				   struct mac_ax_mudecision_para *info);
	u32 (*mu_sta_upd)(struct mac_ax_adapter *adapter,
			  struct mac_ax_mu_sta_upd *info);
	u32 (*upd_ul_fixinfo)(struct mac_ax_adapter *adapter,
			      struct rtw_phl_ax_ul_fixinfo *info);
	u32 (*f2p_test_cmd)(struct mac_ax_adapter *adapter,
			    struct mac_ax_f2p_test_para *info,
			    struct mac_ax_f2p_wd *f2pwd,
			    struct mac_ax_f2p_tx_cmd *ptxcmd,
			    u8 *psigb_addr);
	u32 (*f2p_test_cmd_v1)(struct mac_ax_adapter *adapter,
			       struct mac_ax_f2p_test_para_v1 *info,
			       struct mac_ax_f2p_wd *f2pwd,
			       struct mac_ax_f2p_tx_cmd_v1 *ptxcmd,
			       u8 *psigb_addr);
	u32 (*snd_test_cmd)(struct mac_ax_adapter *adapter,
			    u8 *cmd_buf);
	u32 (*set_fw_fixmode)(struct mac_ax_adapter *adapter,
			      struct mac_ax_fixmode_para *info);
	u32 (*mac_dumpwlanc)(struct mac_ax_adapter *adapter,
			     struct mac_ax_dumpwlanc *para);
	u32 (*mac_dumpwlans)(struct mac_ax_adapter *adapter,
			     struct mac_ax_dumpwlans *para);
	u32 (*mac_dumpwland)(struct mac_ax_adapter *adapter,
			     struct mac_ax_dumpwland *para);
	void (*mac_ss_dl_rpt_cfg)(struct mac_ax_adapter *adapter,
				  struct mac_ax_ss_dl_rpt_info *info,
				  enum mac_ax_ss_rpt_cfg cfg);
	/*outsrcing related */
	u32 (*outsrc_h2c_common)(struct mac_ax_adapter *adapter,
				 struct rtw_g6_h2c_hdr *hdr,
				 u32 *pvalue);
	u32 (*read_pwr_reg)(struct mac_ax_adapter *adapter, u8 band,
			    const u32 offset, u32 *val);
	u32 (*write_pwr_reg)(struct mac_ax_adapter *adapter, u8 band,
			     const u32 offset, u32 val);
	u32 (*write_msk_pwr_reg)(struct mac_ax_adapter *adapter, u8 band,
				 const u32 offset, u32 mask, u32 val);
	u32 (*write_pwr_ofst_mode)(struct mac_ax_adapter *adapter,
				   u8 band, struct rtw_tpu_info *tpu);
	u32 (*write_pwr_ofst_bw)(struct mac_ax_adapter *adapter,
				 u8 band, struct rtw_tpu_info *tpu);
	u32 (*write_pwr_ref_reg)(struct mac_ax_adapter *adapter,
				 u8 band, struct rtw_tpu_info *tpu);
	u32 (*write_pwr_limit_en)(struct mac_ax_adapter *adapter,
				  u8 band, struct rtw_tpu_info *tpu);
	u32 (*write_pwr_limit_rua_reg)(struct mac_ax_adapter *adapter,
				       u8 band, struct rtw_tpu_info *tpu);
	u32 (*write_pwr_limit_reg)(struct mac_ax_adapter *adapter,
				   u8 band, struct rtw_tpu_pwr_imt_info *tpu);
	u32 (*write_pwr_by_rate_reg)(struct mac_ax_adapter *adapter,
				     u8 band,
				     struct rtw_tpu_pwr_by_rate_info *tpu);
	u32 (*lamode_cfg)(struct mac_ax_adapter *adapter,
			  struct mac_ax_la_cfg *cfg);
	u32 (*lamode_trigger)(struct mac_ax_adapter *adapter, u8 tgr);
	u32 (*lamode_buf_cfg)(struct mac_ax_adapter *adapter,
			      struct mac_ax_la_buf_param *param);
	struct mac_ax_la_status (*get_lamode_st)
				 (struct mac_ax_adapter *adapter);
	u32 (*read_xcap_reg)(struct mac_ax_adapter *adapter, u8 sc_xo,
			     u32 *val);
	u32 (*write_xcap_reg)(struct mac_ax_adapter *adapter, u8 sc_xo,
			      u32 val);
	u32 (*write_bbrst_reg)(struct mac_ax_adapter *adapter, u8 val);
	u32 (*tx_path_map_cfg)(struct mac_ax_adapter *adapter,
			       struct hal_txmap_cfg *cfg);
	/*sounding related*/
	u32 (*get_csi_buffer_index)(struct mac_ax_adapter *adapter, u8 band,
				    u8 csi_buffer_id);
	u32 (*set_csi_buffer_index)(struct mac_ax_adapter *adapter, u8 band,
				    u8 macid, u16 csi_buffer_id,
				    u16 buffer_idx);
	u32 (*get_snd_sts_index)(struct mac_ax_adapter *adapter, u8 band,
				 u8 index);
	u32 (*set_snd_sts_index)(struct mac_ax_adapter *adapter, u8 band,
				 u8 macid, u8 index);
	u32 (*init_snd_mer)(struct mac_ax_adapter *adapter, u8 band);
	u32 (*init_snd_mee)(struct mac_ax_adapter *adapter, u8 band);
	u32 (*csi_force_rate)(struct mac_ax_adapter *adapter, u8 band,
			      u8 ht_rate, u8 vht_rate, u8 he_rate);
	u32 (*csi_rrsc)(struct mac_ax_adapter *adapter, u8 band, u32 rrsc);
	u32 (*set_snd_para)(struct mac_ax_adapter *adapter,
			    struct mac_ax_fwcmd_snd *snd_info);
	u32 (*set_csi_para_reg)(struct mac_ax_adapter *adapter,
				struct mac_reg_csi_para *csi_para);
	u32 (*set_csi_para_cctl)(struct mac_ax_adapter *adapter,
				 struct mac_cctl_csi_para *csi_para);
	u32 (*hw_snd_pause_release)(struct mac_ax_adapter *adapter,
				    u8 band, u8 pr);
	u32 (*bypass_snd_sts)(struct mac_ax_adapter *adapter);
	u32 (*deinit_mee)(struct mac_ax_adapter *adapter, u8 band);
	u32 (*snd_sup)(struct mac_ax_adapter *adapter,
		       struct mac_bf_sup *bf_sup);
	u32 (*gidpos)(struct mac_ax_adapter *adapter,
		      struct mac_gid_pos *mu_gid);
	/*lps related*/
	u32 (*cfg_lps)(struct mac_ax_adapter *adapter,
		       u8 macid,
		       enum mac_ax_ps_mode ps_mode,
		       struct mac_ax_lps_info *lps_info);
	u32 (*ps_pwr_state)(struct mac_ax_adapter *adapter,
			    enum mac_ax_pwr_state_action action,
			    enum mac_ax_rpwm_req_pwr_state req_pwr_state);
	u32 (*chk_leave_lps)(struct mac_ax_adapter *adapter, u8 macid);
	u32 (*cfg_ips)(struct mac_ax_adapter *adapter,
		       u8 macid,
		       u8 enable);
	u32 (*chk_leave_ips)(struct mac_ax_adapter *adapter, u8 macid);
	u32 (*ps_notify_wake)(struct mac_ax_adapter *adapter);
	u32 (*cfg_ps_advance_parm)(struct mac_ax_adapter *adapter,
				   struct mac_ax_ps_adv_parm *parm);
	u32 (*periodic_wake_cfg)(struct mac_ax_adapter *adapter,
				 struct mac_ax_periodic_wake_info pw_info);
	u32 (*req_pwr_lvl_cfg)(struct mac_ax_adapter *adapter,
			       struct mac_ax_req_pwr_lvl_info *pwr_lvl_info);
	u32 (*lps_option_cfg)(struct mac_ax_adapter *adapter,
			      struct mac_lps_option *lps_opt);
	/*Wowlan related*/
	u32 (*cfg_wow_wake)(struct mac_ax_adapter *adapter,
			    u8 macid,
			    struct mac_ax_wow_wake_info *info,
			    struct mac_ax_remotectrl_info_parm_ *content);
	u32 (*cfg_disconnect_det)(struct mac_ax_adapter *adapter,
				  u8 macid,
				  struct mac_ax_disconnect_det_info *info);
	u32 (*cfg_keepalive)(struct mac_ax_adapter *adapter,
			     u8 macid,
			     struct mac_ax_keep_alive_info *info);
	u32 (*cfg_gtk_ofld)(struct mac_ax_adapter *adapter,
			    u8 macid,
			    struct mac_ax_gtk_ofld_info *info,
			    struct mac_ax_gtk_info_parm_ *content);
	u32 (*cfg_arp_ofld)(struct mac_ax_adapter *adapter,
			    u8 macid,
			    struct mac_ax_arp_ofld_info *info,
			    void *parp_info_content);
	u32 (*cfg_ndp_ofld)(struct mac_ax_adapter *adapter,
			    u8 macid,
			    struct mac_ax_ndp_ofld_info *info,
			    struct mac_ax_ndp_info_parm_ *content);
	u32 (*cfg_realwow)(struct mac_ax_adapter *adapter,
			   u8 macid,
			   struct mac_ax_realwow_info *info,
			   struct mac_ax_realwowv2_info_parm_ *content);
	u32 (*cfg_nlo)(struct mac_ax_adapter *adapter,
		       u8 macid,
		       struct mac_ax_nlo_info *info,
		       struct mac_ax_nlo_networklist_parm_ *content);
	u32 (*cfg_dev2hst_gpio)(struct mac_ax_adapter *adapter,
				struct mac_ax_dev2hst_gpio_info *parm);
	u32 (*cfg_uphy_ctrl)(struct mac_ax_adapter *adapter,
			     struct mac_ax_uphy_ctrl_info *info);
	u32 (*cfg_wowcam_upd)(struct mac_ax_adapter *adapter,
			      struct mac_ax_wowcam_upd_info *info);
	u32 (*get_wow_wake_rsn)(struct mac_ax_adapter *adapter, u8 *wake_rsn,
				u8 *reset);
	u32 (*cfg_wow_sleep)(struct mac_ax_adapter *adapter,
			     u8 sleep);
	u32 (*get_wow_fw_status)(struct mac_ax_adapter *adapter,
				 u8 *status, u8 func_en);
	u32 (*request_aoac_report)(struct mac_ax_adapter *adapter,
				   u8 rx_ready);
	u32 (*read_aoac_report)(struct mac_ax_adapter *adapter,
				struct mac_ax_aoac_report *rpt_buf, u8 rx_ready);
	u32 (*check_aoac_report_done)(struct mac_ax_adapter *adapter);
	u32 (*wow_stop_trx)(struct mac_ax_adapter *adapter);
	u32 (*cfg_wow_auto_test)(struct mac_ax_adapter *adapter, u8 rxtest);
	/*system related*/
	u32 (*dbcc_enable)(struct mac_ax_adapter *adapter,
			   struct mac_ax_trx_info *info, u8 dbcc_en);
	u32 (*dbcc_pre_cfg)(struct mac_ax_adapter *adapter, struct mac_dbcc_cfg_info *info);
	u32 (*dbcc_cfg)(struct mac_ax_adapter *adapter, struct mac_dbcc_cfg_info *info);
	u32 (*dbcc_trx_ctrl)(struct mac_ax_adapter *adapter,
			     enum mac_ax_band band, u8 pause);
	u32 (*port_cfg)(struct mac_ax_adapter *adapter,
			enum mac_ax_port_cfg_type type,
			struct mac_ax_port_cfg_para *para);
	u32 (*port_init)(struct mac_ax_adapter *adapter,
			 struct mac_ax_port_init_para *para);
	u32 (*enable_imr)(struct mac_ax_adapter *adapter, u8 band,
			  enum mac_ax_hwmod_sel sel);
	u32 (*dump_efuse_map_wl)(struct mac_ax_adapter *adapter,
				 enum mac_ax_efuse_read_cfg cfg,
				 u8 *efuse_map);
	u32 (*dump_efuse_map_bt)(struct mac_ax_adapter *adapter,
				 enum mac_ax_efuse_read_cfg cfg,
				 u8 *efuse_map);
	u32 (*write_efuse)(struct mac_ax_adapter *adapter, u32 addr, u8 val,
			   enum mac_ax_efuse_bank bank);
	u32 (*read_efuse)(struct mac_ax_adapter *adapter, u32 addr, u32 size,
			  u8 *val, enum mac_ax_efuse_bank bank);
	u32 (*read_hidden_efuse)(struct mac_ax_adapter *adapter, u32 addr,
				 u32 size, u8 *val,
				 enum mac_ax_efuse_hidden_cfg hidden_cfg);
	u32 (*get_efuse_avl_size)(struct mac_ax_adapter *adapter, u32 *size);
	u32 (*get_efuse_avl_size_bt)(struct mac_ax_adapter *adapter, u32 *size);
	u32 (*dump_log_efuse)(struct mac_ax_adapter *adapter,
			      enum mac_ax_efuse_parser_cfg parser_cfg,
			      enum mac_ax_efuse_read_cfg cfg,
			      u8 *efuse_map, bool is_limit);
	u32 (*read_log_efuse)(struct mac_ax_adapter *adapter, u32 addr,
			      u32 size, u8 *val);
	u32 (*write_log_efuse)(struct mac_ax_adapter *adapter, u32 addr,
			       u8 val);
	u32 (*dump_log_efuse_bt)(struct mac_ax_adapter *adapter,
				 enum mac_ax_efuse_parser_cfg parser_cfg,
				 enum mac_ax_efuse_read_cfg cfg,
				 u8 *efuse_map);
	u32 (*read_log_efuse_bt)(struct mac_ax_adapter *adapter, u32 addr,
				 u32 size, u8 *val);
	u32 (*write_log_efuse_bt)(struct mac_ax_adapter *adapter, u32 addr,
				  u8 val);
	u32 (*pg_efuse_by_map)(struct mac_ax_adapter *adapter,
			       struct mac_ax_pg_efuse_info *info,
			       enum mac_ax_efuse_read_cfg cfg,
			       bool part, bool is_limit);
	u32 (*pg_efuse_by_map_bt)(struct mac_ax_adapter *adapter,
				  struct mac_ax_pg_efuse_info *info,
				  enum mac_ax_efuse_read_cfg cfg);
	u32 (*mask_log_efuse)(struct mac_ax_adapter *adapter,
			      struct mac_ax_pg_efuse_info *info);
	u32 (*pg_sec_data_by_map)(struct mac_ax_adapter *adapter,
				  struct mac_ax_pg_efuse_info *info);
	u32 (*cmp_sec_data_by_map)(struct mac_ax_adapter *adapter,
				   struct mac_ax_pg_efuse_info *info);
	u32 (*get_efuse_info)(struct mac_ax_adapter *adapter, u8 *efuse_map,
			      enum rtw_efuse_info id, void *value,
			      u32 length, u8 *autoload_status);
	u32 (*set_efuse_info)(struct mac_ax_adapter *adapter, u8 *efuse_map,
			      enum rtw_efuse_info id, void *value, u32 length);
	u32 (*read_hidden_rpt)(struct mac_ax_adapter *adapter,
			       struct mac_defeature_value *rpt);
	u32 (*check_efuse_autoload)(struct mac_ax_adapter *adapter,
				    u8 *autoload_status);
	u32 (*pg_simulator)(struct mac_ax_adapter *adapter,
			    struct mac_ax_pg_efuse_info *info, u8 *phy_map);
	u32 (*checksum_update)(struct mac_ax_adapter *adapter);
	u32 (*checksum_rpt)(struct mac_ax_adapter *adapter, u16 *chksum);
	u32 (*disable_rf_ofld_by_info)(struct mac_ax_adapter *adapter,
				       struct mac_disable_rf_ofld_info info);
	u32 (*set_efuse_ctrl)(struct mac_ax_adapter *adapter, bool is_secure);
	u32 (*otp_test)(struct mac_ax_adapter *adapter, bool is_OTP_test);
	u32 (*get_mac_ft_status)(struct mac_ax_adapter *adapter,
				 enum mac_ax_feature mac_ft,
				 enum mac_ax_status *stat, u8 *buf,
				 const u32 size, u32 *ret_size);
	u32 (*fw_log_cfg)(struct mac_ax_adapter *adapter,
			  struct mac_ax_fw_log *log_cfg);
	u32 (*pinmux_set_func)(struct mac_ax_adapter *adapter,
			       enum mac_ax_gpio_func func);
	u32 (*pinmux_free_func)(struct mac_ax_adapter *adapter,
				enum mac_ax_gpio_func func);
	u32 (*sel_uart_tx_pin)(struct mac_ax_adapter *adapter,
			       enum mac_ax_uart_tx_pin uart_pin);
	u32 (*sel_uart_rx_pin)(struct mac_ax_adapter *adapter,
			       enum mac_ax_uart_rx_pin uart_pin);
	u32 (*gpio_init)(struct mac_ax_adapter *adapter);
	u32 (*set_gpio_func)(struct mac_ax_adapter *adapter,
			     enum rtw_mac_gfunc func, s8 gpio);
	u32 (*get_gpio_val)(struct mac_ax_adapter *adapter, u8 gpio, u8 *val);
	u32 (*get_uart_fw_dbg_gpio)(struct mac_ax_adapter *adapter, u8 *uart_tx_pin,
				    u8 *uart_rx_pin);
	struct mac_ax_hw_info* (*get_hw_info)(struct mac_ax_adapter *adapter);
	u32 (*set_hw_value)(struct mac_ax_adapter *adapter,
			    enum mac_ax_hw_id hw_id, void *value);
	u32 (*get_hw_value)(struct mac_ax_adapter *adapter,
			    enum mac_ax_hw_id hw_id, void *value);
	u32 (*get_err_status)(struct mac_ax_adapter *adapter,
			      enum mac_ax_err_info *err);
	u32 (*set_err_status)(struct mac_ax_adapter *adapter,
			      enum mac_ax_err_info err);
	u32 (*general_pkt_ids)(struct mac_ax_adapter *adapter,
			       struct mac_ax_general_pkt_ids *ids);
	u32 (*coex_init)(struct mac_ax_adapter *adapter,
			 struct mac_ax_coex *coex);
	u32 (*coex_read)(struct mac_ax_adapter *adapter,
			 const u32 offset, u32 *val);
	u32 (*coex_write)(struct mac_ax_adapter *adapter,
			  const u32 offset, const u32 val);
	u32 (*trigger_cmac_err)(struct mac_ax_adapter *adapter);
	u32 (*trigger_cmac1_err)(struct mac_ax_adapter *adapter);
	u32 (*trigger_dmac_err)(struct mac_ax_adapter *adapter);
	u32 (*tsf_sync)(struct mac_ax_adapter *adapter, u8 from_port,
			u8 to_port, s32 sync_offset,
			enum mac_ax_tsf_sync_act action);
	u32 (*read_xtal_si)(struct mac_ax_adapter *adapter, u8 offset, u8 *val);
	u32 (*write_xtal_si)(struct mac_ax_adapter *adapter, u8 offset, u8 val,
			     u8 bitmask);
	u32 (*io_chk_access)(struct mac_ax_adapter *adapter, u32 offset);
	u32 (*ser_ctrl)(struct mac_ax_adapter *adapter, enum mac_ax_func_sw sw);
	u32 (*chk_err_status)(struct mac_ax_adapter *adapter, u8 *ser_status);
	u32 (*get_freerun)(struct mac_ax_adapter *adapter, struct mac_ax_freerun *freerun);
	/* mcc */
	u32 (*reset_mcc_group)(struct mac_ax_adapter *adapter, u8 group);
	u32 (*reset_mcc_request)(struct mac_ax_adapter *adapter, u8 group);
	u32 (*add_mcc)(struct mac_ax_adapter *adapter,
		       struct mac_ax_mcc_role *info);
	u32 (*start_mcc)(struct mac_ax_adapter *adapter,
			 struct mac_ax_mcc_start *info);
	u32 (*stop_mcc)(struct mac_ax_adapter *adapter, u8 group, u8 macid,
			u8 prev_groups);
	u32 (*del_mcc_group)(struct mac_ax_adapter *adapter, u8 group,
			     u8 prev_groups);
	u32 (*mcc_request_tsf)(struct mac_ax_adapter *adapter, u8 group,
			       u8 macid_x, u8 macid_y);
	u32 (*mcc_macid_bitmap)(struct mac_ax_adapter *adapter, u8 group,
				u8 macid, u8 *bitmap, u8 len);
	u32 (*mcc_sync_enable)(struct mac_ax_adapter *adapter, u8 group,
			       u8 source, u8 target, u8 offset);
	u32 (*mcc_set_duration)(struct mac_ax_adapter *adapter,
				struct mac_ax_mcc_duration_info *info);
	u32 (*get_mcc_tsf_rpt)(struct mac_ax_adapter *adapter, u8 group,
			       u32 *tsf_x_high, u32 *tsf_x_low,
			       u32 *tsf_y_high, u32 *tsf_y_low);
	u32 (*get_mcc_status_rpt)(struct mac_ax_adapter *adapter, u8 group,
				  u8 *status, u32 *tsf_high, u32 *tsf_low);
	u32 (*get_mcc_group)(struct mac_ax_adapter *adapter, u8 *pget_group);
	u32 (*check_add_mcc_done)(struct mac_ax_adapter *adapter, u8 group);
	u32 (*check_start_mcc_done)(struct mac_ax_adapter *adapter, u8 group);
	u32 (*check_stop_mcc_done)(struct mac_ax_adapter *adapter, u8 group);
	u32 (*check_del_mcc_group_done)(struct mac_ax_adapter *adapter,
					u8 group);
	u32 (*check_mcc_request_tsf_done)(struct mac_ax_adapter *adapter,
					  u8 group);
	u32 (*check_mcc_macid_bitmap_done)(struct mac_ax_adapter *adapter,
					   u8 group);
	u32 (*check_mcc_sync_enable_done)(struct mac_ax_adapter *adapter,
					  u8 group);
	u32 (*check_mcc_set_duration_done)(struct mac_ax_adapter *adapter,
					   u8 group);
	/* not mcc */
	u32 (*check_access)(struct mac_ax_adapter *adapter, u32 offset);
	u32 (*set_led_mode)(struct mac_ax_adapter *adapter,
			    enum mac_ax_led_mode mode, u8 led_id);
	u32 (*led_ctrl)(struct mac_ax_adapter *adapter, u8 high, u8 led_id);
	u32 (*set_sw_gpio_mode)(struct mac_ax_adapter *adapter,
				enum rtw_gpio_mode mode, u8 gpio);
	u32 (*sw_gpio_ctrl)(struct mac_ax_adapter *adapter, u8 high, u8 gpio);
	u32 (*get_c2h_event)(struct mac_ax_adapter *adapter,
			     struct rtw_c2h_info *c2h,
			     enum phl_msg_evt_id *id,
			     u8 *c2h_info);
	u32 (*cfg_wps)(struct mac_ax_adapter *adapter,
		       struct mac_ax_cfg_wps *wps);
	u32 (*get_wl_dis_val)(struct mac_ax_adapter *adapter, u8 *val);
	u32 (*cfg_per_pkt_phy_rpt)(struct mac_ax_adapter *adapter,
				   struct mac_ax_per_pkt_phy_rpt *rpt);
#if MAC_AX_FEATURE_DBGPKG
	u32 (*fwcmd_lb)(struct mac_ax_adapter *adapter, u32 len, u8 burst);
	u32 (*mem_dump)(struct mac_ax_adapter *adapter, enum mac_ax_mem_sel sel,
			u32 strt_addr, u8 *data, u32 size, u32 dbg_path);
	u32 (*get_mem_size)(struct mac_ax_adapter *adapter,
			    enum mac_ax_mem_sel sel);
	void (*dbg_status_dump)(struct mac_ax_adapter *adapter,
				struct mac_ax_dbgpkg *val,
				struct mac_ax_dbgpkg_en *en);
	u32 (*reg_dump)(struct mac_ax_adapter *adapter,
			enum mac_ax_reg_sel sel);
	u32 (*rx_cnt)(struct mac_ax_adapter *adapter,
		      struct mac_ax_rx_cnt *rxcnt);
	u32 (*dump_fw_rsvd_ple)(struct mac_ax_adapter *adapter, u8 **buf);
	u32 (*fw_dbg_dump)(struct mac_ax_adapter *adapter,
			   u8 **buf,
			   struct mac_ax_fwdbg_en *en);
	u32 (*event_notify)(struct mac_ax_adapter *adapter,
			    enum phl_msg_evt_id id, u8 band);
	u32 (*dbgport_hw_set)(struct mac_ax_adapter *adapter,
			      struct mac_ax_dbgport_hw *dp_hw);
#endif
#if MAC_AX_FEATURE_HV
	u32 (*ram_boot)(struct mac_ax_adapter *adapter, u8 *fw, u32 len);
	/*fw offload related*/
	u32 (*clear_write_request)(struct mac_ax_adapter *adapter);
	u32 (*add_write_request)(struct mac_ax_adapter *adapter,
				 struct mac_ax_write_req *req,
				 u8 *value, u8 *mask);
	u32 (*write_ofld)(struct mac_ax_adapter *adapter);
	u32 (*clear_conf_request)(struct mac_ax_adapter *adapter);
	u32 (*add_conf_request)(struct mac_ax_adapter *adapter,
				struct mac_ax_conf_ofld_req *req);
	u32 (*conf_ofld)(struct mac_ax_adapter *adapter);
	u32 (*clear_read_request)(struct mac_ax_adapter *adapter);
	u32 (*add_read_request)(struct mac_ax_adapter *adapter,
				struct mac_ax_read_req *req);
	u32 (*read_ofld)(struct mac_ax_adapter *adapter);
	u32 (*read_ofld_value)(struct mac_ax_adapter *adapter,
			       u8 **val_buf, u16 *val_len);
#endif
	u32 (*add_cmd_ofld)(struct mac_ax_adapter *adapter,
			    struct rtw_mac_cmd *cmd);
	u32 (*add_cmd_ofld_v1)(struct mac_ax_adapter *adapter,
			       struct rtw_mac_cmd_v1 *cmd);
	u32 (*cmd_ofld)(struct mac_ax_adapter *adapter);
	/* flash related*/
	u32 (*flash_erase)(struct mac_ax_adapter *adapter,
			   u32 addr,
			   u32 length,
			   u32 timeout);
	u32 (*flash_read)(struct mac_ax_adapter *adapter,
			  u32 addr,
			  u32 length,
			  u8 *buffer,
			  u32 timeout);
	u32 (*flash_write)(struct mac_ax_adapter *adapter,
			   u32 addr,
			   u32 length,
			   u8 *buffer,
			   u32 timeout);
	u32 (*fw_status_cmd)(struct mac_ax_adapter *adapter,
			     struct mac_ax_fwstatus_payload *info);
	u32 (*tx_duty)(struct mac_ax_adapter *adapter,
		       u16 pause_intvl, u16 tx_intvl);
	u32 (*tx_duty_stop)(struct mac_ax_adapter *adapter);
	u32 (*fwc2h_ofdma_sts_parse)(struct mac_ax_adapter *adapter,
				     struct mac_ax_fwc2h_sts *fw_c2h_sts,
				     u32 *content);
	u32 (*fw_ofdma_sts_en)(struct mac_ax_adapter *adapter,
			       struct mac_ax_fwsts_para *fwsts_para);
	u32 (*get_phy_rpt_cfg)(struct mac_ax_adapter *adapter,
			       struct mac_ax_phy_rpt_cfg *cfg);
#if MAC_AX_FEATURE_DBGCMD
	s32 (*halmac_cmd)(struct mac_ax_adapter *adapter, char *input, char *output, u32 out_len);
	void (*halmac_cmd_parser)(struct mac_ax_adapter *adapter,
				  char input[][MAC_MAX_ARGV], u32 input_num, char *output,
				  u32 out_len);
#endif
	/* FAST_CH_SW */
	u32 (*fast_ch_sw)(struct mac_ax_adapter *adapter,
			  struct mac_ax_fast_ch_sw_param *fast_ch_sw_param);
	u32 (*fast_ch_sw_done)(struct mac_ax_adapter *adapter);
	u32 (*get_fast_ch_sw_rpt)(struct mac_ax_adapter *adapter, u32 *fast_ch_sw_status_code);
	void (*h2c_agg_en)(struct mac_ax_adapter *adapter, u8 enable);
	void (*h2c_agg_flush)(struct mac_ax_adapter *adapter);
	u32 (*h2c_agg_tx)(struct mac_ax_adapter *adapter);
	u32 (*fw_dbg_dle_cfg)(struct mac_ax_adapter *adapter, bool lock);
	u32 (*add_scanofld_ch)(struct mac_ax_adapter *adapter,
			       struct mac_ax_scanofld_chinfo *chinfo,
			       u8 send_h2c, u8 clear_after_send, u8 band);
	u32 (*scanofld)(struct mac_ax_adapter *adapter, struct mac_ax_scanofld_param *scanParam);
	u32 (*scanofld_fw_busy)(struct mac_ax_adapter *adapter, u8 band);
	u32 (*scanofld_chlist_busy)(struct mac_ax_adapter *adapter, u8 band);
	u32 (*scanofld_hst_ctrl)(struct mac_ax_adapter *adapter, u8 pri_ch, u8 ch_band,
				 enum mac_ax_scanofld_ctrl op, u8 band);
#if MAC_AX_FEATURE_DBGDEC
	u32 (*fw_log_set_array)(struct mac_ax_adapter *adapter, void *symbol_ptr, u32 file_size);
	u32 (*fw_log_unset_array)(struct mac_ax_adapter *adapter);
#endif
	u32 (*get_fw_status)(struct mac_ax_adapter *adapter);
	u32 (*role_sync)(struct mac_ax_adapter *adapter, struct mac_ax_role_info *info);
	u32 (*ch_switch_ofld)(struct mac_ax_adapter *adapter, struct mac_ax_ch_switch_parm parm);
	u32 (*get_ch_switch_rpt)(struct mac_ax_adapter *adapter, struct mac_ax_ch_switch_rpt *rpt);
	u32 (*cfg_bcn_filter)(struct mac_ax_adapter *adapter, struct mac_ax_bcn_fltr cfg);
	u32 (*bcn_filter_rssi)(struct mac_ax_adapter *adapter, u8 macid, u8 size, u8 *rssi);
	u32 (*bcn_filter_tp)(struct mac_ax_adapter *adapter, u8 macid, u16 tx, u16 rx);
	u32 (*cfg_bcn_early_rpt)(struct mac_ax_adapter *adapter, u8 band, u8 port, u8 en);
	/*Proxy related*/
	u32 (*proxyofld)(struct mac_ax_adapter *adapter, struct rtw_hal_mac_proxyofld *pcfg);
	u32 (*proxy_mdns_serv_pktofld)(struct mac_ax_adapter *adapter,
				       struct rtw_hal_mac_proxy_mdns_service *pserv, u8 *pktid);
	u32 (*proxy_mdns_txt_pktofld)(struct mac_ax_adapter *adapter,
				      struct rtw_hal_mac_proxy_mdns_txt *ptxt, u8 *pktid);
	u32 (*proxy_mdns)(struct mac_ax_adapter *adapter, struct rtw_hal_mac_proxy_mdns *pmdns);
	u32 (*proxy_ptcl_pattern)(struct mac_ax_adapter *adapter,
				  struct rtw_hal_mac_proxy_ptcl_pattern *cfg);
	u32 (*check_proxy_done)(struct mac_ax_adapter *adapter, u8 *fw_ret);
	/*Fw Cap related*/
	u32 (*get_wlanfw_cap)(struct mac_ax_adapter *adapter, struct rtw_wcpu_cap_t *wcpu_cap);
	/* NAN related */
	u32(*nan_act_schedule_req)(struct mac_ax_adapter *adapter,
				   struct mac_ax_nan_sched_info *info);
	u32(*nan_bcn_req)(struct mac_ax_adapter *adapter, struct mac_ax_nan_bcn *info);
	u32(*nan_func_ctrl)(struct mac_ax_adapter *adapter, struct mac_ax_nan_func_info *info);
	u32(*nan_pause_faw_tx)(struct mac_ax_adapter *adapter, u32 id_map);
	u32(*nan_de_info)(struct mac_ax_adapter *adapter, u8 status, u8 loc_bcast_sdf);
	u32(*nan_join_cluster)(struct mac_ax_adapter *adapter, u8 is_allow);
	u32(*get_act_schedule_id)(struct mac_ax_adapter *adapter,
				  struct mac_ax_act_ack_info *act_ack_info);
	u32(*nan_get_cluster_info)(struct mac_ax_adapter *adapter,
				   struct mac_ax_nan_info *cluster_info);
	u32(*check_cluster_info)(struct mac_ax_adapter *adapter,
				 struct mac_ax_nan_info *cluster_info);
	/* MP security related */
	u32 (*mp_chk_sec_rec)(struct mac_ax_adapter *adapter, u8 *sec_mode);
	u32 (*mp_pg_sec_phy_wifi)(struct mac_ax_adapter *adapter);
	u32 (*mp_cmp_sec_phy_wifi)(struct mac_ax_adapter *adapter);
	u32 (*mp_pg_sec_hid_wifi)(struct mac_ax_adapter *adapter);
	u32 (*mp_cmp_sec_hid_wifi)(struct mac_ax_adapter *adapter);
	u32 (*mp_pg_sec_dis)(struct mac_ax_adapter *adapter);
	u32 (*mp_cmp_sec_dis)(struct mac_ax_adapter *adapter);
	u32 (*mp_sic_dis)(struct mac_ax_adapter *adapter);
	u32 (*mp_chk_sic_dis)(struct mac_ax_adapter *adapter, u8 *sic_mode);
	u32 (*mp_jtag_dis)(struct mac_ax_adapter *adapter);
	u32 (*mp_chk_jtag_dis)(struct mac_ax_adapter *adapter, u8 *jtag_mode);
	u32 (*mp_uart_tx_dis)(struct mac_ax_adapter *adapter);
	u32 (*mp_chk_uart_tx_dis)(struct mac_ax_adapter *adapter, u8 *uart_tx_mode);
	u32 (*mp_uart_rx_dis)(struct mac_ax_adapter *adapter);
	u32 (*mp_chk_uart_rx_dis)(struct mac_ax_adapter *adapter, u8 *uart_rx_mode);
};

#endif
