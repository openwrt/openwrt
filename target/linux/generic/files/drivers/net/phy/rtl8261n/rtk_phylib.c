/*
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (c) 2023 Realtek Semiconductor Corp. All rights reserved.
 */

#include "rtk_phylib.h"
#include <linux/phy.h>


/* OSAL */

void rtk_phylib_udelay(uint32 usec)
{
    if (1000 <= usec)
    {
        mdelay(usec/1000);
        usec = usec % 1000;
    }
    udelay(usec);
}


/* Register Access APIs */
int32 rtk_phylib_mmd_write(rtk_phydev *phydev, uint32 mmd, uint32 reg, uint8 msb, uint8 lsb, uint32 data)
{
    int32  ret = 0;
    uint32 mask = 0;
    mask = UINT32_BITS_MASK(msb,lsb);

    ret = phy_modify_mmd(phydev, mmd, reg, mask, (data << lsb));

    return ret;
}

int32 rtk_phylib_mmd_read(rtk_phydev *phydev, uint32 mmd, uint32 reg, uint8 msb, uint8 lsb, uint32 *pData)
{
    int32  ret = 0;
    uint32 rData = 0;
    uint32 mask = 0;
    mask = UINT32_BITS_MASK(msb,lsb);

    rData =  phy_read_mmd(phydev, mmd, reg);

    *pData = REG32_FIELD_GET(rData, lsb, mask);
    return ret;
}
