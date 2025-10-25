/*
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (c) 2023 Realtek Semiconductor Corp. All rights reserved.
 */

/*
 * Include Files
 */
#include "realtek.h"

/*
 * Function Declaration
 */
int phy_patch_op(rt_phy_patch_db_t *pPhy_patchDb, struct phy_device *phydev, u8 patch_op, u16 portmask, u16 pagemmd, u16 addr, u8 msb, u8 lsb, u16 data)
{
    rtk_hwpatch_t op;

    op.patch_op = patch_op;
    op.portmask = portmask;
    op.pagemmd  = pagemmd;
    op.addr     = addr;
    op.msb      = msb;
    op.lsb      = lsb;
    op.data     = data;

    return pPhy_patchDb->fPatch_op(phydev, &op);
}

static int _phy_patch_process(struct phy_device *phydev, rtk_hwpatch_t *pPatch, int size)
{
    int i = 0;
    int ret = 0;
    int n;
    rtk_hwpatch_t *patch = pPatch;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phydev, pPatchDb);

    if (size <= 0)
    {
        return 0;
    }
    n = size / sizeof(rtk_hwpatch_t);

    for (i = 0; i < n; i++)
    {
        ret = pPatchDb->fPatch_op(phydev, &patch[i]);
        if (ret < 0)
        {
            phydev_err(phydev, "%s failed! %u[%u][0x%X][0x%X][0x%X] ret=0x%X\n", __FUNCTION__,
                        i + 1, patch[i].patch_op, patch[i].pagemmd, patch[i].addr, patch[i].data, ret);
            return ret;
        }

    }
    return 0;
}

int phy_patch(struct phy_device *phydev)
{
    int ret = 0;
    u32 i = 0;
    u8 patch_type = 0;
    rt_phy_patch_db_t *pPatchDb = NULL;

    PHYPATCH_DB_GET(phydev, pPatchDb);

    if ((pPatchDb == NULL) || (pPatchDb->fPatch_op == NULL) || (pPatchDb->fPatch_flow == NULL))
    {
        phydev_err(phydev, "phy_patch, db is NULL\n");
        return -EINVAL;
    }

    for (i = 0; i < RTK_PATCH_SEQ_MAX; i++)
    {
        patch_type = pPatchDb->table[i].patch_type;
        phydev_dbg(phydev, "phy_patch: pPatchDb->table[%u] patch_type:%u\n", i, patch_type);

        if (RTK_PATCH_TYPE_IS_DATA(patch_type))
        {
            ret = _phy_patch_process(phydev, pPatchDb->table[i].patch.data.conf, pPatchDb->table[i].patch.data.size);
            if (ret < 0)
            {
                phydev_err(phydev, "id:%u patch-%u failed. ret:0x%X\n", i, patch_type, ret);
                return ret;
            }
        }
        else if (RTK_PATCH_TYPE_IS_FLOW(patch_type))
        {
            ret = pPatchDb->fPatch_flow(phydev, pPatchDb->table[i].patch.flow_id);
            if (ret < 0)
            {
                phydev_err(phydev, "id:%u patch-%u failed. ret:0x%X\n", i, patch_type, ret);
                return ret;
            };
        }
        else
        {
            break;
        }
    }

    return 0;
}
