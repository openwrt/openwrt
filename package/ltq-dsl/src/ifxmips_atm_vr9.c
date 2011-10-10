/******************************************************************************
**
** FILE NAME    : ifxmips_atm_vr9.c
** PROJECT      : UEIP
** MODULES      : ATM
**
** DATE         : 7 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : ATM driver common source file (core functions)
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 07 JUL 2009  Xu Liang        Init Version
*******************************************************************************/



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <asm/delay.h>

/*
 *  Chip Specific Head File
 */
#include <ifx_types.h>
#include <ifx_regs.h>
#include <common_routines.h>
#include <ifx_pmu.h>
#include <ifx_rcu.h>
#include "ifxmips_atm_core.h"
#include "ifxmips_atm_fw_vr9.h"



/*
 * ####################################
 *              Definition
 * ####################################
 */



/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  Hardware Init/Uninit Functions
 */
static inline void init_pmu(void);
static inline void uninit_pmu(void);
static inline void reset_ppe(void);
static inline void init_pdma(void);
static inline void init_mailbox(void);
static inline void init_atm_tc(void);
static inline void clear_share_buffer(void);



/*
 * ####################################
 *            Local Variable
 * ####################################
 */



/*
 * ####################################
 *            Local Function
 * ####################################
 */

static inline void init_pmu(void)
{
    //*PMU_PWDCR &= ~((1 << 29) | (1 << 22) | (1 << 21) | (1 << 19) | (1 << 18));
    //PPE_TOP_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_SLL01_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_TC_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_EMA_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_QSB_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_TPE_PMU_SETUP(IFX_PMU_ENABLE);
    DSL_DFE_PMU_SETUP(IFX_PMU_ENABLE);
}

static inline void uninit_pmu(void)
{
    PPE_SLL01_PMU_SETUP(IFX_PMU_DISABLE);
    PPE_TC_PMU_SETUP(IFX_PMU_DISABLE);
    PPE_EMA_PMU_SETUP(IFX_PMU_DISABLE);
    PPE_QSB_PMU_SETUP(IFX_PMU_DISABLE);
    PPE_TPE_PMU_SETUP(IFX_PMU_DISABLE);
    DSL_DFE_PMU_SETUP(IFX_PMU_DISABLE);
    //PPE_TOP_PMU_SETUP(IFX_PMU_DISABLE);
}

static inline void reset_ppe(void)
{
#ifdef MODULE
    //  reset PPE
    ifx_rcu_rst(IFX_RCU_DOMAIN_DSLDFE, IFX_RCU_MODULE_ATM);
    udelay(1000);
    ifx_rcu_rst(IFX_RCU_DOMAIN_DSLTC, IFX_RCU_MODULE_ATM);
    udelay(1000);
    ifx_rcu_rst(IFX_RCU_DOMAIN_PPE, IFX_RCU_MODULE_ATM);
    udelay(1000);
    *PP32_SRST &= ~0x000303CF;
    udelay(1000);
    *PP32_SRST |= 0x000303CF;
    udelay(1000);
#endif
}

static inline void init_pdma(void)
{
    IFX_REG_W32(0x08,       PDMA_CFG);
    IFX_REG_W32(0x00203580, SAR_PDMA_RX_CMDBUF_CFG);
    IFX_REG_W32(0x004035A0, SAR_PDMA_RX_FW_CMDBUF_CFG);
}

static inline void init_mailbox(void)
{
    IFX_REG_W32(0xFFFFFFFF, MBOX_IGU1_ISRC);
    IFX_REG_W32(0x00000000, MBOX_IGU1_IER);
    IFX_REG_W32(0xFFFFFFFF, MBOX_IGU3_ISRC);
    IFX_REG_W32(0x00000000, MBOX_IGU3_IER);
}

static inline void init_atm_tc(void)
{
    /*  clear sync state    */
    *SFSM_STATE0    = 0;
    *SFSM_STATE1    = 0;

    /*  enable keep IDLE    */
//    *SFSM_CFG0     |= 1 << 15;
//    *SFSM_CFG1     |= 1 << 15;
}

static inline void clear_share_buffer(void)
{
    volatile u32 *p;
    unsigned int i;

    p = SB_RAM0_ADDR(0);
    for ( i = 0; i < SB_RAM0_DWLEN + SB_RAM1_DWLEN + SB_RAM2_DWLEN + SB_RAM3_DWLEN; i++ )
        IFX_REG_W32(0, p++);

    p = SB_RAM6_ADDR(0);
    for ( i = 0; i < SB_RAM6_DWLEN; i++ )
        IFX_REG_W32(0, p++);
}

/*
 *  Description:
 *    Download PPE firmware binary code.
 *  Input:
 *    pp32      --- int, which pp32 core
 *    src       --- u32 *, binary code buffer
 *    dword_len --- unsigned int, binary code length in DWORD (32-bit)
 *  Output:
 *    int       --- IFX_SUCCESS:    Success
 *                  else:           Error Code
 */
static inline int pp32_download_code(int pp32, u32 *code_src, unsigned int code_dword_len, u32 *data_src, unsigned int data_dword_len)
{
    unsigned int clr, set;
    volatile u32 *dest;

    if ( code_src == 0 || ((unsigned long)code_src & 0x03) != 0
        || data_src == 0 || ((unsigned long)data_src & 0x03) != 0 )
        return IFX_ERROR;

    clr = pp32 ? 0xF0 : 0x0F;
    if ( code_dword_len <= CDM_CODE_MEMORYn_DWLEN(0) )
        set = pp32 ? (3 << 6): (2 << 2);
    else
        set = 0x00;
    IFX_REG_W32_MASK(clr, set, CDM_CFG);

    /*  copy code   */
    dest = CDM_CODE_MEMORY(pp32, 0);
    while ( code_dword_len-- > 0 )
        IFX_REG_W32(*code_src++, dest++);

    /*  copy data   */
    dest = CDM_DATA_MEMORY(pp32, 0);
    while ( data_dword_len-- > 0 )
        IFX_REG_W32(*data_src++, dest++);

    return IFX_SUCCESS;
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */

extern void ifx_atm_get_fw_ver(unsigned int *major, unsigned int *minor)
{
    ASSERT(major != NULL, "pointer is NULL");
    ASSERT(minor != NULL, "pointer is NULL");

#ifdef VER_IN_FIRMWARE
    *major = FW_VER_ID->major;
    *minor = FW_VER_ID->minor;
#else
    *major = ATM_FW_VER_MAJOR;
    *minor = ATM_FW_VER_MINOR;
#endif
}

void ifx_atm_init_chip(void)
{
    init_pmu();

    reset_ppe();

    init_pdma();

    init_mailbox();

    init_atm_tc();

    clear_share_buffer();
}

void ifx_atm_uninit_chip(void)
{
    uninit_pmu();
}

/*
 *  Description:
 *    Initialize and start up PP32.
 *  Input:
 *    none
 *  Output:
 *    int  --- IFX_SUCCESS: Success
 *             else:        Error Code
 */
int ifx_pp32_start(int pp32)
{
    unsigned int mask = 1 << (pp32 << 4);
    int ret;

    /*  download firmware   */
    ret = pp32_download_code(pp32, firmware_binary_code, sizeof(firmware_binary_code) / sizeof(*firmware_binary_code), firmware_binary_data, sizeof(firmware_binary_data) / sizeof(*firmware_binary_data));
    if ( ret != IFX_SUCCESS )
        return ret;

    /*  run PP32    */
    IFX_REG_W32_MASK(mask, 0, PP32_FREEZE);

    /*  idle for a while to let PP32 init itself    */
    udelay(10);

    return IFX_SUCCESS;
}

/*
 *  Description:
 *    Halt PP32.
 *  Input:
 *    none
 *  Output:
 *    none
 */
void ifx_pp32_stop(int pp32)
{
    unsigned int mask = 1 << (pp32 << 4);

    /*  halt PP32   */
    IFX_REG_W32_MASK(0, mask, PP32_FREEZE);
}
