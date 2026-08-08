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
#include <dal/reg.h>
#include <dal/rtl8371c/dal_rtl8371c_switch.h>
#include <dal/rtl8371c/dal_rtl8371c_port.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

static rtksw_api_ret_t _dal_rtl8371c_switch_enable_patch_request(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_port_t port;
    rtksw_uint32 regData;
    rtksw_uint32 modelID;
    rtksw_uint32 phyState;
    rtksw_uint32 counter;

    if ((retVal = reg16_field_read(0, RTL8371C_MODEL_NAME_IDr, RTL8371C_RTL_IDf, &modelID)) != RT_ERR_OK)
        return retVal;

    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        if ((rtksw_switch_isUtpPort(unit, port) == RT_ERR_OK) && ((rtksw_switch_isUtp2p5gPort(unit, port) == RT_ERR_OK) || (modelID == 0x8366)))
        {   
            if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xA46A, &regData)) != RT_ERR_OK)
                return retVal;

            phyState = ((regData & 0x0700) >> 8);
            if ((phyState != 3) && (phyState != 5))
                return RT_ERR_FAILED;

            /* Enable Patch request */
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xB820, 0x0010)) != RT_ERR_OK)
                return retVal;
            
            /* polling until PHY patch ready */
            counter = 0;
            do 
            {
                if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xb800, &regData)) != RT_ERR_OK)
                    return retVal;

                if ((regData & 0x40) != 0 )
                    break;
                
                counter++;
            } while (counter < 200);   //Wait for patch ready = 1...

            if ((regData & 0x40) == 0 )
                return RT_ERR_BUSYWAIT_TIMEOUT;
        }
    }

    return RT_ERR_OK;
}

static rtksw_api_ret_t _dal_rtl8371c_switch_init_top(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    regData = 0x00A5;
    if ((retVal = reg16_write(unit, RTL8371C_CFG_PHY_TX_POLARITY_SWAPr, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

static rtksw_api_ret_t _dal_rtl8371c_switch_init_afe(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_port_t port;
    rtksw_uint32 idx;
    rtksw_uint32 afePatchData[][2] = {{0xbcc2, 0x0001}, {0xa5d0, 0x0000}, {0xa6d4, 0x0000}, {0xa4a2, 0x0101}, {0xa428, 0x0000}, {0xa6d8, 0x010C}};

    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        if ((rtksw_switch_isUtpPort(unit, port) == RT_ERR_OK) && (rtksw_switch_isUtp2p5gPort(unit, port) == RT_ERR_OK))
        {   
            for(idx = 0; idx < sizeof(afePatchData) / (sizeof(rtksw_uint32) * 2); idx++)
            {
                if((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, afePatchData[idx][0], afePatchData[idx][1])) != RT_ERR_OK)
                    return retVal;
            }
        }
    }

    return RT_ERR_OK;
}

static rtksw_api_ret_t _dal_rtl8371c_switch_init_gigaPHY(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_port_t port;
    rtksw_uint32 modelID;
    rtksw_uint32 idx;
    rtksw_uint32 gigaPHYPatchData[][2] = {
        {0xa436, 0x8048}, {0xa438, 0x1903}, {0xa436, 0x804E}, {0xa438, 0x1903}, {0xa436, 0x8054}, {0xa438, 0x1903}, {0xa436, 0x805A}, {0xa438, 0x1903},
        {0xa436, 0x8060}, {0xa438, 0x1903}, {0xa436, 0x8066}, {0xa438, 0x1903}, {0xa436, 0x806C}, {0xa438, 0x1903}, {0xa5d0, 0x0000}, {0xa4a2, 0x0101},
        {0xa428, 0x0000}};

    if ((retVal = reg16_field_read(0, RTL8371C_MODEL_NAME_IDr, RTL8371C_RTL_IDf, &modelID)) != RT_ERR_OK)
        return retVal;

    if (modelID == 0x8366)
    {
        RTKSW_SCAN_ALL_LOG_PORT(unit, port)
        {
            if ((rtksw_switch_isUtpPort(unit, port) == RT_ERR_OK) && (rtksw_switch_isUtp2p5gPort(unit, port) != RT_ERR_OK))
            {   
                for(idx = 0; idx < sizeof(gigaPHYPatchData) / (sizeof(rtksw_uint32) * 2); idx++)
                {
                    if((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, gigaPHYPatchData[idx][0], gigaPHYPatchData[idx][1])) != RT_ERR_OK)
                        return retVal;
                }
            }
        }
    }

    return RT_ERR_OK;
}

static rtksw_api_ret_t _dal_rtl8371c_switch_init_dataram(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_port_t port;
    rtksw_uint32 idx;
    rtksw_uint32 regData;
    rtksw_uint32 dataramPatchData[][2] = {
        {0xb88e, 0xC201}, {0xb890, 0x8e01}, {0xb88e, 0xC203}, {0xb890, 0x2001}, {0xb88e, 0xC205}, {0xb890, 0x5902}, {0xb88e, 0xC207}, {0xb890, 0x2802},
        {0xb88e, 0xC209}, {0xb890, 0xD803}, {0xb88e, 0xC20B}, {0xb890, 0xA504}, {0xb88e, 0xC20D}, {0xb890, 0x1004}, {0xb88e, 0xC20F}, {0xb890, 0xc005},
        {0xb88e, 0xC211}, {0xb890, 0x7506}, {0xb88e, 0xC213}, {0xb890, 0x6f07}, {0xb88e, 0xC215}, {0xb890, 0x9208}, {0xb88e, 0xC217}, {0xb890, 0xe00a},
        {0xb88e, 0xC219}, {0xb890, 0x900c}, {0xb88e, 0xC21B}, {0xb890, 0x000d}, {0xb88e, 0xC21D}, {0xb890, 0xbe10}, {0xb88e, 0xC21F}, {0xb890, 0x52f1},
        {0xb88e, 0xC221}, {0xb890, 0xf1ef}, {0xb88e, 0xC223}, {0xb890, 0xebea}, {0xb88e, 0xC225}, {0xb890, 0xeae6}, {0xb88e, 0xC227}, {0xb890, 0xe3e2},
        {0xb88e, 0xC229}, {0xb890, 0xe2df}, {0xb88e, 0xC22B}, {0xb890, 0xdddc}, {0xb88e, 0xC22D}, {0xb890, 0xd9d8}, {0xb88e, 0xC22F}, {0xb890, 0xd800},
        {0xb88e, 0xC23F}, {0xb890, 0x0003}, {0xb88e, 0xC241}, {0xb890, 0x0304}, {0xb88e, 0xC243}, {0xb890, 0x0507}, {0xb88e, 0xC245}, {0xb890, 0x0b0c},
        {0xb88e, 0xC247}, {0xb890, 0x0e10}, {0xb88e, 0xC249}, {0xb890, 0x1213}, {0xb88e, 0xC24B}, {0xb890, 0x1516}, {0xb88e, 0xC24D}, {0xb890, 0x1717},
        {0xb88e, 0xC24F}, {0xb890, 0x180D}, {0xb88e, 0xC25F}, {0xb890, 0x0d0e}, {0xb88e, 0xC261}, {0xb890, 0x0e10}, {0xb88e, 0xC263}, {0xb890, 0x1517},
        {0xb88e, 0xC265}, {0xb890, 0x1820}, {0xb88e, 0xC267}, {0xb890, 0x232b}, {0xb88e, 0xC269}, {0xb890, 0x3142}, {0xb88e, 0xC26B}, {0xb890, 0x444f},
        {0xb88e, 0xC26D}, {0xb890, 0x6d75}, {0xb88e, 0xC26F}, {0xb890, 0x7517}, {0xb88e, 0xC271}, {0xb890, 0x1719}, {0xb88e, 0xC273}, {0xb890, 0x1A1D},
        {0xb88e, 0xC275}, {0xb890, 0x1F21}, {0xb88e, 0xC277}, {0xb890, 0x2223}, {0xb88e, 0xC279}, {0xb890, 0x2525}, {0xb88e, 0xC27B}, {0xb890, 0x2526},
        {0xb88e, 0xC27D}, {0xb890, 0x2626}, {0xb88e, 0xC27F}, {0xb890, 0x274A}, {0xb88e, 0xC281}, {0xb890, 0x4a4d}, {0xb88e, 0xC283}, {0xb890, 0x4d4f},
        {0xb88e, 0xC285}, {0xb890, 0x5254}, {0xb88e, 0xC287}, {0xb890, 0x5456}, {0xb88e, 0xC289}, {0xb890, 0x5655}, {0xb88e, 0xC28B}, {0xb890, 0x5554},
        {0xb88e, 0xC28D}, {0xb890, 0x5454}, {0xb88e, 0xC28F}, {0xb890, 0x54df}, {0xb87c, 0x81F3}, {0xb87e, 0x1f8f}};

    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        if ((rtksw_switch_isUtpPort(unit, port) == RT_ERR_OK) && (rtksw_switch_isUtp2p5gPort(unit, port) == RT_ERR_OK))
        {   
            /* 0xB820[7] = 0 */
            if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xb820, &regData)) != RT_ERR_OK)
                return retVal;

            regData &= ~(0x0001 << 7);
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xb820, regData)) != RT_ERR_OK)
                return retVal;

            /* 0xB896[0] = 0 */
            if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xb896, &regData)) != RT_ERR_OK)
                return retVal;

            regData &= ~0x0001;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xb896, regData)) != RT_ERR_OK)
                return retVal;

            /* 0xB892[15:8] = 0 */
            if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xb892, &regData)) != RT_ERR_OK)
                return retVal;

            regData &= ~0xFF00;
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xb892, regData)) != RT_ERR_OK)
                return retVal;

            for(idx = 0; idx < sizeof(dataramPatchData) / (sizeof(rtksw_uint32) * 2); idx++)
            {
                if((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, dataramPatchData[idx][0], dataramPatchData[idx][1])) != RT_ERR_OK)
                    return retVal;
            }
        }
    }

    return RT_ERR_OK;
}

static rtksw_api_ret_t _dal_rtl8371c_switch_init_alggiga(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_port_t port;
    rtksw_uint32 idx;
    rtksw_uint32 modelID;
    rtksw_uint32 regData;
    rtksw_uint32 alggigaPatchData[][2] = {
        {0xa436, 0x80a8}, {0xa438, 0xc5c1}, {0xa436, 0x80be}, {0xa438, 0xc6be}, {0xa436, 0x8090}, {0xa438, 0xb008}, {0xa436, 0x8092}, {0xa438, 0x049c},
        {0xa436, 0x8094}, {0xa438, 0xf81f}};

    if ((retVal = reg16_field_read(0, RTL8371C_MODEL_NAME_IDr, RTL8371C_RTL_IDf, &modelID)) != RT_ERR_OK)
        return retVal;

    if (modelID == 0x8366)
    {
        RTKSW_SCAN_ALL_LOG_PORT(unit, port)
        {
            if ((rtksw_switch_isUtpPort(unit, port) == RT_ERR_OK) && (rtksw_switch_isUtp2p5gPort(unit, port) != RT_ERR_OK))
            {   
                /* 0xa87e[12:8] = 0x10 */
                if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xa87e, &regData)) != RT_ERR_OK)
                    return retVal;

                regData &= ~(0x001F << 8);
                regData |= (0x0010 << 8);
                if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xa87e, regData)) != RT_ERR_OK)
                    return retVal;

                for(idx = 0; idx < sizeof(alggigaPatchData) / (sizeof(rtksw_uint32) * 2); idx++)
                {
                    if((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, alggigaPatchData[idx][0], alggigaPatchData[idx][1])) != RT_ERR_OK)
                        return retVal;
                }
            }
        }
    }

    return RT_ERR_OK;
}

static rtksw_api_ret_t _dal_rtl8371c_switch_init_ncrp(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_port_t port;
    rtksw_uint32 idx;
    rtksw_uint32 regData;
    rtksw_uint32 ncrpPatchData[][2] = {
        {0xa436, 0xA016}, {0xa438, 0x0000}, {0xa436, 0xA012}, {0xa438, 0x0FF8}, {0xa436, 0xA014}, {0xa438, 0x8A00}, {0xa438, 0x0a07}, {0xa438, 0x0000},
        {0xa438, 0x0000}, {0xa438, 0x0000}, {0xa438, 0x0000}, {0xa438, 0x0000}, {0xa438, 0x0000}, {0xa436, 0xA152}, {0xa438, 0x1344}, {0xa436, 0xA154},
        {0xa438, 0x0f51}, {0xa436, 0xA156}, {0xa438, 0x3FFF}, {0xa436, 0xA158}, {0xa438, 0x3FFF}, {0xa436, 0xA15A}, {0xa438, 0x3FFF}, {0xa436, 0xA15C},
        {0xa438, 0x3FFF}, {0xa436, 0xA15E}, {0xa438, 0x3FFF}, {0xa436, 0xA160}, {0xa438, 0x3FFF}, {0xa436, 0xA150}, {0xa438, 0x0003}};

    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        if ((rtksw_switch_isUtpPort(unit, port) == RT_ERR_OK) && (rtksw_switch_isUtp2p5gPort(unit, port) == RT_ERR_OK))
        {   
            /* 0xb820[7] = 1 */
            if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xb820, &regData)) != RT_ERR_OK)
                return retVal;

            regData |= (0x0001 << 7);
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xb820, regData)) != RT_ERR_OK)
                return retVal;

            for(idx = 0; idx < sizeof(ncrpPatchData) / (sizeof(rtksw_uint32) * 2); idx++)
            {
                if((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, ncrpPatchData[idx][0], ncrpPatchData[idx][1])) != RT_ERR_OK)
                    return retVal;
            }

            /* 0xb820[7] = 0 */
            if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xb820, &regData)) != RT_ERR_OK)
                return retVal;

            regData &= ~(0x0001 << 7);
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xb820, regData)) != RT_ERR_OK)
                return retVal;
        }
    }

    return RT_ERR_OK;
}

static rtksw_api_ret_t _dal_rtl8371c_switch_init_alg2p5g(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_port_t port;
    rtksw_uint32 idx;
    rtksw_uint32 alg2p5gPatchData[][2] = {{0xb87c, 0x84f8}, {0xb87e, 0x031e}};

    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        if ((rtksw_switch_isUtpPort(unit, port) == RT_ERR_OK) && (rtksw_switch_isUtp2p5gPort(unit, port) == RT_ERR_OK))
        {   
            for(idx = 0; idx < sizeof(alg2p5gPatchData) / (sizeof(rtksw_uint32) * 2); idx++)
            {
                if((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, alg2p5gPatchData[idx][0], alg2p5gPatchData[idx][1])) != RT_ERR_OK)
                    return retVal;
            }
        }
    }

    return RT_ERR_OK;
}

static rtksw_api_ret_t _dal_rtl8371c_switch_disable_patch_request(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_port_t port;
    rtksw_uint32 modelID;

    if ((retVal = reg16_field_read(0, RTL8371C_MODEL_NAME_IDr, RTL8371C_RTL_IDf, &modelID)) != RT_ERR_OK)
        return retVal;

    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        if ((rtksw_switch_isUtpPort(unit, port) == RT_ERR_OK) && ((rtksw_switch_isUtp2p5gPort(unit, port) == RT_ERR_OK) || (modelID == 0x8366)))
        {   
            /* Disable Patch request */
            if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xB820, 0x0000)) != RT_ERR_OK)
                return retVal;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_switch_init
 * Description:
 *      Set chip to default configuration enviroment
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can set chip registers to default configuration for different release chip model.
 */
rtksw_api_ret_t dal_rtl8371c_switch_init(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;

    /* V3.5 20250723 */
    /* Enable Patch Request */
    if ((retVal = _dal_rtl8371c_switch_enable_patch_request(unit)) != RT_ERR_OK)
        return retVal;

    /* 01 Top Patch */ 
    if ((retVal = _dal_rtl8371c_switch_init_top(unit)) != RT_ERR_OK)
        return retVal;

    /* 03 AFE Patch */
    if ((retVal = _dal_rtl8371c_switch_init_afe(unit)) != RT_ERR_OK)
        return retVal;

    /* 04 giga PHY Patch */
    if ((retVal = _dal_rtl8371c_switch_init_gigaPHY(unit)) != RT_ERR_OK)
        return retVal;

    /* 05 dataram Patch */
    if ((retVal = _dal_rtl8371c_switch_init_dataram(unit)) != RT_ERR_OK)
        return retVal;

    /* 06 alg giga Patch */
    if ((retVal = _dal_rtl8371c_switch_init_alggiga(unit)) != RT_ERR_OK)
        return retVal;

    /* 07 nc rp Patch */
    if ((retVal = _dal_rtl8371c_switch_init_ncrp(unit)) != RT_ERR_OK)
        return retVal;
    
    /* 08 alg 2p5g Patch */
    if ((retVal = _dal_rtl8371c_switch_init_alg2p5g(unit)) != RT_ERR_OK)
        return retVal;

    /* Disable Patch Request */
    if ((retVal = _dal_rtl8371c_switch_disable_patch_request(unit)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_switch_mac_set
 * Description:
 *      Set switch MAC address.
 *  Input:
 *      unit        - Unit ID
 *      pMac        - Switch MAC
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Error Input
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      This API configure the MAC address of switch function.
 */
rtksw_api_ret_t dal_rtl8371c_switch_mac_set(rtksw_uint32 unit, rtksw_mac_t *pMac)
{
    rtksw_uint32      regData;
    rtksw_api_ret_t   retVal;

    if(pMac == NULL)
        return RT_ERR_INPUT;

    regData = ((pMac->octet[0] << 8) | pMac->octet[1]);
    if ((retVal = reg16_field_write(unit, RTL8371C_MAC_L2_ADDR_CTRLr, RTL8371C_MAC2f, &regData)) != RT_ERR_OK)
        return retVal;

    regData = ((pMac->octet[2] << 8) | pMac->octet[3]);
    if ((retVal = reg16_field_write(unit, RTL8371C_MAC_L2_ADDR_CTRLr, RTL8371C_MAC1f, &regData)) != RT_ERR_OK)
        return retVal;

    regData = ((pMac->octet[4] << 8) | pMac->octet[5]);
    if ((retVal = reg16_field_write(unit, RTL8371C_MAC_L2_ADDR_CTRLr, RTL8371C_MAC0f, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_switch_mac_get
 * Description:
 *      Get switch MAC address
 *  Input:
 *      unit        - Unit ID
 *      pMac        - switch mac
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      This API get the MAC address of switch function.
 */
rtksw_api_ret_t dal_rtl8371c_switch_mac_get(rtksw_uint32 unit, rtksw_mac_t *pMac)
{
    ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint8 *accessPtr;

    accessPtr = (rtksw_uint8*)pMac;

    if ((retVal = reg16_field_read(unit, RTL8371C_MAC_L2_ADDR_CTRLr, RTL8371C_MAC2f, &regData)) != RT_ERR_OK)
    {
        return retVal;
    }
    *accessPtr = (regData & 0xFF00) >> 8;
    accessPtr ++;
    *accessPtr = regData & 0xFF;
    accessPtr ++;

    if ((retVal = reg16_field_read(unit, RTL8371C_MAC_L2_ADDR_CTRLr, RTL8371C_MAC1f, &regData)) != RT_ERR_OK)
    {
        return retVal;
    }
    *accessPtr = (regData & 0xFF00) >> 8;
    accessPtr ++;
    *accessPtr = regData & 0xFF;
    accessPtr ++;

    if ((retVal = reg16_field_read(unit, RTL8371C_MAC_L2_ADDR_CTRLr, RTL8371C_MAC0f, &regData)) != RT_ERR_OK)
    {
        return retVal;
    }
    *accessPtr = (regData & 0xFF00) >> 8;
    accessPtr ++;
    *accessPtr = regData & 0xFF;
    accessPtr ++;

    return retVal;
}

/* Function Name:
 *      dal_rtl8371c_switch_reset
 * Description:
 *      Reset chip 
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can reset entire chip.
 */
rtksw_api_ret_t dal_rtl8371c_switch_reset(rtksw_uint32 unit)
{
    rtksw_uint32 regData;

    regData = 0x0001;
    reg16_write(unit, RTL8371C_CHIP_RSTr, &regData); /* no need to check return. */
    return RT_ERR_OK;
}