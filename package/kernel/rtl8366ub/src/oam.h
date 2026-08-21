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

#ifndef __RTKSW_OAM_H__
#define __RTKSW_OAM_H__

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */

typedef enum rtksw_oam_parser_act_e
{
    OAM_PARSER_ACTION_FORWARD = 0,
    OAM_PARSER_ACTION_LOOPBACK,
    OAM_PARSER_ACTION_DISCARD,
    OAM_PARSER_ACTION_END,

} rtksw_oam_parser_act_t;

typedef enum rtksw_oam_multiplexer_act_e
{
    OAM_MULTIPLEXER_ACTION_FORWARD = 0,
    OAM_MULTIPLEXER_ACTION_DISCARD,
    OAM_MULTIPLEXER_ACTION_CPUONLY,
    OAM_MULTIPLEXER_ACTION_END,

} rtksw_oam_multiplexer_act_t;


/*
 * Function Declaration
 */

/* Function Name:
 *      rtksw_oam_init
 * Description:
 *      Initialize oam module.
 * Input:
 *      unit     - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize oam module before calling any oam APIs.
 */
extern rtksw_api_ret_t rtksw_oam_init(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_oam_state_set
 * Description:
 *      This API set OAM state.
 * Input:
 *      unit     - Unit ID
 *      enabled  - OAM state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      This API set OAM state.
 */
extern rtksw_api_ret_t rtksw_oam_state_set(rtksw_uint32 unit, rtksw_enable_t enabled);

/* Function Name:
 *      rtksw_oam_state_get
 * Description:
 *      This API get OAM state.
 * Input:
 *      unit     - Unit ID
 * Output:
 *      pEnabled        - OAM state
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      This API set current OAM state.
 */
extern rtksw_api_ret_t rtksw_oam_state_get(rtksw_uint32 unit, rtksw_enable_t *pEnabled);

/* Function Name:
 *      rtksw_oam_parserAction_set
 * Description:
 *      Set OAM parser action
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      action  - parser action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern rtksw_api_ret_t  rtksw_oam_parserAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_oam_parser_act_t action);

/* Function Name:
 *      rtksw_oam_parserAction_set
 * Description:
 *      Get OAM parser action
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 * Output:
 *      pAction  - parser action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern rtksw_api_ret_t  rtksw_oam_parserAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_oam_parser_act_t *pAction);

/* Function Name:
 *      rtksw_oam_multiplexerAction_set
 * Description:
 *      Set OAM multiplexer action
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 *      action  - parser action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern rtksw_api_ret_t  rtksw_oam_multiplexerAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_oam_multiplexer_act_t action);

/* Function Name:
 *      rtksw_oam_parserAction_set
 * Description:
 *      Get OAM multiplexer action
 * Input:
 *      unit    - Unit ID
 *      port    - port id
 * Output:
 *      pAction  - parser action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern rtksw_api_ret_t  rtksw_oam_multiplexerAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_oam_multiplexer_act_t *pAction);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_oam_init(void)                              rtksw_oam_init(0)
#define rtk_oam_state_set(enabled)                      rtksw_oam_state_set(0, enabled)
#define rtk_oam_state_get(pEnabled)                     rtksw_oam_state_get(0, pEnabled)
#define rtk_oam_parserAction_set(port, action)          rtksw_oam_parserAction_set(0, port, action)
#define rtk_oam_parserAction_get(port, pAction)         rtksw_oam_parserAction_get(0, port, pAction)
#define rtk_oam_multiplexerAction_set(port, action)     rtksw_oam_multiplexerAction_set(0, port, action)
#define rtk_oam_multiplexerAction_get(port, pAction)    rtksw_oam_multiplexerAction_get(0, port, pAction)

#define rtk_oam_parser_act_t        rtksw_oam_parser_act_t
#define rtk_oam_multiplexer_act_t   rtksw_oam_multiplexer_act_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_OAM_H__ */

