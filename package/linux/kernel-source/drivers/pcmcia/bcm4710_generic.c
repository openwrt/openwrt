/*
 *
 * bcm47xx pcmcia driver
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * Based on sa1100_generic.c from www.handhelds.org,
 *	and au1000_generic.c from oss.sgi.com.
 *
 * $Id$
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/config.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/tqueue.h>
#include <linux/timer.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/vmalloc.h>

#include <pcmcia/version.h>
#include <pcmcia/cs_types.h>
#include <pcmcia/cs.h>
#include <pcmcia/ss.h>
#include <pcmcia/bulkmem.h>
#include <pcmcia/cistpl.h>
#include <pcmcia/bus_ops.h>
#include "cs_internal.h"

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>

#include <typedefs.h>
#include <bcm4710.h>
#include <sbextif.h>

#include "bcm4710pcmcia.h"

#ifdef PCMCIA_DEBUG
static int pc_debug = PCMCIA_DEBUG;
#endif

MODULE_DESCRIPTION("Linux PCMCIA Card Services: bcm47xx Socket Controller");

/* This structure maintains housekeeping state for each socket, such
 * as the last known values of the card detect pins, or the Card Services
 * callback value associated with the socket:
 */
static struct bcm47xx_pcmcia_socket *pcmcia_socket;
static int socket_count;


/* Returned by the low-level PCMCIA interface: */
static struct pcmcia_low_level *pcmcia_low_level;

/* Event poll timer structure */
static struct timer_list poll_timer;


/* Prototypes for routines which are used internally: */

static int  bcm47xx_pcmcia_driver_init(void);
static void bcm47xx_pcmcia_driver_shutdown(void);
static void bcm47xx_pcmcia_task_handler(void *data);
static void bcm47xx_pcmcia_poll_event(unsigned long data);
static void bcm47xx_pcmcia_interrupt(int irq, void *dev, struct pt_regs *regs);
static struct tq_struct bcm47xx_pcmcia_task;

#ifdef CONFIG_PROC_FS
static int bcm47xx_pcmcia_proc_status(char *buf, char **start, 
		off_t pos, int count, int *eof, void *data);
#endif


/* Prototypes for operations which are exported to the
 * in-kernel PCMCIA core:
 */

static int bcm47xx_pcmcia_init(unsigned int sock);
static int bcm47xx_pcmcia_suspend(unsigned int sock);
static int bcm47xx_pcmcia_register_callback(unsigned int sock, 
		void (*handler)(void *, unsigned int), void *info);
static int bcm47xx_pcmcia_inquire_socket(unsigned int sock, socket_cap_t *cap);
static int bcm47xx_pcmcia_get_status(unsigned int sock, u_int *value);
static int bcm47xx_pcmcia_get_socket(unsigned int sock, socket_state_t *state);
static int bcm47xx_pcmcia_set_socket(unsigned int sock, socket_state_t *state);
static int bcm47xx_pcmcia_get_io_map(unsigned int sock, struct pccard_io_map *io);
static int bcm47xx_pcmcia_set_io_map(unsigned int sock, struct pccard_io_map *io);
static int bcm47xx_pcmcia_get_mem_map(unsigned int sock, struct pccard_mem_map *mem);
static int bcm47xx_pcmcia_set_mem_map(unsigned int sock, struct pccard_mem_map *mem);
#ifdef CONFIG_PROC_FS
static void bcm47xx_pcmcia_proc_setup(unsigned int sock, struct proc_dir_entry *base);
#endif

static struct pccard_operations bcm47xx_pcmcia_operations = {
	bcm47xx_pcmcia_init,
	bcm47xx_pcmcia_suspend,
	bcm47xx_pcmcia_register_callback,
	bcm47xx_pcmcia_inquire_socket,
	bcm47xx_pcmcia_get_status,
	bcm47xx_pcmcia_get_socket,
	bcm47xx_pcmcia_set_socket,
	bcm47xx_pcmcia_get_io_map,
	bcm47xx_pcmcia_set_io_map,
	bcm47xx_pcmcia_get_mem_map,
	bcm47xx_pcmcia_set_mem_map,
#ifdef CONFIG_PROC_FS
	bcm47xx_pcmcia_proc_setup
#endif
};


/*
 * bcm47xx_pcmcia_driver_init()
 *
 * This routine performs a basic sanity check to ensure that this
 * kernel has been built with the appropriate board-specific low-level
 * PCMCIA support, performs low-level PCMCIA initialization, registers
 * this socket driver with Card Services, and then spawns the daemon
 * thread which is the real workhorse of the socket driver.
 *
 * Please see linux/Documentation/arm/SA1100/PCMCIA for more information
 * on the low-level kernel interface.
 *
 * Returns: 0 on success, -1 on error
 */
static int __init bcm47xx_pcmcia_driver_init(void)
{
	servinfo_t info;
	struct pcmcia_init pcmcia_init;
	struct pcmcia_state state;
	unsigned int i;
	unsigned long tmp;


	printk("\nBCM47XX PCMCIA (CS release %s)\n", CS_RELEASE);

	CardServices(GetCardServicesInfo, &info);

	if (info.Revision != CS_RELEASE_CODE) {
		printk(KERN_ERR "Card Services release codes do not match\n");
		return -1;
	}

#ifdef CONFIG_BCM4710
	pcmcia_low_level=&bcm4710_pcmcia_ops;
#else
#error Unsupported Broadcom BCM47XX board.
#endif

	pcmcia_init.handler=bcm47xx_pcmcia_interrupt;

	if ((socket_count = pcmcia_low_level->init(&pcmcia_init)) < 0) {
		printk(KERN_ERR "Unable to initialize PCMCIA service.\n");
		return -EIO;
	} else {
		printk("\t%d PCMCIA sockets initialized.\n", socket_count);
	}

	pcmcia_socket = 
		kmalloc(sizeof(struct bcm47xx_pcmcia_socket) * socket_count, 
				GFP_KERNEL);
	memset(pcmcia_socket, 0, 
			sizeof(struct bcm47xx_pcmcia_socket) * socket_count);
	if (!pcmcia_socket) {
		printk(KERN_ERR "Card Services can't get memory \n");
		return -1;
	}
			
	for (i = 0; i < socket_count; i++) {
		if (pcmcia_low_level->socket_state(i, &state) < 0) {
			printk(KERN_ERR "Unable to get PCMCIA status\n");
			return -EIO;
		}
		pcmcia_socket[i].k_state = state;
		pcmcia_socket[i].cs_state.csc_mask = SS_DETECT;
		
		if (i == 0) {
			pcmcia_socket[i].virt_io =
				(unsigned long)ioremap_nocache(EXTIF_PCMCIA_IOBASE(BCM4710_EXTIF), 0x1000);
			/* Substract ioport base which gets added by in/out */
			pcmcia_socket[i].virt_io -= mips_io_port_base;
			pcmcia_socket[i].phys_attr =
				(unsigned long)EXTIF_PCMCIA_CFGBASE(BCM4710_EXTIF);
			pcmcia_socket[i].phys_mem =
				(unsigned long)EXTIF_PCMCIA_MEMBASE(BCM4710_EXTIF);
		} else  {
			printk(KERN_ERR "bcm4710: socket 1 not supported\n");
			return 1;
		}
	}

	/* Only advertise as many sockets as we can detect: */
	if (register_ss_entry(socket_count, &bcm47xx_pcmcia_operations) < 0) {
		printk(KERN_ERR "Unable to register socket service routine\n");
		return -ENXIO;
	}

	/* Start the event poll timer.  
	 * It will reschedule by itself afterwards. 
	 */
	bcm47xx_pcmcia_poll_event(0);

	DEBUG(1, "bcm4710: initialization complete\n");
	return 0;

}

module_init(bcm47xx_pcmcia_driver_init);


/*
 * bcm47xx_pcmcia_driver_shutdown()
 *
 * Invokes the low-level kernel service to free IRQs associated with this
 * socket controller and reset GPIO edge detection.
 */
static void __exit bcm47xx_pcmcia_driver_shutdown(void)
{
	int i;

	del_timer_sync(&poll_timer);
	unregister_ss_entry(&bcm47xx_pcmcia_operations);
	pcmcia_low_level->shutdown();
	flush_scheduled_tasks();
	for (i = 0; i < socket_count; i++) {
		if (pcmcia_socket[i].virt_io) 
			iounmap((void *)pcmcia_socket[i].virt_io);
		if (pcmcia_socket[i].phys_attr) 
			iounmap((void *)pcmcia_socket[i].phys_attr);
		if (pcmcia_socket[i].phys_mem) 
			iounmap((void *)pcmcia_socket[i].phys_mem);
	}
	DEBUG(1, "bcm4710: shutdown complete\n");
}

module_exit(bcm47xx_pcmcia_driver_shutdown);

/*
 * bcm47xx_pcmcia_init()
 * We perform all of the interesting initialization tasks in 
 * bcm47xx_pcmcia_driver_init().
 *
 * Returns: 0
 */
static int bcm47xx_pcmcia_init(unsigned int sock)
{
	DEBUG(1, "%s(): initializing socket %u\n", __FUNCTION__, sock);

	return 0;
}

/*
 * bcm47xx_pcmcia_suspend()
 *
 * We don't currently perform any actions on a suspend.
 *
 * Returns: 0
 */
static int bcm47xx_pcmcia_suspend(unsigned int sock)
{
	DEBUG(1, "%s(): suspending socket %u\n", __FUNCTION__, sock);

	return 0;
}


/*
 * bcm47xx_pcmcia_events()
 *
 * Helper routine to generate a Card Services event mask based on
 * state information obtained from the kernel low-level PCMCIA layer
 * in a recent (and previous) sampling. Updates `prev_state'.
 *
 * Returns: an event mask for the given socket state.
 */
static inline unsigned 
bcm47xx_pcmcia_events(struct pcmcia_state *state, 
		struct pcmcia_state *prev_state, 
		unsigned int mask, unsigned int flags)
{
	unsigned int events=0;

	if (state->bvd1 != prev_state->bvd1) {

		DEBUG(3, "%s(): card BVD1 value %u\n", __FUNCTION__, state->bvd1);

		events |= mask & (flags & SS_IOCARD) ? SS_STSCHG : SS_BATDEAD;
	}

	if (state->bvd2 != prev_state->bvd2) {

		DEBUG(3, "%s(): card BVD2 value %u\n", __FUNCTION__, state->bvd2);

		events |= mask & (flags & SS_IOCARD) ? 0 : SS_BATWARN;
	}

	if (state->detect != prev_state->detect) {

		DEBUG(3, "%s(): card detect value %u\n", __FUNCTION__, state->detect);

		events |= mask & SS_DETECT;
	}


	if (state->ready != prev_state->ready) {

		DEBUG(3, "%s(): card ready value %u\n", __FUNCTION__, state->ready);

		events |= mask & ((flags & SS_IOCARD) ? 0 : SS_READY);
	}

	if (events != 0) {
		DEBUG(2, "events: %s%s%s%s%s\n",
		      (events & SS_DETECT) ? "DETECT " : "",
		      (events & SS_READY) ? "READY " : "",
		      (events & SS_BATDEAD) ? "BATDEAD " : "",
		      (events & SS_BATWARN) ? "BATWARN " : "",
		      (events & SS_STSCHG) ? "STSCHG " : "");
	}

	*prev_state=*state;
	return events;
}


/* 
 * bcm47xx_pcmcia_task_handler()
 *
 * Processes serviceable socket events using the "eventd" thread context.
 *
 * Event processing (specifically, the invocation of the Card Services event
 * callback) occurs in this thread rather than in the actual interrupt
 * handler due to the use of scheduling operations in the PCMCIA core.
 */
static void bcm47xx_pcmcia_task_handler(void *data) 
{
	struct pcmcia_state state;
	int i, events, irq_status;

	DEBUG(4, "%s(): entering PCMCIA monitoring thread\n", __FUNCTION__);

	for (i = 0; i < socket_count; i++)  {
		if ((irq_status = pcmcia_low_level->socket_state(i, &state)) < 0)
			printk(KERN_ERR "Error in kernel low-level PCMCIA service.\n");

		events = bcm47xx_pcmcia_events(&state, 
					       &pcmcia_socket[i].k_state, 
					       pcmcia_socket[i].cs_state.csc_mask, 
					       pcmcia_socket[i].cs_state.flags);

		if (pcmcia_socket[i].handler != NULL) {
			pcmcia_socket[i].handler(pcmcia_socket[i].handler_info,
						 events);
		}
	}
}

static struct tq_struct bcm47xx_pcmcia_task = {
	routine: bcm47xx_pcmcia_task_handler
};


/*
 * bcm47xx_pcmcia_poll_event()
 *
 * Let's poll for events in addition to IRQs since IRQ only is unreliable...
 */
static void bcm47xx_pcmcia_poll_event(unsigned long dummy)
{
	DEBUG(4, "%s(): polling for events\n", __FUNCTION__);

	poll_timer.function = bcm47xx_pcmcia_poll_event;
	poll_timer.expires = jiffies + BCM47XX_PCMCIA_POLL_PERIOD;
	add_timer(&poll_timer);
	schedule_task(&bcm47xx_pcmcia_task);
}


/* 
 * bcm47xx_pcmcia_interrupt()
 *
 * Service routine for socket driver interrupts (requested by the
 * low-level PCMCIA init() operation via bcm47xx_pcmcia_thread()).
 *
 * The actual interrupt-servicing work is performed by
 * bcm47xx_pcmcia_task(), largely because the Card Services event-
 * handling code performs scheduling operations which cannot be
 * executed from within an interrupt context.
 */
static void 
bcm47xx_pcmcia_interrupt(int irq, void *dev, struct pt_regs *regs)
{
	DEBUG(3, "%s(): servicing IRQ %d\n", __FUNCTION__, irq);
	schedule_task(&bcm47xx_pcmcia_task);
}


/*
 * bcm47xx_pcmcia_register_callback()
 *
 * Implements the register_callback() operation for the in-kernel
 * PCMCIA service (formerly SS_RegisterCallback in Card Services). If 
 * the function pointer `handler' is not NULL, remember the callback 
 * location in the state for `sock', and increment the usage counter 
 * for the driver module. (The callback is invoked from the interrupt
 * service routine, bcm47xx_pcmcia_interrupt(), to notify Card Services
 * of interesting events.) Otherwise, clear the callback pointer in the
 * socket state and decrement the module usage count.
 *
 * Returns: 0
 */
static int 
bcm47xx_pcmcia_register_callback(unsigned int sock, 
		void (*handler)(void *, unsigned int), void *info)
{
	if (handler == NULL) {
		pcmcia_socket[sock].handler = NULL;
		MOD_DEC_USE_COUNT;
	} else {
		MOD_INC_USE_COUNT;
		pcmcia_socket[sock].handler = handler;
		pcmcia_socket[sock].handler_info = info;
	}
	return 0;
}


/*
 * bcm47xx_pcmcia_inquire_socket()
 *
 * Implements the inquire_socket() operation for the in-kernel PCMCIA
 * service (formerly SS_InquireSocket in Card Services). Of note is
 * the setting of the SS_CAP_PAGE_REGS bit in the `features' field of
 * `cap' to "trick" Card Services into tolerating large "I/O memory" 
 * addresses. Also set is SS_CAP_STATIC_MAP, which disables the memory
 * resource database check. (Mapped memory is set up within the socket
 * driver itself.)
 *
 * In conjunction with the STATIC_MAP capability is a new field,
 * `io_offset', recommended by David Hinds. Rather than go through
 * the SetIOMap interface (which is not quite suited for communicating
 * window locations up from the socket driver), we just pass up
 * an offset which is applied to client-requested base I/O addresses
 * in alloc_io_space().
 *
 * Returns: 0 on success, -1 if no pin has been configured for `sock'
 */
static int
bcm47xx_pcmcia_inquire_socket(unsigned int sock, socket_cap_t *cap)
{
	struct pcmcia_irq_info irq_info;

	if (sock >= socket_count) {
		printk(KERN_ERR "bcm47xx: socket %u not configured\n", sock);
		return -1;
	}

	/* SS_CAP_PAGE_REGS: used by setup_cis_mem() in cistpl.c to set the
	 *   force_low argument to validate_mem() in rsrc_mgr.c -- since in
	 *   general, the mapped * addresses of the PCMCIA memory regions
	 *   will not be within 0xffff, setting force_low would be
	 *   undesirable.
	 *
	 * SS_CAP_STATIC_MAP: don't bother with the (user-configured) memory
	 *   resource database; we instead pass up physical address ranges
	 *   and allow other parts of Card Services to deal with remapping.
	 *
	 * SS_CAP_PCCARD: we can deal with 16-bit PCMCIA & CF cards, but
	 *   not 32-bit CardBus devices.
	 */
	cap->features = (SS_CAP_PAGE_REGS  | SS_CAP_STATIC_MAP | SS_CAP_PCCARD);

	irq_info.sock = sock;
	irq_info.irq = -1;

	if (pcmcia_low_level->get_irq_info(&irq_info) < 0) {
		printk(KERN_ERR "Error obtaining IRQ info socket %u\n", sock);
		return -1;
	}

	cap->irq_mask = 0;
	cap->map_size = PAGE_SIZE;
	cap->pci_irq = irq_info.irq;
	cap->io_offset = pcmcia_socket[sock].virt_io;

	return 0;
}


/*
 * bcm47xx_pcmcia_get_status()
 *
 * Implements the get_status() operation for the in-kernel PCMCIA
 * service (formerly SS_GetStatus in Card Services). Essentially just
 * fills in bits in `status' according to internal driver state or
 * the value of the voltage detect chipselect register.
 *
 * As a debugging note, during card startup, the PCMCIA core issues
 * three set_socket() commands in a row the first with RESET deasserted,
 * the second with RESET asserted, and the last with RESET deasserted
 * again. Following the third set_socket(), a get_status() command will
 * be issued. The kernel is looking for the SS_READY flag (see
 * setup_socket(), reset_socket(), and unreset_socket() in cs.c).
 *
 * Returns: 0
 */
static int 
bcm47xx_pcmcia_get_status(unsigned int sock, unsigned int *status)
{
	struct pcmcia_state state;


	if ((pcmcia_low_level->socket_state(sock, &state)) < 0) {
		printk(KERN_ERR "Unable to get PCMCIA status from kernel.\n");
		return -1;
	}

	pcmcia_socket[sock].k_state = state;

	*status = state.detect ? SS_DETECT : 0;

	*status |= state.ready ? SS_READY : 0;

	/* The power status of individual sockets is not available
	 * explicitly from the hardware, so we just remember the state
	 * and regurgitate it upon request:
	 */
	*status |= pcmcia_socket[sock].cs_state.Vcc ? SS_POWERON : 0;

	if (pcmcia_socket[sock].cs_state.flags & SS_IOCARD)
		*status |= state.bvd1 ? SS_STSCHG : 0;
	else {
		if (state.bvd1 == 0)
			*status |= SS_BATDEAD;
		else if (state.bvd2 == 0)
			*status |= SS_BATWARN;
	}

	*status |= state.vs_3v ? SS_3VCARD : 0;

	*status |= state.vs_Xv ? SS_XVCARD : 0;

	DEBUG(2, "\tstatus: %s%s%s%s%s%s%s%s\n",
	      (*status&SS_DETECT)?"DETECT ":"",
	      (*status&SS_READY)?"READY ":"", 
	      (*status&SS_BATDEAD)?"BATDEAD ":"",
	      (*status&SS_BATWARN)?"BATWARN ":"",
	      (*status&SS_POWERON)?"POWERON ":"",
	      (*status&SS_STSCHG)?"STSCHG ":"",
	      (*status&SS_3VCARD)?"3VCARD ":"",
	      (*status&SS_XVCARD)?"XVCARD ":"");

	return 0;
}


/*
 * bcm47xx_pcmcia_get_socket()
 *
 * Implements the get_socket() operation for the in-kernel PCMCIA
 * service (formerly SS_GetSocket in Card Services). Not a very 
 * exciting routine.
 *
 * Returns: 0
 */
static int 
bcm47xx_pcmcia_get_socket(unsigned int sock, socket_state_t *state)
{
	DEBUG(2, "%s() for sock %u\n", __FUNCTION__, sock);

	/* This information was given to us in an earlier call to set_socket(),
	 * so we're just regurgitating it here:
	 */
	*state = pcmcia_socket[sock].cs_state;
	return 0;
}


/*
 * bcm47xx_pcmcia_set_socket()
 *
 * Implements the set_socket() operation for the in-kernel PCMCIA
 * service (formerly SS_SetSocket in Card Services). We more or
 * less punt all of this work and let the kernel handle the details
 * of power configuration, reset, &c. We also record the value of
 * `state' in order to regurgitate it to the PCMCIA core later.
 *
 * Returns: 0
 */
static int 
bcm47xx_pcmcia_set_socket(unsigned int sock, socket_state_t *state)
{
	struct pcmcia_configure configure;

	DEBUG(2, "\tmask:  %s%s%s%s%s%s\n\tflags: %s%s%s%s%s%s\n"
	      "\tVcc %d  Vpp %d  irq %d\n",
	      (state->csc_mask == 0) ? "<NONE>" : "",
	      (state->csc_mask & SS_DETECT) ? "DETECT " : "",
	      (state->csc_mask & SS_READY) ? "READY " : "",
	      (state->csc_mask & SS_BATDEAD) ? "BATDEAD " : "",
	      (state->csc_mask & SS_BATWARN) ? "BATWARN " : "",
	      (state->csc_mask & SS_STSCHG) ? "STSCHG " : "",
	      (state->flags == 0) ? "<NONE>" : "",
	      (state->flags & SS_PWR_AUTO) ? "PWR_AUTO " : "",
	      (state->flags & SS_IOCARD) ? "IOCARD " : "",
	      (state->flags & SS_RESET) ? "RESET " : "",
	      (state->flags & SS_SPKR_ENA) ? "SPKR_ENA " : "",
	      (state->flags & SS_OUTPUT_ENA) ? "OUTPUT_ENA " : "",
	      state->Vcc, state->Vpp, state->io_irq);

	configure.sock = sock;
	configure.vcc = state->Vcc;
	configure.vpp = state->Vpp;
	configure.output = (state->flags & SS_OUTPUT_ENA) ? 1 : 0;
	configure.speaker = (state->flags & SS_SPKR_ENA) ? 1 : 0;
	configure.reset = (state->flags & SS_RESET) ? 1 : 0;

	if (pcmcia_low_level->configure_socket(&configure) < 0) {
		printk(KERN_ERR "Unable to configure socket %u\n", sock);
		return -1;
	}

	pcmcia_socket[sock].cs_state = *state;
	return 0;
}


/*
 * bcm47xx_pcmcia_get_io_map()
 *
 * Implements the get_io_map() operation for the in-kernel PCMCIA
 * service (formerly SS_GetIOMap in Card Services). Just returns an
 * I/O map descriptor which was assigned earlier by a set_io_map().
 *
 * Returns: 0 on success, -1 if the map index was out of range
 */
static int 
bcm47xx_pcmcia_get_io_map(unsigned int sock, struct pccard_io_map *map)
{
	DEBUG(2, "bcm47xx_pcmcia_get_io_map: sock %d\n", sock);

	if (map->map >= MAX_IO_WIN) {
		printk(KERN_ERR "%s(): map (%d) out of range\n", 
		       __FUNCTION__, map->map);
		return -1;
	}

	*map = pcmcia_socket[sock].io_map[map->map];
	return 0;
}


/*
 * bcm47xx_pcmcia_set_io_map()
 *
 * Implements the set_io_map() operation for the in-kernel PCMCIA
 * service (formerly SS_SetIOMap in Card Services). We configure
 * the map speed as requested, but override the address ranges
 * supplied by Card Services.
 *
 * Returns: 0 on success, -1 on error
 */
int 
bcm47xx_pcmcia_set_io_map(unsigned int sock, struct pccard_io_map *map)
{
	unsigned int speed;
	unsigned long start;

	DEBUG(2, "\tmap %u  speed %u\n\tstart 0x%08lx  stop 0x%08lx\n"
	      "\tflags: %s%s%s%s%s%s%s%s\n",
	      map->map, map->speed, map->start, map->stop,
	      (map->flags == 0) ? "<NONE>" : "",
	      (map->flags & MAP_ACTIVE) ? "ACTIVE " : "",
	      (map->flags & MAP_16BIT) ? "16BIT " : "",
	      (map->flags & MAP_AUTOSZ) ? "AUTOSZ " : "",
	      (map->flags & MAP_0WS) ? "0WS " : "",
	      (map->flags & MAP_WRPROT) ? "WRPROT " : "",
	      (map->flags & MAP_USE_WAIT) ? "USE_WAIT " : "",
	      (map->flags & MAP_PREFETCH) ? "PREFETCH " : "");

	if (map->map >= MAX_IO_WIN) {
		printk(KERN_ERR "%s(): map (%d) out of range\n", 
				__FUNCTION__, map->map);
		return -1;
	}

	if (map->flags & MAP_ACTIVE) {
		speed = (map->speed > 0) ? map->speed : BCM47XX_PCMCIA_IO_SPEED;
		pcmcia_socket[sock].speed_io = speed;
	}

	start = map->start;

	if (map->stop == 1) {
		map->stop = PAGE_SIZE - 1;
	}

	map->start = pcmcia_socket[sock].virt_io;
	map->stop = map->start + (map->stop - start);
	pcmcia_socket[sock].io_map[map->map] = *map;
	DEBUG(2, "set_io_map %d start %x stop %x\n", 
	      map->map, map->start, map->stop);
	return 0;
}


/*
 * bcm47xx_pcmcia_get_mem_map()
 *
 * Implements the get_mem_map() operation for the in-kernel PCMCIA
 * service (formerly SS_GetMemMap in Card Services). Just returns a
 *  memory map descriptor which was assigned earlier by a
 *  set_mem_map() request.
 *
 * Returns: 0 on success, -1 if the map index was out of range
 */
static int 
bcm47xx_pcmcia_get_mem_map(unsigned int sock, struct pccard_mem_map *map)
{
	DEBUG(2, "%s() for sock %u\n", __FUNCTION__, sock);

	if (map->map >= MAX_WIN) {
		printk(KERN_ERR "%s(): map (%d) out of range\n", 
		       __FUNCTION__, map->map);
		return -1;
	}

	*map = pcmcia_socket[sock].mem_map[map->map];
	return 0;
}


/*
 * bcm47xx_pcmcia_set_mem_map()
 *
 * Implements the set_mem_map() operation for the in-kernel PCMCIA
 * service (formerly SS_SetMemMap in Card Services). We configure
 * the map speed as requested, but override the address ranges
 * supplied by Card Services.
 *
 * Returns: 0 on success, -1 on error
 */
static int 
bcm47xx_pcmcia_set_mem_map(unsigned int sock, struct pccard_mem_map *map)
{
	unsigned int speed;
	unsigned long start;
	u_long flags;

	if (map->map >= MAX_WIN) {
		printk(KERN_ERR "%s(): map (%d) out of range\n", 
		       __FUNCTION__, map->map);
		return -1;
	}

	DEBUG(2, "\tmap %u  speed %u\n\tsys_start  %#lx\n"
	      "\tsys_stop   %#lx\n\tcard_start %#x\n"
	      "\tflags: %s%s%s%s%s%s%s%s\n",
	      map->map, map->speed, map->sys_start, map->sys_stop,
	      map->card_start, (map->flags == 0) ? "<NONE>" : "",
	      (map->flags & MAP_ACTIVE) ? "ACTIVE " : "",
	      (map->flags & MAP_16BIT) ? "16BIT " : "",
	      (map->flags & MAP_AUTOSZ) ? "AUTOSZ " : "",
	      (map->flags & MAP_0WS) ? "0WS " : "",
	      (map->flags & MAP_WRPROT) ? "WRPROT " : "",
	      (map->flags & MAP_ATTRIB) ? "ATTRIB " : "",
	      (map->flags & MAP_USE_WAIT) ? "USE_WAIT " : "");

	if (map->flags & MAP_ACTIVE) {
		/* When clients issue RequestMap, the access speed is not always
		 * properly configured:
		 */
		speed = (map->speed > 0) ? map->speed : BCM47XX_PCMCIA_MEM_SPEED;

		/* TBD */
		if (map->flags & MAP_ATTRIB) {
			pcmcia_socket[sock].speed_attr = speed;
		} else {
			pcmcia_socket[sock].speed_mem = speed;
		}
	}

	save_flags(flags);
	cli();
	start = map->sys_start;

	if (map->sys_stop == 0)
		map->sys_stop = PAGE_SIZE - 1;

	if (map->flags & MAP_ATTRIB) {
		map->sys_start = pcmcia_socket[sock].phys_attr + 
			map->card_start;
	} else {
		map->sys_start = pcmcia_socket[sock].phys_mem + 
			map->card_start;
	}

	map->sys_stop = map->sys_start + (map->sys_stop - start);
	pcmcia_socket[sock].mem_map[map->map] = *map;
	restore_flags(flags);
	DEBUG(2, "set_mem_map %d start %x stop %x card_start %x\n", 
			map->map, map->sys_start, map->sys_stop, 
			map->card_start);
	return 0;
}


#if defined(CONFIG_PROC_FS)

/*
 * bcm47xx_pcmcia_proc_setup()
 *
 * Implements the proc_setup() operation for the in-kernel PCMCIA
 * service (formerly SS_ProcSetup in Card Services).
 *
 * Returns: 0 on success, -1 on error
 */
static void 
bcm47xx_pcmcia_proc_setup(unsigned int sock, struct proc_dir_entry *base)
{
	struct proc_dir_entry *entry;

	if ((entry = create_proc_entry("status", 0, base)) == NULL) {
		printk(KERN_ERR "Unable to install \"status\" procfs entry\n");
		return;
	}

	entry->read_proc = bcm47xx_pcmcia_proc_status;
	entry->data = (void *)sock;
}


/*
 * bcm47xx_pcmcia_proc_status()
 *
 * Implements the /proc/bus/pccard/??/status file.
 *
 * Returns: the number of characters added to the buffer
 */
static int 
bcm47xx_pcmcia_proc_status(char *buf, char **start, off_t pos, 
			   int count, int *eof, void *data)
{
	char *p = buf;
	unsigned int sock = (unsigned int)data;

	p += sprintf(p, "k_flags  : %s%s%s%s%s%s%s\n", 
		     pcmcia_socket[sock].k_state.detect ? "detect " : "",
		     pcmcia_socket[sock].k_state.ready ? "ready " : "",
		     pcmcia_socket[sock].k_state.bvd1 ? "bvd1 " : "",
		     pcmcia_socket[sock].k_state.bvd2 ? "bvd2 " : "",
		     pcmcia_socket[sock].k_state.wrprot ? "wrprot " : "",
		     pcmcia_socket[sock].k_state.vs_3v ? "vs_3v " : "",
		     pcmcia_socket[sock].k_state.vs_Xv ? "vs_Xv " : "");

	p += sprintf(p, "status   : %s%s%s%s%s%s%s%s%s\n",
		     pcmcia_socket[sock].k_state.detect ? "SS_DETECT " : "",
		     pcmcia_socket[sock].k_state.ready ? "SS_READY " : "",
		     pcmcia_socket[sock].cs_state.Vcc ? "SS_POWERON " : "",
		     pcmcia_socket[sock].cs_state.flags & SS_IOCARD ? "SS_IOCARD " : "",
		     (pcmcia_socket[sock].cs_state.flags & SS_IOCARD &&
		      pcmcia_socket[sock].k_state.bvd1) ? "SS_STSCHG " : "",
		     ((pcmcia_socket[sock].cs_state.flags & SS_IOCARD) == 0 &&
		      (pcmcia_socket[sock].k_state.bvd1 == 0)) ? "SS_BATDEAD " : "",
		     ((pcmcia_socket[sock].cs_state.flags & SS_IOCARD) == 0 &&
		      (pcmcia_socket[sock].k_state.bvd2 == 0)) ? "SS_BATWARN " : "",
		     pcmcia_socket[sock].k_state.vs_3v ? "SS_3VCARD " : "",
		     pcmcia_socket[sock].k_state.vs_Xv ? "SS_XVCARD " : "");

	p += sprintf(p, "mask     : %s%s%s%s%s\n",
		     pcmcia_socket[sock].cs_state.csc_mask & SS_DETECT ? "SS_DETECT " : "",
		     pcmcia_socket[sock].cs_state.csc_mask & SS_READY ? "SS_READY " : "",
		     pcmcia_socket[sock].cs_state.csc_mask & SS_BATDEAD ? "SS_BATDEAD " : "",
		     pcmcia_socket[sock].cs_state.csc_mask & SS_BATWARN ? "SS_BATWARN " : "",
		     pcmcia_socket[sock].cs_state.csc_mask & SS_STSCHG ? "SS_STSCHG " : "");

	p += sprintf(p, "cs_flags : %s%s%s%s%s\n",
		     pcmcia_socket[sock].cs_state.flags & SS_PWR_AUTO ?
			"SS_PWR_AUTO " : "",
		     pcmcia_socket[sock].cs_state.flags & SS_IOCARD ?
			"SS_IOCARD " : "",
		     pcmcia_socket[sock].cs_state.flags & SS_RESET ?
			"SS_RESET " : "",
		     pcmcia_socket[sock].cs_state.flags & SS_SPKR_ENA ?
			"SS_SPKR_ENA " : "",
		     pcmcia_socket[sock].cs_state.flags & SS_OUTPUT_ENA ?
			"SS_OUTPUT_ENA " : "");

	p += sprintf(p, "Vcc      : %d\n", pcmcia_socket[sock].cs_state.Vcc);
	p += sprintf(p, "Vpp      : %d\n", pcmcia_socket[sock].cs_state.Vpp);
	p += sprintf(p, "irq      : %d\n", pcmcia_socket[sock].cs_state.io_irq);
	p += sprintf(p, "I/O      : %u\n", pcmcia_socket[sock].speed_io);
	p += sprintf(p, "attribute: %u\n", pcmcia_socket[sock].speed_attr);
	p += sprintf(p, "common   : %u\n", pcmcia_socket[sock].speed_mem);
	return p-buf;
}


#endif  /* defined(CONFIG_PROC_FS) */
