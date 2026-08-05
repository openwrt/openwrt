#ifndef _XPON_GLOBAL_PRIVATE_XMCS_PHY_H_
#define _XPON_GLOBAL_PRIVATE_XMCS_PHY_H_

#include "../public/xmcs_phy.h"
#include "xmcs_const.h"

/* 
    enum definition for PHY command 
*/
typedef enum {
	XMCS_PHY_BURST_MODE = 0,
	XMCS_PHY_CONTINUOUS_MODE
} XMCSPHY_TxBurstMode_t ;


/* 
    struct definition for PHY command 
*/
struct XMCS_PhyRxFecConfig_S {
	XPON_Mode_t		fecMode ;
	XPON_Mode_t		fecStatus ;
	XPON_Mode_t		fecCntReset ;
	struct {
		__u32		corrBytes ;
		__u32 		corrCodeWords ;
		__u32		unCorrCodeWords ;
		__u32		totalRxCodeWords ;
		__u32		fecSeconds ;
	} fecCounter ;
} ;

struct XMCS_PhyFrameCount_S {
	XPON_Mode_t		frameCntReset ;
	struct {
		__u32		low ;
		__u32		high ;
		__u32		lof ;
	} frameCounter ;
} ;

struct XMCS_PhyTransSetting_S {
	XPON_Mode_t		txSdInverse ;
	XPON_Mode_t		txFaultInverse ;
	XPON_Mode_t		txBurstEnInverse ;
	XPON_Mode_t		rxSdInverse ;
} ;

struct XMCS_PhyTxBurstCfg_S {
	XMCSPHY_TxBurstMode_t	burstMode ;
} ;
		
struct XMCS_PHY_I2cCtrl_S
{
	__u8  u1CHannelID;
	__u16 u2ClkDiv;
	__u8  u1DevAddr;
	__u8  u1WordAddrNum;
	__u32 u4WordAddr;
	__u8  * pu1Buf;
	__u16 u2ByteCnt;
};


/* 
    PHY IOCTL commands  
*/

#define PHY_MAGIC						0xD7

/* PHY IO Command */
#define PHY_IOS_FEC_MODE				_IOW(PHY_MAGIC, 0x01, struct XMCS_PhyRxFecConfig_S *)
#define PHY_IOG_FEC_STATUS				_IOR(PHY_MAGIC, 0x01, struct XMCS_PhyRxFecConfig_S *)
#define PHY_IOS_FEC_CLEAR				_IOW(PHY_MAGIC, 0x02, struct XMCS_PhyRxFecConfig_S *)
#define PHY_IOG_FEC_COUNTER				_IOR(PHY_MAGIC, 0x02, struct XMCS_PhyRxFecConfig_S *)
#define PHY_IOS_FRAME_CLEAR				_IOW(PHY_MAGIC, 0x03, struct XMCS_PhyFrameCount_S *)
#define PHY_IOG_FRAME_COUNTER			_IOR(PHY_MAGIC, 0x03, struct XMCS_PhyFrameCount_S *)
#define PHY_IOS_TRANSCEIVER_CONFIG		_IOW(PHY_MAGIC, 0x04, struct XMCS_PhyTransSetting_S *)
#define PHY_IOG_TRANSCEIVER_CONFIG		_IOR(PHY_MAGIC, 0x04, struct XMCS_PhyTransSetting_S *)
#define PHY_IOG_TRANSCEIVER_PARAMS		_IOR(PHY_MAGIC, 0x05, struct XMCS_PhyTransParams_S *)
#define PHY_IOS_TX_BURST_CONFIG			_IOW(PHY_MAGIC, 0x06, struct XMCS_PhyTxBurstCfg_S *)
#define PHY_IOG_TX_BURST_CONFIG			_IOR(PHY_MAGIC, 0x06, struct XMCS_PhyTxBurstCfg_S *)
#define PHY_IOG_TX_RX_FEC_STATUS		_IOR(PHY_MAGIC, 0x07, struct XMCS_PhyTxRxFecStatus_S *)
#define PHY_IOS_ROUGE_MODE				_IOW(PHY_MAGIC, 0x08, __u8)
#define PHY_IOS_I2C_CTRL_WRITE			_IOW(PHY_MAGIC, 0x09, struct XMCS_PHY_I2cCtrl_S *)
#define PHY_IOG_I2C_CTRL_READ			_IOR(PHY_MAGIC, 0x09, struct XMCS_PHY_I2cCtrl_S *)


#endif /* _XPON_GLOBAL_PRIVATE_XMCS_PHY_H_  */

