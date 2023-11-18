/******************************************************************************
 *
 * Copyright(c) 2019 - 2020 Realtek Corporation.
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
 *****************************************************************************/
#ifndef _HAL_HEADERS_LE_H_
#define _HAL_HEADERS_LE_H_

/* Common definition from PHL */
#include "../rtw_general_def.h"
#include "../phl_list.h"
#include "../phl_status.h"
#include "../pltfm_ops.h"
#include "../phl_config.h"
#include "../phl_types.h"
#include "../phl_regulation_def.h"
#include "../phl_chnlplan.h"
#include "../phl_country.h"
#include "../phl_txpwr.h"
/* Exported structure/definition from PHL */
#include "../phl_util.h"
#include "../phl_def.h"
#include "../phl_trx_def.h"
#include "../phl_wow_def.h"
#include "../phl_btc_def.h"
#include "../phl_test_def.h"
#include "../phl_debug.h"
#include "../phl_ext_tx_pwr_lmt_def.h"
#ifdef CONFIG_PHL_CHANNEL_INFO
#include "../phl_chan_info_def.h"
#endif /* CONFIG_PHL_CHANNEL_INFO */

#ifdef CONFIG_PCI_HCI
#include "../hci/phl_trx_def_pcie.h"
#endif
#ifdef CONFIG_USB_HCI
#include "../hci/phl_trx_def_usb.h"
#endif
#ifdef CONFIG_SDIO_HCI
#include "../hci/phl_trx_def_sdio.h"
#endif

/* Common definition from HAL*/
#include "hal_general_def.h"
/* Exported structure/definition from HAL */
#include "hal_config.h"
#include "hal_def.h"
#include "phy/bb/halbb_outsrc_def.h"
#include "mac/mac_exp_def.h"
#include "mac/mac_outsrc_def.h"

/*
Exported hal API  from HAL
Prevent HAL mac/bb/ rf/ btc cross referencing
*/
#include "hal_api_drv.h"

#include "hal_io.h" /*exported hal_read, hal_write API from HAL */
#include "hal_fw.h"


#if 1
static inline void hal_mdelay(struct rtw_hal_com_t *h, int ms)
{
	_os_delay_ms(halcom_to_drvpriv(h), ms);
}
static inline void hal_udelay(struct rtw_hal_com_t *h, int us)
{
	_os_delay_ms(halcom_to_drvpriv(h), us);
}

static inline void hal_msleep(struct rtw_hal_com_t *h, int ms)
{
	_os_sleep_ms(halcom_to_drvpriv(h),ms);
}

static inline void hal_usleep(struct rtw_hal_com_t *h, int us)
{
	_os_sleep_us(halcom_to_drvpriv(h), us);
}

#ifdef DBG_HAL_MEM_MOINTOR
static inline void *hal_mem_alloc(struct rtw_hal_com_t *h, u32 buf_sz)
{
	_os_atomic_sub(halcom_to_drvpriv(h), &(h->hal_mem), buf_sz);
	return _os_mem_alloc(halcom_to_drvpriv(h), buf_sz);
}

static inline void hal_mem_free(struct rtw_hal_com_t *h, void *buf, u32 buf_sz)
{
	_os_atomic_add_return(halcom_to_drvpriv(h), &(h->hal_mem), buf_sz);
	_os_mem_free(halcom_to_drvpriv(h), buf, buf_sz);
}
#else
static inline void *hal_mem_alloc(struct rtw_hal_com_t *h, u32 buf_sz)
{
	return _os_mem_alloc(halcom_to_drvpriv(h), buf_sz);
}

static inline void hal_mem_free(struct rtw_hal_com_t *h, void *buf, u32 buf_sz)
{
	_os_mem_free(halcom_to_drvpriv(h), buf, buf_sz);
}
#endif /*DBG_HAL_MEM_MOINTOR*/
static inline void hal_mem_set(struct rtw_hal_com_t *h, void *buf, s8 value, u32 size)
{
	_os_mem_set(halcom_to_drvpriv(h), buf, value, size);
}

static inline void hal_mem_cpy(struct rtw_hal_com_t *h, void *dest,
						const void *src, u32 size)
{
	_os_mem_cpy(halcom_to_drvpriv(h), dest, src, size);
}

static inline int hal_mem_cmp(struct rtw_hal_com_t *h, const void *dest,
						const void *src, u32 size)
{
	return _os_mem_cmp(halcom_to_drvpriv(h), dest, src, size);
}
static inline void hal_mutex_init(struct rtw_hal_com_t *h, _os_mutex *mutex)
{
	_os_mutex_init(halcom_to_drvpriv(h), mutex);
}
static inline void hal_mutex_deinit(struct rtw_hal_com_t *h, _os_mutex *mutex)
{
	_os_mutex_deinit(halcom_to_drvpriv(h), mutex);
}
static inline void hal_mutex_lock(struct rtw_hal_com_t *h, _os_mutex *mutex)
{
	_os_mutex_lock(halcom_to_drvpriv(h), mutex);
}
static inline void hal_mutex_unlock(struct rtw_hal_com_t *h, _os_mutex *mutex)
{
	_os_mutex_unlock(halcom_to_drvpriv(h), mutex);
}
static inline void hal_init_timer(struct rtw_hal_com_t *h, _os_timer *timer,
			void (*call_back_func)(void* context),
			void *context, const char *sz_id)
{
	_os_init_timer(halcom_to_drvpriv(h), timer, call_back_func, context, sz_id);
}

static inline void hal_set_timer(struct rtw_hal_com_t *h, _os_timer *timer,
								u32 ms_delay)
{
	_os_set_timer(halcom_to_drvpriv(h), timer, ms_delay);
}
static inline void hal_cancel_timer(struct rtw_hal_com_t *h, _os_timer *timer)
{
	_os_cancel_timer(halcom_to_drvpriv(h), timer);
}
static inline void hal_release_timer(struct rtw_hal_com_t *h, _os_timer *timer)
{
	_os_release_timer(halcom_to_drvpriv(h), timer);
}

/*IO OPS*/
#ifdef CONFIG_PCI_HCI
static inline u8 hal_pci_read8(struct rtw_hal_com_t *h,u32 addr)
{
	return _os_read8_pcie(halcom_to_drvpriv(h), addr);
}
static inline u16 hal_pci_read16(struct rtw_hal_com_t *h,u32 addr)
{
	return _os_read16_pcie(halcom_to_drvpriv(h), addr);
}
static inline u32 hal_pci_read32(struct rtw_hal_com_t *h,u32 addr)
{
	return _os_read32_pcie(halcom_to_drvpriv(h), addr);
}

static inline int hal_pci_write8(struct rtw_hal_com_t *h, u32 addr, u8 val)
{
	return _os_write8_pcie(halcom_to_drvpriv(h), addr, val);
}
static inline int hal_pci_write16(struct rtw_hal_com_t *h, u32 addr, u16 val)
{
	return _os_write16_pcie(halcom_to_drvpriv(h), addr, val);
}
static inline int hal_pci_write32(struct rtw_hal_com_t *h, u32 addr, u32 val)
{
	return _os_write32_pcie(halcom_to_drvpriv(h), addr, val);
}

#endif
#ifdef CONFIG_USB_HCI
static inline int hal_usb_vendor_req(struct rtw_hal_com_t *h,
			u8 request, u16 value, u16 index,
			void *pdata, u16 len, u8 requesttype)
{
	return _os_usbctrl_vendorreq(halcom_to_drvpriv(h), request, value,
					index, pdata, len, requesttype);
}
#endif /*CONFIG_USB_HCI*/


#ifdef CONFIG_SDIO_HCI
static inline u8 hal_sdio_cmd52_r8(struct rtw_hal_com_t *h, u32 offset)
{
	return _os_sdio_cmd52_r8(halcom_to_drvpriv(h), offset);
}
static inline u8 hal_sdio_cmd53_r8(struct rtw_hal_com_t *h, u32 offset)
{
	return _os_sdio_cmd53_r8(halcom_to_drvpriv(h), offset);
}

static inline u16 hal_sdio_cmd53_r16(struct rtw_hal_com_t *h, u32 offset)
{
	return _os_sdio_cmd53_r16(halcom_to_drvpriv(h), offset);
}

static inline u32 hal_sdio_cmd53_r32(struct rtw_hal_com_t *h, u32 offset)
{
	return _os_sdio_cmd53_r32(halcom_to_drvpriv(h), offset);
}

static inline u8 hal_sdio_cmd53_rn(struct rtw_hal_com_t *h,
					u32 offset, u32 size, u8 *data)
{
	return _os_sdio_cmd53_rn(halcom_to_drvpriv(h), offset, size, data);
}

static inline u8 hal_sdio_cmd53_r(struct rtw_hal_com_t *h,
					u32 offset, u32 size, u8 *val)
{
	size = _ALIGN(size, 4);
	if (size > h->block_sz)
		size = _ALIGN(size, h->block_sz);
	return _os_sdio_cmd53_r(halcom_to_drvpriv(h), offset, size, val);
}

static inline void hal_sdio_cmd52_w8(struct rtw_hal_com_t *h,
					u32 offset, u8 val)
{
	_os_sdio_cmd52_w8(halcom_to_drvpriv(h), offset, val);
}

static inline void hal_sdio_cmd53_w8(struct rtw_hal_com_t *h,
					u32 offset, u8 val)
{
	_os_sdio_cmd53_w8(halcom_to_drvpriv(h), offset, val);
}

static inline void hal_sdio_cmd53_w16(struct rtw_hal_com_t *h,
					u32 offset, u16 val)
{
	_os_sdio_cmd53_w16(halcom_to_drvpriv(h), offset, val);
}

static inline void hal_sdio_cmd53_w32(struct rtw_hal_com_t *h,
					u32 offset, u32 val)
{
	_os_sdio_cmd53_w32(halcom_to_drvpriv(h), offset, val);
}

static inline void hal_sdio_cmd53_wn(struct rtw_hal_com_t *h,
					u32 offset, u32 size, u8 *val)
{
	_os_sdio_cmd53_wn(halcom_to_drvpriv(h), offset, size, val);
}

static inline void hal_sdio_cmd53_w(struct rtw_hal_com_t *h,
					u32 offset, u32 size, u8 *val)
{
	size = _ALIGN(size, 4);
	if (size > h->block_sz)
		size = _ALIGN(size, h->block_sz);

	_os_sdio_cmd53_w(halcom_to_drvpriv(h), offset, size, val);
}

static inline u8 hal_sdio_f0_read(struct rtw_hal_com_t *h,
					u32 addr, void *buf, u32 len)
{

	return _os_sdio_f0_read(halcom_to_drvpriv(h), addr, buf, len);
}

static inline u8 hal_sdio_read_cia_r8(struct rtw_hal_com_t *h, u32 addr)
{
	return _os_sdio_read_cia_r8(halcom_to_drvpriv(h), addr);
}

#endif /*CONFIG_SDIO_HCI*/

#else
/*please refer to hal_pltfm_ops.h*/
#define hal_mdelay(h, ms)	_os_delay_ms(halcom_to_drvpriv(h), ms)
#define hal_udelay(h, us)	_os_delay_ms(halcom_to_drvpriv(h), us)
#define hal_msleep(h, ms)	_os_sleep_ms(halcom_to_drvpriv(h),ms)
#define hal_usleep(h, us)	_os_sleep_us(halcom_to_drvpriv(h), us)

#define hal_mem_alloc(h, buf_sz) _os_mem_alloc(halcom_to_drvpriv(h), buf_sz)
#define hal_mem_free(h, buf, buf_sz)	_os_mem_free(halcom_to_drvpriv(h), buf, buf_sz)
#define hal_mem_set(h, buf, value, size)	_os_mem_set(halcom_to_drvpriv(h), buf, value, size)
#define hal_mem_cpy(h, dest, src, size)	_os_mem_cpy(halcom_to_drvpriv(h), dest, src, size)
#define hal_mem_cmp(h, dest, src, size)	_os_mem_cmp(halcom_to_drvpriv(h), dest, src, size)

#define hal_mutex_init(h, mutex)	_os_mutex_init(halcom_to_drvpriv(h), mutex)
#define hal_mutex_deinit(h, hmutex)	_os_mutex_deinit(halcom_to_drvpriv(h), mutex)
#define hal_mutex_lock(h, mutex)	_os_mutex_lock(halcom_to_drvpriv(h), mutex)
#define hal_mutex_unlock(h, mutex)	_os_mutex_unlock(halcom_to_drvpriv(h), mutex)

#define hal_init_timer(h, timer, call_back_func, context, sz_id) \
	_os_init_timer(halcom_to_drvpriv(h), timer, call_back_func, context, sz_id)
#define hal_set_timer(h, timer, ms_delay)	_os_set_timer(halcom_to_drvpriv(h), timer, ms_delay)
#define hal_cancel_timer(h, timer)		_os_cancel_timer(halcom_to_drvpriv(h), timer)
#define hal_release_timer(h, timer)		_os_release_timer(halcom_to_drvpriv(h), timer)

/*IO OPS*/
#ifdef CONFIG_PCI_HCI
#define hal_pci_read8(h, addr)		_os_read8_pcie(halcom_to_drvpriv(h), addr)
#define hal_pci_read16(h, addr)		_os_read16_pcie(halcom_to_drvpriv(h), addr)
#define hal_pci_read32(h, addr)		_os_read32_pcie(halcom_to_drvpriv(h), addr)
#define hal_pci_write8(h, addr, val)	_os_write8_pcie(halcom_to_drvpriv(h), addr, val)
#define hal_pci_write16(h, addr, val)	_os_write16_pcie(halcom_to_drvpriv(h), addr, val)
#define hal_pci_write32(h, addr, val)	_os_write32_pcie(halcom_to_drvpriv(h), addr, val)
#endif

#ifdef CONFIG_USB_HCI
#define hal_usb_vendor_req(h, request, value, index, pdata, len, requesttype) \
	_os_usbctrl_vendorreq(halcom_to_drvpriv(h), request, value, index, pdata, len, requesttype)
#endif /*CONFIG_USB_HCI*/

#ifdef CONFIG_SDIO_HCI
#define hal_sdio_cmd52_r8(h, offset)	_os_sdio_cmd52_r8(halcom_to_drvpriv(h), offset)
#define hal_sdio_cmd53_r8(h, offset)	_os_sdio_cmd53_r8(halcom_to_drvpriv(h), offset)
#define hal_sdio_cmd53_r16(h, offset)	_os_sdio_cmd53_r16(halcom_to_drvpriv(h), offset)
#define hal_sdio_cmd53_r32(h, offset)	_os_sdio_cmd53_r32(halcom_to_drvpriv(h), offset)
#define hal_sdio_cmd53_rn(h, offset, size, data) _os_sdio_cmd53_rn(halcom_to_drvpriv(h), offset, size, data)
#define hal_sdio_cmd52_w8(h, offset, val)	_os_sdio_cmd52_w8(halcom_to_drvpriv(h), offset, val)
#define hal_sdio_cmd53_w8(h, offset, val)	_os_sdio_cmd53_w8(halcom_to_drvpriv(h), offset, val)
#define hal_sdio_cmd53_w16(h, offset, val)	_os_sdio_cmd53_w16(halcom_to_drvpriv(h), offset, val)
#define hal_sdio_cmd53_w32(h, offset, val)	_os_sdio_cmd53_w32(halcom_to_drvpriv(h), offset, val)
#define hal_sdio_f0_read(h, addr, buf, len)	_os_sdio_f0_read(halcom_to_drvpriv(h), addr, buf, len)
#define hal_sdio_read_cia_r8(h, addr)		_os_sdio_read_cia_r8(halcom_to_drvpriv(h), addr)
#endif /*CONFIG_SDIO_HCI*/
#endif


#endif /*_HAL_HEADERS_LE_H_*/
