/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _MAC_AX_PWR_H_
#define _MAC_AX_PWR_H_

#include "../type.h"
#include "init.h"

/**
 * Power switch command description
 * @PWR_CMD_WRITE:
 * @PWR_CMD_POLL:
 * @PWR_CMD_DELAY:
 * @PWR_CMD_END:
 */
#define	PWR_CMD_WRITE		0
#define	PWR_CMD_POLL		1
#define	PWR_CMD_DELAY		2
#define	PWR_CMD_END		3

#define	PWR_CAV_MSK		BIT(0)
#define	PWR_CBV_MSK		BIT(1)
#define	PWR_CCV_MSK		BIT(2)
#define	PWR_CDV_MSK		BIT(3)
#define	PWR_CEV_MSK		BIT(4)
#define	PWR_CFV_MSK		BIT(5)
#define	PWR_CGV_MSK		BIT(6)
#define	PWR_CTV_MSK		BIT(7)
#define	PWR_CVALL_MSK		0xFF

#define	PWR_INTF_MSK_SDIO	BIT(0)
#define	PWR_INTF_MSK_USB2	BIT(1)
#define	PWR_INTF_MSK_USB3	BIT(2)
#define	PWR_INTF_MSK_USB	(PWR_INTF_MSK_USB3 | PWR_INTF_MSK_USB2)
#define	PWR_INTF_MSK_PCIE	BIT(3)
#define	PWR_INTF_MSK_ALL	0xF

#define PWR_BASE_MAC		0
#define PWR_BASE_USB		1
#define PWR_BASE_PCIE		2

#define	PWR_DELAY_US		0
#define	PWR_DELAY_MS		1

#define PWR_POLL_CNT		2000
#define PWR_POLL_DLY_US		1000

#define LPS_LEAVE_GPIO		10
#define LPS_POLL_CNT		10000
#define LPS_POLL_DLY_US		50

#define PWR_POST_SWITCH		0
#define PWR_PRE_SWITCH		1
#define PWR_END_SWITCH		2

/**
 * @struct mac_pwr_cfg
 * @brief mac_pwr_cfg
 *
 * @var mac_pwr_cfg::addr
 * Please Place Description here.
 * @var mac_pwr_cfg::cut_msk
 * Please Place Description here.
 * @var mac_pwr_cfg::intf_msk
 * Please Place Description here.
 * @var mac_pwr_cfg::base
 * Please Place Description here.
 * @var mac_pwr_cfg::cmd
 * Please Place Description here.
 * @var mac_pwr_cfg::msk
 * Please Place Description here.
 * @var mac_pwr_cfg::val
 * Please Place Description here.
 */
struct mac_pwr_cfg {
	u16 addr;
	u8 cut_msk;
	u8 intf_msk;
	u8 base:4;
	u8 cmd:4;
	u8 msk;
	u8 val;
};

/**
 * @brief pwr_seq_start
 *
 * @param *adapter
 * @param **seq
 * @return Please Place Description here.
 * @retval u32
 */
u32 pwr_seq_start(struct mac_ax_adapter *adapter, struct mac_pwr_cfg **seq);

/**
 * @brief mac_pwr_switch
 *
 * @param *adapter
 * @param on
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_pwr_switch(struct mac_ax_adapter *adapter, u8 on);

#endif
