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
#include <dal/rtl8371c/dal_rtl8371c_ptp.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      dal_rtl8371c_ptp_init
 * Description:
 *      PTP function initialization.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API is used to initialize PTP status.
 */
rtksw_api_ret_t dal_rtl8371c_ptp_init(rtksw_uint32 unit)
{
    rtksw_port_t port;
    rtksw_uint32 freq=0x10000000;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        dal_rtl8371c_ptp_portTypeEnable_set(unit, port, RTKSW_PTP_ETH,RTKSW_ENABLED);
        dal_rtl8371c_ptp_portTypeEnable_set(unit, port, RTKSW_PTP_UDP,RTKSW_ENABLED);
    }

    dal_rtl8371c_ptp_refTimeFreqCfg_set(unit, freq, RTKSW_ENABLED);
    
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_portBypassEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      enable  - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_portBypassEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t Enable)
{
    rtksw_api_ret_t   retVal=RT_ERR_OK;
    rtksw_uint32 phyPort;
    
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
    {
        return RT_ERR_PORT_ID;
    }

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PTP_MISC_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_BYPASSf, &Enable)) != RT_ERR_OK)
        return retVal; 

    return retVal;
} /* end of dal_rtl8371c_ptp_portBypassEnable_set */


/* Function Name:
 *      dal_rtl8371c_ptp_portBypassEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_portBypassEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t   retVal=RT_ERR_OK;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
    {
        return RT_ERR_PORT_ID;
    }

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PTP_MISC_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_BYPASSf, pEnable)) != RT_ERR_OK)
        return retVal; 

     return retVal;
} /* end of dal_rtl8371c_ptp_portBypassEnable_get */

/* Function Name:
 *      dal_rtl8371c_ptp_portTypeEnable_set
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      type    - type
 *      Enable  - State
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_portTypeEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type, rtksw_enable_t Enable)
{
    rtksw_api_ret_t   retVal=RT_ERR_OK;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);    

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
    {
        return RT_ERR_PORT_ID;
    }

    if(type==RTKSW_PTP_ETH)
    {
        if ((retVal = reg16_array_field_write(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_ETH_ENf, &Enable)) != RT_ERR_OK)
            return retVal; 
    }
    else if(type==RTKSW_PTP_UDP)
    {
        if ((retVal = reg16_array_field_write(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_UDP_ENf, &Enable)) != RT_ERR_OK)
            return retVal; 
    }
    
    return retVal;
} 

/* Function Name:
 *      dal_rtl8371c_ptp_portTypeEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      type    - type
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_portTypeEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t   retVal=RT_ERR_OK;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
    {
        return RT_ERR_PORT_ID;
    }

    if(type==RTKSW_PTP_ETH)
    {
        if ((retVal = reg16_array_field_read(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_ETH_ENf, pEnable)) != RT_ERR_OK)
            return retVal; 
    }
    else if(type==RTKSW_PTP_UDP)
    {
        if ((retVal = reg16_array_field_read(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_UDP_ENf, pEnable)) != RT_ERR_OK)
            return retVal; 
    } 
    
    return retVal;
} 


/* Function Name:
 *      dal_rtl8371c_ptp_vlanTpid_set
 * Description:
 *      Set PTP accepted outer or inner tag TPID.
 * Input:
 *      unit    - Unit ID
 *      type    - Type
 *      idx     - Index
 *      Tpid    - Ether type of tag frame parsing in PTP ports.
 * Output:
 *      none.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_vlanTpid_set(rtksw_uint32 unit, rtksw_vlanType_t type, rtksw_uint32 idx, rtksw_uint32 Tpid)
{
    rtksw_api_ret_t retVal;
    
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if((type >= RTKSW_PTP_VLAN_TYPE_END) || (idx >= 1))
        return RT_ERR_INPUT;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(type == RTKSW_PTP_OUTER_VLAN)
    {
        if ((retVal = reg16_field_write(unit, RTL8371C_PTP_OTAG_CONFIG0r, RTL8371C_OTAG_TPID_0f, &Tpid)) != RT_ERR_OK)
            return retVal;
    }
    else if(type == RTKSW_PTP_INNER_VLAN)
    {
        if ((retVal = reg16_field_write(unit, RTL8371C_PTP_ITAG_CONFIG0r, RTL8371C_ITAG_TPID_0f, &Tpid)) != RT_ERR_OK)
            return retVal;        
    }
    
    return RT_ERR_OK;
    
}

/* Function Name:
 *      dal_rtl8371c_ptp_vlanTpid_get
 * Description:
 *      Get PTP accepted outer or inner tag TPID.
 * Input:
 *      unit    - Unit ID
 *      type    - Type
 *      idx     - index
 * Output:
 *      pTpid - Ether type of tag frame parsing in PTP ports.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_vlanTpid_get(rtksw_uint32 unit, rtksw_vlanType_t type, rtksw_uint32 idx, rtksw_uint32 *pTpid)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if((pTpid == NULL) || (type >= RTKSW_PTP_VLAN_TYPE_END) || (idx >= 1))
        return RT_ERR_INPUT;
    
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(type == RTKSW_PTP_OUTER_VLAN)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_PTP_OTAG_CONFIG0r, RTL8371C_OTAG_TPID_0f, pTpid)) != RT_ERR_OK)
            return retVal;
    }
    else if(type == RTKSW_PTP_INNER_VLAN)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_PTP_ITAG_CONFIG0r, RTL8371C_ITAG_TPID_0f, pTpid)) != RT_ERR_OK)
            return retVal;         
    }
    
    return RT_ERR_OK;
    
}

/* Function Name:
 *      dal_rtl8371c_ptp_operCfg_set
 * Description:
 *      Set the PTP time operation configuration of specific port.
 * Input:
 *      unit        - Unit ID
 *      pOperCfg    - pointer to PTP time operation configuraton
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_operCfg_set(rtksw_uint32 unit, rtksw_time_operCfg_t *pOperCfg)
{
    rtksw_int32   retVal;
    rtksw_uint32  reg_val = 0;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pOperCfg)
       return RT_ERR_NULL_POINTER;

    switch (pOperCfg->oper)
    {
        case RTKSW_PTP_TIME_OPER_START:
            reg_val = 0;
            break;
        case RTKSW_PTP_TIME_OPER_LATCH:
            reg_val = 1;
            break;
        case RTKSW_PTP_TIME_OPER_STOP:
            reg_val = 2;
            break;
        case RTKSW_PTP_TIME_OPER_CMD_EXEC:
            reg_val = 3;
            break;
        case RTKSW_PTP_TIME_OPER_FREQ_APPLY:
            reg_val = 4;
            break;
        default:
            return RT_ERR_FAILED;
    }
    
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_OP_CTRLr, RTL8371C_CFG_GPI_TIMER_OP_SELf, &reg_val)) != RT_ERR_OK)
        return retVal; 
    
    reg_val = pOperCfg->rise_tri;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_OP_CTRLr, RTL8371C_CFG_GPI_RISE_TRIGf, &reg_val)) != RT_ERR_OK)
        return retVal;

    reg_val = pOperCfg->fall_tri;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_OP_CTRLr, RTL8371C_CFG_GPI_FALL_TRIGf, &reg_val)) != RT_ERR_OK)
        return retVal;

    if(pOperCfg->tri_apply)
    {
        dal_rtl8371c_ptp_oper_triger(unit);
    }
   
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_operCfg_get
 * Description:
 *      Get the PTP time operation configuration of specific port.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pOperCfg    - pointer to PTP time operation configuraton
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_operCfg_get(rtksw_uint32 unit, rtksw_time_operCfg_t *pOperCfg)
{
    rtksw_int32   retVal = RT_ERR_OK;
    rtksw_uint32  reg_val = 0;
    rtksw_uint32  oper = 0;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pOperCfg)
       return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_OP_CTRLr, RTL8371C_CFG_GPI_TIMER_OP_SELf, &oper)) != RT_ERR_OK)
        return retVal;    

    switch (oper)
    {
        case 0:
            pOperCfg->oper = RTKSW_PTP_TIME_OPER_START;
            break;
        case 1:
            pOperCfg->oper = RTKSW_PTP_TIME_OPER_LATCH;
            break;
        case 2:
            pOperCfg->oper = RTKSW_PTP_TIME_OPER_STOP;
            break;
        case 3:
            pOperCfg->oper = RTKSW_PTP_TIME_OPER_CMD_EXEC;
            break;
        case 4:
            pOperCfg->oper = RTKSW_PTP_TIME_OPER_FREQ_APPLY;
            break;
        default:
            return RT_ERR_FAILED;
    }
    
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_OP_CTRLr, RTL8371C_CFG_GPI_RISE_TRIGf, &reg_val)) != RT_ERR_OK)
        return retVal;
    
    pOperCfg->rise_tri = reg_val;
    
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_OP_CTRLr, RTL8371C_CFG_GPI_FALL_TRIGf, &reg_val)) != RT_ERR_OK)
        return retVal;    
    
    pOperCfg->fall_tri = reg_val;
   
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_oper_triger
 * Description:
 *      Set the PTP time operation triger.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_oper_triger(rtksw_uint32 unit)
{
    rtksw_uint32 retVal;
    rtksw_time_opertriger_t  opt=0;
    rtksw_uint32 regVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    regVal = RTKSW_ENABLED;
    if ((retVal = reg16_field_write(unit, RTL8371C_TOD_UART_SETTINGr, RTL8371C_TIMER_GPIO_OEf, &regVal)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_OP_CTRLr, RTL8371C_CFG_GPI_FALL_TRIGf, &regVal)) != RT_ERR_OK)
        return retVal;    
    opt = regVal&0x1;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_OP_CTRLr, RTL8371C_CFG_GPI_RISE_TRIGf, &regVal)) != RT_ERR_OK)
        return retVal;
    opt = opt | ((regVal&0x1)<<1);    
    
    switch(opt)
    {
     case RTKSW_PTP_TIME_RISE_TRI:
         regVal = RTKSW_DISABLED;         
         if ((retVal = reg16_field_write(unit, RTL8371C_TOD_UART_SETTINGr, RTL8371C_TIMER_GPOf, &regVal)) != RT_ERR_OK)
             return retVal;         
         regVal = RTKSW_ENABLED;         
         if ((retVal = reg16_field_write(unit, RTL8371C_TOD_UART_SETTINGr, RTL8371C_TIMER_GPOf, &regVal)) != RT_ERR_OK)
             return retVal;
        break;
    case RTKSW_PTP_TIME_FALL_TRI:
        regVal = RTKSW_ENABLED;       
        if ((retVal = reg16_field_write(unit, RTL8371C_TOD_UART_SETTINGr, RTL8371C_TIMER_GPOf, &regVal)) != RT_ERR_OK)
            return retVal;       
        regVal = RTKSW_DISABLED;       
        if ((retVal = reg16_field_write(unit, RTL8371C_TOD_UART_SETTINGr, RTL8371C_TIMER_GPOf, &regVal)) != RT_ERR_OK)
            return retVal;
        break;
    case RTKSW_PTP_TIME_BOTH_TRI:
        if ((retVal = reg16_field_read(unit, RTL8371C_TOD_UART_SETTINGr, RTL8371C_TIMER_GPOf, &regVal)) != RT_ERR_OK)
            return retVal;
        regVal=((regVal+1)&1);        
        if ((retVal = reg16_field_write(unit, RTL8371C_TOD_UART_SETTINGr, RTL8371C_TIMER_GPOf, &regVal)) != RT_ERR_OK)
            return retVal;    
        break;
    default:
        return RT_ERR_INPUT;
    }
  
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_latchTime_get
 * Description:
 *      Get the PTP latched time of specific port by hardware.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pLatchTime    - pointer to PTP time operation configuraton
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_latchTime_get(rtksw_uint32 unit, rtksw_time_timeStamp_t *pLatchTime)
{
    rtksw_int32   retVal = RT_ERR_OK;
    rtksw_uint32  sec_l = 0;
    rtksw_uint32  sec_m = 0;
    rtksw_uint32  sec_h = 0;
    rtksw_uint32  nsec_l = 0;
    rtksw_uint32  nsec_h = 0;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pLatchTime)
       return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_SEC_RD0r, RTL8371C_RD_PTP_TIME_SEC_Lf, &sec_l)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_SEC_RD1r, RTL8371C_RD_PTP_TIME_SEC_Mf, &sec_m)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_SEC_RD2r, RTL8371C_RD_PTP_TIME_SEC_Hf, &sec_h)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_NSEC_RD0r, RTL8371C_RD_PTP_TIME_NSEC_Lf, &nsec_l)) != RT_ERR_OK)
        return retVal;;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_NSEC_RD1r, RTL8371C_RD_PTP_TIME_NSEC_Hf, &nsec_h)) != RT_ERR_OK)
        return retVal;

    pLatchTime->sec = ((rtksw_uint64)sec_h << 32) | ((rtksw_uint64)sec_m << 16) | ((rtksw_uint64)sec_l & 0xFFFF);
    pLatchTime->nsec = (((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF));
 
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_systemRefTime_set
 * Description:
 *      Set the reference time of the specified device.
 * Input:
 *      unit        - Unit ID
 *      timeStamp   - reference timestamp value
 *      apply       - state of apply
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_systemRefTime_set(rtksw_uint32 unit, rtksw_time_timeStamp_t timeStamp, rtksw_enable_t apply)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 sec_h, sec_m,sec_l, nsec8_h, nsec8_l;
    rtksw_uint32 nano_second_8;
    rtksw_uint32 busyFlag, count;
    rtksw_uint32 regVal;
    
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit); 
    
    if (timeStamp.nsec > RTKSW_MAX_NUM_OF_NANO_SECOND)
        return RT_ERR_INPUT;

      /* adjust Timer for PHYs */
  //  retVal = phy_ptpRefTime_set(timeStamp);

    sec_h = (timeStamp.sec>>32)& 0xFFFF;
    sec_m = (timeStamp.sec >>16)& 0xFFFF;
    sec_l = timeStamp.sec & 0xFFFF;
  //  nano_second_8 = timeStamp.nsec >> 3;
  
    nano_second_8 = timeStamp.nsec ;
    nsec8_h = (nano_second_8 >>16) & 0x3FFF;
    nsec8_l = nano_second_8 &0xFFFF;

    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_NSEC0r, RTL8371C_CFG_PTP_TIME_NSEC_Lf, &nsec8_l)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_NSEC1r, RTL8371C_CFG_PTP_TIME_NSEC_Hf, &nsec8_h)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_SEC0r, RTL8371C_CFG_PTP_TIME_SEC_Lf, &sec_l)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_SEC1r, RTL8371C_CFG_PTP_TIME_SEC_Mf, &sec_m)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_SEC2r, RTL8371C_CFG_PTP_TIME_SEC_Hf, &sec_h)) != RT_ERR_OK)
        return retVal;

    regVal = PTP_TIME_WRITE;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_CTRLr, RTL8371C_PTP_TIME_CMDf, &regVal)) != RT_ERR_OK)
        return retVal;
    regVal = RTKSW_ENABLED;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_NSEC1r, RTL8371C_CFG_PTP_TIME_VALIDf, &regVal)) != RT_ERR_OK)
        return retVal;
   
    if (apply != RTKSW_DISABLED)
    {
        regVal = RTKSW_ENABLED;
        if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_CTRLr, RTL8371C_PTP_TIME_EXECf, &regVal)) != RT_ERR_OK)
            return retVal;
    
        count = 0;
        do {
            if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_CTRLr, RTL8371C_PTP_TIME_EXECf, &busyFlag)) != RT_ERR_OK)
                return retVal;
            count++;
        } while ((busyFlag != 0)&&(count<5));

        if (busyFlag != 0)
        return RT_ERR_BUSYWAIT_TIMEOUT;
    } 
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_systemRefTime_get
 * Description:
 *      Get the reference time of the specified device by software.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pTimeStamp - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8371
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_systemRefTime_get(rtksw_uint32 unit, rtksw_time_timeStamp_t *pTimeStamp)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 sec_h, sec_m,sec_l;
    rtksw_uint32 nsec8_h, nsec8_l;
    rtksw_uint32  busyFlag, count;
    rtksw_uint32  regVal;    
    
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);
    
    if(pTimeStamp == NULL)
       return RT_ERR_NULL_POINTER;

    regVal = PTP_TIME_READ;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_CTRLr, RTL8371C_PTP_TIME_CMDf, &regVal)) != RT_ERR_OK)
        return retVal;
    regVal = RTKSW_ENABLED;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_CTRLr, RTL8371C_PTP_TIME_EXECf, &regVal)) != RT_ERR_OK)
        return retVal;

    count = 0;
    do {
        if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_CTRLr, RTL8371C_PTP_TIME_EXECf, &busyFlag)) != RT_ERR_OK)
            return retVal;
        count++;
    } while ((busyFlag != 0)&&(count<5));

    if (busyFlag != 0)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_SEC_RD0r, RTL8371C_RD_PTP_TIME_SEC_Lf, &sec_l)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_SEC_RD1r, RTL8371C_RD_PTP_TIME_SEC_Mf, &sec_m)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_SEC_RD2r, RTL8371C_RD_PTP_TIME_SEC_Hf, &sec_h)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_NSEC_RD0r, RTL8371C_RD_PTP_TIME_NSEC_Lf, &nsec8_l)) != RT_ERR_OK)
        return retVal;;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_NSEC_RD1r, RTL8371C_RD_PTP_TIME_NSEC_Hf, &nsec8_h)) != RT_ERR_OK)
        return retVal;

    pTimeStamp->sec= ((rtksw_uint64)sec_h<<32) | ((rtksw_uint64)sec_m<<16) |(rtksw_uint64)sec_l;
    pTimeStamp->nsec= (nsec8_h<<16) | nsec8_l;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_systemRefTimeAdjust_set
 * Description:
 *      Adjust the reference time.
 * Input:
 *      unit        - Unit ID
 *      sign        - significant
 *      timeStamp   - reference timestamp value
 *      apply       - state of apply
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 */
 rtksw_api_ret_t dal_rtl8371c_ptp_systemRefTimeAdjust_set(rtksw_uint32 unit, rtksw_ptp_sys_adjust_t sign, rtksw_time_timeStamp_t timeStamp, rtksw_enable_t apply)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 sec_h, sec_m,sec_l, nsec8_h, nsec8_l,nsec; 
    rtksw_uint32  busyFlag, count;
    rtksw_uint64  sec;
    rtksw_uint32  regVal;
    
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (timeStamp.nsec > dal_rtl8371c_MAX_NUM_OF_NANO_SECOND)
    return RT_ERR_INPUT;

    if (sign >= RTKSW_PTP_SYS_ADJUST_END)
        return RT_ERR_INPUT;

     if (sign == RTKSW_PTP_SYS_ADJUST_PLUS)
    {
        sec = timeStamp.sec;
        nsec= timeStamp.nsec;
    }
    else
    {
        /* adjust Timer of PHY */
        sec = 0 - (timeStamp.sec + 1);
        nsec = 1000000000 - timeStamp.nsec;
    }

    sec_h = (rtksw_uint32)(sec >>32)& 0xFFFF;
    sec_m = (rtksw_uint32)(sec  >>16)& 0xFFFF;
    sec_l = sec  & 0xFFFF;
    nsec8_h = (nsec >>16) & 0xFFFF;
    nsec8_l = nsec &0xFFFF;

    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_SEC0r, RTL8371C_CFG_PTP_TIME_SEC_Lf, &sec_l)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_SEC1r, RTL8371C_CFG_PTP_TIME_SEC_Mf, &sec_m)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_SEC2r, RTL8371C_CFG_PTP_TIME_SEC_Hf, &sec_h)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_NSEC0r, RTL8371C_CFG_PTP_TIME_NSEC_Lf, &nsec8_l)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_NSEC1r, RTL8371C_CFG_PTP_TIME_NSEC_Hf, &nsec8_h)) != RT_ERR_OK)
        return retVal;

    if (apply != RTKSW_DISABLED)
    {
        regVal = PTP_TIME_ADJUST;
        if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_CTRLr, RTL8371C_PTP_TIME_CMDf, &regVal)) != RT_ERR_OK)
            return retVal;

        regVal = RTKSW_ENABLED;
        if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_CTRLr, RTL8371C_PTP_TIME_EXECf, &regVal)) != RT_ERR_OK)
            return retVal;    

        count = 0;
        do {
            if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_CTRLr, RTL8371C_PTP_TIME_EXECf, &busyFlag)) != RT_ERR_OK)
                return retVal;
            count++;
        } while ((busyFlag != 0)&&(count<5));

        if (busyFlag != 0)
        return RT_ERR_BUSYWAIT_TIMEOUT;
    }
     
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_txTimestampFifo_get
 * Description:
 *      Get the top entry from PTP Tx timstamp FIFO on the dedicated port from the specified device.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pTimeEntry  - pointer buffer of TIME timestamp entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_txTimestampFifo_get(rtksw_uint32 unit, rtksw_time_txTimeEntry_t *pTimeEntry)
{
    rtksw_api_ret_t   retVal = RT_ERR_OK;
    rtksw_uint32  reg_val = 0;
    rtksw_uint32  sec_l = 0;
    //rtksw_uint32  sec_m = 0;
    //rtksw_uint32  sec_h = 0;
    rtksw_uint32  count,busyFlag, seqId;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pTimeEntry)
       return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TX_TIMESTAMP_RD0r, RTL8371C_RD_TX_TIMESTAMP_VALIDf, &reg_val)) != RT_ERR_OK)
        return retVal;
    pTimeEntry->valid = (rtksw_uint8)reg_val;
	
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TX_TIMESTAMP_RD0r, RTL8371C_RD_PORT_IDf, &reg_val)) != RT_ERR_OK)
        return retVal;
    pTimeEntry->port = reg_val;

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TX_TIMESTAMP_RD0r, RTL8371C_RD_MSG_TYPEf, &reg_val)) != RT_ERR_OK)
        return retVal;
    pTimeEntry->msg_type = reg_val;
	
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TX_TIMESTAMP_RD0r, RTL8371C_RD_SEQ_ID_Hf, &reg_val)) != RT_ERR_OK)
        return retVal;
    seqId = reg_val<<10;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TX_TIMESTAMP_RD1r, RTL8371C_RD_SEQ_ID_Lf, &reg_val)) != RT_ERR_OK)
        return retVal;
    seqId = seqId|reg_val;
    pTimeEntry->seqId = seqId;

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TX_TIMESTAMP_RD1r, RTL8371C_RD_TX_TIMESTAMP_SEC_Hf, &reg_val)) != RT_ERR_OK)
        return retVal;
    sec_l = reg_val<<2;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TX_TIMESTAMP_RD2r, RTL8371C_RD_TX_TIMESTAMP_SEC_Lf, &reg_val)) != RT_ERR_OK)
        return retVal;
    sec_l = sec_l|(reg_val&0x3);
    pTimeEntry->txTime.sec = (rtksw_uint64)(sec_l); 
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TX_TIMESTAMP_RD2r, RTL8371C_RD_TX_TIMESTAMP_NSEC_Hf, &reg_val)) != RT_ERR_OK)
        return retVal;
    pTimeEntry->txTime.nsec = reg_val << 16;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TX_TIMESTAMP_RD3r, RTL8371C_RD_TX_TIMESTAMP_NSEC_Lf, &reg_val)) != RT_ERR_OK)
        return retVal;
    pTimeEntry->txTime.nsec |= (reg_val&0xFFFF);

    reg_val = PTP_TIME_READ;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_CTRLr, RTL8371C_PTP_TIME_CMDf, &reg_val)) != RT_ERR_OK)
        return retVal;
    reg_val = RTKSW_ENABLED;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_CTRLr, RTL8371C_PTP_TIME_EXECf, &reg_val)) != RT_ERR_OK)
        return retVal;
     
    count = 0;
    do {
        if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_CTRLr, RTL8371C_PTP_TIME_EXECf, &busyFlag)) != RT_ERR_OK)
            return retVal;
        count++;
    } while ((busyFlag != 0)&&(count<5));

    if (busyFlag != 0)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_1PPSOutput_set
 * Description:
 *      Set 1 PPS output configuration of the specified port.
 * Input:
 *      unit        - Unit ID
 *      pulseWidth  - pointer to 1 PPS pulse width, unit: 10 ms
 *      enable      - enable 1 PPS output
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_1PPSOutput_set(rtksw_uint32 unit, rtksw_uint32 pulseWidth, rtksw_enable_t enable)
{
    rtksw_api_ret_t   retVal = RT_ERR_OK;
    rtksw_uint32 reg_val;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (pulseWidth > RTL8371C_MAX_PPS_WIDTH)
    {
        return RT_ERR_OUT_OF_RANGE;
    }
    
    reg_val = pulseWidth/10;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_PPS_CTRLr, RTL8371C_CFG_PPS_WIDTHf, &reg_val)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_PPS_CTRLr, RTL8371C_CFG_PPS_ENf, &enable)) != RT_ERR_OK)
        return retVal;
 
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_1PPSOutput_get
 * Description:
 *      Get 1 PPS output configuration of the specified port.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pPulseWidth - pointer to 1 PPS pulse width, unit: 10 ms
 *      pEnable     - pointer to 1 PPS output enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_1PPSOutput_get(rtksw_uint32 unit, rtksw_uint32 *pPulseWidth, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t   retVal = RT_ERR_OK;
    rtksw_uint32 reg_val;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_PPS_CTRLr, RTL8371C_CFG_PPS_WIDTHf, &reg_val)) != RT_ERR_OK)
    {
        return retVal;
    }
    *pPulseWidth = reg_val*10;    

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_PPS_CTRLr, RTL8371C_CFG_PPS_ENf, pEnable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_clockOutput_set
 * Description:
 *      Set clock output configuration.
 * Input:
 *      unit        - Unit ID
 *      pClkOutput  - pointer to clock output configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_clockOutput_set(rtksw_uint32 unit, rtksw_time_clkOutput_t *pClkOutput)
{
    rtksw_api_ret_t   retVal = RT_ERR_OK;
    rtksw_uint32  reg_val = 0;
    rtksw_uint32  sec_l = 0;
    rtksw_uint32  sec_m = 0;
    rtksw_uint32  sec_h = 0;
    rtksw_uint32  nsec_l = 0;
    rtksw_uint32  nsec_h = 0;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pClkOutput)
       return RT_ERR_NULL_POINTER;
    
    /* adjust Timer of PHY */
    sec_l = (pClkOutput->startTime.sec) & 0xFFFF;
    sec_m = ((pClkOutput->startTime.sec) >> 16) & 0xFFFF;
    sec_h = ((pClkOutput->startTime.sec) >> 32) & 0xFFFF;
    /* convert nsec to 8nsec */
    nsec_l = pClkOutput->startTime.nsec & 0xFFFF;
    nsec_h = pClkOutput->startTime.nsec >> 16;

    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_CLKOUT_SEC0r, RTL8371C_CFG_CLKOUT_TRIG_SEC_Lf, &sec_l)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_CLKOUT_SEC1r, RTL8371C_CFG_CLKOUT_TRIG_SEC_Mf, &sec_m)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_CLKOUT_SEC2r, RTL8371C_CFG_CLKOUT_TRIG_SEC_Hf, &sec_h)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_CLKOUT_NSEC0r, RTL8371C_CFG_CLKOUT_TRIG_NSEC_Lf, &nsec_l)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_CLKOUT_NSEC1r, RTL8371C_CFG_CLKOUT_TRIG_NSEC_Hf, &nsec_h)) != RT_ERR_OK)
        return retVal;

    nsec_l = pClkOutput->halfPeriodNsec & 0xFFFF;
    nsec_h = pClkOutput->halfPeriodNsec >> 16;

    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_CLKOUT_HALF_PERD_NS_Lr, RTL8371C_CFG_CLKOUT_HALF_PERIOD_NS_Lf, &nsec_l)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_CLKOUT_HALF_PERD_NS_Hr, RTL8371C_CFG_CLKOUT_HALF_PERIOD_NS_Hf, &nsec_h)) != RT_ERR_OK)
        return retVal;

    reg_val = pClkOutput->mode;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_CLKOUT_CTRLr, RTL8371C_CFG_PULSE_MODEf, &reg_val)) != RT_ERR_OK)
        return retVal;
    reg_val = pClkOutput->enable;    
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_CLKOUT_CTRLr, RTL8371C_CFG_CLKOUT_ENf, &reg_val)) != RT_ERR_OK)
        return retVal;
 
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_ptp_clockOutput_get
 * Description:
 *      Get clock output configuration.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pClkOutput  - pointer to clock output configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_clockOutput_get(rtksw_uint32 unit, rtksw_time_clkOutput_t *pClkOutput)
{
    rtksw_api_ret_t retVal = RT_ERR_OK;
    rtksw_uint32  reg_val = 0;
    rtksw_uint32  sec_l = 0;
    rtksw_uint32  sec_m = 0;
    rtksw_uint32  sec_h = 0;
    rtksw_uint32  nsec_l = 0;
    rtksw_uint32  nsec_h = 0;
    
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pClkOutput)
       return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_CLKOUT_SEC0r, RTL8371C_CFG_CLKOUT_TRIG_SEC_Lf, &sec_l)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_CLKOUT_SEC1r, RTL8371C_CFG_CLKOUT_TRIG_SEC_Mf, &sec_m)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_CLKOUT_SEC2r, RTL8371C_CFG_CLKOUT_TRIG_SEC_Hf, &sec_h)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_CLKOUT_NSEC0r, RTL8371C_CFG_CLKOUT_TRIG_NSEC_Lf, &nsec_l)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_CLKOUT_NSEC1r, RTL8371C_CFG_CLKOUT_TRIG_NSEC_Hf, &nsec_h)) != RT_ERR_OK)
        return retVal;

    pClkOutput->startTime.sec = ((rtksw_uint64)sec_h << 32) | ((rtksw_uint64)sec_m << 16) | ((rtksw_uint64)sec_l & 0xFFFF);
    pClkOutput->startTime.nsec = (((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF));


    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_CLKOUT_CTRLr, RTL8371C_CFG_PULSE_MODEf, &reg_val)) != RT_ERR_OK)
        return retVal;
    pClkOutput->mode = reg_val;
    
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_CLKOUT_CTRLr, RTL8371C_CFG_CLKOUT_ENf, &reg_val)) != RT_ERR_OK)
        return retVal;
    pClkOutput->enable = reg_val;

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_CLKOUT_CTRLr, RTL8371C_RD_CLKOUT_RUNf, &reg_val)) != RT_ERR_OK)
        return retVal;
    pClkOutput->runing = reg_val;
    
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_CLKOUT_HALF_PERD_NS_Lr, RTL8371C_CFG_CLKOUT_HALF_PERIOD_NS_Lf, &nsec_l)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_CLKOUT_HALF_PERD_NS_Hr, RTL8371C_CFG_CLKOUT_HALF_PERIOD_NS_Hf, &nsec_h)) != RT_ERR_OK)
        return retVal;
    pClkOutput->halfPeriodNsec = (((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF));

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_portCtrl_set
 * Description:
 *      Set PTP control of the specified port.
 * Input:
 *      unit        - Unit ID
 *      port        - port id
 *      pPortcfg    - port role/udp_en/eth_en/always_ts
 * Output:
 *      
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_portCtrl_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_port_ctrl_t *pPortcfg)
{
    rtksw_api_ret_t   retVal = RT_ERR_OK;
    rtksw_uint32 link_delay_l,link_delay_h; 
    rtksw_uint32 phyPort, reg_val;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pPortcfg)
       return RT_ERR_NULL_POINTER;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
    {
        return RT_ERR_PORT_ID;
    }
    
    reg_val = pPortcfg->portrole;
    if ((retVal = reg16_array_field_write(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_PORT_ROLEf, &reg_val)) != RT_ERR_OK)
        return retVal;
    reg_val = pPortcfg->utp_en;
    if ((retVal = reg16_array_field_write(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_UDP_ENf, &reg_val)) != RT_ERR_OK)
        return retVal;
    reg_val = pPortcfg->eth_en;
    if ((retVal = reg16_array_field_write(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_ETH_ENf, &reg_val)) != RT_ERR_OK)
        return retVal;
    reg_val = pPortcfg->always_ts_en;
    if ((retVal = reg16_array_field_write(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_ALWAYS_TSf, &reg_val)) != RT_ERR_OK)
        return retVal;

    link_delay_l= pPortcfg->link_delay & 0x3FF;
    link_delay_h = (pPortcfg->link_delay>>10) & 0xFFFF;    
    if ((retVal = reg16_array_field_write(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_LINK_DELAY_Lf, &link_delay_l)) != RT_ERR_OK)
        return retVal;    
    if ((retVal = reg16_array_field_write(unit, RTL8371C_PTPLINK_DELAY_Hr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_LINK_DELAY_Hf, &link_delay_h)) != RT_ERR_OK)
        return retVal;
	
    return retVal;
}


/* Function Name:
 *      dal_rtl8371c_ptp_TransEnable_get
 * Description:
 *      Get PTP control of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 * Output:
 *      pPortcfg   - port role/udp_en/eth_en/always_ts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_portCtrl_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_port_ctrl_t *pPortcfg)
{
    rtksw_api_ret_t   retVal = RT_ERR_OK;
    rtksw_uint32 link_delay_l=0;
    rtksw_uint32 link_delay_h=0;
    rtksw_uint32 phyPort, reg_val;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pPortcfg)
       return RT_ERR_NULL_POINTER;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
    {
        return RT_ERR_PORT_ID;
    }

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_PORT_ROLEf, &reg_val)) != RT_ERR_OK)
        return retVal;
    pPortcfg->portrole = reg_val;
    if ((retVal = reg16_array_field_read(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_UDP_ENf, &reg_val)) != RT_ERR_OK)
        return retVal;
    pPortcfg->utp_en = reg_val;    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_ETH_ENf, &reg_val)) != RT_ERR_OK)
        return retVal;
    pPortcfg->eth_en = reg_val;    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_ALWAYS_TSf, &reg_val)) != RT_ERR_OK)
        return retVal;
    pPortcfg->always_ts_en = reg_val;    

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PTP_PORT_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_LINK_DELAY_Lf, &link_delay_l)) != RT_ERR_OK)
        return retVal;    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_PTPLINK_DELAY_Hr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_CFG_LINK_DELAY_Hf, &link_delay_h)) != RT_ERR_OK)
        return retVal;
    pPortcfg->link_delay = (link_delay_l&0x3FF)|((link_delay_h&0xFFFF)<<10);

    return retVal;
}

/* Function Name:
 *      dal_rtl8371c_ptp_todDelay_set
 * Description:
 *      Set toddelay.
 * Input:
 *      unit        - Unit ID
 *      toddelay    - toddelay
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_todDelay_set(rtksw_uint32 unit, rtksw_uint32 toddelay)
{
    rtksw_api_ret_t   retVal ;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (toddelay > RTL8371C_MAX_PPS_WIDTH)
        return RT_ERR_OUT_OF_RANGE;

    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_TOD_DELAYr, RTL8371C_PTP_TIME_TOD_DELAYf, &toddelay)) != RT_ERR_OK)
        return retVal;  
    
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_todDelay_get
 * Description:
 *      Set toddelay.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      ptoddelay - toddelay
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_todDelay_get(rtksw_uint32 unit, rtksw_uint32 *ptoddelay)
{
    rtksw_api_ret_t   retVal ;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_TOD_DELAYr, RTL8371C_PTP_TIME_TOD_DELAYf, ptoddelay)) != RT_ERR_OK)
        return retVal;  
    
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_PPSLatchTime_get
 * Description:
 *      Set toddelay.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pLatchTime  - latch time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      mac mode only
 */
rtksw_api_ret_t dal_rtl8371c_ptp_PPSLatchTime_get(rtksw_uint32 unit, rtksw_time_timeStamp_t *pLatchTime)
{
    rtksw_api_ret_t   retVal = RT_ERR_OK;
    rtksw_uint32  sec_l = 0;
    rtksw_uint32  sec_m = 0;
    rtksw_uint32  sec_h = 0;
    rtksw_uint32  nsec_l = 0;
    rtksw_uint32  nsec_h = 0;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((retVal = reg16_field_read(unit, RTL8371C_PPS_IN_LATCH_TIME_SEC_Hr, RTL8371C_PPS_LATCH_PTP_TIME_SEC_Hf, &sec_h)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PPS_IN_LATCH_TIME_SEC_Mr, RTL8371C_PPS_LATCH_PTP_TIME_SEC_Mf, &sec_m)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PPS_IN_LATCH_TIME_SEC_Lr, RTL8371C_PPS_LATCH_PTP_TIME_SEC_Lf, &sec_l)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PPS_IN_LATCH_TIME_NSEC_Hr, RTL8371C_PPS_LATCH_PTP_TIME_NSEC_Hf, &nsec_h)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_PPS_IN_LATCH_TIME_NSEC_Lr, RTL8371C_PPS_LATCH_PTP_TIME_NSEC_Lf, &nsec_l)) != RT_ERR_OK)
        return retVal;

    pLatchTime->sec = ((rtksw_uint64)sec_h << 32) | ((rtksw_uint64)sec_m << 16) | ((rtksw_uint64)sec_l & 0xFFFF);
    pLatchTime->nsec = (((nsec_h & 0x3FFF) << 16) | (nsec_l & 0xFFFF));
 
    return RT_ERR_OK;
}
/* Function Name:
 *      dal_rtl8371c_ptp_refTimeFreqCfg_set
 * Description:
 *      Set the frequency of reference time of PHY of the specified port.
 * Input:
 *      unit    - Unit ID
 *      freq    - reference time frequency
 *      apply   - state of apply
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x10000000.
 *      If it is configured to 0x8000000, the tick frequency would be half of default.
 *      If it is configured to 0x20000000, the tick frequency would be one and half times of default.
 */
rtksw_api_ret_t dal_rtl8371c_ptp_refTimeFreqCfg_set(rtksw_uint32 unit, rtksw_uint32 freq, rtksw_enable_t apply)
{
    rtksw_api_ret_t   retVal;
    rtksw_uint32 reg_val;
    rtksw_uint32  busyFlag, count;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    reg_val = freq&0xFFFF;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_FREQ0r, RTL8371C_TIME_FREQ_OPER0f, &reg_val)) != RT_ERR_OK)
        return retVal;
    reg_val = freq>>16;
    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_TIME_FREQ1r, RTL8371C_TIME_FREQ_OPER1f, &reg_val)) != RT_ERR_OK)
        return retVal;    

    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_APPLY_FREQr, RTL8371C_APPLY_FREQf, &apply)) != RT_ERR_OK)
        return retVal;

    count = 0;
    do {
        if ((retVal = reg16_field_read(unit, RTL8371C_PTP_APPLY_FREQr, RTL8371C_APPLY_FREQf, &busyFlag)) != RT_ERR_OK)
            return retVal;
        count++;
    } while ((busyFlag != 0)&&(count<5));

    if (busyFlag != 0)
        return RT_ERR_BUSYWAIT_TIMEOUT;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_refTimeFreqCfg_get
 * Description:
 *      Set  ptp_RefTimeFreqCfg_get.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      cfgFreq
 *      curFreq
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 */
rtksw_api_ret_t dal_rtl8371c_ptp_refTimeFreqCfg_get(rtksw_uint32 unit, rtksw_uint32 *cfgFreq, rtksw_uint32 *curFreq)
{
    rtksw_api_ret_t   retVal;
    rtksw_uint32 freqtmp;
    rtksw_uint32 reg_val;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_FREQ0r, RTL8371C_TIME_FREQ_OPER0f, &reg_val)) != RT_ERR_OK)
        return retVal;
    freqtmp = reg_val&0xFFFF;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_TIME_FREQ1r, RTL8371C_TIME_FREQ_OPER1f, &reg_val)) != RT_ERR_OK)
        return retVal;    
    freqtmp = freqtmp|((reg_val&0xFFFF)<<16);
    *cfgFreq = freqtmp;

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_CUR_TIME_FREQ0r, RTL8371C_CUR_TIME_FREQ_OPER0f, &reg_val)) != RT_ERR_OK)
        return retVal;
    freqtmp = reg_val&0xFFFF;
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_CUR_TIME_FREQ1r, RTL8371C_CUR_TIME_FREQ_OPER1f, &reg_val)) != RT_ERR_OK)
    return retVal;  
    freqtmp = freqtmp|((reg_val&0xFFFF)<<16);
    *curFreq = freqtmp;
      
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_intControl_set
 * Description:
 *      Set PTP interrupt trigger status configuration.
 * Input:
 *      unit    - Unit ID
 *      type    - Interrupt type.
 *      enable  - Interrupt status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      The API can set PTP interrupt status configuration.
 *      The interrupt trigger status is shown in the following:
 *          PTP_INT_TYPE_1PPS = 0,
 *          PTP_INT_TYPE_TOD_DONE,
 *          PTP_INT_TYPE_TXTIME_EMPTY,
 */
rtksw_api_ret_t dal_rtl8371c_ptp_intControl_set(rtksw_uint32 unit, rtksw_ptp_intrType_t type, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= RTKSW_PTP_INTR_TYPE_END)
        return RT_ERR_INPUT;

    switch(type)
    {
        case RTKSW_PTP_INTR_TYPE_1PPS:
            if ((retVal = reg16_field_write(unit, RTL8371C_PTP_MIB_INTRr, RTL8371C_CFG_IMR_PPS_If, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PTP_INTR_TYPE_TOD_DONE:
            if ((retVal = reg16_field_write(unit, RTL8371C_TOD_INTRr, RTL8371C_IMR_TODf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PTP_INTR_TYPE_TXTIME_EMPTY:
            if ((retVal = reg16_field_write(unit, RTL8371C_PTP_MIB_INTRr, RTL8371C_CFG_IMR_PTPf, &enable)) != RT_ERR_OK)
                return retVal;    
            break;
        case RTKSW_PTP_INTR_TYPE_ALL:
            if ((retVal = reg16_field_write(unit, RTL8371C_PTP_MIB_INTRr, RTL8371C_CFG_IMR_PPS_If, &enable)) != RT_ERR_OK)
                return retVal;
            if ((retVal = reg16_field_write(unit, RTL8371C_TOD_INTRr, RTL8371C_IMR_TODf, &enable)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_write(unit, RTL8371C_PTP_MIB_INTRr, RTL8371C_CFG_IMR_PTPf, &enable)) != RT_ERR_OK)
                return retVal;    
            break;            
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_intControl_get
 * Description:
 *      Get PTP interrupt trigger status configuration.
 * Input:
 *      unit    - Unit ID
 *      type    - Interrupt type.
 * Output:
 *      pEnable - Interrupt status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get interrupt status configuration.
 *      The interrupt trigger status is shown in the following:
 *          PTP_INT_TYPE_1PPS = 0,
 *          PTP_INT_TYPE_TOD_DONE,
 *          PTP_INT_TYPE_TXTIME_EMPTY,
 */
 rtksw_api_ret_t dal_rtl8371c_ptp_intControl_get(rtksw_uint32 unit, rtksw_ptp_intrType_t type, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    switch(type)
    {
        case RTKSW_PTP_INTR_TYPE_1PPS:
            if ((retVal = reg16_field_read(unit, RTL8371C_PTP_MIB_INTRr, RTL8371C_CFG_IMR_PPS_If, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PTP_INTR_TYPE_TOD_DONE:
            if ((retVal = reg16_field_read(unit, RTL8371C_TOD_INTRr, RTL8371C_IMR_TODf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case RTKSW_PTP_INTR_TYPE_TXTIME_EMPTY:
            if ((retVal = reg16_field_read(unit, RTL8371C_PTP_MIB_INTRr, RTL8371C_CFG_IMR_PTPf, pEnable)) != RT_ERR_OK)
                return retVal;    
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_intStatus_set
 * Description:
 *      Set PTP interrupt trigger status configuration.
 * Input:
 *      unit        - Unit ID
 *      statusMask  - status mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      The API can set PTP interrupt status configuration. It is used to clean interrupt status.
 *      The interrupt trigger status is shown in the following:
 *          PTP_INT_TYPE_1PPS          (Bit0)
 *          PTP_INT_TYPE_TOD_DONE      (Bit1)
 *          PTP_INT_TYPE_TXTIME_EMPTY  (Bit2)
 */
rtksw_api_ret_t dal_rtl8371c_ptp_intStatus_set(rtksw_uint32 unit, rtksw_ptp_intStatus_t statusMask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regVal=1;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (statusMask&(1<<RTKSW_PTP_INTR_TYPE_1PPS))
    {
        if ((retVal = reg16_field_write(unit, RTL8371C_PTP_MIB_INTRr, RTL8371C_RD_ISR_PPS_If, &regVal)) != RT_ERR_OK)
            return retVal;
    }

    if (statusMask&(1<<RTKSW_PTP_INTR_TYPE_TOD_DONE))
    {
        if ((retVal = reg16_field_write(unit, RTL8371C_TOD_INTRr, RTL8371C_ISR_TODf, &regVal)) != RT_ERR_OK)
            return retVal;
    }

    if (statusMask&(1<<RTKSW_PTP_INTR_TYPE_TXTIME_EMPTY))
    {
        if ((retVal = reg16_field_write(unit, RTL8371C_PTP_MIB_INTRr, RTL8371C_RD_ISR_PTPf, &regVal)) != RT_ERR_OK)
            return retVal;
    }
    
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_intStatus_get
 * Description:
 *      Get PTP interrupt trigger status configuration.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pStatusMask - status mask.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get interrupt status configuration.
 *      The interrupt trigger status is shown in the following:
 *          PTP_INT_TYPE_1PPS          (Bit0)
 *          PTP_INT_TYPE_TOD_DONE      (Bit1)
 *          PTP_INT_TYPE_TXTIME_EMPTY  (Bit2)
 */
 rtksw_api_ret_t dal_rtl8371c_ptp_intStatus_get(rtksw_uint32 unit, rtksw_ptp_intStatus_t *pStatusMask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 mask=0;
    rtksw_uint32 regVal=0;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (pStatusMask==NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_MIB_INTRr, RTL8371C_RD_ISR_PPS_If, &regVal)) != RT_ERR_OK)
        return retVal;
    mask |= (regVal&0x1)<<RTKSW_PTP_INTR_TYPE_1PPS; 
    if ((retVal = reg16_field_read(unit, RTL8371C_TOD_INTRr, RTL8371C_ISR_TODf, &regVal)) != RT_ERR_OK)
        return retVal;
    mask |= (regVal&0x1)<<RTKSW_PTP_INTR_TYPE_TOD_DONE; 
    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_MIB_INTRr, RTL8371C_RD_ISR_PTPf, &regVal)) != RT_ERR_OK)
        return retVal;
    mask |= (regVal&0x1)<<RTKSW_PTP_INTR_TYPE_TXTIME_EMPTY; 

    *pStatusMask = mask;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_portPktTrap_set
 * Description:
 *      Set PTP packet trap of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      type    - PTP message type
 *      enable  - enable PTP action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
 rtksw_api_ret_t dal_rtl8371c_ptp_portPktTrap_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_pktType_t type, rtksw_enable_t enable)
{
    rtksw_api_ret_t   retVal;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (type>=RTKSW_RTK_PTP_PKT_TYPE_END)
        return RT_ERR_INPUT;

    if (enable>=RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
    {
        return RT_ERR_PORT_ID;
    }

    switch (type)
    {
        case RTKSW_PTP_PKT_TYPE_DELAY_CARE:            
            if ((retVal = reg16_array_field_write(unit, RTL8371C_RMA_PORT_PTP_DELAY_CARE_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_PTP_DELAY_CAREf, &enable)) != RT_ERR_OK)
                return retVal; 
            break;
        case RTKSW_PTP_PKT_TYPE_PDELAY_CARE:
            if ((retVal = reg16_array_field_write(unit, RTL8371C_RMA_PORT_PTP_PDELAY_CARE_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_PTP_PDELAY_CAREf, &enable)) != RT_ERR_OK)
                return retVal;
            break; 
        case RTKSW_PTP_PKT_TYPE_ASM_CARE:            
            if ((retVal = reg16_array_field_write(unit, RTL8371C_RMA_PORT_PTP_ASM_CARE_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_PTP_ASM_CAREf, &enable)) != RT_ERR_OK)
                return retVal; 
            break;			
        default:
            break;
    }
  
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_portPktTrap_get
 * Description:
 *      Get PTP packet trap of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      type    - PTP message type
 * Output:
 *      *pEnable - enable PTP action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_portPktTrap_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_pktType_t type, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (type>=RTKSW_RTK_PTP_PKT_TYPE_END)
        return RT_ERR_INPUT;

    if (pEnable==NULL)
        return RT_ERR_NULL_POINTER;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
    {
        return RT_ERR_PORT_ID;
    }

    switch (type)
    {
        case RTKSW_PTP_PKT_TYPE_DELAY_CARE:            
            if ((retVal = reg16_array_field_read(unit, RTL8371C_RMA_PORT_PTP_DELAY_CARE_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_PTP_DELAY_CAREf, pEnable)) != RT_ERR_OK)
                return retVal; 
            break;
        case RTKSW_PTP_PKT_TYPE_PDELAY_CARE:
            if ((retVal = reg16_array_field_read(unit, RTL8371C_RMA_PORT_PTP_PDELAY_CARE_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_PTP_PDELAY_CAREf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;  
        case RTKSW_PTP_PKT_TYPE_ASM_CARE:            
            if ((retVal = reg16_array_field_read(unit, RTL8371C_RMA_PORT_PTP_ASM_CARE_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_PTP_ASM_CAREf, pEnable)) != RT_ERR_OK)
                return retVal; 
            break;			
        default:
            break;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_ptp_portPktAction_set
 * Description:
 *      Set PTP packet trap of the specified port.
 * Input:
 *      unit        - Unit ID
 *      port        - port id
 *      type        - Eth or UDP
 *      action      - packet action
 *      priority    - trap packet priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
 rtksw_api_ret_t dal_rtl8371c_ptp_portPktAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type, rtksw_trapTarget_t action, rtksw_uint32 priority)
{
    rtksw_api_ret_t   retVal;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (type>=RTKSW_PTP_END)
        return RT_ERR_INPUT;

    if (action>=RTKSW_TRAP_END)
        return RT_ERR_INPUT;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
    {
        return RT_ERR_PORT_ID;
    }

    switch (type)
    {
        case RTKSW_PTP_ETH:            
            if ((retVal = reg16_array_field_write(unit, RTL8371C_RMA_PORT_PTP_ETH2_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTIONf, &action)) != RT_ERR_OK)
                return retVal; 
            break;
        case RTKSW_PTP_UDP:
            if ((retVal = reg16_array_field_write(unit, RTL8371C_RMA_PORT_PTP_UDP_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTIONf, &action)) != RT_ERR_OK)
                return retVal;
            break;          
        default:
            break;
    }

    if ((retVal = reg16_field_write(unit, RTL8371C_RMA_PTP_TRAP_CTRLr, RTL8371C_PRIf, &priority)) != RT_ERR_OK)
        return retVal; 

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_portPktAction_get
 * Description:
 *      Get PTP packet trap of the specified port.
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      type    - Eth or UDP
 * Output:
 *      *pAction   - packet action
 *      *pPriority - trap packet priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT         - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_ptp_portPktAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_ptp_header_t type, rtksw_trapTarget_t *pAction, rtksw_uint32 *pPriority)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (type>=RTKSW_PTP_END)
        return RT_ERR_INPUT;

    if (pAction==NULL)
        return RT_ERR_NULL_POINTER;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
    {
        return RT_ERR_PORT_ID;
    }

    switch (type)
    {
        case RTKSW_PTP_ETH:            
            if ((retVal = reg16_array_field_read(unit, RTL8371C_RMA_PORT_PTP_ETH2_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTIONf, pAction)) != RT_ERR_OK)
                return retVal; 
            break;
        case RTKSW_PTP_UDP:
            if ((retVal = reg16_array_field_read(unit, RTL8371C_RMA_PORT_PTP_UDP_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTIONf, pAction)) != RT_ERR_OK)
                return retVal;
            break;          
        default:
            break;
    }

    if ((retVal = reg16_field_read(unit, RTL8371C_RMA_PTP_TRAP_CTRLr, RTL8371C_PRIf, pPriority)) != RT_ERR_OK)
        return retVal; 

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_clkSrcCtrl_set
 * Description:
 *      Set PTP time Clock source selection
 * Input:
 *      unit    - Unit ID
 *      clksrc  - clock source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK   - Success
 *      RT_ERR_SMI  - SMI access error
 * Note:
 * 0: internal clock (Internal PLL, 1GMHz)
 * 1: external clock, refer to cfg_ext_clk_src)
 */
rtksw_api_ret_t dal_rtl8371c_ptp_clkSrcCtrl_set(rtksw_uint32 unit, rtksw_enable_t clksrc)
{
    rtksw_api_ret_t   retVal=RT_ERR_OK;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (clksrc>1)
        return RT_ERR_INPUT;

    if ((retVal = reg16_field_write(unit, RTL8371C_PTP_CLK_SRC_CTRLr, RTL8371C_CFG_CLK_SRCf, &clksrc)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_clkSrcCtrl_get
 * Description:
 *      Get PTP time Clock source selection
 * Input:
 *      unit    - Unit ID
 * Output:
 *      clksrc  - clock source
 * Return:
 *      RT_ERR_OK   - Success
 *      RT_ERR_SMI  - SMI access error
 * Note:
 * 0: internal clock (Internal PLL, 1GMHz)
 * 1: external clock, refer to cfg_ext_clk_src)
 */
rtksw_api_ret_t dal_rtl8371c_ptp_clkSrcCtrl_get(rtksw_uint32 unit, rtksw_enable_t *clksrc)
{
    rtksw_api_ret_t   retVal=RT_ERR_OK;
    
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((retVal = reg16_field_read(unit, RTL8371C_PTP_CLK_SRC_CTRLr, RTL8371C_CFG_CLK_SRCf, clksrc)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_ptp_timerCompen_set
 * Description:
 *      Set TX/RX timer value compensation..
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      TxImbal - TX timer value compensation
 *      RxImbal - RX timer value compensation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      unit: 1 ns
 */
rtksw_api_ret_t dal_rtl8371c_ptp_timerCompen_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 TxImbal, rtksw_uint32 RxImbal)
{
    rtksw_api_ret_t   retVal = RT_ERR_OK;
    rtksw_uint32 phyPort;
    
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
    {
        return RT_ERR_PORT_ID;
    }

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PTP_TX_IMBALr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_TX_IMBALf, &TxImbal)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PTP_RX_IMBALr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_RX_IMBALf, &RxImbal)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_ptp_timerCompen_get
 * Description:
 *      Get TX/RX timer value compensation..
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 * Output:
 *      pTxImbal  - TX timer value compensation
 *      pRxImbal  - RX timer value compensation

 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      unit: 1 ns
 */
rtksw_api_ret_t dal_rtl8371c_ptp_timerCompen_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 *pTxImbal, rtksw_uint32 *pRxImbal)
{
    rtksw_api_ret_t   retVal = RT_ERR_OK;
    rtksw_uint32 phyPort;
    
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if ((pTxImbal==NULL)||(pRxImbal==NULL))
        return RT_ERR_NULL_POINTER;    

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
    {
        return RT_ERR_PORT_ID;
    }

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PTP_TX_IMBALr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_TX_IMBALf, pTxImbal)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PTP_RX_IMBALr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_RX_IMBALf, pRxImbal)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}
