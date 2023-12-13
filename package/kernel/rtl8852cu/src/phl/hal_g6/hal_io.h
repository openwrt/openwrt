/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#ifndef _HAL_IO_H_
#define _HAL_IO_H_

/*
Below is the data structure used by _io_handler
*/

extern u8 _hal_read8(struct rtw_hal_com_t *hal, u32 addr);
extern u16 _hal_read16(struct rtw_hal_com_t *hal, u32 addr);
extern u32 _hal_read32(struct rtw_hal_com_t *hal, u32 addr);
extern void _hal_read_mem(struct rtw_hal_com_t *hal, u32 addr, u32 cnt, u8 *pmem);

extern int _hal_write8(struct rtw_hal_com_t *hal, u32 addr, u8 val);
extern int _hal_write16(struct rtw_hal_com_t *hal, u32 addr, u16 val);
extern int _hal_write32(struct rtw_hal_com_t *hal, u32 addr, u32 val);


#ifdef CONFIG_SDIO_HCI
u8 _hal_sd_f0_read8(struct rtw_hal_com_t *hal, u32 addr);
#ifdef CONFIG_SDIO_INDIRECT_ACCESS
u8 _hal_sd_iread8(struct rtw_hal_com_t *hal, u32 addr);
u16 _hal_sd_iread16(struct rtw_hal_com_t *hal, u32 addr);
u32 _hal_sd_iread32(struct rtw_hal_com_t *hal, u32 addr);
int _hal_sd_iwrite8(struct rtw_hal_com_t *hal, u32 addr, u8 val);
int _hal_sd_iwrite16(struct rtw_hal_com_t *hal, u32 addr, u16 val);
int _hal_sd_iwrite32(struct rtw_hal_com_t *hal, u32 addr, u32 val);
#endif /* CONFIG_SDIO_INDIRECT_ACCESS */
#endif /* CONFIG_SDIO_HCI */


#ifdef DBG_IO
u32 match_read_sniff(struct rtw_hal_com_t *hal, u32 addr, u16 len, u32 val);
u32 match_write_sniff(struct rtw_hal_com_t *hal, u32 addr, u16 len, u32 val);
bool match_rf_read_sniff_ranges(struct rtw_hal_com_t *hal, u8 path, u32 addr, u32 mask);
bool match_rf_write_sniff_ranges(struct rtw_hal_com_t *hal, u8 path, u32 addr, u32 mask);

extern u8 dbg_hal_read8(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line);
extern u16 dbg_hal_read16(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line);
extern u32 dbg_hal_read32(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line);

extern int dbg_hal_write8(struct rtw_hal_com_t *hal, u32 addr, u8 val, const char *caller, const int line);
extern int dbg_hal_write16(struct rtw_hal_com_t *hal, u32 addr, u16 val, const char *caller, const int line);
extern int dbg_hal_write32(struct rtw_hal_com_t *hal, u32 addr, u32 val, const char *caller, const int line);
extern int dbg_hal_writeN(struct rtw_hal_com_t *hal, u32 addr , u32 length , u8 *data, const char *caller, const int line);

#ifdef CONFIG_SDIO_HCI
u8 dbg_hal_sd_f0_read8(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line);
#ifdef CONFIG_SDIO_INDIRECT_ACCESS
u8 dbg_hal_sd_iread8(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line);
u16 dbg_hal_sd_iread16(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line);
u32 dbg_hal_sd_iread32(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line);
int dbg_hal_sd_iwrite8(struct rtw_hal_com_t *hal, u32 addr, u8 val, const char *caller, const int line);
int dbg_hal_sd_iwrite16(struct rtw_hal_com_t *hal, u32 addr, u16 val, const char *caller, const int line);
int dbg_hal_sd_iwrite32(struct rtw_hal_com_t *hal, u32 addr, u32 val, const char *caller, const int line);
#endif /* CONFIG_SDIO_INDIRECT_ACCESS */
#endif /* CONFIG_SDIO_HCI */

#define hal_read8(hal, addr) dbg_hal_read8((hal), (addr), __FUNCTION__, __LINE__)
#define hal_read16(hal, addr) dbg_hal_read16((hal), (addr), __FUNCTION__, __LINE__)
#define hal_read32(hal, addr) dbg_hal_read32((hal), (addr), __FUNCTION__, __LINE__)

#define  hal_write8(hal, addr, val) dbg_hal_write8((hal), (addr), (val), __FUNCTION__, __LINE__)
#define  hal_write16(hal, addr, val) dbg_hal_write16((hal), (addr), (val), __FUNCTION__, __LINE__)
#define  hal_write32(hal, addr, val) dbg_hal_write32((hal), (addr), (val), __FUNCTION__, __LINE__)


#ifdef CONFIG_SDIO_HCI
#define hal_sd_f0_read8(hal, addr) dbg_hal_sd_f0_read8((hal), (addr), __func__, __LINE__)
#ifdef CONFIG_SDIO_INDIRECT_ACCESS
#define hal_sd_iread8(hal, addr) dbg_hal_sd_iread8((hal), (addr), __func__, __LINE__)
#define hal_sd_iread16(hal, addr) dbg_hal_sd_iread16((hal), (addr), __func__, __LINE__)
#define hal_sd_iread32(hal, addr) dbg_hal_sd_iread32((hal), (addr), __func__, __LINE__)
#define hal_sd_iwrite8(hal, addr, val) dbg_hal_sd_iwrite8((hal), (addr), (val), __func__, __LINE__)
#define hal_sd_iwrite16(hal, addr, val) dbg_hal_sd_iwrite16((hal), (addr), (val), __func__, __LINE__)
#define hal_sd_iwrite32(hal, addr, val) dbg_hal_sd_iwrite32((hal), (addr), (val), __func__, __LINE__)
#endif /* CONFIG_SDIO_INDIRECT_ACCESS */
#endif /* CONFIG_SDIO_HCI */

#else /* DBG_IO */
#define hal_read8(hal, addr) _hal_read8((hal), (addr))
#define hal_read16(hal, addr) _hal_read16((hal), (addr))
#define hal_read32(hal, addr) _hal_read32((hal), (addr))

#define  hal_write8(hal, addr, val) _hal_write8((hal), (addr), (val))
#define  hal_write16(hal, addr, val) _hal_write16((hal), (addr), (val))
#define  hal_write32(hal, addr, val) _hal_write32((hal), (addr), (val))


#ifdef CONFIG_SDIO_HCI
#define hal_sd_f0_read8(hal, addr) _hal_sd_f0_read8((hal), (addr))
#ifdef CONFIG_SDIO_INDIRECT_ACCESS
#define hal_sd_iread8(hal, addr) _hal_sd_iread8((hal), (addr))
#define hal_sd_iread16(hal, addr) _hal_sd_iread16((hal), (addr))
#define hal_sd_iread32(hal, addr) _hal_sd_iread32((hal), (addr))
#define hal_sd_iwrite8(hal, addr, val) _hal_sd_iwrite8((hal), (addr), (val))
#define hal_sd_iwrite16(hal, addr, val) _hal_sd_iwrite16((hal), (addr), (val))
#define hal_sd_iwrite32(hal, addr, val) _hal_sd_iwrite32((hal), (addr), (val))
#endif /* CONFIG_SDIO_INDIRECT_ACCESS */
#endif /* CONFIG_SDIO_HCI */

#endif /* DBG_IO */

void hal_write32_mask(struct rtw_hal_com_t *hal, u16 addr, u32 mask, u32 val);

u32 hal_init_io_priv(struct rtw_hal_com_t *hal,
void (*set_intf_ops)(struct rtw_hal_com_t *hal, struct hal_io_ops *ops));
u32 hal_deinit_io_priv(struct rtw_hal_com_t *hal);

#endif /* _RTW_IO_H_ */
