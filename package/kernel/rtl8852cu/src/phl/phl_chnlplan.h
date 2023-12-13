/******************************************************************************
 *
 * Copyright(c) 2020 Realtek Corporation.
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
#ifndef _PHL_CHNLPLAN_H_
#define _PHL_CHNLPLAN_H_

#define REGULATION_CHPLAN_VERSION 63

enum REGULATION {
    REGULATION_WW        =  0,
    REGULATION_ETSI      =  1,
    REGULATION_FCC       =  2,
    REGULATION_MKK       =  3,
    REGULATION_KCC       =  4,
    REGULATION_NCC       =  5,
    REGULATION_ACMA      =  6,
    REGULATION_NA        =  7,
    REGULATION_IC        =  8,
    REGULATION_CHILE     =  9,
    REGULATION_MEX       = 10,
    REGULATION_MAX       = 11,
};

struct chdef_2ghz {
    /* ch_def index */
    u8 idx;

    /* support channel list
     * support_ch[0]: bit(0~7) stands for ch(1~8)
     * support_ch[1]: bit(0~5) stands for ch (9~14)
     */
    u8 support_ch[2];

    /* passive ch list
     * passive[0]: bit(0~7) stands for ch(1~8)
     * passive[1]: bit(0~5) stands for ch (9~14)
     */
    u8 passive[2];
};

struct chdef_5ghz {
    /* ch_def index */
    u8 idx;

    /*
     * band1 support channel list, passive and dfs
     * bit0 stands for ch36
     * bit1 stands for ch40
     * bit2 stands for ch44
     * bit3 stands for ch48
     */
    u8 support_ch_b1;
    u8 passive_b1;
    u8 dfs_b1;

    /*
     * band2 support channel list, passive and dfs
     * bit0 stands for ch52
     * bit1 stands for ch56
     * bit2 stands for ch60
     * bit3 stands for ch64
     */
    u8 support_ch_b2;
    u8 passive_b2;
    u8 dfs_b2;

    /*
     * band3 support channel list, passive and dfs
     * byte[0]:
     *    bit0 stands for ch100
     *     bit1 stands for ch104
     *     bit2 stands for ch108
     *     bit3 stands for ch112
     *     bit4 stands for ch116
     *     bit5 stands for ch120
     *     bit6 stands for ch124
     *     bit7 stands for ch128
     * byte[1]:
     *    bit0 stands for ch132
     *     bit1 stands for ch136
     *     bit2 stands for ch140
     *     bit3 stands for ch144
     */
    u8 support_ch_b3[2];
    u8 passive_b3[2];
    u8 dfs_b3[2];

    /*
     * band4 support channel list, passive and dfs
     * bit0 stands for ch149
     * bit1 stands for ch153
     * bit2 stands for ch157
     * bit3 stands for ch161
     * bit4 stands for ch165
     * bit5 stands for ch169
     * bit6 stands for ch173
     * bit7 stands for ch177
     */
    u8 support_ch_b4;
    u8 passive_b4;
    u8 dfs_b4;
};

struct freq_plan {
    u8 regulation;
    u8 ch_idx;
};

struct regulatory_domain_mapping {
    u8 domain_code;
    struct freq_plan freq_2g;
    struct freq_plan freq_5g;
};

#define MAX_CHDEF_2GHZ 7
#define MAX_CHDEF_5GHZ 56
#define MAX_RD_MAP_NUM 112



#endif /* _PHL_CHNLPLAN_H_ */
