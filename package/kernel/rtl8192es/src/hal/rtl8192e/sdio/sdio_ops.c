/******************************************************************************
 *
 * Copyright(c) 2012 - 2017 Realtek Corporation.
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
#define _SDIO_OPS_C_

#include <rtl8192e_hal.h>

/* #define SDIO_DEBUG_IO 1 */

/*
 * Description:
 *	The following mapping is for SDIO host local register space.
 *
 * Creadted by Roger, 2011.01.31.
 *   */
static void HalSdioGetCmdAddr8192ESdio(
	IN	PADAPTER			padapter,
	IN	u8				DeviceID,
	IN	u32				Addr,
	OUT	u32				*pCmdAddr
)
{
	switch (DeviceID) {
	case SDIO_LOCAL_DEVICE_ID:
		*pCmdAddr = ((SDIO_LOCAL_DEVICE_ID << 13) | (Addr & SDIO_LOCAL_MSK));
		break;

	case WLAN_IOREG_DEVICE_ID:
		*pCmdAddr = ((WLAN_IOREG_DEVICE_ID << 13) | (Addr & WLAN_IOREG_MSK));
		break;

	case WLAN_TX_HIQ_DEVICE_ID:
		*pCmdAddr = ((WLAN_TX_HIQ_DEVICE_ID << 13) | (Addr & WLAN_FIFO_MSK));
		break;

	case WLAN_TX_MIQ_DEVICE_ID:
		*pCmdAddr = ((WLAN_TX_MIQ_DEVICE_ID << 13) | (Addr & WLAN_FIFO_MSK));
		break;

	case WLAN_TX_LOQ_DEVICE_ID:
		*pCmdAddr = ((WLAN_TX_LOQ_DEVICE_ID << 13) | (Addr & WLAN_FIFO_MSK));
		break;

	case WLAN_RX0FF_DEVICE_ID:
		*pCmdAddr = ((WLAN_RX0FF_DEVICE_ID << 13) | (Addr & WLAN_RX0FF_MSK));
		break;

	default:
		break;
	}
}

static u8 get_deviceid(u32 addr)
{
	u8 devideId;
	u16 pseudoId;


	pseudoId = (u16)(addr >> 16);
	switch (pseudoId) {
	case 0x1025:
		devideId = SDIO_LOCAL_DEVICE_ID;
		break;

	case 0x1026:
		devideId = WLAN_IOREG_DEVICE_ID;
		break;

	/*		case 0x1027:
	 *			devideId = SDIO_FIRMWARE_FIFO;
	 *			break; */

	case 0x1031:
		devideId = WLAN_TX_HIQ_DEVICE_ID;
		break;

	case 0x1032:
		devideId = WLAN_TX_MIQ_DEVICE_ID;
		break;

	case 0x1033:
		devideId = WLAN_TX_LOQ_DEVICE_ID;
		break;

	case 0x1034:
		devideId = WLAN_RX0FF_DEVICE_ID;
		break;

	default:
		/*			devideId = (u8)((addr >> 13) & 0xF); */
		devideId = WLAN_IOREG_DEVICE_ID;
		break;
	}

	return devideId;
}

/*
 * Ref:
 *	HalSdioGetCmdAddr8723ASdio()
 */
static u32 _cvrt2ftaddr(const u32 addr, u8 *pdeviceId, u16 *poffset)
{
	u8 deviceId;
	u16 offset;
	u32 ftaddr;


	deviceId = get_deviceid(addr);
	offset = 0;

	switch (deviceId) {
	case SDIO_LOCAL_DEVICE_ID:
		offset = addr & SDIO_LOCAL_MSK;
		break;

	case WLAN_TX_HIQ_DEVICE_ID:
	case WLAN_TX_MIQ_DEVICE_ID:
	case WLAN_TX_LOQ_DEVICE_ID:
		offset = addr & WLAN_FIFO_MSK;
		break;

	case WLAN_RX0FF_DEVICE_ID:
		offset = addr & WLAN_RX0FF_MSK;
		break;

	case WLAN_IOREG_DEVICE_ID:
	default:
		deviceId = WLAN_IOREG_DEVICE_ID;
		offset = addr & WLAN_IOREG_MSK;
		break;
	}
	ftaddr = (deviceId << 13) | offset;

	if (pdeviceId)
		*pdeviceId = deviceId;
	if (poffset)
		*poffset = offset;

	return ftaddr;
}

u8 _sdio_read8(PADAPTER padapter, u32 addr)
{
	struct intf_hdl *pintfhdl;
	u32 ftaddr;
	u8 val;


	/* psdiodev = pintfhdl->pintf_dev; */
	/* psdio = &psdiodev->intf_data; */

	pintfhdl = &padapter->iopriv.intf;

	ftaddr = _cvrt2ftaddr(addr, NULL, NULL);
	val = _sd_read8(pintfhdl, ftaddr, NULL);


	return val;
}

u8 sdio_read8(struct intf_hdl *pintfhdl, u32 addr)
{
	u32 ftaddr;
	u8 val;

	ftaddr = _cvrt2ftaddr(addr, NULL, NULL);
	val = sd_read8(pintfhdl, ftaddr, NULL);


	return val;
}

u16 sdio_read16(struct intf_hdl *pintfhdl, u32 addr)
{
	u32 ftaddr;
	u16 val;


	ftaddr = _cvrt2ftaddr(addr, NULL, NULL);
	sd_cmd52_read(pintfhdl, ftaddr, 2, (u8 *)&val);
	val = le16_to_cpu(val);


	return val;
}

u32 _sdio_read32(PADAPTER padapter, u32 addr)
{
	/* PADAPTER padapter; */

	struct intf_hdl *pintfhdl;
	u8 bMacPwrCtrlOn;
	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	u32 val;
	s32 err;


	/* padapter = pintfhdl->padapter; */
	/* psdiodev = pintfhdl->pintf_dev; */
	pintfhdl = &padapter->iopriv.intf;

	ftaddr = _cvrt2ftaddr(addr, &deviceId, &offset);

	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (((deviceId == WLAN_IOREG_DEVICE_ID) && (offset < 0x100))
	    || (_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
	    || (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode)
#endif
	   ) {
		err = _sd_cmd52_read(pintfhdl, ftaddr, 4, (u8 *)&val);
#ifdef SDIO_DEBUG_IO
		if (!err) {
#endif
			val = le32_to_cpu(val);
			return val;
#ifdef SDIO_DEBUG_IO
		}

		RTW_ERR("%s: Mac Power off, Read FAIL(%d)! addr=0x%x\n", __func__, err, addr);
		return SDIO_ERR_VAL32;
#endif
	}

	/* 4 bytes alignment */
	shift = ftaddr & 0x3;
	if (shift == 0)
		val = _sd_read32(pintfhdl, ftaddr, NULL);
	else {
		u8 *ptmpbuf;

		ptmpbuf = (u8 *)rtw_malloc(8);
		if (NULL == ptmpbuf) {
			RTW_ERR("%s: Allocate memory FAIL!(size=8) addr=0x%x\n", __func__, addr);
			return SDIO_ERR_VAL32;
		}

		ftaddr &= ~(u16)0x3;
		_sd_read(pintfhdl, ftaddr, 8, ptmpbuf);
		_rtw_memcpy(&val, ptmpbuf + shift, 4);
		val = le32_to_cpu(val);

		rtw_mfree(ptmpbuf, 8);
	}


	return val;
}

u32 sdio_read32(struct intf_hdl *pintfhdl, u32 addr)
{
	PADAPTER padapter;
	u8 bMacPwrCtrlOn;
	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	u32 val;
	s32 err;


	padapter = pintfhdl->padapter;

	ftaddr = _cvrt2ftaddr(addr, &deviceId, &offset);

	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (((deviceId == WLAN_IOREG_DEVICE_ID) && (offset < 0x100))
	    || (_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
	    || (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode)
#endif
	   ) {
		err = sd_cmd52_read(pintfhdl, ftaddr, 4, (u8 *)&val);
#ifdef SDIO_DEBUG_IO
		if (!err) {
#endif
			val = le32_to_cpu(val);
			return val;
#ifdef SDIO_DEBUG_IO
		}

		RTW_ERR("%s: Mac Power off, Read FAIL(%d)! addr=0x%x\n", __func__, err, addr);
		return SDIO_ERR_VAL32;
#endif
	}

	/* 4 bytes alignment */
	shift = ftaddr & 0x3;
	if (shift == 0)
		val = sd_read32(pintfhdl, ftaddr, NULL);
	else {
		u8 *ptmpbuf;

		ptmpbuf = (u8 *)rtw_malloc(8);
		if (NULL == ptmpbuf) {
			RTW_ERR("%s: Allocate memory FAIL!(size=8) addr=0x%x\n", __func__, addr);
			return SDIO_ERR_VAL32;
		}

		ftaddr &= ~(u16)0x3;
		sd_read(pintfhdl, ftaddr, 8, ptmpbuf);
		_rtw_memcpy(&val, ptmpbuf + shift, 4);
		val = le32_to_cpu(val);

		rtw_mfree(ptmpbuf, 8);
	}


	return val;
}

s32 sdio_readN(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *pbuf)
{
	PADAPTER padapter;
	u8 bMacPwrCtrlOn;
	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	s32 err;


	padapter = pintfhdl->padapter;
	err = 0;

	ftaddr = _cvrt2ftaddr(addr, &deviceId, &offset);

	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (((deviceId == WLAN_IOREG_DEVICE_ID) && (offset < 0x100))
	    || (_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
	    || (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode)
#endif
	   ) {
		err = sd_cmd52_read(pintfhdl, ftaddr, cnt, pbuf);
		return err;
	}

	/* 4 bytes alignment */
	shift = ftaddr & 0x3;
	if (shift == 0)
		err = sd_read(pintfhdl, ftaddr, cnt, pbuf);
	else {
		u8 *ptmpbuf;
		u32 n;

		ftaddr &= ~(u16)0x3;
		n = cnt + shift;
		ptmpbuf = rtw_malloc(n);
		if (NULL == ptmpbuf)
			return -1;
		err = sd_read(pintfhdl, ftaddr, n, ptmpbuf);
		if (!err)
			_rtw_memcpy(pbuf, ptmpbuf + shift, cnt);
		rtw_mfree(ptmpbuf, n);
	}


	return err;
}

s32 sdio_write8(struct intf_hdl *pintfhdl, u32 addr, u8 val)
{
	u32 ftaddr;
	s32 err;

	ftaddr = _cvrt2ftaddr(addr, NULL, NULL);
	sd_write8(pintfhdl, ftaddr, val, &err);


	return err;
}

s32 sdio_write16(struct intf_hdl *pintfhdl, u32 addr, u16 val)
{
	u32 ftaddr;
	u8 shift;
	s32 err;


	ftaddr = _cvrt2ftaddr(addr, NULL, NULL);
	val = cpu_to_le16(val);
	err = sd_cmd52_write(pintfhdl, ftaddr, 2, (u8 *)&val);


	return err;
}

s32 _sdio_write32(PADAPTER padapter, u32 addr, u32 val)
{
	/* PADAPTER padapter; */
	struct intf_hdl *pintfhdl;
	u8 bMacPwrCtrlOn;
	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	s32 err;


	/* padapter = pintfhdl->padapter; */
	/* psdiodev = pintfhdl->pintf_dev; */
	pintfhdl = &padapter->iopriv.intf;
	err = 0;

	ftaddr = _cvrt2ftaddr(addr, &deviceId, &offset);

	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (((deviceId == WLAN_IOREG_DEVICE_ID) && (offset < 0x100))
	    || (_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
	    || (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode)
#endif
	   ) {
		val = cpu_to_le32(val);
		err = _sd_cmd52_write(pintfhdl, ftaddr, 4, (u8 *)&val);
		return err;
	}

	/* 4 bytes alignment */
	shift = ftaddr & 0x3;
#if 1
	if (shift == 0)
		_sd_write32(pintfhdl, ftaddr, val, &err);
	else {
		val = cpu_to_le32(val);
		err = _sd_cmd52_write(pintfhdl, ftaddr, 4, (u8 *)&val);
	}
#else
	if (shift == 0)
		sd_write32(pintfhdl, ftaddr, val, &err);
	else {
		u8 *ptmpbuf;

		ptmpbuf = (u8 *)rtw_malloc(8);
		if (NULL == ptmpbuf)
			return -1;

		ftaddr &= ~(u16)0x3;
		err = sd_read(pintfhdl, ftaddr, 8, ptmpbuf);
		if (err) {
			rtw_mfree(ptmpbuf, 8);
			return err;
		}
		val = cpu_to_le32(val);
		_rtw_memcpy(ptmpbuf + shift, &val, 4);
		err = sd_write(pintfhdl, ftaddr, 8, ptmpbuf);

		rtw_mfree(ptmpbuf, 8);
	}
#endif


	return err;
}


s32 sdio_write32(struct intf_hdl *pintfhdl, u32 addr, u32 val)
{
	PADAPTER padapter;
	u8 bMacPwrCtrlOn;
	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	s32 err;


	padapter = pintfhdl->padapter;
	err = 0;

	ftaddr = _cvrt2ftaddr(addr, &deviceId, &offset);

	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (((deviceId == WLAN_IOREG_DEVICE_ID) && (offset < 0x100))
	    || (_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
	    || (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode)
#endif
	   ) {
		val = cpu_to_le32(val);
		err = sd_cmd52_write(pintfhdl, ftaddr, 4, (u8 *)&val);
		return err;
	}

	/* 4 bytes alignment */
	shift = ftaddr & 0x3;
#if 1
	if (shift == 0)
		sd_write32(pintfhdl, ftaddr, val, &err);
	else {
		val = cpu_to_le32(val);
		err = sd_cmd52_write(pintfhdl, ftaddr, 4, (u8 *)&val);
	}
#else
	if (shift == 0)
		sd_write32(pintfhdl, ftaddr, val, &err);
	else {
		u8 *ptmpbuf;

		ptmpbuf = (u8 *)rtw_malloc(8);
		if (NULL == ptmpbuf)
			return -1;

		ftaddr &= ~(u16)0x3;
		err = sd_read(pintfhdl, ftaddr, 8, ptmpbuf);
		if (err) {
			rtw_mfree(ptmpbuf, 8);
			return err;
		}
		val = cpu_to_le32(val);
		_rtw_memcpy(ptmpbuf + shift, &val, 4);
		err = sd_write(pintfhdl, ftaddr, 8, ptmpbuf);

		rtw_mfree(ptmpbuf, 8);
	}
#endif


	return err;
}

s32 sdio_writeN(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *pbuf)
{
	PADAPTER padapter;
	u8 bMacPwrCtrlOn;
	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	s32 err;


	padapter = pintfhdl->padapter;
	err = 0;

	ftaddr = _cvrt2ftaddr(addr, &deviceId, &offset);

	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (((deviceId == WLAN_IOREG_DEVICE_ID) && (offset < 0x100))
	    || (_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
	    || (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode)
#endif
	   ) {
		err = sd_cmd52_write(pintfhdl, ftaddr, cnt, pbuf);
		return err;
	}

	shift = ftaddr & 0x3;
	if (shift == 0)
		err = sd_write(pintfhdl, ftaddr, cnt, pbuf);
	else {
		u8 *ptmpbuf;
		u32 n;

		ftaddr &= ~(u16)0x3;
		n = cnt + shift;
		ptmpbuf = rtw_malloc(n);
		if (NULL == ptmpbuf)
			return -1;
		err = sd_read(pintfhdl, ftaddr, 4, ptmpbuf);
		if (err) {
			rtw_mfree(ptmpbuf, n);
			return err;
		}
		_rtw_memcpy(ptmpbuf + shift, pbuf, cnt);
		err = sd_write(pintfhdl, ftaddr, n, ptmpbuf);
		rtw_mfree(ptmpbuf, n);
	}


	return err;
}

u8 sdio_f0_read8(struct intf_hdl *pintfhdl, u32 addr)
{
	u32 ftaddr;
	u8 val;

	val = sd_f0_read8(pintfhdl, addr, NULL);


	return val;
}

void sdio_read_mem(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *rmem)
{
	s32 err;


	err = sdio_readN(pintfhdl, addr, cnt, rmem);

}

void sdio_write_mem(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *wmem)
{

	sdio_writeN(pintfhdl, addr, cnt, wmem);

}

/*
 * Description:
 *	Read from RX FIFO
 *	Round read size to block size,
 *	and make sure data transfer will be done in one command.
 *
 * Parameters:
 *	pintfhdl	a pointer of intf_hdl
 *	addr		port ID
 *	cnt			size to read
 *	rmem		address to put data
 *
 * Return:
 *	_SUCCESS(1)		Success
 *	_FAIL(0)		Fail
 */
static u32 sdio_read_port(
	struct intf_hdl *pintfhdl,
	u32 addr,
	u32 cnt,
	u8 *mem)
{
	PADAPTER padapter = pintfhdl->padapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	s32 err;

	HalSdioGetCmdAddr8192ESdio(padapter, addr, pHalData->SdioRxFIFOCnt++, &addr);


	cnt = _RND4(cnt);
	cnt = rtw_sdio_cmd53_align_size(adapter_to_dvobj(padapter), cnt);

	err = _sd_read(pintfhdl, addr, cnt, mem);
	/* err = sd_read(pintfhdl, addr, cnt, mem); */



	if (err)
		return _FAIL;
	return _SUCCESS;
}

/*
 * Description:
 *	Write to TX FIFO
 *	Align write size block size,
 *	and make sure data could be written in one command.
 *
 * Parameters:
 *	pintfhdl	a pointer of intf_hdl
 *	addr		port ID
 *	cnt			size to write
 *	wmem		data pointer to write
 *
 * Return:
 *	_SUCCESS(1)		Success
 *	_FAIL(0)		Fail
 */
static u32 sdio_write_port(
	struct intf_hdl *pintfhdl,
	u32 addr,
	u32 cnt,
	u8 *mem)
{
	PADAPTER padapter;
	s32 err;
	struct xmit_buf *xmitbuf = (struct xmit_buf *)mem;

	padapter = pintfhdl->padapter;

	if (!rtw_is_hw_init_completed(padapter)) {
		RTW_INFO("%s [addr=0x%x cnt=%d] padapter->hw_init_completed == _FALSE\n", __func__, addr, cnt);
		return _FAIL;
	}

	cnt = _RND4(cnt);
	HalSdioGetCmdAddr8192ESdio(padapter, addr, cnt >> 2, &addr);

	cnt = rtw_sdio_cmd53_align_size(adapter_to_dvobj(padapter), cnt);

	err = sd_write(pintfhdl, addr, cnt, xmitbuf->pdata);

	rtw_sctx_done_err(&xmitbuf->sctx,
		  err ? RTW_SCTX_DONE_WRITE_PORT_ERR : RTW_SCTX_DONE_SUCCESS);

	if (err) {
		RTW_INFO("%s, error=%d\n", __func__, err);

		return _FAIL;
	}
	return _SUCCESS;
}

void sdio_set_intf_ops(_adapter *padapter, struct _io_ops *pops)
{

	pops->_read8 = &sdio_read8;
	pops->_read16 = &sdio_read16;
	pops->_read32 = &sdio_read32;
	pops->_read_mem = &sdio_read_mem;
	pops->_read_port = &sdio_read_port;

	pops->_write8 = &sdio_write8;
	pops->_write16 = &sdio_write16;
	pops->_write32 = &sdio_write32;
	pops->_writeN = &sdio_writeN;
	pops->_write_mem = &sdio_write_mem;
	pops->_write_port = &sdio_write_port;

	pops->_sd_f0_read8 = sdio_f0_read8;

}

/*
 * Todo: align address to 4 bytes.
 */
s32 _sdio_local_read(
	PADAPTER	padapter,
	u32		addr,
	u32		cnt,
	u8		*pbuf)
{
	struct intf_hdl *pintfhdl;
	u8 bMacPwrCtrlOn;
	s32 err;
	u8 *ptmpbuf;
	u32 n;

	pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192ESdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);

	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if ((_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
	    /*		|| (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode) */
#endif
	   ) {
		err = _sd_cmd52_read(pintfhdl, addr, cnt, pbuf);
		return err;
	}

	n = RND4(cnt);
	ptmpbuf = (u8 *)rtw_malloc(n);
	if (!ptmpbuf)
		return -1;

	err = _sd_read(pintfhdl, addr, n, ptmpbuf);
	if (!err)
		_rtw_memcpy(pbuf, ptmpbuf, cnt);

	if (ptmpbuf)
		rtw_mfree(ptmpbuf, n);

	return err;
}

/*
 * Todo: align address to 4 bytes.
 */
s32 sdio_local_read(
	PADAPTER	padapter,
	u32		addr,
	u32		cnt,
	u8		*pbuf)
{
	struct intf_hdl *pintfhdl;
	u8 bMacPwrCtrlOn;
	s32 err;
	u8 *ptmpbuf;
	u32 n;

	pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192ESdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);

	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if ((_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
	    || (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode)
#endif
	   ) {
		err = sd_cmd52_read(pintfhdl, addr, cnt, pbuf);
		return err;
	}

	n = RND4(cnt);
	ptmpbuf = (u8 *)rtw_malloc(n);
	if (!ptmpbuf)
		return -1;

	err = sd_read(pintfhdl, addr, n, ptmpbuf);
	if (!err)
		_rtw_memcpy(pbuf, ptmpbuf, cnt);

	if (ptmpbuf)
		rtw_mfree(ptmpbuf, n);

	return err;
}

/*
 * Todo: align address to 4 bytes.
 */
s32 _sdio_local_write(
	PADAPTER	padapter,
	u32		addr,
	u32		cnt,
	u8		*pbuf)
{
	struct intf_hdl *pintfhdl;
	u8 bMacPwrCtrlOn;
	s32 err;
	u8 *ptmpbuf;

	if (addr & 0x3)
		RTW_INFO("%s, address must be 4 bytes alignment\n", __FUNCTION__);

	if (cnt  & 0x3)
		RTW_INFO("%s, size must be the multiple of 4\n", __FUNCTION__);

	pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192ESdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);

	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if ((_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
	    /*		|| (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode) */
#endif
	   ) {
		err = _sd_cmd52_write(pintfhdl, addr, cnt, pbuf);
		return err;
	}

	ptmpbuf = (u8 *)rtw_malloc(cnt);
	if (!ptmpbuf)
		return -1;

	_rtw_memcpy(ptmpbuf, pbuf, cnt);

	err = _sd_write(pintfhdl, addr, cnt, ptmpbuf);

	if (ptmpbuf)
		rtw_mfree(ptmpbuf, cnt);

	return err;
}

/*
 * Todo: align address to 4 bytes.
 */
s32 sdio_local_write(
	PADAPTER	padapter,
	u32		addr,
	u32		cnt,
	u8		*pbuf)
{

	struct intf_hdl *pintfhdl;
	u8 bMacPwrCtrlOn;
	s32 err;
	u8 *ptmpbuf;

	if (addr & 0x3)
		RTW_INFO("%s, address must be 4 bytes alignment\n", __FUNCTION__);

	if (cnt  & 0x3)
		RTW_INFO("%s, size must be the multiple of 4\n", __FUNCTION__);

	pintfhdl = &padapter->iopriv.intf;
	HalSdioGetCmdAddr8192ESdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);

	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if ((_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
	    || (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode)
#endif
	   ) {
		err = sd_cmd52_write(pintfhdl, addr, cnt, pbuf);
		return err;
	}

	ptmpbuf = (u8 *)rtw_malloc(cnt);
	if (!ptmpbuf)
		return -1;

	_rtw_memcpy(ptmpbuf, pbuf, cnt);

	err = sd_write(pintfhdl, addr, cnt, ptmpbuf);

	if (ptmpbuf)
		rtw_mfree(ptmpbuf, cnt);

	return err;
}

u8 SdioLocalCmd52Read1Byte(PADAPTER padapter, u32 addr)
{
	struct intf_hdl *pintfhdl;
	u8 val = 0;

	pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192ESdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	sd_cmd52_read(pintfhdl, addr, 1, &val);

	return val;
}

u16 SdioLocalCmd52Read2Byte(PADAPTER padapter, u32 addr)
{
	struct intf_hdl *pintfhdl;
	u16 val = 0;

	pintfhdl = &padapter->iopriv.intf;
	HalSdioGetCmdAddr8192ESdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	sd_cmd52_read(pintfhdl, addr, 2, (u8 *)&val);

	val = le16_to_cpu(val);

	return val;
}

u32 SdioLocalCmd52Read4Byte(PADAPTER padapter, u32 addr)
{
	struct intf_hdl *pintfhdl;
	u32 val = 0;


	pintfhdl = &padapter->iopriv.intf;
	HalSdioGetCmdAddr8192ESdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	sd_cmd52_read(pintfhdl, addr, 4, (u8 *)&val);

	val = le32_to_cpu(val);

	return val;
}

u32 SdioLocalCmd53Read4Byte(PADAPTER padapter, u32 addr)
{
	struct intf_hdl *pintfhdl;
	u8 bMacPwrCtrlOn;
	u32 val = 0;

	pintfhdl = &padapter->iopriv.intf;
	HalSdioGetCmdAddr8192ESdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if ((_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
	    || (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode)
#endif
	   ) {
		sd_cmd52_read(pintfhdl, addr, 4, (u8 *)&val);
		val = le32_to_cpu(val);
	} else
		val = sd_read32(pintfhdl, addr, NULL);

	return val;
}

void SdioLocalCmd52Write1Byte(PADAPTER padapter, u32 addr, u8 v)
{
	struct intf_hdl *pintfhdl;

	pintfhdl = &padapter->iopriv.intf;
	HalSdioGetCmdAddr8192ESdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	sd_cmd52_write(pintfhdl, addr, 1, &v);
}

void SdioLocalCmd52Write2Byte(PADAPTER padapter, u32 addr, u16 v)
{
	struct intf_hdl *pintfhdl;

	pintfhdl = &padapter->iopriv.intf;
	HalSdioGetCmdAddr8192ESdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	v = cpu_to_le16(v);
	sd_cmd52_write(pintfhdl, addr, 2, (u8 *)&v);
}

void SdioLocalCmd52Write4Byte(PADAPTER padapter, u32 addr, u32 v)
{
	struct intf_hdl *pintfhdl;

	pintfhdl = &padapter->iopriv.intf;
	HalSdioGetCmdAddr8192ESdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	v = cpu_to_le32(v);
	sd_cmd52_write(pintfhdl, addr, 4, (u8 *)&v);
}

static s32 ReadInterrupt8192ESdio(PADAPTER padapter, u32 *phisr)
{
	u32 hisr, himr;
	u8 val8, hisr_len;
	int i;

	if (phisr == NULL)
		return _FALSE;

	himr = GET_HAL_DATA(padapter)->sdio_himr;

	/* decide how many bytes need to be read */
	hisr_len = 0;
	while (himr) {
		hisr_len++;
		himr >>= 8;
	}

	hisr = 0;
	for (i = 0; i < hisr_len; i++) {
		val8 = SdioLocalCmd52Read1Byte(padapter, SDIO_REG_HISR + i);
		hisr |= (val8 << (8 * i));
	}

	*phisr = hisr;

	return _TRUE;
}


/*
 *	Description:
 *		Initialize SDIO Host Interrupt Mask configuration variables for future use.
 *
 *	Assumption:
 *		Using SDIO Local register ONLY for configuration.
 *
 *	Created by Roger, 2011.02.11.
 *   */
void InitInterrupt8192ESdio(PADAPTER padapter)
{
	HAL_DATA_TYPE *pHalData;


	pHalData = GET_HAL_DATA(padapter);
	pHalData->sdio_himr = (u32)(\
				    SDIO_HIMR_RX_REQUEST_MSK			|
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
				    SDIO_HIMR_AVAL_MSK					|
#endif
				    /*								SDIO_HIMR_TXERR_MSK				|
				     *								SDIO_HIMR_RXERR_MSK				|
				     *								SDIO_HIMR_TXFOVW_MSK				|
				     *								SDIO_HIMR_RXFOVW_MSK				|
				     *								SDIO_HIMR_TXBCNOK_MSK				|
				     *								SDIO_HIMR_TXBCNERR_MSK			       |
				     *								SDIO_HIMR_C2HCMD_MSK				| */
#ifdef CONFIG_LPS_LCLK
				    SDIO_HIMR_CPWM1_MSK				|
				    SDIO_HIMR_CPWM2_MSK				|
#endif
				    /*								SDIO_HIMR_HSISR_IND_MSK			|
				     *								SDIO_HIMR_GTINT3_IND_MSK			|
				     *								SDIO_HIMR_GTINT4_IND_MSK			|
				     *								SDIO_HIMR_PSTIMEOUT_MSK			|
				     *								SDIO_HIMR_OCPINT_MSK				|
				     *								SDIO_HIMR_ATIMEND_MSK				|
				     *								SDIO_HIMR_ATIMEND_E_MSK			|
				     *								SDIO_HIMR_CTWEND_MSK				| */
				    0);
}

#if 0
/* */
/*	Description: */
/*		Clear corresponding SDIO Host ISR interrupt service. */
/* */
/*	Assumption: */
/*		Using SDIO Local register ONLY for configuration. */
/* */
/*	Created by Roger, 2011.02.11. */
/* */
void ClearInterrupt8192ESdio(PADAPTER padapter)
{
	u32 tmp = 0;
	tmp = SdioLocalCmd52Read4Byte(padapter, SDIO_REG_HISR);
	SdioLocalCmd52Write4Byte(padapter, SDIO_REG_HISR, tmp);
	/*	padapter->IsrContent.IntArray[0] = 0; */
	padapter->IsrContent = 0;
}
#endif

/*
 *	Description:
 *		Enalbe SDIO Host Interrupt Mask configuration on SDIO local domain.
 *
 *	Assumption:
 *		1. Using SDIO Local register ONLY for configuration.
 *		2. PASSIVE LEVEL
 *
 *	Created by Roger, 2011.02.11.
 *   */
void EnableInterrupt8192ESdio(PADAPTER padapter)
{
	PHAL_DATA_TYPE pHalData;
	u32 himr;

	pHalData = GET_HAL_DATA(padapter);
	himr = cpu_to_le32(pHalData->sdio_himr);
	sdio_local_write(padapter, SDIO_REG_HIMR, 4, (u8 *)&himr);


	/*  */
	/* <Roger_Notes> There are some C2H CMDs have been sent before system interrupt is enabled, e.g., C2H, CPWM. */
	/* So we need to clear all C2H events that FW has notified, otherwise FW won't schedule any commands anymore. */
	/* 2011.10.19. */
	/*  */
	rtw_write8(padapter, REG_C2HEVT_CLEAR, C2H_EVT_HOST_CLOSE);

}

/*
 *	Description:
 *		Disable SDIO Host IMR configuration to mask unnecessary interrupt service.
 *
 *	Assumption:
 *		Using SDIO Local register ONLY for configuration.
 *
 *	Created by Roger, 2011.02.11.
 *   */
void DisableInterrupt8192ESdio(PADAPTER padapter)
{
	u32 himr;

	himr = cpu_to_le32(SDIO_HIMR_DISABLED);
	sdio_local_write(padapter, SDIO_REG_HIMR, 4, (u8 *)&himr);

}

/*
 *	Description:
 *		Update SDIO Host Interrupt Mask configuration on SDIO local domain.
 *
 *	Assumption:
 *		1. Using SDIO Local register ONLY for configuration.
 *		2. PASSIVE LEVEL
 *
 *	Created by Roger, 2011.02.11.
 *   */
void UpdateInterruptMask8192ESdio(PADAPTER padapter, u32 AddMSR, u32 RemoveMSR)
{
	HAL_DATA_TYPE *pHalData;

	pHalData = GET_HAL_DATA(padapter);

	if (AddMSR)
		pHalData->sdio_himr |= AddMSR;

	if (RemoveMSR)
		pHalData->sdio_himr &= (~RemoveMSR);

	DisableInterrupt8192ESdio(padapter);
	EnableInterrupt8192ESdio(padapter);
}

/*
 *	Description:
 *		Using 0x100 to check the power status of FW.
 *
 *	Assumption:
 *		Using SDIO Local register ONLY for configuration.
 *
 *	Created by Isaac, 2013.09.10.
 *   */
u8 CheckIPSStatus(PADAPTER padapter)
{
	RTW_INFO("%s(): Read 0x100=0x%02x 0x86=0x%02x\n", __func__,
		 rtw_read8(padapter, 0x100), rtw_read8(padapter, 0x86));

	if (rtw_read8(padapter, 0x100) == 0xEA)
		return _TRUE;
	else
		return _FALSE;
}


#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
void ClearInterrupt8192ESdio(PADAPTER padapter)
{
	HAL_DATA_TYPE   *pHalData = GET_HAL_DATA(padapter);
	u32 v32 = 0;

	RTW_INFO("+%s+\n", __func__);

	v32 = pHalData->sdio_himr | SDIO_HISR_CPWM2;

	pHalData->sdio_hisr &= v32;

	/* clear HISR */
	v32 = pHalData->sdio_hisr & MASK_SDIO_HISR_CLEAR;
	if (v32) {
		v32 = cpu_to_le32(v32);
		sdio_local_write(padapter, SDIO_REG_HISR, 4, (u8 *)&v32);
	}

	RTW_INFO("-%s-\n", __func__);
}
#endif

#ifdef CONFIG_MAC_LOOPBACK_DRIVER
static void sd_recv_loopback(PADAPTER padapter, u32 size)
{
	PLOOPBACKDATA ploopback;
	u32 readsize, allocsize;
	u8 *preadbuf;


	readsize = size;
	RTW_INFO("%s: read size=%d\n", __func__, readsize);
	allocsize = _RND(readsize, rtw_sdio_get_block_size(adapter_to_dvobj(padapter)));

	ploopback = padapter->ploopback;
	if (ploopback) {
		ploopback->rxsize = readsize;
		preadbuf = ploopback->rxbuf;
	} else {
		preadbuf = rtw_malloc(allocsize);
		if (preadbuf == NULL) {
			RTW_INFO("%s: malloc fail size=%d\n", __func__, allocsize);
			return;
		}
	}

	/*	rtw_read_port(padapter, WLAN_RX0FF_DEVICE_ID, readsize, preadbuf); */
	sdio_read_port(&padapter->iopriv.intf, WLAN_RX0FF_DEVICE_ID, readsize, preadbuf);

	if (ploopback)
		_rtw_up_sema(&ploopback->sema);
	else {
		u32 i;

		RTW_INFO("%s: drop pkt\n", __func__);
		for (i = 0; i < readsize; i += 4) {
			RTW_INFO("%08X", *(u32 *)(preadbuf + i));
			if ((i + 4) & 0x1F)
				printk(" ");
			else
				printk("\n");
		}
		printk("\n");
		rtw_mfree(preadbuf, allocsize);
	}
}
#endif /* CONFIG_MAC_LOOPBACK_DRIVER */

#ifdef CONFIG_SDIO_RX_COPY
static u32 sd_recv_rxfifo(PADAPTER padapter, u32 size,
								struct recv_buf **recvbuf_ret)
{
	u32 readsize, ret;
	u8 *preadbuf;
	struct recv_priv *precvpriv;
	struct recv_buf	*precvbuf;
	*recvbuf_ret = NULL;

	readsize = size;

	/* 3 1. alloc recvbuf */
	precvpriv = &padapter->recvpriv;
	precvbuf = rtw_dequeue_recvbuf(&precvpriv->free_recv_buf_queue);
	if (precvbuf == NULL) {
		RTW_INFO("%s: recvbuf unavailable\n", __func__);
		ret = RTW_RBUF_UNAVAIL;
		goto exit;
	}

	/* 3 2. alloc skb */
	if (precvbuf->pskb == NULL) {
		SIZE_PTR tmpaddr = 0;
		SIZE_PTR alignment = 0;

		RTW_INFO("%s: alloc_skb for rx buffer\n", __FUNCTION__);

		precvbuf->pskb = rtw_skb_alloc(MAX_RECVBUF_SZ + RECVBUFF_ALIGN_SZ);

		if (precvbuf->pskb) {
			precvbuf->pskb->dev = padapter->pnetdev;

			tmpaddr = (SIZE_PTR)precvbuf->pskb->data;
			alignment = tmpaddr & (RECVBUFF_ALIGN_SZ - 1);
			skb_reserve(precvbuf->pskb, (RECVBUFF_ALIGN_SZ - alignment));

			precvbuf->phead = precvbuf->pskb->head;
			precvbuf->pdata = precvbuf->pskb->data;
			precvbuf->ptail = skb_tail_pointer(precvbuf->pskb);
			precvbuf->pend = skb_end_pointer(precvbuf->pskb);
			precvbuf->len = 0;
		}

		if (precvbuf->pskb == NULL) {
			RTW_INFO("%s: alloc_skb fail! read=%d\n", __FUNCTION__, readsize);
			ret = RTW_RBUF_PKT_UNAVAIL;
			goto exit;
		}
	}

	/* 3 3. read data from rxfifo */
	preadbuf = precvbuf->pdata;
	/*	rtw_read_port(padapter, WLAN_RX0FF_DEVICE_ID, readsize, preadbuf); */
	ret = sdio_read_port(&padapter->iopriv.intf, WLAN_RX0FF_DEVICE_ID, readsize, preadbuf);
	if (ret == _FAIL) {
		goto exit;
	}


	/* 3 4. init recvbuf */
	precvbuf->len = readsize;

	*recvbuf_ret = precvbuf;
exit:
	return ret;
}

#else
static struct recv_buf *sd_recv_rxfifo(PADAPTER padapter, u32 size)
{
	u32 readsize, allocsize, ret;
	u8 *preadbuf;
	_pkt *ppkt;
	struct recv_priv *precvpriv;
	struct recv_buf	*precvbuf;


	readsize = size;

	/* 3 1. alloc skb */
	/* align to block size */
	allocsize = _RND(readsize, rtw_sdio_get_block_size(adapter_to_dvobj(padapter)));

	ppkt = rtw_skb_alloc(allocsize);

	if (ppkt == NULL) {
		return NULL;
	}

	/* 3 2. read data from rxfifo */
	preadbuf = skb_put(ppkt, readsize);
	/*	rtw_read_port(padapter, WLAN_RX0FF_DEVICE_ID, readsize, preadbuf); */
	ret = sdio_read_port(&padapter->iopriv.intf, WLAN_RX0FF_DEVICE_ID, readsize, preadbuf);
	if (ret == _FAIL) {
		rtw_skb_free(ppkt);
		return NULL;
	}

	/* 3 3. alloc recvbuf */
	precvpriv = &padapter->recvpriv;
	precvbuf = rtw_dequeue_recvbuf(&precvpriv->free_recv_buf_queue);
	if (precvbuf == NULL) {
		rtw_skb_free(ppkt);
		return NULL;
	}

	/* 3 4. init recvbuf */
	precvbuf->pskb = ppkt;

	precvbuf->len = ppkt->len;

	precvbuf->phead = ppkt->head;
	precvbuf->pdata = ppkt->data;
	precvbuf->ptail = skb_tail_pointer(precvbuf->pskb);
	precvbuf->pend = skb_end_pointer(precvbuf->pskb);

	return precvbuf;
}
#endif

static void sd_rxhandler(PADAPTER padapter, struct recv_buf *precvbuf)
{
	struct recv_priv *precvpriv;
	_queue *ppending_queue;


	precvpriv = &padapter->recvpriv;
	ppending_queue = &precvpriv->recv_buf_pending_queue;

	/* 3 1. enqueue recvbuf */
	rtw_enqueue_recvbuf(precvbuf, ppending_queue);

        /* 3 2. trigger recv hdl */
#ifdef CONFIG_RECV_THREAD_MODE
        _rtw_up_sema(&precvpriv->recv_sema);
#else
	#ifdef PLATFORM_LINUX
        tasklet_schedule(&precvpriv->recv_tasklet);
	#endif /* PLATFORM_LINUX */
#endif /* CONFIG_RECV_THREAD_MODE */

}

void sd_int_dpc(PADAPTER padapter)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct intf_hdl *pintfhdl = &padapter->iopriv.intf;

#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	if (pHalData->sdio_hisr & SDIO_HISR_AVAL) {
		/* _irqL irql; */
		u8	freepage[4];

		_sdio_local_read(padapter, SDIO_REG_FREE_TXPG, 4, freepage);
		/* _enter_critical_bh(&pHalData->SdioTxFIFOFreePageLock, &irql); */
		/* _rtw_memcpy(pHalData->SdioTxFIFOFreePage, freepage, 4); */
		/* _exit_critical_bh(&pHalData->SdioTxFIFOFreePageLock, &irql); */
		/* RTW_INFO("SDIO_HISR_AVAL, Tx Free Page = 0x%x%x%x%x\n", */
		/*	freepage[0], */
		/*	freepage[1], */
		/*	freepage[2], */
		/*	freepage[3]); */
		_rtw_up_sema(&(padapter->xmitpriv.xmit_sema));
	}
#endif
	if (pHalData->sdio_hisr & SDIO_HISR_CPWM1) {
		struct reportpwrstate_parm report;

#ifdef CONFIG_LPS_RPWM_TIMER
		_cancel_timer_ex(&(adapter_to_pwrctl(padapter)->pwr_rpwm_timer));
#endif /* CONFIG_LPS_RPWM_TIMER */

		_sdio_local_read(padapter, SDIO_REG_HCPWM1, 1, &report.state);
#ifdef CONFIG_LPS_LCLK
		/* 88e's cpwm value only change BIT0, so driver need to add PS_STATE_S2 for LPS flow. */
		/* modify by Thomas. 2012/4/2.		 */
		report.state |= PS_STATE_S2;
		/* cpwm_int_hdl(padapter, &report); */
		_set_workitem(&(pwrpriv->cpwm_event));
#endif
	}

#ifdef CONFIG_WOWLAN
	if (pHalData->sdio_hisr & SDIO_HISR_CPWM2) {
		u32	value;
		value = rtw_read32(padapter, SDIO_LOCAL_BASE + SDIO_REG_HISR);
		RTW_PRINT("Reset SDIO HISR(0x%08x) original:0x%08x\n",
			  SDIO_LOCAL_BASE + SDIO_REG_HISR, value);
		value |= BIT19;
		rtw_write32(padapter, SDIO_LOCAL_BASE + SDIO_REG_HISR, value);

		value = rtw_read8(padapter, SDIO_LOCAL_BASE + SDIO_REG_HIMR + 2);
		RTW_PRINT("Reset SDIO HIMR CPWM2(0x%08x) original:0x%02x\n",
			  SDIO_LOCAL_BASE + SDIO_REG_HIMR + 2, value);
	}
#endif
	if (pHalData->sdio_hisr & SDIO_HISR_TXERR) {
		u8 *status;
		u32 addr;

		status = rtw_malloc(4);
		if (status) {
			addr = REG_TXDMA_STATUS;
			HalSdioGetCmdAddr8192ESdio(padapter, WLAN_IOREG_DEVICE_ID, addr, &addr);
			_sd_read(pintfhdl, addr, 4, status);
			_sd_write(pintfhdl, addr, 4, status);
			RTW_INFO("%s: SDIO_HISR_TXERR (0x%08x)\n", __func__, le32_to_cpu(*(u32 *)status));
			rtw_mfree(status, 4);
		} else
			RTW_INFO("%s: SDIO_HISR_TXERR, but can't allocate memory to read status!\n", __func__);
	}

#ifdef CONFIG_INTERRUPT_BASED_TXBCN

#ifdef CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
	if (pHalData->sdio_hisr & SDIO_HISR_BCNERLY_INT)
#endif
#ifdef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
		if (pHalData->sdio_hisr & (SDIO_HISR_TXBCNOK | SDIO_HISR_TXBCNERR))
#endif
		{
#if 0 /* for debug */
			if (pHalData->sdio_hisr & SDIO_HISR_BCNERLY_INT)
				RTW_INFO("%s: SDIO_HISR_BCNERLY_INT\n", __func__);

			if (pHalData->sdio_hisr & SDIO_HISR_TXBCNOK)
				RTW_INFO("%s: SDIO_HISR_TXBCNOK\n", __func__);

			if (pHalData->sdio_hisr & SDIO_HISR_TXBCNERR)
				RTW_INFO("%s: SDIO_HISR_TXBCNERR\n", __func__);
#endif
			rtw_mi_set_tx_beacon_cmd(padapter);
		}
#endif /* CONFIG_INTERRUPT_BASED_TXBCN */

	if (pHalData->sdio_hisr & SDIO_HISR_C2HCMD)
		RTW_INFO("%s: C2H Command\n", __func__);

	if (pHalData->sdio_hisr & SDIO_HISR_RX_REQUEST) {
		struct recv_buf *precvbuf;
		int alloc_fail_time = 0;
		u32 hisr = 0, rx_cnt = 0, ret = 0;
                

		pHalData->sdio_hisr ^= SDIO_HISR_RX_REQUEST;
		do {
			pHalData->SdioRxFIFOSize = SdioLocalCmd52Read2Byte(padapter, SDIO_REG_RX0_REQ_LEN);
			if (pHalData->SdioRxFIFOSize != 0) {
#ifdef CONFIG_MAC_LOOPBACK_DRIVER
				sd_recv_loopback(padapter, pHalData->SdioRxFIFOSize);
#else
				ret = sd_recv_rxfifo(padapter, pHalData->SdioRxFIFOSize, &precvbuf);
				if (precvbuf) {
					sd_rxhandler(padapter, precvbuf);
					pHalData->SdioRxFIFOSize = 0;
					rx_cnt++;
				} else {
					alloc_fail_time++;
					if (ret == RTW_RBUF_UNAVAIL || ret == RTW_RBUF_PKT_UNAVAIL)
						rtw_msleep_os(10);
					else {
						RTW_INFO("%s: recv fail!(time=%d)\n", __func__, alloc_fail_time);
						pHalData->SdioRxFIFOSize = 0;
					}
					if (alloc_fail_time >= 10 && rx_cnt != 0)
						break;
				}
#endif
			} else
				break;

			hisr = 0;
			ReadInterrupt8192ESdio(padapter, &hisr);
			hisr &= SDIO_HISR_RX_REQUEST;
			if (!hisr)
				break;
		} while (1);

		if (alloc_fail_time == 10)
			RTW_INFO("%s: exit because recv failed more than 10 times!\n", __func__);
	}

}

void sd_int_hdl(PADAPTER padapter)
{
	u8 data[6];
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	if (RTW_CANNOT_RUN(padapter))
		return;

	_sdio_local_read(padapter, SDIO_REG_HISR, 6, data);
	pHalData->sdio_hisr = le32_to_cpu(*(u32 *)data);
	pHalData->SdioRxFIFOSize = le16_to_cpu(*(u16 *)&data[4]);

	if (pHalData->sdio_hisr & pHalData->sdio_himr) {
		u32 v32;

		pHalData->sdio_hisr &= pHalData->sdio_himr;

		/* clear HISR */
		v32 = pHalData->sdio_hisr & MASK_SDIO_HISR_CLEAR;
		if (v32) {
			v32 = cpu_to_le32(v32);
			_sdio_local_write(padapter, SDIO_REG_HISR, 4, (u8 *)&v32);
		}

		sd_int_dpc(padapter);

	}

}

/*
 *	Description:
 *		Query SDIO Local register to query current the number of Free TxPacketBuffer page.
 *
 *	Assumption:
 *		1. Running at PASSIVE_LEVEL
 *		2. RT_TX_SPINLOCK is NOT acquired.
 *
 *	Created by Roger, 2011.01.28.
 *   */
u8 HalQueryTxBufferStatus8192ESdio(PADAPTER padapter)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	u32 NumOfFreePage;
	/* _irqL irql; */


	pHalData = GET_HAL_DATA(padapter);

	NumOfFreePage = SdioLocalCmd53Read4Byte(padapter, SDIO_REG_FREE_TXPG);

	/* _enter_critical_bh(&pHalData->SdioTxFIFOFreePageLock, &irql); */
	_rtw_memcpy(pHalData->SdioTxFIFOFreePage, &NumOfFreePage, 4);
	/* _exit_critical_bh(&pHalData->SdioTxFIFOFreePageLock, &irql); */

	return _TRUE;
}

/*
 *	Description:
 *		Query SDIO Local register to get the current number of TX OQT Free Space.
 *   */
u8 HalQueryTxOQTBufferStatus8192ESdio(PADAPTER padapter)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);
	pHalData->SdioTxOQTFreeSpace = SdioLocalCmd52Read1Byte(padapter, SDIO_REG_OQT_FREE_PG);
	return _TRUE;

}

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
u8 RecvOnePkt(PADAPTER padapter)
{
	struct recv_buf *precvbuf;
	struct dvobj_priv *psddev;
	PSDIO_DATA psdio_data;
	struct sdio_func *func;
	u32 tmp = 0;
	u16 len = 0;
	u8 res = _FALSE;

	if (padapter == NULL) {
		RTW_ERR("%s: padapter is NULL!\n", __func__);
		return _FALSE;
	}

	psddev = adapter_to_dvobj(padapter);
	psdio_data = &psddev->intf_data;
	func = psdio_data->func;

	/* If RX_DMA is not idle, receive one pkt from DMA */
	res = sdio_local_read(padapter,
			  SDIO_REG_RX0_REQ_LEN, 4, (u8 *)&tmp);
	len = le16_to_cpu(tmp);
	RTW_INFO("+%s: size: %d+\n", __func__, len);

	if (len) {
		sdio_claim_host(func);
		res = sd_recv_rxfifo(padapter, len, &precvbuf);

		if (precvbuf) {
			/* printk("Completed Recv One Pkt.\n"); */
			sd_rxhandler(padapter, precvbuf);
			res = _TRUE;
		} else
			res = _FALSE;
		sdio_release_host(func);
	}
	RTW_INFO("-%s-\n", __func__);
	return res;
}
#endif /* CONFIG_WOWLAN */

