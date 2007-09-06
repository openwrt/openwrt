#ifndef IFX_SSC_DEFINES_H
#define IFX_SSC_DEFINES_H

#include "ifx_peripheral_definitions.h"

/* maximum SSC FIFO size */
#define IFX_SSC_MAX_FIFO_SIZE 32

/* register map of SSC  */

/* address of the Clock Control Register of the SSC */
#define IFX_SSC_CLC                 0x00000000
/* IFX_SSC_CLC register is significant in bits 23 downto 8 and in bits 5, 3, 2, 0  
   bit 1 is hardware modified*/
#define IFX_SSC_CLC_readmask  0x00FFFFEF
#define IFX_SSC_CLC_writemask 0x00FFFF3D
#define IFX_SSC_CLC_hwmask    0x00000002
#define IFX_SSC_CLC_dontcare (IFX_SSC_CLC_readmask & IFX_SSC_CLC_writemask & ~IFX_SSC_CLC_hwmask)

/* address of Port Input Select Register of the SSC */
#define IFX_SSC_PISEL 0x00000004
/* IFX_SSC_PISEL register is significant in lowest three bits only */
#define IFX_SSC_PISEL_readmask  0x00000007
#define IFX_SSC_PISEL_writemask 0x00000007
#define IFX_SSC_PISEL_hwmask    0x00000000
#define IFX_SSC_PISEL_dontcare (IFX_SSC_PISEL_readmask & IFX_SSC_PISEL_writemask & ~IFX_SSC_PISEL_hwmask)

/* address of Identification Register of the SSC */
#define IFX_SSC_ID 0x00000008
/* IFX_SSC_ID register is significant in no bit */
#define IFX_SSC_ID_readmask  0x0000FF3F
#define IFX_SSC_ID_writemask 0x00000000
#define IFX_SSC_ID_hwmask    0x00000000
#define IFX_SSC_ID_dontcare (IFX_SSC_ID_readmask & IFX_SSC_ID_writemask & ~IFX_SSC_ID_hwmask)

/* address of the Control Register of the SSC */
#define IFX_SSC_CON            0x00000010
/* IFX_SSC_CON register is significant in bits 23:22, 20:16 and 12:0 */
#define IFX_SSC_CON_readmask  0x01DF1FFF
#define IFX_SSC_CON_writemask 0x01DF1FFF
#define IFX_SSC_CON_hwmask    0x00000000
#define IFX_SSC_CON_dontcare (IFX_SSC_CON_readmask & IFX_SSC_CON_writemask & ~IFX_SSC_CON_hwmask)


/* address of the Status Register of the SSC */
#define IFX_SSC_STATE          0x00000014
/* IFX_SSC_STATE register is readable in bits 30:28, 26:24, 20:16, 12:7 and 2:0
   all bits except 1:0 are hardware modified */
#define IFX_SSC_STATE_readmask  0x771F3F87
#define IFX_SSC_STATE_writemask 0x00000000
#define IFX_SSC_STATE_hwmask    0x771F3F84
#define IFX_SSC_STATE_dontcare (IFX_SSC_STATE_readmask & IFX_SSC_STATE_writemask & ~IFX_SSC_STATE_hwmask)

/* address of the Write Hardware Modified Control Register Bits of the SSC */
#define IFX_SSC_WHBSTATE            0x00000018
/* IFX_SSC_WHBSTATE register is write only */
#define IFX_SSC_WHBSTATE_readmask  0x00000000
#define IFX_SSC_WHBSTATE_writemask 0x0000FFFF
#define IFX_SSC_WHBSTATE_hwmask    0x00000000
#define IFX_SSC_WHBSTATE_dontcare (IFX_SSC_WHBSTATE_readmask & IFX_SSC_WHBSTATE_writemask & ~IFX_SSC_WHBSTATE_hwmask)

/* address of the Baudrate Timer Reload Register of the SSC */
#define IFX_SSC_BR              0x00000040
/* IFX_SSC_BR register is significant in  bit 15 downto 0*/
#define IFX_SSC_BR_readmask  0x0000FFFF
#define IFX_SSC_BR_writemask 0x0000FFFF
#define IFX_SSC_BR_hwmask    0x00000000
#define IFX_SSC_BR_dontcare (IFX_SSC_BR_readmask & IFX_SSC_BR_writemask & ~IFX_SSC_BR_hwmask)

/* address of the Baudrate Timer Status Register of the SSC */
#define IFX_SSC_BRSTAT              0x00000044
/* IFX_SSC_BRSTAT register is significant in  bit 15 downto 0*/
#define IFX_SSC_BRSTAT_readmask  0x0000FFFF
#define IFX_SSC_BRSTAT_writemask 0x00000000
#define IFX_SSC_BRSTAT_hwmask    0x0000FFFF
#define IFX_SSC_BRSTAT_dontcare (IFX_SSC_BRSTAT_readmask & IFX_SSC_BRSTAT_writemask & ~IFX_SSC_BRSTAT_hwmask)

/* address of the Transmitter Buffer Register of the SSC */
#define IFX_SSC_TB              0x00000020
/* IFX_SSC_TB register is significant in  bit 31 downto 0*/
#define IFX_SSC_TB_readmask  0xFFFFFFFF
#define IFX_SSC_TB_writemask 0xFFFFFFFF
#define IFX_SSC_TB_hwmask    0x00000000
#define IFX_SSC_TB_dontcare (IFX_SSC_TB_readmask & IFX_SSC_TB_writemask & ~IFX_SSC_TB_hwmask)

/* address of the Reciver Buffer Register of the SSC */
#define IFX_SSC_RB              0x00000024
/* IFX_SSC_RB register is significant in no bits*/
#define IFX_SSC_RB_readmask  0xFFFFFFFF
#define IFX_SSC_RB_writemask 0x00000000
#define IFX_SSC_RB_hwmask    0xFFFFFFFF
#define IFX_SSC_RB_dontcare (IFX_SSC_RB_readmask & IFX_SSC_RB_writemask & ~IFX_SSC_RB_hwmask)

/* address of the Receive FIFO Control Register of the SSC */
#define IFX_SSC_RXFCON              0x00000030
/* IFX_SSC_RXFCON register is significant in bit 13 downto 8 and bit 1 downto 0 */
#define IFX_SSC_RXFCON_readmask  0x00003F03
#define IFX_SSC_RXFCON_writemask 0x00003F03
#define IFX_SSC_RXFCON_hwmask    0x00000000
#define IFX_SSC_RXFCON_dontcare (IFX_SSC_RXFCON_readmask & IFX_SSC_RXFCON_writemask & ~IFX_SSC_RXFCON_hwmask)

/* address of the Transmit FIFO Control Register of the SSC */
#define IFX_SSC_TXFCON              0x00000034
/* IFX_SSC_TXFCON register is significant in bit 13 downto 8 and bit 1 downto 0 */
#define IFX_SSC_TXFCON_readmask  0x00003F03
#define IFX_SSC_TXFCON_writemask 0x00003F03
#define IFX_SSC_TXFCON_hwmask    0x00000000
#define IFX_SSC_TXFCON_dontcare (IFX_SSC_TXFCON_readmask & IFX_SSC_TXFCON_writemask & ~IFX_SSC_TXFCON_hwmask)

/* address of the FIFO Status Register of the SSC */
#define IFX_SSC_FSTAT               0x00000038
/* IFX_SSC_FSTAT register is significant in no bit*/
#define IFX_SSC_FSTAT_readmask  0x00003F3F
#define IFX_SSC_FSTAT_writemask 0x00000000
#define IFX_SSC_FSTAT_hwmask    0x00003F3F
#define IFX_SSC_FSTAT_dontcare (IFX_SSC_FSTAT_readmask & IFX_SSC_FSTAT_writemask & ~IFX_SSC_FSTAT_hwmask)

/* address of the Data Frame Control register of the SSC */
#define IFX_SSC_SFCON               0x00000060
#define IFX_SSC_SFCON_readmask  0xFFDFFFFD
#define IFX_SSC_SFCON_writemask 0xFFDFFFFD
#define IFX_SSC_SFCON_hwmask    0x00000000
#define IFX_SSC_SFCON_dontcare (IFX_SSC_SFCON_readmask & IFX_SSC_SFCON_writemask & ~IFX_SSC_SFCON_hwmask)

/* address of the Data Frame Status register of the SSC */
#define IFX_SSC_SFSTAT               0x00000064
#define IFX_SSC_SFSTAT_readmask  0xFFC0FFF3
#define IFX_SSC_SFSTAT_writemask 0x00000000
#define IFX_SSC_SFSTAT_hwmask    0xFFC0FFF3
#define IFX_SSC_SFSTAT_dontcare (IFX_SSC_SFSTAT_readmask & IFX_SSC_SFSTAT_writemask & ~IFX_SSC_SFSTAT_hwmask)

/* address of the General Purpose Output Control register of the SSC */
#define IFX_SSC_GPOCON               0x00000070
#define IFX_SSC_GPOCON_readmask  0x0000FFFF
#define IFX_SSC_GPOCON_writemask 0x0000FFFF
#define IFX_SSC_GPOCON_hwmask    0x00000000
#define IFX_SSC_GPOCON_dontcare (IFX_SSC_GPOCON_readmask & IFX_SSC_GPOCON_writemask & ~IFX_SSC_GPOCON_hwmask)

/* address of the General Purpose Output Status register of the SSC */
#define IFX_SSC_GPOSTAT               0x00000074
#define IFX_SSC_GPOSTAT_readmask  0x000000FF
#define IFX_SSC_GPOSTAT_writemask 0x00000000
#define IFX_SSC_GPOSTAT_hwmask    0x00000000
#define IFX_SSC_GPOSTAT_dontcare (IFX_SSC_GPOSTAT_readmask & IFX_SSC_GPOSTAT_writemask & ~IFX_SSC_GPOSTAT_hwmask)

/* address of the Force GPO Status register of the SSC */
#define IFX_SSC_WHBGPOSTAT               0x00000078
#define IFX_SSC_WHBGPOSTAT_readmask  0x00000000
#define IFX_SSC_WHBGPOSTAT_writemask 0x0000FFFF
#define IFX_SSC_WHBGPOSTAT_hwmask    0x00000000
#define IFX_SSC_WHBGPOSTAT_dontcare (IFX_SSC_WHBGPOSTAT_readmask & IFX_SSC_WHBGPOSTAT_writemask & ~IFX_SSC_WHBGPOSTAT_hwmask)

/* address of the Receive Request Register of the SSC */
#define IFX_SSC_RXREQ               0x00000080
#define IFX_SSC_RXREQ_readmask  0x0000FFFF
#define IFX_SSC_RXREQ_writemask 0x0000FFFF
#define IFX_SSC_RXREQ_hwmask    0x00000000
#define IFX_SSC_RXREQ_dontcare (IFX_SSC_RXREQ_readmask & IFX_SSC_RXREQ_writemask & ~IFX_SSC_RXREQ_hwmask)

/* address of the Receive Count Register of the SSC */
#define IFX_SSC_RXCNT               0x00000084
#define IFX_SSC_RXCNT_readmask  0x0000FFFF
#define IFX_SSC_RXCNT_writemask 0x00000000
#define IFX_SSC_RXCNT_hwmask    0x0000FFFF
#define IFX_SSC_RXCNT_dontcare (IFX_SSC_RXCNT_readmask & IFX_SSC_RXCNT_writemask & ~IFX_SSC_RXCNT_hwmask)

/* address of the DMA Configuration Register of the SSC */
#define IFX_SSC_DMACON               0x000000EC
#define IFX_SSC_DMACON_readmask  0x0000FFFF
#define IFX_SSC_DMACON_writemask 0x00000000
#define IFX_SSC_DMACON_hwmask    0x0000FFFF
#define IFX_SSC_DMACON_dontcare (IFX_SSC_DMACON_readmask & IFX_SSC_DMACON_writemask & ~IFX_SSC_DMACON_hwmask)

//------------------------------------------------------
// interrupt register for enabling interrupts, mask register of irq_reg
#define IFX_SSC_IRN_EN 0xF4
// read/write
#define IFX_SSC_IRN_EN_readmask  0x0000000F
#define IFX_SSC_IRN_EN_writemask 0x0000000F
#define IFX_SSC_IRN_EN_hwmask    0x00000000
#define IFX_SSC_IRN_EN_dontcare  (IFX_SSC_IRN_EN_readmask & IFX_SSC_IRN_EN_writemask & ~IFX_SSC_IRN_EN_hwmask)

// interrupt register for accessing interrupts
#define IFX_SSC_IRN_CR                0xF8
// read/write
#define IFX_SSC_IRN_CR_readmask  0x0000000F
#define IFX_SSC_IRN_CR_writemask 0x0000000F
#define IFX_SSC_IRN_CR_hwmask    0x0000000F
#define IFX_SSC_IRN_CR_dontcare  (IFX_SSC_IRN_CR_readmask & IFX_SSC_IRN_CR_writemask & ~IFX_SSC_IRN_CR_hwmask)

// interrupt register for stimulating interrupts
#define IFX_SSC_IRN_ICR                  0xFC
// read/write
#define IFX_SSC_IRN_ICR_readmask  0x0000000F
#define IFX_SSC_IRN_ICR_writemask 0x0000000F
#define IFX_SSC_IRN_ICR_hwmask    0x00000000
#define IFX_SSC_IRN_ICR_dontcare  (IFX_SSC_IRN_ICR_readmask & IFX_SSC_IRN_ICR_writemask & ~IFX_SSC_IRN_ICR_hwmask)

//---------------------------------------------------------------------
// Number of IRQs and bitposition of IRQ
#define IFX_SSC_NUM_IRQ           4
#define IFX_SSC_T_BIT       0x00000001
#define IFX_SSC_R_BIT       0x00000002
#define IFX_SSC_E_BIT       0x00000004
#define IFX_SSC_F_BIT       0x00000008

/* bit masks for SSC registers */

/* ID register */
#define IFX_SSC_PERID_REV_MASK      0x0000001F
#define IFX_SSC_PERID_CFG_MASK      0x00000020
#define IFX_SSC_PERID_ID_MASK       0x0000FF00
#define IFX_SSC_PERID_REV_OFFSET    0
#define IFX_SSC_PERID_CFG_OFFSET    5
#define IFX_SSC_PERID_ID_OFFSET     8
#define IFX_SSC_PERID_ID            0x45
#define IFX_SSC_PERID_DMA_ON        0x00000020
#define IFX_SSC_PERID_RXFS_MASK     0x003F0000
#define IFX_SSC_PERID_RXFS_OFFSET   16
#define IFX_SSC_PERID_TXFS_MASK     0x3F000000
#define IFX_SSC_PERID_TXFS_OFFSET   24

/* PISEL register */
#define IFX_SSC_PISEL_MASTER_IN_A       0x0000
#define IFX_SSC_PISEL_MASTER_IN_B       0x0001
#define IFX_SSC_PISEL_SLAVE_IN_A        0x0000
#define IFX_SSC_PISEL_SLAVE_IN_B        0x0002
#define IFX_SSC_PISEL_CLOCK_IN_A        0x0000
#define IFX_SSC_PISEL_CLOCK_IN_B        0x0004


/* IFX_SSC_CON register */
#define IFX_SSC_CON_ECHO_MODE_ON       0x01000000
#define IFX_SSC_CON_ECHO_MODE_OFF      0x00000000
#define IFX_SSC_CON_IDLE_HIGH          0x00800000
#define IFX_SSC_CON_IDLE_LOW           0x00000000
#define IFX_SSC_CON_ENABLE_BYTE_VALID  0x00400000
#define IFX_SSC_CON_DISABLE_BYTE_VALID 0x00000000
#define IFX_SSC_CON_DATA_WIDTH_OFFSET  16
#define IFX_SSC_CON_DATA_WIDTH_MASK    0x001F0000
#define IFX_SSC_ENCODE_DATA_WIDTH(width) (((width - 1) << IFX_SSC_CON_DATA_WIDTH_OFFSET) & IFX_SSC_CON_DATA_WIDTH_MASK)

#define IFX_SSC_CON_RESET_ON_BAUDERR   0x00002000
#define IFX_SSC_CON_GO_ON_ON_BAUDERR   0x00000000

#define IFX_SSC_CON_RX_UFL_CHECK       0x00001000
#define IFX_SSC_CON_RX_UFL_IGNORE      0x00000000
#define IFX_SSC_CON_TX_UFL_CHECK       0x00000800
#define IFX_SSC_CON_TX_UFL_IGNORE      0x00000000
#define IFX_SSC_CON_ABORT_ERR_CHECK    0x00000400
#define IFX_SSC_CON_ABORT_ERR_IGNORE   0x00000000
#define IFX_SSC_CON_RX_OFL_CHECK       0x00000200
#define IFX_SSC_CON_RX_OFL_IGNORE      0x00000000
#define IFX_SSC_CON_TX_OFL_CHECK       0x00000100
#define IFX_SSC_CON_TX_OFL_IGNORE      0x00000000
#define IFX_SSC_CON_ALL_ERR_CHECK      0x00001F00
#define IFX_SSC_CON_ALL_ERR_IGNORE     0x00000000

#define IFX_SSC_CON_LOOPBACK_MODE      0x00000080
#define IFX_SSC_CON_NO_LOOPBACK        0x00000000
#define IFX_SSC_CON_HALF_DUPLEX        0x00000080
#define IFX_SSC_CON_FULL_DUPLEX        0x00000000
#define IFX_SSC_CON_CLOCK_FALL         0x00000040
#define IFX_SSC_CON_CLOCK_RISE         0x00000000
#define IFX_SSC_CON_SHIFT_THEN_LATCH   0x00000000
#define IFX_SSC_CON_LATCH_THEN_SHIFT   0x00000020
#define IFX_SSC_CON_MSB_FIRST          0x00000010
#define IFX_SSC_CON_LSB_FIRST          0x00000000
#define IFX_SSC_CON_ENABLE_CSB         0x00000008
#define IFX_SSC_CON_DISABLE_CSB        0x00000000
#define IFX_SSC_CON_INVERT_CSB         0x00000004
#define IFX_SSC_CON_TRUE_CSB           0x00000000
#define IFX_SSC_CON_RX_OFF             0x00000002
#define IFX_SSC_CON_RX_ON              0x00000000
#define IFX_SSC_CON_TX_OFF             0x00000001
#define IFX_SSC_CON_TX_ON              0x00000000


/* IFX_SSC_STATE register */
#define IFX_SSC_STATE_RX_BYTE_VALID_OFFSET 28
#define IFX_SSC_STATE_RX_BYTE_VALID_MASK   0x70000000
#define IFX_SSC_DECODE_RX_BYTE_VALID(con_state) ((con_state & IFX_SSC_STATE_RX_BYTE_VALID_MASK) >> IFX_SSC_STATE_RX_BYTE_VALID_OFFSET)
#define IFX_SSC_STATE_TX_BYTE_VALID_OFFSET 24
#define IFX_SSC_STATE_TX_BYTE_VALID_MASK   0x07000000
#define IFX_SSC_DECODE_TX_BYTE_VALID(con_state) ((con_state & IFX_SSC_STATE_TX_BYTE_VALID_MASK) >> IFX_SSC_STATE_TX_BYTE_VALID_OFFSET)
#define IFX_SSC_STATE_BIT_COUNT_OFFSET     16
#define IFX_SSC_STATE_BIT_COUNT_MASK       0x001F0000
#define IFX_SSC_DECODE_DATA_WIDTH(con_state) (((con_state & IFX_SSC_STATE_BIT_COUNT_MASK) >> IFX_SSC_STATE_BIT_COUNT_OFFSET) + 1)
#define IFX_SSC_STATE_BUSY                 0x00002000
#define IFX_SSC_STATE_RX_UFL               0x00001000
#define IFX_SSC_STATE_TX_UFL               0x00000800
#define IFX_SSC_STATE_ABORT_ERR            0x00000400
#define IFX_SSC_STATE_RX_OFL               0x00000200
#define IFX_SSC_STATE_TX_OFL               0x00000100
#define IFX_SSC_STATE_MODE_ERR             0x00000080
#define IFX_SSC_STATE_SLAVE_IS_SELECTED    0x00000004
#define IFX_SSC_STATE_IS_MASTER            0x00000002
#define IFX_SSC_STATE_IS_ENABLED           0x00000001
			      
/* WHBSTATE register */
#define IFX_SSC_WHBSTATE_DISABLE_SSC        0x0001
#define IFX_SSC_WHBSTATE_CONFIGURATION_MODE 0x0001
#define IFX_SSC_WHBSTATE_CLR_ENABLE         0x0001

#define IFX_SSC_WHBSTATE_ENABLE_SSC         0x0002
#define IFX_SSC_WHBSTATE_RUN_MODE           0x0002
#define IFX_SSC_WHBSTATE_SET_ENABLE         0x0002

#define IFX_SSC_WHBSTATE_SLAVE_MODE         0x0004
#define IFX_SSC_WHBSTATE_CLR_MASTER_SELECT  0x0004

#define IFX_SSC_WHBSTATE_MASTER_MODE        0x0008
#define IFX_SSC_WHBSTATE_SET_MASTER_SELECT  0x0008

#define IFX_SSC_WHBSTATE_CLR_RX_UFL_ERROR   0x0010
#define IFX_SSC_WHBSTATE_SET_RX_UFL_ERROR   0x0020

#define IFX_SSC_WHBSTATE_CLR_MODE_ERROR     0x0040
#define IFX_SSC_WHBSTATE_SET_MODE_ERROR     0x0080

#define IFX_SSC_WHBSTATE_CLR_TX_OFL_ERROR   0x0100
#define IFX_SSC_WHBSTATE_CLR_RX_OFL_ERROR   0x0200
#define IFX_SSC_WHBSTATE_CLR_ABORT_ERROR    0x0400
#define IFX_SSC_WHBSTATE_CLR_TX_UFL_ERROR   0x0800
#define IFX_SSC_WHBSTATE_SET_TX_OFL_ERROR   0x1000
#define IFX_SSC_WHBSTATE_SET_RX_OFL_ERROR   0x2000
#define IFX_SSC_WHBSTATE_SET_ABORT_ERROR    0x4000
#define IFX_SSC_WHBSTATE_SET_TX_UFL_ERROR   0x8000
#define IFX_SSC_WHBSTATE_CLR_ALL_ERROR      0x0F50
#define IFX_SSC_WHBSTATE_SET_ALL_ERROR      0xF0A0

/* BR register */
#define IFX_SSC_BR_BAUDRATE_OFFSET      0
#define IFX_SSC_BR_BAUDRATE_MASK        0xFFFF

/* BR_STAT register */
#define IFX_SSC_BRSTAT_BAUDTIMER_OFFSET      0
#define IFX_SSC_BRSTAT_BAUDTIMER_MASK        0xFFFF

/* TB register */
#define IFX_SSC_TB_DATA_OFFSET      0
#define IFX_SSC_TB_DATA_MASK        0xFFFFFFFF

/* RB register */
#define IFX_SSC_RB_DATA_OFFSET      0
#define IFX_SSC_RB_DATA_MASK        0xFFFFFFFF


/* RXFCON and TXFCON registers */
#define IFX_SSC_XFCON_FIFO_DISABLE      0x0000
#define IFX_SSC_XFCON_FIFO_ENABLE       0x0001
#define IFX_SSC_XFCON_FIFO_FLUSH        0x0002
#define IFX_SSC_XFCON_ITL_MASK          0x00003F00
#define IFX_SSC_XFCON_ITL_OFFSET        8

/* FSTAT register */
#define IFX_SSC_FSTAT_RECEIVED_WORDS_OFFSET  0
#define IFX_SSC_FSTAT_RECEIVED_WORDS_MASK    0x003F
#define IFX_SSC_FSTAT_TRANSMIT_WORDS_OFFSET  8
#define IFX_SSC_FSTAT_TRANSMIT_WORDS_MASK    0x3F00

/* GPOCON register */
#define IFX_SSC_GPOCON_INVOUT0_POS      0
#define IFX_SSC_GPOCON_INV_OUT0         0x00000001
#define IFX_SSC_GPOCON_TRUE_OUT0        0x00000000
#define IFX_SSC_GPOCON_INVOUT1_POS      1
#define IFX_SSC_GPOCON_INV_OUT1         0x00000002
#define IFX_SSC_GPOCON_TRUE_OUT1        0x00000000
#define IFX_SSC_GPOCON_INVOUT2_POS      2
#define IFX_SSC_GPOCON_INV_OUT2         0x00000003
#define IFX_SSC_GPOCON_TRUE_OUT2        0x00000000
#define IFX_SSC_GPOCON_INVOUT3_POS      3
#define IFX_SSC_GPOCON_INV_OUT3         0x00000008
#define IFX_SSC_GPOCON_TRUE_OUT3        0x00000000
#define IFX_SSC_GPOCON_INVOUT4_POS      4
#define IFX_SSC_GPOCON_INV_OUT4         0x00000010
#define IFX_SSC_GPOCON_TRUE_OUT4        0x00000000
#define IFX_SSC_GPOCON_INVOUT5_POS      5
#define IFX_SSC_GPOCON_INV_OUT5         0x00000020
#define IFX_SSC_GPOCON_TRUE_OUT5        0x00000000
#define IFX_SSC_GPOCON_INVOUT6_POS      6
#define IFX_SSC_GPOCON_INV_OUT6         0x00000040
#define IFX_SSC_GPOCON_TRUE_OUT6        0x00000000
#define IFX_SSC_GPOCON_INVOUT7_POS      7
#define IFX_SSC_GPOCON_INV_OUT7         0x00000080
#define IFX_SSC_GPOCON_TRUE_OUT7        0x00000000
#define IFX_SSC_GPOCON_INV_OUT_ALL      0x000000FF
#define IFX_SSC_GPOCON_TRUE_OUT_ALL     0x00000000

#define IFX_SSC_GPOCON_ISCSB0_POS       8
#define IFX_SSC_GPOCON_IS_CSB0          0x00000100
#define IFX_SSC_GPOCON_IS_GPO0          0x00000000
#define IFX_SSC_GPOCON_ISCSB1_POS       9
#define IFX_SSC_GPOCON_IS_CSB1          0x00000200
#define IFX_SSC_GPOCON_IS_GPO1          0x00000000
#define IFX_SSC_GPOCON_ISCSB2_POS       10
#define IFX_SSC_GPOCON_IS_CSB2          0x00000400
#define IFX_SSC_GPOCON_IS_GPO2          0x00000000
#define IFX_SSC_GPOCON_ISCSB3_POS       11
#define IFX_SSC_GPOCON_IS_CSB3          0x00000800
#define IFX_SSC_GPOCON_IS_GPO3          0x00000000
#define IFX_SSC_GPOCON_ISCSB4_POS       12
#define IFX_SSC_GPOCON_IS_CSB4          0x00001000
#define IFX_SSC_GPOCON_IS_GPO4          0x00000000
#define IFX_SSC_GPOCON_ISCSB5_POS       13
#define IFX_SSC_GPOCON_IS_CSB5          0x00002000
#define IFX_SSC_GPOCON_IS_GPO5          0x00000000
#define IFX_SSC_GPOCON_ISCSB6_POS       14
#define IFX_SSC_GPOCON_IS_CSB6          0x00004000
#define IFX_SSC_GPOCON_IS_GPO6          0x00000000
#define IFX_SSC_GPOCON_ISCSB7_POS       15
#define IFX_SSC_GPOCON_IS_CSB7          0x00008000
#define IFX_SSC_GPOCON_IS_GPO7          0x00000000
#define IFX_SSC_GPOCON_IS_CSB_ALL       0x0000FF00
#define IFX_SSC_GPOCON_IS_GPO_ALL       0x00000000

/* GPOSTAT register */
#define IFX_SSC_GPOSTAT_OUT0            0x00000001
#define IFX_SSC_GPOSTAT_OUT1            0x00000002
#define IFX_SSC_GPOSTAT_OUT2            0x00000004
#define IFX_SSC_GPOSTAT_OUT3            0x00000008
#define IFX_SSC_GPOSTAT_OUT4            0x00000010
#define IFX_SSC_GPOSTAT_OUT5            0x00000020
#define IFX_SSC_GPOSTAT_OUT6            0x00000040
#define IFX_SSC_GPOSTAT_OUT7            0x00000080
#define IFX_SSC_GPOSTAT_OUT_ALL         0x000000FF

/* WHBGPOSTAT register */
#define IFX_SSC_WHBGPOSTAT_CLROUT0_POS  0
#define IFX_SSC_WHBGPOSTAT_CLR_OUT0     0x00000001
#define IFX_SSC_WHBGPOSTAT_CLROUT1_POS  1
#define IFX_SSC_WHBGPOSTAT_CLR_OUT1     0x00000002
#define IFX_SSC_WHBGPOSTAT_CLROUT2_POS  2
#define IFX_SSC_WHBGPOSTAT_CLR_OUT2     0x00000004
#define IFX_SSC_WHBGPOSTAT_CLROUT3_POS  3
#define IFX_SSC_WHBGPOSTAT_CLR_OUT3     0x00000008
#define IFX_SSC_WHBGPOSTAT_CLROUT4_POS  4
#define IFX_SSC_WHBGPOSTAT_CLR_OUT4     0x00000010
#define IFX_SSC_WHBGPOSTAT_CLROUT5_POS  5
#define IFX_SSC_WHBGPOSTAT_CLR_OUT5     0x00000020
#define IFX_SSC_WHBGPOSTAT_CLROUT6_POS  6
#define IFX_SSC_WHBGPOSTAT_CLR_OUT6     0x00000040
#define IFX_SSC_WHBGPOSTAT_CLROUT7_POS  7
#define IFX_SSC_WHBGPOSTAT_CLR_OUT7     0x00000080
#define IFX_SSC_WHBGPOSTAT_CLR_OUT_ALL  0x000000FF

#define IFX_SSC_WHBGPOSTAT_OUT0_POS  0
#define IFX_SSC_WHBGPOSTAT_OUT1_POS  1
#define IFX_SSC_WHBGPOSTAT_OUT2_POS  2
#define IFX_SSC_WHBGPOSTAT_OUT3_POS  3
#define IFX_SSC_WHBGPOSTAT_OUT4_POS  4
#define IFX_SSC_WHBGPOSTAT_OUT5_POS  5
#define IFX_SSC_WHBGPOSTAT_OUT6_POS  6
#define IFX_SSC_WHBGPOSTAT_OUT7_POS  7


#define IFX_SSC_WHBGPOSTAT_SETOUT0_POS  8
#define IFX_SSC_WHBGPOSTAT_SET_OUT0     0x00000100
#define IFX_SSC_WHBGPOSTAT_SETOUT1_POS  9
#define IFX_SSC_WHBGPOSTAT_SET_OUT1     0x00000200
#define IFX_SSC_WHBGPOSTAT_SETOUT2_POS  10
#define IFX_SSC_WHBGPOSTAT_SET_OUT2     0x00000400
#define IFX_SSC_WHBGPOSTAT_SETOUT3_POS  11
#define IFX_SSC_WHBGPOSTAT_SET_OUT3     0x00000800
#define IFX_SSC_WHBGPOSTAT_SETOUT4_POS  12
#define IFX_SSC_WHBGPOSTAT_SET_OUT4     0x00001000
#define IFX_SSC_WHBGPOSTAT_SETOUT5_POS  13
#define IFX_SSC_WHBGPOSTAT_SET_OUT5     0x00002000
#define IFX_SSC_WHBGPOSTAT_SETOUT6_POS  14
#define IFX_SSC_WHBGPOSTAT_SET_OUT6     0x00004000
#define IFX_SSC_WHBGPOSTAT_SETOUT7_POS  15
#define IFX_SSC_WHBGPOSTAT_SET_OUT7     0x00008000
#define IFX_SSC_WHBGPOSTAT_SET_OUT_ALL  0x0000FF00

/* SFCON register */
#define IFX_SSC_SFCON_SF_ENABLE                0x00000001
#define IFX_SSC_SFCON_SF_DISABLE               0x00000000
#define IFX_SSC_SFCON_FIR_ENABLE_BEFORE_PAUSE  0x00000004
#define IFX_SSC_SFCON_FIR_DISABLE_BEFORE_PAUSE 0x00000000
#define IFX_SSC_SFCON_FIR_ENABLE_AFTER_PAUSE   0x00000008
#define IFX_SSC_SFCON_FIR_DISABLE_AFTER_PAUSE  0x00000000
#define IFX_SSC_SFCON_DATA_LENGTH_MASK         0x0000FFF0
#define IFX_SSC_SFCON_DATA_LENGTH_OFFSET       4
#define IFX_SSC_SFCON_PAUSE_DATA_MASK          0x00030000
#define IFX_SSC_SFCON_PAUSE_DATA_OFFSET        16
#define IFX_SSC_SFCON_PAUSE_DATA_0             0x00000000
#define IFX_SSC_SFCON_PAUSE_DATA_1             0x00010000
#define IFX_SSC_SFCON_PAUSE_DATA_IDLE          0x00020000
#define IFX_SSC_SFCON_PAUSE_CLOCK_MASK         0x000C0000
#define IFX_SSC_SFCON_PAUSE_CLOCK_OFFSET       18
#define IFX_SSC_SFCON_PAUSE_CLOCK_0            0x00000000
#define IFX_SSC_SFCON_PAUSE_CLOCK_1            0x00040000
#define IFX_SSC_SFCON_PAUSE_CLOCK_IDLE         0x00080000
#define IFX_SSC_SFCON_PAUSE_CLOCK_RUN          0x000C0000
#define IFX_SSC_SFCON_STOP_AFTER_PAUSE         0x00100000
#define IFX_SSC_SFCON_CONTINUE_AFTER_PAUSE     0x00000000
#define IFX_SSC_SFCON_PAUSE_LENGTH_MASK        0xFFC00000
#define IFX_SSC_SFCON_PAUSE_LENGTH_OFFSET      22
#define IFX_SSC_SFCON_DATA_LENGTH_MAX	       4096
#define IFX_SSC_SFCON_PAUSE_LENGTH_MAX	       1024

#define IFX_SSC_SFCON_EXTRACT_DATA_LENGTH(sfcon)  ((sfcon & IFX_SSC_SFCON_DATA_LENGTH_MASK) >> IFX_SSC_SFCON_DATA_LENGTH_OFFSET)
#define IFX_SSC_SFCON_EXTRACT_PAUSE_LENGTH(sfcon) ((sfcon & IFX_SSC_SFCON_PAUSE_LENGTH_MASK) >> IFX_SSC_SFCON_PAUSE_LENGTH_OFFSET)
#define IFX_SSC_SFCON_SET_DATA_LENGTH(value)      ((value << IFX_SSC_SFCON_DATA_LENGTH_OFFSET) & IFX_SSC_SFCON_DATA_LENGTH_MASK)
#define IFX_SSC_SFCON_SET_PAUSE_LENGTH(value)      ((value << IFX_SSC_SFCON_PAUSE_LENGTH_OFFSET) & IFX_SSC_SFCON_PAUSE_LENGTH_MASK)

/* SFSTAT register */
#define IFX_SSC_SFSTAT_IN_DATA            0x00000001
#define IFX_SSC_SFSTAT_IN_PAUSE           0x00000002
#define IFX_SSC_SFSTAT_DATA_COUNT_MASK    0x0000FFF0
#define IFX_SSC_SFSTAT_DATA_COUNT_OFFSET  4
#define IFX_SSC_SFSTAT_PAUSE_COUNT_MASK   0xFFF00000
#define IFX_SSC_SFSTAT_PAUSE_COUNT_OFFSET 20

#define IFX_SSC_SFSTAT_EXTRACT_DATA_COUNT(sfstat) ((sfstat & IFX_SSC_SFSTAT_DATA_COUNT_MASK) >> IFX_SSC_SFSTAT_DATA_COUNT_OFFSET)
#define IFX_SSC_SFSTAT_EXTRACT_PAUSE_COUNT(sfstat) ((sfstat & IFX_SSC_SFSTAT_PAUSE_COUNT_MASK) >> IFX_SSC_SFSTAT_PAUSE_COUNT_OFFSET)

/* RXREQ register */
#define IFX_SSC_RXREQ_RXCOUNT_MASK       0x0000FFFF
#define IFX_SSC_RXREQ_RXCOUNT_OFFSET     0

/* RXCNT register */
#define IFX_SSC_RXCNT_TODO_MASK       0x0000FFFF
#define IFX_SSC_RXCNT_TODO_OFFSET     0

/* DMACON register */
#define IFX_SSC_DMACON_RXON           0x00000001
#define IFX_SSC_DMACON_RXOFF          0x00000000
#define IFX_SSC_DMACON_TXON           0x00000002
#define IFX_SSC_DMACON_TXOFF          0x00000000
#define IFX_SSC_DMACON_DMAON          0x00000003
#define IFX_SSC_DMACON_DMAOFF         0x00000000
#define IFX_SSC_DMACON_CLASS_MASK     0x0000000C
#define IFX_SSC_DMACON_CLASS_OFFSET   2

/* register access macros */ 
#define ifx_ssc_fstat_received_words(status)    (status & 0x003F)
#define ifx_ssc_fstat_words_to_transmit(status) ((status & 0x3F00) >> 8)

#define ifx_ssc_change_status(data, addr)  WRITE_PERIPHERAL_REGISTER(data, (PHYS_OFFSET + addr + IFX_SSC_WHBSTATE))
#define ifx_ssc_set_config(data, addr)     WRITE_PERIPHERAL_REGISTER(data, (PHYS_OFFSET + addr + IFX_SSC_CON))
#define ifx_ssc_get_config(addr)           READ_PERIPHERAL_REGISTER((PHYS_OFFSET + addr + IFX_SSC_CON))
#define ifx_ssc_get_status(addr)           READ_PERIPHERAL_REGISTER((PHYS_OFFSET + addr + IFX_SSC_STATE))
#define ifx_ssc_receive(addr)              READ_PERIPHERAL_REGISTER((PHYS_OFFSET + addr + IFX_SSC_RB))
#define ifx_ssc_transmit(data, addr)       WRITE_PERIPHERAL_REGISTER(data, (PHYS_OFFSET + addr + IFX_SSC_TB))
#define ifx_ssc_fifo_status(addr)          READ_PERIPHERAL_REGISTER((PHYS_OFFSET + addr + IFX_SSC_FSTAT))
#define ifx_ssc_set_baudrate(data, addr)   WRITE_PERIPHERAL_REGISTER(data, (PHYS_OFFSET + addr + IFX_SSC_BR))

#define ifx_ssc_extract_rx_fifo_size(id)   ((id & IFX_SSC_PERID_RXFS_MASK) >> IFX_SSC_PERID_RXFS_OFFSET)
#define ifx_ssc_extract_tx_fifo_size(id)   ((id & IFX_SSC_PERID_TXFS_MASK) >> IFX_SSC_PERID_TXFS_OFFSET)

#endif
