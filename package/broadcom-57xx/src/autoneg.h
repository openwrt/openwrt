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


#ifndef AUTONEG_H
#define AUTONEG_H



/******************************************************************************/
/* Constants. */
/******************************************************************************/

#define AN_LINK_TIMER_INTERVAL_US           12000       /* 10ms */

/* TRUE, FALSE */
#define AN_TRUE                             1
#define AN_FALSE                            0



/******************************************************************************/
/* Main data structure for keeping track of 802.3z auto-negotation state */
/* variables as shown in Figure 37-6 of the IEEE 802.3z specification. */
/******************************************************************************/

typedef struct
{
    /* Pointer to the operating system specific data structure. */
    void *pContext;

    /* Current auto-negotiation state. */
    unsigned long State;
    #define AN_STATE_UNKNOWN                        0
    #define AN_STATE_AN_ENABLE                      1
    #define AN_STATE_AN_RESTART_INIT                2
    #define AN_STATE_AN_RESTART                     3
    #define AN_STATE_DISABLE_LINK_OK                4
    #define AN_STATE_ABILITY_DETECT_INIT            5
    #define AN_STATE_ABILITY_DETECT                 6
    #define AN_STATE_ACK_DETECT_INIT                7
    #define AN_STATE_ACK_DETECT                     8
    #define AN_STATE_COMPLETE_ACK_INIT              9
    #define AN_STATE_COMPLETE_ACK                   10
    #define AN_STATE_IDLE_DETECT_INIT               11
    #define AN_STATE_IDLE_DETECT                    12
    #define AN_STATE_LINK_OK                        13
    #define AN_STATE_NEXT_PAGE_WAIT_INIT            14
    #define AN_STATE_NEXT_PAGE_WAIT                 16

    /* Link timer. */
    unsigned long LinkTime_us;

    /* Current time. */
    unsigned long CurrentTime_us;

    /* Ability, idle, and ack match functions. */
    unsigned long AbilityMatchCnt;

    /* Need these values for consistency check. */
    unsigned short AbilityMatchCfg;

    unsigned short reserved;

    char AbilityMatch;
    char IdleMatch;
    char AckMatch;
    char reserved1;

    /* Tx config data */
    union
    {
        /* The TxConfig register is arranged as follows:                      */
        /*                                                                    */
        /* MSB                                                           LSB  */
        /* +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+  */
        /* | D7| D6| D5| D4| D3| D2| D1| D0|D15|D14|D13|D12|D11|D10| D9| D8|  */
        /* +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+  */
        struct
        {
#ifdef BIG_ENDIAN_HOST
            unsigned short D7:1;        /* PS1 */
            unsigned short D6:1;        /* HD */
            unsigned short D5:1;        /* FD */
            unsigned short D4:1;
            unsigned short D3:1;
            unsigned short D2:1;
            unsigned short D1:1;
            unsigned short D0:1;
            unsigned short D15:1;       /* NP */
            unsigned short D14:1;       /* ACK */
            unsigned short D13:1;       /* RF2 */
            unsigned short D12:1;       /* RF1 */
            unsigned short D11:1;
            unsigned short D10:1;
            unsigned short D9:1;
            unsigned short D8:1;        /* PS2 */
#else /* BIG_ENDIAN_HOST */
            unsigned int D8:1;        /* PS2 */
            unsigned int D9:1;
            unsigned int D10:1;
            unsigned int D11:1;
            unsigned int D12:1;       /* RF1 */
            unsigned int D13:1;       /* RF2 */
            unsigned int D14:1;       /* ACK */
            unsigned int D15:1;       /* NP */
            unsigned int D0:1;
            unsigned int D1:1;
            unsigned int D2:1;
            unsigned int D3:1;
            unsigned int D4:1;
            unsigned int D5:1;        /* FD */
            unsigned int D6:1;        /* HD */
            unsigned int D7:1;        /* PS1 */
#endif
        } bits;

        unsigned short AsUSHORT;

        #define D8_PS2                      bits.D8
        #define D12_RF1                     bits.D12
        #define D13_RF2                     bits.D13
        #define D14_ACK                     bits.D14
        #define D15_NP                      bits.D15
        #define D5_FD                       bits.D5
        #define D6_HD                       bits.D6
        #define D7_PS1                      bits.D7
    } TxConfig;

    /* Rx config data */
    union
    {
        /* The RxConfig register is arranged as follows:                      */
        /*                                                                    */
        /* MSB                                                           LSB  */
        /* +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+  */
        /* | D7| D6| D5| D4| D3| D2| D1| D0|D15|D14|D13|D12|D11|D10| D9| D8|  */
        /* +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+  */
        struct
        {
#ifdef BIG_ENDIAN_HOST
	    unsigned short D7:1;        /* PS1 */
            unsigned short D6:1;        /* HD */
	    unsigned short D5:1;        /* FD */
            unsigned short D4:1;
            unsigned short D3:1;
            unsigned short D2:1;
            unsigned short D1:1;
            unsigned short D0:1;
  	    unsigned short D15:1;       /* NP */
            unsigned short D14:1;       /* ACK */
            unsigned short D13:1;       /* RF2 */    
            unsigned short D12:1;       /* RF1 */
            unsigned short D11:1;
            unsigned short D10:1;
            unsigned short D9:1;
            unsigned short D8:1;        /* PS2 */
#else /* BIG_ENDIAN_HOST */
            unsigned int D8:1;        /* PS2 */
            unsigned int D9:1;
            unsigned int D10:1;
            unsigned int D11:1;
            unsigned int D12:1;       /* RF1 */
            unsigned int D13:1;       /* RF2 */
            unsigned int D14:1;       /* ACK */
            unsigned int D15:1;       /* NP */
            unsigned int D0:1;
            unsigned int D1:1;
            unsigned int D2:1;
            unsigned int D3:1;
            unsigned int D4:1;
            unsigned int D5:1;        /* FD */
            unsigned int D6:1;        /* HD */
            unsigned int D7:1;        /* PS1 */
#endif
        } bits;

        unsigned short AsUSHORT;
    } RxConfig;

    #define AN_CONFIG_NP                            0x0080
    #define AN_CONFIG_ACK                           0x0040
    #define AN_CONFIG_RF2                           0x0020
    #define AN_CONFIG_RF1                           0x0010
    #define AN_CONFIG_PS2                           0x0001
    #define AN_CONFIG_PS1                           0x8000
    #define AN_CONFIG_HD                            0x4000
    #define AN_CONFIG_FD                            0x2000


    /* Management registers. */

    /* Control register. */
    union
    {
        struct
        {
            unsigned int an_enable:1;
            unsigned int loopback:1;
            unsigned int reset:1;
            unsigned int restart_an:1;
        } bits;

        unsigned short AsUSHORT;

        #define mr_an_enable                Mr0.bits.an_enable
        #define mr_loopback                 Mr0.bits.loopback
        #define mr_main_reset               Mr0.bits.reset
        #define mr_restart_an               Mr0.bits.restart_an
    } Mr0;

    /* Status register. */
    union
    {
        struct
        {
            unsigned int an_complete:1;
            unsigned int link_ok:1;
        } bits;

        unsigned short AsUSHORT;

        #define mr_an_complete              Mr1.bits.an_complete
        #define mr_link_ok                  Mr1.bits.link_ok
    } Mr1;

    /* Advertisement register. */
    union
    {
        struct
        {
            unsigned int reserved_4:5;
            unsigned int full_duplex:1;
            unsigned int half_duplex:1;
            unsigned int sym_pause:1;
            unsigned int asym_pause:1;
            unsigned int reserved_11:3;
            unsigned int remote_fault1:1;
            unsigned int remote_fault2:1;
            unsigned int reserved_14:1;
            unsigned int next_page:1;
        } bits;

        unsigned short AsUSHORT;

        #define mr_adv_full_duplex          Mr4.bits.full_duplex
        #define mr_adv_half_duplex          Mr4.bits.half_duplex
        #define mr_adv_sym_pause            Mr4.bits.sym_pause
        #define mr_adv_asym_pause           Mr4.bits.asym_pause
        #define mr_adv_remote_fault1        Mr4.bits.remote_fault1
        #define mr_adv_remote_fault2        Mr4.bits.remote_fault2
        #define mr_adv_next_page            Mr4.bits.next_page
    } Mr4;

    /* Link partner advertisement register. */
    union
    {
        struct
        {
            unsigned int reserved_4:5;
            unsigned int lp_full_duplex:1;
            unsigned int lp_half_duplex:1;
            unsigned int lp_sym_pause:1;
            unsigned int lp_asym_pause:1;
            unsigned int reserved_11:3;
            unsigned int lp_remote_fault1:1;
            unsigned int lp_remote_fault2:1;
            unsigned int lp_ack:1;
            unsigned int lp_next_page:1;
        } bits;

        unsigned short AsUSHORT;

        #define mr_lp_adv_full_duplex       Mr5.bits.lp_full_duplex
        #define mr_lp_adv_half_duplex       Mr5.bits.lp_half_duplex
        #define mr_lp_adv_sym_pause         Mr5.bits.lp_sym_pause
        #define mr_lp_adv_asym_pause        Mr5.bits.lp_asym_pause
        #define mr_lp_adv_remote_fault1     Mr5.bits.lp_remote_fault1
        #define mr_lp_adv_remote_fault2     Mr5.bits.lp_remote_fault2
        #define mr_lp_adv_next_page         Mr5.bits.lp_next_page
    } Mr5;

    /* Auto-negotiation expansion register. */
    union
    {
        struct
        {
            unsigned int reserved_0:1;
            unsigned int page_received:1;
            unsigned int next_pageable:1;
            unsigned int reserved_15:13;
        } bits;

        unsigned short AsUSHORT;
    } Mr6;

    /* Auto-negotiation next page transmit register. */
    union
    {
        struct
        {
            unsigned int code_field:11;
            unsigned int toggle:1;
            unsigned int ack2:1;
            unsigned int message_page:1;
            unsigned int reserved_14:1;
            unsigned int next_page:1;
        } bits;

        unsigned short AsUSHORT;

        #define mr_np_tx                    Mr7.AsUSHORT
    } Mr7;

    /* Auto-negotiation link partner ability register. */
    union
    {
        struct
        {
            unsigned int code_field:11;
            unsigned int toggle:1;
            unsigned int ack2:1;
            unsigned int message_page:1;
            unsigned int ack:1;
            unsigned int next_page:1;
        } bits;

        unsigned short AsUSHORT;
        
        #define mr_lp_np_rx                 Mr8.AsUSHORT
    } Mr8;

    /* Extended status register. */
    union
    {
        struct
        {
            unsigned int reserved_11:12;
            unsigned int base1000_t_hd:1;
            unsigned int base1000_t_fd:1;
            unsigned int base1000_x_hd:1;
            unsigned int base1000_x_fd:1;
        } bits;      

        unsigned short AsUSHORT;
    } Mr15;

    /* Miscellaneous state variables. */
    union
    {
        struct
        {
            unsigned int toggle_tx:1;
            unsigned int toggle_rx:1;
            unsigned int np_rx:1;
            unsigned int page_rx:1;
            unsigned int np_loaded:1;
        } bits;

        unsigned short AsUSHORT;

        #define mr_toggle_tx                MrMisc.bits.toggle_tx
        #define mr_toggle_rx                MrMisc.bits.toggle_rx
        #define mr_np_rx                    MrMisc.bits.np_rx
        #define mr_page_rx                  MrMisc.bits.page_rx
        #define mr_np_loaded                MrMisc.bits.np_loaded
    } MrMisc;

} AN_STATE_INFO, *PAN_STATE_INFO;



/******************************************************************************/
/* Return code of Autoneg8023z. */
/******************************************************************************/

typedef enum
{
    AUTONEG_STATUS_OK               = 0,
    AUTONEG_STATUS_DONE             = 1,
    AUTONEG_STATUS_TIMER_ENABLED    = 2,
//    AUTONEG_STATUS_FAILED           = 0xffffffff,
    AUTONEG_STATUS_FAILED           = 0xfffffff
} AUTONEG_STATUS, *PAUTONEG_STATUS;



/******************************************************************************/
/* Function prototypes. */
/******************************************************************************/

AUTONEG_STATUS Autoneg8023z(PAN_STATE_INFO pAnInfo);
void AutonegInit(PAN_STATE_INFO pAnInfo);



/******************************************************************************/
/* The following functions are defined in the os-dependent module. */
/******************************************************************************/

void MM_AnTxConfig(PAN_STATE_INFO pAnInfo);
void MM_AnTxIdle(PAN_STATE_INFO pAnInfo);
char MM_AnRxConfig(PAN_STATE_INFO pAnInfo, unsigned short *pRxConfig);



#endif /* AUTONEG_H */

