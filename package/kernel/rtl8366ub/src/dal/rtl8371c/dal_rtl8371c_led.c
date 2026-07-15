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
#include <dal/rtl8371c/dal_rtl8371c_led.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      dal_rtl8371c_led_operation_set
 * Description:
 *      Set Led operation mode
 * Input:
 *      unit    - Unit ID
 *      mode 	- LED operation mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can set Led operation mode.
 *      The modes that can be set are as following:
 *      - LED_OP_SCAN,
 *      - LED_OP_PARALLEL,
 *      - LED_OP_SERIAL,
 */
rtksw_api_ret_t dal_rtl8371c_led_operation_set(rtksw_uint32 unit, rtksw_led_operation_t mode)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 enable;
    rtksw_uint32 ledMode;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (mode >= LED_OP_END)
      return RT_ERR_INPUT;

    switch (mode)
    {
        case LED_OP_PARALLEL:
			ledMode = RTL8371C_LEDOP_PARALLEL;
			if ((retVal = reg16_field_write(unit, RTL8371C_SYS_LED_CFGr, RTL8371C_LED_SELECTf, &ledMode)) != RT_ERR_OK)
				return retVal;

			enable = 0;
			if ((retVal = reg16_field_write(unit, RTL8371C_SCAN0_LED_IO_ENr, RTL8371C_LED_SERI_CLK_ENf, &enable)) != RT_ERR_OK)
				return retVal;
			if ((retVal = reg16_field_write(unit, RTL8371C_SCAN0_LED_IO_ENr, RTL8371C_LED_SERI_DATA_ENf, &enable)) != RT_ERR_OK)
				return retVal;
            break;
        case LED_OP_SERIAL:
			ledMode = RTL8371C_LEDOP_SERIAL;
			if ((retVal = reg16_field_write(unit, RTL8371C_SYS_LED_CFGr, RTL8371C_LED_SELECTf, &ledMode)) != RT_ERR_OK)
				return retVal;

			enable = 1;
			if ((retVal = reg16_field_write(unit, RTL8371C_SCAN0_LED_IO_ENr, RTL8371C_LED_SERI_CLK_ENf, &enable)) != RT_ERR_OK)
				return retVal;
			if ((retVal = reg16_field_write(unit, RTL8371C_SCAN0_LED_IO_ENr, RTL8371C_LED_SERI_DATA_ENf, &enable)) != RT_ERR_OK)
				return retVal;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_operation_get
 * Description:
 *      Get Led operation mode
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pMode - Support LED operation mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get Led operation mode.
 *      The modes that can be set are as following:
 *      - LED_OP_SCAN,
 *      - LED_OP_PARALLEL,
 *      - LED_OP_SERIAL,
 */
rtksw_api_ret_t dal_rtl8371c_led_operation_get(rtksw_uint32 unit, rtksw_led_operation_t *pMode)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 ledMode;


    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if (pMode == NULL)
        return RT_ERR_NULL_POINTER;

	if ((retVal = reg16_field_read(unit, RTL8371C_SYS_LED_CFGr, RTL8371C_LED_SELECTf, &ledMode)) != RT_ERR_OK)
		return retVal;

    if (ledMode == RTL8371C_LEDOP_SERIAL)
        *pMode = LED_OP_SERIAL;
    else if (ledMode == RTL8371C_LEDOP_PARALLEL)
        *pMode = LED_OP_PARALLEL;
    else
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_modeForce_set
 * Description:
 *      Set Led group to congiuration force mode
 * Input:
 *      unit    - Unit ID
 *      port    - port ID
 *      group   - Support LED group id.
 *      mode    - Support LED force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Error Port ID
 * Note:
 *      The API can force to one force mode.
 *      The force modes that can be set are as following:
 *      - LED_FORCE_NORMAL,
 *      - LED_FORCE_BLINK,
 *      - LED_FORCE_OFF,
 *      - LED_FORCE_ON.
 */
rtksw_api_ret_t dal_rtl8371c_led_modeForce_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_led_group_t group, rtksw_led_force_mode_t mode)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyPort;
	rtksw_uint32 reg;
    rtksw_uint32 field;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    /* No LED for CPU port */
    if(rtksw_switch_isCPUPort(unit, port) == RT_ERR_OK)
        return RT_ERR_PORT_ID;

    if (group >= LED_GROUP_END)
        return RT_ERR_INPUT;

    if (mode >= LED_FORCE_END)
        return RT_ERR_NOT_ALLOWED;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    /* Set Related Registers */
    if (phyPort == 0){
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT0_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT0_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT0_LED2_MODEf;
		}
	}
    else if (phyPort == 1)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT1_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT1_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT1_LED2_MODEf;
		}
	}
    else if (phyPort == 2)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT2_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT2_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT2_LED2_MODEf;
		}
	}
    else if (phyPort == 3)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT3_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT3_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT3_LED2_MODEf;
		}
	}
    else if (phyPort == 4)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT4_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT4_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT4_LED2_MODEf;
		}
	}
    else if (phyPort == 5)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT5_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT5_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT5_LED2_MODEf;
		}
	}
    else if (phyPort == 6)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT6_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT6_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT6_LED2_MODEf;
		}
	}
    else if (phyPort == 7)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT7_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT7_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT7_LED2_MODEf;
		}
	}
	else
		return RT_ERR_PORT_ID;

    if ((retVal = reg16_field_write(unit, reg, field, (rtksw_uint32 *)&mode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_modeForce_get
 * Description:
 *      Get Led group to congiuration force mode
 * Input:
 *      unit    - Unit ID
 *      port  	- port ID
 *      group 	- Support LED group id.
 *      pMode 	- Support LED force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Error Port ID
 * Note:
 *      The API can get forced Led group mode.
 *      The force modes that can be set are as following:
 *      - LED_FORCE_NORMAL,
 *      - LED_FORCE_BLINK,
 *      - LED_FORCE_OFF,
 *      - LED_FORCE_ON.
 */
rtksw_api_ret_t dal_rtl8371c_led_modeForce_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_led_group_t group, rtksw_led_force_mode_t *pMode)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyPort;
	rtksw_uint32 reg;
    rtksw_uint32 field;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    /* No LED for CPU port */
    if(rtksw_switch_isCPUPort(unit, port) == RT_ERR_OK)
        return RT_ERR_PORT_ID;

    if (group >= LED_GROUP_END)
        return RT_ERR_INPUT;

    if (NULL == pMode)
        return RT_ERR_NULL_POINTER;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    /* Get Related Registers */
    if (phyPort == 0){
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT0_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT0_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT0_LED2_MODEf;
		}
	}
    else if (phyPort == 1)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT1_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT1_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT1_LED2_MODEf;
		}
	}
    else if (phyPort == 2)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT2_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT2_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT2_LED2_MODEf;
		}
	}
    else if (phyPort == 3)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT3_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT3_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT3_LED2_MODEf;
		}
	}
    else if (phyPort == 4)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT4_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT4_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT4_LED2_MODEf;
		}
	}
    else if (phyPort == 5)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT5_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT5_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT5_LED2_MODEf;
		}
	}
    else if (phyPort == 6)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT6_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT6_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT6_LED2_MODEf;
		}
	}
    else if (phyPort == 7)
	{
		if(group == LED_GROUP_0){
			reg = RTL8371C_CPU_FORCE_LED0_CFG0r;
		    field = RTL8371C_PORT7_LED0_MODEf;
		}
		else if(group == LED_GROUP_1){
			reg = RTL8371C_CPU_FORCE_LED1_CFG0r;
		    field = RTL8371C_PORT7_LED1_MODEf;
		}
		else if(group == LED_GROUP_2){
			reg = RTL8371C_CPU_FORCE_LED2_CFG0r;
		    field = RTL8371C_PORT7_LED2_MODEf;
		}
	}
	else
		return RT_ERR_PORT_ID;

    if ((retVal = reg16_field_read(unit, reg, field, (rtksw_uint32 *)pMode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_blinkRate_set
 * Description:
 *      Set LED blinking rate
 * Input:
 *      unit    	- Unit ID
 *      blinkRate 	- blinking rate.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API configure LED blinking rate:
 *      - LED_BLINKRATE_32MS
 *      - LED_BLINKRATE_64MS,
 *      - LED_BLINKRATE_128MS,
 *      - LED_BLINKRATE_256MS,
 *      - LED_BLINKRATE_512MS,
 *      - LED_BLINKRATE_1024MS,
 *      - LED_BLINKRATE_48MS,
 *      - LED_BLINKRATE_96MS,
 */
rtksw_api_ret_t dal_rtl8371c_led_blinkRate_set(rtksw_uint32 unit, rtksw_led_blink_rate_t blinkRate)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (blinkRate >= LED_BLINKRATE_END)
        return RT_ERR_FAILED;

    if ((retVal = reg16_field_write(unit, RTL8371C_LED_MODEr, RTL8371C_SEL_LEDRATEf, (rtksw_uint32 *)&blinkRate)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_blinkRate_get
 * Description:
 *      Get LED blinking rate at mode 0 to mode 3
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pBlinkRate - blinking rate.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API get LED blinking rate:
 *      - LED_BLINKRATE_32MS
 *      - LED_BLINKRATE_64MS,
 *      - LED_BLINKRATE_128MS,
 *      - LED_BLINKRATE_256MS,
 *      - LED_BLINKRATE_512MS,
 *      - LED_BLINKRATE_1024MS,
 *      - LED_BLINKRATE_48MS,
 *      - LED_BLINKRATE_96MS,
 */
rtksw_api_ret_t dal_rtl8371c_led_blinkRate_get(rtksw_uint32 unit, rtksw_led_blink_rate_t *pBlinkRate)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pBlinkRate)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_write(unit, RTL8371C_LED_MODEr, RTL8371C_SEL_LEDRATEf, (rtksw_uint32 *)pBlinkRate)) != RT_ERR_OK)
        return retVal;


    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_groupConfig_set
 * Description:
 *      Set per group Led to congiuration mode
 * Input:
 *      unit    - Unit ID
 *      group   - LED group.
 *      config  - LED configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can set LED indicated information configuration for each LED group with 1 to 1 led mapping to each port.
 *      - Definition  LED Statuses      Description
 *      - 0000        LED_Off           LED pin Tri-State.
 *      - 0001        Dup/Col           Collision, Full duplex Indicator.
 *      - 0010        Link/Act          Link, Activity Indicator.
 *      - 0011        Spd1000           1000Mb/s Speed Indicator.
 *      - 0100        Spd100            100Mb/s Speed Indicator.
 *      - 0101        Spd10             10Mb/s Speed Indicator.
 *      - 0110        Spd1000/Act       1000Mb/s Speed/Activity Indicator.
 *      - 0111        Spd100/Act        100Mb/s Speed/Activity Indicator.
 *      - 1000        Spd10/Act         10Mb/s Speed/Activity Indicator.
 *      - 1001        Spd100 (10)/Act   10/100Mb/s Speed/Activity Indicator.
 *      - 1010        LoopDetect        LoopDetect Indicator.
 *      - 1011        EEE               EEE Indicator.
 *      - 1100        Link/Rx           Link, Activity Indicator.
 *      - 1101        Link/Tx           Link, Activity Indicator.
 *      - 1110        Master            Link on Master Indicator.
 *      - 1111        Act               Activity Indicator. Low for link established.
 */
rtksw_api_ret_t dal_rtl8371c_led_groupConfig_set(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_led_config_t config)
{
    rtksw_api_ret_t retVal;
	rtksw_uint32 enable;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ( LED_GROUP_END <= group )
		return RT_ERR_FAILED;

	if ( LED_CONFIG_END <= config )
		return RT_ERR_FAILED;

	enable = 0;
	if ((retVal = reg16_field_write(unit, RTL8371C_DATA_LED_CFGr, RTL8371C_LED_CONFIG_SELf, &enable)) != RT_ERR_OK)
	return retVal;

	if (group == LED_GROUP_0){
		if ((retVal = reg16_field_write(unit, RTL8371C_DATA_LED_CFGr, RTL8371C_LED0_CFGf, (rtksw_uint32 *)&config)) != RT_ERR_OK)
			return retVal;
	}
	else if (group == LED_GROUP_1){
	    if ((retVal = reg16_field_write(unit, RTL8371C_DATA_LED_CFGr, RTL8371C_LED1_CFGf, (rtksw_uint32 *)&config)) != RT_ERR_OK)
	        return retVal;
	}
	else if (group == LED_GROUP_2){
	    if ((retVal = reg16_field_write(unit, RTL8371C_DATA_LED_CFGr, RTL8371C_LED2_CFGf, (rtksw_uint32 *)&config)) != RT_ERR_OK)
	        return retVal;
	}

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_groupConfig_get
 * Description:
 *      Get Led group congiuration mode
 * Input:
 *      unit    - Unit ID
 *      group 	- LED group.
 * Output:
 *      pConfig - LED configuration.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *       The API can get LED indicated information configuration for each LED group.
 */
rtksw_api_ret_t dal_rtl8371c_led_groupConfig_get(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_led_config_t *pConfig)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if (LED_GROUP_END <= group)
		return RT_ERR_FAILED;

	if (NULL == pConfig)
		return RT_ERR_NULL_POINTER;

	if (group == LED_GROUP_0){
	    if ((retVal = reg16_field_read(unit, RTL8371C_DATA_LED_CFGr, RTL8371C_LED0_CFGf, (rtksw_uint32 *)pConfig)) != RT_ERR_OK)
	        return retVal;
	}
	else if (group == LED_GROUP_1){
	    if ((retVal = reg16_field_read(unit, RTL8371C_DATA_LED_CFGr, RTL8371C_LED1_CFGf, (rtksw_uint32 *)pConfig)) != RT_ERR_OK)
	        return retVal;
	}
	else if (group == LED_GROUP_2){
	    if ((retVal = reg16_field_read(unit, RTL8371C_DATA_LED_CFGr, RTL8371C_LED2_CFGf, (rtksw_uint32 *)pConfig)) != RT_ERR_OK)
	        return retVal;
	}

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_groupAbility_set
 * Description:
 *      Configure per group Led ability
 * Input:
 *      unit    	- Unit ID
 *      group    	- LED group.
 *      pAbility 	- LED ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None.
 */

rtksw_api_ret_t dal_rtl8371c_led_groupAbility_set(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_led_ability_t *pAbility)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (LED_GROUP_END <= group)
        return RT_ERR_FAILED;

    if(pAbility == NULL)
        return RT_ERR_NULL_POINTER;

    if( (pAbility->link_10m >= RTKSW_ENABLE_END) || (pAbility->link_100m >= RTKSW_ENABLE_END) ||
        (pAbility->link_500m >= RTKSW_ENABLE_END) || (pAbility->link_1000m >= RTKSW_ENABLE_END) || (pAbility->link_2500m >= RTKSW_ENABLE_END) ||
        (pAbility->act_rx >= RTKSW_ENABLE_END) || (pAbility->act_tx >= RTKSW_ENABLE_END) )
    {
        return RT_ERR_INPUT;
    }

	if(group == LED_GROUP_0){
	    if ((retVal = reg16_read(unit, RTL8371C_LED0_DATA_CTRLr, &regData)) != RT_ERR_OK)
	        return retVal;
	}
	else if(group == LED_GROUP_1){
	    if ((retVal = reg16_read(unit, RTL8371C_LED1_DATA_CTRLr, &regData)) != RT_ERR_OK)
	        return retVal;
	}
	else if(group == LED_GROUP_2){
	    if ((retVal = reg16_read(unit, RTL8371C_LED2_DATA_CTRLr, &regData)) != RT_ERR_OK)
	        return retVal;
	}

    if(pAbility->link_10m == RTKSW_ENABLED)
        regData |= 0x0001;
    else
        regData &= ~0x0001;

    if(pAbility->link_100m == RTKSW_ENABLED)
        regData |= 0x0002;
    else
        regData &= ~0x0002;

    if(pAbility->link_500m == RTKSW_ENABLED)
        regData |= 0x0004;
    else
        regData &= ~0x0004;

    if(pAbility->link_1000m == RTKSW_ENABLED)
        regData |= 0x0008;
    else
        regData &= ~0x0008;

    if(pAbility->link_2500m == RTKSW_ENABLED)
        regData |= 0x0020;
    else
        regData &= ~0x0020;

    regData |= (0x0001 << 13);

	if(group == LED_GROUP_0){
	    if ((retVal = reg16_write(unit, RTL8371C_LED0_DATA_CTRLr, &regData)) != RT_ERR_OK)
	        return retVal;
	}
	else if(group == LED_GROUP_1){
	    if ((retVal = reg16_write(unit, RTL8371C_LED1_DATA_CTRLr, &regData)) != RT_ERR_OK)
	        return retVal;
	}
	else if(group == LED_GROUP_2){
	    if ((retVal = reg16_write(unit, RTL8371C_LED2_DATA_CTRLr, &regData)) != RT_ERR_OK)
	        return retVal;
	}

	if(group == LED_GROUP_0){
	    if ((retVal = reg16_read(unit, RTL8371C_LED0_DATA_CTRL_1r, &regData)) != RT_ERR_OK)
	        return retVal;
	}
	else if(group == LED_GROUP_1){
	    if ((retVal = reg16_read(unit, RTL8371C_LED1_DATA_CTRL_1r, &regData)) != RT_ERR_OK)
	        return retVal;
	}
	else if(group == LED_GROUP_2){
	    if ((retVal = reg16_read(unit, RTL8371C_LED2_DATA_CTRL_1r, &regData)) != RT_ERR_OK)
	        return retVal;
	}

	if(pAbility->act_rx == RTKSW_ENABLED)
        regData |= 0x0001;
    else
        regData &= ~0x0001;

    if(pAbility->act_tx == RTKSW_ENABLED)
        regData |= 0x0002;
    else
        regData &= ~0x0002;

    if(group == LED_GROUP_0){
	    if ((retVal = reg16_write(unit, RTL8371C_LED0_DATA_CTRL_1r, &regData)) != RT_ERR_OK)
	        return retVal;
	}
	else if(group == LED_GROUP_1){
	    if ((retVal = reg16_write(unit, RTL8371C_LED1_DATA_CTRL_1r, &regData)) != RT_ERR_OK)
	        return retVal;
	}
	else if(group == LED_GROUP_2){
	    if ((retVal = reg16_write(unit, RTL8371C_LED2_DATA_CTRL_1r, &regData)) != RT_ERR_OK)
	        return retVal;
	}

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_groupAbility_get
 * Description:
 *      Get per group Led ability
 * Input:
 *      unit    	- Unit ID
 *      group    	- LED group.
 *      pAbility 	- LED ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None.
 */

rtksw_api_ret_t dal_rtl8371c_led_groupAbility_get(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_led_ability_t *pAbility)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if (LED_GROUP_END <= group)
		return RT_ERR_FAILED;

	if (pAbility == NULL)
        return RT_ERR_NULL_POINTER;

	if (group == LED_GROUP_0){
		if ((retVal = reg16_read(unit, RTL8371C_LED0_DATA_CTRLr, &regData)) != RT_ERR_OK)
			return retVal;
	}
	else if (group == LED_GROUP_1){
		if ((retVal = reg16_read(unit, RTL8371C_LED1_DATA_CTRLr, &regData)) != RT_ERR_OK)
			return retVal;
	}
	else if (group == LED_GROUP_2){
		if ((retVal = reg16_read(unit, RTL8371C_LED2_DATA_CTRLr, &regData)) != RT_ERR_OK)
			return retVal;
	}

    pAbility->link_10m = (regData & 0x0001) ? RTKSW_ENABLED : RTKSW_DISABLED;
    pAbility->link_100m = (regData & 0x0002) ? RTKSW_ENABLED : RTKSW_DISABLED;
    pAbility->link_500m = (regData & 0x0004) ? RTKSW_ENABLED : RTKSW_DISABLED;
    pAbility->link_1000m = (regData & 0x0008) ? RTKSW_ENABLED : RTKSW_DISABLED;
    pAbility->link_2500m = (regData & 0x0020) ? RTKSW_ENABLED : RTKSW_DISABLED;

	if(group == LED_GROUP_0){
	    if ((retVal = reg16_read(unit, RTL8371C_LED0_DATA_CTRL_1r, &regData)) != RT_ERR_OK)
	        return retVal;
	}
	else if(group == LED_GROUP_1){
	    if ((retVal = reg16_read(unit, RTL8371C_LED1_DATA_CTRL_1r, &regData)) != RT_ERR_OK)
	        return retVal;
	}
	else if(group == LED_GROUP_2){
	    if ((retVal = reg16_read(unit, RTL8371C_LED2_DATA_CTRL_1r, &regData)) != RT_ERR_OK)
	        return retVal;
	}

    pAbility->act_rx = (regData & 0x0001) ? RTKSW_ENABLED : RTKSW_DISABLED;
    pAbility->act_tx = (regData & 0x0002) ? RTKSW_ENABLED : RTKSW_DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_serialMode_set
 * Description:
 *      Set Led serial mode active congiuration
 * Input:
 *      unit    - Unit ID
 *      active 	- LED group.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can set LED serial mode active congiuration.
 */
rtksw_api_ret_t dal_rtl8371c_led_serialMode_set(rtksw_uint32 unit, rtksw_led_active_t active)
{
    rtksw_api_ret_t retVal;
	rtksw_uint32 mode;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (active >= RTKSW_LED_ACTIVE_END)
        return RT_ERR_INPUT;

    /* Set Active High or Low */
	if ((retVal = reg16_field_write(unit, RTL8371C_SYS_LED_CFGr, RTL8371C_SERI_LED_ACT_LOWf, (rtksw_uint32 *)&active)) != RT_ERR_OK)
        return retVal;

	/*set to 8G mode (not 16G mode)*/
	mode = 1;
	if ((retVal = reg16_field_write(unit, RTL8371C_LED_MODEr, RTL8371C_DLINK_TIMEf, &mode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_serialMode_get
 * Description:
 *      Get Led group congiuration mode
 * Input:
 *      unit    - Unit ID
 *      group 	- LED group.
 * Output:
 *      pConfig - LED configuration.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *       The API can get LED serial mode active configuration.
 */
rtksw_api_ret_t dal_rtl8371c_led_serialMode_get(rtksw_uint32 unit, rtksw_led_active_t *pActive)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if (NULL == pActive)
		return RT_ERR_NULL_POINTER;

	if ((retVal = reg16_field_read(unit, RTL8371C_SYS_LED_CFGr, RTL8371C_SERI_LED_ACT_LOWf, (rtksw_uint32 *)pActive)) != RT_ERR_OK)
		return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_OutputEnable_set
 * Description:
 *      This API set LED I/O state.
 * Input:
 *      unit    	- Unit ID
 *      enabled     - LED I/O state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      This API set LED I/O state.
 */
rtksw_api_ret_t dal_rtl8371c_led_OutputEnable_set(rtksw_uint32 unit, rtksw_enable_t state)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regdata;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (state >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    if (state == 1)
        regdata = 0;
	else
		regdata = 1;

	if ((retVal = reg16_field_write(unit, RTL8371C_SYS_LED_CFGr, RTL8371C_LED_IO_DISABLEf, &regdata)) != RT_ERR_OK)
		return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_OutputEnable_get
 * Description:
 *      This API get LED I/O state.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pEnabled        - LED I/O state
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      This API set current LED I/O  state.
 */
rtksw_api_ret_t dal_rtl8371c_led_OutputEnable_get(rtksw_uint32 unit, rtksw_enable_t *pState)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regdata;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if (pState == NULL)
        return RT_ERR_NULL_POINTER;

	if ((retVal = reg16_field_read(unit, RTL8371C_SYS_LED_CFGr, RTL8371C_LED_IO_DISABLEf, &regdata)) != RT_ERR_OK)
		return retVal;

    if (regdata == 1)
        *pState = 0;
    else
        *pState = 1;

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rtl8371c_led_parallelLEDRemap_set
 * Description:
 *      This API configure parallel LED output remapping
 * Input:
 *      unit        - Unit ID
 *      LEDPinNo    - LED pin number
 *      port        - port
 *      group       - LED group
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_led_parallelLEDRemap_set(rtksw_uint32 unit, rtksw_uint32 LEDPinNo, rtksw_port_t port, rtksw_led_group_t group)
{
	rtksw_uint32 ledRemapReg[] = 
	{
		RTL8371C_LED_IO_MAPPING8r,
		RTL8371C_LED_IO_MAPPING8r,
		RTL8371C_LED_IO_MAPPING8r,
		RTL8371C_LED_IO_MAPPING7r,
		RTL8371C_LED_IO_MAPPING7r,
		RTL8371C_LED_IO_MAPPING7r,
		RTL8371C_LED_IO_MAPPING6r,
		RTL8371C_LED_IO_MAPPING6r,
		RTL8371C_LED_IO_MAPPING6r,
		RTL8371C_LED_IO_MAPPING5r,
		RTL8371C_LED_IO_MAPPING5r,
		RTL8371C_LED_IO_MAPPING5r,
		RTL8371C_LED_IO_MAPPING4r,
		RTL8371C_LED_IO_MAPPING4r,
		RTL8371C_LED_IO_MAPPING4r,
		RTL8371C_LED_IO_MAPPING3r,
		RTL8371C_LED_IO_MAPPING3r,
		RTL8371C_LED_IO_MAPPING3r,
		RTL8371C_LED_IO_MAPPING2r,
		RTL8371C_LED_IO_MAPPING2r,
		RTL8371C_LED_IO_MAPPING2r
	};

	rtksw_uint32 ledRemapField[] = 
	{
		RTL8371C_CFG_LED_MUX_X0f,
		RTL8371C_CFG_LED_MUX_X1f,
		RTL8371C_CFG_LED_MUX_X2f,
		RTL8371C_CFG_LED_MUX_X3f,
		RTL8371C_CFG_LED_MUX_X4f,
		RTL8371C_CFG_LED_MUX_X5f,
		RTL8371C_CFG_LED_MUX_X6f,
		RTL8371C_CFG_LED_MUX_X7f,
		RTL8371C_CFG_LED_MUX_X8f,
		RTL8371C_CFG_LED_MUX_X9f,
		RTL8371C_CFG_LED_MUX_X10f,
		RTL8371C_CFG_LED_MUX_X11f,
		RTL8371C_CFG_LED_MUX_X12f,
		RTL8371C_CFG_LED_MUX_X13f,
		RTL8371C_CFG_LED_MUX_X14f,
		RTL8371C_CFG_LED_MUX_X15f,
		RTL8371C_CFG_LED_MUX_X16f,
		RTL8371C_CFG_LED_MUX_X17f,
		RTL8371C_CFG_LED_MUX_X18f,
		RTL8371C_CFG_LED_MUX_X19f,
		RTL8371C_CFG_LED_MUX_X20f
	};

	rtksw_api_ret_t retVal;
	rtksw_uint32 phyPort;
	rtksw_uint32 regData;

	/* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if (LEDPinNo > RTL8371C_MAX_LED_PIN_NO)
		return RT_ERR_INPUT;
	
	RTKSW_CHK_PORT_VALID(unit, port);

	if (group >= LED_GROUP_END)
		return RT_ERR_INPUT;

	phyPort = rtksw_switch_port_L2P_get(unit, port);
	regData = (phyPort * 3) + (rtksw_uint32)group;

	if ((retVal = reg16_field_write(unit, ledRemapReg[LEDPinNo], ledRemapField[LEDPinNo], &regData)) != RT_ERR_OK)
		return retVal;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_parallelLEDRemap_set
 * Description:
 *      This API configure parallel LED output remapping
 * Input:
 *      unit        - Unit ID
 *      LEDPinNo    - LED pin number
 * Output:
 *      pPort       - port
 *      pGroup      - LED group
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_led_parallelLEDRemap_get(rtksw_uint32 unit, rtksw_uint32 LEDPinNo, rtksw_port_t *pPort, rtksw_led_group_t *pGroup)
{
	rtksw_uint32 ledRemapReg[] = 
	{
		RTL8371C_LED_IO_MAPPING8r,
		RTL8371C_LED_IO_MAPPING8r,
		RTL8371C_LED_IO_MAPPING8r,
		RTL8371C_LED_IO_MAPPING7r,
		RTL8371C_LED_IO_MAPPING7r,
		RTL8371C_LED_IO_MAPPING7r,
		RTL8371C_LED_IO_MAPPING6r,
		RTL8371C_LED_IO_MAPPING6r,
		RTL8371C_LED_IO_MAPPING6r,
		RTL8371C_LED_IO_MAPPING5r,
		RTL8371C_LED_IO_MAPPING5r,
		RTL8371C_LED_IO_MAPPING5r,
		RTL8371C_LED_IO_MAPPING4r,
		RTL8371C_LED_IO_MAPPING4r,
		RTL8371C_LED_IO_MAPPING4r,
		RTL8371C_LED_IO_MAPPING3r,
		RTL8371C_LED_IO_MAPPING3r,
		RTL8371C_LED_IO_MAPPING3r,
		RTL8371C_LED_IO_MAPPING2r,
		RTL8371C_LED_IO_MAPPING2r,
		RTL8371C_LED_IO_MAPPING2r
	};

	rtksw_uint32 ledRemapField[] = 
	{
		RTL8371C_CFG_LED_MUX_X0f,
		RTL8371C_CFG_LED_MUX_X1f,
		RTL8371C_CFG_LED_MUX_X2f,
		RTL8371C_CFG_LED_MUX_X3f,
		RTL8371C_CFG_LED_MUX_X4f,
		RTL8371C_CFG_LED_MUX_X5f,
		RTL8371C_CFG_LED_MUX_X6f,
		RTL8371C_CFG_LED_MUX_X7f,
		RTL8371C_CFG_LED_MUX_X8f,
		RTL8371C_CFG_LED_MUX_X9f,
		RTL8371C_CFG_LED_MUX_X10f,
		RTL8371C_CFG_LED_MUX_X11f,
		RTL8371C_CFG_LED_MUX_X12f,
		RTL8371C_CFG_LED_MUX_X13f,
		RTL8371C_CFG_LED_MUX_X14f,
		RTL8371C_CFG_LED_MUX_X15f,
		RTL8371C_CFG_LED_MUX_X16f,
		RTL8371C_CFG_LED_MUX_X17f,
		RTL8371C_CFG_LED_MUX_X18f,
		RTL8371C_CFG_LED_MUX_X19f,
		RTL8371C_CFG_LED_MUX_X20f
	};

	rtksw_api_ret_t retVal;
	rtksw_uint32 phyPort;
	rtksw_uint32 regData;

	/* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if (LEDPinNo > RTL8371C_MAX_LED_PIN_NO)
		return RT_ERR_INPUT;

	if (pPort == NULL)
		return RT_ERR_NULL_POINTER;
	
	if (pGroup == NULL)
		return RT_ERR_NULL_POINTER;

	if ((retVal = reg16_field_read(unit, ledRemapReg[LEDPinNo], ledRemapField[LEDPinNo], &regData)) != RT_ERR_OK)
		return retVal;

	phyPort = (regData / 3);
	*pPort = rtksw_switch_port_P2L_get(unit, phyPort);
	*pGroup = (rtksw_led_group_t)(regData % 3);

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_parallelLEDState_set
 * Description:
 *      This API configure parallel LED state
 * Input:
 *      unit        - Unit ID
 *      LEDPinNo    - LED pin number
 *      state       - enable or disable
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_led_parallelLEDState_set(rtksw_uint32 unit, rtksw_uint32 LEDPinNo, rtksw_enable_t state)
{
	rtksw_uint32 ledStateReg[] = 
	{
		RTL8371C_LED_IO_MAPPING1r,
		RTL8371C_LED_IO_MAPPING1r,
		RTL8371C_LED_IO_MAPPING1r,
		RTL8371C_LED_IO_MAPPING1r,
		RTL8371C_LED_IO_MAPPING1r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r
	};

	rtksw_uint32 ledStateField[] = 
	{
		RTL8371C_CFG_LED_PARA_EN_X0f,
		RTL8371C_CFG_LED_PARA_EN_X1f,
		RTL8371C_CFG_LED_PARA_EN_X2f,
		RTL8371C_CFG_LED_PARA_EN_X3f,
		RTL8371C_CFG_LED_PARA_EN_X4f,
		RTL8371C_CFG_LED_PARA_EN_X5f,
		RTL8371C_CFG_LED_PARA_EN_X6f,
		RTL8371C_CFG_LED_PARA_EN_X7f,
		RTL8371C_CFG_LED_PARA_EN_X8f,
		RTL8371C_CFG_LED_PARA_EN_X9f,
		RTL8371C_CFG_LED_PARA_EN_X10f,
		RTL8371C_CFG_LED_PARA_EN_X11f,
		RTL8371C_CFG_LED_PARA_EN_X12f,
		RTL8371C_CFG_LED_PARA_EN_X13f,
		RTL8371C_CFG_LED_PARA_EN_X14f,
		RTL8371C_CFG_LED_PARA_EN_X15f,
		RTL8371C_CFG_LED_PARA_EN_X16f,
		RTL8371C_CFG_LED_PARA_EN_X17f,
		RTL8371C_CFG_LED_PARA_EN_X18f,
		RTL8371C_CFG_LED_PARA_EN_X19f,
		RTL8371C_CFG_LED_PARA_EN_X20f
	};

    rtksw_api_ret_t retVal;
	rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if (LEDPinNo > RTL8371C_MAX_LED_PIN_NO)
		return RT_ERR_INPUT;

	if (state >= RTKSW_ENABLE_END)
		return RT_ERR_INPUT;

	regData = (state == RTKSW_ENABLED) ? 1 : 0;
	if ((retVal = reg16_field_write(unit, ledStateReg[LEDPinNo], ledStateField[LEDPinNo], &regData)) != RT_ERR_OK)
		return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_led_parallelLEDState_get
 * Description:
 *      This API configure parallel LED state
 * Input:
 *      unit        - Unit ID
 *      LEDPinNo    - LED pin number
 * Output:
 *      pState      - enable or disable.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_led_parallelLEDState_get(rtksw_uint32 unit, rtksw_uint32 LEDPinNo, rtksw_enable_t *pState)
{
    rtksw_uint32 ledStateReg[] = 
	{
		RTL8371C_LED_IO_MAPPING1r,
		RTL8371C_LED_IO_MAPPING1r,
		RTL8371C_LED_IO_MAPPING1r,
		RTL8371C_LED_IO_MAPPING1r,
		RTL8371C_LED_IO_MAPPING1r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r,
		RTL8371C_LED_IO_MAPPING0r
	};

	rtksw_uint32 ledStateField[] = 
	{
		RTL8371C_CFG_LED_PARA_EN_X0f,
		RTL8371C_CFG_LED_PARA_EN_X1f,
		RTL8371C_CFG_LED_PARA_EN_X2f,
		RTL8371C_CFG_LED_PARA_EN_X3f,
		RTL8371C_CFG_LED_PARA_EN_X4f,
		RTL8371C_CFG_LED_PARA_EN_X5f,
		RTL8371C_CFG_LED_PARA_EN_X6f,
		RTL8371C_CFG_LED_PARA_EN_X7f,
		RTL8371C_CFG_LED_PARA_EN_X8f,
		RTL8371C_CFG_LED_PARA_EN_X9f,
		RTL8371C_CFG_LED_PARA_EN_X10f,
		RTL8371C_CFG_LED_PARA_EN_X11f,
		RTL8371C_CFG_LED_PARA_EN_X12f,
		RTL8371C_CFG_LED_PARA_EN_X13f,
		RTL8371C_CFG_LED_PARA_EN_X14f,
		RTL8371C_CFG_LED_PARA_EN_X15f,
		RTL8371C_CFG_LED_PARA_EN_X16f,
		RTL8371C_CFG_LED_PARA_EN_X17f,
		RTL8371C_CFG_LED_PARA_EN_X18f,
		RTL8371C_CFG_LED_PARA_EN_X19f,
		RTL8371C_CFG_LED_PARA_EN_X20f
	};

    rtksw_api_ret_t retVal;
	rtksw_uint32 regData;

	/* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

	if (LEDPinNo > RTL8371C_MAX_LED_PIN_NO)
		return RT_ERR_INPUT;

	if (pState == NULL)
		return RT_ERR_NULL_POINTER;

	if ((retVal = reg16_field_read(unit, ledStateReg[LEDPinNo], ledStateField[LEDPinNo], &regData)) != RT_ERR_OK)
		return retVal;

	*pState = (regData == 1) ? RTKSW_ENABLED : RTKSW_DISABLED;
    return RT_ERR_OK;
}