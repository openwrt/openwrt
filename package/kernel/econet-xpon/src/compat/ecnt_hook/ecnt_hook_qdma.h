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
#ifndef _ECNT_HOOK_QDMA_H_
#define _ECNT_HOOK_QDMA_H_


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/	
#include "ecnt_hook.h"
#include "ecnt_hook_qdma_type.h"


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

static inline int QDMA_API_INIT(unsigned int mainType, QDMA_InitCfg_t *pInitCfg) {
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_INIT;
	in_data.qdma_private.pInitCfg = pInitCfg;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_DEINIT(unsigned int mainType){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_DEINIT;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_TX_DMA_MODE(unsigned int mainType, QDMA_Mode_t txMode){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_TX_DMA_MODE;
	in_data.qdma_private.mode = txMode ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_RX_DMA_MODE(unsigned int mainType, QDMA_Mode_t rxMode){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_RX_DMA_MODE;
	in_data.qdma_private.mode = rxMode ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_LOOPBACK_MODE(unsigned int mainType, QDMA_LoopbackMode_t lbMode){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_LOOPBACK_MODE;
	in_data.qdma_private.lbMode = lbMode ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_REGISTER_HOOKFUNC(unsigned int mainType, QDMA_CbType_t type, void *pCbFun){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_REGISTER;
	in_data.qdma_private.qdma_reg_cb.type = type ;
	in_data.qdma_private.qdma_reg_cb.pCbFun = pCbFun ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_UNREGISTER_HOOKFUNC(unsigned int mainType, QDMA_CbType_t type){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_UNREGISTER;
	in_data.qdma_private.qdma_reg_cb.type = type ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_ENABLE_RXPKT_INT(unsigned int mainType){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_ENABLE_RXPKT_INT;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_DISABLE_RXPKT_INT(unsigned int mainType){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_DISABLE_RXPKT_INT;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_RECEIVE_PACKETS(unsigned int mainType, int maxPkts){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_RECEIVE_PACKETS;
	in_data.qdma_private.cnt = maxPkts ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_TRANSMIT_PACKETS(unsigned int mainType, struct sk_buff *skb, uint msg0, uint msg1, struct port_info *pMacInfo){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_TRANSMIT_PACKETS;
	in_data.qdma_private.qdma_transmit.skb = skb ;
	in_data.qdma_private.qdma_transmit.msg0 = msg0 ;
	in_data.qdma_private.qdma_transmit.msg1 = msg1 ;
	in_data.qdma_private.qdma_transmit.pMacInfo = pMacInfo ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TX_QOS_WEIGHT(unsigned int mainType, QDMA_TxQosWeightType_t weightBase, QDMA_TxQosWeightScale_t weightScale){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TX_QOS_WEIGHT;
	in_data.qdma_private.qdma_tx_qos.weightBase = weightBase ;
	in_data.qdma_private.qdma_tx_qos.weightScale = weightScale ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TX_QOS_WEIGHT(unsigned int mainType, QDMA_TxQosWeightType_t *pWeightBase, QDMA_TxQosWeightScale_t *pWeightScale){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TX_QOS_WEIGHT;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	*pWeightBase = in_data.qdma_private.qdma_tx_qos.weightBase ;
	*pWeightScale = in_data.qdma_private.qdma_tx_qos.weightScale ;
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TX_QOS(unsigned int mainType, QDMA_TxQosScheduler_T *pTxQos){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TX_QOS;
	in_data.qdma_private.qdma_tx_qos.pTxQos = pTxQos ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TX_QOS(unsigned int mainType, QDMA_TxQosScheduler_T *pTxQos){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TX_QOS;
	in_data.qdma_private.qdma_tx_qos.pTxQos = pTxQos ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_MAC_LIMIT_THRESHOLD(unsigned int mainType, uint chnlThrh, uint queueThrh){
	struct ECNT_QDMA_Data in_data;
	QDMA_MacLimitThrh_T mac_limit_thrh;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_MAC_LIMIT_THRESHOLD;
	mac_limit_thrh.chnlThrh = chnlThrh ;
	mac_limit_thrh.queueThrh = queueThrh ;
	in_data.qdma_private.qdma_mac_limit_thrh = &mac_limit_thrh ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TXBUF_THRESHOLD(unsigned int mainType, QDMA_TxBufCtrl_T *pTxBufCtrl){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TXBUF_THRESHOLD;
	in_data.qdma_private.pQdmaTxBufCtrl = pTxBufCtrl ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TXBUF_THRESHOLD(unsigned int mainType, QDMA_TxBufCtrl_T *pTxBufCtrl){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TXBUF_THRESHOLD;
	in_data.qdma_private.pQdmaTxBufCtrl = pTxBufCtrl ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_PREFETCH_MODE(unsigned int mainType, QDMA_Mode_t prefecthMode, QDMA_Mode_t overDragMode, uint overDragCnt){
	struct ECNT_QDMA_Data in_data;
	QDMA_PrefetchMode_T qdma_prefetch;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_PREFETCH_MODE;
	qdma_prefetch.prefecthMode = prefecthMode ;
	qdma_prefetch.overDragMode = overDragMode ;
	qdma_prefetch.overDragCnt = overDragCnt ;	
	in_data.qdma_private.qdma_prefetch = &qdma_prefetch ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_PKTSIZE_OVERHEAD_EN(unsigned int mainType, QDMA_Mode_t mode){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_PKTSIZE_OVERHEAD_EN;
	in_data.qdma_private.mode = mode ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline QDMA_Mode_t QDMA_API_GET_PKTSIZE_OVERHEAD_EN(unsigned int mainType){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_PKTSIZE_OVERHEAD_EN;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_PKTSIZE_OVERHEAD_VALUE(unsigned int mainType, int cnt){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_PKTSIZE_OVERHEAD_VALUE;
	in_data.qdma_private.cnt = cnt ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline uint QDMA_API_GET_PKTSIZE_OVERHEAD_VALUE(unsigned int mainType){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_PKTSIZE_OVERHEAD_VALUE;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_LMGR_LOW_THRESHOLD(unsigned int mainType, uint lowThrh){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_LMGR_LOW_THRESHOLD;
	in_data.qdma_private.threshold = lowThrh ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline uint QDMA_API_GET_LMGR_LOW_THRESHOLD(unsigned int mainType){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_LMGR_LOW_THRESHOLD;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TX_DROP_EN(unsigned int mainType, QDMA_Mode_t mode){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TX_DROP_EN;
	in_data.qdma_private.mode = mode ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TX_DROP_EN(unsigned int mainType){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TX_DROP_EN;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TX_RATEMETER(unsigned int mainType, QDMA_TxRateMeter_T *txRateMeterPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TX_RATEMETER;
	in_data.qdma_private.txRateMeterPtr = txRateMeterPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TX_RATEMETER(unsigned int mainType, QDMA_TxRateMeter_T *txRateMeterPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TX_RATEMETER;
	in_data.qdma_private.txRateMeterPtr = txRateMeterPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_ENABLE_TX_RATELIMIT(unsigned int mainType){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_ENABLE_TX_RATELIMIT;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

#if 0
static inline int QDMA_API_SET_TX_RATELIMIT_CFG(unsigned int mainType, QDMA_TxRateLimitCfg_T *txRateLimitCfgPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TX_RATELIMIT_CFG;
	in_data.qdma_private.txRateLimitCfgPtr = txRateLimitCfgPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TX_RATELIMIT_CFG(unsigned int mainType, QDMA_TxRateLimitCfg_T *txRateLimitCfgPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TX_RATELIMIT_CFG;
	in_data.qdma_private.txRateLimitCfgPtr = txRateLimitCfgPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}
#endif

static inline int QDMA_API_SET_TX_RATELIMIT(unsigned int mainType, QDMA_TxRateLimitSet_T *txRateLimitSetPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TX_RATELIMIT;
	in_data.qdma_private.txRateLimitSetPtr = txRateLimitSetPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TX_RATELIMIT(unsigned int mainType, QDMA_TxRateLimitGet_T *txRateLimitGetPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TX_RATELIMIT;
	in_data.qdma_private.txRateLimitGetPtr = txRateLimitGetPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TX_DBA_REPORT(unsigned int mainType, QDMA_TxDbaReport_T *txDbaReportPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TX_DBAREPORT;
	in_data.qdma_private.txDbaReportPtr = txDbaReportPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TX_DBA_REPORT(unsigned int mainType, QDMA_TxDbaReport_T *txDbaReportPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TX_DBAREPORT;
	in_data.qdma_private.txDbaReportPtr = txDbaReportPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_RX_PROTECT_EN(unsigned int mainType, QDMA_Mode_t mode){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_RX_PROTECT_EN;
	in_data.qdma_private.mode = mode ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_RX_PROTECT_EN(unsigned int mainType){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_RX_PROTECT_EN;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_RX_LOW_THRESHOLD(unsigned int mainType, QDMA_RxLowThreshold_T *rxLowThrhPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_RX_LOW_THRESHOLD;
	in_data.qdma_private.rxLowThresholdPtr = rxLowThrhPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_RX_LOW_THRESHOLD(unsigned int mainType, QDMA_RxLowThreshold_T *rxLowThrhPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_RX_LOW_THRESHOLD;
	in_data.qdma_private.rxLowThresholdPtr = rxLowThrhPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_RX_RATELIMIT_EN(unsigned int mainType, QDMA_Mode_t mode){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_RX_RATELIMIT_EN;
	in_data.qdma_private.mode = mode ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_RX_RATELIMIT_PKT_MODE(unsigned int mainType, QDMA_RxPktMode_t pktMode )
{
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_RX_RATELIMIT_PKT_MODE;
	in_data.qdma_private.pktMode = pktMode ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_RX_RATELIMIT_CFG(unsigned int mainType, QDMA_RxRateLimitCfg_T *rxRateLimitCfgPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_RX_RATELIMIT_CFG;
	in_data.qdma_private.rxRateLimitCfgPtr = rxRateLimitCfgPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_RX_RATELIMIT(unsigned int mainType, QDMA_RxRateLimitSet_T *rxRateLimitSetPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_RX_RATELIMIT;
	in_data.qdma_private.rxRateLimitSetPtr = rxRateLimitSetPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_RX_RATELIMIT(unsigned int mainType, QDMA_RxRateLimitGet_T *rxRateLimitGetPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_RX_RATELIMIT;
	in_data.qdma_private.rxRateLimitGetPtr = rxRateLimitGetPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TXQ_DEI_DROP_MODE(unsigned int mainType, QDMA_Mode_t deiDropMode){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TXQ_DEI_DROP_MODE;
	in_data.qdma_private.mode = deiDropMode ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline QDMA_Mode_t QDMA_API_GET_TXQ_DEI_DROP_MODE(unsigned int mainType){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TXQ_DEI_DROP_MODE;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TXQ_CNGST_MODE(unsigned int mainType, QDMA_Mode_t dynCngstEn){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TXQ_CNGST_MODE;
	in_data.qdma_private.mode = dynCngstEn ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline QDMA_Mode_t QDMA_API_GET_TXQ_CNGST_MODE(unsigned int mainType){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_MODE;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TXQ_DEI_THRESHOLD_SCALE(unsigned int mainType, QDMA_TxDynCngstDeiThrhScale_t deiThrhScale){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TXQ_DEI_THRH_SCALE;
	in_data.qdma_private.threshold = deiThrhScale ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline QDMA_TxDynCngstDeiThrhScale_t QDMA_API_GET_TXQ_DEI_THRESHOLD_SCALE(unsigned int mainType){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TXQ_DEI_THRH_SCALE;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TXQ_CNGST_AUTO_CONFIG(unsigned int mainType, QDMA_txCngstCfg_t *pTxCngstCfg){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TXQ_CNGST_AUTO_CONFIG;
	in_data.qdma_private.pTxCngstCfg = pTxCngstCfg ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TXQ_CNGST_AUTO_CONFIG(unsigned int mainType, QDMA_txCngstCfg_t *pTxCngstCfg){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_AUTO_CONFIG;
	in_data.qdma_private.pTxCngstCfg = pTxCngstCfg ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TXQ_CNGST_DYNAMIC_THRESHOLD(unsigned int mainType, QDMA_TxQDynCngstThrh_T *txqCngstThrhPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TXQ_CNGST_DYNAMIC_THRESHOLD;
	in_data.qdma_private.dynCngstThrhPtr = txqCngstThrhPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TXQ_CNGST_DYNAMIC_THRESHOLD(unsigned int mainType, QDMA_TxQDynCngstThrh_T *txqCngstThrhPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_DYNAMIC_THRESHOLD;
	in_data.qdma_private.dynCngstThrhPtr = txqCngstThrhPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TXQ_CNGST_TOTAL_THRESHOLD(unsigned int mainType, QDMA_TxQDynCngstTotalThrh_T *totalThrhPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TXQ_CNGST_TOTAL_THRESHOLD;
	in_data.qdma_private.totalThrhPtr = totalThrhPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TXQ_CNGST_TOTAL_THRESHOLD(unsigned int mainType, QDMA_TxQDynCngstTotalThrh_T *totalThrhPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_TOTAL_THRESHOLD;
	in_data.qdma_private.totalThrhPtr = totalThrhPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TXQ_CNGST_CHANNEL_THRESHOLD(unsigned int mainType, QDMA_TxQDynCngstChnlThrh_T *chnlThrhPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TXQ_CNGST_CHANNEL_THRESHOLD;
	in_data.qdma_private.chnlThrhPtr = chnlThrhPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TXQ_CNGST_CHANNEL_THRESHOLD(unsigned int mainType, QDMA_TxQDynCngstChnlThrh_T *chnlThrhPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_CHANNEL_THRESHOLD;
	in_data.qdma_private.chnlThrhPtr = chnlThrhPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TXQ_CNGST_QUEUE_THRESHOLD(unsigned int mainType, QDMA_TxQDynCngstQueueThrh_T *queueThrhPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TXQ_CNGST_QUEUE_THRESHOLD;
	in_data.qdma_private.queueThrhPtr = queueThrhPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TXQ_CNGST_QUEUE_THRESHOLD(unsigned int mainType, QDMA_TxQDynCngstQueueThrh_T *queueThrhPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_QUEUE_THRESHOLD;
	in_data.qdma_private.queueThrhPtr = queueThrhPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TXQ_CNGST_QUEUE_NONBLOCKING(unsigned int mainType, QDMA_TxQCngstQueueCfg_T *txqCngstQueueCfgPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TXQ_CNGST_QUEUE_NONBLOCKING;
	in_data.qdma_private.txqCngstQueueCfgPtr = txqCngstQueueCfgPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TXQ_CNGST_QUEUE_NONBLOCKING(unsigned int mainType, QDMA_TxQCngstQueueCfg_T *txqCngstQueueCfgPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_QUEUE_NONBLOCKING;
	in_data.qdma_private.txqCngstQueueCfgPtr = txqCngstQueueCfgPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TXQ_CNGST_CHANNEL_NONBLOCKING(unsigned int mainType, QDMA_TxQCngstChannelCfg_T *txqCngstChannelCfgPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TXQ_CNGST_CHANNEL_NONBLOCKING;
	in_data.qdma_private.txqCngstChannelCfgPtr = txqCngstChannelCfgPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TXQ_CNGST_CHANNEL_NONBLOCKING(unsigned int mainType, QDMA_TxQCngstChannelCfg_T *txqCngstChannelCfgPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_CHANNEL_NONBLOCKING;
	in_data.qdma_private.txqCngstChannelCfgPtr = txqCngstChannelCfgPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}


static inline int QDMA_API_SET_TXQ_PEEKRATE_PARAMS(unsigned int mainType, QDMA_PeekRateCfg_t *peekrateCfgPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_PEEKRATE_PARAMS;
	in_data.qdma_private.peekrateCfgPtr = peekrateCfgPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TXQ_PEEKRATE_PARAMS(unsigned int mainType, QDMA_PeekRateCfg_t *peekrateCfgPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_PEEKRATE_PARAMS;
	in_data.qdma_private.peekrateCfgPtr = peekrateCfgPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TXQ_STATIC_QUEUE_NORMAL_THRESHOLD(unsigned int mainType, QDMA_TxQStaticNormalCfg_T *normThrhPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TXQ_STATIC_QUEUE_NORMAL_THRESHOLD;
	in_data.qdma_private.normThrhPtr = normThrhPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_TXQ_STATIC_QUEUE_DEI_THRESHOLD(unsigned int mainType, QDMA_TxQStaticDeiCfg_T *deiThrhPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_TXQ_STATIC_QUEUE_DEI_THRESHOLD;
	in_data.qdma_private.deiThrhPtr = deiThrhPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TXQ_CNGST_DYNAMIC_INFO(unsigned int mainType, QDMA_TxQDynamicCngstInfo_T *allThrhPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_DYNAMIC_INFO;
	in_data.qdma_private.dynCfgPtr = allThrhPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_TXQ_CNGST_STATIC_INFO(unsigned int mainType, QDMA_TxQStaticCngstInfo_T *staticInfoPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_STATIC_INFO;
	in_data.qdma_private.staticCfgPtr = staticInfoPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_VIRTUAL_CHANNEL_MODE(unsigned int mainType, QDMA_VirtualChannelMode_T *virChnlModePtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_VIRTUAL_CHANNEL_MODE;
	in_data.qdma_private.virChnlModePtr = virChnlModePtr;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_VIRTUAL_CHANNEL_MODE(unsigned int mainType, QDMA_VirtualChannelMode_T *virChnlModePtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_MODE;
	in_data.qdma_private.virChnlModePtr = virChnlModePtr;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_SET_VIRTUAL_CHANNEL_QOS(unsigned int mainType, QDMA_VirtualChannelQoS_T *virChnlQoSPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_SET_VIRTUAL_CHANNEL_QOS;
	in_data.qdma_private.virChnlQoSPtr = virChnlQoSPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_GET_VIRTUAL_CHANNEL_QOS(unsigned int mainType, QDMA_VirtualChannelQoS_T *virChnlQoSPtr){
	struct ECNT_QDMA_Data in_data;
	int ret=0;

	in_data.function_id = QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_QOS;
	in_data.qdma_private.virChnlQoSPtr = virChnlQoSPtr ;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

static inline int QDMA_API_READ_VIP_INFO(unsigned int mainType){
	struct ECNT_QDMA_Data in_data;
	int ret=0;
	in_data.function_id = QDMA_FUNCTION_READ_VIP_INFO;
	ret = __ECNT_HOOK(mainType, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
	if(ret != ECNT_HOOK_ERROR)
		return in_data.retValue;
	else
		return ECNT_HOOK_ERROR;
}

/************************************************************************
*                  D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N    D E C L A R A T I O N S
#                  I N L I N E    F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

#endif /* _ECNT_HOOK_QDMA_H_ */

