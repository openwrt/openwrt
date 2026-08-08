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

#ifndef __HAL_MAC_REG_H__
#define __HAL_MAC_REG_H__

/*
 * Include Files
 */
#include <rtk_types.h>

/*
 * Symbol Definition
 */
#define REG_ARRAY_INDEX_NONE    (-1)

#define RTKSW_PARAM_CHK(expr, errCode)\
do {\
    if ((rtksw_int32)(expr)) {\
        return errCode; \
    }\
} while (0)

#define RT_ERR_CHK(op, ret)\
do {\
    if ((ret = (op)) != RT_ERR_OK)\
        return ret;\
} while(0)

/*
 * Data Declaration
 */
/* register field structure definition */
#if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
#define REG_NAME_LEN     128
#endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
typedef struct rtksw_regField_s
{
    unsigned int name;              /* field enum */
    unsigned short lsp;             /* LSP (Least Significant Position) of the field */
    unsigned short len;             /* field length */
    #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
    char field_name[REG_NAME_LEN];            /* name char array */
    #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
} rtksw_regField_t;

/* register structure definition */
typedef struct rtksw_reg_s
{
    #if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
    char name[REG_NAME_LEN];          /* name char array */
    #endif  /* CONFIG_SDK_DUMP_REG_WITH_NAME */
    unsigned int offset;            /* offset address */
    unsigned int field_num;         /* total field numbers */
    unsigned int lport:16;           /* port start index */
    unsigned int hport:16;           /* port end index */
    unsigned int larray:16;          /* array start index */
    unsigned int harray:16;          /* array end index */
    unsigned int portlist_index:16;  /* portlist index */
    unsigned int bit_offset:16;      /* array offset */
    rtksw_regField_t *pFields;  /* register fields */
} rtksw_reg_t;

/* macro for driver information */
#define RTKSW_HAL_GET_MAX_REG_IDX()               (halRegCtrl[unit]->pRtl_driver->reg_idx_max)
#define RTKSW_HAL_GET_MAX_REGFIELD_IDX()          (halRegCtrl[unit]->pRtl_driver->regField_idx_max)
#define RTKSW_HAL_GET_MAX_TABLE_IDX()             (halRegCtrl[unit]->pRtl_driver->table_idx_max)

/* macro for driver register information */
#define RTKSW_HAL_GET_REG_WORD_NUM(reg)          ((RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg)>16)?((RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg)+15)>>4):1)
#define RTKSW_HAL_GET_REG_FIELDS(reg)            (halRegCtrl[unit]->pRtl_driver->pReg_list[reg].pFields)
#define RTKSW_HAL_GET_REG_FIELD_NUM(reg)         (halRegCtrl[unit]->pRtl_driver->pReg_list[reg].field_num)
#define RTKSW_HAL_GET_REG_LPORT(reg)             (halRegCtrl[unit]->pRtl_driver->pReg_list[reg].lport)
#define RTKSW_HAL_GET_REG_HPORT(reg)             (halRegCtrl[unit]->pRtl_driver->pReg_list[reg].hport)
#define RTKSW_HAL_GET_REG_LARRAY(reg)            (halRegCtrl[unit]->pRtl_driver->pReg_list[reg].larray)
#define RTKSW_HAL_GET_REG_HARRAY(reg)            (halRegCtrl[unit]->pRtl_driver->pReg_list[reg].harray)
#define RTKSW_HAL_GET_REG_ARRAY_OFFSET(reg)      (halRegCtrl[unit]->pRtl_driver->pReg_list[reg].bit_offset)
#define RTKSW_HAL_GET_REG_ADDR(reg)              (halRegCtrl[unit]->pRtl_driver->pReg_list[reg].offset)

#define RTKSW_HAL_GET_MACPP_MIN_ADDR()            (halRegCtrl[unit]->pRtl_driver->pMacPpInfo->lowerbound_addr)
#define RTKSW_HAL_GET_MACPP_MAX_ADDR()            (halRegCtrl[unit]->pRtl_driver->pMacPpInfo->upperbound_addr)
#define RTKSW_HAL_GET_MACPP_INTERVAL()            (halRegCtrl[unit]->pRtl_driver->pMacPpInfo->interval)

#define RTKSW_HAL_GET_MACPTPPP_MIN_ADDR()            (halRegCtrl[unit]->pRtl_driver->pMacPTPPpInfo->lowerbound_addr)
#define RTKSW_HAL_GET_MACPTPPP_MAX_ADDR()            (halRegCtrl[unit]->pRtl_driver->pMacPTPPpInfo->upperbound_addr)
#define RTKSW_HAL_GET_MAPTPCPP_INTERVAL()            (halRegCtrl[unit]->pRtl_driver->pMacPTPPpInfo->interval)

/*
 * Function Declaration
 */
/* Define major driver structure */
typedef struct rtksw_tableField_s
{
#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
    char field_name[REG_NAME_LEN];  /* table name */  
#endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */ 
    unsigned short lsp;             /* LSP of the field */
    unsigned short len;             /* field length */
} rtksw_tableField_t;

typedef struct rtksw_table_s
{
#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
    char name[REG_NAME_LEN];  /* table name */  
#endif  /* CONFIG_SDK_DUMP_TABLE_WITH_NAME */    
    unsigned int type;              /* access table type */
    unsigned int size;              /* table size */
    unsigned int datareg_num;       /* total data registers */
    unsigned int field_num;         /* total field numbers */
    rtksw_tableField_t *fields;       /* table fields */
} rtksw_table_t;

/* Define chip PER_PORT block information */
typedef struct rtksw_macPpInfo_s
{
    rtksw_uint32 lowerbound_addr;
    rtksw_uint32 upperbound_addr;
    rtksw_uint32 interval;
} rtksw_macPpInfo_t;

/* Definition mac driver service APIs */
typedef struct rtksw_macdrv_s
{
    rtksw_int32   (*fMdrv_table_read)(rtksw_uint32, rtksw_uint32, rtksw_uint32, rtksw_uint32 *);
    rtksw_int32   (*fMdrv_table_write)(rtksw_uint32, rtksw_uint32, rtksw_uint32, rtksw_uint32 *);
} rtksw_macdrv_t;

typedef struct rtksw_driver_s
{
    rtksw_reg_t           *pReg_list;
    rtksw_table_t         *pTable_list;
    rtksw_macdrv_t         *pMacdrv;
    rtksw_uint32          reg_idx_max;
    rtksw_uint32          regField_idx_max;
    rtksw_uint32          table_idx_max;
    rtksw_macPpInfo_t      *pMacPpInfo;
    rtksw_macPpInfo_t      *pMacPTPPpInfo;
} rtksw_driver_t;

typedef struct rtksw_reg_info_s
{
    rtksw_uint32 offset;          /* offset address */
    rtksw_uint32 lport;           /* port start index */
    rtksw_uint32 hport;           /* port end index */
    rtksw_uint32 larray;          /* array start index */
    rtksw_uint32 harray;          /* array end index */
    rtksw_uint32 bit_offset;      /* array offset */
    rtksw_uint32 is_PpBlock;      /* per-port MAC block or not? */
} rtksw_reg_info_t;

typedef struct rtksw_halreg_s
{
    rtksw_driver_t *pRtl_driver;
} rtksw_halreg_t;

extern rtksw_int32 hal_setAsicReg(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_uint32 value);
extern rtksw_int32 hal_getAsicReg(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_uint32 *pValue);

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
extern rtksw_int32 hal16_isPpBlock_check(rtksw_uint32 unit, rtksw_uint32 addr, rtksw_uint32 *pIsPpBlock);

/* Function Name:
 *      hal16_isPpPTPBlock_check
 * Description:
 *      Check the register is PER_PORT block or not?
 * Input:
 *      unit        - Unit ID
 *      addr        - register address
 * Output:
 *      pIsPpPTPBlock - pointer buffer of chip is PER_PORT block?
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - failed
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern rtksw_int32 hal16_isPpPTPBlock_check(rtksw_uint32 unit, rtksw_uint32 addr, rtksw_uint32 *pIsPpPTPBlock);

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
 *      None
 */
extern rtksw_int32 reg16_read(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_uint32 *pValue);

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
 * Note:
 *      Use semaphore for prevent race condition with reg16_field_write
 */
extern rtksw_int32 reg16_write(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_uint32 *pValue);

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
 *      None
 */
extern rtksw_int32 reg16_field_get(rtksw_uint32 unit, rtksw_uint32  reg, rtksw_uint32 field, rtksw_uint32 *pValue, rtksw_uint32 *pData);

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
 *      pData - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
extern rtksw_int32 reg16_field_set(rtksw_uint32 unit, rtksw_uint32  reg, rtksw_uint32 field, rtksw_uint32 *pValue, rtksw_uint32 *pData);

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
 *      None
 */
extern rtksw_int32 reg16_field_read(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_uint32 field, rtksw_uint32 *pValue);

/* Function Name:
 *      reg16_field_write
 * Description:
 *      Write the value to one specified field of register in chip.
 * Input:
 *      unit        - Unit ID
 *      reg         - register index
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
 *      Use semaphore for prevent race condition with reg16_write
 */
extern rtksw_int32 reg16_field_write(rtksw_uint32 unit, rtksw_uint32  reg, rtksw_uint32 field, rtksw_uint32 *pValue);

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
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
extern rtksw_int32 reg16_idx2Addr_get(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_uint32 *pAddr);

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
extern rtksw_int32 reg16_idxMax_get(rtksw_uint32 unit, rtksw_uint32 *pMax);


/*#################################################*/
/* Following APIS are for register array mechanism */
/*#################################################*/
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
rtksw_int32 reg16_array_read(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_int32 index_1, rtksw_int32 index_2, rtksw_uint32 *pValue);

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
rtksw_int32 reg16_array_write(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_int32 index_1, rtksw_int32 index_2, rtksw_uint32 *pValue);

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
rtksw_int32 reg16_array_field_read(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_int32 index_1, rtksw_int32 index_2, rtksw_uint32 field, rtksw_uint32 *pValue);

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
 *      pValue      - pointer buffer value from the specified field of register
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
rtksw_int32 reg16_array_field_write(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_int32 index_1, rtksw_int32 index_2, rtksw_uint32 field, rtksw_uint32  *pValue);

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
rtksw_int32 reg16_array_field_get(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_int32 index_1, rtksw_int32 index_2, rtksw_uint32 field, rtksw_uint32 *pValue, rtksw_uint32 *pData);

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
rtksw_int32 reg16_array_field_set(rtksw_uint32 unit, rtksw_uint32 reg, rtksw_int32 index_1, rtksw_int32 index_2, rtksw_uint32 field, rtksw_uint32 *pValue, rtksw_uint32 *pData);

#if 0
/* Function Name:
 *      reg_info_get
 * Description:
 *      Get register information of the specified register index.
 * Input:
 *      unit        - Unit ID
 *      reg   - register index
 * Output:
 *      pData - pointer buffer of register information
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
rtksw_int32 
reg_info_get(rtksw_uint32 reg, rtksw_reg_info_t *pData);
#endif

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
extern rtksw_table_t *table16_find(rtksw_uint32 unit, rtksw_uint32 table);

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
extern rtksw_int32 table16_write(rtksw_uint32 unit, rtksw_uint32 table, rtksw_uint32 addr, rtksw_uint32 *pData);

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
extern rtksw_int32 table16_read(rtksw_uint32 unit, rtksw_uint32 table, rtksw_uint32 addr, rtksw_uint32 *pData);

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
extern rtksw_int32 table16_field_get(rtksw_uint32 unit, rtksw_uint32 table, rtksw_uint32 field, rtksw_uint32 *pValue, rtksw_uint32 *pData);

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
extern rtksw_int32 table16_field_set(rtksw_uint32 unit, rtksw_uint32 table, rtksw_uint32 field, rtksw_uint32 *pValue, rtksw_uint32 *pData);

#endif  /* __HAL_MAC_REG_H__ */
