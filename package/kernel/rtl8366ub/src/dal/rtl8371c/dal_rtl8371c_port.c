/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (c) 2025 Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided as is and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8371c/dal_rtl8371c_port.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>
#include <osal/lib.h>

#define RTL8371C_PORT_SPEED_10M         0x0
#define RTL8371C_PORT_SPEED_100M        0x1
#define RTL8371C_PORT_SPEED_1000M       0x2
#define RTL8371C_PORT_SPEED_500M        0x3
#define RTL8371C_PORT_SPEED_2500M       0x5
#define RTL8371C_PORT_SPEED_5G          0x6
#define RTL8371C_PORT_SPEED_10G         0x4

#define RTL8371C_SDS_MODE_DISABLE           0x1f
#define RTL8371C_SDS_MODE_USXGMII           0x0d
#define RTL8371C_SDS_MODE_10GR              0x1a
#define RTL8371C_SDS_MODE_5GR               0x1a
#define RTL8371C_SDS_MODE_SGMII             0x02
#define RTL8371C_SDS_MODE_HSGMII            0x12
#define RTL8371C_SDS_MODE_FIBER_2P5G        0x16
#define RTL8371C_SDS_MODE_FIBER_1000X       0x04
#define RTL8371C_SDS_MODE_FIBER_100FX       0x05
#define RTL8371C_SDS_MODE_FIBER_1000X_100FX 0x07

#define RTL8371C_PHY_WRITE_OPERATION    1
#define RTL8371C_PHY_READ_OPERATION     0
#define RTL8371C_PHY_ACCESS_CMD         1
#define RTL8371C_PHY_ACCESS_TIMEOUT     100

#define RTL8371C_SDS_READ_OPERATION     0
#define RTL8371C_SDS_WRITE_OPERATION    1
#define RTL8371C_SDS_ACCESS_TIMEOUT     100

#define REG_BIT(__data__, bit)              ((__data__ >> bit) & 0x0001)

static rtksw_api_ret_t _dal_rtl8371c_port_extMode_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_mode_ext_t mode)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 i;
    rtksw_uint32 regData;
    rtksw_uint32 USXGMIICfg[][2] =              {{0x304c, 0x05cb}, {0x92a4, 0x05cc}, {0x7300, 0x0422}, {0xfe7f, 0x042b}};
    rtksw_uint32 sds10GRCfg[][2] =              {{0x304c, 0x05cb}, {0x92a4, 0x05cc}, {0x7300, 0x0422}, {0xfe7f, 0x042b}, {0x000f, 0x03eb}};
    rtksw_uint32 sds5GRCfg[][2] =               {{0x7300, 0x0422}, {0x000f, 0x03eb}};
    rtksw_uint32 sds1HSGMIICfg[][2] =           {{0x1e58, 0x0530}, {0x8023, 0x0521}};
    rtksw_uint32 sds1Fiber2P5GCfg[][2] =        {{0x3056, 0x050b}, {0x91a4, 0x050c}, {0x1e58, 0x0530}, {0x8023, 0x0521}, {0x01a0, 0x0044}};
    rtksw_uint32 sds1SGMIICfg[][2] =            {{0x2858, 0x04b0}, {0x8023, 0x04a1}, {0xf0f1, 0x0488}};
    rtksw_uint32 sds1Fiber1000xCfg[][2] =       {{0x3056, 0x048b}, {0x2858, 0x04b0}, {0x8023, 0x04a1}, {0xf0f1, 0x0488}, {0x01a0, 0x0044}};
    rtksw_uint32 sds1Fiber100fxCfg[][2] =       {{0xf0f1, 0x0488}, {0x2100, 0x0040}, {0x01a0, 0x0044}};
    rtksw_uint32 sds1Fiber1000x100fxCfg[][2] =  {{0x3056, 0x048b}, {0x2858, 0x04b0}, {0x8023, 0x04a1}, {0xf0f1, 0x0488}, {0x01a0, 0x0044}};
    rtksw_uint32 sds0HSGMIICfg[][2] =           {{0x030b, 0x051a}, {0x1082, 0x051e}};
    rtksw_uint32 sds0Fiber2P5GCfg[][2] =        {{0x0a28, 0x050a}, {0x000a, 0x0509}, {0x030b, 0x051a}, {0x1082, 0x051e}, {0x01a0, 0x0044}};
    rtksw_uint32 sds0SGMIICfg[][2] =            {{0x0293, 0x049a}, {0x1082, 0x049e}};
    rtksw_uint32 sds0Fiber1000xCfg[][2] =       {{0x000a, 0x0489}, {0x0293, 0x049a}, {0x1082, 0x049e}, {0x01a0, 0x0044}};
    rtksw_uint32 sds0Fiber100fxCfg[][2] =       {{0x2100, 0x0040}, {0x01a0, 0x0044}};
    rtksw_uint32 sds0Fiber1000x100fxCfg[][2] =  {{0x000a, 0x0489}, {0x0293, 0x049a}, {0x1082, 0x049e}, {0x01a0, 0x0044}};

    if(port == EXT_PORT0)
    {
        if (mode == RTKSW_MODE_EXT_DISABLE)
        {
            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_HSGMII)
        {
            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_HSGMII;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            for(i = 0; i < sizeof(sds0HSGMIICfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds0HSGMIICfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds0HSGMIICfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc0;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_HSGMII;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_SGMII)
        {
            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_SGMII;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            for(i = 0; i < sizeof(sds0SGMIICfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds0SGMIICfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds0SGMIICfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc0;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_SGMII;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_FIBER_2P5G)
        {
            regData = 0x3030;
            if ((retVal = reg16_write(unit, RTL8371C_SMI_CTRLr, &regData)) != RT_ERR_OK)
                return RT_ERR_OK;

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_2P5G;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;
            
            for(i = 0; i < sizeof(sds0Fiber2P5GCfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds0Fiber2P5GCfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds0Fiber2P5GCfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc0;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_2P5G;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_1000X)
        {
            regData = 0x3030;
            if ((retVal = reg16_write(unit, RTL8371C_SMI_CTRLr, &regData)) != RT_ERR_OK)
                return RT_ERR_OK;

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_1000X;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            for(i = 0; i < sizeof(sds0Fiber1000xCfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds0Fiber1000xCfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds0Fiber1000xCfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc0;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_1000X;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_100FX)
        {
            regData = 0x3030;
            if ((retVal = reg16_write(unit, RTL8371C_SMI_CTRLr, &regData)) != RT_ERR_OK)
                return RT_ERR_OK;

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_100FX;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;
            
            for(i = 0; i < sizeof(sds0Fiber100fxCfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds0Fiber100fxCfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds0Fiber100fxCfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc0;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_100FX;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_1000X_100FX)
        {
            regData = 0x3030;
            if ((retVal = reg16_write(unit, RTL8371C_SMI_CTRLr, &regData)) != RT_ERR_OK)
                return RT_ERR_OK;

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_1000X_100FX;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;
       
            for(i = 0; i < sizeof(sds0Fiber1000x100fxCfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds0Fiber1000x100fxCfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds0Fiber1000x100fxCfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc0;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_1000X_100FX;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else
            return RT_ERR_INPUT;
    }
    else /* EXT_PORT1 */
    {
        if (mode == RTKSW_MODE_EXT_DISABLE)
        {
            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_USXGMII)
        {
            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_USXGMII;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x00;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;

            for(i = 0; i < sizeof(USXGMIICfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &USXGMIICfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &USXGMIICfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc1;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_USXGMII;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_5GR)
        {
            regData = 0x3030;
            if ((retVal = reg16_write(unit, RTL8371C_SMI_CTRLr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_5GR;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x2;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;

            for(i = 0; i < sizeof(sds5GRCfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds5GRCfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds5GRCfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc1;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_5GR;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x2;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_10GR)
        {
            regData = 0x3030;
            if ((retVal = reg16_write(unit, RTL8371C_SMI_CTRLr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_10GR;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;

            for(i = 0; i < sizeof(sds10GRCfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds10GRCfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds10GRCfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc1;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_10GR;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_SGMII)
        {
            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_SGMII;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;

            for(i = 0; i < sizeof(sds1SGMIICfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds1SGMIICfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds1SGMIICfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc1;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_SGMII;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_HSGMII)
        {
            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_HSGMII;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;
            
            for(i = 0; i < sizeof(sds1HSGMIICfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds1HSGMIICfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds1HSGMIICfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc1;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_HSGMII;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_FIBER_2P5G)
        {
            regData = 0x3030;
            if ((retVal = reg16_write(unit, RTL8371C_SMI_CTRLr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_2P5G;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;

            for(i = 0; i < sizeof(sds1Fiber2P5GCfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds1Fiber2P5GCfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds1Fiber2P5GCfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc1;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_2P5G;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_1000X)
        {
            regData = 0x3030;
            if ((retVal = reg16_write(unit, RTL8371C_SMI_CTRLr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_1000X;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;

            for(i = 0; i < sizeof(sds1Fiber1000xCfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds1Fiber1000xCfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds1Fiber1000xCfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc1;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_1000X;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_100FX)
        {
            regData = 0x3030;
            if ((retVal = reg16_write(unit, RTL8371C_SMI_CTRLr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_100FX;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;

            for(i = 0; i < sizeof(sds1Fiber100fxCfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds1Fiber100fxCfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds1Fiber100fxCfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc1;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_100FX;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else if (mode == RTKSW_MODE_EXT_1000X_100FX)
        {
            regData = 0x3030;
            if ((retVal = reg16_write(unit, RTL8371C_SMI_CTRLr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_1000X_100FX;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;
            
            for(i = 0; i < sizeof(sds1Fiber1000x100fxCfg) / (sizeof(rtksw_uint32) * 2); i++)
            {
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_DATAr, &sds1Fiber1000x100fxCfg[i][0])) != RT_ERR_OK)
                    return RT_ERR_OK;

                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_ADRr, &sds1Fiber1000x100fxCfg[i][1])) != RT_ERR_OK)
                    return RT_ERR_OK;

                regData = 0xc1;
                if ((retVal = reg16_write(unit, RTL8371C_SDS_INDACS_CMDr, &regData)) != RT_ERR_OK)
                    return RT_ERR_OK;
            }

            regData = RTL8371C_SDS_MODE_DISABLE;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;

            regData = RTL8371C_SDS_MODE_FIBER_1000X_100FX;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MODE_CTRL_0r, RTL8371C_CFG_SDS_MODE_1f, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData = 0x0;
            if ((retVal = reg16_field_write(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData)) != RT_ERR_OK)
                return retVal;
        }
        else
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

static rtksw_api_ret_t _dal_rtl8371c_port_extMode_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_mode_ext_t *pMode)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 field;
    rtksw_uint32 regData;
    rtksw_uint32 regData2;

    if(port == EXT_PORT0)
        field = RTL8371C_CFG_SDS_MODE_0f;
    else if(port == EXT_PORT1)
        field = RTL8371C_CFG_SDS_MODE_1f;
    else
        return RT_ERR_PORT_ID;
    
    if ((retVal = reg16_field_read(unit, RTL8371C_SDS_MODE_CTRL_0r, field, &regData)) != RT_ERR_OK)
                return retVal;

    switch (regData)
    { 
        case RTL8371C_SDS_MODE_DISABLE:
            *pMode = RTKSW_MODE_EXT_DISABLE;
            break;
        case RTL8371C_SDS_MODE_USXGMII:
            *pMode = RTKSW_MODE_EXT_USXGMII;
            break;
        case RTL8371C_SDS_MODE_10GR:
            if ((retVal = reg16_field_read(unit, RTL8371C_SDS_MISC_2r, RTL8371C_USX_SUB_MX_S0f, &regData2)) != RT_ERR_OK)
                return retVal;

            if (regData2 == 0x2)
                *pMode = RTKSW_MODE_EXT_5GR;
            else    
                *pMode = RTKSW_MODE_EXT_10GR;

            break;
        case RTL8371C_SDS_MODE_SGMII:
            *pMode = RTKSW_MODE_EXT_SGMII;
            break;
        case RTL8371C_SDS_MODE_HSGMII:
            *pMode = RTKSW_MODE_EXT_HSGMII;
            break;
        case RTL8371C_SDS_MODE_FIBER_2P5G:
            *pMode = RTKSW_MODE_EXT_FIBER_2P5G;
            break;
        case RTL8371C_SDS_MODE_FIBER_1000X:
            *pMode = RTKSW_MODE_EXT_1000X;
            break;
        case RTL8371C_SDS_MODE_FIBER_100FX:
            *pMode = RTKSW_MODE_EXT_100FX;
            break;
        case RTL8371C_SDS_MODE_FIBER_1000X_100FX:
            *pMode = RTKSW_MODE_EXT_1000X_100FX;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyAutoNegoAbility_set
 * Description:
 *      Set ethernet PHY auto-negotiation desired ability.
 * Input:
 *      unit        - Unit ID
 *      port        - port id.
 *      pAbility    - Ability structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      If Full_1000 bit is set to 1, the AutoNegotiation will be automatic set to 1. While both AutoNegotiation and Full_1000 are set to 0, the PHY speed and duplex selection will
 *      be set as following 100F > 100H > 10F > 10H priority sequence.
 */
rtksw_api_ret_t dal_rtl8371c_port_phyAutoNegoAbility_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_ability_t *pAbility)
{
    rtksw_api_ret_t       retVal;
    rtksw_uint32          phyData;
    rtksw_uint32          phyEnMsk0;
    rtksw_uint32          phyEnMsk4;
    rtksw_uint32          phyEnMsk9;
    rtksw_uint32          phyEnMsk2p5g;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if(NULL == pAbility)
        return RT_ERR_NULL_POINTER;

    if (pAbility->Half_10 >= RTKSW_ENABLE_END || pAbility->Full_10 >= RTKSW_ENABLE_END ||
       pAbility->Half_100 >= RTKSW_ENABLE_END || pAbility->Full_100 >= RTKSW_ENABLE_END ||
       pAbility->Full_1000 >= RTKSW_ENABLE_END || pAbility->AutoNegotiation >= RTKSW_ENABLE_END ||
       pAbility->Full_2P5G >= RTKSW_ENABLE_END ||
       pAbility->AsyFC >= RTKSW_ENABLE_END || pAbility->FC >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    /* Speed Check */
    if ((rtksw_switch_isUtp2p5gPort(unit, port) != RT_ERR_OK) && (pAbility->Full_2P5G == 1))
        return RT_ERR_PHY_SPEED;

    /*for PHY auto mode setup*/
    pAbility->AutoNegotiation = 1;

    phyEnMsk0 = 0;
    phyEnMsk4 = 0;
    phyEnMsk9 = 0;
    phyEnMsk2p5g = 0;

    if (1 == pAbility->Half_10)
    {
        /*10BASE-TX half duplex capable in reg 4.5*/
        phyEnMsk4 = phyEnMsk4 | (1 << 5);

        /*Speed selection [1:0] */
        /* 11=Reserved*/
        /* 10= 1000Mpbs*/
        /* 01= 100Mpbs*/
        /* 00= 10Mpbs*/
        phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
        phyEnMsk0 = phyEnMsk0 & (~(1 << 13));
    }

    if (1 == pAbility->Full_10)
    {
        /*10BASE-TX full duplex capable in reg 4.6*/
        phyEnMsk4 = phyEnMsk4 | (1 << 6);
        /*Speed selection [1:0] */
        /* 11=Reserved*/
        /* 10= 1000Mpbs*/
        /* 01= 100Mpbs*/
        /* 00= 10Mpbs*/
        phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
        phyEnMsk0 = phyEnMsk0 & (~(1 << 13));

        /*Full duplex mode in reg 0.8*/
        phyEnMsk0 = phyEnMsk0 | (1 << 8);

    }

    if (1 == pAbility->Half_100)
    {
        /*100BASE-TX half duplex capable in reg 4.7*/
        phyEnMsk4 = phyEnMsk4 | (1 << 7);
        /*Speed selection [1:0] */
        /* 11=Reserved*/
        /* 10= 1000Mpbs*/
        /* 01= 100Mpbs*/
        /* 00= 10Mpbs*/
        phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
        phyEnMsk0 = phyEnMsk0 | (1 << 13);
    }


    if (1 == pAbility->Full_100)
    {
        /*100BASE-TX full duplex capable in reg 4.8*/
        phyEnMsk4 = phyEnMsk4 | (1 << 8);
        /*Speed selection [1:0] */
        /* 11=Reserved*/
        /* 10= 1000Mpbs*/
        /* 01= 100Mpbs*/
        /* 00= 10Mpbs*/
        phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
        phyEnMsk0 = phyEnMsk0 | (1 << 13);
        /*Full duplex mode in reg 0.8*/
        phyEnMsk0 = phyEnMsk0 | (1 << 8);
    }

    if (1 == pAbility->Full_1000)
    {
        /*1000 BASE-T FULL duplex capable setting in reg 9.9*/
        phyEnMsk9 = phyEnMsk9 | (1 << 9);

        /*Speed selection [1:0] */
        /* 11=Reserved*/
        /* 10= 1000Mpbs*/
        /* 01= 100Mpbs*/
        /* 00= 10Mpbs*/
        phyEnMsk0 = phyEnMsk0 | (1 << 6);
        phyEnMsk0 = phyEnMsk0 & (~(1 << 13));

        /*Auto-Negotiation setting in reg 0.12*/
        phyEnMsk0 = phyEnMsk0 | (1 << 12);
    }

    if (1 == pAbility->Full_2P5G)
    {
        phyEnMsk2p5g |= (1 << 7);

        /*Auto-Negotiation setting in reg 0.12*/
        phyEnMsk0 = phyEnMsk0 | (1 << 12);
    }

    if (1 == pAbility->AutoNegotiation)
    {
        /*Auto-Negotiation setting in reg 0.12*/
        phyEnMsk0 = phyEnMsk0 | (1 << 12);
    }

    if (1 == pAbility->AsyFC)
    {
        /*Asymetric flow control in reg 4.11*/
        phyEnMsk4 = phyEnMsk4 | (1 << 11);
    }
    if (1 == pAbility->FC)
    {
        /*Flow control in reg 4.10*/
        phyEnMsk4 = phyEnMsk4 | (1 << 10);
    }

    /*1000 BASE-T control register setting*/
    if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_1000_BASET_CONTROL_REG, &phyData)) != RT_ERR_OK)
        return retVal;

    phyData = (phyData & (~0x0200)) | phyEnMsk9 ;

    if ((retVal = dal_rtl8371c_port_phyReg_set(unit, port, PHY_1000_BASET_CONTROL_REG, phyData)) != RT_ERR_OK)
        return retVal;

    /*Auto-Negotiation control register setting*/
    if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_AN_ADVERTISEMENT_REG, &phyData)) != RT_ERR_OK)
        return retVal;

    phyData = (phyData & (~0x0DE0)) | phyEnMsk4;
    if ((retVal = dal_rtl8371c_port_phyReg_set(unit, port, PHY_AN_ADVERTISEMENT_REG, phyData)) != RT_ERR_OK)
        return retVal;

    /* 2.5G ability setting*/
    if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xA5D4, &phyData)) != RT_ERR_OK)
        return retVal;

    phyData = (phyData & (~0x0080)) | phyEnMsk2p5g;
    if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xA5D4, phyData)) != RT_ERR_OK)
        return retVal;

    /*Control register setting and restart auto*/
    if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
        return retVal;

    phyData = (phyData & (~0x3140)) | phyEnMsk0;
    /*If have auto-negotiation capable, then restart auto negotiation*/
    if (1 == pAbility->AutoNegotiation)
    {
        phyData = phyData | (1 << 9);
    }

    if ((retVal = dal_rtl8371c_port_phyReg_set(unit, port, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyAutoNegoAbility_get
 * Description:
 *      Get PHY ability through PHY registers.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pAbility - Ability structure
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      Get the capablity of specified PHY.
 */
rtksw_api_ret_t dal_rtl8371c_port_phyAutoNegoAbility_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_ability_t *pAbility)
{
    rtksw_api_ret_t       retVal;
    rtksw_uint32          phyData0;
    rtksw_uint32          phyData4;
    rtksw_uint32          phyData9;
    rtksw_uint32          phyEnMsk2p5g;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if(NULL == pAbility)
        return RT_ERR_NULL_POINTER;

    /*Control register setting and restart auto*/
    if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return retVal;

    /*Auto-Negotiation control register setting*/
    if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return retVal;

    /*1000 BASE-T control register setting*/
    if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
        return retVal;

    /* 2.5G BASE-T control register setting */
    if (rtksw_switch_isUtp2p5gPort(unit, port) == RT_ERR_OK)
    {
        if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xA5D4, &phyEnMsk2p5g)) != RT_ERR_OK)
            return retVal;
    }

    if (phyEnMsk2p5g & (1 << 7))
        pAbility->Full_2P5G = 1;
    else
        pAbility->Full_2P5G = 0;

    if (phyData9 & (1 << 9))
        pAbility->Full_1000 = 1;
    else
        pAbility->Full_1000 = 0;

    if (phyData4 & (1 << 11))
        pAbility->AsyFC = 1;
    else
        pAbility->AsyFC = 0;

    if (phyData4 & (1 << 10))
        pAbility->FC = 1;
    else
        pAbility->FC = 0;

    if (phyData4 & (1 << 8))
        pAbility->Full_100 = 1;
    else
        pAbility->Full_100 = 0;

    if (phyData4 & (1 << 7))
        pAbility->Half_100 = 1;
    else
        pAbility->Half_100 = 0;

    if (phyData4 & (1 << 6))
        pAbility->Full_10 = 1;
    else
        pAbility->Full_10 = 0;

    if (phyData4 & (1 << 5))
        pAbility->Half_10 = 1;
    else
        pAbility->Half_10 = 0;

    if (phyData0 & (1 << 12))
        pAbility->AutoNegotiation = 1;
    else
        pAbility->AutoNegotiation = 0;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyForceModeAbility_set
 * Description:
 *      Set the port speed/duplex mode/pause/asy_pause in the PHY force mode.
 * Input:
 *      unit        - Unit ID
 *      port        - port id.
 *      pAbility    - Ability structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      While both AutoNegotiation and Full_1000 are set to 0, the PHY speed and duplex selection will
 *      be set as following 100F > 100H > 10F > 10H priority sequence.
 *      This API can be used to configure combo port in fiber mode.
 *      The possible parameters in fiber mode are Full_1000 and Full 100.
 *      All the other fields in rtksw_port_phy_ability_t will be ignored in fiber port.
 */
rtksw_api_ret_t dal_rtl8371c_port_phyForceModeAbility_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_ability_t *pAbility)
{
    rtksw_api_ret_t      retVal;
    rtksw_uint32         phyData;
    rtksw_uint32         phyEnMsk0;
    rtksw_uint32         phyEnMsk4;
    rtksw_uint32         phyEnMsk9;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if(NULL == pAbility)
        return RT_ERR_NULL_POINTER;

    if (pAbility->Half_10 >= RTKSW_ENABLE_END || pAbility->Full_10 >= RTKSW_ENABLE_END ||
        pAbility->Half_100 >= RTKSW_ENABLE_END || pAbility->Full_100 >= RTKSW_ENABLE_END ||
        pAbility->Full_1000 >= RTKSW_ENABLE_END || pAbility->AutoNegotiation >= RTKSW_ENABLE_END ||
        pAbility->AsyFC >= RTKSW_ENABLE_END || pAbility->FC >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    /* Speed Check */
    if ((pAbility->Full_1000 == 1) || (pAbility->Full_2P5G == 1))
        return RT_ERR_PHY_SPEED;

    /*for PHY force mode setup*/
     pAbility->AutoNegotiation = 0;

     phyEnMsk0 = 0;
     phyEnMsk4 = 0;
     phyEnMsk9 = 0;

     if (1 == pAbility->Half_10)
     {
         /*10BASE-TX half duplex capable in reg 4.5*/
         phyEnMsk4 = phyEnMsk4 | (1 << 5);

         /*Speed selection [1:0] */
         /* 11=Reserved*/
         /* 10= 1000Mpbs*/
         /* 01= 100Mpbs*/
         /* 00= 10Mpbs*/
         phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
         phyEnMsk0 = phyEnMsk0 & (~(1 << 13));
     }

     if (1 == pAbility->Full_10)
     {
         /*10BASE-TX full duplex capable in reg 4.6*/
         phyEnMsk4 = phyEnMsk4 | (1 << 6);
         /*Speed selection [1:0] */
         /* 11=Reserved*/
         /* 10= 1000Mpbs*/
         /* 01= 100Mpbs*/
         /* 00= 10Mpbs*/
         phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
         phyEnMsk0 = phyEnMsk0 & (~(1 << 13));

         /*Full duplex mode in reg 0.8*/
         phyEnMsk0 = phyEnMsk0 | (1 << 8);

     }

     if (1 == pAbility->Half_100)
     {
         /*100BASE-TX half duplex capable in reg 4.7*/
         phyEnMsk4 = phyEnMsk4 | (1 << 7);
         /*Speed selection [1:0] */
         /* 11=Reserved*/
         /* 10= 1000Mpbs*/
         /* 01= 100Mpbs*/
         /* 00= 10Mpbs*/
         phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
         phyEnMsk0 = phyEnMsk0 | (1 << 13);
     }


     if (1 == pAbility->Full_100)
     {
         /*100BASE-TX full duplex capable in reg 4.8*/
         phyEnMsk4 = phyEnMsk4 | (1 << 8);
         /*Speed selection [1:0] */
         /* 11=Reserved*/
         /* 10= 1000Mpbs*/
         /* 01= 100Mpbs*/
         /* 00= 10Mpbs*/
         phyEnMsk0 = phyEnMsk0 & (~(1 << 6));
         phyEnMsk0 = phyEnMsk0 | (1 << 13);
         /*Full duplex mode in reg 0.8*/
         phyEnMsk0 = phyEnMsk0 | (1 << 8);
     }

     if (1 == pAbility->AsyFC)
     {
         /*Asymetric flow control in reg 4.11*/
         phyEnMsk4 = phyEnMsk4 | (1 << 11);
     }
     if (1 == pAbility->FC)
     {
         /*Flow control in reg 4.10*/
         phyEnMsk4 = phyEnMsk4 | ((1 << 10));
     }

     /*1000 BASE-T control register setting*/
     if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_1000_BASET_CONTROL_REG, &phyData)) != RT_ERR_OK)
         return retVal;

     phyData = (phyData & (~0x0200)) | phyEnMsk9 ;

     if ((retVal = dal_rtl8371c_port_phyReg_set(unit, port, PHY_1000_BASET_CONTROL_REG, phyData)) != RT_ERR_OK)
         return retVal;

     /*Auto-Negotiation control register setting*/
     if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_AN_ADVERTISEMENT_REG, &phyData)) != RT_ERR_OK)
         return retVal;

     phyData = (phyData & (~0x0DE0)) | phyEnMsk4;
     if ((retVal = dal_rtl8371c_port_phyReg_set(unit, port, PHY_AN_ADVERTISEMENT_REG, phyData)) != RT_ERR_OK)
         return retVal;

     /*Control register setting and power off/on*/
     phyData = phyEnMsk0 & (~(1 << 12));
     phyData |= (1 << 11);   /* power down PHY, bit 11 should be set to 1 */
     if ((retVal = dal_rtl8371c_port_phyReg_set(unit, port, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
         return retVal;

     phyData = phyData & (~(1 << 11));   /* power on PHY, bit 11 should be set to 0*/
     if ((retVal = dal_rtl8371c_port_phyReg_set(unit, port, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
         return retVal;

     return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyForceModeAbility_get
 * Description:
 *      Get PHY ability through PHY registers.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pAbility - Ability structure
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      Get the capablity of specified PHY.
 */
rtksw_api_ret_t dal_rtl8371c_port_phyForceModeAbility_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_ability_t *pAbility)
{
    rtksw_api_ret_t       retVal;
    rtksw_uint32          phyData0;
    rtksw_uint32          phyData4;
    rtksw_uint32          phyData9;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if(NULL == pAbility)
        return RT_ERR_NULL_POINTER;

    /*Control register setting and restart auto*/
    if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return retVal;

    /*Auto-Negotiation control register setting*/
    if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return retVal;

    /*1000 BASE-T control register setting*/
    if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
        return retVal;

    osal_memset(pAbility, 0x00, sizeof(rtksw_port_phy_ability_t));

    if (phyData9 & (1 << 9))
        pAbility->Full_1000 = 1;
    else
        pAbility->Full_1000 = 0;

    if (phyData4 & (1 << 11))
        pAbility->AsyFC = 1;
    else
        pAbility->AsyFC = 0;

    if (phyData4 & ((1 << 10)))
        pAbility->FC = 1;
    else
        pAbility->FC = 0;

    if (phyData4 & (1 << 8))
        pAbility->Full_100 = 1;
    else
        pAbility->Full_100 = 0;

    if (phyData4 & (1 << 7))
        pAbility->Half_100 = 1;
    else
        pAbility->Half_100 = 0;

    if (phyData4 & (1 << 6))
        pAbility->Full_10 = 1;
    else
        pAbility->Full_10 = 0;

    if (phyData4 & (1 << 5))
        pAbility->Half_10 = 1;
    else
        pAbility->Half_10 = 0;


    if (phyData0 & (1 << 12))
        pAbility->AutoNegotiation = 1;
    else
        pAbility->AutoNegotiation = 0;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyStatus_get
 * Description:
 *      Get ethernet PHY linking status
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      linkStatus  - PHY link status
 *      speed       - PHY link speed
 *      duplex      - PHY duplex mode
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      API will return auto negotiation status of phy.
 */
rtksw_api_ret_t dal_rtl8371c_port_phyStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_linkStatus_t *pLinkStatus, rtksw_port_speed_t *pSpeed, rtksw_port_duplex_t *pDuplex)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if( (NULL == pLinkStatus) || (NULL == pSpeed) || (NULL == pDuplex) )
        return RT_ERR_NULL_POINTER;

    /*Get PHY resolved register*/
    if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xA434, &phyData)) != RT_ERR_OK)
        return retVal;

    /*check link status*/
    if (phyData & (1<<2))
    {
        *pLinkStatus = 1;

        if ((REG_BIT(phyData, 10) == 0) && (REG_BIT(phyData, 9) == 1) && (REG_BIT(phyData, 5) == 0) && (REG_BIT(phyData, 4) == 1))
        {
            *pSpeed = RTKSW_PORT_SPEED_2500M;
            *pDuplex = RTKSW_PORT_FULL_DUPLEX;
        }
        else if ((REG_BIT(phyData, 10) == 0) && (REG_BIT(phyData, 9) == 0) && (REG_BIT(phyData, 5) == 1) && (REG_BIT(phyData, 4) == 0))
        {
            *pSpeed = RTKSW_PORT_SPEED_1000M;
            *pDuplex = RTKSW_PORT_FULL_DUPLEX;
        }
        else if ((REG_BIT(phyData, 10) == 0) && (REG_BIT(phyData, 9) == 0) && (REG_BIT(phyData, 5) == 0) && (REG_BIT(phyData, 4) == 1) && (REG_BIT(phyData, 3) == 1))
        {
            *pSpeed = RTKSW_PORT_SPEED_100M;
            *pDuplex = RTKSW_PORT_FULL_DUPLEX;
        }
        else if ((REG_BIT(phyData, 10) == 0) && (REG_BIT(phyData, 9) == 0) && (REG_BIT(phyData, 5) == 0) && (REG_BIT(phyData, 4) == 1) && (REG_BIT(phyData, 3) == 0))
        {
            *pSpeed = RTKSW_PORT_SPEED_100M;
            *pDuplex = RTKSW_PORT_HALF_DUPLEX;
        }
        else if ((REG_BIT(phyData, 10) == 0) && (REG_BIT(phyData, 9) == 0) && (REG_BIT(phyData, 5) == 0) && (REG_BIT(phyData, 4) == 0) && (REG_BIT(phyData, 3) == 1))
        {
            *pSpeed = RTKSW_PORT_SPEED_10M;
            *pDuplex = RTKSW_PORT_FULL_DUPLEX;
        }
        else if ((REG_BIT(phyData, 10) == 0) && (REG_BIT(phyData, 9) == 0) && (REG_BIT(phyData, 5) == 0) && (REG_BIT(phyData, 4) == 0) && (REG_BIT(phyData, 3) == 0))
        {
            *pSpeed = RTKSW_PORT_SPEED_10M;
            *pDuplex = RTKSW_PORT_HALF_DUPLEX;
        }
        else
            return RT_ERR_FAILED;
    }
    else
    {
        *pLinkStatus = 0;
        *pSpeed = 0;
        *pDuplex = 0;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_macForceLink_set
 * Description:
 *      Set port force linking configuration.
 * Input:
 *      unit            - Unit ID
 *      port            - port id.
 *      pPortability    - port ability configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can set Port/MAC force mode properties.
 */
rtksw_api_ret_t dal_rtl8371c_port_macForceLink_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_mac_ability_t *pPortability)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData = 0;
    rtksw_uint32 fieldData = 0;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pPortability)
        return RT_ERR_NULL_POINTER;

    if (pPortability->forcemode >1|| pPortability->speed >= RTKSW_PORT_SPEED_END || pPortability->speed == RTKSW_PORT_SPEED_500M || pPortability->duplex > 1 ||
       pPortability->link > 1 || pPortability->nway > 1 || pPortability->txpause > 1 || pPortability->rxpause > 1)
        return RT_ERR_INPUT;

    /* Speed check */
    if ((rtksw_switch_isUsxgPort(unit, port) != RT_ERR_OK) && (pPortability->speed == RTKSW_PORT_SPEED_10G))
        return RT_ERR_INPUT;

    if ((rtksw_switch_isUtp2p5gPort(unit, port) != RT_ERR_OK) && (rtksw_switch_isHsgPort(unit, port) != RT_ERR_OK) && (pPortability->speed == RTKSW_PORT_SPEED_2500M))
        return RT_ERR_INPUT;

    /* Ability */
    if ((retVal = reg16_array_read(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, &regData)) != RT_ERR_OK)
        return retVal;

    switch (pPortability->speed)
    {
        case RTKSW_PORT_SPEED_10M:
            fieldData = RTL8371C_PORT_SPEED_10M;
            break;
        case RTKSW_PORT_SPEED_100M:
            fieldData = RTL8371C_PORT_SPEED_100M;
            break;
        case RTKSW_PORT_SPEED_1000M:
            fieldData = RTL8371C_PORT_SPEED_1000M;
            break;
        case RTKSW_PORT_SPEED_500M:
            fieldData = RTL8371C_PORT_SPEED_500M;
            break;
        case RTKSW_PORT_SPEED_2500M:
            fieldData = RTL8371C_PORT_SPEED_2500M;
            break;
        case RTKSW_PORT_SPEED_5G:
            fieldData = RTL8371C_PORT_SPEED_5G;
            break;
        case RTKSW_PORT_SPEED_10G:
            fieldData = RTL8371C_PORT_SPEED_10G;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((retVal = reg16_array_field_set(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_SPD_SELf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    fieldData = pPortability->duplex;
    if ((retVal = reg16_array_field_set(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_DUP_SELf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    fieldData = pPortability->link;
    if ((retVal = reg16_array_field_set(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_LINK_ENf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    fieldData = pPortability->rxpause;
    if ((retVal = reg16_array_field_set(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_RX_PAUSE_ENf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    fieldData = pPortability->txpause;
    if ((retVal = reg16_array_field_set(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_TX_PAUSE_ENf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    fieldData = pPortability->forcemode;
    if ((retVal = reg16_array_field_set(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_ENABLEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;
    
    fieldData = pPortability->forcemode; /* set FORCE_FC also if forcemode == 1 */
    if ((retVal = reg16_array_field_set(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_FC_ENf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_write(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_macForceLink_get
 * Description:
 *      Get port force linking configuration.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pPortability - port ability configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get Port/MAC force mode properties.
 */
rtksw_api_ret_t dal_rtl8371c_port_macForceLink_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_mac_ability_t *pPortability)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 fieldData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pPortability)
        return RT_ERR_NULL_POINTER;

    /* Ability */
    if ((retVal = reg16_array_read(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_get(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_SPD_SELf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    switch (fieldData)
    {
        case RTL8371C_PORT_SPEED_10M:
            pPortability->speed = RTKSW_PORT_SPEED_10M;
            break;
        case RTL8371C_PORT_SPEED_100M:
            pPortability->speed = RTKSW_PORT_SPEED_100M;
            break;
        case RTL8371C_PORT_SPEED_1000M:
            pPortability->speed = RTKSW_PORT_SPEED_1000M;
            break;
        case RTL8371C_PORT_SPEED_500M:
            pPortability->speed = RTKSW_PORT_SPEED_500M;
            break;
        case RTL8371C_PORT_SPEED_2500M:
            pPortability->speed = RTKSW_PORT_SPEED_2500M;
            break;
        case RTL8371C_PORT_SPEED_5G:
            pPortability->speed = RTKSW_PORT_SPEED_5G;
            break;
        case RTL8371C_PORT_SPEED_10G:
            pPortability->speed = RTKSW_PORT_SPEED_10G;
            break;
        default:
            return RT_ERR_FAILED;
    }

    if ((retVal = reg16_array_field_get(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_DUP_SELf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    pPortability->duplex = fieldData;

    if ((retVal = reg16_array_field_get(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_LINK_ENf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    pPortability->link = fieldData;

    if ((retVal = reg16_array_field_get(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_RX_PAUSE_ENf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;
    
     pPortability->rxpause = fieldData;

    if ((retVal = reg16_array_field_get(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_TX_PAUSE_ENf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    pPortability->txpause = fieldData;
    pPortability->nway = 0;

    if ((retVal = reg16_array_field_get(unit, RTL8371C_MAC_FORCE_MODE_CTRL1r, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_ENABLEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    pPortability->forcemode = (regData == 0) ? 0 : 1; 
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_port_macForceLinkExt_set
 * Description:
 *      Set external interface force linking configuration.
 * Input:
 *      unit            - Unit ID
 *      port            - external port ID
 *      mode            - external interface mode
 *      pPortability    - port ability configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can set external interface force mode properties.
 *      The external interface can be set to:
 *      - RTKSW_MODE_EXT_DISABLE,
 *      - RTKSW_MODE_EXT_RGMII,
 *      - RTKSW_MODE_EXT_MII_MAC,
 *      - RTKSW_MODE_EXT_MII_PHY,
 *      - RTKSW_MODE_EXT_TMII_MAC,
 *      - RTKSW_MODE_EXT_TMII_PHY,
 *      - RTKSW_MODE_EXT_GMII,
 *      - RTKSW_MODE_EXT_RMII_MAC,
 *      - RTKSW_MODE_EXT_RMII_PHY,
 *      - RTKSW_MODE_EXT_SGMII,
 *      - RTKSW_MODE_EXT_HSGMII,
 *      - RTKSW_MODE_EXT_USXGMII
 */
rtksw_api_ret_t dal_rtl8371c_port_macForceLinkExt_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_mode_ext_t mode, rtksw_port_mac_ability_t *pPortability)
{
    rtksw_api_ret_t retVal;
    rtksw_port_mac_ability_t ability;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    RTKSW_CHK_PORT_IS_EXT(unit, port);

    if (NULL == pPortability)
        return RT_ERR_NULL_POINTER;

    if ((mode != RTKSW_MODE_EXT_DISABLE) && (mode != RTKSW_MODE_EXT_SGMII) && (mode != RTKSW_MODE_EXT_HSGMII) && (mode != RTKSW_MODE_EXT_1000X_100FX) && 
        (mode != RTKSW_MODE_EXT_1000X) && (mode != RTKSW_MODE_EXT_100FX) && (mode != RTKSW_MODE_EXT_FIBER_2P5G) && (mode != RTKSW_MODE_EXT_USXGMII) &&
        (mode != RTKSW_MODE_EXT_10GR) && (mode != RTKSW_MODE_EXT_5GR))
        return RT_ERR_INPUT;

    if(mode == RTKSW_MODE_EXT_USXGMII)
    {
        if (pPortability->forcemode > 1 || pPortability->speed != RTKSW_PORT_SPEED_10G || pPortability->duplex != RTKSW_PORT_FULL_DUPLEX ||
           pPortability->link >= RTKSW_PORT_LINKSTATUS_END || pPortability->nway > 1 || pPortability->txpause > 1 || pPortability->rxpause > 1)
            return RT_ERR_INPUT;

        if(rtksw_switch_isHsgPort(unit, port) != RT_ERR_OK)
            return RT_ERR_PORT_ID;
    }
    else if(mode == RTKSW_MODE_EXT_HSGMII)
    {
        if (pPortability->forcemode > 1 || pPortability->speed != RTKSW_PORT_SPEED_2500M || pPortability->duplex != RTKSW_PORT_FULL_DUPLEX ||
           pPortability->link >= RTKSW_PORT_LINKSTATUS_END || pPortability->nway > 1 || pPortability->txpause > 1 || pPortability->rxpause > 1)
            return RT_ERR_INPUT;

        if(rtksw_switch_isHsgPort(unit, port) != RT_ERR_OK)
            return RT_ERR_PORT_ID;
    }
    else if (mode != RTKSW_MODE_EXT_DISABLE)
    {
        if (pPortability->forcemode > 1 || pPortability->speed >= RTKSW_PORT_SPEED_END || pPortability->duplex >= RTKSW_PORT_DUPLEX_END ||
           pPortability->link >= RTKSW_PORT_LINKSTATUS_END || pPortability->nway > 1 || pPortability->txpause > 1 || pPortability->rxpause > 1)
            return RT_ERR_INPUT;
    }

    /* Configure EXT port mode */
    if ((retVal = _dal_rtl8371c_port_extMode_set(unit, port, mode)) != RT_ERR_OK)
        return retVal;

    /* Configure Ability */
    osal_memset(&ability, 0x00, sizeof(rtksw_port_mac_ability_t));
    if ((retVal = dal_rtl8371c_port_macForceLink_get(unit, port, &ability)) != RT_ERR_OK)
        return retVal;

    ability.forcemode = pPortability->forcemode;
    ability.duplex    = pPortability->duplex;
    ability.link      = pPortability->link;
    ability.nway      = pPortability->nway;
    ability.txpause   = pPortability->txpause;
    ability.rxpause   = pPortability->rxpause;
    ability.speed     = pPortability->speed;

    if ((retVal = dal_rtl8371c_port_macForceLink_set(unit, port, &ability)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_macForceLinkExt_get
 * Description:
 *      Set external interface force linking configuration.
 * Input:
 *      unit            - Unit ID
 *      port            - external port ID
 * Output:
 *      pMode           - external interface mode
 *      pPortability    - port ability configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get external interface force mode properties.
 */
rtksw_api_ret_t dal_rtl8371c_port_macForceLinkExt_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_mode_ext_t *pMode, rtksw_port_mac_ability_t *pPortability)
{
    rtksw_api_ret_t retVal;
    rtksw_port_mac_ability_t ability;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    RTKSW_CHK_PORT_IS_EXT(unit, port);

    if(NULL == pMode)
        return RT_ERR_NULL_POINTER;

    if(NULL == pPortability)
        return RT_ERR_NULL_POINTER;

    if ((retVal = _dal_rtl8371c_port_extMode_get(unit, port, pMode)) != RT_ERR_OK)
        return retVal;

    osal_memset(&ability, 0x00, sizeof(rtksw_port_mac_ability_t));
    if ((retVal = dal_rtl8371c_port_macForceLink_get(unit, port, &ability)) != RT_ERR_OK)
        return retVal;

    pPortability->forcemode = ability.forcemode;
    pPortability->duplex    = ability.duplex;
    pPortability->link      = ability.link;
    pPortability->nway      = ability.nway;
    pPortability->txpause   = ability.txpause;
    pPortability->rxpause   = ability.rxpause;
    pPortability->speed     = ability.speed;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_macStatus_get
 * Description:
 *      Get port link status.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pPortstatus - port ability configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can get Port/PHY properties.
 */
rtksw_api_ret_t dal_rtl8371c_port_macStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_mac_ability_t *pPortstatus)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 fieldData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pPortstatus)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_read(unit, RTL8371C_MAC_STATUSr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_get(unit, RTL8371C_MAC_STATUSr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_LINK_ABLTYf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    osal_memset(pPortstatus, 0x00, sizeof(rtksw_port_mac_ability_t));
    pPortstatus->link = fieldData;

    if (pPortstatus->link == 1)
    {
        if ((retVal = reg16_array_field_get(unit, RTL8371C_MAC_STATUSr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_SPEED_ABLTYf, &fieldData, &regData)) != RT_ERR_OK)
            return retVal;

        switch (fieldData)
        {
            case RTL8371C_PORT_SPEED_10M:
                pPortstatus->speed = RTKSW_PORT_SPEED_10M;
                break;
            case RTL8371C_PORT_SPEED_100M:
                pPortstatus->speed = RTKSW_PORT_SPEED_100M;
                break;
            case RTL8371C_PORT_SPEED_1000M:
                pPortstatus->speed = RTKSW_PORT_SPEED_1000M;
                break;
            case RTL8371C_PORT_SPEED_500M:
                pPortstatus->speed = RTKSW_PORT_SPEED_500M;
                break;
            case RTL8371C_PORT_SPEED_2500M:
                pPortstatus->speed = RTKSW_PORT_SPEED_2500M;
                break;
            case RTL8371C_PORT_SPEED_5G:
                pPortstatus->speed = RTKSW_PORT_SPEED_5G;
                break;
            case RTL8371C_PORT_SPEED_10G:
                pPortstatus->speed = RTKSW_PORT_SPEED_10G;
                break;
            default:
                return RT_ERR_FAILED;
        }

        if ((retVal = reg16_array_field_get(unit, RTL8371C_MAC_STATUSr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_DUPLEX_ABLTYf, &fieldData, &regData)) != RT_ERR_OK)
            return retVal;

        pPortstatus->duplex = fieldData;

        if ((retVal = reg16_array_field_get(unit, RTL8371C_MAC_STATUSr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_NWAY_ABLTYf, &fieldData, &regData)) != RT_ERR_OK)
            return retVal;

        pPortstatus->nway = fieldData;

        if ((retVal = reg16_array_field_get(unit, RTL8371C_MAC_STATUSr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_TXPAUSE_ABLTYf, &fieldData, &regData)) != RT_ERR_OK)
            return retVal;

        pPortstatus->txpause = fieldData;

        if ((retVal = reg16_array_field_get(unit, RTL8371C_MAC_STATUSr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_RXPAUSE_ABLTYf, &fieldData, &regData)) != RT_ERR_OK)
            return retVal;

        pPortstatus->rxpause = fieldData;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_macLocalLoopbackEnable_set
 * Description:
 *      Set Port Local Loopback. (Redirect TX to RX.)
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      enable  - Loopback state, 0:disable, 1:enable
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can enable/disable Local loopback in MAC.
 *      For UTP port, This API will also enable the digital
 *      loopback bit in PHY register for sync of speed between
 *      PHY and MAC. For EXT port, users need to force the
 *      link state by themself.
 */
rtksw_api_ret_t dal_rtl8371c_port_macLocalLoopbackEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t   retVal;
    rtksw_uint32      regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    regData = (enable == RTKSW_ENABLED) ? 1 : 0;
    if ((retVal = reg16_array_field_write(unit, RTL8371C_MAC_TX_MISCr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_MAC_LOOPBACKf, &regData)) != RT_ERR_OK)
            return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_macLocalLoopbackEnable_get
 * Description:
 *      Get Port Local Loopback. (Redirect TX to RX.)
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pEnable  - Loopback state, 0:disable, 1:enable
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_port_macLocalLoopbackEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_MAC_TX_MISCr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_MAC_LOOPBACKf, &regData)) != RT_ERR_OK)
            return retVal;

    *pEnable = (regData == 1) ? RTKSW_ENABLED : RTKSW_DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyReg_set
 * Description:
 *      Set PHY register data of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id.
 *      reg     - Register id
 *      value   - Register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      This API can set PHY register data of the specific port.
 */
rtksw_api_ret_t dal_rtl8371c_port_phyReg_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_reg_t reg, rtksw_port_phy_data_t value)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if (reg > RTL8371C_PHY_REGNOMAX)
        return RT_ERR_PHY_REG_ID;

    if (value > 0xFFFF)
        return RT_ERR_INPUT;

    if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xA400 + ((rtksw_uint32)reg * 2), (rtksw_uint32)value)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyReg_get
 * Description:
 *      Get PHY register data of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      reg     - Register id
 * Output:
 *      pData   - Register data
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_PHY_REG_ID       - Invalid PHY address
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      This API can get PHY register data of the specific port.
 */
rtksw_api_ret_t dal_rtl8371c_port_phyReg_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_reg_t reg, rtksw_port_phy_data_t *pData)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if (reg > RTL8371C_PHY_REGNOMAX)
        return RT_ERR_PHY_REG_ID;

    if (pData == NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xA400 + ((rtksw_uint32)reg * 2), (rtksw_uint32 *)pData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyOCPReg_set
 * Description:
 *      Set PHY OCP register
 * Input:
 *      unit        - Unit ID
 *      port        - PHY ID
 *      ocpAddr     - OCP register address
 *      ocpData     - OCP Data.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_BUSYWAIT_TIMEOUT                 - Timeout
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_port_phyOCPReg_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 ocpAddr, rtksw_uint32 ocpData)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 phyPort;
    rtksw_uint32 ocpAddr9_6;
    rtksw_uint32 ocpAddr5_1;
    rtksw_uint32 busyCnt = RTL8371C_PHY_ACCESS_TIMEOUT;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if (ocpAddr > 0xFFFF)
        return RT_ERR_INPUT;

    if (ocpData > 0xFFFF)
        return RT_ERR_INPUT;

    /* MSB */
    regData = (ocpAddr >> 10) & 0x003F;
    if ((retVal = reg16_field_write(unit, RTL8371C_GPHY_OCP_MSB_0r, RTL8371C_CFG_CPU_OCPADR_MSBf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    /* LSB & PHY ID */
    phyPort = rtksw_switch_port_L2P_get(unit, port);
    ocpAddr9_6 = (ocpAddr >> 6) & 0x000F;
    ocpAddr5_1 = (ocpAddr >> 1) & 0x001F;
    regData = 0x2000 | (ocpAddr9_6 << 8) | (phyPort << 5) | ocpAddr5_1;
    if ((retVal = reg16_field_write(unit, RTL8371C_IND_ACC_ADDRr, RTL8371C_ADDRESSf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    /* Data */
    regData = ocpData;
    if ((retVal = reg16_field_write(unit, RTL8371C_IND_ACC_WDATAr, RTL8371C_WR_DATAf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    /* R/W op */
    regData = RTL8371C_PHY_WRITE_OPERATION;
    if ((retVal = reg16_field_write(unit, RTL8371C_INDACC_CTRLr, RTL8371C_RWf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    /* Command */
    regData = RTL8371C_PHY_ACCESS_CMD;
    if ((retVal = reg16_field_write(unit, RTL8371C_INDACC_CTRLr, RTL8371C_CMDf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    /* Polling until access command finish */
    while(busyCnt)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_INDACC_STATUSr, RTL8371C_PHY_BUSYf, &regData)) != RT_ERR_OK)
            return retVal;

        if (regData == 0)
            break;

        busyCnt--;
        if(busyCnt == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyOCPReg_get
 * Description:
 *      Set PHY OCP register
 * Input:
 *      unit        - Unit ID
 *      phyNo       - PHY ID
 *      ocpAddr     - OCP register address
 * Output:
 *      pRegData    - OCP data.
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Null pointer
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_port_phyOCPReg_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 ocpAddr, rtksw_uint32 *pRegData )
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 phyPort;
    rtksw_uint32 ocpAddr9_6;
    rtksw_uint32 ocpAddr5_1;
    rtksw_uint32 busyCnt = RTL8371C_PHY_ACCESS_TIMEOUT;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if (ocpAddr > 0xFFFF)
        return RT_ERR_INPUT;

    if (pRegData == NULL)
        return RT_ERR_NULL_POINTER;

    /* MSB */
    regData = (ocpAddr >> 10) & 0x003F;
    if ((retVal = reg16_field_write(unit, RTL8371C_GPHY_OCP_MSB_0r, RTL8371C_CFG_CPU_OCPADR_MSBf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    /* LSB & PHY ID */
    phyPort = rtksw_switch_port_L2P_get(unit, port);
    ocpAddr9_6 = (ocpAddr >> 6) & 0x000F;
    ocpAddr5_1 = (ocpAddr >> 1) & 0x001F;
    regData = 0x2000 | (ocpAddr9_6 << 8) | (phyPort << 5) | ocpAddr5_1;
    if ((retVal = reg16_field_write(unit, RTL8371C_IND_ACC_ADDRr, RTL8371C_ADDRESSf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    /* R/W op */
    regData = RTL8371C_PHY_READ_OPERATION;
    if ((retVal = reg16_field_write(unit, RTL8371C_INDACC_CTRLr, RTL8371C_RWf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    /* Command */
    regData = RTL8371C_PHY_ACCESS_CMD;
    if ((retVal = reg16_field_write(unit, RTL8371C_INDACC_CTRLr, RTL8371C_CMDf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    /* Polling until access command finish */
    while(busyCnt)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_INDACC_STATUSr, RTL8371C_PHY_BUSYf, &regData)) != RT_ERR_OK)
            return retVal;

        if (regData == 0)
            break;

        busyCnt--;
        if(busyCnt == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    /* Data */
    if ((retVal = reg16_field_read(unit, RTL8371C_IND_ACC_RDATAr, RTL8371C_RD_DATAf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    *pRegData = regData;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_backpressureEnable_set
 * Description:
 *      Set the half duplex backpressure enable status of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id.
 *      enable  - Back pressure status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      This API can set the half duplex backpressure enable status of the specific port.
 *      The half duplex backpressure enable status of the port is as following:
 *      - DISABLE(Defer)
 *      - ENABLE (Backpressure)
 */
rtksw_api_ret_t dal_rtl8371c_port_backpressureEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (port != RTKSW_WHOLE_SYSTEM)
        return RT_ERR_PORT_ID;

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    regData = (enable == RTKSW_ENABLED) ? 0 : 1;
    if ((retVal = reg16_field_write(unit, RTL8371C_RG07r, RTL8371C_LONGTXEf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_backpressureEnable_get
 * Description:
 *      Get the half duplex backpressure enable status of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pEnable - Back pressure status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can get the half duplex backpressure enable status of the specific port.
 *      The half duplex backpressure enable status of the port is as following:
 *      - DISABLE(Defer)
 *      - ENABLE (Backpressure)
 */
rtksw_api_ret_t dal_rtl8371c_port_backpressureEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (port != RTKSW_WHOLE_SYSTEM)
        return RT_ERR_PORT_ID;

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_RG07r, RTL8371C_LONGTXEf, &regData)) != RT_ERR_OK)
        return retVal;

    *pEnable = (regData == 1) ? RTKSW_DISABLED : RTKSW_ENABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_adminEnable_set
 * Description:
 *      Set port admin configuration of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id.
 *      enable  - Back pressure status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      This API can set port admin configuration of the specific port.
 *      The port admin configuration of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtksw_api_ret_t dal_rtl8371c_port_adminEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32      data;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
        return retVal;

    if (RTKSW_ENABLED == enable)
    {
        data &= 0xF7FF;
        data |= 0x0200;
    }
    else if (RTKSW_DISABLED == enable)
    {
        data |= 0x0800;
    }

    if ((retVal = dal_rtl8371c_port_phyReg_set(unit, port, PHY_CONTROL_REG, data)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_adminEnable_get
 * Description:
 *      Get port admin configurationof the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pEnable - Back pressure status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can get port admin configuration of the specific port.
 *      The port admin configuration of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtksw_api_ret_t dal_rtl8371c_port_adminEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32      data;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
        return retVal;

    if ( (data & 0x0800) == 0x0800)
    {
        *pEnable = RTKSW_DISABLED;
    }
    else
    {
        *pEnable = RTKSW_ENABLED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_isolation_set
 * Description:
 *      Set permitted port isolation portmask
 * Input:
 *      unit        - Unit ID
 *      port        - port id.
 *      pPortmask   - Permit port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_PORT_MASK    - Invalid portmask.
 * Note:
 *      This API set the port mask that a port can trasmit packet to of each port
 *      A port can only transmit packet to ports included in permitted portmask
 */
rtksw_api_ret_t dal_rtl8371c_port_isolation_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_portmask_t *pPortmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    /* check port mask */
    RTKSW_CHK_PORTMASK_VALID(unit, pPortmask);

    if ((retVal = rtksw_switch_portmask_L2P_get(unit, pPortmask, &pmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PORT_ISO_PORT_PMSKr, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_PORT_MASKf, &pmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_isolation_get
 * Description:
 *      Get permitted port isolation portmask
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pPortmask - Permit port mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API get the port mask that a port can trasmit packet to of each port
 *      A port can only transmit packet to ports included in permitted portmask
 */
rtksw_api_ret_t dal_rtl8371c_port_isolation_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_portmask_t *pPortmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PORT_ISO_PORT_PMSKr, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_PORT_MASKf, &pmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtksw_switch_portmask_P2L_get(unit, pmask, pPortmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyEnableAll_set
 * Description:
 *      Set all PHY enable status.
 * Input:
 *      unit    - Unit ID
 *      enable  - PHY Enable State.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      This API can set all PHY status.
 *      The configuration of all PHY is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtksw_api_ret_t dal_rtl8371c_port_phyEnableAll_set(rtksw_uint32 unit, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 data;
    rtksw_uint32 port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    data = 0x10;
    if ((retVal = reg16_field_write(unit, RTL8371C_CFG_PHY_INIr, RTL8371C_PHY_INI_POWER_DOWNf, &data)) != RT_ERR_OK)
        return retVal;

    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        if(rtksw_switch_isUtpPort(unit, port) == RT_ERR_OK)
        {
            if(rtksw_switch_isUtp2p5gPort(unit, port) == RT_ERR_OK)
            {
                
                if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xa610, &data)) != RT_ERR_OK)
                    return retVal;

                if (RTKSW_ENABLED == enable)
                    data &= 0xF7FF;
                else
                    data |= 0x0800;

                if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa610, data)) != RT_ERR_OK)
                    return retVal;
            }
            else
            {
                if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
                    return retVal;

                if (RTKSW_ENABLED == enable)
                {
                    data &= 0xF7FF;
                    data |= 0x0200;
                }
                else
                {
                    data |= 0x0800;
                }

                if ((retVal = dal_rtl8371c_port_phyReg_set(unit, port, PHY_CONTROL_REG, data)) != RT_ERR_OK)
                    return retVal;
            }
        }
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rtl8371c_port_phyEnableAll_get
 * Description:
 *      Get all PHY enable status.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pEnable - PHY Enable State.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      This API can set all PHY status.
 *      The configuration of all PHY is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtksw_api_ret_t dal_rtl8371c_port_phyEnableAll_get(rtksw_uint32 unit, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 data;
    rtksw_uint32 port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        if(rtksw_switch_isUtpPort(unit, port) == RT_ERR_OK)
        {
            if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_CONTROL_REG, &data)) != RT_ERR_OK)
                return retVal;

            if (data & 0x0800)
            {
                *pEnable = RTKSW_DISABLED;
                return RT_ERR_OK;
            }
        }
    }

    *pEnable = RTKSW_ENABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyComboPortMedia_set
 * Description:
 *      Set Combo port media type
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      media   - Media (COPPER or FIBER or AUTO)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_PORT_ID          - Invalid port ID.
 * Note:
 *      The API can Set Combo port media type.
 */
rtksw_api_ret_t dal_rtl8371c_port_phyComboPortMedia_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_media_t media)
{
    rtksw_api_ret_t retVal;
    rtksw_port_media_t currentMedia;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    /* Check Combo Port ID */
    RTKSW_CHK_PORT_IS_COMBO(unit, port);

    if ((media != RTKSW_PORT_MEDIA_COPPER) && (media != RTKSW_PORT_MEDIA_FIBER))
        return RT_ERR_INPUT;

    if ((retVal = dal_rtl8371c_port_phyComboPortMedia_get(unit, port, &currentMedia)) != RT_ERR_OK)
        return retVal;

    if (media == currentMedia)
        return RT_ERR_OK;

    /* if SDS0 used by EXT_PORT0, can't configure combo = Fiber*/
    if (media == RTKSW_PORT_MEDIA_FIBER)
    {
        
    }

    //TBD
    return RT_ERR_DRIVER_NOT_FOUND;
}

/* Function Name:
 *      dal_rtl8371c_port_phyComboPortMedia_get
 * Description:
 *      Get Combo port media type
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pMedia  - Media (COPPER or FIBER or AUTO)
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_PORT_ID          - Invalid port ID.
 * Note:
 *      The API can Set Combo port media type.
 */
rtksw_api_ret_t dal_rtl8371c_port_phyComboPortMedia_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_media_t *pMedia)
{
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    /* Check Combo Port ID */
    RTKSW_CHK_PORT_IS_COMBO(unit, port);

    if (pMedia == NULL)
        return RT_ERR_NULL_POINTER;

    //TBD
    return RT_ERR_DRIVER_NOT_FOUND;
}

/* Function Name:
 *      dal_rtl8371c_port_rtctEnable_set
 * Description:
 *      Enable RTCT test
 * Input:
 *      unit        - Unit ID
 *      pPortmask   - Port mask of RTCT enabled port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_MASK        - Invalid port mask.
 * Note:
 *      The API can enable RTCT Test
 */
rtksw_api_ret_t dal_rtl8371c_port_rtctEnable_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask)
{
    rtksw_port_t port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Mask Valid */
    RTKSW_CHK_PORTMASK_VALID_ONLY_UTP(unit, pPortmask);

    RTKSW_PORTMASK_SCAN((*pPortmask), port)
    {

    }

    //TBD
    return RT_ERR_DRIVER_NOT_FOUND;
}

/* Function Name:
 *      dal_rtl8371c_port_rtctDisable_set
 * Description:
 *      Disable RTCT test
 * Input:
 *      unit        - Unit ID
 *      pPortmask   - Port mask of RTCT disabled port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_MASK        - Invalid port mask.
 * Note:
 *      The API can disable RTCT Test
 */
rtksw_api_ret_t dal_rtl8371c_port_rtctDisable_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask)
{
    rtksw_port_t port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Mask Valid */
    RTKSW_CHK_PORTMASK_VALID_ONLY_UTP(unit, pPortmask);

    RTKSW_PORTMASK_SCAN((*pPortmask), port)
    {

    }

    //TBD
    return RT_ERR_DRIVER_NOT_FOUND;
}

/* Function Name:
 *      dal_rtl8371c_port_rtctResult_get
 * Description:
 *      Get the result of RTCT test
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 * Output:
 *      pRtctResult - The result of RTCT result
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 *      RT_ERR_PHY_RTCT_NOT_FINISH  - Testing does not finish.
 * Note:
 *      The API can get RTCT test result.
 *      RTCT test may takes 4.8 seconds to finish its test at most.
 *      Thus, if this API return RT_ERR_PHY_RTCT_NOT_FINISH or
 *      other error code, the result can not be referenced and
 *      user should call this API again until this API returns
 *      a RT_ERR_OK.
 *      The result is stored at pRtctResult->ge_result
 *      pRtctResult->linkType is unused.
 *      The unit of channel length is 2.5cm. Ex. 300 means 300 * 2.5 = 750cm = 7.5M
 */
rtksw_api_ret_t dal_rtl8371c_port_rtctResult_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rtctResult_t *pRtctResult)
{
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if (pRtctResult == NULL)
        return RT_ERR_NULL_POINTER;

    //TBD
    return RT_ERR_DRIVER_NOT_FOUND;
}

/* Function Name:
 *      dal_rtl8371c_port_sgmiiLinkStatus_get
 * Description:
 *      Get SGMII status
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 * Output:
 *      pSignalDetect   - Signal detect
 *      pSync           - Sync
 *      pLink           - Link
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can reset Serdes
 */
rtksw_api_ret_t dal_rtl8371c_port_sgmiiLinkStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_data_t *pSignalDetect, rtksw_data_t *pSync, rtksw_port_linkStatus_t *pLink)
{
    rtksw_api_ret_t retVal;
    rtksw_mode_ext_t mode;
    rtksw_uint32 sdsID;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check EXT port */
    RTKSW_CHK_PORT_IS_EXT(unit, port);

    if ((pSignalDetect == NULL) || (pSync == NULL) || (pLink == NULL))
        return RT_ERR_NULL_POINTER;

    if ((retVal = _dal_rtl8371c_port_extMode_get(unit, port, &mode)) != RT_ERR_OK)
        return retVal;

    sdsID = port - 16;
    if ((mode == RTKSW_MODE_EXT_SGMII) || (mode == RTKSW_MODE_EXT_HSGMII) || (mode == RTKSW_MODE_EXT_1000X_100FX) || (mode == RTKSW_MODE_EXT_1000X) || (mode == RTKSW_MODE_EXT_100FX) || (mode == RTKSW_MODE_EXT_FIBER_2P5G))
    {
        if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 1, 29, &regData)) != RT_ERR_OK)
            return retVal;

        *pSignalDetect = (regData >> 8) & 0x1;
        *pSync = regData & 0x1;
        *pLink = (regData >> 4) & 0x1;
    }
    else if ((mode == RTKSW_MODE_EXT_USXGMII) || (mode == RTKSW_MODE_EXT_10GR))
    {
        if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 5, 0, &regData)) != RT_ERR_OK)
            return retVal;

        *pSignalDetect = 0x0;
        *pSync = 0x0;
        *pLink = (regData >> 12) & 0x1;
    }
    else
    {
        *pSignalDetect = 0x0;
        *pSync = 0x0;
        *pLink = 0x0;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_sgmiiNway_set
 * Description:
 *      Configure SGMII/HSGMII port Nway state
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 *      state       - Nway state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API configure SGMII/HSGMII port Nway state
 */
rtksw_api_ret_t dal_rtl8371c_port_sgmiiNway_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t state)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 sdsID;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check EXT port */
    RTKSW_CHK_PORT_IS_EXT(unit, port);

    if (state >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    if (port == EXT_PORT0)
        sdsID = 0;
    else if (port == EXT_PORT1)
        sdsID = 1;
    else
        return RT_ERR_PORT_ID;

    if (state == RTKSW_ENABLED)
    {
        /*2500BASE-X/1000BASE-X/100BASE-FX: Page 2, reg 0 bit[12]=1  */
        if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 2, 0, &regData)) != RT_ERR_OK)
            return retVal;
        
        regData |= 0x1000;
        if ((retVal = dal_rtl8371c_port_serdesReg_set(unit, sdsID, 2, 0, regData)) != RT_ERR_OK)
            return retVal;

        /* HISGMII/SGMII: Page 0, Reg 1 bit[11:8]= 4b'1111 (Nway enable) */
        if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 0, 1, &regData)) != RT_ERR_OK)
            return retVal;
        
        regData |= 0x0F00;
        if ((retVal = dal_rtl8371c_port_serdesReg_set(unit, sdsID, 0, 1, regData)) != RT_ERR_OK)
            return retVal;

        if (sdsID == 1)
        {
            /* TGR/5000BASE-R Nway: Page 31, reg 11 bit[4]=1 (Nway enable) */
            if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 31, 11, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData |= 0x0010;
            if ((retVal = dal_rtl8371c_port_serdesReg_set(unit, sdsID, 31, 11, regData)) != RT_ERR_OK)
                return retVal;
    
            /* USXGMII: Page 7,Reg 17 bit[3:0]=4b'1111 (Nway enable) */
            if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 7, 17, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData |= 0x000F;
            if ((retVal = dal_rtl8371c_port_serdesReg_set(unit, sdsID, 7, 17, regData)) != RT_ERR_OK)
                return retVal;
        }
    }
    else
    {
        /*2500BASE-X/1000BASE-X/100BASE-FX: Page 2, reg 0 bit[12]=1  */
        if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 2, 0, &regData)) != RT_ERR_OK)
            return retVal;
        
        regData &= ~0x1000;
        if ((retVal = dal_rtl8371c_port_serdesReg_set(unit, sdsID, 2, 0, regData)) != RT_ERR_OK)
            return retVal;

        /* HISGMII/SGMII: Page 0, Reg 1 bit[11:8]= 4b'1111 (Nway enable) */
        if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 0, 1, &regData)) != RT_ERR_OK)
            return retVal;
        
        regData &= ~0x0F00;
        if ((retVal = dal_rtl8371c_port_serdesReg_set(unit, sdsID, 0, 1, regData)) != RT_ERR_OK)
            return retVal;

        if (sdsID == 1)
        {
            /* TGR/5000BASE-R Nway: Page 31, reg 11 bit[4]=1 (Nway enable) */
            if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 31, 11, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData &= ~0x0010;
            if ((retVal = dal_rtl8371c_port_serdesReg_set(unit, sdsID, 31, 11, regData)) != RT_ERR_OK)
                return retVal;
    
            /* USXGMII: Page 7,Reg 17 bit[3:0]=4b'1111 (Nway enable) */
            if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 7, 17, &regData)) != RT_ERR_OK)
                return retVal;
            
            regData &= ~0x000F;
            if ((retVal = dal_rtl8371c_port_serdesReg_set(unit, sdsID, 7, 17, regData)) != RT_ERR_OK)
                return retVal;
        }
    }
    
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_sgmiiNway_get
 * Description:
 *      Get SGMII/HSGMII port Nway state
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 * Output:
 *      pState      - Nway state
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can get SGMII/HSGMII port Nway state
 */
rtksw_api_ret_t dal_rtl8371c_port_sgmiiNway_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pState)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 sdsID;
    rtksw_uint32 cnt = 0;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check EXT port */
    RTKSW_CHK_PORT_IS_EXT(unit, port);

    if (port == EXT_PORT0)
        sdsID = 0;
    else if (port == EXT_PORT1)
        sdsID = 1;
    else
        return RT_ERR_PORT_ID;

    if (pState == NULL)
        return RT_ERR_NULL_POINTER;

    /*2500BASE-X/1000BASE-X/100BASE-FX: Page 2, reg 0 bit[12]=1  */
    if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 2, 0, &regData)) != RT_ERR_OK)
        return retVal;
    
    if ((regData & 0x1000) != 0)
        cnt++;

    /* HISGMII/SGMII: Page 0, Reg 1 bit[11:8]= 4b'1111 (Nway enable) */
    if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 0, 1, &regData)) != RT_ERR_OK)
        return retVal;
    
    if ((regData & 0x0F00) != 0)
        cnt++;

    if (sdsID == 0)
    {
        if (cnt == 0)
            *pState = RTKSW_DISABLED;
        else if (cnt == 2)
            *pState = RTKSW_ENABLED;
        else
            return RT_ERR_NOT_INIT;
    }
    else /* sdsID == 1 */
    {
        /* TGR/5000BASE-R Nway: Page 31, reg 11 bit[4]=1 (Nway enable) */
        if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 31, 11, &regData)) != RT_ERR_OK)
            return retVal;
        
        if ((regData & 0x0010) != 0)
            cnt++;

         /* USXGMII: Page 7,Reg 17 bit[3:0]=4b'1111 (Nway enable) */
        if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 7, 17, &regData)) != RT_ERR_OK)
            return retVal;
        
        if ((regData & 0x000F) != 0)
            cnt++;
        
        if (cnt == 0)
            *pState = RTKSW_DISABLED;
        else if (cnt == 4)
            *pState = RTKSW_ENABLED;
        else
            return RT_ERR_NOT_INIT;
    }

    return RT_ERR_OK;
}

/* Function Name:`
 *      dal_rtl8371c_port_autoDos_set
 * Description:
 *      Set Auto Dos state
 * Input:
 *      unit        - Unit ID
 *      type        - Auto DoS type
 *      state       - 1: Eanble(Drop), 0: Disable(Forward)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 * Note:
 *      The API can set Auto Dos state
 */
rtksw_api_ret_t dal_rtl8371c_port_autoDos_set(rtksw_uint32 unit, rtksw_port_autoDosType_t type, rtksw_enable_t state)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 field;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= AUTODOS_END)
        return RT_ERR_INPUT;

    if (state >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    switch (type)
    {
        case AUTODOS_DAEQSA:
            field = RTL8371C_DROP_DAEQSAf;
            break;
        case AUTODOS_LANDATTACKS:
            field = RTL8371C_DROP_LANDATTACKSf;
            break;
        case AUTODOS_BLATATTACKS:
            field = RTL8371C_DROP_BLATATTACKSf;
            break;
        case AUTODOS_SYNFINSCAN:
            field = RTL8371C_DROP_SYNFINSCANf;
            break;
        case AUTODOS_XMASCAN:
            field = RTL8371C_DROP_XMASCANf;
            break;
        case AUTODOS_NULLSCAN:
            field = RTL8371C_DROP_NULLSCANf;
            break;
        case AUTODOS_SYN1024:
            field = RTL8371C_DROP_SYN1024f;
            break;
        case AUTODOS_TCPSHORTHDR:
            field = RTL8371C_DROP_TCPSHORTHDRf;
            break;
        case AUTODOS_TCPFRAGERROR:
            field = RTL8371C_DROP_TCPFRAGERRORf;
            break;
        case AUTODOS_ICMPFRAGMENT:
            field = RTL8371C_DROP_ICMPFRAGMENTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    regData = (state == RTKSW_ENABLED) ? 1 : 0;
    if ((retVal = reg16_field_write(unit, RTL8371C_DOS_CFGr, field, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_autoDos_get
 * Description:
 *      Get Auto Dos state
 * Input:
 *      unit        - Unit ID
 *      type        - Auto DoS type
 * Output:
 *      pState      - 1: Eanble(Drop), 0: Disable(Forward)
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_NULL_POINTER         - Null Pointer
 * Note:
 *      The API can get Auto Dos state
 */
rtksw_api_ret_t dal_rtl8371c_port_autoDos_get(rtksw_uint32 unit, rtksw_port_autoDosType_t type, rtksw_enable_t *pState)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 field;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= AUTODOS_END)
        return RT_ERR_INPUT;

    if (pState == NULL)
        return RT_ERR_NULL_POINTER;

    switch (type)
    {
        case AUTODOS_DAEQSA:
            field = RTL8371C_DROP_DAEQSAf;
            break;
        case AUTODOS_LANDATTACKS:
            field = RTL8371C_DROP_LANDATTACKSf;
            break;
        case AUTODOS_BLATATTACKS:
            field = RTL8371C_DROP_BLATATTACKSf;
            break;
        case AUTODOS_SYNFINSCAN:
            field = RTL8371C_DROP_SYNFINSCANf;
            break;
        case AUTODOS_XMASCAN:
            field = RTL8371C_DROP_XMASCANf;
            break;
        case AUTODOS_NULLSCAN:
            field = RTL8371C_DROP_NULLSCANf;
            break;
        case AUTODOS_SYN1024:
            field = RTL8371C_DROP_SYN1024f;
            break;
        case AUTODOS_TCPSHORTHDR:
            field = RTL8371C_DROP_TCPSHORTHDRf;
            break;
        case AUTODOS_TCPFRAGERROR:
            field = RTL8371C_DROP_TCPFRAGERRORf;
            break;
        case AUTODOS_ICMPFRAGMENT:
            field = RTL8371C_DROP_ICMPFRAGMENTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((retVal = reg16_field_read(unit, RTL8371C_DOS_CFGr, field, &regData)) != RT_ERR_OK)
        return retVal;

    *pState = (regData == 1) ? RTKSW_ENABLED : RTKSW_DISABLED;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_fiberAbility_set
 * Description:
 *      Configure fiber port ability
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 *      pAbility    - Fiber port ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can configure fiber port ability
 */
rtksw_api_ret_t dal_rtl8371c_port_fiberAbility_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_fiber_ability_t *pAbility)
{
    rtksw_api_ret_t retVal;
    rtksw_port_mac_ability_t ability;
    rtksw_mode_ext_t mode;
    rtksw_uint32 regData;
    rtksw_uint32 sdsID;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check port is EXT */
    RTKSW_CHK_PORT_IS_EXT(unit, port);

    if (pAbility == NULL)
        return RT_ERR_NULL_POINTER;

    /* Only 100M & 1000M can set at the same time */
    if ((pAbility->Full_100 == 1) && (pAbility->Full_1000 == 1))
    {
        if ((pAbility->Full_2P5G == 1) || (pAbility->Full_5G == 1) || (pAbility->Full_10G == 1))
            return RT_ERR_INPUT;

        osal_memset(&ability, 0x00, sizeof(rtksw_port_mac_ability_t));
        if ((retVal = dal_rtl8371c_port_macForceLinkExt_set(unit, port, RTKSW_MODE_EXT_1000X_100FX, &ability)) != RT_ERR_OK)
            return retVal;
    }
    else
    {
        /* Only one ability can be set. */
        if ((pAbility->Full_100 + pAbility->Full_1000 + pAbility->Full_2P5G + pAbility->Full_5G + pAbility->Full_10G) > 1)
            return RT_ERR_INPUT;

        /* Only EXT_PORT1 support 5G & 10G */
        if (port == EXT_PORT0)
        {
            if ((pAbility->Full_5G == 1) || (pAbility->Full_10G == 1))
                return RT_ERR_INPUT;
        }

        if (pAbility->Full_100 == 1)
            mode = RTKSW_MODE_EXT_100FX;
        else if (pAbility->Full_1000 == 1)
            mode = RTKSW_MODE_EXT_1000X;
        else if (pAbility->Full_2P5G == 1)
            mode = RTKSW_MODE_EXT_FIBER_2P5G;
        else if (pAbility->Full_5G == 1)
            mode = RTKSW_MODE_EXT_5GR;
        else if (pAbility->Full_10G == 1)
            mode = RTKSW_MODE_EXT_10GR;
        else
            return RT_ERR_INPUT;

        osal_memset(&ability, 0x00, sizeof(rtksw_port_mac_ability_t));
        if ((retVal = dal_rtl8371c_port_macForceLinkExt_set(unit, port, mode, &ability)) != RT_ERR_OK)
            return retVal;
    }

    /* Flow Control */
    /* 100M/1000M/2.5G: Page 2, reg 4 bit[8]:AsyFC, bit[7]:FC */
    /* 5G/10G: Page 31, reg 11 bit[3]:AsyFC, bit[2]:FC */
    sdsID = (port == EXT_PORT0) ? 0 : 1;

    if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 2, 4, &regData)) != RT_ERR_OK)
        return retVal;
    
    if (pAbility->FC == 1)
        regData |= (0x0001 << 7);
    else
        regData &= ~(0x0001 << 7);
    
    if (pAbility->AsyFC == 1)
        regData |= (0x0001 << 8);
    else
        regData &= ~(0x0001 << 8);

    if ((retVal = dal_rtl8371c_port_serdesReg_set(unit, sdsID, 2, 4, regData)) != RT_ERR_OK)
        return retVal;

    if (sdsID == 1)
    {
        if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 31, 11, &regData)) != RT_ERR_OK)
            return retVal;
        
        if (pAbility->FC == 1)
            regData |= (0x0001 << 2);
        else
            regData &= ~(0x0001 << 2);
        
        if (pAbility->AsyFC == 1)
            regData |= (0x0001 << 3);
        else
            regData &= ~(0x0001 << 3);

        if ((retVal = dal_rtl8371c_port_serdesReg_set(unit, sdsID, 31, 11, regData)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_fiberAbility_get
 * Description:
 *      Get fiber port ability
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 * Output:
 *      pAbility    - Fiber port ability
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port ID.
 * Note:
 *      The API can get fiber port ability
 */
rtksw_api_ret_t dal_rtl8371c_port_fiberAbility_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_fiber_ability_t *pAbility)
{
    rtksw_api_ret_t retVal;
    rtksw_port_mac_ability_t ability;
    rtksw_mode_ext_t mode;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check port is EXT */
    RTKSW_CHK_PORT_IS_EXT(unit, port);

    if (pAbility == NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = dal_rtl8371c_port_macForceLinkExt_get(unit, port, &mode, &ability)) != RT_ERR_OK)
        return retVal;
 
    osal_memset(pAbility, 0x00, sizeof(rtksw_port_fiber_ability_t));
    if (mode == RTKSW_MODE_EXT_1000X_100FX)
    {
        pAbility->Full_100 = 1;
        pAbility->Full_1000 = 1;
    }
    else if (mode == RTKSW_MODE_EXT_1000X)
        pAbility->Full_1000 = 1;
    else if (mode == RTKSW_MODE_EXT_100FX)
        pAbility->Full_100 = 1;
    else if (mode == RTKSW_MODE_EXT_FIBER_2P5G)
        pAbility->Full_2P5G = 1;
    else if (mode == RTKSW_MODE_EXT_5GR)
        pAbility->Full_5G = 1;
    else if (mode == RTKSW_MODE_EXT_10GR)
        pAbility->Full_10G = 1;
    else
        return RT_ERR_FAILED;

    /* Flow Control */
    /* 100M/1000M/2.5G: Page 2, reg 4 bit[8]:AsyFC, bit[7]:FC */
    /* 5G/10G: Page 31, reg 11 bit[3]:AsyFC, bit[2]:FC */
    if (port == EXT_PORT0)
    {
        if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, 0, 2, 4, &regData)) != RT_ERR_OK)
            return retVal;

        pAbility->FC = (regData & (0x0001 << 7)) ? 1 : 0;
        pAbility->AsyFC = (regData & (0x0001 << 8)) ? 1 : 0;
        
    }
    else /* EXT_PORT1 */
    {
        if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, 1, 31, 11, &regData)) != RT_ERR_OK)
            return retVal;

        pAbility->FC = (regData & (0x0001 << 2)) ? 1 : 0;
        pAbility->AsyFC = (regData & (0x0001 << 3)) ? 1 : 0;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_port_phyMdx_set
 * Description:
 *      Set PHY MDI/MDIX state
 * Input:
 *      unit        - Unit ID
 *      port        - port ID
 *      mode        - PHY MDI/MDIX mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 * Note:
 *      The API can set PHY MDI/MDIX state
 */
rtksw_api_ret_t dal_rtl8371c_port_phyMdx_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_mdix_mode_t mode)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if (mode >= PHY_FORCE_MODE_END)
        return RT_ERR_INPUT;

    if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xa430, &regData)) != RT_ERR_OK)
        return retVal;

    regData &= ~0x0300;
    if (mode == PHY_AUTO_CROSSOVER_MODE)
        regData |= 0x0100;
    else if (mode == PHY_FORCE_MDI_MODE)
        regData |= 0x0300;
    else if (mode == PHY_FORCE_MDIX_MODE)
        regData |= 0x0200;
    else
        return RT_ERR_INPUT;

    if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa430, regData)) != RT_ERR_OK)
        return retVal;

    /* Restart N-way */
    if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, 0, &regData))!=RT_ERR_OK)
        return retVal;

    regData |= (0x0001 << 9);
    if ((retVal = dal_rtl8371c_port_phyReg_set(unit, port, 0, regData))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyMdx_get
 * Description:
 *      Get PHY MDI/MDIX state
 * Input:
 *      unit        - Unit ID
 *      port        - port ID
 * Output:
 *      pMode       - PHY MDI/MDIX mode
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 * Note:
 *      The API can get PHY MDI/MDIX state
 */
rtksw_api_ret_t dal_rtl8371c_port_phyMdx_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_mdix_mode_t *pMode)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if (pMode == NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xa430, &regData)) != RT_ERR_OK)
        return retVal;

    if ((regData & 0x0300) == 0x0100)
        *pMode = PHY_AUTO_CROSSOVER_MODE;
    else if ((regData & 0x0300) == 0x0300)
        *pMode = PHY_FORCE_MDI_MODE;
    else if ((regData & 0x0300) == 0x0200)
        *pMode = PHY_FORCE_MDIX_MODE;
    else
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyMdxStatus_get
 * Description:
 *      Get PHY MDI/MDIX status
 * Input:
 *      unit        - Unit ID
 *      port        - port ID
 * Output:
 *      pStatus     - PHY MDI/MDIX status
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 * Note:
 *      The API can get PHY MDI/MDIX status
 */
rtksw_api_ret_t dal_rtl8371c_port_phyMdxStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_mdix_status_t *pStatus)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if (pStatus == NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xa430, &regData))!=RT_ERR_OK)
        return retVal;

    if (regData & (0x0001 << 9))
    {
        if (regData & (0x0001 << 8))
            *pStatus = PHY_STATUS_FORCE_MDI_MODE;
        else
            *pStatus = PHY_STATUS_FORCE_MDIX_MODE;
    }
    else
    {
        if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xa434, &regData))!=RT_ERR_OK)
            return retVal;

        if (regData & (0x0001 << 1))
            *pStatus = PHY_STATUS_AUTO_MDI_MODE;
        else
            *pStatus = PHY_STATUS_AUTO_MDIX_MODE;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyTestMode_set
 * Description:
 *      Set PHY in test mode.
 * Input:
 *      unit    - Unit ID
 *      port    - port id.
 *      mode    - PHY test mode 0:normal 1:test mode 1 2:test mode 2 3: test mode 3 4:test mode 4 5~7:reserved
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 *      RT_ERR_NOT_ALLOWED      - The Setting is not allowed, caused by set more than 1 port in Test mode.
 * Note:
 *      
 */
rtksw_api_ret_t dal_rtl8371c_port_phyTestMode_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_test_mode_t mode)
{
    rtksw_api_ret_t       retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    /* Only 2.5G PHY port can support 2.5G test mode */
    if ((rtksw_switch_isUtp2p5gPort(unit, port) != RT_ERR_OK) && (mode >= RTKSW_PHY_2P5G_TEST_MODE_1))
        return RT_ERR_INPUT;

    switch(mode)
    {
        case RTKSW_PHY_TEST_MODE_NORMAL:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa610, 0x2058)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa640, 0x3000)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa61a, 0x0400)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa400, 0x1140)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa430, 0x019d)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa412, 0x0E00)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_TEST_MODE_1:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa400, 0x0140)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa430, 0x0199)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa412, 0x2E00)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_TEST_MODE_2:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa400, 0x0140)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa430, 0x0199)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa412, 0x4E00)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_TEST_MODE_3:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa400, 0x0140)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa430, 0x0199)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa412, 0x6E00)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_TEST_MODE_4:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa400, 0x0140)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa430, 0x0199)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa412, 0x8E00)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_2P5G_TEST_MODE_1:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa610, 0x2058)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa640, 0x2000)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa61a, 0x2400)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_2P5G_TEST_MODE_2:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa610, 0x2058)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa640, 0x2000)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa61a, 0x4400)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_2P5G_TEST_MODE_3:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa610, 0x2058)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa640, 0x2000)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa61a, 0x6400)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_2P5G_TEST_MODE_4_TONE_1:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa610, 0x2058)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa640, 0x2000)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa61a, 0x8400)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_2P5G_TEST_MODE_4_TONE_2:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa610, 0x2058)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa640, 0x2000)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa61a, 0x8800)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_2P5G_TEST_MODE_4_TONE_3:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa610, 0x2058)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa640, 0x2000)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa61a, 0x9000)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_2P5G_TEST_MODE_4_TONE_4:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa610, 0x2058)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa640, 0x2000)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa61a, 0x9400)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_2P5G_TEST_MODE_4_TONE_5:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa610, 0x2058)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa640, 0x2000)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa61a, 0x9800)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_2P5G_TEST_MODE_5:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa610, 0x2058)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa640, 0x2000)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa61a, 0xA400)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_2P5G_TEST_MODE_6:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa610, 0x2058)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa640, 0x2000)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa61a, 0xC400)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PHY_2P5G_TEST_MODE_7:
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa610, 0x2058)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa640, 0x2000)) != RT_ERR_OK)
                return retVal;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa61a, 0xE400)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyTestMode_get
 * Description:
 *      Get PHY in which test mode.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      mode - PHY test mode 0:normal 1:test mode 1 2:test mode 2 3: test mode 3 4:test mode 4 5~7:reserved
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      
 */
rtksw_api_ret_t dal_rtl8371c_port_phyTestMode_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_port_phy_test_mode_t *pMode)
{
    rtksw_uint32      data;
    rtksw_api_ret_t   retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if (pMode == NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xa412, &data)) != RT_ERR_OK)
        return retVal;

    if (data == 0x2E00)
        *pMode = RTKSW_PHY_TEST_MODE_1;
    else if (data == 0x4E00)
        *pMode = RTKSW_PHY_TEST_MODE_2;
    else if (data == 0x6E00)
        *pMode = RTKSW_PHY_TEST_MODE_3;
    else if (data == 0x8E00)
        *pMode = RTKSW_PHY_TEST_MODE_4;
    else if (data == 0x0E00)
    {
        if (rtksw_switch_isUtp2p5gPort(unit, port) != RT_ERR_OK)
            *pMode = RTKSW_PHY_TEST_MODE_NORMAL;
        else
        {
            if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xa61a, &data)) != RT_ERR_OK)
                return retVal;
            
            if (data == 0x2400)
                *pMode = RTKSW_PHY_2P5G_TEST_MODE_1;
            else if (data == 0x4400)
                *pMode = RTKSW_PHY_2P5G_TEST_MODE_2;
            else if (data == 0x6400)
                *pMode = RTKSW_PHY_2P5G_TEST_MODE_3;
            else if (data == 0x8400)
                *pMode = RTKSW_PHY_2P5G_TEST_MODE_4_TONE_1;
            else if (data == 0x8800)
                *pMode = RTKSW_PHY_2P5G_TEST_MODE_4_TONE_2;
            else if (data == 0x9000)
                *pMode = RTKSW_PHY_2P5G_TEST_MODE_4_TONE_3;
            else if (data == 0x9400)
                *pMode = RTKSW_PHY_2P5G_TEST_MODE_4_TONE_4;
            else if (data == 0x9800)
                *pMode = RTKSW_PHY_2P5G_TEST_MODE_4_TONE_5;
            else if (data == 0xA400)
                *pMode = RTKSW_PHY_2P5G_TEST_MODE_5;
            else if (data == 0xC400)
                *pMode = RTKSW_PHY_2P5G_TEST_MODE_6;
            else if (data == 0xE400)
                *pMode = RTKSW_PHY_2P5G_TEST_MODE_7;
            else if (data == 0x0400)
                *pMode = RTKSW_PHY_TEST_MODE_NORMAL;
            else
                return RT_ERR_FAILED;
        }
    }
    else
        return RT_ERR_FAILED;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_maxPacketLength_set
 * Description:
 *      Set Max packet length per port
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      length  - Max packet length.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_port_maxPacketLength_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 length)
{
    rtksw_api_ret_t retVal;
    
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (length > RTL8371C_MAX_PACKET_LENGTH)
        return RT_ERR_INPUT;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PKTMAXLENr, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_MAX_LENGTHf, &length)) != RT_ERR_OK)
        return retVal;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_maxPacketLength_get
 * Description:
 *      Get Max packet length per port
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pLength - Pointer of Max packet length.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_port_maxPacketLength_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 *pLength)
{
    rtksw_api_ret_t retVal;
    
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (pLength == NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PKTMAXLENr, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_MAX_LENGTHf, pLength)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyLinkDownPowerSaving_set
 * Description:
 *      Set Ports Link Down Power Saving state.
 * Input:
 *      unit    - Unit ID
 *      port    - port ID
 *      state   - Link Down Power Saving state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_SMI      - SMI access error
 *      RT_ERR_ENABLE   - Invalid enable input.
 * Note:
 *      This API can set Port Link Down Power Saving state.
 *      The configuration is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtksw_api_ret_t dal_rtl8371c_port_phyLinkDownPowerSaving_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t state)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if (state >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xA430, &regData)) != RT_ERR_OK)
        return retVal;

    if (state == RTKSW_ENABLED)
        regData |= (0x0001 << 2);
    else
        regData &= ~(0x0001 << 2);

    if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xA430, regData)) != RT_ERR_OK)
        return retVal;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_phyLinkDownPowerSaving_get
 * Description:
 *      Get Ports Link Down Power Saving state.
 * Input:
 *      unit    - Unit ID
 *      port    - port ID
 * Output:
 *      pState  - Link Down Power Saving state.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API can get Link Down Power Saving state.
 */
rtksw_api_ret_t dal_rtl8371c_port_phyLinkDownPowerSaving_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pState)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if (pState == NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xA430, &regData)) != RT_ERR_OK)
        return retVal;

    if (regData & (0x0001 << 2))
        *pState = RTKSW_ENABLED;
    else
        *pState = RTKSW_DISABLED;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_serdesReg_set
 * Description:
 *      Set Serdes register data of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id.
 *      page    - Page id.
 *      reg     - Register id
 *      data    - Register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Error input
 *      RT_ERR_BUSYWAIT_TIMEOUT - Access busy
 * Note:
 *      This API can set serdes register data of the specific sdsID.
 */
rtksw_api_ret_t dal_rtl8371c_port_serdesReg_set(rtksw_uint32 unit, rtksw_uint32 sdsID, rtksw_uint32 page, rtksw_uint32 reg, rtksw_uint32 data)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 busyCnt = RTL8371C_SDS_ACCESS_TIMEOUT;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (sdsID > RTL8371C_MAX_SDS_ID)
        return RT_ERR_INPUT;

    if (page > RTL8371C_MAX_SDS_PAGE)
        return RT_ERR_INPUT;
    
    if (reg > RTL8371C_MAX_SDS_REGISTER)
        return RT_ERR_INPUT;

    if (data > 0xFFFF)
        return RT_ERR_INPUT;

    /* Busy Checking */
    if ((retVal = reg16_field_read(unit, RTL8371C_SDS_INDACS_CMDr, RTL8371C_SDS_CMD_BUSYf, &regData)) != RT_ERR_OK)
        return retVal;

    if (regData != 0)
        return RT_ERR_BUSYWAIT_TIMEOUT;
    
    /* Data */
    regData = data;
    if ((retVal = reg16_field_write(unit, RTL8371C_SDS_INDACS_DATAr, RTL8371C_SDS_DATAf, &regData)) != RT_ERR_OK)
        return retVal;

    /* Page */
    regData = page;
    if ((retVal = reg16_field_write(unit, RTL8371C_SDS_INDACS_ADRr, RTL8371C_SDS_PAGEf, &regData)) != RT_ERR_OK)
        return retVal;

    /* register */
    regData = reg;
    if ((retVal = reg16_field_write(unit, RTL8371C_SDS_INDACS_ADRr, RTL8371C_SDS_REGADf, &regData)) != RT_ERR_OK)
        return retVal;

    /* RWOP = Write */
    regData = RTL8371C_SDS_WRITE_OPERATION;
    if ((retVal = reg16_field_write(unit, RTL8371C_SDS_INDACS_CMDr, RTL8371C_SDS_RWOPf, &regData)) != RT_ERR_OK)
        return retVal;

    /* Serdes index */
    regData = sdsID;
    if ((retVal = reg16_field_write(unit, RTL8371C_SDS_INDACS_CMDr, RTL8371C_SDS_INDEXf, &regData)) != RT_ERR_OK)
        return retVal;

    /* Trigger command */
    regData = 1;
    if ((retVal = reg16_field_write(unit, RTL8371C_SDS_INDACS_CMDr, RTL8371C_SDS_CMDf, &regData)) != RT_ERR_OK)
        return retVal;

    /* Polling until command finish */
    while(busyCnt)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_SDS_INDACS_CMDr, RTL8371C_SDS_CMD_BUSYf, &regData)) != RT_ERR_OK)
            return retVal;

        if (regData == 0)
            break;

        busyCnt--;
        if(busyCnt == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_serdesReg_get
 * Description:
 *      Get Serdes register data of the specific port.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      page    - Page id.
 *      reg     - Register id
 * Output:
 *      pData   - Register data
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Error input
 *      RT_ERR_NULL_POINTER     - NULL pointer
 *      RT_ERR_BUSYWAIT_TIMEOUT - Access busy
 * Note:
 *      This API can get Serdes register data of the specific sdsID.
 */
rtksw_api_ret_t dal_rtl8371c_port_serdesReg_get(rtksw_uint32 unit, rtksw_uint32 sdsID, rtksw_uint32 page, rtksw_uint32 reg, rtksw_uint32 *pData)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 busyCnt = RTL8371C_SDS_ACCESS_TIMEOUT;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (sdsID > RTL8371C_MAX_SDS_ID)
        return RT_ERR_INPUT;

    if (page > RTL8371C_MAX_SDS_PAGE)
        return RT_ERR_INPUT;
    
    if (reg > RTL8371C_MAX_SDS_REGISTER)
        return RT_ERR_INPUT;

    if (pData == NULL)
        return RT_ERR_NULL_POINTER;

    /* Busy Checking */
    if ((retVal = reg16_field_read(unit, RTL8371C_SDS_INDACS_CMDr, RTL8371C_SDS_CMD_BUSYf, &regData)) != RT_ERR_OK)
        return retVal;

    if (regData != 0)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    /* Page */
    regData = page;
    if ((retVal = reg16_field_write(unit, RTL8371C_SDS_INDACS_ADRr, RTL8371C_SDS_PAGEf, &regData)) != RT_ERR_OK)
        return retVal;

    /* register */
    regData = reg;
    if ((retVal = reg16_field_write(unit, RTL8371C_SDS_INDACS_ADRr, RTL8371C_SDS_REGADf, &regData)) != RT_ERR_OK)
        return retVal;

    /* RWOP = Read */
    regData = RTL8371C_SDS_READ_OPERATION;
    if ((retVal = reg16_field_write(unit, RTL8371C_SDS_INDACS_CMDr, RTL8371C_SDS_RWOPf, &regData)) != RT_ERR_OK)
        return retVal;

    /* Serdes index */
    regData = sdsID;
    if ((retVal = reg16_field_write(unit, RTL8371C_SDS_INDACS_CMDr, RTL8371C_SDS_INDEXf, &regData)) != RT_ERR_OK)
        return retVal;

    /* Trigger command */
    regData = 1;
    if ((retVal = reg16_field_write(unit, RTL8371C_SDS_INDACS_CMDr, RTL8371C_SDS_CMDf, &regData)) != RT_ERR_OK)
        return retVal;

    /* Polling until command finish */
    while(busyCnt)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_SDS_INDACS_CMDr, RTL8371C_SDS_CMD_BUSYf, &regData)) != RT_ERR_OK)
            return retVal;

        if (regData == 0)
            break;

        busyCnt--;
        if(busyCnt == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    /* Read Data */
    if ((retVal = reg16_field_read(unit, RTL8371C_SDS_INDACS_DATAr, RTL8371C_SDS_DATAf, &regData)) != RT_ERR_OK)
        return retVal;

    *pData = regData;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_serdesPolarity_set
 * Description:
 *      Set Serdes polarity.
 * Input:
 *      unit            - Unit ID
 *      sdsID           - Serdes id.
 *      inputPolarity   - Input Polarity.
 *      outputPolarity  - Output Polarity
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Error input
 * Note:
 *      This API can set serdes polarity of the specific sdsID.
 */
rtksw_api_ret_t dal_rtl8371c_port_serdesPolarity_set(rtksw_uint32 unit, rtksw_uint32 sdsID, rtksw_port_sdsPolarity_t inputPolarity, rtksw_port_sdsPolarity_t outputPolarity)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (sdsID > RTL8371C_MAX_SDS_ID)
        return RT_ERR_INPUT;

    if ((inputPolarity >= SDS_POLARITY_END) || (outputPolarity >= SDS_POLARITY_END))
        return RT_ERR_INPUT;

    /* 3.125G/1.25G TX & RX polarity: */
    /* Page 0, reg 0 bit[9]=1(Invert Serdes Rx) ,bit[8]=1(Invert Serdes Tx) */
    if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 0, 0, &regData)) != RT_ERR_OK)
        return retVal;
        
    if (inputPolarity == SDS_POLARITY_REVERSE)
        regData |= (0x0001 << 9);
    else
        regData &= ~(0x0001 << 9);

    if (outputPolarity == SDS_POLARITY_REVERSE)
        regData |= (0x0001 << 8);
    else
        regData &= ~(0x0001 << 8);
    
    if ((retVal = dal_rtl8371c_port_serdesReg_set(unit, sdsID, 0, 0, regData)) != RT_ERR_OK)
        return retVal;
 
    if (sdsID == 1)
    {
        /* 10.3125G TX & RX polarity: */
        /* Page 6, reg 2 bit[14]=1(Invert Serdes Tx) ,bit[13]=1(Invert Serdes Rx) */
        if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 6, 2, &regData)) != RT_ERR_OK)
            return retVal;
            
        if (inputPolarity == SDS_POLARITY_REVERSE)
            regData |= (0x0001 << 13);
        else
            regData &= ~(0x0001 << 13);

        if (outputPolarity == SDS_POLARITY_REVERSE)
            regData |= (0x0001 << 14);
        else
            regData &= ~(0x0001 << 14);
        
        if ((retVal = dal_rtl8371c_port_serdesReg_set(unit, sdsID, 6, 2, regData)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_port_serdesPolarity_get
 * Description:
 *      Get Serdes polarity.
 * Input:
 *      unit            - Unit ID
 *      sdsID           - Serdes id.
 * Output:
 *      pInputPolarity  - Input Polarity.
 *      pOutputPolarity - Output Polarity
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Error input
 *      RT_ERR_NULL_POINTER     - Null pointer
 * Note:
 *      This API can set serdes polarity of the specific sdsID.
 */
rtksw_api_ret_t dal_rtl8371c_port_serdesPolarity_get(rtksw_uint32 unit, rtksw_uint32 sdsID, rtksw_port_sdsPolarity_t *pInputPolarity, rtksw_port_sdsPolarity_t *pOutputPolarity)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 cntTx = 0;
    rtksw_uint32 cntRx = 0;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (sdsID > RTL8371C_MAX_SDS_ID)
        return RT_ERR_INPUT;

    if ((pInputPolarity == NULL) || (pOutputPolarity == NULL))
        return RT_ERR_NULL_POINTER;

    /* 3.125G/1.25G TX & RX polarity: */
    /* Page 0, reg 0 bit[9]=1(Invert Serdes Rx) ,bit[8]=1(Invert Serdes Tx) */
    if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 0, 0, &regData)) != RT_ERR_OK)
        return retVal;
    
    cntRx += ((regData & (0x0001 << 9))) != 0 ? 1 : 0;
    cntTx += ((regData & (0x0001 << 8))) != 0 ? 1 : 0;

    if (sdsID == 0)
    {
        *pInputPolarity = ((regData & (0x0001 << 9))) != 0 ? SDS_POLARITY_REVERSE : SDS_POLARITY_NORMAL;
        *pOutputPolarity = ((regData & (0x0001 << 8))) != 0 ? SDS_POLARITY_REVERSE : SDS_POLARITY_NORMAL;
    }
    else
    {
        /* 10.3125G TX & RX polarity: */
        /* Page 6, reg 2 bit[14]=1(Invert Serdes Tx) ,bit[13]=1(Invert Serdes Rx) */
        if ((retVal = dal_rtl8371c_port_serdesReg_get(unit, sdsID, 6, 2, &regData)) != RT_ERR_OK)
            return retVal;
        
        cntRx += ((regData & (0x0001 << 13))) != 0 ? 1 : 0;
        cntTx += ((regData & (0x0001 << 14))) != 0 ? 1 : 0;

        if (cntRx == 0)
            *pInputPolarity = SDS_POLARITY_NORMAL;
        else if (cntRx == 2)
            *pInputPolarity = SDS_POLARITY_REVERSE;
        else   
            return RT_ERR_NOT_INIT;

        if (cntTx == 0)
            *pOutputPolarity = SDS_POLARITY_NORMAL;
        else if (cntTx == 2)
            *pOutputPolarity = SDS_POLARITY_REVERSE;
        else   
            return RT_ERR_NOT_INIT;
    }

    return RT_ERR_OK;
}
