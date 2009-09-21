/*
 * Copyright (c) 2004-2007 Atheros Communications Inc.
 * All rights reserved.
 *
 * $ATH_LICENSE_HOSTSDK0_C$
 *
 */
#ifndef _INI_DSET_H_
#define _INI_DSET_H_

/*
 * Each of these represents a WHAL INI table, which consists
 * of an "address column" followed by 1 or more "value columns".
 *
 * Software uses the base WHAL_INI_DATA_ID+column to access a
 * DataSet that holds a particular column of data.
 */
typedef enum {
    WHAL_INI_DATA_ID_NULL               =0,
    WHAL_INI_DATA_ID_MODE_SPECIFIC      =1,  /* 2,3 */
    WHAL_INI_DATA_ID_COMMON             =4,  /* 5 */
    WHAL_INI_DATA_ID_BB_RFGAIN          =6,  /* 7,8 */
    WHAL_INI_DATA_ID_ANALOG_BANK1       =9,  /* 10 */
    WHAL_INI_DATA_ID_ANALOG_BANK2       =11, /* 12 */
    WHAL_INI_DATA_ID_ANALOG_BANK3       =13, /* 14, 15 */
    WHAL_INI_DATA_ID_ANALOG_BANK6       =16, /* 17, 18 */
    WHAL_INI_DATA_ID_ANALOG_BANK7       =19, /* 20 */
    WHAL_INI_DATA_ID_MODE_OVERRIDES     =21, /* 22,23 */
    WHAL_INI_DATA_ID_COMMON_OVERRIDES   =24, /* 25 */

    WHAL_INI_DATA_ID_MAX                =25
} WHAL_INI_DATA_ID;

typedef PREPACK struct {
    A_UINT16 freqIndex; // 1 - A mode 2 - B or G mode 0 - common
    A_UINT16 offset;
    A_UINT32 newValue;
} POSTPACK INI_DSET_REG_OVERRIDE;

#endif
