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
#ifndef _PHL_CUSTOM_API_H_
#define _PHL_CUSTOM_API_H_

enum rtw_phl_status
rtw_phl_custom_prepare_set_info_evt(u32 evt_id,
                                    u32 customer_id,
                                    struct rtw_custom_decrpt *inbuf,
                                    struct phl_module_op_info *op_info,
                                    u32 data_len);

enum rtw_phl_status
rtw_phl_custom_prepare_query_info_evt(u32 evt_id,
                                      u32 customer_id,
                                      struct rtw_custom_decrpt *inbuf,
                                      struct phl_module_op_info *op_info,
                                      u32 data_len);

#endif /*_PHL_CUSTOM_API_H_*/
