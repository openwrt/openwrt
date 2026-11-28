/*
* Description
*
* Copyright (C) 2016-2022 Qin.Xia <qin.xia@siflower.com.cn>
*
* Siflower software
*/

#include "se_common.h"


void vlan_se_wait_busy(VLAN_t *priv, u32 reg, u32 mask)
{
    unsigned long timeout = jiffies + HZ;

    do {
        if (!(reg_read(priv, reg) & mask))
            return;

        udelay(100); // spin_lock hold;
    } while (time_after(timeout, jiffies));

    VLAN_DBG(ERR_LV, "timed out\n");
}

