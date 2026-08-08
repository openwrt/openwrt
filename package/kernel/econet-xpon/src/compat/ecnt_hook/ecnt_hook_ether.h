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
#ifndef _ECNT_HOOK_ETHER_H
#define _ECNT_HOOK_ETHER_H

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/net.h>
#include <linux/if.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/compiler.h>
#include <ecnt_hook/ecnt_hook.h>
#include <modules/eth_global_def.h>

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define ECNT_DRIVER_API  0

/************************************************************************
*               M A C R O S
*************************************************************************
*/


/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/


/* Warning: same sequence with function array 'qdma_operation' */
typedef enum { 
    ETHER_FUNCTION_MDIO_READ,
    ETHER_FUNCTION_MDIO_WRITE,
    ETHER_FUNCTION_MAC_SNED,
    ETHER_FUNCTION_GSW_PBUS_READ,
    ETHER_FUNCTION_GSW_PBUS_WRITE,
    ETHER_FUNCTION_EXT_GSW_PBUS_READ,
    ETHER_FUNCTION_EXT_GSW_PBUS_WRITE,
    ETHER_FUNCTION_SET_RATELIMIT_SWITCH,
    ETHER_FUNCTION_SET_MACTABLE_SYNC_EN,
    ETHER_FUNCTION_RGMII_SETTING,
    ETHER_FUNCTION_RGMII_MODE,
    ETHER_FUNCTION_MAX_NUM,
} ETHER_HookFunction_t ;


typedef struct ECNT_ETHER_Data {
	ETHER_HookFunction_t function_id;	/* need put at first item */
	int retValue;
	
	union {
	    struct {
	        uint add;
	        uint reg;
	        uint data;
	    } phy;
	    struct sk_buff *skb;
	    struct 
	    {
	        u8 wan_type;
	        u8 interface;
	        u8 mode;
	    }traffic_setting;
	    unsigned char ratelimit_En ;
 	    unsigned char enable ;
	} ether_private;
}ECNT_ETHER_Data_s;


/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/
static inline uint32 ETHER_MDIO_READ(uint32 phy_add, uint32 phy_reg){
    ECNT_ETHER_Data_s in_data;
    int ret=0;

    in_data.function_id = ETHER_FUNCTION_MDIO_READ;
    in_data.ether_private.phy.add = phy_add;
    in_data.ether_private.phy.reg = phy_reg;
    ret = __ECNT_HOOK(ECNT_ETHER_SWITCH, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
    if(ret != ECNT_HOOK_ERROR)
        return in_data.ether_private.phy.data;
    else
        return ECNT_HOOK_ERROR;
}

static inline int ETHER_MDIO_WRITE(uint32 phy_add, uint32 phy_reg, uint32 phy_data){
    ECNT_ETHER_Data_s in_data;
    int ret=0;

    in_data.function_id = ETHER_FUNCTION_MDIO_WRITE;
    in_data.ether_private.phy.add = phy_add;
    in_data.ether_private.phy.reg = phy_reg;
    in_data.ether_private.phy.data = phy_data;   
    ret = __ECNT_HOOK(ECNT_ETHER_SWITCH, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
    if(ret != ECNT_HOOK_ERROR)
        return in_data.retValue;
    else
        return ECNT_HOOK_ERROR;
}

static inline int ETHER_MAC_SEND(struct sk_buff *skb){
    
    ECNT_ETHER_Data_s in_data;
    int ret=0;
    in_data.function_id = ETHER_FUNCTION_MAC_SNED;
    in_data.ether_private.skb = skb;
    
    ret = __ECNT_HOOK(ECNT_ETHER_SWITCH, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
    if(ret != ECNT_HOOK_ERROR)
        return in_data.retValue;
    else
        return ECNT_HOOK_ERROR;
}

static inline uint32 ETHER_API_GSW_PBUS_READ(uint32 pbus_addr){
    ECNT_ETHER_Data_s in_data;
    int ret=0;

    in_data.function_id = ETHER_FUNCTION_GSW_PBUS_READ;
    in_data.ether_private.phy.add = pbus_addr;

    ret = __ECNT_HOOK(ECNT_ETHER_SWITCH, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
    if(ret != ECNT_HOOK_ERROR)
        return in_data.ether_private.phy.data;
    else
        return ECNT_HOOK_ERROR;
}

static inline uint32 ETHER_API_GSW_PBUS_WRITE(uint32 pbus_addr, uint32 pbus_data){
    ECNT_ETHER_Data_s in_data;
    int ret=0;

    in_data.function_id = ETHER_FUNCTION_GSW_PBUS_WRITE;
    in_data.ether_private.phy.add = pbus_addr;
    in_data.ether_private.phy.data = pbus_data;

    ret = __ECNT_HOOK(ECNT_ETHER_SWITCH, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
    if(ret != ECNT_HOOK_ERROR)
        return in_data.retValue;
    else
        return ECNT_HOOK_ERROR;
}

static inline uint32 ETHER_API_EXT_GSW_PBUS_READ(uint32 pbus_addr){
    ECNT_ETHER_Data_s in_data;
    int ret=0;

    in_data.function_id = ETHER_FUNCTION_EXT_GSW_PBUS_READ;
    in_data.ether_private.phy.add = pbus_addr;

    ret = __ECNT_HOOK(ECNT_ETHER_SWITCH, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
    if(ret != ECNT_HOOK_ERROR)
        return in_data.ether_private.phy.data;
    else
        return ECNT_HOOK_ERROR;
}

static inline uint32 ETHER_API_EXT_GSW_PBUS_WRITE(uint32 pbus_addr, uint32 pbus_data){
    ECNT_ETHER_Data_s in_data;
    int ret=0;

    in_data.function_id = ETHER_FUNCTION_EXT_GSW_PBUS_WRITE;
    in_data.ether_private.phy.add = pbus_addr;
    in_data.ether_private.phy.data = pbus_data;

    ret = __ECNT_HOOK(ECNT_ETHER_SWITCH, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
    if(ret != ECNT_HOOK_ERROR)
        return in_data.retValue;
    else
        return ECNT_HOOK_ERROR;
}

static inline int ETHER_SET_RATELIMIT_SWITCH(unsigned char enable){
    
    ECNT_ETHER_Data_s in_data;
    int ret=0;
    in_data.function_id = ETHER_FUNCTION_SET_RATELIMIT_SWITCH ;
    in_data.ether_private.ratelimit_En = enable ;
    
    ret = __ECNT_HOOK(ECNT_ETHER_SWITCH, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
    if(ret != ECNT_HOOK_ERROR)
        return in_data.retValue;
    else
        return ECNT_HOOK_ERROR;
}

static inline int ETHER_SET_MACTABLE_SYNC_EN(unsigned char enable){
    
    ECNT_ETHER_Data_s in_data;
    int ret=0;
    in_data.function_id = ETHER_FUNCTION_SET_MACTABLE_SYNC_EN ;
    in_data.ether_private.ratelimit_En = enable ;
    
    ret = __ECNT_HOOK(ECNT_ETHER_SWITCH, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
    if(ret != ECNT_HOOK_ERROR)
        return in_data.retValue;
    else
        return ECNT_HOOK_ERROR;
}

static inline uint32 ETHER_API_TRGMII_SETTING(uint8 wan_type, uint8 interface)
{
    ECNT_ETHER_Data_s in_data;
    int ret=0;

    in_data.function_id = ETHER_FUNCTION_RGMII_SETTING;
    in_data.ether_private.traffic_setting.wan_type = wan_type;
    in_data.ether_private.traffic_setting.interface = interface;

    ret = __ECNT_HOOK(ECNT_ETHER_SWITCH, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
    if(ret != ECNT_HOOK_ERROR)
        return in_data.retValue;
    else
        return ECNT_HOOK_ERROR;
}

static inline uint32 ETHER_API_RGMII_MODE(uint8 mode)
{
    ECNT_ETHER_Data_s in_data;
    int ret=0;

    in_data.function_id = ETHER_FUNCTION_RGMII_MODE;
    in_data.ether_private.traffic_setting.interface = RGMII;
    in_data.ether_private.traffic_setting.mode = mode;

    ret = __ECNT_HOOK(ECNT_ETHER_SWITCH, ECNT_DRIVER_API, (struct ecnt_data *)&in_data);
    if(ret != ECNT_HOOK_ERROR)
        return in_data.retValue;
    else
        return ECNT_HOOK_ERROR;
}
#endif /*_ETHER_HOOK_H*/
