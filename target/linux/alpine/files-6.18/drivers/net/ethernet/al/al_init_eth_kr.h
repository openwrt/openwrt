/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */
/**
 *  Ethernet
 *  @{
 * @file   al_init_eth_kr.h
 *
 * @brief auto-negotiation and link training activation sequence
 *
 *
 */

#ifndef __AL_INIT_ETH_KR_H__
#define __AL_INIT_ETH_KR_H__

#include <mach/al_hal_serdes.h>
#include "al_hal_eth_kr.h"

/**
 * execute Auto-negotiation process
 *
 * @param adapter pointer to the private structure
 * @param serdes_obj pointer to serdes private structure
 * @param grp serdes's group
 * @param lane serdes's lane
 * @param an_adv pointer to the AN Advertisement Registers structure
 *        when NULL, the registers will not be updated.
 * @param partner_adv pointer to the AN Advertisement received from the lp
 *
 * @return 0 on success. otherwise on failure.
 */
int al_eth_an_lt_execute(struct al_hal_eth_adapter *adapter,
			 struct al_serdes_obj	*serdes_obj,
			 enum al_serdes_group	grp,
			 enum al_serdes_lane	lane,
			 struct al_eth_an_adv	*an_adv,
			 struct al_eth_an_adv   *partner_adv);

#endif /*__AL_INIT_ETH_KR_H__*/
