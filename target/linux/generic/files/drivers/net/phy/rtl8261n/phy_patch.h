/*
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (c) 2023 Realtek Semiconductor Corp. All rights reserved.
 */

#ifndef __HAL_PHY_PATCH_H__
#define __HAL_PHY_PATCH_H__

/*
 * Include Files
 */
#include "rtk_phylib_def.h"

/*
 * Symbol Definition
 */
#define PHY_PATCH_MODE_NORMAL       0
#define PHY_PATCH_MODE_CMP          1
#define PHY_PATCH_MODE_BCAST        2
#define PHY_PATCH_MODE_BCAST_BUS    3

#define RTK_PATCH_CMP_W        0  /* write */
#define RTK_PATCH_CMP_WC       1  /* compare */
#define RTK_PATCH_CMP_SWC      2  /* sram compare */
#define RTK_PATCH_CMP_WS       3  /* skip */

#define RTK_PATCH_OP_SECTION_SIZE           50
#define RTK_PATCH_OP_TO_CMP(_op, _cmp)       (_op + (RTK_PATCH_OP_SECTION_SIZE * _cmp))
/* 0~49 normal op */
#define RTK_PATCH_OP_PHY                     0
#define RTK_PATCH_OP_PHYOCP                  1
#define RTK_PATCH_OP_TOP                     2
#define RTK_PATCH_OP_TOPOCP                  3
#define RTK_PATCH_OP_PSDS0                   4
#define RTK_PATCH_OP_PSDS1                   5
#define RTK_PATCH_OP_MSDS                    6
#define RTK_PATCH_OP_MAC                     7

/* 50~99 normal op for compare */
#define RTK_PATCH_OP_CMP_PHY                 RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PHY     , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_PHYOCP              RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PHYOCP  , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_TOP                 RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_TOP     , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_TOPOCP              RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_TOPOCP  , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_PSDS0               RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PSDS0   , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_PSDS1               RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PSDS1   , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_MSDS                RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_MSDS    , RTK_PATCH_CMP_WC)
#define RTK_PATCH_OP_CMP_MAC                 RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_MAC     , RTK_PATCH_CMP_WC)

/* 100~149 normal op for sram compare */
#define RTK_PATCH_OP_CMP_SRAM_PHY            RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PHY     , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_PHYOCP         RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PHYOCP  , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_TOP            RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_TOP     , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_TOPOCP         RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_TOPOCP  , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_PSDS0          RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PSDS0   , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_PSDS1          RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_PSDS1   , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_MSDS           RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_MSDS    , RTK_PATCH_CMP_SWC)
#define RTK_PATCH_OP_CMP_SRAM_MAC            RTK_PATCH_OP_TO_CMP(RTK_PATCH_OP_MAC     , RTK_PATCH_CMP_SWC)

/* 200~255 control op */
#define RTK_PATCH_OP_DELAY_MS                200
#define RTK_PATCH_OP_SKIP                    255


/*
   patch type  PHY_PATCH_TYPE_NONE => empty
   patch type: PHY_PATCH_TYPE_TOP ~ (PHY_PATCH_TYPE_END-1)  => data array
   patch type: PHY_PATCH_TYPE_END ~ (PHY_PATCH_TYPE_END + RTK_PATCH_TYPE_FLOW_MAX)  => flow
*/
#define RTK_PATCH_TYPE_IS_DATA(_patch_type)    (_patch_type > PHY_PATCH_TYPE_NONE && _patch_type < PHY_PATCH_TYPE_END)
#define RTK_PATCH_TYPE_IS_FLOW(_patch_type)    (_patch_type >= PHY_PATCH_TYPE_END && _patch_type <= (PHY_PATCH_TYPE_END + RTK_PATCH_TYPE_FLOWID_MAX))


/*
 * Macro Definition
 */
#define PHYPATCH_DB_GET(_unit, _pPhy_device, _pPatchDb) \
    do { \
        struct rtk_phy_priv *_pPriv = (_pPhy_device)->priv; \
        rt_phy_patch_db_t *_pDb = _pPriv->patch; _pPatchDb = _pDb; \
        /*printk("[PHYPATCH_DB_GET] ? [%s]\n", (_pDb != NULL) ? "E":"N");*/ \
    } while(0)

#define PHYPATCH_TABLE_ASSIGN(_pPatchDb, _table, _idx, _patch_type, _para) \
    do {\
        if (RTK_PATCH_TYPE_IS_DATA(_patch_type)) {\
            _pPatchDb->_table[_idx].patch_type = _patch_type;\
            _pPatchDb->_table[_idx].patch.data.conf = _para;\
            _pPatchDb->_table[_idx].patch.data.size = sizeof(_para);\
        }\
        else if (RTK_PATCH_TYPE_IS_FLOW(_patch_type)) {\
            _pPatchDb->_table[_idx].patch_type = _patch_type;\
            _pPatchDb->_table[_idx].patch.flow_id = _patch_type;\
        }\
        else {\
            _pPatchDb->_table[_idx].patch_type = PHY_PATCH_TYPE_NONE;\
        }\
    } while(0)
#define PHYPATCH_SEQ_TABLE_ASSIGN(_pPatchDb, _idx, _patch_type, _para) PHYPATCH_TABLE_ASSIGN(_pPatchDb, seq_table, _idx, _patch_type, _para)
#define PHYPATCH_CMP_TABLE_ASSIGN(_pPatchDb, _idx, _patch_type, _para) PHYPATCH_TABLE_ASSIGN(_pPatchDb, cmp_table, _idx, _patch_type, _para)

#define PHYPATCH_COMPARE(_mmdpage, _reg, _msb, _lsb, _exp, _real, _mask) \
    do {\
        uint32 _rData = REG32_FIELD_GET(_real, _lsb, _mask);\
        if (_exp != _rData) {\
            osal_printf("PATCH CHECK: %u(0x%X).%u(0x%X)[%u:%u] = 0x%X (!= 0x%X)\n", _mmdpage, _mmdpage, _reg, _reg, _msb, _lsb, _rData, _exp);\
            return RT_ERR_CHECK_FAILED;\
        }\
    } while (0)

/*
 * Function Declaration
 */

extern uint8 phy_patch_op_translate(uint8 patch_mode, uint8 patch_op, uint8 compare_op);
extern int32 phy_patch_op(rt_phy_patch_db_t *pPhy_patchDb, uint32 unit, rtk_port_t port, uint8 portOffset,
                            uint8 patch_op, uint16 portmask, uint16 pagemmd, uint16 addr, uint8 msb, uint8 lsb, uint16 data,
                            uint8 patch_mode);


/* Function Name:
 *      phy_patch
 * Description:
 *      apply initial patch data to PHY
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      portOffset - the index offset of port based the base port in the PHY chip
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHECK_FAILED
 *      RT_ERR_NOT_SUPPORTED
 * Note:
 *      None
 */
extern int32 phy_patch(uint32 unit, rtk_port_t port, uint8 portOffset, uint8 patch_mode);



#endif /* __HAL_PHY_PATCH_H__ */
