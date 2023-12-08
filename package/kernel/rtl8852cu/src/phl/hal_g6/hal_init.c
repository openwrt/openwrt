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
#define _HAL_INIT_C_
#include "hal_headers.h"

#if defined(CONFIG_PCI_HCI)
#include "hal_pci.h"
#elif defined(CONFIG_USB_HCI)
#include "hal_usb.h"
#elif defined(CONFIG_SDIO_HCI)
#include "hal_sdio.h"
#endif


/******************* IO  APIs *******************/
u8 rtw_hal_read8(void *h, u32 addr)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)h;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	return hal_read8(hal_com, addr);
}
u16 rtw_hal_read16(void *h, u32 addr)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)h;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	return hal_read16(hal_com, addr);
}
u32 rtw_hal_read32(void *h, u32 addr)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)h;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	return hal_read32(hal_com, addr);
}
void rtw_hal_write8(void *h, u32 addr, u8 val)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)h;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	hal_write8(hal_com, addr, val);
}
void rtw_hal_write16(void *h, u32 addr, u16 val)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)h;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	hal_write16(hal_com, addr, val);
}
void rtw_hal_write32(void *h, u32 addr, u32 val)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)h;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	hal_write32(hal_com, addr, val);
}

u32 rtw_hal_read_macreg(void *h, u32 offset, u32 bit_mask)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	return hal_ops->read_macreg(hal, offset, bit_mask);
}
void rtw_hal_write_macreg(void *h,
			u32 offset, u32 bit_mask, u32 data)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	hal_ops->write_macreg(hal, offset, bit_mask, data);
}
u32 rtw_hal_read_bbreg(void *h, u32 offset, u32 bit_mask)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	return hal_ops->read_bbreg(hal, offset, bit_mask);
}
void rtw_hal_write_bbreg(void *h,
			u32 offset, u32 bit_mask, u32 data)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	hal_ops->write_bbreg(hal, offset, bit_mask, data);
}

u32 rtw_hal_read_rfreg(void *h,
			enum rf_path path, u32 offset, u32 bit_mask)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	return hal_ops->read_rfreg(hal, path, offset, bit_mask);
}

void rtw_hal_write_rfreg(void *h,
			enum rf_path path, u32 offset, u32 bit_mask, u32 data)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	hal_ops->write_rfreg(hal, path, offset, bit_mask, data);
}


#ifdef PHL_PLATFORM_LINUX
void rtw_hal_mac_reg_dump(void *sel, void *h)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)h;
	rtw_hal_notification(hal_info, MSG_EVT_DBG_FULL_REG_DUMP, HW_BAND_MAX);
}


void rtw_hal_bb_reg_dump(void *sel, void *h)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)h;
	struct rtw_hal_com_t *hal = hal_info->hal_com;
	int i, j = 1;

	RTW_PRINT_SEL(sel, "======= BB REG =======\n");
	for (i = 0x800; i < 0x1000; i += 4) {
		if (j % 4 == 1)
			RTW_PRINT_SEL(sel, "0x%04x", i);
		_RTW_PRINT_SEL(sel, " 0x%08x ", hal_read32(hal, i));
		if ((j++) % 4 == 0)
			_RTW_PRINT_SEL(sel, "\n");
	}

#if 0
	for (i = 0x1800; i < 0x2000; i += 4) {
		if (j % 4 == 1)
			RTW_PRINT_SEL(sel, "0x%04x", i);
		_RTW_PRINT_SEL(sel, " 0x%08x ", hal_read32(hal, i));
		if ((j++) % 4 == 0)
			_RTW_PRINT_SEL(sel, "\n");
	}
#endif

#if 0
	for (i = 0x2c00; i < 0x2c60; i += 4) {
		if (j % 4 == 1)
			RTW_PRINT_SEL(sel, "0x%04x", i);
		_RTW_PRINT_SEL(sel, " 0x%08x ", rtw_read32(adapter, i));
		if ((j++) % 4 == 0)
			_RTW_PRINT_SEL(sel, "\n");
	}

	for (i = 0x2d00; i < 0x2df0; i += 4) {
		if (j % 4 == 1)
			RTW_PRINT_SEL(sel, "0x%04x", i);
		_RTW_PRINT_SEL(sel, " 0x%08x ", rtw_read32(adapter, i));
		if ((j++) % 4 == 0)
			_RTW_PRINT_SEL(sel, "\n");
	}

	for (i = 0x4000; i < 0x4060; i += 4) {
		if (j % 4 == 1)
			RTW_PRINT_SEL(sel, "0x%04x", i);
		_RTW_PRINT_SEL(sel, " 0x%08x ", rtw_read32(adapter, i));
		if ((j++) % 4 == 0)
			_RTW_PRINT_SEL(sel, "\n");
	}

	for (i = 0x4100; i < 0x4200; i += 4) {
		if (j % 4 == 1)
			RTW_PRINT_SEL(sel, "0x%04x", i);
		_RTW_PRINT_SEL(sel, " 0x%08x ", rtw_read32(adapter, i));
		if ((j++) % 4 == 0)
			_RTW_PRINT_SEL(sel, "\n");
	}

#endif

}

void rtw_hal_bb_reg_dump_ex(void *sel, void *h)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)h;
	struct rtw_hal_com_t *hal = hal_info->hal_com;
	int i;

	RTW_PRINT_SEL(sel, "======= BB REG =======\n");
	for (i = 0x800; i < 0x1000; i += 4) {
		RTW_PRINT_SEL(sel, "0x%04x", i);
		_RTW_PRINT_SEL(sel, " 0x%08x ", hal_read32(hal, i));
		_RTW_PRINT_SEL(sel, "\n");
	}

#if 0
	for (i = 0x1800; i < 0x2000; i += 4) {
		RTW_PRINT_SEL(sel, "0x%04x", i);
		_RTW_PRINT_SEL(sel, " 0x%08x ", hal_read32(hal, i));
		_RTW_PRINT_SEL(sel, "\n");
	}
#endif
}


void rtw_hal_rf_reg_dump(void *sel, void *h)
{
#ifdef RTW_WKARD_RF_CR_DUMP
	int i, j = 1, path;
	struct hal_info_t *hal_info = (struct hal_info_t *)h;
	struct rtw_hal_com_t *hal = hal_info->hal_com;
	struct rtw_phl_com_t *phl_com = hal_info->phl_com;
	u32 value;
	u8 path_nums;

	path_nums = (phl_com->phy_cap[0].tx_path_num > phl_com->phy_cap[0].rx_path_num) ?
	             phl_com->phy_cap[0].tx_path_num : phl_com->phy_cap[0].rx_path_num;

	RTW_PRINT_SEL(sel, "======= RF REG =======\n");
	for (path = 0; path < path_nums; path++) {
		RTW_PRINT_SEL(sel, "RF_Path(%x)\n", path);
		for (i = 0; i < 0x100; i++) {
			value = rtw_hal_read_rfreg(hal_info, path, i, 0xfffff);
			if (j % 4 == 1)
				RTW_PRINT_SEL(sel, "0x%02x ", i);
			_RTW_PRINT_SEL(sel, " 0x%08x ", value);
			if ((j++) % 4 == 0)
				_RTW_PRINT_SEL(sel, "\n");
		}
		j = 1;
		#ifdef CONFIG_RTL8852B
		for (i = 0x10000; i < 0x10100; i++) {
			value = rtw_hal_read_rfreg(hal_info, path, i, 0xfffff);
			if (j % 4 == 1)
				RTW_PRINT_SEL(sel, "0x%02x ", i);
			_RTW_PRINT_SEL(sel, " 0x%08x ", value);
			if ((j++) % 4 == 0)
				_RTW_PRINT_SEL(sel, "\n");
		}
		_RTW_PRINT_SEL(sel, "\n");
		#endif
	}
#endif/*RTW_WKARD_RF_CR_DUMP*/

}
#endif

/**
 * rtw_hal_get_sec_cam() - get the security cam raw data from HW
 * @h:		struct hal_info_t *
 * @num:		How many cam you wnat to dump from the first one.
 * @buf:		ptr to buffer which store the content from HW.
 *			If buf is NULL, use console as debug path.
 * @size		Size of allocated memroy for @buf.
 *			The size should be @num * size of security cam offset(0x20).
 *
 * Return rtw_hal_mac_get_addr_cam's return value in enum rtw_hal_status type.
 */
enum rtw_hal_status
rtw_hal_get_sec_cam(void *h, u16 num, u8 *buf, u16 size)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)h;
	enum rtw_hal_status ret = RTW_HAL_STATUS_SUCCESS;

	ret = rtw_hal_mac_get_sec_cam(hal_info, num, buf, size);

	return ret;
}

/**
 * rtw_hal_get_addr_cam() - get the address cam raw data from HW
 * @h:		struct hal_info_t *
 * @num:		How many cam you wnat to dump from the first one.
 * @buf:		ptr to buffer which store the content from HW.
 *			If buf is NULL, use console as debug path.
 * @size		Size of allocated memroy for @buf.
 *			The size should be @num * size of Addr cam offset(0x40).
 *
 * Return rtw_hal_mac_get_addr_cam's return value in enum rtw_hal_status type.
 */
enum rtw_hal_status
rtw_hal_get_addr_cam(void *h, u16 num, u8 *buf, u16 size)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)h;
	enum rtw_hal_status ret = RTW_HAL_STATUS_SUCCESS;

	ret = rtw_hal_mac_get_addr_cam(hal_info, num, buf, size);

	return ret;
}

void rtw_hal_init_int_default_value(struct rtw_phl_com_t *phl_com, void *h,
					enum rtw_hal_int_set_opt opt)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	if (hal_ops->init_int_default_value)
		hal_ops->init_int_default_value(hal, opt);
}

void rtw_hal_enable_interrupt(struct rtw_phl_com_t *phl_com, void *h)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	if (hal_ops->enable_interrupt)
		hal_ops->enable_interrupt(hal);
}

void rtw_hal_disable_interrupt_isr(struct rtw_phl_com_t *phl_com, void *h)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	if (hal_ops->disable_interrupt_isr)
		hal_ops->disable_interrupt_isr(hal);
}

void rtw_hal_disable_interrupt(struct rtw_phl_com_t *phl_com, void *h)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	if (hal_ops->disable_interrupt)
		hal_ops->disable_interrupt(hal);
}

void rtw_hal_config_interrupt(void *h, enum rtw_phl_config_int int_mode)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	if (hal_ops->config_interrupt)
		hal_ops->config_interrupt(hal, int_mode);
}

bool rtw_hal_recognize_interrupt(void *h)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	if (hal_ops->recognize_interrupt)
		return hal_ops->recognize_interrupt(hal);
	else
		return false;
}

bool rtw_hal_recognize_halt_c2h_interrupt(void *h)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	if (hal_ops->recognize_halt_c2h_interrupt)
		return hal_ops->recognize_halt_c2h_interrupt(hal);
	else
		return false;
}

void rtw_hal_clear_interrupt(void *h)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	if (hal_ops->clear_interrupt)
		hal_ops->clear_interrupt(hal);
}

u32 rtw_hal_interrupt_handler(void *h)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);
	u32 ret = 0;


	if (hal_ops->interrupt_handler)
		ret = hal_ops->interrupt_handler(hal);
	else
		PHL_DBG("hal_ops->interrupt_handler is NULL\n");

	PHL_DBG("hal_ops->interrupt_handler ret=0x%x\n", ret);


	return ret;
}

void rtw_hal_restore_interrupt(struct rtw_phl_com_t *phl_com, void *h)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	if (hal_ops->restore_interrupt)
		hal_ops->restore_interrupt(hal);
}

void rtw_hal_restore_rx_interrupt(void *h)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal);

	if (hal_ops->restore_rx_interrupt)
		hal_ops->restore_rx_interrupt(hal);
	else
		PHL_DBG("hal_ops->restore_rx_interrupt is NULL\n");
}

#ifdef PHL_RXSC_ISR
u16 rtw_hal_rpq_isr_check(void *h, u8 dma_ch)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)h;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	u32 ret = RTW_HAL_STATUS_FAILURE;

	if (hal_ops->check_rpq_isr)
		ret = hal_ops->check_rpq_isr(dma_ch, hal_info->hal_com->rx_int_array);

	if(ret == RTW_HAL_STATUS_SUCCESS)
		return false;
	else
		return true;
}
#endif

static enum rtw_hal_status hal_ops_check(struct hal_info_t *hal)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct hal_ops_t *ops = hal_get_ops(hal);
	struct hal_trx_ops *trx_ops = hal_get_trx_ops(hal);

	/***hal_ops initialize section ***/
	if (!ops->init_hal_spec) {
		hal_error_msg("init_hal_spec");
		status = RTW_HAL_STATUS_FAILURE;
	}

	if (!ops->read_chip_version) {
		hal_error_msg("read_chip_version");
		status = RTW_HAL_STATUS_FAILURE;
	}

	if (!ops->hal_init) {
		hal_error_msg("hal_init");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->hal_deinit) {
		hal_error_msg("hal_deinit");
		status = RTW_HAL_STATUS_FAILURE;
	}

	if (!ops->hal_start) {
		hal_error_msg("hal_start");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->hal_stop) {
		hal_error_msg("hal_stop");
		status = RTW_HAL_STATUS_FAILURE;
	}
#ifdef CONFIG_WOWLAN
	if (!ops->hal_wow_init) {
		hal_error_msg("hal_wow_init");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->hal_wow_deinit) {
		hal_error_msg("hal_wow_deinit");
		status = RTW_HAL_STATUS_FAILURE;
	}
#endif /* CONFIG_WOWLAN */
	if (!ops->hal_mp_init) {
		hal_error_msg("hal_mp_init");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->hal_mp_deinit) {
		hal_error_msg("hal_mp_deinit");
		status = RTW_HAL_STATUS_FAILURE;
	}

	if (!ops->hal_cfg_fw) {
		hal_error_msg("hal_cfg_fw");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->init_default_value) {
		hal_error_msg("init_default_value");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->init_int_default_value) {
		hal_error_msg("init_int_default_value");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->hal_hci_configure) {
		hal_error_msg("hal_hci_configure");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->read_macreg) {
		hal_error_msg("read_macreg");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->write_macreg) {
		hal_error_msg("write_macreg");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->read_bbreg) {
		hal_error_msg("read_bbreg");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->write_bbreg) {
		hal_error_msg("write_bbreg");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->read_rfreg) {
		hal_error_msg("read_rfreg");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->write_rfreg) {
		hal_error_msg("write_rfreg");
		status = RTW_HAL_STATUS_FAILURE;
	}

#if defined(CONFIG_PCI_HCI)
	if (!ops->disable_interrupt_isr) {
		hal_error_msg("disable_interrupt_isr");
		status = RTW_HAL_STATUS_FAILURE;
	}
#endif

#if defined(CONFIG_PCI_HCI) || defined(CONFIG_SDIO_HCI)
	if (!ops->enable_interrupt) {
		hal_error_msg("enable_interrupt");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->disable_interrupt) {
		hal_error_msg("disable_interrupt");
		status = RTW_HAL_STATUS_FAILURE;
	}
#ifdef CONFIG_SDIO_HCI
	if (!ops->config_interrupt) {
		hal_error_msg("config_interrupt");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->recognize_halt_c2h_interrupt) {
		hal_error_msg("recognize_halt_c2h_interrupt");
		status = RTW_HAL_STATUS_FAILURE;
	}
#endif
	if (!ops->recognize_interrupt) {
		hal_error_msg("recognize_interrupt");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->clear_interrupt) {
		hal_error_msg("clear_interrupt");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->interrupt_handler) {
		hal_error_msg("interrupt_handler");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!ops->restore_interrupt) {
		hal_error_msg("restore_interrupt");
		status = RTW_HAL_STATUS_FAILURE;
	}
#ifdef PHL_RXSC_ISR
	if (!ops->check_rpq_isr) {
		hal_error_msg("check_rpq_isr");
		status = RTW_HAL_STATUS_FAILURE;
	}
#endif
#endif /*defined(CONFIG_PCI_HCI) || defined(CONFIG_SDIO_HCI)*/


	/***hal_trx_ops section ***/
	if (!trx_ops->init) {
		hal_error_msg("trx init");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->deinit) {
		hal_error_msg("trx deinit");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->map_hw_tx_chnl) {
		hal_error_msg("trx map_hw_tx_chnl");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->get_fwcmd_queue_idx) {
		hal_error_msg("trx get_fwcmd_queue_idx");
		status = RTW_HAL_STATUS_FAILURE;
	}
#ifdef CONFIG_PCI_HCI
	if (!trx_ops->query_tx_res) {
		hal_error_msg("trx query_tx_res");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->query_rx_res) {
		hal_error_msg("trx query_rx_res");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->get_rxbd_num) {
		hal_error_msg("trx get_rxbd_num");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->get_rxbuf_num) {
		hal_error_msg("trx get_rxbuf_num");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->cfg_wow_txdma) {
		hal_error_msg("trx cfg_wow_txdma");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->poll_txdma_idle) {
		hal_error_msg("trx poll_txdma_idle");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->qsel_to_tid) {
		hal_error_msg("trx qsel_to_tid");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->query_txch_hwband) {
		hal_error_msg("trx query_txch_hwband");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->query_txch_map) {
		hal_error_msg("trx query_txch_map");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->query_txch_num) {
		hal_error_msg("trx query_txch_num");
		status = RTW_HAL_STATUS_FAILURE;
	}

	if (!trx_ops->query_rxch_num) {
		hal_error_msg("trx query_rxch_num");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->update_wd) {
		hal_error_msg("trx update_wd");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->update_txbd) {
		hal_error_msg("trx update_txbd");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->tx_start) {
		hal_error_msg("trx tx_start");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->check_rxrdy) {
		hal_error_msg("trx check_rxrdy");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->handle_rxbd_info) {
		hal_error_msg("trx handle_rxbd_info");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->handle_rx_buffer) {
		hal_error_msg("trx handle_rx_buffer");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->update_rxbd) {
		hal_error_msg("trx update_rxbd");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->notify_rxdone) {
		hal_error_msg("trx notify_rxdone");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->handle_wp_rpt) {
		hal_error_msg("trx handle_wp_rpt");
		status = RTW_HAL_STATUS_FAILURE;
	}
#endif /*CONFIG_PCIE_HCI*/

#ifdef CONFIG_USB_HCI
	if (!trx_ops->hal_get_wd_len) {
		hal_error_msg("trx hal_get_wd_len");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->hal_fill_wd) {
		hal_error_msg("trx hal_fill_wd");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->get_bulkout_id) {
		hal_error_msg("trx get_bulkout_id");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->handle_rx_buffer) {
		hal_error_msg("trx handle_rx_buffer");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->usb_tx_agg_cfg) {
		hal_error_msg("trx usb_tx_agg_cfg");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->usb_rx_agg_cfg) {
		hal_error_msg("trx usb_rx_agg_cfg");
		status = RTW_HAL_STATUS_FAILURE;
	}
	if (!trx_ops->handle_wp_rpt) {
		hal_error_msg("trx handle_wp_rpt");
		status = RTW_HAL_STATUS_FAILURE;
	}
#endif

#ifdef CONFIG_SDIO_HCI

#endif
	return status;
}
static enum rtw_hal_status hal_set_ops(struct rtw_phl_com_t *phl_com,
						struct hal_info_t *hal_info)
{
	#ifdef CONFIG_PCI_HCI
	if (phl_get_hci_type(phl_com) == RTW_HCI_PCIE)
		hal_set_ops_pci(phl_com, hal_info);
	#endif

	#ifdef CONFIG_USB_HCI
	if (phl_get_hci_type(phl_com) == RTW_HCI_USB)
		hal_set_ops_usb(phl_com, hal_info);
	#endif

	#ifdef CONFIG_SDIO_HCI
	if (phl_get_hci_type(phl_com) == RTW_HCI_SDIO)
		hal_set_ops_sdio(phl_com, hal_info);
	#endif

	return hal_ops_check(hal_info);
}

#ifdef RTW_PHL_BCN
enum rtw_hal_status hal_bcn_init(struct hal_info_t *hal_info)
{
	struct bcn_entry_pool *bcn_pool = &hal_info->hal_com->bcn_pool;

	hal_info->hal_com->bcn_pool.bcn_num = 0;
	INIT_LIST_HEAD(&bcn_pool->bcn_list);
	_os_spinlock_init(hal_to_drvpriv(hal_info), &hal_info->hal_com->bcn_pool.bcn_lock);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status hal_bcn_deinit(struct hal_info_t *hal_info)
{
	void *drv_priv = hal_to_drvpriv(hal_info);
	struct bcn_entry_pool *bcn_pool = &hal_info->hal_com->bcn_pool;
	struct rtw_bcn_entry *tmp_entry, *type = NULL;

	_os_spinlock(drv_priv, &bcn_pool->bcn_lock, _bh, NULL);

	phl_list_for_loop_safe(tmp_entry, type,
		struct rtw_bcn_entry, &bcn_pool->bcn_list, list)
	{
		list_del(&tmp_entry->list);
		_os_mem_free(drv_priv, tmp_entry, sizeof(struct rtw_bcn_entry));
	}

	_os_spinunlock(drv_priv, &bcn_pool->bcn_lock, _bh, NULL);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status hal_alloc_bcn_entry(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal_info,
		struct rtw_bcn_entry **bcn_entry, struct rtw_bcn_info_cmn *bcn_cmn)
{
	void *drv_priv = hal_to_drvpriv(hal_info);
	struct bcn_entry_pool *bcn_pool = &hal_info->hal_com->bcn_pool;
	struct rtw_bcn_entry *new_entry = _os_mem_alloc(drv_priv, sizeof(struct rtw_bcn_entry));

	if(new_entry == NULL)
		return RTW_HAL_STATUS_FAILURE;

	_os_spinlock(drv_priv, &bcn_pool->bcn_lock, _bh, NULL);

	list_add_tail(&new_entry->list, &bcn_pool->bcn_list);
	bcn_pool->bcn_num ++;

	_os_spinunlock(drv_priv, &bcn_pool->bcn_lock, _bh, NULL);

	*bcn_entry = new_entry;

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status hal_free_bcn_entry(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal_info,
		u8 bcn_id)
{
	void *drv_priv = hal_to_drvpriv(hal_info);
	struct bcn_entry_pool *bcn_pool = &hal_info->hal_com->bcn_pool;
	struct rtw_bcn_entry *tmp_entry, *type = NULL;
	u8 is_found = 0;

	_os_spinlock(drv_priv, &bcn_pool->bcn_lock, _bh, NULL);

	phl_list_for_loop_safe(tmp_entry, type,
		struct rtw_bcn_entry, &bcn_pool->bcn_list, list)
	{
		if(tmp_entry->bcn_cmn->bcn_id == bcn_id){
			is_found = 1;
			break;
		}
	}

	_os_spinunlock(drv_priv, &bcn_pool->bcn_lock, _bh, NULL);

	if(is_found){
		list_del(&tmp_entry->list);
		_os_mem_free(drv_priv, tmp_entry, sizeof(struct rtw_bcn_entry));
		bcn_pool->bcn_num --;
		return RTW_HAL_STATUS_SUCCESS;
	}
	else
		return RTW_HAL_STATUS_FAILURE;
}

enum rtw_hal_status hal_get_bcn_entry(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal_info,
		struct rtw_bcn_entry **bcn_entry, u8 bcn_id)
{
	void *drv_priv = hal_to_drvpriv(hal_info);
	struct bcn_entry_pool *bcn_pool = &hal_info->hal_com->bcn_pool;
	struct rtw_bcn_entry *tmp_entry, *type = NULL;
	u8 is_found = 0;

	_os_spinlock(drv_priv, &bcn_pool->bcn_lock, _bh, NULL);

	phl_list_for_loop_safe(tmp_entry, type,
		struct rtw_bcn_entry, &bcn_pool->bcn_list, list)
	{
		if(tmp_entry->bcn_cmn->bcn_id == bcn_id){
			is_found = 1;
			break;
		}
	}

	_os_spinunlock(drv_priv, &bcn_pool->bcn_lock, _bh, NULL);

	if(is_found){
		*bcn_entry = tmp_entry;
		return RTW_HAL_STATUS_SUCCESS;
	}
	else
		return RTW_HAL_STATUS_FAILURE;
}

enum rtw_hal_status hal_update_bcn_entry(struct rtw_phl_com_t *phl_com,
                                         struct hal_info_t *hal_info,
                                         struct rtw_bcn_entry *bcn_entry,
                                         struct rtw_bcn_info_cmn *bcn_cmn)
{
	struct rtw_phl_stainfo_t *phl_sta = NULL;
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 ridx = 0;
	u8 lidx = 0;

	if (bcn_cmn != NULL) {
		bcn_entry->bcn_cmn = bcn_cmn;
		ridx = bcn_cmn->role_idx;
		lidx = bcn_cmn->lidx;

	} else {
		ridx = bcn_entry->bcn_cmn->role_idx;
		lidx = bcn_entry->bcn_cmn->lidx;
	}

	wrole = &phl_com->wifi_roles[ridx];
	rlink = rtw_phl_get_rlink(wrole, lidx);
	phl_sta = rtw_phl_get_stainfo_self(phl_com->phl_priv, rlink);

	bcn_entry->bcn_hw.band = rlink->hw_band;
	bcn_entry->bcn_hw.port = rlink->hw_port;
	bcn_entry->bcn_hw.mbssid = rlink->hw_mbssid;
	bcn_entry->bcn_hw.hiq_win = rlink->hiq_win;
	bcn_entry->bcn_hw.bss_color = rlink->protocol_cap.bsscolor;
	bcn_entry->bcn_hw.mac_id = (u8)phl_sta->macid;

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_add_beacon(struct rtw_phl_com_t *phl_com, void *hal,
		void *bcn_cmn)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	struct rtw_bcn_entry *bcn_entry = NULL;

	if(hal_alloc_bcn_entry(phl_com, hal_info, &bcn_entry, (struct rtw_bcn_info_cmn *)bcn_cmn) == RTW_HAL_STATUS_FAILURE)
		return RTW_HAL_STATUS_FAILURE;

	if(hal_update_bcn_entry(phl_com, hal_info, bcn_entry, (struct rtw_bcn_info_cmn *)bcn_cmn) == RTW_HAL_STATUS_FAILURE)
		return RTW_HAL_STATUS_FAILURE;

	if(hal_ops->cfg_bcn(phl_com, hal_info, bcn_entry) == RTW_HAL_STATUS_FAILURE)
		return RTW_HAL_STATUS_FAILURE;

	if(hal_ops->upt_bcn(phl_com, hal_info, bcn_entry) == RTW_HAL_STATUS_FAILURE)
		return RTW_HAL_STATUS_FAILURE;

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_update_beacon(struct rtw_phl_com_t *phl_com, void *hal,
		u8 bcn_id)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	struct rtw_bcn_entry *bcn_entry = NULL;

	if (hal_get_bcn_entry(phl_com, hal_info, &bcn_entry, bcn_id)
	    == RTW_HAL_STATUS_FAILURE) {
		PHL_ERR("No beacon entry %u to config!\n", bcn_id);
		return RTW_HAL_STATUS_FAILURE;
	}

	if(hal_update_bcn_entry(phl_com, hal_info, bcn_entry, NULL) == RTW_HAL_STATUS_FAILURE)
		return RTW_HAL_STATUS_FAILURE;

	/* Configure beacon in case of beacon timing changed.
	 * ToDo: Apply only when beacon interval and/or DTIM period
	 * are changed.
	 */
	do {
		#ifdef CONFIG_RTW_SUPPORT_MBSSID_VAP
		/* Beacon timing is port-wide setting. Ignore VAP. */
		struct rtw_wifi_role_t *wrole =
			&phl_com->wifi_roles[bcn_entry->bcn_cmn->role_idx];
		if (wrole->type == PHL_RTYPE_VAP)
			break;
		#endif /* CONFIG_RTW_SUPPORT_MBSSID_VAP */
		if (hal_ops->cfg_bcn(phl_com, hal_info, bcn_entry)
		    == RTW_HAL_STATUS_FAILURE) {
			PHL_ERR("Failed to configure beacon entry %u!\n",
				bcn_id);
			return RTW_HAL_STATUS_FAILURE;
		}
	} while (0);

	if (hal_ops->upt_bcn(phl_com, hal_info, bcn_entry)
	    == RTW_HAL_STATUS_FAILURE) {
		PHL_ERR("Failed to update beacon entry %u!\n", bcn_id);
		return RTW_HAL_STATUS_FAILURE;
	}

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_free_beacon(struct rtw_phl_com_t *phl_com, void *hal,
		u8 bcn_id)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	if(hal_free_bcn_entry(phl_com, hal_info, bcn_id) == RTW_HAL_STATUS_FAILURE)
		return RTW_HAL_STATUS_FAILURE;

	return RTW_HAL_STATUS_SUCCESS;
}
#ifdef CONFIG_RTW_DEBUG_BCN_TX
enum rtw_hal_status rtw_hal_get_beacon_cnt(struct rtw_phl_com_t *phl_com, void *hal,
			u8 bcn_id, struct rtw_bcn_stats **bcn_stats)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_bcn_entry *bcn_entry = NULL;

	if (RTW_HAL_STATUS_FAILURE ==
		hal_get_bcn_entry(phl_com, hal_info, &bcn_entry, bcn_id)) {
		PHL_ERR("Failed to get beacon entry %u!\n", bcn_id);
		bcn_stats = NULL;
		return RTW_HAL_STATUS_FAILURE;
	}

	*bcn_stats = &bcn_entry->bcn_stats;
	return RTW_HAL_STATUS_SUCCESS;
}
#endif
#endif

enum rtw_hal_status rtw_hal_pkt_ofld(void *hal, u8 *id, u8 op,
					u8 *pkt_buf, u16 *pkt_len)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_mac_pkt_ofld(hal_info, id, op, pkt_buf, pkt_len);
}

enum rtw_hal_status rtw_hal_pkt_update_ids(void *hal,
						struct pkt_ofld_entry *entry)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_mac_pkt_update_ids(hal_info, entry);
}

enum rtw_hal_status rtw_hal_get_pwr_state(void *hal, enum rtw_mac_pwr_st *pwr_state)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_mac_get_pwr_state(hal_info, pwr_state);
}

enum rtw_hal_status rtw_hal_init(void *drv_priv,
	struct rtw_phl_com_t *phl_com, void **hal, enum rtl_ic_id ic_id)
{
	struct hal_info_t *hal_info = NULL;
	struct rtw_hal_com_t *hal_com = NULL;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	void (*set_intf_ops)(struct rtw_hal_com_t *hal, struct hal_io_ops *ops) = NULL;
	enum rtw_chip_id chip_id = CHIP_WIFI6_MAX;

	if (ic_id == RTL8852A)
		chip_id = CHIP_WIFI6_8852A;
	else if(ic_id == RTL8834A)
		chip_id = CHIP_WIFI6_8834A;
	else if(ic_id == RTL8852B)
		chip_id = CHIP_WIFI6_8852B;
	else if(ic_id == RTL8852BP)
		chip_id = CHIP_WIFI6_8852BP;
	else if(ic_id == RTL8852C)
		chip_id = CHIP_WIFI6_8852C;
	else if(ic_id == RTL8192XB)
		chip_id = CHIP_WIFI6_8192XB;
	else if(ic_id == RTL8832BR)
		chip_id = CHIP_WIFI6_8832BR;
	else if (ic_id == RTL8851B)
		chip_id = CHIP_WIFI6_8851B;
	else
		chip_id = CHIP_WIFI6_MAX;

	hal_info = _os_mem_alloc(drv_priv, sizeof(struct hal_info_t));
	if (hal_info == NULL) {
		hal_status = RTW_HAL_STATUS_RESOURCE;
		PHL_ERR("alloc hal_info failed\n");
		goto error_hal_mem;
	}
	*hal = hal_info;

	hal_com = _os_mem_alloc(drv_priv, sizeof(struct rtw_hal_com_t));
	if (hal_com == NULL) {
		hal_status = RTW_HAL_STATUS_RESOURCE;
		PHL_ERR("alloc hal_com failed\n");
		goto error_hal_com_mem;
	}
	hal_info->phl_com = phl_com;
	hal_info->hal_com = hal_com;
	hal_com->drv_priv = drv_priv;
	hal_com->hal_priv = hal_info;
	hal_com->chip_id = chip_id;
	hal_com->dbcc_en = false;
	#ifdef DBG_HAL_MEM_MOINTOR
	_os_atomic_set(drv_priv, &(hal_com->hal_mem), 0);
	#endif

	/*set io_ops*/
	#ifdef CONFIG_PCI_HCI
	if (phl_com->hci_type == RTW_HCI_PCIE)
		set_intf_ops = hal_pci_set_io_ops;
	#endif

	#ifdef CONFIG_USB_HCI
	if (phl_com->hci_type == RTW_HCI_USB)
		set_intf_ops = hal_usb_set_io_ops;
	#endif

	#ifdef CONFIG_SDIO_HCI
	if (phl_com->hci_type == RTW_HCI_SDIO)
		set_intf_ops = hal_sdio_set_io_ops;
	#endif

	hal_status = hal_init_io_priv(hal_info->hal_com, set_intf_ops);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("hal_init_io_priv failed\n");
		goto error_io_priv;
	}

	hal_status = rtw_hal_mac_init(phl_com, hal_info);
	if ((hal_status != RTW_HAL_STATUS_SUCCESS) || (hal_info->mac == NULL)) {
		PHL_ERR("rtw_hal_mac_init failed\n");
		goto error_mac_init;
	}

	/*set hal_ops and hal_hook_trx_ops*/
	hal_status = hal_set_ops(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("hal_set_ops failed\n");
		goto error_hal_ops;
	}

	hal_status = hal_info->hal_ops.hal_init(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("hal_ops.hal_init failed\n");
		goto error_hal_init;
	}

	hal_status = rtw_hal_efuse_init(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("rtw_hal_efuse_init failed\n");
		goto error_efuse_init;
	}

	hal_status = rtw_hal_bb_init(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("rtw_hal_bb_init failed\n");
		goto error_bb_init;
	}

	hal_status = rtw_hal_rf_init(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("rtw_hal_rf_init failed\n");
		goto error_rf_init;
	}

	hal_status = rtw_hal_btc_init(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS){
		PHL_ERR("rtw_hal_btc_init failed\n");
		goto error_btc_init;
	}

#ifdef RTW_PHL_BCN
	hal_status = hal_bcn_init(hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS){
		PHL_ERR("hal_bcn_init failed\n");
		goto error_bcn_init;
	}
#endif

	return RTW_HAL_STATUS_SUCCESS;

#ifdef RTW_PHL_BCN
error_bcn_init:
	rtw_hal_btc_deinit(phl_com, hal_info);
#endif

error_btc_init:
	rtw_hal_rf_deinit(phl_com, hal_info);

error_rf_init:
	rtw_hal_bb_deinit(phl_com, hal_info);

error_bb_init:
	rtw_hal_efuse_deinit(phl_com, hal_info);

error_efuse_init:
	hal_info->hal_ops.hal_deinit(phl_com, hal_info);

error_hal_init:
error_hal_ops:
	rtw_hal_mac_deinit(phl_com, hal_info);

error_mac_init:
	hal_deinit_io_priv(hal_com);

error_io_priv:
	if (hal_com) {
		_os_mem_free(drv_priv, hal_com, sizeof(struct rtw_hal_com_t));
		hal_com = NULL;
	}

error_hal_com_mem:
	if (hal_info) {
		_os_mem_free(drv_priv, hal_info, sizeof(struct hal_info_t));
		*hal = hal_info = NULL;
	}

error_hal_mem:
	return hal_status;
}
struct rtw_hal_com_t *rtw_hal_get_halcom(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return hal_info->hal_com;
}

void rtw_hal_deinit(struct rtw_phl_com_t *phl_com, void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	void *drv_priv = hal_to_drvpriv(hal_info);

	if(hal_info == NULL)
		return;

	/* stop mechanism / disassociate hal ops */
#ifdef RTW_PHL_BCN
	hal_bcn_deinit(hal_info);
#endif
	rtw_hal_btc_deinit(phl_com, hal_info);
	rtw_hal_rf_deinit(phl_com, hal_info);
	rtw_hal_bb_deinit(phl_com, hal_info);
	rtw_hal_efuse_deinit(phl_com, hal_info);
	rtw_hal_mac_deinit(phl_com, hal_info);
	hal_info->hal_ops.hal_deinit(phl_com, hal_info);
	hal_deinit_io_priv(hal_info->hal_com);

	#ifdef DBG_HAL_MEM_MOINTOR
	PHL_INFO("[PHL-MEM] %s HAL memory :%d\n", __func__,
	_os_atomic_read(hal_to_drvpriv(hal_info), &(hal_info->hal_com->hal_mem)));
	#endif

	if (hal_info->hal_com) {
		if(hal_info->hal_com->bf_obj)
			hal_bf_deinit(hal_info);
		if(hal_info->hal_com->csi_obj)
			hal_csi_deinit(hal_info);
		if(hal_info->hal_com->snd_obj)
			hal_snd_obj_deinit(hal_info);
		_os_mem_free(drv_priv,
			hal_info->hal_com, sizeof(struct rtw_hal_com_t));
		hal_info->hal_com = NULL;
	}
	if (hal_info) {
		_os_mem_free(drv_priv,
			hal_info, sizeof(struct hal_info_t));
		hal_info = NULL;
	}
}

bool rtw_hal_is_inited(struct rtw_phl_com_t *phl_com, void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return hal_info->hal_com->is_hal_init;
}

u32 rtw_hal_hci_cfg(struct rtw_phl_com_t *phl_com, void *hal,
					struct rtw_ic_info *ic_info)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);

#ifdef CONFIG_SDIO_HCI
	hal_info->hal_com->block_sz = ic_info->sdio_info.block_sz;
#endif
	hal_ops->hal_hci_configure(phl_com, hal_info, ic_info);

	return hal_status;
}

u32 rtw_hal_read_chip_info(struct rtw_phl_com_t *phl_com, void *hal)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);

	hal_ops->read_chip_version(phl_com, hal_info);

	/*get mac,bb,rf capability*/
	hal_ops->init_hal_spec(phl_com, hal_info);

	return hal_status;
}

static void _hal_reset_chandef(struct rtw_hal_com_t *hal_com)
{
	struct rtw_chan_def *chandef = NULL;
	u8 bid = 0;

	for (bid = 0; bid < HW_BAND_MAX; bid++) {
		chandef = &(hal_com->band[bid].cur_chandef);
		chandef->bw = CHANNEL_WIDTH_MAX;
		chandef->chan = 0;
		chandef->offset = CHAN_OFFSET_NO_EXT;
	}
}

void rtw_hal_set_default_var(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);

	hal_info->hal_com->assoc_sta_cnt = 0;
	hal_info->hal_com->band[HW_PHY_0].assoc_sta_cnt = 0;
	hal_info->hal_com->band[HW_PHY_0].assoc_sta_cnt = 0;

	_hal_reset_chandef(hal_info->hal_com);

	hal_ops->init_default_value(hal_info);
}

u32 rtw_hal_var_init(struct rtw_phl_com_t *phl_com, void *hal)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	hal_com->is_hal_init = false;
	rtw_hal_set_default_var(hal_info);

	/*csi init shall after read hw chip info*/
	hal_status = hal_csi_init(
			hal_info,
			phl_com->hal_spec.max_csi_buf_su_nr,
			phl_com->hal_spec.max_csi_buf_mu_nr);
	if (hal_status != RTW_HAL_STATUS_SUCCESS){
		PHL_ERR("rtw_hal_csi_init failed\n");
	}

	/*bf init shall after read hw chip info*/
	hal_status = hal_bf_init(
			hal_info,
			phl_com->hal_spec.max_bf_ent_nr,
			phl_com->hal_spec.max_su_sta_nr,
			phl_com->hal_spec.max_mu_sta_nr);
	if (hal_status != RTW_HAL_STATUS_SUCCESS){
		PHL_ERR("rtw_hal_bf_init failed\n");
	}

	hal_status = hal_snd_obj_init(hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS){
		PHL_ERR("hal_snd_obj_init failed\n");
	}
	return hal_status;
}

static int
_hal_parse_macreg(void *drv_priv, u32 *pdest_buf, u8 *psrc_buf, u32 buflen)
{
	return 0;
}

static void _hal_send_hal_init_hub_msg(struct rtw_phl_com_t *phl_com, u8 init_ok)
{
	struct phl_msg msg = {0};
	u16 evt_id = (init_ok) ? MSG_EVT_HAL_INIT_OK : MSG_EVT_HAL_INIT_FAIL;

	msg.inbuf = NULL;
	msg.inlen = 0;
	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_PHY_MGNT);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, evt_id);
	msg.band_idx = HW_BAND_0;
	rtw_phl_msg_hub_hal_send(phl_com, NULL, &msg);
}

enum rtw_hal_status rtw_hal_preload(struct rtw_phl_com_t *phl_com, void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);

	FUNCIN();

	hal_status = hal_ops->hal_get_efuse(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		return hal_status;

	return hal_status;
}

enum rtw_hal_status hal_rfe_type_chk(struct rtw_phl_com_t *phl_com,
				     struct hal_info_t *hal_info)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_UNKNOWN_RFE_TYPE;

	if(phl_com->dev_cap.rfe_type != 0xff){
		hal_status = RTW_HAL_STATUS_SUCCESS;
	}
	else {
		if(phl_com->dev_cap.bypass_rfe_chk == true){
			rtw_hal_rf_get_default_rfe_type(hal_info->hal_com);
			rtw_hal_rf_get_default_xtal(hal_info->hal_com);
			PHL_WARN("%s: Use default RFE type(0x%x) / XTAL(0x%x) configuration for empty EFUSE\n",
				 __FUNCTION__,
				 hal_info->hal_com->dev_hw_cap.rfe_type,
				 hal_info->hal_com->dev_hw_cap.xcap);
			hal_status = RTW_HAL_STATUS_SUCCESS;
		}
	}

	return hal_status;
}

enum rtw_hal_status rtw_hal_start(struct rtw_phl_com_t *phl_com, void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
#ifdef RTW_WKARD_DEF_CMACTBL_CFG
	enum rf_path tx, rx;
#endif
	hal_status = hal_rfe_type_chk(phl_com, hal_info);
	if(hal_status != RTW_HAL_STATUS_SUCCESS){
		PHL_ERR("%s: Unknown RFE type!!!\n", __FUNCTION__);
		return hal_status;
	}

	hal_status = hal_ops->hal_start(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		return hal_status;

	hal_status = RTW_HAL_STATUS_SUCCESS;
	hal_info->hal_com->is_hal_init = true;
	rtw_hal_set_default_var(hal_info);

	rtw_hal_rf_set_power(hal_info, HW_PHY_0, PWR_BY_RATE);
#ifdef RTW_WKARD_DEF_CMACTBL_CFG
	tx = hal_ops->get_path_from_ant_num(phl_com->phy_cap[0].tx_path_num);
	rx = hal_ops->get_path_from_ant_num(phl_com->phy_cap[0].rx_path_num);
	rtw_hal_bb_trx_path_cfg(hal_info, tx, phl_com->phy_cap[0].txss,
		rx, phl_com->phy_cap[0].rxss);
#endif
#ifdef RTW_WKARD_SINGLE_PATH_RSSI
	hal_info->hal_com->cur_rx_rfpath =
		hal_ops->get_path_from_ant_num(phl_com->phy_cap[0].rx_path_num);
#endif
	#ifdef CONFIG_BTCOEX
	rtw_hal_btc_radio_state_ntfy(hal_info, true);
	#endif

	return hal_status;
}

void rtw_hal_stop(struct rtw_phl_com_t *phl_com, void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);

	do {
		if (true == TEST_STATUS_FLAG(phl_com->dev_state,
		                             RTW_DEV_SURPRISE_REMOVAL)) {
			PHL_WARN("%s(): Device has removed, skip HW stop functions!\n", __func__);
			break;
 		}

#ifdef CONFIG_BTCOEX
		rtw_hal_btc_radio_state_ntfy(hal_info, false);
#endif
		hal_status = hal_ops->hal_stop(phl_com, hal_info);
		hal_info->hal_com->is_hal_init = false;
	} while (false);
}

enum rtw_hal_status rtw_hal_restart(struct rtw_phl_com_t *phl_com, void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);

	#ifdef CONFIG_BTCOEX
	rtw_hal_btc_radio_state_ntfy(hal_info, false);
	#endif

	hal_status = hal_ops->hal_stop(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		return hal_status;

	hal_status = hal_ops->hal_start(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		return hal_status;

	#ifdef CONFIG_BTCOEX
	rtw_hal_btc_radio_state_ntfy(hal_info, true);
	#endif

	hal_info->hal_com->is_hal_init = true;

	return hal_status;
}

enum rtw_hal_status rtw_hal_hal_deinit(struct rtw_phl_com_t *phl_com, void *hal)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	hstatus = rtw_hal_mac_hal_deinit(phl_com, hal_info);

	return hstatus;
}

enum rtw_hal_status
rtw_hal_role_deinit(void *hal, struct rtw_wifi_role_link_t *rlink)
{
	/* hw port cfg - mac_port_deinit */
	return rtw_hal_mac_port_deinit(hal, rlink);
}

enum rtw_hal_status
rtw_hal_role_cfg(void *hal,
                 struct rtw_wifi_role_t *wrole,
                 struct rtw_wifi_role_link_t *rlink)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	#ifdef CONFIG_RTW_SUPPORT_MBSSID_VAP
	if (wrole->type == PHL_RTYPE_VAP) {
		u32	param = 1;

		/* Configure port for M-BSSID */
		PHL_INFO("Config role #%u rlink #%u of type %u. MBSSID: %u.\n",
	          wrole->id, rlink->id, wrole->type, rlink->hw_mbssid);

		return rtw_hal_mac_port_cfg(hal_info, rlink,
					    PCFG_MBSSID_EN, &param);
	}
	#endif /* CONFIG_RTW_SUPPORT_MBSSID_VAP */

	/*enable & configure mac hw-port*/
	hal_status = rtw_hal_mac_port_init(hal_info, wrole, rlink);

	/* init hal mac bfer function if wrole cap if any of bfer cap is true*/
	if (rlink->protocol_cap.he_su_bfmr ||
	    rlink->protocol_cap.he_mu_bfmr ||
	    rlink->protocol_cap.vht_su_bfmr ||
	    rlink->protocol_cap.vht_mu_bfmr ||
	    rlink->protocol_cap.ht_su_bfmr) {
		if ((rtw_phl_role_is_ap_category(wrole)) ||
		    ((rtw_phl_role_is_client_category(wrole)) &&
		     (MLME_LINKED == rlink->mstate))) {
			hal_status = hal_bf_hw_mac_init_bfer(hal_info,
							     rlink->hw_band);
		}
	}
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return hal_status;
}

enum rtw_hal_status
rtw_hal_role_cfg_ex(void *hal,
                    struct rtw_wifi_role_link_t *rlink,
                    enum pcfg_type type,
                    void *param)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	hal_status = rtw_hal_mac_port_cfg(hal_info, rlink, type, param);
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return hal_status;
}

enum rtw_hal_status
rtw_hal_beacon_stop(void *hal,
                    struct rtw_wifi_role_link_t *rlink,
                    bool stop)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;
	u32 bcn_en = (stop) ? 0 : 1;

	PHL_INFO("%s wr-%d, rlink:%d, bcn_en:%s\n", __func__, rlink->wrole->id, rlink->id, (bcn_en) ? "E" : "D");
	hsts = rtw_hal_mac_port_cfg(hal_info, rlink, PCFG_BCN_EN, &bcn_en);

	return hsts;
}

enum rtw_hal_status
hal_ver_check(struct rtw_hal_com_t *hal_com)
{
	if ((hal_com->mac_vc.mac_ver < hal_com->bb_vc.mac_ver) ||
		(hal_com->mac_vc.mac_ver < hal_com->rf_vc.mac_ver) ||
		(hal_com->mac_vc.mac_ver < hal_com->btc_vc.mac_ver) ||
		(hal_com->mac_vc.mac_ver < hal_com->fw_vc.mac_ver))
		return RTW_HAL_STATUS_FAILURE;
	return RTW_HAL_STATUS_SUCCESS;
}


enum rtw_hal_status
rtw_hal_watchdog(void *hal, struct rtw_phl_com_t *phl_com)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_bb_watchdog(hal_info, false);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s rtw_hal_bb_watchdog fail (%x)\n", __FUNCTION__, hal_status);
		goto exit;
	}

	hal_status = rtw_hal_rf_watchdog(hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s rtw_hal_rf_watchdog fail (%x)\n", __FUNCTION__, hal_status);
		goto exit;
	}

	hal_status = rtw_hal_mac_watchdog(hal_info, phl_com);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s rtw_hal_mac_watchdog fail (%x)\n", __FUNCTION__, hal_status);
		goto exit;
	}

exit:
	return hal_status;
}

enum rtw_hal_status
rtw_hal_simple_watchdog(void *hal, u8 io_en)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_bb_simple_watchdog(hal_info, io_en);
	if(hal_status != RTW_HAL_STATUS_SUCCESS){
		PHL_INFO("%s fail (%x)\n",
			 __FUNCTION__, hal_status);
		goto exit;
	}

exit:
	return hal_status;
}


enum rtw_hal_status
rtw_hal_cfg_trx_path(void *hal, enum rf_path tx, u8 tx_nss,
		     enum rf_path rx, u8 rx_nss)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_phl_com_t *phl_com = hal_info->phl_com;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	if (tx < RF_PATH_AB) {
		/* forced tx nss = 1*/
		tx_nss = 1;
	}
	if (rx < RF_PATH_AB) {
		/* forced rx nss = 1*/
		rx_nss = 1;
	}

	hal_status = rtw_hal_bb_trx_path_cfg(
			hal_info,
			tx,
			((tx_nss > phl_com->phy_cap[0].tx_path_num) ?
			  phl_com->phy_cap[0].tx_path_num : tx_nss),
			rx,
			((rx_nss > phl_com->phy_cap[0].rx_path_num) ?
			  phl_com->phy_cap[0].rx_path_num : rx_nss));
#ifdef RTW_WKARD_SINGLE_PATH_RSSI
	hal_info->hal_com->cur_rx_rfpath = rx;
#endif

	return hal_status;
}

enum rtw_hal_status rtw_hal_set_sw_gpio_mode(struct rtw_phl_com_t *phl_com, void *hal, enum rtw_gpio_mode mode, u8 gpio)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mac_set_sw_gpio_mode(hal_info, mode, gpio);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "%s : status(%u).\n", __func__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_sw_gpio_ctrl(struct rtw_phl_com_t *phl_com, void *hal, u8 high, u8 gpio)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mac_sw_gpio_ctrl(hal_info, high, gpio);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "%s : status(%u).\n", __func__, hal_status);

	return hal_status;
}

void rtw_hal_dbg_status_dump(void *hal, struct hal_mac_dbg_dump_cfg *cfg)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	rtw_hal_mac_dbg_status_dump(hal_info, cfg);
}

