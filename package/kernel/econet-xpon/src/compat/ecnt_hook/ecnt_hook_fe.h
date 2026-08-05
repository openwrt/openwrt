/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/
#ifndef _ECNT_HOOK_FE_H_
#define _ECNT_HOOK_FE_H_


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include "ecnt_hook.h"
#include "ecnt_hook_fe_type.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/

/************************************************************************
*                  D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N    D E C L A R A T I O N S
#                  I N L I N E    F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
static inline int FE_API_SET_PACKET_LENGTH(FE_Gdma_Sel_t _gdm_sel, uint _length_long, uint _length_short) {
	struct ecnt_fe_data in_data; 
	int ret = 0;
	
	in_data.function_id = FE_SET_PACKET_LENGTH; 
	in_data.gdm_sel = _gdm_sel; 
	in_data.api_data.pkt_len.length_long = _length_long; 
	in_data.api_data.pkt_len.length_short = _length_short;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_CHANNEL_ENABLE(FE_Gdma_Sel_t _gdm_sel, FE_TXRX_Sel_t _txrx_sel, unchar _channel, FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	 
	in_data.function_id = FE_SET_CHANNEL_ENABLE;
	in_data.gdm_sel = _gdm_sel;
	in_data.txrx_sel = _txrx_sel;
	in_data.channel = _channel;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_MAC_ADDR(FE_Gdma_Sel_t _gdm_sel, unchar *_mac, ushort _mask) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_MAC_ADDR;
	in_data.gdm_sel = _gdm_sel;
	in_data.api_data.mac_addr.mac = _mac;
	in_data.api_data.mac_addr.mask = _mask;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_HWFWD_CHANNEL(FE_Cdma_Sel_t _cdm_sel, unchar _channel, FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_HWFWD_CHANNEL;
	in_data.cdm_sel = _cdm_sel;
	in_data.channel = _channel;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_CHANNEL_RETIRE(FE_Gdma_Sel_t _gdm_sel, unchar _channel, FE_Linkup_t _mode) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_CHANNEL_RETIRE;
	in_data.gdm_sel = _gdm_sel;
	in_data.channel = _channel;
	in_data.api_data.link_mode = _mode;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_CRC_STRIP(FE_Gdma_Sel_t _gdm_sel, FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	 
	in_data.function_id = FE_SET_CRC_STRIP;
	in_data.gdm_sel = _gdm_sel;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_PADDING(FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_PADDING;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_EXT_TPID(uint _tpid) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_EXT_TPID;
	in_data.reg_val = _tpid;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_EXT_TPID(uint *_tpid) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_EXT_TPID;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_tpid = in_data.reg_val;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_FW_CFG(FE_Gdma_Sel_t _gdm_sel, FE_Frame_type_t _dp_sel, FE_Frame_dp_t _dp_type) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_FW_CFG;
	in_data.gdm_sel = _gdm_sel;
	in_data.api_data.fw_cfg.dp_sel = _dp_sel;
	in_data.api_data.fw_cfg.dp_val = _dp_type;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_FW_CFG(FE_Gdma_Sel_t _gdm_sel, uint* pval) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_FW_CFG;
	in_data.gdm_sel = _gdm_sel;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*pval = in_data.reg_val;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_DROP_UDP_CHKSUM_ERR_ENABLE(FE_Gdma_Sel_t _gdm_sel, FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	 
	in_data.function_id = FE_SET_DROP_UDP_CHKSUM_ERR_ENABLE;
	in_data.gdm_sel = _gdm_sel;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_DROP_TCP_CHKSUM_ERR_ENABLE(FE_Gdma_Sel_t _gdm_sel, FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	 
	in_data.function_id = FE_SET_DROP_TCP_CHKSUM_ERR_ENABLE;
	in_data.gdm_sel = _gdm_sel;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}
static inline int FE_API_SET_DROP_IP_CHKSUM_ERR_ENABLE(FE_Gdma_Sel_t _gdm_sel, FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	 
	in_data.function_id = FE_SET_DROP_IP_CHKSUM_ERR_ENABLE;
	in_data.gdm_sel = _gdm_sel;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_DROP_CRC_ERR_ENABLE(FE_Gdma_Sel_t _gdm_sel, FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	 
	in_data.function_id = FE_SET_DROP_CRC_ERR_ENABLE;
	in_data.gdm_sel = _gdm_sel;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_DROP_RUNT_ENABLE(FE_Gdma_Sel_t _gdm_sel, FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	 
	in_data.function_id = FE_SET_DROP_RUNT_ENABLE;
	in_data.gdm_sel = _gdm_sel;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_DROP_LONG_ENABLE(FE_Gdma_Sel_t _gdm_sel, FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	 
	in_data.function_id = FE_SET_DROP_LONG_ENABLE;
	in_data.gdm_sel = _gdm_sel;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_VLAN_CHECK(FE_Gdma_Sel_t _gdm_sel, FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	 
	in_data.function_id = FE_SET_VLAN_CHECK;
	in_data.gdm_sel = _gdm_sel;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_OK_CNT(FE_Gdma_Sel_t _gdm_sel, FE_TXRX_Sel_t _txrx_sel, uint *_cnt) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_OK_CNT;
	in_data.gdm_sel = _gdm_sel;
	in_data.txrx_sel = _txrx_sel;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_cnt = in_data.cnt;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_RX_CRC_ERR_CNT(uint *_cnt) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_RX_CRC_ERR_CNT;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_cnt = in_data.cnt;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_RX_DROP_FIFO_CNT(FE_Gdma_Sel_t _gdm_sel, uint *_cnt) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_RX_DROP_FIFO_CNT;
	in_data.gdm_sel = _gdm_sel;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_cnt = in_data.cnt;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}
static inline int FE_API_GET_RX_DROP_ERR_CNT(FE_Gdma_Sel_t _gdm_sel, uint *_cnt) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_RX_DROP_ERR_CNT;
	in_data.gdm_sel = _gdm_sel;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_cnt = in_data.cnt;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}
static inline int FE_API_GET_OK_BYTE_CNT(FE_Gdma_Sel_t _gdm_sel, FE_TXRX_Sel_t _txrx_sel, uint *_cnt) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_OK_BYTE_CNT;
	in_data.gdm_sel = _gdm_sel;
	in_data.txrx_sel = _txrx_sel;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_cnt = in_data.cnt;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_TX_GET_CNT(FE_Gdma_Sel_t _gdm_sel, uint *_cnt) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_TX_GET_CNT;
	in_data.gdm_sel = _gdm_sel;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_cnt = in_data.cnt;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_TX_DROP_CNT(FE_Gdma_Sel_t _gdm_sel, uint *_cnt) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_TX_DROP_CNT;
	in_data.gdm_sel = _gdm_sel;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_cnt = in_data.cnt;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_TIEM_STAMP(uint *_cnt) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_TIEM_STAMP;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_cnt = in_data.cnt;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_TIME_STAMP(uint ts) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_TIEM_STAMP;
    in_data.timeStamp = ts & 0xffff;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_INS_VLAN_TPID(FE_Gdma_Sel_t _gdm_sel, uint _tpid) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_INS_VLAN_TPID;
	in_data.gdm_sel = _gdm_sel;
	in_data.reg_val = _tpid;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}
static inline int FE_API_SET_VLAN_ENABLE(FE_Gdma_Sel_t _gdm_sel, FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	 
	in_data.function_id = FE_SET_VLAN_ENABLE;
	in_data.gdm_sel = _gdm_sel;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_BLACK_LIST(FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	 
	in_data.function_id = FE_SET_BLACK_LIST;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_ETHER_TYEP(uint _index, FE_Enable_t _enable, FE_PPPOE_t _is_pppoe, uint _value) {
	struct ecnt_fe_data in_data;
	int ret = 0;

	in_data.function_id = FE_SET_ETHER_TYEP;
	in_data.index = _index;
	in_data.api_data.eth_cfg.enable = _enable;
	in_data.api_data.eth_cfg.is_pppoe = _is_pppoe;
	in_data.api_data.eth_cfg.value = _value;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_L2U_KEY(uint _index, FE_L2U_KEY_t _key_sel, uint _key0, uint _key1) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_L2U_KEY;
	in_data.index = _index;
	in_data.api_data.l2u_key.key_sel = _key_sel;
	in_data.api_data.l2u_key.key0 = _key0;
	in_data.api_data.l2u_key.key1 = _key1;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_AC_GROUP_PKT_CNT(uint _index, uint *_cnt) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_AC_GROUP_PKT_CNT;
	in_data.index = _index;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_cnt = in_data.cnt;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_AC_GROUP_BYTE_CNT(uint _index, uint *_cnt, uint *_cnt_hi) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_AC_GROUP_BYTE_CNT;
	in_data.index = _index;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_cnt = in_data.cnt;
	*_cnt_hi = in_data.cnt_hi;
	
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_METER_GROUP(uint _index, uint _value) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_METER_GROUP;
	in_data.index = _index;
	in_data.reg_val = _value;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_GDM_PCP_CODING(FE_Gdma_Sel_t _gdm_sel, FE_TXRX_Sel_t _txrx_sel, FE_PcpMode_t mode) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_GDM_PCP_CODING;
	in_data.gdm_sel = _gdm_sel;
	in_data.txrx_sel = _txrx_sel;
	in_data.api_data.coding_mode = mode;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_CDM_PCP_CODING(FE_Gdma_Sel_t _gdm_sel, FE_TXRX_Sel_t _txrx_sel, FE_PcpMode_t mode) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_CDM_PCP_CODING;
	in_data.gdm_sel = _gdm_sel;
	in_data.txrx_sel = _txrx_sel;
	in_data.api_data.coding_mode = mode;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}	

static inline int FE_API_SET_VIP_ENABLE(uint _index, FE_Enable_t _enable, FE_Patn_type _patten_type, uint _patten) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_VIP_ENABLE;
	in_data.index = _index;
	in_data.api_data.vip_cfg.enable = _enable;
	in_data.api_data.vip_cfg.patten_type = _patten_type;
	in_data.api_data.vip_cfg.patten = _patten;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_ETH_RX_CNT(FE_RxCnt_t *_rxCnt) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_ETH_RX_CNT;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_rxCnt = in_data.api_data.FE_RxCnt;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_ETH_TX_CNT(FE_TxCnt_t *_txCnt) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_ETH_TX_CNT;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_txCnt = in_data.api_data.FE_TxCnt;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_ETH_FRAME_CNT(FE_TXRX_Sel_t _txrx_sel, uint *_cnt) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_ETH_FRAME_CNT;
	in_data.txrx_sel = _txrx_sel;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_cnt = in_data.cnt;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_ETH_ERR_CNT(FE_Err_type_t _type, uint *_cnt) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_ETH_ERR_CNT;
	in_data.err_type = _type;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	*_cnt = in_data.cnt;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_CLEAR_MIB(FE_Gdma_Sel_t _gdm_sel, FE_TXRX_Sel_t _txrx_sel) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	 
	in_data.function_id = FE_SET_CLEAR_MIB;
	in_data.gdm_sel = _gdm_sel;
	in_data.txrx_sel = _txrx_sel;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_CDM_RX_RED_DROP(FE_Cdma_Sel_t _cdm_sel, FE_RedDropQ_Sel_t _dropQ_sel, FE_RedDropMode_Sel_t _dropMode_sel) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_CDM_RX_RED_DROP;
	in_data.cdm_sel = _cdm_sel;
	in_data.dropQ_sel = _dropQ_sel;
	in_data.dropMode_sel = _dropMode_sel;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_GET_CDM_RX_RED_DROP(FE_Cdma_Sel_t _cdm_sel, FE_RedDropQ_Sel_t _dropQ_sel, FE_RedDropMode_Sel_t *_dropMode_sel_p) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_GET_CDM_RX_RED_DROP;
	in_data.cdm_sel = _cdm_sel;
	in_data.dropQ_sel = _dropQ_sel;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
    *_dropMode_sel_p = in_data.dropMode_sel;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_CHANNEL_RETIRE_ALL(FE_Gdma_Sel_t gdm_sel, unchar channel) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_CHANNEL_RETIRE_ALL;
	in_data.gdm_sel = gdm_sel;
	in_data.channel = channel;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_CHANNEL_RETIRE_ONE(FE_Gdma_Sel_t gdm_sel, unchar channel) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_CHANNEL_RETIRE_ONE;
	in_data.gdm_sel = gdm_sel;
	in_data.channel = channel;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_TX_RATE(uint rate,uint mode,uint maxBkSize,uint tick)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_TX_RATE;
	in_data.api_data.rate_cfg.rate = rate;
	in_data.api_data.rate_cfg.mode = mode;	
        in_data.api_data.rate_cfg.maxBkSize = maxBkSize;
        in_data.api_data.rate_cfg.tick = tick;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_RXUC_RATE(uint rate,uint mode)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_RXUC_RATE;
	in_data.api_data.rate_cfg.rate = rate;
	in_data.api_data.rate_cfg.mode = mode;	
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_RXBC_RATE(uint rate,uint mode)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_RXBC_RATE;
	in_data.api_data.rate_cfg.rate = rate;
	in_data.api_data.rate_cfg.mode = mode;	
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_RXMC_RATE(uint rate,uint mode)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_RXMC_RATE;
	in_data.api_data.rate_cfg.rate = rate;
	in_data.api_data.rate_cfg.mode = mode;	
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_RXOC_RATE(uint rate,uint mode)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_SET_RXOC_RATE;
	in_data.api_data.rate_cfg.rate = rate;
	in_data.api_data.rate_cfg.mode = mode;	
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_ADD_VIP_ETHER(ushort type)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_ADD_VIP_ETHER;
	in_data.api_data.vip.type = type;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_ADD_VIP_PPP(ushort type)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_ADD_VIP_PPP;
	in_data.api_data.vip.type = type;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_ADD_VIP_IP(ushort type)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_ADD_VIP_IP;
	in_data.api_data.vip.type = type;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_ADD_VIP_TCP(ushort src,ushort dst)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_ADD_VIP_TCP;
	in_data.api_data.vip.src = src;
	in_data.api_data.vip.dst = dst;	
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_ADD_VIP_UDP(ushort src,ushort dst)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_ADD_VIP_UDP;
	in_data.api_data.vip.src = src;
	in_data.api_data.vip.dst = dst;	
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}


static inline int FE_API_DEL_VIP_ETHER(ushort type)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_DEL_VIP_ETHER;
	in_data.api_data.vip.type = type;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_DEL_VIP_PPP(ushort type)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_DEL_VIP_PPP;
	in_data.api_data.vip.type = type;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_DEL_VIP_IP(ushort type)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_DEL_VIP_IP;
	in_data.api_data.vip.type = type;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_DEL_VIP_TCP(ushort src,ushort dst)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_DEL_VIP_TCP;
	in_data.api_data.vip.src = src;
	in_data.api_data.vip.dst = dst;	
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_DEL_VIP_UDP(ushort src,ushort dst)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_DEL_VIP_UDP;
	in_data.api_data.vip.src = src;
	in_data.api_data.vip.dst = dst;	
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}



static inline int FE_API_ADD_L2LU_VLAN_DSCP(ushort svlan,ushort cvlan,unchar dscp,unchar mask)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_ADD_L2LU_VLAN_DSCP;
	in_data.api_data.l2lu.svlan = svlan;
	in_data.api_data.l2lu.cvlan = cvlan;
	in_data.api_data.l2lu.dscp = dscp;
	in_data.api_data.l2lu.mask = mask;	
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_ADD_L2LU_VLAN_TRFC(ushort svlan,ushort cvlan,unchar trfc,unchar mask)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_ADD_L2LU_VLAN_TRFC;
	in_data.api_data.l2lu.svlan = svlan;
	in_data.api_data.l2lu.cvlan = cvlan;
	in_data.api_data.l2lu.dscp = trfc;
	in_data.api_data.l2lu.mask = mask;	
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_DEL_L2LU_VLAN_DSCP(ushort svlan,ushort cvlan,unchar dscp,unchar mask)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_DEL_L2LU_VLAN_DSCP;
	in_data.api_data.l2lu.svlan = svlan;
	in_data.api_data.l2lu.cvlan = cvlan;
	in_data.api_data.l2lu.dscp = dscp;
	in_data.api_data.l2lu.mask = mask;	
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_DEL_L2LU_VLAN_TRFC(ushort svlan,ushort cvlan,unchar trfc,unchar mask)
{
	struct ecnt_fe_data in_data;
	int ret = 0;
	
	in_data.function_id = FE_DEL_L2LU_VLAN_TRFC;
	in_data.api_data.l2lu.svlan = svlan;
	in_data.api_data.l2lu.cvlan = cvlan;
	in_data.api_data.l2lu.dscp = trfc;
	in_data.api_data.l2lu.mask = mask;	
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int FE_API_SET_TX_FAVOR_OAM_ENABLE(FE_Enable_t _enable) {
	struct ecnt_fe_data in_data;
	int ret = 0;
	 
	in_data.function_id = FE_SET_TX_FAVOR_OAM_ENABLE;
	in_data.api_data.enable = _enable;
	ret = __ECNT_HOOK(ECNT_FE, ECNT_FE_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

#endif /* _ECNT_HOOK_FE_H_ */


