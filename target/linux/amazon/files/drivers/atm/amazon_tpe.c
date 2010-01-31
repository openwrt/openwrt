/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */
//-----------------------------------------------------------------------
/*
 * Description:
 *	Driver for Infineon Amazon TPE
 */
//-----------------------------------------------------------------------
/* Author:	peng.liu@infineon.com
 * Created:	12-April-2004
 */
//-----------------------------------------------------------------------
/* History
 * Last changed on: 13 Oct. 2004
 * Last changed by: peng.liu@infineon.com
 * Last changed on: 28 Jan. 2004
 * Last changed by: peng.liu@infineon.com
 * Last changed Reason:
 *	- AAL5R may send more bytes than expected in MFL (so far, confirmed as 64 bytes)
 */
// 507261:tc.chen 2005/07/26 re-organize code address map to improve performance.
// 507281:tc.chen 2005/07/28 fix f4 segment isssue
/* 511045:linmars 2005/11/04 from Liu.Peng: change NRT_VBR bandwidth calculation based on scr instead of pcr */
 
#ifndef __KERNEL__
#define __KERNEL__
#endif 
#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif

/*TPE level loopback, bypass AWARE DFE */
#undef TPE_LOOPBACK

/* enable debug options */			
#undef AMAZON_ATM_DEBUG

/* enable rx error packet analysis */
#undef AMAZON_ATM_DEBUG_RX

/* test AAL5 Interrupt */
#undef  AMAZON_TPE_TEST_AAL5_INT

/* dump packet */
#undef AMAZON_TPE_DUMP

/* read ARC register*/
/* this register is located in side DFE module*/
#undef AMAZON_TPE_READ_ARC

/* software controlled reassembly */
#undef AMAZON_TPE_SCR

/* recovery from AAL5 bug */
#undef AMAZON_TPE_AAL5_RECOVERY

#if defined(AMAZON_TPE_READ_ARC) || defined(AMAZON_TPE_AAL5_RECOVERY)
#define ALPHAEUS_BASE_ADDR	0x31c00
#define A_CFG_ADDR		(ALPHAEUS_BASE_ADDR+0x04)
#define AR_CB0_STATUS_ADDR		(ALPHAEUS_BASE_ADDR+0x2c)
#define AR_CB1_STATUS_ADDR		(ALPHAEUS_BASE_ADDR+0x30)
#define AT_CELL0_ADDR		(ALPHAEUS_BASE_ADDR+0x90)
#define AR_CELL0_ADDR		(ALPHAEUS_BASE_ADDR+0x1a0)
#define AR_CD_CNT0_ADDR		(ALPHAEUS_BASE_ADDR+0x1c8)
#endif

#include <linux/module.h>
#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel.h> 
#include <linux/slab.h>  
#include <linux/fs.h> 
#include <linux/types.h>
#include <linux/errno.h>  
#include <linux/time.h>
#include <linux/atm.h>
#include <linux/atmdev.h>
#include <linux/netdevice.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/atomic.h>
#include <asm/bitops.h>
#include <asm/system.h>

#include <asm/amazon/amazon.h>
#include <asm/amazon/irq.h>

#include <linux/in.h>
#include <linux/netdevice.h> 
#include <linux/etherdevice.h> 
#include <linux/ip.h>
#include <linux/tcp.h> 
#include <linux/skbuff.h>
#include <linux/in6.h>
#include <linux/delay.h>
#include <asm/amazon/atm_defines.h>
#include <asm/amazon/amazon_dma.h>
#include <asm/amazon/amazon_tpe.h>

#if defined(AMAZON_TPE_READ_ARC) || defined(AMAZON_TPE_AAL5_RECOVERY)
#include <asm/amazon/amazon_mei.h>
#include <asm/amazon/amazon_mei_app.h>
#endif

#define AMAZON_TPE_EMSG(fmt, args...) printk( KERN_ERR  "%s: " fmt,__FUNCTION__, ## args)

/***************************************** External Functions *******************************************/
extern unsigned int amazon_get_fpi_hz(void);
extern void mask_and_ack_amazon_irq(unsigned int irq_nr);
extern void amz_push_oam(unsigned char *);

//amazon_mei.c
#if defined(AMAZON_TPE_READ_ARC) || defined(AMAZON_TPE_AAL5_RECOVERY)
extern MEI_ERROR meiDebugRead(u32 srcaddr, u32 *databuff, u32 databuffsize);
extern MEI_ERROR meiDebugWrite(u32 destaddr, u32 *databuff, u32 databuffsize);
#endif

/***************************************** Internal Functions *******************************************/
int amazon_atm_read_procmem(char *buf, char **start, off_t offset,int count, int *eof, void *data);
/***************************************** Global Data *******************************************/
amazon_atm_dev_t g_atm_dev;		//device data
static struct tq_struct swex_start_task;	//BH task
static struct tq_struct swex_complete_task;	//BH task
#ifdef AMAZON_TPE_SCR
static struct tq_struct a5r_task; 		//BH task
#endif
static struct dma_device_info 	g_dma_dev;	//for DMA
static struct atm_dev * amazon_atm_devs[AMAZON_ATM_PORT_NUM];
static struct oam_last_activity g_oam_time_stamp[AMAZON_ATM_MAX_VCC_NUM];
static u8 g_oam_cell[AMAZON_AAL0_SDU+4];	//for OAM cells
#ifdef AMAZON_CHECK_LINK
static int adsl_link_status;			//ADSL link status, 0:down, 1:up
#endif //AMAZON_CHECK_LINK
/***************************************** Module Parameters *************************************/
// Parameter Definition for module
static int port_enable0 = 1;				// Variable for parameter port_enable0
static int port_enable1 = 0;				// Variable for parameter port_enable1
static int port_max_conn0 = 15;				// Variable for parameter port_max_conn0
static int port_max_conn1 = 0;				// Variable for parameter port_max_conn1
static int port_cell_rate_up0 = 7500;			// Variable for parameter port_cell_rate_up0
static int port_cell_rate_up1 = 7500;			// Variable for parameter port_cell_rate_up1


static int qsb_tau = 1;					// Variable for parameter qsb_tau
static int qsb_srvm = 0xf;				// Variable for parameter qsb_srvm
static int qsb_tstep = 4 ;				// Variable for parameter qsb_tstep

static int cbm_nrt = 3900;				// Variable for parameter cbm_nrt
static int cbm_clp0 =3500;				// Variable for parameter cbm_clp0
static int cbm_clp1 =3200;				// Variable for parameter cbm_clp1
static int cbm_free_cell_no = AMAZON_ATM_FREE_CELLS;	// Variable for parameter cbm_free_cell_no

static int a5_fill_pattern = 0x7e;			// Variable for parameter a5_fill_pattern '~'
static int a5s_mtu = 0x700;				// mtu for tx
static int a5r_mtu = 0x700;				// mtu for rx

static int oam_q_threshold = 	64;			// oam queue threshold, minium value 64
static int rx_q_threshold = 	1000;			// rx queue threshold, minium value 64
static int tx_q_threshold = 	800;			// tx queue threshold, minium value 64

MODULE_PARM(port_max_conn0, "i");
MODULE_PARM_DESC(port_max_conn0, "Maximum atm connection for port #0");
MODULE_PARM(port_max_conn1, "i");
MODULE_PARM_DESC(port_max_conn1, "Maximum atm connection for port #1");
MODULE_PARM(port_enable0, "i");
MODULE_PARM_DESC(port_enable0, "0 -> port disabled, 1->port enabled");
MODULE_PARM(port_enable1, "i");
MODULE_PARM_DESC(port_enable1, "0 -> port disabled, 1->port enabled");
MODULE_PARM(port_cell_rate_up0, "i");
MODULE_PARM_DESC(port_cell_rate_up0, "ATM port upstream rate in cells/s");
MODULE_PARM(port_cell_rate_up1, "i");
MODULE_PARM_DESC(port_cell_rate_up1, "ATM port upstream rate in cells/s");

MODULE_PARM(qsb_tau,"i");
MODULE_PARM_DESC(qsb_tau, "Cell delay variation. value must be > 0");
MODULE_PARM(qsb_srvm, "i");
MODULE_PARM_DESC(qsb_srvm, "Maximum burst size");
MODULE_PARM(qsb_tstep, "i");
MODULE_PARM_DESC(qsb_tstep, "n*32 cycles per sbs cycles n=1,2,4");

MODULE_PARM(cbm_nrt, "i");
MODULE_PARM_DESC(cbm_nrt, "Non real time threshold for cell buffer");
MODULE_PARM(cbm_clp0, "i");
MODULE_PARM_DESC(cbm_clp0, "Threshold for cells with cell loss priority 0");
MODULE_PARM(cbm_clp1, "i");
MODULE_PARM_DESC(cbm_clp1, "Threshold for cells with cell loss priority 1");
MODULE_PARM(cbm_free_cell_no, "i");
MODULE_PARM_DESC(cbm_free_cell_no, "Number of cells in the cell buffer manager");

MODULE_PARM(a5_fill_pattern, "i");
MODULE_PARM_DESC(a5_fill_pattern, "filling pattern (PAD) for aal5 frames");
MODULE_PARM(a5s_mtu, "i");
MODULE_PARM_DESC(a5s_mtu, "max. SDU for upstream");
MODULE_PARM(a5r_mtu, "i");
MODULE_PARM_DESC(a5r_mtu, "max. SDU for downstream");                                                                               

MODULE_PARM(oam_q_threshold, "i");
MODULE_PARM_DESC(oam_q_threshold, "oam queue threshold");

MODULE_PARM(rx_q_threshold, "i");
MODULE_PARM_DESC(rx_q_threshold, "downstream/rx queue threshold");

MODULE_PARM(tx_q_threshold, "i");
MODULE_PARM_DESC(tx_q_threshold, "upstream/tx queue threshold");

/***************************************** local functions *************************************/
/* Brief: valid QID
 * Return: 1 if valid
 * 	   0 if not
 */
static inline int valid_qid(int qid)
{
	return ( (qid>0) && (qid<AMAZON_ATM_MAX_QUEUE_NUM));
}

/*
 * Brief: align to 16 bytes boundary 
 * Parameter:
 *	skb
 * Description:
 *	use skb_reserve to adjust the data pointer
 *	don't change head pointer
 *	pls allocate extrac 16 bytes before call this function
 */
static void inline alloc_align_16(struct sk_buff * skb)
{
	if ( ( ((u32) (skb->data)) & 15) != 0){
		AMAZON_TPE_DMSG("need to adjust the alignment manually\n");
		skb_reserve(skb, 16 - (((u32) (skb->data)) & 15) );
	}

}

/*
 * Brief: initialize the device according to the module paramters
 * Return: not NULL	-	ok
 *	   NULL		-	fails
 * Description: arrange load parameters and call the hardware initialization routines
 */
static void atm_init_parameters(amazon_atm_dev_t *dev)
{
	//port setting
	dev->ports[0].enable = port_enable0;
	dev->ports[0].max_conn = port_max_conn0;
	dev->ports[0].tx_max_cr = port_cell_rate_up0;
	if (port_enable1){
		dev->ports[1].enable = port_enable1;
		dev->ports[1].max_conn = port_max_conn1;
		dev->ports[1].tx_max_cr = port_cell_rate_up1;
	}

	//aal5
	dev->aal5.padding_byte = a5_fill_pattern;
	dev->aal5.tx_max_sdu = a5s_mtu;
	dev->aal5.rx_max_sdu = a5r_mtu;

	//cbm
	dev->cbm.nrt_thr = cbm_nrt;
	dev->cbm.clp0_thr = cbm_clp0;
	dev->cbm.clp1_thr = cbm_clp1;
	dev->cbm.free_cell_cnt = cbm_free_cell_no;

	//qsb
	dev->qsb.tau = qsb_tau;
	dev->qsb.tstepc =qsb_tstep;
	dev->qsb.sbl = qsb_srvm;
	
	//allocate on the fly
	dev->cbm.mem_addr = NULL;
	dev->cbm.qd_addr = NULL;
}


/*	Brief: 		Find QID for VCC
 *	Parameters:	vcc	- VCC data structure
 *  	Return Value:	-EINVAL		- VCC not found
 *			qid		- QID for this VCC
 *	Description:
 *	This function returns the QID of a given VCC
 */
static int amazon_atm_get_queue(struct atm_vcc* vcc)
{
	int	i;
	for (i=0;i<AMAZON_ATM_MAX_QUEUE_NUM;i++) {
		if (g_atm_dev.queues[i].vcc == vcc) return i;
	}
	return -EINVAL;
}


/*
 *	Brief: 		Find QID for VPI/VCI
 *	Parameters:	vpi	- VPI to found
 *			vci	- VCI to found
 *
 *  	Return Value:	-EINVAL		- VPI/VCI not found
 *			qid		- QID for this VPI/VCI
 *
 *	Description:
 *	This function returns the QID for a given VPI/VCI. itf doesn't matter
 */
static int amazon_atm_find_vpivci(u8 vpi, u16 vci)
{
	int	i;
	struct atm_vcc * vcc;
	for (i=0;i<AMAZON_ATM_MAX_QUEUE_NUM;i++) {
		if ( (vcc = g_atm_dev.queues[i].vcc)!= NULL) {
			if ((vcc->vpi == vpi) && (vcc->vci == vci)) return i;
		}
	}
	return -EINVAL;
}

/*	Brief: 		Find QID for VPI
 *	Parameters:	vpi	- VPI to found
 *  	Return Value:	-EINVAL		- VPI not found
 *			qid		- QID for this VPI
 *
 *	Description:
 *	This function returns the QID for a given VPI. itf and VCI don't matter
 */
static int amazon_atm_find_vpi(u8 vpi)
{
	int	i;
	for (i=0;i<AMAZON_ATM_MAX_QUEUE_NUM;i++) {
		if ( g_atm_dev.queues[i].vcc!= NULL) {
			if (g_atm_dev.queues[i].vcc->vpi == vpi) return i;
		}
	}
	return -EINVAL;
}

/*
 *	Brief: 		Clears QID entries for VCC
 *
 *	Parameters:	vcc	- VCC to found
 *
 *	Description:
 *	This function searches for the given VCC and sets it to NULL if found.
 */
static inline void amazon_atm_clear_vcc(int i)
{
	g_atm_dev.queues[i].vcc = NULL;
	g_atm_dev.queues[i].free = 1;
}


/*
 * Brief:	dump skb data
 */
static inline void dump_skb(u32 len, char * data)
{
#ifdef AMAZON_TPE_DUMP
	int i;
	for(i=0;i<len;i++){	
		printk("%2.2x ",(u8)(data[i]));
		if (i % 16 == 15)
			printk("\n");
	}
	printk("\n");
#endif
}

/*
 * Brief:	dump queue descriptor
 */
static inline void dump_qd(int qid)
{
#ifdef AMAZON_TPE_DUMP
	u8 * qd_addr;
	if (valid_qid(qid) != 1) return;
	qd_addr = (u8 *) KSEG1ADDR((unsigned long)g_atm_dev.cbm.qd_addr);
	AMAZON_TPE_EMSG("qid: %u [%8x][%8x][%8x][%8x]\n", qid
		,readl(qd_addr+qid*CBM_QD_SIZE+0x0)
		,readl(qd_addr+qid*CBM_QD_SIZE+0x4)
		,readl(qd_addr+qid*CBM_QD_SIZE+0x8)
		,readl(qd_addr+qid*CBM_QD_SIZE+0xc));
#endif
}

/*
 * Brief:	release TX skbuff
 */
static inline void amazon_atm_free_tx_skb_vcc(struct atm_vcc *vcc, struct sk_buff *skb)
{
	if ( vcc->pop != NULL) {
		vcc->pop(vcc, skb);
	 } else {
	 	dev_kfree_skb_any(skb);
        }
}
/*
 * Brief:	release TX skbuff
 */
static inline void amazon_atm_free_tx_skb(struct sk_buff *skb)
{
	struct atm_vcc* vcc = ATM_SKB(skb)->vcc;
	if (vcc!=NULL){
		amazon_atm_free_tx_skb_vcc(vcc,skb);
	} else {
		dev_kfree_skb_any(skb);//fchang:Added
	}
}

/* Brief:	divide by 64 and round up
 */
static inline u32 divide_by_64_round_up(int input)
{
	u32 tmp1;
	tmp1 = (u32) input;
        tmp1 = (tmp1%64)?(tmp1/64 + 1): (tmp1/64);
        if (tmp1 == 0) tmp1 = 1;
	return tmp1;
}

/*
 * Brief:	statistics
 */
#ifdef AMAZON_ATM_DEBUG
static inline void queue_statics(int qid, qs_t idx)
{
	if (valid_qid(qid)){
		g_atm_dev.queues[qid].qs[idx]++;
	}
}
#else	//not AMAZON_ATM_DEBUG
static inline void queue_statics(int qid, qs_t idx){}
#endif	//AMAZON_ATM_DEBUG		


/*	Brief:	set dma tx full, i.e. there is no available descriptors
 */
static void inline atm_dma_full(void)
{
	AMAZON_TPE_DMSG("ch0 is full\n");
	atomic_set(&g_atm_dev.dma_tx_free_0,0);
}

/*
 *	Brief	set dma tx free (at least one descript is available)
 */
inline static void atm_dma_free(void)
{
	AMAZON_TPE_DMSG("ch0 is free\n");
	atomic_set(&g_atm_dev.dma_tx_free_0,1);
}


/*	Brief:		return the status of DMA TX descriptors
 *	Parameters:	TX channel  (DMA_TX_CH0, TX_CH1)
 *	Return:
 *		1:	there are availabel TX descriptors
 *		0:	no available
 *	Description:
 *
 */
inline int dma_may_send(int ch)
{
	if (atomic_read(&g_atm_dev.dma_tx_free_0)){
		return 1;
	}
	return 0;
}

/******************************* global functions *********************************/ 
/*
 *	Brief: 		SWIE Cell Extraction Start Routine
 *			and task routine for swex_complete_task
 *	Parameters:	irq_stat	- interrupt status
 *
 *	Description:
 *	This is the routine for extracting cell. It will schedule itself if the hardware is busy.
 *	This routine runs in interrupt context
 */
void amazon_atm_swex(void * irq_stat)
{
	u32 ex_stat=0;
	u32 addr;
	// Read extraction status register
	ex_stat = readl(CBM_HWEXSTAT0_ADDR);

	// Check if extraction/insertion is in progress 
	if ( (ex_stat & CBM_EXSTAT_SCB) || (ex_stat & CBM_EXSTAT_FB) || (test_and_set_bit(SWIE_LOCK, &(g_atm_dev.swie.lock))!=0)) {
		AMAZON_TPE_DMSG(" extraction in progress. Will wait\n");
		swex_start_task.data = irq_stat;
		queue_task(&swex_start_task, &tq_immediate);
		mark_bh(IMMEDIATE_BH);
	}else {
		// Extract QID
		g_atm_dev.swie.qid = (((u32)irq_stat) >> 24);
		AMAZON_TPE_DMSG("extracting from qid=%u\n",g_atm_dev.swie.qid);
		//read status word
		addr = KSEG1ADDR((unsigned long)g_atm_dev.cbm.qd_addr);
		addr = readl((addr + g_atm_dev.swie.qid * 0x10 + 4) & 0xFFFFFFC0);
		addr = KSEG1ADDR(addr);
		g_atm_dev.swie.sw = readl(addr+52)&SWIE_ADDITION_DATA_MASK;
		AMAZON_TPE_DMSG("cell addition word: %8x \n", g_atm_dev.swie.sw);
		
		// Start extraction
		AMAZON_WRITE_REGISTER_L(g_atm_dev.swie.qid | SWIE_CBM_PID_SUBADDR, CBM_HWEXPAR0_ADDR);
		AMAZON_WRITE_REGISTER_L(SWIE_CBM_SCE0, CBM_HWEXCMD_ADDR);
	}
}
#ifdef AMAZON_TPE_SCR
u32 g_a5r_wait=0;
/*
 *	Brief: 		AAL5 Packet Extraction Routine	and task routine for a5r_task
 *	Parameters:	irq_stat	- interrupt status
 *
 *	Description:
 *	This is the routine for extracting frame. It will schedule itself if the hardware is busy.
 *	This routine runs in interrupt context
 */
void amazon_atm_a5r(void* qid)
{
	volatile u32 ex_stat=0;
	u32 addr;
	u32 a5r_wait=0;

	ex_stat = readl(CBM_HWEXSTAT0_ADDR);
#if 0
	// Check if extraction/insertion is in progress 
	if ( (ex_stat & CBM_EXSTAT_SCB) || (ex_stat & CBM_EXSTAT_FB) ) {
		AMAZON_TPE_DMSG(" extraction in progress. Will wait\n");
		a5r_task.data = qid;
		queue_task(&a5r_task, &tq_immediate);
		mark_bh(IMMEDIATE_BH);
	}else {
		AMAZON_TPE_DMSG("extracting from qid=%u\n",(u8)qid);
		// Start extraction
		AMAZON_WRITE_REGISTER_L(((u8)qid) | CBM_HWEXPAR_PN_A5, CBM_HWEXPAR0_ADDR);
		AMAZON_WRITE_REGISTER_L(CBM_HWEXCMD_FE0, CBM_HWEXCMD_ADDR);
	}
#else
	//while ( (ex_stat & CBM_EXSTAT_SCB) || (ex_stat & CBM_EXSTAT_FB) ) {
	while ( ex_stat != 0x80){
		a5r_wait++;
		ex_stat = readl(CBM_HWEXSTAT0_ADDR);
#if	0
		if (a5r_wait >= 0xffffff){
			a5r_wait=0;
			printk(".");
		}
#endif
	}
	if (a5r_wait > g_a5r_wait){
		g_a5r_wait = a5r_wait;
	}
	AMAZON_WRITE_REGISTER_L(((u8)qid) | CBM_HWEXPAR_PN_A5, CBM_HWEXPAR0_ADDR);
	AMAZON_WRITE_REGISTER_L(CBM_HWEXCMD_FE0, CBM_HWEXCMD_ADDR);
#endif
}

#endif //AMAZON_TPE_SCR

/*	Brief:	Handle F4/F5 OAM cell
 *	Return:
 *		0	ok
 *		<0	fails
 */
static int inline amazon_handle_oam_cell(void *data, u8 vpi, u16 vci,u32 status)
{
	struct atm_vcc*		vcc=NULL;
	int qid;
	if (!status&SWIE_EOAM_MASK){
		AMAZON_TPE_EMSG("unknown cell received, discarded\n");
		goto amazon_handle_oam_cell_err_exit;
	}else if (status&SWIE_ECRC10ERROR_MASK){
		AMAZON_TPE_EMSG("CRC-10 Error Status:%8x, discarded\n", status);
		goto amazon_handle_oam_cell_err_exit;
	}else{
		if(status & (SWIE_EVCI3_MASK |SWIE_EVCI4_MASK)){
			//F4 level (VPI) OAM, Assume duplex
			qid = amazon_atm_find_vpi(vpi)+CBM_RX_OFFSET;
		}else if (status & (SWIE_EPTI4_MASK|SWIE_EPTI5_MASK)){
			//F5 level (VCI) OAM, Assume duplex
			qid = amazon_atm_find_vpivci(vpi,vci)+CBM_RX_OFFSET;			
		}else{
			qid = -1;
			AMAZON_TPE_EMSG("non-F4/F5 OAM cells?, discarded\n");
			goto amazon_handle_oam_cell_err_exit;
		}
	}
	if (valid_qid(qid) && ((vcc = g_atm_dev.queues[qid].vcc)!=NULL)){
		//TODO, should we do this for ALL OAM types? (Actually only User and CC)
		g_atm_dev.queues[qid].access_time=xtime;
		if (vcc->push_oam){
			(*vcc->push_oam)(vcc,data);
		}else{
			amz_push_oam(data);
		}
	}else{
		AMAZON_TPE_EMSG("no VCC yet\n");
		goto amazon_handle_oam_cell_err_exit;
	}
	return 0;
amazon_handle_oam_cell_err_exit:	
	dump_skb(AMAZON_AAL0_SDU,(char *)data);	
	return -1;
}

/*	Brief:	SWIE Cell Extraction Finish Routine 
 *		and task routine for swex_complete_task
 *	Description:
 *	1.Allocate a buffer of type struct sk_buff
 *	2.Copy the data from the temporary memory to this buffer
 *	3.Push the data to upper layer
 * 	4.Update the statistical data if necessary
 *	5.Release the temporary data

 */
void amazon_atm_swex_push(void * data)
{
	struct atm_vcc*		vcc=NULL;
	struct sk_buff*		skb=NULL;
	struct amazon_atm_cell_header * cell_header;
	u32 status;
	int qid;
	if (!data){
		AMAZON_TPE_EMSG("data is NULL\n");
		return;
	}
	qid = ((u8*)data)[AMAZON_AAL0_SDU];
	status = ((u32*)data)[ATM_AAL0_SDU/4];
	cell_header = (struct amazon_atm_cell_header *) data;
	if (valid_qid(qid) != 1){
		AMAZON_TPE_EMSG("error qid: %u\n",qid);
		AMAZON_TPE_EMSG("unknown cells recieved\n");
	}else if (qid == AMAZON_ATM_OAM_Q_ID){
		//OAM or RM or OTHER cell
		//Find real connection
		
#ifdef IKOS_MINI_BOOT	
		//for OAM loop back test
		dump_skb(56,(char *)data);
		//kfree(data);	using g_oam_cell
		return;
#endif //IKOS_MINI_BOOT				
#ifdef TPE_LOOPBACK					
		amz_push_oam(data);
		return;
#endif//TPE_LOOPBACK
		int ret = 0;
		ret = amazon_handle_oam_cell(data,cell_header->bit.vpi,cell_header->bit.vci,status);
		if (ret == 0)
					return;
				}else{
		//should be normal AAL0 cells
		// Get VCC
		vcc = g_atm_dev.queues[qid].vcc;
		if (vcc != NULL) {
			AMAZON_TPE_DMSG("push to upper layer\n");
			skb = dev_alloc_skb(AMAZON_AAL0_SDU);
			if (skb != NULL) {
				//skb->dev=vcc->dev;
				memcpy(skb_put(skb, AMAZON_AAL0_SDU), data, AMAZON_AAL0_SDU);
				skb->stamp = xtime;
				ATM_SKB(skb)->vcc = vcc;
				(*g_atm_dev.queues[qid].push)(vcc,skb,0);
			}else{
				AMAZON_TPE_EMSG(" No memory left for incoming AAL0 cell! Cell discarded!\n");
				//inform the upper layer
				(*g_atm_dev.queues[qid].push)(vcc,skb,-ENOMEM);
				atomic_inc(&vcc->stats->rx_drop);
			}
		}else{
			AMAZON_TPE_EMSG("invalid qid %u\n",qid);
		}
	}
	//kfree(data);	using g_oam_cell
}

/*
 *	Brief: 		Interrupt handler for software cell extraction (done)
 *	Parameters:	irq	- CPPN for this interrupt
 *			data	- Device ID for this interrupt
 *			regs	- Register file
 *
 *	Description:
 *	When a software extraction is finished this interrupt is issued.
 *	It reads the cell data and sends it to the ATM stack.
 */
void amazon_atm_swex_isr(int irq, void *data, struct pt_regs *regs)
{
	u32 * cell = NULL;
	int i;
	//ATM_AAL0 SDU + QID
	AMAZON_TPE_DMSG("SWIE extraction done\n");
	cell = (u32 *) g_oam_cell;
	if (cell != NULL){
		//convert to host byte order from big endian
		for(i=0;i<ATM_AAL0_SDU;i+=4){
			cell[i/4]=readl(SWIE_ECELL_ADDR+i);	
		}
		cell[ATM_AAL0_SDU/4]= g_atm_dev.swie.sw;
		((u8*)cell)[AMAZON_AAL0_SDU] = g_atm_dev.swie.qid;
#ifdef IKOS_MINI_BOOT
	for(i=0;i<ATM_AAL0_SDU;i+=4){
		AMAZON_TPE_DMSG("[%2x][%2x][%2x][%2x]\n",
			((char*)cell)[i],
			((char*)cell)[i+1],
			((char*)cell)[i+2],
			((char*)cell)[i+3]
			);
	}
	AMAZON_TPE_DMSG("qid: %u\n", ((u8*)cell)[AMAZON_AAL0_SDU]);
	amazon_atm_swex_push((void *) cell);
#else //not IKOS_MINI_BOOT
	swex_complete_task.data = cell;
	queue_task(&swex_complete_task,&tq_immediate);
	mark_bh(IMMEDIATE_BH);
#endif //not IKOS_MINI_BOOT
	}else{
		AMAZON_TPE_EMSG("no memory for receiving AAL0 cell\n");
	}
	
	/* release the lock and check */
	if (test_and_clear_bit(SWIE_LOCK,&(g_atm_dev.swie.lock)) == 0){
		AMAZON_TPE_EMSG("swie lock is already released\n");
	}
	wake_up(&g_atm_dev.swie.sleep);
}
/*	Brief: 		Interrupt handler for software cell insertion
 *
 *	Parameters:	irq	- CPPN for this interrupt
 *			data	- Device ID for this interrupt
 *			regs	- Register file
 *
 *	Description:
 *	When a software insertion is finished this interrupt is issued.
 *	The only purpose is to release the semaphore and read the status register.
 */
void amazon_atm_swin_isr(int irq, void *data, struct pt_regs *regs)
{
	AMAZON_TPE_DMSG("SWIE insertion done\n");
	/* release the lock and check */
	if (test_and_clear_bit(SWIE_LOCK,&(g_atm_dev.swie.lock)) == 0){
		AMAZON_TPE_EMSG("swie lock is already released");
	}
	// Release semaphore
	up(&g_atm_dev.swie.in_sem);

}
/*	Brief: 		Interrupt handler for software cell insertion & extraction
 *	Parameters:	irq	- CPPN for this interrupt
 *			data	- Device ID for this interrupt
 *			regs	- Register file
 *	Description:
 *	When a software insertion or extractionis finished this interrupt is issued.
 */
void amazon_atm_swie_isr(int irq, void *data, struct pt_regs *regs)
{
	u32	status=0;
	// Read status register
	status = readl(SWIE_ISTAT_ADDR);
	AMAZON_TPE_DMSG("insertion status: %8x\n", status);
	if (status & SWIE_ISTAT_DONE){
		//clear interrupt in peripheral and ICU
		AMAZON_WRITE_REGISTER_L(SRC_TOS_MIPS | SRC_CLRR|SRC_SRE_ENABLE | AMAZON_SWIE_INT, SWIE_ISRC_ADDR);
		mask_and_ack_amazon_irq(AMAZON_SWIE_INT);
		
		amazon_atm_swin_isr(irq,data,regs);
	}
	status = readl(SWIE_ESTAT_ADDR);
	AMAZON_TPE_DMSG("extraction status: %8x\n", status);
	if (status & SWIE_ESTAT_DONE){
		//clear interrupt
		AMAZON_WRITE_REGISTER_L(SRC_TOS_MIPS | SRC_CLRR|SRC_SRE_ENABLE | AMAZON_SWIE_INT, SWIE_ESRC_ADDR);
		mask_and_ack_amazon_irq(AMAZON_SWIE_INT);
		
		amazon_atm_swex_isr(irq,data,regs);
	}
	//clear interrupt in ICU
}
 
/*
 *	Brief: 		Insert ATM cell into CBM
 *	Parameters:	queue	- Target queue
 *			cell	- Pointer to cell data
 *  	Return Value:	EBUSY		- CBM is busy
 *			0		- OK, cell inserted
 *	Description:
 *	This function inserts a cell into the CBM using the software insertion
 *	method. The format of the cell should be
 *	Little Endian (address starting from 0)
 *		H3, H2, H1, H0, P3, P2, P1, P0, P7, P6, P5, P4, ..., P47, P46, P45, P44
 *	Big Endian (address starting from 0)
 *		H0, H1, H2, H3, P0, P1, P2, P3, P4, P5, P6, P7, ..., P44, P45, P46, P47
 *	This function does not free memory!!!
 */
int amazon_atm_swin(u8 queue, void* cell)
{
	u32	status=0;
	int i;
	// Read status register
	status = readl(SWIE_ISTAT_ADDR);
	AMAZON_TPE_DMSG(" SWIE status=0x%08x\n",status);

	AMAZON_TPE_DMSG(" Inserting cell qid=%u\n",queue);
	
#ifdef AMAZON_CHECK_LINK
	if (adsl_link_status == 0){
		return -EFAULT;	
	}
#endif //AMAZON_CHECK_LINK

	// Get semaphore (if possible)
	if (down_interruptible(&g_atm_dev.swie.in_sem)) {
		return -ERESTARTSYS;
	}
	/* try to set lock */
	wait_event_interruptible(g_atm_dev.swie.sleep,(test_and_set_bit(SWIE_LOCK,&(g_atm_dev.swie.lock)) == 0));
	if (signal_pending(current)){
		return -ERESTARTSYS; 
	}

	// Store cell in CBM memory
	for(i=0;i<ATM_AAL0_SDU;i+=4){
		AMAZON_WRITE_REGISTER_L(((u32*)cell)[i/4],SWIE_ICELL_ADDR+i);
	}
	//Store queue id
	AMAZON_WRITE_REGISTER_L((u32) queue,SWIE_IQID_ADDR);

	//Start SWIE
	AMAZON_WRITE_REGISTER_L(SWIE_ICMD_START,SWIE_ICMD_ADDR);
	
	return 0;
}

#ifdef AMAZON_ATM_DEBUG
/*
 *	Brief: 		Interrupt handler for HTU
 *
 *	Parameters:	irq	- CPPN for this interrupt
 *			data	- Device ID for this interrupt
 *			regs	- Register file
 *
 */
void amazon_atm_htu_isr(int irq, void *data, struct pt_regs *regs)
{
	u32	irq_stat=0;

	// Read interrupt status register
	irq_stat = readl(HTU_ISR0_ADDR);
	AMAZON_TPE_DMSG("HTU status: %8x\n",irq_stat);
	//Clear interrupt in CBM and ICU
	AMAZON_WRITE_REGISTER_L(SRC_CLRR|SRC_TOS_MIPS | SRC_SRE_ENABLE | AMAZON_HTU_INT, HTU_SRC0_ADDR);
	mask_and_ack_amazon_irq(AMAZON_HTU_INT);	
	// Check if Any Cell Arrived
	if (irq_stat & (HTU_ISR_NE | HTU_ISR_PNE) ) {
		AMAZON_TPE_EMSG("INFNOENTRY %8x\n", readl(HTU_INFNOENTRY_ADDR));		
	}else if (irq_stat & (HTU_ISR_TORD|HTU_ISR_PT)){
		AMAZON_TPE_EMSG("Time Out %8x\n", readl(HTU_INFTIMEOUT_ADDR));
	}else if (irq_stat & HTU_ISR_IT){
		AMAZON_TPE_EMSG("Interrupt Test\n");
	}else if (irq_stat & HTU_ISR_OTOC){
		AMAZON_TPE_EMSG("Overflow of Time Out Counter\n");
	}else if (irq_stat & HTU_ISR_ONEC){
		AMAZON_TPE_EMSG("Overflow of No Entry Counter\n");
	}else{
		AMAZON_TPE_EMSG("unknown HTU interrupt occurs %8x\n", irq_stat);
	}

}
#endif //AMAZON_ATM_DEBUG

#ifdef AMAZON_TPE_TEST_AAL5_INT
/*
 *	Brief: 		Interrupt handler for AAL5
 *
 *	Parameters:	irq	- CPPN for this interrupt
 *			data	- Device ID for this interrupt
 *			regs	- Register file
 *
 */
void amazon_atm_aal5_isr(int irq, void *data, struct pt_regs *regs)
{
	volatile u32 irq_stat=0;

	// Read interrupt status register
	irq_stat = readl(AAL5_SISR0_ADDR);
	if (irq_stat){
		AMAZON_TPE_EMSG("A5S status: %8x\n",irq_stat);
		//Clear interrupt in CBM and ICU
		AMAZON_WRITE_REGISTER_L(SRC_CLRR|SRC_TOS_MIPS | SRC_SRE_ENABLE | AMAZON_AAL5_INT, AAL5_SSRC0_ADDR);
		mask_and_ack_amazon_irq(AMAZON_AAL5_INT);	
	}
	irq_stat = readl(AAL5_RISR0_ADDR);
	if (irq_stat){
		AMAZON_TPE_EMSG("A5R status: %8x\n",irq_stat);
		//Clear interrupt in CBM and ICU
		AMAZON_WRITE_REGISTER_L(SRC_CLRR|SRC_TOS_MIPS | SRC_SRE_ENABLE | AMAZON_AAL5_INT, AAL5_RSRC0_ADDR);
		mask_and_ack_amazon_irq(AMAZON_AAL5_INT);	
	}
}
#endif //AMAZON_TPE_TEST_AAL5_INT

/*
 *	Brief: 		Interrupt handler for CBM
 *
 *	Parameters:	irq	- CPPN for this interrupt
 *			data	- Device ID for this interrupt
 *			regs	- Register file
 *
 *	Description:
 *	This is the MIPS interrupt handler for the CBM. It processes incoming cells
 *	for SWIE queues.
 */
void amazon_atm_cbm_isr(int irq, void *data, struct pt_regs *regs)
{
	u32	irq_stat=0;
	u8 qid=0;

	// Read interrupt status register
	while ( (irq_stat = readl(CBM_INTINF0_ADDR))){
		AMAZON_TPE_DMSG("CBM INT status: %8x\n",irq_stat);
		//Clear interrupt in CBM and ICU
		AMAZON_WRITE_REGISTER_L(SRC_CLRR|SRC_TOS_MIPS | SRC_SRE_ENABLE | AMAZON_CBM_INT, CBM_SRC0_ADDR);
		qid = (u8) ((irq_stat & CBM_INTINF0_QID_MASK)>>CBM_INTINF0_QID_SHIFT);
#ifdef AMAZON_TPE_SCR	
		if (irq_stat & CBM_INTINF0_EF){
			amazon_atm_a5r((void*)qid);
		}
#endif
		// Check if Any Cell Arrived
		if (irq_stat & CBM_INTINF0_ACA) {
			amazon_atm_swex((void *)irq_stat);		
		}
		//TX AAL5 PDU discard
		if (irq_stat & CBM_INTINF0_OPF){
 			if ( (qid) < CBM_RX_OFFSET ){
 				g_atm_dev.mib_counter.tx_drop++;
			}
			queue_statics(qid, QS_HW_DROP);
		}
		if (irq_stat & (CBM_INTINF0_ERR|CBM_INTINF0_Q0E|CBM_INTINF0_Q0I|CBM_INTINF0_RDE)){
			AMAZON_TPE_EMSG("CBM INT status: %8x\n",irq_stat);
			if (irq_stat & CBM_INTINF0_ERR){
				AMAZON_TPE_EMSG("CBM Error: FPI Bus Error\n");
			}
			if (irq_stat & CBM_INTINF0_Q0E){
				AMAZON_TPE_EMSG("CBM Error: Queue 0 Extract\n");
			}
			if (irq_stat & CBM_INTINF0_Q0I){
				AMAZON_TPE_EMSG("CBM Error: Queue 0 Extract\n");
			}
			if (irq_stat & CBM_INTINF0_RDE){
				AMAZON_TPE_EMSG("CBM Error: Read Empty Queue %u\n",qid);
				dump_qd(qid);
			}
		}		
	}
	mask_and_ack_amazon_irq(AMAZON_CBM_INT);
}

/*	Brief:	check the status word after AAL SDU after reassembly
 */
static inline void check_aal5_error(u8 stw0, u8 stw1, int qid)
{
	if (stw0 & AAL5_STW0_MFL){
		AMAZON_TPE_DMSG("Maximum Frame Length\n");
		g_atm_dev.queues[qid].aal5VccOverSizedSDUs++;
	} 
	if (stw0 & AAL5_STW0_CRC){
		AMAZON_TPE_DMSG("CRC\n");
		g_atm_dev.queues[qid].aal5VccCrcErrors++;
	}
#ifdef AMAZON_ATM_DEBUG_RX
	AMAZON_TPE_EMSG("qid:%u stw0:%8x stw1:%8x\n",qid,stw0,stw1);
#endif
}

/* Brief: 	Process DMA rx data
 * Parameters:
 	dma_dev:	pointer to the dma_device_info, provided by us when register the dma device
 * Return: 	no
 * Description: DMA interrupt handerl with OoS support. It is called when there is some data in rx direction.
 *
 */
//507261:tc.chen void atm_process_dma_rx(struct dma_device_info* dma_dev)
void __system atm_process_dma_rx(struct dma_device_info* dma_dev)
{
        u8 * head=NULL;
        u32 length=0;
        u8 stw0=0;
        u8 stw1=0;

	struct sk_buff * skb=NULL;
	struct atm_vcc * vcc=NULL;
	int qid=0;
#ifdef AMAZON_ATM_DEBUG_RX
	static int dma_rx_dump=0;
	static u32 seq=0;
	
	seq++;
	if (dma_rx_dump>0){
		printk("\n=========================[%u]=========================\n",seq);
	}
#endif	
	length=dma_device_read(dma_dev,&head,(void**)&skb);
	AMAZON_TPE_DMSG("receive %8p[%u] from DMA\n", head,length);
	if (head == NULL||length<=0) {
		AMAZON_TPE_DMSG("dma_read null \n");
		goto error_exit;
	}

	if (length > (g_atm_dev.aal5.rx_max_sdu+64)){
		AMAZON_TPE_EMSG("received packet too large (%u)\n",length);
		goto error_exit;
	}
	//check AAL5R trail for error and qid
	//last byte is qid
	length--;
	qid = (int) head[length];
	AMAZON_TPE_DMSG("head[%u] qid %u\n",length, qid);
	//STW0 is always 4 bytes before qid
	length -= 4;
	stw0 = head[length]&0xff;
	AMAZON_TPE_DMSG("head[%u] stw0 %8x\n",length, stw0);
	//position of STW1 depends on the BE bits
	length = length-4 + (stw0&AAL5_STW0_BE);
	stw1 = head[length]&0xff;
	AMAZON_TPE_DMSG("head[%u] stw1 %8x\n",length, stw1);	
	if ( (stw0 & AAL5_STW0_MASK) || (stw1 & AAL5_STW1_MASK) ){
		//AAL5 Error
		check_aal5_error(stw0, stw1,qid);		
		goto error_exit;
	}
	//make data pointers consistent
	//UU + CPI
	length -= 2;
	AMAZON_TPE_DMSG("packet length %u\n", length);

	//error: cannot restore the qid
	if (valid_qid(qid) != 1){
		AMAZON_TPE_EMSG("received frame in invalid qid %u!\n", qid);
		goto error_exit;
	}
	vcc = g_atm_dev.queues[qid].vcc;
	if (vcc == NULL){
		AMAZON_TPE_EMSG("received frame in invalid vcc, qid=%u!\n",qid);
		goto error_exit;
	}
	if (skb == NULL){
		AMAZON_TPE_EMSG("cannot restore skb pointer!\n");
		goto error_exit;
	}
	skb_put(skb,length);
	skb->stamp = xtime;
	g_atm_dev.queues[qid].access_time=xtime;
	if ((*g_atm_dev.queues[qid].push)(vcc,skb,0)){
		g_atm_dev.mib_counter.rx_drop++;
		queue_statics(qid, QS_SW_DROP);					
	}else{
		g_atm_dev.mib_counter.rx++;
		adsl_led_flash();//joelin adsl led
		queue_statics(qid, QS_PKT);
		AMAZON_TPE_DMSG("push successful!\n");
	}
#ifdef AMAZON_ATM_DEBUG_RX
	if (dma_rx_dump>0){
		printk("\nOK packet [dump=%u] length=%u\n",dma_rx_dump,length);
		dump_skb(length+7, head);
	}
	if (dma_rx_dump >0) dma_rx_dump--;
#endif	
	return ;
error_exit:
#ifdef AMAZON_ATM_DEBUG_RX
	if ( (head!=NULL) && (length >0)){
		AMAZON_TPE_EMSG("length=%u\n",length);
		dump_skb(length+5, head);
	}	
	dma_rx_dump++;
#endif	
	g_atm_dev.mib_counter.rx_err++;
	queue_statics(qid, QS_ERR);
	/*
	if (vcc){
		(*g_atm_dev.queues[qid].push)(vcc,skb,1);	
	}
	*/
	if (skb != NULL) {
		dev_kfree_skb_any(skb);
	}
	return;
}

/*Brief:	ISR for DMA pseudo interrupt
 *Parameter:
 	dma_dev:	pointer to the dma_device_info, provided by us when register the dma device
 	intr_status:	
 		RCV_INT:		rx data available
 		TX_BUF_FULL_INT:	tx descriptor run out of
 		TRANSMIT_CPT_INT:	tx descriptor available again
 *Return:
 	0 for success???
 */
//507261:tc.chen int amazon_atm_dma_handler(struct dma_device_info* dma_dev, int intr_status)
int __system amazon_atm_dma_handler(struct dma_device_info* dma_dev, int intr_status)
{
	AMAZON_TPE_DMSG("status:%u\n",intr_status);
	switch (intr_status) {
		case RCV_INT:
			atm_process_dma_rx(dma_dev);
			break;
		case TX_BUF_FULL_INT:
			//TX full: no descriptors
			atm_dma_full();
			break;
		case TRANSMIT_CPT_INT:
			//TX free: at least one descriptor
			atm_dma_free();
			break;
		default:
			AMAZON_TPE_EMSG("unknown status!\n");
	}
	return 0;
}

/*Brief:	free buffer for DMA tx
 *Parameter:
 	dataptr:	pointers to data buffer
 	opt:		optional parameter, used to convey struct skb pointer, passwd in dma_device_write
 *Return:
 	0 for success???
 *Description:
 	called by DMA module to release data buffer after DMA tx transaction
 *Error:
 	cannot restore skb pointer
 */
int amazon_atm_free_tx(u8*dataptr, void* opt)
{
	struct sk_buff *skb;
	if (opt){
		AMAZON_TPE_DMSG("free skb%8p\n",opt);
		skb = (struct sk_buff *)opt;
		amazon_atm_free_tx_skb(skb);
	}else{
		AMAZON_TPE_EMSG("BUG: cannot restore skb pointer!\n");
	}
	return 0;
}

/*Brief:	allocate buffer & do alignment
 */
inline struct sk_buff * amazon_atm_alloc_buffer(int len)
{
	struct sk_buff *skb;
	skb = dev_alloc_skb(len+16);
	if (skb){
		//alignment requriements (4x32 bits (16 bytes) boundary)
		alloc_align_16(skb);
	}
	return skb;
}

/*Brief:	allocate buffer for DMA rx
 *Parameter:
 	len: length
 	opt: optional data to convey the skb pointer, which will be returned to me in interrupt handler,
 *Return:
 	pointer to buffer, NULL means error?
 *Description:
 	must make sure byte alignment
 */
 	
u8* amazon_atm_alloc_rx(int len, int* offset, void **opt)
{
	struct sk_buff *skb;
	*offset = 0;
	skb = amazon_atm_alloc_buffer(len);
	if (skb){
		AMAZON_TPE_DMSG("alloc skb->data:%8p len:%u\n",skb->data,len);
		*(struct sk_buff**)opt = skb;
	}else{
		AMAZON_TPE_DMSG("no memory for receiving atm frame!\n");
		return NULL;
	}
	return skb->data;
}




/* Brief:	Allocate kernel memory for sending a datagram.
 * Parameters
 *	vcc	virtual connection
 *	size	data buffer size
 * Return:
 *	NULL	fail
 *	sk_buff	a pointer to a sk_buff
 * Description:
 *  This function can allocate our own additional memory for AAL5S inbound
 * header (8bytes). We have to replace the protocol default one (alloc_tx in /net/atm/common.c) 
 * when we open the device.
 * byte alignment is done is DMA driver.
 */
struct sk_buff *amazon_atm_alloc_tx(struct atm_vcc *vcc,unsigned int size)
{
	struct sk_buff *skb;

	if (!dma_may_send(DMA_TX_CH0)){
		AMAZON_TPE_EMSG("no DMA descriptor available!\n");
		return NULL;
	}
	//AAL5 inbound header space + alignment extra buffer
	size+=8+AAL5S_INBOUND_HEADER;

        if (atomic_read(&vcc->tx_inuse) && !atm_may_send(vcc,size)) {
                AMAZON_TPE_EMSG("Sorry tx_inuse = %u, size = %u, sndbuf = %u\n",
                    atomic_read(&vcc->tx_inuse),size,vcc->sk->sndbuf);
                return NULL;
        }

        skb = amazon_atm_alloc_buffer(size);
	if (skb == NULL){
		AMAZON_TPE_EMSG("no memory\n");
		return NULL;
	}
	AMAZON_TPE_DMSG("dev_alloc_skb(%u) = %x\n", skb->len, (u32)skb);
        AMAZON_TPE_DMSG("tx_inuse %u += %u\n",atomic_read(&vcc->tx_inuse),skb->truesize);
        atomic_add(skb->truesize+ATM_PDU_OVHD,&vcc->tx_inuse);

	//reserve for AAL5 inbound header
	skb_reserve(skb,AAL5S_INBOUND_HEADER);
        return skb;
}


/* Brief:	change per queue QSB setting according to vcc qos parameters
 * Paramters:
 *	vcc:	atm_vcc pointer 
 *	qid:	CBM queue id (1~15)
 * Return:
  */
static inline void set_qsb(struct atm_vcc *vcc, struct atm_qos *qos, int qid)
{
	qsb_qptl_t	qptl;
	qsb_qvpt_t	qvpt;
	u32 tmp=0;
	unsigned int qsb_clk;
	
	qsb_clk = amazon_get_fpi_hz()>>1;
        
	AMAZON_TPE_EMSG("Class=%u MAX_PCR=%u PCR=%u MIN_PCR=%u SCR=%u MBS=%u CDV=%u\n"
		,qos->txtp.traffic_class
		,qos->txtp.max_pcr
		,qos->txtp.pcr
		,qos->txtp.min_pcr
		,qos->txtp.scr
		,qos->txtp.mbs
		,qos->txtp.cdv
		);
	
	// PCR limiter
	if (qos->txtp.max_pcr == 0){	
		qptl.bit.tprs = 0;                  /* 0 disables the PCR limiter */
	}else {
		// peak cell rate will be slightly lower than requested	(maximum rate / pcr)= (qsbclock/2^3 * timestep/4)/pcr
		tmp = (( (qsb_clk * g_atm_dev.qsb.tstepc)>>5)/ qos->txtp.max_pcr ) + 1;
		// check if an overfow occured
		if (tmp > QSB_TP_TS_MAX) {
			AMAZON_TPE_EMSG("max_pcr is too small, max_pcr:%u tprs:%u\n",qos->txtp.max_pcr, tmp);
		  	qptl.bit.tprs = QSB_TP_TS_MAX;
		}else{
			qptl.bit.tprs = tmp;
		}
	}
	//WFQ
	if (qos->txtp.traffic_class == ATM_CBR  || qos->txtp.traffic_class ==ATM_VBR_RT){
		// real time queue gets weighted fair queueing bypass
		qptl.bit.twfq  = 0;
	}else if (qos->txtp.traffic_class ==ATM_VBR_NRT ||qos->txtp.traffic_class ==ATM_UBR_PLUS ){
		// wfq calculation here are based on virtual cell rates, to reduce granularity for large rates
		// wfq factor is maximum cell rate / garenteed cell rate.
		//qptl.bit.twfq = g_atm_dev.qsb.min_cr * QSB_WFQ_NONUBR_MAX / qos->txtp.min_pcr;
		if (qos->txtp.min_pcr == 0) {
			AMAZON_TPE_EMSG("<warning> MIN_PCR should not be zero\n");
			qptl.bit.twfq = QSB_WFQ_NONUBR_MAX;
		}else{
			tmp = QSB_GCR_MIN * QSB_WFQ_NONUBR_MAX / qos->txtp.min_pcr;		
			if (tmp == 0 ){
				qptl.bit.twfq = 1;
			}else if (tmp > QSB_WFQ_NONUBR_MAX){
				AMAZON_TPE_EMSG("min_pcr is too small, min_pcr:%u twfq:%u\n",qos->txtp.min_pcr, tmp);
				qptl.bit.twfq = QSB_WFQ_NONUBR_MAX;
			}else{
				qptl.bit.twfq = tmp;	
			}
		}
	}else if (qos->txtp.traffic_class == ATM_UBR){
		// ubr bypass, twfq set to maximum value
		qptl.bit.twfq = QSB_WFQ_UBR_BYPASS;
	}else{
		//tx is diabled, treated as UBR
		AMAZON_TPE_EMSG("<warning> unsupported traffic class %u \n", qos->txtp.traffic_class);
		qos->txtp.traffic_class = ATM_UBR;
		qptl.bit.twfq = QSB_WFQ_UBR_BYPASS;
	}
	
	//SCR Leaky Bucket Shaper VBR.0/VBR.1
	if (qos->txtp.traffic_class ==ATM_VBR_RT || qos->txtp.traffic_class ==ATM_VBR_NRT){
		if (qos->txtp.scr == 0){
			//SCR == 0 disable the shaper
			qvpt.bit.ts = 0;
			qvpt.bit.taus = 0;
		}else{
			//CLP
			if (vcc->atm_options&ATM_ATMOPT_CLP){
				//CLP1
				qptl.bit.vbr = 1;
			}else{
				//CLP0
				qptl.bit.vbr = 0;
			}
			//TS and TauS
			tmp = (( (qsb_clk * g_atm_dev.qsb.tstepc)>>5)/ qos->txtp.scr ) + 1;
			if (tmp > QSB_TP_TS_MAX) {
				AMAZON_TPE_EMSG("scr is too small, scr:%u ts:%u\n",qos->txtp.scr, tmp);
				qvpt.bit.ts = QSB_TP_TS_MAX;
			}else{
				qvpt.bit.ts = tmp;
			}
			tmp = (qos->txtp.mbs - 1)*(qvpt.bit.ts - qptl.bit.tprs)/64;
			if (tmp > QSB_TAUS_MAX){
				AMAZON_TPE_EMSG("mbs is too large, mbr:%u taus:%u\n",qos->txtp.mbs, tmp);
				qvpt.bit.taus = QSB_TAUS_MAX;
			}else if (tmp == 0){
				qvpt.bit.taus = 1;
			}else{
				qvpt.bit.taus = tmp;
			}
		}
	}else{
		qvpt.w0 = 0;
	}
	//write the QSB Queue Parameter Table (QPT)
	AMAZON_WRITE_REGISTER_L(QSB_QPT_SET_MASK,QSB_RTM_ADDR);
	AMAZON_WRITE_REGISTER_L(qptl.w0, QSB_RTD_ADDR);
	AMAZON_WRITE_REGISTER_L((QSB_TABLESEL_QPT<<QSB_TABLESEL_SHIFT)
		| QSB_RAMAC_REG_LOW
		| QSB_WRITE
		| qid
		,QSB_RAMAC_ADDR);
	//write the QSB Queue VBR Parameter Table (QVPT)
	AMAZON_WRITE_REGISTER_L(QSB_QVPT_SET_MASK,QSB_RTM_ADDR);
	AMAZON_WRITE_REGISTER_L(qvpt.w0, QSB_RTD_ADDR);
	AMAZON_WRITE_REGISTER_L((QSB_TABLESEL_QVPT<<QSB_TABLESEL_SHIFT)
		| QSB_RAMAC_REG_LOW
		| QSB_WRITE
		| qid
		,QSB_RAMAC_ADDR);	
	AMAZON_TPE_EMSG("tprs:%u twfq:%u ts:%u taus:%u\n",qptl.bit.tprs,qptl.bit.twfq,qvpt.bit.ts,qvpt.bit.taus);
}

/* 
 * Brief:	create/change CBM queue descriptor
 * Parameter:	
 *	vcc:	atm_vcc pointer 
 *	qid:	CBM queue id (1~15)
 */
static inline void set_qd(struct atm_vcc *vcc, u32 qid)
{
	u32 tx_config=0,rx_config=0;
	u32 itf = (u32) vcc->itf;
	u32 dma_qos=0;
	u8 * qd_addr=NULL;
		
	tx_config|=CBM_QD_W3_WM_EN|CBM_QD_W3_CLPt;
	//RT: check if the connection is a real time connection
	if (vcc->qos.txtp.traffic_class == ATM_CBR || vcc->qos.txtp.traffic_class == ATM_VBR_RT){
		tx_config|= CBM_QD_W3_RT;
	}else{
		tx_config|= CBM_QD_W3_AAL5; //don't set the AAL5 flag if it is a RT service
	}
	rx_config = tx_config;
	
	if(vcc->qos.aal == ATM_AAL5){	
		//QoS: DMA QoS according to the traffic class
		switch (vcc->qos.txtp.traffic_class){
			case ATM_CBR: dma_qos = CBR_DMA_QOS;break;
			case ATM_VBR_RT: dma_qos = VBR_RT_DMA_QOS;break;
			case ATM_VBR_NRT: dma_qos = VBR_NRT_DMA_QOS;break;
			case ATM_UBR_PLUS: dma_qos = UBR_PLUS_DMA_QOS;break;
			case ATM_UBR: dma_qos = UBR_DMA_QOS;break;
		}
	
		//TX: upstream, AAL5(EPD or PPD), NOINT, SBid
		tx_config |= CBM_QD_W3_DIR_UP|CBM_QD_W3_INT_NOINT|(itf&CBM_QD_W3_SBID_MASK);
		//RX: DMA QoS, downstream, no interrupt, AAL5(EPD, PPD), NO INT, HCR
#ifdef AMAZON_TPE_SCR
		rx_config |= dma_qos|CBM_QD_W3_DIR_DOWN|CBM_QD_W3_INT_EOF;
#else		
		rx_config |= dma_qos|CBM_QD_W3_DIR_DOWN|CBM_QD_W3_INT_NOINT|CBM_QD_W3_HCR;
#endif		
	}else {
		//should be AAL0	
		//upstream, NOINT, SBid
		tx_config |= CBM_QD_W3_DIR_UP|CBM_QD_W3_INT_NOINT|(itf&CBM_QD_W3_SBID_MASK);
		//RX: downstream, ACA interrupt, 
		rx_config |= CBM_QD_W3_DIR_DOWN|CBM_QD_W3_INT_ACA;
	}

	//Threshold: maximum threshold for tx/rx queue, which is adjustable in steps of 64 cells
	tx_config |=	( (divide_by_64_round_up(tx_q_threshold)&0xffff)<<CBM_QD_W3_THRESHOLD_SHIFT) & CBM_QD_W3_THRESHOLD_MASK;
	rx_config |=	( (divide_by_64_round_up(rx_q_threshold)&0xffff)<<CBM_QD_W3_THRESHOLD_SHIFT) & CBM_QD_W3_THRESHOLD_MASK;
	
	qd_addr = (u8*) KSEG1ADDR((unsigned long)g_atm_dev.cbm.qd_addr);
	//TX
	AMAZON_WRITE_REGISTER_L(tx_config, (qd_addr+qid*CBM_QD_SIZE + 0xc));
	AMAZON_WRITE_REGISTER_L(0, (qd_addr+qid*CBM_QD_SIZE + 0x8));
	//RX
	AMAZON_WRITE_REGISTER_L(rx_config, (qd_addr+(qid+CBM_RX_OFFSET)*CBM_QD_SIZE + 0xc));
	AMAZON_WRITE_REGISTER_L(0, (qd_addr+(qid+CBM_RX_OFFSET)*CBM_QD_SIZE + 0x8));
}
/*
 * Brief:	add HTU table entry
 * Parameter:	
 *	vpi.vci:
 *	qid:	CBM queue id (DEST is qid + CBM_RX_OFFSET)
 *	idx:	entry id (starting from zero to 14)
 * Return:
 *	0:	sucessful
 *	EIO:	HTU table entry cannot be written
 */

inline int set_htu_entry(u8 vpi, u16 vci, u8 qid, u8 idx)
{
	int i = 0;
	u32 tmp1=0;
	while ((tmp1 = readl(HTU_RAMSTAT_ADDR))!=0 && i < 1024) i++;
	if (i > 1024)
	{
		AMAZON_TPE_EMSG("timeout\n");
		return -EIO;
	}
	// write address register,
	AMAZON_WRITE_REGISTER_L(idx, HTU_RAMADDR_ADDR);
	// configure transmit queue
	tmp1 = vpi<<24|vci<<8;
	tmp1|= 	HTU_RAMDAT1_VCON	// valid connection the entry is not validated here !!!!!!!!!!!!!!!!
		|HTU_RAMDAT1_VCI3	// vci3 -> oam queue
		|HTU_RAMDAT1_VCI4	// vci4 -> oam queue
		|HTU_RAMDAT1_VCI6	// vci6 -> rm queue
		|HTU_RAMDAT1_PTI4	// pti4 -> oam queue
		|HTU_RAMDAT1_PTI5;	// pti5 -> oam queue

	// ramdat 1 (in params & oam handling)
	AMAZON_WRITE_REGISTER_L( tmp1, HTU_RAMDAT1_ADDR);
	// ramdat 2 (out params & oam handling)
	tmp1 = ((qid+CBM_RX_OFFSET)&HTU_RAMDAT2_QID_MASK)
		|HTU_RAMDAT2_PTI6
		|HTU_RAMDAT2_PTI7
		|HTU_RAMDAT2_F4U
		|HTU_RAMDAT2_F5U
		;
	AMAZON_WRITE_REGISTER_L( tmp1, HTU_RAMDAT2_ADDR);
	wmb();
	// write HTU entry
	AMAZON_WRITE_REGISTER_L(HTU_RAMCMD_WR, HTU_RAMCMD_ADDR);
	return 0;
}
/*
 * Brief:	add HTU table entry
 * Parameter:	
 *	vcc:	atm_vcc pointer
 *	qid:	CBM queue id
 * Return:
 *	0:	sucessful
 *	EIO:	HTU table entry cannot be written
 */
inline static int set_htu(struct atm_vcc *vcc, u32 qid)
{
	return set_htu_entry(vcc->vpi, vcc->vci, qid, (qid - CBM_DEFAULT_Q_OFFSET));
}

/* 
 * Brief:	allocate a queue
 * Return:	
 *		<=0	no available queues
 *		>0	qid
 */
static int atm_allocate_q(short itf)
{
	int i;
	u32 tmp1=0;
	int qid=0;
	amazon_atm_port_t * dev;
	dev = &g_atm_dev.ports[itf];
	//find start queue id for this interface
	for (i=0; i< itf; i++)
	{
		qid+= g_atm_dev.ports[i].max_conn;
	}
	// apply default queue offset ( oam, free cell queue, others, rm )
	qid += CBM_DEFAULT_Q_OFFSET;
	tmp1 = qid;
	// search for a free queue
	while (	(qid<tmp1+dev->max_conn)
		&& ( g_atm_dev.queues[qid].free != 1)) {
		qid++;;
	}
	// if none was found, send failure message and return
	if ( tmp1+dev->max_conn == qid)
	{
		return -EFAULT;
	}
	return qid;
	
}
/* Brief:	open a aal5 or aal0 connection
 */
static int atm_open(struct atm_vcc *vcc, push_back_t push)
{
	int err=0;
	int qid=0;
	amazon_atm_port_t * port = & g_atm_dev.ports[vcc->itf];
	unsigned long flags;
	/***************** check bandwidth ******************/
	/* 511045:linmars change ATM_VBR_NRT to use scr instead of pcr */
	if ((vcc->qos.txtp.traffic_class==ATM_CBR&&vcc->qos.txtp.max_pcr>port->tx_rem_cr)
	||(vcc->qos.txtp.traffic_class==ATM_VBR_RT&&vcc->qos.txtp.max_pcr>port->tx_rem_cr)
	||(vcc->qos.txtp.traffic_class==ATM_VBR_NRT&&vcc->qos.txtp.scr>port->tx_rem_cr) 
	||(vcc->qos.txtp.traffic_class==ATM_UBR_PLUS&&vcc->qos.txtp.min_pcr>port->tx_rem_cr)
	) {
                AMAZON_TPE_EMSG("not enough bandwidth left (%u) cells per seconds \n",port->tx_rem_cr);
                return -EINVAL;
	}
	if ( (qid = amazon_atm_find_vpivci(vcc->vpi, vcc->vci)) >0 ){
		AMAZON_TPE_EMSG("vpi:%u vci:%u is alreay open on queue:%u\n", vcc->vpi, vcc->vci, qid);
		return -EADDRINUSE;
	}

	/***************** allocate entry queueID for this port *****************/
	if ( (qid=atm_allocate_q(vcc->itf)) <= 0){
		AMAZON_TPE_EMSG("port: %u max:%u qid: %u\n", vcc->itf, port->max_conn, qid);
		AMAZON_TPE_EMSG("no availabel connections for this port:%u\n",vcc->itf);
		return -EINVAL;
	}
	/**************QSB parameters and CBM descriptors*************/
	set_qsb(vcc, &vcc->qos, qid);
	set_qd(vcc, qid);
	mb();
	err=set_htu(vcc,qid);
	if (err){
		AMAZON_TPE_EMSG("set htu entry fails %u\n",err);
		return err;
	}
	/************set internal mapping*************/
	local_irq_save(flags);
	g_atm_dev.queues[qid].free = 0;
	g_atm_dev.queues[qid].vcc = vcc;
	g_atm_dev.queues[qid].push = push;
	g_atm_dev.queues[qid+CBM_RX_OFFSET].free = 0;
	g_atm_dev.queues[qid+CBM_RX_OFFSET].vcc = vcc;
	g_atm_dev.queues[qid+CBM_RX_OFFSET].push = push;
	/******************reserve bandwidth**********************/
	if (vcc->qos.txtp.traffic_class == ATM_CBR){
		//CBR, real time connection, reserve PCR
		port->tx_cur_cr += vcc->qos.txtp.max_pcr;
		port->tx_rem_cr -= vcc->qos.txtp.max_pcr;
	}else if (vcc->qos.txtp.traffic_class == ATM_VBR_RT){
		//VBR_RT, real time connection, reserve PCR
		port->tx_cur_cr += vcc->qos.txtp.max_pcr;
		port->tx_rem_cr -= vcc->qos.txtp.max_pcr;
	}else if (vcc->qos.txtp.traffic_class == ATM_VBR_NRT){
		//VBR_NRT, reserve SCR
		port->tx_cur_cr += vcc->qos.txtp.pcr;
		port->tx_rem_cr -= vcc->qos.txtp.pcr;
	}else if (vcc->qos.txtp.traffic_class == ATM_UBR_PLUS){
		//UBR_PLUS, reserve MCR
		port->tx_cur_cr += vcc->qos.txtp.min_pcr;
		port->tx_rem_cr -= vcc->qos.txtp.min_pcr;
	}
	local_irq_restore(flags);
	return err;
}
/* Brief: 	Open ATM connection
 * Parameters: 	atm_vcc	- Pointer to VCC data structure
 *		vpi	- VPI value for new connection
 *		vci	- VCI value for new connection
 *
 * Return: 	0 	- sucessful
 * 		-ENOMEM	- No memory available
 *		-EINVAL	- No bandwidth/queue/ or unsupported AAL type
 * Description:
 *	This function opens an ATM connection on a specific device/interface
 *
 */
int	amazon_atm_open(struct atm_vcc *vcc,push_back_t push)
{
	int err=0;

	AMAZON_TPE_DMSG("vpi %u vci %u  itf %u aal %u\n"
		,vcc->vpi
		,vcc->vci
		,vcc->itf
		,vcc->qos.aal
		);
		
	AMAZON_TPE_DMSG("tx cl %u bw %u mtu %u\n"
		,vcc->qos.txtp.traffic_class
		,vcc->qos.txtp.max_pcr
		,vcc->qos.txtp.max_sdu
		);
	AMAZON_TPE_DMSG("rx cl %u bw %u mtu %u\n"
		,vcc->qos.rxtp.traffic_class
		,vcc->qos.rxtp.max_pcr
		,vcc->qos.rxtp.max_sdu
		);
	if (vcc->qos.aal == ATM_AAL5 || vcc->qos.aal == ATM_AAL0){
		err = atm_open(vcc,push);
	}else{
		AMAZON_TPE_EMSG("unsupported aal type %u\n", vcc->qos.aal);
		err = -EPROTONOSUPPORT;
	};
	if (err == 0 ){
		//replace the default memory allocation function with our own
		vcc->alloc_tx = amazon_atm_alloc_tx;
		set_bit(ATM_VF_READY,&vcc->flags);
	}
	return err;
}

/* Brief: 	Send ATM OAM cell
 * Parameters: 	atm_vcc	- Pointer to VCC data structure
 *		skb	- Pointer to sk_buff structure, that contains the data
 * Return: 	0 		- sucessful
 * 		-ENOMEM		- No memory available
 *		-EINVAL		- Not supported
 * Description:
 * This function sends a cell over and ATM connection
 * We always release the skb
 * TODO: flags handling (ATM_OF_IMMED, ATM_OF_INRATE)
 */
int	amazon_atm_send_oam(struct atm_vcc *vcc, void * cell, int flags)
{
	int err=0;
	int qid=0;
	struct amazon_atm_cell_header * cell_header;
	// Get cell header
	cell_header = (struct amazon_atm_cell_header*) cell;
	if ((cell_header->bit.pti == ATM_PTI_SEGF5) || (cell_header->bit.pti == ATM_PTI_E2EF5)) {
		qid = amazon_atm_find_vpivci( cell_header->bit.vpi, cell_header->bit.vci);
	}else if (cell_header->bit.vci == 0x3 || cell_header->bit.vci == 0x4) {
		//507281:tc.chen qid = amazon_atm_find_vpi((int) cell_header->bit.vpi);
		// 507281:tc.chen start
		u8 f4_vpi;
		f4_vpi = cell_header->bit.vpi;
		qid = amazon_atm_find_vpi(f4_vpi );
		// 507281:tc.chen end
	}else{
		//non-OAM cells, always invalid
		qid = -EINVAL;
	}
	if (qid == -EINVAL) {
		err =  -EINVAL;
		AMAZON_TPE_EMSG("not valid AAL0 packet\n");
	}else{
		//send the cell using swie
#ifdef TPE_LOOPBACK
		err = amazon_atm_swin(AMAZON_ATM_OAM_Q_ID, cell);
#else		
		err = amazon_atm_swin(qid, cell);
#endif
	}
	//kfree(cell);
	return err;
}

/* Brief: 	Send AAL5 frame through DMA
 * Parameters: 	vpi	- virtual path id 
 *		vci	- virtual circuit id
 *		clp	- cell loss priority
 *		qid	- CBM queue to be sent to
 *		skb	- packet to be sent
 * Return: 	0 		- sucessful
 * 		-ENOMEM		- No memory available
 *		-EINVAL		- Not supported
 * Description:
 * This function sends a AAL5 frame over and ATM connection
 * 	1. make sure that the data is aligned to 4x32-bit boundary
 *	2. provide the inbound data (CPCS-UU and CPI, not used here)
 *	3. set CLPn
 *	4. send the frame by DMA
 *	5. release the buffer ???
 ** use our own allocation alloc_tx
 ** we make sure the alignment and additional memory
 *** we always release the skb

 */
int amazon_atm_dma_tx(u8 vpi, u16 vci, u8 clp, u8 qid, struct sk_buff *skb) 
{
     	int err=0,need_pop=1;
	u32 * data=NULL;
	int nwrite=0;
	struct sk_buff *skb_tmp;
	u32 len=skb->len;	

	//AAL5S inbound header 8 bytes
	if (skb->len > g_atm_dev.aal5.tx_max_sdu - AAL5S_INBOUND_HEADER) {
		AMAZON_TPE_DMSG("tx_max_sdu:%u\n",g_atm_dev.aal5.tx_max_sdu); 
		AMAZON_TPE_DMSG("skb too large [%u]!\n",skb->len);
		err = -EMSGSIZE;
		goto atm_dma_tx_error_exit;
	}
	
	//Check the byte alignment requirement and header space
	if ( ( ((u32)(skb->data)%16) !=AAL5S_INBOUND_HEADER)|| (skb_headroom(skb)<AAL5S_INBOUND_HEADER)){
		//not aligned or no space for header, fall back to memcpy
		skb_tmp = dev_alloc_skb(skb->len+16);
		if (skb_tmp==NULL){
			err = - ENOMEM;			
			goto atm_dma_tx_error_exit;	
		}
		alloc_align_16(skb_tmp);
		g_atm_dev.aal5.cnt_cpy++;
		skb_reserve(skb_tmp,AAL5S_INBOUND_HEADER);
		memcpy(skb_put(skb_tmp,skb->len), skb->data, skb->len);
		amazon_atm_free_tx_skb(skb);
		need_pop=0;
		skb = skb_tmp;
	}
	//Provide AAL5S inbound header
	data = (u32 *)skb_push(skb,8);
	data[0] = __be32_to_cpu(vpi<<20|vci<<4|clp);
	data[1] = __be32_to_cpu(g_atm_dev.aal5.padding_byte<<8|qid);
	
	len = skb->len;

	//send through DMA
	AMAZON_TPE_DMSG("AAL5S header 0 %8x\n", data[0]);
	AMAZON_TPE_DMSG("AAL5S header 0 %8x\n", data[1]);
	AMAZON_TPE_DMSG("about to call dma_write len: %u\n", len);
	nwrite=dma_device_write( &g_dma_dev,skb->data,len,skb);
	if (nwrite != len) {
		//DMA descriptors full
//		AMAZON_TPE_EMSG("AAL5 packet drop due to DMA nwrite:%u skb->len:%u\n", nwrite,len);
		AMAZON_TPE_DMSG("AAL5 packet drop due to DMA nwrite:%u skb->len:%u\n", nwrite,len);
		err = -EAGAIN;
		goto atm_dma_tx_drop_exit;
	}
	AMAZON_TPE_DMSG("just finish call dma_write\n");
	//release in the "dma done" call-back
	return 0;
atm_dma_tx_error_exit:
	g_atm_dev.mib_counter.tx_err++;	
	queue_statics(qid, QS_ERR);
	goto atm_dma_tx_exit;
	
atm_dma_tx_drop_exit:
	g_atm_dev.mib_counter.tx_drop++;
	queue_statics(qid, QS_SW_DROP);
atm_dma_tx_exit:
	if (need_pop){
		amazon_atm_free_tx_skb(skb);
	}else{
		dev_kfree_skb_any(skb);
	}
	return err;
}

/* Brief: 	Send AAL0/AAL5 packet
 * Parameters: 	atm_vcc	- Pointer to VCC data structure
 *		skb	- Pointer to sk_buff structure, that contains the data
 * Return: 	0 		- sucessful
 * 		-ENOMEM		- No memory available
 *		-EINVAL		- Not supported
 * Description:
 *	See amazon_atm_dma_tx
 */
int	amazon_atm_send(struct atm_vcc *vcc,struct sk_buff *skb)
{
	int qid=0;
	u8 clp=0;
        int err=0;
	u32 wm=0;

	if (vcc == NULL || skb == NULL){
		AMAZON_TPE_EMSG("invalid parameter\n");
		return -EINVAL;
	}
	ATM_SKB(skb)->vcc = vcc;
	qid = amazon_atm_get_queue(vcc);
	if (valid_qid(qid) != 1) {
		AMAZON_TPE_EMSG("invalid vcc!\n");
		err = -EINVAL;
		goto atm_send_err_exit;
	}
	
	//Send AAL0 using SWIN
	if (vcc->qos.aal == ATM_AAL0){
#ifdef  TPE_LOOPBACK
		err=amazon_atm_swin((qid+CBM_RX_OFFSET), skb->data);	
#else
		err=amazon_atm_swin(qid, skb->data);
#endif
		if (err){
			goto atm_send_err_exit;
		}	
		goto atm_send_exit;
	}
	
	//Should be AAl5
	//MIB counter
	g_atm_dev.mib_counter.tx++;
	adsl_led_flash();//joelin adsl led
	queue_statics(qid, QS_PKT);

#ifdef AMAZON_CHECK_LINK
	//check adsl link 
	if (adsl_link_status == 0){
		//link down
		AMAZON_TPE_DMSG("ADSL link down, discarded!\n");
		err=-EFAULT;
		goto atm_send_drop_exit;
	}
#endif
	clp = (vcc->atm_options&ATM_ATMOPT_CLP)?1:0;
	//check watermark first
	wm = readl(CBM_WMSTAT0_ADDR);
	if (  (wm & (1<<qid))
	    ||( (vcc->qos.txtp.traffic_class != ATM_CBR
	         &&vcc->qos.txtp.traffic_class != ATM_VBR_RT) 
		&(wm & (CBM_WM_NRT_MASK | (clp&CBM_WM_CLP1_MASK)) ))){
		//wm hit: discard
		AMAZON_TPE_DMSG("watermark hit, discarded!\n");
		err=-EFAULT;
		goto atm_send_drop_exit;
	}
#ifdef  TPE_LOOPBACK
	return amazon_atm_dma_tx(vcc->vpi, vcc->vci,clp, (qid+CBM_RX_OFFSET),skb);
#else	
	return amazon_atm_dma_tx(vcc->vpi, vcc->vci,clp, qid,skb);
#endif

atm_send_exit:	
	amazon_atm_free_tx_skb_vcc(vcc,skb);	
	return 0;
	
atm_send_drop_exit:
	g_atm_dev.mib_counter.tx_drop++;
	queue_statics(qid,QS_SW_DROP);
atm_send_err_exit:	
	amazon_atm_free_tx_skb_vcc(vcc,skb);
	return err;
}

/* Brief:	Return ATM port related MIB
 * Parameter:	interface number
 	 atm_cell_ifEntry_t
 */
int amazon_atm_cell_mib(atm_cell_ifEntry_t* to,u32 itf)
{
	g_atm_dev.mib_counter.htu_unp += readl(HTU_MIBCIUP);
	to->ifInUnknownProtos = g_atm_dev.mib_counter.htu_unp;
#ifdef AMAZON_TPE_READ_ARC
	u32 reg_val=0;
	meiDebugRead((AR_CELL0_ADDR+itf*4),&reg_val,1);
	g_atm_dev.mib_counter.rx_cells += reg_val;
	reg_val=0;
	meiDebugWrite((AR_CELL0_ADDR+itf*4),&reg_val,1);
	to->ifHCInOctets_h = (g_atm_dev.mib_counter.rx_cells * 53)>>32;
	to->ifHCInOctets_l = (g_atm_dev.mib_counter.rx_cells * 53) & 0xffff;
	
	meiDebugRead((AT_CELL0_ADDR+itf*4),&reg_val,1);
	g_atm_dev.mib_counter.tx_cells += reg_val;
	reg_val=0;
	meiDebugWrite((AT_CELL0_ADDR+itf*4),&reg_val,1);
	to->ifHCOutOctets_h = (g_atm_dev.mib_counter.tx_cells * 53)>>32;
	to->ifHCOutOctets_l = (g_atm_dev.mib_counter.rx_cells * 53) & 0xffff;
	
	meiDebugRead((AR_CD_CNT0_ADDR+itf*4),&reg_val,1);
	g_atm_dev.mib_counter.rx_err_cells += reg_val;
	reg_val=0;
	meiDebugWrite((AR_CD_CNT0_ADDR+itf*4),&reg_val,1);
	to->ifInErrors = g_atm_dev.mib_counter.rx_err_cells;
	
	to->ifOutErrors = 0;
#else
	to->ifHCInOctets_h = 0;
	to->ifHCInOctets_l = 0;
	to->ifHCOutOctets_h = 0;
	to->ifHCOutOctets_l = 0;
	to->ifInErrors = 0;
	to->ifOutErrors = 0;
#endif
	return 0;
}

/* Brief:	Return ATM AAL5 related MIB
 * Parameter:
 	 atm_aal5_ifEntry_t
 */
int amazon_atm_aal5_mib(atm_aal5_ifEntry_t* to)
{
	u32 reg_l,reg_h;
	//AAL5R	received Octets from ATM
	reg_l = readl(AAL5_RIOL_ADDR);
	reg_h = readl(AAL5_RIOM_ADDR);
	g_atm_dev.mib_counter.rx_cnt_h +=reg_h;
	if (reg_l + g_atm_dev.mib_counter.rx_cnt_l < reg_l){
		g_atm_dev.mib_counter.rx_cnt_h++;
	}
	
	g_atm_dev.mib_counter.rx_cnt_l+= reg_l;
	//AAL5S	sent Octets to ATM
	reg_l = readl(AAL5_SOOL_ADDR);
	reg_h = readl(AAL5_SOOM_ADDR);
	g_atm_dev.mib_counter.tx_cnt_h +=reg_h;
	if (reg_l + g_atm_dev.mib_counter.tx_cnt_l < reg_l){
		g_atm_dev.mib_counter.tx_cnt_h++;
	}
	g_atm_dev.mib_counter.tx_cnt_l+= reg_l;


	g_atm_dev.mib_counter.tx_ppd += readl(CBM_AAL5ODIS_ADDR);
	g_atm_dev.mib_counter.rx_drop += readl(CBM_AAL5IDIS_ADDR);
	
	//store 
	to->ifHCInOctets_h = g_atm_dev.mib_counter.rx_cnt_h;
	to->ifHCInOctets_l = g_atm_dev.mib_counter.rx_cnt_l;
	to->ifHCOutOctets_h = g_atm_dev.mib_counter.tx_cnt_h;
	to->ifHCOutOctets_l = g_atm_dev.mib_counter.tx_cnt_l;
	to->ifOutDiscards = g_atm_dev.mib_counter.tx_drop;
	to->ifInDiscards = g_atm_dev.mib_counter.rx_drop;

	//Software provided counters
	//packets passed to higher layer
	to->ifInUcastPkts = g_atm_dev.mib_counter.rx;
	//packets passed from higher layer
	to->ifOutUcastPkts = g_atm_dev.mib_counter.tx;
	//number of wrong downstream packets
	to->ifInErrors = g_atm_dev.mib_counter.rx_err;
	//number of wrong upstream packets
	to->ifOutErros = g_atm_dev.mib_counter.tx_err;

	return 0;
}
/* Brief:	Return ATM AAL5 VCC related MIB from internale use
 * Parameter:
 *	qid
 *	atm_aal5_vcc_t
 */
static int __amazon_atm_vcc_mib(int qid, atm_aal5_vcc_t* to)
{
	//aal5VccCrcErrors
	to->aal5VccCrcErrors = g_atm_dev.queues[qid].aal5VccCrcErrors;
	to->aal5VccOverSizedSDUs =g_atm_dev.queues[qid].aal5VccOverSizedSDUs;
	to->aal5VccSarTimeOuts = 0; //not supported yet
	return 0;
}
/* Brief:	Return ATM AAL5 VCC related MIB from vpi/vci
 * Parameter:	atm_vcc
 * 	 atm_aal5_vcc_t
 */
int amazon_atm_vcc_mib_x(int vpi, int vci,atm_aal5_vcc_t* to)
{
	int qid=0;
	int err=0;
	qid =  amazon_atm_find_vpivci(vpi, vci);
	if (qid >0 ){
		err = __amazon_atm_vcc_mib(qid,to);
	}else{
		return -EINVAL;
	}
	return err;
}


/* Brief:	Return ATM AAL5 VCC related MIB
 * Parameter:	atm_vcc
 *	 atm_aal5_vcc_t
 */
int amazon_atm_vcc_mib(struct atm_vcc *vcc,atm_aal5_vcc_t* to)
{
	int qid=0;
	int err=0;
	qid =  amazon_atm_get_queue(vcc);
	if (qid >0 ){
		err = __amazon_atm_vcc_mib(qid,to);
	}else{
		return -EINVAL;
	}
	return err;
}

/* Brief: 	Close ATM connection
 * Parameters: 	atm_vcc	- Pointer to VCC data structure
 * Return: 	no
 * Description:
 * This function closes the given ATM connection
 */
void	amazon_atm_close(struct atm_vcc *vcc){
	int i;
	int qid=0;
	u32 tmp1;
	u8 * qd_addr;
	unsigned long flags;
	if (vcc == NULL){
		AMAZON_TPE_EMSG("invalid parameter. vcc is null\n");
		return;
	}
	u32 itf = (u32) vcc->itf;
	//release bandwidth
	if (vcc->qos.txtp.traffic_class == ATM_CBR){
		g_atm_dev.ports[itf].tx_rem_cr += vcc->qos.txtp.max_pcr;
		g_atm_dev.ports[itf].tx_cur_cr -= vcc->qos.txtp.max_pcr;
	}else if (vcc->qos.txtp.traffic_class == ATM_VBR_RT){
		g_atm_dev.ports[itf].tx_rem_cr += vcc->qos.txtp.max_pcr;
		g_atm_dev.ports[itf].tx_cur_cr -= vcc->qos.txtp.max_pcr;
	}else if (vcc->qos.txtp.traffic_class == ATM_VBR_NRT){
		g_atm_dev.ports[itf].tx_rem_cr += vcc->qos.txtp.pcr;
		g_atm_dev.ports[itf].tx_cur_cr -= vcc->qos.txtp.pcr;
	}else if (vcc->qos.txtp.traffic_class == ATM_UBR_PLUS){
		g_atm_dev.ports[itf].tx_rem_cr += vcc->qos.txtp.min_pcr;
		g_atm_dev.ports[itf].tx_cur_cr -= vcc->qos.txtp.min_pcr;
	}

	qid = amazon_atm_get_queue(vcc);
	if (qid == -EINVAL){
		AMAZON_TPE_EMSG("unknown vcc %u.%u.%u\n", vcc->itf, vcc->vpi, vcc->vci);
		return;
	}
	local_irq_save(flags);
	//Disable HTU entry
	i=0;
	while ((tmp1 = readl(HTU_RAMSTAT_ADDR))!=0 && i < HTU_RAM_ACCESS_MAX) i++;
 	if (i == HTU_RAM_ACCESS_MAX){
		AMAZON_TPE_EMSG("HTU RAM ACCESS out of time\n");
	}

	// write address register
	AMAZON_WRITE_REGISTER_L(qid - CBM_DEFAULT_Q_OFFSET, HTU_RAMADDR_ADDR);
	// invalidate the connection
	AMAZON_WRITE_REGISTER_L(0, HTU_RAMDAT1_ADDR);
	// write command
	AMAZON_WRITE_REGISTER_L(HTU_RAMCMD_WR,HTU_RAMCMD_ADDR);
	
	qd_addr = (u8 *) KSEG1ADDR((unsigned long)g_atm_dev.cbm.qd_addr);
#ifdef AMAZON_ATM_DEBUG
	tmp1 = readl(qd_addr+qid*CBM_QD_SIZE+0x8) & 0xffff;
	AMAZON_TPE_DMSG("TX queue has %u cells \n", tmp1);
	tmp1 = readl( qd_addr+(qid+CBM_RX_OFFSET)*CBM_QD_SIZE+0x08)&0xffff;
	AMAZON_TPE_DMSG("RX queue has %u cells \n", tmp1);
#endif	
	// set threshold of txqueue to 0
	tmp1 = readl(qd_addr+qid*CBM_QD_SIZE+0x0c);
	tmp1&= (~ CBM_QD_W3_THRESHOLD_MASK);
	AMAZON_WRITE_REGISTER_L(tmp1, (qd_addr+qid*CBM_QD_SIZE+0x0c));
	// set threshold of rxqueue to 0
	tmp1 = readl( qd_addr+(qid+CBM_RX_OFFSET)*CBM_QD_SIZE+0x0c);
	tmp1&= (~ CBM_QD_W3_THRESHOLD_MASK);
	AMAZON_WRITE_REGISTER_L(tmp1,(qd_addr+(qid+CBM_RX_OFFSET)*CBM_QD_SIZE+0x0c));

	//clear internal mapping
	amazon_atm_clear_vcc(qid);
	amazon_atm_clear_vcc(qid+CBM_RX_OFFSET);

	local_irq_restore(flags);
}


/* Brief:	initialize internal data structure
 */
static void atm_constructor(amazon_atm_dev_t * dev)
{
	int i;
	memset(dev,0,sizeof(amazon_atm_dev_t));
	atm_init_parameters(dev);
	//internal: queue "free" flag
	for(i=1;i<AMAZON_ATM_MAX_QUEUE_NUM;i++) {
		//dev->queues[i].vcc=NULL;
		dev->queues[i].free = 1;
	}
	for(i=0;i<AMAZON_ATM_PORT_NUM;i++){
		dev->ports[i].tx_rem_cr = dev->ports[i].tx_max_cr;
	}
	//MIB
	atomic_set(&dev->dma_tx_free_0,1); //initially there should be free descriptors
}

/* Brief:	return round up base-2 logarithm
 */
static inline int get_log_2(u32 value)
{
	int i=0,j=1;
	while (i<11){
		if (j>=value) break;
		j=j<<1;
		i++;
	}
	AMAZON_TPE_DMSG("round up base-2 logarithm of %u is %u\n", value, i);
	return i;
}

/* Brief:	TPE hardware initialization
 * Parameter:	specifiy the configurations of the hardware
 */
static inline int atm_init_hard(amazon_atm_dev_t * dev)
{
	int i;
	u32 tmp1, tmp2, tmp3;
	u8 * mem_addr=NULL;
	u8 * qd_addr=NULL;
	//PMU power on the module 1st
	*(AMAZON_PMU_PWDCR) = 	(*AMAZON_PMU_PWDCR) | (AMAZON_PMU_PWDCR_TPE);
	//Reset the module
	*(AMAZON_RST_REQ) = (* AMAZON_RST_REQ) | (AMAZON_RST_REQ_TPE);
	mb();
	mdelay(100);
	*(AMAZON_RST_REQ) = (* AMAZON_RST_REQ) & (~(AMAZON_RST_REQ_TPE));
	mb();
	
	unsigned long qsb_clk = amazon_get_fpi_hz()>>1;
	/*********allocate & arrange memory for CBM *********/
	if (dev->cbm.mem_addr == NULL){
		dev->cbm.allocated = 1;
		mem_addr = (u8 *)__get_free_pages(GFP_KERNEL, get_log_2(((CBM_CELL_SIZE * dev->cbm.free_cell_cnt) >>PAGE_SHIFT) + 1));
		if (mem_addr != NULL){ 
			dev->cbm.mem_addr = mem_addr;
		} else	{
			goto init_no_mem;
		}
	}
	if (dev->cbm.qd_addr == NULL){
#ifdef CONFIG_USE_VENUS
		//to work around a bug, bit15 of QDOFF address should be 1,Aug4, 2004
		//thus, we allocate 64k memory		
		qd_addr = (u8 *)__get_free_pages(GFP_KERNEL, 4);
		if (qd_addr != NULL) {
			dev->cbm.qd_addr_free = (u8*) (((unsigned long) qd_addr));
			dev->cbm.qd_addr = (u8*) (((unsigned long) qd_addr) | 0x8000);
		}else{
			goto init_no_mem;
		}
#else	//CONFIG_USE_VENUS
		qd_addr = (u8 *)kmalloc( CBM_QD_SIZE * AMAZON_ATM_MAX_QUEUE_NUM, GFP_KERNEL);
		if (qd_addr != NULL) {
			dev->cbm.qd_addr = qd_addr;
		}else {
			goto init_no_mem;
		}
#endif //CONFIG_USE_VENUS			
	}
//#ifndef CONFIG_MIPS_UNCACHED
	mem_addr = (u8 *)KSEG1ADDR((unsigned long)dev->cbm.mem_addr);
	qd_addr = (u8 *)KSEG1ADDR((unsigned long)dev->cbm.qd_addr);
//#endif
	//CBM reset cell queue memory, 64 bytes / cell
	memset_io(mem_addr, 0, CBM_CELL_SIZE * dev->cbm.free_cell_cnt);
	//make a link list, last 4 bytes is pointer
	for(i=1;i<dev->cbm.free_cell_cnt;i++){
		AMAZON_WRITE_REGISTER_L(CPHYSADDR((mem_addr + CBM_CELL_SIZE * i)),(mem_addr + CBM_CELL_SIZE * (i-1) + 0x3c));
	}
	//reset queue descriptor
	memset_io(qd_addr, 0, CBM_QD_SIZE * AMAZON_ATM_MAX_QUEUE_NUM);
	//init word 0-2 of q0 (free cell list)
	//address of last cell
	AMAZON_WRITE_REGISTER_L(CPHYSADDR((mem_addr + CBM_CELL_SIZE * (dev->cbm.free_cell_cnt-1))), qd_addr);
	//address of first cell
	AMAZON_WRITE_REGISTER_L(CPHYSADDR((mem_addr)), (qd_addr + 4));
	//no. of free cells
	AMAZON_WRITE_REGISTER_L(dev->cbm.free_cell_cnt,(qd_addr + 8));
	//init q descriptor for OAM receiving
	AMAZON_WRITE_REGISTER_L((CBM_QD_W3_INT_ACA | (divide_by_64_round_up(oam_q_threshold)&0xff)<< CBM_QD_W3_THRESHOLD_SHIFT), (qd_addr + AMAZON_ATM_OAM_Q_ID * CBM_QD_SIZE + 0x0c));
//	AMAZON_WRITE_REGISTER_L((CBM_QD_W3_INT_ACA | (u32)oam_q_threshold<< CBM_QD_W3_THRESHOLD_SHIFT), (qd_addr + AMAZON_ATM_OAM_Q_ID * CBM_QD_SIZE + 0x0c));
	//config CBM
	//set offset address and threshold
	AMAZON_WRITE_REGISTER_L(CPHYSADDR(qd_addr), CBM_QDOFF_ADDR);
	AMAZON_WRITE_REGISTER_L(((dev->cbm.nrt_thr&CBM_THR_MASK)|CBM_WM_3_1), CBM_NRTTHR_ADDR);
	AMAZON_WRITE_REGISTER_L(((dev->cbm.clp0_thr&CBM_THR_MASK)|CBM_WM_3_1), CBM_CLP0THR_ADDR);
	AMAZON_WRITE_REGISTER_L(((dev->cbm.clp1_thr&CBM_THR_MASK)|CBM_WM_3_1), CBM_CLP1THR_ADDR);
	//config interrupts
	AMAZON_WRITE_REGISTER_L( CBM_IMR_MASK & (~(CBM_IMR_ACA|CBM_IMR_Q0E|CBM_IMR_Q0I|CBM_IMR_RDE|CBM_IMR_OPF|CBM_IMR_ERR             	
#ifdef AMAZON_ATM_DEBUG	
						|CBM_IMR_DISC|CBM_IMR_QFD|CBM_IMR_NFCA|CBM_IMR_CLP1TR|CBM_IMR_CLP0TR|CBM_IMR_NRTTR|CBM_IMR_QTR
#endif	
#ifdef AMAZON_TPE_SCR
						|CBM_IMR_EF 
#endif
							)), CBM_IMR0_ADDR);
	AMAZON_WRITE_REGISTER_L(SRC_CLRR|SRC_TOS_MIPS | SRC_SRE_ENABLE | AMAZON_CBM_INT, CBM_SRC0_ADDR);
	
	//HTU
	//RAM entry for number of possible connections per interface
	tmp1 = dev->ports[0].max_conn?dev->ports[0].max_conn-1:0;
	AMAZON_WRITE_REGISTER_L(tmp1, HTU_RX0_ADDR);
	for(i=1;i<AMAZON_ATM_PORT_NUM;i++){
		tmp1+=dev->ports[i].max_conn;
		AMAZON_WRITE_REGISTER_L(tmp1, HTU_RX0_ADDR + 4 * i);
	}
	dev->cbm.max_q_off = tmp1+1;
	//Queue ID for OAM/RM/Other cells
	AMAZON_WRITE_REGISTER_L (AMAZON_ATM_OAM_Q_ID, HTU_DESTOAM_ADDR);
	AMAZON_WRITE_REGISTER_L( AMAZON_ATM_RM_Q_ID, HTU_DESTRM_ADDR);
	AMAZON_WRITE_REGISTER_L( AMAZON_ATM_OTHER_Q_ID, HTU_DESTOTHER_ADDR);
	//Timeout
	AMAZON_WRITE_REGISTER_L((u32) HTUTIMEOUT, HTU_TIMEOUT_ADDR);
#ifdef AMAZON_ATM_DEBUG
        AMAZON_WRITE_REGISTER_L((u32) HTU_ISR_MASK 
				&(~(HTU_ISR_NE|HTU_ISR_TORD|HTU_ISR_OTOC|HTU_ISR_ONEC|HTU_ISR_PNE|HTU_ISR_PT)), HTU_IMR0_ADDR);
	AMAZON_WRITE_REGISTER_L(SRC_CLRR|SRC_TOS_MIPS|SRC_SRE_ENABLE|AMAZON_HTU_INT,HTU_SRC0_ADDR);
#endif
	//QSB
	//global setting, TstepC, SBL, Tau
	//Tau
	AMAZON_WRITE_REGISTER_L(dev->qsb.tau, QSB_TAU_ADDR);
	//SBL
	AMAZON_WRITE_REGISTER_L(dev->qsb.sbl, QSB_SBL_ADDR);
	//tstep
	AMAZON_WRITE_REGISTER_L(dev->qsb.tstepc>>1, QSB_CONFIG_ADDR);

	//port settting
	for(i=0;i<AMAZON_ATM_PORT_NUM;i++){
		if ( (dev->ports[i].enable) && (dev->ports[i].tx_max_cr!=0) ){
			tmp1 = ((qsb_clk * dev->qsb.tstepc) >>1) / dev->ports[i].tx_max_cr;
			tmp2 = tmp1 / 64;	//integer value of Tsb
			tmp3 = tmp1%64 + 1; 	//fractional part of Tsb
			//carry over to integer part (?)
			if (tmp3 == 64) {
				tmp3 = 0;
				tmp2++;
			}
			if (tmp2 == 0){
				tmp2 = 1;
				tmp3 = 1;
			}
			//1. set mask 2. write value to data transfer register 3. start the transfer
			//SCT(FracRate)
			AMAZON_WRITE_REGISTER_L(QSB_SET_SCT_MASK, QSB_RTM_ADDR);
			AMAZON_WRITE_REGISTER_L(tmp3,QSB_RTD_ADDR);
			AMAZON_WRITE_REGISTER_L(((QSB_TABLESEL_SCT<<QSB_TABLESEL_SHIFT)|QSB_RAMAC_REG_LOW|QSB_WRITE|i),QSB_RAMAC_ADDR);
			//SPT(SBV + PN + IntRage)
			AMAZON_WRITE_REGISTER_L(QSB_SET_SPT_MASK, QSB_RTM_ADDR);
			AMAZON_WRITE_REGISTER_L(QSB_SPT_SBVALID|tmp2|(i<<16),QSB_RTD_ADDR);
			AMAZON_WRITE_REGISTER_L(((QSB_TABLESEL_SPT<<QSB_TABLESEL_SHIFT)|QSB_RAMAC_REG_LOW|QSB_WRITE|i),QSB_RAMAC_ADDR);


		}
	}
	
	//SWIE: Setup Service Request Control Registers to enable interrupts
	AMAZON_WRITE_REGISTER_L(SRC_CLRR|SRC_TOS_MIPS | SRC_SRE_ENABLE | AMAZON_SWIE_INT, SWIE_ISRC_ADDR);
	AMAZON_WRITE_REGISTER_L(SRC_CLRR|SRC_TOS_MIPS | SRC_SRE_ENABLE | AMAZON_SWIE_INT, SWIE_ESRC_ADDR);

	wmb();
#ifdef AMAZON_TPE_TEST_AAL5_INT
	AMAZON_WRITE_REGISTER_L(AAL5R_ISR_FE,AAL5_RIMR0_ADDR);
	AMAZON_WRITE_REGISTER_L(0, AAL5_SIMR0_ADDR);
	AMAZON_WRITE_REGISTER_L(SRC_CLRR|SRC_TOS_MIPS | SRC_SRE_ENABLE | AMAZON_AAL5_INT,AAL5_SSRC0_ADDR);
	AMAZON_WRITE_REGISTER_L(SRC_CLRR|SRC_TOS_MIPS | SRC_SRE_ENABLE | AMAZON_AAL5_INT,AAL5_RSRC0_ADDR);
#endif //AMAZON_TPE_TEST_AAL5_INT

	AMAZON_WRITE_REGISTER_L(dev->aal5.tx_max_sdu,AAL5_SMFL_ADDR);	
 	AMAZON_WRITE_REGISTER_L(dev->aal5.rx_max_sdu,AAL5_RMFL_ADDR);
	AMAZON_WRITE_REGISTER_L(AAL5_SCMD_MODE_POLL // enable polling mode
 					|AAL5_SCMD_SS
 					|AAL5_SCMD_AR
					,AAL5_SCMD_ADDR);
	//start CBM
 	AMAZON_WRITE_REGISTER_L(CBM_CFG_START,CBM_CFG_ADDR);
 	wmb();
	return 0;
init_no_mem:
	if (mem_addr != NULL) free_pages((unsigned long)mem_addr,get_log_2(((CBM_CELL_SIZE * dev->cbm.free_cell_cnt) >>PAGE_SHIFT) + 1));
	
#ifdef CONFIG_USE_VENUS	
	//to work around a bug, bit15 of QDOFF address should be 1
	if (qd_addr != NULL) free_pages((unsigned long)qd_addr,4);
#else //CONFIG_USE_VENUS
	if (qd_addr != NULL) kfree(qd_addr);	
#endif //CONFIG_USE_VENUS	
	return -ENOMEM;
}

/*	
 *	Brief:	Create entry in /proc for status information
 */
void atm_create_proc(void)
{
	create_proc_read_entry("amazon_atm", 0,NULL, amazon_atm_read_procmem,(void*)PROC_ATM);
        create_proc_read_entry("amazon_atm_mib", 0,NULL, amazon_atm_read_procmem,(void*)PROC_MIB);
        create_proc_read_entry("amazon_atm_vcc", 0,NULL, amazon_atm_read_procmem,(void*)PROC_VCC);
#if 0	
	create_proc_read_entry("amazon_atm_aal5", 0,NULL, amazon_atm_read_procmem,(void*)PROC_AAL5);
        create_proc_read_entry("amazon_atm_cbm", 0,NULL, amazon_atm_read_procmem,(void*)PROC_CBM);
        create_proc_read_entry("amazon_atm_htu", 0,NULL, amazon_atm_read_procmem,(void*)PROC_HTU);
        create_proc_read_entry("amazon_atm_qsb", 0,NULL, amazon_atm_read_procmem,(void*)PROC_QSB);
        create_proc_read_entry("amazon_atm_swie", 0,NULL, amazon_atm_read_procmem,(void*)PROC_SWIE);
#endif
}

/*
 *	Brief:	Delete entry in /proc for status information
 */
void atm_delete_proc(void)
{
	remove_proc_entry("amazon_atm", NULL);
        remove_proc_entry("amazon_atm_mib", NULL);
        remove_proc_entry("amazon_atm_vcc", NULL);
#if 0	
	remove_proc_entry("amazon_atm_aal5", NULL);
        remove_proc_entry("amazon_atm_cbm", NULL);
        remove_proc_entry("amazon_atm_htu", NULL);
        remove_proc_entry("amazon_atm_qsb", NULL);
        remove_proc_entry("amazon_atm_swie", NULL);
#endif	
}
/* Brief: 	Initialize ATM module
 * Parameters: 	no
 * Return: 	&g_atm_dev - sucessful
 *		NULL	- fails: 
 *			1. invalid parameter
 * 			2. No memory available
 * Description:
 *  This function configure the TPE components according to the input info,
 *	-CBM
 *	-HTU
 *	-QSB
 *	-AAL5
 *
 */
amazon_atm_dev_t * amazon_atm_create(void)
{
	int i;
	AMAZON_TPE_DMSG("atm_init\n");
	/************initialize global data structure****************/
	atm_constructor(&g_atm_dev);
	/***********allocate kernel resources****************/
	//bottom halfs for SWEX
	swex_start_task.routine = amazon_atm_swex;
	swex_start_task.data = NULL;
	swex_complete_task.routine = amazon_atm_swex_push;
	swex_complete_task.data = NULL;
#ifdef AMAZON_TPE_SCR
	a5r_task.routine = amazon_atm_a5r;
	a5r_task.data = NULL;
#endif	//AMAZON_TPE_SCR
	//SWIN semaphore
	sema_init(&(g_atm_dev.swie.in_sem), 1);
	//SWIE lock
	clear_bit(SWIE_LOCK, &(g_atm_dev.swie.lock));
	//SWIE wait queue
	init_waitqueue_head(&(g_atm_dev.swie.sleep));
	atm_create_proc();
		
	//register DMA
	memset(&g_dma_dev,0,sizeof(struct dma_device_info));
	strcpy(g_dma_dev.device_name,"TPE");
	g_dma_dev.weight=1; 
	g_dma_dev.num_tx_chan=2;
     	g_dma_dev.num_rx_chan=2; 
     	g_dma_dev.ack=1;
     	g_dma_dev.tx_burst_len=4;
     	g_dma_dev.rx_burst_len=4;
     	//DMA TX

	for(i=0;i<1;i++){
		g_dma_dev.tx_chan[i].weight=QOS_DEFAULT_WGT;
       		g_dma_dev.tx_chan[i].desc_num=10;
       		g_dma_dev.tx_chan[i].packet_size=g_atm_dev.aal5.tx_max_sdu + AAL5S_INBOUND_HEADER;
       		g_dma_dev.tx_chan[i].control=1;
     	}
	//DMA RX
     	for(i=0;i<2;i++){
       		g_dma_dev.rx_chan[i].weight=QOS_DEFAULT_WGT;
		/* BingTao's suggestion, change from 5->10 will prevent packet loss in NO_TX_INT mode */
		g_dma_dev.rx_chan[i].desc_num=10;
		g_dma_dev.rx_chan[i].packet_size=(g_atm_dev.aal5.rx_max_sdu + AAL5R_TRAILER_LEN+0x10f)&(~0xf);
		g_dma_dev.rx_chan[i].control=1;
     	}
	g_dma_dev.intr_handler=amazon_atm_dma_handler;
    	g_dma_dev.buffer_alloc=amazon_atm_alloc_rx;
     	g_dma_dev.buffer_free=amazon_atm_free_tx;
     	dma_device_register(&g_dma_dev);
/***********intialize the atm hardware ****************/
	if ( atm_init_hard(&g_atm_dev) != 0){
		return NULL;
	}
	//start CBM
	AMAZON_WRITE_REGISTER_L(CBM_CFG_START,CBM_CFG_ADDR);
	wmb();

	//Start HTU
	AMAZON_WRITE_REGISTER_L(HTU_CFG_START ,HTU_CFG_ADDR);
	wmb();


	// Register interrupts for insertion and extraction
	request_irq(AMAZON_SWIE_INT, amazon_atm_swie_isr, IRQF_DISABLED, "tpe_swie", NULL);
	request_irq(AMAZON_CBM_INT, amazon_atm_cbm_isr, IRQF_DISABLED, "tpe_cbm", NULL);
#ifdef AMAZON_ATM_DEBUG
	request_irq(AMAZON_HTU_INT , amazon_atm_htu_isr, IRQF_DISABLED, "tpe_htu", NULL);
#endif
#ifdef AMAZON_TPE_TEST_AAL5_INT	
	request_irq(AMAZON_AAL5_INT, amazon_atm_aal5_isr, IRQF_DISABLED, "tpe_aal5", NULL);
#endif
	return &g_atm_dev;
}

/* Brief: 	clean up atm
 * Parameters: 	no
 * Return: 	no
 * Description:
 *  Disable the device.
 */
void	amazon_atm_cleanup(void){
	int i;
	clear_bit(SWIE_LOCK, &(g_atm_dev.swie.lock));
	wake_up(&g_atm_dev.swie.sleep);
	up(&g_atm_dev.swie.in_sem);
	// diable SWIE interrupts
	AMAZON_WRITE_REGISTER_L(0, SWIE_ISRC_ADDR);
	AMAZON_WRITE_REGISTER_L(0, SWIE_ESRC_ADDR);
	wmb();

	// Disable schedulers ( including interrupts )-----------------------
	for (i = 0; i < AMAZON_ATM_PORT_NUM; i++);
	{
		AMAZON_WRITE_REGISTER_L(QSB_SET_SPT_SBVALID_MASK, QSB_RTM_ADDR);
		AMAZON_WRITE_REGISTER_L( 0 ,QSB_RTD_ADDR);
		AMAZON_WRITE_REGISTER_L( (QSB_TABLESEL_SPT<<QSB_TABLESEL_SHIFT)
						| QSB_RAMAC_REG_LOW
						| QSB_WRITE
						| i,
						QSB_RAMAC_ADDR);
	}
	// disable QSB_Interrupts
	AMAZON_WRITE_REGISTER_L( 0, QSB_IMR_ADDR);
	AMAZON_WRITE_REGISTER_L( 0, QSB_SRC_ADDR);
	// disable CBM interrupts
	AMAZON_WRITE_REGISTER_L( 0	, CBM_IMR0_ADDR);
	AMAZON_WRITE_REGISTER_L( 0 , CBM_SRC0_ADDR);
	// set CBM start bit to 0
	AMAZON_WRITE_REGISTER_L(0,CBM_CFG_ADDR);
	// request hardware extraction of queue 0, wich should force the CBM
	// to recognize that the start bit is not set
	AMAZON_WRITE_REGISTER_L(CBM_HWEXPAR_PN_A5, CBM_HWEXPAR0_ADDR);
	// write frame extraction command into the hw extract command register
	AMAZON_WRITE_REGISTER_L(CBM_HWEXCMD_FE0, CBM_HWEXCMD_ADDR);
	// disable htu
 	// disable all HTU interrupts
	AMAZON_WRITE_REGISTER_L(0  ,HTU_IMR0_ADDR);
	AMAZON_WRITE_REGISTER_L(0  ,HTU_SRC0_ADDR);
	
	if (g_atm_dev.cbm.allocated){
		free_pages((unsigned long)g_atm_dev.cbm.mem_addr, get_log_2(((CBM_CELL_SIZE * g_atm_dev.cbm.free_cell_cnt) >>PAGE_SHIFT)+1));
#ifdef CONFIG_USE_VENUS
		//to work around a bug, bit15 of QDOFF address should be 1
		free_pages((unsigned long)g_atm_dev.cbm.qd_addr_free,4);
#else //CONFIG_USE_VENUS
		kfree(g_atm_dev.cbm.qd_addr);
#endif //CONFIG_USE_VENUS						
	}
	atm_delete_proc();
	// free interrupts for insertion and extraction
	dma_device_unregister(&g_dma_dev);
	free_irq(AMAZON_SWIE_INT, NULL);
	free_irq(AMAZON_CBM_INT, NULL);
#ifdef AMAZON_ATM_DEBUG
	free_irq(AMAZON_HTU_INT, NULL);
#endif
#ifdef AMAZON_TPE_TEST_AAL5_INT
	free_irq(AMAZON_AAL5_INT, NULL);
#endif

}

/************************ ATM network interface ***********************************************/
/*	Brief:		getsockopt
 */
int amazon_atm_getsockopt(struct atm_vcc *vcc, int level, int optname, char *optval, int optlen)
{
	int err=0;
	atm_aal5_vcc_t mib_vcc;
	AMAZON_TPE_DMSG("1\n");
	switch (optname){
		case	SO_AMAZON_ATM_MIB_VCC:
			AMAZON_TPE_DMSG("2\n");
			err = amazon_atm_vcc_mib(vcc, &mib_vcc);
			AMAZON_TPE_DMSG("%u\n",mib_vcc.aal5VccCrcErrors);
                        err = copy_to_user((void *)optval,&mib_vcc, sizeof(mib_vcc));
                        AMAZON_TPE_DMSG("err %u\n",err);
                        break;
                default:
                	return -EFAULT;
	}
	return err;
}

/*	Brief:		IOCTL
 */

int amazon_atm_ioctl(struct atm_dev *dev,unsigned int cmd,void *arg)
{
	int     err=0;
	//MIB
	atm_cell_ifEntry_t mib_cell;
	atm_aal5_ifEntry_t mib_aal5;
	atm_aal5_vcc_x_t mib_vcc;
	if (_IOC_TYPE(cmd) != AMAZON_ATM_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > AMAZON_ATM_IOC_MAXNR) return -ENOTTY;

	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
	if (err) {
		AMAZON_TPE_EMSG("acess verification fails \n");
		return -EFAULT;
	}
	switch(cmd) {
		case AMAZON_ATM_MIB_CELL:
			err = amazon_atm_cell_mib(&mib_cell,(u32)arg);
			if (err==0){
				err = __copy_to_user((void *)arg,&mib_cell,sizeof(mib_cell));
			}else{
				AMAZON_TPE_EMSG("cannot get MIB ATM_CELL\n");
			}
			break;
		case AMAZON_ATM_MIB_AAL5:
			err = amazon_atm_aal5_mib(&mib_aal5);
			if (err==0){
				err=__copy_to_user(arg, &mib_aal5, sizeof(mib_aal5));
			}else{
				AMAZON_TPE_EMSG("cannot get MIB ATM_AAL5\n");
			}
			break;
		case AMAZON_ATM_MIB_VCC:
			err=__copy_from_user(&mib_vcc,arg, sizeof(mib_vcc));
			AMAZON_TPE_DMSG("return of copy_from_user %x\n",err);
			err = amazon_atm_vcc_mib_x(mib_vcc.vpi, mib_vcc.vci, &(mib_vcc.mib_vcc));
			if (err==0){
				err=__copy_to_user(arg, &mib_vcc, sizeof(mib_vcc));	
			}else{
				AMAZON_TPE_EMSG("cannot get MIB ATM_VCC\n");
			}
				
    		default:
    			return -ENOTTY;
	}
	return err;
}
/*	Brief:	return a link list of OAM related time stamp info
 *	Parameter:	none
 *	Return:	
 		a link list of "struct oam_last_activity" data
 *	Description:
 		Each time, a F4/F5 cell or AAL5 packet is received, the time stamp is updated.
		Through this call, u get a list of this time stamp for all active connection.
		Please note that u have read-only access.
 */
const struct oam_last_activity* get_oam_time_stamp()
{
	int i,j;
	for(i=CBM_DEFAULT_Q_OFFSET+CBM_RX_OFFSET,j=0;i<CBM_RX_OFFSET+CBM_DEFAULT_Q_OFFSET+AMAZON_ATM_MAX_VCC_NUM;i++){
		if (g_atm_dev.queues[i].free != 1 && g_atm_dev.queues[i].vcc != NULL){
			//active connection
			if (j !=0 ){
				g_oam_time_stamp[j-1].next = &g_oam_time_stamp[j];
			}
			g_oam_time_stamp[j].vpi = g_atm_dev.queues[i].vcc->vpi;
			g_oam_time_stamp[j].vci = g_atm_dev.queues[i].vcc->vci;
			g_oam_time_stamp[j].stamp = g_atm_dev.queues[i].access_time;
			g_oam_time_stamp[j].next = NULL;
			j++;
		}
	}
	if (j==0) {
		return NULL;
	}else{
		return g_oam_time_stamp;
	}
}


/*	Brief:	call back routine for rx
 *	Parameter:
 *		vcc atm_vcc pointer
 *		skb	data if no error
 		err	error flag, 0: no error, 1:error
 *	Return:	
 *		0	
 *		<>0	cannot push up
 *	Description:
 *		release the packet if cannot push up
 */
static	int amazon_atm_net_push(struct atm_vcc *vcc,struct sk_buff *skb, int err)
{
	if (err){
		if (vcc && vcc->stats) {
			atomic_inc(&vcc->stats->rx_err);
		}
	}else{	
		ATM_SKB(skb)->vcc = vcc;
	
		if (!atm_charge(vcc, skb->truesize)){
			//no space this vcc
			AMAZON_TPE_EMSG("no space for this vcc\n");
			dev_kfree_skb_any(skb);
			return -ENOMEM;
		}
		atomic_inc(&vcc->stats->rx);
		AMAZON_TPE_DMSG("push to vcc\n");
		vcc->push(vcc,skb);
	}
	return 0;
}
int	amazon_atm_net_send_oam(struct atm_vcc*vcc, void *cell, int flags)
{
	return amazon_atm_send_oam(vcc,cell,flags);
}

int	amazon_atm_net_send(struct atm_vcc *vcc,struct sk_buff *skb)
{
	int err=0;
	if (vcc->qos.aal == ATM_AAL0 || vcc->qos.aal == ATM_AAL5) {
		err=amazon_atm_send(vcc,skb);
	}else{
		//not supported
		err =  -EPROTONOSUPPORT;
	}
	if (err){
		atomic_inc(&vcc->stats->tx_err);
	}else{
		atomic_inc(&vcc->stats->tx);
	}
	AMAZON_TPE_DMSG("sent, tx_inuse:%u\n", atomic_read(&vcc->tx_inuse));
	return err;
}

int	amazon_atm_net_open(struct atm_vcc *vcc,short vpi, int vci)
{
	vcc->itf = (int)  vcc->dev->dev_data;
	vcc->vpi = vpi;
	vcc->vci = vci;
	return(amazon_atm_open(vcc,amazon_atm_net_push));
}

static int amazon_atm_change_qos(struct atm_vcc *vcc, struct atm_qos *qos, int flgs)
{
	int qid;
	
	if (vcc == NULL || qos == NULL){
		AMAZON_TPE_EMSG("invalid parameters\n");
		return -EINVAL;
	}
	qid = amazon_atm_get_queue(vcc);
	if (valid_qid(qid) != 1) {
		AMAZON_TPE_EMSG("no vcc connection opened\n");
		return -EINVAL;
	}
	set_qsb(vcc,qos,qid);
	return 0;
}

static struct atmdev_ops amazon_atm_ops = {
   open:	amazon_atm_net_open,
   close:	amazon_atm_close,
   ioctl:	amazon_atm_ioctl,
   send:	amazon_atm_net_send,
   send_oam:	amazon_atm_net_send_oam,
//   getsockopt:	amazon_atm_getsockopt,
   change_qos:	amazon_atm_change_qos,
//   proc_read:	amazon_atm_proc_read,
   owner:	THIS_MODULE,
};				// ATM device callback functions

/*
 * brief "/proc" function
 */
int amazon_atm_read_procmem(char *buf, char **start, off_t offset,int count, int *eof, void *data)
{
	int buf_off=0;	/* for buf */
	int i=0,j=0;
	int type= (u32)data;//which module
	atm_aal5_ifEntry_t mib_aal5;
	atm_cell_ifEntry_t mib_cell;
	atm_aal5_vcc_t mib_vcc;
    	switch(type){
		case PROC_MIB:
			//MIB counter
			amazon_atm_aal5_mib(&mib_aal5);
			//TX:
			buf_off+=sprintf(buf+buf_off,"\n============= AAL5 Upstream =========\n");
			buf_off+=sprintf(buf+buf_off,"received %u (pkts) from upper layer\n", mib_aal5.ifOutUcastPkts);
			buf_off+=sprintf(buf+buf_off,"errors: %u (pkts)\n",mib_aal5.ifOutErros);
			buf_off+=sprintf(buf+buf_off,"discards: %u (ptks)\n", mib_aal5.ifOutDiscards);
			buf_off+=sprintf(buf+buf_off,"transmitted: %x-%x (bytes) \n",
					mib_aal5.ifHCOutOctets_h, mib_aal5.ifHCOutOctets_l);
			//RX:
			buf_off+=sprintf(buf+buf_off,"\n============= AAL5 Downstream =========\n");
			buf_off+=sprintf(buf+buf_off,"received %x-%x (bytes)\n",
					mib_aal5.ifHCInOctets_h,mib_aal5.ifHCInOctets_l);
			buf_off+=sprintf(buf+buf_off,"discards: %u (ptks)\n",mib_aal5.ifInDiscards);
			buf_off+=sprintf(buf+buf_off,"errors: %u (ptks)\n",mib_aal5.ifInErrors);
			buf_off+=sprintf(buf+buf_off,"passed %u (ptks) to upper layer\n",mib_aal5.ifInUcastPkts);
			
			//Cell level
			buf_off+=sprintf(buf+buf_off,"\n============= ATM Cell =========\n");
			amazon_atm_cell_mib(&mib_cell,0);
#ifdef AMAZON_TPE_READ_ARC			
			buf_off+=sprintf(buf+buf_off,"Port 0: downstream received: %x-%x (bytes)\n",mib_cell.ifHCInOctets_h,mib_cell.ifHCInOctets_l);
			buf_off+=sprintf(buf+buf_off,"Port 0: upstream transmitted: %x-%x (bytes)\n",mib_cell.ifHCOutOctets_h,mib_cell.ifHCOutOctets_l);
			buf_off+=sprintf(buf+buf_off,"Port 0: downstream errors: %u (cells)\n",mib_cell.ifInErrors);
			amazon_atm_cell_mib(&mib_cell,1);
			buf_off+=sprintf(buf+buf_off,"Port 1: downstream received: %x-%x (bytes)\n",mib_cell.ifHCInOctets_h,mib_cell.ifHCInOctets_l);
			buf_off+=sprintf(buf+buf_off,"Port 1: upstream transmitted: %x-%x (bytes)\n",mib_cell.ifHCOutOctets_h,mib_cell.ifHCOutOctets_l);
			buf_off+=sprintf(buf+buf_off,"Port 1: downstream errors: %u (cells)\n",mib_cell.ifInErrors);
#endif
			buf_off+=sprintf(buf+buf_off,"HTU discards: %u (cells)\n",mib_cell.ifInUnknownProtos);
			
			buf_off+=sprintf(buf+buf_off,"\n====== Specials =====\n");
			buf_off+=sprintf(buf+buf_off,"AAL5S PPD: %u (cells)\n",g_atm_dev.mib_counter.tx_ppd);
#ifdef AMAZON_TPE_SCR
			buf_off+=sprintf(buf+buf_off,"Reassembly wait: %u \n",g_a5r_wait);
#endif
			break;
		case PROC_ATM:
			//Interface (Port)
			buf_off+=sprintf(buf+buf_off,"[Interfaces]\n");
			for(i=0;i<AMAZON_ATM_PORT_NUM;i++){
				if (g_atm_dev.ports[i].enable==0){
					buf_off+=sprintf(buf+buf_off,"\tport[%u] not in use\n",i);
				}else{
					buf_off+=sprintf(buf+buf_off,"\tport[%u]\n\t\tmax_conn=%u\n"
									,i
									,g_atm_dev.ports[i].max_conn
									);
					buf_off+=sprintf(buf+buf_off,"\t\ttx_max=%u\n\t\trem=%u\n\t\tcur=%u\n"
									,g_atm_dev.ports[i].tx_max_cr
									,g_atm_dev.ports[i].tx_rem_cr
									,g_atm_dev.ports[i].tx_cur_cr
									);
				}

			}
			//Units Info
			//AAL5
			buf_off+=sprintf(buf+buf_off,"[AAL5]\n\tpad=%c(%x)\n\trx_mtu=%u\n\ttx_mtu=%u\n"
    					,g_atm_dev.aal5.padding_byte
    					,g_atm_dev.aal5.padding_byte
    					,g_atm_dev.aal5.rx_max_sdu
    					,g_atm_dev.aal5.tx_max_sdu
    					);
			//CBM
			buf_off+=sprintf(buf+buf_off,
				"[CBM]\n\tnrt_thr=%u\n\tclp0_thr=%u\n\tclp1_thr=%u\n\ttx_q_threshold=%u\n\trx_q_threshold=%u\n\toam_q_threshold=%u\n\tfree_cell_cnt=%u\n"
    					,g_atm_dev.cbm.nrt_thr
    					,g_atm_dev.cbm.clp0_thr
    					,g_atm_dev.cbm.clp1_thr
					,tx_q_threshold
					,rx_q_threshold
					,oam_q_threshold
    					,g_atm_dev.cbm.free_cell_cnt
    					);
			//QSB
			buf_off+=sprintf(buf+buf_off,"[QSB]\n\ttau=%u\n\ttstepc=%u\n\tsbl=%u\n"
    					,g_atm_dev.qsb.tau
    					,g_atm_dev.qsb.tstepc
    					,g_atm_dev.qsb.sbl
    					);
			buf_off+=sprintf(buf+buf_off,"[Debugging]\n\taal5_need_copy=%u\n",g_atm_dev.aal5.cnt_cpy);
			break;
		case PROC_VCC:
			for(i=CBM_DEFAULT_Q_OFFSET,j=0;i<g_atm_dev.cbm.max_q_off+CBM_DEFAULT_Q_OFFSET;i++){
				if (g_atm_dev.queues[i].free!=1){
					buf_off+=sprintf(buf+buf_off,"vcc[%u]\n\tvpi=%u vci=%u itf=%u qid=%u access_time=%u.%u\n"
									,j++
									,g_atm_dev.queues[i].vcc->vpi
									,g_atm_dev.queues[i].vcc->vci
									,g_atm_dev.queues[i].vcc->itf
									,i
									,(u32)g_atm_dev.queues[i+CBM_RX_OFFSET].access_time.tv_sec
									,(u32)g_atm_dev.queues[i+CBM_RX_OFFSET].access_time.tv_usec
									);
					buf_off+=sprintf(buf+buf_off,"\tqos_tx class=%u max_pcr=%u pcr=%u min_pcr=%u scr=%u mbs=%u cdv=%u\n"
									,g_atm_dev.queues[i].vcc->qos.txtp.traffic_class
									,g_atm_dev.queues[i].vcc->qos.txtp.max_pcr
									,g_atm_dev.queues[i].vcc->qos.txtp.pcr
									,g_atm_dev.queues[i].vcc->qos.txtp.min_pcr
									,g_atm_dev.queues[i].vcc->qos.txtp.scr
									,g_atm_dev.queues[i].vcc->qos.txtp.mbs
									,g_atm_dev.queues[i].vcc->qos.txtp.cdv
									);
					buf_off+=sprintf(buf+buf_off,"\tqos_rx class=%u max_pcr=%u pcr=%u min_pcr=%u scr=%u mbs=%u cdv=%u\n"
									,g_atm_dev.queues[i].vcc->qos.rxtp.traffic_class
									,g_atm_dev.queues[i].vcc->qos.rxtp.max_pcr
									,g_atm_dev.queues[i].vcc->qos.rxtp.pcr
									,g_atm_dev.queues[i].vcc->qos.rxtp.min_pcr
									,g_atm_dev.queues[i].vcc->qos.rxtp.scr
									,g_atm_dev.queues[i].vcc->qos.rxtp.mbs
									,g_atm_dev.queues[i].vcc->qos.rxtp.cdv
									);
					__amazon_atm_vcc_mib((i+CBM_RX_OFFSET),&mib_vcc);
					buf_off+=sprintf(buf+buf_off,"\tCRC error=%u\n", mib_vcc.aal5VccCrcErrors);
					buf_off+=sprintf(buf+buf_off,"\toversized packet=%u\n", mib_vcc.aal5VccOverSizedSDUs);
#ifdef 	AMAZON_ATM_DEBUG
					if ( valid_qid(i+CBM_RX_OFFSET)){
					buf_off+=sprintf(buf+buf_off,"\tdownstream statics\n" );
					buf_off+=sprintf(buf+buf_off,"\t\tpackets=%u\n",g_atm_dev.queues[i+CBM_RX_OFFSET].qs[QS_PKT]);
					buf_off+=sprintf(buf+buf_off,"\t\terr_packets=%u\n",g_atm_dev.queues[i+CBM_RX_OFFSET].qs[QS_ERR] );
					buf_off+=sprintf(buf+buf_off,"\t\tsw_dropped=%u\n",g_atm_dev.queues[i+CBM_RX_OFFSET].qs[QS_SW_DROP] );
					}

					buf_off+=sprintf(buf+buf_off,"\tupstream statics\n" );
					buf_off+=sprintf(buf+buf_off,"\t\tpackets=%u\n",g_atm_dev.queues[i].qs[QS_PKT]);
					buf_off+=sprintf(buf+buf_off,"\t\terr_packets=%u\n",g_atm_dev.queues[i].qs[QS_ERR] );
					buf_off+=sprintf(buf+buf_off,"\t\thw_dropped=%u\n",g_atm_dev.queues[i].qs[QS_HW_DROP] );
					buf_off+=sprintf(buf+buf_off,"\t\tsw_dropped=%u\n",g_atm_dev.queues[i].qs[QS_SW_DROP] );

#endif					
	
				}

			}
			break;
		default:
			break;
    	}
	if(buf_off>0)	*eof = 1;
	return buf_off;
}

#ifdef AMAZON_TPE_AAL5_RECOVERY
extern int (*tpe_reset)(void);
extern int (*tpe_start)(void);
extern int (*tpe_inject)(void);
/*	Brief:		Reset TPE hardware
 *	Description
 *		This is a wordaround for AAL5 bug. It tries to reset TPE.
 *	 take care of software
 *	 setup all previous connection
 */
int amazon_tpe_reset(void)
{
	struct atm_vcc * vcc;	
	int err=0;
	int i;
	u8 * qd_addr;
	u32 reg_l, reg_h;
	unsigned int a_cfg_value=0;
	unsigned int a_cfg_old_value=0;
	atm_aal5_ifEntry_t mib_aal5;
	atm_cell_ifEntry_t mib_cell;
	
	//make sure all cells transmitting out first
	//Segmentation done
	amazon_atm_aal5_mib(&mib_aal5);
	reg_l = g_atm_dev.mib_counter.tx_cnt_l;
	reg_h = g_atm_dev.mib_counter.tx_cnt_h;
	while(1){
		mdelay(10);
		amazon_atm_aal5_mib(&mib_aal5);
		if( (reg_l == g_atm_dev.mib_counter.tx_cnt_l) && (reg_h == g_atm_dev.mib_counter.tx_cnt_h) ){
			break;
		}
		AMAZON_TPE_DMSG("AAL5 Segmentation still in progress!\n");
		reg_l = g_atm_dev.mib_counter.tx_cnt_l;
		reg_h = g_atm_dev.mib_counter.tx_cnt_h;
	}
	//QSB done
	qd_addr = (u8 *) KSEG1ADDR((unsigned long)g_atm_dev.cbm.qd_addr);
	for (i=1;i<15;i++){
		while ( (err=readl(qd_addr+i*CBM_QD_SIZE+0x8)&0xffff) !=0  ){
			mdelay(20);
			AMAZON_TPE_DMSG("queue %u not empty (%u)\n",i,err);
		}
	}
	//insurance for interfaces between Aware and CARB
	mdelay(100);
	amazon_atm_cell_mib(&mib_cell,0);
	amazon_atm_cell_mib(&mib_cell,1);
	amazon_atm_aal5_mib(&mib_aal5);
	
	mb();
	while ( (AMAZON_READ_REGISTER_L(AR_CELLRDY_BC0) != 0 ) || (AMAZON_READ_REGISTER_L(AR_CELLRDY_BC0) != 0 )  ){
		AMAZON_TPE_EMSG("\nwaiting for AWARE");
		AMAZON_TPE_EMSG(" BC0 %u ", AMAZON_READ_REGISTER_L(AR_CELLRDY_BC0));
		AMAZON_TPE_EMSG(" BC1 %u ", AMAZON_READ_REGISTER_L(AR_CELLRDY_BC1));
		AMAZON_TPE_EMSG("\n");
		mdelay(1);
	}
	// disable AAI module
	meiDebugRead(A_CFG_ADDR,&a_cfg_value,1);	
	a_cfg_old_value=a_cfg_value;
	a_cfg_value &= (~(0x2800));
	meiDebugWrite(A_CFG_ADDR,&a_cfg_value,1);	
	//clear buffer
	a_cfg_value = 0x1;
	meiDebugWrite(AR_CB0_STATUS_ADDR,&a_cfg_value,1);	
	meiDebugWrite(AR_CB1_STATUS_ADDR,&a_cfg_value,1);	

	if ( atm_init_hard(&g_atm_dev) != 0){
		return -EIO;
	}
	sema_init(&(g_atm_dev.swie.in_sem), 1);
	//SWIE lock
	clear_bit(SWIE_LOCK, &(g_atm_dev.swie.lock));
	//SWIE wait queue
	init_waitqueue_head(&(g_atm_dev.swie.sleep));
	
	for (i=CBM_DEFAULT_Q_OFFSET;i<AMAZON_ATM_MAX_QUEUE_NUM/2;i++) {
		vcc = g_atm_dev.queues[i].vcc;
		if (vcc != NULL){
			set_qsb(vcc, &vcc->qos, i);
			set_qd(vcc, i);
			mb();
			err=set_htu(vcc,i);
			if (err){
				AMAZON_TPE_EMSG("set htu entry fails %u\n",err);
			}
		}
	}
	meiDebugWrite(A_CFG_ADDR,&a_cfg_old_value,1);	
#if 0
	//reset DFE
	*(AMAZON_RST_REQ) = (* AMAZON_RST_REQ) | (AMAZON_RST_REQ_DFE);
	mb();
	*(AMAZON_RST_REQ) = (* AMAZON_RST_REQ) &  (~AMAZON_RST_REQ_DFE);
	mb();
#endif
	
	return 0;
}

/* Brief:	Send a ATM EoP packet to save DMA channel
 */
int amazon_tpe_inject_debug_cell(void)
{
	//Send a ATM cell to save DMA channel
	u8 qid;
	unsigned char atm_cell[48];
	qid = 0x11;
	AMAZON_TPE_DMSG("qid = %d\n",qid);
	memset(atm_cell,0,48);
	atm_cell[3] = 0x2;
	if ( amazon_atm_swin(qid,atm_cell)) {
		AMAZON_TPE_EMSG("cannot insert EoP cell\n");
		return -1;
	}
	return 0;
}

/* Brief:	start HTU (TPE)
 */

int amazon_tpe_start(void)
{
	AMAZON_WRITE_REGISTER_L(HTU_CFG_START ,HTU_CFG_ADDR);
	wmb();
	return 0;
}
#endif //AMAZON_TPE_AAL5_RECOVERY

#ifdef AMAZON_CHECK_LINK
extern int (*adsl_link_notify)(int);
/*	Brief:	notify link status of ADSL link
 *	Parameters:	0	link down
 *			1	link up
 *	Returns:	0 	OK
 *	Details:	called by MEI driver 
 *	should update status and inform upper layer
 */
int amazon_tpe_link_notify(int status)
{
	adsl_link_status = status;
	AMAZON_TPE_DMSG("link status %s\n",(status==1)?"Up":"Down");
	if (status == 0){
		//wait until no cells in upstream queues
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(2*HZ);
	}
	return 0;
}
#endif //ifdef AMAZON_CHECK_LINK

/*
 *	Brief: 		Initialize ATM module
 *
 *  	Return Value:	ENOMEM		- No memory available
 *			EBUSY		- Cannot register atm device
 *			ERESTARTSYS 	- Process interrupted by other signal
 *			0		- OK, module initialized
 *
 *	Description:
 *	This function registers an atm device for all UTOPIA devices.
 *	It also allocates memory for the private device data structures
 */
int __init amazon_atm_net_init(void)
{
	int i;
	int err=0;
	amazon_atm_dev_t *dev = NULL;
	
	if ((dev=amazon_atm_create()) != NULL){
		for(i=0;i<AMAZON_ATM_PORT_NUM;i++){
			if (!dev->ports[i].enable){
				amazon_atm_devs[i] = NULL;
				continue;
			}
			amazon_atm_devs[i] =atm_dev_register("amazon_atm",&amazon_atm_ops,-1,0UL);
			if (amazon_atm_devs[i] == NULL){
				AMAZON_TPE_EMSG("atm_dev_register fails\n");
				err = -EIO;
				goto amazon_atm_net_init_exit;
			}else{
				AMAZON_TPE_DMSG("registering device %u\n",i);
				amazon_atm_devs[i]->ci_range.vpi_bits = 8;
				amazon_atm_devs[i]->ci_range.vci_bits = 16;
				amazon_atm_devs[i]->link_rate = dev->ports[i].tx_max_cr;
				amazon_atm_devs[i]->dev_data = (void *) i;
			}
		}
			
	}else{
		err = -ENOMEM;
		AMAZON_TPE_EMSG("cannot init atm device\n");
		goto amazon_atm_net_init_exit;
	}
#ifdef AMAZON_TPE_AAL5_RECOVERY	
	tpe_reset = & amazon_tpe_reset;
	tpe_start = & amazon_tpe_start;
	tpe_inject = & amazon_tpe_inject_debug_cell;
#endif //AMAZON_TPE_AAL5_RECOVERY
#ifdef AMAZON_CHECK_LINK
	adsl_link_notify=amazon_tpe_link_notify;
#endif //AMAZON_CHECK_LINK
amazon_atm_net_init_exit:
	return err;
}

void __exit amazon_atm_net_cleanup(void)
{
	int i;
	amazon_atm_cleanup();
	for(i=0;i<AMAZON_ATM_PORT_NUM;i++){
		if (amazon_atm_devs[i] != NULL){
			AMAZON_TPE_DMSG("unregister dev %u\n",i);
			atm_dev_deregister(amazon_atm_devs[i]);
		}
	}
	return;
}
EXPORT_SYMBOL(get_oam_time_stamp);

MODULE_LICENSE ("GPL"); 
MODULE_AUTHOR("Infineon IFAP DC COM peng.liu@infineon.com");
MODULE_DESCRIPTION("AMAZON ATM driver");

module_init(amazon_atm_net_init);
module_exit(amazon_atm_net_cleanup);
 




