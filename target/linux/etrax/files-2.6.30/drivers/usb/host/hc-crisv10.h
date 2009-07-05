#ifndef __LINUX_ETRAX_USB_H
#define __LINUX_ETRAX_USB_H

#include <linux/types.h>
#include <linux/list.h>

struct USB_IN_Desc {
  volatile __u16 sw_len;
  volatile __u16 command;
  volatile unsigned long next;
  volatile unsigned long buf;
  volatile __u16 hw_len;
  volatile __u16 status;
};

struct USB_SB_Desc {
  volatile __u16 sw_len;
  volatile __u16 command;
  volatile unsigned long next;
  volatile unsigned long buf;
};

struct USB_EP_Desc {
  volatile __u16 hw_len;
  volatile __u16 command;
  volatile unsigned long sub;
  volatile unsigned long next;
};


/* Root Hub port status struct */
struct crisv10_rh {
  volatile __u16 wPortChange[2];
  volatile __u16 wPortStatusPrev[2];
};

/* HCD description */
struct crisv10_hcd {
  spinlock_t		lock;
  __u8			num_ports;
  __u8                  running;
};


/* Endpoint HC private data description */
struct crisv10_ep_priv {
  int epid;
};

/* Additional software state info for a USB Controller epid */
struct etrax_epid {
  __u8 inuse;       /* !0 = setup in Etrax and used for a endpoint */
  __u8 disabled;    /* !0 = Temporarly disabled to avoid resubmission */
  __u8 type;        /* Setup as: PIPE_BULK, PIPE_CONTROL ... */
  __u8 out_traffic; /* !0 = This epid is for out traffic */
};

/* Struct to hold information of scheduled later URB completion */
struct urb_later_data {
  struct delayed_work dws;
  struct usb_hcd *hcd;
  struct urb *urb;
  int urb_num;
  int status;
};


typedef enum {
  STARTED,
  NOT_STARTED,
  UNLINK,
} crisv10_urb_state_t;


struct crisv10_urb_priv {
  /* Sequence number for this URB. Every new submited URB gets this from
     a incrementing counter. Used when a URB is scheduled for later finish to
     be sure that the intended URB hasn't already been completed (device
     drivers has a tendency to reuse URBs once they are completed, causing us
     to not be able to single old ones out only based on the URB pointer.) */
  __u32 urb_num;

  /* The first_sb field is used for freeing all SB descriptors belonging
     to an urb. The corresponding ep descriptor's sub pointer cannot be
     used for this since the DMA advances the sub pointer as it processes
     the sb list. */
  struct USB_SB_Desc *first_sb;

  /* The last_sb field referes to the last SB descriptor that belongs to
     this urb. This is important to know so we can free the SB descriptors
     that ranges between first_sb and last_sb. */
  struct USB_SB_Desc *last_sb;
  
  /* The rx_offset field is used in ctrl and bulk traffic to keep track
     of the offset in the urb's transfer_buffer where incoming data should be
     copied to. */
  __u32 rx_offset;
  
  /* Counter used in isochronous transfers to keep track of the
     number of packets received/transmitted.  */
  __u32 isoc_packet_counter;

  /* Flag that marks if this Isoc Out URB has finished it's transfer. Used
     because several URBs can be finished before list is processed */
  __u8  isoc_out_done;
  
  /* This field is used to pass information about the urb's current state
     between the various interrupt handlers (thus marked volatile). */
  volatile crisv10_urb_state_t urb_state;
  
  /* In Ctrl transfers consist of (at least) 3 packets: SETUP, IN and ZOUT.
     When DMA8 sub-channel 2 has processed the SB list for this sequence we
     get a interrupt. We also get a interrupt for In transfers and which
     one of these interrupts that comes first depends of data size and device.
     To be sure that we have got both interrupts before we complete the URB
     we have these to flags that shows which part that has completed.
     We can then check when we get one of the interrupts that if the other has
     occured it's safe for us to complete the URB, otherwise we set appropriate
     flag and do the completion when we get the other interrupt. */
  volatile unsigned char ctrl_zout_done;
  volatile unsigned char ctrl_rx_done;

  /* Connection between the submitted urb and ETRAX epid number */
  __u8 epid;
  
  /* The rx_data_list field is used for periodic traffic, to hold
     received data for later processing in the the complete_urb functions,
     where the data us copied to the urb's transfer_buffer. Basically, we
     use this intermediate storage because we don't know when it's safe to
     reuse the transfer_buffer (FIXME?). */
  struct list_head rx_data_list;


  /* The interval time rounded up to closest 2^N */
  int interval;

  /* Pool of EP descriptors needed if it's a INTR transfer.
     Amount of EPs in pool correspons to how many INTR that should
     be inserted in TxIntrEPList (max 128, defined by MAX_INTR_INTERVAL) */
  struct USB_EP_Desc* intr_ep_pool[128];

  /* The mount of EPs allocated for this INTR URB */
  int intr_ep_pool_length;

  /* Pointer to info struct if URB is scheduled to be finished later */
  struct urb_later_data* later_data;

  /* Allocated bandwidth for isochronous and interrupt traffic */
  int bandwidth;
};


/* This struct is for passing data from the top half to the bottom half irq
   handlers */
struct crisv10_irq_reg {
  struct usb_hcd* hcd;
  __u32 r_usb_epid_attn;
  __u8 r_usb_status;
  __u16 r_usb_rh_port_status_1;
  __u16 r_usb_rh_port_status_2;
  __u32 r_usb_irq_mask_read;
  __u32 r_usb_fm_number;
  struct work_struct usb_bh;
};


/* This struct is for passing data from the isoc top half to the isoc bottom
   half. */
struct crisv10_isoc_complete_data {
  struct usb_hcd *hcd;
  struct urb *urb;
  struct work_struct usb_bh;
};

/* Entry item for URB lists for each endpint */
typedef struct urb_entry
{
	struct urb *urb;
	struct list_head list;
} urb_entry_t;

/* ---------------------------------------------------------------------------
   Virtual Root HUB
   ------------------------------------------------------------------------- */
/* destination of request */
#define RH_INTERFACE               0x01
#define RH_ENDPOINT                0x02
#define RH_OTHER                   0x03

#define RH_CLASS                   0x20
#define RH_VENDOR                  0x40

/* Requests: bRequest << 8 | bmRequestType */
#define RH_GET_STATUS           0x0080
#define RH_CLEAR_FEATURE        0x0100
#define RH_SET_FEATURE          0x0300
#define RH_SET_ADDRESS		0x0500
#define RH_GET_DESCRIPTOR	0x0680
#define RH_SET_DESCRIPTOR       0x0700
#define RH_GET_CONFIGURATION	0x0880
#define RH_SET_CONFIGURATION	0x0900
#define RH_GET_STATE            0x0280
#define RH_GET_INTERFACE        0x0A80
#define RH_SET_INTERFACE        0x0B00
#define RH_SYNC_FRAME           0x0C80
/* Our Vendor Specific Request */
#define RH_SET_EP               0x2000


/* Hub port features */
#define RH_PORT_CONNECTION         0x00
#define RH_PORT_ENABLE             0x01
#define RH_PORT_SUSPEND            0x02
#define RH_PORT_OVER_CURRENT       0x03
#define RH_PORT_RESET              0x04
#define RH_PORT_POWER              0x08
#define RH_PORT_LOW_SPEED          0x09
#define RH_C_PORT_CONNECTION       0x10
#define RH_C_PORT_ENABLE           0x11
#define RH_C_PORT_SUSPEND          0x12
#define RH_C_PORT_OVER_CURRENT     0x13
#define RH_C_PORT_RESET            0x14

/* Hub features */
#define RH_C_HUB_LOCAL_POWER       0x00
#define RH_C_HUB_OVER_CURRENT      0x01

#define RH_DEVICE_REMOTE_WAKEUP    0x00
#define RH_ENDPOINT_STALL          0x01

/* Our Vendor Specific feature */
#define RH_REMOVE_EP               0x00


#define RH_ACK                     0x01
#define RH_REQ_ERR                 -1
#define RH_NACK                    0x00

/* Field definitions for */

#define USB_IN_command__eol__BITNR      0 /* command macros */
#define USB_IN_command__eol__WIDTH      1
#define USB_IN_command__eol__no         0
#define USB_IN_command__eol__yes        1

#define USB_IN_command__intr__BITNR     3
#define USB_IN_command__intr__WIDTH     1
#define USB_IN_command__intr__no        0
#define USB_IN_command__intr__yes       1

#define USB_IN_status__eop__BITNR       1 /* status macros. */
#define USB_IN_status__eop__WIDTH       1
#define USB_IN_status__eop__no          0
#define USB_IN_status__eop__yes         1

#define USB_IN_status__eot__BITNR       5
#define USB_IN_status__eot__WIDTH       1
#define USB_IN_status__eot__no          0
#define USB_IN_status__eot__yes         1

#define USB_IN_status__error__BITNR     6
#define USB_IN_status__error__WIDTH     1
#define USB_IN_status__error__no        0
#define USB_IN_status__error__yes       1

#define USB_IN_status__nodata__BITNR    7
#define USB_IN_status__nodata__WIDTH    1
#define USB_IN_status__nodata__no       0
#define USB_IN_status__nodata__yes      1

#define USB_IN_status__epid__BITNR      8
#define USB_IN_status__epid__WIDTH      5

#define USB_EP_command__eol__BITNR      0
#define USB_EP_command__eol__WIDTH      1
#define USB_EP_command__eol__no         0
#define USB_EP_command__eol__yes        1

#define USB_EP_command__eof__BITNR      1
#define USB_EP_command__eof__WIDTH      1
#define USB_EP_command__eof__no         0
#define USB_EP_command__eof__yes        1

#define USB_EP_command__intr__BITNR     3
#define USB_EP_command__intr__WIDTH     1
#define USB_EP_command__intr__no        0
#define USB_EP_command__intr__yes       1

#define USB_EP_command__enable__BITNR   4
#define USB_EP_command__enable__WIDTH   1
#define USB_EP_command__enable__no      0
#define USB_EP_command__enable__yes     1

#define USB_EP_command__hw_valid__BITNR 5
#define USB_EP_command__hw_valid__WIDTH 1
#define USB_EP_command__hw_valid__no    0
#define USB_EP_command__hw_valid__yes   1

#define USB_EP_command__epid__BITNR     8
#define USB_EP_command__epid__WIDTH     5

#define USB_SB_command__eol__BITNR      0 /* command macros. */
#define USB_SB_command__eol__WIDTH      1
#define USB_SB_command__eol__no         0
#define USB_SB_command__eol__yes        1

#define USB_SB_command__eot__BITNR      1
#define USB_SB_command__eot__WIDTH      1
#define USB_SB_command__eot__no         0
#define USB_SB_command__eot__yes        1

#define USB_SB_command__intr__BITNR     3
#define USB_SB_command__intr__WIDTH     1
#define USB_SB_command__intr__no        0
#define USB_SB_command__intr__yes       1

#define USB_SB_command__tt__BITNR       4
#define USB_SB_command__tt__WIDTH       2
#define USB_SB_command__tt__zout        0
#define USB_SB_command__tt__in          1
#define USB_SB_command__tt__out         2
#define USB_SB_command__tt__setup       3


#define USB_SB_command__rem__BITNR      8
#define USB_SB_command__rem__WIDTH      6

#define USB_SB_command__full__BITNR     6
#define USB_SB_command__full__WIDTH     1
#define USB_SB_command__full__no        0
#define USB_SB_command__full__yes       1

#endif

