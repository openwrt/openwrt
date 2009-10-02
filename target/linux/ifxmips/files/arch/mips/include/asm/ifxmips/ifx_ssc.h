/*
 * ifx_ssc.h defines some data sructures used in ifx_ssc.c
 *
 * Copyright (C) 2004 Michael Schoenenborn (IFX COM TI BT)
 *
 *
 */

#ifndef __IFX_SSC_H
#define __IFX_SSC_H
#ifdef __KERNEL__
#include <asm/ifxmips/ifx_ssc_defines.h>
#endif //__KERNEL__

#define PORT_CNT		1	// assume default value

/* symbolic constants to be used in SSC routines */

// ### TO DO: bad performance
#define IFX_SSC_TXFIFO_ITL	1
#define IFX_SSC_RXFIFO_ITL	1

struct ifx_ssc_statistics {
	unsigned int abortErr;	/* abort error */
	unsigned int modeErr;	/* master/slave mode error */
	unsigned int txOvErr;	/* TX Overflow error */
	unsigned int txUnErr;	/* TX Underrun error */
	unsigned int rxOvErr;	/* RX Overflow error */
	unsigned int rxUnErr;	/* RX Underrun error */
	unsigned int rxBytes;
	unsigned int txBytes;
};

struct ifx_ssc_hwopts {
	unsigned int AbortErrDetect:1;	/* Abort Error detection (in slave mode) */
	unsigned int rxOvErrDetect:1;	/* Receive Overflow Error detection */
	unsigned int rxUndErrDetect:1;	/* Receive Underflow Error detection */
	unsigned int txOvErrDetect:1;	/* Transmit Overflow Error detection */
	unsigned int txUndErrDetect:1;	/* Transmit Underflow Error detection */
	unsigned int echoMode:1;	/* Echo mode */
	unsigned int loopBack:1;	/* Loopback mode */
	unsigned int idleValue:1;	/* Idle value */
	unsigned int clockPolarity:1;	/* Idle clock is high or low */
	unsigned int clockPhase:1;	/* Tx on trailing or leading edge */
	unsigned int headingControl:1;	/* LSB first or MSB first */
	unsigned int dataWidth:6;	/* from 2 up to 32 bits */
	unsigned int masterSelect:1;	/* Master or Slave mode */
	unsigned int modeRxTx:2;	/* rx/tx mode */
	unsigned int gpoCs:8;	/* choose outputs to use for chip select */
	unsigned int gpoInv:8;	/* invert GPO outputs */
};

struct ifx_ssc_frm_opts {
	bool FrameEnable;	// SFCON.SFEN
	unsigned int DataLength;	// SFCON.DLEN
	unsigned int PauseLength;	// SFCON.PLEN
	unsigned int IdleData;	// SFCON.IDAT
	unsigned int IdleClock;	// SFCON.ICLK
	bool StopAfterPause;	// SFCON.STOP
};

struct ifx_ssc_frm_status {
	bool DataBusy;		// SFSTAT.DBSY
	bool PauseBusy;		// SFSTAT.PBSY
	unsigned int DataCount;	// SFSTAT.DCNT
	unsigned int PauseCount;	// SFSTAT.PCNT
	bool EnIntAfterData;	// SFCON.IBEN
	bool EnIntAfterPause;	// SFCON.IAEN
};

typedef struct {
	char *buf;
	size_t len;
} ifx_ssc_buf_item_t;

// data structures for batch execution
typedef union {
	struct {
		bool save_options;
	} init;
	ifx_ssc_buf_item_t read;
	ifx_ssc_buf_item_t write;
	ifx_ssc_buf_item_t rd_wr;
	unsigned int set_baudrate;
	struct ifx_ssc_frm_opts set_frm;
	unsigned int set_gpo;
	struct ifx_ssc_hwopts set_hwopts;
} ifx_ssc_batch_cmd_param;

struct ifx_ssc_batch_list {
	unsigned int cmd;
	ifx_ssc_batch_cmd_param cmd_param;
	struct ifx_ssc_batch_list *next;
};

#ifdef __KERNEL__
#define IFX_SSC_IS_MASTER(p) ((p)->opts.masterSelect == SSC_MASTER_MODE)

struct ifx_ssc_port {
	unsigned long mapbase;
	struct ifx_ssc_hwopts opts;
	struct ifx_ssc_statistics stats;
	struct ifx_ssc_frm_status frm_status;
	struct ifx_ssc_frm_opts frm_opts;
	/* wait queue for ifx_ssc_read() */
	wait_queue_head_t rwait, pwait;
	int port_nr;
	char port_is_open;	/* exclusive open  - boolean */
//      int no_of_bits; /* number of _valid_ bits */
//      int elem_size; /* shift for element (no of bytes)*/
	/* buffer and pointers to the read/write position */
	char *rxbuf;		/* buffer for RX */
	char *rxbuf_end;	/* buffer end pointer for RX */
	volatile char *rxbuf_ptr;	/* buffer write pointer for RX */
	char *txbuf;		/* buffer for TX */
	char *txbuf_end;	/* buffer end pointer for TX */
	volatile char *txbuf_ptr;	/* buffer read pointer for TX */
	unsigned int baud;
	/* each channel has its own interrupts */
	/* (transmit/receive/error/frame) */
	unsigned int txirq, rxirq, errirq, frmirq;
};
/* default values for SSC configuration */
// values of CON
#define IFX_SSC_DEF_IDLE_DATA       1	/* enable */
#define IFX_SSC_DEF_BYTE_VALID_CTL  1	/* enable */
#define IFX_SSC_DEF_DATA_WIDTH      32	/* bits */
#define IFX_SSC_DEF_ABRT_ERR_DETECT 0	/* disable */
#define IFX_SSC_DEF_RO_ERR_DETECT   1	/* enable */
#define IFX_SSC_DEF_RU_ERR_DETECT   0	/* disable */
#define IFX_SSC_DEF_TO_ERR_DETECT   0	/* disable */
#define IFX_SSC_DEF_TU_ERR_DETECT   0	/* disable */
#define IFX_SSC_DEF_LOOP_BACK       0	/* disable */
#define IFX_SSC_DEF_ECHO_MODE       0	/* disable */
#define IFX_SSC_DEF_CLOCK_POLARITY  0	/* low */
#define IFX_SSC_DEF_CLOCK_PHASE     1	/* 0: shift on leading edge, latch on trailling edge, 1, otherwise */
#define IFX_SSC_DEF_HEADING_CONTROL IFX_SSC_MSB_FIRST
#define IFX_SSC_DEF_MODE_RXTX	    IFX_SSC_MODE_RXTX
// other values
#define IFX_SSC_DEF_MASTERSLAVE	    IFX_SSC_MASTER_MODE	/* master */
#ifdef CONFIG_USE_EMULATOR
#define IFX_SSC_DEF_BAUDRATE	    10000
#else
#define IFX_SSC_DEF_BAUDRATE	    2000000
#endif
#define IFX_SSC_DEF_RMC	    	    0x10

#define IFX_SSC_DEF_TXFIFO_FL       8
#define IFX_SSC_DEF_RXFIFO_FL       1

#if 1				//TODO
#define IFX_SSC_DEF_GPO_CS	    0x3	/* no chip select */
#define IFX_SSC_DEF_GPO_INV	    0	/* no chip select */
#else
#error "what is ur Chip Select???"
#endif
#define IFX_SSC_DEF_SFCON	    0	/* no serial framing */
#if 0
#define IFX_SSC_DEF_IRNEN	    IFX_SSC_T_BIT | /* enable all int's */\
				    IFX_SSC_R_BIT | IFX_SSC_E_BIT | IFX_SSC_F_BIT
#endif
#define IFX_SSC_DEF_IRNEN	    IFX_SSC_T_BIT | /* enable all int's */\
				    IFX_SSC_R_BIT | IFX_SSC_E_BIT
#endif /* __KERNEL__ */

// batch execution commands
#define IFX_SSC_BATCH_CMD_INIT		1
#define IFX_SSC_BATCH_CMD_READ		2
#define IFX_SSC_BATCH_CMD_WRITE		3
#define IFX_SSC_BATCH_CMD_RD_WR		4
#define IFX_SSC_BATCH_CMD_SET_BAUDRATE	5
#define IFX_SSC_BATCH_CMD_SET_HWOPTS	6
#define IFX_SSC_BATCH_CMD_SET_FRM	7
#define IFX_SSC_BATCH_CMD_SET_GPO	8
#define IFX_SSC_BATCH_CMD_FIFO_FLUSH	9
//#define IFX_SSC_BATCH_CMD_    
//#define IFX_SSC_BATCH_CMD_    
#define IFX_SSC_BATCH_CMD_END_EXEC	0

/* Macros to configure SSC hardware */
/* headingControl: */
#define IFX_SSC_LSB_FIRST            0
#define IFX_SSC_MSB_FIRST            1
/* dataWidth: */
#define IFX_SSC_MIN_DATA_WIDTH       2
#define IFX_SSC_MAX_DATA_WIDTH       32
/* master/slave mode select */
#define IFX_SSC_MASTER_MODE          1
#define IFX_SSC_SLAVE_MODE           0
/* rx/tx mode */
// ### TO DO: !!! ATTENTION! Hardware dependency => move to ifx_ssc_defines.h
#define IFX_SSC_MODE_RXTX	     0
#define IFX_SSC_MODE_RX		     1
#define IFX_SSC_MODE_TX		     2
#define IFX_SSC_MODE_OFF	     3
#define IFX_SSC_MODE_MASK	     IFX_SSC_MODE_RX | IFX_SSC_MODE_TX

/* GPO values */
#define IFX_SSC_MAX_GPO_OUT	     7

#define IFX_SSC_RXREQ_BLOCK_SIZE     32768

/***********************/
/* defines for ioctl's */
/***********************/
#define IFX_SSC_IOCTL_MAGIC     'S'
/* read out the statistics */
#define IFX_SSC_STATS_READ _IOR(IFX_SSC_IOCTL_MAGIC, 1, struct ifx_ssc_statistics)
/* clear the statistics */
#define IFX_SSC_STATS_RESET _IO(IFX_SSC_IOCTL_MAGIC, 2)
/* set the baudrate */
#define IFX_SSC_BAUD_SET _IOW(IFX_SSC_IOCTL_MAGIC, 3, unsigned int)
/* get the current baudrate */
#define IFX_SSC_BAUD_GET _IOR(IFX_SSC_IOCTL_MAGIC, 4, unsigned int)
/* set hardware options */
#define IFX_SSC_HWOPTS_SET _IOW(IFX_SSC_IOCTL_MAGIC, 5, struct ifx_ssc_hwopts)
/* get the current hardware options */
#define IFX_SSC_HWOPTS_GET _IOR(IFX_SSC_IOCTL_MAGIC, 6, struct ifx_ssc_hwopts)
/* set transmission mode */
#define IFX_SSC_RXTX_MODE_SET _IOW(IFX_SSC_IOCTL_MAGIC, 7, unsigned int)
/* get the current transmission mode */
#define IFX_SSC_RXTX_MODE_GET _IOR(IFX_SSC_IOCTL_MAGIC, 8, unsigned int)
/* abort transmission */
#define IFX_SSC_ABORT _IO(IFX_SSC_IOCTL_MAGIC, 9)
#define IFX_SSC_FIFO_FLUSH _IO(IFX_SSC_IOCTL_MAGIC, 9)

/* set general purpose outputs */
#define IFX_SSC_GPO_OUT_SET _IOW(IFX_SSC_IOCTL_MAGIC, 32, unsigned int)
/* clear general purpose outputs */
#define IFX_SSC_GPO_OUT_CLR _IOW(IFX_SSC_IOCTL_MAGIC, 33, unsigned int)
/* get general purpose outputs */
#define IFX_SSC_GPO_OUT_GET _IOR(IFX_SSC_IOCTL_MAGIC, 34, unsigned int)

/*** serial framing ***/
/* get status of serial framing */
#define IFX_SSC_FRM_STATUS_GET _IOR(IFX_SSC_IOCTL_MAGIC, 48, struct ifx_ssc_frm_status)
/* get counter reload values and control bits */
#define IFX_SSC_FRM_CONTROL_GET _IOR(IFX_SSC_IOCTL_MAGIC, 49, struct ifx_ssc_frm_opts)
/* set counter reload values and control bits */
#define IFX_SSC_FRM_CONTROL_SET _IOW(IFX_SSC_IOCTL_MAGIC, 50, struct ifx_ssc_frm_opts)

/*** batch execution ***/
/* do batch execution */
#define IFX_SSC_BATCH_EXEC _IOW(IFX_SSC_IOCTL_MAGIC, 64, struct ifx_ssc_batch_list)

#ifdef __KERNEL__
// routines from ifx_ssc.c
// ### TO DO
/* kernel interface for read and write */
ssize_t ifx_ssc_kread (int, char *, size_t);
ssize_t ifx_ssc_kwrite (int, const char *, size_t);

#ifdef CONFIG_IFX_VP_KERNEL_TEST
void ifx_ssc_tc (void);
#endif // CONFIG_IFX_VP_KERNEL_TEST

#endif //__KERNEL__
#endif // __IFX_SSC_H
