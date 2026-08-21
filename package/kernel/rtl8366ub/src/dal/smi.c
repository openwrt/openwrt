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

#include <rtk_types.h>
#include <dal/smi.h>
#include <rtk_error.h>
#include <linux/mdio.h>
#include <linux/phy.h>

/*******************************************************************************/
/*  MDC/MDIO porting                                                           */
/*******************************************************************************/
/* define the PHY ID currently used */
#define MDC_MDIO_PHY_ID     __mdc_mdio_phy_id  /* PHY ID 0 or 29 */

static int __mdc_mdio_phy_id = 0;
static struct mii_bus *__mii_bus;

void rtk_set_mdc_mdio(struct mii_bus *bus, int id)
{
    __mii_bus = bus;
    __mdc_mdio_phy_id = id;
}

static int rtk_mdio_write(int mii_id, int regnum, rtksw_uint32 data)
{
    return __mdiobus_write(__mii_bus, mii_id, regnum, data);
}

static int rtk_mdio_read(int mii_id, int regnum, rtksw_uint32 *data)
{
    int ret = __mdiobus_read(__mii_bus, mii_id, regnum);
    if (ret < 0)
        return ret;

    *data = ret;

    return 0;
}

/* MDC/MDIO, redefine/implement the following Macro */
#define MDC_MDIO_WRITE(preamableLength, phyID, regID, data) rtk_mdio_write(phyID, regID, data)
#define MDC_MDIO_READ(preamableLength, phyID, regID, pData) rtk_mdio_read(phyID, regID, pData)


/*******************************************************************************/
/*  SPI porting                                                                */
/*******************************************************************************/
/* SPI, redefine/implement the following Macro */
#define SPI_WRITE(data, length)
#define SPI_READ(pData, length)





/*******************************************************************************/
/*  I2C porting                                                                */
/*******************************************************************************/
/* Define the GPIO ID for SCK & SDA */
rtksw_uint32  smi_SCK = 1;    /* GPIO used for SMI Clock Generation */
rtksw_uint32  smi_SDA = 2;    /* GPIO used for SMI Data signal */

/* I2C, redefine/implement the following Macro */
#define GPIO_DIRECTION_SET(gpioID, direction)
#define GPIO_DATA_SET(gpioID, data)
#define GPIO_DATA_GET(gpioID, pData)


#ifdef EASYSMART_SDK
/*******************************************************************************/
/*  SDK access from SoC                                                         */
/*******************************************************************************/
#define REG32(_addr) (*(volatile uint32_t *)(_addr))
#endif

static void rtlglue_drvMutexLock(void)
{
    mutex_lock(&__mii_bus->mdio_lock);
    /* It is empty currently. Implement this function if Lock/Unlock function is needed */
    return;
}

static void rtlglue_drvMutexUnlock(void)
{
    mutex_unlock(&__mii_bus->mdio_lock);
    /* It is empty currently. Implement this function if Lock/Unlock function is needed */
    return;
}

static void _smi_start(void)
{

    /* change GPIO pin to Output only */
    GPIO_DIRECTION_SET(smi_SCK, GPIO_DIR_OUT);
    GPIO_DIRECTION_SET(smi_SDA, GPIO_DIR_OUT);

    /* Initial state: SCK: 0, SDA: 1 */
    GPIO_DATA_SET(smi_SCK, 0);
    GPIO_DATA_SET(smi_SDA, 1);
    CLK_DURATION(DELAY);

    /* CLK 1: 0 -> 1, 1 -> 0 */
    GPIO_DATA_SET(smi_SCK, 1);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(smi_SCK, 0);
    CLK_DURATION(DELAY);

    /* CLK 2: */
    GPIO_DATA_SET(smi_SCK, 1);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(smi_SDA, 0);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(smi_SCK, 0);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(smi_SDA, 1);

}



static void _smi_writeBit(rtksw_uint16 signal, rtksw_uint32 bitLen)
{
    for( ; bitLen > 0; bitLen--)
    {
        CLK_DURATION(DELAY);

        /* prepare data */
        if ( signal & (1<<(bitLen-1)) )
        {
            GPIO_DATA_SET(smi_SDA, 1);
        }
        else
        {
            GPIO_DATA_SET(smi_SDA, 0);
        }
        CLK_DURATION(DELAY);

        /* clocking */
        GPIO_DATA_SET(smi_SCK, 1);
        CLK_DURATION(DELAY);
        GPIO_DATA_SET(smi_SCK, 0);
    }
}



static void _smi_readBit(rtksw_uint32 bitLen, rtksw_uint32 *rData)
{
    rtksw_uint32 u = 0;

    /* change GPIO pin to Input only */
    GPIO_DIRECTION_SET(smi_SDA, GPIO_DIR_IN);

    for (*rData = 0; bitLen > 0; bitLen--)
    {
        CLK_DURATION(DELAY);

        /* clocking */
        GPIO_DATA_SET(smi_SCK, 1);
        CLK_DURATION(DELAY);
        GPIO_DATA_GET(smi_SDA, &u);
        GPIO_DATA_SET(smi_SCK, 0);

        *rData |= (u << (bitLen - 1));
    }

    /* change GPIO pin to Output only */
    GPIO_DIRECTION_SET(smi_SDA, GPIO_DIR_OUT);
}



static void _smi_stop(void)
{

    CLK_DURATION(DELAY);
    GPIO_DATA_SET(smi_SDA, 0);
    GPIO_DATA_SET(smi_SCK, 1);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(smi_SDA, 1);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(smi_SCK, 1);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(smi_SCK, 0);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(smi_SCK, 1);

    /* add a click */
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(smi_SCK, 0);
    CLK_DURATION(DELAY);
    GPIO_DATA_SET(smi_SCK, 1);


    /* change GPIO pin to Input only */
    GPIO_DIRECTION_SET(smi_SDA, GPIO_DIR_IN);
    GPIO_DIRECTION_SET(smi_SCK, GPIO_DIR_IN);
}

rtksw_api_ret_t reg_i2c_read(rtksw_uint32 mAddrs, rtksw_uint32 *rData)
{
    rtksw_uint32 rawData=0, ACK;
    rtksw_uint8  con;
    rtksw_uint32 ret = RT_ERR_OK;

    if(mAddrs > 0xFFFF)
        return RT_ERR_INPUT;

    if(rData == NULL)
        return RT_ERR_NULL_POINTER;

    *rData = 0;

    /*Disable CPU interrupt to ensure that the SMI operation is atomic.
      The API is based on RTL865X, rewrite the API if porting to other platform.*/
    rtlglue_drvMutexLock();

    _smi_start();                                /* Start SMI */

    _smi_writeBit(0x0b, 4);                     /* CTRL code: 4'b1011 for RTL8370 */

    _smi_writeBit(0x4, 3);                        /* CTRL code: 3'b100 */

    _smi_writeBit(0x1, 1);                        /* 1: issue READ command */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for issuing READ command*/
    } while ((ACK != 0) && (con < ack_timer));

    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs&0xff), 8);             /* Set reg_addr[7:0] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for setting reg_addr[7:0] */
    } while ((ACK != 0) && (con < ack_timer));

    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs>>8), 8);                 /* Set reg_addr[15:8] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK by RTL8369 */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_readBit(8, &rawData);                    /* Read DATA [7:0] */
    *rData = rawData&0xff;

    _smi_writeBit(0x00, 1);                        /* ACK by CPU */

    _smi_readBit(8, &rawData);                    /* Read DATA [15: 8] */

    _smi_writeBit(0x01, 1);                        /* ACK by CPU */
    *rData |= (rawData<<8);

    _smi_stop();

    rtlglue_drvMutexUnlock();/*enable CPU interrupt*/

    return ret;
}

rtksw_api_ret_t reg_i2c_write(rtksw_uint32 mAddrs, rtksw_uint32 rData)
{
    rtksw_int8 con;
    rtksw_uint32 ACK;
    rtksw_uint32 ret = RT_ERR_OK;

    if(mAddrs > 0xFFFF)
        return RT_ERR_INPUT;

    if(rData > 0xFFFF)
        return RT_ERR_INPUT;

    /*Disable CPU interrupt to ensure that the SMI operation is atomic.
      The API is based on RTL865X, rewrite the API if porting to other platform.*/
    rtlglue_drvMutexLock();

    _smi_start();                                /* Start SMI */

    _smi_writeBit(0x0b, 4);                     /* CTRL code: 4'b1011 for RTL8370*/

    _smi_writeBit(0x4, 3);                        /* CTRL code: 3'b100 */

    _smi_writeBit(0x0, 1);                        /* 0: issue WRITE command */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for issuing WRITE command*/
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs&0xff), 8);             /* Set reg_addr[7:0] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for setting reg_addr[7:0] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs>>8), 8);                 /* Set reg_addr[15:8] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for setting reg_addr[15:8] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit(rData&0xff, 8);                /* Write Data [7:0] out */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for writting data [7:0] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit(rData>>8, 8);                    /* Write Data [15:8] out */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                        /* ACK for writting data [15:8] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_stop();

    rtlglue_drvMutexUnlock();/*enable CPU interrupt*/

    return ret;
}

rtksw_api_ret_t reg_mdcmdio_read(rtksw_uint32 mAddrs, rtksw_uint32 *rData)
{
    if(mAddrs > 0xFFFF)
        return RT_ERR_INPUT;

    if(rData == NULL)
        return RT_ERR_NULL_POINTER;

    if (!__mii_bus)
        return RT_ERR_FAILED;

    *rData = 0;

    /* Lock */
    rtlglue_drvMutexLock();

    /* Write address control code to register 31 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);

    /* Write address to register 23 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_ADDRESS_REG, mAddrs);

    /* Write read control code to register 21 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_READ_OP);

    /* Read data from register 25 */
    MDC_MDIO_READ(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_DATA_READ_REG, rData);

    /* Unlock */
    rtlglue_drvMutexUnlock();

    return RT_ERR_OK;
}

rtksw_api_ret_t reg_mdcmdio_write(rtksw_uint32 mAddrs, rtksw_uint32 rData)
{
    if(mAddrs > 0xFFFF)
        return RT_ERR_INPUT;

    if(rData > 0xFFFF)
        return RT_ERR_INPUT;

    if (!__mii_bus)
        return RT_ERR_FAILED;

    /* Lock */
    rtlglue_drvMutexLock();

    /* Write address control code to register 31 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);

    /* Write address to register 23 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_ADDRESS_REG, mAddrs);

    /* Write data to register 24 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_DATA_WRITE_REG, rData);

    /* Write data control code to register 21 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_PHY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_WRITE_OP);

    /* Unlock */
    rtlglue_drvMutexUnlock();

    return RT_ERR_OK;
}

rtksw_api_ret_t reg_spi_read(rtksw_uint32 mAddrs, rtksw_uint32 *rData)
{
    if(mAddrs > 0xFFFF)
        return RT_ERR_INPUT;

    if(rData == NULL)
        return RT_ERR_NULL_POINTER;

    *rData = 0;

    /* Lock */
    rtlglue_drvMutexLock();

    /* Write 8 bits READ OP_CODE */
    SPI_WRITE(SPI_READ_OP, SPI_READ_OP_LEN);

    /* Write 16 bits register address */
    SPI_WRITE(mAddrs, SPI_REG_LEN);

    /* Read 16 bits data */
    SPI_READ(rData, SPI_DATA_LEN);

    /* Unlock */
    rtlglue_drvMutexUnlock();

    return RT_ERR_OK;
}

rtksw_api_ret_t reg_spi_write(rtksw_uint32 mAddrs, rtksw_uint32 rData)
{
    if(mAddrs > 0xFFFF)
        return RT_ERR_INPUT;

    if(rData > 0xFFFF)
        return RT_ERR_INPUT;

    /* Lock */
    rtlglue_drvMutexLock();

    /* Write 8 bits WRITE OP_CODE */
    SPI_WRITE(SPI_WRITE_OP, SPI_WRITE_OP_LEN);

    /* Write 16 bits register address */
    SPI_WRITE(mAddrs, SPI_REG_LEN);

    /* Write 16 bits data */
    SPI_WRITE(rData, SPI_DATA_LEN);

    /* Unlock */
    rtlglue_drvMutexUnlock();

    return RT_ERR_OK;
}

#ifdef EASYSMART_SDK
rtksw_api_ret_t reg_soc_read(rtksw_uint32 mAddrs, rtksw_uint32 *rData)
{
    rtksw_uint32 regData;

    if(mAddrs > 0xFFFF)
        return RT_ERR_INPUT;

    if(rData == NULL)
        return RT_ERR_NULL_POINTER;

    /* SoC address = ((switch register << 2) | 0x02000000) */
    /* SoC data = switch data [7:0] [15:8] [23:16] [31:24] */
    regData = REG32((mAddrs << 2) | 0x02000000);
    *rData = ((regData & 0xFF000000) >> 24) | ((regData & 0x00FF0000) >> 8);

    return RT_ERR_OK;
}

rtksw_api_ret_t reg_soc_write(rtksw_uint32 mAddrs, rtksw_uint32 rData)
{
    rtksw_uint32 regData;

    if(mAddrs > 0xFFFF)
        return RT_ERR_INPUT;

    if(rData > 0xFFFF)
        return RT_ERR_INPUT;

    /* SoC address = ((switch register << 2) | 0x02000000) */
    /* SoC data = switch data [7:0] [15:8] [23:16] [31:24] */
    regData = ((rData & 0x000000FF) << 24) | ((rData & 0x0000FF00) << 8);
    REG32((mAddrs << 2) | 0x02000000) = regData;

    return RT_ERR_OK;
}
#endif

rtksw_api_ret_t reg_smi_read(rtksw_uint32 unit, rtksw_uint32 mAddrs, rtksw_uint32 *rData)
{
    rtksw_api_ret_t retVal;
#if defined(CONFIG_MSSDK)
    retVal = ioal_mem32_read(unit, mAddrs, rData);
    return retVal;
#else
    /* User should maintain this switch-case sample code.         */
    /* Make sure that each unit can call correct I2C/SPI/MDC_MDIO */
    /* register reading & writing function. This sample code      */
    /* shows that unit 0 is controlled by I2C interface.          */
    switch (unit)
    {
        case 0:
#ifdef EASYSMART_SDK
            if ((retVal = reg_soc_read(mAddrs, rData)) != RT_ERR_OK)
                return retVal;
#else
            if ((retVal = reg_mdcmdio_read(mAddrs, rData)) != RT_ERR_OK)
                return retVal;
#endif
            break;
#if 0
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
#endif            
        default:
            return RT_ERR_UNIT_ID;
    }

    return RT_ERR_OK;
#endif
}


rtksw_api_ret_t reg_smi_write(rtksw_uint32 unit, rtksw_uint32 mAddrs, rtksw_uint32 rData)
{
    rtksw_api_ret_t retVal;
#if defined(CONFIG_MSSDK)
    retVal = ioal_mem32_write(unit, mAddrs, rData);
    return retVal;
#else
    /* User should maintain this switch-case sample code.         */
    /* Make sure that each unit can call correct I2C/SPI/MDC_MDIO */
    /* register reading & writing function. This sample code      */
    /* shows that unit 0 is controlled by I2C interface.          */
    switch (unit)
    {
        case 0:
#ifdef EASYSMART_SDK
            if ((retVal = reg_soc_write(mAddrs, rData)) != RT_ERR_OK)
                return retVal;
#else
            if ((retVal = reg_mdcmdio_write(mAddrs, rData)) != RT_ERR_OK)
                return retVal;
#endif
            break;
#if 0
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
#endif
        default:
            return RT_ERR_UNIT_ID;
    }

    return RT_ERR_OK;
#endif
}
