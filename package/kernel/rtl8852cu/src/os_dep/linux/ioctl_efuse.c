/******************************************************************************
 *
 * Copyright(c) 2007 - 2020 Realtek Corporation.
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
#include <rtw_efuse.h>

static u8 rtw_efuse_cmd(_adapter *padapter,
							struct rtw_efuse_phl_arg *pefuse_arg ,
							enum rtw_efuse_phl_cmdid cmdid)
{
	u32 i = 0;
	u8 ret = _FAIL;
	pefuse_arg->mp_class = RTW_MP_CLASS_EFUSE;
	pefuse_arg->cmd = cmdid;
	pefuse_arg->cmd_ok = 0;


	rtw_mp_set_phl_cmd(padapter, (void*)pefuse_arg, sizeof(struct rtw_efuse_phl_arg));

	while (i <= 50) {
		rtw_msleep_os(10);
		rtw_mp_get_phl_cmd(padapter, (void*)pefuse_arg, sizeof(struct rtw_efuse_phl_arg));
		if (pefuse_arg->cmd_ok && pefuse_arg->status == RTW_PHL_STATUS_SUCCESS) {
			RTW_INFO("%s,eFuse GET CMD OK !!!\n", __func__);
			ret = _SUCCESS;
			break;
		} else {
			rtw_msleep_os(10);
			if (i > 50) {
				RTW_INFO("%s, eFuse GET CMD FAIL !!!\n", __func__);
				break;
			}
			i++;
		}
	}

	return ret;
}

u32 rtw_efuse_get_map_size(_adapter *padapter , u16 *size , enum rtw_efuse_phl_cmdid cmdid)
{
	struct rtw_efuse_phl_arg *efuse_arg = NULL;
	u8 res = _FAIL;

	efuse_arg = _rtw_malloc(sizeof(struct rtw_efuse_phl_arg));
	if (efuse_arg) {
		_rtw_memset((void *)efuse_arg, 0, sizeof(struct rtw_efuse_phl_arg));
		rtw_efuse_cmd(padapter, efuse_arg, cmdid);
		if (efuse_arg->cmd_ok && efuse_arg->status == RTW_PHL_STATUS_SUCCESS) {
				*size = efuse_arg->io_value;
				res = _SUCCESS;
		} else {
				*size = 0;
				res = _FAIL;
		}
	}
	if (efuse_arg)
		_rtw_mfree(efuse_arg, sizeof(struct rtw_efuse_phl_arg));

	return res;
}

u32 rtw_efuse_get_available_size(_adapter *padapter , u16 *size, u8 efuse_type)
{
	struct rtw_efuse_phl_arg *efuse_arg = NULL;
	u8 res = _FAIL;

	efuse_arg = _rtw_malloc(sizeof(struct rtw_efuse_phl_arg));

	if (efuse_arg) {
		_rtw_memset((void *)efuse_arg, 0, sizeof(struct rtw_efuse_phl_arg));

		if (efuse_type == RTW_EFUSE_WIFI)
			rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_WIFI_GET_AVL_SIZE);
		else
			rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_BT_GET_AVL_SIZE);

		if (efuse_arg->cmd_ok && efuse_arg->status == RTW_PHL_STATUS_SUCCESS) {
				*size = efuse_arg->io_value;
				res = _SUCCESS;
		} else {
				*size = 0;
				res = _FAIL;
		}
	}
	if (efuse_arg)
		_rtw_mfree(efuse_arg, sizeof(struct rtw_efuse_phl_arg));

	return res;
}

static u8 rtw_efuse_fake2map(_adapter *padapter, u8 efuse_type)
{
	struct rtw_efuse_phl_arg *efuse_arg = NULL;
	u8 res = _SUCCESS;

	efuse_arg = _rtw_malloc(sizeof(struct rtw_efuse_phl_arg));
	if (efuse_arg) {
		_rtw_memset((void *)efuse_arg, 0, sizeof(struct rtw_efuse_phl_arg));

		if (efuse_type == RTW_EFUSE_WIFI)
			rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_WIFI_UPDATE);
		else if (efuse_type == RTW_EFUSE_BT)
			rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_BT_UPDATE);

		if (efuse_arg->cmd_ok && efuse_arg->status == RTW_PHL_STATUS_SUCCESS)
			res = _SUCCESS;
		else
			res = _FAIL;
	}
	if (efuse_arg)
		_rtw_mfree(efuse_arg, sizeof(struct rtw_efuse_phl_arg));

	return res;
}

static u8 rtw_efuse_read_map2shadow(_adapter *padapter, u8 efuse_type)
{
	struct rtw_efuse_phl_arg *efuse_arg = NULL;
	u8 res = _SUCCESS;

	efuse_arg = _rtw_malloc(sizeof(struct rtw_efuse_phl_arg));
	if (efuse_arg) {
		_rtw_memset((void *)efuse_arg, 0, sizeof(struct rtw_efuse_phl_arg));
		if (efuse_type == RTW_EFUSE_WIFI)
		    rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_WIFI_UPDATE_MAP);
		else if (efuse_type == RTW_EFUSE_BT)
			rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_BT_UPDATE_MAP);
		if (efuse_arg->cmd_ok && efuse_arg->status == RTW_PHL_STATUS_SUCCESS)
				res = _SUCCESS;
		else
				res = _FAIL;
	}
	if (efuse_arg)
		_rtw_mfree(efuse_arg, sizeof(struct rtw_efuse_phl_arg));

	return res;
}

static u8 rtw_efuse_get_shadow_map(_adapter *padapter, u8 *map, u16 size, u8 efuse_type)
{
	struct rtw_efuse_phl_arg *efuse_arg = NULL;
	u8 res = _FAIL;

	efuse_arg = _rtw_malloc(sizeof(struct rtw_efuse_phl_arg));
	if (efuse_arg) {

		efuse_arg->buf_len = size;
		if (efuse_type == RTW_EFUSE_WIFI)
		    rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_SHADOW_MAP2BUF);
		else if (efuse_type == RTW_EFUSE_BT)
			rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_BT_SHADOW_MAP2BUF);
		if (efuse_arg->cmd_ok && efuse_arg->status == RTW_PHL_STATUS_SUCCESS) {
				_rtw_memcpy((void *)map, efuse_arg->poutbuf, size);
				res = _SUCCESS;
		} else
				res = _FAIL;
	}
	if (efuse_arg)
		_rtw_mfree(efuse_arg, sizeof(struct rtw_efuse_phl_arg));

	return res;
}

static u8 rtw_efuse_renew_update(_adapter *padapter, u8 efuse_type)
{
	struct rtw_efuse_phl_arg *efuse_arg = NULL;
	u8 res = _SUCCESS;

	efuse_arg = _rtw_malloc(sizeof(struct rtw_efuse_phl_arg));
	if (efuse_arg) {
		_rtw_memset((void *)efuse_arg, 0, sizeof(struct rtw_efuse_phl_arg));

		if (efuse_type == RTW_EFUSE_WIFI)
			rtw_efuse_cmd(padapter, efuse_arg, RTW_MP_EFUSE_CMD_WIFI_SET_RENEW);
		else if (efuse_type == RTW_EFUSE_BT)
			RTW_INFO("halmac_get_logical_efuse_size fail\n");

		if (efuse_arg->cmd_ok && efuse_arg->status == RTW_PHL_STATUS_SUCCESS)
			res = _SUCCESS;
		else
			res = _FAIL;
	}
	if (efuse_arg)
		_rtw_mfree(efuse_arg, sizeof(struct rtw_efuse_phl_arg));

	return res;
}

static bool rtw_file_efuse_ismasked(_adapter *padapter, u16 offset, u8 *maskbuf)
{
	int r = offset / 16;
	int c = (offset % 16) / 2;
	int result = 0;

	if (padapter->registrypriv.boffefusemask)
		return _FALSE;

	if (c < 4) /* Upper double word */
		result = (maskbuf[r] & (0x10 << c));
	else
		result = (maskbuf[r] & (0x01 << (c - 4)));

	return (result > 0) ? 0 : 1;
}

static u8 rtw_efuse_compare_data(_adapter *padapter,
				u16 addr, u32 len, u8 *dst, u8 *src, u8 efuse_type)
{
	struct rtw_efuse_phl_arg *efuse_arg = NULL;
	enum rtw_efuse_phl_cmdid cmdid = RTW_EFUSE_CMD_WIFI_GET_MASK_BUF;
	u8 res = _FAIL;
	u16 idx = 0;

	efuse_arg = _rtw_malloc(sizeof(struct rtw_efuse_phl_arg));

	if (efuse_arg) {
		_rtw_memset((void *)efuse_arg, 0, sizeof(struct rtw_efuse_phl_arg));


		if (efuse_type == RTW_EFUSE_WIFI)
			cmdid = RTW_EFUSE_CMD_WIFI_GET_MASK_BUF;
		else if (efuse_type == RTW_EFUSE_BT)
			cmdid = RTW_EFUSE_CMD_BT_GET_MASK_BUF;

		efuse_arg->status = 0;
		efuse_arg->cmd_ok = 0;
		efuse_arg->buf_len = 0;

		if (rtw_efuse_cmd(padapter, efuse_arg, cmdid) == _SUCCESS) {

			RTW_MAP_DUMP_SEL(RTW_DBGDUMP, "efuse mask buf:",
				efuse_arg->poutbuf, efuse_arg->buf_len);

			for (idx = 0; idx + addr < (addr + len); idx++) {
					u16 chkaddr = idx + addr;

				if (!rtw_file_efuse_ismasked(padapter, chkaddr, efuse_arg->poutbuf)) {
					if (_rtw_memcmp((void *)dst + chkaddr , (void *)src + idx, 1)) {
						RTW_INFO("mask cmp success addr: 0x%x cur dst: %02x <=> PG src:%02x\n",
										idx, dst[chkaddr], src[idx]);
						res = _SUCCESS;
					} else {
						RTW_INFO("mask cmp Fail addr: 0x%x cur dst: %02x <=> PG src:%02x\n",
										chkaddr, dst[chkaddr], src[idx]);
						res  = _FAIL;
						break;
					}
				}
			}
		} else {
			RTW_INFO("rtw_efuse_cmd:%d, status:%d,mask len: %d\n",
				efuse_arg->cmd_ok, efuse_arg->status, efuse_arg->buf_len);
			res  = _FAIL;
		}
	}
	if (efuse_arg)
		_rtw_mfree(efuse_arg, sizeof(struct rtw_efuse_phl_arg));
	return res;
}

u8 rtw_efuse_map_read(_adapter * adapter, u16 addr, u16 cnts, u8 *data, u8 efuse_type)
{
	struct dvobj_priv *d;
	u8 *efuse = NULL;
	u16 size, i;
	int err = _FAIL;
	u8 status = _SUCCESS;

	if (efuse_type == RTW_EFUSE_WIFI)
		err = rtw_efuse_get_map_size(adapter, &size, RTW_EFUSE_CMD_WIFI_GET_LOG_SIZE);
	else if (efuse_type == RTW_EFUSE_BT)
		err = rtw_efuse_get_map_size(adapter, &size, RTW_EFUSE_CMD_BT_GET_LOG_SIZE);

	if (err == _FAIL) {
		status = _FAIL;
		RTW_INFO("halmac_get_logical_efuse_size fail\n");
		goto exit;
	}
	/* size error handle */
	if ((addr + cnts) > size) {
		if (addr < size)
			cnts = size - addr;
		else {
			status = _FAIL;
			RTW_INFO(" %s() ,addr + cnts) > size fail\n", __func__);
			goto exit;
		}
	}

	efuse = rtw_zmalloc(size);
	if (efuse) {
		if (rtw_efuse_read_map2shadow(adapter, efuse_type) == _SUCCESS) {
			err = rtw_efuse_get_shadow_map(adapter, efuse, size, efuse_type);
			if (err == _FAIL) {
				rtw_mfree(efuse, size);
				status = _FAIL;
				RTW_INFO(" %s() ,halmac_read_logical_efus map fail\n", __func__);
				goto exit;
			}
		} else {
			RTW_INFO(" %s() ,rtw_efuse_read_map2shadow FAIL !!!\n", __func__);
			rtw_mfree(efuse, size);
			status = _FAIL;
			goto exit;
		}
		if (efuse) {
			RTW_INFO(" %s() ,cp efuse to data\n", __func__);
			_rtw_memcpy(data, efuse + addr, cnts);
			rtw_mfree(efuse, size);
		}
	} else {
			RTW_INFO(" %s() ,alloc efuse fail\n", __func__);
			goto exit;
	}
	status = _SUCCESS;
exit:

	return status;
}

u8 rtw_efuse_map_write(_adapter * adapter, u16 addr, u16 cnts, u8 *data, u8 efuse_type, u8 bpg)
{
	struct dvobj_priv *d;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	u16 size;
	int err = _FAIL;
	u8 mask_buf[64] = "";
	u32 backupRegs[4] = {0};
	u8 status = _SUCCESS;
	struct rtw_efuse_phl_arg *efuse_arg = NULL;
	u16 i = 0;

	efuse_arg = rtw_zmalloc(sizeof(struct rtw_efuse_phl_arg));

	if (efuse_type == RTW_EFUSE_WIFI)
		err = rtw_efuse_get_map_size(adapter, &size, RTW_EFUSE_CMD_WIFI_GET_LOG_SIZE);
	else if (efuse_type == RTW_EFUSE_BT)
		err = rtw_efuse_get_map_size(adapter, &size, RTW_EFUSE_CMD_BT_GET_LOG_SIZE);

	if (err == _FAIL) {
		status = _FAIL;
		goto exit;
	}

	if ((addr + cnts) > size) {
		status = _FAIL;
		goto exit;
	}

	if (efuse_type == RTW_EFUSE_WIFI) {
		while (i != cnts) {
			efuse_arg->io_type = 1;
			efuse_arg->io_offset = addr + i;
			efuse_arg->io_value = data[i];
			rtw_efuse_cmd(adapter, efuse_arg, RTW_EFUSE_CMD_WIFI_WRITE);
			if (i > cnts)
				break;
			i++;
		}
	} else if (efuse_type == RTW_EFUSE_BT) {
		while (i != cnts) {
			efuse_arg->io_type = 1;
			efuse_arg->io_offset = addr + i;
			efuse_arg->io_value = data[i];
			rtw_efuse_cmd(adapter, efuse_arg, RTW_EFUSE_CMD_BT_WRITE);
			if (i > cnts)
				break;
			i++;
		}
	}

	if (bpg) {
		RTW_INFO(" in PG state !!!\n");
		if (efuse_type == RTW_EFUSE_WIFI)
			err = rtw_efuse_fake2map(adapter, efuse_type);
		else if (efuse_type == RTW_EFUSE_BT) {
			u8 reg2chkbt = 0;

			rtw_phl_write8(dvobj->phl, 0xa3, 0x05);
			rtw_msleep_os(10);
			reg2chkbt = rtw_phl_read8(dvobj->phl, 0xa0);

			if (reg2chkbt != 0x04) {
				RTW_ERR("BT State not Active:0x%x ,can't Write \n", reg2chkbt);
				goto exit;
			}

			err = rtw_efuse_fake2map(adapter, efuse_type);
		}

		if (err == _FAIL) {
			status = _FAIL;
			goto exit;
		}
	}
	status = _SUCCESS;
exit :
	if (efuse_arg)
		_rtw_mfree(efuse_arg, sizeof(struct rtw_efuse_phl_arg));

	return status;
}

static u8 rtw_efuse_map_file_load(_adapter *padapter, u8 *filepath, u8 efuse_type)
{
	struct rtw_efuse_phl_arg *efuse_arg = NULL;
	u8 res = _FAIL;

	if (filepath) {
		RTW_INFO("efuse file path %s len %zu", filepath, strlen(filepath));

		efuse_arg = _rtw_malloc(sizeof(struct rtw_efuse_phl_arg));
		if (efuse_arg) {
			_rtw_memset((void *)efuse_arg, 0, sizeof(struct rtw_efuse_phl_arg));
			_rtw_memcpy(efuse_arg->pfile_path, filepath, strlen(filepath));
			if (efuse_type == RTW_EFUSE_WIFI)
				rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_FILE_MAP_LOAD);
			else
				rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_BT_FILE_MAP_LOAD);
		}
	}
	if (efuse_arg->cmd_ok && efuse_arg->status == RTW_PHL_STATUS_SUCCESS)
			res = _SUCCESS;
	else
			res = _FAIL;

	if (efuse_arg)
		_rtw_mfree(efuse_arg, sizeof(struct rtw_efuse_phl_arg));

	return res;
}

static u8 rtw_efuse_mask_file_load(_adapter *padapter, u8 *filepath, u8 efuse_type)
{
	struct rtw_efuse_phl_arg *efuse_arg = NULL;
	u8 res = _FAIL;

	if (filepath) {
		RTW_INFO("efuse file path %s len %zu", filepath, strlen(filepath));
		efuse_arg = _rtw_malloc(sizeof(struct rtw_efuse_phl_arg));
		efuse_arg->status == RTW_PHL_STATUS_FAILURE;

		if (efuse_arg) {
			_rtw_memset((void *)efuse_arg, 0, sizeof(struct rtw_efuse_phl_arg));
			_rtw_memcpy(efuse_arg->pfile_path, filepath, strlen(filepath));
			if (efuse_type == RTW_EFUSE_WIFI)
				rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_FILE_MASK_LOAD);
			else
				rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_BT_FILE_MASK_LOAD);
		}
	}

	if (efuse_arg->cmd_ok && efuse_arg->status == RTW_PHL_STATUS_SUCCESS)
			res = _SUCCESS;
	else
			res = _FAIL;

	if (efuse_arg)
		_rtw_mfree(efuse_arg, sizeof(struct rtw_efuse_phl_arg));

	return res;
}

int rtw_ioctl_efuse_get(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
	_adapter *padapter = rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	char *pch, *ptmp, *token, *tmp[3] = {0x00, 0x00, 0x00};
	u16 i = 0, j = 0, mapLen = 0, addr = 0, cnts = 0;
	int err = 0;
	char *pextra = NULL;
	u8 *pre_efuse_map = NULL;
	u16 max_available_len = 0;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length)) {
		err = -EFAULT;
		goto exit;
	}

	*(extra +  wrqu->data.length) = '\0';
	pch = extra;
	RTW_INFO("%s: in=%s\n", __FUNCTION__, extra);

	i = 0;
	/* mac 16 "00e04c871200" rmap,00,2 */
	while ((token = strsep(&pch, ",")) != NULL) {
		if (i > 2)
			break;
		tmp[i] = token;
		i++;
	}
	pre_efuse_map = rtw_zmalloc(RTW_MAX_EFUSE_MAP_LEN);
	if (pre_efuse_map == NULL)
			goto exit;

	if (strcmp(tmp[0], "status") == 0) {
		//sprintf(extra, "Load File efuse=%s,Load File MAC=%s"
		//	, efuse->file_status == EFUSE_FILE_FAILED ? "FAIL" : "OK"
		//	, pHalData->macaddr_file_status == MACADDR_FILE_FAILED ? "FAIL" : "OK"
		//       );
		goto exit;
	} else if (strcmp(tmp[0], "drvmap") == 0) {
		static u8 drvmaporder = 0;
		u8 *efuse_data = NULL;
		u32 shift, cnt;
		u32 blksz = 0x200; /* The size of one time show, default 512 */
		//EFUSE_GetEfuseDefinition(padapter, EFUSE_WIFI, TYPE_EFUSE_MAP_LEN, (void *)&mapLen, _FALSE);

		//efuse_data = efuse->data;

		shift = blksz * drvmaporder;
		efuse_data += shift;
		cnt = mapLen - shift;

		if (cnt > blksz) {
			cnt = blksz;
			drvmaporder++;
		} else
			drvmaporder = 0;

		sprintf(extra, "\n");
		for (i = 0; i < cnt; i += 16) {
			pextra = extra + strlen(extra);
			pextra += sprintf(pextra, "0x%02x\t", shift + i);
			for (j = 0; j < 8; j++)
				pextra += sprintf(pextra, "%02X ", efuse_data[i + j]);
			pextra += sprintf(pextra, "\t");
			for (; j < 16; j++)
				pextra += sprintf(pextra, "%02X ", efuse_data[i + j]);
			pextra += sprintf(pextra, "\n");
		}
		if ((shift + cnt) < mapLen)
			pextra += sprintf(pextra, "\t...more (left:%d/%d)\n", mapLen-(shift + cnt), mapLen);

	} else if (strcmp(tmp[0], "realmap") == 0) {
		static u8 order = 0;
		u32 shift, cnt;
		u32 blksz = 0x200; /* The size of one time show, default 512 */
		u8 *efuse_data = NULL;

		rtw_efuse_get_map_size(padapter, &mapLen, RTW_EFUSE_CMD_WIFI_GET_LOG_SIZE);

 		if (pre_efuse_map) {
			if (rtw_efuse_map_read(padapter, 0, mapLen, pre_efuse_map, RTW_EFUSE_WIFI) == _FAIL) {
				RTW_INFO("%s: read realmap Fail!!\n", __FUNCTION__);
				err = -EFAULT;
			} else {
				efuse_data = pre_efuse_map;

				_rtw_memset(extra, '\0', strlen(extra));
				shift = blksz * order;
				efuse_data += shift;
				cnt = mapLen - shift;
				if (cnt > blksz) {
					cnt = blksz;
					order++;
				} else
					order = 0;

				sprintf(extra, "\n");
				for (i = 0; i < cnt; i += 16) {
					pextra = extra + strlen(extra);
					pextra += sprintf(pextra, "0x%02x\t", shift + i);
					for (j = 0; j < 8; j++)
						pextra += sprintf(pextra, "%02X ", efuse_data[i + j]);
					pextra += sprintf(pextra, "\t");
					for (; j < 16; j++)
						pextra += sprintf(pextra, "%02X ", efuse_data[i + j]);
					pextra += sprintf(pextra, "\n");

					if (strlen(extra) > 0x7FF)
						break;
				}
				if ((shift + cnt) < mapLen)
					pextra += sprintf(pextra, "\t...more (left:%d/%d)\n", mapLen - (shift + cnt), mapLen);
				}
		}
	} else if (strcmp(tmp[0], "rmap") == 0) {
		u8 *data = NULL;
		if ((tmp[1] == NULL) || (tmp[2] == NULL)) {
			RTW_INFO("%s: rmap Fail!! Parameters error!\n", __FUNCTION__);
			err = -EINVAL;
			goto exit;
		}
		/* rmap addr cnts */
		addr = simple_strtoul(tmp[1], &ptmp, 16);
		RTW_INFO("%s: addr=%x\n", __FUNCTION__, addr);

		cnts = simple_strtoul(tmp[2], &ptmp, 10);
		if (cnts == 0) {
			RTW_INFO("%s: rmap Fail!! cnts error!\n", __FUNCTION__);
			err = -EINVAL;
			goto exit;
		}
		RTW_INFO("%s: cnts=%d\n", __FUNCTION__, cnts);

		rtw_efuse_get_map_size(padapter, &mapLen, RTW_EFUSE_CMD_WIFI_GET_LOG_SIZE);

		if ((addr + cnts) > mapLen) {
			RTW_INFO("%s: addr(0x%X)+cnts(%d) over mapLen %d parameter error!\n", __FUNCTION__, addr, cnts, mapLen);
			err = -EINVAL;
			goto exit;
		}

		if (pre_efuse_map) {
			if (rtw_efuse_map_read(padapter, addr, cnts, pre_efuse_map, RTW_EFUSE_WIFI) == _FAIL) {
				RTW_INFO("%s: rtw_efuse_map_read Fail!!\n", __FUNCTION__);
				err = -EFAULT;
			} else {
				data = pre_efuse_map;
				*extra = 0;
				pextra = extra;
				for (i = 0; i < cnts; i++) {
					pextra += sprintf(pextra, "0x%02X ", data[i]);
				}
			}
		}
	} else if (strcmp(tmp[0], "wlrfkmap") == 0) {
		static u8 order = 0;
		u32 shift, cnt;
		u32 blksz = 0x200; /* The size of one time show, default 512 */
		u8 *efuse_data = NULL;

		rtw_efuse_get_map_size(padapter, &mapLen, RTW_EFUSE_CMD_WIFI_GET_LOG_SIZE);

 		if (pre_efuse_map) {
			if (rtw_efuse_get_shadow_map(padapter, pre_efuse_map, mapLen, RTW_EFUSE_WIFI) == _FAIL) {
				RTW_INFO("%s: read wifi fake map Fail!!\n", __FUNCTION__);
				err = -EFAULT;
			} else {
				efuse_data = pre_efuse_map;

				_rtw_memset(extra, '\0', strlen(extra));
				shift = blksz * order;
				efuse_data += shift;
				cnt = mapLen - shift;
				if (cnt > blksz) {
					cnt = blksz;
					order++;
				} else
					order = 0;

				sprintf(extra, "\n");
				for (i = 0; i < cnt; i += 16) {
					pextra = extra + strlen(extra);
					pextra += sprintf(pextra, "0x%02x\t", shift + i);
					for (j = 0; j < 8; j++)
						pextra += sprintf(pextra, "%02X ", efuse_data[i + j]);
					pextra += sprintf(pextra, "\t");
					for (; j < 16; j++)
						pextra += sprintf(pextra, "%02X ", efuse_data[i + j]);
					pextra += sprintf(pextra, "\n");

					if (strlen(extra) > 0x7FF)
						break;
				}
				if ((shift + cnt) < mapLen)
					pextra += sprintf(pextra, "\t...more (left:%d/%d)\n", mapLen - (shift + cnt), mapLen);
				}
		}
	} else if (strcmp(tmp[0], "wlrfkrmap") == 0) {
		u8 *data = NULL;
		if ((tmp[1] == NULL) || (tmp[2] == NULL)) {
			RTW_INFO("%s: rmap Fail!! Parameters error!\n", __FUNCTION__);
			err = -EINVAL;
			goto exit;
		}
		/* rmap addr cnts */
		addr = simple_strtoul(tmp[1], &ptmp, 16);
		RTW_INFO("%s: addr=%x\n", __FUNCTION__, addr);

		cnts = simple_strtoul(tmp[2], &ptmp, 10);
		if (cnts == 0) {
			RTW_INFO("%s: rmap Fail!! cnts error!\n", __FUNCTION__);
			err = -EINVAL;
			goto exit;
		}
		RTW_INFO("%s: cnts=%d\n", __FUNCTION__, cnts);

		rtw_efuse_get_map_size(padapter, &mapLen, RTW_EFUSE_CMD_WIFI_GET_LOG_SIZE);

		if ((addr + cnts) > mapLen) {
			RTW_INFO("%s: addr(0x%X)+cnts(%d) over mapLen %d parameter error!\n", __FUNCTION__, addr, cnts, mapLen);
			err = -EINVAL;
			goto exit;
		}

		if (pre_efuse_map) {
			if (rtw_efuse_get_shadow_map(padapter, pre_efuse_map, mapLen, RTW_EFUSE_WIFI) == _FAIL) {
				RTW_INFO("%s: read wifi fake map Fail!!\n", __FUNCTION__);
				err = -EFAULT;
			} else {
				data = pre_efuse_map;
				*extra = 0;
				pextra = extra;
				for (i = addr; i < addr + cnts; i++)
					pextra += sprintf(pextra, "0x%02X ", data[i]);
			}
		}
	} else if (strcmp(tmp[0], "btrealmap") == 0) {
		static u8 order = 0;
		u32 shift, cnt;
		u32 blksz = 0x200; /* The size of one time show, default 512 */
		u8 *efuse_data = NULL;

		rtw_efuse_get_map_size(padapter, &mapLen, RTW_EFUSE_CMD_BT_GET_LOG_SIZE);

 		if (pre_efuse_map) {
			if (rtw_efuse_map_read(padapter, 0, mapLen, pre_efuse_map, RTW_EFUSE_BT) == _FAIL) {
				RTW_INFO("%s: read BT realmap Fail!!\n", __FUNCTION__);
				err = -EFAULT;
			} else {
				efuse_data = pre_efuse_map;

				_rtw_memset(extra, '\0', strlen(extra));
				shift = blksz * order;
				efuse_data += shift;
				cnt = mapLen - shift;
				if (cnt > blksz) {
					cnt = blksz;
					order++;
				} else
					order = 0;

				sprintf(extra, "\n");
				for (i = 0; i < cnt; i += 16) {
					pextra = extra + strlen(extra);
					pextra += sprintf(pextra, "0x%02x\t", shift + i);
					for (j = 0; j < 8; j++)
						pextra += sprintf(pextra, "%02X ", efuse_data[i + j]);
					pextra += sprintf(pextra, "\t");
					for (; j < 16; j++)
						pextra += sprintf(pextra, "%02X ", efuse_data[i + j]);
					pextra += sprintf(pextra, "\n");

					if (strlen(extra) > 0x7FF)
						break;
				}
				if ((shift + cnt) < mapLen)
					pextra += sprintf(pextra, "\t...more (left:%d/%d)\n", mapLen - (shift + cnt), mapLen);
			}
 		}
	} else if (strcmp(tmp[0], "btrmap") == 0) {
		u8 *data = NULL;
		if ((tmp[1] == NULL) || (tmp[2] == NULL)) {
			RTW_INFO("%s: btrmap Fail!! Parameters error!\n", __FUNCTION__);
			err = -EINVAL;
			goto exit;
		}
		/* rmap addr cnts */
		addr = simple_strtoul(tmp[1], &ptmp, 16);
		RTW_INFO("%s: addr=%x\n", __FUNCTION__, addr);

		cnts = simple_strtoul(tmp[2], &ptmp, 10);
		if (cnts == 0) {
			RTW_INFO("%s: btrmap Fail!! cnts error!\n", __FUNCTION__);
			err = -EINVAL;
			goto exit;
		}
		RTW_INFO("%s: cnts=%d\n", __FUNCTION__, cnts);

		rtw_efuse_get_map_size(padapter, &mapLen, RTW_EFUSE_CMD_BT_GET_LOG_SIZE);

		if ((addr + cnts) > mapLen) {
			RTW_INFO("%s: addr(0x%X)+cnts(%d) over mapLen %d parameter error!\n", __FUNCTION__, addr, cnts, mapLen);
			err = -EINVAL;
			goto exit;
		}

		if (pre_efuse_map) {
			if (rtw_efuse_map_read(padapter, addr, cnts, pre_efuse_map, RTW_EFUSE_BT) == _FAIL) {
				RTW_INFO("%s: rtw_efuse_map_read Fail!!\n", __FUNCTION__);
				err = -EFAULT;
			} else {
				data = pre_efuse_map;
				*extra = 0;
				pextra = extra;
				for (i = 0; i < cnts; i++)
					pextra += sprintf(pextra, "0x%02X ", data[i]);
			}
		}
	} else if (strcmp(tmp[0], "btrfkmap") == 0) {
		static u8 order = 0;
		u32 shift, cnt;
		u32 blksz = 0x200; /* The size of one time show, default 512 */
		u8 *efuse_data = NULL;

		rtw_efuse_get_map_size(padapter, &mapLen, RTW_EFUSE_CMD_BT_GET_LOG_SIZE);

 		if (pre_efuse_map) {
			if (rtw_efuse_get_shadow_map(padapter, pre_efuse_map, mapLen, RTW_EFUSE_BT) == _FAIL) {
				RTW_INFO("%s: read BT fake map Fail!!\n", __FUNCTION__);
				err = -EFAULT;
			} else {
				efuse_data = pre_efuse_map;

				_rtw_memset(extra, '\0', strlen(extra));
				shift = blksz * order;
				efuse_data += shift;
				cnt = mapLen - shift;
				if (cnt > blksz) {
					cnt = blksz;
					order++;
				} else
					order = 0;

				sprintf(extra, "\n");
				for (i = 0; i < cnt; i += 16) {
					pextra = extra + strlen(extra);
					pextra += sprintf(pextra, "0x%02x\t", shift + i);
					for (j = 0; j < 8; j++)
						pextra += sprintf(pextra, "%02X ", efuse_data[i + j]);
					pextra += sprintf(pextra, "\t");
					for (; j < 16; j++)
						pextra += sprintf(pextra, "%02X ", efuse_data[i + j]);
					pextra += sprintf(pextra, "\n");

					if (strlen(extra) > 0x7FF)
						break;
				}
				if ((shift + cnt) < mapLen)
					pextra += sprintf(pextra, "\t...more (left:%d/%d)\n", mapLen - (shift + cnt), mapLen);
			}
		}
	} else if (strcmp(tmp[0], "ableraw") == 0) {
			rtw_efuse_get_available_size(padapter, &max_available_len, RTW_EFUSE_WIFI);
			sprintf(extra, "[WIFI available raw size]= %d bytes\n", max_available_len);
	} else if (strcmp(tmp[0], "btableraw") == 0) {
			rtw_efuse_get_available_size(padapter, &max_available_len, RTW_EFUSE_BT);
			sprintf(extra, "[BT available raw size]= %d bytes\n", max_available_len);
	} else if (strcmp(tmp[0], "realraw") == 0) {
		static u8 raw_order = 0;
		u32 shift, cnt;
		u32 blksz = 0x200; /* The size of one time show, default 512 */
		u8 *realraw_map_data = NULL;

		rtw_efuse_get_map_size(padapter, &mapLen, RTW_EFUSE_CMD_WIFI_GET_SIZE);

 		if (pre_efuse_map) {
			if (rtw_efuse_raw_map_read (padapter, 0, mapLen, pre_efuse_map, RTW_EFUSE_WIFI) == _FAIL) {
				RTW_INFO("%s: read wifi raw map Fail!!\n", __FUNCTION__);
				err = -EFAULT;
			} else {
				realraw_map_data = pre_efuse_map;

				_rtw_memset(extra, '\0', strlen(extra));
				shift = blksz * raw_order;
				realraw_map_data += shift;
				cnt = mapLen - shift;
				if (cnt > blksz) {
					cnt = blksz;
					raw_order++;
				} else
					raw_order = 0;

				sprintf(extra, "\n");
				for (i = 0; i < cnt; i += 16) {
					pextra = extra + strlen(extra);
					pextra += sprintf(pextra, "0x%02x\t", shift + i);
					for (j = 0; j < 8; j++)
						pextra += sprintf(pextra, "%02X ", realraw_map_data[i + j]);
					pextra += sprintf(pextra, "\t");
					for (; j < 16; j++)
						pextra += sprintf(pextra, "%02X ", realraw_map_data[i + j]);
					pextra += sprintf(pextra, "\n");

					if (strlen(extra) > 0x7FF)
						break;
				}
				if ((shift + cnt) < mapLen)
					pextra += sprintf(pextra, "\t...more (left:%d/%d)\n", mapLen - (shift + cnt), mapLen);
				}
		}
	} else if (strcmp(tmp[0], "btrealraw")== 0) {
		static u8 raw_order = 0;
		u32 shift, cnt;
		u32 blksz = 0x200; /* The size of one time show, default 512 */
		u8 *realraw_map_data = NULL;

		rtw_efuse_get_map_size(padapter, &mapLen, RTW_EFUSE_CMD_BT_GET_SIZE);

 		if (pre_efuse_map) {
			if (rtw_efuse_raw_map_read (padapter, 0, mapLen, pre_efuse_map, RTW_EFUSE_BT) == _FAIL) {
				RTW_INFO("%s: read bt raw map Fail!!\n", __FUNCTION__);
				err = -EFAULT;
			} else {
				realraw_map_data = pre_efuse_map;

				_rtw_memset(extra, '\0', strlen(extra));
				shift = blksz * raw_order;
				realraw_map_data += shift;
				cnt = mapLen - shift;
				if (cnt > blksz) {
					cnt = blksz;
					raw_order++;
				} else
					raw_order = 0;

				sprintf(extra, "\n");
				for (i = 0; i < cnt; i += 16) {
					pextra = extra + strlen(extra);
					pextra += sprintf(pextra, "0x%02x\t", shift + i);
					for (j = 0; j < 8; j++)
						pextra += sprintf(pextra, "%02X ", realraw_map_data[i + j]);
					pextra += sprintf(pextra, "\t");
					for (; j < 16; j++)
						pextra += sprintf(pextra, "%02X ", realraw_map_data[i + j]);
					pextra += sprintf(pextra, "\n");

					if (strlen(extra) > 0x7FF)
						break;
				}
				if ((shift + cnt) < mapLen)
					pextra += sprintf(pextra, "\t...more (left:%d/%d)\n", mapLen - (shift + cnt), mapLen);
				}
		}
	} else
		sprintf(extra, "Command not found!");

exit:
	if (pre_efuse_map)
		rtw_mfree(pre_efuse_map, RTW_MAX_EFUSE_MAP_LEN);
	if (!err)
		wrqu->data.length = strlen(extra);
	RTW_INFO("%s: strlen(extra) =%zu\n", __FUNCTION__, strlen(extra));
	if (copy_to_user(wrqu->data.pointer, extra, wrqu->data.length))
		err = -EFAULT;

	return err;
}


int rtw_ioctl_efuse_set(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wdata, char *extra)
{
	struct iw_point *wrqu;
	struct pwrctrl_priv *pwrctrlpriv ;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	u32 i = 0, j = 0, jj = 0, kk = 0;
	u8 *setdata = NULL;
	u8 *shadowmap = NULL;
	u8 *setrawdata = NULL;
	char *pch, *ptmp, *token, *tmp[3] = {0x00, 0x00, 0x00};
	u16 addr = 0xFF, cnts = 0, max_available_len = 0;
	u16 wifimaplen;
	int err = 0;
	boolean bcmpchk = _TRUE;
	u8 status = _SUCCESS;
	u16 size;
	u8 bpg = true;

	wrqu = (struct iw_point *)wdata;
	pwrctrlpriv = adapter_to_pwrctl(padapter);

	if (copy_from_user(extra, wrqu->pointer, wrqu->length))
		return -EFAULT;

	*(extra + wrqu->length) = '\0';

	setdata = rtw_zmalloc(RTW_MAX_EFUSE_MAP_LEN);
	if (setdata == NULL) {
		err = -ENOMEM;
		goto exit;
	}

	pch = extra;
	RTW_INFO("%s: in=%s\n", __FUNCTION__, extra);

	i = 0;
	while ((token = strsep(&pch, ",")) != NULL) {
		if (i > 2)
			break;
		tmp[i] = token;
		i++;
	}

	/* tmp[0],[1],[2] */
	/* wmap,addr,00e04c871200 */
	if ((strcmp(tmp[0], "wmap") == 0) || (strcmp(tmp[0], "wlwfake") == 0)) {
		if ((tmp[1] == NULL) || (tmp[2] == NULL)) {
			err = -EINVAL;
			goto exit;
		}

		addr = simple_strtoul(tmp[1], &ptmp, 16);
		addr &= 0xFFF;

		cnts = strlen(tmp[2]);
		if (cnts % 2) {
			err = -EINVAL;
			goto exit;
		}
		cnts /= 2;
		if (cnts == 0) {
			err = -EINVAL;
			goto exit;
		}

		RTW_INFO("%s: addr=0x%X\n", __FUNCTION__, addr);
		RTW_INFO("%s: cnts=%d\n", __FUNCTION__, cnts);
		RTW_INFO("%s: map data=%s\n", __FUNCTION__, tmp[2]);

		for (jj = 0, kk = 0; jj < cnts; jj++, kk += 2)
			setdata[jj] = key_2char2num(tmp[2][kk], tmp[2][kk + 1]);

		err = rtw_efuse_get_map_size(padapter, &size, RTW_EFUSE_CMD_WIFI_GET_LOG_SIZE);
		max_available_len = size;

		if (err == _FAIL) {
			status = _FAIL;
			goto exit;
		}

		if ((addr + cnts) > max_available_len) {
			RTW_INFO("%s: addr(0x%X)+cnts(%d) parameter error!\n", __FUNCTION__, addr, cnts);
			err = -EFAULT;
			goto exit;
		}

		if (strcmp(tmp[0], "wlwfake") == 0)
			bpg = false;

		if (rtw_efuse_map_write(padapter, addr, cnts, setdata, RTW_EFUSE_WIFI, bpg) == _FAIL) {
			RTW_INFO("%s: rtw_efuse_map_write error!!\n", __FUNCTION__);
			err = -EFAULT;
			goto exit;
		}

		*extra = 0;
		if (bpg) {
			RTW_INFO("%s: after rtw_efuse_map_write to _rtw_memcmp\n", __func__);
			shadowmap = rtw_zmalloc(size);
			if (shadowmap == NULL) {
				RTW_INFO("%s: ShadowMapWiFi rtw_zmalloc Fail\n", __func__);
				err = 0;
				goto exit;
			}
			if (rtw_efuse_get_shadow_map(padapter, shadowmap, size,RTW_EFUSE_WIFI) == _SUCCESS) {

				if (rtw_efuse_compare_data(padapter, addr, cnts, shadowmap, setdata, RTW_EFUSE_WIFI)) {
					RTW_INFO("%s: WiFi write map afterf compare success\n", __FUNCTION__);
					sprintf(extra, "WiFi write map compare OK\n");
					err = 0;
					goto exit;
				} else {
					RTW_MAP_DUMP_SEL(RTW_DBGDUMP, "shadowmap :", shadowmap + addr, cnts);
					RTW_MAP_DUMP_SEL(RTW_DBGDUMP, "map_pged :", setdata, cnts);
					sprintf(extra, "WiFi write map compare FAIL\n");
					RTW_INFO("%s: WiFi write map compare Fail\n", __FUNCTION__);
					err = 0;
					goto exit;
				}
			}
		} else {
			sprintf(extra, "WiFi write fake map OK\n");
			RTW_INFO("%s: WiFi write fake map OK\n", __FUNCTION__);
			err = 0;
			goto exit;
		}
	} else if ((strcmp(tmp[0], "btwmap") == 0) || (strcmp(tmp[0], "btwfake") == 0)) {
		if ((tmp[1] == NULL) || (tmp[2] == NULL)) {
			err = -EINVAL;
			goto exit;
		}

		addr = simple_strtoul(tmp[1], &ptmp, 16);
		addr &= 0xFFF;

		cnts = strlen(tmp[2]);
		if (cnts % 2) {
			err = -EINVAL;
			goto exit;
		}
		cnts /= 2;
		if (cnts == 0) {
			err = -EINVAL;
			goto exit;
		}

		RTW_INFO("%s: addr=0x%X\n", __FUNCTION__, addr);
		RTW_INFO("%s: cnts=%d\n", __FUNCTION__, cnts);
		RTW_INFO("%s: map data=%s\n", __FUNCTION__, tmp[2]);

		for (jj = 0, kk = 0; jj < cnts; jj++, kk += 2)
			setdata[jj] = key_2char2num(tmp[2][kk], tmp[2][kk + 1]);

		err = rtw_efuse_get_map_size(padapter, &size, RTW_EFUSE_CMD_BT_GET_LOG_SIZE);
		max_available_len = size;

		if (err == _FAIL) {
			status = _FAIL;
			goto exit;
		}

		if ((addr + cnts) > max_available_len) {
			RTW_INFO("%s: addr(0x%X)+cnts(%d) parameter error!\n", __FUNCTION__, addr, cnts);
			err = -EFAULT;
			goto exit;
		}

		if (strcmp(tmp[0], "btwfake") == 0)
			bpg = false;

		if (rtw_efuse_map_write(padapter, addr, cnts, setdata, RTW_EFUSE_BT, bpg) == _FAIL) {
			RTW_INFO("%s: rtw_efuse_map_write error!!\n", __FUNCTION__);
			err = -EFAULT;
			goto exit;
		}

		*extra = 0;
		RTW_INFO("%s: after rtw_efuse_map_write to _rtw_memcmp\n", __func__);
		if (bpg) {
			shadowmap = rtw_zmalloc(size);
			if (shadowmap == NULL) {
				RTW_INFO("%s: shadowmap rtw_zmalloc Fail\n", __func__);
				err = 0;
				goto exit;
			}
			if (rtw_efuse_get_shadow_map(padapter, shadowmap, size,RTW_EFUSE_BT) == _SUCCESS) {

				if (rtw_efuse_compare_data(padapter, addr, cnts, shadowmap, setdata, RTW_EFUSE_BT)) {
					RTW_INFO("%s: BT write map afterf compare success\n", __func__);
					sprintf(extra, "BT write map compare OK\n");
					err = 0;
					goto exit;
				} else {
					RTW_MAP_DUMP_SEL(RTW_DBGDUMP, "shadowmap :", shadowmap, cnts);
					RTW_MAP_DUMP_SEL(RTW_DBGDUMP, "map_pged :", setdata, cnts);
					sprintf(extra, "BT write map compare FAIL\n");
					RTW_INFO("%s: BT write map compare Fail\n", __func__);
					err = 0;
					goto exit;
				}
			}
		} else {
			sprintf(extra, "BT write fake map OK\n");
			RTW_INFO("%s: BT write fake map OK\n", __FUNCTION__);
			err = 0;
			goto exit;
		}
	} else if (strcmp(tmp[0], "btwraw") == 0) {
		u8 reg2chkbt = 0;

		if ((tmp[1] == NULL) || (tmp[2] == NULL)) {
			err = -EINVAL;
			goto exit;
		}

		addr = simple_strtoul(tmp[1], &ptmp, 16);
		addr &= 0xFFF;

		cnts = strlen(tmp[2]);
		if (cnts % 2) {
			err = -EINVAL;
			goto exit;
		}
		cnts /= 2;
		if (cnts == 0) {
			err = -EINVAL;
			goto exit;
		}

		RTW_INFO("%s: addr=0x%X\n", __FUNCTION__, addr);
		RTW_INFO("%s: cnts=%d\n", __FUNCTION__, cnts);
		RTW_INFO("%s: raw data=%s\n", __FUNCTION__, tmp[2]);

		rtw_phl_write8(dvobj->phl, 0xa3, 0x05);
		rtw_msleep_os(10);
		reg2chkbt = rtw_phl_read8(dvobj->phl, 0xa0);
		RTW_INFO("%s: btfk2map before read 0xa0 BT Status =0x%x\n", __func__,
			rtw_phl_read32(dvobj->phl, 0xa0));

		if (reg2chkbt != 0x04) {
			sprintf(extra, "Error ! BT State not Active:0x%x ,can't Write \n" , reg2chkbt);
			goto exit;
		}

		for (jj = 0, kk = 0; jj < cnts; jj++, kk += 2)
			setdata[jj] = key_2char2num(tmp[2][kk], tmp[2][kk + 1]);

		if (rtw_efuse_bt_write_raw_hidden(padapter, addr, cnts, setdata) == _FAIL) {
			RTW_INFO("%s: rtw_efuse_bt_write_hidden error!!\n", __FUNCTION__);
			err = -EFAULT;
			goto exit;
		}
	} else if (strcmp(tmp[0], "wlfk2map") == 0) {
		u16 mapsize = 0;
		u8 *map_pged = NULL;

		RTW_INFO("%s:do rtw_efuse_fake2map\n", __func__);
		rtw_efuse_get_map_size(padapter, &mapsize, RTW_EFUSE_CMD_WIFI_GET_LOG_SIZE);
		shadowmap = rtw_zmalloc(mapsize);
		if (shadowmap == NULL) {
			RTW_INFO("%s: shadowmap rtw_zmalloc Fail\n", __func__);
			err = 0;
			goto exit;
		}
		if (rtw_efuse_get_shadow_map(padapter, shadowmap, mapsize, RTW_EFUSE_WIFI) == _FAIL) {
			RTW_INFO("%s:rtw_efuse_get_shadow_map Fail\n", __func__);
			err = 0;
			goto exit;
		}

		status = rtw_efuse_fake2map(padapter, RTW_EFUSE_WIFI);
		if (status == _SUCCESS)
			RTW_INFO("WIFI write map OK\n");
		else
			RTW_INFO("WIFI write map FAIL\n");

		map_pged = rtw_zmalloc(mapsize);
		if (map_pged == NULL) {
			RTW_INFO("%s: map_pged rtw_zmalloc Fail\n", __func__);
			err = 0;
			goto exit;
		}
		if (rtw_efuse_map_read(padapter, 0x00, mapsize, map_pged, RTW_EFUSE_WIFI) == _SUCCESS) {

			if (rtw_efuse_compare_data(padapter, 0x00, mapsize, map_pged, shadowmap, RTW_EFUSE_WIFI)) {
				RTW_INFO("%s: WiFi write map afterf compare success\n", __func__);
				sprintf(extra, "WiFi write map compare OK\n");
			} else {
				RTW_MAP_DUMP_SEL(RTW_DBGDUMP, "shadowmap :", shadowmap, mapsize);
				RTW_MAP_DUMP_SEL(RTW_DBGDUMP, "map_pged :", map_pged, mapsize);
				sprintf(extra, "WiFi write map compare FAIL\n");
				RTW_INFO("%s: WiFi write map compare Fail\n", __func__);
			}
		}

		if (map_pged)
			rtw_mfree(map_pged, mapsize);
		err = 0;
		goto exit;
	} else if (strcmp(tmp[0], "btfk2map") == 0) {
		u8 reg2chkbt = 0;
		u16 mapsize = 0;
		u8 *map_pged = NULL;

		RTW_INFO("%s:do rtw_efuse_fake2map\n", __func__);
		rtw_efuse_get_map_size(padapter, &mapsize, RTW_EFUSE_CMD_BT_GET_LOG_SIZE);
		shadowmap = rtw_zmalloc(mapsize);
		if (shadowmap == NULL) {
			RTW_INFO("%s: shadowmap rtw_zmalloc Fail\n", __func__);
			err = 0;
			goto exit;
		}
		if (rtw_efuse_get_shadow_map(padapter, shadowmap, mapsize, RTW_EFUSE_BT) == _FAIL) {
			RTW_INFO("%s:rtw_efuse_get_shadow_map Fail\n", __func__);
			err = 0;
			goto exit;
		}

		rtw_phl_write8(dvobj->phl, 0xa3, 0x05);
		rtw_msleep_os(10);
		reg2chkbt = rtw_phl_read8(dvobj->phl, 0xa0);
		RTW_INFO("%s: btfk2map before read 0xa0 BT Status =0x%x\n", __func__,
			rtw_phl_read32(dvobj->phl, 0xa0));

		if (reg2chkbt != 0x04) {
			sprintf(extra, "Error ! BT State not Active:0x%x ,can't Write \n" , reg2chkbt);
			goto exit;
		}

		status = rtw_efuse_fake2map(padapter, RTW_EFUSE_BT);
		if (status == _SUCCESS)
			RTW_INFO("BT write map OK\n");
		else
			RTW_INFO("BT write map FAIL\n");

		map_pged = rtw_zmalloc(mapsize);
		if (map_pged == NULL) {
			RTW_INFO("%s: map_pged rtw_zmalloc Fail\n", __func__);
			err = 0;
			goto exit;
		}

		if (rtw_efuse_map_read(padapter, 0x00, mapsize, map_pged, RTW_EFUSE_BT) == _SUCCESS) {

			if (rtw_efuse_compare_data(padapter, 0x00, mapsize, map_pged, shadowmap, RTW_EFUSE_BT)) {
				RTW_INFO("%s: BT write map afterf compare success\n", __func__);
				sprintf(extra, "BT write map compare OK\n");
			} else {
				RTW_MAP_DUMP_SEL(RTW_DBGDUMP, "shadowmap :", shadowmap, mapsize);
				RTW_MAP_DUMP_SEL(RTW_DBGDUMP, "map_pged :", map_pged, mapsize);
				sprintf(extra, "BT write map compare FAIL\n");
				RTW_INFO("%s: BT write map compare Fail\n", __func__);
			}
		}

		if (map_pged)
			rtw_mfree(map_pged, mapsize);

		err = 0;
		goto exit;
	} else if (strcmp(tmp[0], "update") == 0) {
		if (rtw_efuse_renew_update(padapter, RTW_EFUSE_WIFI) == _FAIL) {
			RTW_INFO("%s: rtw_efuse_renew_update error!!\n", __FUNCTION__);
			sprintf(extra, "WIFI update FAIL\n");
			err = -EFAULT;
			goto exit;
		} else
			sprintf(extra, "WIFI update OK\n");

	}
exit:
	if (setdata)
		rtw_mfree(setdata, RTW_MAX_EFUSE_MAP_LEN);
	if (shadowmap)
		rtw_mfree(shadowmap, size);

	wrqu->length = strlen(extra);

	return err;
}

int rtw_ioctl_efuse_file_map_load(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
	char *rtw_efuse_file_map_path;
	u8 Status = 0;
	_adapter *padapter= rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = &padapter->mppriv;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	rtw_efuse_file_map_path = extra;
	if (rtw_is_file_readable(rtw_efuse_file_map_path) == _TRUE) {
		RTW_INFO("%s do rtw_efuse_mask_file_read = %s!\n", __func__, rtw_efuse_file_map_path);
		Status = rtw_efuse_map_file_load(padapter, rtw_efuse_file_map_path, RTW_EFUSE_WIFI);
		if (Status == _TRUE) {
			pmp_priv->bloadefusemap = _TRUE;
			sprintf(extra, "efuse Map file file_read OK\n");
		} else {
			pmp_priv->bloadefusemap = _FALSE;
			sprintf(extra, "efuse Map file file_read FAIL\n");
		}
	} else {
		sprintf(extra, "efuse file readable FAIL\n");
		RTW_INFO("%s rtw_is_file_readable fail!\n", __func__);
	}

	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_ioctl_efuse_file_mask_load(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
	char *rtw_efuse_file_map_path;
	u8 Status = 0;
	_adapter *padapter= rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = &padapter->mppriv;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	rtw_efuse_file_map_path = extra;
	if (rtw_is_file_readable(rtw_efuse_file_map_path) == _TRUE) {
		RTW_INFO("%s do rtw_efuse_mask_file_read = %s!\n", __func__, rtw_efuse_file_map_path);
		Status = rtw_efuse_mask_file_load(padapter, rtw_efuse_file_map_path, RTW_EFUSE_WIFI);
		if (Status == _TRUE) {
			pmp_priv->bloadefusemap = _TRUE;
			sprintf(extra, "efuse Mask file file_read OK\n");
		} else {
			pmp_priv->bloadefusemap = _FALSE;
			sprintf(extra, "efuse Mask file file_read FAIL\n");
		}
	} else {
		sprintf(extra, "efuse file readable FAIL\n");
		RTW_INFO("%s rtw_is_file_readable fail!\n", __func__);
	}

	wrqu->data.length = strlen(extra);
	return 0;
}


int rtw_ioctl_efuse_bt_file_map_load(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
	char *rtw_efuse_file_map_path;
	u8 Status = 0;
	_adapter *padapter= rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = &padapter->mppriv;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	rtw_efuse_file_map_path = extra;
	if (rtw_is_file_readable(rtw_efuse_file_map_path) == _TRUE) {
		RTW_INFO("%s BT do rtw_efuse_mask_file_read = %s!\n", __func__, rtw_efuse_file_map_path);
		Status = rtw_efuse_map_file_load(padapter, rtw_efuse_file_map_path, RTW_EFUSE_BT);
		if (Status == _TRUE) {
			pmp_priv->bloadefusemap = _TRUE;
			sprintf(extra, "BT efuse Map file file_read OK\n");
		} else {
			pmp_priv->bloadefusemap = _FALSE;
			sprintf(extra, "BT efuse Map file file_read FAIL\n");
		}
	} else {
		sprintf(extra, "BT efuse file readable FAIL\n"
				"indicate the file path and directory to CMD next space,\n"
				"CMD format:rtwpriv wlan0 bt_efuse_file /xx/xx/xx/efuse.map\n");
		RTW_INFO("%s BT rtw_is_file_readable fail!\n", __func__);
	}

	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_ioctl_efuse_bt_file_mask_load(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
	char *rtw_efuse_file_map_path;
	u8 Status = 0;
	_adapter *padapter= rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = &padapter->mppriv;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	rtw_efuse_file_map_path = extra;
	if (rtw_is_file_readable(rtw_efuse_file_map_path) == _TRUE) {
		RTW_INFO("%s BT mask do rtw_efuse_mask_file_read = %s!\n", __func__, rtw_efuse_file_map_path);
		Status = rtw_efuse_mask_file_load(padapter, rtw_efuse_file_map_path, RTW_EFUSE_BT);
		if (Status == _TRUE) {
			pmp_priv->bloadefusemap = _TRUE;
			sprintf(extra, "BT efuse Mask file file_read OK\n");
		} else {
			pmp_priv->bloadefusemap = _FALSE;
			sprintf(extra, "BT efuse Mask file file_read FAIL\n");
		}
	} else {
		sprintf(extra, "BT efuse file readable FAIL\n"
			"indicate the file path and directory to CMD next space,\n"
			"CMD format:rtwpriv wlan0 efuse_bt_mask /xx/xx/xx/efuse.mask\n");
		RTW_INFO("%s BT rtw_is_file_readable fail!\n", __func__);
	}

	wrqu->data.length = strlen(extra);
	return 0;
}

s8 rtw_efuse_get_map_from(_adapter *padapter)
{
	struct rtw_efuse_phl_arg *efuse_arg = NULL;
	u8 map_status = -1;

	efuse_arg = _rtw_malloc(sizeof(struct rtw_efuse_phl_arg));
	if (efuse_arg) {
		_rtw_memset((void *)efuse_arg,
				0,
				sizeof(struct rtw_efuse_phl_arg));
		rtw_efuse_cmd(padapter,
				efuse_arg,
				RTW_MP_EFUSE_CMD_WIFI_GET_MAP_FROM);

		if (efuse_arg->cmd_ok && 
			efuse_arg->status == RTW_PHL_STATUS_SUCCESS) {
			map_status = efuse_arg->io_value;
			RTW_INFO("%s: OK value:%d\n",
				__func__, map_status);
		} else {
			map_status = -1;
			RTW_INFO("%s: fail !\n", __func__);
		}
	}
	if (efuse_arg)
		_rtw_mfree(efuse_arg, sizeof(struct rtw_efuse_phl_arg));

	return map_status;
}

static u8 rtw_efuse_get_realraw(_adapter *padapter, u8 *map, u16 addr, u16 size, u8 efuse_type)
{
	struct rtw_efuse_phl_arg *efuse_arg = NULL;
	u8 res = _FAIL;

	efuse_arg = _rtw_malloc(sizeof(struct rtw_efuse_phl_arg));
	if (efuse_arg) {
		efuse_arg->io_offset = addr;
		efuse_arg->buf_len = size;
		if (efuse_type == RTW_EFUSE_WIFI)
		    rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_WIFI_GET_PHY_MAP);
		else if (efuse_type == RTW_EFUSE_BT)
			rtw_efuse_cmd(padapter, efuse_arg, RTW_EFUSE_CMD_BT_GET_PHY_MAP);
		if (efuse_arg->cmd_ok && efuse_arg->status == RTW_PHL_STATUS_SUCCESS) {
				_rtw_memcpy((void *)map, efuse_arg->poutbuf, size);
				res = _SUCCESS;
		} else
				res = _FAIL;
	}
	if (efuse_arg)
		_rtw_mfree(efuse_arg, sizeof(struct rtw_efuse_phl_arg));

	return res;
}

u8 rtw_efuse_raw_map_read(_adapter * adapter, u16 addr, u16 cnts, u8 *data, u8 efuse_type)
{
	struct dvobj_priv *d;
	u8 *efuse = NULL;
	u16 size, i;
	int err = _FAIL;
	u8 status = _SUCCESS;

	if (efuse_type == RTW_EFUSE_WIFI)
		err = rtw_efuse_get_map_size(adapter, &size, RTW_EFUSE_CMD_WIFI_GET_SIZE);
	else if (efuse_type == RTW_EFUSE_BT)
		err = rtw_efuse_get_map_size(adapter, &size, RTW_EFUSE_CMD_BT_GET_SIZE);

	if (err == _FAIL) {
		status = _FAIL;
		RTW_INFO("get_efuse_size fail\n");
		goto exit;
	}
	/* size error handle */
	if ((addr + cnts) > size) {
		if (addr < size)
			cnts = size - addr;
		else {
			status = _FAIL;
			RTW_INFO(" %s() ,addr + cnts) > size fail\n", __func__);
			goto exit;
		}
	}

	efuse = rtw_zmalloc(size);
	if (efuse) {
		err = rtw_efuse_get_realraw (adapter, efuse, addr, size, efuse_type);
		if (err == _FAIL) {
			rtw_mfree(efuse, size);
			status = _FAIL;
			RTW_INFO(" %s() , phl get realraw fail\n", __func__);
			goto exit;
		}
		if (efuse) {
			RTW_INFO(" %s() ,cp efuse to data\n", __func__);
			_rtw_memcpy(data, efuse + addr, cnts);
			rtw_mfree(efuse, size);
		}
	} else {
			RTW_INFO(" %s() ,alloc efuse fail\n", __func__);
			goto exit;
	}
	status = _SUCCESS;
exit:

	return status;
}

u8 rtw_efuse_bt_write_raw_hidden(_adapter * adapter, u16 addr, u16 cnts, u8 *data)
{
	u8 status = _SUCCESS;
	struct rtw_efuse_phl_arg *efuse_arg = NULL;
	u16 i = 0;

	efuse_arg = _rtw_zmalloc(sizeof(struct rtw_efuse_phl_arg));

	while (i < cnts) {
		efuse_arg->io_type = 1;
		efuse_arg->io_offset = addr + i;
		efuse_arg->io_value = data[i];
		rtw_efuse_cmd(adapter, efuse_arg, RTW_EFUSE_CMD_BT_WRITE_HIDDEN);
		if (efuse_arg->cmd_ok && efuse_arg->status == RTW_PHL_STATUS_SUCCESS)
			status = _SUCCESS;
		else{
			status = _FAIL;
			break;
		}
		i++;
	}
exit :
	if (efuse_arg)
		_rtw_mfree(efuse_arg, sizeof(struct rtw_efuse_phl_arg));

	return status;
}

#endif /*#if defined(CONFIG_MP_INCLUDED)*/

