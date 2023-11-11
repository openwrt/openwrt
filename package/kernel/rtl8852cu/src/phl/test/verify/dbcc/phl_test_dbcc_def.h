/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
 *****************************************************************************/
#ifndef _PHL_TEST_DBCC_DEF_H_
#define _PHL_TEST_DBCC_DEF_H_

#ifdef CONFIG_PHL_TEST_VERIFY
enum dbcc_cmd_status {
	DBCC_STATUS_NOT_INIT = 0,
	DBCC_STATUS_INIT = 1,
	DBCC_STATUS_WAIT_CMD = 2,
	DBCC_STATUS_CMD_EVENT = 3,
	DBCC_STATUS_RUN_CMD = 4,
};

/* dbcc command class */
enum dbcc_class {
	DBCC_CLASS_CONFIG = 0,
	DBCC_CLASS_MAX,
};

enum DBCC_tx_method {
	DBCC_PKT_TX = 0,
	DBCC_PMACT_TX,
	DBCC_TMACT_TX,
	DBCC_FW_PMAC_TX,
};

enum dbcc_tx_cmd {
	DBCC_TX_NONE = 0,
	DBCC_TX_PACKETS,
	DBCC_TX_CONTINUOUS,
	DBCC_TX_CMD_MAX,
};

/* dbcc config command */
enum dbcc_config_cmd {
	DBCC_CONFIG_CMD_SET_DBCC = 0,
	DBCC_CONFIG_CMD_MAX,
};

/* dbcc rx command */
enum dbcc_rx_cmd {
	DBCC_RX_CMD_PHY_CRC_OK = 0,
	DBCC_RX_CMD_PHY_CRC_ERR = 1,
	DBCC_RX_CMD_MAC_CRC_OK = 2,
	DBCC_RX_CMD_MAC_CRC_ERR = 3,
	DBCC_RX_CMD_DRV_CRC_OK = 4,
	DBCC_RX_CMD_DRV_CRC_ERR = 5,
	DBCC_RX_CMD_MAX,
};

/*
 *	Command structure definition.
 *	Fixed part would be dbcc_class/cmd/cmd_ok for command and report parsing.
 *	Data members might have input or output usage.
 */

struct dbcc_cmd_hdr {
	u8 arg_class;
	u8 cmd;
};

struct dbcc_config_arg {
	u8 arg_class;
	u8 cmd;
	u8 dbcc_en;
	u8 macid;
};

struct dbcc_tx_arg {
	u8 arg_class;
	u8 cmd;
	u8 macid;
	u8 tx_method;
	u16 tx_count;
	u16 data_rate;
};

struct dbcc_tx_cnt_arg {
	u8 arg_class;
	u8 cmd;
	u8 band;
	u8 params;
};

struct dbcc_rx_arg {
	u8 arg_class;
	u8 cmd;
	u8 cmd_ok;
	u8 status;
	u32 rx_ok;
	u32 rx_err;
	u8 rssi;
	u8 rx_path;
	u8 rx_evm;
	u8 user;
	u8 strm;
	u8 rxevm_table;
};
#endif /* CONFIG_PHL_TEST_VERIFY */
#endif /* _PHL_TEST_DBCC_DEF_H_ */
