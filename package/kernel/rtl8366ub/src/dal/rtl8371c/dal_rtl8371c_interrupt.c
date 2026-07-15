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
#include <dal/rtl8371c/dal_rtl8371c_interrupt.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>
#include <dal/reg.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      dal_rtl8371c_int_polarity_set
 * Description:
 *      Set interrupt polarity configuration.
 * Input:
 *      unit    - Unit ID
 *      type    - Interrupt polarity type.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can set interrupt polarity configuration.
 */
rtksw_api_ret_t dal_rtl8371c_int_polarity_set(rtksw_uint32 unit, rtksw_int_polarity_t type)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(type >= INT_POLAR_END)
        return RT_ERR_INPUT;

    if ((retVal = reg16_field_write(unit, RTL8371C_INTRPT_POLARITYr, RTL8371C_INTR_POLARITYf, (rtksw_uint32 *)&type)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_int_polarity_get
 * Description:
 *      Get interrupt polarity configuration.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pType - Interruptpolarity type.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can get interrupt polarity configuration.
 */
rtksw_api_ret_t dal_rtl8371c_int_polarity_get(rtksw_uint32 unit, rtksw_int_polarity_t *pType)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pType)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_INTRPT_POLARITYr, RTL8371C_INTR_POLARITYf, (rtksw_uint32 *)pType)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_int_control_set
 * Description:
 *      Set interrupt trigger status configuration.
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
 *      The API can set interrupt status configuration.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_LINK_STATUS
 *      - INT_TYPE_METER_EXCEED
 *      - INT_TYPE_LEARN_LIMIT
 *      - INT_TYPE_LINK_SPEED
 *      - INT_TYPE_CONGEST
 *      - INT_TYPE_GREEN_FEATURE
 *      - INT_TYPE_LOOP_DETECT
 *      - INT_TYPE_8051,
 *      - INT_TYPE_CABLE_DIAG,
 *      - INT_TYPE_ACL,
 *      - INT_TYPE_SLIENT

 */
rtksw_api_ret_t dal_rtl8371c_int_control_set(rtksw_uint32 unit, rtksw_int_type_t type, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
	rtksw_uint32 intEn;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= INT_TYPE_END)
        return RT_ERR_INPUT;

    if ((type == INT_TYPE_GREEN_FEATURE) || (type == INT_TYPE_THERMAL))
        return RT_ERR_INPUT;

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    intEn = (enable == RTKSW_ENABLED) ? 1 : 0;

    switch (type)
    {
        case INT_TYPE_LINK_STATUS:
			if ((retVal = reg16_field_write(unit, RTL8371C_IMRr, RTL8371C_LINK_CHANGEf, &intEn)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_METER_EXCEED:
			if ((retVal = reg16_field_write(unit, RTL8371C_IMRr, RTL8371C_METER_EXCEEDEDf, &intEn)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_LEARN_LIMIT:
			if ((retVal = reg16_field_write(unit, RTL8371C_IMRr, RTL8371C_LEARN_OVERf, &intEn)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_LINK_SPEED:
			if ((retVal = reg16_field_write(unit, RTL8371C_IMRr, RTL8371C_SPEED_CHANGEf, &intEn)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_CONGEST:
			if ((retVal = reg16_field_write(unit, RTL8371C_IMRr, RTL8371C_SPECIAL_CONGESTf, &intEn)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_LOOP_DETECT:
			if ((retVal = reg16_field_write(unit, RTL8371C_IMRr, RTL8371C_LOOP_DETECTIONf, &intEn)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_8051:
			if ((retVal = reg16_field_write(unit, RTL8371C_IMRr, RTL8371C_INTERRUPT_8051f, &intEn)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_CABLE_DIAG:
			if ((retVal = reg16_field_write(unit, RTL8371C_IMRr, RTL8371C_CABLE_DIAG_FINf, &intEn)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_ACL:
			if ((retVal = reg16_field_write(unit, RTL8371C_IMRr, RTL8371C_ACL_ACTIONf, &intEn)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_SLIENT:
			if ((retVal = reg16_field_write(unit, RTL8371C_IMRr, RTL8371C_SILENT_STARTf, &intEn)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_PTP1588:
			if ((retVal = reg16_field_write(unit, RTL8371C_IMRr, RTL8371C_PTP1588f, &intEn)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_THERMAL_METER_LOW:
			if ((retVal = reg16_field_write(unit, RTL8371C_IMRr, RTL8371C_THERMAL_METER_LOWf, &intEn)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_THERMAL_METER_HIGH:
			if ((retVal = reg16_field_write(unit, RTL8371C_IMRr, RTL8371C_THERMAL_METER_HIGHf, &intEn)) != RT_ERR_OK)
				return retVal;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /* Turn on IO PAD */
    regData = 1;
    if ((retVal = reg16_field_write(unit, RTL8371C_IO_MISC_FUNCr, RTL8371C_INT_ENf, &regData)) != RT_ERR_OK)
		return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_int_control_get
 * Description:
 *      Get interrupt trigger status configuration.
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
 *      - INT_TYPE_LINK_STATUS
 *      - INT_TYPE_METER_EXCEED
 *      - INT_TYPE_LEARN_LIMIT
 *      - INT_TYPE_LINK_SPEED
 *      - INT_TYPE_CONGEST
 *      - INT_TYPE_GREEN_FEATURE
 *      - INT_TYPE_LOOP_DETECT
 *      - INT_TYPE_8051,
 *      - INT_TYPE_CABLE_DIAG,
 *      - INT_TYPE_ACL,
 *      - INT_TYPE_UPS,
 *      - INT_TYPE_SLIENT

 */
rtksw_api_ret_t dal_rtl8371c_int_control_get(rtksw_uint32 unit, rtksw_int_type_t type, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((type == INT_TYPE_GREEN_FEATURE) || (type == INT_TYPE_THERMAL))
        return RT_ERR_INPUT;

    switch (type)
    {
        case INT_TYPE_LINK_STATUS:
			if ((retVal = reg16_field_read(unit, RTL8371C_IMRr, RTL8371C_LINK_CHANGEf, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
				return retVal;
            break;

        case INT_TYPE_METER_EXCEED:
			if ((retVal = reg16_field_read(unit, RTL8371C_IMRr, RTL8371C_METER_EXCEEDEDf, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_LEARN_LIMIT:
			if ((retVal = reg16_field_read(unit, RTL8371C_IMRr, RTL8371C_LEARN_OVERf, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_LINK_SPEED:
			if ((retVal = reg16_field_read(unit, RTL8371C_IMRr, RTL8371C_SPEED_CHANGEf, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_CONGEST:
			if ((retVal = reg16_field_read(unit, RTL8371C_IMRr, RTL8371C_SPECIAL_CONGESTf, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_LOOP_DETECT:
			if ((retVal = reg16_field_read(unit, RTL8371C_IMRr, RTL8371C_LOOP_DETECTIONf, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_8051:
			if ((retVal = reg16_field_read(unit, RTL8371C_IMRr, RTL8371C_INTERRUPT_8051f, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_CABLE_DIAG:
			if ((retVal = reg16_field_read(unit, RTL8371C_IMRr, RTL8371C_CABLE_DIAG_FINf, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_ACL:
			if ((retVal = reg16_field_read(unit, RTL8371C_IMRr, RTL8371C_ACL_ACTIONf, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_SLIENT:
			if ((retVal = reg16_field_read(unit, RTL8371C_IMRr, RTL8371C_SILENT_STARTf, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_PTP1588:
			if ((retVal = reg16_field_read(unit, RTL8371C_IMRr, RTL8371C_PTP1588f, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_THERMAL_METER_LOW:
			if ((retVal = reg16_field_read(unit, RTL8371C_IMRr, RTL8371C_THERMAL_METER_LOWf, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case INT_TYPE_THERMAL_METER_HIGH:
			if ((retVal = reg16_field_read(unit, RTL8371C_IMRr, RTL8371C_THERMAL_METER_HIGHf, (rtksw_uint32 *)pEnable)) != RT_ERR_OK)
				return retVal;
            break;

        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_int_status_set
 * Description:
 *      Set interrupt trigger status to clean.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pStatusMask - Interrupt status bit mask.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT - Invalid input parameters.
 * Note:
 *      The API can clean interrupt trigger status when interrupt happened.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_LINK_STATUS    (value[0] (Bit0))
 *      - INT_TYPE_METER_EXCEED   (value[0] (Bit1))
 *      - INT_TYPE_LEARN_LIMIT    (value[0] (Bit2))
 *      - INT_TYPE_LINK_SPEED     (value[0] (Bit3))
 *      - INT_TYPE_CONGEST        (value[0] (Bit4))
 *      - INT_TYPE_GREEN_FEATURE  (value[0] (Bit5))
 *      - INT_TYPE_LOOP_DETECT    (value[0] (Bit6))
 *      - INT_TYPE_8051           (value[0] (Bit7))
 *      - INT_TYPE_CABLE_DIAG     (value[0] (Bit8))
 *      - INT_TYPE_ACL            (value[0] (Bit9))
 *      - INT_TYPE_SLIENT         (value[0] (Bit11))
 *      The status will be cleared after execute this API.

 */
rtksw_api_ret_t dal_rtl8371c_int_status_set(rtksw_uint32 unit, rtksw_int_status_t *pStatusMask)
{
    rtksw_api_ret_t retVal;
	rtksw_uint32 statusMask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if(NULL == pStatusMask)
		return RT_ERR_NULL_POINTER;

	if ((pStatusMask->value[0] & (0x0001 << INT_TYPE_GREEN_FEATURE)) || (pStatusMask->value[0] & (0x0001 << INT_TYPE_THERMAL)))
		return RT_ERR_INPUT;

	if (pStatusMask->value[0] >= (0x0001 << INT_TYPE_END))
		return RT_ERR_INPUT;

	statusMask = pStatusMask->value[0];
	if ((retVal = reg16_write(unit, RTL8371C_IMSr, &statusMask)) != RT_ERR_OK)
		return retVal;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_int_status_get
 * Description:
 *      Get interrupt trigger status.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pStatusMask - Interrupt status bit mask.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get interrupt trigger status when interrupt happened.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_LINK_STATUS    (value[0] (Bit0))
 *      - INT_TYPE_METER_EXCEED   (value[0] (Bit1))
 *      - INT_TYPE_LEARN_LIMIT    (value[0] (Bit2))
 *      - INT_TYPE_LINK_SPEED     (value[0] (Bit3))
 *      - INT_TYPE_CONGEST        (value[0] (Bit4))
 *      - INT_TYPE_GREEN_FEATURE  (value[0] (Bit5))
 *      - INT_TYPE_LOOP_DETECT    (value[0] (Bit6))
 *      - INT_TYPE_8051           (value[0] (Bit7))
 *      - INT_TYPE_CABLE_DIAG     (value[0] (Bit8))
 *      - INT_TYPE_ACL            (value[0] (Bit9))
 *      - INT_TYPE_SLIENT         (value[0] (Bit11))

 *
 */
rtksw_api_ret_t dal_rtl8371c_int_status_get(rtksw_uint32 unit, rtksw_int_status_t* pStatusMask)
{
    rtksw_api_ret_t   retVal;
    rtksw_uint32          ims_mask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if(NULL == pStatusMask)
		return RT_ERR_NULL_POINTER;

	if ((retVal = reg16_read(unit, RTL8371C_IMSr, &ims_mask)) != RT_ERR_OK)
		return retVal;

	pStatusMask->value[0] = (ims_mask & 0x00001FFF);

	return RT_ERR_OK;
}

#if 1
#define ADV_NOT_SUPPORT (0xFFFF)
static rtksw_api_ret_t _rtksw_int_Advidx_get(rtksw_int_advType_t adv_type, rtksw_uint32 *pAsic_idx)
{
    rtksw_uint32 asic_idx[ADV_END] =
    {
        RTL8371C_INTRST_L2_LEARN,
        RTL8371C_INTRST_SPEED_CHANGE,
        RTL8371C_INTRST_SPECIAL_CONGESTION,
        RTL8371C_INTRST_RTKSW_PORT_LINKDOWN,
        RTL8371C_INTRST_RTKSW_PORT_LINKUP,
        ADV_NOT_SUPPORT,
        RTL8371C_INTRST_RLDP_LOOPED,
        RTL8371C_INTRST_RLDP_RELEASED,
    };

    if(adv_type >= ADV_END)
        return RT_ERR_INPUT;

    if(asic_idx[adv_type] == ADV_NOT_SUPPORT)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    *pAsic_idx = asic_idx[adv_type];
    return RT_ERR_OK;
}
#endif

static rtksw_api_ret_t _rtl8371c_getInterruptRelatedStatus(rtksw_uint32 unit, rtksw_uint32 type, rtksw_uint32* pStatus)
{
    CONST rtksw_uint32 indicatorAddress[RTL8371C_INTRST_END] = {RTL8371C_EVT_L2_LEARN_OVERr,
                                                    RTL8371C_EVT_SPEED_CHGr,
                                                    RTL8371C_EVT_SPECIAL_CGSTr,
                                                    RTL8371C_EVT_PLINKDOWNr,
                                                    RTL8371C_EVT_PLINUPr,
                                                    RTL8371C_MTR_OVER_RGr,
                                                    RTL8371C_RLDP_LOOP_STS0r,
                                                    RTL8371C_RLDP_LOOP_STS1r,
                                                    RTL8371C_EVT_SYS_LEARN_OVERr};

    if(type >= RTL8371C_INTRST_END )
        return RT_ERR_OUT_OF_RANGE;

    return reg16_read(unit, indicatorAddress[type], pStatus);
}

static rtksw_api_ret_t _rtl8371c_setInterruptRelatedStatus(rtksw_uint32 unit, rtksw_uint32 type, rtksw_uint32 status)
{
    CONST rtksw_uint32 indicatorAddress[RTL8371C_INTRST_END] = {RTL8371C_EVT_L2_LEARN_OVERr,
                                                    RTL8371C_EVT_SPEED_CHGr,
                                                    RTL8371C_EVT_SPECIAL_CGSTr,
                                                    RTL8371C_EVT_PLINKDOWNr,
                                                    RTL8371C_EVT_PLINUPr,
                                                    RTL8371C_MTR_OVER_RGr,
                                                    RTL8371C_RLDP_LOOP_STS0r,
                                                    RTL8371C_RLDP_LOOP_STS1r,
                                                    RTL8371C_EVT_SYS_LEARN_OVERr};


    if(type >= RTL8371C_INTRST_END )
        return RT_ERR_OUT_OF_RANGE;

    return reg16_write(unit, indicatorAddress[type], &status);
}


/* Function Name:
 *      dal_rtl8371c_int_advanceInfo_get
 * Description:
 *      Get interrupt advanced information.
 * Input:
 *      unit        - Unit ID
 *      adv_type    - Advanced interrupt type.
 * Output:
 *      info - Information per type.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get advanced information when interrupt happened.
 *      The status will be cleared after execute this API.
 */
rtksw_api_ret_t dal_rtl8371c_int_advanceInfo_get(rtksw_uint32 unit, rtksw_int_advType_t adv_type, rtksw_int_info_t *pInfo)
{
    rtksw_api_ret_t   retVal;
    rtksw_uint32      data;
    rtksw_uint32      phyData = 0;
    rtksw_uint32      intAdvType;
    rtksw_uint32      index;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(adv_type >= ADV_END)
        return RT_ERR_INPUT;

    if(NULL == pInfo)
        return RT_ERR_NULL_POINTER;

    if(adv_type != ADV_METER_EXCEED_MASK)
    {
        if((retVal = _rtksw_int_Advidx_get(adv_type, &intAdvType)) != RT_ERR_OK)
            return retVal;
    }

    switch(adv_type)
    {
        case ADV_L2_LEARN_PORT_MASK:
            /* Get physical portmask */
            if((retVal = _rtl8371c_getInterruptRelatedStatus(unit, intAdvType, &data)) != RT_ERR_OK)
                return retVal;

            /* Clear Advanced Info */
            if((retVal = _rtl8371c_setInterruptRelatedStatus(unit, intAdvType, 0xFFFF)) != RT_ERR_OK)
                return retVal;

            /* Translate to logical portmask */
            if((retVal = rtksw_switch_portmask_P2L_get(unit, data, &(pInfo->portMask))) != RT_ERR_OK)
                return retVal;

            /* Get system learn */
            if((retVal = _rtl8371c_getInterruptRelatedStatus(unit, RTL8371C_INTRST_SYS_LEARN, &data)) != RT_ERR_OK)
                return retVal;

            /* Clear system learn */
            if((retVal = _rtl8371c_setInterruptRelatedStatus(unit, RTL8371C_INTRST_SYS_LEARN, 0x0001)) != RT_ERR_OK)
                return retVal;

            pInfo->systemLearnOver = data;
            break;
        case ADV_SPEED_CHANGE_PORT_MASK:
        case ADV_SPECIAL_CONGESTION_PORT_MASK:
        case ADV_PORT_LINKDOWN_PORT_MASK:
        case ADV_PORT_LINKUP_PORT_MASK:
            /* Get physical portmask */
            if((retVal = _rtl8371c_getInterruptRelatedStatus(unit, intAdvType, &data)) != RT_ERR_OK)
                return retVal;

            /* Clear Advanced Info */
            if((retVal = _rtl8371c_setInterruptRelatedStatus(unit, intAdvType, 0xFFFF)) != RT_ERR_OK)
                return retVal;

            /* Translate to logical portmask */
            if (data & 0x01)
                phyData |= 0x01;
            
            if (data & 0x02)
                phyData |= 0x02;
            
            if (data & 0x04)
                phyData |= 0x04;
            
            if (data & 0x08)
                phyData |= 0x08;
            
            if (data & 0x10)
                phyData |= 0x10;
            
            if (data & 0x20)
                phyData |= 0x40;
            
            if (data & 0x40)
                phyData |= 0x80;
            
            if (data & 0x80)
                phyData |= 0x20;

            if((retVal = rtksw_switch_portmask_P2L_get(unit, phyData, &(pInfo->portMask))) != RT_ERR_OK)
                return retVal;

            break;
        case ADV_RLDP_LOOPED:
        case ADV_RLDP_RELEASED:
            /* Get physical portmask */
            if((retVal = _rtl8371c_getInterruptRelatedStatus(unit, intAdvType, &data)) != RT_ERR_OK)
                return retVal;

            /* Clear Advanced Info */
            if((retVal = _rtl8371c_setInterruptRelatedStatus(unit, intAdvType, 0xFFFF)) != RT_ERR_OK)
                return retVal;

            /* Translate to logical portmask */
            if((retVal = rtksw_switch_portmask_P2L_get(unit, data, &(pInfo->portMask))) != RT_ERR_OK)
                return retVal;

            break;
        case ADV_METER_EXCEED_MASK:
			for(index = 0; index < 32; index++)
			{
				/* Get Meter Mask */
				if ((retVal = reg16_array_field_read(unit, RTL8371C_MTR_OVER_RGr, REG_ARRAY_INDEX_NONE, index, RTL8371C_METER_OVERf, &data)) != RT_ERR_OK)
					return retVal;

				if(data)
					pInfo->meterMask[0] |= 1 << index;
				else
					pInfo->meterMask[0] &= ~(1 << index);

				/* Clear Advanced Info */
				data = 1;
				if ((retVal = reg16_array_field_write(unit, RTL8371C_MTR_OVER_RGr, REG_ARRAY_INDEX_NONE, index, RTL8371C_METER_OVERf, &data)) != RT_ERR_OK)
					return retVal;
			}
			for(index = 32; index < 40; index++)
			{
				/* Get Meter Mask */
				if ((retVal = reg16_array_field_read(unit, RTL8371C_MTR_OVER_RGr, REG_ARRAY_INDEX_NONE, index, RTL8371C_METER_OVERf, &data)) != RT_ERR_OK)
					return retVal;

				if(data)
					pInfo->meterMask[1] |= 1 << index;
				else
					pInfo->meterMask[1] &= ~(1 << index);

				/* Clear Advanced Info */
				data = 1;
				if ((retVal = reg16_array_field_write(unit, RTL8371C_MTR_OVER_RGr, REG_ARRAY_INDEX_NONE, index, RTL8371C_METER_OVERf, &data)) != RT_ERR_OK)
					return retVal;
			}
            break;
        case ADV_SILENT_START:
			if ((retVal = reg16_field_read(unit, RTL8371C_EVT_SILENT_STARTr, RTL8371C_INDICATORf, &data)) != RT_ERR_OK)
				return retVal;
			pInfo->silentStart = data;
			break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

