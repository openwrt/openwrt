#include <linux/kernel.h>
#include <net/genetlink.h>

#include "dpns_common.h"
#include "sf_genl_msg.h"
#include "dpns_common_genl.h"
#include "io.h"
#include "ops.h"

static struct dpns_common_priv *g_priv;

static const NPU_STATUS_t port_tmu_status[] = {
	{0x10000C , " q0 sts0" , GENMASK(31,16) , "queue tail ptr"},
	{0 , " q0 sts0" , GENMASK(15,0) , "queue head ptr"},

	{0x100010 , " q0 sts1" , GENMASK(31,0) , "queue pkt cnt"},
	{0x100014 , " q0 sts2" , GENMASK(31,0) , "queue buf cnt"},
	{0x10002C , " q1 sts0" , GENMASK(31,16) , "queue tail ptr"},
	{0 , " q1 sts0" , GENMASK(15,0) , "queue head ptr"},
	{0x100030 , " q1 sts1" , GENMASK(31,0) , "queue pkt cnt"},
	{0x100034 , " q1 sts2" , GENMASK(31,0) , "queue buf cnt"},

	{0x10004C , " q2 sts0" , GENMASK(31,16) , "queue tail ptr"},
	{0 , " q2 sts0" , GENMASK(15,0) , "queue head ptr"},

	{0x100050 , " q2 sts1" , GENMASK(31,0) , "queue pkt cnt"},
	{0x100054 , " q2 sts2" , GENMASK(31,0) , "queue buf cnt"},

	{0x10006C , " q3 sts0" , GENMASK(31,16) , "queue tail ptr"},
	{0 , " q3 sts0" , GENMASK(15,0) , "queue head ptr"},

	{0x100070 , " q3 sts1" , GENMASK(31,0) , "queue pkt cnt"},
	{0x100074 , " q3 sts2" , GENMASK(31,0) , "queue buf cnt"},
	{0x10008C , " q4 sts0" , GENMASK(31,16) , "queue tail ptr"},
	{0 , " q4 sts0" , GENMASK(15,0) , "queue head ptr"},
	{0x100090 , " q4 sts1" , GENMASK(31,0) , "queue pkt cnt"},
	{0x100094 , " q4 sts2" , GENMASK(31,0) , "queue buf cnt"},
	{0x1000aC , " q5 sts0" , GENMASK(31,16) , "queue tail ptr"},
	{0 , " q5 sts0" , GENMASK(15,0) , "queue head ptr"},
	{0x1000b0 , " q5 sts1" , GENMASK(31,0) , "queue pkt cnt"},
	{0x1000b4 , " q5 sts2" , GENMASK(31,0) , "queue buf cnt"},

	{0x1000CC , " q6 sts0" , GENMASK(31,16) , "queue tail ptr"},
	{0 , " q6 sts0" , GENMASK(15,0) , "queue head ptr"},

	{0x1000D0 , " q6 sts1" , GENMASK(31,0) , "queue pkt cnt"},
	{0x1000D4 , " q6 sts2" , GENMASK(31,0) , "queue buf cnt"},

	{0x1000EC , " q7 sts0" , GENMASK(31,16) , "queue tail ptr"},
	{0 , " q7 sts0" , GENMASK(15,0) , "queue head ptr"},

	{0x1000F0 , " q7 sts1" , GENMASK(31,0) , "queue pkt cnt"},
	{0x1000F4 , " q7 sts2" , GENMASK(31,0) , "queue buf cnt"},
	{0x101094 , " shaper0 sts" , GENMASK(31,1) , "csr shp0 credit cntr"},
	{0 , " shaper0 sts" , GENMASK(0,0) , "csr shp0 status"},

	{0x1010B4 , " shaper1 sts" , GENMASK(31,1) , "csr shp1 credit cntr"},
	{0 , " shaper1 sts" , GENMASK(0,0) , "csr shp1 status"},

	{0x1010D4 , " shaper2 sts" , GENMASK(31,1) , "csr shp2 credit cntr"},
	{0 , " shaper2 sts" , GENMASK(0,0) , "csr shp2 status"},

	{0x1010F4 , " shaper3 sts" , GENMASK(31,1) , "csr shp3 credit cntr"},
	{0 , " shaper3 sts" , GENMASK(0,0) , "csr shp3 status"},

	{0x101114 , " shaper4 sts" , GENMASK(31,1) , "csr shp4 credit cntr"},
	{0 , " shaper4 sts" , GENMASK(0,0) , "csr shp4 status"},

	{0x101134 , " shaper5 sts" , GENMASK(31,1) , "csr shp5 credit cntr"},
	{0 , " shaper5 sts" , GENMASK(0,0) , "csr shp5 status"}
};

static const NPU_STATUS_t npu_status[] = {
	// ======================== PART I  NPU status reg =============================
	/* Pkt_rcv status info */
	{0x200014 , "Error status" , GENMASK(31,14) , "fifo full error(all port rcv)"},
	{0 , "Error status" , GENMASK(13,7) , "miss sof error(all port rcv)"},
	{0 , "Error status" , GENMASK(6,0) , "miss eof error(all port rcv)"},
	{0x200080 , "port_dbg sig0" , GENMASK(31,14) , "lif_data_fla"},
	{0 , "port_dbg sig0" , GENMASK(13,11) , "cur_state"},
	{0 , "port_dbg sig0" , GENMASK(10,8) , "nxt_state"},
	{0 , "port_dbg sig0" , GENMASK(7,0) , "cond_in_rw_data"},

	{0x200084 , "port_dbg sig1" , GENMASK(31,13) , "queue_id"},
	{0 , "port_dbg sig1" , GENMASK(12,12) , "rw_rr_en"},
	{0 , "port_dbg sig1" , GENMASK(11,11) , "rw_rr_req"},
	{0 , "port_dbg sig1" , GENMASK(10,10) , "lif_fifo_rd_en"},
	{0 , "port_dbg sig1" , GENMASK(9,9) , "ram_buf_wr_en"},
	{0 , "port_dbg sig1" , GENMASK(8,8) , "bmu_buf_rd_req"},
	{0 , "port_dbg sig1" , GENMASK(7,7) , "linklist_mem_wr_en"},
	{0 , "port_dbg sig1" , GENMASK(6,6) , "port_spl_mem_wr_en"},
	{0 , "port_dbg sig1" , GENMASK(5,5) , "sof_on"},
	{0 , "port_dbg sig1" , GENMASK(4,4) , "eof_on"},
	{0 , "port_dbg sig1" , GENMASK(3,3) , "reclaim start"},
	{0 , "port_dbg sig1" , GENMASK(2,2) , "fast mode"},
	{0 , "port_dbg sig1" , GENMASK(1,1) , "pkt done"},
	{0 , "port_dbg sig1" , GENMASK(0,0) , "port buf alm empty"},
	/* register address */
	{0x200088 , "port_dbg sig2" , GENMASK(31,11) , "port spl mem full"},
	{0 , "port_dbg sig2" , GENMASK(10,10) , "port ptr mem full"},
	{0 , "port_dbg sig2" , GENMASK(9,9) , "llm fifo alm  empty"},
	{0 , "port_dbg sig2" , GENMASK(8,8) , "ram buf wready"},
	{0 , "port_dbg sig2" , GENMASK(7,7) , "linklist mem wready"},
	{0 , "port_dbg sig2" , GENMASK(6,0) , "reclaim mem full"},

	{0x20008c , "port_dbg sig3" , GENMASK(31,9) , "reclaim mem empty"},
	{0 , "port_dbg sig3" , GENMASK(8,8) , "port spl mem empty"},
	{0 , "port_dbg sig3" , GENMASK(7,7) , "pkt ptr mem wready"},
	{0 , "port_dbg sig3" , GENMASK(6,6) , "lif fifo rr start"},
	{0 , "port_dbg sig3" , GENMASK(5,5) , "llm fifo init done"},
	{0 , "port_dbg sig3" , GENMASK(4,0) , "spl fifo cnt"},

	/* Bmu status info */
	{0x28000c , "llm buf used" , GENMASK(31,0) , "buffer used cnt"},

	{0x280010 , "llm mem sts" , GENMASK(31,1) , "fifo full error"},
	{0 , "llm mem sts" , GENMASK(0,0) , "llm fifo init done"},

	{0x280080 , "bmu dbg signal0" , GENMASK(31,11) , "single buf empty"},
	{0 , "bmu dbg signal0" , GENMASK(10,10) , "single buf full"},
	{0 , "bmu dbg signal0" , GENMASK(9,9) , "llm fifo empty"},
	{0 , "bmu dbg signal0" , GENMASK(8,8) , "llm ram full"},
	{0 , "bmu dbg signal0" , GENMASK(7,7) , "reclaim mem0 empty"},
	{0 , "bmu dbg signal0" , GENMASK(6,6) , "reclaim mem0 full"},
	{0 , "bmu dbg signal0" , GENMASK(5,3) , "reclaim mem0 write buf empty"},
	{0 , "bmu dbg signal0" , GENMASK(2,0) , "reclaim mem0 write buf full"},

	{0x280084 , "bmu dbg signal1" , GENMASK(31,15) , "pkt length read"},
	{0 , "bmu dbg signal1" , GENMASK(14,14) , "pkt is linklist"},
	{0 , "bmu dbg signal1" , GENMASK(13,0) , "pkt is ddr"},

	{0x280088 , "bmu dbg signal2" , GENMASK(31,10) , "llm fifo release winner"},
	{0 , "bmu dbg signal2" , GENMASK(9,9) , "reclaim mem0 empty"},
	{0 , "bmu dbg signal2" , GENMASK(8,8) , "reclaim mem0 rdata vld"},
	{0 , "bmu dbg signal2" , GENMASK(7,7) , "reclaim mem0 rd en"},
	{0 , "bmu dbg signal2" , GENMASK(6,6) , "llm fifo wr en"},
	{0 , "bmu dbg signal2" , GENMASK(5,5) , "linklist mem rdata vld"},
	{0 , "bmu dbg signal2" , GENMASK(4,4) , "linklist mem rdata en"},
	{0 , "bmu dbg signal2" , GENMASK(3,3) , "linklist mem rready"},
	{0 , "bmu dbg signal2" , GENMASK(2,0) , "reclaim state"},

	{0x28008c , "bmu dbg signal3" , GENMASK(31,9) , "single buf wr en"},
	{0 , "bmu dbg signal3" , GENMASK(8,8) , "single buf rd en"},
	{0 , "bmu dbg signal3" , GENMASK(7,7) , "llm fifo rd en"},
	{0 , "bmu dbg signal3" , GENMASK(6,6) , "llm fifo rr start"},
	{0 , "bmu dbg signal3" , GENMASK(5,5) , "reclaim mem0 wr en 0"},
	{0 , "bmu dbg signal3" , GENMASK(4,4) , "reclaim mem0 wr en 1"},
	{0 , "bmu dbg signal3" , GENMASK(3,3) , "reclaim mem0 wr en link"},
	{0 , "bmu dbg signal3" , GENMASK(2,2) , "llm fifo wr en1"},
	{0 , "bmu dbg signal3" , GENMASK(1,1) , "llm fifo wr en0"},
	{0 , "bmu dbg signal3" , GENMASK(0,0) , " llm fifo wr en"},

	{0x2800c0 , "bmu rd clr en" , GENMASK(31,0) , "bmu rd clr en"},

	{0x300000 , "cpu rdctrl" , GENMASK(31,4) , "cpu rd busy"},
	{0 , "cpu rdctrl" , GENMASK(3,3) , "cpu rd finish"},
	{0 , "cpu rdctrl" , GENMASK(2,0) , "cpu rd buf sel"},

	{0x300004 , "cpu rdata" , GENMASK(31,0) , "cpu rd data"},

	{0x300020 , "buf ctrl sts" , GENMASK(31,11) , "rcv wfifo full"},
	{0 , "buf ctrl sts" , GENMASK(10,10) , "tm wfifo full"},
	{0 , "buf ctrl sts" , GENMASK(9,9) , "parser rfifo full"},
	{0 , "buf ctrl sts" , GENMASK(8,8) , "tm rfifo full"},
	{0 , "buf ctrl sts" , GENMASK(7,7) , "modify rfifo full"},
	{0 , "buf ctrl sts" , GENMASK(6,6) , "bmu ptr rfifo full"},
	{0 , "buf ctrl sts" , GENMASK(5,5) , "rcv wfifo full"},
	{0 , "buf ctrl sts" , GENMASK(4,4) , "tm wfifo full"},
	{0 , "buf ctrl sts" , GENMASK(3,3) , "parser rfifo full"},
	{0 , "buf ctrl sts" , GENMASK(2,2) , "tm rfifo full"},
	{0 , "buf ctrl sts" , GENMASK(1,1) , "modify rfifo full"},
	{0 , "buf ctrl sts" , GENMASK(0,0) , "bmu ptr rfifo full"},
	/* Parser status info */
	{0x080000 , "parser sts" , GENMASK(31,31) , "mf fifo full latch"},
	{0 , "parser sts" , GENMASK(30,11) , "mf fifo prog full"},
	{0 , "parser sts" , GENMASK(10,10) , "mf fifo full"},
	{0 , "parser sts" , GENMASK(9,9) , "mf fifo prog empty"},
	{0 , "parser sts" , GENMASK(8,8) , "mf fifo empty"},
	{0 , "parser sts" , GENMASK(7,4) , "parser fsm next status"},
	{0 , "parser sts" , GENMASK(3,0) , "parser fsm cur status"},
	/* ivlan vid status info */
	{0x03801c , "ivlan vid sts" , GENMASK(31,0) , "ivlan vid status info"},
	/* ivlan lkp status info */
	{0x038020 , "ivlan lkp sts" , GENMASK(31,0) , "ivlan lkp status info"},
	/* L2 status info */
	{0x038024 , "L2_pp sts" , GENMASK(31,0) , "L2 status info"},
	/* nat status info */
	{0x03802c , "nat sts" , GENMASK(31,0) , "nat status info"},
	/* L3 status info */
	{0x038028 , "L3_pp sts" , GENMASK(31,0) , "L3 status info"},
	/* iacl status info */
	{0x038014 , "ical sts" , GENMASK(31,0) , "iacl status info"},
	/* Arp intf status info */
	{0x038000 , "arp intf sts" , GENMASK(31,9) , "free ptr fifo empty"},
	{0 , "arp intf sts" , GENMASK(8,8) , "free ptr fifo full"},
	{0 , "arp intf sts" , GENMASK(7,7) , "voq0 empty"},
	{0 , "arp intf sts" , GENMASK(6,6) , "voq0 full"},
	{0 , "arp intf sts" , GENMASK(5,5) , " voq1 empty"},
	{0 , "arp intf sts" , GENMASK(4,4) , "voq1 full"},
	{0 , "arp intf sts" , GENMASK(3,2) , "response fifo empty"},
	{0 , "arp intf sts" , GENMASK(1,0) , "response fifo full"},
	/* Evlan lkp status info */
	{0x038008 , "evlan lkp sts" , GENMASK(31,0) , "Evlan lkp status info"},
	/* Evlan xlt status info */
	{0x03800c , "evlan xlt sts" , GENMASK(31,0) , "Evlan xlt status info"},
	/* Evlan act status info */
	{0x038010 , "evlan act sts" , GENMASK(31,0) , "Evlan act status info"},
	/* eacl status info */
	{0x038004 , "eacl sts" , GENMASK(31,0) , "eacl status info"},

	/* Modify status info */
	{0x02801C , "modify sts low" , GENMASK(31,31) , "mbmu fifo full"},
	{0 , "modify sts low" , GENMASK(30,30) , "mbmu fifo empty"},
	{0 , "modify sts low" , GENMASK(29,23) , "voq6~voq0 empty"},
	{0 , "modify sts low" , GENMASK(22,16) , "voq6~voq0 full"},
	{0 , "modify sts low" , GENMASK(15,9) , "mfree4 full"},
	{0 , "modify sts low" , GENMASK(8,8) , "mfree4 empty"},
	{0 , "modify sts low" , GENMASK(7,7) , "mfree3 full"},
	{0 , "modify sts low" , GENMASK(6,6) , "mfree3 empty"},
	{0 , "modify sts low" , GENMASK(5,5) , "mfree2 full"},
	{0 , "modify sts low" , GENMASK(4,4) , "mfree2 empty"},
	{0 , "modify sts low" , GENMASK(3,3) , "mfree1 full"},
	{0 , "modify sts low" , GENMASK(2,2) , "mfree1 empty"},
	{0 , "modify sts low" , GENMASK(1,1) , "mfree0 full"},
	{0 , "modify sts low" , GENMASK(0,0) , "mfree0 empty"},

	{0x028020 , "modify sts mid" , GENMASK(31,19) , "msent2msch rdy"},
	{0 , "modify sts mid" , GENMASK(18,18) , "mem2md rdy"},
	{0 , "modify sts mid" , GENMASK(17,10) , "wait data back"},
	{0 , "modify sts mid" , GENMASK(9,9) , "rd q full"},
	{0 , "modify sts mid" , GENMASK(8,8) , "rd q empty"},
	{0 , "modify sts mid" , GENMASK(7,1) , "mf fifo full"},
	{0 , "modify sts mid" , GENMASK(0,0) , "mf fifo empty"},

	{0x028024 , "modify sts high" , GENMASK(31,26) , "msent6 port2md rdy"},
	{0 , "modify sts high" , GENMASK(25,24) , "msent6 fsm status"},
	{0 , "modify sts high" , GENMASK(23,22) , "msent5 port2md rdy"},
	{0 , "modify sts high" , GENMASK(21,20) , "msent5 fsm status"},
	{0 , "modify sts high" , GENMASK(19,18) , "msent4 port2md rdy"},
	{0 , "modify sts high" , GENMASK(17,16) , "msent4 fsm status"},
	{0 , "modify sts high" , GENMASK(15,14) , "msent3 port2md rdy"},
	{0 , "modify sts high" , GENMASK(13,12) , "msent3 fsm status"},
	{0 , "modify sts high" , GENMASK(11,10) , "msent2 port2md rdy"},
	{0 , "modify sts high" , GENMASK(9,8) , "msent2 fsm status"},
	{0 , "modify sts high" , GENMASK(7,6) , "msent1 port2md rdy"},
	{0 , "modify sts high" , GENMASK(5,4) , "msent1 fsm status"},
	{0 , "modify sts high" , GENMASK(3,2) , "msent0 port2md rdy"},
	{0x028024 , "modify sts high" , GENMASK(1,0) , "msent0 fsm status"},
	/* Mib status info */
	{0x383028 , "pls core cnt sts" , GENMASK(31,0) , "pulse core cnt fsmcs"},
	/* TODO:pls core cnt sts --> pls core nci sts? */
	{0x38302c , "pls core cnt sts" , GENMASK(31,0) , "pulse core nci fsmcs"},

	{0x383030 , "len core sts" , GENMASK(31,6) , "len core cnt fsmcs"},
	{0 , "len core sts" , GENMASK(5,0) , "len core nci fsmcs"},

	{0x383034 , "core ovf sts" , GENMASK(31,16) , "pulse core ovf"},
	{0 , "core ovf sts" , GENMASK(15,2) , "len core len ovf"},
	{0 , "core ovf sts" , GENMASK(1,0) , "len core pulse ovf"},
	// ======================== PART II  TMU status reg =============================

	{0x148014 , "tmu reclaim full max cnt" , GENMASK(31,0) , "reclaim full max cnt"},
	{0x148018 , "tmu reclaim full psg cnt" , GENMASK(31,0) , "reclaim full posedge cnt"},
	{0x14801c , "tmu reclaim full latch" , GENMASK(31,0) , "reclaim full latch"},
	// ======================== PART III  SE status reg =============================
	{0x180000 , "int_status_rgt" , GENMASK(31,14) , "evlan act sch ovf int"},
	{0 , "int_status_rgt" , GENMASK(13,13) , "l2 mf spl ovf int"},
	{0 , "int_status_rgt" , GENMASK(12,12) , "l2 mp sch ovf int"},
	{0 , "int_status_rgt" , GENMASK(11,11) , "mdf ovf int"},
	{0 , "int_status_rgt" , GENMASK(10,10) , "evlan xlt ovf int"},
	{0 , "int_status_rgt" , GENMASK(9,9) , "evlan lkp ovf int"},
	{0 , "int_status_rgt" , GENMASK(8,8) , "evlan sch ovf int"},
	{0 , "int_status_rgt" , GENMASK(7,7) , "mac sch ovf int"},
	{0 , "int_status_rgt" , GENMASK(6,6) , "mac spl ovf int"},
	{0 , "int_status_rgt" , GENMASK(5,5) , "mac speed lkp ovf int"},
	{0 , "int_status_rgt" , GENMASK(4,4) , "l2 lkp buf ovf int"},
	{0 , "int_status_rgt" , GENMASK(3,3) , "l2 lkp sch ovf int"},
	{0 , "int_status_rgt" , GENMASK(2,2) , "ivlan xlt ovf int"},
	{0 , "int_status_rgt" , GENMASK(1,1) , "ivlan lkp ovf int"},
	{0 , "int_status_rgt" , GENMASK(0,0) , "iport lkp spl ovf int"},

	{0x180004 , "clr ram ctrl rgt" , GENMASK(31,20) , "l2 uc port map tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(19,19) , "l2 mc flood speed limit tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(18,18) , "l2 mc port map tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(17,17) , "l2 iso tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(16,16) , "modify header tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(15,15) , "modify copy tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(14,14) , "evlan action tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(13,13) , "evlan lkp tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(12,12) , "evlan xlt tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(11,11) , "evlan port tpid tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(10,10) , "evlan out tpid tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(9,9) , "intf tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(8,8) , "mac speed limit tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(7,7) , "mac tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(6,6) , "l2 hash tb1 clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(5,5) , "l2 hash tb0 clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(4,4) , "ivlan lkp tb cl"},
	{0 , "clr ram ctrl rgt" , GENMASK(3,3) , "iport based vlan tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(2,2) , "ivlan xlt tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(1,1) , "iport tb clr"},
	{0 , "clr ram ctrl rgt" , GENMASK(0,0) , "iport speed limit tb clr"},

	{0x180030 , "l2 lkp req rgt" , GENMASK(31,16) , "l2 host lkp sch req"},
	{0 , "l2 lkp req rgt" , GENMASK(15,7) , "l2 host lkp req"},

	/* Se nat status info */
	{0x188000 , "nat status rgt" , GENMASK(31,30) , "nat speed limit bp ovf"},
	{0 , "nat status rgt" , GENMASK(29,29) , "ddr addr req fifo ovf"},
	{0 , "nat status rgt" , GENMASK(28,28) , "ddr data resp fifo ovf"},
	{0 , "nat status rgt" , GENMASK(27,27) , "ilkp result fifo ovf"},
	{0 , "nat status rgt" , GENMASK(26,26) , "key fifo ovf"},
	{0 , "nat status rgt" , GENMASK(25,25) , "ilkp busy bp ovf"},
	{0 , "nat status rgt" , GENMASK(24,24) , "frag fifo ovf"},
	{0 , "nat status rgt" , GENMASK(23,10) , "elkp fifo status"},
	{0 , "nat status rgt" , GENMASK(9,8) , "frag fifo status"},
	{0 , "nat status rgt" , GENMASK(7,0) , "host mgt reg busy"},

	{0x188024 , "nat cmd rgt" , GENMASK(31,10) , "nat route ip tb1 clr"},
	{0 , "nat cmd rgt" , GENMASK(9,9) , "nat route ip tb0 clr"},
	{0 , "nat cmd rgt" , GENMASK(8,8) , "nat spl tb0 clr"},
	{0 , "nat cmd rgt" , GENMASK(7,7) , "napt all tb clr"},
	{0 , "nat cmd rgt" , GENMASK(6,6) , "napt tb1 clr"},
	{0 , "nat cmd rgt" , GENMASK(5,5) , "napt tb0 clr"},
	{0 , "nat cmd rgt" , GENMASK(4,4) , "snat hash tb1 clr"},
	{0 , "nat cmd rgt" , GENMASK(3,3) , "snat hash tb0 clr"},
	{0 , "nat cmd rgt" , GENMASK(2,2) , "dnat hash tb1 clr"},
	{0 , "nat cmd rgt" , GENMASK(1,1) , "dnat hash tb0 clr"},
	{0 , "nat cmd rgt" , GENMASK(0,0) , "frag init"},

	{0x188038 , "nat lkp req rgt" , GENMASK(31,16) , "host lkp sch req"},
	{0 , "nat lkp req rgt" , GENMASK(15,7) , "host lkp req"},

	/* Tcam status info */
	{0x190000 , "tcam status rgt" , GENMASK(31,6) , "acl spl bp ovf"},
	{0 , "tcam status rgt" , GENMASK(5,5) , "acl sch bp ovf"},
	{0 , "tcam status rgt" , GENMASK(4,4) , "tcam p4 bp ovf"},
	{0 , "tcam status rgt" , GENMASK(3,3) , "tcam p3 bp ovf"},
	{0 , "tcam status rgt" , GENMASK(2,2) , "tcam p2 bp ovf"},
	{0 , "tcam status rgt" , GENMASK(1,1) , "tcam p1 bp ovf"},
	{0 , "tcam status rgt" , GENMASK(0,0) , "tcam p0 bp ovf"},

	/* register address */
	{0x190004 , "tcam clr rgt" , GENMASK(31,5) , "acl spl tb clr"},
	{0 , "tcam clr rgt" , GENMASK(4,4) , "tcam blk4 tb clr"},
	{0 , "tcam clr rgt" , GENMASK(3,3) , "tcam blk3 tb clr"},
	{0 , "tcam clr rgt" , GENMASK(2,2) , "tcam blk2 tb clr"},
	{0 , "tcam clr rgt" , GENMASK(1,1) , "tcam blk1 tb clr"},
	{0 , "tcam clr rgt" , GENMASK(0,0) , "tcam blk0 tb clr"}
};

static int
common_genl_msg_recv(struct genl_info *info, void *buf, size_t buflen)
{

	struct dpns_common_priv *priv = g_priv;
	struct common_genl_msg *msg = buf;
	int err = 0;
        u32 i, j, k, shift, addr, value;
	u32 tmu_status_cnt, npu_status_cnt, index = 0;
	u32 *status_val;

	char smac[ETH_ALEN];

	MAC_t *mac_priv = priv->mac_priv;
	u32 port_bitmap, offload_bitmap;
	u8 iport_num, module_num, log_level;

	struct xgmac_dma_priv *dma_priv = priv->edma_priv;

	if(WARN_ON_ONCE(!priv))
		return -EBUSY;

	switch (msg->method) {
		case DEBUG_DUMP:
                        tmu_status_cnt = ARRAY_SIZE(port_tmu_status) * DPNS_MAX_PORT;
                        npu_status_cnt = tmu_status_cnt + ARRAY_SIZE(npu_status);
                        status_val = kzalloc(sizeof(u32)*npu_status_cnt, GFP_KERNEL);
                        if (!status_val)
                                break;

                        for (i = 0; i < npu_status_cnt; i++) {
                                if (npu_status[i - index].addr == 0x148014) {
                                        for (j = 0; j < DPNS_MAX_PORT; j++) {
                                                for (k = 0; k < ARRAY_SIZE(port_tmu_status); k++) {
                                                        if (port_tmu_status[k].addr != 0) {
                                                                addr = port_tmu_status[k].addr + 0x2000 * j;
                                                                value = sf_readl(priv, addr);
                                                        }
                                                        shift = ffs(port_tmu_status[k].mask) - 1;
                                                        status_val[i++] = (port_tmu_status[k].mask & value) >> shift;
                                                }
                                        }
                                        index = tmu_status_cnt;
                                }

                                if (npu_status[i - index].addr != 0)
                                        value = sf_readl(priv, npu_status[i - index].addr);

                                shift = ffs(npu_status[i - index].mask) - 1;
                                status_val[i] = (npu_status[i - index].mask & value) >> shift;
                        }


                        index = 0;
                        for (i = 0; i < ARRAY_SIZE(npu_status); i++) {
                                if (npu_status[i].addr == 0x200014) {
                                        printk("============ PART I NPU status ============\n");
                                } else if (npu_status[i].addr == 0x180000) {
                                        printk("============ PART III SE status ============\n");
                                }

                                if (npu_status[i].addr == 0x148014) {
                                        printk("============ PART II TMU status ============\n");
                                        for (j = 0; j < DPNS_MAX_PORT; j++) {
                                                for (k = 0; k < ARRAY_SIZE(port_tmu_status); k++) {
                                                        addr = port_tmu_status[k].addr + 0x2000 * j;
                                                        if (port_tmu_status[k].addr != 0) {
                                                                printk("Addr:0x%06x RegName:%s%02u%-24s Mask:0x%08x DEsc:%-32s Value:0x%x\n",
                                                                                addr, "port", j,  port_tmu_status[k].reg_name, port_tmu_status[k].mask,
                                                                                port_tmu_status[k].desc, status_val[index++]);
                                                        }else {
                                                                printk("%-52s Mask:0x%08x Desc:%-32s Value:0x%x\n",
                                                                                "", port_tmu_status[k].mask,
                                                                                port_tmu_status[k].desc, status_val[index++]);
                                                        }
                                                }
                                        }
                                }

                                if (npu_status[i].addr != 0) {
                                        printk("Addr:0x%06x RegName:%-30s Mask:0x%08x DEsc:%-32s Value:0x%x\n",
                                                        npu_status[i].addr, npu_status[i].reg_name, npu_status[i].mask,
                                                        npu_status[i].desc, status_val[index++]);
                                }else {
                                        printk("%-52s Mask:0x%08x DEsc:%-32s Value:0x%x\n",
                                                        " ", npu_status[i].mask,
                                                        npu_status[i].desc, status_val[index++]);
                                }
                        }

                        kfree(status_val);
			break;
		case INTF_ADD:
			u64_to_ether_addr(msg->smac, smac);

			priv->intf_add(priv, msg->vid, msg->pppoe_en, msg->tunnel_en, msg->wan_flag, smac);
			break;
		case INTF_DEL:
			priv->intf_del(priv, msg->index);
			break;
		case ISO_SET:
			iport_num = msg->iport_num;
			port_bitmap = msg->port_bitmap;
			offload_bitmap = msg->offload_bitmap;
			mac_priv->iso_table_update(mac_priv, iport_num,
				port_bitmap, offload_bitmap);
			break;
		case LOG_SET:
			module_num = msg->module_num;
			log_level = msg->log_level;
			g_dbg_log[module_num] = log_level;
			break;
		case INTF_DUMP:
			dump_intf_table(priv);
			break;
		case ISO_DUMP:
			mac_priv->iso_table_dump(mac_priv, msg->iport_num);
			break;
		case LOG_DUMP:
			printk("module_num:0-DPNS_COMMON\t1-DPNS_GENL\t2-DPNS_VLAN\t3-DPNS_L2\n"
					"4-DPNS_NAT\t5-DPNS_L3\t6-DPNS_ACL\t7-DPNS_TMU\t8-DPNS_MCAST\n");
			for (i = 0; i < DPNS_MAX; i++) {
				printk("dpns module:%u level:%u\n",
							   i,g_dbg_log[i]);
			}
			break;
		case MIB_DUMP:
			dpns_read_npu_mib(priv);
			break;
		case DEV_DUMP:
			for (i = 0; i < DPNS_MAX_PORT; i++) {
				if (dma_priv->ndevs[i]) {
					struct dpns_port_vlan_info *pos;
					dpns_port_t *dp_port;
					dp_port = dpns_port_by_netdev(priv, dma_priv->ndevs[i]);
					if (!dp_port)
						return 0;

					printk("dpns hook port id:%u dev name:%s ref_count:%u\n",
						   i, dma_priv->ndevs[i]->name, dp_port->ref_count);

					spin_lock_bh(&dp_port->lock);
					list_for_each_entry(pos, &dp_port->vlan_list, node) {
						printk("dpns hook port id:%u dev name:%s\n",
								i, pos->dev->name);
					}
					spin_unlock_bh(&dp_port->lock);
				}
			}
			break;
		case MEM_DUMP:
			dump_dpns_mem_info();
			break;
		default:
			err = -EINVAL;
	}


	sfgenl_msg_reply(info, &err, sizeof(err));

	return err;
}


static struct sfgenl_msg_ops common_genl_msg_ops = {
	.msg_recv = common_genl_msg_recv,
};

int dpns_common_genl_init(struct dpns_common_priv *priv)
{
	g_priv = priv;
	return sfgenl_ops_register(SF_GENL_COMP_COMMON, &common_genl_msg_ops);
}

int dpns_common_genl_exit(void)
{
	return sfgenl_msg_ops_unregister(SF_GENL_COMP_COMMON);
}
