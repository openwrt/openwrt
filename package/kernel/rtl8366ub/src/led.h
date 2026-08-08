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

#ifndef __RTKSW_API_LED_H__
#define __RTKSW_API_LED_H__

typedef enum rtksw_led_operation_e
{
    LED_OP_SCAN=0,
    LED_OP_PARALLEL,
    LED_OP_SERIAL,
    LED_OP_END,
}rtksw_led_operation_t;


typedef enum rtksw_led_active_e
{
    RTKSW_LED_ACTIVE_HIGH=0,
    RTKSW_LED_ACTIVE_LOW,
    RTKSW_LED_ACTIVE_END,
}rtksw_led_active_t;

typedef enum rtksw_led_config_e
{
    LED_CONFIG_LEDOFF=0,
    LED_CONFIG_DUPCOL,
    LED_CONFIG_LINK_ACT,
    LED_CONFIG_SPD1000,
    LED_CONFIG_SPD100,
    LED_CONFIG_SPD10,
    LED_CONFIG_SPD1000ACT,
    LED_CONFIG_SPD100ACT,
    LED_CONFIG_SPD10ACT,
    LED_CONFIG_SPD10010ACT,
    LED_CONFIG_LOOPDETECT,
    LED_CONFIG_EEE,
    LED_CONFIG_LINKRX,
    LED_CONFIG_LINKTX,
    LED_CONFIG_MASTER,
    LED_CONFIG_ACT,
    LED_CONFIG_END,
}rtksw_led_config_t;

typedef struct rtksw_led_ability_s
{
    rtksw_enable_t link_10m;
    rtksw_enable_t link_100m;
    rtksw_enable_t link_500m;
    rtksw_enable_t link_1000m;
    rtksw_enable_t link_2500m;
    rtksw_enable_t act_rx;
    rtksw_enable_t act_tx;
}rtksw_led_ability_t;

typedef enum rtksw_led_blink_rate_e
{
    LED_BLINKRATE_32MS=0,
    LED_BLINKRATE_64MS,
    LED_BLINKRATE_128MS,
    LED_BLINKRATE_256MS,
    LED_BLINKRATE_512MS,
    LED_BLINKRATE_1024MS,
    LED_BLINKRATE_48MS,
    LED_BLINKRATE_96MS,
    LED_BLINKRATE_END,
}rtksw_led_blink_rate_t;

typedef enum rtksw_led_group_e
{
    LED_GROUP_0 = 0,
    LED_GROUP_1,
    LED_GROUP_2,
    LED_GROUP_END
}rtksw_led_group_t;


typedef enum rtksw_led_force_mode_e
{
    LED_FORCE_NORMAL=0,
    LED_FORCE_BLINK,
    LED_FORCE_OFF,
    LED_FORCE_ON,
    LED_FORCE_END
}rtksw_led_force_mode_t;

typedef enum rtksw_led_serialOutput_e
{
    SERIAL_LED_NONE = 0,
    SERIAL_LED_0,
    SERIAL_LED_0_1,
    SERIAL_LED_0_2,
    SERIAL_LED_END,
}rtksw_led_serialOutput_t;

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
extern rtksw_api_ret_t rtksw_led_enable_set(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_portmask_t *pPortmask);

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
extern rtksw_api_ret_t rtksw_led_enable_get(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_portmask_t *pPortmask);

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
extern rtksw_api_ret_t rtksw_led_operation_set(rtksw_uint32 unit, rtksw_led_operation_t mode);

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
extern rtksw_api_ret_t rtksw_led_operation_get(rtksw_uint32 unit, rtksw_led_operation_t *pMode);

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
extern rtksw_api_ret_t rtksw_led_modeForce_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_led_group_t group, rtksw_led_force_mode_t mode);

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
extern rtksw_api_ret_t rtksw_led_modeForce_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_led_group_t group, rtksw_led_force_mode_t *pMode);

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
extern rtksw_api_ret_t rtksw_led_blinkRate_set(rtksw_uint32 unit, rtksw_led_blink_rate_t blinkRate);

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
extern rtksw_api_ret_t rtksw_led_blinkRate_get(rtksw_uint32 unit, rtksw_led_blink_rate_t *pBlinkRate);

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
extern rtksw_api_ret_t rtksw_led_groupConfig_set(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_led_config_t config);

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
extern rtksw_api_ret_t rtksw_led_groupConfig_get(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_led_config_t *pConfig);

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

extern rtksw_api_ret_t rtksw_led_groupAbility_set(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_led_ability_t *pAbility);

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

extern rtksw_api_ret_t rtksw_led_groupAbility_get(rtksw_uint32 unit, rtksw_led_group_t group, rtksw_led_ability_t *pAbility);


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
extern rtksw_api_ret_t rtksw_led_serialMode_set(rtksw_uint32 unit, rtksw_led_active_t active);

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
extern rtksw_api_ret_t rtksw_led_serialMode_get(rtksw_uint32 unit, rtksw_led_active_t *pActive);

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
extern rtksw_api_ret_t rtksw_led_OutputEnable_set(rtksw_uint32 unit, rtksw_enable_t state);

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
extern rtksw_api_ret_t rtksw_led_OutputEnable_get(rtksw_uint32 unit, rtksw_enable_t *pState);

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
extern rtksw_api_ret_t rtksw_led_serialModePortmask_set(rtksw_uint32 unit, rtksw_led_serialOutput_t output, rtksw_portmask_t *pPortmask);

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
extern rtksw_api_ret_t rtksw_led_serialModePortmask_get(rtksw_uint32 unit, rtksw_led_serialOutput_t *pOutput, rtksw_portmask_t *pPortmask);

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
extern rtksw_api_ret_t rtksw_led_parallelLEDRemap_set(rtksw_uint32 unit, rtksw_uint32 LEDPinNo, rtksw_port_t port, rtksw_led_group_t group);

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
extern rtksw_api_ret_t rtksw_led_parallelLEDRemap_get(rtksw_uint32 unit, rtksw_uint32 LEDPinNo, rtksw_port_t *pPort, rtksw_led_group_t *pGroup);

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
extern rtksw_api_ret_t rtksw_led_parallelLEDState_set(rtksw_uint32 unit, rtksw_uint32 LEDPinNo, rtksw_enable_t state);

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
extern rtksw_api_ret_t rtksw_led_parallelLEDState_get(rtksw_uint32 unit, rtksw_uint32 LEDPinNo, rtksw_enable_t *pState);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_led_enable_set(group, pPortmask)                rtksw_led_enable_set(0, group, pPortmask)
#define rtk_led_enable_get(group, pPortmask)                rtksw_led_enable_get(0, group, pPortmask)
#define rtk_led_operation_set(mode)                         rtksw_led_operation_set(0, mode)
#define rtk_led_operation_get(pMode)                        rtksw_led_operation_get(0, pMode)
#define rtk_led_modeForce_set(port, group, mode)            rtksw_led_modeForce_set(0, port, group, mode)
#define rtk_led_modeForce_get(port, group, pMode)           rtksw_led_modeForce_get(0, port, group, pMode)
#define rtk_led_blinkRate_set(blinkRate)                    rtksw_led_blinkRate_set(0, blinkRate)
#define rtk_led_blinkRate_get(pBlinkRate)                   rtksw_led_blinkRate_get(0, pBlinkRate)
#define rtk_led_groupConfig_set(group, config)              rtksw_led_groupConfig_set(0, group, config)
#define rtk_led_groupConfig_get(group, pConfig)             rtksw_led_groupConfig_get(0, group, pConfig)
#define rtk_led_groupAbility_set(group, pAbility)           rtksw_led_groupAbility_set(0, group, pAbility)
#define rtk_led_groupAbility_get(group, pAbility)           rtksw_led_groupAbility_get(0, group, pAbility)
#define rtk_led_serialMode_set(active)                      rtksw_led_serialMode_set(0, active)
#define rtk_led_serialMode_get(pActive)                     rtksw_led_serialMode_get(0, pActive)
#define rtk_led_OutputEnable_set(state)                     rtksw_led_OutputEnable_set(0, state)
#define rtk_led_OutputEnable_get(pState)                    rtksw_led_OutputEnable_get(0, pState)
#define rtk_led_serialModePortmask_set(output, pPortmask)   rtksw_led_serialModePortmask_set(0, output, pPortmask)
#define rtk_led_serialModePortmask_get(pOutput, pPortmask)  rtksw_led_serialModePortmask_get(0, pOutput, pPortmask)

#define rtk_led_operation_t         rtksw_led_operation_t
#define rtk_led_active_t            rtksw_led_active_t
#define rtk_led_config_t            rtksw_led_config_t
#define rtk_led_ability_t           rtksw_led_ability_t
#define rtk_led_blink_rate_t        rtksw_led_blink_rate_t
#define rtk_led_group_t             rtksw_led_group_t
#define rtk_led_force_mode_t        rtksw_led_force_mode_t
#define rtk_led_serialOutput_t      rtksw_led_serialOutput_t

#define LED_ACTIVE_HIGH         RTKSW_LED_ACTIVE_HIGH
#define LED_ACTIVE_LOW          RTKSW_LED_ACTIVE_LOW
#define LED_ACTIVE_END          RTKSW_LED_ACTIVE_END

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_LED_H__ */
