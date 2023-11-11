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
#ifndef _TRX_TEST_H_
#define _TRX_TEST_H_

#ifdef CONFIG_PHL_TEST_SUITE

#define MAX_TEST_TXREQ_NUM  256
#define MAX_TEST_PAYLOAD_NUM  MAX_TEST_TXREQ_NUM
#define MAX_TEST_RXREQ_NUM  256
#define MAX_TEST_PAYLOAD_SIZE  2308


enum test_mode {
	TEST_MODE_PHL_TX_RING_TEST = 1,
	TEST_MODE_PHL_RX_RING_TEST = 2,
	TEST_MODE_PHL_RING_LOOPBACK = 3,
	TEST_MODE_PHL_PCIE_LOOPBACK = 4,
	TEST_MODE_HAL_TX_TEST = 5,
	TEST_MODE_HAL_RX_TEST = 6,
	TEST_MODE_HAL_WP_REPORT_PARSE = 7,
	TEST_MODE_HAL_RXDESC_PARSE = 8,
	TEST_MODE_HAL_RXBD_PARSE = 9
};

enum pkt_type {
	TEST_PKT_TYPE_UNI = 1,
	TEST_PKT_TYPE_MC = 2,
	TEST_PKT_TYPE_BC = 3,
	TEST_PKT_TYPE_MAX = 0xFF,
};

struct rtw_tx_cap {
	u16 macid;
	u8 tid;
	u8 wmm; /* for halmac add role */
	u8 dma_ch;
	u8 band;
	u8 force_txcap;
	u16 rate;
	u8 bw;
	u8 gi_ltf;
	u8 stbc;
	u8 ldpc;
	u8 bk;
	u8 type;
};

struct rtw_pool {
	u8 *buf;
	u32 buf_len;
	_os_list idle_list;
	_os_list busy_list;
	_os_lock idle_lock;
	_os_lock busy_lock;
	u32 total_cnt;
	u32 idle_cnt;
	u32 busy_cnt;
};

struct rtw_test_rx {
	_os_list list;
	u32 test_id;
	struct rtw_recv_pkt rx;
	u8 *tpkt; 		/* for loopback mode */
};

struct rtw_payload {
	_os_list list;
	u32 test_id;
	struct rtw_pkt_buf_list pkt;
	void *os_rsvd[1];
};

struct rtw_trx_test_param {
	u8 is_trx_test_end;
	u8 mode;
	/* parameter for trx resource*/
	u32 tx_req_num;
	u32 rx_req_num;
	u32 tx_payload_num;
	u32 tx_payload_size;
	enum pkt_type pkt_type;
	u8 ap_mode;
	u8 trx_mode;
	u8 qta_mode;
	u8 qos;
	u8 cur_addr[6]; 	/* mac address of this device */
	u8 sta_addr[6];		/* mac address of associating device */
	u8 bssid[6];
	/* parameter for tx capability */
	struct rtw_t_meta_data tx_cap;
	/* parameter for hw configure */
	/* misc */

};

struct phl_trx_test {
	_os_list rx_q;
	_os_lock rx_q_lock;
	struct rtw_pool tx_req_pool;
	struct rtw_pool rx_req_pool;
	struct rtw_pool tx_pkt_pool;
	struct test_obj_ctrl_interface trx_test_obj;
	struct rtw_trx_test_param test_param;
	struct rtw_phl_handler test_rxq_handler;
};

#define WHDR_OFST_FRAME_CONTROL 0
#define WHDR_OFST_DURATION 2
#define WHDR_OFST_ADDRESS1 4
#define WHDR_OFST_ADDRESS2 10
#define WHDR_OFST_ADDRESS3 16
#define WHDR_OFST_SEQUENCE 22
#define WHDR_OFST_ADDRESS4 24
#define WHDR_QOS_LENGTH 2

#define SET_WHDR_PROTOCOL_VERSION(_hdr, _val) \
	SET_BITS_TO_LE_2BYTE(_hdr, 0, 2, _val)
#define SET_WHDR_TYPE(_hdr, _val) \
	SET_BITS_TO_LE_2BYTE(_hdr, 2, 2, _val)
#define SET_WHDR_SUBTYPE(_hdr, _val) \
	SET_BITS_TO_LE_2BYTE(_hdr, 4, 4, _val)
#define SET_WHDR_TO_DS(_hdr, _val) \
	SET_BITS_TO_LE_2BYTE(_hdr, 8, 1, _val)
#define SET_WHDR_FROM_DS(_hdr, _val) \
	SET_BITS_TO_LE_2BYTE(_hdr, 9, 1, _val)
#define SET_WHDR_MORE_FRAG(_hdr, _val) \
	SET_BITS_TO_LE_2BYTE(_hdr, 10, 1, _val)
#define SET_WHDR_RETRY(_hdr, _val) \
	SET_BITS_TO_LE_2BYTE(_hdr, 11, 1, _val)
#define SET_WHDR_PWR_MGNT(_hdr, _val) \
	SET_BITS_TO_LE_2BYTE(_hdr, 12, 1, _val)
#define SET_WHDR_MORE_DATA(_hdr, _val) \
	SET_BITS_TO_LE_2BYTE(_hdr, 13, 1, _val)
#define SET_WHDR_WEP(_hdr, _val) \
	SET_BITS_TO_LE_2BYTE(_hdr, 14, 1, _val)
#define SET_WHDR_ORDER(_hdr, _val) \
	SET_BITS_TO_LE_2BYTE(_hdr, 15, 1, _val)
#define SET_WHDR_QOS_EN(_hdr, _val) \
	SET_BITS_TO_LE_2BYTE(_hdr, 7, 1, _val)
#define SET_WHDR_DURATION(_hdr, _val) \
	WriteLE2Byte(_hdr + WHDR_OFST_DURATION, _val)
#define SET_WHDR_ADDRESS1(_drv, _hdr, _val) \
	_os_mem_cpy(_drv, _hdr + WHDR_OFST_ADDRESS1, _val, 6)
#define SET_WHDR_ADDRESS2(_drv, _hdr, _val) \
	_os_mem_cpy(_drv, _hdr + WHDR_OFST_ADDRESS2, _val, 6)
#define SET_WHDR_ADDRESS3(_drv, _hdr, _val) \
	_os_mem_cpy(_drv, _hdr + WHDR_OFST_ADDRESS3, _val, 6)
#define SET_WHDR_FRAGMENT_SEQUENCE(_hdr, _val) \
	WriteLE2Byte(_hdr + WHDR_OFST_SEQUENCE, _val)
#define SET_WHDR_ADDRESS4(_drv, _hdr, _val) \
	_os_mem_cpy(_drv, _hdr + WHDR_OFST_ADDRESS4, _val, 6)


#define SET_WHDR_QOS_CTRL_STA_DATA_TID(_qos, _value) \
	SET_BITS_TO_LE_2BYTE((u8 *)_qos, 0, 4, (u8)(_value))

#define SET_WHDR_QOS_CTRL_STA_DATA_EOSP(_qos, _value) \
	SET_BITS_TO_LE_2BYTE((u8 *)_qos, 4, 1, (u8)(_value))

#define SET_WHDR_QOS_CTRL_STA_DATA_ACK_POLICY(_qos, _value) \
	SET_BITS_TO_LE_2BYTE((u8 *)_qos, 5, 2, (u8)(_value))

#define SET_WHDR_QOS_CTRL_STA_DATA_AMSDU(_qos, _value) \
	SET_BITS_TO_LE_2BYTE((u8 *)_qos, 7, 1, (u8)(_value))

#define SET_WHDR_QOS_CTRL_STA_DATA_TXOP(_qos, _value) \
	SET_BITS_TO_LE_1BYTE((u8 *)_qos + 1, 0, 8, (u8)(_value))


enum rtw_phl_status phl_trx_test_init(void *phl);
void phl_trx_test_deinit(void *phl);
void rtw_phl_trx_default_param(void *phl, 
					struct rtw_trx_test_param *test_param);
enum rtw_phl_status rtw_phl_trx_testsuite(void *phl, 
					struct rtw_trx_test_param *test_param);

#else /*!CONFIG_PHL_TEST_SUITE*/
#define phl_trx_test_init(phl) RTW_PHL_STATUS_SUCCESS
#define phl_trx_test_deinit(phl)
#define rtw_phl_trx_default_param(phl, test_param)
#define rtw_phl_trx_testsuite(phl, test_param) RTW_PHL_STATUS_SUCCESS
#endif

#endif /*_TRX_TEST_H_*/
