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
#include <linux/types.h>
#include <linux/phy.h>

typedef enum rtk_phypatch_type_e
{
    PHY_PATCH_TYPE_NONE = 0,
    PHY_PATCH_TYPE_TOP = 1,
    PHY_PATCH_TYPE_SDS,
    PHY_PATCH_TYPE_AFE,
    PHY_PATCH_TYPE_UC,
    PHY_PATCH_TYPE_UC2,
    PHY_PATCH_TYPE_NCTL0,
    PHY_PATCH_TYPE_NCTL1,
    PHY_PATCH_TYPE_NCTL2,
    PHY_PATCH_TYPE_ALGXG,
    PHY_PATCH_TYPE_ALG1G,
    PHY_PATCH_TYPE_NORMAL,
    PHY_PATCH_TYPE_DATARAM,
    PHY_PATCH_TYPE_RTCT,
    PHY_PATCH_TYPE_END
} rtk_phypatch_type_t;

#define RTK_PATCH_TYPE_FLOW(_id)               (PHY_PATCH_TYPE_END + _id)
#define RTK_PATCH_TYPE_FLOWID_MAX              PHY_PATCH_TYPE_END
#define RTK_PATCH_SEQ_MAX                      ( PHY_PATCH_TYPE_END + RTK_PATCH_TYPE_FLOWID_MAX -1)

typedef struct rtk_hwpatch_s
{
    u8 patch_op;
    u8 portmask;
    u16 pagemmd;
    u16 addr;
    u8 msb;
    u8 lsb;
    u16 data;
} rtk_hwpatch_t;

typedef struct rt_phy_patch_db_s
{
    /* patch operation */
    int (*fPatch_op)(struct phy_device *phydev, rtk_hwpatch_t *pPatch_data);
    int (*fPatch_flow)(struct phy_device *phydev, u8 patch_flow);

    /* patch data */
    struct
    {
        rtk_phypatch_type_t patch_type;
        union
        {
            struct
            {
                rtk_hwpatch_t *conf;
                u32 size;
            } data;
            u8 flow_id;
        } patch;
    } table[RTK_PATCH_SEQ_MAX];

} rt_phy_patch_db_t;

/*
 * Symbol Definition
 */
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

/* 200~255 control op */
#define RTK_PATCH_OP_DELAY_MS                200


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
#define PHYPATCH_DB_GET(_pPhy_device, _pPatchDb) \
    do { \
        struct rtl826x_priv *_pPriv = (_pPhy_device)->priv; \
        rt_phy_patch_db_t *_pDb = _pPriv->patch; _pPatchDb = _pDb; \
    } while(0)

#define PHYPATCH_TABLE_ASSIGN(_pPatchDb, _idx, _patch_type, _para) \
    do {\
        if (RTK_PATCH_TYPE_IS_DATA(_patch_type)) {\
            _pPatchDb->table[_idx].patch_type = _patch_type;\
            _pPatchDb->table[_idx].patch.data.conf = _para;\
            _pPatchDb->table[_idx].patch.data.size = sizeof(_para);\
        }\
        else if (RTK_PATCH_TYPE_IS_FLOW(_patch_type)) {\
            _pPatchDb->table[_idx].patch_type = _patch_type;\
            _pPatchDb->table[_idx].patch.flow_id = _patch_type;\
        }\
        else {\
            _pPatchDb->table[_idx].patch_type = PHY_PATCH_TYPE_NONE;\
        }\
    } while(0)

/*
 * Function Declaration
 */

int phy_patch_op(rt_phy_patch_db_t *pPhy_patchDb, struct phy_device *phydev,
                            u8 patch_op, u16 portmask, u16 pagemmd, u16 addr, u8 msb, u8 lsb, u16 data);

int phy_patch(struct phy_device *phydev);

#endif /* __HAL_PHY_PATCH_H__ */
