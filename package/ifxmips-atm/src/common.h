#include <linux/atmdev.h>
#include <asm/ifxmips/ifxmips_irq.h>
#include <linux/irq.h>
#include <linux/sem.h>
#include <linux/coda.h>

#define RX_DMA_CH_CBR                   0
#define RX_DMA_CH_VBR_RT                1
#define RX_DMA_CH_VBR_NRT               2
#define RX_DMA_CH_AVR                   3
#define RX_DMA_CH_UBR                   4
#define RX_DMA_CH_OAM                   5
#define RX_DMA_CH_TOTAL                 6

#define WRX_DMA_CHANNEL_INTERRUPT_MODE  0x00
#define WRX_DMA_CHANNEL_POLLING_MODE    0x01
//#define WRX_DMA_CHANNEL_COUNTER_MODE    0x02
#define WRX_DMA_CHANNEL_COUNTER_MODE    WRX_DMA_CHANNEL_INTERRUPT_MODE
#define WRX_DMA_BUF_LEN_PER_DESCRIPTOR  0x00
#define WRX_DMA_BUF_LEN_PER_CHANNEL     0x01

#define ATM_VBR_RT     6
#define ATM_VBR_NRT    ATM_VBR
#define ATM_UBR_PLUS   7

#define SET_BITS(x, msb, lsb, value)    (((x) & ~(((1 << ((msb) + 1)) - 1) ^ ((1 << (lsb)) - 1))) | (((value) & ((1 << (1 + (msb) - (lsb))) - 1)) << (lsb)))

#define GET_ATM_PRIV(dev)       ((Atm_Priv *)dev->priv)

#define CDM_CFG                         PPE_REG_ADDR(0x0100)

#define CDM_CFG_RAM1                    GET_BITS(*CDM_CFG, 3, 2)
#define CDM_CFG_RAM0                    (*CDM_CFG & (1 << 1))

#define CDM_CFG_RAM1_SET(value)         SET_BITS(0, 3, 2, value)
#define CDM_CFG_RAM0_SET(value)         ((value) ? (1 << 1) : 0)

/*
 *  EMA Registers
 */
#define EMA_CMDCFG                      PPE_REG_ADDR(0x0A00)
#define EMA_DATACFG                     PPE_REG_ADDR(0x0A01)
#define EMA_CMDCNT                      PPE_REG_ADDR(0x0A02)
#define EMA_DATACNT                     PPE_REG_ADDR(0x0A03)
#define EMA_ISR                         PPE_REG_ADDR(0x0A04)
#define EMA_IER                         PPE_REG_ADDR(0x0A05)
#define EMA_CFG                         PPE_REG_ADDR(0x0A06)
#define EMA_SUBID                       PPE_REG_ADDR(0x0A07)


/*
 *  QSB RAM Access Register
 */
#define QSB_RAMAC                       QSB_CONF_REG(0x000D)

#define QSB_RAMAC_RW                    (*QSB_RAMAC & (1 << 31))
#define QSB_RAMAC_TSEL                  GET_BITS(*QSB_RAMAC, 27, 24)
#define QSB_RAMAC_LH                    (*QSB_RAMAC & (1 << 16))
#define QSB_RAMAC_TESEL                 GET_BITS(*QSB_RAMAC, 9, 0)

#define QSB_RAMAC_RW_SET(value)         ((value) ? (1 << 31) : 0)
#define QSB_RAMAC_TSEL_SET(value)       SET_BITS(0, 27, 24, value)
#define QSB_RAMAC_LH_SET(value)         ((value) ? (1 << 16) : 0)
#define QSB_RAMAC_TESEL_SET(value)      SET_BITS(0, 9, 0, value)

/*  QSB */
#define QSB_RAMAC_RW_READ               0
#define QSB_RAMAC_RW_WRITE              1

#define QSB_RAMAC_TSEL_QPT              0x01
#define QSB_RAMAC_TSEL_SCT              0x02
#define QSB_RAMAC_TSEL_SPT              0x03
#define QSB_RAMAC_TSEL_VBR              0x08

#define QSB_RAMAC_LH_LOW                0
#define QSB_RAMAC_LH_HIGH               1

#define QSB_QPT_SET_MASK                0x0
#define QSB_QVPT_SET_MASK               0x0
#define QSB_SET_SCT_MASK                0x0
#define QSB_SET_SPT_MASK                0x0
#define QSB_SET_SPT_SBVALID_MASK        0x7FFFFFFF

#define QSB_SPT_SBV_VALID               (1 << 31)
#define QSB_SPT_PN_SET(value)           (((value) & 0x01) ? (1 << 16) : 0)
#define QSB_SPT_INTRATE_SET(value)      SET_BITS(0, 13, 0, value)

/*
 *  QSB Internal Cell Delay Variation Register
 */
#define QSB_ICDV                        QSB_CONF_REG(0x0007)

#define QSB_ICDV_TAU                    GET_BITS(*QSB_ICDV, 5, 0)

#define QSB_ICDV_TAU_SET(value)         SET_BITS(0, 5, 0, value)

/*
 *  QSB Scheduler Burst Limit Register
 */
#define QSB_SBL                         QSB_CONF_REG(0x0009)

#define QSB_SBL_SBL                     GET_BITS(*QSB_SBL, 3, 0)

#define QSB_SBL_SBL_SET(value)          SET_BITS(0, 3, 0, value)

/*
 *  QSB Configuration Register
 */
#define QSB_CFG                         QSB_CONF_REG(0x000A)

#define QSB_CFG_TSTEPC                  GET_BITS(*QSB_CFG, 1, 0)

#define QSB_CFG_TSTEPC_SET(value)       SET_BITS(0, 1, 0, value)

/*
 *  QSB RAM Transfer Table Register
 */
#define QSB_RTM                         QSB_CONF_REG(0x000B)

#define QSB_RTM_DM                      (*QSB_RTM)

#define QSB_RTM_DM_SET(value)           ((value) & 0xFFFFFFFF)

/*
 *  QSB RAM Transfer Data Register
 */
#define QSB_RTD                         QSB_CONF_REG(0x000C)

#define QSB_RTD_TTV                     (*QSB_RTD)

#define QSB_RTD_TTV_SET(value)          ((value) & 0xFFFFFFFF)

/*
 *  PP32 Debug Control Register
 */
#define PP32_DBG_CTRL                   PP32_DEBUG_REG_ADDR(0x0000)

#define DBG_CTRL_START_SET(value)       ((value) ? (1 << 0) : 0)
#define DBG_CTRL_STOP_SET(value)        ((value) ? (1 << 1) : 0)
#define DBG_CTRL_STEP_SET(value)        ((value) ? (1 << 2) : 0)

#define SB_RAM0_ADDR(x)                 ((volatile u32*)(DANUBE_PPE + (((x) + 0x8000) << 2)))
#define UPDATE_VCC_STAT(conn, item, num)    do { ppe_dev.connection[conn].item += num; } while (0)
/*
 *  EMA Settings
 */
#define EMA_CMD_BUF_LEN      0x0040
#define EMA_CMD_BASE_ADDR    (0x00001580 << 2)
#define EMA_DATA_BUF_LEN     0x0100
#define EMA_DATA_BASE_ADDR   (0x00001900 << 2)
#define EMA_WRITE_BURST      0x2
#define EMA_READ_BURST       0x2


#define CELL_SIZE                       ATM_AAL0_SDU
#define IDLE_CYCLE_NUMBER               30000

#define MBOX_IGU1_ISR                   PPE_REG_ADDR(0x0206)
#define MBOX_IGU3_ISRS                  PPE_REG_ADDR(0x0214)
#define MBOX_IGU1_ISRC                  PPE_REG_ADDR(0x0205)
#define MBOX_IGU3_ISR                   PPE_REG_ADDR(0x0216)
#define MBOX_IGU3_ISRS_SET(n)           (1 << (n))
#define MBOX_IGU3_ISR_ISR(n)            (*MBOX_IGU3_ISR & (1 << (n)))
/*
 *  *  Mailbox IGU1 Registers
 *   */
#define MBOX_IGU1_ISRS                  PPE_REG_ADDR(0x0204)
#define MBOX_IGU1_IER                   PPE_REG_ADDR(0x0207)

#define MBOX_IGU1_ISRS_SET(n)           (1 << (n))
#define MBOX_IGU1_ISRC_CLEAR(n)         (1 << (n))
#define MBOX_IGU1_ISR_ISR(n)            (*MBOX_IGU1_ISR & (1 << (n)))
#define MBOX_IGU1_IER_EN(n)             (*MBOX_IGU1_IER & (1 << (n)))
#define MBOX_IGU1_IER_EN_SET(n)         (1 << (n))

/*
 *  *  Mailbox IGU3 Registers
 *   */
#define MBOX_IGU3_ISRC                  PPE_REG_ADDR(0x0215)
#define MBOX_IGU3_IER                   PPE_REG_ADDR(0x0217)

#define MBOX_IGU3_ISRS_SET(n)           (1 << (n))
#define MBOX_IGU3_ISRC_CLEAR(n)         (1 << (n))
#define MBOX_IGU3_ISR_ISR(n)            (*MBOX_IGU3_ISR & (1 << (n)))
#define MBOX_IGU3_IER_EN(n)             (*MBOX_IGU3_IER & (1 << (n)))
#define MBOX_IGU3_IER_EN_SET(n)         (1 << (n))


// RX Frame Definitions
#define MAX_RX_PACKET_ALIGN_BYTES       3
#define MAX_RX_PACKET_PADDING_BYTES     3
#define RX_INBAND_TRAILER_LENGTH        8
#define MAX_RX_FRAME_EXTRA_BYTES        (RX_INBAND_TRAILER_LENGTH + MAX_RX_PACKET_ALIGN_BYTES + MAX_RX_PACKET_PADDING_BYTES)

// TX Frame Definitions
#define MAX_TX_HEADER_ALIGN_BYTES       12
#define MAX_TX_PACKET_ALIGN_BYTES       3
#define MAX_TX_PACKET_PADDING_BYTES     3
#define TX_INBAND_HEADER_LENGTH         8
#define MAX_TX_FRAME_EXTRA_BYTES        (TX_INBAND_HEADER_LENGTH + MAX_TX_HEADER_ALIGN_BYTES + MAX_TX_PACKET_ALIGN_BYTES + MAX_TX_PACKET_PADDING_BYTES)


// DWORD-Length of Memory Blocks
#define PP32_DEBUG_REG_DWLEN            0x0030
#define PPM_INT_REG_DWLEN               0x0010
#define PP32_INTERNAL_RES_DWLEN         0x00C0
#define PPE_CLOCK_CONTROL_DWLEN         0x0F00
#define CDM_CODE_MEMORY_RAM0_DWLEN      0x1000
#define CDM_CODE_MEMORY_RAM1_DWLEN      0x0800
#define PPE_REG_DWLEN                   0x1000
#define PP32_DATA_MEMORY_RAM1_DWLEN     0x0800
#define PPM_INT_UNIT_DWLEN              0x0100
#define PPM_TIMER0_DWLEN                0x0100
#define PPM_TASK_IND_REG_DWLEN          0x0100
#define PPS_BRK_DWLEN                   0x0100
#define PPM_TIMER1_DWLEN                0x0100
#define SB_RAM0_DWLEN                   0x0400
#define SB_RAM1_DWLEN                   0x0800
#define SB_RAM2_DWLEN                   0x0A00
#define SB_RAM3_DWLEN                   0x0400
#define QSB_CONF_REG_DWLEN              0x0100
/*
 *  QSB Queue Scheduling and Shaping Definitions
 */
#define QSB_WFQ_NONUBR_MAX              0x3f00
#define QSB_WFQ_UBR_BYPASS              0x3fff
#define QSB_TP_TS_MAX                   65472
#define QSB_TAUS_MAX                    64512
#define QSB_GCR_MIN                     18



// OAM Definitions
#define OAM_RX_QUEUE_NUMBER             1
#define OAM_TX_QUEUE_NUMBER_PER_PORT    0
#define OAM_RX_DMA_CHANNEL_NUMBER       OAM_RX_QUEUE_NUMBER
#define OAM_HTU_ENTRY_NUMBER            3
#define OAM_F4_SEG_HTU_ENTRY            0
#define OAM_F4_TOT_HTU_ENTRY            1
#define OAM_F5_HTU_ENTRY                2
#define OAM_F4_CELL_ID                  0
#define OAM_F5_CELL_ID                  15

// ATM Port, QSB Queue, DMA RX/TX Channel Parameters
#define ATM_PORT_NUMBER                 2
#define MAX_QUEUE_NUMBER                16
#define QSB_QUEUE_NUMBER_BASE           1
#define MAX_QUEUE_NUMBER_PER_PORT       (MAX_QUEUE_NUMBER - QSB_QUEUE_NUMBER_BASE)
#define MAX_CONNECTION_NUMBER           MAX_QUEUE_NUMBER
#define MAX_RX_DMA_CHANNEL_NUMBER       8
#define MAX_TX_DMA_CHANNEL_NUMBER       16
#define DMA_ALIGNMENT                   4

#define DEFAULT_RX_HUNT_BITTH           4

/*
 *  FPI Configuration Bus Register and Memory Address Mapping
 */
#define DANUBE_PPE                      (KSEG1 + 0x1E180000)
#define PP32_DEBUG_REG_ADDR(x)          ((volatile u32*)(DANUBE_PPE + (((x) + 0x0000) << 2)))
#define PPM_INT_REG_ADDR(x)             ((volatile u32*)(DANUBE_PPE + (((x) + 0x0030) << 2)))
#define PP32_INTERNAL_RES_ADDR(x)       ((volatile u32*)(DANUBE_PPE + (((x) + 0x0040) << 2)))
#define PPE_CLOCK_CONTROL_ADDR(x)       ((volatile u32*)(DANUBE_PPE + (((x) + 0x0100) << 2)))
#define CDM_CODE_MEMORY_RAM0_ADDR(x)    ((volatile u32*)(DANUBE_PPE + (((x) + 0x1000) << 2)))
#define CDM_CODE_MEMORY_RAM1_ADDR(x)    ((volatile u32*)(DANUBE_PPE + (((x) + 0x2000) << 2)))
#define PPE_REG_ADDR(x)                 ((volatile u32*)(DANUBE_PPE + (((x) + 0x4000) << 2)))
#define PP32_DATA_MEMORY_RAM1_ADDR(x)   ((volatile u32*)(DANUBE_PPE + (((x) + 0x5000) << 2)))
#define PPM_INT_UNIT_ADDR(x)            ((volatile u32*)(DANUBE_PPE + (((x) + 0x6000) << 2)))
#define PPM_TIMER0_ADDR(x)              ((volatile u32*)(DANUBE_PPE + (((x) + 0x6100) << 2)))
#define PPM_TASK_IND_REG_ADDR(x)        ((volatile u32*)(DANUBE_PPE + (((x) + 0x6200) << 2)))
#define PPS_BRK_ADDR(x)                 ((volatile u32*)(DANUBE_PPE + (((x) + 0x6300) << 2)))
#define PPM_TIMER1_ADDR(x)              ((volatile u32*)(DANUBE_PPE + (((x) + 0x6400) << 2)))
#define SB_RAM0_ADDR(x)                 ((volatile u32*)(DANUBE_PPE + (((x) + 0x8000) << 2)))
#define SB_RAM1_ADDR(x)                 ((volatile u32*)(DANUBE_PPE + (((x) + 0x8400) << 2)))
#define SB_RAM2_ADDR(x)                 ((volatile u32*)(DANUBE_PPE + (((x) + 0x8C00) << 2)))
#define SB_RAM3_ADDR(x)                 ((volatile u32*)(DANUBE_PPE + (((x) + 0x9600) << 2)))
#define QSB_CONF_REG(x)                 ((volatile u32*)(DANUBE_PPE + (((x) + 0xC000) << 2)))

/*
 *  Host-PPE Communication Data Address Mapping
 */
#define CFG_WRX_HTUTS                   PPM_INT_UNIT_ADDR(0x2400)   /*  WAN RX HTU Table Size, must be configured before enable PPE firmware.   */
#define CFG_WRX_QNUM                    PPM_INT_UNIT_ADDR(0x2401)   /*  WAN RX Queue Number */
#define CFG_WRX_DCHNUM                  PPM_INT_UNIT_ADDR(0x2402)   /*  WAN RX DMA Channel Number, no more than 8, must be configured before enable PPE firmware.   */
#define CFG_WTX_DCHNUM                  PPM_INT_UNIT_ADDR(0x2403)   /*  WAN TX DMA Channel Number, no more than 16, must be configured before enable PPE firmware.  */
#define CFG_WRDES_DELAY                 PPM_INT_UNIT_ADDR(0x2404)   /*  WAN Descriptor Write Delay, must be configured before enable PPE firmware.  */
#define WRX_DMACH_ON                    PPM_INT_UNIT_ADDR(0x2405)   /*  WAN RX DMA Channel Enable, must be configured before enable PPE firmware.   */
#define WTX_DMACH_ON                    PPM_INT_UNIT_ADDR(0x2406)   /*  WAN TX DMA Channel Enable, must be configured before enable PPE firmware.   */
#define WRX_HUNT_BITTH                  PPM_INT_UNIT_ADDR(0x2407)   /*  WAN RX HUNT Threshold, must be between 2 to 8.  */
#define WRX_QUEUE_CONFIG(i)             ((struct wrx_queue_config*)PPM_INT_UNIT_ADDR(0x2500 + (i) * 20))
#define WRX_DMA_CHANNEL_CONFIG(i)       ((struct wrx_dma_channel_config*)PPM_INT_UNIT_ADDR(0x2640 + (i) * 7))
#define WTX_PORT_CONFIG(i)              ((struct wtx_port_config*)PPM_INT_UNIT_ADDR(0x2440 + (i)))
#define WTX_QUEUE_CONFIG(i)             ((struct wtx_queue_config*)PPM_INT_UNIT_ADDR(0x2710 + (i) * 27))
#define WTX_DMA_CHANNEL_CONFIG(i)       ((struct wtx_dma_channel_config*)PPM_INT_UNIT_ADDR(0x2711 + (i) * 27))
#define WAN_MIB_TABLE                   ((struct wan_mib_table*)PPM_INT_UNIT_ADDR(0x2410))
#define HTU_ENTRY(i)                    ((struct htu_entry*)PPM_INT_UNIT_ADDR(0x2000 + (i)))
#define HTU_MASK(i)                     ((struct htu_mask*)PPM_INT_UNIT_ADDR(0x2020 + (i)))
#define HTU_RESULT(i)                   ((struct htu_result*)PPM_INT_UNIT_ADDR(0x2040 + (i)))

// DREG Idle Counters
#define DREG_AT_CELL0                   PPE_REG_ADDR(0x0D24)
#define DREG_AT_CELL1                   PPE_REG_ADDR(0x0D25)
#define DREG_AT_IDLE_CNT0               PPE_REG_ADDR(0x0D26)
#define DREG_AT_IDLE_CNT1               PPE_REG_ADDR(0x0D27)
#define DREG_AR_CELL0                   PPE_REG_ADDR(0x0D68)
#define DREG_AR_CELL1                   PPE_REG_ADDR(0x0D69)
#define DREG_AR_IDLE_CNT0               PPE_REG_ADDR(0x0D6A)
#define DREG_AR_IDLE_CNT1               PPE_REG_ADDR(0x0D6B)
#define DREG_AR_AIIDLE_CNT0             PPE_REG_ADDR(0x0D6C)
#define DREG_AR_AIIDLE_CNT1             PPE_REG_ADDR(0x0D6D)
#define DREG_AR_BE_CNT0                 PPE_REG_ADDR(0x0D6E)
#define DREG_AR_BE_CNT1                 PPE_REG_ADDR(0x0D6F)


/*
 *  64-bit Data Type
 */
typedef struct {
    unsigned int    h: 32;
    unsigned int    l: 32;
} ppe_u64_t;

/*
 *  PPE ATM Cell Header
 */
#if defined(__BIG_ENDIAN)
    struct uni_cell_header {
        unsigned int        gfc     :4;
        unsigned int        vpi     :8;
        unsigned int        vci     :16;
        unsigned int        pti     :3;
        unsigned int        clp     :1;
    };
#else
    struct uni_cell_header {
        unsigned int        clp     :1;
        unsigned int        pti     :3;
        unsigned int        vci     :16;
        unsigned int        vpi     :8;
        unsigned int        gfc     :4;
    };
#endif  //  defined(__BIG_ENDIAN)

/*
 *  Inband Header and Trailer
 */
#if defined(__BIG_ENDIAN)
    struct rx_inband_trailer {
        /*  0 - 3h  */
        unsigned int        uu      :8;
        unsigned int        cpi     :8;
        unsigned int        stw_res1:4;
        unsigned int        stw_clp :1;
        unsigned int        stw_ec  :1;
        unsigned int        stw_uu  :1;
        unsigned int        stw_cpi :1;
        unsigned int        stw_ovz :1;
        unsigned int        stw_mfl :1;
        unsigned int        stw_usz :1;
        unsigned int        stw_crc :1;
        unsigned int        stw_il  :1;
        unsigned int        stw_ra  :1;
        unsigned int        stw_res2:2;
        /*  4 - 7h  */
        unsigned int        gfc     :4;
        unsigned int        vpi     :8;
        unsigned int        vci     :16;
        unsigned int        pti     :3;
        unsigned int        clp     :1;
    };

    struct tx_inband_header {
        /*  0 - 3h  */
        unsigned int        gfc     :4;
        unsigned int        vpi     :8;
        unsigned int        vci     :16;
        unsigned int        pti     :3;
        unsigned int        clp     :1;
        /*  4 - 7h  */
        unsigned int        uu      :8;
        unsigned int        cpi     :8;
        unsigned int        pad     :8;
        unsigned int        res1    :8;
    };
#else
    struct rx_inband_trailer {
        /*  0 - 3h  */
        unsigned int        stw_res2:2;
        unsigned int        stw_ra  :1;
        unsigned int        stw_il  :1;
        unsigned int        stw_crc :1;
        unsigned int        stw_usz :1;
        unsigned int        stw_mfl :1;
        unsigned int        stw_ovz :1;
        unsigned int        stw_cpi :1;
        unsigned int        stw_uu  :1;
        unsigned int        stw_ec  :1;
        unsigned int        stw_clp :1;
        unsigned int        stw_res1:4;
        unsigned int        cpi     :8;
        unsigned int        uu      :8;
        /*  4 - 7h  */
        unsigned int        clp     :1;
        unsigned int        pti     :3;
        unsigned int        vci     :16;
        unsigned int        vpi     :8;
        unsigned int        gfc     :4;
    };

    struct tx_inband_header {
        /*  0 - 3h  */
        unsigned int        clp     :1;
        unsigned int        pti     :3;
        unsigned int        vci     :16;
        unsigned int        vpi     :8;
        unsigned int        gfc     :4;
        /*  4 - 7h  */
        unsigned int        res1    :8;
        unsigned int        pad     :8;
        unsigned int        cpi     :8;
        unsigned int        uu      :8;
    };
#endif  //  defined(__BIG_ENDIAN)

struct wan_mib_table {
    unsigned int                     res1;
    unsigned int                     wrx_drophtu_cell;
    unsigned int                     wrx_dropdes_pdu;
    unsigned int                     wrx_correct_pdu;
    unsigned int                     wrx_err_pdu;
    unsigned int                     wrx_dropdes_cell;
    unsigned int                     wrx_correct_cell;
    unsigned int                     wrx_err_cell;
    unsigned int                     wrx_total_byte;
    unsigned int                     wtx_total_pdu;
    unsigned int                     wtx_total_cell;
    unsigned int                     wtx_total_byte;
};

/*
 *  Internal Structure of Device
 */
struct port {
    int                     connection_base;        /*  first connection ID (RX/TX queue ID)    */
    unsigned int                     max_connections;        /*  maximum connection number               */
    unsigned int                     connection_table;       /*  connection opened status, every bit     */
    unsigned int                     tx_max_cell_rate;       /*  maximum cell rate                       */
    unsigned int                     tx_current_cell_rate;   /*  currently used cell rate                */
#if !defined(ENABLE_RX_QOS) || !ENABLE_RX_QOS
    int                     rx_dma_channel_base;    /*  first RX DMA channel ID                 */
    unsigned int                     rx_dma_channel_assigned;/*  totally RX DMA channels used            */
#endif  //  !defined(ENABLE_RX_QOS) || !ENABLE_RX_QOS
    int                     oam_tx_queue;           /*  first TX queue ID of OAM cell           */
    struct atm_dev          *dev;

};

struct connection {
    struct atm_vcc          *vcc;                   /*  opened VCC                              */
    struct timespec         access_time;            /*  time when last F4/F5 user cell arrives  */
    unsigned int                     aal5_vcc_crc_err;       /*  number of packets with CRC error        */
    unsigned int                     aal5_vcc_oversize_sdu;  /*  number of packets with oversize error   */
    int                     rx_dma_channel;         /*  RX DMA channel ID assigned              */
    int                     port;                   /*  to which port the connection belongs    */
    unsigned int                     rx_pdu;
    unsigned int                     rx_err_pdu;
    unsigned int                     rx_sw_drop_pdu;
    unsigned int                     tx_pdu;
    unsigned int                     tx_err_pdu;
    unsigned int                     tx_hw_drop_pdu;
    unsigned int                     tx_sw_drop_pdu;
};

struct ppe_dev {
    struct connection       connection[MAX_CONNECTION_NUMBER];
    struct port             port[ATM_PORT_NUMBER];

    struct aal5 {
        unsigned char              padding_byte;               /*  padding byte pattern of AAL5 packet     */
        unsigned int             rx_max_packet_size;         /*  max AAL5 packet length                  */
        unsigned int             rx_min_packet_size;         /*  min AAL5 packet length                  */
        unsigned int             rx_buffer_size;             /*  max memory allocated for a AAL5 packet  */
        unsigned int             tx_max_packet_size;         /*  max AAL5 packet length                  */
        unsigned int             tx_min_packet_size;         /*  min AAL5 packet length                  */
        unsigned int             tx_buffer_size;             /*  max memory allocated for a AAL5 packet  */
        unsigned int    rx_drop_error_packet;       /*  1: drop error packet, 0: ignore errors  */
    }                       aal5;

    struct qsb {
        unsigned int             tau;                        /*  cell delay variation due to concurrency */
        unsigned int             tstepc;                     /*  shceduler burst length                  */
        unsigned int             sbl;                        /*  time step                               */
    }                       qsb;

    struct dma {
        unsigned int             rx_descriptor_number;       /*  number of RX descriptors                */
        unsigned int             tx_descriptor_number;       /*  number of TX descriptors                */
        unsigned int             rx_clp1_desc_threshold;     /*  threshold to drop cells with CLP1       */
        unsigned int             write_descriptor_delay;     /*  delay on descriptor write path          */
        unsigned int             rx_total_channel_used;      /*  total RX channel used                   */
        void            *rx_descriptor_addr;        /*  base address of memory allocated for    */
        struct rx_descriptor
                        *rx_descriptor_base;        /*  base address of RX descriptors          */
        int             rx_desc_read_pos[MAX_RX_DMA_CHANNEL_NUMBER];    /*  first RX descriptor */
                                                                        /*  to be read          */
//        struct sk_buff  **rx_skb_pointers;          /*  base address of RX sk_buff pointers     */

#if defined(ENABLE_RX_QOS) && ENABLE_RX_QOS
        long            rx_weight[MAX_RX_DMA_CHANNEL_NUMBER];           /*  RX schedule weight  */
        long            rx_default_weight[MAX_RX_DMA_CHANNEL_NUMBER];   /*  default weight      */
#endif

        unsigned int             tx_total_channel_used;      /*  total TX channel used                   */
        void            *tx_descriptor_addr;        /*  base address of memory allocated for    */
                                                    /*  TX descriptors                          */
        struct tx_descriptor
                        *tx_descriptor_base;        /*  base address of TX descriptors          */
        int             tx_desc_alloc_pos[MAX_TX_DMA_CHANNEL_NUMBER];   /*  first TX descriptor */
                                                                        /*  could be allocated  */
//        int             tx_desc_alloc_num[MAX_TX_DMA_CHANNEL_NUMBER];   /*  number of allocated */
//                                                                        /*  TX descriptors      */
        int             tx_desc_alloc_flag[MAX_TX_DMA_CHANNEL_NUMBER];  /*  at least one TX     */
                                                                        /*  descriptor is alloc */
//        int             tx_desc_send_pos[MAX_TX_DMA_CHANNEL_NUMBER];    /*  first TX descriptor */
//                                                                        /*  to be send          */
        int             tx_desc_release_pos[MAX_TX_DMA_CHANNEL_NUMBER]; /*  first TX descriptor */
                                                                        /*  to be released      */
        struct sk_buff  **tx_skb_pointers;          /*  base address of TX sk_buff pointers     */
    }                       dma;

    struct mib {
        ppe_u64_t       wrx_total_byte;             /*  bit-64 extention of MIB table member    */
        ppe_u64_t       wtx_total_byte;             /*  bit-64 extention of MIB talbe member    */

        unsigned int             wrx_pdu;                    /*  successfully received AAL5 packet       */
        unsigned int             wrx_drop_pdu;               /*  AAL5 packet dropped by driver on RX     */
        unsigned int             wtx_err_pdu;                /*  error AAL5 packet                       */
        unsigned int             wtx_drop_pdu;               /*  AAL5 packet dropped by driver on TX     */
    }                       mib;
    struct wan_mib_table    prev_mib;

    int                     oam_rx_queue;           /*  RX queue ID of OAM cell                 */
    int                     oam_rx_dma_channel;     /*  RX DMA channel ID of OAM cell           */
    int                     max_connections;        /*  total connections available             */

    struct semaphore        sem;                    /*  lock used by open/close function        */
};

/*
 *  Host-PPE Communication Data Structure
 */
#if defined(__BIG_ENDIAN)
    struct wrx_queue_config {
        /*  0h  */
        unsigned int    res2        :27;
        unsigned int    dmach       :4;
        unsigned int    errdp       :1;
        /*  1h  */
        unsigned int    oversize    :16;
        unsigned int    undersize   :16;
        /*  2h  */
        unsigned int    res1        :16;
        unsigned int    mfs         :16;
        /*  3h  */
        unsigned int    uumask      :8;
        unsigned int    cpimask     :8;
        unsigned int    uuexp       :8;
        unsigned int    cpiexp      :8;
    };

    struct wtx_port_config {
        unsigned int    res1        :27;
        unsigned int    qid         :4;
        unsigned int    qsben       :1;
    };

    struct wtx_queue_config {
        unsigned int    res1        :25;
        unsigned int    sbid        :1;
        unsigned int    res2        :3;
        unsigned int    type        :2;
        unsigned int    qsben       :1;
    };

    struct wrx_dma_channel_config {
        /*  0h  */
        unsigned int    res1        :1;
        unsigned int    mode        :2;
        unsigned int    rlcfg       :1;
        unsigned int    desba       :28;
        /*  1h  */
        unsigned int    chrl        :16;
        unsigned int    clp1th      :16;
        /*  2h  */
        unsigned int    deslen      :16;
        unsigned int    vlddes      :16;
    };

    struct wtx_dma_channel_config {
        /*  0h  */
        unsigned int    res2        :1;
        unsigned int    mode        :2;
        unsigned int    res3        :1;
        unsigned int    desba       :28;
        /*  1h  */
        unsigned int    res1        :32;
        /*  2h  */
        unsigned int    deslen      :16;
        unsigned int    vlddes      :16;
    };

    struct htu_entry {
        unsigned int    res1        :2;
        unsigned int    pid         :2;
        unsigned int    vpi         :8;
        unsigned int    vci         :16;
        unsigned int    pti         :3;
        unsigned int    vld         :1;
    };

    struct htu_mask {
        unsigned int    set         :2;
        unsigned int    pid_mask    :2;
        unsigned int    vpi_mask    :8;
        unsigned int    vci_mask    :16;
        unsigned int    pti_mask    :3;
        unsigned int    clear       :1;
    };

   struct htu_result {
        unsigned int    res1        :12;
        unsigned int    cellid      :4;
        unsigned int    res2        :5;
        unsigned int    type        :1;
        unsigned int    ven         :1;
        unsigned int    res3        :5;
        unsigned int    qid         :4;
    };

    struct rx_descriptor {
        /*  0 - 3h  */
        unsigned int    own         :1;
        unsigned int    c           :1;
        unsigned int    sop         :1;
        unsigned int    eop         :1;
        unsigned int    res1        :3;
        unsigned int    byteoff     :2;
        unsigned int    res2        :2;
        unsigned int    id          :4;
        unsigned int    err         :1;
        unsigned int    datalen     :16;
        /*  4 - 7h  */
        unsigned int    res3        :4;
        unsigned int    dataptr     :28;
    };

    struct tx_descriptor {
        /*  0 - 3h  */
        unsigned int    own         :1;
        unsigned int    c           :1;
        unsigned int    sop         :1;
        unsigned int    eop         :1;
        unsigned int    byteoff     :5;
        unsigned int    res1        :5;
        unsigned int    iscell      :1;
        unsigned int    clp         :1;
        unsigned int    datalen     :16;
        /*  4 - 7h  */
        unsigned int    res2        :4;
        unsigned int    dataptr     :28;
    };
#else
    struct wrx_queue_config {
        /*  0h  */
        unsigned int    errdp       :1;
        unsigned int    dmach       :4;
        unsigned int    res2        :27;
        /*  1h  */
        unsigned int    undersize   :16;
        unsigned int    oversize    :16;
        /*  2h  */
        unsigned int    mfs         :16;
        unsigned int    res1        :16;
        /*  3h  */
        unsigned int    cpiexp      :8;
        unsigned int    uuexp       :8;
        unsigned int    cpimask     :8;
        unsigned int    uumask      :8;
    };

    struct wtx_port_config {
        unsigned int    qsben       :1;
        unsigned int    qid         :4;
        unsigned int    res1        :27;
    };

    struct wtx_queue_config {
        unsigned int    qsben       :1;
        unsigned int    type        :2;
        unsigned int    res2        :3;
        unsigned int    sbid        :1;
        unsigned int    res1        :25;
    };

    struct wrx_dma_channel_config
    {
        /*  0h  */
        unsigned int    desba       :28;
        unsigned int    rlcfg       :1;
        unsigned int    mode        :2;
        unsigned int    res1        :1;
        /*  1h  */
        unsigned int    clp1th      :16;
        unsigned int    chrl        :16;
        /*  2h  */
        unsigned int    vlddes      :16;
        unsigned int    deslen      :16;
    };

    struct wtx_dma_channel_config {
        /*  0h  */
        unsigned int    desba       :28;
        unsigned int    res3        :1;
        unsigned int    mode        :2;
        unsigned int    res2        :1;
        /*  1h  */
        unsigned int    res1        :32;
        /*  2h  */
        unsigned int    vlddes      :16;
        unsigned int    deslen      :16;
    };

    struct rx_descriptor {
        /*  4 - 7h  */
        unsigned int    dataptr     :28;
        unsigned int    res3        :4;
        /*  0 - 3h  */
        unsigned int    datalen     :16;
        unsigned int    err         :1;
        unsigned int    id          :4;
        unsigned int    res2        :2;
        unsigned int    byteoff     :2;
        unsigned int    res1        :3;
        unsigned int    eop         :1;
        unsigned int    sop         :1;
        unsigned int    c           :1;
        unsigned int    own         :1;
    };

    struct tx_descriptor {
        /*  4 - 7h  */
        unsigned int    dataptr     :28;
        unsigned int    res2        :4;
        /*  0 - 3h  */
        unsigned int    datalen     :16;
        unsigned int    clp         :1;
        unsigned int    iscell      :1;
        unsigned int    res1        :5;
        unsigned int    byteoff     :5;
        unsigned int    eop         :1;
        unsigned int    sop         :1;
        unsigned int    c           :1;
        unsigned int    own         :1;
    };
#endif  //  defined(__BIG_ENDIAN)

/*
 *  QSB Queue Parameter Table Entry and Queue VBR Parameter Table Entry
 */
#if defined(__BIG_ENDIAN)
    union qsb_queue_parameter_table {
        struct {
            unsigned int    res1    :1;
            unsigned int    vbr     :1;
            unsigned int    wfqf    :14;
            unsigned int    tp      :16;
        }               bit;
        unsigned int             dword;
    };

    union qsb_queue_vbr_parameter_table {
        struct {
            unsigned int    taus    :16;
            unsigned int    ts      :16;
        }               bit;
        unsigned int             dword;
    };
#else
    union qsb_queue_parameter_table {
        struct {
            unsigned int    tp      :16;
            unsigned int    wfqf    :14;
            unsigned int    vbr     :1;
            unsigned int    res1    :1;
        }               bit;
        unsigned int             dword;
    };

    union qsb_queue_vbr_parameter_table {
        struct {
            unsigned int    ts      :16;
            unsigned int    taus    :16;
        }               bit;
        unsigned int             dword;
    };
#endif  //  defined(__BIG_ENDIAN)


typedef enum
{
    IAD_ATM_CBR     = 6,						/* IAD_ATM_PRI_HIGH,							*/
    IAD_ATM_VBR_RT  = 4,						/* IAD_ATM_PRI_MED_HIGH,	VBR, Real-Time 		*/
    IAD_ATM_VBR_NRT = 2,						/* IAD_ATM_PRI_MED_LOW,     VBR, Non-Real-Time 	*/
    IAD_ATM_UBR     = 0,						/* IAD_ATM_PRI_LOW		  						*/
} iad_atmServiceCategory;

typedef	unsigned int iad_atmDiffServCategory;

typedef struct 
{
   int cellRate;
   int round;   /* IAD_ATM_RATE_CEILING, IAD_ATM_RATE_FLOOR */
} iad_atmCellRateDesc;

typedef struct 
{
    unsigned int               phyID;          /* IAD_ATM_PHY0, IAD_ATM_PHY1 */
    unsigned int               txQHnd;         /* Tx HW Q */
    union _pri
    {
        int                     priority;       /* TS Q:    4 priorities: IAD_ATM_PRI_HIGH, IAD_ATM_PRI_MED_HIGH, IAD_ATM_PRI_MED_LOW, IAD_ATM_PRI_LOW
                                                  non-TS Q: 8 priorities: IAD_ATM_PRI_LEVEL_7, IAD_ATM_PRI_LEVEL_6,..., IAD_ATM_PRI_LEVEL_0 */
        iad_atmServiceCategory  qosClass;       /* IAD_ATM_CBR, IAD_ATM_VBR_RT, IAD_ATM_VBR_NRT, IAD_ATM_UBR */
        iad_atmDiffServCategory diffServClass;  /* IP_QOS */
    } srvCat; /* service category */
    iad_atmCellRateDesc         pcr;            /* Peak Cell Rate */
    iad_atmCellRateDesc         scr;            /* Sustained Cell Rate. */
    iad_atmCellRateDesc         mcr;            /* Minimum Cell Rate, not used */
    int                         mbs;            /* maximum bursting size in cells */
    int                         isPrioritize;   /* TRUE: This flow is of the higher priority than the flows of the same QOS category.(Use MCR to boost priority) */
} iad_atmTrfPar; /* Tx Traffic Parameters */

typedef struct
{
    unsigned int    txGrpId;
    unsigned int    flowId;
    iad_atmTrfPar   trfPar;
} Atm_Ictl_Flow_Set;

typedef struct
{
    unsigned int    txGrpId;
    unsigned int    vpi;
    unsigned int    vci;

    unsigned int    encaps;
    unsigned int    proto;

} Atm_Ictl_Open_Vcc;

typedef struct
{
    struct atm_vcc			vcc;
	unsigned int 			valid;
	unsigned int 			on;
    unsigned int			vccIndex; /* 0~7 */
	unsigned int 			itf;
    struct net_device_stats	stats; 
} Atm_Priv;


extern struct ppe_dev ppe_dev;


int pp32_start(void);
void pp32_stop(void);
void init_rx_tables(void);
void init_tx_tables(void);
struct sk_buff* alloc_skb_rx(void);
struct sk_buff* alloc_skb_tx(unsigned int);
void resize_skb_rx(struct sk_buff *, unsigned int, int);
struct sk_buff* atm_alloc_tx(struct atm_vcc *, unsigned int);
void atm_free_tx_skb_vcc(struct sk_buff *);
int alloc_tx_connection(int);
int ppe_open(struct atm_vcc *vcc);
void ppe_close(struct atm_vcc *vcc);
int ppe_ioctl(struct atm_dev *dev, unsigned int cmd, void *arg);
int ppe_send(struct atm_vcc *vcc, struct sk_buff *skb);
int ppe_send_oam(struct atm_vcc *vcc, void *cell, int flags);
int ppe_change_qos(struct atm_vcc *vcc, struct atm_qos *qos, int flags);
irqreturn_t mailbox_irq_handler(int, void *);
int find_vcc(struct atm_vcc *vcc);
int find_vpi(unsigned int vpi);
int find_vpivci(unsigned int vpi, unsigned int vci);
void mailbox_signal(unsigned int channel, int is_tx);

