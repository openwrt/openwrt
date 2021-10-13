/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#ifndef _MPGE_PHY_H_
#define _MPGE_PHY_H_

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */
/*MII register*/
#define MPGE_PHY_CONTROL                          0x0
#define MPGE_PHY_FIFO_CONTROL                     0x19
#define MPGE_PHY_INTR_MASK                        0x12
#define MPGE_PHY_INTR_STATUS                      0x13

    /*MII register field*/
#define MPGE_CTRL_AUTONEGOTIATION_ENABLE          0x1000
#define MPGE_CTRL_RESTART_AUTONEGOTIATION         0x0200
#define MPGE_CTRL_FULL_DUPLEX                     0x0100
#define MPGE_CONTROL_SPEED_MASK                   0x2040
#define MPGE_CONTROL_100M                         0x2000
#define MPGE_CONTROL_10M                          0x0
#define MPGE_PHY_FIFO_RESET                       0x3
#define MPGE_INTR_SPEED_CHANGE                    0x4000
#define MPGE_INTR_DUPLEX_CHANGE                   0x2000
#define MPGE_INTR_STATUS_LINK_DOWN                0x0800
#define MPGE_INTR_STATUS_LINK_UP                  0x0400
#define MPGE_INTR_DOWNSHIF                        0x0020
#define MPGE_INTR_WOL                             0x0001
#define MPGE_INTR_FAST_LINK_DOWN_STAT_10M         0x40
#define MPGE_INTR_FAST_LINK_DOWN_STAT_100M        0x200
#define MPGE_INTR_FAST_LINK_DOWN_STAT_1000M       0x240
#define MPGE_COMMON_CTRL                          0x1040
#define MPGE_10M_LOOPBACK                         0x4100
#define MPGE_100M_LOOPBACK                        0x6100
#define MPGE_1000M_LOOPBACK                       0x4140

/*MMD1 register*/
#define MPGE_PHY_MMD1_NUM                         0x1
#define MPGE_PHY_MMD1_MSE_THRESH1                 0x1000
#define MPGE_PHY_MMD1_MSE_THRESH2                 0x1001
#define MPGE_PHY_MMD1_DAC                         0x8100
/*MMD1 register field*/
#define MPGE_PHY_MMD1_MSE_THRESH1_VAL             0xf1
#define MPGE_PHY_MMD1_MSE_THRESH2_VAL             0x1f6

/*MMD3 register*/
#define MPGE_PHY_MMD3_NUM                         0x3
#define MPGE_PHY_MMD3_AZ_CTRL1                    0x8008
#define MPGE_PHY_MMD3_AZ_CTRL2                    0x8009
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL3            0x8074
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL4            0x8075
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL5            0x8076
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL6            0x8077
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL7            0x8078
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL9            0x807a
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL13           0x807e
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL14           0x807f

/*MMD3 register field*/
#define MPGE_PHY_MMD3_AZ_CTRL1_VAL                0x7880
#define MPGE_PHY_MMD3_AZ_CTRL2_VAL                0xc8
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL3_VAL        0xc040
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL4_VAL        0xa060
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL5_VAL        0xc040
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL6_VAL        0xa060
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL7_VAL        0xc24c
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL9_VAL        0xc060
#define MPGE_PHY_MMD3_CDT_THRESH_CTRL13_VAL       0xb060
#define MPGE_PHY_MMD3_NEAR_ECHO_THRESH_VAL        0x90b0

/*debug register*/
#define MPGE_PHY_DEBUG_EDAC                       0x4380

/*debug port analog*/
#define MPGE_PHY_DEBUG_ANA_LDO_EFUSE              0x180
#define MPGE_PHY_DEBUG_ANA_DAC_FILTER             0xa080

#define MPGE_PHY_DEBUG_ANA_LDO_EFUSE_DEFAULT      0x50

int mpge_phy_init(a_uint32_t dev_id, a_uint32_t port_bmp);

#ifdef __cplusplus
}
#endif				/* __cplusplus */
#endif				/* _MPGE_PHY_H_ */
