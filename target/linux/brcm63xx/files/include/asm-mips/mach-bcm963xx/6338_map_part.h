/*
<:copyright-gpl 
 Copyright 2004 Broadcom Corp. All Rights Reserved. 
 
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

#ifndef __BCM6338_MAP_H
#define __BCM6338_MAP_H

#include "bcmtypes.h"

#define PERF_BASE           0xfffe0000
#define TIMR_BASE           0xfffe0200 
#define UART_BASE           0xfffe0300
#define GPIO_BASE           0xfffe0400
#define SPI_BASE            0xfffe0c00

typedef struct PerfControl {
  uint32        RevID;
  uint16        testControl;
  uint16        blkEnables;
#define EMAC_CLK_EN     0x0010
#define USBS_CLK_EN     0x0010
#define SAR_CLK_EN      0x0020

#define SPI_CLK_EN      0x0200

  uint32        pll_control;
#define SOFT_RESET 0x00000001

  uint32        IrqMask;
  uint32        IrqStatus;

  uint32        ExtIrqCfg;
#define EI_SENSE_SHFT   0
#define EI_STATUS_SHFT  5
#define EI_CLEAR_SHFT   10
#define EI_MASK_SHFT    15
#define EI_INSENS_SHFT  20
#define EI_LEVEL_SHFT   25

  uint32        unused[4];      /* (18) */
  uint32        BlockSoftReset; /* (28) */
#define BSR_SPI             0x00000001
#define BSR_EMAC            0x00000004
#define BSR_USBH            0x00000008
#define BSR_USBS            0x00000010
#define BSR_ADSL            0x00000020
#define BSR_DMAMEM          0x00000040
#define BSR_SAR             0x00000080
#define BSR_ACLC            0x00000100
#define BSR_ADSL_MIPS_PLL   0x00000400
#define BSR_ALL_BLOCKS      \
    (BSR_SPI | BSR_EMAC | BSR_USBH | BSR_USBS | BSR_ADSL | BSR_DMAMEM | \
     BSR_SAR | BSR_ACLC | BSR_ADSL_MIPS_PLL) 
} PerfControl;

#define PERF ((volatile PerfControl * const) PERF_BASE)


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
#define LOOPBK          0x10
#define TXPARITYEN      0x08
#define TXPARITYEVEN    0x04
#define RXPARITYEN      0x02
#define RXPARITYEVEN    0x01

  byte          config;
#define XMITBREAK       0x40
#define BITS5SYM        0x00
#define BITS6SYM        0x10
#define BITS7SYM        0x20
#define BITS8SYM        0x30
#define ONESTOP         0x07
#define TWOSTOP         0x0f
  /* 4-LSBS represent STOP bits/char
   * in 1/8 bit-time intervals.  Zero
   * represents 1/8 stop bit interval.
   * Fifteen represents 2 stop bits.
   */
  byte          fifoctl;
#define RSTTXFIFOS      0x80
#define RSTRXFIFOS      0x40
  /* 5-bit TimeoutCnt is in low bits of this register.
   *  This count represents the number of characters 
   *  idle times before setting receive Irq when below threshold
   */
  uint32        baudword;
  /* When divide SysClk/2/(1+baudword) we should get 32*bit-rate
   */

  byte          txf_levl;       /* Read-only fifo depth */
  byte          rxf_levl;       /* Read-only fifo depth */
  byte          fifocfg;        /* Upper 4-bits are TxThresh, Lower are
                                 *      RxThreshold.  Irq can be asserted
                                 *      when rx fifo> thresh, txfifo<thresh
                                 */
  byte          prog_out;       /* Set value of DTR (Bit0), RTS (Bit1)
                                 *  if these bits are also enabled to GPIO_o
                                 */
#define	DTREN	0x01
#define	RTSEN	0x02

  byte          unused1;
  byte          DeltaIPEdgeNoSense;     /* Low 4-bits, set corr bit to 1 to 
                                         * detect irq on rising AND falling 
                                         * edges for corresponding GPIO_i
                                         * if enabled (edge insensitive)
                                         */
  byte          DeltaIPConfig_Mask;     /* Upper 4 bits: 1 for posedge sense
                                         *      0 for negedge sense if
                                         *      not configured for edge
                                         *      insensitive (see above)
                                         * Lower 4 bits: Mask to enable change
                                         *  detection IRQ for corresponding
                                         *  GPIO_i
                                         */
  byte          DeltaIP_SyncIP;         /* Upper 4 bits show which bits
                                         *  have changed (may set IRQ).
                                         *  read automatically clears bit
                                         * Lower 4 bits are actual status
                                         */

  uint16        intMask;				/* Same Bit defs for Mask and status */
  uint16        intStatus;
#define DELTAIP         0x0001
#define TXUNDERR        0x0002
#define TXOVFERR        0x0004
#define TXFIFOTHOLD     0x0008
#define TXREADLATCH     0x0010
#define TXFIFOEMT       0x0020
#define RXUNDERR        0x0040
#define RXOVFERR        0x0080
#define RXTIMEOUT       0x0100
#define RXFIFOFULL      0x0200
#define RXFIFOTHOLD     0x0400
#define RXFIFONE        0x0800
#define RXFRAMERR       0x1000
#define RXPARERR        0x2000
#define RXBRK           0x4000

  uint16        unused2;
  uint16        Data;                   /* Write to TX, Read from RX */
                                        /* bits 11:8 are BRK,PAR,FRM errors */

  uint32		unused3;
  uint32		unused4;
} Uart;

#define UART ((volatile Uart * const) UART_BASE)

typedef struct GpioControl {
  uint32        unused0;
  uint32        GPIODir;      /* bits 7:0 */
  uint32        unused1;
  uint32        GPIOio;       /* bits 7:0 */
  uint32        LEDCtrl;
#define         LED3_STROBE             0x08000000
#define         LED2_STROBE             0x04000000
#define         LED1_STROBE             0x02000000
#define         LED0_STROBE             0x01000000
#define         LED_TEST                0x00010000
#define         LED3_DISABLE_LINK_ACT   0x00008000
#define         LED2_DISABLE_LINK_ACT   0x00004000
#define         LED1_DISABLE_LINK_ACT   0x00002000
#define         LED0_DISABLE_LINK_ACT   0x00001000
#define         LED_INTERVAL_SET_MASK   0x00000f00
#define         LED_INTERVAL_SET_320MS  0x00000500
#define         LED_INTERVAL_SET_160MS  0x00000400
#define         LED_INTERVAL_SET_80MS   0x00000300
#define         LED_INTERVAL_SET_40MS   0x00000200
#define         LED_INTERVAL_SET_20MS   0x00000100
#define         LED3_ON                 0x00000080
#define         LED2_ON                 0x00000040
#define         LED1_ON                 0x00000020
#define         LED0_ON                 0x00000010
#define         LED3_ENABLE             0x00000008
#define         LED2_ENABLE             0x00000004
#define         LED1_ENABLE             0x00000002
#define         LED0_ENABLE             0x00000001
  uint32        SpiSlaveCfg;
#define         SPI_SLAVE_RESET         0x00010000
#define         SPI_RESTRICT            0x00000400
#define         SPI_DELAY_DISABLE       0x00000200
#define         SPI_PROBE_MUX_SEL_MASK  0x000001e0
#define         SPI_SER_ADDR_CFG_MASK   0x0000000c
#define         SPI_MODE                0x00000001
  uint32        vRegConfig;
} GpioControl;

#define GPIO ((volatile GpioControl * const) GPIO_BASE)

/* Number to mask conversion macro used for GPIODir and GPIOio */
#define GPIO_NUM_MAX_BITS_MASK          0x0f
#define GPIO_NUM_TO_MASK(X)             (1 << ((X) & GPIO_NUM_MAX_BITS_MASK))

/*
** Spi Controller
*/

typedef struct SpiControl {
  uint16        spiCmd;                 /* (0x0): SPI command */
#define SPI_CMD_START_IMMEDIATE         3

#define SPI_CMD_COMMAND_SHIFT           0
#define SPI_CMD_DEVICE_ID_SHIFT         4
#define SPI_CMD_PREPEND_BYTE_CNT_SHIFT  8

  byte          spiIntStatus;           /* (0x2): SPI interrupt status */
  byte          spiMaskIntStatus;       /* (0x3): SPI masked interrupt status */

  byte          spiIntMask;             /* (0x4): SPI interrupt mask */
#define SPI_INTR_CMD_DONE               0x01
#define SPI_INTR_CLEAR_ALL              0x1f

  byte          spiStatus;              /* (0x5): SPI status */

  byte          spiClkCfg;              /* (0x6): SPI clock configuration */

  byte          spiFillByte;            /* (0x7): SPI fill byte */

  byte          unused0; 
  byte          spiMsgTail;             /* (0x9): msgtail */
  byte          unused1; 
  byte          spiRxTail;              /* (0xB): rxtail */

  uint32        unused2[13];            /* (0x0c - 0x3c) reserved */

  byte          spiMsgCtl;              /* (0x40) control byte */
#define HALF_DUPLEX_W                   1
#define HALF_DUPLEX_R                   2
#define SPI_MSG_TYPE_SHIFT              6
#define SPI_BYTE_CNT_SHIFT              0
  byte          spiMsgData[63];         /* (0x41 - 0x7f) msg data */
  byte          spiRxDataFifo[64];      /* (0x80 - 0xbf) rx data */
  byte          unused3[64];            /* (0xc0 - 0xff) reserved */
} SpiControl;

#define SPI ((volatile SpiControl * const) SPI_BASE)

/*
** External Bus Interface
*/
typedef struct EbiChipSelect {
  uint32        base;                   /* base address in upper 24 bits */
#define EBI_SIZE_8K         0
#define EBI_SIZE_16K        1
#define EBI_SIZE_32K        2
#define EBI_SIZE_64K        3
#define EBI_SIZE_128K       4
#define EBI_SIZE_256K       5
#define EBI_SIZE_512K       6
#define EBI_SIZE_1M         7
#define EBI_SIZE_2M         8
#define EBI_SIZE_4M         9
#define EBI_SIZE_8M         10
#define EBI_SIZE_16M        11
#define EBI_SIZE_32M        12
#define EBI_SIZE_64M        13
#define EBI_SIZE_128M       14
#define EBI_SIZE_256M       15
  uint32        config;
#define EBI_ENABLE          0x00000001      /* .. enable this range */
#define EBI_WAIT_STATES     0x0000000e      /* .. mask for wait states */
#define EBI_WTST_SHIFT      1               /* .. for shifting wait states */
#define EBI_WORD_WIDE       0x00000010      /* .. 16-bit peripheral, else 8 */
#define EBI_WREN            0x00000020      /* enable posted writes */
#define EBI_POLARITY        0x00000040      /* .. set to invert something, 
                                        **    don't know what yet */
#define EBI_TS_TA_MODE      0x00000080      /* .. use TS/TA mode */
#define EBI_TS_SEL          0x00000100      /* .. drive tsize, not bs_b */
#define EBI_FIFO            0x00000200      /* .. use fifo */
#define EBI_RE              0x00000400      /* .. Reverse Endian */
} EbiChipSelect;

typedef struct MpiRegisters {
  EbiChipSelect cs[1];                  /* size chip select configuration */
} MpiRegisters;

#define MPI ((volatile MpiRegisters * const) MPI_BASE)


#endif

