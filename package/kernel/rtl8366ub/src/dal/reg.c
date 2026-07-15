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

#include <reg.h>
#include <rtk_error.h>
#include <chip.h>

#include <dal/dal_mgmts.h>

#if defined(RTK_X86_ASICDRV)
#include <I2Clib.h>
#else
#include <dal/smi.h>
#endif

/*
 * Data Declaration
 */
rtksw_halreg_t halRegCtrlNode[DAL_MGMT_MAX_UNIT] = {0};
rtksw_halreg_t *halRegCtrl[DAL_MGMT_MAX_UNIT] = 
{
    &halRegCtrlNode[0],
#if (DAL_MGMT_MAX_UNIT > 1)  
    &halRegCtrlNode[1],
#endif
#if (DAL_MGMT_MAX_UNIT > 2)  
    &halRegCtrlNode[2],
#endif   
#if (DAL_MGMT_MAX_UNIT > 3)  
    &halRegCtrlNode[3],
#endif   
};

/*
 * Symbol Definition
 */
#if defined(CONFIG_RTL865X_CLE) || defined (RTK_X86_CLE)
extern rtksw_uint32 cleDebuggingDisplay;
#endif

#ifdef CONFIG_VIRTUAL_REG_ASICDRV_TEST
#define CLE_VIRTUAL_REG_SIZE        0x10000
static rtksw_uint16 CleVirtualReg[CLE_VIRTUAL_REG_SIZE];
#endif

/*
 * Function Declaration
 */
static rtksw_regField_t *_reg_field_find(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_uint32 field);
static rtksw_int32 _reg_addr_find(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_int32 index_1, rtksw_int32 index_2, rtksw_uint32 *pAddr);


rtksw_int32 hal_setAsicReg(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_uint32 value)
{
#if defined(RTK_X86_ASICDRV)

    ret_t retVal;

    if (unit) {} /* Skip warning */

    retVal = Access_Write(reg,2,value);
    if(TRUE != retVal) 
        return RT_ERR_SMI;

    if(0x8367B == cleDebuggingDisplay)
        PRINT("W[0x%4.4x]=0x%4.4x\n",reg,value);

#elif defined(CONFIG_VIRTUAL_REG_ASICDRV_TEST)

    if (unit) {} /* Skip warning */

    if(reg >= CLE_VIRTUAL_REG_SIZE)
        return RT_ERR_OUT_OF_RANGE;

    CleVirtualReg[reg] = value;

    if(0x8367B == cleDebuggingDisplay)
        PRINT("W[0x%4.4x]=0x%4.4x\n",reg,CleVirtualReg[reg]);

#else
    ret_t retVal;

    retVal = reg_smi_write(unit, reg, value);
    if(retVal != RT_ERR_OK)
        return RT_ERR_SMI;
  #ifdef CONFIG_RTL865X_CLE
    if(0x8367B == cleDebuggingDisplay)
        PRINT("W[0x%4.4x]=0x%4.4x\n",reg,value);
  #endif

#endif

    return RT_ERR_OK;
}

rtksw_int32 hal_getAsicReg(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_uint32 *pValue)
{

#if defined(RTK_X86_ASICDRV)

    rtksw_uint32 regData;
    ret_t retVal;
    
    if (unit) {} /* Skip warning */

    retVal = Access_Read(reg, 2, &regData);
    if(TRUE != retVal)
        return RT_ERR_SMI;

    *pValue = regData;

    if(0x8367B == cleDebuggingDisplay)
        PRINT("R[0x%4.4x]=0x%4.4x\n", reg, regData);

#elif defined(CONFIG_VIRTUAL_REG_ASICDRV_TEST)
    if (unit) {} /* Skip warning */

    if(reg >= CLE_VIRTUAL_REG_SIZE)
        return RT_ERR_OUT_OF_RANGE;

    *pValue = CleVirtualReg[reg];

    if(0x8367B == cleDebuggingDisplay)
        PRINT("R[0x%4.4x]=0x%4.4x\n", reg, CleVirtualReg[reg]);

#else
    rtksw_uint32 regData;
    ret_t retVal;

    retVal = reg_smi_read(unit, reg, &regData);
    if(retVal != RT_ERR_OK)
        return RT_ERR_SMI;

    *pValue = regData;
  #ifdef CONFIG_RTL865X_CLE
    if(0x8367B == cleDebuggingDisplay)
        PRINT("R[0x%4.4x]=0x%4.4x\n", reg, regData);
  #endif

#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      hal16_isPpBlock_check
 * Description:
 *      Check the register is PER_PORT block or not?
 * Input:
 *      unit        - Unit ID
 *      addr        - register address
 * Output:
 *      pIsPpBlock - pointer buffer of chip is PER_PORT block?
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - failed
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtksw_int32 hal16_isPpBlock_check(rtksw_uint32 unit, rtksw_uint32 addr, rtksw_uint32 *pIsPpBlock)
{
    /* parameter check */
    RTKSW_PARAM_CHK((NULL == pIsPpBlock), RT_ERR_NULL_POINTER);

    if (addr < RTKSW_HAL_GET_MACPP_MIN_ADDR() || addr > RTKSW_HAL_GET_MACPP_MAX_ADDR())
        *pIsPpBlock = FALSE;
    else
        *pIsPpBlock = TRUE;

    return RT_ERR_OK;
} /* end of hal16_isPpBlock_check */

/* Function Name:
 *      hal16_isPpPTPBlock_check
 * Description:
 *      Check the register is PER_PORT PTP block or not?
 * Input:
 *      unit        - Unit ID
 *      addr        - register address
 * Output:
 *      pIsPpPTPBlock - pointer buffer of chip is PER_PORT PTP block?
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - failed
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtksw_int32 hal16_isPpPTPBlock_check(rtksw_uint32 unit, rtksw_uint32 addr, rtksw_uint32 *pIsPpPTPBlock)
{
    /* parameter check */
    RTKSW_PARAM_CHK((NULL == pIsPpPTPBlock), RT_ERR_NULL_POINTER);

    if (addr < RTKSW_HAL_GET_MACPTPPP_MIN_ADDR() || addr > RTKSW_HAL_GET_MACPTPPP_MAX_ADDR())
        *pIsPpPTPBlock = FALSE;
    else
        *pIsPpPTPBlock = TRUE;

    return RT_ERR_OK;
} /* end of hal16_isPpPTPBlock_check */


/* Static Function Body */
/* Function Name:
 *      _reg_field_find
 * Description:
 *      Find this kind of register field structure in this specified chip.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 *      field       - field index
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of register field structure that found
 * Note:
 *      None
 */
static rtksw_regField_t *
_reg_field_find(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_uint32 field)
{
    rtksw_uint32          i;
    rtksw_regField_t  *pField = NULL;

    pField = RTKSW_HAL_GET_REG_FIELDS(reg);
    if(pField == NULL)
    {
        return NULL;
    }
    for (i = 0; i < RTKSW_HAL_GET_REG_FIELD_NUM(reg); i++, pField++)
    {
        if (pField->name == field)
           return pField;
    }
    return NULL;    /* field not found */
} /* end of _reg_field_find */


/* Public Function Body */
/* Function Name:
 *      reg16_read
 * Description:
 *      Read one specified register from the chip or lower layer I/O.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 * Output:
 *      pValue - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
rtksw_int32
reg16_read(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_uint32 *pValue)
{
    rtksw_int32 ret = RT_ERR_FAILED;
    rtksw_uint32 addr = 0, num = 0, i;
    rtksw_uint32 regData;

    RTKSW_PARAM_CHK((reg >= RTKSW_HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    num = RTKSW_HAL_GET_REG_WORD_NUM(reg);
    for (i = 0; i < num; i++)
    {
        ret = hal_getAsicReg(unit, addr+2*i, &regData);
        *(pValue+i) &= (0xFFFF0000 >> (16 * (i % 2)));
        *(pValue+i) |= (regData << (16 * (i % 2)));
    }

    return ret;
} /* end of reg16_read */


/* Function Name:
 *      reg16_write
 * Description:
 *      Write one specified register to the chip or lower layer I/O.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 *      pValue      - pointer buffer of register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. Use semaphore for prevent race condition with reg16_field_write
 *      2. The function is supported 32-bit register access.
 */
rtksw_int32
reg16_write(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_uint32 *pValue)
{
    rtksw_int32 ret = RT_ERR_FAILED;
    rtksw_uint32 addr = 0, num = 0, i;
    rtksw_uint32 regData;

    RTKSW_PARAM_CHK((reg >= RTKSW_HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    if ((ret = _reg_addr_find(unit, reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    num = RTKSW_HAL_GET_REG_WORD_NUM(reg);
    for (i = 0; i < num; i++)
    {
        regData = (*(pValue+i) >> (16 * (i % 2))) & 0xFFFF;
        ret = hal_setAsicReg(unit, addr + (2 * i), regData);
    }

    return ret;
} /* end of reg16_write */


/* Function Name:
 *      reg16_field_get
 * Description:
 *      Get the value from one specified field of register in buffer.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 *      field       - field index of the register
 *      pData       - pointer buffer of register data
 * Output:
 *      pValue - pointer buffer of value from the specified field of register
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
rtksw_int32
reg16_field_get(
    rtksw_uint32  unit, 
    rtksw_uint32  reg,
    rtksw_uint32  field,
    rtksw_uint32  *pValue,
    rtksw_uint32  *pData)
{
    rtksw_uint32  masks;
    rtksw_uint32  field_bit_pos, reg_word_num;
    rtksw_uint32  data_word_pos, data_bit_pos;
    rtksw_regField_t  *pField = NULL;

    RTKSW_PARAM_CHK(((reg >= RTKSW_HAL_GET_MAX_REG_IDX()) || (field >= RTKSW_HAL_GET_MAX_REGFIELD_IDX())), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    pField = _reg_field_find(unit, reg, field);
    RTKSW_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    field_bit_pos = pField->lsp;

    /* Caculate the data LSB bit */
    reg_word_num = RTKSW_HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 4);
    data_bit_pos = field_bit_pos & 0xF;

    if (pField->len < 16)
        masks = (1 << pField->len) - 1;
    else
        masks = 0xFFFF;

    *pValue = (pData[data_word_pos] & (masks << data_bit_pos)) >> data_bit_pos;
    return RT_ERR_OK;
} /* end of reg16_field_get */


/* Function Name:
 *      reg16_field_set
 * Description:
 *      Set the value to one specified field of register in buffer.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 *      field       - field index of the register
 *      pValue      - pointer buffer of value from the specified field of register
 * Output:
 *      pData  - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
rtksw_int32
reg16_field_set(
    rtksw_uint32  unit, 
    rtksw_uint32  reg,
    rtksw_uint32  field,
    rtksw_uint32  *pValue,
    rtksw_uint32  *pData)
{
    rtksw_uint32  masks;
    rtksw_uint32  field_bit_pos, reg_word_num;
    rtksw_uint32  data_word_pos, data_bit_pos;
    rtksw_regField_t  *pField = NULL;

    RTKSW_PARAM_CHK(((reg >= RTKSW_HAL_GET_MAX_REG_IDX()) || (field >= RTKSW_HAL_GET_MAX_REGFIELD_IDX())), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pField = _reg_field_find(unit, reg, field);
    RTKSW_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);


    /* Base on pField->lsp and pField->len to process */
    field_bit_pos = pField->lsp;

    /* Caculate the data LSB bit */
    reg_word_num = RTKSW_HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 4);
    data_bit_pos = field_bit_pos & 0xF;

    if (pField->len < 16)
        masks = (1 << pField->len) - 1;
    else
        masks = 0xFFFF;

    pData[data_word_pos] &= ~(masks << data_bit_pos);
    pData[data_word_pos] |= (((*pValue) & masks) << data_bit_pos);

    return RT_ERR_OK;
} /* end of reg16_field_set */


/* Function Name:
 *      reg16_field_read
 * Description:
 *      Read the value from one specified field of register in chip.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 *      field       - field index of the register
 * Output:
 *      pValue - pointer buffer of value from the specified field of register
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
rtksw_int32
reg16_field_read(
    rtksw_uint32  unit, 
    rtksw_uint32  reg,
    rtksw_uint32  field,
    rtksw_uint32  *pValue)
{
    rtksw_uint32  reg_word_num, data_word_pos;
    rtksw_uint32  data, mask;
    rtksw_uint32  addr = 0, mod16_lsp;
    rtksw_int32   ret = RT_ERR_FAILED;
    rtksw_regField_t  *pField = NULL;

    RTKSW_PARAM_CHK((reg >= RTKSW_HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(unit, reg, field);
    RTKSW_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    reg_word_num = RTKSW_HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (pField->lsp >> 4);
    addr += (data_word_pos * 2);
    mod16_lsp = pField->lsp & 0xF;

    if (hal_getAsicReg(unit, addr, &data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    
    if (pField->len < 16)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFF;
 
    (*pValue) = (data >> mod16_lsp) & mask;
    return RT_ERR_OK;

} /* end of reg16_field_read */


/* Function Name:
 *      reg16_field_write
 * Description:
 *      Write the value to one specified field of register in chip.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 *      field       - field index of the register
 *      pValue      - value from the specified field of register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. Use semaphore for prevent race condition with reg16_write
 *      2. The function is supported 32-bit register access.
 */
rtksw_int32
reg16_field_write(
    rtksw_uint32  unit, 
    rtksw_uint32  reg,
    rtksw_uint32  field,
    rtksw_uint32  *pValue)
{
    rtksw_uint32 reg_word_num, data_word_pos;
    rtksw_uint32 data, mask;
    rtksw_uint32 addr = 0;
    rtksw_uint32 mod16_lsp;
    rtksw_int32 ret = RT_ERR_FAILED;
    rtksw_regField_t *pField = NULL;

    RTKSW_PARAM_CHK((reg >= RTKSW_HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(unit, reg, field);
    RTKSW_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    reg_word_num = RTKSW_HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (pField->lsp >> 4);
    addr += (data_word_pos * 2);
    mod16_lsp = pField->lsp & 0xF;

    if (hal_getAsicReg(unit, addr, &data) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    if (pField->len < 16)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFF;

    data &= ~(mask << mod16_lsp);
    data |= (((*pValue) & mask) << mod16_lsp);

    if (hal_setAsicReg(unit, addr, data) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of reg16_field_write */


/* Function Name:
 *      reg16_idx2Addr_get
 * Description:
 *      Register index to physical address transfer in chip.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 * Output:
 *      pAddr - pointer buffer of register address
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
rtksw_int32 
reg16_idx2Addr_get(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_uint32 *pAddr)
{
    rtksw_int32   ret = RT_ERR_FAILED;

    RTKSW_PARAM_CHK((pAddr == NULL), RT_ERR_NULL_POINTER);
    RTKSW_PARAM_CHK((reg >= RTKSW_HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);

    if ((ret = _reg_addr_find(unit, reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, pAddr)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;   
} /* end of reg16_idx2Addr_get */


/* Function Name:
 *      reg16_idxMax_get
 * Description:
 *      Get the maximum value of register index in chip.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 * Output:
 *      pMax - pointer buffer of maximum value of register index
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
rtksw_int32 
reg16_idxMax_get(rtksw_uint32 unit, rtksw_uint32 *pMax)
{
    RTKSW_PARAM_CHK((pMax == NULL), RT_ERR_NULL_POINTER);

    (*pMax) = RTKSW_HAL_GET_MAX_REG_IDX();
    
    return RT_ERR_OK;   
} /* end of reg16_idxMax_get */


/*#################################################*/
/* Following APIS are for register array mechanism */
/*#################################################*/

/* Function Name:
 *      _reg_addr_find
 * Description:
 *      Calculate one specified register array address.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 *      index_1     - dimension 1 index
 *      index_2     - dimension 2 index
 * Output:
 *      pAddr   - pointer buffer of register address
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_ENTRY_INDEX  - invalid index of register array
 * Note:
 *      The function is supported 32-bit register access.
 */
static rtksw_int32
_reg_addr_find(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_int32 index_1, rtksw_int32 index_2, rtksw_uint32 *pAddr)
{
    rtksw_uint32 dim1_size = 0;
    rtksw_int32 ret = RT_ERR_FAILED;
    rtksw_uint32 isPpReg = 0;
    rtksw_uint32 isPpPTPReg = 0;

    *pAddr = RTKSW_HAL_GET_REG_ADDR(reg);

    /* Check index_1 & index_2 valid or not? */
    if ((index_1 != REG_ARRAY_INDEX_NONE) && ((index_1 < RTKSW_HAL_GET_REG_LPORT(reg)) || (index_1 > RTKSW_HAL_GET_REG_HPORT(reg))))
        return RT_ERR_ENTRY_INDEX;
    
    if ((index_2 != REG_ARRAY_INDEX_NONE) && ((index_2 < RTKSW_HAL_GET_REG_LARRAY(reg)) || (index_2 > RTKSW_HAL_GET_REG_HARRAY(reg))))
        return RT_ERR_ENTRY_INDEX;

    if ((index_1 != REG_ARRAY_INDEX_NONE) && (0 == RTKSW_HAL_GET_REG_HPORT(reg)))
        return RT_ERR_ENTRY_INDEX;

    if ((index_2 != REG_ARRAY_INDEX_NONE) && (0 == RTKSW_HAL_GET_REG_HARRAY(reg)))
        return RT_ERR_ENTRY_INDEX;

    if (hal16_isPpBlock_check(unit, (*pAddr), &isPpReg) != RT_ERR_OK)
        return ret;

    if (hal16_isPpPTPBlock_check(unit, (*pAddr), &isPpPTPReg) != RT_ERR_OK)
        return ret;

    if (isPpReg)
    {
        /* PER_PORT */
        if (index_1 != REG_ARRAY_INDEX_NONE)
            *pAddr += (index_1 * RTKSW_HAL_GET_MACPP_INTERVAL());
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            if (RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg) % 16)
                *pAddr += ((index_2 - RTKSW_HAL_GET_REG_LARRAY(reg)) / (16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg))) * 2;
            else
                *pAddr += (index_2 - RTKSW_HAL_GET_REG_LARRAY(reg)) * (RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg)/8);
        }
    }
    else if (isPpPTPReg)
    {
        /* PER_PORT PTP */
        if (index_1 != REG_ARRAY_INDEX_NONE)
            *pAddr += (index_1 * RTKSW_HAL_GET_MAPTPCPP_INTERVAL());
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            if (RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg) % 16)
                *pAddr += ((index_2 - RTKSW_HAL_GET_REG_LARRAY(reg)) / (16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg))) * 2;
            else
                *pAddr += (index_2 - RTKSW_HAL_GET_REG_LARRAY(reg)) * (RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg)/8);
        }
    }
    else
    {
        /* Global */
        if ((index_1 != REG_ARRAY_INDEX_NONE) && (index_2 != REG_ARRAY_INDEX_NONE))
        {
            /* calculate index_1 dimension size */
            if (RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg) % 16)
                dim1_size = (((RTKSW_HAL_GET_REG_HARRAY(reg) - RTKSW_HAL_GET_REG_LARRAY(reg)) / (16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg)))+1) * 2;
            else
                dim1_size = ((RTKSW_HAL_GET_REG_HARRAY(reg) - RTKSW_HAL_GET_REG_LARRAY(reg)) + 1) * (RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg)/8);
            
            /* Add index 1 offset */
            *pAddr += (index_1 - RTKSW_HAL_GET_REG_LPORT(reg)) * dim1_size;
            
            /* Add index 2 offset */
            if (RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg) % 16)
                *pAddr += ((index_2 - RTKSW_HAL_GET_REG_LARRAY(reg)) / (16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg))) * 2;
            else
                *pAddr += (index_2 - RTKSW_HAL_GET_REG_LARRAY(reg)) * (RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg)/8);
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            if (RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg) % 16)
                *pAddr += ((index_1 - RTKSW_HAL_GET_REG_LPORT(reg)) / (16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg))) * 2;
            else
                *pAddr += (index_1 - RTKSW_HAL_GET_REG_LPORT(reg)) * (RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg)/8);
        }
        else if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            if (RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg) % 16)
                *pAddr += ((index_2 - RTKSW_HAL_GET_REG_LARRAY(reg)) / (16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg))) * 2;
            else
                *pAddr += (index_2 - RTKSW_HAL_GET_REG_LARRAY(reg)) * (RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg)/8);
        }
    }

    
    return RT_ERR_OK;   
} /* end of _reg_addr_find */

/* Function Name:
 *      reg16_array_read
 * Description:
 *      Read one specified register or register array from the chip or lower layer I/O.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 *      index_1     - dimension 1 index
 *      index_2     - dimension 2 index
 * Output:
 *      pValue  - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
rtksw_int32
reg16_array_read(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_int32 index_1, rtksw_int32 index_2, rtksw_uint32 *pValue)
{
    rtksw_uint32  addr = 0, num = 0, i;
    rtksw_int32   ret = RT_ERR_FAILED;
    rtksw_uint32 regData;

    RTKSW_PARAM_CHK((reg >= RTKSW_HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    num = RTKSW_HAL_GET_REG_WORD_NUM(reg);

    for (i = 0; i < num; i++)
    {
        if ((ret = hal_getAsicReg(unit, addr+2*i, &regData)) != RT_ERR_OK)
            return ret;

        *(pValue+i) &= (0xFFFF0000 >> (16 * (i % 2)));
        *(pValue+i) |= (regData << (16 * (i % 2)));
    }

    return RT_ERR_OK;
} /* end of reg16_array_read */

/* Function Name:
 *      reg16_array_write
 * Description:
 *      Write one specified register or register array from the chip or lower layer I/O.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 *      index_1     - dimension 1 index
 *      index_2     - dimension 2 index
 * Output:
 *      pValue  - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
rtksw_int32
reg16_array_write(rtksw_uint32  unit, rtksw_uint32 reg, rtksw_int32 index_1, rtksw_int32 index_2, rtksw_uint32 *pValue)
{
    rtksw_uint32  addr = 0, num, i;
    rtksw_int32   ret = RT_ERR_FAILED;
    rtksw_uint32 regData;

    RTKSW_PARAM_CHK((reg >= RTKSW_HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    num = RTKSW_HAL_GET_REG_WORD_NUM(reg);
    for (i = 0; i < num; i++)
    {
        regData = (*(pValue+i) >> (16 * (i % 2))) & 0xFFFF;
        if ((ret = hal_setAsicReg(unit, addr+2*i, regData)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
} /* end of reg16_array_write */

/* Function Name:
 *      reg16_array_field_read
 * Description:
 *      Read the value from one specified field of register or register array in chip.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 *      index_1     - dimension 1 index
 *      index_2     - dimension 2 index
 *      field       - field index of the register
 * Output:
 *      pValue  - pointer buffer of value from the specified field of register
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
rtksw_int32
reg16_array_field_read(
    rtksw_uint32  unit, 
    rtksw_uint32  reg,
    rtksw_int32   index_1,
    rtksw_int32   index_2,
    rtksw_uint32  field,
    rtksw_uint32  *pValue)
{
    rtksw_uint32  field_bit_pos = 0, reg_word_num;
    rtksw_uint32  data_word_pos;
    rtksw_uint32  data, mask;
    rtksw_uint32  addr = 0, mod16_lsp;
    rtksw_int32   ret = RT_ERR_FAILED;
    rtksw_regField_t  *pField = NULL;

    RTKSW_PARAM_CHK((reg >= RTKSW_HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(unit, reg, field);
    RTKSW_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    if ((RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg) % 16) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-RTKSW_HAL_GET_REG_LARRAY(reg))%(16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg))) * RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg));
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-RTKSW_HAL_GET_REG_LPORT(reg))%(16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg))) * RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg));
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = RTKSW_HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 4);
    addr += data_word_pos*2;
    mod16_lsp = field_bit_pos & 15;

    if (hal_getAsicReg(unit, addr, &data) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if (pField->len < 16)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFF;

    (*pValue) = (data >> mod16_lsp) & mask;
    return RT_ERR_OK;
} /* end of reg16_array_field_read */

/* Function Name:
 *      reg16_array_field_write
 * Description:
 *      Write the value to one specified field of register or register array in chip.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 *      index_1     - dimension 1 index
 *      index_2     - dimension 2 index
 *      field       - field index of the register
 *      pValue      - pointer buffer of value from the specified field of register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. Use semaphore for prevent race condition with reg16_write
 *      2. The function is supported 32-bit register access.
 */
rtksw_int32
reg16_array_field_write(
    rtksw_uint32  unit, 
    rtksw_uint32  reg,
    rtksw_int32   index_1,
    rtksw_int32   index_2,
    rtksw_uint32  field,
    rtksw_uint32  *pValue)
{
    rtksw_uint32  field_bit_pos = 0, reg_word_num;
    rtksw_uint32  data_word_pos;
    rtksw_uint32  data, mask;
    rtksw_uint32  addr = 0, mod16_lsp;
    rtksw_int32   ret = RT_ERR_FAILED;
    rtksw_regField_t  *pField = NULL;

    RTKSW_PARAM_CHK((reg >= RTKSW_HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(unit, reg, field);
    RTKSW_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    if ((RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg) % 16) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-RTKSW_HAL_GET_REG_LARRAY(reg))%(16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg))) * RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg));
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-RTKSW_HAL_GET_REG_LPORT(reg))%(16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg))) * RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg));
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = RTKSW_HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 4);
    addr += data_word_pos*2;
    mod16_lsp = field_bit_pos & 15;

    if (hal_getAsicReg(unit, addr, &data) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    if (pField->len < 16)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFF;

    data &= ~(mask << mod16_lsp);
    data |= (((*pValue) & mask) << mod16_lsp);

    if (hal_setAsicReg(unit, addr, data) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
} /* end of reg16_array_field_write */

/* Function Name:
 *      reg16_array_field_read
 * Description:
 *      Read the value from one specified field of register or register array in buffer.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 *      index_1     - dimension 1 index
 *      index_2     - dimension 2 index
 *      field       - field index of the register
 *      pData       - pointer buffer of register data
 * Output:
 *      pValue  - pointer buffer of value from the specified field of register
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
rtksw_int32
reg16_array_field_get(
    rtksw_uint32  unit, 
    rtksw_uint32  reg,
    rtksw_int32   index_1,
    rtksw_int32   index_2,
    rtksw_uint32  field,
    rtksw_uint32  *pValue,
    rtksw_uint32  *pData)
{
    rtksw_uint32  field_bit_pos = 0, reg_word_num;
    rtksw_uint32  data_word_pos;
    rtksw_uint32  mask;
    rtksw_uint32  mod16_lsp;
    rtksw_regField_t  *pField = NULL;

    RTKSW_PARAM_CHK((reg >= RTKSW_HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    pField = _reg_field_find(unit, reg, field);
    RTKSW_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    if ((RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg) % 16) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-RTKSW_HAL_GET_REG_LARRAY(reg))%(16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg))) * RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg));
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-RTKSW_HAL_GET_REG_LPORT(reg))%(16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg))) * RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg));
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = RTKSW_HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 4);
    mod16_lsp = field_bit_pos & 15;

    if (pField->len < 16)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFF;

    *pValue = (pData[data_word_pos] & (mask << mod16_lsp)) >> mod16_lsp;
    return RT_ERR_OK;
}

/* Function Name:
 *      reg16_array_field_set
 * Description:
 *      Write the value to one specified field of register or register array in buffer.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
 *      index_1     - dimension 1 index
 *      index_2     - dimension 2 index
 *      field       - field index of the register
 *      pValue      - pointer buffer of value from the specified field of register
 * Output:
 *      pData   - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. Use semaphore for prevent race condition with reg16_write
 *      2. The function is supported 32-bit register access.
 */
rtksw_int32
reg16_array_field_set(
    rtksw_uint32  unit, 
    rtksw_uint32  reg,
    rtksw_int32   index_1,
    rtksw_int32   index_2,
    rtksw_uint32  field,
    rtksw_uint32  *pValue,
    rtksw_uint32  *pData)
{
    rtksw_uint32  field_bit_pos = 0, reg_word_num;
    rtksw_uint32  data_word_pos;
    rtksw_uint32  mask;
    rtksw_uint32  mod16_lsp;
    rtksw_regField_t  *pField = NULL;

    RTKSW_PARAM_CHK((reg >= RTKSW_HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    pField = _reg_field_find(unit, reg, field);
    RTKSW_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    if ((RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg) % 16) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-RTKSW_HAL_GET_REG_LARRAY(reg))%(16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg))) * RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg));
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-RTKSW_HAL_GET_REG_LPORT(reg))%(16/RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg))) * RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg));
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = RTKSW_HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 4);
    mod16_lsp = field_bit_pos & 15;

    if (pField->len < 16)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFF;

    pData[data_word_pos] &= ~(mask << mod16_lsp);
    pData[data_word_pos] |= (((*pValue) & mask) << mod16_lsp);
    return RT_ERR_OK;
}

/* Function Name:
 *      table16_find
 * Description:
 *      Find this kind of table structure in this specified chip.
 * Input:
 *      unit        - Unit ID
 *      table       - table index
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of table structure that found
 * Note:
 *      None
 */
rtksw_table_t *
table16_find (rtksw_uint32 unit, rtksw_uint32 table)
{
    return &(halRegCtrl[unit]->pRtl_driver->pTable_list[table]);
} /* end of table16_find */

/* Function Name:
 *      table16_write
 * Description:
 *      Write one L2 specified table entry by table index.
 * Input:
 *      unit        - Unit ID
 *      table       - table index
 *      addr        - entry address of the table
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
table16_write(
    rtksw_uint32  unit, 
    rtksw_uint32  table,
    rtksw_uint32  addr,
    rtksw_uint32  *pData)
{
    rtksw_int32 retVal = RT_ERR_FAILED;

    if (NULL == halRegCtrl[unit]->pRtl_driver->pMacdrv->fMdrv_table_write)
        return RT_ERR_FAILED;

    if ((retVal = halRegCtrl[unit]->pRtl_driver->pMacdrv->fMdrv_table_write(unit, table, addr, pData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}/* end of table16_write */

/* Function Name:
 *      table16_read
 * Description:
 *      Read one L2 specified table entry by table index.
 * Input:
 *      unit        - Unit ID
 *      table       - table index
 *      addr        - entry address of the table
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
table16_read(
    rtksw_uint32  unit, 
    rtksw_uint32  table,
    rtksw_uint32  addr,
    rtksw_uint32  *pData)
{
    rtksw_int32 retVal = RT_ERR_FAILED;

    if (NULL == halRegCtrl[unit]->pRtl_driver->pMacdrv->fMdrv_table_read)
        return RT_ERR_FAILED;

    if ((retVal = halRegCtrl[unit]->pRtl_driver->pMacdrv->fMdrv_table_read(unit, table, addr, pData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}/* end of table16_read */


/* Function Name:
 *      table16_field_get
 * Description:
 *      Get the value from one specified field of table in buffer.
 * Input:
 *      unit        - Unit ID
 *      table       - table index
 *      field       - field index of the table
 *      pData       - pointer buffer of table entry data
 * Output:
 *      pValue - pointer buffer of value from the specified field of table
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_NULL_POINTER       - input parameter is null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 *      1. The API is used when *value argument is WORD type.
 */
rtksw_int32
table16_field_get(
    rtksw_uint32  unit, 
    rtksw_uint32  table,
    rtksw_uint32  field,
    rtksw_uint32  *pValue,
    rtksw_uint32  *pData)
{
    rtksw_int32           data_word_pos, data_bit_pos;
    rtksw_uint32          i;
    rtksw_int32           bit, currentBit;
    rtksw_table_t         *pTable = NULL;
    rtksw_tableField_t    *pTblField = NULL;

    /* parameter check */
    RTKSW_PARAM_CHK((table >= RTKSW_HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK(((NULL == pValue) || (NULL == pData)), RT_ERR_NULL_POINTER);
    pTable = table16_find(unit, table);
    /* NULL means the table is not supported in this chip*/
    RTKSW_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RTKSW_PARAM_CHK((field >= pTable->field_num), RT_ERR_OUT_OF_RANGE);

    pTblField = &pTable->fields[field];

    /* reset pValue */
    for (i = 0; i <= (pTblField->len >> 4); i++ )
        pValue[i] = 0;

    for (i = 0; i < pTable->datareg_num; i++)
    {
        for (bit = 0; bit <= 15; bit++)
        {
            currentBit = (i * 16) + bit;
            if ( (currentBit >= pTblField->lsp) && ( currentBit <= (pTblField->lsp + pTblField->len - 1)) )
            {    
                data_word_pos = (currentBit - pTblField->lsp) >> 4;
                data_bit_pos = (currentBit - pTblField->lsp) & 15;
                if (pData[i] & (0x0001 << bit))
                    pValue[data_word_pos] |= (0x00000001 << data_bit_pos);
                else
                    pValue[data_word_pos] &= ~(0x00000001 << data_bit_pos);
            }
        }
    }

    return RT_ERR_OK;
} /* end of table16_field_get */


/* Function Name:
 *      table16_field_set
 * Description:
 *      Set the value to one specified field of table in buffer.
 * Input:
 *      unit        - Unit ID
 *      table       - table index
 *      field       - field index of the table
 *      pValue      - pointer buffer of value from the specified field of table
 * Output:
 *      pData  - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_NULL_POINTER       - input parameter is null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 *      1. The API is used when *pValue argument is WORD type.
 */
rtksw_int32
table16_field_set(
    rtksw_uint32  unit, 
    rtksw_uint32  table,
    rtksw_uint32  field,
    rtksw_uint32  *pValue,
    rtksw_uint32  *pData)
{
    rtksw_int32           data_word_pos, data_bit_pos;
    rtksw_uint32          i;
    rtksw_int32           bit, currentBit;
    rtksw_table_t         *pTable = NULL;
    rtksw_tableField_t    *pTblField = NULL;

    /* parameter check */
    RTKSW_PARAM_CHK((table >= RTKSW_HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    RTKSW_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    RTKSW_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table16_find(unit, table);
    /* NULL means the table is not supported in this chip  */
    RTKSW_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RTKSW_PARAM_CHK((field >= pTable->field_num), RT_ERR_OUT_OF_RANGE);

    pTblField = &pTable->fields[field];

    for (i = 0; i < pTable->datareg_num; i++)
    {
        for (bit = 0; bit <= 15; bit++)
        {
            currentBit = (i * 16) + bit;
            if ( (currentBit >= pTblField->lsp) && ( currentBit <= (pTblField->lsp + pTblField->len - 1)) )
            {    
                data_word_pos = (currentBit - pTblField->lsp) >> 4;
                data_bit_pos = (currentBit - pTblField->lsp) & 15;
                if (pValue[data_word_pos] & (0x00000001 << data_bit_pos))
                    pData[i] |= (0x0001 << bit);
                else
                    pData[i] &= ~(0x0001 << bit);
            }
        }
    }
    
    return RT_ERR_OK;
} /* end of table16_field_set */



