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
#include <gpio.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      rtksw_gpio_input_get
 * Description:
 *      Get gpio input
 * Input:
 *      unit        - Unit ID
 *      pin 		- GPIO pin
 * Output:
 *      pInput 		- GPIO input
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE 	- input out of range.
 *      RT_ERR_NULL_POINTER 	- input parameter is null pointer.
 * Note:
 *      None
 */
rtksw_api_ret_t rtksw_gpio_input_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_uint32 *pInput)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->gpio_input_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->gpio_input_get(unit, pin, pInput);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_gpio_output_set
 * Description:
 *      Set GPIO output value.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 *      output  - 1 or 0
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT         - Invalid input parameter.
 *      RT_ERR_OUT_OF_RANGE  - input parameter out of range.
 * Note:
 *      The API can set GPIO pin output 1 or 0.
 */
rtksw_api_ret_t rtksw_gpio_output_set(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_uint32 output)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->gpio_output_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->gpio_output_set(unit, pin, output);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_gpio_output_get
 * Description:
 *      Get GPIO output.
 * Input:
 *      unit        - Unit ID
 *      pin         - GPIO pin
 * Output:
 *      pOutput     - GPIO output
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range.
 *      RT_ERR_NULL_POINTER 	- input parameter is null pointer.
 * Note:
 *      The API can get GPIO output.
 */
rtksw_api_ret_t rtksw_gpio_output_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_uint32 *pOutput)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->gpio_output_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->gpio_output_get(unit, pin, pOutput);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_gpio_state_set
 * Description:
 *      Set GPIO control.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 *      state   - GPIO enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT         - Invalid input parameter.
 *      RT_ERR_OUT_OF_RANGE  - input parameter out of range.
 *      RT_ERR_ENABLE        - invalid enable parameter .
 * Note:
 *      The API can set GPIO pin output 1 or 0.
 */
rtksw_api_ret_t rtksw_gpio_state_set(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_enable_t state)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->gpio_state_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->gpio_state_set(unit, pin, state);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_gpio_state_get
 * Description:
 *      Get GPIO enable state.
 * Input:
 *      unit     - Unit ID
 *      pin      - GPIO pin
 * Output:
 *      pState   - GPIO state
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range.
 *      RT_ERR_NULL_POINTER 	- input parameter is null pointer.
 * Note:
 *      The API can get GPIO enable state.
 */
rtksw_api_ret_t rtksw_gpio_state_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_enable_t *pState)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->gpio_state_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->gpio_state_get(unit, pin, pState);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_gpio_mode_set
 * Description:
 *      Set GPIO mode.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 *      mode    - Output or input mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT         - Invalid input parameter.
 *      RT_ERR_OUT_OF_RANGE  - input parameter out of range.
 * Note:
 *      The API can set GPIO  to input or output mode.
 */
rtksw_api_ret_t rtksw_gpio_mode_set(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_gpio_mode_t mode)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->gpio_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->gpio_mode_set(unit, pin, mode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_gpio_mode_get
 * Description:
 *      Get GPIO mode.
 * Input:
 *      unit     - Unit ID
 *      pin      - GPIO pin
 * Output:
 *      pMode    - GPIO mode
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range.
  *      RT_ERR_NULL_POINTER 	- input parameter is null pointer.
 * Note:
 *      The API can get GPIO mode.
 */
rtksw_api_ret_t rtksw_gpio_mode_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_gpio_mode_t *pMode)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->gpio_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->gpio_mode_get(unit, pin, pMode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_gpio_source_set
 * Description:
 *      Set GPIO mode.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 *      mode    - GPIO source mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT         - Invalid input parameter.
 *      RT_ERR_OUT_OF_RANGE  - input parameter out of range.
 * Note:
 *      The API can set GPIO source mode.
 */
rtksw_api_ret_t rtksw_gpio_source_set(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_gpio_source_t mode)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->gpio_source_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->gpio_source_set(unit, pin, mode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_gpio_source_get
 * Description:
 *      Get GPIO mode.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 * Output:
 *      pMode   - GPIO source mode
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range.
  *      RT_ERR_NULL_POINTER 	- input parameter is null pointer.
 * Note:
 *      The API can get GPIO source mode.
 */
rtksw_api_ret_t rtksw_gpio_source_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_gpio_source_t *pMode)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->gpio_source_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->gpio_source_get(unit, pin, pMode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_gpio_aclEnClear_set
 * Description:
 *      Set GPIO acl clear.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT         - Invalid input parameter.
 *      RT_ERR_OUT_OF_RANGE  - input parameter out of range.
 * Note:
 *      The API can set GPIO ACL clear.
 */
rtksw_api_ret_t rtksw_gpio_aclEnClear_set(rtksw_uint32 unit, rtksw_uint32 pin)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->gpio_aclEnClear_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->gpio_aclEnClear_set(unit, pin);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_gpio_aclEnClear_get
 * Description:
 *      Get GPIO acl clear.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 * Output:
 *      pAclEn  - GPIO acl enable
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range.
 *      RT_ERR_NULL_POINTER 	- input parameter is null pointer.
 * Note:
 *      The API can get GPIO acl enable clear.
 */
rtksw_api_ret_t rtksw_gpio_aclEnClear_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_enable_t *pAclEn)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->gpio_aclEnClear_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->gpio_aclEnClear_get(unit, pin, pAclEn);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

