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

#include <dal/rtl8371c/rtk_rtl8371c_macdrv.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>
#include <dal/rtl8371c/rtk_rtl8371c_table_struct.h>

/*
 * Macro Declaration
 */
rtksw_uint32 tableDataWriteReg[RTL8371C_TABLE_WORD] =
{
    RTL8371C_ACCESS_DATA_W00r,
    RTL8371C_ACCESS_DATA_W01r,
    RTL8371C_ACCESS_DATA_W02r,
    RTL8371C_ACCESS_DATA_W03r,
    RTL8371C_ACCESS_DATA_W04r,
    RTL8371C_ACCESS_DATA_W05r,
    RTL8371C_ACCESS_DATA_W06r,
    RTL8371C_ACCESS_DATA_W07r,
    RTL8371C_ACCESS_DATA_W08r,
    RTL8371C_ACCESS_DATA_W09r,
};

rtksw_uint32 tableDataReadReg[RTL8371C_TABLE_WORD] =
{
    RTL8371C_ACCESS_DATA_R00r,
    RTL8371C_ACCESS_DATA_R01r,
    RTL8371C_ACCESS_DATA_R02r,
    RTL8371C_ACCESS_DATA_R03r,
    RTL8371C_ACCESS_DATA_R04r,
    RTL8371C_ACCESS_DATA_R05r,
    RTL8371C_ACCESS_DATA_R06r,
    RTL8371C_ACCESS_DATA_R07r,
    RTL8371C_ACCESS_DATA_R08r,
    RTL8371C_ACCESS_DATA_R09r,
};


/* Function Name:
 *      rtl8371c_table_write
 * Description:
 *      Write one L2 specified table entry by table index.
 * Input:
 *      unit    - Unit ID
 *      table   - table index
 *      addr    - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
rtksw_int32
rtl8371c_table_write( 
    rtksw_uint32  unit,
    rtksw_uint32  table,
    rtksw_uint32  addr,
    rtksw_uint32  *pData)
{

    rtksw_int32       retVal = RT_ERR_FAILED;
    rtksw_uint32      regData;
    rtksw_uint32      fieldData;
    rtksw_uint32      busyCounter;
    rtksw_uint32      i;
    rtksw_uint32      busy;
    rtksw_table_t     *pTable;

    /* Find table. NULL means the table is not supported in this chip unit */
    pTable = table16_find(unit, table);
    RTKSW_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);

    /*for ACL data and mask use the same table, but different index*/
    if (table == RTL8371C_ACL_RULE_DATAt)
        addr += pTable->size;

    busyCounter = RTL8371C_TBL_BUSY_CHECK_NO;
    while(busyCounter)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_ACS_CMDr, RTL8371C_TLB_EXECUTEf, &busy)) != RT_ERR_OK)
            return retVal;

        if(!busy)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((retVal = reg16_write(unit, tableDataWriteReg[i], &pData[i])) != RT_ERR_OK)
            return retVal;
    }

    fieldData = addr;
    if ((retVal = reg16_field_write(unit, RTL8371C_ACS_ADRr, RTL8371C_ADDRESSf, &fieldData)) != RT_ERR_OK)
        return retVal;

    /* Prepare Write Command */
    if ((retVal = reg16_read(unit, RTL8371C_ACS_CMDr, &regData)) != RT_ERR_OK)
        return retVal;

    /* Set table type */
    fieldData = pTable->type;
    if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_TABLE_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    /*set command type -- 0b1 write*/
    fieldData = 1;
    if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_COMMAND_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    /* Issue command */
    fieldData = 1;
    if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_TLB_EXECUTEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_write(unit, RTL8371C_ACS_CMDr, &regData)) != RT_ERR_OK)
        return retVal;

    /*check if table access status*/
    busyCounter = RTL8371C_TBL_BUSY_CHECK_NO;
    while(busyCounter)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_ACS_CMDr, RTL8371C_TLB_EXECUTEf, &busy)) != RT_ERR_OK)
            return retVal;

        if(!busy)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    return RT_ERR_OK;
}/* end of rtl8371c_table_write */

/* Function Name:
 *      rtl8371c_table_read
 * Description:
 *      Read one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
rtksw_int32
rtl8371c_table_read(
    rtksw_uint32  unit,
    rtksw_uint32  table,
    rtksw_uint32  addr,
    rtksw_uint32  *pData)
{

    rtksw_int32       retVal = RT_ERR_FAILED;
    rtksw_uint32      i;
    rtksw_uint32      regData;
    rtksw_uint32      fieldData;
    rtksw_uint32      busyCounter;
    rtksw_uint32      busy;
    rtksw_table_t     *pTable;

    /* Find table. NULL means the table is not supported in this chip unit */
    pTable = table16_find(unit, table);
    RTKSW_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);

    /*for ACL data and mask use the same table, but different index*/
    if (table == RTL8371C_ACL_RULE_DATAt)
        addr += pTable->size;

    busyCounter = RTL8371C_TBL_BUSY_CHECK_NO;
    while(busyCounter)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_ACS_CMDr, RTL8371C_TLB_EXECUTEf, &busy)) != RT_ERR_OK)
            return retVal;

        if(!busy)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    /* Set address */
    fieldData = addr;
    if ((retVal = reg16_field_write(unit, RTL8371C_ACS_ADRr, RTL8371C_ADDRESSf, &fieldData)) != RT_ERR_OK)
        return retVal;

    /* Read Command */
    if ((retVal = reg16_read(unit, RTL8371C_ACS_CMDr, &regData)) != RT_ERR_OK)
        return retVal;

    /* Set table type */
    fieldData = pTable->type;
    if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_TABLE_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    /*set command type -- 0b0 read*/
    fieldData = 0;
    if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_COMMAND_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    /* Issue command */
    fieldData = 1;
    if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_TLB_EXECUTEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_write(unit, RTL8371C_ACS_CMDr, &regData)) != RT_ERR_OK)
        return retVal;

    busyCounter = RTL8371C_TBL_BUSY_CHECK_NO;
    while(busyCounter)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_ACS_CMDr, RTL8371C_TLB_EXECUTEf, &busy)) != RT_ERR_OK)
            return retVal;

        if(!busy)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        if ((retVal = reg16_read(unit, tableDataReadReg[i], &pData[i])) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}/* end of rtl8371c_table_read */