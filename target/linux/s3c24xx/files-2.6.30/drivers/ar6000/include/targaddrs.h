/*
 * Copyright (c) 2004-2007 Atheros Communications Inc.
 * All rights reserved.
 *
 * $ATH_LICENSE_HOSTSDK0_C$
 *
 */

#ifndef __TARGADDRS_H__
#define __TARGADDRS_H__
#if defined(AR6001)
#include "AR6001/addrs.h"
#endif
#if defined(AR6002)
#include "AR6002/addrs.h"
#endif

/*
 * AR6K option bits, to enable/disable various features.
 * By default, all option bits are 0.
 * These bits can be set in LOCAL_SCRATCH register 0.
 */
#define AR6K_OPTION_BMI_DISABLE      0x01 /* Disable BMI comm with Host */
#define AR6K_OPTION_SERIAL_ENABLE    0x02 /* Enable serial port msgs */
#define AR6K_OPTION_WDT_DISABLE      0x04 /* WatchDog Timer override */
#define AR6K_OPTION_SLEEP_DISABLE    0x08 /* Disable system sleep */
#define AR6K_OPTION_STOP_BOOT        0x10 /* Stop boot processes (for ATE) */
#define AR6K_OPTION_ENABLE_NOANI     0x20 /* Operate without ANI */
#define AR6K_OPTION_DSET_DISABLE     0x40 /* Ignore DataSets */
#define AR6K_OPTION_IGNORE_FLASH     0x80 /* Ignore flash during bootup */

/*
 * xxx_HOST_INTEREST_ADDRESS is the address in Target RAM of the
 * host_interest structure.  It must match the address of the _host_interest
 * symbol (see linker script).
 *
 * Host Interest is shared between Host and Target in order to coordinate
 * between the two, and is intended to remain constant (with additions only
 * at the end) across software releases.
 */
#define AR6001_HOST_INTEREST_ADDRESS     0x80000600
#define AR6002_HOST_INTEREST_ADDRESS     0x00500400

#define HOST_INTEREST_MAX_SIZE          0x100

#if !defined(__ASSEMBLER__)
struct register_dump_s;
struct dbglog_hdr_s;

/*
 * These are items that the Host may need to access
 * via BMI or via the Diagnostic Window. The position
 * of items in this structure must remain constant
 * across firmware revisions!
 *
 * Types for each item must be fixed size across
 * target and host platforms.
 *
 * More items may be added at the end.
 */
struct host_interest_s {
    /*
     * Pointer to application-defined area, if any.
     * Set by Target application during startup.
     */
    A_UINT32               hi_app_host_interest;                      /* 0x00 */

    /* Pointer to register dump area, valid after Target crash. */
    A_UINT32               hi_failure_state;                          /* 0x04 */

    /* Pointer to debug logging header */
    A_UINT32               hi_dbglog_hdr;                             /* 0x08 */

    /* Indicates whether or not flash is present on Target.
     * NB: flash_is_present indicator is here not just
     * because it might be of interest to the Host; but
     * also because it's set early on by Target's startup
     * asm code and we need it to have a special RAM address
     * so that it doesn't get reinitialized with the rest
     * of data.
     */
    A_UINT32               hi_flash_is_present;                       /* 0x0c */

    /*
     * General-purpose flag bits, similar to AR6000_OPTION_* flags.
     * Can be used by application rather than by OS.
     */
    A_UINT32               hi_option_flag;                            /* 0x10 */

    /*
     * Boolean that determines whether or not to
     * display messages on the serial port.
     */
    A_UINT32               hi_serial_enable;                          /* 0x14 */

    /* Start address of Flash DataSet index, if any */
    A_UINT32               hi_dset_list_head;                         /* 0x18 */

    /* Override Target application start address */
    A_UINT32               hi_app_start;                              /* 0x1c */

    /* Clock and voltage tuning */
    A_UINT32               hi_skip_clock_init;                        /* 0x20 */
    A_UINT32               hi_core_clock_setting;                     /* 0x24 */
    A_UINT32               hi_cpu_clock_setting;                      /* 0x28 */
    A_UINT32               hi_system_sleep_setting;                   /* 0x2c */
    A_UINT32               hi_xtal_control_setting;                   /* 0x30 */
    A_UINT32               hi_pll_ctrl_setting_24ghz;                 /* 0x34 */
    A_UINT32               hi_pll_ctrl_setting_5ghz;                  /* 0x38 */
    A_UINT32               hi_ref_voltage_trim_setting;               /* 0x3c */
    A_UINT32               hi_clock_info;                             /* 0x40 */

    /*
     * Flash configuration overrides, used only
     * when firmware is not executing from flash.
     * (When using flash, modify the global variables
     * with equivalent names.)
     */
    A_UINT32               hi_bank0_addr_value;                       /* 0x44 */
    A_UINT32               hi_bank0_read_value;                       /* 0x48 */
    A_UINT32               hi_bank0_write_value;                      /* 0x4c */
    A_UINT32               hi_bank0_config_value;                     /* 0x50 */

    /* Pointer to Board Data  */
    A_UINT32               hi_board_data;                             /* 0x54 */
    A_UINT32               hi_board_data_initialized;                 /* 0x58 */

    A_UINT32               hi_dset_RAM_index_table;                   /* 0x5c */

    A_UINT32               hi_desired_baud_rate;                      /* 0x60 */
    A_UINT32               hi_dbglog_config;                          /* 0x64 */
    A_UINT32               hi_end_RAM_reserve_sz;                     /* 0x68 */
    A_UINT32               hi_mbox_io_block_sz;                       /* 0x6c */

    A_UINT32               hi_num_bpatch_streams;                     /* 0x70 */
    A_UINT32               hi_mbox_isr_yield_limit;                   /* 0x74 */

    A_UINT32               hi_refclk_hz;                              /* 0x78 */
};

/* Bits defined in hi_option_flag */
#define HI_OPTION_TIMER_WAR     1 /* not really used */

/*
 * Intended for use by Host software, this macro returns the Target RAM
 * address of any item in the host_interest structure.
 * Example: target_addr = AR6001_HOST_INTEREST_ITEM_ADDRESS(hi_board_data);
 */
#define AR6001_HOST_INTEREST_ITEM_ADDRESS(item) \
    ((A_UINT32)&((((struct host_interest_s *)(AR6001_HOST_INTEREST_ADDRESS))->item)))

#define AR6002_HOST_INTEREST_ITEM_ADDRESS(item) \
    ((A_UINT32)&((((struct host_interest_s *)(AR6002_HOST_INTEREST_ADDRESS))->item)))


#endif /* !__ASSEMBLER__ */

#endif /* __TARGADDRS_H__ */
