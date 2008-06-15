/******************************************************************************/
/*                                                                            */
/* Broadcom BCM5700 Linux Network Driver, Copyright (c) 2000 - 2005 Broadcom  */
/* Corporation.                                                               */
/* All rights reserved.                                                       */
/*                                                                            */
/* This program is free software; you can redistribute it and/or modify       */
/* it under the terms of the GNU General Public License as published by       */
/* the Free Software Foundation, located in the file LICENSE.                 */
/*                                                                            */
/* History:                                                                   */
/*                                                                            */
/******************************************************************************/

#ifndef TIGON3_H
#define TIGON3_H

#include "lm.h"
#ifdef INCLUDE_TBI_SUPPORT
#include "autoneg.h"
#endif



/******************************************************************************/
/* Constants. */
/******************************************************************************/

#ifndef TIGON3_DEBUG
#define TIGON3_DEBUG	0
#endif /* TIGON3_DEBUG */

/* Number of entries in the Jumbo Receive RCB.  This value must 256 or 0. */
/* Currently, Jumbo Receive RCB is disabled. */
#ifndef T3_JUMBO_RCV_RCB_ENTRY_COUNT
#define T3_JUMBO_RCV_RCB_ENTRY_COUNT	0
#endif /* T3_JUMBO_RCV_RCB_ENTRY_COUNT */

#ifndef T3_JUMBO_RCV_ENTRY_COUNT
#define T3_JUMBO_RCV_ENTRY_COUNT	0
#endif /* T3_JUMBO_RCV_ENTRY_COUNT */

#ifndef T3_JUMBO_RCB_ENTRY_COUNT
#define T3_JUMBO_RCB_ENTRY_COUNT	0
#endif /* T3_JUMBO_RCB_ENTRY_COUNT */

/* Maxim number of packet descriptors used for sending packets. */
#define MAX_TX_PACKET_DESC_COUNT            T3_SEND_RCB_ENTRY_COUNT
#define DEFAULT_TX_PACKET_DESC_COUNT        120

/* Maximum number of packet descriptors used for receiving packets. */
#if T3_JUMBO_RCB_ENTRY_COUNT
#define MAX_RX_PACKET_DESC_COUNT                                            \
    (T3_STD_RCV_RCB_ENTRY_COUNT + T3_JUMBO_RCV_RCB_ENTRY_COUNT)
#else
#define MAX_RX_PACKET_DESC_COUNT            T3_STD_RCV_RCB_ENTRY_COUNT
#endif
#define DEFAULT_RX_PACKET_DESC_COUNT        200

/* Threshhold for double copying small tx packets.  0 will disable double */
/* copying of small Tx packets. */
#define DEFAULT_TX_COPY_BUFFER_SIZE         0
#define MIN_TX_COPY_BUFFER_SIZE             64 
#define MAX_TX_COPY_BUFFER_SIZE             512

/* Cache line. */
#define COMMON_CACHE_LINE_SIZE              0x20
#define COMMON_CACHE_LINE_MASK              (COMMON_CACHE_LINE_SIZE-1)

/* Maximum number of fragment we can handle. */
#ifndef MAX_FRAGMENT_COUNT
#define MAX_FRAGMENT_COUNT                  32
#endif

/* B0 bug. */
#define BCM5700_BX_MIN_FRAG_SIZE            10
#define BCM5700_BX_MIN_FRAG_BUF_SIZE        16  /* nice aligned size. */
#define BCM5700_BX_MIN_FRAG_BUF_SIZE_MASK   (BCM5700_BX_MIN_FRAG_BUF_SIZE-1)
#define BCM5700_BX_TX_COPY_BUF_SIZE         (BCM5700_BX_MIN_FRAG_BUF_SIZE * \
                                            MAX_FRAGMENT_COUNT)

/* MAGIC number. */
//#define T3_MAGIC_NUM                        'KevT'
#define T3_FIRMWARE_MAILBOX                0x0b50
#define T3_MAGIC_NUM_FIRMWARE_INIT_DONE    0x4B657654 
#define T3_MAGIC_NUM_DISABLE_DMAW_ON_LINK_CHANGE 0x4861764b

#define T3_NIC_DATA_SIG_ADDR               0x0b54
#define T3_NIC_DATA_SIG                    0x4b657654

#define T3_NIC_DATA_NIC_CFG_ADDR           0x0b58
#define T3_NIC_CFG_LED_MAC_MODE            BIT_NONE
#define T3_NIC_CFG_LED_PHY_MODE_1          BIT_2
#define T3_NIC_CFG_LED_PHY_MODE_2          BIT_3
#define T3_NIC_CFG_LED_MODE_MASK           (BIT_2 | BIT_3)
#define T3_NIC_CFG_PHY_TYPE_UNKNOWN         BIT_NONE
#define T3_NIC_CFG_PHY_TYPE_COPPER          BIT_4
#define T3_NIC_CFG_PHY_TYPE_FIBER           BIT_5
#define T3_NIC_CFG_PHY_TYPE_MASK            (BIT_4 | BIT_5)
#define T3_NIC_CFG_ENABLE_WOL               BIT_6
#define T3_NIC_CFG_ENABLE_ASF               BIT_7
#define T3_NIC_EEPROM_WP                    BIT_8
#define T3_NIC_WOL_LIMIT_10                 BIT_10
#define T3_NIC_MINI_PCI                     BIT_12
#define T3_NIC_FIBER_WOL_CAPABLE            BIT_14
#define T3_NIC_BOTH_PORT_100MB_WOL_CAPABLE  BIT_15
#define T3_NIC_GPIO2_NOT_AVAILABLE          BIT_20

#define T3_NIC_DATA_VER                     0x0b5c
#define T3_NIC_DATA_VER_SHIFT               16

#define T3_NIC_DATA_PHY_ID_ADDR            0x0b74
#define T3_NIC_PHY_ID1_MASK                0xffff0000
#define T3_NIC_PHY_ID2_MASK                0x0000ffff

#define T3_CMD_MAILBOX                      0x0b78
#define T3_CMD_NICDRV_ALIVE                 0x01
#define T3_CMD_NICDRV_PAUSE_FW              0x02
#define T3_CMD_NICDRV_IPV4ADDR_CHANGE       0x03
#define T3_CMD_NICDRV_IPV6ADDR_CHANGE       0x04
#define T3_CMD_5703A0_FIX_DMAFW_DMAR        0x05
#define T3_CMD_5703A0_FIX_DMAFW_DMAW        0x06

#define T3_CMD_NICDRV_ALIVE2                0x0d

#define T3_CMD_LENGTH_MAILBOX               0x0b7c
#define T3_CMD_DATA_MAILBOX                 0x0b80

#define T3_ASF_FW_STATUS_MAILBOX            0x0c00

#define T3_DRV_STATE_MAILBOX                0x0c04
#define T3_DRV_STATE_START                  0x01
#define T3_DRV_STATE_START_DONE             0x80000001
#define T3_DRV_STATE_UNLOAD                 0x02
#define T3_DRV_STATE_UNLOAD_DONE            0x80000002
#define T3_DRV_STATE_WOL                    0x03
#define T3_DRV_STATE_SUSPEND                0x04

#define T3_FW_RESET_TYPE_MAILBOX            0x0c08

#define T3_MAC_ADDR_HIGH_MAILBOX            0x0c14
#define T3_MAC_ADDR_LOW_MAILBOX             0x0c18

#define DRV_WOL_MAILBOX                     0xd30
#define DRV_WOL_SIGNATURE                   0x474c0000

#define DRV_DOWN_STATE_SHUTDOWN             0x1

#define DRV_WOL_SET_MAGIC_PKT               BIT_2

#define T3_NIC_DATA_NIC_CFG_ADDR2           0x0d38 /* bit 2-3 are same as in */
                                                   /* 0xb58 */
#define T3_SHASTA_EXT_LED_MODE_MASK         (BIT_15 | BIT_16)
#define T3_SHASTA_EXT_LED_LEGACY_MODE       BIT_NONE
#define T3_SHASTA_EXT_LED_SHARED_TRAFFIC_LINK_MODE       BIT_15
#define T3_SHASTA_EXT_LED_MAC_MODE          BIT_16
#define T3_SHASTA_EXT_LED_WIRELESS_COMBO_MODE       (BIT_15 | BIT_16)
#define T3_NIC_CFG_CAPACITIVE_COUPLING            BIT_17
#define T3_NIC_CFG_PRESERVE_PREEMPHASIS           BIT_18

/******************************************************************************/
/* Hardware constants. */
/******************************************************************************/

/* Number of entries in the send ring:  must be 512. */
#define T3_SEND_RCB_ENTRY_COUNT             512     
#define T3_SEND_RCB_ENTRY_COUNT_MASK        (T3_SEND_RCB_ENTRY_COUNT-1)

/* Number of send RCBs.  May be 1-16 but for now, only support one. */
#define T3_MAX_SEND_RCB_COUNT               16

/* Number of entries in the Standard Receive RCB.  Must be 512 entries. */
#define T3_STD_RCV_RCB_ENTRY_COUNT          512
#define T3_STD_RCV_RCB_ENTRY_COUNT_MASK     (T3_STD_RCV_RCB_ENTRY_COUNT-1)
#define DEFAULT_STD_RCV_DESC_COUNT          200    /* Must be < 512. */
#define MAX_STD_RCV_BUFFER_SIZE             0x600

/* Number of entries in the Mini Receive RCB.  This value can either be */
/* 0, 1024.  Currently Mini Receive RCB is disabled. */
#ifndef T3_MINI_RCV_RCB_ENTRY_COUNT
#define T3_MINI_RCV_RCB_ENTRY_COUNT         0
#endif /* T3_MINI_RCV_RCB_ENTRY_COUNT */
#define T3_MINI_RCV_RCB_ENTRY_COUNT_MASK    (T3_MINI_RCV_RCB_ENTRY_COUNT-1)
#define MAX_MINI_RCV_BUFFER_SIZE            512
#define DEFAULT_MINI_RCV_BUFFER_SIZE        64
#define DEFAULT_MINI_RCV_DESC_COUNT         100    /* Must be < 1024. */

#define T3_JUMBO_RCV_RCB_ENTRY_COUNT_MASK   (T3_JUMBO_RCV_RCB_ENTRY_COUNT-1)

#define MAX_JUMBO_RCV_BUFFER_SIZE           (10 * 1024) /* > 1514 */
#define DEFAULT_JUMBO_RCV_BUFFER_SIZE       (4 * 1024) /* > 1514 */
#define DEFAULT_JUMBO_RCV_DESC_COUNT        128     /* Must be < 256. */

#define MAX_JUMBO_TX_BUFFER_SIZE            (8 * 1024) /* > 1514 */
#define DEFAULT_JUMBO_TX_BUFFER_SIZE        (4 * 1024) /* > 1514 */

/* Number of receive return RCBs.  Maybe 1-16 but for now, only support one. */
#define T3_MAX_RCV_RETURN_RCB_COUNT         16

/* Number of entries in a Receive Return ring.  This value is either 1024 */
/* or 2048. */
#ifndef T3_RCV_RETURN_RCB_ENTRY_COUNT 
#define T3_RCV_RETURN_RCB_ENTRY_COUNT       1024
#endif /* T3_RCV_RETURN_RCB_ENTRY_COUNT */
#define T3_RCV_RETURN_RCB_ENTRY_COUNT_MASK  (T3_RCV_RETURN_RCB_ENTRY_COUNT-1)


/* Default coalescing parameters. */
#ifdef BCM_NAPI_RXPOLL
#define DEFAULT_RX_COALESCING_TICKS         18
#define DEFAULT_RX_MAX_COALESCED_FRAMES     6
#else
#define DEFAULT_RX_COALESCING_TICKS         60
#define DEFAULT_RX_MAX_COALESCED_FRAMES     15
#endif

#define DEFAULT_TX_COALESCING_TICKS         200
#define DEFAULT_TX_MAX_COALESCED_FRAMES     35

#define MAX_RX_COALESCING_TICKS             500
#define MAX_TX_COALESCING_TICKS             500
#define MAX_RX_MAX_COALESCED_FRAMES         100
#define MAX_TX_MAX_COALESCED_FRAMES         100

#define ADAPTIVE_LO_RX_MAX_COALESCED_FRAMES    5
#define ADAPTIVE_HI_RX_MAX_COALESCED_FRAMES    48
#define ADAPTIVE_LO_RX_COALESCING_TICKS         25
#define ADAPTIVE_HI_RX_COALESCING_TICKS         120
#define ADAPTIVE_LO_PKT_THRESH              52000
#define ADAPTIVE_HI_PKT_THRESH              112000
#define ADAPTIVE_LO_TX_MAX_COALESCED_FRAMES    20
#define ADAPTIVE_HI_TX_MAX_COALESCED_FRAMES    75

#ifdef BCM_NAPI_RXPOLL
#define DEFAULT_RX_COALESCING_TICKS_DURING_INT          18
#define DEFAULT_RX_MAX_COALESCED_FRAMES_DURING_INT      6
#else
#define DEFAULT_RX_COALESCING_TICKS_DURING_INT          25
#define DEFAULT_RX_MAX_COALESCED_FRAMES_DURING_INT      2
#endif
#define DEFAULT_TX_COALESCING_TICKS_DURING_INT          25
#define ADAPTIVE_LO_RX_MAX_COALESCED_FRAMES_DURING_INT  1
#define ADAPTIVE_HI_RX_MAX_COALESCED_FRAMES_DURING_INT  5
#define DEFAULT_TX_MAX_COALESCED_FRAMES_DURING_INT      5

#define BAD_DEFAULT_VALUE                               0xffffffff

#define DEFAULT_STATS_COALESCING_TICKS      1000000
#define MIN_STATS_COALESCING_TICKS          100
#define MAX_STATS_COALESCING_TICKS          3600000000U


/* Receive BD Replenish thresholds. */
#define DEFAULT_RCV_STD_BD_REPLENISH_THRESHOLD      4
#define DEFAULT_RCV_JUMBO_BD_REPLENISH_THRESHOLD    4

/* Maximum physical fragment size. */
#define MAX_FRAGMENT_SIZE                   (64 * 1024)


/* Standard view. */
#define T3_STD_VIEW_SIZE                    (64 * 1024)
#define T3_FLAT_VIEW_SIZE                   (32 * 1024 * 1024)


/* Buffer descriptor base address on the NIC's memory. */

#define T3_NIC_SND_BUFFER_DESC_ADDR         0x4000
#define T3_NIC_STD_RCV_BUFFER_DESC_ADDR     0x6000
#define T3_NIC_JUMBO_RCV_BUFFER_DESC_ADDR   0x7000

#define T3_NIC_STD_RCV_BUFFER_DESC_ADDR_EXT_MEM     0xc000
#define T3_NIC_JUMBO_RCV_BUFFER_DESC_ADDR_EXT_MEM   0xd000
#define T3_NIC_MINI_RCV_BUFFER_DESC_ADDR_EXT_MEM    0xe000

#define T3_NIC_SND_BUFFER_DESC_SIZE         (T3_SEND_RCB_ENTRY_COUNT * \
                                            sizeof(T3_SND_BD) / 4)

#define T3_NIC_STD_RCV_BUFFER_DESC_SIZE     (T3_STD_RCV_RCB_ENTRY_COUNT * \
                                            sizeof(T3_RCV_BD) / 4)

#define T3_NIC_JUMBO_RCV_BUFFER_DESC_SIZE   (T3_JUMBO_RCV_RCB_ENTRY_COUNT * \
                                            sizeof(T3_EXT_RCV_BD) / 4)


/* MBUF pool. */
#define T3_NIC_MBUF_POOL_ADDR               0x8000
#define T3_NIC_MBUF_POOL_SIZE32             0x8000
#define T3_NIC_MBUF_POOL_SIZE96             0x18000
#define T3_NIC_MBUF_POOL_SIZE64             0x10000

#define T3_NIC_MBUF_POOL_ADDR_EXT_MEM       0x20000

#define T3_NIC_BCM5705_MBUF_POOL_ADDR               0x10000
#define T3_NIC_BCM5705_MBUF_POOL_SIZE               0xe000

/* DMA descriptor pool */
#define T3_NIC_DMA_DESC_POOL_ADDR           0x2000
#define T3_NIC_DMA_DESC_POOL_SIZE           0x2000      /* 8KB. */

#define T3_DEF_DMA_MBUF_LOW_WMARK           0x50
#define T3_DEF_RX_MAC_MBUF_LOW_WMARK        0x20
#define T3_DEF_MBUF_HIGH_WMARK              0x60

#define T3_DEF_DMA_MBUF_LOW_WMARK_5705       0x0
#define T3_DEF_RX_MAC_MBUF_LOW_WMARK_5705    0x10
#define T3_DEF_MBUF_HIGH_WMARK_5705          0x60

#define T3_DEF_DMA_MBUF_LOW_WMARK_JUMBO     304
#define T3_DEF_RX_MAC_MBUF_LOW_WMARK_JUMBO  152
#define T3_DEF_MBUF_HIGH_WMARK_JUMBO        380

#define T3_DEF_DMA_DESC_LOW_WMARK           5
#define T3_DEF_DMA_DESC_HIGH_WMARK          10

/* Maximum size of giant TCP packet can be sent */
#define T3_TCP_SEG_MAX_OFFLOAD_SIZE         64*1000
#define T3_TCP_SEG_MIN_NUM_SEG              20

#define T3_RX_CPU_ID    0x1
#define T3_TX_CPU_ID    0x2
#define T3_RX_CPU_SPAD_ADDR  0x30000
#define T3_RX_CPU_SPAD_SIZE  0x4000
#define T3_TX_CPU_SPAD_ADDR  0x34000
#define T3_TX_CPU_SPAD_SIZE  0x4000

typedef struct T3_DIR_ENTRY
{
  PLM_UINT8 Buffer;
  LM_UINT32 Offset;
  LM_UINT32 Length;
} T3_DIR_ENTRY,*PT3_DIR_ENTRY;

typedef struct T3_FWIMG_INFO
{
  LM_UINT32 StartAddress;
  T3_DIR_ENTRY Text;
  T3_DIR_ENTRY ROnlyData;
  T3_DIR_ENTRY Data;
  T3_DIR_ENTRY Sbss;
  T3_DIR_ENTRY Bss;
} T3_FWIMG_INFO, *PT3_FWIMG_INFO;



/******************************************************************************/
/* Tigon3 PCI Registers. */
/******************************************************************************/
/* MSI ENABLE bit is located at this offset */
#define T3_PCI_MSI_ENABLE                   0x58

#define T3_PCI_ID_BCM5700                   0x164414e4
#define T3_PCI_ID_BCM5701                   0x164514e4
#define T3_PCI_ID_BCM5702                   0x164614e4
#define T3_PCI_ID_BCM5702x                  0x16A614e4
#define T3_PCI_ID_BCM5703                   0x164714e4
#define T3_PCI_ID_BCM5703x                  0x16A714e4
#define T3_PCI_ID_BCM5702FE                 0x164D14e4
#define T3_PCI_ID_BCM5704                   0x164814e4
#define T3_PCI_ID_BCM5705                   0x165314e4
#define T3_PCI_ID_BCM5705M                  0x165D14e4
#define T3_PCI_ID_BCM5705F                  0x166E14e4
#define T3_PCI_ID_BCM5901                   0x170D14e4
#define T3_PCI_ID_BCM5901A2                 0x170E14e4
#define T3_PCI_ID_BCM5751F                  0x167E14e4

#define T3_PCI_ID_BCM471F                   0x471f14e4

#define T3_PCI_ID_BCM5753                   0x16f714e4
#define T3_PCI_ID_BCM5753M                  0x16fd14e4
#define T3_PCI_ID_BCM5753F                  0x16fe14e4
#define T3_PCI_ID_BCM5781                   0x16dd14e4

#define T3_PCI_ID_BCM5903M                  0x16ff14e4

#define T3_PCI_VENDOR_ID(x)                 ((x) & 0xffff)
#define T3_PCI_DEVICE_ID(x)                 ((x) >> 16)

#define T3_PCI_MISC_HOST_CTRL_REG           0x68

/* The most significant 16bit of register 0x68. */
/* ChipId:4, ChipRev:4, MetalRev:8 */
#define T3_CHIP_ID_5700_A0                  0x7000
#define T3_CHIP_ID_5700_A1                  0x7001
#define T3_CHIP_ID_5700_B0                  0x7100
#define T3_CHIP_ID_5700_B1                  0x7101
#define T3_CHIP_ID_5700_C0                  0x7200

#define T3_CHIP_ID_5701_A0                  0x0000
#define T3_CHIP_ID_5701_B0                  0x0100
#define T3_CHIP_ID_5701_B2                  0x0102
#define T3_CHIP_ID_5701_B5                  0x0105

#define T3_CHIP_ID_5703_A0                  0x1000
#define T3_CHIP_ID_5703_A1                  0x1001
#define T3_CHIP_ID_5703_A2                  0x1002
#define T3_CHIP_ID_5703_A3                  0x1003

#define T3_CHIP_ID_5704_A0                  0x2000
#define T3_CHIP_ID_5704_A1                  0x2001
#define T3_CHIP_ID_5704_A2                  0x2002

#define T3_CHIP_ID_5705_A0                  0x3000
#define T3_CHIP_ID_5705_A1                  0x3001
#define T3_CHIP_ID_5705_A2                  0x3002
#define T3_CHIP_ID_5705_A3                  0x3003

#define T3_CHIP_ID_5750_A0                  0x4000
#define T3_CHIP_ID_5750_A1                  0x4001
#define T3_CHIP_ID_5750_A3                  0x4003
#define T3_CHIP_ID_5750_B0                  0x4010
#define T3_CHIP_ID_5750_C0                  0x4200

#define T3_CHIP_ID_5714_A0                  0x5000
#define T3_CHIP_ID_5752_A0                  0x6000
#define T3_CHIP_ID_5714                     0x8000


/* Chip Id. */
#define T3_ASIC_REV(_ChipRevId)             ((_ChipRevId) >> 12)
#define T3_ASIC_REV_5700                    0x07
#define T3_ASIC_REV_5701                    0x00
#define T3_ASIC_REV_5703                    0x01
#define T3_ASIC_REV_5704                    0x02
#define T3_ASIC_REV_5705                    0x03
#define T3_ASIC_REV_5750                    0x04
#define T3_ASIC_REV_5714_A0                 0x05 /*5714,5715*/
#define T3_ASIC_REV_5752                    0x06
#define T3_ASIC_REV_5780                    0x08 /* 5780 previously htle */
#define T3_ASIC_REV_5714                    0x09 /*5714,5715*/

#define T3_ASIC_IS_5705_BEYOND(_ChipRevId)                 \
   ((T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5705)       || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5750)       || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5714_A0)    || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5780)       || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5714)       || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5752))       

#define T3_ASIC_IS_575X_PLUS(_ChipRevId)                   \
   ((T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5750)       || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5714_A0)    || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5780)       || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5714)       || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5752))       

#define T3_ASIC_5714_FAMILY(_ChipRevId)                    \
   ((T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5714_A0)   || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5780)      || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5714))

#define T3_ASIC_IS_JUMBO_CAPABLE(_ChipRevId)		\
    ((T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5700)	|| \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5701)	|| \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5703)	|| \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5714_A0)    || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5780)       || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5714)       || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5704))       

#define T3_ASIC_5752(_ChipRevId)  \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5752)

#define T3_ASIC_5705_OR_5750(_ChipRevId)              \
    ((T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5705) || \
    (T3_ASIC_REV(_ChipRevId) == T3_ASIC_REV_5750))

/* Chip id and revision. */
#define T3_CHIP_REV(_ChipRevId)             ((_ChipRevId) >> 8)
#define T3_CHIP_REV_5700_AX                 0x70
#define T3_CHIP_REV_5700_BX                 0x71
#define T3_CHIP_REV_5700_CX                 0x72
#define T3_CHIP_REV_5701_AX                 0x00
#define T3_CHIP_REV_5703_AX                 0x10
#define T3_CHIP_REV_5704_AX                 0x20
#define T3_CHIP_REV_5704_BX                 0x21

#define T3_CHIP_REV_5750_AX                 0x40
#define T3_CHIP_REV_5750_BX                 0x41

/* Metal revision. */
#define T3_METAL_REV(_ChipRevId)            ((_ChipRevId) & 0xff)
#define T3_METAL_REV_A0                     0x00
#define T3_METAL_REV_A1                     0x01
#define T3_METAL_REV_B0                     0x00
#define T3_METAL_REV_B1                     0x01
#define T3_METAL_REV_B2                     0x02

#define T3_PCI_REG_CLOCK_CTRL               0x74

#define T3_PCI_DISABLE_RX_CLOCK             BIT_10
#define T3_PCI_DISABLE_TX_CLOCK             BIT_11
#define T3_PCI_SELECT_ALTERNATE_CLOCK       BIT_12
#define T3_PCI_POWER_DOWN_PCI_PLL133        BIT_15
#define T3_PCI_44MHZ_CORE_CLOCK             BIT_18
#define T3_PCI_625_CORE_CLOCK               BIT_20
#define T3_PCI_FORCE_CLKRUN                 BIT_21
#define T3_PCI_CLKRUN_OUTPUT_EN             BIT_22


#define T3_PCI_REG_ADDR_REG                 0x78
#define T3_PCI_REG_DATA_REG                 0x80

#define T3_PCI_MEM_WIN_ADDR_REG             0x7c
#define T3_PCI_MEM_WIN_DATA_REG             0x84

#define T3_PCI_PM_CAP_REG                   0x48

#define T3_PCI_PM_CAP_PME_D3COLD            BIT_31
#define T3_PCI_PM_CAP_PME_D3HOT             BIT_30

#define T3_PCI_PM_STATUS_CTRL_REG           0x4c

#define T3_PM_POWER_STATE_MASK              (BIT_0 | BIT_1)
#define T3_PM_POWER_STATE_D0                BIT_NONE
#define T3_PM_POWER_STATE_D1                BIT_0
#define T3_PM_POWER_STATE_D2                BIT_1
#define T3_PM_POWER_STATE_D3                (BIT_0 | BIT_1)

#define T3_PM_PME_ENABLE                    BIT_8
#define T3_PM_PME_ASSERTED                  BIT_15

#define T3_MSI_CAPABILITY_ID_REG            0x58
#define T3_MSI_NEXT_CAPABILITY_PTR          0x59

/* PCI state register. */
#define T3_PCI_STATE_REG                    0x70

#define T3_PCI_STATE_FORCE_RESET            BIT_0
#define T3_PCI_STATE_INT_NOT_ACTIVE         BIT_1
#define T3_PCI_STATE_CONVENTIONAL_PCI_MODE  BIT_2
#define T3_PCI_STATE_BUS_SPEED_HIGH         BIT_3
#define T3_PCI_STATE_32BIT_PCI_BUS          BIT_4


/* Broadcom subsystem/subvendor IDs. */
#define T3_SVID_BROADCOM                            0x14e4

#define T3_SSID_BROADCOM_BCM95700A6                 0x1644
#define T3_SSID_BROADCOM_BCM95701A5                 0x0001
#define T3_SSID_BROADCOM_BCM95700T6                 0x0002  /* BCM8002 */
#define T3_SSID_BROADCOM_BCM95700A9                 0x0003  /* Agilent */
#define T3_SSID_BROADCOM_BCM95701T1                 0x0005
#define T3_SSID_BROADCOM_BCM95701T8                 0x0006
#define T3_SSID_BROADCOM_BCM95701A7                 0x0007  /* Agilent */
#define T3_SSID_BROADCOM_BCM95701A10                0x0008
#define T3_SSID_BROADCOM_BCM95701A12                0x8008
#define T3_SSID_BROADCOM_BCM95703Ax1                0x0009
#define T3_SSID_BROADCOM_BCM95703Ax2                0x8009

/* 3COM subsystem/subvendor IDs. */
#define T3_SVID_3COM                                0x10b7

#define T3_SSID_3COM_3C996T                         0x1000
#define T3_SSID_3COM_3C996BT                        0x1006
#define T3_SSID_3COM_3C996CT                        0x1002
#define T3_SSID_3COM_3C997T                         0x1003
#define T3_SSID_3COM_3C1000T                        0x1007
#define T3_SSID_3COM_3C940BR01                      0x1008

/* Fiber boards. */
#define T3_SSID_3COM_3C996SX                        0x1004
#define T3_SSID_3COM_3C997SX                        0x1005


/* Dell subsystem/subvendor IDs. */

#define T3_SVID_DELL                                0x1028

#define T3_SSID_DELL_VIPER                          0x00d1
#define T3_SSID_DELL_JAGUAR                         0x0106
#define T3_SSID_DELL_MERLOT                         0x0109
#define T3_SSID_DELL_SLIM_MERLOT                    0x010a

/* Compaq subsystem/subvendor IDs */

#define T3_SVID_COMPAQ                              0x0e11

#define T3_SSID_COMPAQ_BANSHEE                      0x007c
#define T3_SSID_COMPAQ_BANSHEE_2                    0x009a
#define T3_SSID_COMPAQ_CHANGELING                   0x007d
#define T3_SSID_COMPAQ_NC7780                       0x0085
#define T3_SSID_COMPAQ_NC7780_2                     0x0099

#define T3_PCIE_CAPABILITY_ID_REG           0xD0
#define T3_PCIE_CAPABILITY_ID               0x10

#define T3_PCIE_CAPABILITY_REG              0xD2

/******************************************************************************/
/* MII registers. */
/******************************************************************************/

/* Control register. */
#define PHY_CTRL_REG                                0x00

#define PHY_CTRL_SPEED_MASK                         (BIT_6 | BIT_13)
#define PHY_CTRL_SPEED_SELECT_10MBPS                BIT_NONE
#define PHY_CTRL_SPEED_SELECT_100MBPS               BIT_13
#define PHY_CTRL_SPEED_SELECT_1000MBPS              BIT_6
#define PHY_CTRL_COLLISION_TEST_ENABLE              BIT_7
#define PHY_CTRL_FULL_DUPLEX_MODE                   BIT_8
#define PHY_CTRL_RESTART_AUTO_NEG                   BIT_9
#define PHY_CTRL_ISOLATE_PHY                        BIT_10
#define PHY_CTRL_LOWER_POWER_MODE                   BIT_11
#define PHY_CTRL_AUTO_NEG_ENABLE                    BIT_12
#define PHY_CTRL_LOOPBACK_MODE                      BIT_14
#define PHY_CTRL_PHY_RESET                          BIT_15


/* Status register. */
#define PHY_STATUS_REG                              0x01

#define PHY_STATUS_LINK_PASS                        BIT_2
#define PHY_STATUS_AUTO_NEG_COMPLETE                BIT_5


/* Phy Id registers. */
#define PHY_ID1_REG                                 0x02
#define PHY_ID1_OUI_MASK                            0xffff

#define PHY_ID2_REG                                 0x03
#define PHY_ID2_REV_MASK                            0x000f
#define PHY_ID2_MODEL_MASK                          0x03f0
#define PHY_ID2_OUI_MASK                            0xfc00


/* Auto-negotiation advertisement register. */
#define PHY_AN_AD_REG                               0x04

#define PHY_AN_AD_ASYM_PAUSE                        BIT_11
#define PHY_AN_AD_PAUSE_CAPABLE                     BIT_10
#define PHY_AN_AD_10BASET_HALF                      BIT_5
#define PHY_AN_AD_10BASET_FULL                      BIT_6
#define PHY_AN_AD_100BASETX_HALF                    BIT_7
#define PHY_AN_AD_100BASETX_FULL                    BIT_8
#define PHY_AN_AD_PROTOCOL_802_3_CSMA_CD            0x01

/* Defines for 5714 family fiber on the 546x phy*/

#define PHY_AN_AD_1000XFULL            		0x20
#define PHY_AN_AD_1000XHALF             	0x40
#define PHY_AN_AD_1000XPAUSE            	0x80
#define PHY_AN_AD_1000XPSE_ASYM         	0x100
#define PHY_AN_AD_1000XREM_FAULT_OFFLINE        0x2000
#define PHY_AN_AD_1000XREM_FAULT_AN_ERROR       0x3000

#define PHY_AN_AD_ALL_SPEEDS (PHY_AN_AD_10BASET_HALF | PHY_AN_AD_10BASET_FULL |\
    PHY_AN_AD_100BASETX_HALF | PHY_AN_AD_100BASETX_FULL)

/* Auto-negotiation Link Partner Ability register. */
#define PHY_LINK_PARTNER_ABILITY_REG                0x05

#define PHY_LINK_PARTNER_ASYM_PAUSE                 BIT_11
#define PHY_LINK_PARTNER_PAUSE_CAPABLE              BIT_10


/* Auto-negotiation expansion register. */
#define PHY_AN_EXPANSION_REG                        0x06



/******************************************************************************/
/* BCM5400 and BCM5401 phy info. */
/******************************************************************************/

#define PHY_DEVICE_ID           1

/* OUI: bit 31-10;   Model#: bit 9-4;   Rev# bit 3-0. */
#define PHY_UNKNOWN_PHY                             0x00000000
#define PHY_BCM5400_PHY_ID                          0x60008040
#define PHY_BCM5401_PHY_ID                          0x60008050
#define PHY_BCM5411_PHY_ID                          0x60008070
#define PHY_BCM5461_PHY_ID                          0x600080c0
#define PHY_BCM5701_PHY_ID                          0x60008110
#define PHY_BCM5703_PHY_ID                          0x60008160
#define PHY_BCM5704_PHY_ID                          0x60008190
#define PHY_BCM5705_PHY_ID                          0x600081a0
#define PHY_BCM5750_PHY_ID                          0x60008180
#define PHY_BCM8002_PHY_ID                          0x60010140
#define PHY_BCM5714_PHY_ID                          0x60008340
#define PHY_BCM5780_PHY_ID                          0x60008350
#define PHY_BCM5752_PHY_ID                          0x60008100

#define PHY_BCM5401_B0_REV                          0x1
#define PHY_BCM5401_B2_REV                          0x3
#define PHY_BCM5401_C0_REV                          0x6

#define PHY_ID_OUI_MASK                             0xfffffc00
#define PHY_ID_MODEL_MASK                           0x000003f0
#define PHY_ID_REV_MASK                             0x0000000f
#define PHY_ID_MASK                                 (PHY_ID_OUI_MASK |      \
                                                    PHY_ID_MODEL_MASK)

#define UNKNOWN_PHY_ID(x)   ((((x) & PHY_ID_MASK) != PHY_BCM5400_PHY_ID) && \
                            (((x) & PHY_ID_MASK) != PHY_BCM5401_PHY_ID) && \
                            (((x) & PHY_ID_MASK) != PHY_BCM5411_PHY_ID) && \
                            (((x) & PHY_ID_MASK) != PHY_BCM5701_PHY_ID) && \
                            (((x) & PHY_ID_MASK) != PHY_BCM5703_PHY_ID) && \
                            (((x) & PHY_ID_MASK) != PHY_BCM5704_PHY_ID) && \
                            (((x) & PHY_ID_MASK) != PHY_BCM5705_PHY_ID) && \
                            (((x) & PHY_ID_MASK) != PHY_BCM5750_PHY_ID) && \
                            (((x) & PHY_ID_MASK) != PHY_BCM8002_PHY_ID) && \
                            (((x) & PHY_ID_MASK) != PHY_BCM5714_PHY_ID) && \
                            (((x) & PHY_ID_MASK) != PHY_BCM5780_PHY_ID) && \
                            (((x) & PHY_ID_MASK) != PHY_BCM5752_PHY_ID) && \
                            (((x) & PHY_ID_MASK) != PHY_BCM5461_PHY_ID))

/* 1000Base-T control register. */
#define BCM540X_1000BASET_CTRL_REG                  0x09

#define BCM540X_AN_AD_1000BASET_HALF                BIT_8
#define BCM540X_AN_AD_1000BASET_FULL                BIT_9
#define BCM540X_CONFIG_AS_MASTER                    BIT_11
#define BCM540X_ENABLE_CONFIG_AS_MASTER             BIT_12

#define BCM540X_AN_AD_ALL_1G_SPEEDS (BCM540X_AN_AD_1000BASET_HALF | \
    BCM540X_AN_AD_1000BASET_FULL)

/* Extended control register. */
#define BCM540X_EXT_CTRL_REG                        0x10

#define BCM540X_EXT_CTRL_LINK3_LED_MODE             BIT_1
#define BCM540X_EXT_CTRL_FORCE_LED_OFF              BIT_3
#define BCM540X_EXT_CTRL_TBI                        BIT_15

/* PHY extended status register. */
#define BCM540X_EXT_STATUS_REG                      0x11

#define BCM540X_EXT_STATUS_LINK_PASS                BIT_8


/* DSP Coefficient Read/Write Port. */
#define BCM540X_DSP_RW_PORT                         0x15


/* DSP Coeficient Address Register. */
#define BCM540X_DSP_ADDRESS_REG                     0x17

#define BCM540X_DSP_TAP_NUMBER_MASK                 0x00
#define BCM540X_DSP_AGC_A                           0x00
#define BCM540X_DSP_AGC_B                           0x01
#define BCM540X_DSP_MSE_PAIR_STATUS                 0x02
#define BCM540X_DSP_SOFT_DECISION                   0x03
#define BCM540X_DSP_PHASE_REG                       0x04
#define BCM540X_DSP_SKEW                            0x05
#define BCM540X_DSP_POWER_SAVER_UPPER_BOUND         0x06
#define BCM540X_DSP_POWER_SAVER_LOWER_BOUND         0x07
#define BCM540X_DSP_LAST_ECHO                       0x08
#define BCM540X_DSP_FREQUENCY                       0x09
#define BCM540X_DSP_PLL_BANDWIDTH                   0x0a
#define BCM540X_DSP_PLL_PHASE_OFFSET                0x0b

#define BCM540X_DSP_FILTER_DCOFFSET                 (BIT_10 | BIT_11)
#define BCM540X_DSP_FILTER_FEXT3                    (BIT_8 | BIT_9 | BIT_11)
#define BCM540X_DSP_FILTER_FEXT2                    (BIT_9 | BIT_11)
#define BCM540X_DSP_FILTER_FEXT1                    (BIT_8 | BIT_11)
#define BCM540X_DSP_FILTER_FEXT0                    BIT_11
#define BCM540X_DSP_FILTER_NEXT3                    (BIT_8 | BIT_9 | BIT_10)
#define BCM540X_DSP_FILTER_NEXT2                    (BIT_9 | BIT_10)
#define BCM540X_DSP_FILTER_NEXT1                    (BIT_8 | BIT_10)
#define BCM540X_DSP_FILTER_NEXT0                    BIT_10
#define BCM540X_DSP_FILTER_ECHO                     (BIT_8 | BIT_9)
#define BCM540X_DSP_FILTER_DFE                      BIT_9
#define BCM540X_DSP_FILTER_FFE                      BIT_8

#define BCM540X_DSP_CONTROL_ALL_FILTERS             BIT_12

#define BCM540X_DSP_SEL_CH_0                        BIT_NONE
#define BCM540X_DSP_SEL_CH_1                        BIT_13
#define BCM540X_DSP_SEL_CH_2                        BIT_14
#define BCM540X_DSP_SEL_CH_3                        (BIT_13 | BIT_14)

#define BCM540X_CONTROL_ALL_CHANNELS                BIT_15


/* Auxilliary Control Register (Shadow Register) */
#define BCM5401_AUX_CTRL                            0x18

#define BCM5401_SHADOW_SEL_MASK                     0x7
#define BCM5401_SHADOW_SEL_NORMAL                   0x00
#define BCM5401_SHADOW_SEL_10BASET                  0x01
#define BCM5401_SHADOW_SEL_POWER_CONTROL            0x02
#define BCM5401_SHADOW_SEL_IP_PHONE                 0x03
#define BCM5401_SHADOW_SEL_MISC_TEST1               0x04
#define BCM5401_SHADOW_SEL_MISC_TEST2               0x05
#define BCM5401_SHADOW_SEL_IP_PHONE_SEED            0x06


/* Shadow register selector == '000' */
#define BCM5401_SHDW_NORMAL_DIAG_MODE               BIT_3
#define BCM5401_SHDW_NORMAL_DISABLE_MBP             BIT_4
#define BCM5401_SHDW_NORMAL_DISABLE_LOW_PWR         BIT_5
#define BCM5401_SHDW_NORMAL_DISABLE_INV_PRF         BIT_6
#define BCM5401_SHDW_NORMAL_DISABLE_PRF             BIT_7
#define BCM5401_SHDW_NORMAL_RX_SLICING_NORMAL       BIT_NONE
#define BCM5401_SHDW_NORMAL_RX_SLICING_4D           BIT_8
#define BCM5401_SHDW_NORMAL_RX_SLICING_3LVL_1D      BIT_9
#define BCM5401_SHDW_NORMAL_RX_SLICING_5LVL_1D      (BIT_8 | BIT_9)
#define BCM5401_SHDW_NORMAL_TX_6DB_CODING           BIT_10
#define BCM5401_SHDW_NORMAL_ENABLE_SM_DSP_CLOCK     BIT_11
#define BCM5401_SHDW_NORMAL_EDGERATE_CTRL_4NS       BIT_NONE
#define BCM5401_SHDW_NORMAL_EDGERATE_CTRL_5NS       BIT_12
#define BCM5401_SHDW_NORMAL_EDGERATE_CTRL_3NS       BIT_13
#define BCM5401_SHDW_NORMAL_EDGERATE_CTRL_0NS       (BIT_12 | BIT_13)
#define BCM5401_SHDW_NORMAL_EXT_PACKET_LENGTH       BIT_14
#define BCM5401_SHDW_NORMAL_EXTERNAL_LOOPBACK       BIT_15


/* Auxilliary status summary. */
#define BCM540X_AUX_STATUS_REG                      0x19

#define BCM540X_AUX_LINK_PASS                       BIT_2
#define BCM540X_AUX_SPEED_MASK                      (BIT_8 | BIT_9 | BIT_10)
#define BCM540X_AUX_10BASET_HD                      BIT_8
#define BCM540X_AUX_10BASET_FD                      BIT_9
#define BCM540X_AUX_100BASETX_HD                    (BIT_8 | BIT_9)
#define BCM540X_AUX_100BASET4                       BIT_10
#define BCM540X_AUX_100BASETX_FD                    (BIT_8 | BIT_10)
#define BCM540X_AUX_100BASET_HD                     (BIT_9 | BIT_10)
#define BCM540X_AUX_100BASET_FD                     (BIT_8 | BIT_9 | BIT_10)


/* Interrupt status. */
#define BCM540X_INT_STATUS_REG                      0x1a

#define BCM540X_INT_LINK_CHANGE                     BIT_1
#define BCM540X_INT_SPEED_CHANGE                    BIT_2
#define BCM540X_INT_DUPLEX_CHANGE                   BIT_3
#define BCM540X_INT_AUTO_NEG_PAGE_RX                BIT_10


/* Interrupt mask register. */
#define BCM540X_INT_MASK_REG                        0x1b

/* BCM5461 x1c Shadow Control register */
#define BCM546X_1c_SHADOW_REG                       0x1c

#define BCM546X_1c_WR_EN                            0x8000	/* shadow (1c) write enable bit mask */

#define BCM546X_1c_SPR_CTRL_1                       0x0800	/* shadow (1c) reg 00010 addr */
#define BCM546X_1c_SP1_LINK_LED                     0x0001	/* shadow (1c) reg 00010 link LED mode mask */

#define BCM546X_1c_SPR_CTRL_2                       0x100C	/* shadow (1c) reg 00100 addr  */
#define BCM546X_1c_SP2_NRG_DET                      0x0002	/* shadow (1c) reg 00100 energy detect bit mask */



/******************************************************************************/
/* Register definitions. */
/******************************************************************************/

typedef volatile LM_UINT8 T3_8BIT_REGISTER, *PT3_8BIT_REGISTER;
typedef volatile LM_UINT16 T3_16BIT_REGISTER, *PT3_16BIT_REGISTER;
typedef volatile LM_UINT32 T3_32BIT_REGISTER, *PT3_32BIT_REGISTER;

typedef struct {
    /* Big endian format. */
    T3_32BIT_REGISTER High;
    T3_32BIT_REGISTER Low;
} T3_64BIT_REGISTER, *PT3_64BIT_REGISTER;

typedef T3_64BIT_REGISTER T3_64BIT_HOST_ADDR, *PT3_64BIT_HOST_ADDR;

#define T3_NUM_OF_DMA_DESC    256
#define T3_NUM_OF_MBUF        768

typedef struct 
{
  T3_64BIT_REGISTER host_addr;
  T3_32BIT_REGISTER nic_mbuf;
  T3_16BIT_REGISTER len;
  T3_16BIT_REGISTER cqid_sqid;
  T3_32BIT_REGISTER flags;
  T3_32BIT_REGISTER opaque1;
  T3_32BIT_REGISTER opaque2;
  T3_32BIT_REGISTER opaque3;
}T3_DMA_DESC, *PT3_DMA_DESC;



/******************************************************************************/
/* Ring control block. */
/******************************************************************************/

typedef struct {
    T3_64BIT_REGISTER HostRingAddr;

    union {
        struct {
#ifdef BIG_ENDIAN_HOST
            T3_16BIT_REGISTER MaxLen;
            T3_16BIT_REGISTER Flags;
#else /* BIG_ENDIAN_HOST */
            T3_16BIT_REGISTER Flags;
            T3_16BIT_REGISTER MaxLen;
#endif
        } s;

        T3_32BIT_REGISTER MaxLen_Flags;
    } u;

    T3_32BIT_REGISTER NicRingAddr;
} T3_RCB, *PT3_RCB;

#define T3_RCB_FLAG_USE_EXT_RECV_BD                     BIT_0
#define T3_RCB_FLAG_RING_DISABLED                       BIT_1



/******************************************************************************/
/* Status block. */
/******************************************************************************/

/* 
 * Size of status block is actually 0x50 bytes.  Use 0x80 bytes for
 * cache line alignment. 
 */
#define T3_STATUS_BLOCK_SIZE                                    0x80

typedef struct {
    volatile LM_UINT32 Status;
    #define STATUS_BLOCK_UPDATED                                BIT_0
    #define STATUS_BLOCK_LINK_CHANGED_STATUS                    BIT_1
    #define STATUS_BLOCK_ERROR                                  BIT_2

    volatile LM_UINT32 StatusTag;

#ifdef BIG_ENDIAN_HOST
    volatile LM_UINT16 RcvStdConIdx;
    volatile LM_UINT16 RcvJumboConIdx;

    volatile LM_UINT16 Reserved2;
    volatile LM_UINT16 RcvMiniConIdx;

    struct {
        volatile LM_UINT16 SendConIdx;   /* Send consumer index. */
        volatile LM_UINT16 RcvProdIdx;   /* Receive producer index. */
    } Idx[16];
#else /* BIG_ENDIAN_HOST */
    volatile LM_UINT16 RcvJumboConIdx;
    volatile LM_UINT16 RcvStdConIdx;

    volatile LM_UINT16 RcvMiniConIdx;
    volatile LM_UINT16 Reserved2;

    struct {
        volatile LM_UINT16 RcvProdIdx;   /* Receive producer index. */
        volatile LM_UINT16 SendConIdx;   /* Send consumer index. */
    } Idx[16];
#endif
} T3_STATUS_BLOCK, *PT3_STATUS_BLOCK;



/******************************************************************************/
/* Receive buffer descriptors. */
/******************************************************************************/

typedef struct {
    T3_64BIT_HOST_ADDR HostAddr;

#ifdef BIG_ENDIAN_HOST
    volatile LM_UINT16 Index;
    volatile LM_UINT16 Len;

    volatile LM_UINT16 Type;
    volatile LM_UINT16 Flags;

    volatile LM_UINT16 IpCksum;
    volatile LM_UINT16 TcpUdpCksum;

    volatile LM_UINT16 ErrorFlag;
    volatile LM_UINT16 VlanTag;
#else /* BIG_ENDIAN_HOST */
    volatile LM_UINT16 Len;
    volatile LM_UINT16 Index;

    volatile LM_UINT16 Flags;
    volatile LM_UINT16 Type;

    volatile LM_UINT16 TcpUdpCksum;
    volatile LM_UINT16 IpCksum;

    volatile LM_UINT16 VlanTag;
    volatile LM_UINT16 ErrorFlag;
#endif

    volatile LM_UINT32 Reserved;
    volatile LM_UINT32 Opaque;
} T3_RCV_BD, *PT3_RCV_BD;


typedef struct {
    T3_64BIT_HOST_ADDR HostAddr[3];

#ifdef BIG_ENDIAN_HOST
    LM_UINT16 Len1;
    LM_UINT16 Len2;

    LM_UINT16 Len3;
    LM_UINT16 Reserved1;
#else /* BIG_ENDIAN_HOST */
    LM_UINT16 Len2;
    LM_UINT16 Len1;

    LM_UINT16 Reserved1;
    LM_UINT16 Len3;
#endif

    T3_RCV_BD StdRcvBd;
} T3_EXT_RCV_BD, *PT3_EXT_RCV_BD;


/* Error flags. */
#define RCV_BD_ERR_BAD_CRC                          0x0001
#define RCV_BD_ERR_COLL_DETECT                      0x0002
#define RCV_BD_ERR_LINK_LOST_DURING_PKT             0x0004
#define RCV_BD_ERR_PHY_DECODE_ERR                   0x0008
#define RCV_BD_ERR_ODD_NIBBLED_RCVD_MII             0x0010
#define RCV_BD_ERR_MAC_ABORT                        0x0020
#define RCV_BD_ERR_LEN_LT_64                        0x0040
#define RCV_BD_ERR_TRUNC_NO_RESOURCES               0x0080
#define RCV_BD_ERR_GIANT_FRAME_RCVD                 0x0100


/* Buffer descriptor flags. */
#define RCV_BD_FLAG_END                             0x0004
#define RCV_BD_FLAG_JUMBO_RING                      0x0020
#define RCV_BD_FLAG_VLAN_TAG                        0x0040
#define RCV_BD_FLAG_FRAME_HAS_ERROR                 0x0400
#define RCV_BD_FLAG_MINI_RING                       0x0800
#define RCV_BD_FLAG_IP_CHKSUM_FIELD                 0x1000
#define RCV_BD_FLAG_TCP_UDP_CHKSUM_FIELD            0x2000
#define RCV_BD_FLAG_TCP_PACKET                      0x4000



/******************************************************************************/
/* Send buffer descriptor. */
/******************************************************************************/

typedef struct {
    T3_64BIT_HOST_ADDR HostAddr;

    union {
        struct {
#ifdef BIG_ENDIAN_HOST
            LM_UINT16 Len;
            LM_UINT16 Flags;
#else /* BIG_ENDIAN_HOST */
            LM_UINT16 Flags;
            LM_UINT16 Len;
#endif
        } s1;

        LM_UINT32 Len_Flags;
    } u1;

    union {
        struct {
#ifdef BIG_ENDIAN_HOST
            LM_UINT16 Reserved;
            LM_UINT16 VlanTag;
#else /* BIG_ENDIAN_HOST */
            LM_UINT16 VlanTag;
            LM_UINT16 Reserved;
#endif
        } s2;

        LM_UINT32 VlanTag;
    } u2;
} T3_SND_BD, *PT3_SND_BD;


/* Send buffer descriptor flags. */
#define SND_BD_FLAG_TCP_UDP_CKSUM                   0x0001
#define SND_BD_FLAG_IP_CKSUM                        0x0002
#define SND_BD_FLAG_END                             0x0004
#define SND_BD_FLAG_IP_FRAG                         0x0008
#define SND_BD_FLAG_IP_FRAG_END                     0x0010
#define SND_BD_FLAG_VLAN_TAG                        0x0040
#define SND_BD_FLAG_COAL_NOW                        0x0080
#define SND_BD_FLAG_CPU_PRE_DMA                     0x0100
#define SND_BD_FLAG_CPU_POST_DMA                    0x0200
#define SND_BD_FLAG_INSERT_SRC_ADDR                 0x1000
#define SND_BD_FLAG_CHOOSE_SRC_ADDR                 0x6000
#define SND_BD_FLAG_DONT_GEN_CRC                    0x8000

/* MBUFs */
typedef struct T3_MBUF_FRAME_DESC {
#ifdef BIG_ENDIAN_HOST
  LM_UINT32 status_control;
  union {
    struct {
      LM_UINT8 cqid;
      LM_UINT8 reserved1;
      LM_UINT16 length;
    }s1;
    LM_UINT32 word;
  }u1;
  union {
    struct 
    {
      LM_UINT16 ip_hdr_start;
      LM_UINT16 tcp_udp_hdr_start;
    }s2;

    LM_UINT32 word;
  }u2;

  union {
    struct {
      LM_UINT16 data_start;
      LM_UINT16 vlan_id;
    }s3;
    
    LM_UINT32 word;
  }u3;

  union {
    struct {
      LM_UINT16 ip_checksum;
      LM_UINT16 tcp_udp_checksum;
    }s4;

    LM_UINT32 word;
  }u4;

  union {
    struct {
      LM_UINT16 pseudo_checksum;
      LM_UINT16 checksum_status;
    }s5;

    LM_UINT32 word;
  }u5;
  
  union {
    struct {
      LM_UINT16 rule_match;
      LM_UINT8 class;
      LM_UINT8 rupt;
    }s6;

    LM_UINT32 word;
  }u6;

  union {
    struct {
      LM_UINT16 reserved2;
      LM_UINT16 mbuf_num;
    }s7;

    LM_UINT32 word;
  }u7;

  LM_UINT32 reserved3;
  LM_UINT32 reserved4;
#else
  LM_UINT32 status_control;
  union {
    struct {
      LM_UINT16 length;
      LM_UINT8  reserved1;
      LM_UINT8  cqid;
    }s1;
    LM_UINT32 word;
  }u1;
  union {
    struct 
    {
      LM_UINT16 tcp_udp_hdr_start;
      LM_UINT16 ip_hdr_start;
    }s2;

    LM_UINT32 word;
  }u2;

  union {
    struct {
      LM_UINT16 vlan_id;
      LM_UINT16 data_start;
    }s3;
    
    LM_UINT32 word;
  }u3;

  union {
    struct {
      LM_UINT16 tcp_udp_checksum;
      LM_UINT16 ip_checksum;
    }s4;

    LM_UINT32 word;
  }u4;

  union {
    struct {
      LM_UINT16 checksum_status;
      LM_UINT16 pseudo_checksum;
    }s5;

    LM_UINT32 word;
  }u5;
  
  union {
    struct {
      LM_UINT8 rupt;
      LM_UINT8 class;
      LM_UINT16 rule_match;
    }s6;

    LM_UINT32 word;
  }u6;

  union {
    struct {
      LM_UINT16 mbuf_num;
      LM_UINT16 reserved2;
    }s7;

    LM_UINT32 word;
  }u7;

  LM_UINT32 reserved3;
  LM_UINT32 reserved4;
#endif
}T3_MBUF_FRAME_DESC,*PT3_MBUF_FRAME_DESC;

typedef struct T3_MBUF_HDR {
  union {
    struct {
      unsigned int C:1;
      unsigned int F:1;
      unsigned int reserved1:7;
      unsigned int next_mbuf:16;
      unsigned int length:7;
    }s1;
    
    LM_UINT32 word;
  }u1;
  
  LM_UINT32 next_frame_ptr;
}T3_MBUF_HDR, *PT3_MBUF_HDR;

typedef struct T3_MBUF
{
  T3_MBUF_HDR hdr;
  union
  {
    struct {
      T3_MBUF_FRAME_DESC frame_hdr;
      LM_UINT32 data[20];
    }s1;

    struct {
      LM_UINT32 data[30];
    }s2;
  }body;
}T3_MBUF, *PT3_MBUF;

#define T3_MBUF_BASE   (T3_NIC_MBUF_POOL_ADDR >> 7)
#define T3_MBUF_END    ((T3_NIC_MBUF_POOL_ADDR + T3_NIC_MBUF_POOL_SIZE) >> 7)



/******************************************************************************/
/* Statistics block. */
/******************************************************************************/

typedef struct {
    LM_UINT8 Reserved0[0x400-0x300];

    /* Statistics maintained by Receive MAC. */
    T3_64BIT_REGISTER ifHCInOctets;
    T3_64BIT_REGISTER Reserved1;
    T3_64BIT_REGISTER etherStatsFragments;
    T3_64BIT_REGISTER ifHCInUcastPkts;
    T3_64BIT_REGISTER ifHCInMulticastPkts;
    T3_64BIT_REGISTER ifHCInBroadcastPkts;
    T3_64BIT_REGISTER dot3StatsFCSErrors;
    T3_64BIT_REGISTER dot3StatsAlignmentErrors;
    T3_64BIT_REGISTER xonPauseFramesReceived;
    T3_64BIT_REGISTER xoffPauseFramesReceived;
    T3_64BIT_REGISTER macControlFramesReceived;
    T3_64BIT_REGISTER xoffStateEntered;
    T3_64BIT_REGISTER dot3StatsFramesTooLong;
    T3_64BIT_REGISTER etherStatsJabbers;
    T3_64BIT_REGISTER etherStatsUndersizePkts;
    T3_64BIT_REGISTER inRangeLengthError;
    T3_64BIT_REGISTER outRangeLengthError;
    T3_64BIT_REGISTER etherStatsPkts64Octets;
    T3_64BIT_REGISTER etherStatsPkts65Octetsto127Octets;
    T3_64BIT_REGISTER etherStatsPkts128Octetsto255Octets;
    T3_64BIT_REGISTER etherStatsPkts256Octetsto511Octets;
    T3_64BIT_REGISTER etherStatsPkts512Octetsto1023Octets;
    T3_64BIT_REGISTER etherStatsPkts1024Octetsto1522Octets;
    T3_64BIT_REGISTER etherStatsPkts1523Octetsto2047Octets;
    T3_64BIT_REGISTER etherStatsPkts2048Octetsto4095Octets;
    T3_64BIT_REGISTER etherStatsPkts4096Octetsto8191Octets;
    T3_64BIT_REGISTER etherStatsPkts8192Octetsto9022Octets;

    T3_64BIT_REGISTER Unused1[37];

    /* Statistics maintained by Transmit MAC. */
    T3_64BIT_REGISTER ifHCOutOctets;
    T3_64BIT_REGISTER Reserved2;
    T3_64BIT_REGISTER etherStatsCollisions;
    T3_64BIT_REGISTER outXonSent;
    T3_64BIT_REGISTER outXoffSent;
    T3_64BIT_REGISTER flowControlDone;
    T3_64BIT_REGISTER dot3StatsInternalMacTransmitErrors;
    T3_64BIT_REGISTER dot3StatsSingleCollisionFrames;
    T3_64BIT_REGISTER dot3StatsMultipleCollisionFrames;
    T3_64BIT_REGISTER dot3StatsDeferredTransmissions;
    T3_64BIT_REGISTER Reserved3;
    T3_64BIT_REGISTER dot3StatsExcessiveCollisions;
    T3_64BIT_REGISTER dot3StatsLateCollisions;
    T3_64BIT_REGISTER dot3Collided2Times;
    T3_64BIT_REGISTER dot3Collided3Times;
    T3_64BIT_REGISTER dot3Collided4Times;
    T3_64BIT_REGISTER dot3Collided5Times;
    T3_64BIT_REGISTER dot3Collided6Times;
    T3_64BIT_REGISTER dot3Collided7Times;
    T3_64BIT_REGISTER dot3Collided8Times;
    T3_64BIT_REGISTER dot3Collided9Times;
    T3_64BIT_REGISTER dot3Collided10Times;
    T3_64BIT_REGISTER dot3Collided11Times;
    T3_64BIT_REGISTER dot3Collided12Times;
    T3_64BIT_REGISTER dot3Collided13Times;
    T3_64BIT_REGISTER dot3Collided14Times;
    T3_64BIT_REGISTER dot3Collided15Times;
    T3_64BIT_REGISTER ifHCOutUcastPkts;
    T3_64BIT_REGISTER ifHCOutMulticastPkts;
    T3_64BIT_REGISTER ifHCOutBroadcastPkts;
    T3_64BIT_REGISTER dot3StatsCarrierSenseErrors;
    T3_64BIT_REGISTER ifOutDiscards;
    T3_64BIT_REGISTER ifOutErrors;

    T3_64BIT_REGISTER Unused2[31];

    /* Statistics maintained by Receive List Placement. */
    T3_64BIT_REGISTER COSIfHCInPkts[16];
    T3_64BIT_REGISTER COSFramesDroppedDueToFilters;
    T3_64BIT_REGISTER nicDmaWriteQueueFull;
    T3_64BIT_REGISTER nicDmaWriteHighPriQueueFull;
    T3_64BIT_REGISTER nicNoMoreRxBDs;
    T3_64BIT_REGISTER ifInDiscards;
    T3_64BIT_REGISTER ifInErrors;
    T3_64BIT_REGISTER nicRecvThresholdHit;

    T3_64BIT_REGISTER Unused3[9];

    /* Statistics maintained by Send Data Initiator. */
    T3_64BIT_REGISTER COSIfHCOutPkts[16];
    T3_64BIT_REGISTER nicDmaReadQueueFull;
    T3_64BIT_REGISTER nicDmaReadHighPriQueueFull;
    T3_64BIT_REGISTER nicSendDataCompQueueFull;

    /* Statistics maintained by Host Coalescing. */
    T3_64BIT_REGISTER nicRingSetSendProdIndex;
    T3_64BIT_REGISTER nicRingStatusUpdate;
    T3_64BIT_REGISTER nicInterrupts;
    T3_64BIT_REGISTER nicAvoidedInterrupts;
    T3_64BIT_REGISTER nicSendThresholdHit;

    LM_UINT8 Reserved4[0xb00-0x9c0];
} T3_STATS_BLOCK, *PT3_STATS_BLOCK;



/******************************************************************************/
/* PCI configuration registers. */
/******************************************************************************/

typedef struct {
    T3_16BIT_REGISTER VendorId;
    T3_16BIT_REGISTER DeviceId;

    T3_16BIT_REGISTER Command;
    T3_16BIT_REGISTER Status;

    T3_32BIT_REGISTER ClassCodeRevId;

    T3_8BIT_REGISTER CacheLineSize;
    T3_8BIT_REGISTER LatencyTimer;
    T3_8BIT_REGISTER HeaderType;
    T3_8BIT_REGISTER Bist;

    T3_32BIT_REGISTER MemBaseAddrLow;
    T3_32BIT_REGISTER MemBaseAddrHigh;

    LM_UINT8 Unused1[20];

    T3_16BIT_REGISTER SubsystemVendorId;
    T3_16BIT_REGISTER SubsystemId;

    T3_32BIT_REGISTER RomBaseAddr;

    T3_8BIT_REGISTER PciXCapiblityPtr;
    LM_UINT8 Unused2[7];

    T3_8BIT_REGISTER IntLine;
    T3_8BIT_REGISTER IntPin;
    T3_8BIT_REGISTER MinGnt;
    T3_8BIT_REGISTER MaxLat;

    T3_8BIT_REGISTER PciXCapabilities;
    T3_8BIT_REGISTER PmCapabilityPtr;
    T3_16BIT_REGISTER PciXCommand;
    #define PXC_MAX_READ_BYTE_COUNT_MASK		(BIT_3 | BIT_2)
    #define PXC_MAX_READ_BYTE_COUNT_512			(0)
    #define PXC_MAX_READ_BYTE_COUNT_1024		(BIT_2)
    #define PXC_MAX_READ_BYTE_COUNT_2048		(BIT_3)
    #define PXC_MAX_READ_BYTE_COUNT_4096		(BIT_3 | BIT_2)

    T3_32BIT_REGISTER PciXStatus;

    T3_8BIT_REGISTER PmCapabilityId;
    T3_8BIT_REGISTER VpdCapabilityPtr;
    T3_16BIT_REGISTER PmCapabilities;

    T3_16BIT_REGISTER PmCtrlStatus;
    #define PM_CTRL_PME_STATUS            BIT_15
    #define PM_CTRL_PME_ENABLE            BIT_8
    #define PM_CTRL_PME_POWER_STATE_D0    0
    #define PM_CTRL_PME_POWER_STATE_D1    1
    #define PM_CTRL_PME_POWER_STATE_D2    2
    #define PM_CTRL_PME_POWER_STATE_D3H   3

    T3_8BIT_REGISTER BridgeSupportExt;
    T3_8BIT_REGISTER PmData;

    T3_8BIT_REGISTER VpdCapabilityId;
    T3_8BIT_REGISTER MsiCapabilityPtr;
    T3_16BIT_REGISTER VpdAddrFlag;
    #define VPD_FLAG_WRITE      (1 << 15)
    #define VPD_FLAG_RW_MASK    (1 << 15)
    #define VPD_FLAG_READ       0


    T3_32BIT_REGISTER VpdData;

    T3_8BIT_REGISTER MsiCapabilityId;
    T3_8BIT_REGISTER NextCapabilityPtr;
    T3_16BIT_REGISTER MsiCtrl;
    #define MSI_CTRL_64BIT_CAP     (1 << 7)
    #define MSI_CTRL_MSG_ENABLE(x) (x << 4)
    #define MSI_CTRL_MSG_CAP(x)    (x << 1)
    #define MSI_CTRL_ENABLE        (1 << 0)
  

    T3_32BIT_REGISTER MsiAddrLow;
    T3_32BIT_REGISTER MsiAddrHigh;

    T3_16BIT_REGISTER MsiData;
    T3_16BIT_REGISTER Unused3;

    T3_32BIT_REGISTER MiscHostCtrl;
    #define MISC_HOST_CTRL_CLEAR_INT                        BIT_0
    #define MISC_HOST_CTRL_MASK_PCI_INT                     BIT_1
    #define MISC_HOST_CTRL_ENABLE_ENDIAN_BYTE_SWAP          BIT_2
    #define MISC_HOST_CTRL_ENABLE_ENDIAN_WORD_SWAP          BIT_3
    #define MISC_HOST_CTRL_ENABLE_PCI_STATE_REG_RW          BIT_4
    #define MISC_HOST_CTRL_ENABLE_CLK_REG_RW                BIT_5
    #define MISC_HOST_CTRL_ENABLE_REG_WORD_SWAP             BIT_6
    #define MISC_HOST_CTRL_ENABLE_INDIRECT_ACCESS           BIT_7
    #define MISC_HOST_CTRL_ENABLE_INT_MASK_MODE             BIT_8
    #define MISC_HOST_CTRL_ENABLE_TAGGED_STATUS_MODE        BIT_9

    T3_32BIT_REGISTER DmaReadWriteCtrl;
    #define DMA_CTRL_WRITE_CMD                      0x70000000
    #define DMA_CTRL_WRITE_BOUNDARY_64_PCIE         0x10000000
    #define DMA_CTRL_WRITE_BOUNDARY_128_PCIE        0x30000000
    #define DMA_CTRL_WRITE_BOUNDARY_DISABLE_PCIE    0x70000000
    #define DMA_CTRL_READ_CMD                       0x06000000

    /* bits 21:19 */
    #define DMA_CTRL_WRITE_PCIE_H20MARK_128         0x00180000 
    #define DMA_CTRL_WRITE_PCIE_H20MARK_256         0x00380000

    #define DMA_CTRL_PCIX_READ_WATERMARK_MASK       (BIT_18 | BIT_17 | BIT_16)
    #define DMA_CTRL_PCIX_READ_WATERMARK_64         (0)
    #define DMA_CTRL_PCIX_READ_WATERMARK_128        (BIT_16)
    #define DMA_CTRL_PCIX_READ_WATERMARK_256        (BIT_17)
    #define DMA_CTRL_PCIX_READ_WATERMARK_384        (BIT_17 | BIT_16)
    #define DMA_CTRL_PCIX_READ_WATERMARK_512        (BIT_18)
    #define DMA_CTRL_PCIX_READ_WATERMARK_1024       (BIT_18 | BIT_16)
    #define DMA_CTRL_PCIX_READ_WATERMARK_1536X      (BIT_18 | BIT_17)
    #define DMA_CTRL_PCIX_READ_WATERMARK_1536       (BIT_18 | BIT_17 | BIT_16)

    #define DMA_CTRL_WRITE_BOUNDARY_MASK            (BIT_11 | BIT_12 | BIT_13)
    #define DMA_CTRL_WRITE_BOUNDARY_DISABLE         0
    #define DMA_CTRL_WRITE_BOUNDARY_16              BIT_11
    #define DMA_CTRL_WRITE_BOUNDARY_128_PCIX        BIT_11
    #define DMA_CTRL_WRITE_BOUNDARY_32              BIT_12
    #define DMA_CTRL_WRITE_BOUNDARY_256_PCIX        BIT_12
    #define DMA_CTRL_WRITE_BOUNDARY_64              (BIT_12 | BIT_11)
    #define DMA_CTRL_WRITE_BOUNDARY_384_PCIX        (BIT_12 | BIT_11)
    #define DMA_CTRL_WRITE_BOUNDARY_128             BIT_13
    #define DMA_CTRL_WRITE_BOUNDARY_256             (BIT_13 | BIT_11)
    #define DMA_CTRL_WRITE_BOUNDARY_512             (BIT_13 | BIT_12)
    #define DMA_CTRL_WRITE_BOUNDARY_1024            (BIT_13 | BIT_12 | BIT_11)
    #define DMA_CTRL_WRITE_ONE_DMA_AT_ONCE          BIT_14

    #define DMA_CTRL_READ_BOUNDARY_MASK             (BIT_10 | BIT_9 | BIT_8)
    #define DMA_CTRL_READ_BOUNDARY_DISABLE          0
    #define DMA_CTRL_READ_BOUNDARY_16               BIT_8
    #define DMA_CTRL_READ_BOUNDARY_128_PCIX         BIT_8
    #define DMA_CTRL_READ_BOUNDARY_32               BIT_9
    #define DMA_CTRL_READ_BOUNDARY_256_PCIX         BIT_9
    #define DMA_CTRL_READ_BOUNDARY_64               (BIT_9 | BIT_8)
    #define DMA_CTRL_READ_BOUNDARY_384_PCIX         (BIT_9 | BIT_8)
    #define DMA_CTRL_READ_BOUNDARY_128              BIT_10
    #define DMA_CTRL_READ_BOUNDARY_256              (BIT_10 | BIT_8)
    #define DMA_CTRL_READ_BOUNDARY_512              (BIT_10 | BIT_9)
    #define DMA_CTRL_READ_BOUNDARY_1024             (BIT_10 | BIT_9 | BIT_8)

    T3_32BIT_REGISTER PciState;
    #define T3_PCI_STATE_FORCE_PCI_RESET                    BIT_0
    #define T3_PCI_STATE_INTERRUPT_NOT_ACTIVE               BIT_1
    #define T3_PCI_STATE_NOT_PCI_X_BUS                      BIT_2
    #define T3_PCI_STATE_HIGH_BUS_SPEED                     BIT_3
    #define T3_PCI_STATE_32BIT_PCI_BUS                      BIT_4
    #define T3_PCI_STATE_PCI_ROM_ENABLE                     BIT_5
    #define T3_PCI_STATE_PCI_ROM_RETRY_ENABLE               BIT_6
    #define T3_PCI_STATE_FLAT_VIEW                          BIT_8
    #define T3_PCI_STATE_RETRY_SAME_DMA                     BIT_13

    T3_32BIT_REGISTER ClockCtrl;
    #define T3_PCI_CLKCTRL_TXCPU_CLK_DISABLE                BIT_11
    #define T3_PCI_CLKCTRL_RXCPU_CLK_DISABLE                BIT_10
    #define T3_PCI_CLKCTRL_CORE_CLK_DISABLE                 BIT_9

    T3_32BIT_REGISTER RegBaseAddr;

    T3_32BIT_REGISTER MemWindowBaseAddr;
    
#ifdef NIC_CPU_VIEW
  /* These registers are ONLY visible to NIC CPU */
    T3_32BIT_REGISTER PowerConsumed;
    T3_32BIT_REGISTER PowerDissipated;
#else /* NIC_CPU_VIEW */
    T3_32BIT_REGISTER RegData;
    T3_32BIT_REGISTER MemWindowData;
#endif /* !NIC_CPU_VIEW */

    T3_32BIT_REGISTER ModeCtrl;

    T3_32BIT_REGISTER MiscCfg;

    T3_32BIT_REGISTER MiscLocalCtrl;

    T3_32BIT_REGISTER Unused4;

    /* NOTE: Big/Little-endian clarification needed.  Are these register */
    /* in big or little endian formate. */
    T3_64BIT_REGISTER StdRingProdIdx;
    T3_64BIT_REGISTER RcvRetRingConIdx;
    T3_64BIT_REGISTER SndProdIdx;

    T3_32BIT_REGISTER Unused5[2];			/* 0xb0-0xb7 */

    T3_32BIT_REGISTER DualMacCtrl;			/* 0xb8 */
    #define T3_DUAL_MAC_CH_CTRL_MASK     (BIT_1 | BIT_0)
    #define T3_DUAL_MAC_ID               BIT_2

    T3_32BIT_REGISTER MacMessageExchangeOutput;		/*  0xbc  */
    T3_32BIT_REGISTER MacMessageExchangeInput;		/*  0xc0  */

    T3_32BIT_REGISTER FunctionEventMask;		/*  0xc4  */

    T3_32BIT_REGISTER Unused6[4];			/*  0xc8-0xd7  */

    T3_32BIT_REGISTER DeviceCtrl;			/*  0xd8  */
    #define MAX_PAYLOAD_SIZE_MASK			0x0e0

    LM_UINT8 Unused7[36];

} T3_PCI_CONFIGURATION, *PT3_PCI_CONFIGURATION;

#define PCIX_CMD_MAX_SPLIT_MASK                         0x00700000
#define PCIX_CMD_MAX_SPLIT_SHL                          20
#define PCIX_CMD_MAX_BURST_MASK                         0x000c0000
#define PCIX_CMD_MAX_BURST_SHL                          18
#define PCIX_CMD_MAX_BURST_CPIOB                        2

/******************************************************************************/
/* Mac control registers. */
/******************************************************************************/

typedef struct {
    /* MAC mode control. */
    T3_32BIT_REGISTER Mode;
    #define MAC_MODE_GLOBAL_RESET                       BIT_0
    #define MAC_MODE_HALF_DUPLEX                        BIT_1
    #define MAC_MODE_PORT_MODE_MASK                     (BIT_2 | BIT_3)
    #define MAC_MODE_PORT_MODE_TBI                      (BIT_2 | BIT_3)
    #define MAC_MODE_PORT_MODE_GMII                     BIT_3
    #define MAC_MODE_PORT_MODE_MII                      BIT_2
    #define MAC_MODE_PORT_MODE_NONE                     BIT_NONE
    #define MAC_MODE_PORT_INTERNAL_LOOPBACK             BIT_4
    #define MAC_MODE_TAGGED_MAC_CONTROL                 BIT_7
    #define MAC_MODE_TX_BURSTING                        BIT_8
    #define MAC_MODE_MAX_DEFER                          BIT_9
    #define MAC_MODE_LINK_POLARITY                      BIT_10
    #define MAC_MODE_ENABLE_RX_STATISTICS               BIT_11
    #define MAC_MODE_CLEAR_RX_STATISTICS                BIT_12
    #define MAC_MODE_FLUSH_RX_STATISTICS                BIT_13
    #define MAC_MODE_ENABLE_TX_STATISTICS               BIT_14
    #define MAC_MODE_CLEAR_TX_STATISTICS                BIT_15
    #define MAC_MODE_FLUSH_TX_STATISTICS                BIT_16
    #define MAC_MODE_SEND_CONFIGS                       BIT_17
    #define MAC_MODE_DETECT_MAGIC_PACKET_ENABLE         BIT_18
    #define MAC_MODE_ACPI_POWER_ON_ENABLE               BIT_19
    #define MAC_MODE_ENABLE_MIP                         BIT_20
    #define MAC_MODE_ENABLE_TDE                         BIT_21
    #define MAC_MODE_ENABLE_RDE                         BIT_22
    #define MAC_MODE_ENABLE_FHDE                        BIT_23

    /* MAC status */
    T3_32BIT_REGISTER Status;
    #define MAC_STATUS_PCS_SYNCED                       BIT_0
    #define MAC_STATUS_SIGNAL_DETECTED                  BIT_1
    #define MAC_STATUS_RECEIVING_CFG                    BIT_2
    #define MAC_STATUS_CFG_CHANGED                      BIT_3
    #define MAC_STATUS_SYNC_CHANGED                     BIT_4
    #define MAC_STATUS_PORT_DECODE_ERROR                BIT_10
    #define MAC_STATUS_LINK_STATE_CHANGED               BIT_12
    #define MAC_STATUS_MI_COMPLETION                    BIT_22
    #define MAC_STATUS_MI_INTERRUPT                     BIT_23
    #define MAC_STATUS_AP_ERROR                         BIT_24
    #define MAC_STATUS_ODI_ERROR                        BIT_25
    #define MAC_STATUS_RX_STATS_OVERRUN                 BIT_26
    #define MAC_STATUS_TX_STATS_OVERRUN                 BIT_27

    /* Event Enable */
    T3_32BIT_REGISTER MacEvent;
    #define MAC_EVENT_ENABLE_PORT_DECODE_ERR            BIT_10
    #define MAC_EVENT_ENABLE_LINK_STATE_CHANGED_ATTN    BIT_12
    #define MAC_EVENT_ENABLE_MI_COMPLETION              BIT_22
    #define MAC_EVENT_ENABLE_MI_INTERRUPT               BIT_23
    #define MAC_EVENT_ENABLE_AP_ERROR                   BIT_24
    #define MAC_EVENT_ENABLE_ODI_ERROR                  BIT_25
    #define MAC_EVENT_ENABLE_RX_STATS_OVERRUN           BIT_26
    #define MAC_EVENT_ENABLE_TX_STATS_OVERRUN           BIT_27

    /* Led control. */
    T3_32BIT_REGISTER LedCtrl;
    #define LED_CTRL_OVERRIDE_LINK_LED                  BIT_0
    #define LED_CTRL_1000MBPS_LED_ON                    BIT_1
    #define LED_CTRL_100MBPS_LED_ON                     BIT_2
    #define LED_CTRL_10MBPS_LED_ON                      BIT_3
    #define LED_CTRL_OVERRIDE_TRAFFIC_LED               BIT_4
    #define LED_CTRL_BLINK_TRAFFIC_LED                  BIT_5
    #define LED_CTRL_TRAFFIC_LED                        BIT_6
    #define LED_CTRL_1000MBPS_LED_STATUS                BIT_7
    #define LED_CTRL_100MBPS_LED_STATUS                 BIT_8
    #define LED_CTRL_10MBPS_LED_STATUS                  BIT_9
    #define LED_CTRL_TRAFFIC_LED_STATUS                 BIT_10
    #define LED_CTRL_MAC_MODE                           BIT_NONE
    #define LED_CTRL_PHY_MODE_1                         BIT_11
    #define LED_CTRL_PHY_MODE_2                         BIT_12
    #define LED_CTRL_SHASTA_MAC_MODE                    BIT_13
    #define LED_CTRL_SHARED_TRAFFIC_LINK                BIT_14
    #define LED_CTRL_WIRELESS_COMBO                     BIT_15
    #define LED_CTRL_BLINK_RATE_MASK                    0x7ff80000
    #define LED_CTRL_OVERRIDE_BLINK_PERIOD              BIT_19
    #define LED_CTRL_OVERRIDE_BLINK_RATE                BIT_31

    /* MAC addresses. */
    struct {
        T3_32BIT_REGISTER High;             /* Upper 2 bytes. */
        T3_32BIT_REGISTER Low;              /* Lower 4 bytes. */
    } MacAddr[4];

    /* ACPI Mbuf pointer. */
    T3_32BIT_REGISTER AcpiMbufPtr;

    /* ACPI Length and Offset. */
    T3_32BIT_REGISTER AcpiLengthOffset;
    #define ACPI_LENGTH_MASK                            0xffff
    #define ACPI_OFFSET_MASK                            0x0fff0000
    #define ACPI_LENGTH(x)                              x
    #define ACPI_OFFSET(x)                              ((x) << 16)

    /* Transmit random backoff. */
    T3_32BIT_REGISTER TxBackoffSeed;
    #define MAC_TX_BACKOFF_SEED_MASK                    0x3ff

    /* Receive MTU */
    T3_32BIT_REGISTER MtuSize;
    #define MAC_RX_MTU_MASK                             0xffff

    /* Gigabit PCS Test. */
    T3_32BIT_REGISTER PcsTest;
    #define MAC_PCS_TEST_DATA_PATTERN_MASK              0x0fffff
    #define MAC_PCS_TEST_ENABLE                         BIT_20

    /* Transmit Gigabit Auto-Negotiation. */
    T3_32BIT_REGISTER TxAutoNeg;
    #define MAC_AN_TX_AN_DATA_MASK                      0xffff

    /* Receive Gigabit Auto-Negotiation. */
    T3_32BIT_REGISTER RxAutoNeg;
    #define MAC_AN_RX_AN_DATA_MASK                      0xffff

    /* MI Communication. */
    T3_32BIT_REGISTER MiCom;
    #define MI_COM_CMD_MASK                             (BIT_26 | BIT_27)
    #define MI_COM_CMD_WRITE                            BIT_26
    #define MI_COM_CMD_READ                             BIT_27
    #define MI_COM_READ_FAILED                          BIT_28
    #define MI_COM_START                                BIT_29
    #define MI_COM_BUSY                                 BIT_29

    #define MI_COM_PHY_ADDR_MASK                        0x1f
    #define MI_COM_FIRST_PHY_ADDR_BIT                   21

    #define MI_COM_PHY_REG_ADDR_MASK                    0x1f
    #define MI_COM_FIRST_PHY_REG_ADDR_BIT               16

    #define MI_COM_PHY_DATA_MASK                        0xffff

    /* MI Status. */
    T3_32BIT_REGISTER MiStatus;
    #define MI_STATUS_ENABLE_LINK_STATUS_ATTN           BIT_0
    #define MI_STATUS_10MBPS                            BIT_1

    /* MI Mode. */
    T3_32BIT_REGISTER MiMode;
    #define MI_MODE_CLOCK_SPEED_10MHZ                   BIT_0
    #define MI_MODE_USE_SHORT_PREAMBLE                  BIT_1
    #define MI_MODE_AUTO_POLLING_ENABLE                 BIT_4
    #define MI_MODE_CORE_CLOCK_SPEED_62MHZ              BIT_15

    /* Auto-polling status. */
    T3_32BIT_REGISTER AutoPollStatus;
    #define AUTO_POLL_ERROR                             BIT_0

    /* Transmit MAC mode. */
    T3_32BIT_REGISTER TxMode;
    #define TX_MODE_RESET                               BIT_0
    #define TX_MODE_ENABLE                              BIT_1
    #define TX_MODE_ENABLE_FLOW_CONTROL                 BIT_4
    #define TX_MODE_ENABLE_BIG_BACKOFF                  BIT_5
    #define TX_MODE_ENABLE_LONG_PAUSE                   BIT_6

    /* Transmit MAC status. */
    T3_32BIT_REGISTER TxStatus;
    #define TX_STATUS_RX_CURRENTLY_XOFFED               BIT_0
    #define TX_STATUS_SENT_XOFF                         BIT_1
    #define TX_STATUS_SENT_XON                          BIT_2
    #define TX_STATUS_LINK_UP                           BIT_3
    #define TX_STATUS_ODI_UNDERRUN                      BIT_4
    #define TX_STATUS_ODI_OVERRUN                       BIT_5

    /* Transmit MAC length. */
    T3_32BIT_REGISTER TxLengths;
    #define TX_LEN_SLOT_TIME_MASK                       0xff
    #define TX_LEN_IPG_MASK                             0x0f00
    #define TX_LEN_IPG_CRS_MASK                         (BIT_12 | BIT_13)

    /* Receive MAC mode. */
    T3_32BIT_REGISTER RxMode;
    #define RX_MODE_RESET                               BIT_0
    #define RX_MODE_ENABLE                              BIT_1
    #define RX_MODE_ENABLE_FLOW_CONTROL                 BIT_2
    #define RX_MODE_KEEP_MAC_CONTROL                    BIT_3
    #define RX_MODE_KEEP_PAUSE                          BIT_4
    #define RX_MODE_ACCEPT_OVERSIZED                    BIT_5
    #define RX_MODE_ACCEPT_RUNTS                        BIT_6
    #define RX_MODE_LENGTH_CHECK                        BIT_7
    #define RX_MODE_PROMISCUOUS_MODE                    BIT_8
    #define RX_MODE_NO_CRC_CHECK                        BIT_9
    #define RX_MODE_KEEP_VLAN_TAG                       BIT_10

    /* Receive MAC status. */
    T3_32BIT_REGISTER RxStatus;
    #define RX_STATUS_REMOTE_TRANSMITTER_XOFFED         BIT_0
    #define RX_STATUS_XOFF_RECEIVED                     BIT_1
    #define RX_STATUS_XON_RECEIVED                      BIT_2

    /* Hash registers. */
    T3_32BIT_REGISTER HashReg[4];

    /* Receive placement rules registers. */
    struct {
        T3_32BIT_REGISTER Rule;
        T3_32BIT_REGISTER Value;
    } RcvRules[16];

    #define RCV_DISABLE_RULE_MASK                       0x7fffffff

    #define RCV_RULE1_REJECT_BROADCAST_IDX              0x00
    #define REJECT_BROADCAST_RULE1_RULE                 0xc2000000
    #define REJECT_BROADCAST_RULE1_VALUE                0xffffffff

    #define RCV_RULE2_REJECT_BROADCAST_IDX              0x01
    #define REJECT_BROADCAST_RULE2_RULE                 0x86000004
    #define REJECT_BROADCAST_RULE2_VALUE                0xffffffff

#if INCLUDE_5701_AX_FIX
    #define RCV_LAST_RULE_IDX                           0x04
#else
    #define RCV_LAST_RULE_IDX                           0x02
#endif

    T3_32BIT_REGISTER RcvRuleCfg;
    #define RX_RULE_DEFAULT_CLASS                       (1 << 3)

    T3_32BIT_REGISTER LowWaterMarkMaxRxFrame;

    LM_UINT8 Reserved1[24];

    T3_32BIT_REGISTER HashRegU[4];

    struct {
        T3_32BIT_REGISTER High;
        T3_32BIT_REGISTER Low;
    } MacAddrExt[12];

    T3_32BIT_REGISTER SerdesCfg;
    T3_32BIT_REGISTER SerdesStatus;

    LM_UINT8 Reserved2[24];

    T3_32BIT_REGISTER SgDigControl;
    T3_32BIT_REGISTER SgDigStatus;

    LM_UINT8 Reserved3[72];

    volatile LM_UINT8 TxMacState[16];
    volatile LM_UINT8 RxMacState[20];

    LM_UINT8 Reserved4[476];

    T3_32BIT_REGISTER ifHCOutOctets;
    T3_32BIT_REGISTER Reserved5;
    T3_32BIT_REGISTER etherStatsCollisions;
    T3_32BIT_REGISTER outXonSent;
    T3_32BIT_REGISTER outXoffSent;
    T3_32BIT_REGISTER Reserved6;
    T3_32BIT_REGISTER dot3StatsInternalMacTransmitErrors;
    T3_32BIT_REGISTER dot3StatsSingleCollisionFrames;
    T3_32BIT_REGISTER dot3StatsMultipleCollisionFrames;
    T3_32BIT_REGISTER dot3StatsDeferredTransmissions;
    T3_32BIT_REGISTER Reserved7;
    T3_32BIT_REGISTER dot3StatsExcessiveCollisions;
    T3_32BIT_REGISTER dot3StatsLateCollisions;
    T3_32BIT_REGISTER Reserved8[14];
    T3_32BIT_REGISTER ifHCOutUcastPkts;
    T3_32BIT_REGISTER ifHCOutMulticastPkts;
    T3_32BIT_REGISTER ifHCOutBroadcastPkts;
    T3_32BIT_REGISTER Reserved9[2];
    T3_32BIT_REGISTER ifHCInOctets;
    T3_32BIT_REGISTER Reserved10;
    T3_32BIT_REGISTER etherStatsFragments;
    T3_32BIT_REGISTER ifHCInUcastPkts;
    T3_32BIT_REGISTER ifHCInMulticastPkts;
    T3_32BIT_REGISTER ifHCInBroadcastPkts;
    T3_32BIT_REGISTER dot3StatsFCSErrors;
    T3_32BIT_REGISTER dot3StatsAlignmentErrors;
    T3_32BIT_REGISTER xonPauseFramesReceived;
    T3_32BIT_REGISTER xoffPauseFramesReceived;
    T3_32BIT_REGISTER macControlFramesReceived;
    T3_32BIT_REGISTER xoffStateEntered;
    T3_32BIT_REGISTER dot3StatsFramesTooLong;
    T3_32BIT_REGISTER etherStatsJabbers;
    T3_32BIT_REGISTER etherStatsUndersizePkts;

    T3_32BIT_REGISTER Reserved11[209];

} T3_MAC_CONTROL, *PT3_MAC_CONTROL;



/******************************************************************************/
/* Send data initiator control registers. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
    #define T3_SND_DATA_IN_MODE_RESET                       BIT_0
    #define T3_SND_DATA_IN_MODE_ENABLE                      BIT_1
    #define T3_SND_DATA_IN_MODE_STATS_OFLW_ATTN_ENABLE      BIT_2

    T3_32BIT_REGISTER Status;
    #define T3_SND_DATA_IN_STATUS_STATS_OFLW_ATTN           BIT_2

    T3_32BIT_REGISTER StatsCtrl;
    #define T3_SND_DATA_IN_STATS_CTRL_ENABLE                BIT_0
    #define T3_SND_DATA_IN_STATS_CTRL_FASTER_UPDATE         BIT_1
    #define T3_SND_DATA_IN_STATS_CTRL_CLEAR                 BIT_2
    #define T3_SND_DATA_IN_STATS_CTRL_FLUSH                 BIT_3
    #define T3_SND_DATA_IN_STATS_CTRL_FORCE_ZERO            BIT_4

    T3_32BIT_REGISTER StatsEnableMask;

    T3_32BIT_REGISTER StatsIncMask;

    LM_UINT8 Reserved[108];

    T3_32BIT_REGISTER ClassOfServCnt[16];
    T3_32BIT_REGISTER DmaReadQFullCnt;
    T3_32BIT_REGISTER DmaPriorityReadQFullCnt;
    T3_32BIT_REGISTER SdcQFullCnt;

    T3_32BIT_REGISTER NicRingSetSendProdIdxCnt;
    T3_32BIT_REGISTER StatusUpdatedCnt;
    T3_32BIT_REGISTER InterruptsCnt;
    T3_32BIT_REGISTER AvoidInterruptsCnt;
    T3_32BIT_REGISTER SendThresholdHitCnt;

    /* Unused space. */
    LM_UINT8 Unused[800];
} T3_SEND_DATA_INITIATOR, *PT3_SEND_DATA_INITIATOR;



/******************************************************************************/
/* Send data completion control registers. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
    #define SND_DATA_COMP_MODE_RESET                        BIT_0
    #define SND_DATA_COMP_MODE_ENABLE                       BIT_1

    /* Unused space. */
    LM_UINT8 Unused[1020];
} T3_SEND_DATA_COMPLETION, *PT3_SEND_DATA_COMPLETION;



/******************************************************************************/
/* Send BD Ring Selector Control Registers. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
    #define SND_BD_SEL_MODE_RESET                           BIT_0
    #define SND_BD_SEL_MODE_ENABLE                          BIT_1
    #define SND_BD_SEL_MODE_ATTN_ENABLE                     BIT_2

    T3_32BIT_REGISTER Status;
    #define SND_BD_SEL_STATUS_ERROR_ATTN                    BIT_2

    T3_32BIT_REGISTER HwDiag;

    /* Unused space. */
    LM_UINT8 Unused1[52];

    /* Send BD Ring Selector Local NIC Send BD Consumer Index. */
    T3_32BIT_REGISTER NicSendBdSelConIdx[16];

    /* Unused space. */
    LM_UINT8 Unused2[896];
} T3_SEND_BD_SELECTOR, *PT3_SEND_BD_SELECTOR;



/******************************************************************************/
/* Send BD initiator control registers. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
    #define SND_BD_IN_MODE_RESET                            BIT_0
    #define SND_BD_IN_MODE_ENABLE                           BIT_1
    #define SND_BD_IN_MODE_ATTN_ENABLE                      BIT_2

    T3_32BIT_REGISTER Status;
    #define SND_BD_IN_STATUS_ERROR_ATTN                     BIT_2

    /* Send BD initiator local NIC send BD producer index. */
    T3_32BIT_REGISTER NicSendBdInProdIdx[16];

    /* Unused space. */
    LM_UINT8 Unused2[952];
} T3_SEND_BD_INITIATOR, *PT3_SEND_BD_INITIATOR;



/******************************************************************************/
/* Send BD Completion Control. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
    #define SND_BD_COMP_MODE_RESET                          BIT_0
    #define SND_BD_COMP_MODE_ENABLE                         BIT_1
    #define SND_BD_COMP_MODE_ATTN_ENABLE                    BIT_2

    /* Unused space. */
    LM_UINT8 Unused2[1020];
} T3_SEND_BD_COMPLETION, *PT3_SEND_BD_COMPLETION;



/******************************************************************************/
/* Receive list placement control registers. */
/******************************************************************************/

typedef struct {
    /* Mode. */
    T3_32BIT_REGISTER Mode;
    #define RCV_LIST_PLMT_MODE_RESET                        BIT_0
    #define RCV_LIST_PLMT_MODE_ENABLE                       BIT_1
    #define RCV_LIST_PLMT_MODE_CLASS0_ATTN_ENABLE           BIT_2
    #define RCV_LIST_PLMT_MODE_MAPPING_OOR_ATTN_ENABLE      BIT_3
    #define RCV_LIST_PLMT_MODE_STATS_OFLOW_ATTN_ENABLE      BIT_4

    /* Status. */
    T3_32BIT_REGISTER Status;
    #define RCV_LIST_PLMT_STATUS_CLASS0_ATTN                BIT_2
    #define RCV_LIST_PLMT_STATUS_MAPPING_ATTN               BIT_3
    #define RCV_LIST_PLMT_STATUS_STATS_OFLOW_ATTN           BIT_4

    /* Receive selector list lock register. */
    T3_32BIT_REGISTER Lock;
    #define RCV_LIST_SEL_LOCK_REQUEST_MASK                  0xffff
    #define RCV_LIST_SEL_LOCK_GRANT_MASK                    0xffff0000

    /* Selector non-empty bits. */
    T3_32BIT_REGISTER NonEmptyBits;
    #define RCV_LIST_SEL_NON_EMPTY_MASK                     0xffff

    /* Receive list placement configuration register. */
    T3_32BIT_REGISTER Config;

    /* Receive List Placement statistics Control. */
    T3_32BIT_REGISTER StatsCtrl;
#define RCV_LIST_STATS_ENABLE                               BIT_0
#define RCV_LIST_STATS_FAST_UPDATE                          BIT_1

    /* Receive List Placement statistics Enable Mask. */
    T3_32BIT_REGISTER StatsEnableMask;
    #define T3_DISABLE_LONG_BURST_READ_DYN_FIX              BIT_22

    /* Receive List Placement statistics Increment Mask. */
    T3_32BIT_REGISTER StatsIncMask;

    /* Unused space. */
    LM_UINT8 Unused1[224];

    struct {
        T3_32BIT_REGISTER Head;
        T3_32BIT_REGISTER Tail;
        T3_32BIT_REGISTER Count;

        /* Unused space. */
        LM_UINT8 Unused[4];
    } RcvSelectorList[16];

    /* Local statistics counter. */
    T3_32BIT_REGISTER ClassOfServCnt[16];

    T3_32BIT_REGISTER DropDueToFilterCnt;
    T3_32BIT_REGISTER DmaWriteQFullCnt;
    T3_32BIT_REGISTER DmaHighPriorityWriteQFullCnt;
    T3_32BIT_REGISTER NoMoreReceiveBdCnt;
    T3_32BIT_REGISTER IfInDiscardsCnt;
    T3_32BIT_REGISTER IfInErrorsCnt;
    T3_32BIT_REGISTER RcvThresholdHitCnt;

    /* Another unused space. */
    LM_UINT8 Unused2[420];
} T3_RCV_LIST_PLACEMENT, *PT3_RCV_LIST_PLACEMENT;



/******************************************************************************/
/* Receive Data and Receive BD Initiator Control. */
/******************************************************************************/

typedef struct {
    /* Mode. */
    T3_32BIT_REGISTER Mode;
    #define RCV_DATA_BD_IN_MODE_RESET                   BIT_0
    #define RCV_DATA_BD_IN_MODE_ENABLE                  BIT_1
    #define RCV_DATA_BD_IN_MODE_JUMBO_BD_NEEDED         BIT_2
    #define RCV_DATA_BD_IN_MODE_FRAME_TOO_BIG           BIT_3
    #define RCV_DATA_BD_IN_MODE_INVALID_RING_SIZE       BIT_4

    /* Status. */
    T3_32BIT_REGISTER Status;
    #define RCV_DATA_BD_IN_STATUS_JUMBO_BD_NEEDED       BIT_2
    #define RCV_DATA_BD_IN_STATUS_FRAME_TOO_BIG         BIT_3
    #define RCV_DATA_BD_IN_STATUS_INVALID_RING_SIZE     BIT_4

    /* Split frame minium size. */
    T3_32BIT_REGISTER SplitFrameMinSize;

    /* Unused space. */
    LM_UINT8 Unused1[0x2440-0x240c];

    /* Receive RCBs. */
    T3_RCB JumboRcvRcb;
    T3_RCB StdRcvRcb;
    T3_RCB MiniRcvRcb;

    /* Receive Data and Receive BD Ring Initiator Local NIC Receive */
    /* BD Consumber Index. */
    T3_32BIT_REGISTER NicJumboConIdx;
    T3_32BIT_REGISTER NicStdConIdx;
    T3_32BIT_REGISTER NicMiniConIdx;

    /* Unused space. */
    LM_UINT8 Unused2[4];

    /* Receive Data and Receive BD Initiator Local Receive Return ProdIdx. */
    T3_32BIT_REGISTER RcvDataBdProdIdx[16];

    /* Receive Data and Receive BD Initiator Hardware Diagnostic. */
    T3_32BIT_REGISTER HwDiag;

    /* Unused space. */
    LM_UINT8 Unused3[828];
} T3_RCV_DATA_BD_INITIATOR, *PT3_RCV_DATA_BD_INITIATOR;



/******************************************************************************/
/* Receive Data Completion Control Registes. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
    #define RCV_DATA_COMP_MODE_RESET                        BIT_0
    #define RCV_DATA_COMP_MODE_ENABLE                       BIT_1
    #define RCV_DATA_COMP_MODE_ATTN_ENABLE                  BIT_2

    /* Unused spaced. */
    LM_UINT8 Unused[1020];
} T3_RCV_DATA_COMPLETION, *PT3_RCV_DATA_COMPLETION;



/******************************************************************************/
/* Receive BD Initiator Control. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
    #define RCV_BD_IN_MODE_RESET                            BIT_0
    #define RCV_BD_IN_MODE_ENABLE                           BIT_1
    #define RCV_BD_IN_MODE_BD_IN_DIABLED_RCB_ATTN_ENABLE    BIT_2

    T3_32BIT_REGISTER Status;
    #define RCV_BD_IN_STATUS_BD_IN_DIABLED_RCB_ATTN         BIT_2

    T3_32BIT_REGISTER NicJumboRcvProdIdx;
    T3_32BIT_REGISTER NicStdRcvProdIdx;
    T3_32BIT_REGISTER NicMiniRcvProdIdx;

    T3_32BIT_REGISTER MiniRcvThreshold;
    T3_32BIT_REGISTER StdRcvThreshold;
    T3_32BIT_REGISTER JumboRcvThreshold;

    /* Unused space. */
    LM_UINT8 Unused[992];
} T3_RCV_BD_INITIATOR, *PT3_RCV_BD_INITIATOR;



/******************************************************************************/
/* Receive BD Completion Control Registers. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
    #define RCV_BD_COMP_MODE_RESET                          BIT_0
    #define RCV_BD_COMP_MODE_ENABLE                         BIT_1
    #define RCV_BD_COMP_MODE_ATTN_ENABLE                    BIT_2

    T3_32BIT_REGISTER Status;
    #define RCV_BD_COMP_STATUS_ERROR_ATTN                   BIT_2

    T3_32BIT_REGISTER  NicJumboRcvBdProdIdx;
    T3_32BIT_REGISTER  NicStdRcvBdProdIdx;
    T3_32BIT_REGISTER  NicMiniRcvBdProdIdx;

    /* Unused space. */
    LM_UINT8 Unused[1004];
} T3_RCV_BD_COMPLETION, *PT3_RCV_BD_COMPLETION;



/******************************************************************************/
/* Receive list selector control register. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
    #define RCV_LIST_SEL_MODE_RESET                         BIT_0
    #define RCV_LIST_SEL_MODE_ENABLE                        BIT_1
    #define RCV_LIST_SEL_MODE_ATTN_ENABLE                   BIT_2

    T3_32BIT_REGISTER Status;
    #define RCV_LIST_SEL_STATUS_ERROR_ATTN                  BIT_2

    /* Unused space. */
    LM_UINT8 Unused[1016];
} T3_RCV_LIST_SELECTOR, *PT3_RCV_LIST_SELECTOR;



/******************************************************************************/
/* Mbuf cluster free registers. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
#define MBUF_CLUSTER_FREE_MODE_RESET    BIT_0
#define MBUF_CLUSTER_FREE_MODE_ENABLE   BIT_1

    T3_32BIT_REGISTER Status;

    /* Unused space. */
    LM_UINT8 Unused[1016];
} T3_MBUF_CLUSTER_FREE, *PT3_MBUF_CLUSTER_FREE;



/******************************************************************************/
/* Host coalescing control registers. */
/******************************************************************************/

typedef struct {
    /* Mode. */
    T3_32BIT_REGISTER Mode;
    #define HOST_COALESCE_RESET                         BIT_0
    #define HOST_COALESCE_ENABLE                        BIT_1
    #define HOST_COALESCE_ATTN                          BIT_2
    #define HOST_COALESCE_NOW                           BIT_3
    #define HOST_COALESCE_FULL_STATUS_MODE              BIT_NONE
    #define HOST_COALESCE_64_BYTE_STATUS_MODE           BIT_7
    #define HOST_COALESCE_32_BYTE_STATUS_MODE           BIT_8
    #define HOST_COALESCE_CLEAR_TICKS_ON_RX_BD_EVENT    BIT_9
    #define HOST_COALESCE_CLEAR_TICKS_ON_TX_BD_EVENT    BIT_10
    #define HOST_COALESCE_NO_INT_ON_COALESCE_NOW_MODE   BIT_11
    #define HOST_COALESCE_NO_INT_ON_FORCE_DMAD_MODE     BIT_12

    /* Status. */
    T3_32BIT_REGISTER Status;
    #define HOST_COALESCE_ERROR_ATTN                    BIT_2

    /* Receive coalescing ticks. */
    T3_32BIT_REGISTER RxCoalescingTicks;

    /* Send coalescing ticks. */
    T3_32BIT_REGISTER TxCoalescingTicks;

    /* Receive max coalesced frames. */
    T3_32BIT_REGISTER RxMaxCoalescedFrames;

    /* Send max coalesced frames. */
    T3_32BIT_REGISTER TxMaxCoalescedFrames;

    /* Receive coalescing ticks during interrupt. */
    T3_32BIT_REGISTER RxCoalescedTickDuringInt;

    /* Send coalescing ticks during interrupt. */
    T3_32BIT_REGISTER TxCoalescedTickDuringInt;

    /* Receive max coalesced frames during interrupt. */
    T3_32BIT_REGISTER RxMaxCoalescedFramesDuringInt;

    /* Send max coalesced frames during interrupt. */
    T3_32BIT_REGISTER TxMaxCoalescedFramesDuringInt;

    /* Statistics tick. */
    T3_32BIT_REGISTER StatsCoalescingTicks;

    /* Unused space. */
    LM_UINT8 Unused2[4];

    /* Statistics host address. */
    T3_64BIT_REGISTER StatsBlkHostAddr;

    /* Status block host address.*/
    T3_64BIT_REGISTER StatusBlkHostAddr;

    /* Statistics NIC address. */
    T3_32BIT_REGISTER StatsBlkNicAddr;

    /* Statust block NIC address. */
    T3_32BIT_REGISTER StatusBlkNicAddr;

    /* Flow attention registers. */
    T3_32BIT_REGISTER FlowAttn;

    /* Unused space. */
    LM_UINT8 Unused3[4];

    T3_32BIT_REGISTER NicJumboRcvBdConIdx;
    T3_32BIT_REGISTER NicStdRcvBdConIdx;
    T3_32BIT_REGISTER NicMiniRcvBdConIdx;

    /* Unused space. */
    LM_UINT8 Unused4[36];

    T3_32BIT_REGISTER NicRetProdIdx[16];
    T3_32BIT_REGISTER NicSndBdConIdx[16];

    /* Unused space. */
    LM_UINT8 Unused5[768];
} T3_HOST_COALESCING, *PT3_HOST_COALESCING;



/******************************************************************************/
/* Memory arbiter registers. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
#define T3_MEM_ARBITER_MODE_RESET       BIT_0
#define T3_MEM_ARBITER_MODE_ENABLE      BIT_1

    T3_32BIT_REGISTER Status;

    T3_32BIT_REGISTER ArbTrapAddrLow;
    T3_32BIT_REGISTER ArbTrapAddrHigh;

    /* Unused space. */
    LM_UINT8 Unused[1008];
} T3_MEM_ARBITER, *PT3_MEM_ARBITER;



/******************************************************************************/
/* Buffer manager control register. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
    #define BUFMGR_MODE_RESET                           BIT_0
    #define BUFMGR_MODE_ENABLE                          BIT_1
    #define BUFMGR_MODE_ATTN_ENABLE                     BIT_2
    #define BUFMGR_MODE_BM_TEST                         BIT_3
    #define BUFMGR_MODE_MBUF_LOW_ATTN_ENABLE            BIT_4

    T3_32BIT_REGISTER Status;
    #define BUFMGR_STATUS_ERROR                         BIT_2
    #define BUFMGR_STATUS_MBUF_LOW                      BIT_4

    T3_32BIT_REGISTER MbufPoolAddr;
    T3_32BIT_REGISTER MbufPoolSize;
    T3_32BIT_REGISTER MbufReadDmaLowWaterMark;
    T3_32BIT_REGISTER MbufMacRxLowWaterMark;
    T3_32BIT_REGISTER MbufHighWaterMark;

    T3_32BIT_REGISTER RxCpuMbufAllocReq;
    #define BUFMGR_MBUF_ALLOC_BIT                     BIT_31
    T3_32BIT_REGISTER RxCpuMbufAllocResp;
    T3_32BIT_REGISTER TxCpuMbufAllocReq;
    T3_32BIT_REGISTER TxCpuMbufAllocResp;

    T3_32BIT_REGISTER DmaDescPoolAddr;
    T3_32BIT_REGISTER DmaDescPoolSize;
    T3_32BIT_REGISTER DmaLowWaterMark;
    T3_32BIT_REGISTER DmaHighWaterMark;

    T3_32BIT_REGISTER RxCpuDmaAllocReq;
    T3_32BIT_REGISTER RxCpuDmaAllocResp;
    T3_32BIT_REGISTER TxCpuDmaAllocReq;
    T3_32BIT_REGISTER TxCpuDmaAllocResp;

    T3_32BIT_REGISTER Hwdiag[3];

    /* Unused space. */
    LM_UINT8 Unused[936];
} T3_BUFFER_MANAGER, *PT3_BUFFER_MANAGER;



/******************************************************************************/
/* Read DMA control registers. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
    #define DMA_READ_MODE_RESET                         BIT_0
    #define DMA_READ_MODE_ENABLE                        BIT_1
    #define DMA_READ_MODE_TARGET_ABORT_ATTN_ENABLE      BIT_2
    #define DMA_READ_MODE_MASTER_ABORT_ATTN_ENABLE      BIT_3
    #define DMA_READ_MODE_PARITY_ERROR_ATTN_ENABLE      BIT_4
    #define DMA_READ_MODE_ADDR_OVERFLOW_ATTN_ENABLE     BIT_5
    #define DMA_READ_MODE_FIFO_OVERRUN_ATTN_ENABLE      BIT_6
    #define DMA_READ_MODE_FIFO_UNDERRUN_ATTN_ENABLE     BIT_7
    #define DMA_READ_MODE_FIFO_OVERREAD_ATTN_ENABLE     BIT_8
    #define DMA_READ_MODE_LONG_READ_ATTN_ENABLE         BIT_9
    #define DMA_READ_MODE_MULTI_SPLIT_ENABLE            BIT_11
    #define DMA_READ_MODE_MULTI_SPLIT_RESET             BIT_12
    #define DMA_READ_MODE_FIFO_SIZE_128                 BIT_17
    #define DMA_READ_MODE_FIFO_LONG_BURST               (BIT_16 | BIT_17)

    T3_32BIT_REGISTER Status;
    #define DMA_READ_STATUS_TARGET_ABORT_ATTN           BIT_2
    #define DMA_READ_STATUS_MASTER_ABORT_ATTN           BIT_3
    #define DMA_READ_STATUS_PARITY_ERROR_ATTN           BIT_4
    #define DMA_READ_STATUS_ADDR_OVERFLOW_ATTN          BIT_5
    #define DMA_READ_STATUS_FIFO_OVERRUN_ATTN           BIT_6
    #define DMA_READ_STATUS_FIFO_UNDERRUN_ATTN          BIT_7
    #define DMA_READ_STATUS_FIFO_OVERREAD_ATTN          BIT_8
    #define DMA_READ_STATUS_LONG_READ_ATTN              BIT_9

    /* Unused space. */
    LM_UINT8 Unused[1016];
} T3_DMA_READ, *PT3_DMA_READ;

#if defined(PC)
#undef PC
#define PC pc
#endif

typedef union T3_CPU 
{
  struct
  {
    T3_32BIT_REGISTER mode;
    #define CPU_MODE_HALT   BIT_10
    #define CPU_MODE_RESET  BIT_0 
    T3_32BIT_REGISTER state;
    T3_32BIT_REGISTER EventMask;
    T3_32BIT_REGISTER reserved1[4];
    T3_32BIT_REGISTER PC;
    T3_32BIT_REGISTER Instruction;
    T3_32BIT_REGISTER SpadUnderflow;
    T3_32BIT_REGISTER WatchdogClear;
    T3_32BIT_REGISTER WatchdogVector;
    T3_32BIT_REGISTER WatchdogSavedPC;
    T3_32BIT_REGISTER HardwareBp;
    T3_32BIT_REGISTER reserved2[3];
    T3_32BIT_REGISTER WatchdogSavedState;    
    T3_32BIT_REGISTER LastBrchAddr;    
    T3_32BIT_REGISTER SpadUnderflowSet;    
    T3_32BIT_REGISTER reserved3[(0x200-0x50)/4];
    T3_32BIT_REGISTER Regs[32];
    T3_32BIT_REGISTER reserved4[(0x400-0x280)/4];
  }reg;
}T3_CPU, *PT3_CPU;

/******************************************************************************/
/* Write DMA control registers. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
    #define DMA_WRITE_MODE_RESET                        BIT_0
    #define DMA_WRITE_MODE_ENABLE                       BIT_1
    #define DMA_WRITE_MODE_TARGET_ABORT_ATTN_ENABLE     BIT_2
    #define DMA_WRITE_MODE_MASTER_ABORT_ATTN_ENABLE     BIT_3
    #define DMA_WRITE_MODE_PARITY_ERROR_ATTN_ENABLE     BIT_4
    #define DMA_WRITE_MODE_ADDR_OVERFLOW_ATTN_ENABLE    BIT_5
    #define DMA_WRITE_MODE_FIFO_OVERRUN_ATTN_ENABLE     BIT_6
    #define DMA_WRITE_MODE_FIFO_UNDERRUN_ATTN_ENABLE    BIT_7
    #define DMA_WRITE_MODE_FIFO_OVERREAD_ATTN_ENABLE    BIT_8
    #define DMA_WRITE_MODE_LONG_READ_ATTN_ENABLE        BIT_9
    #define DMA_WRITE_MODE_RECEIVE_ACCELERATE           BIT_10

    T3_32BIT_REGISTER Status;
    #define DMA_WRITE_STATUS_TARGET_ABORT_ATTN          BIT_2
    #define DMA_WRITE_STATUS_MASTER_ABORT_ATTN          BIT_3
    #define DMA_WRITE_STATUS_PARITY_ERROR_ATTN          BIT_4
    #define DMA_WRITE_STATUS_ADDR_OVERFLOW_ATTN         BIT_5
    #define DMA_WRITE_STATUS_FIFO_OVERRUN_ATTN          BIT_6
    #define DMA_WRITE_STATUS_FIFO_UNDERRUN_ATTN         BIT_7
    #define DMA_WRITE_STATUS_FIFO_OVERREAD_ATTN         BIT_8
    #define DMA_WRITE_STATUS_LONG_READ_ATTN             BIT_9

    /* Unused space. */
    LM_UINT8 Unused[1016];
} T3_DMA_WRITE, *PT3_DMA_WRITE;



/******************************************************************************/
/* Mailbox registers. */
/******************************************************************************/

typedef struct {
    /* Interrupt mailbox registers. */
    T3_64BIT_REGISTER Interrupt[4];

    /* General mailbox registers. */
    T3_64BIT_REGISTER General[8];

    /* Reload statistics mailbox. */
    T3_64BIT_REGISTER ReloadStat;

    /* Receive BD ring producer index registers. */
    T3_64BIT_REGISTER RcvStdProdIdx;
    T3_64BIT_REGISTER RcvJumboProdIdx;
    T3_64BIT_REGISTER RcvMiniProdIdx;

    /* Receive return ring consumer index registers. */
    T3_64BIT_REGISTER RcvRetConIdx[16];

    /* Send BD ring host producer index registers. */
    T3_64BIT_REGISTER SendHostProdIdx[16];

    /* Send BD ring nic producer index registers. */
    T3_64BIT_REGISTER SendNicProdIdx[16];
}T3_MAILBOX, *PT3_MAILBOX;

typedef struct {
    T3_MAILBOX Mailbox;

    /* Priority mailbox registers. */
    T3_32BIT_REGISTER HighPriorityEventVector;
    T3_32BIT_REGISTER HighPriorityEventMask;
    T3_32BIT_REGISTER LowPriorityEventVector;
    T3_32BIT_REGISTER LowPriorityEventMask;

    /* Unused space. */
    LM_UINT8 Unused[496];
} T3_GRC_MAILBOX, *PT3_GRC_MAILBOX;


/******************************************************************************/
/* Flow through queues. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Reset;
    
    LM_UINT8 Unused[12];

    T3_32BIT_REGISTER DmaNormalReadFtqCtrl;
    T3_32BIT_REGISTER DmaNormalReadFtqFullCnt;
    T3_32BIT_REGISTER DmaNormalReadFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER DmaNormalReadFtqFifoWritePeek;

    T3_32BIT_REGISTER DmaHighReadFtqCtrl;
    T3_32BIT_REGISTER DmaHighReadFtqFullCnt;
    T3_32BIT_REGISTER DmaHighReadFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER DmaHighReadFtqFifoWritePeek;

    T3_32BIT_REGISTER DmaCompDiscardFtqCtrl;
    T3_32BIT_REGISTER DmaCompDiscardFtqFullCnt;
    T3_32BIT_REGISTER DmaCompDiscardFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER DmaCompDiscardFtqFifoWritePeek;

    T3_32BIT_REGISTER SendBdCompFtqCtrl;
    T3_32BIT_REGISTER SendBdCompFtqFullCnt;
    T3_32BIT_REGISTER SendBdCompFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER SendBdCompFtqFifoWritePeek;

    T3_32BIT_REGISTER SendDataInitiatorFtqCtrl;
    T3_32BIT_REGISTER SendDataInitiatorFtqFullCnt;
    T3_32BIT_REGISTER SendDataInitiatorFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER SendDataInitiatorFtqFifoWritePeek;

    T3_32BIT_REGISTER DmaNormalWriteFtqCtrl;
    T3_32BIT_REGISTER DmaNormalWriteFtqFullCnt;
    T3_32BIT_REGISTER DmaNormalWriteFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER DmaNormalWriteFtqFifoWritePeek;

    T3_32BIT_REGISTER DmaHighWriteFtqCtrl;
    T3_32BIT_REGISTER DmaHighWriteFtqFullCnt;
    T3_32BIT_REGISTER DmaHighWriteFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER DmaHighWriteFtqFifoWritePeek;

    T3_32BIT_REGISTER SwType1FtqCtrl;
    T3_32BIT_REGISTER SwType1FtqFullCnt;
    T3_32BIT_REGISTER SwType1FtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER SwType1FtqFifoWritePeek;

    T3_32BIT_REGISTER SendDataCompFtqCtrl;
    T3_32BIT_REGISTER SendDataCompFtqFullCnt;
    T3_32BIT_REGISTER SendDataCompFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER SendDataCompFtqFifoWritePeek;

    T3_32BIT_REGISTER HostCoalesceFtqCtrl;
    T3_32BIT_REGISTER HostCoalesceFtqFullCnt;
    T3_32BIT_REGISTER HostCoalesceFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER HostCoalesceFtqFifoWritePeek;

    T3_32BIT_REGISTER MacTxFtqCtrl;
    T3_32BIT_REGISTER MacTxFtqFullCnt;
    T3_32BIT_REGISTER MacTxFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER MacTxFtqFifoWritePeek;

    T3_32BIT_REGISTER MbufClustFreeFtqCtrl;
    T3_32BIT_REGISTER MbufClustFreeFtqFullCnt;
    T3_32BIT_REGISTER MbufClustFreeFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER MbufClustFreeFtqFifoWritePeek;

    T3_32BIT_REGISTER RcvBdCompFtqCtrl;
    T3_32BIT_REGISTER RcvBdCompFtqFullCnt;
    T3_32BIT_REGISTER RcvBdCompFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER RcvBdCompFtqFifoWritePeek;

    T3_32BIT_REGISTER RcvListPlmtFtqCtrl;
    T3_32BIT_REGISTER RcvListPlmtFtqFullCnt;
    T3_32BIT_REGISTER RcvListPlmtFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER RcvListPlmtFtqFifoWritePeek;

    T3_32BIT_REGISTER RcvDataBdInitiatorFtqCtrl;
    T3_32BIT_REGISTER RcvDataBdInitiatorFtqFullCnt;
    T3_32BIT_REGISTER RcvDataBdInitiatorFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER RcvDataBdInitiatorFtqFifoWritePeek;

    T3_32BIT_REGISTER RcvDataCompFtqCtrl;
    T3_32BIT_REGISTER RcvDataCompFtqFullCnt;
    T3_32BIT_REGISTER RcvDataCompFtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER RcvDataCompFtqFifoWritePeek;

    T3_32BIT_REGISTER SwType2FtqCtrl;
    T3_32BIT_REGISTER SwType2FtqFullCnt;
    T3_32BIT_REGISTER SwType2FtqFifoEnqueueDequeue;
    T3_32BIT_REGISTER SwType2FtqFifoWritePeek;

    /* Unused space. */
    LM_UINT8 Unused2[736];
} T3_FTQ, *PT3_FTQ;



/******************************************************************************/
/* Message signaled interrupt registers. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
#define MSI_MODE_RESET       BIT_0
#define MSI_MODE_ENABLE      BIT_1
    T3_32BIT_REGISTER Status;

    T3_32BIT_REGISTER MsiFifoAccess;

    /* Unused space. */
    LM_UINT8 Unused[1012];
} T3_MSG_SIGNALED_INT, *PT3_MSG_SIGNALED_INT;



/******************************************************************************/
/* DMA Completion registes. */
/******************************************************************************/

typedef struct {
    T3_32BIT_REGISTER Mode;
    #define DMA_COMP_MODE_RESET                         BIT_0
    #define DMA_COMP_MODE_ENABLE                        BIT_1

    /* Unused space. */
    LM_UINT8 Unused[1020];
} T3_DMA_COMPLETION, *PT3_DMA_COMPLETION;



/******************************************************************************/
/* GRC registers. */
/******************************************************************************/

typedef struct {
    /* Mode control register. */
    T3_32BIT_REGISTER Mode;
    #define GRC_MODE_UPDATE_ON_COALESCING               BIT_0
    #define GRC_MODE_BYTE_SWAP_NON_FRAME_DATA           BIT_1
    #define GRC_MODE_WORD_SWAP_NON_FRAME_DATA           BIT_2
    #define GRC_MODE_BYTE_SWAP_DATA                     BIT_4
    #define GRC_MODE_WORD_SWAP_DATA                     BIT_5
    #define GRC_MODE_SPLIT_HEADER_MODE                  BIT_8
    #define GRC_MODE_NO_FRAME_CRACKING                  BIT_9
    #define GRC_MODE_INCLUDE_CRC                        BIT_10
    #define GRC_MODE_ALLOW_BAD_FRAMES                   BIT_11
    #define GRC_MODE_NO_INTERRUPT_ON_SENDS              BIT_13
    #define GRC_MODE_NO_INTERRUPT_ON_RECEIVE            BIT_14
    #define GRC_MODE_FORCE_32BIT_PCI_BUS_MODE           BIT_15
    #define GRC_MODE_HOST_STACK_UP                      BIT_16
    #define GRC_MODE_HOST_SEND_BDS                      BIT_17
    #define GRC_MODE_TX_NO_PSEUDO_HEADER_CHKSUM         BIT_20
    #define GRC_MODE_NVRAM_WRITE_ENABLE                 BIT_21
    #define GRC_MODE_RX_NO_PSEUDO_HEADER_CHKSUM         BIT_23
    #define GRC_MODE_INT_ON_TX_CPU_ATTN                 BIT_24
    #define GRC_MODE_INT_ON_RX_CPU_ATTN                 BIT_25
    #define GRC_MODE_INT_ON_MAC_ATTN                    BIT_26
    #define GRC_MODE_INT_ON_DMA_ATTN                    BIT_27
    #define GRC_MODE_INT_ON_FLOW_ATTN                   BIT_28
    #define GRC_MODE_4X_NIC_BASED_SEND_RINGS            BIT_29
    #define GRC_MODE_MULTICAST_FRAME_ENABLE             BIT_30

    /* Misc configuration register. */
    T3_32BIT_REGISTER MiscCfg;
    #define GRC_MISC_CFG_CORE_CLOCK_RESET               BIT_0
    #define GRC_MISC_PRESCALAR_TIMER_MASK               0xfe
    #define GRC_MISC_BD_ID_MASK                         0x0001e000
    #define GRC_MISC_BD_ID_5700                         0x0001e000
    #define GRC_MISC_BD_ID_5701                         0x00000000
    #define GRC_MISC_BD_ID_5703                         0x00000000
    #define GRC_MISC_BD_ID_5703S                        0x00002000
    #define GRC_MISC_BD_ID_5702FE                       0x00004000
    #define GRC_MISC_BD_ID_5704                         0x00000000
    #define GRC_MISC_BD_ID_5704CIOBE                    0x00004000
    #define GRC_MISC_BD_ID_5788                         0x00010000
    #define GRC_MISC_BD_ID_5788M                        0x00018000
    #define GRC_MISC_GPHY_KEEP_POWER_DURING_RESET       BIT_26

    /* Miscellaneous local control register. */
    T3_32BIT_REGISTER LocalCtrl;
    #define GRC_MISC_LOCAL_CTRL_INT_ACTIVE              BIT_0
    #define GRC_MISC_LOCAL_CTRL_CLEAR_INT               BIT_1
    #define GRC_MISC_LOCAL_CTRL_SET_INT                 BIT_2
    #define GRC_MISC_LOCAL_CTRL_INT_ON_ATTN             BIT_3

    #define GRC_MISC_LOCAL_CTRL_GPIO_INPUT3		BIT_5
    #define GRC_MISC_LOCAL_CTRL_GPIO_OE3		BIT_6
    #define GRC_MISC_LOCAL_CTRL_GPIO_OUTPUT3		BIT_7

    #define GRC_MISC_LOCAL_CTRL_GPIO_INPUT0             BIT_8
    #define GRC_MISC_LOCAL_CTRL_GPIO_INPUT1             BIT_9
    #define GRC_MISC_LOCAL_CTRL_GPIO_INPUT2             BIT_10
    #define GRC_MISC_LOCAL_CTRL_GPIO_OE0                BIT_11
    #define GRC_MISC_LOCAL_CTRL_GPIO_OE1                BIT_12
    #define GRC_MISC_LOCAL_CTRL_GPIO_OE2                BIT_13
    #define GRC_MISC_LOCAL_CTRL_GPIO_OUTPUT0            BIT_14
    #define GRC_MISC_LOCAL_CTRL_GPIO_OUTPUT1            BIT_15
    #define GRC_MISC_LOCAL_CTRL_GPIO_OUTPUT2            BIT_16
    #define GRC_MISC_LOCAL_CTRL_ENABLE_EXT_MEMORY       BIT_17
    #define GRC_MISC_LOCAL_CTRL_BANK_SELECT             BIT_21
    #define GRC_MISC_LOCAL_CTRL_SSRAM_TYPE              BIT_22

    #define GRC_MISC_MEMSIZE_256K     0
    #define GRC_MISC_MEMSIZE_512K     (1 << 18)
    #define GRC_MISC_MEMSIZE_1024K    (2 << 18)
    #define GRC_MISC_MEMSIZE_2048K    (3 << 18)
    #define GRC_MISC_MEMSIZE_4096K    (4 << 18)
    #define GRC_MISC_MEMSIZE_8192K    (5 << 18)
    #define GRC_MISC_MEMSIZE_16M      (6 << 18)
    #define GRC_MISC_LOCAL_CTRL_AUTO_SEEPROM            BIT_24


    T3_32BIT_REGISTER Timer;

    T3_32BIT_REGISTER RxCpuEvent;
    T3_32BIT_REGISTER RxTimerRef;
    T3_32BIT_REGISTER RxCpuSemaphore;
    T3_32BIT_REGISTER RemoteRxCpuAttn;

    T3_32BIT_REGISTER TxCpuEvent;
    T3_32BIT_REGISTER TxTimerRef;
    T3_32BIT_REGISTER TxCpuSemaphore;
    T3_32BIT_REGISTER RemoteTxCpuAttn;

    T3_64BIT_REGISTER MemoryPowerUp;

    T3_32BIT_REGISTER EepromAddr;
    #define SEEPROM_ADDR_WRITE       0
    #define SEEPROM_ADDR_READ        (1 << 31)
    #define SEEPROM_ADDR_RW_MASK     0x80000000
    #define SEEPROM_ADDR_COMPLETE    (1 << 30)
    #define SEEPROM_ADDR_FSM_RESET   (1 << 29)
    #define SEEPROM_ADDR_DEV_ID(x)   (x << 26)
    #define SEEPROM_ADDR_DEV_ID_MASK 0x1c000000
    #define SEEPROM_ADDR_START       (1 << 25)
    #define SEEPROM_ADDR_CLK_PERD(x) (x << 16)
    #define SEEPROM_ADDR_ADDRESS(x)  (x & 0xfffc)
    #define SEEPROM_ADDR_ADDRESS_MASK 0x0000ffff

    #define SEEPROM_CLOCK_PERIOD        60
    #define SEEPROM_CHIP_SIZE           (64 * 1024)

    T3_32BIT_REGISTER EepromData;
    T3_32BIT_REGISTER EepromCtrl;

    T3_32BIT_REGISTER MdiCtrl;
    T3_32BIT_REGISTER SepromDelay;

    /* Unused space. */
    LM_UINT8 Unused[948];
} T3_GRC, *PT3_GRC;


/******************************************************************************/
/* NVRAM control registers. */
/******************************************************************************/

typedef struct
{
    T3_32BIT_REGISTER Cmd;
    #define NVRAM_CMD_RESET                             BIT_0
    #define NVRAM_CMD_DONE                              BIT_3
    #define NVRAM_CMD_DO_IT                             BIT_4
    #define NVRAM_CMD_WR                                BIT_5
    #define NVRAM_CMD_RD                                BIT_NONE
    #define NVRAM_CMD_ERASE                             BIT_6
    #define NVRAM_CMD_FIRST                             BIT_7
    #define NVRAM_CMD_LAST                              BIT_8
    #define NVRAM_CMD_WRITE_ENABLE                           BIT_16
    #define NVRAM_CMD_WRITE_DISABLE                          BIT_17
    #define NVRAM_CMD_EN_WR_SR                             BIT_18
    #define NVRAM_CMD_DO_WR_SR                             BIT_19

    T3_32BIT_REGISTER Status;
    T3_32BIT_REGISTER WriteData;

    T3_32BIT_REGISTER Addr;
    #define NVRAM_ADDRESS_MASK                          0xffffff

    T3_32BIT_REGISTER ReadData;

    /* Flash config 1 register. */
    T3_32BIT_REGISTER Config1;
    #define FLASH_INTERFACE_ENABLE                      BIT_0
    #define FLASH_SSRAM_BUFFERED_MODE                  BIT_1
    #define FLASH_PASS_THRU_MODE                        BIT_2
    #define FLASH_BIT_BANG_MODE                         BIT_3
    #define FLASH_STATUS_BITS_MASK            (BIT_4 | BIT_5 | BIT_6)
    #define FLASH_SIZE                                  BIT_25
    #define FLASH_COMPAT_BYPASS                         BIT_31
    #define FLASH_VENDOR_MASK                  (BIT_25 | BIT_24 | BIT_1 | BIT_0)
    #define FLASH_VENDOR_ATMEL_EEPROM                        BIT_25
    #define FLASH_VENDOR_ATMEL_FLASH_BUFFERED       (BIT_25 | BIT_1 | BIT_0)
    #define FLASH_VENDOR_ATMEL_FLASH_UNBUFFERED          (BIT_1 | BIT_0)
    #define FLASH_VENDOR_ST                        (BIT_25 | BIT_24 | BIT_0)
    #define FLASH_VENDOR_SAIFUN                     (BIT_24 | BIT_1 | BIT_0)
    #define FLASH_VENDOR_SST_SMALL                           BIT_0
    #define FLASH_VENDOR_SST_LARGE                      (BIT_25 | BIT_0)

    #define BUFFERED_FLASH (FLASH_INTERFACE_ENABLE | FLASH_SSRAM_BUFFERED_MODE)

    /* Buffered flash (Atmel: AT45DB011B) specific information */
    #define BUFFERED_FLASH_PAGE_POS         9
    #define BUFFERED_FLASH_BYTE_ADDR_MASK   ((1<<BUFFERED_FLASH_PAGE_POS) - 1)
    #define BUFFERED_FLASH_PAGE_SIZE        264
    #define BUFFERED_FLASH_PHY_PAGE_SIZE    512

    /* Bleh!  Definitions for Baxter. */
    #define FLASH_PART_5750_TYPEMASK \
            (BIT_25 | BIT_24 | BIT_1 | BIT_0)

    #define FLASH_PART_5752_TYPEMASK \
            (BIT_25 | BIT_24 | BIT_23 | BIT_22 | BIT_1 | BIT_0)

    #define FLASH_PART_5752_EEPROM_ATMEL_64K        BIT_NONE
    #define FLASH_PART_5752_EEPROM_ATMEL_376K       BIT_25
    #define FLASH_PART_5752_FLASH_ATMEL_AT45DB041   (BIT_25 | BIT_1 | BIT_0)
    #define FLASH_PART_5752_FLASH_ATMEL_AT25F512             (BIT_1 | BIT_0)
    #define FLASH_PART_5752_FLASH_SST_45VF010       (BIT_25 |         BIT_0)
    #define FLASH_PART_5752_FLASH_SST_25F512                         (BIT_0)
    #define FLASH_PART_5752_FLASH_ST_M25P10A           (BIT_25 | BIT_24 | BIT_0)
    #define FLASH_PART_5752_FLASH_ST_M25P05A                  (BIT_24 | BIT_0)
    #define FLASH_PART_5752_FLASH_SAIFUN_SA25F010            (BIT_22)
    #define FLASH_PART_5752_FLASH_SAIFUN_SA25F020          (BIT_22 | BIT_1)
    #define FLASH_PART_5752_FLASH_SAIFUN_SA25F040          (BIT_22 | BIT_0)
    #define FLASH_PART_5752_FLASH_SST_25VF010              (BIT_24 | BIT_22)
    #define FLASH_PART_5752_FLASH_SST_25VF020          (BIT_24 | BIT_22 | BIT_1)
    #define FLASH_PART_5752_FLASH_SST_25VF040          (BIT_24 | BIT_22 | BIT_0)
    #define FLASH_PART_5752_FLASH_ST_M45PE10           (BIT_25 | BIT_22)
    #define FLASH_PART_5752_FLASH_ST_M45PE20           (BIT_25 | BIT_22 | BIT_1)
    #define FLASH_PART_5752_FLASH_ST_M45PE40           (BIT_25 | BIT_22 | BIT_0)

    #define FLASH_PART_5752_PAGEMASK \
            (BIT_30 | BIT_29 | BIT_28)

    #define FLASH_PART_5752_PAGE_SIZE_256B         BIT_NONE
    #define FLASH_PART_5752_PAGE_SIZE_512B         BIT_28
    #define FLASH_PART_5752_PAGE_SIZE_1K           BIT_29
    #define FLASH_PART_5752_PAGE_SIZE_2K          (BIT_29 | BIT_28)
    #define FLASH_PART_5752_PAGE_SIZE_4K           BIT_30
    #define FLASH_PART_5752_PAGE_SIZE_264B        (BIT_30 | BIT_28)


    T3_32BIT_REGISTER Config2;
    #define NVRAM_COMMAND_MASK                             0x000000ff
    #define NVRAM_STATUS_COMMAND(x)                        ((x) << 16)
    #define NVRAM_ERASE_COMMAND(x)                             (x)

    T3_32BIT_REGISTER Config3;
    #define NVRAM_COMMAND_MASK                             0x000000ff
    #define NVRAM_READ_COMMAND(x)                          ((x) << 24)
    #define NVRAM_WRITE_UNBUFFERED_COMMAND(x)              ((x) << 8)
    #define NVRAM_WRITE_BUFFERED_COMMAND(x)                ((x) << 16)
    #define NVRAM_RESET_COMMAND(x)                             (x)

    T3_32BIT_REGISTER SwArb;
    #define SW_ARB_REQ_SET0                             BIT_0
    #define SW_ARB_REQ_SET1                             BIT_1
    #define SW_ARB_REQ_SET2                             BIT_2
    #define SW_ARB_REQ_SET3                             BIT_3
    #define SW_ARB_REQ_CLR0                             BIT_4
    #define SW_ARB_REQ_CLR1                             BIT_5
    #define SW_ARB_REQ_CLR2                             BIT_6
    #define SW_ARB_REQ_CLR3                             BIT_7
    #define SW_ARB_GNT0                                 BIT_8
    #define SW_ARB_GNT1                                 BIT_9
    #define SW_ARB_GNT2                                 BIT_10
    #define SW_ARB_GNT3                                 BIT_11
    #define SW_ARB_REQ0                                 BIT_12
    #define SW_ARB_REQ1                                 BIT_13
    #define SW_ARB_REQ2                                 BIT_14
    #define SW_ARB_REQ3                                 BIT_15

    T3_32BIT_REGISTER NvmAccess;
    #define ACCESS_EN                                   BIT_0
    #define ACCESS_WR_EN                                BIT_1
    #define NVRAM_ACCESS_ENABLE                         BIT_0
    #define NVRAM_ACCESS_WRITE_ENABLE                   BIT_1

    T3_32BIT_REGISTER Write1;
    #define NVRAM_WRITE1_WRENA_CMD(x)         (x)
    #define NVRAM_WRITE1_WRDIS_CMD(x)       ((x) << 8)

    T3_32BIT_REGISTER WatchTimer;

    T3_32BIT_REGISTER Config4;

    /* Unused space. */
    LM_UINT8 Unused[972];
} T3_NVRAM, *PT3_NVRAM;


/******************************************************************************/
/* NIC's internal memory. */
/******************************************************************************/

typedef struct {
    /* Page zero for the internal CPUs. */
    LM_UINT8 PageZero[0x100];               /* 0x0000 */

    /* Send RCBs. */
    T3_RCB SendRcb[16];                     /* 0x0100 */

    /* Receive Return RCBs. */
    T3_RCB RcvRetRcb[16];                   /* 0x0200 */

    /* Statistics block. */
    T3_STATS_BLOCK StatsBlk;                /* 0x0300 */

    /* Status block. */
    T3_STATUS_BLOCK StatusBlk;              /* 0x0b00 */

    /* Reserved for software. */
    LM_UINT8 Reserved[1200];                /* 0x0b50 */

    /* Unmapped region. */
    LM_UINT8 Unmapped[4096];                /* 0x1000 */

    /* DMA descriptors. */
    LM_UINT8 DmaDesc[8192];                 /* 0x2000 */

    /* Buffer descriptors. */
    LM_UINT8 BufferDesc[16384];             /* 0x4000 */
} T3_FIRST_32K_SRAM, *PT3_FIRST_32K_SRAM;



/******************************************************************************/
/* Memory layout. */
/******************************************************************************/

typedef struct {
    /* PCI configuration registers. */
    T3_PCI_CONFIGURATION PciCfg;

    /* Unused. */
    LM_UINT8 Unused1[0x100];                            /* 0x0100 */

    /* Mailbox . */
    T3_MAILBOX Mailbox;                                 /* 0x0200 */

    /* MAC control registers. */
    T3_MAC_CONTROL MacCtrl;                             /* 0x0400 */

    /* Send data initiator control registers. */
    T3_SEND_DATA_INITIATOR SndDataIn;                   /* 0x0c00 */

    /* Send data completion Control registers. */
    T3_SEND_DATA_COMPLETION SndDataComp;                /* 0x1000 */

    /* Send BD ring selector. */
    T3_SEND_BD_SELECTOR SndBdSel;                       /* 0x1400 */

    /* Send BD initiator control registers. */
    T3_SEND_BD_INITIATOR SndBdIn;                       /* 0x1800 */

    /* Send BD completion control registers. */
    T3_SEND_BD_COMPLETION SndBdComp;                    /* 0x1c00 */

    /* Receive list placement control registers. */
    T3_RCV_LIST_PLACEMENT RcvListPlmt;                  /* 0x2000 */

    /* Receive Data and Receive BD Initiator Control. */
    T3_RCV_DATA_BD_INITIATOR RcvDataBdIn;               /* 0x2400 */

    /* Receive Data Completion Control */
    T3_RCV_DATA_COMPLETION RcvDataComp;                 /* 0x2800 */

    /* Receive BD Initiator Control Registers. */
    T3_RCV_BD_INITIATOR RcvBdIn;                        /* 0x2c00 */

    /* Receive BD Completion Control Registers. */
    T3_RCV_BD_COMPLETION RcvBdComp;                     /* 0x3000 */

    /* Receive list selector control registers. */
    T3_RCV_LIST_SELECTOR RcvListSel;                    /* 0x3400 */

    /* Mbuf cluster free registers. */
    T3_MBUF_CLUSTER_FREE MbufClusterFree;               /* 0x3800 */

    /* Host coalescing control registers. */
    T3_HOST_COALESCING HostCoalesce;                    /* 0x3c00 */

    /* Memory arbiter control registers. */
    T3_MEM_ARBITER MemArbiter;                          /* 0x4000 */
    
    /* Buffer manger control registers. */
    T3_BUFFER_MANAGER BufMgr;                           /* 0x4400 */

    /* Read DMA control registers. */
    T3_DMA_READ DmaRead;                                /* 0x4800 */

    /* Write DMA control registers. */
    T3_DMA_WRITE DmaWrite;                              /* 0x4c00 */

    T3_CPU rxCpu;                                       /* 0x5000 */
    T3_CPU txCpu;                                       /* 0x5400 */

    /* Mailboxes. */
    T3_GRC_MAILBOX GrcMailbox;                          /* 0x5800 */

    /* Flow Through queues. */
    T3_FTQ Ftq;                                         /* 0x5c00 */

    /* Message signaled interrupt registes. */
    T3_MSG_SIGNALED_INT Msi;                            /* 0x6000 */

    /* DMA completion registers. */
    T3_DMA_COMPLETION DmaComp;                          /* 0x6400 */

    /* GRC registers. */
    T3_GRC Grc;                                         /* 0x6800 */

    /* Unused space. */
    LM_UINT8 Unused2[1024];                             /* 0x6c00 */

    /* NVRAM registers. */
    T3_NVRAM Nvram;                                     /* 0x7000 */

    /* Unused space. */
    LM_UINT8 Unused3[3072];                             /* 0x7400 */
    
    /* The 32k memory window into the NIC's */
    /* internal memory.  The memory window is */
    /* controlled by the Memory Window Base */
    /* Address register.  This register is located */
    /* in the PCI configuration space. */
    union {                                             /* 0x8000 */
        T3_FIRST_32K_SRAM First32k;

        /* Use the memory window base address register to determine the */
        /* MBUF segment. */
        LM_UINT32 Mbuf[32768/4];
        LM_UINT32 MemBlock32K[32768/4];
    } uIntMem;
} T3_STD_MEM_MAP, *PT3_STD_MEM_MAP;


/******************************************************************************/
/* Adapter info. */
/******************************************************************************/

typedef struct
{
    LM_UINT16 Svid;
    LM_UINT16 Ssid;
    LM_UINT32 PhyId;
    LM_UINT32 Serdes;   /* 0 = copper PHY, 1 = Serdes */
} LM_ADAPTER_INFO, *PLM_ADAPTER_INFO;


/******************************************************************************/
/* Flash info. */
/******************************************************************************/

typedef struct {
    LM_UINT8  jedecnum;
    LM_UINT8  romtype;
    #define ROM_TYPE_EEPROM  0x1
    #define ROM_TYPE_FLASH   0x2
    LM_BOOL   buffered;

    LM_UINT32 chipsize;
    LM_UINT32 pagesize;
} FLASHINFO;


#define JEDEC_ATMEL    0x1f
#define JEDEC_ST       0x20
#define JEDEC_SAIFUN   0x4f
#define JEDEC_SST      0xbf

#define ATMEL_AT24C64_CHIP_SIZE                   (64 * 1024)
#define ATMEL_AT24C64_PAGE_SIZE                     (32)
#define ATMEL_AT24C64_PAGE_MASK        (ATMEL_AT24C64_PAGE_SIZE - 1)

#define ATMEL_AT24C512_CHIP_SIZE                 (512 * 1024)
#define ATMEL_AT24C512_PAGE_SIZE                    (128)
#define ATMEL_AT24C512_PAGE_MASK        (ATMEL_AT24C512_PAGE_SIZE - 1)

#define ATMEL_AT45DB0X1B_PAGE_POS                        9
#define ATMEL_AT45DB0X1B_PAGE_SIZE                      264
#define ATMEL_AT45DB0X1B_PAGE_MASK                      0x1ff
#define ATMEL_AT45DB0X1B_BUFFER_WRITE_CMD               0x83

/* Currently unsupported flash type */
#define ATMEL_AT25F512_PAGE_SIZE                        256
#define ATMEL_AT25F512_PAGE_MASK        (ATMEL_AT25F512_PAGE_SIZE - 1)

#define ST_M45PEX0_PAGE_SIZE                            256
#define ST_M45PEX0_PAGE_MASK                (ST_M45PEX0_PAGE_SIZE - 1)
#define ST_M45PEX0_READ_STATUS_CMD                      0x05
#define ST_M45PEX0_PAGE_ERASE_CMD                       0xDB
#define ST_M45PEX0_PAGE_PRGM_CMD                        0x0A
#define ST_M45PEX0_WRENA_CMD                            0x06
#define ST_M45PEX0_WRDIS_CMD                            0x04

#define SAIFUN_SA25F0XX_PAGE_SIZE                       256
#define SAIFUN_SA25F0XX_PAGE_MASK         (SAIFUN_SA25F0XX_PAGE_SIZE - 1)
#define SAIFUN_SA25F0XX_READ_STATUS_CMD                 0x05
#define SAIFUN_SA25F0XX_PAGE_ERASE_CMD                  0x81
#define SAIFUN_SA25F0XX_PAGE_WRITE_CMD                  0x02
#define SAIFUN_SA25F0XX_WRENA_CMD                       0x06

/* Currently unsupported flash type */
#define SST_25VF0X0_PAGE_SIZE                           4098
#define SST_25VF0X0_PAGE_MASK                (SST_25VF0X0_PAGE_SIZE - 1)


/******************************************************************************/
/* Packet queues. */
/******************************************************************************/

DECLARE_QUEUE_TYPE(LM_RX_PACKET_Q, MAX_RX_PACKET_DESC_COUNT);
DECLARE_QUEUE_TYPE(LM_TX_PACKET_Q, MAX_TX_PACKET_DESC_COUNT);



/******************************************************************************/
/* Tx counters. */
/******************************************************************************/

typedef struct {
    LM_COUNTER TxPacketGoodCnt;
    LM_COUNTER TxBytesGoodCnt;
    LM_COUNTER TxPacketAbortedCnt;
    LM_COUNTER NoSendBdLeftCnt;
    LM_COUNTER NoMapRegisterLeftCnt;
    LM_COUNTER TooManyFragmentsCnt;
    LM_COUNTER NoTxPacketDescCnt;
} LM_TX_COUNTERS, *PLM_TX_COUNTERS;



/******************************************************************************/
/* Rx counters. */
/******************************************************************************/

typedef struct {
    LM_COUNTER RxPacketGoodCnt;
    LM_COUNTER RxBytesGoodCnt;
    LM_COUNTER RxPacketErrCnt;
    LM_COUNTER RxErrCrcCnt;
    LM_COUNTER RxErrCollCnt;
    LM_COUNTER RxErrLinkLostCnt;
    LM_COUNTER RxErrPhyDecodeCnt;
    LM_COUNTER RxErrOddNibbleCnt;
    LM_COUNTER RxErrMacAbortCnt;
    LM_COUNTER RxErrShortPacketCnt;
    LM_COUNTER RxErrNoResourceCnt;
    LM_COUNTER RxErrLargePacketCnt;
} LM_RX_COUNTERS, *PLM_RX_COUNTERS;



/******************************************************************************/
/* Receive producer rings. */
/******************************************************************************/

typedef enum {
    T3_UNKNOWN_RCV_PROD_RING    = 0,
    T3_STD_RCV_PROD_RING        = 1,
    T3_MINI_RCV_PROD_RING       = 2,
    T3_JUMBO_RCV_PROD_RING      = 3
} T3_RCV_PROD_RING, *PT3_RCV_PROD_RING;



/******************************************************************************/
/* Packet descriptor. */
/******************************************************************************/

#define LM_PACKET_SIGNATURE_TX              0x6861766b
#define LM_PACKET_SIGNATURE_RX              0x6b766168

typedef struct _LM_PACKET {
    /* Set in LM. */
    LM_STATUS PacketStatus;

    /* Set in LM for Rx, in UM for Tx. */
    LM_UINT32 PacketSize;

    LM_UINT16 Flags;

    LM_UINT16 VlanTag;

    union {
        /* Send info. */
        struct {
            /* Set up by UM. */
            LM_UINT32 FragCount;

#if INCLUDE_TCP_SEG_SUPPORT
            LM_UINT32 MaxSegmentSize;
#endif
        } Tx;

        /* Receive info. */
        struct {
            /* This descriptor belongs to either Std, Mini, or Jumbo ring. */
            LM_UINT16 RcvProdRing;
            LM_UINT16 RcvRingProdIdx;

            /* Receive buffer size */
            LM_UINT32 RxBufferSize;

            /* Checksum information. */
            LM_UINT16 IpChecksum;
            LM_UINT16 TcpUdpChecksum;

        } Rx;
    } u;
} LM_PACKET;



/******************************************************************************/
/* Tigon3 device block. */
/******************************************************************************/

typedef struct _LM_DEVICE_BLOCK {
    /* Memory view. */
    PT3_STD_MEM_MAP pMemView;

    /* Base address of the block of memory in which the LM_PACKET descriptors */
    /* are allocated from. */
    PLM_VOID pPacketDescBase;

    LM_UINT32 MiscHostCtrl;
    LM_UINT32 GrcLocalCtrl;
    LM_UINT32 DmaReadWriteCtrl;
    LM_UINT32 PciState;
    LM_UINT32 ClockCtrl;
    LM_UINT32 DmaReadFifoSize;
    LM_UINT32 GrcMode;

    LM_UINT32 PowerLevel;

    LM_UINT32 Flags;

    #define MINI_PCI_FLAG              0x1
    #define PCI_EXPRESS_FLAG           0x2
    #define BCM5788_FLAG               0x4
    #define FIBER_WOL_CAPABLE_FLAG     0x8
    #define WOL_LIMIT_10MBPS_FLAG      0x10
    #define ENABLE_MWI_FLAG            0x20
    #define USE_TAGGED_STATUS_FLAG     0x40

    /* NIC will not compute the pseudo header checksum.  The driver or OS */
    /* must seed the checksum field with the pseudo checksum. */
    #define NO_TX_PSEUDO_HDR_CSUM_FLAG 0x80

    /* The receive checksum in the BD does not include the pseudo checksum. */
    /* The OS or the driver must calculate the pseudo checksum and add it to */
    /* the checksum in the BD. */
    #define NO_RX_PSEUDO_HDR_CSUM_FLAG 0x100

    #define ENABLE_PCIX_FIX_FLAG       0x200

    #define TX_4G_WORKAROUND_FLAG      0x400
    #define UNDI_FIX_FLAG              0x800
    #define FLUSH_POSTED_WRITE_FLAG    0x1000
    #define REG_RD_BACK_FLAG           0x2000

    /* Use NIC or Host based send BD. */
    #define NIC_SEND_BD_FLAG           0x4000

    /* Athlon fix. */
    #define DELAY_PCI_GRANT_FLAG       0x8000

    /* Enable OneDmaAtOnce */
    #define ONE_DMA_AT_ONCE_FLAG       0x10000

    /* Enable PCI-X multi split */
    #define MULTI_SPLIT_ENABLE_FLAG    0x20000

    #define RX_BD_LIMIT_64_FLAG        0x40000

    #define DMA_WR_MODE_RX_ACCELERATE_FLAG 0x80000

    /* write protect */
    #define EEPROM_WP_FLAG             0x100000
    #define FLASH_DETECTED_FLAG        0x200000

    #define DISABLE_D3HOT_FLAG         0x400000

    /* 5753 should not output gpio2 */
    #define GPIO2_DONOT_OUTPUT         0x800000

    #define USING_MSI_FLAG             0x1000000 
    #define JUMBO_CAPABLE_FLAG         0x2000000 
    #define PROTECTED_NVRAM_FLAG       0x4000000
    #define T3_HAS_TWO_CPUS            0x8000000
    #define HOST_COALESCING_BUG_FIX    0x10000000

    /* 5750 in BCM4785 */
    #define SB_CORE_FLAG               0x20000000
    /* 5750 in RGMII mode (BCM4785) */
    #define RGMII_MODE_FLAG            0x40000000
    /* connected to a ROBO switch */
    #define ROBO_SWITCH_FLAG           0x80000000

#ifdef BCM_WL_EMULATOR
    LM_BOOL  wl_emulate_rx;
    LM_BOOL  wl_emulate_tx;	
    void *wlc; 
#endif /*BCM_WL_EMULATOR*/

    /* Rx info */
    LM_UINT32 RxStdDescCnt;
    LM_UINT32 RxStdQueuedCnt;
    LM_UINT32 RxStdProdIdx;

    PT3_RCV_BD pRxStdBdVirt;
    LM_PHYSICAL_ADDRESS RxStdBdPhy;

    LM_UINT32 RxPacketDescCnt;
    LM_RX_PACKET_Q RxPacketFreeQ;
    LM_RX_PACKET_Q RxPacketReceivedQ;

    LM_PACKET *RxStdRing[T3_STD_RCV_RCB_ENTRY_COUNT];
#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
    LM_PACKET *RxJumboRing[T3_JUMBO_RCV_RCB_ENTRY_COUNT];
#endif

    /* Receive info. */
    PT3_RCV_BD pRcvRetBdVirt;
    LM_PHYSICAL_ADDRESS RcvRetBdPhy;
    LM_UINT32 RcvRetConIdx;
    LM_UINT32 RcvRetRcbEntryCount;
    LM_UINT32 RcvRetRcbEntryCountMask;

#if T3_JUMBO_RCV_RCB_ENTRY_COUNT
    LM_UINT32 RxJumboDescCnt;
    LM_UINT32 RxJumboBufferSize;
    LM_UINT32 RxJumboQueuedCnt;

    LM_UINT32 RxJumboProdIdx;

    PT3_RCV_BD pRxJumboBdVirt;
    LM_PHYSICAL_ADDRESS RxJumboBdPhy;
#endif /* T3_JUMBO_RCV_RCB_ENTRY_COUNT */

    /* These values are used by the upper module to inform the protocol */
    /* of the maximum transmit/receive packet size. */
    LM_UINT32 TxMtu;    /* Does not include CRC. */
    LM_UINT32 RxMtu;    /* Does not include CRC. */

#if INCLUDE_TCP_SEG_SUPPORT
    LM_UINT32 LargeSendMaxSize;
    LM_UINT32 LargeSendMinNumSeg;
#endif

    /* We need to shadow the EMAC, Rx, Tx mode registers.  With B0 silicon, */
    /* we may have problems reading any MAC registers in 10mb mode. */
    LM_UINT32 MacMode;
    LM_UINT32 RxMode;
    LM_UINT32 TxMode;

    /* MiMode register. */
    LM_UINT32 MiMode;

    /* Host coalesce mode register. */
    LM_UINT32 CoalesceMode;

    /* Send info. */
    LM_UINT32 TxPacketDescCnt;

    /* Tx info. */
    LM_TX_PACKET_Q TxPacketFreeQ;
    LM_TX_PACKET_Q TxPacketXmittedQ;

    /* Pointers to SendBd. */
    PT3_SND_BD pSendBdVirt;
    LM_PHYSICAL_ADDRESS SendBdPhy;  /* Only valid for Host based Send BD. */

    /* Send producer and consumer indices. */
    LM_UINT32 SendProdIdx;
    LM_UINT32 SendConIdx;

    /* Number of BD left. */
    MM_ATOMIC_T SendBdLeft;

    T3_SND_BD ShadowSendBd[T3_SEND_RCB_ENTRY_COUNT];
    LM_PACKET *SendRing[T3_SEND_RCB_ENTRY_COUNT];

    /* Counters. */
    LM_RX_COUNTERS RxCounters;
    LM_TX_COUNTERS TxCounters;

    /* Host coalescing parameters. */
    LM_UINT32 RxCoalescingTicks;
    LM_UINT32 TxCoalescingTicks;
    LM_UINT32 RxMaxCoalescedFrames;
    LM_UINT32 TxMaxCoalescedFrames;
    LM_UINT32 StatsCoalescingTicks;
    LM_UINT32 RxCoalescingTicksDuringInt;
    LM_UINT32 TxCoalescingTicksDuringInt;
    LM_UINT32 RxMaxCoalescedFramesDuringInt;
    LM_UINT32 TxMaxCoalescedFramesDuringInt;

    /* DMA water marks. */
    LM_UINT32 DmaMbufLowMark;
    LM_UINT32 RxMacMbufLowMark;
    LM_UINT32 MbufHighMark;

    /* Status block. */
    PT3_STATUS_BLOCK pStatusBlkVirt;
    LM_PHYSICAL_ADDRESS StatusBlkPhy;

    /* Statistics block. */
    PT3_STATS_BLOCK pStatsBlkVirt;
    LM_PHYSICAL_ADDRESS StatsBlkPhy;

    /* Current receive mask. */
    LM_UINT32 ReceiveMask;

    /* Task offload capabilities. */
    LM_TASK_OFFLOAD TaskOffloadCap;

    /* Task offload selected. */
    LM_TASK_OFFLOAD TaskToOffload;

    /* Wake up capability. */
    LM_WAKE_UP_MODE WakeUpModeCap;

    /* Wake up capability. */
    LM_WAKE_UP_MODE WakeUpMode;

    /* Flow control. */
    LM_FLOW_CONTROL FlowControlCap;
    LM_FLOW_CONTROL FlowControl;

    /* interrupt status tag */
    LM_UINT32 LastTag;

    /* Current node address. */
    LM_UINT8 NodeAddress[8];

    /* The adapter's node address. */
    LM_UINT8 PermanentNodeAddress[8];

    /* Adapter info. */
    LM_UINT16 BusNum;               // Init by the upper module.
    LM_UINT8 DevNum;                // Init by the upper module.
    LM_UINT8 FunctNum;              // Init by the upper module.
    LM_UINT16 PciVendorId;
    LM_UINT16 PciDeviceId;
    LM_UINT32 BondId;
    LM_UINT8 Irq;
    LM_UINT8 IntPin;
    LM_UINT8 CacheLineSize;
    LM_UINT8 PciRevId;
    LM_UINT32 PciCommandStatusWords;
    LM_UINT32 ChipRevId;
    LM_UINT16 SubsystemVendorId;
    LM_UINT16 SubsystemId;
    PLM_UINT8 pMappedMemBase;

    /* Saved PCI configuration registers for restoring after a reset. */
    LM_UINT32 SavedCacheLineReg;

    /* Phy info. */
    LM_UINT32 PhyAddr;
    LM_UINT32 PhyId;

    /* Requested phy settings. */
    LM_LINE_SPEED RequestedLineSpeed;
    LM_DUPLEX_MODE RequestedDuplexMode;

    /* Disable auto-negotiation. */
    LM_UINT32 DisableAutoNeg;

    LM_UINT32 AutoNegJustInited;

    /* Ways for the MAC to get link change interrupt. */
    LM_UINT32 PhyIntMode;
    #define T3_PHY_INT_MODE_AUTO                        0
    #define T3_PHY_INT_MODE_MI_INTERRUPT                1
    #define T3_PHY_INT_MODE_LINK_READY                  2
    #define T3_PHY_INT_MODE_AUTO_POLLING                3

    /* Ways to determine link change status. */
    LM_UINT32 LinkChngMode;
    #define T3_LINK_CHNG_MODE_AUTO                      0
    #define T3_LINK_CHNG_MODE_USE_STATUS_REG            1
    #define T3_LINK_CHNG_MODE_USE_STATUS_BLOCK          2

    LM_UINT32 LedCtrl;

    /* WOL Speed */
    LM_UINT32 WolSpeed;
    #define WOL_SPEED_10MB                              1
    #define WOL_SPEED_100MB                             2

    LM_UINT32 PhyFlags;
    #define PHY_RESET_ON_INIT                           0x01
    #define PHY_RESET_ON_LINKDOWN                       0x02
    #define PHY_ADC_FIX                                 0x04
    #define PHY_CHECK_TAPS_AFTER_RESET                  0x08
    #define PHY_5704_A0_FIX                             0x10
    #define PHY_ETHERNET_WIRESPEED                      0x20
    #define PHY_5705_5750_FIX                           0x40
    #define PHY_NO_GIGABIT                              0x80
    #define PHY_CAPACITIVE_COUPLING                     0x100
    #define PHY_IS_FIBER                                0x200
    #define PHY_FIBER_FALLBACK                          0x400


    LM_UINT32 RestoreOnWakeUp;
    LM_LINE_SPEED WakeUpRequestedLineSpeed;
    LM_DUPLEX_MODE WakeUpRequestedDuplexMode;
    LM_UINT32 WakeUpDisableAutoNeg;

    /* Current phy settings. */
    LM_LINE_SPEED LineSpeed;
    LM_LINE_SPEED OldLineSpeed;
    LM_DUPLEX_MODE DuplexMode;
    LM_STATUS LinkStatus;
    LM_UINT32 advertising;
    LM_UINT32 advertising1000;

    LM_UINT32 LoopBackMode;

#define LM_MAC_LOOP_BACK_MODE 1
#define LM_PHY_LOOP_BACK_MODE 2
#define LM_EXT_LOOP_BACK_MODE 3

    LM_LINE_SPEED SavedRequestedLineSpeed;
    LM_DUPLEX_MODE SavedRequestedDuplexMode;
    LM_UINT32 SavedDisableAutoNeg;

    LM_UINT32 MulticastHash[4];

    LM_UINT32 AsfFlags;

#define ASF_ENABLED         1
#define ASF_NEW_HANDSHAKE   2 /* if set, this bit implies ASF enabled as well */

    /* Split Mode flags */
    LM_UINT32 SplitModeMaxReq;

    #define SPLIT_MODE_5704_MAX_REQ                     3

    /* Init flag. */
    LM_BOOL InitDone;

    /* Shutdown flag.  Set by the upper module. */
    LM_BOOL ShuttingDown;

    /* Flag to determine whether to call LM_QueueRxPackets or not in */
    /* LM_ResetAdapter routine. */
    LM_BOOL QueueRxPackets;
    LM_BOOL QueueAgain;

    LM_UINT32 MbufBase;
    LM_UINT32 MbufSize;

    LM_UINT32 NvramSize;

#ifdef INCLUDE_TBI_SUPPORT
    /* Autoneg state info. */
    AN_STATE_INFO AnInfo;

    LM_UINT32 TbiFlags;
    /* set if we have a SERDES PHY. */
    #define ENABLE_TBI_FLAG            0x1
    #define TBI_POLLING_INTR_FLAG      0x2
    #define TBI_PURE_POLLING_FLAG      0x4
    #define TBI_POLLING_FLAGS   (TBI_POLLING_INTR_FLAG | TBI_PURE_POLLING_FLAG)
    #define TBI_DO_PREEMPHASIS         0x8

    LM_UINT32 IgnoreTbiLinkChange;
#endif
#ifdef BCM_NAPI_RXPOLL
    volatile LM_UINT32 RxPoll;
#endif
    char PartNo[24];
    char BootCodeVer[16];
    char IPMICodeVer[24];
    char BusSpeedStr[24];

    FLASHINFO flashinfo;
    LM_UINT8  flashbuffer[256];
} LM_DEVICE_BLOCK;


#define T3_REG_CPU_VIEW               0xc0000000

#define T3_BLOCK_DMA_RD               (1 << 0)
#define T3_BLOCK_DMA_COMP             (1 << 1)
#define T3_BLOCK_RX_BD_INITIATOR      (1 << 2)
#define T3_BLOCK_RX_BD_COMP           (1 << 3)
#define T3_BLOCK_DMA_WR               (1 << 4)
#define T3_BLOCK_MSI_HANDLER          (1 << 5)
#define T3_BLOCK_RX_LIST_PLMT         (1 << 6)
#define T3_BLOCK_RX_LIST_SELECTOR     (1 << 7)
#define T3_BLOCK_RX_DATA_INITIATOR    (1 << 8)
#define T3_BLOCK_RX_DATA_COMP         (1 << 9)
#define T3_BLOCK_HOST_COALESING       (1 << 10)
#define T3_BLOCK_MAC_RX_ENGINE        (1 << 11)
#define T3_BLOCK_MBUF_CLUSTER_FREE    (1 << 12)
#define T3_BLOCK_SEND_BD_INITIATOR    (1 << 13)
#define T3_BLOCK_SEND_BD_COMP         (1 << 14)
#define T3_BLOCK_SEND_BD_SELECTOR     (1 << 15)
#define T3_BLOCK_SEND_DATA_INITIATOR  (1 << 16)
#define T3_BLOCK_SEND_DATA_COMP       (1 << 17)
#define T3_BLOCK_MAC_TX_ENGINE        (1 << 18)
#define T3_BLOCK_MEM_ARBITOR          (1 << 19)
#define T3_BLOCK_MBUF_MANAGER         (1 << 20)
#define T3_BLOCK_MAC_GLOBAL           (1 << 21)

#define LM_ENABLE               1
#define LM_DISABLE              2

#define RX_CPU_EVT_SW0              0
#define RX_CPU_EVT_SW1              1
#define RX_CPU_EVT_RLP              2
#define RX_CPU_EVT_SW3              3
#define RX_CPU_EVT_RLS              4
#define RX_CPU_EVT_SW4              5
#define RX_CPU_EVT_RX_BD_COMP       6
#define RX_CPU_EVT_SW5              7
#define RX_CPU_EVT_RDI              8
#define RX_CPU_EVT_DMA_WR           9
#define RX_CPU_EVT_DMA_RD           10
#define RX_CPU_EVT_SWQ              11
#define RX_CPU_EVT_SW6              12
#define RX_CPU_EVT_RDC              13
#define RX_CPU_EVT_SW7              14
#define RX_CPU_EVT_HOST_COALES      15
#define RX_CPU_EVT_SW8              16
#define RX_CPU_EVT_HIGH_DMA_WR      17
#define RX_CPU_EVT_HIGH_DMA_RD      18
#define RX_CPU_EVT_SW9              19
#define RX_CPU_EVT_DMA_ATTN         20
#define RX_CPU_EVT_LOW_P_MBOX       21
#define RX_CPU_EVT_HIGH_P_MBOX      22
#define RX_CPU_EVT_SW10             23
#define RX_CPU_EVT_TX_CPU_ATTN      24
#define RX_CPU_EVT_MAC_ATTN         25
#define RX_CPU_EVT_RX_CPU_ATTN      26
#define RX_CPU_EVT_FLOW_ATTN        27
#define RX_CPU_EVT_SW11             28
#define RX_CPU_EVT_TIMER            29
#define RX_CPU_EVT_SW12             30
#define RX_CPU_EVT_SW13             31

/* RX-CPU event */
#define RX_CPU_EVENT_SW_EVENT0      (1 << RX_CPU_EVT_SW0)
#define RX_CPU_EVENT_SW_EVENT1      (1 << RX_CPU_EVT_SW1)
#define RX_CPU_EVENT_RLP            (1 << RX_CPU_EVT_RLP)
#define RX_CPU_EVENT_SW_EVENT3      (1 << RX_CPU_EVT_SW3)
#define RX_CPU_EVENT_RLS            (1 << RX_CPU_EVT_RLS)
#define RX_CPU_EVENT_SW_EVENT4      (1 << RX_CPU_EVT_SW4)
#define RX_CPU_EVENT_RX_BD_COMP     (1 << RX_CPU_EVT_RX_BD_COMP)
#define RX_CPU_EVENT_SW_EVENT5      (1 << RX_CPU_EVT_SW5)
#define RX_CPU_EVENT_RDI            (1 << RX_CPU_EVT_RDI)
#define RX_CPU_EVENT_DMA_WR         (1 << RX_CPU_EVT_DMA_WR)
#define RX_CPU_EVENT_DMA_RD         (1 << RX_CPU_EVT_DMA_RD)
#define RX_CPU_EVENT_SWQ            (1 << RX_CPU_EVT_SWQ)
#define RX_CPU_EVENT_SW_EVENT6      (1 << RX_CPU_EVT_SW6)
#define RX_CPU_EVENT_RDC            (1 << RX_CPU_EVT_RDC)
#define RX_CPU_EVENT_SW_EVENT7      (1 << RX_CPU_EVT_SW7)
#define RX_CPU_EVENT_HOST_COALES    (1 << RX_CPU_EVT_HOST_COALES)
#define RX_CPU_EVENT_SW_EVENT8      (1 << RX_CPU_EVT_SW8)
#define RX_CPU_EVENT_HIGH_DMA_WR    (1 << RX_CPU_EVT_HIGH_DMA_WR)
#define RX_CPU_EVENT_HIGH_DMA_RD    (1 << RX_CPU_EVT_HIGH_DMA_RD)
#define RX_CPU_EVENT_SW_EVENT9      (1 << RX_CPU_EVT_SW9)
#define RX_CPU_EVENT_DMA_ATTN       (1 << RX_CPU_EVT_DMA_ATTN)
#define RX_CPU_EVENT_LOW_P_MBOX     (1 << RX_CPU_EVT_LOW_P_MBOX)
#define RX_CPU_EVENT_HIGH_P_MBOX    (1 << RX_CPU_EVT_HIGH_P_MBOX)
#define RX_CPU_EVENT_SW_EVENT10     (1 << RX_CPU_EVT_SW10)
#define RX_CPU_EVENT_TX_CPU_ATTN    (1 << RX_CPU_EVT_TX_CPU_ATTN)
#define RX_CPU_EVENT_MAC_ATTN       (1 << RX_CPU_EVT_MAC_ATTN)
#define RX_CPU_EVENT_RX_CPU_ATTN    (1 << RX_CPU_EVT_RX_CPU_ATTN)
#define RX_CPU_EVENT_FLOW_ATTN      (1 << RX_CPU_EVT_FLOW_ATTN)
#define RX_CPU_EVENT_SW_EVENT11     (1 << RX_CPU_EVT_SW11)
#define RX_CPU_EVENT_TIMER          (1 << RX_CPU_EVT_TIMER)
#define RX_CPU_EVENT_SW_EVENT12     (1 << RX_CPU_EVT_SW12)
#define RX_CPU_EVENT_SW_EVENT13     (1 << RX_CPU_EVT_SW13)

#define RX_CPU_MASK (RX_CPU_EVENT_SW_EVENT0 | \
		     RX_CPU_EVENT_RLP | \
		     RX_CPU_EVENT_RDI | \
		     RX_CPU_EVENT_RDC)

#define TX_CPU_EVT_SW0              0
#define TX_CPU_EVT_SW1              1
#define TX_CPU_EVT_SW2              2
#define TX_CPU_EVT_SW3              3
#define TX_CPU_EVT_TX_MAC           4
#define TX_CPU_EVT_SW4              5
#define TX_CPU_EVT_SBDC             6
#define TX_CPU_EVT_SW5              7
#define TX_CPU_EVT_SDI              8
#define TX_CPU_EVT_DMA_WR           9
#define TX_CPU_EVT_DMA_RD           10
#define TX_CPU_EVT_SWQ              11
#define TX_CPU_EVT_SW6              12
#define TX_CPU_EVT_SDC              13
#define TX_CPU_EVT_SW7              14
#define TX_CPU_EVT_HOST_COALES      15
#define TX_CPU_EVT_SW8              16
#define TX_CPU_EVT_HIGH_DMA_WR      17
#define TX_CPU_EVT_HIGH_DMA_RD      18
#define TX_CPU_EVT_SW9              19
#define TX_CPU_EVT_DMA_ATTN         20
#define TX_CPU_EVT_LOW_P_MBOX       21
#define TX_CPU_EVT_HIGH_P_MBOX      22
#define TX_CPU_EVT_SW10             23
#define TX_CPU_EVT_RX_CPU_ATTN      24
#define TX_CPU_EVT_MAC_ATTN         25
#define TX_CPU_EVT_TX_CPU_ATTN      26
#define TX_CPU_EVT_FLOW_ATTN        27
#define TX_CPU_EVT_SW11             28
#define TX_CPU_EVT_TIMER            29
#define TX_CPU_EVT_SW12             30
#define TX_CPU_EVT_SW13             31


/* TX-CPU event */
#define TX_CPU_EVENT_SW_EVENT0      (1 << TX_CPU_EVT_SW0)
#define TX_CPU_EVENT_SW_EVENT1      (1 << TX_CPU_EVT_SW1)
#define TX_CPU_EVENT_SW_EVENT2      (1 << TX_CPU_EVT_SW2)
#define TX_CPU_EVENT_SW_EVENT3      (1 << TX_CPU_EVT_SW3)
#define TX_CPU_EVENT_TX_MAC         (1 << TX_CPU_EVT_TX_MAC)
#define TX_CPU_EVENT_SW_EVENT4      (1 << TX_CPU_EVT_SW4)
#define TX_CPU_EVENT_SBDC           (1 << TX_CPU_EVT_SBDC)
#define TX_CPU_EVENT_SW_EVENT5      (1 << TX_CPU_EVT_SW5)
#define TX_CPU_EVENT_SDI            (1 << TX_CPU_EVT_SDI)
#define TX_CPU_EVENT_DMA_WR         (1 << TX_CPU_EVT_DMA_WR)
#define TX_CPU_EVENT_DMA_RD         (1 << TX_CPU_EVT_DMA_RD)
#define TX_CPU_EVENT_SWQ            (1 << TX_CPU_EVT_SWQ)
#define TX_CPU_EVENT_SW_EVENT6      (1 << TX_CPU_EVT_SW6)
#define TX_CPU_EVENT_SDC            (1 << TX_CPU_EVT_SDC)
#define TX_CPU_EVENT_SW_EVENT7      (1 << TX_CPU_EVT_SW7)
#define TX_CPU_EVENT_HOST_COALES    (1 << TX_CPU_EVT_HOST_COALES)
#define TX_CPU_EVENT_SW_EVENT8      (1 << TX_CPU_EVT_SW8)
#define TX_CPU_EVENT_HIGH_DMA_WR    (1 << TX_CPU_EVT_HIGH_DMA_WR)
#define TX_CPU_EVENT_HIGH_DMA_RD    (1 << TX_CPU_EVT_HIGH_DMA_RD)
#define TX_CPU_EVENT_SW_EVENT9      (1 << TX_CPU_EVT_SW9)
#define TX_CPU_EVENT_DMA_ATTN       (1 << TX_CPU_EVT_DMA_ATTN)
#define TX_CPU_EVENT_LOW_P_MBOX     (1 << TX_CPU_EVT_LOW_P_MBOX)
#define TX_CPU_EVENT_HIGH_P_MBOX    (1 << TX_CPU_EVT_HIGH_P_MBOX)
#define TX_CPU_EVENT_SW_EVENT10     (1 << TX_CPU_EVT_SW10)
#define TX_CPU_EVENT_RX_CPU_ATTN    (1 << TX_CPU_EVT_RX_CPU_ATTN)
#define TX_CPU_EVENT_MAC_ATTN       (1 << TX_CPU_EVT_MAC_ATTN)
#define TX_CPU_EVENT_TX_CPU_ATTN    (1 << TX_CPU_EVT_TX_CPU_ATTN)
#define TX_CPU_EVENT_FLOW_ATTN      (1 << TX_CPU_EVT_FLOW_ATTN)
#define TX_CPU_EVENT_SW_EVENT11     (1 << TX_CPU_EVT_SW11)
#define TX_CPU_EVENT_TIMER          (1 << TX_CPU_EVT_TIMER)
#define TX_CPU_EVENT_SW_EVENT12     (1 << TX_CPU_EVT_SW12)
#define TX_CPU_EVENT_SW_EVENT13     (1 << TX_CPU_EVT_SW13)


#define TX_CPU_MASK (TX_CPU_EVENT_SW_EVENT0 | \
		     TX_CPU_EVENT_SDI  | \
		     TX_CPU_EVENT_SDC)


#define T3_FTQ_TYPE1_UNDERFLOW_BIT   (1 << 29)
#define T3_FTQ_TYPE1_PASS_BIT        (1 << 30)
#define T3_FTQ_TYPE1_SKIP_BIT        (1 << 31)

#define T3_FTQ_TYPE2_UNDERFLOW_BIT   (1 << 13)
#define T3_FTQ_TYPE2_PASS_BIT        (1 << 14)
#define T3_FTQ_TYPE2_SKIP_BIT        (1 << 15)

#define T3_QID_DMA_READ               1
#define T3_QID_DMA_HIGH_PRI_READ      2
#define T3_QID_DMA_COMP_DX            3
#define T3_QID_SEND_BD_COMP           4
#define T3_QID_SEND_DATA_INITIATOR    5
#define T3_QID_DMA_WRITE              6
#define T3_QID_DMA_HIGH_PRI_WRITE     7
#define T3_QID_SW_TYPE_1              8
#define T3_QID_SEND_DATA_COMP         9
#define T3_QID_HOST_COALESCING        10
#define T3_QID_MAC_TX                 11
#define T3_QID_MBUF_CLUSTER_FREE      12
#define T3_QID_RX_BD_COMP             13
#define T3_QID_RX_LIST_PLM            14
#define T3_QID_RX_DATA_BD_INITIATOR   15
#define T3_QID_RX_DATA_COMP           16
#define T3_QID_SW_TYPE2               17

LM_STATUS LM_LoadFirmware(PLM_DEVICE_BLOCK pDevice,
                          PT3_FWIMG_INFO pFwImg,
                          LM_UINT32 LoadCpu,
                          LM_UINT32 StartCpu);

/******************************************************************************/
/* NIC register read/write macros. */
/******************************************************************************/

/* MAC register access. */
LM_UINT32 LM_RegRd(PLM_DEVICE_BLOCK pDevice, LM_UINT32 Register);

LM_VOID LM_RegRdBack(PLM_DEVICE_BLOCK pDevice, LM_UINT32 Register);

LM_VOID LM_RegWr(PLM_DEVICE_BLOCK pDevice, LM_UINT32 Register,
    LM_UINT32 Value32, LM_UINT32 ReadBack);

LM_UINT32 LM_RegRdInd(PLM_DEVICE_BLOCK pDevice, LM_UINT32 Register);
LM_VOID LM_RegWrInd(PLM_DEVICE_BLOCK pDevice, LM_UINT32 Register,
    LM_UINT32 Value32);

/* MAC memory access. */
LM_UINT32 LM_MemRdInd(PLM_DEVICE_BLOCK pDevice, LM_UINT32 MemAddr);

LM_VOID LM_MemWrInd(PLM_DEVICE_BLOCK pDevice, LM_UINT32 MemAddr,
    LM_UINT32 Value32);

#define MB_REG_WR(pDevice, OffsetName, Value32)                               \
    ((pDevice)->Flags & UNDI_FIX_FLAG) ?                                      \
        LM_RegWrInd(pDevice, OFFSETOF(T3_STD_MEM_MAP, OffsetName)+0x5600,     \
            Value32) :                                                        \
        (void) MM_MEMWRITEL(&((pDevice)->pMemView->OffsetName), Value32)

#define MB_REG_RD(pDevice, OffsetName)                                        \
    (((pDevice)->Flags & UNDI_FIX_FLAG) ?                                     \
        LM_RegRdInd(pDevice, OFFSETOF(T3_STD_MEM_MAP, OffsetName)+0x5600) :   \
        MM_MEMREADL(&((pDevice)->pMemView->OffsetName)))

#define REG_RD(pDevice, OffsetName)                                         \
    LM_RegRd(pDevice, OFFSETOF(T3_STD_MEM_MAP, OffsetName))

#define REG_RD_BACK(pDevice, OffsetName)                                    \
    LM_RegRdBack(pDevice, OFFSETOF(T3_STD_MEM_MAP, OffsetName))

#define REG_WR(pDevice, OffsetName, Value32)                                \
    LM_RegWr(pDevice, OFFSETOF(T3_STD_MEM_MAP, OffsetName), Value32, TRUE)

#define RAW_REG_WR(pDevice, OffsetName, Value32)                            \
    LM_RegWr(pDevice, OFFSETOF(T3_STD_MEM_MAP, OffsetName), Value32, FALSE)

#define REG_RD_OFFSET(pDevice, Offset)                                      \
    MM_MEMREADL(((LM_UINT8 *) (pDevice)->pMemView + Offset))

#define REG_WR_OFFSET(pDevice, Offset, Value32)                             \
    MM_MEMWRITEL(((LM_UINT8 *) (pDevice)->pMemView + Offset), Value32)

#define MEM_RD(pDevice, AddrName)                                           \
    LM_MemRdInd(pDevice, OFFSETOF(T3_FIRST_32K_SRAM, AddrName))
#define MEM_WR(pDevice, AddrName, Value32)                                  \
    LM_MemWrInd(pDevice, OFFSETOF(T3_FIRST_32K_SRAM, AddrName), Value32)

#define MEM_RD_OFFSET(pDevice, Offset)                                      \
    LM_MemRdInd(pDevice, Offset)
#define MEM_WR_OFFSET(pDevice, Offset, Value32)                             \
    LM_MemWrInd(pDevice, Offset, Value32)
				

#endif /* TIGON3_H */

