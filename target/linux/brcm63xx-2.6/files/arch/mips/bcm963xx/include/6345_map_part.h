/*
<:copyright-gpl 
 Copyright 2002 Broadcom Corp. All Rights Reserved. 
 
 This program is free software; you can distribute it and/or modify it 
 under the terms of the GNU General Public License (Version 2) as 
 published by the Free Software Foundation. 
 
 This program is distributed in the hope it will be useful, but WITHOUT 
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
 for more details. 
 
 You should have received a copy of the GNU General Public License along 
 with this program; if not, write to the Free Software Foundation, Inc., 
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA. 
:>
*/

#ifndef __BCM6345_MAP_H
#define __BCM6345_MAP_H


#include "bcmtypes.h"
#include "6345_intr.h"

typedef struct IntControl {
  uint32        RevID;
  uint16        testControl;
  uint16        blkEnables;
#define USB_CLK_EN      0x0100
#define EMAC_CLK_EN     0x0080
#define UART_CLK_EN     0x0008
#define CPU_CLK_EN      0x0001

  uint32        pll_control;
#define SOFT_RESET	0x00000001

  uint32        IrqMask;
  uint32        IrqStatus;

  uint32        ExtIrqCfg;
#define EI_SENSE_SHFT   0
#define EI_STATUS_SHFT  4
#define EI_CLEAR_SHFT   8
#define EI_MASK_SHFT    12
#define EI_INSENS_SHFT  16
#define EI_LEVEL_SHFT   20
} IntControl;

#define INTC_BASE     0xfffe0000
#define PERF ((volatile IntControl * const) INTC_BASE)

#define TIMR_BASE     0xfffe0200    
typedef struct Timer {
  uint16        unused0;
  byte          TimerMask;
#define TIMER0EN        0x01
#define TIMER1EN        0x02
#define TIMER2EN        0x04
  byte          TimerInts;
#define TIMER0          0x01
#define TIMER1          0x02
#define TIMER2          0x04
#define WATCHDOG        0x08
  uint32        TimerCtl0;
  uint32        TimerCtl1;
  uint32        TimerCtl2;
#define TIMERENABLE     0x80000000
#define RSTCNTCLR       0x40000000      
  uint32        TimerCnt0;
  uint32        TimerCnt1;
  uint32        TimerCnt2;
  uint32        WatchDogDefCount;

  /* Write 0xff00 0x00ff to Start timer
   * Write 0xee00 0x00ee to Stop and re-load default count
   * Read from this register returns current watch dog count
   */
  uint32        WatchDogCtl;

  /* Number of 40-MHz ticks for WD Reset pulse to last */
  uint32        WDResetCount;
} Timer;

#define TIMER ((volatile Timer * const) TIMR_BASE)

typedef struct UartChannel {
  byte          unused0;
  byte          control;
#define BRGEN           0x80    /* Control register bit defs */
#define TXEN            0x40
#define RXEN            0x20
#define TXPARITYEN      0x08
#define TXPARITYEVEN    0x04
#define RXPARITYEN      0x02
#define RXPARITYEVEN    0x01
  byte          config;
#define BITS5SYM        0x00
#define BITS6SYM        0x10
#define BITS7SYM        0x20
#define BITS8SYM        0x30
#define XMITBREAK       0x40
#define ONESTOP         0x07
#define TWOSTOP         0x0f

  byte          fifoctl;
#define RSTTXFIFOS      0x80
#define RSTRXFIFOS      0x40
  uint32        baudword;

  byte          txf_levl;
  byte          rxf_levl;
  byte          fifocfg;
  byte          prog_out;

  byte          unused1;
  byte          DeltaIPEdgeNoSense;
  byte          DeltaIPConfig_Mask;
  byte          DeltaIP_SyncIP;
  uint16        intMask;
  uint16        intStatus;
#define TXUNDERR        0x0002
#define TXOVFERR        0x0004
#define TXFIFOEMT       0x0020
#define RXOVFERR        0x0080
#define RXFIFONE        0x0800
#define RXFRAMERR       0x1000
#define RXPARERR        0x2000
#define RXBRK           0x4000

  uint16        unused2;
  uint16        Data;
  uint32		unused3;
  uint32		unused4;
} Uart;

#define UART_BASE     0xfffe0300
#define UART ((volatile Uart * const) UART_BASE)

typedef struct GpioControl {
  uint16        unused0;
  byte          unused1;
  byte          TBusSel;

  uint16        unused2;
  uint16        GPIODir;
  byte          unused3;
  byte          Leds;
  uint16        GPIOio;

  uint32        UartCtl;
} GpioControl;

#define GPIO_BASE     0xfffe0400
#define GPIO ((volatile GpioControl * const) GPIO_BASE)

#define GPIO_NUM_MAX_BITS_MASK          0x0f
#define GPIO_NUM_TO_MASK(X)             (1 << ((X) & GPIO_NUM_MAX_BITS_MASK))


#endif

