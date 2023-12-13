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
#ifndef _PHL_CHNLPLAN_6GHZ_H_
#define _PHL_CHNLPLAN_6GHZ_H_


/*
 * 6 GHz channel group from UNII-5 to UNII-8
 * channel index diff is 4 : minimum working bandwidth : 20 MHz
 * => next channel index = current index + 4
 */

struct chdef_6ghz {
    /* ch_def index */
    u8 idx;

    /*
     * UNII-5 support channel list, ch1 ~ ch93, total : 24
     * bit0 stands for ch1
     * bit1 stands for ch5
     * bit2 stands for ch9
     * ...
     * bit23 stands for ch93
     */
    u8 support_ch_u5[3];
    u8 passive_u5[3];

    /*
     * UNII-6 support channel list, ch97 ~ ch117, total : 6
     * bit0 stands for ch97
     * bit1 stands for ch101
     * bit2 stands for ch105
     * bit3 stands for ch109
     * bit4 stands for ch113
     * bit5 stands for ch117
     */
    u8 support_ch_u6;
    u8 passive_u6;

    /*
     * UNII-7 support channel list, ch121 ~ ch189, total : 18
     * bit0 stands for ch121
     * bit1 stands for ch125
     * bit2 stands for ch129
     * ...
     * bit17 stands for ch189
     */
    u8 support_ch_u7[3];
    u8 passive_u7[3];

    /*
     * UNII-8 support channel list, ch193 ~ ch237, total : 12
     * bit0 stands for ch193
     * bit1 stands for ch197
     * bit2 stands for ch201
     * ...
     * bit10 stands for ch233
     */
    u8 support_ch_u8[2];
    u8 passive_u8[2];
};

struct regulatory_domain_mapping_6g {
    u8 domain_code;
    u8 regulation;
    u8 ch_idx;
};

#define MAX_CHDEF_6GHZ 7
#define MAX_RD_MAP_NUM_6GHZ 13


#endif /* _PHL_CHNLPLAN_6GHZ_H_ */
