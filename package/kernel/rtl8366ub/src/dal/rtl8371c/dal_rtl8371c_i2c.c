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
#include <dal/rtl8371c/dal_rtl8371c_i2c.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>

//static rtksw_I2C_16bit_mode_t rtksw_i2c_mode = I2C_LSB_16BIT_MODE;


/* Function Name:
 *      dal_rtl8371c_i2c_init
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
rtksw_api_ret_t dal_rtl8371c_i2c_init(rtksw_uint32 unit)
{
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      dal_rtl8371c_i2c_data_read
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
rtksw_api_ret_t dal_rtl8371c_i2c_data_read(rtksw_uint32 unit, rtksw_uint8 deviceAddr, rtksw_uint32 slaveRegAddr, rtksw_uint32 *pRegData)
{
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (deviceAddr || slaveRegAddr)
    { 
        /* TBD, Skip warning */
    }

    if (pRegData == NULL)
        return RT_ERR_NULL_POINTER;

    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      dal_rtl8371c_i2c_data_write
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
rtksw_api_ret_t dal_rtl8371c_i2c_data_write(rtksw_uint32 unit, rtksw_uint8 deviceAddr, rtksw_uint32 slaveRegAddr, rtksw_uint32 regData)
{
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (deviceAddr || slaveRegAddr || regData)
    { 
        /* TBD, Skip warning */
    }
   
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      dal_rtl8371c_i2c_mode_set
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
rtksw_api_ret_t dal_rtl8371c_i2c_mode_set(rtksw_uint32 unit, rtksw_I2C_16bit_mode_t i2cmode )
{
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(i2cmode >= I2C_Mode_END)
        return RT_ERR_INPUT;

    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      dal_rtl8371c_i2c_mode_get
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
rtksw_api_ret_t dal_rtl8371c_i2c_mode_get(rtksw_uint32 unit, rtksw_I2C_16bit_mode_t * pI2cMode)
{
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pI2cMode)
        return RT_ERR_NULL_POINTER;

    return RT_ERR_CHIP_NOT_SUPPORTED;
}

