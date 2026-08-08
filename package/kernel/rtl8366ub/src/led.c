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
#include <led.h>
#include <dal/dal_mgmts.h>

/* Function Name:
 *      rtksw_led_enable_set
 * Description:
 *      Set Led enable congiuration
 * Input:
 *      unit        - Unit ID
 *      group       - LED group id.
 *      pPortmask   - LED enable port mask.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_MASK    - Error portmask
 * Note:
 *      The API can be used to enable LED per port per group.
 */
rtksw_api_ret_t rtksw_led_enable_set(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_portmask_t *pPortmask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_enable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_enable_set(unit, group, pPortmask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_enable_get
 * Description:
 *      Get Led enable congiuration
 * Input:
 *      unit        - Unit ID
 *      group       - LED group id.
 * Output:
 *      pPortmask - LED enable port mask.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can be used to get LED enable status.
 */
rtksw_api_ret_t rtksw_led_enable_get(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_portmask_t *pPortmask)
{
     rtksw_api_ret_t retVal;

     RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_enable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_enable_get(unit, group, pPortmask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_operation_set
 * Description:
 *      Set Led operation mode
 * Input:
 *      unit        - Unit ID
 *      mode        - LED operation mode.
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
rtksw_api_ret_t rtksw_led_operation_set(rtksw_uint32 unit, rtksw_led_operation_t mode)
{
     rtksw_api_ret_t retVal;

     RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_operation_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_operation_set(unit, mode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_operation_get
 * Description:
 *      Get Led operation mode
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pMode       - Support LED operation mode.
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
rtksw_api_ret_t rtksw_led_operation_get(rtksw_uint32 unit, rtksw_led_operation_t *pMode)
{
     rtksw_api_ret_t retVal;

     RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_operation_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_operation_get(unit, pMode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_modeForce_set
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
rtksw_api_ret_t rtksw_led_modeForce_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_led_group_t group, rtksw_led_force_mode_t mode)
{
     rtksw_api_ret_t retVal;

     RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_modeForce_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_modeForce_set(unit, port, group, mode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_modeForce_get
 * Description:
 *      Get Led group to congiuration force mode
 * Input:
 *      unit    - Unit ID
 *      port    - port ID
 *      group   - Support LED group id.
 *      pMode   - Support LED force mode.
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
rtksw_api_ret_t rtksw_led_modeForce_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_led_group_t group, rtksw_led_force_mode_t *pMode)
{
     rtksw_api_ret_t retVal;

     RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_modeForce_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_modeForce_get(unit, port, group, pMode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_blinkRate_set
 * Description:
 *      Set LED blinking rate
 * Input:
 *      unit        - Unit ID
 *      blinkRate   - blinking rate.
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
rtksw_api_ret_t rtksw_led_blinkRate_set(rtksw_uint32 unit, rtksw_led_blink_rate_t blinkRate)
{
     rtksw_api_ret_t retVal;

     RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_blinkRate_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_blinkRate_set(unit, blinkRate);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_blinkRate_get
 * Description:
 *      Get LED blinking rate at mode 0 to mode 3
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pBlinkRate  - blinking rate.
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
rtksw_api_ret_t rtksw_led_blinkRate_get(rtksw_uint32 unit, rtksw_led_blink_rate_t *pBlinkRate)
{
     rtksw_api_ret_t retVal;

     RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_blinkRate_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_blinkRate_get(unit, pBlinkRate);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_groupConfig_set
 * Description:
 *      Set per group Led to congiuration mode
 * Input:
 *      unit        - Unit ID
 *      group       - LED group.
 *      config      - LED configuration
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
rtksw_api_ret_t rtksw_led_groupConfig_set(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_led_config_t config)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_groupConfig_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_groupConfig_set(unit, group, config);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_groupConfig_get
 * Description:
 *      Get Led group congiuration mode
 * Input:
 *      unit    - Unit ID
 *      group   - LED group.
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
rtksw_api_ret_t rtksw_led_groupConfig_get(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_led_config_t *pConfig)
{
     rtksw_api_ret_t retVal;

     RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_groupConfig_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_groupConfig_get(unit, group, pConfig);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_groupAbility_set
 * Description:
 *      Configure per group Led ability
 * Input:
 *      unit        - Unit ID
 *      group       - LED group.
 *      pAbility    - LED ability
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

rtksw_api_ret_t rtksw_led_groupAbility_set(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_led_ability_t *pAbility)
{
     rtksw_api_ret_t retVal;

     RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_groupAbility_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_groupAbility_set(unit, group, pAbility);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_groupAbility_get
 * Description:
 *      Get per group Led ability
 * Input:
 *      unit        - Unit ID
 *      group       - LED group.
 *      pAbility    - LED ability
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

rtksw_api_ret_t rtksw_led_groupAbility_get(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_led_ability_t *pAbility)
{
     rtksw_api_ret_t retVal;

     RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_groupAbility_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_groupAbility_get(unit, group, pAbility);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}


/* Function Name:
 *      rtksw_led_serialMode_set
 * Description:
 *      Set Led serial mode active congiuration
 * Input:
 *      unit    - Unit ID
 *      active  - LED group.
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
rtksw_api_ret_t rtksw_led_serialMode_set(rtksw_uint32 unit, rtksw_led_active_t active)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_serialMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_serialMode_set(unit, active);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_serialMode_get
 * Description:
 *      Get Led group congiuration mode
 * Input:
 *      unit    - Unit ID
 *      group   - LED group.
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
rtksw_api_ret_t rtksw_led_serialMode_get(rtksw_uint32 unit, rtksw_led_active_t *pActive)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_serialMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_serialMode_get(unit, pActive);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_OutputEnable_set
 * Description:
 *      This API set LED I/O state.
 * Input:
 *      unit        - Unit ID
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
rtksw_api_ret_t rtksw_led_OutputEnable_set(rtksw_uint32 unit, rtksw_enable_t state)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_OutputEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_OutputEnable_set(unit, state);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_OutputEnable_get
 * Description:
 *      This API get LED I/O state.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pEnabled    - LED I/O state
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      This API set current LED I/O  state.
 */
rtksw_api_ret_t rtksw_led_OutputEnable_get(rtksw_uint32 unit, rtksw_enable_t *pState)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_OutputEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_OutputEnable_get(unit, pState);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_serialModePortmask_set
 * Description:
 *      This API configure Serial LED output Group and portmask
 * Input:
 *      unit        - Unit ID
 *      output      - output group
 *      pPortmask   - output portmask
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
rtksw_api_ret_t rtksw_led_serialModePortmask_set(rtksw_uint32 unit, rtksw_led_serialOutput_t output, rtksw_portmask_t *pPortmask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_serialModePortmask_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_serialModePortmask_set(unit, output, pPortmask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_serialModePortmask_get
 * Description:
 *      This API get Serial LED output Group and portmask
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pOutput     - output group
 *      pPortmask   - output portmask
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      None.
 */
rtksw_api_ret_t rtksw_led_serialModePortmask_get(rtksw_uint32 unit, rtksw_led_serialOutput_t *pOutput, rtksw_portmask_t *pPortmask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_serialModePortmask_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_serialModePortmask_get(unit, pOutput, pPortmask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_parallelLEDRemap_set
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
rtksw_api_ret_t rtksw_led_parallelLEDRemap_set(rtksw_uint32 unit, rtksw_uint32 LEDPinNo, rtksw_port_t port, rtksw_led_group_t group)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_parallelLEDRemap_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_parallelLEDRemap_set(unit, LEDPinNo, port, group);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_parallelLEDRemap_set
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
rtksw_api_ret_t rtksw_led_parallelLEDRemap_get(rtksw_uint32 unit, rtksw_uint32 LEDPinNo, rtksw_port_t *pPort, rtksw_led_group_t *pGroup)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_parallelLEDRemap_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_parallelLEDRemap_get(unit, LEDPinNo, pPort, pGroup);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_parallelLEDState_set
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
rtksw_api_ret_t rtksw_led_parallelLEDState_set(rtksw_uint32 unit, rtksw_uint32 LEDPinNo, rtksw_enable_t state)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_parallelLEDState_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_parallelLEDState_set(unit, LEDPinNo, state);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_led_parallelLEDState_get
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
rtksw_api_ret_t rtksw_led_parallelLEDState_get(rtksw_uint32 unit, rtksw_uint32 LEDPinNo, rtksw_enable_t *pState)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->led_parallelLEDState_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->led_parallelLEDState_get(unit, LEDPinNo, pState);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}