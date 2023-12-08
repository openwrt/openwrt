/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#if defined(CONFIG_MP_INCLUDED)

#include <drv_types.h>
#include <rtw_mp.h>

#define RTW_IWD_MAX_LEN	128

/*
 * Input Format: %s,%d,%d
 *	%s is width, could be
 *		"b" for 1 byte
 *		"w" for WORD (2 bytes)
 *		"dw" for DWORD (4 bytes)
 *	1st %d is address(offset)
 *	2st %d is data to write
 */

int rtw_mp_write_reg(struct net_device *dev,
		     struct iw_request_info *info,
		     struct iw_point *wrqu, char *extra)
{
	char *pch, *pnext;
	char *width_str;
	char width;
	u32 addr, data;
	int ret;
	_adapter *padapter = rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	char input[RTW_IWD_MAX_LEN];
	struct rtw_mp_reg_arg	reg_arg;

	_rtw_memset(input, 0, sizeof(input));

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';

	_rtw_memset(extra, 0, wrqu->length);

	pch = input;

	pnext = strpbrk(pch, " ,.-");
	if (pnext == NULL)
		return -EINVAL;
	*pnext = 0;
	width_str = pch;

	pch = pnext + 1;
	pnext = strpbrk(pch, " ,.-");
	if (pnext == NULL)
		return -EINVAL;
	*pnext = 0;

	ret = sscanf(pch, "%x", &addr);

	pch = pnext + 1;
	pnext = strpbrk(pch, " ,.-");
	if ((pch - input) >= wrqu->length)
		return -EINVAL;

	ret = sscanf(pch, "%x", &data);
	RTW_INFO("data=%x,addr=%x\n", (u32)data, (u32)addr);
	ret = 0;
	width = width_str[0];
	switch (width) {
	case 'b':
		/* 1 byte*/
		if (data > 0xFF) {
			ret = -EINVAL;
			break;
		}
		reg_arg.io_offset = addr;
		reg_arg.io_type = 1;
		reg_arg.io_value = (u8)data;

		if (rtw_mp_phl_reg(padapter, &reg_arg, RTW_MP_REG_CMD_WRITE_BB))
			RTW_INFO("write data=%x,addr=%x OK\n", (u8)data, addr);
		else
			RTW_INFO("write data=%x,addr=%x fail\n", (u8)data, addr);
		break;
	case 'w':
		/* 2 bytes*/
		if (data > 0xFFFF) {
			ret = -EINVAL;
			break;
		}
		reg_arg.io_offset = addr;
		reg_arg.io_type = 2;
		reg_arg.io_value = (u16)data;

		if (rtw_mp_phl_reg(padapter, &reg_arg, RTW_MP_REG_CMD_WRITE_BB))
			RTW_INFO("write data=%x,addr=%x OK\n", (u16)data, addr);
		else
			RTW_INFO("write data=%x,addr=%x fail\n", (u16)data, addr);
		break;
	case 'd':
		/* 4 bytes*/
		reg_arg.io_offset = addr;
		reg_arg.io_type = 4;
		reg_arg.io_value = data;

		if (rtw_mp_phl_reg(padapter, &reg_arg, RTW_MP_REG_CMD_WRITE_BB))
			RTW_INFO("write data=%x,addr=%x OK\n", data, addr);
		else
			RTW_INFO("write data=%x,addr=%x fail\n", data, addr);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}


/*
 * Input Format: %s,%d
 *	%s is width, could be
 *		"b" for 1 byte
 *		"w" for WORD (2 bytes)
 *		"dw" for DWORD (4 bytes)
 *	%d is address(offset)
 *
 * Return:
 *	%d for data readed
 */
int rtw_mp_read_reg(struct net_device *dev,
		    struct iw_request_info *info,
		    struct iw_point *wrqu, char *extra)
{
	char input[RTW_IWD_MAX_LEN];
	char *pch, *pnext;
	char *width_str;
	char width;
	char data[20], tmp[20];
	u32 addr = 0, strtout = 0;
	u32 i = 0, j = 0, ret = 0, data32 = 0;
	_adapter *padapter = rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rtw_mp_reg_arg	reg_arg;

	char *pextra = extra;

	if (wrqu->length > 128)
		return -EFAULT;

	_rtw_memset(input, 0, sizeof(input));
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	_rtw_memset(extra, 0, wrqu->length);
	_rtw_memset(data, '\0', sizeof(data));
	_rtw_memset(tmp, '\0', sizeof(tmp));
	pch = input;
	pnext = strpbrk(pch, " ,.-");
	if (pnext == NULL)
		return -EINVAL;
	*pnext = 0;
	width_str = pch;

	pch = pnext + 1;

	ret = sscanf(pch, "%x", &addr);

	ret = 0;
	width = width_str[0];

	switch (width) {
	case 'b':
		reg_arg.io_offset = addr;
		reg_arg.io_type = 1;

		if (rtw_mp_phl_reg(padapter, &reg_arg, RTW_MP_REG_CMD_READ_BB)) {
			data32 = reg_arg.io_value; //rtw_phl_read8(dvobj->phl, addr);
			RTW_INFO("reg=%x\n", data32);
			sprintf(extra, "%d", data32);
		} else
			sprintf(extra, "reg io fail\n");

		wrqu->length = strlen(extra);
		break;
	case 'w':
		/* 2 bytes*/
		reg_arg.io_offset = addr;
		reg_arg.io_type = 2;

		if (rtw_mp_phl_reg(padapter, &reg_arg, RTW_MP_REG_CMD_READ_BB)) {
			sprintf(data, "%04x", reg_arg.io_value);
			RTW_INFO("reg=%s\n", data);
		} else
			sprintf(extra, "reg io fail\n");

		for (i = 0 ; i <= strlen(data) ; i++) {
			if (i % 2 == 0) {
				tmp[j] = ' ';
				j++;
			}
			if (data[i] != '\0')
				tmp[j] = data[i];

			j++;
		}
		tmp[j]='\0';

		pch = tmp;
		RTW_INFO("pch=%s", pch);

		while (*pch != '\0') {
			pnext = strpbrk(pch, " ");
			if (!pnext || ((pnext - tmp) > 4))
				break;

			pnext++;
			if (*pnext != '\0') {
				/*strtout = simple_strtoul(pnext , &ptmp, 16);*/
				ret = sscanf(pnext, "%x", &strtout);
				pextra += sprintf(pextra, " %d", strtout);
			} else
				break;
			pch = pnext;
		}
		wrqu->length = strlen(extra);
		break;
	case 'd':
		/* 4 bytes */
		reg_arg.io_offset = addr;
		reg_arg.io_type = 4;

		if (rtw_mp_phl_reg(padapter, &reg_arg, RTW_MP_REG_CMD_READ_BB)) {
			sprintf(data, "%08x", reg_arg.io_value);
			RTW_INFO("reg=%s\n", data);
		} else
			sprintf(extra, "reg io fail\n");

		/*add read data format blank*/
		for (i = 0 ; i <= strlen(data) ; i++) {
			if (i % 2 == 0) {
				tmp[j] = ' ';
				j++;
			}
			if (data[i] != '\0')
				tmp[j] = data[i];

			j++;
		}

		pch = tmp;
		RTW_INFO("pch=%s", pch);

		while (*pch != '\0') {
			pnext = strpbrk(pch, " ");
			if (!pnext)
				break;

			pnext++;
			if (*pnext != '\0') {
				ret = sscanf(pnext, "%x", &strtout);
				pextra += sprintf(pextra, " %d", strtout);
			} else
				break;
			pch = pnext;
		}
		wrqu->length = strlen(extra);
		break;

	default:
		wrqu->length = 0;
		ret = -EINVAL;
		break;
	}

	return ret;
}


/*
 * Input Format: %d,%x,%x
 *	%d is RF path, should be smaller than MAX_RF_PATH_NUMS
 *	1st %x is address(offset)
 *	2st %x is data to write
 */
int rtw_mp_write_rf(struct net_device *dev,
		    struct iw_request_info *info,
		    struct iw_point *wrqu, char *extra)
{

	u32 path, addr, data;
	int ret;
	_adapter *padapter = rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	char input[RTW_IWD_MAX_LEN];


	_rtw_memset(input, 0, wrqu->length);
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;


	ret = sscanf(input, "%d,%x,%x", &path, &addr, &data);
	if (ret < 3)
		return -EINVAL;

	if (path >= GET_HAL_RFPATH_NUM(adapter_to_dvobj(padapter)))
		return -EINVAL;
	if (addr > 0xFF)
		return -EINVAL;
	if (data > 0xFFFFF)
		return -EINVAL;

	_rtw_memset(extra, 0, wrqu->length);

	rtw_phl_write_rfreg(GET_PHL_INFO(dvobj), path, addr, 0xFFFFF, data);

	sprintf(extra, "write_rf completed\n");
	wrqu->length = strlen(extra);

	return 0;
}


/*
 * Input Format: %d,%x
 *	%d is RF path, should be smaller than MAX_RF_PATH_NUMS
 *	%x is address(offset)
 *
 * Return:
 *	%d for data readed
 */
int rtw_mp_read_rf(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{
	char input[RTW_IWD_MAX_LEN];
	char *pch, *pnext;
	char data[20], tmp[20];
	u32 path, addr, strtou;
	u32 ret, i = 0 , j = 0;
	_adapter *padapter = rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	char *pextra = extra;

	if (wrqu->length > 128)
		return -EFAULT;
	_rtw_memset(input, 0, wrqu->length);
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	ret = sscanf(input, "%d,%x", &path, &addr);
	if (ret < 2)
		return -EINVAL;

	if (path >= GET_HAL_RFPATH_NUM(adapter_to_dvobj(padapter)))
		return -EINVAL;

	_rtw_memset(extra, 0, wrqu->length);

	sprintf(data, "%08x",  rtw_phl_read_rfreg(GET_PHL_INFO(dvobj), path, addr , 0xFFFFF));
	/*add read data format blank*/
	for (i = 0 ; i <= strlen(data) ; i++) {
		if (i % 2 == 0) {
			tmp[j] = ' ';
			j++;
		}
		tmp[j] = data[i];
		j++;
	}
	pch = tmp;
	RTW_INFO("pch=%s", pch);

	while (*pch != '\0') {
		pnext = strpbrk(pch, " ");
		if (!pnext)
			break;
		pnext++;
		if (*pnext != '\0') {
			/*strtou =simple_strtoul(pnext , &ptmp, 16);*/
			ret = sscanf(pnext, "%x", &strtou);
			pextra += sprintf(pextra, " %d", strtou);
		} else
			break;
		pch = pnext;
	}
	wrqu->length = strlen(extra);

	return 0;
}


int rtw_mp_start(struct net_device *dev,
		 struct iw_request_info *info,
		 struct iw_point *wrqu, char *extra)
{
	int ret = 0;
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmppriv = &padapter->mppriv;
	char *pextra = NULL;

	pmppriv->bprocess_mp_mode = _TRUE;

	if (pmppriv->mode == MP_ON) {
		sprintf(extra, "Already mp_start\n");
		wrqu->length = strlen(extra);
		return ret;
	}

	if (!rtw_is_adapter_up(padapter)) {
		RTW_INFO("adapter_up fail !!!\n");
		return -EPERM;
	}

	rtw_set_scan_deny(padapter, 5000);
	rtw_mi_scan_abort(padapter, _TRUE);

	if (rtw_mp_cmd(padapter, MP_START, RTW_CMDF_DIRECTLY) != _SUCCESS)
		ret = -EPERM;

	_rtw_memset(extra, 0, wrqu->length);
	pextra = extra;
	pextra += sprintf(extra, "mp_start %s\n", ret == 0 ? "ok" : "fail");
	pextra += sprintf(pextra, "EFUSE:%s\n",
		RTW_EFUSE_FROM2STR(rtw_efuse_get_map_from(padapter)));
	wrqu->length = strlen(extra);

	return ret;
}

int rtw_mp_stop(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra)
{
	int ret = 0;
	u8 status = 0;
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmppriv = &padapter->mppriv;


	if (rtw_mp_cmd(padapter, MP_STOP, RTW_CMDF_DIRECTLY) != _SUCCESS)
		ret = -EPERM;

	if (pmppriv->mode != MP_OFF)
		return -EPERM;

	_rtw_memset(extra, 0, wrqu->length);
	sprintf(extra, "mp_stop ok\n");
	wrqu->length = strlen(extra);

	pmppriv->bprocess_mp_mode = _FALSE;

	return ret;
}


int rtw_mp_rate(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra)
{
	u16 rate = MPT_RATE_1M;
	u8		input[RTW_IWD_MAX_LEN];
	_adapter *padapter = rtw_netdev_priv(dev);
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	PMPT_CONTEXT		pMptCtx = &(padapter->mppriv.mpt_ctx);
	u8 tx_nss = get_phy_tx_nss(padapter, padapter_link);
	char *pextra = extra;
	u8 path_i = 0, i = 0;
	u16 pwr_dbm = 0;

	_rtw_memset(input, 0, sizeof(input));
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	rate = rtw_mp_rate_parse(padapter, input);
	padapter->mppriv.rateidx = rate;

	if (rate == 0 && strcmp(input, "1M") != 0) {
		rate = rtw_atoi(input);
#ifndef CONFIG_80211AX_HE
		padapter->mppriv.rateidx = mrate_to_hwrate(rate);
#endif
		/*if (rate <= 0x7f)
			rate = wifirate2_ratetbl_inx((u8)rate);
		else if (rate < 0xC8)
			rate = (rate - 0x79 + MPT_RATE_MCS0);
		HT  rate 0x80(MCS0)  ~ 0x8F(MCS15) ~ 0x9F(MCS31) 128~159
		VHT1SS~2SS rate 0xA0 (VHT1SS_MCS0 44) ~ 0xB3 (VHT2SS_MCS9 #63) 160~179
		VHT rate 0xB4 (VHT3SS_MCS0 64) ~ 0xC7 (VHT2SS_MCS9 #83) 180~199
		else
		VHT rate 0x90(VHT1SS_MCS0) ~ 0x99(VHT1SS_MCS9) 144~153
		rate =(rate - MPT_RATE_VHT1SS_MCS0);
		*/
	}
	_rtw_memset(extra, 0, wrqu->length);

	pextra += sprintf(pextra, "Set data rate to %s index %d\n" , input, padapter->mppriv.rateidx);
	RTW_INFO("%s: %s rate index=%d\n", __func__, input, padapter->mppriv.rateidx);
	pextra += sprintf(pextra, "PPDU Type %s\n", 
								PPDU_TYPE_STR(pmp_priv->rtw_mp_pmact_ppdu_type));
	pextra += sprintf(pextra, "CMD: [mp_plcp_datappdu=%%d]\nPLCP (PPDU Type):\n");
	for (i = pmp_priv->rtw_mp_pmact_ppdu_type; i <= RTW_MP_TYPE_HE_TB; i++)
		pextra += sprintf(pextra, "%d:%s\n", i,PPDU_TYPE_STR(i));

	SetDataRate(padapter);
#if 0
	for (path_i = 0 ; path_i < tx_nss; path_i++) {
		pwr_dbm = rtw_mp_get_pwrtab_dbm(padapter, path_i);
		pextra += sprintf(pextra, "Path%d Pwrdbm:%d" , path_i, pwr_dbm);
	}
#endif
	wrqu->length = strlen(extra);
	return 0;
}


int rtw_mp_channel(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8		input[RTW_IWD_MAX_LEN];
	u32	channel = 1;

	_rtw_memset(input, 0, sizeof(input));
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	channel = rtw_atoi(input);

	_rtw_memset(extra, 0, wrqu->length);
	sprintf(extra, "Change channel %d to channel %d", pmp_priv->channel, channel);
	pmp_priv->channel = channel;
	//pHalData->current_channel = channel; //aka struct rtw_phl_com_t
	SetChannel(padapter);

	wrqu->length = strlen(extra);
	return 0;
}

static void rtw_mp_update_trxsc(_adapter *padapter) {
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8 trxsc_offset = pmp_priv->rtw_mp_trxsc;

	switch (trxsc_offset) {
	case 0:
		pmp_priv->rtw_mp_data_bandwidth = pmp_priv->bandwidth;
		RTW_INFO("%s:TRXSC %d, MP bandwidth = %d\n", __func__, trxsc_offset, pmp_priv->bandwidth);
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		pmp_priv->rtw_mp_data_bandwidth = CHANNEL_WIDTH_20;
		RTW_INFO("%s:TRXSC %d, MP bandwidth = %d\n", __func__, trxsc_offset, pmp_priv->bandwidth);
		break;
	case 9:
	case 10:
	case 11:
	case 12:
		pmp_priv->rtw_mp_data_bandwidth = CHANNEL_WIDTH_40;
		RTW_INFO("%s:TRXSC %d, MP bandwidth = %d\n", __func__, trxsc_offset, pmp_priv->bandwidth);
		break;
	case 13:
	case 14:
		pmp_priv->rtw_mp_data_bandwidth = CHANNEL_WIDTH_40;
		RTW_INFO("%s:TRXSC %d, MP bandwidth = %d\n", __func__, trxsc_offset, pmp_priv->bandwidth);
		break;
	}
}

int rtw_mp_trxsc_offset(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{

	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8		input[RTW_IWD_MAX_LEN];
	u32	trxsc_offset = 0;

	_rtw_memset(input, 0, sizeof(input));
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	trxsc_offset = rtw_atoi(input);
	RTW_INFO("%s: ch offset = %d\n", __func__, trxsc_offset);

	pmp_priv->rtw_mp_trxsc = trxsc_offset;
	rtw_mp_update_trxsc(padapter);
	_rtw_memset(extra, 0, wrqu->length);
	sprintf(extra, "change TRXSC to %d, current Bandwidth=%d\n", 
						pmp_priv->rtw_mp_trxsc, pmp_priv->bandwidth);

	wrqu->length = strlen(extra);
	return 0;
}


int rtw_mp_bandwidth(struct net_device *dev,
		     struct iw_request_info *info,
		     struct iw_point *wrqu, char *extra)
{
	u8 bandwidth = 0, sg = 0;
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8		input[RTW_IWD_MAX_LEN];

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	if (sscanf(input, "40M=%hhd,shortGI=%hhd", &bandwidth, &sg) > 0)
		RTW_INFO("%s: bw=%d sg=%d\n", __func__, bandwidth , sg);
#if 0
	if (bandwidth == 1 && rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_40M))
		bandwidth = CHANNEL_WIDTH_40;
	else if (bandwidth == 2 && rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_80M))
		bandwidth = CHANNEL_WIDTH_80;
	else
		bandwidth = CHANNEL_WIDTH_20;
#else
	rtw_adjust_chbw(padapter, pmp_priv->channel, &bandwidth, &pmp_priv->prime_channel_offset);

	pmp_priv->bandwidth = (u8)bandwidth;
	pmp_priv->preamble = sg;
	_rtw_memset(extra, 0, wrqu->length);
	sprintf(extra, "Change BW %d to BW %d\n", pmp_priv->bandwidth , bandwidth);

	SetBandwidth(padapter);
	rtw_mp_update_trxsc(padapter);
#endif
	wrqu->length = strlen(extra);

	return 0;
}


int rtw_mp_txpower_index(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	char input[RTW_IWD_MAX_LEN];
	u32 rfpath = 0 ;
	u32 txpower_inx = 0, tarpowerdbm = 0;
	char *pextra = extra;
	u8 rf_type = GET_HAL_RFPATH(adapter_to_dvobj(padapter));

	if (wrqu->length > 128)
		return -EFAULT;

	_rtw_memset(input, 0, sizeof(input));

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	_rtw_memset(extra, 0, strlen(extra));

	if (wrqu->length == 2) {
#ifndef CONFIG_80211AX_HE
		if (input[0] != '\0' ) {
			rfpath = rtw_atoi(input);
			txpower_inx = mpt_ProQueryCalTxPower(padapter, rfpath);
		}
#endif
		pextra += sprintf(pextra, " %d\n", txpower_inx);
		tarpowerdbm = mpt_get_tx_power_finalabs_val(padapter, rfpath);
		if (tarpowerdbm > 0)
			pextra += sprintf(pextra, "\t\t dBm:%d", tarpowerdbm);
	} else {
#ifndef CONFIG_80211AX_HE
		txpower_inx = mpt_ProQueryCalTxPower(padapter, 0);
		pextra += sprintf(pextra, "patha=%d", txpower_inx);
		if (rf_type > RF_1T2R) {
			txpower_inx = mpt_ProQueryCalTxPower(padapter, 1);
			pextra += sprintf(pextra, ",pathb=%d", txpower_inx);
		}
		if (rf_type > RF_2T4R) {
			txpower_inx = mpt_ProQueryCalTxPower(padapter, 2);
			pextra += sprintf(pextra, ",pathc=%d", txpower_inx);
		}
		if (rf_type > RF_3T4R) {
			txpower_inx = mpt_ProQueryCalTxPower(padapter, 3);
			pextra += sprintf(pextra, ",pathd=%d", txpower_inx);
		}
#endif
		tarpowerdbm = mpt_get_tx_power_finalabs_val(padapter, 0);
		pextra += sprintf(pextra, "\n\t\t\tpatha dBm=%d", tarpowerdbm);
		if (rf_type > RF_1T2R) {
			tarpowerdbm = mpt_get_tx_power_finalabs_val(padapter, 1);
			pextra += sprintf(pextra, ",pathb dBm=%d", tarpowerdbm);
		}
		if (rf_type > RF_2T4R) {
			tarpowerdbm = mpt_get_tx_power_finalabs_val(padapter, 2);
			pextra += sprintf(pextra, ",pathc dBm=%d", tarpowerdbm);
		}
		if (rf_type > RF_3T4R) {
			tarpowerdbm = mpt_get_tx_power_finalabs_val(padapter, 3);
			pextra += sprintf(pextra, ",pathd dBm=%d", tarpowerdbm);
		}
	}

	wrqu->length = strlen(extra);

	return 0;
}


int rtw_mp_txpower(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{
	u32 idx_a = 0, idx_b = 0, idx_c = 0, idx_d = 0;
	int MsetPower = 1;
	char pout_str_buf[7];
	u8		input[RTW_IWD_MAX_LEN];
	u8 rfpath_i = 0;
	u16 agc_cw_val = 0;
	_adapter *padapter = rtw_netdev_priv(dev);
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct mp_priv *pmppriv = &padapter ->mppriv;
	u8 tx_nss = get_phy_tx_nss(padapter, padapter_link);
	char *pextra = extra;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	MsetPower = strncmp(input, "off", 3);
	if (MsetPower == 0) {
		pmppriv->bSetTxPower = 0;
		sprintf(pextra, "MP Set power off");
	} else {
		if (sscanf(input, "patha=%d,pathb=%d,pathc=%d,pathd=%d", &idx_a, &idx_b, &idx_c, &idx_d) >= 1) {
				pextra += sprintf(pextra, "Set power offset path_A:%d path_B:%d path_C:%d path_D:%d\n", idx_a , idx_b , idx_c , idx_d);
				pmppriv->path_pwr_offset[RF_PATH_A] = (u8)idx_a;
				pmppriv->path_pwr_offset[RF_PATH_B] = (u8)idx_b;
				pmppriv->path_pwr_offset[RF_PATH_C] = (u8)idx_c;
				pmppriv->path_pwr_offset[RF_PATH_D]  = (u8)idx_d;
				pmppriv->bSetTxPower = 1;
		} else if (strncmp(input, "dbm", 3) == 0) {
			u8 signed_flag = 0;
			u8 ret = 0xff;
			int int_num = 0;
			u32 dec_num = 0;
			s16 pout = 0;
			int i;
			u32 poutdbm = 0;
			s32 db_temp = 0;
			s16 pset = 0;
			u8 rfpath;

			if (sscanf(input, "dbm=%7s", pout_str_buf) == 1) {
				ret = 0;
			} else {
				sprintf(extra, "[dbm = -30 ~ 30.00]");
				goto invalid_param_format;
			}

			if(pout_str_buf[0] == '-')
				signed_flag = 1;
			i = sscanf(pout_str_buf, "%d.%3u", &int_num, &dec_num);
			RTW_INFO("%s: pars input =%d.%d\n", __func__, int_num, dec_num);

			if(i == 2)
				dec_num = (dec_num < 10) ? dec_num * 10 : dec_num;

			if (int_num >= 30 || ret == 0xff || dec_num > 99 || (dec_num % 25 != 0)) {
				sprintf(extra, "CMD Format:[dbm= -30.00 ~ 30.00]\n"
					" each scale step value must 0.25 or -0.25\n");
				goto invalid_param_format;
			}

			pset = int_num * TX_POWER_BASE + ((dec_num * TX_POWER_BASE) / 100);
			RTW_INFO("%s: pset=%d\n", __func__, pset);
			pset = ((pset < 0 || signed_flag == 1) ? -pset : pset);


			pextra += sprintf(pextra, "Set power dbm :%d.%d\n", int_num, dec_num);
			pmppriv->txpowerdbm = pset;
			pmppriv->bSetTxPower = 1;
		} else {
			pextra += sprintf(pextra, "Invalid format on line %s\n", input);
			RTW_INFO("Invalid format on line %s\n", input );
			wrqu->length = strlen(extra);
			return 0;
		}

		for (rfpath_i = 0 ; rfpath_i < tx_nss; rfpath_i ++) {
				agc_cw_val = rtw_mp_txpower_dbm(padapter, rfpath_i);
				pextra += sprintf(pextra, "Path:%d PwrAGC:%d\n", rfpath_i,agc_cw_val);
		}
	}

invalid_param_format:
	wrqu->length = strlen(extra);
	return 0;
}


int rtw_mp_ant_tx(struct net_device *dev,
		  struct iw_request_info *info,
		  struct iw_point *wrqu, char *extra)
{
	u8 i;
	u8		input[RTW_IWD_MAX_LEN];
	u8 antenna = 0;
	u16 pwr_dbm = 0;
	_adapter *padapter = rtw_netdev_priv(dev);
	char *pextra = extra;

	_rtw_memset(input, 0, sizeof(input));
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	pextra += sprintf(pextra, "switch Tx antenna to %s\n", input);

	for (i = 0; i < strlen(input); i++) {
		switch (input[i]) {
		case 'a':
			antenna |= MP_ANTENNA_A;
			break;
		case 'b':
			antenna |= MP_ANTENNA_B;
			break;
		case 'c':
			antenna |= MP_ANTENNA_C;
			break;
		case 'd':
			antenna |= MP_ANTENNA_D;
			break;
		}
	}
	/*antenna |= BIT(extra[i]-'a');*/
	RTW_INFO("%s: antenna=0x%x\n", __func__, antenna);
	padapter->mppriv.antenna_trx = antenna;

	SetAntenna(padapter);
	pwr_dbm = rtw_mp_get_pwrtab_dbm(padapter, antenna);
	pextra += sprintf(pextra, "read pwr dbm:%d", pwr_dbm);

	wrqu->length = strlen(extra);
	return 0;
}


int rtw_mp_ant_rx(struct net_device *dev,
		  struct iw_request_info *info,
		  struct iw_point *wrqu, char *extra)
{
	u8 i;
	u16 antenna = 0;
	u8		input[RTW_IWD_MAX_LEN];
	_adapter *padapter = rtw_netdev_priv(dev);

	_rtw_memset(input, 0, sizeof(input));
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	/*RTW_INFO("%s: input=%s\n", __func__, input);*/
	_rtw_memset(extra, 0, wrqu->length);

	sprintf(extra, "switch Rx antenna to %s", input);
	for (i = 0; i < strlen(input); i++) {
		switch (input[i]) {
		case 'a':
			antenna |= MP_ANTENNA_A;
			break;
		case 'b':
			antenna |= MP_ANTENNA_B;
			break;
		case 'c':
			antenna |= MP_ANTENNA_C;
			break;
		case 'd':
			antenna |= MP_ANTENNA_D;
			break;
		}
	}
	RTW_INFO("%s: antenna=0x%x\n", __func__, antenna);

	padapter->mppriv.antenna_trx = antenna;
	SetAntenna(padapter);
	wrqu->length = strlen(extra);

	return 0;
}


int rtw_set_ctx_destAddr(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *wrqu, char *extra)
{
	int jj, kk = 0;

	struct pkt_attrib *pattrib;
	struct mp_priv *pmp_priv;
	_adapter *padapter = rtw_netdev_priv(dev);

	pmp_priv = &padapter->mppriv;
	pattrib = &pmp_priv->tx.attrib;

	if (strlen(extra) < 5)
		return _FAIL;

	RTW_INFO("%s: in=%s\n", __func__, extra);
	for (jj = 0, kk = 0; jj < ETH_ALEN; jj++, kk += 3)
		pattrib->dst[jj] = key_2char2num(extra[kk], extra[kk + 1]);

	RTW_INFO("pattrib->dst:%x %x %x %x %x %x\n", pattrib->dst[0], pattrib->dst[1], pattrib->dst[2], pattrib->dst[3], pattrib->dst[4], pattrib->dst[5]);
	return 0;
}



int rtw_mp_ctx(struct net_device *dev,
	       struct iw_request_info *info,
	       struct iw_point *wrqu, char *extra)
{
	u32 pkTx = 1;
	int countPkTx = 1, cotuTx = 1, CarrSprTx = 1, scTx = 1, sgleTx = 1, stop = 1, payload = 1;
	u32 bStartTest = 1;
	u32 count = 0, pktinterval = 0, pktlen = 0;
	u8 status;
	struct mp_priv *pmp_priv;
	struct pkt_attrib *pattrib;
	_adapter *padapter = rtw_netdev_priv(dev);

	pmp_priv = &padapter->mppriv;
	pattrib = &pmp_priv->tx.attrib;

	if (padapter->registrypriv.mp_mode != 1 ) {
		sprintf(extra, "Error: can't tx ,not in MP mode. \n");
		wrqu->length = strlen(extra);
		return 0;
	}

	if (copy_from_user(extra, wrqu->pointer, wrqu->length))
		return -EFAULT;

	*(extra + wrqu->length) = '\0';
	RTW_INFO("%s: in=%s\n", __func__, extra);
#ifdef CONFIG_CONCURRENT_MODE
	if (!is_primary_adapter(padapter)) {
		sprintf(extra, "Error: MP mode can't support Virtual adapter, Please to use main adapter.\n");
		wrqu->length = strlen(extra);
		return 0;
	}
#endif
	countPkTx = strncmp(extra, "count=", 5); /* strncmp TRUE is 0*/
	cotuTx = strncmp(extra, "background", 20);
	CarrSprTx = strncmp(extra, "background,cs", 20);
	scTx = strncmp(extra, "background,sc", 20);
	sgleTx = strncmp(extra, "background,stone", 20);
	pkTx = strncmp(extra, "background,pkt", 20);
	stop = strncmp(extra, "stop", 4);
	payload = strncmp(extra, "payload=", 8);

	if (sscanf(extra, "count=%d,pkt", &count) > 0)
		RTW_INFO("count= %d\n", count);
	if (sscanf(extra, "pktinterval=%d", &pktinterval) > 0)
		RTW_INFO("pktinterval= %d\n", pktinterval);
	if (sscanf(extra, "pktlen=%d", &pktlen) > 0)
		RTW_INFO("pktlen= %d\n", pktlen);

	if (payload == 0) {
			payload = MP_TX_Payload_default_random;
			if (strncmp(extra, "payload=prbs9", 14) == 0) {
				payload = MP_TX_Payload_prbs9;
				sprintf(extra, "config payload PRBS9\n");
			} else {
				if (sscanf(extra, "payload=%x", &payload) > 0){
					RTW_INFO("payload= %x\n", payload);
					sprintf(extra, "config payload setting = %x\n"
									"1. input payload=[]:\n		"
									"[0]: 00, [1]: A5, [2]: 5A, [3]: FF, [4]: PRBS-9, [5]: Random\n"
									"2. specified a hex payload: payload=0xee\n", payload);
				 }
			}
			pmp_priv->tx.payload = payload;
			wrqu->length = strlen(extra);
			return 0;
	}

	if (_rtw_memcmp(extra, "destmac=", 8)) {
		wrqu->length -= 8;
		rtw_set_ctx_destAddr(dev, info, wrqu, &extra[8]);
		sprintf(extra, "Set dest mac OK !\n");
		return 0;
	}
	/*RTW_INFO("%s: count=%d countPkTx=%d cotuTx=%d CarrSprTx=%d scTx=%d sgleTx=%d pkTx=%d stop=%d\n", __func__, count, countPkTx, cotuTx, CarrSprTx, pkTx, sgleTx, scTx, stop);*/
	_rtw_memset(extra, '\0', strlen(extra));

	if (pktinterval != 0) {
		sprintf(extra, "Pkt Interval = %d", pktinterval);
		padapter->mppriv.pktInterval = pktinterval;
		wrqu->length = strlen(extra);
		return 0;

	} else if (pktlen != 0) {
		sprintf(extra, "Pkt len = %d", pktlen);
		pattrib->pktlen = pktlen;
		wrqu->length = strlen(extra);
		return 0;

	} else if (stop == 0) {
		bStartTest = 0; /* To set Stop*/
		pmp_priv->tx.stop = 1;
		sprintf(extra, "Stop continuous Tx");
	} else {
		bStartTest = 1;

		if (pmp_priv->mode != MP_ON) {
			if (pmp_priv->tx.stop != 1) {
				RTW_INFO("%s:Error MP_MODE %d != ON\n", __func__, pmp_priv->mode);
				return	-EFAULT;
			}
		}
	}

	pmp_priv->tx.count = count;

	if (pkTx == 0 || countPkTx == 0)
		pmp_priv->mode = MP_PACKET_TX;
	if (sgleTx == 0)
		pmp_priv->mode = MP_SINGLE_TONE_TX;
	if (cotuTx == 0)
		pmp_priv->mode = MP_CONTINUOUS_TX;
	if (CarrSprTx == 0)
		pmp_priv->mode = MP_CARRIER_SUPPRISSION_TX;
	if (scTx == 0)
		pmp_priv->mode = MP_SINGLE_CARRIER_TX;

	status = rtw_mp_pretx_proc(padapter, bStartTest, extra);

	if (stop == 0)
		pmp_priv->mode = MP_ON;

	wrqu->length = strlen(extra);
	return status;
}



int rtw_mp_disable_bt_coexist(struct net_device *dev,
			      struct iw_request_info *info,
			      union iwreq_data *wrqu, char *extra)
{
#ifdef CONFIG_BTC
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

#endif
	u8 input[RTW_IWD_MAX_LEN];
	u32 bt_coexist;

	_rtw_memset(input, 0, sizeof(input));

	if (copy_from_user(input, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	input[wrqu->data.length] = '\0';

	bt_coexist = rtw_atoi(input);

#if 0
	if (bt_coexist == 0) {
		RTW_INFO("Set OID_RT_SET_DISABLE_BT_COEXIST: disable BT_COEXIST\n");
#ifdef CONFIG_BTC
		rtw_btcoex_HaltNotify(padapter);
		rtw_btcoex_SetManualControl(padapter, _TRUE);
		/* Force to switch Antenna to WiFi*/
		rtw_write16(padapter, 0x870, 0x300);
		rtw_write16(padapter, 0x860, 0x110);
#endif
		/* CONFIG_BTC */
	} else {
#ifdef CONFIG_BTC
		rtw_btcoex_SetManualControl(padapter, _FALSE);
#endif
	}
#endif

	return 0;
}


int rtw_mp_arx(struct net_device *dev,
				struct iw_request_info *info,
				struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmppriv = &padapter->mppriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	u8	input[RTW_IWD_MAX_LEN];
	u32	ret;
	char *pch, *token, *tmp[2] = {0x00, 0x00};
	u32 i = 0, jj = 0, kk = 0, cnts = 0;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	RTW_INFO("%s: %s\n", __func__, input);
	if (strncmp(input, "setbssid=", 8) == 0) {
		pch = input;
		while ((token = strsep(&pch, "=")) != NULL) {
			if (i > 1)
				break;
			tmp[i] = token;
			i++;
		}
		if ((tmp[0] != NULL) && (tmp[1] != NULL)) {
			cnts = strlen(tmp[1]) / 2;
			if (cnts < 1)
				return -EFAULT;
			RTW_INFO("%s: cnts=%d\n", __func__, cnts);
			RTW_INFO("%s: data=%s\n", __func__, tmp[1]);
			for (jj = 0, kk = 0; jj < cnts ; jj++, kk += 2) {
				pmppriv->network_macaddr[jj] = 
					key_2char2num(tmp[1][kk], tmp[1][kk + 1]);
				RTW_INFO("network_macaddr[%d]=%x\n",
					jj, pmppriv->network_macaddr[jj]);
			}
		} else
			return -EFAULT;

		pmppriv->bSetRxBssid = _TRUE;
	} else if (strncmp(input, "frametype", 9) == 0) {
		if (strncmp(input, "frametype beacon", 16) == 0)
			pmppriv->brx_filter_beacon = _TRUE;
		else
			pmppriv->brx_filter_beacon = _FALSE;

	} else if (strncmp(input, "accept_mac", 10) == 0) {
		pmppriv->bmac_filter = _TRUE;
		pch = input;
		while ((token = strsep(&pch, "=")) != NULL) {
			if (i > 1)
				break;
			tmp[i] = token;
			i++;
		}
		if ((tmp[0] != NULL) && (tmp[1] != NULL)) {
			cnts = strlen(tmp[1]) / 2;
			if (cnts < 1)
				return -EFAULT;
			RTW_INFO("%s: cnts=%d\n", __func__, cnts);
			RTW_INFO("%s: data=%s\n", __func__, tmp[1]);
			for (jj = 0, kk = 0; jj < cnts ; jj++, kk += 2) {
				pmppriv->mac_filter[jj] = key_2char2num(tmp[1][kk], tmp[1][kk + 1]);
				RTW_INFO("%s mac_filter[%d]=%x\n", __func__, jj, pmppriv->mac_filter[jj]);
			}
		} else
			return -EFAULT;

	} else if (strncmp(input, "start", 5) == 0) {
		sprintf(extra, "start");

	} else if (strncmp(input, "stop", 5) == 0) {
		struct rtw_mp_rx_arg rx_arg;

		_rtw_memset((void *)&rx_arg, 0, sizeof(struct rtw_mp_rx_arg));
		rtw_mp_phl_query_rx(padapter, &rx_arg, 2);
		if (rx_arg.cmd_ok) {
			pmppriv->rx_pktcount = rx_arg.rx_ok;
			pmppriv->rx_crcerrpktcount = rx_arg.rx_err;
			RTW_INFO("phl_query_rx rx_ok=%d rx_err=%d\n",
				pmppriv->rx_pktcount, pmppriv->rx_crcerrpktcount);
		} else
			RTW_WARN("phl_query_rx Fail !!!");
	
		pmppriv->bmac_filter = _FALSE;
		pmppriv->bSetRxBssid = _FALSE;
		sprintf(extra, "Received packet OK:%d CRC error:%d ,Filter out:%d",
			pmppriv->rx_pktcount, pmppriv->rx_crcerrpktcount,
			pmppriv->rx_pktcount_filter_out);

	} else if (strncmp(input, "phy", 3) == 0) {
		struct rtw_mp_rx_arg rx_arg;

		_rtw_memset((void *)&rx_arg, 0, sizeof(struct rtw_mp_rx_arg));
		rtw_mp_phl_query_rx(padapter, &rx_arg, 0);
		if (rx_arg.cmd_ok)
			sprintf(extra, "Phy Received packet OK:%d CRC error:%d",
					rx_arg.rx_ok, rx_arg.rx_err);
		else
			sprintf(extra, "PHL Phy Query Fail !!!");

	} else if (strncmp(input, "mac", 3) == 0) {
		struct rtw_mp_rx_arg rx_arg;

		_rtw_memset((void *)&rx_arg, 0, sizeof(struct rtw_mp_rx_arg));
		rtw_mp_phl_query_rx(padapter, &rx_arg, 1);
		if (rx_arg.cmd_ok)
			sprintf(extra, "Mac Received packet OK:%d CRC error:%d",
					rx_arg.rx_ok, rx_arg.rx_err);
		else
			sprintf(extra, "Mac Phy Query Fail !!!");

	} else if (strncmp(input, "mon=", 4) == 0) {
		int bmon = 0;
		ret = sscanf(input, "mon=%d", &bmon);

		if (bmon == 1) {
			pmppriv->rx_bindicatePkt = _TRUE;
			sprintf(extra, "Indicating Receive Packet to network start\n");
		} else {
			pmppriv->rx_bindicatePkt = _FALSE;
			sprintf(extra, "Indicating Receive Packet to network Stop\n");
		}
	} else if (strncmp(input, "loopbk", 6) == 0) {
		u32 val32 = rtw_phl_read32(dvobj->phl, 0xCC20);
		val32 |= BIT0;
		rtw_phl_write32(dvobj->phl, 0xCC20 , val32);
		pmppriv->bloopback = _TRUE;
		sprintf(extra , "Enter MAC LoopBack mode\n");

	} else if (strncmp(input, "gain", 4) == 0) {
		struct rtw_mp_rx_arg rx_arg;
		u32 gain_val = 0xff;
		u8 path_num = 0;
		u8 rf_path = 0xff;
		u8 iscck = 0xff;
		u8 *pch = extra;

		switch (input[4]) {
		case 'a':
			rf_path = RF_PATH_A;
			break;
		case 'b':
			rf_path = RF_PATH_B;
			break;
		case 'c':
			rf_path = RF_PATH_C;
			break;
		case 'd':
			rf_path = RF_PATH_D;
			break;
		}

		if ((sscanf(input + 5, "=0x%x,iscck=%hhd", &gain_val, &iscck) == 2) ||
			(sscanf(input + 5, "=%d,iscck=%hhd", &gain_val, &iscck) == 2))
			RTW_INFO("%s: read gain = %d , is cck =%d\n", __func__, gain_val, iscck);

		else if ((sscanf(input + 4, "=0x%x", &gain_val) == 1) ||
				(sscanf(input + 4, "=%d", &gain_val) == 1))
				iscck = (u8)rtw_mp_is_cck_rate(pmppriv->rateidx);
		else {
				sprintf(pch, "error format: gain=[Dec/Hex]\n"
						"\t\tgaina or gainb=[Dec/Hex],iscck=0/1\n");
				wrqu->length = strlen(pch) + 1;
				return 0;
		}

		if (rf_path == 0xff) {
			rf_path = RF_PATH_A;
			path_num = GET_HAL_RFPATH_NUM(adapter_to_dvobj(padapter)) - 1;
		} else
			path_num = rf_path;

		if (gain_val == 0xff || iscck == 0xff) {
			sprintf(extra, "error format: gaina or gainb=%d,iscck=%d\n", gain_val, iscck);
			wrqu->length = strlen(extra) + 1;
			return 0;
		}

		for (; rf_path <= path_num ; rf_path++) {
			RTW_INFO("%s:set Path:%d gain_offset=%d iscck=%d\n",
					__func__, rf_path, gain_val, iscck);

			_rtw_memset((void *)&rx_arg, 0, sizeof(struct rtw_mp_rx_arg));
			rx_arg.offset = (s8)gain_val;
			rx_arg.iscck = iscck;
			rtw_mp_phl_rx_gain_offset(padapter, &rx_arg, rf_path);

			if (rx_arg.cmd_ok)
				pch += sprintf(pch, "Path %s: 0x%hhx Rx Gain offset OK\n\t\t",
				(rf_path == RF_PATH_A) ? "A":(rf_path == RF_PATH_B) ? "B":
				(rf_path == RF_PATH_C) ? "C":"D", gain_val);
		}
	} else if (strncmp(input, "rssi", 4) == 0) {
		struct rtw_mp_rx_arg rx_arg;
		u8 rssi_path = 0, all_path_num = 0;
		char *pcar = extra;
		u8 i = 0;

		if (strncmp(input, "rssi a", 6) == 0)
			rssi_path = 0;
		else if (strncmp(input, "rssi b", 6) == 0)
			rssi_path = 1;
		else
			all_path_num = GET_HAL_RFPATH_NUM(adapter_to_dvobj(padapter));

		if (all_path_num > 1) {
			rssi_path = 0;
			all_path_num = all_path_num - 1;
		} else 
			all_path_num = rssi_path;

		RTW_INFO("%s:Query RSSI Path:%d to %d\n", __func__, rssi_path, all_path_num);
		_rtw_memset((void *)&rx_arg, 0, sizeof(struct rtw_mp_rx_arg));

		for (i = rssi_path ; i <= all_path_num; i++) {
			rx_arg.rf_path = i;

			rtw_mp_phl_rx_rssi(padapter, &rx_arg);
			if (rx_arg.cmd_ok) {
				int result_int = 0;
				int result_dec = 0;

				result_int = (rx_arg.rssi / 2) -110;
				result_dec = (rx_arg.rssi % 2);
				if (result_dec == 1) {
					result_dec = (result_dec * 10) / 2;
					result_int += 1;
				}
				pcar += sprintf(pcar, "Path%d RSSI=%d.%d ", i, result_int, result_dec);
			} else
				pcar += sprintf(pcar, "Path%d RSSI Fail\n", i);

		}
	} else if (strncmp(input, "physts", 6) == 0) {
		struct rtw_mp_rx_arg rx_arg;
		bool bon;

		if (strncmp(input, "physts on", 9) == 0)
			bon = true;
		else
			bon = false;

		_rtw_memset((void *)&rx_arg, 0, sizeof(struct rtw_mp_rx_arg));
		rtw_mp_phl_rx_physts(padapter, &rx_arg, bon);
		if (rx_arg.cmd_ok)
			sprintf(extra, "start OK" );
		else
			sprintf(extra, "start Fail");
	}

	wrqu->length = strlen(extra) + 1;

	return 0;
}


int rtw_mp_trx_query(struct net_device *dev,
		     struct iw_request_info *info,
		     struct iw_point *wrqu, char *extra)
{
	u32 txok, txfail, rxok, rxfail, rxfilterout;
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;


	if (pmp_priv->rtw_mp_tx_method == RTW_MP_PMACT_TX)
		rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_CMD_PHY_OK, pmp_priv->rtw_mp_tx_method, _FALSE);

	txok = padapter->mppriv.tx.sended;
	txfail = 0;
	rxok = padapter->mppriv.rx_pktcount;
	rxfail = padapter->mppriv.rx_crcerrpktcount;
	rxfilterout = padapter->mppriv.rx_pktcount_filter_out;

	_rtw_memset(extra, '\0', 128);

	sprintf(extra, "Tx OK:%d, Tx Fail:%d, Rx OK:%d, CRC error:%d ,Rx Filter out:%d\n", txok, txfail, rxok, rxfail, rxfilterout);

	wrqu->length = strlen(extra) + 1;

	return 0;
}


int rtw_mp_pwrtrk(struct net_device *dev,
		  struct iw_request_info *info,
		  struct iw_point *wrqu, char *extra)
{
	u8 enable;
	u32 thermal;
	s32 ret = 0;
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8 input[RTW_IWD_MAX_LEN];

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	_rtw_memset(extra, 0, wrqu->length);

	enable = RTW_MP_TSSI_OFF;
	if (wrqu->length > 1) {
		/* not empty string*/
		if (strncmp(input, "off", 3) == 0) {
			enable = RTW_MP_TSSI_OFF;
			sprintf(extra, "TSSI power tracking off");
		} else if (strncmp(input, "on", 2) == 0) {
			enable = RTW_MP_TSSI_ON;
			sprintf(extra, "TSSI power tracking on");
		} else if (strncmp(input, "cal", 3) == 0) {
			enable = RTW_MP_TSSI_CAL;
			sprintf(extra, "TSSI cal");
		} else {
			input[wrqu->length] = '\0';
			enable = rtw_atoi(input);
			sprintf(extra, "TSSI power tracking %d", enable);
		}

		if (enable <= RTW_MP_TSSI_CAL)
			ret = rtw_mp_set_tssi_pwrtrk(padapter, enable);

		if (ret == false)
			sprintf(extra, "set TSSI power tracking fail");
		else
			pmp_priv->tssi_mode = enable;
	} else {
		enable = rtw_mp_get_tssi_pwrtrk(padapter);
		sprintf(extra, "Get TSSI state: %d\n\
		incput (int/str): [0]:off / [1]:on / [2]:cal for TSSI Tracking", enable);
	}
	wrqu->length = strlen(extra);

	return 0;
}

int rtw_mp_psd(struct net_device *dev,
	       struct iw_request_info *info,
	       struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	u8		input[RTW_IWD_MAX_LEN];

	_rtw_memset(input, 0, sizeof(input));
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	strcpy(extra, input);

	wrqu->length = mp_query_psd(padapter, extra);

	return 0;
}

int rtw_mp_uuid(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	u32 uuid;

	if (copy_from_user(extra, wrqu->pointer, wrqu->length))
		return -EFAULT;

	GetUuid(padapter, &uuid);

	_rtw_memset(extra, 0, wrqu->length);
	sprintf(extra, "%d", uuid);

	wrqu->length = strlen(extra);

	return 0;
}

int rtw_mp_thermal(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{
	u8 val[4] = {0};
	u8 ret = 0;
	u16 ther_path_addr[4] = {0};
	u16 cnt = 1;
	_adapter *padapter = rtw_netdev_priv(dev);
	int rfpath = RF_PATH_A;

	if (copy_from_user(extra, wrqu->pointer, wrqu->length))
		return -EFAULT;

	if ((strncmp(extra, "write", 6) == 0)) {
#if 0
		int i;
		u16 raw_cursize = 0, raw_maxsize = 0;

		raw_maxsize = efuse_GetavailableSize(padapter);
		RTW_INFO("[eFuse available raw size]= %d bytes\n", raw_maxsize - raw_cursize);
		if (2 > raw_maxsize - raw_cursize) {
			RTW_INFO("no available efuse!\n");
			return -EFAULT;
		}

		for (i = 0; i < GET_HAL_RFPATH_NUM(adapter_to_dvobj(padapter)); i++) {
				GetThermalMeter(padapter, i , &val[i]);
				if (ther_path_addr[i] != 0 && val[i] != 0) {
					if (rtw_efuse_map_write(padapter, ther_path_addr[i], cnt, &val[i]) == _FAIL) {
						RTW_INFO("Error efuse write thermal addr 0x%x ,val = 0x%x\n", ther_path_addr[i], val[i]);
						return -EFAULT;
					}
				} else {
						RTW_INFO("Error efuse write thermal Null addr,val \n");
						return -EFAULT;
				}
		}
#endif
		_rtw_memset(extra, 0, wrqu->length);
		sprintf(extra, " efuse write ok :%d", val[0]);
	} else {
		ret = sscanf(extra, "%d", &rfpath);
		if (ret < 1) {
			rfpath = RF_PATH_A;
			RTW_INFO("default thermal of path(%d)\n", rfpath);
		}
		if (rfpath >= GET_HAL_RFPATH_NUM(adapter_to_dvobj(padapter)))
			return -EINVAL;

		RTW_INFO("read thermal of path(%d)\n", rfpath);
		GetThermalMeter(padapter, rfpath, &val[0]);

		_rtw_memset(extra, 0, wrqu->length);
		sprintf(extra, "%d [0x%hhx]", val[0], val[0]);
	}

	wrqu->length = strlen(extra);

	return 0;
}



int rtw_mp_reset_stats(struct net_device *dev,
		       struct iw_request_info *info,
		       struct iw_point *wrqu, char *extra)
{
	struct mp_priv *pmp_priv;
	_adapter *padapter = rtw_netdev_priv(dev);

	pmp_priv = &padapter->mppriv;

	pmp_priv->tx.sended = 0;
	pmp_priv->tx_pktcount = 0;
	pmp_priv->rx_pktcount = 0;
	pmp_priv->rx_pktcount_filter_out = 0;
	pmp_priv->rx_crcerrpktcount = 0;

	rtw_mp_reset_phy_count(padapter);

	_rtw_memset(extra, 0, wrqu->length);
	sprintf(extra, "mp_reset_stats ok\n");
	wrqu->length = strlen(extra);

	return 0;
}


int rtw_mp_dump(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra)
{
	struct mp_priv *pmp_priv;
	u8		input[RTW_IWD_MAX_LEN];
	_adapter *padapter = rtw_netdev_priv(dev);

	pmp_priv = &padapter->mppriv;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	if (strncmp(input, "all", 4) == 0) {
		//mac_reg_dump(RTW_DBGDUMP, padapter);
		//bb_reg_dump(RTW_DBGDUMP, padapter);
		//rf_reg_dump(RTW_DBGDUMP, padapter);
	}
	return 0;
}


int rtw_mp_phypara(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{

	_adapter *padapter = rtw_netdev_priv(dev);
	char	input[RTW_IWD_MAX_LEN];
	u32		invalxcap = 0, ret = 0;


	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	RTW_INFO("%s:priv in=%s\n", __func__, input);

	if (strncmp(input, "xcap=", 5) == 0) {
		if ((sscanf(input+4, "=0x%x", &invalxcap) == 1) ||
			(sscanf(input+4, "=%d", &invalxcap) == 1)) {
			if (invalxcap < 255) {
				rtw_mp_set_crystal_cap(padapter, invalxcap);
				sprintf(extra, "Set xcap = %d [0x%hhx]", invalxcap, invalxcap);
			} else
				sprintf(extra, "Error formats , inpunt value over 255 !\n");

			wrqu->length = strlen(extra);
			return ret;
		}
	}

	sprintf(extra, "Error formats , inpunt [xcap=%%d/0x%%x]\n");

	wrqu->length = strlen(extra);
	return ret;
}


int rtw_mp_SetRFPath(struct net_device *dev,
		     struct iw_request_info *info,
		     struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	char	input[RTW_IWD_MAX_LEN];
	int		bMain = 1, bTurnoff = 1;
#ifdef CONFIG_ANTENNA_DIVERSITY
	u8 ret = _TRUE;
#endif

	RTW_INFO("%s:iwpriv in=%s\n", __func__, input);

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;
#if 0
	bMain = strncmp(input, "1", 2); /* strncmp TRUE is 0*/
	bTurnoff = strncmp(input, "0", 3); /* strncmp TRUE is 0*/

	_rtw_memset(extra, 0, wrqu->length);
#ifdef CONFIG_ANTENNA_DIVERSITY
	if (bMain == 0)
		ret = rtw_mp_set_antdiv(padapter, _TRUE);
	else
		ret = rtw_mp_set_antdiv(padapter, _FALSE);
	if (ret == _FALSE)
		RTW_INFO("%s:ANTENNA_DIVERSITY FAIL\n", __func__);
#endif

	if (bMain == 0) {
		MP_PHY_SetRFPathSwitch(padapter, _TRUE);
		RTW_INFO("%s:PHY_SetRFPathSwitch=TRUE\n", __func__);
		sprintf(extra, "mp_setrfpath Main\n");

	} else if (bTurnoff == 0) {
		MP_PHY_SetRFPathSwitch(padapter, _FALSE);
		RTW_INFO("%s:PHY_SetRFPathSwitch=FALSE\n", __func__);
		sprintf(extra, "mp_setrfpath Aux\n");
	} else {
		bMain = MP_PHY_QueryRFPathSwitch(padapter);
		RTW_INFO("%s:Query RF Path = %s\n", __func__, (bMain ? "Main":"Aux"));
		sprintf(extra, "RF Path %s\n" , (bMain ? "1":"0"));
	}
#endif
	wrqu->length = strlen(extra);

	return 0;
}


int rtw_mp_switch_rf_path(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv;
	char	input[RTW_IWD_MAX_LEN];
	int		bwlg = 1, bwla = 1, btg = 1, bbt=1;
	u8 ret = 0;


	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	pmp_priv = &padapter->mppriv;

	RTW_INFO("%s: in=%s\n", __func__, input);

#ifdef CONFIG_RTL8821C /* only support for 8821c wlg/wla/btg/bt RF switch path */
	if ((strncmp(input, "WLG", 3) == 0) || (strncmp(input, "1", 1) == 0)) {
		pmp_priv->rf_path_cfg = SWITCH_TO_WLG;
		sprintf(extra, "switch rf path WLG\n");

	} else if ((strncmp(input, "WLA", 3) == 0) || (strncmp(input, "2", 1) == 0)) {
		pmp_priv->rf_path_cfg = SWITCH_TO_WLA;
		sprintf(extra, "switch rf path WLA\n");

	} else if ((strncmp(input, "BTG", 3) == 0) || (strncmp(input, "0", 1) == 0)) {
		pmp_priv->rf_path_cfg = SWITCH_TO_BTG;
		sprintf(extra, "switch rf path BTG\n");

	} else if ((strncmp(input, "BT", 3) == 0) || (strncmp(input, "3", 1) == 0)) {
		pmp_priv->rf_path_cfg = SWITCH_TO_BT;
		sprintf(extra, "switch rf path BT\n");
	} else {
		pmp_priv->rf_path_cfg = SWITCH_TO_WLG;
		sprintf(extra, "Error input, default set WLG\n");
		return -EFAULT;
	}

	mp_phy_switch_rf_path_set(padapter, &pmp_priv->rf_path_cfg);
#endif
	wrqu->length = strlen(extra);

	return ret;

}
int rtw_mp_QueryDrv(struct net_device *dev,
		    struct iw_request_info *info,
		    union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	char	input[RTW_IWD_MAX_LEN];
	int	qAutoLoad = 1;
	//struct efuse_info *efuse = adapter_to_efuse(padapter);

	if (copy_from_user(input, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;
	RTW_INFO("%s:iwpriv in=%s\n", __func__, input);

	qAutoLoad = strncmp(input, "autoload", 8); /* strncmp TRUE is 0*/

	if (qAutoLoad == 0) {
		RTW_INFO("%s:qAutoLoad\n", __func__);

		//if (efuse->is_autoload_fail)
		//	sprintf(extra, "fail");
		//else
		//	sprintf(extra, "ok");
	}
	wrqu->data.length = strlen(extra) + 1;
	return 0;
}


int rtw_mp_PwrCtlDM(struct net_device *dev,
		    struct iw_request_info *info,
		    struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	u8		input[RTW_IWD_MAX_LEN];
	u8		pwrtrk_state = 0;
	u8		pwtk_type[5][25] = {"Thermal tracking off","Thermal tracking on",
					"TSSI tracking off","TSSI tracking on","TSSI calibration"};

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	RTW_INFO("%s: in=%s\n", __func__, input);

	if (wrqu->length == 2) {
		if(input[0] >= '0' && input[0] <= '4') {
			pwrtrk_state = rtw_atoi(input);
			/*MPT_PwrCtlDM(padapter, pwrtrk_state);*/
			sprintf(extra, "PwrCtlDM start %s\n" , pwtk_type[pwrtrk_state]);
		} else {
			sprintf(extra, "Error unknown number ! Please check your input number\n"
				" 0 : Thermal tracking off\n 1 : Thermal tracking on\n 2 : TSSI tracking off\n"
				" 3 : TSSI tracking on\n 4 : TSSI calibration\n");
		}
		wrqu->length = strlen(extra);

		return 0;
	}
	if (strncmp(input, "start", 5) == 0 || strncmp(input, "thertrk on", 10) == 0) {/* strncmp TRUE is 0*/
		pwrtrk_state = 1;
		sprintf(extra, "PwrCtlDM start %s\n" , pwtk_type[pwrtrk_state]);
	} else if (strncmp(input, "thertrk off", 11) == 0 || strncmp(input, "stop", 5) == 0) {
		pwrtrk_state = 0;
		sprintf(extra, "PwrCtlDM stop %s\n" , pwtk_type[pwrtrk_state]);
	} else if (strncmp(input, "tssitrk off", 11) == 0){
		pwrtrk_state = 2;
		sprintf(extra, "PwrCtlDM stop %s\n" , pwtk_type[pwrtrk_state]);
	} else if (strncmp(input, "tssitrk on", 10) == 0){
		pwrtrk_state = 3;
		sprintf(extra, "PwrCtlDM start %s\n" , pwtk_type[pwrtrk_state]);
	} else if (strncmp(input, "tssik", 5) == 0){
		pwrtrk_state = 4;
		sprintf(extra, "PwrCtlDM start %s\n" , pwtk_type[pwrtrk_state]);
	} else {
		pwrtrk_state = 0;
		sprintf(extra, "Error input, default PwrCtlDM stop\n"
			" thertrk off : Thermal tracking off\n thertrk on : Thermal tracking on\n"
			" tssitrk off : TSSI tracking off\n tssitrk on : TSSI tracking on\n tssik : TSSI calibration\n\n"
			" 0 : Thermal tracking off\n 1 : Thermal tracking on\n 2 : TSSI tracking off\n"
			" 3 : TSSI tracking on\n 4 : TSSI calibration\n");
	}

	/*MPT_PwrCtlDM(padapter, pwrtrk_state);*/
	wrqu->length = strlen(extra);

	return 0;
}

int rtw_mp_iqk(struct net_device *dev,
		 struct iw_request_info *info,
		 struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);

	rtw_mp_trigger_iqk(padapter);

	return 0;
}

int rtw_mp_lck(struct net_device *dev,
		 struct iw_request_info *info,
		 struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);

	rtw_mp_trigger_lck(padapter);

	return 0;
}

int rtw_mp_dpk(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	//struct dm_struct *phydm = adapter_to_phydm(padapter);
	struct pwrctrl_priv *pwrctrlpriv = adapter_to_pwrctl(padapter);

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	*(extra + wrqu->data.length) = '\0';

	if (strncmp(extra, "off", 3) == 0 && strlen(extra) < 4) {
			//phydm->dpk_info.is_dpk_enable = 0;
			//halrf_dpk_enable_disable(phydm);
			sprintf(extra, "set dpk off\n");

	} else if (strncmp(extra, "on", 2) == 0 && strlen(extra) < 3) {
			//phydm->dpk_info.is_dpk_enable = 1;
			//halrf_dpk_enable_disable(phydm);
			sprintf(extra, "set dpk on\n");
	} else	{
			rtw_mp_trigger_dpk(padapter);
			sprintf(extra, "set dpk trigger\n");
	}

	wrqu->data.length = strlen(extra);

	return 0;
}

int rtw_mp_get_tsside(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	char input[RTW_IWD_MAX_LEN];
	u8 rfpath = 0xff;
	s8 tssi_de = 0;
	char pout_str_buf[7];
	char tgr_str_buf[7];
	u8 pout_signed_flag = 0 , tgrpwr_signed_flag = 0;
	int int_num = 0;
	u32 dec_num = 0;
	s32 pout = 0;
	s32 tgrpwr = 0;
	int i;

	if (wrqu->length > 128)
		return -EFAULT;

	_rtw_memset(input, 0, sizeof(input));

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;
	input[wrqu->length] = '\0';

	if (wrqu->length == 2) {
		rfpath = rtw_atoi(input);
		if (rfpath >= 0 && rfpath <= 3) {
			tssi_de = rtw_mp_get_tssi_de(padapter, rfpath);
		} else
			sprintf(extra, "Invalid command format, please indicate RF path 0/1/2/3");

	} else if (sscanf(input + 1, "dbm=%7s pwr=%7s", tgr_str_buf , pout_str_buf) == 2) {
		/*
		* rtwpriv wlan0 mp_get_tsside adbm=12 pwr=12
		* [adbm] target power [pwr] output power
		* rf_path : 0 = adbm , 1 = bdbm
		* dbm : -15.00 ~ 25.00
		* pwr : -15.00 ~ 25.00
		* ex : rtwpriv wlan0 mp_get_tsside adbm=16 pwr=14.25
		*/
		RTW_INFO("%s: in=tgr_str %s  pout_str %s\n", __func__, tgr_str_buf , pout_str_buf);
		switch (input[0]) {
		case 'a':
			rfpath = RF_PATH_A;
			break;
		case 'b':
			rfpath = RF_PATH_B;
			break;
		case 'c':
			rfpath = RF_PATH_C;
			break;
		case 'd':
			rfpath = RF_PATH_D;
			break;
		default:
			goto error;
			break;
		}

		if(pout_str_buf[0] == '-')
			pout_signed_flag = 1;

		i = sscanf(pout_str_buf, "%d.%2u", &int_num, &dec_num);
		pout = int_num * 100;
		RTW_DBG("%s:pout %d int %d dec %d\n", __func__, pout, int_num , dec_num);

		if (i == 2) {

			u8 idx = 0;
			u32 dec = 0;
			u8 str_len = 0;
			char *token, *tmp[3] = {};
			char *pextra;

			pextra = pout_str_buf;
			while ((token = strsep (&pextra,".")) != NULL) {
				tmp[idx] = token;
				RTW_INFO("%s() token %d = %s strlen =%ld\n", __func__,
					idx, tmp[idx], strlen(tmp[idx]));
				idx++;
			}

			str_len = strlen(tmp[1]);
			dec = rtw_atoi(tmp[1]);
			dec_num = (str_len == 1) ? dec * 10: (str_len == 2) ? dec * 1: dec;
			RTW_INFO("%s() decimal_num  = %d\n", __func__, dec_num);
			pout += ((pout < 0 || pout_signed_flag == 1) ? -dec_num : dec_num);
		}
		if (pout < -1500 || pout > 2500)
			goto error;
		RTW_INFO("%s:pout %d\n", __func__, pout);

		if (tgr_str_buf[0] == '-')
			tgrpwr_signed_flag = 1;

		int_num = 0;
		dec_num = 0;

		i = sscanf(tgr_str_buf, "%d.%2u", &int_num, &dec_num);
		tgrpwr = int_num * 100;
		RTW_DBG("%s:tgrpwr %d int %d dec %d\n", __func__, tgrpwr, int_num , dec_num);

		if (i == 2) {

			u8 idx = 0;
			u32 dec = 0;
			u8 str_len = 0;
			char *token, *tmp[3] = {};
			char *pextra;

			pextra = tgr_str_buf;
			while ((token = strsep (&pextra,".")) != NULL) {
				tmp[idx] = token;
				RTW_INFO("%s() token %d = %s strlen =%ld\n", __func__,
					idx, tmp[idx], strlen(tmp[idx]));
				idx++;
			}

			str_len = strlen(tmp[1]);
			dec = rtw_atoi(tmp[1]);
			dec_num = (str_len == 1) ? dec * 10: (str_len == 2) ? dec * 1: dec;
			RTW_INFO("%s() decimal_num  = %d\n", __func__, dec_num);
			tgrpwr += ((tgrpwr < 0 || tgrpwr_signed_flag == 1) ? -dec_num : dec_num);
		}

		if (tgrpwr < -1500 || 2500 < tgrpwr)
			goto error;
		RTW_INFO("%s:tgrpwr %d\n", __func__, tgrpwr);

		tssi_de = (s8)rtw_mp_get_online_tssi_de(padapter, pout, tgrpwr, rfpath);
	}

	if (rfpath == 0)
		sprintf(extra, "patha=%d hex:%x", tssi_de, (u8)tssi_de);
	else if (rfpath == 1)
		sprintf(extra, "pathb=%d hex:%x", tssi_de, (u8)tssi_de);
	else if (rfpath == 2)
		sprintf(extra, "pathc=%d hex:%x", tssi_de, (u8)tssi_de);
	else if (rfpath == 3)
		sprintf(extra, "pathd=%d hex:%x", tssi_de, (u8)tssi_de);
	else
		goto error;

	wrqu->length = strlen(extra);
	return 0;
error:

	sprintf(extra, "Invalid command format, please indicate RF path mp_get_tsside [0/1/2/3]\n\
			GET ONLINE TSSI DE:\n\
			mp_get_tsside adbm=-15.00 ~ 25.00 pwr=-15.00 ~ 25.00\n\
			mp_get_tsside bdbm=-15.00 ~ 25.00 pwr=-15.00 ~ 25.00\n");
	wrqu->length = strlen(extra);

	return 0;
}

int rtw_mp_set_tsside(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{
	int tsside_val = 0;
	u8 rf_path = RF_PATH_A;
	char input[RTW_IWD_MAX_LEN];

	_adapter *padapter = rtw_netdev_priv(dev);

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	RTW_INFO("%s:input =[%s]\n", __func__, input);

	switch (input[4]) {
	case 'a':
		rf_path = RF_PATH_A;
		break;
	case 'b':
		rf_path = RF_PATH_B;
		break;
	case 'c':
		rf_path = RF_PATH_C;
		break;
	case 'd':
		rf_path = RF_PATH_D;
		break;
	default:
		goto exit_err;
	}

	if ((sscanf(input+5, "=0x%x", &tsside_val) == 1) ||
		(sscanf(input+5, "=%d", &tsside_val) == 1)) {

		RTW_INFO("%s:got tsside val =[%d] 0x%x\n", __func__, tsside_val, (u32)tsside_val);
		if (tsside_val > 255)
			sprintf(extra, "Error TSSI DE value: %d" , tsside_val);
		else {
			sprintf(extra, "Set TSSI DE path_%s: %d",
				rf_path == RF_PATH_A ? "A" : rf_path == RF_PATH_B ? "B" :
				rf_path == RF_PATH_C ? "C":"D", tsside_val);
			rtw_mp_set_tsside2verify(padapter, (u32)tsside_val, rf_path);
		}
	} else
		goto exit_err;

	wrqu->length = strlen(extra);

	return 0;

exit_err:
	sprintf(extra, "Invalid command format,\n\t\t"
			"please input TSSI DE value within patha/b/c/d=[decimal] or [hex:0xXX]");

	wrqu->length = strlen(extra);

	return 0;
}

int rtw_mp_getver(struct net_device *dev,
		  struct iw_request_info *info,
		  union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv;

	pmp_priv = &padapter->mppriv;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	sprintf(extra, "rtwpriv=%d\n", RTWPRIV_VER_INFO);
	wrqu->data.length = strlen(extra);
	return 0;
}


int rtw_mp_mon(struct net_device *dev,
	       struct iw_request_info *info,
	       union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = &padapter->mppriv;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	//struct hal_ops *pHalFunc = &hal->hal_func;
	NDIS_802_11_NETWORK_INFRASTRUCTURE networkType;
	int bstart = 1, bstop = 1;

	networkType = Ndis802_11Infrastructure;
	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	*(extra + wrqu->data.length) = '\0';

#if 0 //def CONFIG_MP_INCLUDED
	if (init_mp_priv(padapter) == _FAIL)
		RTW_INFO("%s: initialize MP private data Fail!\n", __func__);
	padapter->mppriv.channel = 6;

	bstart = strncmp(extra, "start", 5); /* strncmp TRUE is 0*/
	bstop = strncmp(extra, "stop", 4); /* strncmp TRUE is 0*/
	if (bstart == 0) {
		mp_join(padapter, WIFI_FW_ADHOC_STATE);
		SetPacketRx(padapter, _TRUE, _FALSE);
		SetChannel(padapter);
		pmp_priv->rx_bindicatePkt = _TRUE;
		pmp_priv->bRTWSmbCfg = _TRUE;
		sprintf(extra, "monitor mode start\n");
	} else if (bstop == 0) {
		SetPacketRx(padapter, _FALSE, _FALSE);
		pmp_priv->rx_bindicatePkt = _FALSE;
		pmp_priv->bRTWSmbCfg = _FALSE;
		padapter->registrypriv.mp_mode = 1;
		pHalFunc->hal_deinit(padapter);
		padapter->registrypriv.mp_mode = 0;
		pHalFunc->hal_init(padapter);
		/*rtw_disassoc_cmd(padapter, 0, 0);*/
		if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE) {
			rtw_disassoc_cmd(padapter, 500, 0);
			rtw_indicate_disconnect(padapter, 0, _FALSE);
			/*rtw_free_assoc_resources_cmd(padapter, _TRUE, 0);*/
		}
		sprintf(extra, "monitor mode Stop\n");
	}
#endif
	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_mp_pretx_proc(_adapter *padapter, u8 bstart, char *extra)
{
	struct mp_priv *pmp_priv = &padapter->mppriv;
	char *pextra = extra;

	switch (pmp_priv->mode) {

	case MP_PACKET_TX:
		if (bstart == 0) {
			pmp_priv->tx.stop = 1;
			pmp_priv->mode = MP_ON;
			sprintf(extra, "Stop continuous Tx");
		} else if (pmp_priv->tx.stop == 1) {
			pextra = extra + strlen(extra);
			pextra += sprintf(pextra, "\nStart continuous DA=ffffffffffff len=1500 count=%u\n", pmp_priv->tx.count);
			pmp_priv->tx.stop = 0;
			/*SetPacketTx(padapter);*/
		} else
			return -EFAULT;
		rtw_set_phl_packet_tx(padapter, bstart);
		return 0;
	case MP_SINGLE_TONE_TX:
		if (bstart != 0)
			strcat(extra, "\nStart continuous DA=ffffffffffff len=1500\n infinite=yes.");
		
		rtw_mp_singletone_tx(padapter, (u8)bstart);
		break;
	case MP_CONTINUOUS_TX:
		if (bstart != 0)
			strcat(extra, "\nStart continuous DA=ffffffffffff len=1500\n infinite=yes.");
		rtw_mp_continuous_tx(padapter, (u8)bstart);
		break;
	case MP_CARRIER_SUPPRISSION_TX:
		if (bstart != 0) {
			if (rtw_mp_hwrate2mptrate(pmp_priv->rateidx) <= MPT_RATE_11M)
				strcat(extra, "\nStart continuous DA=ffffffffffff len=1500\n infinite=yes.");
			else
				strcat(extra, "\nSpecify carrier suppression but not CCK rate");
		}
		rtw_mp_carriersuppr_tx(padapter, (u8)bstart);
		break;
	case MP_SINGLE_CARRIER_TX:
		if (bstart != 0)
			strcat(extra, "\nStart continuous DA=ffffffffffff len=1500\n infinite=yes.");
		rtw_mp_singlecarrier_tx(padapter, (u8)bstart);
		break;
	default:
		sprintf(extra, "Error! Continuous-Tx is not on-going.");
		return -EFAULT;
	}

	if (bstart == 1 && pmp_priv->mode != MP_ON) {
		struct mp_priv *pmp_priv = &padapter->mppriv;

		if (pmp_priv->tx.stop == 0) {
			pmp_priv->tx.stop = 1;
			rtw_msleep_os(5);
		}
#ifdef CONFIG_80211N_HT
		if(padapter->registrypriv.ht_enable &&
			is_supported_ht(padapter->registrypriv.wireless_mode))
			pmp_priv->tx.attrib.ht_en = 1;
#endif
		pmp_priv->tx.stop = 0;
		pmp_priv->tx.count = 1;
		if (pmp_priv->rtw_mp_tx_method == RTW_MP_PMACT_TX) {
			pmp_priv->rtw_mp_tx_method = RTW_MP_TMACT_TX;
			rtw_set_phl_packet_tx(padapter, bstart); /* send 1 pkt for trigger HW non-pkt Tx*/
			pmp_priv->rtw_mp_tx_method = RTW_MP_PMACT_TX;
		}
		/*SetPacketTx(padapter);*/
	} else
		pmp_priv->mode = MP_ON;

	return 0;
}


int rtw_mp_tx(struct net_device *dev,
	      struct iw_request_info *info,
	      union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = &padapter->mppriv;
	PMPT_CONTEXT		pMptCtx = &(padapter->mppriv.mpt_ctx);
	char *pextra = extra;
	u32 bandwidth = 0, sg = 0, channel = 6, txpower = 40, rate = 108, ant = 0, txmode = 1, count = 0;
	u8 bStartTest = 1, status = 0;
#ifdef CONFIG_MP_VHT_HW_TX_MODE
	u8 Idx = 0, tmpU1B;
#endif
	u16 antenna = 0;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;
	RTW_INFO("extra = %s\n", extra);
#if 0	
#ifdef CONFIG_CONCURRENT_MODE
	if (!is_primary_adapter(padapter)) {
		sprintf(extra, "Error: MP mode can't support Virtual adapter, Please to use main adapter.\n");
		wrqu->data.length = strlen(extra);
		return 0;
	}
#endif

	if (strncmp(extra, "stop", 3) == 0) {
		bStartTest = 0; /* To set Stop*/
		pmp_priv->tx.stop = 1;
		sprintf(extra, "Stop continuous Tx");
		status = rtw_mp_pretx_proc(padapter, bStartTest, extra);
		wrqu->data.length = strlen(extra);
		return status;
	} else if (strncmp(extra, "count", 5) == 0) {
		if (sscanf(extra, "count=%d", &count) < 1)
			RTW_INFO("Got Count=%d]\n", count);
		pmp_priv->tx.count = count;
		return 0;
	} else if (strncmp(extra, "setting", 7) == 0) {
		_rtw_memset(extra, 0, wrqu->data.length);
		pextra += sprintf(pextra, "Current Setting :\n Channel:%d", pmp_priv->channel);
		pextra += sprintf(pextra, "\n Bandwidth:%d", pmp_priv->bandwidth);
		pextra += sprintf(pextra, "\n Rate index:%d", pmp_priv->rateidx);
		pextra += sprintf(pextra, "\n TxPower index:%d", pmp_priv->txpoweridx);
		pextra += sprintf(pextra, "\n Antenna TxPath:%d", pmp_priv->antenna_tx);
		pextra += sprintf(pextra, "\n Antenna RxPath:%d", pmp_priv->antenna_rx);
		pextra += sprintf(pextra, "\n MP Mode:%d", pmp_priv->mode);
		wrqu->data.length = strlen(extra);
		return 0;
#ifdef CONFIG_MP_VHT_HW_TX_MODE
	} else if (strncmp(extra, "pmact", 5) == 0) {
		if (strncmp(extra, "pmact=", 6) == 0) {
			_rtw_memset(&pMptCtx->PMacTxInfo, 0, sizeof(pMptCtx->PMacTxInfo));
			if (strncmp(extra, "pmact=start", 11) == 0) {
				pMptCtx->PMacTxInfo.bEnPMacTx = _TRUE;
				sprintf(extra, "Set PMac Tx Mode start\n");
			} else {
				pMptCtx->PMacTxInfo.bEnPMacTx = _FALSE;
				sprintf(extra, "Set PMac Tx Mode Stop\n");
			}
			if (pMptCtx->bldpc == TRUE)
				pMptCtx->PMacTxInfo.bLDPC = _TRUE;

			if (pMptCtx->bstbc == TRUE)
				pMptCtx->PMacTxInfo.bSTBC = _TRUE;

			pMptCtx->PMacTxInfo.bSPreamble = pmp_priv->preamble;
			pMptCtx->PMacTxInfo.bSGI = pmp_priv->preamble;
			pMptCtx->PMacTxInfo.BandWidth = pmp_priv->bandwidth;
			pMptCtx->PMacTxInfo.TX_RATE = HwRateToMPTRate(pmp_priv->rateidx);

			pMptCtx->PMacTxInfo.Mode = pMptCtx->HWTxmode;

			pMptCtx->PMacTxInfo.NDP_sound = FALSE;/*(adapter.PacketType == NDP_PKT)?TRUE:FALSE;*/

			if (padapter->mppriv.pktInterval == 0)
				pMptCtx->PMacTxInfo.PacketPeriod = 100;
			else
				pMptCtx->PMacTxInfo.PacketPeriod = padapter->mppriv.pktInterval;

			if (padapter->mppriv.pktLength < 1000)
				pMptCtx->PMacTxInfo.PacketLength = 1000;
			else
				pMptCtx->PMacTxInfo.PacketLength = padapter->mppriv.pktLength;

			pMptCtx->PMacTxInfo.PacketPattern  = rtw_random32() % 0xFF;

			if (padapter->mppriv.tx_pktcount != 0)
				pMptCtx->PMacTxInfo.PacketCount = padapter->mppriv.tx_pktcount;

			pMptCtx->PMacTxInfo.Ntx = 0;
			for (Idx = 16; Idx < 20; Idx++) {
				tmpU1B = (padapter->mppriv.antenna_tx >> Idx) & 1;
				if (tmpU1B)
					pMptCtx->PMacTxInfo.Ntx++;
			}

			_rtw_memset(pMptCtx->PMacTxInfo.MacAddress, 0xFF, ETH_ALEN);

			PMAC_Get_Pkt_Param(&pMptCtx->PMacTxInfo, &pMptCtx->PMacPktInfo);

			if (MPT_IS_CCK_RATE(pMptCtx->PMacTxInfo.TX_RATE))

				CCK_generator(&pMptCtx->PMacTxInfo, &pMptCtx->PMacPktInfo);
			else {
				PMAC_Nsym_generator(&pMptCtx->PMacTxInfo, &pMptCtx->PMacPktInfo);
				/* 24 BIT*/
				L_SIG_generator(pMptCtx->PMacPktInfo.N_sym, &pMptCtx->PMacTxInfo, &pMptCtx->PMacPktInfo);
			}
			/*	48BIT*/
			if (MPT_IS_HT_RATE(pMptCtx->PMacTxInfo.TX_RATE))
				HT_SIG_generator(&pMptCtx->PMacTxInfo, &pMptCtx->PMacPktInfo);
			else if (MPT_IS_VHT_RATE(pMptCtx->PMacTxInfo.TX_RATE)) {
				/*	48BIT*/
				VHT_SIG_A_generator(&pMptCtx->PMacTxInfo, &pMptCtx->PMacPktInfo);

				/*	26/27/29 BIT  & CRC 8 BIT*/
				VHT_SIG_B_generator(&pMptCtx->PMacTxInfo);

				/* 32 BIT*/
				VHT_Delimiter_generator(&pMptCtx->PMacTxInfo);
			}

			mpt_ProSetPMacTx(padapter);

		} else if (strncmp(extra, "pmact,mode=", 11) == 0) {
			int txmode = 0;

			if (sscanf(extra, "pmact,mode=%d", &txmode) > 0) {
				if (txmode == 1) {
					pMptCtx->HWTxmode = CONTINUOUS_TX;
					sprintf(extra, "\t Config HW Tx mode = CONTINUOUS_TX\n");
				} else if (txmode == 2) {
					pMptCtx->HWTxmode = OFDM_Single_Tone_TX;
					sprintf(extra, "\t Config HW Tx mode = OFDM_Single_Tone_TX\n");
				} else {
					pMptCtx->HWTxmode = PACKETS_TX;
					sprintf(extra, "\t Config HW Tx mode = PACKETS_TX\n");
				}
			} else {
				pMptCtx->HWTxmode = PACKETS_TX;
				sprintf(extra, "\t Config HW Tx mode=\n 0 = PACKETS_TX\n 1 = CONTINUOUS_TX\n 2 = OFDM_Single_Tone_TX");
			}
		} else if (strncmp(extra, "pmact,", 6) == 0) {
			int PacketPeriod = 0, PacketLength = 0, PacketCout = 0;
			int bldpc = 0, bstbc = 0;

			if (sscanf(extra, "pmact,period=%d", &PacketPeriod) > 0) {
				padapter->mppriv.pktInterval = PacketPeriod;
				RTW_INFO("PacketPeriod=%d\n", padapter->mppriv.pktInterval);
				sprintf(extra, "PacketPeriod [1~255]= %d\n", padapter->mppriv.pktInterval);

			} else if (sscanf(extra, "pmact,length=%d", &PacketLength) > 0) {
				padapter->mppriv.pktLength = PacketLength;
				RTW_INFO("PacketPeriod=%d\n", padapter->mppriv.pktLength);
				sprintf(extra, "PacketLength[~65535]=%d\n", padapter->mppriv.pktLength);

			} else if (sscanf(extra, "pmact,count=%d", &PacketCout) > 0) {
				padapter->mppriv.tx_pktcount = PacketCout;
				RTW_INFO("Packet Cout =%d\n", padapter->mppriv.tx_pktcount);
				sprintf(extra, "Packet Cout =%d\n", padapter->mppriv.tx_pktcount);

			} else if (sscanf(extra, "pmact,ldpc=%d", &bldpc) > 0) {
				pMptCtx->bldpc = bldpc;
				RTW_INFO("Set LDPC =%d\n", pMptCtx->bldpc);
				sprintf(extra, "Set LDPC =%d\n", pMptCtx->bldpc);

			} else if (sscanf(extra, "pmact,stbc=%d", &bstbc) > 0) {
				pMptCtx->bstbc = bstbc;
				RTW_INFO("Set STBC =%d\n", pMptCtx->bstbc);
				sprintf(extra, "Set STBC =%d\n", pMptCtx->bstbc);
			} else
				sprintf(extra, "\n period={1~255}\n length={1000~65535}\n count={0~}\n ldpc={0/1}\n stbc={0/1}");

		}

		wrqu->data.length = strlen(extra);
		return 0;
#endif
	} else {

		if (sscanf(extra, "ch=%d,bw=%d,rate=%d,pwr=%d,ant=%d,tx=%d", &channel, &bandwidth, &rate, &txpower, &ant, &txmode) < 6) {
			RTW_INFO("Invalid format [ch=%d,bw=%d,rate=%d,pwr=%d,ant=%d,tx=%d]\n", channel, bandwidth, rate, txpower, ant, txmode);
			_rtw_memset(extra, 0, wrqu->data.length);
			pextra += sprintf(pextra, "\n Please input correct format as bleow:\n");
			pextra += sprintf(pextra, "\t ch=%d,bw=%d,rate=%d,pwr=%d,ant=%d,tx=%d\n", channel, bandwidth, rate, txpower, ant, txmode);
			pextra += sprintf(pextra, "\n [ ch : BGN = <1~14> , A or AC = <36~165> ]");
			pextra += sprintf(pextra, "\n [ bw : Bandwidth: 0 = 20M, 1 = 40M, 2 = 80M ]");
			pextra += sprintf(pextra, "\n [ rate :	CCK: 1 2 5.5 11M X 2 = < 2 4 11 22 >]");
			pextra += sprintf(pextra, "\n [		OFDM: 6 9 12 18 24 36 48 54M X 2 = < 12 18 24 36 48 72 96 108>");
			pextra += sprintf(pextra, "\n [		HT 1S2SS MCS0 ~ MCS15 : < [MCS0]=128 ~ [MCS7]=135 ~ [MCS15]=143 >");
			pextra += sprintf(pextra, "\n [		HT 3SS MCS16 ~ MCS32 : < [MCS16]=144 ~ [MCS23]=151 ~ [MCS32]=159 >");
			pextra += sprintf(pextra, "\n [		VHT 1SS MCS0 ~ MCS9 : < [MCS0]=160 ~ [MCS9]=169 >");
			pextra += sprintf(pextra, "\n [ txpower : 1~63 power index");
			pextra += sprintf(pextra, "\n [ ant : <A = 1, B = 2, C = 4, D = 8> ,2T ex: AB=3 BC=6 CD=12");
			pextra += sprintf(pextra, "\n [ txmode : < 0 = CONTINUOUS_TX, 1 = PACKET_TX, 2 = SINGLE_TONE_TX, 3 = CARRIER_SUPPRISSION_TX, 4 = SINGLE_CARRIER_TX>\n");
			wrqu->data.length = strlen(extra);
			return status;

		} else {
			char *pextra = extra;
			RTW_INFO("Got format [ch=%d,bw=%d,rate=%d,pwr=%d,ant=%d,tx=%d]\n", channel, bandwidth, rate, txpower, ant, txmode);
			_rtw_memset(extra, 0, wrqu->data.length);
			sprintf(extra, "Change Current channel %d to channel %d", padapter->mppriv.channel , channel);
			padapter->mppriv.channel = channel;
			SetChannel(padapter);
			pHalData->current_channel = channel;

			if (bandwidth == 1)
				bandwidth = CHANNEL_WIDTH_40;
			else if (bandwidth == 2)
				bandwidth = CHANNEL_WIDTH_80;
			pextra = extra + strlen(pextra);
			pextra += sprintf(pextra, "\nChange Current Bandwidth %d to Bandwidth %d", padapter->mppriv.bandwidth, bandwidth);
			padapter->mppriv.bandwidth = (u8)bandwidth;
			padapter->mppriv.preamble = sg;
			SetBandwidth(padapter);
			pHalData->current_channel_bw = bandwidth;

			pextra += sprintf(pextra, "\nSet power level :%d", txpower);
			padapter->mppriv.txpoweridx = (u8)txpower;
			pMptCtx->TxPwrLevel[RF_PATH_A] = (u8)txpower;
			pMptCtx->TxPwrLevel[RF_PATH_B] = (u8)txpower;
			pMptCtx->TxPwrLevel[RF_PATH_C] = (u8)txpower;
			pMptCtx->TxPwrLevel[RF_PATH_D]  = (u8)txpower;
			SetTxPower(padapter);

			RTW_INFO("%s: bw=%d sg=%d\n", __func__, bandwidth, sg);

			if (rate <= 0x7f)
				rate = wifirate2_ratetbl_inx((u8)rate);
			else if (rate < 0xC8)
				rate = (rate - 0x80 + MPT_RATE_MCS0);
			/*HT  rate 0x80(MCS0)  ~ 0x8F(MCS15) ~ 0x9F(MCS31) 128~159
			VHT1SS~2SS rate 0xA0 (VHT1SS_MCS0 44) ~ 0xB3 (VHT2SS_MCS9 #63) 160~179
			VHT rate 0xB4 (VHT3SS_MCS0 64) ~ 0xC7 (VHT2SS_MCS9 #83) 180~199
			else
			VHT rate 0x90(VHT1SS_MCS0) ~ 0x99(VHT1SS_MCS9) 144~153
			rate =(rate - MPT_RATE_VHT1SS_MCS0);
			*/
			RTW_INFO("%s: rate index=%d\n", __func__, rate);
			if (rate >= MPT_RATE_LAST)
				return -EINVAL;
			pextra += sprintf(pextra, "\nSet data rate to %d index %d", padapter->mppriv.rateidx, rate);

			padapter->mppriv.rateidx = rate;
			pMptCtx->mpt_rate_index = rate;
			SetDataRate(padapter);

			pextra += sprintf(pextra, "\nSet Antenna Path :%d", ant);
			switch (ant) {
			case 1:
				antenna = ANTENNA_A;
				break;
			case 2:
				antenna = ANTENNA_B;
				break;
			case 4:
				antenna = ANTENNA_C;
				break;
			case 8:
				antenna = ANTENNA_D;
				break;
			case 3:
				antenna = ANTENNA_AB;
				break;
			case 5:
				antenna = ANTENNA_AC;
				break;
			case 9:
				antenna = ANTENNA_AD;
				break;
			case 6:
				antenna = ANTENNA_BC;
				break;
			case 10:
				antenna = ANTENNA_BD;
				break;
			case 12:
				antenna = ANTENNA_CD;
				break;
			case 7:
				antenna = ANTENNA_ABC;
				break;
			case 14:
				antenna = ANTENNA_BCD;
				break;
			case 11:
				antenna = ANTENNA_ABD;
				break;
			case 15:
				antenna = ANTENNA_ABCD;
				break;
			}
			RTW_INFO("%s: antenna=0x%x\n", __func__, antenna);
			padapter->mppriv.antenna_tx = antenna;
			padapter->mppriv.antenna_rx = antenna;
			pHalData->antenna_tx_path = antenna;
			SetAntenna(padapter);

			if (txmode == 0)
				pmp_priv->mode = MP_CONTINUOUS_TX;
			else if (txmode == 1) {
				pmp_priv->mode = MP_PACKET_TX;
				pmp_priv->tx.count = count;
			} else if (txmode == 2)
				pmp_priv->mode = MP_SINGLE_TONE_TX;
			else if (txmode == 3)
				pmp_priv->mode = MP_CARRIER_SUPPRISSION_TX;
			else if (txmode == 4)
				pmp_priv->mode = MP_SINGLE_CARRIER_TX;

			status = rtw_mp_pretx_proc(padapter, bStartTest, extra);
		}

	}
#endif
	wrqu->data.length = strlen(extra);
	return status;
}


int rtw_mp_rx(struct net_device *dev,
	      struct iw_request_info *info,
	      union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = &padapter->mppriv;
	char *pextra = extra;
	u32 bandwidth = 0, sg = 0, channel = 6, ant = 0;
	u16 antenna = 0;
	u8 bStartRx = 0;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;
#if 0
#ifdef CONFIG_CONCURRENT_MODE
	if (!is_primary_adapter(padapter)) {
		sprintf(extra, "Error: MP mode can't support Virtual adapter, Please to use main adapter.\n");
		wrqu->data.length = strlen(extra);
		return 0;
	}
#endif

	if (strncmp(extra, "stop", 4) == 0) {
		_rtw_memset(extra, 0, wrqu->data.length);
		SetPacketRx(padapter, bStartRx, _FALSE);
		pmp_priv->bmac_filter = _FALSE;
		sprintf(extra, "Received packet OK:%d CRC error:%d ,Filter out:%d", padapter->mppriv.rx_pktcount, padapter->mppriv.rx_crcerrpktcount, padapter->mppriv.rx_pktcount_filter_out);
		wrqu->data.length = strlen(extra);
		return 0;

	} else if (sscanf(extra, "ch=%d,bw=%d,ant=%d", &channel, &bandwidth, &ant) < 3) {
		RTW_INFO("Invalid format [ch=%d,bw=%d,ant=%d]\n", channel, bandwidth, ant);
		_rtw_memset(extra, 0, wrqu->data.length);
		pextra += sprintf(pextra, "\n Please input correct format as bleow:\n");
		pextra += sprintf(pextra, "\t ch=%d,bw=%d,ant=%d\n", channel, bandwidth, ant);
		pextra += sprintf(pextra, "\n [ ch : BGN = <1~14> , A or AC = <36~165> ]");
		pextra += sprintf(pextra, "\n [ bw : Bandwidth: 0 = 20M, 1 = 40M, 2 = 80M ]");
		pextra += sprintf(pextra, "\n [ ant : <A = 1, B = 2, C = 4, D = 8> ,2T ex: AB=3 BC=6 CD=12");
		wrqu->data.length = strlen(extra);
		return 0;

	} else {
		char *pextra = extra;
		bStartRx = 1;
		RTW_INFO("Got format [ch=%d,bw=%d,ant=%d]\n", channel, bandwidth, ant);
		_rtw_memset(extra, 0, wrqu->data.length);
		sprintf(extra, "Change Current channel %d to channel %d", padapter->mppriv.channel , channel);
		padapter->mppriv.channel = channel;
		SetChannel(padapter);
		pHalData->current_channel = channel;

		if (bandwidth == 1)
			bandwidth = CHANNEL_WIDTH_40;
		else if (bandwidth == 2)
			bandwidth = CHANNEL_WIDTH_80;
		pextra = extra + strlen(extra);
		pextra += sprintf(pextra, "\nChange Current Bandwidth %d to Bandwidth %d", padapter->mppriv.bandwidth, bandwidth);
		padapter->mppriv.bandwidth = (u8)bandwidth;
		padapter->mppriv.preamble = sg;
		SetBandwidth(padapter);
		pHalData->current_channel_bw = bandwidth;

		pextra += sprintf(pextra, "\nSet Antenna Path :%d", ant);
		switch (ant) {
		case 1:
			antenna = ANTENNA_A;
			break;
		case 2:
			antenna = ANTENNA_B;
			break;
		case 4:
			antenna = ANTENNA_C;
			break;
		case 8:
			antenna = ANTENNA_D;
			break;
		case 3:
			antenna = ANTENNA_AB;
			break;
		case 5:
			antenna = ANTENNA_AC;
			break;
		case 9:
			antenna = ANTENNA_AD;
			break;
		case 6:
			antenna = ANTENNA_BC;
			break;
		case 10:
			antenna = ANTENNA_BD;
			break;
		case 12:
			antenna = ANTENNA_CD;
			break;
		case 7:
			antenna = ANTENNA_ABC;
			break;
		case 14:
			antenna = ANTENNA_BCD;
			break;
		case 11:
			antenna = ANTENNA_ABD;
			break;
		case 15:
			antenna = ANTENNA_ABCD;
			break;
		}
		RTW_INFO("%s: antenna=0x%x\n", __func__, antenna);
		padapter->mppriv.antenna_tx = antenna;
		padapter->mppriv.antenna_rx = antenna;
		pHalData->antenna_tx_path = antenna;
		SetAntenna(padapter);

		strcat(extra, "\nstart Rx");
		SetPacketRx(padapter, bStartRx, _FALSE);
	}
#endif	
	wrqu->data.length = strlen(extra);
	return 0;
}


int rtw_mp_hwtx(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = &padapter->mppriv;
	PMPT_CONTEXT mpt_ctx = &(padapter->mppriv.mpt_ctx);

#if defined(CONFIG_RTL8821B) || defined(CONFIG_RTL8822B) || defined(CONFIG_RTL8821C) || defined(CONFIG_RTL8822C)
	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;
	*(extra + wrqu->data.length) = '\0';

	_rtw_memset(&mpt_ctx->PMacTxInfo, 0, sizeof(RT_PMAC_TX_INFO));
	_rtw_memcpy((void *)&mpt_ctx->PMacTxInfo, (void *)extra, sizeof(RT_PMAC_TX_INFO));
	_rtw_memset(extra, 0, wrqu->data.length);

	if (mpt_ctx->PMacTxInfo.bEnPMacTx == 1 && pmp_priv->mode != MP_ON) {
		sprintf(extra, "MP Tx Running, Please Set PMac Tx Mode Stop\n");
		RTW_INFO("Error !!! MP Tx Running, Please Set PMac Tx Mode Stop\n");
	} else {
		RTW_INFO("To set MAC Tx mode\n");
		mpt_ProSetPMacTx(padapter);
		sprintf(extra, "Set PMac Tx Mode OK\n");
	}
	wrqu->data.length = strlen(extra);
#endif
	return 0;

}

int rtw_mp_pwrlmt(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct registry_priv  *registry_par = &padapter->registrypriv;
	u8 pwrlimtstat = 0;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	*(extra + wrqu->data.length) = '\0';
#if CONFIG_TXPWR_LIMIT
	if (strncmp(extra, "off", 3) == 0 && strlen(extra) < 4) {
		if (rtw_mpt_set_power_limit_en(padapter, _FALSE))
			sprintf(extra, "Turn off Power Limit\n");
		else 
			sprintf(extra, "Turn off Power Limit Fail\n");
	} else if (strncmp(extra, "on", 2) == 0 && strlen(extra) < 3) {
		if (rtw_mpt_set_power_limit_en(padapter, _TRUE))
			sprintf(extra, "Turn on Power Limit\n");
		else 
			sprintf(extra, "Turn on Power Limit Fail\n");
	} else
#endif
	{
		sprintf(extra, "PHL PWRLMT:%s\n",
			(rtw_mpt_get_power_limit_en(padapter) == _TRUE) ?"ON" :"OFF");
	}
	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_mp_dpk_track(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	//struct dm_struct *phydm = adapter_to_phydm(padapter);

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	*(extra + wrqu->data.length) = '\0';

	if (strncmp(extra, "off", 3) == 0 && strlen(extra) < 4) {
		//halrf_set_dpk_track(phydm, FALSE);
		sprintf(extra, "set dpk track off\n");

	} else if (strncmp(extra, "on", 2) == 0 && strlen(extra) < 3) {
		//halrf_set_dpk_track(phydm, TRUE);
		sprintf(extra, "set dpk track on\n");
	}

	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_mp_set_phl_io(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rtw_mp_cmd_arg *cmd_arg = NULL;
	struct rtw_mp_test_cmdbuf *pcmdbuf = NULL;
	u16 i = 0;

	if (copy_from_user(extra, wrqu->pointer, wrqu->length))
			return -EFAULT;

	RTW_INFO("%s, wrqu->length %d !!!\n", __func__, wrqu->length);

	rtw_phl_test_submodule_cmd_process(dvobj->phl_com, (void*)extra, wrqu->length);
	pcmdbuf = (struct rtw_mp_test_cmdbuf *)extra;
	while (1) {
		if (pcmdbuf) {
			cmd_arg = (struct rtw_mp_cmd_arg *)pcmdbuf->buf;
			rtw_phl_test_submodule_get_rpt(dvobj->phl_com, (void *)extra, wrqu->length);
		}
		if (cmd_arg != NULL && cmd_arg->cmd_ok) {
			RTW_INFO("%s,GET CMD OK !!!\n", __func__);
			break;
		} else {
			i++;
			rtw_msleep_os(100);
			if (i == 3) {
				RTW_INFO("%s,GET CMD FAIL !!!\n", __func__);
				break;
			}
		}
	}

	if (copy_to_user(wrqu->pointer, extra, wrqu->length))
		return -EFAULT;

	return 0;
}

int rtw_mp_get_phl_io(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	if (copy_from_user(extra, wrqu->pointer, wrqu->length))
			return -EFAULT;
	*(extra + wrqu->length) = '\0';

	rtw_phl_test_submodule_get_rpt(dvobj->phl_com, (void *)&extra, wrqu->length);

	wrqu->length = strlen(extra);

	return 0;
}

int rtw_mp_tx_pattern_idx(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;

	u32 tx_patt_idx = 0;
	u32 ppdu_type = 0;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	if (sscanf(extra, "index=%d,type=%d", &tx_patt_idx, &ppdu_type) > 0) {
		RTW_INFO("%s: tx_patt_idx=%d ,ppdu_type=%d\n", __func__, tx_patt_idx , ppdu_type);
		pmp_priv->rtw_mp_pmact_patt_idx = tx_patt_idx;
		pmp_priv->rtw_mp_pmact_ppdu_type = ppdu_type;
		rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_CONFIG_PLCP_PATTERN, pmp_priv->rtw_mp_tx_method, _TRUE);
		_rtw_memset(extra, 0, wrqu->data.length);
		sprintf(extra, "Config Tx Pattern idx %d to %d", pmp_priv->rtw_mp_pmact_patt_idx, tx_patt_idx);
	} else if ((strncmp(extra, "stop", 4) == 0)) {
		rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_CONFIG_PLCP_PATTERN, pmp_priv->rtw_mp_tx_method, _FALSE);
		_rtw_memset(extra, 0, wrqu->data.length);
		sprintf(extra, "Config Tx Pattern Stop");
	} else {
		u8 *pstr = extra;
		_rtw_memset(pstr, 0, wrqu->data.length);
	}
	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_mp_tx_plcp_tx_data(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8 user_idx = pmp_priv->mp_plcp_useridx;


	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	if ((strncmp(extra, "ppdu", 4) == 0)) {
		u32 ppdu_type = 0;

		if (sscanf(extra, "ppdu=%d", &ppdu_type) > 0) {
				u8 *pextra = extra;

				RTW_INFO("%s: ppdu_type=%d\n", __func__, ppdu_type);
				_rtw_memset(extra, 0, wrqu->data.length);

				pextra += sprintf(pextra, "Config PPDU Type %s to %s\n", 
						PPDU_TYPE_STR(pmp_priv->rtw_mp_pmact_ppdu_type), PPDU_TYPE_STR(ppdu_type));
				pmp_priv->rtw_mp_pmact_ppdu_type = ppdu_type;

				rtw_update_giltf(padapter);
				rtw_mp_update_coding(padapter);

				if (ppdu_type >= RTW_MP_TYPE_HE_MU_OFDMA) {
					u8 ru_num = 0 , rualloc_num = 0 ,i = 0;

					ru_num = rtw_mp_update_ru_tone(padapter);
					rualloc_num = rtw_mp_update_ru_alloc(padapter);
					pextra += sprintf(pextra, "\nCurrent [%s] RU Alloc index:%d\n",
												RU_TONE_STR(pmp_priv->rtw_mp_ru_tone),
												pmp_priv->mp_plcp_user[user_idx].ru_alloc);

					pextra += sprintf(pextra, "RU Alloc list:[");
					for (i = 0;i <= rualloc_num - 1; i++)
						pextra += sprintf(pextra, "%d ", pmp_priv->ru_alloc_list[i]);
					pextra += sprintf(pextra, "]\n");

					pextra += sprintf(pextra, "\nRU Tone support list(Refer Coding:%s):\n",
												(pmp_priv->mp_plcp_user[user_idx].coding ? "LDPC":"BCC"));
					for (i = 0;i <= ru_num; i++)
						pextra += sprintf(pextra, "%d : [%s]\n",
												pmp_priv->ru_tone_sel_list[i],
												RU_TONE_STR(pmp_priv->ru_tone_sel_list[i]));
					pextra += sprintf(pextra, "\n\nCodingCMD:[mp_plcp_user coding=%%d] (0:BCC 1:LDPC )");
					pextra += sprintf(pextra, "\nRU Tone CMD:[ mp_plcp_user ru_tone=%%d ]");
					pextra += sprintf(pextra, "\nRU Alloc CMD:[ mp_plcp_user ru_alloc=%%d ]");
				}
		} else {
			u8 *pstr = extra;
			_rtw_memset(pstr, 0, wrqu->data.length);
			pstr += sprintf(pstr, "CMD: [mp_plcp_datappdu=%%d]\nPLCP (PPDU Type):\n\
0:CCK\n1:LEGACY\n2:HT_MF\n3:HT_GF\n4:VHT\n5:HE_SU\n6:HE_ER_SU\n7:HE_MU_OFDMA\n8:HE_TB\n");
			}
	}else if ((strncmp(extra, "preamble", 8) == 0)) {
		u8 preamble = 0;

		if (sscanf(extra, "preamble=%hhd", &preamble) > 0) {
			RTW_INFO("%s: preamble=%d\n", __func__, preamble);
			_rtw_memset(extra, 0, wrqu->data.length);

			if (rtw_mp_is_cck_rate(pmp_priv->rateidx)) {
				pmp_priv->preamble = preamble;
				sprintf(extra, "Config Preamble %d to %d", pmp_priv->preamble, preamble);
			} else
				sprintf(extra, "Error !!! only B mode Rate for Preamble!\n");
		} else
			sprintf(extra, "Error format ! input 'preamble=[num]'\n");

	} else if ((strncmp(extra, "stbc", 4) == 0)) {
		u8 stbc = 0;

		if (sscanf(extra, "stbc=%hhd", &stbc) > 0) {
				RTW_INFO("%s: stbc=%d\n", __func__, stbc);
				_rtw_memset(extra, 0, wrqu->data.length);
				sprintf(extra, "Config STBC enable: %d to %d", pmp_priv->rtw_mp_stbc, stbc);
				pmp_priv->rtw_mp_stbc = stbc;
				if (pmp_priv->rtw_mp_stbc)
					pmp_priv->mp_plcp_user[user_idx].dcm = 0;
		} else {
			_rtw_memset(extra, 0, wrqu->data.length);
			sprintf(extra, "input [stbc=0/1]");
		}

	} else if ((strncmp(extra, "giltf", 5) == 0)) {
		u8 idx = 0;
		u8 giltf_num = rtw_update_giltf(padapter);

		if (sscanf(extra, "giltf=%hhd", &idx) > 0) {
			u8 gi = 0, ltf = 0;

			RTW_INFO("%s: gi+ltf=%d\n", __func__, idx);
			_rtw_memset(extra, 0, wrqu->data.length);

			if (giltf_num != 0 && idx <= giltf_num) {
				gi = pmp_priv->st_giltf[idx].gi;
				ltf = pmp_priv->st_giltf[idx].ltf;
				sprintf(extra, "Config GI+LTF to %s ", pmp_priv->st_giltf[idx].type_str);
				pmp_priv->rtw_mp_plcp_gi = gi;
				pmp_priv->rtw_mp_plcp_ltf = ltf;
				RTW_INFO("%s: gi=%d ltf=%d\n", __func__, gi, ltf);
			} else
				sprintf(extra, "Not support GI+LTF index\n");

		} else {
			u8 *pextra = extra;
			u8 i = 0;

			if (giltf_num > 0) {
				pextra += sprintf(pextra, "GI + LTF list:\n");
				for (i = 0;i <= giltf_num; i++)
							pextra += sprintf(pextra, "%d:[%s]\n", i, pmp_priv->st_giltf[i].type_str);
			}
			sprintf(pextra, "PPDU Type Not support GI+LTF.");
		}

	} else if ((strncmp(extra, "tx_time", 7) == 0)) {
			u32 tx_time = 0;

			if (sscanf(extra, "tx_time=%d", &tx_time) > 0) {
				u32 tmp_tx_time = (tx_time * 10) / 4;

				pmp_priv->rtw_mp_plcp_tx_time = tmp_tx_time;
				pmp_priv->rtw_mp_plcp_tx_mode = 1;
				sprintf(extra, "Config Tx Time:%d us", pmp_priv->rtw_mp_plcp_tx_time);
			}

	} else if ((strncmp(extra, "tx_len", 6) == 0)) {
			u32 tx_len = 0;

			if (sscanf(extra, "tx_len=%d", &tx_len) > 0) {
				pmp_priv->mp_plcp_user[user_idx].plcp_txlen = tx_len;
				pmp_priv->rtw_mp_plcp_tx_mode = 0;
				sprintf(extra, "Config Tx Len:%d", pmp_priv->mp_plcp_user[user_idx].plcp_txlen);
		}

	} else if ((strncmp(extra, "he_sigb", 7) == 0)) {
			u32 he_sigb = 0;

			if (sscanf(extra, "he_sigb=%d", &he_sigb) > 0) {
				if (he_sigb <= 5) {
					pmp_priv->rtw_mp_he_sigb = he_sigb;
					sprintf(extra, "Config HE SIGB:%d", he_sigb);
				} else
					sprintf(extra, "Error Config HE SIGB:[%d] (0~5)", he_sigb);
			} else {
				u8 *pstr = extra;

				_rtw_memset(pstr, 0, wrqu->data.length);
				pstr += sprintf(pstr, "invalid CMD Format! input: he_sigb=[Num]\n\
					PLCP (HE SIGB):\n0\n\1\n2\n\3\n\4\n\5\n");
			}

	} else if ((strncmp(extra, "he_sigb_dcm", 7) == 0)) {
			u32 he_sigb_dcm = 0;

			if (sscanf(extra, "he_sigb_dcm=%d", &he_sigb_dcm) > 0) {
				if (he_sigb_dcm <= 1) {
					pmp_priv->rtw_mp_he_sigb_dcm = he_sigb_dcm;
					sprintf(extra, "Config HE SIGB DCM:%d", he_sigb_dcm);
				} else
					sprintf(extra, "Error Config HE SIGB:[%d] (0:Disable 1:Enable)", he_sigb_dcm);

			} else {
				u8 *pstr = extra;

				_rtw_memset(pstr, 0, wrqu->data.length);
				pstr += sprintf(pstr, "invalid CMD Format! input: he_sigb_dcm=[Num]\n\
					PLCP (HE SIGB DCM):\n0:Disable\n1:Enable");
			}

	} else if ((strncmp(extra, "er_su_ru106en", 7) == 0)) {
			u32 ru106en = 0;

			if (sscanf(extra, "er_su_ru106en=%d", &ru106en) > 0) {
				if (ru106en <= 1) {
					pmp_priv->rtw_mp_he_er_su_ru_106_en = ru106en;
					sprintf(extra, "Config he_er_su_ru106:%d", ru106en);
				} else
					sprintf(extra, "Error!!! Config HE ER SU RU106 Enable:[%d] (0:Disable 1:Enable)",
										ru106en);
			} else {
				u8 *pstr = extra;

				_rtw_memset(pstr, 0, wrqu->data.length);
				pstr += sprintf(pstr, "Error!!! Config HE ER SU RU106 Enable: Input number[ 0:Disable 1:Enable ]");
			}

	} else if ((strncmp(extra, "ru_tone", 7) == 0)) {
		u32 ru_tone = 0;

		if (sscanf(extra, "ru_tone=%d", &ru_tone) > 0) {
				RTW_INFO("%s: RU Tone=%d\n", __func__, ru_tone);
				_rtw_memset(extra, 0, wrqu->data.length);
				sprintf(extra, "Config RU tone %d to %d", pmp_priv->rtw_mp_ru_tone, ru_tone);
				pmp_priv->rtw_mp_ru_tone = ru_tone;
		} else {
			_rtw_memset(extra, 0, wrqu->data.length);
			sprintf(extra, "Error!!!\tinput , [ru_tone= number]");
		}

	} else {
				char *pstr = extra;
				u8 ppdu_idx = pmp_priv->rtw_mp_pmact_ppdu_type;

				if (ppdu_idx < RTW_MP_TYPE_HT_MF) {
						pstr += sprintf(pstr, "invalid PPDU Type ! input :ppdu=[Num] over the HT\n");
				} else {
					u8 i = 0;
					u8 num = rtw_update_giltf(padapter);

					_rtw_memset(extra, 0, wrqu->data.length);
					pstr += sprintf(pstr, "invalid CMD Format !! please input: giltf=[Num]\n");
					pstr += sprintf(pstr, "PPDU %s GI+LTF:\n", PPDU_TYPE_STR(ppdu_idx));
					for (i = 0; i <= num; i++)
						pstr += sprintf(pstr, "[%d]: %s\n" ,i , pmp_priv->st_giltf[i].type_str);
				}
		}
	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_mp_tx_plcp_tx_user(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *mpprv = (struct mp_priv *)&padapter->mppriv;

	u32 tx_mcs = 0;
	u8 user_idx = mpprv->mp_plcp_useridx;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	if (sscanf(extra, "mcs=%d", &tx_mcs) > 0) {
		RTW_INFO("%s: mcs=%d\n", __func__, tx_mcs);
		mpprv->mp_plcp_user[user_idx].plcp_mcs = tx_mcs;
		_rtw_memset(extra, 0, wrqu->data.length);
		sprintf(extra, "Config PLCP MCS idx %d to %d",
						mpprv->mp_plcp_user[user_idx].plcp_mcs, tx_mcs);

	} else if ((strncmp(extra, "dcm", 3) == 0)) {
		u8 dcm = 0;

		if (sscanf(extra, "dcm=%hhd", &dcm) > 0) {
				RTW_INFO("%s: dcm=%d\n", __func__, dcm);
				_rtw_memset(extra, 0, wrqu->data.length);
				sprintf(extra, "Config DCM enable: %d to %d", mpprv->mp_plcp_user[user_idx].dcm, dcm);
				mpprv->mp_plcp_user[user_idx].dcm = dcm;

				if (mpprv->mp_plcp_user[user_idx].dcm)
						mpprv->rtw_mp_stbc = 0;
		} else {
			_rtw_memset(extra, 0, wrqu->data.length);
			sprintf(extra, "Error !!! input [dcm=0/1]");
		}

	} else if ((strncmp(extra, "coding", 6) == 0)) {
		u8 coding = 0;

		if (sscanf(extra, "coding=%hhd", &coding) > 0) {

				RTW_INFO("%s: coding=%d\n", __func__, coding);
				_rtw_memset(extra, 0, wrqu->data.length);
				mpprv->mp_plcp_user[user_idx].coding = coding;
				rtw_mp_update_coding(padapter);
				sprintf(extra, "Config coding to %s",
								(mpprv->mp_plcp_user[user_idx].coding?"LDPC":"BCC"));
		} else {
			_rtw_memset(extra, 0, wrqu->data.length);
			sprintf(extra, "Error !!!\n0:BCC 1:LDPC \t input Number [coding=0/1]");
		}

	} else if ((strncmp(extra, "ru_alloc", 8) == 0)) {
		u32 ru_alloc = 0;

		if (sscanf(extra, "ru_alloc=%d", &ru_alloc) > 0) {

				RTW_INFO("%s: RU alloc=%d\n", __func__, ru_alloc);
				_rtw_memset(extra, 0, wrqu->data.length);
				sprintf(extra, "Config RU alloc %d to %d",
								mpprv->mp_plcp_user[user_idx].ru_alloc, ru_alloc);
				mpprv->mp_plcp_user[user_idx].ru_alloc = ru_alloc;
		} else {
			_rtw_memset(extra, 0, wrqu->data.length);
			sprintf(extra, "Error!!!\tinput , [ru_alloc= number]");
		}

	} else if ((strncmp(extra, "txuser", 6) == 0)) {
		u32 txuser = 0;

		if (sscanf(extra, "txuser=%d", &txuser) > 0) {
				RTW_INFO("%s: Sel User idx=%d\n", __func__, txuser);
				_rtw_memset(extra, 0, wrqu->data.length);
				sprintf(extra, "config Tx User %d to %d", mpprv->rtw_mp_plcp_tx_user, txuser);
				mpprv->rtw_mp_plcp_tx_user = txuser;
		} else {
			_rtw_memset(extra, 0, wrqu->data.length);
			sprintf(extra, "Error!!!\tinput , [txuser= number]");
		}

	} else if ((strncmp(extra, "user", 4) == 0)) {
		u32 user_idx = 0;

		if (sscanf(extra, "user=%d", &user_idx) > 0) {
				RTW_INFO("%s: Sel User idx=%d\n", __func__, user_idx);
				_rtw_memset(extra, 0, wrqu->data.length);
				sprintf(extra, "select User idx %d to %d", mpprv->mp_plcp_useridx, user_idx);
				mpprv->mp_plcp_useridx = user_idx;
		} else {
			_rtw_memset(extra, 0, wrqu->data.length);
			sprintf(extra, "Error!!!\tinput , [user= number]");
		}

	} else if ((strncmp(extra, "tx_len", 6) == 0)) {
			u32 tx_len = 0;

			if (sscanf(extra, "tx_len=%d", &tx_len) > 0) {
				mpprv->mp_plcp_user[user_idx].plcp_txlen = tx_len;
				mpprv->rtw_mp_plcp_tx_mode = 0;
				sprintf(extra, "Config Tx Len:%d", mpprv->mp_plcp_user[user_idx].plcp_txlen);
		}

	} else {
		u8 *pstr = extra;
		_rtw_memset(pstr, 0, wrqu->data.length);

		pstr += sprintf(pstr, "invalid CMD Format!\n \
		\t input :\n\
		\t user=%%d\n\
		\t mcs=%%d\n\
		\t dcm=%%d,\n\
		\t coding=%%d\n\
		\t ru_alloc=%%d\n");
	}
	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_mp_tx_method(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	if ((strncmp(extra, "PMACT", 5) == 0)) {
		pmp_priv->rtw_mp_tx_method = RTW_MP_PMACT_TX;
		sprintf(extra, "set PMACT OK");
	} else if ((strncmp(extra, "TMACT", 5) == 0)) {
		pmp_priv->rtw_mp_tx_method = RTW_MP_TMACT_TX;
		rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_MODE_SWITCH, pmp_priv->rtw_mp_tx_method, _FALSE);
		sprintf(extra, "set TMACT OK");
	} else if ((strncmp(extra, "FWPMACT", 7) == 0)) {
		pmp_priv->rtw_mp_tx_method = RTW_MP_FW_PMACT_TX;
		rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_MODE_SWITCH, pmp_priv->rtw_mp_tx_method, _FALSE);
		sprintf(extra, "set FWPMACT OK");
	}
	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_mp_config_phy(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8	set_phy = 0;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	extra[wrqu->data.length] = '\0';
	set_phy = rtw_atoi(extra);

	if (set_phy < 2) {
		sprintf(extra, "set current phy %d to %d", pmp_priv->rtw_mp_cur_phy, set_phy);
		pmp_priv->rtw_mp_cur_phy = set_phy;
		rtw_mp_phl_config_arg(padapter, RTW_MP_CONFIG_CMD_SET_PHY_INDEX);
	} else
		sprintf(extra, "Not suuport phy %d", set_phy);

	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_mp_phl_rfk(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8 k_type = RTW_MP_CAL_MAX;
	u8 k_cap_ctrl = false;
	u8 k_cap_on = false;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	if (strncmp(extra, "iqk", 3) == 0) {
		k_type = RTW_MP_CAL_IQK;

		if (strncmp(extra, "iqk on", 6) == 0) {
			k_cap_ctrl = true;
			k_cap_on = true;
			sprintf(extra, "set iqk on");

		} else if ((strncmp(extra, "iqk off", 7) == 0)) {
			k_cap_ctrl = true;
			k_cap_on = false;
			sprintf(extra, "set iqk off");
		} else
			sprintf(extra, "set iqk trigger");

	} else if (strncmp(extra, "dpk", 3) == 0) {
		k_type = RTW_MP_CAL_DPK;
		if (strncmp(extra, "dpk on", 6) == 0) {
			k_cap_ctrl = true;
			k_cap_on = true;
			sprintf(extra, "set dpk on");
		} else if ((strncmp(extra, "dpk off", 7) == 0)) {
			k_cap_ctrl = true;
			k_cap_on = false;
			sprintf(extra, "set dpk off");
		} else
			sprintf(extra, "set dpk trigger");

	} else if (strncmp(extra, "chk", 3) == 0) {
		k_type = RTW_MP_CAL_CHL_RFK;
		if (strncmp(extra, "chk on", 6) == 0) {
			k_cap_ctrl = true;
			k_cap_on = true;
			sprintf(extra, "set chk on");
		} else if ((strncmp(extra, "chk off", 7) == 0)) {
			k_cap_ctrl = true;
			k_cap_on = false;
			sprintf(extra, "set chk off");
		} else
			sprintf(extra, "set chk trigger");

	} else if (strncmp(extra, "dack", 3) == 0) {
		k_type = RTW_MP_CAL_DACK;
		if (strncmp(extra, "dack on", 6) == 0) {
			k_cap_ctrl = true;
			k_cap_on = true;
			sprintf(extra, "set dack on");
		} else if ((strncmp(extra, "dack off", 7) == 0)) {
			k_cap_ctrl = true;
			k_cap_on = false;
			sprintf(extra, "set dack off");
		} else
			sprintf(extra, "set dack trigger");

	} else if (strncmp(extra, "lck", 3) == 0) {
		k_type = RTW_MP_CAL_LCK;
		if (strncmp(extra, "lck on", 6) == 0) {
			k_cap_ctrl = true;
			k_cap_on = true;
			sprintf(extra, "set lck on");
		} else if ((strncmp(extra, "lck off", 7) == 0)) {
			k_cap_ctrl = true;
			k_cap_on = false;
			sprintf(extra, "set lck off");
		} else
			sprintf(extra, "set lck trigger");

	} else if (strncmp(extra, "dpk_trk", 7) == 0) {
		k_type = RTW_MP_CAL_DPK_TRACK;
		if (strncmp(extra, "dpk_trk on", 10) == 0) {
			k_cap_ctrl = true;
			k_cap_on = true;
			sprintf(extra, "set dpk_trk on");
		} else if ((strncmp(extra, "dpk_trk off", 11) == 0)) {
			k_cap_ctrl = true;
			k_cap_on = false;
			sprintf(extra, "set dpk_trk off");
		}

	} else if (strncmp(extra, "tssi", 4) == 0) {
		k_type = RTW_MP_CAL_TSSI;
		if (strncmp(extra, "tssi on", 7) == 0) {
			k_cap_ctrl = true;
			k_cap_on = true;
			sprintf(extra, "set tssi on");
		} else if ((strncmp(extra, "tssi off", 8) == 0)) {
			k_cap_ctrl = true;
			k_cap_on = false;
			sprintf(extra, "set tssi off");
		} else
			sprintf(extra, "set tssi trigger");

	} else if (strncmp(extra, "gapk", 4) == 0) {
		k_type = RTW_MP_CAL_GAPK;
		if (strncmp(extra, "gapk on", 7) == 0) {
			k_cap_ctrl = true;
			k_cap_on = true;
			sprintf(extra, "set gapk on");
		} else if ((strncmp(extra, "gapk off", 8) == 0)) {
			k_cap_ctrl = true;
			k_cap_on = false;
			sprintf(extra, "set gapk off");
		} else
			sprintf(extra, "set gapk trigger");

	} else
		sprintf(extra, "Error! CMD Format:\n\
				[trigger K] or Set K on/off\n\
				chk\\chk on/off\n\
				dack\\dack on/off\n\
				iqk\\iqk on/off\n\
				lck\\lck on/off\n\
				dpk\\dpk on/off\n\
				dpk_trk on/off\n\
				tssi\\tssi on/off\n\
				gapk\\gapk on/off\n");

	if (k_cap_ctrl) {
		rtw_mp_cal_capab(padapter, k_type, k_cap_on);
	} else if (k_type < RTW_MP_CAL_MAX)
		rtw_mp_cal_trigger(padapter, k_type);

	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_mp_phl_btc_path(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8 btc_mode = 0;

	if (strncmp(extra, "normal", 6) == 0) {

		btc_mode = BTC_MODE_NORMAL;
		RTW_INFO("set BTC Path Normal");
	} else if (strncmp(extra, "wl", 2) == 0 || strncmp(extra, "WL", 2) == 0) {

		btc_mode = BTC_MODE_WL;
		RTW_INFO("set BTC Path WL");
	} else if (strncmp(extra, "bt", 2) == 0 || strncmp(extra, "BT", 2) == 0) {

		btc_mode = BTC_MODE_BT;
		RTW_INFO("set BTC Path BT");
	} else {
		btc_mode = BTC_MODE_WL;
		RTW_INFO("Default set BTC Path WL");
	}

	pmp_priv->btc_path = btc_mode;
	if (rtw_mp_phl_config_arg(padapter, RTW_MP_CONFIG_CMD_SWITCH_BT_PATH)) {
		sprintf(extra, "set BTC Path %s",
				(btc_mode == 0)? "Normal":((btc_mode == 1)? "WL":
				((btc_mode == 2)? "BT":"DEFAULT WL")));

	} else
		sprintf(extra, "set BTC Path Fail");
	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_mp_get_he(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct registry_priv *regsty = &padapter->registrypriv;

#ifdef CONFIG_80211AX_HE
	if (!REGSTY_IS_11AX_ENABLE(regsty) ||
		!is_supported_he(regsty->wireless_mode))
		sprintf(extra, "false");
	 else
		sprintf(extra, "true");
#endif

	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_mp_band(struct net_device *dev,
			 struct iw_request_info *info,
			 union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct registry_priv *regsty = &padapter->registrypriv;
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8 band = 0;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	extra[wrqu->data.length] = '\0';
	band = rtw_atoi(extra);

	if (rtw_hw_is_band_support(adapter_to_dvobj(padapter), band)) {
		pmp_priv->band = band;
		sprintf(extra, "Set band to %s", band == 0 ? "2.4GHz" : (band == 1 ? "5GHz" : "6GHz"));
	} else
		sprintf(extra, "band not supported");

	wrqu->data.length = strlen(extra);
	return 0;

}

static inline void dump_buf(u8 *buf, u32 len)
{
	u32 i;

	RTW_INFO("-----------------Len %d----------------\n", len);
	for (i = 0; i < len; i++)
		RTW_INFO("%2.2x-", *(buf + i));
	RTW_INFO("\n");
}

int rtw_mp_link(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_point *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv;
	char	input[RTW_IWD_MAX_LEN];
	int		bgetrxdata = 0, btxdata = 0, bsetbt = 0;
	int err = 0;
	u32 i = 0, datalen = 0,jj, kk, waittime = 0;
	u16 val = 0x00, ret = 0;
	char *pextra = NULL;
	u8 *setdata = NULL;
	char *pch, *ptmp, *token, *tmp[4] = {0x00, 0x00, 0x00};

	pmp_priv = &padapter->mppriv;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	_rtw_memset(extra, 0, wrqu->length);

	RTW_INFO("%s: in=%s\n", __func__, input);

	bgetrxdata =  (strncmp(input, "rxdata", 6) == 0) ? 1 : 0; /* strncmp TRUE is 0*/
	btxdata =  (strncmp(input, "txdata", 6) == 0) ? 1 : 0; /* strncmp TRUE is 0*/
	bsetbt =  (strncmp(input, "setbt", 5) == 0) ? 1 : 0; /* strncmp TRUE is 0*/

	if (bgetrxdata) {
		RTW_INFO("%s: in= 1 \n", __func__);
		if (pmp_priv->mplink_brx == _TRUE) {

				while (waittime < 100 && pmp_priv->mplink_brx == _FALSE) {
						if (pmp_priv->mplink_brx == _FALSE)
							rtw_msleep_os(10);
						else
							break;
						waittime++;
				}
				if (pmp_priv->mplink_brx == _TRUE) {
					sprintf(extra, "\n");
					pextra = extra + strlen(extra);
					for (i = 0; i < pmp_priv->mplink_rx_len; i ++) {
						pextra += sprintf(pextra, "%02x:", pmp_priv->mplink_buf[i]);
					}
					_rtw_memset(pmp_priv->mplink_buf, '\0' , sizeof(pmp_priv->mplink_buf));
					pmp_priv->mplink_brx = _FALSE;
				}
		}
	} else if (btxdata) {
		struct pkt_attrib *pattrib;

		pch = input;
		setdata = rtw_zmalloc(1024);
		if (setdata == NULL) {
			err = -ENOMEM;
			goto exit;
		}

		i = 0;
		while ((token = strsep(&pch, ",")) != NULL) {
			if (i > 2)
				break;
			tmp[i] = token;
			i++;
		}

		/* tmp[0],[1],[2] */
		/* txdata,00e04c871200........... */
		if (strcmp(tmp[0], "txdata") == 0) {
			if (tmp[1] == NULL) {
				err = -EINVAL;
				goto exit;
			}
		}

		datalen = strlen(tmp[1]);
		if (datalen % 2) {
			err = -EINVAL;
			goto exit;
		}
		datalen /= 2;
		if (datalen == 0) {
			err = -EINVAL;
			goto exit;
		}

		RTW_INFO("%s: data len=%d\n", __FUNCTION__, datalen);
		RTW_INFO("%s: tx data=%s\n", __FUNCTION__, tmp[1]);

		for (jj = 0, kk = 0; jj < datalen; jj++, kk += 2)
			setdata[jj] = key_2char2num(tmp[1][kk], tmp[1][kk + 1]);

		dump_buf(setdata, datalen);
		_rtw_memset(pmp_priv->mplink_buf, '\0' , sizeof(pmp_priv->mplink_buf));
		_rtw_memcpy(pmp_priv->mplink_buf, setdata, datalen);

		pattrib = &pmp_priv->tx.attrib;
		pattrib->pktlen = datalen;
		pmp_priv->tx.count = 1;
		pmp_priv->tx.stop = 0;
		pmp_priv->mplink_btx = _TRUE;
		rtw_mp_set_packet_tx(padapter);
		pmp_priv->mode = MP_PACKET_TX;

	} else if (bsetbt) {

#if 0
#ifdef CONFIG_BTC
		pch = input;
		i = 0;

		while ((token = strsep(&pch, ",")) != NULL) {
			if (i > 3)
				break;
			tmp[i] = token;
			i++;
		}

		if (tmp[1] == NULL) {
			err = -EINVAL;
			goto exit;
		}

		if (strcmp(tmp[1], "scbd") == 0) {
			u16 org_val = 0x8002, pre_val, read_score_board_val;
			u8 state;

			pre_val = (rtw_read16(padapter,(0xaa))) & 0x7fff;

			if (tmp[2] != NULL) {
				state = simple_strtoul(tmp[2], &ptmp, 10);

				if (state)
						org_val = org_val | BIT6;
				else
						org_val = org_val & (~BIT6);

				if (org_val != pre_val) {
					pre_val = org_val;
					rtw_write16(padapter, 0xaa, org_val);
					RTW_INFO("%s,setbt scbd write org_val = 0x%x , pre_val = 0x%x\n", __func__, org_val, pre_val);
				} else {
					RTW_INFO("%s,setbt scbd org_val = 0x%x ,pre_val = 0x%x\n", __func__, org_val, pre_val);
				}
			} else {
					read_score_board_val = (rtw_read16(padapter,(0xaa))) & 0x7fff;
					RTW_INFO("%s,read_score_board_val = 0x%x\n", __func__, read_score_board_val);
			}
			goto exit;

		} else if (strcmp(tmp[1], "testmode") == 0) {

			if (tmp[2] == NULL) {
				err = -EINVAL;
				goto exit;
			}

			val = simple_strtoul(tmp[2], &ptmp, 16);
			RTW_INFO("get tmp, type  %s, val =0x%x!\n", tmp[1], val);

			if (tmp[2] != NULL) {
				_rtw_memset(extra, 0, wrqu->length);
				ret = rtw_btcoex_btset_testmode(padapter, val);
				if (!CHECK_STATUS_CODE_FROM_BT_MP_OPER_RET(ret, BT_STATUS_BT_OP_SUCCESS)) {
					RTW_INFO("%s: BT_OP fail = 0x%x!\n", __FUNCTION__, val);
					sprintf(extra, "BT_OP fail  0x%x!\n", val);
				} else
					sprintf(extra, "Set BT_OP 0x%x done!\n", val);
			}

		}
#endif /* CONFIG_BTC */
#endif
	}

exit:
	if (setdata)
		rtw_mfree(setdata, 1024);

	wrqu->length = strlen(extra);
	return err;

}


int rtw_priv_mp_get(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wdata, char *extra)
{

	struct iw_point *wrqu = (struct iw_point *)wdata;
	u32 subcmd = wrqu->flags;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	int status = 0;
	u8 i = 0;
	char *pch = extra;

	if (!is_primary_adapter(padapter)) {
		RTW_INFO("MP mode only primary Adapter support, iface id = %d,\n", padapter->iface_id);
		RTW_INFO("***Please use Primary Adapter:["ADPT_FMT"]***\n", ADPT_ARG(GET_PRIMARY_ADAPTER(padapter)));
		pch +=sprintf(pch, "Only primary Adapter support MP CMD\n");
		pch +=sprintf(pch, "Please use Primary Adapter:"ADPT_FMT"", ADPT_ARG(GET_PRIMARY_ADAPTER(padapter)));
		wrqu->length = strlen(extra);
		return status;
	}
	for (i = 0; i < dvobj->iface_nums; i++) {
		_adapter *iface = dvobj->padapters[i];

		if (iface == NULL)
			continue;
		if (rtw_is_adapter_up(iface) == _FALSE)
			continue;
		if (MLME_IS_AP(iface)) {
			RTW_INFO("Adapter:["ADPT_FMT"], Please Leave AP mode or Down Interface\n", ADPT_ARG(iface));
			pch += sprintf(pch, "Check Adapter:"ADPT_FMT",\n\
								Please Leave AP mode or Down Interface\n", ADPT_ARG(iface));
			wrqu->length = strlen(extra);
			return status;
		}
	}

	RTW_INFO("%s mutx in %d\n", __func__, subcmd);

	switch (subcmd) {
	case MP_START:
		RTW_INFO("set case mp_start\n");
		status = rtw_mp_start(dev, info, wrqu, extra);
		break;
	case MP_STOP:
		RTW_INFO("set case mp_stop\n");
		status = rtw_mp_stop(dev, info, wrqu, extra);
		break;
	case MP_BANDWIDTH:
		RTW_INFO("set case mp_bandwidth\n");
		status = rtw_mp_bandwidth(dev, info, wrqu, extra);
		break;
	case MP_RESET_STATS:
		RTW_INFO("set case MP_RESET_STATS\n");
		status = rtw_mp_reset_stats(dev, info, wrqu, extra);
		break;
	case MP_SetRFPathSwh:
		RTW_INFO("set MP_SetRFPathSwitch\n");
		status = rtw_mp_SetRFPath(dev, info, wrqu, extra);
		break;
	case WRITE_REG:
		status = rtw_mp_write_reg(dev, info, wrqu, extra);
		break;
	case WRITE_RF:
		status = rtw_mp_write_rf(dev, info, wrqu, extra);
		break;
	case MP_PHYPARA:
		RTW_INFO("mp_get  MP_PHYPARA\n");
		status = rtw_mp_phypara(dev, info, wrqu, extra);
		break;
	case MP_CHANNEL:
		RTW_INFO("set case mp_channel\n");
		status = rtw_mp_channel(dev , info, wrqu, extra);
		break;
	case MP_TRXSC_OFFSET:
		RTW_INFO("set case rtw_mp_trxsc_offset\n");
		status = rtw_mp_trxsc_offset(dev , info, wrqu, extra);
		break;
	case READ_REG:
		RTW_INFO("mp_get  READ_REG\n");
		status = rtw_mp_read_reg(dev, info, wrqu, extra);
		break;
	case READ_RF:
		RTW_INFO("mp_get  READ_RF\n");
		status = rtw_mp_read_rf(dev, info, wrqu, extra);
		break;
	case MP_RATE:
		RTW_INFO("set case mp_rate\n");
		status = rtw_mp_rate(dev, info, wrqu, extra);
		break;
	case MP_TXPOWER:
		RTW_INFO("set case MP_TXPOWER\n");
		status = rtw_mp_txpower(dev, info, wrqu, extra);
		break;
	case MP_ANT_TX:
		RTW_INFO("set case MP_ANT_TX\n");
		status = rtw_mp_ant_tx(dev, info, wrqu, extra);
		break;
	case MP_ANT_RX:
		RTW_INFO("set case MP_ANT_RX\n");
		status = rtw_mp_ant_rx(dev, info, wrqu, extra);
		break;
	case MP_QUERY:
		status = rtw_mp_trx_query(dev, info, wrqu, extra);
		break;
	case MP_CTX:
		RTW_INFO("set case MP_CTX\n");
		status = rtw_mp_ctx(dev, info, wrqu, extra);
		break;
	case MP_ARX:
		RTW_INFO("set case MP_ARX\n");
		status = rtw_mp_arx(dev, info, wrqu, extra);
		break;
	case MP_DUMP:
		RTW_INFO("set case MP_DUMP\n");
		status = rtw_mp_dump(dev, info, wrqu, extra);
		break;
	case MP_PSD:
		RTW_INFO("set case MP_PSD\n");
		status = rtw_mp_psd(dev, info, wrqu, extra);
		break;
	case MP_THER:
		RTW_INFO("set case MP_THER\n");
		status = rtw_mp_thermal(dev, info, wrqu, extra);
		break;
	case MP_PwrCtlDM:
		RTW_INFO("set MP_PwrCtlDM\n");
		status = rtw_mp_PwrCtlDM(dev, info, wrqu, extra);
		break;
	case MP_QueryDrvStats:
		RTW_INFO("mp_get MP_QueryDrvStats\n");
		status = rtw_mp_QueryDrv(dev, info, wdata, extra);
		break;
	case MP_PWRTRK:
		RTW_INFO("set case MP_PWRTRK\n");
		status = rtw_mp_pwrtrk(dev, info, wrqu, extra);
		break;
	case MP_SET_TSSIDE:
		RTW_INFO("set case MP_TSSI_DE\n");
		status = rtw_mp_set_tsside(dev, info, wrqu, extra);
		break;
	case EFUSE_SET:
		RTW_INFO("set case efuse set\n");
		status = rtw_ioctl_efuse_set(dev, info, wdata, extra);
		break;
	case EFUSE_GET:
		RTW_INFO("efuse get EFUSE_GET\n");
		status = rtw_ioctl_efuse_get(dev, info, wdata, extra);
		break;
	case MP_GET_TXPOWER_INX:
		RTW_INFO("mp_get MP_GET_TXPOWER_INX\n");
		status = rtw_mp_txpower_index(dev, info, wrqu, extra);
		break;
	case MP_GETVER:
		RTW_INFO("mp_get MP_GETVER\n");
		status = rtw_mp_getver(dev, info, wdata, extra);
		break;
	case MP_MON:
		RTW_INFO("mp_get MP_MON\n");
		status = rtw_mp_mon(dev, info, wdata, extra);
		break;
	case EFUSE_BT_MASK:
		RTW_INFO("mp_get EFUSE_BT_MASK\n");
		status = rtw_ioctl_efuse_bt_file_mask_load(dev, info, wdata, extra);
		break;
	case EFUSE_MASK:
		RTW_INFO("mp_get EFUSE_MASK\n");
		status = rtw_ioctl_efuse_file_mask_load(dev, info, wdata, extra);
		break;
	case EFUSE_FILE:
		RTW_INFO("mp_get EFUSE_FILE\n");
		status = rtw_ioctl_efuse_file_map_load(dev, info, wdata, extra);
		break;
	case EFUSE_FILE_STORE:
		RTW_INFO("mp_get EFUSE_FILE_STORE\n");
		/*status = rtw_efuse_file_map_store(dev, info, wdata, extra);*/
		break;
	case MP_TX:
		RTW_INFO("mp_get MP_TX\n");
		status = rtw_mp_tx(dev, info, wdata, extra);
		break;
	case MP_RX:
		RTW_INFO("mp_get MP_RX\n");
		status = rtw_mp_rx(dev, info, wdata, extra);
		break;
	case MP_HW_TX_MODE:
		RTW_INFO("mp_get MP_HW_TX_MODE\n");
		status = rtw_mp_hwtx(dev, info, wdata, extra);
		break;
	case MP_GET_TSSIDE:
		RTW_INFO("mp_get TSSI_DE\n");
		status = rtw_mp_get_tsside(dev, info, wrqu, extra);
		break;
#ifdef CONFIG_RTW_CUSTOMER_STR
	case MP_CUSTOMER_STR:
		RTW_INFO("customer str\n");
		status = rtw_mp_customer_str(dev, info, wdata, extra);
		break;
#endif
	case MP_PWRLMT:
		RTW_INFO("mp_get MP_SETPWRLMT\n");
		status = rtw_mp_pwrlmt(dev, info, wdata, extra);
		break;
	case  BT_EFUSE_FILE:
		RTW_INFO("mp_get BT EFUSE_FILE\n");
		status = rtw_ioctl_efuse_bt_file_map_load(dev, info, wdata, extra);
		break;
	case MP_SWRFPath:
		RTW_INFO("mp_get MP_SWRFPath\n");
		status = rtw_mp_switch_rf_path(dev, info, wrqu, extra);
		break;
	case MP_LINK:
		RTW_INFO("mp_get MP_LINK\n");
		status = rtw_mp_link(dev, info, wrqu, extra);
		break;
	case MP_DPK_TRK:
		RTW_INFO("mp_get MP_DPK_TRK\n");
		status = rtw_mp_dpk_track(dev, info, wdata, extra);
		break;
	case MP_DPK:
		RTW_INFO("set MP_DPK\n");
		status = rtw_mp_dpk(dev, info, wdata, extra);
		break;
	case MP_GET_PHL_TEST:
		RTW_INFO("mp_get MP_GET_PHL_TEST\n");
		status = rtw_mp_get_phl_io(dev, info, wrqu, extra);
		break;
	case MP_SET_PHL_TEST:
		RTW_INFO("mp_get MP_SET_PHL_TEST\n");
		status = rtw_mp_set_phl_io(dev, info, wrqu, extra);
		break;
	case MP_SET_PHL_TX_PATTERN:
		RTW_INFO("mp_get MP_SET_PHL_TEST\n");
		status = rtw_mp_tx_pattern_idx(dev, info, wdata, extra);
		break;
	case MP_SET_PHL_PLCP_TX_DATA:
		RTW_INFO("mp_get MP_SET_PHL_PLCP_TX_DATA\n");
		status = rtw_mp_tx_plcp_tx_data(dev, info, wdata, extra);
		break;
	case MP_SET_PHL_PLCP_TX_USER:
		RTW_INFO("mp_get MP_SET_PHL_PLCP_TX_USER\n");
		status = rtw_mp_tx_plcp_tx_user(dev, info, wdata, extra);
		break;
	case MP_SET_PHL_TX_METHOD:
		RTW_INFO("mp_get MP_SET_PHL_TX_METHOD\n");
		status = rtw_mp_tx_method(dev, info, wdata, extra);
		break;
	case MP_SET_PHL_CONIFG_PHY_NUM:
		RTW_INFO("mp_get MP_SET_PHL_CONIFG_PHY_NUM\n");
		status = rtw_mp_config_phy(dev, info, wdata, extra);
		break;
	case MP_PHL_RFK:
		RTW_INFO("mp_get MP_PHL_RFK\n");
		status = rtw_mp_phl_rfk(dev, info, wdata, extra);
		break;
	case MP_PHL_BTC_PATH:
		RTW_INFO("mp_get MP_PHL_BTC_PATH\n");
		status = rtw_mp_phl_btc_path(dev, info, wdata, extra);
		break;
	case MP_GET_HE:
		RTW_INFO("mp_get MP_GET_HE\n");
		status = rtw_mp_get_he(dev, info, wdata, extra);
		break;
	case MP_BAND:
		RTW_INFO("mp_get MP_BAND\n");
		status = rtw_mp_band(dev, info, wdata, extra);
		break;
	case MP_UUID:
		RTW_INFO("set case MP_UUID\n");
		status = rtw_mp_uuid(dev, info, wrqu, extra);
		break;
	default:
		status = -EIO;
	}

	return status;
}

int rtw_priv_mp_set(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wdata, char *extra)
{

	struct iw_point *wrqu = (struct iw_point *)wdata;
	u32 subcmd = wrqu->flags;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	int status = 0;

#ifdef CONFIG_CONCURRENT_MODE
	if (!is_primary_adapter(padapter)) {
		RTW_INFO("MP mode only primary Adapter support\n");
		return -EIO;
	}
#endif

	RTW_INFO("%s mutx in %d\n", __func__, subcmd);

	switch (subcmd) {
	case MP_DISABLE_BT_COEXIST:
		RTW_INFO("set case MP_DISABLE_BT_COEXIST\n");
		status = rtw_mp_disable_bt_coexist(dev, info, wdata, extra);
		break;
	case MP_IQK:
		RTW_INFO("set MP_IQK\n");
		status = rtw_mp_iqk(dev, info, wrqu, extra);
		break;
	case MP_LCK:
		RTW_INFO("set MP_LCK\n");
		status = rtw_mp_lck(dev, info, wrqu, extra);
	break;

	default:
		status = -EIO;
	}

	RTW_INFO("%s mutx done %d\n", __func__, subcmd);

	return status;
}

#endif
