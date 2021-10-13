/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __SYN_REG_H__
#define __SYN_REG_H__

/*
 * MAC register offset
 */
#define SYN_MAC_CONFIGURATION				0x0000
#define SYN_MAC_FRAME_FILTER				0x0004
#define SYN_MAC_FLOW_CONTROL				0x0018
#define SYN_VLAN_TAG					0x001C
#define SYN_VERSION					0x0020
#define SYN_DEBUG					0x0024
#define SYN_REMOTE_WAKE_UP_FRAME_FILTER			0x0028
#define SYN_PMT_CONTROL_STATUS				0x002C
#define SYN_LPI_CONTROL_STATUS				0x0030
#define SYN_LPI_TIMERS_CONTROL				0x0034
#define SYN_INTERRUPT_STATUS				0x0038
#define SYN_INTERRUPT_MASK				0x003C

/*
 * MAC address register offset
 */
#define SYN_MAC_ADDR0_HIGH				0x0040
#define SYN_MAC_ADDR0_LOW				0x0044
#define SYN_MAC_ADDR1_HIGH				0x0048
#define SYN_MAC_ADDR1_LOW				0x004C
#define SYN_MAC_ADDR2_HIGH				0x0050
#define SYN_MAC_ADDR2_LOW				0x0054
#define SYN_MAC_ADDR3_HIGH				0x0058
#define SYN_MAC_ADDR3_LOW				0x005C
#define SYN_MAC_ADDR4_HIGH				0x0060
#define SYN_MAC_ADDR4_LOW				0x0064

/*
 * Watchdog timeout register
 */
#define SYN_WDOG_TIMEOUT				0x00DC

/*
 * Mac Management Counters (MMC) register offset
 */
#define SYN_MMC_CONTROL					0x0100
#define SYN_MMC_RX_INTERRUPT				0x0104
#define SYN_MMC_TX_INTERRUPT				0x0108
#define SYN_MMC_RX_INTERRUPT_MASK			0x010C
#define SYN_MMC_TX_INTERRUPT_MASK			0x0110
#define SYN_MMC_IPC_RX_INTR_MASK			0x0200

/*
 * DMA Register offset
 */
#define SYN_DMA_BUS_MODE				0x1000
#define SYN_DMA_TX_POLL_DEMAND				0x1004
#define SYN_DMA_RX_POLL_DEMAND				0x1008
#define SYN_DMA_RX_DESCRIPTOR_LIST_ADDRESS		0x100C
#define SYN_DMA_TX_DESCRIPTOR_LIST_ADDRESS		0x1010
#define SYN_DMA_STATUS					0x1014
#define SYN_DMA_OPERATION_MODE				0x1018
#define SYN_DMA_INT_ENABLE				0x101C
#define SYN_DMA_MISSED_FRAME_AND_BUFF_OVERFLOW_COUNTER	0x1020
#define SYN_DMA_RX_INTERRUPT_WATCHDOG_TIMER		0x1024
#define SYN_DMA_AXI_BUS_MODE				0x1028
#define SYN_DMA_AHB_OR_AXI_STATUS			0x102C
#define SYN_DMA_CURRENT_HOST_TX_DESCRIPTOR		0x1048
#define SYN_DMA_CURRENT_HOST_RX_DESCRIPTOR		0x104C
#define SYN_DMA_CURRENT_HOST_TX_BUFFER_ADDRESS		0x1050
#define SYN_DMA_CURRENT_HOST_RX_BUFFER_ADDRESS		0x1054

/*
 * Optional HW feature register
 */
#define SYN_HW_FEATURE					0x1058

/*
 * Register Bit Definitions
 */

/*
 * SYN_MAC_CONFIGURATION = 0x0000,	MAC config Register Layout
 */
enum syn_mac_config_reg {
	SYN_MAC_TWOKPE = 0x08000000,			/* Support for 2K packets */
	SYN_MAC_TWOKPE_ENABLE = 0x08000000,
	SYN_MAC_TWOKPE_DISABLE = 0x00000000,
	SYN_MAC_CST = 0x02000000,			/* (CST) CRC Stripping for Type Frames */
	SYN_MAC_CST_ENABLE = 0x02000000,
	SYN_MAC_CST_DISABLE = 0x02000000,
	SYN_MAC_TC = 0x01000000,			/* (TC) Transmit configuration */
	SYN_MAC_WATCHDOG = 0x00800000,
	SYN_MAC_WATCHDOG_ENABLE = 0x00000000,		/* Enable watchdog timer */
	SYN_MAC_WATCHDOG_DISABLE = 0x00800000,		/* (WD)Disable watchdog timer on Rx */
	SYN_MAC_JABBER = 0x00400000,
	SYN_MAC_JABBER_ENABLE = 0x00000000,		/* Enable jabber timer */
	SYN_MAC_JABBER_DISABLE = 0x00400000,		/* (JD)Disable jabber timer on Tx */
	SYN_MAC_FRAME_BURST = 0x00200000,
	SYN_MAC_FRAME_BURST_ENABLE = 0x00200000,	/* (BE)Enable frame bursting
							   during Tx */
	SYN_MAC_FRAME_BURST_DISABLE = 0x00000000,	/* Disable frame bursting */
	SYN_MAC_JUMBO_FRAME = 0x00100000,
	SYN_MAC_JUMBO_FRAME_ENABLE = 0x00100000,	/* (JE)Enable jumbo frame for Rx */
	SYN_MAC_JUMBO_FRAME_DISABLE = 0x00000000,	/* Disable jumbo frame */
	SYN_MAC_INTER_FRAME_GAP7 = 0x000E0000,		/* (IFG) Config7 - 40bit times */
	SYN_MAC_INTER_FRAME_GAP6 = 0x000C0000,		/* (IFG) Config6 - 48bit times */
	SYN_MAC_INTER_FRAME_GAP5 = 0x000A0000,		/* (IFG) Config5 - 56bit times */
	SYN_MAC_INTER_FRAME_GAP4 = 0x00080000,		/* (IFG) Config4 - 64bit times */
	SYN_MAC_INTER_FRAME_GAP3 = 0x00060000,		/* (IFG) Config3 - 72bit times */
	SYN_MAC_INTER_FRAME_GAP2 = 0x00040000,		/* (IFG) Config2 - 80bit times */
	SYN_MAC_INTER_FRAME_GAP1 = 0x00020000,		/* (IFG) Config1 - 88bit times */
	SYN_MAC_INTER_FRAME_GAP0 = 0x00000000,		/* (IFG) Config0 - 96bit times */
	SYN_MAC_DISABLE_CRS = 0x00010000,		/* (DCRS) Disable Carrier Sense During Transmission */
	SYN_MAC_MII_GMII = 0x00008000,
	SYN_MAC_SELECT_MII = 0x00008000,		/* (PS)Port Select-MII mode   */
	SYN_MAC_SELECT_GMII = 0x00000000,		/* GMII mode                  */
	SYN_MAC_FE_SPEED100 = 0x00004000,		/* (FES)Fast Ethernet speed 100Mbps */
	SYN_MAC_FE_SPEED = 0x00004000,			/* (FES)Fast Ethernet speed 100Mbps */
	SYN_MAC_FE_SPEED10 = 0x00000000,		/* (FES)Fast Ethernet speed 10Mbps */
	SYN_MAC_RX_OWN = 0x00002000,
	SYN_MAC_DISABLE_RX_OWN = 0x00002000,		/* (DO)Disable receive own packets */
	SYN_MAC_ENABLE_RX_OWN = 0x00000000,		/* Enable receive own packets */
	SYN_MAC_LOOPBACK = 0x00001000,
	SYN_MAC_LOOPBACK_ON = 0x00001000,		/* (LM)Loopback mode for GMII/MII */
	SYN_MAC_LOOPBACK_OFF = 0x00000000,		/* Normal mode                */
	SYN_MAC_DUPLEX = 0x00000800,
	SYN_MAC_FULL_DUPLEX = 0x00000800,		/* (DM)Full duplex mode       */
	SYN_MAC_HALF_DUPLEX = 0x00000000,		/* Half duplex mode           */
	SYN_MAC_RX_IPC_OFFLOAD = 0x00000400,		/* IPC checksum offload       */
	SYN_MAC_RX_IPC_OFFLOAD_ENABLE = 0x00000400,
	SYN_MAC_RX_IPC_OFFLOAD_DISABLE = 0x00000000,
	SYN_MAC_RETRY = 0x00000200,
	SYN_MAC_RETRY_DISABLE = 0x00000200,		/* (DR)Disable Retry          */
	SYN_MAC_RETRY_ENABLE = 0x00000000,		/* Enable retransmission as per BL */
	SYN_MAC_LINK_UP = 0x00000100,			/* (LUD)Link UP               */
	SYN_MAC_LINK_DOWN = 0x00000100,			/* Link Down                  */
	SYN_MAC_PAD_CRC_STRIP = 0x00000080,
	SYN_MAC_PAD_CRC_STRIP_ENABLE = 0x00000080,	/* (ACS) Automatic Pad/Crc strip enable */
	SYN_MAC_PAD_CRC_STRIP_DISABLE = 0x00000000,	/* Automatic Pad/Crc stripping disable */
	SYN_MAC_BACKOFF_LIMIT = 0x00000060,
	SYN_MAC_BACKOFF_LIMIT3 = 0x00000060,		/* (BL)Back-off limit in HD mode */
	SYN_MAC_BACKOFF_LIMIT2 = 0x00000040,
	SYN_MAC_BACKOFF_LIMIT1 = 0x00000020,
	SYN_MAC_BACKOFF_LIMIT0 = 0x00000000,
	SYN_MAC_DEFERRAL_CHECK = 0x00000010,
	SYN_MAC_DEFERRAL_CHECK_ENABLE = 0x00000010,	/* (DC)Deferral check enable in HD mode */
	SYN_MAC_DEFERRAL_CHECK_DISABLE = 0x00000000,	/* Deferral check disable    */
	SYN_MAC_TX = 0x00000008,
	SYN_MAC_TX_ENABLE = 0x00000008,			/* (TE)Transmitter enable */
	SYN_MAC_TX_DISABLE = 0x00000000,		/* Transmitter disable */
	SYN_MAC_RX = 0x00000004,
	SYN_MAC_RX_ENABLE = 0x00000004,			/* (RE)Receiver enable */
	SYN_MAC_RX_DISABLE = 0x00000000,		/* Receiver disable */
	SYN_MAC_PRELEN_RESERVED = 0x00000003,		/* Preamble Length for Transmit Frames */
	SYN_MAC_PRELEN_3B = 0x00000002,
	SYN_MAC_PRELEN_5B = 0x00000001,
	SYN_MAC_PRELEN_7B = 0x00000000,
};

/*
 * SYN_MAC_FRAME_FILTER = 0x0004,	Mac frame filtering controls Register
 */
enum syn_mac_frame_filter_reg {
	SYN_MAC_FILTER = 0x80000000,
	SYN_MAC_FILTER_OFF = 0x80000000,		/* (RA)Receive all incoming packets */
	SYN_MAC_FILTER_ON = 0x00000000,			/* Receive filtered pkts only */
	SYN_MAC_HASH_PERFECT_FILTER = 0x00000400,	/* Hash or Perfect Filter enable */
	SYN_MAC_SRC_ADDR_FILTER = 0x00000200,
	SYN_MAC_SRC_ADDR_FILTER_ENABLE = 0x00000200,	/* (SAF)Source Address Filter enable */
	SYN_MAC_SRC_ADDR_FILTER_DISABLE = 0x00000000,
	SYN_MAC_SRC_INVA_ADDR_FILTER = 0x00000100,
	SYN_MAC_SRC_INV_ADDR_FILTER_EN = 0x00000100,	/* (SAIF)Inv Src Addr Filter enable */
	SYN_MAC_SRC_INV_ADDR_FILTER_DIS = 0x00000000,
	SYN_MAC_PASS_CONTROL = 0x000000C0,
	SYN_MAC_PASS_CONTROL3 = 0x000000C0,		/* (PCF)Forwards ctrl frames that pass AF */
	SYN_MAC_PASS_CONTROL2 = 0x00000080,		/* Forwards all control frames
							   even if they fail the AF */
	SYN_MAC_PASS_CONTROL1 = 0x00000040,		/* Forwards all control frames except
							   PAUSE control frames to application
							   even if they fail the AF */
	SYN_MAC_PASS_CONTROL0 = 0x00000000,		/* Don't pass control frames */
	SYN_MAC_BROADCAST = 0x00000020,
	SYN_MAC_BROADCAST_DISABLE = 0x00000020,		/* (DBF)Disable Rx of broadcast frames */
	SYN_MAC_BROADCAST_ENABLE = 0x00000000,		/* Enable broadcast frames    */
	SYN_MAC_MULTICAST_FILTER = 0x00000010,
	SYN_MAC_MULTICAST_FILTER_OFF = 0x00000010,	/* (PM) Pass all multicast packets */
	SYN_MAC_MULTICAST_FILTER_ON = 0x00000000,	/* Pass filtered multicast packets */
	SYN_MAC_DEST_ADDR_FILTER = 0x00000008,
	SYN_MAC_DEST_ADDR_FILTER_INV = 0x00000008,	/* (DAIF)Inverse filtering for DA */
	SYN_MAC_DEST_ADDR_FILTER_NOR = 0x00000000,	/* Normal filtering for DA */
	SYN_MAC_MCAST_HASH_FILTER = 0x00000004,
	SYN_MAC_MCAST_HASH_FILTER_ON = 0x00000004,	/* (HMC)perfom multicast hash filtering */
	SYN_MAC_MCAST_HASH_FILTER_OFF = 0x00000000,	/* perfect filtering only     */
	SYN_MAC_UCAST_HASH_FILTER = 0x00000002,
	SYN_MAC_UCAST_HASH_FILTER_ON = 0x00000002,	/* (HUC)Unicast Hash filtering only */
	SYN_MAC_UCAST_HASH_FILTER_OFF = 0x00000000,	/* perfect filtering only     */
	SYN_MAC_PROMISCUOUS_MODE = 0x00000001,
	SYN_MAC_PROMISCUOUS_MODE_ON = 0x00000001,	/* Receive all frames         */
	SYN_MAC_PROMISCUOUS_MODE_OFF = 0x00000000,	/* Receive filtered packets only */
};

/*
 * SYN_MAC_FLOW_CONTROL = 0x0018,	Flow control Register Layout
 */
enum syn_mac_flow_control_reg {
	SYN_MAC_FC_PAUSE_TIME_MASK = 0xFFFF0000,	/* (PT) PAUSE TIME field
							   in the control frame */
	SYN_MAC_FC_PAUSE_TIME_SHIFT = 16,
	SYN_MAC_FC_PAUSE_LOW_THRESH = 0x00000030,
	SYN_MAC_FC_PAUSE_LOW_THRESH3 = 0x00000030,	/* (PLT)thresh for pause
							   tmr 256 slot time */
	SYN_MAC_FC_PAUSE_LOW_THRESH2 = 0x00000020,	/* 144 slot time         */
	SYN_MAC_FC_PAUSE_LOW_THRESH1 = 0x00000010,	/* 28 slot time          */
	SYN_MAC_FC_PAUSE_LOW_THRESH0 = 0x00000000,	/* 4 slot time           */
	SYN_MAC_FC_UNICAST_PAUSE_FRAME = 0x00000008,
	SYN_MAC_FC_UNICAST_PAUSE_FRAME_ON = 0x00000008,	/* (UP)Detect pause frame
							   with unicast addr. */
	SYN_MAC_FC_UNICAST_PAUSE_FRAME_OFF = 0x00000000,/* Detect only pause frame
							   with multicast addr. */
	SYN_MAC_FC_RX_FLOW_CONTROL = 0x00000004,
	SYN_MAC_FC_RX_FLOW_CONTROL_ENABLE = 0x00000004,	/* (RFE)Enable Rx flow control */
	SYN_MAC_FC_RX_FLOW_CONTROL_DISABLE = 0x00000000,/* Disable Rx flow control */
	SYN_MAC_FC_TX_FLOW_CONTROL = 0x00000002,
	SYN_MAC_FC_TX_FLOW_CONTROL_ENABLE = 0x00000002,	/* (TFE)Enable Tx flow control */
	SYN_MAC_FC_TX_FLOW_CONTROL_DISABLE = 0x00000000,/* Disable flow control */
	SYN_MAC_FC_FLOW_CONTROL_BACK_PRESSURE = 0x00000001,
	SYN_MAC_FC_SEND_PAUSE_FRAME = 0x00000001,	/* (FCB/PBA)send pause frm/Apply
							   back pressure */
};

/*
 * SYN_MAC_ADDR_HIGH Register
 */
enum syn_mac_addr_high {
	SYN_MAC_ADDR_HIGH_AE = 0x80000000,
};

/*
 * SYN_DMA_BUS_MODE = 0x0000,		CSR0 - Bus Mode
 */
enum syn_dma_bus_mode_reg {
	SYN_DMA_FIXED_BURST_ENABLE = 0x00010000,	/* (FB)Fixed Burst SINGLE, INCR4,
							   INCR8 or INCR16 */
	SYN_DMA_FIXED_BURST_DISABLE = 0x00000000,	/* SINGLE, INCR */
	SYN_DMA_TX_PRIORITY_RATIO11 = 0x00000000,	/* (PR)TX:RX DMA priority ratio 1:1 */
	SYN_DMA_TX_PRIORITY_RATIO21 = 0x00004000,	/* (PR)TX:RX DMA priority ratio 2:1 */
	SYN_DMA_TX_PRIORITY_RATIO31 = 0x00008000,	/* (PR)TX:RX DMA priority ratio 3:1 */
	SYN_DMA_TX_PRIORITY_RATIO41 = 0x0000C000,	/* (PR)TX:RX DMA priority ratio 4:1 */
	SYN_DMA_ADDRESS_ALIGNED_BEATS = 0x02000000,	/* Address Aligned beats */
	SYN_DMA_BURST_LENGTHX8 = 0x01000000,		/* When set mutiplies the PBL by 8 */
	SYN_DMA_BURST_LENGTH256 = 0x01002000,		/* (dma_burst_lengthx8 |
							   dma_burst_length32) = 256 */
	SYN_DMA_BURST_LENGTH128 = 0x01001000,		/* (dma_burst_lengthx8 |
							   dma_burst_length16) = 128 */
	SYN_DMA_BURST_LENGTH64 = 0x01000800,		/* (dma_burst_lengthx8 |
							   dma_burst_length8) = 64 */
	/* (PBL) programmable burst length */
	SYN_DMA_BURST_LENGTH32 = 0x00002000,		/* Dma burst length = 32 */
	SYN_DMA_BURST_LENGTH16 = 0x00001000,		/* Dma burst length = 16 */
	SYN_DMA_BURST_LENGTH8 = 0x00000800,		/* Dma burst length = 8  */
	SYN_DMA_BURST_LENGTH4 = 0x00000400,		/* Dma burst length = 4  */
	SYN_DMA_BURST_LENGTH2 = 0x00000200,		/* Dma burst length = 2  */
	SYN_DMA_BURST_LENGTH1 = 0x00000100,		/* Dma burst length = 1  */
	SYN_DMA_BURST_LENGTH0 = 0x00000000,		/* Dma burst length = 0  */

	SYN_DMA_DESCRIPTOR8_WORDS = 0x00000080,		/* Enh Descriptor works  1=>
							   8 word descriptor          */
	SYN_DMA_DESCRIPTOR4_WORDS = 0x00000000,		/* Enh Descriptor works  0=>
							   4 word descriptor          */
	SYN_DMA_DESCRIPTOR_SKIP16 = 0x00000040,		/* (DSL)Descriptor skip length (no.of dwords) */
	SYN_DMA_DESCRIPTOR_SKIP8 = 0x00000020,		/* between two unchained descriptors */
	SYN_DMA_DESCRIPTOR_SKIP4 = 0x00000010,
	SYN_DMA_DESCRIPTOR_SKIP2 = 0x00000008,
	SYN_DMA_DESCRIPTOR_SKIP1 = 0x00000004,
	SYN_DMA_DESCRIPTOR_SKIP0 = 0x00000000,
	SYN_DMA_ARBIT_RR = 0x00000000,			/* (DA) DMA RR arbitration    */
	SYN_DMA_ARBIT_PR = 0x00000002,			/* Rx has priority over Tx    */
	SYN_DMA_RESET_ON = 0x00000001,			/* (SWR)Software Reset DMA engine */
	SYN_DMA_RESET_OFF = 0x00000000,
};

/*
 * SYN_DMA_STATUS = 0x0014,		CSR5 - Dma status Register
 */
enum syn_dma_status_reg {
	SYN_DMA_GMAC_PMT_INTR = 0x10000000,		/* (GPI)Gmac subsystem interrupt */
	SYN_DMA_GMAC_MMC_INTR = 0x08000000,		/* (GMI)Gmac MMC subsystem interrupt */
	SYN_DMA_GMAC_LINE_INTF_INTR = 0x04000000,	/* Line interface interrupt */
	SYN_DMA_ERROR_BIT2 = 0x02000000,		/* (EB)Error bits 0-data buffer, 1-desc access */
	SYN_DMA_ERROR_BIT1 = 0x01000000,		/* (EB)Error bits 0-write trnsf, 1-read transfer */
	SYN_DMA_ERROR_BIT0 = 0x00800000,		/* (EB)Error bits 0-Rx DMA, 1-Tx DMA */
	SYN_DMA_TX_STATE = 0x00700000,			/* (TS)Transmit process state */
	SYN_DMA_TX_STOPPED = 0x00000000,		/* Stopped - Reset or Stop Tx Command issued */
	SYN_DMA_TX_FETCHING = 0x00100000,		/* Running - fetching the Tx descriptor */
	SYN_DMA_TX_WAITING = 0x00200000,		/* Running - waiting for status */
	SYN_DMA_TX_READING = 0x00300000,		/* Running - reading the data from host memory */
	SYN_DMA_TX_SUSPENDED = 0x00600000,		/* Suspended - Tx Descriptor unavailabe */
	SYN_DMA_TX_CLOSING = 0x00700000,		/* Running - closing Rx descriptor */
	SYN_DMA_RX_STATE = 0x000E0000,			/* (RS)Receive process state */
	SYN_DMA_RX_STOPPED = 0x00000000,		/* Stopped - Reset or Stop Rx Command issued */
	SYN_DMA_RX_FETCHING = 0x00020000,		/* Running - fetching the Rx descriptor */
	SYN_DMA_RX_WAITING = 0x00060000,		/* Running - waiting for packet */
	SYN_DMA_RX_SUSPENDED = 0x00080000,		/* Suspended - Rx Descriptor unavailable */
	SYN_DMA_RX_CLOSING = 0x000A0000,		/* Running - closing descriptor */
	SYN_DMA_RX_QUEUING = 0x000E0000,		/* Running - queuing the receive frame into host memory */
	SYN_DMA_INT_NORMAL = 0x00010000,		/* (NIS)Normal interrupt summary */
	SYN_DMA_INT_ABNORMAL = 0x00008000,		/* (AIS)Abnormal interrupt summary */
	SYN_DMA_INT_EARLY_RX = 0x00004000,		/* Early receive interrupt (Normal) */
	SYN_DMA_INT_BUS_ERROR = 0x00002000,		/* Fatal bus error (Abnormal) */
	SYN_DMA_INT_EARLY_TX = 0x00000400,		/* Early transmit interrupt (Abnormal) */
	SYN_DMA_INT_RX_WDOG_TO = 0x00000200,		/* Receive Watchdog Timeout (Abnormal) */
	SYN_DMA_INT_RX_STOPPED = 0x00000100,		/* Receive process stopped (Abnormal) */
	SYN_DMA_INT_RX_NO_BUFFER = 0x00000080,		/* RX buffer unavailable (Abnormal) */
	SYN_DMA_INT_RX_COMPLETED = 0x00000040,		/* Completion of frame RX (Normal) */
	SYN_DMA_INT_TX_UNDERFLOW = 0x00000020,		/* Transmit underflow (Abnormal) */
	SYN_DMA_INT_RCV_OVERFLOW = 0x00000010,		/* RX Buffer overflow interrupt */
	SYN_DMA_INT_TX_JABBER_TO = 0x00000008,		/* TX Jabber Timeout (Abnormal) */
	SYN_DMA_INT_TX_NO_BUFFER = 0x00000004,		/* TX buffer unavailable (Normal) */
	SYN_DMA_INT_TX_STOPPED = 0x00000002,		/* TX process stopped (Abnormal) */
	SYN_DMA_INT_TX_COMPLETED = 0x00000001,		/* Transmit completed (Normal) */
};

/*
 * SYN_DMA_OPERATION_MODE = 0x0018,		CSR6 - Dma Operation Mode Register
 */
enum syn_dma_operation_mode_reg {
	SYN_DMA_DISABLE_DROP_TCP_CS = 0x04000000,	/* (DT) Dis. drop. of tcp/ip
							   CS error frames            */
	SYN_DMA_RX_STORE_AND_FORWARD = 0x02000000,	/* Rx (SF)Store and forward   */
	SYN_DMA_RX_FRAME_FLUSH = 0x01000000,		/* Disable Receive Frame Flush*/
	SYN_DMA_TX_STORE_AND_FORWARD = 0x00200000,	/* Tx (SF)Store and forward   */
	SYN_DMA_FLUSH_TX_FIFO = 0x00100000,		/* (FTF)Tx FIFO controller
							   is reset to default        */
	SYN_DMA_TX_THRESH_CTRL = 0x0001C000,		/* (TTC)Controls thre Thresh of
							   MTL tx Fifo                */
	SYN_DMA_TX_THRESH_CTRL16 = 0x0001C000,		/* (TTC)Controls thre Thresh of
							   MTL tx Fifo 16             */
	SYN_DMA_TX_THRESH_CTRL24 = 0x00018000,		/* (TTC)Controls thre Thresh of
							   MTL tx Fifo 24             */
	SYN_DMA_TX_THRESH_CTRL32 = 0x00014000,		/* (TTC)Controls thre Thresh of
							   MTL tx Fifo 32             */
	SYN_DMA_TX_THRESH_CTRL40 = 0x00010000,		/* (TTC)Controls thre Thresh of
							   MTL tx Fifo 40             */
	SYN_DMA_TX_THRESH_CTRL256 = 0x0000c000,		/* (TTC)Controls thre Thresh of
							   MTL tx Fifo 256            */
	SYN_DMA_TX_THRESH_CTRL192 = 0x00008000,		/* (TTC)Controls thre Thresh of
							   MTL tx Fifo 192            */
	SYN_DMA_TX_THRESH_CTRL128 = 0x00004000,		/* (TTC)Controls thre Thresh of
							   MTL tx Fifo 128            */
	SYN_DMA_TX_THRESH_CTRL64 = 0x00000000,		/* (TTC)Controls thre Thresh of
							   MTL tx Fifo 64             */
	SYN_DMA_TX_START = 0x00002000,			/* (ST)Start/Stop transmission*/
	SYN_DMA_RX_FLOW_CTRL_DEACT = 0x00401800,	/* (RFD)Rx flow control
							   deact. Threshold            */
	SYN_DMA_RX_FLOW_CTRL_DEACT1K = 0x00000000,	/* (RFD)Rx flow control
							   deact. Threshold (1kbytes)  */
	SYN_DMA_RX_FLOW_CTRL_DEACT2K = 0x00000800,	/* (RFD)Rx flow control
							   deact. Threshold (2kbytes)  */
	SYN_DMA_RX_FLOW_CTRL_DEACT3K = 0x00001000,	/* (RFD)Rx flow control
							   deact. Threshold (3kbytes)  */
	SYN_DMA_RX_FLOW_CTRL_DEACT4K = 0x00001800,	/* (RFD)Rx flow control
							   deact. Threshold (4kbytes)  */
	SYN_DMA_RX_FLOW_CTRL_DEACT5K = 0x00400000,	/* (RFD)Rx flow control
							   deact. Threshold (4kbytes)  */
	SYN_DMA_RX_FLOW_CTRL_DEACT6K = 0x00400800,	/* (RFD)Rx flow control
							   deact. Threshold (4kbytes)  */
	SYN_DMA_RX_FLOW_CTRL_DEACT7K = 0x00401000,	/* (RFD)Rx flow control
							   deact. Threshold (4kbytes)  */
	SYN_DMA_RX_FLOW_CTRL_ACT = 0x00800600,		/* (RFA)Rx flow control
							   Act. Threshold              */
	SYN_DMA_RX_FLOW_CTRL_ACT1K = 0x00000000,	/* (RFA)Rx flow control
							   Act. Threshold (1kbytes)    */
	SYN_DMA_RX_FLOW_CTRL_ACT2K = 0x00000200,	/* (RFA)Rx flow control
							   Act. Threshold (2kbytes)    */
	SYN_DMA_RX_FLOW_CTRL_ACT3K = 0x00000400,	/* (RFA)Rx flow control
							   Act. Threshold (3kbytes)    */
	SYN_DMA_RX_FLOW_CTRL_ACT4K = 0x00000600,	/* (RFA)Rx flow control
							   Act. Threshold (4kbytes)    */
	SYN_DMA_RX_FLOW_CTRL_ACT5K = 0x00800000,	/* (RFA)Rx flow control
							   Act. Threshold (5kbytes)    */
	SYN_DMA_RX_FLOW_CTRL_ACT6K = 0x00800200,	/* (RFA)Rx flow control
							   Act. Threshold (6kbytes)    */
	SYN_DMA_RX_FLOW_CTRL_ACT7K = 0x00800400,	/* (RFA)Rx flow control
							   Act. Threshold (7kbytes)    */
	SYN_DMA_RX_THRESH_CTRL = 0x00000018,		/* (RTC)Controls thre
							   Thresh of MTL rx Fifo       */
	SYN_DMA_RX_THRESH_CTRL64 = 0x00000000,		/* (RTC)Controls thre
							   Thresh of MTL tx Fifo 64    */
	SYN_DMA_RX_THRESH_CTRL32 = 0x00000008,		/* (RTC)Controls thre
							   Thresh of MTL tx Fifo 32    */
	SYN_DMA_RX_THRESH_CTRL96 = 0x00000010,		/* (RTC)Controls thre
							   Thresh of MTL tx Fifo 96    */
	SYN_DMA_RX_THRESH_CTRL128 = 0x00000018,		/* (RTC)Controls thre
							   Thresh of MTL tx Fifo 128   */
	SYN_DMA_EN_HW_FLOW_CTRL = 0x00000100,		/* (EFC)Enable HW flow control*/
	SYN_DMA_DIS_HW_FLOW_CTRL = 0x00000000,		/* Disable HW flow control    */
	SYN_DMA_FWD_ERROR_FRAMES = 0x00000080,		/* (FEF)Forward error frames  */
	SYN_DMA_FWD_UNDER_SZ_FRAMES = 0x00000040,	/* (FUF)Forward undersize
							   frames                     */
	SYN_DMA_TX_SECOND_FRAME = 0x00000004,		/* (OSF)Operate on 2nd frame  */
	SYN_DMA_RX_START = 0x00000002,			/* (SR)Start/Stop reception   */
};

/*
 * SYN_DMA_INT_ENABLE = 0x101C,		CSR7 - Interrupt enable Register Layout
 */
enum syn_dma_interrupt_reg {
	SYN_DMA_IE_NORMAL = SYN_DMA_INT_NORMAL,		/* Normal interrupt enable */
	SYN_DMA_IE_ABNORMAL = SYN_DMA_INT_ABNORMAL,	/* Abnormal interrupt enable */
	SYN_DMA_IE_EARLY_RX = SYN_DMA_INT_EARLY_RX,	/* Early RX interrupt enable */
	SYN_DMA_IE_BUS_ERROR = SYN_DMA_INT_BUS_ERROR,	/* Fatal bus error enable */
	SYN_DMA_IE_EARLY_TX = SYN_DMA_INT_EARLY_TX,	/* Early TX interrupt enable */
	SYN_DMA_IE_RX_WDOG_TO = SYN_DMA_INT_RX_WDOG_TO,	/* RX Watchdog Timeout enable */
	SYN_DMA_IE_RX_STOPPED = SYN_DMA_INT_RX_STOPPED,	/* RX process stopped enable */
	SYN_DMA_IE_RX_NO_BUFFER = SYN_DMA_INT_RX_NO_BUFFER,
							/* Receive buffer unavailable enable */
	SYN_DMA_IE_RX_COMPLETED = SYN_DMA_INT_RX_COMPLETED,
							/* Completion of frame reception enable */
	SYN_DMA_IE_TX_UNDERFLOW = SYN_DMA_INT_TX_UNDERFLOW,
							/* TX underflow enable */
	SYN_DMA_IE_RX_OVERFLOW = SYN_DMA_INT_RCV_OVERFLOW,
							/* RX Buffer overflow interrupt */
	SYN_DMA_IE_TX_JABBER_TO = SYN_DMA_INT_TX_JABBER_TO,
							/* TX Jabber Timeout enable */
	SYN_DMA_IE_TX_NO_BUFFER = SYN_DMA_INT_TX_NO_BUFFER,
							/* TX buffer unavailable enable */
	SYN_DMA_IE_TX_STOPPED = SYN_DMA_INT_TX_STOPPED,
							/* TX process stopped enable */
	SYN_DMA_IE_TX_COMPLETED = SYN_DMA_INT_TX_COMPLETED,
							/* TX completed enable */
};

/*
 * SYN_DMA_AXI_BUS_MODE = 0x1028
 */
enum syn_dma_axi_bus_mode_reg {
	SYN_DMA_EN_LPI = 0x80000000,
	SYN_DMA_LPI_XIT_FRM = 0x40000000,
	SYN_DMA_WR_OSR_NUM_REQS16 = 0x00F00000,
	SYN_DMA_WR_OSR_NUM_REQS8 = 0x00700000,
	SYN_DMA_WR_OSR_NUM_REQS4 = 0x00300000,
	SYN_DMA_WR_OSR_NUM_REQS2 = 0x00100000,
	SYN_DMA_WR_OSR_NUM_REQS1 = 0x00000000,
	SYN_DMA_RD_OSR_NUM_REQS16 = 0x000F0000,
	SYN_DMA_RD_OSR_NUM_REQS8 = 0x00070000,
	SYN_DMA_RD_OSR_NUM_REQS4 = 0x00030000,
	SYN_DMA_RD_OSR_NUM_REQS2 = 0x00010000,
	SYN_DMA_RD_OSR_NUM_REQS1 = 0x00000000,
	SYN_DMA_ONEKBBE = 0x00002000,
	SYN_DMA_AXI_AAL = 0x00001000,
	SYN_DMA_AXI_BLEN256 = 0x00000080,
	SYN_DMA_AXI_BLEN128 = 0x00000040,
	SYN_DMA_AXI_BLEN64 = 0x00000020,
	SYN_DMA_AXI_BLEN32 = 0x00000010,
	SYN_DMA_AXI_BLEN16 = 0x00000008,
	SYN_DMA_AXI_BLEN8 = 0x00000004,
	SYN_DMA_AXI_BLEN4 = 0x00000002,
	SYN_DMA_UNDEFINED = 0x00000001,
};

/*
 * Values to initialize DMA registers
 */
enum syn_dma_init_values {
	/*
	 * Interrupt groups
	 */
	SYN_DMA_INT_ERROR_MASK = SYN_DMA_INT_BUS_ERROR,		/* Error              */
	SYN_DMA_INT_RX_ABN_MASK = SYN_DMA_INT_RX_NO_BUFFER,	/* RX abnormal intr   */
	SYN_DMA_INT_RX_NORM_MASK = SYN_DMA_INT_RX_COMPLETED,	/* RXnormal intr      */
	SYN_DMA_INT_RX_STOPPED_MASK = SYN_DMA_INT_RX_STOPPED,	/* RXstopped          */
	SYN_DMA_INT_TX_ABN_MASK = SYN_DMA_INT_TX_UNDERFLOW,	/* TX abnormal intr   */
	SYN_DMA_INT_TX_NORM_MASK = SYN_DMA_INT_TX_COMPLETED,	/* TX normal intr     */
	SYN_DMA_INT_TX_STOPPED_MASK = SYN_DMA_INT_TX_STOPPED,	/* TX stopped         */

	SYN_DMA_BUS_MODE_INIT = SYN_DMA_FIXED_BURST_ENABLE | SYN_DMA_BURST_LENGTH8
		| SYN_DMA_DESCRIPTOR_SKIP2 | SYN_DMA_RESET_OFF,

	SYN_DMA_BUS_MODE_VAL = SYN_DMA_BURST_LENGTH32
		| SYN_DMA_BURST_LENGTHX8 | SYN_DMA_DESCRIPTOR_SKIP0
		| SYN_DMA_DESCRIPTOR8_WORDS | SYN_DMA_ARBIT_PR | SYN_DMA_ADDRESS_ALIGNED_BEATS,

	SYN_DMA_OMR = SYN_DMA_TX_STORE_AND_FORWARD | SYN_DMA_RX_STORE_AND_FORWARD
		| SYN_DMA_RX_THRESH_CTRL128 | SYN_DMA_TX_SECOND_FRAME,

	SYN_DMA_INT_EN = SYN_DMA_IE_NORMAL | SYN_DMA_IE_ABNORMAL | SYN_DMA_INT_ERROR_MASK
		| SYN_DMA_INT_RX_ABN_MASK | SYN_DMA_INT_RX_NORM_MASK
		| SYN_DMA_INT_RX_STOPPED_MASK | SYN_DMA_INT_TX_ABN_MASK
		| SYN_DMA_INT_TX_NORM_MASK | SYN_DMA_INT_TX_STOPPED_MASK,
	SYN_DMA_INT_DISABLE = 0,
	SYN_DMA_AXI_BUS_MODE_VAL = SYN_DMA_AXI_BLEN16 | SYN_DMA_RD_OSR_NUM_REQS8
		| SYN_DMA_WR_OSR_NUM_REQS8,
};

/*
 * desc_mode
 *	GMAC descriptors mode
 */
enum desc_mode {
	RINGMODE = 0x00000001,
	CHAINMODE = 0x00000002,
};

extern void syn_disable_dma_interrupt(struct nss_gmac_hal_dev *nghd);
extern void syn_enable_dma_interrupt(struct nss_gmac_hal_dev *nghd);
extern void syn_enable_dma_rx(struct nss_gmac_hal_dev *nghd);
extern void syn_disable_dma_rx(struct nss_gmac_hal_dev *nghd);
extern void syn_enable_dma_tx(struct nss_gmac_hal_dev *nghd);
extern void syn_disable_dma_tx(struct nss_gmac_hal_dev *nghd);
extern void syn_clear_dma_status(struct nss_gmac_hal_dev *nghd);
extern void syn_resume_dma_tx(struct nss_gmac_hal_dev *nghd);
extern uint32_t syn_get_rx_missed(struct nss_gmac_hal_dev *nghd);
extern uint32_t syn_get_fifo_overflows(struct nss_gmac_hal_dev *nghd);

extern void syn_init_tx_desc_base(struct nss_gmac_hal_dev *nghd, uint32_t tx_desc_dma);
extern void syn_init_rx_desc_base(struct nss_gmac_hal_dev *nghd, uint32_t rx_desc_dma);

#endif /*__SYN_REG_H__*/
