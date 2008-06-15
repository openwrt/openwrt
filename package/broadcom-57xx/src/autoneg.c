/******************************************************************************/
/*                                                                            */
/* Broadcom BCM5700 Linux Network Driver, Copyright (c) 2000 - 2004 Broadcom  */
/* Corporation.                                                               */
/* All rights reserved.                                                       */
/*                                                                            */
/* This program is free software; you can redistribute it and/or modify       */
/* it under the terms of the GNU General Public License as published by       */
/* the Free Software Foundation, located in the file LICENSE.                 */
/*                                                                            */
/* History:                                                                   */
/******************************************************************************/

#ifdef INCLUDE_TBI_SUPPORT
#include "mm.h"



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
void
MM_AnTxConfig(
    PAN_STATE_INFO pAnInfo)
{
    PLM_DEVICE_BLOCK pDevice;

    pDevice = (PLM_DEVICE_BLOCK) pAnInfo->pContext;

    REG_WR(pDevice, MacCtrl.TxAutoNeg, (LM_UINT32) pAnInfo->TxConfig.AsUSHORT);

    pDevice->MacMode |= MAC_MODE_SEND_CONFIGS;
    REG_WR(pDevice, MacCtrl.Mode, pDevice->MacMode);
}



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
void
MM_AnTxIdle(
    PAN_STATE_INFO pAnInfo)
{
    PLM_DEVICE_BLOCK pDevice;

    pDevice = (PLM_DEVICE_BLOCK) pAnInfo->pContext;

    pDevice->MacMode &= ~MAC_MODE_SEND_CONFIGS;
    REG_WR(pDevice, MacCtrl.Mode, pDevice->MacMode);
}



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
char
MM_AnRxConfig(
    PAN_STATE_INFO pAnInfo,
    unsigned short *pRxConfig)
{
    PLM_DEVICE_BLOCK pDevice;
    LM_UINT32 Value32;
    char Retcode;

    Retcode = AN_FALSE;

    pDevice = (PLM_DEVICE_BLOCK) pAnInfo->pContext;

    Value32 = REG_RD(pDevice, MacCtrl.Status);
    if(Value32 & MAC_STATUS_RECEIVING_CFG)
    {
        Value32 = REG_RD(pDevice, MacCtrl.RxAutoNeg);
        *pRxConfig = (unsigned short) Value32;

        Retcode = AN_TRUE;
    }

    return Retcode;
}



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
void
AutonegInit(
    PAN_STATE_INFO pAnInfo)
{
    unsigned long j;

    for(j = 0; j < sizeof(AN_STATE_INFO); j++)
    {
        ((unsigned char *) pAnInfo)[j] = 0;
    }

    /* Initialize the default advertisement register. */
    pAnInfo->mr_adv_full_duplex = 1;
    pAnInfo->mr_adv_sym_pause = 1;
    pAnInfo->mr_adv_asym_pause = 1;
    pAnInfo->mr_an_enable = 1;
}



/******************************************************************************/
/* Description:                                                               */
/*                                                                            */
/* Return:                                                                    */
/******************************************************************************/
AUTONEG_STATUS
Autoneg8023z(
    PAN_STATE_INFO pAnInfo)
{
    unsigned short RxConfig;
    unsigned long Delta_us;
    AUTONEG_STATUS AnRet;

    /* Get the current time. */
    if(pAnInfo->State == AN_STATE_UNKNOWN)
    {
        pAnInfo->RxConfig.AsUSHORT = 0;
        pAnInfo->CurrentTime_us = 0;
        pAnInfo->LinkTime_us = 0;
        pAnInfo->AbilityMatchCfg = 0;
        pAnInfo->AbilityMatchCnt = 0;
        pAnInfo->AbilityMatch = AN_FALSE;
        pAnInfo->IdleMatch = AN_FALSE;
        pAnInfo->AckMatch = AN_FALSE;
    }

    /* Increment the timer tick.  This function is called every microsecon. */
//    pAnInfo->CurrentTime_us++;

    /* Set the AbilityMatch, IdleMatch, and AckMatch flags if their */
    /* corresponding conditions are satisfied. */
    if(MM_AnRxConfig(pAnInfo, &RxConfig))
    {
        if(RxConfig != pAnInfo->AbilityMatchCfg)
        {
            pAnInfo->AbilityMatchCfg = RxConfig;
            pAnInfo->AbilityMatch = AN_FALSE;
            pAnInfo->AbilityMatchCnt = 0;
        }
        else
        {
            pAnInfo->AbilityMatchCnt++;
            if(pAnInfo->AbilityMatchCnt > 1)
            {
                pAnInfo->AbilityMatch = AN_TRUE;
                pAnInfo->AbilityMatchCfg = RxConfig;
            }
        }

        if(RxConfig & AN_CONFIG_ACK)
        {
            pAnInfo->AckMatch = AN_TRUE;
        }
        else
        {
            pAnInfo->AckMatch = AN_FALSE;
        }

        pAnInfo->IdleMatch = AN_FALSE;
    }
    else
    {
        pAnInfo->IdleMatch = AN_TRUE;

        pAnInfo->AbilityMatchCfg = 0;
        pAnInfo->AbilityMatchCnt = 0;
        pAnInfo->AbilityMatch = AN_FALSE;
        pAnInfo->AckMatch = AN_FALSE;

        RxConfig = 0;
    }

    /* Save the last Config. */
    pAnInfo->RxConfig.AsUSHORT = RxConfig;

    /* Default return code. */
    AnRet = AUTONEG_STATUS_OK;

    /* Autoneg state machine as defined in 802.3z section 37.3.1.5. */
    switch(pAnInfo->State)
    {
        case AN_STATE_UNKNOWN:
            if(pAnInfo->mr_an_enable || pAnInfo->mr_restart_an)
            {
                pAnInfo->CurrentTime_us = 0;
                pAnInfo->State = AN_STATE_AN_ENABLE;
            }

            /* Fall through.*/

        case AN_STATE_AN_ENABLE:
            pAnInfo->mr_an_complete = AN_FALSE;
            pAnInfo->mr_page_rx = AN_FALSE;

            if(pAnInfo->mr_an_enable)
            {
                pAnInfo->LinkTime_us = 0;
                pAnInfo->AbilityMatchCfg = 0;
                pAnInfo->AbilityMatchCnt = 0;
                pAnInfo->AbilityMatch = AN_FALSE;
                pAnInfo->IdleMatch = AN_FALSE;
                pAnInfo->AckMatch = AN_FALSE;

                pAnInfo->State = AN_STATE_AN_RESTART_INIT;
            }
            else
            {
                pAnInfo->State = AN_STATE_DISABLE_LINK_OK;
            }
            break;

        case AN_STATE_AN_RESTART_INIT:
            pAnInfo->LinkTime_us = pAnInfo->CurrentTime_us;
            pAnInfo->mr_np_loaded = AN_FALSE;

            pAnInfo->TxConfig.AsUSHORT = 0;
            MM_AnTxConfig(pAnInfo);

            AnRet = AUTONEG_STATUS_TIMER_ENABLED;

            pAnInfo->State = AN_STATE_AN_RESTART;

            /* Fall through.*/

        case AN_STATE_AN_RESTART:
            /* Get the current time and compute the delta with the saved */
            /* link timer. */
            Delta_us = pAnInfo->CurrentTime_us - pAnInfo->LinkTime_us;
            if(Delta_us > AN_LINK_TIMER_INTERVAL_US)
            {
                pAnInfo->State = AN_STATE_ABILITY_DETECT_INIT;
            }
            else
            {
                AnRet = AUTONEG_STATUS_TIMER_ENABLED;
            }
            break;

        case AN_STATE_DISABLE_LINK_OK:
            AnRet = AUTONEG_STATUS_DONE;
            break;

        case AN_STATE_ABILITY_DETECT_INIT:
            /* Note: in the state diagram, this variable is set to */
            /* mr_adv_ability<12>.  Is this right?. */
            pAnInfo->mr_toggle_tx = AN_FALSE;

            /* Send the config as advertised in the advertisement register. */
            pAnInfo->TxConfig.AsUSHORT = 0;
            pAnInfo->TxConfig.D5_FD = pAnInfo->mr_adv_full_duplex;
            pAnInfo->TxConfig.D6_HD = pAnInfo->mr_adv_half_duplex;
            pAnInfo->TxConfig.D7_PS1 = pAnInfo->mr_adv_sym_pause;
            pAnInfo->TxConfig.D8_PS2 = pAnInfo->mr_adv_asym_pause;
            pAnInfo->TxConfig.D12_RF1 = pAnInfo->mr_adv_remote_fault1;
            pAnInfo->TxConfig.D13_RF2 = pAnInfo->mr_adv_remote_fault2;
            pAnInfo->TxConfig.D15_NP = pAnInfo->mr_adv_next_page;

            MM_AnTxConfig(pAnInfo);

            pAnInfo->State = AN_STATE_ABILITY_DETECT;

            break;

        case AN_STATE_ABILITY_DETECT:
            if(pAnInfo->AbilityMatch == AN_TRUE &&
                pAnInfo->RxConfig.AsUSHORT != 0)
            {
                pAnInfo->State = AN_STATE_ACK_DETECT_INIT;
            }

            break;

        case AN_STATE_ACK_DETECT_INIT:
            pAnInfo->TxConfig.D14_ACK = 1;
            MM_AnTxConfig(pAnInfo);

            pAnInfo->State = AN_STATE_ACK_DETECT;

            /* Fall through. */

        case AN_STATE_ACK_DETECT:
            if(pAnInfo->AckMatch == AN_TRUE)
            {
                if((pAnInfo->RxConfig.AsUSHORT & ~AN_CONFIG_ACK) ==
                    (pAnInfo->AbilityMatchCfg & ~AN_CONFIG_ACK))
                {
                    pAnInfo->State = AN_STATE_COMPLETE_ACK_INIT;
                }
                else
                {
                    pAnInfo->State = AN_STATE_AN_ENABLE;
                }
            }
            else if(pAnInfo->AbilityMatch == AN_TRUE &&
                pAnInfo->RxConfig.AsUSHORT == 0)
            {
                pAnInfo->State = AN_STATE_AN_ENABLE;
            }

            break;

        case AN_STATE_COMPLETE_ACK_INIT:
            /* Make sure invalid bits are not set. */
            if(pAnInfo->RxConfig.bits.D0 || pAnInfo->RxConfig.bits.D1 ||
                pAnInfo->RxConfig.bits.D2 || pAnInfo->RxConfig.bits.D3 ||
                pAnInfo->RxConfig.bits.D4 || pAnInfo->RxConfig.bits.D9 ||
                pAnInfo->RxConfig.bits.D10 || pAnInfo->RxConfig.bits.D11)
            {
                AnRet = AUTONEG_STATUS_FAILED;
                break;
            }

            /* Set up the link partner advertisement register. */
            pAnInfo->mr_lp_adv_full_duplex = pAnInfo->RxConfig.D5_FD;
            pAnInfo->mr_lp_adv_half_duplex = pAnInfo->RxConfig.D6_HD;
            pAnInfo->mr_lp_adv_sym_pause = pAnInfo->RxConfig.D7_PS1;
            pAnInfo->mr_lp_adv_asym_pause = pAnInfo->RxConfig.D8_PS2;
            pAnInfo->mr_lp_adv_remote_fault1 = pAnInfo->RxConfig.D12_RF1;
            pAnInfo->mr_lp_adv_remote_fault2 = pAnInfo->RxConfig.D13_RF2;
            pAnInfo->mr_lp_adv_next_page = pAnInfo->RxConfig.D15_NP;

            pAnInfo->LinkTime_us = pAnInfo->CurrentTime_us;

            pAnInfo->mr_toggle_tx = !pAnInfo->mr_toggle_tx;
            pAnInfo->mr_toggle_rx = pAnInfo->RxConfig.bits.D11;
            pAnInfo->mr_np_rx = pAnInfo->RxConfig.D15_NP;
            pAnInfo->mr_page_rx = AN_TRUE;

            pAnInfo->State = AN_STATE_COMPLETE_ACK;
            AnRet = AUTONEG_STATUS_TIMER_ENABLED;

            break;

        case AN_STATE_COMPLETE_ACK:
            if(pAnInfo->AbilityMatch == AN_TRUE &&
                pAnInfo->RxConfig.AsUSHORT == 0)
            {
                pAnInfo->State = AN_STATE_AN_ENABLE;
                break;
            }

            Delta_us = pAnInfo->CurrentTime_us - pAnInfo->LinkTime_us;

            if(Delta_us > AN_LINK_TIMER_INTERVAL_US)
            {
                if(pAnInfo->mr_adv_next_page == 0 ||
                    pAnInfo->mr_lp_adv_next_page == 0)
                {
                    pAnInfo->State = AN_STATE_IDLE_DETECT_INIT;
                }
                else
                {
                    if(pAnInfo->TxConfig.bits.D15 == 0 &&
                        pAnInfo->mr_np_rx == 0)
                    {
                        pAnInfo->State = AN_STATE_IDLE_DETECT_INIT;
                    }
                    else
                    {
                        AnRet = AUTONEG_STATUS_FAILED;
                    }
                }
            }

            break;

        case AN_STATE_IDLE_DETECT_INIT:
            pAnInfo->LinkTime_us = pAnInfo->CurrentTime_us;

            MM_AnTxIdle(pAnInfo);

            pAnInfo->State = AN_STATE_IDLE_DETECT;

            AnRet = AUTONEG_STATUS_TIMER_ENABLED;

            break;

        case AN_STATE_IDLE_DETECT:
            if(pAnInfo->AbilityMatch == AN_TRUE &&
                pAnInfo->RxConfig.AsUSHORT == 0)
            {
                pAnInfo->State = AN_STATE_AN_ENABLE;
                break;
            }

            Delta_us = pAnInfo->CurrentTime_us - pAnInfo->LinkTime_us;
            if(Delta_us > AN_LINK_TIMER_INTERVAL_US)
            {
//                if(pAnInfo->IdleMatch == AN_TRUE)
//                {
                    pAnInfo->State = AN_STATE_LINK_OK;
//                }
//                else
//                {
//                    AnRet = AUTONEG_STATUS_FAILED;
//                    break;
//                }
            }

            break;

        case AN_STATE_LINK_OK:
            pAnInfo->mr_an_complete = AN_TRUE;
            pAnInfo->mr_link_ok = AN_TRUE;
            AnRet = AUTONEG_STATUS_DONE;

            break;

        case AN_STATE_NEXT_PAGE_WAIT_INIT:
            break;

        case AN_STATE_NEXT_PAGE_WAIT:
            break;

        default:
            AnRet = AUTONEG_STATUS_FAILED;
            break;
    }

    return AnRet;
}
#endif /* INCLUDE_TBI_SUPPORT */

