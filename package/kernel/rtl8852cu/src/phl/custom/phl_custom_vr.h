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
#ifndef _PHL_CUSTOM_VR_H_
#define _PHL_CUSTOM_VR_H_

#ifdef CONFIG_PHL_CUSTOM_FEATURE_VR

struct _vr_init_param {
	u32 enable;
	u32 test_mode;
	struct rtw_wifi_role_t *wifi_role;
        struct phl_info_t *phl;
};

struct _custom_vr_ctx {
	struct _vr_init_param init;
};

enum phl_mdl_ret_code
phl_custom_hdl_vr_evt(void* dispr,
                      void* custom_ctx,
                      struct _custom_vr_ctx* vr_ctx,
                      struct phl_msg* msg);
enum phl_mdl_ret_code
phl_custom_hdl_vr_fail_evt(void* dispr,
                           void* custom_ctx,
                           struct _custom_vr_ctx* vr_ctx,
                           struct phl_msg* msg);

enum phl_mdl_ret_code
phl_custom_vr_feature_set_hdlr(void* dispr,
                               void* custom_ctx,
                               struct _custom_vr_ctx* vr_ctx,
                               struct rtw_custom_decrpt *cmd);

enum phl_mdl_ret_code
phl_custom_vr_feature_query_hdlr(void* dispr,
                                 void* custom_ctx,
                                 struct _custom_vr_ctx* vr_ctx,
                                 struct rtw_custom_decrpt *cmd);

enum phl_mdl_ret_code
phl_custom_vr_set_role_cap(void* dispr,
                           void* custom_ctx,
                           struct _custom_vr_ctx* vr_ctx,
                           struct phl_module_op_info* info);

enum rtw_phl_status
phl_custom_vr_init_role_link_cap(struct phl_info_t *phl_info,
                                 u8 hw_band,
                                 struct role_link_cap_t *role_link_cap);

#else
#define phl_custom_hdl_vr_evt(_dispr, _custom_ctx, _vr_ctx, _msg) (MDL_RET_IGNORE)
#define phl_custom_hdl_vr_fail_evt(_dispr, _custom_ctx, _vr_ctx, _msg) (MDL_RET_IGNORE)
#define phl_custom_vr_feature_set_hdlr(_dispr, _custom_ctx, _vr_ctx, _cmd) (MDL_RET_IGNORE)
#define phl_custom_vr_feature_query_hdlr(_dispr, _custom_ctx, _vr_ctx, _cmd) (MDL_RET_IGNORE)
#endif

#endif  /*_PHL_CUSTOMIZE_FEATURE_H_*/

