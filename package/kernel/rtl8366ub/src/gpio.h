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

#ifndef __RTKSW_API_GPIO_H__
#define __RTKSW_API_GPIO_H__

/*
 * Data Type Declaration
 */
typedef enum rtksw_gpio_mode_e
{
    GPIO_MODE_OUTPUT = 0,
    GPIO_MODE_INPUT,
    GPIO_MODE_END
} rtksw_gpio_mode_t;

typedef enum rtksw_gpio_source_e
{
    GPIO_SOURCE_CPU = 0,
    GPIO_SOURCE_8051,
    GPIO_SOURCE_END
} rtksw_gpio_source_t;

/* Function Name:
 *      rtksw_gpio_input_get
 * Description:
 *      Get gpio input
 * Input:
 *      unit    - Unit ID
 *      pin 	- GPIO pin
 * Output:
 *      pInput 	- GPIO input
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_OUT_OF_RANGE 	- input out of range.
 *      RT_ERR_NULL_POINTER 	- input parameter is null pointer.
 * Note:
 *      None
 */
extern rtksw_api_ret_t rtksw_gpio_input_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_uint32 *pInput);

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
extern rtksw_api_ret_t rtksw_gpio_output_set(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_uint32 output);

/* Function Name:
 *      rtksw_gpio_output_get
 * Description:
 *      Get GPIO output.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 * Output:
 *      pOutput - GPIO output
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
extern rtksw_api_ret_t rtksw_gpio_output_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_uint32 *pOutput);

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
extern rtksw_api_ret_t rtksw_gpio_state_set(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_enable_t state);

/* Function Name:
 *      rtksw_gpio_state_get
 * Description:
 *      Get GPIO enable state.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 * Output:
 *      pState  - GPIO enable
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
extern rtksw_api_ret_t rtksw_gpio_state_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_enable_t *pState);

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
extern rtksw_api_ret_t rtksw_gpio_mode_set(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_gpio_mode_t mode);

/* Function Name:
 *      rtksw_gpio_mode_get
 * Description:
 *      Get GPIO mode.
 * Input:
 *      unit    - Unit ID
 *      pin     - GPIO pin
 * Output:
 *      pMode   - GPIO mode
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
extern rtksw_api_ret_t rtksw_gpio_mode_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_gpio_mode_t *pMode);

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
extern rtksw_api_ret_t rtksw_gpio_source_set(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_gpio_source_t mode);

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
extern rtksw_api_ret_t rtksw_gpio_source_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_gpio_source_t *pMode);

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
extern rtksw_api_ret_t rtksw_gpio_aclEnClear_set(rtksw_uint32 unit, rtksw_uint32 pin);

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
extern rtksw_api_ret_t rtksw_gpio_aclEnClear_get(rtksw_uint32 unit, rtksw_uint32 pin, rtksw_enable_t *pAclEn);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_gpio_input_get(pin, pInput)         rtksw_gpio_input_get(0, pin, pInput)
#define rtk_gpio_output_set(pin, output)        rtksw_gpio_output_set(0, pin, output)
#define rtk_gpio_output_get(pin, pOutput)       rtksw_gpio_output_get(0, pin, pOutput)
#define rtk_gpio_state_set(pin, state)          rtksw_gpio_state_set(0, pin, state)
#define rtk_gpio_state_get(pin, pState)         rtksw_gpio_state_get(0, pin, pState)
#define rtk_gpio_mode_set(pin, mode)            rtksw_gpio_mode_set(0, pin, mode)
#define rtk_gpio_mode_get(pin, pMode)           rtksw_gpio_mode_get(0, pin, pMode)
#define rtk_gpio_source_set(pin, mode)          rtksw_gpio_source_set(0, pin, mode)
#define rtk_gpio_source_get(pin, pMode)         rtksw_gpio_source_get(0, pin, pMode)
#define rtk_gpio_aclEnClear_set(pin)            rtksw_gpio_aclEnClear_set(0, pin)
#define rtk_gpio_aclEnClear_get(pin, pAclEn)    rtksw_gpio_aclEnClear_get(0, pin, pAclEn)

#define rtk_gpio_mode_t     rtksw_gpio_mode_t
#define rtk_gpio_source_t   rtksw_gpio_source_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /*__RTKSW_API_GPIO_H__*/