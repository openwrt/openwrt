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
/*
The purpose of hal_io.c

a. provides the API
b. provides the protocol engine
c. provides the software interface between caller and the hardware interface

Compiler Flag Option:
1. CONFIG_SDIO_HCI:
    a. USE_SYNC_IRP:  Only sync operations are provided.
    b. USE_ASYNC_IRP:Both sync/async operations are provided.

2. CONFIG_USB_HCI:
   a. USE_ASYNC_IRP: Both sync/async operations are provided.

3. CONFIG_CFIO_HCI:
   b. USE_SYNC_IRP: Only sync operations are provided.

Only sync read/rtw_write_mem operations are provided.
*/

#define _HAL_IO_C_
#include "hal_headers.h"

u8 _hal_read8(struct rtw_hal_com_t *hal, u32 addr)
{
	u8 r_val;
	struct hal_io_priv *io_priv = &hal->iopriv;
	u8(*_read8)(struct rtw_hal_com_t *hal, u32 addr);

	#ifdef DBG_PHL_MAC_REG_RW
	if (rtw_hal_mac_reg_chk(hal, addr) == false) {
		r_val = 0xEA;
		return r_val;
	}
	#endif

	_read8 = io_priv->io_ops._read8;
	r_val = _read8(hal, addr);
	return r_val;
}

u16 _hal_read16(struct rtw_hal_com_t *hal, u32 addr)
{
	u16 r_val;
	struct hal_io_priv *io_priv = &hal->iopriv;
	u16(*_read16)(struct rtw_hal_com_t *hal, u32 addr);

	#ifdef DBG_PHL_MAC_REG_RW
	if (rtw_hal_mac_reg_chk(hal, addr) == false) {
		r_val = 0xEAEA;
		return r_val;
	}
	#endif

	_read16 = io_priv->io_ops._read16;
	r_val = _read16(hal, addr);
	return r_val;
}

u32 _hal_read32(struct rtw_hal_com_t *hal, u32 addr)
{
	u32 r_val;
	struct hal_io_priv *io_priv = &hal->iopriv;
	u32(*_read32)(struct rtw_hal_com_t *hal, u32 addr);

	#ifdef DBG_PHL_MAC_REG_RW
	if (rtw_hal_mac_reg_chk(hal, addr) == false) {
		r_val = 0xEAEAEAEA;
		return r_val;
	}
	#endif

	_read32 = io_priv->io_ops._read32;
	r_val = _read32(hal, addr);
	return r_val;
}

int _hal_write8(struct rtw_hal_com_t *hal, u32 addr, u8 val)
{
	struct hal_io_priv *io_priv = &hal->iopriv;
	int (*_write8)(struct rtw_hal_com_t *hal, u32 addr, u8 val);
	#ifdef RTW_WKARD_BUS_WRITE
	int (*_write_post_cfg)(struct rtw_hal_com_t *hal, u32 addr,
						   u32 value) = NULL;
	#endif
	int ret;

	#ifdef DBG_PHL_MAC_REG_RW
	if (rtw_hal_mac_reg_chk(hal, addr) == false)
		return 0;
	#endif

	_write8 = io_priv->io_ops._write8;
	ret = _write8(hal, addr, val);

	#ifdef RTW_WKARD_BUS_WRITE
	_write_post_cfg = io_priv->io_ops._write_post_cfg;
	if(NULL != _write_post_cfg) {
		ret = _write_post_cfg(hal, addr, val);
	}
	#endif
	return ret;
}
int _hal_write16(struct rtw_hal_com_t *hal, u32 addr, u16 val)
{
	struct hal_io_priv *io_priv = &hal->iopriv;
	int (*_write16)(struct rtw_hal_com_t *hal, u32 addr, u16 val);
	#ifdef RTW_WKARD_BUS_WRITE
	int (*_write_post_cfg)(struct rtw_hal_com_t *hal, u32 addr,
						   u32 value) = NULL;
	#endif
	int ret;

	#ifdef DBG_PHL_MAC_REG_RW
	if (rtw_hal_mac_reg_chk(hal, addr) == false)
		return 0;
	#endif

	_write16 = io_priv->io_ops._write16;
	ret = _write16(hal, addr, val);

	#ifdef RTW_WKARD_BUS_WRITE
	_write_post_cfg = io_priv->io_ops._write_post_cfg;
	if(NULL != _write_post_cfg) {
		ret = _write_post_cfg(hal, addr, val);
	}
	#endif
	return ret;
}
int _hal_write32(struct rtw_hal_com_t *hal, u32 addr, u32 val)
{
	struct hal_io_priv *io_priv = &hal->iopriv;
	int (*_write32)(struct rtw_hal_com_t *hal, u32 addr, u32 val);
	#ifdef RTW_WKARD_BUS_WRITE
	int (*_write_post_cfg)(struct rtw_hal_com_t *hal, u32 addr,
						   u32 value) = NULL;
	#endif
	int ret;

	#ifdef DBG_PHL_MAC_REG_RW
	if (rtw_hal_mac_reg_chk(hal, addr) == false)
		return 0;
	#endif

	_write32 = io_priv->io_ops._write32;
	ret = _write32(hal, addr, val);

	#ifdef RTW_WKARD_BUS_WRITE
	_write_post_cfg = io_priv->io_ops._write_post_cfg;
	if(NULL != _write_post_cfg) {
		ret = _write_post_cfg(hal, addr, val);
	}
	#endif
	return ret;
}

#ifdef CONFIG_SDIO_HCI
u8 _hal_sd_f0_read8(struct rtw_hal_com_t *hal, u32 addr)
{
	u8 r_val = 0x00;
	struct hal_io_priv *io_priv = &hal->iopriv;
	u8(*_sd_f0_read8)(struct rtw_hal_com_t *hal, u32 addr);

	_sd_f0_read8 = io_priv->io_ops._sd_f0_read8;

	if (_sd_f0_read8)
		r_val = _sd_f0_read8(hal, addr);

	return r_val;
}

#ifdef CONFIG_SDIO_INDIRECT_ACCESS
u8 _hal_sd_iread8(struct rtw_hal_com_t *hal, u32 addr)
{
	u8 r_val = 0x00;
	struct hal_io_priv *io_priv = &hal->iopriv;
	u8(*_sd_iread8)(struct rtw_hal_com_t *hal, u32 addr);

	_sd_iread8 = io_priv->io_ops._sd_iread8;

	if (_sd_iread8)
		r_val = _sd_iread8(hal, addr);
	return r_val;
}

u16 _hal_sd_iread16(struct rtw_hal_com_t *hal, u32 addr)
{
	u16 r_val = 0x00;
	struct hal_io_priv *io_priv = &hal->iopriv;
	u16(*_sd_iread16)(struct rtw_hal_com_t *hal, u32 addr);

	_sd_iread16 = io_priv->io_ops._sd_iread16;

	if (_sd_iread16)
		r_val = _sd_iread16(hal, addr);
	return r_val;
}

u32 _hal_sd_iread32(struct rtw_hal_com_t *hal, u32 addr)
{
	u32 r_val = 0x00;
	struct hal_io_priv *io_priv = &hal->iopriv;
	u32(*_sd_iread32)(struct rtw_hal_com_t *hal, u32 addr);

	_sd_iread32 = io_priv->io_ops._sd_iread32;

	if (_sd_iread32)
		r_val = _sd_iread32(hal, addr);
	return r_val;
}

int _hal_sd_iwrite8(struct rtw_hal_com_t *hal, u32 addr, u8 val)
{
	struct hal_io_priv *io_priv = &hal->iopriv;
	int (*_sd_iwrite8)(struct rtw_hal_com_t *hal, u32 addr, u8 val);
	int ret = -1;

	_sd_iwrite8 = io_priv->io_ops._sd_iwrite8;

	if (_sd_iwrite8)
		ret = _sd_iwrite8(hal, addr, val);

	return ret;
}

int _hal_sd_iwrite16(struct rtw_hal_com_t *hal, u32 addr, u16 val)
{
	struct hal_io_priv *io_priv = &hal->iopriv;
	int (*_sd_iwrite16)(struct rtw_hal_com_t *hal, u32 addr, u16 val);
	int ret = -1;

	_sd_iwrite16 = io_priv->io_ops._sd_iwrite16;

	if (_sd_iwrite16)
		ret = _sd_iwrite16(hal, addr, val);
	return ret;
}
int _hal_sd_iwrite32(struct rtw_hal_com_t *hal, u32 addr, u32 val)
{
	struct hal_io_priv *io_priv = &hal->iopriv;
	int (*_sd_iwrite32)(struct rtw_hal_com_t *hal, u32 addr, u32 val);
	int ret = -1;

	_sd_iwrite32 = io_priv->io_ops._sd_iwrite32;

	if (_sd_iwrite32)
		ret = _sd_iwrite32(hal, addr, val);

	return ret;
}
#endif /* CONFIG_SDIO_INDIRECT_ACCESS */
#endif /* CONFIG_SDIO_HCI */

u32 hal_init_io_priv(struct rtw_hal_com_t *hal,
	void (*set_intf_ops)(struct rtw_hal_com_t *hal, struct hal_io_ops *ops))
{
	struct hal_io_priv *iopriv = &hal->iopriv;

	if (set_intf_ops == NULL)
		return RTW_HAL_STATUS_IO_INIT_FAILURE;

	#ifdef CONFIG_SDIO_INDIRECT_ACCESS
	_os_mutex_init(hal->drv_priv, &iopriv->sd_indirect_access_mutex);
	#endif
	set_intf_ops(hal, &iopriv->io_ops);

	return RTW_HAL_STATUS_SUCCESS;
}
u32 hal_deinit_io_priv(struct rtw_hal_com_t *hal)
{
	#ifdef CONFIG_SDIO_INDIRECT_ACCESS
	struct hal_io_priv *iopriv = &hal->iopriv;

	_os_mutex_init(hal->drv_priv, &iopriv->sd_indirect_access_mutex);
	#endif

	return RTW_HAL_STATUS_SUCCESS;
}

/*******************common IO  APIs *******************/
static inline u32 _bit_shift(u32 mask)
{
	u32 i;

	for (i = 0; i <= 31; i++)
		if (mask & BIT(i))
			break;

	return i;
}

u32 hal_read_macreg(struct hal_info_t *hal,
		u32 offset, u32 bit_mask)
{
	u32 val = 0, val32, shift;

	val32 = hal_read32(hal->hal_com, offset);
	if (bit_mask != 0xFFFFFFFF) {
		shift = _bit_shift(bit_mask);
		val = (val32 & bit_mask) >> shift;
		return val;
	}
	else {
		return val32;
	}
}
void hal_write_macreg(struct hal_info_t *hal,
		u32 offset, u32 bit_mask, u32 data)
{
	u32 val32, shift;
	
	if (bit_mask != 0xFFFFFFFF) {
		val32 = hal_read32(hal->hal_com, offset);
		shift = _bit_shift(bit_mask);
		data = ((val32 & (~bit_mask)) | ((data << shift) & bit_mask));
	}
	
	hal_write32(hal->hal_com, offset, data);
}

u32 hal_read_bbreg(struct hal_info_t *hal,
		u32 offset, u32 bit_mask)
{
	u32 val32 = 0;
	val32 = rtw_hal_read_bb_reg(hal->hal_com, offset, bit_mask);
	
	return val32;
}

void hal_write_bbreg(struct hal_info_t *hal,
		u32 offset, u32 bit_mask, u32 data)
{
	rtw_hal_write_bb_reg(hal->hal_com, offset, bit_mask, data);
}

u32 hal_read_rfreg(struct hal_info_t *hal,
		enum rf_path path, u32 offset, u32 bit_mask)
{
	u32 val32 = 0;
	val32 = rtw_hal_read_rf_reg(hal->hal_com, path, offset, bit_mask);
#ifdef DBG_IO
	if (match_rf_read_sniff_ranges(hal->hal_com, path, offset, bit_mask)) {
		PHL_INFO("DBG_IO hal_read_rfreg(%u, 0x%04x, 0x%08x) read:0x%08x(0x%08x)\n"
			, path, offset, bit_mask, (val32 << _bit_shift(bit_mask)), val32);
	}
#endif

	return val32;
}
void hal_write_rfreg(struct hal_info_t *hal,
		enum rf_path path, u32 offset, u32 bit_mask, u32 data)
{
	rtw_hal_write_rf_reg(hal->hal_com, path, offset, bit_mask, data);
#ifdef DBG_IO
	if (match_rf_write_sniff_ranges(hal->hal_com, path, offset, bit_mask)) {
		PHL_INFO("DBG_IO hal_write_rfreg(%u, 0x%04x, 0x%08x) write:0x%08x(0x%08x)\n"
			, path, offset, bit_mask, (data << _bit_shift(bit_mask)), data);
	}
#endif
}

static u32 _cal_bit_shift(u32 bit_mask)
{
	u32 i;

	for (i = 0; i <= 31; i++) {
		if (((bit_mask >> i) &	0x1) == 1)
			break;
	}

	return (i);
}

void hal_write32_mask(struct rtw_hal_com_t *hal, u16 addr, u32 mask, u32 val)
{
	u32 original_value, new_value;
	u32 bit_shift;

	if (mask == 0xFFFFFFFF) {
		hal_write32(hal, addr, val);
	} else {
		original_value = hal_read32(hal, addr);
		bit_shift = _cal_bit_shift(mask);
		new_value = (((original_value) & (~mask)) | ((val << bit_shift) & mask));
		hal_write32(hal, addr, new_value);
	}
}


#ifdef DBG_IO
#define RTW_IO_SNIFF_TYPE_RANGE	0 /* specific address range is accessed */
#define RTW_IO_SNIFF_TYPE_VALUE	1 /* value match for sniffed range */

struct rtw_io_sniff_ent {
	u8 chip;
	u8 hci;
	u32 addr;
	u8 type;
	union {
		u32 end_addr;
		struct {
			u32 mask;
			u32 val;
			bool equal;
		} vm; /* value match */
	} u;
	bool trace;
	char *tag;
};

#define RTW_IO_SNIFF_RANGE_ENT(_chip, _hci, _addr, _end_addr, _trace, _tag) \
	{.chip = _chip, .hci = _hci, .addr = _addr, .u.end_addr = _end_addr, .trace = _trace, .tag = _tag, .type = RTW_IO_SNIFF_TYPE_RANGE,}

#define RTW_IO_SNIFF_VALUE_ENT(_chip, _hci, _addr, _mask, _val, _equal, _trace, _tag) \
	{.chip = _chip, .hci = _hci, .addr = _addr, .u.vm.mask = _mask, .u.vm.val = _val, .u.vm.equal = _equal, .trace = _trace, .tag = _tag, .type = RTW_IO_SNIFF_TYPE_VALUE,}

/* part or all sniffed range is enabled (not all 0) */
#define RTW_IO_SNIFF_EN_ENT(_chip, _hci, _addr, _mask, _trace, _tag) \
	{.chip = _chip, .hci = _hci, .addr = _addr, .u.vm.mask = _mask, .u.vm.val = 0, .u.vm.equal = 0, .trace = _trace, .tag = _tag, .type = RTW_IO_SNIFF_TYPE_VALUE,}

/* part or all sniffed range is disabled (not all 1) */
#define RTW_IO_SNIFF_DIS_ENT(_chip, _hci, _addr, _mask, _trace, _tag) \
	{.chip = _chip, .hci = _hci, .addr = _addr, .u.vm.mask = _mask, .u.vm.val = 0xFFFFFFFF, .u.vm.equal = 0, .trace = _trace, .tag = _tag, .type = RTW_IO_SNIFF_TYPE_VALUE,}

const struct rtw_io_sniff_ent read_sniff[] = {
#ifdef DBG_IO_HCI_EN_CHK
	RTW_IO_SNIFF_EN_ENT(CHIP_WIFI6_MAX, RTW_HCI_SDIO, 0x02, 0x1FC, 1, "SDIO 0x02[8:2] not all 0"),
	RTW_IO_SNIFF_EN_ENT(CHIP_WIFI6_MAX, RTW_HCI_USB, 0x02, 0x1E0, 1, "USB 0x02[8:5] not all 0"),
	RTW_IO_SNIFF_EN_ENT(CHIP_WIFI6_MAX, RTW_HCI_PCIE, 0x02, 0x01C, 1, "PCI 0x02[4:2] not all 0"),
#endif
#ifdef DBG_IO_SNIFF_EXAMPLE
	RTW_IO_SNIFF_RANGE_ENT(CHIP_WIFI6_MAX, 0, 0x522, 0x522, 0, "read TXPAUSE"),
	RTW_IO_SNIFF_DIS_ENT(CHIP_WIFI6_MAX, 0, 0x02, 0x3, 0, "0x02[1:0] not all 1"),
#endif
};

const int read_sniff_num = sizeof(read_sniff) / sizeof(struct rtw_io_sniff_ent);

const struct rtw_io_sniff_ent write_sniff[] = {
#ifdef DBG_IO_HCI_EN_CHK
	RTW_IO_SNIFF_EN_ENT(CHIP_WIFI6_MAX, RTW_HCI_SDIO, 0x02, 0x1FC, 1, "SDIO 0x02[8:2] not all 0"),
	RTW_IO_SNIFF_EN_ENT(CHIP_WIFI6_MAX, RTW_HCI_SDIO, 0x02, 0x1E0, 1, "USB 0x02[8:5] not all 0"),
	RTW_IO_SNIFF_EN_ENT(CHIP_WIFI6_MAX, RTW_HCI_SDIO, 0x02, 0x01C, 1, "PCI 0x02[4:2] not all 0"),
#endif
#ifdef DBG_IO_SNIFF_EXAMPLE
	RTW_IO_SNIFF_RANGE_ENT(CHIP_WIFI6_MAX, 0, 0x522, 0x522, 0, "write TXPAUSE"),
	RTW_IO_SNIFF_DIS_ENT(CHIP_WIFI6_MAX, 0, 0x02, 0x3, 0, "0x02[1:0] not all 1"),
#endif
};

const int write_sniff_num = sizeof(write_sniff) / sizeof(struct rtw_io_sniff_ent);

static bool match_io_sniff_ranges(struct rtw_hal_com_t *hal
	, const struct rtw_io_sniff_ent *sniff, int i, u32 addr, u16 len)
{

	/* check if IO range after sniff end address */
	if (addr > sniff->u.end_addr)
		return 0;

	return 1;
}

static bool match_io_sniff_value(struct rtw_hal_com_t *hal
	, const struct rtw_io_sniff_ent *sniff, int i, u32 addr, u8 len, u32 val)
{
	u8 sniff_len;
	s8 mask_shift;
	u32 mask;
	s8 value_shift;
	u32 value;
	bool ret = 0;

	/* check if IO range after sniff end address */
	sniff_len = 4;
	while (!(sniff->u.vm.mask & (0xFF << ((sniff_len - 1) * 8)))) {
		sniff_len--;
		if (sniff_len == 0)
			goto exit;
	}
	if (sniff->addr + sniff_len <= addr)
		goto exit;

	/* align to IO addr */
	mask_shift = (sniff->addr - addr) * 8;
	value_shift = mask_shift + bitshift(sniff->u.vm.mask);
	if (mask_shift > 0)
		mask = sniff->u.vm.mask << mask_shift;
	else if (mask_shift < 0)
		mask = sniff->u.vm.mask >> -mask_shift;
	else
		mask = sniff->u.vm.mask;

	if (value_shift > 0)
		value = sniff->u.vm.val << value_shift;
	else if (mask_shift < 0)
		value = sniff->u.vm.val >> -value_shift;
	else
		value = sniff->u.vm.val;

	if ((sniff->u.vm.equal && (mask & val) == (mask & value))
		|| (!sniff->u.vm.equal && (mask & val) != (mask & value))
	) {
		ret = 1;
		if (0)
			PHL_INFO(" addr:0x%x len:%u val:0x%x (i:%d sniff_len:%u m_shift:%d mask:0x%x v_shifd:%d value:0x%x equal:%d)\n"
				, addr, len, val, i, sniff_len, mask_shift, mask, value_shift, value, sniff->u.vm.equal);
	}

exit:
	return ret;
}

static bool match_io_sniff(struct rtw_hal_com_t *hal
	, const struct rtw_io_sniff_ent *sniff, int i, u32 addr, u8 len, u32 val)
{
	bool ret = 0;

	if (sniff->chip != CHIP_WIFI6_MAX
		&& sniff->chip != hal_get_chip_id(hal))
		goto exit;
	/*
	if (sniff->hci
		&& !(sniff->hci & hal_get_hci_type(hal)))
		goto exit;
	*/
	if (sniff->addr >= addr + len) /* IO range below sniff start address */
		goto exit;

	switch (sniff->type) {
	case RTW_IO_SNIFF_TYPE_RANGE:
		ret = match_io_sniff_ranges(hal, sniff, i, addr, len);
		break;
	case RTW_IO_SNIFF_TYPE_VALUE:
		if (len == 1 || len == 2 || len == 4)
			ret = match_io_sniff_value(hal, sniff, i, addr, len, val);
		break;
	default:
		/*_os_warn_on(1);*/
		break;
	}

exit:
	return ret;
}

u32 match_read_sniff(struct rtw_hal_com_t *hal, u32 addr, u16 len, u32 val)
{
	int i;
	bool trace = 0;
	u32 match = 0;

	for (i = 0; i < read_sniff_num; i++) {
		if (match_io_sniff(hal, &read_sniff[i], i, addr, len, val)) {
			match++;
			trace |= read_sniff[i].trace;
			if (read_sniff[i].tag)
				PHL_INFO("DBG_IO TAG %s\n", read_sniff[i].tag);
		}
	}

	/*_os_warn_on(trace);*/

	return match;
}

u32 match_write_sniff(struct rtw_hal_com_t *hal, u32 addr, u16 len, u32 val)
{
	int i;
	bool trace = 0;
	u32 match = 0;

	for (i = 0; i < write_sniff_num; i++) {
		if (match_io_sniff(hal, &write_sniff[i], i, addr, len, val)) {
			match++;
			trace |= write_sniff[i].trace;
			if (write_sniff[i].tag)
				PHL_INFO("DBG_IO TAG %s\n", write_sniff[i].tag);
		}
	}

	/*_os_warn_on(trace);*/

	return match;
}

struct rf_sniff_ent {
	u8 path;
	u16 reg;
	u32 mask;
};

struct rf_sniff_ent rf_read_sniff_ranges[] = {
	/* example for all path addr 0x55 with all RF Reg mask */
	/* {MAX_RF_PATH, 0x55, bRFRegOffsetMask}, */
};

struct rf_sniff_ent rf_write_sniff_ranges[] = {
	/* example for all path addr 0x55 with all RF Reg mask */
	/* {MAX_RF_PATH, 0x55, bRFRegOffsetMask}, */
};

int rf_read_sniff_num = sizeof(rf_read_sniff_ranges) / sizeof(struct rf_sniff_ent);
int rf_write_sniff_num = sizeof(rf_write_sniff_ranges) / sizeof(struct rf_sniff_ent);

bool match_rf_read_sniff_ranges(struct rtw_hal_com_t *hal, u8 path, u32 addr, u32 mask)
{
	int i;

	for (i = 0; i < rf_read_sniff_num; i++) {
		if (rf_read_sniff_ranges[i].path == MAX_RF_PATH ||
			rf_read_sniff_ranges[i].path == path)
			if ((addr == rf_read_sniff_ranges[i].reg) &&
				(mask & rf_read_sniff_ranges[i].mask))
				return true;
	}

	return false;
}

bool match_rf_write_sniff_ranges(struct rtw_hal_com_t *hal,
					u8 path, u32 addr, u32 mask)
{
	int i;

	for (i = 0; i < rf_write_sniff_num; i++) {
		if (rf_write_sniff_ranges[i].path == MAX_RF_PATH ||
			rf_write_sniff_ranges[i].path == path)
			if ((addr == rf_write_sniff_ranges[i].reg) && 
				(mask & rf_write_sniff_ranges[i].mask))
				return true;
	}

	return false;
}

u8 dbg_hal_read8(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line)
{
	u8 val = _hal_read8(hal, addr);

	if (match_read_sniff(hal, addr, 1, val)) {
		PHL_INFO("DBG_IO %s:%d hal_read8(0x%04x) return 0x%02x\n"
			, caller, line, addr, val);
	}

	return val;
}

u16 dbg_hal_read16(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line)
{
	u16 val = _hal_read16(hal, addr);

	if (match_read_sniff(hal, addr, 2, val)) {
		PHL_INFO("DBG_IO %s:%d hal_read16(0x%04x) return 0x%04x\n"
			, caller, line, addr, val);
	}

	return val;
}

u32 dbg_hal_read32(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line)
{
	u32 val = _hal_read32(hal, addr);

	if (match_read_sniff(hal, addr, 4, val)) {
		PHL_INFO("DBG_IO %s:%d hal_read32(0x%04x) return 0x%08x\n"
			, caller, line, addr, val);
	}

	return val;
}

int dbg_hal_write8(struct rtw_hal_com_t *hal, u32 addr, u8 val, const char *caller, const int line)
{
	if (match_write_sniff(hal, addr, 1, val)) {
		PHL_INFO("DBG_IO %s:%d hal_write8(0x%04x, 0x%02x)\n"
			, caller, line, addr, val);
	}

	return _hal_write8(hal, addr, val);
}
int dbg_hal_write16(struct rtw_hal_com_t *hal, u32 addr, u16 val, const char *caller, const int line)
{
	if (match_write_sniff(hal, addr, 2, val)) {
		PHL_INFO("DBG_IO %s:%d hal_write16(0x%04x, 0x%04x)\n"
			, caller, line, addr, val);
	}

	return _hal_write16(hal, addr, val);
}
int dbg_hal_write32(struct rtw_hal_com_t *hal, u32 addr, u32 val, const char *caller, const int line)
{
	if (match_write_sniff(hal, addr, 4, val)) {
		PHL_INFO("DBG_IO %s:%d hal_write32(0x%04x, 0x%08x)\n"
			, caller, line, addr, val);
	}

	return _hal_write32(hal, addr, val);
}

#ifdef CONFIG_SDIO_HCI
u8 dbg_hal_sd_f0_read8(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line)
{
	u8 val = _hal_sd_f0_read8(hal, addr);

#if 0
	if (match_read_sniff(adapter, addr, 1, val)) {
		PHL_INFO("DBG_IO %s:%d hal_sd_f0_read8(0x%04x) return 0x%02x\n"
			, caller, line, addr, val);
	}
#endif

	return val;
}

#ifdef CONFIG_SDIO_INDIRECT_ACCESS
u8 dbg_hal_sd_iread8(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line)
{
	u8 val = hal_sd_iread8(hal, addr);

	if (match_read_sniff(hal, addr, 1, val)) {
		PHL_INFO("DBG_IO %s:%d hal_sd_iread8(0x%04x) return 0x%02x\n"
			, caller, line, addr, val);
	}

	return val;
}

u16 dbg_hal_sd_iread16(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line)
{
	u16 val = _hal_sd_iread16(hal, addr);

	if (match_read_sniff(hal, addr, 2, val)) {
		PHL_INFO("DBG_IO %s:%d hal_sd_iread16(0x%04x) return 0x%04x\n"
			, caller, line, addr, val);
	}

	return val;
}

u32 dbg_hal_sd_iread32(struct rtw_hal_com_t *hal, u32 addr, const char *caller, const int line)
{
	u32 val = _hal_sd_iread32(hal, addr);

	if (match_read_sniff(hal, addr, 4, val)) {
		PHL_INFO("DBG_IO %s:%d hal_sd_iread32(0x%04x) return 0x%08x\n"
			, caller, line, addr, val);
	}

	return val;
}

int dbg_hal_sd_iwrite8(struct rtw_hal_com_t *hal, u32 addr, u8 val, const char *caller, const int line)
{
	if (match_write_sniff(hal, addr, 1, val)) {
		PHL_INFO("DBG_IO %s:%d hal_sd_iwrite8(0x%04x, 0x%02x)\n"
			, caller, line, addr, val);
	}

	return _hal_sd_iwrite8(hal, addr, val);
}
int dbg_hal_sd_iwrite16(struct rtw_hal_com_t *hal, u32 addr, u16 val, const char *caller, const int line)
{
	if (match_write_sniff(hal, addr, 2, val)) {
		PHL_INFO("DBG_IO %s:%d hal_sd_iwrite16(0x%04x, 0x%04x)\n"
			, caller, line, addr, val);
	}

	return _hal_sd_iwrite16(hal, addr, val);
}
int dbg_hal_sd_iwrite32(struct rtw_hal_com_t *hal, u32 addr, u32 val, const char *caller, const int line)
{
	if (match_write_sniff(hal, addr, 4, val)) {
		PHL_INFO("DBG_IO %s:%d hal_sd_iwrite32(0x%04x, 0x%08x)\n"
			, caller, line, addr, val);
	}

	return _hal_sd_iwrite32(hal, addr, val);
}

#endif /* CONFIG_SDIO_INDIRECT_ACCESS */

#endif /* CONFIG_SDIO_HCI */
#endif
