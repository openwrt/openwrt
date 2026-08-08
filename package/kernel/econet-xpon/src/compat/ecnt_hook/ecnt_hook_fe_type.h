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
#ifndef _ECNT_HOOK_FE_TYPE_H_
#define _ECNT_HOOK_FE_TYPE_H_


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
	
/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define FE_WAN_RX_UC_RATE 	0
#define FE_WAN_RX_BC_RATE 	1
#define FE_WAN_RX_MC_RATE 	2
#define FE_WAN_RX_OC_RATE 	3
	
#define CDM_TX_ENCODING_SHIFT					(0)
#define CDM_TX_ENCODING_MASK					(0xF<<CDM_TX_ENCODING_SHIFT)
#define CDM_RX_DECODING_SHIFT					(4)
#define CDM_RX_DECODING_MASK					(0xF<<CDM_RX_DECODING_SHIFT)
#define GDM_TX_ENCODING_SHIFT					(8)
#define GDM_TX_ENCODING_MASK					(0xF<<GDM_TX_ENCODING_SHIFT)
#define GDM_RX_DECODING_SHIFT					(12)
#define GDM_RX_DECODING_MASK					(0xF<<GDM_RX_DECODING_SHIFT)

/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/

typedef enum {
	ECNT_FE_API,
}FE_Api_SubType_t;

typedef enum {
	FE_GDM_SEL_GDMA1 = 0 ,
	FE_GDM_SEL_GDMA2
} FE_Gdma_Sel_t;

typedef enum {
	FE_CDM_SEL_CDMA1 = 0 ,
	FE_CDM_SEL_CDMA2
} FE_Cdma_Sel_t;

typedef enum {
	FE_RED_DROP_Q0 = 0 ,
	FE_RED_DROP_Q1L,
	FE_RED_DROP_Q1H
} FE_RedDropQ_Sel_t;

typedef enum {
	FE_RATE_OR_FULL_DROP = 0 ,
	FE_RATE_OR_THRL_DROP,
	FE_RATE_AND_FULL_DROP,
	FE_RATE_AND_THRL_DROP
} FE_RedDropMode_Sel_t;

typedef enum {
	FE_GDM_SEL_TX = 0 ,
	FE_GDM_SEL_RX 
} FE_TXRX_Sel_t;

typedef enum {
	FE_DISABLE = 0 ,
	FE_ENABLE 
} FE_Enable_t;	

typedef enum {
	FE_NOT_PPPOE = 0 ,
	FE_IS_PPPOE 
} FE_PPPOE_t;	

typedef enum {
	ETH_TYPE = 0 ,
	PPPOE_TYPE,
	IP_TYPE,
	TCP_TYPE,
	UDP_TYPE,
} FE_Patn_type;	

typedef enum {
	FE_KEY0 = 0 ,
	FE_KEY1 
} FE_L2U_KEY_t;	

 typedef enum {
	 FE_LINKUP = 0,
	 FE_LINKDOWN
 } FE_Linkup_t;

typedef enum {
	FE_PCP_MODE_DISABLE = 0,
	FE_PCP_MODE_8B0D = 1,
	FE_PCP_MODE_7B1D = 2, 
	FE_PCP_MODE_6B2D = 4,
	FE_PCP_MODE_5B3D = 8
} FE_PcpMode_t;

typedef enum {
	FE_DP_QDMA1_CPU = 0,
	FE_DP_GDM1,
	FE_DP_GDM2,
	FE_DP_QDMA1_HWF,
	FE_DP_PPE,
	FE_DP_QDMA2_CPU,
	FE_DP_QDMA2_HWF,
	FE_DP_DISCARD
} FE_Frame_dp_t;

typedef enum {
	FE_TYPE_OC = 0,
	FE_TYPE_MC,
	FE_TYPE_BC,
	FE_TYPE_UC,
} FE_Frame_type_t ;

typedef enum {
	FE_ERR_CRC = 0,
	FE_ERR_LONG,
	FE_ERR_RUNT,
} FE_Err_type_t ;

typedef enum {
	FE_SET_PACKET_LENGTH = 0,
	FE_SET_CHANNEL_ENABLE,
	FE_SET_MAC_ADDR,
	FE_SET_HWFWD_CHANNEL,
	FE_SET_CHANNEL_RETIRE,
	FE_SET_CRC_STRIP,
	FE_SET_PADDING,
	FE_GET_EXT_TPID,
	FE_SET_EXT_TPID,
	FE_GET_FW_CFG,
	FE_SET_FW_CFG,
	FE_SET_DROP_UDP_CHKSUM_ERR_ENABLE,
	FE_SET_DROP_TCP_CHKSUM_ERR_ENABLE,
	FE_SET_DROP_IP_CHKSUM_ERR_ENABLE,
	FE_SET_DROP_CRC_ERR_ENABLE,
	FE_SET_DROP_RUNT_ENABLE,
	FE_SET_DROP_LONG_ENABLE,
	FE_SET_VLAN_CHECK,
	FE_GET_OK_CNT,
	FE_GET_RX_CRC_ERR_CNT,
	FE_GET_RX_DROP_FIFO_CNT,
	FE_GET_RX_DROP_ERR_CNT,
	FE_GET_OK_BYTE_CNT,
	FE_GET_TX_GET_CNT,
	FE_GET_TX_DROP_CNT,
	FE_GET_TIEM_STAMP,
	FE_SET_TIEM_STAMP,
	FE_SET_INS_VLAN_TPID,
	FE_SET_VLAN_ENABLE,
	FE_SET_BLACK_LIST,
	FE_SET_ETHER_TYEP,
	FE_SET_L2U_KEY,
	FE_GET_AC_GROUP_PKT_CNT,
	FE_GET_AC_GROUP_BYTE_CNT,
	FE_SET_METER_GROUP,
	FE_GET_METER_GROUP,
	FE_SET_GDM_PCP_CODING,
	FE_SET_CDM_PCP_CODING,
	FE_SET_VIP_ENABLE,
	FE_GET_ETH_RX_CNT,
	FE_GET_ETH_TX_CNT,
	FE_GET_ETH_FRAME_CNT,
	FE_GET_ETH_ERR_CNT,
	FE_SET_CLEAR_MIB,
	FE_SET_CDM_RX_RED_DROP,
	FE_GET_CDM_RX_RED_DROP,
	FE_SET_CHANNEL_RETIRE_ALL,
	FE_SET_CHANNEL_RETIRE_ONE,	
	FE_SET_TX_RATE,
	FE_SET_RXUC_RATE,
	FE_SET_RXBC_RATE,
	FE_SET_RXMC_RATE,
	FE_SET_RXOC_RATE,
	FE_ADD_VIP_ETHER,
	FE_ADD_VIP_PPP,
	FE_ADD_VIP_IP,
	FE_ADD_VIP_TCP,
	FE_ADD_VIP_UDP,
	FE_DEL_VIP_ETHER,
	FE_DEL_VIP_PPP,
	FE_DEL_VIP_IP,
	FE_DEL_VIP_TCP,
	FE_DEL_VIP_UDP,
	FE_ADD_L2LU_VLAN_DSCP,
	FE_ADD_L2LU_VLAN_TRFC,
	FE_DEL_L2LU_VLAN_DSCP,
	FE_DEL_L2LU_VLAN_TRFC,	
	FE_SET_TX_FAVOR_OAM_ENABLE,
	FE_FUNCTION_MAX_NUM,
} FE_HookFunctionID_t ;

typedef struct FE_TxCnt_s{
	uint txFrameCnt;
	uint txFrameLen;
	uint txDropCnt;
	uint txBroadcastCnt;
	uint txMulticastCnt;
	uint txLess64Cnt;
	uint txMore1518Cnt;
	uint txEq64Cnt;
	uint txFrom65To127Cnt;
	uint txFrom128To255Cnt;
	uint txFrom256To511Cnt;
	uint txFrom512To1023Cnt;
	uint txFrom1024To1518Cnt;
}FE_TxCnt_t;

typedef struct FE_RxCnt_s{
	uint rxFrameCnt;
	uint rxFrameLen;
	uint rxDropCnt;
	uint rxBroadcastCnt;
	uint rxMulticastCnt;
	uint rxCrcCnt;
	uint rxFragFameCnt;
	uint rxJabberFameCnt;
	uint rxLess64Cnt;
	uint rxMore1518Cnt;
	uint rxEq64Cnt;
	uint rxFrom65To127Cnt;
	uint rxFrom128To255Cnt;
	uint rxFrom256To511Cnt;
	uint rxFrom512To1023Cnt;
	uint rxFrom1024To1518Cnt;
}FE_RxCnt_t;

struct ecnt_fe_data {
	FE_HookFunctionID_t function_id;
	FE_Gdma_Sel_t gdm_sel;
    FE_Cdma_Sel_t cdm_sel;
    FE_RedDropQ_Sel_t dropQ_sel;
    FE_RedDropMode_Sel_t dropMode_sel;
	FE_TXRX_Sel_t txrx_sel;
	FE_Err_type_t err_type;
	uint 	channel;
	uint 	index;
	uint 	reg_val;
	uint 	cnt;
	uint 	cnt_hi;
    uint 	timeStamp;
	union {
		FE_Enable_t enable;
		FE_Linkup_t link_mode;
		FE_PcpMode_t coding_mode;
		uint meter_rate;
		struct {
			uint byteCnt_L;
			uint byteCnt_H;
		}byteCnt;
		struct {
			uint length_long;
			uint length_short;
		} pkt_len;
		struct {
			unchar *mac;
			ushort mask;
		} mac_addr;
		struct {
			FE_Frame_type_t dp_sel;
			FE_Frame_dp_t dp_val;
		} fw_cfg;
		struct {
			FE_Enable_t enable;
			FE_PPPOE_t is_pppoe;
			uint value;
		} eth_cfg;
		struct {
			FE_L2U_KEY_t key_sel;
			uint key0;
			uint key1;
		} l2u_key;
		struct {
			FE_Enable_t enable;
			FE_Patn_type patten_type;
			uint patten;
		} vip_cfg;
		struct {
			uint rate;
			uint mode;
                        uint maxBkSize;
                        uint tick;
		}rate_cfg;
		struct {
			ushort type;
			ushort src;
			ushort dst;
			unchar mode;
		}vip;
		struct {
			unchar mask;
			unchar dscp;
			ushort svlan;
			ushort cvlan;
		}l2lu;
		FE_RxCnt_t FE_RxCnt;
		FE_TxCnt_t FE_TxCnt;
	} api_data;
	int retValue;
};

	
/************************************************************************
*                  D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N    D E C L A R A T I O N S
#                  I N L I N E    F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

#endif /* _ECNT_HOOK_QDMA_TYPE_H_ */

