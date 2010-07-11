/*
 *
 * ETRAX 100LX USB Host Controller Driver
 *
 * Copyright (C) 2005 - 2008  Axis Communications AB
 *
 * Author: Konrad Eriksson <konrad.eriksson@axis.se>
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <linux/usb.h>
#include <linux/platform_device.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/arch/dma.h>
#include <asm/arch/io_interface_mux.h>

#include "../core/hcd.h"
#include "../core/hub.h"
#include "hc-crisv10.h"
#include "hc-cris-dbg.h"


/***************************************************************************/
/***************************************************************************/
/* Host Controller settings                                                */
/***************************************************************************/
/***************************************************************************/

#define VERSION			"1.00-openwrt_diff-v1"
#define COPYRIGHT		"(c) 2005, 2006 Axis Communications AB"
#define DESCRIPTION             "ETRAX 100LX USB Host Controller"

#define ETRAX_USB_HC_IRQ USB_HC_IRQ_NBR
#define ETRAX_USB_RX_IRQ USB_DMA_RX_IRQ_NBR
#define ETRAX_USB_TX_IRQ USB_DMA_TX_IRQ_NBR

/* Number of physical ports in Etrax 100LX */
#define USB_ROOT_HUB_PORTS 2

const char hc_name[] = "hc-crisv10";
const char product_desc[] = DESCRIPTION;

/* The number of epids is, among other things, used for pre-allocating
   ctrl, bulk and isoc EP descriptors (one for each epid).
   Assumed to be > 1 when initiating the DMA lists. */
#define NBR_OF_EPIDS       32

/* Support interrupt traffic intervals up to 128 ms. */
#define MAX_INTR_INTERVAL  128

/* If periodic traffic (intr or isoc) is to be used, then one entry in the EP
   table must be "invalid". By this we mean that we shouldn't care about epid
   attentions for this epid, or at least handle them differently from epid
   attentions for "valid" epids. This define determines which one to use
   (don't change it). */
#define INVALID_EPID       31
/* A special epid for the bulk dummys. */
#define DUMMY_EPID         30

/* Module settings */

MODULE_DESCRIPTION(DESCRIPTION);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Konrad Eriksson <konrad.eriksson@axis.se>");


/* Module parameters */

/* 0 = No ports enabled
   1 = Only port 1 enabled (on board ethernet on devboard)
   2 = Only port 2 enabled (external connector on devboard)
   3 = Both ports enabled
*/
static unsigned int ports = 3;
module_param(ports, uint, S_IRUGO);
MODULE_PARM_DESC(ports, "Bitmask indicating USB ports to use");


/***************************************************************************/
/***************************************************************************/
/* Shared global variables for this module                                 */
/***************************************************************************/
/***************************************************************************/

/* EP descriptor lists for non period transfers. Must be 32-bit aligned. */
static volatile struct USB_EP_Desc TxBulkEPList[NBR_OF_EPIDS] __attribute__ ((aligned (4)));

static volatile struct USB_EP_Desc TxCtrlEPList[NBR_OF_EPIDS] __attribute__ ((aligned (4)));

/* EP descriptor lists for period transfers. Must be 32-bit aligned. */
static volatile struct USB_EP_Desc TxIntrEPList[MAX_INTR_INTERVAL] __attribute__ ((aligned (4)));
static volatile struct USB_SB_Desc TxIntrSB_zout __attribute__ ((aligned (4)));

static volatile struct USB_EP_Desc TxIsocEPList[NBR_OF_EPIDS] __attribute__ ((aligned (4)));
static volatile struct USB_SB_Desc TxIsocSB_zout __attribute__ ((aligned (4)));

static volatile struct USB_SB_Desc TxIsocSBList[NBR_OF_EPIDS] __attribute__ ((aligned (4))); 

/* After each enabled bulk EP IN we put two disabled EP descriptors with the eol flag set,
   causing the DMA to stop the DMA channel. The first of these two has the intr flag set, which
   gives us a dma8_sub0_descr interrupt. When we receive this, we advance the DMA one step in the
   EP list and then restart the bulk channel, thus forcing a switch between bulk EP descriptors
   in each frame. */
static volatile struct USB_EP_Desc TxBulkDummyEPList[NBR_OF_EPIDS][2] __attribute__ ((aligned (4)));

/* List of URB pointers, where each points to the active URB for a epid.
   For Bulk, Ctrl and Intr this means which URB that currently is added to
   DMA lists (Isoc URBs are all directly added to DMA lists). As soon as
   URB has completed is the queue examined and the first URB in queue is
   removed and moved to the activeUrbList while its state change to STARTED and
   its transfer(s) gets added to DMA list (exception Isoc where URBs enter
   state STARTED directly and added transfers added to DMA lists). */
static struct urb *activeUrbList[NBR_OF_EPIDS];

/* Additional software state info for each epid */
static struct etrax_epid epid_state[NBR_OF_EPIDS];

/* Timer handles for bulk traffic timer used to avoid DMA bug where DMA stops
   even if there is new data waiting to be processed */
static struct timer_list bulk_start_timer = TIMER_INITIALIZER(NULL, 0, 0);
static struct timer_list bulk_eot_timer = TIMER_INITIALIZER(NULL, 0, 0);

/* We want the start timer to expire before the eot timer, because the former
   might start traffic, thus making it unnecessary for the latter to time
   out. */
#define BULK_START_TIMER_INTERVAL (HZ/50) /* 20 ms */
#define BULK_EOT_TIMER_INTERVAL (HZ/16) /* 60 ms */

/* Delay before a URB completion happen when it's scheduled to be delayed */
#define LATER_TIMER_DELAY (HZ/50) /* 20 ms */

/* Simplifying macros for checking software state info of a epid */
/* ----------------------------------------------------------------------- */
#define epid_inuse(epid)       epid_state[epid].inuse
#define epid_out_traffic(epid) epid_state[epid].out_traffic
#define epid_isoc(epid)   (epid_state[epid].type == PIPE_ISOCHRONOUS ? 1 : 0)
#define epid_intr(epid)   (epid_state[epid].type == PIPE_INTERRUPT ? 1 : 0)


/***************************************************************************/
/***************************************************************************/
/* DEBUG FUNCTIONS                                                         */
/***************************************************************************/
/***************************************************************************/
/* Note that these functions are always available in their "__" variants,
   for use in error situations. The "__" missing variants are controlled by
   the USB_DEBUG_DESC/USB_DEBUG_URB macros. */
static void __dump_urb(struct urb* purb)
{
  struct crisv10_urb_priv *urb_priv = purb->hcpriv;
  int urb_num = -1;
  if(urb_priv) {
    urb_num = urb_priv->urb_num;
  }
  printk("\nURB:0x%x[%d]\n", (unsigned int)purb, urb_num);
  printk("dev                   :0x%08lx\n", (unsigned long)purb->dev);
  printk("pipe                  :0x%08x\n", purb->pipe);
  printk("status                :%d\n", purb->status);
  printk("transfer_flags        :0x%08x\n", purb->transfer_flags);
  printk("transfer_buffer       :0x%08lx\n", (unsigned long)purb->transfer_buffer);
  printk("transfer_buffer_length:%d\n", purb->transfer_buffer_length);
  printk("actual_length         :%d\n", purb->actual_length);
  printk("setup_packet          :0x%08lx\n", (unsigned long)purb->setup_packet);
  printk("start_frame           :%d\n", purb->start_frame);
  printk("number_of_packets     :%d\n", purb->number_of_packets);
  printk("interval              :%d\n", purb->interval);
  printk("error_count           :%d\n", purb->error_count);
  printk("context               :0x%08lx\n", (unsigned long)purb->context);
  printk("complete              :0x%08lx\n\n", (unsigned long)purb->complete);
}

static void __dump_in_desc(volatile struct USB_IN_Desc *in)
{
  printk("\nUSB_IN_Desc at 0x%08lx\n", (unsigned long)in);
  printk("  sw_len  : 0x%04x (%d)\n", in->sw_len, in->sw_len);
  printk("  command : 0x%04x\n", in->command);
  printk("  next    : 0x%08lx\n", in->next);
  printk("  buf     : 0x%08lx\n", in->buf);
  printk("  hw_len  : 0x%04x (%d)\n", in->hw_len, in->hw_len);
  printk("  status  : 0x%04x\n\n", in->status);
}

static void __dump_sb_desc(volatile struct USB_SB_Desc *sb)
{
  char tt = (sb->command & 0x30) >> 4;
  char *tt_string;

  switch (tt) {
  case 0:
    tt_string = "zout";
    break;
  case 1:
    tt_string = "in";
    break;
  case 2:
    tt_string = "out";
    break;
  case 3:
    tt_string = "setup";
    break;
  default:
    tt_string = "unknown (weird)";
  }

  printk(" USB_SB_Desc at 0x%08lx ", (unsigned long)sb);
  printk(" command:0x%04x (", sb->command);
  printk("rem:%d ", (sb->command & 0x3f00) >> 8);
  printk("full:%d ", (sb->command & 0x40) >> 6);
  printk("tt:%d(%s) ", tt, tt_string);
  printk("intr:%d ", (sb->command & 0x8) >> 3);
  printk("eot:%d ", (sb->command & 0x2) >> 1);
  printk("eol:%d)", sb->command & 0x1);
  printk(" sw_len:0x%04x(%d)", sb->sw_len, sb->sw_len);
  printk(" next:0x%08lx", sb->next);
  printk(" buf:0x%08lx\n", sb->buf);
}


static void __dump_ep_desc(volatile struct USB_EP_Desc *ep)
{
  printk("USB_EP_Desc at 0x%08lx ", (unsigned long)ep);
  printk(" command:0x%04x (", ep->command);
  printk("ep_id:%d ", (ep->command & 0x1f00) >> 8);
  printk("enable:%d ", (ep->command & 0x10) >> 4);
  printk("intr:%d ", (ep->command & 0x8) >> 3);
  printk("eof:%d ", (ep->command & 0x2) >> 1);
  printk("eol:%d)", ep->command & 0x1);
  printk(" hw_len:0x%04x(%d)", ep->hw_len, ep->hw_len);
  printk(" next:0x%08lx", ep->next);
  printk(" sub:0x%08lx\n", ep->sub);
}

static inline void __dump_ep_list(int pipe_type)
{
  volatile struct USB_EP_Desc *ep;
  volatile struct USB_EP_Desc *first_ep;
  volatile struct USB_SB_Desc *sb;

  switch (pipe_type)
    {
    case PIPE_BULK:
      first_ep = &TxBulkEPList[0];
      break;
    case PIPE_CONTROL:
      first_ep = &TxCtrlEPList[0];
      break;
    case PIPE_INTERRUPT:
      first_ep = &TxIntrEPList[0];
      break;
    case PIPE_ISOCHRONOUS:
      first_ep = &TxIsocEPList[0];
      break;
    default:
      return;
    }
  ep = first_ep;

  printk("\n\nDumping EP list...\n\n");

  do {
    __dump_ep_desc(ep);
    /* Cannot phys_to_virt on 0 as it turns into 80000000, which is != 0. */
    sb = ep->sub ? phys_to_virt(ep->sub) : 0;
    while (sb) {
      __dump_sb_desc(sb);
      sb = sb->next ? phys_to_virt(sb->next) : 0;
    }
    ep = (volatile struct USB_EP_Desc *)(phys_to_virt(ep->next));

  } while (ep != first_ep);
}

static inline void __dump_ept_data(int epid)
{
  unsigned long flags;
  __u32 r_usb_ept_data;

  if (epid < 0 || epid > 31) {
    printk("Cannot dump ept data for invalid epid %d\n", epid);
    return;
  }

  local_irq_save(flags);
  *R_USB_EPT_INDEX = IO_FIELD(R_USB_EPT_INDEX, value, epid);
  nop();
  r_usb_ept_data = *R_USB_EPT_DATA;
  local_irq_restore(flags);

  printk(" R_USB_EPT_DATA = 0x%x for epid %d :\n", r_usb_ept_data, epid);
  if (r_usb_ept_data == 0) {
    /* No need for more detailed printing. */
    return;
  }
  printk("  valid           : %d\n", (r_usb_ept_data & 0x80000000) >> 31);
  printk("  hold            : %d\n", (r_usb_ept_data & 0x40000000) >> 30);
  printk("  error_count_in  : %d\n", (r_usb_ept_data & 0x30000000) >> 28);
  printk("  t_in            : %d\n", (r_usb_ept_data & 0x08000000) >> 27);
  printk("  low_speed       : %d\n", (r_usb_ept_data & 0x04000000) >> 26);
  printk("  port            : %d\n", (r_usb_ept_data & 0x03000000) >> 24);
  printk("  error_code      : %d\n", (r_usb_ept_data & 0x00c00000) >> 22);
  printk("  t_out           : %d\n", (r_usb_ept_data & 0x00200000) >> 21);
  printk("  error_count_out : %d\n", (r_usb_ept_data & 0x00180000) >> 19);
  printk("  max_len         : %d\n", (r_usb_ept_data & 0x0003f800) >> 11);
  printk("  ep              : %d\n", (r_usb_ept_data & 0x00000780) >> 7);
  printk("  dev             : %d\n", (r_usb_ept_data & 0x0000003f));
}

static inline void __dump_ept_data_iso(int epid)
{
  unsigned long flags;
  __u32 ept_data;

  if (epid < 0 || epid > 31) {
    printk("Cannot dump ept data for invalid epid %d\n", epid);
    return;
  }

  local_irq_save(flags);
  *R_USB_EPT_INDEX = IO_FIELD(R_USB_EPT_INDEX, value, epid);
  nop();
  ept_data = *R_USB_EPT_DATA_ISO;
  local_irq_restore(flags);

  printk(" R_USB_EPT_DATA = 0x%x for epid %d :\n", ept_data, epid);
  if (ept_data == 0) {
    /* No need for more detailed printing. */
    return;
  }
  printk("  valid           : %d\n", IO_EXTRACT(R_USB_EPT_DATA_ISO, valid,
						ept_data));
  printk("  port            : %d\n", IO_EXTRACT(R_USB_EPT_DATA_ISO, port,
						ept_data));
  printk("  error_code      : %d\n", IO_EXTRACT(R_USB_EPT_DATA_ISO, error_code,
						ept_data));
  printk("  max_len         : %d\n", IO_EXTRACT(R_USB_EPT_DATA_ISO, max_len,
						ept_data));
  printk("  ep              : %d\n", IO_EXTRACT(R_USB_EPT_DATA_ISO, ep,
						ept_data));
  printk("  dev             : %d\n", IO_EXTRACT(R_USB_EPT_DATA_ISO, dev,
						ept_data));
}

static inline void __dump_ept_data_list(void)
{
  int i;

  printk("Dumping the whole R_USB_EPT_DATA list\n");

  for (i = 0; i < 32; i++) {
    __dump_ept_data(i);
  }
}

static void debug_epid(int epid) {
  int i;
  
  if(epid_isoc(epid)) {
    __dump_ept_data_iso(epid);
  } else {
    __dump_ept_data(epid);
  }

  printk("Bulk:\n");
  for(i = 0; i < 32; i++) {
    if(IO_EXTRACT(USB_EP_command, epid, TxBulkEPList[i].command) ==
       epid) {
      printk("%d: ", i); __dump_ep_desc(&(TxBulkEPList[i]));
    }
  }

  printk("Ctrl:\n");
  for(i = 0; i < 32; i++) {
    if(IO_EXTRACT(USB_EP_command, epid, TxCtrlEPList[i].command) ==
       epid) {
      printk("%d: ", i); __dump_ep_desc(&(TxCtrlEPList[i]));
    }
  }

  printk("Intr:\n");
  for(i = 0; i < MAX_INTR_INTERVAL; i++) {
    if(IO_EXTRACT(USB_EP_command, epid, TxIntrEPList[i].command) ==
       epid) {
      printk("%d: ", i); __dump_ep_desc(&(TxIntrEPList[i]));
    }
  }
  
  printk("Isoc:\n");
  for(i = 0; i < 32; i++) {
    if(IO_EXTRACT(USB_EP_command, epid, TxIsocEPList[i].command) ==
       epid) {
      printk("%d: ", i); __dump_ep_desc(&(TxIsocEPList[i]));
    }
  }

  __dump_ept_data_list();
  __dump_ep_list(PIPE_INTERRUPT);
  printk("\n\n");
}



char* hcd_status_to_str(__u8 bUsbStatus) {
  static char hcd_status_str[128];
  hcd_status_str[0] = '\0';
  if(bUsbStatus & IO_STATE(R_USB_STATUS, ourun, yes)) {
    strcat(hcd_status_str, "ourun ");
  }
  if(bUsbStatus & IO_STATE(R_USB_STATUS, perror, yes)) {
    strcat(hcd_status_str, "perror ");
  }
  if(bUsbStatus & IO_STATE(R_USB_STATUS, device_mode, yes)) {
    strcat(hcd_status_str, "device_mode ");
  }
  if(bUsbStatus & IO_STATE(R_USB_STATUS, host_mode, yes)) {
    strcat(hcd_status_str, "host_mode ");
  }
  if(bUsbStatus & IO_STATE(R_USB_STATUS, started, yes)) {
    strcat(hcd_status_str, "started ");
  }
  if(bUsbStatus & IO_STATE(R_USB_STATUS, running, yes)) {
    strcat(hcd_status_str, "running ");
  }
  return hcd_status_str;
}


char* sblist_to_str(struct USB_SB_Desc* sb_desc) {
  static char sblist_to_str_buff[128];
  char tmp[32], tmp2[32];
  sblist_to_str_buff[0] = '\0';
  while(sb_desc != NULL) {
    switch(IO_EXTRACT(USB_SB_command, tt, sb_desc->command)) {
    case 0: sprintf(tmp, "zout");  break;
    case 1: sprintf(tmp, "in");    break;
    case 2: sprintf(tmp, "out");   break;
    case 3: sprintf(tmp, "setup"); break;
    }
    sprintf(tmp2, "(%s %d)", tmp, sb_desc->sw_len);
    strcat(sblist_to_str_buff, tmp2);
    if(sb_desc->next != 0) {
      sb_desc = phys_to_virt(sb_desc->next);
    } else {
      sb_desc = NULL;
    }
  }
  return sblist_to_str_buff;
}

char* port_status_to_str(__u16 wPortStatus) {
  static char port_status_str[128];
  port_status_str[0] = '\0';
  if(wPortStatus & IO_STATE(R_USB_RH_PORT_STATUS_1, connected, yes)) {
    strcat(port_status_str, "connected ");
  }
  if(wPortStatus & IO_STATE(R_USB_RH_PORT_STATUS_1, enabled, yes)) {
    strcat(port_status_str, "enabled ");
  }
  if(wPortStatus & IO_STATE(R_USB_RH_PORT_STATUS_1, suspended, yes)) {
    strcat(port_status_str, "suspended ");
  }
  if(wPortStatus & IO_STATE(R_USB_RH_PORT_STATUS_1, reset, yes)) {
    strcat(port_status_str, "reset ");
  }
  if(wPortStatus & IO_STATE(R_USB_RH_PORT_STATUS_1, speed, full)) {
    strcat(port_status_str, "full-speed ");
  } else {
    strcat(port_status_str, "low-speed ");
  }
  return port_status_str;
}


char* endpoint_to_str(struct usb_endpoint_descriptor *ed) {
  static char endpoint_to_str_buff[128];
  char tmp[32];
  int epnum = ed->bEndpointAddress & 0x0F;
  int dir = ed->bEndpointAddress & 0x80;
  int type = ed->bmAttributes & 0x03;
  endpoint_to_str_buff[0] = '\0';
  sprintf(endpoint_to_str_buff, "ep:%d ", epnum);
  switch(type) {
  case 0:
    sprintf(tmp, " ctrl");
    break;
  case 1:
    sprintf(tmp, " isoc");
    break;
  case 2:
    sprintf(tmp, " bulk");
    break;
  case 3:
    sprintf(tmp, " intr");
    break;
  }
  strcat(endpoint_to_str_buff, tmp);
  if(dir) {
    sprintf(tmp, " in");
  } else {
    sprintf(tmp, " out");
  }
  strcat(endpoint_to_str_buff, tmp);

  return endpoint_to_str_buff;
}

/* Debug helper functions for Transfer Controller */
char* pipe_to_str(unsigned int pipe) {
  static char pipe_to_str_buff[128];
  char tmp[64];
  sprintf(pipe_to_str_buff, "dir:%s", str_dir(pipe));
  sprintf(tmp, " type:%s", str_type(pipe));
  strcat(pipe_to_str_buff, tmp);

  sprintf(tmp, " dev:%d", usb_pipedevice(pipe));
  strcat(pipe_to_str_buff, tmp);
  sprintf(tmp, " ep:%d", usb_pipeendpoint(pipe));
  strcat(pipe_to_str_buff, tmp);
  return pipe_to_str_buff;
}


#define USB_DEBUG_DESC 1

#ifdef USB_DEBUG_DESC
#define dump_in_desc(x) __dump_in_desc(x)
#define dump_sb_desc(...) __dump_sb_desc(...)
#define dump_ep_desc(x) __dump_ep_desc(x)
#define dump_ept_data(x) __dump_ept_data(x)
#else
#define dump_in_desc(...) do {} while (0)
#define dump_sb_desc(...) do {} while (0)
#define dump_ep_desc(...) do {} while (0)
#endif


/* Uncomment this to enable massive function call trace
   #define USB_DEBUG_TRACE */

#ifdef USB_DEBUG_TRACE
#define DBFENTER (printk(": Entering: %s\n", __FUNCTION__))
#define DBFEXIT  (printk(": Exiting:  %s\n", __FUNCTION__))
#else
#define DBFENTER do {} while (0)
#define DBFEXIT  do {} while (0)
#endif

#define CHECK_ALIGN(x) if (((__u32)(x)) & 0x00000003) \
{panic("Alignment check (DWORD) failed at %s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);}

/* Most helpful debugging aid */
#define ASSERT(expr) ((void) ((expr) ? 0 : (err("assert failed at: %s %d",__FUNCTION__, __LINE__))))


/***************************************************************************/
/***************************************************************************/
/* Forward declarations                                                    */
/***************************************************************************/
/***************************************************************************/
void crisv10_hcd_epid_attn_irq(struct crisv10_irq_reg *reg);
void crisv10_hcd_port_status_irq(struct crisv10_irq_reg *reg);
void crisv10_hcd_ctl_status_irq(struct crisv10_irq_reg *reg);
void crisv10_hcd_isoc_eof_irq(struct crisv10_irq_reg *reg);

void rh_port_status_change(__u16[]);
int  rh_clear_port_feature(__u8, __u16);
int  rh_set_port_feature(__u8, __u16);
static void rh_disable_port(unsigned int port);

static void check_finished_bulk_tx_epids(struct usb_hcd *hcd,
					 int timer);

static int  tc_setup_epid(struct usb_host_endpoint *ep, struct urb *urb,
			 int mem_flags);
static void tc_free_epid(struct usb_host_endpoint *ep);
static int  tc_allocate_epid(void);
static void tc_finish_urb(struct usb_hcd *hcd, struct urb *urb, int status);
static void tc_finish_urb_later(struct usb_hcd *hcd, struct urb *urb,
				int status);

static int  urb_priv_create(struct usb_hcd *hcd, struct urb *urb, int epid,
			   int mem_flags);
static void urb_priv_free(struct usb_hcd *hcd, struct urb *urb);

static int crisv10_usb_check_bandwidth(struct usb_device *dev,struct urb *urb);
static void crisv10_usb_claim_bandwidth(
	struct usb_device *dev, struct urb *urb, int bustime, int isoc);
static void crisv10_usb_release_bandwidth(
	struct usb_hcd *hcd, int isoc, int bandwidth);

static inline struct urb *urb_list_first(int epid);
static inline void        urb_list_add(struct urb *urb, int epid,
				      int mem_flags);
static inline urb_entry_t *urb_list_entry(struct urb *urb, int epid);
static inline void        urb_list_del(struct urb *urb, int epid);
static inline void        urb_list_move_last(struct urb *urb, int epid);
static inline struct urb *urb_list_next(struct urb *urb, int epid);

int create_sb_for_urb(struct urb *urb, int mem_flags);
int init_intr_urb(struct urb *urb, int mem_flags);

static inline void  etrax_epid_set(__u8 index, __u32 data);
static inline void  etrax_epid_clear_error(__u8 index);
static inline void  etrax_epid_set_toggle(__u8 index, __u8 dirout,
					      __u8 toggle);
static inline __u8  etrax_epid_get_toggle(__u8 index, __u8 dirout);
static inline __u32 etrax_epid_get(__u8 index);

/* We're accessing the same register position in Etrax so
   when we do full access the internal difference doesn't matter */
#define etrax_epid_iso_set(index, data) etrax_epid_set(index, data)
#define etrax_epid_iso_get(index) etrax_epid_get(index)


static void        tc_dma_process_isoc_urb(struct urb *urb);
static void        tc_dma_process_queue(int epid);
static void        tc_dma_unlink_intr_urb(struct urb *urb);
static irqreturn_t tc_dma_tx_interrupt(int irq, void *vhc);
static irqreturn_t tc_dma_rx_interrupt(int irq, void *vhc);

static void tc_bulk_start_timer_func(unsigned long dummy);
static void tc_bulk_eot_timer_func(unsigned long dummy);


/*************************************************************/
/*************************************************************/
/* Host Controler Driver block                               */
/*************************************************************/
/*************************************************************/

/* HCD operations */
static irqreturn_t crisv10_hcd_top_irq(int irq, void*);
static int crisv10_hcd_reset(struct usb_hcd *);
static int crisv10_hcd_start(struct usb_hcd *);
static void crisv10_hcd_stop(struct usb_hcd *);
#ifdef CONFIG_PM
static int crisv10_hcd_suspend(struct device *, u32, u32);
static int crisv10_hcd_resume(struct device *, u32);
#endif /* CONFIG_PM */
static int crisv10_hcd_get_frame(struct usb_hcd *);

static int  tc_urb_enqueue(struct usb_hcd *, struct urb *, gfp_t mem_flags);
static int  tc_urb_dequeue(struct usb_hcd *, struct urb *, int);
static void tc_endpoint_disable(struct usb_hcd *, struct usb_host_endpoint *ep);

static int rh_status_data_request(struct usb_hcd *, char *);
static int rh_control_request(struct usb_hcd *, u16, u16, u16, char*, u16);

#ifdef CONFIG_PM
static int crisv10_hcd_hub_suspend(struct usb_hcd *);
static int crisv10_hcd_hub_resume(struct usb_hcd *);
#endif /* CONFIG_PM */
#ifdef CONFIG_USB_OTG
static int crisv10_hcd_start_port_reset(struct usb_hcd *, unsigned);
#endif /* CONFIG_USB_OTG */

/* host controller driver interface */
static const struct hc_driver crisv10_hc_driver = 
  {
    .description =	hc_name,
    .product_desc =	product_desc,
    .hcd_priv_size =	sizeof(struct crisv10_hcd),

    /* Attaching IRQ handler manualy in probe() */
    /* .irq =		crisv10_hcd_irq, */

    .flags =		HCD_USB11,

    /* called to init HCD and root hub */
    .reset =		crisv10_hcd_reset,
    .start =		crisv10_hcd_start,	

    /* cleanly make HCD stop writing memory and doing I/O */
    .stop =		crisv10_hcd_stop,

    /* return current frame number */
    .get_frame_number =	crisv10_hcd_get_frame,


    /* Manage i/o requests via the Transfer Controller */
    .urb_enqueue =	tc_urb_enqueue,
    .urb_dequeue =	tc_urb_dequeue,

    /* hw synch, freeing endpoint resources that urb_dequeue can't */
    .endpoint_disable = tc_endpoint_disable,


    /* Root Hub support */
    .hub_status_data =	rh_status_data_request,
    .hub_control =	rh_control_request,
#ifdef CONFIG_PM
    .hub_suspend =	rh_suspend_request,
    .hub_resume =	rh_resume_request,
#endif /* CONFIG_PM */
#ifdef	CONFIG_USB_OTG
    .start_port_reset =	crisv10_hcd_start_port_reset,
#endif /* CONFIG_USB_OTG */
  };


/*
 * conversion between pointers to a hcd and the corresponding
 * crisv10_hcd 
 */

static inline struct crisv10_hcd *hcd_to_crisv10_hcd(struct usb_hcd *hcd)
{
	return (struct crisv10_hcd *) hcd->hcd_priv;
}

static inline struct usb_hcd *crisv10_hcd_to_hcd(struct crisv10_hcd *hcd)
{
	return container_of((void *) hcd, struct usb_hcd, hcd_priv);
}

/* check if specified port is in use */
static inline int port_in_use(unsigned int port)
{
	return ports & (1 << port);
}

/* number of ports in use */
static inline unsigned int num_ports(void)
{
	unsigned int i, num = 0;
	for (i = 0; i < USB_ROOT_HUB_PORTS; i++)
		if (port_in_use(i))
			num++;
	return num;
}

/* map hub port number to the port number used internally by the HC */
static inline unsigned int map_port(unsigned int port)
{
  unsigned int i, num = 0;
  for (i = 0; i < USB_ROOT_HUB_PORTS; i++)
    if (port_in_use(i))
      if (++num == port)
	return i;
  return -1;
}

/* size of descriptors in slab cache */
#ifndef MAX
#define MAX(x, y)		((x) > (y) ? (x) : (y))
#endif


/******************************************************************/
/* Hardware Interrupt functions                                   */
/******************************************************************/

/* Fast interrupt handler for HC */
static irqreturn_t crisv10_hcd_top_irq(int irq, void *vcd)
{
  struct usb_hcd *hcd = vcd;
  struct crisv10_irq_reg reg;
  __u32 irq_mask;
  unsigned long flags;

  DBFENTER;

  ASSERT(hcd != NULL);
  reg.hcd = hcd;

  /* Turn of other interrupts while handling these sensitive cases */
  local_irq_save(flags);
  
  /* Read out which interrupts that are flaged */
  irq_mask = *R_USB_IRQ_MASK_READ;
  reg.r_usb_irq_mask_read = irq_mask;

  /* Reading R_USB_STATUS clears the ctl_status interrupt. Note that
     R_USB_STATUS must be read before R_USB_EPID_ATTN since reading the latter
     clears the ourun and perror fields of R_USB_STATUS. */
  reg.r_usb_status = *R_USB_STATUS;
  
  /* Reading R_USB_EPID_ATTN clears the iso_eof, bulk_eot and epid_attn
     interrupts. */
  reg.r_usb_epid_attn = *R_USB_EPID_ATTN;
  
  /* Reading R_USB_RH_PORT_STATUS_1 and R_USB_RH_PORT_STATUS_2 clears the
     port_status interrupt. */
  reg.r_usb_rh_port_status_1 = *R_USB_RH_PORT_STATUS_1;
  reg.r_usb_rh_port_status_2 = *R_USB_RH_PORT_STATUS_2;
  
  /* Reading R_USB_FM_NUMBER clears the sof interrupt. */
  /* Note: the lower 11 bits contain the actual frame number, sent with each
     sof. */
  reg.r_usb_fm_number = *R_USB_FM_NUMBER;

  /* Interrupts are handled in order of priority. */
  if (irq_mask & IO_MASK(R_USB_IRQ_MASK_READ, port_status)) {
    crisv10_hcd_port_status_irq(&reg);
  }
  if (irq_mask & IO_MASK(R_USB_IRQ_MASK_READ, epid_attn)) {
    crisv10_hcd_epid_attn_irq(&reg);
  }
  if (irq_mask & IO_MASK(R_USB_IRQ_MASK_READ, ctl_status)) {
    crisv10_hcd_ctl_status_irq(&reg);
  }
  if (irq_mask & IO_MASK(R_USB_IRQ_MASK_READ, iso_eof)) {
    crisv10_hcd_isoc_eof_irq(&reg);
  }
  if (irq_mask & IO_MASK(R_USB_IRQ_MASK_READ, bulk_eot)) {
    /* Update/restart the bulk start timer since obviously the channel is
       running. */
    mod_timer(&bulk_start_timer, jiffies + BULK_START_TIMER_INTERVAL);
    /* Update/restart the bulk eot timer since we just received an bulk eot
       interrupt. */
    mod_timer(&bulk_eot_timer, jiffies + BULK_EOT_TIMER_INTERVAL);

    /* Check for finished bulk transfers on epids */
    check_finished_bulk_tx_epids(hcd, 0);
  }
  local_irq_restore(flags);

  DBFEXIT;
  return IRQ_HANDLED;
}


void crisv10_hcd_epid_attn_irq(struct crisv10_irq_reg *reg) {
  struct usb_hcd *hcd = reg->hcd;
  struct crisv10_urb_priv *urb_priv;
  int epid;
  DBFENTER;

  for (epid = 0; epid < NBR_OF_EPIDS; epid++) {
    if (test_bit(epid, (void *)&reg->r_usb_epid_attn)) {
      struct urb *urb;
      __u32 ept_data;
      int error_code;

      if (epid == DUMMY_EPID || epid == INVALID_EPID) {
	/* We definitely don't care about these ones. Besides, they are
	   always disabled, so any possible disabling caused by the
	   epid attention interrupt is irrelevant. */
	continue;
      }

      if(!epid_inuse(epid)) {
	irq_err("Epid attention on epid:%d that isn't in use\n", epid);
	printk("R_USB_STATUS: 0x%x\n", reg->r_usb_status);
	debug_epid(epid);
	continue;
      }

      /* Note that although there are separate R_USB_EPT_DATA and
	 R_USB_EPT_DATA_ISO registers, they are located at the same address and
	 are of the same size. In other words, this read should be ok for isoc
	 also. */
      ept_data = etrax_epid_get(epid);
      error_code = IO_EXTRACT(R_USB_EPT_DATA, error_code, ept_data);

      /* Get the active URB for this epid. We blatantly assume
	 that only this URB could have caused the epid attention. */
      urb = activeUrbList[epid];
      if (urb == NULL) {
	irq_err("Attention on epid:%d error:%d with no active URB.\n",
		epid, error_code);
	printk("R_USB_STATUS: 0x%x\n", reg->r_usb_status);
	debug_epid(epid);
	continue;
      }

      urb_priv = (struct crisv10_urb_priv *)urb->hcpriv;
      ASSERT(urb_priv);

      /* Using IO_STATE_VALUE on R_USB_EPT_DATA should be ok for isoc also. */
      if (error_code == IO_STATE_VALUE(R_USB_EPT_DATA, error_code, no_error)) {

	/* Isoc traffic doesn't have error_count_in/error_count_out. */
	if ((usb_pipetype(urb->pipe) != PIPE_ISOCHRONOUS) &&
	    (IO_EXTRACT(R_USB_EPT_DATA, error_count_in, ept_data) == 3 ||
	     IO_EXTRACT(R_USB_EPT_DATA, error_count_out, ept_data) == 3)) {
	  /* Check if URB allready is marked for late-finish, we can get
	     several 3rd error for Intr traffic when a device is unplugged */
	  if(urb_priv->later_data == NULL) {
	    /* 3rd error. */
	    irq_warn("3rd error for epid:%d (%s %s) URB:0x%x[%d]\n", epid,
		     str_dir(urb->pipe), str_type(urb->pipe),
		     (unsigned int)urb, urb_priv->urb_num);
	  
	    tc_finish_urb_later(hcd, urb, -EPROTO);
	  }

	} else if (reg->r_usb_status & IO_MASK(R_USB_STATUS, perror)) {
	  irq_warn("Perror for epid:%d\n", epid);
	  printk("FM_NUMBER: %d\n", reg->r_usb_fm_number & 0x7ff);
	  printk("R_USB_STATUS: 0x%x\n", reg->r_usb_status);
	  __dump_urb(urb);
	  debug_epid(epid);

	  if (!(ept_data & IO_MASK(R_USB_EPT_DATA, valid))) {
	    /* invalid ep_id */
	    panic("Perror because of invalid epid."
		  " Deconfigured too early?");
	  } else {
	    /* past eof1, near eof, zout transfer, setup transfer */
	    /* Dump the urb and the relevant EP descriptor. */
	    panic("Something wrong with DMA descriptor contents."
		  " Too much traffic inserted?");
	  }
	} else if (reg->r_usb_status & IO_MASK(R_USB_STATUS, ourun)) {
	  /* buffer ourun */
	  printk("FM_NUMBER: %d\n", reg->r_usb_fm_number & 0x7ff);
	  printk("R_USB_STATUS: 0x%x\n", reg->r_usb_status);
	  __dump_urb(urb);
	  debug_epid(epid);

	  panic("Buffer overrun/underrun for epid:%d. DMA too busy?", epid);
	} else {
	  irq_warn("Attention on epid:%d (%s %s) with no error code\n", epid,
		   str_dir(urb->pipe), str_type(urb->pipe));
	  printk("R_USB_STATUS: 0x%x\n", reg->r_usb_status);
	  __dump_urb(urb);
	  debug_epid(epid);
	}

      } else if (error_code == IO_STATE_VALUE(R_USB_EPT_DATA, error_code,
					      stall)) {
	/* Not really a protocol error, just says that the endpoint gave
	   a stall response. Note that error_code cannot be stall for isoc. */
	if (usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS) {
	  panic("Isoc traffic cannot stall");
	}

	tc_dbg("Stall for epid:%d (%s %s) URB:0x%x\n", epid,
	       str_dir(urb->pipe), str_type(urb->pipe), (unsigned int)urb);
	tc_finish_urb(hcd, urb, -EPIPE);

      } else if (error_code == IO_STATE_VALUE(R_USB_EPT_DATA, error_code,
					      bus_error)) {
	/* Two devices responded to a transaction request. Must be resolved
	   by software. FIXME: Reset ports? */
	panic("Bus error for epid %d."
	      " Two devices responded to transaction request\n",
	      epid);

      } else if (error_code == IO_STATE_VALUE(R_USB_EPT_DATA, error_code,
					      buffer_error)) {
	/* DMA overrun or underrun. */
	irq_warn("Buffer overrun/underrun for epid:%d (%s %s)\n", epid,
		 str_dir(urb->pipe), str_type(urb->pipe));

	/* It seems that error_code = buffer_error in
	   R_USB_EPT_DATA/R_USB_EPT_DATA_ISO and ourun = yes in R_USB_STATUS
	   are the same error. */
	tc_finish_urb(hcd, urb, -EPROTO);
      } else {
	  irq_warn("Unknown attention on epid:%d (%s %s)\n", epid,
		   str_dir(urb->pipe), str_type(urb->pipe));
	  dump_ept_data(epid);
      }
    }
  }
  DBFEXIT;
}

void crisv10_hcd_port_status_irq(struct crisv10_irq_reg *reg)
{
  __u16 port_reg[USB_ROOT_HUB_PORTS];
  DBFENTER;
  port_reg[0] = reg->r_usb_rh_port_status_1;
  port_reg[1] = reg->r_usb_rh_port_status_2;
  rh_port_status_change(port_reg);
  DBFEXIT;
}

void crisv10_hcd_isoc_eof_irq(struct crisv10_irq_reg *reg)
{
  int epid;
  struct urb *urb;
  struct crisv10_urb_priv *urb_priv;

  DBFENTER;

  for (epid = 0; epid < NBR_OF_EPIDS - 1; epid++) {

    /* Only check epids that are in use, is valid and has SB list */
    if (!epid_inuse(epid) || epid == INVALID_EPID ||
	TxIsocEPList[epid].sub == 0 || epid == DUMMY_EPID) {
      /* Nothing here to see. */
      continue;
    }
    ASSERT(epid_isoc(epid));

    /* Get the active URB for this epid (if any). */
    urb = activeUrbList[epid];
    if (urb == 0) {
      isoc_warn("Ignoring NULL urb for epid:%d\n", epid);
      continue;
    }
    if(!epid_out_traffic(epid)) {
      /* Sanity check. */
      ASSERT(usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS);

      urb_priv = (struct crisv10_urb_priv *)urb->hcpriv;
      ASSERT(urb_priv);

      if (urb_priv->urb_state == NOT_STARTED) {
	/* If ASAP is not set and urb->start_frame is the current frame,
	   start the transfer. */
	if (!(urb->transfer_flags & URB_ISO_ASAP) &&
	    (urb->start_frame == (*R_USB_FM_NUMBER & 0x7ff))) {
	  /* EP should not be enabled if we're waiting for start_frame */
	  ASSERT((TxIsocEPList[epid].command &
		  IO_STATE(USB_EP_command, enable, yes)) == 0);

	  isoc_warn("Enabling isoc IN EP descr for epid %d\n", epid);
	  TxIsocEPList[epid].command |= IO_STATE(USB_EP_command, enable, yes);

	  /* This urb is now active. */
	  urb_priv->urb_state = STARTED;
	  continue;
	}
      }
    }
  }

  DBFEXIT;
}

void crisv10_hcd_ctl_status_irq(struct crisv10_irq_reg *reg)
{
  struct crisv10_hcd* crisv10_hcd = hcd_to_crisv10_hcd(reg->hcd);

  DBFENTER;
  ASSERT(crisv10_hcd);

/*  irq_dbg("ctr_status_irq, controller status: %s\n",
	  hcd_status_to_str(reg->r_usb_status));*/
  
  /* FIXME: What should we do if we get ourun or perror? Dump the EP and SB
     list for the corresponding epid? */
  if (reg->r_usb_status & IO_MASK(R_USB_STATUS, ourun)) {
    panic("USB controller got ourun.");
  }
  if (reg->r_usb_status & IO_MASK(R_USB_STATUS, perror)) {
    
    /* Before, etrax_usb_do_intr_recover was called on this epid if it was
       an interrupt pipe. I don't see how re-enabling all EP descriptors
       will help if there was a programming error. */
    panic("USB controller got perror.");
  }

  /* Keep track of USB Controller, if it's running or not */
  if(reg->r_usb_status & IO_STATE(R_USB_STATUS, running, yes)) {
    crisv10_hcd->running = 1;
  } else {
    crisv10_hcd->running = 0;
  }
  
  if (reg->r_usb_status & IO_MASK(R_USB_STATUS, device_mode)) {
    /* We should never operate in device mode. */
    panic("USB controller in device mode.");
  }

  /* Set the flag to avoid getting "Unlink after no-IRQ? Controller is probably
     using the wrong IRQ" from hcd_unlink_urb() in drivers/usb/core/hcd.c */
  set_bit(HCD_FLAG_SAW_IRQ, &reg->hcd->flags);
  
  DBFEXIT;
}


/******************************************************************/
/* Host Controller interface functions                            */
/******************************************************************/

static inline void crisv10_ready_wait(void) {
  volatile int timeout = 10000;
  /* Check the busy bit of USB controller in Etrax */
  while((*R_USB_COMMAND & IO_MASK(R_USB_COMMAND, busy)) &&
	(timeout-- > 0));
}

/* reset host controller */
static int crisv10_hcd_reset(struct usb_hcd *hcd)
{
  DBFENTER;
  hcd_dbg(hcd, "reset\n");


  /* Reset the USB interface. */
  /*
  *R_USB_COMMAND =
    IO_STATE(R_USB_COMMAND, port_sel, nop) |
    IO_STATE(R_USB_COMMAND, port_cmd, reset) |
    IO_STATE(R_USB_COMMAND, ctrl_cmd, reset);
  nop();
  */
  DBFEXIT;
  return 0;
}

/* start host controller */
static int crisv10_hcd_start(struct usb_hcd *hcd)
{
  DBFENTER;
  hcd_dbg(hcd, "start\n");

  crisv10_ready_wait();

  /* Start processing of USB traffic. */
  *R_USB_COMMAND =
    IO_STATE(R_USB_COMMAND, port_sel, nop) |
    IO_STATE(R_USB_COMMAND, port_cmd, reset) |
    IO_STATE(R_USB_COMMAND, ctrl_cmd, host_run);

  nop();

  hcd->state = HC_STATE_RUNNING;

  DBFEXIT;
  return 0;
}

/* stop host controller */
static void crisv10_hcd_stop(struct usb_hcd *hcd)
{
  DBFENTER;
  hcd_dbg(hcd, "stop\n");
  crisv10_hcd_reset(hcd);
  DBFEXIT;
}

/* return the current frame number */
static int crisv10_hcd_get_frame(struct usb_hcd *hcd)
{
  DBFENTER;
  DBFEXIT;
  return (*R_USB_FM_NUMBER & 0x7ff);
}

#ifdef	CONFIG_USB_OTG

static int crisv10_hcd_start_port_reset(struct usb_hcd *hcd, unsigned port)
{
  return 0; /* no-op for now */
}

#endif /* CONFIG_USB_OTG */


/******************************************************************/
/* Root Hub functions                                             */
/******************************************************************/

/* root hub status */
static const struct usb_hub_status rh_hub_status = 
  {
    .wHubStatus =		0,
    .wHubChange =		0,
  };

/* root hub descriptor */
static const u8 rh_hub_descr[] =
  {
    0x09,			/* bDescLength	       */
    0x29,			/* bDescriptorType     */
    USB_ROOT_HUB_PORTS,         /* bNbrPorts	       */
    0x00,			/* wHubCharacteristics */
    0x00,		 
    0x01,			/* bPwrOn2pwrGood      */
    0x00,			/* bHubContrCurrent    */
    0x00,			/* DeviceRemovable     */
    0xff			/* PortPwrCtrlMask     */
  };

/* Actual holder of root hub status*/
struct crisv10_rh rh;

/* Initialize root hub data structures (called from dvdrv_hcd_probe()) */
int rh_init(void) {
  int i;
  /* Reset port status flags */
  for (i = 0; i < USB_ROOT_HUB_PORTS; i++) {
    rh.wPortChange[i] = 0;
    rh.wPortStatusPrev[i] = 0;
  }
  return 0;
}

#define RH_FEAT_MASK ((1<<USB_PORT_FEAT_CONNECTION)|\
		      (1<<USB_PORT_FEAT_ENABLE)|\
		      (1<<USB_PORT_FEAT_SUSPEND)|\
		      (1<<USB_PORT_FEAT_RESET))

/* Handle port status change interrupt (called from bottom part interrupt) */
void rh_port_status_change(__u16 port_reg[]) {
  int i;
  __u16 wChange;

  for(i = 0; i < USB_ROOT_HUB_PORTS; i++) {
    /* Xor out changes since last read, masked for important flags */
    wChange = (port_reg[i] & RH_FEAT_MASK) ^ rh.wPortStatusPrev[i];
    /* Or changes together with (if any) saved changes */
    rh.wPortChange[i] |= wChange;
    /* Save new status */
    rh.wPortStatusPrev[i] = port_reg[i];

    if(wChange) {
      rh_dbg("Interrupt port_status change port%d: %s  Current-status:%s\n", i+1,
	     port_status_to_str(wChange),
	     port_status_to_str(port_reg[i]));
    }
  }
}

/* Construct port status change bitmap for the root hub */
static int rh_status_data_request(struct usb_hcd *hcd, char *buf)
{
  struct crisv10_hcd* crisv10_hcd = hcd_to_crisv10_hcd(hcd);
  unsigned int i;

  DBFENTER;
  /*
   * corresponds to hub status change EP (USB 2.0 spec section 11.13.4)
   * return bitmap indicating ports with status change
   */
  *buf = 0;
  spin_lock(&crisv10_hcd->lock);
  for (i = 1; i <= crisv10_hcd->num_ports; i++) {
    if (rh.wPortChange[map_port(i)]) {
      *buf |= (1 << i);
      rh_dbg("rh_status_data_request, change on port %d: %s  Current Status: %s\n", i,
	     port_status_to_str(rh.wPortChange[map_port(i)]),
	     port_status_to_str(rh.wPortStatusPrev[map_port(i)]));
    }
  }
  spin_unlock(&crisv10_hcd->lock);
  DBFEXIT;
  return *buf == 0 ? 0 : 1;
}

/* Handle a control request for the root hub (called from hcd_driver) */
static int rh_control_request(struct usb_hcd *hcd, 
			      u16 typeReq, 
			      u16 wValue, 
			      u16 wIndex,
			      char *buf, 
			      u16 wLength) {

  struct crisv10_hcd *crisv10_hcd = hcd_to_crisv10_hcd(hcd);
  int retval = 0;
  int len;
  DBFENTER;

  switch (typeReq) {
  case GetHubDescriptor:
    rh_dbg("GetHubDescriptor\n");
    len = min_t(unsigned int, sizeof rh_hub_descr, wLength);
    memcpy(buf, rh_hub_descr, len);
    buf[2] = crisv10_hcd->num_ports;
    break;
  case GetHubStatus:
    rh_dbg("GetHubStatus\n");
    len = min_t(unsigned int, sizeof rh_hub_status, wLength);
    memcpy(buf, &rh_hub_status, len);
    break;
  case GetPortStatus:
    if (!wIndex || wIndex > crisv10_hcd->num_ports)
      goto error;
    rh_dbg("GetportStatus, port:%d change:%s  status:%s\n", wIndex,
	   port_status_to_str(rh.wPortChange[map_port(wIndex)]),
	   port_status_to_str(rh.wPortStatusPrev[map_port(wIndex)]));
    *(u16 *) buf = cpu_to_le16(rh.wPortStatusPrev[map_port(wIndex)]);
    *(u16 *) (buf + 2) = cpu_to_le16(rh.wPortChange[map_port(wIndex)]);
    break;
  case SetHubFeature:
    rh_dbg("SetHubFeature\n");
  case ClearHubFeature:
    rh_dbg("ClearHubFeature\n");
    switch (wValue) {
    case C_HUB_OVER_CURRENT:
    case C_HUB_LOCAL_POWER:
      rh_warn("Not implemented hub request:%d \n", typeReq);
      /* not implemented */
      break;
    default:
      goto error;
    }
    break;
  case SetPortFeature:
    if (!wIndex || wIndex > crisv10_hcd->num_ports)
      goto error;
    if(rh_set_port_feature(map_port(wIndex), wValue))
      goto error;
    break;
  case ClearPortFeature:
    if (!wIndex || wIndex > crisv10_hcd->num_ports)
      goto error;
    if(rh_clear_port_feature(map_port(wIndex), wValue))
      goto error;
    break;
  default:
    rh_warn("Unknown hub request: %d\n", typeReq);
  error:
    retval = -EPIPE;
  }
  DBFEXIT;
  return retval;
}

int rh_set_port_feature(__u8 bPort, __u16 wFeature) {
  __u8 bUsbCommand = 0;
  __u8 reset_cnt;
  switch(wFeature) {
  case USB_PORT_FEAT_RESET:
    rh_dbg("SetPortFeature: reset\n");

    if (rh.wPortStatusPrev[bPort] &
        IO_STATE(R_USB_RH_PORT_STATUS_1, enabled, yes))
    {
      __u8 restart_controller = 0;

      if ( (rh.wPortStatusPrev[0] &
            IO_STATE(R_USB_RH_PORT_STATUS_1, enabled, yes)) &&
           (rh.wPortStatusPrev[1] &
            IO_STATE(R_USB_RH_PORT_STATUS_2, enabled, yes)) )
      {
        /* Both ports is enabled. The USB controller will not change state. */
        restart_controller = 0;
      }
      else
      {
        /* Only ports is enabled. The USB controller will change state and
           must be restarted. */
        restart_controller = 1;
      }
      /*
        In ETRAX 100LX it's not possible to reset an enabled root hub port.
        The workaround is to disable and enable the port before resetting it.
        Disabling the port can, if both ports are disabled at once, cause the
        USB controller to change state to HOST_MODE state. 
        The USB controller state transition causes a lot of unwanted
        interrupts that must be avoided.
        Disabling the USB controller status and port status interrupts before
        disabling/resetting the port stops these interrupts.

        These actions are performed:
        1. Disable USB controller status and port status interrupts.
        2. Disable the port
        3. Wait for the port to be disabled.
        4. Enable the port.
        5. Wait for the port to be enabled.
        6. Reset the port.
        7. Wait for for the reset to end.
        8. Wait for the USB controller entering started state.
        9. Order the USB controller to running state.
        10. Wait for the USB controller reaching running state.
        11. Clear all interrupts generated during the disable/enable/reset
            procedure.
        12. Enable the USB controller status and port status interrupts.
      */

      /* 1. Disable USB controller status and USB port status interrupts. */
      *R_USB_IRQ_MASK_CLR = IO_STATE(R_USB_IRQ_MASK_CLR, ctl_status, clr);
      __asm__ __volatile__ ("  nop");
      *R_USB_IRQ_MASK_CLR = IO_STATE(R_USB_IRQ_MASK_CLR, port_status, clr);
      __asm__ __volatile__ ("  nop");
      
      {

        /* Since an root hub port reset shall be 50 ms and the ETRAX 100LX
           root hub port reset is 10 ms we must perform 5 port resets to
           achieve a proper root hub port reset. */
        for (reset_cnt = 0; reset_cnt < 5; reset_cnt ++)
        {
          rh_dbg("Disable Port %d\n", bPort + 1);

          /* 2. Disable the port*/
          if (bPort == 0)
          {
            *R_USB_PORT1_DISABLE = IO_STATE(R_USB_PORT1_DISABLE, disable, yes);
          }
          else
          {
            *R_USB_PORT2_DISABLE = IO_STATE(R_USB_PORT2_DISABLE, disable, yes);
          }

          /* 3. Wait for the port to be disabled. */
          while ( (bPort == 0) ?
                  *R_USB_RH_PORT_STATUS_1 &
                    IO_STATE(R_USB_RH_PORT_STATUS_1, enabled, yes) :
                  *R_USB_RH_PORT_STATUS_2 &
                    IO_STATE(R_USB_RH_PORT_STATUS_2, enabled, yes) ) {}

          rh_dbg("Port %d is disabled. Enable it!\n", bPort + 1);

          /* 4. Enable the port. */
          if (bPort == 0)
          {
            *R_USB_PORT1_DISABLE = IO_STATE(R_USB_PORT1_DISABLE, disable, no);
          }
          else
          {
            *R_USB_PORT2_DISABLE = IO_STATE(R_USB_PORT2_DISABLE, disable, no);
          }

          /* 5. Wait for the port to be enabled again.  */
          while (!( (bPort == 0) ?
                    *R_USB_RH_PORT_STATUS_1 &
                      IO_STATE(R_USB_RH_PORT_STATUS_1, connected, yes) :
                    *R_USB_RH_PORT_STATUS_2 &
                      IO_STATE(R_USB_RH_PORT_STATUS_2, connected, yes) ) ) {}

          rh_dbg("Port %d is enabled.\n", bPort + 1);

          /* 6. Reset the port */
          crisv10_ready_wait();
          *R_USB_COMMAND =
            ( (bPort == 0) ?
              IO_STATE(R_USB_COMMAND, port_sel, port1):
              IO_STATE(R_USB_COMMAND, port_sel, port2) ) |
            IO_STATE(R_USB_COMMAND, port_cmd, reset) |
            IO_STATE(R_USB_COMMAND, busy,     no) |
            IO_STATE(R_USB_COMMAND, ctrl_cmd, nop);
          rh_dbg("Port %d is resetting.\n", bPort + 1);

          /* 7. The USB specification says that we should wait for at least
             10ms for device recover */
          udelay(10500); /* 10,5ms blocking wait */
    
          crisv10_ready_wait();
        }
      }


      /* Check if the USB controller needs to be restarted. */
      if (restart_controller)
      {
        /* 8. Wait for the USB controller entering started state. */
        while (!(*R_USB_STATUS & IO_STATE(R_USB_STATUS, started, yes))) {}

        /* 9. Order the USB controller to running state. */
        crisv10_ready_wait();
        *R_USB_COMMAND =
          IO_STATE(R_USB_COMMAND, port_sel, nop) |
          IO_STATE(R_USB_COMMAND, port_cmd, reset) |
          IO_STATE(R_USB_COMMAND, busy,     no) |
          IO_STATE(R_USB_COMMAND, ctrl_cmd, host_run);

        /* 10. Wait for the USB controller reaching running state. */
        while (!(*R_USB_STATUS & IO_STATE(R_USB_STATUS, running, yes))) {}
      }

      /* 11. Clear any controller or port satus interrupts before enabling
             the interrupts. */
      {
        u16 dummy;

        /* Clear the port status interrupt of the reset port. */
        if (bPort == 0)
        {
          rh_dbg("Clearing port 1 interrupts\n");
          dummy = *R_USB_RH_PORT_STATUS_1;
        }
        else
        {
          rh_dbg("Clearing port 2 interrupts\n");
          dummy = *R_USB_RH_PORT_STATUS_2;
        }

        if (restart_controller)
        {
          /* The USB controller is restarted. Clear all interupts. */
          rh_dbg("Clearing all interrupts\n");
          dummy = *R_USB_STATUS;
          dummy = *R_USB_RH_PORT_STATUS_1;
          dummy = *R_USB_RH_PORT_STATUS_2;
        }
      }

      /* 12. Enable USB controller status and USB port status interrupts.  */
      *R_USB_IRQ_MASK_SET = IO_STATE(R_USB_IRQ_MASK_SET, ctl_status, set);
      __asm__ __volatile__ ("  nop");
      *R_USB_IRQ_MASK_SET = IO_STATE(R_USB_IRQ_MASK_SET, port_status, set);
      __asm__ __volatile__ ("  nop");

    }
    else
    {

      bUsbCommand |= IO_STATE(R_USB_COMMAND, port_cmd, reset);
      /* Select which port via the port_sel field */
      bUsbCommand |= IO_FIELD(R_USB_COMMAND, port_sel, bPort+1);

      /* Make sure the controller isn't busy. */
      crisv10_ready_wait();
      /* Send out the actual command to the USB controller */
      *R_USB_COMMAND = bUsbCommand;

      /* Wait a while for controller to first become started after port reset */
      udelay(12000); /* 12ms blocking wait */
      
      /* Make sure the controller isn't busy. */
      crisv10_ready_wait();

      /* If all enabled ports were disabled the host controller goes down into
         started mode, so we need to bring it back into the running state.
         (This is safe even if it's already in the running state.) */
      *R_USB_COMMAND =
        IO_STATE(R_USB_COMMAND, port_sel, nop) |
        IO_STATE(R_USB_COMMAND, port_cmd, reset) |
        IO_STATE(R_USB_COMMAND, ctrl_cmd, host_run);
    }

    break;
  case USB_PORT_FEAT_SUSPEND:
    rh_dbg("SetPortFeature: suspend\n");
    bUsbCommand |= IO_STATE(R_USB_COMMAND, port_cmd, suspend);
    goto set;
    break;
  case USB_PORT_FEAT_POWER:
    rh_dbg("SetPortFeature: power\n");
    break;
  case USB_PORT_FEAT_C_CONNECTION:
    rh_dbg("SetPortFeature: c_connection\n");
    break;
  case USB_PORT_FEAT_C_RESET:
    rh_dbg("SetPortFeature: c_reset\n");
    break;
  case USB_PORT_FEAT_C_OVER_CURRENT:
    rh_dbg("SetPortFeature: c_over_current\n");
    break;

  set:
    /* Select which port via the port_sel field */
    bUsbCommand |= IO_FIELD(R_USB_COMMAND, port_sel, bPort+1);

    /* Make sure the controller isn't busy. */
    crisv10_ready_wait();
    /* Send out the actual command to the USB controller */
    *R_USB_COMMAND = bUsbCommand;
    break;
  default:
    rh_dbg("SetPortFeature: unknown feature\n");
    return -1;
  }
  return 0;
}

int rh_clear_port_feature(__u8 bPort, __u16 wFeature) {
  switch(wFeature) {
  case USB_PORT_FEAT_ENABLE:
    rh_dbg("ClearPortFeature: enable\n");
    rh_disable_port(bPort);
    break;
  case USB_PORT_FEAT_SUSPEND:
    rh_dbg("ClearPortFeature: suspend\n");
    break;
  case USB_PORT_FEAT_POWER:
    rh_dbg("ClearPortFeature: power\n");
    break;

  case USB_PORT_FEAT_C_ENABLE:
    rh_dbg("ClearPortFeature: c_enable\n");
    goto clear;
  case USB_PORT_FEAT_C_SUSPEND:
    rh_dbg("ClearPortFeature: c_suspend\n");
    goto clear;
  case USB_PORT_FEAT_C_CONNECTION:
    rh_dbg("ClearPortFeature: c_connection\n");
    goto clear;
  case USB_PORT_FEAT_C_OVER_CURRENT:
    rh_dbg("ClearPortFeature: c_over_current\n");
    goto clear;
  case USB_PORT_FEAT_C_RESET:
    rh_dbg("ClearPortFeature: c_reset\n");
    goto clear;
  clear:
    rh.wPortChange[bPort] &= ~(1 << (wFeature - 16));
    break;
  default:
    rh_dbg("ClearPortFeature: unknown feature\n");
    return -1;
  }
  return 0;
}


#ifdef	CONFIG_PM
/* Handle a suspend request for the root hub (called from hcd_driver) */
static int rh_suspend_request(struct usb_hcd *hcd)
{
  return 0; /* no-op for now */
}

/* Handle a resume request for the root hub (called from hcd_driver) */
static int rh_resume_request(struct usb_hcd *hcd)
{
  return 0; /* no-op for now */
}
#endif /* CONFIG_PM */



/* Wrapper function for workaround port disable registers in USB controller  */
static void rh_disable_port(unsigned int port) {
  volatile int timeout = 10000;
  volatile char* usb_portx_disable;
  switch(port) {
  case 0:
    usb_portx_disable = R_USB_PORT1_DISABLE;
    break;
  case 1:
    usb_portx_disable = R_USB_PORT2_DISABLE;
    break;
  default:
    /* Invalid port index */
    return;
  }
  /* Set disable flag in special register  */
  *usb_portx_disable = IO_STATE(R_USB_PORT1_DISABLE, disable, yes);
  /* Wait until not enabled anymore */
  while((rh.wPortStatusPrev[port] &
	IO_STATE(R_USB_RH_PORT_STATUS_1, enabled, yes)) &&
	(timeout-- > 0));

  /* clear disable flag in special register  */
  *usb_portx_disable = IO_STATE(R_USB_PORT1_DISABLE, disable, no);
  rh_info("Physical port %d disabled\n", port+1);
}


/******************************************************************/
/* Transfer Controller (TC) functions                             */
/******************************************************************/

/* FIXME: Should RX_BUF_SIZE be a config option, or maybe we should adjust it
   dynamically?
   To adjust it dynamically we would have to get an interrupt when we reach
   the end of the rx descriptor list, or when we get close to the end, and
   then allocate more descriptors. */
#define NBR_OF_RX_DESC     512
#define RX_DESC_BUF_SIZE   1024
#define RX_BUF_SIZE        (NBR_OF_RX_DESC * RX_DESC_BUF_SIZE)


/* Local variables for Transfer Controller */
/* --------------------------------------- */

/* This is a circular (double-linked) list of the active urbs for each epid.
   The head is never removed, and new urbs are linked onto the list as
   urb_entry_t elements. Don't reference urb_list directly; use the wrapper
   functions instead (which includes spin_locks) */
static struct list_head urb_list[NBR_OF_EPIDS];

/* Read about the need and usage of this lock in submit_ctrl_urb. */
/* Lock for URB lists for each EPID */
static spinlock_t urb_list_lock;

/* Lock for EPID array register (R_USB_EPT_x) in Etrax */
static spinlock_t etrax_epid_lock;

/* Lock for dma8 sub0 handling */
static spinlock_t etrax_dma8_sub0_lock;

/* DMA IN cache bug. Align the DMA IN buffers to 32 bytes, i.e. a cache line.
   Since RX_DESC_BUF_SIZE is 1024 is a multiple of 32, all rx buffers will be
   cache aligned. */
static volatile unsigned char RxBuf[RX_BUF_SIZE] __attribute__ ((aligned (32)));
static volatile struct USB_IN_Desc RxDescList[NBR_OF_RX_DESC] __attribute__ ((aligned (4)));

/* Pointers into RxDescList. */
static volatile struct USB_IN_Desc *myNextRxDesc;
static volatile struct USB_IN_Desc *myLastRxDesc;

/* A zout transfer makes a memory access at the address of its buf pointer,
   which means that setting this buf pointer to 0 will cause an access to the
   flash. In addition to this, setting sw_len to 0 results in a 16/32 bytes
   (depending on DMA burst size) transfer.
   Instead, we set it to 1, and point it to this buffer. */
static int zout_buffer[4] __attribute__ ((aligned (4)));

/* Cache for allocating new EP and SB descriptors. */
static struct kmem_cache *usb_desc_cache;

/* Cache for the data allocated in the isoc descr top half. */
static struct kmem_cache *isoc_compl_cache;

/* Cache for the data allocated when delayed finishing of URBs */
static struct kmem_cache *later_data_cache;


/* Counter to keep track of how many Isoc EP we have sat up. Used to enable
   and disable iso_eof interrupt. We only need these interrupts when we have
   Isoc data endpoints (consumes CPU cycles).
   FIXME: This could be more fine granular, so this interrupt is only enabled
   when we have a In Isoc URB not URB_ISO_ASAP flaged queued. */
static int isoc_epid_counter;

/* Protecting wrapper functions for R_USB_EPT_x */
/* -------------------------------------------- */
static inline void etrax_epid_set(__u8 index, __u32 data) {
  unsigned long flags;
  spin_lock_irqsave(&etrax_epid_lock, flags);
  *R_USB_EPT_INDEX = IO_FIELD(R_USB_EPT_INDEX, value, index);
  nop();
  *R_USB_EPT_DATA = data;
  spin_unlock_irqrestore(&etrax_epid_lock, flags);
}

static inline void etrax_epid_clear_error(__u8 index) {
  unsigned long flags;
  spin_lock_irqsave(&etrax_epid_lock, flags);
  *R_USB_EPT_INDEX = IO_FIELD(R_USB_EPT_INDEX, value, index);
  nop();
  *R_USB_EPT_DATA &=
    ~(IO_MASK(R_USB_EPT_DATA, error_count_in) |
      IO_MASK(R_USB_EPT_DATA, error_count_out) |
      IO_MASK(R_USB_EPT_DATA, error_code));
  spin_unlock_irqrestore(&etrax_epid_lock, flags);
}

static inline void etrax_epid_set_toggle(__u8 index, __u8 dirout,
                                             __u8 toggle) {
  unsigned long flags;
  spin_lock_irqsave(&etrax_epid_lock, flags);
  *R_USB_EPT_INDEX = IO_FIELD(R_USB_EPT_INDEX, value, index);
  nop();
  if(dirout) {
    *R_USB_EPT_DATA &= ~IO_MASK(R_USB_EPT_DATA, t_out);
    *R_USB_EPT_DATA |= IO_FIELD(R_USB_EPT_DATA, t_out, toggle);
  } else {
    *R_USB_EPT_DATA &= ~IO_MASK(R_USB_EPT_DATA, t_in);
    *R_USB_EPT_DATA |= IO_FIELD(R_USB_EPT_DATA, t_in, toggle);
  }
  spin_unlock_irqrestore(&etrax_epid_lock, flags);
}

static inline __u8 etrax_epid_get_toggle(__u8 index, __u8 dirout) {
  unsigned long flags;
  __u8 toggle;
  spin_lock_irqsave(&etrax_epid_lock, flags);
  *R_USB_EPT_INDEX = IO_FIELD(R_USB_EPT_INDEX, value, index);
  nop();
  if (dirout) {
    toggle = IO_EXTRACT(R_USB_EPT_DATA, t_out, *R_USB_EPT_DATA);
  } else {
    toggle = IO_EXTRACT(R_USB_EPT_DATA, t_in, *R_USB_EPT_DATA);
  }
  spin_unlock_irqrestore(&etrax_epid_lock, flags);
  return toggle;
}


static inline __u32 etrax_epid_get(__u8 index) {
  unsigned long flags;
  __u32 data;
  spin_lock_irqsave(&etrax_epid_lock, flags);
  *R_USB_EPT_INDEX = IO_FIELD(R_USB_EPT_INDEX, value, index);
  nop();
  data = *R_USB_EPT_DATA;
  spin_unlock_irqrestore(&etrax_epid_lock, flags);
  return data;
}




/* Main functions for Transfer Controller */
/* -------------------------------------- */

/* Init structs, memories and lists used by Transfer Controller */
int tc_init(struct usb_hcd *hcd) {
  int i;
  /* Clear software state info for all epids */
  memset(epid_state, 0, sizeof(struct etrax_epid) * NBR_OF_EPIDS);

  /* Set Invalid and Dummy as being in use and disabled */
  epid_state[INVALID_EPID].inuse = 1;
  epid_state[DUMMY_EPID].inuse = 1;
  epid_state[INVALID_EPID].disabled = 1;
  epid_state[DUMMY_EPID].disabled = 1;

  /* Clear counter for how many Isoc epids we have sat up */
  isoc_epid_counter = 0;

  /* Initialize the urb list by initiating a head for each list.
     Also reset list hodling active URB for each epid */
  for (i = 0; i < NBR_OF_EPIDS; i++) {
    INIT_LIST_HEAD(&urb_list[i]);
    activeUrbList[i] = NULL;
  }

  /* Init lock for URB lists */
  spin_lock_init(&urb_list_lock);
  /* Init lock for Etrax R_USB_EPT register */
  spin_lock_init(&etrax_epid_lock);
  /* Init lock for Etrax dma8 sub0 handling */
  spin_lock_init(&etrax_dma8_sub0_lock);

  /* We use kmem_cache_* to make sure that all DMA desc. are dword aligned */

  /* Note that we specify sizeof(struct USB_EP_Desc) as the size, but also
     allocate SB descriptors from this cache. This is ok since
     sizeof(struct USB_EP_Desc) == sizeof(struct USB_SB_Desc). */
  usb_desc_cache = kmem_cache_create("usb_desc_cache",
				     sizeof(struct USB_EP_Desc), 0,
				     SLAB_HWCACHE_ALIGN, 0);
  if(usb_desc_cache == NULL) {
    return -ENOMEM;
  }

  /* Create slab cache for speedy allocation of memory for isoc bottom-half
     interrupt handling */
  isoc_compl_cache =
    kmem_cache_create("isoc_compl_cache",
		      sizeof(struct crisv10_isoc_complete_data),
		      0, SLAB_HWCACHE_ALIGN, 0);
  if(isoc_compl_cache == NULL) {
    return -ENOMEM;
  }

  /* Create slab cache for speedy allocation of memory for later URB finish
     struct */
  later_data_cache =
    kmem_cache_create("later_data_cache",
		      sizeof(struct urb_later_data),
		      0, SLAB_HWCACHE_ALIGN, 0);
  if(later_data_cache == NULL) {
    return -ENOMEM;
  }


  /* Initiate the bulk start timer. */
  init_timer(&bulk_start_timer);
  bulk_start_timer.expires = jiffies + BULK_START_TIMER_INTERVAL;
  bulk_start_timer.function = tc_bulk_start_timer_func;
  add_timer(&bulk_start_timer);


  /* Initiate the bulk eot timer. */
  init_timer(&bulk_eot_timer);
  bulk_eot_timer.expires = jiffies + BULK_EOT_TIMER_INTERVAL;
  bulk_eot_timer.function = tc_bulk_eot_timer_func;
  bulk_eot_timer.data = (unsigned long)hcd;
  add_timer(&bulk_eot_timer);

  return 0;
}

/* Uninitialize all resources used by Transfer Controller */
void tc_destroy(void) {

  /* Destroy all slab cache */
  kmem_cache_destroy(usb_desc_cache);
  kmem_cache_destroy(isoc_compl_cache);
  kmem_cache_destroy(later_data_cache);

  /* Remove timers */
  del_timer(&bulk_start_timer);
  del_timer(&bulk_eot_timer);
}

static void restart_dma8_sub0(void) {
  unsigned long flags;
  spin_lock_irqsave(&etrax_dma8_sub0_lock, flags);
  /* Verify that the dma is not running */
  if ((*R_DMA_CH8_SUB0_CMD & IO_MASK(R_DMA_CH8_SUB0_CMD, cmd)) == 0) {
    struct USB_EP_Desc *ep = (struct USB_EP_Desc *)phys_to_virt(*R_DMA_CH8_SUB0_EP);
    while (DUMMY_EPID == IO_EXTRACT(USB_EP_command, epid, ep->command)) {
      ep = (struct USB_EP_Desc *)phys_to_virt(ep->next);
    }
    /* Advance the DMA to the next EP descriptor that is not a DUMMY_EPID. */
    *R_DMA_CH8_SUB0_EP = virt_to_phys(ep);
    /* Restart the DMA */
    *R_DMA_CH8_SUB0_CMD = IO_STATE(R_DMA_CH8_SUB0_CMD, cmd, start);
  }
  spin_unlock_irqrestore(&etrax_dma8_sub0_lock, flags);
}

/* queue an URB with the transfer controller (called from hcd_driver) */
static int tc_urb_enqueue(struct usb_hcd *hcd, 
			  struct urb *urb, 
			  gfp_t mem_flags) {
  int epid;
  int retval;
  int bustime = 0;
  int maxpacket;
  unsigned long flags;
  struct crisv10_urb_priv *urb_priv;
  struct crisv10_hcd* crisv10_hcd = hcd_to_crisv10_hcd(hcd);
  DBFENTER;

  if(!(crisv10_hcd->running)) {
    /* The USB Controller is not running, probably because no device is 
       attached. No idea to enqueue URBs then */
    tc_warn("Rejected enqueueing of URB:0x%x because no dev attached\n",
	    (unsigned int)urb);
    return -ENOENT;
  }

  maxpacket = usb_maxpacket(urb->dev, urb->pipe, usb_pipeout(urb->pipe));
  /* Special case check for In Isoc transfers. Specification states that each
     In Isoc transfer consists of one packet and therefore it should fit into
     the transfer-buffer of an URB.
     We do the check here to be sure (an invalid scenario can be produced with
     parameters to the usbtest suite) */
  if(usb_pipeisoc(urb->pipe) && usb_pipein(urb->pipe) &&
     (urb->transfer_buffer_length < maxpacket)) {
    tc_err("Submit In Isoc URB with buffer length:%d to pipe with maxpacketlen: %d\n", urb->transfer_buffer_length, maxpacket);
    return -EMSGSIZE;
  }

  /* Check if there is a epid for URBs destination, if not this function
     set up one. */
  epid = tc_setup_epid(urb->ep, urb, mem_flags);
  if (epid < 0) {
    tc_err("Failed setup epid:%d for URB:0x%x\n", epid, (unsigned int)urb);
    DBFEXIT;
    return -ENOMEM;
  }

  if(urb == activeUrbList[epid]) {
    tc_err("Resubmition of allready active URB:0x%x\n", (unsigned int)urb);
    return -ENXIO;
  }

  if(urb_list_entry(urb, epid)) {
    tc_err("Resubmition of allready queued URB:0x%x\n", (unsigned int)urb);
    return -ENXIO;
  }

  /* If we actively have flaged endpoint as disabled then refuse submition */
  if(epid_state[epid].disabled) {
    return -ENOENT;
  }

  /* Allocate and init HC-private data for URB */
  if(urb_priv_create(hcd, urb, epid, mem_flags) != 0) {
    DBFEXIT;
    return -ENOMEM;
  }
  urb_priv = urb->hcpriv;

  /* Check if there is enough bandwidth for periodic transfer  */
  if(usb_pipeint(urb->pipe) || usb_pipeisoc(urb->pipe)) {
    /* only check (and later claim) if not already claimed */
    if (urb_priv->bandwidth == 0) {
      bustime = crisv10_usb_check_bandwidth(urb->dev, urb);
      if (bustime < 0) {
        tc_err("Not enough periodic bandwidth\n");
        urb_priv_free(hcd, urb);
        DBFEXIT;
        return -ENOSPC;
      }
    }
  }

  tc_dbg("Enqueue URB:0x%x[%d] epid:%d (%s) bufflen:%d\n",
	 (unsigned int)urb, urb_priv->urb_num, epid,
	 pipe_to_str(urb->pipe), urb->transfer_buffer_length);

  /* Create and link SBs required for this URB */
  retval = create_sb_for_urb(urb, mem_flags);
  if(retval != 0) {
    tc_err("Failed to create SBs for URB:0x%x[%d]\n", (unsigned int)urb,
	   urb_priv->urb_num);
    urb_priv_free(hcd, urb);
    DBFEXIT;
    return retval;
  }

  /* Init intr EP pool if this URB is a INTR transfer. This pool is later
     used when inserting EPs in the TxIntrEPList. We do the alloc here
     so we can't run out of memory later */
  if(usb_pipeint(urb->pipe)) {
    retval = init_intr_urb(urb, mem_flags);
    if(retval != 0) {
      tc_warn("Failed to init Intr URB\n");
      urb_priv_free(hcd, urb);
      DBFEXIT;
      return retval;
    }
  }

  /* Disable other access when inserting USB */
  local_irq_save(flags);

  /* Claim bandwidth, if needed */
  if(bustime) {
    crisv10_usb_claim_bandwidth(urb->dev,
                                urb,
                                bustime,
                                (usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS));
  }
  
  /* Add URB to EP queue */
  urb_list_add(urb, epid, mem_flags);

  if(usb_pipeisoc(urb->pipe)) {
    /* Special processing of Isoc URBs. */
    tc_dma_process_isoc_urb(urb);
  } else {
    /* Process EP queue for rest of the URB types (Bulk, Ctrl, Intr) */
    tc_dma_process_queue(epid);
  }

  local_irq_restore(flags);

  DBFEXIT;
  return 0;
}

/* remove an URB from the transfer controller queues (called from hcd_driver)*/
static int tc_urb_dequeue(struct usb_hcd *hcd, struct urb *urb, int status) {
  struct crisv10_urb_priv *urb_priv;
  unsigned long flags;
  int epid;

  DBFENTER;
  /* Disable interrupts here since a descriptor interrupt for the isoc epid
     will modify the sb list.  This could possibly be done more granular, but
     urb_dequeue should not be used frequently anyway.
  */
  local_irq_save(flags);

  urb->status = status;
  urb_priv = urb->hcpriv;

  if (!urb_priv) {
    /* This happens if a device driver calls unlink on an urb that
       was never submitted (lazy driver) or if the urb was completed
       while dequeue was being called. */
    tc_warn("Dequeing of not enqueued URB:0x%x\n", (unsigned int)urb);
    local_irq_restore(flags);
    return 0;
  }
  epid = urb_priv->epid;

  tc_warn("Dequeing %s URB:0x%x[%d] (%s %s epid:%d) status:%d %s\n",
	  (urb == activeUrbList[epid]) ? "active" : "queued",
	  (unsigned int)urb, urb_priv->urb_num, str_dir(urb->pipe),
	  str_type(urb->pipe), epid, urb->status,
	  (urb_priv->later_data) ? "later-sched" : "");

  /* For Bulk, Ctrl and Intr are only one URB active at a time. So any URB
     that isn't active can be dequeued by just removing it from the queue */
  if(usb_pipebulk(urb->pipe) || usb_pipecontrol(urb->pipe) ||
     usb_pipeint(urb->pipe)) {

    /* Check if URB haven't gone further than the queue */
    if(urb != activeUrbList[epid]) {
      ASSERT(urb_priv->later_data == NULL);
      tc_warn("Dequeing URB:0x%x[%d] (%s %s epid:%d) from queue"
	      " (not active)\n", (unsigned int)urb, urb_priv->urb_num,
	      str_dir(urb->pipe), str_type(urb->pipe), epid);
      
      /* Finish the URB with error status from USB core */
      tc_finish_urb(hcd, urb, urb->status);
      local_irq_restore(flags);
      return 0;
    }
  }

  /* Set URB status to Unlink for handling when interrupt comes. */
  urb_priv->urb_state = UNLINK;

  /* Differentiate dequeing of Bulk and Ctrl from Isoc and Intr */
  switch(usb_pipetype(urb->pipe)) {
  case PIPE_BULK:
    /* Check if EP still is enabled */
    if (TxBulkEPList[epid].command & IO_MASK(USB_EP_command, enable)) {
      /* The EP was enabled, disable it. */
      TxBulkEPList[epid].command &= ~IO_MASK(USB_EP_command, enable);
    }
    /* Kicking dummy list out of the party. */
    TxBulkEPList[epid].next = virt_to_phys(&TxBulkEPList[(epid + 1) % NBR_OF_EPIDS]);
    break;
  case PIPE_CONTROL:
    /* Check if EP still is enabled */
    if (TxCtrlEPList[epid].command & IO_MASK(USB_EP_command, enable)) {
      /* The EP was enabled, disable it. */
      TxCtrlEPList[epid].command &= ~IO_MASK(USB_EP_command, enable);
    }
    break;
  case PIPE_ISOCHRONOUS:
    /* Disabling, busy-wait and unlinking of Isoc SBs will be done in
       finish_isoc_urb(). Because there might the case when URB is dequeued
       but there are other valid URBs waiting */

    /* Check if In Isoc EP still is enabled */
    if (TxIsocEPList[epid].command & IO_MASK(USB_EP_command, enable)) {
      /* The EP was enabled, disable it. */
      TxIsocEPList[epid].command &= ~IO_MASK(USB_EP_command, enable);
    }
    break;
  case PIPE_INTERRUPT:
    /* Special care is taken for interrupt URBs. EPs are unlinked in
       tc_finish_urb */
    break;
  default:
    break;
  }

  /* Asynchronous unlink, finish the URB later from scheduled or other
     event (data finished, error) */
  tc_finish_urb_later(hcd, urb, urb->status);

  local_irq_restore(flags);
  DBFEXIT;
  return 0;
}


static void tc_sync_finish_epid(struct usb_hcd *hcd, int epid) {
  volatile int timeout = 10000;
  struct urb* urb;
  struct crisv10_urb_priv* urb_priv;
  unsigned long flags;
  
  volatile struct USB_EP_Desc *first_ep;  /* First EP in the list. */
  volatile struct USB_EP_Desc *curr_ep;   /* Current EP, the iterator. */
  volatile struct USB_EP_Desc *next_ep;   /* The EP after current. */

  int type = epid_state[epid].type;

  /* Setting this flag will cause enqueue() to return -ENOENT for new
     submitions on this endpoint and finish_urb() wont process queue further */
  epid_state[epid].disabled = 1;

  switch(type) {
  case PIPE_BULK:
    /* Check if EP still is enabled */
    if (TxBulkEPList[epid].command & IO_MASK(USB_EP_command, enable)) {
      /* The EP was enabled, disable it. */
      TxBulkEPList[epid].command &= ~IO_MASK(USB_EP_command, enable);
      tc_warn("sync_finish: Disabling EP for epid:%d\n", epid);

      /* Do busy-wait until DMA not using this EP descriptor anymore */
      while((*R_DMA_CH8_SUB0_EP ==
	     virt_to_phys(&TxBulkEPList[epid])) &&
	    (timeout-- > 0));

    }
    break;

  case PIPE_CONTROL:
    /* Check if EP still is enabled */
    if (TxCtrlEPList[epid].command & IO_MASK(USB_EP_command, enable)) {
      /* The EP was enabled, disable it. */
      TxCtrlEPList[epid].command &= ~IO_MASK(USB_EP_command, enable);
      tc_warn("sync_finish: Disabling EP for epid:%d\n", epid);

      /* Do busy-wait until DMA not using this EP descriptor anymore */
      while((*R_DMA_CH8_SUB1_EP ==
	     virt_to_phys(&TxCtrlEPList[epid])) &&
	    (timeout-- > 0));
    }
    break;

  case PIPE_INTERRUPT:
    local_irq_save(flags);
    /* Disable all Intr EPs belonging to epid */
    first_ep = &TxIntrEPList[0];
    curr_ep = first_ep;
    do {
      next_ep = (struct USB_EP_Desc *)phys_to_virt(curr_ep->next);
      if (IO_EXTRACT(USB_EP_command, epid, next_ep->command) == epid) {
	/* Disable EP */
	next_ep->command &= ~IO_MASK(USB_EP_command, enable);
      }
      curr_ep = phys_to_virt(curr_ep->next);
    } while (curr_ep != first_ep);

    local_irq_restore(flags);
    break;

  case PIPE_ISOCHRONOUS:
    /* Check if EP still is enabled */
    if (TxIsocEPList[epid].command & IO_MASK(USB_EP_command, enable)) {
      tc_warn("sync_finish: Disabling Isoc EP for epid:%d\n", epid);
      /* The EP was enabled, disable it. */
      TxIsocEPList[epid].command &= ~IO_MASK(USB_EP_command, enable);
      
      while((*R_DMA_CH8_SUB3_EP == virt_to_phys(&TxIsocEPList[epid])) &&
	    (timeout-- > 0));
    }
    break;
  }

  local_irq_save(flags);

  /* Finish if there is active URB for this endpoint */
  if(activeUrbList[epid] != NULL) {
    urb = activeUrbList[epid];
    urb_priv = urb->hcpriv;
    ASSERT(urb_priv);
    tc_warn("Sync finish %s URB:0x%x[%d] (%s %s epid:%d) status:%d %s\n",
	    (urb == activeUrbList[epid]) ? "active" : "queued",
	    (unsigned int)urb, urb_priv->urb_num, str_dir(urb->pipe),
	    str_type(urb->pipe), epid, urb->status,
	    (urb_priv->later_data) ? "later-sched" : "");

    tc_finish_urb(hcd, activeUrbList[epid], -ENOENT);
    ASSERT(activeUrbList[epid] == NULL);
  }

  /* Finish any queued URBs for this endpoint. There won't be any resubmitions
     because epid_disabled causes enqueue() to fail for this endpoint */
  while((urb = urb_list_first(epid)) != NULL) {
    urb_priv = urb->hcpriv;
    ASSERT(urb_priv);

    tc_warn("Sync finish %s URB:0x%x[%d] (%s %s epid:%d) status:%d %s\n",
	    (urb == activeUrbList[epid]) ? "active" : "queued",
	    (unsigned int)urb, urb_priv->urb_num, str_dir(urb->pipe),
	    str_type(urb->pipe), epid, urb->status,
	    (urb_priv->later_data) ? "later-sched" : "");

    tc_finish_urb(hcd, urb, -ENOENT);
  }
  epid_state[epid].disabled = 0;
  local_irq_restore(flags);
}

/* free resources associated with an endpoint (called from hcd_driver) */
static void tc_endpoint_disable(struct usb_hcd *hcd, 
				struct usb_host_endpoint *ep) {
  DBFENTER;
  /* Only free epid if it has been allocated. We get two endpoint_disable
     requests for ctrl endpoints so ignore the second one */
  if(ep->hcpriv != NULL) {
    struct crisv10_ep_priv *ep_priv = ep->hcpriv;
    int epid = ep_priv->epid;
    tc_warn("endpoint_disable ep:0x%x ep-priv:0x%x (%s) (epid:%d freed)\n",
	   (unsigned int)ep, (unsigned int)ep->hcpriv,
	   endpoint_to_str(&(ep->desc)), epid);

    tc_sync_finish_epid(hcd, epid);

    ASSERT(activeUrbList[epid] == NULL);
    ASSERT(list_empty(&urb_list[epid]));

    tc_free_epid(ep);
  } else {
    tc_dbg("endpoint_disable ep:0x%x ep-priv:0x%x (%s)\n", (unsigned int)ep,
	   (unsigned int)ep->hcpriv, endpoint_to_str(&(ep->desc)));
  }
  DBFEXIT;
}

static void tc_finish_urb_later_proc(struct work_struct* work) {
  unsigned long flags;
  struct urb_later_data* uld;

  local_irq_save(flags);
  uld = container_of(work, struct urb_later_data, dws.work);
  if(uld->urb == NULL) {
    late_dbg("Later finish of URB = NULL (allready finished)\n");
  } else {
    struct crisv10_urb_priv* urb_priv = uld->urb->hcpriv;
    ASSERT(urb_priv);
    if(urb_priv->urb_num == uld->urb_num) {
      late_dbg("Later finish of URB:0x%x[%d]\n", (unsigned int)(uld->urb),
	       urb_priv->urb_num);
      if(uld->status != uld->urb->status) {
	errno_dbg("Later-finish URB with status:%d, later-status:%d\n",
		  uld->urb->status, uld->status);
      }
      if(uld != urb_priv->later_data) {
	panic("Scheduled uld not same as URBs uld\n");
      }
      tc_finish_urb(uld->hcd, uld->urb, uld->status);
    } else {
      late_warn("Ignoring later finish of URB:0x%x[%d]"
		", urb_num doesn't match current URB:0x%x[%d]",
		(unsigned int)(uld->urb), uld->urb_num,
		(unsigned int)(uld->urb), urb_priv->urb_num);
    }
  }
  local_irq_restore(flags);
  kmem_cache_free(later_data_cache, uld);
}

static void tc_finish_urb_later(struct usb_hcd *hcd, struct urb *urb,
				int status) {
  struct crisv10_urb_priv *urb_priv = urb->hcpriv;
  struct urb_later_data* uld;

  ASSERT(urb_priv);

  if(urb_priv->later_data != NULL) {
    /* Later-finish allready scheduled for this URB, just update status to
       return when finishing later */
    errno_dbg("Later-finish schedule change URB status:%d with new"
	      " status:%d\n", urb_priv->later_data->status, status);
    
    urb_priv->later_data->status = status;
    return;
  }

  uld = kmem_cache_alloc(later_data_cache, GFP_ATOMIC);
  ASSERT(uld);

  uld->hcd = hcd;
  uld->urb = urb;
  uld->urb_num = urb_priv->urb_num;
  uld->status = status;

  INIT_DELAYED_WORK(&uld->dws, tc_finish_urb_later_proc);
  urb_priv->later_data = uld;

  /* Schedule the finishing of the URB to happen later */
  schedule_delayed_work(&uld->dws, LATER_TIMER_DELAY);
}

static void tc_finish_isoc_urb(struct usb_hcd *hcd, struct urb *urb,
			       int status);

static void tc_finish_urb(struct usb_hcd *hcd, struct urb *urb, int status) {
  struct crisv10_hcd* crisv10_hcd = hcd_to_crisv10_hcd(hcd);
  struct crisv10_urb_priv *urb_priv = urb->hcpriv;
  int epid;
  char toggle;
  int urb_num;

  DBFENTER;
  ASSERT(urb_priv != NULL);
  epid = urb_priv->epid;
  urb_num = urb_priv->urb_num;

  if(urb != activeUrbList[epid]) {
    if(urb_list_entry(urb, epid)) {
      /* Remove this URB from the list. Only happens when URB are finished
	 before having been processed (dequeing) */
      urb_list_del(urb, epid);
    } else {
      tc_warn("Finishing of URB:0x%x[%d] neither active or in queue for"
	      " epid:%d\n", (unsigned int)urb, urb_num, epid);
    }
  }

  /* Cancel any pending later-finish of this URB */
  if(urb_priv->later_data) {
    urb_priv->later_data->urb = NULL;
  }

  /* For an IN pipe, we always set the actual length, regardless of whether
     there was an error or not (which means the device driver can use the data
     if it wants to). */
  if(usb_pipein(urb->pipe)) {
    urb->actual_length = urb_priv->rx_offset;
  } else {
    /* Set actual_length for OUT urbs also; the USB mass storage driver seems
       to want that. */
    if (status == 0 && urb->status == -EINPROGRESS) {
      urb->actual_length = urb->transfer_buffer_length;
    } else {
      /*  We wouldn't know of any partial writes if there was an error. */
      urb->actual_length = 0;
    }
  }


  /* URB status mangling */
  if(urb->status == -EINPROGRESS) {
    /* The USB core hasn't changed the status, let's set our finish status */
    urb->status = status;

    if ((status == 0) && (urb->transfer_flags & URB_SHORT_NOT_OK) &&
	usb_pipein(urb->pipe) &&
	(urb->actual_length != urb->transfer_buffer_length)) {
      /* URB_SHORT_NOT_OK means that short reads (shorter than the endpoint's
	 max length) is to be treated as an error. */
      errno_dbg("Finishing URB:0x%x[%d] with SHORT_NOT_OK flag and short"
		" data:%d\n", (unsigned int)urb, urb_num,
		urb->actual_length);
      urb->status = -EREMOTEIO;
    }

    if(urb_priv->urb_state == UNLINK) {
      /* URB has been requested to be unlinked asynchronously */
      urb->status = -ECONNRESET;
      errno_dbg("Fixing unlink status of URB:0x%x[%d] to:%d\n",
		(unsigned int)urb, urb_num, urb->status);
    }
  } else {
    /* The USB Core wants to signal some error via the URB, pass it through */
  }

  /* use completely different finish function for Isoc URBs */
  if(usb_pipeisoc(urb->pipe)) {
    tc_finish_isoc_urb(hcd, urb, status);
    return;
  }

  /* Do special unlinking of EPs for Intr traffic */
  if(usb_pipeint(urb->pipe)) {
    tc_dma_unlink_intr_urb(urb);
  }

  /* Release allocated bandwidth for periodic transfers */
  if(usb_pipeint(urb->pipe) || usb_pipeisoc(urb->pipe))
    crisv10_usb_release_bandwidth(hcd,
                                  usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS,
                                  urb_priv->bandwidth);

  /* This URB is active on EP */
  if(urb == activeUrbList[epid]) {
    /* We need to fiddle with the toggle bits because the hardware doesn't do
       it for us. */
    toggle = etrax_epid_get_toggle(epid, usb_pipeout(urb->pipe));
    usb_settoggle(urb->dev, usb_pipeendpoint(urb->pipe),
		  usb_pipeout(urb->pipe), toggle);

    /* Checks for Ctrl and Bulk EPs */
    switch(usb_pipetype(urb->pipe)) {
    case PIPE_BULK:
      /* Check so Bulk EP realy is disabled before finishing active URB  */
      ASSERT((TxBulkEPList[epid].command & IO_MASK(USB_EP_command, enable)) ==
	     IO_STATE(USB_EP_command, enable, no));
      /* Disable sub-pointer for EP to avoid next tx_interrupt() to
	 process Bulk EP. */
      TxBulkEPList[epid].sub = 0;
      /* No need to wait for the DMA before changing the next pointer.
	 The modulo NBR_OF_EPIDS isn't actually necessary, since we will never use
	 the last one (INVALID_EPID) for actual traffic. */
      TxBulkEPList[epid].next = 
	virt_to_phys(&TxBulkEPList[(epid + 1) % NBR_OF_EPIDS]);
      break;
    case PIPE_CONTROL:
      /* Check so Ctrl EP realy is disabled before finishing active URB  */
      ASSERT((TxCtrlEPList[epid].command & IO_MASK(USB_EP_command, enable)) ==
	     IO_STATE(USB_EP_command, enable, no));
      /* Disable sub-pointer for EP to avoid next tx_interrupt() to
	 process Ctrl EP. */
      TxCtrlEPList[epid].sub = 0;
      break;
    }
  }

  /* Free HC-private URB data*/
  urb_priv_free(hcd, urb);

  if(urb->status) {
    errno_dbg("finish_urb (URB:0x%x[%d] %s %s) (data:%d) status:%d\n",
	      (unsigned int)urb, urb_num, str_dir(urb->pipe),
	      str_type(urb->pipe), urb->actual_length, urb->status);
  } else {
    tc_dbg("finish_urb (URB:0x%x[%d] %s %s) (data:%d) status:%d\n",
	   (unsigned int)urb, urb_num, str_dir(urb->pipe),
	   str_type(urb->pipe), urb->actual_length, urb->status);
  }

  /* If we just finished an active URB, clear active pointer. */
  if (urb == activeUrbList[epid]) {
    /* Make URB not active on EP anymore */
    activeUrbList[epid] = NULL;

    if(urb->status == 0) {
      /* URB finished sucessfully, process queue to see if there are any more
	 URBs waiting before we call completion function.*/
      if(crisv10_hcd->running) {
	/* Only process queue if USB controller is running */
	tc_dma_process_queue(epid);
      } else {
	tc_warn("No processing of queue for epid:%d, USB Controller not"
		" running\n", epid);
      }
    }
  }

  /*  Hand the URB from HCD to its USB device driver, using its completion
      functions */
  usb_hcd_giveback_urb (hcd, urb, status);

  /* Check the queue once more if the URB returned with error, because we
     didn't do it before the completion function because the specification
     states that the queue should not restart until all it's unlinked
     URBs have been fully retired, with the completion functions run */
  if(crisv10_hcd->running) {
    /* Only process queue if USB controller is running */
    tc_dma_process_queue(epid);
  } else {
    tc_warn("No processing of queue for epid:%d, USB Controller not running\n",
	    epid);
  }

  DBFEXIT;
}

static void tc_finish_isoc_urb(struct usb_hcd *hcd, struct urb *urb,
			       int status) {
  struct crisv10_urb_priv *urb_priv = urb->hcpriv;
  int epid, i;
  volatile int timeout = 10000;
  int bandwidth = 0;

  ASSERT(urb_priv);
  epid = urb_priv->epid;

  ASSERT(usb_pipeisoc(urb->pipe));

  /* Set that all isoc packets have status and length set before
     completing the urb. */
  for (i = urb_priv->isoc_packet_counter; i < urb->number_of_packets; i++){
    urb->iso_frame_desc[i].actual_length = 0;
    urb->iso_frame_desc[i].status = -EPROTO;
  }

  /* Check if the URB is currently active (done or error) */
  if(urb == activeUrbList[epid]) {
    /* Check if there are another In Isoc URB queued for this epid */
    if (!list_empty(&urb_list[epid])&& !epid_state[epid].disabled) {
      /* Move it from queue to active and mark it started so Isoc transfers
	 won't be interrupted.
	 All Isoc URBs data transfers are already added to DMA lists so we
	 don't have to insert anything in DMA lists here. */
      activeUrbList[epid] = urb_list_first(epid);
      ((struct crisv10_urb_priv *)(activeUrbList[epid]->hcpriv))->urb_state =
	STARTED;
      urb_list_del(activeUrbList[epid], epid);

      if(urb->status) {
	errno_dbg("finish_isoc_urb (URB:0x%x[%d] %s %s) (%d of %d packets)"
		  " status:%d, new waiting URB:0x%x[%d]\n",
		  (unsigned int)urb, urb_priv->urb_num, str_dir(urb->pipe),
		  str_type(urb->pipe), urb_priv->isoc_packet_counter,
		  urb->number_of_packets, urb->status,
		  (unsigned int)activeUrbList[epid],
		  ((struct crisv10_urb_priv *)(activeUrbList[epid]->hcpriv))->urb_num);
      }

    } else { /* No other URB queued for this epid */
      if(urb->status) {
	errno_dbg("finish_isoc_urb (URB:0x%x[%d] %s %s) (%d of %d packets)"
		  " status:%d, no new URB waiting\n",
		  (unsigned int)urb, urb_priv->urb_num, str_dir(urb->pipe),
		  str_type(urb->pipe), urb_priv->isoc_packet_counter,
		  urb->number_of_packets, urb->status);
      }

      /* Check if EP is still enabled, then shut it down. */
      if (TxIsocEPList[epid].command & IO_MASK(USB_EP_command, enable)) {
	isoc_dbg("Isoc EP enabled for epid:%d, disabling it\n", epid);

	/* Should only occur for In Isoc EPs where SB isn't consumed. */
	ASSERT(usb_pipein(urb->pipe));

	/* Disable it and wait for it to stop */
	TxIsocEPList[epid].command &= ~IO_MASK(USB_EP_command, enable);
	
	/* Ah, the luxury of busy-wait. */
	while((*R_DMA_CH8_SUB3_EP == virt_to_phys(&TxIsocEPList[epid])) &&
	      (timeout-- > 0));
      }

      /* Unlink SB to say that epid is finished. */
      TxIsocEPList[epid].sub = 0;
      TxIsocEPList[epid].hw_len = 0;

      /* No URB active for EP anymore */
      activeUrbList[epid] = NULL;
    }
  } else { /* Finishing of not active URB (queued up with SBs thought) */
    isoc_warn("finish_isoc_urb (URB:0x%x %s) (%d of %d packets) status:%d,"
	      " SB queued but not active\n",
	      (unsigned int)urb, str_dir(urb->pipe),
	      urb_priv->isoc_packet_counter, urb->number_of_packets,
	      urb->status);
    if(usb_pipeout(urb->pipe)) {
      /* Finishing of not yet active Out Isoc URB needs unlinking of SBs. */
      struct USB_SB_Desc *iter_sb, *prev_sb, *next_sb;

      iter_sb = TxIsocEPList[epid].sub ?
	phys_to_virt(TxIsocEPList[epid].sub) : 0;
      prev_sb = 0;

      /* SB that is linked before this URBs first SB */
      while (iter_sb && (iter_sb != urb_priv->first_sb)) {
	prev_sb = iter_sb;
	iter_sb = iter_sb->next ? phys_to_virt(iter_sb->next) : 0;
      }

      if (iter_sb == 0) {
	/* Unlink of the URB currently being transmitted. */
	prev_sb = 0;
	iter_sb = TxIsocEPList[epid].sub ? phys_to_virt(TxIsocEPList[epid].sub) : 0;
      }

      while (iter_sb && (iter_sb != urb_priv->last_sb)) {
	iter_sb = iter_sb->next ? phys_to_virt(iter_sb->next) : 0;
      }

      if (iter_sb) {
	next_sb = iter_sb->next ? phys_to_virt(iter_sb->next) : 0;
      } else {
	/* This should only happen if the DMA has completed
	   processing the SB list for this EP while interrupts
	   are disabled. */
	isoc_dbg("Isoc urb not found, already sent?\n");
	next_sb = 0;
      }
      if (prev_sb) {
	prev_sb->next = next_sb ? virt_to_phys(next_sb) : 0;
      } else {
	TxIsocEPList[epid].sub = next_sb ? virt_to_phys(next_sb) : 0;
      }
    }
  }

  /* Free HC-private URB data*/
  bandwidth = urb_priv->bandwidth;
  urb_priv_free(hcd, urb);

  crisv10_usb_release_bandwidth(hcd, usb_pipeisoc(urb->pipe), bandwidth);

  /*  Hand the URB from HCD to its USB device driver, using its completion
      functions */
  usb_hcd_giveback_urb (hcd, urb, status);
}

static __u32 urb_num = 0;

/* allocate and initialize URB private data */
static int urb_priv_create(struct usb_hcd *hcd, struct urb *urb, int epid,
			   int mem_flags) {
  struct crisv10_urb_priv *urb_priv;
  
  urb_priv = kmalloc(sizeof *urb_priv, mem_flags);
  if (!urb_priv)
    return -ENOMEM;
  memset(urb_priv, 0, sizeof *urb_priv);

  urb_priv->epid = epid;
  urb_priv->urb_state = NOT_STARTED;

  urb->hcpriv = urb_priv;
  /* Assign URB a sequence number, and increment counter */
  urb_priv->urb_num = urb_num;
  urb_num++;
  urb_priv->bandwidth = 0;
  return 0;
}

/* free URB private data */
static void urb_priv_free(struct usb_hcd *hcd, struct urb *urb) {
  int i;
  struct crisv10_urb_priv *urb_priv = urb->hcpriv;
  ASSERT(urb_priv != 0);

  /* Check it has any SBs linked that needs to be freed*/
  if(urb_priv->first_sb != NULL) {
    struct USB_SB_Desc *next_sb, *first_sb, *last_sb;
    int i = 0;
    first_sb = urb_priv->first_sb;
    last_sb = urb_priv->last_sb;
    ASSERT(last_sb);
    while(first_sb != last_sb) {
      next_sb = (struct USB_SB_Desc *)phys_to_virt(first_sb->next);
      kmem_cache_free(usb_desc_cache, first_sb);
      first_sb = next_sb;
      i++;
    }
    kmem_cache_free(usb_desc_cache, last_sb);
    i++;
  }

  /* Check if it has any EPs in its Intr pool that also needs to be freed */
  if(urb_priv->intr_ep_pool_length > 0) {
    for(i = 0; i < urb_priv->intr_ep_pool_length; i++) {
      kfree(urb_priv->intr_ep_pool[i]);
    }
    /*
    tc_dbg("Freed %d EPs from URB:0x%x EP pool\n",
	     urb_priv->intr_ep_pool_length, (unsigned int)urb);
    */
  }

  kfree(urb_priv);
  urb->hcpriv = NULL;
}

static int ep_priv_create(struct usb_host_endpoint *ep, int mem_flags) {
  struct crisv10_ep_priv *ep_priv;
  
  ep_priv = kmalloc(sizeof *ep_priv, mem_flags);
  if (!ep_priv)
    return -ENOMEM;
  memset(ep_priv, 0, sizeof *ep_priv);

  ep->hcpriv = ep_priv;
  return 0;
}

static void ep_priv_free(struct usb_host_endpoint *ep) {
  struct crisv10_ep_priv *ep_priv = ep->hcpriv;
  ASSERT(ep_priv);
  kfree(ep_priv);
  ep->hcpriv = NULL;
}

/*
 * usb_check_bandwidth():
 *
 * old_alloc is from host_controller->bandwidth_allocated in microseconds;
 * bustime is from calc_bus_time(), but converted to microseconds.
 *
 * returns <bustime in us> if successful,
 * or -ENOSPC if bandwidth request fails.
 *
 * FIXME:
 * This initial implementation does not use Endpoint.bInterval
 * in managing bandwidth allocation.
 * It probably needs to be expanded to use Endpoint.bInterval.
 * This can be done as a later enhancement (correction).
 *
 * This will also probably require some kind of
 * frame allocation tracking...meaning, for example,
 * that if multiple drivers request interrupts every 10 USB frames,
 * they don't all have to be allocated at
 * frame numbers N, N+10, N+20, etc.  Some of them could be at
 * N+11, N+21, N+31, etc., and others at
 * N+12, N+22, N+32, etc.
 *
 * Similarly for isochronous transfers...
 *
 * Individual HCDs can schedule more directly ... this logic
 * is not correct for high speed transfers.
 */
static int crisv10_usb_check_bandwidth(
  struct usb_device *dev,
  struct urb *urb)
{
  unsigned int	pipe = urb->pipe;
  long					bustime;
  int						is_in = usb_pipein (pipe);
  int						is_iso = usb_pipeisoc (pipe);
  int						old_alloc = dev->bus->bandwidth_allocated;
  int						new_alloc;

  bustime = NS_TO_US (usb_calc_bus_time (dev->speed, is_in, is_iso,
                                         usb_maxpacket (dev, pipe, !is_in)));
  if (is_iso)
    bustime /= urb->number_of_packets;

  new_alloc = old_alloc + (int) bustime;
  if (new_alloc > FRAME_TIME_MAX_USECS_ALLOC) {
    dev_dbg (&dev->dev, "usb_check_bandwidth FAILED: %d + %ld = %d usec\n",
             old_alloc, bustime, new_alloc);
    bustime = -ENOSPC;      /* report error */
  }

  return bustime;
}

/**
 * usb_claim_bandwidth - records bandwidth for a periodic transfer
 * @dev: source/target of request
 * @urb: request (urb->dev == dev)
 * @bustime: bandwidth consumed, in (average) microseconds per frame
 * @isoc: true iff the request is isochronous
 *
 * HCDs are expected not to overcommit periodic bandwidth, and to record such
 * reservations whenever endpoints are added to the periodic schedule.
 *
 * FIXME averaging per-frame is suboptimal.  Better to sum over the HCD's
 * entire periodic schedule ... 32 frames for OHCI, 1024 for UHCI, settable
 * for EHCI (256/512/1024 frames, default 1024) and have the bus expose how
 * large its periodic schedule is.
 */
static void crisv10_usb_claim_bandwidth(
  struct usb_device *dev,
  struct urb *urb, int bustime, int isoc)
{
  dev->bus->bandwidth_allocated += bustime;
  if (isoc)
    dev->bus->bandwidth_isoc_reqs++;
  else
    dev->bus->bandwidth_int_reqs++;
  struct crisv10_urb_priv *urb_priv;
  urb_priv = urb->hcpriv;
  urb_priv->bandwidth = bustime;
}

/**
 * usb_release_bandwidth - reverses effect of usb_claim_bandwidth()
 * @hcd: host controller
 * @isoc: true iff the request is isochronous
 * @bandwidth: bandwidth returned
 *
 * This records that previously allocated bandwidth has been released.
 * Bandwidth is released when endpoints are removed from the host controller's
 * periodic schedule.
 */
static void crisv10_usb_release_bandwidth(
  struct usb_hcd *hcd,
  int isoc,
  int bandwidth)
{
  hcd_to_bus(hcd)->bandwidth_allocated -= bandwidth;
  if (isoc)
    hcd_to_bus(hcd)->bandwidth_isoc_reqs--;
  else
    hcd_to_bus(hcd)->bandwidth_int_reqs--;
}


/* EPID handling functions, managing EP-list in Etrax through wrappers */
/* ------------------------------------------------------------------- */

/* Sets up a new EPID for an endpoint or returns existing if found */
static int tc_setup_epid(struct usb_host_endpoint *ep, struct urb *urb,
			 int mem_flags) {
  int epid;
  char devnum, endpoint, out_traffic, slow;
  int maxlen;
  __u32 epid_data;
  struct crisv10_ep_priv *ep_priv = ep->hcpriv;
  
  DBFENTER;
  
  /* Check if a valid epid already is setup for this endpoint */
  if(ep_priv != NULL) {
    return ep_priv->epid;
  }

  /* We must find and initiate a new epid for this urb. */
  epid = tc_allocate_epid();
  
  if (epid == -1) {
    /* Failed to allocate a new epid. */
    DBFEXIT;
    return epid;
  }
  
  /* We now have a new epid to use. Claim it. */
  epid_state[epid].inuse = 1;
  
  /* Init private data for new endpoint */
  if(ep_priv_create(ep, mem_flags) != 0) {
    return -ENOMEM;
  }
  ep_priv = ep->hcpriv;
  ep_priv->epid = epid;

  devnum = usb_pipedevice(urb->pipe);
  endpoint = usb_pipeendpoint(urb->pipe);
  slow = (urb->dev->speed == USB_SPEED_LOW);
  maxlen = usb_maxpacket(urb->dev, urb->pipe, usb_pipeout(urb->pipe));

  if (usb_pipetype(urb->pipe) == PIPE_CONTROL) {
    /* We want both IN and OUT control traffic to be put on the same
       EP/SB list. */
    out_traffic = 1;
  } else {
    out_traffic = usb_pipeout(urb->pipe);
  }
    
  if (usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS) {
    epid_data = IO_STATE(R_USB_EPT_DATA_ISO, valid, yes) |
      /* FIXME: Change any to the actual port? */
      IO_STATE(R_USB_EPT_DATA_ISO, port, any) |
      IO_FIELD(R_USB_EPT_DATA_ISO, max_len, maxlen) |
      IO_FIELD(R_USB_EPT_DATA_ISO, ep, endpoint) |
      IO_FIELD(R_USB_EPT_DATA_ISO, dev, devnum);
    etrax_epid_iso_set(epid, epid_data);
  } else {
    epid_data = IO_STATE(R_USB_EPT_DATA, valid, yes) |
      IO_FIELD(R_USB_EPT_DATA, low_speed, slow) |
      /* FIXME: Change any to the actual port? */
      IO_STATE(R_USB_EPT_DATA, port, any) |
      IO_FIELD(R_USB_EPT_DATA, max_len, maxlen) |
      IO_FIELD(R_USB_EPT_DATA, ep, endpoint) |
      IO_FIELD(R_USB_EPT_DATA, dev, devnum);
    etrax_epid_set(epid, epid_data);
  }
  
  epid_state[epid].out_traffic = out_traffic;
  epid_state[epid].type = usb_pipetype(urb->pipe);

  tc_warn("Setting up ep:0x%x epid:%d (addr:%d endp:%d max_len:%d %s %s %s)\n",
	  (unsigned int)ep, epid, devnum, endpoint, maxlen,
	  str_type(urb->pipe), out_traffic ? "out" : "in",
	  slow ? "low" : "full");

  /* Enable Isoc eof interrupt if we set up the first Isoc epid */
  if(usb_pipeisoc(urb->pipe)) {
    isoc_epid_counter++;
    if(isoc_epid_counter == 1) {
      isoc_warn("Enabled Isoc eof interrupt\n");
      *R_USB_IRQ_MASK_SET = IO_STATE(R_USB_IRQ_MASK_SET, iso_eof, set);
    }
  }

  DBFEXIT;
  return epid;
}

static void tc_free_epid(struct usb_host_endpoint *ep) {
  unsigned long flags;
  struct crisv10_ep_priv *ep_priv = ep->hcpriv;
  int epid;
  volatile int timeout = 10000;

  DBFENTER;

  if (ep_priv == NULL) {
    tc_warn("Trying to free unused epid on ep:0x%x\n", (unsigned int)ep);
    DBFEXIT;
    return;
  }

  epid = ep_priv->epid;

  /* Disable Isoc eof interrupt if we free the last Isoc epid */
  if(epid_isoc(epid)) {
    ASSERT(isoc_epid_counter > 0);
    isoc_epid_counter--;
    if(isoc_epid_counter == 0) {
      *R_USB_IRQ_MASK_CLR = IO_STATE(R_USB_IRQ_MASK_CLR, iso_eof, clr);
      isoc_warn("Disabled Isoc eof interrupt\n");
    }
  }

  /* Take lock manualy instead of in epid_x_x wrappers,
     because we need to be polling here */
  spin_lock_irqsave(&etrax_epid_lock, flags);
  
  *R_USB_EPT_INDEX = IO_FIELD(R_USB_EPT_INDEX, value, epid);
  nop();
  while((*R_USB_EPT_DATA & IO_MASK(R_USB_EPT_DATA, hold)) &&
	(timeout-- > 0));
  /* This will, among other things, set the valid field to 0. */
  *R_USB_EPT_DATA = 0;
  spin_unlock_irqrestore(&etrax_epid_lock, flags);
  
  /* Free resource in software state info list */
  epid_state[epid].inuse = 0;

  /* Free private endpoint data */
  ep_priv_free(ep);
  
  DBFEXIT;
}

static int tc_allocate_epid(void) {
  int i;
  DBFENTER;
  for (i = 0; i < NBR_OF_EPIDS; i++) {
    if (!epid_inuse(i)) {
      DBFEXIT;
      return i;
    }
  }
  
  tc_warn("Found no free epids\n");
  DBFEXIT;
  return -1;
}


/* Wrappers around the list functions (include/linux/list.h). */
/* ---------------------------------------------------------- */
static inline int __urb_list_empty(int epid) {
  int retval;
  retval = list_empty(&urb_list[epid]);
  return retval;
}

/* Returns first urb for this epid, or NULL if list is empty. */
static inline struct urb *urb_list_first(int epid) {
  unsigned long flags;
  struct urb *first_urb = 0;
  spin_lock_irqsave(&urb_list_lock, flags);
  if (!__urb_list_empty(epid)) {
    /* Get the first urb (i.e. head->next). */
    urb_entry_t *urb_entry = list_entry((&urb_list[epid])->next, urb_entry_t, list);
    first_urb = urb_entry->urb;
  }
  spin_unlock_irqrestore(&urb_list_lock, flags);
  return first_urb;
}

/* Adds an urb_entry last in the list for this epid. */
static inline void urb_list_add(struct urb *urb, int epid, int mem_flags) {
  unsigned long flags;
  urb_entry_t *urb_entry = (urb_entry_t *)kmalloc(sizeof(urb_entry_t), mem_flags);
  ASSERT(urb_entry);
  
  urb_entry->urb = urb;
  spin_lock_irqsave(&urb_list_lock, flags);
  list_add_tail(&urb_entry->list, &urb_list[epid]);
  spin_unlock_irqrestore(&urb_list_lock, flags);
}

/* Search through the list for an element that contains this urb. (The list
   is expected to be short and the one we are about to delete will often be
   the first in the list.)
   Should be protected by spin_locks in calling function */
static inline urb_entry_t *__urb_list_entry(struct urb *urb, int epid) {
  struct list_head *entry;
  struct list_head *tmp;
  urb_entry_t *urb_entry;
  
  list_for_each_safe(entry, tmp, &urb_list[epid]) {
    urb_entry = list_entry(entry, urb_entry_t, list);
    ASSERT(urb_entry);
    ASSERT(urb_entry->urb);
    
    if (urb_entry->urb == urb) {
      return urb_entry;
    }
  }
  return 0;
}

/* Same function as above but for global use. Protects list by spinlock */
static inline urb_entry_t *urb_list_entry(struct urb *urb, int epid) {
  unsigned long flags;
  urb_entry_t *urb_entry;
  spin_lock_irqsave(&urb_list_lock, flags);
  urb_entry = __urb_list_entry(urb, epid);
  spin_unlock_irqrestore(&urb_list_lock, flags);
  return (urb_entry);
}

/* Delete an urb from the list. */
static inline void urb_list_del(struct urb *urb, int epid) {
  unsigned long flags;
  urb_entry_t *urb_entry;

  /* Delete entry and free. */
  spin_lock_irqsave(&urb_list_lock, flags);
  urb_entry = __urb_list_entry(urb, epid);
  ASSERT(urb_entry);

  list_del(&urb_entry->list);
  spin_unlock_irqrestore(&urb_list_lock, flags);
  kfree(urb_entry);
}

/* Move an urb to the end of the list. */
static inline void urb_list_move_last(struct urb *urb, int epid) {
  unsigned long flags;
  urb_entry_t *urb_entry;
  
  spin_lock_irqsave(&urb_list_lock, flags);
  urb_entry = __urb_list_entry(urb, epid);
  ASSERT(urb_entry);

  list_del(&urb_entry->list);
  list_add_tail(&urb_entry->list, &urb_list[epid]);
  spin_unlock_irqrestore(&urb_list_lock, flags);
}

/* Get the next urb in the list. */
static inline struct urb *urb_list_next(struct urb *urb, int epid) {
  unsigned long flags;
  urb_entry_t *urb_entry;

  spin_lock_irqsave(&urb_list_lock, flags);
  urb_entry = __urb_list_entry(urb, epid);
  ASSERT(urb_entry);

  if (urb_entry->list.next != &urb_list[epid]) {
    struct list_head *elem = urb_entry->list.next;
    urb_entry = list_entry(elem, urb_entry_t, list);
    spin_unlock_irqrestore(&urb_list_lock, flags);
    return urb_entry->urb;
  } else {
    spin_unlock_irqrestore(&urb_list_lock, flags);
    return NULL;
  }
}

struct USB_EP_Desc* create_ep(int epid, struct USB_SB_Desc* sb_desc,
			      int mem_flags) {
  struct USB_EP_Desc *ep_desc;
  ep_desc = (struct USB_EP_Desc *) kmem_cache_alloc(usb_desc_cache, mem_flags);
  if(ep_desc == NULL)
    return NULL;
  memset(ep_desc, 0, sizeof(struct USB_EP_Desc));

  ep_desc->hw_len = 0;
  ep_desc->command = (IO_FIELD(USB_EP_command, epid, epid) |
		      IO_STATE(USB_EP_command, enable, yes));
  if(sb_desc == NULL) {
    ep_desc->sub = 0;
  } else {
    ep_desc->sub = virt_to_phys(sb_desc);
  }
  return ep_desc;
}

#define TT_ZOUT  0
#define TT_IN    1
#define TT_OUT   2
#define TT_SETUP 3

#define CMD_EOL  IO_STATE(USB_SB_command, eol, yes)
#define CMD_INTR IO_STATE(USB_SB_command, intr, yes)
#define CMD_FULL IO_STATE(USB_SB_command, full, yes)

/* Allocation and setup of a generic SB. Used to create SETUP, OUT and ZOUT
   SBs. Also used by create_sb_in() to avoid same allocation procedure at two
   places */
struct USB_SB_Desc* create_sb(struct USB_SB_Desc* sb_prev, int tt, void* data,
			      int datalen, int mem_flags) {
  struct USB_SB_Desc *sb_desc;
  sb_desc = (struct USB_SB_Desc*)kmem_cache_alloc(usb_desc_cache, mem_flags);
  if(sb_desc == NULL)
    return NULL;
  memset(sb_desc, 0, sizeof(struct USB_SB_Desc));

  sb_desc->command = IO_FIELD(USB_SB_command, tt, tt) |
                     IO_STATE(USB_SB_command, eot, yes);

  sb_desc->sw_len = datalen;
  if(data != NULL) {
    sb_desc->buf = virt_to_phys(data);
  } else {
    sb_desc->buf = 0;
  }
  if(sb_prev != NULL) {
    sb_prev->next = virt_to_phys(sb_desc);
  }
  return sb_desc;
}

/* Creates a copy of an existing SB by allocation space for it and copy
   settings */
struct USB_SB_Desc* create_sb_copy(struct USB_SB_Desc* sb_orig, int mem_flags) {
  struct USB_SB_Desc *sb_desc;
  sb_desc = (struct USB_SB_Desc*)kmem_cache_alloc(usb_desc_cache, mem_flags);
  if(sb_desc == NULL)
    return NULL;

  memcpy(sb_desc, sb_orig, sizeof(struct USB_SB_Desc));
  return sb_desc;
}

/* A specific create_sb function for creation of in SBs. This is due to
   that datalen in In SBs shows how many packets we are expecting. It also
   sets up the rem field to show if how many bytes we expect in last packet
   if it's not a full one */
struct USB_SB_Desc* create_sb_in(struct USB_SB_Desc* sb_prev, int datalen,
				 int maxlen, int mem_flags) {
  struct USB_SB_Desc *sb_desc;
  sb_desc = create_sb(sb_prev, TT_IN, NULL,
		      datalen ? (datalen - 1) / maxlen + 1 : 0, mem_flags);
  if(sb_desc == NULL)
    return NULL;
  sb_desc->command |= IO_FIELD(USB_SB_command, rem, datalen % maxlen);
  return sb_desc;
}

void set_sb_cmds(struct USB_SB_Desc *sb_desc, __u16 flags) {
  sb_desc->command |= flags;
}

int create_sb_for_urb(struct urb *urb, int mem_flags) {
  int is_out = !usb_pipein(urb->pipe);
  int type = usb_pipetype(urb->pipe);
  int maxlen = usb_maxpacket(urb->dev, urb->pipe, is_out);
  int buf_len = urb->transfer_buffer_length;
  void *buf = buf_len > 0 ? urb->transfer_buffer : NULL;
  struct USB_SB_Desc *sb_desc = NULL;

  struct crisv10_urb_priv *urb_priv = (struct crisv10_urb_priv *)urb->hcpriv;
  ASSERT(urb_priv != NULL);

  switch(type) {
  case PIPE_CONTROL:
    /* Setup stage */
    sb_desc = create_sb(NULL, TT_SETUP, urb->setup_packet, 8, mem_flags);
    if(sb_desc == NULL)
      return -ENOMEM;
    set_sb_cmds(sb_desc, CMD_FULL);

    /* Attach first SB to URB */
    urb_priv->first_sb = sb_desc;    

    if (is_out) { /* Out Control URB */
      /* If this Control OUT transfer has an optional data stage we add
	 an OUT token before the mandatory IN (status) token */
      if ((buf_len > 0) && buf) {
	sb_desc = create_sb(sb_desc, TT_OUT, buf, buf_len, mem_flags);
	if(sb_desc == NULL)
	  return -ENOMEM;
	set_sb_cmds(sb_desc, CMD_FULL);
      }

      /* Status stage */
      /* The data length has to be exactly 1. This is due to a requirement
         of the USB specification that a host must be prepared to receive
         data in the status phase */
      sb_desc = create_sb(sb_desc, TT_IN, NULL, 1, mem_flags);
      if(sb_desc == NULL)
	return -ENOMEM;
    } else { /* In control URB */
      /* Data stage */
      sb_desc = create_sb_in(sb_desc, buf_len, maxlen, mem_flags);
      if(sb_desc == NULL)
	return -ENOMEM;

      /* Status stage */
      /* Read comment at zout_buffer declaration for an explanation to this. */
      sb_desc = create_sb(sb_desc, TT_ZOUT, &zout_buffer[0], 1, mem_flags);
      if(sb_desc == NULL)
	return -ENOMEM;
      /* Set descriptor interrupt flag for in URBs so we can finish URB after
         zout-packet has been sent */
      set_sb_cmds(sb_desc, CMD_INTR | CMD_FULL);
    }
    /* Set end-of-list flag in last SB */
    set_sb_cmds(sb_desc, CMD_EOL);
    /* Attach last SB to URB */
    urb_priv->last_sb = sb_desc;
    break;

  case PIPE_BULK:
    if (is_out) { /* Out Bulk URB */
      sb_desc = create_sb(NULL, TT_OUT, buf, buf_len, mem_flags);
      if(sb_desc == NULL)
	return -ENOMEM;
      /* The full field is set to yes, even if we don't actually check that
	 this is a full-length transfer (i.e., that transfer_buffer_length %
	 maxlen = 0).
	 Setting full prevents the USB controller from sending an empty packet
	 in that case.  However, if URB_ZERO_PACKET was set we want that. */
      if (!(urb->transfer_flags & URB_ZERO_PACKET)) {
	set_sb_cmds(sb_desc, CMD_FULL);
      }
    } else { /* In Bulk URB */
      sb_desc = create_sb_in(NULL, buf_len, maxlen, mem_flags);
      if(sb_desc == NULL)
	return -ENOMEM;
    }
    /* Set end-of-list flag for last SB */
    set_sb_cmds(sb_desc, CMD_EOL);

    /* Attach SB to URB */
    urb_priv->first_sb = sb_desc;
    urb_priv->last_sb = sb_desc;
    break;

  case PIPE_INTERRUPT:
    if(is_out) { /* Out Intr URB */
      sb_desc = create_sb(NULL, TT_OUT, buf, buf_len, mem_flags);
      if(sb_desc == NULL)
	return -ENOMEM;

      /* The full field is set to yes, even if we don't actually check that
	 this is a full-length transfer (i.e., that transfer_buffer_length %
	 maxlen = 0).
	 Setting full prevents the USB controller from sending an empty packet
	 in that case.  However, if URB_ZERO_PACKET was set we want that. */
      if (!(urb->transfer_flags & URB_ZERO_PACKET)) {
	set_sb_cmds(sb_desc, CMD_FULL);
      }
      /* Only generate TX interrupt if it's a Out URB*/
      set_sb_cmds(sb_desc, CMD_INTR);

    } else { /* In Intr URB */
      sb_desc = create_sb_in(NULL, buf_len, maxlen, mem_flags);
      if(sb_desc == NULL)
	return -ENOMEM;
    }
    /* Set end-of-list flag for last SB */
    set_sb_cmds(sb_desc, CMD_EOL);

    /* Attach SB to URB */
    urb_priv->first_sb = sb_desc;
    urb_priv->last_sb = sb_desc;

    break;
  case PIPE_ISOCHRONOUS:
    if(is_out) { /* Out Isoc URB */
      int i;
      if(urb->number_of_packets == 0) {
	tc_err("Can't create SBs for Isoc URB with zero packets\n");
	return -EPIPE;
      }
      /* Create one SB descriptor for each packet and link them together. */
      for(i = 0; i < urb->number_of_packets; i++) {
	if (urb->iso_frame_desc[i].length > 0) {

	  sb_desc = create_sb(sb_desc, TT_OUT, urb->transfer_buffer +
			      urb->iso_frame_desc[i].offset,
			      urb->iso_frame_desc[i].length, mem_flags);
	  if(sb_desc == NULL)
	    return -ENOMEM;

	  /* Check if it's a full length packet */
	  if (urb->iso_frame_desc[i].length ==
	      usb_maxpacket(urb->dev, urb->pipe, usb_pipeout(urb->pipe))) {
	    set_sb_cmds(sb_desc, CMD_FULL);
	  }
	  
	} else { /* zero length packet */
	  sb_desc = create_sb(sb_desc, TT_ZOUT, &zout_buffer[0], 1, mem_flags);
	  if(sb_desc == NULL)
	    return -ENOMEM;
	  set_sb_cmds(sb_desc, CMD_FULL);
	}
	/* Attach first SB descriptor to URB */
	if (i == 0) {
	  urb_priv->first_sb = sb_desc;
	}
      }
      /* Set interrupt and end-of-list flags in last SB */
      set_sb_cmds(sb_desc, CMD_INTR | CMD_EOL);
      /* Attach last SB descriptor to URB */
      urb_priv->last_sb = sb_desc;
      tc_dbg("Created %d out SBs for Isoc URB:0x%x\n",
	       urb->number_of_packets, (unsigned int)urb);
    } else { /* In Isoc URB */
      /* Actual number of packets is not relevant for periodic in traffic as
	 long as it is more than zero.  Set to 1 always. */
      sb_desc = create_sb(sb_desc, TT_IN, NULL, 1, mem_flags);
      if(sb_desc == NULL)
	return -ENOMEM;
      /* Set end-of-list flags for SB */
      set_sb_cmds(sb_desc, CMD_EOL);

      /* Attach SB to URB */
      urb_priv->first_sb = sb_desc;
      urb_priv->last_sb = sb_desc;
    }
    break;
  default:
    tc_err("Unknown pipe-type\n");
    return -EPIPE;
    break;
  }
  return 0;
}

int init_intr_urb(struct urb *urb, int mem_flags) {
  struct crisv10_urb_priv *urb_priv = (struct crisv10_urb_priv *)urb->hcpriv;
  struct USB_EP_Desc* ep_desc;
  int interval;
  int i;
  int ep_count;

  ASSERT(urb_priv != NULL);
  ASSERT(usb_pipeint(urb->pipe));
  /* We can't support interval longer than amount of eof descriptors in
     TxIntrEPList */
  if(urb->interval > MAX_INTR_INTERVAL) {
    tc_err("Interrupt interval %dms too big (max: %dms)\n", urb->interval,
	   MAX_INTR_INTERVAL);
    return -EINVAL;
  }

  /* We assume that the SB descriptors already have been setup */
  ASSERT(urb_priv->first_sb != NULL);

  /* Round of the interval to 2^n, it is obvious that this code favours
     smaller numbers, but that is actually a good thing */
  /* FIXME: The "rounding error" for larger intervals will be quite
     large. For in traffic this shouldn't be a problem since it will only
     mean that we "poll" more often. */
  interval = urb->interval;
  for (i = 0; interval; i++) {
    interval = interval >> 1;
  }
  urb_priv->interval = 1 << (i - 1);

  /* We can only have max interval for Out Interrupt due to that we can only
     handle one linked in EP for a certain epid in the Intr descr array at the
     time. The USB Controller in the Etrax 100LX continues to process Intr EPs
     so we have no way of knowing which one that caused the actual transfer if
     we have several linked in. */
  if(usb_pipeout(urb->pipe)) {
    urb_priv->interval = MAX_INTR_INTERVAL;
  }

  /* Calculate amount of EPs needed */
  ep_count = MAX_INTR_INTERVAL / urb_priv->interval;

  for(i = 0; i < ep_count; i++) {
    ep_desc = create_ep(urb_priv->epid, urb_priv->first_sb, mem_flags);
    if(ep_desc == NULL) {
      /* Free any descriptors that we may have allocated before failure */
      while(i > 0) {
	i--;
	kfree(urb_priv->intr_ep_pool[i]);
      }
      return -ENOMEM;
    }
    urb_priv->intr_ep_pool[i] = ep_desc;
  }
  urb_priv->intr_ep_pool_length = ep_count;
  return 0;
}

/* DMA RX/TX functions */
/* ----------------------- */

static void tc_dma_init_rx_list(void) {
  int i;

  /* Setup descriptor list except last one */
  for (i = 0; i < (NBR_OF_RX_DESC - 1); i++) {
    RxDescList[i].sw_len = RX_DESC_BUF_SIZE;
    RxDescList[i].command = 0;
    RxDescList[i].next = virt_to_phys(&RxDescList[i + 1]);
    RxDescList[i].buf = virt_to_phys(RxBuf + (i * RX_DESC_BUF_SIZE));
    RxDescList[i].hw_len = 0;
    RxDescList[i].status = 0;
    
    /* DMA IN cache bug. (struct etrax_dma_descr has the same layout as
       USB_IN_Desc for the relevant fields.) */
    prepare_rx_descriptor((struct etrax_dma_descr*)&RxDescList[i]);
    
  }
  /* Special handling of last descriptor */
  RxDescList[i].sw_len = RX_DESC_BUF_SIZE;
  RxDescList[i].command = IO_STATE(USB_IN_command, eol, yes);
  RxDescList[i].next = virt_to_phys(&RxDescList[0]);
  RxDescList[i].buf = virt_to_phys(RxBuf + (i * RX_DESC_BUF_SIZE));
  RxDescList[i].hw_len = 0;
  RxDescList[i].status = 0;
  
  /* Setup list pointers that show progress in list */
  myNextRxDesc = &RxDescList[0];
  myLastRxDesc = &RxDescList[NBR_OF_RX_DESC - 1];
  
  flush_etrax_cache();
  /* Point DMA to first descriptor in list and start it */
  *R_DMA_CH9_FIRST = virt_to_phys(myNextRxDesc);
  *R_DMA_CH9_CMD = IO_STATE(R_DMA_CH9_CMD, cmd, start);
}


static void tc_dma_init_tx_bulk_list(void) {
  int i;
  volatile struct USB_EP_Desc *epDescr;

  for (i = 0; i < (NBR_OF_EPIDS - 1); i++) {
    epDescr = &(TxBulkEPList[i]);
    CHECK_ALIGN(epDescr);
    epDescr->hw_len = 0;
    epDescr->command = IO_FIELD(USB_EP_command, epid, i);
    epDescr->sub = 0;
    epDescr->next = virt_to_phys(&TxBulkEPList[i + 1]);

    /* Initiate two EPs, disabled and with the eol flag set. No need for any
       preserved epid. */
    
    /* The first one has the intr flag set so we get an interrupt when the DMA
       channel is about to become disabled. */
    CHECK_ALIGN(&TxBulkDummyEPList[i][0]);
    TxBulkDummyEPList[i][0].hw_len = 0;
    TxBulkDummyEPList[i][0].command = (IO_FIELD(USB_EP_command, epid, DUMMY_EPID) |
				       IO_STATE(USB_EP_command, eol, yes) |
				       IO_STATE(USB_EP_command, intr, yes));
    TxBulkDummyEPList[i][0].sub = 0;
    TxBulkDummyEPList[i][0].next = virt_to_phys(&TxBulkDummyEPList[i][1]);
    
    /* The second one. */
    CHECK_ALIGN(&TxBulkDummyEPList[i][1]);
    TxBulkDummyEPList[i][1].hw_len = 0;
    TxBulkDummyEPList[i][1].command = (IO_FIELD(USB_EP_command, epid, DUMMY_EPID) |
				       IO_STATE(USB_EP_command, eol, yes));
    TxBulkDummyEPList[i][1].sub = 0;
    /* The last dummy's next pointer is the same as the current EP's next pointer. */
    TxBulkDummyEPList[i][1].next = virt_to_phys(&TxBulkEPList[i + 1]);
  }

  /* Special handling of last descr in list, make list circular */
  epDescr = &TxBulkEPList[i];
  CHECK_ALIGN(epDescr);
  epDescr->hw_len = 0;
  epDescr->command = IO_STATE(USB_EP_command, eol, yes) |
    IO_FIELD(USB_EP_command, epid, i);
  epDescr->sub = 0;
  epDescr->next = virt_to_phys(&TxBulkEPList[0]);
  
  /* Init DMA sub-channel pointers to last item in each list */
  *R_DMA_CH8_SUB0_EP = virt_to_phys(&TxBulkEPList[i]);
  /* No point in starting the bulk channel yet.
   *R_DMA_CH8_SUB0_CMD = IO_STATE(R_DMA_CH8_SUB0_CMD, cmd, start); */
}

static void tc_dma_init_tx_ctrl_list(void) {
  int i;
  volatile struct USB_EP_Desc *epDescr;

  for (i = 0; i < (NBR_OF_EPIDS - 1); i++) {
    epDescr = &(TxCtrlEPList[i]);
    CHECK_ALIGN(epDescr);
    epDescr->hw_len = 0;
    epDescr->command = IO_FIELD(USB_EP_command, epid, i);
    epDescr->sub = 0;
    epDescr->next = virt_to_phys(&TxCtrlEPList[i + 1]);
  }
  /* Special handling of last descr in list, make list circular */
  epDescr = &TxCtrlEPList[i];
  CHECK_ALIGN(epDescr);
  epDescr->hw_len = 0;
  epDescr->command = IO_STATE(USB_EP_command, eol, yes) |
    IO_FIELD(USB_EP_command, epid, i);
  epDescr->sub = 0;
  epDescr->next = virt_to_phys(&TxCtrlEPList[0]);
  
  /* Init DMA sub-channel pointers to last item in each list */
  *R_DMA_CH8_SUB1_EP = virt_to_phys(&TxCtrlEPList[i]);
  /* No point in starting the ctrl channel yet.
   *R_DMA_CH8_SUB1_CMD = IO_STATE(R_DMA_CH8_SUB0_CMD, cmd, start); */
}


static void tc_dma_init_tx_intr_list(void) {
  int i;

  TxIntrSB_zout.sw_len = 1;
  TxIntrSB_zout.next = 0;
  TxIntrSB_zout.buf = virt_to_phys(&zout_buffer[0]);
  TxIntrSB_zout.command = (IO_FIELD(USB_SB_command, rem, 0) |
			   IO_STATE(USB_SB_command, tt, zout) |
			   IO_STATE(USB_SB_command, full, yes) |
			   IO_STATE(USB_SB_command, eot, yes) |
			   IO_STATE(USB_SB_command, eol, yes));
  
  for (i = 0; i < (MAX_INTR_INTERVAL - 1); i++) {
    CHECK_ALIGN(&TxIntrEPList[i]);
    TxIntrEPList[i].hw_len = 0;
    TxIntrEPList[i].command =
      (IO_STATE(USB_EP_command, eof, yes) |
       IO_STATE(USB_EP_command, enable, yes) |
       IO_FIELD(USB_EP_command, epid, INVALID_EPID));
    TxIntrEPList[i].sub = virt_to_phys(&TxIntrSB_zout);
    TxIntrEPList[i].next = virt_to_phys(&TxIntrEPList[i + 1]);
  }

  /* Special handling of last descr in list, make list circular */
  CHECK_ALIGN(&TxIntrEPList[i]);
  TxIntrEPList[i].hw_len = 0;
  TxIntrEPList[i].command =
    (IO_STATE(USB_EP_command, eof, yes) |
     IO_STATE(USB_EP_command, eol, yes) |
     IO_STATE(USB_EP_command, enable, yes) |
     IO_FIELD(USB_EP_command, epid, INVALID_EPID));
  TxIntrEPList[i].sub = virt_to_phys(&TxIntrSB_zout);
  TxIntrEPList[i].next = virt_to_phys(&TxIntrEPList[0]);

  intr_dbg("Initiated Intr EP descriptor list\n");


  /* Connect DMA 8 sub-channel 2 to first in list */
  *R_DMA_CH8_SUB2_EP = virt_to_phys(&TxIntrEPList[0]);
}

static void tc_dma_init_tx_isoc_list(void) {
  int i;

  DBFENTER;

  /* Read comment at zout_buffer declaration for an explanation to this. */
  TxIsocSB_zout.sw_len = 1;
  TxIsocSB_zout.next = 0;
  TxIsocSB_zout.buf = virt_to_phys(&zout_buffer[0]);
  TxIsocSB_zout.command = (IO_FIELD(USB_SB_command, rem, 0) |
			   IO_STATE(USB_SB_command, tt, zout) |
			   IO_STATE(USB_SB_command, full, yes) |
			   IO_STATE(USB_SB_command, eot, yes) |
			   IO_STATE(USB_SB_command, eol, yes));

  /* The last isochronous EP descriptor is a dummy. */
  for (i = 0; i < (NBR_OF_EPIDS - 1); i++) {
    CHECK_ALIGN(&TxIsocEPList[i]);
    TxIsocEPList[i].hw_len = 0;
    TxIsocEPList[i].command = IO_FIELD(USB_EP_command, epid, i);
    TxIsocEPList[i].sub = 0;
    TxIsocEPList[i].next = virt_to_phys(&TxIsocEPList[i + 1]);
  }

  CHECK_ALIGN(&TxIsocEPList[i]);
  TxIsocEPList[i].hw_len = 0;

  /* Must enable the last EP descr to get eof interrupt. */
  TxIsocEPList[i].command = (IO_STATE(USB_EP_command, enable, yes) |
			     IO_STATE(USB_EP_command, eof, yes) |
			     IO_STATE(USB_EP_command, eol, yes) |
			     IO_FIELD(USB_EP_command, epid, INVALID_EPID));
  TxIsocEPList[i].sub = virt_to_phys(&TxIsocSB_zout);
  TxIsocEPList[i].next = virt_to_phys(&TxIsocEPList[0]);

  *R_DMA_CH8_SUB3_EP = virt_to_phys(&TxIsocEPList[0]);
  *R_DMA_CH8_SUB3_CMD = IO_STATE(R_DMA_CH8_SUB3_CMD, cmd, start);
}

static int tc_dma_init(struct usb_hcd *hcd) {
  tc_dma_init_rx_list();
  tc_dma_init_tx_bulk_list();
  tc_dma_init_tx_ctrl_list();
  tc_dma_init_tx_intr_list();
  tc_dma_init_tx_isoc_list();

  if (cris_request_dma(USB_TX_DMA_NBR,
		       "ETRAX 100LX built-in USB (Tx)",
		       DMA_VERBOSE_ON_ERROR,
		       dma_usb)) {
    err("Could not allocate DMA ch 8 for USB");
    return -EBUSY;
  }
 	
  if (cris_request_dma(USB_RX_DMA_NBR,
		       "ETRAX 100LX built-in USB (Rx)",
		       DMA_VERBOSE_ON_ERROR,
		       dma_usb)) {
    err("Could not allocate DMA ch 9 for USB");
    return -EBUSY;
  }

  *R_IRQ_MASK2_SET =
    /* Note that these interrupts are not used. */
    IO_STATE(R_IRQ_MASK2_SET, dma8_sub0_descr, set) |
    /* Sub channel 1 (ctrl) descr. interrupts are used. */
    IO_STATE(R_IRQ_MASK2_SET, dma8_sub1_descr, set) |
    IO_STATE(R_IRQ_MASK2_SET, dma8_sub2_descr, set) |
    /* Sub channel 3 (isoc) descr. interrupts are used. */
    IO_STATE(R_IRQ_MASK2_SET, dma8_sub3_descr, set);
  
  /* Note that the dma9_descr interrupt is not used. */
  *R_IRQ_MASK2_SET =
    IO_STATE(R_IRQ_MASK2_SET, dma9_eop, set) |
    IO_STATE(R_IRQ_MASK2_SET, dma9_descr, set);

  if (request_irq(ETRAX_USB_RX_IRQ, tc_dma_rx_interrupt, 0,
		  "ETRAX 100LX built-in USB (Rx)", hcd)) {
    err("Could not allocate IRQ %d for USB", ETRAX_USB_RX_IRQ);
    return -EBUSY;
  }
  
  if (request_irq(ETRAX_USB_TX_IRQ, tc_dma_tx_interrupt, 0,
		  "ETRAX 100LX built-in USB (Tx)", hcd)) {
    err("Could not allocate IRQ %d for USB", ETRAX_USB_TX_IRQ);
    return -EBUSY;
  }

  return 0;
}

static void tc_dma_destroy(void) {
  free_irq(ETRAX_USB_RX_IRQ, NULL);
  free_irq(ETRAX_USB_TX_IRQ, NULL);

  cris_free_dma(USB_TX_DMA_NBR, "ETRAX 100LX built-in USB (Tx)");
  cris_free_dma(USB_RX_DMA_NBR, "ETRAX 100LX built-in USB (Rx)");

}

static void tc_dma_link_intr_urb(struct urb *urb);

/* Handle processing of Bulk, Ctrl and Intr queues */
static void tc_dma_process_queue(int epid) {
  struct urb *urb;
  struct crisv10_urb_priv *urb_priv;
  unsigned long flags;
  char toggle;

  if(epid_state[epid].disabled) {
    /* Don't process any URBs on a disabled endpoint */
    return;
  }

  /* Do not disturb us while fiddling with EPs and epids */
  local_irq_save(flags);

  /* For bulk, Ctrl and Intr can we only have one URB active at a time for
     a specific EP. */
  if(activeUrbList[epid] != NULL) {
    /* An URB is already active on EP, skip checking queue */
    local_irq_restore(flags);
    return;
  }

  urb = urb_list_first(epid);
  if(urb == NULL) {
    /* No URB waiting in EP queue. Nothing do to */
    local_irq_restore(flags);
    return;
  }

  urb_priv = urb->hcpriv;
  ASSERT(urb_priv != NULL);
  ASSERT(urb_priv->urb_state == NOT_STARTED);
  ASSERT(!usb_pipeisoc(urb->pipe));

  /* Remove this URB from the queue and move it to active */
  activeUrbList[epid] = urb;
  urb_list_del(urb, epid);

  urb_priv->urb_state = STARTED;

  /* Reset error counters (regardless of which direction this traffic is). */
  etrax_epid_clear_error(epid);

  /* Special handling of Intr EP lists */
  if(usb_pipeint(urb->pipe)) {
    tc_dma_link_intr_urb(urb);
    local_irq_restore(flags);
    return;
  }

  /* Software must preset the toggle bits for Bulk and Ctrl */
  if(usb_pipecontrol(urb->pipe)) {
    /* Toggle bits are initialized only during setup transaction in a
       CTRL transfer */
    etrax_epid_set_toggle(epid, 0, 0);
    etrax_epid_set_toggle(epid, 1, 0);
  } else {
    toggle = usb_gettoggle(urb->dev, usb_pipeendpoint(urb->pipe),
			   usb_pipeout(urb->pipe));
    etrax_epid_set_toggle(epid, usb_pipeout(urb->pipe), toggle);
  }

  tc_dbg("Added SBs from (URB:0x%x %s %s) to epid %d: %s\n",
	 (unsigned int)urb, str_dir(urb->pipe), str_type(urb->pipe), epid,
	 sblist_to_str(urb_priv->first_sb));

  /* We start the DMA sub channel without checking if it's running or not,
     because:
     1) If it's already running, issuing the start command is a nop.
     2) We avoid a test-and-set race condition. */
  switch(usb_pipetype(urb->pipe)) {
  case PIPE_BULK:
    /* Assert that the EP descriptor is disabled. */
    ASSERT(!(TxBulkEPList[epid].command & IO_MASK(USB_EP_command, enable)));

    /* Set up and enable the EP descriptor. */
    TxBulkEPList[epid].sub = virt_to_phys(urb_priv->first_sb);
    TxBulkEPList[epid].hw_len = 0;
    TxBulkEPList[epid].command |= IO_STATE(USB_EP_command, enable, yes);

    /* Check if the dummy list is already with us (if several urbs were queued). */
    if (usb_pipein(urb->pipe) && (TxBulkEPList[epid].next != virt_to_phys(&TxBulkDummyEPList[epid][0]))) {
      tc_dbg("Inviting dummy list to the party for urb 0x%lx, epid %d", 
	     (unsigned long)urb, epid);
      
      /* We don't need to check if the DMA is at this EP or not before changing the
	 next pointer, since we will do it in one 32-bit write (EP descriptors are
	 32-bit aligned). */
      TxBulkEPList[epid].next = virt_to_phys(&TxBulkDummyEPList[epid][0]);
    }

    restart_dma8_sub0();

    /* Update/restart the bulk start timer since we just started the channel.*/
    mod_timer(&bulk_start_timer, jiffies + BULK_START_TIMER_INTERVAL);
    /* Update/restart the bulk eot timer since we just inserted traffic. */
    mod_timer(&bulk_eot_timer, jiffies + BULK_EOT_TIMER_INTERVAL);
    break;
  case PIPE_CONTROL:
    /* Assert that the EP descriptor is disabled. */
    ASSERT(!(TxCtrlEPList[epid].command & IO_MASK(USB_EP_command, enable)));

    /* Set up and enable the EP descriptor. */
    TxCtrlEPList[epid].sub = virt_to_phys(urb_priv->first_sb);
    TxCtrlEPList[epid].hw_len = 0;
    TxCtrlEPList[epid].command |= IO_STATE(USB_EP_command, enable, yes);

    *R_DMA_CH8_SUB1_CMD = IO_STATE(R_DMA_CH8_SUB1_CMD, cmd, start);
    break;
  }
  local_irq_restore(flags);
}

static void tc_dma_link_intr_urb(struct urb *urb) {
  struct crisv10_urb_priv *urb_priv = urb->hcpriv;
  volatile struct USB_EP_Desc *tmp_ep;
  struct USB_EP_Desc *ep_desc;
  int i = 0, epid;
  int pool_idx = 0;

  ASSERT(urb_priv != NULL);
  epid = urb_priv->epid;
  ASSERT(urb_priv->interval > 0);
  ASSERT(urb_priv->intr_ep_pool_length > 0);

  tmp_ep = &TxIntrEPList[0];

  /* Only insert one EP descriptor in list for Out Intr URBs.
     We can only handle Out Intr with interval of 128ms because
     it's not possible to insert several Out Intr EPs because they
     are not consumed by the DMA. */
  if(usb_pipeout(urb->pipe)) {
    ep_desc = urb_priv->intr_ep_pool[0];
    ASSERT(ep_desc);
    ep_desc->next = tmp_ep->next;
    tmp_ep->next = virt_to_phys(ep_desc);
    i++;
  } else {
    /* Loop through Intr EP descriptor list and insert EP for URB at
       specified interval */
    do {
      /* Each EP descriptor with eof flag sat signals a new frame */
      if (tmp_ep->command & IO_MASK(USB_EP_command, eof)) {
	/* Insert a EP from URBs EP pool at correct interval */
	if ((i % urb_priv->interval) == 0) {
	  ep_desc = urb_priv->intr_ep_pool[pool_idx];
	  ASSERT(ep_desc);
	  ep_desc->next = tmp_ep->next;
	  tmp_ep->next = virt_to_phys(ep_desc);
	  pool_idx++;
	  ASSERT(pool_idx <= urb_priv->intr_ep_pool_length);
	}
	i++;
      }
      tmp_ep = (struct USB_EP_Desc *)phys_to_virt(tmp_ep->next);
    } while(tmp_ep != &TxIntrEPList[0]);
  }

  intr_dbg("Added SBs to intr epid %d: %s interval:%d (%d EP)\n", epid,
	   sblist_to_str(urb_priv->first_sb), urb_priv->interval, pool_idx);

  /* We start the DMA sub channel without checking if it's running or not,
     because:
     1) If it's already running, issuing the start command is a nop.
     2) We avoid a test-and-set race condition. */
  *R_DMA_CH8_SUB2_CMD = IO_STATE(R_DMA_CH8_SUB2_CMD, cmd, start);
}

static void tc_dma_process_isoc_urb(struct urb *urb) {
  unsigned long flags;
  struct crisv10_urb_priv *urb_priv = urb->hcpriv;
  int epid;

  /* Do not disturb us while fiddling with EPs and epids */
  local_irq_save(flags);

  ASSERT(urb_priv);
  ASSERT(urb_priv->first_sb);
  epid = urb_priv->epid;

  if(activeUrbList[epid] == NULL) {
    /* EP is idle, so make this URB active */
    activeUrbList[epid] = urb;
    urb_list_del(urb, epid);
    ASSERT(TxIsocEPList[epid].sub == 0);
    ASSERT(!(TxIsocEPList[epid].command &
	     IO_STATE(USB_EP_command, enable, yes)));

    /* Differentiate between In and Out Isoc. Because In SBs are not consumed*/
    if(usb_pipein(urb->pipe)) {
    /* Each EP for In Isoc will have only one SB descriptor, setup when
       submitting the first active urb. We do it here by copying from URBs
       pre-allocated SB. */
      memcpy((void *)&(TxIsocSBList[epid]), urb_priv->first_sb,
	     sizeof(TxIsocSBList[epid]));
      TxIsocEPList[epid].hw_len = 0;
      TxIsocEPList[epid].sub = virt_to_phys(&(TxIsocSBList[epid]));
    } else {
      /* For Out Isoc we attach the pre-allocated list of SBs for the URB */
      TxIsocEPList[epid].hw_len = 0;
      TxIsocEPList[epid].sub = virt_to_phys(urb_priv->first_sb);

      isoc_dbg("Attached first URB:0x%x[%d] to epid:%d first_sb:0x%x"
	       " last_sb::0x%x\n",
	       (unsigned int)urb, urb_priv->urb_num, epid,
	       (unsigned int)(urb_priv->first_sb),
	       (unsigned int)(urb_priv->last_sb));
    }

    if (urb->transfer_flags & URB_ISO_ASAP) {
      /* The isoc transfer should be started as soon as possible. The
	 start_frame field is a return value if URB_ISO_ASAP was set. Comparing
	 R_USB_FM_NUMBER with a USB Chief trace shows that the first isoc IN
	 token is sent 2 frames later. I'm not sure how this affects usage of
	 the start_frame field by the device driver, or how it affects things
	 when USB_ISO_ASAP is not set, so therefore there's no compensation for
	 the 2 frame "lag" here. */
      urb->start_frame = (*R_USB_FM_NUMBER & 0x7ff);
      TxIsocEPList[epid].command |= IO_STATE(USB_EP_command, enable, yes);
      urb_priv->urb_state = STARTED;
      isoc_dbg("URB_ISO_ASAP set, urb->start_frame set to %d\n",
	       urb->start_frame);
    } else {
      /* Not started yet. */
      urb_priv->urb_state = NOT_STARTED;
      isoc_warn("urb_priv->urb_state set to NOT_STARTED for URB:0x%x\n",
		(unsigned int)urb);
    }

  } else {
    /* An URB is already active on the EP. Leave URB in queue and let
       finish_isoc_urb process it after current active URB */
    ASSERT(TxIsocEPList[epid].sub != 0);

    if(usb_pipein(urb->pipe)) {
      /* Because there already is a active In URB on this epid we do nothing
         and the finish_isoc_urb() function will handle switching to next URB*/

    } else { /* For Out Isoc, insert new URBs traffic last in SB-list. */
      struct USB_SB_Desc *temp_sb_desc;

      /* Set state STARTED to all Out Isoc URBs added to SB list because we
         don't know how many of them that are finished before descr interrupt*/
      urb_priv->urb_state = STARTED;

      /* Find end of current SB list by looking for SB with eol flag sat */
      temp_sb_desc = phys_to_virt(TxIsocEPList[epid].sub);
      while ((temp_sb_desc->command & IO_MASK(USB_SB_command, eol)) !=
	     IO_STATE(USB_SB_command, eol, yes)) {
	ASSERT(temp_sb_desc->next);
	temp_sb_desc = phys_to_virt(temp_sb_desc->next);
      }

      isoc_dbg("Appended URB:0x%x[%d] (first:0x%x last:0x%x) to epid:%d"
	       " sub:0x%x eol:0x%x\n",
	       (unsigned int)urb, urb_priv->urb_num,
	       (unsigned int)(urb_priv->first_sb),
	       (unsigned int)(urb_priv->last_sb), epid,
	       (unsigned int)phys_to_virt(TxIsocEPList[epid].sub),
	       (unsigned int)temp_sb_desc);

      /* Next pointer must be set before eol is removed. */
      temp_sb_desc->next = virt_to_phys(urb_priv->first_sb);
      /* Clear the previous end of list flag since there is a new in the
	 added SB descriptor list. */
      temp_sb_desc->command &= ~IO_MASK(USB_SB_command, eol);

      if (!(TxIsocEPList[epid].command & IO_MASK(USB_EP_command, enable))) {
	__u32 epid_data;
	/* 8.8.5 in Designer's Reference says we should check for and correct
	   any errors in the EP here.  That should not be necessary if
	   epid_attn is handled correctly, so we assume all is ok. */
	epid_data = etrax_epid_iso_get(epid);
	if (IO_EXTRACT(R_USB_EPT_DATA, error_code, epid_data) !=
	    IO_STATE_VALUE(R_USB_EPT_DATA, error_code, no_error)) {
	  isoc_err("Disabled Isoc EP with error:%d on epid:%d when appending"
		   " URB:0x%x[%d]\n",
		   IO_EXTRACT(R_USB_EPT_DATA, error_code, epid_data), epid,
		   (unsigned int)urb, urb_priv->urb_num);
	}

	/* The SB list was exhausted. */
	if (virt_to_phys(urb_priv->last_sb) != TxIsocEPList[epid].sub) {
	  /* The new sublist did not get processed before the EP was
	     disabled.  Setup the EP again. */

	  if(virt_to_phys(temp_sb_desc) == TxIsocEPList[epid].sub) {
	    isoc_dbg("EP for epid:%d stoped at SB:0x%x before newly inserted"
		     ", restarting from this URBs SB:0x%x\n",
		     epid, (unsigned int)temp_sb_desc,
		     (unsigned int)(urb_priv->first_sb));
	    TxIsocEPList[epid].hw_len = 0;
	    TxIsocEPList[epid].sub = virt_to_phys(urb_priv->first_sb);
	    urb->start_frame = (*R_USB_FM_NUMBER & 0x7ff);
	    /* Enable the EP again so data gets processed this time */
	    TxIsocEPList[epid].command |=
	      IO_STATE(USB_EP_command, enable, yes);

	  } else {
	    /* The EP has been disabled but not at end this URB (god knows
	       where). This should generate an epid_attn so we should not be
	       here */
	    isoc_warn("EP was disabled on sb:0x%x before SB list for"
		     " URB:0x%x[%d] got processed\n",
		     (unsigned int)phys_to_virt(TxIsocEPList[epid].sub),
		     (unsigned int)urb, urb_priv->urb_num);
	  }
	} else {
	  /* This might happend if we are slow on this function and isn't
	     an error. */
	  isoc_dbg("EP was disabled and finished with SBs from appended"
		   " URB:0x%x[%d]\n", (unsigned int)urb, urb_priv->urb_num);
	}
      }
    }
  }
  
  /* Start the DMA sub channel */
  *R_DMA_CH8_SUB3_CMD = IO_STATE(R_DMA_CH8_SUB3_CMD, cmd, start);

  local_irq_restore(flags);
}

static void tc_dma_unlink_intr_urb(struct urb *urb) {
  struct crisv10_urb_priv *urb_priv = urb->hcpriv;
  volatile struct USB_EP_Desc *first_ep;  /* First EP in the list. */
  volatile struct USB_EP_Desc *curr_ep;   /* Current EP, the iterator. */
  volatile struct USB_EP_Desc *next_ep;   /* The EP after current. */
  volatile struct USB_EP_Desc *unlink_ep; /* The one we should remove from
					     the list. */
  int count = 0;
  volatile int timeout = 10000;
  int epid;

  /* Read 8.8.4 in Designer's Reference, "Removing an EP Descriptor from the
     List". */
  ASSERT(urb_priv);
  ASSERT(urb_priv->intr_ep_pool_length > 0);
  epid = urb_priv->epid;

  /* First disable all Intr EPs belonging to epid for this URB */
  first_ep = &TxIntrEPList[0];
  curr_ep = first_ep;
  do {
    next_ep = (struct USB_EP_Desc *)phys_to_virt(curr_ep->next);
    if (IO_EXTRACT(USB_EP_command, epid, next_ep->command) == epid) {
      /* Disable EP */
      next_ep->command &= ~IO_MASK(USB_EP_command, enable);
    }
    curr_ep = phys_to_virt(curr_ep->next);
  } while (curr_ep != first_ep);


  /* Now unlink all EPs belonging to this epid from Descr list */
  first_ep = &TxIntrEPList[0];
  curr_ep = first_ep;
  do {
    next_ep = (struct USB_EP_Desc *)phys_to_virt(curr_ep->next);
    if (IO_EXTRACT(USB_EP_command, epid, next_ep->command) == epid) {
      /* This is the one we should unlink. */
      unlink_ep = next_ep;

      /* Actually unlink the EP from the DMA list. */
      curr_ep->next = unlink_ep->next;

      /* Wait until the DMA is no longer at this descriptor. */
      while((*R_DMA_CH8_SUB2_EP == virt_to_phys(unlink_ep)) &&
	    (timeout-- > 0));

      count++;
    }
    curr_ep = phys_to_virt(curr_ep->next);
  } while (curr_ep != first_ep);

  if(count != urb_priv->intr_ep_pool_length) {
    intr_warn("Unlinked %d of %d Intr EPs for URB:0x%x[%d]\n", count,
	      urb_priv->intr_ep_pool_length, (unsigned int)urb,
	      urb_priv->urb_num);
  } else {
    intr_dbg("Unlinked %d of %d interrupt EPs for URB:0x%x\n", count,
	     urb_priv->intr_ep_pool_length, (unsigned int)urb);
  }
}

static void check_finished_bulk_tx_epids(struct usb_hcd *hcd,
						    int timer) {
  unsigned long flags;
  int epid;
  struct urb *urb;
  struct crisv10_urb_priv * urb_priv;
  __u32 epid_data;

  /* Protect TxEPList */
  local_irq_save(flags);

  for (epid = 0; epid < NBR_OF_EPIDS; epid++) {
    /* A finished EP descriptor is disabled and has a valid sub pointer */
    if (!(TxBulkEPList[epid].command & IO_MASK(USB_EP_command, enable)) &&
	(TxBulkEPList[epid].sub != 0)) {

      /* Get the active URB for this epid */
      urb = activeUrbList[epid];
      /* Sanity checks */
      ASSERT(urb);
      urb_priv = (struct crisv10_urb_priv *)urb->hcpriv;
      ASSERT(urb_priv);
      
      /* Only handle finished out Bulk EPs here,
	 and let RX interrupt take care of the rest */
      if(!epid_out_traffic(epid)) {
	continue;
      }

      if(timer) {
	tc_warn("Found finished %s Bulk epid:%d URB:0x%x[%d] from timeout\n",
		epid_out_traffic(epid) ? "Out" : "In", epid, (unsigned int)urb,
		urb_priv->urb_num);
      } else {
	tc_dbg("Found finished %s Bulk epid:%d URB:0x%x[%d] from interrupt\n",
	       epid_out_traffic(epid) ? "Out" : "In", epid, (unsigned int)urb,
	       urb_priv->urb_num);
      }

      if(urb_priv->urb_state == UNLINK) {
	/* This Bulk URB is requested to be unlinked, that means that the EP
	   has been disabled and we might not have sent all data */
	tc_finish_urb(hcd, urb, urb->status);
	continue;
      }

      ASSERT(urb_priv->urb_state == STARTED);
      if (phys_to_virt(TxBulkEPList[epid].sub) != urb_priv->last_sb) {
	tc_err("Endpoint got disabled before reaching last sb\n");
      }
	
      epid_data = etrax_epid_get(epid);
      if (IO_EXTRACT(R_USB_EPT_DATA, error_code, epid_data) ==
	  IO_STATE_VALUE(R_USB_EPT_DATA, error_code, no_error)) {
	/* This means that the endpoint has no error, is disabled
	   and had inserted traffic, i.e. transfer successfully completed. */
	tc_finish_urb(hcd, urb, 0);
      } else {
	/* Shouldn't happen. We expect errors to be caught by epid
	   attention. */
	tc_err("Found disabled bulk EP desc (epid:%d error:%d)\n",
	       epid, IO_EXTRACT(R_USB_EPT_DATA, error_code, epid_data));
      }
    } else {
      tc_dbg("Ignoring In Bulk epid:%d, let RX interrupt handle it\n", epid);
    }
  }

  local_irq_restore(flags);
}

static void check_finished_ctrl_tx_epids(struct usb_hcd *hcd) {
  unsigned long flags;
  int epid;
  struct urb *urb;
  struct crisv10_urb_priv * urb_priv;
  __u32 epid_data;

  /* Protect TxEPList */
  local_irq_save(flags);

  for (epid = 0; epid < NBR_OF_EPIDS; epid++) {
    if(epid == DUMMY_EPID)
      continue;

    /* A finished EP descriptor is disabled and has a valid sub pointer */
    if (!(TxCtrlEPList[epid].command & IO_MASK(USB_EP_command, enable)) &&
	(TxCtrlEPList[epid].sub != 0)) {
      
      /* Get the active URB for this epid */
      urb = activeUrbList[epid];

      if(urb == NULL) {
	tc_warn("Found finished Ctrl epid:%d with no active URB\n", epid);
	continue;
      }
      
      /* Sanity checks */
      ASSERT(usb_pipein(urb->pipe));
      urb_priv = (struct crisv10_urb_priv *)urb->hcpriv;
      ASSERT(urb_priv);
      if (phys_to_virt(TxCtrlEPList[epid].sub) != urb_priv->last_sb) {
	tc_err("Endpoint got disabled before reaching last sb\n");
      }

      epid_data = etrax_epid_get(epid);
      if (IO_EXTRACT(R_USB_EPT_DATA, error_code, epid_data) ==
	  IO_STATE_VALUE(R_USB_EPT_DATA, error_code, no_error)) {
	/* This means that the endpoint has no error, is disabled
	   and had inserted traffic, i.e. transfer successfully completed. */

	/* Check if RX-interrupt for In Ctrl has been processed before
	   finishing the URB */
	if(urb_priv->ctrl_rx_done) {
	  tc_dbg("Finishing In Ctrl URB:0x%x[%d] in tx_interrupt\n",
		 (unsigned int)urb, urb_priv->urb_num);
	  tc_finish_urb(hcd, urb, 0);
	} else {
	  /* If we get zout descriptor interrupt before RX was done for a
	     In Ctrl transfer, then we flag that and it will be finished
	     in the RX-Interrupt */
	  urb_priv->ctrl_zout_done = 1;
	  tc_dbg("Got zout descr interrupt before RX interrupt\n");
	}
      } else {
	/* Shouldn't happen. We expect errors to be caught by epid
	   attention. */
	tc_err("Found disabled Ctrl EP desc (epid:%d URB:0x%x[%d]) error_code:%d\n", epid, (unsigned int)urb, urb_priv->urb_num, IO_EXTRACT(R_USB_EPT_DATA, error_code, epid_data));
	__dump_ep_desc(&(TxCtrlEPList[epid]));
	__dump_ept_data(epid);
      }      
    }
  }
  local_irq_restore(flags);
}

/* This function goes through all epids that are setup for Out Isoc transfers
   and marks (isoc_out_done) all queued URBs that the DMA has finished
   transfer for.
   No URB completetion is done here to make interrupt routine return quickly.
   URBs are completed later with help of complete_isoc_bottom_half() that
   becomes schedules when this functions is finished. */
static void check_finished_isoc_tx_epids(void) {
  unsigned long flags;
  int epid;
  struct urb *urb;
  struct crisv10_urb_priv * urb_priv;
  struct USB_SB_Desc* sb_desc;
  int epid_done;

  /* Protect TxIsocEPList */
  local_irq_save(flags);

  for (epid = 0; epid < NBR_OF_EPIDS; epid++) {
    if (TxIsocEPList[epid].sub == 0 || epid == INVALID_EPID ||
	!epid_out_traffic(epid)) {
      /* Nothing here to see. */
      continue;
    }
    ASSERT(epid_inuse(epid));
    ASSERT(epid_isoc(epid));

    sb_desc = phys_to_virt(TxIsocEPList[epid].sub);
    /* Find the last descriptor of the currently active URB for this ep.
       This is the first descriptor in the sub list marked for a descriptor
       interrupt. */
    while (sb_desc && !IO_EXTRACT(USB_SB_command, intr, sb_desc->command)) {
      sb_desc = sb_desc->next ? phys_to_virt(sb_desc->next) : 0;
    }
    ASSERT(sb_desc);

    isoc_dbg("Descr IRQ checking epid:%d sub:0x%x intr:0x%x\n",
	     epid, (unsigned int)phys_to_virt(TxIsocEPList[epid].sub),
	     (unsigned int)sb_desc);

    urb = activeUrbList[epid];
    if(urb == NULL) {
      isoc_err("Isoc Descr irq on epid:%d with no active URB\n", epid);
      continue;
    }

    epid_done = 0;
    while(urb && !epid_done) {
      /* Sanity check. */
      ASSERT(usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS);
      ASSERT(usb_pipeout(urb->pipe));
      
      urb_priv = (struct crisv10_urb_priv *)urb->hcpriv;
      ASSERT(urb_priv);
      ASSERT(urb_priv->urb_state == STARTED ||
	     urb_priv->urb_state == UNLINK);
      
      if (sb_desc != urb_priv->last_sb) {
	/* This urb has been sent. */
	urb_priv->isoc_out_done = 1;

      } else { /* Found URB that has last_sb as the interrupt reason */

	/* Check if EP has been disabled, meaning that all transfers are done*/
	if(!(TxIsocEPList[epid].command & IO_MASK(USB_EP_command, enable))) {
	  ASSERT((sb_desc->command & IO_MASK(USB_SB_command, eol)) ==
		 IO_STATE(USB_SB_command, eol, yes));
	  ASSERT(sb_desc->next == 0);
	  urb_priv->isoc_out_done = 1;
	} else {
	  isoc_dbg("Skipping URB:0x%x[%d] because EP not disabled yet\n",
		   (unsigned int)urb, urb_priv->urb_num);
	}
	/* Stop looking any further in queue */
	epid_done = 1;	
      }

      if (!epid_done) {
	if(urb == activeUrbList[epid]) {
	  urb = urb_list_first(epid);
	} else {
	  urb = urb_list_next(urb, epid);
	}
      }
    } /* END: while(urb && !epid_done) */
  }

  local_irq_restore(flags);
}


/* This is where the Out Isoc URBs are realy completed. This function is
   scheduled from tc_dma_tx_interrupt() when one or more Out Isoc transfers
   are done. This functions completes all URBs earlier marked with
   isoc_out_done by fast interrupt routine check_finished_isoc_tx_epids() */

static void complete_isoc_bottom_half(struct work_struct* work) {
  struct crisv10_isoc_complete_data *comp_data;
  struct usb_iso_packet_descriptor *packet;
  struct crisv10_urb_priv * urb_priv;
  unsigned long flags;
  struct urb* urb;
  int epid_done;
  int epid;
  int i;

  comp_data = container_of(work, struct crisv10_isoc_complete_data, usb_bh);

  local_irq_save(flags);

  for (epid = 0; epid < NBR_OF_EPIDS - 1; epid++) {
    if(!epid_inuse(epid) || !epid_isoc(epid) || !epid_out_traffic(epid) || epid == DUMMY_EPID) {
      /* Only check valid Out Isoc epids */
      continue;
    }

    isoc_dbg("Isoc bottom-half checking epid:%d, sub:0x%x\n", epid,
	     (unsigned int)phys_to_virt(TxIsocEPList[epid].sub));

    /* The descriptor interrupt handler has marked all transmitted Out Isoc
       URBs with isoc_out_done.  Now we traverse all epids and for all that
       have out Isoc traffic we traverse its URB list and complete the
       transmitted URBs. */
    epid_done = 0;
    while (!epid_done) {

      /* Get the active urb (if any) */
      urb = activeUrbList[epid];
      if (urb == 0) {
	isoc_dbg("No active URB on epid:%d anymore\n", epid);
	epid_done = 1;
	continue;
      }

      /* Sanity check. */
      ASSERT(usb_pipetype(urb->pipe) == PIPE_ISOCHRONOUS);
      ASSERT(usb_pipeout(urb->pipe));

      urb_priv = (struct crisv10_urb_priv *)urb->hcpriv;
      ASSERT(urb_priv);

      if (!(urb_priv->isoc_out_done)) {
	/* We have reached URB that isn't flaged done yet, stop traversing. */
	isoc_dbg("Stoped traversing Out Isoc URBs on epid:%d"
		 " before not yet flaged URB:0x%x[%d]\n",
		 epid, (unsigned int)urb, urb_priv->urb_num);
	epid_done = 1;
	continue;
      }

      /* This urb has been sent. */
      isoc_dbg("Found URB:0x%x[%d] that is flaged isoc_out_done\n",
	       (unsigned int)urb, urb_priv->urb_num);

      /* Set ok on transfered packets for this URB and finish it */
      for (i = 0; i < urb->number_of_packets; i++) {
	packet = &urb->iso_frame_desc[i];
	packet->status = 0;
	packet->actual_length = packet->length;
      }
      urb_priv->isoc_packet_counter = urb->number_of_packets;
      tc_finish_urb(comp_data->hcd, urb, 0);

    } /* END: while(!epid_done) */
  } /* END: for(epid...) */

  local_irq_restore(flags);
  kmem_cache_free(isoc_compl_cache, comp_data);
}


static void check_finished_intr_tx_epids(struct usb_hcd *hcd) {
  unsigned long flags;
  int epid;
  struct urb *urb;
  struct crisv10_urb_priv * urb_priv;
  volatile struct USB_EP_Desc *curr_ep;   /* Current EP, the iterator. */
  volatile struct USB_EP_Desc *next_ep;   /* The EP after current. */

  /* Protect TxintrEPList */
  local_irq_save(flags);

  for (epid = 0; epid < NBR_OF_EPIDS; epid++) {
    if(!epid_inuse(epid) || !epid_intr(epid) || !epid_out_traffic(epid)) {
      /* Nothing to see on this epid. Only check valid Out Intr epids */
      continue;
    }

    urb = activeUrbList[epid];
    if(urb == 0) {
      intr_warn("Found Out Intr epid:%d with no active URB\n", epid);
      continue;
    }

    /* Sanity check. */
    ASSERT(usb_pipetype(urb->pipe) == PIPE_INTERRUPT);
    ASSERT(usb_pipeout(urb->pipe));
    
    urb_priv = (struct crisv10_urb_priv *)urb->hcpriv;
    ASSERT(urb_priv);

    /* Go through EPs between first and second sof-EP. It's here Out Intr EPs
       are inserted.*/
    curr_ep = &TxIntrEPList[0];
    do {
      next_ep = (struct USB_EP_Desc *)phys_to_virt(curr_ep->next);
      if(next_ep == urb_priv->intr_ep_pool[0]) {
	/* We found the Out Intr EP for this epid */
	
	/* Disable it so it doesn't get processed again */
	next_ep->command &= ~IO_MASK(USB_EP_command, enable);

	/* Finish the active Out Intr URB with status OK */
	tc_finish_urb(hcd, urb, 0);
      }
      curr_ep = phys_to_virt(curr_ep->next);
    } while (curr_ep != &TxIntrEPList[1]);

  }
  local_irq_restore(flags);
}

/* Interrupt handler for DMA8/IRQ24 with subchannels (called from hardware intr) */
static irqreturn_t tc_dma_tx_interrupt(int irq, void *vhc) {
  struct usb_hcd *hcd = (struct usb_hcd*)vhc;
  ASSERT(hcd);

  if (*R_IRQ_READ2 & IO_MASK(R_IRQ_READ2, dma8_sub0_descr)) {
    /* Clear this interrupt */
    *R_DMA_CH8_SUB0_CLR_INTR = IO_STATE(R_DMA_CH8_SUB0_CLR_INTR, clr_descr, do);
    restart_dma8_sub0();
  }

  if (*R_IRQ_READ2 & IO_MASK(R_IRQ_READ2, dma8_sub1_descr)) {
    /* Clear this interrupt */
    *R_DMA_CH8_SUB1_CLR_INTR = IO_STATE(R_DMA_CH8_SUB1_CLR_INTR, clr_descr, do);
    check_finished_ctrl_tx_epids(hcd);
  }

  if (*R_IRQ_READ2 & IO_MASK(R_IRQ_READ2, dma8_sub2_descr)) {
    /* Clear this interrupt */
    *R_DMA_CH8_SUB2_CLR_INTR = IO_STATE(R_DMA_CH8_SUB2_CLR_INTR, clr_descr, do);
    check_finished_intr_tx_epids(hcd);
  }

  if (*R_IRQ_READ2 & IO_MASK(R_IRQ_READ2, dma8_sub3_descr)) {
    struct crisv10_isoc_complete_data* comp_data;

    /* Flag done Out Isoc for later completion */
    check_finished_isoc_tx_epids();

    /* Clear this interrupt */
    *R_DMA_CH8_SUB3_CLR_INTR = IO_STATE(R_DMA_CH8_SUB3_CLR_INTR, clr_descr, do);
    /* Schedule bottom half of Out Isoc completion function. This function
       finishes the URBs marked with isoc_out_done */
    comp_data = (struct crisv10_isoc_complete_data*)
      kmem_cache_alloc(isoc_compl_cache, GFP_ATOMIC);
    ASSERT(comp_data != NULL);
    comp_data ->hcd = hcd;

    INIT_WORK(&comp_data->usb_bh, complete_isoc_bottom_half);
    schedule_work(&comp_data->usb_bh);
  }

  return IRQ_HANDLED;
}

/* Interrupt handler for DMA9/IRQ25 (called from hardware intr) */
static irqreturn_t tc_dma_rx_interrupt(int irq, void *vhc) {
  unsigned long flags;
  struct urb *urb;
  struct usb_hcd *hcd = (struct usb_hcd*)vhc;
  struct crisv10_urb_priv *urb_priv;
  int epid = 0;
  int real_error;

  ASSERT(hcd);

  /* Clear this interrupt. */
  *R_DMA_CH9_CLR_INTR = IO_STATE(R_DMA_CH9_CLR_INTR, clr_eop, do);

  /* Custom clear interrupt for this interrupt */
  /* The reason we cli here is that we call the driver's callback functions. */
  local_irq_save(flags);

  /* Note that this while loop assumes that all packets span only
     one rx descriptor. */
  while(myNextRxDesc->status & IO_MASK(USB_IN_status, eop)) {
    epid = IO_EXTRACT(USB_IN_status, epid, myNextRxDesc->status);
    /* Get the active URB for this epid */
    urb = activeUrbList[epid];

    ASSERT(epid_inuse(epid));
    if (!urb) {
      dma_err("No urb for epid %d in rx interrupt\n", epid);
      goto skip_out;
    }

    /* Check if any errors on epid */
    real_error = 0;
    if (myNextRxDesc->status & IO_MASK(USB_IN_status, error)) {
      __u32 r_usb_ept_data;

      if (usb_pipeisoc(urb->pipe)) {
	r_usb_ept_data = etrax_epid_iso_get(epid);
	if((r_usb_ept_data & IO_MASK(R_USB_EPT_DATA_ISO, valid)) &&
	   (IO_EXTRACT(R_USB_EPT_DATA_ISO, error_code, r_usb_ept_data) == 0) &&
	   (myNextRxDesc->status & IO_MASK(USB_IN_status, nodata))) {
	  /* Not an error, just a failure to receive an expected iso
	     in packet in this frame.  This is not documented
	     in the designers reference. Continue processing.
	  */
	} else real_error = 1;
      } else real_error = 1;
    }

    if(real_error) {
      dma_err("Error in RX descr on epid:%d for URB 0x%x",
	      epid, (unsigned int)urb);
      dump_ept_data(epid);
      dump_in_desc(myNextRxDesc);
      goto skip_out;
    }

    urb_priv = (struct crisv10_urb_priv *)urb->hcpriv;
    ASSERT(urb_priv);
    ASSERT(urb_priv->urb_state == STARTED ||
	   urb_priv->urb_state == UNLINK);

    if ((usb_pipetype(urb->pipe) == PIPE_BULK) ||
	(usb_pipetype(urb->pipe) == PIPE_CONTROL) ||
	(usb_pipetype(urb->pipe) == PIPE_INTERRUPT)) {

      /* We get nodata for empty data transactions, and the rx descriptor's
	 hw_len field is not valid in that case. No data to copy in other
	 words. */
      if (myNextRxDesc->status & IO_MASK(USB_IN_status, nodata)) {
	/* No data to copy */
      } else {
	/*
	dma_dbg("Processing RX for URB:0x%x epid:%d (data:%d ofs:%d)\n",
		(unsigned int)urb, epid, myNextRxDesc->hw_len,
		urb_priv->rx_offset);
	*/
	/* Only copy data if URB isn't flaged to be unlinked*/
	if(urb_priv->urb_state != UNLINK) {
	  /* Make sure the data fits in the buffer. */
	  if(urb_priv->rx_offset + myNextRxDesc->hw_len
	     <= urb->transfer_buffer_length) {

	    /* Copy the data to URBs buffer */
	    memcpy(urb->transfer_buffer + urb_priv->rx_offset,
		   phys_to_virt(myNextRxDesc->buf), myNextRxDesc->hw_len);
	    urb_priv->rx_offset += myNextRxDesc->hw_len;
	  } else {
	    /* Signal overflow when returning URB */
	    urb->status = -EOVERFLOW;
	    tc_finish_urb_later(hcd, urb, urb->status);
	  }
	}
      }

      /* Check if it was the last packet in the transfer */
      if (myNextRxDesc->status & IO_MASK(USB_IN_status, eot)) {
	/* Special handling for In Ctrl URBs. */
	if(usb_pipecontrol(urb->pipe) && usb_pipein(urb->pipe) &&
	   !(urb_priv->ctrl_zout_done)) {
	  /* Flag that RX part of Ctrl transfer is done. Because zout descr
	     interrupt hasn't happend yet will the URB be finished in the
	     TX-Interrupt. */
	  urb_priv->ctrl_rx_done = 1;
	  tc_dbg("Not finishing In Ctrl URB:0x%x from rx_interrupt, waiting"
		 " for zout\n", (unsigned int)urb);
	} else {
	  tc_finish_urb(hcd, urb, 0);
	}
      }
    } else { /* ISOC RX */
      /*
      isoc_dbg("Processing RX for epid:%d (URB:0x%x) ISOC pipe\n",
	       epid, (unsigned int)urb);
      */

      struct usb_iso_packet_descriptor *packet;

      if (urb_priv->urb_state == UNLINK) {
	isoc_warn("Ignoring Isoc Rx data for urb being unlinked.\n");
	goto skip_out;
      } else if (urb_priv->urb_state == NOT_STARTED) {
	isoc_err("What? Got Rx data for Isoc urb that isn't started?\n");
	goto skip_out;
      }

      packet = &urb->iso_frame_desc[urb_priv->isoc_packet_counter];
      ASSERT(packet);
      packet->status = 0;

      if (myNextRxDesc->status & IO_MASK(USB_IN_status, nodata)) {
	/* We get nodata for empty data transactions, and the rx descriptor's
	   hw_len field is not valid in that case. We copy 0 bytes however to
	   stay in synch. */
	packet->actual_length = 0;
      } else {
	packet->actual_length = myNextRxDesc->hw_len;
	/* Make sure the data fits in the buffer. */
	ASSERT(packet->actual_length <= packet->length);
	memcpy(urb->transfer_buffer + packet->offset,
	       phys_to_virt(myNextRxDesc->buf), packet->actual_length);
	if(packet->actual_length > 0)
	  isoc_dbg("Copied %d bytes, packet %d for URB:0x%x[%d]\n",
		   packet->actual_length, urb_priv->isoc_packet_counter,
		   (unsigned int)urb, urb_priv->urb_num);
      }

      /* Increment the packet counter. */
      urb_priv->isoc_packet_counter++;

      /* Note that we don't care about the eot field in the rx descriptor's
	 status. It will always be set for isoc traffic. */
      if (urb->number_of_packets == urb_priv->isoc_packet_counter) {
	/* Complete the urb with status OK. */
	tc_finish_urb(hcd, urb, 0);
      }
    }

  skip_out:
    myNextRxDesc->status = 0;
    myNextRxDesc->command |= IO_MASK(USB_IN_command, eol);
    myLastRxDesc->command &= ~IO_MASK(USB_IN_command, eol);
    myLastRxDesc = myNextRxDesc;
    myNextRxDesc = phys_to_virt(myNextRxDesc->next);
    flush_etrax_cache();
    *R_DMA_CH9_CMD = IO_STATE(R_DMA_CH9_CMD, cmd, restart);
  }

  local_irq_restore(flags);

  return IRQ_HANDLED;
}

static void tc_bulk_start_timer_func(unsigned long dummy) {
  /* We might enable an EP descriptor behind the current DMA position when
     it's about to decide that there are no more bulk traffic and it should
     stop the bulk channel.
     Therefore we periodically check if the bulk channel is stopped and there
     is an enabled bulk EP descriptor, in which case we start the bulk
     channel. */
  
  if (!(*R_DMA_CH8_SUB0_CMD & IO_MASK(R_DMA_CH8_SUB0_CMD, cmd))) {
    int epid;

    timer_dbg("bulk_start_timer: Bulk DMA channel not running.\n");

    for (epid = 0; epid < NBR_OF_EPIDS; epid++) {
      if (TxBulkEPList[epid].command & IO_MASK(USB_EP_command, enable)) {
	timer_warn("Found enabled EP for epid %d, starting bulk channel.\n",
		   epid);
	restart_dma8_sub0();

	/* Restart the bulk eot timer since we just started the bulk channel.*/
	mod_timer(&bulk_eot_timer, jiffies + BULK_EOT_TIMER_INTERVAL);

	/* No need to search any further. */
	break;
      }
    }
  } else {
    timer_dbg("bulk_start_timer: Bulk DMA channel running.\n");
  }
}

static void tc_bulk_eot_timer_func(unsigned long dummy) {
  struct usb_hcd *hcd = (struct usb_hcd*)dummy;
  ASSERT(hcd);
  /* Because of a race condition in the top half, we might miss a bulk eot.
     This timer "simulates" a bulk eot if we don't get one for a while,
     hopefully correcting the situation. */
  timer_dbg("bulk_eot_timer timed out.\n");
  check_finished_bulk_tx_epids(hcd, 1);
}


/*************************************************************/
/*************************************************************/
/* Device driver block                                       */
/*************************************************************/
/*************************************************************/

/* Forward declarations for device driver functions */
static int devdrv_hcd_probe(struct device *);
static int devdrv_hcd_remove(struct device *);
#ifdef CONFIG_PM
static int devdrv_hcd_suspend(struct device *, u32, u32);
static int devdrv_hcd_resume(struct device *, u32);
#endif /* CONFIG_PM */

/* the device */
static struct platform_device *devdrv_hc_platform_device;

/* device driver interface */
static struct device_driver devdrv_hc_device_driver = {
  .name =			(char *) hc_name,
  .bus =			&platform_bus_type,

  .probe =		devdrv_hcd_probe,
  .remove =		devdrv_hcd_remove,

#ifdef CONFIG_PM
  .suspend =		devdrv_hcd_suspend,
  .resume =		devdrv_hcd_resume,
#endif /* CONFIG_PM */
};

/* initialize the host controller and driver  */
static int __init_or_module devdrv_hcd_probe(struct device *dev)
{
  struct usb_hcd *hcd;
  struct crisv10_hcd *crisv10_hcd;
  int retval;

  /* Check DMA burst length */
  if(IO_EXTRACT(R_BUS_CONFIG, dma_burst, *R_BUS_CONFIG) !=
     IO_STATE(R_BUS_CONFIG, dma_burst, burst32)) {
    devdrv_err("Invalid DMA burst length in Etrax 100LX,"
	       " needs to be 32\n");
    return -EPERM;
  }
  
  hcd = usb_create_hcd(&crisv10_hc_driver, dev, dev_name(dev));
  if (!hcd)
    return -ENOMEM;

  crisv10_hcd = hcd_to_crisv10_hcd(hcd);
  spin_lock_init(&crisv10_hcd->lock);
  crisv10_hcd->num_ports = num_ports();
  crisv10_hcd->running = 0;

  dev_set_drvdata(dev, crisv10_hcd);

  devdrv_dbg("ETRAX USB IRQs HC:%d  RX:%d  TX:%d\n", ETRAX_USB_HC_IRQ,
	  ETRAX_USB_RX_IRQ, ETRAX_USB_TX_IRQ);

  /* Print out chip version read from registers */
  int rev_maj = *R_USB_REVISION & IO_MASK(R_USB_REVISION, major);
  int rev_min = *R_USB_REVISION & IO_MASK(R_USB_REVISION, minor);
  if(rev_min == 0) {
    devdrv_info("Etrax 100LX USB Revision %d v1,2\n", rev_maj);
  } else {
    devdrv_info("Etrax 100LX USB Revision %d v%d\n", rev_maj, rev_min);
  }

  devdrv_info("Bulk timer interval, start:%d eot:%d\n",
	      BULK_START_TIMER_INTERVAL,
	      BULK_EOT_TIMER_INTERVAL);


  /* Init root hub data structures */
  if(rh_init()) {
    devdrv_err("Failed init data for Root Hub\n");
    retval = -ENOMEM;
  }

  if(port_in_use(0)) {
    if (cris_request_io_interface(if_usb_1, "ETRAX100LX USB-HCD")) {
      printk(KERN_CRIT "usb-host: request IO interface usb1 failed");
      retval = -EBUSY;
      goto out;
    }
    devdrv_info("Claimed interface for USB physical port 1\n");
  }
  if(port_in_use(1)) {
    if (cris_request_io_interface(if_usb_2, "ETRAX100LX USB-HCD")) {
      /* Free first interface if second failed to be claimed */
      if(port_in_use(0)) {
	cris_free_io_interface(if_usb_1);
      }
      printk(KERN_CRIT "usb-host: request IO interface usb2 failed");
      retval = -EBUSY;
      goto out;
    }
    devdrv_info("Claimed interface for USB physical port 2\n");
  }
  
  /* Init transfer controller structs and locks */
  if((retval = tc_init(hcd)) != 0) {
    goto out;
  }

  /* Attach interrupt functions for DMA and init DMA controller */
  if((retval = tc_dma_init(hcd)) != 0) {
    goto out;
  }

  /* Attach the top IRQ handler for USB controller interrupts */
  if (request_irq(ETRAX_USB_HC_IRQ, crisv10_hcd_top_irq, 0,
		  "ETRAX 100LX built-in USB (HC)", hcd)) {
    err("Could not allocate IRQ %d for USB", ETRAX_USB_HC_IRQ);
    retval = -EBUSY;
    goto out;
  }

  /* iso_eof is only enabled when isoc traffic is running. */
  *R_USB_IRQ_MASK_SET =
    /* IO_STATE(R_USB_IRQ_MASK_SET, iso_eof, set) | */
    IO_STATE(R_USB_IRQ_MASK_SET, bulk_eot, set) |
    IO_STATE(R_USB_IRQ_MASK_SET, epid_attn, set) |
    IO_STATE(R_USB_IRQ_MASK_SET, port_status, set) |
    IO_STATE(R_USB_IRQ_MASK_SET, ctl_status, set);


  crisv10_ready_wait();
  /* Reset the USB interface. */
  *R_USB_COMMAND =
    IO_STATE(R_USB_COMMAND, port_sel, nop) |
    IO_STATE(R_USB_COMMAND, port_cmd, reset) |
    IO_STATE(R_USB_COMMAND, ctrl_cmd, reset);

  /* Designer's Reference, p. 8 - 10 says we should Initate R_USB_FM_PSTART to
     0x2A30 (10800), to guarantee that control traffic gets 10% of the
     bandwidth, and periodic transfer may allocate the rest (90%).
     This doesn't work though.
     The value 11960 is chosen to be just after the SOF token, with a couple
     of bit times extra for possible bit stuffing. */
  *R_USB_FM_PSTART = IO_FIELD(R_USB_FM_PSTART, value, 11960);

  crisv10_ready_wait();
  /* Configure the USB interface as a host controller. */
  *R_USB_COMMAND =
    IO_STATE(R_USB_COMMAND, port_sel, nop) |
    IO_STATE(R_USB_COMMAND, port_cmd, reset) |
    IO_STATE(R_USB_COMMAND, ctrl_cmd, host_config);


  /* Check so controller not busy before enabling ports */
  crisv10_ready_wait();

  /* Enable selected USB ports */
  if(port_in_use(0)) {
    *R_USB_PORT1_DISABLE = IO_STATE(R_USB_PORT1_DISABLE, disable, no);
  } else {
    *R_USB_PORT1_DISABLE = IO_STATE(R_USB_PORT1_DISABLE, disable, yes);
  }
  if(port_in_use(1)) {
    *R_USB_PORT2_DISABLE = IO_STATE(R_USB_PORT2_DISABLE, disable, no);
  } else {
    *R_USB_PORT2_DISABLE = IO_STATE(R_USB_PORT2_DISABLE, disable, yes);
  }

  crisv10_ready_wait();
  /* Start processing of USB traffic. */
  *R_USB_COMMAND =
    IO_STATE(R_USB_COMMAND, port_sel, nop) |
    IO_STATE(R_USB_COMMAND, port_cmd, reset) |
    IO_STATE(R_USB_COMMAND, ctrl_cmd, host_run);

  /* Do not continue probing initialization before USB interface is done */
  crisv10_ready_wait();

  /* Register our Host Controller to USB Core
   * Finish the remaining parts of generic HCD initialization: allocate the
   * buffers of consistent memory, register the bus
   * and call the driver's reset() and start() routines. */
  retval = usb_add_hcd(hcd, ETRAX_USB_HC_IRQ, IRQF_DISABLED);
  if (retval != 0) {
    devdrv_err("Failed registering HCD driver\n");
    goto out;
  }

  return 0;

 out:
  devdrv_hcd_remove(dev);
  return retval;
}


/* cleanup after the host controller and driver */
static int __init_or_module devdrv_hcd_remove(struct device *dev)
{
  struct crisv10_hcd *crisv10_hcd = dev_get_drvdata(dev);
  struct usb_hcd *hcd;

  if (!crisv10_hcd)
    return 0;
  hcd = crisv10_hcd_to_hcd(crisv10_hcd);


  /* Stop USB Controller in Etrax 100LX */
  crisv10_hcd_reset(hcd);

  usb_remove_hcd(hcd);
  devdrv_dbg("Removed HCD from USB Core\n");

  /* Free USB Controller IRQ */
  free_irq(ETRAX_USB_HC_IRQ, NULL);

  /* Free resources */
  tc_dma_destroy();
  tc_destroy();


  if(port_in_use(0)) {
    cris_free_io_interface(if_usb_1);
  }
  if(port_in_use(1)) {
    cris_free_io_interface(if_usb_2);
  }

  devdrv_dbg("Freed all claimed resources\n");

  return 0;
}


#ifdef	CONFIG_PM

static int devdrv_hcd_suspend(struct usb_hcd *hcd, u32 state, u32 level)
{
  return 0; /* no-op for now */
}

static int devdrv_hcd_resume(struct usb_hcd *hcd, u32 level)
{
  return 0; /* no-op for now */
}

#endif /* CONFIG_PM */


/*************************************************************/
/*************************************************************/
/* Module block                                              */
/*************************************************************/
/*************************************************************/
 
/* register driver */
static int __init module_hcd_init(void) 
{
  
  if (usb_disabled())
    return -ENODEV;

  /* Here we select enabled ports by following defines created from
     menuconfig */
#ifndef CONFIG_ETRAX_USB_HOST_PORT1
  ports &= ~(1<<0);
#endif
#ifndef CONFIG_ETRAX_USB_HOST_PORT2
  ports &= ~(1<<1);
#endif

  printk(KERN_INFO "%s version "VERSION" "COPYRIGHT"\n", product_desc);

  devdrv_hc_platform_device =
    platform_device_register_simple((char *) hc_name, 0, NULL, 0);

  if (IS_ERR(devdrv_hc_platform_device))
    return PTR_ERR(devdrv_hc_platform_device);
  return driver_register(&devdrv_hc_device_driver);
  /* 
   * Note that we do not set the DMA mask for the device,
   * i.e. we pretend that we will use PIO, since no specific
   * allocation routines are needed for DMA buffers. This will
   * cause the HCD buffer allocation routines to fall back to
   * kmalloc().
   */
}

/* unregister driver */
static void __exit module_hcd_exit(void) 
{	
  driver_unregister(&devdrv_hc_device_driver);
}


/* Module hooks */
module_init(module_hcd_init);
module_exit(module_hcd_exit);

