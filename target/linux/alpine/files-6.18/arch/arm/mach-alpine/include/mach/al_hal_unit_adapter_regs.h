/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#ifndef __AL_HAL_UNIT_ADAPTER_REGS_H__
#define __AL_HAL_UNIT_ADAPTER_REGS_H__

#define AL_PCI_COMMAND		0x04	/* 16 bits */

#define AL_PCI_EXP_CAP_BASE	0x40
#define AL_PCI_EXP_DEVCTL          8       /* Device Control */
#define  AL_PCI_EXP_DEVCTL_BCR_FLR 0x8000  /* Bridge Configuration Retry / FLR */

#define AL_PCI_EXP_LNKSTA            0x12      /* Link Status */
#define  AL_PCI_EXP_LNKSTA_CLS       0x000f  /* Current Link Speed */
#define  AL_PCI_EXP_LNKSTA_CLS_2_5GB 0x01    /* Current Link Speed 2.5GT/s */
#define  AL_PCI_EXP_LNKSTA_CLS_5_0GB 0x02    /* Current Link Speed 5.0GT/s */
#define  AL_PCI_EXP_LNKSTA_CLS_8_0GB 0x03    /* Current Link Speed 8.0GT/s */
#define  AL_PCI_EXP_LNKSTA_NLW       0x03f0  /* Nogotiated Link Width */
#define  AL_PCI_EXP_LNKSTA_NLW_SHIFT 4       /* start of NLW mask in link status */

#define AL_ADAPTER_GENERIC_CONTROL_0	0x1E0

#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC	AL_BIT(18)
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC_ON_FLR	AL_BIT(26)

/*
 * Unit adapter inline functions
 */

#endif
