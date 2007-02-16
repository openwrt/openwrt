/********************************************************************************
   Title:   $Source: platform.h,v $

   Author:  Dan Steinberg
            Copyright Integrated Device Technology 2001

   Purpose: AR2313 Register/Bit Definitions
   
   Update:
            $Log: platform.h,v $
      
   Notes:   See Merlot architecture spec for complete details.  Note, all
            addresses are virtual addresses in kseg1 (Uncached, Unmapped).
 
********************************************************************************/

#ifndef PLATFORM_H
#define PLATFORM_H

//
// New Combo structure for Both Eth0 AND eth1
//
typedef struct {
  volatile unsigned int mac_control;    /* 0x00 */
  volatile unsigned int mac_addr[2];    /* 0x04 - 0x08*/
  volatile unsigned int mcast_table[2]; /* 0x0c - 0x10 */
  volatile unsigned int mii_addr;       /* 0x14 */
  volatile unsigned int mii_data;       /* 0x18 */
  volatile unsigned int flow_control;   /* 0x1c */
  volatile unsigned int vlan_tag;       /* 0x20 */
  volatile unsigned int pad[7];         /* 0x24 - 0x3c */
  volatile unsigned int ucast_table[8]; /* 0x40-0x5c */
  
} ETHERNET_STRUCT;

/********************************************************************
 * Interrupt controller 
 ********************************************************************/

typedef struct {
  volatile unsigned int wdog_control;		/* 0x08 */
  volatile unsigned int wdog_timer;		/* 0x0c */
  volatile unsigned int misc_status;		/* 0x10 */
  volatile unsigned int misc_mask;		/* 0x14 */
  volatile unsigned int global_status;		/* 0x18 */
  volatile unsigned int reserved;		/* 0x1c */
  volatile unsigned int reset_control;		/* 0x20 */
} INTERRUPT;

/********************************************************************
 * DMA controller
 ********************************************************************/
typedef struct {
  volatile unsigned int bus_mode;        /* 0x00 (CSR0) */
  volatile unsigned int xmt_poll;        /* 0x04 (CSR1) */
  volatile unsigned int rcv_poll;        /* 0x08 (CSR2) */
  volatile unsigned int rcv_base;        /* 0x0c (CSR3) */
  volatile unsigned int xmt_base;        /* 0x10 (CSR4) */
  volatile unsigned int status;          /* 0x14 (CSR5) */
  volatile unsigned int control;         /* 0x18 (CSR6) */
  volatile unsigned int intr_ena;        /* 0x1c (CSR7) */
  volatile unsigned int rcv_missed;      /* 0x20 (CSR8) */
  volatile unsigned int reserved[11];    /* 0x24-0x4c (CSR9-19) */
  volatile unsigned int cur_tx_buf_addr; /* 0x50 (CSR20) */
  volatile unsigned int cur_rx_buf_addr; /* 0x50 (CSR21) */
} DMA;

#endif /* PLATFORM_H */
