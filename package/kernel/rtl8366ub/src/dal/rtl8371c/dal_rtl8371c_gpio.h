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

#ifndef __DAL_RTL8371C_GPIO_H__
#define __DAL_RTL8371C_GPIO_H__

/*
 * Include Files
 */
#include <gpio.h>

#define RTL8371C_GPIOPINNO                  26
#define RTL8371C_GPIOPINMAX                 (RTL8371C_GPIOPINNO-1)


/* Function Name:
 *      dal_rtl8371c_gpio_input_get
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
extern rtksw_api_ret_t dal_rtl8371c_gpio_input_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_uint32 *pInput);

/* Function Name:
 *      dal_rtl8371c_gpio_output_set
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
extern rtksw_api_ret_t dal_rtl8371c_gpio_output_set(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_uint32 output);

/* Function Name:
 *      dal_rtl8371c_gpio_output_get
 * Description:
 *      Get GPIO output.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 * Output:
 *      pOutput  - GPIO output
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
extern rtksw_api_ret_t dal_rtl8371c_gpio_output_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_uint32 *pOutput);

/* Function Name:
 *      dal_rtl8371c_gpio_state_set
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
 *      The API can set GPIO state.
 */
extern rtksw_api_ret_t dal_rtl8371c_gpio_state_set(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_enable_t state);

/* Function Name:
 *      dal_rtl8371c_gpio_state_get
 * Description:
 *      Get GPIO enable state.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 * Output:
 *      pState   - GPIO enable
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range.
 *      RT_ERR_NULL_POINTER 	- input parameter is null pointer.
 * Note:
 *      The API can get GPIO state.
 */
extern rtksw_api_ret_t dal_rtl8371c_gpio_state_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_enable_t *pState);

/* Function Name:
 *      dal_rtl8371c_gpio_mode_set
 * Description:
 *      Set GPIO mode.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 *      mode    - 1 or 0
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
extern rtksw_api_ret_t dal_rtl8371c_gpio_mode_set(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_gpio_mode_t mode);

/* Function Name:
 *      dal_rtl8371c_gpio_mode_get
 * Description:
 *      Get GPIO mode.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
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
extern rtksw_api_ret_t dal_rtl8371c_gpio_mode_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_gpio_mode_t *pMode);

/* Function Name:
 *      dal_rtl8371c_gpio_source_set
 * Description:
 *      Set GPIO mode.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 *      mode    - 1 or 0
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
extern rtksw_api_ret_t dal_rtl8371c_gpio_source_set(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_gpio_source_t mode);


/* Function Name:
 *      dal_rtl8371c_gpio_source_get
 * Description:
 *      Get GPIO mode.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
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
extern rtksw_api_ret_t dal_rtl8371c_gpio_source_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_gpio_source_t *pMode);

/* Function Name:
 *      dal_rtl8371c_gpio_aclEnClear_set
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
extern rtksw_api_ret_t dal_rtl8371c_gpio_aclEnClear_set(rtksw_uint32 unit, rtksw_uint32 pin);

/* Function Name:
 *      dal_rtl8371c_gpio_aclEnClear_get
 * Description:
 *      Get GPIO acl clear.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 * Output:
 *      pAclEn   - GPIO acl enable
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
extern rtksw_api_ret_t dal_rtl8371c_gpio_aclEnClear_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_enable_t *pAclEn);

#endif /* __DAL_RTL8371C_GPIO_H__ */


