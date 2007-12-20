#ifndef DANUBE_SW_H
#define DANUBE_SW_H



/******************************************************************************
**
** FILE NAME    : danube_sw.h
** PROJECT      : Danube
** MODULES     	: ETH Interface (MII0)
**
** DATE         : 11 AUG 2005
** AUTHOR       : Wu Qi Ming
** DESCRIPTION  : ETH Interface (MII0) Driver Header File
** COPYRIGHT    : 	Copyright (c) 2006
**			Infineon Technologies AG
**			Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 11 AUG 2005  Wu Qi Ming      Initiate Version
** 23 OCT 2006  Xu Liang        Add GPL header.
*******************************************************************************/


#define SET_ETH_SPEED_AUTO   SIOCDEVPRIVATE
#define SET_ETH_SPEED_10     SIOCDEVPRIVATE+1
#define SET_ETH_SPEED_100    SIOCDEVPRIVATE+2
#define SET_ETH_DUPLEX_AUTO  SIOCDEVPRIVATE+3
#define SET_ETH_DUPLEX_HALF  SIOCDEVPRIVATE+4
#define SET_ETH_DUPLEX_FULL  SIOCDEVPRIVATE+5
#define SET_ETH_REG          SIOCDEVPRIVATE+6
#define VLAN_TOOLS           SIOCDEVPRIVATE+7
#define MAC_TABLE_TOOLS      SIOCDEVPRIVATE+8
#define SET_VLAN_COS         SIOCDEVPRIVATE+9
#define SET_DSCP_COS         SIOCDEVPRIVATE+10
#define ENABLE_VLAN_CLASSIFICATION    SIOCDEVPRIVATE+11
#define DISABLE_VLAN_CLASSIFICATION   SIOCDEVPRIVATE+12
#define VLAN_CLASS_FIRST              SIOCDEVPRIVATE+13
#define VLAN_CLASS_SECOND             SIOCDEVPRIVATE+14
#define ENABLE_DSCP_CLASSIFICATION    SIOCDEVPRIVATE+15
#define DISABLE_DSCP_CLASSIFICATION   SIOCDEVPRIVATE+16
#define PASS_UNICAST_PACKETS          SIOCDEVPRIVATE+17
#define FILTER_UNICAST_PACKETS        SIOCDEVPRIVATE+18
#define KEEP_BROADCAST_PACKETS        SIOCDEVPRIVATE+19
#define DROP_BROADCAST_PACKETS        SIOCDEVPRIVATE+20
#define KEEP_MULTICAST_PACKETS        SIOCDEVPRIVATE+21
#define DROP_MULTICAST_PACKETS        SIOCDEVPRIVATE+22


/*===mac table commands==*/
#define RESET_MAC_TABLE     0
#define READ_MAC_ENTRY    1
#define WRITE_MAC_ENTRY   2
#define ADD_MAC_ENTRY     3

/*====vlan commands===*/

#define CHANGE_VLAN_CTRL     0
#define READ_VLAN_ENTRY      1
#define UPDATE_VLAN_ENTRY    2
#define CLEAR_VLAN_ENTRY     3
#define RESET_VLAN_TABLE     4
#define ADD_VLAN_ENTRY       5

/*
** MDIO constants.
*/

#define MDIO_BASE_STATUS_REG                0x1
#define MDIO_BASE_CONTROL_REG               0x0
#define MDIO_PHY_ID_HIGH_REG                0x2
#define MDIO_PHY_ID_LOW_REG                 0x3
#define MDIO_BC_NEGOTIATE                0x0200
#define MDIO_BC_FULL_DUPLEX_MASK         0x0100
#define MDIO_BC_AUTO_NEG_MASK            0x1000
#define MDIO_BC_SPEED_SELECT_MASK        0x2000
#define MDIO_STATUS_100_FD               0x4000
#define MDIO_STATUS_100_HD               0x2000
#define MDIO_STATUS_10_FD                0x1000
#define MDIO_STATUS_10_HD                0x0800
#define MDIO_STATUS_SPEED_DUPLEX_MASK	 0x7800
#define MDIO_ADVERTISMENT_REG               0x4
#define MDIO_ADVERT_100_FD                0x100
#define MDIO_ADVERT_100_HD                0x080
#define MDIO_ADVERT_10_FD                 0x040
#define MDIO_ADVERT_10_HD                 0x020
#define MDIO_LINK_UP_MASK                   0x4
#define MDIO_START                          0x1
#define MDIO_READ                           0x2
#define MDIO_WRITE                          0x1
#define MDIO_PREAMBLE              0xfffffffful

#define PHY_RESET                        0x8000
#define AUTO_NEGOTIATION_ENABLE          0X1000
#define AUTO_NEGOTIATION_COMPLETE          0x20
#define RESTART_AUTO_NEGOTIATION          0X200


/*ETOP_MDIO_CFG MASKS*/
#define SMRST_MASK 0X2000
#define PHYA1_MASK 0X1F00
#define PHYA0_MASK 0XF8
#define UMM1_MASK  0X4
#define UMM0_MASK  0X2

/*ETOP_MDIO_ACCESS MASKS*/
#define MDIO_RA_MASK    0X80000000
#define MDIO_RW_MASK    0X40000000


/*ENET_MAC_CFG MASKS*/
#define BP_MASK     1<<12
#define CGEN_MASK   1<<11
#define IFG_MASK    0x3F<<5
#define IPAUS_MASK  1<<4
#define EPAUS_MASK  1<<3
#define DUPLEX_MASK 1<<2
#define SPEED_MASK  0x2
#define LINK_MASK   1

/*ENETS_CoS_CFG MASKS*/
#define VLAN_MASK    2
#define DSCP_MASK    1

/*ENET_CFG MASKS*/
#define VL2_MASK     1<<29
#define FTUC_MASK    1<<25
#define DPBC_MASK    1<<24
#define DPMC_MASK    1<<23

#define PHY0_ADDR    0
#define PHY1_ADDR    1
#define P1M          0

#define DANUBE_SW_REG32(reg_num) *((volatile u32*)(reg_num))

#define OK 0;

#ifdef CONFIG_CPU_LITTLE_ENDIAN
typedef struct mac_table_entry{
   u64 mac_address:48;
   u64 p0:1;
   u64 p1:1;
   u64 p2:1;
   u64 cr:1;
   u64 ma_st:3;
   u64 res:9;
}_mac_table_entry;

typedef struct IFX_Switch_VLanTableEntry{
    u32 vlan_id:12;
    u32 mp0:1;
    u32 mp1:1;
    u32 mp2:1;
    u32 v:1;
    u32 res:16;
}_IFX_Switch_VLanTableEntry;

typedef struct mac_table_req{
    int cmd;
    int index;
    u32 data;
    u64 entry_value;
}_mac_table_req;

#else //not CONFIG_CPU_LITTLE_ENDIAN
typedef struct mac_table_entry{
   u64 mac_address:48;
   u64 p0:1;
   u64 p1:1;
   u64 p2:1;
   u64 cr:1;
   u64 ma_st:3;
   u64 res:9;
}_mac_table_entry;

typedef struct IFX_Switch_VLanTableEntry{
    u32 vlan_id:12;
    u32 mp0:1;
    u32 mp1:1;
    u32 mp2:1;
    u32 v:1;
    u32 res:16;
}_IFX_Switch_VLanTableEntry;


typedef struct mac_table_req{
    int cmd;
    int index;
    u32 data;
    u64 entry_value;
}_mac_table_req;

#endif //CONFIG_CPU_LITTLE_ENDIAN

typedef struct vlan_cos_req{
    int pri;
    int cos_value;
}_vlan_cos_req;

typedef struct dscp_cos_req{
    int dscp;
    int cos_value;
}_dscp_cos_req;


typedef struct vlan_req{
    int cmd;
    int index;
    u32 data;
    u32 entry_value;
}_vlan_req;

typedef struct data_req{
    int index;
    u32 value;
}_data_req;

enum duplex
{
   half,
   full,
   autoneg
};

struct switch_priv {
    struct net_device_stats stats;
    int rx_packetlen;
    u8 *rx_packetdata;
    int rx_status;
    int tx_packetlen;
#ifdef CONFIG_NET_HW_FLOWCONTROL
    int fc_bit;
#endif //CONFIG_NET_HW_FLOWCONTROL
    u8 *tx_packetdata;
    int tx_status;
    struct dma_device_info *dma_device;
    struct sk_buff *skb;
    spinlock_t lock;
    int mdio_phy_addr;
    int current_speed;
    int current_speed_selection;
    int rx_queue_len;
    int full_duplex;
    enum duplex current_duplex;
};

#endif //DANUBE_SW_H
