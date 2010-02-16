
/* macros for debug output */

#define hcd_dbg(hcd, fmt, args...) \
	dev_info(hcd->self.controller, fmt, ## args)
#define hcd_err(hcd, fmt, args...) \
	dev_err(hcd->self.controller, fmt, ## args)
#define hcd_info(hcd, fmt, args...) \
	dev_info(hcd->self.controller, fmt, ## args)
#define hcd_warn(hcd, fmt, args...) \
	dev_warn(hcd->self.controller, fmt, ## args)

/*
#define devdrv_dbg(fmt, args...) \
        printk(KERN_INFO "usb_devdrv dbg: ");printk(fmt, ## args)
*/
#define devdrv_dbg(fmt, args...) {}

#define devdrv_err(fmt, args...) \
        printk(KERN_ERR "usb_devdrv error: ");printk(fmt, ## args)
#define devdrv_info(fmt, args...) \
        printk(KERN_INFO "usb_devdrv: ");printk(fmt, ## args)

#define irq_dbg(fmt, args...) \
        printk(KERN_INFO "crisv10_irq dbg: ");printk(fmt, ## args)
#define irq_err(fmt, args...) \
        printk(KERN_ERR "crisv10_irq error: ");printk(fmt, ## args)
#define irq_warn(fmt, args...) \
        printk(KERN_INFO "crisv10_irq warn: ");printk(fmt, ## args)
#define irq_info(fmt, args...) \
        printk(KERN_INFO "crisv10_hcd: ");printk(fmt, ## args)

/*
#define rh_dbg(fmt, args...) \
  printk(KERN_DEBUG "crisv10_rh dbg: ");printk(fmt, ## args)
*/
#define rh_dbg(fmt, args...) {}

#define rh_err(fmt, args...) \
        printk(KERN_ERR "crisv10_rh error: ");printk(fmt, ## args)
#define rh_warn(fmt, args...) \
        printk(KERN_INFO "crisv10_rh warning: ");printk(fmt, ## args)
#define rh_info(fmt, args...) \
        printk(KERN_INFO "crisv10_rh: ");printk(fmt, ## args)

/*
#define tc_dbg(fmt, args...) \
        printk(KERN_INFO "crisv10_tc dbg: ");printk(fmt, ## args)
*/
#define tc_dbg(fmt, args...) {while(0){}}

#define tc_err(fmt, args...) \
        printk(KERN_ERR "crisv10_tc error: ");printk(fmt, ## args)
/*
#define tc_warn(fmt, args...) \
        printk(KERN_INFO "crisv10_tc warning: ");printk(fmt, ## args)
*/
#define tc_warn(fmt, args...) {while(0){}}

#define tc_info(fmt, args...) \
        printk(KERN_INFO "crisv10_tc: ");printk(fmt, ## args)


/* Debug print-outs for various traffic types */

#define intr_warn(fmt, args...) \
        printk(KERN_INFO "crisv10_intr warning: ");printk(fmt, ## args)
/*
#define intr_dbg(fmt, args...) \
        printk(KERN_DEBUG "crisv10_intr dbg: ");printk(fmt, ## args)
*/
#define intr_dbg(fmt, args...) {while(0){}}


#define isoc_err(fmt, args...) \
        printk(KERN_ERR "crisv10_isoc error: ");printk(fmt, ## args)
/*
#define isoc_warn(fmt, args...) \
        printk(KERN_INFO "crisv10_isoc warning: ");printk(fmt, ## args)
*/
#define isoc_warn(fmt, args...) {while(0){}}

/*
#define isoc_dbg(fmt, args...) \
        printk(KERN_INFO "crisv10_isoc dbg: ");printk(fmt, ## args)
*/
#define isoc_dbg(fmt, args...) {while(0){}}

/*
#define timer_warn(fmt, args...) \
        printk(KERN_INFO "crisv10_timer warning: ");printk(fmt, ## args)
*/
#define timer_warn(fmt, args...) {while(0){}}

/*
#define timer_dbg(fmt, args...) \
        printk(KERN_INFO "crisv10_timer dbg: ");printk(fmt, ## args)
*/
#define timer_dbg(fmt, args...) {while(0){}}


/* Debug printouts for events related to late finishing of URBs */
/*
#define late_dbg(fmt, args...) \
        printk(KERN_INFO "crisv10_late dbg: ");printk(fmt, ## args)
*/
#define late_dbg(fmt, args...) {while(0){}}

#define late_warn(fmt, args...) \
        printk(KERN_INFO "crisv10_late warning: ");printk(fmt, ## args)
/*
#define errno_dbg(fmt, args...) \
        printk(KERN_INFO "crisv10_errno dbg: ");printk(fmt, ## args)
*/
#define errno_dbg(fmt, args...) {while(0){}}


#define dma_dbg(fmt, args...) \
        printk(KERN_INFO "crisv10_dma dbg: ");printk(fmt, ## args)
#define dma_err(fmt, args...) \
        printk(KERN_ERR "crisv10_dma error: ");printk(fmt, ## args)
#define dma_warn(fmt, args...) \
        printk(KERN_INFO "crisv10_dma warning: ");printk(fmt, ## args)
#define dma_info(fmt, args...) \
        printk(KERN_INFO "crisv10_dma: ");printk(fmt, ## args)



#define str_dir(pipe) \
	(usb_pipeout(pipe) ? "out" : "in")
#define str_type(pipe) \
	({								\
		char *s = "?";						\
		switch (usb_pipetype(pipe)) {				\
		case PIPE_ISOCHRONOUS:	s = "iso";  break;		\
		case PIPE_INTERRUPT:	s = "intr"; break;		\
		case PIPE_CONTROL:	s = "ctrl"; break;		\
		case PIPE_BULK:		s = "bulk"; break;		\
		};							\
		s;							\
	})

