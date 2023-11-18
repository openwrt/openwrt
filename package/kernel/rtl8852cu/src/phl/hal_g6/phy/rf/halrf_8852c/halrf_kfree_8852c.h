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
#ifndef _HALRF_KFREE_8852C_H_
#define _HALRF_KFREE_8852C_H_
#ifdef RF_8852C_SUPPORT

/*@--------------------------Define Parameters-------------------------------*/

#define HIDE_EFUSE_START_ADDR_8852C	 0x590
#define HIDE_EFUSE_END_ADDR_8852C	 0x5DF
#define HIDE_EFUSE_SIZE_8852C		 HIDE_EFUSE_END_ADDR_8852C - HIDE_EFUSE_START_ADDR_8852C + 1

#define THERMAL_TRIM_HIDE_EFUSE_A_8852C 0x5DF
#define THERMAL_TRIM_HIDE_EFUSE_B_8852C 0x5DC

#define PABIAS_TRIM_HIDE_EFUSE_A_8852C 0x5DE
#define PABIAS_TRIM_HIDE_EFUSE_B_8852C 0x5DB

#define TSSI_TRIM_HIDE_EFUSE_2GL_A_8852C 0x5D6
#define TSSI_TRIM_HIDE_EFUSE_2GH_A_8852C 0x5D5
#define TSSI_TRIM_HIDE_EFUSE_5GL1_A_8852C 0x5D4
#define TSSI_TRIM_HIDE_EFUSE_5GL2_A_8852C 0x5D3
#define TSSI_TRIM_HIDE_EFUSE_5GM1_A_8852C 0x5D2
#define TSSI_TRIM_HIDE_EFUSE_5GM2_A_8852C 0x5D1
#define TSSI_TRIM_HIDE_EFUSE_5GH1_A_8852C 0x5D0
#define TSSI_TRIM_HIDE_EFUSE_5GH2_A_8852C 0x5CF

#define TSSI_TRIM_HIDE_EFUSE_2GL_B_8852C 0x5AB
#define TSSI_TRIM_HIDE_EFUSE_2GH_B_8852C 0x5AA
#define TSSI_TRIM_HIDE_EFUSE_5GL1_B_8852C 0x5A9
#define TSSI_TRIM_HIDE_EFUSE_5GL2_B_8852C 0x5A8
#define TSSI_TRIM_HIDE_EFUSE_5GM1_B_8852C 0x5A7
#define TSSI_TRIM_HIDE_EFUSE_5GM2_B_8852C 0x5A6
#define TSSI_TRIM_HIDE_EFUSE_5GH1_B_8852C 0x5A5
#define TSSI_TRIM_HIDE_EFUSE_5GH2_B_8852C 0x5A4

#define TSSI_TRIM_HIDE_EFUSE_6GL1_A_8852C 0x5CE
#define TSSI_TRIM_HIDE_EFUSE_6GL2_A_8852C 0x5CD
#define TSSI_TRIM_HIDE_EFUSE_6GL3_A_8852C 0x5CC
#define TSSI_TRIM_HIDE_EFUSE_6GL4_A_8852C 0x5CB
#define TSSI_TRIM_HIDE_EFUSE_6GM1_A_8852C 0x5CA
#define TSSI_TRIM_HIDE_EFUSE_6GM2_A_8852C 0x5C9
#define TSSI_TRIM_HIDE_EFUSE_6GM3_A_8852C 0x5C8
#define TSSI_TRIM_HIDE_EFUSE_6GM4_A_8852C 0x5C7
#define TSSI_TRIM_HIDE_EFUSE_6GH1_A_8852C 0x5C6
#define TSSI_TRIM_HIDE_EFUSE_6GH2_A_8852C 0x5C5
#define TSSI_TRIM_HIDE_EFUSE_6GH3_A_8852C 0x5C4
#define TSSI_TRIM_HIDE_EFUSE_6GH4_A_8852C 0x5C3
#define TSSI_TRIM_HIDE_EFUSE_6GUH1_A_8852C 0x5C2
#define TSSI_TRIM_HIDE_EFUSE_6GUH2_A_8852C 0x5C1
#define TSSI_TRIM_HIDE_EFUSE_6GUH3_A_8852C 0x5C0
#define TSSI_TRIM_HIDE_EFUSE_6GUH4_A_8852C 0x5BF

#define TSSI_TRIM_HIDE_EFUSE_6GL1_B_8852C 0x5A3
#define TSSI_TRIM_HIDE_EFUSE_6GL2_B_8852C 0x5A2
#define TSSI_TRIM_HIDE_EFUSE_6GL3_B_8852C 0x5A1
#define TSSI_TRIM_HIDE_EFUSE_6GL4_B_8852C 0x5A0
#define TSSI_TRIM_HIDE_EFUSE_6GM1_B_8852C 0x59F
#define TSSI_TRIM_HIDE_EFUSE_6GM2_B_8852C 0x59E
#define TSSI_TRIM_HIDE_EFUSE_6GM3_B_8852C 0x59D
#define TSSI_TRIM_HIDE_EFUSE_6GM4_B_8852C 0x59C
#define TSSI_TRIM_HIDE_EFUSE_6GH1_B_8852C 0x59B
#define TSSI_TRIM_HIDE_EFUSE_6GH2_B_8852C 0x59A
#define TSSI_TRIM_HIDE_EFUSE_6GH3_B_8852C 0x599
#define TSSI_TRIM_HIDE_EFUSE_6GH4_B_8852C 0x598
#define TSSI_TRIM_HIDE_EFUSE_6GUH1_B_8852C 0x597
#define TSSI_TRIM_HIDE_EFUSE_6GUH2_B_8852C 0x596
#define TSSI_TRIM_HIDE_EFUSE_6GUH3_B_8852C 0x595
#define TSSI_TRIM_HIDE_EFUSE_6GUH4_B_8852C 0x594

/*@-----------------------End Define Parameters-----------------------*/

void halrf_get_efuse_trim_8852c(struct rf_info *rf,
					enum phl_phy_idx phy);

void halrf_kfree_get_info_8852c(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len);

#endif
#endif	/*_HALRF_SET_PWR_TABLE_8852C_H_*/
