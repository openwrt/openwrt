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
#include <oam.h>

#include <dal/dal_mgmts.h>

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
rtksw_api_ret_t rtksw_oam_init(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->oam_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->oam_init(unit);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}


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
rtksw_api_ret_t rtksw_oam_state_set(rtksw_uint32 unit, rtksw_enable_t enabled)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->oam_state_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->oam_state_set(unit, enabled);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

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
rtksw_api_ret_t rtksw_oam_state_get(rtksw_uint32 unit, rtksw_enable_t *pEnabled)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->oam_state_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->oam_state_get(unit, pEnabled);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}



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
rtksw_api_ret_t  rtksw_oam_parserAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_oam_parser_act_t action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->oam_parserAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->oam_parserAction_set(unit, port, action);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

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
rtksw_api_ret_t  rtksw_oam_parserAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_oam_parser_act_t *pAction)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->oam_parserAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->oam_parserAction_get(unit, port, pAction);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}


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
rtksw_api_ret_t  rtksw_oam_multiplexerAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_oam_multiplexer_act_t action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->oam_multiplexerAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->oam_multiplexerAction_set(unit, port, action);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

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
rtksw_api_ret_t  rtksw_oam_multiplexerAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_oam_multiplexer_act_t *pAction)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->oam_multiplexerAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->oam_multiplexerAction_get(unit, port, pAction);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}


