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
#ifndef _TEST_MODULE_H_
#define _TEST_MODULE_H_

#ifdef CONFIG_PHL_TEST_SUITE
u8 phl_test_module_init(struct phl_info_t *phl_info);
void phl_test_module_deinit(struct rtw_phl_com_t* phl_com);
u8 phl_test_module_start(struct rtw_phl_com_t* phl_com);
void phl_test_module_stop(struct rtw_phl_com_t* phl_com);

#else
#define phl_test_module_init(phl_info) true
#define phl_test_module_deinit(phl_com)
#define phl_test_module_start(phl_com) true
#define phl_test_module_stop(phl_com)
#endif /*CONFIG_PHL_TEST_SUITE*/

#ifdef CONFIG_PHL_TEST_MP
/* phl test mp command */
enum rtw_phl_status phl_test_mp_alloc(struct phl_info_t *phl_info, void *hal, void **mp);
void phl_test_mp_free(void **mp);
void phl_test_mp_init(void *mp);
void phl_test_mp_deinit(void *mp);
void phl_test_mp_start(void *mp, u8 tm_mode);
void phl_test_mp_stop(void *mp, u8 tm_mode);
void phl_test_mp_cmd_process(void *mp, void *buf, u32 buf_len, u8 submdid);
void phl_test_mp_get_rpt(void *mp, void *buf, u32 buf_len);

#else
/* phl test mp command */
#define phl_test_mp_alloc(phl_info, hal, mp) RTW_PHL_STATUS_SUCCESS
#define phl_test_mp_free(mp)
#define phl_test_mp_init(mp)
#define phl_test_mp_deinit(mp)
#define phl_test_mp_start(mp, tm_mode)
#define phl_test_mp_stop(mp, tm_mode)
#define phl_test_mp_cmd_process(mp, buf, buf_len, submdid)
#define phl_test_mp_get_rpt(mp, buf, buf_len)
#endif /* CONFIG_PHL_TEST_MP */

#ifdef CONFIG_PHL_TEST_VERIFY
/* phl test verify command */
enum rtw_phl_status phl_test_verify_alloc(struct phl_info_t *phl_info, void *hal, void **ctx);
void phl_test_verify_free(void **ctx);
void phl_test_verify_init(void *ctx);
void phl_test_verify_deinit(void *ctx);
void phl_test_verify_start(void *ctx);
void phl_test_verify_stop(void *ctx);
void phl_test_verify_cmd_process(void *ctx, void *buf, u32 buf_len, u8 submdid);
void phl_test_verify_get_rpt(void *ctx, void *buf, u32 buf_len);

#else
/* phl test verify command */
#define phl_test_verify_alloc(phl_info, hal, ctx) RTW_PHL_STATUS_SUCCESS
#define phl_test_verify_free(ctx)
#define phl_test_verify_init(ctx)
#define phl_test_verify_deinit(ctx)
#define phl_test_verify_start(ctx)
#define phl_test_verify_stop(ctx)
#define phl_test_verify_cmd_process(ctx, buf, buf_len, submdid)
#define phl_test_verify_get_rpt(ctx, buf, buf_len)
#endif /* CONFIG_PHL_TEST_VERIFY */

#ifdef CONFIG_PHL_TEST_FPGA
/* phl test fpga command */
enum rtw_phl_status phl_test_fpga_alloc(struct phl_info_t *phl_info, void *hal, void **fpga);
void phl_test_fpga_free(void **fpga);
void phl_test_fpga_init(void *fpga);
void phl_test_fpga_deinit(void *fpga);
void phl_test_fpga_start(void *fpga, u8 tm_mode);
void phl_test_fpga_stop(void *fpga, u8 tm_mode);
void phl_test_fpga_cmd_process(void *fpga, void *buf, u32 buf_len, u8 submdid);
void phl_test_fpga_get_rpt(void *fpga, void *buf, u32 buf_len);

#else
/* phl test fpga command */
#define phl_test_fpga_alloc(phl_info, hal, fpga) RTW_PHL_STATUS_SUCCESS
#define phl_test_fpga_free(fpga)
#define phl_test_fpga_init(fpga)
#define phl_test_fpga_deinit(fpga)
#define phl_test_fpga_start(fpga, tm_mode)
#define phl_test_fpga_stop(fpga, tm_mode)
#define phl_test_fpga_cmd_process(fpga, buf, buf_len, submdid)
#define phl_test_fpga_get_rpt(fpga, buf, buf_len)
#endif /* CONFIG_PHL_TEST_FPGA */

#endif	/* _TEST_MODULE_H_ */
