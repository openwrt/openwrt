#ifndef _MAC2DRV_DEF_H_
#define _MAC2DRV_DEF_H_

#include "mac_reg.h"

/* export MAC-related definitions to drv */
/* currently, we redefine reg name used in driver */
/* because we sync header from reg excel */
/* but we do not want to cause driver build fail */
#define B_AX_SDIO_HS0ISR_IND_EN B_AX_HS0ISR_IND_EN
#define B_AX_SDIO_BT_INT B_AX_BT_INT_EN
#define B_AX_WLOCK_1C_B6 B_AX_WLOCK_1C_BIT6

#endif
