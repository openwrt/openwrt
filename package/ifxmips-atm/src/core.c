#include <asm/mach-ifxmips/cgu.h>
#include <linux/module.h>
#include <linux/atmdev.h>
#include <linux/irq.h>

#include "common.h"
#include "proc.h"

// our main struct
struct ppe_dev ppe_dev;

static int port_max_connection[2] = {7, 7};     /*  Maximum number of connections for ports (0-14)  */
static int port_cell_rate_up[2] = {3200, 3200}; /*  Maximum TX cell rate for ports                  */
static int qsb_tau = 1;
static int qsb_srvm = 0x0f;
static int qsb_tstep = 4;
static int write_descriptor_delay = 0x20;
static int aal5_fill_pattern = 0x007E;
static int aal5r_max_packet_size = 0x0700;
static int aal5r_min_packet_size = 0x0000;
static int aal5s_max_packet_size = 0x0700;
static int aal5s_min_packet_size = 0x0000;
static int aal5r_drop_error_packet = 1;
static int dma_rx_descriptor_length = 48;
static int dma_tx_descriptor_length = 64;
static int dma_rx_clp1_descriptor_threshold = 38;

//module_param(port_max_connection, "2-2i");
//module_param(port_cell_rate_up, "2-2i");
module_param(qsb_tau, int, 0);
module_param(qsb_srvm, int, 0);
module_param(qsb_tstep, int, 0);
module_param(write_descriptor_delay, int, 0);
module_param(aal5_fill_pattern, int, 0);
module_param(aal5r_max_packet_size, int, 0);
module_param(aal5r_min_packet_size, int, 0);
module_param(aal5s_max_packet_size, int, 0);
module_param(aal5s_min_packet_size, int, 0);
module_param(aal5r_drop_error_packet, int, 0);
module_param(dma_rx_descriptor_length, int, 0);
module_param(dma_tx_descriptor_length, int, 0);
module_param(dma_rx_clp1_descriptor_threshold, int, 0);

MODULE_PARM_DESC(port_cell_rate_up, "ATM port upstream rate in cells/s");
MODULE_PARM_DESC(port_max_connection, "Maximum atm connection for port (0-1)");
MODULE_PARM_DESC(qsb_tau, "Cell delay variation. Value must be > 0");
MODULE_PARM_DESC(qsb_srvm, "Maximum burst size");
MODULE_PARM_DESC(qsb_tstep, "n*32 cycles per sbs cycles n=1,2,4");
MODULE_PARM_DESC(write_descriptor_delay, "PPE core clock cycles between descriptor write and effectiveness in external RAM");
MODULE_PARM_DESC(a5_fill_pattern, "Filling pattern (PAD) for AAL5 frames");
MODULE_PARM_DESC(aal5r_max_packet_size, "Max packet size in byte for downstream AAL5 frames");
MODULE_PARM_DESC(aal5r_min_packet_size, "Min packet size in byte for downstream AAL5 frames");
MODULE_PARM_DESC(aal5s_max_packet_size, "Max packet size in byte for upstream AAL5 frames");
MODULE_PARM_DESC(aal5s_min_packet_size, "Min packet size in byte for upstream AAL5 frames");
MODULE_PARM_DESC(aal5r_drop_error_packet, "Non-zero value to drop error packet for downstream");
MODULE_PARM_DESC(dma_rx_descriptor_length, "Number of descriptor assigned to DMA RX channel (>16)");
MODULE_PARM_DESC(dma_tx_descriptor_length, "Number of descriptor assigned to DMA TX channel (>16)");
MODULE_PARM_DESC(dma_rx_clp1_descriptor_threshold, "Descriptor threshold for cells with cell loss priority 1");

void init_rx_tables(void)
{
    int i, j;
    struct wrx_queue_config wrx_queue_config = {0};
    struct wrx_dma_channel_config wrx_dma_channel_config = {0};
    struct htu_entry htu_entry = {0};
    struct htu_result htu_result = {0};

    struct htu_mask htu_mask = {    set:        0x03,
                                    pid_mask:   0x00,
                                    vpi_mask:   0x00,
                                    vci_mask:   0x00,
                                    pti_mask:   0x00,
                                    clear:      0x00};

    /*
     *  General Registers
     */
    *CFG_WRX_HTUTS  = ppe_dev.max_connections + OAM_HTU_ENTRY_NUMBER;
    *CFG_WRX_QNUM   = ppe_dev.max_connections + OAM_RX_QUEUE_NUMBER + QSB_QUEUE_NUMBER_BASE;
    *CFG_WRX_DCHNUM = ppe_dev.dma.rx_total_channel_used;
    *WRX_DMACH_ON   = (1 << ppe_dev.dma.rx_total_channel_used) - 1;
    *WRX_HUNT_BITTH = DEFAULT_RX_HUNT_BITTH;

    /*
     *  WRX Queue Configuration Table
     */
    wrx_queue_config.uumask    = 0;
    wrx_queue_config.cpimask   = 0;
    wrx_queue_config.uuexp     = 0;
    wrx_queue_config.cpiexp    = 0;
    wrx_queue_config.mfs       = ppe_dev.aal5.rx_max_packet_size;   // rx_buffer_size
    wrx_queue_config.oversize  = ppe_dev.aal5.rx_max_packet_size;
    wrx_queue_config.undersize = ppe_dev.aal5.rx_min_packet_size;
    wrx_queue_config.errdp     = ppe_dev.aal5.rx_drop_error_packet;
    for ( i = 0; i < QSB_QUEUE_NUMBER_BASE; i++ )
        *WRX_QUEUE_CONFIG(i) = wrx_queue_config;
    for ( j = 0; j < ppe_dev.max_connections; j++ )
    {
#if !defined(ENABLE_RX_QOS) || !ENABLE_RX_QOS
        /*  If RX QoS is disabled, the DMA channel must be fixed.   */
        wrx_queue_config.dmach = ppe_dev.connection[i].rx_dma_channel;
#endif  //  !defined(ENABLE_RX_QOS) || !ENABLE_RX_QOS
        *WRX_QUEUE_CONFIG(i++) = wrx_queue_config;
    }
    /*  OAM RX Queue    */
    for ( j = 0; j < OAM_RX_DMA_CHANNEL_NUMBER; j++ )
    {
#if defined(ENABLE_RX_QOS) && ENABLE_RX_QOS
        wrx_queue_config.dmach = RX_DMA_CH_OAM;
#else
        wrx_queue_config.dmach = ppe_dev.oam_rx_dma_channel + j;
#endif  //  defined(ENABLE_RX_QOS) && ENABLE_RX_QOS
        *WRX_QUEUE_CONFIG(i++) = wrx_queue_config;
    }

    wrx_dma_channel_config.deslen = ppe_dev.dma.rx_descriptor_number;
    wrx_dma_channel_config.chrl   = 0;
    wrx_dma_channel_config.clp1th = ppe_dev.dma.rx_clp1_desc_threshold;
    wrx_dma_channel_config.mode   = WRX_DMA_CHANNEL_COUNTER_MODE;
    wrx_dma_channel_config.rlcfg  = WRX_DMA_BUF_LEN_PER_DESCRIPTOR;
    for ( i = 0; i < ppe_dev.dma.rx_total_channel_used; i++ )
    {
        wrx_dma_channel_config.desba = (((u32)ppe_dev.dma.rx_descriptor_base >> 2) & 0x0FFFFFFF) + ppe_dev.dma.rx_descriptor_number * i * (sizeof(struct rx_descriptor) >> 2);
        *WRX_DMA_CHANNEL_CONFIG(i) = wrx_dma_channel_config;
    }

    /*
     *  HTU Tables
     */
    for ( i = 0; i < ppe_dev.max_connections; i++ )
    {
        htu_result.qid = (unsigned int)i;

        *HTU_ENTRY(i + OAM_HTU_ENTRY_NUMBER)  = htu_entry;
        *HTU_MASK(i + OAM_HTU_ENTRY_NUMBER)   = htu_mask;
        *HTU_RESULT(i + OAM_HTU_ENTRY_NUMBER) = htu_result;
    }
    /*  OAM HTU Entry   */
    htu_entry.vci     = 0x03;
    htu_mask.pid_mask = 0x03;
    htu_mask.vpi_mask = 0xFF;
    htu_mask.vci_mask = 0x0000;
    htu_mask.pti_mask = 0x07;
    htu_result.cellid = ppe_dev.oam_rx_queue;
    htu_result.type   = 1;
    htu_result.ven    = 1;
    htu_result.qid    = ppe_dev.oam_rx_queue;
    *HTU_RESULT(OAM_F4_SEG_HTU_ENTRY) = htu_result;
    *HTU_MASK(OAM_F4_SEG_HTU_ENTRY)   = htu_mask;
    *HTU_ENTRY(OAM_F4_SEG_HTU_ENTRY)  = htu_entry;
    htu_entry.vci     = 0x04;
    htu_result.cellid = ppe_dev.oam_rx_queue;
    htu_result.type   = 1;
    htu_result.ven    = 1;
    htu_result.qid    = ppe_dev.oam_rx_queue;
    *HTU_RESULT(OAM_F4_TOT_HTU_ENTRY) = htu_result;
    *HTU_MASK(OAM_F4_TOT_HTU_ENTRY)   = htu_mask;
    *HTU_ENTRY(OAM_F4_TOT_HTU_ENTRY)  = htu_entry;
    htu_entry.vci     = 0x00;
    htu_entry.pti     = 0x04;
    htu_mask.vci_mask = 0xFFFF;
    htu_mask.pti_mask = 0x01;
    htu_result.cellid = ppe_dev.oam_rx_queue;
    htu_result.type   = 1;
    htu_result.ven    = 1;
    htu_result.qid    = ppe_dev.oam_rx_queue;
    *HTU_RESULT(OAM_F5_HTU_ENTRY) = htu_result;
    *HTU_MASK(OAM_F5_HTU_ENTRY)   = htu_mask;
    *HTU_ENTRY(OAM_F5_HTU_ENTRY)  = htu_entry;
}

void init_tx_tables(void)
{
    int i, j;
    struct wtx_queue_config wtx_queue_config = {0};
    struct wtx_dma_channel_config wtx_dma_channel_config = {0};

    struct wtx_port_config wtx_port_config = {  res1:   0,
                                                qid:    0,
                                                qsben:  1};

    /*
     *  General Registers
     */
    *CFG_WTX_DCHNUM     = ppe_dev.dma.tx_total_channel_used + QSB_QUEUE_NUMBER_BASE;
    *WTX_DMACH_ON       = ((1 << (ppe_dev.dma.tx_total_channel_used + QSB_QUEUE_NUMBER_BASE)) - 1) ^ ((1 << QSB_QUEUE_NUMBER_BASE) - 1);
    *CFG_WRDES_DELAY    = ppe_dev.dma.write_descriptor_delay;

    /*
     *  WTX Port Configuration Table
     */
#if !defined(DISABLE_QSB) || !DISABLE_QSB
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
        *WTX_PORT_CONFIG(i) = wtx_port_config;
#else
    wtx_port_config.qsben = 0;
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
    {
        wtx_port_config.qid = ppe_dev.port[i].connection_base;
        *WTX_PORT_CONFIG(i) = wtx_port_config;

printk("port %d: qid = %d, qsb disabled\n", i, wtx_port_config.qid);
    }
#endif

    /*
     *  WTX Queue Configuration Table
     */
    wtx_queue_config.res1  = 0;
    wtx_queue_config.res2  = 0;
//    wtx_queue_config.type  = 0x03;
    wtx_queue_config.type  = 0x0;
#if !defined(DISABLE_QSB) || !DISABLE_QSB
    wtx_queue_config.qsben = 1;
#else
    wtx_queue_config.qsben = 0;
#endif
    wtx_queue_config.sbid  = 0;
    for ( i = 0; i < QSB_QUEUE_NUMBER_BASE; i++ )
        *WTX_QUEUE_CONFIG(i) = wtx_queue_config;
    for ( j = 0; j < ppe_dev.max_connections; j++ )
    {
        wtx_queue_config.sbid = ppe_dev.connection[i].port & 0x01;  /*  assign QSB to TX queue  */
        *WTX_QUEUE_CONFIG(i) = wtx_queue_config;
        i++;
    }
    /*  OAM TX Queue    */
//    wtx_queue_config.type = 0x01;
    wtx_queue_config.type  = 0x00;
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
    {
        wtx_queue_config.sbid = i & 0x01;
        for ( j = 0; j < OAM_TX_QUEUE_NUMBER_PER_PORT; j++ )
            *WTX_QUEUE_CONFIG(ppe_dev.port[i].oam_tx_queue + j) = wtx_queue_config;
    }

    wtx_dma_channel_config.mode   = WRX_DMA_CHANNEL_COUNTER_MODE;
    wtx_dma_channel_config.deslen = 0;
    wtx_dma_channel_config.desba = 0;
    for ( i = 0; i < QSB_QUEUE_NUMBER_BASE; i++ )
        *WTX_DMA_CHANNEL_CONFIG(i) = wtx_dma_channel_config;
    /*  normal connection and OAM channel   */
    wtx_dma_channel_config.deslen = ppe_dev.dma.tx_descriptor_number;
    for ( j = 0; j < ppe_dev.dma.tx_total_channel_used; j++ )
    {
        wtx_dma_channel_config.desba = (((u32)ppe_dev.dma.tx_descriptor_base >> 2) & 0x0FFFFFFF) + ppe_dev.dma.tx_descriptor_number * j * (sizeof(struct tx_descriptor) >> 2);
        *WTX_DMA_CHANNEL_CONFIG(i++) = wtx_dma_channel_config;
    }
}

static inline void qsb_global_set(void)
{
    int i, j;
    u32 qsb_clk = cgu_get_fpi_bus_clock(2);
   u32 tmp1, tmp2, tmp3;
    union qsb_queue_parameter_table qsb_queue_parameter_table = {{0}};
    union qsb_queue_vbr_parameter_table qsb_queue_vbr_parameter_table = {{0}};
    int qsb_qid;

    *QSB_ICDV = QSB_ICDV_TAU_SET(ppe_dev.qsb.tau);
    *QSB_SBL  = QSB_SBL_SBL_SET(ppe_dev.qsb.sbl);
    *QSB_CFG  = QSB_CFG_TSTEPC_SET(ppe_dev.qsb.tstepc >> 1);

    /*
     *  set SCT and SPT per port
     */
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
        if ( ppe_dev.port[i].max_connections != 0 && ppe_dev.port[i].tx_max_cell_rate != 0 )
        {
            tmp1 = ((qsb_clk * ppe_dev.qsb.tstepc) >> 1) / ppe_dev.port[i].tx_max_cell_rate;
            tmp2 = tmp1 >> 6;                   /*  integer value of Tsb    */
            tmp3 = (tmp1 & ((1 << 6) - 1)) + 1; /*  fractional part of Tsb  */
            /*  carry over to integer part (?)  */
            if ( tmp3 == (1 << 6) )
            {
                tmp3 = 0;
                tmp2++;
            }
            if ( tmp2 == 0 )
                tmp2 = tmp3 = 1;
            /*  1. set mask                                 */
            /*  2. write value to data transfer register    */
            /*  3. start the tranfer                        */
            /*  SCT (FracRate)  */
            *QSB_RTM   = QSB_RTM_DM_SET(QSB_SET_SCT_MASK);
            *QSB_RTD   = QSB_RTD_TTV_SET(tmp3);
            *QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_SCT) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(i & 0x01);
            /*  SPT (SBV + PN + IntRage)    */
            *QSB_RTM   = QSB_RTM_DM_SET(QSB_SET_SPT_MASK);
            *QSB_RTD   = QSB_RTD_TTV_SET(QSB_SPT_SBV_VALID | QSB_SPT_PN_SET(i & 0x01) | QSB_SPT_INTRATE_SET(tmp2));
            *QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_SPT) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(i & 0x01);
        }

	/*
	*  set OAM TX queue
	*/
	for ( i = 0; i < ATM_PORT_NUMBER; i++ )
		if ( ppe_dev.port[i].max_connections != 0 )
		{
			tmp1 = ((qsb_clk * ppe_dev.qsb.tstepc) >> 1) / ppe_dev.port[i].tx_max_cell_rate;
			tmp2 = tmp1 >> 6;                   /*  integer value of Tsb    */
			tmp3 = (tmp1 & ((1 << 6) - 1)) + 1; /*  fractional part of Tsb  */
			/*  carry over to integer part (?)  */
			if ( tmp3 == (1 << 6) )
			{
				tmp3 = 0;
				tmp2++;
			}
			if ( tmp2 == 0 )
				tmp2 = tmp3 = 1;
				/*  1. set mask                                 */
				/*  2. write value to data transfer register    */
				/*  3. start the tranfer                        */
				/*  SCT (FracRate)  */
				*QSB_RTM   = QSB_RTM_DM_SET(QSB_SET_SCT_MASK);
				*QSB_RTD   = QSB_RTD_TTV_SET(tmp3);
				*QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_SCT) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(i & 0x01);

				/*  SPT (SBV + PN + IntRage)    */
				*QSB_RTM   = QSB_RTM_DM_SET(QSB_SET_SPT_MASK);
				*QSB_RTD   = QSB_RTD_TTV_SET(QSB_SPT_SBV_VALID | QSB_SPT_PN_SET(i & 0x01) | QSB_SPT_INTRATE_SET(tmp2));
				*QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_SPT) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(i & 0x01);
		}

		/*
		*      *  set OAM TX queue
		*           */
		for ( i = 0; i < ATM_PORT_NUMBER; i++ )
			if ( ppe_dev.port[i].max_connections != 0 )
				for ( j = 0; j < OAM_TX_QUEUE_NUMBER_PER_PORT; j++ )
				{
					qsb_qid = ppe_dev.port[i].oam_tx_queue + j;

					/*  disable PCR limiter */
					qsb_queue_parameter_table.bit.tp = 0;
					/*  set WFQ as real time queue  */
					qsb_queue_parameter_table.bit.wfqf = 0;
					/*  disable leaky bucket shaper */
					qsb_queue_vbr_parameter_table.bit.taus = 0;
					qsb_queue_vbr_parameter_table.bit.ts = 0;

					/*  Queue Parameter Table (QPT) */
					*QSB_RTM   = QSB_RTM_DM_SET(QSB_QPT_SET_MASK);
					*QSB_RTD   = QSB_RTD_TTV_SET(qsb_queue_parameter_table.dword);
					*QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_QPT) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(qsb_qid);
					/*  Queue VBR Paramter Table (QVPT) */
					*QSB_RTM   = QSB_RTM_DM_SET(QSB_QVPT_SET_MASK);
					*QSB_RTD   = QSB_RTD_TTV_SET(qsb_queue_vbr_parameter_table.dword);
					*QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_VBR) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(qsb_qid);
				}
}

static inline void clear_ppe_dev(void)
{
	int i;

	for (i = 0; i < ppe_dev.dma.tx_total_channel_used; i++ )
	{
		int conn = i + QSB_QUEUE_NUMBER_BASE;
		int desc_base;
		struct sk_buff *skb;

		while(ppe_dev.dma.tx_desc_release_pos[conn] != ppe_dev.dma.tx_desc_alloc_pos[conn])
		{
			desc_base = ppe_dev.dma.tx_descriptor_number * (conn - QSB_QUEUE_NUMBER_BASE) + ppe_dev.dma.tx_desc_release_pos[conn];
			if(!ppe_dev.dma.tx_descriptor_base[desc_base].own)
			{
				skb = ppe_dev.dma.tx_skb_pointers[desc_base];
				atm_free_tx_skb_vcc(skb);

				//  pretend PP32 hold owner bit, so that won't be released more than once, so allocation process don't check this bit
				ppe_dev.dma.tx_descriptor_base[desc_base].own = 1;
			}
			if (++ppe_dev.dma.tx_desc_release_pos[conn] == ppe_dev.dma.tx_descriptor_number)
				ppe_dev.dma.tx_desc_release_pos[conn] = 0;
		}
	}

	for (i = ppe_dev.dma.rx_total_channel_used * ppe_dev.dma.rx_descriptor_number - 1; i >= 0; i--)
		dev_kfree_skb_any(*(struct sk_buff **)(((ppe_dev.dma.rx_descriptor_base[i].dataptr << 2) | KSEG0) - 4));

	kfree(ppe_dev.dma.tx_skb_pointers);
	kfree(ppe_dev.dma.tx_descriptor_addr);
	kfree(ppe_dev.dma.rx_descriptor_addr);
}

static inline int init_ppe_dev(void)
{
    int i, j;
    int rx_desc, tx_desc;
    int conn;
    int oam_tx_queue;
#if !defined(ENABLE_RX_QOS) || !ENABLE_RX_QOS
    int rx_dma_channel_base;
    int rx_dma_channel_assigned;
#endif  //  !defined(ENABLE_RX_QOS) || !ENABLE_RX_QOS

    struct rx_descriptor rx_descriptor = {  own:    1,
                                            c:      0,
                                            sop:    1,
                                            eop:    1,
                                            res1:   0,
                                            byteoff:0,
                                            res2:   0,
                                            id:     0,
                                            err:    0,
                                            datalen:0,
                                            res3:   0,
                                            dataptr:0};

    struct tx_descriptor tx_descriptor = {  own:    1,  //  pretend it's hold by PP32
                                            c:      0,
                                            sop:    1,
                                            eop:    1,
                                            byteoff:0,
                                            res1:   0,
                                            iscell: 0,
                                            clp:    0,
                                            datalen:0,
                                            res2:   0,
                                            dataptr:0};

    memset(&ppe_dev, 0, sizeof(ppe_dev));

    /*
     *  Setup AAL5 members, buffer size must be larger than max packet size plus overhead.
     */
    ppe_dev.aal5.padding_byte         = (u8)aal5_fill_pattern;
    ppe_dev.aal5.rx_max_packet_size   = (u32)aal5r_max_packet_size;
    ppe_dev.aal5.rx_min_packet_size   = (u32)aal5r_min_packet_size;
    ppe_dev.aal5.rx_buffer_size       = ((u32)(aal5r_max_packet_size > CELL_SIZE ? aal5r_max_packet_size + MAX_RX_FRAME_EXTRA_BYTES : CELL_SIZE + MAX_RX_FRAME_EXTRA_BYTES) + DMA_ALIGNMENT - 1) & ~(DMA_ALIGNMENT - 1);
    ppe_dev.aal5.tx_max_packet_size   = (u32)aal5s_max_packet_size;
    ppe_dev.aal5.tx_min_packet_size   = (u32)aal5s_min_packet_size;
    ppe_dev.aal5.tx_buffer_size       = ((u32)(aal5s_max_packet_size > CELL_SIZE ? aal5s_max_packet_size + MAX_TX_FRAME_EXTRA_BYTES : CELL_SIZE + MAX_TX_FRAME_EXTRA_BYTES) + DMA_ALIGNMENT - 1) & ~(DMA_ALIGNMENT - 1);
    ppe_dev.aal5.rx_drop_error_packet = aal5r_drop_error_packet ? 1 : 0;

    /*
     *  Setup QSB members, please refer to Amazon spec 15.4 to get the value calculation formula.
     */
    ppe_dev.qsb.tau     = (u32)qsb_tau;
    ppe_dev.qsb.tstepc  = (u32)qsb_tstep;
    ppe_dev.qsb.sbl     = (u32)qsb_srvm;

    /*
     *  Setup port, connection, other members.
     */
    conn = 0;
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
    {
        /*  first connection ID of port */
        ppe_dev.port[i].connection_base  = conn + QSB_QUEUE_NUMBER_BASE;
        /*  max number of connections of port   */
        ppe_dev.port[i].max_connections  = (u32)port_max_connection[i];
        /*  max cell rate the port has  */
        ppe_dev.port[i].tx_max_cell_rate = (u32)port_cell_rate_up[i];

        /*  link connection ID to port ID   */
        for ( j = port_max_connection[i] - 1; j >= 0; j-- )
            ppe_dev.connection[conn++ + QSB_QUEUE_NUMBER_BASE].port = i;
    }
    /*  total connection numbers of all ports   */
    ppe_dev.max_connections = conn;
    /*  OAM RX queue ID, which is the first available connection ID after */
    /*  connections assigned to ports.                                    */
    ppe_dev.oam_rx_queue = conn + QSB_QUEUE_NUMBER_BASE;

#if defined(ENABLE_RX_QOS) && ENABLE_RX_QOS
    oam_tx_queue = conn;
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
        if ( port_max_connection[i] != 0 )
        {
            ppe_dev.port[i].oam_tx_queue = oam_tx_queue + QSB_QUEUE_NUMBER_BASE;

            for ( j = 0; j < OAM_TX_QUEUE_NUMBER_PER_PORT; j++ )
                /*  Since connection ID is one to one mapped to RX/TX queue ID, the connection  */
                /*  structure must be reserved for OAM RX/TX queues, and member "port" is set   */
                /*  according to port to which OAM TX queue is connected.                       */
                ppe_dev.connection[oam_tx_queue++ + QSB_QUEUE_NUMBER_BASE].port = i;
        }
    /*  DMA RX channel assigned to OAM RX queue */
    ppe_dev.oam_rx_dma_channel = RX_DMA_CH_OAM;
    /*  DMA RX channel will be assigned dynamically when VCC is open.   */
#else   //  defined(ENABLE_RX_QOS) && ENABLE_RX_QOS
    rx_dma_channel_base = 0;
    oam_tx_queue = conn;
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
        if ( port_max_connection[i] != 0 )
        {
            /*  Calculate the number of DMA RX channels could be assigned to port.  */
            rx_dma_channel_assigned = i == ATM_PORT_NUMBER - 1
                                      ? (MAX_RX_DMA_CHANNEL_NUMBER - OAM_RX_DMA_CHANNEL_NUMBER) - rx_dma_channel_base
                                      : (ppe_dev.port[i].max_connections * (MAX_RX_DMA_CHANNEL_NUMBER - OAM_RX_DMA_CHANNEL_NUMBER) + ppe_dev.max_connections / 2) / ppe_dev.max_connections;
            /*  Amend the number, which could be zero.  */
            if ( rx_dma_channel_assigned == 0 )
                rx_dma_channel_assigned = 1;
            /*  Calculate the first DMA RX channel ID could be assigned to port.    */
            if ( rx_dma_channel_base + rx_dma_channel_assigned > MAX_RX_DMA_CHANNEL_NUMBER - OAM_RX_DMA_CHANNEL_NUMBER )
                rx_dma_channel_base = MAX_RX_DMA_CHANNEL_NUMBER - OAM_RX_DMA_CHANNEL_NUMBER - rx_dma_channel_assigned;

            /*  first DMA RX channel ID */
            ppe_dev.port[i].rx_dma_channel_base     = rx_dma_channel_base;
            /*  number of DMA RX channels assigned to this port */
            ppe_dev.port[i].rx_dma_channel_assigned = rx_dma_channel_assigned;
            /*  OAM TX queue ID, which must be assigned after connections assigned to ports */
            ppe_dev.port[i].oam_tx_queue            = oam_tx_queue + QSB_QUEUE_NUMBER_BASE;

            rx_dma_channel_base += rx_dma_channel_assigned;

            for ( j = 0; j < OAM_TX_QUEUE_NUMBER_PER_PORT; j++ )
                /*  Since connection ID is one to one mapped to RX/TX queue ID, the connection  */
                /*  structure must be reserved for OAM RX/TX queues, and member "port" is set   */
                /*  according to port to which OAM TX queue is connected.                       */
                ppe_dev.connection[oam_tx_queue++ + QSB_QUEUE_NUMBER_BASE].port = i;
        }
    /*  DMA RX channel assigned to OAM RX queue */
    ppe_dev.oam_rx_dma_channel = rx_dma_channel_base;

    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
       for ( j = 0; j < port_max_connection[i]; j++ )
            /*  Assign DMA RX channel to RX queues. One channel could be assigned to more than one queue.   */
            ppe_dev.connection[ppe_dev.port[i].connection_base + j].rx_dma_channel = ppe_dev.port[i].rx_dma_channel_base + j % ppe_dev.port[i].rx_dma_channel_assigned;
#endif  //  defined(ENABLE_RX_QOS) && ENABLE_RX_QOS

    /*  initialize semaphore used by open and close */
    sema_init(&ppe_dev.sem, 1);
    /*  descriptor number of RX DMA channel */
    ppe_dev.dma.rx_descriptor_number         = dma_rx_descriptor_length;
    /*  descriptor number of TX DMA channel */
    ppe_dev.dma.tx_descriptor_number         = dma_tx_descriptor_length;
    /*  If used descriptors are more than this value, cell with CLP1 is dropped.    */
    ppe_dev.dma.rx_clp1_desc_threshold = dma_rx_clp1_descriptor_threshold;

    /*  delay on descriptor write path  */
    ppe_dev.dma.write_descriptor_delay       = write_descriptor_delay;

    /*  total DMA RX channel used   */
#if defined(ENABLE_RX_QOS) && ENABLE_RX_QOS
    ppe_dev.dma.rx_total_channel_used = RX_DMA_CH_TOTAL;
#else
    ppe_dev.dma.rx_total_channel_used = rx_dma_channel_base + OAM_RX_DMA_CHANNEL_NUMBER;
#endif  //  defined(ENABLE_RX_QOS) && ENABLE_RX_QOS
    /*  total DMA TX channel used (exclude channel reserved by QSB) */
    ppe_dev.dma.tx_total_channel_used = oam_tx_queue;

    /*  allocate memory for RX descriptors  */
    ppe_dev.dma.rx_descriptor_addr = kmalloc(ppe_dev.dma.rx_total_channel_used * ppe_dev.dma.rx_descriptor_number * sizeof(struct rx_descriptor) + 4, GFP_KERNEL | GFP_DMA);
    if ( !ppe_dev.dma.rx_descriptor_addr )
        goto RX_DESCRIPTOR_BASE_ALLOCATE_FAIL;
    /*  do alignment (DWORD)    */
    ppe_dev.dma.rx_descriptor_base = (struct rx_descriptor *)(((u32)ppe_dev.dma.rx_descriptor_addr + 0x03) & ~0x03);
    ppe_dev.dma.rx_descriptor_base = (struct rx_descriptor *)((u32)ppe_dev.dma.rx_descriptor_base | KSEG1);    //  no cache

    /*  allocate memory for TX descriptors  */
    ppe_dev.dma.tx_descriptor_addr = kmalloc(ppe_dev.dma.tx_total_channel_used * ppe_dev.dma.tx_descriptor_number * sizeof(struct tx_descriptor) + 4, GFP_KERNEL | GFP_DMA);
    if ( !ppe_dev.dma.tx_descriptor_addr )
        goto TX_DESCRIPTOR_BASE_ALLOCATE_FAIL;
    /*  do alignment (DWORD)    */
    ppe_dev.dma.tx_descriptor_base = (struct tx_descriptor *)(((u32)ppe_dev.dma.tx_descriptor_addr + 0x03) & ~0x03);
    ppe_dev.dma.tx_descriptor_base = (struct tx_descriptor *)((u32)ppe_dev.dma.tx_descriptor_base | KSEG1);    //  no cache
    /*  allocate pointers to TX sk_buff */
    ppe_dev.dma.tx_skb_pointers = kmalloc(ppe_dev.dma.tx_total_channel_used * ppe_dev.dma.tx_descriptor_number * sizeof(struct sk_buff *), GFP_KERNEL);
    if ( !ppe_dev.dma.tx_skb_pointers )
        goto TX_SKB_POINTER_ALLOCATE_FAIL;
    memset(ppe_dev.dma.tx_skb_pointers, 0, ppe_dev.dma.tx_total_channel_used * ppe_dev.dma.tx_descriptor_number * sizeof(struct sk_buff *));

    /*  Allocate RX sk_buff and fill up RX descriptors. */
    rx_descriptor.datalen = ppe_dev.aal5.rx_buffer_size;
    for ( rx_desc = ppe_dev.dma.rx_total_channel_used * ppe_dev.dma.rx_descriptor_number - 1; rx_desc >= 0; rx_desc-- )
    {
        struct sk_buff *skb;
        skb = alloc_skb_rx();
        if ( skb == NULL )
            panic("sk buffer is used up\n");
        rx_descriptor.dataptr = (u32)skb->data >> 2;
        ppe_dev.dma.rx_descriptor_base[rx_desc] = rx_descriptor;

    }

    /*  Fill up TX descriptors. */
    tx_descriptor.datalen = ppe_dev.aal5.tx_buffer_size;
    for ( tx_desc = ppe_dev.dma.tx_total_channel_used * ppe_dev.dma.tx_descriptor_number - 1; tx_desc >= 0; tx_desc-- )
        ppe_dev.dma.tx_descriptor_base[tx_desc] = tx_descriptor;

    return 0;

TX_SKB_POINTER_ALLOCATE_FAIL:
    kfree(ppe_dev.dma.tx_descriptor_addr);
TX_DESCRIPTOR_BASE_ALLOCATE_FAIL:
    kfree(ppe_dev.dma.rx_descriptor_addr);
RX_DESCRIPTOR_BASE_ALLOCATE_FAIL:
    return -ENOMEM;
}


static inline void clear_share_buffer(void)
{
    volatile u32 *p = SB_RAM0_ADDR(0);
    unsigned int i;

    /*  write all zeros only    */
    for ( i = 0; i < SB_RAM0_DWLEN + SB_RAM1_DWLEN + SB_RAM2_DWLEN + SB_RAM3_DWLEN; i++ )
        *p++ = 0;
}


static inline void check_parameters(void)
{
    int i;
    int enabled_port_number;
    int unassigned_queue_number;
    int assigned_queue_number;

    enabled_port_number = 0;
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
        if ( port_max_connection[i] < 1 )
            port_max_connection[i] = 0;
        else
            enabled_port_number++;
    /*  If the max connection number of a port is not 0, the port is enabled  */
    /*  and at lease two connection ID must be reserved for this port. One of */
    /*  them is used as OAM TX path.                                          */
    unassigned_queue_number = MAX_QUEUE_NUMBER - QSB_QUEUE_NUMBER_BASE;
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
        if ( port_max_connection[i] > 0 )
        {
            enabled_port_number--;
            assigned_queue_number = unassigned_queue_number - enabled_port_number * (1 + OAM_TX_QUEUE_NUMBER_PER_PORT) - OAM_TX_QUEUE_NUMBER_PER_PORT;
            if ( assigned_queue_number > MAX_QUEUE_NUMBER_PER_PORT - OAM_TX_QUEUE_NUMBER_PER_PORT )
                assigned_queue_number = MAX_QUEUE_NUMBER_PER_PORT - OAM_TX_QUEUE_NUMBER_PER_PORT;
            if ( port_max_connection[i] > assigned_queue_number )
            {
                port_max_connection[i] = assigned_queue_number;
                unassigned_queue_number -= assigned_queue_number;
            }
            else
                unassigned_queue_number -= port_max_connection[i];
        }

    /*  Please refer to Amazon spec 15.4 for setting these values.  */
    if ( qsb_tau < 1 )
        qsb_tau = 1;
    if ( qsb_tstep < 1 )
        qsb_tstep = 1;
    else if ( qsb_tstep > 4 )
        qsb_tstep = 4;
    else if ( qsb_tstep == 3 )
        qsb_tstep = 2;

    /*  There is a delay between PPE write descriptor and descriptor is       */
    /*  really stored in memory. Host also has this delay when writing        */
    /*  descriptor. So PPE will use this value to determine if the write      */
    /*  operation makes effect.                                               */
    if ( write_descriptor_delay < 0 )
        write_descriptor_delay = 0;

    if ( aal5_fill_pattern < 0 )
        aal5_fill_pattern = 0;
    else
        aal5_fill_pattern &= 0xFF;

    /*  Because of the limitation of length field in descriptors, the packet  */
    /*  size could not be larger than 64K minus overhead size.                */
    if ( aal5r_max_packet_size < 0 )
        aal5r_max_packet_size = 0;
    else if ( aal5r_max_packet_size >= 65536 - MAX_RX_FRAME_EXTRA_BYTES )
        aal5r_max_packet_size = 65536 - MAX_RX_FRAME_EXTRA_BYTES;
    if ( aal5r_min_packet_size < 0 )
        aal5r_min_packet_size = 0;
    else if ( aal5r_min_packet_size > aal5r_max_packet_size )
        aal5r_min_packet_size = aal5r_max_packet_size;
    if ( aal5s_max_packet_size < 0 )
        aal5s_max_packet_size = 0;
    else if ( aal5s_max_packet_size >= 65536 - MAX_TX_FRAME_EXTRA_BYTES )
        aal5s_max_packet_size = 65536 - MAX_TX_FRAME_EXTRA_BYTES;
    if ( aal5s_min_packet_size < 0 )
        aal5s_min_packet_size = 0;
    else if ( aal5s_min_packet_size > aal5s_max_packet_size )
        aal5s_min_packet_size = aal5s_max_packet_size;

    if ( dma_rx_descriptor_length < 2 )
        dma_rx_descriptor_length = 2;
    if ( dma_tx_descriptor_length < 2 )
        dma_tx_descriptor_length = 2;
    if ( dma_rx_clp1_descriptor_threshold < 0 )
        dma_rx_clp1_descriptor_threshold = 0;
    else if ( dma_rx_clp1_descriptor_threshold > dma_rx_descriptor_length )
        dma_rx_clp1_descriptor_threshold = dma_rx_descriptor_length;
}

static struct atmdev_ops ppe_atm_ops = {
    owner:      THIS_MODULE,
    open:       ppe_open,
    close:      ppe_close,
    ioctl:      ppe_ioctl,
    send:       ppe_send,
    send_oam:   ppe_send_oam,
    change_qos: ppe_change_qos,
};

int __init danube_ppe_init(void)
{
    int ret;
    int port_num;

    check_parameters();

    ret = init_ppe_dev();
    if ( ret )
        goto INIT_PPE_DEV_FAIL;

    clear_share_buffer();
    init_rx_tables();
    init_tx_tables();
printk("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

    for ( port_num = 0; port_num < ATM_PORT_NUMBER; port_num++ )
        if ( ppe_dev.port[port_num].max_connections != 0 )
        {
            printk("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
			ppe_dev.port[port_num].dev = atm_dev_register("danube_atm", &ppe_atm_ops, -1, 0UL);
            if ( !ppe_dev.port[port_num].dev )
            {
                printk("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
				ret = -EIO;
                goto ATM_DEV_REGISTER_FAIL;
            }
            else
            {
                printk("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
				ppe_dev.port[port_num].dev->ci_range.vpi_bits = 8;
                ppe_dev.port[port_num].dev->ci_range.vci_bits = 16;
                ppe_dev.port[port_num].dev->link_rate = ppe_dev.port[port_num].tx_max_cell_rate;
                ppe_dev.port[port_num].dev->dev_data = (void*)port_num;
            }
        }
    /*  register interrupt handler  */
    ret = request_irq(IFXMIPS_PPE_MBOX_INT, mailbox_irq_handler, IRQF_DISABLED, "ppe_mailbox_isr", NULL);
    if ( ret )
    {
        if ( ret == -EBUSY )
            printk("ppe: IRQ may be occupied by ETH2 driver, please reconfig to disable it.\n");
        goto REQUEST_IRQ_IFXMIPS_PPE_MBOX_INT_FAIL;
    }
    disable_irq(IFXMIPS_PPE_MBOX_INT);

  #if defined(CONFIG_PCI) && defined(USE_FIX_FOR_PCI_PPE) && USE_FIX_FOR_PCI_PPE
    ret = request_irq(PPE_MAILBOX_IGU0_INT, pci_fix_irq_handler, SA_INTERRUPT, "ppe_pci_fix_isr", NULL);
    if ( ret )
        printk("failed in registering mailbox 0 interrupt (pci fix)\n");
  #endif  //  defined(CONFIG_PCI) && defined(USE_FIX_FOR_PCI_PPE) && USE_FIX_FOR_PCI_PPE

    ret = pp32_start();
    if ( ret )
        goto PP32_START_FAIL;

    qsb_global_set();
	HTU_ENTRY(OAM_F4_SEG_HTU_ENTRY)->vld = 1;
    HTU_ENTRY(OAM_F4_TOT_HTU_ENTRY)->vld = 1;
    HTU_ENTRY(OAM_F5_HTU_ENTRY)->vld = 1;

    /*  create proc file    */
    proc_file_create();

    printk("ppe: ATM init succeeded (firmware version 1.1.0.2.1.13\n");
    return 0;

PP32_START_FAIL:

    free_irq(IFXMIPS_PPE_MBOX_INT, NULL);
REQUEST_IRQ_IFXMIPS_PPE_MBOX_INT_FAIL:
ATM_DEV_REGISTER_FAIL:
    clear_ppe_dev();
INIT_PPE_DEV_FAIL:
    printk("ppe: ATM init failed\n");
    return ret;
}

void __exit danube_ppe_exit(void)
{
    int port_num;
	register int l;
	proc_file_delete();
    HTU_ENTRY(OAM_F4_SEG_HTU_ENTRY)->vld = 0;
    HTU_ENTRY(OAM_F4_TOT_HTU_ENTRY)->vld = 0;
    HTU_ENTRY(OAM_F5_HTU_ENTRY)->vld = 0;
    /*  idle for a while to finish running HTU search   */
    for (l = 0; l < IDLE_CYCLE_NUMBER; l++ );
    pp32_stop();
    free_irq(IFXMIPS_PPE_MBOX_INT, NULL);
    for ( port_num = 0; port_num < ATM_PORT_NUMBER; port_num++ )
        if ( ppe_dev.port[port_num].max_connections != 0 )
            atm_dev_deregister(ppe_dev.port[port_num].dev);
    clear_ppe_dev();
}

module_init(danube_ppe_init);
module_exit(danube_ppe_exit);

MODULE_LICENSE("GPL");

