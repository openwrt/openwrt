/*
 * Copyright (c) 2005 Atheros Communications Inc.
 * All rights reserved.
 *
 * $ATH_LICENSE_HOSTSDK0_C$
 *
 */

#if defined(AR6001)
#define GPIO_PIN_COUNT 18
#else
#define GPIO_PIN_COUNT 18
#endif

/*
 * Possible values for WMIX_GPIO_SET_REGISTER_CMDID.
 * NB: These match hardware order, so that addresses can
 * easily be computed.
 */
#define GPIO_ID_OUT             0x00000000
#define GPIO_ID_OUT_W1TS        0x00000001
#define GPIO_ID_OUT_W1TC        0x00000002
#define GPIO_ID_ENABLE          0x00000003
#define GPIO_ID_ENABLE_W1TS     0x00000004
#define GPIO_ID_ENABLE_W1TC     0x00000005
#define GPIO_ID_IN              0x00000006
#define GPIO_ID_STATUS          0x00000007
#define GPIO_ID_STATUS_W1TS     0x00000008
#define GPIO_ID_STATUS_W1TC     0x00000009
#define GPIO_ID_PIN0            0x0000000a
#define GPIO_ID_PIN(n)          (GPIO_ID_PIN0+(n))

#define GPIO_LAST_REGISTER_ID   GPIO_ID_PIN(17)
#define GPIO_ID_NONE            0xffffffff
