/*****************************************************************************
 **   FILE NAME       : ifxhcd.h
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 1.0
 **   DATE            : 1/Jan/2009
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : This file contains the structures, constants, and interfaces for
 **                     the Host Contoller Driver (HCD).
 **
 **                     The Host Controller Driver (HCD) is responsible for translating requests
 **                     from the USB Driver into the appropriate actions on the IFXUSB controller.
 **                     It isolates the USBD from the specifics of the controller by providing an
 **                     API to the USBD.
 **   FUNCTIONS       :
 **   COMPILER        : gcc
 **   REFERENCE       : Synopsys DWC-OTG Driver 2.7
 **   COPYRIGHT       :
 **  Version Control Section  **
 **   $Author$
 **   $Date$
 **   $Revisions$
 **   $Log$       Revision history
*****************************************************************************/

/*!
  \defgroup IFXUSB_HCD HCD Interface
  \ingroup IFXUSB_DRIVER_V3
  \brief  The Host Controller Driver (HCD) is responsible for translating requests
 from the USB Driver into the appropriate actions on the IFXUSB controller.
 It isolates the USBD from the specifics of the controller by providing an
 API to the USBD.
 */


/*!
  \file ifxhcd.h
  \ingroup IFXUSB_DRIVER_V3
  \brief This file contains the structures, constants, and interfaces for
 the Host Contoller Driver (HCD).
 */

#if !defined(__IFXHCD_H__)
#define __IFXHCD_H__

#include <linux/list.h>
#include <linux/usb.h>

#ifdef __USE_TIMER_4_SOF__
#include <linux/hrtimer.h>
#endif
#include <linux/usb/hcd.h>

#include "ifxusb_cif.h"
#include "ifxusb_plat.h"



/*!
  \addtogroup IFXUSB_HCD
 */
/*@{*/

/* Phases for control transfers.*/
typedef enum ifxhcd_control_phase {
	IFXHCD_CONTROL_SETUP,
	IFXHCD_CONTROL_DATA,
	IFXHCD_CONTROL_STATUS
} ifxhcd_control_phase_e;

/* Reasons for halting a host channel. */
typedef enum ifxhcd_halt_status
{
	HC_XFER_NO_HALT_STATUS,         // Initial
	HC_XFER_COMPLETE,               // Xact complete without error, upward
	HC_XFER_URB_COMPLETE,           // Xfer complete without error, short upward
	HC_XFER_STALL,                  // HC stopped abnormally, upward/downward
	HC_XFER_XACT_ERR,               // HC stopped abnormally, upward
	HC_XFER_FRAME_OVERRUN,          // HC stopped abnormally, upward
	HC_XFER_BABBLE_ERR,             // HC stopped abnormally, upward
	HC_XFER_AHB_ERR,                // HC stopped abnormally, upward
	HC_XFER_DATA_TOGGLE_ERR,
	HC_XFER_URB_DEQUEUE,            // HC stopper manually, downward
	HC_XFER_NAK                     // HC stopped by nak monitor, downward
} ifxhcd_halt_status_e;

struct ifxhcd_urbd;
struct ifxhcd_hc ;
struct ifxhcd_epqh ;
struct ifxhcd_hcd;

/*!
 \brief A URB Descriptor (URBD) holds the state of a bulk, control,
  interrupt, or isochronous transfer. A single URBD is created for each URB
  (of one of these types) submitted to the HCD. The transfer associated with
  a URBD may require one or multiple transactions.

  A URBD is linked to a EP Queue Head, which is entered in either the
  isoc, intr or non-periodic schedule for execution. When a URBD is chosen for
  execution, some or all of its transactions may be executed. After
  execution, the state of the URBD is updated. The URBD may be retired if all
  its transactions are complete or if an error occurred. Otherwise, it
  remains in the schedule so more transactions can be executed later.
 */
typedef struct ifxhcd_urbd {
	struct list_head          urbd_list_entry;  // Hook for EPQH->urbd_list and ifxhcd->urbd_complete_list
	struct urb               *urb;              /*!< URB for this transfer */
	                                            //struct urb {
	                                            //  struct list_head urb_list;
	                                            //  struct list_head anchor_list;
	                                            //  struct usb_anchor * anchor;
	                                            //  struct usb_device * dev;
	                                            //  struct usb_host_endpoint * ep;
	                                            //  unsigned int pipe;
	                                            //  int status;
	                                            //  unsigned int transfer_flags;
	                                            //  void * transfer_buffer;
	                                            //  dma_addr_t transfer_dma;
	                                            //  u32 transfer_buffer_length;
	                                            //  u32 actual_length;
	                                            //  unsigned char * setup_packet;
	                                            //  dma_addr_t setup_dma;
	                                            //  int start_frame;
	                                            //  int number_of_packets;
	                                            //  int interval;
	                                            //  int error_count;
	                                            //  void * context;
	                                            //  usb_complete_t complete;
	                                            //  struct usb_iso_packet_descriptor iso_frame_desc[0];
	                                            //};
	                                            //urb_list         For use by current owner of the URB.
	                                            //anchor_list      membership in the list of an anchor
	                                            //anchor           to anchor URBs to a common mooring
	                                            //dev              Identifies the USB device to perform the request.
	                                            //ep               Points to the endpoint's data structure. Will
	                                            //                 eventually replace pipe.
	                                            //pipe             Holds endpoint number, direction, type, and more.
	                                            //                 Create these values with the eight macros available; u
	                                            //                 sb_{snd,rcv}TYPEpipe(dev,endpoint), where the TYPE is
	                                            //                  "ctrl", "bulk", "int" or "iso". For example
	                                            //                 usb_sndbulkpipe or usb_rcvintpipe. Endpoint numbers
	                                            //                 range from zero to fifteen. Note that "in" endpoint two
	                                            //                 is a different endpoint (and pipe) from "out" endpoint
	                                            //                 two. The current configuration controls the existence,
	                                            //                 type, and maximum packet size of any given endpoint.
	                                            //status           This is read in non-iso completion functions to get
	                                            //                 the status of the particular request. ISO requests
	                                            //                 only use it to tell whether the URB was unlinked;
	                                            //                 detailed status for each frame is in the fields of
	                                            //                 the iso_frame-desc.
	                                            //transfer_flags   A variety of flags may be used to affect how URB
	                                            //                 submission, unlinking, or operation are handled.
	                                            //                 Different kinds of URB can use different flags.
	                                            //                      URB_SHORT_NOT_OK
	                                            //                      URB_ISO_ASAP
	                                            //                      URB_NO_TRANSFER_DMA_MAP
	                                            //                      URB_NO_SETUP_DMA_MAP
	                                            //                      URB_NO_FSBR
	                                            //                      URB_ZERO_PACKET
	                                            //                      URB_NO_INTERRUPT
	                                            //transfer_buffer  This identifies the buffer to (or from) which the I/O
	                                            //                 request will be performed (unless URB_NO_TRANSFER_DMA_MAP
	                                            //                 is set). This buffer must be suitable for DMA; allocate it
	                                            //                 with kmalloc or equivalent. For transfers to "in"
	                                            //                 endpoints, contents of this buffer will be modified. This
	                                            //                 buffer is used for the data stage of control transfers.
	                                            //transfer_dma     When transfer_flags includes URB_NO_TRANSFER_DMA_MAP, the
	                                            //                 device driver is saying that it provided this DMA address,
	                                            //                 which the host controller driver should use in preference
	                                            //                 to the transfer_buffer.
	                                            //transfer_buffer_length How big is transfer_buffer. The transfer may be broken
	                                            //                 up into chunks according to the current maximum packet size
	                                            //                 for the endpoint, which is a function of the configuration
	                                            //                 and is encoded in the pipe. When the length is zero, neither
	                                            //                 transfer_buffer nor transfer_dma is used.
	                                            //actual_length    This is read in non-iso completion functions, and it tells
	                                            //                 how many bytes (out of transfer_buffer_length) were transferred.
	                                            //                 It will normally be the same as requested, unless either an error
	                                            //                 was reported or a short read was performed. The URB_SHORT_NOT_OK
	                                            //                 transfer flag may be used to make such short reads be reported
	                                            //                 as errors.
	                                            //setup_packet     Only used for control transfers, this points to eight bytes of
	                                            //                 setup data. Control transfers always start by sending this data
	                                            //                 to the device. Then transfer_buffer is read or written, if needed.
	                                            //setup_dma        For control transfers with URB_NO_SETUP_DMA_MAP set, the device
	                                            //                 driver has provided this DMA address for the setup packet. The
	                                            //                 host controller driver should use this in preference to setup_packet.
	                                            //start_frame      Returns the initial frame for isochronous transfers.
	                                            //number_of_packets Lists the number of ISO transfer buffers.
	                                            //interval         Specifies the polling interval for interrupt or isochronous transfers.
	                                            //                 The units are frames (milliseconds) for for full and low speed devices,
	                                            //                 and microframes (1/8 millisecond) for highspeed ones.
	                                            //error_count      Returns the number of ISO transfers that reported errors.
	                                            //context          For use in completion functions. This normally points to request-specific
	                                            //                 driver context.
	                                            //complete         Completion handler. This URB is passed as the parameter to the completion
	                                            //                 function. The completion function may then do what it likes with the URB,
	                                            //                 including resubmitting or freeing it.
	                                            //iso_frame_desc[0] Used to provide arrays of ISO transfer buffers and to collect the transfer
	                                            //                 status for each buffer.

	struct ifxhcd_epqh       *epqh;
                                                // Actual data portion, not SETUP or STATUS in case of CTRL XFER
                                                // DMA adjusted
	uint8_t                  *setup_buff;       /*!< Pointer to the entire transfer buffer. (CPU accessable)*/
	uint8_t                  *xfer_buff;        /*!< Pointer to the entire transfer buffer. (CPU accessable)*/
	uint32_t                  xfer_len;         /*!< Total number of bytes to transfer in this xfer. */
	unsigned                  is_in    :1;
	unsigned                  is_active:1;

	                          // For ALL XFER
	uint8_t                   error_count;    /*!< Holds the number of bus errors that have occurred for a transaction
	                                               within this transfer.
	                                           */
	/*== AVM/BC 20101111  Needed for URB Complete List ==*/
	int 					  status;
	                          // For ISOC XFER only
	#ifdef __EN_ISOC__
		int                       isoc_frame_index; /*!< Index of the next frame descriptor for an isochronous transfer. A
		                                                 frame descriptor describes the buffer position and length of the
		                                                 data to be transferred in the next scheduled (micro)frame of an
		                                                 isochronous transfer. It also holds status for that transaction.
		                                                 The frame index starts at 0.
		                                             */
		                          // For SPLITed ISOC XFER only
		uint8_t                   isoc_split_pos;   /*!< Position of the ISOC split on full/low speed */
		uint16_t                  isoc_split_offset;/*!< Position of the ISOC split in the buffer for the current frame */
	#endif
} ifxhcd_urbd_t;

/*!
 \brief A EP Queue Head (EPQH) holds the static characteristics of an endpoint and
 maintains a list of transfers (URBDs) for that endpoint. A EPQH structure may
 be entered in either the isoc, intr or non-periodic schedule.
 */

typedef struct ifxhcd_epqh {
	struct list_head     epqh_list_entry;   // Hook for EP Queues
	struct list_head     urbd_list;         /*!< List of URBDs for this EPQH. */
	struct ifxhcd_hc    *hc;                /*!< Host channel currently processing transfers for this EPQH. */
	struct ifxhcd_urbd  *urbd;              /*!< URBD currently assigned to a host channel for this EPQH. */
	struct usb_host_endpoint *sysep;
	uint8_t              ep_type;           /*!< Endpoint type. One of the following values:
	                                             - IFXUSB_EP_TYPE_CTRL
	                                             - IFXUSB_EP_TYPE_ISOC
	                                             - IFXUSB_EP_TYPE_BULK
	                                             - IFXUSB_EP_TYPE_INTR
	                                         */
	uint16_t             mps;               /*!< wMaxPacketSize Field of Endpoint Descriptor. */

	/* == AVM/WK 20100710 Fix - Use toggle of usbcore ==*/
	/*uint8_t              data_toggle;*/     /*!< Determines the PID of the next data packet
	                                             One of the following values:
	                                             - IFXHCD_HC_PID_DATA0
	                                             - IFXHCD_HC_PID_DATA1
	                                         */
	uint8_t              is_active;

	uint8_t              pkt_count_limit;
	#ifdef __EPQD_DESTROY_TIMEOUT__
		struct timer_list destroy_timer;
	#endif

	uint16_t             wait_for_sof;
	uint8_t              need_split;        /*!< Full/low speed endpoint on high-speed hub requires split. */
	uint16_t             interval;          /*!< Interval between transfers in (micro)frames. (for INTR)*/

	uint16_t             period_counter;    /*!< Interval between transfers in (micro)frames. */
	uint8_t              period_do;

	uint8_t aligned_checked;

	#if   defined(__UNALIGNED_BUFFER_ADJ__)
		uint8_t using_aligned_setup;
		uint8_t *aligned_setup;
		uint8_t using_aligned_buf;
		uint8_t *aligned_buf;
		unsigned aligned_buf_len : 19;
	#endif

	uint8_t   *dump_buf;
} ifxhcd_epqh_t;


#if defined(__HC_XFER_TIMEOUT__)
	struct ifxusb_core_if;
	struct ifxhcd_hc;
	typedef struct hc_xfer_info
	{
		struct ifxusb_core_if *core_if;
		struct ifxhcd_hc      *hc;
	} hc_xfer_info_t;
#endif //defined(__HC_XFER_TIMEOUT__)


/*!
 \brief Host channel descriptor. This structure represents the state of a single
 host channel when acting in host mode. It contains the data items needed to
 transfer packets to an endpoint via a host channel.
 */
typedef struct ifxhcd_hc
{
	struct list_head hc_list_entry  ; // Hook to free hc
	struct ifxhcd_epqh *epqh        ; /*!< EP Queue Head for the transfer being processed by this channel. */

	uint8_t  hc_num                 ; /*!< Host channel number used for register address lookup */
	uint8_t *xfer_buff              ; /*!< Pointer to the entire transfer buffer. */
	uint32_t xfer_count             ; /*!< Number of bytes transferred so far. The offset of the begin of the buf */
	uint32_t xfer_len               ; /*!< Total number of bytes to transfer in this xfer. */
	uint16_t start_pkt_count        ; /*!< Packet count at start of transfer. Used to calculate the actual xfer size*/
	ifxhcd_halt_status_e halt_status; /*!< Reason for halting the host channel. */

	unsigned dev_addr       : 7; /*!< Device to access */
	unsigned ep_num         : 4; /*!< EP to access */
	unsigned is_in          : 1; /*!< EP direction. 0: OUT, 1: IN */
	unsigned speed          : 2; /*!< EP speed. */
	unsigned ep_type        : 2; /*!< Endpoint type. */
	unsigned mps            :11; /*!< Max packet size in bytes */
	unsigned data_pid_start : 2; /*!< PID for initial transaction. */
	unsigned do_ping        : 1; /*!< Set to 1 to indicate that a PING request should be issued on this
	                                  channel. If 0, process normally.
	                              */

	unsigned xfer_started   : 1; /*!< Flag to indicate whether the transfer has been started. Set to 1 if
	                                  it has been started, 0 otherwise.
	                              */
	unsigned halting        : 1; /*!< Set to 1 if the host channel has been halted, but the core is not
	                                  finished flushing queued requests. Otherwise 0.
	                              */
	unsigned short_rw       : 1; /*!< When Tx, means termination needed.
	                                  When Rx, indicate Short Read  */
	/* Split settings for the host channel */
	unsigned split          : 2; /*!< Split: 0-Non Split, 1-SSPLIT, 2&3 CSPLIT */

	/*== AVM/BC 20100701 - Workaround FullSpeed Interrupts with HiSpeed Hub ==*/
	unsigned nyet_count;

	/* nak monitor */
	unsigned nak_retry_r    : 16;
	unsigned nak_retry      : 16;
		#define nak_retry_max     40000
	unsigned nak_countdown  : 8;
	unsigned nak_countdown_r: 8;
		#define nak_countdown_max 1

	uint16_t                  wait_for_sof;
	ifxhcd_control_phase_e    control_phase;  /*!< Current phase for control transfers (Setup, Data, or Status). */
	uint32_t ssplit_out_xfer_count; /*!< How many bytes transferred during SSPLIT OUT */
	#ifdef __DEBUG__
		uint32_t          start_hcchar_val;
	#endif
	#ifdef __HC_XFER_TIMEOUT__
		hc_xfer_info_t    hc_xfer_info;
		struct timer_list hc_xfer_timer;
	#endif
	uint32_t hcchar;

	/* Split settings for the host channel */
	uint8_t hub_addr;          /*!< Address of high speed hub */
	uint8_t port_addr;         /*!< Port of the low/full speed device */
	#ifdef __EN_ISOC__
		uint8_t isoc_xact_pos;          /*!< Split transaction position */
	#endif
} ifxhcd_hc_t;


/*!
 \brief This structure holds the state of the HCD, including the non-periodic and
 periodic schedules.
 */
typedef struct ifxhcd_hcd
{
	struct device *dev;
	struct hc_driver hc_driver;
	ifxusb_core_if_t core_if;   /*!< Pointer to the core interface structure. */
	struct usb_hcd *syshcd;

	volatile union ifxhcd_internal_flags
	{
		uint32_t d32;
		struct
		{
			unsigned port_connect_status_change : 1;
			unsigned port_connect_status        : 1;
			unsigned port_reset_change          : 1;
			unsigned port_enable_change         : 1;
			unsigned port_suspend_change        : 1;
			unsigned port_over_current_change   : 1;
			unsigned reserved                   : 27;
		} b;
	} flags; /*!< Internal HCD Flags */

	struct ifxhcd_hc ifxhc[MAX_EPS_CHANNELS];         /*!< Array of pointers to the host channel descriptors. Allows accessing
	                                                       a host channel descriptor given the host channel number. This is
	                                                       useful in interrupt handlers.
	                                                   */
	struct list_head free_hc_list;                    /*!< Free host channels in the controller. This is a list of ifxhcd_hc_t items. */
	uint8_t   *status_buf;                            /*!< Buffer to use for any data received during the status phase of a
	                                                       control transfer. Normally no data is transferred during the status
	                                                       phase. This buffer is used as a bit bucket.
	                                                   */
		#define IFXHCD_STATUS_BUF_SIZE 64

	struct list_head epqh_np_active;    // with URBD, with HC
	struct list_head epqh_np_ready;     // with URBD, No HC

	struct list_head epqh_intr_active;  // with URBD, with HC
	struct list_head epqh_intr_ready;   // with URBD, no pass, No HC

	#ifdef __EN_ISOC__
		struct list_head epqh_isoc_active;  // with URBD, with HC
		struct list_head epqh_isoc_ready;   // with URBD, no pass, No HC
	#endif

	/*== AVM/BC 20101111  URB Complete List ==*/
	struct list_head urbd_complete_list;

	struct list_head epqh_stdby;

	/* AVM/BC 20101111 flags removed */
	//unsigned process_channels_in_use  : 1;
	//unsigned select_eps_in_use        : 1;

	struct tasklet_struct  select_eps;                /*!<  Tasket to do a reset */
	uint32_t lastframe;
	spinlock_t	lock;
#ifdef __USE_TIMER_4_SOF__
	struct hrtimer hr_timer;
#endif
} ifxhcd_hcd_t;

/* Gets the ifxhcd_hcd from a struct usb_hcd */
static inline ifxhcd_hcd_t *syshcd_to_ifxhcd(struct usb_hcd *syshcd)
{
	return (ifxhcd_hcd_t *)(syshcd->hcd_priv[0]);
}

/* Gets the struct usb_hcd that contains a ifxhcd_hcd_t. */
static inline struct usb_hcd *ifxhcd_to_syshcd(ifxhcd_hcd_t *ifxhcd)
{
	return (struct usb_hcd *)(ifxhcd->syshcd);
}

/*! \brief HCD Create/Destroy Functions */
/*@{*/
	extern int  ifxhcd_init  (ifxhcd_hcd_t *_ifxhcd);
	extern void ifxhcd_remove(ifxhcd_hcd_t *_ifxhcd);
/*@}*/

/*! \brief Linux HC Driver API Functions */
/*@{*/
extern int  ifxhcd_start(struct usb_hcd *hcd);
extern void ifxhcd_stop (struct usb_hcd *hcd);
extern int  ifxhcd_get_frame_number(struct usb_hcd *hcd);


/*!
   \brief This function does the setup for a data transfer for a host channel and
  starts the transfer. May be called in either Slave mode or DMA mode. In
  Slave mode, the caller must ensure that there is sufficient space in the
  request queue and Tx Data FIFO.

  For an OUT transfer in Slave mode, it loads a data packet into the
  appropriate FIFO. If necessary, additional data packets will be loaded in
  the Host ISR.

  For an IN transfer in Slave mode, a data packet is requested. The data
  packets are unloaded from the Rx FIFO in the Host ISR. If necessary,
  additional data packets are requested in the Host ISR.

  For a PING transfer in Slave mode, the Do Ping bit is set in the HCTSIZ
  register along with a packet count of 1 and the channel is enabled. This
  causes a single PING transaction to occur. Other fields in HCTSIZ are
  simply set to 0 since no data transfer occurs in this case.

  For a PING transfer in DMA mode, the HCTSIZ register is initialized with
  all the information required to perform the subsequent data transfer. In
  addition, the Do Ping bit is set in the HCTSIZ register. In this case, the
  controller performs the entire PING protocol, then starts the data
  transfer.

  @param _ifxhc Information needed to initialize the host channel. The xfer_len
  value may be reduced to accommodate the max widths of the XferSize and
  PktCnt fields in the HCTSIZn register. The multi_count value may be changed
  to reflect the final xfer_len value.
 */
extern void ifxhcd_hc_start(ifxusb_core_if_t *_core_if, ifxhcd_hc_t *_ifxhc);

//extern int ifxhcd_urb_enqueue(struct usb_hcd *_syshcd, struct usb_host_endpoint *_sysep, struct urb *_urb, gfp_t mem_flags);
//extern int ifxhcd_urb_dequeue(struct usb_hcd *_syshcd, struct urb *_urb);
extern irqreturn_t ifxhcd_irq(struct usb_hcd *_syshcd);
int ifxhcd_urb_enqueue( struct usb_hcd           *_syshcd,
                        /*--- struct usb_host_endpoint *_sysep, Parameter im 2.6.28 entfallen ---*/
                        struct urb               *_urb,
                        gfp_t                     _mem_flags);
int ifxhcd_urb_dequeue( struct usb_hcd *_syshcd,
                        struct urb *_urb, int status /* Parameter neu in 2.6.28 */);

extern void ifxhcd_endpoint_disable(struct usb_hcd *_syshcd, struct usb_host_endpoint *_sysep);

extern int ifxhcd_hub_status_data(struct usb_hcd *_syshcd, char *_buf);
extern int ifxhcd_hub_control( struct usb_hcd *_syshcd,
                               u16             _typeReq,
                               u16             _wValue,
                               u16             _wIndex,
                               char           *_buf,
                               u16             _wLength);

/*@}*/

/*! \brief Transaction Execution Functions */
/*@{*/
extern void                      ifxhcd_complete_urb       (ifxhcd_hcd_t *_ifxhcd, ifxhcd_urbd_t *_urbd,  int _status);

/*@}*/

/*! \brief Deferred Transaction Execution Functions */
/*@{*/

/*== AVM/BC 20101111  URB Complete List ==*/
extern void                      defer_ifxhcd_complete_urb       (ifxhcd_hcd_t *_ifxhcd, ifxhcd_urbd_t *_urbd,  int _status);

/*!
   \brief Clears the transfer state for a host channel. This function is normally
  called after a transfer is done and the host channel is being released.
 */
extern void ifxhcd_hc_cleanup(ifxusb_core_if_t *_core_if, ifxhcd_hc_t *_ifxhc);

/*!
   \brief Attempts to halt a host channel. This function should only be called in
  Slave mode or to abort a transfer in either Slave mode or DMA mode. Under
  normal circumstances in DMA mode, the controller halts the channel when the
  transfer is complete or a condition occurs that requires application
  intervention.

  In slave mode, checks for a free request queue entry, then sets the Channel
  Enable and Channel Disable bits of the Host Channel Characteristics
  register of the specified channel to intiate the halt. If there is no free
  request queue entry, sets only the Channel Disable bit of the HCCHARn
  register to flush requests for this channel. In the latter case, sets a
  flag to indicate that the host channel needs to be halted when a request
  queue slot is open.

  In DMA mode, always sets the Channel Enable and Channel Disable bits of the
  HCCHARn register. The controller ensures there is space in the request
  queue before submitting the halt request.

  Some time may elapse before the core flushes any posted requests for this
  host channel and halts. The Channel Halted interrupt handler completes the
  deactivation of the host channel.
 */
extern void ifxhcd_hc_halt(ifxusb_core_if_t *_core_if,
                    ifxhcd_hc_t *_ifxhc,
                    ifxhcd_halt_status_e _halt_status);

/*!
   \brief Prepares a host channel for transferring packets to/from a specific
  endpoint. The HCCHARn register is set up with the characteristics specified
  in _ifxhc. Host channel interrupts that may need to be serviced while this
  transfer is in progress are enabled.
 */
extern void ifxhcd_hc_init(ifxusb_core_if_t *_core_if, ifxhcd_hc_t *_ifxhc);

/*!
   \brief This function is called to handle the disconnection of host port.
 */
int32_t ifxhcd_disconnect(ifxhcd_hcd_t *_ifxhcd);
/*@}*/

/*!  \brief Interrupt Handler Functions */
/*@{*/
extern irqreturn_t ifxhcd_oc_irq(int _irq, void *_dev);

extern int32_t ifxhcd_handle_oc_intr(ifxhcd_hcd_t *_ifxhcd);
extern int32_t ifxhcd_handle_intr   (ifxhcd_hcd_t *_ifxhcd);
/*@}*/


/*! \brief Schedule Queue Functions */
/*@{*/
extern ifxhcd_epqh_t *ifxhcd_epqh_create (ifxhcd_hcd_t *_ifxhcd, struct urb *_urb);
extern void           ifxhcd_epqh_free   (                       ifxhcd_epqh_t *_epqh);
extern void           select_eps      (ifxhcd_hcd_t *_ifxhcd);
extern void           process_channels(ifxhcd_hcd_t *_ifxhcd);
extern void           process_channels_sub(ifxhcd_hcd_t *_ifxhcd);
extern void 		  complete_channel(ifxhcd_hcd_t *_ifxhcd, ifxhcd_hc_t *_ifxhc, ifxhcd_urbd_t *_urbd);
extern void           ifxhcd_epqh_ready(ifxhcd_hcd_t *_ifxhcd, ifxhcd_epqh_t *_epqh);
extern void           ifxhcd_epqh_active(ifxhcd_hcd_t *_ifxhcd, ifxhcd_epqh_t *_epqh);
extern void           ifxhcd_epqh_idle(ifxhcd_hcd_t *_ifxhcd, ifxhcd_epqh_t *_epqh);
extern void           ifxhcd_epqh_idle_periodic(ifxhcd_epqh_t *_epqh);
extern int            ifxhcd_urbd_create (ifxhcd_hcd_t *_ifxhcd,struct urb *_urb);
/*@}*/

/*! \brief Gets the usb_host_endpoint associated with an URB. */
static inline struct usb_host_endpoint *ifxhcd_urb_to_endpoint(struct urb *_urb)
{
	struct usb_device *dev = _urb->dev;
	int    ep_num = usb_pipeendpoint(_urb->pipe);

	return (usb_pipein(_urb->pipe))?(dev->ep_in[ep_num]):(dev->ep_out[ep_num]);
}

/*!
 * \brief Gets the endpoint number from a _bEndpointAddress argument. The endpoint is
 * qualified with its direction (possible 32 endpoints per device).
 */
#define ifxhcd_ep_addr_to_endpoint(_bEndpointAddress_) ((_bEndpointAddress_ & USB_ENDPOINT_NUMBER_MASK) | \
                                                       ((_bEndpointAddress_ & USB_DIR_IN) != 0) << 4)


/* AVM/WK: not needed?

extern struct usb_device *usb_alloc_dev  (struct usb_device *parent, struct usb_bus *, unsigned port);
extern int                usb_add_hcd    (struct usb_hcd *syshcd, unsigned int irqnum, unsigned long irqflags);
extern void               usb_remove_hcd (struct usb_hcd *syshcd);
extern struct usb_hcd    *usb_create_hcd (const struct hc_driver *driver, struct device *dev, char *bus_name);
extern void               usb_hcd_giveback_urb (struct usb_hcd *syshcd, struct urb *urb);
extern void               usb_put_hcd       (struct usb_hcd *syshcd);
extern long               usb_calc_bus_time (int speed, int is_input, int isoc, int bytecount);

*/
/** Internal Functions */
void         ifxhcd_dump_state(ifxhcd_hcd_t *_ifxhcd);
extern char *syserr(int errno);

/*@}*//*IFXUSB_HCD*/

#endif // __IFXHCD_H__
