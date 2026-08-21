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

#ifndef __RTKSW_API_I2C_H__
#define __RTKSW_API_I2C_H__
#include <rtk_types.h>

#define I2C_GPIO_MAX_GROUP (3)

typedef enum rtksw_I2C_16bit_mode_e{
    I2C_LSB_16BIT_MODE = 0,
    I2C_70B_LSB_16BIT_MODE,
    I2C_Mode_END
}rtksw_I2C_16bit_mode_t;


typedef enum rtksw_I2C_gpio_pin_e{
    I2C_GPIO_PIN_8_9 = 0,
    I2C_GPIO_PIN_15_16 ,
    I2C_GPIO_PIN_35_36 ,
    I2C_GPIO_PIN_END
}rtksw_I2C_gpio_pin_t;


/* Function Name:
 *      rtksw_i2c_data_read
 * Description:
 *      read i2c slave device register.
 * Input:
 *      unit            - Unit ID
 *      deviceAddr      - access Slave device address
 *      slaveRegAddr    - access Slave register address
 * Output:
 *      pRegData     -   read data
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_NULL_POINTER     - input parameter is null pointer
 * Note:
 *      The API can access i2c slave and read i2c slave device register.
 */
extern rtksw_api_ret_t rtksw_i2c_data_read(rtksw_uint32 unit, rtksw_uint8 deviceAddr, rtksw_uint32 slaveRegAddr, rtksw_uint32 *pRegData);

/* Function Name:
 *      rtksw_i2c_data_write
 * Description:
 *      write data to i2c slave device register
 * Input:
 *      unit            - Unit ID
 *      deviceAddr      - access Slave device address
 *      slaveRegAddr    - access Slave register address
 *      regData         - data to set
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 * Note:
 *      The API can access i2c slave and setting i2c slave device register.
 */
extern rtksw_api_ret_t rtksw_i2c_data_write(rtksw_uint32 unit, rtksw_uint8 deviceAddr, rtksw_uint32 slaveRegAddr, rtksw_uint32 regData);


/* Function Name:
 *      rtksw_i2c_init
 * Description:
 *      I2C smart function initialization.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      This API is used to initialize EEE status.
 *      need used GPIO pins
 *      OpenDrain and clock
 */
extern rtksw_api_ret_t rtksw_i2c_init(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_i2c_mode_set
 * Description:
 *      Set I2C data byte-order.
 * Input:
 *      unit    - Unit ID
 *      i2cmode - byte-order mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      This API can set I2c traffic's byte-order .
 */
extern rtksw_api_ret_t rtksw_i2c_mode_set(rtksw_uint32 unit, rtksw_I2C_16bit_mode_t i2cmode);

/* Function Name:
 *      rtksw_i2c_mode_get
 * Description:
 *      Get i2c traffic byte-order setting.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pI2cMode - i2c byte-order
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_NULL_POINTER     - input parameter is null pointer
 * Note:
 *      The API can get i2c traffic byte-order setting.
 */
extern rtksw_api_ret_t rtksw_i2c_mode_get(rtksw_uint32 unit, rtksw_I2C_16bit_mode_t * pI2cMode);


/* Function Name:
 *      rtksw_i2c_gpioPinGroup_set
 * Description:
 *      Set i2c SDA & SCL used GPIO pins group.
 * Input:
 *      unit        - Unit ID
 *      pins_group  - GPIO pins group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      The API can set i2c used gpio pins group.
 *      There are three group pins could be used
 */
extern rtksw_api_ret_t rtksw_i2c_gpioPinGroup_set(rtksw_uint32 unit, rtksw_I2C_gpio_pin_t pins_group);

/* Function Name:
 *      rtksw_i2c_gpioPinGroup_get
 * Description:
 *      Get i2c SDA & SCL used GPIO pins group.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pPins_group - GPIO pins group
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_NULL_POINTER     - input parameter is null pointer
 * Note:
 *      The API can get i2c used gpio pins group.
 *      There are three group pins could be used
 */
extern rtksw_api_ret_t rtksw_i2c_gpioPinGroup_get(rtksw_uint32 unit, rtksw_I2C_gpio_pin_t * pPins_group);


#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_i2c_data_read(deviceAddr, slaveRegAddr, pRegData)   rtksw_i2c_data_read(0, deviceAddr, slaveRegAddr, pRegData)
#define rtk_i2c_data_write(deviceAddr, slaveRegAddr, regData)   rtksw_i2c_data_write(0, deviceAddr, slaveRegAddr, regData)
#define rtk_i2c_init(void)                                      rtksw_i2c_init(0)
#define rtk_i2c_mode_set(i2cmode)                               rtksw_i2c_mode_set(0, i2cmode)
#define rtk_i2c_mode_get(pI2cMode)                              rtksw_i2c_mode_get(0, pI2cMode)
#define rtk_i2c_gpioPinGroup_set(pins_group)                    rtksw_i2c_gpioPinGroup_set(0, pins_group)
#define rtk_i2c_gpioPinGroup_get(pPins_group)                   rtksw_i2c_gpioPinGroup_get(0, pPins_group)

#define rtk_I2C_16bit_mode_t    rtksw_I2C_16bit_mode_t
#define rtk_I2C_gpio_pin_t      rtksw_I2C_gpio_pin_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif

