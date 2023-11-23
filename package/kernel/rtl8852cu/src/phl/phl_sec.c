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
#define _PHL_SEC_C_
#include "phl_headers.h"

#define RTW_PHL_EXT_KEY_LEN 32
#define RTW_SEC_KEY_TYPE_NUM 3

static enum rtw_phl_status
_phl_set_key(struct phl_info_t *phl_info,
             struct rtw_phl_stainfo_t *sta,
             struct phl_sec_param_h *crypt,
             u8 *keybuf)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	if(keybuf)
		PHL_INFO("Add_key:: enc_type(%d) key_id(%d) key_type(%d)\n",
			 crypt->enc_type, crypt->keyid, crypt->key_type);
	hal_status = rtw_hal_set_key(phl_info->hal,
	                             sta,
	                             crypt->enc_type,
	                             (crypt->key_len==RTW_PHL_EXT_KEY_LEN)?1:0,
	                             crypt->spp,
	                             crypt->keyid,
	                             crypt->key_type,
	                             keybuf);
	if (hal_status == RTW_HAL_STATUS_SUCCESS)
		phl_status = RTW_PHL_STATUS_SUCCESS;

	return phl_status;
}

#ifdef CONFIG_CMD_DISP
struct cmd_sec_param {
	struct rtw_phl_stainfo_t *sta;
	struct phl_sec_param_h *crypt;
	u8 *keybuf;
};

static void
_phl_cmd_set_key_done(void *drv_priv,
                      u8 *cmd,
                      u32 cmd_len,
                      enum rtw_phl_status status)
{
	struct cmd_sec_param *param = (struct cmd_sec_param *)cmd;

	if (param) {
		if (param->keybuf)
			_os_kmem_free(drv_priv,
			             param->keybuf,
			             param->crypt->key_len);

		if (param->crypt)
			_os_kmem_free(drv_priv,
			             param->crypt,
			             sizeof(struct phl_sec_param_h));

		_os_kmem_free(drv_priv, param, cmd_len);
	}
}

enum rtw_phl_status
_phl_cmd_set_key(void *phl,
                 struct rtw_phl_stainfo_t *sta,
                 struct phl_sec_param_h *crypt,
                 u8 *keybuf,
                 enum phl_cmd_type cmd_type,
                 u32 cmd_timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;

	struct cmd_sec_param *param = NULL;
	u32 param_len = 0, crypt_len = 0;

	if (cmd_type == PHL_CMD_DIRECTLY) {
		psts = _phl_set_key(phl_info, sta, crypt, keybuf);
		goto _exit;
	}

	param_len = sizeof(struct cmd_sec_param);
	param = _os_kmem_alloc(drv, param_len);
	if (param == NULL) {
		PHL_ERR("%s: alloc param failed!\n", __func__);
		psts = RTW_PHL_STATUS_RESOURCE;
		goto error_param;
	}
	_os_mem_set(drv, param, 0, param_len);
	param->sta = sta;

	crypt_len = sizeof(struct phl_sec_param_h);
	param->crypt = _os_kmem_alloc(drv, crypt_len);
	if (param->crypt == NULL) {
		PHL_ERR("%s: alloc param->crypt failed!\n", __func__);
		psts = RTW_PHL_STATUS_RESOURCE;
		goto error_crypt;
	}
	_os_mem_set(drv, param->crypt, 0, crypt_len);
	_os_mem_cpy(drv, param->crypt, crypt, crypt_len);

	if (keybuf) { /* set key */
		param->keybuf = _os_kmem_alloc(drv, param->crypt->key_len);
		if (param->keybuf == NULL) {
			PHL_ERR("%s: alloc param->keybuf failed!\n", __func__);
			psts = RTW_PHL_STATUS_RESOURCE;
			goto error_key_buf;
		}
		_os_mem_cpy(drv, param->keybuf, keybuf, param->crypt->key_len);
	}

	psts = phl_cmd_enqueue(phl_info,
	                       sta->rlink->hw_band,
	                       MSG_EVT_SEC_KEY,
	                       (u8 *)param,
	                       param_len,
	                       _phl_cmd_set_key_done,
	                       cmd_type,
	                       cmd_timeout);

	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
		goto error_cmd;
	}

	return psts;

error_cmd:
	if (param->keybuf)
		_os_kmem_free(drv, param->keybuf, param->crypt->key_len);
error_key_buf:
	if (param->crypt)
		_os_kmem_free(drv, param->crypt, crypt_len);

error_crypt:
	if (param)
		_os_kmem_free(drv, param, param_len);

error_param:
_exit:
	return psts;
}

enum rtw_phl_status
phl_cmd_set_key_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct cmd_sec_param *cmd_sec_param = (struct cmd_sec_param *)param;

	return _phl_set_key(phl_info,
	                    cmd_sec_param->sta,
	                    cmd_sec_param->crypt,
	                    cmd_sec_param->keybuf);
}
#endif /* CONFIG_CMD_DISP */

enum rtw_phl_status
rtw_phl_cmd_add_key(void *phl,
                    struct rtw_phl_stainfo_t *sta,
                    struct phl_sec_param_h *crypt,
                    u8 *keybuf,
                    enum phl_cmd_type cmd_type,
                    u32 cmd_timeout)
{
#ifdef CONFIG_CMD_DISP
	return _phl_cmd_set_key(phl, sta, crypt, keybuf, cmd_type, cmd_timeout);
#else
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s: not support add key cmd\n",
	          __func__);

	return _phl_set_key((struct phl_info_t *)phl, sta, crypt, keybuf);
#endif /* CONFIG_CMD_DISP */
}

enum rtw_phl_status
rtw_phl_cmd_del_key(void *phl,
                    struct rtw_phl_stainfo_t *sta,
                    struct phl_sec_param_h *crypt,
                    enum phl_cmd_type cmd_type,
                    u32 cmd_timeout)
{
#ifdef CONFIG_CMD_DISP
	return _phl_cmd_set_key(phl, sta, crypt, NULL, cmd_type, cmd_timeout);
#else
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s: not support del key cmd\n",
	          __func__);

	return _phl_set_key((struct phl_info_t *)phl, sta, crypt, NULL);
#endif /* CONFIG_CMD_DISP */
}

u8 rtw_phl_trans_sec_mode(u8 unicast, u8 multicast)
{
	u8	ret = RTW_SEC_ENT_MODE_0;

	if (RTW_ENC_NONE == unicast && RTW_ENC_NONE == multicast) {
		ret = RTW_SEC_ENT_MODE_0;
	} else if ((RTW_ENC_WEP40 == unicast && RTW_ENC_WEP40 == multicast) ||
		(RTW_ENC_WEP104 == unicast && RTW_ENC_WEP104 == multicast)) {
		ret = RTW_SEC_ENT_MODE_1;
	} else if (RTW_ENC_WEP40 == multicast || RTW_ENC_WEP104 == multicast) {
		ret = RTW_SEC_ENT_MODE_3;
	} else {
		ret = RTW_SEC_ENT_MODE_2;
	}

	return ret;
}

u8 rtw_phl_get_sec_cam_idx(void *phl,
                           struct rtw_phl_stainfo_t *sta,
                           u8 keyid,
                           u8 key_type)
{
	u8 ret = 0;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	ret = (u8) rtw_hal_search_key_idx(phl_info->hal, sta, keyid, key_type);

	return ret;
}
