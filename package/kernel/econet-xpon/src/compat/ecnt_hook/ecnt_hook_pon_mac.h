/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°EcoNet Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (¡°EcoNet¡±) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°ECONET SOFTWARE¡±) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

#ifndef __LINUX_ENCT_HOOK_PON_MAC_H
#define __LINUX_ENCT_HOOK_PON_MAC_H

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/

#include <ecnt_hook/ecnt_hook.h>


/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/


/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/

typedef enum Event_Src_Module_e{
    XPON_PHY_MODULE,
	XPON_SN_SET,
}Event_Src_Module_t;

typedef enum PHY_Event_Source_e{
    PON_PHY_EVENT_SOURCE_HW_IRQ  ,  /* event comes from hard irq*/
    PON_PHY_EVENT_SOURCE_SW_POLL ,  /* event comes from sw irq polling */
}PHY_Event_Source_t;

typedef enum PHY_Event_Type_e {
    /* 
        phy interrupt event 
    */
    PHY_EVENT_TRANS_LOS_INT = 0x00  ,
    PHY_EVENT_PHY_LOF_INT           ,
    PHY_EVENT_TF_INT                ,
    PHY_EVENT_TRANS_INT             ,
    PHY_EVENT_TRANS_SD_FAIL_INT     ,
    PHY_EVENT_PHYRDY_INT            , 
    PHY_EVENT_PHY_ILLG_INT          , 
    PHY_EVENT_I2CM_INT              , 
    PHY_EVENT_TRANS_LOS_ILLG_INT    , /* LOS and Illegal INT happen simultaneously */

    /* all phy interrupt event id should be less than this  */ 
    PHY_EVENT_MAX_INT =     0x100   , 

    /* 
        phy non-interrupt event 
    */
    PHY_EVENT_START_ROGUE_MODE      ,
    PHY_EVENT_STOP_ROGUE_MODE       ,

    PHY_EVENT_CALIBRATION_START     ,
    PHY_EVENT_CALIBRATION_STOP      ,
    PHY_EVENT_TX_POWER_ON           ,
    PHY_EVENT_TX_POWER_OFF          ,
    
} PHY_Event_Type_t ;

typedef struct PON_PHY_Event_data_s{
    PHY_Event_Source_t src;
    PHY_Event_Type_t   id;
} PON_PHY_Event_data_t;

enum ECNT_XPON_MAC_SUBTYPE {
    ECNT_XPON_MAC_HOOK,
};

typedef struct xpon_mac_hook_data_s {
    Event_Src_Module_t     src_module;
    union {
        PON_PHY_Event_data_t   * pEvent;
        unsigned char sn[8];
    };
}xpon_mac_hook_data_t;

/************************************************************************
*               M A C R O S
*************************************************************************
*/

/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/
static inline void ECNT_API_XPON_SN_SET(unsigned char sn[8])
{
    struct xpon_mac_hook_data_s data = {0} ;
    data.src_module  = XPON_SN_SET;
    memcpy(data.sn, sn, 8);
            
    if(ECNT_HOOK_ERROR == __ECNT_HOOK(ECNT_XPON_MAC, ECNT_XPON_MAC_HOOK, (struct ecnt_data * )&data) ){
        panic("ECNT_HOOK_ERROR occur. %s:%d\n", __FUNCTION__, __LINE__);
    }
}


#endif // __LINUX_ENCT_HOOK_PON_MAC_H

