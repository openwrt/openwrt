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
#include <dal/rtl8371c/dal_rtl8371c_cpu.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      dal_rtl8371c_cpu_enable_set
 * Description:
 *      Set CPU port function enable/disable.
 * Input:
 *      unit    - Unit ID
 *      enable  - CPU port function enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can set CPU port function enable/disable.
 */
rtksw_api_ret_t dal_rtl8371c_cpu_enable_set(rtksw_uint32 unit, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
	rtksw_uint32 cpuEn;
	rtksw_uint32 portMask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    cpuEn = (enable == RTKSW_ENABLED) ? 1 : 0;

    if ((retVal = reg16_field_write(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_ENf, &cpuEn)) != RT_ERR_OK)
        return retVal;

    if (RTKSW_DISABLED == cpuEn)
    {
    	portMask = 0;
        if ((retVal = reg16_write(unit, RTL8371C_CPU_TAG_AWARE_CTRLr, &portMask)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_cpu_enable_get
 * Description:
 *      Get CPU port and its setting.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pEnable - CPU port function enable
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_L2_NO_CPU_PORT   - CPU port is not exist
 * Note:
 *      The API can get CPU port function enable/disable.
 */
rtksw_api_ret_t dal_rtl8371c_cpu_enable_get(rtksw_uint32 unit, rtksw_enable_t *pEnable)

{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_ENf, pEnable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_cpu_tagPort_set
 * Description:
 *      Set CPU port and CPU tag insert mode.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      mode    - CPU tag insert for packets egress from CPU port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can set CPU port and inserting proprietary CPU tag mode (Length/Type 0x8899)
 *      to the frame that transmitting to CPU port.
 *      The inset cpu tag mode is as following:
 *      - CPU_INSERT_TO_ALL
 *      - CPU_INSERT_TO_TRAPPING
 *      - CPU_INSERT_TO_NONE
 */
rtksw_api_ret_t dal_rtl8371c_cpu_tagPort_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_cpu_insert_t mode)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyPort, awarePort, trapPort;
    rtksw_uint32 insertMode;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (mode >= CPU_INSERT_END)
        return RT_ERR_INPUT;

    phyPort = rtksw_switch_port_L2P_get(unit, port);

    if (phyPort == UNDEFINE_PHY_PORT)
		return RT_ERR_PORT_ID;

	insertMode = mode;

	awarePort = 1 << phyPort;
    if ((retVal = reg16_write(unit, RTL8371C_CPU_TAG_AWARE_CTRLr, &awarePort)) != RT_ERR_OK)
		return retVal;

	if ((retVal = reg16_read(unit, RTL8371C_CPU_TAG_CTRLr, &regData)) != RT_ERR_OK)
		return retVal;

	trapPort = phyPort & 7;
    if ((retVal = reg16_field_set(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TRAP_PORTf, &trapPort, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_set(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_INSERTMODEf, &insertMode, &regData)) != RT_ERR_OK)
        return retVal;


    if ((retVal = reg16_write(unit, RTL8371C_CPU_TAG_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_cpu_tagPort_get
 * Description:
 *      Get CPU port and CPU tag insert mode.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pPort - Port id.
 *      pMode - CPU tag insert for packets egress from CPU port, 0:all insert 1:Only for trapped packets 2:no insert.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_L2_NO_CPU_PORT   - CPU port is not exist
 * Note:
 *      The API can get configured CPU port and its setting.
 *      The inset cpu tag mode is as following:
 *      - CPU_INSERT_TO_ALL
 *      - CPU_INSERT_TO_TRAPPING
 *      - CPU_INSERT_TO_NONE
 */
rtksw_api_ret_t dal_rtl8371c_cpu_tagPort_get(rtksw_uint32 unit, rtksw_port_t *pPort, rtksw_cpu_insert_t *pMode)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmsk, port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pPort)
        return RT_ERR_NULL_POINTER;

    if(NULL == pMode)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_read(unit, RTL8371C_CPU_TAG_AWARE_CTRLr, &pmsk)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_read(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TRAP_PORTf, &port)) != RT_ERR_OK)
        return retVal;

    *pPort = rtksw_switch_port_P2L_get(unit, port);

    if ((retVal = reg16_field_read(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_INSERTMODEf, pMode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_cpu_awarePort_set
 * Description:
 *      Set CPU aware port mask.
 * Input:
 *      unit        - Unit ID
 *      portmask    - Port mask.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_MASK      - Invalid port mask.
 * Note:
 *      The API can set configured CPU aware port mask.
 */
rtksw_api_ret_t dal_rtl8371c_cpu_awarePort_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyMbrPmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Valid port mask */
    if(NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    /* Check port mask valid */
    RTKSW_CHK_PORTMASK_VALID(unit, pPortmask);

    if(rtksw_switch_portmask_L2P_get(unit, pPortmask, &phyMbrPmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if ((retVal = reg16_write(unit, RTL8371C_CPU_TAG_AWARE_CTRLr, &phyMbrPmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_cpu_awarePort_get
 * Description:
 *      Get CPU aware port mask.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pPortmask - Port mask.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 * Note:
 *      The API can get configured CPU aware port mask.
 */
rtksw_api_ret_t dal_rtl8371c_cpu_awarePort_get(rtksw_uint32 unit, rtksw_portmask_t *pPortmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmsk;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_read(unit, RTL8371C_CPU_TAG_AWARE_CTRLr, &pmsk)) != RT_ERR_OK)
        return retVal;

    if(rtksw_switch_portmask_P2L_get(unit, pmsk, pPortmask) != RT_ERR_OK)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_cpu_tagPosition_set
 * Description:
 *      Set CPU tag position.
 * Input:
 *      unit        - Unit ID
 *      position    - CPU tag position.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can set CPU tag position.
 */
rtksw_api_ret_t dal_rtl8371c_cpu_tagPosition_set(rtksw_uint32 unit, rtksw_cpu_position_t position)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 cpuPos;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if (position >= CPU_POS_END)
		return RT_ERR_INPUT;

	cpuPos = position;
	if ((retVal = reg16_field_write(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TAG_POSITIONf, &cpuPos)) != RT_ERR_OK)
		return retVal;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_cpu_tagPosition_get
 * Description:
 *      Get CPU tag position.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pPosition - CPU tag position.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can get CPU tag position.
 */
rtksw_api_ret_t dal_rtl8371c_cpu_tagPosition_get(rtksw_uint32 unit, rtksw_cpu_position_t *pPosition)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 cpuPos;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if(NULL == pPosition)
		return RT_ERR_NULL_POINTER;

	if ((retVal = reg16_field_read(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TAG_POSITIONf, &cpuPos)) != RT_ERR_OK)
		return retVal;

	*pPosition = cpuPos;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_cpu_tagLength_set
 * Description:
 *      Set CPU tag length.
 * Input:
 *      unit    - Unit ID
 *      length  - CPU tag length.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can set CPU tag length.
 */
rtksw_api_ret_t dal_rtl8371c_cpu_tagLength_set(rtksw_uint32 unit, rtksw_cpu_tag_length_t length)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 format, formatPri;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((retVal = reg16_read(unit, RTL8371C_CPU_TAG_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    switch (length)
    {
        case CPU_LEN_8BYTES:
			format = 0;
			formatPri = 0;
            if ((retVal = reg16_field_set(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TAG_FORMATf, &format, &regData)) != RT_ERR_OK)
                return retVal;
            if ((retVal = reg16_field_set(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TAG_FORMAT_PRIf, &formatPri, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case CPU_LEN_4BYTES:
			format = 1;
			formatPri = 0;
            if ((retVal = reg16_field_set(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TAG_FORMATf,  &format, &regData)) != RT_ERR_OK)
                return retVal;
            if ((retVal = reg16_field_set(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TAG_FORMAT_PRIf, &formatPri, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case CPU_LEN_4BYTES_PRIORITY:
			format = 1;
			formatPri = 1;
            if ((retVal = reg16_field_set(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TAG_FORMATf,  &format, &regData)) != RT_ERR_OK)
                return retVal;
            if ((retVal = reg16_field_set(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TAG_FORMAT_PRIf, &formatPri, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((retVal = reg16_write(unit, RTL8371C_CPU_TAG_CTRLr, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_cpu_tagLength_get
 * Description:
 *      Get CPU tag length.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pLength - CPU tag length.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can get CPU tag length.
 */
rtksw_api_ret_t dal_rtl8371c_cpu_tagLength_get(rtksw_uint32 unit, rtksw_cpu_tag_length_t *pLength)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 regData2;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pLength)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TAG_FORMATf, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_read(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TAG_FORMAT_PRIf, &regData2)) != RT_ERR_OK)
        return retVal;

    if (regData == 0)
        *pLength = CPU_LEN_8BYTES;
    else
    {
        if (regData2 == 0)
            *pLength = CPU_LEN_4BYTES;
        else
            *pLength = CPU_LEN_4BYTES_PRIORITY;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_cpu_priRemap_set
 * Description:
 *      Configure CPU priorities mapping to internal absolute priority.
 * Input:
 *      unit        - Unit ID
 *      int_pri     - internal priority value.
 *      new_pri     - new internal priority value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_VLAN_PRIORITY    - Invalid 1p priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      Priority of CPU tag assignment for internal asic priority, and it is used for queue usage and packet scheduling.
 */
rtksw_api_ret_t dal_rtl8371c_cpu_priRemap_set(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_pri_t new_pri)
{
    rtksw_api_ret_t retVal;
	rtksw_uint32 newPriority;
	rtksw_uint32 reg;
    rtksw_uint32 field;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if (new_pri > RTL8371C_PRIMAX || int_pri > RTL8371C_PRIMAX)
		return  RT_ERR_VLAN_PRIORITY;

	newPriority = new_pri;

    if (int_pri == 0){
    	reg = RTL8371C_PRI_TO_CPU_W0r;
        field = RTL8371C_PRIORITY0f;
	}
    else if (int_pri == 1)
	{
    	reg = RTL8371C_PRI_TO_CPU_W0r;
        field = RTL8371C_PRIORITY1f;
	}
    else if (int_pri == 2)
	{
    	reg = RTL8371C_PRI_TO_CPU_W0r;
        field = RTL8371C_PRIORITY2f;
	}
    else if (int_pri == 3)
	{
    	reg = RTL8371C_PRI_TO_CPU_W0r;
        field = RTL8371C_PRIORITY3f;
	}
    else if (int_pri == 4)
	{
    	reg = RTL8371C_PRI_TO_CPU_W1r;
        field = RTL8371C_PRIORITY4f;
	}
    else if (int_pri == 5)
	{
    	reg = RTL8371C_PRI_TO_CPU_W1r;
        field = RTL8371C_PRIORITY5f;
	}
    else if (int_pri == 6)
	{
    	reg = RTL8371C_PRI_TO_CPU_W1r;
        field = RTL8371C_PRIORITY6f;
	}
    else if (int_pri == 7)
	{
		reg = RTL8371C_PRI_TO_CPU_W1r;
		field = RTL8371C_PRIORITY7f;
	}

    if ((retVal = reg16_field_write(unit, reg, field, &newPriority)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_cpu_priRemap_get
 * Description:
 *      Configure CPU priorities mapping to internal absolute priority.
 * Input:
 *      unit        - Unit ID
 *      int_pri     - internal priority value.
 * Output:
 *      pNew_pri    - new internal priority value.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_VLAN_PRIORITY    - Invalid 1p priority.
 *      RT_ERR_QOS_INT_PRIORITY - Invalid priority.
 * Note:
 *      Priority of CPU tag assignment for internal asic priority, and it is used for queue usage and packet scheduling.
 */
rtksw_api_ret_t dal_rtl8371c_cpu_priRemap_get(rtksw_uint32 unit, rtksw_pri_t int_pri, rtksw_pri_t *pNew_pri)
{
    rtksw_api_ret_t retVal;
	rtksw_uint32 reg;
    rtksw_uint32 field;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if(NULL == pNew_pri)
		return RT_ERR_NULL_POINTER;

	if (int_pri > RTL8371C_PRIMAX)
		return  RT_ERR_QOS_INT_PRIORITY;

	if (int_pri == 0){
    	reg = RTL8371C_PRI_TO_CPU_W0r;
        field = RTL8371C_PRIORITY0f;
	}
    else if (int_pri == 1)
	{
    	reg = RTL8371C_PRI_TO_CPU_W0r;
        field = RTL8371C_PRIORITY1f;
	}
    else if (int_pri == 2)
	{
    	reg = RTL8371C_PRI_TO_CPU_W0r;
        field = RTL8371C_PRIORITY2f;
	}
    else if (int_pri == 3)
	{
    	reg = RTL8371C_PRI_TO_CPU_W0r;
        field = RTL8371C_PRIORITY3f;
	}
    else if (int_pri == 4)
	{
    	reg = RTL8371C_PRI_TO_CPU_W1r;
        field = RTL8371C_PRIORITY4f;
	}
    else if (int_pri == 5)
	{
    	reg = RTL8371C_PRI_TO_CPU_W1r;
        field = RTL8371C_PRIORITY5f;
	}
    else if (int_pri == 6)
	{
    	reg = RTL8371C_PRI_TO_CPU_W1r;
        field = RTL8371C_PRIORITY6f;
	}
    else if (int_pri == 7)
	{
		reg = RTL8371C_PRI_TO_CPU_W1r;
		field = RTL8371C_PRIORITY7f;
	}

    if ((retVal = reg16_field_read(unit, reg, field, (rtksw_uint32 *)pNew_pri)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_cpu_acceptLength_set
 * Description:
 *      Set CPU accept  length.
 * Input:
 *      unit    - Unit ID
 *      length  - CPU tag length.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can set CPU accept length.
 */
rtksw_api_ret_t dal_rtl8371c_cpu_acceptLength_set(rtksw_uint32 unit, rtksw_cpu_rx_length_t length)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 acceptLen;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if (length >= CPU_RX_END)
		return RT_ERR_INPUT;

	acceptLen = length;
	if ((retVal = reg16_field_write(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TAG_RXBYTECOUNTf, &acceptLen)) != RT_ERR_OK)
		return retVal;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_cpu_acceptLength_get
 * Description:
 *      Get CPU accept length.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pLength - CPU tag length.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid input.
 * Note:
 *      The API can get CPU accept length.
 */
rtksw_api_ret_t dal_rtl8371c_cpu_acceptLength_get(rtksw_uint32 unit, rtksw_cpu_rx_length_t *pLength)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 acceptLen;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if(NULL == pLength)
		return RT_ERR_NULL_POINTER;

	if ((retVal = reg16_field_read(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TAG_RXBYTECOUNTf, &acceptLen)) != RT_ERR_OK)
		return retVal;

	*pLength = acceptLen;
	return RT_ERR_OK;
}

