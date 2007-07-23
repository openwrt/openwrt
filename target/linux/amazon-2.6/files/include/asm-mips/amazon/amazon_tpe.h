#ifndef AMAZON_TPE_H
#define AMAZON_TPE_H
#include <linux/atm.h>
#include <linux/atmdev.h>
#include <linux/netdevice.h>
#include <linux/ioctl.h>

#ifdef CONFIG_IFX_ATM_MIB
/* For ATM-MIB lists */
#include <linux/list.h>
#endif
#include <asm/amazon/atm_mib.h>

/* CBM Queue arranagement
 * Q0: free cells pool
 * Q1~ Q15: upstream queues
 * Q16: QAM downstream
 * Q17~Q31: downstream queues
 */
#define	AMAZON_ATM_MAX_QUEUE_NUM	32
#define	AMAZON_ATM_PORT_NUM		2
#define	AMAZON_ATM_FREE_CELLS		4000
#define	AMAZON_ATM_MAX_VCC_NUM		(AMAZON_ATM_MAX_QUEUE_NUM/2 - 1)
#define AMAZON_AAL0_SDU 		(ATM_AAL0_SDU+4)				//one more word for status
#define CBM_RX_OFFSET			16						//offset from the same q for tx
#define	AMAZON_ATM_OAM_Q_ID           	16
#define	AMAZON_ATM_RM_Q_ID            	16
#define	AMAZON_ATM_OTHER_Q_ID         	16
#define	CBM_DEFAULT_Q_OFFSET          	1
#define	HTUTIMEOUT                    	0xffff//timeoutofhtutocbm
#define	QSB_WFQ_NONUBR_MAX            	0x3f00
#define	QSB_WFQ_UBR_BYPASS            	0x3fff
#define	QSB_TP_TS_MAX			65472
#define	QSB_TAUS_MAX			64512
#define QSB_GCR_MIN			18
#define HTU_RAM_ACCESS_MAX		1024//maxium time for HTU RAM access

#define SWIE_LOCK		1
#define	PROC_ATM		1
#define	PROC_MIB		2
#define PROC_VCC		3
#define PROC_AAL5               4
#define PROC_CBM                5
#define PROC_HTU                6
#define PROC_QSB                7
#define PROC_SWIE               8

/***************** internal data structure ********************/
typedef int (*push_back_t)(struct atm_vcc *vcc,struct sk_buff *skb,int err) ;
/* Device private data */
typedef struct{
	u8	padding_byte;
	u32	tx_max_sdu;
	u32	rx_max_sdu;
	u32	cnt_cpy;		//no. of packets that need a copy due to alignment
}amazon_aal5_dev_t;

typedef struct{
	u32	max_q_off;		//maxium queues used in real scenario
	u32	nrt_thr;
	u32	clp0_thr;
	u32	clp1_thr;
	u32	free_cell_cnt;
#ifdef CONFIG_USE_VENUS	
	u8 *	qd_addr_free;	//to work around a bug, bit15 of QDOFF address should be 1
#endif
	u8 *	qd_addr;
	u8 *	mem_addr;
	u8	allocated;
}amazon_cbm_dev_t;

typedef struct{

}amazon_htu_dev_t;

typedef struct{
	u32	tau;			//cell delay variation due to concurrency(?)
	u32	tstepc;			//time step, all legal values are 1,2,4
	u32	sbl;			//scheduler burse length (for PHY)
}amazon_qsb_dev_t;

typedef struct{
	u32	qid;			//QID of the current extraction queue
	struct semaphore in_sem;	// Software-Insertion semaphore
	volatile long	lock;		//lock that avoids race contions between SWIN and SWEX
	wait_queue_head_t sleep;	//wait queue for SWIE and SWEX
	u32 sw;			//status word
}amazon_swie_dev_t;

//AAL5 MIB Counter
typedef struct{
	u32	tx,rx;			//number AAL5 CPCS PDU from/to higher-layer
	u32	tx_err,rx_err;  	//ifInErrors and ifOutErros
	u32	tx_drop,rx_drop;	//discarded received packets due to mm shortage
	u32	htu_unp;		//number of unknown received cells
	u32 	rx_cnt_h;		//number of octets received, high 32 bits
	u32 	rx_cnt_l;		//number of octets received, low 32 bits
	u32 	tx_cnt_h;		//number of octets transmitted, high 32 bits
	u32 	tx_cnt_l;		//number of octets transmitted, low 32 bits
	u32	tx_ppd;			//number of cells for AAL5 upstream PPD discards
	u64	rx_cells;		//number of cells for downstream
	u64	tx_cells;		//number of cells for upstream
	u32	rx_err_cells;		//number of cells dropped due to uncorrectable HEC errors
}amazon_mib_counter_t;



typedef enum {QS_PKT,QS_LEN,QS_ERR,QS_HW_DROP,QS_SW_DROP,QS_MAX} qs_t;
//queue statics no. of packet received / sent
//queue statics no. of bytes received / sent
//queue statics no. of packets with error
//queue statics no. of packets dropped by hw
//queue statics no. of packets dropped by sw

typedef struct{
	push_back_t push;		//call back function
	struct atm_vcc * vcc;		//opened vcc
	struct timeval access_time;	//time when last F4/F5 user cells arrive
	int	free;			//whether this queue is occupied, 0: occupied, 1: free
	u32	aal5VccCrcErrors;	//MIB counter
	u32	aal5VccOverSizedSDUs;	//MIB counter

#if defined(AMAZON_ATM_DEBUG) || defined (CONFIG_IFX_ATM_MIB)
	u32	qs[QS_MAX];
#endif	
}amazon_atm_queue_t;


typedef struct{
	int	enable;		//enable / disable
	u32	max_conn;	//maximum number of connections per port
	u32	tx_max_cr;	//Remaining cellrate for this device for tx direction
	u32	tx_rem_cr;	//Remaining cellrate for this device for tx direction
	u32	tx_cur_cr;	//Current cellrate for this device for tx direction
}amazon_atm_port_t;

typedef struct{
	amazon_aal5_dev_t	aal5;
	amazon_cbm_dev_t	cbm;
	amazon_htu_dev_t	htu;
	amazon_qsb_dev_t	qsb;
	amazon_swie_dev_t	swie;
	amazon_mib_counter_t	mib_counter;
	amazon_atm_queue_t	queues[AMAZON_ATM_MAX_QUEUE_NUM];
	amazon_atm_port_t	ports[AMAZON_ATM_PORT_NUM];
	atomic_t		dma_tx_free_0;//TX_CH0 has availabe descriptors
} amazon_atm_dev_t;

struct oam_last_activity{
	u8			vpi;	//vpi for this connection	
	u16			vci;	//vci for t his connection
	struct timeval		stamp;	//time when last F4/F5 user cells arrive
	struct oam_last_activity * next;//for link list purpose
};

typedef union{
#ifdef CONFIG_CPU_LITTLE_ENDIAN
	struct{
		u32	tprs		:16;
		u32	twfq		:14;
		u32	vbr		:1;
		u32 	reserved	:1;
	}bit;
	u32 w0;
#else
	struct{
		u32 	reserved	:1;
		u32	vbr		:1;
		u32	twfq		:14;
		u32	tprs		:16;
	}bit;
	u32 w0;
#endif

}qsb_qptl_t;

typedef union{
#ifdef CONFIG_CPU_LITTLE_ENDIAN
	struct{
		u32	ts		:16;
		u32	taus		:16;
	}bit;
	u32 w0;
#else
	struct{
		u32	taus		:16;
		u32	ts		:16;
	}bit;
	u32 w0;
#endif
}qsb_qvpt_t;



struct amazon_atm_cell_header {
#ifdef CONFIG_CPU_LITTLE_ENDIAN
	struct{
		u32 clp 	:1;	// Cell Loss Priority
		u32 pti		:3;	// Payload Type Identifier
		u32 vci 	:16;	// Virtual Channel Identifier
		u32 vpi		:8;	// Vitual Path Identifier
		u32 gfc 	:4;	// Generic Flow Control
		}bit;
#else
	struct{
		u32 gfc 	:4;	// Generic Flow Control
		u32 vpi		:8;	// Vitual Path Identifier
		u32 vci 	:16;	// Virtual Channel Identifier
		u32 pti		:3;	// Payload Type Identifier
		u32 clp 	:1;	// Cell Loss Priority
		}bit;
#endif
};


/************************ Function Declarations **************************/
amazon_atm_dev_t * 	amazon_atm_create(void);
int	amazon_atm_open(struct atm_vcc *vcc,push_back_t);
int	amazon_atm_send(struct atm_vcc *vcc,struct sk_buff *skb);
int	amazon_atm_send_oam(struct atm_vcc *vcc,void *cell, int flags);
void	amazon_atm_close(struct atm_vcc *vcc);
void	amazon_atm_cleanup(void);
const struct oam_last_activity* get_oam_time_stamp(void);

//mib-related
int amazon_atm_cell_mib(atm_cell_ifEntry_t * to,u32 itf);
int amazon_atm_aal5_mib(atm_aal5_ifEntry_t * to);
int amazon_atm_vcc_mib(struct atm_vcc *vcc,atm_aal5_vcc_t * to);
int amazon_atm_vcc_mib_x(int vpi, int vci,atm_aal5_vcc_t* to);

#define	AMAZON_WRITE_REGISTER_L(data,addr)	do{ *((volatile u32*)(addr)) = (u32)(data); wmb();} while (0)
#define AMAZON_READ_REGISTER_L(addr) 	(*((volatile u32*)(addr)))
/******************************* ioctl stuff****************************************/
#define NUM(dev)    (MINOR(dev) & 0xf)
/*
 * Ioctl definitions
 */
/* Use 'o' as magic number */
#define AMAZON_ATM_IOC_MAGIC  'o'
/* MIB_CELL: get atm cell level mib counter
 * MIB_AAL5: get aal5 mib counter
 * MIB_VCC: get vcc mib counter
 */
typedef struct{
	int vpi;
	int vci;
	atm_aal5_vcc_t mib_vcc;
}atm_aal5_vcc_x_t;
#define AMAZON_ATM_MIB_CELL		_IOWR(AMAZON_ATM_IOC_MAGIC,  0, atm_cell_ifEntry_t)
#define AMAZON_ATM_MIB_AAL5		_IOWR(AMAZON_ATM_IOC_MAGIC,  1, atm_aal5_ifEntry_t)
#define AMAZON_ATM_MIB_VCC		_IOWR(AMAZON_ATM_IOC_MAGIC,  2, atm_aal5_vcc_x_t)
#define AMAZON_ATM_IOC_MAXNR 	3

//sockopt
#define	SO_AMAZON_ATM_MIB_VCC		__SO_ENCODE(SOL_ATM,5,atm_aal5_vcc_t)

#endif // AMAZON_TPE_H

